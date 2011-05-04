/*===========================================================================
                              OEMWSMS.c

  OEM REFERENCE SMS SUPPORT ROUTINES
  
  This file contains a reference implementation of SMS support.
  This file interfaces with WMS services of DMSS.
    
        Copyright (c) 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            Qualcomm Confidential and Proprietary
============================================================================*/
#include "OEMFeatures.h"

#ifdef FEATURE_WMS

#include "clk.h"

#include "OEMHeap.h"

//Not easy to work around the warnings in these macros without redoing data structure
//lint -emacro({661}, MSGOPT_GETUDHBUFFERDATA)
//lint -emacro({662}, MSGOPT_GETUDHBUFFERDATA)
//lint -emacro({661}, MSGOPT_GETUDHBUFFERLENGTH)
//lint -emacro({662}, MSGOPT_GETUDHBUFFERLENGTH)
//lint -emacro((661), MSGOPT_GETUDHBUFFERDATA)
//lint -emacro((662), MSGOPT_GETUDHBUFFERDATA)
//lint -emacro((661), MSGOPT_GETUDHBUFFERLENGTH)
//lint -emacro((662), MSGOPT_GETUDHBUFFERLENGTH)
#include "OEMSMS_priv.h"
#include "OEMSMS.h"
#include "OEMSMSStorage.h"
#include "OEMTAPI.h"
#include "OEMConfig.h"
#include "msm/OEMPhone.h"
#include "AEEComdef.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEE_OEMDispatch.h"
#include "AEESMS.h"
#include "AEETelephone.h"
#include "AEE_OEMSMS.h"

#ifdef FEATURE_UIONE_HDK
/* used for CFGI_MSG_DISABLE_DAEMONS */
#include "CustomConfigItems.h"
#endif /* FEATURE_UIONE_HDK */

#ifndef AEE_SIMULATOR
#include "AEE_OEM.h"
#endif

#define OEMSMS_STORAGE_LAST   9

#ifdef CFGI_MSG_DISABLE_DAEMONS
#define CFGI_ISMS_NOT_PRIMARY_CLIENT CFGI_MSG_DISABLE_DAEMONS
#endif

// In CDMA, each message can either be sent on access or traffic channel
// This state captures that information for each message
#define OEMMSG_STATE_SEND_ON_TRAFFIC    1

struct _OEMMsg
{
   OEMMsg                     *pPrev;        // Don't change the order of this member
   OEMMsg                     *pNext;        // Don't change the order of this member
   AEECallback                *pcb;          // Don't change the order of this member
   uint32                     *pdwStatus;    // Don't change the order of this member
   wms_client_message_s_type  CMsg;
   uint32                     *pnIndex;
   uint32                     dwTag;
   OEMMsgData                 *pMsgData;
   OEMMsgPayload              *pMsgPayload;
   int                         nState;
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
   AEESMSBCSrvID       *psid;        // To Store the Service IDs got from WMS
   OEMSMSBCSrvInfo     *pSrvInfo;    // To Store the Service Info got from WMS
   boolean              bSrvInfoAlloc;
   AEESMSBCConfig       nConfig;     // Cache configuration - Notify on change
   AEESMSBCPref         nPref;       // Cache preference - Notify on change 
};

typedef struct _OEMReadMsg OEMReadMsg;
struct _OEMReadMsg
{
   OEMReadMsg                 *pPrev;     // Don't change the order of this member
   OEMReadMsg                 *pNext;     // Don't change the order of this member
   AEECallback                *pcb;       // Don't change the order of this member
   uint32                     *pdwStatus; // Don't change the order of this member
   OEMMsgData                 *pMsgData;
   OEMMsgPayload              *pMsgPayload;
};

typedef struct _OEMDeleteMsg OEMDeleteMsg;
struct _OEMDeleteMsg
{
   OEMDeleteMsg               *pPrev;     // Don't change the order of this member
   OEMDeleteMsg               *pNext;     // Don't change the order of this member
   AEECallback                *pcb;       // Don't change the order of this member
   uint32                     *pdwStatus; // Don't change the order of this member
};

typedef OEMDeleteMsg OEMBaseMsg;
typedef OEMDeleteMsg OEMAutoStoreMsg;

typedef struct _OEMWms 
{
   wms_client_id_type   clientId;
   OEMMsg               *pCurMOSMS;
   OEMMsg               *pCurStoreSMS;
   OEMReadMsg           *pCurReadSMS;
   OEMDeleteMsg         *pCurDeleteSMS;
   OEMAutoStoreMsg      *pCurAutoStore;
   OEMBcCfg             sBcCfg;
   boolean              bTransmissionPending;
   boolean              bDedicatedChannel;
   boolean              bMOOnAccessChannel;     // Send MO SMS on access channel
   boolean              bMOOnTrafficChannel;    // Send MO SMS on traffic channel
   uint32               dwTextStatus;           // Client Status for text messages
   uint32               dwVMStatus;             // Client Status for voice mail messages
   uint32               dwPageStatus;           // Client Status for page messages
   uint32               dwEMSStatus;            // Client Status for EMS messages
   uint32               dwWAPStatus;            // Client Status for WAP messages
   uint32               dwAnyStatus;            // Client Status for Any type of messages
   uint32               dwBREWDirectedTS;       // Teleservice ID for BREW directed messages
   AEECallback          cbOnStorageEvent[OEMSMS_STORAGE_LAST];    // Callback for handling storage initialization
   boolean              bMemStaInit[OEMSMS_STORAGE_LAST];         // Memory status initialization Status for various storage type
   boolean              bStrInit[OEMSMS_STORAGE_LAST];            // Initilization Status for various storage type
   boolean              bStrFull[OEMSMS_STORAGE_LAST];            // Full Status for various storage type
   uint32               ucStrSMSCount[OEMSMS_STORAGE_LAST];       // SMS count for various storage type
   int                  strSMSMsgList[OEMSMS_STORAGE_LAST][WMS_MESSAGE_LIST_MAX];  // SMS list for various storage type
   uint32               nCount;                 // To Store the count of Service IDs got from WMS
   AEESMSBCSrvID        *psid;                  // To Store the Service IDs got from WMS
   OEMSMSBCSrvInfo      *pSrvInfo;              // To Store the Service Info got from WMS
   wms_gw_domain_pref_e_type gwDomainPref;      // Domain preference for GW messages
   PFNNOTIFY            pfnNotifyOnStorageUpdate;    // Callback for OEMSMS_OnStorageUpdate()
   OEMStorageUpdateData sUpdateData;
   AEECallback          *pcbOnSMSUpdate;        // Callback for OEMSMS_OnSMSUpdate()
   AEECallback          *pcbOnSMSBCConfigUpdate;// Callback for OEMSMSBCConfig_OnSMSBCConfigUpdate()
   wms_routes_s_type    routes;                 // Routes Cache
   boolean              bRoutesInitialized;     // Routes Cache Initialized
   int                  nIsTLAckRequested;      // Value to cache TL Ack Requested
   AEECallback          pcbOnLoopbackMsg;         
   uint32               nLoopbackCount;
} OEMWms;

OEMWms * gpWms = 0;

//Language Mapping between WMS and BREW
typedef struct
{
   wms_language_e_type              OEM_Lang;
   uint32                           AEE_Lang;
} LangMap;

static const LangMap      gsLangMap[]  = {{WMS_LANGUAGE_UNSPECIFIED,    0},
                                         {WMS_LANGUAGE_ENGLISH,         LNG_ENGLISH},
                                         {WMS_LANGUAGE_FRENCH,          LNG_FRENCH},
                                         {WMS_LANGUAGE_SPANISH,         LNG_SPANISH},
                                         {WMS_LANGUAGE_JAPANESE,        LNG_JAPANESE},
                                         {WMS_LANGUAGE_KOREAN,          LNG_KOREAN},
                                         {WMS_LANGUAGE_CHINESE,         LNG_CHINESE},
                                         {WMS_LANGUAGE_HEBREW,          LNG_HEBREW}};

//Language Mapping between WMS and BREW
typedef struct
{
   wms_gw_cb_language_e_type        OEM_Lang;
   uint32                           AEE_Lang;
} GWCBLangMap;

static const GWCBLangMap gsGWCBLangMap[] = {{WMS_GW_CB_LANGUAGE_UNSPECIFIED,    0},
                                            {WMS_GW_CB_LANGUAGE_GERMAN,         LNG_GERMAN},
                                            {WMS_GW_CB_LANGUAGE_ENGLISH,        LNG_ENGLISH},
                                            {WMS_GW_CB_LANGUAGE_ITALIAN,        LNG_ITALIAN},
                                            {WMS_GW_CB_LANGUAGE_FRENCH,         LNG_FRENCH},
                                            {WMS_GW_CB_LANGUAGE_SPANISH,        LNG_SPANISH},
                                            {WMS_GW_CB_LANGUAGE_DUTCH,          LNG_DUTCH},
                                            {WMS_GW_CB_LANGUAGE_SWEDISH,        LNG_SWEDISH},
                                            {WMS_GW_CB_LANGUAGE_DANISH,         LNG_DANISH},
                                            {WMS_GW_CB_LANGUAGE_PORTUGUESE,     LNG_PORTUGUESE},
                                            {WMS_GW_CB_LANGUAGE_FINNISH,        LNG_FINNISH},
                                            {WMS_GW_CB_LANGUAGE_NORWEGIAN,      LNG_NORWEGIAN},
                                            {WMS_GW_CB_LANGUAGE_GREEK,          LNG_GREEK},
                                            {WMS_GW_CB_LANGUAGE_TURKISH,        LNG_TURKISH},
                                            {WMS_GW_CB_LANGUAGE_HUNGARIAN,      LNG_HUNGARIAN},
                                            {WMS_GW_CB_LANGUAGE_POLISH,         LNG_POLISH},
                                            {WMS_GW_CB_LANGUAGE_CZECH,          LNG_CZECH}};

// Teleservice Mapping Between WMS and OEM layer SMS teleservice defines
typedef struct
{
   int                        OEM_TsId;
   wms_teleservice_e_type     DMSS_TsId;
} TsIdMap;

static const TsIdMap   gsTsIdMap[] = 
                  {{SMS_TELESERVICE_CPT_95,              WMS_TELESERVICE_CPT_95 },
                  {SMS_TELESERVICE_CMT_95,               WMS_TELESERVICE_CMT_95 },
                  {SMS_TELESERVICE_VMN_95,               WMS_TELESERVICE_VMN_95 },
                  {SMS_TELESERVICE_IS91_PAGE,            WMS_TELESERVICE_IS91_PAGE },
                  {SMS_TELESERVICE_IS91_VOICE_MAIL,      WMS_TELESERVICE_IS91_VOICE_MAIL},
                  {SMS_TELESERVICE_IS91_SHORT_MESSAGE,   WMS_TELESERVICE_IS91_SHORT_MESSAGE},
                  {SMS_TELESERVICE_MWI,                  WMS_TELESERVICE_MWI},
                  {SMS_TELESERVICE_WAP,                  WMS_TELESERVICE_WAP},
#ifdef CUST_EDITION	  				  
#if defined(FEATURE_QMA)
                  {SMS_TELESERVICE_QMA_WPUSH,            WMS_TELESERVICE_QMA_WPUSH},
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
                  {SMS_TELESERVICE_WPUSH,                WMS_TELESERVICE_WPUSH},
#endif            
#endif /*CUST_EDITION*/
                  {SMS_TELESERVICE_WEMT,                 WMS_TELESERVICE_WEMT},
                  {SMS_TELESERVICE_BROADCAST,            WMS_TELESERVICE_BROADCAST}};

//Priority Mapping between DMSS and BREW
typedef struct
{
   int                     AEE_Priority;
   wms_priority_e_type     OEM_Priority;
} PriorityMap;

static const PriorityMap   gsPriorityMap[] = 
                  {{SMS_PRIORITY_NORMAL,      WMS_PRIORITY_NORMAL},
                  {SMS_PRIORITY_INTERACTIVE,  WMS_PRIORITY_INTERACTIVE},
                  {SMS_PRIORITY_URGENT,       WMS_PRIORITY_URGENT},
                  {SMS_PRIORITY_EMERGENCY,    WMS_PRIORITY_EMERGENCY }};

//Privacy Mapping between DMSS and BREW
typedef struct
{
   int                     AEE_Privacy;
   wms_privacy_e_type      OEM_Privacy;
} PrivacyMap;


static const PrivacyMap   gsPrivacyMap[] = 
                  {{SMS_PRIVACY_NORMAL,         WMS_PRIVACY_NORMAL},
                  {SMS_PRIVACY_RESTRICTED,      WMS_PRIVACY_RESTRICTED},
                  {SMS_PRIVACY_CONFIDENTIAL,    WMS_PRIVACY_CONFIDENTIAL},
                  {SMS_PRIVACY_SECRET,          WMS_PRIVACY_SECRET}};

//Encoding Mapping between DMSS and OEM layer encoding
typedef struct
{
   int                                 AEE_Enc;
   wms_user_data_encoding_e_type       OEM_Enc;
} EncMap;

static const EncMap   gsEncMap[] =  
                  {{AEESMS_ENC_OCTET,            WMS_ENCODING_OCTET},
                  {AEESMS_ENC_IS91EP,            WMS_ENCODING_IS91EP},
                  {AEESMS_ENC_ASCII,             WMS_ENCODING_ASCII},
                  {AEESMS_ENC_LATIN,             WMS_ENCODING_LATIN},
                  {AEESMS_ENC_IA5,               WMS_ENCODING_IA5},
                  {AEESMS_ENC_UNICODE,           WMS_ENCODING_UNICODE},
                  {AEESMS_ENC_SHIFT_JIS,         WMS_ENCODING_SHIFT_JIS},
                  {AEESMS_ENC_KOREAN,            WMS_ENCODING_KOREAN},
                  {AEESMS_ENC_LATIN_HEBREW,      WMS_ENCODING_LATIN_HEBREW},
                  {AEESMS_ENC_GSM_7_BIT_DEFAULT, WMS_ENCODING_GSM_7_BIT_DEFAULT}};

//Tag Mapping between DMSS and AEE layer tags
typedef struct
{
   int                                 AEE_Tag;
   wms_message_tag_e_type              OEM_Tag;
} TagMap;

static const TagMap   gsTagMap[] =  
                  {{AEESMS_TAG_NONE,                     WMS_TAG_NONE},
                  {AEESMS_TAG_MT_READ,                   WMS_TAG_MT_READ},
                  {AEESMS_TAG_MT_NOT_READ,               WMS_TAG_MT_NOT_READ},
                  {AEESMS_TAG_MO_SENT,                   WMS_TAG_MO_SENT},
                  {AEESMS_TAG_MO_NOT_SENT,               WMS_TAG_MO_NOT_SENT},
                  {AEESMS_TAG_MO_SENT_ST_NOT_RECEIVED,   WMS_TAG_MO_SENT_ST_NOT_RECEIVED},
                  {AEESMS_TAG_MO_SENT_ST_NOT_STORED,     WMS_TAG_MO_SENT_ST_NOT_STORED},
                  {AEESMS_TAG_MO_SENT_ST_STORED,         WMS_TAG_MO_SENT_ST_STORED},
                  {AEESMS_TAG_MO_TEMPLATE,               WMS_TAG_MO_TEMPLATE},
                  {AEESMS_TAG_STATUS_RPT,                WMS_TAG_STATUS_RPT}};


//Tag Mapping between DMSS and AEE layer Client Status and CDMA Cause Code
typedef struct
{
   int                       AEE_ClientStatus;
   wms_cdma_tl_status_e_type OEM_ClientStatus;
} CDMAClientStatusMap;

static const CDMAClientStatusMap gsCDMAClientStatusMap[] =
                  {{AEESMS_CS_BUSY,              WMS_TL_OTHER_TERMINAL_PROBLEM_S},
                  {AEESMS_CS_BLOCK,              WMS_TL_SMS_TERMINATION_DENIED_S},
                  {AEESMS_CS_OUT_OF_RESOURCES,   WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S}};

//Tag Mapping between DMSS and AEE layer Client Status and GW Cause Code
typedef struct
{
   int                       AEE_ClientStatus;
   wms_tp_cause_e_type      OEM_ClientStatus;
} GWClientStatusMap;

static const GWClientStatusMap gsGWClientStatusMap[] =
                  {{AEESMS_CS_BUSY,              WMS_TP_CAUSE_UNSPECIFIED_ERROR},
                  {AEESMS_CS_BLOCK,              WMS_TP_CAUSE_UNSPECIFIED_ERROR},
                  {AEESMS_CS_OUT_OF_RESOURCES,   WMS_TP_CAUSE_MEMORY_FULL}};

//Tag Mapping between DMSS and AEE layer storage type
typedef struct
{
   int                                 AEE_StorageType;
   wms_memory_store_e_type             OEM_StorageType;
   wms_message_tag_e_type              OEM_Tag;
} StorageTypeMap;

static const StorageTypeMap   gsStorageTypeMap[] =
                  {{AEESMS_CARD_RUIM,                    WMS_MEMORY_STORE_RUIM,     WMS_TAG_NONE},
                   {AEESMS_CARD_SIM,                     WMS_MEMORY_STORE_SIM,      WMS_TAG_NONE},
                   {AEESMS_NV_CDMA,                      WMS_MEMORY_STORE_NV_CDMA,  WMS_TAG_NONE},
                   {AEESMS_NV_GW,                        WMS_MEMORY_STORE_NV_GW,    WMS_TAG_NONE},
                   {AEESMS_CARD_RUIM_TEMPLATE,           WMS_MEMORY_STORE_RUIM,     WMS_TAG_MO_TEMPLATE},
                   {AEESMS_CARD_SIM_TEMPLATE,            WMS_MEMORY_STORE_SIM,      WMS_TAG_MO_TEMPLATE},
                   {AEESMS_NV_CDMA_TEMPLATE,             WMS_MEMORY_STORE_NV_CDMA,  WMS_TAG_MO_TEMPLATE},
                   {AEESMS_NV_GW_TEMPLATE,               WMS_MEMORY_STORE_NV_GW,    WMS_TAG_MO_TEMPLATE},
                   {AEESMS_CARD_SIM_STATUS_REPORT,       WMS_MEMORY_STORE_SIM,      WMS_TAG_STATUS_RPT}};

//Tag Mapping between DMSS and AEE layer UDH IDs
typedef struct
{
   AEESMSUDHId                         AEE_UDHID;
   wms_udh_id_e_type                   DMSS_UDHID;
} UDHIDMap;

static const UDHIDMap   gsUDHIDMap[] =  
                  {{AEESMS_UDH_CONCAT_8,        WMS_UDH_CONCAT_8},
                  {AEESMS_UDH_SPECIAL_SM,       WMS_UDH_SPECIAL_SM},
                  {AEESMS_UDH_PORT_8,           WMS_UDH_PORT_8},
                  {AEESMS_UDH_PORT_16,          WMS_UDH_PORT_16},
                  {AEESMS_UDH_SMSC_CONTROL,     WMS_UDH_SMSC_CONTROL},
                  {AEESMS_UDH_SOURCE,           WMS_UDH_SOURCE},
                  {AEESMS_UDH_CONCAT_16,        WMS_UDH_CONCAT_16},
                  {AEESMS_UDH_WCMP,             WMS_UDH_WCMP},
                  {AEESMS_UDH_TEXT_FORMATING,   WMS_UDH_TEXT_FORMATING},
                  {AEESMS_UDH_PRE_DEF_SOUND,    WMS_UDH_PRE_DEF_SOUND},
                  {AEESMS_UDH_USER_DEF_SOUND,   WMS_UDH_USER_DEF_SOUND},
                  {AEESMS_UDH_PRE_DEF_ANIM,     WMS_UDH_PRE_DEF_ANIM},
                  {AEESMS_UDH_LARGE_ANIM,       WMS_UDH_LARGE_ANIM},
                  {AEESMS_UDH_SMALL_ANIM,       WMS_UDH_SMALL_ANIM},
                  {AEESMS_UDH_LARGE_PICTURE,    WMS_UDH_LARGE_PICTURE},
                  {AEESMS_UDH_SMALL_PICTURE,    WMS_UDH_SMALL_PICTURE},
                  {AEESMS_UDH_VAR_PICTURE,      WMS_UDH_VAR_PICTURE},
                  {AEESMS_UDH_RFC822,           WMS_UDH_RFC822}};

//Tag Mapping between DMSS and AEE layer message wait types
typedef struct
{
   AEESMSMsgWaitingType                AEE_MsgWaitType;
   wms_gw_msg_waiting_e_type           DMSS_MsgWaitType;
} MsgWaitTypeMap;

static const MsgWaitTypeMap gsMsgWaitTypeMap[] = 
                 {{AEESMS_MSGWAITINGTYPE_NONE,        WMS_GW_MSG_WAITING_NONE}, 
                  {AEESMS_MSGWAITINGTYPE_DISCARD,     WMS_GW_MSG_WAITING_DISCARD},
                  {AEESMS_MSGWAITINGTYPE_STORE,       WMS_GW_MSG_WAITING_STORE},
                  {AEESMS_MSGWAITINGTYPE_NONE_1111,   WMS_GW_MSG_WAITING_NONE_1111}};

//Tag Mapping between DMSS and AEE layer message wait kinds
typedef struct
{
   AEESMSMsgWaitingKind                AEE_MsgWaitKind;
   wms_gw_msg_waiting_kind_e_type      DMSS_MsgWaitKind;
} MsgWaitKindMap;

static const MsgWaitKindMap gsMsgWaitKindMap[] = 
                 {{AEESMS_MSGWAITINGKIND_VOICEMAIL,   WMS_GW_MSG_WAITING_VOICEMAIL}, 
                  {AEESMS_MSGWAITINGKIND_FAX,         WMS_GW_MSG_WAITING_FAX},
                  {AEESMS_MSGWAITINGKIND_EMAIL,       WMS_GW_MSG_WAITING_EMAIL},
                  {AEESMS_MSGWAITINGKIND_OTHER,       WMS_GW_MSG_WAITING_OTHER}};

//GW Domain Preference Mapping between DMSS and AEE layer
typedef struct
{
   int AEE_DomainPref;
   wms_gw_domain_pref_e_type DMSS_DomainPref;
} GWDomainPrefMap;

static const GWDomainPrefMap gsGWDomainPrefMap[] = 
                 {{AEESMS_GW_DOMAIN_PREF_CS_PREFERRED,   WMS_GW_DOMAIN_PREF_CS_PREFERRED}, 
                  {AEESMS_GW_DOMAIN_PREF_PS_PREFERRED,   WMS_GW_DOMAIN_PREF_PS_PREFERRED},
                  {AEESMS_GW_DOMAIN_PREF_CS_ONLY,        WMS_GW_DOMAIN_PREF_CS_ONLY},
                  {AEESMS_GW_DOMAIN_PREF_PS_ONLY,        WMS_GW_DOMAIN_PREF_PS_ONLY}};

/*===========================================================================

  Prototypes

===========================================================================*/
void OEMWMS_MessageListener
(
   wms_msg_event_e_type event,
   wms_msg_event_info_s_type * info_ptr,
   boolean * pshared 
);

static wms_status_e_type OEMWMS_MoMessageListener
(
   wms_msg_event_e_type event,
   const wms_msg_event_info_s_type * info_ptr,
   boolean * pshared 
);

static void OEMWSMS_CommandCallback 
(
   wms_cmd_id_e_type cmd, 
   void * user_data, 
   wms_cmd_err_e_type cmd_err 
);

static uint32 OEMWMS_GetSubmitStatus(const wms_submit_report_info_s_type *submit_info);

#ifdef FEATURE_CDSMS
static void OEMWMS_EventListener
(
   wms_dc_event_e_type event, 
   wms_dc_event_info_s_type * info_ptr
);
#endif /*FEATURE_CDSMS*/

void OEMWMS_CfgListener
(
   wms_cfg_event_e_type event,
   wms_cfg_event_info_s_type * info_ptr
);

static void OEMWMS_PreProcessor
(
   const wms_client_message_s_type * pCMsg, 
   boolean * pshared 
);

boolean OEMWMS_CopyMessage
(
   wms_transaction_id_type transaction_id, 
   wms_client_message_s_type * pCMsg,
   SMSCacheEntry *pce 
);

void OEMWMS_GetAutoStoreInformation( wms_client_message_s_type * pCMsg, SMSCacheEntry *pce );

static boolean OEMWMS_GetNumMsgsInfoFromCDMABDMsg 
(
   wms_client_bd_s_type * pcd,
   wms_teleservice_e_type ts,
   int * pnNumMsgs
);

static boolean OEMWMS_GetNumMsgsInfoFromGWMsg 
(
   wms_gw_deliver_s_type * pData,
   wms_teleservice_e_type ts,
   int * pnNumMsgs
);

static void OEMWMS_SetNumMsgsInfoToGWMsg 
(
   wms_gw_deliver_s_type * pData,
   wms_teleservice_e_type ts,
   int nNumMsgs
);

static boolean OEMWMS_GetCallBackNumberFromCDMABDMsg
(
   wms_client_bd_s_type *pcd,
   wms_teleservice_e_type ts,
   char * pBuffer,
   int nSize, 
   AEESMSNumberType * pNumberType,
   AEESMSNumberPlan * pNumberPlan
);

static boolean OEMWMS_GetAddress
(
   wms_address_s_type * pAddr,
   char * pBuffer,
   int nSize,
   AEESMSNumberType * pNumberType,
   AEESMSNumberPlan * pNumberPlan,
   boolean bCDMA
);

static wms_teleservice_e_type OEMWMS_DetermineGSMMessageType
(
   wms_gw_pp_ts_data_s_type *pgw_pp
);

static boolean OEMWMS_ConstructClientBD 
(
   OEMMsgData * pMsgData,
   OEMMsgPayload * pMsgPayload,
   wms_client_ts_data_s_type * pClientTS,
   sys_sys_mode_e_type sysMode
);

boolean OEMWMS_SetAddress
(
   wms_address_s_type * pAddr,
   char * pBuffer,
   AEESMSNumberType dwNumberType,
   AEESMSNumberPlan dwNumberPlan,
   boolean bCDMA
);

static boolean OEMWMS_ConstructClientMessage 
(
   OEMMsgData * pMsgData,
   wms_client_message_s_type * pMsg,
   wms_raw_ts_data_s_type * pRaw,
   sys_sys_mode_e_type sysMode
);
 
static void OEMWMS_ConstructCDMATemplate
(
   OEMMsgData * pMsgData, 
   wms_client_message_s_type * pMsg, 
   wms_client_bd_s_type * pClientBD
);

static void OEMWMS_ConstructGWTemplate
(
   OEMMsgData * pMsgData, 
   OEMMsgPayload * pMsgPayload, 
   wms_client_message_s_type * pMsg
);

uint32 OEMWMS_GetTimeFromWmsTimestamp(wms_timestamp_s_type *pTime, int *ptz, boolean bRelative);
 
static void OEMWMS_SetTimeToWmsTimestamp(wms_timestamp_s_type *pTime, uint32 ts, int tz, boolean bRelative);

static void OEMWMS_AckCDMAMessage
(
   wms_transaction_id_type tid,
   wms_error_class_e_type eErr,
   wms_cdma_tl_status_e_type eStatus
);

static void OEMWMS_AckGSMMessage
(
   wms_transaction_id_type tid,
   wms_gw_tpdu_type_e_type tpdu_type,
   wms_gw_deliver_report_ack_s_type * pAckInfo
);

static void OEMWMS_NackGSMMessage
(
   wms_transaction_id_type tid,
   wms_gw_tpdu_type_e_type tpdu_type,
   wms_gw_deliver_report_error_s_type * pError
);

static void OEMWMS_StoreSMSListener
(
   wms_msg_event_info_s_type * info_ptr
);

static void OEMWMS_DeleteSMSListener
(
   wms_msg_event_e_type event, 
   wms_msg_event_info_s_type * info_ptr
);

static void OEMWMS_ReadSMSListener
(
   wms_msg_event_info_s_type * info_ptr
);

static void OEMWMS_GetMemoryStatus(void* pUser);

static void OEMWMS_GetMessageList(void* pUser);

static void OEMWMS_ReleaseStoreSMS(AEECallback *pcb);

static void OEMWMS_StoreSMS(OEMMsg* hMsg, AEECallback *pcb, uint32 *pdwStatus);

static void OEMWMS_DeleteStoreSMS(OEMMsg * hMsg);

static void OEMWMS_ReleaseReadSMS(AEECallback *pcb);

static void OEMWMS_ReadSMS(OEMReadMsg* hMsg, AEECallback *pcb, uint32 *pdwStatus, OEMMsgData *pMsgData, OEMMsgPayload *pMsgPayload, uint32 nIndex, int st);

static void OEMWMS_DeleteReadSMS(OEMReadMsg * hMsg);

static void OEMWMS_ReleaseDeleteSMS(AEECallback *pcb);

static void OEMWMS_DeleteSMS(OEMDeleteMsg* hMsg, AEECallback *pcb, uint32 *pdwStatus, uint32 nIndex, AEESMSStorageType st, AEESMSTag mt);

static void OEMWMS_DeleteDeleteSMS(OEMDeleteMsg * hMsg);

static void OEMWMS_ReleaseAutoStoreMsg(AEECallback *pcb);

static void OEMWMS_AutoStoreMsg(OEMAutoStoreMsg* hMsg, AEECallback *pcb, uint32 * pdwStatus, uint32 mc, wms_memory_store_e_type wmsMemoryStore);

static void OEMWMS_DeleteAutoStoreMsg(OEMAutoStoreMsg * hMsg);

static wms_status_e_type OEMSMS_GetMsgAckStatus(uint32        ts, 
                                  wms_message_mode_e_type     mode, 
                                  wms_cdma_tl_status_e_type*  eCDMAStatus, 
                                  wms_tp_cause_e_type*        eGWStatus);

static wms_status_e_type OEMWMS_SendSMSOnTrafficChannel(OEMMsg * hMsg);

static int OEMWMS_GetUDHSize(int nNumHeaders, wms_udh_s_type udh[]);

static void OEMWMS_GetUDHData(int nNumHeaders, wms_udh_s_type udh[], byte * pBuffer);


// ----------------------------------------------------
// A Brief overview of FEATURE_SMS_IGNORE_EMS_5_CONTENT
// ----------------------------------------------------
// EMS 5 Content refers to extra UDH/EMS headers introduced in 
// 3GPP TS 23.040 Release 5.  The underlying AMSS layer supports
// User Prompt Indicator and Extended Objects (for vCard/vCal etc).
// 
// Prior to this release, this data was encoded using MSGOPT_UDH_OTHER
// To keep backward compatibility, code under FEATURE_SMS_IGNORE_EMS_5_CONTENT
// exists in the porting layer, specifically to account for the following
// new structures introduced in wms.h
//
// -- wms_udh_user_prompt_s_type
// -- wms_udh_eo_s_type
// 
// Without this code, the transformation to wms structures breaks
// apps that use and expect data in MSGOPT_UDH_OTHER content which
// uses the wms_udh_other_s_type structure.

#ifndef FEATURE_SMS_IGNORE_EMS_5_CONTENT
static boolean OEMWMS_UDHEncodeIsFirstSegment (int nMaxHeaders, wms_udh_s_type udh[], byte * pBytes, int nBytes);
#endif // FEATURE_SMS_IGNORE_EMS_5_CONTENT

static int OEMWMS_SetUDHData(int nMaxHeaders, wms_udh_s_type udh[], byte * pBuffer, int nBytes);

#ifndef FEATURE_WMS_APP
static void OEMWMS_GetMsgTransferRoute(void);
#endif
#ifdef FEATURE_NOT_BREW_ONLY_SMS
static void OEMWMS_SetPrimaryClient(void);
#endif // #ifdef FEATURE_NOT_BREW_ONLY_SMS

static void OEMWMS_SetMemoryFull(boolean bFull);

static void OEMWMS_GetDomainPref(void);

static void OEMWSMS_NoopCommandCallback 
(
   wms_cmd_id_e_type cmd, 
   void * user_data, 
   wms_cmd_err_e_type cmd_err 
);

static void OEMWMS_NotifyMemoryEvent
(
   AEESMSStorageType st,
   uint32 nEvent
);

static void OEMWSMS_LoopbackCb (void* pUser);

static uint32 Xlate_TS(wms_teleservice_e_type ts);
static uint32 Xlate_Lang(wms_language_e_type lang);
static uint32 Xlate_Tag(wms_message_tag_e_type tag);
static int Xlate_Priority(wms_priority_e_type priority);
static int Xlate_Privacy(wms_privacy_e_type privacy);
static int Xlate_Encoding(wms_user_data_encoding_e_type enc);
static int Xlate_StorageType(wms_memory_store_e_type st, wms_message_tag_e_type tag);
static int Xlate_DomainPref(wms_gw_domain_pref_e_type dp);
static uint32 Xlate_GWCBLang(wms_gw_cb_language_e_type lang);

static int Xlate_wms_status_e_type_2_MOSMS_ERR(wms_status_e_type e);
static int Xlate_wms_cdma_tl_status_e_type_2_MOSMS_ERR(wms_cdma_tl_status_e_type e);
static int Xlate_wms_submit_report_info_2_MOSMS_ERR(const wms_submit_report_info_s_type *submit_info);
static int Xlate_wms_tp_cause_e_type_2_MOSMS_ERR(wms_tp_cause_e_type e);
static int Xlate_wms_rp_cause_e_type_2_MOSMS_ERR(wms_rp_cause_e_type e);
static int Xlate_wms_error_class_e_type_2_MOSMS_ERRTYPE(wms_error_class_e_type e);
static int Xlate_wms_report_status_and_rp_cause_e_type_2_MOSMS_ERRTYPE(wms_report_status_e_type e, wms_rp_cause_e_type eRpCause);
static AEESMSMsgWaitingType Xlate_MsgWaitingType(wms_gw_msg_waiting_e_type dt);
static AEESMSMsgWaitingKind Xlate_MsgWaitingKind(wms_gw_msg_waiting_kind_e_type dk);

static wms_teleservice_e_type Xlate_2_NativeTS(uint32 ts);
static wms_user_data_encoding_e_type Xlate_2_NativeEncoding(int enc);
static wms_priority_e_type Xlate_2_NativePriority(int priority);
static wms_privacy_e_type Xlate_2_NativePrivacy(int privacy);
static wms_language_e_type Xlate_2_NativeLang(int lang);
static wms_message_tag_e_type Xlate_2_NativeTag(int tag);
static wms_memory_store_e_type Xlate_2_NativeStorageType(int st);
static wms_message_tag_e_type Xlate_2_NativeTagFromStorageType(int st);
static wms_udh_id_e_type Xlate_2_NativeUDHId(AEESMSUDHId id);
static wms_gw_msg_waiting_e_type Xlate_2_NativeMsgWaitingType(AEESMSMsgWaitingType at);
static wms_gw_msg_waiting_kind_e_type Xlate_2_NativeMsgWaitingKind(AEESMSMsgWaitingKind ak);
static wms_gw_domain_pref_e_type Xlate_2_NativeDomainPref(int dp);
static int Xlate_wms_cmd_err_2_AEEErrorCode(wms_cmd_err_e_type cmd_err);

static SMSCacheEntry sce;

#ifdef FEATURE_BREW_BROADCAST_SMS

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
static wms_message_mode_e_type Xlate_AEESMSMode_2_wms_message_mode(AEESMSMode mode);
static void OEMWSMSBCMMConfig_EventCallback(wms_bc_mm_event_e_type event, wms_bc_mm_event_info_s_type *info_ptr);
static boolean ConvertAEESMSBCSrvIDToWMSBCMMSrvID(AEESMSBCSrvID *pSrvID, wms_message_mode_e_type mode, wms_bc_mm_srv_id_type *service_id);
static boolean ConvertWMSBCMMSrvIDToAEESMSBCSrvID(wms_bc_mm_srv_id_type *service_id, wms_message_mode_e_type msg_mode, AEESMSBCSrvID *pSrvID);
static boolean ConvertOEMSMSBCSrvInfoToWMSBCMMSrvInfo(OEMSMSBCSrvInfo *pSrvInfo, wms_message_mode_e_type mode, wms_bc_mm_service_info_s_type *srv_info);
static boolean ConvertWMSBCMMSrvInfoToOEMSMSBCSrvInfo(wms_bc_mm_service_info_s_type *srv_info, wms_message_mode_e_type msg_mode, OEMSMSBCSrvInfo *pSrvInfo);
#endif // FEATURE_BROADCAST_SMS_MULTIMODE

static void OEMWSMSBCConfig_CommandCallback(wms_cmd_id_e_type cmd, void *user_data, wms_cmd_err_e_type cmd_err);

#if (defined(FEATURE_CDSMS) && !defined(FEATURE_CDSMS_BROADCAST))
static void OEMWSMSBCConfig_EventCallback(wms_bc_event_e_type event, wms_bc_event_info_s_type *info_ptr);
static boolean ConvertAEESMSBCSrvIDToWMSBCSrvID(AEESMSBCSrvID *pSrvID, wms_message_mode_e_type mode, wms_bc_service_id_s_type *service_id);
static boolean ConvertWMSBCSrvIDToAEESMSBCSrvID(wms_bc_service_id_s_type *service_id, wms_message_mode_e_type msg_mode, AEESMSBCSrvID *pSrvID);
static boolean ConvertOEMSMSBCSrvInfoToWMSBCSrvInfo(OEMSMSBCSrvInfo *pSrvInfo, wms_message_mode_e_type mode, wms_bc_service_info_s_type *srv_info);
static boolean ConvertWMSBCSrvInfoToOEMSMSBCSrvInfo(wms_bc_service_info_s_type *srv_info, wms_message_mode_e_type msg_mode, OEMSMSBCSrvInfo *pSrvInfo);
#endif // (defined(FEATURE_CDSMS) && !defined(FEATURE_CDSMS_BROADCAST))

#endif // FEATURE_BREW_BROADCAST_SMS

#define ONE_MINUTE_SECS          (60)
#define ONE_HOUR_SECS            (60*ONE_MINUTE_SECS)
#define ONE_DAY_SECS             (24*ONE_HOUR_SECS)
#define ONE_MONTH_SECS           (30*ONE_DAY_SECS)

/*===========================================================================

  Called as part of SMS initialization. Registers message, event and status
  listeners with SMS task

===========================================================================*/
int OEMSMS_Init(void)
{
   int nErr, i;
   wms_client_err_e_type eErr;

   FARF(SMS, ("==> OEMSMS_Init"));

   if (gpWms)
      return SUCCESS;

   if ((gpWms = sys_malloc(sizeof(OEMWms))) == NULL)
      return ENOMEMORY;

   gpWms->clientId = WMS_DUMMY_CLIENT_ID;
   
   if ((eErr = wms_client_init(WMS_CLIENT_TYPE_BREW, &gpWms->clientId)) != WMS_CLIENT_ERR_NONE)
   {
      FARF(SMS, ("wms_client_init err=%d", eErr));
      goto Done;
   }

   if ((eErr = wms_client_reg_msg_cb(gpWms->clientId, OEMWMS_MessageListener)) != WMS_CLIENT_ERR_NONE)
   {
      FARF(SMS, ("wms_client_reg_msg_cb err=%d",eErr));
      goto Done;
   }
   
   if ((eErr = wms_client_reg_cfg_cb(gpWms->clientId, OEMWMS_CfgListener)) != WMS_CLIENT_ERR_NONE)
   {
      FARF(SMS, ("wms_client_reg_cfg_cb err=%d",eErr));
      goto Done;
   }

#ifdef FEATURE_CDSMS
   if ((eErr = wms_client_reg_dc_cb(gpWms->clientId, OEMWMS_EventListener)) != WMS_CLIENT_ERR_NONE)
   {
      FARF(SMS, ("wms_client_reg_dc_cb err=%d", eErr));
      goto Done;
   }
#endif /*FEATURE_CDSMS*/

   if ((eErr = wms_client_reg_parse_msg_cb(gpWms->clientId, OEMWMS_PreProcessor)) != WMS_CLIENT_ERR_NONE)
   {
      FARF(SMS, ("wms_client_reg_parse_msg_cb err=%d",eErr));
      goto Done;
   }

#ifdef FEATURE_BREW_BROADCAST_SMS
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
   if ((eErr = wms_client_reg_bc_mm_cb(gpWms->clientId, OEMWSMSBCMMConfig_EventCallback)) != WMS_CLIENT_ERR_NONE)
   {
      FARF(SMS, ("wms_client_reg_bc_mm_cb err=%d",eErr));
      goto Done;
   }
#endif // FEATURE_BROADCAST_SMS_MULTIMODE
#if (defined(FEATURE_CDSMS) && !defined(FEATURE_CDSMS_BROADCAST))
   // This mean we use old API for CDMA BC
   if ((eErr = wms_client_reg_bc_cb(gpWms->clientId, OEMWSMSBCConfig_EventCallback)) != WMS_CLIENT_ERR_NONE)
   {
      FARF(SMS, ("wms_client_reg_bc_cb err=%d",eErr));
      goto Done;
   }
#endif // (defined(FEATURE_CDSMS) && !defined(FEATURE_CDSMS_BROADCAST))
#endif // FEATURE_BREW_BROADCAST_SMS 

   for(i = 0; i < OEMSMS_STORAGE_LAST; i++)
   {
      gpWms->bStrInit[i] = FALSE;
      gpWms->bMemStaInit[i] = FALSE;
      gpWms->ucStrSMSCount[i] = WMS_MESSAGE_LIST_MAX;
   }

   if ((eErr = wms_client_activate(gpWms->clientId)) != WMS_CLIENT_ERR_NONE)
   {
      FARF(SMS, ("wms_client_activate err=%d", eErr));
      goto Done;
   }

   if (OEM_GetConfig(CFGI_BREWDIR_SMS_TELESERVICE, (void*)&gpWms->dwBREWDirectedTS, sizeof(gpWms->dwBREWDirectedTS)) != SUCCESS)
      gpWms->dwBREWDirectedTS = 0;
   
   if (OEM_GetConfig(CFGI_SMS_MO_ON_ACCESS_CHANNEL, (void*)&gpWms->bMOOnAccessChannel, sizeof(gpWms->bMOOnAccessChannel)) != SUCCESS)
      gpWms->bMOOnAccessChannel = FALSE;   // Disable by default

   if (OEM_GetConfig(CFGI_SMS_MO_ON_TRAFFIC_CHANNEL, (void*)&gpWms->bMOOnTrafficChannel, sizeof(gpWms->bMOOnTrafficChannel)) != SUCCESS)
      gpWms->bMOOnTrafficChannel = TRUE;   // Enable by default

   gpWms->nIsTLAckRequested = -1; // Invalidated

Done:
   if (eErr != WMS_CLIENT_ERR_NONE)
   {
      nErr = EFAILED;
      OEMSMS_Cleanup();
   }
   else
      nErr = SUCCESS;

   return nErr;
}

static boolean OEMWMS_IsBREWDirectedSMS(char * pText, uint32 dwBytes)
{
#ifndef AEE_SIMULATOR
   if (OEMSMS_IgnorePrefixForBrewDirectedMsg()) {
      pText = MEMSTR((char *)pText,"//", dwBytes);
   }
#endif // AEE_SIMULATOR
   return AEE_IsBREWDirectedSMS(pText);
}

/*===========================================================================

  Called by SMS task to handle incoming SMS message.  Cache the message
  and notify the registered handler...

===========================================================================*/
void OEMWMS_MessageListener
(
   wms_msg_event_e_type event,
   wms_msg_event_info_s_type * info_ptr,
   boolean * pshared 
)
{
   FARF(SMS, ("==> OEMWMS_MessageListener"));

   if (pshared)
   {
      *pshared = TRUE;
   }

   switch (event)
   {
      case WMS_MSG_EVENT_RECEIVED_MESSAGE:
      case WMS_MSG_EVENT_STATUS_REPORT:
      {
         SMSCacheEntry * pce = NULL;
         wms_message_mode_e_type eMsgMode = info_ptr->mt_message_info.message.msg_hdr.message_mode;
         wms_transaction_id_type tid = info_ptr->mt_message_info.transaction_id;

         if (OEMSMS_IsInitialized() == FALSE) 
         {
            FARF(SMS, ("SMS object not present"));      
            FARF(SMS, ("Rejected SMS Msg (Id = %d)", tid));

#ifdef FEATURE_NOT_BREW_ONLY_SMS
            if (eMsgMode == WMS_MESSAGE_MODE_CDMA)
            {
               OEMWMS_AckCDMAMessage(tid, WMS_ERROR_TEMP, WMS_TL_DESTINATION_OUT_OF_SERVICE_S);
            }
            else if (eMsgMode == WMS_MESSAGE_MODE_GW)
            {
                wms_gw_deliver_report_error_s_type * error = NULL;
                error = sys_malloc(sizeof(wms_gw_deliver_report_error_s_type));

                if(error  == NULL){
                  FARF(SMS, ("==> sys_malloc failed on error"));
                  return; 
                }

                MEMSET(error, 0, sizeof(wms_gw_deliver_report_error_s_type));
                error->tp_cause = WMS_TP_CAUSE_UNSPECIFIED_ERROR;
                OEMWMS_NackGSMMessage(tid, WMS_TPDU_DELIVER_REPORT_ERROR, error);
                sys_free(error);
            }
#endif // #ifdef FEATURE_NOT_BREW_ONLY_SMS
            return;
         }

         FARF(SMS, ("Received SMS Msg (Id = %d)", tid));

         if (eMsgMode == WMS_MESSAGE_MODE_CDMA)
         {
            FARF(SMS, ("Message Teleservice Id = %x", info_ptr->mt_message_info.message.u.cdma_message.teleservice));
         }

         // Get a buffer to copy
         pce = OEMSMSCache_GetEntry();

         if (pce == NULL) 
         {
            // Ooops, we do not have any free Cache Entry's available!
            FARF(SMS, ("No Cache Entry available, SMS Msg (Id = %d) Discarded", tid));         

#ifdef FEATURE_NOT_BREW_ONLY_SMS         
            if (eMsgMode == WMS_MESSAGE_MODE_CDMA)
            {
               OEMWMS_AckCDMAMessage(tid, WMS_ERROR_TEMP, WMS_TL_DESTINATION_BUSY_S);
            }            
            else if (eMsgMode == WMS_MESSAGE_MODE_GW)
            {
                wms_gw_deliver_report_error_s_type * error = NULL;
                error = sys_malloc(sizeof(wms_gw_deliver_report_error_s_type));

                if(error  == NULL){
                  FARF(SMS, ("error ptr NULL. sys_malloc failed!"));
                  return; 
                }

                MEMSET(error, 0, sizeof(wms_gw_deliver_report_error_s_type));
                error->tp_cause = WMS_TP_CAUSE_UNSPECIFIED_ERROR;
                OEMWMS_NackGSMMessage(tid, WMS_TPDU_DELIVER_REPORT_ERROR, error);
                sys_free(error);
            }
#endif // #ifdef FEATURE_NOT_BREW_ONLY_SMS
            return;
         }

         // Decode the incoming message and populate the Cache Entry

         // If it's not a valid message, exit
         if (OEMWMS_CopyMessage(info_ptr->mt_message_info.transaction_id,&info_ptr->mt_message_info.message,pce) == FALSE) 
         {
            OEMSMSCache_Release(pce);     
         
            FARF(SMS, ("Error processing MT SMS Msg (Id = %d)", tid));          

#ifdef FEATURE_NOT_BREW_ONLY_SMS         
            if (eMsgMode == WMS_MESSAGE_MODE_CDMA)
            {
               OEMWMS_AckCDMAMessage(tid, WMS_ERROR_TEMP, WMS_TL_INVALID_TELESERVICE_ID_S);
            }            
            else if (eMsgMode == WMS_MESSAGE_MODE_GW)
            {
                wms_gw_deliver_report_error_s_type * error = NULL;
                error = sys_malloc(sizeof(wms_gw_deliver_report_error_s_type));

                if(error  == NULL){
                  FARF(SMS, ("error ptr NULL. sys_malloc failed!"));
                  return; 
                }

                MEMSET(error, 0, sizeof(wms_gw_deliver_report_error_s_type));
                error->tp_cause = WMS_TP_CAUSE_UNSPECIFIED_ERROR;
                OEMWMS_NackGSMMessage(tid, WMS_TPDU_DELIVER_REPORT_ERROR, error);
                sys_free(error);
            }
#endif // #ifdef FEATURE_NOT_BREW_ONLY_SMS
         }
         else
         {
            wms_status_e_type eStatus;
            wms_cdma_tl_status_e_type  eCDMAStatus = WMS_TL_OTHER_TERMINAL_PROBLEM_S;
            wms_tp_cause_e_type        eGWStatus = WMS_TP_CAUSE_UNSPECIFIED_ERROR;
            boolean bNoAckNeeded = FALSE;

#ifdef FEATURE_NOT_BREW_ONLY_SMS
            uint32 dwBytes;
            char * pszText = NULL;

            bNoAckNeeded = TRUE; // No Acking by default

            pszText = (char*)AEE_GetDecodedTextString(pce->pText, pce->nText, 
                                                      pce->nEncoding, NULL, &dwBytes);
            if (pszText) {
               bNoAckNeeded = !OEMWMS_IsBREWDirectedSMS(pszText, dwBytes); // Ack BREW directed messages
               FREE(pszText);  
            }
#endif // #ifdef FEATURE_NOT_BREW_ONLY_SMS

            eStatus = OEMSMS_GetMsgAckStatus(pce->ts, eMsgMode, &eCDMAStatus, &eGWStatus);
            
            OEMWMS_GetAutoStoreInformation(&info_ptr->mt_message_info.message,pce);

            if (eMsgMode == WMS_MESSAGE_MODE_CDMA)
            {
               if (SMS_TELESERVICE_BROADCAST != pce->ts &&
                   info_ptr->mt_message_info.message.u.cdma_message.is_tl_ack_requested &&
                   !pce->bIsStored &&
                   !bNoAckNeeded)
               {
                  FARF(SMS, ("ACK requested and sent for Msg (Id = %d)", tid));
                  OEMWMS_AckCDMAMessage(tid, (eStatus == WMS_OK_S ? WMS_ERROR_NONE : WMS_ERROR_TEMP), eCDMAStatus);
               } 
               else 
               {
                  FARF(SMS, ("No ACK requested for MT SMS Msg (Id = %d)", tid));
               }
               
               // Notify clients of incoming message 
               // regardless of -ve or +ve or no ack needed case
               OEMSMSCache_Ready(pce);
            }
            else if (eMsgMode == WMS_MESSAGE_MODE_GW)
            {
               if (pce->bIsGWBroadcast || pce->bIsStored || bNoAckNeeded)
               {
                  FARF(SMS, ("No ACK requested for MT SMS Broadcast Msg (Id = %d)", tid));
                  OEMSMSCache_Ready(pce);
               }
               else
               {
                  boolean bAck = FALSE;

                  if ((AEESMS_CLASS_0 == pce->dwMsgClass) || (AEESMS_PID_SM_TYPE_0 == pce->dwPID)) 
                  {
                     bAck = TRUE;
                  }
                  else if (AEESMS_CLASS_2 == pce->dwMsgClass) 
                  {
                     bAck = (gpWms->bStrInit[AEESMS_CARD_SIM] ? !gpWms->bStrFull[AEESMS_CARD_SIM] : FALSE);
                  }
                  else
                  {
                     bAck = (WMS_OK_S == eStatus);
                  }

                  if (bAck)
                  {
                     wms_gw_deliver_report_ack_s_type * ack = NULL;
                     ack = sys_malloc(sizeof(wms_gw_deliver_report_ack_s_type));

                     if(ack  == NULL){
                        FARF(SMS, ("ack ptr NULL. sys_malloc failed!"));
                        return; 
                     }

                     MEMSET(ack, 0, sizeof(wms_gw_deliver_report_ack_s_type));

                     OEMWMS_AckGSMMessage(tid, WMS_TPDU_DELIVER_REPORT_ACK, ack);
                     sys_free(ack);
                     OEMSMSCache_Ready(pce);
                  }
                  else
                  {
                     wms_gw_deliver_report_error_s_type * error = NULL;
                     error = sys_malloc(sizeof(wms_gw_deliver_report_error_s_type));
               
                     if(error  == NULL){
                       FARF(SMS, ("error ptr NULL. sys_malloc failed!"));
                       return; 
                     }

                     MEMSET(error, 0, sizeof(wms_gw_deliver_report_error_s_type));

                     error->tp_cause = eGWStatus;
                     OEMWMS_NackGSMMessage(tid, WMS_TPDU_DELIVER_REPORT_ERROR, error);
                     sys_free(error);

                     // Notify clients of incoming message for -ve Ack also 
                     // Usecase: some UI Apps might use this to trigger a UI 
                     // alert to users to free up memory for the incoming msg.
                     OEMSMSCache_Ready(pce);
                  }
               }
            }
            else
            {
               OEMSMSCache_Release(pce);
            }
         }
      }
      break;

      case WMS_MSG_EVENT_SEND:
         if (info_ptr->status_info.client_id == (wms_client_type_e_type)gpWms->clientId)
         {
            OEMWMS_MoMessageListener(event, info_ptr, pshared);
         }
         break;

      case WMS_MSG_EVENT_SUBMIT_REPORT:
         if (info_ptr->submit_report_info.client_id == (wms_client_type_e_type)gpWms->clientId)
         {
            OEMWMS_MoMessageListener(event, info_ptr, pshared);
         }
         break;

      case WMS_MSG_EVENT_WRITE:
      case WMS_MSG_EVENT_WRITE_TEMPLATE:
      case WMS_MSG_EVENT_WRITE_STS_REPORT:
      case WMS_MSG_EVENT_MODIFY_TAG:
         if (info_ptr->status_info.client_id == (wms_client_type_e_type)gpWms->clientId)
         {
            OEMWMS_StoreSMSListener(info_ptr);
         }
         break;

      case WMS_MSG_EVENT_DELETE:
      case WMS_MSG_EVENT_DELETE_ALL:
      case WMS_MSG_EVENT_DELETE_TEMPLATE:
      case WMS_MSG_EVENT_DELETE_TEMPLATE_ALL:
      case WMS_MSG_EVENT_DELETE_STS_REPORT:
      case WMS_MSG_EVENT_DELETE_STS_REPORT_ALL:
         if (info_ptr->status_info.client_id == (wms_client_type_e_type)gpWms->clientId)
         {
            OEMWMS_DeleteSMSListener(event, info_ptr);
         }
         break;

      case WMS_MSG_EVENT_READ:
      case WMS_MSG_EVENT_READ_TEMPLATE:
      case WMS_MSG_EVENT_READ_STS_REPORT:
         if (info_ptr->status_info.client_id == (wms_client_type_e_type)gpWms->clientId)
         {
            OEMWMS_ReadSMSListener(info_ptr);
         }
         break;

      case WMS_MSG_EVENT_DUPLICATE_CB_PAGE:
         if (gpWms)
         {
            OEMSMSUpdateData * pUpdateData = NULL;
            uint32 * pData = NULL;
            if (gpWms->pcbOnSMSUpdate)
            {
               pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSUpdate->pReserved;
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
                     *pData = (uint32)Xlate_Encoding(WMS_ENCODING_UNICODE);
                     break;

                  case WMS_GW_ALPHABET_8_BIT:
                     *pData = (uint32)Xlate_Encoding(WMS_ENCODING_OCTET);
                     break;

                  default:
                     *pData = (uint32)Xlate_Encoding(WMS_ENCODING_GSM_7_BIT_DEFAULT);
               }

               AEE_SYS_RESUME(gpWms->pcbOnSMSUpdate);
            }
         }
         break;

      default:
         break;
   }
}

#if (defined(WMS_API_VERSION) && (WMS_API_VERSION >= 0x0403) && (WMS_API_VERSION <= 0xFFFF))
//Language Mapping between WMS and BREW
typedef struct
{
   wms_gw_cb_cell_change_e_type     OEM_CCType;
   uint32                           AEE_CCType;
} GWCBCellChangeMap;

static const GWCBCellChangeMap gsGWCBCellChangeMap[] = {{WMS_GW_CB_NO_SERVICE,    AEESMS_GW_CB_NO_SERVICE},
                                                        {WMS_GW_CB_SAME_CELL,     AEESMS_GW_CB_SAME_CELL},
                                                        {WMS_GW_CB_CELL_CHANGE,   AEESMS_GW_CB_CELL_CHANGE},
                                                        {WMS_GW_CB_LAC_CHANGE,    AEESMS_GW_CB_LAC_CHANGE},
                                                        {WMS_GW_CB_PLMN_CHANGE,   AEESMS_GW_CB_PLMN_CHANGE}};

/*===========================================================================

  Translation of GW CB cell change types from DMSS to AEE

===========================================================================*/
static uint32 Xlate_GWCBCellChangeType(wms_gw_cb_cell_change_e_type CCType)
{
   GWCBCellChangeMap * pe;
   int i,nEntries = sizeof(gsGWCBCellChangeMap) / sizeof(GWCBCellChangeMap);

   for(i = 0, pe = (GWCBCellChangeMap *)gsGWCBCellChangeMap; i < nEntries; pe++,i++)
   {
      if (pe->OEM_CCType == CCType)
         return(pe->AEE_CCType);
   }

   return(AEESMS_GW_CB_UNKNOWN); 
}
#endif // #if (defined(WMS_API_VERSION) && (WMS_API_VERSION >= 0x0403) && (WMS_API_VERSION <= 0xFFFF))

/*===========================================================================

  Determines during initialization if the messaging daemons are enabled or not

===========================================================================*/

boolean OEMWMS_IsISMSPrimaryClient(void)
{
   boolean bReturn = FALSE;

#ifdef CFGI_ISMS_NOT_PRIMARY_CLIENT

   boolean cfgINPC = FALSE;
   if (OEM_GetConfig(CFGI_ISMS_NOT_PRIMARY_CLIENT, (void*)&cfgINPC, sizeof(cfgINPC)) != AEE_SUCCESS)
   {
      FARF(SMS, ("Failed to get Item CFGI_ISMS_NOT_PRIMARY_CLIENT"));
   }
   else
   {
      bReturn = (!cfgINPC);
   }
#endif
   return bReturn;
}
/*===========================================================================

  Invoked to pass along the configuration events from the WMS

===========================================================================*/
void OEMWMS_CfgListener
(
   wms_cfg_event_e_type event,
   wms_cfg_event_info_s_type *info_ptr
)
{
   switch(event)
   {
      case WMS_CFG_EVENT_CDMA_READY:
#ifndef FEATURE_WMS_APP
         OEMWMS_GetMsgTransferRoute();
#endif /* FEATURE_WMS_APP */

         CALLBACK_Cancel(&gpWms->cbOnStorageEvent[AEESMS_CARD_RUIM]);
         CALLBACK_Init(&gpWms->cbOnStorageEvent[AEESMS_CARD_RUIM], OEMWMS_GetMemoryStatus, (void*)AEESMS_CARD_RUIM);
         AEE_SYS_RESUME(&gpWms->cbOnStorageEvent[AEESMS_CARD_RUIM]);

         CALLBACK_Cancel(&gpWms->cbOnStorageEvent[AEESMS_NV_CDMA]);
         CALLBACK_Init(&gpWms->cbOnStorageEvent[AEESMS_NV_CDMA], OEMWMS_GetMemoryStatus, (void*)AEESMS_NV_CDMA);
         AEE_SYS_RESUME(&gpWms->cbOnStorageEvent[AEESMS_NV_CDMA]);

         CALLBACK_Cancel(&gpWms->cbOnStorageEvent[AEESMS_CARD_RUIM_TEMPLATE]);
         CALLBACK_Init(&gpWms->cbOnStorageEvent[AEESMS_CARD_RUIM_TEMPLATE], OEMWMS_GetMemoryStatus, (void*)AEESMS_CARD_RUIM_TEMPLATE);
         AEE_SYS_RESUME(&gpWms->cbOnStorageEvent[AEESMS_CARD_RUIM_TEMPLATE]);

         CALLBACK_Cancel(&gpWms->cbOnStorageEvent[AEESMS_NV_CDMA_TEMPLATE]);
         CALLBACK_Init(&gpWms->cbOnStorageEvent[AEESMS_NV_CDMA_TEMPLATE], OEMWMS_GetMemoryStatus, (void*)AEESMS_NV_CDMA_TEMPLATE);
         AEE_SYS_RESUME(&gpWms->cbOnStorageEvent[AEESMS_NV_CDMA_TEMPLATE]);

         // Read CDMA NV Template to Fetch Value for TL Ack Requested
         wms_msg_read_template(gpWms->clientId, OEMWSMS_NoopCommandCallback, NULL, WMS_MEMORY_STORE_NV_CDMA, 0);
         break;

      case WMS_CFG_EVENT_GW_READY:
         OEMWMS_GetDomainPref();
 #ifndef FEATURE_WMS_APP
         OEMWMS_GetMsgTransferRoute();
 #endif /* FEATURE_WMS_APP */
         if (OEMWMS_IsISMSPrimaryClient())
         {
            OEMWMS_SetPrimaryClient();
         }

         CALLBACK_Cancel(&gpWms->cbOnStorageEvent[AEESMS_CARD_SIM]);
         CALLBACK_Init(&gpWms->cbOnStorageEvent[AEESMS_CARD_SIM], OEMWMS_GetMemoryStatus, (void*)AEESMS_CARD_SIM);
         AEE_SYS_RESUME(&gpWms->cbOnStorageEvent[AEESMS_CARD_SIM]);

         CALLBACK_Cancel(&gpWms->cbOnStorageEvent[AEESMS_NV_GW]);
         CALLBACK_Init(&gpWms->cbOnStorageEvent[AEESMS_NV_GW], OEMWMS_GetMemoryStatus, (void*)AEESMS_NV_GW);
         AEE_SYS_RESUME(&gpWms->cbOnStorageEvent[AEESMS_NV_GW]);

         CALLBACK_Cancel(&gpWms->cbOnStorageEvent[AEESMS_CARD_SIM_TEMPLATE]);
         CALLBACK_Init(&gpWms->cbOnStorageEvent[AEESMS_CARD_SIM_TEMPLATE], OEMWMS_GetMemoryStatus, (void*)AEESMS_CARD_SIM_TEMPLATE);
         AEE_SYS_RESUME(&gpWms->cbOnStorageEvent[AEESMS_CARD_SIM_TEMPLATE]);

         CALLBACK_Cancel(&gpWms->cbOnStorageEvent[AEESMS_NV_GW_TEMPLATE]);
         CALLBACK_Init(&gpWms->cbOnStorageEvent[AEESMS_NV_GW_TEMPLATE], OEMWMS_GetMemoryStatus, (void*)AEESMS_NV_GW_TEMPLATE);
         AEE_SYS_RESUME(&gpWms->cbOnStorageEvent[AEESMS_NV_GW_TEMPLATE]);

         CALLBACK_Cancel(&gpWms->cbOnStorageEvent[AEESMS_CARD_SIM_STATUS_REPORT]);
         CALLBACK_Init(&gpWms->cbOnStorageEvent[AEESMS_CARD_SIM_STATUS_REPORT], OEMWMS_GetMemoryStatus, (void*)AEESMS_CARD_SIM_STATUS_REPORT);         
         AEE_SYS_RESUME(&gpWms->cbOnStorageEvent[AEESMS_CARD_SIM_STATUS_REPORT]);
         break;

      case WMS_CFG_EVENT_MEMORY_STATUS:
         {
            int nStrIdx = Xlate_StorageType(info_ptr->memory_status.mem_store, info_ptr->memory_status.tag);

            // If Storage isn't initialized, calculate max slots for Storage and get 
            // message tag information for occupied slots
            if ((0 <= nStrIdx) && (nStrIdx < OEMSMS_STORAGE_LAST))
            {
               if (!gpWms->bMemStaInit[nStrIdx])
               {
                  gpWms->ucStrSMSCount[nStrIdx] = info_ptr->memory_status.max_slots;
                  gpWms->bStrFull[nStrIdx] = (0 == info_ptr->memory_status.free_slots);
                  gpWms->bMemStaInit[nStrIdx] = TRUE;
                  CALLBACK_Cancel(&gpWms->cbOnStorageEvent[nStrIdx]);
                  CALLBACK_Init(&gpWms->cbOnStorageEvent[nStrIdx], OEMWMS_GetMessageList, (void*)nStrIdx);         
                  AEE_SYS_RESUME(&gpWms->cbOnStorageEvent[nStrIdx]);
               }
               else
               {
                  gpWms->bStrFull[nStrIdx] = (0 == info_ptr->memory_status.free_slots);
                  
                  if (gpWms->bStrFull[nStrIdx])
                     OEMWMS_NotifyMemoryEvent(nStrIdx, EVT_MDL_SMSSTORAGE_FULL);
               }
            }
         }
         break;

      case WMS_CFG_EVENT_MESSAGE_LIST:
         {
            int nStrIdx = Xlate_StorageType(info_ptr->message_list.mem_store, info_ptr->message_list.tag);

            if ((0 <= nStrIdx) && 
                (nStrIdx < OEMSMS_STORAGE_LAST) &&
                ((WMS_TAG_NONE == info_ptr->message_list.tag) ||
                 (WMS_TAG_STATUS_RPT == info_ptr->message_list.tag) ||
                 (WMS_TAG_MO_TEMPLATE == info_ptr->message_list.tag)))
            {
               uint32 i = 0;
               uint32 index = 0; //which index are occupied

               // First set all the msgType for all the elements in strSMSMsgList[nStrIdx]
               // as avaiable. Then for the indexes returned in the message list, 
               // assign the proper tags. This way applet will know which slots are 
               // occupied and which are available
               if (gpWms->bMemStaInit[nStrIdx]) // check that memory status event has occurred
               {
                  MEMSET((void*)gpWms->strSMSMsgList[nStrIdx], 0, sizeof(gpWms->strSMSMsgList[nStrIdx]));
            
                  for(i = 0; i < info_ptr->message_list.len; i++) 
                  {               
                     index = info_ptr->message_list.indices[i];
                     gpWms->strSMSMsgList[nStrIdx][index] = info_ptr->message_list.tags[i];            
                  }

                  if (!gpWms->bStrInit[nStrIdx])
                  {
                     gpWms->bStrInit[nStrIdx] = TRUE;
                     OEMWMS_NotifyMemoryEvent(nStrIdx, EVT_MDL_SMSSTORAGE_READY);
                  }
               }  
            }
         }
         break;
     
      case WMS_CFG_EVENT_GW_DOMAIN_PREF:
         if (gpWms)
         {
            gpWms->gwDomainPref = info_ptr->gw_domain_pref;
         }
         break;

      case WMS_CFG_EVENT_CELL_CHANGE:
         if (gpWms)
         {
            OEMSMSUpdateData * pUpdateData = NULL;
            uint32 * pData = NULL;
            if (gpWms->pcbOnSMSUpdate)
            {
               pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSUpdate->pReserved;
               pUpdateData->nEvent = EVT_MDL_SMS_CELL_CHANGE;
               pData = (uint32*)pUpdateData->aEventData;
#if (defined(WMS_API_VERSION) && (WMS_API_VERSION >= 0x0403) && (WMS_API_VERSION <= 0xFFFF))
               *pData = Xlate_GWCBCellChangeType(info_ptr->cell_change_type);
#endif // #if (defined(WMS_API_VERSION) && (WMS_API_VERSION >= 0x0403) && (WMS_API_VERSION <= 0xFFFF))
               AEE_SYS_RESUME(gpWms->pcbOnSMSUpdate);
            }
         }
         break;

      case WMS_CFG_EVENT_MEMORY_FULL:
         if (gpWms)
         {
            int nStrIdx = Xlate_StorageType(info_ptr->mem_store, WMS_TAG_NONE);

            if ((0 <= nStrIdx) && (nStrIdx < OEMSMS_STORAGE_LAST)) {
               OEMWMS_NotifyMemoryEvent(nStrIdx, EVT_MDL_SMSSTORAGE_FULL);
            }
         }
         break;

      case WMS_CFG_EVENT_ROUTES:
         if (gpWms)
         {
            if (gpWms->bRoutesInitialized)
            {
               int nMsgClass, nStrIdx;
               for(nMsgClass = 0; nMsgClass < WMS_MESSAGE_CLASS_MAX; nMsgClass++)
               {
                  if (MEMCMP(&gpWms->routes.pp_routes[nMsgClass], &info_ptr->routes.pp_routes[nMsgClass], sizeof(wms_routing_s_type)))
                  {
                     if (WMS_ROUTE_STORE_AND_NOTIFY == info_ptr->routes.pp_routes[nMsgClass].route)
                     {            
                        nStrIdx = Xlate_StorageType(info_ptr->routes.pp_routes[nMsgClass].mem_store, WMS_TAG_NONE);

                        if ((0 <= nStrIdx) && (nStrIdx < OEMSMS_STORAGE_LAST)) {
                           OEMWMS_NotifyMemoryEvent(nStrIdx, EVT_MDL_SMSSTORAGE_CLASS_0_AUTOSTORE + nMsgClass);
                        }
                     }
                     else
                     {                        
                        OEMWMS_NotifyMemoryEvent(AEESMS_STORAGE_NONE, EVT_MDL_SMSSTORAGE_CLASS_0_AUTOSTORE + nMsgClass);
                     }
                  }
               }
               MEMCPY(&gpWms->routes, &info_ptr->routes, sizeof(wms_routes_s_type));
            }
            else
            {
               MEMCPY(&gpWms->routes, &info_ptr->routes, sizeof(wms_routes_s_type));
               gpWms->bRoutesInitialized = TRUE;
            }
         }
         break;

      default:
         break;
   }
}

/*===========================================================================

  Called to ack CDMA message

===========================================================================*/
static void OEMWMS_AckCDMAMessage
(
   wms_transaction_id_type tid,
   wms_error_class_e_type eErr,
   wms_cdma_tl_status_e_type eStatus
)
{
   wms_ack_info_s_type *ackInfo = NULL;
   ackInfo = sys_malloc(sizeof(wms_ack_info_s_type));

   if(ackInfo == NULL){
      FARF(SMS, ("ackInfo ptr NULL. sys_malloc failed!"));
      return; 
   }

   MEMSET(ackInfo, 0, sizeof(wms_ack_info_s_type));

   ackInfo->transaction_id = tid;
   ackInfo->message_mode = WMS_MESSAGE_MODE_CDMA;
   ackInfo->u.cdma.error_class = eErr;
   ackInfo->u.cdma.tl_status = eStatus;

   wms_msg_ack( gpWms->clientId, &OEMWSMS_NoopCommandCallback, NULL, ackInfo );
   sys_free(ackInfo);
}

/*===========================================================================

  Called to ack GSM message

===========================================================================*/
static void OEMWMS_AckGSMMessage
(
   wms_transaction_id_type tid,
   wms_gw_tpdu_type_e_type tpdu_type,
   wms_gw_deliver_report_ack_s_type * pAck
)
{
   wms_ack_info_s_type *ackInfo = NULL;
   ackInfo = sys_malloc(sizeof(wms_ack_info_s_type));

   if(ackInfo == NULL){
      FARF(SMS, ("ackInfo ptr NULL. sys_malloc failed!"));
      return; 
   }

   MEMSET(ackInfo, 0, sizeof(wms_ack_info_s_type));
   ackInfo->transaction_id = tid;
   ackInfo->message_mode = WMS_MESSAGE_MODE_GW;

   ackInfo->u.gw.success = TRUE;
   ackInfo->u.gw.tpdu_type = tpdu_type;
   MEMCPY(&ackInfo->u.gw.u.ack, pAck, sizeof(ackInfo->u.gw.u.ack));

   wms_msg_ack( gpWms->clientId, OEMWSMS_NoopCommandCallback, NULL, ackInfo );
   sys_free(ackInfo);
}

/*===========================================================================

  Called to nack GSM message

===========================================================================*/
static void OEMWMS_NackGSMMessage
(
   wms_transaction_id_type tid,
   wms_gw_tpdu_type_e_type tpdu_type,
   wms_gw_deliver_report_error_s_type * pError
)
{
   wms_ack_info_s_type *ackInfo = NULL;
   ackInfo = sys_malloc(sizeof(wms_ack_info_s_type));

   if(ackInfo == NULL){
      FARF(SMS, ("ackInfo ptr NULL. sys_malloc failed!"));
      return; 
   }

   MEMSET(ackInfo, 0, sizeof(wms_ack_info_s_type));

   ackInfo->transaction_id = tid;
   ackInfo->message_mode = WMS_MESSAGE_MODE_GW;

   ackInfo->u.gw.success = FALSE;
   ackInfo->u.gw.tpdu_type = tpdu_type;

   if (WMS_TP_CAUSE_MEMORY_FULL == pError->tp_cause)
      ackInfo->u.gw.rp_cause = WMS_RP_CAUSE_MEMORY_EXCEEDED;
   else
      ackInfo->u.gw.rp_cause = WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED;

   MEMCPY(&ackInfo->u.gw.u.error, pError, sizeof(ackInfo->u.gw.u.error));

   wms_msg_ack( gpWms->clientId, OEMWSMS_NoopCommandCallback, NULL, ackInfo );
   sys_free(ackInfo);
}

/*===========================================================================

  Gives a chance to us to ensure that message isn't shared with any one else.
  Use this to make sure that BREW directed messages aren't shared.

===========================================================================*/
static void OEMWMS_PreProcessor
(
   const wms_client_message_s_type * pCMsg, 
   boolean * pshared 
)
{
   uint32 dwBytes;
   wms_status_e_type decodeStatus;
   wms_client_ts_data_s_type * pctsdata = NULL;

   FARF(SMS, ("==> OEMWMS_PreProcessor"));   
   
   if (!pshared)
   {   
      FARF(SMS, ("pshared == 0 in OEMWMS_PreProcessor"));   
      return;
   }

   MEMSET(&sce, 0, sizeof(sce));

   pctsdata = (wms_client_ts_data_s_type *)sce.data;
 
   *pshared = TRUE;

   if (pCMsg->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
   {
      wms_cdma_message_s_type * pm = (wms_cdma_message_s_type *)&pCMsg->u.cdma_message;
      wms_client_bd_s_type *pcd = (wms_client_bd_s_type *)&pctsdata->u.cdma;
         
      FARF(SMS,("SMS Len: %d, Format: %d",pm->raw_ts.len, pm->raw_ts.format));

      decodeStatus = wms_ts_decode(&pm->raw_ts, pctsdata);

      if ((decodeStatus == WMS_OK_S)                             &&
          (pcd->mask & WMS_MASK_BD_USER_DATA)                    && 
          ((pm->teleservice == WMS_TELESERVICE_CMT_95)      ||
           ((pm->teleservice == WMS_TELESERVICE_CMT_91) &&
            (pcd->user_data.is91ep_type == WMS_IS91EP_SHORT_MESSAGE ||
             pcd->user_data.is91ep_type == WMS_IS91EP_SHORT_MESSAGE_FULL))))
      {
         char * pszText = NULL;

         sce.pText = (pcd->mask & WMS_MASK_BD_USER_DATA ? (byte*)pcd->user_data.data : NULL);
         sce.nText = (pcd->mask & WMS_MASK_BD_USER_DATA ? pcd->user_data.data_len : 0);
         sce.nEncoding = Xlate_Encoding(pcd->user_data.encoding);

         FARF(SMS, ("nText = %d, nEncoding = %d", sce.nText, sce.nEncoding));
         FARF(SMS, ("Text = %s", sce.pText));

         if ((pszText = (char*)AEE_GetDecodedTextString(sce.pText, sce.nText, sce.nEncoding, NULL, &dwBytes)) != NULL)
         {
            if (OEMWMS_IsBREWDirectedSMS(pszText, dwBytes) == TRUE)
               *pshared = FALSE;
            FREE(pszText);
         }
      }
   }
   else if ((pCMsg->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW) &&
            (pCMsg->u.gw_message.raw_ts_data.tpdu_type == WMS_TPDU_DELIVER) &&
            (pCMsg->u.gw_message.is_broadcast != TRUE))
   {           
      wms_gw_message_s_type * pm = (wms_gw_message_s_type *)&pCMsg->u.gw_message;

      FARF(SMS,("SMS Len: %d, Format: %d",pm->raw_ts_data.len, pm->raw_ts_data.format));

      decodeStatus = wms_ts_decode(&pm->raw_ts_data, pctsdata);

      if ((decodeStatus == WMS_OK_S) &&
          (OEMWMS_DetermineGSMMessageType(&pctsdata->u.gw_pp) == WMS_TELESERVICE_CMT_95))
      {
         char * pszText = NULL;

         sce.pText = pctsdata->u.gw_pp.u.deliver.user_data.sm_data;
         sce.nText = pctsdata->u.gw_pp.u.deliver.user_data.sm_len;

         switch(pctsdata->u.gw_pp.u.deliver.dcs.alphabet)
         {
            case WMS_GW_ALPHABET_UCS2:
               sce.nEncoding = Xlate_Encoding(WMS_ENCODING_UNICODE);
               break;

            case WMS_GW_ALPHABET_8_BIT:
               sce.nEncoding = Xlate_Encoding(WMS_ENCODING_OCTET);
               break;

            default:
               sce.nEncoding = Xlate_Encoding(WMS_ENCODING_GSM_7_BIT_DEFAULT);
         }
         
         FARF(SMS, ("nText = %d, nEncoding = %d", sce.nText, sce.nEncoding));
         FARF(SMS, ("Text = %s", sce.pText));         
         
         if ((pszText = (char*)AEE_GetDecodedTextString(sce.pText, sce.nText, sce.nEncoding, NULL, &dwBytes)) != NULL)
         {
            if (OEMWMS_IsBREWDirectedSMS(pszText, dwBytes) == TRUE)
               *pshared = FALSE;
            FREE(pszText);
         }
      }
   }
}

/*===========================================================================

  Called by OEMWMS_MessageListener to copy incoming SMS message to SMS cache

===========================================================================*/
boolean OEMWMS_CopyMessage
(
   wms_transaction_id_type transaction_id,
   wms_client_message_s_type *pCMsg,
   SMSCacheEntry *pce 
)
{
   wms_status_e_type decodeStatus;
   wms_teleservice_e_type ts;
   wms_client_ts_data_s_type * pctsdata = NULL;

   FARF(SMS, ("==> OEMWMS_CopyMessage"));   

   pctsdata = (wms_client_ts_data_s_type *)pce->data;
   
   if (pCMsg->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
   {
      if (pCMsg->msg_hdr.tag != WMS_TAG_MO_TEMPLATE)
      {
         wms_cdma_message_s_type * pm = (wms_cdma_message_s_type *)&pCMsg->u.cdma_message;
         wms_client_bd_s_type *pcd = (wms_client_bd_s_type *)&pctsdata->u.cdma;
         
         FARF(SMS,("SMS Len: %d, Format: %d",pm->raw_ts.len, pm->raw_ts.format));

         // Check if message is MO SMS which will mean copy message was called
         // for processing an MO SMS read from storage
         pce->bIsMOSMS = pm->is_mo;

         decodeStatus = wms_ts_decode(&pm->raw_ts, pctsdata);

         // Only continue if we have bearer data (text) or it's a voice mail message
         // or an IS91 Page message. (We reject CPT-95, CMT-95 and IS91 Short Message
         // Types if the Message does not contain user data).

         if (decodeStatus == WMS_OK_S)         
         {
            // If the teleservice of the message happens to be WMS_TELESERVICE_CMT_91
            // we shall map the teleservice to the teleservice corresponding 
            // to the Message Type of the embedded Analog IS91-EP message.
            // See section '4.3.2 IS-91 Extended Protocol Enhanced Services' of IS-637-B
            // for information about CMT_91 teleservice (note that the section does not 
            // directly mention CMT_91 teleservice, but talks about how a IS91-EP message
            // can be embedded inside a SMS Deliver Message.)
            // CMT_91 teleservice is used to embed a IS-91 EP SMS message inside a SMS 
            // Deliver Message in a CDMA network.

            if (pm->teleservice == WMS_TELESERVICE_CMT_91) 
            {
               // A CMT_91 Teleservice message contains a embedded IS91 message
               // the is91ep_type in the user_data specifies the IS91 Message 
               // type (See Section 4.5.2 of IS637-B).

               if (pcd->user_data.is91ep_type == WMS_IS91EP_CLI_ORDER) 
               {
                  ts = WMS_TELESERVICE_IS91_PAGE; 
               } 
               else if (pcd->user_data.is91ep_type == WMS_IS91EP_VOICE_MAIL) 
               {
                  ts = WMS_TELESERVICE_IS91_VOICE_MAIL;
               } 
               else if (pcd->user_data.is91ep_type == WMS_IS91EP_SHORT_MESSAGE ||
                        pcd->user_data.is91ep_type == WMS_IS91EP_SHORT_MESSAGE_FULL) 
               {
                  ts = WMS_TELESERVICE_IS91_SHORT_MESSAGE;
               } 
               else 
               {
                  // This should never happen, but better safe than sorry....
                  FARF(SMS, ("CMT_91 Msg with unknown IS91-EP Message Type %d", pcd->user_data.is91ep_type));
                  FARF(SMS, ("Rejecting CMT_91 Msg"));
                  return(FALSE);
               }
            }
            else if ((pcd->message_id.udh_present) &&
                     (pcd->mask & WMS_MASK_BD_USER_DATA) &&
                     (pcd->user_data.num_headers))
            {
               int i;

               // Initialize with EMS teleservice ID
               ts = pm->teleservice;

               for(i = 0; i < pcd->user_data.num_headers; i++) 
               {
                  if ((pcd->user_data.headers[i].header_id == WMS_UDH_SPECIAL_SM) &&
                     (pcd->user_data.headers[i].u.special_sm.msg_waiting_kind == WMS_GW_MSG_WAITING_VOICEMAIL)) 
                  {
                     FARF(SMS, ("Voice Mail Notification Message Received"));
                     ts =  WMS_TELESERVICE_VMN_95;
                     break;
                  }
                  else if ((pcd->user_data.headers[i].header_id == WMS_UDH_PORT_16) &&
                     (pcd->user_data.headers[i].u.wap_16.dest_port == 2948)) 
                  {
                     FARF(SMS, ("WAP Push Message Received"));
#ifdef CUST_EDITION	  					 
#if defined(FEATURE_QMA)
                     ts =  WMS_TELESERVICE_QMA_WPUSH;
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
                     ts =  WMS_TELESERVICE_WPUSH;
#else                     
                     ts =  WMS_TELESERVICE_WAP;
#endif
#else
                     ts =  WMS_TELESERVICE_WAP;
#endif /*CUST_EDITION*/
                     break;
                  }
               }
            }
            else
            {
               ts = pm->teleservice;
            }
      
            pce->ts = Xlate_TS(ts);
            pce->dwSrvID = pm->service;
            pce->pText = (pcd->mask & WMS_MASK_BD_USER_DATA ? (byte*)pcd->user_data.data : NULL);
            pce->nText = (pcd->mask & WMS_MASK_BD_USER_DATA ? pcd->user_data.data_len : 0);
         
            if (ts == WMS_TELESERVICE_WAP
#ifdef CUST_EDITION	  			
#if defined(FEATURE_QMA)
                || ts == WMS_TELESERVICE_QMA_WPUSH
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
                || ts == WMS_TELESERVICE_WPUSH
#endif
#endif /*CUST_EDITION*/
                )
            {
               pce->nEncoding = AEESMS_ENC_OCTET;
            }
            else
            {
               pce->nEncoding = Xlate_Encoding(pcd->user_data.encoding);
            }

            pce->bIsGWBroadcast = FALSE;

            // Before rest of the processing, let's ensure that if message is BREW directed,
            // its sent with teleservice ID designated for BREW directed messages
            if (gpWms->dwBREWDirectedTS && pce->pText && pce->nText &&
                (ts == WMS_TELESERVICE_CMT_95 || ts == WMS_TELESERVICE_IS91_SHORT_MESSAGE))
            {
               uint32 dwBytes;
               char * pszText = NULL;

               if (((pszText = (char*)AEE_GetDecodedTextString(pce->pText, pce->nText, pce->nEncoding, NULL, &dwBytes)) != NULL) &&
                  (OEMWMS_IsBREWDirectedSMS(pszText, dwBytes) == TRUE) &&
                  (ts != (wms_teleservice_e_type)gpWms->dwBREWDirectedTS))
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
            if (pcd->mask & WMS_MASK_BD_PRIORITY )
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
            if (pcd->mask & WMS_MASK_BD_PRIVACY )
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
            if (pcd->mask & WMS_MASK_BD_LANGUAGE )
            {
               pce->dwLang = Xlate_Lang(pcd->language);
               pce->bLangPresent = TRUE;
               FARF(SMS, ("dwLang = %d", pce->dwLang));
            }
            else
            {
               pce->bLangPresent = FALSE;
            }

            // PID ignored for CDMA Message
            pce->dwMsgClass = WMS_MESSAGE_CLASS_CDMA;
            pce->bRequestDLAck = pcd->reply_option.delivery_ack_requested;
            pce->bRequestUserAck = pcd->reply_option.user_ack_requested;
            pce->bRequestReadAck = pcd->reply_option.read_ack_requested;
            pce->dwTag = Xlate_Tag(pCMsg->msg_hdr.tag);
            pce->dwMsgID = pcd->message_id.id_number;
            if (WMS_BD_TYPE_DELIVERY_ACK == pcd->message_id.type) {
               pce->bIsDLAck = TRUE;
            }
            else if (WMS_BD_TYPE_USER_ACK == pcd->message_id.type) {
               pce->bIsUserAck = TRUE;
            }
            else if (WMS_BD_TYPE_READ_ACK == pcd->message_id.type) {
               pce->bIsReadAck = TRUE;
            }
            else if (WMS_BD_TYPE_CANCELLATION == pcd->message_id.type) {
               pce->bIsCancelRequest = TRUE;
            }

            pce->bIsSCAddress = FALSE;
            pce->dwMessageDisplayMode = ((pcd->mask & WMS_MASK_BD_DISPLAY_MODE) ? (uint32)pcd->display_mode : AEESMS_DISPLAY_MODE_NONE);

            if (OEMWMS_GetNumMsgsInfoFromCDMABDMsg (pcd, ts, &pce->nNumMsgs) == FALSE)
               return FALSE;

            FARF(SMS, ("nNumMsgs = %d", pce->nNumMsgs));
            
            if (OEMWMS_GetCallBackNumberFromCDMABDMsg(pcd, ts, pce->szCallback, sizeof(pce->szCallback), &pce->dwCallbackType, &pce->dwCallbackPlan) == FALSE)
               return FALSE;

            OEMWMS_GetAddress(&pm->address, pce->szFrom, sizeof(pce->szFrom), &pce->dwMOFromMTToType, &pce->dwMOFromMTToPlan, TRUE);

            FARF(SMS, ("tid = %d, nText = %d, nEncoding = %d", pce->dwTransactionId, pce->nText, pce->nEncoding));
            FARF(SMS, ("Text = %s", pce->pText));
            FARF(SMS, ("Callback = %s", (char*)(*pce->szCallback ? pce->szCallback : "Unknown")));

            FARF(SMS, ("To = %s", (char*)(*pce->szFrom ? pce->szFrom : "Unknown")));

            if (pcd->mask & WMS_MASK_BD_VALID_REL) {
               pce->dwRelativeValidity = OEMWMS_GetTimeFromWmsTimestamp(&pcd->validity_relative, NULL, TRUE);                  
               FARF(SMS, ("RelativeValidity = %d", pce->dwRelativeValidity));
            }
            else {
               pce->dwRelativeValidity = 0xFFFFFFFF;
            }

            if (pcd->mask & WMS_MASK_BD_VALID_ABS) {
               pce->dwAbsoluteValidityTimeStamp = OEMWMS_GetTimeFromWmsTimestamp(&pcd->validity_absolute, &pce->nAbsoluteValidityTimeStampZone, FALSE);                  
               FARF(SMS, ("AbsoluteValidity = %d", pce->dwAbsoluteValidityTimeStamp));
            }

            if (pcd->mask & WMS_MASK_BD_MC_TIME)
            {
               FARF(SMS, ("From = %s", (char*)(*pce->szFrom ? pce->szFrom : "Unknown")));
               pce->dwTimeStamp = OEMWMS_GetTimeFromWmsTimestamp(&pcd->mc_time, &pce->nTimeStampZone, FALSE);         
               FARF(SMS, ("TimeStamp = %d", pce->dwTimeStamp));
            }
            
            if (pcd->mask & WMS_MASK_BD_DEFER_ABS) {
               pce->dwAbsoluteDeferredValidity = OEMWMS_GetTimeFromWmsTimestamp(&pcd->deferred_absolute, NULL, FALSE);
               FARF(SMS, ("Absolute Deferred Validity = %d", pce->dwAbsoluteDeferredValidity));
            }

            if (pcd->mask & WMS_MASK_BD_DEFER_REL) {
               pce->dwRelativeDeferredValidity = OEMWMS_GetTimeFromWmsTimestamp(&pcd->deferred_relative, NULL, TRUE);
               FARF(SMS, ("Relative Deferred Validity = %d", pce->dwRelativeDeferredValidity));
            }
            else {
               pce->dwRelativeDeferredValidity = 0xFFFFFFFF;
            }
            pce->dwGWCommand = 0xFFFFFFFF;
            return(TRUE);
         }
         FARF(SMS, ("Teleservice %d Msg has no UserData",pce->ts));
      }
      else
      {
         wms_cdma_template_s_type * pm = (wms_cdma_template_s_type *)&pCMsg->u.cdma_template;
         wms_client_bd_s_type *pcd = (wms_client_bd_s_type *)&pctsdata->u.cdma;

         MEMCPY(pcd, &pm->client_bd, sizeof(wms_client_bd_s_type));

         // Only continue if we have bearer data (text)
         if ((pm->mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
             (pm->teleservice == WMS_TELESERVICE_CMT_95)) 
         {
            pce->ts  = Xlate_TS(pm->teleservice);
            pce->pText = (pcd->mask & WMS_MASK_BD_USER_DATA ? (byte*)pcd->user_data.data : NULL);
            pce->nText = (pcd->mask & WMS_MASK_BD_USER_DATA ? pcd->user_data.data_len : 0);
            pce->nEncoding = Xlate_Encoding(pcd->user_data.encoding);
            
            // Priority
            if (pcd->mask & WMS_MASK_BD_PRIORITY )
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
            if (pcd->mask & WMS_MASK_BD_PRIVACY )
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
            if (pcd->mask & WMS_MASK_BD_LANGUAGE )
            {
               pce->dwLang = Xlate_Lang(pcd->language);
               pce->bLangPresent = TRUE;
               FARF(SMS, ("dwLang = %d", pce->dwLang));
            }
            else
            {
               pce->bLangPresent = FALSE;
            }

            // PID Ignored for CDMA Template
            pce->dwMsgClass = WMS_MESSAGE_CLASS_CDMA;
            pce->bRequestDLAck = pcd->reply_option.delivery_ack_requested;
            pce->bRequestUserAck = pcd->reply_option.user_ack_requested;
            pce->bRequestReadAck = pcd->reply_option.read_ack_requested;
            pce->dwTag = Xlate_Tag(pCMsg->msg_hdr.tag);
            pce->dwMsgID = pcd->message_id.id_number;      
            if (WMS_BD_TYPE_DELIVERY_ACK == pcd->message_id.type) {
               pce->bIsDLAck = TRUE;
            }
            else if (WMS_BD_TYPE_USER_ACK == pcd->message_id.type) {
               pce->bIsUserAck = TRUE;
            }
            else if (WMS_BD_TYPE_READ_ACK == pcd->message_id.type) {
               pce->bIsReadAck = TRUE;
            }
            else if (WMS_BD_TYPE_CANCELLATION == pcd->message_id.type) {
               pce->bIsCancelRequest = TRUE;
            }
            pce->bIsSCAddress = FALSE;
            pce->dwMessageDisplayMode = ((pcd->mask & WMS_MASK_BD_DISPLAY_MODE) ? (uint32)pcd->display_mode : AEESMS_DISPLAY_MODE_NONE);
            pce->bIsMOSMS = TRUE;

            if (OEMWMS_GetCallBackNumberFromCDMABDMsg(pcd, pm->teleservice, pce->szCallback, sizeof(pce->szCallback), &pce->dwCallbackType, &pce->dwCallbackPlan) == FALSE)
               return FALSE;

            FARF(SMS, ("tid = %d, nText = %d, nEncoding = %d", pce->dwTransactionId, pce->nText, pce->nEncoding));
            FARF(SMS, ("Text = %s", pce->pText));
            FARF(SMS, ("Callback = %s", (char*)(*pce->szCallback ? pce->szCallback : "Unknown")));
            
            if (pcd->mask & WMS_MASK_BD_DEFER_ABS) {
               pce->dwAbsoluteDeferredValidity = OEMWMS_GetTimeFromWmsTimestamp(&pcd->deferred_absolute, NULL, FALSE);
            }

            if (pcd->mask & WMS_MASK_BD_DEFER_REL) {
               pce->dwRelativeDeferredValidity = OEMWMS_GetTimeFromWmsTimestamp(&pcd->deferred_relative, NULL, TRUE);
            }
            else {
               pce->dwRelativeDeferredValidity = 0xFFFFFFFF;
            }

            if (pm->mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR)
            {
               OEMWMS_GetAddress(&pm->address, pce->szFrom, sizeof(pce->szFrom), &pce->dwMOFromMTToType, &pce->dwMOFromMTToPlan, TRUE);
               FARF(SMS, ("To = %s", (char*)(*pce->szFrom ? pce->szFrom : "Unknown")));
            }

            if (pm->mask & WMS_CDMA_TEMPLATE_MASK_VALIDITY) {
               pce->dwRelativeValidity = OEMWMS_GetTimeFromWmsTimestamp(&pm->relative_validity, NULL, TRUE);           
               FARF(SMS, ("RelativeValidity = %d", pce->dwRelativeValidity));
            }
            else {
               pce->dwRelativeValidity = 0xFFFFFFFF;
            }

            pce->dwGWCommand = 0xFFFFFFFF;
            return(TRUE);
         }
         FARF(SMS, ("Teleservice %d Msg has no UserData",pce->ts));
      }
   }
   else if (pCMsg->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
   {
      pce->dwMessageDisplayMode = AEESMS_DISPLAY_MODE_NONE;
      if (pCMsg->msg_hdr.tag != WMS_TAG_MO_TEMPLATE)
      {         
         if (pCMsg->u.gw_message.raw_ts_data.format == WMS_FORMAT_GW_CB)    
         {
            wms_gw_message_s_type * pm = (wms_gw_message_s_type *)&pCMsg->u.gw_message;

            FARF(SMS,("SMS GW CB Broadcast Msg Len: %d, Format: %d",pm->raw_ts_data.len, pm->raw_ts_data.format));

            decodeStatus = wms_ts_decode(&pm->raw_ts_data, pctsdata);

            if (decodeStatus == WMS_OK_S)
            {
               pce->ts = Xlate_TS(WMS_TELESERVICE_BROADCAST);

               pce->pText = pctsdata->u.gw_cb.user_data.sm_data;
               pce->nText = pctsdata->u.gw_cb.user_data.sm_len;

               switch(pctsdata->u.gw_cb.cb_page_hdr.cb_dcs.alphabet)
               {
                  case WMS_GW_ALPHABET_UCS2:
                     pce->nEncoding = Xlate_Encoding(WMS_ENCODING_UNICODE);
                     break;

                  case WMS_GW_ALPHABET_8_BIT:
                     pce->nEncoding = Xlate_Encoding(WMS_ENCODING_OCTET);
                     break;

                  default:
                     pce->nEncoding = Xlate_Encoding(WMS_ENCODING_GSM_7_BIT_DEFAULT);
               }

               pce->bIsMOSMS = FALSE;
               pce->bIsSCAddress = TRUE;
               pce->bIsGWBroadcast = TRUE;

               pce->dwLang = Xlate_GWCBLang(pctsdata->u.gw_cb.cb_page_hdr.cb_dcs.language);
               
               // PID Ignored for GSM / WCDMA Broadcast Message

               pce->dwMsgClass = pctsdata->u.gw_cb.cb_page_hdr.cb_dcs.msg_class;

               pce->bIsCompressed = pctsdata->u.gw_cb.cb_page_hdr.cb_dcs.is_compressed;

               pce->dwRawDCS = pctsdata->u.gw_cb.cb_page_hdr.cb_dcs.raw_dcs_data;
 
               pce->dwGWCommand = 0xFFFFFFFF;

               FARF(SMS, ("tid = %d, nText = %d, nEncoding = %d", pce->dwTransactionId, pce->nText, pce->nEncoding));
               FARF(SMS, ("Text = %s", pce->pText));
               return(TRUE);
            }
            else
            {
               FARF(SMS, ("Failed to decode (tid=%d)", transaction_id));
               return(FALSE);
            }
         }
         else
         {
            switch (pCMsg->u.gw_message.raw_ts_data.tpdu_type)
            {
               case WMS_TPDU_DELIVER:
                  {
                     wms_gw_message_s_type * pm = (wms_gw_message_s_type *)&pCMsg->u.gw_message;

                     FARF(SMS,("SMS Len: %d, Format: %d",pm->raw_ts_data.len, pm->raw_ts_data.format));

                     // If this is a broadcast message we shall reject it.
                     if (pm->is_broadcast == TRUE) 
                     {
                        FARF(SMS, ("Rejecting SMS Broadcast Msg (tid=%d)", transaction_id));
                        return(FALSE);
                     }

                     pce->nRawTPDULength = (int)pm->raw_ts_data.len;
                     MEMCPY(pce->aRawTPDUData, pm->raw_ts_data.data, pm->raw_ts_data.len);

                     decodeStatus = wms_ts_decode(&pm->raw_ts_data, pctsdata);

                     if (decodeStatus == WMS_OK_S)
                     {
                        ts = OEMWMS_DetermineGSMMessageType(&pctsdata->u.gw_pp);

                        pce->ts = Xlate_TS(ts);
                        pce->pText = pctsdata->u.gw_pp.u.deliver.user_data.sm_data;
                        pce->nText = pctsdata->u.gw_pp.u.deliver.user_data.sm_len;

                        if (ts == WMS_TELESERVICE_WAP
#ifdef CUST_EDITION	  						
#if defined(FEATURE_QMA)
                            || ts == WMS_TELESERVICE_QMA_WPUSH
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
                            || ts == WMS_TELESERVICE_WPUSH
#endif
#endif /*CUST_EDITION*/
                            )
                        {
                           pce->nEncoding = AEESMS_ENC_OCTET;
                        }
                        else
                        {
                           switch(pctsdata->u.gw_pp.u.deliver.dcs.alphabet)
                           {
                              case WMS_GW_ALPHABET_UCS2:
                                 pce->nEncoding = Xlate_Encoding(WMS_ENCODING_UNICODE);
                                 break;

                              case WMS_GW_ALPHABET_8_BIT:
                                 pce->nEncoding = Xlate_Encoding(WMS_ENCODING_OCTET);
                                 break;

                              default:
                                 pce->nEncoding = Xlate_Encoding(WMS_ENCODING_GSM_7_BIT_DEFAULT);
                           }
                        }

                        pce->dwTimeStamp = OEMWMS_GetTimeFromWmsTimestamp(&pctsdata->u.gw_pp.u.deliver.timestamp, &pce->nTimeStampZone, FALSE);

                        // 1-1 mapping; Hence No Conversion Needed
                        pce->dwPID = pctsdata->u.gw_pp.u.deliver.pid;

                        // 1-1 mapping; Hence No Conversion Needed
                        pce->dwMsgClass = pctsdata->u.gw_pp.u.deliver.dcs.msg_class;

                        pce->bIsCompressed = pctsdata->u.gw_pp.u.deliver.dcs.is_compressed;

                        pce->dwMsgWaitingType = pctsdata->u.gw_pp.u.deliver.dcs.msg_waiting;

                        pce->dwRawDCS = pctsdata->u.gw_pp.u.deliver.dcs.raw_dcs_data;

                        if (pctsdata->u.gw_pp.u.deliver.dcs.msg_waiting_active)
                           pce->dwMsgWaitingKind = pctsdata->u.gw_pp.u.deliver.dcs.msg_waiting_kind;
                        else
                           pce->dwMsgWaitingKind = pctsdata->u.gw_pp.u.deliver.dcs.msg_waiting_kind + AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE;

                        if (OEMWMS_GetNumMsgsInfoFromGWMsg((wms_gw_deliver_s_type *)&pctsdata->u.gw_pp.u.deliver, ts, &pce->nNumMsgs) == FALSE)
                           return FALSE;
            
                        OEMWMS_GetAddress(&pctsdata->u.gw_pp.u.deliver.address, pce->szFrom, sizeof(pce->szFrom), &pce->dwMOFromMTToType, &pce->dwMOFromMTToPlan, FALSE);

                        OEMWMS_GetAddress(&pCMsg->u.gw_message.sc_address, pce->szCallback, sizeof(pce->szCallback), &pce->dwCallbackType, &pce->dwCallbackPlan, FALSE);
            
                        pce->dwTag = Xlate_Tag(pCMsg->msg_hdr.tag);
                        pce->bIsMOSMS = FALSE;
                        pce->bRequestDLAck = pctsdata->u.gw_pp.u.deliver.status_report_enabled;
                        pce->bIsSCAddress = TRUE;
                        pce->bIsGWBroadcast = FALSE;
                        pce->bIsReplyPathPresent = pctsdata->u.gw_pp.u.deliver.reply_path_present;
                        pce->bHasMoreMessageToSend = pctsdata->u.gw_pp.u.deliver.more;
                        pce->dwGWCommand = 0xFFFFFFFF;
                        FARF(SMS, ("tid = %d, nText = %d, nEncoding = %d", pce->dwTransactionId, pce->nText, pce->nEncoding));
                        FARF(SMS, ("Text = %s", pce->pText));
                        FARF(SMS, ("TimeStamp = %d, nNumMsgs = %d", pce->dwTimeStamp, pce->nNumMsgs));
                        FARF(SMS, ("SCAddress = %s", (char*)(*pce->szCallback ? pce->szCallback : "Unknown")));
                        FARF(SMS, ("PID = %d", pce->dwPID ));
                        FARF(SMS, ("From = %s", (char*)(*pce->szFrom ? pce->szFrom : "Unknown")));
                        return(TRUE);
                     }
                     else
                     {
                        FARF(SMS, ("Failed to decode (tid=%d)", transaction_id));
                        return(FALSE);
                     }
                  }

               case WMS_TPDU_SUBMIT:
                  {
                     // This will be used when a stored MO message is read from storage
                     wms_gw_message_s_type * pm = (wms_gw_message_s_type *)&pCMsg->u.gw_message;

                     FARF(SMS,("SMS Len: %d, Format: %d",pm->raw_ts_data.len, pm->raw_ts_data.format));

                     decodeStatus = wms_ts_decode(&pm->raw_ts_data, pctsdata);

                     if (decodeStatus == WMS_OK_S)
                     {
                        ts = WMS_TELESERVICE_CMT_95;
                        pce->ts = Xlate_TS(ts);
                        pce->pText = pctsdata->u.gw_pp.u.submit.user_data.sm_data;
                        pce->nText = pctsdata->u.gw_pp.u.submit.user_data.sm_len;

                        switch(pctsdata->u.gw_pp.u.submit.dcs.alphabet)
                        {
                           case WMS_GW_ALPHABET_UCS2:
                              pce->nEncoding = Xlate_Encoding(WMS_ENCODING_UNICODE);
                              break;

                           case WMS_GW_ALPHABET_8_BIT:
                              pce->nEncoding = Xlate_Encoding(WMS_ENCODING_OCTET);
                              break;

                           default:
                              pce->nEncoding = Xlate_Encoding(WMS_ENCODING_GSM_7_BIT_DEFAULT);
                        }

                        pce->dwRelativeValidity = 0xFFFFFFFF;

                        if (pctsdata->u.gw_pp.u.submit.validity.format == WMS_GW_VALIDITY_RELATIVE) {
                           pce->dwRelativeValidity = OEMWMS_GetTimeFromWmsTimestamp(&pctsdata->u.gw_pp.u.submit.validity.u.time, NULL, TRUE);
                        }
                        else if (pctsdata->u.gw_pp.u.submit.validity.format == WMS_GW_VALIDITY_ABSOLUTE) {
                           pce->dwAbsoluteValidityTimeStamp = OEMWMS_GetTimeFromWmsTimestamp(&pctsdata->u.gw_pp.u.submit.validity.u.time, &pce->nAbsoluteValidityTimeStampZone, FALSE);
                        }

                        pce->nNumMsgs = -1;

                        // 1-1 mapping; Hence No Conversion Needed
                        pce->dwPID = pctsdata->u.gw_pp.u.submit.pid;

                        // 1-1 mapping; Hence No Conversion Needed
                        pce->dwMsgClass = pctsdata->u.gw_pp.u.submit.dcs.msg_class;

                        pce->bIsCompressed = pctsdata->u.gw_pp.u.submit.dcs.is_compressed;

                        pce->dwMsgWaitingType = pctsdata->u.gw_pp.u.submit.dcs.msg_waiting;

                        pce->dwRawDCS = pctsdata->u.gw_pp.u.submit.dcs.raw_dcs_data;

                        if (pctsdata->u.gw_pp.u.submit.dcs.msg_waiting_active)
                           pce->dwMsgWaitingKind = pctsdata->u.gw_pp.u.submit.dcs.msg_waiting_kind;
                        else
                           pce->dwMsgWaitingKind = pctsdata->u.gw_pp.u.submit.dcs.msg_waiting_kind + AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE;
            
                        OEMWMS_GetAddress(&pctsdata->u.gw_pp.u.submit.address, pce->szFrom, sizeof(pce->szFrom), &pce->dwMOFromMTToType, &pce->dwMOFromMTToPlan, FALSE);

                        OEMWMS_GetAddress(&pCMsg->u.gw_message.sc_address, pce->szCallback, sizeof(pce->szCallback), &pce->dwCallbackType, &pce->dwCallbackPlan, FALSE);
            
                        pce->dwTag = Xlate_Tag(pCMsg->msg_hdr.tag);
                        pce->bIsSCAddress = TRUE;
                        pce->bRequestDLAck = pctsdata->u.gw_pp.u.submit.status_report_enabled;
                        pce->bIsMOSMS = TRUE;
                        pce->dwMsgID = pctsdata->u.gw_pp.u.submit.message_reference;
                        pce->bIsGWBroadcast = FALSE;
                        pce->bIsReplyPathPresent = pctsdata->u.gw_pp.u.submit.reply_path_present;
                        pce->dwGWCommand = 0xFFFFFFFF;

                        FARF(SMS, ("tid = %d, nText = %d, nEncoding = %d", pce->dwTransactionId, pce->nText, pce->nEncoding));
                        FARF(SMS, ("Text = %s", pce->pText));
                        FARF(SMS, ("Absolute Validity Timestamp = %d, nNumMsgs = %d", pce->dwAbsoluteValidityTimeStamp, pce->nNumMsgs));
                        FARF(SMS, ("SCAddress = %s", (char*)(*pce->szCallback ? pce->szCallback : "Unknown")));
                        FARF(SMS, ("PID = %d", pce->dwPID ));
                        FARF(SMS, ("To = %s", (char*)(*pce->szFrom ? pce->szFrom : "Unknown")));
                        return(TRUE);
                     }
                     else
                     {
                        FARF(SMS, ("Failed to decode (tid=%d)", transaction_id));
                        return(FALSE);
                     }
                  }

               case WMS_TPDU_STATUS_REPORT:
                  {
                     wms_gw_message_s_type * pm = (wms_gw_message_s_type *)&pCMsg->u.gw_message;

                     FARF(SMS,("SMS Len: %d, Format: %d",pm->raw_ts_data.len, pm->raw_ts_data.format));

                     // If this is a broadcast message we shall reject it.
                     if (pm->is_broadcast == TRUE) 
                     {
                        FARF(SMS, ("Rejecting SMS Broadcast Msg (tid=%d)", transaction_id));
                        return(FALSE);
                     }

                     pce->nRawTPDULength = (int)pm->raw_ts_data.len;
                     MEMCPY(pce->aRawTPDUData, pm->raw_ts_data.data, pm->raw_ts_data.len);

                     decodeStatus = wms_ts_decode(&pm->raw_ts_data, pctsdata);

                     if (decodeStatus == WMS_OK_S)
                     {
                        ts = OEMWMS_DetermineGSMMessageType(&pctsdata->u.gw_pp);

                        // 1-1 mapping; Hence No Conversion Needed
                        pce->dwPID = pctsdata->u.gw_pp.u.status_report.pid;

                        // 1-1 mapping; Hence No Conversion Needed
                        pce->dwMsgClass = pctsdata->u.gw_pp.u.status_report.dcs.msg_class;

                        pce->bIsCompressed = pctsdata->u.gw_pp.u.status_report.dcs.is_compressed;

                        pce->dwMsgWaitingType = pctsdata->u.gw_pp.u.status_report.dcs.msg_waiting;

                        pce->dwRawDCS = pctsdata->u.gw_pp.u.status_report.dcs.raw_dcs_data;

                        if (pctsdata->u.gw_pp.u.status_report.dcs.msg_waiting_active)
                           pce->dwMsgWaitingKind = pctsdata->u.gw_pp.u.status_report.dcs.msg_waiting_kind;
                        else
                           pce->dwMsgWaitingKind = pctsdata->u.gw_pp.u.status_report.dcs.msg_waiting_kind + AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE;
 
                        pce->ts = Xlate_TS(ts);
                        pce->pText = pctsdata->u.gw_pp.u.status_report.user_data.sm_data;
                        pce->nText = pctsdata->u.gw_pp.u.status_report.user_data.sm_len;
                        pce->bIsDLAck = TRUE;
                        pce->dwMsgID = pctsdata->u.gw_pp.u.status_report.message_reference;
                        pce->bIsGWBroadcast = FALSE;

                        if (ts == WMS_TELESERVICE_WAP
#ifdef CUST_EDITION	  						
#if defined(FEATURE_QMA)
                            || ts == WMS_TELESERVICE_QMA_WPUSH
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
                            || ts == WMS_TELESERVICE_WPUSH
#endif
#endif /*CUST_EDITION*/
                            )
                        {
                           pce->nEncoding = AEESMS_ENC_OCTET;
                        }
                        else
                        {
                           switch(pctsdata->u.gw_pp.u.status_report.dcs.alphabet)
                           {
                              case WMS_GW_ALPHABET_UCS2:
                                 pce->nEncoding = Xlate_Encoding(WMS_ENCODING_UNICODE);
                                 break;

                              case WMS_GW_ALPHABET_8_BIT:
                                 pce->nEncoding = Xlate_Encoding(WMS_ENCODING_OCTET);
                                 break;

                              default:
                                 pce->nEncoding = Xlate_Encoding(WMS_ENCODING_GSM_7_BIT_DEFAULT);
                           }
                        }

                        pce->dwTimeStamp = OEMWMS_GetTimeFromWmsTimestamp(&pctsdata->u.gw_pp.u.status_report.timestamp, &pce->nTimeStampZone, FALSE);
            
                        OEMWMS_GetAddress(&pctsdata->u.gw_pp.u.status_report.address, pce->szFrom, sizeof(pce->szFrom), &pce->dwMOFromMTToType, &pce->dwMOFromMTToPlan, FALSE);

                        OEMWMS_GetAddress(&pCMsg->u.gw_message.sc_address, pce->szCallback, sizeof(pce->szCallback), &pce->dwCallbackType, &pce->dwCallbackPlan, FALSE);
            
                        pce->dwTag = Xlate_Tag(pCMsg->msg_hdr.tag);
                        pce->bIsMOSMS = FALSE;
                        pce->bIsSCAddress = TRUE;
                        pce->bHasMoreMessageToSend = pctsdata->u.gw_pp.u.status_report.more;
                        pce->dwMsgDischargeTime = OEMWMS_GetTimeFromWmsTimestamp(&pctsdata->u.gw_pp.u.status_report.discharge_time, &pce->nDischargeTimeZone, FALSE);
                        pce->dwReportStatus = pctsdata->u.gw_pp.u.status_report.tp_status;
                        pce->dwGWCommand = 0xFFFFFFFF;

                        FARF(SMS, ("tid = %d, nText = %d, nEncoding = %d", pce->dwTransactionId, pce->nText, pce->nEncoding));
                        FARF(SMS, ("Text = %s", pce->pText));
                        FARF(SMS, ("TimeStamp = %d, nNumMsgs = %d", pce->dwTimeStamp, pce->nNumMsgs));
                        FARF(SMS, ("SCAddress = %s", (char*)(*pce->szCallback ? pce->szCallback : "Unknown")));
                        FARF(SMS, ("PID = %d", pce->dwPID ));
                        FARF(SMS, ("From = %s", (char*)(*pce->szFrom ? pce->szFrom : "Unknown")));
                        return(TRUE);
                     }
                     else
                     {
                        FARF(SMS, ("Failed to decode (tid=%d)", transaction_id));
                        return(FALSE);
                     }
                  }

               case WMS_TPDU_COMMAND:
                  {
                     wms_gw_message_s_type * pm = (wms_gw_message_s_type *)&pCMsg->u.gw_message;

                     FARF(SMS,("SMS Len: %d, Format: %d",pm->raw_ts_data.len, pm->raw_ts_data.format));

                     decodeStatus = wms_ts_decode(&pm->raw_ts_data, pctsdata);

                     if (decodeStatus == WMS_OK_S)
                     {
                        // Start populating the Command Message.
                        wms_gw_command_s_type * pCommand = &pctsdata->u.gw_pp.u.command;
                     
                        // Request Status Report
                        pce->bRequestDLAck = pCommand->status_report_enabled;

                        // TP-MR
                        pce->dwMsgID = pCommand->message_reference;

                        // TP-MN
                        pce->dwMsgID |= (pCommand->message_number << 16);

                        // TP-DA Destination Address
                        OEMWMS_GetAddress(&pCommand->address, pce->szFrom, sizeof(pce->szFrom), &pce->dwMOFromMTToType, &pce->dwMOFromMTToPlan, FALSE);

                        // TP-PID Protocol Identifier
                        pce->dwPID = pCommand->pid;

                        // TP-CT GW Command
                        pce->dwGWCommand = pCommand->command;
                     
                        // TP-CDL
                        pce->nText = pCommand->command_data_len;

                        // TP-CD
                        pce->pText = pCommand->command_data;

                        // TP-UDHI
                        pce->nEncoding = pCommand->user_data_header_present;

                        pce->dwRelativeValidity = 0xFFFFFFFF;            
                        pce->dwTag = Xlate_Tag(pCMsg->msg_hdr.tag);
                        pce->bIsMOSMS = TRUE;
                        FARF(SMS, ("PID = %d", pce->dwPID ));
                        FARF(SMS, ("To = %s", (char*)(*pce->szFrom ? pce->szFrom : "Unknown")));
                        return(TRUE);
                     }
                     else
                     {
                        FARF(SMS, ("Failed to decode (tid=%d)", transaction_id));
                        return(FALSE);
                     }
                  }

               case WMS_TPDU_SUBMIT_REPORT_ACK:
                  FARF(SMS, ("Rejecting SMS-SUBMIT-REPORT-ACK TPDU (tid=%d)", transaction_id));
                  return(FALSE);

               case WMS_TPDU_SUBMIT_REPORT_ERROR:
                  FARF(SMS, ("Rejecting SMS-SUBMIT-REPORT-ERROR TPDU (tid=%d)", transaction_id));
                  return(FALSE);

               default:
                  FARF(SMS, ("Rejecting TPDU Type = %d (tid=%d)", pCMsg->u.gw_message.raw_ts_data.tpdu_type, transaction_id));
                  return(FALSE);
            }
         }
      }
      else
      {
         // Read from GW Template
         wms_gw_template_s_type * pm = (wms_gw_template_s_type *)&pCMsg->u.gw_template;

         pce->bIsMOSMS = TRUE;

         if (pm->alpha_id.len)
         {
            pce->nAlphaIDLength = pm->alpha_id.len;
            MEMCPY(pce->aAlphaID, pm->alpha_id.data, pm->alpha_id.len);
         }

         if (pm->mask & WMS_GW_TEMPLATE_MASK_DCS)
         {
            pce->dwMsgClass = pm->dcs.msg_class;
            pce->bIsCompressed = pm->dcs.is_compressed;
            pce->dwMsgWaitingType = pm->dcs.msg_waiting;
            pce->dwRawDCS = pm->dcs.raw_dcs_data;
            if (pm->dcs.msg_waiting_active)
               pce->dwMsgWaitingKind = pm->dcs.msg_waiting_kind;
            else
               pce->dwMsgWaitingKind = pm->dcs.msg_waiting_kind + AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE;

            switch(pm->dcs.alphabet)
            {
               case WMS_GW_ALPHABET_UCS2:
                  pce->nEncoding = Xlate_Encoding(WMS_ENCODING_UNICODE);
                  break;

               case WMS_GW_ALPHABET_8_BIT:
                  pce->nEncoding = Xlate_Encoding(WMS_ENCODING_OCTET);
                  break;

               default:
                  pce->nEncoding = Xlate_Encoding(WMS_ENCODING_GSM_7_BIT_DEFAULT);
            }
         }

         if (pm->mask & WMS_GW_TEMPLATE_MASK_VALIDITY)
         {
            pce->dwRelativeValidity = OEMWMS_GetTimeFromWmsTimestamp(&pm->relative_validity, NULL, TRUE);
         }
         else {
            pce->dwRelativeValidity = 0xFFFFFFFF;
         }

         if (pm->mask & WMS_GW_TEMPLATE_MASK_PID)
         {
            // 1-1 Mapping; No Conversion Needed
            pce->dwPID = pm->pid;
         }

         if (pm->mask & WMS_GW_TEMPLATE_MASK_DEST_ADDR)
         {
            OEMWMS_GetAddress(&pm->dest_addr, pce->szFrom, sizeof(pce->szFrom), &pce->dwMOFromMTToType, &pce->dwMOFromMTToPlan, FALSE);
         }
         
         if (pm->mask & WMS_GW_TEMPLATE_MASK_SC_ADDR)
         {
            OEMWMS_GetAddress(&pm->sc_addr, pce->szCallback, sizeof(pce->szCallback), &pce->dwCallbackType, &pce->dwCallbackPlan, FALSE);
         }
         
         pce->dwTag = Xlate_Tag(pCMsg->msg_hdr.tag);
         pce->bIsSCAddress = TRUE;
         pce->dwGWCommand = 0xFFFFFFFF;
         FARF(SMS, ("TimeStamp = %d, nNumMsgs = %d", pce->dwTimeStamp, pce->nNumMsgs));
         FARF(SMS, ("SCAddress = %s", (char*)(*pce->szCallback ? pce->szCallback : "Unknown")));
         FARF(SMS, ("PID = %d", pce->dwPID ));
         FARF(SMS, ("To = %s", (char*)(*pce->szFrom ? pce->szFrom : "Unknown")));
         return(TRUE);
      }
   }
   return(FALSE);
}

/*===========================================================================

  Gets information about message being already stored

===========================================================================*/
void OEMWMS_GetAutoStoreInformation( wms_client_message_s_type * pCMsg, SMSCacheEntry *pce )
{
   int nStrIdx;

   if (!pCMsg || !pce)
      return;

   nStrIdx = Xlate_StorageType(pCMsg->msg_hdr.mem_store, pCMsg->msg_hdr.tag);

   if ((0 <= nStrIdx) && (nStrIdx < OEMSMS_STORAGE_LAST) &&
       (WMS_DUMMY_MESSAGE_INDEX != pCMsg->msg_hdr.index))
   {
      pce->bIsStored = TRUE;
      pce->st = nStrIdx;
      pce->dwIndex = pCMsg->msg_hdr.index;
   }
}

/*===========================================================================

  Translates DMSS Teleservice ID to OEM Teleservice ID

===========================================================================*/
static uint32 Xlate_TS(wms_teleservice_e_type ts)
{
   TsIdMap* pe;
   int i,nEntries = sizeof(gsTsIdMap) / sizeof(TsIdMap);

   for(i = 0, pe = (TsIdMap *)gsTsIdMap; i < nEntries; pe++,i++)
   {
      if (pe->DMSS_TsId == ts)
         return(pe->OEM_TsId);
   }

   // Assume DMSS uses standard TS IDs so that we dont need a mapping table and 
   // we can return the same TS ID
   return(ts); 
}

/*===========================================================================

  Translates OEM Teleservice ID to DMSS Teleservice ID

===========================================================================*/
static wms_teleservice_e_type Xlate_2_NativeTS(uint32 ts)
{
   TsIdMap* pe;
   int i,nEntries = sizeof(gsTsIdMap) / sizeof(TsIdMap);

   for(i = 0, pe = (TsIdMap *)gsTsIdMap; i < nEntries; pe++,i++)
   {
      if (pe->OEM_TsId == ts)
         return(pe->DMSS_TsId);
   }

   // Assume DMSS uses standard TS IDs so that we dont need a mapping table and 
   // we can return the same TS ID
   return((wms_teleservice_e_type)ts); 
}

/*===========================================================================

   Translation of Priority Codes from DMSS to AEE

===========================================================================*/
static int Xlate_Priority(wms_priority_e_type priority)
{
   PriorityMap * pe;
   int i,nEntries = sizeof(gsPriorityMap) / sizeof(PriorityMap);

   for(i = 0, pe = (PriorityMap *)gsPriorityMap; i < nEntries; pe++,i++)
   {
      if (pe->OEM_Priority == priority)
         return(pe->AEE_Priority);

   }

   return(SMS_PRIORITY_NORMAL); // Default   

}

/*===========================================================================

   Translation of Client Status from AEE to DMSS for CDMA

===========================================================================*/
static wms_cdma_tl_status_e_type Xlate_CDMAClientStatus(int cs)
{
   CDMAClientStatusMap * pe;
   int i,nEntries = sizeof(gsCDMAClientStatusMap) / sizeof(CDMAClientStatusMap);

   for(i = 0, pe = (CDMAClientStatusMap *)gsCDMAClientStatusMap; i < nEntries; pe++,i++)
   {
      if (pe->AEE_ClientStatus == cs)
         return(pe->OEM_ClientStatus);
   }
   
   // If value did not match, just pass through...
   return((wms_cdma_tl_status_e_type)cs); // Default   
}

/*===========================================================================

   Translation of Client Status from AEE to DMSS for GW

===========================================================================*/
static wms_tp_cause_e_type Xlate_GWClientStatus(int cs)
{
   GWClientStatusMap * pe;
   int i,nEntries = sizeof(gsGWClientStatusMap) / sizeof(GWClientStatusMap);

   for(i = 0, pe = (GWClientStatusMap *)gsGWClientStatusMap; i < nEntries; pe++,i++)
   {
      if (pe->AEE_ClientStatus == cs)
         return(pe->OEM_ClientStatus);
   }
   
   // If value did not match, just pass through...
   return((wms_tp_cause_e_type)cs); // Default   
}

/*===========================================================================

  Translation of Privacy Codes from DMSS to AEE

===========================================================================*/
static int Xlate_Privacy(wms_privacy_e_type privacy)
{
   PrivacyMap * pe;
   int i,nEntries = sizeof(gsPrivacyMap) / sizeof(PrivacyMap);

   for(i = 0, pe = (PrivacyMap *)gsPrivacyMap; i < nEntries; pe++,i++)
   {
      if (pe->OEM_Privacy == privacy)
         return(pe->AEE_Privacy);
   }

   return(SMS_PRIVACY_NORMAL); // Default   
}

/*===========================================================================

  Translation of Language Codes from DMSS to AEE

===========================================================================*/
static uint32 Xlate_Lang(wms_language_e_type lang)
{
   LangMap * pe;
   int i,nEntries = sizeof(gsLangMap) / sizeof(LangMap);

   for(i = 0, pe = (LangMap *)gsLangMap; i < nEntries; pe++,i++)
   {
      if (pe->OEM_Lang == lang)
         return(pe->AEE_Lang);
   }

   // Assume DMSS uses our LanguageCodes so that we dont need a mapping table and 
   // we can return the same code
   return(lang); 
}

/*===========================================================================

  Translation of GW CB Language Codes from DMSS to AEE

===========================================================================*/
static uint32 Xlate_GWCBLang(wms_gw_cb_language_e_type lang)
{
   GWCBLangMap * pe;
   int i,nEntries = sizeof(gsGWCBLangMap) / sizeof(GWCBLangMap);

   for(i = 0, pe = (GWCBLangMap *)gsGWCBLangMap; i < nEntries; pe++,i++)
   {
      if (pe->OEM_Lang == lang)
         return(pe->AEE_Lang);
   }

   // Assume DMSS uses our LanguageCodes so that we dont need a mapping table and 
   // we can return the same code
   return(lang); 
}

/*===========================================================================

  Translation of Encoding Codes from DMSS to AEE

===========================================================================*/
static int Xlate_Encoding(wms_user_data_encoding_e_type enc)
{
   EncMap * pe;
   int i,nEntries = sizeof(gsEncMap) / sizeof(EncMap);

   for(i = 0, pe = (EncMap *)gsEncMap; i < nEntries; pe++,i++)
   {
      if (pe->OEM_Enc == enc)
         return(pe->AEE_Enc);

   }

   // We don't understand this encoding. Anyway, pass it to client and hope
   // that client understands it
   return(enc); 
}

/*===========================================================================

  This function converts the Time from Julian format to seconds format.

===========================================================================*/
uint32 OEMWMS_GetTimeFromWmsTimestamp(wms_timestamp_s_type *pTime, int *ptz, boolean bRelative)
{
   FARF(SMS, ("==>  OEMWMS_GetTimeFromWmsTimestamp"));

   if (bRelative)
   {
      return (BCD2INT(pTime->year)*ONE_YEAR_SECS +
              BCD2INT(pTime->month)*ONE_MONTH_SECS + 
              BCD2INT(pTime->day)*ONE_DAY_SECS + 
              BCD2INT(pTime->hour)*ONE_HOUR_SECS + 
              BCD2INT(pTime->minute)*ONE_MINUTE_SECS + 
              BCD2INT(pTime->second));
   }
   else
   {   
      uint8 tmp;
      clk_julian_type jt;

      MEMSET(&jt, 0, sizeof(jt));

      tmp = BCD2INT(pTime->year);
   
      // If the year is above 95 it we assume it is in 20 th century(1900+ year
      // otherwise we assume it is in 21 century(2000+year)
      jt.year = (tmp <= 95) ? (YEAR_2000 + tmp) : (YEAR_1900 + tmp);

      jt.month = BCD2INT(pTime->month);
      jt.day = BCD2INT(pTime->day);
      jt.hour = BCD2INT(pTime->hour);
      jt.minute = BCD2INT(pTime->minute);
      jt.second = BCD2INT(pTime->second);

      if (ptz)
         *ptz = pTime->timezone;

      return clk_julian_to_secs(&jt);
   }
}


/*===========================================================================

  This function converts the Time from seconds format to julian format for CDMA
  messages.

===========================================================================*/
static void OEMWMS_SetTimeToWmsTimestamp(wms_timestamp_s_type *pTime, uint32 ts, int tz, boolean bRelative)
{   
   FARF(SMS, ("==>  OEMWMS_SetTimeToWmsTimestamp"));

   if (bRelative)
   {
      pTime->year = INT2BCD((uint8)(ts/ONE_YEAR_SECS));
      ts %= ONE_YEAR_SECS;

      pTime->month = INT2BCD((uint8)(ts/ONE_MONTH_SECS));
      ts %= ONE_MONTH_SECS;

      pTime->day = INT2BCD((uint8)(ts/ONE_DAY_SECS));
      ts %= ONE_DAY_SECS;

      pTime->hour = INT2BCD((uint8)(ts/ONE_HOUR_SECS));
      ts %= ONE_HOUR_SECS;

      pTime->minute = INT2BCD((uint8)(ts/ONE_MINUTE_SECS));
      ts %= ONE_MINUTE_SECS;

      pTime->second = INT2BCD((uint8)ts);
   }
   else
   {
      clk_julian_type jt;

      MEMSET(&jt, 0, sizeof(jt));

      clk_secs_to_julian(ts, &jt);
      pTime->year = INT2BCD((jt.year >= 2000) ? (jt.year - 2000) : (jt.year - 1900));   
      pTime->month = INT2BCD(jt.month); 
      pTime->day = INT2BCD(jt.day);
      pTime->hour = INT2BCD(jt.hour);
      pTime->minute = INT2BCD(jt.minute);
      pTime->second = INT2BCD(jt.second);

      pTime->timezone = tz;
   }
}

/*===========================================================================

  Gets number of messages information from the WMS Client Bearer Data.

===========================================================================*/
static boolean OEMWMS_GetNumMsgsInfoFromCDMABDMsg 
(
   wms_client_bd_s_type * pcd, 
   wms_teleservice_e_type ts, 
   int * pnNumMsgs
)
{
   FARF(SMS, ("==>  OEMWMS_GetNumMsgsInfoFromCDMABDMsg"));

   *pnNumMsgs = -1;

   if (ts == WMS_TELESERVICE_VMN_95 || ts == WMS_TELESERVICE_MWI) 
   {
      // Check if Num Messages information is present in the Bearer Data
      // Structure.
      if (pcd->mask & WMS_MASK_BD_NUM_OF_MSGS) 
      {
         *pnNumMsgs = pcd->num_messages;
         FARF(SMS, ("Number of voicemail messages =%d", *pnNumMsgs));
      }
      else if ((pcd->message_id.udh_present) &&
               (pcd->mask & WMS_MASK_BD_USER_DATA) &&
               (pcd->user_data.num_headers))
      {
         int i;

         for(i = 0; i < pcd->user_data.num_headers; i++) 
         {
            if ((pcd->user_data.headers[i].header_id == WMS_UDH_SPECIAL_SM) &&
               (pcd->user_data.headers[i].u.special_sm.msg_waiting_kind == WMS_GW_MSG_WAITING_VOICEMAIL)) 
            {
               *pnNumMsgs = pcd->user_data.headers[i].u.special_sm.message_count;
               break;
            }
         }
      }
   }
   
   return TRUE;
}

/*===========================================================================

  Gets number of messages information from the WMS GW Message Data.

===========================================================================*/
static boolean OEMWMS_GetNumMsgsInfoFromGWMsg 
(
   wms_gw_deliver_s_type * pData,
   wms_teleservice_e_type ts,
   int * pnNumMsgs
)
{
   FARF(SMS, ("==>  OEMWMS_GetNumMsgsInfoFromGWMsg"));

   *pnNumMsgs = -1;

   if (ts == WMS_TELESERVICE_VMN_95) 
   {
      // Determine number of Voice Messages.

      // There are multiple ways in which a GSM SMS message could indicate 
      // Voice Mail Notification, if SMI does not indicate VMN, then check 
      // the DCS to see if it indicates Voice Mail Notification (see Section
      // 4 - Data Encoding Scheme of TS 23.038)

      /* CPHS VM: Check for clear or set indicator to determine if voicemail is active */
      if ( wms_cfg_check_cphs_msg (&pData->address) )
      {      
         if (pData->address.digits[0] == 0x10)
         {
           *pnNumMsgs = 0;
         }
         else if (pData->address.digits[0] == 0x11)
         {
           // Since we do not have information about how many 
           // Messages are awaiting we shall set nMessages to a value of 1.
           *pnNumMsgs = 1;
         }
      }

      if ((pData->dcs.msg_waiting_kind == WMS_GW_MSG_WAITING_VOICEMAIL) &&
          (pData->dcs.msg_waiting_active == TRUE)) 
      {
         // Since we do not have information about how many 
         // Messages are awaiting we shall set nMessages to a value of 1.
         *pnNumMsgs = 1;
      }
      
      // If Protocol ID is PID_RETURN_CALL, it indicates a Voice Mail 
      // Notification with Number of Messages being defaulted to 1
      
      if (WMS_PID_RETURN_CALL == pData->pid)
      {
         // Since we do not have information about how many 
         // Messages are awaiting we shall set nMessages to a value of 1.
         *pnNumMsgs = 1;
      }

      // If the User Data Header has a Special Message Indication 
      // Information Element, then process the IE to extract the 
      // number of Messages Information. See section 9.2.3.24.2 
      // of TS 23.040.

      if (pData->user_data_header_present)
      {
         int i;

         for(i = 0; i < pData->user_data.num_headers; i++) 
         {
            if ((pData->user_data.headers[i].header_id == WMS_UDH_SPECIAL_SM) &&
               (pData->user_data.headers[i].u.special_sm.msg_waiting_kind == WMS_GW_MSG_WAITING_VOICEMAIL)) 
            {
               *pnNumMsgs = pData->user_data.headers[i].u.special_sm.message_count;
               break;
            }
         }
      }
   }
   return TRUE;
}

/*===========================================================================

  Sets number of messages information to the WMS GW Message Data.

===========================================================================*/
static void OEMWMS_SetNumMsgsInfoToGWMsg 
(
   wms_gw_deliver_s_type * pData,
   wms_teleservice_e_type ts,
   int nNumMsgs
)
{
   FARF(SMS, ("==>  OEMWMS_SetNumMsgsInfoToGWMsg"));

   // This function is used to store a MT message. We set deliver message
   // parameters from which we can get number of messages when msg is read

   if ((WMS_PID_RETURN_CALL == pData->pid) && (nNumMsgs < 2))
   {
      // Ignore this case since Voice Mail is indicated by
      // PID as Return Call
      // No Need to fill up nNumMsgs if it is less than 2 since
      // on Reading back it will always default to 1.
      
      return;
   }

   if (   (WMS_GW_MSG_WAITING_VOICEMAIL == pData->dcs.msg_waiting_kind)
        &&(pData->dcs.msg_waiting_active) 
        &&(nNumMsgs < 2) )
   {
      // Ignore this case since Voice Mail is indicated by
      // DCS as VoiceMail Waiting
      // No Need to fill up nNumMsgs if it is less than 2 since
      // on Reading back it will always default to 1.
      
      return;
   }

   if (ts == WMS_TELESERVICE_VMN_95) 
   {
      pData->user_data.num_headers = 1;
      pData->user_data.headers[0].header_id = WMS_UDH_SPECIAL_SM;
      pData->user_data.headers[0].u.special_sm.msg_waiting_kind = WMS_GW_MSG_WAITING_VOICEMAIL;
    
      if (nNumMsgs < 0)
      {
         // Number of Messages Not Specified; Defaulting to 1
         nNumMsgs = 1;
      }
      pData->user_data.headers[0].u.special_sm.message_count = nNumMsgs;
      pData->user_data_header_present = TRUE;
   }
}

/*===========================================================================

  This function gets the Callback number from the WMS Client Bearer Data
  in the specified buffer.              

===========================================================================*/
static boolean OEMWMS_GetCallBackNumberFromCDMABDMsg
(
   wms_client_bd_s_type *pcd,
   wms_teleservice_e_type ts,
   char * pBuffer,
   int nSize,
   AEESMSNumberType * pNumberType,
   AEESMSNumberPlan * pNumberPlan
)
{
   FARF(SMS, ("==>  OEMWMS_GetCallBackNumberFromCDMABDMsg"));

   // Check if the WMS bd message has Callback number 
   if (pcd->mask & WMS_MASK_BD_CALLBACK) 
   {
      // If the incoming message is  IS91 Page message, we shall
      // re-compute the number of digits in the callback number by
      // ignoring the trailing '0x0' digits. (A real 0 is encoded in
      // the IS91 CLI message as '0xA' so we are not in danger of ignoring
      // legitimate 0 digits from the message.)

      if (ts == WMS_TELESERVICE_IS91_PAGE) 
      {
         int j = 0;

         // In an IS91 Page message, the Callback number is padded with trailing
         // zeroes, we do not want to display these digits, so we try to strip
         // the trailing zeroes out by setting the number_of_digits information
         // appropriately. E.g. 55512120000 (for 5551212)

         // Count the number of trailing zeros in the callback number
         // array and recompute the length of the callback number ignoring
         // the trailing zeros.

         for(j = pcd->callback.number_of_digits - 1; j >= 0; j--) 
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

      return OEMWMS_GetAddress(&pcd->callback, pBuffer, nSize, pNumberType, pNumberPlan, TRUE);
   }
   return TRUE;
}

/*===========================================================================

  Converts 4 bit GSM/WCDMA 4 bit mode number to ascii value             

===========================================================================*/
static byte GetGWDigit(byte bVal)
{
   byte bReturn = ' ';

   if (((0 == bVal) || (bVal >= 1)) && (bVal <= 9))
      bReturn = '0' + bVal;
   else if (bVal == 0x0a)
      bReturn = '*';
   else if (bVal == 0x0b)
      bReturn = '#';
   else if ((bVal >= 0x0c) && (bVal <= 0x0e))
      bReturn = 'a' + bVal - 0x0c;

   return bReturn;
}

/*===========================================================================

  This function gets the phone number as string copied to pBuffer              

===========================================================================*/
boolean OEMWMS_GetAddress
(
   wms_address_s_type * pAddr,
   char * pBuffer, /* return null terminated string here */
   int nSize,
   AEESMSNumberType * pNumberType,
   AEESMSNumberPlan * pNumberPlan,
   boolean bCDMA
)
{
   int i;
   byte * pDst;
   int nOffset = 0;

   FARF(SMS, ("==>  OEMWMS_GetAddress"));

   if (!pAddr)
      return FALSE;

   if (nSize <= 0) {
	   return FALSE;
   }

   if (0 < nSize) {
	   nSize--;   /* leave space for null termination */
   }

   if (pNumberType)
      *pNumberType = pAddr->number_type;

   if (pNumberPlan)
      *pNumberPlan = pAddr->number_plan;

   pDst = (byte*)pBuffer;

   if (pAddr->number_type == WMS_NUMBER_INTERNATIONAL) 
   {
	   if (nOffset < nSize) {
		   pDst[nOffset] = DIALER_PLUS_CHAR;
		   nOffset++;
	   }
   }

   if (pAddr->digit_mode == WMS_DIGIT_MODE_8_BIT) 
   {
      // Each byte in the callback.digits array is a ascii representation
      // of a digit, so we can directly copy each byte into the cbDigits
      // array.
	  int nDigits = MIN((nSize-nOffset),pAddr->number_of_digits);
     MEMCPY(pDst + nOffset, pAddr->digits, nDigits);
	  nOffset += nDigits;
   }   
   else if (pAddr->digit_mode == WMS_DIGIT_MODE_4_BIT) 
   {
	   byte bVal;
      int nDigits = MIN((nSize-nOffset),pAddr->number_of_digits);
      
	   for (i = 0; i < nDigits; i++) {
		   bVal = (byte)pAddr->digits[i] & 0x0f;
		   pDst[nOffset] = (bCDMA ? GetDigit(bVal) : GetGWDigit(bVal));
		   nOffset++;
      }
   } 
   else 
   {
      // Unknown Digit mode
      FARF(SMS, ("Unknown Digit Mode in Address %d", pAddr->digit_mode));
      return FALSE;
   }

   // If the number is all '0's then report error
   {
	   boolean bCBNumIsAllZeros = FALSE;
	   for(i = 0; i < nOffset; i++) 
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
   }

   pDst[nOffset] = '\0'; /* null terminate. No need to check 
						 ** against size cuz the space was guaranteed. */
   return TRUE;
}

/*===========================================================================
   This function determines if the given PID is a Valid Text PID for 
   GSM / WCDMA Messages

   Note: This Needs to be changed by OEMs if they need to interpret 
   different PIDs as text Messages
===========================================================================*/
boolean OEMWMS_IsValidTextPID
(
   wms_pid_e_type uPID
)
{
   boolean bRetVal = FALSE;
   
   switch (uPID)
   {
      case WMS_PID_DEFAULT:
      case WMS_PID_IMPLICIT:
      case WMS_PID_TELEX:
      case WMS_PID_G3_FAX:
      case WMS_PID_G4_FAX:
      case WMS_PID_VOICE_PHONE:
      case WMS_PID_ERMES:
      case WMS_PID_VIDEOTEX:
      case WMS_PID_TELETEX_UNSPEC:
      case WMS_PID_TELETEX_PSPDN:
      case WMS_PID_TELETEX_CSPDN:
      case WMS_PID_TELETEX_PSTN:
      case WMS_PID_TELETEX_ISDN:
      case WMS_PID_UCI:
      case WMS_PID_MSG_HANDLING:
      case WMS_PID_X400:
      case WMS_PID_INTERNET_EMAIL:
      case WMS_PID_SC_SPECIFIC_1:
      case WMS_PID_SC_SPECIFIC_2:
      case WMS_PID_SC_SPECIFIC_3:
      case WMS_PID_SC_SPECIFIC_4:
      case WMS_PID_SC_SPECIFIC_5:
      case WMS_PID_SC_SPECIFIC_6:
      case WMS_PID_SC_SPECIFIC_7:
      case WMS_PID_GSM_UMTS:
      case WMS_PID_SM_TYPE_0:
      case WMS_PID_REPLACE_SM_1:
      case WMS_PID_REPLACE_SM_2:
      case WMS_PID_REPLACE_SM_3:
      case WMS_PID_REPLACE_SM_4:
      case WMS_PID_REPLACE_SM_5:
      case WMS_PID_REPLACE_SM_6:
      case WMS_PID_REPLACE_SM_7:
      case WMS_PID_EMS:
         bRetVal = TRUE;
         break;

      default:
         break;
   }

   return bRetVal;
}

/*===========================================================================
   This function determines the message type of the GSM Deliver Message and 
   maps the Message Type to a IS-637 Teleservice Id.
===========================================================================*/
static wms_teleservice_e_type OEMWMS_DetermineGSMMessageType
(
   wms_gw_pp_ts_data_s_type *pgw_pp
)
{
   wms_teleservice_e_type ts = WMS_TELESERVICE_UNKNOWN;
   wms_gw_dcs_s_type * pdcs = NULL;
   wms_gw_user_data_s_type * puserdata = NULL;
   boolean bUDHPresent = FALSE;
   wms_pid_e_type pid;

   if (!pgw_pp) 
   {
      FARF(SMS, ("Null pMsg"));
      goto Done;
   }

   if (pgw_pp->tpdu_type == WMS_TPDU_DELIVER)
   {
      pdcs = &pgw_pp->u.deliver.dcs;
      puserdata = &pgw_pp->u.deliver.user_data;

      if ((0 == puserdata->sm_data[0]) &&
          (11 == puserdata->sm_data[5]) &&
          (132 == puserdata->sm_data[6]))
      {
         FARF(SMS, ("GSM WAP Push Message Received"));
#if defined(FEATURE_QMA)
         ts =  WMS_TELESERVICE_QMA_WPUSH;
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
         ts =  WMS_TELESERVICE_WPUSH;
#else
         ts = WMS_TELESERVICE_WAP;
#endif
         goto Done;
      }
      else if (wms_cfg_check_cphs_msg(&pgw_pp->u.deliver.address))
      {
         FARF(SMS, ("CPHS VMN Message Received"));
         ts = WMS_TELESERVICE_VMN_95;
         goto Done;
      }
      else
      {
         bUDHPresent = pgw_pp->u.deliver.user_data_header_present;
         pid = pgw_pp->u.deliver.pid;

         FARF(SMS, ("MT GSM: more = %d, RPP = %d", pgw_pp->u.deliver.more, pgw_pp->u.deliver.reply_path_present));
      
         FARF(SMS, ("MT GSM: SRE = %d", pgw_pp->u.deliver.status_report_enabled));
      }
   }
   else if (pgw_pp->tpdu_type == WMS_TPDU_STATUS_REPORT)
   {
      pdcs = &pgw_pp->u.status_report.dcs;
      puserdata = &pgw_pp->u.status_report.user_data;

      if ((0 == puserdata->sm_data[0]) &&
          (11 == puserdata->sm_data[5]) &&
          (132 == puserdata->sm_data[6]))
      {
         FARF(SMS, ("GSM WAP Push Message Received"));
#ifdef CUST_EDITION	  		 
#if defined(FEATURE_QMA)
         ts =  WMS_TELESERVICE_QMA_WPUSH;
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
         ts =  WMS_TELESERVICE_WPUSH;
#else
         ts = WMS_TELESERVICE_WAP;
#endif
#endif /*CUST_EDITION*/
         goto Done;
      }
      else
      {
         bUDHPresent = pgw_pp->u.status_report.user_data_header_present;
         pid = pgw_pp->u.status_report.pid;
      }
   }
   else
   {
      FARF(SMS, ("Invalid Message Type"));
      goto Done;
   }

   FARF(SMS, ("UDH = %d, pid = %d", bUDHPresent, pid));

   FARF(SMS, ("MT GSM DCS: MsgClass = %d, compr = %d, alpha = %d",
              pdcs->msg_class, pdcs->is_compressed, pdcs->alphabet));
 
   FARF(SMS, ("MT GSM DCS: MW = %d, MWA = %d, MWK = %d",
              pdcs->msg_waiting, pdcs->msg_waiting_active, pdcs->msg_waiting_kind));
 
   // The message type shall be determined by examining the parameters TP-PID, TP-DCS, 
   // TP-UDHI and TP-UD. For detailed information about these parameters refer to documents 
   // 3GPP TS 23.040 and 3GPP TS 23.038.
   // The Message is determined to be a VoiceMailNotification Message if one of the 
   // following is TRUE.
   // 1. PID is 0x5F (3GPP TS 23.040, section 9.2.3.9)
   // 2. TP-DCS indicates MessageWaiting Coding Group & Message Kind
   //    is Voice Mail.  Section 4 of TS 23.038.
   // 3. TP-UHDI is True (User Header present) and TP-UD Header has
   //    Special Message Indication Information Element which indicates
   //    Voice Message Waiting.
   // Here we go...

   if (bUDHPresent) 
   {
      int i;

      // Initialize with EMS teleservice ID
      ts = WMS_TELESERVICE_WEMT;

      for(i = 0; i < puserdata->num_headers; i++) 
      {
         if ((puserdata->headers[i].header_id == WMS_UDH_SPECIAL_SM) &&
            (puserdata->headers[i].u.special_sm.msg_waiting_kind == WMS_GW_MSG_WAITING_VOICEMAIL)) 
         {
            FARF(SMS, ("GSM Voice Mail Notification Message Received"));
            ts =  WMS_TELESERVICE_VMN_95;
            break;
         }
         else if ((puserdata->headers[i].header_id == WMS_UDH_PORT_16) &&
            (puserdata->headers[i].u.wap_16.dest_port == 2948)) 
         {
            FARF(SMS, ("GSM WAP Push Message Received"));
#if defined(FEATURE_QMA)
            ts =  WMS_TELESERVICE_QMA_WPUSH;
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
            ts =  WMS_TELESERVICE_WPUSH;
#else
            ts =  WMS_TELESERVICE_WAP;
#endif
            break;
         }
      }
   }
   else if ((pid == WMS_PID_RETURN_CALL) ||
            ((pdcs->msg_waiting_kind == WMS_GW_MSG_WAITING_VOICEMAIL) && 
             (pdcs->msg_waiting_active == TRUE) && 
             (pdcs->msg_waiting != WMS_GW_MSG_WAITING_NONE))) 
   {         
      FARF(SMS, ("GSM Voice Mail Notification Message Received"));
      ts =  WMS_TELESERVICE_VMN_95;      
   }

   if (ts == WMS_TELESERVICE_UNKNOWN) 
   {
      // Message is not Voice Mail, so check if it is Text Message.
      // The Message is determined to be a Text Message if the following
      // is true.

      // PID = 0x0 and the TP-DCS Coding Group is not MessageWaiting
      if ((OEMWMS_IsValidTextPID(pid)) && (pdcs->msg_waiting_active == FALSE)) 
      {
         FARF(SMS, ("GSM Text Message Received"));
         ts = WMS_TELESERVICE_CMT_95;
      }
   }

   // Set the type to unknown if it is compressed
   if ((ts == WMS_TELESERVICE_CMT_95) && (pdcs->is_compressed == TRUE)) 
   {
      FARF(SMS, ("Received Compressed Text Message"));
      ts = WMS_TELESERVICE_UNKNOWN;
   }

Done:
   return ts;
}

/*===========================================================================

  Checks if node belongs to list of pending operations

===========================================================================*/
boolean OEMWSMS_IsValidNode(void* pNode, void* pList)
{
   OEMBaseMsg * pTest = NULL;
   boolean    bRetVal = FALSE;

   if (!gpWms)
      return FALSE;

   OEMCriticalSection_Enter(&gSMSCriticalSection);

   for(pTest = (OEMBaseMsg *)pList; pTest; pTest = pTest->pNext)
   {
      if (pNode == pTest)
      {
         bRetVal = TRUE;
         break;
      }
   }

   OEMCriticalSection_Leave(&gSMSCriticalSection);

   if (!bRetVal)
   {
      FARF(SMS, ("%x Invalid Node", pNode));
   }

   return bRetVal;
}


/*===========================================================================

  Called to handle MO SMS events

===========================================================================*/
static wms_status_e_type OEMWMS_MoMessageListener
(
   wms_msg_event_e_type event,
   const wms_msg_event_info_s_type * info_ptr,
   boolean * pshared 
)
{
   wms_status_e_type nRet = WMS_GENERAL_ERROR_S;
   OEMMsg *hMsg = NULL;

   if (event == WMS_MSG_EVENT_SEND) 
   {
      FARF(SMS, ("MessageListener: Send"));

      hMsg = (OEMMsg *)info_ptr->status_info.user_data;

      //Check if it is a CB for our MOSMS 
      if (OEMWSMS_IsValidNode(hMsg, gpWms->pCurMOSMS)) 
      {
         FARF(SMS, ("0x%x MO SMS Node", hMsg));
         if (pshared)
         {
            *pshared = FALSE;
         }

         // Make copy of message going to be sent
         MEMSET(&sce, 0, sizeof(sce));

         if (info_ptr->status_info.alpha_id.len)
         {
            sce.nAlphaIDLength = info_ptr->status_info.alpha_id.len;
            MEMCPY(sce.aAlphaID, info_ptr->status_info.alpha_id.data, info_ptr->status_info.alpha_id.len);
         }

         if (OEMWMS_CopyMessage(0,(wms_client_message_s_type*)&info_ptr->status_info.message,&sce)) 
         {
            OEMSMS_CopyPayloadFromCache(&sce, hMsg->pMsgPayload);
            OEMSMS_CopyPayloadDataFromCache(&sce, hMsg->pMsgData);
         }

         nRet = WMS_OK_S;
      }
   }
   else if (event == WMS_MSG_EVENT_SUBMIT_REPORT) 
   {
      FARF(SMS, ("MessageListener: Submit"));

      hMsg = (OEMMsg *)info_ptr->submit_report_info.user_data;

      if (OEMWSMS_IsValidNode(hMsg, gpWms->pCurMOSMS))
      {
         FARF(SMS, ("0x%x MO SMS Node", hMsg));

         if ((info_ptr->submit_report_info.message_mode == WMS_MESSAGE_MODE_CDMA) &&
            (info_ptr->submit_report_info.report_status == WMS_RPT_ACCESS_TOO_LARGE) &&
            (gpWms->bMOOnTrafficChannel))
         {
            if ((nRet = OEMWMS_SendSMSOnTrafficChannel(hMsg)) != WMS_OK_S)
            {
               AEESMS_SETERROR(*(hMsg->pdwStatus), AEESMS_ERRORTYPE_UNKNOWN, Xlate_wms_status_e_type_2_MOSMS_ERR(nRet));
               AEE_SYS_RESUME(hMsg->pcb);
            }
         }
         else 
         {
            if (pshared)
            {
               *pshared = FALSE;
            }
            *(hMsg->pdwStatus) = OEMWMS_GetSubmitStatus(&info_ptr->submit_report_info);
            AEE_SYS_RESUME(hMsg->pcb);

            nRet = WMS_OK_S;
         }
      }
   }

   return nRet;
}

/*===========================================================================

  A NO OP Callback function for WMS

===========================================================================*/
static void OEMWSMS_NoopCommandCallback (wms_cmd_id_e_type cmd, void* user_data, wms_cmd_err_e_type cmd_err)
{
   (void)cmd;
   (void)user_data;
   FARF(SMS, ("CMDCB: Err code %d",cmd_err));
}

/*===========================================================================

  Called to get Submit Statuse

===========================================================================*/
static uint32 OEMWMS_GetSubmitStatus(const wms_submit_report_info_s_type *submit_info)
{
   uint32 dwStatus;

   if (submit_info->message_mode == WMS_MESSAGE_MODE_CDMA)
   {
      AEESMS_SETERROR(dwStatus, 
                      Xlate_wms_error_class_e_type_2_MOSMS_ERRTYPE(submit_info->cause_info.error_class), 
                      Xlate_wms_submit_report_info_2_MOSMS_ERR(submit_info));
   }
   else if (submit_info->message_mode == WMS_MESSAGE_MODE_GW)
   {
      AEESMS_SETERROR(dwStatus, 
                      Xlate_wms_report_status_and_rp_cause_e_type_2_MOSMS_ERRTYPE(submit_info->report_status, submit_info->cause_info.cause_value), 
                      Xlate_wms_submit_report_info_2_MOSMS_ERR(submit_info));
   }
   else
   {         
      AEESMS_SETERROR(dwStatus, AEESMS_ERRORTYPE_UNKNOWN, AEESMS_ERROR_GENERAL_PROBLEM);
   }
   return dwStatus;
}

/*===========================================================================

  Constructs OEMMsg struct to be sent              

===========================================================================*/   
int OEMSMS_NewMessage(OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, AEETSysMode mode, OEMMsg** hMsgParam)
{
   OEMMsg * hMsg = NULL;
   wms_client_ts_data_s_type* clientBD = NULL;
   sys_sys_mode_e_type sysMode = SYS_SYS_MODE_NONE;
   int nErr = AEE_SUCCESS;
   wms_raw_ts_data_s_type *rawBD = NULL;   

   FARF(SMS, ("==>  OEMSMS_NewMessage"));

   switch (mode)
   {
      case AEET_SYS_MODE_CDMA:
      case AEET_SYS_MODE_HDR:
         sysMode = SYS_SYS_MODE_CDMA;
         break;

      case AEET_SYS_MODE_GSM:
      case AEET_SYS_MODE_WCDMA:
         sysMode = SYS_SYS_MODE_GSM;
         break;
   }
   
   if ((SYS_SYS_MODE_CDMA != sysMode) &&
       (SYS_SYS_MODE_GSM != sysMode))
   {
      FARF(SMS, ("Unknown Mode"));
      return EBADPARM;
   }

   hMsg = sys_malloc(sizeof(OEMMsg));

   if (!hMsg) 
   {
      FARF(SMS, ("Memory Allocation Failed!"));
      return ENOMEMORY;
   }

   clientBD = sys_malloc(sizeof(wms_client_ts_data_s_type));
   if (clientBD == NULL) {
       FARF(SMS, ("clientBD malloc failed!"));
       nErr = ENOMEMORY;
       goto Done;
   }
   
   MEMSET(clientBD, 0, sizeof(wms_client_ts_data_s_type));

   if (pMsgData->dwTag != AEESMS_TAG_MO_TEMPLATE)
   {
      rawBD = sys_malloc(sizeof(wms_raw_ts_data_s_type));
      if (rawBD == NULL) {
          FARF(SMS, ("rawBD malloc failed!"));
          nErr = ENOMEMORY;
          goto Done;
      }
      MEMSET(rawBD, 0, sizeof(wms_raw_ts_data_s_type));

      // Construct Client BD Type
      if (FALSE == OEMWMS_ConstructClientBD(pMsgData, pMsgPayload, clientBD, sysMode)) {
         nErr = EFAILED;
         goto Done;
      }

      if (WMS_OK_S != wms_ts_encode(clientBD, rawBD)) {
         nErr = EBADTASK;
         goto Done;
      }

      // Construct Client Message
      if (FALSE == OEMWMS_ConstructClientMessage( pMsgData, &hMsg->CMsg, rawBD, sysMode))
      {
         FARF(SMS, ("OEMWMS_ConstructClientMessage failed!"));
         nErr = EFAILED;
         goto Done;
      }
   }
   else
   {
      hMsg->CMsg.msg_hdr.tag = Xlate_2_NativeTag(pMsgData->dwTag);
      if (SYS_SYS_MODE_CDMA == sysMode)
      {
         // Construct Client BD Type
         if (FALSE == OEMWMS_ConstructClientBD(pMsgData, pMsgPayload, clientBD, sysMode))
         {
            FARF(SMS, ("OEMWMS_ConstructClientBD failed!"));
            nErr = EFAILED;
            goto Done;
         }

         // Construct CDMA Template
         OEMWMS_ConstructCDMATemplate(pMsgData, &hMsg->CMsg, &clientBD->u.cdma);
      }
      else
      {
         // Construct GW Template
         OEMWMS_ConstructGWTemplate(pMsgData, pMsgPayload, &hMsg->CMsg);
      }
   }

Done:

   FREEIF(clientBD);
   FREEIF(rawBD);

   if (AEE_SUCCESS != nErr) {
      FREEIF(hMsg);
   }
   else {
      hMsg->pMsgData = pMsgData;
      hMsg->pMsgPayload = pMsgPayload;
   }

   *hMsgParam = hMsg;
   return nErr;
}

/*===========================================================================

  Converts AEE encoding to DMSS encoding            

===========================================================================*/
static wms_user_data_encoding_e_type Xlate_2_NativeEncoding(int enc)
{
   EncMap * pe;
   int i,nEntries = sizeof(gsEncMap) / sizeof(EncMap);

   for(i = 0, pe = (EncMap *)gsEncMap; i < nEntries; pe++,i++)
   {
      if (pe->AEE_Enc == enc)
         return(pe->OEM_Enc);
   }
   // Assume that we are being passed an encoding understood by DMSS
   return((wms_user_data_encoding_e_type)enc); 
}

/*===========================================================================

  Provides message ID

===========================================================================*/
uint16 OEMWMS_GetUniqueSMSMessageID (void)
{   
   static uint16 id = 0;
   return id++;
}

/*===========================================================================

  Constructs the client bearer data from the message and the user data

===========================================================================*/
static boolean OEMWMS_ConstructClientBD 
(
   OEMMsgData * pMsgData,
   OEMMsgPayload * pMsgPayload,
   wms_client_ts_data_s_type * pClientTS,
   sys_sys_mode_e_type sysMode
) 
{
   FARF(SMS, ("==>  OEMWMS_ConstructClientBD"));

   switch (sysMode)
   {
      case SYS_SYS_MODE_CDMA:
         {
            wms_user_data_encoding_e_type ne;
            wms_client_bd_s_type * pClientBD = (wms_client_bd_s_type *)&pClientTS->u.cdma;

            FARF(SMS, ("CDMA Mode Client BD"));

            // Set Format
            pClientTS->format = WMS_FORMAT_CDMA;

            // Reset Mask
            pClientBD->mask = 0;
   
            if (pMsgPayload->nBytes) {
               // Set User Data   
               ne = Xlate_2_NativeEncoding(pMsgPayload->nEncoding);

               switch (ne) 
               {
                  case WMS_ENCODING_ASCII:  // 7-bit ASCII
                  case WMS_ENCODING_IA5:
                     pClientBD->user_data.encoding = ne;
                     pClientBD->user_data.is91ep_type = (wms_IS91EP_type_e_type)0;
                     pClientBD->user_data.padding_bits = pMsgPayload->nPadding;
                     pClientBD->user_data.data_len = (uint8)MIN(pMsgPayload->nBytes,WMS_CDMA_USER_DATA_MAX);
                     pClientBD->user_data.number_of_digits = (pClientBD->user_data.data_len*8 - pClientBD->user_data.padding_bits)/7;
                     // copy the data directly
                     MEMCPY((void*)pClientBD->user_data.data, (void*)pMsgPayload->data, (uint32)pClientBD->user_data.data_len);
                     break;

                  case WMS_ENCODING_UNICODE: // Unicode
                     pClientBD->user_data.encoding = ne;
                     pClientBD->user_data.is91ep_type = (wms_IS91EP_type_e_type)0;
                     pClientBD->user_data.data_len = (uint8)MIN(pMsgPayload->nBytes,WMS_CDMA_USER_DATA_MAX);
                     if (pClientBD->user_data.data_len & 0x01) // Even it
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
                     pClientBD->user_data.is91ep_type = (wms_IS91EP_type_e_type)0;
                     pClientBD->user_data.data_len = (uint8)MIN(pMsgPayload->nBytes,WMS_CDMA_USER_DATA_MAX);
                     pClientBD->user_data.padding_bits = pMsgPayload->nPadding;
                     pClientBD->user_data.number_of_digits = pClientBD->user_data.data_len;
                     // copy the data directly
                     MEMCPY((void*)pClientBD->user_data.data, (void*)pMsgPayload->data, (uint32)pClientBD->user_data.data_len);
               }
               pClientBD->mask |= WMS_MASK_BD_USER_DATA;
            }

            // Message Identifier 
            if (pMsgData->bIsDLAck)
            {
               pClientBD->message_id.type = WMS_BD_TYPE_DELIVERY_ACK;
            }
            else if (pMsgData->bIsUserAck)
            {
               pClientBD->message_id.type = WMS_BD_TYPE_USER_ACK;
            }
            else if (pMsgData->bIsReadAck)
            {
               pClientBD->message_id.type = WMS_BD_TYPE_READ_ACK;
            }
            else if (pMsgData->bIsCancelRequest)
            {
               pClientBD->message_id.type = WMS_BD_TYPE_CANCELLATION;
            }
            else
            {
               if (pMsgData->bIsMOSMS)
               {
                  pClientBD->message_id.type = WMS_BD_TYPE_SUBMIT;
               }
               else
               {
                  pClientBD->message_id.type = WMS_BD_TYPE_DELIVER;
               }
            }
            pClientBD->message_id.id_number = (pMsgData->dwMsgID ? pMsgData->dwMsgID : OEMWMS_GetUniqueSMSMessageID()); // Message Id
            pClientBD->mask |= WMS_MASK_BD_MSG_ID;                            // set mask

            // Reply option is an optional bearer data parameter
            // This subparameter should not be included in the message with
            // USER_ACK_REQ, DAK_REQ and READ_ACK_REQ all set to
            if ( pMsgData->bRequestDLAck || 
                 pMsgData->bRequestUserAck ||
                 pMsgData->bRequestReadAck )
            {
               pClientBD->reply_option.delivery_ack_requested = pMsgData->bRequestDLAck;
               pClientBD->reply_option.user_ack_requested = pMsgData->bRequestUserAck;
               pClientBD->reply_option.read_ack_requested = pMsgData->bRequestReadAck;

               pClientBD->mask |= WMS_MASK_BD_REPLY_OPTION; //BD reply option on
            }

            // Set Abs Validity
            if (pMsgData->dwAbsoluteValidityTimeStamp)
            {                  
               // Set Absolute Validity 
               pClientBD->mask |= WMS_MASK_BD_VALID_ABS;
               OEMWMS_SetTimeToWmsTimestamp(&pClientBD->validity_absolute, pMsgData->dwAbsoluteValidityTimeStamp, pMsgData->nAbsoluteValidityTimeStampZone, FALSE);
            }

            if (0xFFFFFFFF != pMsgData->dwRelativeValidity)
            {                  
               // Set Relative Validity 
               pClientBD->mask |= WMS_MASK_BD_VALID_REL;
               OEMWMS_SetTimeToWmsTimestamp(&pClientBD->validity_relative, pMsgData->dwRelativeValidity, 0, TRUE);
            }

            if (pMsgData->dwTimeStamp)
            {                 
               // Set Time Stamp 
               pClientBD->mask |= WMS_MASK_BD_MC_TIME;
               OEMWMS_SetTimeToWmsTimestamp(&pClientBD->mc_time, pMsgData->dwTimeStamp, pMsgData->nTimeStampZone, FALSE);
            }

            // Number of messages
            if (!pMsgData->bIsMOSMS)
            {
               if (pMsgData->nNumMsgs >= 0)
               {            
                  // Number of messages
                  pClientBD->num_messages = (byte)(pMsgData->nNumMsgs);
                  pClientBD->mask |= WMS_MASK_BD_NUM_OF_MSGS;
               }
            }

            // callback number
            if (*pMsgData->szCallback)
            {
               OEMWMS_SetAddress(&pClientBD->callback, pMsgData->szCallback, pMsgData->dwCallbackType, pMsgData->dwCallbackPlan, TRUE);
               if (pClientBD->callback.number_of_digits)
                  pClientBD->mask |= WMS_MASK_BD_CALLBACK;
            }

            // Priority
            if ( pMsgData->bPriorityPresent )
            {
               pClientBD->priority = Xlate_2_NativePriority(pMsgData->nPriority);
               pClientBD->mask |= WMS_MASK_BD_PRIORITY;
            }

            // Privacy
            if ( pMsgData->bPrivacyPresent )
            {
               pClientBD->privacy = Xlate_2_NativePrivacy(pMsgData->nPrivacy);
               pClientBD->mask |= WMS_MASK_BD_PRIVACY;
            }
            
            // Language
            if ( pMsgData->bLangPresent )
            {
               pClientBD->language = Xlate_2_NativeLang(pMsgData->dwLang);
               pClientBD->mask |= WMS_MASK_BD_LANGUAGE;
            }

            // PID and Message Class Ignored for CDMA Message 

            // User Data Headers
            if ((pClientBD->user_data.num_headers = OEMWMS_SetUDHData(sizeof(pClientBD->user_data.headers)/sizeof(wms_udh_s_type), pClientBD->user_data.headers, (byte*)((void*) pMsgData->data), pMsgData->nUDHLength)) != 0)
            {      
               pClientBD->message_id.udh_present = TRUE;
               pClientBD->mask |= WMS_MASK_BD_USER_DATA;
               // If no user data is present but EMS data is... set the encoding value correctly.
               if (!pMsgPayload->nBytes) 
               {         
                  wms_user_data_encoding_e_type ne;
                  ne = Xlate_2_NativeEncoding(pMsgPayload->nEncoding);
                  pClientBD->user_data.encoding = ne;
               }   
            }
              
            if (pMsgData->dwAbsoluteDeferredValidity)
            {
               OEMWMS_SetTimeToWmsTimestamp(&pClientBD->deferred_absolute, 
                                            pMsgData->dwAbsoluteDeferredValidity, 
                                            0, FALSE);
               pClientBD->mask |= WMS_MASK_BD_DEFER_ABS;
            }
              
            if (0xFFFFFFFF != pMsgData->dwRelativeDeferredValidity)
            {
               OEMWMS_SetTimeToWmsTimestamp(&pClientBD->deferred_relative, 
                                            pMsgData->dwRelativeDeferredValidity, 
                                            0, TRUE);
               pClientBD->mask |= WMS_MASK_BD_DEFER_REL;
            }

            if ((!pMsgData->bIsMOSMS) && (AEESMS_DISPLAY_MODE_NONE != pMsgData->dwMessageDisplayMode))
            {
               pClientBD->display_mode = (wms_display_mode_e_type)pMsgData->dwMessageDisplayMode;
               pClientBD->mask |= WMS_MASK_BD_DISPLAY_MODE;
            }
         }
         return TRUE;

      case SYS_SYS_MODE_GSM:
         {
            FARF(SMS, ("GSM Mode Client BD"));

            // Set Format
            pClientTS->format = WMS_FORMAT_GW_PP;

            if ((TRUE == pMsgData->bIsDLAck) || 
                (AEESMS_TAG_STATUS_RPT == pMsgData->dwTag))
            {
               wms_gw_status_report_s_type * pStatusReport = NULL;

               pClientTS->u.gw_pp.tpdu_type = WMS_TPDU_STATUS_REPORT;

               // Start populating the Status Report.
               pStatusReport = (wms_gw_status_report_s_type *)&pClientTS->u.gw_pp.u.status_report;

               // TP-MR Address
               pStatusReport->message_reference = pMsgData->dwMsgID;
                  
               // TP-RA Address
               OEMWMS_SetAddress(&pStatusReport->address, pMsgData->szMOFromMTTo, pMsgData->dwMOFromMTToType, pMsgData->dwMOFromMTToPlan, FALSE);
                  
               // Timestamp
               OEMWMS_SetTimeToWmsTimestamp(&pStatusReport->timestamp, pMsgData->dwTimeStamp, pMsgData->nTimeStampZone, FALSE);

               // TP-DT Discharge Time
               OEMWMS_SetTimeToWmsTimestamp(&pStatusReport->discharge_time, pMsgData->dwMsgDischargeTime, pMsgData->nDischargeTimeZone, FALSE);

               // TP-ST Report Status
               pStatusReport->tp_status = (wms_tp_status_e_type)pMsgData->dwReportStatus;

               // TP-MMS
               pStatusReport->more = pMsgData->bHasMoreMessageToSend;

               // TP-PID Protocol Identifier - Section 9.2.3.9 of TS 23.040.
               pStatusReport->mask |= WMS_TPDU_MASK_PID;
               pStatusReport->pid = (wms_pid_e_type)pMsgData->dwPID;

               // TP-DCS Data Coding Scheme
               pStatusReport->mask |= WMS_TPDU_MASK_DCS;
               if (0xFFFFFFFF == pMsgData->dwRawDCS)
               {
                  switch (Xlate_2_NativeEncoding(pMsgPayload->nEncoding))
                  {
                     case WMS_ENCODING_UNICODE:
                        pStatusReport->dcs.alphabet = WMS_GW_ALPHABET_UCS2;
                        break;

                     case WMS_ENCODING_GSM_7_BIT_DEFAULT:
                        pStatusReport->dcs.alphabet = WMS_GW_ALPHABET_7_BIT_DEFAULT;
                        break;

                     default:
                        pStatusReport->dcs.alphabet = WMS_GW_ALPHABET_8_BIT;
                  }
      
                  pStatusReport->dcs.msg_class     = (wms_message_class_e_type)pMsgData->dwMsgClass;
                  pStatusReport->dcs.is_compressed = pMsgData->bIsCompressed;
                  pStatusReport->dcs.msg_waiting   = (wms_gw_msg_waiting_e_type)pMsgData->dwMsgWaitingType;

                  if (((pMsgData->dwMsgWaitingKind == AEESMS_MSGWAITINGKIND_VOICEMAIL) ||
                       (pMsgData->dwMsgWaitingKind >= AEESMS_MSGWAITINGKIND_VOICEMAIL + 1)) &&
                      (pMsgData->dwMsgWaitingKind <= AEESMS_MSGWAITINGKIND_OTHER))
                  {
                     pStatusReport->dcs.msg_waiting_active = TRUE;
                     pStatusReport->dcs.msg_waiting_kind = (wms_gw_msg_waiting_kind_e_type)pMsgData->dwMsgWaitingKind;
                  }
                  else if ((pMsgData->dwMsgWaitingKind >= AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE) &&
                           (pMsgData->dwMsgWaitingKind <= AEESMS_MSGWAITINGKIND_OTHER_INACTIVE))
                  {
                     pStatusReport->dcs.msg_waiting_active = FALSE;
                     pStatusReport->dcs.msg_waiting_kind = (wms_gw_msg_waiting_kind_e_type)(pMsgData->dwMsgWaitingKind - AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE);
                  }
                  wms_ts_encode_dcs(&pStatusReport->dcs, &pStatusReport->dcs.raw_dcs_data);
               }
               else
               {
                  pStatusReport->dcs.raw_dcs_data = (uint8)pMsgData->dwRawDCS;
                  wms_ts_decode_dcs(&pStatusReport->dcs.raw_dcs_data, &pStatusReport->dcs);
               }
                  
               // TP-UD - User Data
               pStatusReport->mask |= WMS_TPDU_MASK_USER_DATA;
               pStatusReport->user_data.sm_len = (uint8)MIN(pMsgPayload->nBytes,sizeof(pStatusReport->user_data.sm_data));
               MEMCPY((void*)&pStatusReport->user_data.sm_data[0], (void*)pMsgPayload->data, (uint32)pStatusReport->user_data.sm_len);

               // User Data Headers
               if ((pStatusReport->user_data.num_headers = OEMWMS_SetUDHData(sizeof(pStatusReport->user_data.headers)/sizeof(wms_udh_s_type), pStatusReport->user_data.headers, (byte*)((void*)pMsgData->data), pMsgData->nUDHLength)) != 0)
               {      
                  pStatusReport->user_data_header_present = TRUE;
               }
            }
            else
            {
               if (pMsgData->bIsMOSMS)
               {
                  if (0xFFFFFFFF != pMsgData->dwGWCommand) {
                     wms_gw_command_s_type * pCommand = NULL;

                     // Originate a GSM Command.
                     pClientTS->u.gw_pp.tpdu_type = WMS_TPDU_COMMAND;
                     
                     // Start populating the Command Message.
                     pCommand = (wms_gw_command_s_type *)&pClientTS->u.gw_pp.u.command;
                     
                     // Request Status Report
                     pCommand->status_report_enabled = pMsgData->bRequestDLAck;

                     // TP-MR
                     if (pMsgData->dwMsgID & 0x0000FFFF) {
                        pCommand->message_reference = pMsgData->dwMsgID & 0x0000FFFF;
                     }
                     else {
                        pCommand->message_reference = OEMWMS_GetUniqueSMSMessageID() % 255;
                     }

                     // TP-MN
                     pCommand->message_number = (pMsgData->dwMsgID >> 16);
         
                     // TP-DA Destination Address
                     OEMWMS_SetAddress(&pCommand->address, pMsgData->szMOFromMTTo, pMsgData->dwMOFromMTToType, pMsgData->dwMOFromMTToPlan, FALSE);

                     // TP-PID Protocol Identifier
                     pCommand->pid = (wms_pid_e_type)pMsgData->dwPID;

                     // TP-CT GW Command
                     pCommand->command = (wms_gw_command_e_type)pMsgData->dwGWCommand;

                     // TP-UDHI
                     pCommand->user_data_header_present = pMsgPayload->nEncoding;
                     
                     // TP-CDL
                     pCommand->command_data_len = (uint8)MIN(pMsgPayload->nBytes,sizeof(pCommand->command_data));

                     // TP-CD
                     MEMCPY((void*)pCommand->command_data, (void*)pMsgPayload->data, (uint32)pCommand->command_data_len);
                  }
                  else {
                     wms_gw_submit_s_type * pSubmitMsg = NULL;

                     // Originate a GSM Submit Message.
                     pClientTS->u.gw_pp.tpdu_type = WMS_TPDU_SUBMIT;

                     // Start populating the Submit Message.
                     pSubmitMsg = (wms_gw_submit_s_type *)&pClientTS->u.gw_pp.u.submit;

                     // We'll set the Reject Duplicates field to FALSE for now,
                     // currently we do not support re-attempts, so there should not
                     // be any duplicate messages that we sent.
                     pSubmitMsg->reject_duplicates = FALSE;

                     // We do not support reply path currently.
                     pSubmitMsg->reply_path_present = pMsgData->bIsReplyPathPresent;

                     // Request Status Report
                     pSubmitMsg->status_report_enabled = pMsgData->bRequestDLAck;

                     // TP-MR TP-Message-Reference - Section 9.2.3.6 of TS 23.040
                     // TP-MR needs to be a number between 0 and 255. The standard
                     // requires that we use the Last-Used-TP-MR value on the SIM.
                     // For now we shall generate the TP-MR the same way that we
                     // generate Message Ids for CDMA messages.
                     pSubmitMsg->message_reference = (pMsgData->dwMsgID ? pMsgData->dwMsgID : OEMWMS_GetUniqueSMSMessageID() % 255); // Message Id
         
                     // TP-DA Destination Address
                     OEMWMS_SetAddress(&pSubmitMsg->address, pMsgData->szMOFromMTTo, pMsgData->dwMOFromMTToType, pMsgData->dwMOFromMTToPlan, FALSE);

                     // TP-PID Protocol Identifier - Section 9.2.3.9 of TS 23.040.
                     pSubmitMsg->pid = (wms_pid_e_type)pMsgData->dwPID;

                     // TP-DCS Data Coding Scheme
                     if (0xFFFFFFFF == pMsgData->dwRawDCS)
                     {
                        switch (Xlate_2_NativeEncoding(pMsgPayload->nEncoding))
                        {
                           case WMS_ENCODING_UNICODE:
                              pSubmitMsg->dcs.alphabet = WMS_GW_ALPHABET_UCS2;
                              break;

                           case WMS_ENCODING_GSM_7_BIT_DEFAULT:
                              pSubmitMsg->dcs.alphabet = WMS_GW_ALPHABET_7_BIT_DEFAULT;
                              break;

                           default:
                              pSubmitMsg->dcs.alphabet = WMS_GW_ALPHABET_8_BIT;
                        }
            
                        pSubmitMsg->dcs.msg_class     = (wms_message_class_e_type)pMsgData->dwMsgClass;
                        pSubmitMsg->dcs.is_compressed = pMsgData->bIsCompressed;
                        pSubmitMsg->dcs.msg_waiting   = (wms_gw_msg_waiting_e_type)pMsgData->dwMsgWaitingType;

                        if (((pMsgData->dwMsgWaitingKind == AEESMS_MSGWAITINGKIND_VOICEMAIL) ||
                           (pMsgData->dwMsgWaitingKind >= AEESMS_MSGWAITINGKIND_VOICEMAIL + 1)) &&
                           (pMsgData->dwMsgWaitingKind <= AEESMS_MSGWAITINGKIND_OTHER))
                        {
                           pSubmitMsg->dcs.msg_waiting_active = TRUE;
                           pSubmitMsg->dcs.msg_waiting_kind = (wms_gw_msg_waiting_kind_e_type)pMsgData->dwMsgWaitingKind;
                        }
                        else if ((pMsgData->dwMsgWaitingKind >= AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE) &&
                                 (pMsgData->dwMsgWaitingKind <= AEESMS_MSGWAITINGKIND_OTHER_INACTIVE))
                        {
                           pSubmitMsg->dcs.msg_waiting_active = FALSE;
                           pSubmitMsg->dcs.msg_waiting_kind = (wms_gw_msg_waiting_kind_e_type)(pMsgData->dwMsgWaitingKind - AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE);
                        }
                        wms_ts_encode_dcs(&pSubmitMsg->dcs, &pSubmitMsg->dcs.raw_dcs_data);
                     }
                     else
                     {
                        pSubmitMsg->dcs.raw_dcs_data = (uint8)pMsgData->dwRawDCS;
                        wms_ts_decode_dcs(&pSubmitMsg->dcs.raw_dcs_data, &pSubmitMsg->dcs);
                     }

                     if (pMsgData->dwAbsoluteValidityTimeStamp)
                     {
                        pSubmitMsg->validity.format = WMS_GW_VALIDITY_ABSOLUTE;
                        OEMWMS_SetTimeToWmsTimestamp(&pSubmitMsg->validity.u.time, pMsgData->dwAbsoluteValidityTimeStamp, pMsgData->nAbsoluteValidityTimeStampZone, FALSE);
                     }
                     else if (0xFFFFFFFF != pMsgData->dwRelativeValidity)
                     {
                        pSubmitMsg->validity.format = WMS_GW_VALIDITY_RELATIVE;
                        OEMWMS_SetTimeToWmsTimestamp(&pSubmitMsg->validity.u.time, pMsgData->dwRelativeValidity, 0, TRUE);
                     }
                     else
                     {
                        pSubmitMsg->validity.format = WMS_GW_VALIDITY_NONE;
                     }

                     // TP-UD - User Data
                     pSubmitMsg->user_data.sm_len = (uint8)MIN(pMsgPayload->nBytes,(int)sizeof(pSubmitMsg->user_data.sm_data));
                     MEMCPY((void*)pSubmitMsg->user_data.sm_data, (void*)pMsgPayload->data, (uint32)pSubmitMsg->user_data.sm_len);

                     // User Data Headers
                     if ((pSubmitMsg->user_data.num_headers = OEMWMS_SetUDHData(sizeof(pSubmitMsg->user_data.headers)/sizeof(wms_udh_s_type), pSubmitMsg->user_data.headers, (byte*)((void*) pMsgData->data), pMsgData->nUDHLength)) != 0)
                     {      
                        pSubmitMsg->user_data_header_present = TRUE;
                     }
                  }
               }
               else
               {
                  wms_gw_deliver_s_type * pDeliverMsg = NULL;

                  // This will get exercised when storing an MT message
                  pClientTS->u.gw_pp.tpdu_type = WMS_TPDU_DELIVER;

                  // Start populating the Deliver Message.
                  pDeliverMsg = (wms_gw_deliver_s_type *)&pClientTS->u.gw_pp.u.deliver;

                  // TP-DA Source Address
                  OEMWMS_SetAddress(&pDeliverMsg->address, pMsgData->szMOFromMTTo, pMsgData->dwMOFromMTToType, pMsgData->dwMOFromMTToPlan, FALSE);

                  // TP-PID Protocol Identifier - Section 9.2.3.9 of TS 23.040.
                  // Standard indicates that value of 0 can be used for simple
                  // MS-to-SC short message transfer.
                  pDeliverMsg->pid = pClientTS->u.gw_pp.u.deliver.pid;
                  
                  // Specify presence of reply path
                  pDeliverMsg->reply_path_present = pMsgData->bIsReplyPathPresent;
                  
                  // Request Status Report
                  pDeliverMsg->status_report_enabled = pMsgData->bRequestDLAck;

                  // Speccify TP-MMS
                  pDeliverMsg->more = pMsgData->bHasMoreMessageToSend;

                  // TP-DCS Data Coding Scheme
                  if (0xFFFFFFFF == pMsgData->dwRawDCS)
                  {
                     switch (Xlate_2_NativeEncoding(pMsgPayload->nEncoding))
                     {
                        case WMS_ENCODING_UNICODE:
                           pDeliverMsg->dcs.alphabet = WMS_GW_ALPHABET_UCS2;
                           break;

                        case WMS_ENCODING_GSM_7_BIT_DEFAULT:
                           pDeliverMsg->dcs.alphabet = WMS_GW_ALPHABET_7_BIT_DEFAULT;
                           break;

                        default:
                           pDeliverMsg->dcs.alphabet = WMS_GW_ALPHABET_8_BIT;
                     }
         
                     pDeliverMsg->dcs.msg_class     = (wms_message_class_e_type)pMsgData->dwMsgClass;
                     pDeliverMsg->dcs.is_compressed = pMsgData->bIsCompressed;
                     pDeliverMsg->dcs.msg_waiting   = (wms_gw_msg_waiting_e_type)pMsgData->dwMsgWaitingType;

                     if (((pMsgData->dwMsgWaitingKind == AEESMS_MSGWAITINGKIND_VOICEMAIL) ||
                          (pMsgData->dwMsgWaitingKind >= AEESMS_MSGWAITINGKIND_VOICEMAIL + 1)) &&
                         (pMsgData->dwMsgWaitingKind <= AEESMS_MSGWAITINGKIND_OTHER))
                     {
                        pDeliverMsg->dcs.msg_waiting_active = TRUE;
                        pDeliverMsg->dcs.msg_waiting_kind = (wms_gw_msg_waiting_kind_e_type)pMsgData->dwMsgWaitingKind;
                     }
                     else if ((pMsgData->dwMsgWaitingKind >= AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE) &&
                              (pMsgData->dwMsgWaitingKind <= AEESMS_MSGWAITINGKIND_OTHER_INACTIVE))
                     {
                        pDeliverMsg->dcs.msg_waiting_active = FALSE;
                        pDeliverMsg->dcs.msg_waiting_kind = (wms_gw_msg_waiting_kind_e_type)(pMsgData->dwMsgWaitingKind - AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE);
                     }
                     wms_ts_encode_dcs(&pDeliverMsg->dcs, &pDeliverMsg->dcs.raw_dcs_data);
                  }
                  else
                  {
                     pDeliverMsg->dcs.raw_dcs_data = (uint8)pMsgData->dwRawDCS;
                     wms_ts_decode_dcs(&pDeliverMsg->dcs.raw_dcs_data, &pDeliverMsg->dcs);
                  }

                  // TP-UD - User Data
                  pDeliverMsg->user_data.sm_len = (uint8)MIN(pMsgPayload->nBytes,(int)sizeof(pDeliverMsg->user_data.sm_data));
                  MEMCPY((void*)pDeliverMsg->user_data.sm_data, (void*)pMsgPayload->data, (uint32)pDeliverMsg->user_data.sm_len);

                  // Timestamp
                  OEMWMS_SetTimeToWmsTimestamp(&pDeliverMsg->timestamp, pMsgData->dwTimeStamp, pMsgData->nTimeStampZone, FALSE);

                  // Number of messages
                  OEMWMS_SetNumMsgsInfoToGWMsg(pDeliverMsg, Xlate_2_NativeTS(pMsgData->ts), pMsgData->nNumMsgs);

                  // User Data Headers
                  if (1 == pDeliverMsg->user_data.num_headers)
                  {
                     // 1 header occupied for Special SM (Voicemail) User Data Header 
                     if ((pDeliverMsg->user_data.num_headers += OEMWMS_SetUDHData((sizeof(pDeliverMsg->user_data.headers)/sizeof(wms_udh_s_type)) - 1, &pDeliverMsg->user_data.headers[1], (byte*)((void*) pMsgData->data), pMsgData->nUDHLength)) != 0)
                     {      
                        pDeliverMsg->user_data_header_present = TRUE;
                     }
                  }
                  else
                  {
                     if ((pDeliverMsg->user_data.num_headers = OEMWMS_SetUDHData(sizeof(pDeliverMsg->user_data.headers)/sizeof(wms_udh_s_type), pDeliverMsg->user_data.headers,(byte*)((void*) pMsgData->data), pMsgData->nUDHLength)) != 0)
                     {      
                        pDeliverMsg->user_data_header_present = TRUE;
                     }
                  }
               }
            }
         }
         return TRUE;

      default:
         FARF(SMS, ("Unknown Mode"));
         return FALSE;
   }
}

/*===========================================================================

  Converts an ASCII string to a GW 4 bit string

===========================================================================*/
uint32 GetASCII2GW4Bit (char * pcBuf, byte * pBuf, uint32 dwLen) 
{
   uint32   i,j;
   byte     d;

   for (i = j = 0; i < dwLen; i++) 
   {
      d = pcBuf[i];
      switch (d)
      {
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            pBuf[j++] = d - '0';
            break;

         case '*':
            pBuf[j++] = 0x0a;
            break;

         case '#':
            pBuf[j++] = 0x0b;
            break;

         case 'a':
         case 'b':
         case 'c':
            pBuf[j++] = 0x0c + (d - 'a');
            break;

         default:
            break;
      }
   }

   return(uint32)j;
}

/*===========================================================================

  Translates to native priority

===========================================================================*/
static wms_priority_e_type Xlate_2_NativePriority(int priority)
{
   int i;

   for(i = 0; i < sizeof(gsPriorityMap)/sizeof(PriorityMap); i++)
   {
      if (gsPriorityMap[i].AEE_Priority == priority)
         return gsPriorityMap[i].OEM_Priority;
   }
   return ((wms_priority_e_type)priority);
}

/*===========================================================================

  Translates to native privacy

===========================================================================*/
static wms_privacy_e_type Xlate_2_NativePrivacy(int privacy)
{
   int i;

   for(i = 0; i < sizeof(gsPrivacyMap)/sizeof(PrivacyMap); i++)
   {
      if (gsPrivacyMap[i].AEE_Privacy == privacy)
         return gsPrivacyMap[i].OEM_Privacy;
   }
   return ((wms_privacy_e_type)privacy);
}

/*===========================================================================

  Translates to native language

===========================================================================*/
static wms_language_e_type Xlate_2_NativeLang(int lang)
{
   int i;

   for(i = 0; i < sizeof(gsLangMap)/sizeof(LangMap); i++)
   {
      if (gsLangMap[i].AEE_Lang == lang)
         return gsLangMap[i].OEM_Lang;
   }
   return ((wms_language_e_type)lang);
}

/*===========================================================================

  Translates to AEE tag

===========================================================================*/
static uint32 Xlate_Tag(wms_message_tag_e_type tag)
{
   int i;

   for(i = 0; i < sizeof(gsTagMap)/sizeof(TagMap); i++)
   {
      if (gsTagMap[i].OEM_Tag == tag)
         return gsTagMap[i].AEE_Tag;
   }
   return tag;
}

/*===========================================================================

  Translates to native tag

===========================================================================*/
static wms_message_tag_e_type Xlate_2_NativeTag(int tag)
{
   int i;

   for(i = 0; i < sizeof(gsTagMap)/sizeof(TagMap); i++)
   {
      if (gsTagMap[i].AEE_Tag == tag)
         return gsTagMap[i].OEM_Tag;
   }
   return ((wms_message_tag_e_type)tag);
}

/*===========================================================================

  Translates to storage type

===========================================================================*/
static int Xlate_StorageType(wms_memory_store_e_type st, wms_message_tag_e_type tag)
{
   int i;

   // If tag isn't something that's required for storage type, set it none
   // so that it's ignored
   if ((WMS_TAG_STATUS_RPT != tag) && (WMS_TAG_MO_TEMPLATE != tag))
      tag = WMS_TAG_NONE;

   for(i = 0; i < sizeof(gsStorageTypeMap)/sizeof(StorageTypeMap); i++)
   {
      if ((gsStorageTypeMap[i].OEM_StorageType == st) && 
          (gsStorageTypeMap[i].OEM_Tag == tag))
      {
         return gsStorageTypeMap[i].AEE_StorageType;
      }
   }
   return AEESMS_STORAGE_NONE;
}

/*===========================================================================

  Translates to native storage type

===========================================================================*/
static wms_memory_store_e_type Xlate_2_NativeStorageType(int st)
{
   int i;

   for(i = 0; i < sizeof(gsStorageTypeMap)/sizeof(StorageTypeMap); i++)
   {
      if (gsStorageTypeMap[i].AEE_StorageType == st)
         return gsStorageTypeMap[i].OEM_StorageType;
   }
   return WMS_MEMORY_STORE_NONE;
}

/*===========================================================================

  Translates to tag type from storage type

===========================================================================*/
static wms_message_tag_e_type Xlate_2_NativeTagFromStorageType(int st)
{
   int i;

   for(i = 0; i < sizeof(gsStorageTypeMap)/sizeof(StorageTypeMap); i++)
   {
      if (gsStorageTypeMap[i].AEE_StorageType == st)
         return gsStorageTypeMap[i].OEM_Tag;
   }
   return WMS_TAG_NONE;
}

/*===========================================================================

  Translates to domain preference

===========================================================================*/
static int Xlate_DomainPref(wms_gw_domain_pref_e_type dp)
{
   int i;

   for(i = 0; i < sizeof(gsGWDomainPrefMap)/sizeof(GWDomainPrefMap); i++)
   {
      if (gsGWDomainPrefMap[i].DMSS_DomainPref == dp)
         return gsGWDomainPrefMap[i].AEE_DomainPref;
   }
   return dp;
}

/*===========================================================================

  Translates to native domain preference

===========================================================================*/
static wms_gw_domain_pref_e_type Xlate_2_NativeDomainPref(int dp)
{
   int i;

   for(i = 0; i < sizeof(gsGWDomainPrefMap)/sizeof(GWDomainPrefMap); i++)
   {
      if (gsGWDomainPrefMap[i].AEE_DomainPref == dp)
         return gsGWDomainPrefMap[i].DMSS_DomainPref;
   }
   return ((wms_gw_domain_pref_e_type)dp);
}

/*===========================================================================

  Constructs a client message type from the raw bearer data

===========================================================================*/
static boolean OEMWMS_ConstructClientMessage
(
   OEMMsgData * pMsgData,
   wms_client_message_s_type * pMsg,
   wms_raw_ts_data_s_type * pRaw,
   sys_sys_mode_e_type sysMode
) 
{
   FARF(SMS, ("==>  OEMWMS_ConstructClientMessage"));

   switch (sysMode)
   {
      case SYS_SYS_MODE_CDMA:
         {
            wms_cdma_message_s_type * pCMsg;

            FARF(SMS, ("CDMA Mode Message"));

            pMsg->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
            pMsg->msg_hdr.tag          = Xlate_2_NativeTag(pMsgData->dwTag);
            pMsg->msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
            pMsg->msg_hdr.index        = WMS_DUMMY_MESSAGE_INDEX;

            pCMsg = (wms_cdma_message_s_type *)&pMsg->u.cdma_message;

            pCMsg->is_mo = pMsgData->bIsMOSMS; 
            if (0 == gpWms->nIsTLAckRequested)
            {
               pCMsg->is_tl_ack_requested = FALSE;
            }
            else // For All other cases
            {
               pCMsg->is_tl_ack_requested = TRUE;
            }

            if (pMsgData->bIsMOSMS)
            {
               if (pMsgData->bIsBREWDirectedSMS)
               {
                  if (OEM_GetConfig(CFGI_BREWDIR_SMS_TELESERVICE, (void*)&pCMsg->teleservice, sizeof(pCMsg->teleservice)) != SUCCESS)
                  {
                     pCMsg->teleservice = WMS_TELESERVICE_CMT_95;
                  }
               }
               else
               {
                  pCMsg->teleservice = Xlate_2_NativeTS(pMsgData->ts);
               }
            }
            else
            {
               pCMsg->teleservice = Xlate_2_NativeTS(pMsgData->ts);
            }

            if (WMS_TELESERVICE_BROADCAST == pCMsg->teleservice)
            {
              // For now, we only worry about "service" for cdma BC messages
              pCMsg->is_service_present = TRUE;
              pCMsg->service            = (wms_service_e_type)pMsgData->dwSrvID;
            }

            // Set the raw_bd field with what we created above
            MEMCPY(&pCMsg->raw_ts, pRaw, sizeof(wms_raw_ts_data_s_type));   
   
            OEMWMS_SetAddress(&pCMsg->address, pMsgData->szMOFromMTTo, pMsgData->dwMOFromMTToType, pMsgData->dwMOFromMTToPlan, TRUE);

         }
         return TRUE;

      case SYS_SYS_MODE_GSM:
         {
            wms_gw_message_s_type * pCMsg;
         
            FARF(SMS, ("GSM Mode Message"));

            // Populate the msg_hdr structure
            pMsg->msg_hdr.index        = WMS_DUMMY_MESSAGE_INDEX;
            pMsg->msg_hdr.mem_store    = WMS_MEMORY_STORE_NONE;
            pMsg->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
            pMsg->msg_hdr.tag          = Xlate_2_NativeTag(pMsgData->dwTag);

            pCMsg = (wms_gw_message_s_type *)&pMsg->u.gw_message;

            // Populate the gw_message structure
            pCMsg->is_broadcast = FALSE;
   
            if (*pMsgData->szCallback)
            {
               OEMWMS_SetAddress(&pCMsg->sc_address, pMsgData->szCallback, pMsgData->dwCallbackType, pMsgData->dwCallbackPlan, FALSE);
            }
            else
            {
               int nSize;
               char * pszSCAddress = NULL;
               if ((OEM_GetConfig(CFGI_SMS_GW_SCADDRESS_LENGTH, &nSize, sizeof(nSize)) == SUCCESS) &&
                   ((pszSCAddress = (char*)MALLOC(nSize+1)) != NULL) &&
                   (OEM_GetConfig(CFGI_SMS_GW_SCADDRESS, pszSCAddress, nSize + 1) == SUCCESS))
               {
                  OEMWMS_SetAddress(&pCMsg->sc_address, pszSCAddress, AEESMS_NUMBER_UNKNOWN, AEESMS_NUMBER_PLAN_UNKNOWN, FALSE);
               }
               FREEIF(pszSCAddress);
            }

            MEMCPY(&pCMsg->raw_ts_data, pRaw, sizeof(wms_raw_ts_data_s_type));
         }
         return TRUE;

      default:
         FARF(SMS, ("Unknown Mode"));
         return FALSE;
   }
}

/*===========================================================================

  Sets address in structure from address string

===========================================================================*/
boolean OEMWMS_SetAddress
(
   wms_address_s_type * psadr, 
   char* pszAddress, 
   AEESMSNumberType dwNumberType, 
   AEESMSNumberPlan dwNumberPlan,
   boolean bCDMA
)
{
   char * atStringPtr, * dotStringPtr;

   if (!psadr || !pszAddress || !*pszAddress)
   {
      return FALSE;
   }

   // Determine whether the To address is an email address or a phone number.
   // We just check for <some chars>@<some chars>.<some chars>. May not be a 
   // totally fool proof way of checking/validating the email address. 
   // But it works!!!
   atStringPtr = STRCHR(pszAddress,(char)'@');
   dotStringPtr = STRCHR(pszAddress,(char)'.');

   // Check to make sure that the user entered both "@" and "." symbols
   // in his send to address.
   if (atStringPtr && dotStringPtr) 
   {   
      // Send address is an email address. Code the digit mode, number mode and
      // the number type accordingly.
      psadr->digit_mode = WMS_DIGIT_MODE_8_BIT;            // 8 bit ASCII codes
      psadr->number_mode = WMS_NUMBER_MODE_DATA_NETWORK; 
      psadr->number_type = WMS_NUMBER_INTERNET_EMAIL;          // EMAIL address
      MEMSET((byte*)psadr->digits, 0, sizeof(psadr->digits));
      STRLCPY((char*)psadr->digits, pszAddress, sizeof(psadr->digits));
      psadr->number_of_digits = (byte)STRLEN((char *)pszAddress);
   } 
   else 
   {  
      if (DIALER_PLUS_CHAR == *pszAddress) 
      {
         if (OEM_CONFIG_ALLOW_INTERNATIONAL) 
         {
            char * pcBuf = (char*)(pszAddress + 1); // Remove '+' char
            psadr->digit_mode  = WMS_DIGIT_MODE_8_BIT;
            psadr->number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
            psadr->number_type = WMS_NUMBER_INTERNATIONAL;
            psadr->number_plan = (wms_number_plan_e_type)dwNumberPlan;
            MEMSET((byte*)psadr->digits, 0, sizeof(psadr->digits));
            // Remove the plus character and copy
            STRLCPY((char*)psadr->digits, pcBuf, sizeof(psadr->digits));         
            psadr->number_of_digits = (byte)STRLEN((char *)pcBuf);
         } 
         else 
         {
            int nPrefix = STRLEN(OEM_CONFIG_INTERNATIONAL_PREFIX);
            psadr->digit_mode = WMS_DIGIT_MODE_4_BIT;   // DTMF digits
            psadr->number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;  
            psadr->number_type = (wms_number_type_e_type)dwNumberType;  
            psadr->number_plan = (wms_number_plan_e_type)dwNumberPlan; 

            if (nPrefix)
            { 
               if (bCDMA)
                  (void)GetASCII2DTMF((char *)OEM_CONFIG_INTERNATIONAL_PREFIX, (byte*)psadr->digits, nPrefix);
               else
                  (void)GetASCII2GW4Bit((char *)OEM_CONFIG_INTERNATIONAL_PREFIX, (byte*)psadr->digits, nPrefix);
            }         
            
            if (bCDMA)
               (void)GetASCII2DTMF((char *)pszAddress, (byte*)(psadr->digits + nPrefix), STRLEN((char *)pszAddress));
            else
               (void)GetASCII2GW4Bit((char *)pszAddress, (byte*)(psadr->digits + nPrefix), STRLEN((char *)pszAddress));

            psadr->number_of_digits = (byte)(nPrefix + STRLEN((char *)pszAddress));
         }
      }
      else
      {
         psadr->digit_mode = WMS_DIGIT_MODE_4_BIT;   // DTMF digits
         psadr->number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK; 
         psadr->number_type = (wms_number_type_e_type)dwNumberType;  
         psadr->number_plan = (wms_number_plan_e_type)dwNumberPlan; 
   
         if (bCDMA)
            GetASCII2DTMF((char *)pszAddress, (byte*)psadr->digits, STRLEN((char *)pszAddress));
         else
            GetASCII2GW4Bit((char *)pszAddress, (byte*)psadr->digits, STRLEN((char *)pszAddress));
   
         psadr->number_of_digits = (byte)STRLEN((char *)pszAddress);
      }
   }
   return TRUE;
}

/*===========================================================================

  Constructs a client message type from the raw bearer data

===========================================================================*/
static void OEMWMS_ConstructCDMATemplate
(
   OEMMsgData * pMsgData, 
   wms_client_message_s_type * pMsg, 
   wms_client_bd_s_type * pClientBD
)
{
   FARF(SMS, ("==>  OEMWMS_ConstructCDMATemplate"));

   pMsg->u.cdma_template.teleservice = Xlate_2_NativeTS(pMsgData->ts);

   pMsg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_DEST_ADDR;
   OEMWMS_SetAddress(&pMsg->u.cdma_template.address, pMsgData->szMOFromMTTo, pMsgData->dwMOFromMTToType, pMsgData->dwMOFromMTToPlan, TRUE);

   if (pMsgData->dwRelativeValidity)
   {
      pMsg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_VALIDITY;
      OEMWMS_SetTimeToWmsTimestamp(&pMsg->u.cdma_template.relative_validity, pMsgData->dwRelativeValidity, 0, TRUE);
   }

   pMsg->u.cdma_template.mask |= WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
   MEMCPY(&pMsg->u.cdma_template.client_bd, pClientBD, sizeof(wms_client_bd_s_type));
}

/*===========================================================================

  Constructs a client message type from the raw bearer data

===========================================================================*/
static void OEMWMS_ConstructGWTemplate
(
   OEMMsgData * pMsgData,  
   OEMMsgPayload * pMsgPayload, 
   wms_client_message_s_type * pMsg
)
{
   FARF(SMS, ("==>  OEMWMS_ConstructGWTemplate"));
         
   if (pMsgData->nAlphaIDLength)
   {
      pMsg->u.gw_template.alpha_id.len = pMsgData->nAlphaIDLength;  
      pMsg->u.gw_template.alpha_id.data = pMsgData->data + pMsgData->nUDHLength + pMsgData->nRawTPDULength;
   }

   if (STRLEN(pMsgData->szMOFromMTTo) > 0)
   {
      pMsg->u.gw_template.mask |= WMS_GW_TEMPLATE_MASK_DEST_ADDR;
      OEMWMS_SetAddress(&pMsg->u.gw_template.dest_addr, pMsgData->szMOFromMTTo, pMsgData->dwMOFromMTToType, pMsgData->dwMOFromMTToPlan, FALSE);
   }

   if (STRLEN(pMsgData->szCallback) > 0)
   {
      pMsg->u.gw_template.mask |= WMS_GW_TEMPLATE_MASK_SC_ADDR;
      OEMWMS_SetAddress(&pMsg->u.gw_template.sc_addr, pMsgData->szCallback, pMsgData->dwCallbackType, pMsgData->dwCallbackPlan, FALSE);
   }

   if (pMsgData->dwRelativeValidity)
   {
      pMsg->u.gw_template.mask |= WMS_GW_TEMPLATE_MASK_VALIDITY;
      OEMWMS_SetTimeToWmsTimestamp(&pMsg->u.gw_template.relative_validity, pMsgData->dwRelativeValidity, 0, TRUE);
   }

   pMsg->u.gw_template.mask |= WMS_GW_TEMPLATE_MASK_PID;
   pMsg->u.gw_template.pid = (wms_pid_e_type)pMsgData->dwPID;
   
   pMsg->u.gw_template.mask |= WMS_GW_TEMPLATE_MASK_DCS;
   if (0xFFFFFFFF == pMsgData->dwRawDCS)
   {
      switch (Xlate_2_NativeEncoding(pMsgPayload->nEncoding))
      {
         case WMS_ENCODING_UNICODE:
            pMsg->u.gw_template.dcs.alphabet = WMS_GW_ALPHABET_UCS2;
            break;

         case WMS_ENCODING_GSM_7_BIT_DEFAULT:
            pMsg->u.gw_template.dcs.alphabet = WMS_GW_ALPHABET_7_BIT_DEFAULT;
            break;

         default:
            pMsg->u.gw_template.dcs.alphabet = WMS_GW_ALPHABET_8_BIT;
      }

      pMsg->u.gw_template.dcs.msg_class = (wms_message_class_e_type)pMsgData->dwMsgClass;
      pMsg->u.gw_template.dcs.is_compressed = pMsgData->bIsCompressed;
      pMsg->u.gw_template.dcs.msg_waiting   = (wms_gw_msg_waiting_e_type)pMsgData->dwMsgWaitingType;

      if (((pMsgData->dwMsgWaitingKind == AEESMS_MSGWAITINGKIND_VOICEMAIL) ||
           (pMsgData->dwMsgWaitingKind >= AEESMS_MSGWAITINGKIND_VOICEMAIL + 1)) &&
          (pMsgData->dwMsgWaitingKind <= AEESMS_MSGWAITINGKIND_OTHER))
      {
         pMsg->u.gw_template.dcs.msg_waiting_active = TRUE;
         pMsg->u.gw_template.dcs.msg_waiting_kind = (wms_gw_msg_waiting_kind_e_type)pMsgData->dwMsgWaitingKind;
      }
      else if ((pMsgData->dwMsgWaitingKind >= AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE) &&
               (pMsgData->dwMsgWaitingKind <= AEESMS_MSGWAITINGKIND_OTHER_INACTIVE))
      {
         pMsg->u.gw_template.dcs.msg_waiting_active = FALSE;
         pMsg->u.gw_template.dcs.msg_waiting_kind = (wms_gw_msg_waiting_kind_e_type)(pMsgData->dwMsgWaitingKind - AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE);
      }
      wms_ts_encode_dcs(&pMsg->u.gw_template.dcs, &pMsg->u.gw_template.dcs.raw_dcs_data);
   }
   else
   {
      pMsg->u.gw_template.dcs.raw_dcs_data = (uint8)pMsgData->dwRawDCS;
      wms_ts_decode_dcs(&pMsg->u.gw_template.dcs.raw_dcs_data, &pMsg->u.gw_template.dcs);
   }
}

/*===========================================================================

  Called to give some gap and synchronize calls between returning callback 
  of send sms and informing about the new loopback message.

  This essentially helps to serialize the callbacks so the send msg cb
  fires before the MT message cb would i.e. the user should first get the cb
  that he scheduled for send message, which follows a complex path and at 
  a later time, get the incoming message notification.

  FUZZY LOGIC 101, Why twice?
  - First it waits for InvokeAppSendMsgCb to resume
  - Second time, it waits for LinkAppSendMsgCb to resume

  This should be monitored since it can break if the above changes.
===========================================================================*/

static void OEMWSMS_LoopbackCb ( void* pUser )
{
   SMSCacheEntry * pce = (SMSCacheEntry *) pUser;
   
   if ( !gpWms->nLoopbackCount )
   {
      // Gotta wait one more time...
      gpWms->nLoopbackCount++;

      CALLBACK_Init (&gpWms->pcbOnLoopbackMsg, OEMWSMS_LoopbackCb, (void*)pce);
      AEE_SYS_RESUME (&gpWms->pcbOnLoopbackMsg);
   }
   else
   {
      // Notify clients of the loopback message
      OEMSMSCache_Ready (pce);

      // OEM TODO - To be implemented by OEMs if necessary, see function 
      // stub comments for more info.
      OEMSMSCust_LoopbackMsg (pce);
   }
}


/*===========================================================================

  Called to send SMS

===========================================================================*/
void OEMSMS_SendSMS(OEMMsg* hMsg, AEECallback *pcb, uint32 *pdwStatus)
{
   int nErr = SUCCESS;
   uint32 dwStatus = 0;

   FARF(SMS, ("==>  OEMSMS_SendSMS"));

   if (!gpWms) 
   {
      nErr = EFAILED;
      AEESMS_SETERROR(dwStatus, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_GENERAL_PROBLEM);
      goto Done;
   }

   // Install the message being processed
   hMsg->pcb = pcb;
   hMsg->pdwStatus = pdwStatus;

   if (gpWms->pCurMOSMS) 
   {
      hMsg->pNext = gpWms->pCurMOSMS;
      gpWms->pCurMOSMS->pPrev = hMsg;
   }

   gpWms->pCurMOSMS = hMsg;

   //
   // Check for loopback message.  This does not need to go to the network
   // Just put it in the incoming MT message queue.
   //
   if ( hMsg->pMsgData->bIsLoopbackMsg )
   {
      SMSCacheEntry * pce = NULL;
      
      FARF(SMS, ("==>  LOOPBACK SMS MESSAGE <=="));
      
      pce = OEMSMSCache_GetEntry();

      if (pce == NULL) 
      {
         // Ooops, we do not have any free Cache Entry's available!
         FARF(SMS, ("No Cache Entry available for loopback, SMS Msg Discarded"));       
         
         // Since we are out of cache, send back a failure and exit
         AEESMS_SETERROR(dwStatus, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_BUSY);
         *pdwStatus = dwStatus;
         AEE_SYS_RESUME(hMsg->pcb);

         goto Done;
      }

      if (OEMWMS_CopyMessage(pce->dwTransactionId,(wms_client_message_s_type*)&hMsg->CMsg,pce)) 
      {
         // Override some variables, related to loopback
         pce->bIsLoopbackMsg = TRUE;
         pce->bIsMOSMS = FALSE;
         pce->nFromAppletCls = hMsg->pMsgData->nFromAppletCls;

         // Things look good, send back success in callback
         AEESMS_SETERROR(dwStatus, AEESMS_ERRORTYPE_NONE, AEESMS_ERROR_NONE);
         *pdwStatus = dwStatus;
         AEE_SYS_RESUME(hMsg->pcb);

         // Start callback to serialize MT so it goes after
         // send message cb
         gpWms->nLoopbackCount = 0;
         CALLBACK_Init (&gpWms->pcbOnLoopbackMsg, OEMWSMS_LoopbackCb, (void*)pce);
         AEE_SYS_RESUME (&gpWms->pcbOnLoopbackMsg);
      }
      else
      {
         // Oops, failed again, a General problem??
         AEESMS_SETERROR(dwStatus, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_GENERAL_PROBLEM);
         *pdwStatus = dwStatus;
         AEE_SYS_RESUME(hMsg->pcb);

         goto Done;
      }

      goto Done;
   }

   if (gpWms->bTransmissionPending) 
   {
      FARF(SMS, ("Transmission Pending"));
      // We are done here as traffic channel is being brought up for some
      // previous message
      hMsg->nState = OEMMSG_STATE_SEND_ON_TRAFFIC;    
      nErr = SUCCESS;
      goto Done;
   }

   if ((gpWms->bMOOnAccessChannel)
       ||(WMS_MESSAGE_MODE_GW == hMsg->CMsg.msg_hdr.message_mode))
   {
      FARF(SMS, ("Send over Access Channel/GW message"));
      wms_msg_send(gpWms->clientId, 
                   &OEMWSMS_CommandCallback, 
                   (const void *)hMsg, 
                   WMS_SEND_MODE_CLIENT_MESSAGE, 
                   &hMsg->CMsg);
   }
   else if (gpWms->bMOOnTrafficChannel)
   {
      wms_status_e_type wmsStatus = OEMWMS_SendSMSOnTrafficChannel(hMsg);

      FARF(SMS, ("Send over Traffic channel"));

      if (WMS_OK_S != wmsStatus)
      {        
         nErr = EFAILED;
         AEESMS_SETERROR(dwStatus, AEESMS_ERRORTYPE_UNKNOWN, Xlate_wms_status_e_type_2_MOSMS_ERR(wmsStatus));
         goto Done;
      }
   }
   else
   {
      FARF(SMS, ("Configuration Problem - Channel not specified for MO SMS"));
      nErr = EFAILED;
      AEESMS_SETERROR(dwStatus, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_GENERAL_PROBLEM);
      goto Done;
   }

Done:
   if (nErr != SUCCESS)
   {
      *pdwStatus = dwStatus;
      AEE_SYS_RESUME(pcb);
   }
}

#ifdef FEATURE_CDSMS
/*===========================================================================

   Event Listener

===========================================================================*/
static void OEMWMS_EventListener(wms_dc_event_e_type event, 
                                  wms_dc_event_info_s_type * info_ptr)
{
   wms_status_e_type ret = WMS_NETWORK_FAILURE_S;

   (void) info_ptr; /* unused parameter */
   FARF(SMS,("Phone_EventListener"));
        
   if (WMS_DC_EVENT_CONNECTING == event)
      return;

   //We are interested in the Events only if we had initiated the DC to be brought up for
   //sending a SMS.
   if (gpWms->bTransmissionPending == TRUE) 
   {
      OEMMsg * pNode;
      gpWms->bTransmissionPending = FALSE;

      if (event == WMS_DC_EVENT_CONNECTED) 
      {
         FARF(SMS, ("Phone_EL_CONN"));

         gpWms->bDedicatedChannel = TRUE;

         for(pNode = gpWms->pCurMOSMS; 
             (pNode && (OEMMSG_STATE_SEND_ON_TRAFFIC == pNode->nState)); 
             pNode = pNode->pNext)
         {
            //When message is sent (or failed), the function OEMWMS_MessageListener() is invoked
            ret  = wms_msg_send(gpWms->clientId, 
                                 OEMWSMS_CommandCallback, 
                                 pNode, 
                                 WMS_SEND_MODE_CLIENT_MESSAGE, 
                                 &pNode->CMsg); 

            if (ret != WMS_OK_S) 
            {
               AEESMS_SETERROR(*(pNode->pdwStatus), AEESMS_ERRORTYPE_UNKNOWN, Xlate_wms_status_e_type_2_MOSMS_ERR(ret));
               AEE_SYS_RESUME(pNode->pcb);
            }
         }
      }
      else
      {
         // Failure
         for(pNode = gpWms->pCurMOSMS; pNode; pNode = pNode->pNext)
         {
            AEESMS_SETERROR(*(pNode->pdwStatus), AEESMS_ERRORTYPE_UNKNOWN, Xlate_wms_status_e_type_2_MOSMS_ERR(ret));
            AEE_SYS_RESUME(pNode->pcb);
         }
      }
   }
}
#endif /*FEATURE_CDSMS*/

/*===========================================================================

  Translates WMS status to AEE error

===========================================================================*/
static int Xlate_wms_status_e_type_2_MOSMS_ERR(wms_status_e_type e)
{  
   switch(e) 
   {
      case WMS_OK_S:
         return AEESMS_ERROR_NONE;

      case WMS_OUT_OF_RESOURCES_S:
         return AEESMS_ERROR_NO_MEMORY;

      case WMS_NOT_ALLOWED_IN_AMPS_S:
         return AEESMS_ERROR_NOT_ALLOWED_IN_AMPS;

      case WMS_NETWORK_NOT_READY_S:
         return AEESMS_ERROR_NETWORK_NOT_READY;

      case WMS_INVALID_TRANSACTION_ID_S:
         return AEESMS_ERROR_INVALID_TRANSACTION_ID;

      case WMS_PHONE_NOT_READY_S:
         return AEESMS_ERROR_PHONE_NOT_READY;

      case WMS_ACCESS_TOO_LARGE_S:
         return AEESMS_ERROR_ACCESS_TOO_LARGE;

      case WMS_DTC_TOO_LARGE_S:
         return AEESMS_ERROR_DTC_TOO_LARGE;

      case WMS_UNSUPPORTED_S:
         return AEESMS_ERROR_UNSUPPORTED;

      case WMS_NETWORK_FAILURE_S:         
         return AEESMS_ERROR_NETWORK_PROBLEM;

      default:
         return AEESMS_ERROR_GENERAL_PROBLEM;
   }
}

/*===========================================================================

  Translates WMS CDMA TL status to AEE error

===========================================================================*/
static int Xlate_wms_cdma_tl_status_e_type_2_MOSMS_ERR(wms_cdma_tl_status_e_type e)
{  
   switch(e) 
   {
      case WMS_TL_ADDRESS_TRANSLATION_FAILURE_S:
         return AEESMS_ERROR_ADDRESS_TRANSLATION_FAILURE;

      case WMS_TL_SMS_ORIGINATION_DENIED_S:
         return AEESMS_ERROR_SMS_ORIGINATION_DENIED;

      case WMS_TL_SMS_TERMINATION_DENIED_S:
         return AEESMS_ERROR_SMS_TERMINATION_DENIED;

      case WMS_TL_SMS_NOT_SUPPORTED_S:
         return AEESMS_ERROR_SMS_NOT_SUPPORTED;

      case WMS_TL_ADDRESS_VACANT_S:
         return AEESMS_ERROR_ADDRESS_VACANT;

      case WMS_TL_NETWORK_RESOURCE_SHORTAGE_S:
         return AEESMS_ERROR_NETWORK_RESOURCE_SHORTAGE;

      case WMS_TL_NETWORK_FAILURE_S:
         return AEESMS_ERROR_NETWORK_FAILURE;

      case WMS_TL_INVALID_TELESERVICE_ID_S:
         return AEESMS_ERROR_NETWORK_INVALID_TELESERVICE_ID;

      case WMS_TL_OTHER_NETWORK_PROBLEM_S:
         return AEESMS_ERROR_NETWORK_PROBLEM;

      case WMS_TL_NO_PAGE_RESPONSE_S:
         return AEESMS_ERROR_DESTINATION_NO_PAGE_RESPONSE;

      case WMS_TL_DESTINATION_BUSY_S:
         return AEESMS_ERROR_DESTINATION_BUSY;

      case WMS_TL_NO_ACK_S:
         return AEESMS_ERROR_NO_ACK;

      case WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S:
         return AEESMS_ERROR_DESTINATION_RESOURCE_SHORTAGE;

      case WMS_TL_SMS_DELIVERY_POSTPONED_S:
         return AEESMS_ERROR_SMS_DELIVERY_POSTPONED;

      case WMS_TL_DESTINATION_OUT_OF_SERVICE_S:
         return AEESMS_ERROR_DESTINATION_OUT_OF_SERVICE;

      case WMS_TL_DESTINATION_NO_LONGER_AT_THIS_ADDRESS_S:
         return AEESMS_ERROR_DESTINATION_NO_LONGER_AT_THIS_ADDRESS;

      case WMS_TL_OTHER_TERMINAL_PROBLEM_S:
         return AEESMS_ERROR_DESTINATION_PROBLEM;

      case WMS_TL_RADIO_IF_RESOURCE_SHORTAGE_S:
         return AEESMS_ERROR_RADIO_IF_RESOURCE_SHORTAGE;

      case WMS_TL_RADIO_IF_INCOMPATIBLE_S:
         return AEESMS_ERROR_RADIO_IF_INCOMPATIBLE;

      case WMS_TL_OTHER_RADIO_IF_PROBLEM_S:
         return AEESMS_ERROR_RADIO_PROBLEM;

      case WMS_TL_UNEXPECTED_PARM_SIZE_S:
         return AEESMS_ERROR_UNEXPECTED_PARM_SIZE;

      case WMS_TL_SUPPL_SERVICE_NOT_SUPPORTED:
         return AEESMS_ERROR_SUPPL_SERVICE_NOT_SUPPORTED;

      case WMS_TL_RESERVED_101_S:
         return AEESMS_ERROR_RESERVED;

      case WMS_TL_MISSING_EXPECTED_PARM_S:
         return AEESMS_ERROR_MISSING_EXPECTED_PARM;

      case WMS_TL_MISSING_MANDATORY_PARM_S:
         return AEESMS_ERROR_MISSING_MANDATORY_PARM;

      case WMS_TL_UNRECOGNIZED_PARM_VALUE_S:
         return AEESMS_ERROR_UNRECOGNIZED_PARM_VALUE;

      case WMS_TL_UNEXPECTED_PARM_VALUE_S:
         return AEESMS_ERROR_UNEXPECTED_PARM_VALUE;

      case WMS_TL_USER_DATA_SIZE_ERROR_S:
         return AEESMS_ERROR_USER_DATA_SIZE_ERROR;

      case WMS_TL_OTHER_GENERAL_PROBLEMS_S:
         return AEESMS_ERROR_GENERAL_PROBLEM;

      default:
         if ((e >= WMS_TL_OTHER_NETWORK_PROBLEM_MORE_FIRST_S) &&
             (e <= WMS_TL_OTHER_NETWORK_PROBLEM_MORE_LAST_S))
         {
            return AEESMS_ERROR_NETWORK_PROBLEM;
         }
         else if ((e >= WMS_TL_OTHER_TERMINAL_PROBLEM_MORE_FIRST_S) &&
                  (e <= WMS_TL_OTHER_TERMINAL_PROBLEM_MORE_LAST_S))
         {
            return AEESMS_ERROR_DESTINATION_PROBLEM;
         }
         else if ((e >= WMS_TL_SMS_DELIVERY_POSTPONED_MORE_FIRST_S) &&
                  (e <= WMS_TL_SMS_DELIVERY_POSTPONED_MORE_LAST_S))
         {
            return AEESMS_ERROR_SMS_DELIVERY_POSTPONED;
         }
         else if ((e >= WMS_TL_OTHER_RADIO_IF_PROBLEM_MORE_FIRST_S) &&
                  (e <= WMS_TL_OTHER_RADIO_IF_PROBLEM_MORE_LAST_S))
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

  Translates WMS retort status to AEE error

===========================================================================*/
static int Xlate_wms_submit_report_info_2_MOSMS_ERR(const wms_submit_report_info_s_type *submit_info)
{  
   int nMOSMSErr;

   switch(submit_info->report_status) 
   {
      case WMS_RPT_OK:
         return AEESMS_ERROR_NONE;

      case WMS_RPT_LL_ERROR:
         return AEESMS_ERROR_LOWER_LAYER_FAILURE;

      case WMS_RPT_OUT_OF_RESOURCES:
         return AEESMS_ERROR_NO_MEMORY;

      case WMS_RPT_NETWORK_NOT_READY:
         return AEESMS_ERROR_NETWORK_NOT_READY;

      case WMS_RPT_PHONE_NOT_READY:
         return AEESMS_ERROR_PHONE_NOT_READY;

      case WMS_RPT_CDMA_TL_ERROR:
         return Xlate_wms_cdma_tl_status_e_type_2_MOSMS_ERR(submit_info->cause_info.tl_status);

      case WMS_RPT_NO_ACK:
         return AEESMS_ERROR_NO_ACK;

      case WMS_RPT_ACCESS_TOO_LARGE:
         return AEESMS_ERROR_ACCESS_TOO_LARGE;

      case WMS_RPT_DC_TOO_LARGE:
         return AEESMS_ERROR_DTC_TOO_LARGE;

      case WMS_RPT_NOT_ALLOWED_IN_AMPS:
         return AEESMS_ERROR_NOT_ALLOWED_IN_AMPS;
      
      case WMS_RPT_CANNOT_SEND_BROADCAST:
         return AEESMS_ERROR_CANNOT_SEND_BROADCAST;

      case WMS_RPT_ACCESS_BLOCK:
         return AEESMS_ERROR_RPT_ACCESS_BLOCK;

      case WMS_RPT_RP_ERROR:
         if ((nMOSMSErr = Xlate_wms_rp_cause_e_type_2_MOSMS_ERR(submit_info->cause_info.cause_value)) != AEESMS_ERROR_GENERAL_PROBLEM)
         {
            return nMOSMSErr;
         }
         else
         {
            return Xlate_wms_tp_cause_e_type_2_MOSMS_ERR(submit_info->u.error.tp_cause);
         }
      
      case WMS_RPT_CP_ERROR:
         return Xlate_wms_rp_cause_e_type_2_MOSMS_ERR(submit_info->cause_info.cause_value);

      case WMS_RPT_SMR_TR1M_TIMEOUT:
      case WMS_RPT_SMR_TR2M_TIMEOUT:
      case WMS_RPT_SMR_NO_RESOURCES:
         return AEESMS_ERROR_LOWER_LAYER_FAILURE;

      case WMS_RPT_MO_CONTROL_DISALLOW:
         return AEESMS_ERROR_MO_CONTROL_DISALLOW;

      case WMS_RPT_MO_CONTROL_ERROR:
         return AEESMS_ERROR_MO_CONTROL_ERROR;
    
      // case WMS_RPT_MO_RETRY_PERIOD_EXPIRED:
         // return AEESMS_ERROR_MO_RETRY_PERIOD_EXPIRED;

      default:
         return AEESMS_ERROR_GENERAL_PROBLEM;
   }
}

/*===========================================================================

  Translates TP cause to AEE error

===========================================================================*/
static int Xlate_wms_tp_cause_e_type_2_MOSMS_ERR(wms_tp_cause_e_type e)
{  
   switch(e) 
   {
      case WMS_TP_CAUSE_TELEMATIC_NOT_SUPPORTED:
         return AEESMS_ERROR_UNSUPPORTED_TELEMATIC;

      case WMS_TP_CAUSE_SM_TYPE0_NOT_SUPPORTED:
      case WMS_TP_CAUSE_CANNOT_REPLACE_SM:
      case WMS_TP_CAUSE_UNSPECIFIED_PID_ERROR:
         return AEESMS_ERROR_PID_PROBLEM;
      
      case WMS_TP_CAUSE_ALPHABET_NOT_SUPPORTED:
         return AEESMS_ERROR_ALPHABET_NOT_SUPPORTED;

      case WMS_TP_CAUSE_MSG_CLASS_NOT_SUPPORTED:
         return AEESMS_ERROR_MSG_CLASS_NOT_SUPPORTED;

      case WMS_TP_CAUSE_SC_BUSY:
         return AEESMS_ERROR_SC_BUSY;

      case WMS_TP_CAUSE_NO_SC_SUBSCRIPTION:
         return AEESMS_ERROR_NO_SC_SUBSCRIPTION;

      case WMS_TP_CAUSE_SC_FAILURE:
         return AEESMS_ERROR_SC_FAILURE;

      default:
         return AEESMS_ERROR_GENERAL_PROBLEM;
   }
}

/*===========================================================================

  Translates RP cause to AEE error

===========================================================================*/
static int Xlate_wms_rp_cause_e_type_2_MOSMS_ERR(wms_rp_cause_e_type e)
{  
   switch(e) 
   {
      case WMS_RP_CAUSE_UNASSIGNED_NUMBER:
         return AEESMS_ERROR_UNASSIGNED_NUMBER;

      case WMS_RP_CAUSE_OPERATOR_DETERMINED_BARRING:
         return AEESMS_ERROR_OPERATOR_DETERMINED_BARRING;

      case WMS_RP_CAUSE_CALL_BARRED:
         return AEESMS_ERROR_CALL_BARRED;

      case WMS_RP_CAUSE_NETWORK_FAILURE:
         return AEESMS_ERROR_NETWORK_FAILURE;

      case WMS_RP_CAUSE_SM_REJECTED:
         return AEESMS_ERROR_SM_REJECTED;

      case WMS_RP_CAUSE_DESTINATION_OUT_OF_ORDER:
         return AEESMS_ERROR_DESTINATION_OUT_OF_ORDER;

      case WMS_RP_CAUSE_UNIDENTIFIED_SUBSCRIBER:
         return AEESMS_ERROR_UNIDENTIFIED_SUBSCRIBER;

      case WMS_RP_CAUSE_FACILITY_REJECTED:
         return AEESMS_ERROR_FACILITY_REJECTED;

      case WMS_RP_CAUSE_UNKNOWN_SUBSCRIBER:
         return AEESMS_ERROR_UNKNOWN_SUBSCRIBER;

      case WMS_RP_CAUSE_NETWORK_OUT_OF_ORDER:
         return AEESMS_ERROR_NETWORK_OUT_OF_ORDER;

      case WMS_RP_CAUSE_TEMPORARY_FAILURE:
         return AEESMS_ERROR_TEMPORARY_FAILURE;

      case WMS_RP_CAUSE_CONGESTION:
         return AEESMS_ERROR_CONGESTION;

      case WMS_RP_CAUSE_RESOURCES_UNAVAILABLE_UNSPECIFIED:
         return AEESMS_ERROR_RESOURCES_UNAVAILABLE_UNSPECIFIED;

      case WMS_RP_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED:
         return AEESMS_ERROR_REQUESTED_FACILITY_NOT_SUBSCRIBED;

      case WMS_RP_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED:
         return AEESMS_ERROR_REQUESTED_FACILITY_NOT_IMPLEMENTED;

      case WMS_RP_CAUSE_INVALID_SM_TRANSFER_REF_VALUE:
         return AEESMS_ERROR_INVALID_SM_TRANSFER_REF_VALUE;

      case WMS_RP_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE:
         return AEESMS_ERROR_SEMANTICALLY_INCORRECT_MESSAGE;

      case WMS_RP_CAUSE_INVALID_MANDATORY_INFORMATION:
         return AEESMS_ERROR_INVALID_MANDATORY_INFORMATION;

      case WMS_RP_CAUSE_MESSAGE_TYPE_NON_EXISTENT:
         return AEESMS_ERROR_MESSAGE_TYPE_NON_EXISTENT;

      case WMS_RP_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE:
         return AEESMS_ERROR_MESSAGE_TYPE_NOT_COMPATIBLE;

      case WMS_RP_CAUSE_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED:
         return AEESMS_ERROR_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED;

      case WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED:
         return AEESMS_ERROR_PROTOCOL_ERROR_UNSPECIFIED;

      case WMS_RP_CAUSE_INTERWORKING_UNSPECIFIED:
         return AEESMS_ERROR_INTERWORKING_UNSPECIFIED;

      default:
         return AEESMS_ERROR_GENERAL_PROBLEM;
   }
}

/*===========================================================================

  Translates wms_error_class_e_type to MOSMS error type
===========================================================================*/
static int Xlate_wms_error_class_e_type_2_MOSMS_ERRTYPE(wms_error_class_e_type e)
{
   switch (e)
   {
      case WMS_ERROR_NONE:
         return AEESMS_ERRORTYPE_NONE;

      case WMS_ERROR_TEMP:
         return AEESMS_ERRORTYPE_TEMP;

      case WMS_ERROR_PERM:
         return AEESMS_ERRORTYPE_PERM;

      default:
         return AEESMS_ERRORTYPE_UNKNOWN;
   }
}

/*===========================================================================

  Translates wms_report_status_e_type to MOSMS error type
===========================================================================*/
static int Xlate_wms_report_status_and_rp_cause_e_type_2_MOSMS_ERRTYPE(wms_report_status_e_type e, wms_rp_cause_e_type eRpCause)
{
   switch (e)
   {
      case WMS_RPT_OK:
         return AEESMS_ERRORTYPE_NONE;

      case WMS_RPT_LL_ERROR:
      case WMS_RPT_GENERAL_ERROR:
      case WMS_RPT_OUT_OF_RESOURCES:
      case WMS_RPT_NETWORK_NOT_READY:
      case WMS_RPT_PHONE_NOT_READY:
      case WMS_RPT_NO_ACK:
      case WMS_RPT_NOT_ALLOWED_IN_AMPS:
      case WMS_RPT_ACCESS_BLOCK:
      case WMS_RPT_SMR_TR1M_TIMEOUT:
      case WMS_RPT_SMR_TR2M_TIMEOUT:
      case WMS_RPT_SMR_NO_RESOURCES:
         return AEESMS_ERRORTYPE_TEMP;

      case WMS_RPT_MO_CONTROL_DISALLOW:
      case WMS_RPT_MO_CONTROL_ERROR:
      case WMS_RPT_CDMA_TL_ERROR:
      case WMS_RPT_ACCESS_TOO_LARGE: 
      case WMS_RPT_DC_TOO_LARGE:
      case WMS_RPT_CANNOT_SEND_BROADCAST:
         return AEESMS_ERRORTYPE_PERM;
      
      case WMS_RPT_RP_ERROR:
      case WMS_RPT_CP_ERROR:
         if (WMS_RP_CAUSE_TEMPORARY_FAILURE == eRpCause)
         {
            return AEESMS_ERRORTYPE_TEMP;
         }
         else
         {
            return AEESMS_ERRORTYPE_PERM;
         }

      default:
         return AEESMS_ERRORTYPE_UNKNOWN;
   }
}
  
/*===========================================================================

  Deletes OEMMsg instance

===========================================================================*/
void OEMSMS_DeleteMessage(OEMMsg* hMsg)
{
   if (gpWms && OEMWSMS_IsValidNode(hMsg, gpWms->pCurMOSMS)) 
   {
      if (hMsg->pPrev)
         hMsg->pPrev->pNext = hMsg->pNext;
      else
         gpWms->pCurMOSMS = hMsg->pNext;
      
      if (hMsg->pNext)
         hMsg->pNext->pPrev = hMsg->pPrev;

      if (!gpWms->pCurMOSMS)
      {
         gpWms->bTransmissionPending = FALSE;
         if (gpWms->bDedicatedChannel)
         {
            (void) wms_dc_disconnect(gpWms->clientId, OEMWSMS_NoopCommandCallback, NULL);
            gpWms->bDedicatedChannel = FALSE;
         }
      }
   }

   if (hMsg)
      sys_free(hMsg);
}

/*===========================================================================

   Deregisters Message, Event and Status listeners

===========================================================================*/
int OEMSMS_Cleanup(void)
{
   if (gpWms) 
   {
      wms_client_deactivate(gpWms->clientId);
      wms_client_release(gpWms->clientId);

      sys_free(gpWms);

      gpWms = 0;
   }

   return SUCCESS;
}

/*===========================================================================

   Sends SMS on traffic channel

===========================================================================*/
static wms_status_e_type OEMWMS_SendSMSOnTrafficChannel(OEMMsg * hMsg)
{
   OEMPhone * pPhone = NULL;
   wms_status_e_type wmsStatus = WMS_OK_S;

   //Check if the Traffic Channel is Already up. If yes, proceed with sending the msg
   if (SUCCESS == OEMPhone_New(&pPhone))
   {
      if (OEMPhone_InConversation(pPhone)) 
      {
         FARF(SMS, ("SMS in call"));

         // When message is sent (or failed), the function OEMWMS_MessageListener() is invoked 
         wms_msg_send(gpWms->clientId, 
                      &OEMWSMS_CommandCallback, 
                      (const void *)hMsg, 
                      WMS_SEND_MODE_CLIENT_MESSAGE, 
                      &hMsg->CMsg);
      }
      else 
      {      
         FARF(SMS, ("Raise the Traffic Channel"));
         
         // Raise the Traffic channel
         // Flag for sending over traffic channel
         gpWms->bTransmissionPending = TRUE;
         hMsg->nState = OEMMSG_STATE_SEND_ON_TRAFFIC;
         
#if (defined(WMS_API_VERSION) && (WMS_API_VERSION >= 0x0480) && (WMS_API_VERSION <= 0xFFFF))
         // Call with DC_SO_AUTO since WMS will then read the NV Item
         // If this succeeds, response falls in OEMWMS_EventListener()         
         wms_dc_connect( gpWms->clientId, 
                         OEMWSMS_CommandCallback, 
                         (const void *)hMsg, 
                         WMS_DC_SO_AUTO );
#else
         // If this succeeds, response falls in OEMWMS_EventListener()         
         wms_dc_connect( gpWms->clientId, 
                         OEMWSMS_CommandCallback, 
                         (const void *)hMsg, 
                         WMS_DC_SO_6 );
#endif // (defined(WMS_API_VERSION) && (WMS_API_VERSION >= 0x0480) && (WMS_API_VERSION <= 0xFFFF))
      }
      OEMPhone_Release(pPhone);
   }
   else
   {
      FARF(SMS, ("OEMPhone_New failed during MO SMS"));
      wmsStatus = WMS_OUT_OF_RESOURCES_S;
   }
   return wmsStatus;
}

/*===========================================================================

   OEMWMS_GetMemoryStatus

===========================================================================*/
static void OEMWMS_GetMemoryStatus(void* pUser)
{

   wms_status_e_type wmsStatus;

   int st = (int)pUser;

   FARF(SMS, ("OEMWMS_GetMemoryStatus"));

   wmsStatus = wms_cfg_get_memory_status(gpWms->clientId,
                                        OEMWSMS_NoopCommandCallback,
                                        NULL,
                                        Xlate_2_NativeStorageType(st),
                                        Xlate_2_NativeTagFromStorageType(st));
   if (wmsStatus != WMS_OK_S) 
   {
      FARF(SMS, ("wms_cfg_get_memory_status failed err = %d", wmsStatus));
   }
}

/*===========================================================================

   OEMWMS_GetMsgTransferRoute

===========================================================================*/
#ifndef FEATURE_WMS_APP
static void OEMWMS_GetMsgTransferRoute()
{
   wms_status_e_type wmsStatus;

   FARF(SMS, ("OEMWMS_GetMsgTransferRoute"));

   wmsStatus = wms_cfg_get_routes(gpWms->clientId,
                                  OEMWSMS_NoopCommandCallback,
                                  NULL);
   if (wmsStatus != WMS_OK_S) 
   {
      FARF(SMS, ("wms_cfg_get_routes failed err = %d", wmsStatus));
   }
}
#endif

#if defined( FEATURE_NOT_BREW_ONLY_SMS)&& defined (CUST_EDITION)
/*===========================================================================

   OEMWMS_SetPrimaryClient

===========================================================================*/
static void OEMWMS_SetPrimaryClient(void)
{
   wms_status_e_type wmsStatus;

   FARF(SMS, ("OEMWMS_SetPrimaryClient"));

#ifdef FEATURE_UIONE_HDK
   wmsStatus = wms_cfg_set_primary_client(gpWms->clientId,
                                          OEMWSMS_NoopCommandCallback,
                                          NULL,
                                          TRUE,
                                          TRUE);
#else
   wmsStatus = wms_cfg_set_primary_client(gpWms->clientId,
                                          OEMWSMS_NoopCommandCallback,
                                          NULL,
                                          TRUE,
                                          FALSE);
#endif /* FEATURE_UIONE_HDK */

   if (wmsStatus != WMS_OK_S) 
   {
      FARF(SMS, ("wms_cfg_set_primary_client failed err = %d", wmsStatus));
   }
}
#endif // #ifdef FEATURE_NOT_BREW_ONLY_SMS

/*===========================================================================

   OEMWMS_SetMemoryFull

===========================================================================*/
static void OEMWMS_SetMemoryFull(boolean bFull)
{
   wms_status_e_type wmsStatus;

   FARF(SMS, ("OEMWMS_SetMemoryFull"));

   wmsStatus = wms_cfg_set_memory_full(gpWms->clientId,
                                       OEMWSMS_NoopCommandCallback,
                                       NULL,
                                       bFull);

   if (wmsStatus != WMS_OK_S) 
   {
      FARF(SMS, ("wms_cfg_set_memory_full failed err = %d", wmsStatus));
   }
}

/*===========================================================================

   OEMWMS_GetDomainPref

===========================================================================*/
static void OEMWMS_GetDomainPref()
{
   wms_status_e_type wmsStatus;

   FARF(SMS, ("OEMWMS_GetDomainPref"));

   wmsStatus = wms_cfg_get_gw_domain_pref(gpWms->clientId,
                                          OEMWSMS_NoopCommandCallback,
                                          NULL);

   if (wmsStatus != WMS_OK_S) 
   {
      FARF(SMS, ("wms_cfg_get_gw_domain_pref failed err = %d", wmsStatus));
   }
}

/*===========================================================================

   OEMSMS_GetDomainPref

===========================================================================*/
int OEMSMS_GetDomainPref(int * pgwDomainPref)
{
   FARF(SMS, ("OEMSMS_GetDomainPref"));

   if (!pgwDomainPref)
   {
      return EBADPARM;
   }

   if (gpWms)
   {
      *pgwDomainPref = Xlate_DomainPref(gpWms->gwDomainPref);
      return SUCCESS;
   }
   else
   {
      return EFAILED;
   }
}

/*===========================================================================

   OEMSMS_SetDomainPref

===========================================================================*/
int OEMSMS_SetDomainPref(int gwDomainPref)
{
   FARF(SMS, ("OEMSMS_SetDomainPref"));

   if (!gpWms)
   {
      return EFAILED;
   }

   if (gwDomainPref < AEESMS_GW_DOMAIN_PREF_LAST)
   {
      wms_status_e_type wmsStatus;
      wmsStatus = wms_cfg_set_gw_domain_pref(gpWms->clientId,
                                             OEMWSMS_NoopCommandCallback,
                                             NULL,
                                             Xlate_2_NativeDomainPref(gwDomainPref));

      if (wmsStatus != WMS_OK_S) 
      {
         FARF(SMS, ("wms_cfg_set_gw_domain_pref failed err = %d", wmsStatus));
         return EFAILED;
      }
      return SUCCESS;
   }
   else
   {
      return EBADPARM;
   }
}

/*===========================================================================

   OEMWMS_GetMessageList

===========================================================================*/
static void OEMWMS_GetMessageList(void* pUser)
{
   wms_status_e_type wmsStatus;

   int st = (int)pUser;

   FARF(SMS, ("OEMWMS_GetMessageList"));

   wmsStatus = wms_cfg_get_message_list(gpWms->clientId,
                                        OEMWSMS_NoopCommandCallback,
                                        NULL,
                                        Xlate_2_NativeStorageType(st),
                                        Xlate_2_NativeTagFromStorageType(st));
   if (wmsStatus != WMS_OK_S) 
   {
      FARF(SMS, ("wms_cfg_get_message_list failed err = %d", wmsStatus));
   }
}

/*===========================================================================

   Returns TRUE if storage is initialized. Returns FALSE otherwise.

===========================================================================*/
boolean OEMSMS_IsStorageInitialized(AEESMSStorageType st)
{        
   if (st >= OEMSMS_STORAGE_LAST)
      return FALSE;

   return (gpWms->bStrInit[st]);
}

/*===========================================================================

   Returns storage status

===========================================================================*/  
void OEMSMS_GetStorageStatus(AEESMSStorageType st, AEESMSTag mt, AEESMSStorageStatus * pss, uint32 * pnRv, AEECallback *pcb)
{
   int i, nErr = SUCCESS;

   FARF(SMS, ("OEMSMS_GetStorageStatus"));

   if (st < OEMSMS_STORAGE_LAST)
   {
      if (gpWms->bStrInit[st] == FALSE) 
      {
         nErr = EFAILED;
         goto Done;
      }

      MEMSET(pss, 0, sizeof(AEESMSStorageStatus));

      for(i = 0; i < gpWms->ucStrSMSCount[st]; i++)
      {
         if (gpWms->strSMSMsgList[st][i] == WMS_TAG_NONE)
            pss->nFreeSlots++;
         else if ((AEESMS_TAG_NONE == mt) ||
                  (gpWms->strSMSMsgList[st][i] == Xlate_2_NativeTag(mt)))
            pss->nUsedTagSlots++;
      }
      pss->nMaxSlots = gpWms->ucStrSMSCount[st];
      pss->nSize = sizeof(AEESMSStorageStatus);
   }
   else
   {
      nErr = EBADPARM;
      goto Done;
   }

Done:
   *pnRv = nErr;
   AEE_SYS_RESUME(pcb);
}

/*===========================================================================

   Returns system mode corresponding to storage type

===========================================================================*/  
static AEETSysMode Xlate_StorageTypeToSystemMode(AEESMSStorageType st)
{
   if (st == AEESMS_CARD_RUIM || 
       st == AEESMS_NV_CDMA || 
       st == AEESMS_CARD_RUIM_TEMPLATE || 
       st == AEESMS_NV_CDMA_TEMPLATE)
   {
      return AEET_SYS_MODE_CDMA;
   }
   else if (st == AEESMS_CARD_SIM || 
            st == AEESMS_NV_GW || 
            st == AEESMS_CARD_SIM_TEMPLATE || 
            st == AEESMS_NV_GW_TEMPLATE || 
            st == AEESMS_CARD_SIM_STATUS_REPORT)
   {
      return AEET_SYS_MODE_GSM;
   }
   else
   {
      return AEET_SYS_MODE_NO_SRV;
   }
}

/*===========================================================================

   Returns WMS mode corresponding to storage type

===========================================================================*/  
static wms_message_mode_e_type Xlate_StorageTypeToWMSMode(AEESMSStorageType st)
{
   if (st == AEESMS_CARD_RUIM || 
       st == AEESMS_NV_CDMA || 
       st == AEESMS_CARD_RUIM_TEMPLATE || 
       st == AEESMS_NV_CDMA_TEMPLATE)
   {
      return WMS_MESSAGE_MODE_CDMA;
   }
   else if (st == AEESMS_CARD_SIM || 
            st == AEESMS_NV_GW || 
            st == AEESMS_CARD_SIM_TEMPLATE || 
            st == AEESMS_NV_GW_TEMPLATE || 
            st == AEESMS_CARD_SIM_STATUS_REPORT)
   {
      return WMS_MESSAGE_MODE_GW;
   }
   else
   {
      return WMS_MESSAGE_MODE_MAX;
   }
}

/*===========================================================================

   Stores a message on the card

===========================================================================*/  
void OEMSMS_StoreMsg(AEESMSStorageType st, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 * pnIndex, uint32 *pnRv, AEECallback *pcb)
{
   int nErr = SUCCESS, nIndex; 
   OEMMsg *hMsg = NULL;

   FARF(SMS, ("OEMSMS_StoreMsg"));

   if (st < OEMSMS_STORAGE_LAST)
   {
      if (gpWms->bStrInit[st] == FALSE) 
      {
         nErr = EFAILED;
         goto Done;
      }

      for(nIndex = 0; nIndex < gpWms->ucStrSMSCount[st]; nIndex++)
      {
         if (gpWms->strSMSMsgList[st][nIndex] == WMS_TAG_NONE)
            break;
      }

      if (nIndex >= gpWms->ucStrSMSCount[st])
      {
         nErr = ENOPERSISTMEMORY;
         goto Done;
      }
   }
   else
   {
      nErr = EBADPARM;
      goto Done;
   }
   nErr = OEMSMS_NewMessage(pMsgData, pMsgPayload, Xlate_StorageTypeToSystemMode(st), &hMsg);

   if (AEE_SUCCESS == nErr) 
   {
      hMsg->CMsg.msg_hdr.mem_store = Xlate_2_NativeStorageType(st);
      hMsg->CMsg.msg_hdr.message_mode = Xlate_StorageTypeToWMSMode(st);

      hMsg->CMsg.msg_hdr.index = nIndex;
      hMsg->pnIndex = pnIndex;
      hMsg->dwTag = pMsgData->dwTag;
   
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = hMsg;
      pcb->pfnCancel   = OEMWMS_ReleaseStoreSMS;

      FARF(SMS, ("pnIndex: 0x%x, *pnIndex: %d", hMsg->pnIndex, *hMsg->pnIndex));

      OEMWMS_StoreSMS(hMsg, pcb, pnRv);
   }

Done:
   if (SUCCESS != nErr) 
   {
      *pnRv = nErr;
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================

   Updates a message on the card

===========================================================================*/  
void OEMSMS_UpdateMsg(AEESMSStorageType st, uint32 nIndex, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 *pnRv, AEECallback *pcb)
{
   int nErr = SUCCESS; 
   OEMMsg *hMsg = NULL;

   FARF(SMS, ("OEMSMS_UpdateMsg"));

   if (st < OEMSMS_STORAGE_LAST)
   {
      if (gpWms->bStrInit[st] == FALSE) 
      {
         nErr = EFAILED;
         goto Done;
      }

      if (nIndex >= gpWms->ucStrSMSCount[st])
      {
         nErr = EBADPARM;
         goto Done;
      }

      if (gpWms->strSMSMsgList[st][nIndex] == WMS_TAG_NONE)
      {
         nErr = EBADPARM;
         goto Done;
      }
   }
   else
   {
      nErr = EBADPARM;
      goto Done;
   }

   nErr = OEMSMS_NewMessage(pMsgData, pMsgPayload, Xlate_StorageTypeToSystemMode(st), &hMsg);

   if (AEE_SUCCESS == nErr) 
   {
      gpWms->strSMSMsgList[st][nIndex] = Xlate_2_NativeTag(pMsgData->dwTag);
      hMsg->CMsg.msg_hdr.mem_store = Xlate_2_NativeStorageType(st);
      hMsg->CMsg.msg_hdr.message_mode = Xlate_StorageTypeToWMSMode(st);

      hMsg->CMsg.msg_hdr.index = nIndex;
      hMsg->pnIndex = NULL;
      hMsg->dwTag = pMsgData->dwTag;
   
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = hMsg;
      pcb->pfnCancel   = OEMWMS_ReleaseStoreSMS;
      OEMWMS_StoreSMS(hMsg, pcb, pnRv);
   }

Done:
   if (SUCCESS != nErr) 
   {
      *pnRv = nErr;
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================

  Called to store SMS

===========================================================================*/
static void OEMWMS_StoreSMS(OEMMsg* hMsg, AEECallback *pcb, uint32 *pdwStatus)
{
   int nErr = SUCCESS;

   FARF(SMS, ("==>  OEMWMS_StoreSMS"));

   if (!gpWms) 
   {
      nErr = EFAILED;
      goto Done;
   }

   hMsg->pcb = pcb;
   hMsg->pdwStatus = pdwStatus;

   if (gpWms->pCurStoreSMS) 
   {
      hMsg->pNext = gpWms->pCurStoreSMS;
      gpWms->pCurStoreSMS->pPrev = hMsg;
   }

   gpWms->pCurStoreSMS = hMsg;

   if (hMsg->dwTag == AEESMS_TAG_MO_TEMPLATE)
   {
      if (hMsg->pnIndex)
      {
         FARF(SMS, ("Insert Template: 0x%x", hMsg->pnIndex));
         wms_msg_write_template( gpWms->clientId, 
                                 OEMWSMS_CommandCallback,
                                 (const void *)hMsg,
                                 WMS_WRITE_MODE_INSERT,
                                 &hMsg->CMsg);
      }
      else
      {
         FARF(SMS, ("Replace Template: 0x%x", hMsg->pnIndex));
         wms_msg_write_template( gpWms->clientId, 
                                 OEMWSMS_CommandCallback,
                                 (const void *)hMsg,
                                 WMS_WRITE_MODE_REPLACE,
                                 &hMsg->CMsg);
      }
   }
   else if (hMsg->dwTag == AEESMS_TAG_STATUS_RPT)
   {
      if (hMsg->pnIndex)
      {
         FARF(SMS, ("Insert Status Report: 0x%x", hMsg->pnIndex));
         wms_msg_write_sts_report(gpWms->clientId, 
                                  OEMWSMS_CommandCallback,
                                  (const void *)hMsg,
                                  WMS_WRITE_MODE_INSERT,
                                  &hMsg->CMsg);
      }
      else
      {
         FARF(SMS, ("Replace Status Report: 0x%x", hMsg->pnIndex));
         wms_msg_write_sts_report(gpWms->clientId, 
                                  OEMWSMS_CommandCallback,
                                  (const void *)hMsg,
                                  WMS_WRITE_MODE_REPLACE,
                                  &hMsg->CMsg);
      }
   }
   else
   {
      if (hMsg->pnIndex)
      {
         FARF(SMS, ("Insert Message: 0x%x", hMsg->pnIndex));
         wms_msg_write(gpWms->clientId, 
                     OEMWSMS_CommandCallback,
                     (const void *)hMsg,
                     WMS_WRITE_MODE_INSERT,
                     &hMsg->CMsg);
      }
      else
      {
         FARF(SMS, ("Replace Message: 0x%x", hMsg->pnIndex));
         wms_msg_write(gpWms->clientId, 
                     OEMWSMS_CommandCallback,
                     (const void *)hMsg,
                     WMS_WRITE_MODE_REPLACE,
                     &hMsg->CMsg);
      }
   }

Done:
   if (nErr != SUCCESS)
   {
      *pdwStatus = nErr;
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================

  Called to release store SMS struct

===========================================================================*/
static void OEMWMS_ReleaseStoreSMS(AEECallback *pcb)
{
   FARF(SMS, ("==>  OEMWMS_ReleaseStoreSMS"));

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if (pcb->pfnCancel != OEMWMS_ReleaseStoreSMS)
      return;
   
   OEMWMS_DeleteStoreSMS((OEMMsg*)pcb->pCancelData);

   pcb->pCancelData = 0;
   pcb->pfnCancel   = 0;
}

/*===========================================================================

  Deletes OEMMsg instance for Store SMS

===========================================================================*/
static void OEMWMS_DeleteStoreSMS(OEMMsg * hMsg)
{
   if (gpWms && OEMWSMS_IsValidNode(hMsg, gpWms->pCurStoreSMS)) 
   {
      if (hMsg->pPrev)
         hMsg->pPrev->pNext = hMsg->pNext;
      else
         gpWms->pCurStoreSMS = hMsg->pNext;
      
      if (hMsg->pNext)
         hMsg->pNext->pPrev = hMsg->pPrev;
   }

   if (hMsg)
      sys_free(hMsg);
}

/*===========================================================================

   Sets auto storage status for the messages of the specified class

===========================================================================*/
void OEMSMS_AutoStoreMsg(AEESMSClass mc, AEESMSStorageType st, uint32 *pnRv, AEECallback *pcb)
{   
   int nErr = SUCCESS; 
   OEMAutoStoreMsg *hMsg;

   FARF(SMS, ("OEMSMS_AutoStoreMsg"));

   if (mc > AEESMS_CLASS_CDMA)
   {
      nErr = EBADPARM;
      goto Done;
   }

   hMsg = sys_malloc(sizeof(OEMAutoStoreMsg));

   if (hMsg) 
   {   
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = hMsg;
      pcb->pfnCancel   = OEMWMS_ReleaseAutoStoreMsg;
      OEMWMS_AutoStoreMsg(hMsg, pcb, pnRv, mc, Xlate_2_NativeStorageType(st));
   }
   else
      nErr = ENOMEMORY;

Done:
   if (SUCCESS != nErr) 
   {
      *pnRv = nErr;
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================

  Called to enable/disable auto store on messages of the specified class

===========================================================================*/
static void OEMWMS_AutoStoreMsg(OEMAutoStoreMsg* hMsg, AEECallback *pcb, uint32 * pdwStatus, uint32 mc, wms_memory_store_e_type wmsMemoryStore)
{
   int nMsgClass, nErr = SUCCESS;
   wms_routes_s_type routes; 

   FARF(SMS, ("==>  OEMWMS_AutoStoreMsg"));

   if (!gpWms) 
   {
      nErr = EFAILED;
      goto Done;
   }

   hMsg->pcb = pcb;
   hMsg->pdwStatus = pdwStatus;

   if (gpWms->pCurAutoStore) 
   {
      hMsg->pNext = gpWms->pCurAutoStore;
      gpWms->pCurAutoStore->pPrev = hMsg;
   }

   gpWms->pCurAutoStore = hMsg;

   for(nMsgClass = 0; nMsgClass < WMS_MESSAGE_CLASS_MAX; nMsgClass++)
   {
     routes.pp_routes[nMsgClass].route = WMS_ROUTE_NO_CHANGE;
     routes.pp_routes[nMsgClass].mem_store = WMS_MEMORY_STORE_NONE;
   }

   if (WMS_MEMORY_STORE_NONE == wmsMemoryStore)
   {
      routes.pp_routes[mc].route = WMS_ROUTE_TRANSFER_ONLY;
      routes.pp_routes[mc].mem_store = WMS_MEMORY_STORE_NONE;
   }
   else
   {      
      routes.pp_routes[mc].route = WMS_ROUTE_STORE_AND_NOTIFY;
      routes.pp_routes[mc].mem_store = wmsMemoryStore;
   }

   wms_cfg_set_routes(gpWms->clientId,
                      OEMWSMS_CommandCallback,
                     (const void *)hMsg,
                      &routes);

Done:
   if (nErr != SUCCESS)
   {
      *pdwStatus = nErr;
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================

  Called to release auto store struct

===========================================================================*/
static void OEMWMS_ReleaseAutoStoreMsg(AEECallback *pcb)
{
   FARF(SMS, ("==>  OEMWMS_ReleaseAutoStoreMsg"));

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if (pcb->pfnCancel != OEMWMS_ReleaseAutoStoreMsg)
      return;
   
   OEMWMS_DeleteAutoStoreMsg((OEMAutoStoreMsg*)pcb->pCancelData);

   pcb->pCancelData = 0;
   pcb->pfnCancel   = 0;
}

/*===========================================================================

  Deletes OEMAutoStoreMsg instance for auto store request

===========================================================================*/
static void OEMWMS_DeleteAutoStoreMsg(OEMAutoStoreMsg * hMsg)
{
   if (gpWms && OEMWSMS_IsValidNode(hMsg, gpWms->pCurAutoStore)) 
   {
      if (hMsg->pPrev)
         hMsg->pPrev->pNext = hMsg->pNext;
      else
         gpWms->pCurAutoStore = hMsg->pNext;
      
      if (hMsg->pNext)
         hMsg->pNext->pPrev = hMsg->pPrev;
   }

   if (hMsg)
      sys_free(hMsg);
}

/*===========================================================================

   OEMWMS_NotifyMemoryEvent - Notifies specified event for storage type

===========================================================================*/  
static void OEMWMS_NotifyMemoryEvent
(
   AEESMSStorageType st,
   uint32 nEvent
)
{
   OEMStorageUpdateData * pUpdateData = NULL;

   if (gpWms->pfnNotifyOnStorageUpdate)
   {
      pUpdateData = (OEMStorageUpdateData *)&(gpWms->sUpdateData);
      pUpdateData->nEvent = nEvent;
      pUpdateData->sEventData.st = st;
      gpWms->pfnNotifyOnStorageUpdate(pUpdateData);
   }
}

/*===========================================================================

   Listener Invoked on WMS_MSG_EVENT_WRITE

===========================================================================*/  
static void OEMWMS_StoreSMSListener
(
   wms_msg_event_info_s_type * info_ptr
)
{
   int nStrIdx;
   OEMMsg  *hMsg = (OEMMsg *)info_ptr->status_info.user_data;

   if (!gpWms)
   {
      return;
   }
   
   // Special Handling for Reading the TL Ack Requested
   if ( (info_ptr->status_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
        &&(info_ptr->status_info.message.msg_hdr.tag == WMS_TAG_MO_TEMPLATE) 
        &&(info_ptr->status_info.message.msg_hdr.mem_store == WMS_MEMORY_STORE_NV_CDMA) )
   {
      if (info_ptr->status_info.message.u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION)
      {
         gpWms->nIsTLAckRequested = 1;
      }
      else
      {
         gpWms->nIsTLAckRequested = 0;
      }
   }

   nStrIdx = Xlate_StorageType(info_ptr->status_info.message.msg_hdr.mem_store, 
                               info_ptr->status_info.message.msg_hdr.tag);
   
   if ((0 <= nStrIdx) && (nStrIdx < OEMSMS_STORAGE_LAST) &&
       (WMS_DUMMY_MESSAGE_INDEX != info_ptr->status_info.message.msg_hdr.index))
   {
      OEMStorageUpdateData * pUpdateData = NULL;

      if (gpWms->pfnNotifyOnStorageUpdate)
      {
         pUpdateData = (OEMStorageUpdateData *)&(gpWms->sUpdateData);
         pUpdateData->nEvent = (WMS_WRITE_MODE_INSERT == info_ptr->status_info.write_mode ? EVT_MDL_SMSSTORAGE_STORE : EVT_MDL_SMSSTORAGE_UPDATE);
         pUpdateData->sEventData.st = nStrIdx;
         pUpdateData->sEventData.nIndex = info_ptr->status_info.message.msg_hdr.index;
         pUpdateData->sEventData.mt = Xlate_Tag(info_ptr->status_info.message.msg_hdr.tag);
         gpWms->pfnNotifyOnStorageUpdate(pUpdateData);
      }
   }

   if (OEMWSMS_IsValidNode(hMsg, gpWms->pCurStoreSMS))
   {
      if ((0 <= nStrIdx) && (nStrIdx < OEMSMS_STORAGE_LAST) &&
         (WMS_DUMMY_MESSAGE_INDEX != info_ptr->status_info.message.msg_hdr.index))
      {
         *hMsg->pdwStatus = SUCCESS;

         if (hMsg->pnIndex)
         {
            *hMsg->pnIndex = info_ptr->status_info.message.msg_hdr.index;     
            gpWms->strSMSMsgList[nStrIdx][info_ptr->status_info.message.msg_hdr.index] = Xlate_2_NativeTag(hMsg->dwTag);
         }
      }
      else
         *hMsg->pdwStatus = EFAILED;
      
      AEE_SYS_RESUME(hMsg->pcb);
   }
}

/*===========================================================================

   Reads a message from the card

===========================================================================*/  
void OEMSMS_ReadMsg(AEESMSStorageType st, uint32 nIndex, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 * pnRv, AEECallback *pcb)
{
   int nErr = SUCCESS; 
   void *hMsg;

   FARF(SMS, ("OEMSMS_ReadMsg"));

   if (st < OEMSMS_STORAGE_LAST)
   {
      if (gpWms->bStrInit[st] == FALSE) 
      {
         nErr = EFAILED;
         goto Done;
      }

      if (nIndex >= gpWms->ucStrSMSCount[st])
      {
         nErr = EFAILED;
         goto Done;
      }

      if (gpWms->strSMSMsgList[st][nIndex] == WMS_TAG_NONE)
      {
         nErr = EBADPARM;
         goto Done;
      }
   }
   else
   {
      nErr = EBADPARM;
      goto Done;
   }

   hMsg = sys_malloc(sizeof(OEMReadMsg));

   if (hMsg) 
   {   
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = hMsg;
      pcb->pfnCancel   = OEMWMS_ReleaseReadSMS;
      OEMWMS_ReadSMS(hMsg, pcb, pnRv, pMsgData, pMsgPayload, nIndex, st);
   }
   else
      nErr = ENOMEMORY;

Done:
   if (SUCCESS != nErr) 
   {
      *pnRv = nErr;
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================

  Called to read SMS

===========================================================================*/
static void OEMWMS_ReadSMS(OEMReadMsg* hMsg, AEECallback *pcb, uint32 *pdwStatus, OEMMsgData *pMsgData, OEMMsgPayload *pMsgPayload, uint32 nIndex, int st)
{
   int nErr = SUCCESS;

   FARF(SMS, ("==>  OEMWMS_ReadSMS"));

   if (!gpWms) 
   {
      nErr = EFAILED;
      goto Done;
   }

   hMsg->pcb = pcb;
   hMsg->pdwStatus = pdwStatus;
   hMsg->pMsgData = pMsgData;
   hMsg->pMsgPayload = pMsgPayload;

   if (gpWms->pCurReadSMS) 
   {
      hMsg->pNext = gpWms->pCurReadSMS;
      gpWms->pCurReadSMS->pPrev = hMsg;
   }

   gpWms->pCurReadSMS = hMsg;

   switch (st)
   {
      case AEESMS_CARD_RUIM:
      case AEESMS_CARD_SIM:
      case AEESMS_NV_CDMA:
      case AEESMS_NV_GW:
         wms_msg_read(gpWms->clientId, 
                      OEMWSMS_CommandCallback,
                      (const void *)hMsg,
                      Xlate_2_NativeStorageType(st),
                      nIndex);
         break;

      case AEESMS_CARD_RUIM_TEMPLATE:
      case AEESMS_CARD_SIM_TEMPLATE:
      case AEESMS_NV_CDMA_TEMPLATE:
      case AEESMS_NV_GW_TEMPLATE:
         wms_msg_read_template(gpWms->clientId, 
                               OEMWSMS_CommandCallback,
                               (const void *)hMsg,
                               Xlate_2_NativeStorageType(st),
                               nIndex);
         break;

      case AEESMS_CARD_SIM_STATUS_REPORT:
         wms_msg_read_sts_report(gpWms->clientId, 
                                 OEMWSMS_CommandCallback,
                                 (const void *)hMsg,
                                 Xlate_2_NativeStorageType(st),
                                 nIndex);
         break;

      default:
         nErr = EFAILED;
         goto Done;
   }

Done:
   if (nErr != SUCCESS)
   {
      *pdwStatus = nErr;
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================

  Called to release read SMS struct

===========================================================================*/
static void OEMWMS_ReleaseReadSMS(AEECallback *pcb)
{
   FARF(SMS, ("==>  OEMWMS_ReleaseReadSMS"));

  //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if (pcb->pfnCancel != OEMWMS_ReleaseReadSMS)
      return;
   
   OEMWMS_DeleteReadSMS((OEMReadMsg*)pcb->pCancelData);

   pcb->pCancelData = 0;
   pcb->pfnCancel   = 0;
}

/*===========================================================================

  Deletes OEMReadMsg instance for Read SMS

===========================================================================*/
static void OEMWMS_DeleteReadSMS(OEMReadMsg * hMsg)
{
   if (gpWms && OEMWSMS_IsValidNode(hMsg, gpWms->pCurReadSMS)) 
   {
      if (hMsg->pPrev)
         hMsg->pPrev->pNext = hMsg->pNext;
      else
         gpWms->pCurReadSMS = hMsg->pNext;
      
      if (hMsg->pNext)
         hMsg->pNext->pPrev = hMsg->pPrev;
   }

   if (hMsg)
      sys_free(hMsg);
}

/*===========================================================================

   Listener Invoked on WMS_MSG_EVENT_READ

===========================================================================*/  
static void OEMWMS_ReadSMSListener
(
   wms_msg_event_info_s_type * info_ptr
)
{
   OEMReadMsg  *hMsg = (OEMReadMsg *)info_ptr->status_info.user_data;

   if (-1 == gpWms->nIsTLAckRequested)
   {
      // Special Handling for Reading the TL Ack Requested
      if ( (info_ptr->status_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
         &&(info_ptr->status_info.message.msg_hdr.tag == WMS_TAG_MO_TEMPLATE) 
         &&(info_ptr->status_info.message.msg_hdr.mem_store == WMS_MEMORY_STORE_NV_CDMA) )
      {
         if (info_ptr->status_info.message.u.cdma_template.mask & WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION)
         {
            gpWms->nIsTLAckRequested = 1;
         }
         else
         {
            gpWms->nIsTLAckRequested = 0;
         }
      }
   }
   
   if (!gpWms || !OEMWSMS_IsValidNode(hMsg, gpWms->pCurReadSMS))
   {
      return;
   }

   MEMSET(&sce, 0, sizeof(sce));

   if (OEMWMS_CopyMessage(0,&info_ptr->status_info.message,&sce) == FALSE) 
   {
      *hMsg->pdwStatus = EFAILED;
   }
   else
   {
      *hMsg->pdwStatus = SUCCESS;
      OEMSMS_CopyPayloadFromCache(&sce, hMsg->pMsgPayload);
      OEMSMS_CopyPayloadDataFromCache(&sce, hMsg->pMsgData);
   }

   AEE_SYS_RESUME(hMsg->pcb);
}

/*===========================================================================

   Removes a message from the card

===========================================================================*/  
void OEMSMS_RemoveMsg(AEESMSStorageType st, AEESMSTag mt, uint32 nIndex, uint32 * pnRv, AEECallback *pcb)
{
   int nErr = SUCCESS; 
   OEMDeleteMsg *hMsg;

   FARF(SMS, ("OEMSMS_RemoveMsg"));

   if (st < OEMSMS_STORAGE_LAST)
   {
      if (gpWms->bStrInit[st] == FALSE) 
      {
         nErr = EFAILED;
         goto Done;
      }

      if (nIndex >= gpWms->ucStrSMSCount[st] && nIndex != 0xffffffff)
      {
         nErr = EFAILED;
         goto Done;
      }
   }
   else
   {
      nErr = EBADPARM;
      goto Done;
   }

   hMsg = sys_malloc(sizeof(OEMDeleteMsg));

   if (hMsg) 
   {   
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = hMsg;
      pcb->pfnCancel   = OEMWMS_ReleaseDeleteSMS;
      OEMWMS_DeleteSMS(hMsg, pcb, pnRv, nIndex, st, mt);
   }
   else
      nErr = ENOMEMORY;

Done:
   if (SUCCESS != nErr) 
   {
      *pnRv = nErr;
      AEE_SYS_RESUME(pcb);
   }
}

/*===========================================================================

  Called to delete stored SMS

===========================================================================*/
static void OEMWMS_DeleteSMS(OEMDeleteMsg* hMsg, AEECallback *pcb, uint32 *pdwStatus, uint32 nIndex, AEESMSStorageType st, AEESMSTag mt)
{
   int nErr = SUCCESS;

   FARF(SMS, ("==>  OEMWMS_DeleteSMS"));

   if (!gpWms) 
   {
      nErr = EFAILED;
      goto Done;
   }

   hMsg->pcb = pcb;
   hMsg->pdwStatus = pdwStatus;

   if (gpWms->pCurDeleteSMS) 
   {
      hMsg->pNext = gpWms->pCurDeleteSMS;
      gpWms->pCurDeleteSMS->pPrev = hMsg;
   }

   gpWms->pCurDeleteSMS = hMsg;

   if (((AEESMS_CARD_RUIM_TEMPLATE == st) || (AEESMS_CARD_SIM_TEMPLATE == st) || 
        (AEESMS_NV_CDMA_TEMPLATE == st) || (AEESMS_NV_GW_TEMPLATE == st)) && 
       ((AEESMS_TAG_MO_TEMPLATE == mt) || (AEESMS_TAG_NONE == mt)))
   {
      if (nIndex != 0xffffffff)
      {
         wms_msg_delete_template(gpWms->clientId, 
                                 OEMWSMS_CommandCallback,
                                 (const void *)hMsg,
                                 Xlate_2_NativeStorageType(st),
                                 nIndex);
      }
      else
      {
         wms_msg_delete_template_all(gpWms->clientId, 
                                     OEMWSMS_CommandCallback,
                                     (const void *)hMsg,
                                     Xlate_2_NativeStorageType(st));
      }
   }
   else if ((AEESMS_CARD_SIM_STATUS_REPORT == st) && 
            ((AEESMS_TAG_STATUS_RPT == mt) || (AEESMS_TAG_NONE == mt)))
   {
      if (nIndex != 0xffffffff)
      {
         wms_msg_delete_sts_report(gpWms->clientId, 
                                   OEMWSMS_CommandCallback,
                                   (const void *)hMsg,
                                   Xlate_2_NativeStorageType(st),
                                   nIndex);
      }
      else
      {
         wms_msg_delete_sts_report_all(gpWms->clientId, 
                                       OEMWSMS_CommandCallback,
                                       (const void *)hMsg,
                                       Xlate_2_NativeStorageType(st));
      }
   }
   else if (((AEESMS_CARD_RUIM == st) || (AEESMS_CARD_SIM == st) || 
             (AEESMS_NV_CDMA == st) || (AEESMS_NV_GW == st)) &&
            (AEESMS_TAG_MO_TEMPLATE != mt) &&
            (AEESMS_TAG_STATUS_RPT != mt))
   {
      if (nIndex != 0xffffffff)
      {
         wms_msg_delete(gpWms->clientId, 
                        OEMWSMS_CommandCallback,
                        (const void *)hMsg,
                        Xlate_2_NativeStorageType(st),
                        nIndex);
      }
      else
      {
         wms_msg_delete_all(gpWms->clientId, 
                            OEMWSMS_CommandCallback,
                            (const void *)hMsg,
                            Xlate_2_NativeStorageType(st),
                            Xlate_2_NativeTag(mt));
      }
   }
   else {
      nErr = EBADPARM;
   }

Done:
   if (nErr != SUCCESS)
   {
      *pdwStatus = nErr;
      AEE_SYS_RESUME(pcb);
   }
}


/*===========================================================================

  Called to release delete SMS struct

===========================================================================*/
static void OEMWMS_ReleaseDeleteSMS(AEECallback *pcb)
{
   FARF(SMS, ("==>  OEMWMS_ReleaseDeleteSMS"));

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if (pcb->pfnCancel != OEMWMS_ReleaseDeleteSMS)
      return;
   
   OEMWMS_DeleteDeleteSMS((OEMDeleteMsg*)pcb->pCancelData);

   pcb->pCancelData = 0;
   pcb->pfnCancel   = 0;
}


/*===========================================================================

  Deletes OEMDeleteMsg instance for Delete SMS

===========================================================================*/
static void OEMWMS_DeleteDeleteSMS(OEMDeleteMsg * hMsg)
{
   if (gpWms && OEMWSMS_IsValidNode(hMsg, gpWms->pCurDeleteSMS)) 
   {
      if (hMsg->pPrev)
         hMsg->pPrev->pNext = hMsg->pNext;
      else
         gpWms->pCurDeleteSMS = hMsg->pNext;
      
      if (hMsg->pNext)
         hMsg->pNext->pPrev = hMsg->pPrev;
   }

   if (hMsg)
      sys_free(hMsg);
}

/*===========================================================================

   Listener Invoked on WMS_MSG_EVENT_DELETE and WMS_MSG_EVENT_DELETE_ALL

===========================================================================*/  
static void OEMWMS_DeleteSMSListener
(
   wms_msg_event_e_type event,
   wms_msg_event_info_s_type * info_ptr
)
{
   int nStrIdx;
   OEMDeleteMsg *hMsg = (OEMDeleteMsg *)info_ptr->status_info.user_data;
   OEMStorageUpdateData * pUpdateData = NULL;

   if (!gpWms)
   {
      return;
   }

   nStrIdx = Xlate_StorageType(info_ptr->status_info.message.msg_hdr.mem_store, info_ptr->status_info.message.msg_hdr.tag);
      
   if ((0 <= nStrIdx) && (nStrIdx < OEMSMS_STORAGE_LAST) &&
       gpWms->pfnNotifyOnStorageUpdate)
   {
      pUpdateData = (OEMStorageUpdateData *)&(gpWms->sUpdateData);
      pUpdateData->sEventData.st = nStrIdx;
      pUpdateData->sEventData.nIndex = info_ptr->status_info.message.msg_hdr.index;
      pUpdateData->sEventData.mt = Xlate_Tag(info_ptr->status_info.message.msg_hdr.tag);
      if (WMS_MSG_EVENT_DELETE == event)
         pUpdateData->nEvent = EVT_MDL_SMSSTORAGE_DELETE;
      else
         pUpdateData->nEvent = (AEESMS_TAG_NONE == pUpdateData->sEventData.mt ? EVT_MDL_SMSSTORAGE_DELETE_ALL : EVT_MDL_SMSSTORAGE_DELETE_TAG);
      gpWms->pfnNotifyOnStorageUpdate(pUpdateData);
   }

   if (OEMWSMS_IsValidNode(hMsg, gpWms->pCurDeleteSMS))
   {
      *hMsg->pdwStatus = SUCCESS;
      AEE_SYS_RESUME(hMsg->pcb);
   }
}

/*=============================================================================

  Returns list of IDs matching message type

=============================================================================*/
int OEMSMS_GetEnumMsgIDs(AEESMSStorageType st, AEESMSTag mt, uint32 * pdwIndex, uint32 * pdwSize)
{
   int nErr = SUCCESS, i = 0, nCount = 0;
   FARF(SMS, ("OEMSMS_GetEnumMsgIDs"));

   if (!pdwSize || (st >= OEMSMS_STORAGE_LAST))
   {
      return EBADPARM;
   }
      
   // Count messages of specific tag type
   for(i = 0; i < gpWms->ucStrSMSCount[st]; i++)
   {
      if ((WMS_TAG_NONE != gpWms->strSMSMsgList[st][i]) &&
          ((AEESMS_TAG_NONE == mt) || (gpWms->strSMSMsgList[st][i] == Xlate_2_NativeTag(mt))))
      {
         nCount++;

         if (*pdwSize >= nCount*sizeof(uint32))
         {
            *pdwIndex++ = i;
         }
      }
   }

   *pdwSize = nCount*sizeof(uint32);
   
   return nErr;
}

/*===========================================================================

   Sets message ack status for specified message type

===========================================================================*/
int OEMSMS_SetMsgAckStatus(uint32 cs, uint32 mt)
{
   int  nErr = SUCCESS;
   uint32 oldCS = AEESMS_CS_OK;

   if (!gpWms)
      return EFAILED;

   switch (mt)
   {      
      case AEESMS_TYPE_EMS:
         oldCS = gpWms->dwEMSStatus;
         gpWms->dwEMSStatus = cs;
         break;

      case AEESMS_TYPE_WAP:
         oldCS = gpWms->dwWAPStatus;
         gpWms->dwWAPStatus = cs;
         break;

      case AEESMS_TYPE_TEXT:
         oldCS = gpWms->dwTextStatus;
         gpWms->dwTextStatus = cs;
         break;

      case AEESMS_TYPE_VOICE_MAIL_NOTIFICATION:
         oldCS = gpWms->dwVMStatus;
         gpWms->dwVMStatus = cs;
         break;

      case AEESMS_TYPE_PAGE:
         oldCS = gpWms->dwPageStatus;
         gpWms->dwPageStatus = cs;
         break;

      case AEESMS_TYPE_ANY:
         oldCS = gpWms->dwAnyStatus;
         gpWms->dwEMSStatus = gpWms->dwWAPStatus = gpWms->dwTextStatus = cs;
         gpWms->dwVMStatus = gpWms->dwPageStatus = gpWms->dwAnyStatus = cs;

         if ( AEESMS_CS_OUT_OF_RESOURCES == cs )
         {
            OEMWMS_SetMemoryFull( TRUE );
         }
         else if ( AEESMS_CS_OK == cs )
         {
            OEMWMS_SetMemoryFull( FALSE );
         }
         // else if its BUSY or BLOCK, there should be no change to memory status
         break;

      default:
         nErr = EBADPARM;
         break;
   }

   // Send model event about client status change
   if (gpWms && SUCCESS == nErr)
   {
      OEMSMSUpdateData * pUpdateData = NULL;
      uint32 * pData = NULL;
      if (gpWms->pcbOnSMSUpdate)
      {
         FARF(SMS, ("EVT_MDL_SMS_CLIENT_STATUS_CHANGE"));
         pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMS_CS_CHANGE;
         pData = (uint32*)pUpdateData->aEventData;

         // Send message type, old & new client status info
         *pData = mt;
         pData++;

         *pData = oldCS;
         pData++;

         *pData = cs;

         AEE_SYS_RESUME(gpWms->pcbOnSMSUpdate);
      }
   }
   
   return nErr;
}

/*===========================================================================

   Gets message ack status for specified teleservice ID

Returns:
   WMS_OK_S
      If OK, you can ignore eCDMAStatus and eGWStatus since they are not filled
   WMS_GENERAL_ERROR_S
      eCDMAStatus/eGWStatus (based on mode) are filled with status

===========================================================================*/
static wms_status_e_type OEMSMS_GetMsgAckStatus(uint32        ts, 
                                  wms_message_mode_e_type     mode, 
                                  wms_cdma_tl_status_e_type*  eCDMAStatus, 
                                  wms_tp_cause_e_type*        eGWStatus)

{
   wms_status_e_type eStatus = WMS_GENERAL_ERROR_S;

   if (!gpWms)
      return(eStatus);

   if (WMS_MESSAGE_MODE_CDMA == mode)
   {
      switch (ts)
      {
         case SMS_TELESERVICE_CMT_95:
         case SMS_TELESERVICE_IS91_SHORT_MESSAGE:
            if ( gpWms->dwTextStatus == AEESMS_CS_OK )
            {
              eStatus      = WMS_OK_S;
            }
            else
            {
              *eCDMAStatus = Xlate_CDMAClientStatus (gpWms->dwTextStatus);
            }
            break;
#ifdef CUST_EDITION	  
#if defined(FEATURE_QMA)
         case SMS_TELESERVICE_QMA_WPUSH:
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
         case SMS_TELESERVICE_WPUSH:
#endif            
#endif /*CUST_EDITION*/
         case SMS_TELESERVICE_WAP:
            if ( gpWms->dwWAPStatus == AEESMS_CS_OK )
            {
              eStatus      = WMS_OK_S;
            }
            else
            {
              *eCDMAStatus = Xlate_CDMAClientStatus (gpWms->dwWAPStatus);
            }
            break;

         case SMS_TELESERVICE_WEMT:
            if ( gpWms->dwEMSStatus == AEESMS_CS_OK )
            {
             eStatus      = WMS_OK_S;
            }
            else
            {
              *eCDMAStatus = Xlate_CDMAClientStatus (gpWms->dwEMSStatus);
            }
            break;

         case SMS_TELESERVICE_MWI:
         case SMS_TELESERVICE_VMN_95:
         case SMS_TELESERVICE_IS91_VOICE_MAIL:
            if ( gpWms->dwVMStatus == AEESMS_CS_OK )
            {
              eStatus      = WMS_OK_S;
            }
            else
            {
              *eCDMAStatus = Xlate_CDMAClientStatus (gpWms->dwVMStatus);
            }
            break;

         case SMS_TELESERVICE_CPT_95:
         case SMS_TELESERVICE_IS91_PAGE:
            if ( gpWms->dwPageStatus == AEESMS_CS_OK )
            {
              eStatus      = WMS_OK_S;
            }
            else
            {
              *eCDMAStatus = Xlate_CDMAClientStatus (gpWms->dwPageStatus);
            }
            break;

         default:
            eStatus = WMS_OK_S;
            break;
      }
   }
   else if (WMS_MESSAGE_MODE_GW == mode)
   {
      if ( gpWms->dwAnyStatus == AEESMS_CS_OK )
      {
          eStatus    = WMS_OK_S;
      }
      else
      {
          *eGWStatus = Xlate_GWClientStatus (gpWms->dwAnyStatus);
      }
   }
   return(eStatus);
}

/*===========================================================================

   Gets UDH data

===========================================================================*/
void OEMSMS_GetUDHData(SMSCacheEntry * pce, int * pnBytes, byte * pBuffer)
{   
   wms_client_ts_data_s_type * pctsdata = NULL;

   FARF(SMS, ("==> OEMSMS_GetUDHData"));   

   if (!pnBytes)
      return;

   pctsdata = (wms_client_ts_data_s_type *)pce->data;

   if (pctsdata->format == WMS_FORMAT_CDMA)
   {
      wms_client_bd_s_type * pcd = &pctsdata->u.cdma;

      if (pcd->message_id.udh_present &&
          (pcd->mask & WMS_MASK_BD_USER_DATA) &&
          pcd->user_data.num_headers)
      {
         int nUDHSize = OEMWMS_GetUDHSize(pcd->user_data.num_headers, pcd->user_data.headers);
         if (pBuffer && (*pnBytes >= nUDHSize))
         {
            OEMWMS_GetUDHData(pcd->user_data.num_headers, pcd->user_data.headers, (byte*)((void*)pBuffer));
         }
         *pnBytes = nUDHSize;
      }
      else
      {
         *pnBytes = 0;
      }
   }

   if (pctsdata->format == WMS_FORMAT_GW_PP)
   {
      switch (pctsdata->u.gw_pp.tpdu_type)
      {
         case WMS_TPDU_DELIVER:
         {
            wms_gw_deliver_s_type * pcd = &pctsdata->u.gw_pp.u.deliver;

            if (pcd->user_data_header_present &&
               pcd->user_data.num_headers)
            {
               int nUDHSize = OEMWMS_GetUDHSize(pcd->user_data.num_headers, pcd->user_data.headers);
               if (pBuffer && (*pnBytes >= nUDHSize))
               {
                  OEMWMS_GetUDHData(pcd->user_data.num_headers, pcd->user_data.headers,(byte*)((void*) pBuffer));
               }
               *pnBytes = nUDHSize;
            }
            else
            {
               *pnBytes = 0;
            }
         }
         break;

         case WMS_TPDU_SUBMIT:
         {
            wms_gw_submit_s_type * pcs = &pctsdata->u.gw_pp.u.submit;
            if (pcs->user_data_header_present &&
               pcs->user_data.num_headers)
            {
               int nUDHSize = OEMWMS_GetUDHSize(pcs->user_data.num_headers, pcs->user_data.headers);
               if (pBuffer && (*pnBytes >= nUDHSize))
               {
                  OEMWMS_GetUDHData(pcs->user_data.num_headers, pcs->user_data.headers, (byte*)((void*) pBuffer));
               }
               *pnBytes = nUDHSize;
            }
            else
            {
               *pnBytes = 0;
            }
         }
         break;

         case WMS_TPDU_STATUS_REPORT:
         {
            wms_gw_status_report_s_type * pcsr = &pctsdata->u.gw_pp.u.status_report;
            if (pcsr->user_data_header_present &&
                pcsr->user_data.num_headers)
            {
               int nUDHSize = OEMWMS_GetUDHSize(pcsr->user_data.num_headers, pcsr->user_data.headers);
               if (pBuffer && (*pnBytes >= nUDHSize))
               {
                  OEMWMS_GetUDHData(pcsr->user_data.num_headers, pcsr->user_data.headers, (byte*)((void*) pBuffer));
               }
               *pnBytes = nUDHSize;
            }
            else
            {
               *pnBytes = 0;
            }
         }
         break;

         default:
            FARF(SMS, ("==> OEMSMS_GetUDHData - Invalid TPDU"));   
            break;   
      }
   }
}

/*===========================================================================

   Gets GW CB Parameters

===========================================================================*/
void OEMSMS_GetGWCBParms(SMSCacheEntry * pce, OEMMsgData * pOEMMsgData)
{   
   wms_client_ts_data_s_type * pctsdata = NULL;

   FARF(SMS, ("==> OEMSMS_GetGWCBParms"));   

   if ((!pce) || (!pOEMMsgData))
      return;

   if (pce->bIsGWBroadcast == FALSE)
      return;

   pctsdata = (wms_client_ts_data_s_type *)pce->data;

   if (pctsdata->format == WMS_FORMAT_GW_CB)
   {
      wms_gw_cb_ts_data_s_type * pcd = &pctsdata->u.gw_cb;

      pOEMMsgData->dwSrvID            = pcd->cb_page_hdr.cb_srv_id;
      pOEMMsgData->dwGWCBPageNumber   = pcd->cb_page_hdr.page_number;
      pOEMMsgData->dwGWCBTotalPages   = pcd->cb_page_hdr.total_pages;
      pOEMMsgData->dwGWCBGeoScope     = pcd->cb_page_hdr.geo_scope;
      pOEMMsgData->dwGWCBMessageCode  = pcd->cb_page_hdr.message_code;
      pOEMMsgData->dwGWCBUpdateNumber = pcd->cb_page_hdr.update_number;
   }
}

/*===========================================================================

   Calculates size required for UDH data

===========================================================================*/
static int OEMWMS_GetUDHSize(int nNumHeaders, wms_udh_s_type udh[])
{
   int i = 0, nSize = 0;

   for(i = 0; i < nNumHeaders; i++)
   {
      switch (udh[i].header_id)
      {
         case WMS_UDH_CONCAT_8:
         case WMS_UDH_CONCAT_16:
         case WMS_UDH_PORT_8:
         case WMS_UDH_PORT_16:
            nSize += 7;          // length + data; (2 + n)
            break;

         case WMS_UDH_SPECIAL_SM:
            nSize += 6;          // length + data; (2 + n)
            break;

         case WMS_UDH_TEXT_FORMATING:
            if (udh[i].u.text_formating.is_color_present)
            {
               nSize += 7;          // length + data; (2 + n)
            }
            else
            {
               nSize += 6;          // length + data; (2 + n)
            }
            break;

         case WMS_UDH_PRE_DEF_SOUND:
         case WMS_UDH_PRE_DEF_ANIM:
            nSize += 5;          // length + data; (2 + n)
            break;
         
         case WMS_UDH_LARGE_ANIM:
         case WMS_UDH_LARGE_PICTURE:
            nSize += 133;        // length + data; (2 + n)
            break;
         
         case WMS_UDH_SMALL_ANIM:
         case WMS_UDH_SMALL_PICTURE:
            nSize += 37;         // length + data; (2 + n)
            break;

         case WMS_UDH_RFC822:
            nSize += 4;          // length + data; (2 + n)
            break;

         case WMS_UDH_VAR_PICTURE:
            nSize += (6 + ((udh[i].u.var_picture.width*udh[i].u.var_picture.height) + 7)/8);
            break;

         case WMS_UDH_USER_DEF_SOUND:
            nSize += (2 + 3 + udh[i].u.user_def_sound.data_length);
            break;

#ifndef FEATURE_SMS_IGNORE_EMS_5_CONTENT
         case WMS_UDH_EXTENDED_OBJECT:
            // First concat segment contains the extended object header info
            if ( udh[i].u.eo.first_segment )
            {
                // length + header size + actual content size
                nSize += (2 + 10 + udh[i].u.eo.content.length);
            }
            else
            {
                // length + header size + actual content size 
                nSize += (2 + 3 + udh[i].u.eo.content.length);
            }
            break;
          
         case WMS_UDH_USER_PROMPT:
            nSize += 6;          // length + data; (2 + n)
            break;
#endif // FEATURE_SMS_IGNORE_EMS_5_CONTENT

         default:
            nSize += (2 + 3 + udh[i].u.other.header_length);
            break;
      }
   }
   return nSize;
}

/*===========================================================================

   Gets UDH data in specified buffer

===========================================================================*/
static void OEMWMS_GetUDHData(int nNumHeaders, wms_udh_s_type udh[], byte * pBuffer)
{
   int i = 0;

   for(i = 0; i < nNumHeaders; i++)
   {
      switch (udh[i].header_id)
      {
         case WMS_UDH_CONCAT_8:
            MSGOPT_SETUDHCONCAT8BUFFER(pBuffer,udh[i].u.concat_8.seq_num, udh[i].u.concat_8.total_sm, udh[i].u.concat_8.msg_ref);
            pBuffer += 7;          // length + data; (2 + n)
            break;

         case WMS_UDH_CONCAT_16:
            MSGOPT_SETUDHCONCAT16BUFFER(pBuffer,udh[i].u.concat_16.seq_num, udh[i].u.concat_16.total_sm, udh[i].u.concat_16.msg_ref);
            pBuffer += 7;          // length + data; (2 + n)
            break;

         case WMS_UDH_PORT_8:
            MSGOPT_SETUDHPORT8BUFFER(pBuffer,udh[i].u.wap_8.dest_port, udh[i].u.wap_8.orig_port);
            pBuffer += 7;          // length + data; (2 + n)
            break;

         case WMS_UDH_PORT_16:
            MSGOPT_SETUDHPORT16BUFFER(pBuffer, udh[i].u.wap_16.dest_port, udh[i].u.wap_16.orig_port);
            pBuffer += 7;          // length + data; (2 + n)
            break;

         case WMS_UDH_SPECIAL_SM:
            MSGOPT_SETUDHSPECIALSMBUFFER(pBuffer, Xlate_MsgWaitingType(udh[i].u.special_sm.msg_waiting), Xlate_MsgWaitingKind(udh[i].u.special_sm.msg_waiting_kind), udh[i].u.special_sm.message_count);
            pBuffer += 6;          // length + data; (2 + n)
            break;
 
         case WMS_UDH_TEXT_FORMATING:
            if (udh[i].u.text_formating.is_color_present)
            {               
               MSGOPT_SETUDHTEXTWITHCOLORFORMATBUFFER(pBuffer, udh[i].u.text_formating.start_position, udh[i].u.text_formating.text_formatting_length, 
                                                      (byte)(udh[i].u.text_formating.alignment_type|(udh[i].u.text_formating.font_size << 2)|
                                                            (udh[i].u.text_formating.style_bold << 4)|(udh[i].u.text_formating.style_italic << 5)|
                                                            (udh[i].u.text_formating.style_underlined << 6)|(udh[i].u.text_formating.style_strikethrough << 7)),
                                                      (byte)(udh[i].u.text_formating.text_color_foreground|(udh[i].u.text_formating.text_color_background << 4)));
               pBuffer += 7;          // length + data; (2 + n)
            }
            else
            {
               MSGOPT_SETUDHTEXTFORMATBUFFER(pBuffer, udh[i].u.text_formating.start_position, udh[i].u.text_formating.text_formatting_length, 
                                             (byte)(udh[i].u.text_formating.alignment_type|(udh[i].u.text_formating.font_size << 2)|
                                                   (udh[i].u.text_formating.style_bold << 4)|(udh[i].u.text_formating.style_italic << 5)|
                                                   (udh[i].u.text_formating.style_underlined << 6)|(udh[i].u.text_formating.style_strikethrough << 7)));
               pBuffer += 6;          // length + data; (2 + n)
            }
            break;

         case WMS_UDH_PRE_DEF_SOUND:
            MSGOPT_SETUDHPREDEFSOUNDBUFFER(pBuffer, udh[i].u.pre_def_sound.position, udh[i].u.pre_def_sound.snd_number);
            pBuffer += 5;          // length + data; (2 + n)
            break;

         case WMS_UDH_PRE_DEF_ANIM:
            MSGOPT_SETUDHPREDEFANIMBUFFER(pBuffer, udh[i].u.pre_def_anim.position, udh[i].u.pre_def_anim.animation_number);
            pBuffer += 5;          // length + data; (2 + n)
            break;
         
         case WMS_UDH_LARGE_ANIM:
            MSGOPT_SETUDHLARGEANIMBUFFER(pBuffer, udh[i].u.large_anim.position, udh[i].u.large_anim.data);
            pBuffer += 133;        // length + data; (2 + n)
            break;

         case WMS_UDH_LARGE_PICTURE:
            MSGOPT_SETUDHLARGEPICBUFFER(pBuffer, udh[i].u.large_picture.position, udh[i].u.large_picture.data);
            pBuffer += 133;        // length + data; (2 + n)
            break;
         
         case WMS_UDH_SMALL_ANIM:
            MSGOPT_SETUDHSMALLANIMBUFFER(pBuffer, udh[i].u.small_anim.position, udh[i].u.small_anim.data);
            pBuffer += 37;         // length + data; (2 + n)
            break;

         case WMS_UDH_SMALL_PICTURE:
            MSGOPT_SETUDHSMALLPICBUFFER(pBuffer, udh[i].u.small_picture.position, udh[i].u.small_picture.data);
            pBuffer += 37;         // length + data; (2 + n)
            break;

         case WMS_UDH_RFC822:
            MSGOPT_SETRFC822BUFFER(pBuffer, udh[i].u.rfc822.header_length);
            pBuffer += 4;          // length + data; (2 + n)
            break;

         case WMS_UDH_VAR_PICTURE:
            MSGOPT_SETUDHVARPICBUFFER(pBuffer, udh[i].u.var_picture.position, udh[i].u.var_picture.width, udh[i].u.var_picture.height, udh[i].u.var_picture.data);
            pBuffer += (2 + 4 + ((udh[i].u.var_picture.width*udh[i].u.var_picture.height) + 7)/8);
            break;

         case WMS_UDH_USER_DEF_SOUND:
            MSGOPT_SETUDHUSERDEFSOUNDBUFFER(pBuffer, udh[i].u.user_def_sound.position, udh[i].u.user_def_sound.data_length, udh[i].u.user_def_sound.user_def_sound);
            pBuffer += (2 + 3 + udh[i].u.user_def_sound.data_length);
            break;

#ifndef FEATURE_SMS_IGNORE_EMS_5_CONTENT
         case WMS_UDH_EXTENDED_OBJECT:
            // First concat segment contains the extended object header info
            if ( udh[i].u.eo.first_segment )
            {
               pBuffer[0] = ((3 + 7 + udh[i].u.eo.content.length) & 0x00ff);    // first 2 bytes
               pBuffer[1] = ((3 + 7 + udh[i].u.eo.content.length) >> 8 );       // ..are length
               pBuffer[2] = WMS_UDH_EXTENDED_OBJECT;     // UDH ID
               pBuffer[3] = (udh[i].u.eo.content.length + 7) & 0x00ff;
               pBuffer[4] = (udh[i].u.eo.content.length + 7) >> 8;
               pBuffer[5] = udh[i].u.eo.reference;           // Reference number
               pBuffer[6] = udh[i].u.eo.length >> 8;         // EO Length
               pBuffer[7] = udh[i].u.eo.length & 0x00FF;     // ..
               pBuffer[8] = udh[i].u.eo.control;             // EO Control Data
               pBuffer[9] = udh[i].u.eo.type;                // e.g. vCard, vCal
               pBuffer[10] = udh[i].u.eo.position >> 8;      // EO Position
               pBuffer[11] = udh[i].u.eo.position & 0x00FF;  // ..
               MEMCPY(((unsigned char*)(pBuffer + 12)),(udh[i].u.eo.content.data),(udh[i].u.eo.content.length));
 
               pBuffer += (12 + udh[i].u.eo.content.length);
            }
            else
            {
                // This contains only the raw extended object data
                MSGOPT_SETUDHOTHERBUFFER ( pBuffer, 
                                           WMS_UDH_EXTENDED_OBJECT,
                                           udh[i].u.eo.content.length,
                                           udh[i].u.eo.content.data );
                pBuffer += (2 + 3 + udh[i].u.eo.content.length);
            }
            break;

         case WMS_UDH_USER_PROMPT:
            MSGOPT_SETUDHOTHERBUFFER(pBuffer, 
                                     WMS_UDH_USER_PROMPT, // UDH ID
                                     1,                   // Size of data
                                     &udh[i].u.user_prompt.number_of_objects); // Data
            pBuffer += 6;          // length + data; (2 + n)
            break;

#endif // FEATURE_SMS_IGNORE_EMS_5_CONTENT
            
         default:
            MSGOPT_SETUDHOTHERBUFFER(pBuffer, udh[i].u.other.header_id, (uint32)udh[i].u.other.header_length, udh[i].u.other.data);
            pBuffer += (2 + 3 + udh[i].u.other.header_length);
            break;
      }
   }
}

/*===========================================================================

   Translation of Message Waiting Types DMSS to AEE

===========================================================================*/
static AEESMSMsgWaitingType Xlate_MsgWaitingType(wms_gw_msg_waiting_e_type dt)
{
   MsgWaitTypeMap * pt;
   int i,nEntries = sizeof(gsMsgWaitTypeMap)/sizeof(MsgWaitTypeMap);

   for(i = 0, pt = (MsgWaitTypeMap *)gsMsgWaitTypeMap; i < nEntries; pt++,i++)
   {
      if (pt->DMSS_MsgWaitType == dt)
         return(pt->AEE_MsgWaitType);
   }

   return(AEESMS_MSGWAITINGTYPE_NONE); // Default   
}

/*===========================================================================

   Translation of Message Waiting Kinds DMSS to AEE

===========================================================================*/
static AEESMSMsgWaitingKind Xlate_MsgWaitingKind(wms_gw_msg_waiting_kind_e_type dk)
{
   MsgWaitKindMap * pk;
   int i,nEntries = sizeof(gsMsgWaitKindMap)/sizeof(MsgWaitKindMap);

   for(i = 0, pk = (MsgWaitKindMap *)gsMsgWaitKindMap; i < nEntries; pk++,i++)
   {
      if (pk->DMSS_MsgWaitKind == dk)
         return(pk->AEE_MsgWaitKind);
   }

   return(AEESMS_MSGWAITINGKIND_OTHER); // Default   
}

#ifndef FEATURE_SMS_IGNORE_EMS_5_CONTENT
/*===========================================================================

   Checks to see if message is first segment before encoding data
   Needed for Extended Object UDH encoding since its encoding header
   depends on this value.  Needs to be done in advance since order of
   UDH elements is not mandatory i.e. concat header which carriers this
   information might be after extended object element.

   It is first segment if it does not have the concat header or if it
   has the header with sequence number 1

===========================================================================*/
static boolean OEMWMS_UDHEncodeIsFirstSegment (int nMaxHeaders, wms_udh_s_type udh[], byte * pBytes, int nBytes)
{
   int nProcessed = 0, i = 0;
   boolean bFirstSegment = TRUE;
   
   while (nProcessed < nBytes)
   {
      if (i >= nMaxHeaders)
         break;

      udh[i].header_id = Xlate_2_NativeUDHId(*(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed))));

      if ( udh[i].header_id == WMS_UDH_CONCAT_16 )
      {
          udh[i].u.concat_16.seq_num = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
          
          if ( udh[i].u.concat_16.seq_num != 1 )
          {
              FARF(SMS, ("Not first Segment!"));
              bFirstSegment = FALSE;
          }
          break;
      }
      
      nProcessed += (2 + MSGOPT_GETUDHBUFFERLENGTH((byte*)(pBytes + nProcessed)));
      i++;
   }
 
   return bFirstSegment;
}
#endif // FEATURE_SMS_IGNORE_EMS_5_CONTENT

/*===========================================================================

   Sets UDH data from specified buffer and returns number of headers set

===========================================================================*/
static int OEMWMS_SetUDHData(int nMaxHeaders, wms_udh_s_type udh[], byte * pBytes, int nBytes)
{
   int nProcessed = 0, i = 0;

   while (nProcessed < nBytes)
   {
      if (i >= nMaxHeaders)
         break;

      udh[i].header_id = Xlate_2_NativeUDHId(*(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed))));

      switch (udh[i].header_id)
      {
         case WMS_UDH_CONCAT_8:
            udh[i].u.concat_8.seq_num = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            udh[i].u.concat_8.total_sm = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2);
            udh[i].u.concat_8.msg_ref = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3) + 
                                       (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 4) << 8);
            break;

         case WMS_UDH_CONCAT_16:
            udh[i].u.concat_16.seq_num = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            udh[i].u.concat_16.total_sm = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2);
            udh[i].u.concat_16.msg_ref = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3) + 
                                         (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 4) << 8));
            break;

         case WMS_UDH_PORT_8:
            udh[i].u.wap_8.dest_port = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1) + 
                                       (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2) << 8));
            udh[i].u.wap_8.orig_port = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3) + 
                                       (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 4) << 8));
            break;

         case WMS_UDH_PORT_16:
            udh[i].u.wap_16.dest_port = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1) + 
                                       (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2) << 8));
            udh[i].u.wap_16.orig_port = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3) + 
                                       (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 4) << 8));         // length + data; (2 + n)
            break;

         case WMS_UDH_SPECIAL_SM:
            udh[i].u.special_sm.msg_waiting = Xlate_2_NativeMsgWaitingType(*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1));
            udh[i].u.special_sm.msg_waiting_kind = Xlate_2_NativeMsgWaitingKind(*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2));
            udh[i].u.special_sm.message_count = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3);
            break;

         case WMS_UDH_TEXT_FORMATING:
            udh[i].u.text_formating.start_position = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            udh[i].u.text_formating.text_formatting_length = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2);
            udh[i].u.text_formating.alignment_type = (wms_udh_alignment_e_type)
                                                    ((*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3)) & 3);
            udh[i].u.text_formating.font_size = (wms_udh_font_size_e_type)
                                                   (((*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3)) & 12) >> 2);
            udh[i].u.text_formating.style_bold =   (((*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3)) & 16) >> 4);
            udh[i].u.text_formating.style_italic = (((*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3)) & 32) >> 5);
            udh[i].u.text_formating.style_underlined = (((*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3)) & 64) >> 6);
            udh[i].u.text_formating.style_strikethrough = (((*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3)) & 128) >> 7);
            if (5 == MSGOPT_GETUDHBUFFERLENGTH((byte*)(pBytes + nProcessed)))
            {
               // Text Color Information
               udh[i].u.text_formating.is_color_present = TRUE;
               udh[i].u.text_formating.text_color_foreground = (wms_udh_text_color_e_type)
                                                                  ((*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 4)) & 15);
               udh[i].u.text_formating.text_color_background = (wms_udh_text_color_e_type)
                                                                 (((*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 4)) & 240) >> 4);
            }
            break;

         case WMS_UDH_PRE_DEF_SOUND:
            udh[i].u.pre_def_sound.position = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            udh[i].u.pre_def_sound.snd_number = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2);
            break;

         case WMS_UDH_PRE_DEF_ANIM:
            udh[i].u.pre_def_anim.position = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            udh[i].u.pre_def_anim.animation_number = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2);
            break;
         
         case WMS_UDH_LARGE_ANIM:
            udh[i].u.large_anim.position = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            MEMCPY(udh[i].u.large_anim.data, (byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3), 128);
            break;

         case WMS_UDH_LARGE_PICTURE:
            udh[i].u.large_picture.position = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            MEMCPY(udh[i].u.large_picture.data, (byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3), 128);
            break;
         
         case WMS_UDH_SMALL_ANIM:
            udh[i].u.small_anim.position = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            MEMCPY(udh[i].u.small_anim.data, (byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3), 32);
            break;

         case WMS_UDH_SMALL_PICTURE:
            udh[i].u.small_picture.position = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            MEMCPY(udh[i].u.small_picture.data, (byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3), 32);
            break;

         case WMS_UDH_RFC822:
            udh[i].u.rfc822.header_length = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            break;

         case WMS_UDH_VAR_PICTURE:
            udh[i].u.var_picture.position = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            udh[i].u.var_picture.width = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2);
            udh[i].u.var_picture.height= *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3);
            MEMCPY(udh[i].u.var_picture.data, (byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 4), ((udh[i].u.var_picture.width*udh[i].u.var_picture.height) + 7)/8);
            break;

         case WMS_UDH_USER_DEF_SOUND:
            udh[i].u.user_def_sound.position = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1);
            udh[i].u.user_def_sound.data_length = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2);
            MEMCPY(udh[i].u.user_def_sound.user_def_sound, (byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3), udh[i].u.user_def_sound.data_length);
            break;

#ifndef FEATURE_SMS_IGNORE_EMS_5_CONTENT
         case WMS_UDH_EXTENDED_OBJECT:
         {
            // Is first segment if it has concat 16 udh missing or with seq number 1
            boolean bIsFirstSeg = OEMWMS_UDHEncodeIsFirstSegment (nMaxHeaders, udh, pBytes, nBytes);

            if ( bIsFirstSeg )
            {
               // Only the first segment contains the extended object header
               udh[i].u.eo.first_segment = TRUE;

               // Content Length
               udh[i].u.eo.content.length = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1) + 
                                           (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2) << 8));

               // Actual content length is obtained after stripping of headers.
               udh[i].u.eo.content.length -= 7;

               // Reference
               udh[i].u.eo.reference = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3));

               // Total Object Length
               udh[i].u.eo.length = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 5) + 
                     (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 4) << 8));

               // Control/Handling
               udh[i].u.eo.control = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 6)); 

               // Type
               udh[i].u.eo.type = (wms_udh_eo_id_e_type)(*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 7)); 

               // Position
               udh[i].u.eo.position = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 9) + 
                     (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 8) << 8));

               // Content data
               MEMCPY ( udh[i].u.eo.content.data, 
                      (byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 10), 
                      udh[i].u.eo.content.length );

            }
            else
            {
               // This contains only the raw extended object data
               udh[i].u.eo.first_segment = FALSE;

               udh[i].u.eo.content.length = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2);

               MEMCPY ( udh[i].u.eo.content.data, 
                       (byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3), 
                       udh[i].u.eo.content.length );
            }
         }
         break;

         case WMS_UDH_USER_PROMPT:
            udh[i].u.user_prompt.number_of_objects = *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3);
            break;
#endif // FEATURE_SMS_IGNORE_EMS_5_CONTENT
            
         default:
            udh[i].u.other.header_id = (wms_udh_id_e_type)
                                            *(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)));
            udh[i].u.other.header_length = (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 1) + 
                                           (*(byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 2) << 8));
            MEMCPY(udh[i].u.other.data, (byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed)) + 3), udh[i].u.other.header_length);
            break;
      }
      nProcessed += (2 + MSGOPT_GETUDHBUFFERLENGTH((byte*)(pBytes + nProcessed)));
      i++;
   }
   return i;
}

/*===========================================================================

  A Callback function for WMS functions called for ISMSStorage

===========================================================================*/
static void OEMWSMS_CommandCallback 
(
   wms_cmd_id_e_type cmd, 
   void * user_data, 
   wms_cmd_err_e_type cmd_err 
)
{
   int nErr = SUCCESS;
   OEMBaseMsg * hMsg = (OEMBaseMsg *)user_data;

   FARF(SMS, ("==>  OEMWSMS_CommandCallback"));

   nErr = Xlate_wms_cmd_err_2_AEEErrorCode(cmd_err);

   if (gpWms)
   {
      // For transfer routes, this is only indication of SUCCESS/FAILURE
      if (OEMWSMS_IsValidNode(hMsg, gpWms->pCurAutoStore))
      {
         *hMsg->pdwStatus = nErr;
         AEE_SYS_RESUME(hMsg->pcb);
         return;
      }

      // For other cases, we only handle FAILURE here and wait for message event
      if (nErr != SUCCESS)
      {
         if (OEMWSMS_IsValidNode(hMsg, gpWms->pCurMOSMS))
         {
            AEESMS_SETERROR(*hMsg->pdwStatus, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_GENERAL_PROBLEM);
            AEE_SYS_RESUME(hMsg->pcb);
            return;
         }

         if (OEMWSMS_IsValidNode(hMsg, gpWms->pCurDeleteSMS) ||
            OEMWSMS_IsValidNode(hMsg, gpWms->pCurReadSMS) ||
            OEMWSMS_IsValidNode(hMsg, gpWms->pCurStoreSMS))
         {
            *hMsg->pdwStatus = nErr;
            AEE_SYS_RESUME(hMsg->pcb);
            return;
         }
      }
   }
}

/*===========================================================================

  Translates AEE UDH ID to DMSS UDH ID

===========================================================================*/
static wms_udh_id_e_type Xlate_2_NativeUDHId(AEESMSUDHId id)
{
   int i;

   for(i = 0; i < sizeof(gsUDHIDMap)/sizeof(UDHIDMap); i++)
   {
      if (gsUDHIDMap[i].AEE_UDHID == id)
      {
         return gsUDHIDMap[i].DMSS_UDHID;
      }
   }

   // Assume DMSS uses standard UDH IDs so that we dont need a mapping table and 
   // we can return the same UDH ID
   return((wms_udh_id_e_type)id); 
}

/*===========================================================================

  Translates DMSS message wait type to AEE message wait type

===========================================================================*/
static wms_gw_msg_waiting_e_type Xlate_2_NativeMsgWaitingType(AEESMSMsgWaitingType at)
{
   int i;

   for(i = 0; i < sizeof(gsMsgWaitTypeMap)/sizeof(MsgWaitTypeMap); i++)
   {
      if (gsMsgWaitTypeMap[i].AEE_MsgWaitType == at)
      {
         return gsMsgWaitTypeMap[i].DMSS_MsgWaitType;
      }
   }
   return WMS_GW_MSG_WAITING_NONE;
}

/*===========================================================================

  Translates DMSS message wait kind to AEE message wait kind

===========================================================================*/
static wms_gw_msg_waiting_kind_e_type Xlate_2_NativeMsgWaitingKind(AEESMSMsgWaitingKind ak)
{
   int i;

   for(i = 0; i < sizeof(gsMsgWaitKindMap)/sizeof(MsgWaitKindMap); i++)
   {
      if (gsMsgWaitKindMap[i].AEE_MsgWaitKind == ak)
      {
         return gsMsgWaitKindMap[i].DMSS_MsgWaitKind;
      }
   }
   return WMS_GW_MSG_WAITING_OTHER;
}

/*===========================================================================
Function: Xlate_wms_cmd_err_2_AEEErrorCode
Description:
   Translates the WMS Command Error Codes to AEE Error Codes
Return Value:
   AEE Error Code
===========================================================================*/
static int Xlate_wms_cmd_err_2_AEEErrorCode(wms_cmd_err_e_type cmd_err)
{
   int nErr = EFAILED;

   FARF(SMS, ("==>  Xlate_wms_cmd_err_2_AEEErrorCode"));

   switch(cmd_err)
   {
      case WMS_CMD_ERR_NONE:
         nErr = SUCCESS;
         break;

      // Add more codes here to be more specific

      default:
         nErr = EFAILED;
         break;
   }
   return (nErr);
}

/*===========================================================================
Cancels SMS update listener callback
===========================================================================*/
static void OEMSMS_CancelSMSUpdateListener(AEECallback *pcb)
{
   FARF(SMS, ("==>  OEMSMS_CancelSMSUpdateListener"));

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != OEMSMS_CancelSMSUpdateListener)
      return;

   if (pcb == gpWms->pcbOnSMSUpdate) {
      gpWms->pcbOnSMSUpdate = 0;
   }

   pcb->pCancelData = 0;
   pcb->pfnCancel = 0;
}

/*===========================================================================
Called by ISMS to register callback to be invoked on a lower layer SMS event
===========================================================================*/
void OEMSMS_OnSMSUpdate(OEMSMSUpdateData * pEventData, AEECallback *pcb)
{
   FARF(SMS, ("==>  OEMSMS_OnSMSUpdate"));
         
   // Make sure that its not there in any other list
   CALLBACK_Cancel(pcb);

   pcb->pfnCancel = OEMSMS_CancelSMSUpdateListener;
   pcb->pReserved = pEventData;
   gpWms->pcbOnSMSUpdate = pcb;
}

/*===========================================================================
Called by ISMSStorage to register callback to be invoked on update in storage
status
===========================================================================*/
void OEMSMS_OnStorageUpdate(PFNNOTIFY pfnNotify)
{
   FARF(SMS, ("==>  OEMSMS_OnStorageUpdate"));
   gpWms->pfnNotifyOnStorageUpdate = pfnNotify;
}

/*===========================================================================
Cancels SMSBCConfig update listener callback
===========================================================================*/
static void OEMSMSBCConfig_CancelSMSBCConfigUpdateListener(AEECallback *pcb)
{
   FARF(SMS, ("==>  OEMSMSBCConfig_CancelSMSBCConfigUpdateListener"));

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != OEMSMSBCConfig_CancelSMSBCConfigUpdateListener)
      return;

   if (pcb == gpWms->pcbOnSMSBCConfigUpdate) {
      gpWms->pcbOnSMSBCConfigUpdate = 0;
   }

   pcb->pCancelData = 0;
   pcb->pfnCancel = 0;
}

/*===========================================================================
Called by ISMS to register callback to be invoked on a lower layer SMS BCConfig
event
===========================================================================*/
void OEMSMSBCConfig_OnSMSBCConfigUpdate(OEMSMSUpdateData * pEventData, AEECallback *pcb)
{
   FARF(SMS, ("==>  OEMSMSBCConfig_OnSMSBCConfigUpdate"));
         
   // Make sure that its not there in any other list
   CALLBACK_Cancel(pcb);

   pcb->pfnCancel = OEMSMSBCConfig_CancelSMSBCConfigUpdateListener;
   pcb->pReserved = pEventData;
   gpWms->pcbOnSMSBCConfigUpdate = pcb;
}
#ifdef FEATURE_BREW_BROADCAST_SMS
/*===========================================================================
Function: OEMSMSBCConfig_CleanOpInfo
Description:
   Cleans up information about current broadcast operation from gpWms
Return Value:
   None
===========================================================================*/
static void OEMSMSBCConfig_CleanOpInfo(void)
{
   if (gpWms->sBcCfg.bSrvInfoAlloc)
      FREEIF(gpWms->sBcCfg.pSrvInfo);

   MEMSET(&gpWms->sBcCfg, 0, sizeof(gpWms->sBcCfg));
}

/*===========================================================================

  Called to cancel pending BCConfig operation

===========================================================================*/
static void OEMSMSBCConfig_CancelOp(AEECallback *pcb)
{
   FARF(SMS, ("==>  OEMSMSBCConfig_CancelOp"));

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if (pcb->pfnCancel != OEMSMSBCConfig_CancelOp)
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
   int nErr = SUCCESS;

   FARF(SMS, ("==>  OEMSMSBCConfig_GetBCConfig"));

   if (gpWms->sBcCfg.bBroadcastInUse != TRUE)
   {
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = NULL;
      pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

      // Fill up the Current BC Configuration
      gpWms->sBcCfg.bBroadcastInUse = TRUE;                
      gpWms->sBcCfg.eCurRequest     = OEMSMSBCConfig_GetBCConfigRequest;
      gpWms->sBcCfg.dwMode         = mode;
      gpWms->sBcCfg.pcb            = pcb;
      gpWms->sBcCfg.pConfig        = pConfig;
      gpWms->sBcCfg.pnErr          = pnErr; 

      if (mode == AEESMS_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
#ifdef FEATURE_CDSMS_BROADCAST 
         wms_bc_mm_get_config((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, Xlate_AEESMSMode_2_wms_message_mode(mode));
#else // FEATURE_CDSMS_BROADCAST
         wms_bc_get_config((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL);
#endif // FEATURE_CDSMS_BROADCAST
#else // FEATURE_CDSMS
		 FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_CDSMS
      }
      else if (mode == AEESMS_MODE_GW)
      {
#ifdef FEATURE_GWSMS_BROADCAST
         wms_bc_mm_get_config((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, Xlate_AEESMSMode_2_wms_message_mode(mode));
#else // FEATURE_GWSMS_BROADCAST
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_GWSMS_BROADCAST
      }
      else
      {
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
      }
   }
   else
   {
      FARF(SMS, ("Request Rejected - Broadcast In Use"));
      nErr = EITEMBUSY;
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
   int nErr = SUCCESS;

   FARF(SMS, ("==>  OEMSMSBCConfig_GetBCPref"));

   if (gpWms->sBcCfg.bBroadcastInUse != TRUE)
   {
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = NULL;
      pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

      // Fill up the Current BC Configuration
      gpWms->sBcCfg.bBroadcastInUse = TRUE;                
      gpWms->sBcCfg.eCurRequest     = OEMSMSBCConfig_GetBCPrefRequest;
      gpWms->sBcCfg.dwMode         = mode;
      gpWms->sBcCfg.pcb            = pcb;
      gpWms->sBcCfg.pPref          = pPref;
      gpWms->sBcCfg.pnErr          = pnErr;  

      if (mode == AEESMS_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
#ifdef FEATURE_CDSMS_BROADCAST 
         wms_bc_mm_get_pref((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode);
#else // FEATURE_CDSMS_BROADCAST 
         wms_bc_get_pref((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL);
#endif // FEATURE_CDSMS_BROADCAST
#else // FEATURE_CDSMS
		 FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_CDSMS
	  }
      else if (mode == AEESMS_MODE_GW)
      {
#ifdef FEATURE_GWSMS_BROADCAST
         wms_bc_mm_get_pref((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode);
#else // FEATURE_GWSMS_BROADCAST
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_GWSMS_BROADCAST
      }
      else
      {
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
      }      
   }
   else
   {
      FARF(SMS, ("Request Rejected - Broadcast In Use"));
      nErr = EITEMBUSY;
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
   int nErr = SUCCESS;

   FARF(SMS, ("==>  OEMSMSBCConfig_SetBCPref"));

   if (gpWms->sBcCfg.bBroadcastInUse != TRUE)
   {
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = NULL;
      pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

      // Fill up the Current BC Configuration
      gpWms->sBcCfg.bBroadcastInUse = TRUE;                
      gpWms->sBcCfg.eCurRequest     = OEMSMSBCConfig_SetBCPrefRequest;
      gpWms->sBcCfg.dwMode         = mode;
      gpWms->sBcCfg.pcb            = pcb;
      gpWms->sBcCfg.pnErr          = pnErr;  

      if (mode == AEESMS_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
#ifdef FEATURE_CDSMS_BROADCAST 
         wms_bc_mm_set_pref((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode, nPref);
#else // FEATURE_CDSMS_BROADCAST
         wms_bc_set_pref((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, nPref);
#endif // FEATURE_CDSMS_BROADCAST 
#else // FEATURE_CDSMS
		 FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_CDSMS
      }
      else if (mode == AEESMS_MODE_GW)
      {
#ifdef FEATURE_GWSMS_BROADCAST
		  wms_bc_mm_set_pref((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode, nPref);
#else // FEATURE_GWSMS_BROADCAST
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_GWSMS_BROADCAST
	  }
      else
      {
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
      }
   }
   else
   {
      FARF(SMS, ("Request Rejected - Broadcast In Use"));
      nErr = EITEMBUSY;
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
   int nErr = SUCCESS;

   FARF(SMS, ("==>  OEMSMSBCConfig_GetNumberOfServiceIDs"));

   if (gpWms->sBcCfg.bBroadcastInUse != TRUE)
   {
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = NULL;
      pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

      // Fill up the Current BC Configuration
      gpWms->sBcCfg.bBroadcastInUse = TRUE;                
      gpWms->sBcCfg.eCurRequest     = OEMSMSBCConfig_GetNumberOfServiceIDsRequest;
      gpWms->sBcCfg.dwMode         = mode;
      gpWms->sBcCfg.pcb            = pcb;
      gpWms->sBcCfg.pnCount        = pnCount;
      gpWms->sBcCfg.pnErr          = pnErr;  

      if (mode == AEESMS_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
#ifdef FEATURE_CDSMS_BROADCAST 
         wms_bc_mm_get_all_service_ids((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode);
#else // FEATURE_CDSMS_BROADCAST
         wms_bc_get_all_service_ids((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL);
#endif // FEATURE_CDSMS_BROADCAST
#else // FEATURE_CDSMS
		 FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_CDSMS
      }
      else if (mode == AEESMS_MODE_GW)
      {
#ifdef FEATURE_GWSMS_BROADCAST
         wms_bc_mm_get_all_service_ids((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode);
#else // FEATURE_GWSMS_BROADCAST
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_GWSMS_BROADCAST
	  }
      else
      {
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
      }
   }
   else
   {
      FARF(SMS, ("Request Rejected - Broadcast In Use"));
      nErr = EITEMBUSY;
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
   int nErr = SUCCESS;
   
   FARF(SMS, ("==>  OEMSMSBCConfig_GetServiceOpts"));

   if (gpWms->sBcCfg.bBroadcastInUse != TRUE)
   {
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = NULL;
      pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

      // Fill up the Current BC Configuration
      gpWms->sBcCfg.bBroadcastInUse = TRUE;                
      gpWms->sBcCfg.eCurRequest     = OEMSMSBCConfig_GetServiceOptsRequest;
      gpWms->sBcCfg.dwMode         = mode;
      gpWms->sBcCfg.pcb            = pcb;
      gpWms->sBcCfg.psid           = pSrvID;
      gpWms->sBcCfg.pcb            = pcb;
      gpWms->sBcCfg.pSrvInfo       = pSrvInfo;
      gpWms->sBcCfg.pnErr          = pnErr;

      if (mode == AEESMS_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
#ifdef FEATURE_CDSMS_BROADCAST 
         wms_bc_mm_srv_id_type srv_id;
         MEMSET(&srv_id, 0, sizeof(wms_bc_mm_srv_id_type));
         if (ConvertAEESMSBCSrvIDToWMSBCMMSrvID(pSrvID, mode, &srv_id) == TRUE)
         {
            wms_bc_mm_get_service_info((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode, &srv_id);
         }
         else
         {
            nErr = EBADPARM;
         }
#else // FEATURE_CDSMS_BROADCAST 
         wms_bc_service_id_s_type srv_id;
         MEMSET(&srv_id, 0, sizeof(wms_bc_service_id_s_type));
         if (ConvertAEESMSBCSrvIDToWMSBCSrvID(pSrvID, mode, &srv_id) == TRUE)
         {
            wms_bc_get_service_info((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, &srv_id);
         }
         else
         {
            nErr = EBADPARM;
         }
#endif // FEATURE_CDSMS_BROADCAST
#else // FEATURE_CDSMS
		 FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_CDSMS
      }
      else if (mode == AEESMS_MODE_GW)
      {
#ifdef FEATURE_GWSMS_BROADCAST
         wms_bc_mm_srv_id_type srv_id;
         MEMSET(&srv_id, 0, sizeof(wms_bc_mm_srv_id_type));
         if (ConvertAEESMSBCSrvIDToWMSBCMMSrvID(pSrvID, mode, &srv_id) == TRUE)
         {
            wms_bc_mm_get_service_info((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode, &srv_id);
         }
         else
         {
            nErr = EBADPARM;
         }
#else // FEATURE_GWSMS_BROADCAST
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_GWSMS_BROADCAST
      }
      else
      {
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
      }
   }
   else
   {
      FARF(SMS, ("Request Rejected - Broadcast In Use"));
      nErr = EITEMBUSY;
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
   int nErr = SUCCESS;

   FARF(SMS, ("==>  OEMSMSBCConfig_SetServiceOpts"));

   if (gpWms->sBcCfg.bBroadcastInUse != TRUE)
   {
      if ((gpWms->sBcCfg.pSrvInfo = MALLOC(sizeof(OEMSMSBCSrvInfo))) == NULL)
      {
         nErr = ENOMEMORY;
         goto Done;
      }
      
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = NULL;
      pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

      gpWms->sBcCfg.bSrvInfoAlloc = TRUE;

      MEMCPY(gpWms->sBcCfg.pSrvInfo, pSrvInfo, sizeof(OEMSMSBCSrvInfo));
         
      // Fill up the Current BC Configuration
      gpWms->sBcCfg.bBroadcastInUse = TRUE;                
      gpWms->sBcCfg.eCurRequest     = OEMSMSBCConfig_SetServiceOptsRequest;
      gpWms->sBcCfg.dwMode         = mode;
      gpWms->sBcCfg.pcb            = pcb;
      gpWms->sBcCfg.pnErr          = pnErr;

      if (mode == AEESMS_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
#ifdef FEATURE_CDSMS_BROADCAST 
         wms_bc_mm_service_info_s_type *srv_info = NULL;
         srv_info = sys_malloc(sizeof(wms_bc_mm_service_info_s_type));
         if (srv_info == NULL) {
            FARF(SMS, ("srv_info failed on sys_malloc!"));
            return;
         }
         MEMSET(srv_info, 0, sizeof(wms_bc_mm_service_info_s_type));
         if (ConvertOEMSMSBCSrvInfoToWMSBCMMSrvInfo(pSrvInfo, mode, srv_info) == TRUE)
         {
            wms_bc_mm_change_service_info((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode, srv_info);
         }
         else
         {
            nErr = EBADPARM;
         }
         sys_free(srv_info);
#else // FEATURE_CDSMS_BROADCAST 
         wms_bc_service_info_s_type *srv_info = NULL;
         srv_info = sys_malloc(sizeof(wms_bc_service_info_s_type));
         if (srv_info == NULL) {
             FARF(SMS, ("srv_info failed on sys_malloc!"));
             return;
         }
         MEMSET(srv_info, 0, sizeof(wms_bc_service_info_s_type));
         if (ConvertOEMSMSBCSrvInfoToWMSBCSrvInfo(pSrvInfo, mode, srv_info) == TRUE)
         {
            wms_bc_select_service((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, &srv_info->srv_id, srv_info->selected, srv_info->priority);
         }
         else
         {
            nErr = EBADPARM;
         }
         sys_free(srv_info);
#endif // FEATURE_CDSMS_BROADCAST
#else // FEATURE_CDSMS
		 FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_CDSMS
      }
      else if (mode == AEESMS_MODE_GW)
      {
#ifdef FEATURE_GWSMS_BROADCAST
         wms_bc_mm_service_info_s_type *srv_info = NULL;
         srv_info = sys_malloc(sizeof(wms_bc_mm_service_info_s_type));
         if (srv_info == NULL) {
             FARF(SMS, ("srv_info failed on sys_malloc!"));
             return;
         }
         MEMSET(srv_info, 0, sizeof(wms_bc_mm_service_info_s_type));
         if (ConvertOEMSMSBCSrvInfoToWMSBCMMSrvInfo(pSrvInfo, mode, srv_info) == TRUE)
         {
            wms_bc_mm_change_service_info((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode, srv_info);
         }
         else
         {
            nErr = EBADPARM;
         }
         sys_free(srv_info);
#else // FEATURE_GWSMS_BROADCAST
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_GWSMS_BROADCAST
      }
      else
      {
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
      }
   }
   else
   {
      FARF(SMS, ("Request Rejected - Broadcast In Use"));
      nErr = EITEMBUSY;
   }
   
Done:
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
   int nErr = SUCCESS;
   
   FARF(SMS, ("==>  OEMSMSBCConfig_GetNumberOfServiceOpts"));

   if (gpWms->sBcCfg.bBroadcastInUse != TRUE)
   {
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = NULL;
      pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

      // Fill up the Current BC Configuration
      gpWms->sBcCfg.bBroadcastInUse = TRUE;                
      gpWms->sBcCfg.eCurRequest     = OEMSMSBCConfig_GetNumberOfServiceOptsRequest;
      gpWms->sBcCfg.dwMode         = mode;
      gpWms->sBcCfg.pcb            = pcb;
      gpWms->sBcCfg.pnCount        = pnCount;
      gpWms->sBcCfg.pnErr          = pnErr;               

      if (mode == AEESMS_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
#ifdef FEATURE_CDSMS_BROADCAST 
         wms_bc_mm_get_table((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode);
#else // FEATURE_CDSMS_BROADCAST 
         wms_bc_get_table((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL);
#endif // FEATURE_CDSMS_BROADCAST 
#else // FEATURE_CDSMS
		 FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_CDSMS
      }
      else if (mode == AEESMS_MODE_GW)
      {
#ifdef FEATURE_GWSMS_BROADCAST
         wms_bc_mm_get_table((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode);
#else // FEATURE_GWSMS_BROADCAST
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_GWSMS_BROADCAST
      }
      else
      {
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
      }
   }
   else
   {
      FARF(SMS, ("Request Rejected - Broadcast In Use"));
      nErr = EITEMBUSY;
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
   int nErr = SUCCESS;
   
   FARF(SMS, ("==>  OEMSMSBCConfig_DeleteService"));

   if (gpWms->sBcCfg.bBroadcastInUse != TRUE)
   {
      CALLBACK_Cancel(pcb);
      pcb->pCancelData = NULL;
      pcb->pfnCancel   = OEMSMSBCConfig_CancelOp;

      // Fill up the Current BC Configuration
      gpWms->sBcCfg.bBroadcastInUse = TRUE;                
      gpWms->sBcCfg.eCurRequest     = OEMSMSBCConfig_DeleteServiceRequest;
      gpWms->sBcCfg.dwMode         = mode;
      gpWms->sBcCfg.pcb            = pcb;
      gpWms->sBcCfg.pnErr          = pnErr;

      if (mode == AEESMS_MODE_CDMA)
      {
#ifdef FEATURE_CDSMS
#ifdef FEATURE_CDSMS_BROADCAST 
         wms_bc_mm_srv_id_type srv_id;
         MEMSET(&srv_id, 0, sizeof(wms_bc_mm_srv_id_type));
         if (ConvertAEESMSBCSrvIDToWMSBCMMSrvID(pSrvID, mode, &srv_id) == TRUE)
         {
            wms_bc_mm_delete_services((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode, 1, &srv_id);
         }
         else
         {
            nErr = EBADPARM;
         }
#else // FEATURE_CDSMS_BROADCAST
         wms_bc_service_id_s_type srv_id;
         MEMSET(&srv_id, 0, sizeof(wms_bc_service_id_s_type));
         if (ConvertAEESMSBCSrvIDToWMSBCSrvID(pSrvID, mode, &srv_id) == TRUE)
         {
            wms_bc_delete_service((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, &srv_id);
         }
         else
         {
            nErr = EBADPARM;
         }
#endif // FEATURE_CDSMS_BROADCAST
#else // FEATURE_CDSMS
		 FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_CDSMS
      }
      else if (mode == AEESMS_MODE_GW)
      {
#ifdef FEATURE_GWSMS_BROADCAST
         wms_bc_mm_srv_id_type srv_id;
         MEMSET(&srv_id, 0, sizeof(wms_bc_mm_srv_id_type));
         if (ConvertAEESMSBCSrvIDToWMSBCMMSrvID(pSrvID, mode, &srv_id) == TRUE)
         {
            wms_bc_mm_delete_services((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, (wms_message_mode_e_type)mode, 1, &srv_id);
         }
         else
         {
            nErr = EBADPARM;
         }
#else // FEATURE_GWSMS_BROADCAST
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
#endif // FEATURE_GWSMS_BROADCAST
      }
      else
      {
         FARF(SMS, ("Request Rejected - Unsupported SMS Mode=%d", mode));
         nErr = EUNSUPPORTED;
      }
   }
   else
   {
      FARF(SMS, ("Request Rejected - Broadcast In Use"));
      nErr = EITEMBUSY;
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
void OEMSMSBCConfig_FillServiceIDs(AEESMSBCSrvID *psid, uint32 dwBytes)
{
   FARF(SMS, ("==>  OEMSMSBCConfig_FillServiceIDs"));

   if ((psid != NULL) && 
       (gpWms->nCount) &&
       (gpWms->psid != NULL) &&
       (dwBytes >= gpWms->nCount*sizeof(AEESMSBCSrvID)))
   {
      MEMCPY(psid, gpWms->psid, gpWms->nCount*sizeof(AEESMSBCSrvID));
   }

   FREEIF(gpWms->psid);
   gpWms->psid = 0;
   gpWms->nCount = 0;
}

/*===========================================================================
Function: OEMSMSBCConfig_FillServiceInfo
Description:
   Fills the Service Info
Return Value:
   None
===========================================================================*/
void OEMSMSBCConfig_FillServiceInfo(OEMSMSBCSrvInfo *pSrvInfo, uint32 dwBytes)
{
   FARF(SMS, ("==>  OEMSMSBCConfig_FillServiceIDs"));

   if ((pSrvInfo != NULL) &&  
       (gpWms->nCount) &&
       (gpWms->pSrvInfo != NULL) &&
       (dwBytes >= gpWms->nCount*sizeof(OEMSMSBCSrvInfo)))
   {
      MEMCPY(pSrvInfo, gpWms->pSrvInfo, gpWms->nCount*sizeof(OEMSMSBCSrvInfo));
   }

   FREEIF(gpWms->pSrvInfo);
   gpWms->pSrvInfo = 0;
   gpWms->nCount = 0;
}

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
/*===========================================================================
Function: Xlate_AEESMSMode_2_wms_message_mode
Description:
   Translates the AEE SMS Mode to WMS Message Mode
Return Value:
   WMS Message Mode
===========================================================================*/
static wms_message_mode_e_type Xlate_AEESMSMode_2_wms_message_mode(AEESMSMode mode)
{
   wms_message_mode_e_type wmsMode;

   FARF(SMS, ("==>  Xlate_AEESMSMode_2_wms_message_mode"));

   switch(mode)
   {
      case AEESMS_MODE_CDMA:
         wmsMode = WMS_MESSAGE_MODE_CDMA;
         break;

      case AEESMS_MODE_GW:
         wmsMode = WMS_MESSAGE_MODE_GW;
         break;

      default:
         FARF(SMS, ("Invalid AEESMS Mode"));
         wmsMode = WMS_MESSAGE_MODE_MAX;
         break;
   }

   return wmsMode;
}

/*===========================================================================
Function: OEMWSMSBCMMConfig_EventCallback
Description:
   OEMWSMS Event Callback for Multimode Broadcast  
Return Value:
   None
===========================================================================*/
static void OEMWSMSBCMMConfig_EventCallback(wms_bc_mm_event_e_type event, wms_bc_mm_event_info_s_type *info_ptr)
{
   int i = 0;
   boolean bEventHandled = FALSE;

   FARF(SMS, ("==>  OEMWSMSBCMMConfig_EventCallback"));
   
   if (!gpWms)
      return;

   if (event == WMS_BC_MM_EVENT_CONFIG)
   {   
      if ((gpWms->sBcCfg.nConfig != info_ptr->u.bc_config) && 
          ((gpWms->sBcCfg.nConfig = info_ptr->u.bc_config) == info_ptr->u.bc_config) &&
          (gpWms->pcbOnSMSBCConfigUpdate))
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_CONFIG;
         pData = (uint32*)pUpdateData->aEventData;
         
         // Message Mode
         *pData = info_ptr->message_mode;
         pData++;

         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }

      if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetBCConfigRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         *gpWms->sBcCfg.pConfig = info_ptr->u.bc_config;
         bEventHandled = TRUE;
      }
   }
   else if (event == WMS_BC_MM_EVENT_PREF)
   {      
      if ((gpWms->sBcCfg.nPref != info_ptr->u.bc_pref) && 
          ((gpWms->sBcCfg.nPref = info_ptr->u.bc_pref) == info_ptr->u.bc_pref) &&
          (gpWms->pcbOnSMSBCConfigUpdate))
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_PREF;
         pData = (uint32*)pUpdateData->aEventData;
         
         // Message Mode
         *pData = info_ptr->message_mode;
         pData++;

         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }

      if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetBCPrefRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         *gpWms->sBcCfg.pPref = info_ptr->u.bc_pref;
         bEventHandled = TRUE;
      }
      else if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_SetBCPrefRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         bEventHandled = TRUE;
      }
   }
   else if ((event == WMS_BC_MM_EVENT_SRV_IDS) &&
            (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetNumberOfServiceIDsRequest))
   {
      if ((gpWms->psid = MALLOC(sizeof(AEESMSBCSrvID)*info_ptr->u.bc_mm_srv_ids.size)) != NULL)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         *gpWms->sBcCfg.pnCount = gpWms->nCount = info_ptr->u.bc_mm_srv_ids.size;

         for(i = 0; i < gpWms->nCount; i++)
         {
            ConvertWMSBCMMSrvIDToAEESMSBCSrvID(&info_ptr->u.bc_mm_srv_ids.entries[i], info_ptr->message_mode, &gpWms->psid[i]);
         }
      }
      else
      {
         *gpWms->sBcCfg.pnErr = ENOMEMORY;
      }
      bEventHandled = TRUE;
   }
   else if ((event == WMS_BC_MM_EVENT_SRV_INFO) &&
            (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetServiceOptsRequest))
   {
      *gpWms->sBcCfg.pnErr = SUCCESS;
      ConvertWMSBCMMSrvInfoToOEMSMSBCSrvInfo(&info_ptr->u.bc_mm_srv_info, info_ptr->message_mode, gpWms->sBcCfg.pSrvInfo);
      bEventHandled = TRUE;
   }
   else if (event == WMS_BC_MM_EVENT_ADD_SRVS)
   {
      if (gpWms->pcbOnSMSBCConfigUpdate)
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_ADDED;
         pData = (uint32*)pUpdateData->aEventData;
         
         // Message Mode
         *pData = info_ptr->message_mode;
         pData++;

         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }

      if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_SetServiceOptsRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         bEventHandled = TRUE;
      }
   }
   else if (event == WMS_BC_MM_EVENT_SRV_UPDATED)
   {      
      if ((gpWms->pcbOnSMSBCConfigUpdate) &&
          ((WMS_MESSAGE_MODE_CDMA == info_ptr->message_mode) || (WMS_MESSAGE_MODE_GW == info_ptr->message_mode)))
      {
         uint32 * pData = NULL;
         AEESMSBCSrvID *pSrvID = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_UPDATED;
         pData = (uint32*)pUpdateData->aEventData;
            
         // Message Mode
         *pData = info_ptr->message_mode;
         pData++;

         // Service ID
         pSrvID = (AEESMSBCSrvID *)pData;
         ConvertWMSBCMMSrvIDToAEESMSBCSrvID(&info_ptr->u.bc_mm_srv_info.srv_id, info_ptr->message_mode, pSrvID);
         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }

      if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_SetServiceOptsRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         bEventHandled = TRUE;
      }
   }
   else if ((event == WMS_BC_MM_EVENT_TABLE) &&
            (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetNumberOfServiceOptsRequest))
   {
      if ((gpWms->pSrvInfo = MALLOC(sizeof(OEMSMSBCSrvInfo)*info_ptr->u.bc_mm_table.size)) != NULL)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         *gpWms->sBcCfg.pnCount = gpWms->nCount = info_ptr->u.bc_mm_table.size;
         for(i = 0; i < gpWms->nCount; i++)
         {
            ConvertWMSBCMMSrvInfoToOEMSMSBCSrvInfo(&info_ptr->u.bc_mm_table.entries[i], info_ptr->message_mode, &gpWms->pSrvInfo[i]);
         }
      }
      else
      {
         *gpWms->sBcCfg.pnErr = ENOMEMORY;
      }
      bEventHandled = TRUE;
   }
   else if (event == WMS_BC_MM_EVENT_DELETE_SRVS)
   {
      if ((gpWms->pcbOnSMSBCConfigUpdate) &&
          ((WMS_MESSAGE_MODE_CDMA == info_ptr->message_mode) || (WMS_MESSAGE_MODE_GW == info_ptr->message_mode)))
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_DELETED;
         pData = (uint32*)pUpdateData->aEventData;
            
         // Message Mode
         *pData = info_ptr->message_mode;
         pData++;
      }

      if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_DeleteServiceRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         bEventHandled = TRUE;
      }
   }
   else if (event == WMS_BC_MM_EVENT_SELECT_ALL_SRVS)
   {
      if (gpWms->pcbOnSMSBCConfigUpdate)
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_SELECT_ALL_SRVS;
         pData = (uint32*)pUpdateData->aEventData;
         
         // Message Mode
         *pData = info_ptr->message_mode;
         pData++;

         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }
   }
   else if (event == WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS)
   {
      if (gpWms->pcbOnSMSBCConfigUpdate)
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_SET_PRIORITY_ALL_SRVS;
         pData = (uint32*)pUpdateData->aEventData;
         
         // Message Mode
         *pData = info_ptr->message_mode;
         pData++;

         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }
   }
   else if (event == WMS_BC_MM_EVENT_DELETE_ALL_SRVS)
   {
      if (gpWms->pcbOnSMSBCConfigUpdate)
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_DELETE_ALL_SRVS;
         pData = (uint32*)pUpdateData->aEventData;
         
         // Message Mode
         *pData = info_ptr->message_mode;
         pData++;

         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }
   }
   else
   {
      // Not Handled Event
      bEventHandled = FALSE;
   }
   
   if (bEventHandled)
   {
      // Call the Kernel Callback
      AEE_SYS_RESUME(gpWms->sBcCfg.pcb);
   }
}

/*===========================================================================
Function: ConvertAEESMSBCSrvIDToWMSBCMMSrvID
Description:
   Convert AEESMS Service ID to WMS BC MM Service ID
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertAEESMSBCSrvIDToWMSBCMMSrvID(AEESMSBCSrvID *pSrvID, wms_message_mode_e_type mode, wms_bc_mm_srv_id_type *service_id)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertAEESMSBCSrvIDToWMSBCMMSrvID"));

   if (pSrvID == NULL || service_id == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      if (mode == AEESMS_MODE_CDMA)
      {
         service_id->bc_srv_id.service  = (wms_service_e_type)pSrvID->srvType;
         service_id->bc_srv_id.language = Xlate_2_NativeLang(pSrvID->dwLang);
      }
      else if (mode == AEESMS_MODE_GW)
      {
         service_id->gw_cb_range.from = pSrvID->fromService;
         service_id->gw_cb_range.to   = pSrvID->toService;
      }
      else
      {
         bRetVal = FALSE;
      }
   }
   return bRetVal;
}

/*===========================================================================
Function: ConvertWMSBCSrvIDToAEESMSBCMMSrvID
Description:
   Convert WMS BC MM Service ID to AEESMS Service ID
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertWMSBCMMSrvIDToAEESMSBCSrvID(wms_bc_mm_srv_id_type *service_id, wms_message_mode_e_type msg_mode,AEESMSBCSrvID *pSrvID)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertWMSBCMMSrvIDToAEESMSBCSrvID"));
     
   if (pSrvID == NULL || service_id == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      if (msg_mode == WMS_MESSAGE_MODE_CDMA)
      {
         pSrvID->srvType = (AEESMSBCType)service_id->bc_srv_id.service;
         pSrvID->dwLang  = Xlate_Lang(service_id->bc_srv_id.language);
      }
      else if (msg_mode == WMS_MESSAGE_MODE_GW)
      {
         pSrvID->fromService = service_id->gw_cb_range.from;
         pSrvID->toService   = service_id->gw_cb_range.to;
      }
      else
      {
         bRetVal = FALSE;
      }
   }
   return bRetVal;
}

/*===========================================================================
Function: ConvertOEMSMSBCSrvInfoToWMSBCMMSrvInfo
Description:
   Convert OEMSMS Service Info to WMS BC MM Service Info
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertOEMSMSBCSrvInfoToWMSBCMMSrvInfo(OEMSMSBCSrvInfo *pSrvInfo, wms_message_mode_e_type mode, wms_bc_mm_service_info_s_type *srv_info)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertOEMSMSBCSrvInfoToWMSBCMMSrvInfo"));

   if (pSrvInfo == NULL || srv_info == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      // Service ID
      if (ConvertAEESMSBCSrvIDToWMSBCMMSrvID(&pSrvInfo->sSrvID, mode, &srv_info->srv_id) == FALSE)
      {
         bRetVal = FALSE;
      }

      // Selected
      srv_info->selected = pSrvInfo->bSelected;

      // Priority
      srv_info->priority = pSrvInfo->nPriority;

      // Label
      MEMCPY(srv_info->label, pSrvInfo->szLabel, WMS_BC_MM_SRV_LABEL_SIZE);

      // Label Encoding
      srv_info->label_encoding = Xlate_2_NativeEncoding(pSrvInfo->nLabelEncoding);

      // Alert
      srv_info->alert = pSrvInfo->nAlert;

      // Max Messages
      srv_info->max_messages = pSrvInfo->nMaxMessages;
   }
   return bRetVal;
}

/*===========================================================================
Function: ConvertWMSBCMMSrvInfoToOEMSMSBCSrvInfo
Description:
   Convert WMS BC MM Service Info to OEMSMS Service Info
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertWMSBCMMSrvInfoToOEMSMSBCSrvInfo(wms_bc_mm_service_info_s_type *srv_info, wms_message_mode_e_type msg_mode, OEMSMSBCSrvInfo *pSrvInfo)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertWMSBCMMSrvInfoToOEMSMSBCSrvInfo"));

   if (pSrvInfo == NULL || srv_info == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      // Service ID
      if (ConvertWMSBCMMSrvIDToAEESMSBCSrvID(&srv_info->srv_id, msg_mode, &pSrvInfo->sSrvID) == FALSE)
      {
         bRetVal = FALSE;
      }

      // Selected
      pSrvInfo->bSelected = srv_info->selected;

      // Priority
      pSrvInfo->nPriority = srv_info->priority;

      // Label
      MEMSET(pSrvInfo->szLabel, 0, sizeof(pSrvInfo->szLabel));
      MEMCPY(pSrvInfo->szLabel, srv_info->label, WMS_BC_MM_SRV_LABEL_SIZE);

      // Label Encoding
      pSrvInfo->nLabelEncoding = Xlate_Encoding(srv_info->label_encoding);
       
      // Alert
      pSrvInfo->nAlert          = srv_info->alert;

      // Max Messages
      pSrvInfo->nMaxMessages   = srv_info->max_messages;
   }

   return bRetVal;
}
#endif // FEATURE_BROADCAST_SMS_MULTIMODE

/*===========================================================================
Function: OEMWSMSBCConfig_CommandCallback
Description:
   OEMWSMS Command Callback 
Return Value:
   None
===========================================================================*/
static void OEMWSMSBCConfig_CommandCallback(wms_cmd_id_e_type cmd, void *user_data, wms_cmd_err_e_type cmd_err)
{
   int nErr;

   FARF(SMS, ("==>  OEMWSMSBCConfig_CommandCallback"));

   nErr = Xlate_wms_cmd_err_2_AEEErrorCode(cmd_err);

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
   // Special Handling for Add Service
   if ((cmd == WMS_CMD_BC_MM_CHANGE_SRV_INFO) &&
       (cmd_err == WMS_CMD_ERR_BC_BAD_SRV_ID))
   {
      if (gpWms->sBcCfg.dwMode == WMS_MESSAGE_MODE_CDMA)
      {
         wms_bc_mm_service_info_s_type *srv_info = NULL;
         srv_info = sys_malloc(sizeof(wms_bc_mm_service_info_s_type));
         if (srv_info == NULL) {
             FARF(SMS, ("srv_info failed on sys_malloc!"));
             return;
         }
         MEMSET(srv_info, 0, sizeof(wms_bc_mm_service_info_s_type));

         // Service not Found ; Hence Add it
         if (ConvertOEMSMSBCSrvInfoToWMSBCMMSrvInfo(gpWms->sBcCfg.pSrvInfo, gpWms->sBcCfg.dwMode, srv_info) == TRUE)
         {
            wms_bc_mm_add_services((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, user_data, gpWms->sBcCfg.dwMode, 1, srv_info);
            nErr = SUCCESS;
         }
         else
         {
            nErr = EBADPARM;
         }
         sys_free(srv_info);
      }
      else // GW Mode
      {
         wms_bc_mm_service_info_s_type *srv_info = NULL;
         srv_info = sys_malloc(sizeof(wms_bc_mm_service_info_s_type));
         if (srv_info == NULL) {
             FARF(SMS, ("srv_info failed on sys_malloc!"));
             return;
         }
         MEMSET(srv_info, 0, sizeof(wms_bc_mm_service_info_s_type));
         // Service not Found ; Hence Add it
         if (ConvertOEMSMSBCSrvInfoToWMSBCMMSrvInfo(gpWms->sBcCfg.pSrvInfo, gpWms->sBcCfg.dwMode, srv_info) == TRUE)
         {
            wms_bc_mm_add_services((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, user_data, gpWms->sBcCfg.dwMode, 1, srv_info);
            nErr = SUCCESS;
         }
         else
         {
            nErr = EBADPARM;
         }
         sys_free(srv_info);
      }
   }
#endif // FEATURE_BROADCAST_SMS_MULTIMODE
#if (defined(FEATURE_CDSMS) && !defined(FEATURE_CDSMS_BROADCAST))
   if (cmd == WMS_CMD_BC_SELECT_SRV)
   {
      wms_bc_service_info_s_type *srv_info = NULL;
      srv_info = sys_malloc(sizeof(wms_bc_service_info_s_type));
      if (srv_info == NULL) {
          FARF(SMS, ("srv_info failed on sys_malloc!"));
          return;
      }
      MEMSET(srv_info, 0, sizeof(wms_bc_service_info_s_type));

      if (cmd_err == WMS_CMD_ERR_BC_BAD_SRV_ID)
      {
         // Service not Found ; Hence Add it
         if (ConvertOEMSMSBCSrvInfoToWMSBCSrvInfo(gpWms->sBcCfg.pSrvInfo, gpWms->sBcCfg.dwMode, srv_info) == TRUE)
         {
            wms_bc_add_service((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, user_data, srv_info);
            nErr = SUCCESS;
         }
         else
         {
            nErr = EBADPARM;
         }
      }
      else if (cmd_err == WMS_CMD_ERR_NONE) 
      {         
         if (ConvertOEMSMSBCSrvInfoToWMSBCSrvInfo(gpWms->sBcCfg.pSrvInfo, gpWms->sBcCfg.dwMode, srv_info) == TRUE)
         {
            wms_bc_change_label((wms_client_id_type)WMS_CLIENT_TYPE_BREW, OEMWSMSBCConfig_CommandCallback, NULL, &srv_info->srv_id, srv_info->label);
            nErr = SUCCESS;
         }
         else
         {
            nErr = EBADPARM;
         }
      }
      sys_free(srv_info);
   }
#endif // (defined(FEATURE_CDSMS) && !defined(FEATURE_CDSMS_BROADCAST))

   if (nErr != SUCCESS)
   {
      // Fill up the Data
      if (gpWms->sBcCfg.pnErr)
         *gpWms->sBcCfg.pnErr = nErr;

      // Call the Kernel Callback
      if (gpWms->sBcCfg.pcb)
         AEE_SYS_RESUME(gpWms->sBcCfg.pcb);
   }
}

#if (defined(FEATURE_CDSMS) && !defined(FEATURE_CDSMS_BROADCAST))
/*===========================================================================
Function: OEMWSMSBCConfig_EventCallback
Description:
   OEMWSMS Event Callback for 1x Broadcast  
Return Value:
   None
===========================================================================*/
static void OEMWSMSBCConfig_EventCallback(wms_bc_event_e_type event, wms_bc_event_info_s_type *info_ptr)
{
   boolean bEventHandled = FALSE;

   FARF(SMS, ("==>  OEMWSMSBCConfig_EventCallback"));

   if (!gpWms)
      return;

   if (event == WMS_BC_EVENT_CONFIG)
   {   
      if ((gpWms->sBcCfg.nConfig != info_ptr->bc_config) && 
          ((gpWms->sBcCfg.nConfig = info_ptr->bc_config) == info_ptr->bc_config) &&
          (gpWms->pcbOnSMSBCConfigUpdate))
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_CONFIG;
         pData = (uint32*)pUpdateData->aEventData;
            
         // Message Mode
         *pData = WMS_MESSAGE_MODE_CDMA;
         pData++;

         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }

      if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetBCConfigRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         *gpWms->sBcCfg.pConfig = info_ptr->bc_config;
         bEventHandled = TRUE;
      }
   }
   else if (event == WMS_BC_EVENT_PREF)
   {      
      if ((gpWms->sBcCfg.nPref != info_ptr->bc_pref) && 
          ((gpWms->sBcCfg.nPref = info_ptr->bc_pref) == info_ptr->bc_pref) &&
          (gpWms->pcbOnSMSBCConfigUpdate))
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_PREF;
         pData = (uint32*)pUpdateData->aEventData;
            
         // Message Mode
         *pData = WMS_MESSAGE_MODE_CDMA;
         pData++;

         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }

      if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetBCPrefRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         *gpWms->sBcCfg.pPref = info_ptr->bc_pref;
         bEventHandled = TRUE;
      }
      else if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_SetBCPrefRequest)

      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         bEventHandled = TRUE;
      }
   }
   else if ((event == WMS_BC_EVENT_SRV_IDS) &&
            (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetNumberOfServiceIDsRequest))
   {
      if ((gpWms->psid = MALLOC(sizeof(AEESMSBCSrvID)*info_ptr->bc_srv_ids.size)) != NULL)
      {
         int i = 0;
         *gpWms->sBcCfg.pnErr = SUCCESS;
         *gpWms->sBcCfg.pnCount = gpWms->nCount = info_ptr->bc_srv_ids.size;
         for(i = 0; i < gpWms->nCount; i++)
         {
            ConvertWMSBCSrvIDToAEESMSBCSrvID(&info_ptr->bc_srv_ids.entries[i], gpWms->sBcCfg.dwMode, &gpWms->psid[i]);
         }
      }
      else
      {
         *gpWms->sBcCfg.pnErr = ENOMEMORY;
      }
      bEventHandled = TRUE;
   }
   else if ((event == WMS_BC_EVENT_SRV_INFO) &&
            (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetServiceOptsRequest))
   {
      *gpWms->sBcCfg.pnErr = SUCCESS;
      (void)ConvertWMSBCSrvInfoToOEMSMSBCSrvInfo(&info_ptr->bc_srv_info, gpWms->sBcCfg.dwMode, gpWms->sBcCfg.pSrvInfo);
      bEventHandled = TRUE;
   }
   else if (event == WMS_BC_EVENT_SRV_ADDED)
   {      
      if (gpWms->pcbOnSMSBCConfigUpdate)
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_ADDED;
         pData = (uint32*)pUpdateData->aEventData;
            
         // Message Mode
         *pData = WMS_MESSAGE_MODE_CDMA;
         pData++;
      }

      if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_SetServiceOptsRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         bEventHandled = TRUE;
      }
   }
   else if (event == WMS_BC_EVENT_SRV_UPDATED)
   {      
      if (gpWms->pcbOnSMSBCConfigUpdate)
      {
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
         (void)ConvertWMSBCSrvIDToAEESMSBCSrvID(&info_ptr->bc_srv_info.srv_id, WMS_MESSAGE_MODE_CDMA, pSrvID);
         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }

      if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_SetServiceOptsRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         bEventHandled = TRUE;
      }
   }
   else if ((event == WMS_BC_EVENT_TABLE) &&
            (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_GetNumberOfServiceOptsRequest))
   {
      if ((gpWms->pSrvInfo = MALLOC(sizeof(OEMSMSBCSrvInfo)*info_ptr->bc_table.size)) != NULL)
      {
         uint32 i = 0;
         *gpWms->sBcCfg.pnErr = SUCCESS;
         *gpWms->sBcCfg.pnCount = gpWms->nCount = info_ptr->bc_table.size;
         for(i = 0; i < gpWms->nCount; i++)
         {
            (void)ConvertWMSBCSrvInfoToOEMSMSBCSrvInfo(&info_ptr->bc_table.entries[i], gpWms->sBcCfg.dwMode, &gpWms->pSrvInfo[i]);
         }
      }
      else
      {
         *gpWms->sBcCfg.pnErr = ENOMEMORY;
      }
      bEventHandled = TRUE;
   }
   else if (event == WMS_BC_EVENT_SRV_DELETED)
   {
      if (gpWms->pcbOnSMSBCConfigUpdate)
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_DELETED;
         pData = (uint32*)pUpdateData->aEventData;
            
         // Message Mode
         *pData = (uint32)WMS_MESSAGE_MODE_CDMA;
         pData++;
      }

      if (gpWms->sBcCfg.eCurRequest == OEMSMSBCConfig_DeleteServiceRequest)
      {
         *gpWms->sBcCfg.pnErr = SUCCESS;
         bEventHandled = TRUE;
      }
   }
   else if (event == WMS_BC_EVENT_ALL_SRV_DELETED)
   {
      if (gpWms->pcbOnSMSBCConfigUpdate)
      {
         uint32 * pData = NULL;
         OEMSMSUpdateData * pUpdateData = (OEMSMSUpdateData *)gpWms->pcbOnSMSBCConfigUpdate->pReserved;
         pUpdateData->nEvent = EVT_MDL_SMSBCCONFIG_DELETE_ALL_SRVS;
         pData = (uint32*)pUpdateData->aEventData;
            
         // Message Mode
         *pData = (uint32)WMS_MESSAGE_MODE_CDMA;
         pData++;

         AEE_SYS_RESUME(gpWms->pcbOnSMSBCConfigUpdate);
      }
   }
   else
   {
      // Not Handled Event
      bEventHandled = FALSE;
   }
   
   if (bEventHandled)
   {
      // Call the Kernel Callback
      AEE_SYS_RESUME(gpWms->sBcCfg.pcb);
   }
}

/*===========================================================================
Function: ConvertAEESMSBCSrvIDToWMSBCSrvID
Description:
   Convert AEESMS Service ID to WMS BC Service ID
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertAEESMSBCSrvIDToWMSBCSrvID(AEESMSBCSrvID *pSrvID, wms_message_mode_e_type mode, wms_bc_service_id_s_type *service_id)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertAEESMSBCSrvIDToWMSBCSrvID"));

   if (pSrvID == NULL || service_id == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      if (mode == AEESMS_MODE_CDMA)
      {
         service_id->service  = (wms_service_e_type)pSrvID->srvType;
         service_id->language = Xlate_2_NativeLang(pSrvID->dwLang);
      }
      else
      {
         bRetVal = FALSE;
      }
   }
   return bRetVal;
}

/*===========================================================================
Function: ConvertWMSBCSrvIDToAEESMSBCSrvID
Description:
   Convert WMS BC Service ID to AEESMS Service ID
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertWMSBCSrvIDToAEESMSBCSrvID(wms_bc_service_id_s_type *service_id, wms_message_mode_e_type msg_mode, AEESMSBCSrvID *pSrvID)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertWMSBCSrvIDToAEESMSBCSrvID"));
     
   if (pSrvID == NULL || service_id == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      if (msg_mode == WMS_MESSAGE_MODE_CDMA)
      {
         pSrvID->srvType = (AEESMSBCType)service_id->service;
         pSrvID->dwLang  = Xlate_Lang(service_id->language);
      }
      else
      {
         bRetVal = FALSE;
      }
   }
   return bRetVal;
}

/*===========================================================================
Function: ConvertOEMSMSBCSrvInfoToWMSBCSrvInfo
Description:
   Convert OEMSMS Service Info to WMS BC Service Info
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertOEMSMSBCSrvInfoToWMSBCSrvInfo(OEMSMSBCSrvInfo *pSrvInfo, wms_message_mode_e_type mode, wms_bc_service_info_s_type *srv_info)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertOEMSMSBCSrvInfoToWMSBCSrvInfo"));

   if (pSrvInfo == NULL || srv_info == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      // Service ID
      if (ConvertAEESMSBCSrvIDToWMSBCSrvID(&pSrvInfo->sSrvID, mode, &srv_info->srv_id) == FALSE)
      {
         bRetVal = FALSE;
      }

      // Selected
      srv_info->selected = pSrvInfo->bSelected;

      // Priority
      srv_info->priority = (wms_priority_e_type)pSrvInfo->nPriority;

      // Label
      MEMCPY(srv_info->label, pSrvInfo->szLabel, WMS_BC_SRV_LABEL_SIZE);

      // Label Encoding
      srv_info->label_encoding = Xlate_2_NativeEncoding(pSrvInfo->nLabelEncoding);

      // Alert
      srv_info->alert = (wms_bc_alert_e_type)pSrvInfo->nAlert;

      // Max Messages
      srv_info->max_messages = pSrvInfo->nMaxMessages;
   }
   return bRetVal;
}

/*===========================================================================
Function: ConvertWMSBCSrvInfoToOEMSMSBCSrvInfo
Description:
   Convert WMS BC Service Info to OEMSMS Service Info
Return Value:
  Indicates successful/ unsuccessful conversion
===========================================================================*/
static boolean ConvertWMSBCSrvInfoToOEMSMSBCSrvInfo(wms_bc_service_info_s_type *srv_info, wms_message_mode_e_type msg_mode, OEMSMSBCSrvInfo *pSrvInfo)
{
   boolean bRetVal = TRUE;

   FARF(SMS, ("==>  ConvertWMSBCSrvInfoToOEMSMSBCSrvInfo"));

   if (pSrvInfo == NULL || srv_info == NULL)
   {
      bRetVal = FALSE;
   }
   else
   {
      // Service ID
      if (ConvertWMSBCSrvIDToAEESMSBCSrvID(&srv_info->srv_id, msg_mode, &pSrvInfo->sSrvID) == FALSE)
      {
         bRetVal = FALSE;
      }

      // Selected
      pSrvInfo->bSelected = srv_info->selected;

      // Priority
      pSrvInfo->nPriority = srv_info->priority;

      // Label
      MEMSET(pSrvInfo->szLabel, 0, sizeof(pSrvInfo->szLabel));
      MEMCPY(pSrvInfo->szLabel, srv_info->label, WMS_BC_SRV_LABEL_SIZE);

      // Label Encoding
      pSrvInfo->nLabelEncoding = Xlate_Encoding(srv_info->label_encoding);
       
      // Alert
      pSrvInfo->nAlert          = srv_info->alert;

      // Max Messages
      pSrvInfo->nMaxMessages   = srv_info->max_messages;
   }

   return bRetVal;
}
#endif // (defined(FEATURE_CDSMS) && !defined(FEATURE_CDSMS_BROADCAST))

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
#endif /*FEATURE_WMS*/

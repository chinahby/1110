/*======================================================
FILE:  AEESMS.c

SERVICES:  AEESMS member functions

GENERAL DESCRIPTION:

   Implements ISMSMsg, ISMS, ISMSNotifier, ISMSStorage, ISMSStorage2, 
   ISMSBCSrvOpts, ISMSBCConfig interfaces and related functionality

        Copyright © 1999-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
//lint -emacro({661}, MSGOPT_GETUDHBUFFERDATA)
//lint -emacro({662}, MSGOPT_GETUDHBUFFERDATA)
//lint -emacro({661}, MSGOPT_GETUDHBUFFERLENGTH)
#include "AEEWebOptMgr.h"
#include "AEE_OEM.h"
#include "AEESMS.h"
#include "OEMSMSStorage.h"
#include "AEE_OEMSMS.h"
#include "AEEModTable.h"
#include "AEETAPI.h"
#include "AEEBase.h"
#include "AEEModel.h"
#include "OEMConfig.h" 
#include "OEMHeap.h"
#include "AEE_OEMDispatch.h"

#define MAX_MSG_LENGTH           256
#define MAX_ALPHA_ID_LENGTH      256

#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }

#define ONE_MINUTE_SECS          (60)
#define ONE_HOUR_SECS            (60*ONE_MINUTE_SECS)
#define ONE_DAY_SECS             (24*ONE_HOUR_SECS)
#define ONE_MONTH_SECS           (30*ONE_DAY_SECS)

#define DEFAULT_EVENT_CACHE_ENTRIES 16

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/

struct ISMSMsg {
   const AEEVTBL(ISMSMsg)        *pvt;
   uint32                        uRef;
   IShell                        *pIShell;                 
   WebOptMgr                     *pWom;          // Our collection of WebOpts
};

struct IModel
{
   AEEBASE_INHERIT(IModel, IBase);

   ModelListener              *m_pListeners;       // List of listeners...
};

struct ISMS {
   const AEEVTBL(ISMS)           *pvt;
   uint32                        uRef;
   IShell                        *pIShell; 
   OEMSMSMgr                     *pOEMSms;

   // Members to support model events
   ACONTEXT *                    pac;
   IModel                        model;
   OEMSMSUpdateData              sUpdateData;      // Place holder for model event data
   AEECallback                   cbOnSMSUpdate;
   AEECallback                   cbSysObj;
};

AEEINTERFACE(ISMSNotifier)
{
   INHERIT_INotifier(ISMSNotifier);
};

struct ISMSNotifier{
   const AEEVTBL(ISMSNotifier)   *pvt;
   uint32                        uRef;
   IShell                        *pIShell;
   uint32                        dwNotify;
   OEMSMSMgr                     *pOEMSms;
   AEECallback                    cbOnMtSMS;
};

struct ISMSStorage2 {
   const AEEVTBL(ISMSStorage2)   *pvt;
   uint32                        uRef;
   IShell                        *pIShell;

   // Members to support enumeration
   AEESMSTag                     mt;               // Enum message tag
   AEESMSStorageType             st;               // Enum storage type
   uint32                        dwCount;          // Messages IDs in pdwIndex
   uint32                        *pdwIndex;        // Array of message IDs                      
   uint32                        dwOffset;         // Offset in Array of message IDs
   boolean                       bEnumInit;        // Enum Initialized

   // Members to support model events
   ACONTEXT *                    pac;
   IModel                        model;
   OEMStorageUpdateData          sUpdateData;      // Place holder for model event data
   AEECallback                   cbOnStorageUpdate;
   AEECallback                   cbSysObj;
};

struct ISMSBCSrvOpts {
   const AEEVTBL(ISMSBCSrvOpts)  *pvt;
   uint32                        uRef;
   IShell                        *pIShell;                 
   WebOptMgr                     *pWom;          // Our collection of WebOpts 
};

struct ISMSBCConfig {
   const AEEVTBL(ISMSBCConfig)   *pvt;
   uint32                        uRef;
   IShell                        *pIShell;

   // Members to support model events
   ACONTEXT *                    pac;
   IModel                        model;
   OEMSMSUpdateData              sUpdateData;      // Place holder for model event data
   AEECallback                   cbOnSMSBCConfigUpdate;
   AEECallback                   cbSysObj;
}; 

typedef struct {
   AEECallback    cb;
   AEECallback *  pcb;
   ACONTEXT *     pac;
} SMSAppCbData;

typedef struct {
   AEECallback    cb;
   AEECallback *  pcb;
   ACONTEXT *     pac;
   ISMS *         pme;
   uint32 *       pnRv;
   OEMMsgData     *pMsgData;
   OEMMsgPayload  *pMsgPayload;
} SMSAppSendMsgCbData;

typedef struct {
   AEECallback       cb;
   AEECallback *     pcb;
   ACONTEXT *        pac;
   uint32 *          pnRv;
   uint32            nIndex;  // Used for UpdateMsg
   uint32 *          pnIndex; // Used for StoreMsg
   AEESMSStorageType st;
   AEESMSTag         mt;
} SMSAppStoreMsgCbData;

typedef struct {
   AEECallback    cb;
   AEECallback *  pcb;
   ACONTEXT *     pac;
   uint32 *       pnRv;
   ISMSMsg **     ppISMSMsg;
   OEMMsgData     *pMsgData;
   OEMMsgPayload  *pMsgPayload;
} SMSAppReadMsgCbData;

typedef struct {
   AEECallback       cb;
   AEECallback *     pcb;
   ACONTEXT *        pac;
   uint32 *          pnRv;
   uint32            nIndex;
   AEESMSStorageType st;
   AEESMSTag         mt;
} SMSAppDeleteMsgCbData;

typedef struct {
   AEECallback    cb;
   AEECallback *  pcb;
   ACONTEXT *     pac;
   uint32 *       pnRv;
   ISMSMsg **     ppISMSMsg;
   uint32 *       pnIndex;
   OEMMsgData     *pMsgData;
   OEMMsgPayload  *pMsgPayload;
   ISMSStorage2 *  pISMSStorage;
} SMSAppEnumMsgCbData;

typedef struct {
   AEECallback     cb;
   AEECallback *   pcb;
   ACONTEXT *      pac;
   uint32 *        pnCount;
   AEESMSBCSrvID **ppsid;
   uint32 *        pnErr;
} SMSAppGetSrvIDsCbData;

typedef struct {
   AEECallback     cb;
   AEECallback *   pcb;
   ACONTEXT *      pac;
   ISMSBCSrvOpts **ppi;
   uint32 *        pnErr;
   OEMSMSBCSrvInfo sSMSBcSrvInfo;
} SMSAppGetSrvOptsCbData;

typedef struct {
   AEECallback     cb;
   AEECallback *   pcb;
   ACONTEXT *      pac;
   uint32 *        pnCount;
   ISMSBCSrvOpts ***pppi;
   uint32 *        pnErr;
} SMSAppGetAllSrvOptsCbData;

// Mapping of SMS teleservice ID to message type
typedef struct
{
   int ts;
   int mt;
} MsgTypeMap;

static const MsgTypeMap   gsMsgTypeMap[] = 
                  {{SMS_TELESERVICE_CPT_95,              AEESMS_TYPE_PAGE },
                  {SMS_TELESERVICE_IS91_PAGE,            AEESMS_TYPE_PAGE },
                  {SMS_TELESERVICE_VMN_95,               AEESMS_TYPE_VOICE_MAIL_NOTIFICATION },
                  {SMS_TELESERVICE_MWI,                  AEESMS_TYPE_VOICE_MAIL_NOTIFICATION },
                  {SMS_TELESERVICE_IS91_VOICE_MAIL,      AEESMS_TYPE_VOICE_MAIL_NOTIFICATION },
                  {SMS_TELESERVICE_CMT_95,               AEESMS_TYPE_TEXT },
                  {SMS_TELESERVICE_IS91_SHORT_MESSAGE,   AEESMS_TYPE_TEXT },
                  {SMS_TELESERVICE_WAP,                  AEESMS_TYPE_WAP },
                  {SMS_TELESERVICE_WEMT,                 AEESMS_TYPE_EMS },
                  {SMS_TELESERVICE_BROADCAST,            AEESMS_TYPE_BROADCAST }};

// Mapping of SMS encoding to AEE encoding
typedef struct
{
   int se;
   int ae;
} AEESMSEncMap;

static const AEESMSEncMap   gsAEESMSEncMap[] =                   
                  {{AEESMS_ENC_OCTET,            AEE_ENC_OCTET},
                  {AEESMS_ENC_LATIN,             AEE_ENC_ISOLATIN1},
                  {AEESMS_ENC_ASCII,             AEE_ENC_ISOLATIN1},
                  {AEESMS_ENC_IS91EP,            AEE_ENC_ISOLATIN1},
                  {AEESMS_ENC_IA5,               AEE_ENC_ISOLATIN1},
                  {AEESMS_ENC_UNICODE,           AEE_ENC_UNICODE},
                  {AEESMS_ENC_SHIFT_JIS,         AEE_ENC_S_JIS},
                  {AEESMS_ENC_KOREAN,            AEE_ENC_KSC5601},  
                  {AEESMS_ENC_LATIN_HEBREW,      AEE_ENC_LATIN_HEBREW},
                  {AEESMS_ENC_GSM_7_BIT_DEFAULT, AEE_ENC_GSM_7_BIT_DEFAULT}};

// Mapping of UDH message options to AEE UDH IDs
typedef struct
{
   int mo;
   int id;
} AEEMsgOptUDHIDMap;

static const AEEMsgOptUDHIDMap   gsAEEMsgOptUDHIDMap[] =                   
                  {{MSGOPT_UDH_CONCAT,       AEESMS_UDH_CONCAT_8},
                  {MSGOPT_UDH_CONCAT,        AEESMS_UDH_CONCAT_16},
                  {MSGOPT_UDH_SPECIAL_SM,    AEESMS_UDH_SPECIAL_SM},
                  {MSGOPT_UDH_PORT,          AEESMS_UDH_PORT_8},
                  {MSGOPT_UDH_PORT,          AEESMS_UDH_PORT_16},
                  {MSGOPT_UDH_TEXT_FORMAT,   AEESMS_UDH_TEXT_FORMATING},
                  {MSGOPT_UDH_PREDEFINED,    AEESMS_UDH_PRE_DEF_SOUND},
                  {MSGOPT_UDH_PREDEFINED,    AEESMS_UDH_PRE_DEF_ANIM},
                  {MSGOPT_UDH_USERDEFINED,   AEESMS_UDH_USER_DEF_SOUND},
                  {MSGOPT_UDH_USERDEFINED,   AEESMS_UDH_LARGE_ANIM},
                  {MSGOPT_UDH_USERDEFINED,   AEESMS_UDH_SMALL_ANIM},
                  {MSGOPT_UDH_USERDEFINED,   AEESMS_UDH_LARGE_PICTURE},
                  {MSGOPT_UDH_USERDEFINED,   AEESMS_UDH_SMALL_PICTURE},
                  {MSGOPT_UDH_VAR_PICTURE,   AEESMS_UDH_VAR_PICTURE},
                  {MSGOPT_UDH_RFC822,        AEESMS_UDH_RFC822},  
                  {MSGOPT_UDH_OTHER,         AEESMS_UDH_SMSC_CONTROL},  
                  {MSGOPT_UDH_OTHER,         AEESMS_UDH_SOURCE},  
                  {MSGOPT_UDH_OTHER,         AEESMS_UDH_WCMP}};

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/


static uint32      CSMSMsg_AddRef(ISMSMsg *pme);
static uint32      CSMSMsg_Release(ISMSMsg *pme);
static int         CSMSMsg_QueryInterface(ISMSMsg *pme, AEECLSID id, void **ppo);
static int         CSMSMsg_AddOpt(ISMSMsg *pme, SMSMsgOpt *awozList);
static int         CSMSMsg_RemoveOpt(ISMSMsg *pme, int32 nOptId, int32 nIndex);
static int         CSMSMsg_GetOpt(ISMSMsg *pme, int32 nOptId, int32 nIndex, SMSMsgOpt *pwo) ;

static const ISMSMsgVtbl gvtISMSMsg = {
   CSMSMsg_AddRef,
   CSMSMsg_Release,
   CSMSMsg_QueryInterface,
   CSMSMsg_AddOpt,
   CSMSMsg_RemoveOpt,
   CSMSMsg_GetOpt
};

static uint32      CSMS_AddRef(ISMS *pme);
static uint32      CSMS_Release(ISMS *pme);
static int         CSMS_QueryInterface(ISMS *pme, AEECLSID id, void **ppo);
static void        CSMS_SendMsg(ISMS *pme, ISMSMsg * pISMSMsg, AEECallback * pcb, uint32 *pnRv);
static int         CSMS_ReceiveMsg(ISMS *pme, uint32 uMsgId, ISMSMsg **ppMsg);
static int         CSMS_SetClientStatus(ISMS *pme, AEESMSClientStatus cs, AEESMSType mt);
static int         CSMS_GetBytesAvailableForPayload(ISMS * po, ISMSMsg * pISMSMsg, int * pnBytes, int * pnPadding);
static int         CSMS_GetEncodingsAvailableForMOSMS(ISMS * po, uint32 * pBuffer, uint32 * pnSize);
static void        CSMS_cbOnSMSUpdate(ISMS *pMe);

static const ISMSVtbl gvtISMS = {
   CSMS_AddRef,
   CSMS_Release,
   CSMS_QueryInterface,
   CSMS_SendMsg,
   CSMS_ReceiveMsg,
   CSMS_SetClientStatus,
   CSMS_GetBytesAvailableForPayload,
   CSMS_GetEncodingsAvailableForMOSMS
};

static uint32      CSMSNotifier_AddRef(ISMSNotifier *pme);
static uint32      CSMSNotifier_Release(ISMSNotifier *pme);
static void        CSMSNotifier_SetMask(ISMSNotifier *pme, uint32 *pdwMask);

static const ISMSNotifierVtbl gvtISMSNotifier = {
   CSMSNotifier_AddRef,
   CSMSNotifier_Release,
   CSMSNotifier_SetMask
};

// Used by ISMS for notifications
static ISMSNotifier * gpSMSNotifier = NULL;

static int                 IModelClass_AddListener(IModel *pme, ModelListener *pListener);
static void                IModelClass_Notify(IModel *pme, ModelEvent *pEvent);
static void                IModelClass_NotifyApp(IModel *pme, ModelEvent *pEvent, ACONTEXT* pac);

static const VTBL(IModel) gSMSModelMethods = {
   AEEBASE_AddRef(IModel),
   AEEBASE_Release(IModel),
   AEEBASE_QueryInterface(IModel),
   IModelClass_AddListener,
   IModelClass_Notify
};

static uint32              CSMSStorage_AddRef(ISMSStorage2 *pme);
static uint32              CSMSStorage_Release(ISMSStorage2 *pme);
static int                 CSMSStorage_QueryInterface(ISMSStorage2 *pme, AEECLSID id, void **ppo);
static void                CSMSStorage_GetStorageStatus(ISMSStorage2 * po, AEESMSStorageType st, AEESMSTag mt, AEECallback *pcb, AEESMSStorageStatus * pss, uint32 *pnRv);
static void                CSMSStorage_StoreMsg(ISMSStorage2 * po, AEESMSStorageType st, ISMSMsg * pi, AEECallback *pcb, uint32 * pnIndex, uint32 *pnRv);
static void                CSMSStorage_ReadMsg(ISMSStorage2 * pme, AEESMSStorageType st, uint32 nIndex, AEECallback *pcb, ISMSMsg ** ppi, uint32 * pnErr);
static void                CSMSStorage_Delete(ISMSStorage2 * pme, AEESMSStorageType st, AEESMSTag mt, uint32 nIndex, AEECallback *pcb, uint32 * pnErr);
static void                CSMSStorage_EnumMsgInit(ISMSStorage2 * pme, AEESMSStorageType st, AEESMSTag mt, AEECallback *pcb, uint32 * pnErr);
static void                CSMSStorage_EnumNextMsg(ISMSStorage2 * pme, AEESMSStorageType st, AEECallback *pcb, uint32 * pnIndex, ISMSMsg ** ppi, uint32 * pnErr);
static void                CSMSStorage_UpdateMsg(ISMSStorage2 * po, AEESMSStorageType st, uint32 nIndex, ISMSMsg * pi, AEECallback *pcb, uint32 *pnErr);
static void                CSMSStorage_AutoStoreMsg(ISMSStorage2 * po, AEESMSClass mc, AEESMSStorageType st, AEECallback *pcb, uint32 *pnErr);
static void                CSMSStorage_cbOnStorageUpdate(ISMSStorage2 *pMe);

static const ISMSStorage2Vtbl gvtISMSStorage = {
   CSMSStorage_AddRef,
   CSMSStorage_Release,
   CSMSStorage_QueryInterface,
   CSMSStorage_GetStorageStatus,
   CSMSStorage_StoreMsg,
   CSMSStorage_ReadMsg,
   CSMSStorage_Delete,
   CSMSStorage_EnumMsgInit,
   CSMSStorage_EnumNextMsg,
   CSMSStorage_UpdateMsg,
   CSMSStorage_AutoStoreMsg
};

static uint32      CSMSBCSrvOpts_AddRef(ISMSBCSrvOpts *pme);
static uint32      CSMSBCSrvOpts_Release(ISMSBCSrvOpts *pme);
static int         CSMSBCSrvOpts_QueryInterface(ISMSBCSrvOpts *pme, AEECLSID id, void **ppo);
static int         CSMSBCSrvOpts_AddOpt(ISMSBCSrvOpts *pme, SMSBCSrvOpts *awozList);
static int         CSMSBCSrvOpts_RemoveOpt(ISMSBCSrvOpts *pme, int32 nOptId, int32 nIndex);
static int         CSMSBCSrvOpts_GetOpt(ISMSBCSrvOpts *pme, int32 nOptId, int32 nIndex, SMSBCSrvOpts *pwo) ;

static const ISMSBCSrvOptsVtbl gvtISMSBCSrvOpts = {
   CSMSBCSrvOpts_AddRef,
   CSMSBCSrvOpts_Release,
   CSMSBCSrvOpts_QueryInterface,
   CSMSBCSrvOpts_AddOpt,
   CSMSBCSrvOpts_RemoveOpt,
   CSMSBCSrvOpts_GetOpt
};

static uint32      CSMSBCConfig_AddRef(ISMSBCConfig *pme);
static uint32      CSMSBCConfig_Release(ISMSBCConfig *pme);
static int         CSMSBCConfig_QueryInterface(ISMSBCConfig *pme, AEECLSID id, void **ppo);
static void        CSMSBCConfig_GetBCConfig(ISMSBCConfig * pme, AEESMSMode dwMode, AEECallback *pcb, AEESMSBCConfig * pConfig, uint32 * pnErr);
static void        CSMSBCConfig_GetBCPref(ISMSBCConfig * pme, AEESMSMode dwMode, AEECallback *pcb, AEESMSBCPref * pPref, uint32 * pnErr);
static void        CSMSBCConfig_SetBCPref(ISMSBCConfig * pme, AEESMSMode dwMode, AEESMSBCPref nPref, AEECallback *pcb, uint32 * pnErr);
static void        CSMSBCConfig_GetServiceIDs(ISMSBCConfig * pme, AEESMSMode dwMode, AEECallback *pcb, uint32 * pnCount, AEESMSBCSrvID ** ppsid, uint32 * pnErr);
static void        CSMSBCConfig_GetServiceOpts(ISMSBCConfig * pme, AEESMSMode dwMode, AEESMSBCSrvID sSrvID, AEECallback *pcb, ISMSBCSrvOpts ** ppi, uint32 * pnErr);
static void        CSMSBCConfig_SetServiceOpts(ISMSBCConfig * pme, AEESMSMode dwMode, ISMSBCSrvOpts * pi, AEECallback *pcb, uint32 * pnErr);
static void        CSMSBCConfig_GetAllServiceOpts(ISMSBCConfig * pme, AEESMSMode dwMode, AEECallback *pcb, uint32 * pnCount, ISMSBCSrvOpts *** pppi, uint32 * pnErr);
static void        CSMSBCConfig_DeleteService(ISMSBCConfig * pme, AEESMSMode dwMode, AEESMSBCSrvID sSrvID, AEECallback *pcb, uint32 * pnErr);
static void        CSMSBCConfig_cbOnSMSBCConfigUpdate(ISMSBCConfig *pMe);

static const ISMSBCConfigVtbl gvtISMSBCConfig = {
   CSMSBCConfig_AddRef,
   CSMSBCConfig_Release,
   CSMSBCConfig_QueryInterface,
   CSMSBCConfig_GetBCConfig,
   CSMSBCConfig_GetBCPref,
   CSMSBCConfig_SetBCPref,
   CSMSBCConfig_GetServiceIDs,
   CSMSBCConfig_GetServiceOpts,
   CSMSBCConfig_SetServiceOpts,
   CSMSBCConfig_GetAllServiceOpts,
   CSMSBCConfig_DeleteService
};

/*======================================================================= 
Method Prototypes
=======================================================================*/
static int GetEncodedMsgPayload(char* pszBrewPrefix, byte * pMsgPayload, int nBytes, int nEncoding, AEESMSEncType nSMSEncoding, byte * pEncodedPayload, int * pnBytes, int * pnPadding);

/*======================================================================= 
Local Method Prototypes
=======================================================================*/

static void DecodePackedString(byte *pSrc, int i, char *pszDst);
static void DecodeNetworkOrder(byte *pbSrc, int i, byte *pszDst);
static void DecodeString(byte *pSrc, int i, char *pszDst);
static void EncodeNetworkOrder(AECHAR * pSrc, int i, uint16 * pszDst);
static void PackString(char * pText, int nText, byte * pBuffer);

static void InvokeAppCb(void * pData);
static void ReleaseAppCb(AEECallback * pcb);
static int LinkAppCb(AEECallback * pcb, SMSAppCbData ** ppAppCbData);
static int LinkAppSendMsgCb(AEECallback * pcb, SMSAppSendMsgCbData ** ppAppSendMsgCbData);
static int LinkAppStoreMsgCb(AEECallback * pcb, SMSAppStoreMsgCbData ** ppAppStoreMsgCbData);
static int LinkAppReadMsgCb(AEECallback * pcb, SMSAppReadMsgCbData ** ppAppReadMsgCbData);
static int LinkAppDeleteMsgCb(AEECallback * pcb, SMSAppDeleteMsgCbData ** ppAppDeleteMsgCbData);
static int LinkAppEnumMsgCb(AEECallback * pcb, SMSAppEnumMsgCbData ** ppAppEnumMsgCbData);
static int LinkAppGetSrvIDsCb(AEECallback * pcb, SMSAppGetSrvIDsCbData ** ppAppGetSrvIDsCbData);
static int LinkAppGetSrvOptsCb(AEECallback * pcb, SMSAppGetSrvOptsCbData ** ppAppGetSrvOptsCbData);
static int LinkAppGetAllSrvOptsCb(AEECallback * pcb, SMSAppGetAllSrvOptsCbData ** ppAppGetAllSrvOptsCbData);
static int ISMSMsgToOEMMsg(ISMSMsg * pISMSMsg, OEMMsgData ** ppMsgData, OEMMsgPayload ** ppMsgPayload);
static int OEMMsgToISMSMsg(OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, ISMSMsg ** ppISMSMsg);
static int Xlate_TStoMT(int ts);
static int Xlate_MTtoTS(int mt);
static int Xlate_SMSEncToAEEEnc(int se);
static int Xlate_AEEEncToSMSEnc(int ae);
static int Xlate_UDHIDToMsgOpt(int id);
static void ParseBrewMsg(char *pText, int nBytes, AEECLSID *pCls, 
                         char ** ppszBrewHandler, byte **ppParsedText, byte *pDelimiter );
static char * StripOffPrefix( char *pszText );
static boolean IsMsgPackable(byte * pData, int nBytes, int en, boolean b7bit);
static void PackUDHMsgOpts(byte * pBytes, int nBytes, ISMSMsg * pISMSMsg);
static void UnpackUDHMsgOpts(ISMSMsg *pISMSMsg, int * pnBytes, byte * pBytes);
static int ComputeUDHSizeOnSMSPayload(ISMSMsg * pISMSMsg);
static boolean UnpackSMSBCSrvOpts(ISMSBCSrvOpts *pISMSBCSrvOpts, OEMSMSBCSrvInfo * pSrvInfo);
static boolean PackSMSBCSrvOpts(OEMSMSBCSrvInfo * pSrvInfo, ISMSBCSrvOpts *pISMSBCSrvOpts);

static void CSMS_Dtor(ISMS * pme);
static void CSMS_OnSMSUpdate(AEECallback * pcb, OEMSMSUpdateData * pSMSUpdateData);
static void CSMS_cbFromOEMLayerOnSMSUpdate(void * po);

typedef struct _CSMSStaticData CSMSStaticData;
struct _CSMSStaticData
{
   boolean bDebugMsg;
   uint32 dwCount;
   AEECallback cbFromOEMLayerOnSMSUpdate;
   OEMSMSUpdateData sUpdateData;
   AEECallback * pcbOnSMSUpdate;
};

static CSMSStaticData gSMS = {0};

static void CSMSStorage_Dtor(ISMSStorage2 * pme);
static void CSMSStorage_OnStorageUpdate(AEECallback * pcb, OEMStorageUpdateData * pStorageUpdateData);
static void CSMSStorage_cbFromOEMLayerOnStorageUpdate(void * po);
static void CSMSStorage_NotifyFromOEMLayerOnStorageUpdate(void * po);

typedef struct _CSMSStorageStaticData CSMSStorageStaticData;
struct _CSMSStorageStaticData
{
   uint32 dwCount, dwResumeFrom, dwAddResumeTo, dwTotalEntries, dwUsedEntries;
   AEECallback * pcbOnStorageUpdate;
   AEECallback cbFromOEMLayerOnStorageUpdate;
   OEMStorageUpdateData sUpdateData[DEFAULT_EVENT_CACHE_ENTRIES];
};

static CSMSStorageStaticData * gpSMSStorage = 0;

static void CSMSBCConfig_Dtor(ISMSBCConfig * pme);
static void CSMSBCConfig_OnSMSBCConfigUpdate(AEECallback * pcb, OEMSMSUpdateData * pSMSBCConfigUpdateData);
static void CSMSBCConfig_cbFromOEMLayerOnSMSBCConfigUpdate(void * po);

typedef struct _CSMSBCConfigStaticData CSMSBCConfigStaticData;
struct _CSMSBCConfigStaticData
{
   uint32 dwCount;
   AEECallback cbFromOEMLayerOnSMSBCConfigUpdate;
   OEMSMSUpdateData sUpdateData;
   AEECallback * pcbOnSMSBCConfigUpdate;
};

static CSMSBCConfigStaticData gSMSBCConfig = {0};
/*======================================================================= 
Extern Method Prototypes
=======================================================================*/
extern void AEETAPI_NotifyOnMtSMS( uint32 dwTransactionId, int ts );

/*===========================================================================

Function: ISMSMsg_New

Description:
   ISMSMsg constructor
 
===========================================================================*/
int ISMSMsg_New(IShell *pIShell, AEECLSID uCls, void **ppif)
{
   // uCls is intentionally unused
   int nReturn = ENOMEMORY;
   SMSMsgOpt awoz[1];
   ISMSMsg *pme = NULL;

   FARF(SMS, ("==>  ISMSMsg_New"));

   // Make empty option list
   awoz[0].nId  = WEBOPT_END;

   // Get our memory including a WebOpt manager
   pme = MALLOCREC_EX(ISMSMsg, WebOptMgr_GetSize(awoz));

   *ppif = pme;

   if (pme) {
      // Our vtable
      pme->pvt = &gvtISMSMsg;

      pme->pWom = (WebOptMgr *)(pme+1);
      WebOptMgr_CtorZ(pme->pWom,awoz);

      // The shell 
      pme->uRef = 1;
      pme->pIShell = pIShell;
      ISHELL_AddRef(pIShell);

      nReturn = SUCCESS;
   }
   return nReturn;
}

/*===========================================================================

Function: CSMSMsg_AddRef

Description:
   Increases reference count
 
===========================================================================*/
static uint32 CSMSMsg_AddRef(ISMSMsg *pme)
{   
   FARF(SMS, ("==>  CSMSMsg_AddRef"));
   return ++pme->uRef;
}

/*===========================================================================

Function: CSMSMsg_Release

Description:
   Decrements reference count. Frees object when reference count reaches 0. 
 
===========================================================================*/
static uint32 CSMSMsg_Release(ISMSMsg *pme)
{   
   FARF(SMS, ("==>  CSMSMsg_Release"));

   if (--pme->uRef) {
      return pme->uRef;
   }

   WebOptMgr_Dtor(pme->pWom);
   RELEASEIF(pme->pIShell);
   FREEIF(pme);

   return 0;
}

/*===========================================================================

Function: CSMSMsg_QueryInterface

Description:
   Query Interface
 
===========================================================================*/
static int CSMSMsg_QueryInterface(ISMSMsg *pme, AEECLSID id, void **ppo)
{  
   FARF(SMS, ("==>  CSMSMsg_QueryInterface"));

   if ((id == AEECLSID_QUERYINTERFACE) || 
       (id == AEECLSID_SMSMSG)         ||
       (id == AEECLSID_WEBOPTS)) 
   {
      *ppo = pme;
      IQI_AddRef(*ppo);
      return SUCCESS;
   } 
   else 
   {
      return ECLASSNOTSUPPORT;
   }
}

/*===========================================================================

Function: CSMSMsg_AddOpt

Description:
   Uses base class (IWebOpts) implementation
 
===========================================================================*/
static int CSMSMsg_AddOpt(ISMSMsg *pme, SMSMsgOpt *awozList)
{  
   FARF(SMS, ("==>  CSMSMsg_AddOpt"));
   return WebOptMgr_AddOptV(pme->pWom, awozList);
}


/*===========================================================================

Function: CSMSMsg_RemoveOpt

Description:
   Uses base class (IWebOpts) implementation
 
===========================================================================*/
static int CSMSMsg_RemoveOpt(ISMSMsg *pme, int32 nOptId, int32 nIndex)
{  
   FARF(SMS, ("==>  CSMSMsg_RemoveOpt:Id=%d", nOptId));
   return WebOptMgr_RemoveOpt(pme->pWom,nOptId, nIndex);
}

/*===========================================================================

Function: CSMSMsg_GetOpt

Description:
   Uses base class (IWebOpts) implementation
 
===========================================================================*/
static int CSMSMsg_GetOpt(ISMSMsg *pme, int32 nOptId, int32 nIndex, SMSMsgOpt *pwo)
{  
   FARF(SMS, ("==>  CSMSMsg_GetOpt:Id=%d", nOptId));
   return WebOptMgr_GetOpt(pme->pWom,nOptId, nIndex, pwo);
}

/*===========================================================================

Function: ISMS_New

Description:
   ISMS constructor
 
===========================================================================*/
int ISMS_New(IShell *pIShell, AEECLSID uCls, void **ppif)
{   
   int nErr = SUCCESS;
   OEMSMSMgr * pif = NULL;
   ISMS * pme = NULL;

   FARF(SMS, ("==>  ISMS_New"));

   if (SUCCESS == (nErr = OEMSMS_New(&pif))) 
   {
      pme = (ISMS *)MALLOC(sizeof(ISMS));

      *ppif = pme;

      if (pme) 
      {
         pme->pvt      = &gvtISMS;
         pme->pOEMSms  = pif;

         pme->uRef     = 1;
         pme->pIShell  = pIShell;
         ISHELL_AddRef(pIShell);         
         
         // Initialize the model interface...
         pme->model.pMe = (IBase*)pme;
         pme->model.pvt = &gSMSModelMethods;

         // Guard against application exiting without calling ISMS_Release()
         CALLBACK_Init(&pme->cbSysObj, CSMS_Dtor, pme);
         AEE_LinkSysObject(&pme->cbSysObj);

         CALLBACK_Init(&pme->cbOnSMSUpdate, CSMS_cbOnSMSUpdate, (void*)pme);
         CSMS_OnSMSUpdate(&pme->cbOnSMSUpdate, &pme->sUpdateData);

         if (!gSMS.dwCount++) {
            CALLBACK_Init(&gSMS.cbFromOEMLayerOnSMSUpdate, CSMS_cbFromOEMLayerOnSMSUpdate, &gSMS);
            OEMSMS_OnSMSUpdate(&gSMS.sUpdateData, &gSMS.cbFromOEMLayerOnSMSUpdate);
         }

         pme->pac = AEE_GetAppContext();
      }
      else 
      {                       
         OEMSMS_Release(pif);
         nErr = ENOMEMORY;
      }
   }

   return nErr;
}

/*===========================================================================

Function: CSMS_AddRef

Description:
   Increases reference count
 
===========================================================================*/
static uint32 CSMS_AddRef(ISMS *pme)
{
   FARF(SMS, ("==>  CSMS_AddRef"));
   return ++pme->uRef;
}

/*===========================================================================

Function: CSMS_Release

Description: 
   Decrements reference count. Frees object when reference count reaches 0. 
 
===========================================================================*/
static uint32 CSMS_Release(ISMS *pme)
{   
   FARF(SMS, ("==>  CSMS_Release"));

   if (--pme->uRef) {
      return pme->uRef;
   }

   CSMS_Dtor(pme);
   RELEASEIF(pme->pIShell);
   OEMSMS_Release(pme->pOEMSms);
   FREEIF(pme);

   return 0;
}

/*===========================================================================

Function: CSMS_QueryInterface

Description:
   Query Interface
 
===========================================================================*/
static int CSMS_QueryInterface(ISMS *pme, AEECLSID id, void **ppo)
{
   IBase * po = NULL;

   FARF(SMS, ("==>  CSMS_QueryInterface"));

   switch (id) 
   {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_SMS:
         po = (IBase*)pme;
         break;

      case AEEIID_MODEL:
         po = (IBase*)&pme->model;
         break;

      default:
         break;
   }
 
   *ppo = po;

   if (po) {
      IBASE_AddRef(po);
      return SUCCESS;
   }
      
   return ECLASSNOTSUPPORT;
}

/*===========================================================================

Function: CSMS_SendMsg

Description:
   Sends specified message
 
===========================================================================*/
static void CSMS_SendMsg(ISMS *pme, ISMSMsg * pISMSMsg, AEECallback * pcb, uint32 *pnRv)
{
   OEMMsgData * pMsgData = NULL, * pReallocData = NULL;
   OEMMsgPayload * pMsgPayload = NULL;
   SMSAppSendMsgCbData * pAppSendMsgCbData = NULL;
   int nErr = SUCCESS;

   FARF(SMS, ("==>  CSMS_SendMsg"));

   if ((nErr = ISMSMsgToOEMMsg(pISMSMsg, &pMsgData, &pMsgPayload)) != SUCCESS)
      goto Done;
   
   // Expand pMsgData to accommodate AlphaID
   if ((pReallocData = REALLOC(pMsgData, sizeof(OEMMsgData) + 
                                         pMsgData->nUDHLength + 
                                         MAX_ALPHA_ID_LENGTH)) == NULL)
   {
      nErr = ENOMEMORY;
      goto Done;
   }

   pMsgData = pReallocData;
   pMsgData->nAlphaIDLength = MAX_ALPHA_ID_LENGTH;

   if ((nErr = LinkAppSendMsgCb(pcb, &pAppSendMsgCbData)) == SUCCESS)
   {
      // Set the sender's class id information
      // (used for loopback messages)
      pMsgData->nFromAppletCls = AEE_GetAppContextCls(pAppSendMsgCbData->pac);

      ISMS_AddRef(pme);
      pAppSendMsgCbData->pme = pme;
      pAppSendMsgCbData->pnRv = pnRv;
      pAppSendMsgCbData->pMsgData = pMsgData;
      pAppSendMsgCbData->pMsgPayload = pMsgPayload;
      OEMSMS_SendMessage(pme->pOEMSms, pMsgData, pMsgPayload, pAppSendMsgCbData->pnRv, &pAppSendMsgCbData->cb);
   }

Done:
   // Handle Error Here. No Error means that we are able to call OEMSMS_SendMessage
   if (nErr)
   {  
      if (nErr == ENOMEMORY)
         AEESMS_SETERROR(*pnRv, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_NO_MEMORY);
      else
         AEESMS_SETERROR(*pnRv, AEESMS_ERRORTYPE_TEMP, AEESMS_ERROR_GENERAL_PROBLEM);

      if (pAppSendMsgCbData) {
         ISHELL_Resume(pme->pIShell, pAppSendMsgCbData->pcb);
      }
      else {
         FREEIF(pMsgData);
         FREEIF(pMsgPayload);
         ISHELL_Resume(pme->pIShell, pcb);
      }
   }
}

/*===========================================================================

Function: CSMS_ReceiveMsg

Description:
   Receives specified message
 
===========================================================================*/
static int CSMS_ReceiveMsg(ISMS *pme, uint32 uMsgId, ISMSMsg **ppMsg)
{
   int nErr = EFAILED;
   ISMSMsg * pISMSMsg = NULL;
   OEMMsgData * pMsgData = NULL;
   OEMMsgPayload * pMsgPayload = NULL;
   byte * pszText = NULL;
   OEMMsgPayload sMsgPayload;

   FARF(SMS, ("==>  CSMS_ReceiveMsg"));
   
   if ((pMsgData = MALLOC(sizeof(OEMMsgData))) == NULL)
   {
      nErr = ENOMEMORY;
      goto Done;
   }

   if ((nErr = OEMSMS_GetMsgData(pme->pOEMSms, uMsgId, pMsgData)) != SUCCESS)
   {
      FARF(SMS, ("Failed to get message data"));
      goto Done;
   }

   if (pMsgData->nUDHLength || pMsgData->nRawTPDULength || pMsgData->nAlphaIDLength)
   {
      // Coming here means message data has UDH or/and raw TPDU. Reallocate size 
      // and get message data with UDH or/and raw TPDU 
      OEMMsgData * pTemp = NULL;

      if ((pTemp = REALLOC(pMsgData, sizeof(OEMMsgData) + 
                                     pMsgData->nUDHLength + 
                                     pMsgData->nRawTPDULength +
                                     pMsgData->nAlphaIDLength)) == NULL)
      {
         nErr = ENOMEMORY;
         goto Done;
      }
      else
      {
         pMsgData = pTemp;
         pTemp = NULL;
      }

      if ((nErr = OEMSMS_GetMsgData(pme->pOEMSms, uMsgId, pMsgData)) != SUCCESS)
      {
         FARF(SMS, ("Failed to get message data"));
         goto Done;
      }
   }

   MEMSET(&sMsgPayload, 0, sizeof(OEMMsgPayload));

   if (OEMSMS_GetMsgPayload(pme->pOEMSms, uMsgId, &sMsgPayload) != SUCCESS)
   {
      FARF(SMS, ("Failed to get message payload size"));
      nErr = EFAILED;
      goto Done;
   }

   if ((pMsgPayload = MALLOC(sizeof(OEMMsgPayload) + sMsgPayload.nBytes)) == NULL)
   {
      FARF(SMS, ("Memory Allocation Failed"));
      nErr = ENOMEMORY;
      goto Done;
   }

   pMsgPayload->nBytes = sMsgPayload.nBytes;

   if (OEMSMS_GetMsgPayload(pme->pOEMSms, uMsgId, pMsgPayload) != SUCCESS)
   {
      FARF(SMS, ("Failed to get message payload"));
      nErr = EFAILED;
      goto Done;
   }      
  
   if (Xlate_TStoMT(pMsgData->ts) == AEESMS_TYPE_TEXT)
   {
      pszText = AEE_GetDecodedTextString(pMsgPayload->data, pMsgPayload->nBytes, pMsgPayload->nEncoding, NULL, NULL);

      if (pszText)
      {
         if (AEE_IsBREWDirectedSMS((char*)pszText) == TRUE)
         {
            // BREW directed messages will be fetched by destination class only
            // so mark OEM layer entry as free
            FARF(SMS, ("Free Msg=%d", uMsgId));
            OEMSMS_FreeMsg(pme->pOEMSms, uMsgId);
         }
         FREE(pszText);
      }
   }

   if ((nErr = OEMMsgToISMSMsg(pMsgData, pMsgPayload, &pISMSMsg)) != SUCCESS)
   {         
      FARF(SMS, ("Failed to create ISMSMsg"));
      nErr = EFAILED;
      goto Done;
   }

Done:
   FREEIF(pMsgData);         
   FREEIF(pMsgPayload);

   if (nErr == SUCCESS)
      *ppMsg = pISMSMsg;

   return nErr;
}

/*===========================================================================

Function: CSMS_SetClientStatus

Description:
   Sets main client status for the specified message type. Client needs 
   AEECLSID_SMSMAINCLIENT privilege for this.
 
===========================================================================*/
static int CSMS_SetClientStatus(ISMS *pme, AEESMSClientStatus cs, AEESMSType mt)
{
   FARF(SMS, ("==>  CSMS_SetClientStatus"));

   if (!pme->pOEMSms)
      return EFAILED;

   if (ISHELL_CheckPrivLevel(pme->pIShell, AEECLSID_SMSMAINCLIENT, TRUE)) 
   {
      return OEMSMS_SetClientStatus(pme->pOEMSms, cs, mt);
   }
   else
   {
      FARF(SMS,("FindClass: No ext class entry for priv class %x",AEECLSID_SMSMAINCLIENT));
      return EPRIVLEVEL;
   }
}

/*===========================================================================

Function: CSMS_GetBytesAvailableForPayload

Description:
   Checks various ISMSMsg parameters and finds that how many more bytes and
   are available for payload. Also specifies if bits are padded for last byte.

===========================================================================*/
static int CSMS_GetBytesAvailableForPayload(ISMS * po, ISMSMsg * pISMSMsg, int * pnBytes, int * pnPadding)
{
   SMSMsgOpt smo;
   int nEncoding, nBytes = 0;
   int nMaxPayloadLength, nPayloadBytes = 0, nPadding = 0, nErr = SUCCESS;
   byte * pMsgPayload = NULL;
   char * pszBrewPrefix = NULL;
   AEESMSEncType nSMSEncoding;

   FARF(SMS, ("==>  CSMS_GetBytesAvailableForPayload"));

   if (OEM_GetConfig(CFGI_SMS_MAX_PAYLOAD_LENGTH, &nMaxPayloadLength, sizeof(nMaxPayloadLength)) != SUCCESS)
   {   
      FARF(SMS, ("Max Payload Length Not Configured"));
      nErr = EFAILED;
      goto Done;
   }
   else
   {
      if (!pISMSMsg)
      {
         nErr = SUCCESS;
         goto Done;
      }
   }
   
   // Get MO SMS Encoding
   if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_MOSMS_ENCODING, &smo) == SUCCESS)
      nSMSEncoding = (uint32)smo.pVal;
   else
   {
      // Get default MO SMS Encoding
      if (OEM_GetConfig(CFGI_SMS_DEFAULT_MO_ENC, &nSMSEncoding, sizeof(nSMSEncoding)) != SUCCESS)
      {   
         FARF(SMS, ("MO SMS Encoding Not Specified"));
         nErr = EFAILED;
         goto Done;
      }
   }

   // See if there is a destination class or handler
   if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BREW_CLASS, &smo) == SUCCESS)
   {
      char szBuffer[32];
      nBytes = SNPRINTF(szBuffer, sizeof(szBuffer), "%s%.8x:", BREW_SMS_PREFIX, (AEECLSID)smo.pVal);
      if ((pszBrewPrefix = MALLOC(nBytes + 1)) != NULL)
         STRLCPY(pszBrewPrefix, szBuffer, nBytes + 1);
      else
      {
         nErr = ENOMEMORY;
         goto Done;
      }
   }
   else if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BREW_HANDLER_SZ, &smo) == SUCCESS)
   {
      boolean bSpace = FALSE;

      // Account for //%s:
      nBytes = STRLEN((char*)smo.pVal) + 3;

      if (nBytes & 0x1)
      {
         bSpace = TRUE;
         nBytes++;
      }
      
      if ((pszBrewPrefix = MALLOC(nBytes + 1)) != NULL)
      {
         if (bSpace)
            SNPRINTF(pszBrewPrefix, nBytes + 1, "// %s:", (char*)smo.pVal);
         else
            SNPRINTF(pszBrewPrefix, nBytes + 1, "//%s:", (char*)smo.pVal);
      }
      else
      {
         nErr = ENOMEMORY;
         goto Done;
      }
   }
      
   // Get Message Payload Encoding
   if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_ENCODING, &smo) == SUCCESS)
      nEncoding = (uint32)smo.pVal;
   else
   {
      // Assume Device Encoding by default
      AEEDeviceInfo di;
      di.wStructSize = sizeof(AEEDeviceInfo);
      AEE_GetDeviceInfo(&di);
      nEncoding = di.wEncoding; 
   }

   // Get Message Payload
   if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_SZ, &smo) == SUCCESS)
   {
      pMsgPayload = (byte*)smo.pVal;
      nPayloadBytes = STRLEN((char*)smo.pVal);
   }
   else if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_WSZ, &smo) == SUCCESS)
   {
      pMsgPayload = (byte*)smo.pVal;
      nPayloadBytes = WSTRLEN((AECHAR*)smo.pVal)*sizeof(AECHAR);
   }
   else if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_BINARY, &smo) == SUCCESS)
   {
      pMsgPayload = (byte*)WEBOPT_GETVARBUFFERDATA(smo.pVal);
      nPayloadBytes = WEBOPT_GETVARBUFFERLENGTH(smo.pVal);
   }

   if ((nErr = GetEncodedMsgPayload(pszBrewPrefix, pMsgPayload, nPayloadBytes, nEncoding, nSMSEncoding, NULL, &nBytes, &nPadding)) != SUCCESS)
      goto Done;

   // Account for UDH size on SMS payload
   nBytes += ComputeUDHSizeOnSMSPayload(pISMSMsg);

Done:
   FREEIF(pszBrewPrefix);

   if (nErr == SUCCESS)
   {
      if (pnBytes)
         *pnBytes = nMaxPayloadLength - nBytes;

      if (pnPadding)
         *pnPadding = nPadding;
   }

   return nErr;
}

/*===========================================================================

Function: CSMS_GetEncodingsAvailableForMOSMS

Description:
   Queries OEM layer to list encodings available for MO SMS.

===========================================================================*/
static int CSMS_GetEncodingsAvailableForMOSMS(ISMS * po, uint32 * pBuffer, uint32 * pnSize)
{
   int nErr;
   uint32 nSize;

   if (!pBuffer && !pnSize)
      return EBADPARM;

   if ((nErr = OEM_GetConfig(CFGI_SMS_MO_ENC_TYPES_COUNT, &nSize, sizeof(nSize))) == SUCCESS)
   {
      nSize *= sizeof(uint32); // Convert count to size

      if (pBuffer && (*pnSize >= nSize))
         nErr = OEM_GetConfig(CFGI_SMS_MO_ENC_TYPES, pBuffer, *pnSize);
      else
         *pnSize = nSize;
   }

   return nErr;
}

/*======================================================================= 
Invoked when OEM layer wants to notify an event
=======================================================================*/
static void CSMS_cbOnSMSUpdate(ISMS *pMe)
{
   ModelEvent me;
   uint32 * pData = NULL;
   
   FARF(SMS, ("==>  CSMS_cbOnSMSUpdate"));

   MEMSET(&me, 0, sizeof(me));

   me.evCode = pMe->sUpdateData.nEvent;
   me.pModel = &pMe->model;
   pData = (uint32*)pMe->sUpdateData.aEventData;

   switch (pMe->sUpdateData.nEvent)
   {
      case EVT_MDL_SMS_DUPLICATE_CB_PAGE:
         {
            SMSMsgOpt smo[2];
            ISMSMsg * pISMSMsg = NULL;

            if (SUCCESS != ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_SMSMSG, (void**)&me.dwParam))
               goto Done;

            pISMSMsg = (ISMSMsg *)me.dwParam;
      
            MEMSET(smo, 0, sizeof(smo));

            smo[1].nId = MSGOPT_END;

            // Set for copying of options
            smo[0].nId = MSGOPT_COPYOPTS;
            smo[0].pVal = (void *)TRUE;
               
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

            // Fill up the GW CB Message Options
            smo[0].nId = MSGOPT_BC_SERVICE_ID;
            smo[0].pVal = (void*)*pData;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            pData++;

            smo[0].nId = MSGOPT_BC_PAGE_NUMBER;
            smo[0].pVal = (void*)*pData;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            pData++;

            smo[0].nId = MSGOPT_BC_TOTAL_PAGES;
            smo[0].pVal = (void*)*pData;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            pData++;

            smo[0].nId = MSGOPT_BC_GEO_SCOPE;
            smo[0].pVal = (void*)*pData;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            pData++;

            smo[0].nId = MSGOPT_BC_MESSAGE_CODE;
            smo[0].pVal = (void*)*pData;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            pData++;

            smo[0].nId = MSGOPT_BC_UPDATE_NUMBER;
            smo[0].pVal = (void*)*pData;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            pData++;

            smo[0].nId = MSGOPT_LANG;
            smo[0].pVal = (void*)*pData;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            pData++;

            smo[0].nId = MSGOPT_PAYLOAD_ENCODING;
            smo[0].pVal = (void*)*pData;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            pData++;

            IModelClass_NotifyApp(&pMe->model, &me, pMe->pac);

            ISMSMSG_Release(pISMSMsg);
         }
         break;

      case EVT_MDL_SMS_CELL_CHANGE:
         me.dwParam = *pData;
         IModelClass_NotifyApp(&pMe->model, &me, pMe->pac);
         break;

      case EVT_MDL_SMS_CS_CHANGE:
         me.dwParam = (uint32) pData;
         IModelClass_NotifyApp(&pMe->model, &me, pMe->pac);
         break;

      default:
         break;
   }

Done:
   CSMS_OnSMSUpdate(&pMe->cbOnSMSUpdate, &pMe->sUpdateData);
}

/*======================================================================= 
EXPORTED cb from OEM
Get any new messages from the system.
On every new message parse if needed for brew directed messages.
Start or notify the applets that are destined.
=======================================================================*/
static void CSMSNotifier_cbOnMtSMS( ISMSNotifier *pMe )
{
   uint32 dwTransactionId = 0, dwBytes = 0;
   int ts = 0, mt = 0;
   boolean bIsLoopback = FALSE;
   boolean bNoClientWantsThisMsg;
   AEECLSID clsid;
   byte delimiter = 0;

   FARF(SMS, ("==>  CSMSNotifier_cbOnMtSMS"));
   
   while( 1 ) 
   {
      bNoClientWantsThisMsg = TRUE; // Reset bNoClientWantsThisMsg
      clsid = 0;                    // Reset destination class ID

      if( SUCCESS != OEMSMS_GetNewMsgInfo( pMe->pOEMSms, &dwTransactionId, &ts, &bIsLoopback ) ) 
      {   
         FARF(SMS, ("No Msg Pending"));
         break;
      }
      
      // Translate Teleservice ID to Message Type
      mt = Xlate_TStoMT(ts);
   
      FARF(SMS, ("New message with ID = %d, type = %d", dwTransactionId, mt)); 

      // First check if it is loopback
      if ( bIsLoopback )
      {
         FARF(SMS, ("Loopback message"));
         if( pMe->dwNotify & NMASK_SMS_TYPE_LOOPBACK ) 
         {
            FARF(SMS, ("Loopback message Notification"));
            ISHELL_Notify( pMe->pIShell, AEECLSID_SMSNOTIFIER, (mt << 16l)|NMASK_SMS_TYPE_LOOPBACK, (void*)dwTransactionId );
            bNoClientWantsThisMsg = FALSE;
         }
      }
      else if( mt == AEESMS_TYPE_TEXT) 
      {
         char * pszBrewHandler = NULL;
         byte * pText = NULL, * pParsedText = NULL;
         OEMMsgPayload * pMsgPayload = NULL;
         OEMMsgPayload   sMsgPayload;
        
         MEMSET(&sMsgPayload, 0, sizeof(OEMMsgPayload));

         if (OEMSMS_GetMsgPayload(pMe->pOEMSms, dwTransactionId, &sMsgPayload) != SUCCESS)
         {
            FARF(SMS, ("Failed to get message size"));
            break;
         }
         
         // If payload is empty, there is no need to check for BREW directed messages.
         // Directly post text message notification.
         if (sMsgPayload.nBytes == 0) 
         {
            if( pMe->dwNotify & NMASK_SMS_TYPE ) 
            {
               FARF(SMS, ("Text message Notification"));
               ISHELL_Notify( pMe->pIShell, AEECLSID_SMSNOTIFIER, (AEESMS_TYPE_TEXT << 16l)|NMASK_SMS_TYPE, (void*)dwTransactionId );
               bNoClientWantsThisMsg = FALSE;
            }
         }
         else 
         {
            if ((pMsgPayload = sys_malloc(sizeof(OEMMsgPayload) + sMsgPayload.nBytes)) == NULL)
            {
               FARF(SMS, ("Memory Allocation Failed"));
               break;
            }

            pMsgPayload->nBytes = sMsgPayload.nBytes;

            if (OEMSMS_GetMsgPayload(pMe->pOEMSms, dwTransactionId, pMsgPayload) != SUCCESS)
            {
               FARF(SMS, ("Failed to get message data"));
               sys_free(pMsgPayload);
               break;
            }

            if ((pText = AEE_GetDecodedTextString(pMsgPayload->data, pMsgPayload->nBytes, pMsgPayload->nEncoding, NULL, &dwBytes)) == NULL)
            {
               FARF(SMS, ("Memory Allocation Failed"));
               FREEIF(pMsgPayload);
               break;
            }
            
            ParseBrewMsg((char*)pText, dwBytes, &clsid, &pszBrewHandler, &pParsedText, &delimiter);

            if (clsid || pszBrewHandler) 
            {      
               // This is a brew directed message and needs special handling
               if (pszBrewHandler)
               {
                  if (clsid)
                  {
                     // Message if of //<SMSHandler>:<Payload> type
                     char * pData = StripOffPrefix((char*)pText);     // pData = //<SMSHandler>'\0'<Payload>

                     // While sending handler directed SMS, odd length handler are made even length by placing ' '
                     // after "//". This is done to ensure that //<SMSHandler>:<Payload> can pass transformation from
                     // payload encoding to MO SMS encoding. Following check ensures that this is not exposed to
                     // applications
                     if (!STRNCMP(pData, "// ", STRLEN("// ")))
                     {
                        MEMMOVE((void *)pData, (void *)" //", STRLEN(" //"));  // Replace "// XXXX'\0'<Payload>" with " //XXXX'\0'<Payload>"
                        pData++;                               // pData = "//XXXX'\0'<Payload>"
                     }
                     
                     if (delimiter)
                     {
                        // Make pData = "//XXXX:<Payload>"
                        *(pData + STRLEN("//") + STRLEN(pszBrewHandler)) = delimiter;
                     }
                     FARF(SMS, ("EVT_APP_BROWSE_URL to cls=0x%x, msg=%s", clsid, pData));
                     ISHELL_StartAppletArgs(pMe->pIShell, clsid, pData); 
                     
                     FARF(SMS, ("Post EVT_APP_MESSAGE_EX to cls=0x%x, tid=%d", clsid, dwTransactionId));
                     ISHELL_PostEvent( pMe->pIShell, clsid, EVT_APP_MESSAGE_EX, 0, dwTransactionId );
                     bNoClientWantsThisMsg = FALSE;
                  }
                  else
                  {                 
                     FARF(SMS, ("No class found for %s", pszBrewHandler));
                  }
               }
               else
               {
                  AEEAppInfo ai;

                  if (ISHELL_QueryClass(pMe->pIShell, clsid, &ai) == TRUE) 
                  {
                     // Message is of //BREW:<ClsID>:<Payload> type. pParsedText points to 
                     // <Payload>
                     FARF(SMS, ("EVT_APP_MESSAGE to cls=0x%x, en=%d, msg=%s", clsid, pMsgPayload->nEncoding, pParsedText));
                     ISHELL_SendEvent(pMe->pIShell, clsid, EVT_APP_MESSAGE, (uint16)pMsgPayload->nEncoding, (uint32)pParsedText);
                     
                     FARF(SMS, ("Send EVT_APP_MESSAGE_EX to cls=0x%x, tid=%d", clsid, dwTransactionId));
                     ISHELL_SendEvent( pMe->pIShell, clsid, EVT_APP_MESSAGE_EX, 0, dwTransactionId );
                     bNoClientWantsThisMsg = FALSE;
                  }
                  else
                  {                 
                     FARF(SMS, ("cls=0x%x not found", clsid));
                  }
               }
            }
            else
            {
               if( pMe->dwNotify & NMASK_SMS_TYPE ) 
               {
                  FARF(SMS, ("Text message Notification"));
                  ISHELL_Notify( pMe->pIShell, AEECLSID_SMSNOTIFIER, (AEESMS_TYPE_TEXT << 16l)|NMASK_SMS_TYPE, (void*)dwTransactionId );
                  bNoClientWantsThisMsg = FALSE;
               }
            }
            sys_free(pMsgPayload);
            FREEIF(pText);
         }
      }
      else 
      {
         if( pMe->dwNotify & NMASK_SMS_TYPE ) 
         {   
            FARF(SMS, ("Message Type %d Notification", mt));
            ISHELL_Notify( pMe->pIShell, AEECLSID_SMSNOTIFIER, (mt << 16l)|NMASK_SMS_TYPE, (void*)dwTransactionId);
            bNoClientWantsThisMsg = FALSE;
         }

         if ((AEESMS_TYPE_BROADCAST == mt) && (pMe->dwNotify & NMASK_SMS_BROADCAST))
         {         
            OEMMsgData sMsgData;
            MEMSET(&sMsgData, 0, sizeof(OEMMsgData));
            if (SUCCESS == OEMSMS_GetMsgData(pMe->pOEMSms, dwTransactionId, &sMsgData))
            {
               FARF(SMS, ("Broadcast Service Type %d Notification", sMsgData.dwSrvID));
               ISHELL_Notify( pMe->pIShell, AEECLSID_SMSNOTIFIER, (sMsgData.dwSrvID << 16l)|NMASK_SMS_BROADCAST, (void*)dwTransactionId);
               bNoClientWantsThisMsg = FALSE;
            }
         }
      }

      // For messages not directed to a particular BREW applet, 
      // invoke ITAPI notification to let ITAPI send notifications 
      // to its clients 
      // 
      //                -- exception -- 
      // 
      // loopback messages which are delivered only to 
      // clients who have asked for them specifically (typical use case
      // would be the main messaging app being interested in these)
      if (!clsid && !bIsLoopback)
      {
         FARF(SMS, ("Notification sent to TAPI" ));
         AEETAPI_NotifyOnMtSMS(dwTransactionId, ts);
      }

      if (bNoClientWantsThisMsg == TRUE)
      {
         FARF(SMS, ("==> No client wants this message .... freeing message!!!!" ));
         // If no client is interested in this message, which means we haven't sent
         // transaction ID to any client, set message free
         OEMSMS_FreeMsg(pMe->pOEMSms, dwTransactionId);
      }
   }

   // Re-register callback
   OEMSMS_OnMTMessage( pMe->pOEMSms, &gpSMSNotifier->cbOnMtSMS );
}

/*===========================================================================

Function: ISMSNotifier_New

Description:
   ISMSNotifier constructor
 
===========================================================================*/
int ISMSNotifier_New(IShell *pIShell, AEECLSID uCls, void **ppif)
{   
   int nErr = SUCCESS;

   FARF(SMS, ("==>  ISMSNotifier_New"));

   if( !gpSMSNotifier ) 
   {
      OEMSMSMgr *pif;
      
      if( SUCCESS == (nErr = OEMSMS_New( &pif )) ) 
      {
         gpSMSNotifier = (ISMSNotifier *)sys_malloc( sizeof(ISMSNotifier) );
         
         if( gpSMSNotifier ) 
         {
            gpSMSNotifier->pvt      = &gvtISMSNotifier;
            gpSMSNotifier->pOEMSms  = pif;
            
            gpSMSNotifier->uRef     = 1;
            
            gpSMSNotifier->pIShell  = pIShell;
            ISHELL_AddRef(pIShell);

            CALLBACK_Init( &gpSMSNotifier->cbOnMtSMS, (PFNNOTIFY)CSMSNotifier_cbOnMtSMS, gpSMSNotifier );
            OEMSMS_OnMTMessage( pif, &gpSMSNotifier->cbOnMtSMS );
      
            *ppif = gpSMSNotifier;
         }
         else 
         {                       
            OEMSMS_Release( pif );
            nErr = ENOMEMORY;
         }
      }
   }
   else   
   {
      CSMSNotifier_AddRef( (ISMSNotifier *)gpSMSNotifier );
      *ppif = gpSMSNotifier;
   }

   return nErr;
}

/*===========================================================================

Function: CSMSNotifier_AddRef

Description:
   Increases reference count
 
===========================================================================*/
static uint32 CSMSNotifier_AddRef(ISMSNotifier *pme)
{
   FARF(SMS, ("==>  CSMSNotifier_AddRef"));
   return ++pme->uRef;
}

/*===========================================================================

Function: CSMSNotifier_Release

Description: 
   Decrements reference count. Frees object when reference count reaches 0. 
 
===========================================================================*/
static uint32 CSMSNotifier_Release(ISMSNotifier *pme)
{
   FARF(SMS, ("==>  CSMSNotifier_Release"));

   if (--pme->uRef) {
      return pme->uRef;
   }

   gpSMSNotifier = 0;
   RELEASEIF(pme->pIShell);
   OEMSMS_Release(pme->pOEMSms);
   CALLBACK_Cancel( &pme->cbOnMtSMS );   
   sys_free(pme);

   return 0;
}

/*===========================================================================

Function: CSMSNotifier_SetMask

Description: 
   Sets notification mask. 
 
===========================================================================*/
static void CSMSNotifier_SetMask(ISMSNotifier *pme, uint32 *pdwMasks)
{
   FARF(SMS, ("==>  CSMSNotifier_SetMask"));

   while (*pdwMasks)
   {
      pme->dwNotify |= GET_NOTIFIER_MASK(*pdwMasks);
      pdwMasks++;
   }
}

/*===========================================================================

Function: ISMSStorage_New

Description:
   ISMSStorage2 constructor
 
===========================================================================*/
int ISMSStorage_New(IShell *pIShell, AEECLSID uCls, void **ppif)
{
   int nErr = SUCCESS;
   ISMSStorage2 * pme = NULL;

   FARF(SMS, ("==>  ISMSStorage_New"));

   // Ensure that application has AEECLSID_SMSMAINCLIENT privilege
   if (ISHELL_CheckPrivLevel(pIShell, AEECLSID_SMSMAINCLIENT, TRUE) == FALSE)
   {
      return EPRIVLEVEL;
   }
      
   if (!gpSMSStorage)
   {
      if (NULL == (gpSMSStorage = sys_malloc(sizeof(CSMSStorageStaticData))))
         return ENOMEMORY;
      gpSMSStorage->dwTotalEntries = DEFAULT_EVENT_CACHE_ENTRIES;
   }
   
   pme = (ISMSStorage2 *)MALLOC(sizeof(ISMSStorage2));

   *ppif = pme;

   if (pme) 
   {
      pme->pvt      = &gvtISMSStorage;

      pme->uRef     = 1;
      pme->pIShell  = pIShell;
      ISHELL_AddRef(pIShell);

      // Initialize the model interface...
      pme->model.pMe = (IBase*)pme;
      pme->model.pvt = &gSMSModelMethods;

      // Guard against application exiting without calling ISMSSTORAGE_Release()
      CALLBACK_Init(&pme->cbSysObj, CSMSStorage_Dtor, pme);
      AEE_LinkSysObject(&pme->cbSysObj);

      CALLBACK_Init(&pme->cbOnStorageUpdate, CSMSStorage_cbOnStorageUpdate, (void*)pme);
      CSMSStorage_OnStorageUpdate(&pme->cbOnStorageUpdate, &pme->sUpdateData);

      if (!gpSMSStorage->dwCount++) {
         CALLBACK_Init(&gpSMSStorage->cbFromOEMLayerOnStorageUpdate, CSMSStorage_cbFromOEMLayerOnStorageUpdate, &gpSMSStorage);
         OEMSMS_OnStorageUpdate(CSMSStorage_NotifyFromOEMLayerOnStorageUpdate);
      }

      pme->pac = AEE_GetAppContext();
   }
   else 
   {
      nErr = ENOMEMORY;
   }

   return nErr;
}

/*===========================================================================

Function: CSMSStorage_AddRef

Description:
   Increases reference count
 
===========================================================================*/
static uint32 CSMSStorage_AddRef(ISMSStorage2 *pme)
{
   FARF(SMS, ("==>  CSMSStorage_AddRef"));
   return ++pme->uRef;
}

/*===========================================================================

Function: CSMSStorage_Release

Description: 
   Decrements reference count. Frees object when reference count reaches 0. 
 
===========================================================================*/
static uint32 CSMSStorage_Release(ISMSStorage2 *pme)
{
   FARF(SMS, ("==>  CSMSStorage_Release"));

   if (--pme->uRef) {
      return pme->uRef;
   }

   CSMSStorage_Dtor(pme);
   RELEASEIF(pme->pIShell);
   FREEIF(pme->pdwIndex);
   FREEIF(pme);

   return 0;
}

/*===========================================================================

Function: CSMSStorage_QueryInterface

Description:
   Query Interface
 
===========================================================================*/
static int CSMSStorage_QueryInterface(ISMSStorage2 *pme, AEECLSID id, void **ppo)
{
   IBase * po = NULL;

   FARF(SMS, ("==>  CSMSStorage_QueryInterface"));

   switch (id) 
   {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_SMSSTORAGE:
      case AEEIID_SMSSTORAGE2:
         po = (IBase*)pme;
         break;

      case AEEIID_MODEL:
         po = (IBase*)&pme->model;
         break;

      default:
         break;
   }
 
   *ppo = po;

   if (po) {
      IBASE_AddRef(po);
      return SUCCESS;
   }
      
   return ECLASSNOTSUPPORT;
}

/*===========================================================================

Function: CSMSStorage_GetStorageStatus

Description:
   Returns storage status of current storage type.
 
===========================================================================*/
static void CSMSStorage_GetStorageStatus(ISMSStorage2 * pme, AEESMSStorageType st, AEESMSTag mt, AEECallback *pcb, AEESMSStorageStatus * pss, uint32 *pnRv)
{
   int nErr;
   SMSAppCbData * pAppCbData = NULL;

   FARF(SMS, ("==>  CSMSStorage_GetStorageStatus"));

   if (((nErr = OEMSMS_IsStorageInitialized(st) ? SUCCESS : EFAILED) == SUCCESS) &&
      ((nErr = LinkAppCb(pcb, &pAppCbData)) == SUCCESS))
   {
      OEMSMS_GetStorageStatus(st, mt, pss, pnRv, &pAppCbData->cb);
   }
   else
   {
      *pnRv = nErr;

      if (pAppCbData)
         ISHELL_Resume(pme->pIShell, pAppCbData->pcb);
      else
         ISHELL_Resume(pme->pIShell, pcb);
   }
}

/*===========================================================================

Function: CSMSStorage_StoreMsg

Description:
   Stores message in current storage type.
 
===========================================================================*/
static void CSMSStorage_StoreMsg(ISMSStorage2 * pme, AEESMSStorageType st, ISMSMsg * pi, AEECallback *pcb, uint32 * pnIndex, uint32 *pnRv)
{   
   OEMMsgData * pMsgData = NULL;
   OEMMsgPayload * pMsgPayload = NULL;
   SMSAppStoreMsgCbData * pAppStoreMsgCbData = NULL;
   int nErr = SUCCESS;

   FARF(SMS, ("==>  CSMSStorage_StoreMsg"));

   if (!OEMSMS_IsStorageInitialized(st))
   {
      nErr = EFAILED;
      goto Done;
   }

   if ((nErr = ISMSMsgToOEMMsg(pi, &pMsgData, &pMsgPayload)) != SUCCESS)
      goto Done;

   if ((nErr = LinkAppStoreMsgCb(pcb, &pAppStoreMsgCbData)) == SUCCESS)
   {
      pAppStoreMsgCbData->pnIndex = pnIndex;
      pAppStoreMsgCbData->pnRv = pnRv;
      pAppStoreMsgCbData->st = st;
      pAppStoreMsgCbData->mt = pMsgData->dwTag;
      OEMSMS_StoreMsg(st, pMsgData, pMsgPayload, pnIndex, pnRv, &pAppStoreMsgCbData->cb);
   }

Done:
   FREEIF(pMsgData);
   FREEIF(pMsgPayload);

   // Handle Error Here. No Error means that we are able to call OEMSMS_SendMessage
   if (nErr)
   {
      *pnRv = nErr;

      if (pAppStoreMsgCbData)
         ISHELL_Resume(pme->pIShell, pAppStoreMsgCbData->pcb);
      else
         ISHELL_Resume(pme->pIShell, pcb);
   }
}

/*===========================================================================

Function: CSMSStorage_ReadMsg

Description:
   Reads message from current storage type.
 
===========================================================================*/
static void CSMSStorage_ReadMsg(ISMSStorage2 * pme, AEESMSStorageType st, uint32 nIndex, AEECallback *pcb, ISMSMsg ** ppi, uint32 * pnErr)
{  
   int nErr;
   SMSAppReadMsgCbData * pAppReadMsgCbData = NULL;

   FARF(SMS, ("==>  CSMSStorage_ReadMsg")); 

   if (((nErr = OEMSMS_IsStorageInitialized(st) ? SUCCESS : EFAILED) == SUCCESS) &&
       ((nErr = LinkAppReadMsgCb(pcb, &pAppReadMsgCbData)) == SUCCESS))
   {
      pAppReadMsgCbData->pnRv = pnErr;
      pAppReadMsgCbData->ppISMSMsg = ppi;
      OEMSMS_ReadMsg(st, nIndex, pAppReadMsgCbData->pMsgData, pAppReadMsgCbData->pMsgPayload, pnErr, &pAppReadMsgCbData->cb);
   }
   else
   {
      *pnErr = nErr;

      if (pAppReadMsgCbData)
         ISHELL_Resume(pme->pIShell, pAppReadMsgCbData->pcb);
      else
         ISHELL_Resume(pme->pIShell, pcb);
   }
}

/*===========================================================================

Function: CSMSStorage_Delete

Description:
   Deletes specified message from current storage type.
 
===========================================================================*/
static void CSMSStorage_Delete(ISMSStorage2 * pme, AEESMSStorageType st, AEESMSTag mt, uint32 nIndex, AEECallback *pcb, uint32 * pnErr)
{
   int nErr;
   SMSAppDeleteMsgCbData * pAppDeleteMsgCbData = NULL;

   FARF(SMS, ("==>  CSMSStorage_Delete"));

   if (((nErr = OEMSMS_IsStorageInitialized(st) ? SUCCESS : EFAILED) == SUCCESS) &&
       ((nErr = LinkAppDeleteMsgCb(pcb, &pAppDeleteMsgCbData)) == SUCCESS))
   {
      pAppDeleteMsgCbData->st = st;
      pAppDeleteMsgCbData->mt = mt;
      pAppDeleteMsgCbData->nIndex = nIndex;
      pAppDeleteMsgCbData->pnRv = pnErr;
      OEMSMS_RemoveMsg(st, mt, nIndex, pnErr, &pAppDeleteMsgCbData->cb);
   }
   else
   {
      *pnErr = nErr;

      if (pAppDeleteMsgCbData)
         ISHELL_Resume(pme->pIShell, pAppDeleteMsgCbData->pcb);
      else
         ISHELL_Resume(pme->pIShell, pcb);
   }
}

/*===========================================================================

Function: CSMSStorage_EnumMsgInit

Description:
   Initializes current storage type for enumeration according to the specified
   parameters.
 
===========================================================================*/
static void CSMSStorage_EnumMsgInit(ISMSStorage2 * pme, AEESMSStorageType st, AEESMSTag mt, AEECallback *pcb, uint32 * pnErr)
{   
   int nErr = SUCCESS;
   uint32 dwSize = 0;

   FARF(SMS, ("==>  CSMSStorage_EnumMsgInit"));

   if (!pcb || !pnErr)
      return;

   // Reset if its already processing an enumeration
   if (pme->bEnumInit)
   {
      FREEIF(pme->pdwIndex);
      pme->pdwIndex = NULL;
      pme->bEnumInit = FALSE;
   }

   if (OEMSMS_IsStorageInitialized(st) &&
       (SUCCESS == OEMSMS_GetEnumMsgIDs(st, mt, NULL, &dwSize)) &&
       ((0 == dwSize) || 
        ((NULL != (pme->pdwIndex = MALLOC(dwSize))) &&
         (SUCCESS == OEMSMS_GetEnumMsgIDs(st, mt, pme->pdwIndex, &dwSize)))))
   {
      pme->mt = mt;
      pme->st = st;
      pme->dwOffset = 0;
      pme->dwCount = dwSize/sizeof(uint32);
      pme->bEnumInit = TRUE;
   }
   else
   {
      nErr = EFAILED;
      FREEIF(pme->pdwIndex);
      pme->pdwIndex = NULL;
   }
   
   *pnErr = nErr;
   ISHELL_Resume(pme->pIShell, pcb);
}

/*===========================================================================

Function: CSMSStorage_EnumNextMsg

Description:
   Find next message in enumeration
 
===========================================================================*/
static void CSMSStorage_EnumNextMsg(ISMSStorage2 * pme, AEESMSStorageType st, AEECallback *pcb, uint32 * pnIndex, ISMSMsg ** ppi, uint32 * pnErr)
{
   SMSAppEnumMsgCbData * pAppEnumMsgCbData = NULL;
   int nErr = SUCCESS;
   
   FARF(SMS, ("==>  CSMSStorage_EnumNextMsg"));

   // Null Check
   if (!pcb || !pnErr)
      return;

   // We need atleast one of ppi and pnIndex. Also make sure that storage type
   // specified is same as the one specified in CSMSStorage_EnumMsgInit
   if ((!ppi && !pnIndex) || (pme->st != st))
   {
      nErr = EBADPARM;
      goto Done;
   }

   // Check if ISMSSTORAGE_EnumMsgInit() isn't called
   if (!pme->bEnumInit)
   {
      nErr = EFAILED;
      goto Done;
   }

   // Check if enumeration is complete
   if (pme->dwOffset >= pme->dwCount)
   {
      if (ppi)
         *ppi = NULL;

      if (pnIndex)
         *pnIndex = 0xffffffff;

      FREEIF(pme->pdwIndex);
      pme->pdwIndex = NULL;
      pme->bEnumInit = FALSE;

      nErr = SUCCESS;
      goto Done;
   }
   

   if (((nErr = OEMSMS_IsStorageInitialized(st) ? SUCCESS : EFAILED) == SUCCESS) &&
      ((nErr = LinkAppEnumMsgCb(pcb, &pAppEnumMsgCbData)) == SUCCESS))
   {
      pAppEnumMsgCbData->pnRv = pnErr;
      pAppEnumMsgCbData->ppISMSMsg = ppi;
      pAppEnumMsgCbData->pnIndex = pnIndex;
      pAppEnumMsgCbData->pISMSStorage = pme;
      OEMSMS_ReadMsg(st, pme->pdwIndex[pme->dwOffset], pAppEnumMsgCbData->pMsgData, 
                     pAppEnumMsgCbData->pMsgPayload, pnErr, &pAppEnumMsgCbData->cb);
      return;
   }
   else
   {
      goto Done;
   }

Done:
   *pnErr = nErr;

   if (pAppEnumMsgCbData)
      ISHELL_Resume(pme->pIShell, pAppEnumMsgCbData->pcb);
   else
      ISHELL_Resume(pme->pIShell, pcb);
}

/*===========================================================================

Function: CSMSStorage_UpdateMsg

Description:
   Update the message in current storage type.
 
===========================================================================*/
static void CSMSStorage_UpdateMsg(ISMSStorage2 * pme, AEESMSStorageType st, uint32 nIndex, ISMSMsg * pi, AEECallback *pcb, uint32 *pnErr)
{   
   OEMMsgData * pMsgData = NULL;
   OEMMsgPayload * pMsgPayload = NULL;
   SMSAppStoreMsgCbData * pAppStoreMsgCbData = NULL;
   int nErr = SUCCESS;

   FARF(SMS, ("==>  CSMSStorage_UpdateMsg"));

   if (!OEMSMS_IsStorageInitialized(st))
   {
      nErr = EFAILED;
      goto Done;
   }

   if ((nErr = ISMSMsgToOEMMsg(pi, &pMsgData, &pMsgPayload)) != SUCCESS)
      goto Done;

   if ((nErr = LinkAppStoreMsgCb(pcb, &pAppStoreMsgCbData)) == SUCCESS)
   {
      pAppStoreMsgCbData->nIndex = nIndex;
      pAppStoreMsgCbData->pnRv = pnErr;
      pAppStoreMsgCbData->st = st;
      pAppStoreMsgCbData->mt = pMsgData->dwTag;
      OEMSMS_UpdateMsg(st, nIndex, pMsgData, pMsgPayload, pnErr, &pAppStoreMsgCbData->cb);
   }

Done:
   FREEIF(pMsgData);
   FREEIF(pMsgPayload);

   // Handle Error Here. No Error means that we are able to call OEMSMS_SendMessage
   if (nErr)
   {
      *pnErr = nErr;

      if (pAppStoreMsgCbData)
         ISHELL_Resume(pme->pIShell, pAppStoreMsgCbData->pcb);
      else
         ISHELL_Resume(pme->pIShell, pcb);
   }
}

/*===========================================================================

Function: CSMSStorage_AutoStoreMsg

Description:
   Enables or disables auto storage for the specified message class type.
 
===========================================================================*/
static void CSMSStorage_AutoStoreMsg(ISMSStorage2 * pme, AEESMSClass mc, AEESMSStorageType st, AEECallback *pcb, uint32 *pnErr)
{
   SMSAppCbData * pAppCbData = NULL;
   int nErr = SUCCESS;
      
   FARF(SMS, ("==>  CSMSStorage_AutoStoreMsg"));

   if ((nErr = LinkAppCb(pcb, &pAppCbData)) == SUCCESS)
   {
      // Pass through to OEM
      OEMSMS_AutoStoreMsg(mc, st, pnErr, &pAppCbData->cb);
   }

   if (nErr != SUCCESS)
   {
      *pnErr = nErr;
      if (pAppCbData)
      {
         ISHELL_Resume(pme->pIShell, pAppCbData->pcb);
      }
      else
      {
         ISHELL_Resume(pme->pIShell, pcb);
      }
   }
}

/*======================================================================= 
Invoked when OEM layer wants to notify an event
=======================================================================*/
static void CSMSStorage_cbOnStorageUpdate(ISMSStorage2 *pMe)
{
   ModelEvent me;

   FARF(SMS, ("==>  CSMSStorage_cbOnStorageUpdate"));

   MEMSET(&me, 0, sizeof(me));

   me.evCode = pMe->sUpdateData.nEvent;
   me.pModel = &pMe->model;
   me.dwParam = (uint32)&pMe->sUpdateData.sEventData;

   IModelClass_NotifyApp(&pMe->model, &me, pMe->pac);

   CSMSStorage_OnStorageUpdate(&pMe->cbOnStorageUpdate, &pMe->sUpdateData);
}

/*===========================================================================

Function: ISMSBCSrvOpts_New

Description:
   ISMSBCSrvOpts constructor
 
===========================================================================*/
int ISMSBCSrvOpts_New(IShell *pIShell, AEECLSID uCls, void **ppif)
{
   // uCls is intentionally unused
   int nReturn = ENOMEMORY;
   SMSMsgOpt awoz[1];
   ISMSBCSrvOpts *pme;

   // Ensure that application has AEECLSID_SMSMAINCLIENT privilege
   if (ISHELL_CheckPrivLevel(pIShell, AEECLSID_SMSMAINCLIENT, TRUE) == FALSE)
   {
      return EPRIVLEVEL;
   }

   // Make empty option list
   awoz[0].nId  = WEBOPT_END;

   // Get our memory including a WebOpt manager
   pme = MALLOCREC_EX(ISMSBCSrvOpts, WebOptMgr_GetSize(awoz));

   *ppif = pme;

   if (pme) 
   {
      // Our vtable
      pme->pvt = &gvtISMSBCSrvOpts;

      pme->pWom = (WebOptMgr *)(pme+1);
      WebOptMgr_CtorZ(pme->pWom,awoz);

      // The shell 
      pme->uRef = 1;
      pme->pIShell = pIShell;
      ISHELL_AddRef(pIShell);

      nReturn = SUCCESS;
   }
   return nReturn;
}

/*===========================================================================

Function: CSMSBCSrvOpts_AddRef

Description:
   Increases reference count
 
===========================================================================*/
static uint32 CSMSBCSrvOpts_AddRef(ISMSBCSrvOpts *pme)
{
   return ++pme->uRef;
}

/*===========================================================================

Function: CSMSBCSrvOpts_Release

Description:
   Decrements reference count. Frees object when reference count reaches 0. 
 
===========================================================================*/
static uint32 CSMSBCSrvOpts_Release(ISMSBCSrvOpts *pme)
{
   if (--pme->uRef) 
   {
      return pme->uRef;
   }

   WebOptMgr_Dtor(pme->pWom);
   RELEASEIF(pme->pIShell);
   FREEIF(pme);

   return 0;
}

/*===========================================================================

Function: CSMSBCSrvOpts_QueryInterface

Description:
   Query Interface
 
===========================================================================*/
static int CSMSBCSrvOpts_QueryInterface(ISMSBCSrvOpts *pme, AEECLSID id, void **ppo)
{
   if ((id == AEECLSID_QUERYINTERFACE) || 
       (id == AEECLSID_SMSMSG)         ||
       (id == AEECLSID_WEBOPTS)) 
   {
      *ppo = pme;
      IQI_AddRef(*ppo);
      return SUCCESS;
   } 
   else 
   {
      return ECLASSNOTSUPPORT;
   }
}

/*===========================================================================

Function: CSMSBCSrvOpts_AddOpt

Description:
   Uses base class (IWebOpts) implementation
 
===========================================================================*/
static int CSMSBCSrvOpts_AddOpt(ISMSBCSrvOpts *pme, SMSBCSrvOpts *awozList)
{
   return WebOptMgr_AddOptV(pme->pWom, awozList);
}

/*===========================================================================

Function: CSMSBCSrvOpts_RemoveOpt

Description:
   Uses base class (IWebOpts) implementation
 
===========================================================================*/
static int CSMSBCSrvOpts_RemoveOpt(ISMSBCSrvOpts *pme, int32 nOptId, int32 nIndex)
{
   return WebOptMgr_RemoveOpt(pme->pWom,nOptId, nIndex);
}

/*===========================================================================

Function: CSMSBCSrvOpts_GetOpt

Description:
   Uses base class (IWebOpts) implementation
 
===========================================================================*/
static int CSMSBCSrvOpts_GetOpt(ISMSBCSrvOpts *pme, int32 nOptId, int32 nIndex, SMSBCSrvOpts *pwo)
{
   return WebOptMgr_GetOpt(pme->pWom,nOptId, nIndex, pwo);
}

/*===========================================================================

Function: ISMSBCConfig_New

Description:
   ISMSBCConfig constructor
 
===========================================================================*/
int ISMSBCConfig_New(IShell *pIShell, AEECLSID uCls, void **ppif)
{
   // uCls is intentionally unused
   int nReturn = ENOMEMORY;
   ISMSBCConfig *pme;

   // Ensure that application has AEECLSID_SMSMAINCLIENT privilege
   if (ISHELL_CheckPrivLevel(pIShell, AEECLSID_SMSMAINCLIENT, TRUE) == FALSE)
   {
      return EPRIVLEVEL;
   }

   // Get our memory
   pme = MALLOCREC(ISMSBCConfig);

   *ppif = pme;

   if (pme) 
   {
      // Our vtable
      pme->pvt = &gvtISMSBCConfig;

      // The shell 
      pme->uRef = 1;
      pme->pIShell = pIShell;
      ISHELL_AddRef(pIShell);
         
      // Initialize the model interface...
      pme->model.pMe = (IBase*)pme;
      pme->model.pvt = &gSMSModelMethods;

      // Guard against application exiting without calling ISMSBCCONFIG_Release()
      CALLBACK_Init(&pme->cbSysObj, CSMSBCConfig_Dtor, pme);
      AEE_LinkSysObject(&pme->cbSysObj);

      CALLBACK_Init(&pme->cbOnSMSBCConfigUpdate, CSMSBCConfig_cbOnSMSBCConfigUpdate, (void*)pme);
      CSMSBCConfig_OnSMSBCConfigUpdate(&pme->cbOnSMSBCConfigUpdate, &pme->sUpdateData);

      if (!gSMSBCConfig.dwCount++) {
         CALLBACK_Init(&gSMSBCConfig.cbFromOEMLayerOnSMSBCConfigUpdate, CSMSBCConfig_cbFromOEMLayerOnSMSBCConfigUpdate, &gSMSBCConfig);
         OEMSMSBCConfig_OnSMSBCConfigUpdate(&gSMSBCConfig.sUpdateData, &gSMSBCConfig.cbFromOEMLayerOnSMSBCConfigUpdate);
      }

      pme->pac = AEE_GetAppContext();

      nReturn = SUCCESS;
   }
   return nReturn;
}

/*===========================================================================

Function: CSMSBCConfig_AddRef

Description:
   Increases reference count
 
===========================================================================*/
static uint32 CSMSBCConfig_AddRef(ISMSBCConfig *pme)
{
   return ++pme->uRef;
}

/*===========================================================================

Function: CSMSBCConfig_Release

Description:
   Decrements reference count. Frees object when reference count reaches 0. 
 
===========================================================================*/
static uint32 CSMSBCConfig_Release(ISMSBCConfig *pme)
{
   if (--pme->uRef) 
   {
      return pme->uRef;
   }
   
   CSMSBCConfig_Dtor(pme);
   RELEASEIF(pme->pIShell);
   FREEIF(pme);

   return 0;
}

/*===========================================================================

Function: CSMSBCConfig_QueryInterface

Description:
   Query Interface
 
===========================================================================*/
static int CSMSBCConfig_QueryInterface(ISMSBCConfig *pme, AEECLSID id, void **ppo)
{
   IBase * po = NULL;

   FARF(SMS, ("==>  CSMSBCConfig_QueryInterface"));

   switch (id) 
   {
      case AEECLSID_QUERYINTERFACE:
      case AEECLSID_SMSBCCONFIG:
         po = (IBase*)pme;
         break;

      case AEEIID_MODEL:
         po = (IBase*)&pme->model;
         break;

      default:
         break;
   }
 
   *ppo = po;

   if (po) {
      IBASE_AddRef(po);
      return SUCCESS;
   }
      
   return ECLASSNOTSUPPORT;
}

/*===========================================================================

Function: CSMSBCConfig_GetBCConfig

Description:
   Returns carrier's broadcast configuration
 
===========================================================================*/
static void CSMSBCConfig_GetBCConfig(ISMSBCConfig * pme, AEESMSMode dwMode, AEECallback *pcb, AEESMSBCConfig * pConfig, uint32 * pnErr)
{
   SMSAppCbData * pAppCbData = NULL;
   int nErr = EFAILED;
      
   FARF(SMS, ("==>  CSMSBCConfig_GetBCConfig"));

   if ((nErr = LinkAppCb(pcb, &pAppCbData)) == SUCCESS)
   {
      // Pass through to OEM
      OEMSMSBCConfig_GetBCConfig(dwMode, &pAppCbData->cb, pConfig, pnErr);
   }
      
   if (nErr != SUCCESS)
   {
      *pnErr = nErr;
      if (pAppCbData)
      {
         ISHELL_Resume(pme->pIShell, pAppCbData->pcb);
      }
      else
      {
         ISHELL_Resume(pme->pIShell, pcb);
      }
   }
}

/*===========================================================================

Function: CSMSBCConfig_GetBCPref

Description:
   Returns user's broadcast preference
 
===========================================================================*/
static void CSMSBCConfig_GetBCPref(ISMSBCConfig * pme, AEESMSMode dwMode, AEECallback *pcb, AEESMSBCPref * pPref, uint32 * pnErr)
{
   SMSAppCbData * pAppCbData = NULL;
   int nErr = EFAILED;
      
   FARF(SMS, ("==>  CSMSBCConfig_GetBCPref"));

   if ((nErr = LinkAppCb(pcb, &pAppCbData)) == SUCCESS)
   {
      // Pass through to OEM
      OEMSMSBCConfig_GetBCPref(dwMode, &pAppCbData->cb, pPref, pnErr);
   }

   if (nErr != SUCCESS)
   {
      *pnErr = nErr;
      if (pAppCbData)
      {
         ISHELL_Resume(pme->pIShell, pAppCbData->pcb);
      }
      else
      {
         ISHELL_Resume(pme->pIShell, pcb);
      }
   }
}

/*===========================================================================

Function: CSMSBCConfig_SetBCPref

Description:
   Sets user's broadcast preference
 
===========================================================================*/
static void CSMSBCConfig_SetBCPref(ISMSBCConfig * pme, AEESMSMode dwMode, AEESMSBCPref nPref, AEECallback *pcb, uint32 * pnErr)
{
   SMSAppCbData * pAppCbData = NULL;
   int nErr = EFAILED;
      
   FARF(SMS, ("==>  CSMSBCConfig_SetBCPref"));

   if ((nErr = LinkAppCb(pcb, &pAppCbData)) == SUCCESS)
   {
      // Pass through to OEM 
      OEMSMSBCConfig_SetBCPref(dwMode, nPref, &pAppCbData->cb, pnErr);
   }

   if (nErr != SUCCESS)
   {
      *pnErr = nErr;
      if (pAppCbData)
      {
         ISHELL_Resume(pme->pIShell, pAppCbData->pcb);
      }
      else
      {
         ISHELL_Resume(pme->pIShell, pcb);
      }
   }
}

/*===========================================================================

Function: CSMSBCConfig_GetServiceIDs

Description:
   Returns service IDs currently configured in the table.
 
===========================================================================*/
static void CSMSBCConfig_GetServiceIDs(ISMSBCConfig * pme, AEESMSMode dwMode, AEECallback *pcb, uint32 * pnCount, AEESMSBCSrvID ** ppsid, uint32 * pnErr)
{
   SMSAppGetSrvIDsCbData * pSMSAppGetSrvIDsCbData = NULL;
   int nErr = EFAILED;
      
   FARF(SMS, ("==>  CSMSBCConfig_GetServiceIDs"));

   if ((nErr = LinkAppGetSrvIDsCb(pcb, &pSMSAppGetSrvIDsCbData)) == SUCCESS)
   {
      pSMSAppGetSrvIDsCbData->pnCount = pnCount;
      pSMSAppGetSrvIDsCbData->ppsid   = ppsid;
      pSMSAppGetSrvIDsCbData->pnErr   = pnErr;
      
      // Pass through to OEM 
      OEMSMSBCConfig_GetNumberOfServiceIDs(dwMode, &pSMSAppGetSrvIDsCbData->cb, pSMSAppGetSrvIDsCbData->pnCount, pSMSAppGetSrvIDsCbData->pnErr);
   }
   
   if (nErr != SUCCESS)
   {
      *pnErr = nErr;
      if (pSMSAppGetSrvIDsCbData)
      {
         ISHELL_Resume(pme->pIShell, pSMSAppGetSrvIDsCbData->pcb);
      }
      else
      {
         ISHELL_Resume(pme->pIShell, pcb);
      }
   }
}

/*===========================================================================

Function: CSMSBCConfig_GetServiceOpts

Description:
   Returns service options corresponding to the specified ID.
 
===========================================================================*/
static void CSMSBCConfig_GetServiceOpts(ISMSBCConfig * pme, AEESMSMode dwMode, AEESMSBCSrvID sSrvID, AEECallback *pcb, ISMSBCSrvOpts ** ppi, uint32 * pnErr)
{
   SMSAppGetSrvOptsCbData * pSMSAppGetSrvOptsCbData = NULL;
   int nErr = EFAILED;
      
   FARF(SMS, ("==>  CSMSBCConfig_GetServiceOpts"));

   if ((nErr = LinkAppGetSrvOptsCb(pcb, &pSMSAppGetSrvOptsCbData)) == SUCCESS)
   {
      pSMSAppGetSrvOptsCbData->ppi     = ppi;
      pSMSAppGetSrvOptsCbData->pnErr   = pnErr;

      // Pass through to OEM
      OEMSMSBCConfig_GetServiceOpts(dwMode, &sSrvID, &pSMSAppGetSrvOptsCbData->cb, &pSMSAppGetSrvOptsCbData->sSMSBcSrvInfo, pSMSAppGetSrvOptsCbData->pnErr);
   }
   
   if (nErr != SUCCESS)
   {
      *pnErr = nErr;
      if (pSMSAppGetSrvOptsCbData)
      {
         ISHELL_Resume(pme->pIShell, pSMSAppGetSrvOptsCbData->pcb);
      }
      else
      {
         ISHELL_Resume(pme->pIShell, pcb);
      }
   }
}

/*===========================================================================

Function: CSMSBCConfig_SetServiceOpts

Description:
   Sets service options corresponding to the specified ID.
 
===========================================================================*/
static void CSMSBCConfig_SetServiceOpts(ISMSBCConfig * pme, AEESMSMode dwMode, ISMSBCSrvOpts * pi, AEECallback *pcb, uint32 * pnErr)
{
   SMSAppCbData * pAppCbData = NULL;
   OEMSMSBCSrvInfo sSrvInfo;
   int nErr = EFAILED;
      
   FARF(SMS, ("==>  CSMSBCConfig_SetServiceOpts"));

   MEMSET(&sSrvInfo, 0, sizeof(OEMSMSBCSrvInfo));

   if ((nErr = LinkAppCb(pcb, &pAppCbData)) == SUCCESS)
   {
      if (UnpackSMSBCSrvOpts(pi, &sSrvInfo))
      {
         // Pass through to OEM 
         OEMSMSBCConfig_SetServiceOpts(dwMode, &sSrvInfo, &pAppCbData->cb, pnErr);
      }
      else
      {
         nErr = EBADPARM;
      }
   }

   if (nErr != SUCCESS)
   {
      *pnErr = nErr;
      if (pAppCbData)
      {
         ISHELL_Resume(pme->pIShell, pAppCbData->pcb);
      }
      else
      {
         ISHELL_Resume(pme->pIShell, pcb);
      }
   }
}

/*===========================================================================

Function: CSMSBCConfig_GetAllServiceOpts

Description:
   Returns service options corresponding to all of the service IDs.
 
===========================================================================*/
static void CSMSBCConfig_GetAllServiceOpts(ISMSBCConfig * pme, AEESMSMode dwMode, AEECallback *pcb, uint32 * pnCount, ISMSBCSrvOpts *** pppi, uint32 * pnErr)
{
   SMSAppGetAllSrvOptsCbData * pSMSAppGetAllSrvOptsCbData = NULL;
   int nErr = EFAILED;
      
   FARF(SMS, ("==>  CSMSBCConfig_GetAllServiceOpts"));

   if ((nErr = LinkAppGetAllSrvOptsCb(pcb, &pSMSAppGetAllSrvOptsCbData)) == SUCCESS)
   {
      pSMSAppGetAllSrvOptsCbData->pnCount = pnCount;
      pSMSAppGetAllSrvOptsCbData->pppi     = pppi;
      pSMSAppGetAllSrvOptsCbData->pnErr   = pnErr;

      // Pass through to OEM 
      OEMSMSBCConfig_GetNumberOfServiceOpts(dwMode, &pSMSAppGetAllSrvOptsCbData->cb, pSMSAppGetAllSrvOptsCbData->pnCount, pSMSAppGetAllSrvOptsCbData->pnErr);
   }
   
   if (nErr != SUCCESS)
   {
      *pnErr = nErr;
      if (pSMSAppGetAllSrvOptsCbData)
      {
         ISHELL_Resume(pme->pIShell, pSMSAppGetAllSrvOptsCbData->pcb);
      }
      else
      {
         ISHELL_Resume(pme->pIShell, pcb);
      }
   }
}

/*===========================================================================

Function: CSMSBCConfig_DeleteService

Description:
   Removes a service from the device service table.
 
===========================================================================*/
static void CSMSBCConfig_DeleteService(ISMSBCConfig * pme, AEESMSMode dwMode, AEESMSBCSrvID sSrvID, AEECallback *pcb, uint32 * pnErr)
{
   SMSAppCbData * pAppCbData = NULL;
   int nErr = EFAILED;
      
   FARF(SMS, ("==>  CSMSBCConfig_DeleteService"));

   if ((nErr = LinkAppCb(pcb, &pAppCbData)) == SUCCESS)
   {
      // Pass through to OEM 
      OEMSMSBCConfig_DeleteService(dwMode, &sSrvID, &pAppCbData->cb, pnErr);
   }

   if (nErr != SUCCESS)
   {
      *pnErr = nErr;
      if (pAppCbData)
      {
         ISHELL_Resume(pme->pIShell, pAppCbData->pcb);
      }
      else
      {
         ISHELL_Resume(pme->pIShell, pcb);
      }
   }
}

/*======================================================================= 
Invoked when OEM layer wants to notify an event
=======================================================================*/
static void CSMSBCConfig_cbOnSMSBCConfigUpdate(ISMSBCConfig *pMe)
{
   ModelEvent me;
   uint32 * pData = NULL;
   
   FARF(SMS, ("==>  CSMSBCConfig_cbOnSMSBCConfigUpdate"));

   MEMSET(&me, 0, sizeof(me));

   me.evCode = pMe->sUpdateData.nEvent;
   me.pModel = &pMe->model;
   pData = (uint32*)pMe->sUpdateData.aEventData;

   switch (pMe->sUpdateData.nEvent)
   {
      case EVT_MDL_SMSBCCONFIG_CONFIG:
      case EVT_MDL_SMSBCCONFIG_PREF:
      case EVT_MDL_SMSBCCONFIG_DELETED:
      case EVT_MDL_SMSBCCONFIG_ADDED:
      case EVT_MDL_SMSBCCONFIG_SELECT_ALL_SRVS:
      case EVT_MDL_SMSBCCONFIG_SET_PRIORITY_ALL_SRVS:
      case EVT_MDL_SMSBCCONFIG_DELETE_ALL_SRVS:
         me.dwParam = *pData;
         IModelClass_NotifyApp(&pMe->model, &me, pMe->pac);
         break;

      case EVT_MDL_SMSBCCONFIG_UPDATED:
         me.dwParam = (uint32)pData;
         IModelClass_NotifyApp(&pMe->model, &me, pMe->pac);
         break;

      default:
         break;
   }

   CSMSBCConfig_OnSMSBCConfigUpdate(&pMe->cbOnSMSBCConfigUpdate, &pMe->sUpdateData);
}

/*======================================================================= 
EXPORTED by external reference.
=======================================================================*/
boolean AEE_IsBREWDirectedSMS( char *pMsg )
{
   AEECLSID cls = 0;
   char * pszBREWHandler = NULL;
   byte delimiter = 0;

   if (!pMsg) {
      return FALSE;
   }

   ParseBrewMsg(pMsg, STRLEN(pMsg), &cls, &pszBREWHandler, NULL, &delimiter);

   // Is it a brew directed / handler directed message?
   // Will be one if it has the delimiter and a Class ID or Handler
   if (delimiter && (cls || pszBREWHandler))
   {  
      boolean bIsBD = TRUE;
      
      // If the handler is not registered with BREW, 
      // the message is not considered a BREW directed message. 
      // This allows other clients to handle it. 
      if (pszBREWHandler != NULL) 
      {
         boolean bVerifyHandler; 
         
         // If we fail to get the config item, use the default value
         if (SUCCESS != OEM_GetConfig(CFGI_SMS_VERIFY_BREW_HANDLER, &bVerifyHandler, sizeof(boolean))) 
         {
            bVerifyHandler = TRUE; // TRUE is the default config item value
         }

         if (bVerifyHandler && (0 == cls)) 
         {
            bIsBD =  FALSE;
         } 
      }

      //Restore pMsg as ParseBrewMsg will split pMsg
      *(pMsg + STRLEN(pMsg)) = delimiter;

      return  bIsBD;
   }

   return FALSE;
}

/*===========================================================================
Called to get decoded text string
===========================================================================*/
byte * AEE_GetDecodedTextString( byte * pText, uint32 nText, int se, int * pae, uint32 * pnBytes)
{
   uint32 nSize;
   byte * pBuffer = NULL;

   FARF(SMS, ("==>  AEE_GetDecodedTextString"));

   if (!pText || !nText)
      return NULL;

   switch (se) 
   {
      case AEESMS_ENC_IA5:
      case AEESMS_ENC_ASCII:
         // For IA5 and ASCII encoding, pText contains packed 7-bit characters.
         // Inflate size as we unpack characters here.
         nSize = ( nText * 8)/7;
         break;
           
      default:
         // For all other encoding types, set nSize same as nText         
         nSize = nText;
         break;
   }

   if ((pBuffer = MALLOC(nSize + sizeof(AECHAR))) == NULL)
      return NULL;

   switch (se) 
   {   
      case AEESMS_ENC_IA5:
      case AEESMS_ENC_ASCII:
         // Unpack the 7 bit data of pText in pBuffer
         DecodePackedString(pText, nSize, (char*)pBuffer);
         break;

      case AEESMS_ENC_UNICODE:        
         // Flip the bytes in each Unicode Character, convert from Network 
         // byte order to to Host Byte Order
         DecodeNetworkOrder(pText, nSize, pBuffer);
         break;

      case AEESMS_ENC_GSM_7_BIT_DEFAULT:
         // Assumes that for GSM 7 bit encoding the User Data has already been 
         // unpacked by the native device software, we may need to do some 
         // translation to ASCII, but there is a one-to-one mapping from the GSM 
         // default encoding and ASCII for most commonly used characters
      case AEESMS_ENC_IS91EP:
      case AEESMS_ENC_LATIN:
         // User Data is already unpacked.
         // fall through
      default:        
         DecodeString(pText, nSize, (char*)pBuffer);
         break;
   }

   if (pae)
      *pae = Xlate_SMSEncToAEEEnc(se);

   if (pnBytes)
      *pnBytes = nSize;

   return pBuffer;
}

/*===========================================================================
Called to get encoded text string
===========================================================================*/
static int GetEncodedMsgPayload(char* pszBrewPrefix, byte * pMsgPayload, int nBytes, int nEncoding, AEESMSEncType nSMSEncoding, byte * pEncodedPayload, int * pnBytes, int * pnPadding)
{
   int nErr = SUCCESS;
   int nExtra = (pszBrewPrefix ? STRLEN(pszBrewPrefix) : 0);

   if (!nBytes)
   {
      nBytes += nExtra;
      *pnBytes = nBytes;
      *pnPadding = 0;

      if (pEncodedPayload)
      {
         if (pszBrewPrefix)
            MEMCPY(pEncodedPayload, pszBrewPrefix, nExtra);
      }
   }
   else
   {
      switch (nSMSEncoding) 
      {  
         case AEESMS_ENC_OCTET:
            nBytes += nExtra;
            *pnBytes = nBytes;
            *pnPadding = 0;

            if (pEncodedPayload)
            {
               if (pszBrewPrefix)
                  MEMCPY(pEncodedPayload, pszBrewPrefix, nExtra);

               if (pMsgPayload)
                  MEMCPY(pEncodedPayload + nExtra, pMsgPayload, nBytes - nExtra);
            }
            break;

         case AEESMS_ENC_ASCII:
            switch (nEncoding)
            {
               case AEE_ENC_ISOLATIN1:
                  if (IsMsgPackable(pMsgPayload, nBytes, nEncoding, TRUE))
                  {
                     nBytes += nExtra;
                     *pnBytes = (nBytes * 7)/8;
                     *pnBytes += ((nBytes * 7) % 8 ? 1 : 0);
                     *pnPadding = ((nBytes * 7) % 8 ? 8 - (nBytes * 7)%8 : 0);

                     if (pEncodedPayload)
                     {
                        byte * pBuffer = NULL;
                        if ((pBuffer = MALLOC(nBytes)) != NULL)
                        {
                           if (pszBrewPrefix)
                              MEMCPY(pBuffer, pszBrewPrefix, nExtra);

                           if (pMsgPayload)
                              MEMCPY(pBuffer + nExtra, pMsgPayload, nBytes - nExtra);

                           PackString((char*)pBuffer, nBytes, (byte*)pEncodedPayload);

                           FREE(pBuffer);
                        }
                        else
                           nErr = ENOMEMORY;
                     }
                  }
                  else
                     nErr = EFAILED;
                  break;

               case AEE_ENC_UNICODE:
               case AEE_ENC_EUC_CN:
                  if (IsMsgPackable(pMsgPayload, nBytes, nEncoding, TRUE))
                  {
                     nBytes /= sizeof(AECHAR);
                     nBytes += nExtra;
                     *pnBytes = (nBytes * 7)/8;
                     *pnBytes += ((nBytes * 7) % 8 ? 1 : 0);
                     *pnPadding = ((nBytes * 7) % 8 ? 8 - (nBytes * 7)%8 : 0);
                     if (pEncodedPayload)
                     {                  
                        byte * pBuffer = NULL;
                        if ((pBuffer = MALLOC(nBytes + 1)) != NULL)
                        {
                           if (pszBrewPrefix)
                              MEMCPY(pBuffer, pszBrewPrefix, nExtra);

                           if (pMsgPayload)
                              WSTRTOSTR((AECHAR*)pMsgPayload, (char*)(pBuffer + nExtra), nBytes - nExtra + 1);

                           PackString((char*)pBuffer, nBytes, (byte*)pEncodedPayload);

                           FREE(pBuffer);
                        }
                        else
                           nErr = ENOMEMORY;
                     }
                  }
                  else
                     nErr = EFAILED;
                  break;

               default:
                  nErr = EFAILED;
            }
            break;

         case AEESMS_ENC_LATIN:
            switch (nEncoding)
            {
               case AEE_ENC_ISOLATIN1:
                  nBytes += nExtra;
                  *pnBytes = nBytes;
                  *pnPadding = 0;
                  if (pEncodedPayload)
                  {
                     if (pszBrewPrefix)
                        MEMCPY(pEncodedPayload, pszBrewPrefix, nExtra);

                     if (pMsgPayload)
                        MEMCPY(pEncodedPayload + nExtra, pMsgPayload, nBytes - nExtra);
                  }
                  break;
         
               case AEE_ENC_UNICODE:      
               case AEE_ENC_EUC_CN: 
                  if (IsMsgPackable(pMsgPayload, nBytes, nEncoding, FALSE))
                  {
                     nBytes /= sizeof(AECHAR);
                     nBytes += nExtra;
                     *pnBytes = nBytes;               
                     *pnPadding = 0;
                     if (pEncodedPayload)
                     {                  
                        byte * pBuffer = NULL;
                        if ((pBuffer = MALLOC(nBytes + 1)) != NULL)
                        {
                           if (pszBrewPrefix)
                              MEMCPY(pBuffer, pszBrewPrefix, nExtra);

                           if (pMsgPayload)
                              WSTRTOSTR((AECHAR*)pMsgPayload, (char*)(pBuffer + nExtra), nBytes - nExtra + 1);

                           MEMCPY(pEncodedPayload, pBuffer, nBytes);

                           FREE(pBuffer);
                        }
                        else
                           nErr = ENOMEMORY;
                     }
                  }
                  else
                     nErr = EFAILED;
                  break;

               default:
                  nErr = EFAILED;
            }
            break;

         case AEESMS_ENC_UNICODE:
            switch (nEncoding)
            {
               case AEE_ENC_ISOLATIN1:
                  nBytes *= sizeof(AECHAR);
                  nBytes += nExtra;
                  *pnBytes = nBytes;
                  *pnPadding = 0;

                  if (pEncodedPayload)
                  {
                     byte * pBuffer = NULL;
                     if ((pBuffer = MALLOC(nBytes + sizeof(AECHAR))) != NULL)
                     {
                        if (pszBrewPrefix)
                           MEMCPY(pBuffer, pszBrewPrefix, nExtra);

                        if (pMsgPayload)
                           STRTOWSTR((char*)pMsgPayload, (AECHAR*)(pBuffer + nExtra), nBytes - nExtra + sizeof(AECHAR));

                        EncodeNetworkOrder((AECHAR*)pBuffer, nBytes, (AECHAR*)pEncodedPayload);

                        FREE(pBuffer);
                     }
                     else
                        nErr = ENOMEMORY;
                  }
                  break;
               
               case AEE_ENC_UNICODE:
               case AEE_ENC_EUC_CN:
                  nBytes += nExtra;
                  *pnBytes = nBytes;
                  *pnPadding = 0;

                  if (pEncodedPayload)
                  {               
                     byte * pBuffer = NULL;
                     if ((pBuffer = MALLOC(nBytes)) != NULL)
                     {
                        if (pszBrewPrefix)
                           MEMCPY(pBuffer, pszBrewPrefix, nExtra);

                        if (pMsgPayload)
                           MEMCPY(pBuffer + nExtra, pMsgPayload, nBytes - nExtra);

                        EncodeNetworkOrder((AECHAR*)pBuffer, nBytes, (AECHAR*)pEncodedPayload);

                        FREE(pBuffer);
                     }
                     else
                        nErr = ENOMEMORY;
                  }
                  break;

               default:
                  nErr = EFAILED;
            }
            break;

         case AEESMS_ENC_GSM_7_BIT_DEFAULT:
            if (AEE_ENC_GSM_7_BIT_DEFAULT == nEncoding)
            {
               if (IsMsgPackable(pMsgPayload, nBytes, nEncoding, TRUE))
               {

                  if (pEncodedPayload)
                  {
                     // Don't pack here as packing is done by lower layer
                     nBytes += nExtra;
                     *pnBytes = nBytes;
                     *pnPadding = 0;

                     if (pszBrewPrefix)
                        MEMCPY(pEncodedPayload, pszBrewPrefix, nExtra);

                     if (pMsgPayload)
                        MEMCPY(pEncodedPayload + nExtra, pMsgPayload, nBytes - nExtra);
                  }
                  else
                  {
                     // On query, tell how many bytes will it take after packing
                     int nRem;
                     nBytes += nExtra;
                     *pnBytes = (nBytes * 7)/8;
                     nRem = (nBytes * 7) % 8;
                     *pnBytes += (nRem ? 1 : 0);
                     *pnPadding = (nRem ? 8 - nRem : 0);
                  }
               }
               else
                  nErr = EFAILED;
            }
            else
               nErr = EFAILED;
            break;

         default:
            if (Xlate_SMSEncToAEEEnc(nSMSEncoding) == nEncoding)
            {
               nBytes += nExtra;
               *pnBytes = nBytes;
               *pnPadding = 0;

               if (pEncodedPayload)
               {
                  if (pszBrewPrefix)
                     MEMCPY(pEncodedPayload, pszBrewPrefix, nExtra);

                  if (pMsgPayload)
                     MEMCPY(pEncodedPayload + nExtra, pMsgPayload, nBytes - nExtra);
               }
            }
            else
               nErr = EFAILED;
            break;
      }
   }
   return nErr;
}

/*=================================================================================
Size in bits of data type
=================================================================================*/
#define bitsize(type) (sizeof(type) * 8)

/*=================================================================================
Create a mask of bits sized to the number of bits in the given type
=================================================================================*/
#define copymask(type) ((0xffffffff) >> (32 - bitsize(type)))

/*=================================================================================
Masks the bits in data at the given offset for given number of width bits
=================================================================================*/
#define MASK(width, offset, data) \
   (((width) == bitsize(data)) ? (data) :   \
   ((((copymask(data) << (bitsize(data) - ((width) % bitsize(data)))) & copymask(data)) >>  (offset)) & (data)))

/*=================================================================================
Same as the macro MASK except also shifts the data in the result by the given number 
of shift bits
=================================================================================*/
#define MASK_AND_SHIFT(width, offset, shift, data)  \
                  ((((signed) (shift)) < 0) ?       \
                    MASK((width), (offset), (data)) << -(shift) :  \
                    MASK((width), (offset), (data)) >>  (((unsigned) (shift))))

/*============================================================================

FUNCTION UNPACKBYTES

DESCRIPTION
  Given a buffer and an offset, unpacks the requested number of bits into
  a byte

DEPENDENCIES
  None
   
RETURN VALUE
  Unpacked item

SIDE EFFECTS
  None
   
============================================================================*/
static byte UNPACKBYTES(byte *src, word pos, word len)
{
   byte result = 0;
   int rshift = 0;

   src += pos/8;
   pos %= 8;
   
   rshift = MAX( 8 - (pos + len), 0);

   if ( rshift > 0 ) {

     result = (byte)MASK_AND_SHIFT(len, pos, rshift, *src);
  
   } else {

     result = (byte)MASK(8-pos, pos, *src);
     src++;
     len -= 8 - pos;

      if ( len > 0 ) result = ( result<<len ) | (*src >> (8-len));  // if any bits left
   }

   return result;
}

/*===========================================================================
Function: DecodePackedString

Description:
   Unpack an byte array containing 7-bit packed characters into a character
   array.

Parameter(s):
   byte *pSrc    [ in ]
   int i         [ in ]  Number of 7-bit characters in the array. 
   char *pszDst  [ out ] Pointer to destination character array.

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None
===========================================================================*/
static void DecodePackedString(byte *pSrc, int i, char *pszDst)
{
   byte     b;
   uint16   wPos;

   FARF(SMS, ("==>  DecodePackedString"));

   for (wPos = 0; i ; i--, wPos += 7) {
      b = UNPACKBYTES((byte *)pSrc,wPos,(word)7);

      // Protect against decode/encode adding some junk at end
      if ((i == 1) && (b < 0x20))
         b = 0;

      *(pszDst++) = (char)b;
   }
}

/*===========================================================================
Function: DecodeString

Description:
   Copy an array of bytes into an array of characters.

Parameter(s):
   byte *pSrc    [ in ]
   int i         [ in ]  Length of byte array. 
   char *pszDst  [ out ] Pointer to destination character array.

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None
===========================================================================*/
static void DecodeString(byte *pSrc, int i, char *pszDst)
{
   byte  bchar;

   FARF(SMS, ("==>  DecodeString"));

   for (; i; i--, pSrc++) {
      bchar = *pSrc;
      *(pszDst++) = (char)bchar;
   }
}

/*===========================================================================
Function: DecodeNetworkOrder

Description: 
   Take a network order unicode character and converts to host order unicode character.

Prototype:
   void DecodeNetworkOrder(byte *pbSrc, int i, byte *pszDst)

Parameter(s):  

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None
===========================================================================*/
static void DecodeNetworkOrder(byte *pbSrc, int i, byte *pszDst)
{
   uint16  achar;

   FARF(SMS, ("==>  DecodeNetworkOrder"));

   // previous code had alignment issues when pbSrc or pszDst were not on
   // word boundries. thus, we have to do it a byte at a time.
   for (i = i/2; i; i--) {
      achar = (*(pbSrc++) << 8);
      achar |= *(pbSrc++);
      achar = NTOHS(achar);
      *(pszDst++) = (achar & 0xFF00) >> 8;
      *(pszDst++) = achar&0xFF;
   }
}

/* --------------------------------------------------
** Masks the number of bits give by length starting
** at the given offset.  Unlike MASK and MASK_AND_SHIFT,
** this macro only creates that mask, it does not
** operate on the data buffer
** -------------------------------------------------- */
#define MASK_B(offset, len) \
  ((0xff >> offset) & (0xff << (8 - (offset + len))))

/*============================================================================

FUNCTION PACKBYTES

DESCRIPTION
  Packs the given byte into the destination at the given offset for the
  given number of length bits

DEPENDENCIES
  None
   
RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
static void PACKBYTES(
   byte src, 
   byte dst[], 
   word pos, 
   word len 
)
{
   word   t_pos = pos % 8;
   word   bits  = 8 - t_pos;

   dst += (pos+len-1)/8;

   if ( bits >= len )
   {
       *dst &= (byte) ~MASK_B(t_pos, len);
       *dst |= (byte) (MASK_B(t_pos, len) & (src << (bits - len)));
   }
   else /* len > bits */
   {
       dst--;
       *dst &= (byte) ~MASK_B(t_pos, bits);
       *dst |= (byte) (MASK_B(t_pos, bits) & (src >> (len - bits)));

       dst++;
       *dst &= (byte) ~MASK_B(0, (len - bits));
       *dst |= (byte) (MASK_B(0, (len - bits)) & (src << (8 - (len - bits))));
   }
}

/*===========================================================================
Function: PackString

Description:
   Packs a character array into a byte array containing 7-bit packed characters .

Parameter(s):
   char *pText    [ in ] Character array
   int nText      [ in ]  Number of characters in the character array. 
   byte *pBuffer  [ out ] Pointer to destination byte array.

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None
===========================================================================*/
void PackString(char * pText, int nText, byte * pBuffer)
{
   int i;
   word bit_pos = 0;

   if (pBuffer) 
   {
      for (i = 0; i < (int)nText; i++)
      {
         PACKBYTES((pText[i]), (byte *)pBuffer, bit_pos, 7);
         bit_pos += 7;
      }
                 
      if (bit_pos % 8 != 0)
      {  
         PACKBYTES(0, (byte *)pBuffer, bit_pos, (word)(8 - bit_pos % 8));               
         bit_pos += 8 - bit_pos % 8;            
      }
   }
}

/*===========================================================================
Function: EncodeNetworkOrder

Description: 
   Take a host order Unicode character and converts
   to network order Unicode character.

Prototype:
   void EncodeNetworkOrder(AECHAR * pSrc, int i, uint16 * pszDst);

Parameter(s):  

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None
===========================================================================*/
static void EncodeNetworkOrder(AECHAR * pSrc, int i, uint16 * pszDst)
{
   uint16  uchar;

   for (i = i/2; i; i--, pSrc++) {
      uchar = HTONS(*pSrc);
      *(pszDst++) = uchar;
   }
}

/*========================================================================== 
Local Methods
==========================================================================*/
/*===========================================================================

Function: InvokeAppCb

Description:
   Invokes app callback
 
===========================================================================*/
static void InvokeAppCb(void * pData)
{
   SMSAppCbData * pAppCbData = (SMSAppCbData *)pData;
  
   AEE_ResumeCallback(pAppCbData->pcb, pAppCbData->pac);
}

/*===========================================================================

Function: ReleaseAppCb

Description:
   Releases app callback
 
===========================================================================*/
static void ReleaseAppCb(AEECallback * pcb)
{  
   if(pcb->pfnCancel != ReleaseAppCb)
      return;

   CALLBACK_Cancel(&((SMSAppCbData *)pcb->pCancelData)->cb);
}

/*===========================================================================

Function: LinkAppCb

Description:
   Links app callback to system
 
===========================================================================*/
static int LinkAppCb(AEECallback * pcb, SMSAppCbData ** ppAppCbData)
{
   int nErr = SUCCESS;
   AEECallback * pcbLinked = NULL;
   SMSAppCbData * pAppCbData = NULL;

   if ((pcbLinked = AEE_LinkSysCallbackEx(pcb, sizeof(SMSAppCbData))) == NULL)
   {
      nErr = ENOMEMORY;
      goto Done;
   }

   pAppCbData = (SMSAppCbData *)(pcbLinked + 1);
   pAppCbData->pac = AEE_GetAppContext();
   pAppCbData->pcb = pcbLinked;

   pcbLinked->pfnCancel = ReleaseAppCb;
   pcbLinked->pCancelData = pAppCbData;

   *ppAppCbData = pAppCbData;

   // Create kernel callback to be passed to OEM layer.
   CALLBACK_Init(&pAppCbData->cb, InvokeAppCb, pAppCbData);

Done:
   return nErr;
}

/*===========================================================================

Function: InvokeAppSendMsgCb

Description:
   Invokes app callback for ISMS_SendMsg
 
===========================================================================*/
static void InvokeAppSendMsgCb(void * pData)
{
   SMSAppSendMsgCbData * pAppSendMsgCbData = (SMSAppSendMsgCbData *)pData;

   if (*pAppSendMsgCbData->pnRv == SUCCESS)
   {
      ModelEvent me;
      ISMSMsg * pISMSMsg = NULL;
      
      OEMMsgToISMSMsg(pAppSendMsgCbData->pMsgData, pAppSendMsgCbData->pMsgPayload, &pISMSMsg);
      
      me.evCode = EVT_MDL_SMS_MSG_SENT;
      me.pModel = &pAppSendMsgCbData->pme->model;
      me.dwParam = (uint32)pISMSMsg;
      IModelClass_NotifyApp(&pAppSendMsgCbData->pme->model, &me, pAppSendMsgCbData->pac);

      ISMSMSG_Release(pISMSMsg);
   }
  
   AEE_ResumeCallback(pAppSendMsgCbData->pcb, pAppSendMsgCbData->pac);
}

/*===========================================================================

Function: ReleaseAppSendMsgCb

Description:
   Releases app callback for ISMS_SendMsg
 
===========================================================================*/
static void ReleaseAppSendMsgCb(AEECallback * pcb)
{   
   if(pcb->pfnCancel != ReleaseAppSendMsgCb)
      return;

   ISMS_Release(((SMSAppSendMsgCbData*)pcb->pCancelData)->pme);
   FREEIF(((SMSAppSendMsgCbData*)pcb->pCancelData)->pMsgData);
   FREEIF(((SMSAppSendMsgCbData*)pcb->pCancelData)->pMsgPayload);
   CALLBACK_Cancel(&((SMSAppSendMsgCbData*)pcb->pCancelData)->cb);
}

/*===========================================================================

Function: LinkAppSendMsgCb

Description:
   Links app callback for ISMS_SendMsg to system
 
===========================================================================*/
static int LinkAppSendMsgCb(AEECallback * pcb, SMSAppSendMsgCbData ** ppAppSendMsgCbData)
{
   int nErr = SUCCESS;
   AEECallback * pcbLinked = NULL;
   SMSAppSendMsgCbData * pAppSendMsgCbData = NULL;

   if ((pcbLinked = AEE_LinkSysCallbackEx(pcb, sizeof(SMSAppSendMsgCbData))) == NULL)
   {
      nErr = ENOMEMORY;
      goto Done;
   }

   pAppSendMsgCbData = (SMSAppSendMsgCbData *)(pcbLinked + 1);
   pAppSendMsgCbData->pac = AEE_GetAppContext();
   pAppSendMsgCbData->pcb = pcbLinked;

   pcbLinked->pfnCancel = ReleaseAppSendMsgCb;
   pcbLinked->pCancelData = pAppSendMsgCbData;

   *ppAppSendMsgCbData = pAppSendMsgCbData;

   // Create kernel callback to be passed to OEM layer.
   CALLBACK_Init(&pAppSendMsgCbData->cb, InvokeAppSendMsgCb, pAppSendMsgCbData);

Done:
   return nErr;
}

/*===========================================================================

Function: InvokeAppStoreMsgCb

Description:
   Invokes app callback for ISMSSTORAGE_StoreMsg
 
===========================================================================*/
static void InvokeAppStoreMsgCb(void * pData)
{
   SMSAppStoreMsgCbData * pAppStoreMsgCbData = (SMSAppStoreMsgCbData *)pData;

   AEE_ResumeCallback(pAppStoreMsgCbData->pcb, pAppStoreMsgCbData->pac);
}

/*===========================================================================

Function: ReleaseAppStoreMsgCb

Description:
   Releases app callback for ISMSSTORAGE_StoreMsg
 
===========================================================================*/
static void ReleaseAppStoreMsgCb(AEECallback * pcb)
{   
   if(pcb->pfnCancel != ReleaseAppStoreMsgCb)
      return;

   CALLBACK_Cancel(&((SMSAppStoreMsgCbData*)pcb->pCancelData)->cb);
}

/*===========================================================================

Function: LinkAppStoreMsgCb

Description:
   Links app callback for ISMSSTORAGE_StoreMsg to system
 
===========================================================================*/
static int LinkAppStoreMsgCb(AEECallback * pcb, SMSAppStoreMsgCbData ** ppAppStoreMsgCbData)
{
   int nErr = SUCCESS;
   AEECallback * pcbLinked = NULL;
   SMSAppStoreMsgCbData * pAppStoreMsgCbData = NULL;

   if ((pcbLinked = AEE_LinkSysCallbackEx(pcb, sizeof(SMSAppStoreMsgCbData))) == NULL)
   {
      nErr = ENOMEMORY;
      goto Done;
   }

   pAppStoreMsgCbData = (SMSAppStoreMsgCbData *)(pcbLinked + 1);
   pAppStoreMsgCbData->pac = AEE_GetAppContext();
   pAppStoreMsgCbData->pcb = pcbLinked;

   pcbLinked->pfnCancel = ReleaseAppStoreMsgCb;
   pcbLinked->pCancelData = pAppStoreMsgCbData;

   *ppAppStoreMsgCbData = pAppStoreMsgCbData;

   // Create kernel callback to be passed to OEM layer.
   CALLBACK_Init(&pAppStoreMsgCbData->cb, InvokeAppStoreMsgCb, pAppStoreMsgCbData);

Done:
   return nErr;
}

/*===========================================================================

Function: InvokeAppReadMsgCb

Description:
   Invokes app callback for ISMSSTORAGE_ReadMsg
 
===========================================================================*/
static void InvokeAppReadMsgCb(void * pData)
{
   SMSAppReadMsgCbData * pAppReadMsgCbData = (SMSAppReadMsgCbData *)pData;
  
   if (*pAppReadMsgCbData->pnRv == SUCCESS)
   {
      OEMMsgToISMSMsg(pAppReadMsgCbData->pMsgData, pAppReadMsgCbData->pMsgPayload, pAppReadMsgCbData->ppISMSMsg);
   }

   AEE_ResumeCallback(pAppReadMsgCbData->pcb, pAppReadMsgCbData->pac);
}

/*===========================================================================

Function: ReleaseAppReadMsgCb

Description:
   Releases app callback for ISMSSTORAGE_ReadMsg
 
===========================================================================*/
static void ReleaseAppReadMsgCb(AEECallback * pcb)
{   
   if(pcb->pfnCancel != ReleaseAppReadMsgCb)
      return;

   CALLBACK_Cancel(&((SMSAppReadMsgCbData*)pcb->pCancelData)->cb);
}

/*===========================================================================

Function: LinkAppReadMsgCb

Description:
   Links app callback for ISMSSTORAGE_ReadMsg to system
 
===========================================================================*/
static int LinkAppReadMsgCb(AEECallback * pcb, SMSAppReadMsgCbData ** ppAppReadMsgCbData)
{
   int nErr = SUCCESS;
   AEECallback * pcbLinked = NULL;
   SMSAppReadMsgCbData * pAppReadMsgCbData = NULL;
   byte * pBytes = NULL;

   if ((pcbLinked = AEE_LinkSysCallbackEx(pcb, sizeof(SMSAppReadMsgCbData) + 
                                               sizeof(OEMMsgData) + MAX_MSG_LENGTH + MAX_ALPHA_ID_LENGTH +
                                               sizeof(OEMMsgPayload) + MAX_MSG_LENGTH)) == NULL)
   {
      nErr = ENOMEMORY;
      goto Done;
   }
   
   pAppReadMsgCbData = (SMSAppReadMsgCbData *)(pcbLinked + 1);
   pAppReadMsgCbData->pac = AEE_GetAppContext();
   pAppReadMsgCbData->pcb = pcbLinked;
    
   pcbLinked->pfnCancel = ReleaseAppReadMsgCb;
   pcbLinked->pCancelData = pAppReadMsgCbData; 
   
   pAppReadMsgCbData->pMsgData = (OEMMsgData*)(pAppReadMsgCbData + 1);
   pAppReadMsgCbData->pMsgData->nUDHLength = MAX_MSG_LENGTH;
   pAppReadMsgCbData->pMsgData->nAlphaIDLength = MAX_ALPHA_ID_LENGTH;

   pBytes = (byte*)(pAppReadMsgCbData + 1);
   pBytes += (sizeof(OEMMsgData) + MAX_MSG_LENGTH + MAX_ALPHA_ID_LENGTH);

   pAppReadMsgCbData->pMsgPayload = (OEMMsgPayload*)pBytes;
   pAppReadMsgCbData->pMsgPayload->nBytes = MAX_MSG_LENGTH;

   *ppAppReadMsgCbData = pAppReadMsgCbData;

   // Create kernel callback to be passed to OEM layer.
   CALLBACK_Init(&pAppReadMsgCbData->cb, InvokeAppReadMsgCb, pAppReadMsgCbData);

Done:
   return nErr;
}

/*===========================================================================

Function: InvokeAppDeleteMsgCb

Description:
   Invokes app callback for ISMSSTORAGE_DeleteMsg
 
===========================================================================*/
static void InvokeAppDeleteMsgCb(void * pData)
{
   SMSAppDeleteMsgCbData * pAppDeleteMsgCbData = (SMSAppDeleteMsgCbData *)pData;

   AEE_ResumeCallback(pAppDeleteMsgCbData->pcb, pAppDeleteMsgCbData->pac);
}

/*===========================================================================

Function: ReleaseAppDeleteMsgCb

Description:
   Releases app callback for ISMSSTORAGE_DeleteMsg
 
===========================================================================*/
static void ReleaseAppDeleteMsgCb(AEECallback * pcb)
{   
   if(pcb->pfnCancel != ReleaseAppDeleteMsgCb)
      return;

   CALLBACK_Cancel(&((SMSAppDeleteMsgCbData*)pcb->pCancelData)->cb);
}

/*===========================================================================

Function: LinkAppDeleteMsgCb

Description:
   Links app callback for ISMSSTORAGE_DeleteMsg to system
 
===========================================================================*/
static int LinkAppDeleteMsgCb(AEECallback * pcb, SMSAppDeleteMsgCbData ** ppAppDeleteMsgCbData)
{
   int nErr = SUCCESS;
   AEECallback * pcbLinked = NULL;
   SMSAppDeleteMsgCbData * pAppDeleteMsgCbData = NULL;

   if ((pcbLinked = AEE_LinkSysCallbackEx(pcb, sizeof(SMSAppDeleteMsgCbData))) == NULL)
   {
      nErr = ENOMEMORY;
      goto Done;
   }

   pAppDeleteMsgCbData = (SMSAppDeleteMsgCbData *)(pcbLinked + 1);
   pAppDeleteMsgCbData->pac = AEE_GetAppContext();
   pAppDeleteMsgCbData->pcb = pcbLinked;

   pcbLinked->pfnCancel = ReleaseAppDeleteMsgCb;
   pcbLinked->pCancelData = pAppDeleteMsgCbData;

   *ppAppDeleteMsgCbData = pAppDeleteMsgCbData;

   // Create kernel callback to be passed to OEM layer.
   CALLBACK_Init(&pAppDeleteMsgCbData->cb, InvokeAppDeleteMsgCb, pAppDeleteMsgCbData);

Done:
   return nErr;
}

/*===========================================================================

Function: InvokeAppEnumMsgCb

Description:
   Invokes app callback for ISMSSTORAGE_EnumNextMsg
 
===========================================================================*/
static void InvokeAppEnumMsgCb(void * pData)
{
   ISMSStorage2 * pme = NULL;
   SMSAppEnumMsgCbData * pAppEnumMsgCbData = (SMSAppEnumMsgCbData *)pData;
  
   // Get pme
   pme = pAppEnumMsgCbData->pISMSStorage;

   // Check if we could read message and message still has same tag as the one
   // being enumerated
   if ((SUCCESS == *pAppEnumMsgCbData->pnRv) &&
       ((AEESMS_TAG_NONE == pme->mt) || 
        (pAppEnumMsgCbData->pMsgData->dwTag == pme->mt)))
   {
      if (pAppEnumMsgCbData->ppISMSMsg)
      {
         OEMMsgToISMSMsg(pAppEnumMsgCbData->pMsgData, 
                         pAppEnumMsgCbData->pMsgPayload, 
                         pAppEnumMsgCbData->ppISMSMsg);
      }

      if (pAppEnumMsgCbData->pnIndex)
      {
         *pAppEnumMsgCbData->pnIndex = pme->pdwIndex[pme->dwOffset];
      }

      // Move offset by one
      pme->dwOffset++;

      AEE_ResumeCallback(pAppEnumMsgCbData->pcb, pAppEnumMsgCbData->pac);
   }
   else
   {
      // Guard against scenario where one client is enumerating where as
      // other has deleted some message

      // Move offset by one
      pme->dwOffset++;

      if (pme->dwOffset < pme->dwCount)
      {
         // Read Next Msg
         OEMSMS_ReadMsg(pme->st, pme->pdwIndex[pme->dwOffset], 
                        pAppEnumMsgCbData->pMsgData, pAppEnumMsgCbData->pMsgPayload, 
                        pAppEnumMsgCbData->pnRv, &pAppEnumMsgCbData->cb);
      }
      else
      {
         // End of Enumeration
         if (pAppEnumMsgCbData->ppISMSMsg)
         {
            *pAppEnumMsgCbData->ppISMSMsg = NULL;
         }

         if (pAppEnumMsgCbData->pnIndex)
         {
            *pAppEnumMsgCbData->pnIndex = 0xffffffff;
         }

         FREEIF(pme->pdwIndex);
         pme->pdwIndex = NULL;
         pme->bEnumInit = FALSE;

         AEE_ResumeCallback(pAppEnumMsgCbData->pcb, pAppEnumMsgCbData->pac);
      }
   }
}

/*===========================================================================

Function: ReleaseAppEnumMsgCb

Description:
   Releases app callback for ISMSSTORAGE_EnumNextMsg
 
===========================================================================*/
static void ReleaseAppEnumMsgCb(AEECallback * pcb)
{   
   if(pcb->pfnCancel != ReleaseAppEnumMsgCb)
      return;

   CALLBACK_Cancel(&((SMSAppEnumMsgCbData*)pcb->pCancelData)->cb);
}

/*===========================================================================

Function: LinkAppEnumMsgCb

Description:
   Links app callback for ISMSSTORAGE_EnumNextMsg to system
 
===========================================================================*/
static int LinkAppEnumMsgCb(AEECallback * pcb, SMSAppEnumMsgCbData ** ppAppEnumMsgCbData)
{
   int nErr = SUCCESS;
   AEECallback * pcbLinked = NULL;
   SMSAppEnumMsgCbData * pAppEnumMsgCbData = NULL;
   byte * pBytes = NULL;

   if ((pcbLinked = AEE_LinkSysCallbackEx(pcb, sizeof(SMSAppEnumMsgCbData) + 
                                               sizeof(OEMMsgData) + MAX_MSG_LENGTH + MAX_ALPHA_ID_LENGTH +
                                               sizeof(OEMMsgPayload) + MAX_MSG_LENGTH)) == NULL)
   {
      nErr = ENOMEMORY;
      goto Done;
   }

   pAppEnumMsgCbData = (SMSAppEnumMsgCbData *)(pcbLinked + 1);
   pAppEnumMsgCbData->pac = AEE_GetAppContext();
   pAppEnumMsgCbData->pcb = pcbLinked;

   pcbLinked->pfnCancel = ReleaseAppEnumMsgCb;
   pcbLinked->pCancelData = pAppEnumMsgCbData;

   pAppEnumMsgCbData->pMsgData = (OEMMsgData*)(pAppEnumMsgCbData + 1);
   pAppEnumMsgCbData->pMsgData->nUDHLength = MAX_MSG_LENGTH;
   pAppEnumMsgCbData->pMsgData->nAlphaIDLength = MAX_ALPHA_ID_LENGTH;

   pBytes = (byte*)(pAppEnumMsgCbData + 1);
   pBytes += (sizeof(OEMMsgData) + MAX_MSG_LENGTH + MAX_ALPHA_ID_LENGTH);

   pAppEnumMsgCbData->pMsgPayload = (OEMMsgPayload*)pBytes;
   pAppEnumMsgCbData->pMsgPayload->nBytes = MAX_MSG_LENGTH;

   *ppAppEnumMsgCbData = pAppEnumMsgCbData;

   // Create kernel callback to be passed to OEM layer.
   CALLBACK_Init(&pAppEnumMsgCbData->cb, InvokeAppEnumMsgCb, pAppEnumMsgCbData);

Done:
   return nErr;
}

/*===========================================================================

Function: InvokeAppGetSrvIDsCb

Description:
   Invokes app callback for ISMSBCConfig_GetServiceIDs
 
===========================================================================*/
static void InvokeAppGetSrvIDsCb(void * pData)
{
   SMSAppGetSrvIDsCbData *pAppGetSrvIDsCbData = (SMSAppGetSrvIDsCbData *)pData;
   
   FARF(SMS, ("==>  InvokeAppGetSrvIDsCb"));

   if ((*pAppGetSrvIDsCbData->pnErr == SUCCESS) &&
       (*pAppGetSrvIDsCbData->pnCount != 0))
   {
      if ((*pAppGetSrvIDsCbData->ppsid = MALLOC(*pAppGetSrvIDsCbData->pnCount*sizeof(AEESMSBCSrvID))) != NULL)
      {
         // Pass through to OEM
         OEMSMSBCConfig_FillServiceIDs(*pAppGetSrvIDsCbData->ppsid, *pAppGetSrvIDsCbData->pnCount*sizeof(AEESMSBCSrvID));
      }
      else
      {
         *pAppGetSrvIDsCbData->pnErr = ENOMEMORY;
      }
   }

   // Finally Resume the Application 
   AEE_ResumeCallback(pAppGetSrvIDsCbData->pcb, pAppGetSrvIDsCbData->pac);
}

/*===========================================================================

Function: ReleaseAppGetSrvIDsCb

Description:
   Releases app callback for ISMSBCConfig_GetServiceIDs
 
===========================================================================*/
static void ReleaseAppGetSrvIDsCb(AEECallback * pcb)
{   
   if(pcb->pfnCancel != ReleaseAppGetSrvIDsCb)
      return;

   CALLBACK_Cancel(&((SMSAppGetSrvIDsCbData*)pcb->pCancelData)->cb);
}

/*===========================================================================

Function: LinkAppGetSrvIDsCb

Description:
   Links app callback to system for ISMSBCConfig_GetServiceIDs
 
===========================================================================*/
static int LinkAppGetSrvIDsCb(AEECallback * pcb, SMSAppGetSrvIDsCbData ** ppAppGetSrvIDsCbData)
{
   int nErr = SUCCESS;
   AEECallback * pcbLinked = NULL;
   SMSAppGetSrvIDsCbData * pAppGetSrvIDsCbData = NULL;

   if ((pcbLinked = AEE_LinkSysCallbackEx(pcb, sizeof(SMSAppGetSrvIDsCbData))) == NULL)
   {
      nErr = ENOMEMORY;
   }
   else
   {
      pAppGetSrvIDsCbData = (SMSAppGetSrvIDsCbData *)(pcbLinked + 1);
      pAppGetSrvIDsCbData->pac = AEE_GetAppContext();
      pAppGetSrvIDsCbData->pcb = pcbLinked;

      pcbLinked->pfnCancel = (PFNCBCANCEL)ReleaseAppGetSrvIDsCb;
      pcbLinked->pCancelData = pAppGetSrvIDsCbData;

      *ppAppGetSrvIDsCbData = pAppGetSrvIDsCbData;

      // Create kernel callback to be passed to OEM layer.
      CALLBACK_Init(&pAppGetSrvIDsCbData->cb, InvokeAppGetSrvIDsCb, pAppGetSrvIDsCbData);
   }
   return nErr;
}

/*===========================================================================

Function: InvokeAppGetSrvOptsCb

Description:
   Invokes app callback for ISMSBCConfig_GetServiceOpts

===========================================================================*/
static void InvokeAppGetSrvOptsCb(void * pData)
{
   int nErr = SUCCESS;
   SMSAppGetSrvOptsCbData *pAppGetSrvOptsCbData = (SMSAppGetSrvOptsCbData *)pData;
   
   FARF(SMS, ("==>  InvokeAppGetSrvOptsCb"));

   if (*pAppGetSrvOptsCbData->pnErr == SUCCESS)
   {
      if ((nErr = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_SMSBCSRVOPTS, (void **)pAppGetSrvOptsCbData->ppi)) == SUCCESS)
      {
         if (PackSMSBCSrvOpts(&pAppGetSrvOptsCbData->sSMSBcSrvInfo, *pAppGetSrvOptsCbData->ppi) != TRUE)
         {
            *pAppGetSrvOptsCbData->pnErr = EBADPARM;
         }
      }
      else
      {
         *pAppGetSrvOptsCbData->pnErr = nErr;
      }
   }

   // Finally Resume the Application 
   AEE_ResumeCallback(pAppGetSrvOptsCbData->pcb, pAppGetSrvOptsCbData->pac);
}

/*===========================================================================

Function: ReleaseAppGetSrvOptsCb

Description:
   Releases app callback for ISMSBCConfig_GetServiceOpts
 
===========================================================================*/
static void ReleaseAppGetSrvOptsCb(AEECallback * pcb)
{   
   if(pcb->pfnCancel != ReleaseAppGetSrvOptsCb)
      return;

   CALLBACK_Cancel(&((SMSAppGetSrvOptsCbData*)pcb->pCancelData)->cb);
}

/*===========================================================================

Function: LinkAppGetSrvOptsCb

Description:
   Links app callback to system for ISMSBCConfig_GetServiceOpts
 
===========================================================================*/
static int LinkAppGetSrvOptsCb(AEECallback * pcb, SMSAppGetSrvOptsCbData ** ppAppGetSrvOptsCbData)
{
   int nErr = SUCCESS;
   AEECallback * pcbLinked = NULL;
   SMSAppGetSrvOptsCbData * pAppGetSrvOptsCbData = NULL;

   if ((pcbLinked = AEE_LinkSysCallbackEx(pcb, sizeof(SMSAppGetSrvOptsCbData))) == NULL)
   {
      nErr = ENOMEMORY;
   }
   else
   {
      pAppGetSrvOptsCbData = (SMSAppGetSrvOptsCbData *)(pcbLinked + 1);
      pAppGetSrvOptsCbData->pac = AEE_GetAppContext();
      pAppGetSrvOptsCbData->pcb = pcbLinked;

      pcbLinked->pfnCancel = (PFNCBCANCEL)ReleaseAppGetSrvOptsCb;
      pcbLinked->pCancelData = pAppGetSrvOptsCbData;

      *ppAppGetSrvOptsCbData = pAppGetSrvOptsCbData;

      // Create kernel callback to be passed to OEM layer.
      CALLBACK_Init(&pAppGetSrvOptsCbData->cb, InvokeAppGetSrvOptsCb, pAppGetSrvOptsCbData);
   }
   return nErr;
}

/*===========================================================================

Function: InvokeAppGetAllSrvOptsCb

Description:
   Invokes app callback

===========================================================================*/
static void InvokeAppGetAllSrvOptsCb(void * pData)
{
   SMSAppGetAllSrvOptsCbData *pAppGetAllSrvOptsCbData = (SMSAppGetAllSrvOptsCbData *)pData;
   
   FARF(SMS, ("==>  CSMSBCConfig_SrvInfoCb"));

   if ((*pAppGetAllSrvOptsCbData->pnErr == SUCCESS) &&
       (*pAppGetAllSrvOptsCbData->pnCount != 0))
   {   
      OEMSMSBCSrvInfo *pSrvInfo = NULL;

      if (((pSrvInfo = MALLOC(*pAppGetAllSrvOptsCbData->pnCount*sizeof(OEMSMSBCSrvInfo))) != NULL) &&
          ((*pAppGetAllSrvOptsCbData->pppi = MALLOC(*pAppGetAllSrvOptsCbData->pnCount*sizeof(ISMSBCSrvOpts*))) != NULL))
      {
         int nErr = SUCCESS;
         uint32 i = 0;
         ISMSBCSrvOpts ** ppi = *pAppGetAllSrvOptsCbData->pppi;

         // Pass through to OEM
         OEMSMSBCConfig_FillServiceInfo(pSrvInfo, *pAppGetAllSrvOptsCbData->pnCount*sizeof(OEMSMSBCSrvInfo));

         for (i = 0; 
              ((i < *pAppGetAllSrvOptsCbData->pnCount) && (*pAppGetAllSrvOptsCbData->pnErr == SUCCESS)); 
              i++)
         {
            if ((nErr = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_SMSBCSRVOPTS, (void **)&ppi[i])) == SUCCESS)
            {
               if (PackSMSBCSrvOpts(&pSrvInfo[i], ppi[i]) != TRUE)
               {
                  *pAppGetAllSrvOptsCbData->pnErr = EBADPARM;
               }
            }
            else
            {
               *pAppGetAllSrvOptsCbData->pnErr = nErr;
            }
         }

         if (*pAppGetAllSrvOptsCbData->pnErr != SUCCESS)
         {
            for (i = 0; i < *pAppGetAllSrvOptsCbData->pnCount; i++)
            {
               if (ppi[i])
               {
                  ISMSBCSRVOPTS_Release(ppi[i]);
               }
            }
            if (*pAppGetAllSrvOptsCbData->pppi)
            {
               FREE(*pAppGetAllSrvOptsCbData->pppi);
               *pAppGetAllSrvOptsCbData->pppi = NULL;
            }
         }
      }
      else
      {
         *pAppGetAllSrvOptsCbData->pnErr = ENOMEMORY;
      }

      FREEIF(pSrvInfo);
   }

   // Finally Resume the Application 
   AEE_ResumeCallback(pAppGetAllSrvOptsCbData->pcb, pAppGetAllSrvOptsCbData->pac);
}

/*===========================================================================

Function: ReleaseAppGetAllSrvOptsCb

Description:
   Releases app callback for ISMSBCConfig_GetAllServiceOpts
 
===========================================================================*/
static void ReleaseAppGetAllSrvOptsCb(AEECallback * pcb)
{   
   if(pcb->pfnCancel != ReleaseAppGetAllSrvOptsCb)
      return;

   CALLBACK_Cancel(&((SMSAppGetAllSrvOptsCbData*)pcb->pCancelData)->cb);
}

/*===========================================================================

Function: LinkAppGetAllSrvOptsCb

Description:
   Links app callback to system for ISMSBCConfig_GetAllServiceOpts
 
===========================================================================*/
static int LinkAppGetAllSrvOptsCb(AEECallback * pcb, SMSAppGetAllSrvOptsCbData ** ppAppGetAllServiceOptsCbData)
{
   int nErr = SUCCESS;
   AEECallback * pcbLinked = NULL;
   SMSAppGetAllSrvOptsCbData * pAppGetAllServiceOptsCbData = NULL;

   if ((pcbLinked = AEE_LinkSysCallbackEx(pcb, sizeof(SMSAppGetAllSrvOptsCbData))) == NULL)
   {
      nErr = ENOMEMORY;
   }
   else
   {
      pAppGetAllServiceOptsCbData = (SMSAppGetAllSrvOptsCbData *)(pcbLinked + 1);
      pAppGetAllServiceOptsCbData->pac = AEE_GetAppContext();
      pAppGetAllServiceOptsCbData->pcb = pcbLinked;

      pcbLinked->pfnCancel = (PFNCBCANCEL)ReleaseAppGetAllSrvOptsCb;
      pcbLinked->pCancelData = pAppGetAllServiceOptsCbData;

      *ppAppGetAllServiceOptsCbData = pAppGetAllServiceOptsCbData;

      // Create kernel callback to be passed to OEM layer.
      CALLBACK_Init(&pAppGetAllServiceOptsCbData->cb, InvokeAppGetAllSrvOptsCb, pAppGetAllServiceOptsCbData);
   }
   return nErr;
}

/*===========================================================================

Function: ISMSMsgToOEMMsg

Description:
   Converts ISMSMsg to OEMMsgData and OEMMsgPayload
 
===========================================================================*/
static int ISMSMsgToOEMMsg(ISMSMsg * pISMSMsg, OEMMsgData ** ppMsgData, OEMMsgPayload ** ppMsgPayload)
{
   SMSMsgOpt smo;
   AEEDeviceInfo di;
   OEMMsgData * pMsgData = NULL;
   OEMMsgPayload * pMsgPayload = NULL;
   int nMaxPayloadLength, nEncoding, nErr = EFAILED, nPayloadBytes = 0, nUDHLength = 0, nAlphaIDLength = 0, nBytes = 0;
   byte * pBuffer = NULL;
   char * pszBrewPrefix = NULL;
   AEESMSEncType nSMSEncoding;

   if (OEM_GetConfig(CFGI_SMS_MAX_PAYLOAD_LENGTH, &nMaxPayloadLength, sizeof(nMaxPayloadLength)) != SUCCESS)
   {   
      FARF(SMS, ("Max Payload Length Not Configured"));
      nErr = EFAILED;
      goto Done;
   }
   
   // Get MO SMS Encoding
   if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_MOSMS_ENCODING, &smo) == SUCCESS)
      nSMSEncoding = (uint32)smo.pVal;
   else
   {
      // Get default MO SMS Encoding
      if (OEM_GetConfig(CFGI_SMS_DEFAULT_MO_ENC, &nSMSEncoding, sizeof(nSMSEncoding)) != SUCCESS)
      {   
         FARF(SMS, ("MO SMS Encoding Not Specified"));
         nErr = EFAILED;
         goto Done;
      }
   }

   // Calculate size required for UDH data
   UnpackUDHMsgOpts(pISMSMsg, &nUDHLength, NULL);

   // Get Alpha ID Length
   if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_ALPHA_ID, &smo) == SUCCESS)
   {
      nAlphaIDLength = MSGOPT_GETUDHBUFFERLENGTH(smo.pVal);
   }

   if (((pMsgData = MALLOC(sizeof(OEMMsgData) + nUDHLength + nAlphaIDLength)) == NULL) ||
       ((pMsgPayload = MALLOC(sizeof(OEMMsgPayload) + MAX_MSG_LENGTH)) == NULL))
   {
      nErr = ENOMEMORY;
      goto Done;
   }

   // Set bytes for UDH data and Alpha ID
   pMsgData->nUDHLength = nUDHLength;
   pMsgData->nAlphaIDLength = nAlphaIDLength;

   MEMSET(&di, 0, sizeof(di));

   // Is it a loopback message
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_LOOPBACK_MSG, &smo)) == SUCCESS)
      pMsgData->bIsLoopbackMsg = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bIsLoopbackMsg = FALSE; // Assume that message isn't loopback by default

   // Get Destination Device/Email Address
   if (((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_TO_DEVICE_SZ, &smo)) == SUCCESS) ||
       ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_TO_EMAIL_SZ, &smo)) == SUCCESS))
   {
      STRLCPY(pMsgData->szMOFromMTTo, (char*)smo.pVal, sizeof(pMsgData->szMOFromMTTo)); 
      pMsgData->bIsMOSMS = TRUE;

      // Get Destination Type
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_TO_NUMBER_TYPE, &smo)) == SUCCESS)
         pMsgData->dwMOFromMTToType = (uint32)smo.pVal;
      else
         pMsgData->dwMOFromMTToType = AEESMS_NUMBER_UNKNOWN;

      // Get Destination Plan
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_TO_NUMBER_PLAN, &smo)) == SUCCESS)
         pMsgData->dwMOFromMTToPlan = (uint32)smo.pVal;
      else
         pMsgData->dwMOFromMTToPlan = AEESMS_NUMBER_PLAN_UNKNOWN;
   }
   else if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_FROM_DEVICE_SZ, &smo)) == SUCCESS)
   {
      STRLCPY(pMsgData->szMOFromMTTo, (char*)smo.pVal, sizeof(pMsgData->szMOFromMTTo));      
      pMsgData->bIsMOSMS = FALSE;

      // Get Source Type
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_FROM_NUMBER_TYPE, &smo)) == SUCCESS)
         pMsgData->dwMOFromMTToType = (uint32)smo.pVal;
      else
         pMsgData->dwMOFromMTToType = AEESMS_NUMBER_UNKNOWN;

      // Get Source Plan
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_FROM_NUMBER_PLAN, &smo)) == SUCCESS)
         pMsgData->dwMOFromMTToPlan = (uint32)smo.pVal;
      else
         pMsgData->dwMOFromMTToPlan = AEESMS_NUMBER_PLAN_UNKNOWN;
   }
   else
   {
      FARF(SMS, ("Message doesn't contain source/destination"));
   }

   // See if there is a destination class or handler
   if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BREW_CLASS, &smo) == SUCCESS)
   {
      char szBuffer[32];
      nBytes = SNPRINTF(szBuffer, sizeof(szBuffer), "%s%.8x:", BREW_SMS_PREFIX, (AEECLSID)smo.pVal);
      if ((pszBrewPrefix = MALLOC(nBytes + 1)) != NULL)
         STRLCPY(pszBrewPrefix, szBuffer, nBytes + 1);
      else
      {
         nErr = ENOMEMORY;
         goto Done;
      }
      pMsgData->bIsBREWDirectedSMS = TRUE;
   }
   else if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BREW_HANDLER_SZ, &smo) == SUCCESS)
   {
      boolean bSpace = FALSE;

      // Account for //%s:
      nBytes = STRLEN((char*)smo.pVal) + 3;
      
      if (nBytes & 0x1)
      {
         bSpace = TRUE;
         nBytes++;
      }
      
      if ((pszBrewPrefix = MALLOC(nBytes + 1)) != NULL)
      {
         if (bSpace)
            SNPRINTF(pszBrewPrefix, nBytes + 1, "// %s:", (char*)smo.pVal);
         else
            SNPRINTF(pszBrewPrefix, nBytes + 1, "//%s:", (char*)smo.pVal);
      }
      else
      {
         nErr = ENOMEMORY;
         goto Done;
      }
      pMsgData->bIsBREWDirectedSMS = TRUE;
   }

   // Get Message Payload Encoding
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_ENCODING, &smo)) == SUCCESS)
      nEncoding = (uint32)smo.pVal;
   else
   {
      // Assume Device Encoding by default
      di.wStructSize = sizeof(di);
      AEE_GetDeviceInfo(&di);
      nEncoding = di.wEncoding; 
   }
   
   // Get Message Payload
   if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_SZ, &smo) == SUCCESS)
   {
      pBuffer = (byte*)smo.pVal;
      nPayloadBytes = STRLEN((char*)smo.pVal);
   }
   else if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_WSZ, &smo) == SUCCESS)
   {
      pBuffer = (byte*)smo.pVal;
      nPayloadBytes = WSTRLEN((AECHAR*)smo.pVal)*sizeof(AECHAR);
   }
   else if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_BINARY, &smo) == SUCCESS)
   {
      pBuffer = (byte*)WEBOPT_GETVARBUFFERDATA(smo.pVal);
      nPayloadBytes = WEBOPT_GETVARBUFFERLENGTH(smo.pVal);
   }
   else
   {
      FARF(SMS, ("Message doesn't contain payload"));
   }

   if (((nErr = GetEncodedMsgPayload(pszBrewPrefix, pBuffer, nPayloadBytes, nEncoding, nSMSEncoding, NULL, &pMsgPayload->nBytes, &pMsgPayload->nPadding)) == SUCCESS) &&
       ((nErr = (nMaxPayloadLength >= pMsgPayload->nBytes ? SUCCESS : EFAILED)) == SUCCESS) &&
       ((nErr = GetEncodedMsgPayload(pszBrewPrefix, pBuffer, nPayloadBytes, nEncoding, nSMSEncoding, pMsgPayload->data, &pMsgPayload->nBytes, &pMsgPayload->nPadding)) == SUCCESS))
   {
      pMsgPayload->nEncoding = nSMSEncoding;
   }
   else
      goto Done;
      
   // Get callback number or SC Address
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_CALLBACK_SZ, &smo)) == SUCCESS)
   {      
      pMsgData->bIsSCAddress = FALSE;
      STRLCPY(pMsgData->szCallback, (char*)smo.pVal, sizeof(pMsgData->szCallback));

      // Get Callback Type
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_CALLBACK_NUMBER_TYPE, &smo)) == SUCCESS)
         pMsgData->dwCallbackType = (uint32)smo.pVal;
      else
         pMsgData->dwCallbackType = AEESMS_NUMBER_UNKNOWN;

      // Get Callback Plan
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_CALLBACK_NUMBER_PLAN, &smo)) == SUCCESS)
         pMsgData->dwCallbackPlan = (uint32)smo.pVal;
      else
         pMsgData->dwCallbackPlan = AEESMS_NUMBER_PLAN_UNKNOWN;
   }
   else if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_SC_ADDRESS_SZ, &smo)) == SUCCESS)
   {
      pMsgData->bIsSCAddress = TRUE;
      STRLCPY(pMsgData->szCallback, (char*)smo.pVal, sizeof(pMsgData->szCallback));

      // Get SCAddress Type
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_SC_ADDRESS_NUMBER_TYPE, &smo)) == SUCCESS)
         pMsgData->dwCallbackType = (uint32)smo.pVal;
      else
         pMsgData->dwCallbackType = AEESMS_NUMBER_UNKNOWN;

      // Get SCAddress Plan
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_SC_ADDRESS_NUMBER_PLAN, &smo)) == SUCCESS)
         pMsgData->dwCallbackPlan = (uint32)smo.pVal;
      else
         pMsgData->dwCallbackPlan = AEESMS_NUMBER_PLAN_UNKNOWN;
   }

   // Get Message Language
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_LANG, &smo)) == SUCCESS)
   {
      pMsgData->dwLang = (uint32)smo.pVal;
      pMsgData->bLangPresent = TRUE;
   }
   else
   {
      pMsgData->bLangPresent = FALSE;
   }

   // Get Protocol ID
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_GW_PID, &smo)) == SUCCESS)
      pMsgData->dwPID = (uint32)smo.pVal;
   else
   {
      // Assign Default PID
      pMsgData->dwPID = AEESMS_PID_DEFAULT;
   }

   // Get Message Class
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_CLASS, &smo)) == SUCCESS)
      pMsgData->dwMsgClass = (uint32)smo.pVal;
   else
   {
      // Assign Default Message Class
      pMsgData->dwMsgClass = AEESMS_CLASS_NONE;
   }

   // Get Message Priority
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PRIORITY, &smo)) == SUCCESS)
   {
      pMsgData->nPriority = (uint32)smo.pVal;
      pMsgData->bPriorityPresent = TRUE;
   }
   else
   {
      pMsgData->bPriorityPresent = FALSE;
   }

   // Get Message Absolute Deferred Delivery
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_ABSOULTE_DEFERREDTIME, &smo)) == SUCCESS)
      pMsgData->dwAbsoluteDeferredValidity = JULIANTOSECONDS((JulianType*)smo.pVal);
   else
      pMsgData->dwAbsoluteDeferredValidity = 0;

   // Get Message Relative Deferred Delivery
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_RELATIVE_DEFERREDTIME, &smo)) == SUCCESS)
      pMsgData->dwRelativeDeferredValidity = (uint32)smo.pVal;
   else
      pMsgData->dwRelativeDeferredValidity = 0xFFFFFFFF; // No Relative Deferred Time

   // Get Message Privacy
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PRIVACY, &smo)) == SUCCESS)
   {
      pMsgData->nPrivacy = (uint32)smo.pVal;
      pMsgData->bPrivacyPresent = TRUE;
   }
   else
   {
      pMsgData->bPrivacyPresent = FALSE;
   }

   // Get Message Waiting Type
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_MSG_WAITING_TYPE, &smo)) == SUCCESS)
      pMsgData->dwMsgWaitingType = (uint32)smo.pVal;
   else
      pMsgData->dwMsgWaitingType = AEESMS_MSGWAITINGTYPE_NONE; // Assume None by default

   // Get Message Waiting Kind
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_MSG_WAITING_KIND, &smo)) == SUCCESS)
      pMsgData->dwMsgWaitingKind = (uint32)smo.pVal;
   else
      pMsgData->dwMsgWaitingKind = AEESMS_MSGWAITINGKIND_NONE; // Assume None by default

   // Get Raw DCS
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_RAW_DCS_DATA, &smo)) == SUCCESS)
      pMsgData->dwRawDCS = (uint32)((uint32)smo.pVal & 0x000000FF);
   else
      pMsgData->dwRawDCS = 0xFFFFFFFF; // Assume 0xFFFFFFFF by default

   // Get TP-DCS compressed status
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_COMPRESSED_MSG, &smo)) == SUCCESS)
      pMsgData->bIsCompressed = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bIsCompressed = FALSE; // Assume that message isn't compressed by default

   // Get TP-MMS status
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_MORE_MSG_TO_SEND, &smo)) == SUCCESS)
      pMsgData->bHasMoreMessageToSend = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bHasMoreMessageToSend = FALSE; // Assume that TP-MMS isn't set by default

   // Get Message Delivery Ack Request Status
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_REQUEST_DL_ACK, &smo)) == SUCCESS)
      pMsgData->bRequestDLAck = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bRequestDLAck = FALSE; // Assume that DL ack isn't requested by default

   // Get Message User Ack Request Status
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_REQUEST_USER_ACK, &smo)) == SUCCESS)
      pMsgData->bRequestUserAck = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bRequestUserAck = FALSE; // Assume that User ack isn't requested by default

   // Get Message Read Ack Request Status
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_REQUEST_READ_ACK, &smo)) == SUCCESS)
      pMsgData->bRequestReadAck = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bRequestReadAck = FALSE; // Assume that Read ack isn't requested by default
   
   // Get Message DL Ack Status
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_DL_ACK, &smo)) == SUCCESS)
      pMsgData->bIsDLAck = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bIsDLAck = FALSE; // Assume that message isn't Delivery Ack by default

   // Get Message User Ack Status
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_USER_ACK, &smo)) == SUCCESS)
      pMsgData->bIsUserAck = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bIsUserAck = FALSE; // Assume that message isn't User Ack by default

   // Get Message Read Ack Status
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_READ_ACK, &smo)) == SUCCESS)
      pMsgData->bIsReadAck = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bIsReadAck = FALSE; // Assume that message isn't Read Ack by default

   // Check if it's a cancellation Request
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_CANCELLATION_REQUEST, &smo)) == SUCCESS)
      pMsgData->bIsCancelRequest = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bIsCancelRequest = FALSE; // Assume that it's not a cancellation request by default

   // Get Message Reply Path Status
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_REPLY_PATH_PRESENT, &smo)) == SUCCESS)
      pMsgData->bIsReplyPathPresent = ((uint32)smo.pVal ? TRUE : FALSE);
   else
      pMsgData->bIsReplyPathPresent = FALSE; // Assume that Replay Path isn't present by default

   // Get Message Tag
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_TAG, &smo)) == SUCCESS)
   {
      pMsgData->dwTag = (uint32)smo.pVal;
      if ((AEESMS_TAG_MO_NOT_SENT == pMsgData->dwTag) ||
          (AEESMS_TAG_MO_TEMPLATE == pMsgData->dwTag))
      {
         pMsgData->bIsMOSMS = TRUE;
      }
   }
   else
      pMsgData->dwTag = (pMsgData->bIsMOSMS ? AEESMS_TAG_MO_NOT_SENT : AEESMS_TAG_MT_NOT_READ);

   if (AEESMS_TAG_STATUS_RPT == pMsgData->dwTag)
   {
      // Get message discharge time
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_DISCHARGETIME, &smo)) == SUCCESS)
      {
         pMsgData->dwMsgDischargeTime = JULIANTOSECONDS((JulianType*)smo.pVal);
         pMsgData->nDischargeTimeZone = ((AEESMSGWTime*)smo.pVal)->nTimeZone;
      }
      else
         pMsgData->dwMsgDischargeTime = pMsgData->nDischargeTimeZone = 0;

      // Get report status
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_REPORT_STATUS, &smo)) == SUCCESS)
         pMsgData->dwReportStatus = (uint32)smo.pVal;
      else
         pMsgData->dwReportStatus = 0;
   }

   // Get message type
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_MSGTYPE, &smo)) == SUCCESS)
      pMsgData->ts = Xlate_MTtoTS((int)smo.pVal);
   else
      pMsgData->ts = (pMsgData->nUDHLength ? SMS_TELESERVICE_WEMT : SMS_TELESERVICE_CMT_95);

   // Get Message ID
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_MSG_ID, &smo)) == SUCCESS)
      pMsgData->dwMsgID = (uint32)smo.pVal;
   else
      pMsgData->dwMsgID = 0;

   // The parameter, validity, can be set for MT messages also.
   // Although that would not be too useful, to keep compliant with IS-637
   // these should not be ignored by ISMS if present
   {
      // Get Relative Validity
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_RELATIVE_VALIDITYTIME, &smo)) == SUCCESS)
      {
         pMsgData->dwRelativeValidity = (uint32)smo.pVal;
      }
      else {
         pMsgData->dwRelativeValidity = 0xFFFFFFFF; // No validity
      }

      // Get Absolute Validity
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_ABSOLUTE_VALIDITY, &smo)) == SUCCESS)
      {
         pMsgData->dwAbsoluteValidityTimeStamp = JULIANTOSECONDS((JulianType*)smo.pVal);
         pMsgData->nAbsoluteValidityTimeStampZone = ((AEESMSGWTime*)smo.pVal)->nTimeZone;
      }
   }

   // Same for timestamp, can be for MO as well as MT, technically...
   {
      // Get message time stamp
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_GW_TIMESTAMP, &smo)) == SUCCESS)
      {
         pMsgData->dwTimeStamp = JULIANTOSECONDS((JulianType*)smo.pVal);
         pMsgData->nTimeStampZone = ((AEESMSGWTime*)smo.pVal)->nTimeZone;
      }
      else if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_TIMESTAMP, &smo)) == SUCCESS)
      {
         pMsgData->dwTimeStamp = JULIANTOSECONDS((JulianType*)smo.pVal);
      }
      else
         pMsgData->dwTimeStamp = pMsgData->nTimeStampZone = 0;
   }

   if (!pMsgData->bIsMOSMS)
   {
      // Get message count for voice mail indication
      if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_NUM, &smo)) == SUCCESS)
         pMsgData->nNumMsgs = (int)smo.pVal;
      else
         pMsgData->nNumMsgs = -1;
   }

   // Get Message display mode
   if ((nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_DISPLAY_MODE, &smo)) == SUCCESS)
   {
      pMsgData->dwMessageDisplayMode = (uint32)smo.pVal;
   }
   else
   {
      pMsgData->dwMessageDisplayMode = AEESMS_DISPLAY_MODE_NONE; 
   }

   // No Need to Unpack the MSGOPT_TPDU_DELIVER and MSGOPT_TPDU_STATUS_REPORT Message Options
   // as they are specific to MT messages only

   // Get UDH data
   UnpackUDHMsgOpts(pISMSMsg, &pMsgData->nUDHLength, pMsgData->data);
   
   // Get Alpha ID
   if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_ALPHA_ID, &smo) == SUCCESS)
   {
      MEMCPY(pMsgData->data + pMsgData->nUDHLength, 
             MSGOPT_GETUDHBUFFERDATA(smo.pVal), 
             MSGOPT_GETUDHBUFFERLENGTH(smo.pVal));
   }

   // Check if it's a GW command and fetch corresponding data
   if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_GW_COMMAND, &smo) == SUCCESS) {
      pMsgData->dwGWCommand = (uint32)smo.pVal;

      // Check for command data
      if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_COMMAND_DATA, &smo) == SUCCESS) {
         pMsgPayload->nBytes = MSGOPT_GETUDHBUFFERLENGTH(smo.pVal);
         MEMCPY(pMsgPayload->data, 
                MSGOPT_GETUDHBUFFERDATA(smo.pVal), 
                MSGOPT_GETUDHBUFFERLENGTH(smo.pVal));
      }

      // Check for command data UDHI
      if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_GW_COMMAND_UDHI, &smo) == SUCCESS) {
         pMsgPayload->nEncoding = (int)smo.pVal;
      }

      // Check for command message number
      if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_GW_COMMAND_MN, &smo) == SUCCESS) {
         pMsgData->dwMsgID |= (((uint32)smo.pVal) << 16);
      }
   }
   else {
      pMsgData->dwGWCommand = 0xFFFFFFFF;
   }

   if (AEESMS_TYPE_BROADCAST == Xlate_TStoMT(pMsgData->ts))
   {
      // Get BC msg options
      if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BCTYPE, &smo) == SUCCESS)
      {
         pMsgData->bIsGWBroadcast = FALSE;
         pMsgData->dwSrvID = (uint32) smo.pVal;
      }
      else if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BC_SERVICE_ID, &smo) == SUCCESS)
      {
         pMsgData->bIsGWBroadcast = TRUE;
         pMsgData->dwSrvID = (uint32) smo.pVal;
   
         // Fill up the GW CB Message Options
         if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BC_PAGE_NUMBER, &smo) == SUCCESS)
         {
               pMsgData->dwGWCBPageNumber = (uint32) smo.pVal;
         }
   
         if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BC_TOTAL_PAGES, &smo) == SUCCESS)
         {
               pMsgData->dwGWCBTotalPages = (uint32) smo.pVal;
         }
   
         if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BC_GEO_SCOPE, &smo) == SUCCESS)
         {
               pMsgData->dwGWCBGeoScope = (uint32) smo.pVal;
         }
         
         if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BC_MESSAGE_CODE, &smo) == SUCCESS)
         {
               pMsgData->dwGWCBMessageCode = (uint32) smo.pVal;
         }
   
         if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_BC_UPDATE_NUMBER, &smo) == SUCCESS)
         {
               pMsgData->dwGWCBUpdateNumber = (uint32) smo.pVal;
         }
      }
   }
   
   nErr = SUCCESS;

Done:
   FREEIF(pszBrewPrefix);

   if (nErr == SUCCESS)
   {   
      *ppMsgData = pMsgData;
      *ppMsgPayload = pMsgPayload;
   }
   else
   {
      FREEIF(pMsgData);
      FREEIF(pMsgPayload);
   }
   return nErr;
}

/*===========================================================================

Function: OEMMsgToISMSMsg

Description:
   Converts OEMMsgData and OEMMsgPayload to ISMSMsg
 
===========================================================================*/
static int OEMMsgToISMSMsg(OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, ISMSMsg ** ppISMSMsg)
{
   int nErr;
   SMSMsgOpt smo[2];
   ISMSMsg * pISMSMsg = NULL;

   if (!pMsgData && !pMsgPayload)
      return EBADPARM;

   if ((nErr = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_SMSMSG, (void**)&pISMSMsg)) != SUCCESS)
      return nErr;
      
   MEMSET(smo, 0, sizeof(smo));

   smo[1].nId = MSGOPT_END;

   // Set for copying of options
   smo[0].nId = MSGOPT_COPYOPTS;
   smo[0].pVal = (void *)TRUE;
      
   ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

   if (pMsgData)
   {     
      AEESMSGWTime jt;

      // Set if loopback 
      smo[0].nId = MSGOPT_LOOPBACK_MSG;
      smo[0].pVal = (void*)pMsgData->bIsLoopbackMsg;
      
      ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

      // If its a loopback, add the sending applets class id
      if ( pMsgData->bIsLoopbackMsg )
      {
         smo[0].nId = MSGOPT_LOOPBACK_FROM_APPLET;
         smo[0].pVal = (void*)pMsgData->nFromAppletCls;

         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set message language (if present)
      if ( pMsgData->bLangPresent )
      {
         smo[0].nId = MSGOPT_LANG;
         smo[0].pVal = (void*)pMsgData->dwLang;

         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set PID
      smo[0].nId = MSGOPT_GW_PID;
      smo[0].pVal = (void*)pMsgData->dwPID;
      
      ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

      // Set Message Class
      smo[0].nId = MSGOPT_CLASS;
      smo[0].pVal = (void*)pMsgData->dwMsgClass;
      
      ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

      // Set message priority (if present)
      if ( pMsgData->bPriorityPresent )
      {
         smo[0].nId = MSGOPT_PRIORITY;
         smo[0].pVal = (void*)pMsgData->nPriority;

         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set Message Absolute Deferred Delivery
      if (pMsgData->dwAbsoluteDeferredValidity) {
         GETJULIANDATE(pMsgData->dwAbsoluteDeferredValidity, (JulianType*)&jt);
         smo[0].nId = MSGOPT_ABSOULTE_DEFERREDTIME;
         smo[0].pVal = (void*)&jt;
         
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set Message Relative Deferred Delivery
      if (0xFFFFFFFF != pMsgData->dwRelativeDeferredValidity) {
         smo[0].nId = MSGOPT_RELATIVE_DEFERREDTIME;
         smo[0].pVal = (void*)pMsgData->dwRelativeDeferredValidity;
         
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set message privacy (if present)
      if ( pMsgData->bPrivacyPresent )
      {
         smo[0].nId = MSGOPT_PRIVACY;
         smo[0].pVal = (void*)pMsgData->nPrivacy;

         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set message waiting type
      smo[0].nId = MSGOPT_MSG_WAITING_TYPE;
      smo[0].pVal = (void*)pMsgData->dwMsgWaitingType;
      
      ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

      // Set message waiting kind
      smo[0].nId = MSGOPT_MSG_WAITING_KIND;
      smo[0].pVal = (void*)pMsgData->dwMsgWaitingKind;
      
      ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

      if (0xFFFFFFFF != pMsgData->dwRawDCS)
      {
         // Set message waiting kind
         smo[0].nId = MSGOPT_RAW_DCS_DATA;
         smo[0].pVal = (void*)(pMsgData->dwRawDCS & 0xFF);
         
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set TP-DCS compressed status
      smo[0].nId = MSGOPT_COMPRESSED_MSG;
      smo[0].pVal = (void*)pMsgData->bIsCompressed;
      
      ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

      // Set TP-MMS status
      smo[0].nId = MSGOPT_MORE_MSG_TO_SEND;
      smo[0].pVal = (void*)pMsgData->bHasMoreMessageToSend;
      
      ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

      // Set callback number/SC address
      if (*pMsgData->szCallback)
      {
         smo[0].nId = pMsgData->bIsSCAddress ? MSGOPT_SC_ADDRESS_SZ : MSGOPT_CALLBACK_SZ;
         smo[0].pVal = (void*)pMsgData->szCallback;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

         smo[0].nId = pMsgData->bIsSCAddress ? MSGOPT_SC_ADDRESS_NUMBER_TYPE : MSGOPT_CALLBACK_NUMBER_TYPE;
         smo[0].pVal = (void*)pMsgData->dwCallbackType;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

         smo[0].nId = pMsgData->bIsSCAddress ? MSGOPT_SC_ADDRESS_NUMBER_PLAN : MSGOPT_CALLBACK_NUMBER_PLAN;
         smo[0].pVal = (void*)pMsgData->dwCallbackPlan;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set sender address
      if (*pMsgData->szMOFromMTTo)
      {
         smo[0].nId = (pMsgData->bIsMOSMS ? MSGOPT_TO_DEVICE_SZ : MSGOPT_FROM_DEVICE_SZ);
         smo[0].pVal = (void*)pMsgData->szMOFromMTTo;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

         smo[0].nId = (pMsgData->bIsMOSMS ? MSGOPT_TO_NUMBER_TYPE : MSGOPT_FROM_NUMBER_TYPE);
         smo[0].pVal = (void*)pMsgData->dwMOFromMTToType;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

         smo[0].nId = (pMsgData->bIsMOSMS ? MSGOPT_TO_NUMBER_PLAN : MSGOPT_FROM_NUMBER_PLAN);
         smo[0].pVal = (void*)pMsgData->dwMOFromMTToPlan;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set delivery ack request
      if (pMsgData->bRequestDLAck)
      {
         smo[0].nId = MSGOPT_REQUEST_DL_ACK;
         smo[0].pVal = (void*)pMsgData->bRequestDLAck;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set user ack request
      if (pMsgData->bRequestUserAck)
      {
         smo[0].nId = MSGOPT_REQUEST_USER_ACK;
         smo[0].pVal = (void*)pMsgData->bRequestUserAck;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set read ack request
      if (pMsgData->bRequestReadAck)
      {
         smo[0].nId = MSGOPT_REQUEST_READ_ACK;
         smo[0].pVal = (void*)pMsgData->bRequestReadAck;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set Delivery ack
      if (pMsgData->bIsDLAck)
      {
         smo[0].nId = MSGOPT_DL_ACK;
         smo[0].pVal = (void*)pMsgData->bIsDLAck;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set user ack
      if (pMsgData->bIsUserAck)
      {
         smo[0].nId = MSGOPT_USER_ACK;
         smo[0].pVal = (void*)pMsgData->bIsUserAck;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set read ack
      if (pMsgData->bIsReadAck)
      {
         smo[0].nId = MSGOPT_READ_ACK;
         smo[0].pVal = (void*)pMsgData->bIsReadAck;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Specify if it's cancellation request
      if (pMsgData->bIsCancelRequest)
      {
         smo[0].nId = MSGOPT_CANCELLATION_REQUEST;
         smo[0].pVal = (void*)pMsgData->bIsCancelRequest;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set Reply Path Present
      if (pMsgData->bIsReplyPathPresent)
      {
         smo[0].nId = MSGOPT_REPLY_PATH_PRESENT;
         smo[0].pVal = (void*)pMsgData->bIsReplyPathPresent;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set message tag
      smo[0].nId = MSGOPT_TAG;
      smo[0].pVal = (void*)pMsgData->dwTag;
      
      ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

      if (AEESMS_TAG_STATUS_RPT == pMsgData->dwTag)
      {
         // Set message discharge time
         if (pMsgData->dwMsgDischargeTime)
         {
            MEMSET(&jt, 0, sizeof(jt));
            GETJULIANDATE(pMsgData->dwMsgDischargeTime, (JulianType*)&jt);            
            jt.nTimeZone = (sint15)pMsgData->nDischargeTimeZone;
            smo[0].nId = MSGOPT_DISCHARGETIME;
            smo[0].pVal = (void*)&jt;
         
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
         }

         // Set report status
         smo[0].nId = MSGOPT_REPORT_STATUS;
         smo[0].pVal = (void*)pMsgData->dwReportStatus;
         
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set user data headers
      PackUDHMsgOpts(pMsgData->data, pMsgData->nUDHLength, pISMSMsg);

      // Set message type
      smo[0].nId = MSGOPT_MSGTYPE;
      smo[0].pVal = (void*)Xlate_TStoMT(pMsgData->ts);
      
      ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

      // Set message ID
      smo[0].nId = MSGOPT_MSG_ID;
      smo[0].pVal = (void*)(pMsgData->dwMsgID & 0x0000FFFF);
      
      ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

      // These parameters can be set for MT messages also,
      // Although that would not be too useful, to keep compliant with IS-637
      // these should not be ignored by ISMS if present
      {
         // Set Relative Validity
         if (0xFFFFFFFF != pMsgData->dwRelativeValidity) {
            smo[0].nId = MSGOPT_RELATIVE_VALIDITYTIME;
            smo[0].pVal = (void*)pMsgData->dwRelativeValidity;
            
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
         }

         // Absolute Validity
         if (pMsgData->dwAbsoluteValidityTimeStamp) {
            // Specify message absolute validity
            MEMSET(&jt, 0, sizeof(jt));
            GETJULIANDATE(pMsgData->dwAbsoluteValidityTimeStamp, (JulianType*)&jt);
            jt.nTimeZone = (sint15)pMsgData->nAbsoluteValidityTimeStampZone;

            smo[0].nId = MSGOPT_ABSOLUTE_VALIDITY;
            smo[0].pVal = (void*)&jt;
            
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
         }
      }

      // Same for timestamp, can be for both MO and MT...
      {
         if (pMsgData->dwTimeStamp) {
            // Set message time stamp
            MEMSET(&jt, 0, sizeof(jt));
            GETJULIANDATE(pMsgData->dwTimeStamp, (JulianType*)&jt);
            jt.nTimeZone = (sint15)pMsgData->nTimeStampZone;

            smo[0].nId = MSGOPT_TIMESTAMP;
            smo[0].pVal = (void*)&jt;
            
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

            smo[0].nId = MSGOPT_GW_TIMESTAMP;
            smo[0].pVal = (void*)&jt;
            
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
         }
      }

      if (!pMsgData->bIsMOSMS)
      {
         // Set message count for voice mail indication
         if (AEESMS_TYPE_VOICE_MAIL_NOTIFICATION == Xlate_TStoMT(pMsgData->ts))
         {
            if (pMsgData->nNumMsgs >= 0)
            {
               smo[0].nId = MSGOPT_NUM;
               smo[0].pVal = (void*)pMsgData->nNumMsgs;
      
               ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            }
         }
      }

      // Check if the Calling Application has PL_SYSTEM privilege to access Raw TPDU
      if (!pMsgData->bIsMOSMS &&
          pMsgData->nRawTPDULength &&
          ISHELL_CheckPrivLevel(AEE_GetShell(), PL_SYSTEM, TRUE))
      {
         char *pBuffer = NULL;
         if ((pBuffer = MALLOC(pMsgData->nRawTPDULength + sizeof(AECHAR))) != NULL)
         {
            WEBOPT_SETVARBUFFER(pBuffer, pMsgData->data + pMsgData->nUDHLength, pMsgData->nRawTPDULength);
            smo[0].nId = (pMsgData->bIsDLAck ? MSGOPT_TPDU_STATUS_REPORT : MSGOPT_TPDU_DELIVER);
            smo[0].pVal = (void*)pBuffer;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            FREE(pBuffer);
         }
      }

      // Check for AlphaID
      if (pMsgData->nAlphaIDLength)
      {
         char *pBuffer = NULL;
         if ((pBuffer = MALLOC(pMsgData->nAlphaIDLength + sizeof(AECHAR))) != NULL)
         {
            WEBOPT_SETVARBUFFER(pBuffer, 
                                pMsgData->data + pMsgData->nUDHLength + pMsgData->nRawTPDULength, 
                                pMsgData->nAlphaIDLength);
            smo[0].nId = MSGOPT_ALPHA_ID;
            smo[0].pVal = (void*)pBuffer;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            FREE(pBuffer);
         }
      }

      if (AEESMS_TYPE_BROADCAST == Xlate_TStoMT(pMsgData->ts))
      {
         if (pMsgData->bIsGWBroadcast)
         {
            // Fill up the GW CB Message Options
            smo[0].nId = MSGOPT_BC_SERVICE_ID;
            smo[0].pVal = (void*)pMsgData->dwSrvID;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

            smo[0].nId = MSGOPT_BC_PAGE_NUMBER;
            smo[0].pVal = (void*)pMsgData->dwGWCBPageNumber;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

            smo[0].nId = MSGOPT_BC_TOTAL_PAGES;
            smo[0].pVal = (void*)pMsgData->dwGWCBTotalPages;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

            smo[0].nId = MSGOPT_BC_GEO_SCOPE;
            smo[0].pVal = (void*)pMsgData->dwGWCBGeoScope;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

            smo[0].nId = MSGOPT_BC_MESSAGE_CODE;
            smo[0].pVal = (void*)pMsgData->dwGWCBMessageCode;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

            smo[0].nId = MSGOPT_BC_UPDATE_NUMBER;
            smo[0].pVal = (void*)pMsgData->dwGWCBUpdateNumber;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
         }
         else
         {
            smo[0].nId = MSGOPT_BCTYPE;
            smo[0].pVal = (void*)pMsgData->dwSrvID;
            ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
         }
      }

      if (AEESMS_DISPLAY_MODE_NONE != pMsgData->dwMessageDisplayMode)
      {
         // Set message display mode
         smo[0].nId = MSGOPT_DISPLAY_MODE;
         smo[0].pVal = (void*)pMsgData->dwMessageDisplayMode;
         
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      // Set Storage Information
      if (pMsgData->bIsStored)
      {
         smo[0].nId = MSGOPT_AUTO_STORAGE_TYPE;
         smo[0].pVal = (void*)pMsgData->st;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
         
         smo[0].nId = MSGOPT_AUTO_STORAGE_INDEX;
         smo[0].pVal = (void*)pMsgData->dwIndex;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }

      if (0xFFFFFFFF != pMsgData->dwGWCommand) {
         smo[0].nId = MSGOPT_GW_COMMAND;
         smo[0].pVal = (void*)pMsgData->dwGWCommand;
      
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

         // Set command message number
         smo[0].nId = MSGOPT_GW_COMMAND_MN;
         smo[0].pVal = (void*)(pMsgData->dwMsgID >> 16);
         
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
      }
   }
      
   if(pMsgPayload) 
   {
      if (0xFFFFFFFF != pMsgData->dwGWCommand) {

         // Set command data UDHI
         smo[0].nId = MSGOPT_GW_COMMAND_UDHI;
         smo[0].pVal = (void*)(pMsgPayload->nEncoding);
         
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

         // Set command data
         if (pMsgPayload->nBytes) {
            char * pBuffer = NULL;
            if ((pBuffer = MALLOC(pMsgPayload->nBytes + sizeof(AECHAR))) != NULL)
            {
               WEBOPT_SETVARBUFFER(pBuffer, pMsgPayload->data, pMsgPayload->nBytes);
               smo[0].nId = MSGOPT_COMMAND_DATA;
               smo[0].pVal = (void*)pBuffer;
               ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
               FREE(pBuffer);
            }
         }
      }
      else {
         uint32 nBytes;

         int nSMSEncoding = pMsgPayload->nEncoding; 

         byte * pText = AEE_GetDecodedTextString(pMsgPayload->data, pMsgPayload->nBytes, pMsgPayload->nEncoding, &pMsgPayload->nEncoding, &nBytes);
            
         // Specify SMS encoding so that same message can be forwarded and stored
         smo[0].nId = MSGOPT_MOSMS_ENCODING;
         smo[0].pVal = (void*)nSMSEncoding;
         
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
            
         // Set message encoding to an AEE encoding
         smo[0].nId = MSGOPT_PAYLOAD_ENCODING;
         smo[0].pVal = (void*)pMsgPayload->nEncoding;
         
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);

         if (pText)
         {   
            AEECLSID clsid = 0;
            char * pszBrewHandler = NULL;
            byte * pParsedText = pText;      // Initialize to pText
            byte delimiter = 0;

            ParseBrewMsg((char*)pText, nBytes, &clsid, &pszBrewHandler, &pParsedText, &delimiter);

            // Set BREW class or BREW handler
            if (clsid || pszBrewHandler)
            {
               if (pszBrewHandler)
               {
                  smo[0].nId = MSGOPT_BREW_HANDLER_SZ;
                  smo[0].pVal = (void*)pszBrewHandler;
                  ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
               }
               else
               {
                  smo[0].nId = MSGOPT_BREW_CLASS;
                  smo[0].pVal = (void*)clsid;
                  ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
               }

               // For BREW directed message, reduce nBytes by BREW specific prefix
               // This will reduce length by STRLEN("//BREW:<ClsId>:") for applet
               // directed and by STRLEN("//BREWHANDLER:") for handler directed message
               if ( delimiter )
               {
                  // ParseBrewMsg splits pText by replacing delimiter with \0
                  nBytes -= (STRLEN((char*)pText) + 1);
               }
               else
               {
                  // OEM defined handler, there is no delimiter, so to get
                  // actual payload text, reduce by //HANDLER
                  // since SMS will be of the form //HANDLERHelloWorld!
                  nBytes -= (STRLEN("//")+ STRLEN((char*)pszBrewHandler));
               }
            }

            // Set message payload
            if ((pMsgPayload->nEncoding == AEE_ENC_UNICODE) ||
               (pMsgPayload->nEncoding == AEE_ENC_ISOLATIN1))
            {
               if (pMsgPayload->nEncoding == AEE_ENC_UNICODE) {
                  // pParsedText might not align correctly for unicode string, 
                  // which can cause misalignment.
                  // Here we allocat a new buffer to store the content of pParsedText.
                  AECHAR *pszPayloadText = NULL;                  
                  pszPayloadText = (AECHAR *) MALLOC ((size_t) nBytes + sizeof(AECHAR));
                  
                  if (pszPayloadText != NULL) {
                     // Copy the content of payload text.
                     MEMMOVE((void *) pszPayloadText, (void *) pParsedText, (int) nBytes);
                     smo[0].nId = MSGOPT_PAYLOAD_WSZ;
                     smo[0].pVal = (void*)pszPayloadText;
                  
                     (void) ISMSMSG_AddOpt(pISMSMsg, smo);                     
                     FREE(pszPayloadText);
                  }
                  else
                  {
                     nErr = ENOMEMORY;
                  }                  
               } else {
                  smo[0].nId = MSGOPT_PAYLOAD_SZ;
                  smo[0].pVal = (void*)pParsedText;
                  (void) ISMSMSG_AddOpt(pISMSMsg, smo);
               }
            }
            else
            {
               char * pBuffer = NULL;
               if ((pBuffer = MALLOC(nBytes + sizeof(AECHAR))) != NULL)
               {
                  WEBOPT_SETVARBUFFER(pBuffer, pParsedText, nBytes);
                  smo[0].nId = MSGOPT_PAYLOAD_BINARY;
                  smo[0].pVal = (void*)pBuffer;
                  ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
                  FREE(pBuffer);
               }
            }
            FREE(pText);
         }
      }
   }

   if (SUCCESS == nErr) {
      *ppISMSMsg = pISMSMsg;
   }

   return nErr;
}

/*===========================================================================

Function: StripOffPrefix

Description:
   Parses pszText to strip off the prefix returning //BREWHANDLER'\0'<Payload>.
===========================================================================*/
static char * StripOffPrefix( char *pszText )
{
   char * psz = pszText;

   if ((pszText) && (AEE_IsDebugBREWSMS() || OEMSMS_IgnorePrefixForBrewDirectedMsg()))
   {
      psz = STRSTR(pszText,"//");
   }

   return psz;
}

/*===========================================================================

Function: ParseBrewMsg

Description:
   Parses pText to check if message is directed to a BREW app.
   //BREW:<ClsID>:<Payload> is changed to //BREW:<ClsID>'\0'<Payload>.
   //BREWHANDLER:<Payload> is changed to //BREWHANDLER'\0'<Payload>.

Parameters:
   pText             - (IN) SMS Payload Text 
   nBytes            - (IN) Length of text since it can contain NUL
   *pCls             - (OUT) Class ID if found in payload
   **ppszBrewHandler - (OUT) Brew Handler if found
   **ppParsedText    - (OUT) Actual payload text
   *pDelimiter       - (OUT) Delimiter
===========================================================================*/
static void ParseBrewMsg(char *pText, int nBytes, AEECLSID *pCls, 
                         char ** ppszBrewHandler, byte **ppParsedText, byte *pDelimiter )
{
   char * pBuffer = NULL;

   FARF(SMS, ("==>  ParseBrewMsg"));

   // Advance to the first token "//" on test devices. On non test devices, we assume
   // that BREW directed message is of //BREW:<clsid>:<payload> or //BREWHANDLER:<payload>
   // type

   // Pass in Brew Directed Message
   // The function will compute and check if this the Prefix is ignorable
   if ((pText) && (AEE_IsDebugBREWSMS() || OEMSMS_IgnorePrefixForBrewDirectedMsg())) {
      if ((pBuffer = MEMSTR(pText,"//", nBytes)) != NULL) {
         nBytes -= (pBuffer - pText);
         pText = pBuffer;
      }
   }

   // Check if message is of "//BREW:<clsid>:<payload>" type
   if (0 == STRNICMP(pText,BREW_SMS_PREFIX,BREW_SMS_PREFIX_LEN)) {
      pBuffer = pText + BREW_SMS_PREFIX_LEN;   // pBuffer == "<clsid>:<payload>"

      pText = MEMCHR(pBuffer, ':', nBytes - BREW_SMS_PREFIX_LEN);   // pText == ":<payload>"
      
      if (pText) {
         *pText++ = 0;                     // pText == "<payload>", pBuffer == "<clsid>"

         if (pCls) {
            *pCls = (AEECLSID)STRTOUL(pBuffer, NULL, 16);
         }

         if (ppParsedText) {
            *ppParsedText = (byte*)pText;  // *ppParsedText == "<payload>"
         }

         if (pDelimiter) {
            *pDelimiter = ':';
         }
      }
   }
   // Check for BREW HANDLER directed message
   else if (0 == STRNCMP(pText, "//", STRLEN("//")))
   {
      // OEM defined handler - dont need to have a : delimiter
      if (OEMSMS_OEMDefinedHandler(pText, &pBuffer)) {
         if (ppszBrewHandler) {
            *ppszBrewHandler = pBuffer;
         }
            
         if (pCls) {
            *pCls = ISHELL_GetHandler(AEE_GetShell(), AEECLSID_APP, pBuffer);
         }   

         // *ppParsedText == "<payload>", pBuffer is the OEM Handler            
         if (ppParsedText) {
            *ppParsedText = (byte*)pText + STRLEN("//") + STRLEN(pBuffer);
         }
      }
      else {
         // Check if message is of "//BREWHANDLER:<payload>" type

         pBuffer = pText;                         // pBuffer == "//BREWHANDLER:<payload>"

         pText = MEMCHR(pBuffer, ':', nBytes);            // pText == ":<payload>"

         if (pText) 
         {
            char * pszBrewHandler = NULL;

            *pText++ = 0;                     // pText == "<payload>", pBuffer == "//BREWHANDLER"

            if (0 == STRNCMP(pBuffer, "// ", STRLEN("// "))) {
               pszBrewHandler = pBuffer + STRLEN("// ");     // pszBrewHandler == "BREWHANDLER"
            }
            else {
               pszBrewHandler = pBuffer + STRLEN("//");      // pszBrewHandler == "BREWHANDLER"
            }

            if (ppszBrewHandler) {
               *ppszBrewHandler = pszBrewHandler;
            }

            if (pCls) {
               *pCls = ISHELL_GetHandler(AEE_GetShell(), AEECLSID_APP, pszBrewHandler);
            }
             
            if (ppParsedText) {
                *ppParsedText = (byte*)pText;       // *ppParsedText == "<payload>"
            }

            if (pDelimiter) {
                *pDelimiter = ':';
            }
         }
      }
   }
}

/*===========================================================================

Function: Xlate_TStoMT

Description:
   Translates OEM teleservice to AEE message type
 
===========================================================================*/
static int Xlate_TStoMT(int ts)
{
   int i;
   for (i = 0; i < sizeof(gsMsgTypeMap)/sizeof(MsgTypeMap); i++)
   {
      if (ts == gsMsgTypeMap[i].ts)
      {
         return gsMsgTypeMap[i].mt;
      }
   }
   return AEESMS_TYPE_UNKNOWN;
}

/*===========================================================================

Function: Xlate_MTtoTS

Description:
   Translates AEE message type to OEM teleservice
 
===========================================================================*/
static int Xlate_MTtoTS(int mt)
{
   int i;
   for (i = 0; i < sizeof(gsMsgTypeMap)/sizeof(MsgTypeMap); i++)
   {
      if (mt == gsMsgTypeMap[i].mt)
      {
         return gsMsgTypeMap[i].ts;
      }
   }
   return SMS_TELESERVICE_UNKNOWN;
}

/*===========================================================================

Function: Xlate_SMSEncToAEEEnc

Description:
   Translates SMS encoding to AEE encoding
 
===========================================================================*/
static int Xlate_SMSEncToAEEEnc(int se)
{
   int i;
   for (i = 0; i < sizeof(gsAEESMSEncMap)/sizeof(AEESMSEncMap); i++)
   {
      if (gsAEESMSEncMap[i].se == se)
      {
         return gsAEESMSEncMap[i].ae;
      }
   }

   // Pass whatever encoding we have assuming that it makes to client
   return se;
}

/*===========================================================================

Function: Xlate_AEEEncToSMSEnc

Description:
   Translates AEE encoding to SMS encoding
 
===========================================================================*/
static int Xlate_AEEEncToSMSEnc(int ae)
{   
   int i;
   for (i = 0; i < sizeof(gsAEESMSEncMap)/sizeof(AEESMSEncMap); i++)
   {
      if (gsAEESMSEncMap[i].ae == ae)
      {
         return gsAEESMSEncMap[i].se;
      }
   }

   // Pass whatever encoding we have assuming that it makes to client
   return ae;
}

/*===========================================================================

Function: IsMsgPackable

Description:
   Tests if message can be sent as packed 7/8 bit
 
===========================================================================*/
static boolean IsMsgPackable(byte * pData, int nBytes, int en, boolean b7bit)
{
   int i;
   boolean bReturn = FALSE;

   if ((en == AEE_ENC_ISOLATIN1 || en == AEE_ENC_GSM_7_BIT_DEFAULT) && 
       b7bit)
   { 
      char * pText = (char*)pData;

      for (i = 0; i < nBytes; i++)
      {
         if ((unsigned char)pText[i] > 0x7f)
            goto Done;
      }
      bReturn = TRUE;
   }
   else if (en == AEE_ENC_UNICODE)
   {
      AECHAR * pText = (AECHAR*)pData;

      if (nBytes & 0x1) // Check for Odd number of bytes
         goto Done;

      for (i = 0; i < nBytes/2; i++)
      {
         if ((unsigned)pText[i] > (unsigned)(b7bit ? 0x7f : 0xff))
            goto Done;
      }
      bReturn = TRUE;
   }

Done:
   return bReturn;
}

/*===========================================================================

Function: IsMsgOptUDH

Description:
   Checks if specified ID belongs to UDH.

===========================================================================*/
static boolean IsMsgOptUDH(int32 nId)
{
   boolean bMatch = FALSE;

   if ((nId >= MSGOPT_UDH_CONCAT) && (nId <= MSGOPT_UDH_OTHER))
      bMatch = TRUE;

   return bMatch;
}

/*===========================================================================

Function: Xlate_UDHIDToMsgOpt

Description:
   Translates UDH ID to corresponding message option
 
===========================================================================*/
static int Xlate_UDHIDToMsgOpt(int id)
{
   int i;
   for (i = 0; i < sizeof(gsAEEMsgOptUDHIDMap)/sizeof(AEEMsgOptUDHIDMap); i++)
   {
      if (gsAEEMsgOptUDHIDMap[i].id == id)
      {
         return gsAEEMsgOptUDHIDMap[i].mo;
      }
   }

   // Pass MSGOPT_UDH_OTHER
   return MSGOPT_UDH_OTHER;
}

/*===========================================================================

Function: UnpackUDHMsgOpts

Description:
   Unpack the UDH MSGOPTS Structure to byte pointer. Can be used to calculate
   size required by specifying pBytes as NULL.

===========================================================================*/
static void UnpackUDHMsgOpts(ISMSMsg *pISMSMsg, int * pnBytes, byte * pBytes)
{
   int nIndex = 0, nLength = 0, nBytes = 0;   
   SMSMsgOpt smo;

   if (!pnBytes)
      return;

   while (ISMSMSG_GetOptWithIndex(pISMSMsg, MSGOPT_ANY, nIndex++, &smo) == SUCCESS)
   {
      if (TRUE == IsMsgOptUDH(smo.nId))
      {
         nLength = MSGOPT_GETUDHBUFFERLENGTH(smo.pVal) + 2; // 2 bytes for Length
         if (pBytes && ((nBytes + nLength) <= *pnBytes))
         {
            MEMCPY(pBytes + nBytes, (byte*)(smo.pVal), nLength);
         }
         nBytes += nLength;
      }
   }
   *pnBytes = nBytes;
}

/*===========================================================================

Function: PackUDHMsgOpts

Description:
   Pack the UDH MSGOPTS Structure from byte pointer

===========================================================================*/
static void PackUDHMsgOpts(byte * pBytes, int nBytes, ISMSMsg * pISMSMsg)
{
   if (pBytes && pISMSMsg && nBytes)
   {
      int nProcessed = 0;
      SMSMsgOpt smo[2];
      
      MEMSET(smo, 0, sizeof(smo));

      smo[1].nId = MSGOPT_END;

      // Set UDH options
      while (nProcessed < nBytes)
      {
         smo[0].nId = Xlate_UDHIDToMsgOpt(*(MSGOPT_GETUDHBUFFERDATA((byte*)(pBytes + nProcessed))));
         smo[0].pVal = (void*)(pBytes + nProcessed);
   
         ISMSMSG_AddOpt(pISMSMsg, (SMSMsgOpt*)smo);
         
         // Increment nProcessed by (bytes containing length + bytes containing data)
         nProcessed += (2 + MSGOPT_GETUDHBUFFERLENGTH((byte*)(pBytes + nProcessed)));
      }
   }
}

/*===========================================================================

Function: ComputeUDHSizeOnSMSPayload

Description:
   Computes number of bytes UDH will take on SMS payload

===========================================================================*/
static int ComputeUDHSizeOnSMSPayload(ISMSMsg * pISMSMsg)
{
   int nIndex = 0, nBytes = 0;   
   SMSMsgOpt smo;

   while (ISMSMSG_GetOptWithIndex(pISMSMsg, MSGOPT_ANY, nIndex++, &smo) == SUCCESS)
   {
      switch (smo.nId)
      {
         case MSGOPT_UDH_CONCAT:
            if (*(MSGOPT_GETUDHBUFFERDATA(smo.pVal)) == AEESMS_UDH_CONCAT_8)
            {       
               // Header ID Octet + Header Length Octet + Header Length
               nBytes += 5; 
            }
            else if (*(MSGOPT_GETUDHBUFFERDATA(smo.pVal)) == AEESMS_UDH_CONCAT_16)
            {
               // Header ID Octet + Header Length Octet + Header Length
               nBytes += 6; 
            }
            break;
   
         case MSGOPT_UDH_SPECIAL_SM:
            // Header ID Octet + Header Length Octet + Header Length
            nBytes += 4;
            break;

         case MSGOPT_UDH_PORT:
            if (*(MSGOPT_GETUDHBUFFERDATA(smo.pVal)) == AEESMS_UDH_PORT_8)
            {
               // Header ID Octet + Header Length Octet + Header Length
               nBytes += 4; 
            }
            else if (*(MSGOPT_GETUDHBUFFERDATA(smo.pVal)) == AEESMS_UDH_PORT_16)
            {
               // Header ID Octet + Header Length Octet + Header Length
               nBytes += 6; 
            }
            break;

         case MSGOPT_UDH_TEXT_FORMAT:
            if (4 == MSGOPT_GETUDHBUFFERLENGTH(smo.pVal))
            { 
               // Text format without text color information
               // Header ID Octet + Header Length Octet + Header Length
               nBytes += 5; 
            }
            else
            { 
               // Text format with text color information
               // Header ID Octet + Header Length Octet + Header Length
               nBytes += 6; 
            }
            break;

         case MSGOPT_UDH_PREDEFINED:
            // Header ID Octet + Header Length Octet + Header Length
            nBytes += 4;
            break;

         case MSGOPT_UDH_USERDEFINED:
            // Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)
            nBytes += MSGOPT_GETUDHBUFFERLENGTH(smo.pVal);
            break;

         case MSGOPT_UDH_VAR_PICTURE:
            // Header ID Octet + Header Length Octet + Header Length + 3 (for height, width and position octets)
            nBytes += MSGOPT_GETUDHBUFFERLENGTH(smo.pVal);
            break;

         case MSGOPT_UDH_RFC822:
            // Header ID Octet + Header Length Octet + Header Length
            nBytes += 3;
            break;

         case MSGOPT_UDH_OTHER:
            // Header ID Octet + Header Length Octet + Header Length
            nBytes += (MSGOPT_GETUDHBUFFERLENGTH(smo.pVal) - 1);
            break;

         default:
            break;
      }
   }

   // Add 1 byte for UDHL if UDH Present (nBytes > 0)
   if (nBytes > 0)
   {
      nBytes++;
   }

   return (nBytes);
}

/*===========================================================================

Function: UnpackSMSBCSrvOpts

Description:
   Unpack the Broadcast Service Options to the OEMSMSBCSrvInfo Strcuture

Return Value:
   Indicates success / failure

===========================================================================*/
static boolean UnpackSMSBCSrvOpts(ISMSBCSrvOpts *pISMSBCSrvOpts, OEMSMSBCSrvInfo * pSrvInfo)
{
   SMSBCSrvOpts sso;  
   
   // Service ID
   if (ISMSBCSRVOPTS_GetOpt(pISMSBCSrvOpts, SMSBCSRVOPT_SERVICEID, &sso) == SUCCESS)
   {
      pSrvInfo->sSrvID = *((AEESMSBCSrvID *)sso.pVal);
   }
   else
   {
      return FALSE; // Mandatory Parameter - Hence return False
   }

   // Service Selected
   if (ISMSBCSRVOPTS_GetOpt(pISMSBCSrvOpts, SMSBCSRVOPT_SELECTED, &sso) == SUCCESS)
   {
      pSrvInfo->bSelected = (boolean)((uint32)sso.pVal);
   }
   else
   {
      pSrvInfo->bSelected = FALSE; // Default Value
   }

   // Service Priority
   if (ISMSBCSRVOPTS_GetOpt(pISMSBCSrvOpts, SMSBCSRVOPT_PRIORITY, &sso) == SUCCESS)
   {
      pSrvInfo->nPriority = (AEEMSGPriority)sso.pVal;
   }
   else
   {
      pSrvInfo->nPriority = AEESMS_PRIORITY_NORMAL; // Default Value
   }

   // Service Label
   if (ISMSBCSRVOPTS_GetOpt(pISMSBCSrvOpts, SMSBCSRVOPT_LABEL_SZ, &sso) == SUCCESS)
   {
      STRLCPY(pSrvInfo->szLabel, (char*)sso.pVal, sizeof(pSrvInfo->szLabel));
   }
   else if (ISMSBCSRVOPTS_GetOpt(pISMSBCSrvOpts, SMSBCSRVOPT_LABEL_WSZ, &sso) == SUCCESS)
   {
      WSTRLCPY((AECHAR*)pSrvInfo->szLabel, (AECHAR*)sso.pVal, sizeof(pSrvInfo->szLabel));
   }
   else
   {
      MEMSET(pSrvInfo->szLabel, 0, sizeof(pSrvInfo->szLabel));
   }

   // Service Label Encoding
   if (ISMSBCSRVOPTS_GetOpt(pISMSBCSrvOpts, SMSBCSRVOPT_LABEL_ENCODING, &sso) == SUCCESS)
   {
      pSrvInfo->nLabelEncoding = Xlate_AEEEncToSMSEnc((int)sso.pVal);
   }
   else
   {
      // Assume Device Encoding by default
      AEEDeviceInfo di;
      di.wStructSize = sizeof(AEEDeviceInfo);
      AEE_GetDeviceInfo(&di);
      pSrvInfo->nLabelEncoding = Xlate_AEEEncToSMSEnc(di.wEncoding); 
   }

   // Service Alert
   if (ISMSBCSRVOPTS_GetOpt(pISMSBCSrvOpts, SMSBCSRVOPT_ALERT, &sso) == SUCCESS)
   {
      pSrvInfo->nAlert = (AEESMSBCAlertType)sso.pVal;
   }
   else
   {
      pSrvInfo->nAlert = AEESMS_BC_ALERT_DEFAULT;  // Default Value
   }

   // Service Max Messages
   if (ISMSBCSRVOPTS_GetOpt(pISMSBCSrvOpts, SMSBCSRVOPT_MAX_MSGS, &sso) == SUCCESS)
   {
      pSrvInfo->nMaxMessages = (byte)((uint32)sso.pVal);
   }
   else
   {
      pSrvInfo->nMaxMessages = 10; // Default Value 
   }

   return TRUE;
}

/*===========================================================================

Function: PackSMSBCSrvOpts

Description:
   Pack the OEMSMSBCSrvInfo Strcuture to Broadcast Service Options

Return Value:
   Indicates success / failure

===========================================================================*/
static boolean PackSMSBCSrvOpts(OEMSMSBCSrvInfo * pSrvInfo, ISMSBCSrvOpts *pISMSBCSrvOpts)
{
   SMSBCSrvOpts sso[2];

   MEMSET(sso, 0, sizeof(sso));

   sso[1].nId = SMSBCSRVOPT_END;

   // Set for copying of options
   sso[0].nId = SMSBCSRVOPT_COPYOPTS;
   sso[0].pVal = (void *)TRUE;
      
   ISMSBCSRVOPTS_AddOpt(pISMSBCSrvOpts, (SMSBCSrvOpts*)sso);

   // Service ID
   sso[0].nId = SMSBCSRVOPT_SERVICEID;
   sso[0].pVal = (void*)&pSrvInfo->sSrvID;
   if (ISMSBCSRVOPTS_AddOpt(pISMSBCSrvOpts, (SMSBCSrvOpts*)sso) != SUCCESS)
   {
      return FALSE;
   }
   
   // Service Selected
   sso[0].nId = SMSBCSRVOPT_SELECTED;
   sso[0].pVal = (void*)pSrvInfo->bSelected;
   if (ISMSBCSRVOPTS_AddOpt(pISMSBCSrvOpts, (SMSBCSrvOpts*)sso) != SUCCESS)
   {
      return FALSE;
   }

   // Service Priority
   sso[0].nId = SMSBCSRVOPT_PRIORITY;
   sso[0].pVal = (void*)pSrvInfo->nPriority;
   if (ISMSBCSRVOPTS_AddOpt(pISMSBCSrvOpts, (SMSBCSrvOpts*)sso) != SUCCESS)
   {
      return FALSE;
   }
     
   // Service Label 
   sso[0].nId = (pSrvInfo->nLabelEncoding == AEESMS_ENC_UNICODE ? SMSBCSRVOPT_LABEL_WSZ : SMSBCSRVOPT_LABEL_SZ);
   sso[0].pVal = (void*)pSrvInfo->szLabel;
   if (ISMSBCSRVOPTS_AddOpt(pISMSBCSrvOpts, (SMSBCSrvOpts*)sso) != SUCCESS)
   {
      return FALSE;
   }
     
   // Service Label Encoding 
   sso[0].nId = SMSBCSRVOPT_LABEL_ENCODING;
   sso[0].pVal = (void*)Xlate_SMSEncToAEEEnc(pSrvInfo->nLabelEncoding);
   if (ISMSBCSRVOPTS_AddOpt(pISMSBCSrvOpts, (SMSBCSrvOpts*)sso) != SUCCESS)
   {
      return FALSE;
   }
     
   // Service Alert
   sso[0].nId = SMSBCSRVOPT_ALERT;
   sso[0].pVal = (void*)pSrvInfo->nAlert;
   if (ISMSBCSRVOPTS_AddOpt(pISMSBCSrvOpts, (SMSBCSrvOpts*)sso) != SUCCESS)
   {
      return FALSE;
   }
     
   // Service Max Messages
   sso[0].nId = SMSBCSRVOPT_MAX_MSGS;
   sso[0].pVal = (void*)pSrvInfo->nMaxMessages;
   if (ISMSBCSRVOPTS_AddOpt(pISMSBCSrvOpts, (SMSBCSrvOpts*)sso) != SUCCESS)
   {
      return FALSE;
   }
     
   return TRUE;
}

/*=============================================================================
   Removes the specified listener from the list
=============================================================================*/
static void IModelClass_CancelListener(ModelListener * pListener)
{
   IModel * pme = pListener->pCancelData;
   ModelListener * pf;

   // Do not mess with the list unless it was actually found there...

   for(pf = pme->m_pListeners; pf != NULL; pf = pf->pNext)
   {
      if(pf == pListener)
      {
         if(pListener->pPrev)
            pListener->pPrev->pNext = pListener->pNext;
         else
            pme->m_pListeners = pListener->pNext;
      
         if(pListener->pNext)
            pListener->pNext->pPrev = pListener->pPrev;
         break;
      }
   }

   pListener->pfnCancel   = NULL;
   pListener->pCancelData = NULL;
   pListener->pNext       = pListener->pPrev = NULL;
}

/*=============================================================================
   Adds the specified listener in the list
=============================================================================*/
static int IModelClass_AddListener(IModel *pme, ModelListener *pListener)
{
   LISTENER_Cancel(pListener);

   pListener->pNext = pme->m_pListeners;
   pListener->pPrev = NULL;

   if(pme->m_pListeners)
      pme->m_pListeners->pPrev = pListener;

   pme->m_pListeners = pListener;

   pListener->pfnCancel = IModelClass_CancelListener;
   pListener->pCancelData  = (void *)pme;

   return SUCCESS;
}

/*=============================================================================
   Notifier the specified event to the registered listeners
=============================================================================*/
static __inline
void IModelClass_Notify(IModel *pme, ModelEvent *pEvent)
{
   ModelListener * pListener = NULL;

   for (pListener = pme->m_pListeners; pListener != NULL; pListener = pListener->pNext)
   {
      if (pListener->pfnListener)
         pListener->pfnListener(pListener->pListenerData, pEvent);
   }
}

static void IModelClass_NotifyApp(IModel *pme, ModelEvent *pEvent, ACONTEXT* pac)
{
   ACONTEXT* pacRestore;

   pacRestore = AEE_EnterAppContext(pac);
   IModel_Notify(pme,pEvent);
   AEE_LeaveAppContext(pacRestore);
}

static void CSMS_Dtor(ISMS * pme)
{
   CALLBACK_Cancel(&pme->cbSysObj);
   CALLBACK_Cancel(&pme->cbOnSMSUpdate);

   if (!--gSMS.dwCount) {
      CALLBACK_Cancel(&gSMS.cbFromOEMLayerOnSMSUpdate);
   }
}

static void CSMS_CancelUpdateListener(AEECallback *pcb)
{
   AEECallback * pListCB = NULL, * pPrevCB = NULL;

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != CSMS_CancelUpdateListener)
      return;
      
   pListCB = gSMS.pcbOnSMSUpdate;

   while (pListCB)
   {
      if (pcb == pListCB)
      {
         if (pPrevCB)
            pPrevCB->pNext = pListCB->pNext;
         else
            gSMS.pcbOnSMSUpdate = pListCB->pNext;

         pListCB->pNext = NULL;
         break;
      }
      else
      {
         pPrevCB = pListCB;
         pListCB = pListCB->pNext;
      }
   }

   pcb->pCancelData = 0;
   pcb->pfnCancel = 0;
}

static void CSMS_OnSMSUpdate(AEECallback * pcb, OEMSMSUpdateData * pSMSUpdateData)
{
   // Make sure that its not there in any other list
   CALLBACK_Cancel(pcb);

   pcb->pfnCancel = CSMS_CancelUpdateListener;
   pcb->pReserved = pSMSUpdateData;
   pcb->pNext = gSMS.pcbOnSMSUpdate;
   gSMS.pcbOnSMSUpdate = pcb;
}

static void CSMS_cbFromOEMLayerOnSMSUpdate(void * po)
{
   while (gSMS.pcbOnSMSUpdate)
   {
      MEMCPY(gSMS.pcbOnSMSUpdate->pReserved, &gSMS.sUpdateData, sizeof(gSMS.sUpdateData));
      AEE_SYS_RESUME(gSMS.pcbOnSMSUpdate);
   }
   OEMSMS_OnSMSUpdate(&gSMS.sUpdateData, &gSMS.cbFromOEMLayerOnSMSUpdate);
}

static void CSMSStorage_Dtor(ISMSStorage2 * pme)
{
   CALLBACK_Cancel(&pme->cbSysObj);
   CALLBACK_Cancel(&pme->cbOnStorageUpdate);

   if (!--gpSMSStorage->dwCount) {
      OEMSMS_OnStorageUpdate(NULL);
      sys_free(gpSMSStorage);
      gpSMSStorage = 0;
   }
}

static void CSMSStorage_CancelUpdateListener(AEECallback *pcb)
{
   AEECallback * pListCB = NULL, * pPrevCB = NULL;

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != CSMSStorage_CancelUpdateListener)
      return;
      
   pListCB = gpSMSStorage->pcbOnStorageUpdate;

   while (pListCB)
   {
      if (pcb == pListCB)
      {
         if (pPrevCB)
            pPrevCB->pNext = pListCB->pNext;
         else
            gpSMSStorage->pcbOnStorageUpdate = pListCB->pNext;

         pListCB->pNext = NULL;
         break;
      }
      else
      {
         pPrevCB = pListCB;
         pListCB = pListCB->pNext;
      }
   }

   pcb->pCancelData = 0;
   pcb->pfnCancel = 0;
}

static void CSMSStorage_OnStorageUpdate(AEECallback * pcb, OEMStorageUpdateData * pStorageUpdateData)
{
   // Make sure that its not there in any other list
   CALLBACK_Cancel(pcb);

   pcb->pfnCancel = CSMSStorage_CancelUpdateListener;
   pcb->pReserved = pStorageUpdateData;
   pcb->pNext = gpSMSStorage->pcbOnStorageUpdate;
   gpSMSStorage->pcbOnStorageUpdate = pcb;
}

static void CSMSStorage_NotifyFromOEMLayerOnStorageUpdate(void * po)
{
   OEMStorageUpdateData * pEventData = (OEMStorageUpdateData *)po;

   if (gpSMSStorage->dwUsedEntries == gpSMSStorage->dwTotalEntries)
   {
      // There is no room left so we need to make queue bigger
      void * pRealloc = sys_realloc(gpSMSStorage, 
                                    (sizeof(CSMSStorageStaticData) - 
                                     DEFAULT_EVENT_CACHE_ENTRIES*sizeof(OEMStorageUpdateData) + 
                                     2*gpSMSStorage->dwTotalEntries*sizeof(OEMStorageUpdateData)));
      if (pRealloc)
      {
         gpSMSStorage = pRealloc;
         MEMCPY(&gpSMSStorage->sUpdateData[gpSMSStorage->dwTotalEntries], gpSMSStorage->sUpdateData, sizeof(OEMStorageUpdateData)*gpSMSStorage->dwAddResumeTo);
         gpSMSStorage->dwAddResumeTo += gpSMSStorage->dwTotalEntries;
         gpSMSStorage->dwTotalEntries *= 2;
      }
   }

   if (gpSMSStorage->dwUsedEntries < gpSMSStorage->dwTotalEntries)
   {
      // There is room in queue for this entry
      MEMCPY(&gpSMSStorage->sUpdateData[gpSMSStorage->dwAddResumeTo], pEventData, sizeof(OEMStorageUpdateData));
      gpSMSStorage->dwAddResumeTo = (gpSMSStorage->dwAddResumeTo + 1)%gpSMSStorage->dwTotalEntries;
      gpSMSStorage->dwUsedEntries++;
   }
   AEE_SYS_RESUME(&gpSMSStorage->cbFromOEMLayerOnStorageUpdate);
}

static void CSMSStorage_cbFromOEMLayerOnStorageUpdate(void * po)
{
   while (gpSMSStorage->pcbOnStorageUpdate)
   {
      MEMCPY(gpSMSStorage->pcbOnStorageUpdate->pReserved, &gpSMSStorage->sUpdateData[gpSMSStorage->dwResumeFrom], sizeof(OEMStorageUpdateData));
      AEE_SYS_RESUME(gpSMSStorage->pcbOnStorageUpdate);
   }
   gpSMSStorage->dwResumeFrom = (gpSMSStorage->dwResumeFrom + 1)%gpSMSStorage->dwTotalEntries;
   gpSMSStorage->dwUsedEntries--;

   if (0 == gpSMSStorage->dwUsedEntries)
   {
      // We have dispatched all the events we were supposed to dispatch so reset the indices
      gpSMSStorage->dwResumeFrom = gpSMSStorage->dwAddResumeTo = 0;
   }
   else
   {
      // Resume
      AEE_SYS_RESUME(&gpSMSStorage->cbFromOEMLayerOnStorageUpdate);
   }
}

static void CSMSBCConfig_Dtor(ISMSBCConfig * pme)
{
   CALLBACK_Cancel(&pme->cbSysObj);
   CALLBACK_Cancel(&pme->cbOnSMSBCConfigUpdate);

   if (!--gSMSBCConfig.dwCount) {
      CALLBACK_Cancel(&gSMSBCConfig.cbFromOEMLayerOnSMSBCConfigUpdate);
   }
}

static void CSMSBCConfig_CancelUpdateListener(AEECallback *pcb)
{
   AEECallback * pListCB = NULL, * pPrevCB = NULL;

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != CSMSBCConfig_CancelUpdateListener)
      return;
      
   pListCB = gSMSBCConfig.pcbOnSMSBCConfigUpdate;

   while (pListCB)
   {
      if (pcb == pListCB)
      {
         if (pPrevCB)
            pPrevCB->pNext = pListCB->pNext;
         else
            gSMSBCConfig.pcbOnSMSBCConfigUpdate = pListCB->pNext;

         pListCB->pNext = NULL;
         break;
      }
      else
      {
         pPrevCB = pListCB;
         pListCB = pListCB->pNext;
      }
   }

   pcb->pCancelData = 0;
   pcb->pfnCancel = 0;
}

static void CSMSBCConfig_OnSMSBCConfigUpdate(AEECallback * pcb, OEMSMSUpdateData * pSMSBCConfigUpdateData)
{
   // Make sure that its not there in any other list
   CALLBACK_Cancel(pcb);

   pcb->pfnCancel = CSMSBCConfig_CancelUpdateListener;
   pcb->pReserved = pSMSBCConfigUpdateData;
   pcb->pNext = gSMSBCConfig.pcbOnSMSBCConfigUpdate;
   gSMSBCConfig.pcbOnSMSBCConfigUpdate = pcb;
}

static void CSMSBCConfig_cbFromOEMLayerOnSMSBCConfigUpdate(void * po)
{
   while (gSMSBCConfig.pcbOnSMSBCConfigUpdate)
   {
      MEMCPY(gSMSBCConfig.pcbOnSMSBCConfigUpdate->pReserved, &gSMSBCConfig.sUpdateData, sizeof(gSMSBCConfig.sUpdateData));
      AEE_SYS_RESUME(gSMSBCConfig.pcbOnSMSBCConfigUpdate);
   }
   OEMSMSBCConfig_OnSMSBCConfigUpdate(&gSMSBCConfig.sUpdateData, &gSMSBCConfig.cbFromOEMLayerOnSMSBCConfigUpdate);
}

boolean AEESMS_IsDebugMsgOn()
{
   return gSMS.bDebugMsg;
}

void AEESMS_EnableDebugMsg(boolean bOn)
{
   gSMS.bDebugMsg = (bOn ? TRUE : FALSE);
}

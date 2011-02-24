/*===========================================================================

                             O E M G S M S M S . c     
                

GENERAL DESCRIPTION
   This file contains functions used to provide GSM SMS services

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   BREW must be initialized.

Copyright ?2002-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Then include OEM headers.*/
#include "OEMFeatures.h"

/* The whole file depends on the following include.*/                         
#if defined(FEATURE_GSM1x)

#include "OEMClassIDs.h"
#include "OEMGSM1xCardHandler.h"
#include "OEMGSM1xMessage.h"

//#include "OEMCriticalSection.h"

/* Then include AEE headers.*/
#include "AEE.h"
#include "AEE_OEM.h"
#include "AEEFile.h"
#include "AEEShell.h"

#include "AEEStdLib.h"
#include "AEE_OEMDispatch.h"
#include "AEEGSMSMS.h"
#include "AEEGSM1xSig.h"

#include "OEMGSMSMS_SVC.h"


/*===========================================================================

                      CONSTANTS and ENUMs

===========================================================================*/

#ifdef GSMSMS_UNITTEST
    #define STATIC 
    #define FARF_ERROR    0
    #define FARF_HIGH     0
    #define FARF_MED      0
    #define FARF_LOW      0
    #define FARF_UNITTEST 0
#else
    #define STATIC static
    #define FARF_ERROR    0
    #define FARF_HIGH     0
    #define FARF_MED      0
    #define FARF_LOW      0
    #define FARF_UNITTEST 0
#endif

#ifdef FARF
    #undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
    #define FARF_ALWAYS  1
#else
    #define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 

#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }

// This should only be used for function parameters, not for local variables
#define PARAM_NOT_REF(param)         /*lint -esym(715,param) */


/*===========================================================================

                      CONSTANTS

===========================================================================*/

#define GSMSMS_NV_MBOX_RECORD_LEN (GSMSMS_MAX_SIM_MESSAGE_LEN+2+30)
#define GSMSMS_NV_MBOX_RECORD_MARKER 0xEE

#define GSMSMS_ERROR_IND (-1)

#define GSMSMS_NV_ENTRY_SIZE  500
#define GSMSMS_NUM_NV_ENTRIES 10
#define GSMSMS_NUM_NV_VM_ENTRIES 1
#define GSMSMS_USER_DATA_IEI  0x41

// TP FCS values from GSM 23.40, 9.2.3.22 TODO Move this to header file
//#define GSMSMS_TP_FCS_VPF_NOT_SUPPORTED   0xC6 
//#define GSMSMS_TP_FCS_SIM_STORAGE_FULL    0xD0 // Class 2 msg
//#define GSMSMS_TP_FCS_MEM_CAP_EXCEEDED    0xD3 // Class 1 msg
#define GSMSMS_TP_FCS_UNSPECIFIED_ERROR   0xFF

#ifdef GSMSMS_UNITTEST
#define GSMSMS_TIMEOUT_VAL             (5*1000)
#else
#define GSMSMS_TIMEOUT_VAL             (36*1000)
#endif

#define NVRAM_DB_NAME "SIM_SMS"
#define NVRAM_VM_DB_NAME "SIM_VM"


#if !defined(BCD2INT)
// BCD to integer
#define BCD2INT(x)  (((((x) & 0xf0) >> 4) * 10) + ((x) & 0x0f))
#endif

/*===========================================================================

                      TYPES 

===========================================================================*/
// RP CAUSE values from GSM 24.11 table 8.4
enum {
   GSMSMS_RP_CAUSE_MEM_CAP_EXCEEDED  = 22, 
// GSMSMS_RP_CAUSE_FACILITY_NOT_IMPL = 69,   // These are commented out because
// GSMSMS_RP_CAUSE_INVALID_MSG_TYPE  = 97,   // they generate Lint warnings
// GSMSMS_RP_CAUSE_IE_NOT_IMPL       = 99    // because they are not used yet
   GSMSMS_RP_CAUSE_PROTOCOL_ERROR    = 111, 
   GSMSMS_RP_CAUSE_TEMPORARY_FAILURE = 41 
};

enum {
   CES_FREE        = 0,
   CES_PARSING     = 1,
   CES_READY       = 2,
   CES_DISPATCHING = 3
};

// RP layer constants (GSM 24.011)
enum {
   GSMSMS_RP_MO_DATA           = 0,
   GSMSMS_RP_MT_DATA           = 1,
   GSMSMS_RP_MO_ACK            = 2,
   GSMSMS_RP_MT_ACK            = 3 ,
   GSMSMS_RP_MO_ERROR          = 4,
   GSMSMS_RP_MT_ERROR          = 5,
   GSMSMS_SM_MEMORY_AVAILABLE  = 6
};

// TL layer constants (GSM 23.040)
enum {
   GSMSMS_TL_SMS_DELIVER        = 0, // MT
   GSMSMS_TL_SMS_DELIVER_REPORT = 0, // MO
   GSMSMS_TL_SMS_SUBMIT_REPORT  = 1, // MT
   GSMSMS_TL_SMS_SUBMIT         = 1, // MO
   GSMSMS_TL_SMS_STATUS_REPORT  = 2, // MT
   GSMSMS_TL_SMS_COMMAND        = 2  // MO
};

// Values for GSM1xMessage callback type identication
enum {
   USER_CB_NONE  = 0,
   USER_CB_IPHONE= 1,
   USER_CB_SMMA  = 2
};

// RerouteTapi parameters type
typedef struct  
{
    char         szDst[GSMSMS_MAX_ADDRESS_LEN+1];
    char         szMsg[GSMSMS_MAX_SMS_SUBMIT_UD_LEN+1];
    AEECLSID     clsDst;
    uint32     * pdwStatus;
    AEECallback* pcb;

} RerouteTapiParameters;


/*===========================================================================

                      EXTERNAL FUNCTIONS

===========================================================================*/

extern AEEGSM1xSig_Status OEMGSM1xControl_GetStatus(void);

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/

struct _IGSMSMSNotifier
{
   const AEEVTBL(IGSMSMSNotifier) *pvt;

   IShell                  * m_pIShell;
   uint32                    m_dwNotify; 
   boolean                   m_bInitialized;
};


struct _IGSMSMS
{
   const AEEVTBL(IGSMSMS) * pvt; 

   IShell           * m_pIShell;
   IFileMgr         * m_pIFileMgr;
   IFile            * m_pSMSMbox;
   IFile            * m_pVMMbox;
   boolean            m_bInitialized;
   boolean            m_bSIMSMSEnabled;
   boolean            m_bCardHandlerIsInit;
   boolean            m_bSMMAPending;
   byte               m_DeliverRP_MR;
   byte               m_SubmitRP_MR;
   AEECallback      * m_pCallback;
   AEECallback        m_TAPICallback;
   GSMSMSSendReport   m_TAPIReport;
   PFNSMSSTATUS       m_TAPIpfn;
   void             * m_pReport;
   byte               m_TPOffset;
   byte               m_TPLen;
};


/*===========================================================================

                      PUBLIC METHOD DECLARATIONS

===========================================================================*/
int OEMGSMSMS_New(IShell * ps, AEECLSID cls,void **ppif);
int OEMGSMSMSNotifier_New(IShell * ps, AEECLSID cls,void **ppif);

/*===========================================================================

                      INTERFACE METHOD DECLARATIONS

===========================================================================*/
static uint32 OEMGSMSMSNotifier_AddRef (IGSMSMSNotifier * po);
static uint32 OEMGSMSMSNotifier_Release(IGSMSMSNotifier * po);
static void   OEMGSMSMSNotifier_SetMask(IGSMSMSNotifier * po, uint32 * pdwMask);

static uint32 OEMGSMSMS_AddRef(struct _IGSMSMS * pMe);
static uint32 OEMGSMSMS_Release(struct _IGSMSMS * pMe);
static int OEMGSMSMS_QueryInterface(struct _IGSMSMS *pMe, AEECLSID clsid, void **ppNew);

static boolean OEMGSMSMS_IsInit(struct _IGSMSMS *pMe);
static int OEMGSMSMS_SendSMSSubmit
(
   struct _IGSMSMS *pMe, const GSMSMSSubmitType * pMsg, AEECallback *pCb, GSMSMSSendReport *pReport
);

static int OEMGSMSMS_SendSMSDeliverReport
(
   struct _IGSMSMS *pMe, const GSMSMSDeliverReportType *pDeliverReport
);

static int OEMGSMSMS_SendMoreMemoryAvailable
(
   struct _IGSMSMS *pMe, AEECallback *pCb, GSMSMSSMMAReport *pReport
);

static int OEMGSMSMS_GetMemoryCapExceededFlag
(
   struct _IGSMSMS *pMe, uint8 *pMemAvalable
);

static int OEMGSMSMS_SetMemoryCapExceededFlag
(
   struct _IGSMSMS *pMe, uint8 flag
);

static int OEMGSMSMS_GetTPMR(struct _IGSMSMS *pMe, uint8 *pTPMR);
static int OEMGSMSMS_SetTPMR(struct _IGSMSMS *pMe, uint8 TPMR);

#if 0
static int OEMGSMSMS_SendSMSCommand
(
   struct _IGSMSMS *pMe, GSMSMSCommandType const * pMsg, AEECallback *pCb, GSMSMSSendReport *pReport
);
#endif

// SIM/NV Interface
static int GSMSMS_StoreAtIndex
(
   struct _IGSMSMS *pMe, uint16 index, const GSMSMSMsg * pMsg, GSMSMSStorageType storeTo
);
static int OEMGSMSMS_StoreMessage
(
   struct _IGSMSMS *pMe, const GSMSMSMsg * pMsg, GSMSMSStorageType storeTo, uint16 *pIndex
);
static int OEMGSMSMS_MoveMessage
(
   struct _IGSMSMS *pMe, GSMSMSStorageType moveFrom, uint16 fromIndex, GSMSMSStorageType moveTo, uint16 *pToIndex
);
static int OEMGSMSMS_StoreStatusReport
(
   struct _IGSMSMS *pMe, uint16 index, const GSMSMSMsg * pMsg, GSMSMSStorageType storeTo
);
static int OEMGSMSMS_DeleteAllMessages
(
   struct _IGSMSMS * pMe, uint16 msgMask, GSMSMSStorageType deleteFrom
);
static int OEMGSMSMS_DeleteMessage
(
   struct _IGSMSMS * pMe, uint16 index, GSMSMSStorageType deleteFrom
);
static int OEMGSMSMS_GetMessage
(
   struct _IGSMSMS * pMe, uint16 index, GSMSMSRawMsg *pMsg, GSMSMSStorageType readFrom
);
static int OEMGSMSMS_GetStatusReport
(
   struct _IGSMSMS * pMe, uint16 index, GSMSMSRawMsg *pMsg, GSMSMSStorageType readFrom
);
static int OEMGSMSMS_GetStoreSize
(   
   struct _IGSMSMS * pMe, 
   GSMSMSStorageType readFrom,
   uint16 *pCount
); 
static int  OEMGSMSMS_SetMessageStatus
(
   struct _IGSMSMS * pMe, uint16 index, GSMSMSStatusType status, 
   GSMSMSStorageType writeTo
);
static int OEMGSMSMS_GetMessageStatus
(
   struct _IGSMSMS * pMe, 
   uint16 index, 
   GSMSMSStatusType *pStatus, 
   GSMSMSStorageType readFrom
);
static int OEMGSMSMS_GetSCAddress
(
   struct _IGSMSMS * pMe, GSMSMSAddress *pAddress
);
static int OEMGSMSMS_SetSCAddress
(
   struct _IGSMSMS * pMe, const GSMSMSAddress *pAddress
);

//
// Helper Functions
//
static int OEMGSMSMS_CreateDefaultMessage
(
   struct _IGSMSMS *pMe, 
   GSMSMSMsgType type, 
   GSMSMSMsg *pMsg
);
static int OEMGSMSMS_DecodeMessage
(
   struct _IGSMSMS *pMe, const GSMSMSRawMsg *pRaw, GSMSMSMsg *pMsg
);
static int OEMGSMSMS_DecodeUserData
(
   struct _IGSMSMS *pMe, const GSMSMSMsg *pMsg, AECHAR *pwzStr, uint16 wstrlen
);
static int OEMGSMSMS_EncodeUserData
(
   struct _IGSMSMS *pMe, const AECHAR *pwszText, byte *pDest, byte destlen, 
   GSMSMSEncodingType encodingOut, byte *pEncodedLen
);

/*===========================================================================

                    PROTECTED METHOD DECLARATIONS

For use by IPhone, SS  etc...
===========================================================================*/
void OEMGSMSMS_InjectMessage
(
    struct _IGSMSMS *pMe,
    const byte * pData, 
    uint8 length
);

void GSMSMS_ProcessCacheNotification(void);

int GSMSMS_ScheduleRerouteTapi (const char * pszDst,
                                const char * pszMsg,
                                AEECLSID     clsDst,
                                uint32     * pdwStatus,
                                AEECallback* pcb);

/*===========================================================================

                      PRIVATE METHOD DECLARATIONS

===========================================================================*/
void GSMSMS_StatusCallback(void *userData, int status);

static void GSMSMS_ProcessCache(void *dummy);
static void GSMSMS_IPhoneCb(int status);
static void GSMSMS_SMMACb(int status);

STATIC int GSMSMS_SendSMSRaw
(
    const byte* pIS637UserData, 
    uint8 len,
    uint8 callbackType,
    boolean startTransaction
);

static void GSMSMS_TimerCallback(void *data);
static void GSMSMS_InitTimerCallback(void *pData);
STATIC int GSMSMS_GetFreeSlot(struct _IGSMSMS *pMe, uint16 *pIndex, GSMSMSStorageType storeTo);
STATIC void GSMSMS_DecodeDCS(byte rawdcs, GSMSMSDCS *pDCS);
STATIC uint8 GSMSMS_DecodeAddress(byte const *pData, GSMSMSAddress *pAddr, boolean bIsSCAddr);
STATIC void GSMSMS_DecodeTime(byte const *pRawTime, GSMSMSTime *pTime);
STATIC void GSMSMS_EncodeTime(GSMSMSTime const *pTime, byte *pRawTime);
STATIC void GSMSMS_GSMToWSTR(byte const *pszAlpha,
                             uint32      nAlphaSize,
                             AECHAR     *pwzStr,
                             uint32      nWStrSize);
STATIC void GSMSMS_DecodeMessageStatus(byte raw, GSMSMSStatusType *pStatus);
STATIC boolean GSMSMS_EncodeVP(GSMSMSVP const *vp, byte *pRaw, byte *pcLen);
STATIC void GSMSMS_EncodeDCS(byte *pszOut, GSMSMSDCS const *pDCS);
STATIC int GSMSMS_EncodeAddress(GSMSMSAddress const *pAddr, byte *pszRaw, boolean bIsSCAddr);
STATIC byte GSMSMS_WSTRTo7BitGSM(AECHAR const *pwzStr, byte *pszAlpha, byte nAlphaSize);
STATIC int GSMSMS_ConvertToSIMRaw(const GSMSMSMsg *pMsg, GSMSMSRawMsg *pRawMsg);

STATIC int GSMSMSSubmitToRaw
( 
    const GSMSMSSubmitType * pMsg, 
    GSMSMSRawMsg *pRawMsg
);

STATIC int GSMSMS_DeliverToRaw
( 
    const GSMSMSDeliverType * pMsg, 
    GSMSMSRawMsg *pRawMsg,
    boolean read
);

STATIC int GSMSMS_DecodeSMSStatusReport
(
    const byte * pRaw, 
    GSMSMSStatusReportType * pSMSStatusReport
);

STATIC int GSMSMS_DecodeSMSDeliverMsg
(
    const byte * pRaw, 
    GSMSMSDeliverType * pSMSDeliver
);

STATIC int GSMSMS_DecodeSMSSubmitReportMsg
(
    GSMSMSRawData const * pRawMsg,
    GSMSMSSubmitReportType * pSMSSubmitReport
);

STATIC int GSMSMS_DecodeSMSSubmitMsg
(
    const byte * pRaw, 
    GSMSMSSubmitType * pSMSSubmit
);

STATIC int GSMSMS_DecodeBearerData(GSMSMSMessage*, byte, GSMSMSRawData*);

STATIC void GSMSMS_InitGSMCharMap(void);
STATIC int GSMSMS_UnicodeToGSM(AECHAR uc, byte *gsm, boolean *isExtended);
static void GSMSMS_CbAdapterForTAPI(void * pCbData);

static int GSMSMS_InitFile(const char *name, uint16 entries);
static void GSMSMS_CancelCb(AEECallback *pCb);
static int GSMSMS_ValidateStatusUpdate(GSMSMSStatusType oldStatus, GSMSMSStatusType status);

static IFile * GSMSMS_OpenDB(struct _IGSMSMS *pMe, GSMSMSStorageType store);
static void GSMSMS_VMMboxTimerCallback(void *pData);
static void GSMSMS_SMSMboxTimerCallback(void *pData);
static void GSMSMS_SMMATimerCallback(void *pData);

static int CalcUDLengthInBytes(byte UDL, GSMSMSDCS DCS, boolean UDHI, const byte *UD);

static void RerouteTapiUserModeWrapper(void* dummy);
static void RerouteTapiResultWrapper(void * pUser, int status);

static void GetHardCodedDefaultSMSCAddr(CardHandlerAddressType* pScAddr);

/*===========================================================================

                     LOCAL/STATIC DATA

===========================================================================*/
// GSM character set to 8-bit ASCII conversation table.
//
// Note that the gsmToUTF16[27] character is followed by a
// second byte, which is a continuation of that character
// and may be decoded using the gsmExtTable[] table below
//
static uint16 gsmToUTF16[128];

// The second byte of the GSM Extension Table.  This table
// only contains the characters this mobile supports
static const struct {
   byte gsmChar;
   uint16 unicodeChar;
} gsmExtTable[] = {
                    {0x0A, 0x000C}, // FORM FEED
                    {0x14, 0x005E}, // CIRCUMFLEX ACCENT
                    {0x28, 0x007B}, // LEFT CURLY BRACKET
                    {0x29, 0x007D}, // RIGHT CURLY BRACKET
                    {0x2F, 0x005C}, // REVERSE SOLIDUS
                    {0x3C, 0x005B}, // LEFT SQUARE BRACKET
                    {0x3D, 0x007E}, // TILDE
                    {0x3E, 0x005D}  // RIGHT SQUARE BRACKET
                    };

static byte gRefNum = 0;

//
// GSMSMS Module
//

static const IGSMSMSVtbl gOEMGSMSMSFuncs = { 
    OEMGSMSMS_AddRef,
    OEMGSMSMS_Release,
    OEMGSMSMS_QueryInterface,

    OEMGSMSMS_IsInit,
    OEMGSMSMS_SendSMSSubmit,
    OEMGSMSMS_SendSMSDeliverReport,
    OEMGSMSMS_SendMoreMemoryAvailable,

    OEMGSMSMS_StoreMessage,
    OEMGSMSMS_MoveMessage,
    OEMGSMSMS_StoreStatusReport,
    OEMGSMSMS_DeleteAllMessages,
    OEMGSMSMS_DeleteMessage,
    OEMGSMSMS_GetMessage,
    OEMGSMSMS_GetStatusReport,
    OEMGSMSMS_GetStoreSize,
    OEMGSMSMS_SetMessageStatus,
    OEMGSMSMS_GetMessageStatus,
    OEMGSMSMS_GetSCAddress,
    OEMGSMSMS_SetSCAddress,
    OEMGSMSMS_GetMemoryCapExceededFlag,
    OEMGSMSMS_SetMemoryCapExceededFlag,
    OEMGSMSMS_GetTPMR,
    OEMGSMSMS_SetTPMR,

    OEMGSMSMS_CreateDefaultMessage,
    OEMGSMSMS_DecodeMessage,
    OEMGSMSMS_DecodeUserData,
    OEMGSMSMS_EncodeUserData,
    OEMGSMSMS_InjectMessage
};

static struct _IGSMSMS sGSMSMS = {
                                    &gOEMGSMSMSFuncs,
                                    NULL, // m_pIShell
                                    NULL, // m_pIFileMgr
                                    NULL, // m_pSMSMbox
                                    NULL, // m_pVMMbox
                                    FALSE,// m_bInitialized
                                    FALSE,// m_bSIMSMSEnabled
                                    FALSE,// m_bCardHandlerIsInit
                                    FALSE,// m_bSMMAPending
                                    0,    // m_DeliverRP_MR
                                    0,    // m_SubmitRP_MR
                                    NULL, // m_pCallback
                                 };

static const IGSMSMSNotifierVtbl gOEMGSMSMSNotifierFuncs = {
    OEMGSMSMSNotifier_AddRef,
    OEMGSMSMSNotifier_Release,
    OEMGSMSMSNotifier_SetMask
};

STATIC struct _IGSMSMSNotifier sGSMSMSNotifier = { 
                                    &gOEMGSMSMSNotifierFuncs, 
                                    0, 
                                    0,
                                    FALSE 
                                 };

// callback structure to handle new incoming messages
static AEECallback        gNewMessageCB = {0};


// callback structure and data to support RerouteTapi
static boolean gRerouteTapiTransactionInProgress = FALSE;
static RerouteTapiParameters gRerouteTapiParameters;
static AEECallback gRerouteTapiCallback;


//static OEMCriticalSection gGSMSMSCriticalSection;

// Temp variables that are used in a single place, and are allocated statically
// instead of on the stack 
static GSMSMSMessage      gSMSMessage;
static AECHAR             gszMessageInUnicode[256];
static char               gszMessageInBytes[128];


/*===========================================================================

                     BREW Methods

===========================================================================*/

//***************************************************************************
// GSMSMS BREW Framework Functions
//***************************************************************************
void OEMGSMSMSNotifier_Init(IShell * ps)
{
   IGSMSMSNotifier * pTmpGSMSMSNotifier;

   // Callback used to switch into BREW context after MT message arrives
   CALLBACK_Init(&gNewMessageCB, GSMSMS_ProcessCache, NULL);

   if (OEMGSMSMSNotifier_New(ps, AEECLSID_IGSMSMSNOTIFIER, (void *)&pTmpGSMSMSNotifier) != SUCCESS)
   {
      FARF(ERROR, ("Unable to init IGSMSMSNotifier."));
   }
}


int OEMGSMSMSNotifier_New(IShell * ps, AEECLSID cls,void **ppif)
{
   if (cls != AEECLSID_IGSMSMSNOTIFIER) 
   {
      return ECLASSNOTSUPPORT;
   }

   if (sGSMSMSNotifier.m_bInitialized)
   {
     *ppif = &sGSMSMSNotifier;
      return SUCCESS;
   }
   sGSMSMSNotifier.m_pIShell = ps;
   ISHELL_AddRef(ps);

   sGSMSMSNotifier.m_bInitialized = TRUE;
   return SUCCESS;
}


//
// OEMGSMSMSNotifier_AddRef
//
static uint32 OEMGSMSMSNotifier_AddRef(struct _IGSMSMSNotifier * pMe)
{
   FARF(UNITTEST, ("Called OEMGSMSMSNotifier_AddRef"));

   return 1;
}

//
// OEMGSMSMSNotifier_Release
//
static uint32 OEMGSMSMSNotifier_Release(struct _IGSMSMSNotifier * pMe)
{

   FARF(UNITTEST, ("Called OEMGSMSMSNotifier_Release"));

   return 1;
}

//
// OEMGSMSMSNotifier_SetMask
//
static void OEMGSMSMSNotifier_SetMask(struct _IGSMSMSNotifier * pMe, uint32 * pdwMask)
{  
   uint32   dw, dwm;

   dwm = 0;
   while((dw = *pdwMask) != 0)
   {
      dwm |= GET_NOTIFIER_MASK(dw);
      pdwMask++;
   }

   FARF(UNITTEST, ("Setmask called %d", pMe->m_dwNotify));
   pMe->m_dwNotify = dwm;
}  

//***************************************************************************
// GSMSMS BREW Framework Functions
//***************************************************************************
void OEMGSMSMS_Init(IShell * ps)
{
   IGSMSMS * pTmpGSMSMS;

   FARF(UNITTEST, ("Called OEMGSMSMS_Init"));

   OEMGSMSMS_SVC_Init();

   if (OEMGSMSMS_New(ps, AEECLSID_IGSMSMS, (void *)&pTmpGSMSMS) != SUCCESS)
   {
      FARF(ERROR, ("OEMGSMSMS_New failed"));
   }
}

//
// OEMGSMSMS_New
//
int OEMGSMSMS_New(IShell * ps, AEECLSID cls,void **ppif)
{
   int                rv;

   FARF(UNITTEST, ("Called OEMGSMSMS_New"));

   *ppif = 0;

   if (cls != AEECLSID_IGSMSMS) 
   {
      return ECLASSNOTSUPPORT;
   }

   if (sGSMSMS.m_bInitialized) 
   {
      *ppif = &sGSMSMS;
      return SUCCESS;
   }

   sGSMSMS.m_pIShell = ps;

   *ppif = &sGSMSMS;

   // Prepare the SIM interface
   sGSMSMS.m_bCardHandlerIsInit = OEMGSM1xCardHandler_Init();

   // OEMGSM1xCardHandler_Init can fail if SIM is not CHV enabled
   if (sGSMSMS.m_bCardHandlerIsInit)
   {
      sGSMSMS.m_bSIMSMSEnabled = OEMGSM1xCardHandler_IsSMSEnabled();
   }
   else
   {
      sGSMSMS.m_bSIMSMSEnabled = FALSE;
   }

   if (ISHELL_CreateInstance(sGSMSMS.m_pIShell, AEECLSID_FILEMGR,
                              (void **)&sGSMSMS.m_pIFileMgr) != SUCCESS) 
   {
      FARF(ERROR, ("ISHELL_CreateInstance failed"));
      sGSMSMS.m_pIShell = NULL;
      return EFAILED;
   }

   rv = GSMSMS_InitFile(NVRAM_DB_NAME, GSMSMS_NUM_NV_ENTRIES);
   if (rv != SUCCESS)
   {
      (void)IFILEMGR_Release(sGSMSMS.m_pIFileMgr);
      sGSMSMS.m_pIFileMgr = NULL;

      sGSMSMS.m_pIShell = NULL;

      return rv;
   }

   rv = GSMSMS_InitFile(NVRAM_VM_DB_NAME, GSMSMS_NUM_NV_VM_ENTRIES);
   if (rv != SUCCESS)
   {
      (void)IFILEMGR_Release(sGSMSMS.m_pIFileMgr);
      sGSMSMS.m_pIFileMgr = NULL;

      sGSMSMS.m_pIShell = NULL;

      return rv;
   }

   GSMSMS_InitGSMCharMap();

   sGSMSMS.m_bSMMAPending = FALSE;
   sGSMSMS.m_bInitialized = TRUE;

   // Let IShell instance know we need it
   ISHELL_AddRef(sGSMSMS.m_pIShell);

   // check if card handler init in one second
   (void)ISHELL_SetTimer(sGSMSMS.m_pIShell, 10000,
                         GSMSMS_InitTimerCallback, (void *)&sGSMSMS);
   
   return SUCCESS;
}

//
// GSMSMS_InitFile
//
static int GSMSMS_InitFile(const char *name, uint16 entries)
{
   uint16  i;
   uint8 buf[GSMSMS_NV_MBOX_RECORD_LEN];
   int32 count; 
   IFile *fp;

   // Create the file if it doesn't exist
   fp = IFILEMGR_OpenFile(sGSMSMS.m_pIFileMgr, name, _OFM_CREATE);

   // Assume the file already exists if NULL
   if (fp == NULL)
   {
       // Read in the SMS NV Databases
       fp = IFILEMGR_OpenFile(sGSMSMS.m_pIFileMgr, name, _OFM_READWRITE);

       if (fp == NULL)
       {
          FARF(ERROR, ("IFILEMGR_OpenFile failed"));
          return EFAILED;
       }
   }

   // verify that the file is 
   // GSMSMS_NUM_NV_ENTRIES*GSMSMS_MAX_SIM_MESSAGE_LEN long
   for (i=0; i < entries; i++)
   {
      (void)IFILE_Seek(fp, _SEEK_START, GSMSMS_NV_MBOX_RECORD_LEN*i);

      // Verify the record starts with a valid record marker
      count = IFILE_Read(fp, &buf[0], GSMSMS_NV_MBOX_RECORD_LEN);
      if (count != GSMSMS_NV_MBOX_RECORD_LEN || 
          buf[0] != GSMSMS_NV_MBOX_RECORD_MARKER)
      {
         //Create an empty record
         buf[0] = GSMSMS_NV_MBOX_RECORD_MARKER;
         buf[1] = GSMSMS_STATUS_NONE;
         MEMSET(&buf[2], 0xff, GSMSMS_NV_MBOX_RECORD_LEN-2);

         (void)IFILE_Seek(fp, _SEEK_START, GSMSMS_NV_MBOX_RECORD_LEN*i);

         if (IFILE_Write(fp, buf, GSMSMS_NV_MBOX_RECORD_LEN) != GSMSMS_NV_MBOX_RECORD_LEN)
         {
            FARF(ERROR, ("IFILE_Write failed"));
            IFILE_Release(fp);
            return EFAILED;
         }
      }
   }
   IFILE_Release(fp);

   return SUCCESS;
}

//
// OEMGSMSMS_AddRef
//
static uint32 OEMGSMSMS_AddRef(struct _IGSMSMS * pMe)
{
   FARF(UNITTEST, ("Called OEMGSMSMS_AddRef"));

   return 1;
}

//
// OEMGSMSMS_Release
//
static uint32 OEMGSMSMS_Release(struct _IGSMSMS * pMe)
{

   FARF(UNITTEST,("Called OEMGSMSMS_Release"));

   return 1;
}


/*=============================================================================
FUNCTION: OEMGSMSMS_QueryInterface                        EXTERNAL FUNCTION 

DESCRIPTION:
   This method may be used to request an alternative interface to the 
   IGSMSMS. Currently no alternative interface is defined.
        
PARAMETERS:
   pMe   : Pointer to the IGSMSMS object.
   clsid : Class id of the interface requested. 
   ppNew : returns pointer to the new interface requested.
           NULL if the requested interface is  not supported.

RETURN VALUE:
   SUCCESS       -  ppNew contains a pointer to the requested interface.
   EUNSUPPORTED  -  requested interface is not supported.

SIDE EFFECTS:
   None
=============================================================================*/
static int OEMGSMSMS_QueryInterface(struct _IGSMSMS *pMe, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEECLSID_IGSMSMS || clsid == AEECLSID_QUERYINTERFACE)
   {
      *ppNew = (void *)pMe;

      (void)OEMGSMSMS_AddRef(pMe);
      return SUCCESS;
   }

   ppNew=0;
   return ECLASSNOTSUPPORT;
}

/*===========================================================================

                     IGSMSMS Methods   

===========================================================================*/

/*=============================================================================
FUNCTION: OEMGSMSMS_SendSMSSubmit

DESCRIPTION:
   Sends a GSM SMS_SUBMIT message tunnelled in a IS-637 message
        
   When the SMS_SubmitReport message is received, the pCB function will be 
   called and the status can be retrieved from the GSMSMSSendReport structure.

   If a status report message was requested, the SMS_StatusReport message 
   will be delivered via the callback specified in the OnMTMessage 
   registration.

PROTOTYPE:
   int IGSMSMS_SendSMSSubmit(struct _IGSMSMS *po, const GSMSMSSubmitType * pMsg, 
                              AEECallback *pCb, GSMSMSSendReport *pReport)

PARAMETERS:
   po:      Pointer to the IGSMSMS object
   pMsg:    Pointer to the SMS Submit message to send
   pCb:     This callback will be invoked by AEE when the SMS_SUBMIT_REPORT is received.
   pReport: Pointer to structure to be filled in reporting the submit status

RETURN VALUE:
   AEE_GSMSMS_EBUSY if a transaction is pending
   AEE_GSMSMS_EBADPARAM if a pMsg == NULL or pCb == NULL
   AEE_GSMSMS_EENCODE if the message could not be encoded
   AEE_GSMSMS_ENOSERVICE if the phone is not in GSM1x mode
   AEE_GSMSMS_SUCCESS if successful
  

SIDE EFFECTS:
   None
=============================================================================*/
static int OEMGSMSMS_SendSMSSubmit(struct _IGSMSMS *pMe, GSMSMSSubmitType const * pMsg, AEECallback *pCb, GSMSMSSendReport *pReport)
{
   GSMSMSRawData rawMsg;
   byte *SMS_Submit, *RPUDL;
   uint8 idx = 0;
   int udByteLen;
   byte len;
   int rv;

   FARF(UNITTEST, ("Called OEMGSMSMS_SendSMSSubmit"));

    // See if a transaction is pending
    if (pMe->m_pCallback != NULL)
    {
       return AEE_GSMSMS_EBUSY;
    }

    if (pMsg == NULL || pCb == NULL)
    {
       return AEE_GSMSMS_EBADPARAM;
    }

    // Store data to correlate with SMS_SUBMIT_REPORT
    pMe->m_SubmitRP_MR = gRefNum;
    FARF(LOW, ("Set RP_MR = %d",pMe->m_SubmitRP_MR));

    CALLBACK_Cancel(pCb);

    // RP_MO_DATA
    rawMsg.data[idx++] = GSMSMS_RP_MO_DATA;   // RP_MTI
    rawMsg.data[idx++] = gRefNum++;           // RP_MR
    
    rawMsg.data[idx++] = 0;                   // RP_OA
    
    // RP_DA
    if ((rv = GSMSMS_EncodeAddress(&pMsg->SCAddr, &rawMsg.data[idx], TRUE)) < 0)
    {
       FARF(ERROR, ("GSMSMS_EncodeAddress failed"));
       return AEE_GSMSMS_EENCODE;
    }
    idx+=(byte)rv;


    // store index offset for later use (index of TP-MTI byte)
    rawMsg.data[idx] = idx+1;                   // RP UserData length
    RPUDL = &rawMsg.data[idx++];

    // Append the GSM SMS_SUBMIT message TL data
    SMS_Submit=&(rawMsg.data[0]);
    
    // format TLParam as GSM data here
    SMS_Submit[idx++] = 
        1 | //TP-MTI  2 bits SMS_SUBMIT (01)
        (byte)(pMsg->RD << 2) |   //TP-RD  1 bit  Reject Duplicates (0)
        (byte)(pMsg->VP.format << 3) | //TP-VPF 2 bits No validity field (00)
        (byte)(pMsg->SRR << 5) |  //TP-SRR 1 bit  Status report requested
        (byte)(pMsg->UDHI << 6) | //TP-UDHI 1 bit No user data header
        (byte)(pMsg->RP << 7);    //TP-RP  1 bit  Reply path not set (0)

    //TP-MR
    SMS_Submit[idx++] = pMsg->MR;

    // This functionality needs to change
    if ((rv = GSMSMS_EncodeAddress(&pMsg->DA, &SMS_Submit[idx], FALSE)) < 0)
    {
       FARF(ERROR, ("GSMSMS_EncodeAddress failed"));
       return AEE_GSMSMS_EENCODE;
    }
    idx+=(byte)rv;

    SMS_Submit[idx++] = pMsg->PID; //TP-PID 1 octet (SME to SME)
    
    // TP-DCS
    GSMSMS_EncodeDCS(&SMS_Submit[idx++], &pMsg->DCS);

    // TP-VP
    if (GSMSMS_EncodeVP(&pMsg->VP, &SMS_Submit[idx], &len) == FALSE)
    { 
       return AEE_GSMSMS_EENCODE;
    }
    idx+=len;

    SMS_Submit[idx++] = pMsg->UDL;

    if ((udByteLen = CalcUDLengthInBytes(pMsg->UDL, pMsg->DCS, pMsg->UDHI, pMsg->UD)) < 0) {
        return AEE_GSMSMS_EENCODE;
    }

    //TP-UD
    MEMCPY((void *)&SMS_Submit[idx], pMsg->UD, udByteLen);
    idx+=(byte)udByteLen;

    *RPUDL = idx-*RPUDL;
    FARF(LOW, ("After Encode: UDL = %d, idx = %d",pMsg->UDL, idx));

    // Set the cancel CB
    pCb->pfnCancel = GSMSMS_CancelCb;
    pCb->pCancelData = pMe;
    pMe->m_pReport = (void *)pReport;
    pMe->m_pCallback = pCb;

    rv = GSMSMS_SendSMSRaw(
           (const byte *)rawMsg.data,
           idx,
           USER_CB_IPHONE,
           TRUE); 

   if (rv != AEE_GSMSMS_SUCCESS)
   {
      // The transaction was never started so 
      // do not cancel it
      pCb->pfnCancel = NULL;
      pCb->pCancelData = NULL;
      pMe->m_pReport = NULL;
      pMe->m_pCallback = NULL;
      return rv;
   }

   // Set and expiration timer for the transaction
   (void)ISHELL_SetTimer(pMe->m_pIShell, GSMSMS_TIMEOUT_VAL, 
            GSMSMS_TimerCallback, (void *)pMe);

   return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMGSMSMS_SendSMSDeliverReport

DESCRIPTION:

   This function allows an application to send a GSM SMS_DELIVER_REPORT message. 
   This should be sent in response to receiving an SMS_DELIVER message.

   The user would first fill out a GSMSMSDeliverReportType structure either manually
   or by calling IGSMSMS_CreateDefaultMessage to fill in the structure and only
   modifying the parameters that are different from the default. For a simple 
   SMS_DELIVER_REPORT message, this would typically be the UserData (GSMSMSSubmitType->UD) 
   and the destination address (GSMSMSSubmitType->DA) for a simple SMS message.
   
PROTOTYPE:
   int OEMGSMSMS_SendSMSDeliverReport(struct _IGSMSMS *pMe, 
                                    GSMSMSDeliverReportType const *pDeliverReport)

PARAMETERS:
   pMe:            Pointer to the IGSMSMS object
   pDeliverReport: Pointer to the SMS Deliver Report message to send

RETURN VALUE:
   If successful: AEE_GSMSMS_SUCCESS. 
   If the message was not valid: AEE_GSMSMS_EFAILED 
   If the the paremeters were not valid: AEE_GSMSMS_EBADPARAM 
   AEE_GSMSMS_ENOSERVICE if the phone is not in GSM1x mode

SIDE EFFECTS:
    None
=============================================================================*/
static int OEMGSMSMS_SendSMSDeliverReport(struct _IGSMSMS *pMe, GSMSMSDeliverReportType const *pDeliverReport)
{
   GSMSMSRawData rawMsg;
   byte *DeliverRpt, *RPUDL;
   uint8 idx = 0;
   int rv;

   FARF(UNITTEST, ("Called OEMGSMSMS_SendSMSDeliverReport"));

   if (pDeliverReport == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }
   // clear the user data
   MEMSET((void *)&rawMsg, 0x0, sizeof(rawMsg));

   DeliverRpt = (byte *)rawMsg.data;

   if (pDeliverReport->FCS_present)
   {
      DeliverRpt[idx++] = GSMSMS_RP_MO_ERROR;
      DeliverRpt[idx++] = pMe->m_DeliverRP_MR;
      DeliverRpt[idx++] = 1;    // length (spec doesn't say how to encode this)

      // Association of GSM 23.040 section 9.2.3.22 with GSM 24.11 table 8.4
      switch (pDeliverReport->FCS)
      {
      case 0xD3: // Memory Capacity Exceeded
         DeliverRpt[idx++] = GSMSMS_RP_CAUSE_MEM_CAP_EXCEEDED;
         break;
 
      case 0xD0: // USIM Storage full (SIM full, not NV)
      case 0xD2: // Error in MS       (NV full, not SIM)
      default:
         DeliverRpt[idx++] = GSMSMS_RP_CAUSE_PROTOCOL_ERROR;
         break;
      }
   }
   else
   {
      DeliverRpt[idx++] = GSMSMS_RP_MO_ACK;
      DeliverRpt[idx++] = pMe->m_DeliverRP_MR;
   }
   DeliverRpt[idx++] = GSMSMS_USER_DATA_IEI; 

   // Store offset for later
   DeliverRpt[idx] = idx+1;
   RPUDL = &DeliverRpt[idx++];

   DeliverRpt[idx++] = 
       0 | //TP-MTI  2 bits SMS_DELIVER (00)
       (byte)(pDeliverReport->UDHI << 3);  //TP-UDHI 1 bit No header (0)

   if (pDeliverReport->FCS_present)
   {
      DeliverRpt[idx++] = pDeliverReport->FCS; 
   }

   DeliverRpt[idx++] = pDeliverReport->PID_present | 
                       (byte)(pDeliverReport->DCS_present << 1) | 
                       (byte)(pDeliverReport->UDL_present << 2); // TP-PI

   if (pDeliverReport->PID_present)
      DeliverRpt[idx++] = pDeliverReport->PID;  // TP-PID

   if (pDeliverReport->DCS_present)
      GSMSMS_EncodeDCS(&DeliverRpt[idx++], &pDeliverReport->DCS);  // TP-DCS

   if (pDeliverReport->UDL_present)
   {
      int udByteLen;

      if ((udByteLen = CalcUDLengthInBytes(pDeliverReport->UDL, 
                                           pDeliverReport->DCS, 
                                           pDeliverReport->UDHI, 
                                           pDeliverReport->UD)) < 0) {
          return AEE_GSMSMS_EENCODE;
      }
      DeliverRpt[idx++] = pDeliverReport->UDL;   // TP-UDL
      MEMCPY(&DeliverRpt[idx], pDeliverReport->UD, udByteLen); // TP-UD
      idx+=(byte)udByteLen;
   }

   *RPUDL = idx - (*RPUDL);

   rv = GSMSMS_SendSMSRaw(
           (const byte *)rawMsg.data,
           idx,
           USER_CB_NONE,
           FALSE); 

   return rv;
}

/*=============================================================================
Function: GSMSMS_StoreAtIndex()

Description:

   This function stores a GSM SMS message on the SIM or to NV RAM store at  
   the specified slot index.

Prototype:
   int GSMSMS_StoreAtIndex(struct _IGSMSMS *pMe, uint16 index, GSMSMSMsg * pMsg, 
                           GSMSMSStorageType storeTo)

Parameters:
   pMe:     Pointer to the IGSMSMS object
   index:   Slot index to store the message. 
   pMsg:    Pointer to the GSM SMS Msg to store
   storeTo: Indicates whether to store to the SIM or to NVRAM database

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If the message could not be stored: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None
=============================================================================*/
static int GSMSMS_StoreAtIndex
(
   struct _IGSMSMS *pMe, 
   uint16 index, 
   const GSMSMSMsg * pMsg, 
   GSMSMSStorageType storeTo
)
{
   GSMSMSRawMsg rawMsg;

   FARF(UNITTEST,("Called GSMSMS_StoreAtIndex"));

   if (pMsg == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   // Convert the message
   if (GSMSMS_ConvertToSIMRaw(pMsg, &rawMsg) != AEE_GSMSMS_SUCCESS)
   {
      // TODO indicate invalid parameter error
      return AEE_GSMSMS_EFAILED;
   }

   switch (storeTo)
   {
   case GSMSMS_STORE_SIM:
       if (!pMe->m_bSIMSMSEnabled)
       {
          return AEE_GSMSMS_ESST;
       }

       // Store the message on the SIM
       if (OEMGSM1xCardHandler_SetSms(index, (CardHandlerRawSMSType *)&rawMsg.msg.sim.rawMsg[0]) == UIM_FAIL)
       {
          FARF(ERROR, ("OEMGSM1xCardHandler_SetSms failed"));
          return AEE_GSMSMS_EFAILED;
       }
       break;

   case GSMSMS_STORE_NVRAM:
   case GSMSMS_STORE_NVRAM_VM:
       {
          IFile *pIFile = NULL;

          // This IFile pointer is released by a timer
          // so do not add a release here
          if ((pIFile = GSMSMS_OpenDB(pMe, storeTo)) == NULL) 
          {
             FARF(ERROR, ("Database Open failed"));
             return AEE_GSMSMS_EFAILED;
          }

          if (index >= GSMSMS_NUM_NV_ENTRIES)
          {
             return AEE_GSMSMS_EBADPARAM;
          }

          IFILE_Seek(pIFile, _SEEK_START, index*GSMSMS_NV_MBOX_RECORD_LEN+1);
          if (IFILE_Write(pIFile, &rawMsg.msg.sim.rawMsg[0], sizeof(rawMsg.msg.sim.rawMsg)) != sizeof(rawMsg.msg.sim.rawMsg))
          {
             FARF(ERROR, ("Write to MBOX failed"));
             return AEE_GSMSMS_EFAILED;
          }
       }
       break;

   default:
      return AEE_GSMSMS_EBADPARAM;
   }

   return AEE_GSMSMS_SUCCESS;
}
/*=============================================================================
Function: OEMGSMSMS_StoreMessage()

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
   int OEMGSMSMS_StoreMessage(struct _IGSMSMS *pMe, GSMSMSMsg * pMsg, 
                             GSMSMSStorageType storeTo, uint16 *pIndex )

Parameters:
   pMe:     Pointer to the IGSMSMS object
   pMsg:    Pointer to the GSM SMS Msg to store
   storeTo: Indicates whether to store to the SIM or to NVRAM database
   pIndex:  Pointer to variable containing the index to to store the message at.
            If the index is set to GSMSMS_INDEX_ANY an index will be selected
            and the value of *pIndex will be updated.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If the store was full: AEE_GSMSMS_ESTORE_FULL
   If the message could not be stored: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None
=============================================================================*/
static int OEMGSMSMS_StoreMessage
(
   struct _IGSMSMS *pMe, 
   const GSMSMSMsg * pMsg, 
   GSMSMSStorageType storeTo,
   uint16 *pIndex
)
{
   FARF(UNITTEST, ("Called OEMGSMSMS_StoreMessage"));

   if (pMsg == NULL || pIndex == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   if (*pIndex == 0xFFFF && 
       (GSMSMS_GetFreeSlot(pMe, pIndex, storeTo) != AEE_GSMSMS_SUCCESS))
   {
      return AEE_GSMSMS_ESTORE_FULL;
   }

   return GSMSMS_StoreAtIndex(pMe, *pIndex, pMsg, storeTo);
}

/*=============================================================================
Function: OEMGSMSMS_MoveMessage()

Description:

   This function moves a GSM SMS message from one store to another. It will also
   allow the the user to move from one slot to another on the same store.

Prototype:
   int OEMGSMSMS_MoveMessage(struct _IGSMSMS *pMe, GSMSMSStorageType moveFrom, uint16 fromIndex,
                             GSMSMSStorageType moveTo, uint16 pToIndex )

Parameters:
   pMe:       Pointer to the IGSMSMS object
   moveFrom:  Store to move the message from
   fromIndex: Index of the message to move
   moveTo:    Store to move the message to
   pToIndex:  Pointer to the index of the stored message, if set to 
              GSMSMS_INDEX_ANY an empty slot will be selected and returned

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If the store was full: AEE_GSMSMS_ESTORE_FULL
   If the message could not be stored: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None
=============================================================================*/
static int OEMGSMSMS_MoveMessage
(
   struct _IGSMSMS *pMe, GSMSMSStorageType moveFrom, uint16 fromIndex, GSMSMSStorageType moveTo, uint16 *pToIndex
)
{
   GSMSMSRawMsg rawMsg;
   int rv;

   FARF(UNITTEST, ("Called OEMGSMSMS_MoveMessage"));

   if (pToIndex == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   if (*pToIndex == 0xFFFF && 
       (GSMSMS_GetFreeSlot(pMe, pToIndex, moveTo) != AEE_GSMSMS_SUCCESS))
   {
      return AEE_GSMSMS_ESTORE_FULL;
   }

   if ((rv = OEMGSMSMS_GetMessage(pMe, fromIndex, &rawMsg, moveFrom)) != 
           AEE_GSMSMS_SUCCESS)
   {
      return rv;
   }

   if (moveTo == GSMSMS_STORE_SIM)
   {
       if (!pMe->m_bSIMSMSEnabled)
       {
          return AEE_GSMSMS_ESST;
       }

       // Store the message on the SIM
       if (OEMGSM1xCardHandler_SetSms(*pToIndex, 
              (CardHandlerRawSMSType *)&rawMsg.msg.sim.rawMsg[0]) == UIM_FAIL)
       {
          FARF(ERROR, ("OEMGSM1xCardHandler_SetSms failed"));
          return AEE_GSMSMS_EFAILED;
       }
   }
   else if (moveTo == GSMSMS_STORE_NVRAM)
   {
      IFile *pIFile = NULL;

      // This IFile pointer is released by a timer
      // so do not add a release here
      if ((pIFile = GSMSMS_OpenDB(pMe, moveTo)) == NULL) 
      {
         FARF(ERROR, ("Database Open failed"));
         return AEE_GSMSMS_EFAILED;
      }
       
      IFILE_Seek(pIFile, _SEEK_START, (*pToIndex)*GSMSMS_NV_MBOX_RECORD_LEN+1);
      if (IFILE_Write(pIFile, &rawMsg.msg.sim.rawMsg[0], sizeof(rawMsg.msg.sim.rawMsg)) != sizeof(rawMsg.msg.sim.rawMsg))
      {
         FARF(ERROR, ("Write to MBOX failed"));
         return AEE_GSMSMS_EFAILED;
      }
   }
   // Delete the original message
   return OEMGSMSMS_DeleteMessage(pMe, fromIndex, moveFrom);
}

/*=============================================================================
Function: OEMGSMSMS_StoreStatusReport()

Description:

   This function stores a GSM SMS_StatusReport message for the SMS_Submit 
   message stored at the specified slot on the SIM or in NV RAM.

   In GSM, the SMSR directory on the SIM contains the StatusReport. This
   entry contains an index into the SMS directory for the corresponding
   SMS Submit message. When the entry in the SMS directory is deleted,
   the corresponding entry in the SMSR is no longer valid.

   Therefore, the status report must be stored on the same store as the 
   SMS_Submit message. If the SMS_Submit message is ever moved to another store,
   the SMS_StatusReport must be moved at the same time or it will be lost.

   If the status report is stored in NVRAM only the portion of the message
   that would be saved to the SMSR is preserved.

Prototype:
   int OEMGSMSMS_StoreStatusReport(struct _IGSMSMS *po, uint16 index, 
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
  If the message could not be stored: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

=============================================================================*/
static int OEMGSMSMS_StoreStatusReport
(
   struct _IGSMSMS *pMe, 
   uint16 index, 
   const GSMSMSMsg * pMsg, 
   GSMSMSStorageType storeTo
)
{
   GSMSMSRawMsg smsmsg; 
   int rv;

   FARF(UNITTEST, ("Called OEMGSMSMS_StoreStatusReport"));

   if (pMsg == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   rv = OEMGSMSMS_GetMessage(pMe, index, &smsmsg, storeTo);
   if (rv != AEE_GSMSMS_SUCCESS)
   {
      return EFAILED;
   }

   if (smsmsg.msg.sim.status != GSMSMS_STATUS_MO_SENT_ST_NOT_RECEIVED && 
       smsmsg.msg.sim.status != GSMSMS_STATUS_MO_SENT_ST_NOT_STORED)
   {
      return AEE_GSMSMS_EFAILED;
   }

   switch (storeTo)
   {
   case GSMSMS_STORE_SIM:
      if (!pMe->m_bSIMSMSEnabled)
      {
         return AEE_GSMSMS_ESST;
      }

      // NO SMSR on SIM
      return AEE_GSMSMS_EFAILED;

   case GSMSMS_STORE_NVRAM:
      // Conversion from SMSStatusReport to Raw format not implemented
      return AEE_GSMSMS_EFAILED;
   default:
      return AEE_GSMSMS_EBADPARAM;
   }
   
   return AEE_GSMSMS_SUCCESS;
}

/*===========================================================================
Function: OEMGSMSMS_DeleteAllMessages()

Description:

   This function deletes all entries of a specified type from the store 
   (SIM, NVRAM or NVRAM Voivemail). It provides a way to delete only mobile 
   originated (MO), mobile terminated (MT), or all messages.

Prototype:
   int OEMGSMSMS_DeleteAllMessages(struct _IGSMSMS * pMe, uint16 msgMask, GSMSMSStorageType deleteFrom)

Parameters:
   pMe:     Pointer to the IGSMSMS object
   msgMask: Indicates the type of messages to delete. Any combination of the following:
               GSMSMS_SIM_MO 
               GSMSMS_SIM_MT 
               GSMSMS_SIM_ALL 
            
   deleteFrom: Indicates whether to delete from the SIM or from NVRAM database.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If deleteFrom is invalid: AEE_GSMSMS_EBADPARAM 
   If the messages could not be deleted: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_DeleteAllMessages(struct _IGSMSMS * pMe, uint16 mask, GSMSMSStorageType deleteFrom)
{
   FARF(UNITTEST, ("Called OEMGSMSMS_DeleteAllMessages"));

   switch (deleteFrom)
   {
   case GSMSMS_STORE_SIM:
       {
           if (!pMe->m_bSIMSMSEnabled)
           {
              return AEE_GSMSMS_ESST;
           }

           if ((mask == GSMSMS_SIM_ALL)  && 
               (OEMGSM1xCardHandler_ClearAllSms() == UIM_FAIL))
           {
              return AEE_GSMSMS_EFAILED;
           }
           else if (mask)
           {
               uint16 i;
               uint16 simSMSCount;
               byte status;
           
               if (OEMGSM1xCardHandler_IsSMSInit() == FALSE)
               {
                  return AEE_GSMSMS_EBUSY;
               }

               simSMSCount = OEMGSM1xCardHandler_GetNumSMSSlots();
               for (i=0; i< simSMSCount; i++)
               {
                   if (OEMGSM1xCardHandler_GetSMSStatus((uint8)i, &status) != UIM_PASS)
                   {
                      return AEE_GSMSMS_EFAILED;
                   }

                   if (((mask == GSMSMS_SIM_MT) && ((status & 0x05) == 0x01)) ||
                       ((mask == GSMSMS_SIM_MO) && (status & 0x05) == 0x05))
                   {
                      if (OEMGSM1xCardHandler_ClearSms((byte)i) != UIM_PASS)
                      {
                         return AEE_GSMSMS_EFAILED;
                      }
                   }
               }
           }
       }
       break;

   case GSMSMS_STORE_NVRAM:
   case GSMSMS_STORE_NVRAM_VM:
       {
           IFile *pIFile = NULL;
           uint8 buf[GSMSMS_NV_MBOX_RECORD_LEN];
           uint16 index;

           if ((pIFile = GSMSMS_OpenDB(pMe, deleteFrom)) == NULL) 
           {
              FARF(ERROR, ("Database Open failed"));
              return AEE_GSMSMS_EFAILED;
           }

           for (index=0; index < GSMSMS_NUM_NV_ENTRIES; index++)
           {
              IFILE_Seek(pIFile, _SEEK_START, index*GSMSMS_NV_MBOX_RECORD_LEN);
              if (IFILE_Read(pIFile, &buf[0], 2) == 2)
              {
                 if (((mask == GSMSMS_SIM_MO) && ((buf[1] & 0x05) == 0x05)) ||
                     ((mask == GSMSMS_SIM_MT) && ((buf[1] & 0x05) == 0x01)) ||
                     (mask == GSMSMS_SIM_ALL))
                 {
                    buf[1] = GSMSMS_STATUS_NONE;
                    MEMSET(&buf[2], 0xff, GSMSMS_NV_MBOX_RECORD_LEN-2);
                    IFILE_Seek(pIFile, _SEEK_START, index*GSMSMS_NV_MBOX_RECORD_LEN);
                    if (IFILE_Write(pIFile, buf, GSMSMS_NV_MBOX_RECORD_LEN) != GSMSMS_NV_MBOX_RECORD_LEN)
                    {
                       FARF(ERROR, ("Write to MBOX failed"));
                       return AEE_GSMSMS_EFAILED;
                    }
                 }
              }
              else
              {
                 return AEE_GSMSMS_EFAILED;
              }
           } 
       }
       break;

   default:
      return AEE_GSMSMS_EBADPARAM;
   }
   return AEE_GSMSMS_SUCCESS;
}

/*===========================================================================
Function: OEMGSMSMS_DeleteMessage()

Description:

   This function deletes an entry from a specified slot on the store 
   (SIM, NVRAM or NVRAM Voicemail).

Prototype:
   int OEMGSMSMS_DeleteMessage(struct _IGSMSMS * pMe, uint16 index, 
                               GSMSMSStorageType deleteFrom)

Parameters:
   pMe:        Pointer to the IGSMSMS object
   index:      Slot index of message to delete
   deleteFrom: Indicates whether to delete from the SIM or from NVRAM database.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If deleteFrom is invalid: AEE_GSMSMS_EBADPARAM 
   If the messages could not be deleted: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_DeleteMessage(struct _IGSMSMS * pMe, uint16 index, GSMSMSStorageType deleteFrom)
{
   FARF(UNITTEST, ("Called OEMGSMSMS_DeleteMessage"));

   switch (deleteFrom)
   {
   case GSMSMS_STORE_SIM:

       if (!pMe->m_bSIMSMSEnabled)
       {
          return AEE_GSMSMS_ESST;
       }

       if (OEMGSM1xCardHandler_ClearSms(index) == UIM_FAIL)
       {
          return AEE_GSMSMS_EFAILED;
       }
       break;
   case GSMSMS_STORE_NVRAM:
   case GSMSMS_STORE_NVRAM_VM:
       {
           IFile *pIFile = NULL;
           uint8 buf[GSMSMS_NV_MBOX_RECORD_LEN];

           if ((pIFile = GSMSMS_OpenDB(pMe, deleteFrom)) == NULL) 
           {
              FARF(ERROR, ("Database Open failed"));
              return AEE_GSMSMS_EFAILED;
           }

           if (index >= GSMSMS_NUM_NV_ENTRIES)
           {
              return AEE_GSMSMS_EBADPARAM;
           }

           // Don't overwrite the record marker, skip buf[0]
           buf[1] = GSMSMS_STATUS_NONE;
           MEMSET(&buf[2], 0xff, GSMSMS_NV_MBOX_RECORD_LEN-2);
           (void)IFILE_Seek(pIFile, _SEEK_START, index*GSMSMS_NV_MBOX_RECORD_LEN+1);
           if (IFILE_Write(pIFile, &buf[1], GSMSMS_NV_MBOX_RECORD_LEN-1) != GSMSMS_NV_MBOX_RECORD_LEN-1)
           {
              FARF(ERROR, ("Write to MBOX failed"));
              return AEE_GSMSMS_EFAILED;
           }
       }
       break;

   default:
      return AEE_GSMSMS_EBADPARAM;
   }
   return AEE_GSMSMS_SUCCESS;
}

/*===========================================================================
Function: OEMGSMSMS_GetMessage()

Description:

   This function retrieves an SMS entry from a specified slot on the store 
   (SIM or NVRAM).

   The message is retrieved in raw format and can be decoded using 
   IGSMSMS_DecodeMessage and IGSMSMS_DecodeUserData.

Prototype:
   int OEMGSMSMS_GetMessage(struct _IGSMSMS * pMe, uint16 index, GSMSIMMsg *pMsg, 
                            GSMSMSStorageType readFrom)

Parameters:
   pMe:      Pointer to the IGSMSMS object
   index:    Slot index of message to retrieve
   pMsg:     Pointer to struct to be filled in with the retrieved message
   readFrom: Indicates whether to delete from the SIM or from NVRAM database.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If pMsg is NULL, or readFrom is invalid: AEE_GSMSMS_EBADPARAM 
   If the messages could not be read: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_GetMessage(struct _IGSMSMS * pMe, uint16 index, GSMSMSRawMsg *pMsg, GSMSMSStorageType readFrom)
{
   FARF(UNITTEST, ("Called OEMGSMSMS_GetMessage"));

   if (pMsg == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   pMsg->format = GSMSMS_RAW_FORMAT_SIM;

   switch (readFrom)
   {
   case GSMSMS_STORE_SIM:
       if (!pMe->m_bSIMSMSEnabled)
       {
          return AEE_GSMSMS_ESST;
       }

       if (OEMGSM1xCardHandler_GetSms(index, 
           (CardHandlerRawSMSType *)&pMsg->msg.sim.rawMsg[0]) == UIM_FAIL)
       {
          return AEE_GSMSMS_EFAILED;
       }
       break;
   case GSMSMS_STORE_NVRAM:
   case GSMSMS_STORE_NVRAM_VM:
       {
           IFile *pIFile = NULL;

           if ((pIFile = GSMSMS_OpenDB(pMe, readFrom)) == NULL) 
           {
              FARF(ERROR, ("Database Open failed"));
              return AEE_GSMSMS_EFAILED;
           }

           if (index >= GSMSMS_NUM_NV_ENTRIES)
           {
              return AEE_GSMSMS_EBADPARAM;
           }

           IFILE_Seek(pIFile, _SEEK_START, index*GSMSMS_NV_MBOX_RECORD_LEN+1);
           if (IFILE_Read(pIFile, &pMsg->msg.sim.rawMsg[0], 
                  sizeof(pMsg->msg.sim.rawMsg)) != sizeof(pMsg->msg.sim.rawMsg))
           {
              FARF(ERROR, ("Read from MBOX failed"));
              return AEE_GSMSMS_EFAILED;
           }
       }
       break;

   default:
      return AEE_GSMSMS_EBADPARAM;
   }

   GSMSMS_DecodeMessageStatus(pMsg->msg.sim.rawMsg[0], &pMsg->msg.sim.status);
   return AEE_GSMSMS_SUCCESS;
}

/*===========================================================================
Function: OEMGSMSMS_GetStatusReport()

Description:

   This function retrieves an SMS StatusReport entry that corresponds to a
   SMS_Submit message in a specified slot on the store (SIM or NVRAM).

Prototype:
   int OEMGSMSMS_GetStatusReport(struct _IGSMSMS * pMe, uint16 index, 
                                 GSMSIMMsg *pMsg, 
                                 GSMSMSStorageType readFrom)

Parameters:
   pMe:      Pointer to the IGSMSMS object
   index:    Slot index of message to retrieve
   pMsg:     Pointer to the GSM SMS Msg to write the status report to
   readFrom: Indicates whether to delete from the SIM or from NVRAM database.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If readFrom is invalid: AEE_GSMSMS_EBADPARAM 
   If the messages could not be read: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_GetStatusReport
(
   struct _IGSMSMS * pMe, 
   uint16 index, 
   GSMSMSRawMsg *pMsg,
   GSMSMSStorageType readFrom
)
{
   FARF(UNITTEST, ("Called OEMGSMSMS_GetStatusReport"));

   // Not implemented
   return AEE_GSMSMS_EFAILED;
}

/*===========================================================================
Function: OEMGSMSMS_SetMessageStatus()

Description:

   This function marks a MT message as read or unread in the specified store.

Prototype:
   int OEMGSMSMS_SetMessageStatus(struct _IGSMSMS * pMe, uint16 index, GSMSMSStatusType status, GSMSMSStorageType writeTo)

Parameters:
   pMe:      Pointer to the IGSMSMS object
   index:    Slot index of message to mark
   status:   The status to write
   writeTo:  Indicates whether to use the SIM or NVRAM database

Return Value:
  If successful: AEE_GSMSMS_SUCCESS 
  If the entry could not be updated: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_SetMessageStatus
(
   struct _IGSMSMS * pMe, 
   uint16            index, 
   GSMSMSStatusType  status, 
   GSMSMSStorageType writeTo
)
{

   FARF(UNITTEST, ("Called OEMGSMSMS_SetSMSStatus"));

   switch (writeTo)
   {
   case GSMSMS_STORE_SIM:
    {
       GSMSMSRawMsg msg;

       if (!pMe->m_bSIMSMSEnabled)
       {
          return AEE_GSMSMS_ESST;
       }

       // Get the old message from the SIM
       if (OEMGSM1xCardHandler_GetSms(index, 
           (CardHandlerRawSMSType *)&msg.msg.sim.rawMsg[0]) == UIM_FAIL)
       {
          return AEE_GSMSMS_EFAILED;
       }

       if (GSMSMS_ValidateStatusUpdate(msg.msg.sim.rawMsg[0], status) != 
              AEE_GSMSMS_SUCCESS)
       {
          return AEE_GSMSMS_EFAILED;
       }
       msg.msg.sim.rawMsg[0] = status;

       // Store the message on the SIM
       if (OEMGSM1xCardHandler_SetSms(index, 
           (CardHandlerRawSMSType *)&msg.msg.sim.rawMsg[0]) == UIM_FAIL)
       {
          FARF(ERROR, ("OEMGSM1xCardHandler_SetSms failed"));
          return AEE_GSMSMS_EFAILED;
       }
       break;
     }

   case GSMSMS_STORE_NVRAM:
   case GSMSMS_STORE_NVRAM_VM:
       {
          GSMSMSStatusType oldStatus;
          int rv;
          IFile *pIFile = NULL;

          if (index >= GSMSMS_NUM_NV_ENTRIES)
          {
             return AEE_GSMSMS_EBADPARAM;
          }

          rv = OEMGSMSMS_GetMessageStatus(pMe, index, &oldStatus, writeTo);
          if (rv != AEE_GSMSMS_SUCCESS)
          {
             return rv;
          }

          if (GSMSMS_ValidateStatusUpdate(oldStatus, status) != 
                 AEE_GSMSMS_SUCCESS)
          {
             return AEE_GSMSMS_EFAILED;
          }

          if ((pIFile = GSMSMS_OpenDB(pMe, writeTo)) == NULL) 
          {
             FARF(ERROR, ("Database Open failed"));
             return AEE_GSMSMS_EFAILED;
          }

          IFILE_Seek(pIFile, _SEEK_START, index*GSMSMS_NV_MBOX_RECORD_LEN+1);
          if (IFILE_Write(pIFile, &status, 1) != 1)
          {
             FARF(ERROR, ("Write to MBOX failed"));
             return AEE_GSMSMS_EFAILED;
          }
       }
       break;

   default:
      return AEE_GSMSMS_EBADPARAM;
   }

   return AEE_GSMSMS_SUCCESS;
}

/*===========================================================================
Function: GSMSMS_ValidateStatusUpdate()

Description:

   This function validates the status transition is valid

Prototype:
   int GSMSMS_ValidateStatusUpdate(GSMSMSStatusType oldStatus, GSMSMSStatusType status)

Parameters:
   oldStatus: the current status
   status:    the new status

Return Value:
  If successful: AEE_GSMSMS_SUCCESS 
  Otherwise: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int GSMSMS_ValidateStatusUpdate(GSMSMSStatusType oldStatus, GSMSMSStatusType status)
{
   switch (status)
   {
   case GSMSMS_STATUS_MT_READ:
   case GSMSMS_STATUS_MT_NOT_READ:
      if ((oldStatus & 0x04) == 0x04)
      {
         FARF(HIGH, ("MO status for MT entry"));
         return AEE_GSMSMS_EFAILED;
      }
      break;

   case GSMSMS_STATUS_MO_SENT:
   case GSMSMS_STATUS_MO_NOT_SENT:
   case GSMSMS_STATUS_MO_SENT_ST_NOT_RECEIVED:
   case GSMSMS_STATUS_MO_SENT_ST_NOT_STORED:
   case GSMSMS_STATUS_MO_SENT_ST_STORED:
      // TODO Should we parse the message to see if the SRR bit is set?
      if ((oldStatus & 0x04) != 0x04)
      {
         FARF(HIGH, ("MT status for MO entry"));
         return AEE_GSMSMS_EFAILED;
      }
      break;

   case GSMSMS_STATUS_NONE:
   default:
      return AEE_GSMSMS_EFAILED;
   }
   return AEE_GSMSMS_SUCCESS;
}

/*===========================================================================
Function: OEMGSMSMS_GetSCAddress()

Description:

   This function retrieves the default GSM Service Center address from the 
   SIM.

Prototype:
   int OEMGSMSMS_GetSCAddress(IGSMSMS * pMe, GSMSMSAddress *pAddress)

Parameters:
   pMe:      Pointer to the IGSMSMS object
   pAddress: Pointer to the structure to fill in with the default SC address

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the SC address could not be retrieved: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_GetSCAddress
(
   struct _IGSMSMS * pMe, GSMSMSAddress *pAddress
)
{
    CardHandlerAddressType scAddr;
    uint16 digit, idx, i, len;
    uint32 result;
    byte tmp;

    FARF(UNITTEST, ("Called OEMGSMSMS_GetSCAddress"));

    if (pAddress == NULL)
    {
       return AEE_GSMSMS_EBADPARAM;
    }

    // use default SC address
    result = OEMGSM1xCardHandler_ReadSMSCAddress(0, &scAddr);

    if (OEMGSM1xCardHandler_ReadSMSCAddress(0, &scAddr) == UIM_FAIL)
    {
       FARF(ERROR, ("ReadSMSCAddress failed"));
       return AEE_GSMSMS_EFAILED;
    }

    if (OEMGSM1xCardHandler_ReadSMSCAddress(0, &scAddr) == GSM1X_UIM_EMPTY)
    {
       FARF(ERROR, ("ReadSMSCAddress empty"));
       GetHardCodedDefaultSMSCAddr(&scAddr);
    } 

    pAddress->npi = (GSMSMSNpiType)(scAddr.numbering_plan);
    pAddress->ton = (GSMSMSTonType)(scAddr.nature_of_address & 0x7F);

    idx = 0;

    // convert bytes to digits
    len=(scAddr.length-1)*2;

    // convert to ASCII
    for (i=0, digit=0; i < len; i+=2)
    {
        pAddress->addr[digit++] = (scAddr.digits[idx] & 0x0F) +'0';
        tmp = (scAddr.digits[idx++] >> 4);
        if (((i+1) < len) && tmp <= 0x09)
        {
           pAddress->addr[digit++] = tmp +'0';
        }
    }
    pAddress->addr[digit] = '\0'; // NULL terminate
   
    return AEE_GSMSMS_SUCCESS;
} 

/*===========================================================================
Function: OEMGSMSMS_SetSCAddress()

Description:

   This function sets the default SC address in the SIM.

Prototype:
   void OEMGSMSMS_SetSCAddress(struct _IGSMSMS * pMe, const GSMSMSAddress *pAddress) 

Parameters:
   pMe:      Pointer to the IGSMSMS object
   pAddress: Pointer to the structure to containing the new SC address

Return Value:
  If successful: AEE_GSMSMS_SUCCESS 
  If the SC address could not be retrieved: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_SetSCAddress
(
   struct _IGSMSMS * pMe, 
   GSMSMSAddress const *pAddress
)
{
    CardHandlerAddressType scAddr;
    uint8 digit, idx;

    FARF(UNITTEST, ("Called OEMGSMSMS_SetSCAddress"));

    if (pAddress == NULL)
    {
       return AEE_GSMSMS_EBADPARAM;
    }

    scAddr.numbering_plan    = pAddress->npi;
    scAddr.nature_of_address =(byte)pAddress->ton;
	scAddr.ext               = 1; 

    MEMSET(&scAddr.digits[0], 0xff, sizeof(scAddr.digits));

    // convert ASCII to reverse BCD
    for(digit = 0, idx=0; pAddress->addr[digit] != '\0';)
    {
        scAddr.digits[idx] = (byte)pAddress->addr[digit++] - (byte)'0';
        if (pAddress->addr[digit] != '\0')
        {
           scAddr.digits[idx] |= (byte)(pAddress->addr[digit++] - '0') << 4;
        }
        else
        {
           scAddr.digits[idx] |= (byte)0xF0;
        }
        idx++;
    } 

    scAddr.length = idx+1; 

    // set default SC address
    if (OEMGSM1xCardHandler_SetSMSCAddress(0, &scAddr) != UIM_PASS)
    {
       FARF(ERROR, ("SetSMSCAddress failed"));
       return AEE_GSMSMS_EFAILED;
    }

    return AEE_GSMSMS_SUCCESS;
}

/*===========================================================================
Function: OEMGSMSMS_GetStoreSize()

Description:

   This function returns the size of the message store in slots.

Prototype:
   int OEMGSMSMS_GetStoreSize(struct _IGSMSMS * pMe, GSMSMSStorageType readFrom, 
                                uint16 *pCount)

Parameters:
   po:        Pointer to the IGSMSMS object
   readFrom:  Message store to query
   pCount:    Pointer to the variable to set to the number of slots in the store

Return Value:
  If successful: AEE_GSMSMS_SUCCESS 
  Otherwise: AEE_GSMSMS_EBADPARAM
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_GetStoreSize
(
   struct _IGSMSMS * pMe, 
   GSMSMSStorageType readFrom, 
   uint16 *pCount
)
{
   
   FARF(UNITTEST, ("Called OEMGSMSMS_GetStoreSize"));

   if (pCount == NULL || pMe == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   switch (readFrom)
   {
   case GSMSMS_STORE_SIM:
      {
         if (!pMe->m_bSIMSMSEnabled)
         {
            return AEE_GSMSMS_ESST;
         }

         *pCount = OEMGSM1xCardHandler_GetNumSMSSlots();
         break;
      }

   case GSMSMS_STORE_NVRAM:

      *pCount = GSMSMS_NUM_NV_ENTRIES;
      break;

   case GSMSMS_STORE_NVRAM_VM:

      *pCount = GSMSMS_NUM_NV_VM_ENTRIES;
      break;

   default:
      return AEE_GSMSMS_EBADPARAM;
   }

   return AEE_GSMSMS_SUCCESS;
}

/*===========================================================================
Function: OEMGSMSMS_GetMessageStatus()

Description:

   This function reads the status of a message in the specified store.
   The status will indicate whether the specified slotis free or if the
   message stored there is mobile terminated or mobile originated.

   If the message is mobile terminated it will also indicate whether the
   message has been read or not.

   If the message is mobile originated, it will also indicate whether the
   message has been sent or not and if sent, whether there is a pending
   or received status report for the message.
   This function is used to get a list of message indexes on the store for
   a particular message type.

Prototype:
   void OEMGSMSMS_GetMessageStatus(struct _IGSMSMS * pMe, uint16 index, 
                               GSMSMSStatusType *pStatus, 
                               GSMSMSStorageType readFrom)

Parameters:
   pMe:      Pointer to the IGSMSMS object
   index:    The index in the store to read from
   pStatus:  Pointer to the status to read
   readFrom: Indicates whether to use the SIM or NVRAM database.

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If readFrom is invalid: AEE_GSMSMS_EBADPARAM 
   If the store could not be read: AEE_GSMSMS_EFAILED
   If SIM handler is not done initializing yet: AEE_GSMSMS_EBUSY
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_GetMessageStatus
(
   struct _IGSMSMS * pMe, 
   uint16 index, 
   GSMSMSStatusType *pStatus, 
   GSMSMSStorageType readFrom
)
{
   FARF(UNITTEST, ("Called OEMGSMSMS_GetMessageStatus"));

   if (pStatus == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   switch (readFrom)
   {
   case GSMSMS_STORE_SIM:
      {
         uint8 status;

         if (!pMe->m_bSIMSMSEnabled)
         {
            return AEE_GSMSMS_ESST;
         }

         if (OEMGSM1xCardHandler_IsSMSInit() == FALSE)
         {
            return AEE_GSMSMS_EBUSY;
         }

         if (OEMGSM1xCardHandler_GetSMSStatus((uint8)index, &status) != UIM_PASS)
         {
            return AEE_GSMSMS_EBADPARAM;
         }

         GSMSMS_DecodeMessageStatus(status, pStatus);
         return AEE_GSMSMS_SUCCESS;
      }

   case GSMSMS_STORE_NVRAM:
   case GSMSMS_STORE_NVRAM_VM:
      {
         IFile * pIFile = NULL;
         GSMSMSStatusType status;

         if (index >= GSMSMS_NUM_NV_ENTRIES)
         {
            return AEE_GSMSMS_EBADPARAM;
         }

         if ((pIFile = GSMSMS_OpenDB(pMe, readFrom)) == NULL) 
         {
            FARF(ERROR, ("Database Open failed"));
            return AEE_GSMSMS_EFAILED;
         }
         IFILE_Seek(pIFile, _SEEK_START, index*GSMSMS_NV_MBOX_RECORD_LEN+1);
         if (IFILE_Read(pIFile, &status, 1) != 1)
         {
            FARF(ERROR, ("Read from MBOX failed"));
            return AEE_GSMSMS_EFAILED;
         }
         GSMSMS_DecodeMessageStatus(status, pStatus);
 
         return AEE_GSMSMS_SUCCESS;
      }

   default:
      break;
   }

   return AEE_GSMSMS_EBADPARAM;
}

/*=============================================================================
FUNCTION: OEMGSMSMS_DecodeUserData

DESCRIPTION:

   This function is used decode the UserData into UNICODE for display.
   If the UserData contains a user data header it will be skipped. 

   ONLY THE RAW TEXT IS RETURNED by this method.

   If the message is a concatinated or EMS message, it must be
   decoded directly by an external library or by the applet.

   See the description of IGSMSMS_DecodeMessage for an example of how
   this is used.

   If wstrlen is less than the decoded data length, the decoded data will be
   truncated and NULL terminated.

PROTOTYPE:
   OEMGSMSMS_DecodeUserData(struct _IGSMSMS *pMe, const GSMSMSMsg *pMsg, 
                            AECHAR *pwzStr, byte wstrlen)

PARAMETERS:
   pMe:     Pointer to the IGSMSMS object
   pMsg:    Pointer to the decoded SMS message
   pwzStr:    Pointer to the unicode string to fill with the decoded data
   wstrlen: Size in bytes of the wstr unicode string

RETURN VALUE:
  If successful: AEE_GSMSMS_SUCCESS 
  If the data could not be decoded: AEE_GSMSMS_EFAILED
  If pMsg or pwzStr are NULL: AEE_GSMSMS_EINVALID

SIDE EFFECTS:
    This routine does not handle user data with a user data header
=============================================================================*/
static int OEMGSMSMS_DecodeUserData(struct _IGSMSMS *pMe, const GSMSMSMsg *pMsg, AECHAR *pwzStr, uint16 wstrlen)
{
   uint16 i, msglen;
   byte tmp;
   byte sz8bit[GSMSMS_MAX_SMS_DELIVER_REPORT_UD_LEN*8/7]; // septets
   GSMSMSDCS const *dcs;
   byte const *UD;
   byte UDL;
   byte escChars;
   boolean UDHI; 

   FARF(UNITTEST, ("Called OEMGSMSMS_DecodeUserData"));

   if (pMsg == NULL || pwzStr == NULL || pMe == NULL || wstrlen == 0)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   // TODO - handle user data header
   // The stucture should split out the user data header and reduce the UDL appropriately

   switch (pMsg->msgType)
   {
   case GSMSMS_MSG_SMS_DELIVER:
      UDHI = pMsg->msg.SMSDeliver.UDHI;
      UD = pMsg->msg.SMSDeliver.UD;
      UDL = pMsg->msg.SMSDeliver.UDL;
      dcs = &pMsg->msg.SMSDeliver.DCS;
      break;

   case GSMSMS_MSG_SMS_SUBMIT:
      UDHI = pMsg->msg.SMSSubmit.UDHI;
      UD = pMsg->msg.SMSSubmit.UD;
      UDL = pMsg->msg.SMSSubmit.UDL;
      dcs = &pMsg->msg.SMSSubmit.DCS;
      break;

   case GSMSMS_MSG_SMS_DELIVER_REPORT:
      UDHI = pMsg->msg.SMSDeliverReport.UDHI;
      UD = pMsg->msg.SMSDeliverReport.UD;
      UDL = pMsg->msg.SMSDeliverReport.UDL;
      dcs = &pMsg->msg.SMSDeliverReport.DCS;
      break;

   case GSMSMS_MSG_SMS_STATUS_REPORT:
      UDHI = pMsg->msg.SMSStatusReport.UDHI;
      UD = pMsg->msg.SMSStatusReport.UD;
      UDL = pMsg->msg.SMSStatusReport.UDL;
      dcs = &pMsg->msg.SMSStatusReport.DCS;
      break;

   case GSMSMS_MSG_SMS_SUBMIT_REPORT:
      UDHI = pMsg->msg.SMSSubmitReport.UDHI;
      UD = pMsg->msg.SMSSubmitReport.UD;
      UDL = pMsg->msg.SMSSubmitReport.UDL;
      dcs = &pMsg->msg.SMSSubmitReport.DCS;
      break;

   //case GSMMsgSMSCommmand: // not decodeable
   default:
      return AEE_GSMSMS_EBADPARAM;
   }

   MEMSET(pwzStr, 0, wstrlen);

   if (UDHI)
   {
      if (UDL < UD[0])
      {
         return AEE_GSMSMS_EBADPARAM;
      }
      UDL -= (UD[0]+1); // reduce user data length by user data header length
      UD += (UD[0]+1);  // Skip the user data header
   }

   if (UDL == 0)
   {
      *pwzStr = 0x0000;
      return AEE_GSMSMS_SUCCESS;
   }

   // Check for GSM 7bit 
   if (dcs->msgCoding == GSMSMS_ENCODING_7BIT) 
   {
       byte idx, idx2;
       msglen = UDL; // number of septets
       escChars = 0;
       sz8bit[0] = 255; // Flag string as empty
       for (i=0; i<msglen+escChars; i++)
       {
          idx = i*7/8;
          idx2 = (i*7)%8;
          if (idx2 > 0)
          {
             tmp = ((UD[idx] | (UD[idx+1] << 8)) & (0x007F << idx2)) >> idx2;
          }
          else
          {
             tmp = UD[idx] & 0x007F;
          }
          sz8bit[i] = tmp;
          if (tmp == 0x1b)
          {
             escChars++;
          }
       }
       GSMSMS_GSMToWSTR(sz8bit, i, pwzStr, (uint32)wstrlen/sizeof(AECHAR));
   }
   // Check for UCS2  
   else if (dcs->msgCoding == GSMSMS_ENCODING_UCS2)
   {
       GSMSMS_GSMToWSTR(UD, UDL, pwzStr, (uint32)wstrlen);
   }
   // check for 8-bit  
   else if (dcs->msgCoding == GSMSMS_ENCODING_8BIT)
   {
      FARF(HIGH, ("ERROR - DCS 8bit encoding not defined"));
      return AEE_GSMSMS_EBADPARAM;
   }
   else
   {
      FARF(HIGH, ("ERROR - Unsupported encoding"));
      return AEE_GSMSMS_EBADPARAM;
   }
   return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMGSMSMS_EncodeUserData

DESCRIPTION:

   This function is used decode the UserData into UNICODE for display.

   If a UserData Header is included then address of the offset user data
   should be passed to this function. The udlen parameter must also be
   reduced accordingly.

   If the message will not fit in the space provided it will be truncated.

Prototype:
   OEMGSMSMS_EncodeUserData(struct _IGSMSMS *pMe, AECHAR *pwszText, byte *pUD, byte udlen, 
                          GSMEncodingType encoding, byte *encodedLen);

PARAMETERS:
   pMe:        Pointer to the IGSMSMS object
   pwszText:   Pointer to the unicode string to encode
   pDest:      Pointer in the UserData to contain the encoded data
   destlen:    Size in bytes of the UserData left for the encoded string 
   encoding:   Encoding to use for text
   encodedLen: Length of the encoded text in bytes

RETURN VALUE:
  If successful: AEE_GSMSMS_SUCCESS 
  If the encoded data could not fit udlen characters: AEE_GSMSMS_EBUFFER
  If pDest is NULL, pwszText is NULL or encoding is invalid: AEE_GSMSMS_EINVALID

SIDE EFFECTS:
  None
=============================================================================*/
static int OEMGSMSMS_EncodeUserData
(
   struct _IGSMSMS *pMe, 
   const AECHAR *pwszText, 
   byte *pDest, 
   byte destlen, 
   GSMSMSEncodingType encoding,
   byte *pEncodedLen
)
{
   uint16 i, k, bitindex, bit, index;
   uint8 len, destidx, newlen=0;
   byte tmp;
   char sz8bit[GSMSMS_MAX_SMS_SUBMIT_REPORT_UD_LEN];

   FARF(UNITTEST, ("Called OEMGSMSMS_EncodeUserData"));

   if (pDest == NULL || pEncodedLen == NULL || pwszText == NULL || destlen > sizeof(sz8bit))
   {
      FARF(ERROR, ("Invalid parameter"));
      return AEE_GSMSMS_EBADPARAM;
   }

   switch (encoding)
   { 
   case GSMSMS_ENCODING_7BIT:
   case GSMSMS_ENCODING_UCS2:
      break;
   default:
      FARF(ERROR, ("Invalid encoding"));
      return AEE_GSMSMS_EBADPARAM;
   }

   // clear the user data
   MEMSET((void *)pDest,0xff,destlen);

   switch (encoding)
   {
   case GSMSMS_ENCODING_UCS2:
      len = (byte)WSTRLEN(pwszText);
      if (len > destlen)
         len = destlen;

      MEMCPY((void *)pDest, pwszText, len*sizeof(AECHAR));
      *pEncodedLen = (byte)(len*sizeof(AECHAR));
      break;

   case GSMSMS_ENCODING_7BIT:
      // convert to 8-bit string in 7 bit GSM encoding
      len = GSMSMS_WSTRTo7BitGSM(pwszText, (byte *)sz8bit, (byte)sizeof(sz8bit)); 

      // convert to 7bit
      bitindex=0;
      tmp=0;
      for (i=0; i<len; i++)
      {
         for (k=0; k< 7; k++)
         {
            index = bitindex % 8;
            destidx = (uint8)(bitindex / 8);
            bit = sz8bit[bitindex/7] & (1 << (bitindex % 7));
            if (bit > 0)
            {
               tmp += 1 << index;
            }

            if (index == 7)
            {
               pDest[destidx] = tmp;
               tmp=0;
            }

            bitindex++;
         }
      }
      index = bitindex % 8;
      destidx = (uint8)(bitindex / 8);

      if (index > 0)
      {
         pDest[destidx] = tmp;
         newlen=destidx+1;
      }
      else
      {
         newlen=destidx;
      }
      *pEncodedLen = newlen;
      break;

   default:
      // not supported
      return (AEE_GSMSMS_EBADPARAM);
   }
   return AEE_GSMSMS_SUCCESS;
} 

/*=============================================================================
FUNCTION: OEMGSMSMS_DecodeMessage

DESCRIPTION:
   This function is used decode a raw SMS message into an appropriate 
   structure representation.

   The UserData (UD) will not be decoded and can be subsequently:

        AECHAR wszText[GSMSMS_MAX_UD_CHAR];
        GSMSMSMsg DecodedMsg;

        // Decode Message
        IGSMSMS_DecodeMessage(pMe, pNotifyMsg->rawMsg, sizeof(pNotifyMsg->rawMsg), &DecodedMsg);

        // Decode UserData
        IGSMSMS_DecodeUserData(pMe, DecodedMsg, &wszText[0], sizeof(wszText));
        
PROTOTYPE:
   int IGSMSMS_DecodeMessage(IGSMSMS * po, const GSMSMSRawMsg *pRawMsg, const GSMSMSMsg *pMsg)

PARAMETERS:
   po:      Pointer to the IGSMSMS object
   pRawMsg: Pointer to the raw GSM SMS message to decode
   pMsg:    Pointer to the GSMSMSMsg struct to fill with the decoded information

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the store could not be decoded: AEE_GSMSMS_EFAILED
   If pRawMsg or pMsg are NULL: AEE_GSMSMS_EBADPARAM
   
COMMENTS:  
   None

SIDE EFFECTS: 
   None
=============================================================================*/
static int OEMGSMSMS_DecodeMessage(struct _IGSMSMS *pMe, GSMSMSRawMsg const *pRaw, GSMSMSMsg *pMsg)
{
   byte len, idx=0;
   GSMSMSAddress addr;
   int rv;

   FARF(UNITTEST, ("Called OEMGSMSMS_DecodeMessage"));

   if (pRaw == NULL || pMsg == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   if (pRaw->format == GSMSMS_RAW_FORMAT_GSM)
   {
      switch (pRaw->msg.gsm.data[0])
      {
      case GSMSMS_RP_MT_DATA:
         // skip RP_MR
         
         idx = 2;
         if (pRaw->msg.gsm.data[idx] > GSMSMS_MAX_SC_ADDRESS_LEN)
         {
            return AEE_GSMSMS_EFAILED;
         }
         len = GSMSMS_DecodeAddress(&pRaw->msg.gsm.data[idx], &addr, TRUE);
         idx+=len; // skip RP-OA (SC-Address)

         // Verify RP-DA length is zero
         if (pRaw->msg.gsm.data[idx] != 0) 
         {
            return AEE_GSMSMS_EFAILED;
         }

         idx++;     // skip RP-DA len
         idx++;     // skip USER-DATA len
         
         if ((pRaw->msg.gsm.data[idx] & 0x03) == GSMSMS_TL_SMS_DELIVER)
         {
             FARF(LOW, ("Received SMS_DELIVER"));
             pMsg->msgType = GSMSMS_MSG_SMS_DELIVER;
             rv = GSMSMS_DecodeSMSDeliverMsg(&pRaw->msg.gsm.data[idx], &pMsg->msg.SMSDeliver);

             if (rv != AEE_GSMSMS_SUCCESS)
             {
                return rv;
             }

             // fill in the SC address
             pMsg->msg.SMSDeliver.SCAddr = addr;
         }
         else  if ((pRaw->msg.gsm.data[idx] & 0x03) == GSMSMS_TL_SMS_STATUS_REPORT)
         {
             FARF(LOW, ("Received SMS_STATUS_REPORT"));
             pMsg->msgType = GSMSMS_MSG_SMS_STATUS_REPORT;
             rv = GSMSMS_DecodeSMSStatusReport(&pRaw->msg.gsm.data[idx], &pMsg->msg.SMSStatusReport);

             if (rv != AEE_GSMSMS_SUCCESS)
             {
                return rv;
             }

         }
         else
         {
            FARF(HIGH, ("No valid TP_MTI found"));
            return AEE_GSMSMS_EFAILED;
         }
         break;

      case GSMSMS_RP_MT_ACK:
      case GSMSMS_RP_MT_ERROR:

         if (GSMSMS_DecodeSMSSubmitReportMsg(&pRaw->msg.gsm, 
                &pMsg->msg.SMSSubmitReport) != AEE_GSMSMS_SUCCESS)
         {
            FARF(ERROR, ("GSMSMS_DecodeSMSSubmitReportMsg failed"));
            return EFAILED;
         }
         pMsg->msgType = GSMSMS_MSG_SMS_SUBMIT_REPORT;
         break;

      default:
         FARF(ERROR, ("No valid RP MTI found"));
         return AEE_GSMSMS_EFAILED;
      }
      return AEE_GSMSMS_SUCCESS; 
   }
   else if (pRaw->format == GSMSMS_RAW_FORMAT_SIM)
   {
      if (pRaw->msg.sim.status == GSMSMS_STATUS_MT_READ ||
          pRaw->msg.sim.status == GSMSMS_STATUS_MT_NOT_READ)
      {
         pMsg->msgType = GSMSMS_MSG_SMS_DELIVER;

         // fill in the SC address
         if (pRaw->msg.sim.rawMsg[1] > GSMSMS_MAX_SC_ADDRESS_LEN)
         {
            return AEE_GSMSMS_EFAILED;
         }
         len = GSMSMS_DecodeAddress(&pRaw->msg.sim.rawMsg[1], &pMsg->msg.SMSDeliver.SCAddr, TRUE);
         rv = GSMSMS_DecodeSMSDeliverMsg(&pRaw->msg.sim.rawMsg[len+1], &pMsg->msg.SMSDeliver);

         if (rv != AEE_GSMSMS_SUCCESS)
         {
            return rv;
         }

      }
      else if (pRaw->msg.sim.status & 0x05) // Is this an MO message
      {
         pMsg->msgType = GSMSMS_MSG_SMS_SUBMIT;
         // fill in the SC address
         if (pRaw->msg.sim.rawMsg[1] > GSMSMS_MAX_SC_ADDRESS_LEN)
         {
            return AEE_GSMSMS_EFAILED;
         }
         len = GSMSMS_DecodeAddress(&pRaw->msg.sim.rawMsg[1], &pMsg->msg.SMSSubmit.SCAddr, TRUE);
         rv = GSMSMS_DecodeSMSSubmitMsg(&pRaw->msg.sim.rawMsg[len+1], &pMsg->msg.SMSSubmit);

         if (rv != AEE_GSMSMS_SUCCESS)
         {
            return rv;
         }

      }
      return AEE_GSMSMS_SUCCESS; 
   }
   return AEE_GSMSMS_EFAILED; 
}

/*===========================================================================
Function: OEMGSMSMS_CreateDefaultMessage()

Description:

   This function initializes a Mobile Originated (MO) message structure to
   the default valuses that are most commonly used.

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

   The defaults for a GSMSMSCommandType are:
      SCAddr - Filled in with Default SC Address from SIM
      SRR    - Status Report Request set to True;
      UDHI   - User Data Header Indicator set to False
      MR     - Message Reference is 0 and is filled in when sent
      PID    - Protocol ID is SME to SME 
      CT     - Command Type is set to 0 
      MN     - Message Number is set to 0 
      CDL    - Command Data Length is set to 0
      CD     - Command Data is initialized to all zeros

Prototype:
   int OEMGSMSMS_CreateDefaultMessage(struct _IGSMSMS *pMe, GSMSMSMsgType type, 
                                      GSMSMSMsg * pMsg)

Parameters:
   pMe:     Pointer to the IGSMSMS object
   type:    The type of message to create
   pGsmMsg: Pointer to the GSM SMS Msg structure to initialize

Return Value:
  If successful: AEE_GSMSMS_SUCCESS. 
  If the message was not valid: AEE_GSMSMS_EFAILED
  If the default SC address could not be read: AEE_GSMSMS_ENODEFAULTSCADDR
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_CreateDefaultMessage
(
   struct _IGSMSMS *pMe, 
   GSMSMSMsgType type, 
   GSMSMSMsg * pMsg
)
{
   CardHandlerAddressType scAddr;
   byte i,tmp,idx;

   FARF(UNITTEST, ("Called OEMGSMSMS_CreateDefaultMessage"));

   if (pMsg == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   pMsg->msgType = type;

   switch (pMsg->msgType)
   {
   case GSMSMS_MSG_SMS_SUBMIT:
      pMsg->msg.SMSSubmit.RD = TRUE;
      pMsg->msg.SMSSubmit.RP = FALSE;
      pMsg->msg.SMSSubmit.VP.format = GSMSMS_VP_FORMAT_NOT_PRESENT;
      pMsg->msg.SMSSubmit.SRR = FALSE;
      pMsg->msg.SMSSubmit.UDHI = FALSE;
      pMsg->msg.SMSSubmit.MR = 0; 
      pMsg->msg.SMSSubmit.PID = 0; // SME to SME

      // DCS = 0
      pMsg->msg.SMSSubmit.DCS.codingGroup = GSMSMS_DCS_CG_GENERAL_DATA;
      pMsg->msg.SMSSubmit.DCS.msgCoding = GSMSMS_ENCODING_7BIT;
      pMsg->msg.SMSSubmit.DCS.cg.generalData.msgClass = GSMSMS_MSG_CLASS_NONE;
      pMsg->msg.SMSSubmit.DCS.cg.generalData.isCompressed = FALSE;
      pMsg->msg.SMSSubmit.DCS.cg.generalData.autoDelete = FALSE;
    
      // DA must be set by Application
      pMsg->msg.SMSSubmit.DA.addr[0] = '\0';
      pMsg->msg.SMSSubmit.DA.ton = GSMSMS_TON_NATIONAL_NUMBER;
      pMsg->msg.SMSSubmit.DA.npi = GSMSMS_NPI_ISDN_TELEPHONE;

      pMsg->msg.SMSSubmit.UDL = 0;

      if (OEMGSM1xCardHandler_ReadSMSCAddress(0, &scAddr) != UIM_PASS)
      {
         GetHardCodedDefaultSMSCAddr(&scAddr);
      }

      // Convert SC Address to GSMSMS format
      for (i=0,idx=0; i<scAddr.length-1;i++)
      {
         pMsg->msg.SMSSubmit.SCAddr.addr[idx++] = '0'+(scAddr.digits[i] & 0x0f);
         tmp = (scAddr.digits[i] >> 4) & 0x0f;
         if (tmp < 0x0a)
         {
            pMsg->msg.SMSSubmit.SCAddr.addr[idx++] = '0'+tmp;
         }
      }
      pMsg->msg.SMSSubmit.SCAddr.addr[idx]='\0';

      pMsg->msg.SMSSubmit.SCAddr.ton = (GSMSMSTonType)(scAddr.nature_of_address & 0x07);
      pMsg->msg.SMSSubmit.SCAddr.npi = (GSMSMSNpiType)scAddr.numbering_plan;

      break;

   case GSMSMS_MSG_SMS_DELIVER_REPORT:
      pMsg->msg.SMSDeliverReport.UDHI = FALSE;
      pMsg->msg.SMSDeliverReport.FCS_present = FALSE;
      pMsg->msg.SMSDeliverReport.FCS = 0;

      pMsg->msg.SMSDeliverReport.PID_present = FALSE;
      pMsg->msg.SMSDeliverReport.PID = 0;

      pMsg->msg.SMSDeliverReport.DCS_present = FALSE;
      pMsg->msg.SMSDeliverReport.DCS.codingGroup = GSMSMS_DCS_CG_GENERAL_DATA;
      pMsg->msg.SMSDeliverReport.DCS.msgCoding = GSMSMS_ENCODING_7BIT;
      pMsg->msg.SMSDeliverReport.DCS.cg.generalData.msgClass = GSMSMS_MSG_CLASS_NONE;
      pMsg->msg.SMSDeliverReport.DCS.cg.generalData.isCompressed = FALSE;
      pMsg->msg.SMSDeliverReport.DCS.cg.generalData.autoDelete = FALSE;
      pMsg->msg.SMSDeliverReport.UDL_present = FALSE;
      pMsg->msg.SMSDeliverReport.UDL = 0;
      MEMSET((void *)&pMsg->msg.SMSDeliverReport.UD[0], 0, sizeof(pMsg->msg.SMSDeliverReport.UD));
      break;

   default:
      return AEE_GSMSMS_EBADPARAM;
   }

   return AEE_GSMSMS_SUCCESS;
}

/*===========================================================================
Function: OEMGSMSMS_SendMoreMemoryAvailable()

Description:

   This function allows an application to send a GSM RP Layer SMMA message. 
   This function is called in accordance with the GSM 23.040 spec.

   If the message times out, pReport->sendResult is set to 
   AEE_GSMSMS_ETIMEDOUT. All other pReport fields are invalid.

   If the message is successfully sent, pReport->sendResult is set to 
   AEE_GSMSMS_SUCCESS.

Prototype:
   int OEMGSMSMS_MoreMemoryAvailable(IGSMSMS *po, AEECallback *pCb, 
                                     GSMSMSSMMAReport *pReport)

Parameters:
   pMe:     Pointer to the IGSMSMS object
   pCb:     Pointer to AEECallback to call upon response message arrival
   pReport: Pointer to structure to be filled in with report

Return Value:
   If successful: AEE_GSMSMS_SUCCESS. 
   If the message could not be sent: AEE_GSMSMS_EFAILED 
   
Comments:  
   None

Side Effects: 
   None

===========================================================================*/
static int OEMGSMSMS_SendMoreMemoryAvailable
(
   struct _IGSMSMS *pMe, 
   AEECallback *pCb,
   GSMSMSSMMAReport *pReport
)
{
   byte SMS_MA[2];
   uint8 idx = 0;
   int rv;

   FARF(UNITTEST, ("Called OEMGSMSMS_SendMoreMemoryAvailable"));

   SMS_MA[idx++] = GSMSMS_SM_MEMORY_AVAILABLE;
   SMS_MA[idx++] = gRefNum++; // RP-MR

   CALLBACK_Cancel(pCb);
   pMe->m_pReport = (void *)pReport;
   pMe->m_pCallback = pCb;

   // Set the cancel CB
   pCb->pfnCancel = GSMSMS_CancelCb;
   pCb->pCancelData = pMe;

   rv = GSMSMS_SendSMSRaw(
          (const byte *)SMS_MA,
          idx,
          USER_CB_SMMA,
          TRUE); 

   if (rv != AEE_GSMSMS_SUCCESS)
   {
      pCb->pfnCancel = NULL;
      pCb->pCancelData = NULL;
      pMe->m_pReport = NULL;
      pMe->m_pCallback = NULL;
   }
   else
   {
      (void)ISHELL_SetTimer(pMe->m_pIShell, GSMSMS_TIMEOUT_VAL, GSMSMS_SMMATimerCallback, (void *)pMe);
   }
   return rv;
}

/*===========================================================================
Function: OEMGSMSMS_GetMemoryCapExceededFlag()

Description:

   This function retrieves the default Memory Capacity Exceeded Flag on the 
   SIM. In accordance with the GSM TS 11.11 spec, pFlag is set to 0 if 
   the flag is set, and set to 1 if the flag is not set. All other values 
   are reserved.

Prototype:
   void OEMGSMSMS_GetMemoryCapExceededFlag(IGSMSMS * po, uint8 *pFlag) 

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

===========================================================================*/
static int OEMGSMSMS_GetMemoryCapExceededFlag
(
   struct _IGSMSMS *pMe, uint8 *pFlag
)
{
   CardHandlerSMSSinfoType smssInfo;

   if (OEMGSM1xCardHandler_ReadSMSS(&smssInfo) == UIM_FAIL)
   {
      return AEE_GSMSMS_EFAILED;
   }

   *pFlag = smssInfo.memCapExcInd;

   return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
Function: OEMGSMSMS_SetMemoryCapExceededFlag()

Description:

   This function updates the Memory Capacity Exceeded Flag on the 
   SIM. In accordance with the GSM TS 11.11 spec, flag should be set to 0 
   if the MemCapExceded flag is set, and set to 1 if the it is not set. All 
   other values are reserved.

Prototype:
   void OEMGSMSMS_SetMemoryCapExceededFlag(IGSMSMS * po, uint8 flag) 

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

=============================================================================*/
static int OEMGSMSMS_SetMemoryCapExceededFlag
(
   struct _IGSMSMS *pMe, uint8 flag
)
{
   CardHandlerSMSSinfoType smssInfo;

   if (OEMGSM1xCardHandler_ReadSMSS(&smssInfo) == UIM_FAIL)
   {
      return AEE_GSMSMS_EFAILED;
   }

   smssInfo.memCapExcInd = flag;

   if (OEMGSM1xCardHandler_SetSMSS(&smssInfo) == UIM_FAIL)
   {
      return AEE_GSMSMS_EFAILED;
   }
   
   return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
Function: OEMGSMSMS_GetTPMR()

Description:

   This function retrieves the TP-MR value from EF-SMSS on the 
   SIM. 

Prototype:
   int OEMGSMSMS_GetTPMR(IGSMSMS * po, uint8 *pTPMR) 

Parameters:
   po:   Pointer to the IGSMSMS object
   pTPMR: Pointer to variable to set to TP-MR value in the SMSS on the SIM

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the flag could not be set: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

=============================================================================*/
static int OEMGSMSMS_GetTPMR(struct _IGSMSMS *pMe, uint8 *pTPMR)
{
   CardHandlerSMSSinfoType smssInfo;

   if (OEMGSM1xCardHandler_ReadSMSS(&smssInfo) == UIM_FAIL)
   {
      return AEE_GSMSMS_EFAILED;
   }

   *pTPMR = smssInfo.lastUsedTP_MR;
   return AEE_GSMSMS_SUCCESS;
}
/*=============================================================================
Function: OEMGSMSMS_SetTPMR()

Description:

   This function updates the TP-MR value on the 
   SIM. 

Prototype:
   void OEMGSMSMS_SetTPMR(IGSMSMS * po, uint8 TPMR) 

Parameters:
   po:   Pointer to the IGSMSMS object
   TPMR: New setting for the TP-MR in the SMSS on the SIM

Return Value:
   If successful: AEE_GSMSMS_SUCCESS 
   If the flag could not be set: AEE_GSMSMS_EFAILED
   
Comments:  
   None

Side Effects: 
   None

=============================================================================*/
static int OEMGSMSMS_SetTPMR(struct _IGSMSMS *pMe, uint8 TPMR)
{
   CardHandlerSMSSinfoType smssInfo;

   if (OEMGSM1xCardHandler_ReadSMSS(&smssInfo) == UIM_FAIL)
   {
      return AEE_GSMSMS_EFAILED;
   }

   smssInfo.lastUsedTP_MR = TPMR;

   if (OEMGSM1xCardHandler_SetSMSS(&smssInfo) == UIM_FAIL)
   {
      return AEE_GSMSMS_EFAILED;
   }
   
   return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMGSMSMS_IsInit

DESCRIPTION:
  Called to indicate whether interface is init
        
PARAMETERS:
  pMe:   pointer to struct _IGSMSMS object

RETURN VALUE:
  none

SIDE EFFECTS:
  None
=============================================================================*/
static boolean OEMGSMSMS_IsInit(struct _IGSMSMS *pMe)
{
   if (pMe->m_bCardHandlerIsInit == FALSE)
   {
      // Incase the phone was just CHV enabled
      pMe->m_bCardHandlerIsInit = OEMGSM1xCardHandler_Init();
   }

   // Check again in case the value is stale
   pMe->m_bSIMSMSEnabled = OEMGSM1xCardHandler_IsSMSEnabled();
   
   //DBGPRINTF("OEMGSMSMS (2) pMe->m_bSIMSMSEnabled = %d", pMe->m_bSIMSMSEnabled);
   if (pMe->m_bCardHandlerIsInit && OEMGSM1xCardHandler_IsSMSInit())
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

/*=============================================================================
FUNCTION: OEMGSMSMS_InjectMessage

DESCRIPTION:
  This is a test interface that enables unit testing of an app that uses
  the IGSMSMS interface. It will send a SMSDeliver message to the mobile
  as if it has come in over the network.

  This can be modified to always return AEE_GSMSMS_EFAILED in the final
  version if unit testing is disabled.

PARAMETERS:
   pMe: [in]  : Pointer to the IGSMSMS object
   pData [in] : GSM RP layer data of SMS Deliver message to send
   length [in]: length of the data in bytes

RETURN VALUE:
  none

SIDE EFFECTS:
  none
=============================================================================*/
void OEMGSMSMS_InjectMessage(struct _IGSMSMS *pMe, const byte * pData, uint8 length)
{

   byte tempData[GSM1xMESSAGE_SIGNALING_MAX];

   if (length > GSM1xMESSAGE_SIGNALING_MAX)
   {
      FARF(ERROR, ("length parameter too large"));
      return;
   }

   MEMSET(tempData, 0, sizeof(tempData));
   MEMCPY(tempData, pData, length);

   // if SubmitReport injected set MR to real MR
   if (pData[0] == 3 || pData[0] == 5)
   {
      if (pData[1] == 0)
      {
         tempData[1] = pMe->m_SubmitRP_MR;
      }
      else
      {
         tempData[1] = pMe->m_SubmitRP_MR+1;
      }
   }

   OEMGSMSMS_SVC_ProcessMessage(tempData, length);
}


/*===========================================================================

                     Public Non-IGSMSMS Methods   

===========================================================================*/


/*=============================================================================
FUNCTION: OEMGSMSMS_RerouteTAPI

DESCRIPTION:
  Called to send BREW directed messages (sent via ITAPI_SendSMS) while in 
  GSM1x mode. If pszDst is an email address, the message will not be sent 
  successfully.

PARAMETERS:
   pITapi [in]: Pointer to the ITAPI Interface object  
   pszDst [in]: Number of the destination where message must be sent to.

   pszMsg [in]: Text message to be sent to the destination mobile.
   clsID  [in]: Specifies the class ID of the BREW applicaiton on the 
                destination mobile to which this message must be sent  
   pfn [in]   : Notification function that is invoked to inform the status 
                of the SMS message sent  
   pUser [in] : User data to be sent to the notification function  

RETURN VALUE:
  none

SIDE EFFECTS:
  none
=============================================================================*/
int OEMGSMSMS_RerouteTAPI
(
   const char * pszDst,
   const char * pszMsg,
   AEECLSID clsDst,
   PFNSMSSTATUS pfn,
   void *pUser
)
{
   GSMSMSMsg gsm;
   AECHAR unicodeMsg[300]; 
   int rv;
   AECHAR *ptr;
   int brewPrefixLen;
   byte dummyEncodedLen;

   if (sGSMSMS.m_bInitialized != TRUE) 
   {
      return(EFAILED);
   }

   // Use default SC address
   rv = OEMGSMSMS_CreateDefaultMessage(&sGSMSMS, GSMSMS_MSG_SMS_SUBMIT, &gsm);
   if (rv != AEE_GSMSMS_SUCCESS)
   {
      // Map GSMSMS errors to standard BREW errors
      if (rv == AEE_GSMSMS_ENODEFAULTSCADDR)
      {
         return ENEEDMORE;
      }
      else
      {
         return EFAILED;
      }
   }

   gsm.msg.SMSSubmit.DCS.codingGroup = GSMSMS_DCS_CG_GENERAL_DATA;
   gsm.msg.SMSSubmit.DCS.msgCoding = GSMSMS_ENCODING_7BIT;
   gsm.msg.SMSSubmit.DCS.cg.generalData.msgClass = GSMSMS_MSG_CLASS_NONE;
   gsm.msg.SMSSubmit.DCS.cg.generalData.isCompressed = FALSE;
   gsm.msg.SMSSubmit.DCS.cg.generalData.autoDelete = FALSE; // Should this be here?

   // Check for BREW directed message
   ptr = unicodeMsg;
   if(clsDst)
   {
      char temp[20];

      (void) SPRINTF(temp,"%s%x:",BREW_SMS_PREFIX,clsDst);
      STR_TO_WSTR(temp, unicodeMsg, sizeof(unicodeMsg));
      ptr += STRLEN(temp);
      brewPrefixLen = STRLEN(temp);
   }
   else
   {
      // Only BREW directed messages are re-routed
      return(EFAILED);
   }

   STR_TO_WSTR(pszMsg, ptr, sizeof(unicodeMsg)-(sizeof(AECHAR)*brewPrefixLen));

   rv = OEMGSMSMS_EncodeUserData(&sGSMSMS, unicodeMsg, &gsm.msg.SMSSubmit.UD[0], 
           sizeof(gsm.msg.SMSSubmit.UD), GSMSMS_ENCODING_7BIT, &dummyEncodedLen);

   gsm.msg.SMSSubmit.UDL=WSTRLEN(unicodeMsg);


   if (rv != AEE_GSMSMS_SUCCESS)
   {
      return rv;
   }

   // Add the destination address 
   MEMSET(gsm.msg.SMSSubmit.DA.addr, 0, sizeof(gsm.msg.SMSSubmit.DA.addr));
   gsm.msg.SMSSubmit.DA.npi = GSMSMS_NPI_ISDN_TELEPHONE;
   // Check for international number
   if (gsm.msg.SMSSubmit.DA.addr[0] == '+')
   {
      gsm.msg.SMSSubmit.DA.ton = GSMSMS_TON_INTERNATIONAL_NUMBER;
      STRNCPY((char *)&gsm.msg.SMSSubmit.DA.addr[1], pszDst, sizeof(gsm.msg.SMSSubmit.DA.addr)-1);
   }
   else
   {
      gsm.msg.SMSSubmit.DA.ton = GSMSMS_TON_NATIONAL_NUMBER;
      STRNCPY((char *)&gsm.msg.SMSSubmit.DA.addr[0], pszDst, sizeof(gsm.msg.SMSSubmit.DA.addr));
   }

   CALLBACK_Init(&sGSMSMS.m_TAPICallback, GSMSMS_CbAdapterForTAPI, pUser);
   sGSMSMS.m_TAPIpfn = pfn;
   
   ISHELL_RegisterSystemCallback(sGSMSMS.m_pIShell, &sGSMSMS.m_TAPICallback, AEE_SCB_APP_CLOSED);

   return OEMGSMSMS_SendSMSSubmit(&sGSMSMS, &gsm.msg.SMSSubmit, &sGSMSMS.m_TAPICallback, &sGSMSMS.m_TAPIReport);
}

/*===========================================================================

                     Private Methods   

===========================================================================*/
/*=============================================================================
FUNCTION: GSMSMS_InitGSMCharMap

DESCRIPTION:
  Initialize the map between 7BIT GSM characters and theit UNICODE equivalents
        
PARAMETERS:
  none

RETURN VALUE:
  none

SIDE EFFECTS:
  None
=============================================================================*/
STATIC void GSMSMS_InitGSMCharMap(void)
{
   FARF(UNITTEST, ("Called InitGSMCharMap"));

   gsmToUTF16[0] = 0x0040; // COMMERCIAL AT @ 
   gsmToUTF16[1] = 0x00A3; // POUND SIGN ?
   gsmToUTF16[2] = 0x0024; // DOLLAR SIGN $
   gsmToUTF16[3] = 0x00A5; // YEN SIGN ?
   gsmToUTF16[4] = 0x00E8; // 0x04 4 LATIN SMALL LETTER E WITH GRAVE ?
   gsmToUTF16[5] = 0x00E9; // LATIN SMALL LETTER E WITH ACUTE ?
   gsmToUTF16[6] = 0x00F9; // LATIN SMALL LETTER U WITH GRAVE 
   gsmToUTF16[7] = 0x00EC; // LATIN SMALL LETTER I WITH GRAVE ?
   gsmToUTF16[8] = 0x00F2; // LATIN SMALL LETTER O WITH GRAVE ?
   gsmToUTF16[9] = 0x00E7; // LATIN CAPITAL LETTER C WITH CEDILLA ?
   gsmToUTF16[10] = 0x000A; // LINE FEED 
   gsmToUTF16[11] = 0x00D8; // LATIN CAPITAL LETTER O WITH STROKE ?
   gsmToUTF16[12] = 0x00F8; // LATIN SMALL LETTER O WITH STROKE ?
   gsmToUTF16[13] = 0x000D; // CARRIAGE RETURN 
   gsmToUTF16[14] = 0x00C5; // LATIN CAPITAL LETTER A WITH RING ABOVE ?
   gsmToUTF16[15] = 0x00E5; // 0x0F 15 LATIN SMALL LETTER A WITH RING ABOVE ?229 

   // The UTF16 characters need ISO 8859-7 fonts to display
   gsmToUTF16[16] = 0x0394; // GREEK CAPITAL LETTER DELTA
   gsmToUTF16[17] = 0x005F; //  LOW LINE _
   gsmToUTF16[18] = 0x03A6; // GREEK CAPITAL LETTER PHI
   gsmToUTF16[19] = 0x0393; // GREEK CAPITAL LETTER GAMMA
   gsmToUTF16[20] = 0x039B; // GREEK CAPITAL LETTER LAMBDA
   gsmToUTF16[21] = 0x03A9; // GREEK CAPITAL LETTER OMEGA
   gsmToUTF16[22] = 0x03A0; // GREEK CAPITAL LETTER PI
   gsmToUTF16[23] = 0x03A8; // GREEK CAPITAL LETTER PSI
   gsmToUTF16[24] = 0x03A3; // GREEK CAPITAL LETTER SIGMA
   gsmToUTF16[25] = 0x0398; // GREEK CAPITAL LETTER THETA
   gsmToUTF16[26] = 0x039E; // GREEK CAPITAL LETTER XI

   gsmToUTF16[27] = 0x00A0; // ESCAPE TO EXTENSION TABLE (or NBSP)

   gsmToUTF16[28] = 0x00C6; // LATIN CAPITAL LETTER AE ?
   gsmToUTF16[29] = 0x00E6; // LATIN SMALL LETTER AE ?230 
   gsmToUTF16[30] = 0x00DF; // LATIN SMALL LETTER SHARP S (German) ?
   gsmToUTF16[31] = 0x00C9; // LATIN CAPITAL LETTER E WITH ACUTE 

   gsmToUTF16[32] = 0x0020; // SPACE
   gsmToUTF16[33] = 0x0021; // EXCLAMATION MARK
   gsmToUTF16[34] = 0x0022; // QUOTATION MARK
   gsmToUTF16[35] = 0x0023; // NUMBER SIGN

   gsmToUTF16[36] = 0x00A4; // CURRENCY SIGN ?

   gsmToUTF16[37] = 0x0025; // PERCENT SIGN %
   gsmToUTF16[38] = 0x0026; // AMPERSAND &
   gsmToUTF16[39] = 0x0027; // APOSTROPHE '
   gsmToUTF16[40] = 0x0028; // LEFT PARENTHESIS (
   gsmToUTF16[41] = 0x0029; // RIGHT PARENTHESIS )
   gsmToUTF16[42] = 0x002A; // ASTERISK *
   gsmToUTF16[43] = 0x002B; // PLUS SIGN +
   gsmToUTF16[44] = 0x002C; // COMMA ,
   gsmToUTF16[45] = 0x002D; // HYPHEN-MINUS -
   gsmToUTF16[46] = 0x002E; // FULL STOP .
   gsmToUTF16[47] = 0x002F; // SOLIDUS (SLASH) /
   gsmToUTF16[48] = 0x0030; // DIGIT ZERO 0
   gsmToUTF16[49] = 0x0031; // DIGIT ONE 1
   gsmToUTF16[50] = 0x0032; // DIGIT TWO 2
   gsmToUTF16[51] = 0x0033; // DIGIT THREE 3
   gsmToUTF16[52] = 0x0034; // DIGIT FOUR 4
   gsmToUTF16[53] = 0x0035; // DIGIT FIVE 5
   gsmToUTF16[54] = 0x0036; // DIGIT SIX 6
   gsmToUTF16[55] = 0x0037; // DIGIT SEVEN 7
   gsmToUTF16[56] = 0x0038; // DIGIT EIGHT 8
   gsmToUTF16[57] = 0x0039; // DIGIT NINE 9
   gsmToUTF16[58] = 0x003A; // COLON :
   gsmToUTF16[59] = 0x003B; // SEMICOLON ;
   gsmToUTF16[60] = 0x003C; // LESS-THAN SIGN <
   gsmToUTF16[61] = 0x003D; // EQUALS SIGN =
   gsmToUTF16[62] = 0x003E; // GREATER-THAN SIGN >
   gsmToUTF16[63] = 0x003F; // QUESTION MARK ? 

   gsmToUTF16[64] = 0x00A1; // INVERTED EXCLAMATION MARK ?

   gsmToUTF16[65] = 0x0041; // LATIN CAPITAL LETTER A 
   gsmToUTF16[66] = 0x0042; // LATIN CAPITAL LETTER B 
   gsmToUTF16[67] = 0x0043; // LATIN CAPITAL LETTER C 
   gsmToUTF16[68] = 0x0044; // LATIN CAPITAL LETTER D 
   gsmToUTF16[69] = 0x0045; // LATIN CAPITAL LETTER E 
   gsmToUTF16[70] = 0x0046; // LATIN CAPITAL LETTER F 
   gsmToUTF16[71] = 0x0047; // LATIN CAPITAL LETTER G 
   gsmToUTF16[72] = 0x0048; // LATIN CAPITAL LETTER H 
   gsmToUTF16[73] = 0x0049; // LATIN CAPITAL LETTER I 
   gsmToUTF16[74] = 0x004A; // LATIN CAPITAL LETTER J 
   gsmToUTF16[75] = 0x004B; // LATIN CAPITAL LETTER K 
   gsmToUTF16[76] = 0x004C; // LATIN CAPITAL LETTER L 
   gsmToUTF16[77] = 0x004D; // LATIN CAPITAL LETTER M 
   gsmToUTF16[78] = 0x004E; // LATIN CAPITAL LETTER N 
   gsmToUTF16[79] = 0x004F; // LATIN CAPITAL LETTER O 
   gsmToUTF16[80] = 0x0050; // LATIN CAPITAL LETTER P 
   gsmToUTF16[81] = 0x0051; // LATIN CAPITAL LETTER Q 
   gsmToUTF16[82] = 0x0052; // LATIN CAPITAL LETTER R 
   gsmToUTF16[83] = 0x0053; // LATIN CAPITAL LETTER S 
   gsmToUTF16[84] = 0x0054; // LATIN CAPITAL LETTER T 
   gsmToUTF16[85] = 0x0055; // LATIN CAPITAL LETTER U 
   gsmToUTF16[86] = 0x0056; // LATIN CAPITAL LETTER V 
   gsmToUTF16[87] = 0x0057; // LATIN CAPITAL LETTER W 
   gsmToUTF16[88] = 0x0058; // LATIN CAPITAL LETTER X 
   gsmToUTF16[89] = 0x0059; // LATIN CAPITAL LETTER Y 
   gsmToUTF16[90] = 0x005A; // LATIN CAPITAL LETTER Z

   gsmToUTF16[91] = 0x00C0; // LATIN CAPITAL LETTER A WITH DIAERESIS ?
   gsmToUTF16[92] = 0x00D6; // LATIN CAPITAL LETTER O WITH DIAERESIS ?
   gsmToUTF16[93] = 0x00D1; // LATIN CAPITAL LETTER N WITH TILDE ?
   gsmToUTF16[94] = 0x00DC; // LATIN CAPITAL LETTER U WITH DIAERESIS ?
   gsmToUTF16[95] = 0x00A7; // SECTION SIGN ?
   gsmToUTF16[96] = 0x00B0; // INVERTED QUESTION MARK ?

   gsmToUTF16[97] = 0x0061; // LATIN SMALL LETTER A 
   gsmToUTF16[98] = 0x0062; // LATIN SMALL LETTER B 
   gsmToUTF16[99] = 0x0063; // LATIN SMALL LETTER C 
   gsmToUTF16[100] = 0x0064; // LATIN SMALL LETTER D 
   gsmToUTF16[101] = 0x0065; // LATIN SMALL LETTER E 
   gsmToUTF16[102] = 0x0066; // LATIN SMALL LETTER F 
   gsmToUTF16[103] = 0x0067; // LATIN SMALL LETTER G 
   gsmToUTF16[104] = 0x0068; // LATIN SMALL LETTER H 
   gsmToUTF16[105] = 0x0069; // LATIN SMALL LETTER I 
   gsmToUTF16[106] = 0x006A; // LATIN SMALL LETTER J 
   gsmToUTF16[107] = 0x006B; // LATIN SMALL LETTER K 
   gsmToUTF16[108] = 0x006C; // LATIN SMALL LETTER L 
   gsmToUTF16[109] = 0x006D; // LATIN SMALL LETTER M 
   gsmToUTF16[110] = 0x006E; // LATIN SMALL LETTER N 
   gsmToUTF16[111] = 0x006F; // LATIN SMALL LETTER O 
   gsmToUTF16[112] = 0x0070; // LATIN SMALL LETTER P 
   gsmToUTF16[113] = 0x0071; // LATIN SMALL LETTER Q 
   gsmToUTF16[114] = 0x0072; // LATIN SMALL LETTER R 
   gsmToUTF16[115] = 0x0073; // LATIN SMALL LETTER S 
   gsmToUTF16[116] = 0x0074; // LATIN SMALL LETTER T 
   gsmToUTF16[117] = 0x0075; // LATIN SMALL LETTER U 
   gsmToUTF16[118] = 0x0076; // LATIN SMALL LETTER V 
   gsmToUTF16[119] = 0x0077; // LATIN SMALL LETTER W 
   gsmToUTF16[120] = 0x0078; // LATIN SMALL LETTER X 
   gsmToUTF16[121] = 0x0079; // LATIN SMALL LETTER Y 
   gsmToUTF16[122] = 0x007A; // LATIN SMALL LETTER Z 

   gsmToUTF16[123] = 0x00FA; // LATIN SMALL LETTER A WITH DIAERESIS ?
   gsmToUTF16[124] = 0x00E4; // LATIN SMALL LETTER O WITH DIAERESIS ?
   gsmToUTF16[125] = 0x00F5; // LATIN SMALL LETTER N WITH TILDE ?
   gsmToUTF16[126] = 0x00FC; // LATIN SMALL LETTER U WITH DIAERESIS ?
   gsmToUTF16[127] = 0x00E0; // LATIN SMALL LETTER A WITH GRAVE ?
}

/*=============================================================================
FUNCTION: GSMSMS_UnicodeToGSM

DESCRIPTION:
  Convert a UNICODE character to its GSM 7BIT equivalent.
        
PARAMETERS:
 uc [in]         : UNICODE character to convert
 gsm [out]       : pointer to char to set with 7-bit GSM equivalent
 isExtended [out]: pointer to flag to set if character is an extended 
                   GSM character

RETURN VALUE:
  Returns SUCCESS if the character is a valid 7-bit GSM character,
  otherwise EFAILED

SIDE EFFECTS:
  None
=============================================================================*/
STATIC int GSMSMS_UnicodeToGSM(AECHAR uc, byte *gsm, boolean *isExtended)
{
  *isExtended = FALSE;

  switch (uc)
  {
  case 0x0040: // COMMERCIAL AT @
     *gsm=0;
     break;
  case 0x00A3: // POUND SIGN ?
     *gsm=1;
     break;
  case 0x0024: // DOLLAR SIGN $
     *gsm=2;
     break;
  case 0x00A5: // YEN SIGN ?
     *gsm=3;
     break;
  case 0x00E8: // 0x04 4 LATIN SMALL LETTER E WITH GRAVE ?
     *gsm=4;
     break;
  case 0x00E9: // LATIN SMALL LETTER E WITH ACUTE ?
     *gsm=5;
     break;
  case 0x00F9: // LATIN SMALL LETTER U WITH GRAVE
     *gsm=6;
     break;
  case 0x00EC: // LATIN SMALL LETTER I WITH GRAVE ?
     *gsm=7;
     break;
  case 0x00F2: // LATIN SMALL LETTER O WITH GRAVE ?
     *gsm=8;
     break;
  case 0x00E7: // LATIN CAPITAL LETTER C WITH CEDILLA ?
     *gsm=9;
     break;
  case 0x000A: // LINE FEED
     *gsm=10;
     break;
  case 0x00D8: // LATIN CAPITAL LETTER O WITH STROKE ?
     *gsm=11;
     break;
  case 0x00F8: // LATIN SMALL LETTER O WITH STROKE ?
     *gsm=12;
     break;
  case 0x000D: // CARRIAGE RETURN
     *gsm=13;
     break;
  case 0x00C5: // LATIN CAPITAL LETTER A WITH RING ABOVE ?
     *gsm=14;
     break;
  case 0x00E5: // 0x0F 15 LATIN SMALL LETTER A WITH RING ABOVE ?229
     *gsm=15;
     break;

   // The UTF16 characters need ISO 8859-7 fonts to display
  case 0x0394: // GREEK CAPITAL LETTER DELTA
     *gsm=16;
     break;
  case 0x005F: //  LOW LINE _
     *gsm=17;
     break;
  case 0x03A6: // GREEK CAPITAL LETTER PHI
     *gsm=18;
     break;
  case 0x0393: // GREEK CAPITAL LETTER GAMMA
     *gsm=19;
     break;
  case 0x039B: // GREEK CAPITAL LETTER LAMBDA
     *gsm=20;
     break;
  case 0x03A9: // GREEK CAPITAL LETTER OMEGA
     *gsm=21;
     break;
  case 0x03A0: // GREEK CAPITAL LETTER PI
     *gsm=22;
     break;
  case 0x03A8: // GREEK CAPITAL LETTER PSI
     *gsm=23;
     break;
  case 0x03A3: // GREEK CAPITAL LETTER SIGMA
     *gsm=24;
     break;
  case 0x0398: // GREEK CAPITAL LETTER THETA
     *gsm=25;
     break;
  case 0x039E: // GREEK CAPITAL LETTER XI
     *gsm=26;
     break;

  case 0x00A0: // ESCAPE TO EXTENSION TABLE (or NBSP)
     *gsm=27; 
     break;
  case 0x00C6: // LATIN CAPITAL LETTER AE ?
     *gsm=28;
     break;
  case 0x00E6: // LATIN SMALL LETTER AE ?230
     *gsm=29;
     break;
  case 0x00DF: // LATIN SMALL LETTER SHARP S (German) ?
     *gsm=30;
     break;
  case 0x00C9: // LATIN CAPITAL LETTER E WITH ACUTE
     *gsm=31;
     break;

  case 0x00A4: // CURRENCY SIGN ?
     *gsm=36;
     break;

  case 0x00C0: // LATIN CAPITAL LETTER A WITH DIAERESIS ?
     *gsm=91;
     break;
  case 0x00D6: // LATIN CAPITAL LETTER O WITH DIAERESIS ?
     *gsm=92;
     break;
  case 0x00D1: // LATIN CAPITAL LETTER N WITH TILDE ?
     *gsm=93;
     break;
  case 0x00DC: // LATIN CAPITAL LETTER U WITH DIAERESIS ?
     *gsm=94;
     break;
  case 0x00A7: // SECTION SIGN ?
     *gsm=95;
     break;
  case 0x00B0: // INVERTED QUESTION MARK ?
     *gsm=96;
     break;

  case 0x00FA: // LATIN SMALL LETTER A WITH DIAERESIS ?
     *gsm=123; 
     break;
  case 0x00E4: // LATIN SMALL LETTER O WITH DIAERESIS ?
     *gsm=124;
     break;
  case 0x00F5: // LATIN SMALL LETTER N WITH TILDE ?
     *gsm=125;
     break;
  case 0x00FC: // LATIN SMALL LETTER U WITH DIAERESIS ?
     *gsm=126;
     break;
  case 0x00E0: // LATIN SMALL LETTER A WITH GRAVE ?
     *gsm=127;
     break;

  // Extended Characters
  case 0x000C: // FORM FEED
     *gsm=0x0A;
     *isExtended = TRUE;
     break;
  case 0x005E: // CIRCUMFLEX ACCENT
     *gsm=0x14;
     *isExtended = TRUE;
     break;
  case 0x007B: // LEFT CURLY BRACKET
     *gsm=0x28;
     *isExtended = TRUE;
     break;
  case 0x007D: // RIGHT CURLY BRACKET
     *gsm=0x29;
     *isExtended = TRUE;
     break;
  case 0x005C: // REVERSE SOLIDUS
     *gsm=0x2F;
     *isExtended = TRUE;
     break;
  case 0x005B: // LEFT SQUARE BRACKET
     *gsm=0x3C;
     *isExtended = TRUE;
     break;
  case 0x007E: // TILDE
     *gsm=0x3D;
     *isExtended = TRUE;
     break;
  case 0x005D: // RIGHT SQUARE BRACKET
     *gsm=0x3E;
     *isExtended = TRUE;
     break;

  case 0x00A1: // INVERTED EXCLAMATION MARK ?
     *gsm=64;
     break;

  default:

     if ((uc >= 0x0020 && uc <= 0x005A && uc != 0x00A1 && uc != 0x00A4) ||
         (uc >= 0x0061 && uc <= 0x007A))
     {
        *gsm = (byte)uc;
     }
     else
     {
        return EFAILED;
     }
  }
  return SUCCESS;
}

/*=============================================================================
FUNCTION: GSMSMS_OpenDB

DESCRIPTION:
  Called when accessing the NVRAM databases. The open IFile object is released
  by a timer if the object is not used for more than one second.
        
PARAMETERS:
 pMe:   pointer to struct _IGSMSMS object
 store: The NVRAM store to use

RETURN VALUE:
  Returns the IFile pointer or NULL

SIDE EFFECTS:
  None
=============================================================================*/
static IFile * GSMSMS_OpenDB(struct _IGSMSMS *pMe, GSMSMSStorageType store)
{
   if (store == GSMSMS_STORE_NVRAM)
   {
      if (pMe->m_pSMSMbox == NULL)
      {
         ACONTEXT * prevContext;

         // Set App Context to System for this call
         prevContext = AEE_EnterAppContext(NULL);

         pMe->m_pSMSMbox = IFILEMGR_OpenFile(sGSMSMS.m_pIFileMgr, NVRAM_DB_NAME, _OFM_READWRITE);
         AEE_LeaveAppContext(prevContext);
         
         // Set the timer
         (void)ISHELL_SetTimer(pMe->m_pIShell, 1000, GSMSMS_SMSMboxTimerCallback, (void *)pMe);
      }
      else
      {
         (void)ISHELL_CancelTimer(pMe->m_pIShell, GSMSMS_SMSMboxTimerCallback, (void *)pMe); 
         (void)ISHELL_SetTimer(pMe->m_pIShell, 1000, GSMSMS_SMSMboxTimerCallback, (void *)pMe);
      }
      return pMe->m_pSMSMbox;
   }
   else if (store == GSMSMS_STORE_NVRAM_VM)
   {
      if (pMe->m_pVMMbox == NULL)
      {
         ACONTEXT * prevContext;

         // Set App Context to System for this call
         prevContext = AEE_EnterAppContext(NULL);
    
         pMe->m_pVMMbox = IFILEMGR_OpenFile(sGSMSMS.m_pIFileMgr, NVRAM_VM_DB_NAME, _OFM_READWRITE);
         AEE_LeaveAppContext(prevContext);

         // Set the timer
         (void)ISHELL_SetTimer(pMe->m_pIShell, 1000, GSMSMS_VMMboxTimerCallback, (void *)pMe);
      }
      else
      {
         // Restart the Timer
         (void)ISHELL_CancelTimer(pMe->m_pIShell, GSMSMS_VMMboxTimerCallback, (void *)pMe); 
         (void)ISHELL_SetTimer(pMe->m_pIShell, 1000, GSMSMS_VMMboxTimerCallback, (void *)pMe);
      }
      return pMe->m_pVMMbox;
   }
   return NULL;
}

/*=============================================================================
FUNCTION: GSMSMS_VMMboxTimerCallback

DESCRIPTION:
  Called when the Voicemail mailbox timer expires
        
PARAMETERS:
  pData : pointer passed to the callback

RETURN VALUE:
  none

SIDE EFFECTS:
  None
=============================================================================*/
static void GSMSMS_VMMboxTimerCallback(void *pData)
{
   struct _IGSMSMS *pMe = &sGSMSMS;

   if (pMe->m_pVMMbox != NULL)
   {
      IFILE_Release(pMe->m_pVMMbox);
      pMe->m_pVMMbox = NULL;
   }
}

/*=============================================================================
FUNCTION: GSMSMS_SMSMboxTimerCallback

DESCRIPTION:
  Called when the SMS mailbox timer expires
        
PARAMETERS:
  pData : pointer passed to the callback

RETURN VALUE:
  none

SIDE EFFECTS:
  None
=============================================================================*/
static void GSMSMS_SMSMboxTimerCallback(void *pData)
{
   struct _IGSMSMS *pMe = &sGSMSMS;

   if (pMe->m_pSMSMbox != NULL)
   {
      IFILE_Release(pMe->m_pSMSMbox);
      pMe->m_pSMSMbox = NULL;
   }
}


/*=============================================================================
FUNCTION: GSMSMS_InitTimerCallback

DESCRIPTION:
  Called to indicate interface is init
        
PARAMETERS:
  data : pointer passed to the callback

RETURN VALUE:
  none

SIDE EFFECTS:
  None
=============================================================================*/
static void GSMSMS_InitTimerCallback(void *pData)
{
   struct _IGSMSMS *pMe = &sGSMSMS;

   if (pMe->m_bCardHandlerIsInit == FALSE)
   {
      // Incase the phone was just CHV enabled
      pMe->m_bCardHandlerIsInit = OEMGSM1xCardHandler_Init();
   }
   if (pMe->m_bCardHandlerIsInit == TRUE)
   {
      // Check again in case the value is stale
      pMe->m_bSIMSMSEnabled = OEMGSM1xCardHandler_IsSMSEnabled();
      //DBGPRINTF("OEMGSMSMS pMe->m_bSIMSMSEnabled = %d", pMe->m_bSIMSMSEnabled);

      if (OEMGSM1xCardHandler_IsSMSInit())
      {
         (void)ISHELL_Notify(pMe->m_pIShell, AEECLSID_IGSMSMSNOTIFIER, 
                             NMASK_GSMSMS_INIT, 0);
         return;
      }
   }

   (void)ISHELL_SetTimer(pMe->m_pIShell, 1000,
                         GSMSMS_InitTimerCallback, (void *)pMe);
}

/*=============================================================================
FUNCTION: GSMSMS_TimerCallback

DESCRIPTION:
  Called when the transaction timer expires
        
PARAMETERS:
  data : pointer passed to the callback

RETURN VALUE:
  none

SIDE EFFECTS:
  None
=============================================================================*/
static void GSMSMS_TimerCallback(void *pData)
{
   struct _IGSMSMS *pMe = &sGSMSMS;

   if (pMe->m_pCallback != NULL && pMe->m_pReport)
   {
      // Send RP_ERROR to app
      GSMSMSSendReport *pSendReport = (GSMSMSSendReport *)pMe->m_pReport;

      if (pData != NULL)
      {
         pSendReport->sendResult = AEE_GSMSMS_ETIMEDOUT;
      }
      else
      {
         // This was called because GSMSMS_SendSMSRaw returned a
         // callback indicating the send failed
         pSendReport->sendResult = AEE_GSMSMS_EFAILED;
      }
      
      // Terminate the Transaction
      OEMGSM1xMessage_ReleaseSMSTransaction();

      ISHELL_Resume(pMe->m_pIShell,pMe->m_pCallback);
   }
}

/*=============================================================================
FUNCTION: GSMSMS_SMMATimerCallback

DESCRIPTION:
  Called when the SMMA transaction timer expires
        
PARAMETERS:
  data : pointer passed to the callback

RETURN VALUE:
  none

SIDE EFFECTS:
  None
=============================================================================*/
static void GSMSMS_SMMATimerCallback(void *pData)
{
   struct _IGSMSMS *pMe = &sGSMSMS;

   pMe->m_bSMMAPending = FALSE;

   if (pMe->m_pCallback != NULL)
   {
      // Send RP_ERROR to app
      GSMSMSSMMAReport *pReport = (GSMSMSSMMAReport *)pMe->m_pReport;

      pReport->sendResult = AEE_GSMSMS_ETIMEDOUT;
      
      // Terminate the Transaction
      OEMGSM1xMessage_ReleaseSMSTransaction();

      ISHELL_Resume(pMe->m_pIShell,pMe->m_pCallback);
   }

}

/*=============================================================================
FUNCTION: GSMSMS_ConvertToSIMRaw

DESCRIPTION:
  Used to convert a GSM data structure to the format stored on the SIM
        
PARAMETERS:
  pMsg [in] : pointer to the message to encode
  pRawMsg [out] : pointer to the encoded message

RETURN VALUE:
  AEE_GSMSMS_SUCCESS on success,
  AEE_GSMSMS_EBADPARAM, or AEE_GSMSMS_EFAILED otherwise.

SIDE EFFECTS:
  None
=============================================================================*/
STATIC int GSMSMS_ConvertToSIMRaw(const GSMSMSMsg *pMsg, GSMSMSRawMsg *pRawMsg)
{
   int rv;

   if (pRawMsg == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   FARF(UNITTEST, ("Called GSMSMS_ConvertToSIMRaw"));

   MEMSET((void *)pRawMsg, 0xff,sizeof(GSMSMSRawMsg));

    switch (pMsg->msgType)
    {
    case GSMSMS_MSG_SMS_SUBMIT:
       rv = GSMSMSSubmitToRaw(&pMsg->msg.SMSSubmit, pRawMsg);
       break;
    case GSMSMS_MSG_SMS_DELIVER:
       rv = GSMSMS_DeliverToRaw(&pMsg->msg.SMSDeliver, pRawMsg, FALSE);
       break;
    case GSMSMS_MSG_SMS_STATUS_REPORT:
       // Not implemented
    default:
       return AEE_GSMSMS_EBADPARAM;
    }
    return rv;
}

/*=============================================================================
FUNCTION: GSMSMS_DeliverToRaw

DESCRIPTION:
     Encodes the SMSDeliver for the SIM
        
PARAMETERS:
  pMsg    : Pointer to the decoded message
  pRawMsg : Pointer to the raw message
  bRead    : Mark the SMS_Deliver message as read if set to TRUE

RETURN VALUE:
  none

SIDE EFFECTS:
    None
=============================================================================*/
STATIC int GSMSMS_DeliverToRaw
( 
    const GSMSMSDeliverType * pMsg, 
    GSMSMSRawMsg *pRawMsg,
    boolean bRead
)
{
    byte *SMS_Deliver;
    uint16 idx = 0;
    int rv;

    FARF(UNITTEST, ("Called GSMSMS_DeliverToRaw"));

    if (pRawMsg == NULL || pMsg == NULL)
    {
       return AEE_GSMSMS_EBADPARAM;
    }

    pRawMsg->format = GSMSMS_RAW_FORMAT_SIM;

    // Encode SIM status byte
    pRawMsg->msg.sim.rawMsg[0] = (bRead) ? GSMSMS_STATUS_MT_READ : 
                                          GSMSMS_STATUS_MT_NOT_READ;

    // Convert the SC address to GSM encoding
    if ((rv = GSMSMS_EncodeAddress(&pMsg->SCAddr, &pRawMsg->msg.sim.rawMsg[1], TRUE)) < 0)
    {
       FARF(ERROR, ("GSMSMS_EncodeAddress failed"));
       return AEE_GSMSMS_EENCODE;
    }
    idx = (byte)rv+1;

    // Append the GSM SMS_SUBMIT message TL data
    SMS_Deliver=&pRawMsg->msg.sim.rawMsg[0];
    
    // format TLParam as GSM data here
    SMS_Deliver[idx++] = 
        0 | //TP-MTI  2 bits SMS_DELIVER 
        (byte)(pMsg->MMS << 2) | //TP-MMS 
        (byte)(pMsg->SRI << 5) | //TP-SRI
        (byte)(pMsg->UDHI << 6) |//TP-UDHI
        (byte)(pMsg->RP << 7);   //TP-RP 

    // This functionality needs to change
    if ((rv = GSMSMS_EncodeAddress(&pMsg->OA, &SMS_Deliver[idx], FALSE)) < 0)
    {
       FARF(ERROR, ("GSMSMS_EncodeAddress failed"));
       return AEE_GSMSMS_EENCODE;
    }
    idx+=(byte)rv;

    SMS_Deliver[idx++] = pMsg->PID; //TP-PID 1 octet (SME to SME)
    
    // TP-DCS
    GSMSMS_EncodeDCS(&SMS_Deliver[idx++], &pMsg->DCS);

    GSMSMS_EncodeTime(&pMsg->SCTS, &SMS_Deliver[idx]);
    idx+=7;

    SMS_Deliver[idx++] = pMsg->UDL;

    //TP-UD
    MEMCPY((void *)&SMS_Deliver[idx], pMsg->UD, pMsg->UDL);

    idx+=pMsg->UDL;
    FARF(LOW, ("After Encode: UDL = %d, idx = %d",pMsg->UDL, idx));

    return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
FUNCTION: GSMSMSSubmitToRaw

DESCRIPTION:
     Encodes the SMSSubmit for the SIM
        
PARAMETERS:
  pMsg:    [in] SMS Submit message to encode
  pRawMsg: [out] encoded message


RETURN VALUE:
  AEE_GSMSMS_EBADPARAM if a parameter is bad
  AEE_GSMSMS_EENCODE if the encoding failed
  AEE_GSMSMS_EFAILED for a non-specific error
  AEE_GSMSMS_SUCCESS on success
  
SIDE EFFECTS:
    None
=============================================================================*/
STATIC int GSMSMSSubmitToRaw
( 
    const GSMSMSSubmitType * pMsg, 
    GSMSMSRawMsg *pRawMsg
)
{
    byte *SMS_Submit;
    uint16 idx = 0;
    byte len;
    int rv;
    CardHandlerAddressType scAddr;

    FARF(UNITTEST, ("Called GSMSMSSubmitToRaw"));

    if (pRawMsg == NULL || pMsg == NULL)
    {
       return AEE_GSMSMS_EBADPARAM;
    }

    if (pMsg->UDL > sizeof(pMsg->UD))
    {
       FARF(ERROR, ("Error: UDL is too large"));
       return AEE_GSMSMS_EBADPARAM;
    }

    pRawMsg->format = GSMSMS_RAW_FORMAT_SIM;

    // Encode SIM status byte
    pRawMsg->msg.sim.rawMsg[0] = GSMSMS_STATUS_MO_NOT_SENT;

    // Get the default SC address
    if (pMsg->SCAddr.addr[0] == '\0')
    {
       // use default SC address
       if (OEMGSM1xCardHandler_ReadSMSCAddress(0, &scAddr) != UIM_PASS)
       {
          FARF(ERROR, ("ReadSMSCAddress failed"));
          return AEE_GSMSMS_EFAILED;
       }

       // encode the raw SC Addr
       pRawMsg->msg.sim.rawMsg[1] = scAddr.length;
       pRawMsg->msg.sim.rawMsg[2] = 0x80 | 
                                   (byte)(scAddr.nature_of_address << 4) | 
                                   scAddr.numbering_plan;
       MEMCPY(&pRawMsg->msg.sim.rawMsg[3], scAddr.digits, scAddr.length-1);
       idx+=scAddr.length+1;
    }
    else
    {
        // Convert the SC address to GSM encoding
        if ((rv = GSMSMS_EncodeAddress(&pMsg->SCAddr, &pRawMsg->msg.sim.rawMsg[1], TRUE)) < 0)
        {
           FARF(ERROR, ("GSMSMS_EncodeAddress failed"));
           return AEE_GSMSMS_EENCODE;
        }
        idx = (byte)rv+1;
    }

    // Append the GSM SMS_SUBMIT message TL data
    SMS_Submit=&pRawMsg->msg.sim.rawMsg[0];
    
    // format TLParam as GSM data here
    SMS_Submit[idx++] = 
        1 | //TP-MTI  2 bits SMS_SUBMIT (01)
        (byte)(pMsg->RD << 2) |   //TP-RD   1 bit  Reject Duplicates (0)
        (byte)(pMsg->VP.format << 3) | //TP-VPF  2 bits No validity field present (00)
        (byte)(pMsg->SRR << 5) |  //TP-SRR  1 bit  Status report requested
        (byte)(pMsg->UDHI << 6) | //TP-UDHI 1 bit  No user data header
        (byte)(pMsg->RP << 7);    //TP-RP   1 bit  Reply path parameter not set (0)

    //TP-MR
    SMS_Submit[idx++] = pMsg->MR;

    // This functionality needs to change
    if ((rv = GSMSMS_EncodeAddress(&pMsg->DA, &SMS_Submit[idx], FALSE)) < 0)
    {
       FARF(ERROR, ("GSMSMS_EncodeAddress failed"));
       return AEE_GSMSMS_EENCODE;
    }
    idx+=(byte)rv;

    SMS_Submit[idx++] = pMsg->PID; //TP-PID 1 octet (SME to SME)
    
    // TP-DCS
    GSMSMS_EncodeDCS(&SMS_Submit[idx++], &pMsg->DCS);

    // TP-VP
    if (GSMSMS_EncodeVP(&pMsg->VP, &SMS_Submit[idx], &len) == FALSE)
    {
       return AEE_GSMSMS_EENCODE;
    }
    idx+=len;

    SMS_Submit[idx++] = pMsg->UDL;

    //TP-UD
    MEMCPY((void *)&SMS_Submit[idx], pMsg->UD, pMsg->UDL);

    idx+=pMsg->UDL;

    return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
FUNCTION: GSMSMS_SendSMSRaw

DESCRIPTION:
     Sends a IS-637 message with a tunnelled GSM1x message. Formats the IS-637 
     client message structure (including bearer data) and bit-packs the user
     data.
        
PARAMETERS:
    * IS637UserData [in] : The GSM RL/TL data message to send to send
    len        [in] : length of the message to send
    pfn        [in] : function to call for status report
    startTransaction [in] : to indicate if a new transaction is to be started


RETURN VALUE:
  AEE_GSMSMS_SUCCESS or AEE_GSMSMS_EFAILED

SIDE EFFECTS:
    None
=============================================================================*/
STATIC int GSMSMS_SendSMSRaw
(
    const byte* pIS637UserData, // IS-637 User Data
    uint8 len,
    uint8 callbackType,
    boolean startTransaction
)
{
    boolean statusCallbackNeeded = TRUE;

    FARF(UNITTEST, ("Called GSMSMS_SendSMSRaw"));

    if (pIS637UserData == NULL)
    {
       return AEE_GSMSMS_EBADPARAM;
    }

    if (!sGSMSMS.m_bInitialized)
    {
       return AEE_GSMSMS_EFAILED;
    }

    if (OEMGSM1xControl_GetStatus() != GSM1xSIG_ACTIVE)
    {
       return AEE_GSMSMS_ENOSERVICE;
    }

    if(len > GSM1xMESSAGE_SIGNALING_MAX )
    {
       FARF(ERROR, ("encoding: UD too big: %d", len));
       return AEE_GSMSMS_EFAILED;
    }

    FARF(UNITTEST, ("Calling OEMGSM1xMessage_SendMessage"));

    if (callbackType == USER_CB_NONE)
    {
       statusCallbackNeeded = FALSE;
    }

    // if we start a transaction we must terminate it
    if (OEMGSM1xMessage_SendSMSMessage (statusCallbackNeeded,
                                        (void*)callbackType, 
                                        len,
                                        pIS637UserData, 
                                        startTransaction) == SUCCESS)
    {
       return AEE_GSMSMS_SUCCESS;
    }
    else
       return AEE_GSMSMS_EFAILED;
} 


/*=============================================================================
FUNCTION: GSMSMS_DecodeSMSStatusReport

DESCRIPTION:
    Converts an incoming SMS Status Report message to a structure to pass to
    any applets registered to receive a NMASK_GSM1X_SMS_STATUS_RPT
        
PARAMETERS:
    pIDT              [in] : Pointer to intermediate data type 
    pSMSStatusReport [out] : Pointer to the structure to contain the decoded 
                             SMS Status Report

RETURN VALUE:
  AEE_GSMSMS_SUCCESS or AEE_GSMSMS_EFAILED

SIDE EFFECTS:
    None
=============================================================================*/
STATIC int GSMSMS_DecodeSMSStatusReport
(
    const byte * pData, 
    GSMSMSStatusReportType * pSMSStatusReport
)
{
   uint16 idx = 0;

   FARF(UNITTEST, ("Called GSMSMS_DecodeSMSStatusReport"));

   if (pData == NULL || pSMSStatusReport == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }
   
   // validate the message according to spec requirements
   if ((pData[idx] & 0x03) != GSMSMS_TL_SMS_STATUS_REPORT) 
   {
      FARF(ERROR, ("Message is not a StatusReport"));
      return AEE_GSMSMS_EFAILED;
   }

   pSMSStatusReport->MMS = pData[idx] & 0x04;
   pSMSStatusReport->SRQ = (pData[idx] & 0x08) ? TRUE : FALSE;
   pSMSStatusReport->UDHI = pData[idx++] & 0x10;

   pSMSStatusReport->MR = pData[idx++];

   // TP-RA
   if (pData[idx] > GSMSMS_MAX_ADDRESS_LEN)
   {
      return AEE_GSMSMS_EFAILED;
   }
   idx += GSMSMS_DecodeAddress(&pData[idx], &pSMSStatusReport->RA, FALSE);

   GSMSMS_DecodeTime(&pData[idx], &pSMSStatusReport->SCTS);
   idx+=7;
   GSMSMS_DecodeTime(&pData[idx], &pSMSStatusReport->DT);
   idx+=7;
   pSMSStatusReport->ST = pData[idx++];
   pSMSStatusReport->PID_present = (pData[idx] & 0x01) ? TRUE : FALSE;
   pSMSStatusReport->DCS_present = (pData[idx] & 0x02) ? TRUE : FALSE;
   pSMSStatusReport->UDL_present = (pData[idx++] & 0x04) ? TRUE : FALSE;
 
   if (pSMSStatusReport->PID_present)
   {
      pSMSStatusReport->PID = pData[idx++]; 
   }
   else
   {
      pSMSStatusReport->PID = 0;
   }
   
   if (pSMSStatusReport->DCS_present)
   {
      GSMSMS_DecodeDCS(pData[idx++], &pSMSStatusReport->DCS); 
   }
   else
       pSMSStatusReport->DCS.codingGroup = GSMSMS_DCS_CG_NOT_PRESENT;
   
   if (pSMSStatusReport->UDL_present)
   {
      pSMSStatusReport->UDL = pData[idx++]; 
      MEMCPY((void *)&pSMSStatusReport->UD[0], &pData[idx], pSMSStatusReport->UDL);
      idx+=pSMSStatusReport->UDL;
   }
   else
   {
      pSMSStatusReport->UDL = 0;
   }
    
   return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
FUNCTION: GSMSMS_DecodeSMSDeliverMsg

DESCRIPTION:
    Converts an incoming SMS Deliver message to a structure to pass to
    any applets registered to receive a NMASK_GSM1X_SMS_STATUS_RPT
        
PARAMETERS:
    pIDT          [in] : Pointer to intermediate data type 
    pSMSDeliver  [out] : Pointer to the structure to contain the decoded 
                         SMS Status Report

RETURN VALUE:
  AEE_GSMSMS_SUCCESS or AEE_GSMSMS_EFAILED

SIDE EFFECTS:
    None
=============================================================================*/
STATIC int GSMSMS_DecodeSMSDeliverMsg
(
    const byte * pData, 
    GSMSMSDeliverType * pSMSDeliver
)
{
    uint16 idx = 0;

    FARF(UNITTEST, ("Called GSMSMS_DecodeSMSDeliverMsg"));

    if (pData == NULL || pSMSDeliver == NULL)
       return AEE_GSMSMS_EBADPARAM;
    
    // TODO validate the message according to spec requirements
    if ((pData[idx] & 0x03) != GSMSMS_TL_SMS_DELIVER)
    {
        FARF(ERROR, ("Bad message format"));
        return AEE_GSMSMS_EFAILED;
    }

    pSMSDeliver->MMS =  (pData[idx] >> 2) & 0x01;
    pSMSDeliver->SRI =  (pData[idx] >> 5) & 0x01;
    pSMSDeliver->UDHI = (pData[idx] >> 6) & 0x01;
    pSMSDeliver->RP = (pData[idx++] >> 7) & 0x01;

    // TP-OA
    if (pData[idx] > GSMSMS_MAX_ADDRESS_LEN)
    {
       return AEE_GSMSMS_EFAILED;
    }
    idx += GSMSMS_DecodeAddress(&pData[idx], &pSMSDeliver->OA, FALSE);

    pSMSDeliver->PID = pData[idx++]; 
    GSMSMS_DecodeDCS(pData[idx++], &pSMSDeliver->DCS); 
    
    GSMSMS_DecodeTime(&pData[idx], &pSMSDeliver->SCTS);
    idx+=7;

    pSMSDeliver->UDL = pData[idx++]; 
    MEMCPY((void *)&pSMSDeliver->UD[0], &pData[idx], pSMSDeliver->UDL);

    idx+=pSMSDeliver->UDL;

    FARF(LOW, ("MMS=%d SRI=%d UDHI=%d", pSMSDeliver->MMS, pSMSDeliver->SRI, pSMSDeliver->UDHI));
    FARF(LOW, ("RP=%d  PID=%d ", pSMSDeliver->RP, pSMSDeliver->PID));
    FARF(LOW, ("UDL=%d", pSMSDeliver->UDL));

    return AEE_GSMSMS_SUCCESS;
}
/*=============================================================================
FUNCTION: GSMSMS_DecodeSMSSubmitMsg

DESCRIPTION:
    Converts an incoming SMS Deliver message to a structure to pass to
    any applets registered to receive a NMASK_GSM1X_SMS_STATUS_RPT
        
PARAMETERS:
    pIDT          [in] : Pointer to intermediate data type 
    pSMSDeliver  [out] : Pointer to the structure to contain the decoded 
                         SMS Status Report

RETURN VALUE:
  AEE_GSMSMS_SUCCESS or AEE_GSMSMS_EFAILED

SIDE EFFECTS:
    None
=============================================================================*/
STATIC int GSMSMS_DecodeSMSSubmitMsg
(
    const byte * pData, 
    GSMSMSSubmitType * pSMSSubmit
)
{
    uint16 idx = 0;

    FARF(UNITTEST, ("Called GSMSMS_DecodeSMSSubmitMsg"));

    if (pData == NULL || pSMSSubmit == NULL)
    {
       return AEE_GSMSMS_EBADPARAM;
    }
    
    // TODO validate the message according to spec requirements
    if ((pData[idx] & 0x03) != GSMSMS_TL_SMS_SUBMIT)
    {
        FARF(ERROR, ("Bad message format %d %d", pData[idx] & 0x03,GSMSMS_TL_SMS_SUBMIT));
        return AEE_GSMSMS_EFAILED;
    }

    pSMSSubmit->RD = (pData[idx] >> 2) & 0x01;
    pSMSSubmit->VP.format = (GSMSMSVPFormat)((pData[idx] >> 3) & 0x03);
    pSMSSubmit->SRR = (pData[idx] >> 5) & 0x01;
    pSMSSubmit->UDHI = (pData[idx] >> 6) & 0x01;
    pSMSSubmit->RP = (pData[idx++] >> 7) & 0x01;
    pSMSSubmit->MR = (pData[idx++] >> 7) & 0x01;

    // TP-OA
    if (pData[idx] > GSMSMS_MAX_ADDRESS_LEN)
    {
       return AEE_GSMSMS_EFAILED;
    }
    idx += GSMSMS_DecodeAddress(&pData[idx], &pSMSSubmit->DA, FALSE);

    pSMSSubmit->PID = pData[idx++]; 
    GSMSMS_DecodeDCS(pData[idx++], &pSMSSubmit->DCS); 
    
    switch (pSMSSubmit->VP.format)
    {
    case GSMSMS_VP_FORMAT_NOT_PRESENT:
       break;
    case GSMSMS_VP_FORMAT_ENHANCED:
    case GSMSMS_VP_FORMAT_ABSOLUTE:
    case GSMSMS_VP_FORMAT_RELATIVE:
       // TODO not supported ... yet
    default:
       pSMSSubmit->VP.format = GSMSMS_VP_FORMAT_NOT_PRESENT;
       return AEE_GSMSMS_EFAILED;
    }

    pSMSSubmit->UDL = pData[idx++]; 
    MEMCPY((void *)&pSMSSubmit->UD[0], &pData[idx], pSMSSubmit->UDL);

    idx+=pSMSSubmit->UDL;

    return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
FUNCTION: GSMSMS_DecodeSMSSubmitReportMsg

DESCRIPTION:
        
PARAMETERS:
    pIDT          [in] : Pointer to intermediate data type 
    pSMSSubmitReport  [out] : Pointer to the structure to contain the decoded 
                         SMS Status Report

RETURN VALUE:
  AEE_GSMSMS_SUCCESS or AEE_GSMSMS_EFAILED or AEE_GSMSMS_EBADPARAM

SIDE EFFECTS:
    None
=============================================================================*/
STATIC int GSMSMS_DecodeSMSSubmitReportMsg
(
    GSMSMSRawData const * pRawMsg, 
    GSMSMSSubmitReportType * pSMSSubmitReport
)
{
    byte const * pData; 
    byte idx=0, len, PI;

    FARF(UNITTEST, ("Called GSMSMS_DecodeSMSSubmitReportMsg"));

    if (pRawMsg == NULL || pSMSSubmitReport == NULL)
    {
       return AEE_GSMSMS_EBADPARAM;
    }

    pData = pRawMsg->data;

    if (sGSMSMS.m_SubmitRP_MR != pData[1])
    {
       FARF(ERROR, ("Received SubmitReport with invalid RP_MR"));
       return AEE_GSMSMS_EFAILED;
    }
    
    idx = 2; // Skip RP-MR

    if (pData[0] == GSMSMS_RP_MT_ERROR)
    {
       // skip Failure Cause
       len = pData[idx]+1;
       idx+=len;
       if (pData[idx] != GSMSMS_USER_DATA_IEI || pData[idx+1] == 0)
       {
          // No TPDU was sent
          pSMSSubmitReport->UDHI = 0;
          pSMSSubmitReport->FCS_present = TRUE;
          pSMSSubmitReport->FCS = 0xFF; // Unspecified error cause
          pSMSSubmitReport->PID_present = FALSE; 
          pSMSSubmitReport->DCS_present = FALSE; 
          pSMSSubmitReport->DCS.codingGroup = GSMSMS_DCS_CG_GENERAL_DATA;
          pSMSSubmitReport->DCS.msgCoding = GSMSMS_ENCODING_7BIT;
          pSMSSubmitReport->DCS.cg.generalData.msgClass = GSMSMS_MSG_CLASS_NONE;
          pSMSSubmitReport->DCS.cg.generalData.isCompressed = FALSE;
          pSMSSubmitReport->DCS.cg.generalData.autoDelete = FALSE;
          pSMSSubmitReport->UDL_present = FALSE; 

          return SUCCESS;
       }
    }
    else if (pData[0] == GSMSMS_RP_MT_ACK &&
            (pData[2] != GSMSMS_USER_DATA_IEI || pData[3] == 0))
    {
       // No TPDU was sent
       pSMSSubmitReport->UDHI = 0;
       pSMSSubmitReport->FCS_present = FALSE;
       pSMSSubmitReport->PID_present = FALSE; 
       pSMSSubmitReport->DCS.codingGroup = GSMSMS_DCS_CG_GENERAL_DATA;
       pSMSSubmitReport->DCS.msgCoding = GSMSMS_ENCODING_7BIT;
       pSMSSubmitReport->DCS.cg.generalData.msgClass = GSMSMS_MSG_CLASS_NONE;
       pSMSSubmitReport->DCS.cg.generalData.isCompressed = FALSE;
       pSMSSubmitReport->DCS.cg.generalData.autoDelete = FALSE;
       pSMSSubmitReport->DCS_present = FALSE; 
       pSMSSubmitReport->UDL_present = FALSE; 

       return SUCCESS;
    }

    if (pData[idx] != GSMSMS_USER_DATA_IEI) // USER-DATA IEI
    {
        FARF(ERROR, ("User Data IEI invalid"));
        return AEE_GSMSMS_EFAILED;
    }

    idx++; // skip USER-DATA IEI
    idx++; // skip USER-DATA length 

    // TODO validate the message according to spec requirements
    if ((pData[idx] & 0x03) != GSMSMS_TL_SMS_SUBMIT_REPORT)
    {
        FARF(ERROR, ("Bad message format"));
        return AEE_GSMSMS_EFAILED;
    }

    pSMSSubmitReport->UDHI = (pData[idx++] >> 2) & 0x01;

    if (pData[0] == GSMSMS_RP_MT_ERROR)
    {
       pSMSSubmitReport->FCS = pData[idx++]; 
       pSMSSubmitReport->FCS_present = TRUE;
    }
    else
    {
       pSMSSubmitReport->FCS_present = FALSE;
    }
  
    PI = pData[idx++];
    if (PI & 0x01)
    {
       pSMSSubmitReport->PID = pData[idx++]; 
       pSMSSubmitReport->PID_present = TRUE; 
    }
    else
    {
       pSMSSubmitReport->PID = 0;
       pSMSSubmitReport->PID_present = FALSE; 
    }

    if (PI & 0x02)
    {
       GSMSMS_DecodeDCS(pData[idx++], &pSMSSubmitReport->DCS); 
       pSMSSubmitReport->DCS_present = TRUE; 
    }
    else
    {
       pSMSSubmitReport->DCS.codingGroup = GSMSMS_DCS_CG_GENERAL_DATA;
       pSMSSubmitReport->DCS.msgCoding = GSMSMS_ENCODING_7BIT;
       pSMSSubmitReport->DCS.cg.generalData.msgClass = GSMSMS_MSG_CLASS_NONE;
       pSMSSubmitReport->DCS.cg.generalData.isCompressed = FALSE;
       pSMSSubmitReport->DCS.cg.generalData.autoDelete = FALSE;
       pSMSSubmitReport->DCS_present = FALSE; 
    }
    
    if (PI & 0x04)
    {
       pSMSSubmitReport->UDL = pData[idx++]; 
       pSMSSubmitReport->UDL_present = TRUE; 
       MEMCPY((void *)&pSMSSubmitReport->UD[0], &pData[idx], pSMSSubmitReport->UDL);
       idx+=pSMSSubmitReport->UDL;
    }
    else
    {
       pSMSSubmitReport->UDL = 0;
       pSMSSubmitReport->UDL_present = FALSE; 
    }

    return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
FUNCTION: GSMSMS_ProcessCache

DESCRIPTION:
    Process cache entries added from IPhone_MessageListener
        
PARAMETERS:
    dummy [in] : not used

RETURN VALUE:
  AEE_GSMSMS_SUCCESS or AEE_GSMSMS_EFAILED

SIDE EFFECTS:
  None
=============================================================================*/
static void GSMSMS_ProcessCache(void *dummy)
{
   GSMSMSMsg                 gsm;
   GSMSMSRawMsg              rawMsg;
   int                       decodeStatus;
   boolean                   messageFound;
   byte                      pMessageLen;

   struct _IGSMSMS         * pMe = &sGSMSMS;

   PARAM_NOT_REF(data);

   FARF(UNITTEST, ("Called GSMSMS_ProcessCache"));

   if ( !pMe->m_bInitialized )
   {
      return;
   } 
    
   // Process the Cache entries for SMS
   for (;;) 
   {
      messageFound = OEMGSMSMS_SVC_GetSMSMessage(&gSMSMessage, &pMessageLen);
      
      if (!messageFound)
         break;

      // decode the cache entry
      rawMsg.format = GSMSMS_RAW_FORMAT_GSM;
      decodeStatus = GSMSMS_DecodeBearerData(&gSMSMessage, pMessageLen, &rawMsg.msg.gsm);

      // Only continue if we have bearer data (text) or it's a voice mail message
      if( (decodeStatus != AEE_GSMSMS_SUCCESS) )
      {
         FARF(ERROR, ("GSMSMS message decode failed"));
         continue;
      }

      switch (rawMsg.msg.gsm.data[0])
      {
      case GSMSMS_RP_MT_DATA:
 
         if (OEMGSMSMS_DecodeMessage((struct _IGSMSMS *)pMe, 
                                     &rawMsg, &gsm) != AEE_GSMSMS_SUCCESS)
         {
            FARF(HIGH, ("MT message could not be decoded"));
            return;
         }

         if (gsm.msgType == GSMSMS_MSG_SMS_DELIVER)
         {
             boolean brew_directed;
             
             pMe->m_DeliverRP_MR = rawMsg.msg.gsm.data[1]; // RP_MR

             OEMGSMSMS_DecodeUserData((struct _IGSMSMS *)pMe,
                                      &gsm,
                                      gszMessageInUnicode,
                                      sizeof(gszMessageInUnicode));

             WSTRTOSTR(gszMessageInUnicode, gszMessageInBytes, 128);

             brew_directed = AEE_IsBREWDirectedSMS(gszMessageInBytes);

             if (brew_directed)
             {
                GSMSMSMsg* drpt = &gsm; // reuse stack memory that is not needed anymore

                // Message has been handled, send the ACK
                if (OEMGSMSMS_CreateDefaultMessage(pMe,
                                             GSMSMS_MSG_SMS_DELIVER_REPORT,
                                             drpt) != AEE_GSMSMS_SUCCESS)
                {
                   FARF(HIGH, ("OEMGSMSMS_CreateDefaultMessage failed"));
                }
                else if (OEMGSMSMS_SendSMSDeliverReport(pMe, 
                            &(drpt->msg.SMSDeliverReport)) != AEE_GSMSMS_SUCCESS)
                {
                   FARF(HIGH, ("OEMGSMSMS_SendSMSDeliverReport failed"));
                }
                return; // BREW Directed Msg
             }
         }
         else if (gsm.msgType != GSMSMS_MSG_SMS_STATUS_REPORT)
         {
            FARF(HIGH, ("No valid TP_MTI found"));
            return;
         }

         if (!(sGSMSMSNotifier.m_bInitialized && 
              (sGSMSMSNotifier.m_dwNotify & NMASK_GSMSMS_MT_MESSAGE))) {
            return;
         }

         // No context switch occurs here so the applets are all notified
         // and process the data in the current context so it is OK that
         // the data is on the stack
         // TODO fix this to unwind the stack
         (void)ISHELL_Notify(pMe->m_pIShell, AEECLSID_IGSMSMSNOTIFIER, 
                       NMASK_GSMSMS_MT_MESSAGE, &rawMsg);

         break;

      case GSMSMS_RP_MT_ACK:
      case GSMSMS_RP_MT_ERROR:

         if (pMe->m_pCallback == NULL)
         {
            FARF(HIGH, ("Received spurious SMS_SUBMIT_REPORT"));
            return;
         }
         // Match the RP_MR
         else if (pMe->m_SubmitRP_MR != rawMsg.msg.gsm.data[1])
         {
            FARF(MED, ("Received SMS_SUBMIT_REPORT has mismatched RP-MR %d %d",
               pMe->m_SubmitRP_MR, rawMsg.msg.gsm.data[1]));
            return;
         }
         else if (pMe->m_bSMMAPending)
         {
            GSMSMSSMMAReport *pReport = 
               (GSMSMSSMMAReport *)pMe->m_pReport;

            if (rawMsg.msg.gsm.data[0] == GSMSMS_RP_MT_ERROR)
            {
               pReport->rpError = TRUE;
               pReport->rpCause = rawMsg.msg.gsm.data[3];
            }
            else
            {
               pReport->rpError = FALSE;
               pReport->rpCause = 0;
            }
            pMe->m_bSMMAPending = FALSE;
            (void)ISHELL_CancelTimer(pMe->m_pIShell, 
                                     GSMSMS_SMMATimerCallback, 
                                     (void *)pMe); 

            // This will cancel the Callback
            ISHELL_Resume(pMe->m_pIShell,pMe->m_pCallback);
            return;
         }
         else if (OEMGSMSMS_DecodeMessage((struct _IGSMSMS *)pMe, 
                                          &rawMsg, &gsm) != AEE_GSMSMS_SUCCESS)
         {
            FARF(ERROR, ("MT_ACK/ERROR message could not be decoded"));
            return;
         }

         if (gsm.msgType == GSMSMS_MSG_SMS_SUBMIT_REPORT)
         {
            // Call the SMS_Submit Callback (one deep queue)
            GSMSMSSendReport *pSendReport = 
               (GSMSMSSendReport *)pMe->m_pReport;

            pSendReport->sendResult = AEE_GSMSMS_SUCCESS;

            MEMCPY(&pSendReport->reportMsg, 
                   &rawMsg, 
                   sizeof(pSendReport->reportMsg));

            (void)ISHELL_CancelTimer(pMe->m_pIShell, 
                                     GSMSMS_TimerCallback, 
                                     (void *)pMe); 
                   
            // This will cancel the Callback
            ISHELL_Resume(pMe->m_pIShell,pMe->m_pCallback);
            return;
         }
         FARF(ERROR, ("No valid TP-MTI found"));
         return;

      default:
         FARF(ERROR, ("No valid RP-MTI found"));
         return;
      }

      return; // For loop is used as a GOTO to go to the end on error
   }
}

/*=============================================================================
FUNCTION: GSMSMS_DecodeBearerData 

DESCRIPTION:
    Convert the raw bearer data into an intermediate data type
        
PARAMETERS:
    packedMessage [in]: pointer to the raw bearer data
    packedMessageLen [in] : raw bearer data length
    rawGsm  [out]: pointer to the intermediate data structure to hold the 
                   decoded data

RETURN VALUE:
    none

SIDE EFFECTS:
    None
=============================================================================*/
STATIC int GSMSMS_DecodeBearerData
(
   GSMSMSMessage *packedMessage,
   byte           packedMessageLen,
   GSMSMSRawData *pRawGsm
)
{
   FARF(LOW, ("Called GSMSMS_DecodeBearerData"));

   if (packedMessage == NULL || pRawGsm == NULL)
   {
      return AEE_GSMSMS_EBADPARAM;
   }

   // Copy to the actual user data
   MEMSET((void *)pRawGsm->data, 0xff, sizeof(pRawGsm->data));

   // Assertion
   if (sizeof(pRawGsm->data) < packedMessageLen)
   {
      FARF(HIGH, ("ASSERT FAILED (%d) (%d)",sizeof(pRawGsm->data), packedMessageLen));
      return AEE_GSMSMS_EFAILED;
   }

   MEMCPY((void *)pRawGsm->data, packedMessage, packedMessageLen);
   
   return AEE_GSMSMS_SUCCESS;
}

/*=============================================================================
FUNCTION: GSMSMS_WSTRTo7BitGSM 

DESCRIPTION:
    Convert the unicode string to 7bit GSM 
        
PARAMETERS:
    pwzStr  : pointer to the unicode text
    alpha  : pointer to the buffer to receive the GSM encoded data
    nAlphaSize : maximum size of the alpha buffer in bytes

RETURN VALUE:
    none

SIDE EFFECTS:
    None
=============================================================================*/
STATIC byte GSMSMS_WSTRTo7BitGSM(AECHAR const *pwzStr, byte *pAlpha, byte nAlphaSize)
{
   byte len=0;
   byte ch;
   boolean isExtended;

   if (pAlpha == NULL)
   {
      FARF(HIGH, ("NULL param in GSMSMS_WSTRTo7BitGSM"));
      return 0;
   }

   MEMSET((void *)pAlpha, 0xFF, nAlphaSize);

   if (pwzStr == NULL)
   {
      return 0;
   }

   while (*pwzStr != 0x0000 && len < nAlphaSize)
   {
      if (GSMSMS_UnicodeToGSM(*pwzStr, &ch, &isExtended) == SUCCESS)
      {
         if (isExtended == TRUE)
         {
            if (len+1 < nAlphaSize)
            {
               pAlpha[len++] = 0x1B;
            }
            else
            {
               break;
            }
         }
         pAlpha[len++] = ch;
      }
      else // No GSM 7-bit equivalent
      {
         pAlpha[len++] = ' '; // SPACE is the same in GSM and UTF-8
      }
      pwzStr++;
   }
   return len;
} 

/*=============================================================================
FUNCTION: GSMSMS_GSMToWSTR 

DESCRIPTION:
    Convert the 7bit GSM text to unicode
        
PARAMETERS:
    pszAlpha   : pointer to the buffer containing the GSM encoded data
    nAlphaSize : maximum size of the alpha buffer in bytes
    pwzStr     : pointer to the buffer to receive the unicode text
    nWStrSize  : maximum size of the wStr buffer in characters

RETURN VALUE:
    none

SIDE EFFECTS:
    None
=============================================================================*/
STATIC void GSMSMS_GSMToWSTR(byte const *pszAlpha,
                             uint32      nAlphaSize,
                             AECHAR     *pwzStr,
                             uint32      nWStrSize)
{
   uint32  i, j, numExt, maxlen = nWStrSize;

   FARF(LOW, ("Called GSMSMS_GSMToWSTR"));

   if (pwzStr == NULL || pszAlpha == NULL)
   {
      FARF(ERROR, ("Internal Error! parameter is NULL"));
      return;
   }

   if (nAlphaSize == 0) {
      pwzStr[0] = (AECHAR)'\0';
      return;
   }

   MEMSET((void *)pwzStr, 0, nWStrSize);
   numExt=0;
   for (i = 0; i < MIN(nAlphaSize,maxlen+numExt); i++, pwzStr++) 
   {
       if (pszAlpha[i] > 127)
       {
          return;
       }
       *pwzStr = gsmToUTF16[pszAlpha[i]];
       if (0x001B == pszAlpha[i]) 
       {
            i++;

            // NOTE: If the character isn't found in the Extension Table,
            //       just keep the ESC character
            for (j = 0; j < sizeof(gsmExtTable)/sizeof(gsmExtTable[0]); j++ ) 
            {
               if (gsmExtTable[j].gsmChar == (char)pszAlpha[i]) 
               {
                  *pwzStr = gsmExtTable[j].unicodeChar;
                  numExt++;
                  break;
               }
            }
       }
   }
} 

/*=============================================================================
FUNCTION: GSMSMS_EncodeTime

DESCRIPTION:
     Converts a GSM time format (SCTS,DT) to a struct
        
PARAMETERS:
      rawtime  [in] : pointer to the raw GSM format as transmitted
      time    [out] : pointer to the structure to contain the decoded time

RETURN VALUE:
  none

SIDE EFFECTS:
    None
=============================================================================*/
STATIC void GSMSMS_EncodeTime(GSMSMSTime const *pTime, byte *pszRawtime)
{
    uint8 utmp;
    
    if (pszRawtime == NULL || pTime == NULL)
    {
       return;
    }

    FARF(LOW, ("Called GSMSMS_EncodeTime"));

    utmp = pTime->year % 100;

    pszRawtime[0] = ((utmp          % 10) << 4) | (utmp / 10);
    pszRawtime[1] = ((pTime->month  % 10) << 4) | (pTime->month  / 10);
    pszRawtime[2] = ((pTime->day    % 10) << 4) | (pTime->day    / 10);
    pszRawtime[3] = ((pTime->hour   % 10) << 4) | (pTime->hour   / 10);
    pszRawtime[4] = ((pTime->minute % 10) << 4) | (pTime->minute / 10);
    pszRawtime[5] = ((pTime->second % 10) << 4) | (pTime->second / 10);

    if (pTime->tzoffset < 0)
    {
       pszRawtime[6] = 0x08;
       utmp = (uint8)((-pTime->tzoffset) / 15);
    }
    else
    {
       utmp = (uint8)(pTime->tzoffset / 15);
    }

    pszRawtime[6] |= ((utmp % 10) << 4) | (utmp / 10);
}

/*=============================================================================
FUNCTION: GSMSMS_DecodeTime

DESCRIPTION:
     Converts a GSM time format (SCTS,DT) to a struct
        
PARAMETERS:
      rawtime  [in] : pointer to the raw GSM format as transmitted
      time    [out] : pointer to the structure to contain the decoded time

RETURN VALUE:
  none

SIDE EFFECTS:
    None
=============================================================================*/
STATIC void GSMSMS_DecodeTime(byte const *pszRawTime, GSMSMSTime *pTime)
{
    FARF(LOW, ("Called GSMSMS_DecodeTime"));

    if (pszRawTime == NULL || pTime == NULL)
    {
       return;
    }

    if (((pszRawTime[0] >> 4) > 9) || ((pszRawTime[0] & 0x0F) > 9))
    {
       FARF(HIGH, ("Invalid year field in GSM time"));
       pTime->year = 0;
    }
    else
    {
       pTime->year = 2000 + BCD2INT(pszRawTime[0] >> 4 | pszRawTime[0] << 4);
    }
    if (((pszRawTime[1] >> 4) > 9) || ((pszRawTime[1] & 0x0F) > 9))
    {
       FARF(HIGH, ("Invalid month field in GSM time"));
       pTime->month = 0;
    }
    else
    {
       pTime->month = BCD2INT(pszRawTime[1] >> 4 | pszRawTime[1] << 4);
    }
    if (((pszRawTime[2] >> 4) > 9) || ((pszRawTime[2] & 0x0F) > 9))
    {
       FARF(HIGH, ("Invalid day field in GSM time"));
       pTime->day = 0;
    }
    else
    {
       pTime->day = BCD2INT(pszRawTime[2] >> 4 | pszRawTime[2] << 4);
    }
    if (((pszRawTime[0] >> 4) > 9) || ((pszRawTime[0] & 0x0F) > 9))
    {
       FARF(HIGH, ("Invalid hour field in GSM time"));
       pTime->hour = 0;
    }
    else
    {
       pTime->hour = BCD2INT(pszRawTime[3] >> 4 | pszRawTime[3] << 4);
    }
    if (((pszRawTime[4] >> 4) > 9) || ((pszRawTime[4] & 0x0F) > 9))
    {
       FARF(HIGH, ("Invalid minute field in GSM time"));
       pTime->minute = 0;
    }
    else
    {
       pTime->minute = BCD2INT(pszRawTime[4] >> 4 | pszRawTime[4] << 4);
    }
    if (((pszRawTime[5] >> 4) > 9) || ((pszRawTime[5] & 0x0F) > 9))
    {
       FARF(HIGH, ("Invalid second field in GSM time"));
       pTime->second = 0;
    }
    else
    {
       pTime->second = BCD2INT(pszRawTime[5] >> 4 | pszRawTime[5] << 4);
    }

    // account for timezone offset in minutes
    if (((pszRawTime[6] >> 4) > 9) || ((pszRawTime[6] & 0x0F) > 9))
    {
       FARF(HIGH, ("Invalid timezone offset field in GSM time"));
       pTime->tzoffset = 0;
    }
    else
    {
       pTime->tzoffset = BCD2INT((pszRawTime[6] >> 4) | ((pszRawTime[6] & 0x07) << 4))*15; 
       if (pszRawTime[6] & 0x08)
       {
           pTime->tzoffset = -pTime->tzoffset;
       }
    }
}

/*=============================================================================
FUNCTION: GSMSMS_DecodeAddress

DESCRIPTION:
     Converts GSM address to a unicode string
        
PARAMETERS:
      pData   [in] : Pointer to the raw GSM address field
      waddr  [out] : Pointer to the unicode string to contain the decoded 
                     address. International numbers are indicated with a
                     leading '+'

RETURN VALUE:
  The length in bytes of the address field is returned

SIDE EFFECTS:
  none
=============================================================================*/
STATIC uint8 GSMSMS_DecodeAddress(byte const *pData, GSMSMSAddress *pGsmAddr, boolean bIsSCAddr)
{
    // TP-OA
    uint16 i, digit, len, idx = 0;
    byte tmp;

    FARF(LOW, ("Called GSMSMS_DecodeAddress"));

    if (pGsmAddr == NULL || pData == NULL)
    {
       return 0;
    }

    // if bIsSCAddr == TRUE, len is num of bytes
    // otherwise len is number of digits
    len = pData[0];

    pGsmAddr->npi = (GSMSMSNpiType)(pData[1] & 0x0f);
    pGsmAddr->ton = (GSMSMSTonType)((pData[1] >> 4) & 0x07);

    idx = 2;

    if (bIsSCAddr)
    {
       // convert bytes to digits
       len=(uint8)((len-1)*2);
    }

    // convert to ASCII
    for(i=0, digit=0; i < len; i+=2)
    {  
        pGsmAddr->addr[digit++] = (pData[idx] & 0x0F) +'0';
        tmp = (pData[idx++] >> 4);
        if (((i+1) < len) && tmp <= 0x09)
        {
           pGsmAddr->addr[digit++] = tmp +'0';
        }
    }
    pGsmAddr->addr[digit] = '\0'; // NULL terminate

    return ((uint8)((len+1)/2)+2);
}

/*=============================================================================
FUNCTION: GSMSMS_EncodeDCS

DESCRIPTION:
     Converts a unicode string to GSM user data with the specified encoding
        
PARAMETERS:
      out [out] : Pointer to the GSM DCS byte to set
      dcs  [in] : Pointer to the DCS structure to encode

RETURN VALUE:
  none

SIDE EFFECTS:
  none
=============================================================================*/
STATIC void GSMSMS_EncodeDCS(byte *pszOut, const GSMSMSDCS *pDCS)
{
   *pszOut = 0;

   FARF(LOW, ("Called GSMSMS_EncodeDCS"));

   if (pszOut == NULL || pDCS == NULL)
   {
      return;
   }

   switch (pDCS->codingGroup)
   {
   case GSMSMS_DCS_CG_GENERAL_DATA:
      if (pDCS->cg.generalData.msgClass != GSMSMS_MSG_CLASS_NONE)
      {
         *pszOut |= (1 << 4) | pDCS->cg.generalData.msgClass;
         *pszOut |= pDCS->msgCoding; // already shifted
         *pszOut = (byte)(pDCS->cg.generalData.isCompressed << 5);
      }
      if (pDCS->cg.generalData.autoDelete)
      {
         *pszOut |= 0x40;
      }
      break;
   default:
      // nothing else supported yet
      *pszOut=0;
   }
}

/*=============================================================================
FUNCTION: GSMSMS_DecodeDCS

DESCRIPTION:
     Converts GSM user data to unicode
        
PARAMETERS:
      rawdcs   [in] : Pointer to the raw data coding scheme
      dcs     [out] : Pointer to the structure to contain the decoded data 
                      coding scheme 

RETURN VALUE:
  none

SIDE EFFECTS:
  none
=============================================================================*/
STATIC void GSMSMS_DecodeDCS(byte RawDCS, GSMSMSDCS *pDCS)
{
   FARF(LOW, ("Called GSMSMS_DecodeDCS"));

   if (pDCS == NULL)
   {
      return;
   }

   // check general coding group and auto deletion
   if ((RawDCS & 0x80) == 0)
   {
       pDCS->codingGroup = GSMSMS_DCS_CG_GENERAL_DATA;
       pDCS->cg.generalData.isCompressed = (RawDCS >> 5) & 0x01;
       if ((RawDCS & 0x10) == 0x10)
       {
          pDCS->cg.generalData.msgClass = (GSMSMSMsgClassType)(RawDCS & 0x03);
       }
       else
       {
          pDCS->cg.generalData.msgClass = GSMSMS_MSG_CLASS_NONE;
       }
      
       if ((RawDCS & 0xc0) == 0x40)
       {
          pDCS->cg.generalData.autoDelete = TRUE;
       }
       else
       {
          pDCS->cg.generalData.autoDelete = FALSE;
       }

       pDCS->msgCoding = (GSMSMSEncodingType)((RawDCS >> 2) & 0x03);
   }
   // check for message waiting indication
   else if (((RawDCS & 0xF0) >= 0xC0))
   {
       switch (RawDCS & 0x30) {
       case 0x00:
          pDCS->codingGroup = GSMSMS_DCS_CG_MESSAGE_WAITING;
          pDCS->cg.messageWaiting.indicationType = 
            (GSMSMSMsgWaitingIndType)(RawDCS & 0x03);
          pDCS->cg.messageWaiting.setIndication = (RawDCS >> 3) & 0x01;
          pDCS->cg.messageWaiting.store = FALSE;
          pDCS->msgCoding = GSMSMS_ENCODING_7BIT;
          break;
       case 0x10:
          pDCS->codingGroup = GSMSMS_DCS_CG_MESSAGE_WAITING;
          pDCS->cg.messageWaiting.indicationType= 
            (GSMSMSMsgWaitingIndType)(RawDCS & 0x03);
          pDCS->cg.messageWaiting.setIndication = (RawDCS >> 3) & 0x01;
          pDCS->cg.messageWaiting.store = TRUE;
          pDCS->msgCoding = GSMSMS_ENCODING_7BIT;
          break;
       case 0x20:
          pDCS->codingGroup = GSMSMS_DCS_CG_MESSAGE_WAITING;
          pDCS->cg.messageWaiting.indicationType= 
            (GSMSMSMsgWaitingIndType)(RawDCS & 0x03);
          pDCS->cg.messageWaiting.setIndication = (RawDCS >> 3) & 0x01;
          pDCS->cg.messageWaiting.store = TRUE;
          pDCS->msgCoding = GSMSMS_ENCODING_UCS2;
          break;
       case 0x30:
          pDCS->codingGroup = GSMSMS_DCS_CG_GENERAL_DATA;
          pDCS->cg.generalData.isCompressed = FALSE;
          pDCS->cg.generalData.msgClass = (GSMSMSMsgClassType)(RawDCS & 0x03);
          pDCS->msgCoding = ((RawDCS >> 2) & 0x01) ? GSMSMS_ENCODING_8BIT : GSMSMS_ENCODING_7BIT;
          break;
       default: // lint wants a default case
          break; 
       }
   }
   else
   {
      pDCS->codingGroup = GSMSMS_DCS_CG_RESERVED;
      FARF(HIGH, ("RESERVED DCS ENCODING!!!"));
   }
}

/*=============================================================================
FUNCTION: GSMSMS_EncodeVP

DESCRIPTION:
  Encodes the GSM validity period
        
PARAMETERS:
  pVP  [in] : Pointer to the validity period structure 
  raw [out] : Pointer to the buffer to receive the encoded validity period
  len [out] : Pointer to the variable to set to the length of the encoded
              validity period

RETURN VALUE:
 TRUE if successful, otherwise FALSE

SIDE EFFECTS:
  none
=============================================================================*/
STATIC boolean GSMSMS_EncodeVP(GSMSMSVP const *pVP, byte *pszRaw, byte *pcLen)
{
   uint32 ltime;

   FARF(LOW, ("Called GSMSMS_EncodeVP"));

   if (pVP == NULL || pszRaw == NULL || pcLen == NULL)
   {
      return FALSE;
   }

   // Encode the validity period;
   switch (pVP->format)
   {
   case GSMSMS_VP_FORMAT_NOT_PRESENT:
      *pcLen = 0;
      break;

   case GSMSMS_VP_FORMAT_RELATIVE:
      ltime = pVP->vp.relative.weeks*7*24*60+pVP->vp.relative.days*24*60+
             pVP->vp.relative.hours*60+pVP->vp.relative.minutes;

      if (ltime > 58*7*24*60)
      {
         return FALSE; 
      }
      else if (ltime > 28*24*60)
      {
         // time in weeks
         *pszRaw = (byte)(ltime/(7*24*60))+192;
      }
      else if (ltime > 24*60)
      {
         // time in days
         *pszRaw = (byte)(ltime/(24*60))+166;
      }
      else if (ltime > 12*60)
      {
         // time in 1/2 hours
         *pszRaw = (byte)(ltime/(30))+143;
      }
      else
      {
         // time in 5 min intervals
         *pszRaw = (byte)(ltime/(5));
      }

      *pcLen =1;
      break;

   case GSMSMS_VP_FORMAT_ABSOLUTE:
      GSMSMS_EncodeTime(&(pVP->vp.absolute.time), pszRaw);
      break;

   case GSMSMS_VP_FORMAT_ENHANCED:
      FARF(HIGH, ("Enhanced VP format not suported"));
      return FALSE;

   default:
      FARF(HIGH, ("Unhandled case in GSMSMS_EncodeVP"));
      return FALSE;
   }
   return TRUE;
}

/*=============================================================================
FUNCTION: GSMSMS_EncodeAddress

DESCRIPTION:
     Converts a character string to a GSM address
        
PARAMETERS:
      addr   [in] : Pointer to the string containing the address. 
                    International numbers are indicated with a leading '+'
      pData [out] : Pointer to the start of the raw GSM address field

RETURN VALUE:
  GSMSMS_ERROR_IND on failure, otherwise the length in bytes of the GSM 
  address field is returned

SIDE EFFECTS:
  none
=============================================================================*/
STATIC int GSMSMS_EncodeAddress(GSMSMSAddress const *pGsmAddr, byte *pData, boolean bIsSCAddr)
{
    // TP-OA
    uint8 i, j, len;
    byte const *pstart;
    byte d;

    FARF(LOW, ("Called GSMSMS_EncodeAddress %s",(bIsSCAddr == TRUE) ? "(SC Addr)" : ""));

    if (pGsmAddr == NULL || pData == NULL)
    {
       FARF(ERROR, ("NULL parameter"));
       return (GSMSMS_ERROR_IND);
    }

    // len
    len = (uint8)strlen((char *)pGsmAddr->addr);

    if (len > GSMSMS_MAX_ADDRESS_LEN)
    {
       FARF(ERROR, ("Address length too long (%d)",len));
       return (GSMSMS_ERROR_IND);
    }

    if (len == 0)
    {
       FARF(ERROR, ("Address length is zero"));
       return (GSMSMS_ERROR_IND);
    }

    // Sent the length field appropriately
    if (bIsSCAddr)
    {
       pData[0] = 1+(len+1)/2;
    }
    else
    {
       pData[0] = len;
    }

    // validate TON/NPI
    if (pGsmAddr->ton <= 7 && (pGsmAddr->npi <=10 && pGsmAddr->npi != 2 && pGsmAddr->npi != 7))  /*lint !e685 */
    {
        // npi:   ISDN | reserved | TON
        pData[1] = (pGsmAddr->ton << 4) | pGsmAddr->npi | 0x80;
    }
    else
    {
       FARF(ERROR, ("Invalid TON/NPI (%d, %d)",pGsmAddr->ton, pGsmAddr->npi));
       return (GSMSMS_ERROR_IND);
    }

    // convert to order swapped DTMF
    pstart = pGsmAddr->addr;
    pData[2] = 0xff;
    for (i = 0,j = 2; i < len; i++) 
    {
        if ((pstart[i] >= '0') && (pstart[i] <= '9')) 
        {
            d = (byte)(pstart[i] - '0');
            pData[j] &= 0x0f << (((i+1) % 2)*4);  // mask off bits to set
            pData[j] |= d << ((i % 2)*4);  // change each ASCII digit to DTMF 
            //FARF(HIGH("SendSMSRaw: DestAddr[%d] = %c (%x)",i,pstart[i],pData[j]));
            if (i % 2)
            {
              pData[++j] = 0xff;
            }
        }
    }

    return ((len+1)/2+2);
}

/*=============================================================================
FUNCTION: GSMSMS_GetFreeSlot

DESCRIPTION:
     Finds a free slot on the store
        
PARAMETERS:
      index [out] : the selected slot
      storeTo [in] : the message store to use

RETURN VALUE:
  AEE_GSMSMS_SUCCESS if a free slot was found, 
  AEE_GSMSMS_EBUSY if SMS handler is not finished initializing,
  otherwise AEE_GSMSMS_EFAILED

SIDE EFFECTS:
  none
=============================================================================*/
STATIC int GSMSMS_GetFreeSlot(struct _IGSMSMS *pMe, uint16 *pIndex, GSMSMSStorageType storeTo)
{
    uint16 i;

    FARF(LOW, ("Called GSMSMS_GetFreeSlot"));

    if (pIndex == NULL)
    {
       FARF(ERROR, ("NULL parameter"));
       return AEE_GSMSMS_EBADPARAM;
    }

    switch (storeTo)
    {
    case GSMSMS_STORE_SIM:
      {
        uint16 SMSCount;
        uint8 status;
 
        if (!pMe->m_bSIMSMSEnabled)
        {
           return AEE_GSMSMS_ESST;
        }

        if (OEMGSM1xCardHandler_IsSMSInit() == FALSE)
        {
           return AEE_GSMSMS_EBUSY;
        }
 
        SMSCount = OEMGSM1xCardHandler_GetNumSMSSlots();
        for (i=0; i<SMSCount; i++)
        {
           if (OEMGSM1xCardHandler_GetSMSStatus((uint8)i, &status) != UIM_PASS)
           {
              return AEE_GSMSMS_EFAILED;
           }
           if (status == OEMGSM1xCardHandler_MSG_UNUSED)
           {
              *pIndex = i;
              return AEE_GSMSMS_SUCCESS;
           }
        }
        break;
      }
    case GSMSMS_STORE_NVRAM:
    case GSMSMS_STORE_NVRAM_VM:
        {
           IFile * pIFile = NULL;
           byte status;

           if ((pIFile = GSMSMS_OpenDB(pMe, storeTo)) == NULL) 
           {
              FARF(ERROR, ("Database Open failed"));
              return AEE_GSMSMS_EFAILED;
           }

           for (i=0; i<GSMSMS_NUM_NV_ENTRIES; i++)
           {
              IFILE_Seek(pIFile, _SEEK_START, i*GSMSMS_NV_MBOX_RECORD_LEN+1);
              if (IFILE_Read(pIFile, &status, 1) != 1)
              {
                 FARF(ERROR, ("Read from MBOX failed"));
                 return AEE_GSMSMS_EFAILED;
              }
              if (status == GSMSMS_STATUS_NONE)
              {
                 *pIndex = i;
                 return AEE_GSMSMS_SUCCESS;
              }
           }
        }
        break;

    default:
        break;
    }
  
    return AEE_GSMSMS_EFAILED;
}

/*=============================================================================
FUNCTION: GSMSMS_DecodeMessageStatus

DESCRIPTION:
     Verifies that the status byte is valid, otherwise it marks the slot as free.
        
PARAMETERS:
      rawStatus [in] : status data dead from the slot
      status [out] : pointer to variable to set to the slot status

RETURN VALUE:
  none

SIDE EFFECTS:
  none
=============================================================================*/
STATIC void GSMSMS_DecodeMessageStatus(byte rawStatus, GSMSMSStatusType *pStatus)
{
   FARF(LOW, ("Called GSMSMS_DecodeMessageStatus"));

   if (pStatus == NULL)
   {
      FARF(ERROR, ("NULL parameter"));
      return;
   }

   switch (rawStatus)
   {
   case GSMSMS_STATUS_NONE:
   case GSMSMS_STATUS_MT_READ:
   case GSMSMS_STATUS_MT_NOT_READ:
   case GSMSMS_STATUS_MO_SENT:
   case GSMSMS_STATUS_MO_NOT_SENT:
   case GSMSMS_STATUS_MO_SENT_ST_NOT_RECEIVED:
   case GSMSMS_STATUS_MO_SENT_ST_NOT_STORED:
   case GSMSMS_STATUS_MO_SENT_ST_STORED:
      *pStatus = (GSMSMSStatusType)rawStatus;
      break;
   default:
      *pStatus = GSMSMS_STATUS_NONE;
   }
}


/*=============================================================================
FUNCTION: GSMSMS_StatusCallback

DESCRIPTION:
  Called to indicate the success or failure of the GSMSMS_SendSMSRaw call.
  If the call was successful, we will just keep waiting for the response
  message (SMS_SubmitReport).
        
PARAMETERS:
  userData [in] : callback type
  status [in]  : pointer to variable indicating the send status

RETURN VALUE:
  none

SIDE EFFECTS:
  none
=============================================================================*/
void GSMSMS_StatusCallback(void *userData, int status)
{
   uint32 callbackType = (uint32)userData;

   if (sGSMSMS.m_bInitialized && sGSMSMS.m_pCallback != NULL)
   {
      switch (callbackType)
      {
       case USER_CB_IPHONE:
          GSMSMS_IPhoneCb(status);
          break;

       case USER_CB_SMMA:
          GSMSMS_SMMACb(status);
          break;

       default:
          FARF(ERROR, ("Bad userData value in statusCallback"));
      }
   }
}

/*=============================================================================
FUNCTION: GSMSMS_IPhoneCb

DESCRIPTION:
  Called to indicate the success or failure of the GSMSMS_SendSMSRaw call.
  If the call was successful, we will just keep waiting for the response
  message (SMS_SubmitReport).
        
PARAMETERS:
  status [in]  : pointer to variable indicating the send status

RETURN VALUE:
  none

SIDE EFFECTS:
  none
=============================================================================*/
void GSMSMS_IPhoneCb(int status)
{
   if (status == EFAILED)
   {
      (void)ISHELL_CancelTimer(sGSMSMS.m_pIShell, GSMSMS_TimerCallback, &sGSMSMS); 

      // Create error message if failed
      GSMSMS_TimerCallback(NULL);
   }
}

/*=============================================================================
FUNCTION: GSMSMS_SMMACb

DESCRIPTION:
  Called to indicate the success or failure of the 
  OEMGSMSMS_SendMoreMemoryAvailable call.

  If the call was successful, we will just keep waiting for the response
  message (RP_ACK).
        
PARAMETERS:
  status [in]  : pointer to variable indicating the send status

RETURN VALUE:
  none

SIDE EFFECTS:
  none
=============================================================================*/
void GSMSMS_SMMACb(int status)
{
   if (status == EFAILED)
   {
      (void)ISHELL_CancelTimer(sGSMSMS.m_pIShell, GSMSMS_SMMATimerCallback, &sGSMSMS); 

      // Create error message if failed
      GSMSMS_SMMATimerCallback(NULL);
   }
}

/*=============================================================================
FUNCTION: GSMSMS_CbAdapterForTAPI

DESCRIPTION:
  Called when the SMSSubmitReport arrives to notify the app that originated
  the BREW directed messages via ITAPI_SendSMS.

PARAMETERS:
   pCbData [in]: Pointer to the callback data

RETURN VALUE:
  none

SIDE EFFECTS:
  none
=============================================================================*/
static void GSMSMS_CbAdapterForTAPI(void * pCbData)
{
   int status = EFAILED;
   GSMSMSMsg gsm;

   struct _IGSMSMS *pMe = &sGSMSMS;
   PFNSMSSTATUS pfn;

   if (pMe->m_bInitialized != TRUE) 
   {
      return;
   }

   if (sGSMSMS.m_TAPIpfn != NULL)
   {
      if ((pMe->m_TAPIReport.sendResult != AEE_GSMSMS_SUCCESS) || 
         (OEMGSMSMS_DecodeMessage(pMe, &pMe->m_TAPIReport.reportMsg, &gsm) != SUCCESS))
      {
         status = EFAILED;
      }
      else if (gsm.msgType == GSMSMS_MSG_SMS_SUBMIT_REPORT)
      {
         if (gsm.msg.SMSSubmitReport.FCS_present)
         {
            status = EFAILED;
         }
         else
         {
            status = SUCCESS;
         }
      }
      pfn = pMe->m_TAPIpfn;
      pfn(pMe->m_TAPICallback.pNotifyData, status);
      MEMSET(&pMe->m_TAPICallback,0,sizeof(pMe->m_TAPICallback));
   }
}

/*=============================================================================
FUNCTION: GSMSMS_CancelCb

DESCRIPTION:
  Called when the current SMS Submit transaction is cancelled.

PARAMETERS:
   pCb [in] : Pointer to the cancelled callback

RETURN VALUE:
  none

SIDE EFFECTS:
  none
=============================================================================*/
static void GSMSMS_CancelCb(AEECallback *pCb)
{
   struct _IGSMSMS *pMe = (struct _IGSMSMS *)pCb->pCancelData;

  //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pCb->pfnCancel != GSMSMS_CancelCb)
      return;
   
   // Do this just incase the cancel was caused by the App going away
   (void)ISHELL_CancelTimer(pMe->m_pIShell, NULL, (void *)1); 
                   
   // If the callback was active, the transaction should be pending
   // Terminate the Transaction
   OEMGSM1xMessage_ReleaseSMSTransaction();

   pCb->pfnCancel = NULL;
   pCb->pCancelData = NULL;

   pMe->m_pReport = NULL;
   pMe->m_pCallback = NULL;
}

/*=============================================================================
FUNCTION: CalcUDLengthInBytes

DESCRIPTION:
  Helper function to calcullate the number of bytes of the user data (UD) 
  including the user data header, and considering the encoding used for 
  the text

PARAMETERS:
   UDL [in] : The user data length (in characters)
   DCS [in] : The GSM data coding scheme used for the text
   UDHI [in] : The GSM user data header indicator
   UD [in] : The encoded user data including the user data header if present

RETURN VALUE:
  -1 if there was an error
  otherwise, the length of the user data in bytes

SIDE EFFECTS:
  none
=============================================================================*/
static int CalcUDLengthInBytes(byte UDL, GSMSMSDCS DCS, boolean UDHI, const byte *UD)
{
   int udlen;

   switch (DCS.msgCoding) {
   case GSMSMS_ENCODING_8BIT:
       udlen = UDL;
       break;
   case GSMSMS_ENCODING_7BIT:
       udlen = UDL*7/8 + ((UDL*7 % 8) != 0);
       break;
   case GSMSMS_ENCODING_UCS2:
       udlen = UDL*2;
       break;
   default: 
       return -1;
   }

   // Add user data header length if user data is present
   if (UDHI) {
      udlen+=UD[0];
   }
   return udlen;
}


/*=============================================================================
FUNCTION: RerouteTapiResultWrapper

DESCRIPTION:
   Wraps the RerouteTapi result callback function in order to mark that
   there is no RerouteTapi transaction.
        
PARAMETERS:
   pUser [in] : forwarded 
   status [in] : forwarded

RETURN VALUE:
   None

SIDE EFFECTS:
   None
=============================================================================*/
static void RerouteTapiResultWrapper(void * pUser, int status)
{
    gRerouteTapiTransactionInProgress = FALSE;

    (*gRerouteTapiParameters.pdwStatus) = status;

    AEE_ResumeCallback(gRerouteTapiParameters.pcb, 0); // schedule callback in user mode
}


/*=============================================================================
FUNCTION: RerouteTapiUserModeWrapper

DESCRIPTION:
   Calls RerouteTapi.
        
PARAMETERS:
   dummy [in] : not used

RETURN VALUE:
   None

SIDE EFFECTS:
   None
=============================================================================*/
static void RerouteTapiUserModeWrapper(void* dummy)
{
   OEMGSMSMS_RerouteTAPI(
      (char*)&gRerouteTapiParameters.szDst,
      (char*)&gRerouteTapiParameters.szMsg,
       gRerouteTapiParameters.clsDst,
       RerouteTapiResultWrapper, // use a wrapper around the user's callback function
       NULL
      );
}


/*=============================================================================
FUNCTION: GSMSMS_ProcessCacheNotification

DESCRIPTION:
   Notify that new MT messages are available in the cache.

PARAMETERS:
   None

RETURN VALUE:
   none

SIDE EFFECTS:
   none
=============================================================================*/
void GSMSMS_ProcessCacheNotification(void)
{
   AEE_ResumeCallback(&gNewMessageCB, 0);
}


/*=============================================================================
Function: GSMSMS_ScheduleRerouteTapi()

Description:
  Asynchrously call RerouteTapiUserModeWrapper in user mode.

Parameters:
   pszDst     - Destination address.
   pszMsg     - Message to be sent out.
   clsDst     - Class destination. Set to non-zero value only for BREW 
                directed messages.
   pdwStatus  - Place to be filled with the async operation's status.
   pcb        - User mode callback to report the async operation status..

Return Value:
   EFAILED   - General error
   EITEMBUSY - SMS transaction is already in progress
   SUCCESS   - Synchronous part of the routing was successful

Comments:  
   None

Side Effects: 
   None

Also See:
  OEMGSMSMS_SVC_RerouteTAPI()
===========================================================================*/
int GSMSMS_ScheduleRerouteTapi (const char * pszDst,
                                const char * pszMsg,
                                AEECLSID     clsDst,
                                uint32     * pdwStatus,
                                AEECallback* pcb)
{
   uint16 strTotalLength;

   if (gRerouteTapiTransactionInProgress == TRUE)
   {
       return EITEMBUSY;
   }
 
   CALLBACK_Init(
      &gRerouteTapiCallback, 
      RerouteTapiUserModeWrapper,
      (void*)NULL
      );

   // copy data to user mode
   strTotalLength = STRLEN(pszDst)+1; // add 1 to include NULL char
   if (strTotalLength > sizeof(gRerouteTapiParameters.szDst))
   {
       return EFAILED;
   }

   COPY_TO_USER(&gRerouteTapiParameters.szDst, pszDst, strTotalLength);

   strTotalLength = STRLEN(pszMsg)+1; // add 1 to include NULL char
   if (strTotalLength > sizeof(gRerouteTapiParameters.szMsg))
   {
       return EFAILED;
   }

   COPY_TO_USER(&gRerouteTapiParameters.szMsg, pszMsg, strTotalLength);

   COPY_TO_USER(&gRerouteTapiParameters.clsDst, &clsDst, sizeof(gRerouteTapiParameters.clsDst));
   COPY_TO_USER(&gRerouteTapiParameters.pdwStatus, &pdwStatus, sizeof(gRerouteTapiParameters.pdwStatus));
   COPY_TO_USER(&gRerouteTapiParameters.pcb, &pcb, sizeof(gRerouteTapiParameters.pcb));

   gRerouteTapiTransactionInProgress = TRUE;

   // schedule callback in user mode, in the right app context 
   AEE_ResumeCallback(&(gRerouteTapiCallback), AEE_GetAppContext());

   return SUCCESS;
}


/*=============================================================================
FUNCTION: GetHardCodedDefaultSMSCAddr

DESCRIPTION:
   Get the software, hardcoded, default SMS center address.
        
PARAMETERS:
   pScAddr [out] : address data structure that will be set

RETURN VALUE:
   None

SIDE EFFECTS:
   None
=============================================================================*/
static void GetHardCodedDefaultSMSCAddr
(
   CardHandlerAddressType* pScAddr
)
{
    pScAddr->length = 2;
    pScAddr->ext = 0;
    pScAddr->digits[0] = 0x77;
    pScAddr->nature_of_address = (byte)GSMSMS_TON_NATIONAL_NUMBER;
    pScAddr->numbering_plan = (byte)GSMSMS_NPI_ISDN_TELEPHONE;
}


#endif /* FEATURE_GSM1x */


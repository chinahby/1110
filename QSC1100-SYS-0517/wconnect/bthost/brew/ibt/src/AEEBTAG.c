/*=============================================================================
FILE:  AEEBTAG.c

SERVICES:  Implementation of IBTAudioGateway

GENERAL DESCRIPTION:
   Implements IBTAudioGateway.

===============================================================================
        Copyright (c) 2007-2009 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTAG.c#7 $ 
$DateTime: 2009/04/27 16:29:52 $
$Author: phuongn $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2009-04-24  pn   Fixed issue of two ERROR sent in response to AT+COPS.
2009-03-24  co   Featurized applicable code under FEATURE_BMP.
2009-03-18  co   Use IContacts for memory dialing.
2009-02-19  co   To stop RING alert when call ends.
2009-02-19  co   Fixed bugs in formatting +COPS response.
2009-01-13  gb   AG object will now get created even if IConfig object 
                 creation fails in AEEBTAG_AudioHandlerInit().
2008-12-15  hs   Set the proper state in AEEBTAG_StartSCO() to fix a bug. 
2008-11-24  co   Fixed bugs in AEEBTAG_EndCall().
2008-10-02  rs   Removing compiler and Lint warnings.

=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEE_OEM.h"
#include "AEE_OEMHeap.h"
#include "AEE_OEMDispatch.h"
#include "AEEBTAudioGateway.bid"
#include "OEMBTAG.h"
#include "OEMBTAudio.h"
#include "OEMBTUtils.h"
#include "AEECriticalSection.h"
#include "AEEModel.h"
#include "AEEISignal.h"
#include "AEEISignalCtl.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalCBFactory.h"
#include "AEEISignalBus.h"
#include "AEESignalBus.bid"
#include "AEETelephone.h"
#include "AEECard.h"
#include "AEEAddrBookExt.h"
#ifdef FEATURE_BMP
#include "pim_IContacts.h"
#include "pim_IContactsSearch.h"
#include "pim_IContactsResults.h"
#include "pim_IContactsRecord.h"
#include "pim_EFSDBDevice.bid"
#include "pim_ContactsSearch.bid"
#endif /* FEATURE_BMP */
#include "AEECallHistory.h"
#include "AEEConfig.h"
#include "AEEBT.bid"
#include "AEEBT.h"
#include "AEEBTMgr.bid"
#include "AEEIBTMgr.h"
#ifndef BT_SIMULATION 
#include "OEMCFGI.h"
#include "OEMNV.h"
#include "AEEStd.h"
#else
#include "btmsg.h"
#endif /* BT_SIMULATION */

/*======================================================================= 
   Defines
=======================================================================*/
typedef struct
{
   char*    pszCmdName;
   uint16   uNumParams;
   boolean  (*procFunc) (IBTAudioGateway* pMe, char* args[]);
}  ATCmdProcInfo;

typedef struct
{
   CallListener         callListener;
   ITelephone*          piPhone;
   ICallMgr*            piCallMgr;
   AEETCalls*           piCallList;
   uint32               uCallListSize;
   uint32               uFlags;
   char                 szStrToDial [AEET_MAX_DIGITS_LENGTH];
}  AEEBTAGCallManager;

typedef struct
{
   char*                pszATCmd;
   uint16               uNumChars;
   uint16               uParamsRcvd;
   uint32               uFlags;
   ISignalCtl*          piATCmdSignalCtl;
   ISignal*             piATCmdSignal;
}  AEEBTAGATCmdHandler;

struct IBTAudioGateway
{
   const IBTAudioGatewayVtbl*        pvt;
   uint32               uRefs;
   uint32               uFlags;
   IShell*              piShell;
   ACONTEXT*            pAC;
   OEMAG*               pOEMAG;
   ISignalCBFactory*    piSigCBF;
   ISignalBus*          piOpSignalBus;
   ISignalBus*          piEvSignalBus;

   // config info
   BTAGConfigFlags      dCfgFlags;
   BTAGButtonFunction   dButtonAction;
   BTSecurityLevel      dSecurity;
   BDAddress            sAddrList [AEEBTAG_MAX_DEVICES];
   int                  nNumCfgDevs;
   IBT*                 piBT;
   IBTMgr*              piMgr;

   // state info
   BTAGState            dState;
   BTAGOperationType    dPendingOperation;
   BTAGOperationType    dLastCmpltOperation;
   BTSecurityLevel      dTempSecurity;
   int                  nConnAttempts;
   int                  nConnIdx;
   BTAGConnection       sConnections[OEMBTAG_MAX_CONNECTIONS];
   BTAGConnection*      pOpConn; // connection associated with current op
   int                  nNumConnections;
   BTAGEventType        dPendingEvent[10];
   int                  nNumReadyEvents;
   int                  nNextReadyEventIdx;

   // signals for OEM events
   ISignalCtl*          piEventSignalCtl;
   ISignal*             piEventSignal;

   // volume control
   uint8                uRemoteSpkrGain;  // 0-15
   uint8                uRemoteMicGain;   // 0-15
   IConfig*             piConfig;
   ModelListener        configListener;

   // call state info
   AEEBTAGCallManager   CallMgr;

   // AT command handler
   AEEBTAGATCmdHandler  ATCmdHndlr;
};

static uint32  AEEBTAG_AddRef (IBTAudioGateway*);
static uint32  AEEBTAG_Release (IBTAudioGateway*);
static int     AEEBTAG_QueryInterface (IBTAudioGateway*, AEECLSID, void**);
static int     AEEBTAG_OnOpResult (IBTAudioGateway*, ISignal*);
static int     AEEBTAG_GetOpResult (IBTAudioGateway*, BTAGOperationType*, 
                                    BTResult*);
static int     AEEBTAG_OnEventNotify (IBTAudioGateway*, ISignal*);
static int     AEEBTAG_GetEvent (IBTAudioGateway*, BTAGEventType*);
static int     AEEBTAG_GetConnectionList (IBTAudioGateway*, BTAGConnection*, 
                                          int, int*);
static int     AEEBTAG_GetStatus (IBTAudioGateway*, BTAGStatus*);
static int     AEEBTAG_Config (IBTAudioGateway*, BTAGConfigFlags, 
                               BTAGButtonFunction, BTSecurityLevel, 
                               const char*, const BDAddress*, int);
static int     AEEBTAG_ConfigEx (IBTAudioGateway*, const char*);
static int     AEEBTAG_Enable (IBTAudioGateway*);
static int     AEEBTAG_Disable (IBTAudioGateway*);
static int     AEEBTAG_Connect (IBTAudioGateway*, const BDAddress*);
static int     AEEBTAG_Disconnect (IBTAudioGateway*, const BDAddress*);
static int     AEEBTAG_ConnectAudio (IBTAudioGateway*, const BDAddress*);
static int     AEEBTAG_DisconnectAudio (IBTAudioGateway*, const BDAddress*);
static int     AEEBTAG_ReportVRState (IBTAudioGateway*, boolean);

static const AEEVTBL(IBTAudioGateway) gpvtIBTAudioGateway =
{
   AEEBTAG_AddRef,
   AEEBTAG_Release,
   AEEBTAG_QueryInterface,
   AEEBTAG_OnOpResult,
   AEEBTAG_GetOpResult,
   AEEBTAG_OnEventNotify,
   AEEBTAG_GetEvent,
   AEEBTAG_GetConnectionList,
   AEEBTAG_GetStatus,
   AEEBTAG_Config,
   AEEBTAG_ConfigEx,
   AEEBTAG_Enable,
   AEEBTAG_Disable,
   AEEBTAG_Connect,
   AEEBTAG_Disconnect,
   AEEBTAG_ConnectAudio,
   AEEBTAG_DisconnectAudio,
   AEEBTAG_ReportVRState
};

static IBTAudioGateway* pgTheOnlyAG = NULL;
static AEECriticalSection gAEEAGCriticalSection;

static boolean AEEBTAG_ATCmdProcSetCOPS (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcReadCOPS (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcCNUM (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcCMEE (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcReadCSCS (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcQueryCSCS (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcSetCSCS (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcCSQ (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcCBC (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcCIMI (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcBLDN (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcBTRH (IBTAudioGateway* pMe, char* args[]);
static boolean AEEBTAG_ATCmdProcMemDial (IBTAudioGateway* pMe, char* args[]);

static const ATCmdProcInfo ATCmdTable[] =
{
   { "AT+COPS=",    2, AEEBTAG_ATCmdProcSetCOPS    },
   { "AT+COPS?",    0, AEEBTAG_ATCmdProcReadCOPS   },
   { "AT+CNUM",     0, AEEBTAG_ATCmdProcCNUM       },
   { "AT+CMEE=1",   0, AEEBTAG_ATCmdProcCMEE       },
   { "AT+CSCS?",    0, AEEBTAG_ATCmdProcReadCSCS   },
   { "AT+CSCS=?",   0, AEEBTAG_ATCmdProcQueryCSCS  },
   { "AT+CSCS=",    1, AEEBTAG_ATCmdProcSetCSCS    },
   { "AT+CSQ",      0, AEEBTAG_ATCmdProcCSQ        },
   { "AT+CBC",      0, AEEBTAG_ATCmdProcCBC        },
   { "AT+CIMI",     0, AEEBTAG_ATCmdProcCIMI       },
   { "AT+BLDN",     0, AEEBTAG_ATCmdProcBLDN       },
   { "AT+BTRH?",    0, AEEBTAG_ATCmdProcBTRH       },
   { "ATD>",        1, AEEBTAG_ATCmdProcMemDial    }
};

#define AEEAG_LOCK()    AEECriticalSection_Enter(&gAEEAGCriticalSection)
#define AEEAG_UNLOCK()  AEECriticalSection_Leave(&gAEEAGCriticalSection)

#define AEEBTAG_AUTOANSWER_TIME                 2000 // 2 seconds
#define AEEBTAG_AUDIO_DISC_DELAY_TIME            500 // 500 ms

// per HFP 1.5 specs, these are the supported extended error codes
#define AEEBTAG_CME_AG_FAILURE                     0
#define AEEBTAG_CME_NO_CONNECTION                  1
#define AEEBTAG_CME_OP_NOT_ALLOWED                 3
#define AEEBTAG_CME_OP_NOT_SUPPORTED               4
#define AEEBTAG_CME_PH_SIM_PIN_REQUIRED            5
#define AEEBTAG_CME_SIM_NOT_INSERTED              10
#define AEEBTAG_CME_SIM_PIN_REQUIRED              11
#define AEEBTAG_CME_SIM_PUK_REQUIRED              12
#define AEEBTAG_CME_SIM_FAILURE                   13
#define AEEBTAG_CME_SIM_BUSY                      14
#define AEEBTAG_CME_INCORRECT_PASSWORD            16
#define AEEBTAG_CME_SIM_PIN2_REQUIRED             17
#define AEEBTAG_CME_SIM_PUK2_REQUIRED             18
#define AEEBTAG_CME_MEMORY_FULL                   20
#define AEEBTAG_CME_INVALID_INDEX                 21
#define AEEBTAG_CME_MEMORY_FAILURE                23
#define AEEBTAG_CME_TEXT_STRING_TOO_LONG          24
#define AEEBTAG_CME_INVALID_CHAR_IN_TEXT          25
#define AEEBTAG_CME_DIAL_STRING_TOO_LONG          26
#define AEEBTAG_CME_INVALID_CHAR_IN_DIAL_STRING   27
#define AEEBTAG_CME_NO_NETWORK_SERVICE            30
#define AEEBTAG_CME_NETWORK_NOT_ALLOWED           32

// per HFP 1.5 specs, number type 128-143=unknown, 
// 144-159=international, and 160-175=national
#define AEEBTAG_NUM_TYPE_UNKNOWN       128
#define AEEBTAG_NUM_TYPE_INTERNATIONAL 144
#define AEEBTAG_NUM_TYPE_NATIONAL      160

#define AEEBTAG_MEM_DIAL_INDEX_LEN     5  // arbitrary value

// per HFP 1.5 specs
#define AEEBTAG_AT_CMD_TERM_CHAR            '\r'
#define AEEBTAG_AT_CMD_DELIMITER            ';'
#define AEEBTAG_AT_CMD_PARAM_SEPARATOR      ','

#define AEEBTAG_FLAG_OP_BY_ME                   (1 << 0)
#define AEEBTAG_FLAG_BRINGING_UP_AUDIO          (1 << 1)
#define AEEBTAG_FLAG_SEND_RING                  (1 << 2)
#define AEEBTAG_FLAG_SEC_UPDATE_FAILED          (1 << 3)
#define AEEBTAG_FLAG_A2DP_CONNECTED             (1 << 4)
#define AEEBTAG_FLAG_A2DP_STREAMING             (1 << 5)
#define AEEBTAG_FLAG_A2DP_SUSPENDED_BY_ME       (1 << 6)
#define AEEBTAG_FLAG_SCO_FOR_KEYBEEP            (1 << 7)

#define AEEBTAG_FLAG_ATCMD_COPS_SET_RCVD        (1 << 0)
#define AEEBTAG_FLAG_ATCMD_CMEE_RCVD            (1 << 1)

#define AEEBTAG_FLAG_CALLMGR_DEV_DIALED         (1 << 0)
#define AEEBTAG_FLAG_CALLMGR_DEV_ANSWERED       (1 << 1)
#define AEEBTAG_FLAG_CALLMGR_DEV_ENDED          (1 << 2)

#define IS_CONNECTABLE()                       \
   ((AEEBTAG_STATE_ENABLED <= pMe->dState) &&  \
    ((0 < pMe->nNumConnections) || (0 < pMe->nNumCfgDevs)))

/************************************************************************ 
                           PRIVATE FUNCTIONS
************************************************************************/
static int AEEBTAG_StartACL (IBTAudioGateway* pMe)
{
   int result = AEE_EFAILED;
   if (0 == pMe->nNumCfgDevs)
   {
      DBGPRINTF ("AG_StartACL - no configured devs");
   }
   else if (pMe->dCfgFlags & AEEBTAG_CFG_NO_AUTO_RECONN_B)
   {
      DBGPRINTF ("AG_StartACL - configured to not auto connect");
   }
   else
   {
      while ((AEE_SUCCESS != result) && (pMe->nConnIdx < pMe->nNumCfgDevs))
      {
         pMe->uFlags |= AEEBTAG_FLAG_OP_BY_ME;
         result = AEEBTAG_Connect (pMe, &pMe->sAddrList[pMe->nConnIdx]);
         if (AEE_SUCCESS != result)
         {
            pMe->uFlags &= ~AEEBTAG_FLAG_OP_BY_ME;
            DBGPRINTF ("AG_StartACL - failed to connect to %02x%02x%02x",
                       pMe->sAddrList[pMe->nConnIdx].uAddr[5],
                       pMe->sAddrList[pMe->nConnIdx].uAddr[4],
                       pMe->sAddrList[pMe->nConnIdx].uAddr[3]);
         }
         pMe->nConnIdx++;
      }
   }
   return result;
}

static BTAGConnection* AEEBTAG_FindActiveConn (IBTAudioGateway* pMe)
{
   BTAGConnection* pConn = NULL;
   // the first connection is always the active one
   if (AEEBT_CONN_STATE_CONNECTED == pMe->sConnections[0].dConnState)
   {
      pConn = &pMe->sConnections[0];
   }
   return pConn;
}

static boolean AEEBTAG_ResumeA2DP (IBTAudioGateway* pMe)
{
   boolean bWait = FALSE;
   DBGPRINTF ("AG_ResumeA2DP - flags=0x%0x", pMe->uFlags);
   if (pMe->uFlags & AEEBTAG_FLAG_A2DP_SUSPENDED_BY_ME)
   {
      OEMBTAG_ResumeA2DP (pMe->pOEMAG);
      pMe->uFlags &= ~AEEBTAG_FLAG_A2DP_SUSPENDED_BY_ME;
      bWait = TRUE;
   }
   return bWait;
}

static boolean AEEBTAG_SuspendA2DP (IBTAudioGateway* pMe)
{
   boolean bWait = FALSE;
   DBGPRINTF ("AG_SuspendA2DP - flags=0x%0x", pMe->uFlags);
   if (pMe->uFlags & AEEBTAG_FLAG_A2DP_STREAMING)
   {
      OEMBTAG_SuspendA2DP (pMe->pOEMAG);
      pMe->uFlags |= AEEBTAG_FLAG_A2DP_SUSPENDED_BY_ME;
      bWait = TRUE;
   }
   return bWait;
}

static int AEEBTAG_StopSCO (IBTAudioGateway* pMe)
{
   int result;
   BTAGConnection* pConn;

   DBGPRINTF ("Stopping SCO");

   if (AEEBTAG_STATE_AUDIO_CONNECTED != pMe->dState)
   {
      result = AEE_EALREADY;
   }
   else if ((pConn = AEEBTAG_FindActiveConn(pMe)) == NULL)
   {
      result = AEE_ENOSUCH;
   }
   else if (AEEBT_CONN_STATE_CONNECTED == pConn->dAudioConnState)
   {
      pMe->uFlags |= AEEBTAG_FLAG_OP_BY_ME;
      result = AEEBTAG_DisconnectAudio (pMe, &pConn->remoteAddr);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~AEEBTAG_FLAG_OP_BY_ME;
      }
   }
   else
   {
      result = AEE_SUCCESS;
   }
   return result;
}

static int AEEBTAG_StartSCO (IBTAudioGateway* pMe)
{
   int result;
   BTAGConnection* pConn;

   DBGPRINTF ("Starting SCO");

   ISHELL_CancelTimer (pMe->piShell, (PFNNOTIFY) AEEBTAG_StopSCO, pMe );

   if (0 == pMe->nNumConnections)
   {
      result = AEEBTAG_StartACL (pMe);
   }
   else if ((pConn = AEEBTAG_FindActiveConn(pMe)) == NULL)
   {
      result = AEE_EFAILED;
   }
   else if (FALSE != AEEBTAG_SuspendA2DP (pMe)) // TRUE means wait
   {
      DBGPRINTF ("AG_StartSCO - waiting for A2DP to suspend");
	  pMe->uFlags |= AEEBTAG_FLAG_BRINGING_UP_AUDIO;
      result = AEE_EITEMBUSY;
   }
   else if (AEEBT_CONN_STATE_CONNECTED != pConn->dAudioConnState)
   {
      pMe->uFlags |= AEEBTAG_FLAG_OP_BY_ME;
      pMe->uFlags |= AEEBTAG_FLAG_BRINGING_UP_AUDIO;
      result = AEEBTAG_ConnectAudio (pMe, &pConn->remoteAddr);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~AEEBTAG_FLAG_OP_BY_ME;
      }
   }
   else
   {
      result = AEE_SUCCESS;
   }
   return result;
}

static boolean AEEBTAG_Dial (IBTAudioGateway* pMe, char* pszString2Dial)
{
   boolean bSuccess = FALSE;
   int nLen = std_strlen (pszString2Dial) + 20;
   ICall* piCall = NULL;
   char* pszURL = MALLOC (nLen);

   if (NULL != pszURL)
   {
      std_strlcpy (pszURL, "tel:Call?Number=", nLen);
      std_strlcat (pszURL, pszString2Dial, nLen);
      /* TODO: Need to "sanitize" string */
      DBGPRINTF ("AG_Dial - URL=\"%s\"", pszURL);
   }

   if ((NULL != pszURL) && (ISHELL_SendURL (pMe->piShell, pszURL) != FALSE))
   {
      bSuccess = TRUE;
      DBGPRINTF ("AG_Dial - dialed via URL");
   }
   else if (ICALLMGR_OriginateVoice (pMe->CallMgr.piCallMgr, 
                                     pszString2Dial, &piCall, NULL) == SUCCESS)
   {
      bSuccess = TRUE;
      DBGPRINTF ("AG_Dial - dialed via ICALLMGR");
   }
   else
   {
      DBGPRINTF_ERROR ("AG_Dial - failed to make call ");
   }

   if (FALSE != bSuccess)
   {
      pMe->CallMgr.uFlags |= AEEBTAG_FLAG_CALLMGR_DEV_DIALED;
      (void) AEEBTAG_StartSCO (pMe);
      bSuccess = TRUE;
   }

   FREEIF (pszURL);
   if (NULL != piCall)
   {
      ICALL_Release (piCall);
   }

   return bSuccess;
}

static boolean AEEBTAG_Redial (IBTAudioGateway* pMe)
{
   int result, i, nSize = 0;
   boolean bSuccess = FALSE;
   ICallHistory* piCallHist = NULL;
   const AEECallHistoryEntry* pEntry = NULL;
   char* pszNum = NULL;

   if ((ISHELL_CreateInstance (pMe->piShell, AEECLSID_CALLHISTORY,
                               (void**)&piCallHist) == AEE_SUCCESS) &&
       (ICALLHISTORY_EnumInitByCallType (
          piCallHist, AEECALLHISTORY_CALL_TYPE_TO) == AEE_SUCCESS) &&
       ((pEntry = ICALLHISTORY_EnumNext (piCallHist, &result)) != NULL))
   {
      for (i = 0; i < pEntry->wNumFields; i++ )
      {
         if (pEntry->pFields[i].wID == AEECALLHISTORY_FIELD_NUMBER_EX)
         {
            nSize = std_wstrlen ((AECHAR*)(pEntry->pFields[i].pData)) + 1;
            pszNum = MALLOC (nSize);
            if (NULL == pszNum)
            {
               DBGPRINTF_ERROR ("AG_Redial - failed to get mem for dial str");
            }
            else
            {
               WSTRTOSTR ((AECHAR*)(pEntry->pFields[i].pData), pszNum, nSize);
               DBGPRINTF_ERROR ("AG_Redial - num=%s", pszNum);
               bSuccess = AEEBTAG_Dial (pMe, pszNum);
               FREEIF (pszNum);
            }
            break;
         }
      }
   }
   if (NULL != piCallHist)
   {
      ICALLHISTORY_Release (piCallHist);
   }
   return bSuccess;
}


static boolean AEEBTAG_MemDial (IBTAudioGateway* pMe, const char* pszMemIdx)
{
   boolean bSuccess = FALSE;

#ifdef FEATURE_BMP
   int nLenReq;
   int nPhoneNums = 0;

   pim_IContacts *pEfsContacts = NULL; // we only support the Contact Book on the phone
   pim_IContactsSearch *piContactsSearch = NULL;
   pim_IContactsResults* piResults = NULL;
   pim_IContactsRecord* piRec = NULL;

   char KeyStr[ sizeof("Phone/xyz/SpeedDial") ] = {"Phone/"};
   AECHAR wszMemIdx[ AEEBTAG_MEM_DIAL_INDEX_LEN ];
   AECHAR wszKeyStr[ (sizeof("SPEEDDIAL = ") + AEEBTAG_MEM_DIAL_INDEX_LEN) *
                        sizeof(AECHAR) ] = {L"SPEEDDIAL = "};

   // Construct the key string for record search
   if ((NULL != pszMemIdx) &&
       (AEEBTAG_MEM_DIAL_INDEX_LEN >= std_strlen(pszMemIdx)))
   {
      STRTOWSTR( pszMemIdx, wszMemIdx, sizeof(wszMemIdx) );
      WSTRLCAT( wszKeyStr, wszMemIdx, sizeof(wszKeyStr) / sizeof(AECHAR) );
   }
   else
   {
      DBGPRINTF ("AG_MemDial - invalid memory index");
      return FALSE;
   }

   if (AEE_SUCCESS != ISHELL_CreateInstance (pMe->piShell,
                                             pim_AEECLSID_EFSDBDevice,
                                             (void **)&pEfsContacts))
   {
      DBGPRINTF ("AG_MemDial - faied to create EfsContacts");
   }
   // we don't actually have to create an IContactsSearch object if we are
   // only earching for EFS contacts. we can use pim_IContacts_Search with the
   // pEfsContacts object. IContactsSearch is intended for searching across
   // multiple icontacts devices.
   // 
   // however, we want the code to be extendable for future support to devices
   // other than EFS contacts.
   else if (AEE_SUCCESS != ISHELL_CreateInstance (pMe->piShell,
                                                  pim_AEECLSID_ContactsSearch,
                                                  (void **)&piContactsSearch))
   {
      DBGPRINTF ("AG_MemDial - faied to create iContactsSearch");
   }
   else if (AEE_SUCCESS != pim_IContactsSearch_Search (piContactsSearch,
                                                       &pEfsContacts,
                                                       1,
                                                       wszKeyStr,
                                                       &piResults))
   {
      DBGPRINTF ("AG_MemDial - contact search failed");
   }
   // there should only be one result.
   // get the number of phone entries on the record here as well
   else if ((AEE_SUCCESS != pim_IContactsResults_GetNext (
                               piResults, &piRec)) ||
            (AEE_SUCCESS != pim_IContactsRecord_GetNumChildren (
                               piRec,
                               "Phone/",
                               &nPhoneNums)) ||
            (999 < nPhoneNums)) // normally, nPhoneNums should have less than 3 digits
   {
      DBGPRINTF ("AG_MemDial - record retrieval failed, phNums=%d",
                 nPhoneNums);
   }
   else // so far so good, let's find the number to dial
   {
      while (nPhoneNums)
      {
         // Construct the key string for SpeedDial index
         std_strlprintf (KeyStr, sizeof(KeyStr),
                         "Phone/%d/SpeedDial",
                         nPhoneNums);

         if ((AEE_SUCCESS != pim_IContactsRecord_GetFieldValue (
                                piRec,
                                KeyStr,
                                wszKeyStr,
                                sizeof(wszKeyStr) /
                                   sizeof(AECHAR),
                                &nLenReq)) ||
             ((sizeof(wszKeyStr) / sizeof(AECHAR)) < nLenReq))
         {
            DBGPRINTF ("AG_MemDial - failed to get wszKeyStr");
            break;
         }
         else if (0 == WSTRNCMP( wszKeyStr, wszMemIdx, sizeof(wszMemIdx) ))
         {
            //hit! now get the number associated with the entry
            std_strlprintf (KeyStr, sizeof(KeyStr),
                            "Phone/%d/Value",
                            nPhoneNums);

            if ((AEE_SUCCESS != pim_IContactsRecord_GetFieldValue (
                                   piRec,
                                   KeyStr,
                                   wszKeyStr,
                                   sizeof(wszKeyStr) /
                                      sizeof(AECHAR),
                                   &nLenReq)) ||
                ((sizeof(wszKeyStr) / sizeof(AECHAR)) < nLenReq))
            {
               DBGPRINTF ("AG_MemDial - failed to get number");
            }
            else
            {
               // got the phone number to dial
               char* pszNum = NULL;
               nLenReq = std_wstrlen (wszKeyStr) + 1;
               pszNum = MALLOC (nLenReq);

               if (NULL != pszNum)
               {
                  WSTRTOSTR (wszKeyStr, pszNum, nLenReq);
                  DBGPRINTF ("AG_MemDial - num=%s", pszNum);
                  bSuccess = AEEBTAG_Dial (pMe, pszNum);
                  FREEIF (pszNum);
               }
               else
               {
                  DBGPRINTF ("AG_MemDial - failed to get mem for dial str");
               }
            }
            break;
         }
         // else, continue
      } // end of the loop
   }

   IQI_RELEASEIF(pEfsContacts);
   IQI_RELEASEIF(piContactsSearch);
   IQI_RELEASEIF(piResults);
   IQI_RELEASEIF(piRec);

#else /* else, !FEATURE_BMP */

   IAddrBook* piAddrBook = NULL;

   if ((ISHELL_CreateInstance (pMe->piShell, AEECLSID_ADDRBOOK, 
                               (void **)&piAddrBook) == SUCCESS) &&
       (IADDRBOOK_EnumRecInit (piAddrBook, AEE_ADDR_CAT_SPEEDDIAL,
                               AEE_ADDRFIELD_NONE, NULL, 0)) == SUCCESS)
   {
      IAddrRec *piAddrRec = NULL;
      int nMemIdx = ATOI (pszMemIdx);

      while ((piAddrRec = IADDRBOOK_EnumNextRec (piAddrBook)) != NULL)
      {
         int i, phoneNumberIdx = -1;
         boolean bFound = FALSE;
         AEEAddrField* pField;
         for (i = IADDRREC_GetFieldCount(piAddrRec); i>0; i--)
         {
            if ((pField = IADDRREC_GetField (piAddrRec, i-1)) != NULL)
            {
               if ((AEE_ADDRFIELD_INDEX == pField->fType) &&
                   (nMemIdx == *(int*)pField->pBuffer))
               {
                  bFound = TRUE;
               }
               if (AEE_ADDRFIELD_PHONE_OTHER == pField->fType)
               {
                  phoneNumberIdx = i-1;
               }
            }
         }
         if ((FALSE != bFound) &&
             ((pField = IADDRREC_GetField (piAddrRec, phoneNumberIdx)) != NULL))
         {
            // got the phone number to dial
            int nSize = std_wstrlen ((AECHAR *)(pField->pBuffer)) + 1;
            char* pszNum = MALLOC (nSize);
            if (NULL == pszNum)
            {
               DBGPRINTF_ERROR ("AG_MemDial - failed to get mem for dial str");
            }
            else
            {
               WSTRTOSTR ((AECHAR*)(pField->pBuffer), pszNum, nSize);
               DBGPRINTF_ERROR ("AG_MemDial - num=%s", pszNum);
               bSuccess = AEEBTAG_Dial (pMe, pszNum);
               FREEIF (pszNum);
            }
         }
      }
   }
   if (NULL != piAddrBook)
   {
      IADDRBOOK_Release (piAddrBook);
   }
#endif /* FEATURE_BMP */

   return bSuccess;
}


static AEETCallState AEEBTAG_GetCallState (IBTAudioGateway* pMe)
{
   ICall* piCall = NULL;
   AEETCallState callState = AEET_CALL_STATE_NONE;
   if (ICALLMGR_GetActiveVoiceCall (pMe->CallMgr.piCallMgr, &piCall) == SUCCESS)
   {
      callState = ICALL_GetState (piCall);
      ICALL_Release (piCall);
   }
   return callState;
}

static void AEEBTAG_AutoAnswerCall (IBTAudioGateway* pMe)
{
   AEETCallState callState = AEEBTAG_GetCallState (pMe);

   DBGPRINTF ("AutoAnswerCall - callState=0x%x flags=0x%x", 
              callState, pMe->CallMgr.uFlags);

   if (callState & AEET_CALL_STATE_CONV)
   {
      if (AEEBTAG_STATE_CONNECTED == pMe->dState)
      {
         (void) AEEBTAG_StartSCO (pMe);
      }
   }
   else if (callState & AEET_CALL_STATE_INCOM)
   {
      pMe->CallMgr.uFlags |= AEEBTAG_FLAG_CALLMGR_DEV_ANSWERED;
   }
}

static void AEEBTAG_AnswerCall (IBTAudioGateway* pMe)
{
   ICall* piCall = NULL;
   AEETCallState callState = AEET_CALL_STATE_NONE;
   if (ICALLMGR_GetActiveVoiceCall (pMe->CallMgr.piCallMgr, &piCall) == SUCCESS)
   {
      callState = ICALL_GetState (piCall);
      DBGPRINTF ("AnswerCall - callState=0x%4x", callState);
      if (callState & AEET_CALL_STATE_INCOM)
      {
         if (AEEBTAG_STATE_AUDIO_CONNECTED == pMe->dState)
         {
            if (ISHELL_SendURL (pMe->piShell, "tel:AnswerCall") == FALSE)
            {
               (void) ICALL_Answer (piCall);
            }
         }
         else
         {
            (void) AEEBTAG_StartSCO (pMe);
         }
         pMe->CallMgr.uFlags |= AEEBTAG_FLAG_CALLMGR_DEV_ANSWERED;
      }
      ICALL_Release (piCall);
   }
}

static void AEEBTAG_EndCall (IBTAudioGateway* pMe)
{
   /* this function is to end ALL calls */

   ICall*  piCall = NULL;
   uint8   uIndex;

   DBGPRINTF ("EndCall - AGst=%d", pMe->dState);

   OEMBTAG_StopRingAlert (pMe->pOEMAG);

   if (AEEBTAG_STATE_AUDIO_CONNECTED == pMe->dState)
   {
      pMe->CallMgr.uFlags |= AEEBTAG_FLAG_CALLMGR_DEV_ENDED;
      (void) AEEBTAG_StopSCO (pMe);
   }
   else
   {
      pMe->CallMgr.uFlags &= ~AEEBTAG_FLAG_CALLMGR_DEV_ENDED;

      if (ITELEPHONE_GetCalls (pMe->CallMgr.piPhone, pMe->CallMgr.piCallList,
                               pMe->CallMgr.uCallListSize) != SUCCESS)
      {
         DBGPRINTF_ERROR ("EndCall - failed to get call list. "
                          "All bets are off!");
      }
      else
      {
         DBGPRINTF ("EndCall - numCalls=%d", pMe->CallMgr.piCallList->dwCount);

         /* To end only voice calls */
         for (uIndex = 0; uIndex < pMe->CallMgr.piCallList->dwCount; uIndex++)
         {
            /* get call type */
            if ((ICALLMGR_GetCall (pMe->CallMgr.piCallMgr,
                                   pMe->CallMgr.piCallList->calls [uIndex],
                                   &piCall) == SUCCESS) &&
                (AEET_CALL_TYPE_VOICE == ICALL_GetType (piCall)))
            {
               (void) ICALL_End (piCall);
            }

            if (NULL != piCall)
            {
               ICALL_Release (piCall);
            }
         }
      }
   }
}

static void AEEBTAG_SendErrResponse (IBTAudioGateway* pMe, uint32 uErrCode)
{
   char szErrMsg [20] = "ERROR";
   if (pMe->ATCmdHndlr.uFlags & AEEBTAG_FLAG_ATCMD_CMEE_RCVD)
   {
      std_strlprintf ((char*)szErrMsg, sizeof(szErrMsg),
                      "+CME ERROR: %d", uErrCode);
   }
   DBGPRINTF ("Sending - %s", szErrMsg);
   OEMBTAG_SendResponse (pMe->pOEMAG, szErrMsg);
}

static boolean AEEBTAG_ATCmdProcSetCOPS (IBTAudioGateway* pMe, char* args[])
{
   DBGPRINTF ("ATCmdProcSetCOPS - %c %c", *args[0], *args[1]);

   // Per HFP 1.5 specs, only acceptable format is "AT+COPS=3,0"
   if ((*args[0] == '3') && (*args[1] == '0'))
   {
      pMe->ATCmdHndlr.uFlags |= AEEBTAG_FLAG_ATCMD_COPS_SET_RCVD;
      OEMBTAG_SendResponse (pMe->pOEMAG, "OK");
   }
   else
   {
      AEEBTAG_SendErrResponse (pMe, AEEBTAG_CME_OP_NOT_ALLOWED);
   }
   return TRUE; // don't send error response
}

static boolean AEEBTAG_ATCmdProcReadCOPS (IBTAudioGateway* pMe, char* args[])
{
   boolean bSuccess = FALSE;
   uint32 uError = AEEBTAG_CME_OP_NOT_ALLOWED;

   DBGPRINTF ("ATCmdProcReadCOPS - entered");

   if (pMe->ATCmdHndlr.uFlags & AEEBTAG_FLAG_ATCMD_COPS_SET_RCVD)
   {
      AEETPhInfo phInfo;
      AEETSSInfo ssInfo;
      int* pnMode  = (int*)&phInfo.network_sel_mode_pref;

      uError = AEEBTAG_CME_AG_FAILURE;
      if (ITELEPHONE_GetPhoneInfo (pMe->CallMgr.piPhone, 
                                   &phInfo, sizeof(phInfo)) != SUCCESS)
      {
         DBGPRINTF_ERROR ("ATCmdProcReadCOPS - failed to get phone info");
      }
      else if ((*pnMode < 0) || (*pnMode > 4))
      {
         DBGPRINTF_ERROR ("ATCmdProcReadCOPS - invalid mode %d", *pnMode);
      }
      else if (ITELEPHONE_GetServingSystemInfo (pMe->CallMgr.piPhone, &ssInfo, 
                                                sizeof(ssInfo)) != SUCCESS)
      {
         DBGPRINTF_ERROR ("ATCmdProcReadCOPS - failed to get SS info");
      }
      else
      {
         /* Note that in here we assume ASCII is used for
          * the carrier name string stored in the system.
          */
         int nMsgLen  = sizeof("+COPS: 1,0,\"\"");
         uint8* pName = NULL;
         char* pszMsg = NULL;

         if (FALSE != ssInfo.mm_information.full_name_avail)
         {
            nMsgLen += ssInfo.mm_information.full_name.length;
            pName = ssInfo.mm_information.full_name.name;
         }
         else if (FALSE != ssInfo.mm_information.short_name_avail)
         {
            nMsgLen += ssInfo.mm_information.short_name.length;
            pName = ssInfo.mm_information.short_name.name;
         }
         else
         {
            DBGPRINTF_ERROR ("ATCmdProcReadCOPS - network name not available");
         }

         pszMsg = MALLOC (nMsgLen);

         if (NULL == pszMsg)
         {
            DBGPRINTF_ERROR ("ATCmdProcReadCOPS - MALLOC failed");
            uError = AEEBTAG_CME_MEMORY_FAILURE;
         }
         else
         {
            bSuccess = TRUE;
            if (NULL == pName)
            {
               std_strlprintf (pszMsg, nMsgLen, "+COPS: %d", *pnMode);
            }
            else
            {
               int nNameLen;
               int temp;

               temp     = nMsgLen - sizeof("+COPS: 1,0,\"\"");
               nNameLen = (temp <
                            sizeof(ssInfo.mm_information.short_name.name)) ?
                          temp :
                          (sizeof(ssInfo.mm_information.short_name.name) - 1);
               pName[ nNameLen ] = '\0';
               std_strlprintf (pszMsg, nMsgLen, 
                               "+COPS: %d,0,\"%s\"", *pnMode, pName);
            }
            OEMBTAG_SendResponse (pMe->pOEMAG, pszMsg);
            OEMBTAG_SendResponse (pMe->pOEMAG, "OK");
            FREE (pszMsg);
         }
      }
   }

   if (FALSE == bSuccess)
   {
      AEEBTAG_SendErrResponse (pMe, uError);
   }
   return TRUE; // don't send error response
}

static void AEEBTAG_GetSubcriberNumFromNV (IBTAudioGateway* pMe)
{
   int nSize = 0;
   int nLen  = 0;
   char* pszMsg  = NULL;

   if (ISHELL_GetDeviceInfoEx (pMe->piShell, AEE_DEVICEITEM_MDN, 
                               NULL, &nSize) == SUCCESS)
   {
      nSize += 20;
      pszMsg = MALLOC (nSize);
   }
   if (NULL != pszMsg)
   {
      int nMsgSize = nSize;

      std_strlcpy (pszMsg, "+CNUM: ,\"", nSize);
      nLen = std_strlen (pszMsg);
      if (ISHELL_GetDeviceInfoEx (pMe->piShell, AEE_DEVICEITEM_MDN, 
                                  (void*)(pszMsg + nLen), &nSize) == SUCCESS)
      {
         uint8 uNumType = (pszMsg[nLen] == '+') ? 
                          AEEBTAG_NUM_TYPE_INTERNATIONAL : 
                          AEEBTAG_NUM_TYPE_UNKNOWN;
         pszMsg [nLen + nSize] = '\0';
         nLen = std_strlen (pszMsg);
         std_strlprintf (&pszMsg [nLen], (nMsgSize - nLen), "\",%d,,4",
                         uNumType);
         OEMBTAG_SendResponse (pMe->pOEMAG, pszMsg);
      }
   }
   FREEIF (pszMsg);
}

static void AEEBTAG_SendCNUMResponse (IBTAudioGateway* pMe, AEEAddrField* pF)
{
   int   nLen;
   int   nSize  = std_wstrlen (pF->pBuffer) + 20;
   char* pszMsg = MALLOC (nSize);
   if (NULL != pszMsg)
   {
      uint8 uNumType = (pszMsg[0] == '+') ? 
                       AEEBTAG_NUM_TYPE_INTERNATIONAL : 
                       AEEBTAG_NUM_TYPE_UNKNOWN;
      std_strlcpy (pszMsg, "+CNUM: ,\"", nSize);
      nLen = std_strlen (pszMsg);
      WSTRTOSTR ((AECHAR*)pF->pBuffer, &pszMsg [nLen], nSize - nLen);
      nLen = std_strlen (pszMsg);
      std_strlprintf (
         &pszMsg [nLen], (nSize - nLen), "\",%d,,%d", uNumType,
         (AEE_ADDRFIELD_PHONE_FAX == pF->fID) ? 5 : 4);
      OEMBTAG_SendResponse (pMe->pOEMAG, pszMsg);
      FREEIF (pszMsg);
   }
}

static void AEEBTAG_GetSubcriberNumFromSIM (IBTAudioGateway* pMe)
{
   ICard* piCard = NULL;
   IAddrBook* piAddrBook = NULL;
   AEECardApp cardApp;

   if ((ISHELL_CreateInstance (pMe->piShell, AEECLSID_CARD,
                               (void**)&piCard) == SUCCESS) &&
       (ICARD_GetCardApps (piCard, &cardApp) == SUCCESS) &&
       (ISHELL_CreateInstance (pMe->piShell, AEECLSID_ADDRBOOK, 
                               (void **)&piAddrBook) == SUCCESS))
   {
      IAddrRec *piAddrRec = NULL;
      AEEAddrCat cat = 
         (cardApp.operational_slot & AEECARD_GSM_SLOT2_OPERATIONAL) ?
         AEE_ADDR_CAT_SIM_MSISDN2 : AEE_ADDR_CAT_SIM_MSISDN;

      if (IADDRBOOK_EnumRecInit (piAddrBook, cat, AEE_ADDRFIELD_NONE,
                                 NULL, 0) == SUCCESS)
      {
         while ((piAddrRec = IADDRBOOK_EnumNextRec (piAddrBook)) != NULL)
         {
            int i;
            AEEAddrField* pField;
            for (i = IADDRREC_GetFieldCount(piAddrRec); i>0; i--)
            {
               pField = IADDRREC_GetField (piAddrRec, i-1);
               if ((NULL != pField) && (AEEDB_FT_PHONE == pField->fType))
               {
                  AEEBTAG_SendCNUMResponse (pMe, pField);
                  break;
               }
            }
            IADDRREC_Release (piAddrRec);
         }
      }
   }
   if (NULL != piCard)
   {
      ICARD_Release (piCard);
   }
   if (NULL != piAddrBook)
   {
      IADDRBOOK_Release (piAddrBook);
   }
}

static boolean AEEBTAG_ATCmdProcCNUM (IBTAudioGateway* pMe, char* args[])
{
   AEETSSInfo ssInfo;

   DBGPRINTF ("ATCmdProcCNUM - entered");

   if (ITELEPHONE_GetServingSystemInfo (pMe->CallMgr.piPhone, &ssInfo, 
                                        sizeof(ssInfo)) != SUCCESS)
   {
      DBGPRINTF_ERROR ("ATCmdProcCNUM - failed to get SS info");
   }
   else if (AEET_SYS_MODE_CDMA == ssInfo.sys_mode)
   {
      AEEBTAG_GetSubcriberNumFromNV (pMe);
   }
   else
   {
      AEEBTAG_GetSubcriberNumFromSIM (pMe);
   }
   OEMBTAG_SendResponse (pMe->pOEMAG, "OK");
   return TRUE; // don't send error
}

static boolean AEEBTAG_ATCmdProcCMEE (IBTAudioGateway* pMe, char* args[])
{
   DBGPRINTF ("ATCmdProcCMEE - entered");
   pMe->ATCmdHndlr.uFlags |= AEEBTAG_FLAG_ATCMD_CMEE_RCVD;
   OEMBTAG_SendResponse (pMe->pOEMAG, "OK");
   return TRUE;
}

static boolean AEEBTAG_ATCmdProcReadCSCS (IBTAudioGateway* pMe, char* args[])
{
   boolean bSuccess = FALSE;
   DBGPRINTF ("ATCmdProcReadCSCS - unprocessed");
   return bSuccess;
}

static boolean AEEBTAG_ATCmdProcQueryCSCS (IBTAudioGateway* pMe, char* args[])
{
   boolean bSuccess = FALSE;
   DBGPRINTF ("ATCmdProcQueryCSCS - unprocessed");
   return bSuccess;
}

static boolean AEEBTAG_ATCmdProcSetCSCS (IBTAudioGateway* pMe, char* args[])
{
   boolean bSuccess = FALSE;
   DBGPRINTF ("ATCmdProcSetCSCS - unprocessed");
   return bSuccess;
}

static boolean AEEBTAG_ATCmdProcCSQ (IBTAudioGateway* pMe, char* args[])
{
   boolean bSuccess = FALSE;
   DBGPRINTF ("ATCmdProcCSQ - unprocessed");
   return bSuccess;
}

static boolean AEEBTAG_ATCmdProcCBC (IBTAudioGateway* pMe, char* args[])
{
   boolean bSuccess = FALSE;
   DBGPRINTF ("ATCmdProcCBC - unprocessed");
   return bSuccess;
}

static boolean AEEBTAG_ATCmdProcCIMI (IBTAudioGateway* pMe, char* args[])
{
   boolean bSuccess = FALSE;
   DBGPRINTF ("ATCmdProcCIMI - unprocessed");

   return bSuccess;
}

static boolean AEEBTAG_ATCmdProcBTRH (IBTAudioGateway* pMe, char* args[])
{
   // not supported; must send OK back
   OEMBTAG_SendResponse (pMe->pOEMAG, "OK");
   return TRUE;
}

static boolean AEEBTAG_ATCmdProcBLDN (IBTAudioGateway* pMe, char* args[])
{
   DBGPRINTF ("ATCmdProcBLDN - entered");

   if (FALSE == AEEBTAG_Redial (pMe))
   {
      AEEBTAG_SendErrResponse (pMe, AEEBTAG_CME_AG_FAILURE);
   }
   else
   {
      OEMBTAG_SendResponse (pMe->pOEMAG, "OK");
   }
   return TRUE; // don't send error
}

static boolean AEEBTAG_ATCmdProcMemDial (IBTAudioGateway* pMe, char* args[])
{
   DBGPRINTF ("ATCmdProcMemDial - mem=%s", args[0]);

   if (FALSE == AEEBTAG_MemDial (pMe, args[0]))
   {
      AEEBTAG_SendErrResponse (pMe, AEEBTAG_CME_AG_FAILURE);
   }
   else
   {
      OEMBTAG_SendResponse (pMe->pOEMAG, "OK");
   }
   return TRUE; // don't send error
}

static void AEEBTAG_HandleCallOrig (IBTAudioGateway* pMe)
{
   DBGPRINTF ("HndlCallOrig - numCalls=%d", pMe->CallMgr.piCallList->dwCount);

   if (IS_CONNECTABLE() != FALSE)
   {
      if ((pMe->CallMgr.uFlags & AEEBTAG_FLAG_CALLMGR_DEV_DIALED) == 0)
      {
         if ((pMe->dCfgFlags & AEEBTAG_CFG_PRIVATE_B) == 0)
         {
            // call audio should be on headset if private mode is not selected
            (void) AEEBTAG_StartSCO (pMe);
         }
         else if (1 == pMe->CallMgr.piCallList->dwCount)
         {
            // since this call made by AG call audio should be on AG if:
            // 1. private mode is selected, and
            // 2. this call is not part of a multi-party call
            (void)AEEBTAG_StopSCO (pMe);
         }
         // else, leave it alone
      }
      // else, since call is made by headset, audio would have already 
      // been brought up; leave it alone
   }
   pMe->CallMgr.uFlags &= ~AEEBTAG_FLAG_CALLMGR_DEV_DIALED;
}

static void AEEBTAG_HandleCallIncom (IBTAudioGateway* pMe, AEECallDesc cd)
{
   DBGPRINTF ("HndlCallIncom - numCalls=%d", pMe->CallMgr.piCallList->dwCount);

   if (IS_CONNECTABLE() != FALSE)
   {
#ifndef BT_SIMULATION
      nv_item_type nvi;

      if ((pMe->CallMgr.piCallList->dwCount == 1) &&
          (NV_DONE_S == OEMNV_Get (NV_AUTO_ANSWER_I, &nvi)) &&
          (FALSE != nvi.auto_answer.enable))
      {
         ISHELL_SetTimer (pMe->piShell, AEEBTAG_AUTOANSWER_TIME,
                          (PFNNOTIFY) AEEBTAG_AutoAnswerCall, pMe);
      }
#endif
   }
}

static void AEEBTAG_HandleCallAnswer (IBTAudioGateway* pMe)
{
   DBGPRINTF ("HndlCallAnswer - numCalls=%d", pMe->CallMgr.piCallList->dwCount);

   if (IS_CONNECTABLE() != FALSE)
   {
      OEMBTAG_StopRingAlert (pMe->pOEMAG);

      ISHELL_CancelTimer (pMe->piShell, 
                          (PFNNOTIFY) AEEBTAG_AutoAnswerCall, pMe);

      if ((pMe->CallMgr.uFlags & AEEBTAG_FLAG_CALLMGR_DEV_ANSWERED) ||
          ((pMe->dCfgFlags & AEEBTAG_CFG_PRIVATE_B) == 0))
      {
         if ((AEEBTAG_STATE_CONNECTED == pMe->dState) ||
             ((pMe->dCfgFlags & AEEBTAG_CFG_NO_AUTO_RECONN_B) == 0))
         {
            (void) AEEBTAG_StartSCO (pMe);
         }
      }
      else if (((pMe->CallMgr.uFlags & AEEBTAG_FLAG_CALLMGR_DEV_ANSWERED) == 0) &&
               (pMe->dCfgFlags & AEEBTAG_CFG_PRIVATE_B))
      {
         (void) AEEBTAG_StopSCO (pMe);
      }
      // leave audio alone for all other conditions
   }
   pMe->CallMgr.uFlags &= ~AEEBTAG_FLAG_CALLMGR_DEV_ANSWERED;
}

static void AEEBTAG_HandleCallEnd (IBTAudioGateway* pMe)
{
   DBGPRINTF ("HndlCallEnd - numCalls=%d", pMe->CallMgr.piCallList->dwCount);

   pMe->CallMgr.uFlags &= ~AEEBTAG_FLAG_CALLMGR_DEV_DIALED;
   pMe->CallMgr.uFlags &= ~AEEBTAG_FLAG_CALLMGR_DEV_ANSWERED;
   pMe->CallMgr.uFlags &= ~AEEBTAG_FLAG_CALLMGR_DEV_ENDED;
   OEMBTAG_StopRingAlert (pMe->pOEMAG);

   if (pMe->CallMgr.piCallList->dwCount == 1) // last call is ending
   {
      (void) AEEBTAG_StopSCO (pMe);
   }
}

static void AEEBTAG_HandleCallEvent (IBTAudioGateway* pMe, ModelEvent* pEvent)
{
   AEETCallEvent *pCallEvent = (AEETCallEvent *)(void *)pEvent;

   if ((NULL != pMe) && (pgTheOnlyAG == pMe))
   {
      if (ITELEPHONE_GetCalls (pMe->CallMgr.piPhone, pMe->CallMgr.piCallList,
                               pMe->CallMgr.uCallListSize) != SUCCESS)
      {
         DBGPRINTF_ERROR ("HndlCallEv - failed to get call list. "
                          "All bets are off!");
      }

      switch (pCallEvent->evCode)
      {
         case AEET_EVENT_CALL_ORIG:
         {
            AEEBTAG_HandleCallOrig (pMe);
            break;
         }
         case AEET_EVENT_CALL_INCOM:
         {
            AEEBTAG_HandleCallIncom (pMe, pCallEvent->call.cd);
            break;
         }
         case AEET_EVENT_CALL_ANSWER:
         {
            AEEBTAG_HandleCallAnswer (pMe);
            break;
         }
         case AEET_EVENT_CALL_END:
         {
            AEEBTAG_HandleCallEnd (pMe);
            break;
         }
         default:
         {
            break;
         }
      }
   }
}

static void AEEBTAG_VolumeChangeListener(
   IBTAudioGateway *pMe,
   ModelEvent *pEvent
)
{
#ifndef BT_SIMULATION
   nv_item_type nvi;
   if (NV_DONE_S == OEMNV_Get (NV_EAR_LVL_SHADOW_I, &nvi))

   if ((NULL != pMe) && (NULL != pEvent)) 
   {
      DBGPRINTF ("VolChgListener - vol=%d ev=0x%x", 
                 nvi.ear_lvl_shadow, pEvent->evCode);
      if ((EVT_MDL_GENERIC_CHANGE == pEvent->evCode) &&
          (CFGI_EAR_VOL == (AEEConfigItem) pEvent->dwParam))
      {
         DBGPRINTF ("VolChgListener - vol=%d", nvi.ear_lvl_shadow);
         // convert to HS volume (0-15) and send to HS
      }
   }
#endif
}

static void AEEBTAG_HandleSoundEv (IBTAudioGateway* pMe, OEMBTSoundType snd)
{
   AEETCallState callState;

   if ((AEEBTAG_STATE_CONNECTED != pMe->dState) &&
       (pMe->dCfgFlags & AEEBTAG_CFG_NO_AUTO_RECONN_B))
   {
      DBGPRINTF ("HndlSoundEv - dropping event");
      return; // dropping this sound event
   }

   callState = AEEBTAG_GetCallState (pMe);

   DBGPRINTF ("HndlSoundEv - sEv=%d cSt=%d", snd, callState );

   if (AEET_CALL_STATE_INCOM == callState)
   {
      switch (snd)
      {
         case OEMBT_SND_TYPE_NONE:
            // AVS is indicating it's going to stop playback
            // but we're expecting some type of audio playback for ring.
            // What to do??? maybe wait for next playback notification?
            break;
         case OEMBT_SND_TYPE_BT_ALL:
         case OEMBT_SND_TYPE_BT_SCO:
            // do inband
            OEMBTAG_SetInband (pMe->pOEMAG, TRUE);
            pMe->uFlags |= AEEBTAG_FLAG_SEND_RING;
            (void) AEEBTAG_StartSCO (pMe);
            break;
         case OEMBT_SND_TYPE_OTHER_START:
         case OEMBT_SND_TYPE_BT_OTHER:
         case OEMBT_SND_TYPE_DTMF_RING_START:
            OEMBTAG_SendRingAlert (pMe->pOEMAG);
            break;
      }
   }
   else
   {
      switch (snd)
      {
         case OEMBT_SND_TYPE_NONE:
            if (AEET_CALL_STATE_NONE != callState)
            {
               (void) AEEBTAG_StartSCO (pMe);
            }
            else if ((pMe->CallMgr.uFlags & AEEBTAG_FLAG_CALLMGR_DEV_DIALED) == 0)
            {
               DBGPRINTF ("Setting Stop SCO timer for %x ms",
                          AEEBTAG_AUDIO_DISC_DELAY_TIME);
               ISHELL_SetTimer (pMe->piShell, AEEBTAG_AUDIO_DISC_DELAY_TIME, 
                                (PFNNOTIFY) AEEBTAG_StopSCO, pMe );
            }
            break;
         case OEMBT_SND_TYPE_BT_OTHER:
            (void) AEEBTAG_StopSCO (pMe);
            break;
         case OEMBT_SND_TYPE_BT_SCO:
         case OEMBT_SND_TYPE_BT_ALL:
            if ((pMe->uFlags & AEEBTAG_FLAG_A2DP_CONNECTED) == 0)
            {
               (void) AEEBTAG_StartSCO (pMe);
            }
            break;
         case OEMBT_SND_TYPE_DTMF_START:
            if (((pMe->uFlags & AEEBTAG_FLAG_A2DP_STREAMING) == 0) &&
                (AEEBTAG_CFG_SEND_KEYBEEP_B & pMe->dCfgFlags))
            {
               DBGPRINTF ("Starting SCO for keybeep");
               if(AEE_SUCCESS == AEEBTAG_StartSCO (pMe))
               {
                  pMe->uFlags |= AEEBTAG_FLAG_SCO_FOR_KEYBEEP;
               }
            }
            break;
         case OEMBT_SND_TYPE_DTMF_STOP:
            if (AEEBTAG_FLAG_SCO_FOR_KEYBEEP & pMe->uFlags)
            {
               DBGPRINTF ("Setting Stop SCO timer for %x ms",
                          AEEBTAG_AUDIO_DISC_DELAY_TIME*5);
               ISHELL_SetTimer (pMe->piShell, AEEBTAG_AUDIO_DISC_DELAY_TIME * 5, 
                                (PFNNOTIFY) AEEBTAG_StopSCO, pMe );
            }
            break;
         default:
            DBGPRINTF ("HndlSndEv - snd %d ignored", snd);
            break;
      }
   }
}

static BTAGConnection* AEEBTAG_FindConnectionEntry (
   IBTAudioGateway* pMe, 
   const BDAddress* pAddr,
   boolean bMustExist
)
{
   BTAGConnection* pConn = NULL;
   int i;
   for (i = 0; i < ARR_SIZE(pMe->sConnections); i++)
   {
      // connection entry exists?
      if ((AEEBT_BD_ADDRS_EQUAL (&pMe->sConnections[i].remoteAddr, 
                                 pAddr) != FALSE) &&
          (AEEBT_CONN_STATE_DISCONNECTED != pMe->sConnections[i].dConnState))
      {
         pConn = &pMe->sConnections[i];
         break;
      }
   }

   if ((NULL == pConn) && (FALSE == bMustExist))
   {
      // get a new connection entry
      for (i = 0; i < ARR_SIZE(pMe->sConnections); i++)
      {
         if (AEEBT_CONN_STATE_DISCONNECTED == pMe->sConnections[i].dConnState)
         {
            pConn = &pMe->sConnections[i];
            pConn->remoteAddr = *pAddr;
            break;
         }
      }
   }
   return pConn;
}

static void AEEBTAG_ResolveActiveConn (
   IBTAudioGateway* pMe, 
   BTAGConnection*  pConn
)
{
   if (AEEBT_CONN_STATE_DISCONNECTED == pConn->dConnState)
   {
      int i;
      int nNumEntries = ARR_SIZE(pMe->sConnections);
      for (i = 0; ((i <= pMe->nNumConnections) && (i < nNumEntries)); i++)
      {
         if (&pMe->sConnections[i] == pConn)
         {
            if (i != pMe->nNumConnections)
            {
               // not the last entry, replace it with last entry
               pMe->sConnections[i] = pMe->sConnections[pMe->nNumConnections];
            }
            break;
         }
      }
   }
   else
   {
      if (pConn != &pMe->sConnections[0])
      {
         BTAGConnection tempConn = *pConn;
         // swap 'em
         AEEAG_LOCK();
         if (pMe->pOpConn == pConn)
         {
            pMe->pOpConn = &pMe->sConnections[0];
         }
         else if (pMe->pOpConn == &pMe->sConnections[0])
         {
            pMe->pOpConn = pConn;
         }
         *pConn = pMe->sConnections[0];
         pMe->sConnections[0] = tempConn;
         AEEAG_UNLOCK();
      }
   }
}

static void AEEBTAG_QueueEvent (
   IBTAudioGateway* pMe, 
   BTAGEventType evType,
   void* pParams
)
{
   BTAGEventType evDropped = AEEBTAG_EV_NONE;
   int nMaxEvCnt = ARR_SIZE (pMe->dPendingEvent);
   int nQEndIdx;

   if (pMe->nNumReadyEvents == nMaxEvCnt)
   {
      evDropped = pMe->dPendingEvent[pMe->nNextReadyEventIdx];
      pMe->nNumReadyEvents--;
      pMe->nNextReadyEventIdx++; // dropping the oldest event
      pMe->nNextReadyEventIdx = pMe->nNextReadyEventIdx % nMaxEvCnt;
   }
   nQEndIdx = (pMe->nNextReadyEventIdx + pMe->nNumReadyEvents) % nMaxEvCnt;
   pMe->dPendingEvent[nQEndIdx] = evType;
   pMe->nNumReadyEvents++;

   if (AEEBTAG_EV_NONE != evDropped)
   {
      DBGPRINTF_ERROR ("QueueEv - dropping event %d", evDropped);
   }
   ISignalBus_Strobe (pMe->piEvSignalBus);
}

static void AEEBTAG_HandleButtonPressEv (IBTAudioGateway* pMe)
{
   AEETCallState callState = AEEBTAG_GetCallState (pMe);

   DBGPRINTF ("HndlButtonPressEv - callSt=%d", callState);

   if ((AEET_CALL_STATE_NONE == callState) ||
       (AEET_CALL_STATE_IDLE & callState))
   {
       switch (pMe->dButtonAction)
       {
          case AEEBTAG_HSBUTTON_DIAL:
             (void) AEEBTAG_Dial (pMe, pMe->CallMgr.szStrToDial);
             break;
          case AEEBTAG_HSBUTTON_REDIAL:
             (void) AEEBTAG_Redial (pMe);
             break;
          case AEEBTAG_HSBUTTON_DEFER:
             AEEBTAG_QueueEvent (pMe, AEEBTAG_EV_BUTTON_PRESS, NULL);
             break;
          case AEEBTAG_HSBUTTON_VR:
             AEEBTAG_QueueEvent (pMe, AEEBTAG_EV_VR_ACT_RCVD, NULL);
             break;
          default:
             DBGPRINTF ("HndlButtonPressEv - ignored, buttonF=%d", 
                        pMe->dButtonAction);
             break;
       }
   }
   else if (AEET_CALL_STATE_INCOM & callState)
   {
      AEEBTAG_AnswerCall (pMe);
   }
   else if (AEET_CALL_STATE_CONV & callState)
   {
      if (AEEBTAG_STATE_AUDIO_CONNECTED == pMe->dState)
      {
         AEEBTAG_EndCall (pMe);
      }
      else // HS pulling audio
      {
         (void) AEEBTAG_StartSCO (pMe);
      }
   }
}

static void AEEBTAG_HandleSpkrGain (IBTAudioGateway* pMe, OEMAGEvent* pEv)
{
   if (pMe->uRemoteSpkrGain != pEv->u8Buf[0])
   {
      DBGPRINTF ("HndlSpkrGain - got %d", pEv->u8Buf[0]);
      if (AEEBTAG_STATE_AUDIO_CONNECTED == pMe->dState)
      {
        if (0 == pEv->u8Buf[0])
        {
          OEMBTAG_SetAudioMuting (pMe->pOEMAG, TRUE);
        }
        else if (0 == pMe->uRemoteSpkrGain)
        {
          // TODO: Shouldn't this actually set the volume?
          OEMBTAG_SetAudioMuting (pMe->pOEMAG, FALSE);
        }
      }
      pMe->uRemoteSpkrGain = pEv->u8Buf[0];
   }
}

static void AEEBTAG_HandleMicGain (IBTAudioGateway* pMe, OEMAGEvent* pEv)
{
   if (pMe->uRemoteMicGain != pEv->u8Buf[0])
   {
      pMe->uRemoteMicGain = pEv->u8Buf[0];
      DBGPRINTF ("HndlMicGain - got %d", pEv->u8Buf[0]);
   }
}

static void AEEBTAG_OpCmpltCleanup (IBTAudioGateway* pMe)
{
   uint32 uFlags;

   AEEAG_LOCK();
   pMe->dLastCmpltOperation = pMe->dPendingOperation;
   pMe->dPendingOperation   = AEEBTAG_OP_NONE;
   uFlags = pMe->uFlags;
   pMe->uFlags &= ~AEEBTAG_FLAG_OP_BY_ME;
   AEEAG_UNLOCK();

   if ((uFlags & AEEBTAG_FLAG_OP_BY_ME) == 0)
   {
     ISignalBus_Strobe (pMe->piOpSignalBus);
   }
}

static void AEEBTAG_HandleEnabledEv (IBTAudioGateway* pMe)
{
   AEETCallState callState = AEEBTAG_GetCallState (pMe);

   AEEBTAG_OpCmpltCleanup (pMe);
   pMe->dState = AEEBTAG_STATE_ENABLED;
   AEEBTAG_QueueEvent (pMe, AEEBTAG_EV_ENABLED, NULL);

   pMe->nNumConnections = 0;
   pMe->nConnIdx = 0;

   if ((AEET_CALL_STATE_NONE != callState) &&
       ((AEET_CALL_STATE_IDLE & callState) == 0) &&
       ((AEEBTAG_CFG_PRIVATE_B & pMe->dCfgFlags) == 0))
   {
      // there's an ative call, and private mode is not set
      (void) AEEBTAG_StartSCO (pMe);
   }
}

static void AEEBTAG_HandleDisabledEv (IBTAudioGateway* pMe)
{
   AEEBTAG_OpCmpltCleanup (pMe);
   pMe->nNumConnections = 0;
   pMe->dState = AEEBTAG_STATE_DISABLED;
   pMe->uFlags = 0;
   AEEBTAG_QueueEvent (pMe, AEEBTAG_EV_DISABLED, NULL);
}

static void AEEBTAG_HandleConnFailedEv (IBTAudioGateway* pMe, OEMAGEvent* pEv)
{
   BTAGConnection* pConn =
      AEEBTAG_FindConnectionEntry (pMe, &pEv->bdAddr, TRUE);

   if ((pConn == NULL) || 
       (AEEBT_CONN_STATE_CONNECTING != pConn->dConnState))
   {
      DBGPRINTF_ERROR ("HndlConnFailedEv - conn entry not found");
   }
   else
   {
      boolean bTryAgain = FALSE;
      if ((AEEBTAG_OP_CONNECT == pMe->dPendingOperation) &&
          (AEEBT_CONN_STATE_CONNECTING == pConn->dConnState))
      {
         if (pMe->uFlags & AEEBTAG_FLAG_OP_BY_ME)
         {
            if (++pMe->nConnAttempts < 3)
            {
               bTryAgain = TRUE;
            }
         }
         AEEBTAG_OpCmpltCleanup (pMe);
      }
      if (FALSE != bTryAgain)
      {
        AEEBTAG_StartACL (pMe); // keep trying
      }
      else
      {
        pConn->dConnState = AEEBT_CONN_STATE_DISCONNECTED;
      }
   }
}

static void AEEBTAG_HandleConnEv (IBTAudioGateway* pMe, OEMAGEvent* pEv)
{
   BTAGConnection* pConn =
      AEEBTAG_FindConnectionEntry (pMe, &pEv->bdAddr, FALSE);

   if (pConn == NULL)
   {
      // possible scenario: max conn supported is one, we're
      // trying to connect to dev A while dev B's trying to 
      // connect to us; simply reject the connection
      DBGPRINTF_ERROR ("HndlConnEv - rejecting connection");
      AEEBTAG_Disconnect (pMe, &pEv->bdAddr);
   }
   else
   {
      if ((AEEBTAG_OP_CONNECT == pMe->dPendingOperation) &&
          (AEEBT_CONN_STATE_CONNECTING == pConn->dConnState))
      {
         AEEBTAG_OpCmpltCleanup (pMe);
      }
      else
      {
         DBGPRINTF ("HndlConnEv - remote initiated conn");
      }
      pConn->dConnState = AEEBT_CONN_STATE_CONNECTED;
      pMe->nNumConnections++;
      pMe->nConnIdx = 0;
      pMe->nConnAttempts = 0;
      if (1 == pMe->nNumConnections)
      {
         pMe->dState = AEEBTAG_STATE_CONNECTED;
      }
      else
      {
         AEEBTAG_ResolveActiveConn (pMe, pConn);
      }
      AEEBTAG_QueueEvent (pMe, AEEBTAG_EV_CONNECTED, &pEv->bdAddr);

      if (pMe->uFlags & AEEBTAG_FLAG_BRINGING_UP_AUDIO)
      {
         (void) AEEBTAG_StartSCO (pMe);
      }
   }
}

static void AEEBTAG_HandleDisconnEv (IBTAudioGateway* pMe, OEMAGEvent* pEv)
{
   BTAGConnection* pConn =
      AEEBTAG_FindConnectionEntry (pMe, &pEv->bdAddr, TRUE);

   if ((pConn == NULL) ||
       (AEEBT_CONN_STATE_DISCONNECTED == pConn->dConnState))
   {
      DBGPRINTF_ERROR ("HndlDisconnEv - conn entry not found");
   }
   else
   {
      if ((AEEBTAG_OP_DISCONNECT == pMe->dPendingOperation) &&
          (AEEBT_CONN_STATE_DISCONNECTING == pConn->dConnState))
      {
         AEEBTAG_OpCmpltCleanup (pMe);
      }
      else if (AEEBTAG_OP_NONE == pMe->dPendingOperation)
      {
         DBGPRINTF ("HndlConnEv - remote initiated disconn");
      }
      pConn->dAudioConnState = AEEBT_CONN_STATE_DISCONNECTED;
      pConn->dConnState = AEEBT_CONN_STATE_DISCONNECTED;
      pMe->nNumConnections--;
      pMe->nConnIdx = 0;
      pMe->nConnAttempts = 0;
      if (0 == pMe->nNumConnections)
      {
         pMe->dState = AEEBTAG_STATE_ENABLED;
      }
      else
      {
         AEEBTAG_ResolveActiveConn (pMe, pConn);
      }
      AEEBTAG_QueueEvent (pMe, AEEBTAG_EV_DISCONNECTED, &pEv->bdAddr);
   }
}
static void AEEBTAG_HandleAudioConnFailedEv (
   IBTAudioGateway* pMe, 
   OEMAGEvent* pEv
)
{
   BTAGConnection* pConn = 
      AEEBTAG_FindConnectionEntry (pMe, &pEv->bdAddr, TRUE);

   if (pConn == NULL)
   {
      DBGPRINTF_ERROR ("HndlAudioDiscFailedEv - conn entry not found");
   }
   if ((AEEBTAG_OP_CONNECT_AUDIO == pMe->dPendingOperation) &&
       (AEEBT_CONN_STATE_CONNECTING == pConn->dAudioConnState))
   {
      AEEBTAG_OpCmpltCleanup (pMe);
      pConn->dAudioConnState = AEEBT_CONN_STATE_DISCONNECTED;
   }
   else
   {
      DBGPRINTF_ERROR ("HndlAudioDiscFailedEv - not initiated locally");
   }
}

static void AEEBTAG_HandleAudioDisconnEv (IBTAudioGateway* pMe, OEMAGEvent* pEv)
{
   BTAGConnection* pConn = 
      AEEBTAG_FindConnectionEntry (pMe, &pEv->bdAddr, TRUE);

   if ((pConn == NULL) || 
       (AEEBT_CONN_STATE_DISCONNECTED == pConn->dAudioConnState))
   {
      DBGPRINTF_ERROR ("HndlAudioDiscEv - conn entry not found");
   }
   else
   {
      if ((AEEBTAG_OP_DISCONNECT_AUDIO == pMe->dPendingOperation) &&
          (AEEBT_CONN_STATE_DISCONNECTING == pConn->dAudioConnState))
      {
         AEEBTAG_OpCmpltCleanup (pMe);
      }
      else
      {
         DBGPRINTF ("HndlAudioDiscEv - remote initiated audio disconn");
      }
      pMe->dState = AEEBTAG_STATE_CONNECTED;
      pConn->dAudioConnState = AEEBT_CONN_STATE_DISCONNECTED;
      AEEBTAG_QueueEvent (pMe, AEEBTAG_EV_AUDIO_DISCONNECTED, &pEv->bdAddr);

      pMe->uFlags &= ~AEEBTAG_FLAG_SCO_FOR_KEYBEEP;
      if (pMe->CallMgr.uFlags & AEEBTAG_FLAG_CALLMGR_DEV_ENDED)
      {
         AEEBTAG_EndCall (pMe);
      }
      AEEBTAG_ResumeA2DP (pMe);
   }
}

static void AEEBTAG_HandleAudioConnEv (IBTAudioGateway* pMe, OEMAGEvent* pEv)
{
   BTAGConnection* pConn = 
      AEEBTAG_FindConnectionEntry (pMe, &pEv->bdAddr, TRUE);

   if ((pConn == NULL) || 
       (AEEBT_CONN_STATE_CONNECTED == pConn->dAudioConnState))
   {
      DBGPRINTF_ERROR ("HndlAudioConnEv - conn entry not found");
   }
   else
   {
      AEETCallState callState = AEEBTAG_GetCallState (pMe);

      if ((AEEBTAG_OP_CONNECT_AUDIO == pMe->dPendingOperation) &&
          (AEEBT_CONN_STATE_CONNECTING == pConn->dAudioConnState))
      {
         AEEBTAG_OpCmpltCleanup (pMe);
         pMe->uFlags &= ~AEEBTAG_FLAG_BRINGING_UP_AUDIO;
      }
      else
      {
         DBGPRINTF ("HndlAudioConnEv - remote initiated audio conn");
      }
      pMe->dState = AEEBTAG_STATE_AUDIO_CONNECTED;
      pConn->dAudioConnState = AEEBT_CONN_STATE_CONNECTED;
      AEEBTAG_ResolveActiveConn (pMe, pConn);
      AEEBTAG_QueueEvent (pMe, AEEBTAG_EV_AUDIO_CONNECTED, &pEv->bdAddr);

      if (pMe->CallMgr.uFlags & AEEBTAG_FLAG_CALLMGR_DEV_ANSWERED)
      {
         pMe->uFlags &= ~AEEBTAG_FLAG_SCO_FOR_KEYBEEP;
         AEEBTAG_AnswerCall (pMe);
      }
      else if ((AEET_CALL_STATE_INCOM == callState) &&
          (pMe->uFlags & AEEBTAG_FLAG_SEND_RING))
      {
         pMe->uFlags &= ~AEEBTAG_FLAG_SCO_FOR_KEYBEEP;
         OEMBTAG_SendRingAlert (pMe->pOEMAG);
      }
      pMe->uFlags &= ~AEEBTAG_FLAG_SEND_RING;
   }
}

static void AEEBTAG_OEMEventCb (void* pUser)
{
   IBTAudioGateway* pMe = (IBTAudioGateway*)pUser;
   OEMAGEvent event;

   if ((pMe == NULL) || (pMe != pgTheOnlyAG) ||
       (NULL == pMe->piEventSignalCtl))
   {
      DBGPRINTF_ERROR ("OEMEventCb - NULL pointer!");
      return;
   }

   ISignalCtl_Enable (pMe->piEventSignalCtl);

   while (OEMBTAG_GetNextEvent (pMe->pOEMAG, &event) == AEE_SUCCESS)
   {
      DBGPRINTF ("OEMEventCb - got ev %d", event.type);

      switch (event.type)
      {
         case OEMBTAG_EV_ENABLED:
            AEEBTAG_HandleEnabledEv (pMe);
            break;
         case OEMBTAG_EV_DISABLED:
            AEEBTAG_HandleDisabledEv (pMe);
            break;
         case OEMBTAG_EV_CONNECT_FAILED:
            AEEBTAG_HandleConnFailedEv (pMe, &event);
            break;
         case OEMBTAG_EV_CONNECTED:
            AEEBTAG_HandleConnEv (pMe, &event);
            break;
         case OEMBTAG_EV_DISCONNECTED:
            AEEBTAG_HandleDisconnEv (pMe, &event);
            break;
         case OEMBTAG_EV_AUDIO_CONNECTED:
            AEEBTAG_HandleAudioConnEv (pMe, &event);
            break;
         case OEMBTAG_EV_AUDIO_DISCONNECTED:
            AEEBTAG_HandleAudioDisconnEv (pMe, &event);
            break;
         case OEMBTAG_EV_AUDIO_CONNECT_FAILED:
            AEEBTAG_HandleAudioConnFailedEv (pMe, &event);
            break;
         case OEMBTAG_EV_BUTTON_PRESS:
            AEEBTAG_HandleButtonPressEv (pMe);
            break;
         case OEMBTAG_EV_SPKR_GAIN:
            AEEBTAG_HandleSpkrGain (pMe, &event);
            break;
         case OEMBTAG_EV_MIC_GAIN:
            AEEBTAG_HandleMicGain (pMe, &event);
            break;
         case OEMBTAG_EV_DEV_PICKUP:
            AEEBTAG_AnswerCall (pMe);
            break;
         case OEMBTAG_EV_DEV_HANGUP:
            AEEBTAG_EndCall (pMe);
            break;
         case OEMBTAG_EV_DEV_DIAL:
            AEEBTAG_Dial (pMe, (char*)event.u8Buf);
            break;
         case OEMBTAG_EV_DEV_MEM_DIAL:
            AEEBTAG_MemDial (pMe, (char*)event.u8Buf);
            break;
         case OEMBTAG_EV_DEV_REDIAL:
            AEEBTAG_Redial (pMe);
            break;
         case OEMBTAG_EV_VR_ACT:
            AEEBTAG_QueueEvent (pMe, AEEBTAG_EV_VR_ACT_RCVD, NULL);
            break;
         case OEMBTAG_EV_SND_TYPE:
            AEEBTAG_HandleSoundEv (pMe, event.soundType);
            break;
         case OEMBTAG_EV_A2DP_CONNECTED:
            pMe->uFlags |= AEEBTAG_FLAG_A2DP_CONNECTED;
            break;
         case OEMBTAG_EV_A2DP_DISCONNECTED:
            pMe->uFlags &= ~AEEBTAG_FLAG_A2DP_CONNECTED;
            break;
         case OEMBTAG_EV_A2DP_START:
            pMe->uFlags |= AEEBTAG_FLAG_A2DP_STREAMING;
            pMe->uFlags &= ~AEEBTAG_FLAG_A2DP_SUSPENDED_BY_ME;
            (void) AEEBTAG_StopSCO (pMe);
            break;
         case OEMBTAG_EV_A2DP_SUSPEND:
            pMe->uFlags &= ~AEEBTAG_FLAG_A2DP_STREAMING;
            if (pMe->uFlags & AEEBTAG_FLAG_BRINGING_UP_AUDIO)
            {
               (void) AEEBTAG_StartSCO (pMe);
            }
            break;
         case OEMBTAG_EV_OP_CMPLT_SUCCESS:
            if (AEEBTAG_OP_CONFIG == event.op)
            {
               AEEBTAG_OpCmpltCleanup (pMe);
               pMe->dSecurity = pMe->dTempSecurity;
               AEEBTAG_QueueEvent (pMe, AEEBTAG_EV_CONFIG_UPDATED, NULL);
            }
            else
            {
               DBGPRINTF ("OEMEventCb - op %d started", event.op);
               // don't inform app; wait for resulting event
            }
            break;
         case OEMBTAG_EV_OP_CMPLT_FAILED:
         {
            if (event.op == pMe->dPendingOperation)
            {
               if (event.op == AEEBTAG_OP_CONFIG)
               {
                  AEEAG_LOCK();
                  pMe->uFlags |= AEEBTAG_FLAG_SEC_UPDATE_FAILED;
                  AEEAG_UNLOCK();
               }
               AEEBTAG_OpCmpltCleanup (pMe);
            }
            else
            {
               DBGPRINTF ("OEMEventCb - op=%d", event.op);
            }
            break;
         }
         default:
            DBGPRINTF_ERROR ("OEMEventCb - unprocessed ev %d", event.type);
            break;
      }
   }
}

static void AEEBTAG_ParseATCmd (IBTAudioGateway* pMe)
{
   char* pszParams [OEMBTAG_AT_CMD_MAX_PARAMS];
   char* pszATCmd = pMe->ATCmdHndlr.pszATCmd;
   char* pszEnd;
   int i, j;
   boolean bCmdValid;

   while (pszATCmd < (pMe->ATCmdHndlr.pszATCmd + pMe->ATCmdHndlr.uNumChars))
   {
      bCmdValid = FALSE;
      if ((pszEnd = std_strchr (pszATCmd, AEEBTAG_AT_CMD_TERM_CHAR)) != NULL)
      {
         *pszEnd = '\0';
         for (i = 0; i < ARR_SIZE (ATCmdTable); i++)
         {
            int len = std_strlen (ATCmdTable[i].pszCmdName);
            if (std_strncmp (ATCmdTable[i].pszCmdName, pszATCmd, len) == 0)
            {
               pMe->ATCmdHndlr.uParamsRcvd = 0;
               pszParams[0] = pszATCmd + len;
               if (len < (pszEnd - pszATCmd)) // params exist?
               {
                  for (j = 1; (j < OEMBTAG_AT_CMD_MAX_PARAMS) && 
                              (NULL !=pszParams[j-1]); j++)
                  {
                     pMe->ATCmdHndlr.uParamsRcvd++;
                     pszParams[j] = 
                        std_strchr (pszParams[j-1], 
                                    AEEBTAG_AT_CMD_PARAM_SEPARATOR);
                     if (NULL != pszParams[j])
                     {
                        *pszParams[j] = '\0';
                        pszParams[j]++;
                     }
                  }
               }
               if (pMe->ATCmdHndlr.uParamsRcvd < ATCmdTable[i].uNumParams)
               {
                  DBGPRINTF ("ParseATCmd - expected %d params, found %d",
                             ATCmdTable[i].uNumParams, 
                             pMe->ATCmdHndlr.uParamsRcvd);
               }
               else
               {
                  DBGPRINTF ("ParseATCmd - Processing \"%s\"", 
                             ATCmdTable[i].pszCmdName);
                  bCmdValid = ATCmdTable[i].procFunc (pMe, pszParams);
               }
               break;
            }
         }
      }

      if (FALSE == bCmdValid)
      {
         DBGPRINTF_ERROR ("ParseATCmd - dropping cmd \"%s\"", pszATCmd);
         AEEBTAG_SendErrResponse (pMe, AEEBTAG_CME_OP_NOT_SUPPORTED);
      }

      // move to next received command, if any
      if (NULL != pszEnd)
      {
         pszATCmd = pszEnd + 1;
      }
      else
      {
         pszATCmd = pMe->ATCmdHndlr.pszATCmd + pMe->ATCmdHndlr.uNumChars;
      }
   }
}

static void AEEBTAG_RetrieveATCmd (void* pUser)
{
   IBTAudioGateway* pMe = (IBTAudioGateway*)pUser;

   if (pMe == NULL)
   {
      DBGPRINTF_ERROR ("RetrieveATCmd - NULL pointer!");
      return;
   }

   pMe->ATCmdHndlr.uNumChars = OEMBTAG_AT_CMD_MAX_LEN;
   while (OEMBTAG_RetrieveATCmd (pMe->pOEMAG, pMe->ATCmdHndlr.pszATCmd,
                                 &pMe->ATCmdHndlr.uNumChars) == AEE_SUCCESS)
   {
      pMe->ATCmdHndlr.pszATCmd [pMe->ATCmdHndlr.uNumChars] = '\0';
      AEEBTAG_ParseATCmd (pMe);

      // reset
      pMe->ATCmdHndlr.uNumChars = OEMBTAG_AT_CMD_MAX_LEN;
   }
   ISignalCtl_Enable (pMe->ATCmdHndlr.piATCmdSignalCtl);
}

static void AEEBTAG_ATCmdParserCleanup (IBTAudioGateway* pMe)
{
   if (NULL != pMe->pOEMAG)
   {
      OEMBTAG_ATCmdParserCleanup (pMe->pOEMAG);
   }
   if (NULL != pMe->ATCmdHndlr.piATCmdSignal)
   {
      ISignal_Release (pMe->ATCmdHndlr.piATCmdSignal);
      pMe->ATCmdHndlr.piATCmdSignal = NULL;
   }
   if (NULL != pMe->ATCmdHndlr.piATCmdSignalCtl)
   {
      ISignalCtl_Detach (pMe->ATCmdHndlr.piATCmdSignalCtl);
      ISignalCtl_Release (pMe->ATCmdHndlr.piATCmdSignalCtl);
      pMe->ATCmdHndlr.piATCmdSignalCtl = NULL;
   }
   FREEIF (pMe->ATCmdHndlr.pszATCmd);
}

static int AEEBTAG_ATCmdParserInit (IBTAudioGateway* pMe)
{
   int result = AEE_EFAILED;

   if ((result = ISignalCBFactory_CreateSignal (
                   pMe->piSigCBF, AEEBTAG_RetrieveATCmd, pMe, 
                   &pMe->ATCmdHndlr.piATCmdSignal,
                   &pMe->ATCmdHndlr.piATCmdSignalCtl)) != SUCCESS)
   {
      DBGPRINTF_ERROR ("ATCmdParserInit - failed to create signal r=%d", 
                       result);
   }
   else if ((pMe->ATCmdHndlr.pszATCmd = 
             MALLOC (OEMBTAG_AT_CMD_MAX_LEN + 1)) == NULL)
   {
      DBGPRINTF_ERROR ("ATCmdParserInit - memory allocation failed");
      result = AEE_ENOMEMORY;
   }
   else
   {
      result = OEMBTAG_ATCmdParserInit (pMe->pOEMAG, 
                                        pMe->ATCmdHndlr.piATCmdSignal);
   }
   return result;
}

static void AEEBTAG_EventCbDeregister (IBTAudioGateway* pMe)
{
   if (NULL != pMe->pOEMAG)
   {
      OEMBTAG_EventCbDeregister (pMe->pOEMAG);
   }
   if (NULL != pMe->piEventSignal)
   {
      ISignal_Release (pMe->piEventSignal);
      pMe->piEventSignal = NULL;
   }
   if (NULL != pMe->piEventSignalCtl)
   {
      ISignalCtl_Detach (pMe->piEventSignalCtl);
      ISignalCtl_Release (pMe->piEventSignalCtl);
      pMe->piEventSignalCtl = NULL;
   }
}

// Registers with OEM layer to receive signal when AEE AG events are avail
static int AEEBTAG_EventCbRegister (IBTAudioGateway* pMe)
{
   int result;
   if ((result = ISignalCBFactory_CreateSignal (
                   pMe->piSigCBF, AEEBTAG_OEMEventCb, pMe,
                   &pMe->piEventSignal, &pMe->piEventSignalCtl)) != SUCCESS)
   {
      DBGPRINTF_ERROR ("EventCbRegister - failed to create signal r=%d",
                       result);
   }
   else
   {
      result = OEMBTAG_EventCbRegister (pMe->pOEMAG, pMe->piEventSignal);
   }
   return result;
}

static void AEEBTAG_AudioHandlerCleanup (IBTAudioGateway* pMe)
{
   if (NULL != pMe->pOEMAG)
   {
      OEMBTAG_AudioHandlerCleanup (pMe->pOEMAG);
   }
   if (NULL != pMe->piConfig)
   {
      LISTENER_Cancel (&pMe->configListener);
      ICONFIG_Release (pMe->piConfig);
      pMe->piConfig = NULL;
   }
}

static int AEEBTAG_AudioHandlerInit (IBTAudioGateway* pMe)
{
   int result = SUCCESS;
   IModel* piModel = NULL;

   if  (ISHELL_CreateInstance (pMe->piShell, AEECLSID_CONFIG, 
                                (void**)&pMe->piConfig) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AudioHandlerInit - failed to create IConfig obj");
      pMe->piConfig = NULL;
   }
   if ( (pMe->piConfig != NULL ) &&
        (ICONFIG_GetModel (pMe->piConfig, &piModel)) == SUCCESS)
   {
      result = IMODEL_AddListenerEx ( piModel, &pMe->configListener,
                                     (PFNLISTENER) AEEBTAG_VolumeChangeListener, 
                                     pMe);
      if (result != SUCCESS)
      {
         DBGPRINTF_ERROR ("AudioHandlerInit - failed to add volume listener");
      }
      if (NULL != piModel)
      {
         IMODEL_Release (piModel);
      }
   }
   result = OEMBTAG_AudioHandlerInit (pMe->pOEMAG);

   return result;
}

static void AEEBTAG_CallHandlerCleanup (IBTAudioGateway* pMe)
{
   FREEIF (pMe->CallMgr.piCallList);
   if (NULL != pMe->CallMgr.piCallMgr)
   {
      ICALLMGR_Release (pMe->CallMgr.piCallMgr);
      pMe->CallMgr.piCallMgr = NULL;
   }
   if (NULL != pMe->CallMgr.piPhone)
   {
      LISTENER_Cancel (&pMe->CallMgr.callListener);
      ITELEPHONE_Release (pMe->CallMgr.piPhone);
      pMe->CallMgr.piPhone = NULL;
   }
}

static int AEEBTAG_CallHandlerInit (IBTAudioGateway* pMe)
{
   int result;
   IModel* piModel = NULL;
   uint32 uCount;

   if ((result = ISHELL_CreateInstance (pMe->piShell, AEECLSID_TELEPHONE,
                                        (void**)&pMe->CallMgr.piPhone)) != SUCCESS)
   {
     DBGPRINTF_ERROR ("CallHandlerInit - ITelephone create instance failed");
     pMe->CallMgr.piPhone = NULL;
   }
   else if ((result = ISHELL_CreateInstance (
                        pMe->piShell, AEECLSID_CALLMGR,
                        (void**)&pMe->CallMgr.piCallMgr)) != SUCCESS)
   {
     DBGPRINTF_ERROR ("CallHandlerInit - ICallMgr create instance failed");
     pMe->CallMgr.piCallMgr = NULL;
   }
   else if ((result = ITELEPHONE_QueryInterface (pMe->CallMgr.piPhone, 
                                                 AEEIID_MODEL, 
                                                 (void**) &piModel)) != SUCCESS)
   {
      DBGPRINTF_ERROR ("CallHandlerInit - ITelephone does not support IModel");
   }
   else if ((result = IMODEL_AddListenerEx (
                        piModel, &pMe->CallMgr.callListener, 
                        (PFNLISTENER) AEEBTAG_HandleCallEvent, 
                        pMe)) != SUCCESS)
   {
      DBGPRINTF_ERROR ("CallHandlerInit - failed to add phone listener");
   }
   else if ((result = ITELEPHONE_GetMaxCallDescriptors (
                        pMe->CallMgr.piPhone, &uCount)) != SUCCESS)
   {
      DBGPRINTF_ERROR ("CallHandlerInit - failed to get max call descriptors");
   }
   else
   {
      pMe->CallMgr.uCallListSize = 
         sizeof(uint32) + (uCount * sizeof(AEECallDesc));
      pMe->CallMgr.piCallList = MALLOC (pMe->CallMgr.uCallListSize);
      result = (NULL == pMe->CallMgr.piCallList) ? AEE_ENOMEMORY : AEE_SUCCESS;
   }

   if (NULL != piModel)
   {
      IMODEL_Release (piModel);
   }
   return result;
}

static int AEEBTAG_UpdateSecurity (IBTAudioGateway* pMe, BTSecurityLevel dSec)
{
   int result = AEE_EFAILED;

   if ((NULL == pMe->piBT) &&
       (ISHELL_CreateInstance (pMe->piShell, AEECLSID_BT, 
                               (void**)&pMe->piBT) != SUCCESS))
   {
     DBGPRINTF_ERROR ("AG_UpdateSec - failed to create IBT");
   }
   else if ((NULL == pMe->piMgr) &&
            (IBT_GetObject (pMe->piBT, AEECLSID_BTMgr, NULL, 0,
                            (void**)&pMe->piMgr) != AEE_SUCCESS))
   {
     DBGPRINTF_ERROR ("AG_UpdateSec - failed to create IBTMgr");
   }
   else
   {
      BTServiceIdentifier  svc2protect = {
         AEEBT_SEC_BY_SERVICE_CLASS, 4, {0, 0, 0, 0, 0},
         {AEEBT_SCU_HEADSET, AEEBT_SCU_HEADSET_AUDIO_GATEWAY,
         AEEBT_SCU_HANDSFREE, AEEBT_SCU_HANDSFREE_AUDIO_GATEWAY} };
      if ((result = IBTMgr_SetSvcSecurity( pMe->piMgr, dSec, 
                                           &svc2protect)) != AEE_SUCCESS)
      {
         DBGPRINTF_ERROR ("AG_Config - failed to set sec r=%d", result);
      }
   }
   return result;
}

static void AEEBTAG_Cleanup (IBTAudioGateway* pMe)
{
   DBGPRINTF ("AEEBTAG_Cleanup - entered");

   AEEBTAG_CallHandlerCleanup (pMe);
   AEEBTAG_AudioHandlerCleanup (pMe);
   AEEBTAG_ATCmdParserCleanup (pMe);
   AEEBTAG_EventCbDeregister (pMe);

   if (NULL != pMe->pOEMAG)
   {
      if (AEEBT_SEC_NONE < pMe->dSecurity)
      {
         AEEBTAG_UpdateSecurity (pMe, AEEBT_SEC_NONE);
         // not going to wait for result
      }
      OEMBTAG_Release (pMe->pOEMAG);
      pMe->pOEMAG = NULL;
   }
   if (NULL != pMe->piMgr)
   {
      IBTMgr_Release (pMe->piMgr);
      pMe->piMgr = NULL;
   }
   if (NULL != pMe->piBT)
   {
      IBT_Release (pMe->piBT);
      pMe->piBT = NULL;
   }
   if (NULL != pMe->piEvSignalBus)
   {
      ISignalBus_Clear (pMe->piEvSignalBus);
      ISignalBus_Release (pMe->piEvSignalBus);
      pMe->piEvSignalBus = NULL;
   }
   if (NULL != pMe->piOpSignalBus)
   {
      ISignalBus_Clear (pMe->piOpSignalBus);
      ISignalBus_Release (pMe->piOpSignalBus);
      pMe->piOpSignalBus = NULL;
   }
   if (NULL != pMe->piSigCBF)
   {
      ISignalCBFactory_Release (pMe->piSigCBF);
      pMe->piSigCBF = NULL;
   }
   if (NULL != pMe->piShell)
   {
      ISHELL_Release (pMe->piShell);
      pMe->piShell = NULL;
   }
   FREEIF (pMe);
   pgTheOnlyAG = NULL;
}

static int AEEBTAG_Init (IBTAudioGateway* pMe, IShell* piShell)
{
   int result = AEE_EFAILED;

   DBGPRINTF ("AEEBTAG_Init - entered");

   pgTheOnlyAG  = pMe;
   pMe->pvt     = &gpvtIBTAudioGateway;
   pMe->piShell = piShell;
   pMe->uRefs   = 1;
   pMe->pAC     = AEE_GetAppContext();
   pMe->nNextReadyEventIdx = 0;
   pMe->dState  = AEEBTAG_STATE_DISABLED;
   ISHELL_AddRef (piShell);

   if (ISHELL_CreateInstance (piShell, AEECLSID_SignalBus,
                              (void**)&pMe->piEvSignalBus) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTAG_Init - failed to create Ev signal bus");
   }
   else if (ISHELL_CreateInstance (piShell, AEECLSID_SignalBus,
                                   (void**)&pMe->piOpSignalBus) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTAG_Init - failed to create Op signal bus");
   }
   else if (ISHELL_CreateInstance (piShell, AEECLSID_SignalCBFactory,
                                   (void**)&pMe->piSigCBF) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTAG_Init - failed to create signal factory");
   }
   else if (AEEBTAG_EventCbRegister (pMe) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTAG_Init - failed to register EvCb");
   }
   else if (AEEBTAG_CallHandlerInit (pMe) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTAG_Init - failed to init CallHandler");
   }
   else if (AEEBTAG_AudioHandlerInit (pMe) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTAG_Init - failed to init AudioHandler");
   }
   else if (AEEBTAG_ATCmdParserInit (pMe) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTAG_Init - failed to init ATCmdParser");
   }
   else
   {
      pMe->uFlags |= AEEBTAG_FLAG_OP_BY_ME;
      pMe->dSecurity = AEEBT_SEC_NONE;
      if (AEEBTAG_UpdateSecurity (pMe, pMe->dSecurity) == AEE_SUCCESS)
      {
         AEEBTAG_OpCmpltCleanup (pMe);
      }
      result = AEE_SUCCESS;
   }

   return result;
}

/************************************************************************ 
                             VTABLE FUNCTIONS
************************************************************************/
/*======================================================================= 
AEEBTAG_AddRef()
=======================================================================*/
static uint32 AEEBTAG_AddRef (IBTAudioGateway* pMe)
{
   // increment ref count
   if (pMe != NULL)
   {
      return ++pMe->uRefs;
   }
   return 0;
}

/*======================================================================= 
AEEBTAG_Release()
=======================================================================*/
static uint32 AEEBTAG_Release (IBTAudioGateway* pMe)
{
   uint32 uRefs;

   if (0 == pMe->uRefs)
   {
      return pMe->uRefs;
   }
   DBGPRINTF_MED ("AEEBTAG_Release - ref count = %d", pMe->uRefs);

   uRefs = --pMe->uRefs;

   if ( 0 == uRefs )
   {
      AEEBTAG_Cleanup (pMe);
   }
   
   return uRefs;
}

/*======================================================================= 
AEEBTAG_QueryInterface()
=======================================================================*/
static int AEEBTAG_QueryInterface (
   IBTAudioGateway* pMe, 
   AEECLSID clsID, 
   void** ppObj
)
{
   if (!AEE_CHECK_PTR("AEEBTAG_QueryInterface", ppObj, sizeof(void*), TRUE))
   {
     return EMEMPTR;
   }

   if ((AEEIID_IQI == clsID) ||
       (AEEIID_IBTAudioGateway == clsID) ||
       (AEECLSID_BTAudioGateway == clsID))
   {
     *ppObj = (IQueryInterface*)pMe;
     IQI_AddRef (*ppObj);
     return SUCCESS;
   }
   return ECLASSNOTSUPPORT;
}

/*======================================================================= 
AEEBTAG_OnOpResult()
=======================================================================*/
static int AEEBTAG_OnOpResult (IBTAudioGateway* pMe, ISignal* piSig)
{
   int result;
   if (NULL == piSig)
   {
      result = AEE_EBADPARM;
   }
   else
   {
      ISignalBus_Remove (pMe->piOpSignalBus, piSig);
      ISignalBus_Add (pMe->piOpSignalBus, piSig);
      result = AEE_SUCCESS;
   }
   return result;
}

/*======================================================================= 
AEEBTAG_GetOpResult()
=======================================================================*/
static int AEEBTAG_GetOpResult (
   IBTAudioGateway*   pMe, 
   BTAGOperationType* pdOp, 
   BTResult*          pdResult
)
{
   int result;
   uint32 uFlags;
   if (!AEE_CHECK_PTR ("AEEBTAG_GetOpResult", pdOp, 
                       sizeof(BTAGOperationType), TRUE) ||
       !AEE_CHECK_PTR ("AEEBTAG_GetOpResult", pdResult, 
                       sizeof(BTResult), TRUE))
   {
      result = AEE_EBADPARM;
   }
   else if (AEEBTAG_OP_NONE == pMe->dLastCmpltOperation)
   {
      result = AEE_ENOMORE;
   }
   else
   {
      AEEAG_LOCK();
      *pdOp = pMe->dLastCmpltOperation;
      pMe->dLastCmpltOperation = AEEBTAG_OP_NONE;
      uFlags = pMe->uFlags;
      pMe->uFlags &= ~AEEBTAG_FLAG_SEC_UPDATE_FAILED;
      AEEAG_UNLOCK();

      switch (*pdOp)
      {
         case AEEBTAG_OP_CONFIG:
            *pdResult = (uFlags & AEEBTAG_FLAG_SEC_UPDATE_FAILED) ?
               AEE_EFAILED : AEE_SUCCESS;
            break;
         case AEEBTAG_OP_ENABLE:
            *pdResult = (AEEBTAG_STATE_ENABLED <= pMe->dState) ?
               AEE_SUCCESS : AEE_EFAILED;
            break;
         case AEEBTAG_OP_DISABLE:
            *pdResult = (AEEBTAG_STATE_DISABLED == pMe->dState) ?
               AEE_SUCCESS : AEE_EFAILED;
            break;
         case AEEBTAG_OP_CONNECT:
            *pdResult = 
               (AEEBT_CONN_STATE_CONNECTED == pMe->pOpConn->dConnState) ?
               AEE_SUCCESS : AEE_EFAILED;
            break;
         case AEEBTAG_OP_DISCONNECT:
            *pdResult = 
               (AEEBT_CONN_STATE_DISCONNECTED == pMe->pOpConn->dConnState) ?
               AEE_SUCCESS : AEE_EFAILED;
            break;
         case AEEBTAG_OP_CONNECT_AUDIO:
            *pdResult = 
               (AEEBT_CONN_STATE_CONNECTED == pMe->pOpConn->dAudioConnState) ?
               AEE_SUCCESS : AEE_EFAILED;
            break;
         case AEEBTAG_OP_DISCONNECT_AUDIO:
            *pdResult = 
               (AEEBT_CONN_STATE_DISCONNECTED == pMe->pOpConn->dAudioConnState) ?
               AEE_SUCCESS : AEE_EFAILED;
            break;
      }
      result = AEE_SUCCESS;
   }
   return result;
}

/*======================================================================= 
AEEBTAG_OnEventNotify()
=======================================================================*/
static int AEEBTAG_OnEventNotify (IBTAudioGateway* pMe, ISignal* piSig)
{
   int result;
   if (NULL == piSig)
   {
      result = AEE_EBADPARM;
   }
   else
   {
      ISignalBus_Remove (pMe->piEvSignalBus, piSig);
      ISignalBus_Add (pMe->piEvSignalBus, piSig);
      result = AEE_SUCCESS;
   }
   return result;
}

/*======================================================================= 
AEEBTAG_GetEvent()
=======================================================================*/
static int AEEBTAG_GetEvent (
   IBTAudioGateway*  pMe, 
   BTAGEventType*    pdEvent
)
{
   int result;
   if (!AEE_CHECK_PTR ("AEEBTAG_GetEvent", pdEvent, 
                       sizeof(BTAGEventType), TRUE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      int nMaxEvCnt = ARR_SIZE (pMe->dPendingEvent);

      AEEAG_LOCK();
      if (pMe->nNumReadyEvents > 0)
      {
         *pdEvent = pMe->dPendingEvent[pMe->nNextReadyEventIdx];
         pMe->nNumReadyEvents--;
         pMe->nNextReadyEventIdx++;
         pMe->nNextReadyEventIdx = pMe->nNextReadyEventIdx % nMaxEvCnt;
         result = AEE_SUCCESS;
      }
      else
      {
         result = AEE_ENOMORE;
      }
      AEEAG_UNLOCK();
   }
   return result;
}

/*======================================================================= 
AEEBTAG_GetConnectionList()
=======================================================================*/
static int AEEBTAG_GetConnectionList (
   IBTAudioGateway* pMe, 
   BTAGConnection*  psConnList,
   int              nNumEntries,
   int*             pnNumConn
)
{
   int result;
   if (!AEE_CHECK_PTR ("AEEBTAG_GetConnectionList", psConnList, 
                       sizeof(BTAGConnection), TRUE) ||
       !AEE_CHECK_PTR ("AEEBTAG_GetConnectionList", pnNumConn, 
                       sizeof(int), TRUE))
   {
      result = AEE_EBADPARM;
   }
   else if (nNumEntries < pMe->nNumConnections)
   {
      *pnNumConn = pMe->nNumConnections;
      result = AEE_EBUFFERTOOSMALL;
   }
   else
   {
      *pnNumConn = pMe->nNumConnections;
      MEMCPY (psConnList, pMe->sConnections, 
              sizeof(BTAGConnection)*pMe->nNumConnections);
      result = AEE_SUCCESS;
   }
   return result;
}

/*======================================================================= 
AEEBTAG_GetStatus()
=======================================================================*/
static int AEEBTAG_GetStatus (IBTAudioGateway* pMe, BTAGStatus* psStatus)
{
   int result;
   if (!AEE_CHECK_PTR ("AEEBTAG_GetStatus", psStatus, 
                       sizeof(BTAGStatus), TRUE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      psStatus->dPendingOperation    = pMe->dPendingOperation;
      psStatus->dState               = pMe->dState;
      psStatus->nMaxNumConnSupported = OEMBTAG_MAX_CONNECTIONS;
      psStatus->nNumConnections      = pMe->nNumConnections;
      result = AEE_SUCCESS;
   }
   return result;
}

/*======================================================================= 
AEEBTAG_Config()
=======================================================================*/
static int AEEBTAG_Config (
   IBTAudioGateway*   pMe,
   BTAGConfigFlags    dFlags,
   BTAGButtonFunction dButtonAction,
   BTSecurityLevel    dSec,
   const char*        pszDialStr,
   const BDAddress*   psAddrList,
   int                nNumDevs
)
{
   int result;
   boolean bChangeSec = FALSE;

   if (AEEBTAG_OP_CONFIG == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTAG_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if ((nNumDevs > 0) && (NULL == psAddrList))
   {
      result = AEE_EBADPARM;
   }
   else if ((AEEBTAG_HSBUTTON_DEFER < dButtonAction) ||
            ((AEEBTAG_HSBUTTON_DIAL == dButtonAction) &&
             ((NULL == pszDialStr) || (std_strlen (pszDialStr) == 0))))
   {
      result = AEE_EBADPARM;
   }
   else if (dFlags & ~(AEEBTAG_CFG_PRIVATE_B | 
                       AEEBTAG_CFG_SEND_KEYBEEP_B |
                       AEEBTAG_CFG_NO_AUTO_RECONN_B |
                       AEEBTAG_CFG_VIDEOPHONE_ENABLED_B))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      if (dSec != pMe->dSecurity)
      {
         bChangeSec = TRUE;
      }
      pMe->dCfgFlags     = dFlags;
      pMe->dTempSecurity = dSec;
      pMe->dButtonAction = dButtonAction;
      pMe->nNumCfgDevs   = MIN (nNumDevs, ARR_SIZE (pMe->sAddrList));
      MEMCPY (pMe->sAddrList, psAddrList, pMe->nNumCfgDevs*sizeof(BDAddress));
      if (NULL != pszDialStr)
      {
         MEMCPY (pMe->CallMgr.szStrToDial, pszDialStr, 
                 sizeof (pMe->CallMgr.szStrToDial));
      }

      pMe->dPendingOperation = AEEBTAG_OP_CONFIG;
      if (FALSE != bChangeSec)
      {
         if ((result = AEEBTAG_UpdateSecurity (pMe, dSec)) == AEE_SUCCESS)
         {
            pMe->dSecurity = pMe->dTempSecurity;
            AEEBTAG_OpCmpltCleanup (pMe);
         }
      }
      else
      {
         result = AEE_SUCCESS;
         AEEBTAG_OpCmpltCleanup (pMe);
      }
   }
   return result;
}

/*======================================================================= 
AEEBTAG_ConfigEx()
=======================================================================*/
static int AEEBTAG_ConfigEx (
   IBTAudioGateway*  pMe,
   const char*       pszParamStr
)
{
   int result;
   int nLen;

   if (AEEBTAG_OP_CONFIG == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTAG_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if ((NULL == pszParamStr) || (0 == (nLen = std_strlen(pszParamStr))))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      const char* pszStr = pszParamStr;
      BTAGButtonFunction dTempButtonFunc;
      BDAddress sTempAddrList [AEEBTAG_MAX_DEVICES];
      int nTempNumCfgDevs;
      char szTempStrToDial [AEET_MAX_DIGITS_LENGTH] = "";
      BTAGConfigFlags dTempCfgFlags;

      // remember with current config data
      dTempButtonFunc = pMe->dButtonAction;
      dTempCfgFlags   = pMe->dCfgFlags;
      nTempNumCfgDevs = pMe->nNumCfgDevs;
      pMe->dTempSecurity = pMe->dSecurity;
      MEMCPY (szTempStrToDial, pMe->CallMgr.szStrToDial, 
              sizeof (szTempStrToDial));
      MEMCPY (sTempAddrList, pMe->sAddrList, sizeof(sTempAddrList));

      result = AEE_SUCCESS;
      while ((AEE_SUCCESS == result) && (pszStr < (pszParamStr + nLen)))
      {
         switch (pszStr[0])
         {
            case 'b': // button or bdaddress
               if (std_strnicmp (pszStr, "button=", 7) == 0)
               {
                  pszStr += 7;
                  if (std_strnicmp (pszStr, "defer;", 6) == 0)
                  {
                     dTempButtonFunc = AEEBTAG_HSBUTTON_DEFER;
                     pszStr += 6;
                  }
                  else if (std_strnicmp (pszStr, "dial;", 5) == 0)
                  {
                     dTempButtonFunc = AEEBTAG_HSBUTTON_DIAL;
                     pszStr += 5;
                  }
                  else if (std_strnicmp (pszStr, "redial;", 7) == 0)
                  {
                     dTempButtonFunc = AEEBTAG_HSBUTTON_REDIAL;
                     pszStr += 7;
                  }
                  else if (std_strnicmp (pszStr, "vr;", 3) == 0)
                  {
                     dTempButtonFunc = AEEBTAG_HSBUTTON_VR;
                     pszStr += 3;
                  }
                  else if (std_strnicmp (pszStr, "ignore;", 7) == 0)
                  {
                     dTempButtonFunc = AEEBTAG_HSBUTTON_IGNORE;
                     pszStr += 7;
                  }
                  else
                  {
                     result = AEE_EBADPARM;
                  }
               }
               else if (std_strnicmp (pszStr, "bdaddr=", 7) == 0)
               {
                  BDAddress bdAddr;
                  pszStr += 7;
                  while (OEMBT_StrToBDAddr (pszStr, &bdAddr) == AEE_SUCCESS)
                  {
                     // TBD: maybe shouldn't consider all-zero bd address valid
                     sTempAddrList [nTempNumCfgDevs++] = bdAddr;
                     pszStr += 12;
                     if (',' == *pszStr)
                     {
                        // another address present
                        pszStr++; // skip the ','
                     }
                     else if (';' == *pszStr)
                     {
                        // no more bd address
                        pszStr++;
                        break;
                     }
                     else
                     {
                        result = AEE_EBADPARM;
                     }
                  }
               }
               else
               {
                  DBGPRINTF_ERROR ("ConfigEx - failed to extract BDAddr");
                  result = AEE_EBADPARM;
               }
               break;
            case 'd': // dialstr
               if (std_strnicmp (pszStr, "dialstr=", 8) == 0)
               {
                  const char* pStart;
                  const char* pEnd;
                  pStart = std_strchr (pszStr, '\'');
                  if (NULL != pStart)
                  {
                     pStart++;
                     pEnd = std_strchr (pStart, '\'');
                     if ((NULL != pEnd) && (';' == pEnd[1]))
                     {
                        pszStr = pEnd + 2;
                        if (std_strlcpy (szTempStrToDial, pStart, 
                                         sizeof(szTempStrToDial)) > 
                            sizeof(szTempStrToDial))
                        {
                           result = AEE_EBADPARM;
                        }
                        else
                        {
                           szTempStrToDial[pEnd - pStart] = '\0';
                        }
                     }
                  }
                  else
                  {
                     result = AEE_EBADPARM;
                  }
               }
               else
               {
                  result = AEE_EBADPARM;
               }
               break;
            case 'f': // flags
               if (std_strnicmp (pszStr, "flags=", 6) == 0)
               {
                  dTempCfgFlags = 0;
                  pszStr += 6;
                  do
                  {
                     if (std_strnicmp (pszStr, "none;", 5) == 0)
                     {
                        pszStr += 5;
                        break;
                     }
                     if (std_strnicmp (pszStr, "private", 7) == 0)
                     {
                        dTempCfgFlags |= AEEBTAG_CFG_PRIVATE_B;
                        pszStr += 7;
                     }
                     else if (std_strnicmp (pszStr, "keybeep", 7) == 0)
                     {
                        dTempCfgFlags |= AEEBTAG_CFG_SEND_KEYBEEP_B;
                        pszStr += 7;
                     }
                     else if (std_strnicmp (pszStr, "call_connect", 12) == 0)
                     {
                        dTempCfgFlags |= AEEBTAG_CFG_NO_AUTO_RECONN_B;
                        pszStr += 12;
                     }
                     else if (std_strnicmp (pszStr, "vp", 2) == 0)
                     {
                        dTempCfgFlags |= AEEBTAG_CFG_VIDEOPHONE_ENABLED_B;
                        pszStr += 2;
                     }
                     else
                     {
                        result = AEE_EBADPARM;
                     }

                     if (',' == *pszStr)
                     {
                        // another flag present
                        pszStr++; // skip the ','
                     }
                     else if (';' == *pszStr)
                     {
                        // no more flag
                        pszStr++;
                        break;
                     }
                     else
                     {
                        result = AEE_EBADPARM;
                     }

                  } while (AEE_SUCCESS == result);
               }
               break;
            case 's': // sec
               if (std_strnicmp (pszStr, "sec=", 4) == 0)
               {
                  pszStr += 4;
                  if (std_strnicmp (pszStr, "none;", 5) == 0)
                  {
                     pMe->dTempSecurity = AEEBT_SEC_NONE;
                     pszStr += 5;
                  }
                  else if (std_strnicmp (pszStr, "auth;", 5) == 0)
                  {
                     pMe->dTempSecurity = AEEBT_SEC_AUTHENTICATE;
                     pszStr += 5;
                  }
                  else if (std_strnicmp (pszStr, "auth+encrypt;", 13) == 0)
                  {
                     pMe->dTempSecurity = AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT;
                     pszStr += 13;
                  }
                  else
                  {
                     result = AEE_EBADPARM;
                  }
               }
               break;
            default:
               result = AEE_EBADPARM;
         }
      }

      if ((AEE_SUCCESS == result) &&
          ((AEEBTAG_HSBUTTON_DIAL == dTempButtonFunc) && 
           (std_strlen (szTempStrToDial) == 0)))
      {
         result = AEE_EBADPARM;
      }
      else if (AEE_SUCCESS == result)
      {
         boolean bChangeSec = FALSE;

         DBGPRINTF ("ConfigEx - cfgFlags=0x%x button=%d #cfgDevs=%d", 
                    dTempCfgFlags, dTempButtonFunc, nTempNumCfgDevs);


         if (pMe->dTempSecurity != pMe->dSecurity)
         {
            bChangeSec = TRUE;
         }
         pMe->dCfgFlags     = dTempCfgFlags;
         pMe->dButtonAction = dTempButtonFunc;
         pMe->nNumCfgDevs   = MIN (nTempNumCfgDevs, ARR_SIZE (pMe->sAddrList));
         std_memmove (pMe->sAddrList, sTempAddrList, 
                      pMe->nNumCfgDevs*sizeof(BDAddress));
         if (std_strlen (szTempStrToDial) > 0)
         {
            std_memmove (pMe->CallMgr.szStrToDial, szTempStrToDial, 
                         sizeof (pMe->CallMgr.szStrToDial));
         }

         pMe->dPendingOperation = AEEBTAG_OP_CONFIG;
         if (FALSE != bChangeSec)
         {
            /*
            if ((result = OEMBTAG_UpdateSecurity (
                            pMe->pOEMAG, pMe->dTempSecurity)) != AEE_SUCCESS)
            {
               pMe->dPendingOperation = AEEBTAG_OP_NONE;
            }
            */
            if ((result = AEEBTAG_UpdateSecurity (
                            pMe, pMe->dTempSecurity)) == AEE_SUCCESS)
            {
               pMe->dSecurity = pMe->dTempSecurity;
               AEEBTAG_OpCmpltCleanup (pMe);
            }
         }
         else
         {
            result = AEE_SUCCESS;
            AEEBTAG_OpCmpltCleanup (pMe);
         }
      }
   }
   return result;
}

/*======================================================================= 
AEEBTAG_Enable()
=======================================================================*/
static int AEEBTAG_Enable (IBTAudioGateway* pMe)
{
   int result;

   DBGPRINTF ("AG_Enable - st=%d #devCfg=%d", pMe->dState, pMe->nNumCfgDevs);

   if (AEEBTAG_OP_ENABLE == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTAG_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if (AEEBTAG_STATE_DISABLED != pMe->dState)
   {
      result = AEE_EBADSTATE;
   }
   else
   {
      pMe->dPendingOperation = AEEBTAG_OP_ENABLE;
      if ((result = OEMBTAG_Enable (pMe->pOEMAG)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTAG_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTAG_Disable()
=======================================================================*/
static int AEEBTAG_Disable (IBTAudioGateway* pMe)
{
   int result;

   DBGPRINTF ("AG_Disable - st=%d", pMe->dState);

   if (AEEBTAG_OP_DISABLE == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTAG_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if (AEEBTAG_STATE_DISABLED == pMe->dState)
   {
      result = AEE_EBADSTATE;
   }
   else
   {
      pMe->dPendingOperation = AEEBTAG_OP_DISABLE;
      if ((result = OEMBTAG_Disable (pMe->pOEMAG)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTAG_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTAG_Connect()
=======================================================================*/
static int AEEBTAG_Connect (IBTAudioGateway* pMe, const BDAddress* pAddr)
{
   int result = AEE_EFAILED;

   DBGPRINTF ("AG_Connect - #conn=%d", pMe->nNumConnections);

   if (AEEBTAG_OP_CONNECT == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTAG_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if (AEEBTAG_STATE_DISABLED == pMe->dState)
   {
      result = AEE_EBADSTATE;
   }
   else if (pMe->nNumConnections == OEMBTAG_MAX_CONNECTIONS)
   {
      result = AEE_ENOMORE; // reached connection capacity
   }
   else if (!AEE_CHECK_PTR ("AEEBTAG_Connect", pAddr, sizeof(BDAddress), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->pOpConn = AEEBTAG_FindConnectionEntry (pMe, pAddr, FALSE);

      if (NULL == pMe->pOpConn)
      {
         DBGPRINTF_ERROR ("AG_Connect - conn DB full!");
         result = AEE_ENOMORE; // how is this possible???
      }
      else if (AEEBT_CONN_STATE_CONNECTED == pMe->pOpConn->dConnState)
      {
         result = AEE_EBADSTATE; // already connected to given device
      }
      else
      {
         pMe->dPendingOperation = AEEBTAG_OP_CONNECT;
         if ((result = OEMBTAG_Connect (
                         pMe->pOEMAG, 
                         &pMe->pOpConn->remoteAddr)) != AEE_SUCCESS)
         {
            pMe->dPendingOperation = AEEBTAG_OP_NONE;
         }
         else
         {
            pMe->pOpConn->dConnState = AEEBT_CONN_STATE_CONNECTING;
         }
         // don't increment connection count until CONNECTED ev
      }
   }
   return result;
}

/*======================================================================= 
AEEBTAG_Disconnect()
=======================================================================*/
static int AEEBTAG_Disconnect (IBTAudioGateway* pMe, const BDAddress* pAddr)
{
   int result;

   DBGPRINTF ("AG_Disconnect - #conn=%d", pMe->nNumConnections);

   if (AEEBTAG_OP_DISCONNECT == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTAG_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if (pMe->nNumConnections == 0)
   {
      result = AEE_ENOSUCH;
   }
   else if (!AEE_CHECK_PTR ("AEEBTAG_Disconnect", pAddr, 
                            sizeof(BDAddress), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->pOpConn = AEEBTAG_FindConnectionEntry (pMe, pAddr, TRUE);

      if (NULL == pMe->pOpConn)
      {
         DBGPRINTF_ERROR ("AG_Disconnect - conn entry not found!");
         result = AEE_ENOSUCH; // no ACL link to given device
      }
      else
      {
         pMe->dPendingOperation = AEEBTAG_OP_DISCONNECT;
         if ((result = OEMBTAG_Disconnect (
                         pMe->pOEMAG, 
                         &pMe->pOpConn->remoteAddr)) != AEE_SUCCESS)
         {
            pMe->dPendingOperation = AEEBTAG_OP_NONE;
         }
         else
         {
            pMe->pOpConn->dConnState = AEEBT_CONN_STATE_DISCONNECTING;
         }
      }
   }
   return result;
}

/*======================================================================= 
AEEBTAG_ConnectAudio()
=======================================================================*/
static int AEEBTAG_ConnectAudio (IBTAudioGateway* pMe, const BDAddress* pAddr)
{
   int result;

   DBGPRINTF ("AG_ConnectAudio - #conn=%d", pMe->nNumConnections);

   if (AEEBTAG_OP_CONNECT_AUDIO == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTAG_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if (AEEBTAG_STATE_AUDIO_CONNECTED == pMe->dState)
   {
      result = AEE_EBADSTATE; // audio link already up
   }
   else if (!AEE_CHECK_PTR ("AEEBTAG_ConnectAudio", pAddr, 
                            sizeof(BDAddress), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->pOpConn = AEEBTAG_FindConnectionEntry (pMe, pAddr, TRUE);

      if (NULL == pMe->pOpConn)
      {
         DBGPRINTF_ERROR ("AG_ConnectAudio - conn entry not found!");
         result = AEE_ENOSUCH; // no ACL link to given device
      }
      else
      {
         pMe->dPendingOperation = AEEBTAG_OP_CONNECT_AUDIO;
         if ((result = OEMBTAG_ConnectAudio (
                         pMe->pOEMAG, 
                         &pMe->pOpConn->remoteAddr)) != AEE_SUCCESS)
         {
            pMe->dPendingOperation = AEEBTAG_OP_NONE;
         }
         else
         {
            pMe->pOpConn->dAudioConnState = AEEBT_CONN_STATE_CONNECTING;
         }
      }
   }
   return result;
}

/*======================================================================= 
AEEBTAG_DisconnectAudio()
=======================================================================*/
static int AEEBTAG_DisconnectAudio (IBTAudioGateway* pMe, const BDAddress* pAddr)
{
   int result;

   DBGPRINTF ("AG_DisconnectAudio - #conn=%d", pMe->nNumConnections);

   if (AEEBTAG_OP_DISCONNECT_AUDIO == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTAG_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if (AEEBTAG_STATE_AUDIO_CONNECTED != pMe->dState)
   {
      result = AEE_EBADSTATE; // no audio link to bring down
   }
   else if (!AEE_CHECK_PTR ("AEEBTAG_DisconnectAudio", pAddr, 
                            sizeof(BDAddress), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->pOpConn = AEEBTAG_FindConnectionEntry (pMe, pAddr, TRUE);

      if (NULL == pMe->pOpConn)
      {
         DBGPRINTF_ERROR ("AG_DisconnectAudio - conn entry not found!");
         result = AEE_ENOSUCH; // no ACL link to give device
      }
      else if (AEEBT_CONN_STATE_CONNECTED != pMe->pOpConn->dAudioConnState)
      {
         result = AEE_ENOTALLOWED; // audio link is not with given device
      }
      else
      {
         pMe->dPendingOperation = AEEBTAG_OP_DISCONNECT_AUDIO;
         if ((result = OEMBTAG_DisconnectAudio (
                         pMe->pOEMAG, 
                         &pMe->pOpConn->remoteAddr)) != AEE_SUCCESS)
         {
            pMe->dPendingOperation = AEEBTAG_OP_NONE;
         }
         else
         {
            pMe->pOpConn->dAudioConnState = AEEBT_CONN_STATE_DISCONNECTING;
         }
      }
   }
   return result;
}

/*======================================================================= 
AEEBTAG_ReportVRState()
=======================================================================*/
static int AEEBTAG_ReportVRState (IBTAudioGateway* pMe, boolean bVRActive)
{
   return (OEMBTAG_ReportVRState (pMe->pOEMAG, bVRActive));
}

/************************************************************************ 
                             PUBLIC FUNCTIONS
************************************************************************/
/*======================================================================= 
AEEBTAG_New()
=======================================================================*/
int AEEBTAG_New (IShell* pIShell, void** ppif)
{
   int result;

   DBGPRINTF ("AEEBTAG_New - entered");

   if (ISHELL_CheckPrivLevel (pIShell, AEECLSID_BTAudioGateway, TRUE) == FALSE)
   {
      result = AEE_EPRIVLEVEL;
   }
   else if (!AEE_CHECK_PTR ("AEEBTAG_New", ppif, sizeof(void*), TRUE))
   {
      result =  AEE_EBADPARM;
   }
   else if (NULL != pgTheOnlyAG)
   {
      result =  AEE_ENOTALLOWED; // the only object has already been created
   }
   else
   {
      IBTAudioGateway *pMe = NULL;
      result = AEE_SUCCESS;

      if ((pMe = MALLOCREC (IBTAudioGateway)) == NULL)
      {
         result = AEE_ENOMEMORY;
      }
      else if (OEMBTAG_New (pIShell, &pMe->pOEMAG) != AEE_SUCCESS)
      {
         result = AEE_ENOMEMORY;
      }
      else
      {
         result = AEEBTAG_Init (pMe, pIShell);
      }

      if ((AEE_SUCCESS != result) && (NULL != pMe))
      {
         AEEBTAG_Cleanup (pMe);
      }
      else
      {
         *ppif = (void *)pgTheOnlyAG;
      }
   }

   return result;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */


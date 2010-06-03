/*=============================================================================
FILE:  AEEBTMgr.c

SERVICES:  Implementation of IBTMgr

GENERAL DESCRIPTION:
   Implements IBTAudioGateway.

===============================================================================
        Copyright (c) 2007-2009 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTMgr.c#3 $ 
$DateTime: 2009/03/13 17:22:06 $

when        who  what, where, why
----------  ---  --------------------------------------------------------------
2009-03-13  NM   Fixing issue with retrieving radio status
2009-01-15  NM   Fixing compilation issue on Non BMP build
2009-01-07  NM   Handling lisbon security level requirements for setting 
                 service security
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEE_OEM.h"
#include "AEE_OEMHeap.h"
#include "AEE_OEMDispatch.h"
#include "AEEBTMgr.bid"
#include "OEMBTMgr.h"
#include "AEECriticalSection.h"
#include "AEEISignal.h"
#include "AEEISignalCtl.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalCBFactory.h"
#include "AEEISignalBus.h"
#include "AEESignalBus.bid"

/*=============================================================================
 Macros 
=============================================================================*/
#define AEEBTMGR_LOCK()    AEECriticalSection_Enter(&pMe->criticalSection)
#define AEEBTMGR_UNLOCK()  AEECriticalSection_Leave(&pMe->criticalSection)

#define AEEBTMGR_FLAG_OP_BY_ME                  (1 << 0)

/*======================================================================= 
   Defines
=======================================================================*/
typedef struct
{
   BTMgrEventType       dType;
   BTResult             dResult;
}  BTMgrEvent;

struct IBTMgr
{
   const IBTMgrVtbl*    pvt;
   uint32               uRefs;
   uint32               uFlags;
   IShell*              piShell;
   ACONTEXT*            pAC;
   OEMBTMgr*            pOEMBTMgr;
   ISignalBus*          piEvSignalBus;
   AEECriticalSection   criticalSection;

   // state info
   BTLocalSettings      myInfo;
   BTServiceIdentifier  serviceSecurityInfo;
   BTRadioState         dRadioState;
   int                  nNumConnections;
   BTMgrOperationType   dPendingOperation;
   BTMgrOperationType   dLastCmpltOperation;
   BTMgrEvent           dPendingEvent[10];
   int                  nNumReadyEvents;
   int                  nNextReadyEventIdx;
   AEECallback          evSignalCb;    
   // for connection request
   ISignal*             piConnReqSignal; // does not need to switch context

};

static uint32  AEEBTMgr_AddRef (IBTMgr*);
static uint32  AEEBTMgr_Release (IBTMgr*);
static int     AEEBTMgr_QueryInterface (IBTMgr*, AEECLSID, void**);
static int     AEEBTMgr_OnEventNotify (IBTMgr*, ISignal*);
static int     AEEBTMgr_GetEvent (IBTMgr*, BTMgrEventType*, BTResult*);
static int     AEEBTMgr_GetRadioState (IBTMgr*, BTRadioState*);
static int     AEEBTMgr_GetLocalSettings (IBTMgr* p, 
                                          BTLocalSettingBitmap*,
                                          BTLocalSettings*);
static int     AEEBTMgr_SetName (IBTMgr*, const uint8*);
static int     AEEBTMgr_SetCoD (IBTMgr*, const BTClassOfDevice*);
static int     AEEBTMgr_SetSecurity (IBTMgr*, BTSecurityLevel);
static int     AEEBTMgr_SetPrefConnRole (IBTMgr* p, boolean);
static int     AEEBTMgr_SetRoleSwitchPolicy (IBTMgr* p, boolean);
static int     AEEBTMgr_SetSvcSecurity (IBTMgr*, 
                                        BTSecurityLevel,
                                        const BTServiceIdentifier*);
static int     AEEBTMgr_SetBlockedDevList (IBTMgr*, int, const BDAddress*);
static int     AEEBTMgr_OnConnRequest (IBTMgr*, ISignal*);
static int     AEEBTMgr_GetConnRequest (IBTMgr*, 
                                        BDAddress*, 
                                        BTServiceIdentifier*);
static int     AEEBTMgr_AuthorizeConn (IBTMgr*, 
                                       const BDAddress*,
                                       const BTServiceIdentifier*,
                                       boolean);
static int     AEEBTMgr_TuneScanPerformance (IBTMgr*, BTActVsPwr, BTActVsPwr);
static int     AEEBTMgr_DisableRadio (IBTMgr*);
static int     AEEBTMgr_EnableRadio (IBTMgr*);
static int     AEEBTMgr_DisableVisibility (IBTMgr*);
static int     AEEBTMgr_RestoreVisibility (IBTMgr*);

static const AEEVTBL(IBTMgr) gpvtIBTMgr =
{
   AEEBTMgr_AddRef,
   AEEBTMgr_Release,
   AEEBTMgr_QueryInterface,
   AEEBTMgr_OnEventNotify,
   AEEBTMgr_GetEvent,
   AEEBTMgr_GetRadioState,
   AEEBTMgr_GetLocalSettings,
   AEEBTMgr_SetName,
   AEEBTMgr_SetCoD,
   AEEBTMgr_SetSecurity,
   AEEBTMgr_SetPrefConnRole,
   AEEBTMgr_SetRoleSwitchPolicy,
   AEEBTMgr_SetBlockedDevList,
   AEEBTMgr_SetSvcSecurity,
   AEEBTMgr_OnConnRequest,
   AEEBTMgr_GetConnRequest,
   AEEBTMgr_AuthorizeConn,
   AEEBTMgr_TuneScanPerformance,
   AEEBTMgr_DisableRadio,
   AEEBTMgr_EnableRadio,
   AEEBTMgr_DisableVisibility,
   AEEBTMgr_RestoreVisibility
};

/************************************************************************ 
                           PRIVATE FUNCTIONS
************************************************************************/
static void AEEBTMgr_EvSignalCallback(void *pData)
{
   IBTMgr* pMe = (IBTMgr*)pData;
   if (NULL != pMe->piEvSignalBus)
   {
      ISignalBus_Strobe (pMe->piEvSignalBus);
   }
   return;
}
static void AEEBTMgr_QueueEvent (
   IBTMgr* pMe,
   BTMgrEventType dType,
   BTResult dResult
)
{
   if (NULL != pMe->piEvSignalBus)
   {
      BTMgrEventType evDropped = AEEBTMGR_EV_NONE;
      int nMaxEvCnt = ARR_SIZE (pMe->dPendingEvent);
      int nQEndIdx;

      if (pMe->nNumReadyEvents == nMaxEvCnt)
      {
         evDropped = pMe->dPendingEvent[pMe->nNextReadyEventIdx].dType;
         pMe->nNumReadyEvents--;
         pMe->nNextReadyEventIdx++; // dropping the oldest event
         pMe->nNextReadyEventIdx = pMe->nNextReadyEventIdx % nMaxEvCnt;
      }
      nQEndIdx = (pMe->nNextReadyEventIdx + pMe->nNumReadyEvents) % nMaxEvCnt;
      pMe->dPendingEvent[nQEndIdx].dType   = dType;
      pMe->dPendingEvent[nQEndIdx].dResult = dResult;
      pMe->nNumReadyEvents++;

      if (AEEBTMGR_EV_NONE != evDropped)
      {
         //DBGPRINTF_ERROR ("QueueEv - dropping event %d", evDropped);
      }
      CALLBACK_Cancel(&pMe->evSignalCb);
      CALLBACK_Init(&pMe->evSignalCb, AEEBTMgr_EvSignalCallback, pMe);
      AEE_SYS_RESUME(&pMe->evSignalCb);
   }
}

static void AEEBTMgr_OpCmpltCleanup (
   IBTMgr* pMe,
   BTMgrEventType dEv,
   BTResult dResult
)
{
   uint32 uFlags;

   AEEBTMGR_LOCK();
   pMe->dLastCmpltOperation = pMe->dPendingOperation;
   pMe->dPendingOperation   = AEEBTMGR_OP_NONE;
   uFlags = pMe->uFlags;
   pMe->uFlags &= ~AEEBTMGR_FLAG_OP_BY_ME;
   AEEBTMGR_UNLOCK();

   if ((uFlags & AEEBTMGR_FLAG_OP_BY_ME) == 0)
   {
      AEEBTMgr_QueueEvent (pMe, dEv, dResult);
   }
}

static void AEEBTMgr_SettingDone (
   IBTMgr* pMe, 
   BTMgrEventType dEvType,
   BTResult dResult)
{
   AEEBTMgr_OpCmpltCleanup (pMe, dEvType, dResult);
   if (AEEBT_RESULT_SUCCESS == dResult)
   {
      BTLocalSettings localSettings;
      OEMBTMgr_GetLocalSettings (pMe->pOEMBTMgr, &localSettings);
      if (MEMCMP (&localSettings, &pMe->myInfo, sizeof(BTLocalSettings)))
      {
         pMe->myInfo = localSettings;
         AEEBTMgr_QueueEvent (pMe, AEEBTMGR_EV_SETTINGS_CHANGED, dResult);
      }
   }
}

static void AEEBTMgr_EventCb (IBTMgr* pMe, OEMBTMgrEvent* pEv)
{
   BTMgrEventType aeeEv = AEEBTMGR_EV_NONE;

   /*DBGPRINTF ("Mgr_EvCb - pMe=%x op=%x ev=%d", 
              pMe, pMe->dPendingOperation, pEv->dType);*/

   if ((NULL != pMe) && (NULL != pEv))
   {
      switch (pEv->dType)
      {
         case OEMBTMGR_EV_SET_NAME_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_SET_NAME)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_SET_NAME_SUCCESS : AEEBTMGR_EV_SET_NAME_FAILED;
               AEEBTMgr_SettingDone (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_SET_COD_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_SET_COD)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_SET_COD_SUCCESS : AEEBTMGR_EV_SET_COD_FAILED;
               AEEBTMgr_SettingDone (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_SET_SEC_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_SET_SEC)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_SET_SEC_SUCCESS : AEEBTMGR_EV_SET_SEC_FAILED;
               AEEBTMgr_SettingDone (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_SET_ROLE_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_SET_ROLE)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_SET_ROLE_SUCCESS : AEEBTMGR_EV_SET_ROLE_FAILED;
               AEEBTMgr_SettingDone (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_SET_SW_POL_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_SET_ROLE_SW_POLICY)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_SET_SW_POL_SUCCESS : AEEBTMGR_EV_SET_SW_POL_FAILED;
               AEEBTMgr_SettingDone (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_SET_BLOCKED_DEVS_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_SET_BLOCKED_DEV_LIST)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_SET_BLOCKED_DEVS_SUCCESS : 
                  AEEBTMGR_EV_SET_BLOCKED_DEVS_FAILED;
               AEEBTMgr_SettingDone (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_PERF_TUNING_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_TUNE_PERF)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_PERF_TUNING_SUCCESS : AEEBTMGR_EV_PERF_TUNING_FAILED;
               AEEBTMgr_SettingDone (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_SET_SVC_SEC_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_SET_SERVICE_SEC)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_SET_SVC_SEC_SUCCESS : AEEBTMGR_EV_SET_SVC_SEC_FAILED;
               AEEBTMgr_OpCmpltCleanup (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_AUTH_CONN_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_AUTH_CONN)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_AUTH_CONN_SUCCESS : AEEBTMGR_EV_AUTH_CONN_FAILED;
               AEEBTMgr_OpCmpltCleanup (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_DISABLE_RADIO_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_DISABLE_RADIO)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_DISABLE_RADIO_SUCCESS : 
                  AEEBTMGR_EV_DISABLE_RADIO_FAILED;
               AEEBTMgr_OpCmpltCleanup (pMe, aeeEv, pEv->dResult);
            }
            break;
         case  OEMBTMGR_EV_ENABLE_RADIO_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_ENABLE_RADIO)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_ENABLE_RADIO_SUCCESS : 
                  AEEBTMGR_EV_ENABLE_RADIO_FAILED;
               AEEBTMgr_OpCmpltCleanup (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_DISABLE_VIS_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_DISABLE_VISIBILITY)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_DISABLE_VISIBILITY_SUCCESS :
                  AEEBTMGR_EV_DISABLE_VISIBILITY_FAILED;
               AEEBTMgr_OpCmpltCleanup (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_RESTORE_VIS_DONE:
            if (pMe->dPendingOperation == AEEBTMGR_OP_RESTORE_VISIBILITY)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTMGR_EV_RESTORE_VISIBILITY_SUCCESS :
                  AEEBTMGR_EV_RESTORE_VISIBILITY_FAILED;
               AEEBTMgr_OpCmpltCleanup (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_RADIO_OFF:
            AEEBTMgr_QueueEvent (pMe, AEEBTMGR_EV_RADIO_OFF, 
                                 AEEBT_RESULT_SUCCESS);
            break;
         case OEMBTMGR_EV_RADIO_ON:
            AEEBTMgr_QueueEvent (pMe, AEEBTMGR_EV_RADIO_ON, 
                                 AEEBT_RESULT_SUCCESS);
            break;
         case OEMBTMGR_EV_VIS_OFF:
            AEEBTMgr_QueueEvent (pMe, AEEBTMGR_EV_VISIBILITY_OFF, 
                                 AEEBT_RESULT_SUCCESS);
            break;
         case OEMBTMGR_EV_VIS_ON:
            AEEBTMgr_QueueEvent (pMe, AEEBTMGR_EV_VISIBILITY_ON, 
                                 AEEBT_RESULT_SUCCESS);
            break;
         case OEMBTMGR_EV_AUTH_REQ: // not exposed to app
         {
            BDAddress bdAddr;
            BTServiceIdentifier svcId;
            if (pMe->dPendingOperation != AEEBTMGR_OP_NONE)
            {
               //DBGPRINTF_ERROR ("BTMgr_OEMEventCb - busy, can't authorize!");
            }
            else if ((OEMBTMgr_GetConnRequest (pMe->pOEMBTMgr, &bdAddr, 
                                               &svcId) == AEE_SUCCESS) &&
                     (AEEBTMgr_AuthorizeConn (pMe, &bdAddr, &svcId, 
                                              TRUE) == AEE_SUCCESS))
            {
               pMe->uFlags |= AEEBTMGR_FLAG_OP_BY_ME;
            }
            else
            {
               //DBGPRINTF_ERROR ("BTMgr_OEMEventCb - failed to authorize conn!");
            }
            break;
         }
         default:
            //DBGPRINTF ("BTMgr_OEMEventCb - unprocessed ev %d", pEv->dType);
            break;
      }
   }
}

static void AEEBTMgr_Cleanup (IBTMgr* pMe)
{
   DBGPRINTF ("AEEBTMgr_Cleanup - entered");
   CALLBACK_Cancel(&pMe->evSignalCb);
   if (NULL != pMe->piEvSignalBus)
   {
      ISignalBus_Clear (pMe->piEvSignalBus);
      ISignalBus_Release (pMe->piEvSignalBus);
      pMe->piEvSignalBus = NULL;
   }
   if (NULL != pMe->piShell)
   {
      ISHELL_Release (pMe->piShell);
      pMe->piShell = NULL;
   }
   if (NULL != pMe->pOEMBTMgr)
   {
      if (NULL != pMe->piConnReqSignal)
      {
         OEMBTMgr_ConnRequestDeregister (pMe->pOEMBTMgr);
         pMe->piConnReqSignal = NULL;
      }
      OEMBTMgr_DeregisterEventCb (pMe->pOEMBTMgr, pMe);
      OEMBTMgr_Release (pMe->pOEMBTMgr);
      pMe->pOEMBTMgr = NULL;
   }
   FREEIF (pMe);
}

static int AEEBTMgr_Init (IBTMgr* pMe, IShell* piShell)
{
   int result = AEE_EFAILED;

   DBGPRINTF ("AEEBTMgr_Init - entered");

   pMe->pvt     = &gpvtIBTMgr;
   pMe->piShell = piShell;
   pMe->uRefs   = 1;
   pMe->pAC     = AEE_GetAppContext();
   pMe->nNextReadyEventIdx = 0;
   ISHELL_AddRef (piShell);

   if (OEMBTMgr_New (piShell, &pMe->pOEMBTMgr) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTMgr_Init - failed to create OEMBTMgr obj");
   }
   else if (OEMBTMgr_RegisterEventCb (pMe->pOEMBTMgr, AEEBTMgr_EventCb, 
                                      pMe) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTMgr_Init - failed to register EvCb");
   }
   else
   {
      result = AEE_SUCCESS;
   }

   return result;
}

/************************************************************************ 
                             VTABLE FUNCTIONS
************************************************************************/
/*======================================================================= 
AEEBTMgr_AddRef()
=======================================================================*/
static uint32 AEEBTMgr_AddRef (IBTMgr* pMe)
{
   // increment ref count
   if (pMe != NULL)
   {
      return ++pMe->uRefs;
   }
   return 0;
}

/*======================================================================= 
AEEBTMgr_Release()
=======================================================================*/
static uint32 AEEBTMgr_Release (IBTMgr* pMe)
{
   uint32 uRefs;

   if (0 == pMe->uRefs)
   {
      return pMe->uRefs;
   }
   DBGPRINTF_MED ("AEEBTMgr_Release - ref count = %d", pMe->uRefs);

   uRefs = --pMe->uRefs;

   if ( 0 == uRefs )
   {
      AEEBTMgr_Cleanup (pMe);
   }
   
   return uRefs;
}

/*======================================================================= 
AEEBTMgr_QueryInterface()
=======================================================================*/
static int AEEBTMgr_QueryInterface (
   IBTMgr*  pMe, 
   AEECLSID clsID, 
   void**   ppObj
)
{
   if ( NULL == ppObj )
   {
      return EBADPARM;
   }

   if ( !AEE_CHECK_PTR("AEEBTMgr_QueryInterface", ppObj, sizeof(void*), TRUE))
   {
      return EMEMPTR;
   }

   if ((AEEIID_IQI == clsID) ||
       (AEEIID_IBTMgr == clsID) ||
       (AEECLSID_BTMgr == clsID))
   {
      *ppObj = (IQueryInterface*)pMe;
      IQI_AddRef (*ppObj);
      return SUCCESS;
   }
   return ECLASSNOTSUPPORT;
}

/*======================================================================= 
AEEBTMgr_OnEventNotify()
=======================================================================*/
static int AEEBTMgr_OnEventNotify (IBTMgr* pMe, ISignal* piSig)
{
   int result;
   if (NULL == piSig)
   {
      result = AEE_EBADPARM;
   }
   else if ((NULL == pMe->piEvSignalBus) &&
            ((result = ISHELL_CreateInstance (
                         pMe->piShell, AEECLSID_SignalBus,
                         (void**)&pMe->piEvSignalBus)) != SUCCESS))
   {
      DBGPRINTF_ERROR ("Mgr_OnEventNotify - failed to create Ev signal bus");
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
AEEBTMgr_GetEvent()
=======================================================================*/
static int AEEBTMgr_GetEvent (
   IBTMgr* pMe, 
   BTMgrEventType* pdEventType,
   BTResult* pdResult
)
{
   int result;
   DBGPRINTF ("AEEBTMgr_GetEvent - #ev=%d", pMe->nNumReadyEvents);
   
   if (!AEE_CHECK_PTR ("AEEBTMgr_GetEvent", pdEventType, 
                       sizeof(BTMgrEventType), TRUE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      int nMaxEvCnt = ARR_SIZE (pMe->dPendingEvent);

      AEEBTMGR_LOCK();
      if (pMe->nNumReadyEvents > 0)
      {
         *pdEventType = pMe->dPendingEvent[pMe->nNextReadyEventIdx].dType;
         *pdResult    = pMe->dPendingEvent[pMe->nNextReadyEventIdx].dResult;
         pMe->nNumReadyEvents--;
         pMe->nNextReadyEventIdx++;
         pMe->nNextReadyEventIdx = pMe->nNextReadyEventIdx % nMaxEvCnt;
         result = AEE_SUCCESS;
      }
      else
      {
         result = AEE_ENOMORE;
      }
      AEEBTMGR_UNLOCK();
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_GetRadioState()
=======================================================================*/
static int AEEBTMgr_GetRadioState (IBTMgr* pMe, BTRadioState* pdRadioState)
{
   int result = AEE_EFAILED;

   if (NULL == pdRadioState)
   {
      result = AEE_EBADPARM;
   }
   else
   {
      result = OEMBTMgr_GetRadioState (pMe->pOEMBTMgr, pdRadioState);
      pMe->dRadioState = *pdRadioState;
      DBGPRINTF ("AEEBTMgr_GetRadioState - #st=%d", *pdRadioState);      
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_GetLocalSettings()
=======================================================================*/
static int AEEBTMgr_GetLocalSettings (
   IBTMgr* pMe, 
   BTLocalSettingBitmap* pdWhatsValid,
   BTLocalSettings* pSettings
)
{
   int result;
   if ((NULL == pdWhatsValid) || (NULL == pSettings))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      *pdWhatsValid = 0xFF; // 8 fields are valid
      OEMBTMgr_GetLocalSettings (pMe->pOEMBTMgr, pSettings);
      result = AEE_SUCCESS;
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_SetName()
=======================================================================*/
static int AEEBTMgr_SetName (IBTMgr* pMe, const uint8* pszName)
{
   int result;
   int nLen;
   DBGPRINTF ("AEEBTMgr_SetName - entered");

   if (AEEBTMGR_OP_SET_NAME == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if ((NULL == pszName) || (0 == (nLen = STRLEN((char*)pszName))))
   {
      result = AEE_EBADPARM;
   }
   else if (!AEE_CHECK_PTR ("AEEBTMgr_SetName", pszName, nLen, FALSE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_SET_NAME;
      if ((result = OEMBTMgr_SetName (pMe->pOEMBTMgr, pszName)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_SetCoD()
=======================================================================*/
static int AEEBTMgr_SetCoD (IBTMgr* pMe, const BTClassOfDevice* pCoD)
{
   int result;
   DBGPRINTF ("AEEBTMgr_SetCoD - entered");

   if (AEEBTMGR_OP_SET_COD == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if (!AEE_CHECK_PTR ("AEEBTMgr_SetCoD", pCoD, 
                            sizeof(BTClassOfDevice), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_SET_COD;
      if ((result = OEMBTMgr_SetCoD (pMe->pOEMBTMgr, pCoD)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_SetSecurity()
=======================================================================*/
static int AEEBTMgr_SetSecurity (IBTMgr* pMe, BTSecurityLevel dSec)
{
   int result;
   DBGPRINTF ("AEEBTMgr_SetSecurity - sec=%d", dSec);

   if (AEEBTMGR_OP_SET_SEC == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if (AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT < dSec)
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_SET_SEC;
      if ((result = OEMBTMgr_SetSecurity (pMe->pOEMBTMgr, 
                                          NULL, dSec)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_SetPrefConnRole()
=======================================================================*/
static int AEEBTMgr_SetPrefConnRole (IBTMgr* pMe, boolean bMaster)
{
   int result;
   DBGPRINTF ("AEEBTMgr_SetPrefConnRole - entered");

   if (AEEBTMGR_OP_SET_ROLE == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_SET_ROLE;
      if ((result = OEMBTMgr_SetPrefConnRole (
                      pMe->pOEMBTMgr, NULL, bMaster)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_SetRoleSwitchPolicy()
=======================================================================*/
static int AEEBTMgr_SetRoleSwitchPolicy (IBTMgr* pMe, boolean bAllowed)
{
   int result;
   DBGPRINTF ("AEEBTMgr_SetRoleSwitchPolicy - entered");

   if (AEEBTMGR_OP_SET_ROLE_SW_POLICY == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_SET_ROLE_SW_POLICY;
      if ((result = OEMBTMgr_SetRoleSwitchPolicy (
                      pMe->pOEMBTMgr, NULL, bAllowed)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_SetSvcSecurity()
=======================================================================*/
static int AEEBTMgr_SetSvcSecurity (
   IBTMgr* pMe, 
   BTSecurityLevel dSecLevel,
   const BTServiceIdentifier* pSvcId)
{
   int result;
   DBGPRINTF ("AEEBTMgr_SetSvcSecurity - entered");

   if ((AEEBTMGR_OP_NONE != pMe->dPendingOperation) &&
       (AEEBTMGR_OP_SET_SERVICE_SEC != pMe->dPendingOperation))
   {
      result = AEE_EITEMBUSY;
   }
   else if (!AEE_CHECK_PTR ("AEEBTMgr_SetSvcSecurity", pSvcId, 
                            sizeof(BTServiceIdentifier), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else if ( (dSecLevel != AEEBT_SEC_NONE) &&
	         (dSecLevel != AEEBT_SEC_AUTHENTICATE) &&
	         (dSecLevel != AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT) 
#ifdef FEATURE_BMP
             && (dSecLevel != AEEBT_SEC_SM4_LOW) &&
             (dSecLevel != AEEBT_SEC_SM4_MEDIUM) &&
			 (dSecLevel != AEEBT_SEC_SM4_HIGH) 
#endif /* FEATURE_BMP */
           )
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_SET_SERVICE_SEC;
      if ((result = OEMBTMgr_SetSvcSecurity (
                      pMe->pOEMBTMgr, dSecLevel, pSvcId)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_SetBlockedDevList()
=======================================================================*/
static int AEEBTMgr_SetBlockedDevList (
   IBTMgr* pMe, 
   int nNumDevs, 
   const BDAddress* pAddrList)
{
   int result;
   DBGPRINTF ("AEEBTMgr_SetBlockedDevList - entered");

   if (AEEBTMGR_OP_SET_BLOCKED_DEV_LIST == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if ((nNumDevs > 0) &&
            !AEE_CHECK_PTR ("AEEBTMgr_SetBlockedDevList", pAddrList, 
                            (sizeof(BDAddress)*nNumDevs), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_SET_BLOCKED_DEV_LIST;
      if ((result = OEMBTMgr_SetBlockedDevList (
                      pMe->pOEMBTMgr, nNumDevs, pAddrList)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_OnConnRequest()
=======================================================================*/
static int AEEBTMgr_OnConnRequest (IBTMgr* pMe, ISignal* piSig)
{
   int result = OEMBTMgr_ConnRequestRegister(pMe->pOEMBTMgr, piSig);
   if (AEE_SUCCESS == result)
   {
      pMe->piConnReqSignal = piSig;
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_GetConnRequest()
=======================================================================*/
static int AEEBTMgr_GetConnRequest (
   IBTMgr* pMe, 
   BDAddress* pAddr, 
   BTServiceIdentifier* pSvcId)
{
   return (OEMBTMgr_GetConnRequest(pMe->pOEMBTMgr, pAddr, pSvcId));
}

/*======================================================================= 
AEEBTMgr_AuthorizeConn()
=======================================================================*/
static int AEEBTMgr_AuthorizeConn (
   IBTMgr* pMe, 
   const BDAddress* pAddr,
   const BTServiceIdentifier* pSvcId,
   boolean bAccept)
{
   int result;
   //DBGPRINTF ("AEEBTMgr_AuthorizeConn - entered");

   if (AEEBTMGR_OP_AUTH_CONN == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else if (!AEE_CHECK_PTR ("AEEBTMgr_AuthorizeConn", pAddr, 
                            sizeof(BDAddress), FALSE) ||
            !AEE_CHECK_PTR ("AEEBTMgr_AuthorizeConn", pSvcId, 
                            sizeof(BTServiceIdentifier), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_AUTH_CONN;
      if ((result = OEMBTMgr_AuthorizeConn (
                      pMe->pOEMBTMgr, pAddr, pSvcId, bAccept)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_TuneScanPerformance()
=======================================================================*/
static int AEEBTMgr_TuneScanPerformance (
   IBTMgr* pMe, 
   BTActVsPwr dPageScanParam, 
   BTActVsPwr dInqScanParam)
{
   int result;
   DBGPRINTF ("AEEBTMgr_TuneScanPerformance - entered");

   if (AEEBTMGR_OP_TUNE_PERF == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_TUNE_PERF;
      if ((result = OEMBTMgr_TuneScanPerformance (
                      pMe->pOEMBTMgr, dPageScanParam, 
                      dInqScanParam)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_DisableRadio()
=======================================================================*/
static int AEEBTMgr_DisableRadio (IBTMgr* pMe)
{
   int result;
   DBGPRINTF ("AEEBTMgr_DisableRadio - entered");

   if (AEEBTMGR_OP_DISABLE_RADIO == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_DISABLE_RADIO;
      if ((result = OEMBTMgr_RadioControl (
                      pMe->pOEMBTMgr, OEMBTMGR_SET_RADIO_OFF)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_EnableRadio()
=======================================================================*/
static int AEEBTMgr_EnableRadio (IBTMgr* pMe)
{
   int result;
   DBGPRINTF ("AEEBTMgr_EnableRadio - entered");

   if (AEEBTMGR_OP_ENABLE_RADIO == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_ENABLE_RADIO;
      if ((result = OEMBTMgr_RadioControl (
                      pMe->pOEMBTMgr, OEMBTMGR_SET_RADIO_ON)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_DisableVisibility()
=======================================================================*/
static int AEEBTMgr_DisableVisibility (IBTMgr* pMe)
{
   int result;
   DBGPRINTF ("AEEBTMgr_DisableVisibility - entered");

   if (AEEBTMGR_OP_DISABLE_VISIBILITY == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_DISABLE_VISIBILITY;
      if ((result = OEMBTMgr_RadioControl (
                      pMe->pOEMBTMgr, 
                      OEMBTMGR_SET_RADIO_UNDISCOVERABLE)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTMgr_RestoreVisibility()
=======================================================================*/
static int AEEBTMgr_RestoreVisibility (IBTMgr* pMe)
{
   int result;
   DBGPRINTF ("AEEBTMgr_RestoreVisibility - entered");

   if (AEEBTMGR_OP_RESTORE_VISIBILITY == pMe->dPendingOperation)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTMGR_OP_NONE != pMe->dPendingOperation)
   {
      result = AEE_EITEMBUSY;
   }
   else
   {
      pMe->dPendingOperation = AEEBTMGR_OP_RESTORE_VISIBILITY;
      if ((result = OEMBTMgr_RadioControl (
                      pMe->pOEMBTMgr, 
                      OEMBTMGR_RESTORE_DISCOVERABILITY)) != AEE_SUCCESS)
      {
         pMe->dPendingOperation = AEEBTMGR_OP_NONE;
      }
   }
   return result;
}

/************************************************************************ 
                             PUBLIC FUNCTIONS
************************************************************************/
/*======================================================================= 
AEEBTMgr_New()
=======================================================================*/
int AEEBTMgr_New (IShell* piShell, void** ppif)
{
   int result;
   IBTMgr* pMe = NULL;

   DBGPRINTF ("AEEBTMgr_New - entered");

   if (ISHELL_CheckPrivLevel (piShell, AEECLSID_BTMgr, TRUE) == FALSE)
   {
      result = AEE_EPRIVLEVEL;
   }
   else if (NULL == ppif)
   {
      result =  AEE_EBADPARM;
   }
   else if ((pMe = MALLOCREC (IBTMgr)) == NULL)
   {
      result = AEE_ENOMEMORY;
   }
   else if ((result = AEEBTMgr_Init (pMe, piShell)) != AEE_SUCCESS)
   {
      AEEBTMgr_Cleanup (pMe);
   }
   else
   {
      *ppif = (void *)pMe;
   }
   return result;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */


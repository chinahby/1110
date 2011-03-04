/*=============================================================================
FILE:  AEEBTConnection.c

SERVICES:  Implementation of IBTConnection

GENERAL DESCRIPTION:
   Implements IBTAudioGateway.

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTConnection.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
09/15/08   sp  Compiler warning fix

=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEE_OEM.h"
#include "AEE_OEMHeap.h"
#include "AEE_OEMDispatch.h"
#include "AEEIBTConnection.h"
#include "AEEBTConnection.bid"
#include "AEECriticalSection.h"
#include "AEEISignal.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalCBFactory.h"
#include "AEEISignalBus.h"
#include "AEESignalBus.bid"
#include "OEMBTMgr.h"

/*=============================================================================
 Macros 
=============================================================================*/
#define AEEBTCONN_LOCK()    AEECriticalSection_Enter(&pMe->criticalSection)
#define AEEBTCONN_UNLOCK()  AEECriticalSection_Leave(&pMe->criticalSection)

/*======================================================================= 
   Defines
=======================================================================*/
typedef struct
{
   BTConnEventType      dType;
   BTResult             dResult;
}  BTConnEvent;

struct IBTConnection
{
   const IBTConnectionVtbl*    pvt;
   uint32               uRefs;
   uint32               uFlags;
   IShell*              piShell;
   ACONTEXT*            pAC;
   OEMBTMgr*            pOEMBTMgr;
   ISignalBus*          piEvSignalBus;
   AEECriticalSection   criticalSection;

   // state info
   BTConnectionStatus   status;
   BTConnOperationType  dLastCmpltOperation;
   BTConnEvent          dPendingEvent[5];
   int                  nNumReadyEvents;
   int                  nNextReadyEventIdx;
   AEECallback          evSignalCb;   
};

static uint32  AEEBTConn_AddRef (IBTConnection*);
static uint32  AEEBTConn_Release (IBTConnection*);
static int     AEEBTConn_QueryInterface (IBTConnection*, AEECLSID, void**);
static int     AEEBTConn_OnEventNotify (IBTConnection*, ISignal*);
static int     AEEBTConn_GetEvent (IBTConnection*, BTConnEventType*, BTResult*);
static int     AEEBTConn_GetStatus (IBTConnection*, 
                                    BTConnStatBitmap*,
                                    BTConnectionStatus*);
static int     AEEBTConn_SetRoleSwitchPolicy (IBTConnection*, boolean);
static int     AEEBTConn_SetRole (IBTConnection*, boolean);
static int     AEEBTConn_SetMinSecurity  (IBTConnection*, BTSecurityLevel);
static int     AEEBTConn_SetLowPowerConfig (IBTConnection*, 
                                            BTLowPowerMode,
                                            uint16,
                                            uint16);

static const AEEVTBL(IBTConnection) gpvtIBTConn =
{
   AEEBTConn_AddRef,
   AEEBTConn_Release,
   AEEBTConn_QueryInterface,
   AEEBTConn_OnEventNotify,
   AEEBTConn_GetEvent,
   AEEBTConn_GetStatus,
   AEEBTConn_SetRoleSwitchPolicy,
   AEEBTConn_SetRole,
   AEEBTConn_SetMinSecurity,
   AEEBTConn_SetLowPowerConfig
};

/************************************************************************ 
                           PRIVATE FUNCTIONS
************************************************************************/
static void AEEBTConn_EvSignalCallback(void *pData)
{
   IBTConnection* pMe = (IBTConnection*)pData;
   if (NULL != pMe->piEvSignalBus)
   {
      ISignalBus_Strobe (pMe->piEvSignalBus);
   }
   return;
}
static void AEEBTConn_QueueEvent (
   IBTConnection* pMe,
   BTConnEventType dType,
   BTResult dResult
)
{
   if (NULL != pMe->piEvSignalBus)
   {
      BTConnEventType evDropped = AEEBTCONN_EV_NONE;
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

      if (AEEBTCONN_EV_NONE != evDropped)
      {
         //DBGPRINTF_ERROR ("QueueEv - dropping event %d", evDropped);
      }
      CALLBACK_Cancel(&pMe->evSignalCb);
      CALLBACK_Init(&pMe->evSignalCb, AEEBTConn_EvSignalCallback, pMe);
      AEE_SYS_RESUME(&pMe->evSignalCb);
   }
}

static void AEEBTConn_OpCmpltCleanup (
   IBTConnection* pMe, 
   BTConnEventType dEv,
   BTResult dResult
)
{

   AEEBTCONN_LOCK();
   pMe->dLastCmpltOperation = pMe->status.dPendingOp;
   pMe->status.dPendingOp   = AEEBTCONN_OP_NONE;
   AEEBTCONN_UNLOCK();
   AEEBTConn_QueueEvent (pMe, dEv, dResult);
}

static void AEEBTConn_EvCb (IBTConnection* pMe, OEMBTMgrEvent* pEv)
{
   BTConnEventType aeeEv = AEEBTCONN_EV_NONE;
   if ((NULL != pMe) && (NULL != pEv))
   {
      switch (pEv->dType)
      {
         case OEMBTMGR_EV_SET_CONN_SEC_DONE:
            if (pMe->status.dPendingOp == AEEBTCONN_OP_SET_SEC)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTCONN_EV_SET_SECURITY_SUCCESS : 
                  AEEBTCONN_EV_SET_SECURITY_FAILED;
               AEEBTConn_OpCmpltCleanup (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_SET_CONN_ROLE_DONE:
            if (pMe->status.dPendingOp == AEEBTCONN_OP_SET_ROLE)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTCONN_EV_SET_ROLE_SUCCESS : AEEBTCONN_EV_SET_ROLE_FAILED;
               AEEBTConn_OpCmpltCleanup (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_SET_CONN_SW_POL_DONE:
            if (pMe->status.dPendingOp == AEEBTCONN_OP_SET_ROLE_SW_POLICY)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTCONN_EV_SET_ROLE_SW_POLICY_SUCCESS : 
                  AEEBTCONN_EV_SET_ROLE_SW_POLICY_FAILED;
               AEEBTConn_OpCmpltCleanup (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_SET_CONN_LPM_DONE:
            if (pMe->status.dPendingOp == AEEBTCONN_OP_SET_LOW_POWER_CONFIG)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTCONN_EV_SET_LOW_POWER_CONFIG_SUCCESS : 
                  AEEBTCONN_EV_SET_LOW_POWER_CONFIG_FAILED;
               AEEBTConn_OpCmpltCleanup (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTMGR_EV_CONN_STAT_CHANGED:
            if (OEMBTMgr_GetConnStatus (pMe->pOEMBTMgr, &pMe->status,
                                        FALSE) == AEE_SUCCESS)
            {
               AEEBTConn_QueueEvent (pMe, AEEBTCONN_EV_STATUS_CHANGED, 
                                     pEv->dResult);
            }
            break;
      }
   }
}

static void AEEBTConn_Cleanup (IBTConnection* pMe)
{
   DBGPRINTF ("AEEBTConn_Cleanup - dev name=%s", pMe->status.about.szName);

   OEMBTMgr_DeregisterConnEv (pMe->pOEMBTMgr, pMe);
   if (NULL != pMe->piEvSignalBus)
   {
      ISignalBus_Clear (pMe->piEvSignalBus);
      ISignalBus_Release (pMe->piEvSignalBus);
      pMe->piEvSignalBus = NULL;
   }
   CALLBACK_Cancel(&pMe->evSignalCb);
   if (NULL != pMe->pOEMBTMgr)
   {
      OEMBTMgr_Release (pMe->pOEMBTMgr);
      pMe->pOEMBTMgr = NULL;
   }
   if (NULL != pMe->piShell)
   {
      ISHELL_Release (pMe->piShell);
      pMe->piShell = NULL;
   }
   FREEIF (pMe);
}

static int AEEBTConn_Init (
   IBTConnection*   pMe, 
   IShell*          piShell, 
   const BDAddress* pAddr
)
{
   int result = AEE_EFAILED;

   DBGPRINTF ("AEEBTConn_Init - entered");

   pMe->pvt     = &gpvtIBTConn;
   pMe->piShell = piShell;
   pMe->uRefs   = 1;
   pMe->pAC     = AEE_GetAppContext();
   pMe->nNextReadyEventIdx = 0;
   ISHELL_AddRef (piShell);

   pMe->status.about.bdAddr = *pAddr;

   if (OEMBTMgr_New (piShell, &pMe->pOEMBTMgr) != AEE_SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTConn_Init - failed to create OEMBTMgr obj");
   }
   else if ((result = OEMBTMgr_GetConnStatus (pMe->pOEMBTMgr, &pMe->status,
                                              TRUE)) != AEE_SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTConn_Init - failed to get conn stat r=%d", result);
   }
   else if (OEMBTMgr_RegisterConnEv (pMe->pOEMBTMgr, AEEBTConn_EvCb, 
                                     pMe, pAddr) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTConn_Init - failed to register EvCb");
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
AEEBTConn_AddRef()
=======================================================================*/
static uint32  AEEBTConn_AddRef (IBTConnection* pMe)
{
   // increment ref count
   if (pMe != NULL)
   {
      return ++pMe->uRefs;
   }
   return 0;
}

/*======================================================================= 
AEEBTConn_Release()
=======================================================================*/
static uint32  AEEBTConn_Release (IBTConnection* pMe)
{
   uint32 uRefs;

   if (0 == pMe->uRefs)
   {
      return pMe->uRefs;
   }
   DBGPRINTF_MED ("AEEBTConn_Release - ref count = %d", pMe->uRefs);

   uRefs = --pMe->uRefs;

   if ( 0 == uRefs )
   {
      AEEBTConn_Cleanup (pMe);
   }

   return uRefs;
}

/*======================================================================= 
AEEBTConn_QueryInterface()
=======================================================================*/
static int AEEBTConn_QueryInterface (
   IBTConnection* pMe, 
   AEECLSID       clsID, 
   void**         ppObj
)
{
   if ( NULL == ppObj )
   {
      return EBADPARM;
   }

   if ( !AEE_CHECK_PTR("AEEBTConn_QueryInterface", ppObj, sizeof(void*), TRUE))
   {
      return EMEMPTR;
   }

   if ((AEEIID_IQI == clsID) ||
       (AEEIID_IBTConnection == clsID) ||
       (AEECLSID_BTConnection == clsID))
   {
      *ppObj = (IQueryInterface*)pMe;
      IQI_AddRef (*ppObj);
      return SUCCESS;
   }
   return ECLASSNOTSUPPORT;
}

/*======================================================================= 
AEEBTConn_OnEventNotify()
=======================================================================*/
static int AEEBTConn_OnEventNotify (IBTConnection* pMe, ISignal* piSig)
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
      DBGPRINTF_ERROR ("AEEBTConn_OnEventNotify - failed to create signal bus");
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
AEEBTConn_GetEvent()
=======================================================================*/
static int AEEBTConn_GetEvent (
   IBTConnection* pMe, 
   BTConnEventType* pdEventType,
   BTResult* pdResult
)
{
   int result;
   if (!AEE_CHECK_PTR ("AEEBTConn_GetEvent", pdEventType, 
                       sizeof(BTConnEventType), TRUE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      int nMaxEvCnt = ARR_SIZE (pMe->dPendingEvent);

      AEEBTCONN_LOCK();
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
      AEEBTCONN_UNLOCK();
   }
   return result;
}

/*======================================================================= 
AEEBTConn_GetStatus()
=======================================================================*/
static int AEEBTConn_GetStatus (
   IBTConnection*       pMe, 
   BTConnStatBitmap*    pdWhatsValid,
   BTConnectionStatus*  pStatus
)
{
   int result;
   if (!AEE_CHECK_PTR ("AEEBTConn_GetStatus", pdWhatsValid, 
                       sizeof(BTConnStatBitmap), TRUE) ||
       !AEE_CHECK_PTR ("AEEBTConn_GetStatus", pStatus, 
                       sizeof(BTConnectionStatus), TRUE))
   {
      result = AEE_EBADPARM;
   }
   else if (NULL == pMe->pOEMBTMgr)
   {
      DBGPRINTF_ERROR ("AEEBTConn_GetStatus - no OEMBTMgr obj");
      result = AEE_EBADSTATE;
   }
   else if ((result = OEMBTMgr_GetConnStatus (pMe->pOEMBTMgr, &pMe->status,
                                              FALSE)) != AEE_SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTConn_GetStatus - failed to get conn stat r=%d", 
                       result);
   }
   else
   {
      *pStatus = pMe->status;
      *pdWhatsValid = 0x00FF;
      result = AEE_SUCCESS;
   }
   return result;
}

/*======================================================================= 
AEEBTConn_SetRoleSwitchPolicy()
=======================================================================*/
static int AEEBTConn_SetRoleSwitchPolicy (IBTConnection* pMe, boolean bAllowed)
{
   int result;
   DBGPRINTF ("AEEBTConn_SetRoleSwitchPolicy - allow=%d", bAllowed);

   if (ISHELL_CheckPrivLevel (pMe->piShell, 
                              AEEPRIVID_BTConnection, TRUE) == FALSE)
   {
      result = AEE_EPRIVLEVEL;
   }
   else if (AEEBTCONN_OP_SET_ROLE_SW_POLICY == pMe->status.dPendingOp)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTCONN_OP_NONE != pMe->status.dPendingOp)
   {
      result = AEE_EITEMBUSY;
   }
   else
   {
      pMe->status.dPendingOp = AEEBTCONN_OP_SET_ROLE_SW_POLICY;
      if ((result = OEMBTMgr_SetRoleSwitchPolicy (pMe->pOEMBTMgr, 
                                                  &pMe->status.about.bdAddr,
                                                  bAllowed)) != AEE_SUCCESS)
      {
         pMe->status.dPendingOp = AEEBTCONN_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTConn_SetRole()
=======================================================================*/
static int AEEBTConn_SetRole (IBTConnection* pMe, boolean bMaster)
{
   int result;
   DBGPRINTF ("AEEBTConn_SetPrefConnRole - master=%d", bMaster);

   if (ISHELL_CheckPrivLevel (pMe->piShell, 
                              AEEPRIVID_BTConnection, TRUE) == FALSE)
   {
      result = AEE_EPRIVLEVEL;
   }
   else if (AEEBTCONN_OP_SET_ROLE == pMe->status.dPendingOp)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTCONN_OP_NONE != pMe->status.dPendingOp)
   {
      result = AEE_EITEMBUSY;
   }
   else
   {
      pMe->status.dPendingOp = AEEBTCONN_OP_SET_ROLE;
      if ((result = OEMBTMgr_SetPrefConnRole (pMe->pOEMBTMgr, 
                                              &pMe->status.about.bdAddr,
                                              bMaster)) != AEE_SUCCESS)
      {
         pMe->status.dPendingOp = AEEBTCONN_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
AEEBTConn_SetMinSecurity()
=======================================================================*/
static int AEEBTConn_SetMinSecurity (IBTConnection* pMe, BTSecurityLevel dSec)
{
   int result;
   DBGPRINTF ("AEEBTConn_SetSecurity - sec=%d", dSec);

   if (ISHELL_CheckPrivLevel (pMe->piShell, 
                              AEEPRIVID_BTConnection, TRUE) == FALSE)
   {
      result = AEE_EPRIVLEVEL;
   }
   else if (AEEBTCONN_OP_SET_SEC == pMe->status.dPendingOp)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTCONN_OP_NONE != pMe->status.dPendingOp)
   {
      result = AEE_EITEMBUSY;
   }
   else if (AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT < dSec)
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->status.dPendingOp = AEEBTCONN_OP_SET_SEC;
      if ((result = OEMBTMgr_SetSecurity (pMe->pOEMBTMgr,  
                                          &pMe->status.about.bdAddr,
                                          dSec)) != AEE_SUCCESS)
      {
         pMe->status.dPendingOp = AEEBTCONN_OP_NONE;
      }
   }
   return result;
}


/*======================================================================= 
AEEBTConn_SetLowPowerConfig()
=======================================================================*/
static int AEEBTConn_SetLowPowerConfig (
   IBTConnection* pMe, 
   BTLowPowerMode dLowPowerMode,
   uint16         uMaxInterval,
   uint16         uIdleTimeout
)
{
   int result;
   DBGPRINTF ("AEEBTConn_SetLowPowerConfig - mode=%d i=%d t=%d", 
              dLowPowerMode, uMaxInterval, uIdleTimeout);

   if (ISHELL_CheckPrivLevel (pMe->piShell, 
                              AEEPRIVID_BTConnection, TRUE) == FALSE)
   {
      result = AEE_EPRIVLEVEL;
   }
   else if (AEEBTCONN_OP_SET_LOW_POWER_CONFIG == pMe->status.dPendingOp)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTCONN_OP_NONE != pMe->status.dPendingOp)
   {
      result = AEE_EITEMBUSY;
   }
   else
   {
      pMe->status.dPendingOp = AEEBTCONN_OP_SET_LOW_POWER_CONFIG;
      if ((result = OEMBTMgr_SetLowPowerConfig (
                      pMe->pOEMBTMgr,  &pMe->status.about.bdAddr,
                      dLowPowerMode, uMaxInterval, 
                      uIdleTimeout)) != AEE_SUCCESS)
      {
         pMe->status.dPendingOp = AEEBTCONN_OP_NONE;
      }
   }
   return result;
}

/************************************************************************ 
                             PUBLIC FUNCTIONS
************************************************************************/
/*======================================================================= 
AEEBTConn_New()
=======================================================================*/
int AEEBTConn_New (
   IShell* piShell, 
   const BDAddress* pAddr, 
   void** ppif
)
{
   int result;
   IBTConnection* pMe = NULL;

   DBGPRINTF ("AEEBTConn_New - entered");

   if (!AEE_CHECK_PTR ("AEEBTConn_New", pAddr, sizeof(BDAddress), FALSE) ||
       (NULL == ppif))
   {
      result =  AEE_EBADPARM;
   }
   else if ((pMe = MALLOCREC (IBTConnection)) == NULL)
   {
      result = AEE_ENOMEMORY;
   }
   else if ((result = AEEBTConn_Init (pMe, piShell, pAddr)) != AEE_SUCCESS)
   {
      AEEBTConn_Cleanup (pMe);
   }
   else
   {
      *ppif = (void *)pMe;
   }
   return result;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */


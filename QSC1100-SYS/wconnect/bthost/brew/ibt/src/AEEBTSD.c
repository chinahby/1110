/*=============================================================================
FILE:  AEEBTSD.c

SERVICES:  Implementation of IBTServiceDiscovery

GENERAL DESCRIPTION:
   Implements IBTServiceDiscovery.

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTSD.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
$Author: deepikas $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2008-10-13   pn  Uncommented debug printing.
  2008-09-19   pn  String lengths returned now reflex space for NULL.
                   GetSearchResult() now returns correct error code.

=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEE_OEM.h"
#include "AEE_OEMHeap.h"
#include "AEE_OEMDispatch.h"
#include "AEEBTServiceDiscovery.bid"
#include "OEMBTSD.h"
#include "OEMBTUtils.h"
#include "AEECriticalSection.h"
#include "AEEISignal.h"
#include "AEEISignalCtl.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalCBFactory.h"
#include "AEEISignalBus.h"
#include "AEESignalBus.bid"
#include "AEEBT.bid"
#include "AEEBT.h"
#include "AEEStd.h"
#ifdef BT_SIMULATION
#include "btmsg.h"
#endif
/*=============================================================================
   Macros 
=============================================================================*/
#define AEEBTSD_LOCK()    AEECriticalSection_Enter(&pMe->criticalSection)
#define AEEBTSD_UNLOCK()  AEECriticalSection_Leave(&pMe->criticalSection)

#define AEEBTSD_FLAG_OP_BY_ME                  (1 << 0)

/*======================================================================= 
   Defines
=======================================================================*/
#define AEEBTSD_OP_NONE                         0
#define AEEBTSD_OP_START_DEV_SEARCH             (1 << 0)
#define AEEBTSD_OP_STOP_DEV_SEARCH              (1 << 1)
#define AEEBTSD_OP_START_SVC_SEARCH             (1 << 2)
#define AEEBTSD_OP_STOP_SVC_SEARCH              (1 << 3)
#define AEEBTSD_OP_START_NAME_REQ               (1 << 4)
#define AEEBTSD_OP_STOP_NAME_REQ                (1 << 5)
#define AEEBTSD_OP_SET_DISCOVERABLE_MODE        (1 << 6)
#define AEEBTSD_OP_REG_SVC                      (1 << 7)
#define AEEBTSD_OP_DEREG_SVC                    (1 << 8)

typedef uint32 BTSDOperationType;

/*======================================================================= 
   Structures for IBTServiceDiscovery
=======================================================================*/
typedef struct
{
   BTSDEventType           dType;
   BTResult                dResult;
}  BTSDEvent;

typedef struct
{
   BDAddress               bdAddr;
   uint8                   szName[AEEBT_MAX_LEN_DEVICE_NAME + 1];
}  BTSDNameRcvd;

struct IBTServiceDiscovery
{
   const IBTServiceDiscoveryVtbl*    pvt;
   uint32                  uRefs;
   uint32                  uFlags;
   IShell*                 piShell;
   ACONTEXT*               pAC;
   ISignalBus*             piEvSignalBus;
   AEECriticalSection      criticalSection;

   // state info
   OEMBTSD*                pOEMBTSD;
   BTSDOperationType       dPendingOps;
   BTSDEvent               dPendingEvent[10];
   int                     nNumReadyEvents;
   int                     nNextReadyEventIdx;

   // device search info
   BDAddress               bdAddr[OEMBTSD_MAX_DISCOVERED_DEV];
   BTClassOfDevice         CoD[OEMBTSD_MAX_DISCOVERED_DEV];
   int                     nNumDevFound;
   int                     nNextIdx;

   // most recent device name request info
   BTSDNameRcvd            lastName;

   // most recent service search request info
   BDAddress               svcSrchAddr;
   char*                   pszXMLResultStr;
   int                     nNumResultRecs;

   BTSvcDiscoverableMode   dMode; // valid until SetDiscMode is done

   BTServiceRecordSummary  localRecSummary[OEMBTSD_MAX_SERVICE_DB_ENTRIES];
   uint32                  myHandles[OEMBTSD_MAX_SERVICE_DB_ENTRIES];
   int                     nNumMyHandles;
   char*                   pszRecXML; // XML rep for rec with uRecHandle
   uint32                  uRecHandle;
   AEECallback             evSignalCb;    

};

static uint32  AEEBTSD_AddRef (IBTServiceDiscovery*);
static uint32  AEEBTSD_Release (IBTServiceDiscovery*);
static int     AEEBTSD_QueryInterface (IBTServiceDiscovery*, AEECLSID, void**);
static int     AEEBTSD_OnEventNotify (IBTServiceDiscovery*, ISignal*);
static int     AEEBTSD_GetEvent (IBTServiceDiscovery*, BTSDEventType*, 
                                 BTResult*);
static int     AEEBTSD_StartDeviceSearch (IBTServiceDiscovery*,
                                          BTClassOfService, int, int);
static int     AEEBTSD_StopDeviceSearch (IBTServiceDiscovery*);
static int     AEEBTSD_GetNextResponse (IBTServiceDiscovery*, BTDeviceInfo*);
static int     AEEBTSD_StartNameRequest (IBTServiceDiscovery*,
                                         const BDAddress*);
static int     AEEBTSD_StopNameRequest (IBTServiceDiscovery*, const BDAddress*);
static int     AEEBTSD_GetName (IBTServiceDiscovery*, BTDeviceInfo*);
static int     AEEBTSD_RegisterService (IBTServiceDiscovery*, const char*, 
                                        boolean);
static int     AEEBTSD_GetMyRecHandles (IBTServiceDiscovery*, uint32*,int,int*);
static int     AEEBTSD_DeregisterService (IBTServiceDiscovery*, uint32);
static int     AEEBTSD_GetLocalDBSummary (IBTServiceDiscovery*, 
                                          BTServiceRecordSummary*,
                                          int, int*);
static int     AEEBTSD_ReadRec (IBTServiceDiscovery*, uint32, char*,int,int*);
static int     AEEBTSD_WriteRec (IBTServiceDiscovery*, uint32, const char*);
static int     AEEBTSD_GetDiscoverableMode (IBTServiceDiscovery*, 
                                            BTSvcDiscoverableMode*);
static int     AEEBTSD_SetDiscoverableMode (IBTServiceDiscovery*, 
                                            BTSvcDiscoverableMode);
static int     AEEBTSD_StartServiceSearch (IBTServiceDiscovery*,
                                           const BDAddress*,
                                           const BTSvcSearchPattern*);
static int     AEEBTSD_StopServiceSearch (IBTServiceDiscovery*);
static int     AEEBTSD_GetNumFoundRecs (IBTServiceDiscovery*, int*);
static int     AEEBTSD_GetSearchResult (IBTServiceDiscovery*, char*, int, int*);

static const AEEVTBL(IBTServiceDiscovery) gpvtIBTSD =
{
   AEEBTSD_AddRef,
   AEEBTSD_Release,
   AEEBTSD_QueryInterface,
   AEEBTSD_OnEventNotify,
   AEEBTSD_GetEvent,
   AEEBTSD_StartDeviceSearch,
   AEEBTSD_StopDeviceSearch,
   AEEBTSD_GetNextResponse,
   AEEBTSD_StartNameRequest,
   AEEBTSD_StopNameRequest,
   AEEBTSD_GetName,
   AEEBTSD_RegisterService,
   AEEBTSD_GetMyRecHandles,
   AEEBTSD_DeregisterService,
   AEEBTSD_GetLocalDBSummary,
   AEEBTSD_ReadRec,
   AEEBTSD_WriteRec,
   AEEBTSD_GetDiscoverableMode,
   AEEBTSD_SetDiscoverableMode,
   AEEBTSD_StartServiceSearch,
   AEEBTSD_StopServiceSearch,
   AEEBTSD_GetNumFoundRecs,
   AEEBTSD_GetSearchResult
};

/************************************************************************ 
                           PRIVATE HELPER FUNCTIONS
************************************************************************/
static void AEEBTSD_EvSignalCallback(void *pData)
{
   IBTServiceDiscovery* pMe = (IBTServiceDiscovery*)pData;
   if (NULL != pMe->piEvSignalBus)
   {
      ISignalBus_Strobe (pMe->piEvSignalBus);
   }
   return;
}

static void AEEBTSD_QueueEvent (
   IBTServiceDiscovery* pMe,
   BTSDEventType        dType,
   BTResult             dResult
)
{
   if (NULL != pMe->piEvSignalBus)
   {
      BTSDEventType evDropped = AEEBTSD_EV_NONE;
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

      if (AEEBTSD_EV_NONE != evDropped)
      {
         DBGPRINTF_ERROR ("SD_QueueEv - dropping event %d", evDropped);
      }
      CALLBACK_Cancel(&pMe->evSignalCb);
      CALLBACK_Init(&pMe->evSignalCb, AEEBTSD_EvSignalCallback, pMe);
      AEE_SYS_RESUME(&pMe->evSignalCb);
   }
}

static void AEEBTSD_EvCb (IBTServiceDiscovery* pMe, OEMBTSDEvent* pEv)
{
   BTSDEventType aeeEv = AEEBTSD_EV_NONE;

   if ((NULL != pMe) && (NULL != pEv))
   {
     DBGPRINTF ("SD_EvCb - pMe=%x op=%x ev=%d", 
                pMe, pMe->dPendingOps, pEv->dType);

      switch (pEv->dType)
      {
         case OEMBTSD_EV_DEV_DISCOVERY_STARTED:
            if (pMe->dPendingOps & AEEBTSD_OP_START_DEV_SEARCH)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTSD_EV_DEV_SEARCH_STARTED : AEEBTSD_EV_DEV_SEARCH_FAILED;
               AEEBTSD_QueueEvent (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTSD_EV_DEV_DISCOVERY_COMPLETED:
            if (pMe->dPendingOps & AEEBTSD_OP_START_DEV_SEARCH)
            {
               aeeEv = AEEBTSD_EV_DEV_SEARCH_COMPLETE;
               pMe->dPendingOps &= ~(AEEBTSD_OP_START_DEV_SEARCH | 
                                     AEEBTSD_OP_STOP_DEV_SEARCH);
               AEEBTSD_QueueEvent (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTSD_EV_DEV_DISCOVERY_RESPONSE:
            if (pMe->dPendingOps & AEEBTSD_OP_START_DEV_SEARCH)
            {
               aeeEv = AEEBTSD_EV_DEV_SEARCH_RESPONSE;
               pMe->bdAddr[pMe->nNumDevFound] = pEv->devInfo.bdAddr;
               pMe->CoD[pMe->nNumDevFound] = pEv->devInfo.COD;
               pMe->nNumDevFound++;
               AEEBTSD_QueueEvent (pMe, aeeEv, AEEBT_RESULT_SUCCESS);
            }
            break;
         case OEMBTSD_EV_SVC_DISCOVERY_STARTED:
            if (pMe->dPendingOps & AEEBTSD_OP_START_SVC_SEARCH)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTSD_EV_SVC_SEARCH_STARTED : AEEBTSD_EV_SVC_SEARCH_FAILED;
               AEEBTSD_QueueEvent (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTSD_EV_SVC_DISCOVERY_COMPLETED:
            if (pMe->dPendingOps & AEEBTSD_OP_START_SVC_SEARCH)
            {
               if (AEEBT_RESULT_SUCCESS == pEv->dResult)
               {
                  if (AEEBT_BD_ADDRS_EQUAL (&pMe->svcSrchAddr,
                                            &pEv->devInfo.bdAddr))
                  {
                     FREEIF (pMe->pszXMLResultStr);
                     pMe->pszXMLResultStr = pEv->pszXML;
                     pMe->nNumResultRecs = pEv->uVal;
                     pEv->dResult = AEEBT_RESULT_SUCCESS;
                     DBGPRINTF ("SD_EvCb - #recs=%d", pEv->uVal);
                  }
                  else
                  {
                     DBGPRINTF_ERROR ("SD_EvCb - search result for wrong dev");
                     pEv->dResult = AEEBT_RESULT_UNKNOWN_ERR;
                  }
               }
               if (AEEBT_RESULT_SUCCESS != pEv->dResult)
               {
                  FREEIF (pEv->pszXML);
               }
               aeeEv = AEEBTSD_EV_SVC_SEARCH_COMPLETE;
               pMe->dPendingOps &= ~(AEEBTSD_OP_START_SVC_SEARCH |
                                     AEEBTSD_OP_STOP_SVC_SEARCH);
               AEEBTSD_QueueEvent (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTSD_EV_NAME_REQ_STARTED:
            if (pMe->dPendingOps & AEEBTSD_OP_START_NAME_REQ)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTSD_EV_NAME_REQ_STARTED : AEEBTSD_EV_NAME_REQ_FAILED;
               AEEBTSD_QueueEvent (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTSD_EV_NAME_REQ_COMPLETED:
            if (pMe->dPendingOps & AEEBTSD_OP_START_NAME_REQ)
            {
               if (AEEBT_RESULT_SUCCESS == pEv->dResult)
               {
                  if (AEEBT_BD_ADDRS_EQUAL (&pMe->lastName.bdAddr,
                                            &pEv->devInfo.bdAddr) )
                  {
                     std_memmove (pMe->lastName.szName, pEv->devInfo.szName, 
                                  sizeof(pMe->lastName.szName));
                     pEv->dResult = AEEBT_RESULT_SUCCESS;
                  }
                  else
                  {
                     DBGPRINTF_ERROR ("SD_EvCb - got name for wrong dev");
                     pEv->dResult = AEEBT_RESULT_UNKNOWN_ERR;
                  }
               }
               aeeEv = AEEBTSD_EV_NAME_REQ_COMPLETE;
               pMe->dPendingOps &= ~(AEEBTSD_OP_START_NAME_REQ |
                                     AEEBTSD_OP_STOP_NAME_REQ);
               AEEBTSD_QueueEvent (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTSD_EV_SVC_REG_DONE:
            if (pMe->dPendingOps & AEEBTSD_OP_REG_SVC)
            {
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTSD_EV_SVC_REG_COMPLETE : AEEBTSD_EV_SVC_REG_FAILED;
               if (AEEBT_RESULT_SUCCESS == pEv->dResult)
               {
                  pMe->myHandles[pMe->nNumMyHandles++] = pEv->uVal;
               }
               pMe->dPendingOps &= ~AEEBTSD_OP_REG_SVC;
               AEEBTSD_QueueEvent (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTSD_EV_SVC_DEREG_DONE:
            if (pMe->dPendingOps & AEEBTSD_OP_DEREG_SVC)
            {
               int i = 0;
               aeeEv = (AEEBT_RESULT_SUCCESS == pEv->dResult) ?
                  AEEBTSD_EV_SVC_DEREG_COMPLETE : AEEBTSD_EV_SVC_DEREG_FAILED;
               while (i < ARR_SIZE (pMe->myHandles))
               {
                  if ((pMe->myHandles[i] == pEv->uVal) &&
                      (pMe->myHandles[i] != 0))
                  {
                     pMe->myHandles[i] = 0;
                     pMe->nNumMyHandles--;
                     break;
                  }
                  i++;
               }
               if (i >= ARR_SIZE (pMe->myHandles)) // handle not found?
               {
                  DBGPRINTF_ERROR ("SD_EvCb - unknown handle %x with dereg ev",
                                   pEv->uVal);
               }
               else
               {
                  while (i < (ARR_SIZE (pMe->myHandles) - 1))
                  {
                     if (pMe->myHandles[i+1] != 0)
                     {
                        pMe->myHandles[i]   = pMe->myHandles[i+1];
                        pMe->myHandles[i+1] = 0;
                     }
                     i++;
                  }
               }
               pMe->dPendingOps &= ~AEEBTSD_OP_DEREG_SVC;
               AEEBTSD_QueueEvent (pMe, aeeEv, pEv->dResult);
            }
            break;
         case OEMBTSD_EV_SET_DISC_MODE_DONE:
            if (pMe->dPendingOps & AEEBTSD_OP_SET_DISCOVERABLE_MODE)
            {
               OEMBTSD_GetDiscoverableMode (pMe->pOEMBTSD, &pEv->dMode);
               aeeEv = (pEv->dMode == pMe->dMode) ?
                  AEEBTSD_EV_DISC_MODE_SET_SUCCESS : 
                  AEEBTSD_EV_DISC_MODE_SET_FAILED;
               pMe->dPendingOps &= ~AEEBTSD_OP_SET_DISCOVERABLE_MODE;
               AEEBTSD_QueueEvent (pMe, aeeEv, pEv->dResult);
            }
            break;
         default:
            DBGPRINTF ("SD_EvCb - dropping unexp event %d", pEv->dType);
            break;
      }
   }
}

static void AEEBTSD_Cleanup (IBTServiceDiscovery* pMe)
{
   DBGPRINTF ("AEEBTSD_Cleanup - pMe=%x", pMe);

   FREEIF (pMe->pszXMLResultStr);
   FREEIF (pMe->pszRecXML);

   if (NULL != pMe->piEvSignalBus)
   {
      ISignalBus_Clear (pMe->piEvSignalBus);
      ISignalBus_Release (pMe->piEvSignalBus);
      pMe->piEvSignalBus = NULL;
   }
   CALLBACK_Cancel(&pMe->evSignalCb);
   if (NULL != pMe->pOEMBTSD)
   {
      OEMBTSD_DeregisterEvCb (pMe->pOEMBTSD, pMe);
      OEMBTSD_Release (pMe->pOEMBTSD, pMe->myHandles, pMe->nNumMyHandles);
      pMe->pOEMBTSD = NULL;
   }
   if (NULL != pMe->piShell)
   {
      ISHELL_Release (pMe->piShell);
      pMe->piShell = NULL;
   }
   FREEIF (pMe);
}

static int AEEBTSD_Init (IBTServiceDiscovery* pMe, IShell* piShell)
{
   int result = AEE_EFAILED;

   DBGPRINTF ("AEEBTSD_Init - entered");

   pMe->pvt     = &gpvtIBTSD;
   pMe->piShell = piShell;
   pMe->uRefs   = 1;
   pMe->pAC     = AEE_GetAppContext();
   pMe->nNextReadyEventIdx = 0;
   ISHELL_AddRef (piShell);

   if (OEMBTSD_RegisterEvCb (pMe->pOEMBTSD, AEEBTSD_EvCb, pMe) != SUCCESS)
   {
      DBGPRINTF_ERROR ("AEEBTSD_Init - failed to register EvCb");
   }
   else
   {
      result = AEE_SUCCESS;
   }
   return result;
}


static boolean AEEBTSD_IsMyRecHandle (IBTServiceDiscovery* pMe, uint32 uHandle)
{
   int i;
   for (i = 0; i < pMe->nNumMyHandles; i++)
   {
      if (uHandle == pMe->myHandles[i])
      {
         break;
      }
   }
   if (i == pMe->nNumMyHandles)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

/************************************************************************ 
                             VTABLE FUNCTIONS
************************************************************************/
/*======================================================================= 
   AEEBTSD_AddRef
=======================================================================*/
static uint32  AEEBTSD_AddRef (IBTServiceDiscovery* pMe)
{
   // increment ref count
   if (pMe != NULL)
   {
      return ++pMe->uRefs;
   }
   return 0;
}

/*======================================================================= 
   AEEBTSD_Release
=======================================================================*/
static uint32  AEEBTSD_Release (IBTServiceDiscovery* pMe)
{
   uint32 uRefs;

   if (0 == pMe->uRefs)
   {
      return pMe->uRefs;
   }
   DBGPRINTF_MED ("AEEBTSD_Release - ref count = %d", pMe->uRefs);

   uRefs = --pMe->uRefs;

   if ( 0 == uRefs )
   {
      AEEBTSD_Cleanup (pMe);
   }

   return uRefs;
}

/*======================================================================= 
   AEEBTSD_QueryInterface
=======================================================================*/
static int AEEBTSD_QueryInterface (
   IBTServiceDiscovery* pMe, 
   AEECLSID             clsID, 
   void**               ppObj
)
{
   if ( NULL == ppObj )
   {
      return EBADPARM;
   }

   if ( !AEE_CHECK_PTR("AEEBTSD_QueryInterface", ppObj, sizeof(void*), TRUE))
   {
      return EMEMPTR;
   }

   if ((AEEIID_IQI == clsID) ||
       (AEEIID_IBTServiceDiscovery == clsID) ||
       (AEECLSID_BTServiceDiscovery == clsID))
   {
      *ppObj = (IQueryInterface*)pMe;
      IQI_AddRef (*ppObj);
      return SUCCESS;
   }
   return AEE_ECLASSNOTSUPPORT;
}

/*======================================================================= 
   AEEBTSD_OnEventNotify
=======================================================================*/
static int AEEBTSD_OnEventNotify (IBTServiceDiscovery* pMe, ISignal* piSig)
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
      DBGPRINTF_ERROR ("AEEBTSD_OnEventNotify - failed to create signal bus");
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
   AEEBTSD_GetEvent
=======================================================================*/
static int AEEBTSD_GetEvent (
   IBTServiceDiscovery* pMe, 
   BTSDEventType*       pdEventType,
   BTResult*            pdResult
)
{
   int result;
   if (!AEE_CHECK_PTR ("AEEBTSD_GetEvent", pdEventType, 
                       sizeof(BTSDEventType), TRUE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      int nMaxEvCnt = ARR_SIZE (pMe->dPendingEvent);

      AEEBTSD_LOCK();
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
      AEEBTSD_UNLOCK();
   }
   return result;
}


/*======================================================================= 
   AEEBTSD_StartDeviceSearch
=======================================================================*/
static int AEEBTSD_StartDeviceSearch (
   IBTServiceDiscovery* pMe,
   BTClassOfService dCoS,
   int nMaxDevices, 
   int nDuration
)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_StartDeviceSearch - CoS=0x%x ops=%x", dCoS, pMe->dPendingOps);

   if (AEEBTSD_OP_START_DEV_SEARCH & pMe->dPendingOps)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTSD_OP_NONE != pMe->dPendingOps)
   {
      result = AEE_EITEMBUSY;
   }
   else if (((AEEBT_COS_POSITIONING < dCoS) && (AEEBT_COS_ALL > dCoS)) ||
            (0 >= nMaxDevices) || (10 > nDuration))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->dPendingOps = AEEBTSD_OP_START_DEV_SEARCH;
      pMe->nNumDevFound      = 0;
      pMe->nNextIdx          = 0;
      result = OEMBTSD_StartDevDiscovery (pMe->pOEMBTSD, dCoS, 
                                          nMaxDevices, nDuration);
      if (result != AEE_SUCCESS)
      {
         pMe->dPendingOps = AEEBTSD_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_StopDeviceSearch
=======================================================================*/
static int AEEBTSD_StopDeviceSearch (IBTServiceDiscovery* pMe)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_StopDeviceSearch - entered");

   if (AEEBTSD_OP_STOP_DEV_SEARCH == pMe->dPendingOps)
   {
      result = AEE_EALREADY;
   }
   else if ((AEEBTSD_OP_NONE != pMe->dPendingOps) &&
            (AEEBTSD_OP_START_DEV_SEARCH != pMe->dPendingOps))
   {
      result = AEE_EBADSTATE;
   }
   else
   {
      pMe->dPendingOps |= AEEBTSD_OP_STOP_DEV_SEARCH;
      result = OEMBTSD_StopDevDiscovery (pMe->pOEMBTSD);
      if (result != AEE_SUCCESS)
      {
         pMe->dPendingOps &= ~AEEBTSD_OP_STOP_DEV_SEARCH;
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_GetNextResponse
=======================================================================*/
static int AEEBTSD_GetNextResponse (
   IBTServiceDiscovery* pMe, 
   BTDeviceInfo*        pDevInfo
)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_GetNextResponse - #found=%d #read=%d", 
              pMe->nNumDevFound, pMe->nNextIdx);

   if (!AEE_CHECK_PTR ("SD_GetNextResponse", pDevInfo, 
                       sizeof(BTDeviceInfo), TRUE))
   {
      result = AEE_EBADPARM;
   }
   else if ((pMe->nNumDevFound <= 0) || (pMe->nNextIdx >= pMe->nNumDevFound))
   {
      result = AEE_ENOMORE;
   }
   else
   {
      pDevInfo->bdAddr = pMe->bdAddr[pMe->nNextIdx];
      pDevInfo->COD    = pMe->CoD[pMe->nNextIdx];
      pMe->nNextIdx++;
      result = AEE_SUCCESS;
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_StartNameRequest
=======================================================================*/
static int AEEBTSD_StartNameRequest (
   IBTServiceDiscovery* pMe,
   const BDAddress*     pAddr
)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_StartNameRequest - entered");

   if (AEEBTSD_OP_START_NAME_REQ & pMe->dPendingOps)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTSD_OP_NONE != pMe->dPendingOps)
   {
      result = AEE_EITEMBUSY;
   }
   else if (!AEE_CHECK_PTR ("SD_StartNameRequest", pAddr, 
                            sizeof(BDAddress), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->dPendingOps = AEEBTSD_OP_START_NAME_REQ;
      pMe->lastName.bdAddr = *pAddr;
      result = OEMBTSD_GetName (pMe->pOEMBTSD, pAddr);
      if (result != AEE_SUCCESS)
      {
         pMe->dPendingOps = AEEBTSD_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_StopNameRequest
=======================================================================*/
static int AEEBTSD_StopNameRequest (
   IBTServiceDiscovery* pMe, 
   const BDAddress*     pAddr
)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_StopNameRequest - entered");

   if (AEEBTSD_OP_STOP_NAME_REQ & pMe->dPendingOps)
   {
      result = AEE_EALREADY;
   }
   else if ((AEEBTSD_OP_NONE != pMe->dPendingOps) &&
            (AEEBTSD_OP_START_NAME_REQ != pMe->dPendingOps))
   {
      result = AEE_EBADSTATE;
   }
   else
   {
      pMe->dPendingOps |= AEEBTSD_OP_STOP_NAME_REQ;
      result = OEMBTSD_GetNameCancel (pMe->pOEMBTSD, pAddr);
      if (result != AEE_SUCCESS)
      {
         pMe->dPendingOps &= ~AEEBTSD_OP_STOP_NAME_REQ;
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_GetName
=======================================================================*/
static int AEEBTSD_GetName (
   IBTServiceDiscovery* pMe, 
   BTDeviceInfo*        pDevInfo
)
{
   int result = AEE_EFAILED;
   if (!AEE_CHECK_PTR ("SD_GetName", pDevInfo, sizeof(BTDeviceInfo), TRUE))
   {
      result = AEE_EBADPARM;
   }
   else if (AEEBT_BD_ADDRS_EQUAL (&pDevInfo->bdAddr, 
                                  &pMe->lastName.bdAddr) == FALSE)
   {
      result = AEE_ENOSUCH;
   }
   else
   {
      std_memmove (pDevInfo->szName, pMe->lastName.szName,
                   sizeof(pDevInfo->szName));
      result = AEE_SUCCESS;
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_RegisterService
=======================================================================*/
static int AEEBTSD_RegisterService (
   IBTServiceDiscovery* pMe, 
   const char*          pszXML, 
   boolean              bConnectable
)
{
   int result = AEE_EFAILED;
   if (ISHELL_CheckPrivLevel (pMe->piShell, AEEPRIVID_Service_Registration, 
                              TRUE) != TRUE )
   {
      result = AEE_EPRIVLEVEL;
   }
   else if (AEEBTSD_OP_REG_SVC & pMe->dPendingOps)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTSD_OP_NONE != pMe->dPendingOps)
   {
      result = AEE_EITEMBUSY;
   }
   else if (!AEE_CHECK_PTR ("SD_RegisterService", pszXML, sizeof(char), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->dPendingOps |= AEEBTSD_OP_REG_SVC;
      result = OEMBTSD_RegisterService (pMe->pOEMBTSD, pszXML, bConnectable);
      if (result != AEE_SUCCESS)
      {
         pMe->dPendingOps &= ~AEEBTSD_OP_REG_SVC;
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_GetRecHandle
=======================================================================*/
static int AEEBTSD_GetMyRecHandles (
   IBTServiceDiscovery* pMe, 
   uint32*              puHandle,
   int                  nNumHndls,
   int*                 pnValidHndls
)
{
   int result = AEE_EFAILED;
   if (((0 != nNumHndls) && 
        (!AEE_CHECK_PTR ("SD_GetMyRecHandles", puHandle, 
                         (sizeof(uint32) * nNumHndls), TRUE))) ||
       (!AEE_CHECK_PTR ("SD_GetMyRecHandles", pnValidHndls,
                        sizeof(int), TRUE)))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      int i;
      *pnValidHndls = pMe->nNumMyHandles;
      if (nNumHndls < pMe->nNumMyHandles)
      {
         result = AEE_EBUFFERTOOSMALL;
      }
      if (NULL != puHandle)
      {
         for (i = 0; (i < pMe->nNumMyHandles) && (i < nNumHndls); i++ )
         {
            puHandle[i] = pMe->myHandles[i];
         }
         result = AEE_SUCCESS;
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_DeregisterService
=======================================================================*/
static int AEEBTSD_DeregisterService (
   IBTServiceDiscovery* pMe, 
   uint32               uHandle
)
{
   int result = AEE_EFAILED;
   if (AEEBTSD_OP_DEREG_SVC & pMe->dPendingOps)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTSD_OP_NONE != pMe->dPendingOps)
   {
      result = AEE_EITEMBUSY;
   }
   else if (AEEBTSD_IsMyRecHandle (pMe, uHandle) == FALSE)
   {
      result = AEE_ENOSUCH;
   }
   else
   {
      pMe->dPendingOps |= AEEBTSD_OP_DEREG_SVC;
      result = OEMBTSD_DeregisterService (pMe->pOEMBTSD, uHandle);
      if (result != AEE_SUCCESS)
      {
         pMe->dPendingOps = AEEBTSD_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_GetLocalDBSummary
=======================================================================*/
static int AEEBTSD_GetLocalDBSummary (
   IBTServiceDiscovery*    pMe, 
   BTServiceRecordSummary* pDBSummary,
   int                     nNumRecs,
   int*                    pnRemRecs
)
{
   int result = AEE_EFAILED;
   if (((0 != nNumRecs) &&
        (!AEE_CHECK_PTR ("SD_GetLocalDBSummary", pDBSummary, 
                         sizeof(BTServiceRecordSummary) * nNumRecs, TRUE))) ||
       (!AEE_CHECK_PTR ("SD_GetLocalDBSummary", pnRemRecs, sizeof(int), TRUE))
       )
   {
      result = AEE_EBADPARM;
   }
   else
   {
      result = AEE_EBUFFERTOOSMALL;
      *pnRemRecs = OEMBTSD_GetLocalDBRecCount (pMe->pOEMBTSD);
      if (0 < nNumRecs)
      {
         result = OEMBTSD_GetLocalDBSummary (pMe->pOEMBTSD, pDBSummary, 
                                             nNumRecs);
         if (AEE_SUCCESS == result)
         {
            if (nNumRecs < *pnRemRecs)
            {
               *pnRemRecs -= nNumRecs;
               result = AEE_EBUFFERTOOSMALL;
            }

         }
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_ReadRec
=======================================================================*/
static int AEEBTSD_ReadRec (
   IBTServiceDiscovery* pMe, 
   uint32               uHandle,
   char*                pszXML,
   int                  nSize,
   int*                 pnRequiredSize
)
{
   int result = AEE_EFAILED;
   if (((0 != nSize) && 
        (!AEE_CHECK_PTR ("SD_ReadRec", pszXML, 
                         (sizeof(char) * nSize), TRUE))) ||
       (!AEE_CHECK_PTR ("SD_ReadRec", pnRequiredSize,
                        sizeof(int), TRUE)))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      if ((NULL != pMe->pszRecXML) && (uHandle != pMe->uRecHandle))
      {
         // free remnant of another rec
         FREEIF (pMe->pszRecXML);
      }
      if (NULL == pMe->pszRecXML)
      {
         pMe->uRecHandle = uHandle;
         result = OEMBTSD_ReadRec (pMe->pOEMBTSD, uHandle, &pMe->pszRecXML);
         if (AEE_SUCCESS != result)
         {
            pMe->uRecHandle = 0;
            FREEIF (pMe->pszRecXML);
         }
      }
      if ((NULL != pMe->pszRecXML) && (uHandle == pMe->uRecHandle))
      {
         *pnRequiredSize = std_strlen (pMe->pszRecXML) + 1;
         if (nSize < *pnRequiredSize)
         {
            result = AEE_EBUFFERTOOSMALL;
         }
         else
         {
            std_strlcpy (pszXML, pMe->pszRecXML, nSize);
            result = AEE_SUCCESS;
         }
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_WriteRec
=======================================================================*/
static int AEEBTSD_WriteRec (
   IBTServiceDiscovery* pMe, 
   uint32               uHandle,
   const char*          pszXML
)
{
   int result = AEE_EFAILED;
   if (!AEE_CHECK_PTR ("SD_WriteRec", pszXML, sizeof(char), FALSE))
   {
      result = AEE_EBADPARM;
   }
   else if (AEEBTSD_IsMyRecHandle (pMe, uHandle) == FALSE)
   {
      result = AEE_ENOSUCH;
   }
   else
   {
      result = OEMBTSD_WriteRec (pMe->pOEMBTSD, uHandle, pszXML);
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_GetDiscoverableMode
=======================================================================*/
static int AEEBTSD_GetDiscoverableMode (
   IBTServiceDiscovery*    pMe,
   BTSvcDiscoverableMode*  pdMode
)
{
   int result = AEE_EBADPARM;
   if (AEE_CHECK_PTR ("SD_GetDiscoverableMode", pdMode, 
                      sizeof(BTSvcDiscoverableMode), TRUE))
   {
      result = OEMBTSD_GetDiscoverableMode (pMe->pOEMBTSD, pdMode);
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_SetDiscoverableMode
=======================================================================*/
static int AEEBTSD_SetDiscoverableMode (
   IBTServiceDiscovery*  pMe,
   BTSvcDiscoverableMode dMode
)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("SD_SetDiscoverableMode - dMode=%d", dMode);

   if (AEEBTSD_OP_SET_DISCOVERABLE_MODE & pMe->dPendingOps)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTSD_DISCOVERABLE_MODE_LIMITED >= dMode)
   {
      pMe->dPendingOps |= AEEBTSD_OP_SET_DISCOVERABLE_MODE;
      pMe->dMode = dMode;
      result = OEMBTSD_SetDiscoverableMode (pMe->pOEMBTSD, dMode);
      if (result != AEE_SUCCESS)
      {
         pMe->dPendingOps &= ~AEEBTSD_OP_SET_DISCOVERABLE_MODE;
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_StartServiceSearch
=======================================================================*/
static int AEEBTSD_StartServiceSearch (
   IBTServiceDiscovery*       pMe,
   const BDAddress*           pAddr,
   const BTSvcSearchPattern*  pPattern
)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_StartServiceSearch - entered");

   if (AEEBTSD_OP_START_SVC_SEARCH & pMe->dPendingOps)
   {
      result = AEE_EALREADY;
   }
   else if (AEEBTSD_OP_NONE != pMe->dPendingOps)
   {
      result = AEE_EITEMBUSY;
   }
   else if (!AEE_CHECK_PTR ("SD_StartServiceSearch", pAddr, 
                            sizeof(BDAddress), FALSE) ||
            !AEE_CHECK_PTR ("SD_StartServiceSearch", pPattern,
                            sizeof(BTSvcSearchPattern), FALSE) ||
            (pPattern->nNumUUID16s > AEEBTSD_MAX_SEARCH_UUID) ||
            (pPattern->nNumAttributes > AEEBTSD_MAX_SEARCH_ATTR_ID) ||
            ((pPattern->nNumUUID16s == 0) && (pPattern->nNumAttributes == 0) &&
             (pPattern->bUUID128Valid == FALSE)))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      pMe->dPendingOps = AEEBTSD_OP_START_SVC_SEARCH;
      pMe->svcSrchAddr = *pAddr;
      result = OEMBTSD_StartSvcDiscovery (pMe->pOEMBTSD, pAddr, pPattern);
      if (result != AEE_SUCCESS)
      {
         pMe->dPendingOps = AEEBTSD_OP_NONE;
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_StopServiceSearch
=======================================================================*/
static int AEEBTSD_StopServiceSearch (IBTServiceDiscovery* pMe)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_StopServiceSearch - op=%x", pMe->dPendingOps);

   if (AEEBTSD_OP_STOP_SVC_SEARCH & pMe->dPendingOps)
   {
      result = AEE_EALREADY;
   }
   else if ((AEEBTSD_OP_NONE != pMe->dPendingOps) &&
            (AEEBTSD_OP_START_SVC_SEARCH != pMe->dPendingOps))
   {
      result = AEE_EBADSTATE;
   }
   else
   {
      pMe->dPendingOps |= AEEBTSD_OP_STOP_SVC_SEARCH;
      result = OEMBTSD_StopSvcDiscovery (pMe->pOEMBTSD, &pMe->svcSrchAddr);
      if (result != AEE_SUCCESS)
      {
         pMe->dPendingOps &= ~AEEBTSD_OP_STOP_SVC_SEARCH;
      }
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_GetNumFoundRecs
=======================================================================*/
static int AEEBTSD_GetNumFoundRecs (
   IBTServiceDiscovery* pMe,
   int*                 pnFoundRecs
)
{
   int result = AEE_EBADPARM;
   if (AEE_CHECK_PTR ("SD_GetNumFoundRecs", pnFoundRecs, sizeof(int), TRUE))
   {
      *pnFoundRecs = pMe->nNumResultRecs;
      result = AEE_SUCCESS;
   }
   return result;
}

/*======================================================================= 
   AEEBTSD_GetSearchResult
=======================================================================*/
static int AEEBTSD_GetSearchResult (
   IBTServiceDiscovery* pMe,
   char*                pszXML,
   int                  nSize, 
   int*                 pnRequiredSize
)
{
   int result = AEE_EFAILED;
   if (((0 != nSize) && 
        (!AEE_CHECK_PTR ("SD_GetSearchResult", pszXML, 
                         (sizeof(char) * nSize), TRUE))) ||
       (!AEE_CHECK_PTR ("SD_GetSearchResult", pnRequiredSize,
                        sizeof(int), TRUE)))
   {
      result = AEE_EBADPARM;
   }
   else if (NULL == pMe->pszXMLResultStr)
   {
      result = AEE_EBADSTATE;
   }
   else
   {
      *pnRequiredSize = std_strlen (pMe->pszXMLResultStr) + 1;
      if (nSize < *pnRequiredSize)
      {
         result = AEE_EBUFFERTOOSMALL;
      }
      else
      {
         std_strlcpy (pszXML, pMe->pszXMLResultStr, nSize);
         result = AEE_SUCCESS;
      }
   }
   return result;
}

/************************************************************************ 
                             PUBLIC FUNCTIONS
************************************************************************/
/*======================================================================= 
AEEBTSD_New()
=======================================================================*/
int AEEBTSD_New (IShell* piShell, void** ppif)
{
   int result;
   IBTServiceDiscovery* pMe = NULL;

   DBGPRINTF ("AEEBTSD_New - entered");

   if (!AEE_CHECK_PTR ("AEEBTSD_New", ppif, sizeof(void*), TRUE))
   {
      result =  AEE_EBADPARM;
   }
   else if ((pMe = MALLOCREC (IBTServiceDiscovery)) == NULL)
   {
      result = AEE_ENOMEMORY;
   }
   else if (OEMBTSD_New (piShell, &pMe->pOEMBTSD) != AEE_SUCCESS)
   {
      result = AEE_ENOMEMORY;
   }
   else if ((result = AEEBTSD_Init (pMe, piShell)) != AEE_SUCCESS)
   {
      AEEBTSD_Cleanup (pMe);
   }
   else
   {
      *ppif = (void *)pMe;
   }
   return result;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */


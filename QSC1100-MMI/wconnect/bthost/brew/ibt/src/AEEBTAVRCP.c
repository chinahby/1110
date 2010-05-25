/*=============================================================================
FILE:  AEEBTAVRCP.c

SERVICES:  Implementation of IBTAVRCPTarget

GENERAL DESCRIPTION:
Implements Bluetooth AVRCP via the IBTAVRCPTarget interface.

===============================================================================
Copyright (c) 2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTAVRCP.c#2 $ 
$DateTime: 2009/03/17 16:13:17 $
$Author: c_hshukl $

=============================================================================*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who  what, where, why
----------   ---  -----------------------------------------------------------
2009-03-17    hs   Addition of command queues and AVRCP bug fix for 
                   disconnection failure 
2008-09-17    rs   Modifying the cleanup operation for Simulation.
2008-09-16    hs   Fixed the compiler warnings.

=========================================================================== */

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV)
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEE_OEM.h"
#include "OEMHeap.h"
#include "AEE_OEMDispatch.h"
#include "AEEIBTAVRCPTarget.h"
#include "AEEBTAVRCPTarget.bid"
#include "OEMBTAVRCPTarget.h"
#include "AEEBTUtils.h"
#include "OEMBTUtils.h"
#include "AEECriticalSection.h"
#include "AEEModel.h"
#include "AEEISignal.h"
#include "AEEISignalCtl.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalCBFactory.h"
#include "AEEISignalBus.h"
#include "AEESignalBus.bid"
#include "AEEConfig.h"
#ifndef BT_SIMULATION
#include "OEMCFGI.h"
#endif
#include "AEEBT.bid"
#include "AEEBT.h"
#include "AEEBTMgr.bid"
#include "AEEIBTMgr.h"

/*======================================================================= 
Defines
=======================================================================*/

#define BTAVRCP_PSM 0x0017
/* Maximum number of outstanding AVRCP events at a time */
#define AVRCPTG_MAX_EVENTS 5

static AEECriticalSection gAEEAVRCPCriticalSection;
#define AEEAVRCP_LOCK() AEECriticalSection_Enter(&gAEEAVRCPCriticalSection)
#define AEEAVRCP_UNLOCK() AEECriticalSection_Leave(&gAEEAVRCPCriticalSection)

typedef enum
{
   BTAVRCPOP_NONE,
   BTAVRCPOP_ENABLE,
   BTAVRCPOP_DISABLE,
   BTAVRCPOP_CONNECT,
   BTAVRCPOP_DISCONNECT
} BTAVRCPOp;

typedef struct BTAVRCPAppList
{
   /* TODO: Is there a copy of a shell for each app? Or just one? */
   IShell*                piShell;
   ACONTEXT*              pAC;
   AEEBTEvtQueue          evtAppAvrcpTgQueue;
   struct BTAVRCPAppList* pNext;
} BTAVRCPAppList;

typedef struct BTAVRCPDevList
{
   BDAddress              sAddr;
   struct BTAVRCPDevList* pNext;
} BTAVRCPDevList;

typedef struct IBTAVRCPTarget
{
   const IBTAVRCPTargetVtbl*  pvt;
   uint32               uRefs;
   OEMAVRCPTarget*      pOEMAVRCP;
   ISignalBus*          piSignalBus;

   // config info
   BTSecurityLevel      dSecurity;
   IBT*                 piBT;
   IBTMgr*              piMgr;

   // state info
   BTSecurityLevel      dTempSecurity;
   boolean              bEnabled;
   BTAVRCPAppList*      pAppList;
   BTAVRCPDevList*      pDevList;
   BTAVRCPOp            eLastOp;
   boolean              bDoDeregister; 
   AEECallback          evSignalCb;
   AEEBTEvtQueue        evtAeeOemAvrcpTgQueue;
} IBTAVRCPTarget;

/*======================================================================= 
Forward Function Declarations
=======================================================================*/

static uint32    AEEBTAVRCPTarget_AddRef (IBTAVRCPTarget*);
static uint32    AEEBTAVRCPTarget_Release (IBTAVRCPTarget*);
static int       AEEBTAVRCPTarget_QueryInterface (IBTAVRCPTarget*, AEECLSID,
                                                  void**);
static AEEResult AEEBTAVRCPTarget_Enable (IBTAVRCPTarget*, BTSecurityLevel);
static AEEResult AEEBTAVRCPTarget_Connect (IBTAVRCPTarget*, const BDAddress*);
static AEEResult AEEBTAVRCPTarget_Disconnect (IBTAVRCPTarget*,
                                              const BDAddress*);
static AEEResult AEEBTAVRCPTarget_Disable (IBTAVRCPTarget*);
static AEEResult AEEBTAVRCPTarget_GetStatus (IBTAVRCPTarget*, BTSecurityLevel*,
                                             boolean*, BDAddress*, int, int*);
static AEEResult AEEBTAVRCPTarget_OnEventNotify (IBTAVRCPTarget*, ISignal*);
static AEEResult AEEBTAVRCPTarget_GetEvent (IBTAVRCPTarget*,
                                            AEEBTAVRCPEventType*);
/*======================================================================= 
Internal Static Function Declarations
=======================================================================*/
static void AEEBTAVRCP_Cleanup (IBTAVRCPTarget*);
/*======================================================================= 
V-Table
=======================================================================*/
static const AEEVTBL(IBTAVRCPTarget) gpvtIBTAVRCPTarget = 
{
   AEEBTAVRCPTarget_AddRef,
   AEEBTAVRCPTarget_Release,
   AEEBTAVRCPTarget_QueryInterface,
   AEEBTAVRCPTarget_Enable,
   AEEBTAVRCPTarget_Connect,
   AEEBTAVRCPTarget_Disconnect,
   AEEBTAVRCPTarget_Disable,
   AEEBTAVRCPTarget_GetStatus,
   AEEBTAVRCPTarget_OnEventNotify,
   AEEBTAVRCPTarget_GetEvent,
};

/*======================================================================= 
Global Variables
=======================================================================*/

static IBTAVRCPTarget* pgAVRCP = NULL;


/************************************************************************ 
PRIVATE FUNCTIONS
************************************************************************/

static BTAVRCPAppList* AEEBTAVRCP_FindAppByAC ( IBTAVRCPTarget* pMe, 
                                               ACONTEXT* pAC )
{
   BTAVRCPAppList *result = pMe->pAppList;
   while( result )
   {
      if( result->pAC == pAC )
      {
         break;
      }
      result = result->pNext;
   }
   return result;
}

static AEEResult AEEBTAVRCP_InsertApp( IBTAVRCPTarget* pMe, 
                                      ACONTEXT* pAC, 
                                      IShell* pShell )
{
   AEEResult rv = AEE_EFAILED;
   BTAVRCPAppList *new = pMe->pAppList;

   AEEAVRCP_LOCK();

   if ( pMe->pAppList == NULL )
   {
      pMe->pAppList = OEM_Malloc(sizeof( BTAVRCPAppList) );
      new = pMe->pAppList;
   }
   else
   {
      while( new->pNext )
      {
         new = new->pNext;
      }
      new->pNext = OEM_Malloc(sizeof( BTAVRCPAppList) );
      new = new->pNext;
   }

   if( new == NULL )
   {
      rv = AEE_ENOMEMORY;
   }
   else
   {
      rv = AEE_SUCCESS;

      new->pAC = pAC;
      rv = AEEBTEvtQ_Init(&new->evtAppAvrcpTgQueue,
         AVRCPTG_MAX_EVENTS,
         sizeof(AEEBTAVRCPEventType)); 

      if (AEE_SUCCESS != rv)
      {
         OEM_Free(new);
         MSG_ERROR("AEE AVRCP Target App event queue can't be initialised",
            0,0,0);
      }
      else
      {
         new->piShell = pShell;
         ISHELL_AddRef( pShell );
      }
   }

   AEEAVRCP_UNLOCK();

   return rv;
}

static AEEResult AEEBTAVRCP_DeletAppByAC( IBTAVRCPTarget* pMe,
                                         const ACONTEXT* pAC )
{
   AEEResult rv = AEE_EFAILED;
   BTAVRCPAppList *node = pMe->pAppList;
   BTAVRCPAppList *prev = pMe->pAppList;

   AEEAVRCP_LOCK();
   while( node )
   {
      if( node->pAC == pAC )
      {
         prev->pNext = node->pNext;
         ISHELL_Release( node->piShell );
         if( node == pMe->pAppList )
         {
            if(prev->pNext)
            {
               pMe->pAppList = prev->pNext;
            }
            else
            {
               pMe->pAppList = NULL;
            }
         }
         AEEBTEvtQ_Free(&node->evtAppAvrcpTgQueue);
         OEM_Free(node);
         break;
      }
      prev = node;
      node = node->pNext;
   }
   AEEAVRCP_UNLOCK();
   return rv;
}

static AEEResult AEEBTAVRCP_FindDev( IBTAVRCPTarget* pMe,
                                    const BDAddress *pAddr )
{
   AEEResult result =  AEE_EFAILED; 
   BTAVRCPDevList *pDevList = pMe->pDevList;
   while( pDevList )
   {
      if( AEEBT_BD_ADDRS_EQUAL( &pDevList->sAddr, pAddr ) )
      {
         result = AEE_SUCCESS; 
         break;
      }
      pDevList = pDevList->pNext;
   }
   return result;
}

static AEEResult AEEBTAVRCP_InsertDev( IBTAVRCPTarget* pMe,
                                      const BDAddress *pAddr )
{
   AEEResult rv = AEE_EFAILED;
   BTAVRCPDevList *pNewDevList = pMe->pDevList;

   if ( pMe->pDevList == NULL )
   {
      pMe->pDevList = OEM_Malloc( sizeof(BTAVRCPDevList) );
      pNewDevList = pMe->pDevList;
   }
   else
   {
      while( pNewDevList->pNext )
      {
         pNewDevList = pNewDevList->pNext;
      }
      pNewDevList->pNext = OEM_Malloc( sizeof(BTAVRCPDevList) );
      pNewDevList = pNewDevList->pNext;
   }
   if( pNewDevList == NULL )
   {
      rv = AEE_ENOMEMORY;
   }
   else
   {
      rv = AEE_SUCCESS;
      pNewDevList->sAddr = *pAddr;
      pNewDevList->pNext = NULL;
   }
   return rv;
}

static AEEResult AEEBTAVRCP_DeleteDev( IBTAVRCPTarget* pMe,
                                      const BDAddress *pAddr )
{
   AEEResult rv = AEE_EFAILED;
   BTAVRCPDevList *node = pMe->pDevList;
   BTAVRCPDevList *prev = pMe->pDevList;

   while( node )
   {
      if( AEEBT_BD_ADDRS_EQUAL( &node->sAddr, pAddr ) )
      {
         prev->pNext = node->pNext;
         if( node == pMe->pDevList )
         {
            if(prev->pNext)
            {
               pMe->pDevList = prev->pNext; 
            }
            else
            {
               pMe->pDevList = NULL;
            }
         }
         FREE( node );
         rv = AEE_SUCCESS;
         break;
      }
      prev = node;
      node = node->pNext;
   }
   return rv;
}

static void AEEBTAVRCP_EvSignalCallback(void *pData)
{
   IBTAVRCPTarget* pMe = (IBTAVRCPTarget*)pData;
   AEEBTAVRCPEventType                    pEvent;
   int                                    ret = AEE_EFAILED;
   BTAVRCPAppList                        *node = NULL;

   // pop the first event on the queue.
   ret = AEEBTEvtQ_Get(&pMe->evtAeeOemAvrcpTgQueue, (uint8*)&pEvent);

   while (AEE_SUCCESS == ret)
   {
      switch( pEvent.Event )
      {
      case AEEBTAVRCP_ENABLED:
         pMe->bEnabled = TRUE; 
         if( pMe->eLastOp == BTAVRCPOP_ENABLE )
         {
            pMe->eLastOp = BTAVRCPOP_NONE;
         }
         break;
      case AEEBTAVRCP_DISABLED:
         pMe->bEnabled = FALSE; 
         pMe->eLastOp = BTAVRCPOP_NONE;
         break;

      case AEEBTAVRCP_CONNECTED:
         AEEBTAVRCP_InsertDev( pMe, &pEvent.Address ); //fall through
      case AEEBTAVRCP_CONN_FAILED:
         if( pMe->eLastOp == BTAVRCPOP_CONNECT )
         {
            pMe->eLastOp = BTAVRCPOP_NONE;
         }
         break;

      case AEEBTAVRCP_DISCONNECTED:
         AEEBTAVRCP_DeleteDev( pMe, &pEvent.Address );
         if( ( pMe->eLastOp != BTAVRCPOP_CONNECT ) && 
            ( pMe->eLastOp != BTAVRCPOP_ENABLE) &&
            ( pMe->eLastOp != BTAVRCPOP_DISABLE))
         {
            pMe->eLastOp = BTAVRCPOP_NONE;
         }
         if (( pMe->pDevList == NULL ) && (pMe->bDoDeregister ))
         {
            // There is an outstanding  Disable command pending 
            if( (ret = OEMBTAVRCP_Disable( pMe->pOEMAVRCP )) != AEE_SUCCESS )
            {
               pMe->eLastOp = BTAVRCPOP_NONE;
            }
            pMe->bDoDeregister = FALSE;
         }
         break;
      default:
         break;
      }

      if ( pMe->uRefs != 0 )
      {
         AEEAVRCP_LOCK();
         node = pMe->pAppList;
         while( node )
         {
            AEEBTEvtQ_Put(&node->evtAppAvrcpTgQueue, (uint8*)&pEvent);        
            node = node->pNext;
         }
         AEEAVRCP_UNLOCK();

         if (NULL != pMe->piSignalBus)
         {
            ISignalBus_Strobe( pMe->piSignalBus );
         }
      }

      if (( pEvent.Event  == AEEBTAVRCP_DISABLED ) && ( pMe->uRefs == 0 ))
      {
         //Invoke cleanup 
         AEEBTAVRCP_Cleanup(pMe);
      }
      ret = AEEBTEvtQ_Get(&pMe->evtAeeOemAvrcpTgQueue, (uint8*)&pEvent);
   }
}

static void AEEBTAVRCP_OEMEventCb( void* pUser, AEEBTAVRCPEventType *pEvent )
{
   /* This callback will be called in the OEM's task context */
   IBTAVRCPTarget *pMe = (IBTAVRCPTarget*) pUser;

   CALLBACK_Cancel(&pMe->evSignalCb);
   AEEBTEvtQ_Put(&pMe->evtAeeOemAvrcpTgQueue, (uint8*)pEvent);
   CALLBACK_Init(&pMe->evSignalCb, AEEBTAVRCP_EvSignalCallback, pMe);
   AEE_SYS_RESUME(&pMe->evSignalCb);
}

static void AEEBTAVRCP_EventCbDeregister( IBTAVRCPTarget* pMe )
{
   if( pMe->pOEMAVRCP != NULL )
   {
      OEMBTAVRCP_EventCbDeregister( pMe->pOEMAVRCP );
   }
}

static int AEEBTAVRCP_EventCbRegister( IBTAVRCPTarget* pMe )
{
   int result = AEE_EFAILED;
   result = OEMBTAVRCP_EventCbRegister( pMe->pOEMAVRCP, AEEBTAVRCP_OEMEventCb, pMe );
   return result;
}

static int AEEBTAVRCP_UpdateSecurity( IBTAVRCPTarget* pMe,
                                     BTSecurityLevel dSec )
{
   int             result = AEE_EFAILED;
   BTAVRCPAppList *App;

   if( (App = AEEBTAVRCP_FindAppByAC( pMe, AEE_GetAppContext() ))
      == NULL )
   {
      return AEE_ERESOURCENOTFOUND;
   }

   if( (NULL == pMe->piBT) &&
      (ISHELL_CreateInstance( App->piShell, AEECLSID_BT, 
      (void**)&pMe->piBT) != SUCCESS) )
   {
      DBGPRINTF_ERROR( "AVRCP_UpdateSec - failed to create IBT" );
   }
   else if( (NULL == pMe->piMgr) &&
      (IBT_GetObject( pMe->piBT, AEECLSID_BTMgr, NULL, 0,
      (void**)&pMe->piMgr) != AEE_SUCCESS) )
   {
      DBGPRINTF_ERROR( "AVRCP_UpdateSec - failed to create IBTMgr" );
   }
   else
   {
      BTServiceIdentifier
         svc2protect = { AEEBT_SEC_BY_SERVICE_CLASS,
         2,
      { 0, 0, 0, 0, 0 },
      { AEEBT_SCU_AV_REMOTE_CONTROL_TARGET,
      AEEBT_SCU_AV_REMOTE_CONTROL,0,0,0 },
      { 0, 0, 0, 0, 0 },
      { BTAVRCP_PSM, BTAVRCP_PSM, 0, 0, 0 }
      };
      if( ( result = 
         IBTMgr_SetSvcSecurity( pMe->piMgr, dSec, &svc2protect) )
         != AEE_SUCCESS )
      {
         DBGPRINTF_ERROR( "AVRCP_Config - failed to set sec r=%d", result );
      }
      else
      {
         pMe->dSecurity = dSec;
      }
   }
   return result;
}

static void AEEBTAVRCP_Cleanup( IBTAVRCPTarget* pMe )
{
   DBGPRINTF( "BTAVRCP_Cleanup - entered" );

   if( AEEBT_SEC_NONE < pMe->dSecurity )
   {
      AEEBTAVRCP_UpdateSecurity( pMe, AEEBT_SEC_NONE );
      // not going to wait for result
   }
   // We are not going to send events to App anymore...
   if ( pMe->piSignalBus != NULL )
   {
      ISignalBus_Release(pMe->piSignalBus);
      pMe->piSignalBus  = NULL;
   }
   if( pMe->piMgr != NULL )
   {
      IBTMgr_Release (pMe->piMgr);
      pMe->piMgr = NULL;
   }
   if( pMe->piBT != NULL )
   {
      IBT_Release (pMe->piBT);
      pMe->piBT = NULL;
   }
   CALLBACK_Cancel(&pMe->evSignalCb);

#ifndef BT_SIMULATION
   // In case of Simulation When the Simulator is closed, BREW unloads
   // and we don't have a way to get notified about disable which 
   // results in a memory leak
   if ( pMe->bEnabled == TRUE )
   {
      //Disable first and wait for disabled Event....
      AEEBTAVRCPTarget_Disable(pMe);
      return;
   }
#endif
   AEEBTAVRCP_EventCbDeregister( pMe );
   if( pMe->pOEMAVRCP != NULL )
   {
      OEMBTAVRCP_Release( pMe->pOEMAVRCP );
      pMe->pOEMAVRCP = NULL;
   }
   /* Freeing the queue */
   AEEBTEvtQ_Free(&pMe->evtAeeOemAvrcpTgQueue);
   AEEAVRCP_LOCK();
   while( pMe->pAppList )
   {
      BTAVRCPAppList *temp;

      temp = pMe->pAppList;
      pMe->pAppList = pMe->pAppList->pNext;
      AEEBTAVRCP_DeletAppByAC (pMe, temp->pAC );
   }
   AEEAVRCP_UNLOCK();

   while( pMe->pDevList )
   {
      BTAVRCPDevList *temp;

      temp = pMe->pDevList;
      pMe->pDevList = pMe->pDevList->pNext;
      OEM_Free(temp);
   }

   OEM_Free( pMe );
   pgAVRCP = NULL;
}

static int AEEBTAVRCP_Init (IBTAVRCPTarget* pMe, IShell* piShell)
{
   int result = AEE_EFAILED;

   DBGPRINTF ("AEEBTAVRCP_Init - entered");

   pgAVRCP            = pMe;
   pMe->pvt           = &gpvtIBTAVRCPTarget;
   pMe->uRefs         = 1;
   pMe->bEnabled      = FALSE;
   pMe->pAppList      = NULL;
   pMe->pDevList      = NULL;
   pMe->eLastOp       = BTAVRCPOP_NONE;
   pMe->bDoDeregister = FALSE;

   if( AEEBTAVRCP_InsertApp( pMe, AEE_GetAppContext(), piShell )
      != AEE_SUCCESS )
   {
      return AEE_ENOMEMORY;
   }

   if( ISHELL_CreateInstance( piShell, AEECLSID_SignalBus,
      (void**)&pMe->piSignalBus ) 
      != SUCCESS )
   {
      DBGPRINTF_ERROR( "AEEBTAVRCP_Init - failed to create signal bus" );
   }
   else if( AEEBTAVRCP_EventCbRegister( pMe ) != SUCCESS )
   {
      DBGPRINTF_ERROR ("AEEBTAVRCP_Init - failed to register EvCb");
   }
   else if( AEEBTEvtQ_Init(&pMe->evtAeeOemAvrcpTgQueue,
      AVRCPTG_MAX_EVENTS,
      sizeof(AEEBTAVRCPEventType))
      != AEE_SUCCESS )
   {
      MSG_ERROR("AEE AVRCP Target Aee event queue can't be initialised",
         0,0,0);
   }
   else
   {
      result = AEE_SUCCESS;
   }

   if( result != AEE_SUCCESS )
   {
      AEEBTAVRCP_Cleanup( pMe );
   }

   return result;
}
/************************************************************************ 
PUBLIC FUNCTIONS
************************************************************************/

int AEEBTAVRCP_New (IShell* pIShell, void** ppif)
{
   int result;

   DBGPRINTF ("AEEBTAVRCP_New - entered");

   if( ISHELL_CheckPrivLevel(pIShell, AEECLSID_BTAVRCPTarget, TRUE) == FALSE )
   {
      result = AEE_EPRIVLEVEL;
   }
   else if( !AEE_CHECK_PTR("AEEBTAVRCP_New", ppif, sizeof(void*), TRUE) )
   {
      result =  AEE_EBADPARM;
   }
   else if( pgAVRCP != NULL )
   {
      *ppif = (void *)pgAVRCP;
      AEEBTAVRCPTarget_AddRef( pgAVRCP );
      if( AEEBTAVRCP_InsertApp( pgAVRCP, AEE_GetAppContext(), pIShell )
         != AEE_SUCCESS )
      {
         return AEE_ENOMEMORY;
      }
      return AEE_SUCCESS;
   }
   else
   {
      IBTAVRCPTarget *pMe = NULL;
      result = AEE_EFAILED;

      if( (pMe = OEM_Malloc( sizeof(IBTAVRCPTarget) )) == NULL)
      {
         result = AEE_ENOMEMORY;
      }
      else if( OEMBTAVRCP_New( pIShell, &( pMe->pOEMAVRCP ) ) != AEE_SUCCESS)
      {
         result = AEE_ENOMEMORY;
      }
      else
      {
         result = AEEBTAVRCP_Init (pMe, pIShell);
      }

      if( (AEE_SUCCESS != result) && (NULL != pMe) )
      {
         AEEBTAVRCP_Cleanup (pMe);
      }
      else
      {
         *ppif = (void *)pgAVRCP;
      }
   }

   return result;
}

/************************************************************************ 
VTABLE FUNCTIONS
************************************************************************/
static uint32 AEEBTAVRCPTarget_AddRef (IBTAVRCPTarget* pMe)
{
   // increment ref count
   if( pMe != NULL )
   {
      return ++pMe->uRefs;
   }
   return 0;
}

static uint32 AEEBTAVRCPTarget_Release (IBTAVRCPTarget* pMe)
{
   uint32 uRefs;

   if( 0 == pMe->uRefs )
   {
      return pMe->uRefs;
   }
   DBGPRINTF_MED ("AEEBTAVRCP_Release - ref count = %d", pMe->uRefs);

   uRefs = --pMe->uRefs;

   if(  0 == uRefs )
   {
      AEEBTAVRCP_Cleanup (pMe);
   }

   return uRefs;
}

static int AEEBTAVRCPTarget_QueryInterface ( IBTAVRCPTarget* pMe,
                                            AEECLSID clsID,
                                            void** ppObj )
{
   if( !AEE_CHECK_PTR("AEEBTAVRCP_QueryInterface", ppObj, sizeof(void*), TRUE) )
   {
      return EMEMPTR;
   }

   if( (AEEIID_IQI == clsID) ||
      (AEEIID_IBTAVRCPTarget == clsID) ||
      (AEECLSID_BTAVRCPTarget == clsID) )
   {
      *ppObj = (IQueryInterface*)pMe;
      IQI_AddRef (*ppObj);
      return SUCCESS;
   }
   return ECLASSNOTSUPPORT;
}

static AEEResult AEEBTAVRCPTarget_Enable (IBTAVRCPTarget* pMe,
                                          BTSecurityLevel SecLvl)
{
   int result = AEE_EFAILED;

   DBGPRINTF ("AVRCP_Enable");

   if( pMe->eLastOp == BTAVRCPOP_ENABLE )
   {
      result = AEE_EALREADY;
   }
   else if( pMe->eLastOp != BTAVRCPOP_NONE )
   {
      result = AEE_EITEMBUSY;
   }
   else if( pMe->bEnabled == TRUE )
   {
      result = AEE_EBADSTATE;
   }
   else
   {
      pMe->eLastOp = BTAVRCPOP_ENABLE;
      AEEBTAVRCP_UpdateSecurity( pMe, SecLvl );
      if( (result = OEMBTAVRCP_Enable( pMe->pOEMAVRCP )) != AEE_SUCCESS )
      {
         pMe->eLastOp = BTAVRCPOP_NONE;
      }
   }
   return result;  
}

static AEEResult AEEBTAVRCPTarget_Connect (IBTAVRCPTarget* pMe,
                                           const BDAddress* pBDAddr)
{
   int result = AEE_EFAILED;

   DBGPRINTF ("AVRCP_Connect");

   if( pMe->eLastOp == BTAVRCPOP_CONNECT )
   {
      result = AEE_EALREADY;
   }
   else if( pMe->eLastOp != BTAVRCPOP_NONE )
   {
      result = AEE_EITEMBUSY;
   }
   else if( pMe->bEnabled == FALSE )
   {
      result = AEE_EBADSTATE;
   }
   else
   {
      pMe->eLastOp = BTAVRCPOP_CONNECT;
      if( (result = OEMBTAVRCP_Connect( pMe->pOEMAVRCP, pBDAddr )) 
         != AEE_SUCCESS )
      {
         pMe->eLastOp = BTAVRCPOP_NONE;
      }
   }
   return result;  
}

static AEEResult AEEBTAVRCPTarget_Disconnect( IBTAVRCPTarget* pMe,
                                             const BDAddress* pBDAddr )
{
   int result = AEE_EFAILED;

   DBGPRINTF ("AVRCP_Disconnect");

   if( pMe->eLastOp == BTAVRCPOP_DISCONNECT )
   {
      result = AEE_EALREADY;
   }
   else if( pMe->eLastOp != BTAVRCPOP_NONE )
   {
      result = AEE_EITEMBUSY;
   }
   else if( AEEBTAVRCP_FindDev( pMe, pBDAddr ) == AEE_SUCCESS )
   {
      pMe->eLastOp = BTAVRCPOP_DISCONNECT;
      if( (result = OEMBTAVRCP_Disconnect( pMe->pOEMAVRCP, pBDAddr ))
         != AEE_SUCCESS )
      {
         pMe->eLastOp = BTAVRCPOP_NONE;
      }
   }
   else
   {
      result = AEE_ERESOURCENOTFOUND;
   }
   return result;  
}

static AEEResult AEEBTAVRCPTarget_Disable( IBTAVRCPTarget* pMe )
{
   int result = AEE_EFAILED;
   BTAVRCPDevList *pDev = pMe->pDevList;

   DBGPRINTF( "AVRCP_Disable" );

   if( pMe->eLastOp == BTAVRCPOP_DISABLE )
   {
      result = AEE_EALREADY;
   }
   else if( pMe->eLastOp != BTAVRCPOP_NONE )
   {
      result = AEE_EITEMBUSY;
   }
   else if( pMe->bEnabled != TRUE )
   {
      result = AEE_EBADSTATE;
   }
   else
   {
      pMe->eLastOp = BTAVRCPOP_DISABLE;
      //Disconnect all the connected Devices first
      while ( pDev )
      {
         if( (result = OEMBTAVRCP_Disconnect( pMe->pOEMAVRCP, &(pDev->sAddr )) )
            != AEE_SUCCESS )
         {
            pMe->eLastOp = BTAVRCPOP_NONE;
         }
         pDev = pDev->pNext;       
      }

      if ( pMe->pDevList == NULL ) // No outstanding connection...
      {
         if( (result = OEMBTAVRCP_Disable( pMe->pOEMAVRCP )) != AEE_SUCCESS )
         {
            pMe->eLastOp = BTAVRCPOP_NONE;
         }
      }
      else
      {
         pMe->bDoDeregister = TRUE;
      }
   }
   return result;  
}

static AEEResult AEEBTAVRCPTarget_GetStatus( IBTAVRCPTarget* pMe,
                                            BTSecurityLevel *pSecLevel,
                                            boolean* pEnabled, 
                                            BDAddress* pAddrList, int dSize,
                                            int* pSizeReq )
{
   BTAVRCPDevList *pDevList = pMe->pDevList;
   int i = 0;  
   *pEnabled = pMe->bEnabled; 
   *pSecLevel = pMe->dSecurity;

   while(( pDevList ) && ( i < dSize ))
   {
      MEMCPY(&(pAddrList[i]),&(pDevList->sAddr), sizeof (BDAddress));
      pDevList = pDevList->pNext;
      i++; 
   }
   /* Check if we have more devices in local list */
   while (pDevList)
   {
      pDevList = pDevList->pNext;
      i++; 
   }

   *pSizeReq = i;

   if ( *pSizeReq  <= dSize )
   {
      return AEE_SUCCESS;
   }
   else
   {
      return AEE_EBUFFERTOOSMALL;
   }
}

static AEEResult AEEBTAVRCPTarget_OnEventNotify( IBTAVRCPTarget* pMe,
                                                ISignal* piSig )
{
   int result = AEE_EFAILED;
   BTAVRCPAppList *pApp = AEEBTAVRCP_FindAppByAC( pMe, AEE_GetAppContext() );

   if( piSig == NULL )
   {
      result = AEE_EBADPARM;
   }
   else if( pApp == NULL )
   {
      result = AEE_EFAILED;
   }
   else
   {
      ISignalBus_Remove (pMe->piSignalBus, piSig);
      ISignalBus_Add (pMe->piSignalBus, piSig);
      result = AEE_SUCCESS;
   }
   return result;
}

static AEEResult AEEBTAVRCPTarget_GetEvent (IBTAVRCPTarget* pMe,
                                            AEEBTAVRCPEventType* pEvent)
{
   int result = AEE_EFAILED;
   BTAVRCPAppList *pApp;

   pApp = AEEBTAVRCP_FindAppByAC( pMe, AEE_GetAppContext() );
   if( pApp == NULL )
   {
      return result;
   }

   if( !AEE_CHECK_PTR ("AEEBTAVRCP_GetEvent", 
      pEvent,
      sizeof(AEEBTAVRCPEventType),
      TRUE) )
   {
      result = AEE_EBADPARM;
   }
   else
   {
      MEMSET((void*)pEvent, 0, sizeof(AEEBTAVRCPEventType));
      AEEAVRCP_LOCK();
      {
         // pop the first event on the queue.
         result = AEEBTEvtQ_Get(&pApp->evtAppAvrcpTgQueue, (uint8*)pEvent);
      }
      AEEAVRCP_UNLOCK();
      MSG_MED("AEEBTAVRCP TG Ext: get event ret %x <-",result,0,0);
   }  
   return result;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV) */


/*============================================================================
FILE:  AEEBTA2DP.c

SERVICES:  Implementation of IBTA2DP interface

GENERAL DESCRIPTION:
   Implements IBTA2DP and IBTA2DPNotifier interfaces

===============================================================================
           Copyright (c) 2007-2008 QUALCOMM Incorporated.
                All Rights Reserved.
           Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTA2DP.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV)
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEEShell.h"
#include "AEE_OEMDispatch.h"
#include "AEEISignalBus.h"
#include "AEEISignalCtl.h"
#include "AEESignalBus.bid"
#include "AEE_OEMDispatch.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalCBFactory.h"

#include "AEEIBTA2DP.h"
#include "AEEIBTA2DPNotifier.h"
#include "OEMBTA2DP.h"
#include "AEEBTUtils.h"
#include "AEEBTDef.h"
#include "AEEBT.bid"
#include "AEEBT.h"
#include "AEEBTMgr.bid"
#include "AEEIBTMgr.h"
#include "AEE_OEM.h"
//=============================================================================
//  Type definitions
//============================================================================= 
#define AEE_A2DP_MAX_CONNECTIONS   1
#define AEEBTA2DP_MAX_BDA_LEN      6
#define AEEBTA2DP_MAX_EVENTS       10


//=============================================================================
//  A2DP states
//============================================================================= 
#define AEE_A2DP_STATE_INIT                   0
#define AEE_A2DP_STATE_ENABLE_PEND            1
#define AEE_A2DP_STATE_ENABLED                2
#define AEE_A2DP_STATE_CONN_PEND              3
#define AEE_A2DP_STATE_CONNECTED              4
#define AEE_A2DP_STATE_DISC_PEND              5
#define AEE_A2DP_STATE_DISABLE_PEND           6


//=============================================================================
// AEE BT A2DP connection struct
//=============================================================================
typedef struct 
{
   BDAddress bdAddr;
   uint32    state;
   boolean   bFree;
}AEEBTA2DPConn;
//=============================================================================
// AEE BT A2DP Object
//=============================================================================
struct IBTA2DP {
   const IBTA2DPVtbl   *pvt;
   uint32              uRef;
   IShell              *pIShell;
   AEECallback         cbAppClose;    // callback that is called by BREW when 
                                      // app terminates
   uint32              state;
   AEECallback         evSignalCb;    
   AEEBTEvtQueue       sysEvtQueue;
   OEMBTA2DP           *pOemObj;
   int                 numCon;
   AEEBTA2DPConn       aConn[AEE_A2DP_MAX_CONNECTIONS];
   IBT*                piBT;
   IBTMgr*             piMgr;
   BTSecurityLevel     btSec;
};
//=============================================================================
// AEE BT A2DP notifier object
//=============================================================================
struct IBTA2DPNotifier {
   const IBTA2DPNotifierVtbl  *pvt;
   uint32                     uRef;
   IShell                     *pIShell;
   AEEBTEvtQueue              appEvtQueue;
   AEECallback                cbAppClose; 
   IBTA2DPNotifier            *pNextObject; 
   ISignalBus                 *piSigBusEvt;
};
//=============================================================================
// AEE BT A2DP notifier object manager
//=============================================================================
typedef struct
{
  IBTA2DPNotifier  *pHeadObject;    // singly linked list of OBEX objects
} AEEBTA2DPNotifMgr;

//=============================================================================
// IBTA2DPNotifier Interface functions 
//=============================================================================
static uint32 AEEBTA2DPNotif_AddRef(IBTA2DPNotifier *p);
static uint32 AEEBTA2DPNotif_Release(IBTA2DPNotifier *p);
static int AEEBTA2DPNotif_QueryInterface(IBTA2DPNotifier *p,
                                         AEECLSID idReq, void **ppo);
static int AEEBTA2DPNotif_OnEventNotify(IBTA2DPNotifier *p,
                                        ISignal         *piSignal);
static int AEEBTA2DPNotif_GetEvent(IBTA2DPNotifier *p,
                                   BTA2DPEvent     *pEvent);

//=============================================================================
// Interface function definitions
//=============================================================================
static uint32 AEEBTA2DP_AddRef(IBTA2DP *p);
static uint32 AEEBTA2DP_Release(IBTA2DP *p);
static int AEEBTA2DP_QueryInterface(IBTA2DP *p,
                                  AEECLSID idReq, void **ppo);
static int AEEBTA2DP_Enable(IBTA2DP *p, BTSecurityLevel sec);
static int AEEBTA2DP_Disable(IBTA2DP *p);
static int AEEBTA2DP_Connect(IBTA2DP *p, const BDAddress *pBDAddr);
static int AEEBTA2DP_Disconnect(IBTA2DP *p, const BDAddress *pBDAddr);
static int AEEBTA2DP_SetDevice(IBTA2DP *p, const BDAddress *pBDAddr);
static int AEEBTA2DP_Configure(IBTA2DP *p,
                             const BDAddress *pBDAddr,
                             BTA2DPConfig config,
                             const char *pszConfig);
static int AEEBTA2DP_GetConfig(IBTA2DP   *p,
                             const BDAddress *pBDAddr,
                             char      *pszConfig,
                             int       nlen,
                             int       *pnReqLen);

/*=============================================================================
 Static functions
=============================================================================*/
static int AEEBTA2DP_Delete(IBTA2DP *me);
static void AEEBTA2DP_OEMEventCb( void            *cbData,
                                  OEMBTA2DPEvent    *pEvt);
static void AEEBTA2DP_SysEvtCallback(void* pme);
static uint32 AEEBTA2DPNotif_Delete(IBTA2DPNotifier *me);
static AEEBTA2DPConn* AEEBTA2DP_FindConnection(IBTA2DP *me,
                                               const BDAddress *pBDAddr,
                                               boolean bCreate);
static void AEEBTA2DP_ConInit(AEEBTA2DPConn *pCon);
static int AEEBTA2DP_UpdateSecurity (IBTA2DP *me, BTSecurityLevel dSec);
/*=============================================================================
 IBTA2DP vector table
=============================================================================*/
static const IBTA2DPVtbl g_Vtbl_BTA2DP = {
   AEEBTA2DP_AddRef,
   AEEBTA2DP_Release,
   AEEBTA2DP_QueryInterface,
   AEEBTA2DP_Enable,
   AEEBTA2DP_Disable,
   AEEBTA2DP_SetDevice,
   AEEBTA2DP_Connect,
   AEEBTA2DP_Disconnect,
   AEEBTA2DP_Configure,
   AEEBTA2DP_GetConfig
};
/*=============================================================================
 IBTA2DPNotifier vector table
=============================================================================*/
static const IBTA2DPNotifierVtbl g_Vtbl_BTA2DPNotifier = {
   AEEBTA2DPNotif_AddRef,
   AEEBTA2DPNotif_Release,
   AEEBTA2DPNotif_QueryInterface,
   AEEBTA2DPNotif_OnEventNotify,
   AEEBTA2DPNotif_GetEvent
};
/*=============================================================================
 Globals
=============================================================================*/
static IBTA2DP                   *gpA2dp; // to keep track if the object is singleton
static AEEBTA2DPNotifMgr         gAEEBTA2DPNotifMgr;
//=============================================================================
// 
//
//=============================================================================
int AEEBTA2DP_New(IShell* pIShell, AEECLSID cls, void **ppif)
{
   IBTA2DP *me  = NULL;
   int ret      = AEE_SUCCESS;
   
   MSG_DEBUG("AEE a2dp new ->",0,0,0);

   if (NULL == ppif)
   {
      MSG_ERROR("AEE a2dp new ppif = NULL ",0,0,0);
      return AEE_EBADPARM;
   }
   if (gpA2dp != NULL)
   {
      MSG_ERROR("AEE a2dp singleton obj. cannot create another inst ",0,0,0);
      return AEE_EUNSUPPORTED;
   }
   // allocate AEE object
   me = MALLOCREC(IBTA2DP);

   if (NULL == me)
   {
      MSG_ERROR("AEE a2dp cannot allocate me",0,0,0);
      return AEE_ENOMEMORY;
   }

   MEMSET((void*)me, 0, sizeof(IBTA2DP));
   
   ret = AEEBTEvtQ_Init(&me->sysEvtQueue,
                        AEEBTA2DP_MAX_EVENTS,
                        sizeof(OEMBTA2DPEvent));

   if (AEE_SUCCESS == ret)
   {
      // register with OEM
      ret = OEMBTA2DP_Init(&me->pOemObj, AEEBTA2DP_OEMEventCb, me);
   }
   if (AEE_SUCCESS == ret)
   {
      int i = 0;
      MSG_ERROR("AEE a2dp init done ...",0,0,0);
      // initialize the object
      me->pvt             = &g_Vtbl_BTA2DP;
      me->uRef            = 1;
      me->pIShell         = pIShell;

      for ( i = 0; i < AEE_A2DP_MAX_CONNECTIONS; i++)
      {
         AEEBTA2DP_ConInit(&me->aConn[i]);
      }
      
      // Initialize the cb that will be called in case the app closes
      CALLBACK_Init(&me->cbAppClose, AEEBTA2DP_Delete, me);
      AEE_LinkSysObject(&me->cbAppClose);
      ISHELL_AddRef(pIShell);
      // return the interface
      *ppif = (void *)me;
      gpA2dp = me;
   }
   else
   {
      MSG_ERROR("AEE a2dp new failed clean up...",0,0,0);
      if (NULL != me->pOemObj)
      {
         OEMBTA2DP_Release(me->pOemObj);
         me->pOemObj = NULL;
      }
      AEEBTEvtQ_Free(&me->sysEvtQueue);
      FREEIF(me);
   }
   MSG_DEBUG("AEE a2dp new <-",0,0,0);
   return ret;
}
static void AEEBTA2DP_ConInit(AEEBTA2DPConn *pCon)
{
   MEMSET((void*)pCon, 0, sizeof(AEEBTA2DPConn));
   pCon->bFree = TRUE;
   return;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEBTA2DP_AddRef(IBTA2DP *me)
{
   MSG_DEBUG("AEE a2dp add ref %x ",me->uRef,0,0);

   return ++me->uRef;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEBTA2DP_Release(IBTA2DP *me)
{
   uint32 uRef = --me->uRef;

   MSG_DEBUG("AEE a2dp release -> ",0,0,0);

   if (0 == uRef)
   {
      MSG_HIGH("AEE a2dp uRef 0.. calling delete ",0,0,0);
      AEEBTA2DP_Delete(me);
      gpA2dp = NULL;
   }

   MSG_DEBUG("AEE a2dp release uRef <-",uRef,0,0);

   return uRef;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEBTA2DP_Delete(IBTA2DP *me)
{
   MSG_DEBUG("AEE a2dp delete ->",0,0,0);

   if (me->pOemObj)
   {
      MSG_HIGH("AEE a2dp caling oem release...",0,0,0);
      OEMBTA2DP_Release(me->pOemObj);
      me->pOemObj = NULL;
   }

   CALLBACK_Cancel(&me->evSignalCb);
   if (NULL != me->piMgr)
   {
      AEEBTA2DP_UpdateSecurity (me, AEEBT_SEC_NONE);
      IBTMgr_Release (me->piMgr);
      me->piMgr = NULL;
   }
   if (NULL != me->piBT)
   {
      IBT_Release (me->piBT);
      me->piBT = NULL;
   }
   AEEBTEvtQ_Free(&me->sysEvtQueue);
   CALLBACK_Cancel(&me->cbAppClose);
   RELEASEIF(me->pIShell);
   FREEIF(me);

   MSG_DEBUG("AEE a2dp delete <-",0,0,0);
   return AEE_SUCCESS; 
}
//=============================================================================
// 
//
//=============================================================================
static int AEEBTA2DP_QueryInterface(IBTA2DP *me,
                                    AEECLSID iid, void **ppo)
{
   switch (iid)
   {
      case AEEIID_IQI:
      case AEEIID_IBTA2DP:
         *ppo = me;
         break;

      default:
         *ppo = 0;
         return AEE_ECLASSNOTSUPPORT;
   }
   IQI_AddRef(*ppo);
   return AEE_SUCCESS;
}
//=============================================================================
// 
// IBTA2DP_Enable
//=============================================================================
static int AEEBTA2DP_Enable(IBTA2DP *me, BTSecurityLevel sec)
{
   int ret = AEE_SUCCESS;

   MSG_DEBUG("AEE a2dp enable ->",0,0,0);

   if (sec > AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT)
   {
      MSG_ERROR("AEE a2dp invalid security level %x",sec,0,0);
      return AEE_EBADPARM;
   }

   if (me->state != AEE_A2DP_STATE_INIT)
   {
      MSG_ERROR("AEE a2dp invalid state %x",me->state,0,0);
      return AEE_EBADSTATE;
   }

   ret = OEMBTA2DP_Enable(me->pOemObj);

   if (AEE_SUCCESS == ret)
   {
      me->btSec = sec;
      me->state = AEE_A2DP_STATE_ENABLE_PEND;
   }

   MSG_DEBUG("AEE a2dp enable ret %x <-",ret,0,0);

   return ret;
}

//=============================================================================
//
//
//=============================================================================
static int AEEBTA2DP_Disable(IBTA2DP *me)
{
   int ret = AEE_SUCCESS;

   MSG_DEBUG("AEE a2dp disable ->",0,0,0);

   if (me->state < AEE_A2DP_STATE_ENABLED)
   {
      return AEE_EBADSTATE;
   }

   ret = OEMBTA2DP_Disable(me->pOemObj);

   if (AEE_SUCCESS == ret)
   {
      me->state = AEE_A2DP_STATE_DISABLE_PEND;
   }
   else
   {
      MSG_ERROR("AEE a2dp cannot disable ret %x <-",ret,0,0);
   }

   MSG_DEBUG("AEE a2dp disable ret %x <-",ret,0,0);

   return ret;
}


//=============================================================================
// 
//
//=============================================================================
static int AEEBTA2DP_Connect(IBTA2DP *me, const BDAddress *pBDAddr)
{
   int ret = AEE_SUCCESS;
   AEEBTA2DPConn  *pCurCon = NULL;
  

   MSG_DEBUG("AEE a2dp connect ->",0,0,0);

   if ((NULL == pBDAddr) || 
       (!AEE_CHECK_PTR("AEEBTA2DP_Connect",
                        pBDAddr, sizeof(BDAddress), FALSE)))
   {
      MSG_ERROR("AEE a2dp connect bad param %x",pBDAddr,0,0);
      return AEE_EBADPARM;
   }

   // check if the engine is enabled 
   if (AEE_A2DP_STATE_INIT == me->state)
   {
      MSG_ERROR("AEE a2dp connect not enabled state %x",me->state,0,0);
      return AEE_EBADSTATE;
   }

   if (me->state != AEE_A2DP_STATE_ENABLED)
   {
      MSG_HIGH("AEE a2dp connect item busy state %x",me->state,0,0);
      return AEE_EITEMBUSY;
   }
   if (me->numCon == AEE_A2DP_MAX_CONNECTIONS)
   {
      return AEE_ENOMORE;
   }
   pCurCon = AEEBTA2DP_FindConnection(me, pBDAddr, TRUE);

   if (NULL == pCurCon)
   {
      MSG_ERROR ("AEE a2dp connect cannot handle any more con!",0,0,0);
      ret = AEE_ENOMORE;
   }
   else if (AEE_A2DP_STATE_CONNECTED == pCurCon->state)
   {
      ret = AEE_EBADSTATE; // already connected to given device
   }
   else
   {
      // try to connect to the device
      ret = OEMBTA2DP_Connect(me->pOemObj, pBDAddr);

      if (AEE_SUCCESS == ret)
      {
         // mark the connection obj as busy
          pCurCon->bFree = FALSE;
         // mark the connection state as conn pending
          pCurCon->state = AEE_A2DP_STATE_CONN_PEND;
         // mark the engine state 
          me->state = AEE_A2DP_STATE_CONN_PEND;

          MSG_MED("AEE a2dp connect pending %x",me->state,0,0);
      }
      else
      {
          AEEBTA2DP_ConInit( pCurCon );
      }
   }

   MSG_DEBUG("AEE a2dp connect ret %x <-",ret,0,0);

   return ret;
}


//=============================================================================
// 
//
//=============================================================================
static int AEEBTA2DP_Disconnect(IBTA2DP *me, const BDAddress *pBDAddr)
{
   int i = 0;
   int ret = AEE_SUCCESS;
   AEEBTA2DPConn  *pCurCon = NULL;

   MSG_DEBUG("AEE a2dp disconnect ->",0,0,0);

   if ((NULL == pBDAddr) || 
       (!AEE_CHECK_PTR("AEEBTA2DP_Disconnect",
                        pBDAddr, sizeof(BDAddress), FALSE)))
   {
      MSG_ERROR("AEE a2dp disconnect bad param %x",pBDAddr,0,0);
      return AEE_EBADPARM;
   }
   // cannot start an operation while one is already in progress
   if (me->state != AEE_A2DP_STATE_ENABLED)
   {
      MSG_HIGH("AEE a2dp disconnect item busy state %x",me->state,0,0);
      return AEE_EITEMBUSY;
   }

   pCurCon = AEEBTA2DP_FindConnection(me, pBDAddr, FALSE);

   if (NULL == pCurCon)
   {
      MSG_ERROR ("AEE a2dp connect cannot handle any more con!",0,0,0);
      ret = AEE_EBADPARM;
   }
   else if (AEE_A2DP_STATE_CONNECTED != pCurCon->state)
   {
      ret = AEE_EBADSTATE; // already connected to given device
   }
   else
   {
      ret = OEMBTA2DP_Disconnect(me->pOemObj, pBDAddr);

      if (AEE_SUCCESS == ret)
      {
         me->aConn[i].state = AEE_A2DP_STATE_DISC_PEND;
         me->state = AEE_A2DP_STATE_DISC_PEND;
         MSG_MED("AEE a2dp connect pending %x, iter %x",me->state,i,0);
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEBTA2DP_SetDevice(IBTA2DP *me, const BDAddress *pBDAddr)
{
   int ret = AEE_SUCCESS;
   int i = 0;

   for (i = 0; i < AEE_A2DP_MAX_CONNECTIONS; i++)
   {
      if ( me->aConn[i].state == AEE_A2DP_STATE_CONNECTED)
      {
         break;
      }
   }
   if (AEE_A2DP_MAX_CONNECTIONS != i)
   {
      ret = AEE_EBADSTATE;
   }
   else
   {
      // go through the list and see if any device is in connection
      ret = OEMBTA2DP_SetDevice(me->pOemObj, pBDAddr);
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEBTA2DP_Configure(IBTA2DP *me,
                               const BDAddress *pBDAddr,
                               BTA2DPConfig config,
                               const char *pszConfig)
{
   int ret = AEE_SUCCESS; 
   AEEBTA2DPConn  *pCon = NULL;
   MSG_MED("AEE a2dp config %x, bda %x, szcfg %x ->",config,pBDAddr,pszConfig);
   

   if ((config == 0) || ( config > AEEA2DP_CFG_LOWEST_BITRATE))
   {
      return AEE_EUNSUPPORTED;
   }
   if (NULL != pBDAddr)
   {
      // see if the bdaddr is in the array of connected devices
      pCon = AEEBTA2DP_FindConnection(me, pBDAddr, FALSE);
      if (NULL != pCon)
      {
         if (pCon->state != AEE_A2DP_STATE_CONNECTED)
         {
            ret = AEE_EBADSTATE;
         }
      }
      else
      {
         ret = AEE_EBADPARM;
      }
   }
   if (AEE_SUCCESS == ret )
   {
      ret = OEMBTA2DP_Config(me->pOemObj, pBDAddr ,config);
   }
   MSG_MED("AEE a2dp config ret %x <- ",ret,0,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEBTA2DP_GetConfig(IBTA2DP   *me,
                             const BDAddress *pBDAddr,
                             char      *pszConfig,
                             int       nlen,
                             int       *pnReqLen)
{
   return AEE_EUNSUPPORTED;
}
//=============================================================================
// 
//
//=============================================================================
void AEEBTA2DP_OEMEventCb( void            *cbData,
                           OEMBTA2DPEvent    *pEvt)
{
   IBTA2DP *me = (IBTA2DP*)cbData;

   // if this callback is being called, 
   // we are still registered with the oem layer
   AEEBTEvtQ_Put(&me->sysEvtQueue, (uint8*)pEvt);
   CALLBACK_Cancel(&me->evSignalCb);
   CALLBACK_Init(&me->evSignalCb, AEEBTA2DP_SysEvtCallback, me);
   AEE_SYS_RESUME(&me->evSignalCb);
}
//=============================================================================
// 
//
//=============================================================================
void AEEBTA2DP_SysEvtCallback(void* pme)
{
   IBTA2DP *me = (IBTA2DP*)pme;
   OEMBTA2DPEvent  evt;
   int ret = AEE_EFAILED;
   BTA2DPEvent notifEvt;
   boolean bNotify = TRUE;

   MSG_MED("AEE a2dp evt callback", 0,0,0);
   // pop the first event on the queue.
   ret = AEEBTEvtQ_Get(&me->sysEvtQueue, (uint8*)&evt);

   while (AEE_SUCCESS == ret)
   {
      IBTA2DPNotifier *pNotif;
      bNotify = TRUE;
      MEMSET((void*)&notifEvt, 0, sizeof(BTA2DPEvent));

      MSG_MED("AEE a2dp evt %x recvd",evt.event,0,0);

      switch (evt.event)
      {
         case AEEBTA2DP_EVT_ENABLED:
         {
            MSG_MED("AEE a2dp evt enabled state %x res %x",
                    me->state,evt.u.evtEnbl.result,0);

            if (AEE_A2DP_STATE_ENABLE_PEND == me->state)
            {
               OEMBTA2DP_StateEnabled(me->pOemObj);
               notifEvt.event = evt.event;
               notifEvt.bLocalInit = TRUE;
               notifEvt.result = AEE_SUCCESS;
               me->state = AEE_A2DP_STATE_ENABLED;
               AEEBTA2DP_UpdateSecurity(me, me->btSec);               
            }
            break;
         }
         case AEEBTA2DP_EVT_ENABLE_FAILED:
         {
            MSG_MED("AEE a2dp evt enable failed state %x res %x",
                     me->state,evt.u.evtEnbl.result,0);

            if (AEE_A2DP_STATE_ENABLE_PEND == me->state)
            {
               notifEvt.event = evt.event;
               notifEvt.bLocalInit = TRUE;
               notifEvt.result = evt.u.evtEnbl.result;
               me->state = AEE_A2DP_STATE_INIT;
            }
            break;
         }
         case AEEBTA2DP_EVT_DISABLED:
         {
            MSG_MED("AEE a2dp evt disable state %x , res %x ",
                     me->state,evt.u.evtEnbl.result,0);

            if (AEE_A2DP_STATE_DISABLE_PEND == me->state)
            {
               int i = 0;
               notifEvt.event = evt.event;
               notifEvt.bLocalInit = TRUE;
               notifEvt.result = evt.u.evtEnbl.result;
               AEEBTA2DP_UpdateSecurity (me, AEEBT_SEC_NONE);
               me->state = AEE_A2DP_STATE_INIT;
               for ( i = 0; i < AEE_A2DP_MAX_CONNECTIONS; i++)
               {
                  AEEBTA2DP_ConInit(&me->aConn[i]);
               }
            }
            break;
         }
         case AEEBTA2DP_EVT_CONNECTED:
         {
            AEEBTA2DPConn *pCon = NULL;
            notifEvt.event = evt.event;
            notifEvt.result = AEE_SUCCESS;
            notifEvt.remoteAddr = evt.u.evtConn.bdAddr;
            MSG_MED("AEE a2dp evt connected sate %x ",
                     me->state,0,0);
            OEMBTA2DP_StateConnected(me->pOemObj);
            pCon = AEEBTA2DP_FindConnection(me,
                                            &evt.u.evtConn.bdAddr,
                                            TRUE);
            if (pCon != NULL)
            {
               MSG_MED("AEE a2dp evt connected got conn obj state %x, con state %x",
                        me->state,pCon->state,0);
               // either remote or locally iniated conn
               if (AEE_A2DP_STATE_CONN_PEND == pCon->state)
               {
                  notifEvt.bLocalInit = TRUE;
                  me->state = AEE_A2DP_STATE_ENABLED;
               }
               pCon->state = AEE_A2DP_STATE_CONNECTED;
               me->numCon++;
            }
            else
            {
               OEMBTA2DP_Disconnect(me->pOemObj,&evt.u.evtConn.bdAddr);
            }
            break;
         }
         case AEEBTA2DP_EVT_CON_FAILED:
         {
            AEEBTA2DPConn *pCon = NULL;
            OEMBTA2DP_StateConnectFailed(me->pOemObj);
            notifEvt.event = evt.event;
            notifEvt.result = evt.u.evtConFail.result;
            notifEvt.remoteAddr = evt.u.evtConFail.bdAddr;

            pCon = AEEBTA2DP_FindConnection(me,
                                            &evt.u.evtConn.bdAddr,
                                            FALSE);
            if (pCon != NULL)
            {
               if (AEE_A2DP_STATE_CONN_PEND == me->state)
               {
                   me->state = AEE_A2DP_STATE_ENABLED;
               }
               notifEvt.bLocalInit = TRUE;
               AEEBTA2DP_ConInit(pCon);
            }
            break;
         }
         case AEEBTA2DP_EVT_DISCONNECTED:
         {
            AEEBTA2DPConn *pCon = NULL;
            notifEvt.event = evt.event;
            notifEvt.result = evt.u.evtDisc.result;
            notifEvt.remoteAddr = evt.u.evtDisc.bdAddr;
            OEMBTA2DP_StateDisconnected(me->pOemObj);
            pCon = AEEBTA2DP_FindConnection(me,
                                            &evt.u.evtDisc.bdAddr,
                                            FALSE);
            if (pCon != NULL)
            {
               notifEvt.bLocalInit = (AEE_A2DP_STATE_DISC_PEND == pCon->state) ?
                                      TRUE : FALSE;
               AEEBTA2DP_ConInit(pCon);
               me->numCon--;
               if ((AEE_A2DP_STATE_DISC_PEND == me->state) ||
                   (AEE_A2DP_STATE_CONN_PEND == me->state))
               {
                  me->state = AEE_A2DP_STATE_ENABLED;
               }
            }
            break;
         }
         case AEEBTA2DP_EVT_START:
         {
            notifEvt.event = evt.event;
            notifEvt.result = AEE_SUCCESS;
            notifEvt.bLocalInit = evt.u.evtStart.bLocalInit;
            notifEvt.remoteAddr = evt.u.evtStart.bdAddr;
            break;
         }
         case AEEBTA2DP_EVT_SUSPEND:
         {
            notifEvt.event = evt.event;
            notifEvt.result = AEE_SUCCESS;
            notifEvt.bLocalInit = evt.u.evtSuspend.bLocalInit;
            notifEvt.remoteAddr = evt.u.evtSuspend.bdAddr;
            break;
         }
         case AEEBTA2DP_EVT_OPEN:
         {
            notifEvt.event = evt.event;
            notifEvt.result = AEE_SUCCESS;
            notifEvt.bLocalInit = evt.u.evtOpen.bLocalInit;
            notifEvt.remoteAddr = evt.u.evtOpen.bdAddr;
            break;
         }
         case AEEBTA2DP_EVT_CLOSE:
         {
            notifEvt.event = evt.event;
            notifEvt.result = AEE_SUCCESS;
            notifEvt.bLocalInit = evt.u.evtClose.bLocalInit;
            notifEvt.remoteAddr = evt.u.evtClose.bdAddr;
            break;
         }
         default :
            bNotify = FALSE;
            break;
      }

      if (bNotify == TRUE)
      {
         for (pNotif = gAEEBTA2DPNotifMgr.pHeadObject;
            pNotif != NULL;
            pNotif = pNotif->pNextObject)
         {
            // queue the event 
            AEEBTEvtQ_Put(&pNotif->appEvtQueue, (uint8*)&notifEvt);
            if (pNotif->piSigBusEvt)
            {
               ISignalBus_Strobe(pNotif->piSigBusEvt);
            }
         }
      }
      ret = AEEBTEvtQ_Get(&me->sysEvtQueue, (uint8*)&evt);
   }
}
//=============================================================================
// 
//
//=============================================================================
int AEEBTA2DPNotifier_New(IShell* pIShell, AEECLSID cls, void **ppif)
{
   IBTA2DPNotifier *me = NULL;
   int ret      = AEE_SUCCESS;

   MSG_DEBUG("AEE a2dp notif new ->",0,0,0);

   if (NULL == ppif)
   {
      MSG_ERROR("AEE a2dp notif new ppif = NULL ",0,0,0);
      return AEE_EBADPARM;
   }
   // allocate AEE object
   me = MALLOCREC(IBTA2DPNotifier);

   if (NULL == me)
   {
      MSG_ERROR("AEE a2dp notif cannot allocate me",0,0,0);
      return AEE_ENOMEMORY;
   }

   MEMSET((void*)me, 0, sizeof(IBTA2DPNotifier));

   ret = AEEBTEvtQ_Init(&me->appEvtQueue,
                        AEEBTA2DP_MAX_EVENTS,
                        sizeof(BTA2DPEvent));

   if (AEE_SUCCESS != ret)
   {
      MSG_ERROR("AEE bt a2dp cannot alloc evt Q ret %x", ret,0,0);
      FREEIF(me);
      return AEE_EFAILED;
   }

   // initialize the object
   me->pvt             = &g_Vtbl_BTA2DPNotifier;
   me->uRef            = 1;
   me->pIShell         = pIShell;

   // Initialize the cb that will be called in case the app closes
   CALLBACK_Init(&me->cbAppClose, AEEBTA2DPNotif_Delete, me);
   AEE_LinkSysObject(&me->cbAppClose);

   // add the object to the queue
   ret = ISHELL_CreateInstance(me->pIShell, AEECLSID_SignalBus,
                                 (void**)&me->piSigBusEvt);
   if (AEE_SUCCESS != ret)
   {
      MSG_ERROR("AEE bt a2dp cannot create bus %x", ret,0,0);
      AEEBTEvtQ_Free(&me->appEvtQueue);
      FREEIF(me);
      return AEE_EFAILED;
   }
   me->pNextObject  = gAEEBTA2DPNotifMgr.pHeadObject;
   gAEEBTA2DPNotifMgr.pHeadObject = me;

   ISHELL_AddRef(pIShell);
   // return the interface
   *ppif = (void *)me;

   MSG_DEBUG("AEE a2dp notif new <-",0,0,0);

   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEBTA2DPNotif_AddRef(IBTA2DPNotifier *me)
{
   MSG_DEBUG("AEE a2dp notif add ref %x",me->uRef,0,0);
   return ++me->uRef;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEBTA2DPNotif_Release(IBTA2DPNotifier *me)
{
   uint32 uRef = --me->uRef;

   MSG_DEBUG("AEE a2dp notif release -> ",0,0,0);

   if (0 == uRef)
   {
      MSG_HIGH("AEE a2dp notif uRef 0.. calling delete ",0,0,0);
      AEEBTA2DPNotif_Delete(me);
   }
   MSG_DEBUG("AEE a2dp notif uRef <-",uRef,0,0);
   return uRef;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEBTA2DPNotif_QueryInterface(IBTA2DPNotifier *me,
                                         AEECLSID iid, void **ppo)
{
   switch (iid)
   {
      case AEEIID_IQI:
      case AEEIID_IBTA2DPNotifier:
         *ppo = me;
         break;

      default:
         *ppo = 0;
         return AEE_ECLASSNOTSUPPORT;
   }
   IQI_AddRef(*ppo);
   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEBTA2DPNotif_Delete(IBTA2DPNotifier *me)
{
   IBTA2DPNotifier  **ppc = NULL;

   MSG_DEBUG("AEE bt a2dp delete -> ",0,0,0);

   // free the event queue
   AEEBTEvtQ_Free(&me->appEvtQueue);
   CALLBACK_Cancel(&me->cbAppClose);

   for (ppc = &gAEEBTA2DPNotifMgr.pHeadObject; *ppc != NULL; ppc = &(*ppc)->pNextObject)
   {
      if (*ppc == me)
      {
         *ppc = me->pNextObject;
         me->pNextObject = NULL; 
         break;
      }
   }
   if (me->piSigBusEvt)
   {
      ISignalBus_Release(me->piSigBusEvt);
   }

   RELEASEIF(me->pIShell);
   FREEIF(me);
   MSG_DEBUG("AEE a2dp notif delete <- ",0,0,0);
   return AEE_SUCCESS;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEBTA2DPNotif_OnEventNotify(IBTA2DPNotifier *me,
                                        ISignal         *piSignal)
{
   int ret = AEE_SUCCESS;

   MSG_DEBUG("AEE a2dp notif OnEventNotify -> ",0,0,0);

   if (NULL == piSignal)
   {
      MSG_ERROR("AEE a2dp notif on evt notify, bad sig %x ",piSignal,0,0);
      return AEE_EBADPARM;
   }
   ret = ISignalBus_Add(me->piSigBusEvt, piSignal);
   MSG_DEBUG("AEE a2dp notif OnEventNotify ret %x <- ",ret,0,0);

   return ret;
}

//=============================================================================
// 
//
//=============================================================================

static int AEEBTA2DPNotif_GetEvent(IBTA2DPNotifier *me,
                                   BTA2DPEvent     *pEvent)
{
   int ret = AEE_ENOMORE;

   if ((NULL == pEvent) ||
       (!AEE_CHECK_PTR("AEEBTA2DPNotif_GetEvent",
                        pEvent, sizeof(BTA2DPEvent), TRUE)))
   {
      MSG_ERROR("AEE a2dp notif GetEvent bad param pEvent %x"
               ,pEvent,0,0);
      return AEE_EBADPARM;
   }
   MSG_MED("AEE a2dp notif get event -> ",0,0,0);

   MEMSET((void*)pEvent, 0, sizeof(BTA2DPEvent));
   // pop the first event on the queue.
   ret = AEEBTEvtQ_Get(&me->appEvtQueue, (uint8*)pEvent);

   MSG_MED("AEE a2dp notif get event ret %x <-",ret,0,0);
   return ret;
}


//=============================================================================
// 
//
//=============================================================================
static AEEBTA2DPConn* AEEBTA2DP_FindConnection(IBTA2DP *me,
                                               const BDAddress *pBDAddr,
                                               boolean bCreate)
{
   AEEBTA2DPConn *pConn = NULL;
   int i;

   for (i = 0; i < AEE_A2DP_MAX_CONNECTIONS ; i++)
   {
      // connection entry exists?
      if ((AEEBT_BD_ADDRS_EQUAL (&me->aConn[i].bdAddr, 
                                 pBDAddr) != FALSE) &&
                                 (AEE_A2DP_STATE_INIT != me->aConn[i].state))
      {
         pConn = &me->aConn[i];
         break;
      }
   }

   if ((NULL == pConn) && (TRUE == bCreate))
   {
      // get a new connection entry
      for (i = 0; i < AEE_A2DP_MAX_CONNECTIONS; i++)
      {
         if (TRUE == me->aConn[i].bFree)
         {
            me->aConn[i].bFree = FALSE;
            pConn = &me->aConn[i];
            pConn->bdAddr = *pBDAddr;
            break;
         }
      }
   }
   return pConn;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEBTA2DP_UpdateSecurity (IBTA2DP *me, BTSecurityLevel dSec)
{
   int result = AEE_EFAILED;
   IShell* pIShell = AEE_GetShell();

   if ((NULL == me->piBT) && 
          ((result = ISHELL_CreateInstance (pIShell, AEECLSID_BT, 
                               (void**)&me->piBT)) != AEE_SUCCESS))
   {
      MSG_ERROR("AEE a2dp cannot create ibt ret %x", result,0,0);
   }
   else if ((NULL == me->piMgr) &&
            ((result = IBT_GetObject (me->piBT, AEECLSID_BTMgr, NULL, 0,
                            (void**)&me->piMgr)) != AEE_SUCCESS))
   {
      MSG_ERROR ("AEE a2dp get ibt mgr failed ret %x", result, 0,0);
   }
   else
   {
         BTServiceIdentifier  svc2protect = {
         AEEBT_SEC_BY_SERVICE_CLASS,
         2,               // number of protocols /services
         {0, 0, 0, 0, 0}, //protocols
         {AEEBT_SCU_AUDIO_SOURCE,AEEBT_SCU_ADVANCED_AUDIO_DISTRIBUTION},
         {0,0,0,0,0}, //scns
         {0,0,0,0,0}}; //psm

      if ((result = IBTMgr_SetSvcSecurity( me->piMgr, dSec, 
                                           &svc2protect)) != AEE_SUCCESS)
      {
         MSG_ERROR("AEE a2dp set security failed r=%x", result,0,0);
      }
   }

   return result;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV) */


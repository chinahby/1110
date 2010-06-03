/*=============================================================================
FILE:  AEERemoteControl_Target.c

SERVICES:  Implementation of IRemoteControl_Target

GENERAL DESCRIPTION:
   Implements the IRemoteControl_Target interface, tied to Bluetooth AVRCP and
   the IBTAVRCPTarget implementation.

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEERemoteControl_Target.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
=============================================================================*/

#include "OEMFeatures.h"
#include "AEEStdLib.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV)
#include "AEEStdErr.h"
#include "AEE_OEM.h"
#include "AEE_OEMHeap.h"
#include "AEE_OEMDispatch.h"

#include "AEEIRemoteControl_Target.h"
#include "AEEBTAVRCPTarget.bid"

#include "OEMBTAVRCPTarget.h"

#include "AEECriticalSection.h"
#include "AEEISignal.h"
#include "AEEISignalCtl.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalCBFactory.h"

#include "AEE_OEMComDef.h"
#include "AEEBTUtils.h"
/*======================================================================= 
   Defines
=======================================================================*/

// Maximum number of outstanding passthrough commands for an app 
#define RC_MAX_CMD 16

//=============================================================================
//   RC Object
//=============================================================================
struct IRemoteControl_Target
{
   const IRemoteControl_TargetVtbl *pvt;
   uint32                  uRef;
   IShell                  *pIShell;
   AEECallback             cbAppClose;
   uint64                  commandMask;

   ISignal                 *piSig;
   ISignal                 *piSigVendor;

   AEEBTEvtQueue           cmdQueue;
   AEEBTEvtQueue           cmdQueueVendor;
   IRemoteControl_Target   *pNextObject; 
};
//=============================================================================
//    Forward Function Declarations
//=============================================================================
static uint32 AEERemoteControl_Target_AddRef(IRemoteControl_Target* pMe);
static uint32 AEERemoteControl_Target_Release(IRemoteControl_Target* pMe);
static int AEERemoteControl_Target_QueryInterface(IRemoteControl_Target* pMe,
                                                  AEEIID cls, void** ppo);
static AEEResult 
AEERemoteControl_Target_RegCommandMask(IRemoteControl_Target* pMe,
                                       const uint64struct *CommandMask);

static AEEResult
AEERemoteControl_Target_OnCommandNotify(IRemoteControl_Target* pMe,
                                        ISignal* Sig);

static AEEResult
AEERemoteControl_Target_GetCommand(IRemoteControl_Target* pMe,
                                   AEERemoteControl_CommandType* Command);

static AEEResult AEERemoteControl_Target_SetFocus(IRemoteControl_Target* pMe);

static AEEResult
AEERemoteControl_Target_GetSupportedTransports(IRemoteControl_Target* pMe,
                                               AEERemoteControl_Transport* TransportIDList,
                                               int TransportIDListLen,
                                               int* TransportIDListLenReq);

static AEEResult
AEERemoteControl_Target_RegVendorID(IRemoteControl_Target* pMe,
                                    const char* VendorID,
                                    AEERemoteControl_Transport TransportID);

static AEEResult
AEERemoteControl_Target_OnVendorCommandNotify(IRemoteControl_Target* pMe,
                                              ISignal* Sig);

static AEEResult
AEERemoteControl_Target_GetVendorCommand(IRemoteControl_Target* pMe,
                                         AEERemoteControl_VendorCommandType* Command);

static AEEResult
AEERemoteControl_Target_ReplyVendorCommand(IRemoteControl_Target* pMe,
                                           const AEERemoteControl_VendorCommandType* Command);

static AEEResult
AEERemoteControl_Target_RegExtension(IRemoteControl_Target* pMe,
                                     IQueryInterface* Extension);

//=============================================================================
//    V-Table
//=============================================================================
static const AEEVTBL(IRemoteControl_Target) gpvtIRC_Target = 
{
  AEERemoteControl_Target_AddRef,
  AEERemoteControl_Target_Release,
  AEERemoteControl_Target_QueryInterface,
  AEERemoteControl_Target_RegCommandMask,  
  AEERemoteControl_Target_OnCommandNotify,
  AEERemoteControl_Target_GetCommand,
  AEERemoteControl_Target_SetFocus,
  AEERemoteControl_Target_GetSupportedTransports,
  AEERemoteControl_Target_RegVendorID,
  AEERemoteControl_Target_OnVendorCommandNotify,
  AEERemoteControl_Target_GetVendorCommand,
  AEERemoteControl_Target_ReplyVendorCommand,
  AEERemoteControl_Target_RegExtension
};
//=============================================================================
// AEE Remote control Manager
//=============================================================================
typedef struct
{
  IRemoteControl_Target  *pHeadObject;    // singly linked list of objects
  OEMAVRCPTarget         *pOemObj;
  IRemoteControl_Target  *pCurCtl;
  AEECallback            evSignalCb;    
} AEERemoteControlMgr;

//=============================================================================
// Global Variables
//=============================================================================
static AEERemoteControlMgr    gAEERCMgr;
static AEECriticalSection gAEERCCriticalSection;

// Remote Control operations required by each of the 4 Bluetooth AVRCP
// categories. If all commands in a category are supported, that category
// shall be listed as supported by AVRCP.
static const uint64 AEERemoteControlCat1 =
  AEERemoteControl_OP_PLAY |
  AEERemoteControl_OP_STOP;

static const uint64 AEERemoteControlCat2 =
  AEERemoteControl_OP_VOLUME_UP |
  AEERemoteControl_OP_VOLUME_DOWN;

static const uint64 AEERemoteControlCat3 =
  AEERemoteControl_OP_CHANNEL_UP |
  AEERemoteControl_OP_CHANNEL_DOWN;

static const uint64 AEERemoteControlCat4 =
  AEERemoteControl_OP_ROOT_MENU |
  AEERemoteControl_OP_UP        |
  AEERemoteControl_OP_DOWN      |
  AEERemoteControl_OP_LEFT      |
  AEERemoteControl_OP_RIGHT     |
  AEERemoteControl_OP_SELECT;

#define AEERC_LOCK()    AEECriticalSection_Enter(&gAEERCCriticalSection)
#define AEERC_UNLOCK()  AEECriticalSection_Leave(&gAEERCCriticalSection)
//=============================================================================
//                         PRIVATE FUNCTIONS
//
//=============================================================================

static void AEERC_EvSignalCallback(void *pData)
{
   AEERC_LOCK();

   if ((gAEERCMgr.pCurCtl != NULL) && (gAEERCMgr.pCurCtl->piSig != NULL)) 
   {
      ISignal_Set(gAEERCMgr.pCurCtl->piSig);
   }
   AEERC_UNLOCK();
   return;
}
OEMBTAVRCPRspType AEERC_OemEvtCallback(void *pUser,
                                       AEERemoteControl_CommandType *pCmd )
{
   OEMBTAVRCPRspType resp = OEMBT_AVRCP_RESPONSE_NOT_IMPLEMENTED;
   uint64 recvCmd = uint64struct_to_uint64(pCmd->Type);
      
   //MSG_DEBUG("AEE RC evt cb ",0,0,0);

   AEERC_LOCK();

   if ((gAEERCMgr.pCurCtl != NULL) &&
       (gAEERCMgr.pCurCtl->commandMask & recvCmd))
   {
      if (gAEERCMgr.pCurCtl->piSig) 
      {
         AEEBTEvtQ_Put(&gAEERCMgr.pCurCtl->cmdQueue, (uint8*)pCmd);
         resp = OEMBT_AVRCP_RESPONSE_ACCEPTED;
         CALLBACK_Cancel(&gAEERCMgr.evSignalCb);
         CALLBACK_Init(&gAEERCMgr.evSignalCb, AEERC_EvSignalCallback, &gAEERCMgr);
         AEE_SYS_RESUME(&gAEERCMgr.evSignalCb);
      }
   }
   AEERC_UNLOCK();
   return resp;
}
//=============================================================================
//
//
//=============================================================================
int AEERC_Cleanup(IRemoteControl_Target *me)
{
   IRemoteControl_Target **ppc;

   AEERC_LOCK();

   // free the event queue
   if (me->piSig)
   {
      ISignal_Release(me->piSig);
      me->piSig = NULL;
   }
   AEEBTEvtQ_Free(&me->cmdQueue);
   CALLBACK_Cancel(&me->cbAppClose);

   for (ppc = &gAEERCMgr.pHeadObject; *ppc != NULL; ppc = &(*ppc)->pNextObject)
   {
      if (*ppc == me)
      {
         *ppc = me->pNextObject;
         me->pNextObject = NULL; 
         break;
      }
   }
   if (gAEERCMgr.pCurCtl == me)
   {
      gAEERCMgr.pCurCtl = NULL;
   }
   // if this was the last object in the list, 
   if (NULL == gAEERCMgr.pHeadObject)
   {
      CALLBACK_Cancel(&gAEERCMgr.evSignalCb);
      OEMBTAVRCP_CmdCbDeregister(gAEERCMgr.pOemObj);
      OEMBTAVRCP_Release(gAEERCMgr.pOemObj);
      gAEERCMgr.pOemObj = NULL;
   }
   AEERC_UNLOCK();
   RELEASEIF(me->pIShell);
   FREEIF(me);
   MSG_DEBUG("AEE RC delete <- ",0,0,0);
   return AEE_SUCCESS;
}


//=============================================================================
//                           VTABLE FUNCTIONS
//
//=============================================================================
static uint32 AEERemoteControl_Target_AddRef(IRemoteControl_Target *me)
{
   // increment ref count
   if (NULL != me)
   {
      return ++me->uRef;
   }
   return 0;
}
//=============================================================================
//
//
//=============================================================================
static uint32 AEERemoteControl_Target_Release(IRemoteControl_Target *me)
{
   uint32 uRefs;

   if (0 == me->uRef)
   {
      return me->uRef;
   }

   MSG_MED("AEE RC rel - ref cnt = %x", me->uRef,0,0);

   uRefs = --me->uRef;

   if (0 == uRefs)
   {
      AEERC_Cleanup(me);
   }
   return uRefs;
}
//=============================================================================
//
//
//=============================================================================
static int AEERemoteControl_Target_QueryInterface(IRemoteControl_Target *me,
                                                  AEEIID iid,
                                                  void** ppo)
{
   if (!AEE_CHECK_PTR("AEE RC Query Interface", ppo, sizeof(void*), TRUE))
   {
      return AEE_EMEMPTR;
   }

   if ((AEEIID_IQI == iid) ||
      (AEEIID_IRemoteControl_Target == iid)) 
   {
      *ppo = (IQueryInterface*)me;
      IQI_AddRef (*ppo);
      return AEE_SUCCESS;
   }
   return AEE_ECLASSNOTSUPPORT;
}
//=============================================================================
//
//
//=============================================================================
static AEEResult 
AEERemoteControl_Target_RegCommandMask(IRemoteControl_Target *me,
                                       const uint64struct *CommandMask)
{
   IRemoteControl_Target *pRC = gAEERCMgr.pHeadObject;
   int ret = AEE_SUCCESS;
   uint64 allMasks = 0;
   OEMBTAVRCPCatType cats = 0;

   if (CommandMask)
   {
      me->commandMask = uint64struct_to_uint64(*CommandMask);
      MSG_MED("AEE RC reg cmd msk %x", 0,0,0);
      AEERC_LOCK();
      while( pRC )
      {
        allMasks |= pRC->commandMask;
        pRC = pRC->pNextObject;
      }
      AEERC_UNLOCK();
      if( (allMasks & AEERemoteControlCat1) == AEERemoteControlCat1 )
      {
        cats |= OEMBT_AVRCP_CATEGORY1;
      }
      if( (allMasks & AEERemoteControlCat2) == AEERemoteControlCat2 )
      {
        cats |= OEMBT_AVRCP_CATEGORY2;
      }
      if( (allMasks & AEERemoteControlCat3) == AEERemoteControlCat3 )
      {
        cats |= OEMBT_AVRCP_CATEGORY3;
      }
      if( (allMasks & AEERemoteControlCat4) == AEERemoteControlCat4 )
      {
        cats |= OEMBT_AVRCP_CATEGORY4;
      }

      if(OEMBTAVRCP_SetCategory(gAEERCMgr.pOemObj, cats)!= AEE_SUCCESS)
      {
         MSG_ERROR("AEE RC: Set category not successful ",0,0,0);
      }
   }
   else
   {
      ret = AEE_EBADPARM;
   }
   return ret;
}
//=============================================================================
//
//
//=============================================================================

static AEEResult
AEERemoteControl_Target_OnCommandNotify(IRemoteControl_Target *me,
                                        ISignal *piSig)
{
   MSG_DEBUG("AEE RC on cmd notif -> ",0,0,0);

   if (NULL == piSig)
   {
      MSG_ERROR("AEE RC on cmd notify, bad sig %x ",piSig,0,0);
      return AEE_EBADPARM;
   }

   me->piSig = piSig;
   ISignal_AddRef(piSig);

   MSG_DEBUG("AEE RC on cmd notif <- ",0,0,0);

   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================

static int
AEERemoteControl_Target_GetCommand(IRemoteControl_Target *me,
                                   AEERemoteControl_CommandType *pCmd)
{
   int ret = AEE_ENOMORE;

   MSG_MED("AEE RC get event -> ",0,0,0);

   if ((NULL == pCmd) ||
       (!AEE_CHECK_PTR("AEERemoteControl_Target_GetCommand",
                        pCmd, sizeof(AEERemoteControl_CommandType), TRUE)))
   {
      MSG_ERROR("AEE RC get cmd bad param pCmd %x",pCmd,0,0);
      return AEE_EBADPARM;
   }

   MEMSET((void*)pCmd, 0, sizeof(AEERemoteControl_CommandType));

   AEERC_LOCK();
   // pop the first event on the queue.
   ret = AEEBTEvtQ_Get(&me->cmdQueue, (uint8*)pCmd);

   AEERC_UNLOCK();
   MSG_MED("AEE RC get event ret %x <-",ret,0,0);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================

static AEEResult
AEERemoteControl_Target_GetSupportedTransports(IRemoteControl_Target* me,
                                               AEERemoteControl_Transport* TransportIDList,
                                               int TransportIDListLen,
                                               int* pTransportIDListLenReq)
{
   int ret = AEE_SUCCESS;

   if (NULL != pTransportIDListLenReq)
   {
      *pTransportIDListLenReq = 1;
   }
   if (TransportIDListLen <= 0)
   {
      ret = AEE_EBUFFERTOOSMALL;
   }
   else 
   {
      if ((NULL == TransportIDList) &&
          ((!AEE_CHECK_PTR("AEERemoteControl_Target_GetSupportedTransports",
                          TransportIDList, sizeof(AEERemoteControl_Transport) * TransportIDListLen, TRUE))))
      {
         ret = AEE_EBADPARM;
      }
      else
      {
         TransportIDList[0] = AEERemoteControl_TRANS_BT_AVRCP_1_0;
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static AEEResult AEERemoteControl_Target_SetFocus(IRemoteControl_Target* me)
{
   IRemoteControl_Target **ppc = NULL;
   int ret = AEE_EFAILED;

   AEERC_LOCK();
   
   for (ppc = &gAEERCMgr.pHeadObject; *ppc != NULL; ppc = &(*ppc)->pNextObject)
   {
      if (*ppc == me)
      {
         MSG_MED("AEE RC Set focus tgt %x <-",me,0,0);
         gAEERCMgr.pCurCtl = me;
         ret = AEE_SUCCESS;
         break;
      }
   }
   AEERC_UNLOCK();

   return ret;
  
}
//=============================================================================
// 
//
//=============================================================================

static AEEResult
AEERemoteControl_Target_RegVendorID(IRemoteControl_Target* pMe,
                                    const char* VendorID,
                                    AEERemoteControl_Transport TransportID)
{
   return AEE_EUNSUPPORTED;
}
//=============================================================================
// 
//
//=============================================================================

static AEEResult
AEERemoteControl_Target_OnVendorCommandNotify(IRemoteControl_Target* me,
                                              ISignal* piSig)
{
   return AEE_EUNSUPPORTED;
}
//=============================================================================
// 
//
//=============================================================================

static AEEResult
AEERemoteControl_Target_GetVendorCommand(IRemoteControl_Target* pMe,
                                         AEERemoteControl_VendorCommandType* Command)
{
   return AEE_EUNSUPPORTED;
}
//=============================================================================
// 
//
//=============================================================================

static AEEResult
AEERemoteControl_Target_ReplyVendorCommand(IRemoteControl_Target* pMe,
                                           const AEERemoteControl_VendorCommandType* Command)
{
   return AEE_EUNSUPPORTED;
}
//=============================================================================
// 
//
//=============================================================================

static AEEResult
AEERemoteControl_Target_RegExtension(IRemoteControl_Target* pMe,
                                     IQueryInterface* Extension)
{
  return AEE_EUNSUPPORTED;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV) */

//=============================================================================
//                            PUBLIC FUNCTIONS
//
//=============================================================================
int AEERemoteControl_New(IShell* piShell, AEECLSID cls, void** ppif)
{
#if !defined(FEATURE_IBT_DYNAMIC) || !defined(FEATURE_BT_EXTPF_AV)
  return ECLASSNOTSUPPORT;
#else
   IRemoteControl_Target *me = NULL;
   int ret      = AEE_SUCCESS;

   MSG_DEBUG("AEE RC new ->",0,0,0);

   if (NULL == ppif)
   {
      MSG_ERROR("AEE RC new ppif = NULL ",0,0,0);
      return AEE_EBADPARM;
   }
   // allocate AEE object
   me = MALLOCREC(IRemoteControl_Target);

   if (NULL == me)
   {
      MSG_ERROR("AEE RC cannot allocate me",0,0,0);
      return AEE_ENOMEMORY;
   }

   MEMSET((void*)me, 0, sizeof(IRemoteControl_Target));

   ret = AEEBTEvtQ_Init(&me->cmdQueue,
                        RC_MAX_CMD,
                        sizeof(AEERemoteControl_CommandType));

   if (AEE_SUCCESS == ret)
   {
      if (NULL == gAEERCMgr.pHeadObject)
      {
         // this is the first object , so initialize the common object
         // register with OEM
         if (AEE_SUCCESS == (ret = OEMBTAVRCP_New(piShell, &gAEERCMgr.pOemObj)))
         {
            ret = OEMBTAVRCP_CmdCbRegister(gAEERCMgr.pOemObj, AEERC_OemEvtCallback, &gAEERCMgr); 
         }
      }
      // initialize the object
      me->pvt             = &gpvtIRC_Target;
      me->uRef            = 1;
      me->pIShell         = piShell;
      // Initialize the cb that will be called in case the app closes
      CALLBACK_Init(&me->cbAppClose, AEERC_Cleanup, me);
      AEE_LinkSysObject(&me->cbAppClose);
      ISHELL_AddRef(piShell);

      // link into global mgr
      AEERC_LOCK();
      me->pNextObject = gAEERCMgr.pHeadObject;
      gAEERCMgr.pHeadObject = me;
      AEERC_UNLOCK();
      *ppif = (void *)me;
   }

   // Clean up if init failed
   if (AEE_SUCCESS != ret)
   {
      AEERemoteControl_Target_Release(me);
      *ppif = NULL;
   }

   MSG_DEBUG("AEE RC new ret %x <-",ret,0,0);

   return ret;
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV)*/
}


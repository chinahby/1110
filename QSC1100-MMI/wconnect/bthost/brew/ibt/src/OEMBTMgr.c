/*=============================================================================
FILE:         OEMBTMgr.c

SERVICES:     OEMBTMgr implementation

DESCRIPTION:  This file handles the OEM implementation for IBTMgr

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBTMgr.c#2 $ 
$DateTime: 2009/03/13 17:22:06 $
$Author: c_novinm $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2009-03-13   NM  Adding interface to get radio state
  2008-10-02   rs  Removing compiler and Lint warnings.
  2008-09-19   pn  Klocwork fixes; Revised edit history format.
  2008-09-15   sp  Compiler warning fix

=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#include "bt.h"
#include "btsd.h"
#include "btsio.h"
#include "OEMHeap.h"
#include "OEMBTUtils.h"
#include "OEMBTMgr.h"
#include "OEMCriticalSection.h"
#include "AEEIBTConnection.h"
#ifndef BT_SIMULATION
#include "AEEStd.h"
#endif
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEEISignal.h"
#include "AEEISignalBus.h"
#include "AEESignalBus.bid"
#include "btmsg.h"

#ifdef BT_SIMULATION
#include "bt_sim_interface.h"
#endif
#include "AEE_OEMDispatch.h"

typedef struct
{
   boolean              bValid;
   BDAddress            remoteAddr;
   BTServiceIdentifier  serviceId;
}  BTAuthorizeRequest;

typedef struct _BTMgrConnEvCb BTMgrConnEvCb;
struct _BTMgrConnEvCb
{
   BTMgrConnEvCb*       pNext;
   ConnEvCallbackFn*    pEvCbFn;
   IBTConnection*       piConn;
};

typedef struct _BTMgrEvCb BTMgrEvCb;
struct _BTMgrEvCb
{
   BTMgrEvCb*           pNext;
   MgrEvCallbackFn*     pEvCbFn;
   IBTMgr*              piMgr;
};

struct _OEMBTMgr
{
   IShell*              piShell;
   uint32               uRefs;
   BTLocalSettings      myInfo;

   // driver interface
   bt_app_id_type       appId;
   bt_app_id_type       connStatAppId;
   uint32               uFlags;
   BTMgrEvCb*           pMgrEvCbFnList;
   BTAuthorizeRequest   pendingRequest[OEMBTMGR_MAX_CONNECTIONS];
   ISignalBus*          piConnReqSignalBus;
   AEECallback          BREWEvCb;    
   BTMgrConnEvCb*       pConnEvCbFnList;
};

static OEMBTMgr* pgTheOEMBTMgr = NULL;
static OEMCriticalSection gOEMBTMgrCriticalSection;

#define OEMBTMGR_LOCK()    OEMCriticalSection_Enter(&gOEMBTMgrCriticalSection)
#define OEMBTMGR_UNLOCK()  OEMCriticalSection_Leave(&gOEMBTMgrCriticalSection)

#define OEMBTMGR_FLAG_OP_BY_ME                  (1 << 0)
#define OEMBTMGR_FLAG_SET_NAME                  (1 << 1)
#define OEMBTMGR_FLAG_SET_COD                   (1 << 2)
#define OEMBTMGR_FLAG_SET_SVC_SEC               (1 << 3)
#define OEMBTMGR_FLAG_SET_SEC                   (1 << 4)
#define OEMBTMGR_FLAG_SET_CONN_SEC              (1 << 5)
#define OEMBTMGR_FLAG_SET_ROLE                  (1 << 6)
#define OEMBTMGR_FLAG_SET_CONN_ROLE             (1 << 7)
#define OEMBTMGR_FLAG_SET_ROLE_SW_POLICY        (1 << 8)
#define OEMBTMGR_FLAG_SET_CONN_ROLE_SW_POLICY   (1 << 9)
#define OEMBTMGR_FLAG_SET_BLOCKED_DEVS          (1 << 10)
#define OEMBTMGR_FLAG_SET_CONN_LOW_POWER_CFG    (1 << 11)
#define OEMBTMGR_FLAG_AUTH_CONN                 (1 << 12)
#define OEMBTMGR_FLAG_PERF_TUNING               (1 << 13)
#define OEMBTMGR_FLAG_DISABLE_RADIO             (1 << 14)
#define OEMBTMGR_FLAG_ENABLE_RADIO              (1 << 15)
#define OEMBTMGR_FLAG_DISABLE_VIS               (1 << 16)
#define OEMBTMGR_FLAG_RESTORE_VIS               (1 << 17)

/************************************************************************ 
                           PRIVATE FUNCTIONS
************************************************************************/
static void OEMBTMgr_ProcessCmdDone (
   OEMBTMgr*               pMe, 
   bt_ev_gn_cmd_done_type* ev_ptr,
   OEMBTMgrEvent*          pOemEv
)
{
   MSG_HIGH ("Mgr_ProcessCmdDone - cmd=0x%x stat=0x%x", 
              ev_ptr->cmd_type, ev_ptr->cmd_status,0 );

   pOemEv->dResult = OEMBT_CmdStatus2Result (ev_ptr->cmd_status);
   switch (ev_ptr->cmd_type)
   {
      case BT_CMD_RM_CONFIG_APPLICATION:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_PERF_TUNING)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_PERF_TUNING;
            pOemEv->dType = OEMBTMGR_EV_PERF_TUNING_DONE;
         }
         break;
      case BT_CMD_RM_SET_CONN_ROLE:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_ROLE)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_ROLE;
            pOemEv->dType = OEMBTMGR_EV_SET_ROLE_DONE;
         }
         else if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_CONN_ROLE)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_CONN_ROLE;
            pOemEv->dType = OEMBTMGR_EV_SET_CONN_ROLE_DONE;
         }
         break;
      case BT_CMD_RM_SET_CONN_ROLE_SW_REQ_OK:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_ROLE_SW_POLICY)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_ROLE_SW_POLICY;
            pOemEv->dType = OEMBTMGR_EV_SET_SW_POL_DONE;
         }
         else if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_CONN_ROLE_SW_POLICY)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_CONN_ROLE_SW_POLICY;
            pOemEv->dType = OEMBTMGR_EV_SET_CONN_SW_POL_DONE;
         }
         break;
      case BT_CMD_RM_SET_CONN_SECURITY:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_CONN_SEC)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_CONN_SEC;
            pOemEv->dType = OEMBTMGR_EV_SET_CONN_SEC_DONE;
         }
         break;
      case BT_CMD_RM_SET_DEVICE_SECURITY:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_SEC)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_SEC;
            pOemEv->dType = OEMBTMGR_EV_SET_SEC_DONE;
         }
         break;
#ifdef FEATURE_BT_2_1
      case BT_CMD_RM_SET_SM4:
#else
      case BT_CMD_RM_SET_SERVICE_SECURITY:
#endif
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_SVC_SEC)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_SVC_SEC;
            pOemEv->dType = OEMBTMGR_EV_SET_SVC_SEC_DONE;
         }
         break;
      case BT_CMD_RM_SET_DEVICE_BLACK_LIST:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_BLOCKED_DEVS)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_BLOCKED_DEVS;
            pOemEv->dType = OEMBTMGR_EV_SET_BLOCKED_DEVS_DONE;
         }
         break;
      case BT_CMD_RM_AUTHORIZE_REPLY:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_AUTH_CONN)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_AUTH_CONN;
            pOemEv->dType = OEMBTMGR_EV_AUTH_CONN_DONE;
         }
         break;
      case BT_CMD_RM_SET_LOCAL_INFO:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_NAME)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_NAME;
            pOemEv->dType = OEMBTMGR_EV_SET_NAME_DONE;
         }
         else if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_COD)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_COD;
            pOemEv->dType = OEMBTMGR_EV_SET_COD_DONE;
         }
         break;
      case BT_CMD_RM_DISABLE_RADIO:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_DISABLE_RADIO)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_DISABLE_RADIO;
            pOemEv->dType = OEMBTMGR_EV_DISABLE_RADIO_DONE;
         }
         else if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_ENABLE_RADIO)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_ENABLE_RADIO;
            pOemEv->dType = OEMBTMGR_EV_ENABLE_RADIO_DONE;
         }
         break;
      case BT_CMD_RM_DISABLE_DISCOVERABILITY:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_DISABLE_VIS)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_DISABLE_VIS;
            pOemEv->dType = OEMBTMGR_EV_DISABLE_VIS_DONE;
         }
         else if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_RESTORE_VIS)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_RESTORE_VIS;
            pOemEv->dType = OEMBTMGR_EV_RESTORE_VIS_DONE;
         }
         break;
      case BT_CMD_RM_LOW_POWER_CONFIG:
         if (pgTheOEMBTMgr->uFlags & OEMBTMGR_FLAG_SET_CONN_LOW_POWER_CFG)
         {
            pgTheOEMBTMgr->uFlags &= ~OEMBTMGR_FLAG_SET_CONN_LOW_POWER_CFG;
            pOemEv->dType = OEMBTMGR_EV_SET_CONN_LPM_DONE;
         }
         break;
      default:
         MSG_ERROR ("Mgr_ProcessCmdDone - unexp cmd 0x%x", 
                          ev_ptr->cmd_type,0,0);
         break;
   }
   MSG_LOW ("Mgr_ProcessCmdDone - ev=0x%x result=0x%x",
                    pOemEv->dType, pOemEv->dResult,0 );
}

static void OEMBTMgr_CoreEv2OemEv (
   OEMBTMgr*       pMe,
   bt_ev_msg_type* pEv, 
   OEMBTMgrEvent*  pOemEv
)
{
   switch (pEv->ev_hdr.ev_type)
   {
      case BT_EV_GN_CMD_DONE:
         OEMBTMgr_ProcessCmdDone (pMe, &pEv->ev_msg.ev_gn_cmd_done, pOemEv);
         break;
      case BT_EV_RM_RADIO_DISABLED:
         pOemEv->dType = (pEv->ev_msg.ev_rm_radio.disabled == FALSE) ?
            OEMBTMGR_EV_RADIO_ON : OEMBTMGR_EV_RADIO_OFF;
         break;
      case BT_EV_RM_RADIO_ACTIVITY:
         pOemEv->dType = 
            (pEv->ev_msg.ev_rm_ract.radio_activity & BT_RA_INQ_SCANNING) ?
            OEMBTMGR_EV_VIS_ON : OEMBTMGR_EV_VIS_OFF;
         break;
      case BT_EV_RM_LINK_STATUS:
         pOemEv->dType = OEMBTMGR_EV_CONN_STAT_CHANGED;
         break;
      default:
         MSG_ERROR ("Mgr_CoreEv2OemEv - unexp core ev 0x%x",
                          pEv->ev_hdr.ev_type,0,0);
         pOemEv->dType = OEMBTMGR_EV_MAX;
         break;
   }
}

static bt_rm_act_vs_pwr_type OEMBTMgr_Brew2CoreScanParam (BTActVsPwr param)
{
   switch (param)
   {
      case AEEBT_AVP_SMALLEST_ACT_MOST_POWER:
         return BT_RM_AVP_SMALLEST_ACT_MOST_POWER;
      case AEEBT_AVP_LARGER_ACT_LESS_POWER:
         return BT_RM_AVP_SMALLEST_ACT_MOST_POWER;
      case AEEBT_AVP_LARGEST_ACT_LEAST_POWER:
         return BT_RM_AVP_LARGEST_ACT_LEAST_POWER;
      default:
         return BT_RM_AVP_AUTOMATIC;
   }
}

static int OEMBTMgr_Brew2CoreSvcId (
   const BTServiceIdentifier* pSvcId, 
   bt_service_id_type* service_ident_ptr,
   int i
)
{
   int result = AEE_SUCCESS;
   switch (pSvcId->dIDMethod)
   {
      case AEEBT_SEC_BY_SERVICE_CLASS:
         service_ident_ptr->service_id_method = BT_SIM_SDP_UUID;
         service_ident_ptr->sdp_uuid = pSvcId->dSvcCls[i];
         break;
      case AEEBT_SEC_BY_SERVER_CHANNEL:
         service_ident_ptr->service_id_method = BT_SIM_RFCOMM_SCN;
         service_ident_ptr->rfcomm_scn = (uint8)pSvcId->uSvrChnl[i];
         break;
      case AEEBT_SEC_BY_L2CAP_PSM:
         service_ident_ptr->service_id_method = BT_SIM_L2CAP_PSM;
         service_ident_ptr->l2cap_psm = (uint16)pSvcId->uPSM[i];
         break;
      case AEEBT_SEC_BY_PROTOCOL:
         if (pSvcId->dProtocol[i] == AEEBT_PROT_RFCOMM)
         {
            service_ident_ptr->service_id_method = BT_SIM_DEFAULT_RFCOMM;
         }
         else if (pSvcId->dProtocol[i] == AEEBT_PROT_L2CAP)
         {
            service_ident_ptr->service_id_method = BT_SIM_DEFAULT_L2CAP;
         }
         else
         {
            result = AEE_EUNSUPPORTED;
         }
         break;
      default:
         result = AEE_EUNSUPPORTED;
   }
   return result;
}

static void OEMBTMgr_SendConnEv (OEMBTMgr* pMe, OEMBTMgrEvent* pEv)
{
   BTMgrConnEvCb* pEvCb = pMe->pConnEvCbFnList;
   pEv->pNext = NULL;
   while (NULL != pEvCb)
   {
      (pEvCb->pEvCbFn) (pEvCb->piConn, pEv);
      pEvCb = pEvCb->pNext;
   }
}

static void OEMBTMgr_SendMgrEv (OEMBTMgr* pMe, OEMBTMgrEvent* pEv)
{
   BTMgrEvCb* pEvCb = pMe->pMgrEvCbFnList;
   pEv->pNext = NULL;
   while (NULL != pEvCb)
   {
      (pEvCb->pEvCbFn) (pEvCb->piMgr, pEv);
      pEvCb = pEvCb->pNext;
   }
}
static void OEMBTMgr_ConnReqCallback(void *pData)
{
   OEMBTMgr* pMe = (OEMBTMgr*)pData;

   if (NULL != pMe->piConnReqSignalBus)
   {              
      ISignalBus_Strobe (pMe->piConnReqSignalBus);
   }
   return;
}
// called from BT task context; so just convert and queue the event
static void OEMBTMgr_EventCallback (bt_ev_msg_type* bt_ev_msg_ptr)
{
   OEMBTMgr* pMe = pgTheOEMBTMgr;
   OEMBTMgrEvent ev;

   if ((NULL == pMe) || (NULL == pMe->pMgrEvCbFnList) ||
       (bt_ev_msg_ptr->ev_hdr.bt_app_id != pMe->appId))
   {
      return;
   }
   switch (bt_ev_msg_ptr->ev_hdr.ev_type)
   {
      case BT_EV_RM_AUTHORIZE_REQUEST:
      {
         int i;
         BTServiceIdentifier* pSvcId = NULL;

         OEMBTMGR_LOCK();
         for (i = 0; i < ARR_SIZE(pMe->pendingRequest); i++)
         {
            if (FALSE == pMe->pendingRequest[i].bValid)
            {
               bt_serv_id_method_type sim = bt_ev_msg_ptr->ev_msg.ev_rm_atzrq.
                                               service_id.service_id_method;
               pSvcId = &pMe->pendingRequest[i].serviceId;
               pSvcId->nNumItems = 1;
               switch (sim)
               {
                  case BT_SIM_SDP_UUID:
                     pSvcId->dIDMethod    = AEEBT_SEC_BY_SERVICE_CLASS;
                     pSvcId->dSvcCls[0]   = 
                        bt_ev_msg_ptr->ev_msg.ev_rm_atzrq.service_id.sdp_uuid;
                     break;
                  case BT_SIM_RFCOMM_SCN:
                     pSvcId->dIDMethod    = AEEBT_SEC_BY_SERVER_CHANNEL;
                     pSvcId->uSvrChnl[0]  = 
                        bt_ev_msg_ptr->ev_msg.ev_rm_atzrq.service_id.rfcomm_scn;
                     break;
                  case BT_SIM_L2CAP_PSM:
                     pSvcId->dIDMethod    = AEEBT_SEC_BY_L2CAP_PSM;
                     pSvcId->uPSM[0]      = 
                        bt_ev_msg_ptr->ev_msg.ev_rm_atzrq.service_id.l2cap_psm;
                     break;
                  case BT_SIM_DEFAULT_RFCOMM:
                     pSvcId->dIDMethod    = AEEBT_SEC_BY_PROTOCOL;
                     pSvcId->dProtocol[0] = AEEBT_PROT_RFCOMM;
                     break;
                  case BT_SIM_DEFAULT_L2CAP:
                     pSvcId->dIDMethod    = AEEBT_SEC_BY_PROTOCOL;
                     pSvcId->dProtocol[0] = AEEBT_PROT_L2CAP;
                     break;
                  default:
                     MSG_ERROR (
                        "OEMBTMgr_EvCb - unexp svc id method %d", sim,0,0);
                     pSvcId = NULL;
                     i = ARR_SIZE(pMe->pendingRequest);
               }
               if (i < ARR_SIZE(pMe->pendingRequest))
               {
                  pMe->pendingRequest[i].remoteAddr = 
                     *((BDAddress*)&bt_ev_msg_ptr->ev_msg.ev_rm_atzrq.bd_addr);
                  pMe->pendingRequest[i].bValid = TRUE;
               }
               break;
            }
         }
         OEMBTMGR_UNLOCK();

         if (NULL == pSvcId)
         {
            MSG_ERROR ("OEMBTMgr_EvCb - dropping event",0,0,0);
         }
         else if (NULL != pMe->piConnReqSignalBus)
         {
            CALLBACK_Cancel(&pMe->BREWEvCb);
            CALLBACK_Init(&pMe->BREWEvCb, OEMBTMgr_ConnReqCallback, pMe);
            AEE_SYS_RESUME(&pMe->BREWEvCb);
         }
         else
         {
            ev.dType = OEMBTMGR_EV_AUTH_REQ;
            OEMBTMgr_SendMgrEv (pMe, &ev);
         }
         break;
      }
      default:
      {
         OEMBTMgr_CoreEv2OemEv (pMe, bt_ev_msg_ptr, &ev);
         if ((OEMBTMGR_EV_SET_CONN_SEC_DONE <= ev.dType) &&
             (OEMBTMGR_EV_CONN_STAT_CHANGED >= ev.dType))
         {
            MSG_LOW ("OEMBTMgr_EventCb - sending conn ev=%d", ev.dType,0,0);
            OEMBTMgr_SendConnEv (pMe, &ev);
         }
         else if ((OEMBTMGR_EV_MAX != ev.dType) && 
                  (OEMBTMGR_EV_NONE != ev.dType))
         {
            MSG_LOW ("OEMBTMgr_EventCb - sending ev=%d", ev.dType,0,0);
            OEMBTMgr_SendMgrEv (pMe, &ev);
         }
         break;
      }
   }
}

static void OEMBTMgr_GetLocalInfo (OEMBTMgr* pMe)
{
   bt_role_type     role;
   bt_version_type  version;
   bt_cod_type      cod;
   bt_security_type security;
   BTLocalSettings* pMyInfo = &pMe->myInfo;

   bt_cmd_rm_get_connection_role (pMe->appId, NULL, &role);
   pMyInfo->bPrefMasterRole = (role == BT_ROLE_MASTER) ? TRUE : FALSE;

   bt_cmd_rm_get_conn_role_sw_req_ok (pMe->appId, NULL, 
                                      &pMyInfo->bRoleSwitchAllowed);
   bt_cmd_rm_get_local_info (pMe->appId, (bt_bd_addr_type*)&pMyInfo->bdAddr, 
                             &version, 
                             (bt_lmp_features_type*)&pMyInfo->lmpFeatures, 
                             &cod, (char*)pMyInfo->u8Name, &security);
   pMyInfo->lmpVersion.VersNum      = version.lmp_version;
   pMyInfo->lmpVersion.SubVersNum   = version.lmp_subversion;
   pMyInfo->lmpVersion.CompID       = version.manufacturer_name;

   bt_sd_parse_class_of_device (&cod, (uint16*)&pMyInfo->CoD.COS,
                                (uint16*)&pMyInfo->CoD.DevCls, 
                                (uint16*)&pMyInfo->CoD.SubDevCls);
   pMyInfo->CoD.SubDevCls -= pMyInfo->CoD.DevCls;

   switch (security)
   {
      case BT_SEC_NONE:
         pMyInfo->dSecLevel = AEEBT_SEC_NONE;
         break;
      case BT_SEC_AUTHENTICATE:
         pMyInfo->dSecLevel = AEEBT_SEC_AUTHENTICATE;
         break;
      case BT_SEC_AUTHENTICATE_AND_ENCRYPT:
         pMyInfo->dSecLevel = AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT;
         break;
   }
}

/************************************************************************ 
                       Exposed to AEE layer
************************************************************************/
void OEMBTMgr_Release (OEMBTMgr* pMe)
{
   if ((NULL == pMe) || (pMe != pgTheOEMBTMgr))
   {
      DBGPRINTF_ERROR ("OEMBTMgr_Release - pMe=0x%x pgTheOEMBTMgr=%0x%x",
                       pMe, pgTheOEMBTMgr);
   }
   else if (pMe->uRefs > 0)
   {
      pMe->uRefs--;
      if (0 == pMe->uRefs)
      {
         if (BT_APP_ID_NULL != pMe->appId)
         {
            bt_cmd_rm_unregister_radio_activity (pMe->appId);
            bt_cmd_ec_free_application_id (pMe->appId);
            pMe->appId = BT_APP_ID_NULL;
         }
         if (NULL != pMe->piShell)
         {
            ISHELL_Release (pMe->piShell);
            pMe->piShell = NULL;
         }
         OEM_Free (pMe);
         pgTheOEMBTMgr = NULL;
      }
   }
}

int OEMBTMgr_New (IShell* piShell, OEMBTMgr** ppo)
{
   int result = AEE_EFAILED;

   if (NULL == pgTheOEMBTMgr)
   {
      pgTheOEMBTMgr = OEM_Malloc (sizeof(OEMBTMgr));
      if (NULL == pgTheOEMBTMgr)
      {
         DBGPRINTF_ERROR ("OEMBTMgr_New - no mem for obj");
      }
      else if ((pgTheOEMBTMgr->appId = 
                bt_cmd_ec_get_application_id ()) == BT_APP_ID_NULL)
      {
         DBGPRINTF_ERROR ("OEMBTMgr_New - failed to get appID");
      }
      else if (bt_cmd_ec_register_event_set (
                 pgTheOEMBTMgr->appId, OEMBTMgr_EventCallback, 
                 BT_EC_ES_RADIO_MANAGER,
                 BT_EV_NULL, BT_EV_NULL) != BT_CS_GN_SUCCESS)
      {
         DBGPRINTF_ERROR ("OEMBTMgr_New - failed to register for RM events");
      }
      else if (bt_cmd_ec_register_event_set (
                 pgTheOEMBTMgr->appId, OEMBTMgr_EventCallback, BT_EC_ES_CUSTOM,
                 BT_EV_GN_CMD_DONE, BT_EV_GN_CMD_DONE) != BT_CS_GN_SUCCESS)
      {
         DBGPRINTF_ERROR ("OEMBTMgr_New - failed to register for CMD_DONE ev");
      }
      else
      {
         ISHELL_AddRef (piShell);
         pgTheOEMBTMgr->piShell = piShell;
         pgTheOEMBTMgr->uRefs = 1;

         OEMBTMgr_GetLocalInfo (pgTheOEMBTMgr);
         bt_cmd_rm_register_radio_activity( pgTheOEMBTMgr->appId );
         *ppo = pgTheOEMBTMgr;
         result = AEE_SUCCESS;
      }
   }
   else
   {
      pgTheOEMBTMgr->uRefs++;
      *ppo = pgTheOEMBTMgr;
      result = AEE_SUCCESS;
   }

   if (NULL != pgTheOEMBTMgr)
   {
      DBGPRINTF_MED ("OEMBTMgr_New - ref count = %d", pgTheOEMBTMgr->uRefs);
   }
#ifdef BT_SIMULATION
   bt_sim_start(PROFILE_GAP, FALSE);
#endif
   return result;
}

void OEMBTMgr_DeregisterEventCb (OEMBTMgr* pMe, IBTMgr* piMgr)
{
   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && (NULL != piMgr))
   {
      BTMgrEvCb* pEvCb = pMe->pMgrEvCbFnList;
      BTMgrEvCb* pPrevEvCb = NULL;

      OEMBTMGR_LOCK();
      while (NULL != pEvCb)
      {
         if (pEvCb->piMgr == piMgr)
         {
            if (NULL != pPrevEvCb)
            {
               pPrevEvCb->pNext = pEvCb->pNext;
            }
            else
            {
               pMe->pMgrEvCbFnList = pEvCb->pNext;
            }
            FREE (pEvCb);
            break;
         }
         pPrevEvCb = pEvCb;
         pEvCb = pEvCb->pNext;
      }
      OEMBTMGR_UNLOCK();
   }
}

int OEMBTMgr_RegisterEventCb (
   OEMBTMgr*         pMe,
   MgrEvCallbackFn*  pCbFn,
   IBTMgr*           piMgr
)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && 
       (NULL != pCbFn) && (NULL != piMgr))
   {
      BTMgrEvCb** ppEvCb = &pMe->pMgrEvCbFnList;
      OEMBTMGR_LOCK();
      // find the end of the callback function list
      while ((NULL != ppEvCb) && (NULL != *ppEvCb))
      {
         ppEvCb = &((*ppEvCb)->pNext);
      }
      DBGPRINTF ("Mgr_RegEvCb - ppEvCb=0x%x", ppEvCb);
      if (NULL != ppEvCb)
      {
         *ppEvCb = MALLOCREC (BTMgrEvCb);
         if (NULL != *ppEvCb)
         {
            (*ppEvCb)->pEvCbFn = pCbFn;
            (*ppEvCb)->piMgr   = piMgr;
            (*ppEvCb)->pNext   = NULL;
            result = AEE_SUCCESS;
         }
      }
      OEMBTMGR_UNLOCK();
   }
   return result;
}


void OEMBTMgr_DeregisterConnEv (OEMBTMgr* pMe, IBTConnection* piConn)
{
   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && (NULL != piConn))
   {
      BTMgrConnEvCb* pEvCb = pMe->pConnEvCbFnList;
      BTMgrConnEvCb* pPrevEvCb = NULL;

      OEMBTMGR_LOCK();
      while (NULL != pEvCb)
      {
         if (pEvCb->piConn == piConn)
         {
            if (NULL != pPrevEvCb)
            {
               pPrevEvCb->pNext = pEvCb->pNext;
            }
            else
            {
               pMe->pConnEvCbFnList = pEvCb->pNext;
            }
            OEM_Free (pEvCb);
            break;
         }
         pPrevEvCb = pEvCb;
         pEvCb = pEvCb->pNext;
      }
      OEMBTMGR_UNLOCK();
   }
}

int OEMBTMgr_RegisterConnEv (
   OEMBTMgr* pMe, 
   ConnEvCallbackFn* pCbFn,
   IBTConnection* piConn,
   const BDAddress* pAddr
)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && 
       (NULL != pCbFn) && (NULL != piConn))
   {
      BTMgrConnEvCb** ppEvCb = &pMe->pConnEvCbFnList;
      OEMBTMGR_LOCK();
      // find the end of the signal list
      while ((NULL != ppEvCb) && (NULL != *ppEvCb))
      {
         ppEvCb = &((*ppEvCb)->pNext);
      }
      DBGPRINTF ("Mgr_RegConnEv - ppEvCb=0x%x", ppEvCb);
      if (NULL != ppEvCb)
      {
         *ppEvCb = OEM_Malloc (sizeof(BTMgrConnEvCb));
         if (NULL != *ppEvCb)
         {
            (*ppEvCb)->pEvCbFn = pCbFn;
            (*ppEvCb)->piConn = piConn;
            (*ppEvCb)->pNext = NULL;
            result = AEE_SUCCESS;
         }
      }
      OEMBTMGR_UNLOCK();
   }
   if (AEE_SUCCESS == result)
   {
      bt_cmd_rm_register_link_status (pMe->appId, (bt_bd_addr_type*)pAddr,
                                      TRUE, TRUE, TRUE, FALSE, FALSE, 1);
   }
   return result;
}

int OEMBTMgr_GetConnRequest (
   OEMBTMgr* pMe,
   BDAddress* pAddr, 
   BTServiceIdentifier* pSvcId
)
{
   int result = AEE_EFAILED;
   if ((NULL == pMe) || (pgTheOEMBTMgr != pMe) ||
       (NULL == pAddr) || (NULL == pSvcId))
   {
      result = AEE_EBADPARM;
   }
   else
   {
      int i;
      OEMBTMGR_LOCK();
      for (i = 0; i < ARR_SIZE(pMe->pendingRequest); i++)
      {
         if (FALSE != pMe->pendingRequest[i].bValid)
         {
            *pAddr  = pMe->pendingRequest[i].remoteAddr;
            *pSvcId = pMe->pendingRequest[i].serviceId;
            result  = AEE_SUCCESS;
            break;
         }
      }
      OEMBTMGR_UNLOCK();

      if (i == ARR_SIZE(pMe->pendingRequest))
      {
         result = AEE_ENOSUCH;
      }
   }
   return result;
}

int OEMBTMgr_GetConnStatus (
   OEMBTMgr* pMe, 
   BTConnectionStatus* pStatus,
   boolean bGetInfo
)
{
   int result = AEE_EBADPARM;
   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && (NULL != pStatus))
   {
      bt_cmd_status_type stat;
      bt_rm_link_status_type status;
      status.bd_addr = *((bt_bd_addr_type*)&pStatus->about.bdAddr);
      if ((stat = bt_cmd_rm_get_link_status (pMe->appId, 
                                             &status)) != BT_CS_GN_SUCCESS)
      {
         result = OEMBT_CmdStatus2AEEResult (stat);
      }
      else
      {
         // security
         switch (status.security)
         {
            case BT_SEC_NONE:
               pStatus->dSecurity = AEEBT_SEC_NONE;
               break;
            case BT_SEC_AUTHENTICATE:
               pStatus->dSecurity = AEEBT_SEC_AUTHENTICATE;
               break;
            case BT_SEC_AUTHENTICATE_AND_ENCRYPT:
            default:
               pStatus->dSecurity = AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT;
               break;
         }
         // mode
         switch (status.link_mode)
         {
            case BT_LM_ACTIVE:
               pStatus->dConnMode = AEEBT_CONN_MODE_ACTIVE;
               break;
            case BT_LM_HOLD:
               pStatus->dConnMode = AEEBT_CONN_MODE_HOLD;
               break;
            case BT_LM_SNIFF:
               pStatus->dConnMode = AEEBT_CONN_MODE_SNIFF;
               break;
            case BT_LM_PARK:
               pStatus->dConnMode = AEEBT_CONN_MODE_PARK;
               break;
            default:
               MSG_ERROR ("Mgr_GetConnStat - unknown link mode %d",
                                status.link_mode, 0, 0);
               pStatus->dConnMode = AEEBT_CONN_MODE_ACTIVE;
               break;
         }
         // role
         pStatus->bLocalIsMaster = 
            (status.role == BT_ROLE_MASTER) ? TRUE : FALSE;
         pStatus->RSSI = status.rssi;
         result = AEE_SUCCESS;

         // others
         pStatus->RSSI = status.rssi;
         pStatus->bRoleSwitchAllowed = status.role_sw_ok; // driver change
         pStatus->bSCOPresent = status.sco_present; // driver change required

         if (FALSE != bGetInfo)
         {
            BTClassOfDevice* pCoD = &pStatus->about.COD;
            int nLen;
            bt_device_type dev;
            dev.bd_addr = status.bd_addr;

            bt_cmd_rm_device_read (pMe->appId, &dev, NULL, NULL);

            bt_sd_parse_class_of_device (&dev.class_of_device,
                                         (uint16*)&pCoD->COS,
                                         (uint16*)&pCoD->DevCls, 
                                         (uint16*)&pCoD->SubDevCls);
            pCoD->SubDevCls -= pCoD->DevCls;
            pStatus->about.LMPFeatures = 
               *((BTLMPFeatures*)&dev.lmp_features);
            pStatus->about.LMPVersion.CompID = 
               (BTCompanyID) dev.version.manufacturer_name;
            pStatus->about.LMPVersion.VersNum = 
               dev.version.lmp_version;
            pStatus->about.LMPVersion.SubVersNum = 
               dev.version.lmp_subversion;
            nLen = sizeof(pStatus->about.szName);
            if (sizeof(dev.name_str) < nLen)
            {
               nLen = sizeof(dev.name_str);
            }
            std_memmove (pStatus->about.szName, (void*)dev.name_str, nLen);
         }

      }
   }
   return result;
}

int OEMBTMgr_ConnRequestDeregister (OEMBTMgr* pMe)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_ConnReqDereg - entered");
   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && 
       (NULL != pMe->piConnReqSignalBus))
   {
      CALLBACK_Cancel(&pMe->BREWEvCb);
      ISignalBus_Clear (pMe->piConnReqSignalBus);
      ISignalBus_Release (pMe->piConnReqSignalBus);
      pMe->piConnReqSignalBus = NULL;
      result = AEE_SUCCESS;
   }
   return result;
}

int OEMBTMgr_ConnRequestRegister (OEMBTMgr* pMe, ISignal* piSig)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_ConnReqReg - piSig=0x%x", piSig);
   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && (NULL != piSig))
   {
      if (NULL != pMe->piConnReqSignalBus)
      {
         DBGPRINTF_ERROR ("Mgr_ConnReqReg - too late!");
         result = AEE_EALREADY;
      }
      else if (ISHELL_CreateInstance (
                 pMe->piShell, AEECLSID_SignalBus,
                 (void**)&pMe->piConnReqSignalBus) != SUCCESS)
      {
         DBGPRINTF_ERROR ("Mgr_ConnReqReg - failed to create signal bus");
      }
      else
      {
         ISignalBus_Remove (pMe->piConnReqSignalBus, piSig);
         ISignalBus_Add (pMe->piConnReqSignalBus, piSig);
         result = AEE_SUCCESS;
      }
   }
   return result;
}

void OEMBTMgr_GetLocalSettings (OEMBTMgr* pMe, BTLocalSettings* pSettings)
{
   MSG_LOW ("Mgr_GetLocalSettings - pSettings=0x%x", pSettings,0,0);
   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && (NULL != pSettings))
   {
      OEMBTMgr_GetLocalInfo (pMe);
      *pSettings = pMe->myInfo;
   }
}

int OEMBTMgr_SetName (OEMBTMgr* pMe, const uint8* pszName)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_SetName - pszName=0x%x", pszName);

   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && (NULL != pszName))
   {
      bt_cmd_status_type stat;

      DBGPRINTF ("Mgr_SetName - name=%s", pszName);

      pMe->uFlags |= OEMBTMGR_FLAG_SET_NAME;
      stat = bt_cmd_rm_set_local_info (pMe->appId, NULL, (char*)pszName);
      result = OEMBT_CmdStatus2AEEResult (stat);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~OEMBTMGR_FLAG_SET_NAME;
      }
   }
   return result;
}

int OEMBTMgr_SetCoD (OEMBTMgr* pMe, const BTClassOfDevice* pCoD)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_SetCoD - pCoD=0x%x", pCoD);
   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && (NULL != pCoD))
   {
      bt_cmd_status_type stat;
      bt_cod_type cod;
      bt_sd_build_class_of_device (pCoD->COS, pCoD->DevCls, 
                                   pCoD->DevCls + pCoD->SubDevCls, &cod);
      pMe->uFlags |= OEMBTMGR_FLAG_SET_COD;
      stat = bt_cmd_rm_set_local_info (pMe->appId, &cod, NULL);
      result = OEMBT_CmdStatus2AEEResult (stat);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~OEMBTMGR_FLAG_SET_COD;
      }
   }
   return result;
}

int OEMBTMgr_SetSecurity (
   OEMBTMgr* pMe, 
   const BDAddress* pAddr, 
   BTSecurityLevel dSec
)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_SetSec - sec=%d", dSec);

#ifdef FEATURE_BT_2_1
   result = AEE_EUNSUPPORTED;
#else
   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe))
   {
      bt_cmd_status_type stat;
      uint32 flags = 0;
      bt_security_type sec = BT_SEC_AUTHENTICATE_AND_ENCRYPT;
      switch (dSec)
      {
         case AEEBT_SEC_NONE:
            sec = BT_SEC_NONE;
            break;
         case AEEBT_SEC_AUTHENTICATE:
            sec = BT_SEC_AUTHENTICATE;
            break;
         case AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT:
            sec = BT_SEC_AUTHENTICATE_AND_ENCRYPT;
            break;
      }
      if (NULL == pAddr)
      {
         flags = OEMBTMGR_FLAG_SET_SEC;
         pMe->uFlags |= flags;
         stat = bt_cmd_rm_set_device_security (pMe->appId, NULL, sec);
      }
      else
      {
         flags = OEMBTMGR_FLAG_SET_CONN_SEC;
         pMe->uFlags |= flags;
         stat = bt_cmd_rm_set_connection_security(
            pMe->appId, (bt_bd_addr_type*)pAddr, sec);
      }
      result = OEMBT_CmdStatus2AEEResult (stat);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~flags;
      }
   }
#endif /* FEATURE_BT_2_1 */
   return result;
}

int OEMBTMgr_SetPrefConnRole (
   OEMBTMgr* pMe, 
   const BDAddress* pAddr, 
   boolean bMaster
)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_SetPrefConnRole - bMaster=0x%x", bMaster);

   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe))
   {
      bt_cmd_status_type stat;
      bt_role_type role = (bMaster == FALSE) ? BT_ROLE_SLAVE : BT_ROLE_MASTER;
      uint32 flags = (NULL == pAddr) ? 
         OEMBTMGR_FLAG_SET_ROLE : OEMBTMGR_FLAG_SET_CONN_ROLE;
      pMe->uFlags |= flags;
      stat = bt_cmd_rm_set_connection_role (
               pMe->appId, (bt_bd_addr_type*)pAddr, role);
      result = OEMBT_CmdStatus2AEEResult (stat);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~flags;
      }
   }
   return result;
}

int OEMBTMgr_SetRoleSwitchPolicy (
   OEMBTMgr* pMe, 
   const BDAddress* pAddr, 
   boolean bAllowed
)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_SetRoleSwitchPolicy - bAllowed=0x%x", bAllowed);

   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe))
   {
      bt_cmd_status_type stat;
      uint32 flags = (NULL == pAddr) ? 
         OEMBTMGR_FLAG_SET_ROLE_SW_POLICY : 
         OEMBTMGR_FLAG_SET_CONN_ROLE_SW_POLICY;
      pMe->uFlags |= flags;
      stat = bt_cmd_rm_set_conn_role_sw_req_ok (
               pMe->appId, (bt_bd_addr_type*)pAddr, bAllowed);
      result = OEMBT_CmdStatus2AEEResult (stat);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~flags;
      }
   }
   return result;
}

int OEMBTMgr_SetSvcSecurity (
   OEMBTMgr* pMe,
   BTSecurityLevel sec,
   const BTServiceIdentifier* pSvcId
)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_SetSvcSecurity - sec=%d pSvcId=0x%x", sec, pSvcId);

   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && (NULL != pSvcId))
   {
      bt_service_id_type service_ident;
      bt_cmd_status_type stat;
      int i = 0;

#ifndef FEATURE_BT_2_1
      bt_security_type minSec;

      switch (sec)
      {
         case AEEBT_SEC_NONE:
            minSec = BT_SEC_NONE;
            break;
         case AEEBT_SEC_AUTHENTICATE:
            minSec = BT_SEC_AUTHENTICATE;
            break;
         default:
            minSec = BT_SEC_AUTHENTICATE_AND_ENCRYPT;
            break;
      }
#endif
      while ((i < pSvcId->nNumItems) && (i < ARR_SIZE(pSvcId->dSvcCls)) &&
             (OEMBTMgr_Brew2CoreSvcId (pSvcId, &service_ident, 
                                       i++) == AEE_SUCCESS))
      {
         pMe->uFlags |= OEMBTMGR_FLAG_SET_SVC_SEC;
#ifndef FEATURE_BT_2_1
         stat = bt_cmd_rm_set_service_security (pMe->appId, &service_ident, 
                                                minSec, TRUE, FALSE);
#else
         stat = bt_cmd_rm_set_sm4 (pMe->appId, 
                                   &service_ident, 
                                   BT_RM_SL_1_LOW, 
                                   TRUE, 
                                   FALSE );
#endif
         result = OEMBT_CmdStatus2AEEResult (stat);
         if (AEE_SUCCESS != result)
         {
            pMe->uFlags &= ~OEMBTMGR_FLAG_SET_SVC_SEC;
         }
      }
   }
   return result;
}

int OEMBTMgr_SetBlockedDevList (
   OEMBTMgr* pMe,
   int nNumDev,
   const BDAddress* pAddrList
)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_SetBlockedDevList - nNum=%d AddrList=0x%x", 
              nNumDev, pAddrList);

   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && 
       ((nNumDev == 0) || (NULL != pAddrList)))
   {
      bt_cmd_status_type stat;
      pMe->uFlags |= OEMBTMGR_FLAG_SET_BLOCKED_DEVS;
      stat = bt_cmd_rm_set_device_black_list (pMe->appId, nNumDev,
                                          (bt_bd_addr_type*)pAddrList);
      result = OEMBT_CmdStatus2AEEResult (stat);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~OEMBTMGR_FLAG_SET_BLOCKED_DEVS;
      }
   }
   return result;
}

int OEMBTMgr_SetLowPowerConfig (
   OEMBTMgr* pMe,
   const BDAddress* pAddr,
   BTLowPowerMode dLowPowerMode,
   uint16 uMaxInterval, 
   uint16 uIdleTimeout
)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_SetLowPowerConfig - mode=%d", dLowPowerMode);

   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && (NULL != pAddr))
   {
      bt_cmd_status_type stat;
      bt_low_power_ctrl_type ctrl_type;
      switch (dLowPowerMode)
      {
         case AEEBT_LPM_ACTIVE:
            ctrl_type = BT_LPC_ACTIVE;
            break;
         case AEEBT_LPM_SNIFF:
            ctrl_type = BT_LPC_SNIFF;
            break;
         case AEEBT_LPM_SNIFF_LOCK:
            ctrl_type = BT_LPC_SNIFF_LOCK;
            break;
         case AEEBT_LPM_DEFAULT:
         default:
            ctrl_type = BT_LPC_DONT_CARE;
            break;
      }
      pMe->uFlags |= OEMBTMGR_FLAG_SET_CONN_LOW_POWER_CFG;
      stat = bt_cmd_rm_low_power_config (pMe->appId, (bt_bd_addr_type*)pAddr, 
                                         ctrl_type, uMaxInterval, uIdleTimeout);
      result = OEMBT_CmdStatus2AEEResult (stat);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~OEMBTMGR_FLAG_SET_CONN_LOW_POWER_CFG;
      }
   }
   return result;
}

int OEMBTMgr_AuthorizeConn (
   OEMBTMgr* pMe,
   const BDAddress* pAddr,
   const BTServiceIdentifier* pSvcId,
   boolean bAccept
)
{
   int result = AEE_EBADPARM;
   MSG_HIGH ("Mgr_AuthorizeConn - accept=%d f=0x%x", bAccept,0,0);

   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) &&
       (NULL != pAddr) && (NULL != pSvcId))
   {
      bt_service_id_type service_ident;
      int i;

      OEMBTMGR_LOCK();
      for (i = 0; i < ARR_SIZE(pMe->pendingRequest); i++)
      {
         if (AEEBT_BD_ADDRS_EQUAL (&pMe->pendingRequest[i].remoteAddr,
                                   pAddr) != FALSE)
         {
            pMe->pendingRequest[i].bValid = FALSE; // handled
            break;
         }
      }
      OEMBTMGR_UNLOCK();

      if (OEMBTMgr_Brew2CoreSvcId (pSvcId, &service_ident, 0) == AEE_SUCCESS)
      {
         bt_cmd_status_type stat;
         pMe->uFlags |= OEMBTMGR_FLAG_AUTH_CONN;
         stat = bt_cmd_rm_authorize_reply (pMe->appId, &service_ident,
                                           (bt_bd_addr_type*)pAddr, bAccept);
         result = OEMBT_CmdStatus2AEEResult (stat);
         if (AEE_SUCCESS != result)
         {
            pMe->uFlags &= ~OEMBTMGR_FLAG_AUTH_CONN;
         }
      }
   }
   return result;
}

int OEMBTMgr_TuneScanPerformance (
   OEMBTMgr* pMe,
   BTActVsPwr pageScanParam,
   BTActVsPwr inqScanParam
)
{
   int result = AEE_EBADPARM;
   DBGPRINTF ("Mgr_TuneScanPerformance - p=%d i=%d", 
              pageScanParam, inqScanParam);

   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe))
   {
      bt_cmd_status_type stat;
      pMe->uFlags |= OEMBTMGR_FLAG_PERF_TUNING;
      stat = bt_cmd_rm_config_application (
               pMe->appId, FALSE,
               OEMBTMgr_Brew2CoreScanParam (pageScanParam),
               OEMBTMgr_Brew2CoreScanParam (inqScanParam), TRUE);
      result = OEMBT_CmdStatus2AEEResult (stat);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~OEMBTMGR_FLAG_PERF_TUNING;
      }
      else
      {
         pMe->myInfo.dPgScanParam  = pageScanParam;
         pMe->myInfo.dInqScanParam = inqScanParam;
      }
   }
   return result;
}

int OEMBTMgr_RadioControl (OEMBTMgr* pMe, OEMBTMgrRadioControlCommand cmd)
{
   int result = AEE_EBADPARM;
   uint32 flags = 0;

   DBGPRINTF ("Mgr_RadioControl - cmd=%d", cmd);

   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe))
   {
      bt_cmd_status_type stat = BT_CS_GN_UNRECOGNIZED_CMD;
      switch (cmd)
      {
         case OEMBTMGR_SET_RADIO_OFF:
            flags = OEMBTMGR_FLAG_DISABLE_RADIO;
            pMe->uFlags |= flags;
            stat = bt_cmd_rm_disable_radio (pMe->appId, TRUE);
            break;
         case OEMBTMGR_SET_RADIO_ON:
            flags = OEMBTMGR_FLAG_ENABLE_RADIO;
            pMe->uFlags |= flags;
            stat = bt_cmd_rm_disable_radio (pMe->appId, FALSE);
            break;
         case OEMBTMGR_SET_RADIO_UNDISCOVERABLE:
            flags = OEMBTMGR_FLAG_DISABLE_VIS;
            pMe->uFlags |= flags;
            stat = bt_cmd_rm_disable_discoverability (pMe->appId, TRUE);
            break;
         case OEMBTMGR_RESTORE_DISCOVERABILITY:
            flags = OEMBTMGR_FLAG_RESTORE_VIS;
            pMe->uFlags |= flags;
            stat = bt_cmd_rm_disable_discoverability (pMe->appId, FALSE);
            break;
      }
      result = OEMBT_CmdStatus2AEEResult (stat);
      if (AEE_SUCCESS != result)
      {
         pMe->uFlags &= ~flags;
      }
   }
   return result;
}

int OEMBTMgr_GetRadioState (OEMBTMgr* pMe, BTRadioState* pRadioState)
{
   int result = AEE_EBADPARM;

   if ((NULL != pMe) && (pgTheOEMBTMgr == pMe) && (NULL != pRadioState))
   {
      bt_cmd_status_type     stat;
      bt_radio_activity_type radio_activity;

      stat = bt_cmd_rm_get_radio_activity( pMe->appId, &radio_activity );
      DBGPRINTF ("Mgr_GetRadioStatus - radio activity =%d", radio_activity);
      result = OEMBT_CmdStatus2AEEResult (stat);

      if ( result == AEE_SUCCESS )
      {
         *pRadioState = 0;
         if ( radio_activity & BT_RA_RADIO_OFF )
         {
            *pRadioState |= AEEBT_RADIO_OFF;
         }         
         if ( radio_activity & BT_RA_INQUIRING )
         {
            *pRadioState |= AEEBT_RADIO_INQUIRING;
         }
         if ( radio_activity & BT_RA_PAGING )
         {
            *pRadioState |= AEEBT_RADIO_PAGING;
         }
         if ( radio_activity & BT_RA_INQ_SCANNING )
         {
            *pRadioState |= AEEBT_RADIO_INQ_SCANNING;
         }
         if ( radio_activity & BT_RA_PAGE_SCANNING )
         {
            *pRadioState |= AEEBT_RADIO_PAGE_SCANNING;
         }
      }
      DBGPRINTF ("Mgr_GetRadioStatus - status=%d", *pRadioState);
   }

   return result;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */


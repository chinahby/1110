/*=============================================================================
FILE:  OEMOBEXSrv.c

SERVICES:  OEM implementation of OBEX Server

GENERAL DESCRIPTION:
      OEM implementation of OBEX Server

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMOBEXSrv.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
09/15/08   sp  Compiler warning fix

=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP)
#include "OEMOBEXDefs.h"
#include "OEMOBEXDefs_priv.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "mt_obex.h"
#include "OEMHeap.h"
#include "AEEStdErr.h"
#include "AEE_OEMHeap.h"
#include "AEEIOBEXHeader.h"

#ifdef BT_SIMULATION
#include "btmsg.h"
#include "bt_sim_interface.h"
#else
#include "msg.h"
#endif

//=============================================================================
// 
//
//=============================================================================
#define OEMOBEX_NO_CONN_ID 0xFFFF

#define RELEASE_HDR(x) if ((x) != NULL) {OEMOBEXHdr_Release((x));\
                                        (x) = NULL;}
#define ADDREF_HDR(x) if ((x) != NULL) { if (me->pOemSendHdr != NULL)\
                                         {\
                                             OEMOBEXHdr_Release(me->pOemSendHdr);}\
                                             me->pOemSendHdr = (x);\
                                             OEMOBEXHdr_AddRef((x));\
                                             pHdrList = OEMOBEXHdr_GetHeaderList((x));\
                                          }
//=============================================================================
// 
//
//=============================================================================

static void OEMOBEXSrv_EventCallback(bt_ev_msg_type* bt_ev_msg_ptr);
static OEMOBEXSrv* OEMOBEXSrv_FindMe(mt_obex_app_id_type appId);
static void OEMOBEXSrv_HandleConnInd(OEMOBEXSrv *me,
                                    mt_obex_ev_srv_con_ind_type *pConEvt);
static void OEMOBEXSrv_HandleDiscInd(OEMOBEXSrv *me,
                                     mt_obex_ev_srv_dcn_ind_type *pDcnEvt);
static void OEMOBEXSrv_HandleGetInd(OEMOBEXSrv *me,
                                    mt_obex_ev_srv_get_ind_type *pGetEvt);
static void OEMOBEXSrv_HandlePutInd(OEMOBEXSrv *me,
                                    mt_obex_ev_srv_put_ind_type *pPutEvt);
static void OEMOBEXSrv_HandleSetPathInd(OEMOBEXSrv *me,
                                mt_obex_ev_srv_set_path_ind_type *pSetPathEvt);
static void OEMOBEXSrv_HandleAuthInd(OEMOBEXSrv *me,
                                     bt_pf_ev_goep_srv_auth_req_ind_type *pAuthEvt);
static void OEMOBEXSrv_HandleCmdDone(OEMOBEXSrv  *me,
                                        bt_ev_gn_cmd_done_type  *pCmdDn);
static OEMOBEXHdr* OEMOBEXSrv_GetRecvHdr(OEMOBEXSrv *me);
static void OEMOBEXSrv_RecvHdrRelease(OEMOBEXSrv *me);

//=============================================================================
// 
//
//=============================================================================

struct OEMOBEXSrv{
   mt_obex_app_id_type   appId;
   boolean               bFullAccess;
   mt_obex_srv_auth_type auth;       // server auth type
   boolean               bCliAuth;   // client has req for auth
   boolean               bSrvAuth;   // server has requested for auth
   boolean               bUnAuthorized; // last value of bUnauthorised that came with conn req
   OEMOBEXAuthInfo       cliAuthReq; // auth info as that requested by client
   uint16                connId;     // conn id for the current auth sequence
   uint16                targetId;   // target id for the current auth sequence
   boolean               bConnected;
   uint8 aUsrId[OEMOBEX_MAX_UID_LEN];
   int                   nUidLen;
   mt_obex_status_type   status;

   void                  *cbData;    // data to be sent to aee when calling its cb
   PFNOBEXEVTCB          pAeeCb;     // aee cb

   uint8                 channel;    // scn of BT tranport
   bt_bd_addr_type       bdAddr;     // bd addr of remote device
   OEMOBEXHdr            *aRecvHdr[OEMOBEX_MAX_RECEIVE_BUFF]; // pool of receive buffers
   OEMOBEXHdr            *pOemSendHdr;
   struct OEMOBEXSrv     *pNextObject;
   uint32                uRef;
};
//=============================================================================
// 
//
//=============================================================================
// singly linked list of OBEXServers
typedef struct
{
   OEMOBEXSrv*   pHeadObject;
}  OEMOBEXSrvMgr;

static OEMOBEXSrvMgr        gOBEXSrvMgr;
static rex_crit_sect_type   gOBEXSrvCritSect; 

//=============================================================================
// 
//
//=============================================================================
int  OEMOBEXSrv_init(OEMOBEXSrv **pOemObj, void *cbData, PFNOBEXEVTCB eventCb)
{
   OEMOBEXSrv *me = NULL;

   MSG_DEBUG("OEM OBEX Srv init -> ",0, 0, 0);
   //allocate me
   me = OEM_Malloc(sizeof(OEMOBEXSrv));

   if (NULL == me)
   {
      MSG_ERROR("OEM OBEX Srv init Cannot alloc mem ",0, 0, 0);
      return AEE_ENOMEMORY;
   }

   MEMSET(me, 0, sizeof(OEMOBEXSrv));

   // try to allocate a receive header
   if (!OEMOBEXSrv_RecvHdrAvailable(me))
   {
      // no receive headers available
      OEM_Free(me);
      MSG_ERROR("OEM OBEX Srv init Cannot alloc recv header ",0, 0, 0);
      return AEE_EFAILED;
   }

   me->appId = bt_cmd_ec_get_app_id_and_register(OEMOBEXSrv_EventCallback);

   if (BT_APP_ID_NULL == me->appId)
   {
      OEMOBEXSrv_RecvHdrRelease(me);
      OEM_Free(me);
      MSG_ERROR("OEM OBEX Cannot reg app with BT ",0, 0, 0);
      return AEE_EFAILED;
   }

   me->cbData = cbData;
   me->pAeeCb = eventCb;
   me->connId = OEMOBEX_NO_CONN_ID;

   me->nUidLen = 0;
   me->bCliAuth = FALSE;
   me->bSrvAuth = FALSE;
   me->bUnAuthorized = FALSE;
   me->uRef          = 1;
   me->bConnected    = FALSE;

   rex_enter_crit_sect(&gOBEXSrvCritSect);

   me->pNextObject = gOBEXSrvMgr.pHeadObject;
   gOBEXSrvMgr.pHeadObject = me;

   rex_leave_crit_sect(&gOBEXSrvCritSect);
   //return the oem object
   *pOemObj = me;

   MSG_DEBUG("OEM OBEX Srv init <- ",0, 0, 0);
#ifdef BT_SIMULATION
   bt_sim_start(PROFILE_OBEX_SERVER, TRUE);
#endif 
   return SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
int  OEMOBEXSrv_Release(OEMOBEXSrv *me)
{
   OEMOBEXSrv **ppc = NULL;
   uint32      uRef = --me->uRef;

   MSG_DEBUG("OEM OBEX Srv release uRef %x -> ",uRef, 0, 0);

   if (0 == uRef)
   {
      if (BT_APP_ID_NULL != me->appId)
      {
         bt_cmd_ec_free_application_id(me->appId);
         me->appId = 0;
      }
      if (me->pOemSendHdr != NULL)
      {
         OEMOBEXHdr_Release(me->pOemSendHdr);
         me->pOemSendHdr = NULL;
      }
      rex_enter_crit_sect(&gOBEXSrvCritSect);

      for (ppc = &gOBEXSrvMgr.pHeadObject; *ppc != NULL; ppc = &(*ppc)->pNextObject)
      {
         if ((*ppc)->appId == me->appId)
         {
            *ppc = me->pNextObject;
            me->pNextObject = NULL; 
            break;
         }
      }
      rex_leave_crit_sect(&gOBEXSrvCritSect);

      OEMOBEXSrv_RecvHdrRelease(me);
      OEM_Free(me);
   }
   MSG_DEBUG("OEM OBEX Srv release uRef %x <- ",uRef, 0, 0);
   return AEE_SUCCESS;
}
//=============================================================================
// 
// 
//=============================================================================
int OEMOBEXSrv_Register(OEMOBEXSrv *me, const OEMOBEX_TransportConfigInfo *pRegInfo)
{
   int ret                     = AEE_SUCCESS;
   OEMOBEXEvent                  eventData;
   mt_obex_status_type           status;
   mt_obex_tp_config_struct_type tpParams;
   mt_obex_byte_seq_type         target;
   mt_obex_srv_auth_type         auth;

   MSG_DEBUG("OEM OBEX Srv register -> ",0,0,0);

   MEMSET((void*)&tpParams, 0, sizeof(tpParams));

   auth = OEMOBEX_AeeToMtObexAuth(pRegInfo->auth);

   target.len  = pRegInfo->nTargetLen;
   target.data = pRegInfo->pTargetId;
   //create the mt-obex params that are needed for registration
   switch (pRegInfo->transport)
   {
      case OEMOBEX_TRANS_BT_RFCOMM:
      {
         mt_obex_bt_config_struct_type  *pbtConfig = &tpParams.bt_config;
         
         tpParams.transports = OEMOBEX_TRANS_BT_RFCOMM; // #check
         pbtConfig->security     =
            OEMOBEX_OEMToMtSecurity(pRegInfo->transportConfig.btConfig.security);
         pbtConfig->authRequired = 1;
         //#check there should be a way to not specify the uuid
         // instead, the oem layer should create a sd object and do the registration
        
         tpParams.bt_config.uuid = (mt_obex_sd_uuid_type)pRegInfo->transportConfig.btConfig.uuid; //#check
        
         break;
      }
      default:
      return AEE_EUNSUPPORTED;
   }
   // this call is not enqued to the cmd queue
   status = mt_obex_srv_register(me->appId, &target, auth, &tpParams, &me->channel);

   //#check sdp registration
   ret = OEMOBEX_CmdStatusToAeeResult(status);

   MSG_HIGH("OEM OBEX Srv Reg compl stat %x, scn %x ",ret, me->channel, 0);
   if (AEE_SUCCESS == ret)
   {
      MEMSET((void*)&eventData, 0, sizeof(OEMOBEXEvent));
      eventData.event  = AEEOBEX_EVT_SRV_REG_CMPL;
      eventData.u.srv.status = ret;

      me->pAeeCb(me->cbData, &eventData, NULL, NULL);
   }

   MSG_MED("OEM OBEX Srv register <-",0,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
int OEMOBEXSrv_DeRegister(OEMOBEXSrv *me)
{
   mt_obex_status_type status;
   int ret = AEE_SUCCESS;
   OEMOBEXEvent eventData;

   MSG_DEBUG("OEM OBEX Srv de register ->",0,0,0);

   // this is not enqueued as a command
   status = mt_obex_srv_deregister_server(me->appId, (uint8)me->channel);

   ret =  OEMOBEX_CmdStatusToAeeResult(status);

   if (AEE_SUCCESS == ret)
   {
      MEMSET((void*)&eventData, 0, sizeof(OEMOBEXEvent));
      eventData.event  = AEEOBEX_EVT_SRV_DEREG_CMPL;
      eventData.u.srv.status = AEE_SUCCESS;

      me->pAeeCb(me->cbData, &eventData, NULL, NULL);
   }

   MSG_MED("OEM OBEX Srv de register ret %x <-",ret,0,0);

   return ret;
}

//=============================================================================
// 
//
//=============================================================================

int OEMOBEXSrv_ConnectResp(OEMOBEXSrv  *me,
                           uint16      connId,
                           OEMOBEXHdr  *pOemHdr,
                           uint32      cmdStatus)
{
   boolean bAccept = (cmdStatus == AEE_SUCCESS) ? TRUE : FALSE;
   mt_obex_hdr_list_type *pHdrList = NULL;
   mt_obex_status_type status = OEMOBEX_AEEToMtStatus(cmdStatus);
   int ret = AEE_SUCCESS;

   MSG_DEBUG("OEM OBEX Srv CONN resp conn id %x stat %x ->",connId,cmdStatus,0);

   rex_enter_crit_sect(&gOBEXSrvCritSect);

   ADDREF_HDR(pOemHdr);

   if ((TRUE == me->bCliAuth) && 
      (FALSE == me->bSrvAuth) &&
      (FALSE == me->bUnAuthorized))
   {
      // this is the case of client initiated authentication
      // and no server authentication
      if (AEEOBEX_STAT_SUCCESS == cmdStatus)
      {
         // server has accepted the connection
         // send the auth chalenge evt to app
         OEMOBEXAuthInfo oemAuthChal;
         OEMOBEXEvent eventData;

         MEMSET((void*)&eventData, 0 ,sizeof(OEMOBEXEvent));
         MEMSET((void*)&oemAuthChal, 0 ,sizeof(OEMOBEXAuthInfo));

         eventData.event   = AEEOBEX_EVT_AUTH_CHALLENGE;
         eventData.u.srv.status  = AEE_SUCCESS;
         eventData.u.srv.pOemHdr = NULL;
         eventData.u.srv.bFinal = TRUE;
         eventData.u.srv.connId = me->connId;
         eventData.u.srv.targetId = me->targetId;
         

         oemAuthChal = me->cliAuthReq;

         me->status = status;
         // clear the auth info
         MEMSET((void*)&me->cliAuthReq, 0, sizeof(OEMOBEXAuthInfo));
   
         me->pAeeCb(me->cbData,&eventData, &oemAuthChal, NULL);
      }
      else
      {
         // server has rejected the request
         me->bCliAuth = FALSE;
         me->bSrvAuth = FALSE;
         me->bUnAuthorized = FALSE;
         
         status = mt_obex_srv_accept_connect(me->appId, connId,
                                             FALSE, pHdrList, 
                                             status);
      }
   }
   else if (((FALSE == me->bCliAuth) && 
         (FALSE == me->bSrvAuth) &&
         (FALSE == me->bUnAuthorized)) ||
         // server authentication 
         ((FALSE == me->bCliAuth) && 
         (TRUE == me->bSrvAuth) &&
         (FALSE == me->bUnAuthorized)) ||
         // server and client auth
         ((TRUE == me->bCliAuth) && 
         (TRUE == me->bSrvAuth) &&
         (FALSE == me->bUnAuthorized)))
   {
      me->bCliAuth = FALSE;
      me->bSrvAuth = FALSE;
      me->bUnAuthorized = FALSE;

      status = mt_obex_srv_accept_connect(me->appId, connId,
                                      bAccept, pHdrList, 
                                      status);
      ret =  OEMOBEX_CmdStatusToAeeResult(status);

      if (AEE_SUCCESS != ret)
      {
         RELEASE_HDR(pOemHdr);
      }
   }
   else 
   {
      MSG_ERROR("OEM OBEX Srv conn resp bad state ",ret,0,0);
      ret = AEE_EFAILED;
   }

   rex_leave_crit_sect(&gOBEXSrvCritSect);

   MSG_DEBUG("OEM OBEX Srv conn resp ret <-",ret,0,0);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================

int OEMOBEXSrv_PutResp(OEMOBEXSrv *me,
                       uint16     connId,
                       OEMOBEXHdr *pOemHdr,
                       uint32     cmdStatus)
{
   mt_obex_hdr_list_type *pHdrList = NULL;
   mt_obex_status_type status = OEMOBEX_AEEToMtStatus(cmdStatus);
   int ret = AEE_SUCCESS;

   MSG_DEBUG("OEM OBEX Srv PUT resp conn id %x stat %x ->",connId,cmdStatus,0);

   rex_enter_crit_sect(&gOBEXSrvCritSect);

   ADDREF_HDR(pOemHdr);

   status = mt_obex_srv_put_response(me->appId, connId, pHdrList, status);

   ret = OEMOBEX_CmdStatusToAeeResult(status);

   if (AEE_SUCCESS != ret)
   {
      RELEASE_HDR(pOemHdr);
   }
   rex_leave_crit_sect(&gOBEXSrvCritSect);

   MSG_DEBUG("OEM OBEX Srv PUT resp ret %x <-",ret,0,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================

int OEMOBEXSrv_GetResp(OEMOBEXSrv *me,
                       uint16     connId,
                       OEMOBEXHdr *pOemHdr,
                       uint32     cmdStatus)
{
   mt_obex_hdr_list_type *pHdrList = NULL;
   mt_obex_status_type status = OEMOBEX_AEEToMtStatus(cmdStatus);
   int ret = AEE_SUCCESS;

   MSG_DEBUG("OEM OBEX Srv GET resp conn id %x stat %x ->",connId,cmdStatus,0);

   rex_enter_crit_sect(&gOBEXSrvCritSect);
   ADDREF_HDR(pOemHdr);

   status = mt_obex_srv_get_response(me->appId, connId, pHdrList, status);
   ret = OEMOBEX_CmdStatusToAeeResult(status);

   if (AEE_SUCCESS != ret)
   {
      RELEASE_HDR(pOemHdr);
   }
   rex_leave_crit_sect(&gOBEXSrvCritSect);
   MSG_DEBUG("OEM OBEX Srv GET resp ret %x <-",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
int OEMOBEXSrv_SetPathResp(OEMOBEXSrv *me,
                           uint16     connId,
                           OEMOBEXHdr *pOemHdr,
                           uint32     cmdStatus)
{
   mt_obex_hdr_list_type *pHdrList = NULL;
   mt_obex_status_type status = OEMOBEX_AEEToMtStatus(cmdStatus);
   int ret = AEE_SUCCESS;

   MSG_DEBUG("OEM OBEX Srv SETPATH resp conn id %x stat %x ->",connId,cmdStatus,0);

   rex_enter_crit_sect(&gOBEXSrvCritSect);

   ADDREF_HDR(pOemHdr);

   status = mt_obex_srv_confirm_set_path(me->appId, connId, pHdrList, status);
   ret = OEMOBEX_CmdStatusToAeeResult(status);

   if (AEE_SUCCESS != ret)
   {
      RELEASE_HDR(pOemHdr);
   }
   rex_leave_crit_sect(&gOBEXSrvCritSect);
   MSG_DEBUG("OEM OBEX Srv SETPATH resp ret %x <-",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================

int OEMOBEXSrv_DiscResp(OEMOBEXSrv *me,
                        uint16     connId,
                        OEMOBEXHdr *pOemHdr,
                        uint32     cmdStatus)
{
   mt_obex_hdr_list_type *pHdrList = NULL;
   mt_obex_status_type status = OEMOBEX_AEEToMtStatus(cmdStatus);
   int ret = AEE_SUCCESS;

   MSG_DEBUG("OEM OBEX Srv DISC resp conn id %x stat %x ->",connId,cmdStatus,0);

   rex_enter_crit_sect(&gOBEXSrvCritSect);
   ADDREF_HDR(pOemHdr);

   status = mt_obex_srv_disconnect_response(me->appId, connId, pHdrList, status);

   ret =  OEMOBEX_CmdStatusToAeeResult(status);

   if (AEE_SUCCESS != ret)
   {
      RELEASE_HDR(pOemHdr);
   }
   rex_leave_crit_sect(&gOBEXSrvCritSect);
   MSG_DEBUG("OEM OBEX Srv DISC resp ret %x <-",ret,0,0);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================

int OEMOBEXSrv_Disconnect(OEMOBEXSrv *me,
                          uint16     connId)
{
   mt_obex_status_type status; 
   int ret = AEE_SUCCESS;

   MSG_DEBUG("OEM OBEX Srv DISCONNECT conn id %x ->",connId,0,0);

   status = mt_obex_srv_force_disconnect(me->appId, connId);

   ret = OEMOBEX_CmdStatusToAeeResult(status);

   MSG_DEBUG("OEM OBEX Srv DISCONNECT ret %x <-",ret,0,0);

   return ret;
}

//=============================================================================
// 
//
//=============================================================================

int OEMOBEXSrv_Authenticate(OEMOBEXSrv      *me,
                            OEMOBEXAuthInfo *pAuthInfo,
                            uint16          connId)
{
   mt_obex_tp_conn_struct_type connStruct;
   mt_realm_info mtRealm;
   mt_obex_status_type status;
   int len = 0;
   int ret = AEE_SUCCESS;


   MSG_DEBUG("OEM OBEX Srv authenticate conn id %x ->",connId,0,0);

   rex_enter_crit_sect(&gOBEXSrvCritSect);

   if (me->bConnected)
   {
      rex_leave_crit_sect(&gOBEXSrvCritSect);
    // we allow authentication only during connection
     MSG_ERROR("OEM OBEX Srv connected ;auth not allowed ",0,0,0);
     return AEE_EBADSTATE;
   }

   me->bSrvAuth = TRUE;
   connStruct.transport_conn.bt_conn.bd_addr_ptr =
      (bt_bd_addr_type*)&me->bdAddr;

   connStruct.transport_conn.bt_conn.channel = 
          me->channel;

   len = (pAuthInfo->nRealmLen < MT_OBEX_MAX_REALM_LEN) ?
          pAuthInfo->nRealmLen :
          MT_OBEX_MAX_REALM_LEN;

   mtRealm.len          = len;
   mtRealm.realmStrType = (uint8)pAuthInfo->charSet;

   if (0 < len)
   {
      MEMCPY((void*)&mtRealm.realmStr[0],(void*)&pAuthInfo->aRealm[0],len); //#check wstr?
   }
   me->bFullAccess = pAuthInfo->bFullAccess;
   status = mt_obex_server_authenticate(&connStruct,
                                       me->connId,
                                       &mtRealm,
                                       pAuthInfo->bUserIdReq,
                                       pAuthInfo->bFullAccess);

   ret = OEMOBEX_CmdStatusToAeeResult(status);

   if (AEE_SUCCESS != ret)
   {
      me->bSrvAuth = FALSE;
   }
   rex_leave_crit_sect(&gOBEXSrvCritSect);

   MSG_DEBUG("OEM OBEX Srv authenticate ret %x <-",ret,0,0);

   return ret;
}


//=============================================================================
// 
//
//=============================================================================
int OEMOBEXSrv_AuthResponse(OEMOBEXSrv  *me,
                            const uint8 *pUserID,
                            int         uidLen,
                            const uint8 *pPassword,
                            int         pswdLen)
{
   char aPswd[BT_PF_OBEX_MAX_PASSWORD_LEN];
   mt_obex_status_type mtCmdStatus;
   boolean bReadOnly = !me->bFullAccess;
   int ret = AEE_SUCCESS;

   MSG_DEBUG("OEM OBEX Srv auth resp uidLen %x  pswdLen %x->",uidLen,pswdLen,0);

   MSG_HIGH("OEM OBEX Srv auth resp ignoring uid ",0,0,0);

   if (pswdLen >= BT_PF_OBEX_MAX_PASSWORD_LEN) // we include the null terminator as well
   {
      MSG_ERROR("OEM OBEX Srv auth pswd too long pswdLen %x",pswdLen,0,0);
      return AEE_EBADPARM;
   }

   MEMSET((void*)&aPswd[0], 0, BT_PF_OBEX_MAX_PASSWORD_LEN);

   if (pPassword != NULL)
   {
      MEMCPY((void*)&aPswd[0], (void*)pPassword, pswdLen);
   }
   aPswd[BT_PF_OBEX_MAX_PASSWORD_LEN - 1] = 0;
   mtCmdStatus = mt_obex_srv_auth_response(me->appId,
                                           me->connId,
                                          (char*)&aPswd[0],
                                           bReadOnly);

   ret = OEMOBEX_CmdStatusToAeeResult(mtCmdStatus);

   MSG_DEBUG("OEM OBEX Srv Auth resp ret %x <- ",ret,0,0);

   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXSrv_EventCallback(bt_ev_msg_type* ev_msg_ptr)
{
   OEMOBEXSrv *me = OEMOBEXSrv_FindMe(ev_msg_ptr->ev_hdr.bt_app_id);


   MSG_HIGH("OEM OBEX Srv Evt cb ->",0,0,0);
      

   if (NULL == me)
   {
      MSG_ERROR("OEM OBEX Srv Evt cb no matching app id %x",
                 ev_msg_ptr->ev_hdr.bt_app_id,0,0);
      return; //no matching appid found. what happened here?
   }

   rex_enter_crit_sect(&gOBEXSrvCritSect);
   switch ( ev_msg_ptr->ev_hdr.ev_type )
   {
      case  BT_EV_GN_CMD_DONE:
      {
         OEMOBEXSrv_HandleCmdDone(me, &ev_msg_ptr->ev_msg.ev_gn_cmd_done);
         break;
      } // end of case BT_EV_GN_CMD_DONE
      case MT_OBEX_EV_SRV_CON_IND:
      {
         OEMOBEXSrv_HandleConnInd(me, &ev_msg_ptr->ev_msg.ev_goep_srv_con_ind);
         RELEASE_HDR(me->pOemSendHdr);
         break;
      }
      case MT_OBEX_EV_SRV_AUTH_IND:
      {
         OEMOBEXSrv_HandleAuthInd(me, &ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind);
         break;
      }
      case  MT_OBEX_EV_SRV_DCN_IND:
      {
         OEMOBEXSrv_HandleDiscInd(me, &ev_msg_ptr->ev_msg.ev_goep_srv_dcn_ind);
         RELEASE_HDR(me->pOemSendHdr);
         break;
      }
      case  MT_OBEX_EV_SRV_GET_IND:
      {
         OEMOBEXSrv_HandleGetInd(me, &ev_msg_ptr->ev_msg.ev_goep_srv_get_ind);
         RELEASE_HDR(me->pOemSendHdr);
         break;
      }
      case  MT_OBEX_EV_SRV_PUT_IND:
      {
         OEMOBEXSrv_HandlePutInd(me, &ev_msg_ptr->ev_msg.ev_goep_srv_put_ind);
         RELEASE_HDR(me->pOemSendHdr);
         break;
      }
      case  MT_OBEX_EV_SRV_SET_PATH_IND:
      {
         OEMOBEXSrv_HandleSetPathInd(me,
            &ev_msg_ptr->ev_msg.ev_goep_srv_set_path_ind);
         RELEASE_HDR(me->pOemSendHdr);
         break;
      }
      default:
         break;
   }

   rex_leave_crit_sect(&gOBEXSrvCritSect);
   MSG_DEBUG("OEM OBEX Srv Evt cb <-",0,0,0);
   return;
}

//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXSrv_HandleAuthInd(OEMOBEXSrv *me,
                                     bt_pf_ev_goep_srv_auth_req_ind_type *pAuthEvt)
{
    int len = 0;
    OEMOBEXEvent     eventData;
    
    MSG_HIGH("OEM OBEX Srv Auth Req ind: %x", pAuthEvt->conn_id, 0,0);

    MEMSET((void*)&eventData, 0, sizeof(eventData));

    rex_enter_crit_sect(&gOBEXSrvCritSect);
    // save the conn id
    me->connId = pAuthEvt->conn_id;
    me->channel = pAuthEvt->ch_num;

    // save the client auth parameters
    // which will be sent to the aee
    // layer with the auth chalenge event
    me->cliAuthReq.bUserIdReq  = pAuthEvt->user_id_rqd;
    me->cliAuthReq.bFullAccess = pAuthEvt->full_access;
    me->cliAuthReq.charSet     = pAuthEvt->realm_str_type;
    
    // cli has initiated authentication
    me->bCliAuth = TRUE;   
    me->bSrvAuth = FALSE;
    me->bUnAuthorized = FALSE;

    rex_leave_crit_sect(&gOBEXSrvCritSect);

    len = (pAuthEvt->realm_len < OEMOBEX_MAX_REALM_LEN) ?
           pAuthEvt->realm_len :
           OEMOBEX_MAX_REALM_LEN;

    MEMSET((void*)&me->cliAuthReq.aRealm[0], 0, sizeof(me->cliAuthReq.aRealm));

    if (len > 0)
    {
       MEMCPY((void*)&me->cliAuthReq.aRealm[0],(char*)pAuthEvt->realm_str,
                 len);
    }
    mt_obex_srv_get_client_addr(me->appId,
                                pAuthEvt->conn_id,
                                (bt_bd_addr_type* )&me->bdAddr);
   eventData.event    = AEEOBEX_EVT_SRV_CONN_REQ;
   eventData.u.srv.connId   = pAuthEvt->conn_id;
   eventData.u.srv.targetId = pAuthEvt->target_id; //no headers with this event ? #check

   me->pAeeCb(me->cbData, &eventData, NULL, NULL);

}
//=============================================================================
// 
//
//=============================================================================

static void OEMOBEXSrv_HandleConnInd(OEMOBEXSrv *me,
                                    mt_obex_ev_srv_con_ind_type *pConEvt)
{
   OEMOBEXEvent     eventData;

   MSG_MED("OEM OBEX Srv con ind: conn id %x", pConEvt->conn_id, 0,0);

   MEMSET((void*)&eventData, 0, sizeof(eventData));

   rex_enter_crit_sect(&gOBEXSrvCritSect);
   // save the curr con id and target id
   me->connId = pConEvt->conn_id;
   me->targetId = pConEvt->target_id;

   mt_obex_srv_get_client_addr(me->appId,
                               pConEvt->conn_id,
                               (bt_bd_addr_type* )&me->bdAddr);


   if ((FALSE == me->bCliAuth) &&
       (FALSE == me->bSrvAuth) &&
       (FALSE == me->bUnAuthorized))
   {
      // cli conn req without auth 
      eventData.event    = AEEOBEX_EVT_SRV_CONN_REQ;
      eventData.u.srv.connId   = pConEvt->conn_id;
      eventData.u.srv.targetId = pConEvt->target_id;

      if((pConEvt->cmd_headers_cnt != 0) &&
          (pConEvt->cmd_headers_ptr != NULL))
      {
         MSG_HIGH("OEM OBEX Srv con ind hdr cnt %x",
                         pConEvt->cmd_headers_cnt, 0,0);
         eventData.u.srv.pOemHdr = OEMOBEXSrv_GetRecvHdr(me);
         OEMOBEXHdr_MtToAeeHeaders(pConEvt->cmd_headers_cnt,
                                   pConEvt->cmd_headers_ptr,
                                   eventData.u.srv.pOemHdr,
                                   FALSE,
                                   me->appId);
      }
      me->pAeeCb(me->cbData, &eventData, NULL, NULL);
   }
   else if ((TRUE == me->bSrvAuth) &&
            (FALSE == me->bUnAuthorized) &&
            (TRUE == pConEvt->unauthorized))
   {
      // server had requested for auth and we got the AR header 
      OEMOBEXAuthResp oemAuthResp;

      MEMSET((void*)&oemAuthResp, 0 ,sizeof(OEMOBEXAuthResp));

      me->bUnAuthorized = TRUE;
      eventData.event    = AEEOBEX_EVT_AUTH_RESPONSE;
      eventData.u.srv.connId   = pConEvt->conn_id;
      eventData.u.srv.targetId = pConEvt->target_id;

      //oemAuthResp.bFullAccess = TRUE; //#checknot returned by obex; why do we need this


      if (pConEvt->user_id_len > 0)
      {
         int len = 0;
         int i = 0;
         len = (pConEvt->user_id_len >
                OEMOBEX_MAX_UID_LEN) ? 
                OEMOBEX_MAX_UID_LEN :
                pConEvt->user_id_len;

         for (i = 0; i < len; i++)
         {
            oemAuthResp.aUserId[i] = pConEvt->user_id[i];
         }
         oemAuthResp.nUidLen = len;
      }
      me->pAeeCb(me->cbData, &eventData, NULL, &oemAuthResp);
   }
   else if ((TRUE == me->bSrvAuth) &&
            (TRUE == me->bUnAuthorized) &&
            (FALSE == pConEvt->unauthorized))
   {
      OEMOBEXAuthResp oemAuthResp;
      MEMSET((void*)&oemAuthResp, 0 ,sizeof(OEMOBEXAuthResp));

      // authentication complete for server 
      me->bUnAuthorized = FALSE;
      eventData.event    = AEEOBEX_EVT_AUTH_COMPL;
      eventData.u.srv.connId   = pConEvt->conn_id;
      eventData.u.srv.targetId = pConEvt->target_id;
      eventData.u.srv.status = AEE_SUCCESS;
      // in case of failure, disc indi cb is called
      me->pAeeCb(me->cbData, &eventData, NULL, &oemAuthResp);
   }
   else if ((TRUE == me->bCliAuth) &&
            (FALSE == me->bSrvAuth) &&
            (FALSE == me->bUnAuthorized) &&
            (FALSE  == pConEvt->unauthorized))
   {
      mt_obex_hdr_list_type *pHdrList = NULL;
      int ret = AEE_SUCCESS;
      mt_obex_status_type status;

      // client authentication no server authentication
      MSG_DEBUG("OEM OBEX Srv CONN resp conn,cli auth con id %x stat %x ->",me->connId,me->status,0);

      me->bCliAuth = FALSE;
      status = mt_obex_srv_accept_connect(me->appId, me->connId,
                                          TRUE, pHdrList, 
                                          me->status);// should always be success if we reach here?
      ret =  OEMOBEX_CmdStatusToAeeResult(status);

      if (AEE_SUCCESS != ret)
      {
         MSG_ERROR("OEM OBEX Srv CONN resp conn failed ret %x ->",ret,0,0);
         // #check should evt error be sent to app?
         RELEASE_HDR(me->pOemSendHdr);
      }
   }
   rex_leave_crit_sect(&gOBEXSrvCritSect);
   return;
}
//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXSrv_HandleDiscInd(OEMOBEXSrv *me,
                                     mt_obex_ev_srv_dcn_ind_type *pDcnEvt)
{
   OEMOBEXEvent eventData;
      
   MSG_HIGH("OEM OBEX Srv DISC Ind,conn id %x", pDcnEvt->conn_id,0, 0);

   MEMSET((void*)&eventData, 0 ,sizeof(eventData));
   eventData.event    = AEEOBEX_EVT_SRV_DISC_REQ;
   eventData.u.srv.flags    = 0;
   eventData.u.srv.status   = AEE_SUCCESS;
   eventData.u.srv.connId   = pDcnEvt->conn_id;
   eventData.u.srv.targetId = pDcnEvt->target_id;
   eventData.u.srv.pOemHdr  = NULL;

   rex_enter_crit_sect(&gOBEXSrvCritSect);
   me->bCliAuth = FALSE;
   me->bSrvAuth = FALSE;
   me->bUnAuthorized = FALSE;

   if((pDcnEvt->cmd_headers_cnt != 0) &&
      (pDcnEvt->cmd_headers_ptr != NULL))
   {
      eventData.u.srv.pOemHdr = OEMOBEXSrv_GetRecvHdr(me);
      OEMOBEXHdr_MtToAeeHeaders(pDcnEvt->cmd_headers_cnt,
                                pDcnEvt->cmd_headers_ptr,
                                eventData.u.srv.pOemHdr,
                                FALSE,
                                me->appId);
   }

   // call the aee callback with this event
   me->pAeeCb(me->cbData,
              &eventData,
              NULL,
              NULL);

   rex_leave_crit_sect(&gOBEXSrvCritSect);
   return;
}


//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXSrv_HandleGetInd(OEMOBEXSrv *me,
                                    mt_obex_ev_srv_get_ind_type *pGetEvt)
{
   OEMOBEXEvent eventData;
   MSG_HIGH("OEM OBEX Srv GET Ind, id=%x,status=%x", pGetEvt->conn_id,
             pGetEvt->status, 0);

   MEMSET((void*)&eventData, 0 ,sizeof(eventData));

    if ((BT_CS_GN_SUCCESS != pGetEvt->status) &&
        (BT_CS_PF_OBEX_CONTINUE != pGetEvt->status) && 
        (BT_CS_PF_OBEXSRV_INCOMPLETE_GET != pGetEvt->status))
   {
      eventData.event    = AEEOBEX_EVT_SRV_ABORTED;
   }
   else
   {
      eventData.event    = AEEOBEX_EVT_SRV_GET_REQ;
   }
   eventData.u.srv.flags    = 0;
   eventData.u.srv.status   = 
                OEMOBEX_MtToAeeStatus((mt_obex_status_type)pGetEvt->status);
   eventData.u.srv.connId   = pGetEvt->conn_id;
   eventData.u.srv.targetId = pGetEvt->target_id;
   eventData.u.srv.bFinal   = (BT_CS_PF_OBEX_CONTINUE == pGetEvt->status) ?
                               TRUE:
                               FALSE;

   if((pGetEvt->cmd_headers_cnt != 0) &&
      (pGetEvt->cmd_headers_ptr != NULL))
   {
      MSG_DEBUG("OEM OBEX Srv hdr cnt %x", pGetEvt->cmd_headers_cnt,0, 0);

      eventData.u.srv.pOemHdr = OEMOBEXSrv_GetRecvHdr(me);
      OEMOBEXHdr_MtToAeeHeaders(pGetEvt->cmd_headers_cnt,
                                pGetEvt->cmd_headers_ptr,
                                eventData.u.srv.pOemHdr,
                                FALSE,
                                me->appId);
   }

   // call the aee callback with this event
   me->pAeeCb(me->cbData,
              &eventData,
              NULL,
              NULL);

   return;
}

//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXSrv_HandlePutInd(OEMOBEXSrv *me,
                                    mt_obex_ev_srv_put_ind_type *pPutEvt)
{

   OEMOBEXEvent eventData;
   MSG_HIGH("OEM OBEX Srv PUT Ind, id=%x,status=%x", pPutEvt->conn_id,
             pPutEvt->status, 0);

   MEMSET((void*)&eventData, 0 ,sizeof(eventData));

   if ((BT_CS_GN_SUCCESS != pPutEvt->status) &&
       (BT_CS_PF_OBEX_CONTINUE != pPutEvt->status))
   {
      eventData.event    = AEEOBEX_EVT_SRV_ABORTED;
   }
   else
   {
      eventData.event    = AEEOBEX_EVT_SRV_PUT_REQ;
   }
   eventData.u.srv.flags    = 0;
   eventData.u.srv.status   = 
                OEMOBEX_MtToAeeStatus((mt_obex_status_type)pPutEvt->status);
   eventData.u.srv.connId   = pPutEvt->conn_id;
   eventData.u.srv.targetId = pPutEvt->target_id;
   eventData.u.srv.bFinal   = (BT_CS_PF_OBEX_CONTINUE == pPutEvt->status) ?
                               FALSE:
                               TRUE;

   if((pPutEvt->cmd_headers_cnt != 0) &&
      (pPutEvt->cmd_headers_ptr != NULL))
   {
      MSG_DEBUG("OEM OBEX Srv hdr cnt %x", pPutEvt->cmd_headers_cnt,0, 0);
      eventData.u.srv.pOemHdr = OEMOBEXSrv_GetRecvHdr(me);
      OEMOBEXHdr_MtToAeeHeaders(pPutEvt->cmd_headers_cnt,
                                pPutEvt->cmd_headers_ptr,
                                eventData.u.srv.pOemHdr,
                                FALSE,
                                me->appId);
   }

   // call the aee callback with this event
   me->pAeeCb(me->cbData,
              &eventData,
              NULL,
              NULL);
   return;
}
//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXSrv_HandleSetPathInd(OEMOBEXSrv *me,
                                mt_obex_ev_srv_set_path_ind_type *pSetPathEvt)
{
   OEMOBEXEvent eventData;

   MSG_HIGH("OEM OBEX Evt Handler S SetPath Ind, id=%x",
             pSetPathEvt->conn_id,0, 0);

   MEMSET((void*)&eventData, 0 ,sizeof(eventData));
   eventData.event    = AEEOBEX_EVT_SRV_SETPATH_REQ;
   eventData.u.srv.flags    = 0;
   eventData.u.srv.status   = AEE_SUCCESS;
   eventData.u.srv.connId   = pSetPathEvt->conn_id;
   eventData.u.srv.targetId = pSetPathEvt->target_id;

   eventData.u.srv.flags = (pSetPathEvt->create) ?
      (eventData.u.srv.flags & ~AEEOBEX_FLAG_DONT_CREATE_DIR) :
      (eventData.u.srv.flags | AEEOBEX_FLAG_DONT_CREATE_DIR); //#check
   eventData.u.srv.flags = (pSetPathEvt->up_level)?
      (eventData.u.srv.flags | AEEOBEX_FLAG_BACKUP) :
      (eventData.u.srv.flags);

   if(((pSetPathEvt->cmd_headers_cnt != 0) &&
      (pSetPathEvt->cmd_headers_ptr != NULL)) || 
      (NULL != pSetPathEvt->folder_ptr))
   {
      eventData.u.srv.pOemHdr = OEMOBEXSrv_GetRecvHdr(me);
      OEMOBEXHdr_MtToAeeHeaders(pSetPathEvt->cmd_headers_cnt,
                                pSetPathEvt->cmd_headers_ptr,
                                eventData.u.srv.pOemHdr,
                                FALSE,
                                me->appId);
   }

   if (NULL != pSetPathEvt->folder_ptr)
   {
      int len = WSTRLEN(pSetPathEvt->folder_ptr) + 1;

      if (eventData.u.srv.pOemHdr)
      {
          OEMOBEXHdr_AddString(eventData.u.srv.pOemHdr,
                               AEEOBEX_HDR_NAME,
                               pSetPathEvt->folder_ptr,
                               len);
      }
      else
      {
         MSG_ERROR("OEM OBEX Srv Evt Handler S SetPath Dropping name",
             0, 0, 0);
      }
   }

   // call the aee callback with this event
   me->pAeeCb(me->cbData,
              &eventData,
              NULL,
              NULL);

   return;
}

//=============================================================================
// 
//
//=============================================================================
static OEMOBEXSrv* OEMOBEXSrv_FindMe(mt_obex_app_id_type appId)
{
   OEMOBEXSrv *me = NULL;

   rex_enter_crit_sect(&gOBEXSrvCritSect);

   for (me = gOBEXSrvMgr.pHeadObject; me != NULL; me = me->pNextObject)
   {
      if (me->appId == appId)
      {
         break;
      }
   }
   rex_leave_crit_sect(&gOBEXSrvCritSect);
   return me;
}
//=============================================================================
// 
//
//=============================================================================

static void OEMOBEXSrv_HandleCmdDone(OEMOBEXSrv  *me,
                                     bt_ev_gn_cmd_done_type  *pCmdDn)
{
   OEMOBEXEvent eventData;
   MSG_HIGH("OEM OBEX srv cmd done evt - status=%x, cmd=%x ->", pCmdDn->cmd_status, pCmdDn->cmd_type, 0);

   MEMSET((void*)&eventData, 0 ,sizeof(eventData));
   eventData.u.srv.connId   = me->connId;
   eventData.u.srv.targetId = me->targetId;
   eventData.u.srv.flags    = 0;
   eventData.u.srv.status   = 
                OEMOBEX_MtToAeeStatus((mt_obex_status_type)pCmdDn->cmd_status);
   eventData.u.srv.pOemHdr  = NULL;
   eventData.u.srv.bFinal   = 0;

   switch (pCmdDn->cmd_type )
   {
      case BT_PF_CMD_GOEP_SRV_ACCEPT:
      {
         MSG_HIGH("OEM OBEX srv cmd dne accept conn ",0,0,0);
         eventData.event    = AEEOBEX_EVT_SRV_CONN_COMPL;
         RELEASE_HDR(me->pOemSendHdr);
         break;
      }
      case BT_PF_CMD_GOEP_SRV_FORCE_DCN:
      {
         if ((pCmdDn->cmd_status != BT_CS_GN_SUCCESS) &&
             (pCmdDn->cmd_status != BT_CS_GN_PENDING))
         {   
            MSG_HIGH("OEM OBEX srv cmd dne force disc failed",0,0,0);
            eventData.event    = OEMOBEX_EVT_SRV_DISC_COMPL;
            RELEASE_HDR(me->pOemSendHdr);
         }
         break;
      }
      case BT_PF_CMD_GOEP_SRV_DCN_RSP:
      {
         MSG_HIGH("OEM OBEX srv cmd dne force disc",0,0,0);
         eventData.event    = OEMOBEX_EVT_SRV_DISC_COMPL;
         RELEASE_HDR(me->pOemSendHdr);
         break;
      }
      case BT_PF_CMD_GOEP_SRV_GET_RSP:
      {
         MSG_HIGH("OEM OBEX srv cmd dne GET resp ",0,0,0);
         eventData.event    = OEMOBEX_EVT_SRV_GET_COMPL;
         break;
      }
      case BT_PF_CMD_GOEP_SRV_PUT_RSP:
      {
         MSG_HIGH("OEM OBEX srv cmd dne  PUT resp",0,0,0);
         eventData.event    = OEMOBEX_EVT_SRV_PUT_COMPL;
         break;
      }
      case BT_PF_CMD_GOEP_SRV_CFM_SET_PATH:
      {
         MSG_HIGH("OEM OBEX srv cmd dne SETPATH resp ",0,0,0);
         eventData.event    = OEMOBEX_EVT_SRV_SETPATH_COMPL;
         break;
      }
      default:
      break;
   }

       // call the aee callback with this event
   me->pAeeCb(me->cbData,
              &eventData,
              NULL,
              NULL);

}



//=============================================================================
// 
//
//=============================================================================
static OEMOBEXHdr* OEMOBEXSrv_GetRecvHdr(OEMOBEXSrv *me)
{
   boolean  bFound = FALSE;
   int i = 0;
   OEMOBEXHdr *pHdr = NULL;

    // see if there is free header in the list 
   for (i = 0; i < OEMOBEX_MAX_RECEIVE_BUFF; i++)
   {
      if (me->aRecvHdr[i] != NULL)
      {  // there is an oem header, check if it is free
         bFound = OEMOBEXHdr_IsFree(me->aRecvHdr[i]);

         if (TRUE == bFound)
         {
            pHdr = me->aRecvHdr[i];
            OEMOBEXHdr_AddRef(pHdr);
            break;
         }
      }
   }
   return pHdr;
}
//=============================================================================
// 
//
//=============================================================================
boolean OEMOBEXSrv_RecvHdrAvailable(OEMOBEXSrv *me)
{
   boolean  bFound = FALSE;
   int i = 0;
   int ret = AEE_ENOMEMORY;

   // see if there is free header in the list 
   for (i = 0; i < OEMOBEX_MAX_RECEIVE_BUFF; i++)
   {
      if (me->aRecvHdr[i] != NULL)
      {  // there is an oem header, check if it is free
         bFound = OEMOBEXHdr_IsFree(me->aRecvHdr[i]);

         if (TRUE == bFound)
         {
            ret = AEE_SUCCESS;
            break;
         }
      }
   }

   if (FALSE == bFound)
   {
      // no oem hdr in the list we have is free, try to allocate one
      for (i = 0; i < OEMOBEX_MAX_RECEIVE_BUFF; i++)
      {
         if (NULL == me->aRecvHdr[i])
         {  // there is place for an oem header, see if we can allocate one
            ret = OEMOBEXHeader_Init(&me->aRecvHdr[i], OEMOBEX_MAX_RECV_BUFFER_SIZE);
            break;
         }
      }
   }

   if (AEE_SUCCESS == ret)
   {
      bFound = TRUE;
   }
   
   return bFound;
}
//=============================================================================
// 
//
//=============================================================================

static void OEMOBEXSrv_RecvHdrRelease(OEMOBEXSrv *me)
{
   int i = 0;

   for (i = 0; i < OEMOBEX_MAX_RECEIVE_BUFF; i++)
   {
      if (me->aRecvHdr[i] != NULL)
      {
         OEMOBEXHdr_Release(me->aRecvHdr[i]);
         me->aRecvHdr[i] = NULL;
      }
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================
int OEMOBEXSrv_GetBda(OEMOBEXSrv *me, uint8* pBda)
{
   int i = 0;

   for (i =0; i < OEMOBEX_BDADDR_LEN; i++)
   {
      pBda[i] = me->bdAddr.bd_addr_bytes[i];
   }

   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
int OEMOBEXSrv_GetScn(OEMOBEXSrv *me, uint32 * pScn)
{
   *pScn = me->channel;

   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
int OEMOBEXSrv_GetPacketSize(OEMOBEXSrv *me, uint32 *pPacketSize)
{
   *pPacketSize = OEMOBEX_MAX_RECV_BUFFER_SIZE;//#check
   return AEE_SUCCESS;
}
#endif /* #if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP) */


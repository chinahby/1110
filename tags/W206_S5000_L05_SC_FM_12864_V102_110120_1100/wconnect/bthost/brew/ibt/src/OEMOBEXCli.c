/*=============================================================================
FILE:  OEMOBEXCli.c

SERVICES:  OEM implementation of OBEX Client

GENERAL DESCRIPTION:
        OEM implementation of OBEX Client
   
===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMOBEXCli.c#1 $ 
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
#define OEMOBEX_CONN_ID_INVALID                   0xFFFF

//=============================================================================
// 
//
//=============================================================================
//
static uint32 OEMOBEXCli_MtCmdToAeeEvt( bt_pf_cmd_type cmdType);
static OEMOBEXCli* OEMOBEXCli_FindMe(mt_obex_app_id_type appId);
static OEMOBEXHdr* OEMOBEXCli_GetRecvHdr(OEMOBEXCli *me);
static rex_crit_sect_type   gOBEXCliCritSect; 
//=============================================================================
// 
//
//=============================================================================
struct OEMOBEXCli
{
   void                        *cbData;
   uint32                      uRef;     //reference count
   mt_obex_app_id_type         appId;
   mt_obex_srv_auth_type       auth;
   mt_obex_tp_conn_struct_type connStruct;
   boolean                     bReadOnly;
   uint8                       targetId;
   uint16                      clientConnID;
   struct OEMOBEXCli           *pNextObject ;  // linked into mgr
   OEMOBEXHdr                  *aRecvHdr[OEMOBEX_MAX_RECEIVE_BUFF];
   OEMOBEXHdr                  *pOemSendHdr;
   PFNOBEXEVTCB                pAeeCb;
   boolean                     bConnected;
   boolean                     bDiscInitiated;
   bt_bd_addr_type             bdAddr;
};

//=============================================================================
// 
//
//=============================================================================
// OBEX object manager
typedef struct
{
  OEMOBEXCli *pHeadObject;    // singly linked list of OBEX objects
} OEMOBEXCliMgr;
//=============================================================================
// 
//
//=============================================================================
static OEMOBEXCliMgr         gOBEXCliMgr;
static rex_crit_sect_type    gOBEXCliCritSect;

static int OEMOBEXCli_Authenticate(OEMOBEXCli *pOemobj,OEMOBEXAuthInfo *pAuthInfo);
static void OEMOBEXCli_EventCallback(bt_ev_msg_type* ev_msg_ptr);
static void OEMOBEXCli_HandleCmdDne(OEMOBEXCli *me,
                                    bt_ev_gn_cmd_done_type *pCmdDn);
static void OEMOBEXCli_HandleConCfm(OEMOBEXCli *me,
                                    mt_obex_ev_cli_con_cfm_type *pConCfmEvt);
static void OEMOBEXCli_HandleDiscInd(OEMOBEXCli *me,
                                     mt_obex_ev_cli_dcn_ind_type *pDcnIndEvt);
static void OEMOBEXCli_HandleAuthInd(OEMOBEXCli *me,
                                    mt_obex_ev_cli_auth_ind_type *pCliAuthEvt);
static void OEMOBEXCli_HandleAuthRespInd(OEMOBEXCli *me,
                                    mt_obex_ev_cli_auth_res_ind_type *pCliAuthEvt);
static void OEMOBEXCli_HandleSetPathRespInd(OEMOBEXCli *me,
                                    mt_obex_ev_cli_set_path_cfm_type *pSetPathEvt);
static void OEMOBEXCli_HandlePutRespInd(OEMOBEXCli *me,
                                    mt_obex_ev_cli_put_cfm_type *pCliPutEvt);
static void OEMOBEXCli_HandleGetRespInd(OEMOBEXCli *me,
                                    mt_obex_ev_cli_get_rcv_data_type *pCliGetEvt);

//==========================================================================
//   
//==========================================================================

int OEMOBEXCli_Init(OEMOBEXCli **pOemobj, void* cbData, PFNOBEXEVTCB aeeEvtCb)
{
   OEMOBEXCli *me;

   MSG_DEBUG("OEM OBEX cli init ->",0,0,0);

   me = (OEMOBEXCli*)OEM_Malloc(sizeof(OEMOBEXCli));

   if (NULL == me)
   {
      MSG_ERROR("OEM OBEX cli could not alloc memory",0,0,0);
      return AEE_ENOMEMORY;
   }
   //since sys_malloc does not zero out the memory, need to initialize it.
   MEMSET(me, 0, sizeof(OEMOBEXCli));
   me->uRef         = 1;
   me->cbData       = cbData;
   me->targetId     = 0;
   me->clientConnID = 0;
   me->pAeeCb       = aeeEvtCb;
   me->pOemSendHdr  = NULL;
   me->bDiscInitiated = FALSE;
   me->bConnected     = FALSE;
   me->appId =  bt_cmd_ec_get_app_id_and_register(OEMOBEXCli_EventCallback);

   if (BT_APP_ID_NULL == me->appId)
   {
      OEMOBEXCli_Release(me);
      MSG_ERROR("OEM OBEX cli could not alloc app id",0,0,0);
      return AEE_EFAILED;
   }

   *pOemobj = me;

   rex_enter_crit_sect(&gOBEXCliCritSect);

   me->pNextObject  = gOBEXCliMgr.pHeadObject;
   gOBEXCliMgr.pHeadObject = me;

   rex_leave_crit_sect(&gOBEXCliCritSect);

   MSG_DEBUG("OEM OBEX cli init <-",0,0,0);

#ifdef BT_SIMULATION
   bt_sim_start(PROFILE_OBEX_CLIENT, TRUE);
#endif 

   return AEE_SUCCESS;
}

//=============================================================================
// 
//
//=============================================================================
int OEMOBEXCli_Release(OEMOBEXCli *me)
{
   uint32      uRef = --me->uRef;
   OEMOBEXCli  **ppc = NULL;
   int i = 0;

   MSG_DEBUG("OEM OBEX cli release uRef %x ->",uRef,0,0);

   if (0 == uRef)
   {
      if (BT_APP_ID_NULL != me->appId)
      {
         bt_cmd_ec_free_application_id(me->appId);
         me->appId = 0;
      }
      rex_enter_crit_sect(&gOBEXCliCritSect);

      for (ppc = &gOBEXCliMgr.pHeadObject; *ppc != NULL; ppc = &(*ppc)->pNextObject)
      {
         if ((*ppc)->appId == me->appId)
         {
            *ppc = me->pNextObject;
            me->pNextObject = NULL; 
            break;
         }
       }
      rex_leave_crit_sect(&gOBEXCliCritSect);

      for (i = 0; i < OEMOBEX_MAX_RECEIVE_BUFF; i++)
      {
         if (me->aRecvHdr[i] != NULL)
         {
             OEMOBEXHdr_Release(me->aRecvHdr[i]);
             me->aRecvHdr[i] = NULL;
         }
      }
      OEM_Free(me);
   }

   MSG_DEBUG("OEM OBEX cli release uRef %x <-",uRef,0,0);
   return uRef;
}
//=============================================================================
// 
//
//=============================================================================
int OEMOBEXCli_SetTransport(OEMOBEXCli *me,OEMOBEX_TransportConnInfo *pTransInfo)
{
   int ret = AEE_SUCCESS;
   mt_obex_tp_conn_struct_type *pConnInfo = &me->connStruct;
   

   MSG_DEBUG("OEM OBEX cli set transport ->",0,0,0);

   switch (pTransInfo->transport)
   {
      case OEMOBEX_TRANS_BT_RFCOMM:
      {
         int i = 0;
         pConnInfo->transport = MT_OBEX_BLUETOOTH_TRANSPORT;

         for (i = 0; i < OEMOBEX_BDADDR_LEN; i++)
         {
            me->bdAddr.bd_addr_bytes[i] = pTransInfo->transportConn.btConn.bdAddr[i];
         }
         pConnInfo->transport_conn.bt_conn.bd_addr_ptr = (bt_bd_addr_type*)&me->bdAddr;
         pConnInfo->transport_conn.bt_conn.channel = 
          pTransInfo->transportConn.btConn.channel;
         pConnInfo->transport_conn.bt_conn.uuid =
          (mt_obex_sd_uuid_type)pTransInfo->transportConn.btConn.uuid; 
         pConnInfo->transport_conn.bt_conn.security = 
          OEMOBEX_OEMToMtSecurity(pTransInfo->transportConn.btConn.security);
         break;
      }
     default:
     {
       ret = AEE_EUNSUPPORTED; 
       break;
     }
   }

   MSG_DEBUG("OEM OBEX cli set transport ret %x<-",ret,0,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int OEMOBEXCli_Authenticate(OEMOBEXCli *me,OEMOBEXAuthInfo *pAuthInfo)
{
   mt_realm_info        realm;
   mt_obex_status_type  status;
   int len = 0;
   int ret = AEE_SUCCESS;

   MSG_DEBUG("OEM OBEX cli set authenticate realm len %x ->",pAuthInfo->nRealmLen,0,0);

   len = (pAuthInfo->nRealmLen < MT_OBEX_MAX_REALM_LEN) ?
          pAuthInfo->nRealmLen : MT_OBEX_MAX_REALM_LEN;

   realm.len          = len;
   realm.realmStrType = (uint8)pAuthInfo->charSet;
   if (0 < len)
   {
      MEMCPY((void*)&realm.realmStr[0],(void*)&pAuthInfo->aRealm[0],len);
   }
   status = mt_obex_client_authenticate(&me->connStruct,
                                        &realm,
                                        pAuthInfo->bUserIdReq,
                                        pAuthInfo->bFullAccess);

   ret = OEMOBEX_CmdStatusToAeeResult(status);

   MSG_DEBUG("OEM OBEX cli authenticate ret %x <-",ret,0,0);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================

int OEMOBEXCli_Connect(OEMOBEXCli *me,
                       OEMOBEXHdr *pOemHdr,
                       OEMOBEXAuthInfo *pAuthInfo)
{
   int ret            = AEE_SUCCESS;
   mt_obex_status_type status = MT_OBEX_SUCCESS;
   mt_obex_hdr_list_type *pHdrList = NULL;

   MSG_DEBUG("OEM OBEX cli connect pOemHdr %x pAuthInfo %x ->",pOemHdr,pAuthInfo,0);

   if (pOemHdr)
   {
      OEMOBEXHdr_AddRef(pOemHdr);
      pHdrList = OEMOBEXHdr_GetHeaderList(pOemHdr);
   }

   // if authentication is required, set the params
   if (NULL != pAuthInfo)
   {
      ret = OEMOBEXCli_Authenticate(me, pAuthInfo);
   }

   if (AEE_SUCCESS == ret)
   {
      me->clientConnID = OEMOBEX_CONN_ID_INVALID;
      // this will be freed when the connect completes
      me->pOemSendHdr = pOemHdr;
      status = mt_obex_cli_connect(&me->connStruct, me->appId, pHdrList);
      ret = OEMOBEX_CmdStatusToAeeResult(status);
   }
   if ((AEE_SUCCESS != ret) && (me->pOemSendHdr != NULL))
   {
     OEMOBEXHdr_Release(me->pOemSendHdr);
     me->pOemSendHdr = NULL;
   }

   MSG_DEBUG("OEM OBEX cli connect ret %x <-",ret,0,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int OEMOBEXCli_Disconnect(OEMOBEXCli *me, OEMOBEXHdr *pOemHdr)
{
   int ret            = AEE_SUCCESS;
   mt_obex_status_type status   = (mt_obex_status_type)BT_CS_GN_SUCCESS;
   mt_obex_hdr_list_type *pHdrList = NULL;

   MSG_DEBUG("OEM OBEX cli disconnect pOemHdr %x  ->",pOemHdr,0,0);

   if (pOemHdr)
   {
      OEMOBEXHdr_AddRef(pOemHdr);
      pHdrList = OEMOBEXHdr_GetHeaderList(pOemHdr);
   }
   me->pOemSendHdr = pOemHdr;
   me->bDiscInitiated = TRUE;
   status = mt_obex_cli_disconnect(me->appId, me->clientConnID, pHdrList);
   ret = OEMOBEX_CmdStatusToAeeResult(status);

   if ((AEE_SUCCESS != ret) && (me->pOemSendHdr != NULL))
   {
      OEMOBEXHdr_Release(me->pOemSendHdr);
      me->pOemSendHdr = NULL;
      me->bDiscInitiated = FALSE;
   }

   MSG_DEBUG("OEM OBEX cli disconnect ret  %x  <-",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
int OEMOBEXCli_Put(OEMOBEXCli      *me,
                   OEMOBEXHdr      *pOemHdr,
                   boolean         bfinal,
                   OEMOBEXAuthInfo *pAuthInfo)
{
   int ret = AEE_SUCCESS;
   mt_obex_status_type status   = (mt_obex_status_type)BT_CS_GN_SUCCESS;
   mt_obex_status_type mtStatus = MT_OBEX_SUCCESS;

   mt_obex_hdr_list_type *pHdrList = NULL;

   MSG_DEBUG("OEM OBEX cli PUT -> ",0,0,0);

   MSG_MED("OEM OBEX cli PUT bfinal %x, pOemHdr %x pAuthInfo %x",bfinal,pOemHdr,pAuthInfo);

   // currently authentication is supported only during connect
   if(NULL != pAuthInfo)
   {
     MSG_ERROR("OEM OBEX cli auth not suported for Put",0,0,0);
     return AEE_EUNSUPPORTED;
   }
   // do not allow connectionless Puts 
   if (TRUE != me->bConnected)
   {
      MSG_ERROR("OEM OBEX cli not connected",0,0,0);
      return AEE_EBADSTATE;
   }
   if (pOemHdr)
   {
      OEMOBEXHdr_AddRef(pOemHdr);
      pHdrList = OEMOBEXHdr_GetHeaderList(pOemHdr);
   }
   mtStatus = (bfinal) ? MT_OBEX_SUCCESS: MT_OBEX_CONTINUE;
   me->pOemSendHdr = pOemHdr;

   status = mt_obex_cli_put(me->appId,
                            me->clientConnID,
                            pHdrList,
                            mtStatus);

   ret = OEMOBEX_CmdStatusToAeeResult(status);

   if ((AEE_SUCCESS != ret) && (me->pOemSendHdr != NULL))
   {
      OEMOBEXHdr_Release(me->pOemSendHdr);
      me->pOemSendHdr = NULL;
   }

   MSG_DEBUG("OEM OBEX cli PUT ret %x <- ",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
int OEMOBEXCli_Get(OEMOBEXCli      *me,
                   OEMOBEXHdr      *pOemHdr,
                   boolean         bfinal,
                   OEMOBEXAuthInfo *pAuthInfo)
{
   int ret = AEE_SUCCESS;
   mt_obex_status_type status = (mt_obex_status_type)BT_CS_GN_SUCCESS;
   mt_obex_hdr_list_type *pHdrList = NULL;

   MSG_DEBUG("OEM OBEX cli GET -> ",0,0,0);
   MSG_MED("OEM OBEX cli GET bfinal %x, pOemHdr %x pAuthInfo %x",bfinal,pOemHdr,pAuthInfo);

   // currently authentication is supported only during connect
   if(NULL != pAuthInfo)
   {
      MSG_ERROR("OEM OBEX cli auth not suported for GET",0,0,0);
     return AEE_EUNSUPPORTED;
   }
   // do not allow connectionless Gets
   if (TRUE != me->bConnected)
   {
      MSG_ERROR("OEM OBEX cli not connected",0,0,0);
      return AEE_EBADSTATE;
   }
   if (pOemHdr)
   {
      OEMOBEXHdr_AddRef(pOemHdr);
      pHdrList = OEMOBEXHdr_GetHeaderList(pOemHdr);
   }
   me->pOemSendHdr = pOemHdr;
   status = mt_obex_cli_get(me->appId, me->clientConnID, pHdrList, bfinal);

   ret = OEMOBEX_CmdStatusToAeeResult(status);

   if ((AEE_SUCCESS != ret) && (me->pOemSendHdr != NULL))
   {
      OEMOBEXHdr_Release(me->pOemSendHdr);
      me->pOemSendHdr = NULL;
   }

   MSG_DEBUG("OEM OBEX cli GET ret %x <- ",ret,0,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================

int OEMOBEXCli_SetPath(OEMOBEXCli      *me,
                       OEMOBEXHdr      *pOemHdr,
                       boolean         upLevel,
                       boolean         dontCreate,
                       OEMOBEXAuthInfo *pAuthInfo)
{
   int ret = AEE_SUCCESS;
   mt_obex_status_type status;
   mt_obex_hdr_list_type *pHdrList = NULL;
   uint16 pathName[BT_PF_MAX_FILENAME_LEN];

   MSG_DEBUG("OEM OBEX cli SETPATH -> ",0,0,0);

   MSG_MED("OEM OBEX cli SETPATH pOemHdr %x, upLevel %x dontCreate %x",pOemHdr,upLevel,dontCreate);

   // currently authentication is supported only during connect
   if(NULL != pAuthInfo)
   {
      MSG_ERROR("OEM OBEX cli auth not suported for SETPATH",0,0,0);
     return AEE_EUNSUPPORTED;
   }
   // do not allow connectionless Ops
   if (TRUE != me->bConnected)
   {
      MSG_ERROR("OEM OBEX cli not connected",0,0,0);
      return AEE_EBADSTATE;
   }
   if (pOemHdr)
   {
      OEMOBEXHdr_AddRef(pOemHdr);
      pHdrList = OEMOBEXHdr_GetHeaderList(pOemHdr);
      me->pOemSendHdr = pOemHdr;
   }
   ret = OEMOBEXHdr_GetNameHeader(pOemHdr,&pathName[0],BT_PF_MAX_FILENAME_LEN);

   if (AEE_SUCCESS == ret)
   {

   status = mt_obex_cli_set_path(me->appId,
                                 me->clientConnID, 
                                 (uint16*)&pathName,
                                 upLevel,
                                 dontCreate,
                                 pHdrList);

   ret = OEMOBEX_CmdStatusToAeeResult(status);
   }

   if ((AEE_SUCCESS != ret) && (me->pOemSendHdr != NULL))
   {
      OEMOBEXHdr_Release(me->pOemSendHdr);
      me->pOemSendHdr = NULL;
   }

   MSG_DEBUG("OEM OBEX cli SETPATH ret %x <- ",ret,0,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int OEMOBEXCli_Abort(OEMOBEXCli *me, OEMOBEXHdr *pOemHdr) //#check, this doesnt take headers
{
   mt_obex_status_type status;

   if (TRUE != me->bConnected)
   {
      MSG_ERROR("OEM OBEX cli abort: not connected ",0,0,0);
      return AEE_EBADSTATE;
   }
   status = mt_obex_cli_abort_operation(me->appId, me->clientConnID);
         
   return  OEMOBEX_CmdStatusToAeeResult(status);
}

//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXCli_EventCallback(bt_ev_msg_type* ev_msg_ptr)
{
   OEMOBEXCli *me = OEMOBEXCli_FindMe(ev_msg_ptr->ev_hdr.bt_app_id);
   boolean bFreeSendHdr = TRUE;


   if (NULL == me)
   {
      MSG_ERROR("OEM OBEX CLI Event callback called for non existing app", 0, 0, 0);
      return;

   }
   switch ( ev_msg_ptr->ev_hdr.ev_type )
   {
      case  BT_EV_GN_CMD_DONE:
      {
         OEMOBEXCli_HandleCmdDne(me, &ev_msg_ptr->ev_msg.ev_gn_cmd_done);

         if ((BT_CS_GN_SUCCESS == ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status) ||
            (BT_CS_GN_PENDING == ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status))
         {
            bFreeSendHdr = FALSE;
         }
         break;
      } 
      case MT_OBEX_EV_CLI_CON_CFM:
      {
         OEMOBEXCli_HandleConCfm(me,
                                 &ev_msg_ptr->ev_msg.ev_goep_cli_con_cfm);
         break;
      }
      case MT_OBEX_EV_CLI_DCN_IND:
      {
         OEMOBEXCli_HandleDiscInd(me,
                                  &ev_msg_ptr->ev_msg.ev_goep_cli_dcn_ind);
         break;
      }
      case MT_OBEX_EV_CLI_AUTH_IND:
      {
         OEMOBEXCli_HandleAuthInd(me,
                                  &ev_msg_ptr->ev_msg.ev_goep_cli_auth_ind);
         break;
      }
      case MT_OBEX_EV_CLI_AUTH_RESP_IND:
      {
         OEMOBEXCli_HandleAuthRespInd(me,
                              &ev_msg_ptr->ev_msg.ev_goep_cli_auth_res_ind);
         break;
      }
      case MT_OBEX_EV_CLI_GET_RCV_DATA:
      { 
         OEMOBEXCli_HandleGetRespInd(me,
                                &ev_msg_ptr->ev_msg.ev_goep_cli_get_rcv_data);
         break;
      }
      case MT_OBEX_EV_CLI_PUT_CFM:
      {
         OEMOBEXCli_HandlePutRespInd(me,
                                    &ev_msg_ptr->ev_msg.ev_goep_cli_put_cfm);
         break;
      }
      case MT_OBEX_EV_CLI_SET_PATH_CFM:
      {
         OEMOBEXCli_HandleSetPathRespInd(me,
                                    &ev_msg_ptr->ev_msg.ev_goep_cli_set_path_cfm);
         break;
      }
      default :
         MSG_ERROR("OEM OBEX Cli Evt Handler unknown event ",0, 0, 0);
         break;
   }
   if ((bFreeSendHdr) && (me->pOemSendHdr != NULL))
   {
      OEMOBEXHdr_Release(me->pOemSendHdr);
      me->pOemSendHdr = NULL;
   }

   return; 
}

//=============================================================================
// 
//
//=============================================================================
int OEMOBEXCli_AuthResponse(OEMOBEXCli  *me,
                            const uint8 *pUserID,
                            int         uidLen,
                            const uint8 *pPassword,
                            int         pswdLen)
{
   char aUid[BT_PF_OBEX_MAX_USERID_LEN];
   char aPswd[BT_PF_OBEX_MAX_PASSWORD_LEN];
   mt_obex_status_type mtCmdStatus;
   int ret = AEE_SUCCESS;

   MSG_MED("OEM OBEX Cli Auth resp uidLen %x pswdLen %x -> ",uidLen, pswdLen, 0);

   if ((uidLen >= BT_PF_OBEX_MAX_USERID_LEN) ||
       (pswdLen >= BT_PF_OBEX_MAX_PASSWORD_LEN))
   {
      MSG_ERROR("OEM OBEX Cli uidlen pswd len  too long ",0,0,0);
      return AEE_EBADPARM;
   }

   MEMSET((void*)&aUid[0], 0, BT_PF_OBEX_MAX_USERID_LEN);
   MEMSET((void*)&aPswd[0], 0, BT_PF_OBEX_MAX_PASSWORD_LEN);

   if (pUserID != NULL)
   {
      MEMCPY((void*)&aUid[0], (void*)pUserID, uidLen);
   }
   if (pPassword != NULL)
   {
      MEMCPY((void*)&aPswd[0], (void*)pPassword, pswdLen);
   }

   aPswd[BT_PF_OBEX_MAX_PASSWORD_LEN - 1] = 0;
   mtCmdStatus = mt_obex_cli_auth_response(me->appId,
                                           me->clientConnID,
                                           (byte*)&aUid[0],
                                           (uint8)uidLen,
                                           (char*)&aPswd[0]);

   ret = OEMOBEX_CmdStatusToAeeResult(mtCmdStatus);

   MSG_DEBUG("OEM OBEX Cli Auth resp uidLen %x pswdLen %x ret <- ",uidLen, pswdLen,ret);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXCli_HandleCmdDne(OEMOBEXCli *me,
                                    bt_ev_gn_cmd_done_type *pCmdDn)

{
   OEMOBEXEvent eventData;
   MEMSET((void*)&eventData, 0 ,sizeof(eventData));

   MSG_MED("OEM OBEX Cli cmd done stat %x ",pCmdDn->cmd_status,0,0);

   if ((BT_CS_GN_SUCCESS == pCmdDn->cmd_status) ||
       (BT_CS_GN_PENDING == pCmdDn->cmd_status) ||
       ((pCmdDn->cmd_type == BT_PF_CMD_GOEP_CLI_DISCONNECT) && 
        (FALSE == me->bConnected)) )// this indicates that the peer
                                    // disconnected while we were trying to disconnect  
   {
      return;
   }
   
   eventData.event   = 
            (uint8)OEMOBEXCli_MtCmdToAeeEvt((bt_pf_cmd_type)pCmdDn->cmd_type);
   eventData.u.cli.pOemHdr = NULL;
   eventData.u.cli.status  = 
            OEMOBEX_MtToAeeStatus((mt_obex_status_type)pCmdDn->cmd_status);
   
   me->pAeeCb(me->cbData,&eventData, NULL, NULL);
   return;
}

//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXCli_HandleConCfm(OEMOBEXCli *me,
                                    mt_obex_ev_cli_con_cfm_type *pConCfmEvt)

{
   OEMOBEXEvent eventData;

   MSG_MED("OEM OBEX Cli con cfm mt stat %x ",pConCfmEvt->status,0,0);

   MEMSET((void*)&eventData, 0 ,sizeof(eventData));

   eventData.event   = AEEOBEX_EVT_CLI_CONN_RESP;
   eventData.u.cli.status  = 
            OEMOBEX_MtToAeeStatus((mt_obex_status_type)pConCfmEvt->status);
   
   if((pConCfmEvt->rsp_headers_cnt != 0) &&
      (pConCfmEvt->rsp_headers_ptr != NULL))
   {
      eventData.u.cli.pOemHdr = OEMOBEXCli_GetRecvHdr(me);
      OEMOBEXHdr_MtToAeeHeaders(pConCfmEvt->rsp_headers_cnt,
                                pConCfmEvt->rsp_headers_ptr,
                                eventData.u.cli.pOemHdr,
                                FALSE,
                                me->appId);
   }
   
   if (MT_OBEX_SUCCESS == pConCfmEvt->status)
   {
      me->bConnected = TRUE;
      me->clientConnID = pConCfmEvt->conn_id;
   }
 
   me->pAeeCb(me->cbData,&eventData, NULL, NULL);
   return;
}

//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXCli_HandleDiscInd(OEMOBEXCli *me,
                                     mt_obex_ev_cli_dcn_ind_type *pDcnIndEvt)

{
   OEMOBEXEvent eventData;
   MEMSET((void*)&eventData, 0 ,sizeof(eventData));

   if (me->clientConnID != pDcnIndEvt->conn_id)
   {
      MSG_ERROR("OEM OBEX Evt Handler Client Disconnect ind - wrong conn id: %x", pDcnIndEvt->conn_id, 0, 0);
   }
   else
   {
      if (me->bDiscInitiated)
      {
         MSG_MED("OEM OBEX Cli Discon resp con id %x",
                  pDcnIndEvt->conn_id, 0, 0);
         eventData.event   = AEEOBEX_EVT_CLI_DISC_RESP;
         eventData.u.cli.status  = AEEOBEX_STAT_SUCCESS;
      }
      else
      {
         MSG_MED("OEM OBEX Cli Discon ind con id %x",
                  pDcnIndEvt->conn_id, 0, 0);
         eventData.event   = AEEOBEX_EVT_ERROR;
         eventData.u.cli.status  = AEEOBEX_STAT_SERVER_FORCED_DISCONNECT;
         me->bDiscInitiated = FALSE;
      }
      me->bConnected    = FALSE;
      me->clientConnID  = OEMOBEX_CONN_ID_INVALID;
      if((pDcnIndEvt->rsp_headers_cnt != 0) &&
         (pDcnIndEvt->rsp_headers_ptr != NULL))
      {
         eventData.u.cli.pOemHdr = OEMOBEXCli_GetRecvHdr(me);
         OEMOBEXHdr_MtToAeeHeaders(pDcnIndEvt->rsp_headers_cnt,
                                   pDcnIndEvt->rsp_headers_ptr,
                                   eventData.u.cli.pOemHdr,
                                   TRUE,
                                   me->appId);
      }
   }
   me->pAeeCb(me->cbData,&eventData, NULL, NULL);
   return;
}
//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXCli_HandleAuthInd(OEMOBEXCli *me,
                                    mt_obex_ev_cli_auth_ind_type *pCliAuthEvt)
{
   OEMOBEXAuthInfo oemAuthChal;
   OEMOBEXEvent eventData;

   MEMSET((void*)&eventData, 0 ,sizeof(OEMOBEXEvent));
   MEMSET((void*)&oemAuthChal, 0 ,sizeof(OEMOBEXAuthInfo));
   me->clientConnID = pCliAuthEvt->conn_id;

   MSG_HIGH("OEM OBEX Cli auth ind. conn id:%x UIDReqd:%x",
      pCliAuthEvt->conn_id, pCliAuthEvt->user_id_required, 0);

   eventData.event   = AEEOBEX_EVT_AUTH_CHALLENGE;
   eventData.u.cli.status  = AEE_SUCCESS;
   eventData.u.cli.pOemHdr = NULL;

   oemAuthChal.bUserIdReq  = pCliAuthEvt->user_id_required;
   oemAuthChal.bFullAccess = pCliAuthEvt->full_access;
   oemAuthChal.charSet     = 0;
   oemAuthChal.nRealmLen   = 0;

   // realm is not sent to the OEM layer //#check
   MEMSET((void*)&oemAuthChal.aRealm[0], 0, sizeof(oemAuthChal.aRealm));

   me->pAeeCb(me->cbData,&eventData, &oemAuthChal, NULL);

   return;
}
//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXCli_HandleAuthRespInd(OEMOBEXCli *me,
                                    mt_obex_ev_cli_auth_res_ind_type *pCliAuthEvt)
{
   OEMOBEXAuthResp oemAuthResp;
   OEMOBEXEvent eventData;

   MSG_HIGH("OEM OBEX Cli auth response ind. conn id : %x",
            pCliAuthEvt->conn_id, 0, 0);

   MEMSET((void*)&eventData, 0 ,sizeof(OEMOBEXEvent));
   MEMSET((void*)&oemAuthResp, 0 ,sizeof(OEMOBEXAuthResp));

   me->clientConnID = pCliAuthEvt->conn_id;
   eventData.event   = AEEOBEX_EVT_AUTH_RESPONSE;
   eventData.u.cli.status  = AEE_SUCCESS;
   eventData.u.cli.pOemHdr = NULL;

   oemAuthResp.bFullAccess = TRUE; //#check not returned by obex
   oemAuthResp.nUidLen     = 0;   //#check not returned by obex
   MEMSET((void*)oemAuthResp.aUserId, 0, sizeof(oemAuthResp.aUserId));// #check

   me->pAeeCb(me->cbData,&eventData, NULL, &oemAuthResp);

   return;
}

//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXCli_HandleSetPathRespInd(OEMOBEXCli *me,
                                    mt_obex_ev_cli_set_path_cfm_type *pSetPathEvt)

{
   OEMOBEXEvent eventData;

   MSG_HIGH("OEM OBEX Cli Evt Handler set path Cfm, id=%x, status=%x",
             pSetPathEvt->conn_id, pSetPathEvt->status, 0);

   MEMSET((void*)&eventData, 0 ,sizeof(OEMOBEXEvent));

   if (me->clientConnID != pSetPathEvt->conn_id)
   {
      MSG_ERROR("OEM OBEX Evt Handler Client set path ind - wrong conn id: %x",
                pSetPathEvt->conn_id, 0, 0);
   }
   else
   {
      eventData.event   = AEEOBEX_EVT_CLI_SETPATH_RESP;
      eventData.u.cli.status  = 
                OEMOBEX_MtToAeeStatus((mt_obex_status_type)pSetPathEvt->status);

      if((pSetPathEvt->rsp_headers_cnt != 0) &&
         (pSetPathEvt->rsp_headers_ptr != NULL))
      {
         eventData.u.cli.pOemHdr = OEMOBEXCli_GetRecvHdr(me);
         OEMOBEXHdr_MtToAeeHeaders(pSetPathEvt->rsp_headers_cnt,
                                   pSetPathEvt->rsp_headers_ptr,
                                   eventData.u.cli.pOemHdr,
                                   FALSE,
                                   me->appId);
         
      }
      me->pAeeCb(me->cbData,&eventData, NULL, NULL);
   }

   return;
}
//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXCli_HandlePutRespInd(OEMOBEXCli *me,
                                    mt_obex_ev_cli_put_cfm_type *pCliPutEvt)
{
   OEMOBEXEvent eventData;

   MSG_HIGH("OEM OBEX Cli Evt Handler Put Cfm, id=%x, status=%x",
              pCliPutEvt->conn_id, pCliPutEvt->status, 0);

   MEMSET((void*)&eventData, 0 ,sizeof(OEMOBEXEvent));

   if (me->clientConnID != pCliPutEvt->conn_id)
   {
      MSG_ERROR("OEM OBEX Evt Handler Client Put data ind - wrong conn id: %x",
                 pCliPutEvt->conn_id, 0, 0);
   }
   else
   {
      eventData.event   = AEEOBEX_EVT_CLI_PUT_RESP;
      eventData.u.cli.status  = 
                OEMOBEX_MtToAeeStatus((mt_obex_status_type)pCliPutEvt->status);

      if((pCliPutEvt->rsp_headers_cnt != 0) &&
         (pCliPutEvt->rsp_headers_ptr != NULL))
      {
         eventData.u.cli.pOemHdr = OEMOBEXCli_GetRecvHdr(me);
         OEMOBEXHdr_MtToAeeHeaders(pCliPutEvt->rsp_headers_cnt,
                                   pCliPutEvt->rsp_headers_ptr,
                                   eventData.u.cli.pOemHdr,
                                   FALSE,
                                   me->appId);
      }
      me->pAeeCb(me->cbData,&eventData, NULL, NULL);
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================
static void OEMOBEXCli_HandleGetRespInd(OEMOBEXCli *me,
                                    mt_obex_ev_cli_get_rcv_data_type *pCliGetEvt)

{
   OEMOBEXEvent eventData;

   MSG_HIGH("OEM OBEX Cli Evt Handler GET Rcv Data, id=%x, status=%x", 
      pCliGetEvt->conn_id, pCliGetEvt->status, 0);

   MEMSET((void*)&eventData, 0 ,sizeof(OEMOBEXEvent));

   if (me->clientConnID != pCliGetEvt->conn_id)
   {
      MSG_ERROR("OEM OBEX Evt Handler Client get data ind - wrong conn id: %x",
                 pCliGetEvt->conn_id, 0, 0);
   }
   else
   {
      eventData.event   = AEEOBEX_EVT_CLI_GET_RESP;
      eventData.u.cli.status  = 
                OEMOBEX_MtToAeeStatus((mt_obex_status_type)pCliGetEvt->status);
      eventData.u.cli.pOemHdr = OEMOBEXCli_GetRecvHdr(me);
      OEMOBEXHdr_MtToAeeHeaders(pCliGetEvt->rsp_headers_cnt,
                                pCliGetEvt->rsp_headers_ptr,
                                eventData.u.cli.pOemHdr,
                                FALSE,
                                me->appId);
      me->pAeeCb(me->cbData,&eventData, NULL, NULL);
   }
   return;
}

//=============================================================================
// 
//
//=============================================================================
static OEMOBEXCli* OEMOBEXCli_FindMe(mt_obex_app_id_type appId)
{
   OEMOBEXCli *me = NULL;


   rex_enter_crit_sect(&gOBEXCliCritSect);

   for (me = gOBEXCliMgr.pHeadObject; me != NULL; me = me->pNextObject)
   {
      if (me->appId == appId)
      {
       break;
      }
   }
   rex_leave_crit_sect(&gOBEXCliCritSect);

   return me;
}


//=============================================================================
// 
//
//=============================================================================
static uint32 OEMOBEXCli_MtCmdToAeeEvt( bt_pf_cmd_type cmdType)
{
   switch (cmdType )
   {
      case BT_PF_CMD_GOEP_CLI_CONNECT:
         return AEEOBEX_EVT_CLI_CONN_RESP;
      case BT_PF_CMD_GOEP_CLI_DISCONNECT:
         return AEEOBEX_EVT_CLI_DISC_RESP;
      case BT_PF_CMD_GOEP_CLI_GET:
         return AEEOBEX_EVT_CLI_GET_RESP;
      case BT_PF_CMD_GOEP_CLI_PUT:
         return AEEOBEX_EVT_CLI_PUT_RESP;
      case BT_PF_CMD_GOEP_CLI_SET_PATH:
         return AEEOBEX_EVT_CLI_SETPATH_RESP;
      default:
         break;
   }
   return AEEOBEX_EVT_ERROR;
}
//=============================================================================
// 
//
//=============================================================================
boolean OEMOBEXCli_RecvHdrAvailable(OEMOBEXCli *me)
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
static OEMOBEXHdr* OEMOBEXCli_GetRecvHdr(OEMOBEXCli *me)
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
int OEMOBEXCli_GetBda(OEMOBEXCli *me, uint8* pBda)
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
int OEMOBEXCli_GetScn(OEMOBEXCli *me, uint32 * pScn)
{
   *pScn = me->connStruct.transport_conn.bt_conn.channel;
   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
int OEMOBEXCli_GetPacketSize(OEMOBEXCli *me, uint32 *pPacketSize)
{
   *pPacketSize = OEMOBEX_MAX_RECV_BUFFER_SIZE;//#check
   return AEE_SUCCESS;
}
#endif /* #if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP) */


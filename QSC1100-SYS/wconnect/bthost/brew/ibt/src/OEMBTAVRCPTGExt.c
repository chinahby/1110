/*=============================================================================
FILE:  OEMBTAVRCPTGExt.c

SERVICES:  OEM implementation of Bluetooth AVRCPTG EXT file

GENERAL DESCRIPTION:
        OEM implementation of Bluetooth AVRCPTG EXT file
   

===============================================================================
        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
 ----------   ---  -----------------------------------------------------------
 2008-09-16    hs   Fixed the compiler warnings.

 =========================================================================== */

#include "OEMFeatures.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV)

#ifdef FEATURE_BT_AVRCP_13

#include "OEMBTAVRCPTGExt_Def.h"   
#include "AEEAVRCPTG_Priv_Ext.h"
#include "AEEAVRCPTG_priv_ext_def.h"
#include "OEMBTUtils.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "btcomdef.h"
#include "bt.h"
#include "OEMHeap.h"
#include "AEEStdErr.h"
#include "AEE_OEMHeap.h"
#include "AEEShell.h"
#include "AEEBtDef.h"
#ifndef BT_SIMULATION
#include "msg.h"
#else
#include "btmsg.h"
#include "bt_sim_interface.h"
#endif

//=============================================================================
//                Advance declarations 
//
//=============================================================================
//

static void OEMBTAVRCPTGExt_EventCallback(
                                         bt_ev_msg_type* ev_msg_ptr);

static void OEMBTAVRCPTGExt_HandleCmdDone(
                                         bt_ev_gn_cmd_done_type *pCmdDn );

static int OEMBTAVRCPTGExt_CmdStatusToAeeResult(
                                               bt_cmd_status_type status );

static void OEMBTAVRCPTGExt_HandleCapCmd(
                                        bt_pf_ev_avrcp_capability_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleListAttrCmd(
                                             bt_pf_ev_avrcp_list_attributes_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleListAttrValCmd(
                                                bt_pf_ev_avrcp_list_attribute_values_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleGetAttrValCmd(
                                               bt_pf_ev_avrcp_get_attribute_values_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleSetAttrValCmd(
                                               bt_pf_ev_avrcp_set_attribute_values_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleGetAttrTxtCmd(
                                               bt_pf_ev_avrcp_get_attribute_setting_text_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleGetAttrValTxtCmd(
                                                  bt_pf_ev_avrcp_get_attribute_setting_value_text_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleInformCharsetCmd(
                                                  bt_pf_ev_avrcp_inform_character_set_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleBattStatusCmd(
                                               bt_pf_ev_avrcp_inform_battery_status_of_ct_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleGetElementAttrCmd(
                                                   bt_pf_ev_avrcp_get_element_attribute_command_ind_type* );

static void OEMBTAVRCPTGExt_HandlePlayStatusCmd(
                                               bt_pf_ev_avrcp_get_play_status_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleEventNotifCmd(
                                               bt_pf_ev_avrcp_event_notification_command_ind_type* );

static void OEMBTAVRCPTGExt_HandleGroupNavCmd(
                                             bt_pf_ev_avrcp_group_navigation_command_ind_type* );

static bt_cmd_status_type OEMBTAVRCPTGExt_AeeToPFStatus( BTResult result );

static uint32 OEMBTAVRCPTGExt_PfToAeeEventId(uint32 eventid);
//=============================================================================
// 
//
//=============================================================================


struct OEMBTAVRCPTGExt
{
   void                         *cbData;
   uint32                       uRef;     //reference count
   bt_app_id_type               appId;
   OEMBTAVRCPTG_ExtCmdCb        pAeeCb;
   BDAddress                    bdAddr;
   BTResult                     ret; 
};
//=============================================================================
// 
//
//=============================================================================


//=============================================================================
//  OEMBTAVRCPTGExt_Init
//
//=============================================================================
static const BDAddress OEMBTAVRCPTg_bdAddrNull = BT_PF_BD_ADDR_NULL;

static OEMBTAVRCPTGExt *me;

int OEMBTAVRCPTGExt_Init(OEMBTAVRCPTGExt **pOemobj, void* cbData, OEMBTAVRCPTG_ExtCmdCb aeeEvtCb)
{
   OEMBTAVRCPTGExt *pme;

   MSG_HIGH("OEMAVRCPTG Ext: init ->",0,0,0);

#ifdef BT_SIMULATION
   StartBT_StubThread();
   ShowBTDialog( PROFILE_AVRCP_RMT_CTL );
#endif

   pme = (OEMBTAVRCPTGExt*)OEM_Malloc(sizeof(OEMBTAVRCPTGExt));

   if (NULL == pme)
   {
      MSG_ERROR("OEMAVRCPTG Ext: could not alloc memory",0,0,0);
      return AEE_ENOMEMORY;
   }
   //since sys_malloc does not zero out the memory, need to initialize it.
   MEMSET(pme, 0, sizeof(OEMBTAVRCPTGExt));
   pme->uRef           = 1;
   pme->cbData         = cbData;
   pme->pAeeCb         = aeeEvtCb;
   MEMCPY( &pme->bdAddr, &OEMBTAVRCPTg_bdAddrNull, sizeof( pme->bdAddr ) );

   pme->appId =  bt_cmd_ec_get_app_id_and_register(OEMBTAVRCPTGExt_EventCallback);

   if (BT_APP_ID_NULL == pme->appId)
   {
      OEMBTAVRCPTGExt_Release(pme);
      MSG_ERROR("OEMAVRCPTG Ext: could not alloc app id",0,0,0);
      return AEE_EFAILED;
   }

   bt_cmd_pf_avrcp_metadata_enable(pme->appId);
   MSG_HIGH("OEMAVRCPTG Ext: init <-",0,0,0);
   *pOemobj = pme;
   me = pme;

   return AEE_SUCCESS;
}

//=============================================================================
//  OEMBTAVRCPTGExt_Release
//
//=============================================================================
int OEMBTAVRCPTGExt_Release(OEMBTAVRCPTGExt *me)
{
   uint32           uRef = --me->uRef;

   MSG_HIGH("OEMBT PBAP srv release uRef %x ->",uRef,0,0);

   if (0 == uRef)
   {
      if (BT_APP_ID_NULL != me->appId)
      {
         bt_cmd_ec_free_application_id(me->appId);
         me->appId = 0;
      }
      OEM_Free(me);
   }
   MSG_HIGH("OEMAVRCPTG Ext: init release uRef %x <-",uRef,0,0);
   return uRef;
}

//=============================================================================
//  OEMBTAVRCPTGExt_EventCallback
//
//=============================================================================

static void OEMBTAVRCPTGExt_EventCallback( struct bt_ev_msg_struct *ev_msg_ptr )
{
   switch ( ev_msg_ptr->ev_hdr.ev_type )
   {
   case BT_EV_GN_CMD_DONE:
      OEMBTAVRCPTGExt_HandleCmdDone( &ev_msg_ptr->ev_msg.ev_gn_cmd_done );
      break;

   case BT_EV_PF_AVRCP_CAPABILITY_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleCapCmd( &ev_msg_ptr->ev_msg.ev_avrcp_capability_command );
      break;

   case BT_EV_PF_AVRCP_LIST_ATTRIBUTES_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleListAttrCmd( &ev_msg_ptr->ev_msg.ev_avrcp_list_attributes_command );
      break;

   case BT_EV_PF_AVRCP_LIST_ATTRIBUTE_VALUES_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleListAttrValCmd( &ev_msg_ptr->ev_msg.ev_avrcp_list_attribute_values_command );
      break;

   case BT_EV_PF_AVRCP_GET_ATTRIBUTE_VALUES_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleGetAttrValCmd( &ev_msg_ptr->ev_msg.ev_avrcp_get_attribute_values_command );
      break;

   case BT_EV_PF_AVRCP_SET_ATTRIBUTE_VALUES_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleSetAttrValCmd( &ev_msg_ptr->ev_msg.ev_avrcp_set_attribute_values_command );
      break;

   case BT_EV_PF_AVRCP_GET_ATTRIBUTE_SETTING_TEXT_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleGetAttrTxtCmd( &ev_msg_ptr->ev_msg.ev_get_attribute_setting_text_command );
      break;

   case BT_EV_PF_AVRCP_GET_ATTRIBUTE_SETTING_VALUE_TEXT_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleGetAttrValTxtCmd( &ev_msg_ptr->ev_msg.ev_avrcp_get_attribute_setting_value_text_command );
      break;

   case BT_EV_PF_AVRCP_INFORM_CHARACTER_SET_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleInformCharsetCmd( &ev_msg_ptr->ev_msg.ev_avrcp_inform_character_set_command );
      break;

   case BT_EV_PF_AVRCP_INFORM_BATTERY_STATUS_OF_CT_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleBattStatusCmd( &ev_msg_ptr->ev_msg.ev_avrcp_inform_battery_status_of_ct_command );
      break;

   case BT_EV_PF_AVRCP_GET_ELEMENT_ATTRIBUTE_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleGetElementAttrCmd( &ev_msg_ptr->ev_msg.ev_avrcp_get_element_attribute_command );
      break;

   case BT_EV_PF_AVRCP_GET_PLAY_STATUS_COMMAND_IND:
      OEMBTAVRCPTGExt_HandlePlayStatusCmd( &ev_msg_ptr->ev_msg.ev_avrcp_get_play_status_command );
      break;

   case BT_EV_PF_AVRCP_EVENT_NOTIFICATION_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleEventNotifCmd( &ev_msg_ptr->ev_msg.ev_avrcp_event_notification_command );
      break;

   case BT_EV_PF_AVRCP_GROUP_NAVIGATION_COMMAND_IND:
      OEMBTAVRCPTGExt_HandleGroupNavCmd( &ev_msg_ptr->ev_msg.ev_avrcp_group_navigation_command );
      break;

   default:
      MSG_ERROR( "OOEMAVRCPTG Ext: Invalid event = %x",
                 ev_msg_ptr->ev_hdr.ev_type,0,0 );

   }
}

//=============================================================================
// OEMBTAVRCPTGExt_AeeToPFStatus
//
//=============================================================================
static bt_cmd_status_type 
OEMBTAVRCPTGExt_AeeToPFStatus(AEEBTAVRCPTgCmdResponseStatus stat)
{
   switch (stat)
   {
   case AEEBTAVRCPTG_CMD_RESPONSE_INVALID_PDU:
      return BT_CS_PF_AVRCP_RESPONSE_INVALID_PDU;

   case AEEBTAVRCPTG_CMD_RESPONSE_INVALID_PARAMETER:
      return BT_CS_PF_AVRCP_RESPONSE_INVALID_PARAMETER;

   case AEEBTAVRCPTG_CMD_RESPONSE_PARAMETER_NOT_FOUND:
      return BT_CS_PF_AVRCP_RESPONSE_PARAMETER_NOT_FOUND;

   case AEEBTAVRCPTG_CMD_RESPONSE_INTERNAL_ERROR:
      return BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR;

   case AEEBTAVRCPTG_CMD_RESPONSE_NOT_IMPLEMENTED:
      return BT_CS_PF_AVRCP_NOT_IMPLEMENTED;

   case AEEBTAVRCPTG_CMD_RESPONSE_REJECTED:
      return BT_CS_PF_AVRCP_REJECTED;

   case AEEBTAVRCPTG_CMD_RESPONSE_ACCEPTED:
   case AEEBTAVRCPTG_CMD_RESPONSE_IN_TRANSITION:
   case AEEBTAVRCPTG_CMD_RESPONSE_IMPLEMENTED:
   case AEEBTAVRCPTG_CMD_RESPONSE_CHANGED:
      break;
   default:
      return BT_CS_PF_AVRCP_REJECTED;
   }
   return BT_CS_GN_SUCCESS;
}

//=============================================================================
// OEMBTAVRCPTGExt_AeeToPfEventId
//
//=============================================================================
static uint8 OEMBTAVRCPTGExt_AeeToPfEventId(uint32 event)
{
   uint8 pfEvtIndex = 1;
   while ( !(event & 0x0001) )
   {
      event = event >> 1;
      pfEvtIndex++;
   }
   return pfEvtIndex;
}

//=============================================================================
//   OEMBTAVRCPTGExt_PfToAeeEventId
//
//=============================================================================

static uint32 OEMBTAVRCPTGExt_PfToAeeEventId(uint32 eventid)
{
   uint32 event = 1;
   event = (event << (eventid - 1));
   return event;
}
//=============================================================================
//  OEMBTAVRCPTGExt_CmdStatusToAeeResult 
//
//=============================================================================
static int OEMBTAVRCPTGExt_CmdStatusToAeeResult(bt_cmd_status_type status)
{
   switch ( status )
   {
   case BT_CS_GN_SUCCESS:
   case BT_CS_GN_PENDING:
      return AEE_SUCCESS;
   case BT_CS_GN_CMD_Q_FULL:
   case BT_CS_PF_OUT_OF_MEMORY:
      return AEE_ENOMEMORY;
   case BT_CS_PF_INVALID_PARAMETERS:
      return AEE_EBADPARM;
   default:
      return AEE_EFAILED;
   }
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleCapCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleCmdDone( bt_ev_gn_cmd_done_type *pCmdDn )
{
   AEEBTAVRCPTgExtCmdInfo  cmdInfo;

   MSG_HIGH("OOEMAVRCPTG Ext: HandleCmdDone stat=%x cmd=%x",
            pCmdDn->cmd_status, pCmdDn->cmd_type,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));
   switch ( pCmdDn->cmd_type )
   {
   case BT_PF_CMD_AVRCP_METADATA_ENABLE:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_EVENTID_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_COMPID_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_LIST_ATTRID_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_LIST_ATTRVALUE_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_GET_ATTRVALUE_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_SET_ATTRVALUE_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_GET_ATTR_SETTING_TEXT_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_GET_ATTR_SETTING_VALUE_TEXT_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_PLAY_STATUS_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_GET_ELEMENTS_ATTR_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_REG_NOTIFICATION_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_GROUP_NAV_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   case BT_PF_CMD_AVRCP_SIMPLE_META_REPLY:
      cmdInfo.cmdId.cResult = OEMBT_CmdStatus2Result( pCmdDn->cmd_status );
      break;
   default:
      MSG_HIGH("OOEMAVRCPTG Ext: Invalid cmd done stat=%x cmd=%x",
               pCmdDn->cmd_status, pCmdDn->cmd_type,0);

   }
}
//=============================================================================
//  OEMBTAVRCPTGExt_HandleCapCmd
//
//=============================================================================
static void 
OEMBTAVRCPTGExt_HandleCapCmd(
                            bt_pf_ev_avrcp_capability_command_ind_type *pCapCmdInd )
{
   AEEBTAVRCPTgExtCmdInfo   cmdInfo;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleCapCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pCapCmdInd->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_CAPABILITY_COMMAND_IND;
   cmdInfo.cmdId.txn = pCapCmdInd->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   cmdInfo.pduInfo.capId = ( AEEBTAVRCPTgCapId )pCapCmdInd->cap_id;

   me->pAeeCb( me->cbData,&cmdInfo );
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleListAttrCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleListAttrCmd(
                                             bt_pf_ev_avrcp_list_attributes_command_ind_type *pListAttrInd )
{
   AEEBTAVRCPTgExtCmdInfo       cmdInfo;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleListAttrCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pListAttrInd->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_LIST_ATTRIBUTES_COMMAND_IND;
   cmdInfo.cmdId.txn = pListAttrInd->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));

   me->pAeeCb( me->cbData,&cmdInfo );
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleListAttrValCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleListAttrValCmd(
                                                bt_pf_ev_avrcp_list_attribute_values_command_ind_type *pListAttrVal)
{
   AEEBTAVRCPTgExtCmdInfo  cmdInfo;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleListAttrValCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pListAttrVal->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_LIST_ATTRIBUTE_VALUES_COMMAND_IND;
   cmdInfo.cmdId.txn = pListAttrVal->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   cmdInfo.attrId = pListAttrVal->attr_id;

   me->pAeeCb( me->cbData, &cmdInfo );

}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleGetAttrValCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleGetAttrValCmd(
                                               bt_pf_ev_avrcp_get_attribute_values_command_ind_type *pGetAttrVal)
{
   AEEBTAVRCPTgExtCmdInfo       cmdInfo;
   uint32                       i;
   bt_pf_avrcp_pdu_id_type      pduId; 

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleGetAttrValCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pGetAttrVal->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_GET_ATTRIBUTE_VALUES_COMMAND_IND;
   cmdInfo.cmdId.txn = pGetAttrVal->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   if ( pGetAttrVal->attr_count != 0 )
   {
      cmdInfo.pduInfo.pduData.count = pGetAttrVal->attr_count;

      if ( cmdInfo.pduInfo.pduData.count > AEEBTAVRCPTG_MAX_NUM_OF_VALS )
      {
         cmdInfo.pduInfo.pduData.count = AEEBTAVRCPTG_MAX_NUM_OF_VALS;
      }

      for (i = 0; i < cmdInfo.pduInfo.pduData.count; i++)
      {
         cmdInfo.pduInfo.pduData.val[i] = pGetAttrVal->attr_ids[i];
      }

   } 
   else
   {
      MSG_ERROR("OOEMAVRCPTG Ext: HandleGetAttrValCmd. Invalid count ",
                0,0,0);
      pduId = BT_PF_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE;
      bt_cmd_pf_avrcp_send_simple_meta_reply( me->appId,
                                              cmdInfo.cmdId.txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return; 
   }
   me->pAeeCb( me->cbData, &cmdInfo );
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleSetAttrValCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleSetAttrValCmd(
                                               bt_pf_ev_avrcp_set_attribute_values_command_ind_type *pSetAttrVal)
{
   AEEBTAVRCPTgExtCmdInfo       cmdInfo;
   uint32                       i;
   bt_pf_avrcp_pdu_id_type      pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleSetAttrValCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pSetAttrVal->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_SET_ATTRIBUTE_VALUES_COMMAND_IND;
   cmdInfo.cmdId.txn = pSetAttrVal->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   if ( pSetAttrVal->attr_id_count != 0 )
   {
      cmdInfo.pduInfo.pduData.count = pSetAttrVal->attr_id_count;

      if ( ( cmdInfo.pduInfo.pduData.count * 2 ) > AEEBTAVRCPTG_MAX_NUM_OF_VALS )
      {
         cmdInfo.pduInfo.pduData.count = ( AEEBTAVRCPTG_MAX_NUM_OF_VALS / 2 );
      }

      for (i = 0; i < (cmdInfo.pduInfo.pduData.count* 2); i++)
      {
         *( i + ( uint32* )cmdInfo.pduInfo.pduData.val ) = pSetAttrVal->attrs[i];
      }
   } 
   else
   {
      MSG_ERROR("OOEMAVRCPTG Ext: HandleSetAttrValCmd. Invalid count ",
                0,0,0);
      pduId = BT_PF_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE;
      bt_cmd_pf_avrcp_send_simple_meta_reply( me->appId,
                                              cmdInfo.cmdId.txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return; 
   }
   me->pAeeCb( me->cbData, &cmdInfo );
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleGetAttrTxtCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleGetAttrTxtCmd(
                                               bt_pf_ev_avrcp_get_attribute_setting_text_command_ind_type *pAttrTxt)
{
   AEEBTAVRCPTgExtCmdInfo       cmdInfo;
   uint32                       i;
   bt_pf_avrcp_pdu_id_type      pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleGetAttrTxtCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pAttrTxt->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_GET_ATTRIBUTE_SETTING_TEXT_COMMAND_IND;
   cmdInfo.cmdId.txn = pAttrTxt->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   if ( pAttrTxt->attr_count != 0 )
   {
      cmdInfo.pduInfo.pduData.count = pAttrTxt->attr_count;

      if ( cmdInfo.pduInfo.pduData.count > AEEBTAVRCPTG_MAX_NUM_OF_VALS )
      {
         cmdInfo.pduInfo.pduData.count = AEEBTAVRCPTG_MAX_NUM_OF_VALS;
      }

      for (i = 0; i < cmdInfo.pduInfo.pduData.count; i++)
      {
         *( i + (uint32* )cmdInfo.pduInfo.pduData.val) = pAttrTxt->attr_ids[i];
      }
   } 
   else
   {
      MSG_ERROR("OOEMAVRCPTG Ext: HandleGetAttrTxtCmd. Invalid count ",
                0,0,0);
      pduId = BT_PF_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;
      bt_cmd_pf_avrcp_send_simple_meta_reply( me->appId,
                                              cmdInfo.cmdId.txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return; 
   }
   me->pAeeCb( me->cbData, &cmdInfo );
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleGetAttrValTxtCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleGetAttrValTxtCmd(
                                                  bt_pf_ev_avrcp_get_attribute_setting_value_text_command_ind_type* pValTxt)
{
   AEEBTAVRCPTgExtCmdInfo  cmdInfo;
   uint32                       i;
   bt_pf_avrcp_pdu_id_type      pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleGetAttrValTxtCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pValTxt->bd_addr));

   cmdInfo.cmdId.cmdPduId = 
   AEEBTAVRCPTG_GET_ATTRIBUTE_SETTING_VALUE_TEXT_COMMAND_IND;
   cmdInfo.cmdId.txn = pValTxt->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   cmdInfo.attrId = pValTxt->attr_id;
   if ( pValTxt->attr_val_count != 0 )
   {
      cmdInfo.pduInfo.pduData.count = pValTxt->attr_val_count;

      if ( cmdInfo.pduInfo.pduData.count > AEEBTAVRCPTG_MAX_NUM_OF_VALS )
      {
         cmdInfo.pduInfo.pduData.count = AEEBTAVRCPTG_MAX_NUM_OF_VALS;
      }

      for (i = 0; i < cmdInfo.pduInfo.pduData.count; i++)
      {
         *( i + (uint32* )cmdInfo.pduInfo.pduData.val) = pValTxt->attr_vals[i];
      }
   } 
   else
   {

      pduId = BT_PF_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT;
      bt_cmd_pf_avrcp_send_simple_meta_reply( me->appId,
                                              cmdInfo.cmdId.txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return;
   }
   me->pAeeCb( me->cbData, &cmdInfo );
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleInformCharsetCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleInformCharsetCmd(
                                                  bt_pf_ev_avrcp_inform_character_set_command_ind_type* pCharset)
{
   AEEBTAVRCPTgExtCmdInfo  cmdInfo;
   uint32                       i;
   bt_pf_avrcp_pdu_id_type      pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleInformCharsetCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pCharset->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_INFORM_CHARACTER_SET_COMMAND_IND;

   cmdInfo.cmdId.txn = pCharset->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   if ( pCharset->charset_count != 0 )
   {
      cmdInfo.pduInfo.pduData.count = pCharset->charset_count;

      if ( cmdInfo.pduInfo.pduData.count > AEEBTAVRCPTG_MAX_NUM_OF_VALS )
      {
         cmdInfo.pduInfo.pduData.count = AEEBTAVRCPTG_MAX_NUM_OF_VALS;
      }

      for (i = 0; i < cmdInfo.pduInfo.pduData.count; i++)
      {
         *( i + (uint32* )cmdInfo.pduInfo.pduData.val) = pCharset->charset_ids[i];
      }
   } 
   else
   {
      MSG_ERROR("OEMAVRCPTG Ext: HandleInformCharsetCmd.Invalid count ",
                0,0,0);
      pduId = BT_PF_AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET;
      bt_cmd_pf_avrcp_send_simple_meta_reply( me->appId,
                                              cmdInfo.cmdId.txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return;
   }
   me->pAeeCb( me->cbData, &cmdInfo );  

}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleBattStatusCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleBattStatusCmd(
                                               bt_pf_ev_avrcp_inform_battery_status_of_ct_command_ind_type *pBatSt)
{
   AEEBTAVRCPTgExtCmdInfo  cmdInfo;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleBattStatusCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pBatSt->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_INFORM_BATTERY_STATUS_OF_CT_COMMAND_IND;
   cmdInfo.cmdId.txn = pBatSt->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   cmdInfo.pduInfo.ctBattStatus = pBatSt->batt_status;

   me->pAeeCb( me->cbData, &cmdInfo );
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleGetElementAttrCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleGetElementAttrCmd(
                                                   bt_pf_ev_avrcp_get_element_attribute_command_ind_type* pElemAttr)
{
   AEEBTAVRCPTgExtCmdInfo  cmdInfo;
   uint32                       i;
   bt_pf_avrcp_pdu_id_type      pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleGetElementAttrCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pElemAttr->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_GET_ELEMENT_ATTRIBUTE_COMMAND_IND;

   cmdInfo.cmdId.txn = pElemAttr->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   MEMCPY( cmdInfo.trackIndex, pElemAttr->media_element_id,
           AEEBTAVRCPTG_SIZE_OF_TRACK_INDEX );
   if ( pElemAttr->elements_count != 0 )
   {
      cmdInfo.pduInfo.pduData.count = pElemAttr->elements_count;
      if ( cmdInfo.pduInfo.pduData.count > AEEBTAVRCPTG_MAX_NUM_OF_VALS )
      {
         cmdInfo.pduInfo.pduData.count = AEEBTAVRCPTG_MAX_NUM_OF_VALS;
      }

      for (i = 0; i < cmdInfo.pduInfo.pduData.count; i++)
      {
         *( i + cmdInfo.pduInfo.pduData.val) = pElemAttr->media_attrbutes[i];
      }
   } 
   else
   {
      MSG_ERROR("OOEMAVRCPTG Ext: HandleGetElementAttrCmd. Invalid count ",
                0,0,0);
      pduId = BT_PF_AVRCP_GET_ELEMENT_ATTRIBUTES;
      bt_cmd_pf_avrcp_send_simple_meta_reply( me->appId,
                                              cmdInfo.cmdId.txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return; 
   }
   me->pAeeCb( me->cbData, &cmdInfo );  
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandlePlayStatusCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandlePlayStatusCmd(
                                               bt_pf_ev_avrcp_get_play_status_command_ind_type* pPlayStat)
{
   AEEBTAVRCPTgExtCmdInfo  cmdInfo;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandlePlayStatusCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pPlayStat->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_GET_PLAY_STATUS_COMMAND_IND;
   cmdInfo.cmdId.txn = pPlayStat->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));

   me->pAeeCb( me->cbData, &cmdInfo );
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleEventNotifCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleEventNotifCmd(
                                               bt_pf_ev_avrcp_event_notification_command_ind_type *pEvtNotif )
{
   AEEBTAVRCPTgExtCmdInfo  cmdInfo;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleEventNotifCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pEvtNotif->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_EVENT_NOTIFICATION_COMMAND_IND;
   cmdInfo.cmdId.txn = pEvtNotif->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   cmdInfo.pduInfo.notifyInfo.eventId = OEMBTAVRCPTGExt_PfToAeeEventId( pEvtNotif->notif_event_id );
   cmdInfo.pduInfo.notifyInfo.playbackInterval = pEvtNotif->playback_interval;

   me->pAeeCb( me->cbData, &cmdInfo );
}

//=============================================================================
//  OEMBTAVRCPTGExt_HandleGroupNavCmd
//
//=============================================================================
static void OEMBTAVRCPTGExt_HandleGroupNavCmd(
                                             bt_pf_ev_avrcp_group_navigation_command_ind_type* pGpNavCmd)
{
   AEEBTAVRCPTgExtCmdInfo  cmdInfo;

   MSG_HIGH("OOEMAVRCPTG Ext: OEMBTAVRCPTGExt_HandleGroupNavCmd ",
            0,0,0);

   MEMSET((void*)&cmdInfo, 0 ,sizeof(cmdInfo));

   me->bdAddr = *((BDAddress*)(&pGpNavCmd->bd_addr));

   cmdInfo.cmdId.cmdPduId = AEEBTAVRCPTG_GROUP_NAVIGATION_COMMAND_IND;
   cmdInfo.cmdId.txn = pGpNavCmd->txn;
   MEMCPY(&(cmdInfo.cmdId.bdAddr), &(me->bdAddr), sizeof (BDAddress));
   cmdInfo.pduInfo.grpNavInfo.grpNavCmd = 
      ( AEEBTAVRCPTgGroupNavCommandEnum )pGpNavCmd->vendor_unique;
   cmdInfo.pduInfo.grpNavInfo.buttPress = pGpNavCmd->button_press;

   me->pAeeCb( me->cbData, &cmdInfo );
}

//=============================================================================
//  
//       Reply to the AVRCP Metadata transfer commands
// 
//=============================================================================

//=============================================================================
//  OEMBTAVRCPTGExt_SendEventIdReply
//=============================================================================
int OEMBTAVRCPTGExt_SendEventIdReply(OEMBTAVRCPTGExt            *pMeExt,
                                     uint8                       txn,
                                     uint32                     eventCount,
                                     AEEBTAVRCPTgMediaEventMask *eventIds)
{
   uint8                                     *events;
   uint32                                    i;
   bt_cmd_status_type                        stat; 
   bt_pf_avrcp_pdu_id_type                   pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: SendEventIdReply ",0,0,0);

   events = (uint8 *) AV_Malloc(eventCount);
   if ( ( NULL == events ) || ( 0 == eventCount ) )
   {
      MSG_ERROR("OOEMAVRCPTG Ext: SendEventIdReply. Mem alloc fail ",
                0,0,0);
      pduId = BT_PF_AVRCP_GET_CAPABILITIES;
      bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                              txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return AEE_ENOMEMORY;
   }
   MEMSET( (void *)events, 0, eventCount);

   for (i = 0; i < eventCount; i++ )
   {
      events[i] = OEMBTAVRCPTGExt_AeeToPfEventId( eventIds[i] ); 
   }

   stat = bt_cmd_pf_avrcp_send_eventid_reply (pMeExt->appId,
                                              txn,
                                              (uint8)eventCount,
                                              (bt_pf_avrcp_metadata_transfer_events_type *)events );

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );

}

//=============================================================================
//  OEMBTAVRCPTGExt_SendCompIdReply
//=============================================================================
int OEMBTAVRCPTGExt_SendCompIdReply( OEMBTAVRCPTGExt *pMeExt,
                                     uint8            txn,
                                     uint32           compCount,
                                     uint32           *comp )

{
   uint32                                    *compIds;
   uint32                                    i;
   bt_cmd_status_type                        stat; 
   bt_pf_avrcp_pdu_id_type                   pduId;


   compIds = AV_Malloc(compCount * sizeof(uint32));

   MSG_HIGH("OOEMAVRCPTG Ext: SendCompIdReply count = %d ",compCount,0,0);

   if ( ( NULL == compIds )  || ( 0 == compCount ) )
   {
      MSG_ERROR("OOEMAVRCPTG Ext: SendCompIdReply. Mem alloc fail ",
                0,0,0);
      pduId = BT_PF_AVRCP_GET_CAPABILITIES;

      bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                              txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return AEE_ENOMEMORY; 
   }

   MEMSET( (void *)compIds, 0, (compCount * sizeof(uint32)) );

   for (i = 0; i < compCount; i++ )
   {
      compIds[i] = comp[i];
   }
   stat = bt_cmd_pf_avrcp_send_compid_reply (pMeExt->appId,
                                             txn,
                                             (uint8)compCount,
                                             compIds);

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );
}

//=============================================================================
//  OEMBTAVRCPTGExt_ListAttrIdReply
//=============================================================================
int OEMBTAVRCPTGExt_ListAttrIdReply(OEMBTAVRCPTGExt             *pMeExt,
                                    uint8                       txn,
                                    uint32                      attrCount,
                                    AEEBTAVRCPTgPlayerAppAttrs  *attrIds)

{
   uint8                                    *attrs;
   uint32                                    i;
   bt_cmd_status_type                        stat; 
   bt_pf_avrcp_pdu_id_type                   pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: ListAttrIdReply ",0,0,0);

   attrs = (uint8 *) AV_Malloc( attrCount );

   if ( ( NULL == attrIds )  || ( 0 == attrCount ) )
   {
      MSG_ERROR("OOEMAVRCPTG Ext: ListAttrIdReply. Mem alloc fail ",
                0,0,0);
      pduId = BT_PF_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES;
      bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                              txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return AEE_ENOMEMORY;
   }

   MEMSET( (void *)attrs, 0, attrCount );

   for (i = 0; i < attrCount; i++ )
   {
      attrs[i] = ( uint8 )attrIds[i]; 
   }

   stat = bt_cmd_pf_avrcp_send_list_attrid_reply (pMeExt->appId,
                                                  txn,
                                                  (uint8)attrCount,
                                                  (bt_pf_avrcp_player_app_settings_type *)attrs );

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );
}

//=============================================================================
//  OEMBTAVRCPTGExt_ListAttrValReply
//=============================================================================
int OEMBTAVRCPTGExt_ListAttrValReply(OEMBTAVRCPTGExt             *pMeExt,
                                     uint8                       txn,
                                     AEEBTAVRCPTgPlayerAppAttrs  attrId,
                                     uint32                      attrValCount,
                                     uint32                      *attrValue)

{
   uint8                                     *attrVals;
   uint32                                    i;
   bt_cmd_status_type                        stat; 
   bt_pf_avrcp_pdu_id_type                   pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: ListAttrValReply ",0,0,0);

   attrVals = (uint8 *) AV_Malloc(attrValCount);
   if ( ( NULL == attrVals ) || ( 0 == attrValCount ) )
   {
      MSG_ERROR("OOEMAVRCPTG Ext: ListAttrValReply. Mem alloc fail ",
                0,0,0);
      pduId = BT_PF_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES;
      bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                              txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return AEE_ENOMEMORY;
   }
   MEMSET( (void *)attrVals, 0, attrValCount );

   for (i = 0; i < attrValCount; i++ )
   {
      attrVals[i] = (uint8) attrValue[i]; 
   }

   stat = bt_cmd_pf_avrcp_send_list_attrvalue_reply (pMeExt->appId,
                                                     txn,
                                                     (bt_pf_avrcp_player_app_settings_type) attrId,
                                                     (uint8)attrValCount,
                                                     (uint8*)attrVals );

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );
}

//=============================================================================
//  OEMBTAVRCPTGExt_GetAttrValReply
//=============================================================================
int OEMBTAVRCPTGExt_GetAttrValReply(OEMBTAVRCPTGExt             *pMeExt,
                                    uint8                       txn,
                                    uint32                      attrCount,
                                    uint32                      *attrIds)

{
   uint8                                     *attrs;
   uint32                                    i;
   bt_cmd_status_type                        stat; 
   bt_pf_avrcp_pdu_id_type                   pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: GetAttrValReply ",0,0,0);

   attrs = (uint8 *) AV_Malloc(attrCount * 2);
   if ( ( NULL == attrIds ) || ( 0 == attrCount ) )
   {
      MSG_ERROR("OOEMAVRCPTG Ext: GetAttrValReply. Mem alloc fail ",
                0,0,0);
      pduId = BT_PF_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE;
      bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                              txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return AEE_ENOMEMORY;
   }
   MEMSET( (void *)attrs, 0, (attrCount * 2) );

   for (i = 0; i < (attrCount * 2) ; i++ )
   {
      attrs[i] = (uint8) attrIds[i]; 
   }

   stat = bt_cmd_pf_avrcp_send_get_attr_value_reply (pMeExt->appId,
                                                     txn,
                                                     (uint8)attrCount,
                                                     (uint8*)attrs );

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );
}

//=============================================================================
//  OEMBTAVRCPTGExt_GetAttrSettingTxtReply
//=============================================================================
int OEMBTAVRCPTGExt_GetAttrSettingTxtReply(OEMBTAVRCPTGExt            *pMeExt,
                                           uint8                      txn,
                                           uint32                     attrCount,
                                           AEEBTAVRCPTgItemTxt       *attrIdTxt) 

{
   bt_pf_avrcp_attr_text_type                *attrsInfoText;
   uint32                                    i;
   uint32                                    j;
   bt_cmd_status_type                        stat; 
   bt_pf_avrcp_pdu_id_type                   pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: GetAttrSettingTxtReply ",0,0,0);

   attrsInfoText = (bt_pf_avrcp_attr_text_type *) AV_Malloc( attrCount * 
                                                              sizeof( bt_pf_avrcp_attr_text_type ) );
   if ( ( NULL == attrsInfoText ) || ( 0 == attrCount ) )
   {
      MSG_ERROR("OOEMAVRCPTG Ext: GetAttrSettingTxtReply. Mem alloc fail ",
                0,0,0);
      pduId = BT_PF_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;
      bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                              txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return AEE_ENOMEMORY;
   }

   MEMSET((void*)attrsInfoText, 0 , 
          ( attrCount * sizeof( bt_pf_avrcp_attr_text_type ) ) );

   for (i = 0; i <  attrCount ; i++ )
   {
      attrsInfoText[i].item_id      = (uint8)attrIdTxt[i].itemId; 
      attrsInfoText[i].item_charset = (uint16)attrIdTxt[i].charset;
      attrsInfoText[i].item_string_val.num_of_bytes = attrIdTxt[i].itemTxtLen;
      attrsInfoText[i].item_string_val.p = AV_Malloc(attrIdTxt[i].itemTxtLen);
      if ( NULL == attrsInfoText[i].item_string_val.p )
      {
         MSG_ERROR("OOEMAVRCPTG Ext: GetAttrSettingTxtReply. Mem alloc fail ",
                   0,0,0);
         for (j = 0; j <  i ; j++ )
         {
            AV_Free( attrsInfoText[j].item_string_val.p );
         }
         AV_Free((void *)attrsInfoText);

         pduId = BT_PF_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;
         bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                                 txn,
                                                 pduId, 
                                                 BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
         return AEE_ENOMEMORY;
      }
      MEMCPY( attrsInfoText[i].item_string_val.p, attrIdTxt[i].itemTxt,
              attrIdTxt[i].itemTxtLen); 

   }

   stat = bt_cmd_pf_avrcp_send_get_attr_setting_text_reply (pMeExt->appId,
                                                            txn,
                                                            (uint8) attrCount,
                                                            attrsInfoText);

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );
}

//=============================================================================
//  OEMBTAVRCPTGExt_GetAttrSettingValTxtReply
//=============================================================================
int OEMBTAVRCPTGExt_GetAttrSettingValTxtReply(OEMBTAVRCPTGExt            *pMeExt,
                                              uint8                      txn,
                                              uint32                     valCount,
                                              AEEBTAVRCPTgItemTxt       *attrValTxt)
{ 
   bt_pf_avrcp_attr_text_type                *attrValsText;
   uint32                                    i;
   uint32                                    j;
   bt_cmd_status_type                        stat; 
   bt_pf_avrcp_pdu_id_type                   pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: GetAttrSettingValTxtReply ",0,0,0);

   attrValsText = (bt_pf_avrcp_attr_text_type *) AV_Malloc( valCount * 
                                                             sizeof( bt_pf_avrcp_attr_text_type ) );
   if ( ( NULL == attrValsText ) || ( 0 == valCount ) )
   {
      MSG_ERROR("OOEMAVRCPTG Ext: GetAttrSettingValTxtReply. Mem alloc fail ",
                0,0,0);
      pduId = BT_PF_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;
      bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                              txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return AEE_ENOMEMORY;
   }
   MEMSET((void*)attrValsText, 0 , 
          (valCount * sizeof( bt_pf_avrcp_attr_text_type )) );

   for (i = 0; i <  valCount ; i++ )
   {
      attrValsText[i].item_id      = (uint8)attrValTxt[i].itemId; 
      attrValsText[i].item_charset = (uint16)attrValTxt[i].charset;
      attrValsText[i].item_string_val.num_of_bytes = attrValTxt[i].itemTxtLen;
      attrValsText[i].item_string_val.p = AV_Malloc(attrValTxt[i].itemTxtLen);
      if ( NULL == attrValsText[i].item_string_val.p )
      {
         MSG_ERROR("OOEMAVRCPTG Ext: GetAttrSettingValTxtReply. Mem alloc fail ",
                   0,0,0);
         for (j = 0; j <  i ; j++ )
         {
            AV_Free( attrValsText[j].item_string_val.p );
         }
         AV_Free((void *)attrValsText);

         pduId = BT_PF_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT;
         bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                                 txn,
                                                 pduId, 
                                                 BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
         return AEE_ENOMEMORY;
      }
      MEMCPY( attrValsText[i].item_string_val.p, attrValTxt[i].itemTxt,
              attrValTxt[i].itemTxtLen); 

   }

   stat = bt_cmd_pf_avrcp_send_get_attr_setting_value_text_reply (pMeExt->appId,
                                                                  txn,
                                                                  (uint8)valCount,
                                                                  attrValsText);

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );

}

//=============================================================================
//  OEMBTAVRCPTGExt_SendPlayStatusReply
//=============================================================================
int OEMBTAVRCPTGExt_SendPlayStatusReply(OEMBTAVRCPTGExt            *pMeExt,
                                        uint8                      txn,
                                        uint32                     songLen,
                                        uint32                     songPos,
                                        AEEBTAVRCPTgPlayStatus     playStatus)
{
   bt_cmd_status_type                        stat; 
   bt_pf_avrcp_play_status_type              playStat;

   MSG_HIGH("OOEMAVRCPTG Ext: SendPlayStatusReply ",0,0,0);
   MEMSET((void*)&playStat, 0 , sizeof( bt_pf_avrcp_play_status_type ) );

   playStat.song_length   =  songLen;
   playStat.song_position =  songPos;
   playStat.status        =  (uint8)playStatus;

   stat = bt_cmd_pf_avrcp_send_play_status_reply(pMeExt->appId,
                                                 txn,
                                                 &playStat);

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );
}

//=============================================================================
//  OEMBTAVRCPTGExt_SendGetElementAttrReply
//=============================================================================
int OEMBTAVRCPTGExt_SendGetElementAttrReply(OEMBTAVRCPTGExt            *pMeExt,
                                            uint8                       txn,
                                            uint32                      mediaAttrCount,
                                            AEEBTAVRCPTgItemTxt        *mediaAttr)
{ 
   bt_pf_avrcp_element_attr_type             *mediaAttrInfo;
   uint32                                    i;
   uint32                                    j;
   bt_cmd_status_type                        stat; 
   bt_pf_avrcp_pdu_id_type                   pduId;

   MSG_HIGH("OOEMAVRCPTG Ext: SendGetElementAttrReply count = %x",mediaAttrCount,0,0);

   mediaAttrInfo = (bt_pf_avrcp_element_attr_type *) AV_Malloc( mediaAttrCount * 
                                                                 sizeof( bt_pf_avrcp_element_attr_type) );
   if ( ( NULL == mediaAttrInfo ) || ( 0 == mediaAttrCount ) )
   {
      MSG_ERROR("OOEMAVRCPTG Ext: SendGetElementAttrReply. Mem alloc fail ",
                0,0,0);
      pduId = BT_PF_AVRCP_GET_ELEMENT_ATTRIBUTES;
      bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                              txn,
                                              pduId, 
                                              BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
      return AEE_ENOMEMORY;
   }
   MEMSET((void*)mediaAttrInfo, 0 , 
          (mediaAttrCount * sizeof( bt_pf_avrcp_element_attr_type ) ) );

   for (i = 0; i <  mediaAttrCount; i++ )
   {
      mediaAttrInfo[i].media_attr_id      = (uint8)mediaAttr[i].itemId; 
      mediaAttrInfo[i].char_set = (uint16)mediaAttr[i].charset;
      switch( mediaAttrInfo[i].media_attr_id )
      {
      case AEEBTAVRCPTG_MEDIA_ATTR_ILLEGAL:
         break;
 
      case AEEBTAVRCPTG_MEDIA_ATTR_TITLE:
      case AEEBTAVRCPTG_MEDIA_ATTR_ARTIST:
      case AEEBTAVRCPTG_MEDIA_ATTR_ALBUM:
      case AEEBTAVRCPTG_MEDIA_ATTR_GENRE:
         mediaAttrInfo[i].char_set = (uint16)mediaAttr[i].charset;
         break;

      case AEEBTAVRCPTG_MEDIA_ATTR_TRACK_NUMBER:
      case AEEBTAVRCPTG_MEDIA_ATTR_TOTAL_TRACK_NUMBER:
      case AEEBTAVRCPTG_MEDIA_ATTR_PLAYING_TIME:
        /* Numeric values must be in ASCII txt but spec only mandates UTF8 
         * so we send numeric values in UTF8 charset since CT might not 
         * support ASCII txt 
         */
         mediaAttrInfo[i].char_set = BT_PF_AVRCP_LANG_ENCODING_UTF8;
         break;
      }
      mediaAttrInfo[i].media_attr_val.num_of_bytes = mediaAttr[i].itemTxtLen;
      mediaAttrInfo[i].media_attr_val.p = AV_Malloc(mediaAttr[i].itemTxtLen);
      if ( NULL == mediaAttrInfo[i].media_attr_val.p )
      {
         MSG_ERROR("OOEMAVRCPTG Ext: SendGetElementAttrReply. Mem alloc fail ",
                   0,0,0);
         for (j = 0; j <  i ; j++ )
         {
            AV_Free( mediaAttrInfo[j].media_attr_val.p );
         }
         AV_Free((void *)mediaAttrInfo);

         pduId = BT_PF_AVRCP_GET_ELEMENT_ATTRIBUTES;
         bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                                 txn,
                                                 pduId, 
                                                 BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
         return AEE_ENOMEMORY;
      }
      MEMCPY( mediaAttrInfo[i].media_attr_val.p, mediaAttr[i].itemTxt,
              mediaAttr[i].itemTxtLen); 

   }

   stat = bt_cmd_pf_avrcp_send_get_element_attributes_reply (pMeExt->appId,
                                                             txn,
                                                             (uint8)mediaAttrCount,
                                                             mediaAttrInfo);

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );

}

//=============================================================================
//  OEMBTAVRCPTGExt_SendRegNotifReply
//=============================================================================
int OEMBTAVRCPTGExt_SendRegNotifReply(OEMBTAVRCPTGExt            *pMeExt,
                                      uint8                       txn,
                                      AEEBTAVRCPTgMediaEventMask  eventId,
                                      AEEBTAVRCPTgPlaybackInfo     *playInfo,
                                      AEEBTAVRCPTgContBattStatus   battStat,
                                      AEEBTAVRCPTgSystemStatus     sysStat,
                                      boolean                      isFinal,
                                      uint32                       *attrs,
                                      uint32                       attrCount)
{
   bt_pf_avrcp_notif_event_type              notifEvt;
   bt_cmd_status_type                        stat; 
   bt_pf_avrcp_pdu_id_type                   pduId;
   uint32                                    i; 

   MSG_HIGH("OOEMAVRCPTG Ext: SendRegNotifReply ",0,0,0);

   MEMSET((void*)&notifEvt, 0 , sizeof( bt_pf_avrcp_notif_event_type ) );

   notifEvt.event_id = OEMBTAVRCPTGExt_AeeToPfEventId( eventId );

   switch ( eventId )
   {
   case AEEBTAVRCPTG_PLAYBACK_STATUS_CHANGED:
      if ( NULL != playInfo )
      {
         notifEvt.u.play_status = (uint8)playInfo->playStatus;
      }
      break;    

   case AEEBTAVRCPTG_TRACK_CHANGED:
      if ( NULL != playInfo )
      {
         MEMCPY((uint8 *)notifEvt.u.track_index, (uint8 *)playInfo->trackIndex, 
                AEEBTAVRCPTG_SIZE_OF_TRACK_INDEX );
      }

      break;

   case AEEBTAVRCPTG_TRACK_REACHED_END:
   case AEEBTAVRCPTG_TRACK_REACHED_START:
      break;

   case AEEBTAVRCPTG_PLAYBACK_POS_CHANGED:
      if ( NULL != playInfo )
      {
         notifEvt.u.playback_pos = playInfo->curSongPos;
      }
      break;

   case AEEBTAVRCPTG_BATT_STATUS_CHANGED:
      notifEvt.u.battery_status = (uint8)battStat;
      break;
   case AEEBTAVRCPTG_SYSTEM_STATUS_CHANGED:
      notifEvt.u.system_status = (uint8)sysStat;
      break;

   case AEEBTAVRCPTG_PLAYER_APP_SETTING_CHANGED:
      if ( ( NULL != attrs ) && (0 != attrCount ) )
      {
         notifEvt.u.device_setting.num_of_attr = (uint8)attrCount;
         notifEvt.u.device_setting.attributes = AV_Malloc(attrCount* 2);
         if (NULL == notifEvt.u.device_setting.attributes )
         {
            MSG_ERROR("OOEMAVRCPTG Ext: SendRegNotifReply. Mem alloc fail ",
                      0,0,0);
            pduId = BT_PF_AVRCP_REGISTER_NOTIFICATION;
            bt_cmd_pf_avrcp_send_simple_meta_reply( pMeExt->appId,
                                                    txn,
                                                    pduId, 
                                                    BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR ); 
            return AEE_ENOMEMORY;
         }

		 for(i = 0; i < (attrCount * 2); i++ )
		 {
		    notifEvt.u.device_setting.attributes[i] = (uint8)attrs[i];
		 }
      }
      break;
   }

   stat = bt_cmd_pf_avrcp_send_register_notification_reply(pMeExt->appId, 
                                                           txn, 
                                                           &notifEvt, 
                                                           isFinal); 
   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );

}

//=============================================================================
//  OEMBTAVRCPTGExt_SendGroupNavReply
//=============================================================================
int OEMBTAVRCPTGExt_SendGroupNavReply(OEMBTAVRCPTGExt            *pMeExt,
                                      uint8                      txn,
                                      uint32                     vendorUniqueId,
                                      boolean                    buttonPress,
                                      AEEBTAVRCPTgCmdResponseStatus respCode)
{
   bt_cmd_status_type                        stat; 

   MSG_HIGH("OOEMAVRCPTG Ext: SendGroupNavReply button press = %x",buttonPress,0,0);

   stat = bt_cmd_pf_avrcp_send_group_navigation_reply (pMeExt->appId,
                                                       txn,
                                                       (uint16)vendorUniqueId,
                                                       buttonPress,
                                                       (bt_pf_avrcp_rsp_type) respCode);

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );
}

//=============================================================================
//  OEMBTAVRCPTGExt_SendEventIdReply
//=============================================================================
int OEMBTAVRCPTGExt_SendSimpleMetaReply(OEMBTAVRCPTGExt                *pMeExt,
                                        uint8                           txn,
                                        AEEBTAVRCPTgPduId               pduId, 
                                        AEEBTAVRCPTgCmdResponseStatus   status )
{
   bt_cmd_status_type                        stat; 
   bt_cmd_status_type                        pstat; 

   MSG_HIGH("OOEMAVRCPTG Ext: SendSimpleMetaReply ",0,0,0);


   pstat = OEMBTAVRCPTGExt_AeeToPFStatus(status);
   stat = bt_cmd_pf_avrcp_send_simple_meta_reply ( pMeExt->appId,
                                                   txn,
                                                   (uint8)pduId,
                                                   pstat);

   return OEMBTAVRCPTGExt_CmdStatusToAeeResult( stat );
}

#endif /* FEATURE_BT_AVRCP_13 */

#endif /* (FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV) */

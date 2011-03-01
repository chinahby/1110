/*=============================================================================
  FILE:         OEMBTAVRCPTarget.c

  SERVICES:     OEMBTAVRCPTarget implementation

  DESCRIPTION:  This file handles the OEM implementation for IBTAVRCPTarget

  ===============================================================================
  Copyright (c) 2007-2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  $Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBTAVRCPTarget.c#1 $ 
  $DateTime: 2009/01/07 18:14:54 $
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

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV)
#include "AEEStdDef.h"
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEEShell.h"
#include "AEEIBTAVRCPTarget.h"
#include "AEEBTDef.h"
#include "OEMBTAVRCPTarget.h"
#include "bt.h"
#include "btpf.h"
#ifdef BT_SIMULATION
#include "bt_sim_interface.h"
#else
#include "msg.h"
#endif
#include "btmsg.h"
#include "btpfcmdi.h"
#include "btcmdi.h"
#include "OEMHeap.h"

#define OEMAVRCP_LOCK() /* TODO */
#define OEMAVRCP_UNLOCK() /* TODO */

struct OEMAVRCPTarget {
  bt_app_id_type          appId;
  int8                    SupportedCats;
  int8                    DesiredCats;
  bt_cmd_status_type      CmdDoneStatus;
  boolean                 bEnabled;
  int                     dRefCnt;
  OEMBTAVRCP_EvCb        *pfEvCb;
  void                   *pEvCbUser;
  OEMBTAVRCP_CmdCb       *pfCmdCb;
  void                   *pCmdCbUser;
  OEMBTAVRCP_VendorCmdCb *pfVendorCmdCb;
  void                   *pVendorCmdCbUser;
  BDAddress               Addr;
};

typedef struct {
  uint32   reason;
  uint32   status;
} OEMBTAVRCP_error_map_type;

static const OEMBTAVRCP_error_map_type OEMBTAVRCP_error_map[] = 
  {
    {BT_CS_PF_CONFIG_FAIL_INVALID_PARAMETERS, AEEBT_RESULT_NEGOTIATION_FAILURE},
    {BT_CS_PF_CONNECT_REFUSED_NO_RESOURCES, AEEBT_RESULT_NO_RESOURCE},
    {BT_CS_PF_DISCONNECT_LOWER_LAYER, AEEBT_RESULT_REMOTE_DISCONNECT},
    {BT_CS_PF_CONNECT_REFUSED_SECURITY, AEEBT_RESULT_NEGOTIATION_FAILURE},
    {BT_CS_PF_DISCONNECT_REMOTE_REQUEST, AEEBT_RESULT_REMOTE_DISCONNECT},
    {BT_BE_PAGE_TIMEOUT, AEEBT_RESULT_REMOTE_TIMEOUT},
    {BT_BE_CMD_DISALLOWED, AEEBT_RESULT_BAD_PARAM},
    {BT_BE_AUTHENTICATION_FAILURE, AEEBT_RESULT_NEGOTIATION_FAILURE},
    {BT_CS_PF_DISCONNECT_LOWER_LAYER, AEEBT_RESULT_REMOTE_DISCONNECT},
    
    //This has to be the last errorcode, if any new errorcodes has to be
    // added then, add above this 
    {BT_CS_GN_SUCCESS, AEEBT_RESULT_SUCCESS},
  };


OEMAVRCPTarget *gpOEMBTAVRCP;

static const BDAddress OEMBTAVRCP_bdAddrNull = BT_PF_BD_ADDR_NULL;

/******************************************************************************
   INTERNAL FUNCTIONS
******************************************************************************/

static uint16 OEMBTAVRCP_ConvertCmdStatus( bt_cmd_status_type cmd_status )
{
  switch ( cmd_status )
  {  
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return AEEBT_RESULT_SUCCESS;
    default:
      return EFAILED;
  }
}

static uint32 OEMBTAVRCP_MapErrorCode( uint32 reason )
{
  int i;
  uint32 result = 0;

  if((reason >= (BT_CMD_EV_HCI_BASE+BT_BE_PAGE_TIMEOUT)) && 
     reason < ((BT_CMD_EV_HCI_BASE+BT_BE_LAST)))
  {
    result = (reason & 0x00FF);
    switch(result)
    {
      case BT_BE_PAGE_TIMEOUT:
      case BT_BE_CMD_DISALLOWED:
      case BT_BE_AUTHENTICATION_FAILURE:
	  	MSG_FATAL("***zzg BT_BE_AUTHENTICATION_FAILURE 2***", 0, 0, 0);
        reason = result;
        break;
      default:
        reason = BT_CS_PF_DISCONNECT_LOWER_LAYER;
    }
  }

  for( i = 0; OEMBTAVRCP_error_map[i].reason != BT_CS_GN_SUCCESS; i++ )
  {
    if( OEMBTAVRCP_error_map[i].reason == reason )
    {
      return OEMBTAVRCP_error_map[i].status;
    }
  }
  MSG_ERROR("OEMBTAVRCP_MapErrorCode:Couldn't find Errorcode 0x%x",reason,0,0);

  return SUCCESS;
}

static void OEMBTAVRCP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr)
{
  OEMAVRCPTarget *pMe = gpOEMBTAVRCP;
  AEEBTAVRCPEventType event;
  boolean send_event = FALSE;

  AEERemoteControl_CommandType op;
  boolean send_op = FALSE;

  //  AEERemoteControl_VendorCommandType vendorop;
  boolean send_vendorop = FALSE;

  MSG_HIGH( "AVRCPEvCb - ev=%x", bt_ev_msg_ptr->ev_hdr.ev_type,
               0, 0 );

  switch (bt_ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
      {
        bt_ev_gn_cmd_done_type* pm =
          (bt_ev_gn_cmd_done_type*)&bt_ev_msg_ptr->ev_msg;

        pMe->CmdDoneStatus = bt_ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status;

        switch ( pm->cmd_type )
        {
          case BT_PF_CMD_AVRCP_CONNECT:
          case BT_PF_CMD_AVRCP_DISCONNECT:
          case BT_PF_CMD_AVRCP_SEND_REPLY:
          case BT_PF_CMD_AVRCP_SEND_GENERIC_REPLY:
          case BT_CMD_RM_SET_CONNECTABLE:
            break;

          case BT_PF_CMD_AVRCP_ENABLE:
            if ( pMe->CmdDoneStatus == BT_CS_GN_SUCCESS )
            {
              event.Event = AEEBTAVRCP_ENABLED;
            }
            else
            {
              event.Event = AEEBTAVRCP_ENABLE_FAILED; 
            }
            event.Result = AEEBT_RESULT_SUCCESS;
            send_event = TRUE; 
            break;

          case  BT_PF_CMD_AVRCP_DISABLE:
            event.Event = AEEBTAVRCP_DISABLED;
            if ( pMe->CmdDoneStatus == BT_CS_GN_SUCCESS )
            {
              event.Result = AEEBT_RESULT_SUCCESS;
            }
            else
            {
              event.Result = OEMBTAVRCP_ConvertCmdStatus( pMe->CmdDoneStatus );
            }
            send_event = TRUE; 
            break;

          default:
          MSG_ERROR( "AVRCPEvCb - unexpect Cmd Done %x", pm->cmd_type, 0, 0 );
            break;
        }
        break;
      } // end of case BT_EV_GN_CMD_DONE

    case BT_EV_PF_AVRCP_CON:
      event.Event = AEEBTAVRCP_CONNECTED;
      event.Address = *((BDAddress*)&bt_ev_msg_ptr->ev_msg.ev_avrcp_con.bd_addr);
      event.Result = AEEBT_RESULT_SUCCESS;
      send_event = TRUE;

      pMe->Addr = event.Address;

      (void)bt_cmd_rm_set_connectable( pMe->appId, FALSE, BT_RM_AVP_AUTOMATIC );
      break;

    case BT_EV_PF_AVRCP_CON_FAILED:
      event.Event = AEEBTAVRCP_CONN_FAILED;
      event.Address = pMe->Addr;
      event.Result = 
        OEMBTAVRCP_MapErrorCode( bt_ev_msg_ptr->ev_msg.ev_avrcp_con_failed.reason );
      send_event = TRUE;
      break;

    case BT_EV_PF_AVRCP_DISCON:
      event.Event = AEEBTAVRCP_DISCONNECTED;
      event.Address = pMe->Addr;
      event.Result = 
        OEMBTAVRCP_MapErrorCode( bt_ev_msg_ptr->ev_msg.ev_avrcp_discon.reason );
      send_event = TRUE;

      MEMCPY( &pMe->Addr, &OEMBTAVRCP_bdAddrNull, sizeof(pMe->Addr) );
      if( pMe->DesiredCats != pMe->SupportedCats )
      {
        OEMBTAVRCP_SetCategory( pMe, pMe->DesiredCats );
      }

      (void)bt_cmd_rm_set_connectable( pMe->appId, TRUE, BT_RM_AVP_AUTOMATIC );
      break;

    case BT_EV_PF_AVRCP_OP_SELECT:
    case BT_EV_PF_AVRCP_OP_UP:
    case BT_EV_PF_AVRCP_OP_DOWN:
    case BT_EV_PF_AVRCP_OP_LEFT:
    case BT_EV_PF_AVRCP_OP_RIGHT:
    case BT_EV_PF_AVRCP_OP_RIGHT_UP:
    case BT_EV_PF_AVRCP_OP_RIGHT_DOWN:
    case BT_EV_PF_AVRCP_OP_LEFT_UP:
    case BT_EV_PF_AVRCP_OP_LEFT_DOWN:
    case BT_EV_PF_AVRCP_OP_ROOT_MENU:
    case BT_EV_PF_AVRCP_OP_SETUP_MENU:
    case BT_EV_PF_AVRCP_OP_CONTENTS_MENU:
    case BT_EV_PF_AVRCP_OP_FAVORITE_MENU:
    case BT_EV_PF_AVRCP_OP_EXIT:
    case BT_EV_PF_AVRCP_OP_0:
    case BT_EV_PF_AVRCP_OP_1:
    case BT_EV_PF_AVRCP_OP_2:
    case BT_EV_PF_AVRCP_OP_3:
    case BT_EV_PF_AVRCP_OP_4:
    case BT_EV_PF_AVRCP_OP_5:
    case BT_EV_PF_AVRCP_OP_6:
    case BT_EV_PF_AVRCP_OP_7:
    case BT_EV_PF_AVRCP_OP_8:
    case BT_EV_PF_AVRCP_OP_9:
    case BT_EV_PF_AVRCP_OP_DOT:
    case BT_EV_PF_AVRCP_OP_ENTER:
    case BT_EV_PF_AVRCP_OP_CLEAR:
    case BT_EV_PF_AVRCP_OP_CHANNEL_UP:
    case BT_EV_PF_AVRCP_OP_CHANNEL_DOWN:
    case BT_EV_PF_AVRCP_OP_PREVIOUS_CHANNEL:
    case BT_EV_PF_AVRCP_OP_SOUND_SELECT:
    case BT_EV_PF_AVRCP_OP_INPUT_SELECT:
    case BT_EV_PF_AVRCP_OP_DISPLAY_INFORMATION:
    case BT_EV_PF_AVRCP_OP_HELP:
    case BT_EV_PF_AVRCP_OP_PAGE_UP:
    case BT_EV_PF_AVRCP_OP_PAGE_DOWN:
    case BT_EV_PF_AVRCP_OP_POWER:
    case BT_EV_PF_AVRCP_OP_VOLUME_UP:
    case BT_EV_PF_AVRCP_OP_VOLUME_DOWN:
    case BT_EV_PF_AVRCP_OP_MUTE:
    case BT_EV_PF_AVRCP_OP_PLAY:
    case BT_EV_PF_AVRCP_OP_STOP:
    case BT_EV_PF_AVRCP_OP_PAUSE:
    case BT_EV_PF_AVRCP_OP_RECORD:
    case BT_EV_PF_AVRCP_OP_REWIND:
    case BT_EV_PF_AVRCP_OP_FAST_FORWARD:
    case BT_EV_PF_AVRCP_OP_EJECT:
    case BT_EV_PF_AVRCP_OP_FORWARD:
    case BT_EV_PF_AVRCP_OP_BACKWARD:
    case BT_EV_PF_AVRCP_OP_ANGLE:
    case BT_EV_PF_AVRCP_OP_SUBPICTURE:
    case BT_EV_PF_AVRCP_OP_F1:
    case BT_EV_PF_AVRCP_OP_F2:
    case BT_EV_PF_AVRCP_OP_F3:
    case BT_EV_PF_AVRCP_OP_F4:
    case BT_EV_PF_AVRCP_OP_F5:
      {
        int offset = bt_ev_msg_ptr->ev_hdr.ev_type - BT_EV_PF_AVRCP_OP_SELECT;
        int64 cmd = 1;
        cmd = cmd << offset;
        op.Type = uint64struct_from_uint64(cmd);
        op.Direction = (AEERemoteControl_OpDirection) bt_ev_msg_ptr->ev_msg.ev_avrcp_op.status;
        send_op = TRUE;
      }
      break;

    case BT_EV_PF_AVRCP_OP_GENERIC:
      {
        /* TODO: XML-ify the generic op & fill in vendorop*/
        send_vendorop = TRUE;
        break;
      }
    default:
      {
      MSG_ERROR( "AVRCPEvCb - unexpect event %x",
                bt_ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
        return;
      }
  }
  OEMAVRCP_LOCK();
  if( send_event && pMe->pfEvCb )
  {
    pMe->pfEvCb( pMe->pEvCbUser, &event );
  }
  else if( send_event )
  {
    MSG_ERROR("AVRCP: no destination for event",0,0,0);    
  }
  else if( send_op && pMe->pfCmdCb )
  {
    OEMBTAVRCPRspType resp;

    resp = pMe->pfCmdCb( pMe->pCmdCbUser, &op );
    bt_cmd_pf_avrcp_send_reply( pMe->appId,
                                bt_ev_msg_ptr->ev_msg.ev_avrcp_op.txn_id,
                                bt_ev_msg_ptr->ev_msg.ev_avrcp_op.status,
                                bt_ev_msg_ptr->ev_hdr.ev_type,
                                (bt_pf_avrcp_rsp_type)resp );

  }
  else if( send_op )
  {
    MSG_ERROR("AVRCP: no destination for passthrough command",0,0,0);
    bt_cmd_pf_avrcp_send_reply( pMe->appId,
                                bt_ev_msg_ptr->ev_msg.ev_avrcp_op.txn_id,
                                bt_ev_msg_ptr->ev_msg.ev_avrcp_op.status,
                                bt_ev_msg_ptr->ev_hdr.ev_type,
                                BT_PF_AVRCP_RESPONSE_REJECTED );
  }
  else if( send_vendorop && pMe->pfVendorCmdCb )
  {
    // TODO: Actually send to CB once data has been parsed
    bt_pf_avrcp_generic_op_type *op;
    op = &bt_ev_msg_ptr->ev_msg.ev_avrcp_generic_op;
    op->cmd = BT_PF_AVRCP_RESPONSE_NOT_IMPLEMENTED;
    bt_cmd_pf_avrcp_send_generic_reply( pMe->appId,
                                        op->txn_id,
                                        op );
    //pMe->pfVendorCmdCb( pMe->pVendorCmdCbUser, &vendorop );
  }
  else if( send_vendorop )
  {
    bt_pf_avrcp_generic_op_type *op;
    MSG_ERROR("AVRCP: no destination for vendor op",0,0,0);
    op = &bt_ev_msg_ptr->ev_msg.ev_avrcp_generic_op;
    op->cmd = BT_PF_AVRCP_RESPONSE_REJECTED;
    bt_cmd_pf_avrcp_send_generic_reply( pMe->appId,
                                        op->txn_id,
                                        op );
  }  
  OEMAVRCP_UNLOCK();
}


/******************************************************************************
   PUBLIC FUNCTIONS
******************************************************************************/

AEEResult OEMBTAVRCP_EventCbRegister (OEMAVRCPTarget* pMe, OEMBTAVRCP_EvCb* pCB, void* pUser)
{
  OEMAVRCP_LOCK();
  pMe->pfEvCb = pCB;
  pMe->pEvCbUser = pUser;
  OEMAVRCP_UNLOCK();
  return AEE_SUCCESS; 
}

AEEResult OEMBTAVRCP_EventCbDeregister (OEMAVRCPTarget* pMe)
{
  pMe->pfEvCb = NULL;
  return AEE_SUCCESS; 
}

AEEResult OEMBTAVRCP_Release (OEMAVRCPTarget* pMe)
{
  /* TODO: Handle references correctly */
  AEEResult rv = AEE_EFAILED;
  int ref;
  if ( pMe != NULL )
  {
    if (pMe->dRefCnt > 0)
    {
      ref = --pMe->dRefCnt;

      if (0 == ref)
      {
        if( pMe->bEnabled == TRUE )
        {
          // TODO: Handle disconnecting as well, since disable won't disconnect
          rv = OEMBTAVRCP_Disable( pMe );
        }
        else
        {
          rv = AEE_SUCCESS;
        }

        if( rv == AEE_SUCCESS )
        {
          (void)bt_cmd_ec_free_application_id( pMe->appId );
          pMe->appId = BT_APP_ID_NULL;
          gpOEMBTAVRCP = NULL;
          OEM_Free(pMe);
          rv = AEE_SUCCESS;
        }    
      }
    }
  }
  return rv;
}

AEEResult OEMBTAVRCP_New (IShell* pIShell, OEMAVRCPTarget** pMe)
{
  OEMAVRCPTarget*    pNew;
  if ( gpOEMBTAVRCP == NULL )
  {
    // allocate the object
    pNew =  (OEMAVRCPTarget*)OEM_Malloc(sizeof(OEMAVRCPTarget));
    
    if ( pNew == NULL )
    {
      return ENOMEMORY;
    }
    // init BT layer
    pNew->appId =
      bt_cmd_ec_get_app_id_and_register( OEMBTAVRCP_EventCallback );
    if ( pNew->appId == BT_APP_ID_NULL )
    {
      FREE( pNew );
      return EFAILED;
    }
    pNew->dRefCnt = 1;
    pNew->SupportedCats = OEMBT_AVRCP_CATEGORY1;
    MEMCPY( &pNew->Addr, &OEMBTAVRCP_bdAddrNull, sizeof( pNew->Addr ) );
  }
  else
  {
    pNew = gpOEMBTAVRCP;
    pNew->dRefCnt++;
  }
  gpOEMBTAVRCP = pNew;
  *pMe = gpOEMBTAVRCP;
#ifdef BT_SIMULATION
  bt_sim_start(PROFILE_AVRCP_TARGET, TRUE);
#endif
  return AEE_SUCCESS;
}

AEEResult OEMBTAVRCP_Enable( OEMAVRCPTarget* pMe )
{
  int rv = bt_cmd_pf_avrcp_enable( pMe->appId, pMe->SupportedCats );

  if(( rv!= BT_CS_GN_PENDING )&& (rv!= BT_CS_GN_SUCCESS))
  {
    return AEE_EFAILED;
  }
  (void)bt_cmd_rm_set_connectable( pMe->appId, TRUE, BT_RM_AVP_AUTOMATIC );

  pMe->bEnabled = TRUE;
  return AEE_SUCCESS;
}

AEEResult OEMBTAVRCP_Connect( OEMAVRCPTarget* pMe, const BDAddress* pBDAddr )
{
  bt_cmd_status_type rv;
  pMe->Addr = *pBDAddr;
  rv = bt_cmd_pf_avrcp_connect( pMe->appId,
                                (bt_bd_addr_type*)pBDAddr );
  return OEMBTAVRCP_ConvertCmdStatus( rv );
}

AEEResult OEMBTAVRCP_Disconnect( OEMAVRCPTarget* pMe, const BDAddress* pBDAddr )
{
  bt_cmd_status_type rv;

  rv = bt_cmd_pf_avrcp_disconnect( pMe->appId );
  return OEMBTAVRCP_ConvertCmdStatus( rv );
}

AEEResult OEMBTAVRCP_Disable( OEMAVRCPTarget* pMe )
{
  bt_cmd_status_type rv;
  rv = bt_cmd_pf_avrcp_disable( pMe->appId ); 
  (void)bt_cmd_rm_set_connectable( pMe->appId, FALSE, BT_RM_AVP_AUTOMATIC );
  return OEMBTAVRCP_ConvertCmdStatus( rv );
}

AEEResult OEMBTAVRCP_CmdCbDeregister (OEMAVRCPTarget* pMe)
{
  pMe->pfCmdCb = NULL;
  return AEE_SUCCESS;
}

AEEResult OEMBTAVRCP_CmdCbRegister(OEMAVRCPTarget* pMe,
                                   OEMBTAVRCP_CmdCb* pCB,
                                   void* pUser)
{
  OEMAVRCP_LOCK();
  pMe->pfCmdCb = pCB;
  pMe->pCmdCbUser = pUser;
  OEMAVRCP_UNLOCK();
  return AEE_SUCCESS;
}

AEEResult OEMBTAVRCP_VendorCmdCbDeregister (OEMAVRCPTarget* pMe)
{
  pMe->pfVendorCmdCb = NULL;
  return AEE_SUCCESS;
}

AEEResult OEMBTAVRCP_VendorCmdCbRegister(OEMAVRCPTarget* pMe, 
                                         OEMBTAVRCP_VendorCmdCb* pCB,
                                         void* pUser)
{
  OEMAVRCP_LOCK();
  pMe->pfVendorCmdCb = pCB;
  pMe->pVendorCmdCbUser = pUser;
  OEMAVRCP_UNLOCK();
  return AEE_SUCCESS;
}

AEEResult OEMBTAVRCP_SetCategory( OEMAVRCPTarget *pMe, OEMBTAVRCPCatType Category )
{
  AEEResult rv = AEE_EFAILED;

  pMe->DesiredCats = Category;
  if( Category == pMe->SupportedCats )
  {
    rv = AEE_SUCCESS;
  }
  else
  {
    if( pMe->bEnabled && 
        BT_BD_ADDRS_EQUAL( &pMe->Addr, &OEMBTAVRCP_bdAddrNull ) )
    {
      pMe->SupportedCats = Category;
      bt_cmd_pf_avrcp_disable( pMe->appId );
      bt_cmd_pf_avrcp_enable( pMe->appId,  pMe->SupportedCats );
      rv = AEE_SUCCESS;
    }
  }
  return rv;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV) */


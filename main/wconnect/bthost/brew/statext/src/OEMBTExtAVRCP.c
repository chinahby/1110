/*===========================================================================

FILE:      OEMBTExtAVRCP.c

SERVICES:  BlueTooth Advanced Audio Distribution Profile  QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth AVRCP.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2005-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtAVRCP.c#2 $
$DateTime: 2009/04/14 15:01:41 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-04-14   pn  Watermark queues get initialized by dsm_queue_init().

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV)
#include "bt.h"
#include "bti.h"
#include "btpf.h"
#include "btpfcmdi.h"
#include "msg.h"
#include "dsm.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtAVRCP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

typedef struct OEMBTExtAVRCPobj_struct
{
  bt_app_id_type          m_appId;
  ACONTEXT*               m_pac;
  IBTExtNotifier*         m_pNotifier;
  boolean                 m_bEnabled;
  // Stores incoming generic ops.
  q_type                  m_Q;
  dsm_watermark_type      m_WM;

  bt_cmd_status_type CmdDoneStatus;
} OEMBTExtAVRCPobj_t;

typedef struct {
  uint32   reason;
  uint32   status;
} OEMBTExtAVRCP_error_map_type;
//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static uint16 OEMBTExtAVRCP_ConvertCmdStatus( bt_cmd_status_type cmd_status );
static void OEMBTExtAVRCP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtAVRCPobj_t*     gpOEMBTExtAVRCPobj = NULL;

static const OEMBTExtAVRCP_error_map_type OEMBTExtAVRCP_error_map[] = 
{
    {BT_CS_PF_CONFIG_FAIL_INVALID_PARAMETERS, AEEBT_AVRCP_ERR_CONFIG_PARAMS_NOT_AGREEABLE},
    {BT_CS_PF_CONNECT_REFUSED_NO_RESOURCES, AEEBT_AVRCP_ERR_CONNECT_REFUSED_NO_RESOURCES},
    {BT_CS_PF_DISCONNECT_LOWER_LAYER, AEEBT_AVRCP_ERR_LINK_LOST},
    {BT_CS_PF_CONNECT_REFUSED_SECURITY, AEEBT_AVRCP_ERR_AUTHENTICATION_FAILED},
    {BT_CS_PF_DISCONNECT_REMOTE_REQUEST, AEEBT_AVRCP_ERR_NORMAL_DISCONNECT},
    {BT_BE_PAGE_TIMEOUT, AEEBT_AVRCP_ERR_PAGE_FAILED},
    {BT_BE_CMD_DISALLOWED, AEEBT_AVRCP_ERR_WRONG_COMMAND},
    {BT_BE_AUTHENTICATION_FAILURE, AEEBT_AVRCP_ERR_AUTHENTICATION_FAILED},
    {BT_CS_PF_DISCONNECT_LOWER_LAYER, AEEBT_AVRCP_ERR_LINK_LOST},
    
    // This has to be the last errorcode, if any new errorcodes has to be
    //added then, add above this 
    {BT_CS_GN_SUCCESS, AEEBT_AVRCP_ERR_NONE},
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtAVRCP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtAVRCP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtAVRCP_Init(IBTExtAVRCP* pParent)
{
  OEMBTExtAVRCPobj_t*    pNew;

  // populate free event list?
  if ( gpOEMBTExtAVRCPobj == NULL )
  {
    // allocate the object
    pNew = MALLOCREC( OEMBTExtAVRCPobj_t );
    if ( pNew == NULL )
    {
      return ENOMEMORY;
    }

    // init RX queue for generic ops
    dsm_queue_init( &pNew->m_WM, 10000, &pNew->m_Q );
    pNew->m_WM.hi_watermark    = 500;

    // init BT layer
    pNew->m_appId =
      bt_cmd_ec_get_app_id_and_register( OEMBTExtAVRCP_EventCallback );
    if ( pNew->m_appId == BT_APP_ID_NULL )
    {
      FREE( pNew );
      return EFAILED;
    }

    if ( ISHELL_CreateInstance( pParent->m_pIShell, AEECLSID_BLUETOOTH_NOTIFIER,
                                (void**) &pNew->m_pNotifier ) != SUCCESS )
    {
      OEMBTExtAVRCP_Release( pParent );
      return EFAILED;
    }

    pNew->m_pac        = AEE_GetAppContext();
    gpOEMBTExtAVRCPobj = pNew;

  }
  else
  {
    return EFAILED;
  }

  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtAVRCP_Release( IBTExtAVRCP* pParent )
{
  OEMBTExtAVRCPobj_t *pMe = gpOEMBTExtAVRCPobj;
  uint32 rv = EFAILED;
  if ( pMe != NULL )
  {
    if( pMe->m_bEnabled == TRUE )
    {
      rv = OEMBTExtAVRCP_Disable( pParent );
    }
    else
    {
      rv = AEEBT_AVRCP_ERR_NONE;
    }

    if( rv == AEEBT_AVRCP_ERR_NONE )
    {
      (void)bt_cmd_ec_free_application_id( pMe->m_appId );
      pMe->m_appId = BT_APP_ID_NULL;
      pMe->m_pac   = NULL;
      
      dsm_empty_queue( &pMe->m_WM );

      if( pMe->m_pNotifier != NULL )
      {
        IBTEXTNOTIFIER_Release( pMe->m_pNotifier );
        pMe->m_pNotifier = NULL;
      }
      
      FREE(pMe);
      gpOEMBTExtAVRCPobj = NULL;
      rv = SUCCESS;
    }    
  }
  return rv;
}

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_Enable(IBTExtAVRCP* pParent,
                         AEEBTAVRCPCatType supportedCats )
{  
  OEMBTExtAVRCPobj_t *pMe = gpOEMBTExtAVRCPobj;

  if( bt_cmd_pf_avrcp_enable( pMe->m_appId, supportedCats ) != BT_CS_GN_PENDING )
  {
    return EFAILED;
  }
  (void)bt_cmd_rm_set_connectable( pMe->m_appId, TRUE, BT_RM_AVP_AUTOMATIC );

  pMe->m_bEnabled = TRUE;
  return SUCCESS;
}

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_Connect(IBTExtAVRCP* pParent,
                          AEEBTBDAddr* pBDAddr)
{
  bt_cmd_status_type rv;
  OEMBTExtAVRCPobj_t *pMe = gpOEMBTExtAVRCPobj;

  rv = bt_cmd_pf_avrcp_connect( pMe->m_appId,
                               (bt_bd_addr_type*)pBDAddr );
  return ( (int)OEMBTExtAVRCP_ConvertCmdStatus( rv ) );
}

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_Disconnect(IBTExtAVRCP* po)
{
  bt_cmd_status_type rv;
  OEMBTExtAVRCPobj_t *pMe = gpOEMBTExtAVRCPobj;

  rv = bt_cmd_pf_avrcp_disconnect( pMe->m_appId );

  return ( (int)OEMBTExtAVRCP_ConvertCmdStatus( rv ) );
}

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_Disable(IBTExtAVRCP* po)
{
  OEMBTExtAVRCPobj_t *pMe = gpOEMBTExtAVRCPobj;
  int                 rv;

  bt_cmd_pf_avrcp_disable( pMe->m_appId );
  rv = (int)OEMBTExtAVRCP_ConvertCmdStatus( 
                (bt_cmd_status_type)pMe->CmdDoneStatus );
  (void)bt_cmd_rm_set_connectable( pMe->m_appId, FALSE, BT_RM_AVP_AUTOMATIC );

  return rv;
}

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_SendReply(IBTExtAVRCP*           pParent,
                            uint8                  uTxnID,
                            AEEBTAVRCPOpStatusType uStatus,
                            AEEBTAVRCPEventType    Evt,
                            AEEBTAVRCPRspType      Rsp)
{
  bt_cmd_status_type rv;
  OEMBTExtAVRCPobj_t *pMe = gpOEMBTExtAVRCPobj;
  
  rv = bt_cmd_pf_avrcp_send_reply( 
            pMe->m_appId,
            uTxnID,
            uStatus,
            (bt_event_type)((bt_event_type)Evt+BT_EV_PF_AVRCP_CON),
            (bt_pf_avrcp_rsp_type)Rsp );
  
  return ( (int)OEMBTExtAVRCP_ConvertCmdStatus( rv ) );
}

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_SendGenericReply(IBTExtAVRCP*             pParent,
                                   AEEBTAVRCPGenericOpType* pFrame,
                                   uint8*                   pRsp )
{
  bt_cmd_status_type rv;
  OEMBTExtAVRCPobj_t *pMe = gpOEMBTExtAVRCPobj;
  dsm_item_type      *dsm_ptr;
  bt_pf_avrcp_generic_op_type op;

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, pFrame->operand_count );
  dsm_pushdown_tail( &dsm_ptr, pRsp, pFrame->operand_count,
                     DSM_DS_SMALL_ITEM_POOL );

  op.txn_id       = pFrame->txn_id;
  op.cmd          = pFrame->cmd;
  op.subunit_type = pFrame->subunit_type;
  op.subunit_id   = pFrame->subunit_id;
  op.opcode       = pFrame->opcode;
  op.operand      = dsm_ptr;

  rv = bt_cmd_pf_avrcp_send_generic_reply( pMe->m_appId,
                                           pFrame->txn_id,
                                           &op );
  
  return ( (int)OEMBTExtAVRCP_ConvertCmdStatus( rv ) );
}

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_GetGenericOp(IBTExtAVRCP*             pParent,
                               uint8*                   pBuf,
                               int                      nSize )
{
  int                 rv = EFAILED;
  OEMBTExtAVRCPobj_t *pMe = gpOEMBTExtAVRCPobj;
  dsm_item_type      *dsm_ptr;
  
  dsm_ptr = dsm_dequeue( &pMe->m_WM );

  if( dsm_ptr )
  {
    if( (pBuf != NULL)                       &&
        (dsm_length_packet(dsm_ptr) <= nSize) )
    {
      dsm_pullup( &dsm_ptr, (void*)pBuf,
                  dsm_length_packet(dsm_ptr) );
    }
    dsm_free_packet( &dsm_ptr );
  }

  return rv;
}

static uint16 OEMBTExtAVRCP_ConvertCmdStatus( bt_cmd_status_type cmd_status )
{
  switch ( cmd_status )
  {  
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return AEEBT_AVRCP_ERR_NONE;
    default:
      return EFAILED;
  }
}

/*===========================================================================

Function:  OEMBTExtAVRCP_MapErrorCode()

Description:
   This is called locally to map the lower layer reasons to 
   App reasons.

Parameters:
   uint16 reason: Reason passed from the lower layer.

Return Value:  APP reason or SUCCESS 

Componts:      None

Side Effects:  None

===========================================================================*/
static uint16 OEMBTExtAVRCP_MapErrorCode( uint16 reason )
{
  int i;
  uint16 result = 0;

  if((reason >= (BT_CMD_EV_HCI_BASE+BT_BE_PAGE_TIMEOUT)) && 
      reason < ((BT_CMD_EV_HCI_BASE+BT_BE_LAST)))
  {
    result = (reason & 0x00FF);
    switch(result)
    {
      case BT_BE_PAGE_TIMEOUT:
      case BT_BE_CMD_DISALLOWED:
      case BT_BE_AUTHENTICATION_FAILURE:
	  		MSG_FATAL("***zzg BT_BE_AUTHENTICATION_FAILURE 3***", 0, 0, 0);
           reason = result;
           break;
      default:
          reason = BT_CS_PF_DISCONNECT_LOWER_LAYER;
    }
  }

  for( i = 0; OEMBTExtAVRCP_error_map[i].reason != BT_CS_GN_SUCCESS; i++ )
  {
    if( OEMBTExtAVRCP_error_map[i].reason == reason )
    {
      return OEMBTExtAVRCP_error_map[i].status;
    }
  }
  MSG_ERROR("OEMBTExtAVRCP_MapErrorCode:Couldn't find Errorcode 0x%x",reason,0,0);

  return SUCCESS;
}


// called from BT task context; so just queue the event
static void OEMBTExtAVRCP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr)
{
  OEMBTExtAVRCPobj_t* pMe = gpOEMBTExtAVRCPobj;
  AEEBTNotification*  pN  = NULL;

  if ( pMe == NULL )
  {
    return; // the object no longer exists
  }

  MSG_HIGH( "AVRCPEvCb - ev=%x", bt_ev_msg_ptr->ev_hdr.ev_type,
            0, 0 );

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "AVRCPEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }

  pN->data.uError = AEEBT_AVRCP_ERR_NONE;

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
        case BT_PF_CMD_AVRCP_ENABLE:
        case BT_PF_CMD_AVRCP_DISABLE:
        case BT_PF_CMD_AVRCP_SEND_REPLY:
        case BT_PF_CMD_AVRCP_SEND_GENERIC_REPLY:
        case BT_CMD_RM_SET_CONNECTABLE:
          IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
          return;

        default:
          MSG_ERROR( "AVRCPEvCb - unexpect Cmd Done %x", pm->cmd_type, 0, 0 );
          IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
          return;
      }      
    } // end of case BT_EV_GN_CMD_DONE   

    case BT_EV_PF_AVRCP_CON:
      pN->uID = AEEBT_AVRCP_EVT_CONNECTED;
      pN->data.bdAddr = *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_avrcp_con.bd_addr);
      (void)bt_cmd_rm_set_connectable( pMe->m_appId, FALSE, BT_RM_AVP_AUTOMATIC );
      break;
    case BT_EV_PF_AVRCP_CON_FAILED:
      pN->uID = AEEBT_AVRCP_EVT_CON_FAILED;
      pN->data.uError = OEMBTExtAVRCP_MapErrorCode(
              (uint16)bt_ev_msg_ptr->ev_msg.ev_avrcp_con_failed.reason);
      break;
    case BT_EV_PF_AVRCP_DISCON:
      pN->uID = AEEBT_AVRCP_EVT_DISCONNECTED;
      pN->data.uError = OEMBTExtAVRCP_MapErrorCode(
              (uint16)bt_ev_msg_ptr->ev_msg.ev_avrcp_discon.reason);
      (void)bt_cmd_rm_set_connectable( pMe->m_appId, TRUE, BT_RM_AVP_AUTOMATIC );
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
      pN->uID = bt_ev_msg_ptr->ev_hdr.ev_type - BT_EV_PF_AVRCP_CON;
      pN->data.AvrcpOp.opStatus = bt_ev_msg_ptr->ev_msg.ev_avrcp_op.status;
      pN->data.AvrcpOp.txnId = bt_ev_msg_ptr->ev_msg.ev_avrcp_op.txn_id;
      break;
    case BT_EV_PF_AVRCP_OP_GENERIC:
      {
        dsm_item_type                  *dsm_ptr;
        bt_pf_ev_avrcp_generic_op_type *op;

        op = &bt_ev_msg_ptr->ev_msg.ev_avrcp_generic_op;
        dsm_ptr = op->operand;

        pN->uID = AEEBT_AVRCP_EVT_OP_GENERIC;
        pN->data.AvrcpGenericOp.txn_id        = op->txn_id;
        pN->data.AvrcpGenericOp.cmd           = op->cmd;
        pN->data.AvrcpGenericOp.subunit_type  = op->subunit_type;
        pN->data.AvrcpGenericOp.subunit_id    = op->subunit_id;
        pN->data.AvrcpGenericOp.opcode        = op->opcode;
        pN->data.AvrcpGenericOp.operand_count = dsm_length_packet( dsm_ptr );
        
        dsm_enqueue( &pMe->m_WM, &dsm_ptr );
      }
      break;
    default:
    {
      MSG_ERROR( "AVRCPEvCb - unexpect event %x",
                 bt_ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | NMASK_BT_AVRCP;
  IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
}
#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV) */

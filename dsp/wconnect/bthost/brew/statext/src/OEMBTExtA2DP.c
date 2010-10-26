/*===========================================================================

FILE:      OEMBTExtA2DP.c

SERVICES:  BlueTooth Advanced Audio Distribution Profile  QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth A2DP.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtA2DP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV)
#include "bt.h"
#include "bti.h"
#include "btpf.h"
#include "btpfcmdi.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtA2DP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

typedef struct OEMBTExtA2DPobj_struct
{
  bt_app_id_type          m_appId;
  ACONTEXT*               m_pac;
  IBTExtNotifier*         m_pNotifier;
  boolean                 m_bConnected;
  boolean                 m_bInUse;
} OEMBTExtA2DPobj_t;

typedef struct {
  uint32   reason;
  uint32   status;
} OEMBTExtA2DP_error_map_type;

//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static uint16 OEMBTExtA2DP_ConvertCmdStatus( bt_cmd_status_type cmd_status );
static void OEMBTExtA2DP_EventCallback( struct bt_ev_msg_struct* bt_ev_msg_ptr );

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtA2DPobj_t*     gpOEMBTExtA2DPobj = NULL;

static const OEMBTExtA2DP_error_map_type OEMBTExtA2DP_error_map[] = 
{

  {BT_CS_PF_A2DP_ERR_INCOMPATIABLE_DEVICES,
      AEEBT_A2DP_ERR_INCOMPATIABLE_DEVICES},
  {BT_CS_PF_A2DP_ERR_ENDPOINT_DISCOVERY, 
      AEEBT_A2DP_ERR_ENDPOINT_DISCOVERY},
  {BT_CS_PF_A2DP_ERR_FAILED_TO_START_STREAM,
      AEEBT_A2DP_ERR_FAILED_TO_START_STREAM},
  {BT_CS_PF_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM, 
      AEEBT_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM},
  {BT_CS_PF_A2DP_ERR_ABORT_FAILED, 
      AEEBT_A2DP_ERR_ABORT_FAILED},
  {BT_CS_PF_A2DP_ERR_FAILED_TO_PROCESS_CMD, 
      AEEBT_A2DP_ERR_FAILED_TO_PROCESS_CMD},
  {BT_CS_PF_A2DP_ERR_SET_CONFIG_FAILED, 
      AEEBT_A2DP_ERR_SET_CONFIG_FAILED},
  {BT_CS_PF_CONFIG_FAIL_INVALID_PARAMETERS, 
      AEEBT_A2DP_ERR_CONFIG_PARAMS_NOT_AGREEABLE},
  {BT_CS_PF_CONNECT_REFUSED_NO_RESOURCES, 
      AEEBT_A2DP_ERR_CONNECT_REFUSED_NO_RESOURCES},
  {BT_CS_PF_CONNECT_TIMEOUT, 
      AEEBT_A2DP_ERR_PAGE_FAILED},  
  {BT_CS_PF_CONNECT_REFUSED_SECURITY, 
      AEEBT_A2DP_ERR_AUTHENTICATION_FAILED},
  {BT_CS_PF_DISCONNECT_REMOTE_REQUEST, 
      AEEBT_A2DP_ERR_NORMAL_DISCONNECT},
  {BT_CS_PF_A2DP_NORMAL_DISCONNECT,
      AEEBT_A2DP_ERR_NORMAL_DISCONNECT},
  {BT_CS_PF_DISCONNECT_LOWER_LAYER, 
      AEEBT_A2DP_ERR_LINK_LOST},
  {BT_BE_PAGE_TIMEOUT,
      AEEBT_A2DP_ERR_PAGE_FAILED},
  {BT_BE_CMD_DISALLOWED,
      AEEBT_A2DP_ERR_WRONG_COMMAND},
  {BT_CS_PF_NO_RESOURCES,
      AEEBT_A2DP_ERR_RESOURCES_NOT_AVALIABLE},
  {BT_CS_PF_RESET_IN_PROGRESS,
      AEEBT_A2DP_ERR_RESET_IN_PROGRESS},
  /* This has to be the last errorcode, if any new errorcodes has to be
   * added then, add above this */
  {BT_CS_PF_A2DP_ERR_A2DP_NONE,
      AEEBT_A2DP_ERR_NONE},  
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtA2DP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtA2DP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtA2DP_Init( IBTExtA2DP* pParent )
{
  OEMBTExtA2DPobj_t*    pNew;

  // populate free event list?
  if ( gpOEMBTExtA2DPobj == NULL )
  {
    // allocate the object
    pNew = MALLOCREC( OEMBTExtA2DPobj_t );
    if ( pNew == NULL )
    {
      return ENOMEMORY;
    }

    // init BT layer
    pNew->m_appId =
      bt_cmd_ec_get_app_id_and_register( OEMBTExtA2DP_EventCallback );
    if ( pNew->m_appId == BT_APP_ID_NULL )
    {
      FREE( pNew );
      return EFAILED;
    }

    // Register with the A2DP layer
    if( bt_cmd_pf_a2dp_enable( pNew->m_appId ) != BT_CS_GN_PENDING )
    {
      (void)bt_cmd_ec_free_application_id( pNew->m_appId );
      FREE( pNew );
      return EFAILED;
    }

    if ( ISHELL_CreateInstance( pParent->m_pIShell, AEECLSID_BLUETOOTH_NOTIFIER,
                                (void**) &pNew->m_pNotifier ) != SUCCESS )
    {
      OEMBTExtA2DP_Release( pParent );
      return EFAILED;
    }

    pNew->m_pac        = AEE_GetAppContext();
    gpOEMBTExtA2DPobj  = pNew;

  }
  else
  {
    return EFAILED;
  }

  (void)bt_cmd_rm_set_connectable( pNew->m_appId, TRUE, BT_RM_AVP_AUTOMATIC );

  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtA2DP_Release( IBTExtA2DP* pParent )
{
  OEMBTExtA2DPobj_t *pMe = gpOEMBTExtA2DPobj;
  if ( pMe != NULL )
  {
    (void)bt_cmd_rm_set_connectable( pMe->m_appId, FALSE, BT_RM_AVP_AUTOMATIC );
    bt_cmd_pf_a2dp_disable( pMe->m_appId );
    (void)bt_cmd_ec_free_application_id( pMe->m_appId );
    pMe->m_appId = BT_APP_ID_NULL;
    pMe->m_pac   = NULL;

    if ( pMe->m_pNotifier != NULL )
    {
      IBTEXTNOTIFIER_Release( pMe->m_pNotifier );
    }

    FREE(pMe);
    gpOEMBTExtA2DPobj = NULL;
  }

  return SUCCESS;
}

// See AEEBTExtA2DP
int OEMBTExtA2DP_SetDevice(IBTExtA2DP* pParent,
                           AEEBTBDAddr* pBDAddr)
{
  bt_cmd_status_type rv;
  OEMBTExtA2DPobj_t *pMe = gpOEMBTExtA2DPobj;

  rv = bt_cmd_pf_a2dp_set_device( pMe->m_appId,
                                  (bt_bd_addr_type*)pBDAddr );
  
  return ( (int)OEMBTExtA2DP_ConvertCmdStatus( rv ) );
}

// See AEEBTExtA2DP
int OEMBTExtA2DP_Connect(IBTExtA2DP* pParent,
                         AEEBTBDAddr* pBDAddr)
{
  bt_cmd_status_type rv;
  OEMBTExtA2DPobj_t *pMe = gpOEMBTExtA2DPobj;

  rv = bt_cmd_pf_a2dp_connect( pMe->m_appId,
                               (bt_bd_addr_type*)pBDAddr );

  return ( (int)OEMBTExtA2DP_ConvertCmdStatus( rv ) );
}

// See AEEBTExtA2DP
int OEMBTExtA2DP_Disconnect(IBTExtA2DP* po)
{
  bt_cmd_status_type rv;
  OEMBTExtA2DPobj_t *pMe = gpOEMBTExtA2DPobj;

  rv = bt_cmd_pf_a2dp_disconnect( pMe->m_appId );
  return ( (int)OEMBTExtA2DP_ConvertCmdStatus( rv ) );
}

// See AEEBTExtA2DP
int OEMBTExtA2DP_Ctl(IBTExtA2DP* po, uint8 ctl, uint32 data)
{
  bt_cmd_status_type rv;
  OEMBTExtA2DPobj_t *pMe = gpOEMBTExtA2DPobj;

  rv = bt_cmd_pf_a2dp_ctl( pMe->m_appId, (bt_pf_a2dp_ctl_type)ctl, data );
  
  return ( (int)OEMBTExtA2DP_ConvertCmdStatus( rv ) );
}

// See AEEBTExtA2DP
int OEMBTExtA2DP_Start(IBTExtA2DP* po)
{
  bt_cmd_status_type rv;
  OEMBTExtA2DPobj_t *pMe = gpOEMBTExtA2DPobj;

  rv = bt_cmd_pf_a2dp_start( pMe->m_appId, NULL );

  return ( (int)OEMBTExtA2DP_ConvertCmdStatus( rv ) );
}

// See AEEBTExtA2DP
int OEMBTExtA2DP_Suspend(IBTExtA2DP* po)
{
  bt_cmd_status_type rv;
  OEMBTExtA2DPobj_t *pMe = gpOEMBTExtA2DPobj;

  rv = bt_cmd_pf_a2dp_suspend( pMe->m_appId );

  return ( (int)OEMBTExtA2DP_ConvertCmdStatus( rv ) );
}

static uint16 OEMBTExtA2DP_ConvertCmdStatus( bt_cmd_status_type cmd_status )
{
  switch ( cmd_status )
  {
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return AEEBT_A2DP_ERR_NONE;
    default:
      return EFAILED;
  }
}

/*===========================================================================

Function:  OEMBTExtA2DP_MapErrorCode()

Description:
   This is called locally to map the lower layer reasons to 
   App reasons.

Parameters:
   uint16 reason: Reason passed from the lower layer.

Return Value:  APP reason or SUCCESS 

Componts:      None

Side Effects:  None

===========================================================================*/
static uint16 OEMBTExtA2DP_MapErrorCode( uint16 reason )
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
           reason = result;
           break;
      default:
          reason = BT_CS_PF_DISCONNECT_LOWER_LAYER;
    }
  }

  for( i = 0; OEMBTExtA2DP_error_map[i].reason != BT_CS_PF_A2DP_ERR_A2DP_NONE; i++ )
  {
    if( OEMBTExtA2DP_error_map[i].reason == reason )
    {
      return OEMBTExtA2DP_error_map[i].status;
    }
  }
  MSG_LOW("OEMBTExtA2DP_MapErrorCode:Couldn't find Errorcode 0x%x",reason,0,0);

  return SUCCESS;
}

// called from BT task context; so just queue the event
static void OEMBTExtA2DP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr)
{
  OEMBTExtA2DPobj_t*  pMe = gpOEMBTExtA2DPobj;
  AEEBTNotification*  pN  = NULL;

  if ( pMe == NULL )
  {
    return; // the object no longer exists
  }

  MSG_HIGH( "A2DPEvCb - ev=%x", bt_ev_msg_ptr->ev_hdr.ev_type,
            0, 0 );

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "A2DPEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }

  pN->data.uError = AEEBT_A2DP_ERR_NONE;

  switch (bt_ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
    {
      bt_ev_gn_cmd_done_type* pm =
        (bt_ev_gn_cmd_done_type*)&bt_ev_msg_ptr->ev_msg;

      switch ( pm->cmd_type )
      {
        case BT_PF_CMD_A2DP_CONNECT:
        case BT_PF_CMD_A2DP_DISCONNECT:
        case BT_PF_CMD_A2DP_ENABLE:
        case BT_PF_CMD_A2DP_DISABLE:
        case BT_PF_CMD_A2DP_SET_DEVICE:
        case BT_PF_CMD_A2DP_CTL:
        case BT_PF_CMD_A2DP_START:
        case BT_PF_CMD_A2DP_SUSPEND:
        case BT_CMD_RM_SET_CONNECTABLE:
          if( pm->cmd_status != BT_CS_GN_SUCCESS )
          {
            MSG_ERROR( "A2DPEvCb - Bad Cmd Done %x Status %x.",
                       pm->cmd_type, pm->cmd_status, 0 );
            if( pm->cmd_type == BT_PF_CMD_A2DP_CONNECT )
            {
              // Something wrong with the connect command.
              // Send back a connect failed event:
              pN->uID = AEEBT_A2DP_EVT_CON_FAILED;
              pN->data.uError = AEEBT_A2DP_ERR_NONE;
              break;
            }
          }
          IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
          return;

        default:
          MSG_ERROR( "A2DPEvCb - unexpect Cmd Done %x", pm->cmd_type, 0, 0 );
          IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
          return;
      }
      break;
    } // end of case BT_EV_GN_CMD_DONE

    case BT_EV_PF_A2DP_CON:
      pN->uID = AEEBT_A2DP_EVT_CONNECTED;
      pN->data.bdAddr = *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_a2dp_con.bd_addr);
      (void)bt_cmd_rm_set_connectable( pMe->m_appId, FALSE, BT_RM_AVP_AUTOMATIC );
      break;
    case BT_EV_PF_A2DP_CON_FAILED:
      pN->uID = AEEBT_A2DP_EVT_CON_FAILED;
      pN->data.uError = OEMBTExtA2DP_MapErrorCode(
              (uint16)bt_ev_msg_ptr->ev_msg.ev_a2dp_con_failed.reason);
      break;
  case BT_EV_PF_A2DP_DISCON:
      pN->uID = AEEBT_A2DP_EVT_DISCONNECTED;
      pN->data.uError = OEMBTExtA2DP_MapErrorCode(
              (uint16)bt_ev_msg_ptr->ev_msg.ev_a2dp_discon.reason);

      (void)bt_cmd_rm_set_connectable( pMe->m_appId, TRUE, BT_RM_AVP_AUTOMATIC );
      break;
    case BT_EV_PF_A2DP_START:
      pN->uID = AEEBT_A2DP_EVT_START;
      pN->data.A2DPStartInit = bt_ev_msg_ptr->ev_msg.ev_a2dp_start.origin;
      break;
    case BT_EV_PF_A2DP_SUSPEND:
      pN->uID = AEEBT_A2DP_EVT_SUSPEND;
      pN->data.A2DPSuspendInit = bt_ev_msg_ptr->ev_msg.ev_a2dp_suspend.origin;
      break;
    case BT_EV_PF_A2DP_OPEN:
      pN->uID = AEEBT_A2DP_EVT_OPEN;
      pN->data.A2DPOpenInit = bt_ev_msg_ptr->ev_msg.ev_a2dp_open.origin;
      break;
    case BT_EV_PF_A2DP_CLOSE:
      pN->uID = AEEBT_A2DP_EVT_CLOSE;
      pN->data.A2DPCloseInit = bt_ev_msg_ptr->ev_msg.ev_a2dp_close.origin;
      break;
    case BT_EV_PF_A2DP_BITRATE:
      pN->uID = AEEBT_A2DP_EVT_BITRATE;
      pN->data.A2DPBitrate = bt_ev_msg_ptr->ev_msg.ev_a2dp_bitrate.bitrate_hint;
      break;

    default:
    {
      MSG_ERROR( "A2DPEvCb - unexpect event %x",
                 bt_ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | NMASK_BT_A2DP;
  IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
}
#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV) */

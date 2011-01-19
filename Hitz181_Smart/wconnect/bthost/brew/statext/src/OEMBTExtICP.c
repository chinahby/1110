/*===========================================================================

FILE:      OEMBTExtICP.c

SERVICES:  BlueTooth Intercom Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Intercom Profile.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtICP.c#1 $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
10/09/08   jtl  Removing usage of depricated functions.

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP)
#include "bt.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "oi_obexspec.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtICP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================
typedef enum
{
  AEEBT_ICP_STATE_INIT,
  AEEBT_ICP_STATE_REGISTERED,
  AEEBT_ICP_STATE_CONNECTED,
  AEEBT_ICP_STATE_DISCONNECTED,
  AEEBT_ICP_STATE_CALL_SETUP_PENDING,
  AEEBT_ICP_STATE_CALL_SETUP
} AEEBTICPState;


typedef struct OEMBTExtICPobj_struct
{
  ACONTEXT*                   pac;
  IBTExtNotifier*             pNotifier;

  bt_app_id_type              appId;
  AEEBTBDAddr                 BDAddr;         // remote device's

  AEEBTICPState               state;

  struct OEMBTExtICPobj_struct* pNextICP;  // linked into mgr
} OEMBTExtICPobj_t;

typedef struct
{
  boolean                     bInUse;
  bt_event_type               event;
  bt_app_id_type              appId;
  uint16                      connId;
  AEECallback                 cb;
} OEMBTExtICP_EvCb;

// one of these
typedef struct
{
  OEMBTExtICPobj_t* pNextICP;    // singly linked list of ICPs
  OEMBTExtICP_EvCb  FreeEvCb[ 10 ];
} OEMBTExtICP_Mgr;

//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static int  OEMBTExtICP_CheckCmdStatus( bt_cmd_status_type stat );
static void OEMBTExtICP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtICP_Mgr        gMgr;               // manager object

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtICP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtICP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtICP_Init(IBTExtICP* pParent)
{
  OEMBTExtICPobj_t* pMe;

  // allocate pMe
  pMe = MALLOCREC(OEMBTExtICPobj_t);
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  if ( ISHELL_CreateInstance( pParent->m_pIShell, 
                              AEECLSID_BLUETOOTH_NOTIFIER, 
                              (void**) &pMe->pNotifier ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init BT layer
  pMe->appId = bt_cmd_ec_get_app_id_and_register(OEMBTExtICP_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    FREE( pMe );
    return EFAILED;
  }
  
  pMe->pac = AEE_GetAppContext();

  if ( AEEHandle_To( &gOEMBTExtHandleList,
                     (OEMINSTANCE*)pMe,
                     &pParent->m_hBT ) == FALSE )
  {
    FREE(pMe);
    return EFAILED;
  }

  if ( gMgr.pNextICP == NULL ) // first time?
  {
    uint8 i;
    for ( i = 0; i < (sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++ )
    {
      gMgr.FreeEvCb[ i ].bInUse = FALSE;
    }
  }
  // link into mgr
  pMe->pNextICP = gMgr.pNextICP;
  gMgr.pNextICP = pMe;

  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtICP_Release( IBTExtICP* pParent )
{
  OEMBTExtICPobj_t*  pMe;
  OEMBTExtICPobj_t** ppc;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) == TRUE )
  {
    // unlink from mgr
    for (ppc = &gMgr.pNextICP; *ppc != NULL; ppc = &(*ppc)->pNextICP)
    {
      if ((*ppc)->appId == pMe->appId)
      {
        *ppc = pMe->pNextICP;
        pMe->pNextICP = NULL;
        break;
      }
    }

    (void)bt_cmd_ec_free_application_id( pMe->appId );
    pMe->appId = BT_APP_ID_NULL;

    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    pMe->pNotifier = NULL;
    pMe->pac = NULL;

    FREE(pMe);
    AEEHandle_Clear( &gOEMBTExtHandleList, pParent->m_hBT );

    return SUCCESS;
  }

  return EFAILED;
}

// IBTEXTICP_Connect (see AEEBTExtICP.h)
int OEMBTExtICP_Connect( 
  IBTExtICP*          pParent, 
  const AEEBTBDAddr*  pBDAddr
)
{
  bt_cmd_status_type  stat;
  OEMBTExtICPobj_t    *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }

  // Remember BD Addr of peer device
  pMe->BDAddr = *pBDAddr;

  stat = bt_cmd_pf_icp_connect( pMe->appId, 
                                (bt_bd_addr_type*) pBDAddr );

  return OEMBTExtICP_CheckCmdStatus( stat );
}

// IBTEXTICP_AcceptConnection (see AEEBTExtICP.h)
int OEMBTExtICP_AcceptConnection( 
  IBTExtICP*          pParent, 
  const AEEBTBDAddr*  pBDAddr, 
  boolean             bAccept
)
{
  bt_cmd_status_type  stat;
  OEMBTExtICPobj_t    *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_pf_icp_accept_connection( pMe->appId, 
                                          (bt_bd_addr_type*) pBDAddr,
                                          bAccept ); 

  return OEMBTExtICP_CheckCmdStatus( stat );
}

// IBTEXTICP_Disconnect (see AEEBTExtICP.h)
int OEMBTExtICP_Disconnect( IBTExtICP* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtICPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_icp_disconnect( pMe->appId, 
                                   (bt_bd_addr_type*) &pMe->BDAddr );

  return OEMBTExtICP_CheckCmdStatus( stat );
}

// IBTEXTICP_ClearCall (see AEEBTExtICP.h)
int OEMBTExtICP_ClearCall(   
  IBTExtICP*          pParent, 
  AEEBTICPCauseType   cause
)
{
  bt_cmd_status_type  stat;
  OEMBTExtICPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_icp_clear_call( pMe->appId, 
                                   (bt_bd_addr_type*) &pMe->BDAddr,
                                   (bt_pf_icp_cause_type) cause );

  return OEMBTExtICP_CheckCmdStatus( stat );
}

// IBTEXTICP_EstablishCall (see AEEBTExtICP.h)
int OEMBTExtICP_EstablishCall(   
  IBTExtICP*                pParent, 
  const AEEBTICPNumberType* pCallingNum,
  const AEEBTICPNumberType* pCalledNum
)
{
  bt_cmd_status_type  stat;
  OEMBTExtICPobj_t *pMe;

  bt_pf_icp_number_type callingNum;
  bt_pf_icp_number_type calledNum;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  callingNum.calling      = pCallingNum->bCalling;
  callingNum.type         = 
      (bt_pf_ctp_number_type_type)pCallingNum->type;
  callingNum.plan         = 
      (bt_pf_ctp_number_plan_type)pCallingNum->plan;
  callingNum.presentation = 
      (bt_pf_ctp_presentation_type)pCallingNum->presentation;
  callingNum.screening    = 
      (bt_pf_ctp_screening_type)pCallingNum->screening;
  callingNum.num_digits   = pCallingNum->numDigits;
  STRLCPY( (char *)callingNum.digits,
           (char *)pCallingNum->digits,
           AEEBT_TCS_MAX_DIGITS );
   
  calledNum.calling       = pCalledNum->bCalling;
  calledNum.type          = 
      (bt_pf_ctp_number_type_type)pCalledNum->type;
  calledNum.plan          = 
      (bt_pf_ctp_number_plan_type)pCalledNum->plan;
  calledNum.presentation  = 
      (bt_pf_ctp_presentation_type)pCalledNum->presentation;
  calledNum.screening     = 
      (bt_pf_ctp_screening_type)pCalledNum->screening;
  calledNum.num_digits    = pCalledNum->numDigits;
  STRLCPY( (char *)calledNum.digits,
           (char *)pCalledNum->digits,
           AEEBT_TCS_MAX_DIGITS );

  stat = bt_cmd_pf_icp_establish_call( pMe->appId, 
                                       (bt_bd_addr_type*) &pMe->BDAddr,
                                       &callingNum,
                                       &calledNum );

  return OEMBTExtICP_CheckCmdStatus( stat );
}

// IBTEXTICP_IncomingStatus (see AEEBTExtICP.h)
int OEMBTExtICP_IncomingStatus(   
  IBTExtICP*             pParent,
  AEEBTICPCallStatusType status
)
{
  bt_cmd_status_type  stat;
  OEMBTExtICPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_icp_incoming_status( pMe->appId, 
                                       (bt_bd_addr_type*) &pMe->BDAddr,
                                       (bt_pf_icp_call_status_type) status );

  return OEMBTExtICP_CheckCmdStatus( stat );
}

// IBTEXTICP_AcceptCall (see AEEBTExtICP.h)
int OEMBTExtICP_AcceptCall( IBTExtICP* pParent)
{
  bt_cmd_status_type  stat;
  OEMBTExtICPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_icp_accept_call( pMe->appId, 
                                    (bt_bd_addr_type*) &pMe->BDAddr );

  return OEMBTExtICP_CheckCmdStatus( stat );
}

// IBTEXTICP_Register (see AEEBTExtICP.h)
int OEMBTExtICP_Register( IBTExtICP* pParent )
{
  bt_cmd_status_type stat;
  OEMBTExtICPobj_t   *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_icp_register( pMe->appId );

  return OEMBTExtICP_CheckCmdStatus( stat );
}


// IBTEXTICP_Deregister (see AEEBTExtICP.h)
int OEMBTExtICP_Deregister( IBTExtICP* pParent )
{
  bt_cmd_status_type          stat;
  OEMBTExtICPobj_t            *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_icp_deregister( pMe->appId );

  return OEMBTExtICP_CheckCmdStatus( stat );
}

//==========================================================================
//   static helper functions
//==========================================================================
static int OEMBTExtICP_CheckCmdStatus( bt_cmd_status_type stat )
{
  switch (stat)
  {
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return SUCCESS;
    case BT_CS_GN_CMD_Q_FULL:
      return ENOMEMORY;
    case BT_CS_GN_BAD_CMD_STATE:
      return EBADSTATE;
    case OI_STATUS_INVALID_PARAMETERS:
      return EBADPARM;
    default:
      return EFAILED;
  }
}

static OEMBTExtICPobj_t* OEMBTExtICP_FindMe(bt_app_id_type appId)
{
  OEMBTExtICPobj_t* pMe;

  for (pMe = gMgr.pNextICP; pMe != NULL; pMe = pMe->pNextICP)
  {
    if (pMe->appId == appId)
    {
      return pMe;
    }
  }
  return NULL;
}

static uint16 OEMBTExtICP_MapCmdStatus( bt_cmd_status_type status )
{
  switch ( status )
  {
    case OI_OK:
      return AEEBT_ICP_ERR_NONE;
    case OI_STATUS_INVALID_PARAMETERS:
      return AEEBT_ICP_ERR_BAD_PARAM;
    case OI_STATUS_NO_RESOURCES:
      return AEEBT_ICP_ERR_RESOURCE;
    case OI_STATUS_ALREADY_REGISTERED:
      return AEEBT_ICP_ERR_REC_EXISTS;
    case OI_SDP_DATABASE_OUT_OF_RESOURCES:
      return AEEBT_ICP_ERR_DB_FULL;
    default:
      return AEEBT_ICP_ERR_FAILED;
  }
}

static boolean OEMBTExtICP_HandleCmdDone( 
  bt_ev_gn_cmd_done_type* pCmdDn, 
  AEEBTNotification*      pN,
  OEMBTExtICPobj_t*       pMe
)
{
  boolean doSendNotif = TRUE;

  MSG_LOW( "ICP_HandleCmdDone - st=%x stat=%x cmd=%x", pMe->state,
           pCmdDn->cmd_status, pCmdDn->cmd_type );

  switch ( pCmdDn->cmd_type )
  {
    case BT_PF_CMD_ICP_REGISTER:
    {
      pN->uID = AEEBT_ICP_EVT_REG_DONE;
      pN->data.uError = OEMBTExtICP_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ICP_ERR_NONE )
      {
        pMe->state = AEEBT_ICP_STATE_REGISTERED;
      }
      break;
    }
    case BT_PF_CMD_ICP_DEREGISTER:
    {
      pN->uID = AEEBT_ICP_EVT_DEREG_DONE;
      pN->data.uError = OEMBTExtICP_MapCmdStatus( pCmdDn->cmd_status );
      pMe->state = AEEBT_ICP_STATE_INIT;
      break;
    }
    case BT_EV_PF_ICP_DCN_CFM:
    {
      pN->uID = AEEBT_ICP_EVT_DISCONNECTED;
      pN->data.uError = OEMBTExtICP_MapCmdStatus( pCmdDn->cmd_status );
      pMe->state = AEEBT_ICP_STATE_INIT;
      break;
    }
    case BT_EV_PF_ICP_REL_IND:
    {
      pN->uID = AEEBT_ICP_EVT_CALL_RELEASED;  
      pN->data.uError = OEMBTExtICP_MapCmdStatus( pCmdDn->cmd_status );
      pMe->state = AEEBT_ICP_STATE_DISCONNECTED;
      
      break;
    }
    default:
    {
      doSendNotif = FALSE;
    }
  }

  return doSendNotif;
}

// called from BT task context; so just queue the event
static void OEMBTExtICP_EventCallback( bt_ev_msg_type* ev_msg_ptr )
{
  OEMBTExtICPobj_t*  pMe = OEMBTExtICP_FindMe( ev_msg_ptr->ev_hdr.bt_app_id );
  AEEBTNotification* pN  = NULL;

  if ( pMe == NULL )
  {
    return; // probably for an app that no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "A2DPEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }

  pN->data.uError  = AEEBT_ICP_ERR_NONE;

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
    {
      if ( ! OEMBTExtICP_HandleCmdDone( &ev_msg_ptr->ev_msg.ev_gn_cmd_done,
                                        pN, pMe ) )
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return; // no need to send notification to app
      }
      break;
    }
    case BT_EV_PF_ICP_CON_IND:
    {
      MSG_LOW( "ICPEvCb: con ind ", 0, 0, 0 );

      pMe->BDAddr = 
        *((AEEBTBDAddr*)&ev_msg_ptr->ev_msg.ev_icp_connect_ind.bd_addr);

      pN->uID = AEEBT_ICP_EVT_CONN_REQ;
      pN->data.bdAddr = pMe->BDAddr;

      break;
    }
    case BT_EV_PF_ICP_CON_CFM:
    {
      MSG_LOW( "ICPEvCb: con cfm, status=%x", 
               ev_msg_ptr->ev_msg.ev_icp_connect_cfm.status, 0, 0 );

      if (ev_msg_ptr->ev_msg.ev_icp_connect_cfm.status == BT_CS_GN_SUCCESS )
      {
        pMe->state = AEEBT_ICP_STATE_CONNECTED;
        pN->uID = AEEBT_ICP_EVT_CONNECTED;
      }
      else
      {
        pN->uID = AEEBT_ICP_EVT_CONNECT_FAILED;
      }
      pN->data.bdAddr = pMe->BDAddr;

      break;
    }
    case BT_EV_PF_ICP_DCN_CFM:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_icp_disconnect_cfm.bd_addr ) == FALSE )
      {
        MSG_ERROR( "ICPEvCb: dcn cfm, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "ICPEvCb: dcn cfm ", 0, 0, 0 );

      pMe->state = AEEBT_ICP_STATE_INIT;

      pN->uID    = AEEBT_ICP_EVT_DISCONNECTED;
      pN->data.bdAddr = pMe->BDAddr;

      break;
    }
    case BT_EV_PF_ICP_EST_IND:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_icp_establish_ind.bd_addr ) == FALSE )
      {
        MSG_ERROR( "ICPEvCb: est ind, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "ICPEvCb: est ind ", 0, 0, 0 );

      pMe->state = AEEBT_ICP_STATE_CALL_SETUP_PENDING;

      pN->uID = AEEBT_ICP_EVT_CALL_SETUP_REQ;
      pN->data.bdAddr = pMe->BDAddr;

      break;
    }
    case BT_EV_PF_ICP_EST_CFM:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_icp_establish_cfm.bd_addr ) == FALSE )
      {
        MSG_ERROR( "ICPEvCb: est cfm, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "ICPEvCb: est cfm, status = %x", 
               ev_msg_ptr->ev_msg.ev_icp_establish_cfm.status, 0, 0 );

      if (ev_msg_ptr->ev_msg.ev_icp_establish_cfm.status == BT_CS_GN_SUCCESS )
      {
        pMe->state = AEEBT_ICP_STATE_CALL_SETUP;
        pN->uID = AEEBT_ICP_EVT_CALL_SETUP_SUCCESS;
      }
      else
      {
        pMe->state = AEEBT_ICP_STATE_CONNECTED;
        pN->uID = AEEBT_ICP_EVT_CALL_SETUP_FAILED;
      }
      pN->data.bdAddr = pMe->BDAddr;

      break;
    }
    case BT_EV_PF_ICP_REL_IND:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_icp_release_ind.bd_addr ) == FALSE )
      {
        MSG_ERROR( "ICPEvCb: rel ind, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "ICPEvCb: rel ind ", 0, 0, 0 );

      pMe->state = AEEBT_ICP_STATE_DISCONNECTED;
      
      pN->uID = AEEBT_ICP_EVT_CALL_RELEASED;
      pN->data.bdAddr = pMe->BDAddr;
      pN->data.IcpObject.cause = ev_msg_ptr->ev_msg.ev_icp_release_ind.cause;
      
      break;
    }
    case BT_EV_PF_ICP_OUT_STATUS:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_icp_outgoing_status.bd_addr ) == FALSE )
      {
        MSG_ERROR( "ICPEvCb: out status, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "ICPEvCb: out status ", 0, 0, 0 );

      pN->uID = AEEBT_ICP_EVT_OUTGOING_STATUS;
      pN->data.bdAddr = pMe->BDAddr;
      pN->data.IcpObject.status = ev_msg_ptr->ev_msg.ev_icp_outgoing_status.status;
      
      break;
    }
    default:
    {
      MSG_ERROR( "ICPEvCb - unexpect event %x", 
                 ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_ICP );
  IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
}

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP)

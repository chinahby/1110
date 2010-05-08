/*===========================================================================

FILE:      OEMBTExtCTP.c

SERVICES:  BlueTooth Cordless Telephony Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Cordless Telephony 
        Profile.
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtCTP.c#1 $

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
#include "OEMBTExtCTP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================
typedef enum
{
  AEEBT_CTP_STATE_INIT,
  AEEBT_CTP_STATE_REGISTERED,
  AEEBT_CTP_STATE_CONNECTED,
  AEEBT_CTP_STATE_DISCONNECTED,
  AEEBT_CTP_STATE_CALL_SETUP_PENDING,
  AEEBT_CTP_STATE_CALL_SETUP
} AEEBTCTPState;


typedef struct OEMBTExtCTPobj_struct
{
  ACONTEXT*                   pac;
  IBTExtNotifier*             pNotifier;

  bt_app_id_type              appId;
  AEEBTBDAddr                 BDAddr;         // remote device's

  AEEBTCTPState               state;

  struct OEMBTExtCTPobj_struct* pNextCTP;  // linked into mgr
} OEMBTExtCTPobj_t;

typedef struct
{
  boolean                     bInUse;
  bt_event_type               event;
  bt_app_id_type              appId;
  uint16                      connId;
  AEECallback                 cb;
} OEMBTExtCTP_EvCb;

// one of these
typedef struct
{
  OEMBTExtCTPobj_t* pNextCTP;    // singly linked list of CTPs
  OEMBTExtCTP_EvCb  FreeEvCb[ 10 ];
} OEMBTExtCTP_Mgr;

//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static int  OEMBTExtCTP_CheckCmdStatus( bt_cmd_status_type stat );
static void OEMBTExtCTP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtCTP_Mgr        gMgr;               // manager object

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtCTP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtCTP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtCTP_Init(IBTExtCTP* pParent)
{
  OEMBTExtCTPobj_t* pMe;

  // allocate pMe
  pMe = MALLOCREC(OEMBTExtCTPobj_t);
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
  pMe->appId = bt_cmd_ec_get_app_id_and_register(OEMBTExtCTP_EventCallback);
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

  if ( gMgr.pNextCTP == NULL ) // first time?
  {
    uint8 i;
    for ( i = 0; i < (sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++ )
    {
      gMgr.FreeEvCb[ i ].bInUse = FALSE;
    }
  }
  // link into mgr
  pMe->pNextCTP = gMgr.pNextCTP;
  gMgr.pNextCTP = pMe;

  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtCTP_Release( IBTExtCTP* pParent )
{
  OEMBTExtCTPobj_t*  pMe;
  OEMBTExtCTPobj_t** ppc;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) == TRUE )
  {
    // unlink from mgr
    for (ppc = &gMgr.pNextCTP; *ppc != NULL; ppc = &(*ppc)->pNextCTP)
    {
      if ((*ppc)->appId == pMe->appId)
      {
        *ppc = pMe->pNextCTP;
        pMe->pNextCTP = NULL;
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

// IBTEXTCTP_Connect (see AEEBTExtCTP.h)
int OEMBTExtCTP_Connect( 
  IBTExtCTP*          pParent, 
  const AEEBTBDAddr*  pBDAddr
)
{
  bt_cmd_status_type  stat;
  OEMBTExtCTPobj_t    *pMe;

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

  // Remember BD Addr of CTP Gateway
  pMe->BDAddr = *pBDAddr;

  stat = bt_cmd_pf_ctp_connect( pMe->appId, 
                                (bt_bd_addr_type*) pBDAddr );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

// IBTEXTCTP_AcceptConnection (see AEEBTExtCTP.h)
int OEMBTExtCTP_AcceptConnection( 
  IBTExtCTP*          pParent, 
  const AEEBTBDAddr*  pBDAddr, 
  boolean             bAccept
)
{
  bt_cmd_status_type  stat;
  OEMBTExtCTPobj_t    *pMe;

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

  stat = bt_cmd_pf_ctp_accept_connection( pMe->appId, 
                                          (bt_bd_addr_type*) pBDAddr,
                                          bAccept ); 

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

// IBTEXTCTP_Disconnect (see AEEBTExtCTP.h)
int OEMBTExtCTP_Disconnect( IBTExtCTP* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtCTPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_disconnect( pMe->appId, 
                                   (bt_bd_addr_type*) &pMe->BDAddr );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

// IBTEXTCTP_ClearCall (see AEEBTExtCTP.h)
int OEMBTExtCTP_ClearCall(   
  IBTExtCTP*          pParent, 
  AEEBTCTPCauseType   cause
)
{
  bt_cmd_status_type  stat;
  OEMBTExtCTPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_clear_call( pMe->appId, 
                                   (bt_bd_addr_type*) &pMe->BDAddr,
                                   (bt_pf_ctp_cause_type) cause );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

// IBTEXTCTP_EstablishCall (see AEEBTExtCTP.h)
int OEMBTExtCTP_EstablishCall(   
  IBTExtCTP*                pParent, 
  const AEEBTCTPNumberType* pCallingNum,
  const AEEBTCTPNumberType* pCalledNum
)
{
  bt_cmd_status_type  stat;
  OEMBTExtCTPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_establish_call( pMe->appId, 
                                      (bt_bd_addr_type*) &pMe->BDAddr,
                                       (bt_pf_ctp_number_type*) pCallingNum,
                                       (bt_pf_ctp_number_type*) pCalledNum );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

// IBTEXTCTP_Info(see AEEBTExtCTP.h)
int OEMBTExtCTP_Info(   
  IBTExtCTP*                pParent, 
  boolean                   bSendingCmplt,
  char                      keypadFacility,
  const AEEBTCTPNumberType* pCallingNum,
  const AEEBTCTPNumberType* pCalledNum,
  AEEBTCTPAudioControlType  audioControl
)
{
  bt_cmd_status_type  stat;
  OEMBTExtCTPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_info( pMe->appId, 
                             (bt_bd_addr_type*) &pMe->BDAddr,
                             bSendingCmplt,
                             keypadFacility,
                             (bt_pf_ctp_number_type*) pCallingNum,
                             (bt_pf_ctp_number_type*) pCalledNum,
                             (bt_pf_ctp_audio_control_type) audioControl );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

// IBTEXTCTP_IncomingStatus (see AEEBTExtCTP.h)
int OEMBTExtCTP_IncomingStatus(   
  IBTExtCTP*             pParent, 
  AEEBTCTPCallStatusType status
)
{
  bt_cmd_status_type  stat;
  OEMBTExtCTPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_incoming_status( pMe->appId, 
                                       (bt_bd_addr_type*) &pMe->BDAddr,
                                       (bt_pf_ctp_call_status_type) status );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

// IBTEXTCTP_AcceptCall (see AEEBTExtCTP.h)
int OEMBTExtCTP_AcceptCall( IBTExtCTP* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtCTPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_accept_call( pMe->appId, 
                                    (bt_bd_addr_type*) &pMe->BDAddr );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

// IBTEXTCTP_StartDTMF (see AEEBTExtCTP.h)
int OEMBTExtCTP_StartDTMF(   
  IBTExtCTP*             pParent, 
  char                   tone
)
{
  bt_cmd_status_type  stat;
  OEMBTExtCTPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_start_dtmf( pMe->appId, 
                                   (bt_bd_addr_type*) &pMe->BDAddr,
                                   tone );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

// IBTEXTCTP_StopDTMF (see AEEBTExtCTP.h)
int OEMBTExtCTP_StopDTMF( IBTExtCTP* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtCTPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_stop_dtmf( pMe->appId, 
                                  (bt_bd_addr_type*) &pMe->BDAddr );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

// IBTEXTCTP_Register (see AEEBTExtCTP.h)
int OEMBTExtCTP_Register( IBTExtCTP* pParent )
{
  bt_cmd_status_type stat;
  OEMBTExtCTPobj_t   *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_register( pMe->appId );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}


// IBTEXTCTP_Deregister (see AEEBTExtCTP.h)
int OEMBTExtCTP_Deregister( IBTExtCTP* pParent )
{
  bt_cmd_status_type          stat;
  OEMBTExtCTPobj_t            *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_deregister( pMe->appId );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}


// IBTEXTCTP_RegisterRecall (see AEEBTExtCTP.h)
int OEMBTExtCTP_RegisterRecall( IBTExtCTP* pParent )
{
  bt_cmd_status_type          stat;
  OEMBTExtCTPobj_t            *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ctp_register_recall( pMe->appId,
                                        (bt_bd_addr_type*) &pMe->BDAddr );

  return OEMBTExtCTP_CheckCmdStatus( stat );
}

//==========================================================================
//   static helper functions
//==========================================================================
static int OEMBTExtCTP_CheckCmdStatus( bt_cmd_status_type stat )
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

static OEMBTExtCTPobj_t* OEMBTExtCTP_FindMe(bt_app_id_type appId)
{
  OEMBTExtCTPobj_t* pMe;

  for (pMe = gMgr.pNextCTP; pMe != NULL; pMe = pMe->pNextCTP)
  {
    if (pMe->appId == appId)
    {
      return pMe;
    }
  }
  return NULL;
}

static uint16 OEMBTExtCTP_MapCmdStatus( bt_cmd_status_type status )
{
  switch ( status )
  {
    case OI_OK:
      return AEEBT_CTP_ERR_NONE;
    case OI_STATUS_INVALID_PARAMETERS:
      return AEEBT_CTP_ERR_BAD_PARAM;
    case OI_STATUS_NO_RESOURCES:
      return AEEBT_CTP_ERR_RESOURCE;
    case OI_STATUS_ALREADY_REGISTERED:
      return AEEBT_CTP_ERR_REC_EXISTS;
    case OI_SDP_DATABASE_OUT_OF_RESOURCES:
      return AEEBT_CTP_ERR_DB_FULL;
    default:
      return AEEBT_CTP_ERR_FAILED;
  }
}

static boolean OEMBTExtCTP_HandleCmdDone( 
  bt_ev_gn_cmd_done_type* pCmdDn, 
  AEEBTNotification*      pN,
  OEMBTExtCTPobj_t*       pMe
)
{
  boolean doSendNotif = TRUE;

  MSG_LOW( "CTP_HandleCmdDone - st=%x stat=%x cmd=%x", pMe->state,
           pCmdDn->cmd_status, pCmdDn->cmd_type );

  switch ( pCmdDn->cmd_type )
  {
    case BT_PF_CMD_CTP_REGISTER:
    {
      pN->uID = AEEBT_CTP_EVT_REG_DONE;
      pN->data.uError = OEMBTExtCTP_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_CTP_ERR_NONE )
      {
        pMe->state = AEEBT_CTP_STATE_REGISTERED;
      }
      break;
    }
    case BT_PF_CMD_CTP_DEREGISTER:
    {
      pN->uID = AEEBT_CTP_EVT_DEREG_DONE;
      pN->data.uError = OEMBTExtCTP_MapCmdStatus( pCmdDn->cmd_status );
      pMe->state = AEEBT_CTP_STATE_INIT;
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
static void OEMBTExtCTP_EventCallback( bt_ev_msg_type* ev_msg_ptr )
{
  OEMBTExtCTPobj_t*  pMe = OEMBTExtCTP_FindMe( ev_msg_ptr->ev_hdr.bt_app_id );
  AEEBTNotification* pN  = NULL;

  if ( pMe == NULL )
  {
    return; // probably for an app that no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "CTPEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }

  pN->data.uError  = AEEBT_CTP_ERR_NONE;

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
    {
      if ( ! OEMBTExtCTP_HandleCmdDone( &ev_msg_ptr->ev_msg.ev_gn_cmd_done,
                                        pN, pMe ) )
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return; // no need to send notification to app
      }
      break;
    }
    case BT_EV_PF_CTP_CON_IND:
    {
      MSG_LOW( "CTPEvCb: con ind ", 0, 0, 0 );

      pMe->BDAddr = 
        *((AEEBTBDAddr*)&ev_msg_ptr->ev_msg.ev_ctp_connect_ind.bd_addr);

      pN->uID = AEEBT_CTP_EVT_CONN_REQ;
      pN->data.bdAddr = pMe->BDAddr;

      break;
    }
    case BT_EV_PF_CTP_CON_CFM:
    {
      MSG_LOW( "CTPEvCb: con cfm, status=%x", 
               ev_msg_ptr->ev_msg.ev_ctp_connect_cfm.status, 0, 0 );

      if (ev_msg_ptr->ev_msg.ev_ctp_connect_cfm.status == BT_CS_GN_SUCCESS )
      {
        pMe->state = AEEBT_CTP_STATE_CONNECTED;
        pN->uID = AEEBT_CTP_EVT_CONNECTED;
      }
      else
      {
        pN->uID = AEEBT_CTP_EVT_CONNECT_FAILED;
      }
      pN->data.bdAddr = pMe->BDAddr;

      break;
    }
    case BT_EV_PF_CTP_DCN_CFM:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_ctp_disconnect_cfm.bd_addr ) == FALSE )
      {
        MSG_ERROR( "CTPEvCb: dcn cfm, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "CTPEvCb: dcn cfm ", 0, 0, 0 );

      pMe->state = AEEBT_CTP_STATE_INIT;

      pN->uID    = AEEBT_CTP_EVT_DISCONNECTED;
      pN->data.bdAddr = pMe->BDAddr;

      break;
    }
    case BT_EV_PF_CTP_EST_IND:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_ctp_establish_ind.bd_addr ) == FALSE )
      {
        MSG_ERROR( "CTPEvCb: est ind, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "CTPEvCb: est ind ", 0, 0, 0 );

      pMe->state = AEEBT_CTP_STATE_CALL_SETUP_PENDING;

      pN->uID = AEEBT_CTP_EVT_CALL_SETUP_REQ;
      pN->data.bdAddr = pMe->BDAddr;

      break;
    }
    case BT_EV_PF_CTP_EST_CFM:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_ctp_establish_cfm.bd_addr ) == FALSE )
      {
        MSG_ERROR( "CTPEvCb: est cfm, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "CTPEvCb: est cfm, status = %x", 
               ev_msg_ptr->ev_msg.ev_ctp_establish_cfm.status, 0, 0 );

      if (ev_msg_ptr->ev_msg.ev_ctp_establish_cfm.status == BT_CS_GN_SUCCESS )
      {
        pMe->state = AEEBT_CTP_STATE_CALL_SETUP;
        pN->uID = AEEBT_CTP_EVT_CALL_SETUP_SUCCESS;
      }
      else
      {
        pMe->state = AEEBT_CTP_STATE_CONNECTED;
        pN->uID = AEEBT_CTP_EVT_CALL_SETUP_FAILED;
      }
      pN->data.bdAddr = pMe->BDAddr;

      break;
    }
    case BT_EV_PF_CTP_INFO_IND:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_ctp_info_ind.bd_addr ) == FALSE )
      {
        MSG_ERROR( "CTPEvCb: info ind, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "CTPEvCb: info ind", 0, 0, 0 );

      pN->uID = AEEBT_CTP_EVT_CALL_INFO_REQ;
      pN->data.bdAddr = pMe->BDAddr;

      pN->data.CtpObject.bSendingCmplt  = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.sending_complete;
      pN->data.CtpObject.keypadFacility = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.keypad_facility;
      
      // Calling number 
      pN->data.CtpObject.callingNumber.bCalling = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.calling_number.calling;
      pN->data.CtpObject.callingNumber.type = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.calling_number.type;
      pN->data.CtpObject.callingNumber.plan = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.calling_number.plan;
      pN->data.CtpObject.callingNumber.presentation = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.calling_number.presentation;
      pN->data.CtpObject.callingNumber.screening = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.calling_number.screening;
      pN->data.CtpObject.callingNumber.numDigits = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.calling_number.num_digits;
      STRLCPY( (char*) &pN->data.CtpObject.callingNumber.digits, 
               (char*) &ev_msg_ptr->ev_msg.ev_ctp_info_ind.calling_number.digits,
               AEEBT_TCS_MAX_DIGITS );
      
      // Called number
      pN->data.CtpObject.calledNumber.bCalling = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.called_number.calling;
      pN->data.CtpObject.calledNumber.type = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.called_number.type;
      pN->data.CtpObject.calledNumber.plan = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.called_number.plan;
      pN->data.CtpObject.calledNumber.presentation = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.called_number.presentation;
      pN->data.CtpObject.calledNumber.screening = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.called_number.screening;
      pN->data.CtpObject.calledNumber.numDigits = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.called_number.num_digits;
      STRLCPY( (char *) &pN->data.CtpObject.calledNumber.digits, 
               (char *) &ev_msg_ptr->ev_msg.ev_ctp_info_ind.called_number.digits,
               AEEBT_TCS_MAX_DIGITS );

      pN->data.CtpObject.audioControl   = 
        ev_msg_ptr->ev_msg.ev_ctp_info_ind.audio_control;

      break;
    }
    case BT_EV_PF_CTP_DTMF_START:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_ctp_dtmf_start.bd_addr ) == FALSE )
      {
        MSG_ERROR( "CTPEvCb: dtmf start, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "CTPEvCb: dtmf start ", 0, 0, 0 );

      pN->uID = AEEBT_CTP_EVT_DTMF_START;
      pN->data.bdAddr = pMe->BDAddr;
      pN->data.CtpObject.tone  = ev_msg_ptr->ev_msg.ev_ctp_dtmf_start.tone;
      
      break;
    }
    case BT_EV_PF_CTP_DTMF_STOP:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_ctp_dtmf_stop.bd_addr ) == FALSE )
      {
        MSG_ERROR( "CTPEvCb: dtmf stop, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "CTPEvCb: dtmf stop ", 0, 0, 0 );

      pN->data.bdAddr = pMe->BDAddr;
      pN->uID = AEEBT_CTP_EVT_DTMF_STOP;
      break;
    }
    case BT_EV_PF_CTP_REL_IND:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_ctp_release_ind.bd_addr ) == FALSE )
      {
        MSG_ERROR( "CTPEvCb: rel ind, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "CTPEvCb: rel ind ", 0, 0, 0 );

      pMe->state = AEEBT_CTP_STATE_DISCONNECTED;
      
      pN->uID = AEEBT_CTP_EVT_CALL_RELEASED;
      pN->data.bdAddr = pMe->BDAddr;
      pN->data.CtpObject.cause = ev_msg_ptr->ev_msg.ev_ctp_release_ind.cause;
      
      break;
    }
    case BT_EV_PF_CTP_OUT_STATUS:
    {
      if ( AEEBT_BD_ADDRS_EQUAL( 
             &pMe->BDAddr, 
             &ev_msg_ptr->ev_msg.ev_ctp_outgoing_status.bd_addr ) == FALSE )
      {
        MSG_ERROR( "CTPEvCb: out status, wrong bd_addr", 0, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "CTPEvCb: out status ", 0, 0, 0 );

      pN->uID = AEEBT_CTP_EVT_OUTGOING_STATUS;
      pN->data.bdAddr = pMe->BDAddr;
      pN->data.CtpObject.status = ev_msg_ptr->ev_msg.ev_ctp_outgoing_status.status;
      
      break;
    }
    default:
    {
      MSG_ERROR( "CTPEvCb - unexpect event %x", 
                 ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_CTP );
  IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
}

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP)

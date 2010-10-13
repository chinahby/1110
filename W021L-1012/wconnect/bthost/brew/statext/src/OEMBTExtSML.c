/*===========================================================================

FILE:      OEMBTExtSML.c

SERVICES:  BlueTooth SyncML Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth SyncML Profile.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtSML.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_SML)
#include "bt.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "oi_obexspec.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtSML.h"

//==========================================================================
//   Macro definitions
//==========================================================================
#define SML_DSM_ITEM_SIZE   500

//==========================================================================
//   Type definitions
//==========================================================================

typedef struct OEMBTExtSMLobj_struct
{
  ACONTEXT*                   pac;
  IBTExtNotifier*             pNotifier;

  bt_app_id_type              appId;
  AEEBTBDAddr                 BDAddr;         // remote device's

  IShell*                     pIShell;

  IFile*                      pFile;
  FileInfo                    fileInfo;
  AECHAR                      wFileName[ AEEBT_MAX_FILE_NAME + 1 ];
  char                        type[ 100 ];

  uint32                      bytesSent;
  uint32                      bytesRcvd;

  uint16                      serverConnID;
  uint16                      clientConnID;

  dsm_item_type*              dsm_ptr;

  AEEBTSMLbyteSeqType         data;    

  boolean                     bReadOnly;

  boolean                     bIsServer;
  boolean                     bDoDeregister; 

  AEECallback                 cb;

  struct OEMBTExtSMLobj_struct* pNextSML;  // linked into mgr
} OEMBTExtSMLobj_t;

typedef struct
{
  boolean                     bInUse;

  bt_event_type               event;
  bt_app_id_type              appId;
  uint16                      connId;

  bt_cmd_status_type          status;
} OEMBTExtSML_EvCb;

// one of these
typedef struct
{
  OEMBTExtSMLobj_t* pNextSML;    // singly linked list of SMLs
  OEMBTExtSML_EvCb  FreeEvCb[ 10 ];
} OEMBTExtSML_Mgr;

//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static int  OEMBTExtSML_CheckCmdStatus( bt_cmd_status_type stat );
static void OEMBTExtSML_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);
static OEMBTExtSML_EvCb* OEMBTExtSML_GetFreeEvCb( void );

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtSML_Mgr        gMgr;               // manager object

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtSML_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtSML *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtSML_Init(IBTExtSML* pParent)
{
  OEMBTExtSMLobj_t* pMe;

  // allocate pMe
  pMe = MALLOCREC(OEMBTExtSMLobj_t);
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
  pMe->appId = bt_cmd_ec_get_app_id_and_register(OEMBTExtSML_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    FREE( pMe );
    return EFAILED;
  }

  pMe->dsm_ptr = bt_get_free_dsm_ptr( BT_TL_RFCOMM, SML_DSM_ITEM_SIZE );
  if ( pMe->dsm_ptr == NULL )
  {
    (void)bt_cmd_ec_free_application_id( pMe->appId );
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

  if ( gMgr.pNextSML == NULL ) // first time?
  {
    uint8 i;
    for ( i = 0; i < (sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++ )
    {
      gMgr.FreeEvCb[ i ].bInUse = FALSE;
    }
  }

  pMe->pIShell = pParent->m_pIShell;

  // link into mgr
  pMe->pNextSML = gMgr.pNextSML;
  gMgr.pNextSML = pMe;

  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtSML_Release( IBTExtSML* pParent )
{
  OEMBTExtSMLobj_t*  pMe;
  OEMBTExtSMLobj_t** ppc;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) == TRUE )
  {
    // unlink from mgr
    for (ppc = &gMgr.pNextSML; *ppc != NULL; ppc = &(*ppc)->pNextSML)
    {
      if ((*ppc)->appId == pMe->appId)
      {
        *ppc = pMe->pNextSML;
        pMe->pNextSML = NULL;
        break;
      }
    }

    dsm_free_packet( &pMe->dsm_ptr );
    (void)bt_cmd_ec_free_application_id( pMe->appId );
    pMe->appId = BT_APP_ID_NULL;

    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    pMe->pNotifier = NULL;
    pMe->pac = NULL;

    FREE(pMe);
    (void) AEEHandle_Clear( &gOEMBTExtHandleList, pParent->m_hBT );

    return SUCCESS;
  }

  return EFAILED;
}

// IBTEXTSML_RegisterClient (see AEEBTExtSML.h)
int OEMBTExtSML_RegisterClient( 
  IBTExtSML*     pParent, 
  const char*    pServiceUUID,
  boolean        bAuth )
{
  bt_cmd_status_type stat;
  OEMBTExtSMLobj_t   *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pServiceUUID == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_pf_sml_register_client( pMe->appId,  
                                        (char*) pServiceUUID,
                                        bAuth );

  return OEMBTExtSML_CheckCmdStatus( stat );
}

// IBTEXTSML_RegisterServer (see AEEBTExtSML.h)
int OEMBTExtSML_RegisterServer( 
  IBTExtSML*     pParent, 
  const char*    pServiceUUID,
  boolean        bAuth )
{
  bt_cmd_status_type stat;
  OEMBTExtSMLobj_t   *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pServiceUUID == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_pf_sml_register_server( pMe->appId,  
                                        (char*) pServiceUUID,
                                        bAuth );

  return OEMBTExtSML_CheckCmdStatus( stat );
}

// IBTEXTSML_DeregisterClient (see AEEBTExtSML.h)
int OEMBTExtSML_DeregisterClient( IBTExtSML* pParent )
{
  bt_cmd_status_type          stat;
  OEMBTExtSMLobj_t            *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_sml_deregister_client( pMe->appId );

  return OEMBTExtSML_CheckCmdStatus( stat );
}

// IBTEXTSML_DeregisterServer (see AEEBTExtSML.h)
int OEMBTExtSML_DeregisterServer( IBTExtSML* pParent )
{
  bt_cmd_status_type          stat;
  OEMBTExtSMLobj_t            *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_sml_deregister_server( pMe->appId );

  return OEMBTExtSML_CheckCmdStatus( stat );
}

// IBTEXTSML_AcceptConnection (see AEEBTExtSML.h)
int OEMBTExtSML_AcceptConnection( 
  IBTExtSML*          pParent,
  boolean             bAccept
)
{
  bt_cmd_status_type  stat;
  OEMBTExtSMLobj_t    *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_sml_accept_connect( pMe->appId, 
                                       pMe->serverConnID,
                                       bAccept,
                                       BT_CS_GN_SUCCESS );


  return OEMBTExtSML_CheckCmdStatus( stat );
}

// IBTEXTSML_Connect (see AEEBTExtSML.h)
int OEMBTExtSML_Connect( 
  IBTExtSML*             pParent, 
  const AEEBTBDAddr*     pBDAddr, 
  uint8                  uChannelNumber,
  const char*            pTarget )
{
  bt_cmd_status_type  stat;
  OEMBTExtSMLobj_t    *pMe;

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

  stat = bt_cmd_pf_sml_connect( pMe->appId, 
                                (bt_bd_addr_type*) pBDAddr,
                                uChannelNumber,
                                (char*) pTarget );

  return OEMBTExtSML_CheckCmdStatus( stat );
}

// IBTEXTSML_Disconnect (see AEEBTExtSML.h)
int OEMBTExtSML_Disconnect( IBTExtSML* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtSMLobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_sml_disconnect( pMe->appId, pMe->clientConnID );
 
  return OEMBTExtSML_CheckCmdStatus( stat );
}

// IBTEXTSML_Get (see AEEBTExtSML.h)
int OEMBTExtSML_Get( 
  IBTExtSML*    pParent, 
  const char*   pType )
{
  bt_cmd_status_type  stat;
  OEMBTExtSMLobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pType == NULL ) 
  {
    return EBADPARM;
  }

  stat = bt_cmd_pf_sml_get( pMe->appId, 
                            pMe->clientConnID,
                            (char*) pType );

  return OEMBTExtSML_CheckCmdStatus( stat );
}

// IBTEXTSML_Put (see AEEBTExtSML.h)
int OEMBTExtSML_Put( 
  IBTExtSML*                 pParent, 
  const char*                pType,
  const AEEBTSMLbyteSeqType* pData )
{
  bt_cmd_status_type  stat;
  OEMBTExtSMLobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pType == NULL || pData == NULL ) 
  {
    return EBADPARM;
  }

  stat = bt_cmd_pf_sml_put( pMe->appId, 
                            pMe->clientConnID,
                            (char*) pType,
                            (bt_pf_byte_seq_type*) pData,
                            BT_CS_GN_SUCCESS);

  return OEMBTExtSML_CheckCmdStatus( stat );
}

// IBTEXTSML_GetResponse (see AEEBTExtSML.h)
int OEMBTExtSML_GetResponse( 
  IBTExtSML*                 pParent, 
  const AEEBTSMLbyteSeqType* pData )
{
  bt_cmd_status_type  stat;
  OEMBTExtSMLobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pData == NULL ) 
  {
    return EBADPARM;
  }

  stat = bt_cmd_pf_sml_get_rsp( pMe->appId, 
                                pMe->clientConnID,
                                (bt_pf_byte_seq_type*) pData,
                                BT_CS_GN_SUCCESS);

  return OEMBTExtSML_CheckCmdStatus( stat );
}

// IBTEXTSML_PutResponse (see AEEBTExtSML.h)
int OEMBTExtSML_PutResponse( IBTExtSML* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtSMLobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_sml_put_rsp( pMe->appId, 
                                pMe->clientConnID,
                                BT_CS_GN_SUCCESS);

  return OEMBTExtSML_CheckCmdStatus( stat );
}

//==========================================================================
//   static helper functions
//==========================================================================
static int OEMBTExtSML_CheckCmdStatus( bt_cmd_status_type stat )
{
  switch (stat)
  {
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return SUCCESS;
    case BT_CS_GN_CMD_Q_FULL:
      return ENOMEMORY;
    case OI_STATUS_INVALID_PARAMETERS:
      return EBADPARM;
    default:
      return EFAILED;
  }
}

static OEMBTExtSMLobj_t* OEMBTExtSML_FindMe(bt_app_id_type appId)
{
  OEMBTExtSMLobj_t* pMe;

  for (pMe = gMgr.pNextSML; pMe != NULL; pMe = pMe->pNextSML)
  {
    if (pMe->appId == appId)
    {
      return pMe;
    }
  }
  return NULL;
}

static boolean OEMBTExtSML_HandleCmdDone( 
  bt_ev_gn_cmd_done_type* pCmdDn, 
  AEEBTNotification*      pN,
  OEMBTExtSMLobj_t*       pMe
)
{
  boolean doSendNotif = TRUE;

  MSG_LOW( "SML_HandleCmdDone - stat=%x cmd=%x", pCmdDn->cmd_status, pCmdDn->cmd_type, 0 );

  switch ( pCmdDn->cmd_type )
  {
    case BT_PF_CMD_SML_REGISTER_CLI:
    {
      pN->uID = AEEBT_SML_EVT_REG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_SML_REGISTER_SRV:
    {
      pN->uID = AEEBT_SML_EVT_REG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_SML_DEREGISTER_CLI:
    {
      pN->uID = AEEBT_SML_EVT_DEREG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_SML_DEREGISTER_SRV:
    {
      pN->uID = AEEBT_SML_EVT_DEREG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    default:
    {
      doSendNotif = FALSE;
    }
  }

  return doSendNotif;
}

static OEMBTExtSML_EvCb* OEMBTExtSML_GetFreeEvCb( void )
{
  uint8 i;
  OEMBTExtSML_EvCb* pEvCb = NULL;

  for ( i=0; i<(sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++)
  {
    if ( gMgr.FreeEvCb[ i ].bInUse == FALSE )
    {
      gMgr.FreeEvCb[ i ].bInUse = TRUE;
      pEvCb = &gMgr.FreeEvCb[ i ];
      break;
    }
  }
  return pEvCb;
}

// called from BT task context; so just queue the event
static void OEMBTExtSML_EventCallback( bt_ev_msg_type* ev_msg_ptr )
{
  OEMBTExtSMLobj_t*   pMe    = OEMBTExtSML_FindMe( ev_msg_ptr->ev_hdr.bt_app_id );
  OEMBTExtSML_EvCb*   pEvCb  = OEMBTExtSML_GetFreeEvCb();
  AEEBTNotification*  pN     = NULL;

  // Not being used
  pEvCb->bInUse = FALSE;

  if ( pMe == NULL )
  {
    return; // probably for an app that no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "SMLEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }

  pN->data.uError  = AEEBT_SML_ERR_NONE;

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
    {
      if ( ! OEMBTExtSML_HandleCmdDone( &ev_msg_ptr->ev_msg.ev_gn_cmd_done,
                                        pN, pMe ) )
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return; // no need to send notification to app
      }
      break;
    } 
    case BT_EV_PF_SML_CON_CFM:
    {
      MSG_LOW( "SMLEvCb: con cfm, id=%x stat=%x", 
               ev_msg_ptr->ev_msg.ev_sml_con_cfm.conn_id,
               ev_msg_ptr->ev_msg.ev_sml_con_cfm.status, 0 );
      if (ev_msg_ptr->ev_msg.ev_sml_con_cfm.status == BT_CS_GN_SUCCESS )
      {
        pMe->clientConnID = ev_msg_ptr->ev_msg.ev_sml_con_cfm.conn_id;
        pMe->bReadOnly    = ev_msg_ptr->ev_msg.ev_sml_con_cfm.read_only;

        pN->uID = AEEBT_SML_EVT_CONNECTED;
      }
      else
      {
        pN->uID = AEEBT_SML_EVT_CONN_FAILED;
      }
      break;
    }
    case BT_EV_PF_SML_DCN_CFM:
    {
      MSG_LOW( "SMLEvCb: dcn cfm, id=%x stat=%x", 
               ev_msg_ptr->ev_msg.ev_sml_dcn_cfm.conn_id,
               ev_msg_ptr->ev_msg.ev_sml_dcn_cfm.status, 0 );
      break;
    }
    case BT_EV_PF_SML_CLI_DCN_IND:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_sml_cli_dcn_ind.conn_id )
      {
        MSG_ERROR( "SMLEvCb: dcn ind, wrong conn id=%x", 
                ev_msg_ptr->ev_msg.ev_sml_cli_dcn_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      pN->uID = AEEBT_SML_EVT_DISCONNECTED;
      pMe->clientConnID = BT_PF_SML_NO_CONN_ID;

      if ( pMe->bDoDeregister )
      {
        pMe->bDoDeregister = FALSE;
        bt_cmd_pf_sml_deregister_client( pMe->appId );
      }
      break;
    }
    case BT_EV_PF_SML_CLI_CON_IND:
    {
      MSG_LOW( "SMLEvCb: con ind, id=%x st=%x", 
               ev_msg_ptr->ev_msg.ev_sml_cli_con_ind.conn_id, pMe->state, 0 );

      pMe->serverConnID = ev_msg_ptr->ev_msg.ev_sml_cli_con_ind.conn_id;
      pN->uID = AEEBT_SML_EVT_CONN_REQ;
      break;
    }
    case BT_EV_PF_SML_SRV_CON_IND:
    {
      MSG_LOW( "SMLEvCb: con ind, id=%x st=%x", 
               ev_msg_ptr->ev_msg.ev_sml_srv_con_ind.conn_id, pMe->state, 0 );

      pMe->serverConnID = ev_msg_ptr->ev_msg.ev_sml_srv_con_ind.conn_id;

      MEMCPY( (void*) pN->data.SmlObject.userID,
              (void*) ev_msg_ptr->ev_msg.ev_sml_srv_con_ind.user_id,
              ev_msg_ptr->ev_msg.ev_sml_srv_con_ind.user_id_len );

      pN->data.SmlObject.userIDlen = ev_msg_ptr->ev_msg.ev_sml_srv_con_ind.user_id_len;

      pN->uID = AEEBT_SML_EVT_CONN_REQ;
      break;
    }
    case BT_EV_PF_SML_SRV_DCN_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_sml_srv_dcn_ind.conn_id )
      {
        MSG_ERROR( "SMLEvCb: dcn ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_sml_srv_dcn_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      pN->uID = AEEBT_SML_EVT_DISCONNECTED;
      pMe->serverConnID = BT_PF_SML_NO_CONN_ID;

      if ( pMe->bDoDeregister )
      {
        pMe->bDoDeregister = FALSE;
        bt_cmd_pf_sml_deregister_server( pMe->appId );
      }
      break;
    }
    case BT_EV_PF_SML_CLI_GET_IND:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_sml_cli_get_ind.conn_id )
      {
        MSG_ERROR( "SMLEvCb: cli get ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_sml_cli_get_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "SMLEvCb: cli get ind, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_sml_cli_get_ind.status, 
               ev_msg_ptr->ev_msg.ev_sml_cli_get_ind.conn_id, 0 );

      pN->data.SmlObject.OBEXheadersCount = ev_msg_ptr->ev_msg.ev_sml_cli_get_ind.cmd_headers_cnt;
      pN->data.SmlObject.pOBEXheaders = 
        (AEEBTSMLOBEXhdrType*) ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.cmd_headers_ptr;

      pN->uID = AEEBT_SML_EVT_CLI_GET_REQ;
      break;
    }
    case BT_EV_PF_SML_SRV_GET_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_sml_srv_get_ind.conn_id )
      {
        MSG_ERROR( "SMLEvCb: srv get ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_sml_srv_get_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "SMLEvCb: srv get ind, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_sml_srv_get_ind.status, 
               ev_msg_ptr->ev_msg.ev_sml_srv_get_ind.conn_id, 0 );

      pN->data.SmlObject.OBEXheadersCount = ev_msg_ptr->ev_msg.ev_sml_srv_get_ind.cmd_headers_cnt;
      pN->data.SmlObject.pOBEXheaders = 
        (AEEBTSMLOBEXhdrType*) ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.cmd_headers_ptr;

      pN->uID = AEEBT_SML_EVT_SRV_GET_REQ;
      break;
    }
    case BT_EV_PF_SML_CLI_PUT_IND:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_sml_cli_put_ind.conn_id )
      {
        MSG_ERROR( "SMLEvCb: cli put ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_sml_cli_put_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "SMLEvCb: cli put ind, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_sml_cli_put_ind.status, 
               ev_msg_ptr->ev_msg.ev_sml_cli_put_ind.conn_id, 0 );

      pN->data.SmlObject.OBEXheadersCount = ev_msg_ptr->ev_msg.ev_sml_cli_put_ind.cmd_headers_cnt;
      pN->data.SmlObject.pOBEXheaders = 
        (AEEBTSMLOBEXhdrType*) ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.cmd_headers_ptr;

      pN->uID = AEEBT_SML_EVT_CLI_PUT_REQ;
      break;
    }
    case BT_EV_PF_SML_SRV_PUT_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.conn_id )
      {
        MSG_ERROR( "SMLEvCb: srv put ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "SMLEvCb: srv put ind, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.status, 
               ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.conn_id, 0 );

      pN->data.SmlObject.OBEXheadersCount = ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.cmd_headers_cnt;
      pN->data.SmlObject.pOBEXheaders = 
        (AEEBTSMLOBEXhdrType*) ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.cmd_headers_ptr;

      pN->uID = AEEBT_SML_EVT_SRV_PUT_REQ;
      break;
    }
    case BT_EV_PF_SML_GET_RCV_DATA:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_sml_get_rcv_data.conn_id ||
           pMe->serverConnID != ev_msg_ptr->ev_msg.ev_sml_get_rcv_data.conn_id )
      {
        MSG_ERROR( "SMLEvCb: get rcv data ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_sml_get_rcv_data.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "SMLEvCb: get rcv data ind, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_sml_get_rcv_data.status, 
               ev_msg_ptr->ev_msg.ev_sml_get_rcv_data.conn_id, 0 );

      pN->data.SmlObject.OBEXheadersCount = ev_msg_ptr->ev_msg.ev_sml_get_rcv_data.rsp_headers_cnt;
      pN->data.SmlObject.pOBEXheaders = 
        (AEEBTSMLOBEXhdrType*) ev_msg_ptr->ev_msg.ev_sml_get_rcv_data.rsp_headers_ptr;

      pN->uID = AEEBT_SML_EVT_GET_RCV_DATA;
      break;
    }
    case BT_EV_PF_SML_PUT_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_sml_put_cfm.conn_id ||
           pMe->serverConnID != ev_msg_ptr->ev_msg.ev_sml_put_cfm.conn_id )
      {
        MSG_ERROR( "SMLEvCb: put cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_sml_put_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "SMLEvCb: put cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.status, 
               ev_msg_ptr->ev_msg.ev_sml_srv_put_ind.conn_id, 0 );

      pN->data.SmlObject.OBEXheadersCount = ev_msg_ptr->ev_msg.ev_sml_put_cfm.rsp_headers_cnt;
      pN->data.SmlObject.pOBEXheaders = 
        (AEEBTSMLOBEXhdrType*) ev_msg_ptr->ev_msg.ev_sml_put_cfm.rsp_headers_ptr;

      pN->uID = AEEBT_SML_EVT_PUT_CFM;
      break;
    }
    default:
    {
      MSG_ERROR( "SMLEvCb - unexpect event %x", 
                 ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_SML );
  IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
}

#endif // defined(FEATURE_IBT)

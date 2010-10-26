/*===========================================================================

FILE:      OEMBTExtL2.c

SERVICES:  BlueTooth Radio L2CAP QCT Extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth L2 layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2004-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtL2.c#2 $
$DateTime: 2009/04/14 15:01:41 $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
2009-04-14  pn  Watermark queues get initialized by dsm_queue_init().
11/12/08   JTL  Fix crash when receiving empty packets.
09/15/08   sp   Compiler warning fix

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT)
#include "bt.h"
#include "bti.h"
#ifndef FEATURE_BT_SOC   //modi by yangdecai
#if 0
#include "btlm.h"
#include "btbb.h"
#endif
#endif /* FEATURE_BT_SOC */
#include "msg.h"
#include "dsm.h"

#include "AEENet.h"
#include "AEEBTExt_priv.h"
#include "OEMBTExtL2.h"

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif

#include "OEMHeap.h"

//==========================================================================
//   Macro definitions
//==========================================================================

#define BT_MSG_LAYER  BT_MSG_L2
#include "btmsg.h"

/* How often L2 polls to see if a stream is writable */
#define BT_L2_FLOW_POLL_MS 20

#define WATERMARK_HI           9000
#define WATERMARK_LO           1000
#define WATERMARK_MAX         10000

//==========================================================================
//   Type definitions
//==========================================================================

//
// One of these per IBTExtL2 instance:
//
typedef struct OEMBTExtL2obj_struct
{
  // Variables to keep track of BT core info
  bt_app_id_type     appId;
  AEEBTBDAddr        bdAddr;
  AEEBTL2ConfigInfo  sConfigInfo;
  boolean            bDisconnImmediate;

  ACONTEXT*          m_pac;
  IBTExtNotifier*    m_pNotifier;
  AEECallback        m_cbRead;
  AEECallback        m_cbWrite;

  bt_cmd_status_type CmdDoneStatus;

  // Rcv Data associated with this connection
  dsm_watermark_type rxWaterMark;
  q_type             rxQueue;

  // Keep track of all of the L2 instances:
  struct OEMBTExtL2obj_struct* pNextL2;
} OEMBTExtL2obj_t;


//
// Only one of these:
//
typedef struct
{
  OEMBTExtL2obj_t* pNextL2;    // singly linked list of L2s
} OEMBTExtL2_Mgr;

//==========================================================================
//   Function prototypes
//==========================================================================
static int16 OEMBTExtL2_ConvertCmdStatus( bt_cmd_status_type cmd_status );
static void  OEMBTExtL2_EventCallback( struct bt_ev_msg_struct* bt_ev_msg_ptr );

static OEMBTExtL2obj_t* OEMBTExtL2_FindMe_byAID(bt_app_id_type appId);
static OEMBTExtL2obj_t* OEMBTExtL2_FindMe_byPSM(bt_app_id_type appId);

static void OEMBTExtL2_RxLoWater( dsm_watermark_type *pWM, OEMBTExtL2obj_t *pMe );
static void OEMBTExtL2_RxHiWater( dsm_watermark_type *pWM, OEMBTExtL2obj_t *pMe );

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtL2_Mgr gMgr = {NULL};
static rex_crit_sect_type gsOEMBTExtL2_cs;


//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtL2_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtL2 *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtL2_Init(IBTExtL2* pParent)
{
  OEMBTExtL2obj_t* pMe;

  // allocate pMe
  pMe = sys_malloc(sizeof(OEMBTExtL2obj_t));
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  if ( ISHELL_CreateInstance( pParent->m_pIShell, AEECLSID_BLUETOOTH_NOTIFIER, 
                              (void**) &pMe->m_pNotifier ) != SUCCESS )
  {
    sys_free( pMe );
    return EFAILED;
  }

  // init BT layer
  pMe->appId = bt_cmd_ec_get_app_id_and_register(OEMBTExtL2_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    MSG_ERROR("OEMBTExtL2: Can't get app id",0,0,0);
    IBTEXTNOTIFIER_Release( pMe->m_pNotifier );
    sys_free( pMe );
    return EFAILED;
  }

  pMe->m_pac   = AEE_GetAppContext();

  if ( AEEHandle_To( &gOEMBTExtHandleList,
                     (OEMINSTANCE*)pMe,
                     &pParent->m_hBT ) == FALSE )
  {
    MSG_ERROR("OEMBTExtL2: Can't get handle",0,0,0);
    bt_cmd_ec_free_application_id( pMe->appId );
    sys_free(pMe);
    return EFAILED;
  }

  if( gMgr.pNextL2 == NULL )
  {
    rex_init_crit_sect( &gsOEMBTExtL2_cs );
  }

  // link into mgr
  pMe->pNextL2 = gMgr.pNextL2;
  gMgr.pNextL2 = pMe;

  pMe->bDisconnImmediate = FALSE;

  // Default values for a connection:
  pMe->sConfigInfo.psm               = BT_L2_PSM_INVALID;
  pMe->sConfigInfo.cid               = BT_L2_NULL_CID;
  pMe->sConfigInfo.in_flush_to       = BT_L2_DEFAULT_FLUSH_TIMEOUT;
  pMe->sConfigInfo.in_mtu            = BT_L2_DEFAULT_MTU;
  pMe->sConfigInfo.out_mtu           = BT_L2_DEFAULT_MTU;
  pMe->sConfigInfo.in_qos            = BT_QOS_BEST_EFFORT;
  pMe->sConfigInfo.token_rate        = BT_L2_DEFAULT_TOKEN_RATE;
  pMe->sConfigInfo.token_bucket_size = BT_L2_DEFAULT_TOKEN_BUCKET_SIZE;
  pMe->sConfigInfo.peak_bandwidth    = BT_L2_DEFAULT_PEAK_BANDWIDTH;
  pMe->sConfigInfo.latency           = BT_L2_DEFAULT_LATENCY;
  pMe->sConfigInfo.delay_variation   = BT_L2_DEFAULT_DELAY_VARIATION;

  // Set values for RX Watermark
  dsm_queue_init( &pMe->rxWaterMark, WATERMARK_MAX, &pMe->rxQueue );
  pMe->rxWaterMark.lo_watermark         = WATERMARK_LO;
  pMe->rxWaterMark.hi_watermark         = WATERMARK_HI;
  pMe->rxWaterMark.lowater_func_ptr     = (wm_cb_type)OEMBTExtL2_RxLoWater;
  pMe->rxWaterMark.lowater_func_data    = (void*)pMe;
  pMe->rxWaterMark.hiwater_func_ptr     = (wm_cb_type)OEMBTExtL2_RxHiWater;
  pMe->rxWaterMark.hiwater_func_data    = (void*)pMe;
  
  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtL2_Release( IBTExtL2* pParent )
{
  OEMBTExtL2obj_t*  pMe;
  OEMBTExtL2obj_t** ppc;
  bt_app_id_type    aid;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  aid = pMe->appId;
  pMe->appId = BT_APP_ID_NULL;

  if( OEMBTExtL2_FindMe_byAID( aid ) == NULL )
  {
    /* This is the last L2 object using this App ID */
    (void)bt_cmd_rm_set_connectable( aid, FALSE, BT_RM_AVP_AUTOMATIC );
    bt_cmd_ec_free_application_id( aid );
  }

  // unlink from mgr
  rex_enter_crit_sect( &gsOEMBTExtL2_cs );
  for( ppc = &gMgr.pNextL2; *ppc != NULL; ppc = &(*ppc)->pNextL2 )
  {
    if( *ppc == pMe )
    {
      *ppc = pMe->pNextL2;
      pMe->pNextL2 = NULL;
      break;
    }
  }
  rex_leave_crit_sect( &gsOEMBTExtL2_cs );

  // Cancel callbacks
  CALLBACK_Cancel( &pMe->m_cbRead );
  pMe->m_cbRead.pfnNotify = NULL;
  pMe->m_cbRead.pNotifyData = NULL;

  CALLBACK_Cancel( &pMe->m_cbWrite );
  pMe->m_cbWrite.pfnNotify = NULL;
  pMe->m_cbWrite.pNotifyData = NULL;

  // Get rid of the notifier
  IBTEXTNOTIFIER_Release( pMe->m_pNotifier );
  pMe->m_pNotifier = NULL;
  pMe->m_pac = NULL;

  sys_free(pMe);
  (void) AEEHandle_Clear( &gOEMBTExtHandleList, pParent->m_hBT );

  return SUCCESS;
}

int32 OEMBTExtL2_Read( IBTExtL2* pParent, void* buffer, uint32 bytes )
{
  OEMBTExtL2obj_t *pMe;
  int32 rv = 0;
  int32 sz_req;
  dsm_item_type *dsm_ptr;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    rv = AEE_L2_ERROR;
  }
  else
  {
    dsm_ptr = (dsm_item_type*)q_check( pMe->rxWaterMark.q_ptr );
    if( dsm_ptr != NULL )
    {
      sz_req = dsm_length_packet(dsm_ptr);
      if( 0 == bytes )
      {
         rv = 0;
      }
      else if( sz_req > bytes )
      {
        rv = AEE_L2_BUF_TOO_SMALL;
      }
      else
      {
        dsm_ptr = dsm_dequeue( &pMe->rxWaterMark );
        rv = dsm_pullup( &dsm_ptr, buffer, bytes );
        if( dsm_ptr != NULL )
        {
          if( sz_req != 0 )
          {
            MSG_ERROR("OEM L2: Bytes left over after rx. Dropping",0,0,0);
          }
          dsm_free_packet( &dsm_ptr );
        }
      }
    }
    else
    {
      rv = AEE_L2_WOULDBLOCK;
    }
  }
  return rv;
}

void OEMBTExtL2_Readable( IBTExtL2* pParent, PFNNOTIFY pfn, void* pUser )
{
  OEMBTExtL2obj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return;
  }

  rex_enter_crit_sect( &gsOEMBTExtL2_cs );
  if( CALLBACK_IsQueued( &pMe->m_cbRead ) )
  {
    CALLBACK_Cancel( &pMe->m_cbRead );
  }

  CALLBACK_Init(&pMe->m_cbRead, pfn, pUser);
  
  if( q_cnt( pMe->rxWaterMark.q_ptr ) > 0 )
  {
    // There's already data to be read. Just schedule the callback
    AEE_APP_RESUME( &pMe->m_cbRead, pMe->m_pac );
  }
  rex_leave_crit_sect( &gsOEMBTExtL2_cs );
}

void OEMBTExtL2_Cancel( IBTExtL2* pParent,  PFNNOTIFY pfn, void* pUser )
{
  OEMBTExtL2obj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return;
  }

  rex_enter_crit_sect( &gsOEMBTExtL2_cs );
  if ( (pMe->m_cbRead.pfnNotify == pfn) && 
       (pMe->m_cbRead.pNotifyData == pUser) )
  {
    CALLBACK_Cancel(&pMe->m_cbRead);
    pMe->m_cbRead.pfnNotify = NULL;
    pMe->m_cbRead.pNotifyData = NULL;
  }
  
  if ( (pMe->m_cbWrite.pfnNotify == pfn) && 
       (pMe->m_cbWrite.pNotifyData == pUser) )
  {
    CALLBACK_Cancel(&pMe->m_cbWrite);
    pMe->m_cbWrite.pfnNotify = NULL;
    pMe->m_cbWrite.pNotifyData = NULL;
  }
  rex_leave_crit_sect( &gsOEMBTExtL2_cs );
}

int OEMBTExtL2_Register( IBTExtL2* pParent, uint32 PSM )
{
  OEMBTExtL2obj_t *pMe;
  int rv;
  bt_cmd_status_type status;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  DBGPRINTF( "BT OEM L2 CMD RX: Register. PSM %x",
             PSM, 0, 0 );

  pMe->sConfigInfo.psm = PSM;
  pMe->CmdDoneStatus = BT_CS_GN_PENDING;
  status = bt_cmd_l2_register( pMe->appId, PSM );
  while( status == BT_CS_GN_PENDING && pMe->CmdDoneStatus == BT_CS_GN_PENDING )
  {
    // Only wait if the command was successfully enqueued
    // This loop will exit when the BT task has completed the command & replied,
    // which will change pMe->CmdDoneStatus.
    rex_sleep(1);
  }


  rv = OEMBTExtL2_ConvertCmdStatus( pMe->CmdDoneStatus );

  if( rv == AEEBT_L2_ERR_NONE )
  {
    status = bt_cmd_rm_set_connectable( pMe->appId, TRUE, BT_RM_AVP_AUTOMATIC );
    rv = OEMBTExtL2_ConvertCmdStatus( status );
  }

  return rv;
}

int OEMBTExtL2_Deregister( IBTExtL2* pParent )
{
  OEMBTExtL2obj_t *pMe;
  int rv;
  bt_cmd_status_type status;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  DBGPRINTF( "BT OEM L2 CMD RX: Deregister. PSM %x",
             pMe->sConfigInfo.psm, 0, 0 );
  status = bt_cmd_l2_unregister( pMe->appId, pMe->sConfigInfo.psm );
  rv = OEMBTExtL2_ConvertCmdStatus( pMe->CmdDoneStatus );

  status = bt_cmd_rm_set_connectable( pMe->appId, FALSE, BT_RM_AVP_AUTOMATIC );
  if( rv == AEEBT_L2_ERR_NONE )
  {
    rv = OEMBTExtL2_ConvertCmdStatus( status );
  }

  return rv;
}

int OEMBTExtL2_SetParams( IBTExtL2* pParent, uint16 proto_id, const AEEBTL2ConfigInfo* info )
{
  OEMBTExtL2obj_t *pMe;
  OEMBTExtL2obj_t *pSamePSM;
  int rv;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  
  if( (pSamePSM = OEMBTExtL2_FindMe_byPSM( proto_id )) != NULL
      && pSamePSM->appId != pMe->appId)
  {
  /* There's another L2 instance using this PSM. Lets use that 
  * appID */
    (void)bt_cmd_ec_free_application_id( pMe->appId );
    pMe->appId = pSamePSM->appId;
  }
  rex_enter_crit_sect( &gsOEMBTExtL2_cs );
  pMe->sConfigInfo.in_flush_to       = info->in_flush_to;
  pMe->sConfigInfo.in_mtu            = info->in_mtu;
  pMe->sConfigInfo.in_qos            = info->in_qos;
  pMe->sConfigInfo.token_rate        = info->token_rate;
  pMe->sConfigInfo.token_bucket_size = info->token_bucket_size;
  pMe->sConfigInfo.peak_bandwidth    = info->peak_bandwidth;
  pMe->sConfigInfo.latency           = info->latency;
  pMe->sConfigInfo.delay_variation   = info->delay_variation;

  pMe->CmdDoneStatus = bt_cmd_l2_set_params( pMe->appId,
                                 pMe->sConfigInfo.cid,
                                 proto_id,
                                 info->in_flush_to,
                                 info->in_mtu,
                                 (bt_qos_service_type)info->in_qos,
                                 info->token_rate,
                                 info->token_bucket_size,
                                 info->peak_bandwidth,
                                 info->latency,
                                 info->delay_variation );
  rex_leave_crit_sect( &gsOEMBTExtL2_cs );
  
  rv = OEMBTExtL2_ConvertCmdStatus( pMe->CmdDoneStatus );
  return rv;
}

int OEMBTExtL2_Connect( IBTExtL2* pParent, uint16 proto_id, const AEEBTBDAddr* pBDAddr )
{
  OEMBTExtL2obj_t *pMe;
  OEMBTExtL2obj_t *pSamePSM;
  int rv;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if( pMe->sConfigInfo.cid != BT_L2_NULL_CID )
  {
    rv = AEEBT_L2_ALREADY_CONNECTED;
  }
  else
  {
    if( (pSamePSM = OEMBTExtL2_FindMe_byPSM( proto_id )) != NULL 
        && pSamePSM->appId != pMe->appId)
    {
      /* There's another L2 instance using this PSM. Lets use that 
       * appID */
      (void)bt_cmd_ec_free_application_id( pMe->appId );
      pMe->appId = pSamePSM->appId;
    }
    DBGPRINTF( "BT OEM L2 CMD RX: Connect. PSM %x, BDA %x %x",
               proto_id, pBDAddr->uAddr[4], pBDAddr->uAddr[5] );
    pMe->CmdDoneStatus = bt_cmd_l2_connect_immediate_cid( pMe->appId,
                                              proto_id,
                                              (bt_bd_addr_type*)pBDAddr,
                                              &pMe->sConfigInfo.cid );
    DBGPRINTF( "BT OEM L2 CMD RX: Connect. LCID %x",
               pMe->sConfigInfo.cid, 0, 0 );
    rv = OEMBTExtL2_ConvertCmdStatus( pMe->CmdDoneStatus );
    if( rv == AEEBT_L2_ERR_NONE )
    {
      rex_enter_crit_sect( &gsOEMBTExtL2_cs );
      pMe->sConfigInfo.psm = proto_id;
      pMe->bdAddr = *pBDAddr;
      rex_leave_crit_sect( &gsOEMBTExtL2_cs );
    }
  }
  return rv;
}

int OEMBTExtL2_Disconnect( IBTExtL2* pParent )
{
  OEMBTExtL2obj_t *pMe;
  int rv;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if( pMe->sConfigInfo.cid == BT_L2_NULL_CID )
  {
    rv = AEEBT_L2_NOT_CONNECTED;
  }
  else
  {
    DBGPRINTF( "BT OEM L2 CMD RX: Disconnect. PSM %x, CID %x, BDA %x",
               pMe->sConfigInfo.psm, pMe->sConfigInfo.cid, pMe->bdAddr.uAddr[5] );
    pMe->CmdDoneStatus = bt_cmd_l2_disconnect( pMe->appId,
                                   pMe->sConfigInfo.cid );
    rv = OEMBTExtL2_ConvertCmdStatus( pMe->CmdDoneStatus );
  }
  return rv;
}

int OEMBTExtL2_DisconnImmediate( IBTExtL2* pParent )
{
  OEMBTExtL2obj_t *pMe;
  int rv;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if( pMe->sConfigInfo.cid == BT_L2_NULL_CID )
  {
    rv = AEEBT_L2_NOT_CONNECTED;
  }
  else
  {
    DBGPRINTF( "BT OEM L2 CMD RX: Disconn Immediate. PSM %x, CID %x, BDA %x",
               pMe->sConfigInfo.psm, pMe->sConfigInfo.cid, pMe->bdAddr.uAddr[5] );
    pMe->CmdDoneStatus = bt_cmd_l2_disconn_immediate( pMe->appId,
                                          pMe->sConfigInfo.cid );
    rv = OEMBTExtL2_ConvertCmdStatus( pMe->CmdDoneStatus );
  }
  return rv;
}

int32 OEMBTExtL2_Write( IBTExtL2* pParent, const byte* buf, uint32 length )
{
  OEMBTExtL2obj_t *pMe;
  dsm_item_type* dsm_ptr;
  int rv = 0;
  bt_cmd_status_type status;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return AEE_L2_ERROR;
  }
  if( pMe->sConfigInfo.cid == BT_L2_NULL_CID )
  {
    rv = AEE_L2_ERROR;
  }
  else if( bt_cmd_l2_get_acl_tx_flow_status( pMe->sConfigInfo.cid ) != TRUE )
  {
      rv = AEE_L2_WOULDBLOCK;
  }
  else
  {
    dsm_ptr = bt_get_free_dsm_ptr( BT_TL_L2CAP, length );
    if( !dsm_ptr )
    {
      status = BT_CS_GN_CMD_Q_FULL;
    }
    else
    {
      rv = dsm_pushdown_tail( &dsm_ptr,
                              buf,
                              length,
                              DSM_DS_POOL_SIZE( length ) );
      if( rv != length )
      {
        status = BT_CS_GN_CMD_Q_FULL;
        DBGPRINTF( "BT OEM L2: Write error filling DSM", 0,0,0 );    
        dsm_free_buffer( dsm_ptr );
      }
      else
      {
        status = bt_cmd_l2_write( pMe->appId, pMe->sConfigInfo.cid, dsm_ptr, NULL );
      }
    }
    if( status == BT_CS_GN_CMD_Q_FULL )
    {
      rv = AEE_L2_WOULDBLOCK;
    }
    else if( OEMBTExtL2_ConvertCmdStatus( pMe->CmdDoneStatus ) != AEEBT_L2_ERR_NONE )
    {
      rv = AEE_L2_ERROR;
    }
  }
  return rv;
}

int32 OEMBTExtL2_WriteV( IBTExtL2* pParent, const SockIOBlock iov[], uint16 nIovCount )
{
  OEMBTExtL2obj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  // TODO: Implement
  return EUNSUPPORTED;
}

int32 OEMBTExtL2_ReadV( IBTExtL2* pParent, SockIOBlock iov[], uint16 nIovCount )
{
  OEMBTExtL2obj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  // TODO: Implement
  return EUNSUPPORTED;
}

void OEMBTExtL2_FlowCB( IBTExtL2* pParent )
{
  OEMBTExtL2obj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return;
  }

  if( bt_cmd_l2_get_acl_tx_flow_status( pMe->sConfigInfo.cid ) == TRUE )
  {
    AEE_APP_RESUME( &pMe->m_cbWrite, pMe->m_pac );
  }  
  else
  {
    // Currently, we don't have real flow control for L2
    // Just poll to see when bb flow is back on again.
    AEE_SetSysTimer( BT_L2_FLOW_POLL_MS, (PFNNOTIFY)OEMBTExtL2_FlowCB, (void*)pParent );
  }
}

void OEMBTExtL2_Writable( IBTExtL2* pParent, PFNNOTIFY pfn, void* pUser )
{
  OEMBTExtL2obj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return;
  }

  rex_enter_crit_sect( &gsOEMBTExtL2_cs );
  if( CALLBACK_IsQueued( &pMe->m_cbWrite ) )
  {
    CALLBACK_Cancel( &pMe->m_cbWrite );
  }

  CALLBACK_Init(&pMe->m_cbWrite, pfn, pUser);
  rex_leave_crit_sect( &gsOEMBTExtL2_cs );
  
  OEMBTExtL2_FlowCB( pParent );
}

uint16 OEMBTExtL2_NumConn( IBTExtL2* pParent, const AEEBTBDAddr *pBdAddr )
{
  OEMBTExtL2obj_t *pMe;
  uint16 num_conn;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return 0;
  }
  
  bt_cmd_l2_get_num_conn( pMe->appId, (bt_bd_addr_type*)pBdAddr, &num_conn );

  return num_conn;
}

//==========================================================================
//   static helper functions
//==========================================================================
static OEMBTExtL2obj_t* OEMBTExtL2_FindMe_byAID(bt_app_id_type appId)
{
  OEMBTExtL2obj_t* pMe;

  rex_enter_crit_sect( &gsOEMBTExtL2_cs );
  for (pMe = gMgr.pNextL2; pMe != NULL; pMe = pMe->pNextL2)
  {
    if (pMe->appId == appId)
    {
      break;
    }
  }
  rex_leave_crit_sect( &gsOEMBTExtL2_cs );
  return pMe;
}

static OEMBTExtL2obj_t* OEMBTExtL2_FindMe_byPSM(uint16 PSM)
{
  OEMBTExtL2obj_t* pMe;

  rex_enter_crit_sect( &gsOEMBTExtL2_cs );
  for (pMe = gMgr.pNextL2; pMe != NULL; pMe = pMe->pNextL2)
  {
    if (pMe->sConfigInfo.psm == PSM)
    {
      break;
    }
  }
  rex_leave_crit_sect( &gsOEMBTExtL2_cs );
  return pMe;
}

static OEMBTExtL2obj_t* OEMBTExtL2_FindMe_byCID(uint16 wCid)
{
  OEMBTExtL2obj_t* pMe;

  rex_enter_crit_sect( &gsOEMBTExtL2_cs );
  for (pMe = gMgr.pNextL2; pMe != NULL; pMe = pMe->pNextL2)
  {
    if (pMe->sConfigInfo.cid == wCid)
    {
      break;
    }
  }
  rex_leave_crit_sect( &gsOEMBTExtL2_cs );
  return pMe;
}

static int16 OEMBTExtL2_ConvertCmdStatus( bt_cmd_status_type cmd_status )
{
  switch ( cmd_status )
  {  
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return AEEBT_L2_ERR_NONE;

    case BT_CS_L2_SETPRM_REG_FULL:
      return AEEBT_L2_SETPRM_REG_FULL;

    case BT_CS_L2_REG_FULL:
      return AEEBT_L2_REG_FULL;

    case BT_CS_L2_REG_DUPLICATE:
      return AEEBT_L2_REG_DUPLICATE;

    case BT_CS_L2_CONN_NOT_REGISTERED:
      return AEEBT_L2_CONN_NOT_REGISTERED;

    case BT_CS_L2_CONN_ALL_USED:
      return AEEBT_L2_CONN_ALL_USED;

    case BT_CS_L2_WRITE_CID_NOT_FOUND:
      return AEEBT_L2_WRITE_CID_NOT_FOUND;

    case BT_CS_L2_WRITE_BIG_PKT_SIZE:
      return AEEBT_L2_WRITE_BIG_PKT_SIZE;

    case BT_CS_L2_DISC_CID_NOT_FOUND:
      return AEEBT_L2_DISC_CID_NOT_FOUND;

    case BT_CS_L2_DISC_ERROR:
      return AEEBT_L2_DISC_ERROR;

    case BT_CS_L2_UNREG_NOT_FOUND:
      return AEEBT_L2_UNREG_NOT_FOUND;

    case BT_CS_L2_INVALID_PSM:
      return AEEBT_L2_INVALID_PSM;

    case BT_CS_L2_CONN_NOT_UP:
      return AEEBT_L2_CONN_NOT_UP;

    case BT_CS_GN_BAD_CMD_STATE:
      return AEEBT_L2_UNSPECIFIED_ERROR;
      //return AEEBT_L2_BAD_CMD_STATE;

    case BT_CS_GN_CMD_Q_FULL:
      return AEE_L2_WOULDBLOCK;

    case BT_CS_GN_BAD_APP_ID:
      return AEEBT_L2_UNSPECIFIED_ERROR;
      //return AEEBT_L2_BAD_APP_ID;

    case BT_CS_GN_UNRECOGNIZED_CMD:
      return AEEBT_L2_UNSPECIFIED_ERROR;
      //return AEEBT_L2_UNRECOGNIZED_CMD;

    case BT_CS_GN_MAX_CMD_RETRIES:
      return AEEBT_L2_UNSPECIFIED_ERROR;
      //return AEEBT_L2_MAX_CMD_RETRIES;

    case BT_CS_GN_RETRY_CMD_LATER:
      return AEEBT_L2_UNSPECIFIED_ERROR;
      // return AEEBT_L2_RETRY_CMD_LATER;


    default:
      return AEEBT_L2_UNSPECIFIED_ERROR;
  }
}

static uint16 OEMBTExtL2_ConvertEvReason( bt_event_reason_type ev_reason )
{
  switch ( ev_reason )
  {
    case BT_EVR_GN_SUCCESS:
      return AEEBT_L2_ERR_NONE;
    case BT_EVR_L2_CONFIG_PARAMS_NOT_AGREEABLE:
      return AEEBT_L2_CONFIG_PARAMS_NOT_AGREEABLE;
    case BT_EVR_L2_LOCAL_REJECT_CONNECTION:
      return AEEBT_L2_LOCAL_REJECT_CONNECTION;
    case BT_EVR_L2_REMOTE_REJECT_CONNECTION:
      return AEEBT_L2_REMOTE_REJECT_CONNECTION;
    case BT_EVR_L2_PAGE_FAILED:
      return AEEBT_L2_PAGE_FAILED;
    case BT_EVR_L2_LINK_LOST:
      return AEEBT_L2_LINK_LOST;
    case BT_EVR_L2_AUTHENTICATION_FAILED:
      return AEEBT_L2_AUTHENTICATION_FAILED;
    case BT_EVR_L2_UNSPECIFIED_ERROR:
      return AEEBT_L2_UNSPECIFIED_ERROR;
    case BT_EVR_L2_NORMAL_DISCONNECT:
      return AEEBT_L2_ERR_NONE;
    case BT_EVR_L2_PING_SUCCESS:
      return AEEBT_L2_ERR_NONE;
    case BT_EVR_L2_PING_FAILED:
      return AEEBT_L2_PING_FAILED;
    case BT_EVR_L2_CONFIG_SUCCESS:
      return AEEBT_L2_ERR_NONE;

    default:
      return AEEBT_L2_UNSPECIFIED_ERROR;
  }
}

static AEEBTNotification* OEMBTExtL2_GetFreeNotification( IBTExtNotifier* pNot )
{
  AEEBTNotification* pN;
  pN = IBTEXTNOTIFIER_GetFreeNotification( pNot );
  if( pN == NULL )
  {
    MSG_ERROR( "L2EvCb - Can't get free not.",
               0, 0, 0 );
    return NULL;
  }

  pN->uID = 0xFFFFFFFF;
  pN->data.uError  = AEEBT_L2_ERR_NONE;
  return pN;
}

// called from BT task context; so just queue the event
static void OEMBTExtL2_EventCallback(bt_ev_msg_type* bt_ev_msg_ptr)
{
  OEMBTExtL2obj_t*   pMe  = NULL;
  AEEBTNotification* pN   = NULL;
  dsm_item_type*     pDSM = NULL;

  switch (bt_ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
    {
      OEMBTExtL2obj_t* pL2Obj;

      DBGPRINTF( "BT PF L2 RX EV: Cmd Done, Status: %x, App: %x, Cmd: %x", 
                 bt_ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                 bt_ev_msg_ptr->ev_msg.ev_gn_cmd_done.bt_app_id,
                 bt_ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type );
      rex_enter_crit_sect( &gsOEMBTExtL2_cs );
      for (pL2Obj = gMgr.pNextL2; pL2Obj != NULL; pL2Obj = pL2Obj->pNextL2)
      {
        if (pL2Obj->appId == bt_ev_msg_ptr->ev_msg.ev_gn_cmd_done.bt_app_id)
        {
          pL2Obj->CmdDoneStatus = bt_ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status;
        }
      }
      rex_leave_crit_sect( &gsOEMBTExtL2_cs );
    } // end of case BT_EV_GN_CMD_DONE
    break;

    case BT_EV_L2_CONNECTED:
      pMe = OEMBTExtL2_FindMe_byCID(bt_ev_msg_ptr->ev_msg.ev_l2_conn.cid);
      if( pMe == NULL )
      {
        /* This is not a response to a connection request,
         * but an incomming connection */
        pMe = OEMBTExtL2_FindMe_byAID(bt_ev_msg_ptr->ev_hdr.bt_app_id);
      }
      if( pMe != NULL )
      {
        pN = OEMBTExtL2_GetFreeNotification( pMe->m_pNotifier );
        if( pN != NULL )
        {
          pN->uID = AEEBT_L2_EVT_CONNECTED;

          rex_enter_crit_sect( &gsOEMBTExtL2_cs );
          pMe->sConfigInfo.cid = bt_ev_msg_ptr->ev_msg.ev_l2_conn.cid;
          pMe->sConfigInfo.out_mtu   = bt_ev_msg_ptr->ev_msg.ev_l2_conn.mtu;
          dsm_empty_queue( &pMe->rxWaterMark );

          pN->data.sL2Connected.bdAddr = *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_l2_conn.bd_addr);
          // Set bdAddr in the object, so that it can be returned in subsequent
          // calls to the GetStatus function.
          pMe->bdAddr = pN->data.sL2Connected.bdAddr;
          pN->data.sL2Connected.info = pMe->sConfigInfo;
          rex_leave_crit_sect( &gsOEMBTExtL2_cs );
        }
      }
      else
      {
        BT_ERR( "BT OEM L2: Can't find entry for connected event! CID %x AID %x",
                bt_ev_msg_ptr->ev_msg.ev_l2_conn.cid,
                bt_ev_msg_ptr->ev_hdr.bt_app_id, 0 );
      }
      break;

    case BT_EV_L2_CONNECTION_FAILED:
      pMe = OEMBTExtL2_FindMe_byCID(bt_ev_msg_ptr->ev_msg.ev_l2_conn_failed.cid);

      if( pMe != NULL )
      {
        pN = OEMBTExtL2_GetFreeNotification( pMe->m_pNotifier );
        if( pN != NULL )
        {
          pN->uID = AEEBT_L2_EVT_CONNECT_FAILED;
          pMe->sConfigInfo.cid = BT_L2_NULL_CID;
          pN->data.sL2ConnectFailed.uError = 
            OEMBTExtL2_ConvertEvReason( bt_ev_msg_ptr->ev_msg.ev_l2_conn_failed.reason );
          pN->data.sL2ConnectFailed.psm = pMe->sConfigInfo.psm;
          pN->data.sL2ConnectFailed.bdAddr = pMe->bdAddr;
        }
      }
      break;

    case BT_EV_L2_RX_DATA:
      pMe = OEMBTExtL2_FindMe_byCID(bt_ev_msg_ptr->ev_msg.ev_l2_rxd.cid);
      if( pMe != NULL )
      {
        pDSM = bt_ev_msg_ptr->ev_msg.ev_l2_rxd.dsm_ptr;
        if( pDSM == NULL )
        {
          /* This may happen if the L2CAP packet contained no data.
           * Create an empty DSM to put onto the watermark. */
          pDSM = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
        }
        if( pDSM != NULL )
        {
          dsm_enqueue( &pMe->rxWaterMark,
                       &pDSM);
          AEE_APP_RESUME( &pMe->m_cbRead, pMe->m_pac );
        }
      }
      break;

    case BT_EV_L2_RECONFIGURED:
      pMe = OEMBTExtL2_FindMe_byCID(bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.cid);
      if( pMe != NULL )
      {
        pN = OEMBTExtL2_GetFreeNotification( pMe->m_pNotifier );
        if( pN != NULL )
        {
          pN->uID = AEEBT_L2_EVT_RECONFIGURED;
          
          rex_enter_crit_sect( &gsOEMBTExtL2_cs );
          pMe->sConfigInfo.out_mtu = bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.out_mtu;
          pMe->sConfigInfo.in_flush_to = bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.in_flush_to;
          pMe->sConfigInfo.in_qos = (uint8)bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.in_qos.service_type;
          pMe->sConfigInfo.token_rate = (uint8)bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.in_qos.token_rate;
          pMe->sConfigInfo.token_bucket_size = (uint8)bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.in_qos.token_bucket_size;
          pMe->sConfigInfo.peak_bandwidth = (uint8)bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.in_qos.peak_bandwidth;
          pMe->sConfigInfo.latency = (uint8)bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.in_qos.latency;
          pMe->sConfigInfo.delay_variation = (uint8)bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.in_qos.delay_variation;
          
          pN->data.sL2Connected.info   = pMe->sConfigInfo;
          rex_leave_crit_sect( &gsOEMBTExtL2_cs );
          
          pN->data.sL2Reconfigured.uError = 
            OEMBTExtL2_ConvertEvReason( bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.reason );
        }
      }
      break;

    case BT_EV_L2_DISCONNECTED:
      pMe = OEMBTExtL2_FindMe_byCID(bt_ev_msg_ptr->ev_msg.ev_l2_reconfig.cid);
      if( pMe != NULL )
      {
        pN = OEMBTExtL2_GetFreeNotification( pMe->m_pNotifier );
        if( pN != NULL )
        {
          if( pMe->bDisconnImmediate == FALSE )
          {
            pN->uID = AEEBT_L2_EVT_DISCONNECTED;
          }
          else
          {
            pN->uID = AEEBT_L2_EVT_DISCONN_IMMEDIATE;
          }
          pMe->sConfigInfo.cid = BT_L2_NULL_CID;
          pN->data.sL2Disconnected.uError = 
            OEMBTExtL2_ConvertEvReason( bt_ev_msg_ptr->ev_msg.ev_l2_disc.reason );
          pN->data.sL2Disconnected.cid = bt_ev_msg_ptr->ev_msg.ev_l2_disc.cid;
        }
      }
      break;

    case BT_EV_L2_PING:
    {
      /* Note: This assumes that notifications are broadcast to everyone. This
       * will send the L2 Ping to all of the L2 instances. */
      pMe = gMgr.pNextL2;
      if( pMe != NULL )
      {
        byte* data = (byte*)bt_ev_msg_ptr->ev_msg.ev_l2_ping.data;
        pN = OEMBTExtL2_GetFreeNotification( pMe->m_pNotifier );
        if( pN != NULL )
        {
          pN->uID = AEEBT_L2_EVT_PING_RSP;
          
          pN->data.sL2PingRsp.data = sys_malloc( bt_ev_msg_ptr->ev_msg.ev_l2_ping.length );
          pN->data.sL2PingRsp.bdAddr = *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_l2_ping.bd_addr);
          pN->data.sL2PingRsp.size = bt_ev_msg_ptr->ev_msg.ev_l2_ping.length;
          MEMCPY(pN->data.sL2PingRsp.data, 
                 data,
                 pN->data.sL2PingRsp.size );
          pN->data.sL2PingRsp.uError = bt_ev_msg_ptr->ev_msg.ev_l2_ping.reason;
        }
      }
    }
    break;

    default:
    {
      BT_ERR( "BT OEM L2: unexpect event %x", 
              bt_ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      return;
    }
  } // end of switch (ev_type)
  
  if( pN != NULL )
  {
    pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_L2 );
    IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
  }
}


// IBTEXTSPP_GetOEMHandle (see AEEBTExtSPP.h)
int OEMBTExtL2_GetOEMHandle(IBTExtL2 *pParent, int32 *pnHandle)
{
  OEMBTExtL2obj_t *pDummy;

  if (!pnHandle) return EFAILED;

  // Check if pParent has a valid handle
  if (AEEHandle_From(&gOEMBTExtHandleList,pParent->m_hBT,(OEMINSTANCE*)&pDummy))
  {
    *pnHandle = pParent->m_hBT;
    return SUCCESS;
  }
  return EFAILED;
}

// IBTEXTSPP_Clone (see AEEBTExtSPP.h)
int OEMBTExtL2_Clone(IBTExtL2 *pParent, int32 nHandle)
{

  OEMBTExtL2obj_t *pMe;

  // Get the pMe referred to by nHandle
  if( AEEHandle_From( &gOEMBTExtHandleList, nHandle,
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  // Discard the object originally created for this pParent
  OEMBTExtL2_Release(pParent);

  // Set this pParent's object to be the object referenced
  // by nHandle.  This will succeed, because we just freed a handle.
  // So ignore the return value.
  AEEHandle_To(&gOEMBTExtHandleList, (OEMINSTANCE*)pMe, &pParent->m_hBT);

  // Since object is now owned by pParent, set nHandle to be invalid.
  AEEHandle_Clear(&gOEMBTExtHandleList, nHandle);

  // Update app context
  pMe->m_pac          = AEE_GetAppContext();

  // Free and recreate the notifier so it's in the right app context.
  IBTEXTNOTIFIER_Release( pMe->m_pNotifier );
  if (SUCCESS != ISHELL_CreateInstance( pParent->m_pIShell,
      AEECLSID_BLUETOOTH_NOTIFIER, (void**) &pMe->m_pNotifier ))
  {
    // This error shouldn't really happen, and, not really sure what
    // to do if it does.
    return EFAILED;
  }

  return SUCCESS;
}

int OEMBTExtL2_GetStatus(IBTExtL2 *pParent, AEEBTL2Connected *pStatus)
{
  OEMBTExtL2obj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, (OEMINSTANCE*)&pMe )
 != TRUE )
  {
    return EFAILED;
  }

  pStatus->bdAddr = pMe->bdAddr;
  pStatus->info = pMe->sConfigInfo;
  return SUCCESS;
}
/*======================================================================= 
=======================================================================*/
/////////////////////////////////////////////////////////
// NOTE: This DSM callback is called with tasks locked //
/////////////////////////////////////////////////////////
static void OEMBTExtL2_RxLoWater( dsm_watermark_type *pWM, OEMBTExtL2obj_t *pMe )
{
  MSG_LOW("OEM L2: Low WM Cid %x", pMe->sConfigInfo.cid,0,0);
}

/*======================================================================= 
=======================================================================*/
/////////////////////////////////////////////////////////
// NOTE: This DSM callback is called with tasks locked //
/////////////////////////////////////////////////////////
static void OEMBTExtL2_RxHiWater( dsm_watermark_type *pWM, OEMBTExtL2obj_t *pMe )
{
  MSG_HIGH("OEM L2: High WM Cid %x", pMe->sConfigInfo.cid,0,0);  
}


#endif /* FEATURE_IBT */


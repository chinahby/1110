/*===========================================================================

FILE:      OEMBTExtSPP.c

SERVICES:  BlueTooth Serial Port Protocol QCT extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth Serial Port Protocol

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2003-2009 by QUALCOMM Incorporated. 
         All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtSPP.c#2 $
$DateTime: 2009/04/14 15:01:41 $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
2009-04-14  pn  Watermark queues get initialized by dsm_queue_init().
09/15/08   sp  Compiler warning fix

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT)
#include "bt.h"
#include "bti.h"
#include "btsio.h"
#include "btsd.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtSPP.h"

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif

#include "OEMHeap.h"

//==========================================================================
//   Macro definitions
//==========================================================================

#define WATERMARK_HI           3000
#define WATERMARK_LO           1000
#define WATERMARK_MAX         10000
#define WATERMARK_ENQUEUE_SIZE  200

#define MAX_SPP_STREAMS           5

#define OEMBTEXTSPP_NUM_EVENTS   50

//==========================================================================
//   Type definitions
//==========================================================================

typedef struct OEMBTExtSPP_Evt OEMBTExtSPP_Evt;

typedef struct OEMBTExtSPPobj_struct
{
  uint8               uIndex; // index into watermark callback function tables

  dsm_watermark_type  tx_wm;
  dsm_watermark_type  rx_wm;
  q_type              tx_q;
  q_type              rx_q;
  dsm_item_type*      dsm_item_ptr;

  // valid from Open to Close
  AEEBTBDAddr         BDAddr;
  sio_stream_id_type  streamID;

  AEEBTSppStatus      status;
  AEEBTSppConfig      config;
  AEEBTModemStatus    modemStatus;
  AEEBTLineError      lineError;

  boolean             bOKToSend;
  //boolean             bDataRcvd;

  IBTExtNotifier*     m_pNotifier;
  AEECallback*        m_pcbRead;    // for READ's
  AEECallback*        m_pcbWrite;   // for WRITE's - may contain multiple items
  ACONTEXT*           m_pac;

  struct OEMBTExtSPPobj_struct* pNextSPP;  // linked into mgr
} OEMBTExtSPPobj_t;

// one of these
typedef struct
{
  OEMBTExtSPPobj_t* pNextSPP;   // singly linked list of SPPs
  boolean           bIdxTaken[ MAX_SPP_STREAMS ];
} OEMBTExtSPP_Mgr;

typedef void (*OEMBTExtSPP_ev_hndlr_type) (bt_ev_msg_type* bt_ev_ptr);

//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static void OEMBTExtSPP_Cancel( AEECallback* pcb );
static OEMBTExtSPPobj_t* OEMBTExtSPP_FindMe( sio_stream_id_type streamID);
static uint16 OEMBTExtSPP_ConvertBTReason( bt_event_reason_type bt_reason );
static void OEMBTExtSPP_StatusChangeCB( bt_spp_status_type* pStatus );
static void OEMBTExtSPP_ConfigChangeCB( bt_spp_cfg_rpt_type* pCfg );
static void OEMBTExtSPP_ModemStatusCB( bt_spp_ms_type* pModemStatus );
static void OEMBTExtSpp_LineErrorCB( bt_spp_le_type* pLineError );

static void OEMBTExtSPP_DataReceived1( void );
static void OEMBTExtSPP_DataReceived2( void );
static void OEMBTExtSPP_DataReceived3( void );
static void OEMBTExtSPP_DataReceived4( void );
static void OEMBTExtSPP_DataReceived5( void );
static void OEMBTExtSPP_RxHiWater1( void );
static void OEMBTExtSPP_RxHiWater2( void );
static void OEMBTExtSPP_RxHiWater3( void );
static void OEMBTExtSPP_RxHiWater4( void );
static void OEMBTExtSPP_RxHiWater5( void );
static void OEMBTExtSPP_RxLoWater1( void );
static void OEMBTExtSPP_RxLoWater2( void );
static void OEMBTExtSPP_RxLoWater3( void );
static void OEMBTExtSPP_RxLoWater4( void );
static void OEMBTExtSPP_RxLoWater5( void );
static void OEMBTExtSPP_TxLoWater1( void );
static void OEMBTExtSPP_TxLoWater2( void );
static void OEMBTExtSPP_TxLoWater3( void );
static void OEMBTExtSPP_TxLoWater4( void );
static void OEMBTExtSPP_TxLoWater5( void );
static void OEMBTExtSPP_TxHiWater1( void );
static void OEMBTExtSPP_TxHiWater2( void );
static void OEMBTExtSPP_TxHiWater3( void );
static void OEMBTExtSPP_TxHiWater4( void );
static void OEMBTExtSPP_TxHiWater5( void );
static void OEMBTExtSPP_EnableDTR1( void );
static void OEMBTExtSPP_EnableDTR2( void );
static void OEMBTExtSPP_EnableDTR3( void );
static void OEMBTExtSPP_EnableDTR4( void );
static void OEMBTExtSPP_EnableDTR5( void );
static void OEMBTExtSPP_FlushTx1( void );
static void OEMBTExtSPP_FlushTx2( void );
static void OEMBTExtSPP_FlushTx3( void );
static void OEMBTExtSPP_FlushTx4( void );
static void OEMBTExtSPP_FlushTx5( void );

static void OEMBTExtSPP_store_bt_event( bt_ev_msg_type* bt_ev_ptr );
static uint16 OEMBTExtSPP_get_max_event_bytes( void );

static OEMBTExtSPPobj_t* OEMBTExtSPP_CheckParentPtr( IBTExtSPP* pParent );

// Event handler functions:
static void OEMBTExtSPP_ev_flush_tx( bt_ev_msg_type *bt_ev_ptr );
static void OEMBTExtSPP_ev_enable_dtr( bt_ev_msg_type *bt_ev_ptr );
static void OEMBTExtSPP_ev_tx_lo_wm( bt_ev_msg_type *bt_ev_ptr );
static void OEMBTExtSPP_ev_rx_hi_wm( bt_ev_msg_type *bt_ev_ptr );
static void OEMBTExtSPP_ev_rx_lo_wm( bt_ev_msg_type *bt_ev_ptr );
static void OEMBTExtSPP_ev_rx_gne( bt_ev_msg_type *bt_ev_ptr );
static void OEMBTExtSPP_ev_line_error( bt_ev_msg_type *bt_ev_ptr );
static void OEMBTExtSPP_ev_modem_status( bt_ev_msg_type *bt_ev_ptr );
static void OEMBTExtSPP_ev_config_change( bt_ev_msg_type *bt_ev_ptr );
static void OEMBTExtSPP_ev_status_change( bt_ev_msg_type *bt_ev_ptr );

// The main event processor:
static void OEMBTExtSPP_process_ev_queue( void );

// The order of the event handler table must match the order of events
// in bt.h:
static const OEMBTExtSPP_ev_hndlr_type OEMBTExtSPP_ev_hndlr_table[] = 
  { OEMBTExtSPP_ev_flush_tx,
    OEMBTExtSPP_ev_enable_dtr,
    NULL, /* tx_hi_wm */
    OEMBTExtSPP_ev_tx_lo_wm,
    NULL, /* tx_gne */
    NULL, /* tx_empty */
    OEMBTExtSPP_ev_rx_hi_wm,
    OEMBTExtSPP_ev_rx_lo_wm,
    OEMBTExtSPP_ev_rx_gne,
    NULL, /* rx_empty */
    OEMBTExtSPP_ev_line_error,
    OEMBTExtSPP_ev_modem_status,
    OEMBTExtSPP_ev_config_change,
    OEMBTExtSPP_ev_status_change
  };
    
    

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtSPP_Mgr     gMgr;               // manager object

static wm_cb_type non_empty_func_table[ ] =
{
  OEMBTExtSPP_DataReceived1,
  OEMBTExtSPP_DataReceived2,
  OEMBTExtSPP_DataReceived3,
  OEMBTExtSPP_DataReceived4,
  OEMBTExtSPP_DataReceived5
};

static wm_cb_type rx_hiwater_func_table[ ] =
{
  OEMBTExtSPP_RxHiWater1,
  OEMBTExtSPP_RxHiWater2,
  OEMBTExtSPP_RxHiWater3,
  OEMBTExtSPP_RxHiWater4,
  OEMBTExtSPP_RxHiWater5
};

static wm_cb_type rx_lowater_func_table[ ] =
{
  OEMBTExtSPP_RxLoWater1,
  OEMBTExtSPP_RxLoWater2,
  OEMBTExtSPP_RxLoWater3,
  OEMBTExtSPP_RxLoWater4,
  OEMBTExtSPP_RxLoWater5
};

static wm_cb_type tx_hiwater_func_table[ ] =
{
  OEMBTExtSPP_TxHiWater1,
  OEMBTExtSPP_TxHiWater2,
  OEMBTExtSPP_TxHiWater3,
  OEMBTExtSPP_TxHiWater4,
  OEMBTExtSPP_TxHiWater5
};

static wm_cb_type tx_lowater_func_table[ ] =
{
  OEMBTExtSPP_TxLoWater1,
  OEMBTExtSPP_TxLoWater2,
  OEMBTExtSPP_TxLoWater3,
  OEMBTExtSPP_TxLoWater4,
  OEMBTExtSPP_TxLoWater5
};

static sio_vv_func_ptr_type enable_dtr_func_table[ ] =
{
  OEMBTExtSPP_EnableDTR1,
  OEMBTExtSPP_EnableDTR2,
  OEMBTExtSPP_EnableDTR3,
  OEMBTExtSPP_EnableDTR4,
  OEMBTExtSPP_EnableDTR5
};

static sio_vv_func_ptr_type flush_tx_func_table[ ] =
{
  OEMBTExtSPP_FlushTx1,
  OEMBTExtSPP_FlushTx2,
  OEMBTExtSPP_FlushTx3,
  OEMBTExtSPP_FlushTx4,
  OEMBTExtSPP_FlushTx5
};

static bt_event_q_info_type OEMBTExtSPP_ev_q_info;
static q_type               OEMBTExtSPP_ev_q;

// Used not for protecting data, but for task synchronisation
static rex_crit_sect_type OEMBTExtSPP_crit_sect;

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtSPP_Init()

Description:
   This is called from AEE when an app tries to create an instance of a
   class ID associated with this module.

Parameters:
   IBTExtSPP *pParent: Pointer to the IBTExtSPP allocated by AEE

Return Value:  AEE_SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates app-associated memory for this object

===========================================================================*/
int OEMBTExtSPP_Init(IBTExtSPP *pParent)
{
  OEMBTExtSPPobj_t* pMe;
  uint8 i;
  
  for ( i=0; i<MAX_SPP_STREAMS; i++ )
  {
    if ( gMgr.bIdxTaken[ i ] == FALSE )
    {
      break;
    }
  }
  /* check the boundry condition */
  if( i == MAX_SPP_STREAMS )
  {
      return EFAILED;
  }
  // allocate pMe
  pMe = sys_malloc(sizeof(OEMBTExtSPPobj_t));
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  // Set the parent's BT handle:
  if ( AEEHandle_To( &gOEMBTExtHandleList,
                     (OEMINSTANCE*)pMe,
                     &pParent->m_hBT ) == FALSE )
  {
    sys_free(pMe);
    return EFAILED;
  }

  if ( ISHELL_CreateInstance( pParent->pIShell, AEECLSID_BLUETOOTH_NOTIFIER, 
                              (void**) &pMe->m_pNotifier ) != SUCCESS )
  {
    sys_free( pMe );
    return EFAILED;
  }

  gMgr.bIdxTaken[ i ] = TRUE;
  pMe->uIndex         = i;
  pMe->streamID       = SIO_NO_STREAM_ID;
  pMe->m_pac          = AEE_GetAppContext();

  if( gMgr.pNextSPP == NULL )
  {
    q_init( &OEMBTExtSPP_ev_q );
    OEMBTExtSPP_ev_q_info.event_q_ptr = &OEMBTExtSPP_ev_q;
    OEMBTExtSPP_ev_q_info.max_q_depth = OEMBTEXTSPP_NUM_EVENTS;
    OEMBTExtSPP_ev_q_info.event_q_bit_mask = BT_EVQ_BREW_SPP_B;
    OEMBTExtSPP_ev_q_info.max_event_bytes = OEMBTExtSPP_get_max_event_bytes();
    bt_ec_register_brew_spp( OEMBTExtSPP_process_ev_queue );
  }

  // link into mgr
  pMe->pNextSPP = gMgr.pNextSPP;
  gMgr.pNextSPP = pMe;
  
  return SUCCESS;
}


//==========================================================================
//   IBTExtSPP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
int OEMBTExtSPP_Release( IBTExtSPP* pParent )
{
  OEMBTExtSPPobj_t*  pMe;
  OEMBTExtSPPobj_t** ppc;

  pMe = OEMBTExtSPP_CheckParentPtr( pParent );

  // decrement ref count
  if ( pMe != NULL )
  {
    for (ppc = &gMgr.pNextSPP; *ppc != NULL; ppc = &(*ppc)->pNextSPP)
    {
      if ((*ppc)->uIndex == pMe->uIndex)
      {
        gMgr.bIdxTaken[ pMe->uIndex ] = FALSE;
        *ppc = pMe->pNextSPP;
        pMe->pNextSPP = NULL;

        if ( pMe->streamID != SIO_NO_STREAM_ID )
        {
          sio_close( pMe->streamID, NULL );

          // just assume sio_close() succeeds
          pMe->streamID = SIO_NO_STREAM_ID;
        }
        if ( gMgr.pNextSPP == NULL )
        {
          bt_ec_deregister_brew_spp( OEMBTExtSPP_process_ev_queue );
        }
        break;
      }
    }

    if (pMe->m_pNotifier)
    {
      IBTEXTNOTIFIER_Release( pMe->m_pNotifier );
    }
    pMe->m_pNotifier = NULL;
    pMe->m_pac = NULL;

    sys_free(pMe);
    (void) AEEHandle_Clear( &gOEMBTExtHandleList, pParent->m_hBT );

    return SUCCESS;
  }
  return EFAILED;
}

// IBTEXTSPP_Open (see AEEBTExtSPP.h)
int OEMBTExtSPP_Open( 
  IBTExtSPP*                pParent, 
  const AEEBTSppOpenConfig* pOpenCfg
)
{
  sio_open_type       so;
  bt_spp_open_type    bso;
  char svcName[ BT_SD_MAX_SERVICE_NAME_LEN ];
  OEMBTExtSPPobj_t            *pMe;

  pMe = OEMBTExtSPP_CheckParentPtr( pParent );

  if ( (pMe == NULL) || (pOpenCfg == NULL) )
  {
    return EBADPARM;
  }

  // pointer validation
  if ( !AEE_CHECK_PTR( "OEMBTExtSPP_Open", pOpenCfg, 
                       sizeof(AEEBTSppOpenConfig), FALSE) ) 
  {
    return EMEMPTR;
  }

  if ( pMe->streamID != SIO_NO_STREAM_ID )
  {
    return EBADSTATE;
  }

  if ( pOpenCfg->pBDAddr != NULL )
  {
    pMe->BDAddr = *pOpenCfg->pBDAddr;
  }

  if ( pOpenCfg->pSvcName != NULL )
  {
    WSTRTOSTR( pOpenCfg->pSvcName, svcName, BT_SD_MAX_SERVICE_NAME_LEN );
  }

    /* set up TX watermark */
  dsm_queue_init( &pMe->tx_wm, WATERMARK_MAX, &pMe->tx_q );
  pMe->tx_wm.lo_watermark         = WATERMARK_LO;
  pMe->tx_wm.hi_watermark         = WATERMARK_HI;
  pMe->tx_wm.lowater_func_ptr     = tx_lowater_func_table[ pMe->uIndex ];
  pMe->tx_wm.hiwater_func_ptr     = tx_hiwater_func_table[ pMe->uIndex ];

  /* set up RX watermark */
  dsm_queue_init( &pMe->rx_wm, WATERMARK_MAX, &pMe->rx_q );
  pMe->rx_wm.lo_watermark         = WATERMARK_LO;
  pMe->rx_wm.hi_watermark         = WATERMARK_HI;
  pMe->rx_wm.lowater_func_ptr     = rx_lowater_func_table[ pMe->uIndex ];
  pMe->rx_wm.non_empty_func_ptr   = non_empty_func_table[ pMe->uIndex ];
  pMe->rx_wm.hiwater_func_ptr     = rx_hiwater_func_table[ pMe->uIndex ];

  pMe->dsm_item_ptr               = NULL;


  so.port_id      = SIO_PORT_BT_SPP;
  so.stream_mode  = SIO_GENERIC_MODE;
  so.tx_flow      = SIO_FCTL_OFF;
  so.rx_flow      = SIO_FCTL_OFF;
  so.tx_queue     = &pMe->tx_wm;
  so.rx_queue     = &pMe->rx_wm;
  so.rx_func_ptr  = NULL;
  so.bt_open_ptr  = &bso;

  bso.client_app            = pOpenCfg->bClientApp;
  bso.bd_addr_ptr           = (bt_bd_addr_type*)&pMe->BDAddr;
  bso.service_uuid          = pOpenCfg->uSvcId;
  bso.service_version       = pOpenCfg->uSvcVersion;
  bso.rc_server_channel     = (bt_spp_scn_enum_type)pOpenCfg->uChannelNumber;
  bso.service_name_str_ptr  = ( pOpenCfg->pSvcName != NULL ) ? svcName : NULL;
  bso.max_frame_size        = pOpenCfg->uMaxFrameSize;
  bso.status_change_fptr    = OEMBTExtSPP_StatusChangeCB;
  bso.config_change_fptr    = OEMBTExtSPP_ConfigChangeCB;
  bso.modem_status_fptr     = OEMBTExtSPP_ModemStatusCB;
  bso.line_error_fptr       = OEMBTExtSpp_LineErrorCB;

  rex_enter_crit_sect( &OEMBTExtSPP_crit_sect );
  pMe->streamID = SIO_MAX_STREAM; // indicates opening
  pMe->streamID = sio_open( &so );
  rex_leave_crit_sect( &OEMBTExtSPP_crit_sect );

  return ( pMe->streamID == SIO_NO_STREAM_ID ) ? EFAILED : SUCCESS;
}

// IBTEXTSPP_IOCtl (see AEEBTExtSPP.h)
int OEMBTExtSPP_IOCtl( 
  IBTExtSPP* pParent, 
  AEEBTIOCtlCommand cmd, 
  AEEBTIoCtlParam* pParam
)
{
  sio_ioctl_cmd_type    ioctl_cmd;
  sio_ioctl_param_type  param;
  bt_spp_status_type    spp_status;
  bt_spp_config_type    spp_cfg;
  OEMBTExtSPPobj_t            *pMe;

  pMe = OEMBTExtSPP_CheckParentPtr( pParent );

  if ( (pMe == NULL) || (cmd >= AEEBT_IOCTL_MAX) )
  {
    return EBADPARM;
  }

  // pointer validation
  if ( (pParam != NULL) && 
       !AEE_CHECK_PTR( "OEMBTExtSPP_IOCtl", pParam, 
                       sizeof(AEEBTIoCtlParam), TRUE) ) 
  {
    return EMEMPTR;
  }

  if ( pMe->streamID == SIO_NO_STREAM_ID )
  {
    return EBADSTATE;
  }

  switch ( cmd )
  {
    case AEEBT_IOCTL_INBOUND_FLOW_ENABLE:
      ioctl_cmd = SIO_IOCTL_INBOUND_FLOW_ENABLE;
      break;
    case AEEBT_IOCTL_INBOUND_FLOW_DISABLE:
      ioctl_cmd = SIO_IOCTL_INBOUND_FLOW_DISABLE;
      break;
    case AEEBT_IOCTL_FLUSH_TX:
      ioctl_cmd = SIO_IOCTL_FLUSH_TX;
      param.record_flush_func_ptr = flush_tx_func_table[ pMe->uIndex ];
      break;
    case AEEBT_IOCTL_CD_ASSERT:
      ioctl_cmd = SIO_IOCTL_CD_ASSERT;
      break;
    case AEEBT_IOCTL_CD_DEASSERT:
      ioctl_cmd = SIO_IOCTL_CD_DEASSERT;
      break;
    case AEEBT_IOCTL_RI_ASSERT:
      ioctl_cmd = SIO_IOCTL_RI_ASSERT;
      break;
    case AEEBT_IOCTL_RI_DEASSERT:
      ioctl_cmd = SIO_IOCTL_RI_DEASSERT;
      break;
    case AEEBT_IOCTL_DSR_ASSERT:
      ioctl_cmd = SIO_IOCTL_DSR_ASSERT;
      break;
    case AEEBT_IOCTL_DSR_DEASSERT:
      ioctl_cmd = SIO_IOCTL_DSR_DEASSERT;
      break;
    case AEEBT_IOCTL_ENABLE_DTR_EVENT:
      ioctl_cmd = SIO_IOCTL_ENABLE_DTR_EVENT;
      param.enable_dte_ready_event = enable_dtr_func_table[ pMe->uIndex ];
      break;
    case AEEBT_IOCTL_GET_DTR:
      if ( pParam == NULL )
      {
        return EBADPARM;
      }
      ioctl_cmd = SIO_IOCTL_DTE_READY_ASSERTED;
      param.dte_ready_asserted = pParam->pbDTRAsserted;
      break;
    case AEEBT_IOCTL_DISABLE_DTR_EVENT:
      ioctl_cmd = SIO_IOCTL_DISABLE_DTR_EVENT;
      break;
    case AEEBT_IOCTL_GET_RTS:
      if ( pParam == NULL )
      {
        return EBADPARM;
      }
      ioctl_cmd = SIO_IOCTL_GET_CURRENT_RTS;
      param.rts_asserted = pParam->pbRTSAsserted;
      break;
    case AEEBT_IOCTL_BT_CONFIGURE:
      if ( pParam == NULL )
      {
        return EBADPARM;
      }
      spp_cfg.baudrate       = pParam->pSppConfig->uBaudRate; 
      spp_cfg.format         = pParam->pSppConfig->uFormat;
      spp_cfg.flow_ctrl      = pParam->pSppConfig->uFlowCtrl;
      spp_cfg.xon_char       = pParam->pSppConfig->uXonChar;
      spp_cfg.xoff_char      = pParam->pSppConfig->uXoffChar;
      spp_cfg.max_frame_size = pParam->pSppConfig->uMaxFrameSize;
      ioctl_cmd = SIO_IOCTL_BT_CONFIGURE;
      param.bt_spp_config_ptr = &spp_cfg;
      break;
    case AEEBT_IOCTL_BT_DISCONNECT:
      ioctl_cmd = SIO_IOCTL_BT_DISCONNECT;
      break;
    case AEEBT_IOCTL_BT_GET_STATUS:
      if ( pParam == NULL )
      {
        return EBADPARM;
      }
      ioctl_cmd = SIO_IOCTL_BT_GET_STATUS;
      param.bt_spp_status_ptr = &spp_status;
      break;
    default:
      return EBADPARM;
  }

  sio_ioctl( pMe->streamID, ioctl_cmd, &param );

  if ( cmd == AEEBT_IOCTL_BT_GET_STATUS )
  {
    /*lint -save -e645*/
    /*lint -save -e613*/
    if ( AEEBT_BD_ADDRS_EQUAL( &spp_status.bd_addr, &pMe->BDAddr ) == FALSE )
    {
      return EFAILED;
    }
    pParam->pSppStatus->pBDAddr     = &pMe->BDAddr;
    pParam->pSppStatus->uID         = pMe->uIndex;
    pParam->pSppStatus->bClientApp  = spp_status.client_app;
    pParam->pSppStatus->state       = (AEEBTSppState)spp_status.spp_state;
    pParam->pSppStatus->uSvcID      = spp_status.service_uuid;
    pParam->pSppStatus->uSvcVersion = spp_status.service_version;
    pParam->pSppStatus->uReason     = 
      OEMBTExtSPP_ConvertBTReason( spp_status.spp_reason );
    pParam->pSppStatus->uChannelNumber = spp_status.rc_server_channel;
    /*lint -restore*/
    /*lint -restore*/
  }

  return SUCCESS;
}

// IBTEXTSPP_Read (see AEEBTExtSPP.h)
int32 OEMBTExtSPP_Read( 
  IBTExtSPP* pParent, 
  char* pDst, 
  int32 uMaxBytes
)
{
  int32 bytesRead = 0;
  int32 totalBytesRead = 0;
  OEMBTExtSPPobj_t            *pMe;

  pMe = OEMBTExtSPP_CheckParentPtr( pParent );

  if ( (pMe == NULL) || (pDst == NULL) || (uMaxBytes == 0) )
  {
    return AEE_STREAM_ERROR;
  }

  // pointer validation
  if (!AEE_CHECK_PTR( "OEMBTExtSPP_Read", pDst, uMaxBytes, TRUE)) 
  {
    return AEE_STREAM_ERROR;
  }

  if ( pMe->streamID == SIO_NO_STREAM_ID )
  {
    return AEE_STREAM_ERROR;
  }

  while (TRUE)
  {
    if ( pMe->dsm_item_ptr != NULL )
    {
      bytesRead = dsm_pullup( &pMe->dsm_item_ptr, (pDst+totalBytesRead), 
                              (uMaxBytes-totalBytesRead) );
      if ( bytesRead == 0 )
      {
        MSG_ERROR( "OEMBTExtSPP_Read - dsm_pullup() returned 0", 0, 0, 0 );
      }

      totalBytesRead += bytesRead;
      if ( totalBytesRead >= uMaxBytes )  // destination full?
      {
        break;
      }
      /* dsm_pullup will free the dsm_item after pulling off the last byte */
    }
    if ( pMe->dsm_item_ptr == NULL )  // DSM item has been freed?
    {
      pMe->dsm_item_ptr = dsm_dequeue( &pMe->rx_wm ); // get next one
      if ( pMe->dsm_item_ptr == NULL )  // rx queue is empty?
      {
        break;
      }
    }
  }

  if ( totalBytesRead == 0 )
  {
    return AEE_STREAM_WOULDBLOCK;
  }
  else
  {
    return totalBytesRead;
  }
}

// IBTEXTSPP_Readable (see AEEBTExtSPP.h)
void OEMBTExtSPP_Readable( IBTExtSPP* pParent, AEECallback* pCb )
{
  OEMBTExtSPPobj_t            *pMe;
  
  pMe = OEMBTExtSPP_CheckParentPtr( pParent );

  if ( (pMe == NULL) || (pCb == NULL) ||
       (!AEE_CHECK_PTR( "OEMBTExtSPP_Readable", pCb, 
                        sizeof(AEECallback), TRUE)) ||
       (pCb->pfnNotify == NULL) || (pMe->streamID == SIO_NO_STREAM_ID)
     )
  {
    return;
  }

  CALLBACK_Cancel(pCb);


  rex_enter_crit_sect( &OEMBTExtSPP_crit_sect );
   // readable? check if the Watermark has any DSM items queued on it or if
   // the current dsm item has  unread data still left on it.
  if ( (q_cnt( pMe->rx_wm.q_ptr ) > 0 ) || (pMe->dsm_item_ptr != NULL) )
  {
    ISHELL_Resume( pParent->pIShell, pCb );
  }
  else
  {
    // queue callback
    pCb->pCancelData  = &pMe->m_pcbRead;
    pCb->pfnCancel    = OEMBTExtSPP_Cancel;
    pCb->pNext        = pMe->m_pcbRead;
    pMe->m_pcbRead    = pCb;
  }
  rex_leave_crit_sect( &OEMBTExtSPP_crit_sect );
}


// IBTEXTSPP_Write (see AEEBTExtSPP.h)
int32 OEMBTExtSPP_Write( 
  IBTExtSPP* pParent, 
  char* pSrc, 
  int32 uNumBytes
)
{
  dsm_item_type*  dsm_ptr;
  uint16 count;
  int32 bytesWritten = 0;
  OEMBTExtSPPobj_t            *pMe;

  pMe = OEMBTExtSPP_CheckParentPtr( pParent );

  if ( (pMe == NULL) || (pSrc == NULL) )
  {
    return AEE_STREAM_ERROR;
  }

  // pointer validation
  if (!AEE_CHECK_PTR("OEMBTExtSPP_Write", pSrc, uNumBytes, FALSE)) 
  {
    return AEE_STREAM_ERROR;
  }

  if ( pMe->streamID == SIO_NO_STREAM_ID )
  {
    return AEE_STREAM_ERROR;
  }

  while ( (pMe->status.state == AEEBT_SPP_ST_CONNECTED) &&
          pMe->bOKToSend && (uNumBytes > 0) &&
          (dsm_ptr = bt_get_free_dsm_ptr( BT_TL_RFCOMM, 
                                          WATERMARK_ENQUEUE_SIZE )) != NULL )
  {
    count = dsm_pushdown_tail( &dsm_ptr, pSrc, 
                               MIN( WATERMARK_ENQUEUE_SIZE, uNumBytes ), 
                               (dsm_mempool_id_enum_type)dsm_ptr->pool_id );
    sio_transmit(pMe->streamID, dsm_ptr );

    pSrc          += count;
    bytesWritten  += count;
    uNumBytes     -= count;
  }
  
  if ( bytesWritten == 0 )
  {
    MSG_ERROR( "OEMBTExtSPP_Write - ok=%x dsm=%x cnt=%x", pMe->bOKToSend,
               dsm_ptr, uNumBytes );
    return AEE_STREAM_WOULDBLOCK;
  }
  else
  {
    return bytesWritten;
  }
}

// IBTEXTSPP_Writeable (see AEEBTExtSPP.h)
void OEMBTExtSPP_Writeable( IBTExtSPP* pParent, AEECallback* pCb )
{
  OEMBTExtSPPobj_t            *pMe;

  pMe = OEMBTExtSPP_CheckParentPtr( pParent );

  if ( (pMe == NULL) || (pCb == NULL) ||
       (!AEE_CHECK_PTR( "OEMBTExtSPP_Writeable", pCb, 
                        sizeof(AEECallback), TRUE)) ||
       (pCb->pfnNotify == NULL) || (pMe->streamID == SIO_NO_STREAM_ID)
     )
  {
    return;
  }

  CALLBACK_Cancel(pCb);

  rex_enter_crit_sect( &OEMBTExtSPP_crit_sect );

  // writeable?
  if ( pMe->bOKToSend )
  {
    ISHELL_Resume( pParent->pIShell, pCb );
  }
  else
  {
    // queue callback
    pCb->pCancelData  = &pMe->m_pcbWrite;
    pCb->pfnCancel    = OEMBTExtSPP_Cancel;
    pCb->pNext        = pMe->m_pcbWrite;
    pMe->m_pcbWrite   = pCb;
  }

  rex_leave_crit_sect( &OEMBTExtSPP_crit_sect );
}

// IBTEXTSPP_Close (see AEEBTExtSPP.h)
int OEMBTExtSPP_Close( IBTExtSPP* pParent )
{
  OEMBTExtSPPobj_t            *pMe;

  pMe = OEMBTExtSPP_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  if ( pMe->streamID == SIO_NO_STREAM_ID )
  {
    return EBADSTATE;
  }

  if ( pMe->m_pcbRead != NULL )
  {
    CALLBACK_Cancel( pMe->m_pcbRead );
    pMe->m_pcbRead = NULL;
  }

  if ( pMe->m_pcbWrite != NULL )
  {
    CALLBACK_Cancel( pMe->m_pcbWrite );
    pMe->m_pcbWrite = NULL;
  }

  // free all DSM items
  dsm_empty_queue( &pMe->tx_wm );
  dsm_empty_queue( &pMe->rx_wm );
  dsm_free_packet( &pMe->dsm_item_ptr );

  sio_close( pMe->streamID, NULL );

  return SUCCESS;
}

// IBTEXTSPP_GetOEMHandle (see AEEBTExtSPP.h)
int OEMBTExtSPP_GetOEMHandle(IBTExtSPP *pParent, int32 *pnHandle)
{
  OEMBTExtSPPobj_t *pDummy;

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
int OEMBTExtSPP_Clone(IBTExtSPP *pParent, int32 nHandle)
{

  OEMBTExtSPPobj_t *pMe;

  // Get the pMe referred to by nHandle
  if( AEEHandle_From( &gOEMBTExtHandleList, nHandle,
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  // Release the OEMBTExtSPP object associated with this pParent.  It is
  // not needed, because the purpose of this function is to replace it with
  // the alternative OEMBTExtSPP object that is referred to by nHandle.
  OEMBTExtSPP_Release(pParent);

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
  if (SUCCESS != ISHELL_CreateInstance( pParent->pIShell, 
      AEECLSID_BLUETOOTH_NOTIFIER, (void**) &pMe->m_pNotifier ))
  {
    // This error shouldn't really happen, and, not really sure what
    // to do if it does.
    return EFAILED;
  }

  return SUCCESS;
}

// IBTEXTSPP_GetuID (see AEEBTExtSPP.h)
int OEMBTExtSPP_GetuID(IBTExtSPP *pParent, uint16 *puID)
{
  OEMBTExtSPPobj_t *pMe;

  if (!puID) return EFAILED;

  pMe = OEMBTExtSPP_CheckParentPtr( pParent );
  if (pMe)
  {
    *puID = pMe->uIndex;
    return SUCCESS;
  }
  return EFAILED;
}

int OEMBTExtSPP_GetStatus(IBTExtSPP *pParent, AEEBTSppStatus *pssStatus)
{
  OEMBTExtSPPobj_t *pMe;

  pMe = OEMBTExtSPP_CheckParentPtr( pParent );
  if (pMe)
  {
    *pssStatus = pMe->status;
    return SUCCESS;
  }
  return EFAILED;
}

//==========================================================================
//   static helper functions
//==========================================================================
static void OEMBTExtSPP_Cancel( AEECallback* pcb )
{
  AEECallback** ppcb;

  rex_enter_crit_sect( &OEMBTExtSPP_crit_sect );

  ppcb = pcb->pCancelData;
  //Verify that it is me set as the cancel function.
  //Cross-task cancellations need this check.
  if( pcb->pfnCancel == OEMBTExtSPP_Cancel )
  {
    pcb->pfnCancel = NULL;
    pcb->pCancelData = NULL;

    for ( ; *ppcb != NULL; ppcb = &(*ppcb)->pNext)
    {
      if (pcb == *ppcb) 
      {
        *ppcb = pcb->pNext;
        pcb->pNext = NULL;
        break;
      }
    }
  }

  rex_leave_crit_sect( &OEMBTExtSPP_crit_sect );
}

static uint16 OEMBTExtSPP_ConvertBTReason( bt_event_reason_type bt_reason )
{
  switch ( bt_reason )
  {
    case BT_EVR_GN_SUCCESS:
      return AEEBT_SPP_ERR_NONE;
    case BT_EVR_SPP_OUT_OF_SERVER_CHANNELS:
    case BT_EVR_SPP_SDP_REGISTRATION_FAILED:
    case BT_EVR_SPP_RFCOMM_REGISTRATION_FAILED:
      return AEEBT_SPP_ERR_OUT_OF_RESOURCES;
    case BT_EVR_SPP_SDP_TIMEOUT:
      return AEEBT_SPP_ERR_REMOTE_TIMEOUT;
    default:
      return AEEBT_SPP_ERR_FAILED;
  }
}

static OEMBTExtSPPobj_t* OEMBTExtSPP_FindMe( sio_stream_id_type streamID)
{
  OEMBTExtSPPobj_t* pSPP = NULL;

  if ( streamID != SIO_NO_STREAM_ID )
  {
    for (pSPP = gMgr.pNextSPP; pSPP != NULL; pSPP = pSPP->pNextSPP)
    {
      if ( pSPP->streamID == streamID )
      {
        break;
      }
    }
  }
  return pSPP;
}

///////////////////////////////////////////////////////////////////////
// Event handler functions:
///////////////////////////////////////////////////////////////////////

static void OEMBTExtSPP_ev_flush_tx( bt_ev_msg_type *bt_ev_ptr )
{
  OEMBTExtSPPobj_t*   pMe;
  AEEBTNotification*  pN = NULL;

  pMe = OEMBTExtSPP_FindMe( bt_ev_ptr->ev_msg.ev_spp_flush_tx.stream_id );
  if( pMe )
  {
    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "SPPEvCb - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->data.uID = pMe->uIndex;
      pN->uID = AEEBT_SPP_EVT_TX_FLUSHED;
      pN->uID = (pN->uID << 16) | NMASK_BT_SPP;
      IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
    }
  }
}

static void OEMBTExtSPP_ev_enable_dtr( bt_ev_msg_type *bt_ev_ptr )
{
  OEMBTExtSPPobj_t*   pMe;
  AEEBTNotification*  pN = NULL;

  pMe = OEMBTExtSPP_FindMe( bt_ev_ptr->ev_msg.ev_spp_enable_dtr.stream_id );
  if( pMe )
  {
    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "SPPEvCb - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->data.uID = pMe->uIndex;
      pN->uID = AEEBT_SPP_EVT_DTR_CHANGED;
      pN->uID = (pN->uID << 16) | NMASK_BT_SPP;
      IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
    }
  }
}

static void OEMBTExtSPP_ev_tx_lo_wm( bt_ev_msg_type *bt_ev_ptr )
{
  OEMBTExtSPPobj_t*   pMe;
  pMe = OEMBTExtSPP_FindMe( bt_ev_ptr->ev_msg.ev_spp_tx_lo_wm.stream_id );
  if( (pMe != NULL) && (pMe->status.state == AEEBT_SPP_ST_CONNECTED) )
  {
    /* If Low WM has been exceeded already, don't set OKToSend flag to TRUE. 
       As long as the WM is above the Lo WM, it may not be safe to set this 
       to TRUE. This prevents an overflow of the WM when the handling of this 
       event is delayed and more data has already been put on the WM in 
       between the LOW WM callback and this event handler. */
    if (pMe->tx_wm.current_cnt <= WATERMARK_LO)
    {
      pMe->bOKToSend = TRUE;
      while (pMe->m_pcbWrite != NULL)
      {
        AEECallback* pCb = pMe->m_pcbWrite;
        AEE_APP_RESUME( pCb, pMe->m_pac );
        pMe->m_pcbWrite = NULL;
      }
    }
    else /* debug only */
    {
      MSG_HIGH("OEMBTExtSPP_ev_tx_lo_wm: Count ABOVE Low WM, don't set bOKToSend!!",0,0,0);
    }
  }
}

static void OEMBTExtSPP_ev_rx_hi_wm( bt_ev_msg_type *bt_ev_ptr )
{
  //MSG_MED( "RxHiWM - c=%d r=%d d=%d", pMe->rx_wm.current_cnt, 
  //         (pMe->m_pcbRead != NULL), pMe->bDataRcvd );
  sio_ioctl( bt_ev_ptr->ev_msg.ev_spp_rx_hi_wm.stream_id, 
             SIO_IOCTL_INBOUND_FLOW_DISABLE, NULL );
}

static void OEMBTExtSPP_ev_rx_lo_wm( bt_ev_msg_type *bt_ev_ptr )
{
  //MSG_MED( "RxLoWM - c=%d r=%d w=%d", pMe->rx_wm.current_cnt, 
  //         (pMe->m_pcbRead != NULL), (pMe->m_pcbWrite != NULL) );
  sio_ioctl( bt_ev_ptr->ev_msg.ev_spp_rx_lo_wm.stream_id, 
             SIO_IOCTL_INBOUND_FLOW_ENABLE, NULL );
}

static void OEMBTExtSPP_ev_rx_gne( bt_ev_msg_type *bt_ev_ptr )
{
  OEMBTExtSPPobj_t*   pMe;
  pMe = OEMBTExtSPP_FindMe( bt_ev_ptr->ev_msg.ev_spp_rx_gne.stream_id );
  if( pMe )
  {
    while (pMe->m_pcbRead != NULL)
    {
      AEECallback* pCb = pMe->m_pcbRead;
      AEE_APP_RESUME( pCb, pMe->m_pac );
      pMe->m_pcbRead = NULL;
    }
  }
}

static void OEMBTExtSPP_ev_line_error( bt_ev_msg_type *bt_ev_ptr )
{
  OEMBTExtSPPobj_t*   pMe;
  AEEBTNotification*  pN = NULL;
  bt_spp_le_type*     line_error_ptr = 
    &bt_ev_ptr->ev_msg.ev_spp_line_error.line_error;

  pMe = OEMBTExtSPP_FindMe( line_error_ptr->stream_id );
  if( pMe )
  {
    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "SPPEvCb - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->data.pLineError->uID = pMe->uIndex;
    }      
    switch ( line_error_ptr->cur_line_error )
    {
      case BT_SPP_LE_NONE:
        pMe->lineError.lineErrorType  = AEEBT_SPP_LE_NONE;
        break;
      case BT_SPP_LE_OVERRUN_ERROR:
        pMe->lineError.lineErrorType  = AEEBT_SPP_LE_OVERRUN_ERROR;
        pMe->lineError.overruns       = line_error_ptr->overruns;
        break;
      case BT_SPP_LE_PARITY_ERROR:
        pMe->lineError.lineErrorType  = AEEBT_SPP_LE_PARITY_ERROR;
        pMe->lineError.parityErrors   = line_error_ptr->parity_errors;
        break;
      case BT_SPP_LE_FRAMING_ERROR:
        pMe->lineError.lineErrorType  = AEEBT_SPP_LE_FRAMING_ERROR;
        pMe->lineError.framingErrors  = line_error_ptr->framing_errors;
        break;
      default:
        IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
        return;
    }

    if( pN != NULL )
    {
      pN->uID             = AEEBT_SPP_EVT_LINE_ERROR;
      pN->data.pLineError = &pMe->lineError;
      
      pN->uID = (pN->uID << 16) | NMASK_BT_SPP;
      IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
    }
  }
}



static void OEMBTExtSPP_ev_modem_status( bt_ev_msg_type *bt_ev_ptr )
{
  OEMBTExtSPPobj_t*   pMe;
  AEEBTNotification*  pN = NULL;
  bt_spp_ms_type*     modem_status_ptr = 
    &bt_ev_ptr->ev_msg.ev_spp_modem_status.modem_status;

  pMe = OEMBTExtSPP_FindMe( modem_status_ptr->stream_id );
  if( pMe )
  {
    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "SPPEvCb - Can't get free not.",
                 0, 0, 0 );
    }

    pMe->modemStatus.uID          = pMe->uIndex;
    pMe->modemStatus.changedMask  = 
      (AEEBTModemStatusBitmap) modem_status_ptr->ms_changed_mask;
    pMe->modemStatus.status       = 
      (AEEBTModemStatusBitmap) modem_status_ptr->modem_status;
    pMe->modemStatus.breakPresent = modem_status_ptr->break_present;
    pMe->modemStatus.breakLength  = modem_status_ptr->break_length;
    pMe->modemStatus.breaks       = modem_status_ptr->breaks;

    if( pN != NULL )
    {
      pN->uID               = AEEBT_SPP_EVT_MODEM_STATUS;
      pN->data.pModemStatus  = &pMe->modemStatus;
      
      pN->uID = (pN->uID << 16) | NMASK_BT_SPP;
      IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
    }
  }
}

static void OEMBTExtSPP_ev_config_change( bt_ev_msg_type *bt_ev_ptr )
{
  OEMBTExtSPPobj_t*    pMe;
  AEEBTNotification*   pN = NULL;
  bt_spp_cfg_rpt_type* config_ptr =
    &bt_ev_ptr->ev_msg.ev_spp_config_change.config_change;

  pMe = OEMBTExtSPP_FindMe( config_ptr->stream_id );
  if( pMe )
  {
    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "SPPEvCb - Can't get free not.",
                 0, 0, 0 );
    }

    pMe->config.uID           = pMe->uIndex;
    pMe->config.uBaudRate     = config_ptr->spp_config.baudrate;
    pMe->config.uFormat       = config_ptr->spp_config.format;
    pMe->config.uFlowCtrl     = config_ptr->spp_config.flow_ctrl;
    pMe->config.uXonChar      = config_ptr->spp_config.xon_char;
    pMe->config.uXoffChar     = config_ptr->spp_config.xoff_char;
    pMe->config.uMaxFrameSize = config_ptr->spp_config.max_frame_size;

    if( pN != NULL )
    {
      pN->uID             = AEEBT_SPP_EVT_CONFIG_CHANGED;
      pN->data.pSppConfig  = &pMe->config;
      
      pN->uID = (pN->uID << 16) | NMASK_BT_SPP;
      IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
    }
  }
}

static void OEMBTExtSPP_ev_status_change( bt_ev_msg_type *bt_ev_ptr )
{
  OEMBTExtSPPobj_t*   pMe;
  AEEBTNotification*  pN = NULL;
  bt_spp_status_type* pStatus =
    &bt_ev_ptr->ev_msg.ev_spp_status_change.status_change;

  pMe = OEMBTExtSPP_FindMe( pStatus->stream_id );
  if( pMe )
  {
    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "SPPEvCb - Can't get free not.",
                 0, 0, 0 );
    }

    MSG_MED( "StatusChangeCB - id=%x st=%x r=%x", 
             pMe->uIndex, pStatus->spp_state, pStatus->spp_reason );
    
    pMe->BDAddr             = *((AEEBTBDAddr*)&pStatus->bd_addr);
    pMe->streamID           = pStatus->stream_id;
    
    pMe->status.pBDAddr     = &pMe->BDAddr;
    pMe->status.uID         = pMe->uIndex;
    pMe->status.bClientApp  = pStatus->client_app;
    pMe->status.state       = (AEEBTSppState) pStatus->spp_state;
    pMe->status.uSvcID      = pStatus->service_uuid;
    pMe->status.uSvcVersion = pStatus->service_version;
    pMe->status.uChannelNumber  = pStatus->rc_server_channel;

    if( pN != NULL )
    {
      pN->data.pSppStatus     = &pMe->status;
    }
    
    switch ( pStatus->spp_state )
    {
      case BT_SPP_ST_CLOSED:
        if( pN != NULL )
        {
          pN->uID = AEEBT_SPP_EVT_CLOSED;
        }
        pMe->streamID = SIO_NO_STREAM_ID;
        break;
      case BT_SPP_ST_OPENING:
        IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
        return;
      case BT_SPP_ST_OPEN_ERROR:
        if( pN != NULL )
        {
          pN->uID = AEEBT_SPP_EVT_OPENED;
        }
        pMe->status.uReason = OEMBTExtSPP_ConvertBTReason( pStatus->spp_reason );
        break;
      case BT_SPP_ST_OPEN:
        pMe->bOKToSend = FALSE;
        dsm_empty_queue( &pMe->tx_wm );
        dsm_empty_queue( &pMe->rx_wm );
        dsm_free_packet( &pMe->dsm_item_ptr );
        if( pN != NULL )
        {
          pN->uID = AEEBT_SPP_EVT_OPENED;
        }
        pMe->status.uReason = AEEBT_SPP_ERR_NONE;
        break;
      case BT_SPP_ST_CONNECTED:
        pMe->bOKToSend = TRUE;
        if( pN != NULL )
        {
          pN->uID = AEEBT_SPP_EVT_CONNECTED;
        }
        break;
      case BT_SPP_ST_DISCONNECTING:
        pMe->bOKToSend = FALSE;
        IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
        return;
      case BT_SPP_ST_DISCONNECTED:
        pMe->bOKToSend = FALSE;
        if( pN != NULL )
        {
          pN->uID = AEEBT_SPP_EVT_DISCONNECTED;
        }
        dsm_empty_queue( &pMe->rx_wm );
        dsm_empty_queue( &pMe->tx_wm );
        dsm_free_packet( &pMe->dsm_item_ptr );
        break;
      default:
        IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
        return;
    }
    if( pN != NULL )
    {
      pN->uID = (pN->uID << 16) | NMASK_BT_SPP;
      IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
    }
  }
}

static void OEMBTExtSPP_process_ev_queue( void )
{
  ////////////////////////////////////////////////////////
  // This function is called from the BT task's signal
  // processing loop.
  ////////////////////////////////////////////////////////
  bt_ev_msg_type*  bt_ev_ptr;
  uint16           idx = 0;
  int cnt = q_cnt( &OEMBTExtSPP_ev_q );

  BT_VSTAT_MAX( cnt, OEMBTEXTSPP_MAX_EV_Q_DEPTH );
  
  while ( (bt_ev_ptr = (bt_ev_msg_type *) q_get( &OEMBTExtSPP_ev_q ) ) != NULL )
  {
    idx = bt_ev_ptr->ev_hdr.ev_type - BT_CMD_EV_SP_BASE;
    /* Check if the event received was for SPP else drop it */
    if (idx < ARR_SIZE(OEMBTExtSPP_ev_hndlr_table))
    {
      if( OEMBTExtSPP_ev_hndlr_table[ idx ] )
      {
        rex_enter_crit_sect( &OEMBTExtSPP_crit_sect );
        OEMBTExtSPP_ev_hndlr_table[ idx ]( bt_ev_ptr );
        rex_leave_crit_sect( &OEMBTExtSPP_crit_sect );
      }
    }
    else
    {
        MSG_ERROR( "OEMBTExtSPP_process_ev_queue - evt dropped %x", idx, 0, 0 );
    }
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }
}


////////////////////////////////////////////////////////////////////////////////
// Callbacks from either BT or DSM watermarks
////////////////////////////////////////////////////////////////////////////////

static void OEMBTExtSPP_StatusChangeCB( bt_spp_status_type* pStatus )
{
  bt_ev_msg_type bt_ev;
  bt_ev.ev_hdr.ev_type = BT_EV_SPP_STATUS_CHANGE;
  bt_ev.ev_msg.ev_spp_status_change.status_change = *pStatus;
  OEMBTExtSPP_store_bt_event( &bt_ev );
}

static void OEMBTExtSPP_ConfigChangeCB( bt_spp_cfg_rpt_type* config_ptr )
{
  bt_ev_msg_type bt_ev;
  bt_ev.ev_hdr.ev_type = BT_EV_SPP_CONFIG_CHANGE;
  bt_ev.ev_msg.ev_spp_config_change.config_change = *config_ptr;
  OEMBTExtSPP_store_bt_event( &bt_ev );
}

static void OEMBTExtSPP_ModemStatusCB( bt_spp_ms_type* modem_status_ptr )
{
  bt_ev_msg_type bt_ev;
  bt_ev.ev_hdr.ev_type = BT_EV_SPP_MODEM_STATUS;
  bt_ev.ev_msg.ev_spp_modem_status.modem_status = *modem_status_ptr;
  OEMBTExtSPP_store_bt_event( &bt_ev );
}

static void OEMBTExtSpp_LineErrorCB( bt_spp_le_type* line_error_ptr )
{
  bt_ev_msg_type bt_ev;
  bt_ev.ev_hdr.ev_type = BT_EV_SPP_LINE_ERROR;
  bt_ev.ev_msg.ev_spp_line_error.line_error = *line_error_ptr;
  OEMBTExtSPP_store_bt_event( &bt_ev );
}

static void OEMBTExtSPP_DataReceived( wm_cb_type rx_non_empty_func_ptr )
{
  /////////////////////////////////////////////////////////
  // NOTE: This DSM callback is called with tasks locked //
  /////////////////////////////////////////////////////////

  OEMBTExtSPPobj_t* pMe;
  bt_ev_msg_type bt_ev;
  boolean    found = FALSE;

  for ( pMe = gMgr.pNextSPP; (pMe != NULL) && !found; pMe = pMe->pNextSPP )
  {
    if (pMe->rx_wm.non_empty_func_ptr == rx_non_empty_func_ptr)
    {
      found = TRUE;
      bt_ev.ev_hdr.ev_type = BT_EV_SPP_RX_GNE;
      bt_ev.ev_msg.ev_spp_rx_gne.stream_id = pMe->streamID;
      OEMBTExtSPP_store_bt_event( &bt_ev );
    }
  }
}
static void OEMBTExtSPP_DataReceived1( void )
{
  OEMBTExtSPP_DataReceived( OEMBTExtSPP_DataReceived1 );
}
static void OEMBTExtSPP_DataReceived2( void )
{
  OEMBTExtSPP_DataReceived( OEMBTExtSPP_DataReceived2 );
}
static void OEMBTExtSPP_DataReceived3( void )
{
  OEMBTExtSPP_DataReceived( OEMBTExtSPP_DataReceived3 );
}
static void OEMBTExtSPP_DataReceived4( void )
{
  OEMBTExtSPP_DataReceived( OEMBTExtSPP_DataReceived4 );
}
static void OEMBTExtSPP_DataReceived5( void )
{
  OEMBTExtSPP_DataReceived( OEMBTExtSPP_DataReceived5 );
}

static void OEMBTExtSPP_RxLoWater( wm_cb_type rx_lowater_func_ptr )
{
  /////////////////////////////////////////////////////////
  // NOTE: This DSM callback is called with tasks locked //
  /////////////////////////////////////////////////////////

  OEMBTExtSPPobj_t* pMe;
  bt_ev_msg_type bt_ev;
  boolean found = FALSE;

  for ( pMe = gMgr.pNextSPP; (pMe != NULL) && !found; pMe = pMe->pNextSPP )
  {
    if ( pMe->rx_wm.lowater_func_ptr == rx_lowater_func_ptr )
    {
      bt_ev.ev_hdr.ev_type = BT_EV_SPP_RX_LO_WM;
      bt_ev.ev_msg.ev_spp_rx_lo_wm.stream_id = pMe->streamID;
      OEMBTExtSPP_store_bt_event( &bt_ev );
      found = TRUE;
    }
  }
}
static void OEMBTExtSPP_RxLoWater1( void )
{
  OEMBTExtSPP_RxLoWater( OEMBTExtSPP_RxLoWater1 );
}
static void OEMBTExtSPP_RxLoWater2( void )
{
  OEMBTExtSPP_RxLoWater( OEMBTExtSPP_RxLoWater2 );
}
static void OEMBTExtSPP_RxLoWater3( void )
{
  OEMBTExtSPP_RxLoWater( OEMBTExtSPP_RxLoWater3 );
}
static void OEMBTExtSPP_RxLoWater4( void )
{
  OEMBTExtSPP_RxLoWater( OEMBTExtSPP_RxLoWater4 );
}
static void OEMBTExtSPP_RxLoWater5( void )
{
  OEMBTExtSPP_RxLoWater( OEMBTExtSPP_RxLoWater5 );
}

static void OEMBTExtSPP_RxHiWater( wm_cb_type rx_hiwater_func_ptr )
{
  /////////////////////////////////////////////////////////
  // NOTE: This DSM callback is called with tasks locked //
  /////////////////////////////////////////////////////////

  OEMBTExtSPPobj_t* pMe;
  bt_ev_msg_type bt_ev;
  boolean found = FALSE;

  for ( pMe = gMgr.pNextSPP; (pMe != NULL) && !found; pMe = pMe->pNextSPP )
  {
    if ( pMe->rx_wm.hiwater_func_ptr == rx_hiwater_func_ptr )
    {
      bt_ev.ev_hdr.ev_type = BT_EV_SPP_RX_HI_WM;
      bt_ev.ev_msg.ev_spp_rx_hi_wm.stream_id = pMe->streamID;
      OEMBTExtSPP_store_bt_event( &bt_ev );

      found = TRUE;
    }
  }
}
static void OEMBTExtSPP_RxHiWater1( void )
{
  OEMBTExtSPP_RxHiWater( OEMBTExtSPP_RxHiWater1 );
}
static void OEMBTExtSPP_RxHiWater2( void )
{
  OEMBTExtSPP_RxHiWater( OEMBTExtSPP_RxHiWater2 );
}
static void OEMBTExtSPP_RxHiWater3( void )
{
  OEMBTExtSPP_RxHiWater( OEMBTExtSPP_RxHiWater3 );
}
static void OEMBTExtSPP_RxHiWater4( void )
{
  OEMBTExtSPP_RxHiWater( OEMBTExtSPP_RxHiWater4 );
}
static void OEMBTExtSPP_RxHiWater5( void )
{
  OEMBTExtSPP_RxHiWater( OEMBTExtSPP_RxHiWater5 );
}

static void OEMBTExtSPP_TxLowWater( wm_cb_type tx_lowater_func_ptr )
{
  /////////////////////////////////////////////////////////
  // NOTE: This DSM callback is called with tasks locked //
  /////////////////////////////////////////////////////////

  OEMBTExtSPPobj_t* pMe;
  bt_ev_msg_type bt_ev;
  boolean found = FALSE;

  for ( pMe = gMgr.pNextSPP; (pMe != NULL) && !found; pMe = pMe->pNextSPP )
  {
    if ( pMe->tx_wm.lowater_func_ptr == tx_lowater_func_ptr )
    {
      //pMe->bOKToSend = TRUE;
      found = TRUE;
      bt_ev.ev_hdr.ev_type = BT_EV_SPP_TX_LO_WM;
      bt_ev.ev_msg.ev_spp_tx_lo_wm.stream_id = pMe->streamID;
      OEMBTExtSPP_store_bt_event( &bt_ev );
    }
  }
}
static void OEMBTExtSPP_TxLoWater1( void )
{
  OEMBTExtSPP_TxLowWater( OEMBTExtSPP_TxLoWater1 );
}
static void OEMBTExtSPP_TxLoWater2( void )
{
  OEMBTExtSPP_TxLowWater( OEMBTExtSPP_TxLoWater2 );
}
static void OEMBTExtSPP_TxLoWater3( void )
{
  OEMBTExtSPP_TxLowWater( OEMBTExtSPP_TxLoWater3 );
}
static void OEMBTExtSPP_TxLoWater4( void )
{
  OEMBTExtSPP_TxLowWater( OEMBTExtSPP_TxLoWater4 );
}
static void OEMBTExtSPP_TxLoWater5( void )
{
  OEMBTExtSPP_TxLowWater( OEMBTExtSPP_TxLoWater5 );
}

static void OEMBTExtSPP_TxHiWater( wm_cb_type tx_hiwater_func_ptr )
{
  /////////////////////////////////////////////////////////
  // NOTE: This DSM callback is called with tasks locked //
  /////////////////////////////////////////////////////////

  OEMBTExtSPPobj_t* pMe;
  boolean found = FALSE;

  for ( pMe = gMgr.pNextSPP; (pMe != NULL) && !found; pMe = pMe->pNextSPP )
  {
    if ( pMe->tx_wm.hiwater_func_ptr == tx_hiwater_func_ptr )
    {
      pMe->bOKToSend = FALSE;
      found = TRUE;
    }
  }

}
static void OEMBTExtSPP_TxHiWater1( void )
{
  OEMBTExtSPP_TxHiWater( OEMBTExtSPP_TxHiWater1 );
}
static void OEMBTExtSPP_TxHiWater2( void )
{
  OEMBTExtSPP_TxHiWater( OEMBTExtSPP_TxHiWater2 );
}
static void OEMBTExtSPP_TxHiWater3( void )
{
  OEMBTExtSPP_TxHiWater( OEMBTExtSPP_TxHiWater3 );
}
static void OEMBTExtSPP_TxHiWater4( void )
{
  OEMBTExtSPP_TxHiWater( OEMBTExtSPP_TxHiWater4 );
}
static void OEMBTExtSPP_TxHiWater5( void )
{
  OEMBTExtSPP_TxHiWater( OEMBTExtSPP_TxHiWater5 );
}

static void OEMBTExtSPP_EnableDTR( int8 index )
{
  /////////////////////////////////////////////////////////
  // NOTE: This DSM callback is called with tasks locked //
  /////////////////////////////////////////////////////////

  OEMBTExtSPPobj_t* pMe;
  bt_ev_msg_type bt_ev;
  boolean found = FALSE;

  for ( pMe = gMgr.pNextSPP; (pMe != NULL) && !found; pMe = pMe->pNextSPP )
  {
    if ( pMe->uIndex == index )
    {
      found = TRUE;
      bt_ev.ev_hdr.ev_type = BT_EV_SPP_ENABLE_DTR;
      bt_ev.ev_msg.ev_spp_enable_dtr.stream_id = pMe->streamID;
      OEMBTExtSPP_store_bt_event( &bt_ev );
    }
  }
}
static void OEMBTExtSPP_EnableDTR1( void )
{
  OEMBTExtSPP_EnableDTR( 1 );
}
static void OEMBTExtSPP_EnableDTR2( void )
{
  OEMBTExtSPP_EnableDTR( 2 );
}
static void OEMBTExtSPP_EnableDTR3( void )
{
  OEMBTExtSPP_EnableDTR( 3 );
}
static void OEMBTExtSPP_EnableDTR4( void )
{
  OEMBTExtSPP_EnableDTR( 4 );
}
static void OEMBTExtSPP_EnableDTR5( void )
{
  OEMBTExtSPP_EnableDTR( 5 );
}


static void OEMBTExtSPP_FlushTx( int8 index )
{
  /////////////////////////////////////////////////////////
  // NOTE: This DSM callback is called with tasks locked //
  /////////////////////////////////////////////////////////

  OEMBTExtSPPobj_t* pMe;
  bt_ev_msg_type bt_ev;
  boolean found = FALSE;

  for ( pMe = gMgr.pNextSPP; (pMe != NULL) && !found; pMe = pMe->pNextSPP )
  {
    if ( pMe->uIndex == index )
    {
      found = TRUE;
      bt_ev.ev_hdr.ev_type = BT_EV_SPP_FLUSH_TX;
      bt_ev.ev_msg.ev_spp_flush_tx.stream_id = pMe->streamID;
      OEMBTExtSPP_store_bt_event( &bt_ev );
    }
  }
}
static void OEMBTExtSPP_FlushTx1( void )
{
  OEMBTExtSPP_FlushTx( 1 );
}
static void OEMBTExtSPP_FlushTx2( void )
{
  OEMBTExtSPP_FlushTx( 2 );
}
static void OEMBTExtSPP_FlushTx3( void )
{
  OEMBTExtSPP_FlushTx( 3 );
}
static void OEMBTExtSPP_FlushTx4( void )
{
  OEMBTExtSPP_FlushTx( 4 );
}
static void OEMBTExtSPP_FlushTx5( void )
{
  OEMBTExtSPP_FlushTx( 5 );
}

static OEMBTExtSPPobj_t* OEMBTExtSPP_CheckParentPtr( IBTExtSPP* pParent )
{
  OEMBTExtSPPobj_t *rv;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&rv ) != TRUE )
  {
    rv = NULL;
  }
  
  return rv;
}

static void OEMBTExtSPP_store_bt_event( bt_ev_msg_type* bt_ev_ptr )
{
  bt_store_bt_event( bt_ev_ptr, &OEMBTExtSPP_ev_q_info );
}

static uint16 OEMBTExtSPP_get_max_event_bytes( void )
{
  uint16 max_eb;

  max_eb =              sizeof( bt_ev_spp_flush_tx_type      )  ;
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_enable_dtr_type    ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_tx_hi_wm_type      ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_tx_lo_wm_type      ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_tx_gne_type        ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_rx_hi_wm_type      ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_rx_lo_wm_type      ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_rx_gne_type        ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_line_error_type    ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_modem_status_type  ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_config_change_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_spp_status_change_type ) );

  max_eb += sizeof( bt_ev_hdr_type );

  return max_eb;
}

#endif // defined(FEATURE_IBT)

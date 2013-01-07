/*===========================================================================

FILE:      OEMBTExtAG.c

SERVICES:  BlueTooth Audio Gateway QCT Extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth audio gateway.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2004-2009 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtAG.c#3 $
  $DateTime: 2009/04/14 15:01:41 $
  $Author: phuongn $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-04-14   pn  Watermark queues get initialized by dsm_queue_init().
  2009-01-13   rs  Moved the emptying of external I/O watermark queue functions 
                   from bt to OEM.
  2008-10-06   co  Updated the copyright block.
  2008-10-06   co  Replaced deprecated functions.

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT)
#include "bt.h"
#include "msg.h"
#include "snd.h"
#include "voc.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtAG.h"

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

// one of these per OEMBTExtAG Instance
typedef struct
{
  bt_app_id_type      m_appId;
  AECHAR              m_memToDial[ AEEBT_MAX_MEM_DIAL_DIGITS+1 ];
  AECHAR              m_numToDial[ AEEBT_MAX_PHONE_NUM_DIGITS+1 ];

  /* for External I/O */
  AEECallback*        m_pCb;    // for AT commands forwarded by BT driver
  boolean             m_bDisablingExtIO;
  dsm_item_type*      m_pDSMItem;
  dsm_watermark_type* m_pTxWm;
  dsm_watermark_type* m_pRxWm;
  q_type              m_txQ;
  q_type              m_rxQ;

  ACONTEXT*           m_pac;
  IBTExtNotifier*     m_pNotifier;
} OEMBTExtAGobj_t;

//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static void OEMBTExtAG_ATCmdRxCb( void );
static int  OEMBTExtAG_CheckCmdStatus( bt_cmd_status_type stat );
static void OEMBTExtAG_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);
static void OEMBTExtAG_AudioChanged( uint32 audio_type );
static OEMBTExtAGobj_t* OEMBTExtAG_CheckParentPtr( IBTExtAG* pParent );

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtAGobj_t*  gpOEMBTExtAGobj = NULL;

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtAG_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance of
   an IBTExtAG.
   There can be only one AG interface.

Parameters:
   IBTExtAG* pParent: Pointer to the parent IBTExtAG structure.

Return Value:  SUCCESS or a BREW error code.

Componts:      None

Side Effects:  Allocates APP memory for 

===========================================================================*/
int OEMBTExtAG_Init( IBTExtAG* pParent )
{
  OEMBTExtAGobj_t*   pNew;
  bt_cmd_status_type stat;

  if ( gpOEMBTExtAGobj != NULL )
  {
    // all ready created the singleton object
    return EFAILED;
  }

  // allocate the object
  pNew = MALLOCREC( OEMBTExtAGobj_t );
  if ( pNew == NULL )
  {
    return ENOMEMORY;
  }
  
  if ( AEEHandle_To( &gOEMBTExtHandleList, (OEMINSTANCE*)pNew,
                     &pParent->m_hBT ) == FALSE )
  {
    FREE( pNew );
    return EFAILED;
  }

  // init BT layer
  pNew->m_appId           = BT_APP_ID_NULL;
  pNew->m_pNotifier       = NULL;
  pNew->m_pac             = NULL;
  pNew->m_pCb             = NULL;
  pNew->m_bDisablingExtIO = FALSE;
  pNew->m_pDSMItem        = NULL;
  pNew->m_pRxWm           = NULL;
  pNew->m_pTxWm           = NULL;
  (void) q_init( &pNew->m_rxQ );
  (void) q_init( &pNew->m_txQ );

  pNew->m_appId = bt_cmd_ec_get_application_id();
  if ( pNew->m_appId == BT_APP_ID_NULL )
  {
    FREE( pNew );
    return EFAILED;
  }
  stat = bt_cmd_ec_reg_event_set_any_app_id( pNew->m_appId,
                                             OEMBTExtAG_EventCallback,
                                             BT_EC_ES_AUDIO_GATEWAY,
                                             BT_EV_NULL,
                                             BT_EV_NULL );
  if ( stat == BT_CS_GN_SUCCESS )
  {
    stat = bt_cmd_ec_register_event_set( pNew->m_appId,
                                         OEMBTExtAG_EventCallback,
                                         BT_EC_ES_CUSTOM,
                                         BT_EV_GN_CMD_DONE, 
                                         BT_EV_GN_CMD_DONE );
  }
  if ( stat != BT_CS_GN_SUCCESS )
  {
    OEMBTExtAG_Release( pParent );
    return EFAILED;
  }

  if ( ISHELL_CreateInstance( pParent->m_pIShell, AEECLSID_BLUETOOTH_NOTIFIER, 
                              (void**) &pNew->m_pNotifier ) != SUCCESS )
  {
    OEMBTExtAG_Release( pParent );
    return EFAILED;
  }

  pNew->m_pac     = AEE_GetAppContext();
  gpOEMBTExtAGobj = pNew;

#ifdef FEATURE_AVS_BT_SCO_REWORK
  // register with AVS
  snd_set_bt_cb_func( OEMBTExtAG_AudioChanged );
#endif
  return SUCCESS;
}


// IBase (see AEE.h)
int OEMBTExtAG_Release( IBTExtAG* pParent )
{
  OEMBTExtAGobj_t* pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  gpOEMBTExtAGobj = NULL;
  pMe->m_pac      = NULL;

  if ( pMe->m_appId != BT_APP_ID_NULL )
  {
    (void) bt_cmd_ec_unregister_event_set( pMe->m_appId,
                                           BT_EC_ES_CUSTOM,
                                           BT_EV_GN_CMD_DONE,
                                           BT_EV_GN_CMD_DONE );

    (void) bt_cmd_ec_unregister_event_set( pMe->m_appId,
                                           BT_EC_ES_AUDIO_GATEWAY,
                                           BT_EV_NULL,
                                           BT_EV_NULL );

    (void)bt_cmd_ec_free_application_id( pMe->m_appId );
    pMe->m_appId = BT_APP_ID_NULL;
  }

  if ( pMe->m_pNotifier != NULL )
  {
    IBTEXTNOTIFIER_Release( pMe->m_pNotifier );
    pMe->m_pNotifier = NULL;
  }

  FREE(pMe);
  (void) AEEHandle_Clear( &gOEMBTExtHandleList, pParent->m_hBT );

  return SUCCESS;
}

// IBTExtAG_Config (see AEEBTExtAG.h)
int OEMBTExtAG_Config(
  IBTExtAG*       pParent, 
  AEEBTIdleMode   mode, 
  uint16          wIdleTimeout
)
{
  bt_ag_idle_mode_type idleMode;
  bt_cmd_status_type   stat;
  int                  result;
  OEMBTExtAGobj_t*     pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  switch( mode ) 
  {
    case AEEBT_AG_IDLEMODE_PASSIVE:
      idleMode = BT_AGIM_AUTOMATIC;
      break;
    case AEEBT_AG_IDLEMODE_ACTIVE:
      idleMode = BT_AGIM_LOW_POWER;
      break;
    default:
      return EBADPARM;
  }

  stat = bt_cmd_ag_config(pMe->m_appId, idleMode, wIdleTimeout);
  result = OEMBTExtAG_CheckCmdStatus( stat );

  return result;
}

// IBTExtAG_Enable (see AEEBTExtAG.h)
int OEMBTExtAG_Enable(
  IBTExtAG*           pParent, 
  const AEEBTBDAddr*  pAddr, 
  AEEBTAudioDevice    dev
)
{
  bt_bd_addr_type      addr;
  bt_audio_device_type audioDevice;
  bt_cmd_status_type   stat;
  int                  result;
  OEMBTExtAGobj_t*     pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  switch( dev )
  {
    case AEEBT_AG_AUDIO_DEVICE_HEADSET:
      audioDevice = BT_AD_HEADSET;
      break;
    case AEEBT_AG_AUDIO_DEVICE_HANDSFREE:
      audioDevice = BT_AD_HANDSFREE;
      break;
    default:
      return EBADPARM;
  }

  if ( pAddr == NULL )
  {
    uint8 i = 0;
    while ( i < sizeof( bt_bd_addr_type ))
    {
      addr.bd_addr_bytes[i++] = 0;
    }
  }
  else
  {
    addr = *((bt_bd_addr_type*)pAddr);
  }

  stat = bt_cmd_ag_enable(pMe->m_appId, &addr, audioDevice);
  result = OEMBTExtAG_CheckCmdStatus( stat );

  return result;
}

// IBTExtAG_Connect (see AEEBTExtAG.h)
int OEMBTExtAG_Connect(
  IBTExtAG*           pParent, 
  const AEEBTBDAddr*  pAddr, 
  AEEBTAudioDevice    dev
)
{
  bt_audio_device_type audioDevice;
  bt_cmd_status_type   stat;
  int                  result;
  OEMBTExtAGobj_t*     pMe = OEMBTExtAG_CheckParentPtr( pParent );

  DBGPRINTF("***zzg OEMBTExtAG_Connect***");

  if ( pMe == NULL )
  {
    return EBADPARM;
  }
  
  if ( pAddr == NULL )
  {
    return EBADPARM;
  }

  switch( dev )
  {
    case AEEBT_AG_AUDIO_DEVICE_HEADSET:
      audioDevice = BT_AD_HEADSET;
      break;
    case AEEBT_AG_AUDIO_DEVICE_HANDSFREE:
      audioDevice = BT_AD_HANDSFREE;
      break;
    default:
      return EBADPARM;
  }

  stat = bt_cmd_ag_connect( pMe->m_appId, (bt_bd_addr_type*)pAddr, 
                            audioDevice );
  result = OEMBTExtAG_CheckCmdStatus( stat );

  return result;
}

// IBTExtAG_Ring (see AEEBTExtAG.h)
int OEMBTExtAG_Ring( IBTExtAG* pParent, uint16 uMaxRings, uint16 uPeriod )
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_ag_ring_audio_device(pMe->m_appId, uMaxRings, uPeriod);
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTExtAG_SetVRCapable (see AEEBTExtAG.h)
int OEMBTExtAG_SetVRCapable( 
  IBTExtAG*     pParent, 
  boolean       vr_capable
)
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_ag_set_vr_capable( pMe->m_appId, vr_capable );
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTEXTAG_UpdateVRState (see AEEBTExtAG.h)
int OEMBTExtAG_UpdateVRState(
  IBTExtAG* pParent,
  boolean   bVROn
)
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }
  
  stat = bt_cmd_ag_update_vr_state( pMe->m_appId, bVROn );
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTExtAG_SetSpkrGain (see AEEBTExtAG.h)
int OEMBTExtAG_SetSpkrGain( IBTExtAG* pParent, uint8 nGain )
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }
  
  stat = bt_cmd_ag_set_ad_spkr_gain(pMe->m_appId, nGain);
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTExtAG_SetMicGain (see AEEBTExtAG.h)
int OEMBTExtAG_SetMicGain( IBTExtAG* pParent, uint8 nGain )
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }
  
  stat = bt_cmd_ag_set_ad_mic_gain(pMe->m_appId, nGain);
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTExtAG_ConnectAudio (see AEEBTExtAG.h)
int OEMBTExtAG_ConnectAudio( IBTExtAG* pParent )
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }
  
  stat = bt_cmd_ag_audio_connect( pMe->m_appId, 0 );
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTExtAG_DisconnectAudio (see AEEBTExtAG.h)
int OEMBTExtAG_DisconnectAudio( IBTExtAG* pParent )
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_ag_audio_disconnect( pMe->m_appId );
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTExtAG_Disconnect (see AEEBTExtAG.h)
int OEMBTExtAG_Disconnect( IBTExtAG* pParent )
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }
  
  stat = bt_cmd_ag_disconnect( pMe->m_appId );
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTExtAG_Disable (see AEEBTExtAG.h)
int OEMBTExtAG_Disable( IBTExtAG* pParent )
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_ag_disable(pMe->m_appId);
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTEXTAG_GetDialedString (see AEEBTExtAG.h)
int OEMBTExtAG_GetDialedString( 
  IBTExtAG* pParent, 
  AECHAR*   pString, 
  uint8*    pNumChars,
  boolean   bIsMemLocation
)
{
  int              result = SUCCESS;
  uint8            len;
  AECHAR*          pSrc;
  OEMBTExtAGobj_t* pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  pSrc = bIsMemLocation ? pMe->m_memToDial : pMe->m_numToDial;
  if ( (pString == NULL) || (pNumChars == NULL) ||
       ! AEE_CHECK_PTR( "OEMBTExtAG_GetDialedString", pString, 
                        sizeof(AECHAR)* (*pNumChars), TRUE ) ||
       ! AEE_CHECK_PTR( "OEMBTExtAG_GetDialedString", pNumChars, 
                        sizeof(uint8), TRUE ) )
  {
    return EBADPARM;
  }

  len = WSTRLEN( pSrc );
  if ( len >= *pNumChars )
  {
    result = EBADPARM;
  }
  else
  {
    WSTRLCPY( pString, pSrc, *pNumChars );
    *pNumChars = len;    
  }

  return result;
}


// IBTEXTAG_EnableExternalIO (see AEEBTExtAG.h)
int OEMBTExtAG_EnableExternalIO( IBTExtAG* pParent, AEECallback* pCb )
{
  bt_cmd_status_type stat;
  int                result = SUCCESS;
  OEMBTExtAGobj_t*   pMe    = OEMBTExtAG_CheckParentPtr( pParent );

  if ( (pMe == NULL) || (pCb == NULL) || 
       (pCb->pfnNotify == NULL) || (pCb->pNotifyData == NULL) )
  {
    return EBADPARM;
  }

  if ( pMe->m_pCb != NULL )
  {
    return EBADSTATE;
  }

  pMe->m_pRxWm = MALLOCREC( dsm_watermark_type );
  pMe->m_pTxWm = MALLOCREC( dsm_watermark_type );
  pMe->m_pCb   = MALLOCREC( AEECallback );

  if ( (pMe->m_pRxWm == NULL) || (pMe->m_pTxWm == NULL) || 
       (pMe->m_pCb == NULL) )
  {
    FREEIF( pMe->m_pRxWm );
    FREEIF( pMe->m_pTxWm );
    FREEIF( pMe->m_pCb );
    return ENOMEMORY;
  }

  *pMe->m_pCb = *pCb;

  /* set up RX watermark */
  dsm_queue_init( pMe->m_pRxWm, 1000, &pMe->m_rxQ );
  pMe->m_pRxWm->each_enqueue_func_ptr = OEMBTExtAG_ATCmdRxCb;

  /*  Set up TX watermark.  */
  dsm_queue_init( pMe->m_pTxWm, 1000, &pMe->m_txQ );

  stat = bt_cmd_ag_external_io_config( pMe->m_appId, TRUE, pMe->m_pRxWm,
                                       pMe->m_pTxWm, BT_AG_IC_NONE_M );
  result = OEMBTExtAG_CheckCmdStatus( stat );
  if ( result != SUCCESS )
  {
    FREEIF( pMe->m_pRxWm );
    FREEIF( pMe->m_pTxWm );
    FREEIF( pMe->m_pCb );
  }
  return result;
}

// IBTEXTAG_DisableExternalIO (see AEEBTExtAG.h)
int OEMBTExtAG_DisableExternalIO( IBTExtAG* pParent )
{
  bt_cmd_status_type stat;
  uint8              nCmdStatus;
  OEMBTExtAGobj_t*   pMe    = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  if ( pMe->m_pCb == NULL )
  {
    return EBADSTATE;
  }

  /* Set the flag here in case BT_EV_GN_CMD_DONE will be received before
  *  the invocation of bt_cmd_ag_external_io_config() below returns, so that
  *  BT_EV_GN_CMD_DONE can be properly processed in OEMBTExtAG_ProcessCmdDone().
  */
  pMe->m_bDisablingExtIO = TRUE;

  stat = bt_cmd_ag_external_io_config( pMe->m_appId, FALSE, NULL, NULL, 
                                       BT_AG_IC_NONE_M );
  nCmdStatus = OEMBTExtAG_CheckCmdStatus( stat );

  if ( SUCCESS != nCmdStatus )
  {
    pMe->m_bDisablingExtIO = FALSE;
  }

  return nCmdStatus;
}

// IBTEXTAG_RetrieveCommand (see AEEBTExtAG.h)
int OEMBTExtAG_RetrieveCommand( 
  IBTExtAG* pParent, 
  uint8*    pszCommand,
  uint8*    puNumBytes
)
{
  OEMBTExtAGobj_t* pMe = OEMBTExtAG_CheckParentPtr( pParent );
  uint8 bytesToRead    = 0;
  uint8 bytesRead      = 0;

  if ( (pMe == NULL) || (pszCommand == NULL) || (puNumBytes == NULL) ||
       (*puNumBytes == 0) || !AEE_CHECK_PTR( "OEMBTExtAG_RetrieveCommand", 
                                             pszCommand, *puNumBytes, TRUE) )
  {
    return EBADPARM;
  }

  if ( pMe->m_pCb == NULL )
  {
    return EBADSTATE;
  }

  bytesToRead = *puNumBytes;
  *puNumBytes = 0;

  while ( TRUE )
  {
    if ( pMe->m_pDSMItem != NULL )
    {
      bytesRead = dsm_pullup( &pMe->m_pDSMItem, (pszCommand + *puNumBytes), 
                              (bytesToRead - *puNumBytes) );
      if ( bytesRead == 0 )
      {
        MSG_ERROR( "OEMBTExtAG_RetrieveCommand - dsm_pullup() returned 0", 
                   0, 0, 0 );
      }

      *puNumBytes += bytesRead;

      if ( *puNumBytes >= bytesToRead )  // destination full?
      {
        break;
      }
      /* dsm_pullup will free the dsm_item after pulling off the last byte */
    }
    if ( pMe->m_pDSMItem == NULL )  // DSM item has been freed?
    {
      pMe->m_pDSMItem = dsm_dequeue( pMe->m_pRxWm ); // get next one
      if ( pMe->m_pDSMItem == NULL )  // rx queue is empty?
      {
        break;
      }
    }
  }
  return SUCCESS;
}


// IBTEXTAG_SendResponse (see AEEBTExtAG.h)
int OEMBTExtAG_SendResponse( 
  IBTExtAG*    pParent, 
  const uint8* pszResponse,
  uint8        uNumBytes
)
{
  OEMBTExtAGobj_t* pMe    = OEMBTExtAG_CheckParentPtr( pParent );
  dsm_item_type*   dsm_item_ptr;
  uint8            bytesWritten = 0;

  if ( (pMe == NULL) || (pszResponse == NULL) || (uNumBytes == 0) || 
       !AEE_CHECK_PTR( "OEMBTExtAG_SendResponse", 
                       pszResponse, uNumBytes, FALSE) )
  {
    return EBADPARM;
  }

  if ( pMe->m_pCb == NULL )
  {
    return EBADSTATE;
  }

  dsm_item_ptr = bt_get_free_dsm_ptr( BT_TL_RFCOMM, uNumBytes );
  if ( dsm_item_ptr != NULL )
  {
    bytesWritten = 
      dsm_pushdown_tail( &dsm_item_ptr, pszResponse, uNumBytes, 
                         (dsm_mempool_id_enum_type)dsm_item_ptr->pool_id );
  }
  if ( bytesWritten < uNumBytes )
  {
    return ENOMEMORY;
  }
  else
  {
    dsm_enqueue( pMe->m_pTxWm, &dsm_item_ptr );

    return SUCCESS;
  }
}

// IBTExtAG_SetInbandRingCapable (see AEEBTExtAG.h)
int OEMBTExtAG_SetInbandRingCapable( IBTExtAG* pParent, boolean bCapable )
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_ag_set_inband_ring( pMe->m_appId, bCapable );
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTExtAG_SetBatteryChargeIndicator (see AEEBTExtAG.h)
int OEMBTExtAG_SetBatteryChargeIndicator( 
  IBTExtAG*               pParent, 
  AEEBTBatteryChargeLevel currentLevel )
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_ag_set_battchg_ind( pMe->m_appId, 
                                    (bt_ag_battchg_level_type) currentLevel );
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

// IBTExtAG_SetRSSI (see AEEBTExtAG.h)
int OEMBTExtAG_SetRSSI( IBTExtAG* pParent, AEEBTRSSI currentLevel )
{
  bt_cmd_status_type stat;
  OEMBTExtAGobj_t*   pMe = OEMBTExtAG_CheckParentPtr( pParent );

  if ( pMe == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_ag_set_rssi( pMe->m_appId, 
                             (bt_ag_rssi_level_type) currentLevel );
  return( OEMBTExtAG_CheckCmdStatus( stat ) );
}

//==========================================================================
//   static helper functions
//==========================================================================
static int OEMBTExtAG_CheckCmdStatus( bt_cmd_status_type stat )
{
  switch (stat)
  {
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return SUCCESS;
    case BT_CS_GN_CMD_Q_FULL:
      return ENOMEMORY;
    default:
      return EFAILED;
  }
}

static void OEMBTExtAG_ATCmdRxCb( void )
{
  OEMBTExtAGobj_t* pMe = gpOEMBTExtAGobj;

  if ( (pMe != NULL) && (pMe->m_pCb != NULL) && (pMe->m_pRxWm != NULL) )
  {
    AEE_APP_RESUME( pMe->m_pCb, pMe->m_pac );
  }
}

static OEMBTExtAGobj_t* OEMBTExtAG_CheckParentPtr( IBTExtAG* pParent )
{
  OEMBTExtAGobj_t *rv;

  if( (AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&rv ) != TRUE) ||
      (rv != gpOEMBTExtAGobj) )
  {
    rv = NULL;
  }
  
  return rv;
}

static boolean OEMBTExtAG_ConvertCmdDone( 
  bt_cmd_status_type cmd_status,
  AEEEvent           evt,
  uint8              default_stat,
  AEEBTNotification* pN
)
{
  pN->uID = evt;

  switch (cmd_status)
  {
    case BT_CS_GN_SUCCESS:
      pN->data.uError = AEEBT_AG_ERR_NONE;
      return FALSE;          // wait for other event
    case BT_CS_AG_ALREADY_ENABLED:
      pN->data.uError = AEEBT_AG_ERR_ALREADY_ENABLED;
      break;
    case BT_CS_AG_INVALID_PARAMETERS:
      pN->data.uError = AEEBT_AG_ERR_BAD_PARAMS;
      break;
    case BT_CS_AG_NO_SIO_STREAM:
      pN->data.uError = AEEBT_AG_ERR_NO_SIO_STREAM;
      break;
    case BT_CS_AG_NOT_ENABLED:
      pN->data.uError = AEEBT_AG_ERR_NOT_ENABLED;
      break;
    case BT_CS_AG_NO_AUDIO:
      pN->data.uError = AEEBT_AG_ERR_NO_AUDIO;
      break;
    default:
      pN->data.uError = default_stat;
      break;
  }
  return TRUE;
}


static boolean OEMBTExtAG_ProcessCmdDone( 
  bt_ev_gn_cmd_done_type* pm,
  AEEBTNotification*      pN
)
{
  OEMBTExtAGobj_t* pMe = gpOEMBTExtAGobj;

  switch (pm->cmd_type)
  {
    case BT_CMD_AG_CONFIG:
    {
      // status is always SUCCESS; no other event expected;
      pN->uID   = AEEBT_AG_EVT_CONFIGURED;
      break;
    }
    case BT_CMD_AG_ENABLE:
    {
      if ( ! OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                        AEEBT_AG_EVT_ENABLED,
                                        AEEBT_AG_ERR_ENABLE_FAILED, 
                                        pN ) )
      {
        return FALSE;
      }
      break;
    }
    case BT_CMD_AG_CONNECT:
    {
      if ( ! OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                        AEEBT_AG_EVT_CONNECTED,
                                        AEEBT_AG_ERR_CONNECT_FAILED, 
                                        pN ) )
      {
        return FALSE;
      }
      break;
    }
    case BT_CMD_AG_RING_AUDIO_DEVICE:
    {
      // if success, no other event expected
      OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                 AEEBT_AG_EVT_RUNG,
                                 AEEBT_AG_ERR_RING_FAILED, 
                                 pN );
      break;
    }
    case BT_CMD_AG_SET_VR_CAPABLE:
    {
      // status is always SUCCESS; no other event expected;
      pN->uID   = AEEBT_AG_EVT_VR_CAPABLE_SET;
      break;
    }
    case BT_CMD_AG_SET_AD_SPKR_GAIN:
    {
      // no other event expected
      OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                 AEEBT_AG_EVT_SPKR_GAIN_SET,
                                 AEEBT_AG_ERR_SET_SPKR_GAIN_FAILED, 
                                 pN );
      break;
    }
    case BT_CMD_AG_SET_AD_MIC_GAIN:
    {
      // no other event expected
      OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                 AEEBT_AG_EVT_MIC_GAIN_SET,
                                 AEEBT_AG_ERR_SET_MIC_GAIN_FAILED, 
                                 pN );
      break;
    }
    case BT_CMD_AG_AUDIO_CONNECT:
    {
      if ( ! OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                        AEEBT_AG_EVT_AUDIO_CONNECTED,
                                        AEEBT_AG_ERR_CONNECT_AUDIO_FAILED, 
                                        pN ) )
      {
        return FALSE;
      }
      else
      {
        pN->uID = AEEBT_AG_EVT_AUDIO_CONNECT_FAILED;
      }
      break;
    }
    case BT_CMD_AG_AUDIO_DISCONNECT:
    {
      // status is always SUCCESS; AUDIO DISCONNECTED event expected;
      return FALSE;
    }
    case BT_CMD_AG_DISCONNECT:
    {
      if ( ! OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                        AEEBT_AG_EVT_DISCONNECTED,
                                        AEEBT_AG_ERR_DISCONNECT_FAILED, 
                                        pN ) )
      {
        return FALSE;
      }
      break;
    }
    case BT_CMD_AG_DISABLE:
    {
      if ( ! OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                        AEEBT_AG_EVT_DISABLED,
                                        AEEBT_AG_ERR_DISABLE_FAILED, 
                                        pN ) )
      {
        return FALSE;
      }
      break;
    }
    case BT_CMD_AG_UPDATE_VR_STATE:
    {
      // status is always SUCCESS; no other event expected;
      pN->uID   = AEEBT_AG_EVT_VR_STATE_UPDATED;
      break;
    }
    case BT_CMD_AG_EXT_IO_CONFIG:
    {
      // no other event expected
      OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                 pMe->m_bDisablingExtIO ?
                                   AEEBT_AG_EVT_EXTERNAL_IO_DISABLED :
                                   AEEBT_AG_EVT_EXTERNAL_IO_ENABLED,
                                 AEEBT_AG_ERR_FAILED, pN );

      pMe->m_bDisablingExtIO = FALSE;

      if ( ((pN->uID == AEEBT_AG_EVT_EXTERNAL_IO_DISABLED) && 
            (pN->data.uError == AEEBT_AG_ERR_NONE)) ||
           ((pN->uID == AEEBT_AG_EVT_EXTERNAL_IO_ENABLED) && 
            (pN->data.uError != AEEBT_AG_ERR_NONE)) )
      {
        if ( pMe->m_pRxWm != NULL )
        { 
           dsm_empty_queue( pMe->m_pRxWm );
           FREEIF( pMe->m_pRxWm );
        }

        if ( pMe->m_pTxWm != NULL )
        { 
           dsm_empty_queue( pMe->m_pTxWm );
           FREEIF( pMe->m_pTxWm );
        }
        
        FREEIF( pMe->m_pCb );
      }
      break;
    }
    case BT_CMD_AG_SET_INBAND_RING:
    {
      // status is always SUCCESS; no other event expected;
      pN->uID   = AEEBT_AG_EVT_INBAND_RING_CAPABLE_SET;
      break;
    }
    case BT_CMD_AG_SET_BATTCHG_IND:
    {
      // no other event expected
      OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                 AEEBT_AG_EVT_BATTERY_CHARGE_IND_SET,
                                 AEEBT_AG_ERR_BAD_PARAMS, 
                                 pN );
      break;
    }
    case BT_CMD_AG_SET_RSSI:
    {
      // no other event expected
      OEMBTExtAG_ConvertCmdDone( pm->cmd_status, 
                                 AEEBT_AG_EVT_RSSI_SET,
                                 AEEBT_AG_ERR_BAD_PARAMS, 
                                 pN );
      break;
    }
    default:
    {
      MSG_ERROR( "AGEvCb - unexpect Cmd Done %x", pm->cmd_type, 0, 0 );
      return FALSE;
    }
  }
  
  return TRUE;
}


// called from BT task context; so just queue the event
static void OEMBTExtAG_EventCallback( bt_ev_msg_type* bt_ev_msg_ptr )
{
  OEMBTExtAGobj_t*    pMe = gpOEMBTExtAGobj;
  AEEBTNotification*  pN  = NULL;
  
  if ( pMe == NULL )
  {
    return; // the object no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
  if( pN == NULL )
  {
    return;
  }

  pN->data.uError  = AEEBT_AG_ERR_NONE;

  switch (bt_ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
    {
      if ( ! OEMBTExtAG_ProcessCmdDone( &bt_ev_msg_ptr->ev_msg.ev_gn_cmd_done, 
                                        pN ) )
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_AG_ENABLED:
    {
      pN->uID = AEEBT_AG_EVT_ENABLED;
      break;
    }
    case BT_EV_AG_CONNECTED:
    {
      pN->uID         = AEEBT_AG_EVT_CONNECTED;
      pN->data.bdAddr = 
        *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_ag_connected.bd_addr);
      break;
    }
    case BT_EV_AG_CONNECTION_FAILED:
    {
      pN->uID         = AEEBT_AG_EVT_CONNECT_FAILED;
      pN->data.bdAddr =
        *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_ag_connection_failed.bd_addr);
      break;
    }
    case BT_EV_AG_DISCONNECTED:
    {
      pN->data.bdAddr = 
        *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_ag_disconnected.bd_addr);
      pN->uID   = AEEBT_AG_EVT_DISCONNECTED;
      break;
    }
    case BT_EV_AG_RING_FAILED:
    {
      pN->uID         = AEEBT_AG_EVT_RUNG;
      pN->data.uError = AEEBT_AG_ERR_RING_FAILED;
      break;
    }
    case BT_EV_AG_BUTTON_PRESSED:
    {
      pN->uID   = AEEBT_AG_EVT_BUTTON_PRESSED;
      break;
    }
    case BT_EV_AG_SPKR_GAIN_REPORT:
    {
      pN->uID               = AEEBT_AG_EVT_SPKR_GAIN_REPORT;
      pN->data.uVolumeGain  = 
        bt_ev_msg_ptr->ev_msg.ev_ag_ad_spkr_gain_report.ad_speaker_gain;
      break;
    }
    case BT_EV_AG_MIC_GAIN_REPORT:
    {
      pN->uID               = AEEBT_AG_EVT_MIC_GAIN_REPORT;
      pN->data.uVolumeGain  =
        bt_ev_msg_ptr->ev_msg.ev_ag_ad_mic_gain_report.ad_microphone_gain;
      break;
    }
    case BT_EV_AG_AUDIO_CONNECT_FAILED:
    {
      pN->uID         = AEEBT_AG_EVT_AUDIO_CONNECT_FAILED;
      if ( bt_ev_msg_ptr->ev_msg.ev_ag_audio_connect_failed.event_reason ==
           BT_EVR_AG_CONN_FAILED)
      {
        pN->data.uError = AEEBT_AG_ERR_CONNECT_FAILED;
      }
      else
      {
        pN->data.uError = AEEBT_AG_ERR_ENABLE_FAILED;
      }
      break;
    }
    case BT_EV_AG_AUDIO_CONNECTED:
    {
      pN->uID   = AEEBT_AG_EVT_AUDIO_CONNECTED;
      break;
    }
    case BT_EV_AG_AUDIO_DISCONNECTED:
    {
      bt_ev_ag_audio_disconnected_type* pEvt = 
        &bt_ev_msg_ptr->ev_msg.ev_ag_audio_disconnected;

      pN->uID   = AEEBT_AG_EVT_AUDIO_DISCONNECTED;
      switch ( (bt_error_code_type) pEvt->event_reason )
      {
        case BT_BE_LOCAL_HOST_TERMINATED_CONNECTION:
          pN->data.reason = AEEBT_REASON_LOCAL_HOST_TERMINATED_CONNECTION;
          break;
        case BT_BE_USER_ENDED_CONNECTION:
          pN->data.reason = AEEBT_REASON_USER_ENDED_CONNECTION;
          break;
        default:
          MSG_ERROR( "AGEvCb - AUDIO_DISC r=%x", pEvt->event_reason, 0, 0 );
          pN->data.reason = AEEBT_REASON_USER_ENDED_CONNECTION;
      }
      break;
    }
    case BT_EV_AG_DISABLED:
    {
      pN->uID   = AEEBT_AG_EVT_DISABLED;
      break;
    }
    case BT_EV_AG_SLC_CHANGED:
    {
      if ( bt_ev_msg_ptr->ev_msg.ev_ag_slc_changed.slc_up )
      {
        pN->uID = AEEBT_AG_EVT_SLC_UP;
      }
      else
      {
        pN->uID = AEEBT_AG_EVT_SLC_DOWN;
      }
      break;
    }
    case BT_EV_AG_DEV_PICKUP:
    {
      pN->uID   = AEEBT_AG_EVT_DEV_PICKUP;
      break;
    }
    case BT_EV_AG_DEV_HANGUP:
    {
      pN->uID   = AEEBT_AG_EVT_DEV_HANGUP;
      break;
    }
    case BT_EV_AG_VR_TOGGLE:
    {
      if ( bt_ev_msg_ptr->ev_msg.ev_ag_vr_state.vr_enabled )
      {
        pN->uID = AEEBT_AG_EVT_VR_ON;
      }
      else
      {
        pN->uID = AEEBT_AG_EVT_VR_OFF;
      }
      break;
    }
    case BT_EV_AG_DEV_DIAL:
    {
      uint8 i, temp_buf[ AEEBT_MAX_PHONE_NUM_DIGITS+1 ];
      for ( i=0; i<AEEBT_MAX_PHONE_NUM_DIGITS; i++)
      {
        temp_buf[ i ] = bt_ev_msg_ptr->ev_msg.ev_ag_dev_dial.phone_num[ i ];
        if ( temp_buf[ i ] == 0 )
        {
          break;
        }
      }
      temp_buf[ i ] = 0; // make sure string is null terminated
      AEEBT_TO_WSTR( (char*) temp_buf, 
                     pMe->m_numToDial, sizeof(pMe->m_numToDial) );
      pN->uID    = AEEBT_AG_EVT_DEV_DIAL;
      break;
    }
    case BT_EV_AG_DEV_MEM_DIAL:
    {
      uint8 i, temp_buf[ AEEBT_MAX_MEM_DIAL_DIGITS+1 ];
      for ( i=0; i<=AEEBT_MAX_MEM_DIAL_DIGITS; i++)
      {
        temp_buf[ i ] = bt_ev_msg_ptr->ev_msg.ev_ag_dev_mem_dial.mem_entry[ i ];
      }
      AEEBT_TO_WSTR( (char*) temp_buf, 
                     pMe->m_memToDial, sizeof(pMe->m_memToDial) );
      pN->uID   = AEEBT_AG_EVT_DEV_MEM_DIAL;
      break;
    }
    case BT_EV_AG_DEV_REDIAL:
    {
      pN->uID   = AEEBT_AG_EVT_DEV_REDIAL;
      break;
    }
    case BT_EV_AG_DEV_SEND_DTMF:
    {
      pN->uID             = AEEBT_AG_EVT_DEV_SEND_DTMF;
      pN->data.uDTMFChar  = bt_ev_msg_ptr->ev_msg.ev_ag_dev_send_dtmf.dtmf_char;
      break;
    }
    default:
    {
      MSG_ERROR( "AGEvCb - unexpect event %x", 
                 bt_ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_AG );
  IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
}

// called by AVS
static void OEMBTExtAG_AudioChanged( uint32 audio_type )
{
  OEMBTExtAGobj_t*    pMe = gpOEMBTExtAGobj;
  AEEBTNotification*  pN  = NULL;

  if ( pMe == NULL )
  {
    return; // the object no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
  if( pN == NULL )
  {
    return;
  }

  pN->uID = AEEBT_AG_EVT_SOUND_CHANGING;

  switch ( audio_type )
  {
#ifdef FEATURE_AVS_BT_SCO_REWORK
    case VOC_PLAYBACK_STOP:       /* no audio                   */
      pN->data.sSoundType = AEEBT_SND_NONE;
      break;
    case VOC_PLAYBACK_SCO:        /* BT SCO supported audio     */
      pN->data.sSoundType = AEEBT_SND_BT_SCO;
      break;
    case VOC_PLAYBACK_A2DP:       /* BT non-SCO supported audio */
      pN->data.sSoundType = AEEBT_SND_BT_OTHER;
      break;
    case VOC_PLAYBACK_OTHER:      /* non BT support audio       */
      pN->data.sSoundType = AEEBT_SND_OTHER;
      break;
    case VOC_PLAYBACK_OTHER_STOP: /* non BT support audio       */
      pN->data.sSoundType = AEEBT_SND_OTHER_STOP;
      break;      
    case (VOC_PLAYBACK_SCO | VOC_PLAYBACK_A2DP):
      pN->data.sSoundType = (AEEBT_SND_BT_SCO | AEEBT_SND_BT_OTHER);
      break;
    case VOC_PLAYBACK_DTMF_START: /* playing DTMF               */
      pN->data.sSoundType = AEEBT_SND_DTMF_START;
      break;
    case VOC_PLAYBACK_DTMF_STOP:  /* stop playing DTMF          */
      pN->data.sSoundType = AEEBT_SND_DTMF_STOP;
      break;
    case VOC_PLAYBACK_DTMF_RINGER_START: /* start playing DTMF ring */
      pN->data.sSoundType = AEEBT_SND_DTMF_RING_START;
      break;
    case VOC_PLAYBACK_DTMF_RINGER_STOP: /* stop playing DTMF ring */
      pN->data.sSoundType = AEEBT_SND_DTMF_RING_STOP;
      break;
#endif /* FEATURE_AVS_BT_SCO_REWORK */
    default:
      MSG_ERROR( "AGAuChg - unexpect evt=0x%X, appevt=0x%x", audio_type, 
                 pN->data.sSoundType, 0 );
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_AG );
  IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
}

#endif // defined(FEATURE_IBT)

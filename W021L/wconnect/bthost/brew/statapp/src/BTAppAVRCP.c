/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppAVRCP.c                               */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW AVRCP Application.             */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains AVRCP test feature.      */  
/*                                                                           */  
/*              Copyright (c) 2005-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppAVRCP.c#2 $
$DateTime: 2009/06/17 10:41:55 $
$Author: nksingh $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
06/11/09    ns   changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
11/21/08   ssk   Prevent multiple disconnect calls from AVRCP APP to BT driver.
                 This fixes issues in back-to-back calls to  BTApp_DisableBT().
11/21/08   ssk   Handle A2DP\AVRCP disconnect evts after switching brew context
04/10/08   VM    Added BTApp_EnableAVRCP()
03/06/08   VM    Removed redundant IBTEXTRM_SetSecBySvcCls call
11/01/07   ag    Merged Lisbon(BT2.1) code to the mainline
02/19/07   jj    Fix has been added to build AVRCP menu only if top menu is 
                 AVRCP_TEST_MENU , which was effecting automation.
11/01/06   dsn   Fixed typecasting errors with A2DP and automation.
01/23/06   ns    Added diagnostic support to query information about the 
                 operations performed by HS/HF
12/26/05   ns    Added support to set passkey while enabling AVRCP in automation
                 Device is set to discoverable mode when AVRCP is enabled
12/02/05   sn    Externalized BTApp_AVRCPInit() and BTApp_AVRCPCleanup().
10/18/05   dc    Removed the Disable menu when AVRCP is connected.
                 Added function for AVRCP automation.
                 Fixed a bug in BTApp_HandleAVRCPMainMenu where ev_processed
                 was not being set to TRUE when the IBT call is success for 
                 connect and disconnect cases.
10/18/05   sn    Added ShowDevMsg for AVRCP connection failure case.
09/15/05   ns    Build test menu when a device is selected for use for visible
                 feedback to the user
04/29/05   jtl   Initial Release. 

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH
#ifdef FEATURE_BT_EXTPF_AV
#include "BTApp.h"
#include "BTAppUtils.h"
#include "BTAppAVRCP.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

extern uint32 uBTApp_NMask;

static boolean btapp_avrcp_init_done = FALSE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
/* ==========================================================================
FUNCTION BTApp_AVRCPHandleDevListMenu
DESCRIPTION
============================================================================= */
void BTApp_AVRCPHandleDevListMenu(
  CBTApp*           pMe
)
{
  pMe->mAVRCP.bdAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
  BTApp_BuildMenu( pMe,  BT_APP_MENU_AVRCP_TEST );  
}

/* ==========================================================================
FUNCTION BTApp_ProcessAVRCPNotifications
DESCRIPTION
============================================================================= */
void BTApp_ProcessAVRCPNotifications( 
  CBTApp*           pMe, 
  AEEEvent          evt, 
  NotificationData* pData
)
{
  MSG_LOW( "AVRCP Notif - evt=%d", evt, 0, 0 );

  if( TOP_MENU == BT_APP_MENU_AVRCP_TEST )
  {
    pMe->bBusyIconUp = FALSE;
  }

  switch ( evt )
  {
    case AEEBT_AVRCP_EVT_CONNECTED:
      pMe->mAVRCP.bConnected = TRUE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      BTApp_BuildMenu( pMe, TOP_MENU );
      break;
    case AEEBT_AVRCP_EVT_CON_FAILED:
      MSG_ERROR( "AVRCP - Connect failed err=%x",
                 pData->uError,0,0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      BTApp_ShowDevMsg( pMe, IDS_MSG_AG_CONNF, &pMe->mAVRCP.bdAddr, 2 );
      break;
    case AEEBT_AVRCP_EVT_DISCONNECTED:
      pMe->mAVRCP.bConnected = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      if( pMe->mAVRCP.bReleasing == TRUE )
      {
        ISHELL_PostEventEx( pMe->a.m_pIShell, EVTFLG_ASYNC,
                            AEECLSID_BLUETOOTH_APP,
                            EVT_USER, EVT_AVRCP_DISCONNECTED, 0L);
      }
      break;
    case AEEBT_AVRCP_EVT_OP_SELECT:
    case AEEBT_AVRCP_EVT_OP_UP:
    case AEEBT_AVRCP_EVT_OP_DOWN:
    case AEEBT_AVRCP_EVT_OP_LEFT:
    case AEEBT_AVRCP_EVT_OP_RIGHT:
    case AEEBT_AVRCP_EVT_OP_RIGHT_UP:
    case AEEBT_AVRCP_EVT_OP_RIGHT_DOWN:
    case AEEBT_AVRCP_EVT_OP_LEFT_UP:
    case AEEBT_AVRCP_EVT_OP_LEFT_DOWN:
    case AEEBT_AVRCP_EVT_OP_ROOT_MENU:
    case AEEBT_AVRCP_EVT_OP_SETUP_MENU:
    case AEEBT_AVRCP_EVT_OP_CONTENTS_MENU:
    case AEEBT_AVRCP_EVT_OP_FAVORITE_MENU:
    case AEEBT_AVRCP_EVT_OP_EXIT:
    case AEEBT_AVRCP_EVT_OP_0:
    case AEEBT_AVRCP_EVT_OP_1:
    case AEEBT_AVRCP_EVT_OP_2:
    case AEEBT_AVRCP_EVT_OP_3:
    case AEEBT_AVRCP_EVT_OP_4:
    case AEEBT_AVRCP_EVT_OP_5:
    case AEEBT_AVRCP_EVT_OP_6:
    case AEEBT_AVRCP_EVT_OP_7:
    case AEEBT_AVRCP_EVT_OP_8:
    case AEEBT_AVRCP_EVT_OP_9:
    case AEEBT_AVRCP_EVT_OP_DOT:
    case AEEBT_AVRCP_EVT_OP_ENTER:
    case AEEBT_AVRCP_EVT_OP_CLEAR:
    case AEEBT_AVRCP_EVT_OP_CHANNEL_UP:
    case AEEBT_AVRCP_EVT_OP_CHANNEL_DOWN:
    case AEEBT_AVRCP_EVT_OP_PREVIOUS_CHANNEL:
    case AEEBT_AVRCP_EVT_OP_SOUND_SELECT:
    case AEEBT_AVRCP_EVT_OP_INPUT_SELECT:
    case AEEBT_AVRCP_EVT_OP_DISPLAY_INFORMATION:
    case AEEBT_AVRCP_EVT_OP_HELP:
    case AEEBT_AVRCP_EVT_OP_PAGE_UP:
    case AEEBT_AVRCP_EVT_OP_PAGE_DOWN:
    case AEEBT_AVRCP_EVT_OP_POWER:
    case AEEBT_AVRCP_EVT_OP_VOLUME_UP:
    case AEEBT_AVRCP_EVT_OP_VOLUME_DOWN:
    case AEEBT_AVRCP_EVT_OP_MUTE:
    case AEEBT_AVRCP_EVT_OP_PLAY:
    case AEEBT_AVRCP_EVT_OP_STOP:
    case AEEBT_AVRCP_EVT_OP_PAUSE:
    case AEEBT_AVRCP_EVT_OP_RECORD:
    case AEEBT_AVRCP_EVT_OP_REWIND:
    case AEEBT_AVRCP_EVT_OP_FAST_FORWARD:
    case AEEBT_AVRCP_EVT_OP_EJECT:
    case AEEBT_AVRCP_EVT_OP_FORWARD:
    case AEEBT_AVRCP_EVT_OP_BACKWARD:
    case AEEBT_AVRCP_EVT_OP_ANGLE:
    case AEEBT_AVRCP_EVT_OP_SUBPICTURE:
    case AEEBT_AVRCP_EVT_OP_F1:
    case AEEBT_AVRCP_EVT_OP_F2:
    case AEEBT_AVRCP_EVT_OP_F3:
    case AEEBT_AVRCP_EVT_OP_F4:
    case AEEBT_AVRCP_EVT_OP_F5:
    case AEEBT_AVRCP_EVT_OP_GENERIC:
      // Add the event to the event list.
      pMe->mAVRCP.evtTable[pMe->mAVRCP.evtEnd] = evt;
      pMe->mAVRCP.evtType[pMe->mAVRCP.evtEnd]  = pData->AvrcpOp.opStatus;
      pMe->mAVRCP.evtEnd++;
      pMe->mAVRCP.evtEnd %= MAX_AVRCP_EVENTS;
      if( pMe->mAVRCP.evtEnd == pMe->mAVRCP.evtStart )
      {
        pMe->mAVRCP.evtStart++;
        pMe->mAVRCP.evtStart %= MAX_AVRCP_EVENTS;
      }
      BTApp_BuildMenu( pMe, TOP_MENU );

      // Respond to the remote device
      if( evt != AEEBT_AVRCP_EVT_OP_GENERIC )
      {
        IBTEXTAVRCP_SendReply( pMe->mAVRCP.po, pData->AvrcpOp.txnId, pData->AvrcpOp.opStatus,
                               evt, AEEBT_AVRCP_RESPONSE_ACCEPTED );
      }
      else
      {
        /* All generic ops are currently not supported */
        uint8 *pBuf;
        pBuf = MALLOC( pData->AvrcpGenericOp.operand_count );

        IBTEXTAVRCP_GetGenericOp( pMe->mAVRCP.po,
                                  pBuf,
                                  pData->AvrcpGenericOp.operand_count );

        /* Fomulate the response based on the input buffer */
        if( pBuf == NULL )
        {
          pData->AvrcpGenericOp.operand_count = 0;
        }
        pData->AvrcpGenericOp.cmd = AEEBT_AVRCP_RESPONSE_NOT_IMPLEMENTED;
        IBTEXTAVRCP_SendGenericReply( pMe->mAVRCP.po,
                                      &pData->AvrcpGenericOp,
                                      pBuf );

        if( pBuf != NULL )
        {
          FREE( pBuf );
        }
      }
      
      break;
    case AEEBT_AVRCP_EVT_TARGET_COMMAND:
    default:
      {
        MSG_ERROR( "AVRCP - unexpected ev=%d", evt, 0, 0 );
        break;
      }
  }
}

/* ==========================================================================
FUNCTION BTApp_AVRCPInit
DESCRIPTION
============================================================================= */
boolean BTApp_AVRCPInit( CBTApp* pMe )
{
  uint32 uNMask = NMASK_BT_AVRCP | (((uint32)(NOTIFIER_VAL_ANY)) << 16);

  if ( btapp_avrcp_init_done == FALSE )
  {
    if ( ISHELL_RegisterNotify( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_APP,
                                AEECLSID_BLUETOOTH_NOTIFIER, 
                                uNMask ) 
         == SUCCESS  )
    {
      uBTApp_NMask |= NMASK_BT_AVRCP;

      pMe->mAVRCP.bdAddr     =  NULL_BD_ADDR;
      pMe->mAVRCP.evtStart   =  0;
      pMe->mAVRCP.evtEnd     =  0;
      pMe->mAVRCP.bReleasing =  FALSE;

      btapp_avrcp_init_done = TRUE;
    }
  }
  
  return btapp_avrcp_init_done;
}

/* ==========================================================================
FUNCTION BTApp_AVRCPCleanup
DESCRIPTION
============================================================================= */
void BTApp_AVRCPCleanup( CBTApp* pMe )
{
  pMe->mAVRCP.bReleasing = TRUE;
  if( pMe->mAVRCP.bConnected == TRUE )
  {
    IBTEXTAVRCP_Disconnect( pMe->mAVRCP.po );
    BTApp_ShowBusyIcon( pMe );
  }
  else
  {
    if ( pMe->mAVRCP.po != NULL )
    {
      if( IBTEXTAVRCP_Release( pMe->mAVRCP.po ) == SUCCESS )
      {
        pMe->mAVRCP.po         = NULL;
        pMe->mAVRCP.bEnabled   = FALSE;
        pMe->mAVRCP.bReleasing = FALSE;

        ISHELL_RegisterNotify( pMe->a.m_pIShell,  AEECLSID_BLUETOOTH_APP,
                               AEECLSID_BLUETOOTH_NOTIFIER, 0 );
        uBTApp_NMask &= ~NMASK_BT_AVRCP;
        ISHELL_RegisterNotify( pMe->a.m_pIShell,  AEECLSID_BLUETOOTH_APP,
                               AEECLSID_BLUETOOTH_NOTIFIER, uBTApp_NMask );

        btapp_avrcp_init_done = FALSE;
      }
      else
      {
        MSG_ERROR( "AVRCP - can't unregister", 0, 0, 0 );
      }
    }
  }
  BTApp_BuildMenu( pMe, TOP_MENU );
}

/* ==========================================================================
FUNCTION BTApp_AVRCPDisable
DESCRIPTION
============================================================================= */
void BTApp_AVRCPDisable( CBTApp* pMe )
{

  if( ( pMe->mAVRCP.bConnected == TRUE) && ( pMe->mAVRCP.bReleasing == FALSE ) )
  {
    pMe->mAVRCP.bReleasing = TRUE;
    IBTEXTAVRCP_Disconnect( pMe->mAVRCP.po );
    BTApp_ShowBusyIcon( pMe );
  }
  else if( pMe->mAVRCP.bConnected == FALSE )
  {
    if ( pMe->mAVRCP.po != NULL )
    {
      if( IBTEXTAVRCP_Release( pMe->mAVRCP.po ) == SUCCESS )
      {
        pMe->mAVRCP.po         = NULL;
        pMe->mAVRCP.bEnabled   = FALSE;
        pMe->mAVRCP.bReleasing = FALSE;
      }
      else
      {
        MSG_ERROR( "AVRCP - can't unregister", 0, 0, 0 );
      }
    }
  }
  else
  {
    /* We can come here when there are multiple calls to  BTApp_DisableBT() */
    MSG_HIGH( "AVRCP disconnection already in progress, Do Nothing ", 0,0,0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BuildAVRCPTestMenu
DESCRIPTION
============================================================================= */
boolean BTApp_BuildAVRCPTestMenu( CBTApp* pMe, BTAppMenuType menu )
{
  boolean built = TRUE;
  CtlAddItem ai;

  if ( BTApp_AVRCPInit( pMe ) == FALSE )
  {
    MSG_ERROR( "AVRCPBuildMenu - failed to create AVRCP object", 0, 0, 0 );
    BTApp_AVRCPCleanup( pMe );
    return FALSE;
  }

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  BTApp_InitAddItem( &ai );

  if ( menu == BT_APP_MENU_AVRCP_OP_LIST )
  {
    int i,j;
    uint16 button_op;
    AECHAR button_name[SHORT_TEXT_BUF_LEN];
    // set the title
    IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_AVRCP_OP_MENU, NULL );
    
    // Add a line for each button press in the history
    for( i = pMe->mAVRCP.evtStart, j=0; i != pMe->mAVRCP.evtEnd; i++, i%=MAX_AVRCP_EVENTS,j++ )
    {
      if( pMe->mAVRCP.evtType[i] == AEEBT_AVRCP_OP_PRESSED )
      {
        button_op = IDB_RADIO_FILLED;
      }
      else
      {
        button_op = IDB_RADIO_UNFILLED;
      }
      if( ISHELL_LoadResString( pMe->a.m_pIShell, BTAPP_RES_FILE,
                                IDS_AVRCP_SELECT+pMe->mAVRCP.evtTable[i]-AEEBT_AVRCP_EVT_OP_SELECT,
                                button_name, SHORT_TEXT_BUF_LEN*sizeof(AECHAR) )
          > 0 )
      {
        ai.wImage  = button_op;
        ai.pText   = button_name;
        ai.wItemID = j;
        IMENUCTL_AddItemEx( pMe->m_pIMenu, &ai );
      }
    }
    IMENUCTL_SetSel( pMe->m_pIMenu, j-1 );
    if( TOP_MENU == BT_APP_MENU_AVRCP_OP_LIST )
    {
      MENU_SET_SEL( j-1 );
    }
  }
  else if ( menu == BT_APP_MENU_AVRCP_TEST )
  {
    // set the title
    IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_AVRCP, NULL );
    
    // Add individual entries to the Menu
    if ( pMe->mAVRCP.bEnabled == FALSE )
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE, 0 );
    }
    else
    {
      if ( pMe->mAVRCP.bConnected == FALSE )
      {
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISABLE, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_USE, 0 );
        if ( AEEBT_BD_ADDRS_EQUAL( &pMe->mAVRCP.bdAddr, &NULL_BD_ADDR ) == FALSE )
        {
          // The saved BD Addr is not null.
          BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT, 0 );
        }
      }
      else
      {
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AVRCP_OP_MENU, 0 );
        if( pMe->mAVRCP.evtStart != pMe->mAVRCP.evtEnd )
        {
          BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AVRCP_CLEAR, 0 );
        }
      }
    }
  }
  // Activate menu
  if( (menu == BT_APP_MENU_AVRCP_TEST && TOP_MENU != BT_APP_MENU_AVRCP_TEST )
      || (menu == BT_APP_MENU_AVRCP_OP_LIST) && TOP_MENU != BT_APP_MENU_AVRCP_OP_LIST )
  {
    PUSH_MENU( menu );
  }
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  
  return built;
}

/* ==========================================================================
FUNCTION BTApp_HandleAVRCPOpMenu
DESCRIPTION
============================================================================= */
boolean BTApp_HandleAVRCPOpMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  MENU_SET_SEL( IMENUCTL_GetSel( pMe->m_pIMenu ) );
  if( key == AVK_CLR )
  {
    ev_processed = BTApp_HandleClearKey( pMe );
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleAVRCPMainMenu
DESCRIPTION
============================================================================= */
boolean BTApp_HandleAVRCPMainMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  uint16  selection;
  
  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key)) == 0 )
  {
    selection = IMENUCTL_GetSel( pMe->m_pIMenu );
  }
  
  MSG_MED( "HndlAVRCP - k=0x%x sel=%x", key, selection, 0 );

  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
      ev_processed = TRUE;  // ignored
      break;
    case AVK_1:
    case AVK_2:
    case AVK_3:
    case AVK_4:
    case AVK_SELECT:
    {
      switch ( selection )
      {
        case IDS_ENABLE:
          if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_AVRCP,
                                     (void**)&pMe->mAVRCP.po )
              == SUCCESS )
          {
            if( IBTEXTAVRCP_Enable( pMe->mAVRCP.po, 
                                AEEBT_AVRCP_CATEGORY1 | AEEBT_AVRCP_CATEGORY2 |
                                AEEBT_AVRCP_CATEGORY3 | AEEBT_AVRCP_CATEGORY4 )
                == SUCCESS )
            {
              pMe->mAVRCP.bEnabled = TRUE;
            }
            if ( pMe->mSD.bDiscoverable == FALSE )
            {
              IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
            }            
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
            ev_processed = TRUE;
            BTApp_BuildMenu( pMe, TOP_MENU );
          }
          break;
        case IDS_DISABLE:
          BTApp_AVRCPDisable( pMe );
          BTApp_CheckToClearDiscoverable( pMe );        
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
          ev_processed = TRUE;
          BTApp_BuildMenu( pMe, TOP_MENU );
          break;
        case IDS_CONNECT:
          if ( IBTEXTAVRCP_Connect( pMe->mAVRCP.po, &pMe->mAVRCP.bdAddr ) != SUCCESS )
          {
            BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
          }
          else
          {
            ev_processed = TRUE;
            BTApp_ShowBusyIcon( pMe );
          }
          break;
        case IDS_USE:
          ev_processed = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_ALL );
          break;
        case IDS_DISCONNECT:
          if ( IBTEXTAVRCP_Disconnect( pMe->mAVRCP.po ) == SUCCESS )
          {
            ev_processed = TRUE;
            BTApp_ShowBusyIcon( pMe );
          }          
          break;
        case IDS_AVRCP_OP_MENU:
          ev_processed = BTApp_BuildMenu( pMe, BT_APP_MENU_AVRCP_OP_LIST );
          break;
        case IDS_AVRCP_CLEAR:
          pMe->mAVRCP.evtEnd = pMe->mAVRCP.evtStart;
          ev_processed = BTApp_BuildMenu( pMe, TOP_MENU );
          break;
        default:
          MSG_ERROR( "HndlAVRCP - unexp sel=%x", selection, 0, 0 );
          break;
      }
      break;
    }
    case AVK_CLR:
    {
      ev_processed = BTApp_HandleClearKey( pMe );
      break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleAVRCPTestMenu
DESCRIPTION
============================================================================= */
boolean BTApp_HandleAVRCPTestMenu( CBTApp* pMe, uint16 key, BTAppMenuType menu )
{
  boolean ev_processed = FALSE;

  if ( pMe->uCurrMsgId != 0 )
  {
    int rv;
    // If a message is up, clear it.
    rv = BTApp_HandleClearKey( pMe );
    // BTApp_BuildAVRCPTestMenu( pMe, menu );
    return rv;
  }

  if( menu == BT_APP_MENU_AVRCP_TEST )
  {
    ev_processed = BTApp_HandleAVRCPMainMenu( pMe, key );
  }
  else
  {
    ev_processed = BTApp_HandleAVRCPOpMenu( pMe, key );
  }

  return ev_processed;
}
/* ==========================================================================
FUNCTION BTApp_EnableAVRCP
DESCRIPTION
============================================================================= */
void BTApp_EnableAVRCP( CBTApp* pMe, boolean* pbSettingBondable )
{
  if ( pMe->mAVRCP.bEnableAVRCP == TRUE )
  {
#ifdef FEATURE_BT_2_1
    if( pMe->mAVRCP.bSecSelected !=FALSE )
    {
      IBTEXTRM_SetSecByPSM( pMe->mRM.po, 
                            AEEBT_SD_PROTOCOL_AVCTP,
                            pMe->mAVRCP.srvSecType,
                            pMe->mAVRCP.bAuthorize,
                            pMe->mAVRCP.bAuthorizeFirst );
    }
#endif //FEATURE_BT_2_1

    if ( BTApp_AVRCPInit( pMe ) == FALSE )
    {
      MSG_ERROR( "EnableAVRCP - Registration for AVRCP notifications failed", 
                 0, 0, 0 );
      BTApp_AVRCPCleanup( pMe );
    }
    else if ( pMe->mAVRCP.bEnabled == FALSE )
    {
      MSG_LOW( "EnableAVRCP - enabling AVRCP", 0, 0, 0 );
      if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_AVRCP, 
                                 (void**)&pMe->mAVRCP.po ) == SUCCESS )
      {

        if( IBTEXTAVRCP_Enable( pMe->mAVRCP.po, 
                                AEEBT_AVRCP_CATEGORY1 | AEEBT_AVRCP_CATEGORY2 |
                                AEEBT_AVRCP_CATEGORY3 | AEEBT_AVRCP_CATEGORY4 )
            == SUCCESS )
        {
          pMe->mAVRCP.bEnabled = TRUE;
        }
      }
      else
      {
        MSG_ERROR( "EnableAVRCP - Failed to enable AVRCP", 0, 0, 0 );
        BTApp_AVRCPCleanup( pMe );
      }
    }
  }
}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION
#endif /* FEATURE_BT_EXTPF_AV */
#endif /* FEATURE_APP_BLUETOOTH */

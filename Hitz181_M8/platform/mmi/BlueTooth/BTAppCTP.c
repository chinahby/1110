/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppCTP.c                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains CTP test feature.        */  
/*                                                                           */  
/*              (c) COPYRIGHT 2004-2005 QUALCOMM Incorporated.               */
/*                         All Rights Reserved.                              */ 
/*                                                                           */
/*                         QUALCOMM Proprietary                              */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppCTP.c#1 $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
06/26/07   pn   Removed compiler warnings.
03/25/05   pn   Made use of BTApp_SetBondable() and BTApp_ClearBondable().
12/28/04   gs   Initial version

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp.h"
#include "BTAppUtils.h"

#ifdef FEATURE_BT_EXTPF_CTP
#include "AEEBTExtCTP.h"

#define EVT_CTP_CONN_REQ                         1
#define EVT_CTP_CONNECTED                        2
#define EVT_CTP_CONN_FAILED                      3
#define EVT_CTP_DISCONNECTED                     4

#define EVT_CTP_REG                              5
#define EVT_CTP_REG_FAILED                       6
#define EVT_CTP_DEREG                            7
#define EVT_CTP_DEREG_FAILED                     8

extern uint32 uBTApp_NMask;

/* ==========================================================================
FUNCTION BTApp_ProcessCTPNotifications
DESCRIPTION
============================================================================= */
void BTApp_ProcessCTPNotifications(
  CBTApp* pMe, 
  AEEEvent evt, 
  NotificationData* pData
)
{
  uint32 userEvent;

  switch ( evt )
  {
    case AEEBT_CTP_EVT_REG_DONE:  
    {
      if ( pData->uError == AEEBT_CTP_ERR_NONE )
      {
        userEvent = EVT_CTP_REG;
      }
      else 
      {
        userEvent = EVT_CTP_REG_FAILED;
      }
      BTAPP_POST_USER_EVENT( CTP, userEvent );
      break;
    }
    case AEEBT_CTP_EVT_DEREG_DONE: 
    {
      if ( pData->uError == AEEBT_CTP_ERR_NONE )
      {
        userEvent = EVT_CTP_DEREG;
      }
      else 
      {
        userEvent = EVT_CTP_DEREG_FAILED;
      }
      BTAPP_POST_USER_EVENT( CTP, userEvent );
      break;
    }
    case AEEBT_CTP_EVT_CONN_REQ:  
    {
      pMe->mCTP.remoteBDAddr = pData->bdAddr;
      BTAPP_POST_USER_EVENT( CTP, EVT_CTP_CONN_REQ );
      break;
    }
    case AEEBT_CTP_EVT_CONNECTED:  
    {
      BTAPP_POST_USER_EVENT( CTP, EVT_CTP_CONNECTED );
      break;
    }
    case AEEBT_CTP_EVT_CONNECT_FAILED: 
    {
      BTAPP_POST_USER_EVENT( CTP, EVT_CTP_CONN_FAILED );
      break;
    }
    case AEEBT_CTP_EVT_DISCONNECTED: 
    {
      BTAPP_POST_USER_EVENT( CTP, EVT_CTP_DISCONNECTED );
      break;
    }
    default:
      MSG_ERROR( "CTP - unexpected ev=%d", evt, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_CTPBuildMainMenu
DESCRIPTION
============================================================================= */
void BTApp_CTPBuildMainMenu( CBTApp* pMe )
{
  char       szStatus[] = " -   ";
  uint8      len = 0;

  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  if ( pMe->mCTP.bConnected )
  {
    szStatus[ 5 ] = 'C';
  }
  if ( pMe->mCTP.bRegistered )
  {
    szStatus[ 4 ] = 'R';
  }

  ISHELL_LoadResString( pMe->a.m_pIShell, 
                        AEE_APPSBTAPP_RES_FILE, 
                        IDS_CTP_TESTS, 
                        pMe->pText2, 
                        SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
  len = WSTRLEN( pMe->pText2 );

  STRTOWSTR( szStatus, &pMe->pText2[ len ], 
             (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
  IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );

  BTApp_InitAddItem( &ai );

  if ( pMe->mCTP.bRegistered == FALSE ) 
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REGISTER, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEREGISTER, 0 );
  }

  if ( pMe->mCTP.bConnected == FALSE ) 
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
  }

  if ( pMe->mCTP.bRegistered == TRUE && pMe->mCTP.bConnected == TRUE ) 
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CTP_EST_CALL, 0 );
  }
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_CTP_TESTS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_CTPCleanup
DESCRIPTION
============================================================================= */
static void BTApp_CTPCleanup( CBTApp* pMe )
{
  if ( pMe->mCTP.po != NULL )
  {
    IBTEXTCTP_Release( pMe->mCTP.po );
    pMe->mCTP.po = NULL;
  }
  
  // unregister for CTP notification
  ISHELL_RegisterNotify( pMe->a.m_pIShell,  
                         AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 
                         0 );
  uBTApp_NMask &= ~NMASK_BT_CTP;
  ISHELL_RegisterNotify( pMe->a.m_pIShell,  
                         AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 
                         uBTApp_NMask );
}

/* ==========================================================================
FUNCTION BTApp_CTPEstablishCall
DESCRIPTION
============================================================================= */
boolean BTApp_CTPEstablishCall( CBTApp* pMe )
{
  AEEBTCTPNumberType calledNumber;
  AEEBTCTPNumberType callingNumber;

  // TODO: Fill up called and calling number data structure

  if ( IBTEXTCTP_EstablishCall( pMe->mCTP.po, 
                                &calledNumber,
                                &callingNumber ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_CTP_MSG_EST_CALL_FAILED, NULL, 3 );
    return FALSE;
  }
  else
  {
    // TODO: Show call progress time
    BTApp_ShowBusyIcon( pMe );
    return TRUE;
  }
}

/* ==========================================================================
FUNCTION BTApp_CTPInit
DESCRIPTION
============================================================================= */
static boolean BTApp_CTPInit( CBTApp* pMe )
{
  static boolean init_done = FALSE;
  uint32 uNMask = NMASK_BT_CTP | (((uint32)(NOTIFIER_VAL_ANY)) << 16);

  if ( init_done == FALSE )
  {
    if ( (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_CTP, 
                                 (void**)&pMe->mCTP.po ) == SUCCESS) &&
         (ISHELL_RegisterNotify( pMe->a.m_pIShell,  AEECLSID_BLUETOOTH_APP,
                                 AEECLSID_BLUETOOTH_NOTIFIER, 
                                 uNMask ) == SUCCESS) )
    {
      uBTApp_NMask |= NMASK_BT_CTP;
      init_done = TRUE;
    }
  }
  return init_done;
}

/* ==========================================================================
FUNCTION BTApp_CTPBuildMenu
DESCRIPTION
============================================================================= */
boolean BTApp_CTPBuildMenu( CBTApp* pMe, BTAppMenuType menu )
{
  boolean built = TRUE;
  switch ( menu)
  {
    case BT_APP_MENU_CTP_TESTS:
    {
      if ( BTApp_CTPInit( pMe ) != FALSE )
      {
        BTApp_CTPBuildMainMenu( pMe );
      }
      else
      {
        MSG_ERROR( "CTPBuildMenu - failed to create CTP object", 0, 0, 0 );
        BTApp_CTPCleanup( pMe );
        built = FALSE;
      }
      break;
    }
    default:
      built = FALSE;
      break;
  }
  return built;
}

/* ==========================================================================
FUNCTION BTApp_CTPHandleSelection
DESCRIPTION
============================================================================= */
static boolean BTApp_CTPHandleSelection( CBTApp* pMe, uint16 sel )
{
  boolean key_handled = TRUE;

  switch ( sel )
  {
    case IDS_REGISTER:
    {
      BTApp_SetBondable( pMe );
      if ( IBTEXTCTP_Register( pMe->mCTP.po ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case IDS_DEREGISTER:
    {
      if ( IBTEXTCTP_Deregister( pMe->mCTP.po ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case IDS_CONNECT:
    {
      BTApp_SetBondable( pMe );
      if ( IBTEXTCTP_Connect( 
             pMe->mCTP.po, 
             &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case IDS_DISCONNECT:
    {
      if ( IBTEXTCTP_Disconnect( pMe->mCTP.po ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    default:
    {
      key_handled = FALSE;
      break;
    }
  }
  return key_handled;
}

/* ==========================================================================
FUNCTION BTApp_HandleCTPMainMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleCTPMainMenu( CBTApp* pMe, uint16 key )
{
  uint16 selection;
  boolean ev_processed = TRUE;

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_CTPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key)
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_CTPHandleSelection( pMe, selection );
        break;
      }
      case AVK_CLR:
      {
        BTApp_HandleClearKey( pMe );
        break;
      }
      default:
      {
        ev_processed = FALSE;
        break;
      }
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_CTPHandleMenus
DESCRIPTION
============================================================================= */
boolean BTApp_CTPHandleMenus( CBTApp *pMe, uint16 key, BTAppMenuType menu )
{
  boolean handled = TRUE;
  switch ( menu )
  {
    case BT_APP_MENU_CTP_TESTS:
      handled = BTApp_HandleCTPMainMenu( pMe, key );
      break;
    default:
      MSG_ERROR(" CTP: unexpected menu type %x", menu, 0, 0 );
      handled = FALSE;
      break;
  }
  return handled;
}

/* ==========================================================================
FUNCTION BTApp_CTPHandleUserEvents
DESCRIPTION
============================================================================= */
void BTApp_CTPHandleUserEvents( CBTApp* pMe, uint32 dwParam )
{
  switch ( dwParam )
  {
    case EVT_CTP_CONN_REQ:
      pMe->mCTP.bConnected = TRUE;
      IBTEXTCTP_AcceptConnection( pMe->mCTP.po, 
                                  &pMe->mCTP.remoteBDAddr,
                                  TRUE );
      BTApp_BuildTopMenu( pMe ); // rebuild menu to show second 'C'
      break;
    case EVT_CTP_REG:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_DONE, NULL, 2 );
      pMe->mCTP.bRegistered = TRUE;
      break;
    case EVT_CTP_REG_FAILED:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      break;
    case EVT_CTP_DEREG:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_DONE, NULL, 2 );
      pMe->mCTP.bRegistered = FALSE;
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      break;
    case EVT_CTP_DEREG_FAILED:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      break;
  case EVT_CTP_CONNECTED:
      pMe->mCTP.bConnected = TRUE;
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      (void) POP_MENU();
      BTApp_BuildTopMenu( pMe ); // rebuild menu to show 'C'
      break;
    case EVT_CTP_DISCONNECTED:
      pMe->mCTP.bConnected = FALSE;
      if ( pMe->mCTP.bEndingTest != FALSE )
      {
        pMe->mCTP.bEndingTest = FALSE;
        BTApp_HandleClearKey( pMe );
      }
      else
      {
        (void) POP_MENU();
        BTApp_BuildTopMenu( pMe ); // rebuild menu to hide 'C'
      }
      break;
    case EVT_CTP_CONN_FAILED:
      BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      break;
    default:
      MSG_ERROR(" CTP: unexpected user event %x", dwParam, 0, 0 );
      break;
  }
}

#endif /* FEATURE_BT_EXTPF_CTP */
#endif /* FEATURE_APP_BLUETOOTH */


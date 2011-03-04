/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppICP.c                                 */
/*                                                                           */
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains ICP test feature.        */
/*                                                                           */
/*              (c) COPYRIGHT 2004-2006,2008 QUALCOMM Incorporated.          */
/*                         All Rights Reserved.                              */ 
/*                                                                           */
/*                         QUALCOMM Proprietary                              */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppICP.c#1 $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
10/09/08   jtl  Removing usage of depricated functions.
06/26/07   pn   Removed compiler warnings.
08/14/06   dsn  Fix for typecasting errors with RVCT compiler.
03/25/05   pn   Made use of BTApp_SetBondable() and BTApp_ClearBondable().
                Added BTApp_ICPConnect().
01/20/05   gs   Call setup and teardown now works
01/12/05   gs   Added more support for ICP testing
12/28/04   gs   Initial version

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp_priv.h"		//"BTApp.h"

#include "BTAppUtils.h"

#ifdef FEATURE_BT_EXTPF_CTP
#include "AEEBTExtICP.h"

#define EVT_ICP_CONN_REQ                         1
#define EVT_ICP_CONNECTED                        2
#define EVT_ICP_CONN_FAILED                      3
#define EVT_ICP_DISCONNECTED                     4

#define EVT_ICP_REG                              5
#define EVT_ICP_REG_FAILED                       6
#define EVT_ICP_DEREG                            7
#define EVT_ICP_DEREG_FAILED                     8

#define EVT_ICP_CALL_SETUP_REQ                   9
#define EVT_ICP_CALL_SETUP_SUCCESS              10
#define EVT_ICP_CALL_SETUP_FAILED               11
#define EVT_ICP_CALL_RELEASED                   12
#define EVT_ICP_OUTGOING_STATUS                 13

/* Constants */
/* Commented out for now to avoid compiler warning
static AECHAR wStatusOR[]    = 
    {'O','v','e','r','l','a','p',' ','R','e','c','e','i','v','i','n','g',0};
static AECHAR wStatusPR[]    =  {'P','r','o','c','e','e','d','i','n','g',0};
static AECHAR wStatusAL[]    =  {'A','l','e','r','t','i','n','g',0};
*/

extern uint32 uBTApp_NMask;

/* ==========================================================================
FUNCTION BTApp_ProcessICPNotifications
DESCRIPTION
============================================================================= */
void BTApp_ProcessICPNotifications(
  CBTApp* pMe, 
  AEEEvent evt, 
  NotificationData* pData
)
{
  uint32 userEvent;

  switch ( evt )
  {
    case AEEBT_ICP_EVT_REG_DONE:  
    {
      if ( pData->uError == AEEBT_ICP_ERR_NONE )
      {
        userEvent = EVT_ICP_REG;
      }
      else 
      {
        userEvent = EVT_ICP_REG_FAILED;
      }
      BTAPP_POST_USER_EVENT( ICP, userEvent );
      break;
    }
    case AEEBT_ICP_EVT_DEREG_DONE: 
    {
      if ( pData->uError == AEEBT_ICP_ERR_NONE )
      {
        userEvent = EVT_ICP_DEREG;
      }
      else 
      {
        userEvent = EVT_ICP_DEREG_FAILED;
      }
      BTAPP_POST_USER_EVENT( ICP, userEvent );
      break;
    }
    case AEEBT_ICP_EVT_CONN_REQ:  
    {
      pMe->mICP.remoteBDAddr = pData->bdAddr;
      BTAPP_POST_USER_EVENT( ICP, EVT_ICP_CONN_REQ );
      break;
    }
    case AEEBT_ICP_EVT_CONNECTED:  
    {
      BTAPP_POST_USER_EVENT( ICP, EVT_ICP_CONNECTED );
      break;
    }
    case AEEBT_ICP_EVT_CONNECT_FAILED: 
    {
      BTAPP_POST_USER_EVENT( ICP, EVT_ICP_CONN_FAILED );
      break;
    }
    case AEEBT_ICP_EVT_DISCONNECTED: 
    {
      BTAPP_POST_USER_EVENT( ICP, EVT_ICP_DISCONNECTED );
      break;
    }
    case AEEBT_ICP_EVT_CALL_SETUP_REQ: 
    {
      BTAPP_POST_USER_EVENT( ICP, EVT_ICP_CALL_SETUP_REQ );
      break;
    }
    case AEEBT_ICP_EVT_CALL_SETUP_SUCCESS:
    {
      BTAPP_POST_USER_EVENT( ICP, EVT_ICP_CALL_SETUP_SUCCESS );
      break;
    }
  case AEEBT_ICP_EVT_CALL_SETUP_FAILED:
    {     
      BTAPP_POST_USER_EVENT( ICP, EVT_ICP_CALL_SETUP_FAILED );
      break;
    }
    case AEEBT_ICP_EVT_CALL_RELEASED:
    {
      BTAPP_POST_USER_EVENT( ICP, EVT_ICP_CALL_RELEASED );
      break;
    }
    case AEEBT_ICP_EVT_OUTGOING_STATUS:
    {
      pMe->mICP.status = pData->IcpObject.status;
      BTAPP_POST_USER_EVENT( ICP, EVT_ICP_OUTGOING_STATUS );
      break;
    }
    default:
      MSG_ERROR( "ICP - unexpected ev=%d", evt, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_ICPBuildMainMenu
DESCRIPTION
============================================================================= */
void BTApp_ICPBuildMainMenu( CBTApp* pMe )
{
  char       szStatus[] = " -     ";
  uint8      len = 0;

  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  if ( pMe->mICP.bRegistered )
  {
    szStatus[ 4 ] = 'R';
  }
  if ( pMe->mICP.bConnected )
  {
    szStatus[ 5 ] = 'C';
  }
  if ( pMe->mICP.bCallEstablished )
  {
    szStatus[ 6 ] = 'C';
  }

  ISHELL_LoadResString( pMe->m_pShell, 
                        AEE_APPSBTAPP_RES_FILE, 
                        IDS_ICP_TESTS, 
                        pMe->pText2, 
                        SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
  len = WSTRLEN( pMe->pText2 );

  STRTOWSTR( szStatus, &pMe->pText2[ len ], 
             (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
  IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );

  BTApp_InitAddItem( &ai );

  if ( pMe->mICP.bRegistered == FALSE ) 
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REGISTER, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEREGISTER, 0 );

    if ( pMe->mICP.bCallEstablished ) 
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ICP_CLEAR_CALL, 0 );
    }
    else
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ICP_EST_CALL, 0 );
    }
  }
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_ICP_TESTS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_ICPCleanup
DESCRIPTION
============================================================================= */
static void BTApp_ICPCleanup( CBTApp* pMe )
{
  if ( pMe->mICP.po != NULL )
  {
    IBTEXTICP_Release( pMe->mICP.po );
    pMe->mICP.po = NULL;
  }
  
  // unregister for ICP notification
  ISHELL_RegisterNotify( pMe->m_pShell,  
                         AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 
                         0 );
  uBTApp_NMask &= ~NMASK_BT_ICP;
  ISHELL_RegisterNotify( pMe->m_pShell,  
                         AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 
                         uBTApp_NMask );
}

/* ==========================================================================
FUNCTION BTApp_ICPEstablishCall
DESCRIPTION
============================================================================= */
boolean BTApp_ICPEstablishCall( CBTApp* pMe )
{
  AEEBTICPNumberType calledNumber;
  AEEBTICPNumberType callingNumber;

  // Fill up called and calling number data structure
  callingNumber.bCalling = TRUE;
  STRLCPY( (char *)callingNumber.digits, "4085571084", AEEBT_TCS_MAX_DIGITS );
  callingNumber.numDigits = 10;

  calledNumber.bCalling = FALSE;
  STRLCPY( (char *)calledNumber.digits, "4084230356", AEEBT_TCS_MAX_DIGITS );
  calledNumber.numDigits = 10;

  if ( IBTEXTICP_EstablishCall( pMe->mICP.po, 
                                &callingNumber,
                                &calledNumber ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_ICP_MSG_EST_CALL_FAILED, NULL, 3 );
    return FALSE;
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
    return TRUE;
  }
}

/* ==========================================================================
FUNCTION BTApp_ICPInit
DESCRIPTION
============================================================================= */
static boolean BTApp_ICPInit( CBTApp* pMe )
{
  static boolean init_done = FALSE;
  uint32 uNMask = NMASK_BT_ICP | (((uint32)(NOTIFIER_VAL_ANY)) << 16);

  if ( init_done == FALSE )
  {
    if ( (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BLUETOOTH_ICP, 
                                 (void**)&pMe->mICP.po ) == SUCCESS) &&
         (ISHELL_RegisterNotify( pMe->m_pShell,  AEECLSID_BLUETOOTH_APP,
                                 AEECLSID_BLUETOOTH_NOTIFIER, 
                                 uNMask ) == SUCCESS) )
    {
      uBTApp_NMask |= NMASK_BT_ICP;
      init_done = TRUE;
    }
  }
  return init_done;
}

/* ==========================================================================
FUNCTION BTApp_ICPBuildMenu
DESCRIPTION
============================================================================= */
boolean BTApp_ICPBuildMenu( CBTApp* pMe, BTAppMenuType menu )
{
  boolean built = TRUE;
  switch ( menu)
  {
    case BT_APP_MENU_ICP_TESTS:
    {
      if ( BTApp_ICPInit( pMe ) != FALSE )
      {
        BTApp_ICPBuildMainMenu( pMe );
      }
      else
      {
        MSG_ERROR( "ICPBuildMenu - failed to create ICP object", 0, 0, 0 );
        BTApp_ICPCleanup( pMe );
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
FUNCTION BTApp_ICPHandleSelection
DESCRIPTION
============================================================================= */
static boolean BTApp_ICPHandleSelection( CBTApp* pMe, uint16 sel )
{
  boolean key_handled = TRUE;

  switch ( sel )
  {
    case IDS_REGISTER:
    {
      BTApp_SetBondable( pMe );
      if ( IBTEXTICP_Register( pMe->mICP.po ) != SUCCESS )
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
      if ( IBTEXTICP_Deregister( pMe->mICP.po ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case IDS_DISCONNECT:
    {
      if ( IBTEXTICP_Disconnect( pMe->mICP.po ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case IDS_ICP_EST_CALL:
    {
      pMe->mICP.bCallingParty = TRUE;
      key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_TELEPHONY );
      break;
    }
    case IDS_ICP_CLEAR_CALL:
    {
      if ( IBTEXTICP_ClearCall( 
             pMe->mICP.po, 
             AEEBT_TCS_CAUSE_NORMAL_CALL_CLEARING ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_TCS_CLEAR_CALL_FAILED, NULL, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
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
FUNCTION BTApp_HandleICPMainMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleICPMainMenu( CBTApp* pMe, uint16 key )
{
  uint16 selection;
  boolean ev_processed = TRUE;

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_ICPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key)
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_ICPHandleSelection( pMe, selection );
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
FUNCTION BTApp_ICPHandleMenus
DESCRIPTION
============================================================================= */
boolean BTApp_ICPHandleMenus( CBTApp *pMe, uint16 key, BTAppMenuType menu )
{
  boolean handled = TRUE;
  switch ( menu )
  {
    case BT_APP_MENU_ICP_TESTS:
      handled = BTApp_HandleICPMainMenu( pMe, key );
      break;
    default:
      MSG_ERROR(" ICP: unexpected menu type %x", menu, 0, 0 );
      handled = FALSE;
      break;
  }
  return handled;
}

/* ==========================================================================
FUNCTION BTApp_ICPHandleUserEvents
DESCRIPTION
============================================================================= */
void BTApp_ICPHandleUserEvents( CBTApp* pMe, uint32 dwParam )
{
  switch ( dwParam )
  {
    case EVT_ICP_CONN_REQ:
      pMe->mICP.bConnected = TRUE;
      pMe->mICP.bCallingParty = FALSE;
      IBTEXTICP_AcceptConnection( pMe->mICP.po, 
                                  &pMe->mICP.remoteBDAddr,
                                  TRUE );
      BTApp_BuildTopMenu( pMe ); // rebuild menu to show first 'C'
      break;
    case EVT_ICP_REG:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_DONE, NULL, 2 );
      pMe->mICP.bRegistered = TRUE;
      break;
    case EVT_ICP_REG_FAILED:
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
      break;
    case EVT_ICP_DEREG:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_DONE, NULL, 2 );
      pMe->mICP.bRegistered = FALSE;
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      break;
    case EVT_ICP_DEREG_FAILED:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      break;
    case EVT_ICP_CONNECTED:
      pMe->mICP.bConnected = TRUE;
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      BTApp_BuildTopMenu( pMe ); // rebuild menu to show first 'C'

      if ( pMe->mICP.bCallingParty ) 
      {
        BTApp_ICPEstablishCall( pMe );
      }
      break;
    case EVT_ICP_CALL_SETUP_SUCCESS:
      pMe->mICP.bCallEstablished = TRUE;
      BTApp_ShowMessage( pMe, IDS_MSG_TCS_CALL_SETUP_SUCCESS, NULL , 3 );
      BTApp_BuildTopMenu( pMe );
      break;
    case EVT_ICP_CALL_SETUP_FAILED:
      BTApp_ShowMessage ( pMe, IDS_MSG_TCS_CALL_SETUP_FAILED, NULL, 3 );
      break;
    case EVT_ICP_DISCONNECTED:
      pMe->mICP.bConnected = FALSE;
      pMe->mICP.bCallEstablished = FALSE;
      if ( pMe->mICP.bEndingTest != FALSE )
      {
        pMe->mICP.bEndingTest = FALSE;
        BTApp_HandleClearKey( pMe );
      }
      else
      {
        BTApp_BuildTopMenu( pMe ); // rebuild menu to hide first 'C'
      }
      break;
    case EVT_ICP_CONN_FAILED:
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      break;
    case EVT_ICP_CALL_SETUP_REQ:
      pMe->mICP.bCallEstablished = TRUE;
      IBTEXTICP_AcceptCall( pMe->mICP.po );

      BTApp_BuildTopMenu( pMe ); // rebuild menu to show second 'C'
      break;
    case EVT_ICP_CALL_RELEASED:
      pMe->mICP.bCallEstablished = FALSE;
      
      BTApp_BuildTopMenu( pMe ); // rebuild menu to hide second 'C'
      break;
    /* TODO : Activate this code after corresponding resource string
       has been defined
    case EVT_ICP_OUTGOING_STATUS:
      switch ( pMe->mICP.status )
      {
      case AEEBT_TCS_CALL_STATUS_OVERLAP_RECEIVING:
        BTApp_ShowMessage( pMe, IDS_MSG_TCS_OUT_STATUS, wStatusOR, 3); 
        break;
      case AEEBT_TCS_CALL_STATUS_PROCEEDING:
        BTApp_ShowMessage( pMe, IDS_MSG_TCS_OUT_STATUS, wStatusPR, 3); 
        break;
      case AEEBT_TCS_CALL_STATUS_ALERTING:}
        BTApp_ShowMessage( pMe, IDS_MSG_TCS_OUT_STATUS, wStatusAL, 3); 
        break;
      default:
        break;    
      }
      break;
    */
    default:
      MSG_ERROR(" ICP: unexpected user event %x", dwParam, 0, 0 );
      break;
  }
}

/* ==========================================================================
FUNCTION BTApp_ICPConnect
DESCRIPTION
============================================================================= */
void BTApp_ICPConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr )
{
  BTApp_SetBondable( pMe );
  if ( IBTEXTICP_Connect( pMe->mICP.po, pBDAddr ) != SUCCESS )
  {
    BTApp_ClearBondable( pMe ); // no need to be bondable anymore
    BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
  }
}

#endif /* FEATURE_BT_EXTPF_ICP */
#endif /* FEATURE_APP_BLUETOOTH */


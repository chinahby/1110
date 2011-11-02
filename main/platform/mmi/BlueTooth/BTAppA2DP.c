/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppA2DP.c                                */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW A2DP Application.              */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains A2DP test feature.       */  
/*                                                                           */  
/*              Copyright (c) 2005-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppA2DP.c#2 $
$DateTime: 2009/06/17 10:41:55 $
$Author: nksingh $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
06/11/09    ns  changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
11/21/08   ssk  Prevent multiple disconnect calls from A2DP APP to BT driver.
                This fixes issues in back-to-back calls to BTApp_DisableBT()
11/21/08   ssk  Handle A2DP\AVRCP disconnect evts after switching brew context
11/14/08   co   Not set sound device to A2DP when it gets connected/enabled
                while SCO is up.
10/06/08   co   Not set sound device to A2DP when it's connected during a
                call session.
08/07/08   vm   Initialize and Deinitialize AG when A2DP is enabled/disabled 
05/12/08   VM   Replacing deprecated functions with BREW replacements.
04/10/08   VM   Added BTApp_EnableA2DP()
02/11/08   ns    Reverting changes made for app coexistance
01/29/08   ns   Initialize and Deinitialize AG when A2DP is enabled/disabled 
12/04/07   mh   Profile and state configuration updates to app manager
24/08/07   dm   Check if A2DP is registered before Processing Notifications 
06/26/07   pn   Removed compiler warnings.
11/15/07   ns   Added fix for setting volume level for A2DP link once connected
11/29/06   mp   Fixed the A2DP retry connect even for normal A2DP disconnect
11/01/06   dsn  Fixed typecasting errors with A2DP and automation.
09/12/06   pn   Made BTApp work with and without SCO_REWORK enabled.
07/31/06   ns   Added changes related to audio rework feature
04/25/06   pn   Added code to track when A2DP streaming is started/suspended.
12/26/05   ns   Added support to set passkey while enabling A2DP in automation
                Device is set to discoverable mode when A2DP is enabled
12/02/05   sn   Externalized BTApp_A2DPInit() and BTApp_A2DPCleanup().
10/09/05   dc   Removed the Disable menu when A2DP is connected
06/27/05   ns   Fixed problem in A2DP enabling. Null pointer passed as 
                parameter in case of failure while creating an instance
06/17/05   ns   Fixed problem of incorrect status in A2DP control automation
06/16/05   ns   Added UI and automation support for A2DP control interface
05/28/05   dc   Added functions to handle A2DP automation. 
04/27/05   jtl  Better handling of menu stack
03/14/05   jtl  Initial Release. 

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp_priv.h"		//"BTApp.h"


#ifdef FEATURE_BT_EXTPF_AV
#include "BTAppA2DP.h"
#endif

#include "BTAppUtils.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#ifdef FEATURE_BT_EXTPF_AV
#include "AEEBTExtA2DP.h"

extern uint32 uBTApp_NMask;

extern void    BTApp_InitA2DPVolumes( CBTApp* pMe );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION


/* ==========================================================================
Keep track of how many reconnect attempts have been made to the same device.
If we go past MAX_RETRIES, give up trying to connect.
============================================================================= */
#define BTAPP_A2DP_MAX_RETRIES 3
static  uint8 BTApp_A2DP_retries = 0;

/* ==========================================================================
FUNCTION BTApp_A2DPHandleDevListMenu
DESCRIPTION
============================================================================= */
void BTApp_A2DPHandleDevListMenu(
  CBTApp*           pMe
)
{
  pMe->mA2DP.bdAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
  pMe->bConfigChanged = TRUE;
  IBTEXTA2DP_SetDevice( pMe->mA2DP.po, &pMe->mA2DP.bdAddr );
#ifndef FEATURE_AVS_BT_SCO_REWORK
  BTApp_UseA2DPDevice( pMe );
#endif
  BTApp_BuildA2DPTestMenu( pMe );
}

/* ==========================================================================
FUNCTION BTApp_ProcessA2DPNotifications
DESCRIPTION
============================================================================= */
void BTApp_ProcessA2DPNotifications( 
  CBTApp*           pMe, 
  AEEEvent          evt, 
  NotificationData* pData
)
{
  MSG_HIGH( "A2DP Notif - evt=%d", evt, 0, 0 );

  if( TOP_MENU == BT_APP_MENU_A2DP_TEST )
  {
    pMe->bBusyIconUp = FALSE;
  }

  /* If A2DP is already De-registered then donot process notifications */
  if ( pMe->mA2DP.po == NULL)
  {
     MSG_ERROR("Notification received when A2DP is not registered", 0, 0, 0);
     return;
  }

  MSG_FATAL("***zzg BTApp_ProcessA2DPNotifications evt=%x***", evt, 0, 0);
  
  switch ( evt )
  {
    case AEEBT_A2DP_EVT_CONNECTED:
    {
      pMe->mA2DP.bConnected = TRUE;
      pMe->mA2DP.bStreaming = FALSE;
      // init volumes for A2DP device type
      BTApp_InitA2DPVolumes( pMe );     
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

      //if ( TOP_MENU == BT_APP_MENU_A2DP_TEST )
      {
        BTApp_ShowDevMsg( pMe, IDS_MSG_AG_CONN, &pMe->mA2DP.bdAddr, 2 );
      }

      BTApp_BuildMenu( pMe, TOP_MENU );

      if ( (BTApp_CallPresent( pMe ) == BT_APP_CALL_NONE) &&
           (pMe->mAG.bAudioConnected == FALSE) )
      {
        BTApp_UseA2DPDevice( pMe );
      }

      BTApp_A2DP_retries = 0;
      BTAppMgr_UpdateProfileType( pMe, &pData->bdAddr, BTAPP_MGR_A2DP, BTAPP_MGR_CONNECTED );	 
	  
      break;
    }
    case AEEBT_A2DP_EVT_CON_FAILED:
    {
	  MSG_FATAL("***zzg BTApp_ProcessA2DPNotifi AEEBT_A2DP_EVT_CON_FAILED uError=%x***", pData->uError,0,0);
      MSG_ERROR( "A2DP - Connect failed err=%x",
                  pData->uError,0,0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      if( BTApp_A2DP_retries < BTAPP_A2DP_MAX_RETRIES )
      {
        IBTEXTA2DP_Connect( pMe->mA2DP.po, &pMe->mA2DP.bdAddr );
        BTApp_A2DP_retries++;
      }
      else
      {
        BTApp_ShowDevMsg( pMe, IDS_MSG_AG_CONNF, &pMe->mA2DP.bdAddr, 2 );
      }
      BTApp_ReleaseA2DPDevice( pMe );	 
	  
      break;
    }
    case AEEBT_A2DP_EVT_DISCONNECTED:
    {
      pMe->mA2DP.bConnected = FALSE;
      pMe->mA2DP.bStreaming = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      BTApp_ReleaseA2DPDevice( pMe );
      if( pMe->mA2DP.bReleasing == TRUE )
      {
        ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC,
                            AEECLSID_BLUETOOTH_APP,
                            EVT_USER, EVT_A2DP_DISCONNECTED, 0L);
      }
      else 
      { 
        /* if its not normal disconnect or link lost, retry A2DP conn */
        if((pData->uError != AEEBT_A2DP_ERR_NORMAL_DISCONNECT) &&
              (pData->uError != AEEBT_A2DP_ERR_LINK_LOST))
        { 
          if( BTApp_A2DP_retries < BTAPP_A2DP_MAX_RETRIES )
          {
            IBTEXTA2DP_Connect( pMe->mA2DP.po, &pMe->mA2DP.bdAddr );
            BTApp_A2DP_retries++;
          }
          else
          {
            BTApp_ShowDevMsg( pMe, IDS_MSG_AG_DISCONN, &pMe->mA2DP.bdAddr, 2 );
            BTApp_BuildMenu( pMe, TOP_MENU );
          }
        }
        else
        {
          BTApp_ShowDevMsg( pMe, IDS_MSG_AG_DISCONN, &pMe->mA2DP.bdAddr, 2 );
          BTApp_BuildMenu( pMe, TOP_MENU );
        }
      }
      BTAppMgr_UpdateProfileType( pMe, &pMe->mA2DP.bdAddr, BTAPP_MGR_A2DP, BTAPP_MGR_DISCONNECTED );
	  
      break;
    }
    case AEEBT_A2DP_EVT_BITRATE:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    }
    case AEEBT_A2DP_EVT_START:
      pMe->mA2DP.bStreaming = TRUE;
      BTAppMgr_UpdateStateType( pMe, &pMe->mA2DP.bdAddr, BTAPP_MGR_USE_A2DP );
      break;
    case AEEBT_A2DP_EVT_SUSPEND:
      pMe->mA2DP.bStreaming = FALSE;
      BTAppMgr_UpdateStateType( pMe, &pMe->mA2DP.bdAddr, BTAPP_MGR_RELEASE_A2DP );
      break;
    case AEEBT_A2DP_EVT_OPEN:
    case AEEBT_A2DP_EVT_CLOSE:
      break;
    default:
      MSG_ERROR( "A2DP - unexpected ev=%d", evt, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_A2DPInit
DESCRIPTION
============================================================================= */
boolean BTApp_A2DPInit( CBTApp* pMe )
{
  
  uint32 uNMask = NMASK_BT_A2DP | (((uint32)(NOTIFIER_VAL_ANY)) << 16);

  if ( pMe->mA2DP.bInitialized != FALSE )
  {
    return TRUE;
  }
  if ( ISHELL_RegisterNotify( pMe->m_pShell, AEECLSID_BLUETOOTH_APP,
                              AEECLSID_BLUETOOTH_NOTIFIER, 
                              uNMask ) 
       == SUCCESS  )
  {
    uBTApp_NMask |= NMASK_BT_A2DP;
    pMe->mA2DP.bInitialized = TRUE;
    return TRUE;
  }
  return FALSE;
}

/* ==========================================================================
FUNCTION BTApp_A2DPCleanup
DESCRIPTION
============================================================================= */
void BTApp_A2DPCleanup( CBTApp* pMe )
{
  pMe->mA2DP.bReleasing = TRUE;
  if( pMe->mA2DP.bConnected == TRUE )
  {
    IBTEXTA2DP_Disconnect( pMe->mA2DP.po );
    BTApp_ShowBusyIcon( pMe );
  }
  else
  {
    if ( pMe->mA2DP.po != NULL )
    {
      IBTEXTA2DP_Release( pMe->mA2DP.po );
      pMe->mA2DP.po = NULL;
      pMe->mA2DP.bInitialized = FALSE;
    }

    pMe->mA2DP.bEnabled   = FALSE;
    pMe->mA2DP.bReleasing = FALSE;
    
    // unregister A2DP notification
    ISHELL_RegisterNotify( pMe->m_pShell,  AEECLSID_BLUETOOTH_APP,
                           AEECLSID_BLUETOOTH_NOTIFIER, 0 );
    uBTApp_NMask &= ~NMASK_BT_A2DP;
    ISHELL_RegisterNotify( pMe->m_pShell,  AEECLSID_BLUETOOTH_APP,
                           AEECLSID_BLUETOOTH_NOTIFIER, uBTApp_NMask );
  }
}

/* ==========================================================================
FUNCTION BTApp_A2DPDisable
DESCRIPTION
============================================================================= */
void BTApp_A2DPDisable( CBTApp* pMe )
{
	if ((pMe->mA2DP.bConnected == TRUE) && (pMe->mA2DP.bReleasing == FALSE))
	{
		pMe->mA2DP.bReleasing = TRUE;
		IBTEXTA2DP_Disconnect(pMe->mA2DP.po);
		BTApp_ShowBusyIcon(pMe);
	}
	else if (pMe->mA2DP.bConnected == FALSE)
	{
		if (pMe->mA2DP.po != NULL)
		{
			IBTEXTA2DP_Release(pMe->mA2DP.po);
			pMe->mA2DP.po = NULL;
			pMe->mA2DP.bInitialized = FALSE;
		}
		
		pMe->mA2DP.bEnabled   = FALSE;
		pMe->mA2DP.bReleasing = FALSE;

		// unregister A2DP notification
		ISHELL_RegisterNotify(pMe->m_pShell,  AEECLSID_BLUETOOTH_APP, AEECLSID_BLUETOOTH_NOTIFIER, 0);
		uBTApp_NMask &= ~NMASK_BT_A2DP;

		ISHELL_RegisterNotify(pMe->m_pShell,  AEECLSID_BLUETOOTH_APP, AEECLSID_BLUETOOTH_NOTIFIER, uBTApp_NMask);    
		
		if ((pMe->mAG.bInitialized != FALSE) && (pMe->mAG.bEnabled == FALSE))
		{
			MSG_LOW( "A2DPDisable - Deinitializing AG as AG is not enabled", 0, 0, 0 );
			BTApp_DeregisterAGNotif(pMe);
			BTApp_AGDeInit((IApplet*)pMe);
		}
	}
	else
	{
		/* We can come here when there are multiple calls to  BTApp_DisableBT() */
		MSG_HIGH( "A2DP disconnection already in progress, Do Nothing ", 0,0,0 );
	}
}

/* ==========================================================================
FUNCTION BTApp_BuildA2DPCtrlMenu
DESCRIPTION
============================================================================= */
boolean BTApp_BuildA2DPControlMenu( CBTApp* pMe )
{
  boolean built = TRUE;
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_A2DP_CONTROL, NULL );

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mA2DP.bCloseOnEnd == TRUE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_SUSPEND_ON_END, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_CLOSE_ON_END, 0 );
  }
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_START_DELAY, 0 );
  if ( pMe->mA2DP.bHonorInUse == TRUE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_IGNORE_IN_USE, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_HONOR_IN_USE, 0 );
  }
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_RECONFIG_BY_RECONFIG, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_RECONFIG_BY_CLOSE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_RECONFIG_BY_REPAGE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_INCREMENT_BITRATE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_DECREMENT_BITRATE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_SET_BITRATE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_AUTO_BITRATE, 0 );

  // Activate menu
  if( TOP_MENU != BT_APP_MENU_A2DP_CONTROL )
  {
    PUSH_MENU( BT_APP_MENU_A2DP_CONTROL );
  }
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  return built;
}

/* ==========================================================================
FUNCTION BTApp_BuildA2DPTestMenu
DESCRIPTION
============================================================================= */
boolean BTApp_BuildA2DPTestMenu( CBTApp* pMe )
{
  boolean built = TRUE;
  CtlAddItem ai;

  if ( BTApp_A2DPInit( pMe ) == FALSE )
  {
    MSG_ERROR( "A2DPBuildMenu - failed to create A2DP object", 0, 0, 0 );
    BTApp_A2DPCleanup( pMe );
    return FALSE;
  }

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_A2DP, NULL );

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mA2DP.bEnabled == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE, 0 );
  }
  else
  {
    if ( pMe->mA2DP.bConnected == FALSE )
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISABLE, 0 );
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_USE, 0 );
      if ( AEEBT_BD_ADDRS_EQUAL( &pMe->mA2DP.bdAddr, &NULL_BD_ADDR ) == FALSE )
      {
        // The saved BD Addr is not null.
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT, 0 );
      }
    }
    else
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
    }
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_A2DP_CONTROL, 0 );
  }

  // Activate menu
  if( TOP_MENU != BT_APP_MENU_A2DP_TEST )
  {
    PUSH_MENU( BT_APP_MENU_A2DP_TEST );
  }
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  
  return built;
}

/* ==========================================================================
FUNCTION BTApp_HandleA2DPTestMenu
DESCRIPTION
============================================================================= */
boolean BTApp_HandleA2DPTestMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  uint16  selection;

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key)) == 0 )
  {
    selection = IMENUCTL_GetSel( pMe->m_pIMenu );
  }

  MENU_SET_SEL( selection );
  MSG_MED( "HndlA2DP - k=0x%x sel=%x", key, selection, 0 );

  if ( pMe->uCurrMsgId != 0 )
  {
    int rv;
    // If a message is up, clear it.
    rv = BTApp_HandleClearKey( pMe );
    BTApp_BuildA2DPTestMenu( pMe );
    return rv;
  }

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
        {
          if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BLUETOOTH_A2DP,
                                     (void**)&pMe->mA2DP.po ) == SUCCESS )
          {
            pMe->mA2DP.bEnabled = TRUE;
            /* A2DP has a dependency on AG. If AG notifications are not registered 
               then sound change events will not be received by APP. So register 
               for AG notifications whenever A2DP is enabled  */
            if ( pMe->mAG.bInitialized == FALSE )
            {
              if ( BTApp_AGInit( pMe ) == FALSE )
              {
                MSG_ERROR( "A2DP Enable - Failed to register for AG events", 0, 0, 0 );
              }
            }

            // Clear ctl items reset on enabling A2DP
            pMe->mA2DP.bCloseOnEnd = FALSE;
            pMe->mA2DP.bHonorInUse = TRUE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
            BTApp_BuildMenu( pMe, TOP_MENU );
            BTApp_SetBondable( pMe );
            if ( pMe->mSD.bDiscoverable == FALSE )
            {
              IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
            }
            if ( AEEBT_BD_ADDRS_EQUAL( &pMe->mA2DP.bdAddr, &NULL_BD_ADDR ) == FALSE )
            {
              // The saved BD Addr is not null.
              IBTEXTA2DP_SetDevice( pMe->mA2DP.po, &pMe->mA2DP.bdAddr );
#ifndef FEATURE_AVS_BT_SCO_REWORK
              BTApp_UseA2DPDevice( pMe );
#endif
              BTApp_ClearBondable( pMe );
            }
          }
          break;
        }
        case IDS_DISABLE:
        {        
          BTApp_A2DPDisable( pMe );
          BTApp_CheckToClearDiscoverable(pMe );
          
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
          BTApp_BuildMenu( pMe, TOP_MENU );
          BTApp_ReleaseA2DPDevice( pMe );
          break;
        }
        case IDS_CONNECT:
        {
          if ( IBTEXTA2DP_Connect( pMe->mA2DP.po, &pMe->mA2DP.bdAddr ) != SUCCESS )
          {
            BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
          }
          else
          {
            BTApp_A2DP_retries = 0;
            ev_processed = TRUE;
            BTApp_ShowBusyIcon( pMe );
          }
          break;
        }
        case IDS_USE:
        {
          ev_processed = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_ALL );
          break;
        }
        case IDS_DISCONNECT:
        {
          BTApp_A2DP_retries = BTAPP_A2DP_MAX_RETRIES;
          BTApp_ShowBusyIcon( pMe );
          if ( IBTEXTA2DP_Disconnect( pMe->mA2DP.po ) != SUCCESS )
          {
            BTApp_ShowMessage( pMe, IDS_A2DP_DISCONNECT_FAILED, NULL, 3 );
          }
          else
          {
            ev_processed = TRUE;
          }
          break;
        }
        case IDS_A2DP_CONTROL:
        {
          BTApp_BuildMenu( pMe, BT_APP_MENU_A2DP_CONTROL );
        }
        default:
          MSG_ERROR( "HndlA2DP - unexp sel=%x", selection, 0, 0 );
          break;
      }
      break;
    }
    case AVK_CLR:
    {
      ev_processed = BTApp_HandleClearKey( pMe );
    }
  }

  return ev_processed;
}

//Add By zzg 2011_10_31
void BTApp_A2DPSetRetries(CBTApp* pMe, boolean least)		
{
	if (TRUE == least)
	{
		BTApp_A2DP_retries = 0;
	}
	else
	{
		BTApp_A2DP_retries = BTAPP_A2DP_MAX_RETRIES;
	}
}


/* ==========================================================================
FUNCTION BTApp_HandleA2DPControlMenu
DESCRIPTION
============================================================================= */
boolean BTApp_HandleA2DPControlMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = FALSE;
  uint16  selection;
  uint32 data = 0;
  int status = EFAILED;

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key)) == 0 )
  {
    selection = IMENUCTL_GetSel( pMe->m_pIMenu );
  }

  MENU_SET_SEL( selection );
  MSG_MED( "HndlA2DP - k=0x%x sel=%x", key, selection, 0 );

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
    case AVK_5:
    case AVK_6:
    case AVK_7:
    case AVK_8:
    case AVK_9:
    case AVK_SELECT:
    {
      ev_processed = TRUE;
      switch ( selection )
      {
        case IDS_A2DP_SUSPEND_ON_END:
        {
          status = IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_SUSPEND_ON_END, data );
          if ( status == SUCCESS )
          {
            pMe->mA2DP.bCloseOnEnd = FALSE;
          }
          break;
        }
        case IDS_A2DP_CLOSE_ON_END:
        {
          status = IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_CLOSE_ON_END, data );
          if ( status == SUCCESS )
          {
            pMe->mA2DP.bCloseOnEnd = TRUE;
          }
          break;
        }
        case IDS_A2DP_START_DELAY:
        {
          pMe->mA2DP.uControlEvtID = IDS_A2DP_START_DELAY;
          PUSH_MENU( BT_APP_MENU_A2DP_CONTROL_DATA );
          ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE,
                               IDD_BT_TEXT_EDIT, NULL );
          return ev_processed;
        }
        case IDS_A2DP_HONOR_IN_USE:
        {
          status = IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_HONOR_IN_USE, data );
          if ( status == SUCCESS )
          {
            pMe->mA2DP.bHonorInUse = TRUE;
          }
          break;
        }
        case IDS_A2DP_IGNORE_IN_USE:
        {
          status = IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_IGNORE_IN_USE, data );
          if ( status == SUCCESS )
          {
            pMe->mA2DP.bHonorInUse = FALSE;
          }
          break;
        }
        case IDS_A2DP_RECONFIG_BY_RECONFIG:
        {
          status = IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_RECFG_BY_RECFG, data );
          break;
        }
        case IDS_A2DP_RECONFIG_BY_CLOSE:
        {
          status = IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_RECFG_BY_CLOSE, data );
          break;
        }
        case IDS_A2DP_RECONFIG_BY_REPAGE:
        {
          status = IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_RECFG_BY_REPAGE, data );
          break;
        }
        case IDS_A2DP_INCREMENT_BITRATE:
        {
          status = IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_INCR_BITRATE, data );
          break;
        }
        case IDS_A2DP_DECREMENT_BITRATE:
        {
          status = IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_DECR_BITRATE, data );
          break;
        }
        case IDS_A2DP_SET_BITRATE:
        {
          pMe->mA2DP.uControlEvtID = IDS_A2DP_SET_BITRATE;        
          PUSH_MENU( BT_APP_MENU_A2DP_CONTROL_DATA );
          ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE,  
                               IDD_BT_TEXT_EDIT, NULL );
          return ev_processed;
        }
        case IDS_A2DP_AUTO_BITRATE:
        {
          status = IBTEXTA2DP_Ctl( pMe->mA2DP.po, AEEBT_A2DP_CTL_AUTO_BITRATE, data );
          break;
        }
        default:
          ev_processed = FALSE;
          MSG_ERROR( "HndlA2DP - unexp sel=%x", selection, 0, 0 );
          break;
      }
      if ( ev_processed == TRUE )
      {
        if ( status != SUCCESS )
        {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION                  
          BTApp_ShowMessage( pMe, IDS_A2DP_MSG_CTRL_CMD_FAILED, NULL, 3 );
        }
        else
        {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION                  
          BTApp_ShowMessage( pMe, IDS_A2DP_MSG_CTRL_CMD_SUCCESS, NULL, 3 );
        }
      }
      break;
    }
    case AVK_CLR:
    {
      ev_processed = BTApp_HandleClearKey( pMe );
    }
  }
  return ev_processed;
}
/* ==========================================================================
FUNCTION BTApp_EnableA2DP
DESCRIPTION
============================================================================= */
void BTApp_EnableA2DP( CBTApp* pMe, boolean* pbSettingBondable )
{
  MSG_FATAL("***zzg BTApp_EnableA2DP pbSettingBondable=%x, bEnableA2DP=%x***", pbSettingBondable, pMe->mA2DP.bEnableA2DP, 0);

  pMe->mA2DP.bEnableA2DP = pMe->mA2DP.bEnableA2DP ? FALSE : TRUE;
  
  if ( pMe->mA2DP.bEnableA2DP == TRUE )
  {

#ifdef FEATURE_BT_2_1
    if( pMe->mA2DP.bSecSelected !=FALSE )
    {
      IBTEXTRM_SetSecByPSM( pMe->mRM.po, 
                            AEEBT_SD_PROTOCOL_AVDTP,
                            pMe->mA2DP.srvSecType,
                            pMe->mA2DP.bAuthorize,
                            pMe->mA2DP.bAuthorizeFirst );
    }
#endif //FEATURE_BT_2_1

    if ( BTApp_A2DPInit( pMe ) == FALSE )
    {
      MSG_ERROR("EnableA2DP - Registration for A2DP notifications failed", 0, 0, 0 );
	  
	  MSG_FATAL("***zzg BTApp_EnableA2DP BTApp_A2DPInit Failed!***", 0, 0, 0 );
	  
      BTApp_A2DPCleanup( pMe );
    }
    else if ( pMe->mA2DP.bEnabled == FALSE )
    {
      MSG_LOW( "EnableA2DP - enabling A2DP", 0, 0, 0 );
      if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BLUETOOTH_A2DP, 
                                 (void**)&pMe->mA2DP.po ) == SUCCESS )
      {
        pMe->mA2DP.bEnabled = TRUE;

        // Clear ctl items reset on enabling A2DP
        pMe->mA2DP.bCloseOnEnd = FALSE;
        pMe->mA2DP.bHonorInUse = TRUE;

        if ( *pbSettingBondable == FALSE )
        {
          BTApp_SetBondable( pMe );
          *pbSettingBondable = TRUE;
        }
        /* A2DP has a dependency on AG. If AG notifications are not registered 
           then sound change events will not be received by APP. So register 
           for AG notifications whenever A2DP is enabled  */
        if ( pMe->mAG.bInitialized == FALSE )
        {
          if ( BTApp_AGInit( pMe ) == FALSE )
          {
            MSG_ERROR( "BTApp_EnableBT - Failed to init AG while enabling A2DP", 0, 0, 0 );
          }
        }
        if ( BDADDR_VALID( &pMe->mA2DP.bdAddr ) )
        {
          // The saved BD Addr is not null.
          IBTEXTA2DP_SetDevice( pMe->mA2DP.po, &pMe->mA2DP.bdAddr );
#ifdef FEATURE_AVS_BT_SCO_REWORK
          if ( (BTApp_CallPresent( pMe ) == BT_APP_CALL_NONE) &&
               (pMe->mAG.bAudioConnected == FALSE) )
          {
            BTApp_UseA2DPDevice( pMe );
          }
#endif
          BTApp_ClearBondable( pMe );
        }
      }
      else
      {
        MSG_ERROR( "EnableA2DP - Failed to enable A2DP", 0, 0, 0 );
        BTApp_A2DPCleanup( pMe );
      }
    }
  }
}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#endif /* FEATURE_BT_EXTPF_AV */

#endif /* FEATURE_APP_BLUETOOTH */

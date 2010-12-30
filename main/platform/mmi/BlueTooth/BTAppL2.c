/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppL2.c                                  */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW L2 Application.                */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains L2 test feature.         */  
/*                                                                           */  
/*                (c) COPYRIGHT 2007-2009 QUALCOMM Incorporated.                  */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppL2.c#2 $
$DateTime: 2009/06/17 10:41:55 $
$Author: nksingh $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
06/11/09    ns  changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
05/12/08   VM   Replacing deprecated functions with BREW replacements.
04/10/08   VM   Added BTApp_EnableL2()
02/27/07   jtl  Initial Release. 

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp.h"
#include "BTAppL2.h"
#include "BTAppUtils.h"
#include "AEEBTExtL2.h"
#include "AEEBTExtSD.h"

extern uint32 uBTApp_NMask;

#define AEEBT_L2_SVC_NAME { 'L', '2', ' ', 'T', 'e', 's', 't' }
#define AEEBT_L2_UUID { 0x77, 0x77, 0x77, 0x77, 0x77, 0xBE, 0xEF, 0xCA,\
                        0xFE, 0xDE, 0xAD, 0xF0, 0x05, 0xBA, 0x11, 0x00 }

/* ==========================================================================
FUNCTION BTApp_L2WriteDataCB
DESCRIPTION
============================================================================= */
void BTApp_L2WriteDataCB( void* pData )
{
  int32 i;
  CBTApp* pMe = (CBTApp*) pData;
  
  if ( pMe->mL2.pMem == NULL )
  {
    pMe->mL2.pMem = MALLOC( pMe->mL2.sCliInfo.out_mtu );
  }

  if ( pMe->mL2.pMem == NULL )
  {
    MSG_ERROR( "WriteL2Data - MALLOC failed %x bytes",
               pMe->mL2.sCliInfo.out_mtu, 0, 0 );
  }
  else
  {
    for ( i=0; i<pMe->mL2.sCliInfo.out_mtu; i++)
    {
      pMe->mL2.pMem[ i ] = (uint8)i + (uint8)pMe->mL2.uCliPacketsSent;
    }

    while( pMe->mL2.uCliTotalBytesSent < pMe->mL2.uCliBytesToSend )
    {
      i = IBTEXTL2_Write( pMe->mL2.pCliO, pMe->mL2.pMem, pMe->mL2.sCliInfo.out_mtu );
      if ( i > 0 )
      {
        pMe->mL2.uCliTotalBytesSent += i;
        pMe->mL2.uCliPacketsSent++;
      }
      else
      {
        if ( i == AEE_L2_WOULDBLOCK )
        {
          IBTEXTL2_Writable( pMe->mL2.pCliO, BTApp_L2WriteDataCB, pMe );
        }
        else
        {
          MSG_ERROR( "WriteL2 -- error %x", i,0,0);
        }
        break;
      }
    }
  }  
}

/* ==========================================================================
FUNCTION BTApp_L2ReadDataCB
DESCRIPTION
============================================================================= */
void BTApp_L2ReadDataCB( void* pData )
{
  int32 bytesRcvd;
  uint8 *pMem;
  int   i;
  CBTApp* pMe = (CBTApp*) pData;

  pMem = MALLOC( pMe->mL2.sSrvInfo.in_mtu );

  while( ( bytesRcvd = IBTEXTL2_Read( pMe->mL2.pSrvO, 
                                      pMem,
                                      pMe->mL2.sSrvInfo.in_mtu ) )
         > 0 )
  {
    if( bytesRcvd != pMe->mL2.sSrvInfo.in_mtu )
    {
      MSG_ERROR( "L2 Read=%x MTU=%x", bytesRcvd, pMe->mL2.sSrvInfo.in_mtu, 0 );
    }
    
    if ( pMe->mL2.bSrvDoVerify )
    {
      for( i = 0; i < bytesRcvd; i++ )
      {
        if( (uint8)pMem[i] != ( (uint8)i + (uint8)pMe->mL2.uSrvPacketsRcvd ) )
        {
          MSG_ERROR( "L2 Verify data failed. i=%x, data=%x, expceted=%x",
                     (uint8)i,
                     pMem[i],
                     (uint8)pMem[i]+(uint8)pMe->mL2.uSrvPacketsRcvd );
        }
      }
    }
    pMe->mL2.uSrvPacketsRcvd++;
  }
  FREE( pMem );
  IBTEXTL2_Readable( pMe->mL2.pSrvO, BTApp_L2ReadDataCB, pMe );
}

/* ==========================================================================
FUNCTION BTApp_L2HandleDevListMenu
DESCRIPTION
============================================================================= */
void BTApp_L2HandleDevListMenu(
  CBTApp*           pMe
)
{
  pMe->mL2.CliBDAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
  if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_L2,
                             (void**)&pMe->mL2.pCliO ) == SUCCESS )
  {
    if( pMe->mL2.bDoSDP == FALSE )
    {
      IBTEXTL2_Connect( pMe->mL2.pCliO, pMe->mL2.sCliInfo.psm, &pMe->mL2.CliBDAddr );      
    }
    else
    {
      AEEBTSearchPattern Srch;
      AEEBTUUID128       my_svc = AEEBT_L2_UUID;
      
      pMe->mL2.bCliConnecting = TRUE;
      
      Srch.uNumUUIDs           = 0;
      Srch.uNumAttrIDs         = 1;
      Srch.aAttrID[0].bIsRange = FALSE;
      Srch.aAttrID[0].uValue   = AEEBT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST;
      Srch.uNumUUID128s        = 1;
      Srch.aUUID128[0]         = my_svc;

      IBTEXTSD_SearchService( pMe->mSD.po,
                              &pMe->mL2.CliBDAddr,
                              &Srch,
                              &pMe->mL2.sSvcRec,
                              1 );
    }
  }
  BTApp_BuildL2TestMenu( pMe );
}

/* ==========================================================================
FUNCTION BTApp_ProcessL2Notifications
DESCRIPTION
============================================================================= */
void BTApp_ProcessL2Notifications( 
  CBTApp*           pMe, 
  AEEEvent          evt, 
  NotificationData* pData
)
{
  AEEBTL2Connected*     evt_connected  = (AEEBTL2Connected*)pData;
  //AEEBTL2ConnectFailed* evt_con_failed = (AEEBTL2ConnectFailed*)pData;
  AEEBTL2Disconnected*  evt_discon     = (AEEBTL2Disconnected*)pData;
  AEEBTL2Reconfigured*  evt_recfg      = (AEEBTL2Reconfigured*)pData;
  //AEEBTL2PingRsp*       evt_ping_rsp   = (AEEBTL2PingRsp*)pData;
  int i,j, bFound;

  MSG_HIGH( "L2 Notif - evt=%d", evt, 0, 0 );

  if( TOP_MENU == BT_APP_MENU_L2_TEST )
  {
    pMe->bBusyIconUp = FALSE;
  }

  switch ( evt )
  {
    case EVT_DEV_SEARCH_RESP:
      bFound = FALSE;
      pMe->mL2.bCliConnecting = FALSE;
      for( i = 0; i < pMe->mSD.uNumSvcRecs /*pMe->mL2.sSvcRec.uNumAttr*/; i++ )
      {
        if( pMe->mL2.sSvcRec.svcAttr[i].attrType == 
            AEEBT_SD_ATTR_TYPE_PROTO_DESC_LIST )
        {
          MSG_ERROR("L2: Found proto descriptor list",0,0,0);

          for( j = 0; 
               j < 
                 pMe->mL2.sSvcRec.svcAttr[i].value.protoDescList.uNumProtoDesc;
               j++ )
          {
            if( pMe->mL2.sSvcRec.svcAttr[i].value.protoDescList.aProtoDesc[j].uUUID ==
                AEEBT_SD_PROTOCOL_L2CAP )
            {
              // Assume this is the one we're looking for.
              pMe->mL2.sCliInfo.psm = 
                pMe->mL2.sSvcRec.svcAttr[i].value.protoDescList.aProtoDesc[j].aParam[0].uValue;
              bFound = TRUE;
            }
            else
            {
              MSG_ERROR("L2: Found non l2 proto %x",
                        pMe->mL2.sSvcRec.svcAttr[i].value.protoDescList.aProtoDesc[j].uUUID,0,0);
            }
          }
        }
        else
        {
          MSG_ERROR("L2: Found non proto descriptor list %x",
                    pMe->mL2.sSvcRec.svcAttr[i].attrType,0,0);
        }
      }
      if( bFound )
      {
        IBTEXTL2_Connect( pMe->mL2.pCliO, pMe->mL2.sCliInfo.psm, &pMe->mL2.CliBDAddr );
      }
      else
      {
        MSG_ERROR("Can't find PSM for client connect",0,0,0);
      }
      break;
    case AEEBT_L2_EVT_CONNECTED:
      if( AEEBT_BD_ADDRS_EQUAL(&evt_connected->bdAddr, &pMe->mL2.CliBDAddr) &&
          (pMe->mL2.bCliConnected == FALSE) )
      {
        // Not the best check, but assume this is a client connection
        pMe->mL2.sCliInfo      = evt_connected->info;
        pMe->mL2.bCliConnected = TRUE;
        pMe->mL2.uCliTotalBytesSent = 0;
        pMe->mL2.uCliPacketsSent = 0;
        pMe->mL2.uCliStartTime = GETUPTIMEMS();
        IBTEXTL2_Writable( pMe->mL2.pCliO, BTApp_L2WriteDataCB, pMe );
        if( TOP_MENU == BT_APP_MENU_L2_TEST )
        {
          BTApp_BuildMenu( pMe, BT_APP_MENU_L2_RESULTS );
        }
      }
      else if( pMe->mL2.bSrvConnected == FALSE ) 
      {
        // A server connection
        pMe->mL2.sSrvInfo = evt_connected->info;
        pMe->mL2.bSrvConnected = TRUE;
        pMe->mL2.uSrvPacketsRcvd = 0;
        IBTEXTL2_Readable( pMe->mL2.pSrvO, BTApp_L2ReadDataCB, pMe );
      }
      break;
    case AEEBT_L2_EVT_CONNECT_FAILED:
      if( pMe->mL2.pCliO != NULL )
      {
        IBTEXTL2_Release( pMe->mL2.pCliO );
        pMe->mL2.pCliO = NULL;
        MSG_ERROR( "L2 - Connect failed err=%x",
                   pData->uError,0,0);
      }
      break;
    case AEEBT_L2_EVT_DISCONN_IMMEDIATE:
    case AEEBT_L2_EVT_DISCONNECTED:
      if( evt_discon->cid == pMe->mL2.sCliInfo.cid )
      {
        pMe->mL2.bCliConnected = FALSE;
        IBTEXTL2_Release( pMe->mL2.pCliO );
        pMe->mL2.pCliO = NULL;
        ISHELL_CancelTimer( pMe->a.m_pIShell,
                            (PFNNOTIFY) BTApp_BuildL2ResultsMenu,
                            pMe );
        FREE( pMe->mL2.pMem );
        pMe->mL2.pMem = NULL;
        if( TOP_MENU == BT_APP_MENU_L2_TEST )
        {
          BTApp_BuildMenu( pMe, TOP_MENU );
        }
      }
      else if( evt_discon->cid == pMe->mL2.sSrvInfo.cid )
      {
        pMe->mL2.bSrvConnected = FALSE;
      }
      break;
    case AEEBT_L2_EVT_RECONFIGURED:
      if( evt_recfg->info.cid == pMe->mL2.sCliInfo.cid )
      {
        pMe->mL2.sCliInfo = evt_recfg->info;
      }
      else if( evt_recfg->info.cid == pMe->mL2.sSrvInfo.cid )
      {
        pMe->mL2.sSrvInfo = evt_recfg->info;
      }
      break;
    case AEEBT_L2_EVT_PING_RSP:
      break;
    default:
      MSG_ERROR( "L2 - unexpected ev=%d", evt, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_L2SetSecurity
DESCRIPTION
============================================================================= */
void BTApp_L2SetSecurity( CBTApp* pMe, uint8 sec_lvl )
{
  AEEBTServiceIdentifier sID;
  sID.serviceIdMethod = AEEBT_SIM_L2CAP_PSM;
  sID.id.uPSM = pMe->mL2.sSrvInfo.psm;
  IBTEXTRM_SetServiceSecurity( pMe->mRM.po,
                               &sID,
                               sec_lvl,
                               FALSE /* bAuthorize */,
                               FALSE /* bAuthorizeFirst */);

  if( pMe->mL2.sSrvInfo.psm != pMe->mL2.sCliInfo.psm )
  {
    sID.id.uPSM = pMe->mL2.sCliInfo.psm;
    IBTEXTRM_SetServiceSecurity( pMe->mRM.po,
                                 &sID,
                                 sec_lvl,
                                 FALSE /* bAuthorize */,
                                 FALSE /* bAuthorizeFirst */);
  }

  if( sec_lvl == AEEBT_SEC_NONE )
  {
    pMe->mL2.bEnableSrvSvcSec = FALSE;
    pMe->mL2.bEnableCliSvcSec = FALSE;
  }
  else
  {
    pMe->mL2.bEnableSrvSvcSec = TRUE;
    pMe->mL2.bEnableCliSvcSec = TRUE;
  }    
}

/* ==========================================================================
FUNCTION BTApp_L2Enable
DESCRIPTION
============================================================================= */
void BTApp_L2Enable( CBTApp* pMe )
{
  AEEBTUuidList svc_uuids;
  AEEBTUUID128  my_svc     = AEEBT_L2_UUID;
  AECHAR        svc_name[] = AEEBT_L2_SVC_NAME;
  
  svc_uuids.uNumUUIDs = 1;
  svc_uuids.aUUID[0]  = AEEBT_SD_PROTOCOL_L2CAP;
  
  svc_uuids.uNumUUID128s = 1;
  svc_uuids.aUUID128[0]  = my_svc;


  if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_L2,
                             (void**)&pMe->mL2.pSrvO ) == SUCCESS )
  {
    pMe->mL2.bEnabled = TRUE;
    IBTEXTL2_Register( pMe->mL2.pSrvO,
                       pMe->mL2.sSrvInfo.psm );
    IBTEXTSD_RegisterCustomService( pMe->mSD.po,
                                    &svc_uuids,
                                    0, /* version */
                                    0, /* scn */
                                    pMe->mL2.sSrvInfo.psm,
                                    svc_name );
  }
  else
  {
    MSG_ERROR("Can't create L2 class",0,0,0);
  }
}

/* ==========================================================================
FUNCTION BTApp_L2Init
DESCRIPTION
============================================================================= */
boolean BTApp_L2Init( CBTApp* pMe )
{
  static boolean init_done = FALSE;
  uint32 uNMask = NMASK_BT_L2 | (((uint32)(NOTIFIER_VAL_ANY)) << 16);

  if ( init_done == FALSE )
  {
    if ( ISHELL_RegisterNotify( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_APP,
                                AEECLSID_BLUETOOTH_NOTIFIER, 
                                uNMask ) 
         == SUCCESS  )
    {
      uBTApp_NMask |= NMASK_BT_L2;
      init_done = TRUE;
    }
    pMe->mL2.bReleasing                 = FALSE;
    pMe->mL2.pCliO                      = NULL;
    pMe->mL2.bEnabled                   = FALSE;
    pMe->mL2.bSrvConnected              = FALSE;
    pMe->mL2.bCliConnected              = FALSE;
    pMe->mL2.bDoSDP                     = FALSE;
    pMe->mL2.sCliInfo.psm               = 7777;
    pMe->mL2.sCliInfo.cid               = 0;
    pMe->mL2.sCliInfo.in_flush_to       = AEEBT_L2_FLUSH_TO_DEFALUT;
    pMe->mL2.sCliInfo.in_mtu            = BTAPP_L2_TEST_MTU;
    pMe->mL2.sCliInfo.out_mtu           = BTAPP_L2_TEST_MTU;
    pMe->mL2.sCliInfo.in_qos            = AEEBT_L2_QOS_DEFAULT_TYPE;
    pMe->mL2.sCliInfo.token_rate        = 0;
    pMe->mL2.sCliInfo.token_bucket_size = 0;
    pMe->mL2.sCliInfo.peak_bandwidth    = 0;
    pMe->mL2.sCliInfo.latency           = 0;
    pMe->mL2.sCliInfo.delay_variation   = 0;
    pMe->mL2.dwSettingSec               = 0;

    if( pMe->mL2.bEnableL2Srv == TRUE )
    {
      BTApp_L2Enable( pMe );
    }

    if( pMe->mL2.bEnableSrvSvcSec != FALSE )
    {
      BTApp_L2SetSecurity( pMe, AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT );
    }
  }
  
  return init_done;
}

/* ==========================================================================
FUNCTION BTApp_L2Cleanup
DESCRIPTION
============================================================================= */
void BTApp_L2Cleanup( CBTApp* pMe )
{
  if( pMe->mL2.bCliConnected == TRUE )
  {
    pMe->mL2.bReleasing = TRUE;
    IBTEXTL2_Disconnect( pMe->mL2.pCliO );
  }
  if( pMe->mL2.bSrvConnected == TRUE )
  {
    pMe->mL2.bReleasing = TRUE;
    IBTEXTL2_Disconnect( pMe->mL2.pSrvO );
  }

  if( pMe->mL2.bReleasing == FALSE )
  {
    if ( pMe->mL2.pCliO != NULL )
    {
      IBTEXTL2_Release( pMe->mL2.pCliO );
      pMe->mL2.pCliO = NULL;
    }
    if ( pMe->mL2.pSrvO != NULL )
    {
      IBTEXTL2_Release( pMe->mL2.pSrvO );
      pMe->mL2.pSrvO = NULL;
    }

    pMe->mL2.bEnabled   = FALSE;
    pMe->mL2.bReleasing = FALSE;
    
    BTApp_L2SetSecurity( pMe, AEEBT_SEC_NONE );

    ISHELL_RegisterNotify( pMe->a.m_pIShell,  AEECLSID_BLUETOOTH_APP,
                           AEECLSID_BLUETOOTH_NOTIFIER, 0 );
    uBTApp_NMask &= ~NMASK_BT_L2;
    ISHELL_RegisterNotify( pMe->a.m_pIShell,  AEECLSID_BLUETOOTH_APP,
                           AEECLSID_BLUETOOTH_NOTIFIER, uBTApp_NMask );
  }
}

/* ==========================================================================
FUNCTION BTApp_L2Disable
DESCRIPTION
============================================================================= */
void BTApp_L2Disable( CBTApp* pMe )
{
  if( pMe->mL2.bCliConnected == TRUE )
  {
    pMe->mL2.bReleasing = TRUE;
    IBTEXTL2_Disconnect( pMe->mL2.pCliO );
    BTApp_ShowBusyIcon( pMe );
  }
  if( pMe->mL2.bSrvConnected == TRUE )
  {
    pMe->mL2.bReleasing = TRUE;
    IBTEXTL2_Disconnect( pMe->mL2.pSrvO );
    BTApp_ShowBusyIcon( pMe );
  }

  if( pMe->mL2.bReleasing == FALSE )
  {
    if ( pMe->mL2.pCliO != NULL )
    {
      IBTEXTL2_Release( pMe->mL2.pCliO );
      pMe->mL2.pCliO = NULL;
    }
    if ( pMe->mL2.pSrvO != NULL )
    {
      IBTEXTL2_Release( pMe->mL2.pSrvO );
      pMe->mL2.pSrvO = NULL;
    }
    pMe->mL2.bEnabled   = FALSE;
    pMe->mL2.bReleasing = FALSE;
  }
}

/* ==========================================================================
FUNCTION BTApp_BuildL2ResultsMenu
DESCRIPTION
============================================================================= */
boolean BTApp_BuildL2ResultsMenu( CBTApp* pMe )
{
  boolean built = TRUE;
  uint16  uLen = 0;
  AEERect rc;
  AECHAR wBuf1[ 64 ];

  if ( TOP_MENU != BT_APP_MENU_L2_RESULTS )
  {
    CLEAR_SCREEN();

    // set rect for info display area
    SETAEERECT ( &rc, pMe->m_rect.x, 
                 pMe->m_rect.y, 
                 pMe->m_rect.dx, 
                 pMe->m_rect.dy);
    ISTATIC_SetRect( pMe->m_pStatic, &rc );

    ISTATIC_SetProperties( 
      pMe->m_pStatic, 
      ISTATIC_GetProperties( pMe->m_pStatic ) & 
        ~ST_MIDDLETEXT & ~ST_CENTERTEXT );

    // get the screen title
    ISHELL_LoadResString( pMe->a.m_pIShell, AEE_APPSBTAPP_RES_FILE, IDS_L2, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
  }

  ISHELL_LoadResString( pMe->a.m_pIShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_BYTES_SENT, 
                        wBuf1, sizeof( wBuf1 ) );

  WSPRINTF( &pMe->pText1[ uLen], 
            LONG_TEXT_BUF_LEN - uLen,
            wBuf1,
            pMe->mL2.uCliTotalBytesSent );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( &pMe->pText1[uLen], wBuf1, LONG_TEXT_BUF_LEN - uLen );
  uLen += WSTRLEN( pMe->pText1 );

  ISHELL_LoadResString( pMe->a.m_pIShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_KBITS_PER_SEC,
                        wBuf1, sizeof( wBuf1 ) );
  WSPRINTF( &pMe->pText1[ uLen], 
            LONG_TEXT_BUF_LEN - uLen,
            wBuf1,
            (pMe->mL2.uCliTotalBytesSent * 8) / (GETUPTIMEMS() - pMe->mL2.uCliStartTime));
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( &pMe->pText1[uLen], wBuf1, LONG_TEXT_BUF_LEN - uLen );
  uLen += WSTRLEN( pMe->pText1 );


  // display text
  ISTATIC_SetText( pMe->m_pStatic, pMe->pText2, pMe->pText1, 
                   AEE_FONT_BOLD, AEE_FONT_NORMAL );
  ISTATIC_Redraw( pMe->m_pStatic );

  if ( TOP_MENU != BT_APP_MENU_L2_RESULTS )
  {
    PUSH_MENU( BT_APP_MENU_L2_RESULTS );
  }

  ISHELL_SetTimer( pMe->a.m_pIShell, 
                   ONE_SECOND, 
                   (PFNNOTIFY) BTApp_BuildL2ResultsMenu, pMe );
  return built;
}

/* ==========================================================================
FUNCTION BTApp_HandleL2ResultsMenu
DESCRIPTION
============================================================================= */
boolean BTApp_HandleL2ResultsMenu( CBTApp* pMe, AEEEvent evt )
{
  boolean ev_processed = TRUE;

  MSG_MED( "HndlL2Test - k=0x%x", evt, 0, 0 );

  switch ( evt )
  {
    case AVK_CLR:
      if( pMe->mL2.pCliO != NULL )
      {
        IBTEXTL2_Disconnect( pMe->mL2.pCliO );
      }
      ISHELL_CancelTimer( pMe->a.m_pIShell,
                          (PFNNOTIFY) BTApp_BuildL2ResultsMenu,
                          pMe );
      BTApp_HandleClearKey( pMe );
      break;

    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_UP:
    case AVK_DOWN:
      break;  // do nothing

    default:
      ev_processed = FALSE;
      break;
  }
  return ev_processed;  
}

/* ==========================================================================
FUNCTION BTApp_BuildL2TestMenu
DESCRIPTION
============================================================================= */
boolean BTApp_BuildL2TestMenu( CBTApp* pMe )
{
  boolean built = TRUE;
  CtlAddItem ai;

  if ( BTApp_L2Init( pMe ) == FALSE )
  {
    MSG_ERROR( "L2BuildMenu - failed to create L2 object", 0, 0, 0 );
    BTApp_L2Cleanup( pMe );
    return FALSE;
  }

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_L2, NULL );

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mL2.bEnabled == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_ENABLE_SERVER, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISABLE_SERVER, 0 );
  }
  if ( pMe->mL2.bCliConnected == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT_CLIENT, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT_CLIENT, 0 );
  }

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SERVER_PSM, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CLIENT_PSM, 0 );

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SPP_DATA_SIZE_THOUSAND,
                     (pMe->mL2.uCliBytesToSend == 2000) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SPP_DATA_SIZE_MILLION,
                     (pMe->mL2.uCliBytesToSend == 2000000) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SPP_DATA_SIZE_BILLION,
                     (pMe->mL2.uCliBytesToSend == 2000000000) ?
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SECURITY,
                     pMe->mL2.bEnableSrvSvcSec ?
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SDP_QUERY,
                     pMe->mL2.bDoSDP ?
                     IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );

  // Activate menu
  if( TOP_MENU != BT_APP_MENU_L2_TEST )
  {
    PUSH_MENU( BT_APP_MENU_L2_TEST );
  }
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
  
  return built;
}

/* ==========================================================================
FUNCTION BTApp_HandleL2TestMenu
DESCRIPTION
============================================================================= */
boolean BTApp_HandleL2TestMenu( CBTApp* pMe, uint16 key )
{
  boolean       ev_processed = FALSE;
  uint16        selection;
  AEEBTUuidList svc_uuids;
  AEEBTUUID128  my_svc     = AEEBT_L2_UUID;
  
  svc_uuids.uNumUUIDs = 1;
  svc_uuids.aUUID[0]  = AEEBT_SD_PROTOCOL_L2CAP;
  
  svc_uuids.uNumUUID128s = 1;
  svc_uuids.aUUID128[0]       = my_svc;


  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key)) == 0 )
  {
    selection = IMENUCTL_GetSel( pMe->m_pIMenu );
  }

  MENU_SET_SEL( selection );
  MSG_MED( "HndlL2 - k=0x%x sel=%x", key, selection, 0 );

  if ( pMe->uCurrMsgId != 0 )
  {
    int rv;
    // If a message is up, clear it.
    rv = BTApp_HandleClearKey( pMe );
    BTApp_BuildL2TestMenu( pMe );
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
    case AVK_5:
    case AVK_6:
    case AVK_7:
    case AVK_8:
    case AVK_9:
    case AVK_SELECT:
    {
      switch ( selection )
      {
        case IDS_ENABLE_SERVER:
          BTApp_L2Enable( pMe );
          BTApp_BuildMenu( pMe, TOP_MENU );
          break;
        case IDS_DISABLE_SERVER:
          if( pMe->mL2.bSrvConnected == FALSE )
          {
            pMe->mL2.bEnabled = FALSE;
            IBTEXTSD_UnregisterCustomService( pMe->mSD.po,
                                              svc_uuids.aUUID128 );
            IBTEXTL2_Deregister( pMe->mL2.pSrvO );
            IBTEXTL2_Release( pMe->mL2.pSrvO );
            pMe->mL2.pSrvO = NULL;
          }
          BTApp_BuildMenu( pMe, TOP_MENU );
          break;
        case IDS_CONNECT_CLIENT:
          BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_ALL );
          break;
        case IDS_DISCONNECT_CLIENT:
          if( pMe->mL2.pCliO != NULL )
          {
            IBTEXTL2_Disconnect( pMe->mL2.pCliO );
          }
          break;
        case IDS_SERVER_PSM:
          if( pMe->mL2.bEnableSrvSvcSec == FALSE &&
              pMe->mL2.bEnabled == FALSE )
          {
            PUSH_MENU( BT_APP_MENU_L2_TEST );
            pMe->mL2.uDialogType = IDS_SERVER_PSM;
            ISHELL_CreateDialog( pMe->a.m_pIShell, AEE_APPSBTAPP_RES_FILE,  
                                 IDD_BT_TEXT_EDIT, NULL );
          }
          break;
        case IDS_CLIENT_PSM:
          if( pMe->mL2.bEnableCliSvcSec == FALSE )
          {
            PUSH_MENU( BT_APP_MENU_L2_TEST );
            pMe->mL2.uDialogType = IDS_CLIENT_PSM;
            ISHELL_CreateDialog( pMe->a.m_pIShell, AEE_APPSBTAPP_RES_FILE,  
                                 IDD_BT_TEXT_EDIT, NULL );
          }
          break;
        case IDS_SPP_DATA_SIZE_THOUSAND:
          pMe->mL2.uCliBytesToSend = 2000;
          BTApp_BuildMenu( pMe, TOP_MENU );
          pMe->bConfigChanged = TRUE;
          break;
        case IDS_SPP_DATA_SIZE_MILLION:
          pMe->mL2.uCliBytesToSend = 2000000;
          BTApp_BuildMenu( pMe, TOP_MENU );
          pMe->bConfigChanged = TRUE;
          break;
        case IDS_SPP_DATA_SIZE_BILLION:
          pMe->mL2.uCliBytesToSend = 2000000000;
          BTApp_BuildMenu( pMe, TOP_MENU );
          pMe->bConfigChanged = TRUE;
          break;
        case IDS_SECURITY:
        {
          pMe->mL2.dwSettingSec += 2;
          if( pMe->mL2.bEnableSrvSvcSec == FALSE )
          {
            BTApp_L2SetSecurity( pMe, AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT );
          }
          else
          {
            BTApp_L2SetSecurity( pMe, AEEBT_SEC_NONE );
          }
          BTApp_BuildMenu( pMe, TOP_MENU );
          pMe->bConfigChanged = TRUE;
          break;
        }
        case IDS_SDP_QUERY:
          if( pMe->mL2.bDoSDP == FALSE )
          {
            pMe->mL2.bDoSDP = TRUE;
          }
          else
          {
            pMe->mL2.bDoSDP = FALSE;
          }
          BTApp_BuildMenu( pMe, TOP_MENU );
          pMe->bConfigChanged = TRUE;
          break;
        default:
          MSG_ERROR( "HndlL2 - unexp sel=%x", selection, 0, 0 );
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
/* ==========================================================================
FUNCTION BTApp_EnableL2
DESCRIPTION
============================================================================= */
void BTApp_EnableL2( CBTApp* pMe )
{
  
  if ( pMe->mL2.bEnableL2Srv == TRUE )
  {
#ifdef FEATURE_BT_2_1
    if ( pMe->mL2.bSecSelected !=FALSE )
    {
      IBTEXTRM_SetSecByPSM( pMe->mRM.po, 
                            pMe->mL2.sSrvInfo.psm,
                            pMe->mL2.srvSecType,FALSE,FALSE );    
      if ( pMe->mL2.sCliInfo.psm != pMe->mL2.sSrvInfo.psm )
      {
        IBTEXTRM_SetSecByPSM( pMe->mRM.po, 
                              pMe->mL2.sCliInfo.psm,
                              pMe->mL2.srvSecType,
                              pMe->mL2.bAuthorize,
                              pMe->mL2.bAuthorizeFirst );
      }
    }
#endif //FEATURE_BT_2_1
    MSG_LOW( "EnableL2 - enabling L2", 0, 0, 0 );
    if ( BTApp_L2Init( pMe ) == FALSE )
    {
      MSG_ERROR( "EnableL2 - Enabling L2 failed", 
                 0, 0, 0 );
      BTApp_L2Cleanup( pMe );
    }
  }
}
#endif /* FEATURE_APP_BLUETOOTH */

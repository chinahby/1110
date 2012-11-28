/*===========================================================================

FILE:      OEMBTExtRM.c

SERVICES:  BlueTooth Radio Manager QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Radio Manager.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2003-2009 by QUALCOMM Incorporated. 
         All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtRM.c#5 $
$DateTime: 2009/07/10 11:47:34 $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
2009-06-11  ns  changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
2008-10-17  co  Replaced BT_SSR_LPC_DEBUG with FEATURE_BT_TEST_API.
2008-10-07  pn  Replaced deprecated functions.
2008-09-15  sp  Compiler warning fix

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT)
#include "bt.h"
#include "btsd.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExt.h"

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif

//==========================================================================
//   Macro definitions
//==========================================================================
#define SETTING_LOCAL_NAME      0x01
#define SETTING_LOCAL_COD       0x02
#define SETTING_LOCAL_SECURITY  0x04
#define ENABLING_AUTO_SRV_SRCH  0x08
#define DISABLING_AUTO_SRV_SRCH 0x10
#define SETTING_NICK_NAME       0x20
#define SETTING_MANUF_DATA      0x40

#define MAX_CONNECTIONS         4

//==========================================================================
//   Type definitions
//==========================================================================

typedef struct OEMBTExtRM_handleMap
{
  boolean           bInUse;
  bt_rm_handle_type handle;
  bt_bd_addr_type   bdAddr;
} OEMBTExtRM_handleMap_type;

typedef struct OEMBTExtRMobj_struct
{
  AEECLSID                cls;

  bt_app_id_type          appId;

  bt_app_id_type          testAppId[3];

  uint8                   localSetting;
  uint8                   uSetConnRoleIdxBitmap;

  // valid from Unbond to UNBONDED
  boolean                 bUnbonding;

  // valid from EnterTestMode to ExitTestMode
  boolean                 bInTestMode;

  // valid from DisableRadio to RADIO_STATUS
  boolean                 bDisablingRadio;

  // valid from DisableVisibility to VISIBILITY_STATUS
  boolean                 bDisablingVisibility;

  // valid from RegisterConnStatus to UnregisterConnStatus
  boolean                 bSendConnStatus;

  // user rebond authorization
  boolean                 bRebondAuthorized;
  AEEBTBDAddr             bdAddr;

  bt_rm_dev_iterator_type iterator;

  AEEBTLinkStatus         linkStatus;
  AEEBTDeviceUpdateStatus devUpdateStatus;
  AEEBTAuthorizeReq       authorizeReq;

  ACONTEXT*               m_pac;
  IBTExtNotifier*         m_pNotifier;

  AEECallback*            pRadioActivityCb;
  AEEBTRadioActivity*     pRadioActivity; /* points to app's mem */

  AEECallback*            pOOBDataCb;
  uint8*                  pOOBDataSize;

  struct OEMBTExtRMobj_struct* pNextRM;  // linked into mgr
} OEMBTExtRMobj_t;

typedef struct
{
  boolean                     bInUse;
  bt_event_type               event;
  bt_app_id_type              appId;
  uint32                      genericData;
  AEECallback                 cb;
} OEMBTExtRM_EvCb;

// one of these
typedef struct
{
  OEMBTExtRMobj_t*          pNextRM;    // singly linked list of RMs
  OEMBTExtRM_EvCb           FreeEvCb[ 10 ];
  bt_app_id_type            connStatusAppId;
  OEMBTExtRM_handleMap_type m_HandleMap[ MAX_CONNECTIONS ];
} OEMBTExtRM_Mgr;

// Used not for protecting data, but for task synchronisation
static rex_crit_sect_type OEMBTExtRM_crit_sect;

//==========================================================================
//   Function prototypes
//==========================================================================

int OEMBTExtRM_UnregisterRadioActivity( IBTExtRM* pParent );
uint32 OEMBTExtRM_Release( IBTExtRM* pParent );

// static helper functions
static int  OEMBTExtRM_CheckCmdStatus( bt_cmd_status_type stat );
static void OEMBTExtRM_Brew2CoreDev( const AEEBTDeviceInfo*, bt_device_type* );
static void OEMBTExtRM_Core2BrewDev( const bt_device_type*, AEEBTDeviceInfo*,
                                     bt_service_type* svcs, AEECLSID clsid );
static uint16 OEMBTExtRM_ConvertCmdStatus( bt_cmd_status_type cmd_status );
static void OEMBTExtRM_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);
static OEMBTExtRM_EvCb* OEMBTExtRM_GetFreeEvCb( void );
static void OEMBTExtRM_CancelCallback( AEECallback* pcb );
static void OEMBTExtRM_ConnStatusEventCallback( bt_ev_msg_type* bt_ev_msg_ptr );

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtRM_Mgr     gMgr;               // manager object

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtRM_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtRM *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtRM_Init(IBTExtRM* pParent)
{
  OEMBTExtRMobj_t* pMe;
  uint8 i = 0;

  // allocate pMe
  pMe = MALLOCREC(OEMBTExtRMobj_t);
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }
  // MALLOCREC returns zero-initialized memory
  pMe->appId = BT_APP_ID_NULL;

  if ( AEEHandle_To( &gOEMBTExtHandleList, (OEMINSTANCE*)pMe,
                     &pParent->m_hBT ) == FALSE )
  {
    FREE(pMe);
    return EFAILED;
  }

  if ( gMgr.pNextRM == NULL ) // first time?
  {
    uint8 i;
    for ( i = 0; i < ARR_SIZE( gMgr.FreeEvCb ); i++ )
    {
      gMgr.FreeEvCb[ i ].bInUse = FALSE;
    }
    for ( i = 0; i < ARR_SIZE( gMgr.m_HandleMap ); i++ )
    {
      gMgr.m_HandleMap[ i ].bInUse = FALSE;
    }
    gMgr.connStatusAppId = bt_cmd_ec_get_application_id( );
    if ( gMgr.connStatusAppId != BT_APP_ID_NULL )
    {
      bt_cmd_ec_reg_event_set_any_app_id( gMgr.connStatusAppId,
                                          OEMBTExtRM_ConnStatusEventCallback,
                                          BT_EC_ES_CUSTOM,
                                          BT_EV_RM_CONNECTED_ACL,
                                          BT_EV_RM_CONNECTED_ACL );
      bt_cmd_ec_reg_event_set_any_app_id( gMgr.connStatusAppId,
                                          OEMBTExtRM_ConnStatusEventCallback,
                                          BT_EC_ES_CUSTOM,
                                          BT_EV_RM_DISCONNECTED_ACL,
                                          BT_EV_RM_DISCONNECTED_ACL );
    }
    else
    {
      MSG_ERROR( "OEMBTExtRM_Init - Failed to get ConnStatusAppID", 0, 0, 0 );
    }
  }

  if ( ISHELL_CreateInstance( pParent->m_pIShell, AEECLSID_BLUETOOTH_NOTIFIER, 
                              (void**) &pMe->m_pNotifier ) != SUCCESS )
  {
    OEMBTExtRM_Release( pParent );
    return EFAILED;
  }

  // init BT layer
  pMe->appId = bt_cmd_ec_get_app_id_and_register(OEMBTExtRM_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    OEMBTExtRM_Release( pParent );
    return EFAILED;
  }

  pMe->m_pac   = AEE_GetAppContext();

  // link into mgr
  pMe->pNextRM = gMgr.pNextRM;
  gMgr.pNextRM = pMe;

  pMe->cls         = pParent->m_cls;

  for(i = 0 ; i < 3 ; i++)
  {
      pMe->testAppId[i] = bt_cmd_ec_get_app_id_and_register(OEMBTExtRM_EventCallback);
  }

  pMe->bRebondAuthorized = TRUE; // Initializing to TRUE
  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtRM_Release( IBTExtRM* pParent )
{
  OEMBTExtRMobj_t*  pMe;
  OEMBTExtRMobj_t** ppc = &gMgr.pNextRM;
  uint8 i = 0;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) == TRUE )
  {
    // unlink from mgr
    while ( *ppc != NULL )
    {
      if ((*ppc)->appId == pMe->appId)
      {
        *ppc = pMe->pNextRM;
        pMe->pNextRM = NULL;
        break;
      }
      ppc = &(*ppc)->pNextRM;
    }

    if ( (gMgr.pNextRM == NULL) &&  // last object being released?
         (gMgr.connStatusAppId != BT_APP_ID_NULL) )
    {
      bt_cmd_ec_unregister_event_set( gMgr.connStatusAppId, 
                                      BT_EC_ES_CUSTOM,
                                      BT_EV_GN_CMD_DONE,
                                      BT_EV_GN_CMD_DONE );

      bt_cmd_ec_unregister_event_set( gMgr.connStatusAppId, 
                                      BT_EC_ES_RADIO_MANAGER,
                                      BT_EV_NULL,
                                      BT_EV_NULL );

      (void)bt_cmd_ec_free_application_id( gMgr.connStatusAppId );
    }

    if ( pMe->pRadioActivityCb != NULL )
    {
      OEMBTExtRM_UnregisterRadioActivity( pParent );
    }

    if ( pMe->appId != BT_APP_ID_NULL )
    {
      (void)bt_cmd_ec_free_application_id( pMe->appId );
      pMe->appId = BT_APP_ID_NULL;
    }

    for(i = 0 ; i < 3 ; i++)
    {
      if ( pMe->testAppId[i] != BT_APP_ID_NULL )
      {
         (void) bt_cmd_ec_free_application_id( pMe->testAppId[i] );
         pMe->testAppId[i] = BT_APP_ID_NULL ;
      }
    }

    if ( pMe->m_pNotifier != NULL )
    {
      IBTEXTNOTIFIER_Release( pMe->m_pNotifier );
      pMe->m_pNotifier = NULL;
    }
    pMe->m_pac = NULL;

    FREE(pMe);
    (void) AEEHandle_Clear( &gOEMBTExtHandleList, pParent->m_hBT );

    return SUCCESS;
  }

  return EFAILED;
}

// IBTEXTRM_ConfigApplication (see AEEBTExtRM.h)
int OEMBTExtRM_ConfigApplication( 
  IBTExtRM*      pParent, 
  boolean        bRemoveAppConfig,
  AEEBTActVsPwr  pageScanActVsPwr,
  AEEBTActVsPwr  inqScanActVsPwr,
  boolean        bDeliverDevUpdates
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  stat = bt_cmd_rm_config_application( pMe->appId, bRemoveAppConfig,
                                       (bt_rm_act_vs_pwr_type)pageScanActVsPwr,
                                       (bt_rm_act_vs_pwr_type)inqScanActVsPwr,
                                       bDeliverDevUpdates );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}


// IBTEXTRM_AutoServiceSearchEnable (see AEEBTExtRM.h)
int OEMBTExtRM_AutoServiceSearchEnable( IBTExtRM*  pParent )
{
  bt_cmd_status_type  stat;
  int                 result;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pMe->localSetting & (ENABLING_AUTO_SRV_SRCH | DISABLING_AUTO_SRV_SRCH) )
  {
    // an enable or disable operation is in progress
    return EALREADY; 
  }

  pMe->localSetting |= ENABLING_AUTO_SRV_SRCH;
  
  stat = bt_cmd_rm_auto_service_search_disable( pMe->appId, FALSE );

  result = OEMBTExtRM_CheckCmdStatus( stat );

  if ( result != SUCCESS )
  {
    pMe->localSetting &= ~ENABLING_AUTO_SRV_SRCH;
  }

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}

// IBTEXTRM_AutoServiceSearchDisable (see AEEBTExtRM.h)
int OEMBTExtRM_AutoServiceSearchDisable( IBTExtRM*  pParent )
{
  bt_cmd_status_type  stat;
  int                 result;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pMe->localSetting & (ENABLING_AUTO_SRV_SRCH | DISABLING_AUTO_SRV_SRCH) )
  {
    // an enable or disable operation is in progress
    return EALREADY; 
  }

  pMe->localSetting |= DISABLING_AUTO_SRV_SRCH;
  
  stat = bt_cmd_rm_auto_service_search_disable( pMe->appId, TRUE );

  result = OEMBTExtRM_CheckCmdStatus( stat );

  if ( result != SUCCESS )
  {
    pMe->localSetting &= ~DISABLING_AUTO_SRV_SRCH;
  }

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}

// IBTEXTRM_GetLocalInfo (see AEEBTExtRM.h)
int OEMBTExtRM_GetLocalInfo( 
  IBTExtRM*         pParent,
  AEEBTLocalInfo*   pLocalInfo
)
{
  bt_lmp_features_type  lmp_features;
  bt_version_type       bt_version_info;
  bt_cod_type           class_of_device;
  char                  bt_name[ BT_MAX_NAME_LEN + 1 ];
  OEMBTExtRMobj_t*      pMe;
#ifdef FEATURE_BT_2_1
  char                  bt_short_name[ BT_MAX_EIR_NAME_LEN + 1 ];
#endif

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pLocalInfo == NULL )
  {
    return EBADPARM;
  }
  /*lint -save -e613*/
#ifdef FEATURE_BT_2_1
  bt_cmd_rm_get_local_info_ext( pMe->appId,
                                (bt_bd_addr_type*) &pLocalInfo->bdAddr,
                                &bt_version_info,
                                &lmp_features, &class_of_device, bt_name, 
                                (bt_security_type*)&pLocalInfo->security,
                                bt_short_name );
#else
  bt_cmd_rm_get_local_info( pMe->appId,
                            (bt_bd_addr_type*) &pLocalInfo->bdAddr,
                            &bt_version_info,
                            &lmp_features, &class_of_device, bt_name, 
                            (bt_security_type*)&pLocalInfo->security );
#endif /* FEATURE_BT_2_1 */
  
  bt_sd_parse_class_of_device( &class_of_device, 
                               (uint16*)&pLocalInfo->svcCls, 
                               (uint16*)&pLocalInfo->majorDevCls,
                               (uint16*)&pLocalInfo->minorDevCls );
  pLocalInfo->uLMPVersion     = bt_version_info.lmp_version;
  pLocalInfo->uCompanyID      = bt_version_info.manufacturer_name;
  pLocalInfo->uLMPSubVersion  = bt_version_info.lmp_subversion;
  pLocalInfo->uLMPFeatures[0] = lmp_features.lmp_features_bytes[0];
  pLocalInfo->uLMPFeatures[1] = lmp_features.lmp_features_bytes[1];
  pLocalInfo->uLMPFeatures[2] = lmp_features.lmp_features_bytes[2];

  AEEBT_TO_WSTR( bt_name, pLocalInfo->wName, sizeof( pLocalInfo->wName ) );
  /*lint -restore*/
#ifdef FEATURE_BT_2_1
  if ( pMe->cls == AEECLSID_BLUETOOTH_RM )
  {
    AEEBT_TO_WSTR( bt_short_name, pLocalInfo->wShortName, 
                   sizeof( pLocalInfo->wShortName ) );
  }
#endif /* FEATURE_BT_2_1 */
  return SUCCESS;
}

// IBTEXTRM_SetName (see AEEBTExtRM.h)
int OEMBTExtRM_SetName( IBTExtRM* pParent, const AECHAR* pName )
{
  bt_cmd_status_type stat;
  char               bt_name[ AEEBT_MAX_DEVICENAME_LEN + 1 ];
  int                result;
  OEMBTExtRMobj_t*   pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( (pName == NULL) || (WSTRLEN( pName ) > sizeof( bt_name )) )
  {
    return EBADPARM;
  }

  AEEBT_FROM_WSTR( pName, bt_name, sizeof( bt_name ) );
  pMe->localSetting |= SETTING_LOCAL_NAME;
#ifdef FEATURE_BT_2_1
  stat = bt_cmd_rm_set_local_info_ext( pMe->appId, NULL, bt_name, NULL );
#else
  stat = bt_cmd_rm_set_local_info( pMe->appId, NULL, bt_name );
#endif /* FEATURE_BT_2_1 */
  
  result = OEMBTExtRM_CheckCmdStatus( stat );
  if ( result != SUCCESS )
  {
    pMe->localSetting &= ~SETTING_LOCAL_NAME;
  }

  return result;
}


// IBTEXTRM_SetClassOfDevice (see AEEBTExtRM.h)
int OEMBTExtRM_SetClassOfDevice( 
  IBTExtRM*             pParent, 
  AEEBTServiceClass     svcCls,
  AEEBTMajorDeviceClass majDevCls,
  AEEBTMinorDeviceClass minDevCls
)
{
  bt_cmd_status_type  stat;
  bt_cod_type         cod;
  OEMBTExtRMobj_t*    pMe;
  int                 result;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  bt_sd_build_class_of_device( svcCls, majDevCls, minDevCls, &cod );
  pMe->localSetting |= SETTING_LOCAL_COD;
#ifdef FEATURE_BT_2_1
  stat = bt_cmd_rm_set_local_info_ext( pMe->appId, &cod, NULL, NULL );
#else
  stat = bt_cmd_rm_set_local_info( pMe->appId, &cod, NULL );
#endif /* FEATURE_BT_2_1 */
  result = OEMBTExtRM_CheckCmdStatus( stat );
  if ( result != SUCCESS )
  {
    pMe->localSetting &= ~SETTING_LOCAL_COD;
  }

  return result;
}


// IBTEXTRM_SetSecurity (see AEEBTExtRM.h)
int OEMBTExtRM_SetSecurity( IBTExtRM* pParent, AEEBTSecurity sec )
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;
  int                 result;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  pMe->localSetting |= SETTING_LOCAL_SECURITY;
  
  stat = bt_cmd_rm_set_device_security( pMe->appId, NULL, 
                                        (bt_security_type) sec );
  
  result = OEMBTExtRM_CheckCmdStatus( stat );
  
  if ( result != SUCCESS )
  {
    pMe->localSetting &= ~SETTING_LOCAL_SECURITY;
  }

  return result;
}
// IBTEXTRM_BondCancel(see AEEBTExtRM.h)
int OEMBTExtRM_BondCancel( IBTExtRM*           pParent,
                           const AEEBTBDAddr*  pBDAddr)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;
  int                 result;

  if ( (AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE ) || (pBDAddr == NULL) )
  {
    return EBADPARM;
  }
  stat = bt_cmd_rm_bond_cancel( pMe->appId, (bt_bd_addr_type*)pBDAddr);
  result = OEMBTExtRM_CheckCmdStatus( stat );
  if ( result == SUCCESS )
  {
    /*lint -save -e613*/
    pMe->devUpdateStatus.bdAddr = *pBDAddr;
    /*lint -restore*/
  }
  MSG_HIGH( "OEMBTExtRM_BondCancel - ret=%x", result, 0, 0 );
  return result;
}

// IBTEXTRM_Bond (see AEEBTExtRM.h)
int OEMBTExtRM_Bond( 
  IBTExtRM*           pParent, 
  const AEEBTBDAddr*  pBDAddr, 
  const AECHAR*       pPinCode
)
{
  bt_cmd_status_type  stat;
  bt_pin_code_type    pin;
  uint8               i;
  OEMBTExtRMobj_t*    pMe;
  int                 result;

  DBGPRINTF("***zzg OEMBTExtRM_Bond Start***");

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  pin.length = WSTRLEN( pPinCode );
  
  if ( (pBDAddr == NULL) || (pPinCode == NULL) ||
       ! AEE_CHECK_PTR( "OEMBTExtRM_Bond", pPinCode, pin.length, FALSE ) ||
       (pin.length > AEEBT_MAX_PIN_CODE_LEN)
       )
  {
    MSG_ERROR( "OEMBTExtRM_Bond - pinLen=%d", pin.length, 0, 0 );
    return EBADPARM;
  }

  for ( i=0; i<BT_MAX_PIN_CODE_LEN; i++)
  {
    pin.pin_code[i] = 0;
  }
  AEEBT_FROM_WSTR( pPinCode, pin.pin_code, sizeof( pin.pin_code ) );

  DBGPRINTF("***zzg bt_cmd_rm_bond Start***");

#ifndef FEATURE_BT_2_1
  stat = bt_cmd_rm_bond( pMe->appId, (bt_bd_addr_type*)pBDAddr, &pin );
#else
  stat = bt_cmd_rm_bond_ext( pMe->appId, (bt_bd_addr_type*)pBDAddr, &pin, FALSE );
#endif /* FEATURE_BT_2_1 */
  
  result = OEMBTExtRM_CheckCmdStatus( stat );

  DBGPRINTF("***zzg bt_cmd_rm_bond result=%x,stat=%x***", result, stat);

  if ( result == SUCCESS )
  {
    /*lint -save -e613*/
    pMe->devUpdateStatus.bdAddr = *pBDAddr;
    /*lint -restore*/
  }

  return result;
}


// IBTEXTRM_Unbond (see AEEBTExtRM.h)
int OEMBTExtRM_Unbond( IBTExtRM* pParent, const AEEBTBDAddr* pBDAddr )
{
  bt_cmd_status_type          stat;
  bt_rm_dev_update_ctrl_type  update_bitmap;
  bt_device_type              device;
  int                         result;
  OEMBTExtRMobj_t*            pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }

  update_bitmap  = BT_RM_DU_UNBOND_B;
  /*lint -save -e613*/
  device.bd_addr = *((bt_bd_addr_type*)pBDAddr);
  /*lint -restore*/
  stat = bt_cmd_rm_device_update( pMe->appId, update_bitmap, &device );
  result = OEMBTExtRM_CheckCmdStatus( stat );

  if ( result == SUCCESS )
  {
    pMe->bUnbonding = TRUE;
    /*lint -save -e613*/
    pMe->devUpdateStatus.bdAddr = *pBDAddr;
    /*lint -restore*/
  }
  return result;
}

// IBTEXTSD_SetBondable (see AEEBTExtRM.h)
int OEMBTExtRM_SetBondable( IBTExtRM* pParent, boolean bondable )
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  stat = bt_cmd_rm_set_bondable( pMe->appId, bondable );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}

// IBTEXTSD_PinReply (see AEEBTExtRM.h)
int OEMBTExtRM_PinReply( 
  IBTExtRM*           pParent, 
  const AEEBTBDAddr*  pBDAddr, 
  const AECHAR*       pPinCode 
)
{
  bt_cmd_status_type  stat;
  bt_pin_code_type    pin;
  boolean             pin_valid = FALSE;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  pin.length = ( pPinCode == NULL ) ? 0 : WSTRLEN( pPinCode );
  if ( pin.length > AEEBT_MAX_PIN_CODE_LEN )
  {
    MSG_ERROR( "OEMBTExtRM_PinReply - pinLen=%d", pin.length, 0, 0 );
    return EBADPARM;
  }

  if ( (pPinCode != NULL) && (pin.length > 0) )
  {
    pin_valid = TRUE;
    AEEBT_FROM_WSTR( pPinCode, pin.pin_code, sizeof( pin.pin_code ) );
  }

  stat = bt_cmd_rm_pin_reply( pMe->appId, (bt_bd_addr_type*) pBDAddr, 
                              pin_valid, &pin );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}

// IBTEXTRM_RegisterLinkStatus (see AEEBTExtRM.h)
int OEMBTExtRM_RegisterLinkStatus(
  IBTExtRM*           pParent, 
  const AEEBTBDAddr*  pBDAddr, 
  uint16              uRSSIDelta
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_rm_register_link_status( pMe->appId, 
                                         (bt_bd_addr_type*) pBDAddr, 
                                         TRUE, TRUE, TRUE, TRUE, TRUE, 
                                         uRSSIDelta );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}


// IBTEXTRM_UnregisterLinkStatus (see AEEBTExtRM.h)
int OEMBTExtRM_UnregisterLinkStatus(
  IBTExtRM*           pParent, 
  const AEEBTBDAddr*  pBDAddr
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }
  stat = bt_cmd_rm_unregister_link_status( pMe->appId, 
                                           (bt_bd_addr_type*) pBDAddr );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}


// IBTEXTRM_GetLinkStatus (see AEEBTExtRM.h)
int OEMBTExtRM_GetLinkStatus(
  IBTExtRM*        pParent, 
  AEEBTLinkStatus* pLinkStatus
)
{
  bt_rm_link_status_type linkStat;
  OEMBTExtRMobj_t*       pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pLinkStatus == NULL )
  {
    return EBADPARM;
  }
  /*lint -save -e613*/
  linkStat.bd_addr = *(bt_bd_addr_type*)&pLinkStatus->bdAddr;
  if ( bt_cmd_rm_get_link_status( pMe->appId, 
                                  &linkStat ) == BT_CS_GN_SUCCESS )
  {
    pLinkStatus->updateBitmap    = (AEEBTLinkUpdateBitmap)linkStat.updates;
#ifdef FEATURE_BT_2_1
    if (linkStat.sec_level == BT_RM_SL_0_NONE) 
    {
        pLinkStatus->security        = AEEBT_SEC_NONE;
    }
    else
    {
        pLinkStatus->security        = AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT;
    }
#else
    pLinkStatus->security        = (AEEBTSecurity)linkStat.security;
#endif /* BT_SWDEV_2_1_SSP */
    pLinkStatus->linkMode        = (AEEBTLinkMode)linkStat.link_mode;
    pLinkStatus->uCurrIntervalMs = linkStat.cur_interval_ms;
    
    if ( linkStat.role == BT_ROLE_MASTER )
    {
      pLinkStatus->bMaster     = TRUE;
    }
    else
    {
      pLinkStatus->bMaster     = FALSE;
    }
    
    pLinkStatus->ACLPktTypes   = linkStat.pkt_types_acl;
    pLinkStatus->SCOPktTypes   = linkStat.pkt_types_sco;
    pLinkStatus->rssi          = linkStat.rssi;
  }
  else
  {
    return EFAILED;
  }
  /*lint -restore*/
  return SUCCESS;
}


// IBTEXTRM_GetConnectionRole (see AEEBTExtRM.h)
int OEMBTExtRM_GetConnectionRole( 
  IBTExtRM*           pParent, 
  const AEEBTBDAddr*  pBDAddr,
  boolean*            pbMaster
)
{
  bt_cmd_status_type  stat;
  bt_role_type        role;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  // NULL BD address is valid
  stat = bt_cmd_rm_get_connection_role( 
           pMe->appId, (bt_bd_addr_type*) pBDAddr, &role );

  *pbMaster = (role == BT_ROLE_MASTER) ? TRUE : FALSE;

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}


// IBTEXTRM_SetConnectionRole (see AEEBTExtRM.h)
int OEMBTExtRM_SetConnectionRole( 
  IBTExtRM*           pParent, 
  const AEEBTBDAddr*  pBDAddr,
  boolean             bMaster
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;
  uint8               i;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  // NULL BD address is valid
  if ( pBDAddr != NULL )
  {
    for( i = 0; i < MAX_CONNECTIONS; i++ )
    {
      if ( (gMgr.m_HandleMap[ i ].bInUse != FALSE) &&
           (AEEBT_BD_ADDRS_EQUAL( &gMgr.m_HandleMap[i].bdAddr,
                                  pBDAddr ) != FALSE ) )
      {
        if ( pMe->uSetConnRoleIdxBitmap & (1 << i) )
        {
          // set connection role in progress
          return EALREADY;
        }
        else
        {
          pMe->uSetConnRoleIdxBitmap |= (1 << i);
        }
        break;
      }
    }
    if ( i >= MAX_CONNECTIONS )
    {
      // not connected to given BD address
      return ENOSUCH;
    }
  }

  stat = bt_cmd_rm_set_connection_role( 
           pMe->appId, (bt_bd_addr_type*) pBDAddr,
           bMaster ? BT_ROLE_MASTER : BT_ROLE_SLAVE );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}


// IBTEXTRM_SetConnectionSecurity (see AEEBTExtRM.h)
int OEMBTExtRM_SetConnectionSecurity(
  IBTExtRM*          pParent, 
  const AEEBTBDAddr* pBDAddr, 
  AEEBTSecurity      security
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }
  stat = bt_cmd_rm_set_connection_security( pMe->appId, 
                                            (bt_bd_addr_type*) pBDAddr, 
                                            (bt_security_type) security );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}


// IBTEXTRM_SetDeviceSecurity (see AEEBTExtRM.h)
int OEMBTExtRM_SetDeviceSecurity(
  IBTExtRM*          pParent, 
  const AEEBTBDAddr* pBDAddr, 
  AEEBTSecurity      security
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;
  int                 result;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }
  stat = bt_cmd_rm_set_device_security( pMe->appId, 
                                        (bt_bd_addr_type*) pBDAddr, 
                                        (bt_security_type) security );
  result = OEMBTExtRM_CheckCmdStatus( stat );

  if ( result == SUCCESS )
  {
    pMe->devUpdateStatus.bdAddr = *pBDAddr;
  }
  return result;
}


// IBTEXTRM_DeviceAdd (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceAdd( 
  IBTExtRM*              pParent, 
  const AEEBTDeviceInfo* pDevice 
)
{
  bt_cmd_status_type  stat;
  bt_device_type      device;
  OEMBTExtRMobj_t*    pMe;
  int                 result;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pDevice == NULL )
  {
    return EBADPARM;
  }

  device.bd_addr = *((bt_bd_addr_type*)&pDevice->bdAddr);
  device.valid   = FALSE;
  
  if ( pDevice->bValid )
  {
    OEMBTExtRM_Brew2CoreDev( pDevice, &device );
  }

  stat = bt_cmd_rm_device_add( pMe->appId, &device );
  result = OEMBTExtRM_CheckCmdStatus( stat );

  if ( result == SUCCESS )
  {
    pMe->devUpdateStatus.bdAddr = pDevice->bdAddr;
  }

  return result;
}


// IBTEXTRM_DeviceUpdate (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceUpdate( 
  IBTExtRM*               pParent, 
  AEEBTDeviceUpdateBitmap updateBitmap,
  const AEEBTDeviceInfo*  pDevice
)
{
  bt_cmd_status_type  stat;
  bt_device_type      device;
  OEMBTExtRMobj_t*    pMe;
  int                 result;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pDevice == NULL )
  {
    return EBADPARM;
  }

  device.bd_addr = *((bt_bd_addr_type*)&pDevice->bdAddr);
  if ( updateBitmap & AEEBT_RM_DU_SECURITY_B )
  {
    device.security = (bt_security_type) pDevice->security;
  }
  if ( updateBitmap & AEEBT_RM_DU_NICK_NAME_B )
  {
    AEEBT_FROM_WSTR( pDevice->wNickName, device.nick_name_str, 
                     sizeof( device.nick_name_str ) );
  }
  if ( updateBitmap & AEEBT_RM_DU_VALUE_1_B )
  {
    device.value_1 = pDevice->uValue1;
  }
  if ( updateBitmap & AEEBT_RM_DU_VALUE_2_B )
  {
    device.value_2 = pDevice->uValue2;
  }
  if ( updateBitmap & AEEBT_RM_DU_VALUE_3_B )
  {
    device.value_3 = pDevice->uValue3;
  }
  if ( updateBitmap & AEEBT_RM_DU_VALUE_4_B )
  {
    device.value_4 = pDevice->uValue4;
  }

  stat = bt_cmd_rm_device_update( pMe->appId, 
                                  (bt_rm_dev_update_ctrl_type) updateBitmap, 
                                  &device );
  result = OEMBTExtRM_CheckCmdStatus( stat );

  if ( result == SUCCESS )
  {
    pMe->devUpdateStatus.bdAddr = pDevice->bdAddr;
  }
  return result;
}


// IBTEXTRM_DeviceServicesUpdate (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceServicesUpdate( 
  IBTExtRM*                 pParent, 
  const AEEBTBDAddr*        pBDAddr,
  uint16                    uNumServices,
  const AEEBTSimpleService* pServices
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;
  bt_service_type     services[ AEEBT_MAX_NUM_OF_SRV_REC ];
  uint16              i;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( (pBDAddr == NULL) || (pServices == NULL) )
  {
    return EBADPARM;
  }
  for ( i=0; i<uNumServices && i<AEEBT_MAX_NUM_OF_SRV_REC; i++ )
  {
    services[ i ].service_class         = pServices[ i ].uServiceClass;
    services[ i ].profile_version       = pServices[ i ].uProfileVersion;
    services[ i ].server_channel_number = pServices[ i ].uServerChannel;
  }
  stat = bt_cmd_rm_device_services_update( pMe->appId, 
                                           (bt_bd_addr_type*) pBDAddr,
                                           uNumServices, services );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}



// IBTEXTRM_DeviceRead (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceRead( 
  IBTExtRM*           pParent, 
  AEEBTDeviceInfo*    pDevice
)
{
  bt_cmd_status_type  stat;
  bt_device_type      device;
  bt_service_type     svcs[ AEEBT_MAX_NUM_OF_SRV_REC ];
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pDevice == NULL )
  {
    return EBADPARM;
  }
  device.bd_addr = *((bt_bd_addr_type*) &pDevice->bdAddr);
  pDevice->uNumSvcs = AEEBT_MAX_NUM_OF_SRV_REC;
  pDevice->bValid   = FALSE;
  stat = bt_cmd_rm_device_read( pMe->appId, &device, &pDevice->uNumSvcs,
                                svcs );
  if ( stat == BT_CS_GN_SUCCESS )
  {
    OEMBTExtRM_Core2BrewDev( &device, pDevice, svcs, pMe->cls );
  }
  else
  {
    return EBADPARM;
  }
  return SUCCESS;
}


// IBTEXTRM_DeviceEnumInit (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceEnumInit( 
  IBTExtRM*                     pParent, 
  const AEEBTDeviceEnumerator*  pEnum
)
{
  OEMBTExtRMobj_t* pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pEnum == NULL )
  {
    return EBADPARM;
  }

  pMe->iterator.reset = TRUE;
  pMe->iterator.control = (bt_rm_iter_ctrl_enum_type)pEnum->control;
  switch ( pEnum->control )
  {
    case AEEBT_RM_EC_ALL:
      break;  // no filtering
    case AEEBT_RM_EC_MATCH_SERVICE_CLASS:
      pMe->iterator.service_class = 
        (bt_service_class_enum_type) pEnum->svcCls;
      break;
    case AEEBT_RM_EC_MATCH_BONDED:
      pMe->iterator.bonded = pEnum->bBonded;
      break;
    case AEEBT_RM_EC_MATCH_BD_ADDR_MASK:
      pMe->iterator.bd_addr_mask  = *((bt_bd_addr_type*)&pEnum->bdAddrMask);
      pMe->iterator.bd_addr_value = *((bt_bd_addr_type*)&pEnum->bdAddrValue);
      break;
    case AEEBT_RM_EC_MATCH_VALUE_1:
      pMe->iterator.value_1 = pEnum->uValue1;
      break;
    case AEEBT_RM_EC_MATCH_VALUE_2:
      pMe->iterator.value_2 = pEnum->uValue2;
      break;
    case AEEBT_RM_EC_MATCH_VALUE_3:
      pMe->iterator.value_3 = pEnum->uValue3;
      break;
    case AEEBT_RM_EC_MATCH_VALUE_4:
      pMe->iterator.value_4 = pEnum->uValue4;
      break;
    default:
      return EBADPARM;
  }
  return SUCCESS;
}

// IBTEXTRM_DeviceEnumNext (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceEnumNext( 
  IBTExtRM*             pParent, 
  AEEBTDeviceInfo*      pDevice
)
{
  bt_cmd_status_type  stat;
  bt_device_type      device;
  bt_service_type     svcs[ AEEBT_MAX_NUM_OF_SRV_REC ];
  OEMBTExtRMobj_t*    pMe;
  int                 result;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pDevice == NULL )
  {
    return EBADPARM;
  }

  pDevice->uNumSvcs = AEEBT_MAX_NUM_OF_SRV_REC;
  stat = bt_cmd_rm_device_read_iter( pMe->appId, &pMe->iterator, 
                                     &device, &pDevice->uNumSvcs,
                                     svcs );
  pMe->iterator.reset = FALSE;
  result = OEMBTExtRM_CheckCmdStatus( stat );

  if ( result == SUCCESS)
  {
    /*lint -save -e644*/
    if ( device.valid )
    {
      OEMBTExtRM_Core2BrewDev( &device, pDevice, svcs, pMe->cls );
    }
    else
    {
      pDevice->bValid = FALSE;
    }
    /*lint -restore*/
  }
  return result;
}

// IBTEXTRM_DeviceRemove (see AEEBTExtRM.h)
int OEMBTExtRM_DeviceRemove( IBTExtRM* pParent, const AEEBTBDAddr* pBDAddr )
{
  bt_cmd_status_type stat;
  OEMBTExtRMobj_t*   pMe;
  int                result;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_rm_device_remove( pMe->appId, (bt_bd_addr_type*)pBDAddr );
  result = OEMBTExtRM_CheckCmdStatus( stat );
  if ( result == SUCCESS )
  {
    pMe->devUpdateStatus.bdAddr = *pBDAddr;
  }
  return result;
}

// IBTEXTRM_SetHCIMode (see AEEBTExtRM.h)
int OEMBTExtRM_SetHCIMode( IBTExtRM* pParent, AEEBTHCIMode HCIMode )
{
  bt_cmd_status_type  stat;
  bt_hci_mode_type    mode;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if( ISHELL_CheckPrivLevel( pParent->m_pIShell, PL_SYSTEM, TRUE ) != TRUE )
  {
    return EPRIVLEVEL;
  }

  switch ( HCIMode )
  {
    case AEEBT_HCIM_OFF:
      mode = BT_HCIM_OFF;
      break;
    case AEEBT_HCIM_STANDARD_HCI_ON:      /*  Stand alone DSP image.  */
      mode = BT_HCIM_STANDARD_HCI_ON;
      break;
    case AEEBT_HCIM_STANDARD_HCI_VOC_ON:  /*  BT/vocoder DSP image.   */
      mode = BT_HCIM_STANDARD_HCI_VOC_ON;
      break;
    case AEEBT_HCIM_APIT_HCI_ON:          /*  BT/vocoder DSP image.   */
      mode = BT_HCIM_APIT_HCI_ON;
      break;
    case AEEBT_HCIM_APIT_HCI_SA_ON:       /*  Stand alone DSP image.  */
      mode = BT_HCIM_APIT_HCI_SA_ON;
      break;
#ifdef FEATURE_FACTORY_TESTMODE
    case AEEBT_HCIM_STANDARD_HCI_FTM:     /*  HCI via FTM.            */
      mode = BT_HCIM_STANDARD_HCI_FTM;
      break;
#endif
    default:
      return EBADPARM;
  }
  stat = bt_cmd_dc_set_hci_mode( mode );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}

// IBTEXTRM_GetHCIMode (see AEEBTExtRM.h)
int OEMBTExtRM_GetHCIMode( IBTExtRM* pParent, AEEBTHCIMode* pHCIMode )
{
  bt_hci_mode_type mode = bt_cmd_dc_get_hci_mode();
  OEMBTExtRMobj_t* pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pHCIMode == NULL )
  {
    return EBADPARM;
  }

  switch ( mode )
  {
    case BT_HCIM_OFF:
      *pHCIMode = AEEBT_HCIM_OFF;
      break;
    case BT_HCIM_STANDARD_HCI_ON:
      *pHCIMode = AEEBT_HCIM_STANDARD_HCI_ON;
      break;
    case BT_HCIM_STANDARD_HCI_VOC_ON:
      *pHCIMode = AEEBT_HCIM_STANDARD_HCI_VOC_ON;
      break;
    case BT_HCIM_APIT_HCI_ON:
      *pHCIMode = AEEBT_HCIM_APIT_HCI_ON;
      break;
    case BT_HCIM_APIT_HCI_SA_ON:
      *pHCIMode = AEEBT_HCIM_APIT_HCI_SA_ON;
      break;
#ifdef FEATURE_FACTORY_TESTMODE
    case BT_HCIM_STANDARD_HCI_FTM:
      *pHCIMode = AEEBT_HCIM_STANDARD_HCI_FTM;
      break;
#endif
    default:
      // print error message here
      MSG_ERROR( "Unknown HCI mode %x", mode, 0, 0 );
      *pHCIMode = AEEBT_HCIM_OFF;
      break;
  }

  return SUCCESS;
}


// IBTEXTRM_SetServiceSecurity (see AEEBTExtRM.h)
int OEMBTExtRM_SetServiceSecurity(
  IBTExtRM*                     pParent, 
  const AEEBTServiceIdentifier* pServiceId,
  AEEBTSecurity                 minSecurity, 
  boolean                       bAuthorize, 
  boolean                       bAuthorizeFirst
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;
  bt_service_id_type  service_ident;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pServiceId == NULL )
  {
    return EBADPARM;
  }

  service_ident.service_id_method = 
    (bt_serv_id_method_type) pServiceId->serviceIdMethod;
  switch ( service_ident.service_id_method )
  {
    case BT_SIM_SDP_UUID:
      service_ident.sdp_uuid = pServiceId->id.uServiceClass;
      break;
    case BT_SIM_RFCOMM_SCN:
      service_ident.rfcomm_scn = pServiceId->id.uServerChannel;
      break;
    case BT_SIM_L2CAP_PSM:
      service_ident.l2cap_psm = pServiceId->id.uPSM;
      break;
  }
  stat = bt_cmd_rm_set_service_security( pMe->appId, 
                                         &service_ident,
                                         (bt_security_type) minSecurity,
                                         bAuthorize, bAuthorizeFirst );

  return (OEMBTExtRM_CheckCmdStatus( stat ));
}

// IBTEXTRM_AuthorizeReply (see AEEBTExtRM.h)
int OEMBTExtRM_AuthorizeReply( 
  IBTExtRM*                     pParent, 
  const AEEBTServiceIdentifier* pServiceId,
  const AEEBTBDAddr*            pBDAddr,
  boolean                       bAuthorize
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;
  bt_service_id_type  service_ident;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( (pServiceId == NULL) || (pBDAddr == NULL) )
  {
    return EBADPARM;
  }

  service_ident.service_id_method = 
    (bt_serv_id_method_type) pServiceId->serviceIdMethod;
  switch ( service_ident.service_id_method )
  {
    case BT_SIM_SDP_UUID:
      service_ident.sdp_uuid = pServiceId->id.uServiceClass;
      break;
    case BT_SIM_RFCOMM_SCN:
      service_ident.rfcomm_scn = pServiceId->id.uServerChannel;
      break;
    case BT_SIM_L2CAP_PSM:
      service_ident.l2cap_psm = pServiceId->id.uPSM;
      break;
  }

  stat = bt_cmd_rm_authorize_reply( pMe->appId, &service_ident,
                                    (bt_bd_addr_type*)pBDAddr, bAuthorize );

  return (OEMBTExtRM_CheckCmdStatus( stat ));
}


// IBTEXTRM_EnterTestMode (see AEEBTExtRM.h)
int OEMBTExtRM_EnterTestMode( IBTExtRM* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if( ISHELL_CheckPrivLevel( pParent->m_pIShell, PL_SYSTEM, TRUE ) != TRUE )
  {
    return EPRIVLEVEL;
  }
  if ( pMe->bInTestMode != FALSE )
  {
    return EALREADY;
  }
  stat = bt_cmd_rm_test_mode_enable( pMe->appId, TRUE );

  return (OEMBTExtRM_CheckCmdStatus( stat ));
}


// IBTEXTRM_ExitTestMode (see AEEBTExtRM.h)
int OEMBTExtRM_ExitTestMode( IBTExtRM* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if( ISHELL_CheckPrivLevel( pParent->m_pIShell, PL_SYSTEM, TRUE ) != TRUE )
  {
    return EPRIVLEVEL;
  }
  if ( pMe->bInTestMode == FALSE )
  {
    return EALREADY;
  }
  stat = bt_cmd_rm_test_mode_enable( pMe->appId, FALSE );

  return (OEMBTExtRM_CheckCmdStatus( stat ));
}

// IBTEXTRM_GetBTOnOffState (see AEEBTExtRM.h)
int OEMBTExtRM_GetBTOnOffState( 
  IBTExtRM* pParent, 
  boolean*  pBTIsOn 
)
{
  OEMBTExtRMobj_t*    pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pBTIsOn == NULL )
  {
    return EBADPARM;
  }

  *pBTIsOn = bt_cmd_dc_is_bt_on();

  return SUCCESS;
}

// IBTEXTRM_GetRoleSwitchPolicy (see AEEBTExtRM.h)
int OEMBTExtRM_GetRoleSwitchPolicy( 
  IBTExtRM*           pParent, 
  const AEEBTBDAddr*  pBDAddr,
  boolean*            pbAllowed
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pbAllowed == NULL )
  {
    return EBADPARM;
  }

  // NULL BD address is valid
  stat = bt_cmd_rm_get_conn_role_sw_req_ok( 
           pMe->appId, (bt_bd_addr_type*) pBDAddr, pbAllowed );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}


// IBTEXTRM_AllowRoleSwitch
int OEMBTExtRM_AllowRoleSwitch( 
  IBTExtRM*          pParent, 
  const AEEBTBDAddr* pBDAddr,
  boolean            bAllow
)
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  // NULL BD Address is valid
  stat = bt_cmd_rm_set_conn_role_sw_req_ok( pMe->appId, 
                                            (bt_bd_addr_type*)pBDAddr, 
                                            bAllow );
  return (OEMBTExtRM_CheckCmdStatus( stat ));
}

// IBTEXTRM_DisableRadio
int OEMBTExtRM_DisableRadio( IBTExtRM* pParent, boolean bDisable )
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if( ISHELL_CheckPrivLevel( pParent->m_pIShell, PL_SYSTEM, TRUE ) != TRUE )
  {
    return EPRIVLEVEL;
  }
  pMe->bDisablingRadio = bDisable;

  TASKLOCK();
  stat = bt_cmd_rm_disable_radio( pMe->appId, bDisable );
  TASKFREE();

  return (OEMBTExtRM_CheckCmdStatus( stat ));
}

// IBTEXTRM_DisableVisibility
int OEMBTExtRM_DisableVisibility( IBTExtRM* pParent, boolean bDisable )
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if( ISHELL_CheckPrivLevel( pParent->m_pIShell, PL_SYSTEM, TRUE ) != TRUE )
  {
    return EPRIVLEVEL;
  }
  pMe->bDisablingVisibility = bDisable;

  TASKLOCK();
  stat = bt_cmd_rm_disable_discoverability( pMe->appId, bDisable );
  TASKFREE();

  return (OEMBTExtRM_CheckCmdStatus( stat ));
}

// IBTEXTRM_RegisterConnStatus
int OEMBTExtRM_RegisterConnStatus( IBTExtRM* pParent )
{
  OEMBTExtRMobj_t*    pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  pMe->bSendConnStatus = TRUE;

  return SUCCESS;

}

// IBTEXTRM_UnregisterConnStatus
int OEMBTExtRM_UnregisterConnStatus( IBTExtRM* pParent )
{
  OEMBTExtRMobj_t*    pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  pMe->bSendConnStatus = FALSE;

  return SUCCESS;
}

// IBTEXTRM_RegisterRadioActivity (see AEEBTExtRM.h)
int OEMBTExtRM_RegisterRadioActivity( 
  IBTExtRM*           pParent, 
  AEEBTRadioActivity* pRadioAct,
  AEECallback*        pCb )
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;
  int                 result;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( (pRadioAct == NULL) ||
       (!AEE_CHECK_PTR( "OEMBTExtRM_RegisterRadioStatus", pRadioAct, 
                        sizeof(AEEBTRadioActivity), TRUE)) )
  {
    return EBADPARM;
  }

  if ( (pCb == NULL) ||
       (!AEE_CHECK_PTR( "OEMBTExtRM_RegisterRadioStatus", pCb, 
                        sizeof(AEECallback), TRUE)) ||
       (pCb->pfnNotify == NULL) )
  {
    return EBADPARM;
  }

  stat = bt_cmd_rm_register_radio_activity( pMe->appId );

  result = OEMBTExtRM_CheckCmdStatus( stat );

  if ( result == SUCCESS )
  {
    pMe->pRadioActivity   = pRadioAct;
    // queue callback
    //pCb->pCancelData      = &pMe->pRadioActivityCb;
    //pCb->pfnCancel        = OEMBTExtRM_CancelCallback;
    //pCb->pNext            = pMe->pRadioActivityCb;
    pMe->pRadioActivityCb = pCb;
  }
  return ( result );
}

// IBTEXTRM_UnegisterRadioActivity (see AEEBTExtRM.h)
int OEMBTExtRM_UnregisterRadioActivity( IBTExtRM* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtRMobj_t*    pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pMe->pRadioActivityCb != NULL )
  {
    CALLBACK_Cancel( pMe->pRadioActivityCb );
    pMe->pRadioActivityCb = NULL;
    pMe->pRadioActivity   = NULL;
  }

  stat = bt_cmd_rm_unregister_radio_activity( pMe->appId );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}

// IBTEXTRM_GetRadioStatus (see AEEBTExtRM.h)
int OEMBTExtRM_GetRadioActivity( 
  IBTExtRM*           pParent, 
  AEEBTRadioActivity* pRadioActivity )
{
  OEMBTExtRMobj_t*        pMe;
  int                     result;
  bt_cmd_status_type      stat;
  bt_radio_activity_type  radio_activity;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  stat = bt_cmd_rm_get_radio_activity( pMe->appId, &radio_activity );

  result = OEMBTExtRM_CheckCmdStatus( stat );

  if ( result == SUCCESS )
  {
    *pRadioActivity = 0;
    if ( radio_activity & BT_RA_RADIO_OFF )
    {
      *pRadioActivity |= AEEBT_RA_RADIO_OFF;
    }
    if ( radio_activity & BT_RA_INQUIRING )
    {
      *pRadioActivity |= AEEBT_RA_INQUIRING;
    }
    if ( radio_activity & BT_RA_PAGING )
    {
      *pRadioActivity |= AEEBT_RA_PAGING;
    }
    if ( radio_activity & BT_RA_INQ_SCANNING )
    {
      *pRadioActivity |= AEEBT_RA_INQ_SCANNING;
    }
    if ( radio_activity & BT_RA_PAGE_SCANNING )
    {
      *pRadioActivity |= AEEBT_RA_PAGE_SCANNING;
    }
  }

  return ( result );
}


// IBTEXTRM_SetNickName
int OEMBTExtRM_SetNickName( 
  IBTExtRM*          pParent, 
  const AEEBTBDAddr* pBDAddr, 
  const AECHAR*      pwNickName)
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  int                 result;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pwNickName == NULL )
  {
    return EBADPARM;
  }
  if ( pBDAddr == NULL ) // local short names?
  {
    char szName[ BT_MAX_NAME_LEN ];

    AEEBT_FROM_WSTR( pwNickName, szName, sizeof( szName ) );
    SETBIT( pMe->localSetting, SETTING_NICK_NAME );

    stat = bt_cmd_rm_set_local_info_ext( pMe->appId, NULL, NULL, szName );
    result = OEMBTExtRM_CheckCmdStatus( stat );
    if ( result != SUCCESS )
    {
      CLRBIT( pMe->localSetting, SETTING_NICK_NAME );
    }
  }
  else // remote device's nick name
  {
    bt_device_type      device;
    device.bd_addr = *((bt_bd_addr_type*)pBDAddr);

    AEEBT_FROM_WSTR( pwNickName, device.nick_name_str, 
                     sizeof( device.nick_name_str ) );
    stat = bt_cmd_rm_device_update( pMe->appId, BT_RM_DU_NICK_NAME_B, &device );
    result = OEMBTExtRM_CheckCmdStatus( stat );
    if ( result == SUCCESS )
    {
      pMe->devUpdateStatus.bdAddr = *pBDAddr;
      pMe->devUpdateStatus.updateBitmap = AEEBT_RM_DUS_NICK_NAME_B;
    }
  }
  return result;
#endif
}

// IBTEXTRM_SetEIRManufData
int OEMBTExtRM_SetEIRManufData( 
  IBTExtRM*    pParent, 
  const uint8* pManufData,
  uint8        uSize )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( (pManufData == NULL) || (uSize == 0) )
  {
    return EBADPARM;
  }
  SETBIT( pMe->localSetting, SETTING_MANUF_DATA );

  stat = bt_cmd_rm_wr_eir_data( pMe->appId, BT_EIR_TYPE_MANUF_SPECIFIC_DATA, 
                                uSize, pManufData );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_GetEIRUUIDList
int OEMBTExtRM_GetEIRUUIDList(
  IBTExtRM*          pParent, 
  const AEEBTBDAddr* pBDAddr, 
  AEEBTUUIDType      uuidType,
  uint8              uuidListStartIdx,
  uint8*             pListSize,
  void*              pUUIDList )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( (pBDAddr == NULL) || (pUUIDList == NULL) )
  {
    return EBADPARM;
  }
  stat = bt_cmd_rm_get_uuid_list( pMe->appId, (bt_bd_addr_type*)pBDAddr,
                                  (bt_uuid_type)uuidType, uuidListStartIdx,
                                  pListSize, (uint16*)pUUIDList );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_GetEIRManufData
int OEMBTExtRM_GetEIRManufData(
  IBTExtRM*          pParent, 
  const AEEBTBDAddr* pBDAddr, 
  uint8*             pSize,
  uint8*             pManufData )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( (pManufData == NULL) || (*pSize == 0) )
  {
    return EBADPARM;
  }
  stat = bt_cmd_rm_get_manuf_data( pMe->appId, (bt_bd_addr_type*)pBDAddr,
                                   pSize, pManufData );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_SetIOCap
int OEMBTExtRM_SetIOCap( IBTExtRM* pParent, AEEBTIOCapType IOCap )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  bt_rm_io_cap_type   io_cap;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  switch ( IOCap )
  {
    case AEEBT_RM_IOC_DISPLAY_YES_NO:
      io_cap = BT_RM_IOC_DISPLAY_YES_NO;
      break;
    case AEEBT_RM_IOC_DISPLAY_ONLY:
      io_cap = BT_RM_IOC_DISPLAY_ONLY;
      break;
    case AEEBT_RM_IOC_KEYBOARD_ONLY:
      io_cap = BT_RM_IOC_KEYBOARD_ONLY;
      break;
    case AEEBT_RM_IOC_NO_IO:
      io_cap = BT_RM_IOC_NO_IO;
      break;
    default:
      return EBADPARM;
  }
  stat = bt_cmd_rm_set_io_cap( pMe->appId, io_cap );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}


// IBTEXTRM_SetSecBySvcCls
int OEMBTExtRM_SetSecBySvcCls(
  IBTExtRM*         pParent, 
  uint16            uServiceClass,
  AEEBTSvcSecLevel  minSecurity, 
  boolean           bAuthorize, 
  boolean           bAuthorizeFirst )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  bt_service_id_type  service_ident;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  service_ident.service_id_method = BT_SIM_SDP_UUID;
  service_ident.sdp_uuid = (bt_sd_uuid_type)uServiceClass;

  stat = bt_cmd_rm_set_sm4( pMe->appId, &service_ident,
                            (bt_sec_level_type) minSecurity,
                            bAuthorize, bAuthorizeFirst );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_SetSecBySrvrChnl
int OEMBTExtRM_SetSecBySrvrChnl(
  IBTExtRM*         pParent, 
  uint8             uSCN,
  AEEBTSvcSecLevel  minSecurity, 
  boolean           bAuthorize, 
  boolean           bAuthorizeFirst )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  bt_service_id_type  service_ident;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  service_ident.service_id_method = BT_SIM_RFCOMM_SCN;
  service_ident.rfcomm_scn = uSCN;

  stat = bt_cmd_rm_set_sm4( pMe->appId, &service_ident,
                            (bt_sec_level_type) minSecurity,
                            bAuthorize, bAuthorizeFirst );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_SetSecByPSM
int OEMBTExtRM_SetSecByPSM(
  IBTExtRM*         pParent, 
  uint16            uPSM,
  AEEBTSvcSecLevel  minSecurity, 
  boolean           bAuthorize, 
  boolean           bAuthorizeFirst )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  bt_service_id_type  service_ident;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  service_ident.service_id_method = BT_SIM_L2CAP_PSM;
  service_ident.l2cap_psm = uPSM;

  stat = bt_cmd_rm_set_sm4( pMe->appId, &service_ident,
                            (bt_sec_level_type) minSecurity,
                            bAuthorize, bAuthorizeFirst );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_SetRemoteOOBData
int OEMBTExtRM_SetRemoteOOBData(
  IBTExtRM*          pParent, 
  const uint8*       pOOBData,
  uint8              uSize )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }
  if ( pOOBData == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_rm_wr_remote_oob_data( pMe->appId, 
                                       uSize,
                                       pOOBData );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_UserConfirmationReply
int OEMBTExtRM_UserConfirmationReply(
  IBTExtRM*          pParent, 
  const AEEBTBDAddr* pBDAddr, 
  boolean            bConfirmed )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  stat = bt_cmd_rm_user_confirmation_reply( pMe->appId, 
                                            (bt_bd_addr_type*)pBDAddr,
                                            bConfirmed );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_KeypressNotification
int OEMBTExtRM_KeypressNotification(
  IBTExtRM*          pParent, 
  const AEEBTBDAddr* pBDAddr, 
  AEEBTKeypressType  keypress )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  bt_rm_keypress_notification_type kpn;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  switch ( keypress)
  {
    case AEEBT_RM_KPN_STARTED:
      kpn = BT_RM_KPN_STARTED;
      break;
    case AEEBT_RM_KPN_DIGIT_ENTERED:
      kpn = BT_RM_KPN_DIGIT_ENTERED;
      break;
    case AEEBT_RM_KPN_DIGIT_ERASED:
      kpn = BT_RM_KPN_DIGIT_ERASED;
      break;
    case AEEBT_RM_KPN_CLEARED:
      kpn = BT_RM_KPN_CLEARED;
      break;
    case AEEBT_RM_KPN_COMPLETED:
      kpn = BT_RM_KPN_COMPLETED;
      break;
    default:
      return EBADPARM;
  }

  stat = bt_cmd_rm_keypress_notification( pMe->appId, 
                                          (bt_bd_addr_type*)pBDAddr,
                                          kpn );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_PasskeyReply
int OEMBTExtRM_PasskeyReply(
  IBTExtRM*          pParent, 
  const AEEBTBDAddr* pBDAddr,
  const char passkey[ AEEBT_SSP_PASSKEY_LEN ]  )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  uint8 i;
  boolean bPasskeyValid = TRUE;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  for ( i=0; i<(AEEBT_SSP_PASSKEY_LEN-1); i++ )
  {
    if ( DECCHK( passkey[ i ] ) == FALSE )
    {
      bPasskeyValid = FALSE;
      break;
    }
  }

  stat = bt_cmd_rm_passkey_reply( pMe->appId, 
                                  (bt_bd_addr_type*)pBDAddr,
                                  passkey, bPasskeyValid );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_CreateOOBData
int OEMBTExtRM_CreateOOBData(
  IBTExtRM*     pParent, 
  uint8*        pOOBDataSize,
  AEECallback*  pCb )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pCb == NULL || pOOBDataSize == NULL )
  {
    return EBADPARM;
  }

  pMe->pOOBDataSize = pOOBDataSize;
  pMe->pOOBDataCb   = pCb;

  stat = bt_cmd_rm_create_local_oob_data( pMe->appId );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_ReadOOBData
int OEMBTExtRM_ReadOOBData(
  IBTExtRM*     pParent, 
  uint8*        pOOBDataSize,
  uint8*        pOOBData )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( (pOOBData == NULL) || (*pOOBDataSize == 0) )
  {
    return EBADPARM;
  }

  stat = bt_cmd_rm_rd_local_oob_data( pMe->appId, 
                                      pOOBDataSize, pOOBData );
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_AuthorizeRebond
int OEMBTExtRM_AuthorizeRebond(
  IBTExtRM*           pParent, 
  const AEEBTBDAddr*  pBDAddr,
  boolean             bAuthorized,
  AEEBTRebondSecurity rebondSec )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }
  pMe->bRebondAuthorized = bAuthorized;
  pMe->bdAddr = *pBDAddr;
  stat = bt_cmd_rm_authorize_rebond( pMe->appId, 
                                     (bt_bd_addr_type*)pBDAddr,
                                     bAuthorized,
                                     (bt_rm_authorize_bond_type) rebondSec );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_SSPBond
int OEMBTExtSSPBond(
  IBTExtRM*           pParent, 
  const AEEBTBDAddr*  pBDAddr,
  boolean             bMITMProtectionRequired )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  bt_pin_code_type    pin;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }

  pin.length = 0;

  stat = bt_cmd_rm_bond_ext( pMe->appId, (bt_bd_addr_type*)pBDAddr, &pin,
                             bMITMProtectionRequired );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

// IBTEXTRM_SetSM4DebugMode
int OEMBTExtRM_SetSM4DebugMode( IBTExtRM* pParent, boolean bEnabled )
{
#ifndef FEATURE_BT_2_1
  return EUNSUPPORTED;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  stat = bt_cmd_rm_set_sm4_debug_mode( pMe->appId, bEnabled );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif
}

int OEMBTExtRM_LowPowerConfig( 
  IBTExtRM*             pParent,
  const AEEBTBDAddr*    pBDAddr, 
  AEEBTLowPowerControl  lpc, 
  uint16                uMaxInterval,
  uint16                uIdleTO, 
  uint16                uMaxLatency, 
  boolean               bMustUse, 
  boolean               bSniffInitiatorOriented )
{
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  bt_low_power_ctrl_type  native_lpc;
  bt_sniff_ssr_ctrl_type  ssc;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }

  switch ( lpc )
  {
    case AEEBT_LPC_DONT_CARE:
      native_lpc = BT_LPC_DONT_CARE;
      break;
    case AEEBT_LPC_ACTIVE:
      native_lpc = BT_LPC_ACTIVE;
      break;
    case AEEBT_LPC_SNIFF:
      native_lpc = BT_LPC_SNIFF;
      break;
    case AEEBT_LPC_PARK:
      native_lpc = BT_LPC_PARK;
      break;
    case AEEBT_LPC_HOLD:
      native_lpc = BT_LPC_HOLD;
      break;
    case AEEBT_LPC_SNIFF_LOCK:
      native_lpc = BT_LPC_SNIFF_LOCK;
      break;
    default:
      return EBADPARM;
  }

  ssc.max_latency_ms = uMaxLatency;
  ssc.local_cid = ssc.server_channel = 0;
  ssc.stick_to_app_param = bMustUse;
  ssc.sniff_initiator_oriented = bSniffInitiatorOriented;

#ifdef FEATURE_BT_2_1
  stat = bt_cmd_rm_low_power_config_ext( 
           pMe->appId, 
           (bt_bd_addr_type*)pBDAddr,
           native_lpc,
           uMaxInterval,
           uIdleTO,
           &ssc );
#else
  stat = bt_cmd_rm_low_power_config( 
           pMe->appId, 
           (bt_bd_addr_type*)pBDAddr,
           native_lpc,
           uMaxInterval,
           uIdleTO );
#endif
  return( OEMBTExtRM_CheckCmdStatus( stat ) );
}

int OEMBTExtRM_LowPowerConfigTest( 
  IBTExtRM*             pParent,
  const AEEBTBDAddr*    pBDAddr, 
  AEEBTLowPowerControl  lpc, 
  uint16                uMaxInterval,
  uint16                uIdleTO, 
  uint16                uMaxLatency, 
  boolean               bMustUse, 
  boolean               bSniffInitiatorOriented, 
  uint8                 testId )
{

#if (!defined(FEATURE_BT_2_1) || !defined(FEATURE_BT_TEST_API))
  MSG_HIGH ("RM:SSR LPC Not supported", 0, 0, 0 );
  return EUNSUPPORTED ;
#else
  OEMBTExtRMobj_t*    pMe;
  bt_cmd_status_type  stat;
  bt_low_power_ctrl_type  native_lpc;
  bt_sniff_ssr_ctrl_type  ssc;

  MSG_HIGH ("RM:SSR LPC Test : testId=%d", testId, 0, 0 );

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }

  switch ( lpc )
  {
    case AEEBT_LPC_DONT_CARE:
      native_lpc = BT_LPC_DONT_CARE;
      break;
    case AEEBT_LPC_ACTIVE:
      native_lpc = BT_LPC_ACTIVE;
      break;
    case AEEBT_LPC_SNIFF:
      native_lpc = BT_LPC_SNIFF;
      break;
    case AEEBT_LPC_PARK:
      native_lpc = BT_LPC_PARK;
      break;
    case AEEBT_LPC_HOLD:
      native_lpc = BT_LPC_HOLD;
      break;
    case AEEBT_LPC_SNIFF_LOCK:
      native_lpc = BT_LPC_SNIFF_LOCK;
      break;
    default:
      return EBADPARM;
  }

  ssc.max_latency_ms = uMaxLatency;
  ssc.local_cid = ssc.server_channel = 0;
  ssc.stick_to_app_param = bMustUse;
  ssc.sniff_initiator_oriented = bSniffInitiatorOriented;

  stat = bt_cmd_rm_low_power_config_ext( 
           pMe->testAppId[testId], 
           (bt_bd_addr_type*)pBDAddr,
           native_lpc,
           uMaxInterval,
           uIdleTO,
           &ssc );

  return( OEMBTExtRM_CheckCmdStatus( stat ) );
#endif /* !FEATURE_BT_2_1 || !FEATURE_BT_TEST_API */
}

//==========================================================================
//   static helper functions
//==========================================================================
static void OEMBTExtRM_CancelCallback( AEECallback* pcb )
{
  AEECallback** ppcb;

  rex_enter_crit_sect( &OEMBTExtRM_crit_sect ); // do I need this?

  ppcb = pcb->pCancelData;
  //Verify that it is me set as the cancel function.
  //Cross-task cancellations need this check.
  if( pcb->pfnCancel == OEMBTExtRM_CancelCallback )
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

  rex_leave_crit_sect( &OEMBTExtRM_crit_sect );
}


static int OEMBTExtRM_CheckCmdStatus( bt_cmd_status_type stat )
{
  switch (stat)
  {
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return SUCCESS;
    case BT_CS_GN_CMD_Q_FULL:
      return ENOMEMORY;
#ifdef FEATURE_FACTORY_TESTMODE
    case BT_CS_DC_SIO_PORT_ALREADY_OPENED:
#endif
    case BT_CS_DC_NO_SIO_PORT_ID:
      return AEEBT_EPORT;
    case BT_CS_RM_UNRECOGNIZED_CMD:
      return EUNSUPPORTED;
    case BT_CS_RM_BAD_PARAM:
      return EBADPARM;
    case BT_CS_RM_NO_CONNECTION:
      return ENOSUCH;
    default:
      return EFAILED;
  }
}


static void OEMBTExtRM_Brew2CoreDev(
  const AEEBTDeviceInfo*  pSrcDev,
  bt_device_type*   pDstDev
)
{
#ifdef FEATURE_BT_2_1
  uint8 i;
#endif

  pDstDev->valid                              = TRUE;
  pDstDev->bd_addr                            = 
    *((bt_bd_addr_type*) &pSrcDev->bdAddr);
  pDstDev->lmp_features.lmp_features_bytes[0] = pSrcDev->uLMPFeatures[0];
  pDstDev->lmp_features.lmp_features_bytes[1] = pSrcDev->uLMPFeatures[1];
  pDstDev->lmp_features.lmp_features_bytes[2] = pSrcDev->uLMPFeatures[2];
  pDstDev->version.lmp_version                = pSrcDev->uLMPVersion;
  pDstDev->version.manufacturer_name          = pSrcDev->uManufacturerID;
  pDstDev->version.lmp_subversion             = pSrcDev->uLMPSubversion;
  pDstDev->security                           = 
    (bt_security_type) pSrcDev->security;
  pDstDev->bonded                             = pSrcDev->bBonded;
  pDstDev->link_key_type                      = 
    (bt_hc_link_key_type) pSrcDev->linkKeyType;

  pDstDev->value_1                            = pSrcDev->uValue1;
  pDstDev->value_2                            = pSrcDev->uValue2;
  pDstDev->value_3                            = pSrcDev->uValue3;
  pDstDev->value_4                            = pSrcDev->uValue4;

  pDstDev->service_class   = pSrcDev->serviceClass;
  pDstDev->major_dev_class = pSrcDev->majorDevClass;
  pDstDev->minor_dev_class = pSrcDev->minorDevClass;

#ifdef FEATURE_BT_2_1
  pDstDev->eir_data.eir_flag          = 0;
  pDstDev->eir_data.device_ranking    = pSrcDev->EIRData.deviceRanking;
  if ( ISBITSET( pSrcDev->EIRData.uFlags, AEEBT_EIR_DATA_RCVD_B ) != FALSE )
  {
    SETBIT( pDstDev->eir_data.eir_flag, BT_EIR_DATA_RCVD_B );
    if ( ISBITSET( pSrcDev->EIRData.uFlags, AEEBT_EIR_NAME_CMPLT_B ) != FALSE )
    {
      SETBIT( pDstDev->eir_data.eir_flag, BT_EIR_NAME_CMPLT_B );
    }
    if ( ISBITSET( pSrcDev->EIRData.uFlags, AEEBT_EIR_UUID16_LIST_CMPLT_B) != FALSE )
    {
      SETBIT( pDstDev->eir_data.eir_flag, BT_EIR_UUID16_LIST_CMPLT_B );
    }
    if ( ISBITSET( pSrcDev->EIRData.uFlags, AEEBT_EIR_UUID32_LIST_CMPLT_B) != FALSE )
    {
      SETBIT( pDstDev->eir_data.eir_flag, BT_EIR_UUID32_LIST_CMPLT_B );
    }
    if ( ISBITSET( pSrcDev->EIRData.uFlags, AEEBT_EIR_UUID128_LIST_CMPLT_B) != FALSE )
    {
      SETBIT( pDstDev->eir_data.eir_flag, BT_EIR_UUID128_LIST_CMPLT_B );
    }
    for (i=0; i<AEEBT_NUM_UUID_TYPES; i++)
    {
      pDstDev->eir_data.num_uuids[ i ]  = pSrcDev->EIRData.uNumUUIDs[ i ];
    }
    pDstDev->eir_data.manuf_data_size = pSrcDev->EIRData.uManufDataSize;
  }
  pDstDev->link_key_status = (bt_rm_lk_status_type)pSrcDev->linkKeyStatus;
  pDstDev->ssp_capable = pSrcDev->bSSPCapable;
#endif

  bt_sd_build_class_of_device( pSrcDev->serviceClass, 
                               pSrcDev->majorDevClass, 
                               pSrcDev->minorDevClass, 
                               &pDstDev->class_of_device );

  AEEBT_FROM_WSTR( pSrcDev->wName, pDstDev->name_str, 
                   sizeof( pDstDev->name_str ) );
  AEEBT_FROM_WSTR( pSrcDev->wNickName, pDstDev->nick_name_str,
                   sizeof( pDstDev->nick_name_str ) );
}

static void OEMBTExtRM_Core2BrewDev( 
  const bt_device_type*   pSrcDev,
  AEEBTDeviceInfo*        pDstDev,
  bt_service_type*        svcs,
  AEECLSID                clsid
)
{
  uint8 i;

  bt_sd_parse_class_of_device( &pSrcDev->class_of_device,
                               (uint16*)&pDstDev->serviceClass, 
                               (uint16*)&pDstDev->majorDevClass, 
                               (uint16*)&pDstDev->minorDevClass );

  pDstDev->bValid           = pSrcDev->valid;
  pDstDev->bdAddr           = *((AEEBTBDAddr*)&pSrcDev->bd_addr);
  pDstDev->uLMPFeatures[0]  = pSrcDev->lmp_features.lmp_features_bytes[0];
  pDstDev->uLMPFeatures[1]  = pSrcDev->lmp_features.lmp_features_bytes[1];
  pDstDev->uLMPFeatures[2]  = pSrcDev->lmp_features.lmp_features_bytes[2];
  /* TODO: need size of array */
  MEMCPY( pDstDev->uLMPFeaturesPg1, 
          (void*)(&pSrcDev->lmp_features_ext), 
          sizeof( bt_lmp_features_type ) );

  pDstDev->uLMPVersion      = pSrcDev->version.lmp_version;
  pDstDev->uManufacturerID  = pSrcDev->version.manufacturer_name;
  pDstDev->uLMPSubversion   = pSrcDev->version.lmp_subversion;
  pDstDev->security         = (AEEBTSecurity) pSrcDev->security;
  pDstDev->bBonded          = pSrcDev->bonded;
  pDstDev->linkKeyType      = (AEEBTLinkKeyType) pSrcDev->link_key_type;

  pDstDev->uValue1          = pSrcDev->value_1;
  pDstDev->uValue2          = pSrcDev->value_2;
  pDstDev->uValue3          = pSrcDev->value_3;
  pDstDev->uValue4          = pSrcDev->value_4;

  AEEBT_TO_WSTR( (char *)pSrcDev->name_str, pDstDev->wName, sizeof( pDstDev->wName ) );
  AEEBT_TO_WSTR( (char *)pSrcDev->nick_name_str, pDstDev->wNickName,
                 sizeof( pDstDev->wNickName ) );

  for ( i=0; i<pDstDev->uNumSvcs; i++ )
  {
    pDstDev->svcs[ i ].uServiceClass    = svcs[ i ].service_class;
    pDstDev->svcs[ i ].uProfileVersion  = svcs[ i ].profile_version;
    pDstDev->svcs[ i ].uServerChannel   = svcs[ i ].server_channel_number;
  }

#ifdef FEATURE_BT_2_1
  if ( clsid == AEECLSID_BLUETOOTH_RM )
  {
    pDstDev->EIRData.uFlags = 0;
    if ( ISBITSET( pSrcDev->eir_data.eir_flag, BT_EIR_DATA_RCVD_B) != FALSE )
    {
      SETBIT( pDstDev->EIRData.uFlags, AEEBT_EIR_DATA_RCVD_B );
      if ( ISBITSET( pSrcDev->eir_data.eir_flag, BT_EIR_NAME_CMPLT_B) != FALSE )
      {
        SETBIT( pDstDev->EIRData.uFlags, AEEBT_EIR_NAME_CMPLT_B );
      }
      if ( ISBITSET( pSrcDev->eir_data.eir_flag, 
                     BT_EIR_UUID16_LIST_CMPLT_B) != FALSE )
      {
        SETBIT( pDstDev->EIRData.uFlags, AEEBT_EIR_UUID16_LIST_CMPLT_B );
      }
      if ( ISBITSET( pSrcDev->eir_data.eir_flag, 
                     BT_EIR_UUID32_LIST_CMPLT_B) != FALSE )
      {
        SETBIT( pDstDev->EIRData.uFlags, AEEBT_EIR_UUID32_LIST_CMPLT_B );
      }
      if ( ISBITSET( pSrcDev->eir_data.eir_flag, 
                     BT_EIR_UUID128_LIST_CMPLT_B) != FALSE )
      {
        SETBIT( pDstDev->EIRData.uFlags, AEEBT_EIR_UUID128_LIST_CMPLT_B );
      }
      for (i=0; i<AEEBT_NUM_UUID_TYPES; i++)
      {
        pDstDev->EIRData.uNumUUIDs[ i ] = pSrcDev->eir_data.num_uuids[ i ];
      }
      pDstDev->EIRData.uManufDataSize = pSrcDev->eir_data.manuf_data_size;
    }
    pDstDev->EIRData.deviceRanking  = pSrcDev->eir_data.device_ranking;
  }
  pDstDev->linkKeyStatus      = (AEEBTLinkKeyStatus) pSrcDev->link_key_status;
  pDstDev->bSSPCapable        = pSrcDev->ssp_capable;
#endif /* FEATURE_BT_2_1 */
}

static OEMBTExtRMobj_t* OEMBTExtRM_FindMe(bt_app_id_type appId)
{
  OEMBTExtRMobj_t* pMe;

  for (pMe = gMgr.pNextRM; pMe != NULL; pMe = pMe->pNextRM)
  {
    if (pMe->appId == appId)
    {
      return pMe;
    }
  }
  return NULL;
}

static uint16 OEMBTExtRM_ConvertCmdStatus( bt_cmd_status_type cmd_status )
{
  switch ( cmd_status )
  {
    case BT_CS_GN_SUCCESS:
      return AEEBT_RM_ERR_NONE;
    case BT_CS_RM_BAD_PARAM:
      return AEEBT_RM_ERR_BAD_PARAM;
    case BT_CS_RM_MAX_DEVICES:
      return AEEBT_RM_ERR_DB_FULL;
    case BT_CS_RM_MAX_CONNECTIONS:
      return AEEBT_RM_ERR_MAX_CONN;
    case BT_CS_RM_NO_CONNECTION:
      return AEEBT_RM_ERR_NO_CONN;
    case BT_CS_RM_DEVICE_EXISTS:
      return AEEBT_RM_ERR_DEVICE_EXISTS;
    case BT_CS_RM_EFS_ERR:
      return AEEBT_RM_ERR_DB_WRITE_FAILED;
    case BT_CS_RM_NO_DEVICE:
      return AEEBT_RM_ERR_NO_DEVICE;
    case BT_CS_RM_CMD_NOT_ALLOWED:
      return AEEBT_RM_ERR_CMD_NOT_ALLOWED;
    default:
      return AEEBT_RM_ERR_FAILED;
  }
}

static uint16 OEMBTExtRM_ConvertEvReason( bt_event_reason_type ev_reason )
{
  switch ( ev_reason )
  {
    case BT_EVR_GN_SUCCESS:
      return AEEBT_RM_ERR_NONE;
    case BT_EVR_RM_RADIO_OFF:
      return AEEBT_RM_ERR_RADIO_OFF;
    case BT_EVR_RM_BAD_PARAM:
      return AEEBT_RM_ERR_BAD_PARAM;
    case BT_EVR_RM_NO_RESOURCES:
      return AEEBT_RM_ERR_MAX_CONN;
    case BT_EVR_RM_NO_CONNECTION:
      return AEEBT_RM_ERR_NO_CONN;
    /*Failure reasons if ACL connection fails because of security reason */
    case BT_EVR_RM_SECURITY_NOT_MET:
    case (BT_CMD_EV_HCI_BASE+BT_BE_AUTHENTICATION_FAILURE):
    case (BT_CMD_EV_HCI_BASE+BT_BE_PAIRING_NOT_ALLOWED):
      return AEEBT_RM_ERR_SECURITY_NOT_MET;
    case (BT_CMD_EV_HCI_BASE+BT_BE_PDU_NOT_ALLOWED):
      return AEEBT_RM_ERR_REMOTE_REJECTED;
    default:
      return AEEBT_RM_ERR_CONN_FAIL;
  }
}

static void OEMBTExtRM_addHandle( bt_ev_rm_connected_acl_type *info )
{
  int i;
  for (i = 0; i < MAX_CONNECTIONS; i++)
  {
    if( gMgr.m_HandleMap[i].bInUse == FALSE )
    {
      gMgr.m_HandleMap[i].bInUse = TRUE;
      gMgr.m_HandleMap[i].handle = info->handle;
      gMgr.m_HandleMap[i].bdAddr = *(bt_bd_addr_type*)(&info->bd_addr);
      return;
    }
  }
  MSG_ERROR("OEMBT RM: Connection table full! handle 0x%x",info->handle,0,0);
}

static AEEBTBDAddr* OEMBTExtRM_removeHandle( bt_ev_rm_disconnected_acl_type *info )
{
  OEMBTExtRMobj_t*  pMe;
  int               i;

  for (i = 0; i < MAX_CONNECTIONS; i++)
  {
    if( (gMgr.m_HandleMap[i].bInUse != FALSE) &&
        (gMgr.m_HandleMap[i].handle == info->handle) )
    {
      for ( pMe = gMgr.pNextRM; pMe != NULL; pMe = pMe->pNextRM )
      {
        // if connection role was being set, it's not going to happen anymore
        pMe->uSetConnRoleIdxBitmap &= ~(1 << i);
      }
      gMgr.m_HandleMap[i].bInUse = FALSE;
      return (AEEBTBDAddr*)(&gMgr.m_HandleMap[i].bdAddr);
    }
  }
  MSG_ERROR("OEMBT RM: Handle not found in table!",info->handle,0,0);
  return (AEEBTBDAddr*)(&gMgr.m_HandleMap[0].bdAddr);
}


static void OEMBTExtRM_BREWCb( OEMBTExtRM_EvCb* pEvCb )
{

  OEMBTExtRMobj_t*  pMe = OEMBTExtRM_FindMe( pEvCb->appId );

  if ( pMe == NULL )
  {
    // how did we get here?
    MSG_ERROR( "ReadCb: wrong app id=%x",pEvCb->appId, 0, 0 );
    return;
  }

  switch ( pEvCb->event )
  {
    case BT_EV_RM_RADIO_ACTIVITY:
    {
      if ( pMe->pRadioActivityCb != NULL )
      {
        *pMe->pRadioActivity = (AEEBTRadioActivity) pEvCb->genericData;
        AEE_APP_RESUME( pMe->pRadioActivityCb, pMe->m_pac );
      }
      break;
    }
#ifdef FEATURE_BT_2_1
    case BT_EV_RM_OOB_LOCAL_DATA:
      AEE_APP_RESUME( pMe->pOOBDataCb, pMe->m_pac );
      pMe->pOOBDataCb = NULL;
      break;
#endif /* FEATURE_BT_2_1 */
    default:
      break;
  }

  // done with EvCb
  rex_enter_crit_sect( &OEMBTExtRM_crit_sect );
  pEvCb->bInUse = FALSE;
  rex_leave_crit_sect( &OEMBTExtRM_crit_sect );

}


static OEMBTExtRM_EvCb* OEMBTExtRM_GetFreeEvCb( void )
{
  uint8 i;
  OEMBTExtRM_EvCb* pEvCb = NULL;

  for ( i=0; i<(sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++)
  {
    if ( gMgr.FreeEvCb[ i ].bInUse == FALSE )
    {
      gMgr.FreeEvCb[ i ].bInUse = TRUE;
      pEvCb = &gMgr.FreeEvCb[ i ];
      break;
    }
  }
  if ( pEvCb == NULL )
  {
    MSG_ERROR( "GetFreeEvCb: none left", 0, 0, 0 );
  }
  return pEvCb;
}

// called from BT task context; so just queue the event
static void OEMBTExtRM_EventCallback(bt_ev_msg_type* bt_ev_msg_ptr)
{
  OEMBTExtRMobj_t*   pMe = OEMBTExtRM_FindMe(bt_ev_msg_ptr->ev_hdr.bt_app_id);
  AEEBTNotification* pN  = NULL;
  OEMBTExtRM_EvCb* pEvCb = NULL;

  if ( pMe == NULL )
  {
    return; // probably for an app that no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "RMEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }
  pN->data.uError  = AEEBT_RM_ERR_NONE;

  switch (bt_ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
    {
      bt_ev_gn_cmd_done_type* pm = &bt_ev_msg_ptr->ev_msg.ev_gn_cmd_done;

      switch ( pm->cmd_type )
      {
        case BT_CMD_RM_CONFIG_APPLICATION:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_APPLICATION_CONFIGURED;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_AUTO_SERVICE_SEARCH_DISABLE:
        {
          // no BT event expected
          if ( pMe->localSetting & DISABLING_AUTO_SRV_SRCH )
          { 
            pN->uID = AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_DISABLED;
            pMe->localSetting &= ~DISABLING_AUTO_SRV_SRCH;
          }
          else if ( pMe->localSetting & ENABLING_AUTO_SRV_SRCH )
          {
            pN->uID = AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_ENABLED;
            pMe->localSetting &= ~ENABLING_AUTO_SRV_SRCH;
          }
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_SET_LOCAL_INFO:
        {
          // always success, no BT event expected
          if ( pMe->localSetting & SETTING_LOCAL_NAME )
          {
            pN->uID  = AEEBT_RM_EVT_LOCAL_NAME_SET;
            pMe->localSetting &= ~SETTING_LOCAL_NAME;
          }
          else
          {
            pN->uID  = AEEBT_RM_EVT_LOCAL_COD_SET;
            pMe->localSetting &= ~SETTING_LOCAL_COD;
          }
          break;
        }
#ifndef FEATURE_BT_2_1
        case BT_CMD_RM_BOND:
        {
#else
        case BT_CMD_RM_BOND_EXT:
        case BT_CMD_RM_AUTHORIZE_REBOND:
        {
           /* Will be FALSE only when user presses NO on Rebond*/
          if ( pMe->bRebondAuthorized == FALSE )
          {
            pN->uID = AEEBT_RM_EVT_BONDED;
            pN->data.sBonded.uError = AEEBT_RM_ERR_FAILED;
            pN->data.sBonded.bdAddr = pMe->bdAddr;
            pMe->bRebondAuthorized = TRUE;
            break;
          }
#endif //FEATURE_BT_2_1
          if ( pm->cmd_status == BT_CS_GN_SUCCESS )
          {
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return; // expecting BONDED event later
          }

          pMe->devUpdateStatus.error = 
            OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          pN->uID = AEEBT_RM_EVT_BONDED;
          pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
          break;
        }
        case BT_CMD_RM_BOND_CANCEL:
        {
          if ( pm->cmd_status == BT_CS_GN_SUCCESS )
          {
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return; // expecting BOND FAILED event later
          }
          pMe->devUpdateStatus.error = 
            OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          pN->uID = AEEBT_RM_EVT_BONDED;
          pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
          break;
        }
        case BT_CMD_RM_SET_BONDABLE:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_BONDABLE_MODE;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_DEVICE_ADD:
        {
          if ( pm->cmd_status == BT_CS_GN_SUCCESS )
          {
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return; // expecting DEV_UPDATED event later
          }
          pMe->devUpdateStatus.error = 
            OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          pN->uID = AEEBT_RM_EVT_DEVICE_ADDED;
          pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
          break;
        }
        case BT_CMD_RM_DEVICE_UPDATE:
        {
          if ( pm->cmd_status == BT_CS_GN_SUCCESS )
          {
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return; // expecting DEV_UPDATED event later
          }
          pMe->devUpdateStatus.error = 
            OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          if ( pMe->bUnbonding )
          {
            pMe->bUnbonding = FALSE;
            pN->uID = AEEBT_RM_EVT_UNBONDED;
          }
          else if ( pMe->devUpdateStatus.updateBitmap & 
                    AEEBT_RM_DUS_NICK_NAME_B )
          {
            pMe->devUpdateStatus.updateBitmap &= ~AEEBT_RM_DUS_NICK_NAME_B;
            pN->uID = AEEBT_RM_EVT_NICK_NAME_SET;
            pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          }
          else
          {
            pN->uID = AEEBT_RM_EVT_DEVICE_UPDATED;
          }
          pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
          break;
        }
        case BT_CMD_RM_DEVICE_SERVICES_UPDATE:
        {
          if ( pm->cmd_status == BT_CS_GN_SUCCESS )
          {
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return; // expecting DEV_UPDATED event later
          }
          pMe->devUpdateStatus.error = 
            OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          pN->uID = AEEBT_RM_EVT_DEVICE_SERVICES_UPDATED;
          pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
          break;
        }
        case BT_CMD_RM_DEVICE_REMOVE:
        {
          if ( pm->cmd_status == BT_CS_GN_SUCCESS )
          {
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return; // expecting DEV_UPDATED event later
          }
          pMe->devUpdateStatus.error = 
            OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          pN->uID = AEEBT_RM_EVT_DEVICE_REMOVED;
          pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
          break;
        }
        case BT_CMD_RM_PIN_REPLY:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_PIN_REPLIED;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_REG_LINK_STATUS:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_LINK_STATUS_REG;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_UNREG_LINK_STATUS:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_LINK_STATUS_UNREG;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_REG_RADIO_ACTIVITY:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_RADIO_ACTIVITY_REG;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_UNREG_RADIO_ACTIVITY:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_RADIO_ACTIVITY_UNREG;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_SET_CONN_ROLE_SW_REQ_OK:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_ROLE_SW_CTRL_SET;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_SET_CONN_ROLE:
        {
          if ( ( pMe->uSetConnRoleIdxBitmap != 0 ) &&
               ( pm->cmd_status == BT_CS_GN_SUCCESS ) )
          {
            // role set for existing connection
            // expecting LINK_STATUS event later
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return;
          }
          else
          {
              // Here assuming that App will set connection role of 
              // one device at a time. Otherwise at this time we do not 
              // have BD_ADDR with us to clear one bit of uSetConnRoleIdxBitmap
              pMe->uSetConnRoleIdxBitmap = 0 ;
          }
          // no BT event expected when setting role for future connections
          pN->uID = AEEBT_RM_EVT_CONN_ROLE_SET;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_SET_CONN_SECURITY:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_CONN_SECURITY_SET;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_SET_DEVICE_SECURITY:
        {
          if ( pMe->localSetting & SETTING_LOCAL_SECURITY )
          {
            pMe->localSetting &= ~SETTING_LOCAL_SECURITY;

            // no BT event expected
            pN->uID = AEEBT_RM_EVT_LOCAL_SECURITY_SET;
            pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          }
          else
          {
            if ( pm->cmd_status == BT_CS_GN_SUCCESS )
            {
              IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
              return; // expecting DEV_UPDATED event later
            }

            pMe->devUpdateStatus.error = 
              OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
            pN->uID = AEEBT_RM_EVT_DEVICE_SECURITY_SET;
            pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
          }
          break;
        }
        case BT_CMD_RM_SET_SERVICE_SECURITY:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_SERVICE_SECURITY_SET;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_AUTHORIZE_REPLY:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_AUTHORIZE_REPLIED;
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_TEST_MODE_ENABLE:
        {
          // no BT event expected
          pN->data.uError = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          if ( pN->data.uError == AEEBT_RM_ERR_NONE )
          {
            pMe->bInTestMode = !pMe->bInTestMode;
          }

          if ( pMe->bInTestMode != FALSE )
          {
            pN->uID = AEEBT_RM_EVT_TEST_MODE_ENTERED;
          }
          else
          {
            pN->uID = AEEBT_RM_EVT_TEST_MODE_EXITED;
          }
          break;
        }
        case BT_CMD_RM_DISABLE_RADIO:
        {
          if ( pm->cmd_status == BT_CS_GN_SUCCESS )
          {
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return; // expecting RADIO_DISABLED event later
          }

            pN->uID = AEEBT_RM_EVT_RADIO_STATUS;
            pN->data.uStatus = pMe->bDisablingRadio;
          break;
        }
        case BT_CMD_RM_DISABLE_DISCOVERABILITY:
        {
          // no BT event expected
          pN->uID          = AEEBT_RM_EVT_VISIBILITY_STATUS;
          pN->data.uStatus = 
            ( pm->cmd_status == BT_CS_GN_SUCCESS ) ? 
            !pMe->bDisablingVisibility : pMe->bDisablingVisibility;
          break;
        }

#ifdef FEATURE_BT_2_1
        case BT_CMD_RM_SET_LOCAL_INFO_EXT:
        {
          // no BT event expected
          if ( pMe->localSetting & SETTING_LOCAL_NAME )
          {
            pN->uID  = AEEBT_RM_EVT_LOCAL_NAME_SET;
            pMe->localSetting &= ~SETTING_LOCAL_NAME;
            pN->data.uError = AEEBT_RM_ERR_NONE;
          }
          else if ( pMe->localSetting & SETTING_LOCAL_COD )
          {
            pN->uID  = AEEBT_RM_EVT_LOCAL_COD_SET;
            pMe->localSetting &= ~SETTING_LOCAL_COD;
            pN->data.uError = AEEBT_RM_ERR_NONE;
          }
          else if ( pMe->localSetting & SETTING_NICK_NAME )
          {
            pMe->localSetting &= ~SETTING_NICK_NAME;
            pN->uID = AEEBT_RM_EVT_NICK_NAME_SET;
            pN->data.uError = AEEBT_RM_ERR_NONE;
          }
          else
          {
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return; // expecting DEV_UPDATED event later
          }
          break;
        }
        case BT_CMD_RM_WR_EIR_DATA:
        {
          if( ISBITSET( pMe->localSetting, SETTING_MANUF_DATA ) )
          {
            CLRBIT( pMe->localSetting, SETTING_MANUF_DATA );
            pN->uID = AEEBT_RM_EVT_MANUF_DATA_SET;
            pN->data.uError = AEEBT_RM_ERR_NONE;
          }
          break;
        }
        case BT_CMD_RM_SET_IO_CAP:
        {
          // no BT event expected
          pN->uID          = AEEBT_RM_EVT_IO_CAP_SET;
          pN->data.uError  = AEEBT_RM_ERR_NONE;
          break;
        }
        case BT_CMD_RM_SET_SM4:
        {
          // no BT event expected
          pN->uID          = AEEBT_RM_EVT_SERVICE_SECURITY_SET;
          pN->data.uError  = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_WR_REMOTE_OOB_DATA:
        {
          // no BT event expected
          IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
          return;
        }
        case BT_CMD_RM_USER_CONFIRMATION_REPLY:
        {
          // no BT event expected
          pN->uID          = AEEBT_RM_EVT_USER_CFM_REPLIED;
          pN->data.uError  = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_PASSKEY_REPLY:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_PASSKEY_REPLIED;
          pN->data.uError  = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_KEYPRESS_NOTIFICATION:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_KEYPRESS_NOTIFICATION_REPLIED;
          pN->data.uError  = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
        case BT_CMD_RM_CREATE_LOCAL_OOB_DATA:
        {
          IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
          return; // expecting OOOB_LOCAL_DATA event later
        }
        // case BT_CMD_RM_BOND_EXT:  // already processed above
        // case BT_CMD_RM_AUTHORIZE_REBOND: // already processed above
        case BT_CMD_RM_SET_SM4_DEBUG_MODE:
        {
          // no BT event expected
          IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
          return;
        }
        case BT_CMD_RM_LOW_POWER_CONFIG_EXT:
        {
          // no BT event expected
          pN->uID = AEEBT_RM_EVT_LPM_CONFIGURED;
          pN->data.uError  = OEMBTExtRM_ConvertCmdStatus( pm->cmd_status );
          break;
        }
#endif  /* FEATURE_BT_2_1 */

        default:
        {
          MSG_ERROR( "RMEvCb - unexpect Cmd Done %x", pm->cmd_type, 0, 0 );
          IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
          return;
        }
      }
      break;
    } // end of case BT_EV_GN_CMD_DONE

    case BT_EV_RM_BONDED:
    {
      MSG_HIGH( "OEMBTExt RM BT_EV_RM_BOND- ",
                       0, 0, 0 );
      pN->uID = AEEBT_RM_EVT_BONDED;
      if ( pMe->cls == AEECLSID_BLUETOOTH_RM_OLD )
      {
        pMe->devUpdateStatus.bdAddr = 
          *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_rm_bond.bd_addr);
        pMe->devUpdateStatus.error = AEEBT_RM_ERR_NONE;

        pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
      }
#ifdef FEATURE_BT_2_1
      else
      {
        pN->data.sBonded.bdAddr = 
          *(AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_rm_bond.bd_addr;
        pN->data.sBonded.uError = AEEBT_RM_ERR_NONE;

        switch ( bt_ev_msg_ptr->ev_msg.ev_rm_bond.lk_status_type )
        {
          case BT_RM_LKT_NONE:
            pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_NONE;
            break;
          case BT_RM_LKT_GENERAL:
            pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_GENERAL;
            break;
          case BT_RM_LKT_AUTH_GENERAL:
            pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_AUTH_GENERAL;
            break;
          case BT_RM_LKT_DEDICATED:
            pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_DEDICATED;
            break;
          case BT_RM_LKT_AUTH_DEDICATED:
            pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_AUTH_DEDICATED;
            break;
          case BT_RM_LKT_DEBUG:
            pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_DEBUG;
            break;
          case BT_RM_LKT_COMBINATION:
            pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_COMBINATION;
            break;
          case BT_RM_LKT_UNBONDED:
            pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_UNBONDED;
            break;
          case BT_RM_LKT_AUTH_UNBONDED:
            pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_AUTH_UNBONDED;
            break;
          default:
            pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_NONE;
            break;
        }
      }
#else
/*
      DBGPRINTF_HIGH( "BONDED - Lisbon support not available." );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
      return;
*/
      pMe->devUpdateStatus.bdAddr = 
        *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_rm_bond.bd_addr);
      pMe->devUpdateStatus.error = AEEBT_RM_ERR_NONE;

      pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
#endif /* FEATURE_BT_2_1 */
      break;
    }

    case BT_EV_RM_BOND_FAILED:
    {
      pN->uID = AEEBT_RM_EVT_BONDED;
      MSG_HIGH( "OEMBTExt RM BT_EV_RM_BOND_FAILED- stat=%x",
                       bt_ev_msg_ptr->ev_msg.ev_rm_bondf.reason, 0, 0 );
      if ( pMe->cls == AEECLSID_BLUETOOTH_RM_OLD )
      {
        pMe->devUpdateStatus.bdAddr = 
          *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_rm_bondf.bd_addr);
        pMe->devUpdateStatus.error = 
          OEMBTExtRM_ConvertEvReason( bt_ev_msg_ptr->ev_msg.ev_rm_bondf.reason );

        pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
      }
#ifdef FEATURE_BT_2_1
      else
      {
        pN->data.sBonded.bdAddr = 
          *(AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_rm_bondf.bd_addr;
        pN->data.sBonded.uError = 
          OEMBTExtRM_ConvertEvReason( bt_ev_msg_ptr->ev_msg.ev_rm_bondf.reason );
        pN->data.sBonded.linkKeyStatus = AEEBT_RM_LK_NONE;
      }
#else
/*
      DBGPRINTF_HIGH( "BOND_FAILED - Lisbon support not available." );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
      return;
*/
      pMe->devUpdateStatus.bdAddr = 
        *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_rm_bondf.bd_addr);
      pMe->devUpdateStatus.error = 
        OEMBTExtRM_ConvertEvReason( bt_ev_msg_ptr->ev_msg.ev_rm_bondf.reason );

      pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
#endif /* FEATURE_BT_2_1 */
      break;
    }

    case BT_EV_RM_PIN_REQUEST:
    {
      pN->uID         = AEEBT_RM_EVT_PIN_REQUESTED;
      pN->data.bdAddr = 
        *(AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_rm_pinrq.bd_addr;
      break;
    }

    case BT_EV_RM_DEVICE_UPDATED:
    {
      pMe->devUpdateStatus.bdAddr =
        *(AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_rm_devud.bd_addr;

      if ( bt_ev_msg_ptr->ev_msg.ev_rm_devud.success == FALSE )
      {
        pMe->devUpdateStatus.error = AEEBT_RM_ERR_FAILED;
      }
      else
      {
        pMe->devUpdateStatus.error = AEEBT_RM_ERR_NONE;
      }

      if ( bt_ev_msg_ptr->ev_msg.ev_rm_devud.update_status & 
           BT_RM_DUS_UNBONDED_B )
      {
        pMe->bUnbonding = FALSE;
        pN->uID = AEEBT_RM_EVT_UNBONDED;
      }
      else if ( bt_ev_msg_ptr->ev_msg.ev_rm_devud.update_status & 
                BT_RM_DUS_ADDED_B )
      {
        pN->uID = AEEBT_RM_EVT_DEVICE_ADDED;
      }
      else if ( bt_ev_msg_ptr->ev_msg.ev_rm_devud.update_status & 
                BT_RM_DUS_REMOVED_B )
      {
        pN->uID = AEEBT_RM_EVT_DEVICE_REMOVED;
      }
      else if ( bt_ev_msg_ptr->ev_msg.ev_rm_devud.update_status & 
                BT_RM_DUS_SECURITY_B )
      {
        pN->uID = AEEBT_RM_EVT_DEVICE_SECURITY_SET;
      }
      else if ( bt_ev_msg_ptr->ev_msg.ev_rm_devud.update_status & 
                BT_RM_DUS_SERVICES_B )
      {
        pN->uID = AEEBT_RM_EVT_DEVICE_SERVICES_UPDATED;
      }
      else if ( bt_ev_msg_ptr->ev_msg.ev_rm_devud.update_status & 
                BT_RM_DUS_NICK_NAME_B )
      {
        // nick name update is treated different than the rest
        pMe->devUpdateStatus.updateBitmap &= ~AEEBT_RM_DUS_NICK_NAME_B;
        pN->uID = AEEBT_RM_EVT_NICK_NAME_SET;
        pN->data.uError = pMe->devUpdateStatus.error;
      }
      else
      {
        pMe->devUpdateStatus.updateBitmap = 
          (AEEBTDeviceUpdateStatusBitmap) bt_ev_msg_ptr->ev_msg.ev_rm_devud.
                                                         update_status;
        pN->uID = AEEBT_RM_EVT_DEVICE_UPDATED;
      }

      pN->data.pDevUpdateStatus = &pMe->devUpdateStatus;
      break;
    }

    case BT_EV_RM_LINK_STATUS:
    {
      bt_rm_link_status_type* sptr = 
        &bt_ev_msg_ptr->ev_msg.ev_rm_lstat.link_status;

      pN->uID = AEEBT_RM_EVT_LINK_STATUS;

      if ( pMe->uSetConnRoleIdxBitmap != 0 )
      {
        uint8 i;
        for( i = 0; i < MAX_CONNECTIONS; i++ )
        {
          if ( (gMgr.m_HandleMap[ i ].bInUse != FALSE) &&
               (AEEBT_BD_ADDRS_EQUAL( &gMgr.m_HandleMap[i].bdAddr,
                                      &sptr->bd_addr ) != FALSE ) &&
               (pMe->uSetConnRoleIdxBitmap & (1 << i)) &&
               (sptr->updates & BT_RM_LU_ROLE_B) )
          {
            // this link status results from Set Connection Role command 
            pMe->uSetConnRoleIdxBitmap &= ~(1 << i);
            pN->uID = AEEBT_RM_EVT_CONN_ROLE_SET;
            if ( sptr->updates & BT_RM_LU_ERROR_VALID_B )
            {
              pN->data.uError = 
                OEMBTExtRM_ConvertEvReason( (bt_event_reason_type)sptr->error );
            }
            else
            {
              pN->data.uError = AEEBT_RM_ERR_NONE;
            }
            break;
          }
        }
      }
      if ( pN->uID == AEEBT_RM_EVT_LINK_STATUS )
      {
        pMe->linkStatus.bdAddr          = *(AEEBTBDAddr*)&sptr->bd_addr;
        pMe->linkStatus.updateBitmap    = (AEEBTLinkUpdateBitmap)sptr->updates;
        pMe->linkStatus.security        = (AEEBTSecurity)sptr->security;
        pMe->linkStatus.linkMode        = (AEEBTLinkMode)sptr->link_mode;
        pMe->linkStatus.uCurrIntervalMs = sptr->cur_interval_ms;
  
        if ( sptr->role == BT_ROLE_MASTER )
        {
          pMe->linkStatus.bMaster     = TRUE;
        }
        else
        {
          pMe->linkStatus.bMaster     = FALSE;
        }
  
        pMe->linkStatus.ACLPktTypes   = sptr->pkt_types_acl;
        pMe->linkStatus.SCOPktTypes   = sptr->pkt_types_sco;
        pMe->linkStatus.rssi          = sptr->rssi;
  
        pN->data.pLinkStatus          = &pMe->linkStatus;
      }
      break;
    }

    case BT_EV_RM_RADIO_ACTIVITY:
    {
      if ( (pMe->pRadioActivityCb != NULL) &&
           (pMe->pRadioActivity   != NULL) &&
           (pEvCb = OEMBTExtRM_GetFreeEvCb()) != NULL )
      {
        pEvCb->event         = bt_ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId         = bt_ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->genericData   = 
          (uint32) bt_ev_msg_ptr->ev_msg.ev_rm_ract.radio_activity;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtRM_BREWCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
      return;
    }
    case BT_EV_RM_AUTHORIZE_REQUEST:
    {
      bt_ev_rm_authorize_request_type* rptr =
        &bt_ev_msg_ptr->ev_msg.ev_rm_atzrq;

      pMe->authorizeReq.bdAddr = *(AEEBTBDAddr*)&rptr->bd_addr;
      pMe->authorizeReq.serviceId.serviceIdMethod = 
        (AEEBTServiceIdMethod)rptr->service_id.service_id_method;
      pMe->authorizeReq.authorize_dir = (AEEBTRM_atzrq_type) rptr->authorize_dir;
      switch ( rptr->service_id.service_id_method )
      {
        case BT_SIM_SDP_UUID:
          pMe->authorizeReq.serviceId.id.uServiceClass = 
            rptr->service_id.sdp_uuid;
          break;
        case BT_SIM_RFCOMM_SCN:
          pMe->authorizeReq.serviceId.id.uServerChannel = 
            rptr->service_id.rfcomm_scn;
          break;
        case BT_SIM_L2CAP_PSM:
          pMe->authorizeReq.serviceId.id.uPSM = rptr->service_id.l2cap_psm;
          break;
      }

      pN->uID = AEEBT_RM_EVT_AUTHORIZE_REQUEST;
      pN->data.pAuthorizeReq = &pMe->authorizeReq;
      break;
    }

    case BT_EV_RM_RADIO_DISABLED:
    {
      pN->uID = AEEBT_RM_EVT_RADIO_STATUS;
      pN->data.uStatus = !bt_ev_msg_ptr->ev_msg.ev_rm_radio.disabled;
      break;
    }

    case BT_EV_RM_KEYPRESS_NOTIFICATION:
    {
      pN->uID = AEEBT_RM_EVT_KEYPRESS_NOTIFICATION;
      pN->data.keypress = bt_ev_msg_ptr->ev_msg.ev_rm_keypress_notif.key;
      break;
    }
    case BT_EV_RM_OOB_LOCAL_DATA:
    {
      if ( (pMe->pOOBDataCb != NULL) &&
           (pEvCb = OEMBTExtRM_GetFreeEvCb()) != NULL )
      {
        pEvCb->event              = bt_ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId              = bt_ev_msg_ptr->ev_hdr.bt_app_id;

        MEMCPY( pMe->pOOBDataSize,
                (uint8*)&bt_ev_msg_ptr->ev_msg.ev_rm_oob_ld.size_oob_data,
                sizeof(uint8) );
        CALLBACK_Init( &pEvCb->cb, OEMBTExtRM_BREWCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
      return;
    }
    case BT_EV_RM_OOB_REMOTE_DATA_REQUEST:
    {
      pN->uID = AEEBT_RM_EVT_OOB_REMOTE_DATA_REQUEST;
      pN->data.bdAddr = *(AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.
                          ev_rm_oob_rdreq.bd_addr;
      break;
    }
    case BT_EV_RM_PASSKEY_NOTIFICATION:
    {
      pN->uID = AEEBT_RM_EVT_PASSKEY_NOTIFICATION;
      pN->data.sPasskey.bdAddr  = *(AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.
                                    ev_rm_passkey_notif.bd_addr;
      STRLCPY( pN->data.sPasskey.passkey,
               (char*) bt_ev_msg_ptr->ev_msg.ev_rm_passkey_notif.passkey,
               sizeof(pN->data.sPasskey.passkey) );
      break;
    }
    case BT_EV_RM_PASSKEY_REQUEST:
    {
      pN->uID = AEEBT_RM_EVT_PASSKEY_REQUEST;
      pN->data.bdAddr = *(AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.
                          ev_rm_passkey_req.bd_addr;
      break;
    }
    case BT_EV_RM_REBOND_REQUEST:
    {
      pN->uID = AEEBT_RM_EVT_REBOND_REQUEST;
      pN->data.bdAddr = *(AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.
                          ev_rm_rebreq.bd_addr;
      break;
    }
    case BT_EV_RM_USER_CONFIRMATION_REQUEST:
    {
      pN->uID = AEEBT_RM_EVT_USER_CFM_REQUEST;
      pN->data.sPasskey.bdAddr  = *(AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.
                                    ev_rm_ucr.bd_addr;
      STRLCPY( pN->data.sPasskey.passkey,
               (char*) bt_ev_msg_ptr->ev_msg.ev_rm_ucr.passkey,
               sizeof(pN->data.sPasskey.passkey) );
      break;
    }

    default:
    {
      MSG_ERROR( "RMEvCb - unexpect event %x", 
                 bt_ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_RM );
  IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
}

// called from BT task context
static void OEMBTExtRM_ConnStatusEventCallback( bt_ev_msg_type* bt_ev_msg_ptr )
{
  OEMBTExtRMobj_t*    pMe;
  AEEBTNotification*  pN  = NULL;
  uint32              uID;
  AEEBTBDAddr         bdAddr;
  bt_event_type       evt = bt_ev_msg_ptr->ev_hdr.ev_type;

  if ( (evt != BT_EV_RM_CONNECTED_ACL) && 
       (evt != BT_EV_RM_DISCONNECTED_ACL) )
  {
    MSG_ERROR( "ConnStatusEvCb - unexpect event %x", evt, 0, 0 );
    return;
  }

  if ( evt == BT_EV_RM_CONNECTED_ACL )
  {
    uID    = AEEBT_RM_EVT_CONN;
    bdAddr = *(AEEBTBDAddr*)(&bt_ev_msg_ptr->ev_msg.ev_rm_conna.bd_addr);
    OEMBTExtRM_addHandle( &bt_ev_msg_ptr->ev_msg.ev_rm_conna );
  }
  else
  {
    uID    = AEEBT_RM_EVT_DISCONN;
    bdAddr = *OEMBTExtRM_removeHandle( &bt_ev_msg_ptr->ev_msg.ev_rm_disca );
  }

  for (pMe = gMgr.pNextRM; pMe != NULL; pMe = pMe->pNextRM)
  {
    if ( pMe->bSendConnStatus != FALSE )
    {
      pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
      if( pN == NULL )
      {
        MSG_ERROR( "ConnStatusEvCb - Can't get free not.", 0, 0, 0 );
        return;
      }
      pN->uID = (uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_RM );
      pN->data.bdAddr = bdAddr;
      IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
    }
  }
} 
#endif // defined(FEATURE_IBT)

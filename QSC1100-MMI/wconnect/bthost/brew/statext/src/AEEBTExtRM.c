/*===========================================================================

FILE:      AEEBTExtRM.c

SERVICES:  BlueTooth Radio Manager QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth Radio Manager.
        Most of the interface is implemented directly by the OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtRM.c#2 $
$DateTime: 2009/04/16 17:46:44 $
===========================================================================*/

#include "OEMFeatures.h"

#ifdef FEATURE_IBT

#include "AEEBTExt_priv.h"
#include "OEMBTExtRM.h"

//==========================================================================
//   Macro definitions
//==========================================================================
#define SETTING_LOCAL_NAME      0x01
#define SETTING_LOCAL_COD       0x02
#define SETTING_LOCAL_SECURITY  0x04

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtRM VTABLE
static uint32 AEEBTExtRM_AddRef( IBTExtRM* po );
static uint32 AEEBTExtRM_Release( IBTExtRM* po );
static int    AEEBTExtRM_QueryInterface( IBTExtRM* po, AEECLSID class, 
                                         void** ppo);
//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtRM) gpvtIBTExtRM =
{
  AEEBTExtRM_AddRef, 
  AEEBTExtRM_Release,
  AEEBTExtRM_QueryInterface,
  OEMBTExtRM_GetLocalInfo,
  OEMBTExtRM_SetName,
  OEMBTExtRM_SetClassOfDevice,
  OEMBTExtRM_SetSecurity,
  OEMBTExtRM_Bond,
  OEMBTExtRM_Unbond,
  OEMBTExtRM_SetBondable,
  OEMBTExtRM_PinReply,
  OEMBTExtRM_RegisterLinkStatus,
  OEMBTExtRM_UnregisterLinkStatus,
  OEMBTExtRM_GetLinkStatus,
  OEMBTExtRM_SetConnectionRole,
  OEMBTExtRM_SetConnectionSecurity,
  OEMBTExtRM_SetDeviceSecurity,
  OEMBTExtRM_DeviceAdd,
  OEMBTExtRM_DeviceUpdate,
  OEMBTExtRM_DeviceRead,
  OEMBTExtRM_DeviceEnumInit,
  OEMBTExtRM_DeviceEnumNext,
  OEMBTExtRM_DeviceRemove,
  OEMBTExtRM_SetHCIMode,
  OEMBTExtRM_GetHCIMode,
  OEMBTExtRM_SetServiceSecurity,
  OEMBTExtRM_AuthorizeReply,
  OEMBTExtRM_EnterTestMode,
  OEMBTExtRM_ExitTestMode,
  OEMBTExtRM_ConfigApplication,
  OEMBTExtRM_AutoServiceSearchEnable,
  OEMBTExtRM_AutoServiceSearchDisable,
  OEMBTExtRM_DeviceServicesUpdate,
  OEMBTExtRM_GetBTOnOffState,
  OEMBTExtRM_AllowRoleSwitch,
  OEMBTExtRM_DisableRadio,
  OEMBTExtRM_DisableVisibility,
  OEMBTExtRM_RegisterConnStatus,
  OEMBTExtRM_UnregisterConnStatus,
  OEMBTExtRM_RegisterRadioActivity,
  OEMBTExtRM_UnregisterRadioActivity,
  OEMBTExtRM_GetRadioActivity,
  OEMBTExtRM_GetConnectionRole,
  OEMBTExtRM_GetRoleSwitchPolicy,
  OEMBTExtRM_SetNickName,
  OEMBTExtRM_SetEIRManufData,
  OEMBTExtRM_GetEIRUUIDList,
  OEMBTExtRM_GetEIRManufData,
  OEMBTExtRM_SetIOCap,
  OEMBTExtRM_SetSecBySvcCls,
  OEMBTExtRM_SetSecBySrvrChnl,
  OEMBTExtRM_SetSecByPSM,
  OEMBTExtRM_SetRemoteOOBData,
  OEMBTExtRM_UserConfirmationReply,
  OEMBTExtRM_PasskeyReply,
  OEMBTExtRM_CreateOOBData,
  OEMBTExtRM_ReadOOBData,
  OEMBTExtRM_AuthorizeRebond,
  OEMBTExtRM_KeypressNotification,
  OEMBTExtSSPBond,
  OEMBTExtRM_SetSM4DebugMode,
  OEMBTExtRM_LowPowerConfig,
  OEMBTExtRM_LowPowerConfigTest,
  OEMBTExtRM_BondCancel
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtRM_New()

Description:
This is called from the shell when an app tries to create an instance of a
class ID associated with this module.

Parameters:
IShell* pShell:  IShell pointer
AEECLSID cls:    Class to create
void** ppof:     pointer to created interface pointer

Return Value:  SUCCESS if object created and initialized
ENOMEMORY if unable to create object
EFAILED if unable to initialize object
ECLASSNOTSUPPORT if wrong class ID
EBADPARM if input pointers are NULL

Componts:      None

Side Effects:  None

===========================================================================*/
int AEEBTExtRM_New( IShell* ps, AEECLSID cls, void** ppif )
{
  IBTExtRM* pMe;

  if ( cls != AEECLSID_BLUETOOTH_RM )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC( IBTExtRM );
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;
  pMe->m_cls     = cls;
  
  // init BT layer
  if( OEMBTExtRM_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtRM;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtRM_AddRef( pMe );
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtRM VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtRM_AddRef( IBTExtRM* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->nRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtRM_Release( IBTExtRM* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->nRefs;
    if ( pMe->nRefs > 0 )
    {
      return pMe->nRefs;
    }

    if( OEMBTExtRM_Release( pMe ) != SUCCESS )
    {
      // Uh oh. Release didn't work.
      ++pMe->nRefs;
      return EFAILED;
    }

#ifndef FEATURE_BREW_3_0
    AEE_UnlinkSysObject( &pMe->u.so );
#endif

    FREE( pMe);
  }
  
  return SUCCESS;
}

// IQI (see AEE.h)
static int AEEBTExtRM_QueryInterface(
  IBTExtRM* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if ( !AEE_CHECK_PTR( "AEEBTExtRM_QueryInterface", ppo, sizeof(void*), TRUE ) )
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_RM:
      *ppo = pMe;
      AEEBTExtRM_AddRef( *ppo );
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}

#endif /* FEATURE_IBT */

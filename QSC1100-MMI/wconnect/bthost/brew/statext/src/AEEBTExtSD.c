/*===========================================================================

FILE:      AEEBTExtSD.c

SERVICES:  BlueTooth Service Discovery QCT extension

GENERAL DESCRIPTION: AEE BREW interface to the Bluetooth Service Discovery
        Most of the implementation of SD is in the OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtSD.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#ifdef FEATURE_IBT

#include "AEEBTExt_priv.h"
#include "OEMBTExtSD.h"


//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

typedef struct IBTExtSD_Evt IBTExtSD_Evt;

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtSD VTABLE
static uint32 AEEBTExtSD_AddRef( IBTExtSD* po );
static uint32 AEEBTExtSD_Release( IBTExtSD* po );
static int    AEEBTExtSD_QueryInterface( IBTExtSD* po, AEECLSID class, 
                                         void** ppo);

// static helper functions
//static void AEEBTExtSD_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtSD) gpvtIBTExtSD =
{
  AEEBTExtSD_AddRef, 
  AEEBTExtSD_Release,
  AEEBTExtSD_QueryInterface,
  OEMBTExtSD_DiscoverDevices,
  OEMBTExtSD_StopDeviceDiscovery,
  OEMBTExtSD_SetDiscoverable,
  OEMBTExtSD_GetDeviceName,
  OEMBTExtSD_BrowseService,
  OEMBTExtSD_SearchService,
  OEMBTExtSD_RegisterService,
  OEMBTExtSD_UnregisterService,
  OEMBTExtSD_SetLimitedDiscoverable,
  OEMBTExtSD_GetDiscoverableMode,
  OEMBTExtSD_AddServiceRecord,
  OEMBTExtSD_AddServiceAttribute,
  OEMBTExtSD_UpdateServiceAttribute,
  OEMBTExtSD_ServiceDBEnumInit,
  OEMBTExtSD_ServiceDBEnumNext,
  OEMBTExtSD_RegisterCustomService,
  OEMBTExtSD_UnregisterCustomService,
  OEMBTExtSD_GetDeviceNameCancel,
  OEMBTExtSD_BrowseServiceCancel
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtSD_New()

Description:
   This is called from the shell when an app tries to create an instance of a
   class ID associated with this module.

Parameters:
   IShell* pShell:  IShell pointer
   AEECLSID cls:    Class to create
   void** ppof:     pointer to created interface pointer

Return Value:  AEE_SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object
               ECLASSNOTSUPPORT if wrong class ID
               EBADPARM if input pointers are NULL

Componts:      None

Side Effects:  None

===========================================================================*/
int AEEBTExtSD_New( IShell* ps, AEECLSID cls, void** ppif )
{
  IBTExtSD* pMe;

  if ( cls != AEECLSID_BLUETOOTH_SD )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  // allocate pMe
  pMe = MALLOCREC( IBTExtSD );
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  pMe->pIShell = ps;
  ISHELL_AddRef( pMe->pIShell );

  OEMBTExtSD_Init( pMe );

  // init me
  pMe->u.pvt = &gpvtIBTExtSD;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtSD_AddRef( pMe );
  *ppif = pMe;

  return AEE_SUCCESS;
}


//==========================================================================
//   IBTExtSD VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtSD_AddRef( IBTExtSD* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->nRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtSD_Release( IBTExtSD* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->nRefs;
    if ( pMe->nRefs > 0 )
    {
      return pMe->nRefs;
    }

#ifndef FEATURE_BREW_3_0
    AEE_UnlinkSysObject( &pMe->u.so );
#endif
    OEMBTExtSD_Release( pMe );

    ISHELL_Release( pMe->pIShell );

    FREE( pMe );
  }
  
  return 0;
}

// IQI (see AEE.h)
static int AEEBTExtSD_QueryInterface(
  IBTExtSD* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if ( !AEE_CHECK_PTR( "AEEBTExtSD_QueryInterface", ppo, sizeof(void*), TRUE ) )
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_SD:
       *ppo = pMe;
       AEEBTExtSD_AddRef( *ppo );
       return SUCCESS;
    default:
       *ppo = NULL;
       return ECLASSNOTSUPPORT;
  }
}

#endif /* FEATURE_IBT */

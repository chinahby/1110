/*===========================================================================

FILE:      AEEBTExtSML.c

SERVICES:  BlueTooth SyncML Profile QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth SyncML Profile.
        Most of the interface is implemented directly by the OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtSML.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_SML)

#include "AEEBTExt_priv.h"
#include "OEMBTExtSML.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtSML VTABLE
static uint32 AEEBTExtSML_AddRef( IBTExtSML* po );
static uint32 AEEBTExtSML_Release( IBTExtSML* po );
static int    AEEBTExtSML_QueryInterface( IBTExtSML* po, 
                                          AEECLSID   class, 
                                          void**     ppo);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtSML) gpvtIBTExtSML =
{
  AEEBTExtSML_AddRef, 
  AEEBTExtSML_Release,
  AEEBTExtSML_QueryInterface,
  OEMBTExtSML_RegisterClient,
  OEMBTExtSML_RegisterServer,
  OEMBTExtSML_DeregisterClient,
  OEMBTExtSML_DeregisterServer,
  OEMBTExtSML_AcceptConnection,
  OEMBTExtSML_Connect,
  OEMBTExtSML_Disconnect,
  OEMBTExtSML_Get,
  OEMBTExtSML_Put
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtSML_New()

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
int AEEBTExtSML_New( IShell* ps, AEECLSID cls, void** ppif )
{
  IBTExtSML* pMe;

  if ( cls != AEECLSID_BLUETOOTH_SML )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC( IBTExtSML );
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;
  
  // init BT layer
  if( OEMBTExtSML_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtSML;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtSML_AddRef( pMe );
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtSML VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtSML_AddRef( IBTExtSML* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->m_uRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtSML_Release( IBTExtSML* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    if( OEMBTExtSML_Release( pMe ) != SUCCESS )
    {
      // Uh oh. Release didn't work.
      ++pMe->m_uRefs;
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
static int AEEBTExtSML_QueryInterface(
  IBTExtSML* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if ( !AEE_CHECK_PTR( "AEEBTExtSML_QueryInterface", ppo, 
                       sizeof(void*), TRUE ) )
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_SML:
      *ppo = pMe;
      AEEBTExtSML_AddRef( *ppo );
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_SML) */

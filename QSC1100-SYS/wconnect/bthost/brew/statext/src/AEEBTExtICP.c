/*===========================================================================

FILE:      AEEBTExtICP.c

SERVICES:  BlueTooth Intercom Profile QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth Intercom 
        Profile.  Most of the interface is implemented directly by the 
        OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtICP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP)

#include "AEEBTExt_priv.h"
#include "OEMBTExtICP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtICP VTABLE
static uint32 AEEBTExtICP_AddRef( IBTExtICP* po );
static uint32 AEEBTExtICP_Release( IBTExtICP* po );
static int    AEEBTExtICP_QueryInterface( IBTExtICP* po, 
                                          AEECLSID   class, 
                                          void**     ppo);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtICP) gpvtIBTExtICP =
{
  AEEBTExtICP_AddRef, 
  AEEBTExtICP_Release,
  AEEBTExtICP_QueryInterface,
  OEMBTExtICP_Connect,
  OEMBTExtICP_AcceptConnection,
  OEMBTExtICP_Disconnect,
  OEMBTExtICP_ClearCall,
  OEMBTExtICP_EstablishCall,
  OEMBTExtICP_IncomingStatus,
  OEMBTExtICP_AcceptCall,
  OEMBTExtICP_Register,
  OEMBTExtICP_Deregister
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtICP_New()

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
int AEEBTExtICP_New( IShell* ps, AEECLSID cls, void** ppif )
{
  IBTExtICP* pMe;

  if ( cls != AEECLSID_BLUETOOTH_ICP )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC( IBTExtICP );
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;
  
  // init BT layer
  if( OEMBTExtICP_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtICP;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtICP_AddRef( pMe );
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtICP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtICP_AddRef( IBTExtICP* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->m_uRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtICP_Release( IBTExtICP* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    if( OEMBTExtICP_Release( pMe ) != SUCCESS )
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
static int AEEBTExtICP_QueryInterface(
  IBTExtICP* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if ( !AEE_CHECK_PTR( "AEEBTExtICP_QueryInterface", ppo, 
                       sizeof(void*), TRUE ) )
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_ICP:
      *ppo = pMe;
      AEEBTExtICP_AddRef( *ppo );
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP) */

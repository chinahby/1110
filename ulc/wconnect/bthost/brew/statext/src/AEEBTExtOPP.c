/*===========================================================================

FILE:      AEEBTExtOPP.c

SERVICES:  BlueTooth Object Push Profile QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth Object Push Profile.
        Most of the interface is implemented directly by the OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2009 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtOPP.c#2 $
$DateTime: 2009/04/17 11:28:04 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_OPP)

#include "AEEBTExt_priv.h"
#include "OEMBTExtOPP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtOPP VTABLE
static uint32 AEEBTExtOPP_AddRef( IBTExtOPP* po );
static uint32 AEEBTExtOPP_Release( IBTExtOPP* po );
static int    AEEBTExtOPP_QueryInterface( IBTExtOPP* po, 
                                          AEECLSID class, 
                                          void** ppo);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtOPP) gpvtIBTExtOPP =
{
  AEEBTExtOPP_AddRef, 
  AEEBTExtOPP_Release,
  AEEBTExtOPP_QueryInterface,
  OEMBTExtOPP_Register,
  OEMBTExtOPP_Deregister,
  OEMBTExtOPP_AcceptConnection,
  OEMBTExtOPP_Connect,
  OEMBTExtOPP_Abort,
  OEMBTExtOPP_Push,
  OEMBTExtOPP_Pull,
  OEMBTExtOPP_Disconnect,
  OEMBTExtOPP_GetProgressInfo
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtOPP_New()

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
int AEEBTExtOPP_New( IShell* ps, AEECLSID cls, void** ppif )
{
  IBTExtOPP* pMe;

  if ( cls != AEECLSID_BLUETOOTH_OPP )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC( IBTExtOPP );
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;
  
  // init BT layer
  if( OEMBTExtOPP_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtOPP;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtOPP_AddRef( pMe );
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtOPP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtOPP_AddRef( IBTExtOPP* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->m_uRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtOPP_Release( IBTExtOPP* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    if( OEMBTExtOPP_Release( pMe ) != SUCCESS )
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
static int AEEBTExtOPP_QueryInterface(
  IBTExtOPP* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if ( !AEE_CHECK_PTR( "AEEBTExtOPP_QueryInterface", ppo, 
                       sizeof(void*), TRUE ) )
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_OPP:
      *ppo = pMe;
      AEEBTExtOPP_AddRef( *ppo );
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_OPP) */

/*===========================================================================

FILE:      AEEBTExtBPP.c

SERVICES:  BlueTooth Basic Printing Profile QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth Basic Printing Profile.
        Most of the interface is implemented directly by the OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtBPP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BPP)

#include "AEEBTExt_priv.h"
#include "OEMBTExtBPP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtBPP VTABLE
static uint32 AEEBTExtBPP_AddRef( IBTExtBPP* po );
static uint32 AEEBTExtBPP_Release( IBTExtBPP* po );
static int    AEEBTExtBPP_QueryInterface( IBTExtBPP* po, 
                                          AEECLSID   class, 
                                          void**     ppo);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtBPP) gpvtIBTExtBPP =
{
  AEEBTExtBPP_AddRef, 
  AEEBTExtBPP_Release,
  AEEBTExtBPP_QueryInterface,
  OEMBTExtBPP_Register,
  OEMBTExtBPP_Deregister,
  OEMBTExtBPP_Authenticate,
  OEMBTExtBPP_AcceptConnection,
  OEMBTExtBPP_Connect,
  OEMBTExtBPP_Disconnect,
  OEMBTExtBPP_Abort,
  OEMBTExtBPP_SendFile,
  OEMBTExtBPP_SOAPRequest,
  OEMBTExtBPP_GetRefObjResponse
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtBPP_New()

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
int AEEBTExtBPP_New(IShell* ps, AEECLSID cls, void** ppif)
{
  IBTExtBPP* pMe;

  if ( cls != AEECLSID_BLUETOOTH_BPP )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC(IBTExtBPP);
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;
  
  // init BT layer
  if( OEMBTExtBPP_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtBPP;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtBPP_AddRef(pMe);
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtBPP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtBPP_AddRef( IBTExtBPP* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->m_uRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtBPP_Release( IBTExtBPP* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    if( OEMBTExtBPP_Release( pMe ) != SUCCESS )
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
static int AEEBTExtBPP_QueryInterface(
  IBTExtBPP* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if (!AEE_CHECK_PTR("AEEBTExtBPP_QueryInterface", ppo, sizeof(void*), TRUE))
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_BPP:
      *ppo = pMe;
      AEEBTExtBPP_AddRef(*ppo);
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BPP) */

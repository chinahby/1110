/*===========================================================================

FILE:      AEEBTExtPBAP.c

SERVICES:  BlueTooth Phone Book Access Profile QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth Phone Book Access 
        Profile.  Most of the interface is implemented directly by the 
        OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtPBAP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_PBAP)

#include "AEEBTExt_priv.h"
#include "OEMBTExtPBAP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtPBAP VTABLE
static uint32 AEEBTExtPBAP_AddRef( IBTExtPBAP* po );
static uint32 AEEBTExtPBAP_Release( IBTExtPBAP* po );
static int    AEEBTExtPBAP_QueryInterface( IBTExtPBAP* po, 
                                          AEECLSID   class, 
                                          void**     ppo);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtPBAP) gpvtIBTExtPBAP =
{
  AEEBTExtPBAP_AddRef, 
  AEEBTExtPBAP_Release,
  AEEBTExtPBAP_QueryInterface,
  OEMBTExtPBAP_RegisterServer,
  OEMBTExtPBAP_DeregisterServer,
  OEMBTExtPBAP_AcceptConnection,
  OEMBTExtPBAP_Authenticate,
  OEMBTExtPBAP_Connect,
  OEMBTExtPBAP_Disconnect,
  OEMBTExtPBAP_RegisterClient,
  OEMBTExtPBAP_DeregisterClient,
  OEMBTExtPBAP_Abort,
  OEMBTExtPBAP_PullPhoneBook,
  OEMBTExtPBAP_SetPhoneBook,
  OEMBTExtPBAP_PullvCardListing,
  OEMBTExtPBAP_PullvCardEntry,
  OEMBTExtPBAP_PullPhoneBookResponse,
  OEMBTExtPBAP_SetPhoneBookResponse,
  OEMBTExtPBAP_PullvCardListingResponse,
  OEMBTExtPBAP_PullvCardEntryResponse
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtPBAP_New()

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
int AEEBTExtPBAP_New(IShell* ps, AEECLSID cls, void** ppif)
{
  IBTExtPBAP* pMe;

  if ( cls != AEECLSID_BLUETOOTH_PBAP )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC(IBTExtPBAP);
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;
  
  // init BT layer
  if( OEMBTExtPBAP_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtPBAP;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtPBAP_AddRef(pMe);
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtPBAP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtPBAP_AddRef( IBTExtPBAP* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->m_uRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtPBAP_Release( IBTExtPBAP* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    if( OEMBTExtPBAP_Release( pMe ) != SUCCESS )
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
static int AEEBTExtPBAP_QueryInterface(
  IBTExtPBAP* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if (!AEE_CHECK_PTR("AEEBTExtPBAP_QueryInterface", ppo, sizeof(void*), TRUE))
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_PBAP:
      *ppo = pMe;
      AEEBTExtPBAP_AddRef(*ppo);
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_PBAP) */

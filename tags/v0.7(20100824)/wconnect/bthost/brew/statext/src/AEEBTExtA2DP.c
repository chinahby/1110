/*===========================================================================

FILE:      AEEBTExtA2DP.c

SERVICES:  BlueTooth A2DP QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth A2DP.
        Most of the interface is implemented directly by the OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtA2DP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV)
#include "AEEBTExt_priv.h"
#include "OEMBTExtA2DP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================
// IBTExtA2DP VTABLE
static uint32 AEEBTExtA2DP_AddRef( IBTExtA2DP* po );
static uint32 AEEBTExtA2DP_Release( IBTExtA2DP* po );
static int    AEEBTExtA2DP_QueryInterface( IBTExtA2DP* po, AEECLSID class,
                                           void** ppo);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtA2DP) gpvtIBTExtA2DP =
{
  AEEBTExtA2DP_AddRef,
  AEEBTExtA2DP_Release,
  AEEBTExtA2DP_QueryInterface,
  OEMBTExtA2DP_SetDevice,
  OEMBTExtA2DP_Connect,
  OEMBTExtA2DP_Disconnect,
  OEMBTExtA2DP_Ctl,
  OEMBTExtA2DP_Start,
  OEMBTExtA2DP_Suspend
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtA2DP_New()

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
int AEEBTExtA2DP_New(IShell* ps, AEECLSID cls, void** ppif)
{
  IBTExtA2DP* pMe;

  if ( cls != AEECLSID_BLUETOOTH_A2DP )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC(IBTExtA2DP);
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;

  // init BT layer
  if( OEMBTExtA2DP_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtA2DP;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtA2DP_AddRef(pMe);
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtA2DP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtA2DP_AddRef( IBTExtA2DP* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->m_uRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtA2DP_Release( IBTExtA2DP* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    if( OEMBTExtA2DP_Release( pMe ) != SUCCESS )
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
static int AEEBTExtA2DP_QueryInterface(
                                       IBTExtA2DP* pMe,
                                       AEECLSID class,
                                       void** ppo
                                       )
{
  // pointer validation
  if (!AEE_CHECK_PTR("AEEBTExtA2DP_QueryInterface", ppo, sizeof(void*), TRUE))
  {
    return EMEMPTR;
  }

  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_A2DP:
      *ppo = pMe;
      AEEBTExtA2DP_AddRef(*ppo);
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}
#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV) */

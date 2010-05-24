/*===========================================================================

FILE:      AEEBTExtAVRCP.c

SERVICES:  BlueTooth AVRCP QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth AVRCP.
        Most of the interface is implemented directly by the OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtAVRCP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV)
#include "AEEBTExt_priv.h"
#include "OEMBTExtAVRCP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================
// IBTExtAVRCP VTABLE
static uint32 AEEBTExtAVRCP_AddRef( IBTExtAVRCP* po );
static uint32 AEEBTExtAVRCP_Release( IBTExtAVRCP* po );
static int    AEEBTExtAVRCP_QueryInterface( IBTExtAVRCP* po, AEECLSID class,
                                           void** ppo);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtAVRCP) gpvtIBTExtAVRCP =
{
  AEEBTExtAVRCP_AddRef,
  AEEBTExtAVRCP_Release,
  AEEBTExtAVRCP_QueryInterface,
  OEMBTExtAVRCP_Enable,
  OEMBTExtAVRCP_Connect,
  OEMBTExtAVRCP_Disconnect,
  OEMBTExtAVRCP_Disable,
  OEMBTExtAVRCP_SendReply,
  OEMBTExtAVRCP_SendGenericReply,
  OEMBTExtAVRCP_GetGenericOp
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtAVRCP_New()

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
int AEEBTExtAVRCP_New(IShell* ps, AEECLSID cls, void** ppif)
{
  IBTExtAVRCP* pMe;

  if ( cls != AEECLSID_BLUETOOTH_AVRCP )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC(IBTExtAVRCP);
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;

  // init BT layer
  if( OEMBTExtAVRCP_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtAVRCP;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtAVRCP_AddRef(pMe);
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtAVRCP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtAVRCP_AddRef( IBTExtAVRCP* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->m_uRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtAVRCP_Release( IBTExtAVRCP* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    if( OEMBTExtAVRCP_Release( pMe ) != SUCCESS )
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
static int AEEBTExtAVRCP_QueryInterface(
                                       IBTExtAVRCP* pMe,
                                       AEECLSID class,
                                       void** ppo
                                       )
{
  // pointer validation
  if (!AEE_CHECK_PTR("AEEBTExtAVRCP_QueryInterface", ppo, sizeof(void*), TRUE))
  {
    return EMEMPTR;
  }

  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_AVRCP:
      *ppo = pMe;
      AEEBTExtAVRCP_AddRef(*ppo);
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}
#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV) */

/*===========================================================================

FILE:      AEEBTExtCTP.c

SERVICES:  BlueTooth Cordless Telephony Profile QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth Cordless Telephony 
        Profile.  Most of the interface is implemented directly by the 
        OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtCTP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP)

#include "AEEBTExt_priv.h"
#include "OEMBTExtCTP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtCTP VTABLE
static uint32 AEEBTExtCTP_AddRef( IBTExtCTP* po );
static uint32 AEEBTExtCTP_Release( IBTExtCTP* po );
static int    AEEBTExtCTP_QueryInterface( IBTExtCTP* po, 
                                          AEECLSID   class, 
                                          void**     ppo);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtCTP) gpvtIBTExtCTP =
{
  AEEBTExtCTP_AddRef, 
  AEEBTExtCTP_Release,
  AEEBTExtCTP_QueryInterface,
  OEMBTExtCTP_Connect,
  OEMBTExtCTP_AcceptConnection,
  OEMBTExtCTP_Disconnect,
  OEMBTExtCTP_ClearCall,
  OEMBTExtCTP_EstablishCall,
  OEMBTExtCTP_Info,
  OEMBTExtCTP_IncomingStatus,
  OEMBTExtCTP_AcceptCall,
  OEMBTExtCTP_StartDTMF,
  OEMBTExtCTP_StopDTMF,
  OEMBTExtCTP_Register,
  OEMBTExtCTP_Deregister,
  OEMBTExtCTP_RegisterRecall
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtCTP_New()

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
int AEEBTExtCTP_New(IShell* ps, AEECLSID cls, void** ppif)
{
  IBTExtCTP* pMe;

  if ( cls != AEECLSID_BLUETOOTH_CTP )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC(IBTExtCTP);
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;
  
  // init BT layer
  if( OEMBTExtCTP_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtCTP;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtCTP_AddRef(pMe);
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtCTP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtCTP_AddRef( IBTExtCTP* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->m_uRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtCTP_Release( IBTExtCTP* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    if( OEMBTExtCTP_Release( pMe ) != SUCCESS )
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
static int AEEBTExtCTP_QueryInterface(
  IBTExtCTP* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if (!AEE_CHECK_PTR("AEEBTExtCTP_QueryInterface", ppo, sizeof(void*), TRUE))
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_CTP:
      *ppo = pMe;
      AEEBTExtCTP_AddRef(*ppo);
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP) */

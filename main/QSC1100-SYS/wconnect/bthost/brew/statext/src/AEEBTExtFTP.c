/*===========================================================================

FILE:      AEEBTExtFTP.c

SERVICES:  BlueTooth File Transfer Profile QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth File Transfer Profile.
        Most of the interface is implemented directly by the OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2009 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtFTP.c#2 $
$DateTime: 2009/04/17 11:28:04 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_FTP)

#include "AEEBTExt_priv.h"
#include "OEMBTExtFTP.h"


//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtFTP VTABLE
static uint32 AEEBTExtFTP_AddRef( IBTExtFTP* po );
static uint32 AEEBTExtFTP_Release( IBTExtFTP* po );
static int    AEEBTExtFTP_QueryInterface( IBTExtFTP* po, 
                                          AEECLSID class, 
                                          void** ppo);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtFTP) gpvtIBTExtFTP =
{
  AEEBTExtFTP_AddRef, 
  AEEBTExtFTP_Release,
  AEEBTExtFTP_QueryInterface,
  OEMBTExtFTP_Register,
  OEMBTExtFTP_Deregister,
  OEMBTExtFTP_AcceptConnection,
  OEMBTExtFTP_SendResult,
  OEMBTExtFTP_Connect,
  OEMBTExtFTP_Abort,
  OEMBTExtFTP_SetPath,
  OEMBTExtFTP_CreateFolder,
  OEMBTExtFTP_ListFolder,
  OEMBTExtFTP_Put,
  OEMBTExtFTP_Get,
  OEMBTExtFTP_Delete,
  OEMBTExtFTP_Authenticate,
  OEMBTExtFTP_Disconnect,
  OEMBTExtFTP_AuthData,
  OEMBTExtFTP_GetProgressInfo
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtFTP_New()

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
int AEEBTExtFTP_New(IShell* ps, AEECLSID cls, void** ppif)
{
  IBTExtFTP* pMe;

  if ( cls != AEECLSID_BLUETOOTH_FTP )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC( IBTExtFTP );
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;
  
  // init BT layer
  if( OEMBTExtFTP_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtFTP;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtFTP_AddRef( pMe );
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtFTP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtFTP_AddRef( IBTExtFTP* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->m_uRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtFTP_Release( IBTExtFTP* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    if( OEMBTExtFTP_Release( pMe ) != SUCCESS )
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
static int AEEBTExtFTP_QueryInterface(
  IBTExtFTP* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if ( !AEE_CHECK_PTR( "AEEBTExtFTP_QueryInterface", ppo, 
                       sizeof(void*), TRUE ) )
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_FTP:
      *ppo = pMe;
      AEEBTExtFTP_AddRef( *ppo );
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_FTP) */

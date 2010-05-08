/*===========================================================================

FILE:      AEEBTExtL2.c

SERVICES:  BlueTooth L2CAP interface

GENERAL DESCRIPTION: AEE BREW interface to the Bluetooth L2 layer.
        Most of the functionality of this module is implemented in SVC mode
        in OEMBTExtAG.c

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2004-2005, 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtL2.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#ifdef FEATURE_IBT

#include "AEENet.h"
#include "AEEBTExt_priv.h"
#include "OEMBTExtL2.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtAG VTABLE
static uint32  AEEBTExtL2_AddRef( IBTExtL2* po );
static uint32  AEEBTExtL2_Release( IBTExtL2* po );

//==========================================================================
//   Static data
//==========================================================================
static const VTBL(IBTExtL2) gpvtIBTExtL2 =
{
  AEEBTExtL2_AddRef, 
  AEEBTExtL2_Release,
  OEMBTExtL2_Readable,
  OEMBTExtL2_Read,
  OEMBTExtL2_Cancel,
  OEMBTExtL2_Register,
  OEMBTExtL2_Deregister,
  OEMBTExtL2_SetParams,
  OEMBTExtL2_Connect,
  OEMBTExtL2_Disconnect,
  OEMBTExtL2_Write,
  OEMBTExtL2_WriteV,
  OEMBTExtL2_ReadV,
  OEMBTExtL2_Writable,
  OEMBTExtL2_NumConn,
  OEMBTExtL2_DisconnImmediate,
  OEMBTExtL2_GetOEMHandle,
  OEMBTExtL2_Clone,
  OEMBTExtL2_GetStatus
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtL2_New()

Description:
   This is called from the shell when an app tries to create an instance of a
   class ID associated with this module.

Parameters:
   IShell* pShell:  IShell pointer
   AEECLSID cls:    Class to create
   void** ppof:     pointer to created interface pointer

Return Value:  None

Componts:      None

Side Effects:  None

===========================================================================*/
int AEEBTExtL2_New( IShell* ps, AEECLSID cls, void** ppif )
{
  IBTExtL2* pMe;

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  if ( ! AEE_CHECK_PTR( "AEEBTExtL2_New", ppif, sizeof(void*), TRUE ) )
  {
    return EMEMPTR;
  }

  if ( AEECLSID_BLUETOOTH_L2 != cls )
  {
    return ECLASSNOTSUPPORT;
  }

  // allocate pMe
  pMe = MALLOCREC( IBTExtL2 );
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;
  ISHELL_AddRef( ps );

  // init BT layer
  if( OEMBTExtL2_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtL2;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtL2_AddRef( pMe );

  *ppif = pMe;
  
  return SUCCESS;
}

//==========================================================================
//   IBTExtL2 VTABLE (see header files for documentation)
//==========================================================================
// IBase (see AEE.h)
static uint32 AEEBTExtL2_AddRef( IBTExtL2* pMe )
{
  // increment ref count
  return ++pMe->m_uRefs;
}


// IBase (see AEE.h)
static uint32 AEEBTExtL2_Release( IBTExtL2* pMe )
{
  // decrement ref count
  --pMe->m_uRefs;
  
  // outstanding references?
  if ( pMe->m_uRefs > 0 )
  {
    return pMe->m_uRefs;
  } 
  
  // Ref count is zero.  clean up
  (void)OEMBTExtL2_Release( pMe );
  FREE(pMe);
  return SUCCESS;
}

#endif /* FEATURE_IBT */


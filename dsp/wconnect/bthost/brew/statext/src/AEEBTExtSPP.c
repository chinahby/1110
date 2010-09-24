/*===========================================================================

FILE:      AEEBTExtSPP.c

SERVICES:  BlueTooth Serial Port Protocol QCT extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth Serial Port Protocol

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

    Copyright © 2004-2005, 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtSPP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#ifdef FEATURE_IBT

#include "AEEBTExt_priv.h"
#include "OEMBTExtSPP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

#define WATERMARK_HI           3000
#define WATERMARK_LO           1000
#define WATERMARK_MAX         10000
#define WATERMARK_ENQUEUE_SIZE  200

#define MAX_SPP_STREAMS           5

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtSPP VTABLE
static uint32 AEEBTExtSPP_AddRef( IBTExtSPP* po );
static uint32 AEEBTExtSPP_Release( IBTExtSPP* po );
static int    AEEBTExtSPP_QueryInterface( IBTExtSPP* po, AEECLSID class, 
                                          void** ppo );
//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtSPP) gpvtIBTExtSPP =
{
  AEEBTExtSPP_AddRef, 
  AEEBTExtSPP_Release,
  AEEBTExtSPP_QueryInterface,
  OEMBTExtSPP_Read,
  OEMBTExtSPP_Readable,
  OEMBTExtSPP_Open,
  OEMBTExtSPP_IOCtl,
  OEMBTExtSPP_Write,
  OEMBTExtSPP_Writeable,
  OEMBTExtSPP_Close,
  OEMBTExtSPP_GetOEMHandle,
  OEMBTExtSPP_Clone,
  OEMBTExtSPP_GetuID,
  OEMBTExtSPP_GetStatus
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtSPP_New()

Description:
   This is called from the shell when an app tries to create an instance of a
   class ID associated with this module.

Parameters:
   IShell* pShell:  IShell pointer
   AEECLSID cls:    Class to create
   void** ppof:     pointer to created interface pointer

Return Value:  AEE_SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object
               ECLASSNOTSUPPORT if wrong class ID
               EBADPARM if input pointers are NULL

Componts:      None

Side Effects:  None

===========================================================================*/
int AEEBTExtSPP_New( IShell* ps, AEECLSID cls, void** ppif )
{
  IBTExtSPP* pMe;

  if ( cls != AEECLSID_BLUETOOTH_SPP )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  // allocate pMe
  pMe = MALLOCREC( IBTExtSPP );
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtSPP;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  pMe->pIShell        = ps;
  ISHELL_AddRef( pMe->pIShell );

  OEMBTExtSPP_Init( pMe );

  AEEBTExtSPP_AddRef( pMe );
  *ppif = pMe;

  return AEE_SUCCESS;
}


//==========================================================================
//   IBTExtSPP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtSPP_AddRef( IBTExtSPP* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->nRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtSPP_Release( IBTExtSPP* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->nRefs;
    if ( pMe->nRefs > 0 )
    {
      return pMe->nRefs;
    }

#ifndef FEATURE_BREW_3_0
    AEE_UnlinkSysObject( &pMe->u.so );
#endif

    OEMBTExtSPP_Release( pMe );
    
    ISHELL_Release( pMe->pIShell );

    FREE( pMe);
  }
  
  return 0;
}

// IQI (see AEE.h)
static int AEEBTExtSPP_QueryInterface(
  IBTExtSPP* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if ( !AEE_CHECK_PTR( "AEEBTExtSPP_QueryInterface", ppo, 
                       sizeof(void*), TRUE ) )
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_SPP:
       *ppo = pMe;
       AEEBTExtSPP_AddRef( *ppo );
       return SUCCESS;
    default:
       *ppo = NULL;
       return ECLASSNOTSUPPORT;
  }
}

#endif /* FEATURE_IBT */

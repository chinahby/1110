/*===========================================================================

FILE:      AEEBTExtNA.c

SERVICES:  BlueTooth Service Discovery QCT extension

GENERAL DESCRIPTION: AEE layer BREW interface to the Bluetooth Network Access

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtNA.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#ifdef FEATURE_IBT

#include "AEEBTExt_priv.h"
#include "OEMBTExtNA.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================


//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtNA VTABLE
static uint32  AEEBTExtNA_AddRef( IBTExtNA* po);
static uint32  AEEBTExtNA_Release( IBTExtNA* po);
static int     AEEBTExtNA_QueryInterface( IBTExtNA* po, AEECLSID class, 
                                          void** ppo);


//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtNA) gpvtIBTExtNA =
{
  AEEBTExtNA_AddRef, 
  AEEBTExtNA_Release,
  AEEBTExtNA_QueryInterface,
  OEMBTExtNA_Enable,
  OEMBTExtNA_Disable
};

static IBTExtNA* pIBTExtNAobj = NULL;

/*===========================================================================

Function:  AEEBTExtNA_New()

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
int AEEBTExtNA_New( IShell* ps, AEECLSID cls, void** ppif )
{
  IBTExtNA* pNew;

#ifdef AEE_SIMULATOR
  /* Do not create an object if this is being run on the SDK. */
  *ppif = NULL;
  return EUNSUPPORTED;
#endif

  if ( cls != AEECLSID_BLUETOOTH_NA )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  if ( ! AEE_CHECK_PTR( "IBTExtNA_New", ppif, sizeof(void*), TRUE ) )
  {
    return EMEMPTR;
  }
  
  if ( pIBTExtNAobj == NULL )
  {
    // allocate the object
    pNew = (IBTExtNA*) AEE_OEM_NEWCLASS( (IBaseVtbl*)&gpvtIBTExtNA, 
                                     sizeof(IBTExtNA) );
    if ( pNew == NULL )
    {
      return ENOMEMORY;
    }
    // init me
    pNew->m_pIShell = ps;
    pNew->m_uRefs   = 1;
    pNew->m_cls     = cls;

    if( OEMBTExtNA_Init( pNew ) != SUCCESS )
    {
      FREE( pNew );
      return EFAILED;
    }

    pIBTExtNAobj = pNew;
  }
  else
  {
    AEEBTExtNA_AddRef( pIBTExtNAobj );
  }
  
  *ppif = pIBTExtNAobj;

  return SUCCESS;
}


//==========================================================================
//   IBTExtNA VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtNA_AddRef( IBTExtNA* pMe )
{
  if ( pMe != NULL )
  {
    return (++pMe->m_uRefs);  // increment ref count
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtNA_Release( IBTExtNA* pMe )
{
  if ( pMe != NULL )
  {
    // decrement ref count
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    // Ref count is zero.  clean up
    (void)OEMBTExtNA_Release( pMe );

    FREE( pMe );

    //set the global pointer to NULL.
    pIBTExtNAobj = NULL;
  }
  
  return 0;
}

// IQI (see AEE.h)
static int AEEBTExtNA_QueryInterface(
  IBTExtNA* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  if ( (pMe == NULL) || (ppo == NULL) )
  {
    return EBADPARM;
  }

  // pointer validation
  if ( !AEE_CHECK_PTR( "AEEBTExtNA_QueryInterface", ppo, sizeof(void*), TRUE ) )
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_NA:
       *ppo = pMe;
       AEEBTExtNA_AddRef( *ppo );
       return SUCCESS;
    default:
       *ppo = NULL;
       return ECLASSNOTSUPPORT;
  }
}

#endif /* FEATURE_IBT */

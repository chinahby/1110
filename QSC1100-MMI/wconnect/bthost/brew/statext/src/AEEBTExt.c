/*===========================================================================

FILE:      AEEBTExt.c

SERVICES:  BlueTooth QCT extension

GENERAL DESCRIPTION: BREW interface to Bluetooth 

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2004-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExt.c#1 $
$DateTime: 2009/01/07 18:14:54 $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
09/15/08   sp  Compiler warning fix

===========================================================================*/

#include "OEMFeatures.h"

#ifdef FEATURE_IBT
#include "AEEBTExt_priv.h"
#include "BREWVersion.h"

#if MIN_BREW_VERSION(3,0)
#include "AEE_OEMDispatch.h"
#endif

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================
// IBTExtNotifier VTABLE
static uint32 AEEBTExtNotifier_AddRef(IBTExtNotifier *po);
static uint32 AEEBTExtNotifier_Release(IBTExtNotifier *po);
#if MIN_BREW_BUILD(3,1,4,7)
static void   AEEBTExtNotifier_SetMask(IBTExtNotifier *po, 
                                       uint32 *pdwMask);
#else
static void   AEEBTExtNotifier_SetMask(IBTExtNotifier *po, 
                                       const uint32 *pdwMask);
#endif // MIN_BREW_BUILD(3,1,4,7)

//==========================================================================
//   Helper functions
//==========================================================================
static void AEEBTExtNotifierCb( IBTExtNotifier* pMe );
static void AEEBTExtNotifier_OnExitCb( IBTExtNotifier* pMe );

//==========================================================================
//   Static data
//==========================================================================
static const VTBL(IBTExtNotifier) gpvtIBTExtNotifier = {
   AEEBTExtNotifier_AddRef,
   AEEBTExtNotifier_Release,
   AEEBTExtNotifier_SetMask,
   OEMBTExtNotifier_DoNotify,            // These following should only
   OEMBTExtNotifier_GetFreeNotification, // be called in SVC mode
   OEMBTExtNotifier_ReleaseNotification
};

static IBTExtNotifier*    pAEEBTExtNotifier = NULL;

//==========================================================================
//   Public Funtions
//==========================================================================

/*=============================================================================
FUNCTION: AEEBTExtNotifier_New

DESCRIPTION:  
  Allocates a new IBTExtNotifier object and initializes it or 
  adds reference to an existing object.
=============================================================================*/
int AEEBTExtNotifier_New( IShell *pIShell, AEECLSID cls, void **ppif )
{
  IBTExtNotifier* pNew;

#ifdef AEE_SIMULATOR
  /* Do not create an object if this is being run on the SDK. */
  *ppif = NULL;
  return EUNSUPPORTED;
#endif
  
  /* Check parameters. */
  if ( (pIShell == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }
  if ( ! AEE_CHECK_PTR( "AEEBTExtNotifier_New", ppif, sizeof(void*), TRUE) )
  {
    return EMEMPTR;
  }

  if ( cls != AEECLSID_BLUETOOTH_NOTIFIER )
  {
    return EUNSUPPORTED;
  }

  if ( pAEEBTExtNotifier == NULL )
  {
    pNew = (IBTExtNotifier*) AEE_NewClassEx( (IBaseVtbl*)&gpvtIBTExtNotifier,
                                             sizeof(IBTExtNotifier), TRUE ); 
    
    if (pNew == NULL)
    {
      return ENOMEMORY;
    }

    if(OEMBTExtNotifier_Init( pNew ) != SUCCESS)
    {
      FREE(pNew);
      return EFAILED;
    }

    pNew->m_pIShell     = pIShell;
    ISHELL_AddRef( pIShell );

    pNew->m_cls         = cls;
    pNew->m_uRefs       = 0;

    pNew->m_cb.pfnCancel = NULL;
    CALLBACK_Init( &pNew->m_cb, AEEBTExtNotifierCb, pNew );
    pNew->m_cb.pfnCancel = NULL;

    CALLBACK_Init( &pNew->m_cbExit, AEEBTExtNotifier_OnExitCb, pNew );
    ISHELL_OnExit( pNew->m_pIShell, &pNew->m_cbExit );

    pAEEBTExtNotifier   = pNew;
  }

  AEEBTExtNotifier_AddRef( pAEEBTExtNotifier );
  *ppif = pAEEBTExtNotifier;

  return SUCCESS;
}

//==========================================================================
//   Helper Funtions
//==========================================================================

static void AEEBTExtNotifier_OnExitCb( IBTExtNotifier* pMe )
{
  if ( pAEEBTExtNotifier != NULL )
  {
    CALLBACK_Cancel( &pAEEBTExtNotifier->m_cbExit );
    CALLBACK_Cancel( &pAEEBTExtNotifier->m_cb );
    ISHELL_Release( pAEEBTExtNotifier->m_pIShell );
    OEMBTExtNotifier_Release(pMe->m_hBTNotifier);
    FREE( pMe );
    pAEEBTExtNotifier = NULL;
  }
}

static uint32 AEEBTExtNotifier_AddRef( IBTExtNotifier *pMe )
{
  if ( pMe != NULL )
  {
    return (++pMe->m_uRefs);
  } 
  return 0;
}

static uint32 AEEBTExtNotifier_Release( IBTExtNotifier *pMe )
{
  if ( pMe != NULL )
  {
    if ( pMe->m_uRefs > 0 )
    {
      --pMe->m_uRefs; // decrement ref count
    }
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }
    AEEBTExtNotifier_OnExitCb( pMe );
  } 
  return 0;
}

/*=============================================================================
FUNCTION: AEEBTExtNotifier_SetMask

DESCRIPTION:  
  This function is called by Brew when an application does an
  ISHELL_RegisterNotify for the IBTExtNotifier object.  We merge
  all the masks for all the registrations and save a bitmap of
  the sum of all the events being requested.
=============================================================================*/
static void AEEBTExtNotifier_SetMask(
  IBTExtNotifier*  pMe,
#if MIN_BREW_BUILD(3,1,4,7)
  uint32*    pdwMasks
#else
  const uint32*    pdwMasks
#endif // MIN_BREW_BUILD(3,1,4,7)
)
{
  uint32   dwCount = 0;

  if ( pMe == NULL )
  {
    return;
  }

  while ( pdwMasks[dwCount] != 0 )
  {
    dwCount++;
  }
  
  OEMBTExtNotifier_SetMask(pMe->m_hBTNotifier, pdwMasks, dwCount);

}

static void AEEBTExtNotifierCb( IBTExtNotifier* pMe )
{
  AEEBTNotification sNot;
  
  while ( OEMBTExtNotifier_GetCb( pMe->m_hBTNotifier, &sNot ) == TRUE )
  {
    int result;

    result = ISHELL_Notify( pMe->m_pIShell, AEECLSID_BLUETOOTH_NOTIFIER, 
                            sNot.uID, (void*)&sNot.data );
    if ( result != SUCCESS )
    {
      DBGPRINTF( "NotifierCb - r=%d", result, 0, 0 );
    }

  }
}

#endif /* FEATURE_IBT */

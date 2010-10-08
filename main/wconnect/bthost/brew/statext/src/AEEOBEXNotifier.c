/*===========================================================================

FILE:      AEEOBEXNotifier.c

SERVICES:  IOBEXNotifier extension

GENERAL DESCRIPTION: BREW interface to OBEX

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2004, 2007-2008  QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEOBEXNotifier.c#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
  $Change: 815296 $
                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/15/08   PR     Fix compiler warnings
01/05/07   MH	  Free notification event memory function prototype added.
01/03/07   MH	  Free notification event memory.
11/13/04   sivap  initial checkin. unit tested for IOBEXServer API
=============================================================================*/


#include "OEMFeatures.h"

#ifdef FEATURE_IOBEX

#include "AEEOBEXNotifier.h"


//#include "OEMBridle.h"
#include "OEMOBEXNotifier.h"
//#include "bridle_OEMOBEXNotifier.h"

#include "OEMOBEX.h"

#include "BREWVersion.h"
#include "AEE.h"
#include "AEEStdLib.h"


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
// IOBEXNotifier VTABLE
static uint32 AEEOBEXNotifier_AddRef(IOBEXNotifier *po);
static uint32 AEEOBEXNotifier_Release(IOBEXNotifier *po);
static void   AEEOBEXNotifier_SetMask(IOBEXNotifier *po,
                                       uint32 *pdwMask);

//==========================================================================
//   Helper functions
//==========================================================================
static void AEEOBEXNotifierCb( IOBEXNotifier* pMe );
static void AEEOBEXNotifier_OnExitCb( IOBEXNotifier* pMe );

//==========================================================================
//   Static data
//==========================================================================
static const VTBL(IOBEXNotifier) gpvtIOBEXNotifier = {
   AEEOBEXNotifier_AddRef,
   AEEOBEXNotifier_Release,
   AEEOBEXNotifier_SetMask,
   OEMOBEXNotifier_DoNotify // This should only be called in SVC mode

};

static IOBEXNotifier*    pAEEOBEXNotifier = NULL;

//==========================================================================
//   Public Funtions
//==========================================================================

/*=============================================================================
FUNCTION: AEEOBEXNotifier_New

DESCRIPTION:
  Allocates a new IOBEXNotifier object and initializes it or
  adds reference to an existing object. Note that the notifier is a singleton
  and is reused by all instances of IOBEX
=============================================================================*/
int AEEOBEXNotifier_New( IShell *pIShell, AEECLSID cls, void **ppif )
{
  IOBEXNotifier* pNew;

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
  if ( ! AEE_CHECK_PTR( "AEEOBEXNotifier_New", ppif, sizeof(void*), TRUE) )
  {
    return EMEMPTR;
  }

  if ( cls != AEECLSID_OBEX_NOTIFIER)
  {
    return EUNSUPPORTED;
  }

  if ( pAEEOBEXNotifier == NULL )
  {
  //allocate the memory in system memory using AEE_NewClassEx
    pNew = (IOBEXNotifier*) AEE_OEM_NEWCLASSEX( (IBaseVtbl*)&gpvtIOBEXNotifier,
                                             sizeof(IOBEXNotifier), TRUE );

    if (pNew == NULL)
    {
      return ENOMEMORY;
    }

    if(OEMOBEXNotifier_Init( pNew ) != SUCCESS)
    {
      FREE(pNew);
      return EFAILED;
    }

    pNew->m_pIShell     = pIShell;
    ISHELL_AddRef( pIShell );

    pNew->m_cls         = cls;
    pNew->m_uRefs       = 0;

    pNew->m_cb.pfnCancel = NULL;
    CALLBACK_Init( &pNew->m_cb, AEEOBEXNotifierCb, pNew );
    pNew->m_cb.pfnCancel = NULL;

    CALLBACK_Init( &pNew->m_cbExit, AEEOBEXNotifier_OnExitCb, pNew );
    ISHELL_OnExit( pNew->m_pIShell, &pNew->m_cbExit );

    pAEEOBEXNotifier   = pNew;
  }

  AEEOBEXNotifier_AddRef( pAEEOBEXNotifier );
  *ppif = pAEEOBEXNotifier;

  return SUCCESS;
}

//==========================================================================
//   Helper Funtions
//==========================================================================

static void AEEOBEXNotifier_OnExitCb( IOBEXNotifier* pMe )
{
  if ( pAEEOBEXNotifier != NULL )
  {
    CALLBACK_Cancel( &pAEEOBEXNotifier->m_cbExit );
    CALLBACK_Cancel( &pAEEOBEXNotifier->m_cb );
    ISHELL_Release( pAEEOBEXNotifier->m_pIShell );
//    IFBRIDLE_PACK(OEMOBEXNotifier_Release)(pMe->m_hNotifier);
    OEMOBEXNotifier_Release(pMe->m_hNotifier);
    FREE( pMe );
    pAEEOBEXNotifier = NULL;
  }
}

static uint32 AEEOBEXNotifier_AddRef( IOBEXNotifier *pMe )
{
  if ( pMe != NULL )
  {
    return (++pMe->m_uRefs);
  }
  return 0;
}

static uint32 AEEOBEXNotifier_Release( IOBEXNotifier *pMe )
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
    AEEOBEXNotifier_OnExitCb( pMe );
  }
  return 0;
}

/*=============================================================================
FUNCTION: AEEOBEXNotifier_SetMask

DESCRIPTION:
  This function is called by Brew when an application does an
  ISHELL_RegisterNotify for the IOBEXNotifier object.  We merge
  all the masks for all the registrations and save a bitmap of
  the sum of all the events being requested.
=============================================================================*/
static void AEEOBEXNotifier_SetMask(
  IOBEXNotifier*  pMe,
  uint32*    pdwMasks
)
{
  uint32   dwCount = 0;

  if ( (pMe == NULL) || (pdwMasks == NULL) )
  {
    return;
  }

  while ( pdwMasks[dwCount] != 0 )
  {
    dwCount++;
  }

//  IFBRIDLE_PACK(OEMOBEXNotifier_SetMask)(pMe->m_hNotifier, pdwMasks, dwCount);
  OEMOBEXNotifier_SetMask(pMe->m_hNotifier, pdwMasks, dwCount);
}

static void AEEOBEXNotifierCb( IOBEXNotifier* pMe )
{
  AEEOBEXNotificationType sNot;

//  while ( IFBRIDLE_PACK(OEMOBEXNotifier_GetCb(pMe->m_hNotifier,
//                                               &sNot) == TRUE ) )
  while (OEMOBEXNotifier_GetCb(pMe->m_hNotifier, &sNot) == TRUE  )
  {
    int result;

    result = ISHELL_Notify( pMe->m_pIShell, AEECLSID_OBEX_NOTIFIER,
                            sNot.uID, (void*)&sNot.data );

    /*
    Free OBEX headers memory allocated for notification events.

	OEM layer allocates memory to pass data to application. 
	Notification event is syncronous call, and it is safe to release memory. 
    This callback is called from the app context, the memory being allocated  
    in app context get freed. App shall not copy or use memory pointers after 
	notification event call back return.
    */
    OEMOBEX_FreeAppNotificationEventHeaders(sNot.uID, &sNot.data);

    if ( result != SUCCESS )
    {
      DBGPRINTF( "NotifierCb - r=%d", result, 0, 0 );
    }

  }
}

#endif /* FEATURE_IOBEX */

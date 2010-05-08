

/*===========================================================================

FILE:      OEMOBEXNotifier.c

SERVICES:  IOBEX Notifier

GENERAL DESCRIPTION: BREW interface to IOBEX

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMOBEXNotifier.c#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
  $Change: 815296 $
                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/26/07   MH     Fixed OEM OBEX nofifier release to free notifier handle 
10/25/06   MH     Including rex.h  
11/13/04   sivap  initial checkin. unit tested for IOBEXServer API
2/9/04     sivap  allocate obex notifier in system memory. allow NULL pac in
                  DoNotify()
=============================================================================*/


#include "OEMFeatures.h"
#ifdef FEATURE_IOBEX

#include "BREWVersion.h"


#include "rex.h"
#include "msg.h"

#include "AEEOBEXNotifier.h"
#include "OEMOBEXNotifier.h"
#include "OEMHeap.h"

#ifdef FEATURE_BREW_3_0
    #include "AEE_OEMDispatch.h"
#endif

#include "AEE.h"
#include "AEEStdLib.h"




//==========================================================================
//   Macro definitions
//==========================================================================

/* Maximum number of OBEX  API Instances. Really, we want
 * this to be twice as big as the number of expected instances */
#define OEMOBEX_NUM_INSTANCES 40

//==========================================================================
//   Type definitions
//==========================================================================
typedef struct {
    AEEOBEXNotificationType  *m_pNotification;
    IOBEXNotifier            *m_pParent;
    uint32                    m_dwNotify;
} OEMOBEXNotifier_t;


//==========================================================================
//   Function prototypes
//==========================================================================

//==========================================================================
//   Static data
//==========================================================================

static OEMOBEXNotifier_t *pOEMOBEXNotifier;
static AEEOBEXNotificationType  gNotifications[ 20 ];
static AEEOBEXNotificationType* gpFreeNotifications = NULL;

#if MIN_BREW_VERSION(3,0)
static AEEInstance gpOBEXInstances[ OEMOBEX_NUM_INSTANCES ] = {0,};
AEEInstanceList gOEMOBEXHandleList = { gpOBEXInstances,
    OEMOBEX_NUM_INSTANCES,
    0};
#endif /* MIN_BREW_VERSION(3,0) */

//==========================================================================
//   Public Funtions
//==========================================================================

/*=============================================================================
FUNCTION: OEMOBEXNotifier_Init

DESCRIPTION:
  Allocates a new IOBEXNotifier object and initializes it or
  adds reference to an existing object.
=============================================================================*/
uint32 OEMOBEXNotifier_Init( IOBEXNotifier *pAEENotifier )
{
    uint8 i;

    if ( pOEMOBEXNotifier == NULL ) {
        //pOEMOBEXNotifier = MALLOCREC(OEMOBEXNotifier_t);
        pOEMOBEXNotifier = sys_malloc(sizeof(OEMOBEXNotifier_t));
        if (pOEMOBEXNotifier == NULL) {
            return ENOMEMORY;
        }

        // populate free notification list
        for ( i=0; i<sizeof(gNotifications)/sizeof(gNotifications[0]); i++ ) {
            gNotifications[i].pNext = gpFreeNotifications;
            gpFreeNotifications = &gNotifications[i];
        }
        pOEMOBEXNotifier->m_dwNotify = 0;
        pOEMOBEXNotifier->m_pParent = pAEENotifier;
    }

    if ( AEEHandle_To( &gOEMOBEXHandleList,
                       (OEMINSTANCE*)pOEMOBEXNotifier,
                       &pAEENotifier->m_hNotifier )
         == FALSE ) {
        //FREE(pOEMOBEXNotifier);
        sys_free(pOEMOBEXNotifier);
        pOEMOBEXNotifier = NULL;
        return EFAILED;
    }

    return SUCCESS;
}


uint32 OEMOBEXNotifier_Release( uint32 hHandle )
{
    OEMOBEXNotifier_t *pMe;
    if ( AEEHandle_From( &gOEMOBEXHandleList, hHandle, (OEMINSTANCE*)&pMe ) == FALSE ) {
        return EFAILED;
    }
    if ( pMe != pOEMOBEXNotifier ) {
        //How did we get here?
        return EFAILED;
    }


    //FREE( pMe );
    sys_free( pMe );
    AEEHandle_Clear( &gOEMOBEXHandleList, hHandle);
    pOEMOBEXNotifier = NULL;

    return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMOBEXNotifier_SetMask

DESCRIPTION:
  This function is called by Brew when an application does an
  ISHELL_RegisterNotify for the IOBEXNotifier object.  We merge
  all the masks for all the registrations and save a bitmap of
  the sum of all the events being requested.
=============================================================================*/
void OEMOBEXNotifier_SetMask( uint32     hHandle,
                              const uint32 *pdwMasks,
                              int32         dwCount
                            )
{
    OEMOBEXNotifier_t *pMe;
    if ( AEEHandle_From( &gOEMOBEXHandleList, hHandle, (OEMINSTANCE*)&pMe ) == FALSE ) {
        return;
    }
    if ( pMe != pOEMOBEXNotifier ) {
        //How did we get here?
        return;
    }

    pMe->m_dwNotify = 0;
    for (--dwCount; dwCount >= 0; --dwCount) {
        pMe->m_dwNotify |= GET_NOTIFIER_MASK( pdwMasks[dwCount] );
    }
}

static AEEOBEXNotificationType* OEMOBEX_GetFreeNotification( void )
{
    AEEOBEXNotificationType* pN;

    // allocate a notification
    TASKLOCK();
    {
        pN = gpFreeNotifications;
        if (pN) {
            gpFreeNotifications = gpFreeNotifications->pNext;
        }
    } TASKFREE();
    return pN;
}

static void OEMOBEX_EnqueueNotification( OEMOBEXNotifier_t *pMe,
                                         AEEOBEXNotificationType* pN
                                       )
{
    pN->pNext = NULL;
    // append notification to list
    TASKLOCK();
    {
        AEEOBEXNotificationType** ppN;
        for (ppN = &pMe->m_pNotification; ppN != NULL && *ppN != NULL ; ppN = &(*ppN)->pNext)
            ;
        if ( ppN != NULL )
            *ppN = pN;
    } TASKFREE();
}

boolean OEMOBEXNotifier_GetCb( uint32 hHandle, AEEOBEXNotificationType *pNot )
{
    boolean rv = FALSE;
    OEMOBEXNotifier_t *pMe;
    AEEOBEXNotificationType* pN;

    if ( AEEHandle_From( &gOEMOBEXHandleList, hHandle, (OEMINSTANCE*)&pMe ) == FALSE ) {
        return FALSE;
    }
    if ( pMe != pOEMOBEXNotifier ) {
        //How did we get here?
        return FALSE;
    }

    // Stop when we run out of notifications.
    if ( pMe->m_pNotification == NULL ) {
        return FALSE;
    }

    TASKLOCK();
    {
        pN = pMe->m_pNotification;
        pMe->m_pNotification = pMe->m_pNotification->pNext;
    } TASKFREE();

    // Copy the data:
    *pNot = *pN;
    // Don't hand back pointers to our internal table:
    pNot->pNext = NULL;
    rv = TRUE;

    // free notification
    TASKLOCK();
    {
        pN->pNext = gpFreeNotifications;
        gpFreeNotifications = pN;
    } TASKFREE();

    DBGPRINTF_LOW( "NotifierCb - sh=%x mask=%x nID=%x",
              pMe->m_pParent->m_pIShell, pMe->m_dwNotify, pN->uID );
    return rv;
}

int OEMOBEXNotifier_DoNotify( IOBEXNotifier*     pParent,
                              AEEOBEXNotificationType*  pNotification,
                              ACONTEXT*           pac )
{
    OEMOBEXNotifier_t *pMe;
    AEEOBEXNotificationType* pN;

#ifdef FEATURE_BRIDLE
    // NOTE: This should not be called from USR mode.
    if ( BRIDLE_IS_IN_USR_MODE() ) {
        DBGPRINTF_ERROR( "OEMOBEXNotifier_DoNotify called from user mode", 0, 0, 0 );
        return EFAILED;
    }
#endif /* MIN_BREW_VERSION(3,0) */

    if ( AEEHandle_From( &gOEMOBEXHandleList, pParent->m_hNotifier, (OEMINSTANCE*)&pMe )
         == FALSE ) {
        return EFAILED;
    }
    if ( pMe != pOEMOBEXNotifier ) {
        //How did we get here?
        DBGPRINTF_ERROR( "DoNotify - pMe=%x stObj=%x", pMe, pOEMOBEXNotifier, 0 );
        return EFAILED;
    }

    if ( (pMe == NULL) || (pNotification == NULL)  ) {
        DBGPRINTF_ERROR( "DoNotify - NULL ptr pN=%x ", pNotification, 0 );
        return EBADPARM;
    }

    if ( ! AEE_CHECK_PTR( "OEMOBEXNotifier_DoNotify", pNotification,
                          sizeof(AEEOBEXNotificationType), FALSE ) ) {
        return EMEMPTR;
    }

    if ( (GET_NOTIFIER_MASK(pNotification->uID) & pMe->m_dwNotify) == 0 ) {
        DBGPRINTF_ERROR( "DoNotify - bad mask uID=%x nM=%x",
                   pNotification->uID, pMe->m_dwNotify, 0 );
        return AEEOBEX_ENMASK;
    }

    if ( (pN = OEMOBEX_GetFreeNotification()) == NULL) {
        DBGPRINTF_ERROR( "DoNotify - no free notifications", 0, 0, 0 );
        return ENOMEMORY;
    }

    *pN = *pNotification;
    OEMOBEX_EnqueueNotification( pMe, pN );

    if ( ! CALLBACK_IsQueued( &pMe->m_pParent->m_cb ) ) {
        AEE_APP_RESUME( &pMe->m_pParent->m_cb, pac );
    }

    return SUCCESS;
}

#endif // ifdef FEATURE_IOBEX

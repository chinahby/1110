/*===========================================================================

FILE:      OEMBTExt.c

SERVICES:  BlueTooth QCT extension

GENERAL DESCRIPTION: BREW interface to Bluetooth 

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

      Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExt.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT)
#include "bt.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMHeap.h"

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif
#include "BREWVersion.h"

//==========================================================================
//   Macro definitions
//==========================================================================

/* Maximum number of BT API Instances for all subsystems. Really, we want 
 * this to be twice as big as the number of expected instances */
#define OEMBTEXT_NUM_INSTANCES 16

//==========================================================================
//   Type definitions
//==========================================================================
typedef struct
{
  AEEBTNotification*  m_pNotification;
  IBTExtNotifier    *m_pParent;
  uint32             m_dwNotify;
} OEMBTExtNotifier_t;


//==========================================================================
//   Function prototypes
//==========================================================================
// IBTExtNotifier VTABLE

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtNotifier_t *pOEMBTExtNotifier;
static AEEBTNotification  gNotifications[ 20 ];
static AEEBTNotification* gpFreeNotifications = NULL;

#if MIN_BREW_VERSION(3,0)
static AEEInstance gpBTExtInstances[ OEMBTEXT_NUM_INSTANCES ] = {0,};
AEEInstanceList gOEMBTExtHandleList = { gpBTExtInstances,
                                        OEMBTEXT_NUM_INSTANCES,
                                        0 };
#endif /* MIN_BREW_VERSION(3,0) */

//==========================================================================
//   Public Funtions
//==========================================================================

/*=============================================================================
FUNCTION: OEMBTExtNotifier_Init

DESCRIPTION:  
  Allocates a new IBTExtNotifier object and initializes it or 
  adds reference to an existing object.
=============================================================================*/
uint32 OEMBTExtNotifier_Init( IBTExtNotifier *pAEENotifier )
{
  uint8 i;

  if ( pOEMBTExtNotifier == NULL )
  {
    pOEMBTExtNotifier = MALLOCREC(OEMBTExtNotifier_t);

    if (pOEMBTExtNotifier == NULL)
    {
      return ENOMEMORY;
    }

    // populate free notification list
    for ( i=0; i<sizeof(gNotifications)/sizeof(gNotifications[0]); i++ )
    {
      gNotifications[i].pNext = gpFreeNotifications;
      gpFreeNotifications = &gNotifications[i];
    }
    pOEMBTExtNotifier->m_dwNotify = 0;
    pOEMBTExtNotifier->m_pParent = pAEENotifier;
  }

  if ( AEEHandle_To( &gOEMBTExtHandleList,
                     (OEMINSTANCE*)pOEMBTExtNotifier,
                     &pAEENotifier->m_hBTNotifier )
       == FALSE )
  {
    FREE(pOEMBTExtNotifier);
    pOEMBTExtNotifier = NULL;
    return EFAILED;
  }

  return SUCCESS;
}


uint32 OEMBTExtNotifier_Release( IBTHandle hHandle )
{
  OEMBTExtNotifier_t *pMe;
  if ( AEEHandle_From( &gOEMBTExtHandleList, hHandle, 
                       (OEMINSTANCE*)&pMe ) == FALSE )
  {
    return EFAILED;
  }
  if ( pMe != pOEMBTExtNotifier )
  {
    //How did we get here?
    return EFAILED;
  }

  FREE( pMe );
  (void) AEEHandle_Clear( &gOEMBTExtHandleList, hHandle);
  pOEMBTExtNotifier = NULL;

  return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMBTExtNotifier_SetMask

DESCRIPTION:  
  This function is called by Brew when an application does an
  ISHELL_RegisterNotify for the IBTExtNotifier object.  We merge
  all the masks for all the registrations and save a bitmap of
  the sum of all the events being requested.
=============================================================================*/
void OEMBTExtNotifier_SetMask( IBTHandle     hHandle,
#if MIN_BREW_BUILD(3,1,4,7)
                               uint32 *pdwMasks,
#else
                               const uint32 *pdwMasks,
#endif // MIN_BREW_BUILD(3,1,4,7)
                               int32         dwCount 
)
{
  OEMBTExtNotifier_t *pMe;
  if ( AEEHandle_From( &gOEMBTExtHandleList, hHandle, 
                       (OEMINSTANCE*)&pMe ) == FALSE )
  {
    return;
  }
  if ( pMe != pOEMBTExtNotifier )
  {
    //How did we get here?
    return;
  }

  pMe->m_dwNotify = 0;
  for ( --dwCount; dwCount >= 0; --dwCount )
  {
    pMe->m_dwNotify |= GET_NOTIFIER_MASK( pdwMasks[dwCount] );
  }
}

static void OEMBTExt_EnqueueNotification( 
  OEMBTExtNotifier_t* pMe, 
  AEEBTNotification*  pN
)
{
  // append notification to list
  TASKLOCK();
  {
    AEEBTNotification** ppN;
    for ( ppN = &pMe->m_pNotification; ppN != NULL && *ppN != NULL ; 
          ppN = &(*ppN)->pNext) 
      ;
    if( ppN != NULL )
    {
      *ppN = pN;
    }
  } TASKFREE();
}

boolean OEMBTExtNotifier_GetCb( IBTHandle hHandle, AEEBTNotification *pNot )
{
  boolean rv = FALSE;
  OEMBTExtNotifier_t* pMe;
  AEEBTNotification*  pN;

  if ( AEEHandle_From( &gOEMBTExtHandleList, hHandle, 
                      (OEMINSTANCE*)&pMe ) == FALSE )
  {
    return FALSE;
  }
  if ( pMe != pOEMBTExtNotifier )
  {
    //How did we get here?
    return FALSE;
  }

  // Stop when we run out of notifications.
  if( pMe->m_pNotification == NULL )
  {
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

  MSG_HIGH( "NotifierCb - sh=%x mask=%x nID=%x", 
            pMe->m_pParent->m_pIShell, pMe->m_dwNotify, pN->uID ); 
  return rv;
}

AEEBTNotification* OEMBTExtNotifier_GetFreeNotification( IBTExtNotifier* pParent )
{
  OEMBTExtNotifier_t* pMe;
  AEEBTNotification* pN;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBTNotifier, 
                       (OEMINSTANCE*)&pMe ) == FALSE )
  {
    return NULL;
  }

  // allocate a notification
  TASKLOCK();
  {
    pN = gpFreeNotifications;
    if (pN)
    {
      gpFreeNotifications = gpFreeNotifications->pNext;
      pN->pNext = NULL;
    }
  }
  TASKFREE();
  return pN;
}

void OEMBTExtNotifier_ReleaseNotification( IBTExtNotifier* pParent, AEEBTNotification*  pN )
{
  OEMBTExtNotifier_t* pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBTNotifier, 
                       (OEMINSTANCE*)&pMe ) == FALSE )
  {
    return;
  }
  if ( pN == NULL )
  {
    return;
  }
  
  TASKLOCK();
  {
    pN->pNext = gpFreeNotifications;
    gpFreeNotifications = pN;
  } 
  TASKFREE();
}

int OEMBTExtNotifier_DoNotify( IBTExtNotifier*     pParent, 
                               AEEBTNotification*  pNotification,
                               ACONTEXT*           pac )
{
  OEMBTExtNotifier_t* pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBTNotifier, 
                       (OEMINSTANCE*)&pMe ) == FALSE )
  {
    return EFAILED;
  }
  if ( pMe != pOEMBTExtNotifier )
  {
    //How did we get here?
    MSG_ERROR( "DoNotify - pMe=%x stObj=%x", pMe, pOEMBTExtNotifier, 0 );
    return EFAILED;
  }

  if ( (pMe == NULL) || (pNotification == NULL) || (pac == NULL) )
  {
    MSG_ERROR( "DoNotify - NULL ptr pN=%x pac=%x", pNotification, pac, 0 );
    return EBADPARM;
  }

  if ( ! AEE_CHECK_PTR( "OEMBTExtNotifier_DoNotify", pNotification, 
                        sizeof(AEEBTNotification), FALSE ) )
  {
    return EMEMPTR;
  }

  if ( (GET_NOTIFIER_MASK(pNotification->uID) & pMe->m_dwNotify) == 0 )
  {
    MSG_ERROR( "DoNotify - bad mask uID=%x nM=%x", 
               pNotification->uID, pMe->m_dwNotify, 0 );
    // free notification
    TASKLOCK();
    {
      pNotification->pNext = gpFreeNotifications;
      gpFreeNotifications = pNotification;
    } TASKFREE();
    return AEEBT_ENMASK;
  }

  OEMBTExt_EnqueueNotification( pMe, pNotification );

  if ( ! CALLBACK_IsQueued( &pMe->m_pParent->m_cb ) )
  {
    AEE_APP_RESUME( &pMe->m_pParent->m_cb, pac );
  }

  return SUCCESS;
}

AEEBTError OEMBTExt_MapCmdStatus( bt_cmd_status_type status )
{
  switch ( status )
  {
  case BT_CS_GN_SUCCESS:
  /* While connecting to secondary channels RFCOMM may return the same */
  case BT_CS_GN_PENDING:
      return AEEBT_ERR_NONE;

#ifndef FEATURE_BT_EXTPF
    // errors on the local device
    case BT_CS_PF_INVALID_PARAMETERS:
      return AEEBT_ERR_BAD_PARAM;

    case BT_CS_PF_INVALID_HANDLE:
      return AEEBT_ERR_INVALID_HANDLE;

    case BT_CS_PF_NO_RESOURCES:
      return AEEBT_ERR_RESOURCE;

    case BT_CS_PF_OUT_OF_MEMORY:
      return AEEBT_ERR_OUT_OF_MEMORY;

    case BT_CS_PF_SDP_DATABASE_OUT_OF_RESOURCES:
      return AEEBT_ERR_SD_DB_FULL;

    case BT_CS_PF_ALREADY_REGISTERED:
      return AEEBT_ERR_ALREADY_REGISTERED;

    case BT_CS_PF_NOT_REGISTERED:
      return AEEBT_ERR_NOT_REGISTERED;

    case BT_CS_PF_ALREADY_CONNECTED:
      return AEEBT_ERR_ALREADY_CONNECTED;

#ifdef FEATURE_BT_EXTPF_GOEP
    case BT_CS_PF_OBEX_NOT_CONNECTED:
      return AEEBT_ERR_NOT_CONNECTED;
#endif /* FEATURE_BT_EXTPF_GOEP */

    case BT_CS_PF_CHANNEL_NOT_FOUND:
      return AEEBT_ERR_CHANNEL_NOT_FOUND;

    case BT_CS_PF_NOT_INITIALIZED:
      return AEEBT_ERR_NOT_INITIALIZED;

    case BT_CS_PF_INITIALIZATION_FAILED:
      return AEEBT_ERR_INITIALIZATION_FAILED;

    case BT_CS_PF_NEGOTIATION_FAILURE:
      return AEEBT_ERR_NEGOTIATION_FAILURE;

#ifdef FEATURE_BT_EXTPF_GOEP
    case BT_CS_PF_OBEX_INVALID_OPERATION:
      return AEEBT_ERR_INVALID_OPERATION;

    case BT_CS_PF_OBEX_OPERATION_IN_PROGRESS:
      return AEEBT_ERR_OPERATION_IN_PROGRESS;
    
    // errors triggered by a remote response
    case BT_CS_PF_OBEX_DISCONNECT_FAILED:
      return AEEBT_ERR_OBEX_DISCONNECT_FAILED;

    case BT_CS_PF_OBEX_PUT_RESPONSE_ERROR:
      return AEEBT_ERR_OBEX_PUT_RSP;

    case BT_CS_PF_OBEX_GET_RESPONSE_ERROR:
      return AEEBT_ERR_OBEX_GET_RSP;

    case BT_CS_PF_OBEX_CONNECT_FAILED:
      return AEEBT_ERR_OBEX_CONNECT_FAILED;

    case BT_CS_PF_OBEX_ERROR:
      return AEEBT_ERR_OBEX_UNKNOWN;
    
    case BT_CS_PF_OBEX_NO_SUCH_FOLDER: // FTP specific error
      return AEEBT_ERR_OBEX_FOLDER_NOT_FOUND;

    case BT_CS_PF_OBEX_FOLDER_BROWSING_NOT_ALLOWED: // FTP specific error
      return AEEBT_ERR_OBEX_BROWSING_NOT_ALLOWED;

    // OBEX response error codes returned from the remote device
    case BT_CS_PF_OBEX_PARTIAL_CONTENT:
      return AEEBT_ERR_OBEX_PARTIAL_CONTENT;

    case BT_CS_PF_OBEX_BAD_REQUEST:
      return AEEBT_ERR_OBEX_BAD_REQUEST;

    case BT_CS_PF_OBEX_UNAUTHORIZED:
      return AEEBT_ERR_OBEX_UNAUTHORIZED;

    case BT_CS_PF_OBEX_ACCESS_DENIED:
      return AEEBT_ERR_OBEX_FORBIDDEN;

    case BT_CS_PF_OBEX_NOT_FOUND:
      return AEEBT_ERR_OBEX_NOT_FOUND;

    case BT_CS_PF_OBEX_METHOD_NOT_ALLOWED:
      return AEEBT_ERR_OBEX_METHOD_NOT_ALLOWED;

    case BT_CS_PF_OBEX_VALUE_NOT_ACCEPTABLE:
      return AEEBT_ERR_OBEX_NOT_ACCEPTABLE;

    case BT_CS_PF_OBEX_LENGTH_REQUIRED:
      return AEEBT_ERR_OBEX_LENGTH_REQUIRED;

    case BT_CS_PF_OBEX_PRECONDITION_FAILED:
      return AEEBT_ERR_OBEX_PRECONDITION_FAILED;

    case BT_CS_PF_OBEX_UNSUPPORTED_MEDIA_TYPE:
      return AEEBT_ERR_OBEX_UNSUPPORTED_MEDIA_TYPE;

    case BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR:
      return AEEBT_ERR_OBEX_INTERNAL_SERVER_ERROR;

    case BT_CS_PF_OBEX_NOT_IMPLEMENTED:
      return AEEBT_ERR_OBEX_NOT_IMPLEMENTED;

    case BT_CS_PF_OBEX_UNKNOWN_COMMAND:
      return AEEBT_ERR_OBEX_NOT_IMPLEMENTED;

    case BT_CS_PF_OBEX_SERVICE_UNAVAILABLE:
      return AEEBT_ERR_OBEX_SERVICE_UNAVAILABLE;

    case BT_CS_PF_OBEX_DATABASE_FULL:
      return AEEBT_ERR_OBEX_DATABASE_FULL;
      
    case BT_CS_PF_OBEX_DATABASE_LOCKED:
      return AEEBT_ERR_OBEX_DATABASE_LOCKED;

    case BT_CS_PF_OBEX_CLIENT_ABORTED_COMMAND:
      return AEEBT_ERR_OBEX_CLIENT_ABORTED_COMMAND;

    case BT_CS_PF_INVALID_STATE:
      return AEEBT_ERR_OBEX_CANNOT_PROCESS_COMMAND;

    case BT_CS_PF_PENDING:
      return AEEBT_ERR_OPERATION_IN_PROGRESS;

#endif // FEATURE_BT_EXTPF_GOEP
#endif // FEATURE_BT_EXTPF

    default:
      return AEEBT_ERR_UNKNOWN;
  }
}

void OEMBTExt_ResumeCallback( AEECallback* pCb )
{
#ifdef FEATURE_BREW_3_0
#if MIN_BREW_VERSIONEx(3,1,4)
  AEE_SYS_RESUME( pCb );
#elif MIN_BREW_VERSION(3,1)
  AEE_ResumeSvcCallback( pCb);
#else
  AEE_ResumeSvcCallback( pCb, 0 );
#endif /* MIN_BREW_VERSIONEx(3,1,4) */
#else
  AEE_ResumeCallback( pCb, AEE_RESUME_CB_SYS);
#endif
}

#endif // defined(FEATURE_IBT)

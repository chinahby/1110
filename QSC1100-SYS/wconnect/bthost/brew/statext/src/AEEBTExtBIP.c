/*===========================================================================

FILE:      AEEBTExtBIP.c

SERVICES:  BlueTooth Basic Imaging Profile QCT extension

GENERAL DESCRIPTION: BREW AEE interface to the Bluetooth Basic Imaging Profile.
        Most of the interface is implemented directly by the OEM layer.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/AEEBTExtBIP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BIP)

#include "AEEBTExt_priv.h"
#include "OEMBTExtBIP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

// IBTExtBIP VTABLE
static uint32 AEEBTExtBIP_AddRef( IBTExtBIP* po );
static uint32 AEEBTExtBIP_Release( IBTExtBIP* po );
static int    AEEBTExtBIP_QueryInterface( IBTExtBIP* po, 
                                          AEECLSID   class, 
                                          void**     ppo);

//==========================================================================
//   Static data
//==========================================================================

static const VTBL(IBTExtBIP) gpvtIBTExtBIP =
{
  AEEBTExtBIP_AddRef, 
  AEEBTExtBIP_Release,
  AEEBTExtBIP_QueryInterface,
  OEMBTExtBIP_RegisterImagingResponder,
  OEMBTExtBIP_RegisterArchivedObjects,
  OEMBTExtBIP_RegisterReferencedObjects,
  OEMBTExtBIP_Deregister,
  OEMBTExtBIP_AcceptConnection,
  OEMBTExtBIP_Authenticate,
  OEMBTExtBIP_Connect,
  OEMBTExtBIP_Disconnect,
  OEMBTExtBIP_Abort,
  OEMBTExtBIP_GetCapabilities,
  OEMBTExtBIP_PutImage,
  OEMBTExtBIP_PutLinkedAttachment,
  OEMBTExtBIP_PutLinkedThumbnail,
  OEMBTExtBIP_GetImageProperties,
  OEMBTExtBIP_GetImageList,
  OEMBTExtBIP_GetImage,
  OEMBTExtBIP_GetPartialImage,
  OEMBTExtBIP_GetLinkedAttachment,
  OEMBTExtBIP_GetLinkedThumbnail,
  OEMBTExtBIP_DeleteImage,
  OEMBTExtBIP_GetMonitoringImage,
  OEMBTExtBIP_GetStatus,
  OEMBTExtBIP_RemoteDisplay,
  OEMBTExtBIP_StartArchive,
  OEMBTExtBIP_StartPrint,
  OEMBTExtBIP_GetCapabilitiesResponse,
  OEMBTExtBIP_PutImageResponse,
  OEMBTExtBIP_PutLinkedAttachmentResponse,
  OEMBTExtBIP_PutLinkedThumbnailResponse,
  OEMBTExtBIP_GetImagePropertiesResponse,
  OEMBTExtBIP_GetImageListResponse,
  OEMBTExtBIP_GetImageResponse,
  OEMBTExtBIP_GetMonitoringImageResponse,
  OEMBTExtBIP_GetPartialImageResponse,
  OEMBTExtBIP_GetLinkedAttachmentResponse,
  OEMBTExtBIP_GetLinkedThumbnailResponse,
  OEMBTExtBIP_DeleteImageResponse,
  OEMBTExtBIP_RemoteDisplayResponse,
  OEMBTExtBIP_StartArchiveResponse
};

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  AEEBTExtBIP_New()

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
int AEEBTExtBIP_New(IShell* ps, AEECLSID cls, void** ppif)
{
  IBTExtBIP* pMe;

  if ( cls != AEECLSID_BLUETOOTH_BIP )
  {
    return ECLASSNOTSUPPORT;
  }

  if ( (ps == NULL) || (ppif == NULL) )
  {
    return EBADPARM;
  }

  *ppif = NULL;

  // allocate pMe
  pMe = MALLOCREC(IBTExtBIP);
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  pMe->m_pIShell = ps;
  
  // init BT layer
  if( OEMBTExtBIP_Init( pMe ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init me
  pMe->u.pvt = &gpvtIBTExtBIP;
#ifndef FEATURE_BREW_3_0
  AEE_LinkSysObject( &pMe->u.so );
#endif

  AEEBTExtBIP_AddRef(pMe);
  *ppif = pMe;

  return SUCCESS;
}


//==========================================================================
//   IBTExtBIP VTABLE (see header files for documentation)
//==========================================================================

// IBase (see AEE.h)
static uint32 AEEBTExtBIP_AddRef( IBTExtBIP* pMe )
{
  // increment ref count
  if ( pMe != NULL )
  {
    return ++pMe->m_uRefs;
  }
  return 0;
}


// IBase (see AEE.h)
static uint32 AEEBTExtBIP_Release( IBTExtBIP* pMe )
{
  // decrement ref count
  if ( pMe != NULL )
  {
    --pMe->m_uRefs;
    if ( pMe->m_uRefs > 0 )
    {
      return pMe->m_uRefs;
    }

    if( OEMBTExtBIP_Release( pMe ) != SUCCESS )
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
static int AEEBTExtBIP_QueryInterface(
  IBTExtBIP* pMe, 
  AEECLSID class, 
  void** ppo
)
{
  // pointer validation
  if (!AEE_CHECK_PTR("AEEBTExtBIP_QueryInterface", ppo, sizeof(void*), TRUE))
  {
    return EMEMPTR;
  }
  
  switch (class)
  {
    case AEECLSID_BASE:
    case AEECLSID_QUERYINTERFACE:
    case AEECLSID_BLUETOOTH_BIP:
      *ppo = pMe;
      AEEBTExtBIP_AddRef(*ppo);
      return SUCCESS;
    default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
  }
}

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BIP) */

#ifndef _OEMBTEXTBIP_H_
#define _OEMBTEXTBIP_H_

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BIP)
/*===========================================================================

FILE:      OEMBTExtBIP.h

SERVICES:  BlueTooth Basic Imaging Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Basic Imaging Profile

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtBIP.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtBIP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtBIP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtBIP_Init( IBTExtBIP* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtBIP_Release( IBTExtBIP* pParent );

// IBTEXTBIP_RegisterImagingResponder (see AEEBTExtBIP.h)
int OEMBTExtBIP_RegisterImagingResponder( 
                          IBTExtBIP*             pParent, 
                          const char*            pSrvName,
                          AEEBTBIPsupCapabilities supCapabilities,
                          AEEBTBIPsupFeatures    supFeatures,
                          AEEBTBIPsupFunctions   supFunctions,
                          uint64                 imagingCapacity,
                          boolean                auth );

// IBTEXTBIP_RegisterArchivedObjects (see AEEBTExtBIP.h)
int OEMBTExtBIP_RegisterArchivedObjects( 
                          IBTExtBIP*             pParent, 
                          const char*            pSrvName,
                          AEEBTBIPSDUUID128type* pServiceID, 
                          AEEBTBIPsupFunctions   supFunctions,
                          boolean                auth );

// IBTEXTBIP_RegisterReferencedObjects (see AEEBTExtBIP.h)
int OEMBTExtBIP_RegisterReferencedObjects( 
                          IBTExtBIP*             pParent, 
                          const char*            pSrvName,
                          AEEBTBIPSDUUID128type* pServiceID, 
                          AEEBTBIPsupFunctions   supFunctions,
                          boolean                auth );

// IBTEXTBIP_Deregister (see AEEBTExtBIP.h)
int OEMBTExtBIP_Deregister( 
                          IBTExtBIP*             pParent );

// IBTEXTBIP_AcceptConnection (see AEEBTExtBIP.h)
int OEMBTExtBIP_AcceptConnection( 
                          IBTExtBIP*             pParent );

// IBTEXTBIP_Authenticate (see AEEBTExtBIP.h)
int OEMBTExtBIP_Authenticate( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pPIN );

// IBTEXTBIP_Connect (see AEEBTExtBIP.h)
int OEMBTExtBIP_Connect( 
                          IBTExtBIP*             pParent,
                          const AEEBTBDAddr*     pBDAddr, 
                          uint8                  uChannelNumber,
                          AEEBTBIPSDUUID128type* pTarget );

// IBTEXTBIP_Disconnect (see AEEBTExtBIP.h)
int OEMBTExtBIP_Disconnect( 
                          IBTExtBIP*             pParent );

// IBTEXTBIP_Abort (see AEEBTExtBIP.h)
int OEMBTExtBIP_Abort( 
                          IBTExtBIP*             pParent );

// IBTEXTBIP_GetCapabilities (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetCapabilities( 
                          IBTExtBIP*             pParent,
                          char*                  pXMLdata,
                          uint16                 XMLdataMaxSize );

// IBTEXTBIP_PutImage (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutImage( 
                          IBTExtBIP*             pParent, 
                          const char*            pXMLdata,
                          const char*            pFileName );

// IBTEXTBIP_PutLinkedAttachment (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutLinkedAttachment( 
                          IBTExtBIP*             pParent, 
                          const AECHAR*          pImageHandle,
                          const char*            pXMLdata,
                          const char*            pFileName );

// IBTEXTBIP_PutLinkedThumbnail (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutLinkedThumbnail( 
                          IBTExtBIP*             pParent, 
                          const AECHAR*          pImageHandle,
                          const char*            pFileName );

// IBTEXTBIP_GetImageProperties (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImageProperties( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pImageHandle,
                          char*                  pXMLdata,
                          uint16                 XMLdataMaxSize );

// IBTEXTBIP_GetImageList (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImageList( 
                          IBTExtBIP*             pParent,
                          const char*            pXMLdata,
                          uint16                 numRetHandles,
                          uint16                 startOffset,
                          boolean                latest,
                          char*                  pXMLdata2,
                          uint16                 XMLdataMaxSize2 );

// IBTEXTBIP_GetImage (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImage( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pImageHandle,
                          const char*            pXMLdata,
                          const char*            pFileName );

// IBTEXTBIP_GetPartialImage (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetPartialImage( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pImageHandle,
                          uint32                 partialLength,
                          uint32                 partialOffset );

// IBTEXTBIP_GetLinkedAttachment (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetLinkedAttachment( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pImageHandle,
                          const AECHAR*          pAttachmentName );

// IBTEXTBIP_GetLinkedThumbnail (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetLinkedThumbnail( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pImageHandle,
                          const char*            pFileName );

// IBTEXTBIP_DeleteImage (see AEEBTExtBIP.h)
int OEMBTExtBIP_DeleteImage( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pImageHandle );

// IBTEXTBIP_GetMonitoringImage (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetMonitoringImage( 
                          IBTExtBIP*             pParent,
                          boolean                storeFlag,
                          const char*            pFileName );

// IBTEXTBIP_GetStatus (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetStatus( 
                          IBTExtBIP*             pParent );

// IBTEXTBIP_RemoteDisplay (see AEEBTExtBIP.h)
int OEMBTExtBIP_RemoteDisplay( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pImageHandle,
                          AEEBTBIPRDcmd          displayCommand );

// IBTEXTBIP_StartArchive (see AEEBTExtBIP.h)
int OEMBTExtBIP_StartArchive( 
                          IBTExtBIP*             pParent,
                          AEEBTBIPSDUUID128type* pServiceID );

// IBTEXTBIP_StartPrint (see AEEBTExtBIP.h)
int OEMBTExtBIP_StartPrint( 
                          IBTExtBIP*             pParent,
                          AEEBTBIPSDUUID128type* pServiceID,
                          const char*            pDPOFfileName );

// IBTEXTBIP_GetCapabilitiesResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetCapabilitiesResponse( 
                          IBTExtBIP*             pParent,
                          const char*            pXMLdata );

// IBTEXTBIP_PutImageResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutImageResponse( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pImageHandle,
                          boolean                bGetThumbnail );

// IBTEXTBIP_PutLinkedAttachmentResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutLinkedAttachmentResponse( 
                          IBTExtBIP*             pParent,
                          const char*            pFileName );

// IBTEXTBIP_PutLinkedThumbnailResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutLinkedThumbnailResponse( 
                          IBTExtBIP*             pParent,
                          const char*            pFileName );

// IBTEXTBIP_GetImagePropertiesResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImagePropertiesResponse( 
                          IBTExtBIP*             pParent,
                          const char*            pXMLdata );

// IBTEXTBIP_GetImageListResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImageListResponse( 
                          IBTExtBIP*             pParent,
                          const char*            pXMLdata,
                          const char*            pXMLdata2,
                          uint16                 numImageHandles );

// IBTEXTBIP_GetImageResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImageResponse( 
                          IBTExtBIP*             pParent,
                          const char*            pFileName );

// IBTEXTBIP_GetMonitoringImageResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetMonitoringImageResponse( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pImageHandle,
                          const char*            pFileName );

// IBTEXTBIP_GetPartialImageResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetPartialImageResponse( 
                          IBTExtBIP*             pParent,
                          const char*            pFileName,
                          uint32                 partialLength,
                          uint32                 partialOffset );

// IBTEXTBIP_GetLinkedAttachmentResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetLinkedAttachmentResponse( 
                          IBTExtBIP*             pParent,
                          const char*            pFileName );

// IBTEXTBIP_GetLinkedThumbnailResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetLinkedThumbnailResponse( 
                          IBTExtBIP*             pParent,
                          const char*            pFileName );

// IBTEXTBIP_DeleteImageResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_DeleteImageResponse( 
                          IBTExtBIP*             pParent,
                          boolean                bSuccess );

// IBTEXTBIP_RemoteDisplayResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_RemoteDisplayResponse( 
                          IBTExtBIP*             pParent,
                          const AECHAR*          pImageHandle,
                          boolean                bSuccess );

// IBTEXTBIP_StartArchiveResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_StartArchiveResponse( 
                          IBTExtBIP*             pParent,
                          boolean                bSuccess );

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BIP)
#endif // _OEMBTEXTBIP_H_

/*===========================================================================

FILE:      OEMBTExtBIP.c

SERVICES:  BlueTooth Basic Imaging Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Basic Imaging Profile.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2004-2009 by QUALCOMM Incorporated. 
                All Rights Reserved.
            Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtBIP.c#3 $
$DateTime: 2009/05/27 09:43:29 $
$Author: phuongn $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2009-05-27   pn  Fixed test feature handling BIP_SRV_START_PRINT_IND.
2009-05-22   pn  GetImageListResponse() updated to accept NULL data pointers.
2008-10-07   pn  Replaced deprecated functions.

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BIP)
#include "bt.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "oi_obexspec.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtBIP.h"

//==========================================================================
//   Macro definitions
//==========================================================================
#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
#define BIP_DATA_BUFFER_SIZE   AEEBT_OBEX_BUFFER_SIZE
#else
#define BIP_DATA_BUFFER_SIZE   32757
#endif /* FEATURE_BT_REDUCE_MEMORY_USAGE */
#else
#define BIP_DATA_BUFFER_SIZE   4266
#endif /* FEATURE_BT_SOC */

//==========================================================================
//   Type definitions
//==========================================================================

typedef struct OEMBTExtBIPobj_struct
{
  ACONTEXT*                   pac;
  IBTExtNotifier*             pNotifier;

  bt_app_id_type              appId;
  AEEBTBDAddr                 BDAddr;         // remote device's

  IShell*                     pIShell;

  IFile*                      pFile;
  FileInfo                    fileInfo;

  AECHAR                      wImageName[ AEEBT_MAX_FILE_NAME + 1];
  AECHAR                      wAttachmentName[ AEEBT_MAX_FILE_NAME + 1];
  AECHAR                      wImageHandle[ AEEBT_BIP_IMAGE_HANDLE_LEN + 1];

  char*                       pUserBuffer;
  uint16                      userBufferMaxSize;

  // Pointer to XML string sent as response by the Server
  char*                       pXMLdata;
  uint16                      XMLdataLen;

  uint32                      bytesSent;
  uint32                      bytesRcvd;

  uint16                      serverConnID;
  uint16                      clientConnID;

  char*                       pDataBuffer;

  // Image and Image descriptor bytes and len
  AEEBTBIPbyteSeqType         data;    
  AEEBTBIPbyteSeqType         descData;

  // Which server - primary, secondary or neither
  boolean                     bIsServer;
  boolean                     bIsSecServer;

  boolean                     bDoDeregister; 

  // ServiceID of archived object server / referenced object server
  AEEBTBIPSDUUID128type       serviceID;
  AEEBTBIPServiceType         serviceType;

  // Handle to Client data
  uint8*                      pData;
  uint16                      dataLen;

  boolean                     bFinal;

  // PutImage - whether to request thumbnail version
  boolean                     bGetThumbnail;

  // GetPartialImage
  uint32                      partialLength;
  uint32                      partialOffset;

  // GetMonitoringImage
  boolean                     storeFlag;

  // GetImageList
  uint16                      numReturnedHandles;
  uint16                      startOffset;
  boolean                     latest;

  AEECallback                 cb;

  struct OEMBTExtBIPobj_struct* pNextBIP;  // linked into mgr
} OEMBTExtBIPobj_t;

typedef struct
{
  boolean                     bInUse;
  boolean                     final;
  bt_event_type               event;
  bt_app_id_type              appId;
  uint16                      connId;

  uint16                      state;
  bt_cmd_status_type          status;

  uint8*                      pData;
  uint16                      dataLen;

  uint16                      numHandles;

  uint8*                      pImageDesc;
  uint16                      descLen;

  boolean                     bServerEvt;
  AEECallback                 cb;
} OEMBTExtBIP_EvCb;

// one of these
typedef struct
{
  OEMBTExtBIPobj_t* pNextBIP;    // singly linked list of BIPs
  OEMBTExtBIP_EvCb  FreeEvCb[ 10 ];
} OEMBTExtBIP_Mgr;

//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static int  OEMBTExtBIP_DeregisterServer( OEMBTExtBIPobj_t* pParent );
static int  OEMBTExtBIP_CheckCmdStatus( bt_cmd_status_type stat );
static void OEMBTExtBIP_DisconnectCb( OEMBTExtBIP_EvCb* pEvCb );
static void OEMBTExtBIP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);
static OEMBTExtBIP_EvCb* OEMBTExtBIP_GetFreeEvCb( void );

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtBIP_Mgr        gMgr;               // manager object

//==========================================================================
//   Public Funtions
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
int OEMBTExtBIP_Init(IBTExtBIP* pParent)
{
  OEMBTExtBIPobj_t* pMe;

  // allocate pMe
  pMe = MALLOCREC(OEMBTExtBIPobj_t);
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  if ( ISHELL_CreateInstance( pParent->m_pIShell, 
                              AEECLSID_BLUETOOTH_NOTIFIER, 
                              (void**) &pMe->pNotifier ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  // init BT layer
  pMe->appId = bt_cmd_ec_get_app_id_and_register(OEMBTExtBIP_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    FREE( pMe );
    return EFAILED;
  }

  if ( ( pMe->pDataBuffer = MALLOC( BIP_DATA_BUFFER_SIZE ) ) == NULL )
  {
    (void)bt_cmd_ec_free_application_id( pMe->appId );
    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    FREE( pMe );
    return EFAILED;
  }
  
  pMe->pac = AEE_GetAppContext();

  if ( AEEHandle_To( &gOEMBTExtHandleList,
                     (OEMINSTANCE*)pMe,
                     &pParent->m_hBT ) == FALSE )
  {
    FREE(pMe);
    return EFAILED;
  }

  if ( gMgr.pNextBIP == NULL ) // first time?
  {
    uint8 i;
    for ( i = 0; i < (sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++ )
    {
      gMgr.FreeEvCb[ i ].bInUse = FALSE;
    }
  }

  pMe->pIShell = pParent->m_pIShell;

  // link into mgr
  pMe->pNextBIP = gMgr.pNextBIP;
  gMgr.pNextBIP = pMe;

  // More initialization
  pMe->bIsServer = FALSE;
  pMe->bIsSecServer = FALSE;

  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtBIP_Release( IBTExtBIP* pParent )
{
  OEMBTExtBIPobj_t*  pMe;
  OEMBTExtBIPobj_t** ppc;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) == TRUE )
  {
    // unlink from mgr
    for (ppc = &gMgr.pNextBIP; *ppc != NULL; ppc = &(*ppc)->pNextBIP)
    {
      if ((*ppc)->appId == pMe->appId)
      {
        *ppc = pMe->pNextBIP;
        pMe->pNextBIP = NULL;
        break;
      }
    }

    if ( pMe->pDataBuffer )  
    {
      FREE( pMe->pDataBuffer );
    }

    (void)bt_cmd_ec_free_application_id( pMe->appId );
    pMe->appId = BT_APP_ID_NULL;

    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    pMe->pNotifier = NULL;
    pMe->pac = NULL;

    FREE(pMe);
    (void) AEEHandle_Clear( &gOEMBTExtHandleList, pParent->m_hBT );

    return SUCCESS;
  }

  return EFAILED;
}

// IBTEXTBIP_RegisterImagingResponder (see AEEBTExtBIP.h)
int OEMBTExtBIP_RegisterImagingResponder( 
  IBTExtBIP*               pParent, 
  const char*              pSrvName,
  AEEBTBIPsupCapabilities  supCapabilities,
  AEEBTBIPsupFeatures      supFeatures,
  AEEBTBIPsupFunctions     supFunctions,
  uint64                   imagingCapacity,
  boolean                  auth )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t   *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pSrvName == NULL )
  {
    return EBADPARM;
  }

  pMe->serviceType = AEEBT_BIP_IMAGING_RESPONDER;

  stat = bt_cmd_pf_bip_srv_register_imaging_responder(  
                                     pMe->appId,  
                                     (char*)pSrvName,
                                     supCapabilities,
                                     supFeatures,
                                     supFunctions,
                                     imagingCapacity,
                                     auth );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_RegisterArchivedObjects(see AEEBTExtBIP.h)
int OEMBTExtBIP_RegisterArchivedObjects( 
  IBTExtBIP*             pParent, 
  const char*            pSrvName,
  AEEBTBIPSDUUID128type* pServiceID,
  AEEBTBIPsupFunctions   supFunctions,
  boolean                auth )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t   *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pSrvName == NULL )
  {
    return EBADPARM;
  }

  pMe->serviceType = AEEBT_BIP_ARCHIVED_OBJECTS;
 
  stat = bt_cmd_pf_bip_srv_register_archived_objects( 
                                           pMe->appId,
                                           (char*) pSrvName,
                                           (bt_sd_uuid128_type*) pServiceID,
                                           supFunctions,
                                           auth );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_RegisterReferencedObjects(see AEEBTExtBIP.h)
int OEMBTExtBIP_RegisterReferencedObjects( 
  IBTExtBIP*             pParent, 
  const char*            pSrvName,
  AEEBTBIPSDUUID128type* pServiceID,
  AEEBTBIPsupFunctions   supFunctions,
  boolean                auth )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t   *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pSrvName == NULL )
  {
    return EBADPARM;
  }

  pMe->serviceType = AEEBT_BIP_REFERENCED_OBJECTS;
 
  stat = bt_cmd_pf_bip_srv_register_referenced_objects( 
                                           pMe->appId,
                                           (char*) pSrvName,
                                           (bt_sd_uuid128_type*) pServiceID,
                                           supFunctions,
                                           auth );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_Deregister (see AEEBTExtBIP.h)
int OEMBTExtBIP_Deregister( IBTExtBIP* pParent )
{
  int                         stat;
  OEMBTExtBIPobj_t            *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pMe->serverConnID != BT_PF_BIP_NO_CONN_ID ) {
    // We have a connection up.  Tear it down.
    stat = bt_cmd_pf_bip_srv_force_disconnect( pMe->appId, 
                                               pMe->serverConnID );
    if ( stat == BT_CS_GN_PENDING )
    {
      pMe->bDoDeregister = TRUE;
      stat = OEMBTExtBIP_CheckCmdStatus( BT_CS_GN_SUCCESS );
    }
  }
  else
  {
    stat = OEMBTExtBIP_DeregisterServer( pMe );
  }

  return stat;
}

// IBTEXTBIP_DeregisterServer
static int OEMBTExtBIP_DeregisterServer( OEMBTExtBIPobj_t* pMe )
{
  bt_cmd_status_type          stat;

  if( !pMe )
  {
    return EFAILED;
  }

  if ( pMe->serviceType == AEEBT_BIP_IMAGING_RESPONDER ) 
  {
    stat = bt_cmd_pf_bip_srv_deregister_imaging_responder( pMe->appId );
  }
  else if ( pMe->serviceType == AEEBT_BIP_ARCHIVED_OBJECTS ) 
  {
    stat = bt_cmd_pf_bip_srv_deregister_archived_objects( pMe->appId );
  }
  else
  {
    stat = bt_cmd_pf_bip_srv_deregister_referenced_objects( pMe->appId );
  }

  return OEMBTExtBIP_CheckCmdStatus( stat );
}


// IBTEXTBIP_AcceptConnection (see AEEBTExtBIP.h)
int OEMBTExtBIP_AcceptConnection( 
  IBTExtBIP*          pParent
)
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t    *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_CON_IND, 
                                    BT_CS_GN_SUCCESS );


  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_Authenticate (see AEEBTExtBIP.h)
int OEMBTExtBIP_Authenticate( 
  IBTExtBIP*          pParent,
  const AECHAR*       pPIN
)
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t    *pMe;

  char sPIN[ AEEBT_MAX_PASSWORD_LEN + 1];

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  AEEBT_FROM_WSTR( pPIN, sPIN, sizeof( sPIN ) ); 
  
  if ( pMe->bIsServer == TRUE ) 
  {
    stat = bt_cmd_pf_bip_srv_auth_response( 
                                    pMe->appId, 
                                    pMe->serverConnID,
                                    sPIN);
  }
  else
  {
    stat = bt_cmd_pf_bip_cli_auth_response( 
                                    pMe->appId, 
                                    pMe->clientConnID,
                                    sPIN);
  }


  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_Connect (see AEEBTExtBIP.h)
int OEMBTExtBIP_Connect( 
  IBTExtBIP*             pParent, 
  const AEEBTBDAddr*     pBDAddr, 
  uint8                  uChannelNumber,
  AEEBTBIPSDUUID128type* pTarget
)
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t    *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_pf_bip_cli_connect( pMe->appId, 
                                    (bt_bd_addr_type*) pBDAddr,
                                    uChannelNumber,
                                    (byte*) pTarget );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_Abort (see AEEBTExtBIP.h)
int OEMBTExtBIP_Abort( IBTExtBIP *pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_bip_cli_abort( pMe->appId, pMe->clientConnID );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_Disconnect (see AEEBTExtBIP.h)
int OEMBTExtBIP_Disconnect( IBTExtBIP* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_bip_cli_disconnect( pMe->appId, pMe->clientConnID );
 
  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetCapabilities (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetCapabilities( 
  IBTExtBIP* pParent, 
  char*      pXMLdata,
  uint16     XMLdataMaxSize )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pXMLdata == NULL ) 
  {
    return EBADPARM;
  }

  // Return server imaging capabilities information in the user supplied
  // buffer 
  pMe->bytesRcvd         = 0;
  pMe->pUserBuffer       = pXMLdata;
  pMe->userBufferMaxSize = XMLdataMaxSize;

  stat = bt_cmd_pf_bip_cli_get_capabilities( pMe->appId, pMe->clientConnID );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetImageProperties (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImageProperties( 
  IBTExtBIP*    pParent, 
  const AECHAR* pImageHandle,
  char*         pXMLdata,
  uint16        XMLdataMaxSize )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pXMLdata == NULL ) 
  {
    return EBADPARM;
  }

  // Return image properties information in the user supplied
  // buffer 
  pMe->bytesRcvd         = 0;
  pMe->pUserBuffer       = pXMLdata;
  pMe->userBufferMaxSize = XMLdataMaxSize;

  WSTRLCPY( pMe->wImageHandle, pImageHandle, ARR_SIZE( pMe->wImageHandle ) );

  stat = bt_cmd_pf_bip_cli_get_image_properties( 
                         pMe->appId, 
                         pMe->clientConnID,
                         (bt_pf_bip_image_handle_type*) pMe->wImageHandle );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetImageList (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImageList( 
  IBTExtBIP*    pParent,
  const char*   pXMLdata,
  uint16        numReturnedHandles,
  uint16        startOffset,
  boolean       latest,
  char*         pXMLdata2,
  uint16        XMLdataMaxSize2 )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pXMLdata2 == NULL  ) 
  {
    return EBADPARM;
  }

  pMe->descData.AEEBTBIPdata    = (byte *) pXMLdata;
  pMe->descData.AEEBTBIPdataLen = STRLEN( pXMLdata );

  // Return image properties information in the user supplied
  // buffer 
  pMe->bytesRcvd          = 0;
  pMe->pUserBuffer        = pXMLdata2;
  pMe->userBufferMaxSize  = XMLdataMaxSize2;

  pMe->numReturnedHandles = numReturnedHandles;
  pMe->startOffset        = startOffset;
  pMe->latest             = latest;

  stat = bt_cmd_pf_bip_cli_get_images_list( 
                         pMe->appId, 
                         pMe->clientConnID,
                         pMe->numReturnedHandles,
                         pMe->startOffset,
                         pMe->latest,
                         (bt_pf_byte_seq_type*) &pMe->descData );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetImage (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImage( 
  IBTExtBIP*    pParent, 
  const AECHAR* pImageHandle,
  const char*   pXMLdata,
  const char*   pFileName )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;
  IFileMgr* pIFileMgr       = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pImageHandle == NULL || pFileName == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }
  IFILEMGR_Remove( pIFileMgr, pFileName );
  if ( (pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                        pFileName, 
                                        _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return EFAILED;
  }

  WSTRLCPY( pMe->wImageHandle, pImageHandle, ARR_SIZE( pMe->wImageHandle ) );

  pMe->bytesRcvd                = 0;
  pMe->descData.AEEBTBIPdata    = (byte *) pXMLdata;
  pMe->descData.AEEBTBIPdataLen = STRLEN( pXMLdata );

  if ( pIFileMgr ) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  stat = bt_cmd_pf_bip_cli_get_image( 
                         pMe->appId, 
                         pMe->clientConnID,
                         (bt_pf_bip_image_handle_type*) pImageHandle,
                         (bt_pf_byte_seq_type*) &pMe->descData );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetPartialImage (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetPartialImage( 
  IBTExtBIP*    pParent, 
  const AECHAR* pFileName,
  uint32        partialLength,
  uint32        partialOffset )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;
  IFileMgr* pIFileMgr       = NULL;

  char sImageName[ AEEBT_MAX_FILE_NAME + 1];

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pFileName == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  AEEBT_FROM_WSTR( pFileName, sImageName, sizeof( sImageName ) ); 

  IFILEMGR_Remove( pIFileMgr, sImageName );
  if ( (pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                        sImageName, 
                                        _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return EFAILED;
  }

  WSTRLCPY( pMe->wImageName, pFileName, ARR_SIZE( pMe->wImageName ) );
  pMe->partialLength            = partialLength;
  pMe->partialOffset            = partialOffset;
  pMe->bytesRcvd                = 0;

  if ( pIFileMgr ) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  stat = bt_cmd_pf_bip_cli_get_partial_image( 
                         pMe->appId, 
                         pMe->clientConnID,
                         (uint16*) pFileName,
                         partialLength,
                         partialOffset );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetLinkedAttachment (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetLinkedAttachment( 
  IBTExtBIP*    pParent, 
  const AECHAR* pImageHandle,
  const AECHAR* pAttachmentName )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;
  IFileMgr* pIFileMgr       = NULL;

  char sAttachmentName[ AEEBT_MAX_FILE_NAME + 1];

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pImageHandle == NULL || pAttachmentName == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
      return EFAILED;
  }


  AEEBT_FROM_WSTR( pAttachmentName, sAttachmentName, sizeof( sAttachmentName ) ); 

  IFILEMGR_Remove( pIFileMgr, sAttachmentName );
  if ( (pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                        sAttachmentName, 
                                        _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return EFAILED;
  }

  if ( pIFileMgr ) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  // Save name of attachment and image handle
  WSTRLCPY( pMe->wAttachmentName, pAttachmentName, 
            ARR_SIZE( pMe->wAttachmentName ) );
  WSTRLCPY( pMe->wImageHandle, pImageHandle, ARR_SIZE( pMe->wImageHandle ) );
  pMe->bytesRcvd = 0;

  stat = bt_cmd_pf_bip_cli_get_linked_attachment( 
                         pMe->appId, 
                         pMe->clientConnID,
                         (bt_pf_bip_image_handle_type*) pImageHandle,
                         (uint16*) pAttachmentName );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetLinkedThumbnail (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetLinkedThumbnail( 
  IBTExtBIP*    pParent, 
  const AECHAR* pImageHandle,
  const char*   pFileName )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;
  IFileMgr* pIFileMgr       = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pImageHandle == NULL || pFileName == NULL ) 
  {
    return EBADPARM;
  }

  WSTRLCPY( pMe->wImageHandle, pImageHandle, ARR_SIZE( pMe->wImageHandle ) );
  pMe->bytesRcvd = 0;

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
      return EFAILED;
  }

  IFILEMGR_Remove( pIFileMgr, pFileName );
  if ( (pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                        pFileName, 
                                        _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return EFAILED;
  }

  if ( pIFileMgr ) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  stat = bt_cmd_pf_bip_cli_get_linked_thumbnail( 
                         pMe->appId, 
                         pMe->clientConnID,
                         (bt_pf_bip_image_handle_type*) pImageHandle);

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetMonitoringImage (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetMonitoringImage( 
  IBTExtBIP*  pParent, 
  boolean     storeFlag,
  const char* pFileName )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;
  IFileMgr* pIFileMgr       = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pFileName == NULL ) 
  {
    return EFAILED;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }
  IFILEMGR_Remove( pIFileMgr, pFileName );
  if ( (pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                        pFileName, 
                                       _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return EFAILED;
  }

  if ( pIFileMgr ) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  pMe->bytesRcvd = 0;
  pMe->storeFlag = storeFlag;

  stat = bt_cmd_pf_bip_cli_get_monitoring_image( pMe->appId, 
                                                 pMe->clientConnID,
                                                 pMe->storeFlag );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetStatus (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetStatus( IBTExtBIP* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_bip_cli_get_status( pMe->appId, 
                                       pMe->clientConnID );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_DeleteImage (see AEEBTExtBIP.h)
int OEMBTExtBIP_DeleteImage( 
  IBTExtBIP*    pParent,
  const AECHAR* pImageHandle )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pImageHandle == NULL ) 
  {
    return EBADPARM;
  }

  stat = bt_cmd_pf_bip_cli_delete_image( pMe->appId, 
                                         pMe->clientConnID,
                                         (bt_pf_bip_image_handle_type*) pImageHandle );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_RemoteDisplay (see AEEBTExtBIP.h)
int OEMBTExtBIP_RemoteDisplay( 
  IBTExtBIP*    pParent,
  const AECHAR* pImageHandle,
  AEEBTBIPRDcmd displayCommand )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pImageHandle == NULL && 
       displayCommand == AEEBT_BIP_REMOTE_DISPLAY_SELECT_IMAGE_CMD ) 
  {
    // Image handle must be present if display command is "Select" image
    return EBADPARM;
  }

  stat = bt_cmd_pf_bip_cli_remote_display( pMe->appId, 
                                           pMe->clientConnID,
                                           (bt_pf_bip_image_handle_type*) pImageHandle,
                                           (uint8) displayCommand );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_StartArchive (see AEEBTExtBIP.h)
int OEMBTExtBIP_StartArchive( 
  IBTExtBIP*             pParent,
  AEEBTBIPSDUUID128type* pServiceID )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pServiceID == NULL ) 
  {
    return EBADPARM;
  }

  stat = bt_cmd_pf_bip_cli_start_archive( pMe->appId, 
                                          pMe->clientConnID,
                                          (bt_sd_uuid128_type*) pServiceID );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_StartPrint (see AEEBTExtBIP.h)
int OEMBTExtBIP_StartPrint( 
  IBTExtBIP*             pParent,
  AEEBTBIPSDUUID128type* pServiceID,
  const char*            pDPOFfileName )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  int       bytesRead       = 0;
  boolean   final           = 0;
  IFileMgr* pIFileMgr       = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pServiceID == NULL || pDPOFfileName == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  if ( (pMe->pFile = 
        IFILEMGR_OpenFile( pIFileMgr, pDPOFfileName, _OFM_READ )) == NULL )
  {
    MSG_ERROR( "StartPrint: open DPOF file failed", 0, 0, 0 );
    return EFAILED;
  }

  // get filesize info
  if ( IFILE_GetInfo( pMe->pFile, &pMe->fileInfo ) == EFAILED )
  {
    return EFAILED;
  }

  bytesRead = BIP_DATA_BUFFER_SIZE;
  bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
  if ( bytesRead == 0 )
  {
    MSG_ERROR( "StartPrint: failed to read from obj", 0, 0, 0 );
    return EFAILED;
  }
  else if ( bytesRead >= pMe->fileInfo.dwSize )
  {
    final  = 1;
  }

  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;
  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->bytesSent            = bytesRead;

  MEMCPY( (void*) &pMe->serviceID, (void*) &pServiceID, sizeof (pMe->serviceID ) );

  IFILEMGR_Release( pIFileMgr );

  stat = bt_cmd_pf_bip_cli_start_print( pMe->appId, 
                                        pMe->clientConnID,
                                        (bt_sd_uuid128_type*) pServiceID,
                                        (bt_pf_byte_seq_type*) &pMe->data,
                                        final );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_PutImage (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutImage( 
  IBTExtBIP*  pParent, 
  const char* pXMLdata,
  const char* pFileName )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  int       bytesRead       = 0;
  boolean   final           = 0;
  IFileMgr* pIFileMgr       = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pFileName == NULL || pXMLdata == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  if ( (pMe->pFile = 
        IFILEMGR_OpenFile( pIFileMgr, pFileName, _OFM_READ )) == NULL )
  {
    MSG_ERROR( "PutImage: open img file failed", 0, 0, 0 );
    return EFAILED;
  }

  // get filesize info
  if ( IFILE_GetInfo( pMe->pFile, &pMe->fileInfo ) == EFAILED )
  {
    return EFAILED;
  }

  bytesRead = BIP_DATA_BUFFER_SIZE;
  bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
  if ( bytesRead == 0 )
  {
    MSG_ERROR( "PutImage: failed to read from obj", 0, 0, 0 );
    return EFAILED;
  }
  else if ( bytesRead >= pMe->fileInfo.dwSize )
  {
    final  = 1;
  }

  AEEBT_TO_WSTR( pFileName, pMe->wImageName, sizeof(pMe->wImageName) );

  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;
  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->bytesSent            = bytesRead;

  pMe->descData.AEEBTBIPdata    = (byte *) pXMLdata;
  pMe->descData.AEEBTBIPdataLen = STRLEN( pXMLdata );

  IFILEMGR_Release( pIFileMgr );

  stat = bt_cmd_pf_bip_cli_put_image( pMe->appId, 
                                      pMe->clientConnID,
                                      pMe->wImageName,
                                      (bt_pf_byte_seq_type*) &pMe->descData,
                                      (bt_pf_byte_seq_type*) &pMe->data,
                                      final );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_PutLinkedAttachment (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutLinkedAttachment( 
  IBTExtBIP*    pParent, 
  const AECHAR* pImageHandle,
  const char*   pXMLdata, 
  const char*   pFileName )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  int       bytesRead       = 0;
  boolean   final           = 0;
  IFileMgr* pIFileMgr       = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pFileName == NULL || pXMLdata == NULL || pImageHandle == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  if ( (pMe->pFile = 
        IFILEMGR_OpenFile( pIFileMgr, pFileName, _OFM_READ )) == NULL )
  {
    MSG_ERROR( "PutLinkedAttachment: open attachment file failed", 0, 0, 0 );
    return EFAILED;
  }

  // get filesize info
  if ( IFILE_GetInfo( pMe->pFile, &pMe->fileInfo ) == EFAILED )
  {
    return EFAILED;
  }

  bytesRead = BIP_DATA_BUFFER_SIZE;
  bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
  if ( bytesRead == 0 )
  {
    MSG_ERROR( "PutLinkedAttachment: failed to read from obj", 0, 0, 0 );
    return EFAILED;
  }
  else if ( bytesRead >= pMe->fileInfo.dwSize )
  {
    final  = 1;
  }

  AEEBT_TO_WSTR( pFileName, pMe->wAttachmentName, sizeof(pMe->wAttachmentName) );

  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;
  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->bytesSent            = bytesRead;

  pMe->descData.AEEBTBIPdata    = (byte *) pXMLdata;
  pMe->descData.AEEBTBIPdataLen = STRLEN( pXMLdata );

  IFILEMGR_Release( pIFileMgr );

  stat = bt_cmd_pf_bip_cli_put_linked_attachment( 
                                   pMe->appId, 
                                   pMe->clientConnID,
                                   (bt_pf_bip_image_handle_type*) pImageHandle,
                                   (bt_pf_byte_seq_type*) &pMe->descData,
                                   (bt_pf_byte_seq_type*) &pMe->data,
                                   final );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_PutLinkedThumbnail(see AEEBTExtBIP.h)
int OEMBTExtBIP_PutLinkedThumbnail( 
  IBTExtBIP*    pParent, 
  const AECHAR* pImageHandle,
  const char*   pFileName )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t *pMe;

  int       bytesRead       = 0;
  boolean   final           = 0;
  IFileMgr* pIFileMgr       = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pFileName == NULL || pImageHandle == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  if ( (pMe->pFile = 
        IFILEMGR_OpenFile( pIFileMgr, pFileName, _OFM_READ )) == NULL )
  {
    MSG_ERROR( "PutLinkedThumbnail: open img file failed", 0, 0, 0 );
    return EFAILED;
  }

  // get filesize info
  if ( IFILE_GetInfo( pMe->pFile, &pMe->fileInfo ) == EFAILED )
  {
    return EFAILED;
  }

  WSTRLCPY( pMe->wImageHandle, pImageHandle, ARR_SIZE( pMe->wImageHandle ) );

  bytesRead = BIP_DATA_BUFFER_SIZE;
  bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
  if ( bytesRead == 0 )
  {
    MSG_ERROR( "PutLinkedThumbnail: failed to read from obj", 0, 0, 0 );
    return EFAILED;
  }
  else if ( bytesRead >= pMe->fileInfo.dwSize )
  {
    final  = 1;
  }

  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;
  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->bytesSent            = bytesRead;

  IFILEMGR_Release( pIFileMgr );

  stat = bt_cmd_pf_bip_cli_put_linked_thumbnail( 
                                   pMe->appId, 
                                   pMe->clientConnID,
                                   (bt_pf_bip_image_handle_type*) pImageHandle,
                                   (bt_pf_byte_seq_type*) &pMe->data,
                                   final );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetCapabilitiesResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetCapabilitiesResponse( 
  IBTExtBIP*  pParent, 
  const char* pXMLdata )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;
  uint16             bytesRead = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return BT_CS_PF_INTERNAL_ERROR;
  }

  // Save pointer to user provided XML string locally
  pMe->pXMLdata   = (char*) pXMLdata;
  pMe->XMLdataLen = STRLEN( pXMLdata );

  pMe->bytesSent = 0;
  if ( pMe->bytesSent + BIP_DATA_BUFFER_SIZE < pMe->XMLdataLen ) 
  {
    bytesRead = BIP_DATA_BUFFER_SIZE;
  }
  else
  {
    bytesRead = pMe->XMLdataLen;
    status    = BT_CS_GN_SUCCESS;

    // No more data to send
    final  = 1;
  }

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) &pMe->pXMLdata[ 0 ];

  pMe->bytesSent += bytesRead;

  // Special case.  We expect XML string to be non-empty
  if ( bytesRead == 0 ) 
  {
    (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                      pMe->serverConnID,
                                      BT_EV_PF_BIP_SRV_GET_CAPABILITIES_IND, 
                                      BT_CS_PF_OBEX_ERROR );

    return EFAILED;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_CAPABILITIES_IND, 
                                    BT_CS_GN_SUCCESS );

  stat = bt_cmd_pf_bip_srv_get_capabilities_response(
                                       pMe->appId, 
                                       pMe->serverConnID,
                                       (bt_pf_byte_seq_type*) &pMe->data,
                                       final,
                                       status );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_PutImageResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutImageResponse( 
  IBTExtBIP*    pParent, 
  const AECHAR* pImageHandle,
  boolean       bGetThumbnail )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t*   pMe;
  bt_cmd_status_type  status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*           pIFileMgr = NULL;

  char sImageName[ AEEBT_MAX_FILE_NAME + 1 ];

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return BT_CS_PF_INTERNAL_ERROR;
  }

  // Save in local variables
  WSTRLCPY( pMe->wImageHandle, pImageHandle, ARR_SIZE( pMe->wImageHandle ) );
  pMe->bGetThumbnail = bGetThumbnail;

  // Sanity check
  if ( pMe->pFile ) 
  {
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
  }

  AEEBT_FROM_WSTR( pMe->wImageName, sImageName, sizeof( sImageName ) );

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
      return BT_CS_PF_INTERNAL_ERROR;
  }

  // Open file for writing (delete, first if it exists)
  IFILEMGR_Remove( pIFileMgr, sImageName );
  if ( ( pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                         sImageName, 
                                         _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return BT_CS_PF_INTERNAL_ERROR;
  }

  if ( IFILE_Write( pMe->pFile, pMe->pData, pMe->dataLen ) != pMe->dataLen )
  {
    MSG_ERROR( "PutImageResponse: failed to write to obj", 0, 0, 0 );
    
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    status = BT_CS_PF_WRITE_ERROR;
  }
  else
  {
    if (pMe->bFinal == 1) 
    {
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }

  if (pIFileMgr) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_PUT_IMAGE_IND, 
                                    BT_CS_GN_SUCCESS );

  stat = bt_cmd_pf_bip_srv_put_image_response(
                         pMe->appId, 
                         pMe->serverConnID,
                         (bt_pf_bip_image_handle_type*) pMe->wImageHandle,
                         status );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_PutLinkedAttachmentResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutLinkedAttachmentResponse( 
  IBTExtBIP*  pParent,
  const char* pFileName )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t*   pMe;
  bt_cmd_status_type  status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*           pIFileMgr = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return BT_CS_PF_INTERNAL_ERROR;
  }

  // Sanity check
  if ( pMe->pFile ) 
  {
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
      return BT_CS_PF_INTERNAL_ERROR;
  }

  // Open file for writing (delete, first if it exists)
  IFILEMGR_Remove( pIFileMgr, pFileName );
  if ( ( pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                         pFileName, 
                                         _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return BT_CS_PF_INTERNAL_ERROR;
  }

  if ( IFILE_Write( pMe->pFile, pMe->pData, pMe->dataLen ) != pMe->dataLen )
  {
    MSG_ERROR( "PutLinkedAttachmentResponse: failed to write to obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    status = BT_CS_PF_WRITE_ERROR;
  }
  else
  {
    if (pMe->bFinal == 1) 
    {
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }

  if (pIFileMgr) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( 
                         pMe->appId, 
                         pMe->serverConnID,
                         BT_EV_PF_BIP_SRV_PUT_LINKED_ATTACHMENT_IND, 
                         BT_CS_GN_SUCCESS );

  stat = bt_cmd_pf_bip_srv_put_linked_attachment_response(
                         pMe->appId, 
                         pMe->serverConnID,
                         status );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_PutLinkedThumbnailResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_PutLinkedThumbnailResponse( 
  IBTExtBIP*  pParent,
  const char* pFileName )
{
  bt_cmd_status_type  stat;
  OEMBTExtBIPobj_t*   pMe;
  bt_cmd_status_type  status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*           pIFileMgr = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return BT_CS_PF_INTERNAL_ERROR;
  }

  // Sanity check
  if ( pMe->pFile ) 
  {
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
      return BT_CS_PF_INTERNAL_ERROR;
  }

  // Open file for writing (delete first, if it exists)
  IFILEMGR_Remove( pIFileMgr, pFileName );
  if ( ( pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                         pFileName, 
                                         _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return BT_CS_PF_INTERNAL_ERROR;
  }

  if ( IFILE_Write( pMe->pFile, pMe->pData, pMe->dataLen ) != pMe->dataLen )
  {
    MSG_ERROR( "PutLinkedThumbnailResponse: failed to write to obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    status = BT_CS_PF_WRITE_ERROR;
  }
  else
  {
    if (pMe->bFinal == 1) 
    {
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }

  if (pIFileMgr) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( 
                         pMe->appId, 
                         pMe->serverConnID,
                         BT_EV_PF_BIP_SRV_PUT_LINKED_THUMBNAIL_IND, 
                         BT_CS_GN_SUCCESS );

  stat = bt_cmd_pf_bip_srv_put_linked_thumbnail_response(
                         pMe->appId, 
                         pMe->serverConnID,
                         status );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetImagePropertiesResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImagePropertiesResponse( 
  IBTExtBIP*  pParent,
  const char* pXMLdata )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;
  uint16             bytesRead = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return BT_CS_PF_INTERNAL_ERROR;
  }

  // Save pointer to user provided XML string locally
  pMe->pXMLdata   = (char*) pXMLdata;
  pMe->XMLdataLen = STRLEN( pXMLdata );

  pMe->bytesSent = 0;
  if ( pMe->bytesSent + BIP_DATA_BUFFER_SIZE < pMe->XMLdataLen ) 
  {
    bytesRead = BIP_DATA_BUFFER_SIZE;
  }
  else
  {
    bytesRead = pMe->XMLdataLen;

    // No more data to send
    final  = 1;
    status = BT_CS_GN_SUCCESS;
  }

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) &pMe->pXMLdata[ 0 ];

  pMe->bytesSent += bytesRead;

  // Special case.  We expect XML string to be non-empty
  if ( bytesRead == 0 ) 
  {
    (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                      pMe->serverConnID,
                                      BT_EV_PF_BIP_SRV_GET_IMAGE_PROPERTIES_IND, 
                                      BT_CS_PF_OBEX_ERROR );    

    return EFAILED;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_IMAGE_PROPERTIES_IND, 
                                    BT_CS_GN_SUCCESS ); 

  stat = bt_cmd_pf_bip_srv_get_image_properties_response(
                                       pMe->appId, 
                                       pMe->serverConnID,
                                       status,
                                       (bt_pf_byte_seq_type*) &pMe->data,
                                       final );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetImageListResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImageListResponse( 
  IBTExtBIP*  pParent,
  const char* pXMLdata,    // NULL acceptable
  const char* pXMLdata2,   // NULL acceptable
  uint16      numImageHandles )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;
  uint16             bytesRead = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;

  uint16             XMLdataLen2 = (NULL != pXMLdata2) ? STRLEN(pXMLdata2 ) : 0;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  // Save pointer to user provided XML string locally
  pMe->pXMLdata   = (char*) pXMLdata;
  pMe->XMLdataLen = (NULL != pXMLdata) ? STRLEN( pXMLdata ) : 0;

  // Since the descriptor bytes are packed in the response message first,
  // only the remaining buffer space is available for the image-listing
  // XML string
  if ( XMLdataLen2 + pMe->XMLdataLen > BIP_DATA_BUFFER_SIZE ) 
  {
    if ( XMLdataLen2 < BIP_DATA_BUFFER_SIZE ) 
    {
      bytesRead = BIP_DATA_BUFFER_SIZE - XMLdataLen2;
    }
    // else, remaining XML descriptor bytes
    // will not be sent in subsequent response messages
  }
  else
  {
    bytesRead = pMe->XMLdataLen;

    // No more data to send
    final  = 1;
    status = BT_CS_GN_SUCCESS;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_IMAGES_LIST_IND,
                                    BT_CS_GN_SUCCESS );

  pMe->bytesSent            = bytesRead;

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) pMe->pXMLdata;

  // Image descriptor filter applied by the Responder (the whole thing better
  // fit in the very first response message, i.e pXMLdataLen2 < BIP_DATA_BUFFER_SIZE !) 
  pMe->descData.AEEBTBIPdataLen = XMLdataLen2;
  pMe->descData.AEEBTBIPdata    = (byte*) pXMLdata2;

  stat = bt_cmd_pf_bip_srv_get_images_list_response(
                                       pMe->appId, 
                                       pMe->serverConnID,
                                       status,
                                       numImageHandles,
                                       (bt_pf_byte_seq_type*) &pMe->descData,
                                       (bt_pf_byte_seq_type*) &pMe->data,
                                       final );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetImageResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetImageResponse( 
  IBTExtBIP*  pParent,
  const char* pFileName )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;
  uint16             bytesRead = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*          pIFileMgr = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pFileName == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  if ( ( pMe->pFile = 
         IFILEMGR_OpenFile( pIFileMgr, pFileName, _OFM_READ ) ) != NULL )
  {
    // get filesize info
    IFILE_GetInfo( pMe->pFile, &pMe->fileInfo );

    bytesRead = BIP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    if ( bytesRead >= pMe->fileInfo.dwSize )
    {
      final  = 1;
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                      pMe->serverConnID,
                                      BT_EV_PF_BIP_SRV_GET_IMAGE_IND, 
                                      BT_CS_PF_OBEX_NOT_FOUND );

    return EFAILED;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_IMAGE_IND, 
                                    BT_CS_GN_SUCCESS );

  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;
  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->bytesSent            = bytesRead;

  IFILEMGR_Release( pIFileMgr );

  stat = bt_cmd_pf_bip_srv_get_image_response(
                                       pMe->appId, 
                                       pMe->serverConnID,
                                       status,
                                       pMe->fileInfo.dwSize,
                                       (bt_pf_byte_seq_type*) &pMe->data,
                                       final );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetMonitoringImageResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetMonitoringImageResponse( 
  IBTExtBIP*    pParent,
  const AECHAR* pImageHandle,
  const char*   pFileName )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;
  uint16             bytesRead = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*          pIFileMgr = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pFileName == NULL || pImageHandle == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  if ( ( pMe->pFile = 
         IFILEMGR_OpenFile( pIFileMgr, pFileName, _OFM_READ ) ) != NULL )
  {
    // get filesize info
    IFILE_GetInfo( pMe->pFile, &pMe->fileInfo );

    bytesRead = BIP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    if ( bytesRead >= pMe->fileInfo.dwSize )
    {
      final  = 1;  
      status = BT_CS_GN_SUCCESS;
    
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                      pMe->serverConnID,
                                      BT_EV_PF_BIP_SRV_GET_MONITORING_IMAGE_IND, 
                                      BT_CS_PF_OBEX_NOT_FOUND );

    return EFAILED;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_MONITORING_IMAGE_IND, 
                                    BT_CS_GN_SUCCESS );

  WSTRLCPY( pMe->wImageHandle, pImageHandle, ARR_SIZE( pMe->wImageHandle ) );

  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;
  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->bytesSent            = bytesRead;

  IFILEMGR_Release( pIFileMgr );

  stat = bt_cmd_pf_bip_srv_get_monitoring_image_response(
                               pMe->appId, 
                               pMe->serverConnID,
                               (bt_pf_bip_image_handle_type*) pImageHandle,
                               (bt_pf_byte_seq_type*) &pMe->data,
                               final,
                               status );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetPartialImageResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetPartialImageResponse( 
  IBTExtBIP*    pParent,
  const char*   pFileName,
  uint32        partialLength,
  uint32        partialOffset)
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;
  uint16             bytesRead = 0;
  boolean            endFlag   = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*          pIFileMgr = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pFileName == NULL ) 
  {
    return EBADPARM;
  }

  // Do some initialization
  pMe->partialLength = partialLength;
  pMe->partialOffset = partialOffset;

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  if ( ( pMe->pFile = 
         IFILEMGR_OpenFile( pIFileMgr, pFileName, _OFM_READ ) ) != NULL )
  {
    // get filesize info
    IFILE_GetInfo( pMe->pFile, &pMe->fileInfo );

    if ( partialLength < BIP_DATA_BUFFER_SIZE ) 
    {
      bytesRead = partialLength;
    }
    else
    {
      bytesRead = BIP_DATA_BUFFER_SIZE;
    }

    if ( partialOffset > 0 && partialOffset < pMe->fileInfo.dwSize ) 
    {
      IFILE_Seek( pMe->pFile, _SEEK_START, partialOffset );
    }

    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    
    if ( bytesRead >= pMe->fileInfo.dwSize || 
         bytesRead >= partialLength + partialOffset ) 
    {
      final  = 1;
      status = BT_CS_GN_SUCCESS;

      if ( bytesRead >= pMe->fileInfo.dwSize )
      {
        endFlag = 1;
      }

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                      pMe->serverConnID,
                                      BT_EV_PF_BIP_SRV_GET_PARTIAL_IMAGE_IND, 
                                      BT_CS_PF_OBEX_NOT_FOUND );

    return EFAILED;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_PARTIAL_IMAGE_IND, 
                                    BT_CS_GN_SUCCESS );

  // Update partialLength and partialOffset values
  pMe->partialLength -= bytesRead;
  pMe->partialOffset += bytesRead;

  pMe->data.AEEBTBIPdata    = (byte *)pMe->pDataBuffer;
  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->bytesSent            = bytesRead;

  IFILEMGR_Release( pIFileMgr );

  stat = bt_cmd_pf_bip_srv_get_partial_image_response(
                               pMe->appId, 
                               pMe->serverConnID,
                               pMe->fileInfo.dwSize,
                               endFlag,
                               (bt_pf_byte_seq_type*) &pMe->data,
                               final,
                               status );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetLinkedAttachmentResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetLinkedAttachmentResponse( 
  IBTExtBIP*    pParent,
  const char*   pFileName )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;
  uint16             bytesRead = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*          pIFileMgr = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pFileName == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  if ( ( pMe->pFile = 
         IFILEMGR_OpenFile( pIFileMgr, pFileName, _OFM_READ ) ) != NULL )
  {
    // get filesize info
    IFILE_GetInfo( pMe->pFile, &pMe->fileInfo );

    bytesRead = BIP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    if ( bytesRead >= pMe->fileInfo.dwSize )
    {
      final  = 1;  
      status = BT_CS_GN_SUCCESS;
    
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                      pMe->serverConnID,
                                      BT_EV_PF_BIP_SRV_GET_LINKED_ATTACHMENT_IND, 
                                      BT_CS_PF_OBEX_NOT_FOUND );

    return EFAILED;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_LINKED_ATTACHMENT_IND, 
                                    BT_CS_GN_SUCCESS );

  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;
  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->bytesSent            = bytesRead;

  IFILEMGR_Release( pIFileMgr );

  stat = bt_cmd_pf_bip_srv_get_linked_attachment_response(
                               pMe->appId, 
                               pMe->serverConnID,
                               status,
                               (bt_pf_byte_seq_type*) &pMe->data,
                               final );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_GetLinkedThumbnailResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_GetLinkedThumbnailResponse( 
  IBTExtBIP*    pParent,
  const char*   pFileName )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;
  uint16             bytesRead = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*          pIFileMgr = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pFileName == NULL ) 
  {
    return EBADPARM;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  if ( ( pMe->pFile = 
         IFILEMGR_OpenFile( pIFileMgr, pFileName, _OFM_READ ) ) != NULL )
  {
    // get filesize info
    IFILE_GetInfo( pMe->pFile, &pMe->fileInfo );

    bytesRead = BIP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    if ( bytesRead >= pMe->fileInfo.dwSize )
    {
      final  = 1;  
      status = BT_CS_GN_SUCCESS;
    
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                      pMe->serverConnID,
                                      BT_EV_PF_BIP_SRV_GET_LINKED_THUMBNAIL_IND, 
                                      BT_CS_PF_OBEX_NOT_FOUND );

    return EFAILED;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_LINKED_THUMBNAIL_IND, 
                                    BT_CS_GN_SUCCESS );

  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;
  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->bytesSent            = bytesRead;

  IFILEMGR_Release( pIFileMgr );

  stat = bt_cmd_pf_bip_srv_get_linked_thumbnail_response(
                               pMe->appId, 
                               pMe->serverConnID,
                               status,
                               (bt_pf_byte_seq_type*) &pMe->data,
                               final );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_DeleteImageResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_DeleteImageResponse( 
  IBTExtBIP*    pParent,
  boolean       bSuccess )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( bSuccess == FALSE )
  {
    (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                      pMe->serverConnID,
                                      BT_EV_PF_BIP_SRV_DELETE_IMAGE_IND, 
                                      BT_CS_PF_FAIL );
    return EFAILED;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_DELETE_IMAGE_IND, 
                                    BT_CS_GN_SUCCESS );

  stat = bt_cmd_pf_bip_srv_delete_image_response( pMe->appId, 
                                                  pMe->serverConnID,
                                                  BT_CS_GN_SUCCESS);

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_RemoteDisplayResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_RemoteDisplayResponse( 
  IBTExtBIP*    pParent,
  const AECHAR* pImageHandle,
  boolean       bSuccess )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;
  bt_cmd_status_type status    = BT_CS_GN_SUCCESS;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( bSuccess == FALSE )
  {
    (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                      pMe->serverConnID,
                                      BT_EV_PF_BIP_SRV_REMOTE_DISPLAY_IND, 
                                      BT_CS_PF_FAIL );
    return EFAILED;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_REMOTE_DISPLAY_IND, 
                                    BT_CS_GN_SUCCESS );

  stat = bt_cmd_pf_bip_srv_remote_display_response( 
                             pMe->appId, 
                             pMe->serverConnID,
                             (bt_pf_bip_image_handle_type*) pImageHandle,
                             status);

  return OEMBTExtBIP_CheckCmdStatus( stat );
}

// IBTEXTBIP_StartArchiveResponse (see AEEBTExtBIP.h)
int OEMBTExtBIP_StartArchiveResponse( 
  IBTExtBIP*    pParent,
  boolean       bSuccess )
{
  bt_cmd_status_type stat;
  OEMBTExtBIPobj_t*  pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( bSuccess == FALSE )
  {
    (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                      pMe->serverConnID,
                                      BT_EV_PF_BIP_SRV_START_ARCHIVE_IND, 
                                      BT_CS_PF_FAIL );
    return EFAILED;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_START_ARCHIVE_IND, 
                                    BT_CS_GN_SUCCESS );

  stat = bt_cmd_pf_bip_srv_start_archive_response( pMe->appId, 
                                                   pMe->serverConnID,
                                                   BT_CS_GN_SUCCESS );

  return OEMBTExtBIP_CheckCmdStatus( stat );
}


//==========================================================================
//   static helper functions
//==========================================================================
static int OEMBTExtBIP_CheckCmdStatus( bt_cmd_status_type stat )
{
  switch (stat)
  {
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return SUCCESS;
    case BT_CS_GN_CMD_Q_FULL:
      return ENOMEMORY;
    case OI_STATUS_INVALID_PARAMETERS:
      return EBADPARM;
    default:
      return EFAILED;
  }
}

static OEMBTExtBIPobj_t* OEMBTExtBIP_FindMe(bt_app_id_type appId)
{
  OEMBTExtBIPobj_t* pMe;

  for (pMe = gMgr.pNextBIP; pMe != NULL; pMe = pMe->pNextBIP)
  {
    if (pMe->appId == appId)
    {
      return pMe;
    }
  }
  return NULL;
}

static boolean OEMBTExtBIP_HandleCmdDone( 
  bt_ev_gn_cmd_done_type* pCmdDn, 
  AEEBTNotification*      pN,
  OEMBTExtBIPobj_t*       pMe
)
{
  boolean doSendNotif = TRUE;

  MSG_LOW( "BIP_HandleCmdDone - stat=%x cmd=%x", pCmdDn->cmd_status, pCmdDn->cmd_type, 0 );

  switch ( pCmdDn->cmd_type )
  {
    case BT_PF_CMD_BIP_SRV_REG_IMAGING_RERSPONDER:
    {
      pMe->serverConnID = BT_PF_BIP_NO_CONN_ID;
      pMe->bIsServer = TRUE;
      pMe->bIsSecServer = FALSE;
      pN->uID = AEEBT_BIP_EVT_REG_DONE;
      pN->data.BipObject.bPriServer = TRUE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_BIP_SRV_REG_ARCHIVED_OBJECTS:
    {
      pMe->serverConnID = BT_PF_BIP_NO_CONN_ID;
      pMe->bIsSecServer = TRUE;
      pMe->bIsServer = FALSE;
      pN->uID = AEEBT_BIP_EVT_REG_DONE;
      pN->data.BipObject.bPriServer = FALSE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_BIP_SRV_REG_REFERENCED_OBJECTS:
    {
      pMe->serverConnID = BT_PF_BIP_NO_CONN_ID;
      pMe->bIsSecServer = TRUE;
      pMe->bIsServer = FALSE;
      pN->uID = AEEBT_BIP_EVT_REG_DONE;
      pN->data.BipObject.bPriServer = FALSE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_BIP_SRV_DEREG_IMAGING_RERSPONDER:
    {
      pMe->bIsServer = FALSE;
      pN->uID = AEEBT_BIP_EVT_DEREG_DONE;
      pN->data.BipObject.bPriServer = TRUE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_BIP_SRV_DEREG_ARCHIVED_OBJECTS:
    {
      pMe->bIsSecServer = FALSE;
      pN->uID = AEEBT_BIP_EVT_DEREG_DONE;
      pN->data.BipObject.bPriServer = FALSE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_BIP_SRV_DEREG_REFERENCED_OBJECTS:
    {
      pMe->bIsSecServer = FALSE;
      pN->uID = AEEBT_BIP_EVT_DEREG_DONE;
      pN->data.BipObject.bPriServer = FALSE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_BIP_CLI_CONNECT:
    {
      pN->uID = AEEBT_BIP_EVT_CONN_FAILED;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_DISCONNECT:
    {
      pN->uID = AEEBT_BIP_EVT_DISCONNECTED;
      pN->data.BipObject.bServerEvt = FALSE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_GET_CAPABILITIES:
    {
      pN->uID = AEEBT_BIP_EVT_GET_CAPABILITIES_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_PUT_IMAGE:
    {
      pN->uID = AEEBT_BIP_EVT_PUT_IMAGE_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_PUT_LINKED_THUMBNAIL:
    {
      pN->uID = AEEBT_BIP_EVT_PUT_THUMBNAIL_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_PUT_LINKED_ATTACHMENT:
    {
      pN->uID = AEEBT_BIP_EVT_PUT_ATTACHMENT_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_GET_IMAGES_LIST:
    {
      pN->uID = AEEBT_BIP_EVT_GET_IMAGE_LIST_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_GET_IMAGE_PROPERTIES:
    {
      pN->uID = AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_GET_IMAGE:
    {
      pN->uID = AEEBT_BIP_EVT_GET_IMAGE_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_GET_LINKED_THUMBNAIL:
    {
      pN->uID = AEEBT_BIP_EVT_GET_THUMBNAIL_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_GET_LINKED_ATTACHMENT:
    {
      pN->uID = AEEBT_BIP_EVT_GET_ATTACHMENT_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_DELETE_IMAGE:
    {
      pN->uID = AEEBT_BIP_EVT_DELETE_IMAGE_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_GET_STATUS:
    {
      pN->uID = AEEBT_BIP_EVT_GET_STATUS_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_GET_PARTIAL_IMAGE:
    {
      pN->uID = AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_START_PRINT:
    {
      pN->uID = AEEBT_BIP_EVT_START_PRINT_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_START_ARCHIVE:
    {
      pN->uID = AEEBT_BIP_EVT_START_ARCHIVE_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_REMOTE_DISPLAY:
    {
      pN->uID = AEEBT_BIP_EVT_REMOTE_DISPLAY_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BIP_CLI_GET_MONITORING_IMAGE:
    {
      pN->uID = AEEBT_BIP_EVT_GET_MONITORING_IMAGE_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    default:
    {
      doSendNotif = FALSE;
    }
  }

  return doSendNotif;
}

static OEMBTExtBIP_EvCb* OEMBTExtBIP_GetFreeEvCb( void )
{
  uint8 i;
  OEMBTExtBIP_EvCb* pEvCb = NULL;

  for ( i=0; i<(sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++)
  {
    if ( gMgr.FreeEvCb[ i ].bInUse == FALSE )
    {
      gMgr.FreeEvCb[ i ].bInUse = TRUE;
      pEvCb = &gMgr.FreeEvCb[ i ];
      break;
    }
  }
  return pEvCb;
}

static void OEMBTExtBIP_GetCapabilitiesServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint16 bytesRead          = 0;
  boolean final             = 0;
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->serverConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetCapabilitiesServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }

  if ( pMe->bytesSent + BIP_DATA_BUFFER_SIZE < pMe->XMLdataLen ) 
  {
    bytesRead = BIP_DATA_BUFFER_SIZE;
  }
  else
  {
    bytesRead = pMe->XMLdataLen - pMe->bytesSent;

    // No more data to send
    final  = 1;
    status = BT_CS_GN_SUCCESS;
  }

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) &pMe->pXMLdata[pMe->bytesSent];

  pMe->bytesSent += bytesRead;

  pEvCb->bInUse = FALSE;

  bt_cmd_pf_bip_srv_get_capabilities_response( 
                                       pMe->appId, 
                                       pMe->serverConnID,
                                       (bt_pf_byte_seq_type*) &pMe->data,
                                       final,
                                       status );
}

static void OEMBTExtBIP_GetCapabilitiesClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t*   pMe = OEMBTExtBIP_FindMe( pEvCb->appId );
  AEEBTNotification*  pN;

  // Loop counter for copying data to user supplied buffer
  int i = 0;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetCapabilitiesClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }

  // Copy data to user buffer
  for ( i=0; i<pEvCb->dataLen; i++ ) 
  {
    // Check for overflow condition
    if ( pMe->bytesRcvd < pMe->userBufferMaxSize - 1 )
    {
      pMe->pUserBuffer[ pMe->bytesRcvd++ ] = pEvCb->pData[ i ];
    }
    else
    {
      // Ran out of buffer space, don't copy anymore
    }
  }

  (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                        pMe->clientConnID,
                                        BT_EV_PF_BIP_CLI_GET_CAPABILITIES_CFM);

  // If this is the last set of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    // Null terminate the string
    pMe->pUserBuffer[ pMe->bytesRcvd ] = '\0';
    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID              = AEEBT_BIP_EVT_GET_CAPABILITIES_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );
      
      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_cli_get_capabilities( pMe->appId, pMe->clientConnID );
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtBIP_GetImagePropertiesServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint16 bytesRead          = 0;
  boolean final             = 0;
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->serverConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetImagePropertiesServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  
  if ( pMe->bytesSent + BIP_DATA_BUFFER_SIZE < pMe->XMLdataLen ) 
  {
    bytesRead = BIP_DATA_BUFFER_SIZE;
  }
  else
  {
    bytesRead = pMe->XMLdataLen - pMe->bytesSent;

    // No more data to send
    final  = 1;
    status = BT_CS_GN_SUCCESS;
  }

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) &pMe->pXMLdata[pMe->bytesSent];

  pMe->bytesSent += bytesRead;

  pEvCb->bInUse = FALSE;

  bt_cmd_pf_bip_srv_get_image_properties_response( 
                                       pMe->appId, 
                                       pMe->serverConnID,
                                       status,
                                       (bt_pf_byte_seq_type*) &pMe->data,
                                       final );
}

static void OEMBTExtBIP_GetImagePropertiesClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe  = OEMBTExtBIP_FindMe( pEvCb->appId );
  AEEBTNotification*  pN = NULL;

  // Loop counter for copying data to user supplied buffer
  int i = 0;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetImagePropertiesClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }

  // Copy data to user buffer
  for ( i=0; i<pEvCb->dataLen; i++ ) 
  {
    // Check for overflow condition
    if ( pMe->bytesRcvd < pMe->userBufferMaxSize - 1 )
    {
      pMe->pUserBuffer[ pMe->bytesRcvd++ ] = pEvCb->pData[ i ];
    }
    else
    {
      // Ran out of buffer space, don't copy anymore
    }
  }

  (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                        pMe->clientConnID,
                                        BT_EV_PF_BIP_CLI_GET_IMAGE_PROPERTIES_CFM);

  // If this is the last set of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    // Null terminate the string
    pMe->pUserBuffer[ pMe->bytesRcvd ] = '\0';

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID              = AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );

      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_cli_get_image_properties( 
                         pMe->appId, 
                         pMe->clientConnID,
                         (bt_pf_bip_image_handle_type*) pMe->wImageHandle );
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtBIP_GetImageListServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint16 bytesRead          = 0;
  boolean final             = 0;
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->serverConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetImageListServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  
  if ( pMe->bytesSent + BIP_DATA_BUFFER_SIZE < pMe->XMLdataLen ) 
  {
    bytesRead = BIP_DATA_BUFFER_SIZE;
  }
  else
  {
    bytesRead = pMe->XMLdataLen - pMe->bytesSent;

    // No more data to send
    final  = 1;
    status = BT_CS_GN_SUCCESS;
  }

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_IMAGES_LIST_IND,
                                    BT_CS_GN_SUCCESS );

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) &pMe->pXMLdata[pMe->bytesSent];

  pMe->bytesSent += bytesRead;

  // Descriptor XML string was sent in the very first response message
  pMe->descData.AEEBTBIPdataLen = 0;
  pMe->descData.AEEBTBIPdata    = NULL;

  pEvCb->bInUse = FALSE;

  bt_cmd_pf_bip_srv_get_images_list_response( 
                                     pMe->appId, 
                                     pMe->serverConnID,
                                     status,
                                     0,      // numImageHandles
                                     (bt_pf_byte_seq_type*) &pMe->descData,
                                     (bt_pf_byte_seq_type*) &pMe->data,
                                     final);
}

static void OEMBTExtBIP_GetImageListClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe  = OEMBTExtBIP_FindMe( pEvCb->appId );
  AEEBTNotification*  pN = NULL;

  // Loop counter for copying data to user supplied buffer
  int i = 0;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetImageListClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }

  // Copy data to user buffer
  for ( i=0; i<pEvCb->dataLen; i++ ) 
  {
    // Check for overflow condition
    if ( pMe->bytesRcvd < pMe->userBufferMaxSize - 1 )
    {
      pMe->pUserBuffer[ pMe->bytesRcvd++ ] = pEvCb->pData[ i ];
    }
    else
    {
      // Ran out of buffer space, don't copy anymore
    }
  }

  (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                        pMe->clientConnID,
                                        BT_EV_PF_BIP_CLI_GET_IMAGES_LIST_CFM);

  // If this is the last set of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    // Null terminate the string
    pMe->pUserBuffer[ pMe->bytesRcvd ] = '\0';

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      // Let app know about the number of image-handles
      pN->data.BipObject.handleCount = pEvCb->numHandles;

      pN->uID              = AEEBT_BIP_EVT_GET_IMAGE_LIST_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );
      
      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_cli_get_images_list( 
                         pMe->appId, 
                         pMe->clientConnID,
                         pMe->numReturnedHandles,
                         pMe->startOffset,
                         pMe->latest,
                         (bt_pf_byte_seq_type*) &pMe->descData );
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtBIP_GetImageServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesRead          = 0;
  boolean final             = 0;
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->serverConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetImageServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else 
  {
    bytesRead = BIP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    pMe->bytesSent += bytesRead;
    if ( bytesRead == 0 )
    {
      MSG_ERROR( "GetImageServerCb: failed to read from obj", 0, 0, 0 );
      status = BT_CS_PF_READ_ERROR;
    }
    else if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
    {
      final  = 1;
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }

  pEvCb->bInUse = FALSE;

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_IMAGE_IND, 
                                    BT_CS_GN_SUCCESS );

  bt_cmd_pf_bip_srv_get_image_response( 
                                pMe->appId, 
                                pMe->serverConnID,
                                status,
                                pMe->fileInfo.dwSize,
                                (bt_pf_byte_seq_type*) &pMe->data,
                                final );
}

static void OEMBTExtBIP_GetImageClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe  = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesWritten    = 0;
  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetImageClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }

  if ( (bytesWritten = IFILE_Write( pMe->pFile, 
                                    pEvCb->pData, 
                                    pEvCb->dataLen )) != pEvCb->dataLen )
  {
    MSG_ERROR( "GetImageClientCb: failed to write to obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    return;
  }
 
  pMe->bytesRcvd += bytesWritten;

  (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                        pMe->clientConnID,
                                        BT_EV_PF_BIP_CLI_GET_IMAGE_CFM);
  
  // If this is the last set of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID              = AEEBT_BIP_EVT_GET_IMAGE_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );
      
      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_cli_get_image( 
                         pMe->appId, 
                         pMe->clientConnID,
                         (bt_pf_bip_image_handle_type*) pMe->wImageHandle,
                         NULL );
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtBIP_GetPartialImageServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesRead          = 0;
  boolean final             = 0;
  boolean endFlag           = 0;
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->serverConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetPartialImageServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else 
  {
    if ( pMe->partialLength < BIP_DATA_BUFFER_SIZE ) 
    {
      bytesRead = pMe->partialLength;
    }
    else
    {
      bytesRead = BIP_DATA_BUFFER_SIZE;
    }

    if ( pMe->partialOffset > 0 && pMe->partialOffset < pMe->fileInfo.dwSize ) 
    {
      IFILE_Seek( pMe->pFile, _SEEK_START, pMe->partialOffset );
    }

    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    pMe->bytesSent += bytesRead;
    if ( bytesRead == 0 )
    {
      MSG_ERROR( "GetPartialImageServerCb: failed to read from obj", 0, 0, 0 );
      status = BT_CS_PF_READ_ERROR;
    }
    
    if ( pMe->bytesSent >= pMe->fileInfo.dwSize ||
         pMe->bytesSent >= pMe->partialLength + pMe->partialOffset )
    {
      final  = 1;
      status = BT_CS_GN_SUCCESS;

      if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
      {
        endFlag = 1;
      }

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }

  // Update partialLength and partialOffset values
  pMe->partialLength -= bytesRead;
  pMe->partialOffset += bytesRead;

  pEvCb->bInUse = FALSE;

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_PARTIAL_IMAGE_IND, 
                                    BT_CS_GN_SUCCESS );

  bt_cmd_pf_bip_srv_get_partial_image_response( 
                                     pMe->appId, 
                                     pMe->serverConnID,
                                     pMe->fileInfo.dwSize,
                                     endFlag,
                                     (bt_pf_byte_seq_type*) &pMe->data,
                                     final,
                                     status );
}

static void OEMBTExtBIP_GetPartialImageClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe  = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesWritten    = 0;
  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetPartialImageClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }
  else if ( (bytesWritten = IFILE_Write( pMe->pFile, 
                                         pEvCb->pData, 
                                         pEvCb->dataLen )) != pEvCb->dataLen )
  {
    MSG_ERROR( "GetPartialImageClientCb: failed to write to obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    return;
  }

  pMe->bytesRcvd += bytesWritten;

  (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                        pMe->clientConnID,
                                        BT_EV_PF_BIP_CLI_GET_PARTIAL_IMAGE_CFM);
  
  // If this is the last set of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID              = AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );

      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_cli_get_partial_image( 
                                         pMe->appId, 
                                         pMe->clientConnID,
                                         (uint16*) pMe->wImageName,
                                         pMe->partialLength,
                                         pMe->partialOffset );
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtBIP_GetLinkedAttachmentServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesRead          = 0;
  boolean final             = 0;
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->serverConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetLinkedAttachmentServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else 
  {
    bytesRead = BIP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    pMe->bytesSent += bytesRead;
    if ( bytesRead == 0 )
    {
      MSG_ERROR( "GetLinkedAttachmentServerCb: failed to read from obj", 0, 0, 0 );
      status = BT_CS_PF_READ_ERROR;
    }
    else if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
    {
      final  = 1;
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }

  pEvCb->bInUse = FALSE;

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_LINKED_ATTACHMENT_IND, 
                                    BT_CS_GN_SUCCESS );

  bt_cmd_pf_bip_srv_get_linked_attachment_response( 
                                     pMe->appId, 
                                     pMe->serverConnID,
                                     status,
                                     (bt_pf_byte_seq_type*) &pMe->data,
                                     final);
}

static void OEMBTExtBIP_GetLinkedAttachmentClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe  = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesWritten    = 0;
  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetLinkedAttachmentClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }
  else if ( (bytesWritten = IFILE_Write( pMe->pFile, 
                                         pEvCb->pData, 
                                         pEvCb->dataLen )) != pEvCb->dataLen )
  {
    MSG_ERROR( "GetLinkedAttachmentClientCb: failed to write to obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    return;
  }

  pMe->bytesRcvd += bytesWritten;

  (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                        pMe->clientConnID,
                                        BT_EV_PF_BIP_CLI_GET_LINKED_ATTACHMENT_CFM);

  // If this is the last set of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID              = AEEBT_BIP_EVT_GET_ATTACHMENT_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );

      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_cli_get_linked_attachment( 
                         pMe->appId, 
                         pMe->clientConnID,
                         (bt_pf_bip_image_handle_type*) pMe->wImageHandle,
                         (uint16*) pMe->wAttachmentName );
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtBIP_GetLinkedThumbnailServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesRead          = 0;
  boolean final             = 0;
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->serverConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetLinkedThumbnailServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else 
  {
    bytesRead = BIP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    pMe->bytesSent += bytesRead;
    if ( bytesRead == 0 )
    {
      MSG_ERROR( "GetLinkedThumbnailServerCb: failed to read from obj", 0, 0, 0 );
      status = BT_CS_PF_READ_ERROR;
    }
    else if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
    {
      final  = 1;
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }

  pEvCb->bInUse = FALSE;

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_LINKED_THUMBNAIL_IND, 
                                    BT_CS_GN_SUCCESS );

  bt_cmd_pf_bip_srv_get_linked_thumbnail_response( 
                                     pMe->appId, 
                                     pMe->serverConnID,
                                     status,
                                     (bt_pf_byte_seq_type*) &pMe->data,
                                     final);
}

static void OEMBTExtBIP_GetLinkedThumbnailClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe  = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesWritten    = 0;
  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetLinkedThumbnailClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }
  else if ( (bytesWritten = IFILE_Write( pMe->pFile, 
                                         pEvCb->pData, 
                                         pEvCb->dataLen )) != pEvCb->dataLen )
  {
    MSG_ERROR( "GetLinkedThumbnailClientCb: failed to write to obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    return;
  }

  pMe->bytesRcvd += bytesWritten;

  (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                        pMe->clientConnID,
                                        BT_EV_PF_BIP_CLI_GET_LINKED_THUMBNAIL_CFM);

  // If this is the last set of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID              = AEEBT_BIP_EVT_GET_THUMBNAIL_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );

      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_cli_get_linked_thumbnail( 
                         pMe->appId, 
                         pMe->clientConnID,
                         (bt_pf_bip_image_handle_type*) pMe->wImageHandle);
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtBIP_GetMonitoringImageServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesRead          = 0;
  boolean final             = 0;
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->serverConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetMonitoringImageServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else 
  {
    bytesRead = BIP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    pMe->bytesSent += bytesRead;
    if ( bytesRead == 0 )
    {
      MSG_ERROR( "GetMonitoringImageServerCb: failed to read from obj", 0, 0, 0 );
      status = BT_CS_PF_READ_ERROR;
    }
    else if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
    {
      final  = 1;
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }

  pEvCb->bInUse = FALSE;

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_GET_MONITORING_IMAGE_IND, 
                                    BT_CS_GN_SUCCESS );

  bt_cmd_pf_bip_srv_get_monitoring_image_response( 
                          pMe->appId, 
                          pMe->serverConnID,
                          (bt_pf_bip_image_handle_type*) pMe->wImageHandle,
                          (bt_pf_byte_seq_type*) &pMe->data,
                          final,
                          status);
}

static void OEMBTExtBIP_GetMonitoringImageClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe  = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesWritten    = 0;
  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetMonitoringImageClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }
  else if ( (bytesWritten = IFILE_Write( pMe->pFile, 
                                         pEvCb->pData, 
                                         pEvCb->dataLen )) != pEvCb->dataLen )
  {
    MSG_ERROR( "GetMonitoringImageClientCb: failed to write to obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    return;
  }

  pMe->bytesRcvd += bytesWritten;

  (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                        pMe->clientConnID,
                                        BT_EV_PF_BIP_CLI_GET_MONITORING_IMAGE_CFM);
  
  // If this is the last set of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID              = AEEBT_BIP_EVT_GET_MONITORING_IMAGE_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );

      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_bip_cli_get_monitoring_image( pMe->appId, 
                                                   pMe->clientConnID,
                                                   pMe->storeFlag );
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtBIP_DeleteImageClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe  = OEMBTExtBIP_FindMe( pEvCb->appId );
  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "DeleteImageClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }

  (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                        pMe->clientConnID,
                                        BT_EV_PF_BIP_CLI_DELETE_IMAGE_CFM);

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "BIP - Can't get free not.",
               0, 0, 0 );
  }
  else
  {
    pN->uID              = AEEBT_BIP_EVT_DELETE_IMAGE_DONE;
    pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );
    
    pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
    IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
  }
  pEvCb->bInUse = FALSE;
}

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

static void OEMBTExtBIP_StartPrintClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesRead          = 0;
  boolean final             = 0;

  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pEvCb->status != BT_CS_PF_OBEX_CONTINUE )
  {
    MSG_LOW( "StartPrintClientCb: done with status - %x",
             pEvCb->status, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pEvCb->bInUse = FALSE;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID         = AEEBT_BIP_EVT_START_PRINT_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pEvCb->status );

      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
    return;
  }
  
  bytesRead = BIP_DATA_BUFFER_SIZE;
  bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
  pMe->bytesSent += bytesRead;
  if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
  {
    final  = 1;
  }

  pEvCb->bInUse = FALSE;

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;

  bt_cmd_pf_bip_cli_start_print( pMe->appId, 
                                 pMe->clientConnID,
                                 (bt_sd_uuid128_type*) &pMe->serviceID,
                                 (bt_pf_byte_seq_type*) &pMe->data,
                                  final );
}

static void OEMBTExtBIP_PutImageServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if(pMe->pFile != NULL)
  {
    if ( IFILE_Write( pMe->pFile, pEvCb->pData, pEvCb->dataLen ) != pEvCb->dataLen )
    {
      MSG_ERROR( "PutImageServerCb: failed to write to obj", 0, 0, 0 );

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;

      status = BT_CS_PF_WRITE_ERROR;
    }
    else
    {
      if (pEvCb->final == 1) 
      {
        if ( pMe->bGetThumbnail == TRUE ) 
        {
          status = BT_CS_PF_OBEX_PARTIAL_CONTENT;
        }
        else
        {
          status = BT_CS_GN_SUCCESS;
        }

        IFILE_Release( pMe->pFile );
        pMe->pFile = NULL;
      }
    }
  }
  else
  {
    status = BT_CS_PF_INTERNAL_ERROR;
  }

  pEvCb->bInUse = FALSE;

  (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                    pMe->serverConnID,
                                    BT_EV_PF_BIP_SRV_PUT_IMAGE_IND, 
                                    BT_CS_GN_SUCCESS );

  bt_cmd_pf_bip_srv_put_image_response( 
    pMe->appId, 
    pMe->serverConnID,
    (bt_pf_bip_image_handle_type*) pMe->wImageHandle,
    status );
}

static void OEMBTExtBIP_PutImageClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesRead          = 0;
  boolean final             = 0;

  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pEvCb->status != BT_CS_PF_OBEX_CONTINUE )
  {
    MSG_LOW( "PutImageClientCb: done with status - %x", 
             pEvCb->status, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pEvCb->bInUse = FALSE;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      // End of image transfer
      pN->uID         = AEEBT_BIP_EVT_PUT_IMAGE_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pEvCb->status );
      
      if ( pN->data.uError == AEEBT_BIP_ERR_NONE ) 
      {
        // Let the app know the image handle that the server returned
        WSTRLCPY( pN->data.BipObject.wImageHandle, pMe->wImageHandle, 
                  ARR_SIZE( pN->data.BipObject.wImageHandle ) );
      }
      
      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
    return;
  }
  
  bytesRead = BIP_DATA_BUFFER_SIZE;
  bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
  pMe->bytesSent += bytesRead;
  if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
  {
    final  = 1;
  }

  pEvCb->bInUse = FALSE;

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;

  bt_cmd_pf_bip_cli_put_image( pMe->appId, 
                               pMe->clientConnID,
                               NULL,
                               NULL,
                               (bt_pf_byte_seq_type*) &pMe->data,
                               final );
}

static void OEMBTExtBIP_PutLinkedAttachmentServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( IFILE_Write( pMe->pFile, pEvCb->pData, pEvCb->dataLen ) != pEvCb->dataLen )
  {
    MSG_ERROR( "PutLinkedAttachmentServerCb: failed to write to obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    status = BT_CS_PF_WRITE_ERROR;
  }
  else
  {
    if (pEvCb->final == 1) 
    {
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }

  pEvCb->bInUse = FALSE;

  (void) bt_cmd_pf_bip_srv_req_cfm( 
                         pMe->appId, 
                         pMe->serverConnID,
                         BT_EV_PF_BIP_SRV_PUT_LINKED_ATTACHMENT_IND, 
                         BT_CS_GN_SUCCESS );

  bt_cmd_pf_bip_srv_put_linked_attachment_response( pMe->appId, 
                                                    pMe->serverConnID,
                                                    status );
}

static void OEMBTExtBIP_PutLinkedAttachmentClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesRead          = 0;
  boolean final             = 0;

  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }
 
  if ( pEvCb->status != BT_CS_PF_OBEX_CONTINUE )
  {
    MSG_LOW( "PutLinkedAttachmentClientCb: done with status - %x", 
             pEvCb->status, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pEvCb->bInUse = FALSE;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID         = AEEBT_BIP_EVT_PUT_ATTACHMENT_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pEvCb->status );

      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
    return;
  }

  bytesRead = BIP_DATA_BUFFER_SIZE;
  bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
  pMe->bytesSent += bytesRead;
  if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
  {
    final  = 1;
  }

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;

  bt_cmd_pf_bip_cli_put_linked_attachment( 
                                         pMe->appId, 
                                         pMe->clientConnID,
                                         NULL,
                                         NULL,
                                         (bt_pf_byte_seq_type*) &pMe->data,
                                         final );

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtBIP_PutLinkedThumbnailServerCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  bt_cmd_status_type status = BT_CS_PF_OBEX_CONTINUE;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( IFILE_Write( pMe->pFile, pEvCb->pData, pEvCb->dataLen ) != pEvCb->dataLen )
  {
    MSG_ERROR( "PutLinkedThumbnailServerCb: failed to write to obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    status = BT_CS_PF_WRITE_ERROR;
  }
  else
  {
    if (pEvCb->final == 1) 
    {
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }
  }

  pEvCb->bInUse = FALSE;

  (void) bt_cmd_pf_bip_srv_req_cfm( 
                         pMe->appId, 
                         pMe->serverConnID,
                         BT_EV_PF_BIP_SRV_PUT_LINKED_THUMBNAIL_IND, 
                         BT_CS_GN_SUCCESS );

  bt_cmd_pf_bip_srv_put_linked_thumbnail_response( pMe->appId, 
                                                   pMe->serverConnID,
                                                   status );
}

static void OEMBTExtBIP_PutLinkedThumbnailClientCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe     = OEMBTExtBIP_FindMe( pEvCb->appId );
  uint32 bytesRead          = 0;
  boolean final             = 0;

  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pEvCb->status != BT_CS_PF_OBEX_CONTINUE )
  {
    MSG_LOW( "PutLinkedThumbnailClientCb: done with status - %x", 
             pEvCb->status, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pEvCb->bInUse = FALSE;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BIP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID         = AEEBT_BIP_EVT_PUT_THUMBNAIL_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pEvCb->status );

      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
    return;
  }

  bytesRead = BIP_DATA_BUFFER_SIZE;
  bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
  pMe->bytesSent += bytesRead;
  if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
  {
    final  = 1;
  }

  pEvCb->bInUse = FALSE;

  pMe->data.AEEBTBIPdataLen = bytesRead;
  pMe->data.AEEBTBIPdata    = (byte*) pMe->pDataBuffer;

  bt_cmd_pf_bip_cli_put_linked_thumbnail( 
           pMe->appId, 
           pMe->clientConnID,
           (bt_pf_bip_image_handle_type*) pMe->wImageHandle,
           (bt_pf_byte_seq_type*) &pMe->data,
           final );
}

static void OEMBTExtBIP_DisconnectCb( OEMBTExtBIP_EvCb* pEvCb )
{
  OEMBTExtBIPobj_t* pMe = OEMBTExtBIP_FindMe( pEvCb->appId );
  AEEBTNotification* notif;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( (pEvCb->bServerEvt == TRUE  && pMe->serverConnID != pEvCb->connId) ||
       (pEvCb->bServerEvt != TRUE  && pMe->clientConnID != pEvCb->connId) )
  {
    MSG_ERROR( "DisconnectCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
  }
  else
  {
    if ( pMe->pFile ) 
    {
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }

    notif = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if ( notif != NULL )
    {
      if ( pEvCb->bServerEvt == TRUE ) 
      {
        pMe->serverConnID = BT_PF_BIP_NO_CONN_ID;
        notif->data.BipObject.bServerEvt = TRUE;
      }
      else
      {
        pMe->clientConnID = BT_PF_BIP_NO_CONN_ID;
        notif->data.BipObject.bServerEvt = FALSE;
      }

      notif->uID = AEEBT_BIP_EVT_DISCONNECTED;
      notif->data.uError = AEEBT_BIP_ERR_NONE;
      notif->data.bdAddr = pMe->BDAddr;
      notif->uID = (notif->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, notif, pMe->pac );
    }
    else
    {
      MSG_ERROR( "DisconnectCb: no free notifications",0,0,0);
    }
  }
  TASKLOCK();
  pEvCb->bInUse = FALSE;
  TASKFREE();

  // Do server dereg if it is pending (after force disconnect)
  if ( pMe->bDoDeregister )
  {
    pMe->bDoDeregister = FALSE;
    if ( OEMBTExtBIP_DeregisterServer( pMe ) != SUCCESS )
    {
      MSG_ERROR( "DisconnectCb: S de-reg failed!", 0, 0, 0 );
    }
  }
}

// called from BT task context; so just queue the event
static void OEMBTExtBIP_EventCallback( bt_ev_msg_type* ev_msg_ptr )
{
  OEMBTExtBIPobj_t*   pMe    = OEMBTExtBIP_FindMe( ev_msg_ptr->ev_hdr.bt_app_id );
  OEMBTExtBIP_EvCb*   pEvCb  = NULL;
  AEEBTNotification*  pN     = NULL;

  uint16       descriptorLen = 0;

  if ( pMe == NULL )
  {
    return; // probably for an app that no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "BIPEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }

  pN->data.uError  = AEEBT_BIP_ERR_NONE;

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
    {
      if ( ! OEMBTExtBIP_HandleCmdDone( &ev_msg_ptr->ev_msg.ev_gn_cmd_done,
                                        pN, pMe ) )
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return; // no need to send notification to app
      }
      break;
    } // end of case BT_EV_GN_CMD_DONE
    case BT_EV_PF_BIP_CLI_AUTH_REQ:
    {
      MSG_LOW( "BIPEvCb: C auth req, id=%x st=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_auth_req.conn_id, pMe->state, 0 );

      pMe->clientConnID = ev_msg_ptr->ev_msg.ev_bip_cli_auth_req.conn_id;
      pN->uID = AEEBT_BIP_EVT_AUTH_REQ;
      break;
    }
    case BT_EV_PF_BIP_CLI_CON_CFM:
    {
      MSG_LOW( "BIPEvCb: C con cfm, id=%x stat=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_con_cfm.conn_id,
               ev_msg_ptr->ev_msg.ev_bip_cli_con_cfm.status, 0 );
      if (ev_msg_ptr->ev_msg.ev_bip_cli_con_cfm.status == BT_CS_GN_SUCCESS )
      {
        pMe->clientConnID = ev_msg_ptr->ev_msg.ev_bip_cli_con_cfm.conn_id;
        pN->uID = AEEBT_BIP_EVT_CONNECTED;
      }
      else
      {
        pN->uID = AEEBT_BIP_EVT_CONN_FAILED;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_DCN_CFM:
    {
      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_dcn_cfm.conn_id;
        pEvCb->bServerEvt = FALSE;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtBIP_DisconnectCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_CLI_GET_CAPABILITIES_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_get_capabilities_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C get cap cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_get_capabilities_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C get cap cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_capabilities_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_capabilities_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_get_capabilities_cfm.conn_id;
        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_cli_get_capabilities_cfm.data_ptr;
        pEvCb->dataLen = ev_msg_ptr->ev_msg.ev_bip_cli_get_capabilities_cfm.data_len;

        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_cli_get_capabilities_cfm.final;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_get_capabilities_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetCapabilitiesClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_GET_IMAGE_PROPERTIES_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_get_properties_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C get img prop cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_get_properties_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C get img prop cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_properties_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_properties_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_get_properties_cfm.conn_id;
        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_cli_get_properties_cfm.data_ptr;
        pEvCb->dataLen = ev_msg_ptr->ev_msg.ev_bip_cli_get_properties_cfm.data_len;

        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_cli_get_properties_cfm.final;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_get_properties_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetImagePropertiesClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_GET_IMAGES_LIST_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C get img list cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C get img list cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.conn_id;
        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.data_ptr;
        pEvCb->dataLen  = ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.data_len;

        pEvCb->numHandles = 
          ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.num_returned_handles;
        pEvCb->pImageDesc = 
          ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.image_handles_descriptor_ptr;
        pEvCb->descLen = 
          ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.image_handles_descriptor_len;

        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.final;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_get_list_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetImageListClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_GET_IMAGE_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_get_image_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C get img cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_get_image_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C get img cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_image_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_image_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_get_image_cfm.conn_id;
        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_cli_get_image_cfm.data_ptr;
        pEvCb->dataLen  = ev_msg_ptr->ev_msg.ev_bip_cli_get_image_cfm.data_len;

        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_cli_get_image_cfm.final;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_get_image_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetImageClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_GET_PARTIAL_IMAGE_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_get_partial_image_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C get partial img cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_get_partial_image_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C get partial img cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_partial_image_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_partial_image_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_get_partial_image_cfm.conn_id;
        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_cli_get_partial_image_cfm.data_ptr;
        pEvCb->dataLen = ev_msg_ptr->ev_msg.ev_bip_cli_get_partial_image_cfm.data_len;

        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_cli_get_partial_image_cfm.final;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_get_partial_image_cfm.result;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetPartialImageClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_GET_LINKED_ATTACHMENT_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_get_attachment_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C get attachment cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_get_attachment_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C get attachment cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_attachment_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_attachment_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_get_attachment_cfm.conn_id;
        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_cli_get_attachment_cfm.data_ptr;
        pEvCb->dataLen  = ev_msg_ptr->ev_msg.ev_bip_cli_get_attachment_cfm.data_len;

        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_cli_get_attachment_cfm.final;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_get_attachment_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetLinkedAttachmentClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_GET_LINKED_THUMBNAIL_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_get_thumbnail_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C get thumbnail cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_get_thumbnail_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C get thumbnail cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_thumbnail_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_thumbnail_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_get_thumbnail_cfm.conn_id;
        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_cli_get_thumbnail_cfm.data_ptr;
        pEvCb->dataLen = ev_msg_ptr->ev_msg.ev_bip_cli_get_thumbnail_cfm.data_len;

        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_cli_get_thumbnail_cfm.final;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_get_thumbnail_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetLinkedThumbnailClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_GET_MONITORING_IMAGE_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_get_monitor_image_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C get monitor img cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_get_monitor_image_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C get monitor img cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_monitor_image_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_monitor_image_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_get_monitor_image_cfm.conn_id;
        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_cli_get_monitor_image_cfm.data_ptr;
        pEvCb->dataLen = ev_msg_ptr->ev_msg.ev_bip_cli_get_monitor_image_cfm.data_len;

        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_cli_get_monitor_image_cfm.final;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_get_monitor_image_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetMonitoringImageClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_GET_STATUS_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_get_status_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C get status cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_get_status_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C get status cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_status_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_get_status_cfm.conn_id, 0 );

      (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                            pMe->clientConnID,
                                            BT_EV_PF_BIP_CLI_GET_STATUS_CFM);

      pN->uID         = AEEBT_BIP_EVT_GET_STATUS_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( 
        ev_msg_ptr->ev_msg.ev_bip_cli_get_status_cfm.result );

      break;
    }
    case BT_EV_PF_BIP_CLI_DELETE_IMAGE_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_delete_image_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C del img cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_delete_image_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C del img cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_delete_image_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_delete_image_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_delete_image_cfm.conn_id;

        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_delete_image_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_DeleteImageClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_START_ARCHIVE_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_start_archive_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C start archive cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_start_archive_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C start archive cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_start_archive_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_start_archive_cfm.conn_id, 0 );

      (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                            pMe->clientConnID,
                                            BT_EV_PF_BIP_CLI_START_ARCHIVE_CFM);

      pN->uID         = AEEBT_BIP_EVT_START_ARCHIVE_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( 
        ev_msg_ptr->ev_msg.ev_bip_cli_start_archive_cfm.result );
      break;
    }
    case BT_EV_PF_BIP_CLI_START_PRINT_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_start_print_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C start print cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_start_print_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C start print cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_start_print_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_start_print_cfm.conn_id, 0 );

      (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                            pMe->clientConnID,
                                            BT_EV_PF_BIP_CLI_START_PRINT_CFM);

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_start_print_cfm.conn_id;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_start_print_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_StartPrintClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_REMOTE_DISPLAY_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_remote_display_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C rem dis cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_remote_display_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C del rem dis, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_remote_display_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_remote_display_cfm.conn_id, 0 );

      (void) bt_cmd_pf_bip_cli_get_rsp_cfm( pMe->appId, 
                                            pMe->clientConnID,
                                            BT_EV_PF_BIP_CLI_REMOTE_DISPLAY_CFM);

      pN->uID         = AEEBT_BIP_EVT_REMOTE_DISPLAY_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( 
        ev_msg_ptr->ev_msg.ev_bip_cli_remote_display_cfm.result );
      break;
    }
    case BT_EV_PF_BIP_CLI_PUT_IMAGE_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_put_image_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C put img cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_put_image_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C put img cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_put_image_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_put_image_cfm.conn_id, 0 );

      if ( WSTRLEN( (AECHAR*) 
           &ev_msg_ptr->ev_msg.ev_bip_cli_put_image_cfm.image_handle ) > 0 )
      {
        // Save image handle string
        bt_pf_copy_unicode_str(
          (uint16*)pMe->wImageHandle,
          (uint16*)&ev_msg_ptr->ev_msg.ev_bip_cli_put_image_cfm.image_handle,
          BT_PF_BIP_IMAGE_HANDLE_LEN,
          NULL );
      }

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_put_image_cfm.conn_id;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_put_image_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_PutImageClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_PUT_LINKED_ATTACHMENT_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_put_attachment_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C put attachment cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_put_attachment_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C put attachment cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_put_attachment_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_put_attachment_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_put_attachment_cfm.conn_id;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_put_attachment_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_PutLinkedAttachmentClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_CLI_PUT_LINKED_THUMBNAIL_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bip_cli_put_thumbnail_cfm.conn_id )
      {
        MSG_ERROR( "BIPEvCb: C put thumbnail cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_cli_put_thumbnail_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BIPEvCb: C put thumbnail cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bip_cli_put_thumbnail_cfm.result, 
               ev_msg_ptr->ev_msg.ev_bip_cli_put_thumbnail_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_cli_put_thumbnail_cfm.conn_id;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_cli_put_thumbnail_cfm.result;
        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_PutLinkedThumbnailClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BIP_SRV_AUTH_IND:
    {
      MSG_LOW( "BIPEvCb: S auth ind, id=%x st=%x", 
               ev_msg_ptr->ev_msg.ev_bip_srv_auth_ind.conn_id, pMe->state, 0 );

      pMe->serverConnID = ev_msg_ptr->ev_msg.ev_bip_srv_auth_ind.conn_id;
      pN->uID = AEEBT_BIP_EVT_AUTH_REQ;
      break;
    }
    case BT_EV_PF_BIP_SRV_CON_IND:
    {
      MSG_LOW( "BIPEvCb: S con ind, id=%x st=%x", 
               ev_msg_ptr->ev_msg.ev_bip_srv_con_ind.conn_id, pMe->state, 0 );

      pMe->serverConnID = ev_msg_ptr->ev_msg.ev_bip_srv_con_ind.conn_id;
      pN->uID = AEEBT_BIP_EVT_CONN_REQ;
      pN->data.bdAddr = *((AEEBTBDAddr*)&ev_msg_ptr->ev_msg.ev_bip_srv_con_ind.bd_addr);  
      break;
    }
    case BT_EV_PF_BIP_SRV_DCN_IND:
    {
      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_dcn_ind.conn_id;
        pEvCb->bServerEvt = TRUE;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtBIP_DisconnectCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_GET_CAPABILITIES_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_get_capabilities_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S get cap ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_get_capabilities_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_capabilities_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( 
                               pMe->appId, 
                               pMe->serverConnID,
                               BT_EV_PF_BIP_SRV_GET_CAPABILITIES_IND, 
                               BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_capabilities_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        pN->uID = AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_INITIAL;
        break;
      }

      // Subsequent events are handled locally without app intervention
      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_get_capabilities_ind.conn_id;
        pEvCb->state    = ev_msg_ptr->ev_msg.ev_bip_srv_get_capabilities_ind.state;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_srv_get_capabilities_ind.status;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetCapabilitiesServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_GET_IMAGE_PROPERTIES_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_get_properties_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S get img prop ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_get_properties_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_properties_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( 
                               pMe->appId, 
                               pMe->serverConnID,
                               BT_EV_PF_BIP_SRV_GET_IMAGE_PROPERTIES_IND, 
                               BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_properties_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Pass clients image handle to the app
        bt_pf_copy_unicode_str(
          (uint16*)pN->data.BipObject.wImageHandle,
          (uint16*)&ev_msg_ptr->ev_msg.ev_bip_srv_get_properties_ind.image_handle,
          BT_PF_BIP_IMAGE_HANDLE_LEN,
          NULL );

        pN->uID = AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_INITIAL;
        break;
      }

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_get_properties_ind.conn_id;
        pEvCb->state    = ev_msg_ptr->ev_msg.ev_bip_srv_get_properties_ind.state;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_srv_get_properties_ind.status;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetImagePropertiesServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_GET_IMAGES_LIST_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S get img list ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( 
                         pMe->appId, 
                         pMe->serverConnID,
                         BT_EV_PF_BIP_SRV_GET_IMAGES_LIST_IND, 
                         BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Pass on client's parameters to the app
        pN->data.BipObject.handleCount = ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.handle_count;
        pN->data.BipObject.startOffset = ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.list_offset;
        pN->data.BipObject.bLatest     = ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.latest;

        descriptorLen = ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.handles_descriptor_len;
        if ( descriptorLen > AEEBT_BIP_MAX_IMAGE_DESC_LEN ) 
        {
          descriptorLen = AEEBT_BIP_MAX_IMAGE_DESC_LEN;
        }

        MEMCPY( (void*) pN->data.BipObject.ImageDescriptor, 
                (void*) ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.handles_descriptor_ptr,
                descriptorLen );
        
        // Null terminate the string
        pN->data.BipObject.ImageDescriptor[ descriptorLen ] = '\0';

        pN->uID = AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_INITIAL;
        break;
      }

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.conn_id;
        pEvCb->state    = ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.state;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_srv_get_list_ind.status;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetImageListServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_GET_IMAGE_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_get_image_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S get img ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_get_image_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_image_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
         (void) bt_cmd_pf_bip_srv_req_cfm( 
                               pMe->appId, 
                               pMe->serverConnID,
                               BT_EV_PF_BIP_SRV_GET_IMAGE_IND, 
                               BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_GET_IMAGE_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_image_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Pass client's image handle and descriptor filter to the app
        bt_pf_copy_unicode_str(
          (uint16*)pN->data.BipObject.wImageHandle,
          (uint16*)&ev_msg_ptr->ev_msg.ev_bip_srv_get_image_ind.image_handle,
          BT_PF_BIP_IMAGE_HANDLE_LEN,
          NULL );

        descriptorLen = ev_msg_ptr->ev_msg.ev_bip_srv_get_image_ind.image_descriptor_len;
        if ( descriptorLen > AEEBT_BIP_MAX_IMAGE_DESC_LEN ) 
        {
          descriptorLen = AEEBT_BIP_MAX_IMAGE_DESC_LEN;
        }
        MEMCPY( (void*) pN->data.BipObject.ImageDescriptor, 
                (void*) ev_msg_ptr->ev_msg.ev_bip_srv_get_image_ind.image_descriptor_ptr,
                descriptorLen );

        // Null terminate the string
        pN->data.BipObject.ImageDescriptor[ descriptorLen ] = '\0';

        pN->uID = AEEBT_BIP_EVT_GET_IMAGE_REQ_INITIAL;
        break;
      }

      // Subsequent events are handled locally without app intervention
      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_get_image_ind.conn_id;
        pEvCb->state    = ev_msg_ptr->ev_msg.ev_bip_srv_get_image_ind.state;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_srv_get_image_ind.status;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetImageServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_GET_PARTIAL_IMAGE_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_get_partial_image_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S get partial img ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_get_partial_image_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_partial_image_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( 
                               pMe->appId, 
                               pMe->serverConnID,
                               BT_EV_PF_BIP_SRV_GET_PARTIAL_IMAGE_IND, 
                               BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_partial_image_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Pass client parameters to the app
        bt_pf_copy_unicode_str(
          (uint16*)pN->data.BipObject.wImageName,
          (uint16*)ev_msg_ptr->ev_msg.ev_bip_srv_get_partial_image_ind.image_name_str,
          BT_PF_MAX_FILENAME_LEN,
          NULL );

        pN->data.BipObject.partialLength = 
          ev_msg_ptr->ev_msg.ev_bip_srv_get_partial_image_ind.partial_length;
        pN->data.BipObject.partialOffset = 
          ev_msg_ptr->ev_msg.ev_bip_srv_get_partial_image_ind.partial_offset;

        pN->uID = AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_INITIAL;
        break;
      }

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_get_partial_image_ind.conn_id;
        pEvCb->state    = ev_msg_ptr->ev_msg.ev_bip_srv_get_partial_image_ind.state;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_srv_get_partial_image_ind.status;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetPartialImageServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_GET_LINKED_ATTACHMENT_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_get_attachment_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S get attachment ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_get_attachment_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_attachment_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( 
                               pMe->appId, 
                               pMe->serverConnID,
                               BT_EV_PF_BIP_SRV_GET_LINKED_ATTACHMENT_IND, 
                               BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_attachment_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Pass attachment name and image handle to the app
        bt_pf_copy_unicode_str(
          (uint16*)pN->data.BipObject.wAttachmentName,
          (uint16*)ev_msg_ptr->ev_msg.ev_bip_srv_get_attachment_ind.attachment_name_str,
          BT_PF_MAX_FILENAME_LEN,
          NULL );

        bt_pf_copy_unicode_str(
          (uint16*)pN->data.BipObject.wImageHandle,
          (uint16*)&ev_msg_ptr->ev_msg.ev_bip_srv_get_attachment_ind.image_handle,
          BT_PF_BIP_IMAGE_HANDLE_LEN,
          NULL );

        pN->uID = AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_INITIAL;
        break;
      }

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_get_attachment_ind.conn_id;
        pEvCb->state    = ev_msg_ptr->ev_msg.ev_bip_srv_get_attachment_ind.state;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_srv_get_attachment_ind.status;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetLinkedAttachmentServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_GET_LINKED_THUMBNAIL_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_get_thumbnail_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S get thumbnail ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_get_thumbnail_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_thumbnail_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( 
                               pMe->appId, 
                               pMe->serverConnID,
                               BT_EV_PF_BIP_SRV_GET_LINKED_THUMBNAIL_IND, 
                               BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_thumbnail_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Pass image handle to the app
        bt_pf_copy_unicode_str(
          (uint16*)pN->data.BipObject.wImageHandle,
          (uint16*)&ev_msg_ptr->ev_msg.ev_bip_srv_get_thumbnail_ind.image_handle,
          BT_PF_BIP_IMAGE_HANDLE_LEN,
          NULL );

        pN->uID = AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_INITIAL;
        break;
      }

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_get_thumbnail_ind.conn_id;
        pEvCb->state    = ev_msg_ptr->ev_msg.ev_bip_srv_get_thumbnail_ind.state;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_srv_get_thumbnail_ind.status;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetLinkedThumbnailServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_GET_MONITORING_IMAGE_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_get_monitor_image_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S get monitor img ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_get_monitor_image_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_monitor_image_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( 
                               pMe->appId, 
                               pMe->serverConnID,
                               BT_EV_PF_BIP_SRV_GET_MONITORING_IMAGE_IND, 
                               BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_get_monitor_image_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Let the app know whether the client wants to capture the image
        pN->data.BipObject.bStoreFlag = 
          ev_msg_ptr->ev_msg.ev_bip_srv_get_monitor_image_ind.store_image;

        pN->uID = AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_INITIAL;
        break;
      }

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event      = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId      = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId     = ev_msg_ptr->ev_msg.ev_bip_srv_get_monitor_image_ind.conn_id;
        pEvCb->state      = ev_msg_ptr->ev_msg.ev_bip_srv_get_monitor_image_ind.state;
        pEvCb->status     = ev_msg_ptr->ev_msg.ev_bip_srv_get_monitor_image_ind.status;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_GetMonitoringImageServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_GET_STATUS_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_get_status_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S get status ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_get_status_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                        pMe->serverConnID,
                                        BT_EV_PF_BIP_SRV_GET_STATUS_IND, 
                                        BT_CS_GN_SUCCESS );

      // Return Success if the secondary connection has successfully terminated,
      // else return Continue response.  
      if ( pMe->clientConnID == BT_PF_BIP_NO_CONN_ID ) 
      {
        bt_cmd_pf_bip_srv_get_status_response( pMe->appId, 
                                               pMe->serverConnID,
                                               BT_CS_GN_SUCCESS );
      }
      else
      {
        bt_cmd_pf_bip_srv_get_status_response( pMe->appId, 
                                               pMe->serverConnID,
                                               BT_CS_PF_OBEX_CONTINUE );
      }

      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_DELETE_IMAGE_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_delete_image_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S del img, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_delete_image_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_delete_image_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                          pMe->serverConnID,
                                          BT_EV_PF_BIP_SRV_DELETE_IMAGE_IND, 
                                          BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_DELETE_IMAGE_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_delete_image_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Give the image handle to the app
        bt_pf_copy_unicode_str(
          (uint16*)pN->data.BipObject.wImageHandle,
          (uint16*)&ev_msg_ptr->ev_msg.ev_bip_srv_delete_image_ind.image_handle,
          BT_PF_BIP_IMAGE_HANDLE_LEN,
          NULL );

        pN->uID = AEEBT_BIP_EVT_DELETE_IMAGE_REQ_INITIAL;
        break;
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_START_ARCHIVE_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_start_archive_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S start archive, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_start_archive_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_start_archive_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                          pMe->serverConnID,
                                          BT_EV_PF_BIP_SRV_START_ARCHIVE_IND, 
                                          BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_START_ARCHIVE_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_start_archive_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Give serviceID of ArchivedObjects server to the app
        MEMCPY( (void*) &pN->data.BipObject.serviceID,
                (void*) &ev_msg_ptr->ev_msg.ev_bip_srv_start_archive_ind.arch_obj_srv_service_id,
                sizeof ( pN->data.BipObject.serviceID ) );

        pN->uID = AEEBT_BIP_EVT_START_ARCHIVE_REQ_INITIAL;
        break;
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
    case BT_EV_PF_BIP_SRV_REMOTE_DISPLAY_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_remote_display_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S del img, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_remote_display_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_remote_display_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                          pMe->serverConnID,
                                          BT_EV_PF_BIP_SRV_REMOTE_DISPLAY_IND, 
                                          BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_remote_display_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Give the image handle and display command to the app
        bt_pf_copy_unicode_str(
          (uint16*)pN->data.BipObject.wImageHandle,
          (uint16*)&ev_msg_ptr->ev_msg.ev_bip_srv_remote_display_ind.image_handle,
          BT_PF_BIP_IMAGE_HANDLE_LEN,
          NULL );

        pN->data.BipObject.displayCommand = 
          ev_msg_ptr->ev_msg.ev_bip_srv_remote_display_ind.display_command;

        pN->uID = AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_INITIAL;
        break;
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_PUT_IMAGE_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S put img ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( pMe->appId, 
                                          pMe->serverConnID,
                                          BT_EV_PF_BIP_SRV_PUT_IMAGE_IND, 
                                          BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_PUT_IMAGE_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        // Save the image name
        bt_pf_copy_unicode_str(
          (uint16*)pMe->wImageName,
          (uint16*)ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.image_name_str,
          BT_PF_MAX_FILENAME_LEN,
          NULL );

        pMe->pData    = ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.data_ptr;
        pMe->dataLen  = ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.data_len;
        pMe->bFinal   = ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.final;

        // Pass client's image descriptor to the app
        descriptorLen = ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.image_descriptior_len;
        if ( descriptorLen > AEEBT_BIP_MAX_IMAGE_DESC_LEN ) 
        {
          descriptorLen = AEEBT_BIP_MAX_IMAGE_DESC_LEN;
        }
        MEMCPY( (void*) pN->data.BipObject.ImageDescriptor, 
                (void*) ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.image_descriptior_ptr,
                descriptorLen );

         // Null terminate the string
        pN->data.BipObject.ImageDescriptor[ descriptorLen ] = '\0';

        pN->uID = AEEBT_BIP_EVT_PUT_IMAGE_REQ_INITIAL;
        break;
      }

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.conn_id;
        pEvCb->state    = ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.state;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.status;
        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.final;

        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.data_ptr;
        pEvCb->dataLen  = ev_msg_ptr->ev_msg.ev_bip_srv_put_image_ind.data_len;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_PutImageServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_PUT_LINKED_ATTACHMENT_IND:
    {
      if ( pMe->serverConnID != 
           ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S put attachment ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( 
                         pMe->appId, 
                         pMe->serverConnID,
                         BT_EV_PF_BIP_SRV_PUT_LINKED_ATTACHMENT_IND, 
                         BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_PUT_ATTACHMENT_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        pMe->pData    = ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.data_ptr;
        pMe->dataLen  = ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.data_len;
        pMe->bFinal   = ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.final;

        // Pass client's image descriptor and image handle to the app
        bt_pf_copy_unicode_str(
          (uint16*)pN->data.BipObject.wImageHandle,
          (uint16*)&ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.image_handle,
          BT_PF_BIP_IMAGE_HANDLE_LEN,
          NULL );

        descriptorLen = ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.attachment_descriptor_len;
        if ( descriptorLen > AEEBT_BIP_MAX_IMAGE_DESC_LEN ) 
        {
          descriptorLen = AEEBT_BIP_MAX_IMAGE_DESC_LEN;
        }
        MEMCPY( (void*) pN->data.BipObject.ImageDescriptor, 
                (void*) ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.attachment_descriptor_ptr,
                ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.attachment_descriptor_len );

        // Null terminate the string
        pN->data.BipObject.ImageDescriptor[ descriptorLen ] = '\0';

        pN->uID = AEEBT_BIP_EVT_PUT_ATTACHMENT_REQ_INITIAL;
        break;
      }

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.conn_id;
        pEvCb->state    = ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.state;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.status;
        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.final;

        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.data_ptr;
        pEvCb->dataLen = ev_msg_ptr->ev_msg.ev_bip_srv_put_attachment_ind.data_len;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_PutLinkedAttachmentServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_BIP_SRV_PUT_LINKED_THUMBNAIL_IND:
    {
      if ( pMe->serverConnID != 
           ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.conn_id )
      {
        MSG_ERROR( "BIPEvCb: S put thumbnail ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      // This is the first or last ind event.  Notify app
      if ( ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.state == 
           BT_PF_BIP_SRV_REQ_CLEANUP ) 
      {
        (void) bt_cmd_pf_bip_srv_req_cfm( 
                         pMe->appId, 
                         pMe->serverConnID,
                         BT_EV_PF_BIP_SRV_PUT_LINKED_THUMBNAIL_IND, 
                         BT_CS_GN_SUCCESS );

        pN->uID = AEEBT_BIP_EVT_PUT_THUMBNAIL_REQ_CLEANUP;
        break;
      }
      else if ( ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.state == 
                BT_PF_BIP_SRV_REQ_INITIAL ) 
      {
        pMe->pData    = ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.data_ptr;
        pMe->dataLen  = ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.data_len;
        pMe->bFinal   = ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.final;

        // Pass client's image handle to the app
        bt_pf_copy_unicode_str(
          (uint16*)pN->data.BipObject.wImageHandle,
          (uint16*)&ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.image_handle,
          BT_PF_BIP_IMAGE_HANDLE_LEN,
          NULL );

        pN->uID = AEEBT_BIP_EVT_PUT_THUMBNAIL_REQ_INITIAL;
        break;
      }

      pEvCb = OEMBTExtBIP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.conn_id;
        pEvCb->state    = ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.state;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.status;
        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.final;

        pEvCb->pData    = ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.data_ptr;
        pEvCb->dataLen  = ev_msg_ptr->ev_msg.ev_bip_srv_put_thumbnail_ind.data_len;

        CALLBACK_Init( &pMe->cb, OEMBTExtBIP_PutLinkedThumbnailServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    default:
    {
      MSG_ERROR( "BIPEvCb - unexpect event %x", 
                 ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BIP );
  IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
}

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BIP)

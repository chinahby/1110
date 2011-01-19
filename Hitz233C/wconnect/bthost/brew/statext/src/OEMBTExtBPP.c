/*===========================================================================

FILE:      OEMBTExtBPP.c

SERVICES:  BlueTooth Basic Printing Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Basic Printing Profile.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2004-2008 by QUALCOMM Incorporated. 
                All Rights Reserved.
            Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtBPP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
$Author: deepikas $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2008-10-07   pn  Replaced deprecated functions.

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BPP)
#include "bt.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "oi_obexspec.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtBPP.h"

//==========================================================================
//   Macro definitions
//==========================================================================
#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
#define BPP_DATA_BUFFER_SIZE   AEEBT_OBEX_BUFFER_SIZE
#else
#define BPP_DATA_BUFFER_SIZE   32757
#endif /* FEATURE_BT_REDUCE_MEMORY_USAGE */
#else
#define BPP_DATA_BUFFER_SIZE   4266
#endif /* FEATURE_BT_SOC */

//==========================================================================
//   Type definitions
//==========================================================================

typedef struct OEMBTExtBPPobj_struct
{
  ACONTEXT*                   pac;
  IBTExtNotifier*             pNotifier;

  bt_app_id_type              appId;
  AEEBTBDAddr                 BDAddr;         // remote device's

  IShell*                     pIShell;

  IFile*                      pFile;
  FileInfo                    fileInfo;
  AECHAR                      wFileName[ AEEBT_MAX_FILE_NAME + 1 ];
  char                        type[ 100 ];

  uint32*                     pJobID;

  char*                       pUserBuffer;
  uint16*                     pUserBufferLen;
  uint16                      userBufferMaxSize;

  uint32                      bytesSent;
  uint32                      bytesRcvd;

  uint32                      SOAPbytesSent;
  uint32                      SOAPbytesRcvd;

  // Job and Status channel connIDs
  uint16                      clientConnID;
  uint16                      clientSTSConnID;

  // Object channel connID
  uint16                      serverConnID;

  char*                       pDataBuffer;

  AEEBTBPPbyteSeqType         data;  
  AEEBTBPPbyteSeqType         SOAPdata; 

  boolean                     bIsServer;
  boolean                     bDoDeregister; 

  // Target that the current transaction (e.g, SOAPRequest)
  // corresponds to
  AEEBTBPPTargetType          target;

  // GetReferenceObjects parameters
  AECHAR                      wReferenceName[ AEEBT_MAX_FILE_NAME + 1 ];
  boolean                     bGetFileSize;
  uint32                      offset;
  int32                       count;

  // SOAP message buffer 
  char                        sndrSOAPmsg[2];

  AEECallback                 cb;

  struct OEMBTExtBPPobj_struct* pNextBPP;  // linked into mgr
} OEMBTExtBPPobj_t;

typedef struct
{
  boolean                     bInUse;
  boolean                     final;
  bt_event_type               event;
  bt_app_id_type              appId;
  uint16                      connId;

  bt_cmd_status_type          status;

  uint32                      jobID;

  uint8*                      pReply;
  uint16                      replyLen;
} OEMBTExtBPP_EvCb;

// one of these
typedef struct
{
  OEMBTExtBPPobj_t* pNextBPP;    // singly linked list of BPPs
  OEMBTExtBPP_EvCb  FreeEvCb[ 10 ];
} OEMBTExtBPP_Mgr;

//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static int  OEMBTExtBPP_CheckCmdStatus( bt_cmd_status_type stat );
static void OEMBTExtBPP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);
static OEMBTExtBPP_EvCb* OEMBTExtBPP_GetFreeEvCb( void );
static char* OEMBTExtBPP_MapDocumentType( AEEBTBPPdocType docType );

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtBPP_Mgr        gMgr;               // manager object

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtBPP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtBPP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtBPP_Init(IBTExtBPP* pParent)
{
  OEMBTExtBPPobj_t* pMe;

  // allocate pMe
  pMe = MALLOCREC(OEMBTExtBPPobj_t);
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
  pMe->appId = bt_cmd_ec_get_app_id_and_register(OEMBTExtBPP_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    FREE( pMe );
    return EFAILED;
  }

  if ( ( pMe->pDataBuffer = MALLOC( BPP_DATA_BUFFER_SIZE ) ) == NULL )
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

  if ( gMgr.pNextBPP == NULL ) // first time?
  {
    uint8 i;
    for ( i = 0; i < (sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++ )
    {
      gMgr.FreeEvCb[ i ].bInUse = FALSE;
    }
  }

  pMe->pIShell = pParent->m_pIShell;

  // link into mgr
  pMe->pNextBPP = gMgr.pNextBPP;
  gMgr.pNextBPP = pMe;

  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtBPP_Release( IBTExtBPP* pParent )
{
  OEMBTExtBPPobj_t*  pMe;
  OEMBTExtBPPobj_t** ppc;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) == TRUE )
  {
    // unlink from mgr
    for (ppc = &gMgr.pNextBPP; *ppc != NULL; ppc = &(*ppc)->pNextBPP)
    {
      if ((*ppc)->appId == pMe->appId)
      {
        *ppc = pMe->pNextBPP;
        pMe->pNextBPP = NULL;
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

// IBTEXTBPP_Register (see AEEBTExtBPP.h)
int OEMBTExtBPP_Register( 
  IBTExtBPP*     pParent, 
  const char*    pSrvName 
)
{
  bt_cmd_status_type stat;
  OEMBTExtBPPobj_t   *pMe;

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

  stat = bt_cmd_pf_bpp_sndr_register( pMe->appId,  
                                      (char*) pSrvName );

  return OEMBTExtBPP_CheckCmdStatus( stat );
}

// IBTEXTBPP_Deregister (see AEEBTExtBPP.h)
int OEMBTExtBPP_Deregister( IBTExtBPP* pParent )
{
  bt_cmd_status_type          stat;
  OEMBTExtBPPobj_t            *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_bpp_sndr_deregister( pMe->appId );

  return OEMBTExtBPP_CheckCmdStatus( stat );
}

// IBTEXTBPP_Authenticate (see AEEBTExtBPP.h)
int OEMBTExtBPP_Authenticate( 
  IBTExtBPP*          pParent,
  AEEBTBPPTargetType  target,
  const AECHAR*       pUserID,
  const AECHAR*       pPassword
)
{
  bt_cmd_status_type  stat;
  OEMBTExtBPPobj_t    *pMe;

  char userID[ AEEBT_MAX_USERID_LEN + 1 ];
  char password[ AEEBT_MAX_PASSWORD_LEN + 1 ];

  uint8 userIDLen = 0;
  /*
  * UserId is optional
  */
  if(pUserID != NULL) 
  {
    AEEBT_FROM_WSTR( pUserID, userID, sizeof( userID ) );
    userIDLen = STRLEN( userID );
  }
  /*
  * Password is mandatory
  * return Bad paremter if password is NULL
  */
  if(pPassword != NULL) 
  {
    AEEBT_FROM_WSTR( pPassword, password, sizeof( password ) );
  }
  else
  {
      return EBADPARM ;
  }

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_bpp_sndr_auth_response( pMe->appId, 
                                           (bt_pf_bpp_target_type) target,
                                           (byte*) userID, 
                                           userIDLen,
                                           password );

  return OEMBTExtBPP_CheckCmdStatus( stat );
}

// IBTEXTBPP_AcceptConnection (see AEEBTExtBPP.h)
int OEMBTExtBPP_AcceptConnection( 
  IBTExtBPP*          pParent
)
{
  bt_cmd_status_type  stat;
  OEMBTExtBPPobj_t    *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_bpp_sndr_srv_req_cfm( pMe->appId, 
                                         pMe->serverConnID,
                                         BT_EV_PF_BPP_SNDR_CON_IND, 
                                         BT_CS_GN_SUCCESS );

  return OEMBTExtBPP_CheckCmdStatus( stat );
}

// IBTEXTBPP_Connect (see AEEBTExtBPP.h)
int OEMBTExtBPP_Connect( 
  IBTExtBPP*             pParent, 
  const AEEBTBDAddr*     pBDAddr, 
  uint8                  uChannelNumber,
  AEEBTBPPTargetType     target
)
{
  bt_cmd_status_type  stat;
  OEMBTExtBPPobj_t    *pMe;

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

  stat = bt_cmd_pf_bpp_sndr_connect( pMe->appId, 
                                     (bt_bd_addr_type*) pBDAddr,
                                     uChannelNumber,
                                     (bt_pf_bpp_target_type) target );

  return OEMBTExtBPP_CheckCmdStatus( stat );
}

// IBTEXTBPP_Disconnect (see AEEBTExtBPP.h)
int OEMBTExtBPP_Disconnect( 
  IBTExtBPP*             pParent, 
  AEEBTBPPTargetType     target
)
{
  bt_cmd_status_type  stat;
  OEMBTExtBPPobj_t *pMe;

  uint16 connID = 0;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  // Pick the right connId based on target type
  if ( target == AEEBT_BPP_TARGET_DPS ) 
  {
    connID = pMe->clientConnID;
  }
  else if ( target == AEEBT_BPP_TARGET_STS) 
  {
    connID = pMe->clientSTSConnID;
  }
  else
  {
    // Invalid target
    MSG_ERROR( "BPP_Disconnect - Invalid target=%x", target, 0, 0 );
    return EBADPARM;
  }

  stat = bt_cmd_pf_bpp_sndr_disconnect( pMe->appId, connID );
 
  return OEMBTExtBPP_CheckCmdStatus( stat );
}

// IBTEXTBPP_Abort (see AEEBTExtBPP.h)
int OEMBTExtBPP_Abort( IBTExtBPP *pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtBPPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  // It makes more sense to abort operations on the STS channel in BPP
  // as this is the place were the printer can wait for infinite period
  // without responding to any "get" operation.
  // Best would be to provide one more parameter that puts in the target 
  // as well.
  stat = bt_cmd_pf_bpp_sndr_abort( pMe->appId, pMe->clientSTSConnID );

  return OEMBTExtBPP_CheckCmdStatus( stat );
}

// IBTEXTBPP_SendFile (see AEEBTExtBPP.h)
int OEMBTExtBPP_SendFile( 
  IBTExtBPP*      pParent, 
  AEEBTBPPdocType documentType,
  const char*     pFileName,
  const AECHAR*   pDescriptor,
  uint32          jobID
)
{
  bt_cmd_status_type  stat;
  OEMBTExtBPPobj_t *pMe;

  int       bytesRead       = 0;
  boolean   final           = 0;
  IFileMgr* pIFileMgr       = NULL;
  char*     pType           = NULL;

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

  if ( (pMe->pFile = 
        IFILEMGR_OpenFile( pIFileMgr, pFileName, _OFM_READ )) == NULL )
  {
    MSG_ERROR( "SendFile: open file failed", 0, 0, 0 );
    return EFAILED;
  }

  // get filesize info
  if ( IFILE_GetInfo( pMe->pFile, &pMe->fileInfo ) == EFAILED )
  {
    return EFAILED;
  }

  pType = OEMBTExtBPP_MapDocumentType( documentType );

  bytesRead = BPP_DATA_BUFFER_SIZE;
  bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
  if ( bytesRead == 0 )
  {
    MSG_ERROR( "SendFile: failed to read from obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;
    
    return EFAILED;
  }
  else if ( bytesRead >= pMe->fileInfo.dwSize )
  {
    final = 1;

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;
  }

  pMe->data.AEEBTBPPdata    = (byte*) pMe->pDataBuffer;
  pMe->data.AEEBTBPPdataLen = bytesRead;
  pMe->bytesSent            = bytesRead;

  if (jobID == 0) 
  {
    pMe->pJobID = NULL;
  }
  else
  {
    pMe->pJobID = &jobID;
  }

  AEEBT_TO_WSTR( pFileName, pMe->wFileName, sizeof( pMe->wFileName ) );
  STRLCPY( pMe->type, pType, sizeof(pMe->type) );

  IFILEMGR_Release( pIFileMgr );

  stat = bt_cmd_pf_bpp_sndr_send_file( pMe->appId, 
                                       pMe->clientConnID,
                                       pType,
                                       (uint16*) pDescriptor,
                                       (uint16*) pMe->wFileName,
                                       (bt_pf_bpp_job_id_type*) pMe->pJobID,
                                       (bt_pf_byte_seq_type*) &pMe->data,
                                       final );

  return OEMBTExtBPP_CheckCmdStatus( stat );
}

// IBTEXTBPP_SOAPRequest (see AEEBTExtBPP.h)
int OEMBTExtBPP_SOAPRequest( 
  IBTExtBPP*         pParent, 
  const char*        pSndrSOAPmsg,
  char*              pSOAPmsg,
  uint16             SOAPmsgMaxLen,
  AEEBTBPPTargetType target )
{
  bt_cmd_status_type  stat;
  OEMBTExtBPPobj_t *pMe;

  uint16  connID         = 0;  
  boolean final;

  uint16  sndrSOAPmsgLen;

  if (NULL == pSndrSOAPmsg) {
      sndrSOAPmsgLen = 0;
  }
  else 
  {
      sndrSOAPmsgLen = STRLEN( pSndrSOAPmsg );
  }

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  // Return SOAP response message in the user supplied buffer
  pMe->SOAPbytesRcvd        = 0;
  pMe->pUserBuffer          = pSOAPmsg;
  pMe->userBufferMaxSize    = SOAPmsgMaxLen;

  pMe->SOAPdata.AEEBTBPPdata    = (byte*) pSndrSOAPmsg;

  // Sanity check SOAP message length
  if ( sndrSOAPmsgLen > 0 ) 
  {
    final = 0;
    pMe->SOAPdata.AEEBTBPPdataLen = sndrSOAPmsgLen;
    pMe->SOAPbytesSent            = sndrSOAPmsgLen;    
  }
  else
  {
    final = 1;
    pMe->SOAPdata.AEEBTBPPdataLen = 0;
    pMe->SOAPbytesSent            = 0;

    if (pMe->sndrSOAPmsg != NULL) {
        pMe->sndrSOAPmsg[0] = '\0';
    }
  }

  // Find out on what channel, i.e, connection the SOAP request
  // is going out on
  if ( target == AEEBT_BPP_TARGET_DPS ) 
  {
    connID = pMe->clientConnID;
  }
  else if ( target == AEEBT_BPP_TARGET_STS ) 
  {
    connID = pMe->clientSTSConnID;
  }
  else
  {
    MSG_ERROR( "BPP_SOAPRequest - Invalid target=%x", target, 0, 0 );
    return EBADPARM;
  }

  // Remeber what connection we are sending SOAP message on
  pMe->target = target;

  stat = bt_cmd_pf_bpp_sndr_soap_request( 
                         pMe->appId, 
                         connID,
                         (bt_pf_byte_seq_type*) &pMe->SOAPdata,
                         final );

  return OEMBTExtBPP_CheckCmdStatus( stat );
}

// IBTEXTBPP_GetRefObjResponse (see AEEBTExtBPP.h)
int OEMBTExtBPP_GetRefObjResponse( 
  IBTExtBPP*    pParent, 
  const char*   pRefObjName,
  uint32        offset,
  int32         count )
{
  bt_cmd_status_type  stat;
  OEMBTExtBPPobj_t *pMe;

  uint32 bytesRead          = 0;
  int32* pFileSize          = NULL;
  bt_cmd_status_type status = BT_CS_GN_SUCCESS;
  IFileMgr* pIFileMgr       = NULL;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  // Initialization
  pMe->bytesSent = 0;

  pMe->data.AEEBTBPPdataLen = 0;
  pMe->data.AEEBTBPPdata    = NULL;

  // Sanity check
  if (pMe->pFile != NULL) 
  {
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }
  
  if ( (pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                        pRefObjName,
                                        _OFM_READ )) == NULL )
  {
    MSG_ERROR( "GetRefObjResponse: invalid file=%s", pRefObjName, 0, 0 );
    status = BT_CS_PF_READ_ERROR;
  }
  else
  {
    // Get filesize info
    if ( IFILE_GetInfo( pMe->pFile, &pMe->fileInfo ) == EFAILED )
    {
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;

      return EFAILED;
    }

    if ( count < BPP_DATA_BUFFER_SIZE && count > -1 ) 
    {
      bytesRead = count;
    }
    else
    {
      bytesRead = BPP_DATA_BUFFER_SIZE;
    }

    if ( offset > 0 && offset < pMe->fileInfo.dwSize ) 
    {
      IFILE_Seek( pMe->pFile, _SEEK_START, offset );
    }

    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    pMe->bytesSent += bytesRead;

    if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
    {
      status = BT_CS_GN_SUCCESS;

      IFILE_Release( pMe->pFile );  
      pMe->pFile = NULL;   
    }

    if (pIFileMgr) 
    {
      IFILEMGR_Release( pIFileMgr );
    }

    if ( pMe->bGetFileSize == TRUE ) 
    {
      pFileSize = (int32*) &pMe->fileInfo.dwSize;
    }

    pMe->data.AEEBTBPPdataLen = bytesRead;
    pMe->data.AEEBTBPPdata    = (byte*) pMe->pDataBuffer;
  }

  stat = bt_cmd_pf_bpp_sndr_get_ref_obj_response( 
                                           pMe->appId, 
                                           pMe->serverConnID,
                                           (bt_pf_byte_seq_type*) &pMe->data,
                                           pFileSize,
                                           status ); 

  return OEMBTExtBPP_CheckCmdStatus( stat );
}

//==========================================================================
//   static helper functions
//==========================================================================
static int OEMBTExtBPP_CheckCmdStatus( bt_cmd_status_type stat )
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

static OEMBTExtBPPobj_t* OEMBTExtBPP_FindMe(bt_app_id_type appId)
{
  OEMBTExtBPPobj_t* pMe;

  for (pMe = gMgr.pNextBPP; pMe != NULL; pMe = pMe->pNextBPP)
  {
    if (pMe->appId == appId)
    {
      return pMe;
    }
  }
  return NULL;
}

static boolean OEMBTExtBPP_HandleCmdDone( 
  bt_ev_gn_cmd_done_type* pCmdDn, 
  AEEBTNotification*      pN,
  OEMBTExtBPPobj_t*       pMe
)
{
  boolean doSendNotif = TRUE;

  MSG_LOW( "BPP_HandleCmdDone - stat=%x cmd=%x", pCmdDn->cmd_status, pCmdDn->cmd_type, 0 );

  switch ( pCmdDn->cmd_type )
  {
    case BT_PF_CMD_BPP_SNDR_REGISTER:
    {
      pN->uID = AEEBT_BPP_EVT_REG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );

      break;
    }
    case BT_PF_CMD_BPP_SNDR_DEREGISTER:
    {
      pN->uID = AEEBT_BPP_EVT_DEREG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_BPP_SNDR_DISCONNECT:
    {
      pN->uID = AEEBT_BPP_EVT_DISCONNECTED;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );

      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }      
      break;
    }
    case BT_PF_CMD_BPP_SNDR_SOAP_REQ:
    {
      pN->uID = AEEBT_BPP_EVT_SOAP_REQUEST_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );

      if ( pN->data.uError == AEEBT_ERR_NONE ) 
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_BPP_SNDR_CONNECT:
    {
      pN->uID = AEEBT_BPP_EVT_CONN_FAILED;
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

static OEMBTExtBPP_EvCb* OEMBTExtBPP_GetFreeEvCb( void )
{
  uint8 i;
  OEMBTExtBPP_EvCb* pEvCb = NULL;

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

static char* OEMBTExtBPP_MapDocumentType( AEEBTBPPdocType docType )
{
  switch ( docType ) 
  {
  case AEEBT_BPP_DOC_FORMAT_XHTML_PRINT:
    return (char*) AEEBT_BPP_DOC_TYPE_XHTML_PRINT;

  case AEEBT_BPP_DOC_FORMAT_BASIC_TEXT:
    return (char*) AEEBT_BPP_DOC_TYPE_BASIC_TEXT;

  case AEEBT_BPP_DOC_FORMAT_VCARD:
    return (char*) AEEBT_BPP_DOC_TYPE_VCARD;

  case AEEBT_BPP_DOC_FORMAT_VCARD_3_0:
    return (char*) AEEBT_BPP_DOC_TYPE_VCARD;

  case AEEBT_BPP_DOC_FORMAT_VCALENDAR:
    return (char*) AEEBT_BPP_DOC_TYPE_VCALENDAR;

  case AEEBT_BPP_DOC_FORMAT_VCALENDAR_2_0:
    return (char*) AEEBT_BPP_DOC_TYPE_VCALENDAR;

  case AEEBT_BPP_DOC_FORMAT_VMESSAGE:
    return (char*) AEEBT_BPP_DOC_TYPE_VMESSAGE;

  case AEEBT_BPP_DOC_FORMAT_POSTSCRIPT_2:
    return (char*) AEEBT_BPP_DOC_TYPE_POSTSCRIPT_2;

  case AEEBT_BPP_DOC_FORMAT_POSTSCRIPT_3:
    return (char*) AEEBT_BPP_DOC_TYPE_POSTSCRIPT_3;

  case AEEBT_BPP_DOC_FORMAT_PCL5E:
    return (char*) AEEBT_BPP_DOC_TYPE_PCL5E;

  case AEEBT_BPP_DOC_FORMAT_PCL3C:
    return (char*) AEEBT_BPP_DOC_TYPE_PCL3C;

  case AEEBT_BPP_DOC_FORMAT_PDF:
    return (char*) AEEBT_BPP_DOC_TYPE_PDF;
  
  case AEEBT_BPP_DOC_FORMAT_JPEG:
    return (char*) AEEBT_BPP_DOC_TYPE_JPEG;

  case AEEBT_BPP_DOC_FORMAT_GIF89A:
    return (char*) AEEBT_BPP_DOC_TYPE_GIF89A;

  default:
    MSG_ERROR( "SendFile: wrong document type=%x",  docType, 0, 0 );
    return NULL;
  }
}

static void OEMBTExtBPP_SendFileCb( OEMBTExtBPP_EvCb* pEvCb )
{
  OEMBTExtBPPobj_t* pMe     = OEMBTExtBPP_FindMe( pEvCb->appId );
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
    MSG_LOW( "SendFile: completed with status - ", pEvCb->status, 0, 0 );

    if ( pMe->pFile )
    { 
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "BPP Send file CB - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      // End of file transfer
      pN->uID         = AEEBT_BPP_EVT_SEND_FILE_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pEvCb->status );
      
      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BPP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }

    // Free eventCb struct
    pEvCb->bInUse = FALSE;
    return;
  }
  
  bytesRead = BPP_DATA_BUFFER_SIZE;
  bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
  pMe->bytesSent += bytesRead;
  if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
  {
    final  = 1;
  }

  pEvCb->bInUse = FALSE;

  pMe->data.AEEBTBPPdataLen = bytesRead;
  pMe->data.AEEBTBPPdata    = (byte*) pMe->pDataBuffer;

  bt_cmd_pf_bpp_sndr_send_file( pMe->appId, 
                                pMe->clientConnID,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                (bt_pf_byte_seq_type*) &pMe->data,
                                final );
}

static void OEMBTExtBPP_SOAPRequestCb( OEMBTExtBPP_EvCb* pEvCb )
{
  OEMBTExtBPPobj_t* pMe     = OEMBTExtBPP_FindMe( pEvCb->appId );
  uint16             connID = 0;
    
  AEEBTNotification*  pN = NULL;

  // Loop counter for copying data to user supplied buffer
  int i = 0;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId &&
       pMe->clientSTSConnID != pEvCb->connId )
  {
    MSG_ERROR( "SOAPRequestCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }

  // Copy data to user buffer
  for ( i=0; i<pEvCb->replyLen; i++ ) 
  {
    // Check for overflow condition
    if ( pMe->SOAPbytesRcvd < pMe->userBufferMaxSize - 1 )
    {
      pMe->pUserBuffer[ pMe->SOAPbytesRcvd++ ] = pEvCb->pReply[ i ];
    }
    else
    {
      // Ran out of buffer space, don't copy anymore
    }
  }

  // Choose the right connID
  if ( pMe->target == AEEBT_BPP_TARGET_DPS ) 
  {
    connID = pMe->clientConnID;
  }
  else
  {
    connID = pMe->clientSTSConnID;
  }

  (void) bt_cmd_pf_bpp_sndr_get_rsp_cfm( pMe->appId, 
                                         connID );


  // Null terminate the string
  pMe->pUserBuffer[ pMe->SOAPbytesRcvd ] = '\0';
  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "BPPEvCb - Can't get free not.",
             0, 0, 0 );
  } 
  else 
  {
    pN->data.uError = OEMBTExt_MapCmdStatus( pEvCb->status );

    if (pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) 
    {
      MSG_HIGH("final OK response for get",0, 0, 0 );
      // If this is the last set of data, notify App that we are done
      // No more Get to be reissued
      pN->uID              = AEEBT_BPP_EVT_SOAP_REQUEST_DONE;
    }
    else 
    {
      MSG_HIGH("CONTINUE response for get",0, 0, 0 );
      /* on a continue with end of body notify above */
      if (pEvCb->final == 1) {
        MSG_HIGH("CONTINUE response with end of body: reporting up",0, 0, 0 );
        pN->data.uError = AEEBT_ERR_NONE;
        // notify App abt the soap response that gives the printing status
        // and allow the application to send empty GetEvent req following 
        // this response so that it can be in the position to know what is
        // happening with the printer
        pN->uID              = AEEBT_BPP_EVT_SOAP_REQ_DONE_CONTINUE;
      }
      else
      {
        MSG_HIGH("CONTINUE response without end of body: get again",0, 0, 0 );
        /* end of body is yet to arrive so fetch more data */
        pMe->SOAPdata.AEEBTBPPdata    = (byte*) pMe->sndrSOAPmsg;
        pMe->SOAPdata.AEEBTBPPdataLen = STRLEN(pMe->sndrSOAPmsg);
        pMe->SOAPbytesSent           += pMe->SOAPdata.AEEBTBPPdataLen;
        bt_cmd_pf_bpp_sndr_soap_request( pMe->appId, 
                                         connID,
                                         (bt_pf_byte_seq_type*) &pMe->SOAPdata,
                                         1 );
        // Clear sender SOAP message buffer
        pMe->sndrSOAPmsg[0] = '\0';
        pEvCb->bInUse = FALSE;
        /* No notification needed to go up in this case, free the resource */
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
    }

  MSG_HIGH("reporting up",0, 0, 0 );
  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BPP );
  IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
  }
  pEvCb->bInUse = FALSE;
}

static void OEMBTExtBPP_GetRefObjCb( OEMBTExtBPP_EvCb* pEvCb )
{
  OEMBTExtBPPobj_t* pMe     = OEMBTExtBPP_FindMe( pEvCb->appId );
  uint32 bytesRead          = 0;
  bt_cmd_status_type status = BT_CS_GN_SUCCESS;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->serverConnID != pEvCb->connId )
  {
    MSG_ERROR( "GetRefObjCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else if ((pMe->pFile == NULL) && (pMe->bytesSent >= pMe->fileInfo.dwSize)) 
  {
      /* All the bytes in the file is sent.
      File is currently closed , send Empty end of body header to indicate
      complete object being sent.*/
     MSG_ERROR( "GetRefObjCb: Request for more than the file size bytes bytesSent=%x:fileSize=%x", 
                                                        pMe->bytesSent, pMe->fileInfo.dwSize, 0 );
      pMe->data.AEEBTBPPdataLen = 0;
      pMe->data.AEEBTBPPdata    = NULL;
  }
  else
  {
    if ((pMe->count < BPP_DATA_BUFFER_SIZE) && (pMe->count > -1)) 
    {
      bytesRead = pMe->count;
    }
    else
    {
      bytesRead = BPP_DATA_BUFFER_SIZE;
    }

    if ( pMe->offset > 0 && pMe->offset < pMe->fileInfo.dwSize ) 
    {
      IFILE_Seek( pMe->pFile, _SEEK_START, pMe->offset );
    }

    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    pMe->bytesSent += bytesRead;

    if ( pMe->bytesSent >= pMe->fileInfo.dwSize )
    {
      status = BT_CS_GN_SUCCESS;
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }

    pMe->data.AEEBTBPPdataLen = bytesRead;
    pMe->data.AEEBTBPPdata    = (byte*) pMe->pDataBuffer;
  }

  pEvCb->bInUse = FALSE;

  bt_cmd_pf_bpp_sndr_get_ref_obj_response( pMe->appId, 
                                           pMe->serverConnID,
                                           (bt_pf_byte_seq_type*) &pMe->data,
                                           NULL,
                                           status ); 
}

// called from BT task context; so just queue the event
static void OEMBTExtBPP_EventCallback( bt_ev_msg_type* ev_msg_ptr )
{
  OEMBTExtBPPobj_t*  pMe   = OEMBTExtBPP_FindMe( ev_msg_ptr->ev_hdr.bt_app_id );
  OEMBTExtBPP_EvCb*  pEvCb = NULL;
  AEEBTNotification* pN    = NULL;

  if ( pMe == NULL )
  {
    return; // probably for an app that no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "BPPEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }

  pN->data.uError  = AEEBT_BPP_ERR_NONE;

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
    {
      if ( ! OEMBTExtBPP_HandleCmdDone( &ev_msg_ptr->ev_msg.ev_gn_cmd_done,
                                        pN, pMe ) )
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return; // no need to send notification to app
      }
      break;
    }
    case BT_EV_PF_BPP_SNDR_AUTH_REQ:
    {
      MSG_LOW( "BPPEvCb: Sndr auth req, t=%x bu=%x", 
               ev_msg_ptr->ev_msg.ev_bpp_sndr_auth_req.target, 
               ev_msg_ptr->ev_msg.ev_bpp_sndr_auth_req.user_id_required, 0 );

      if ( ev_msg_ptr->ev_msg.ev_bpp_sndr_auth_req.target == 
          (AEEBTBPPTargetType) AEEBT_BPP_TARGET_DPS )
      {
        // Job channel
        pN->data.BppObject.target = AEEBT_BPP_TARGET_DPS;
      }
      else
      {
        // Assuming Status channel
        pN->data.BppObject.target = AEEBT_BPP_TARGET_STS;
      }
      pN->data.BppObject.bUserIDRequired = 
        ev_msg_ptr->ev_msg.ev_bpp_sndr_auth_req.user_id_required;
      pN->uID = AEEBT_BPP_EVT_AUTH_REQ;

      break;
    }
    case BT_EV_PF_BPP_SNDR_CON_CFM:
    {
      MSG_LOW( "BPPEvCb: Sndr con cfm, id=%x stat=%x", 
               ev_msg_ptr->ev_msg.ev_bpp_sndr_con_cfm.conn_id,
               ev_msg_ptr->ev_msg.ev_bpp_sndr_con_cfm.status, 0 );
      if (ev_msg_ptr->ev_msg.ev_bpp_sndr_con_cfm.status == BT_CS_GN_SUCCESS )
      {
        pN->uID = AEEBT_BPP_EVT_CONNECTED;
      }
      else
      {
        pN->uID = AEEBT_BPP_EVT_CONN_FAILED;
      }
      if (ev_msg_ptr->ev_msg.ev_bpp_sndr_con_cfm.target == 
          (AEEBTBPPTargetType) AEEBT_BPP_TARGET_DPS )
      {
        // Job channel
        pMe->clientConnID = ev_msg_ptr->ev_msg.ev_bpp_sndr_con_cfm.conn_id;
      }
      else
      {
        // Assuming that this is Status channel
        pMe->clientSTSConnID = ev_msg_ptr->ev_msg.ev_bpp_sndr_con_cfm.conn_id;
      }

      pN->data.BppObject.target = 
        (AEEBTBPPTargetType) ev_msg_ptr->ev_msg.ev_bpp_sndr_con_cfm.target;

      break;
    }
    case BT_EV_PF_BPP_SNDR_DCN_IND:
    {
      if ( pMe->clientConnID != 
           ev_msg_ptr->ev_msg.ev_bpp_sndr_dcn_ind.conn_id &&
           pMe->clientSTSConnID != 
           ev_msg_ptr->ev_msg.ev_bpp_sndr_dcn_ind.conn_id )
      {
        MSG_ERROR( "BPPEvCb: Sndr dcn cfm, wrong conn id=%x", 
                ev_msg_ptr->ev_msg.ev_bpp_sndr_dcn_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BPPEvCb: Sndr dcn cfm, id=%x",  
               ev_msg_ptr->ev_msg.ev_bpp_sndr_dcn_ind.conn_id, 0, 0 );

      if ( ev_msg_ptr->ev_msg.ev_bpp_sndr_dcn_ind.conn_id == 
           pMe->clientConnID ) 
      {
        // Job channel
        pN->data.BppObject.target = AEEBT_BPP_TARGET_DPS;
      }
      else
      {
        // Assuming Status channel
        pN->data.BppObject.target = AEEBT_BPP_TARGET_STS;
      }
      pN->uID = AEEBT_BPP_EVT_DISCONNECTED;

      break;
    }
    case BT_EV_PF_BPP_SNDR_CON_IND:
    {
      MSG_LOW( "BPPEvCb: Sndr con ind, id=%x", 
               ev_msg_ptr->ev_msg.ev_bpp_sndr_con_ind.conn_id, 0, 0 );

      pMe->serverConnID = ev_msg_ptr->ev_msg.ev_bpp_sndr_con_ind.conn_id;
      
      pN->uID         = AEEBT_BPP_EVT_CONN_REQ;
      pN->data.bdAddr = *((AEEBTBDAddr*)&ev_msg_ptr->ev_msg.ev_bpp_sndr_con_ind.bd_addr);  
      break;
    }
    case BT_EV_PF_BPP_SNDR_SRV_DCN_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bpp_sndr_srv_dcn_ind.conn_id )
      {
        MSG_ERROR( "BPPEvCb: Sndr Srv dcn ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bpp_sndr_srv_dcn_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      pN->uID = AEEBT_BPP_EVT_SEC_DISCONNECTED;
      pMe->serverConnID = BT_PF_BPP_NO_CONN_ID;

      break;
    }
    case BT_EV_PF_BPP_SNDR_SEND_FILE_CFM:
    {
      if ( pMe->clientConnID != ev_msg_ptr->ev_msg.ev_bpp_sndr_send_file_cfm.conn_id )
      {
        MSG_ERROR( "BPPEvCb: Sndr send file cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bpp_sndr_send_file_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BPPEvCb: Sndr send file cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bpp_sndr_send_file_cfm.status, 
               ev_msg_ptr->ev_msg.ev_bpp_sndr_send_file_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bpp_sndr_send_file_cfm.conn_id;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bpp_sndr_send_file_cfm.status;
        CALLBACK_Init( &pMe->cb, OEMBTExtBPP_SendFileCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BPP_SNDR_GET_CFM:
    {
      if ( pMe->clientConnID != 
           ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.conn_id && 
           pMe->clientSTSConnID != 
           ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.conn_id )
      {
        MSG_ERROR( "BPPEvCb: Sndr soap req cfm, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BPPEvCb: Sndr soap req cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.status, 
               ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.conn_id, 0 );

      pEvCb = OEMBTExtBPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.conn_id;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.status;

        pEvCb->jobID    = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.job_id;
        pEvCb->pReply   = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.reply_ptr;
        pEvCb->replyLen = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.reply_len;
        pEvCb->final    = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_cfm.final;

        CALLBACK_Init( &pMe->cb, OEMBTExtBPP_SOAPRequestCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_BPP_SNDR_GET_REF_OBJ_IND:
    {
      if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.conn_id )
      {
        MSG_ERROR( "BPPEvCb: Sndr ref obj ind, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "BPPEvCb: Sndr ref obj ind, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.status, 
               ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.conn_id, 0 );

      // Acknowledge the event first
      (void) bt_cmd_pf_bpp_sndr_srv_req_cfm( pMe->appId, 
                                             pMe->serverConnID,
                                             BT_EV_PF_BPP_SNDR_GET_REF_OBJ_IND, 
                                             BT_CS_GN_SUCCESS );

      if ( ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.name_len == 0 ) 
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      if ( ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.offset == 0 && 
           ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.name_len > 0 ) 
      {
        // First request
        pN->data.BppObject.offset = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.offset;
        pN->data.BppObject.count  = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.count;

        bt_pf_copy_unicode_str( 
          (uint16*)pN->data.BppObject.wRefObjName,
          (uint16*)ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.name_str,
          BT_PF_MAX_FILENAME_LEN,
          NULL );

        pMe->bGetFileSize = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.get_file_size;
        pN->uID = AEEBT_BPP_EVT_GET_REF_OBJ_REQ;

        break;
      }

      pEvCb = OEMBTExtBPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.conn_id;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.status;
        pMe->offset       = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.offset;
        pMe->count        = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.count;
        pMe->bGetFileSize = ev_msg_ptr->ev_msg.ev_bpp_sndr_get_ref_obj_ind.get_file_size;

        CALLBACK_Init( &pMe->cb, OEMBTExtBPP_GetRefObjCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    default:
    {
      MSG_ERROR( "BPPEvCb - unexpect event %x", 
                 ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_BPP );
  IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
}

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BPP)

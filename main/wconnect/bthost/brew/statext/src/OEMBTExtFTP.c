/*===========================================================================

FILE:      OEMBTExtFTP.c

SERVICES:  BlueTooth File Transfer Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth File Transfer Profile.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2004-2009 by QUALCOMM Incorporated. 
                All Rights Reserved.
            Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtFTP.c#3 $
$DateTime: 2009/04/17 11:28:04 $
$Author: ganeshs $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2009-11-02   gs  Updated to allow longer filenames in Put/Get
2008-10-07   pn  Replaced deprecated functions.

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_FTP)
#include "bt.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "oi_obexspec.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtFTP.h"

//==========================================================================
//   Macro definitions
//==========================================================================
#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
/* MAX payload size in OBEX Body/End-of-Body header 
  ( 16378 = 16K - 6 byte header ) */
#define FTP_BUFFER_SIZE     AEEBT_OBEX_BUFFER_SIZE + 5 
                            /* extra room of 5 bytes to fit rx'ed data in case
                               conn-id hdr is not sent with the payload */
#else
#define FTP_BUFFER_SIZE     32757
#endif /* FEATURE_BT_REDUCE_MEMORY_USAGE */
#else
#define FTP_BUFFER_SIZE     4266
#endif /* FEATURE_BT_SOC */
const AECHAR W_DIRECTORY_STR[] = { '/', 0 };

//==========================================================================
//   Type definitions
//==========================================================================

typedef struct OEMBTExtFTPobj_struct
{
  ACONTEXT*                   pac;
  IBTExtNotifier*             pNotifier;

  bt_app_id_type              appId;
  AEEBTBDAddr                 BDAddr;         // remote device's

  AECHAR                      wRootFolder[ AEEBT_MAX_FILE_NAME + 1];
  AECHAR                      wCurrentFolder[ (AEEBT_MAX_FILE_NAME * 2) + 1];
  AECHAR                      wName[ AEEBT_MAX_FILE_NAME + 1 ];

  AEEBTFTPAuthentication      auth;
  boolean                     bReadOnly;
  boolean                     bBrowseAllowed;

  bt_pf_ftp_srv_conn_id_type  serverConnID;
  bt_pf_ftp_cli_conn_id_type  clientConnID;

  boolean                     bAcceptConn;

  uint32                      bytesSent;
  uint32                      bytesRcvd;
  uint8*                      pBuffer;
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
  bt_cmd_status_type          prevWriteStatus;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */
  boolean                     bIsServer;
  boolean                     bDoDeregister;    // for server only
  AEEBTFTPRequestType         currReq;
  IFileMgr*                   pFileMgr;
  IFile*                      pFile;
  uint32                      dwFileSize;
  boolean                     bBrowse;

  struct OEMBTExtFTPobj_struct* pNextFTP;  // linked into mgr
} OEMBTExtFTPobj_t;

typedef struct
{
  boolean                     bInUse;
  bt_event_type               event;
  bt_app_id_type              appId;
  uint16                      connId;
  bt_pf_ftp_handle_type       handle;
  uint32                      maxBytes;
  uint8*                      pSrc;
  bt_pf_ftp_mode_type         mode;
  AECHAR                      wName[ AEEBT_MAX_FILE_NAME + 1 ];
  uint8                       nameLen;
  AEECallback                 cb;
  bt_cmd_status_type          status;
} OEMBTExtFTP_EvCb;

// one of these
typedef struct
{
  OEMBTExtFTPobj_t* pNextFTP;    // singly linked list of FTPs
  OEMBTExtFTP_EvCb  FreeEvCb[ 5 ];
} OEMBTExtFTP_Mgr;

//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static void OEMBTExtFTP_Cleanup( OEMBTExtFTPobj_t* pMe );
static void OEMBTExtFTP_SplitPath( const AECHAR* pwSrc, 
                                   AECHAR* pwFile, uint8 uFileBufLen,
                                   AECHAR* pwPath, uint8 uPathBufLen );
static int  OEMBTExtFTP_CheckCmdStatus( bt_cmd_status_type stat );
static void OEMBTExtFTP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);
static OEMBTExtFTP_EvCb* OEMBTExtFTP_GetFreeEvCb( void );

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtFTP_Mgr        gMgr;               // manager object
static rex_crit_sect_type     FTPCritSect;

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtFTP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtFTP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtFTP_Init(IBTExtFTP* pParent)
{
  OEMBTExtFTPobj_t* pMe;

  // allocate pMe
  pMe = MALLOCREC(OEMBTExtFTPobj_t);
  if ( pMe == NULL )
  {
    return ENOMEMORY;
  }

  if ( ISHELL_CreateInstance( pParent->m_pIShell, AEECLSID_BLUETOOTH_NOTIFIER, 
                              (void**) &pMe->pNotifier ) != SUCCESS )
  {
    FREE( pMe );
    return EFAILED;
  }

  if ( ISHELL_CreateInstance( pParent->m_pIShell, AEECLSID_FILEMGR, 
                              (void **)&pMe->pFileMgr ) != SUCCESS )
  {
    OEMBTExtFTP_Cleanup( pMe );
    return EFAILED;
  }

  // init BT layer
  pMe->appId = bt_cmd_ec_get_app_id_and_register(OEMBTExtFTP_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    OEMBTExtFTP_Cleanup( pMe );
    return EFAILED;
  }

  pMe->pBuffer = MALLOC( FTP_BUFFER_SIZE );
  if ( pMe->pBuffer == NULL )
  {
    OEMBTExtFTP_Cleanup( pMe );
    return EFAILED;
  }
  
  pMe->pac = AEE_GetAppContext();

  if ( AEEHandle_To( &gOEMBTExtHandleList,
                     (OEMINSTANCE*)pMe,
                     &pParent->m_hBT ) == FALSE )
  {
    OEMBTExtFTP_Cleanup( pMe );
    return EFAILED;
  }

  if ( gMgr.pNextFTP == NULL ) // first time?
  {
    uint8 i;
    for ( i = 0; i < (sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++ )
    {
      gMgr.FreeEvCb[ i ].bInUse = FALSE;
    }
  }

  // init me
  pMe->clientConnID = BT_PF_FTP_NO_CONN_ID;
  pMe->serverConnID = BT_PF_FTP_NO_CONN_ID;
  /* No server registered yet */
  pMe->bIsServer    = FALSE;


#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
  pMe->prevWriteStatus = BT_CS_GN_SUCCESS;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */

  // link into mgr
  pMe->pNextFTP = gMgr.pNextFTP;
  gMgr.pNextFTP = pMe;

  return SUCCESS;
}

// IBase (see AEE.h)
uint32 OEMBTExtFTP_Release( IBTExtFTP* pParent )
{
  OEMBTExtFTPobj_t*  pMe;
  OEMBTExtFTPobj_t** ppc;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) == TRUE )
  {
    // unlink from mgr
    for (ppc = &gMgr.pNextFTP; *ppc != NULL; ppc = &(*ppc)->pNextFTP)
    {
      if ((*ppc)->appId == pMe->appId)
      {
        *ppc = pMe->pNextFTP;
        pMe->pNextFTP = NULL;
        break;
      }
    }

    pMe->pac = NULL;
    OEMBTExtFTP_Cleanup( pMe );
    (void) AEEHandle_Clear( &gOEMBTExtHandleList, pParent->m_hBT );
    return SUCCESS;
  }

  return EFAILED;
}

// IBTEXTFTP_Register (see AEEBTExtFTP.h)
int OEMBTExtFTP_Register( IBTExtFTP*                  pParent, 
                          const AEEBTFTPRegistration* pReg )
{
  bt_cmd_status_type stat;
  OEMBTExtFTPobj_t*  pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pReg == NULL )
  {
    return EBADPARM;
  }

  pMe->auth           = pReg->auth;
  pMe->bReadOnly      = pReg->bReadOnly;
  pMe->bBrowseAllowed = pReg->bBrowseAllowed;

  stat = bt_cmd_pf_ftp_srv_register( pMe->appId, 
                                     (bt_pf_goep_srv_auth_type)pMe->auth,
                                     pMe->bReadOnly,
                                     pMe->bBrowseAllowed,
                                     (char*)pReg->szServiceName );
  return OEMBTExtFTP_CheckCmdStatus( stat );
}


// IBTEXTFTP_Deregister (see AEEBTExtFTP.h)
int OEMBTExtFTP_Deregister( IBTExtFTP* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  /* check if the server is registered */
  if (pMe->bIsServer != TRUE) {
    return EFAILED;
  }

  if ( pMe->serverConnID != BT_PF_FTP_NO_CONN_ID )
  {
    stat = bt_cmd_pf_ftp_srv_force_disconnect( pMe->appId, 
                                               pMe->serverConnID );
    if ( stat == BT_CS_GN_PENDING )
    {
      pMe->bDoDeregister = TRUE;
    }
  }
  else
  {
    stat = bt_cmd_pf_ftp_srv_deregister( pMe->appId );
  }
  return OEMBTExtFTP_CheckCmdStatus( stat );
}


// IBTEXTFTP_AcceptConnection (see AEEBTExtFTP.h)
int OEMBTExtFTP_AcceptConnection( 
  IBTExtFTP*          pParent, 
  const AEEBTBDAddr*  pBDAddr, 
  boolean             bAccept,
  const AECHAR*       pwRootFolder
)
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  
  if ( (pBDAddr == NULL) || 
       (AEEBT_BD_ADDRS_EQUAL( pBDAddr, &pMe->BDAddr ) == FALSE) ||
       (pwRootFolder == NULL) )
  {
    return EBADPARM;
  }

  pMe->bAcceptConn = bAccept;

  WSTRLCPY( pMe->wRootFolder, pwRootFolder, ARR_SIZE( pMe->wRootFolder ) );
  WSTRLCPY( pMe->wCurrentFolder, pwRootFolder, ARR_SIZE( pMe->wCurrentFolder ) );
  stat = bt_cmd_pf_ftp_srv_accept_connect( pMe->appId, pMe->serverConnID,
                                           bAccept, pMe->wRootFolder );
  return OEMBTExtFTP_CheckCmdStatus( stat );
}

// IBTEXTFTP_SendResult (see AEEBTExtFTP.h)
int OEMBTExtFTP_SendResult(
  IBTExtFTP*          pParent, 
  AEEBTFTPRequestType reqType, 
  AEEBTFTPStatus      result,
  const AECHAR*       pwName
)
{
  bt_cmd_status_type  stat = BT_CS_GN_SUCCESS;
  OEMBTExtFTPobj_t*   pMe;
  ACONTEXT*           pCurAC;
  IFile*              pFile = NULL;
  AEEFileInfoEx       fileInfoEx;
  boolean             bFileFound;
  char                szName[ AEEBT_MAX_FILE_NAME + 1 ];

  fileInfoEx.nStructSize = sizeof( AEEFileInfoEx );
  fileInfoEx.pszFile = NULL;
  fileInfoEx.nMaxFile = 0;
  fileInfoEx.pszDescription = NULL;
  fileInfoEx.nDescriptionSize = 0;
  fileInfoEx.pClasses = NULL;
  fileInfoEx.nClassesSize = 0;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pwName == NULL )
  {
    return EBADPARM;
  }

  switch ( result )
  {
    case AEEBT_FTP_STAT_SUCCESS:
      stat = BT_CS_GN_SUCCESS;
      break;
    case AEEBT_FTP_STAT_NOT_ALLOWED:
      stat = BT_CS_PF_OBEX_ACCESS_DENIED;
      break;
    case AEEBT_FTP_STAT_FILESYS_FULL:
      stat = BT_CS_PF_OBEX_SERVICE_UNAVAILABLE;
      break;
    case AEEBT_FTP_STAT_EXISTS:
      stat = BT_CS_PF_OBEX_ACCESS_DENIED;
      break;
    case AEEBT_FTP_STAT_NOT_FOUND:
      stat = BT_CS_PF_OBEX_NOT_FOUND;
      break;
    case AEEBT_FTP_STAT_BAD_REQ:
      stat = BT_CS_PF_OBEX_BAD_REQUEST;
      break;
    default:
      return EBADPARM;
  }

  switch ( reqType)
  {
    case AEEBT_FTP_REQ_CD:   // set path
      if ( stat == BT_CS_GN_SUCCESS )
      {
        if ( WSTRLEN( pMe->wCurrentFolder ) > 0 )
        {
          if ( WSTRICMP( pMe->wCurrentFolder, pwName ) != 0 )
          {
            WSTRLCPY( pMe->wCurrentFolder, pwName, 
                      ARR_SIZE( pMe->wCurrentFolder ) );
          }
        }
        else
        {
          WSTRLCPY( pMe->wCurrentFolder, pwName, ARR_SIZE( pMe->wCurrentFolder ) );
        }
      }
      if ( pMe->bIsServer == FALSE)
      {
        bt_cmd_pf_ftp_cli_set_folder_done( pMe->appId, pMe->clientConnID, 
                                           stat );
      }
      else
      {
        bt_cmd_pf_ftp_srv_set_folder_done( pMe->appId, pMe->serverConnID, 
                                           stat );
      }
      break;
    case AEEBT_FTP_REQ_LS:   // list folder
      if ( result == AEEBT_FTP_STAT_SUCCESS )
      {
        AEEBT_FROM_WSTR( pwName, szName, sizeof(szName) );
        pCurAC = ENTER_APP_CONTEXT( pMe->pac );
        bFileFound = TRUE;
        if ( (IFILEMGR_Test( pMe->pFileMgr, szName ) != SUCCESS) ||
             (IFILEMGR_GetInfoEx( pMe->pFileMgr, szName, 
                                  &fileInfoEx ) != SUCCESS) ||
             ((pFile = IFILEMGR_OpenFile( pMe->pFileMgr, szName, 
                                          _OFM_READWRITE )) == NULL) )
        {
          bFileFound = FALSE;
        }
        (void) LEAVE_APP_CONTEXT( pCurAC );

        if ( bFileFound == FALSE )
        {
          return EBADPARM;
        }
        else
        {
           pMe->bBrowse = TRUE;
           pMe->dwFileSize = 0;
           pMe->bytesSent = 0;
           pMe->bytesRcvd = 0;
        }
      }
      if ( pMe->bIsServer == FALSE )
      {
        bt_cmd_pf_ftp_cli_browse_done( pMe->appId, pMe->clientConnID, 
                                       (bt_pf_ftp_handle_type) pFile, 
                                       fileInfoEx.dwSize, stat );
      }
      else
      {
        bt_cmd_pf_ftp_srv_browse_done( pMe->appId, pMe->serverConnID, 
                                       (bt_pf_ftp_handle_type) pFile, 
                                       fileInfoEx.dwSize, stat );
      }
      break;
    case AEEBT_FTP_REQ_PUT:
    case AEEBT_FTP_REQ_GET:
      if ( pMe->bIsServer == FALSE )
      {
        bt_cmd_pf_ftp_cli_open_done( pMe->appId, pMe->clientConnID,
                                     (bt_pf_ftp_handle_type) NULL,
                                     0, stat );
      }
      else
      {
        bt_cmd_pf_ftp_srv_open_done( pMe->appId, pMe->serverConnID,
                                     (bt_pf_ftp_handle_type) NULL,
                                     0, stat );
      }
      break;
    case AEEBT_FTP_REQ_DEL:
      bt_cmd_pf_ftp_srv_delete_done( pMe->appId, pMe->serverConnID, stat );
      break;
  }
  return SUCCESS;
}

// IBTEXTFTP_Connect (see AEEBTExtFTP.h)
int OEMBTExtFTP_Connect( 
  IBTExtFTP*          pParent, 
  const AEEBTBDAddr*  pBDAddr, 
  uint8               uChannelNumber
)
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;  
  bt_pf_realm_info_type realmInfo;
  boolean userId = TRUE;
  boolean accessControl = TRUE;  

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }
  pMe->BDAddr = *pBDAddr;
  
  /* Call the API to initiate the Client AUTH data 
   */  
  MSG_LOW("Calling bt_obex_client_authenticate. pMe->auth = %d",
           pMe->auth, 0, 0 );
  if(pMe->auth)
  {
      MSG_LOW("Call to bt_obex_client_authenticate",
               0, 0, 0 );
      realmInfo.realm_len = 0;
      memset(realmInfo.realm_str, 0 , BT_PF_OBEX_MAX_REALM_LEN);
      stat = bt_cmd_pf_goep_cli_authenticate((bt_bd_addr_type*)pBDAddr,
                                uChannelNumber,
                                &realmInfo,
                                userId,
                                accessControl);
      MSG_LOW("bt_obex_client_authenticate stat = %d",
                 stat, 0, 0 );
      if(stat != BT_CS_GN_SUCCESS)
      {
          return(EFAILED);
      }

  }  
  stat = bt_cmd_pf_ftp_cli_connect( pMe->appId, (bt_bd_addr_type*)pBDAddr,
                                    uChannelNumber );
  return OEMBTExtFTP_CheckCmdStatus( stat );
}


// IBTEXTFTP_Abort (see AEEBTExtFTP.h)
int OEMBTExtFTP_Abort( IBTExtFTP *pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_ftp_cli_abort( pMe->appId, pMe->clientConnID );
  return OEMBTExtFTP_CheckCmdStatus( stat );
}

// IBTEXTFTP_SetPath (see AEEBTExtFTP.h)
int OEMBTExtFTP_SetPath( 
  IBTExtFTP*          pParent,
  const AECHAR*       pwPathName,
  AEEBTFTPSetPathFlag flag
)
{
  bt_cmd_status_type          stat;
  OEMBTExtFTPobj_t*           pMe;
  bt_pf_ftp_cli_set_path_type set_path_ctrl;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( (pwPathName == NULL) && (flag == AEEBT_FTP_SET_PATH_TO_FOLDER) )
  {
    return EBADPARM;
  }
  switch ( flag )
  {
    case AEEBT_FTP_SET_PATH_TO_FOLDER:
      set_path_ctrl = BT_PF_FTP_CLI_SET_PATH_TO_FOLDER;
      break;
    case AEEBT_FTP_SET_PATH_TO_PARENT:
      set_path_ctrl = BT_PF_FTP_CLI_SET_PATH_TO_PARENT;
      break;
    case AEEBT_FTP_SET_PATH_TO_ROOT:
      set_path_ctrl = BT_PF_FTP_CLI_SET_PATH_TO_ROOT;
      break;
    default:
      return EBADPARM;
  }
  stat = bt_cmd_pf_ftp_cli_set_path( pMe->appId, pMe->clientConnID,
                                     (AECHAR*)pwPathName, set_path_ctrl );
  return OEMBTExtFTP_CheckCmdStatus( stat );
}

// IBTEXTFTP_CreateFolder (see AEEBTExtFTP.h)
int OEMBTExtFTP_CreateFolder( IBTExtFTP *pParent, const AECHAR *pwDirName )
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pMe->clientConnID == BT_PF_FTP_NO_CONN_ID )
  {
    return EBADSTATE;
  }
  if ( pwDirName == NULL )
  {
    return EBADPARM;
  }
  stat = bt_cmd_pf_ftp_cli_new_subfolder( pMe->appId, pMe->clientConnID, 
                                          (AECHAR*)pwDirName );
  return OEMBTExtFTP_CheckCmdStatus( stat );
}

// IBTEXTFTP_ListFolder (see AEEBTExtFTP.h)
int OEMBTExtFTP_ListFolder( 
  IBTExtFTP*    pParent, 
  const AECHAR* pwDirName, 
  const AECHAR* pwDirList
)
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;
  char                szBuf[ AEEBT_MAX_FILE_NAME + 1 ];

  MSG_FATAL("***zzg OEMBTExtFTP_ListFolder***", 0, 0, 0);

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pMe->clientConnID == BT_PF_FTP_NO_CONN_ID )
  {
    return EBADSTATE;
  }
  if ( pwDirList == NULL )
  {
    return EBADPARM;
  }
  
  //AEEBT_FROM_WSTR( pwDirList, szBuf, sizeof(szBuf) );
  WSTRTOSTR(pwDirList, szBuf, sizeof(szBuf));	//Modify by zzg 2011_12_28
  
  DBGPRINTF("***zzg OEMBTExtFTP_ListFolder pwDirList=%s***", pwDirList);
  DBGPRINTF("***zzg OEMBTExtFTP_ListFolder szBuf=%s***", szBuf);
  
  if ( ((pMe->pFile = IFILEMGR_OpenFile( pMe->pFileMgr, szBuf, 
                                         _OFM_READWRITE )) == NULL) && 
       ((pMe->pFile = IFILEMGR_OpenFile( pMe->pFileMgr, szBuf, 
                                         _OFM_CREATE )) == NULL))
  {
  	DBGPRINTF("***zzg IFILEMGR_OpenFile EFILEOPEN***");
    return EFILEOPEN;
  }
 
  pMe->bBrowse = TRUE;
  pMe->dwFileSize = 0;
  pMe->bytesSent = 0;
  pMe->bytesRcvd = 0;
  stat = bt_cmd_pf_ftp_cli_get_folder_listing( pMe->appId, pMe->clientConnID, 
                                               (AECHAR*)pwDirName );

  DBGPRINTF("***zzg bt_cmd_pf_ftp_cli_get_folder_listing stat=%x***", stat);
  
  return OEMBTExtFTP_CheckCmdStatus( stat );
}

// IBTEXTFTP_Put (see AEEBTExtFTP.h)
int OEMBTExtFTP_Put( 
  IBTExtFTP*     pParent,
  AEEBTFTPObject objType,
  const AECHAR*  pwLocalName,
  const AECHAR*  pwRemoteName
)
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;
  ACONTEXT*           pCurAC;
  boolean             bFileFound;
  IFile*              pFile = NULL;
  AEEFileInfoEx       fileInfoEx;
  char                szFileName[ 2*AEEBT_MAX_FILE_NAME + 1 ];

  fileInfoEx.nStructSize = sizeof( AEEFileInfoEx );
  fileInfoEx.pszFile = NULL;
  fileInfoEx.nMaxFile = 0;
  fileInfoEx.pszDescription = NULL;
  fileInfoEx.nDescriptionSize = 0;
  fileInfoEx.pClasses = 0;
  fileInfoEx.nClassesSize = 0;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pwLocalName == NULL )
  {
    return EBADPARM;
  }

  if ( (pMe->bIsServer != FALSE) && (pwRemoteName == NULL) )
  {
    return EBADPARM;
  }

  pCurAC = ENTER_APP_CONTEXT( pMe->pac );
  bFileFound = FALSE;
  AEEBT_FROM_WSTR( pwLocalName, szFileName, sizeof(szFileName) );
  if ( IFILEMGR_Test( pMe->pFileMgr, szFileName ) == SUCCESS )
  {
    bFileFound = TRUE;
  }
  (void) LEAVE_APP_CONTEXT( pCurAC );

  if ( bFileFound == FALSE )
  {
    return EBADPARM;
  }

  pMe->bytesSent = 0;
  pMe->bytesRcvd = 0;

  if ( pMe->bIsServer == FALSE )
  {
    // client calls this function to copy file/folder to server
    if ( pMe->clientConnID == BT_PF_FTP_NO_CONN_ID )
    {
      return EBADSTATE;
    }
    pMe->dwFileSize = 0;
    pMe->bBrowse = FALSE;


    TASKLOCK();
    if ( objType == AEEBT_FTP_FILE )
    {
      // store file name and path separately
      OEMBTExtFTP_SplitPath( 
        pwLocalName, 
        pMe->wName, ARR_SIZE(pMe->wName),
        pMe->wCurrentFolder, ARR_SIZE(pMe->wCurrentFolder) );

      stat = bt_cmd_pf_ftp_cli_put( pMe->appId, pMe->clientConnID, 
                                    (AECHAR*)pwLocalName, 
                                    (AECHAR*)pwRemoteName );
    }
    else
    {
      WSTRLCPY( pMe->wCurrentFolder, pwLocalName, 
                ARR_SIZE( pMe->wCurrentFolder ) );
      stat = bt_cmd_pf_ftp_cli_tree_put( pMe->appId, pMe->clientConnID, 
                                         (AECHAR*)pwLocalName, 
                                         (AECHAR*)pwRemoteName );
    }
    TASKFREE();
  }
  else
  {
    // Server calls this function in response to receiving the event 
    // AEEBT_FTP_EVT_GET_REQ to give file the client is trying to get.
    // So, open the file now

    pCurAC = ENTER_APP_CONTEXT( pMe->pac );
    if ( (IFILEMGR_GetInfoEx( pMe->pFileMgr, szFileName, 
                              &fileInfoEx ) != SUCCESS) ||
         ((pFile = IFILEMGR_OpenFile( pMe->pFileMgr, szFileName, 
                                      _OFM_READ )) == NULL) )
    {
      MSG_ERROR( "FTP_Put - OpenFile failed pF=%x", pFile, 0, 0 );
      bFileFound = FALSE;
    }
    (void) LEAVE_APP_CONTEXT( pCurAC );

    if ( bFileFound == FALSE )
    {
      return EFILEOPEN;
    }
    else
    {
       pMe->dwFileSize = fileInfoEx.dwSize;
       pMe->bBrowse = FALSE;
    }

    TASKLOCK();
    stat = bt_cmd_pf_ftp_srv_open_done( pMe->appId, pMe->serverConnID, 
                                        (bt_pf_ftp_handle_type)pFile,
                                        fileInfoEx.dwSize, BT_CS_GN_SUCCESS );
    TASKFREE();

    if ( (stat != BT_CS_GN_PENDING) && (stat != BT_CS_GN_SUCCESS) )
    {
      IFILE_Release( pFile );
    }
    else
    {
      pMe->currReq = AEEBT_FTP_REQ_PUT;
    }
  }

  return OEMBTExtFTP_CheckCmdStatus( stat );
}


// IBTEXTFTP_Get (see AEEBTExtFTP.h)
int OEMBTExtFTP_Get(
  IBTExtFTP*     pParent,
  AEEBTFTPObject objType,
  const AECHAR*  pwLocalName,
  const AECHAR*  pwRemoteName
)
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;
  ACONTEXT*           pCurAC;
  boolean             bFileFound;
  IFile*              pFile = NULL;
  char                szFileName[ 2*AEEBT_MAX_FILE_NAME + 1 ];

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( (pwLocalName == NULL) ||
       ((pMe->bIsServer != FALSE) && (pwRemoteName == NULL)) )
  {
    return EBADPARM;
  }

  pCurAC = ENTER_APP_CONTEXT( pMe->pac );
  bFileFound = FALSE;
  AEEBT_FROM_WSTR( pwLocalName, szFileName, sizeof(szFileName) );
  if ( IFILEMGR_Test( pMe->pFileMgr, szFileName ) == SUCCESS )
  {
    bFileFound = TRUE;
  }
  (void) LEAVE_APP_CONTEXT( pCurAC );

  if ( bFileFound != FALSE )
  {
    return EBADPARM;
  }

  pMe->bytesRcvd = 0;
  pMe->bytesSent = 0;

  if ( pMe->bIsServer == FALSE )
  {
    pMe->dwFileSize = 0;
    pMe->bBrowse = FALSE;

    // client calls this function to copy file/folder from server
    TASKLOCK();
    if ( objType == AEEBT_FTP_FILE )
    {
      // store file name and path separately
      OEMBTExtFTP_SplitPath( 
        pwLocalName, 
        pMe->wName, ARR_SIZE(pMe->wName),
        pMe->wCurrentFolder, ARR_SIZE(pMe->wCurrentFolder) );

      stat = bt_cmd_pf_ftp_cli_get( pMe->appId, pMe->clientConnID,
                                    (AECHAR*)pwLocalName, 
                                    (AECHAR*)pwRemoteName );
    }
    else
    {
      WSTRLCPY( pMe->wCurrentFolder, pwLocalName,
                ARR_SIZE( pMe->wCurrentFolder ) );
      stat = bt_cmd_pf_ftp_cli_tree_get( pMe->appId, pMe->clientConnID,
                                         (AECHAR*)pwLocalName, 
                                         (AECHAR*)pwRemoteName );
    }
    TASKFREE();
  }
  else
  {
    // server calls this function in response to receiving event
    // AEEBT_FTP_EVT_PUT_REQ to get the file client is trying to put
    // So, open the file now
    pCurAC = ENTER_APP_CONTEXT( pMe->pac );
    pFile = IFILEMGR_OpenFile( pMe->pFileMgr, szFileName, _OFM_CREATE );
    (void) LEAVE_APP_CONTEXT( pCurAC );
    pMe->bBrowse = FALSE;

    if ( pFile == NULL )
    {
      MSG_ERROR( "FTP_Get - OpenFile failed", 0, 0, 0 );
      return EFILEOPEN;
    }

    TASKLOCK();
    stat = bt_cmd_pf_ftp_srv_open_done(
      pMe->appId, pMe->serverConnID, 
      (bt_pf_ftp_handle_type) pFile, 0, BT_CS_GN_SUCCESS );
    TASKFREE();

    if ( (stat != BT_CS_GN_PENDING) && (stat != BT_CS_GN_SUCCESS) )
    {
      IFILE_Release( pFile );
      IFILEMGR_Remove( pMe->pFileMgr, szFileName );
    }
    else
    {
      pMe->currReq = AEEBT_FTP_REQ_GET;
    }
  }

  return OEMBTExtFTP_CheckCmdStatus( stat );
}

// IBTEXTFTP_Delete (see AEEBTExtFTP.h)
int OEMBTExtFTP_Delete( IBTExtFTP *pParent, const AECHAR* pwName )
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pMe->clientConnID == BT_PF_FTP_NO_CONN_ID )
  {
    return EBADSTATE;
  }
  if ( pwName == NULL )
  {
    return EBADPARM;
  }
  stat = bt_cmd_pf_ftp_cli_delete( pMe->appId, pMe->clientConnID, 
                                   (AECHAR*)pwName );
  return OEMBTExtFTP_CheckCmdStatus( stat );
}

// IBTEXTFTP_Authenticate (see AEEBTExtFTP.h)
int OEMBTExtFTP_Authenticate( 
  IBTExtFTP*         pParent,
  const AEEBTBDAddr* pBDAddr, 
  const AECHAR*      pwUserID,
  const AECHAR*      pwPassword
)
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;
  uint8 uLen = 0;
  char szUserID[ AEEBT_MAX_USERID_LEN ];
  char szPassword[ AEEBT_MAX_PASSWORD_LEN ];

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( (pBDAddr == NULL) || 
       (AEEBT_BD_ADDRS_EQUAL( pBDAddr, &pMe->BDAddr ) == FALSE) ||
       (pwPassword == NULL) )
  {
    return EBADPARM;
  }
  if ( pwUserID != NULL )
  {
    AEEBT_FROM_WSTR( pwUserID, szUserID, sizeof(szUserID) );
    uLen = STRLEN( szUserID );
  }
  AEEBT_FROM_WSTR( pwPassword, szPassword, sizeof(szPassword) );

  if ( pMe->bIsServer == FALSE )
  {
    stat = bt_cmd_pf_ftp_cli_auth_response( pMe->appId, pMe->clientConnID,
                                            (byte*)szUserID, uLen, szPassword );
  }
  else
  {
    stat = bt_cmd_pf_ftp_srv_auth_response( pMe->appId, pMe->serverConnID,
                                            szPassword, pMe->bReadOnly,
                                            pMe->bBrowseAllowed );
  }
  return OEMBTExtFTP_CheckCmdStatus( stat );
}

// IBTEXTFTP_Disconnect (see AEEBTExtFTP.h)
int OEMBTExtFTP_Disconnect( IBTExtFTP* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtFTPobj_t*   pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( pMe->bIsServer )
  {
    // force disconnect
    stat = bt_cmd_pf_ftp_srv_force_disconnect( pMe->appId, pMe->serverConnID );
  }
  else
  {
    stat = bt_cmd_pf_ftp_cli_disconnect( pMe->appId, pMe->clientConnID );
  }
  return OEMBTExtFTP_CheckCmdStatus( stat );
}

// IBTEXTFTP_GetProgressInfo (see AEEBTExtFTP.h)
int OEMBTExtFTP_GetProgressInfo( IBTExtFTP* pParent,
                                 AEEBTProgressInfo* pProgressInfo )
{
  OEMBTExtFTPobj_t *pMe;
  int ret = SUCCESS;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    ret = EFAILED;
  }
  else if ( pProgressInfo == NULL )  
  {
     ret = EBADPARM;
  }
  else
  {
    pProgressInfo->objSize  = pMe->dwFileSize;
    pProgressInfo->numBytes = ( pMe->bytesSent > 0 ) ?
                              pMe->bytesSent :
                              pMe->bytesRcvd;
  }
  return ret;
}

//==========================================================================
//   static helper functions
//==========================================================================
static void OEMBTExtFTP_Cleanup( OEMBTExtFTPobj_t* pMe )
{
  if ( pMe->pBuffer != NULL )
  {
    FREE( pMe->pBuffer );
    pMe->pBuffer = NULL;
  }
  if ( pMe->appId != BT_APP_ID_NULL )
  {
    (void)bt_cmd_ec_free_application_id( pMe->appId );
    pMe->appId = BT_APP_ID_NULL;
  }
  if ( pMe->pFileMgr != NULL )
  {
    IFILEMGR_Release( pMe->pFileMgr );
    pMe->pFileMgr = NULL;
  }
  if ( pMe->pNotifier != NULL )
  {
    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    pMe->pNotifier = NULL;
  }
  FREE( pMe );
}

static void OEMBTExtFTP_SplitPath( 
  const AECHAR* pwSrc, 
  AECHAR* pwFile, uint8 uFileBufLen, 
  AECHAR* pwPath, uint8 uPathBufLen
)
{
  AECHAR* pwTmp;
  uint8 uLen;

  if ( (pwTmp = WSTRRCHR( pwSrc, DIRECTORY_CHAR )) != NULL)
  {
    uLen = pwTmp - pwSrc;
    WSTRLCPY( pwFile, pwTmp, uFileBufLen );
    WSTRLCPY( pwPath, pwSrc, uPathBufLen );
    pwPath[ uLen ] = NULL;
  }
  else
  {
    WSTRLCPY( pwFile, pwSrc, uFileBufLen );
  }
}

static int OEMBTExtFTP_CheckCmdStatus( bt_cmd_status_type stat )
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

static OEMBTExtFTPobj_t* OEMBTExtFTP_FindMe(bt_app_id_type appId)
{
  OEMBTExtFTPobj_t* pMe;

  for (pMe = gMgr.pNextFTP; pMe != NULL; pMe = pMe->pNextFTP)
  {
    if (pMe->appId == appId)
    {
      return pMe;
    }
  }
  return NULL;
}

static void OEMBTExtFTP_NotifyProgress(OEMBTExtFTPobj_t* pMe)
{
  AEEBTNotification*  pN    = NULL;

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "FTP Notify progress - Can't get free not.",
               0, 0, 0 );
  }
  else if ( pMe->bBrowse )
  {  
     MSG_HIGH( "FTP Notify progress - browsing... ", 0, 0, 0 );
     IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
  }
  else
  {
    pN->data.uError  = AEEBT_FTP_ERR_NONE ;
    pN->data.ProgressInfo.objSize = pMe->dwFileSize;
    pN->data.ProgressInfo.numBytes = ( pMe->bytesSent > 0 ) ?
                                       pMe->bytesSent :
                                       pMe->bytesRcvd;
    pN->uID = AEEBT_FTP_EVT_PROGRESS_IND;

    MSG_HIGH( "FTP Notify progress - objSize %x, bytes trans %x",
               pMe->dwFileSize, pN->data.ProgressInfo.numBytes, 0 );

    pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_FTP );
    IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
  }
  return;
}

static boolean OEMBTExtFTP_HandleCmdDone( 
  bt_ev_gn_cmd_done_type* pCmdDn, 
  AEEBTNotification*      pN,
  OEMBTExtFTPobj_t*       pMe
)
{
  boolean doSendNotif = TRUE;

  MSG_LOW( "FTP_HandleCmdDone - stat=%x cmd=%x", 
           pCmdDn->cmd_status, pCmdDn->cmd_type, 0 );

  switch ( pCmdDn->cmd_type )
  {
    case BT_PF_CMD_FTP_SRV_REGISTER:
    {
      pN->uID = AEEBT_FTP_EVT_REG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_FTP_ERR_NONE )
      {
        pMe->bIsServer = TRUE;
      }
      break;
    }
    case BT_PF_CMD_FTP_SRV_DEREGISTER:
    {
      pN->uID = AEEBT_FTP_EVT_DEREG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError != AEEBT_ERR_OPERATION_IN_PROGRESS )
      {
        pMe->bIsServer = FALSE;
      }
      break;
    }
    case BT_PF_CMD_FTP_SRV_ACCEPT:
    {
      if ( pCmdDn->cmd_status != BT_CS_GN_SUCCESS || 
           pMe->bAcceptConn == FALSE )
      {
        doSendNotif = FALSE;
        pMe->serverConnID = BT_PF_FTP_NO_CONN_ID;
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_CONNECTED;
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
        /* Init to success since there was no previous write.*/
        pMe->prevWriteStatus = BT_CS_GN_SUCCESS;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */
      }
      break;
    }
    case BT_PF_CMD_FTP_CLI_CONNECT:
    {
      if ( pCmdDn->cmd_status == BT_CS_GN_SUCCESS )
      {
        doSendNotif = FALSE;
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
        /* Init to success since there was no previous write.*/
        pMe->prevWriteStatus = BT_CS_GN_SUCCESS;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_CONN_FAILED;
        pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      }
      break;
    }
    case BT_PF_CMD_FTP_CLI_DISCONNECT:
    {
      if ( pCmdDn->cmd_status == BT_CS_GN_SUCCESS )
      {
        doSendNotif = FALSE;
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_DISCONNECTED;
        pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      }
      break;
    }
    case BT_PF_CMD_FTP_CLI_PUT:
    case BT_PF_CMD_FTP_CLI_TREE_PUT:
    {
      if ( pCmdDn->cmd_status == BT_CS_GN_SUCCESS )
      {
        doSendNotif = FALSE;
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_PUT_DNE;
        pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      }
      break;
    }
    case BT_PF_CMD_FTP_CLI_DELETE:
    {
      if ( pCmdDn->cmd_status == BT_CS_GN_SUCCESS )
      {
        doSendNotif = FALSE;
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_DELETE_DNE;
        pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      }
      break;
    }
    case BT_PF_CMD_FTP_CLI_GET:
    case BT_PF_CMD_FTP_CLI_TREE_GET:
    {
      if ( pCmdDn->cmd_status == BT_CS_GN_SUCCESS )
      {
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
        /* Init to success since there was no previous write.*/
        pMe->prevWriteStatus = BT_CS_GN_SUCCESS;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */
        doSendNotif = FALSE;
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_GET_DNE;
        pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      }
      break;
    }
    case BT_PF_CMD_FTP_CLI_SET_PATH:
    {
      if ( pCmdDn->cmd_status == BT_CS_GN_SUCCESS )
      {
        doSendNotif = FALSE;
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_PATH_SET;
        pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      }
      break;
    }
    case BT_PF_CMD_FTP_CLI_NEW_SUBFOLDER:
    {
      if ( pCmdDn->cmd_status == BT_CS_GN_SUCCESS )
      {
        doSendNotif = FALSE;
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_FOLDER_CREATED;
        pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      }
      break;
    }
    case BT_PF_CMD_FTP_CLI_GET_FOLDER_LISTING:
    {
      if ( pCmdDn->cmd_status == BT_CS_GN_SUCCESS )
      {
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
        /* Init to success since there was no previous write.*/
        pMe->prevWriteStatus = BT_CS_GN_SUCCESS;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */
        doSendNotif = FALSE;
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_FOLDER_LISTING;
        pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      }
      break;
    }
    case BT_PF_CMD_FTP_CLI_ABORT:
    {
      pN->uID = AEEBT_FTP_EVT_ABORTED;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_FTP_SRV_FORCE_DCN:
    {
      doSendNotif = FALSE;
      break;
    }
    default:
    {
      doSendNotif = FALSE;
    }
  }

  return doSendNotif;
}

static void OEMBTExtFTP_OpenCb( OEMBTExtFTP_EvCb* pEvCb )
{
  OEMBTExtFTPobj_t*  pMe = OEMBTExtFTP_FindMe( pEvCb->appId );
  bt_cmd_status_type status = BT_CS_GN_SUCCESS;
  char               szName[ (AEEBT_MAX_FILE_NAME * 2) + 1 ];
  uint8              uLen = 0;
  AEEOpenFileMode    mode;
  IFile*             pFile = NULL;
  AEEFileInfoEx      fileInfoEx;
  AECHAR*            pwSrc = pEvCb->wName;
  ACONTEXT*          pCurAC;

  fileInfoEx.nStructSize = sizeof( AEEFileInfoEx );
  fileInfoEx.pszFile = NULL;
  fileInfoEx.nMaxFile = 0;
  fileInfoEx.pszDescription = NULL;
  fileInfoEx.nDescriptionSize = 0;
  fileInfoEx.pClasses = 0;
  fileInfoEx.nClassesSize = 0;

  if ( pMe == NULL )
  {
    // how did we get here?
    MSG_ERROR( "OpenCb: wrong app id=%x",pEvCb->appId, 0, 0 );
    return;
  }

  if ( WSTRLEN( pMe->wCurrentFolder ) > 0 )
  {
    AEEBT_FROM_WSTR( pMe->wCurrentFolder, szName, sizeof(szName) );
    STRLCAT( szName, DIRECTORY_STR, sizeof( szName ) );
    uLen = STRLEN( szName );

    pwSrc = WSTRRCHR( pEvCb->wName, DIRECTORY_CHAR );
    if ( pwSrc == NULL )
    {
      pwSrc = pEvCb->wName;
    }
    else
    {
      pwSrc++;
    }
  }
  // REMOVE when done DEBUGging
  // MSG_ERROR( "OpenCb: name=%c%c%c", pwSrc[0], pwSrc[1], pwSrc[2] );
  // REMOVE when done DEBUGging

  AEEBT_FROM_WSTR( pwSrc, szName + uLen, sizeof(szName) - uLen );

  mode = (pEvCb->mode == BT_PF_FTP_MODE_READ) ? _OFM_READ : _OFM_CREATE;

  pCurAC = ENTER_APP_CONTEXT( pMe->pac );
  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "OpenCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else if ( ((pFile = IFILEMGR_OpenFile( pMe->pFileMgr, szName, 
                                         mode )) == NULL) ||
            (IFILEMGR_GetInfoEx( pMe->pFileMgr, szName, &fileInfoEx ) != SUCCESS) )
  {
    MSG_ERROR( "OpenCb: OpenFile failed pFile=%x mode=%x", pFile, mode, 0 );
    status = (mode == _OFM_READ) ? BT_CS_PF_OBEX_NOT_FOUND : 
                                   BT_CS_PF_OBEX_ACCESS_DENIED;
  }
  else
  {
    IFILE_Seek( pFile, _SEEK_START, 0 );
  }
  (void) LEAVE_APP_CONTEXT( pCurAC );

  // REMOVE when done DEBUGging
  // MSG_ERROR( "OpenCb: h=%x", pFile, 0, 0 );
  // REMOVE when done DEBUGging

  pMe->bytesSent = 0;
  pMe->bytesRcvd = 0;
  pMe->dwFileSize = (pEvCb->mode == BT_PF_FTP_MODE_READ) ?
                     fileInfoEx.dwSize :
                     0;
  pMe->bBrowse = FALSE;

  rex_enter_crit_sect( &FTPCritSect );
  pEvCb->bInUse = FALSE;
  rex_leave_crit_sect( &FTPCritSect );

  bt_cmd_pf_ftp_cli_open_done( pMe->appId, pMe->clientConnID, 
                               (bt_pf_ftp_handle_type)pFile, 
                               fileInfoEx.dwSize, status );
}

static void OEMBTExtFTP_ReadCb( OEMBTExtFTP_EvCb* pEvCb )
{
  OEMBTExtFTPobj_t*  pMe = OEMBTExtFTP_FindMe( pEvCb->appId );
  uint32             bytesRead = 0;
  bt_cmd_status_type status = BT_CS_GN_SUCCESS;
  AEEFileInfoEx      fileInfoEx;
  ACONTEXT*          pCurAC;

  fileInfoEx.nStructSize = sizeof( AEEFileInfoEx );
  fileInfoEx.pszFile = NULL;
  fileInfoEx.nMaxFile = 0;
  fileInfoEx.pszDescription = NULL;
  fileInfoEx.nDescriptionSize = 0;
  fileInfoEx.pClasses = NULL;
  fileInfoEx.nClassesSize = 0;

  if ( pMe == NULL )
  {
    // how did we get here?
    MSG_ERROR( "ReadCb: wrong app id=%x",pEvCb->appId, 0, 0 );
    return;
  }

  // validate file handle
  if ( pEvCb->handle == NULL ) 
  {
    MSG_ERROR( "ReadCb: NULL file handle", 0, 0, 0 );
    return;
  }

  pCurAC = ENTER_APP_CONTEXT( pMe->pac );
  if ( ((pMe->bIsServer != FALSE) && (pMe->serverConnID != pEvCb->connId)) || 
       ((pMe->bIsServer == FALSE) && (pMe->clientConnID != pEvCb->connId)) )
  {
    MSG_ERROR( "ReadCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else if ( IFILE_GetInfoEx( (IFile*)pEvCb->handle, &fileInfoEx ) != SUCCESS )
  {
    MSG_ERROR( "ReadCb: file inaccessible, h=%x", pEvCb->handle, 0, 0 );
    status = OI_OBEX_ACCESS_DENIED;
  }
  else if ( pMe->bytesSent >= fileInfoEx.dwSize )
  {
    // REMOVE when done DEBUGging
    // MSG_ERROR( "ReadCb: h=%x sent=%d fSize=%d", pEvCb->handle, pMe->bytesSent, 
    //            fileInfoEx.dwSize );
    // REMOVE when done DEBUGging
    status = BT_CS_PF_END_OF_FILE;
  }
  else // everything checks out
  {
    if ( pMe->bytesSent == 0 )
    {
      if ( IFILE_Seek( (IFile*)pEvCb->handle, _SEEK_START, 0 ) != SUCCESS )
      {
        MSG_ERROR( "ReadCb: seek failed", 0, 0, 0 );
      }
    }
    bytesRead = pEvCb->maxBytes;
    if ( bytesRead > AEEBT_OBEX_BUFFER_SIZE )
    {
      bytesRead = AEEBT_OBEX_BUFFER_SIZE;
    }
    bytesRead = IFILE_Read( (IFile*)pEvCb->handle, pMe->pBuffer, 
                            bytesRead );
    pMe->bytesSent += bytesRead;

    OEMBTExtFTP_NotifyProgress( pMe );

    if ( bytesRead == 0 )
    {
      MSG_ERROR( "ReadCb: failed to read from file", 0, 0, 0 );
      status = BT_CS_PF_READ_ERROR;
    }
  }
  (void) LEAVE_APP_CONTEXT( pCurAC );

  // done with EvCb
  rex_enter_crit_sect( &FTPCritSect );
  pEvCb->bInUse = FALSE;
  rex_leave_crit_sect( &FTPCritSect );

  if ( pMe->bIsServer )
  {
    bt_cmd_pf_ftp_srv_read_done( pMe->appId, pMe->serverConnID, pEvCb->handle,
                                 pMe->pBuffer, bytesRead, status );
  }
  else
  {
    bt_cmd_pf_ftp_cli_read_done( pMe->appId, pMe->clientConnID, pEvCb->handle,
                                 pMe->pBuffer, bytesRead, status );
  }
}

static void OEMBTExtFTP_WriteCb( OEMBTExtFTP_EvCb* pEvCb )
{
  OEMBTExtFTPobj_t*  pMe = OEMBTExtFTP_FindMe( pEvCb->appId );
  uint32             bytesWritten = 0;
  bt_cmd_status_type status = BT_CS_GN_SUCCESS;
  ACONTEXT*          pCurAC;

#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
  /* Default to TRUE so Write Done notification is done 
     unless explictly disabled. */
  boolean            notifyWriteDoneNeeded = TRUE;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */

  if ( pMe == NULL )
  {
    // how did we get here?
    MSG_ERROR( "WriteCb: wrong app id=%x",pEvCb->appId, 0, 0 );
    return;
  }

  // validate file handle
  if ( pEvCb->handle == NULL ) 
  {
    MSG_ERROR( "WriteCb: NULL file handle", 0, 0, 0 );
    return;
  }

  pCurAC = ENTER_APP_CONTEXT( pMe->pac );
  if ( ((pMe->bIsServer != FALSE) && (pMe->serverConnID != pEvCb->connId)) || 
       ((pMe->bIsServer == FALSE) && (pMe->clientConnID != pEvCb->connId)) )
  {
    MSG_ERROR( "WriteCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
  else if (pMe->prevWriteStatus != BT_CS_GN_SUCCESS) 
  {
    /* Error on previous write operation, don't do this write, return status. */
    status = pMe->prevWriteStatus;

    MSG_ERROR( "WriteCb: Previous Write Error! Status = 0x%X", pMe->prevWriteStatus,0,0 );
  }
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */
  else if ( pEvCb->maxBytes == 0 )
  {
    MSG_LOW( "WriteCb: nothing to write", 0, 0, 0 );
  }
  else if ( IFILEMGR_GetFreeSpace( pMe->pFileMgr, NULL ) < pEvCb->maxBytes )
  {
    MSG_ERROR( "WriteCb: EFS full, failed to write to obj", 0, 0, 0 );
    status = BT_CS_PF_OBEX_DATABASE_FULL;
  }
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
  /* Check for NULL buffer or if size is larger than buffer size. */
  else if (pMe->pBuffer == NULL || pEvCb->maxBytes > FTP_BUFFER_SIZE)
  {
      /* Error, can't copy to save buffer, shouldn't happen. */
      MSG_ERROR( "WriteCb: Save Buffer Error, pBuffer:0x%X, maxBytes:%d", 
                 pMe->pBuffer, pEvCb->maxBytes, 0 );
      /* Not sure what this error code should be... */
      status = BT_CS_PF_WRITE_ERROR;
  }
  else
  {
      /* Copy data to saved buffer. */
      MSG_LOW("OEMBTExtFTP_WriteCb: Copy to saved buffer",0, 0, 0 );
      MEMCPY( (void*)pMe->pBuffer, (void*)pEvCb->pSrc, pEvCb->maxBytes);

      if ( pMe->bIsServer )
      {
        MSG_LOW("OEMBTExtFTP_WriteCb: Call bt_cmd_pf_ftp_srv_write_done",0, 0, 0 );
        bt_cmd_pf_ftp_srv_write_done( pMe->appId, pMe->serverConnID,
                                      pEvCb->handle, status );
      }
      else
      {
        MSG_LOW("OEMBTExtFTP_WriteCb: Call bt_cmd_pf_ftp_cli_write_done",0, 0, 0 );
        bt_cmd_pf_ftp_cli_write_done( pMe->appId, pMe->clientConnID,
                                      pEvCb->handle, status );
      }

      /* Write Done notification is done so not needed to be done after File write. */
      notifyWriteDoneNeeded = FALSE;

      MSG_LOW("OEMBTExtFTP_WriteCb: Call IFILE_Write",0, 0, 0 );
      if ( (bytesWritten = IFILE_Write( (IFile*)pEvCb->handle, pMe->pBuffer, 
                                         pEvCb->maxBytes )) == 0 )
      {
        MSG_ERROR( "WriteCb: failed to write to obj", 0, 0, 0 );
        status = BT_CS_PF_WRITE_ERROR;

      }
      else
      {
        MSG_LOW("WriteCb: FILE Write Done: maxBytes-0x%X bytesWritten-0x%X",pEvCb->maxBytes, bytesWritten,0 );

        pMe->bytesRcvd += bytesWritten;
        OEMBTExtFTP_NotifyProgress( pMe );
      }
  }
  (void) LEAVE_APP_CONTEXT( pCurAC );

  // done with EvCb
  rex_enter_crit_sect( &FTPCritSect );
  pEvCb->bInUse = FALSE;
  rex_leave_crit_sect( &FTPCritSect );

  /* Write Done notification is done so not needed to be done after File write. */
  if (notifyWriteDoneNeeded == TRUE)
  {
    if ( pMe->bIsServer )
    {
      MSG_LOW("OEMBTExtFTP_WriteCb: Call bt_cmd_pf_ftp_srv_write_done POST WRITE",0, 0, 0 );
      bt_cmd_pf_ftp_srv_write_done( pMe->appId, pMe->serverConnID,
                                    pEvCb->handle, status );
    }
    else
    {
      MSG_LOW("OEMBTExtFTP_WriteCb: Call bt_cmd_pf_ftp_cli_write_done POST WRITE",0, 0, 0 );
      bt_cmd_pf_ftp_cli_write_done( pMe->appId, pMe->clientConnID,
                                    pEvCb->handle, status );
    }
  }

  /* Save current status as previous for next call. */
  pMe->prevWriteStatus = status;
  MSG_LOW("OEMBTExtFTP_WriteCb: Saved Write Status: 0x%X",pMe->prevWriteStatus, 0, 0 );

#else
  else if ( (bytesWritten = IFILE_Write( (IFile*)pEvCb->handle, pEvCb->pSrc, 
                                         pEvCb->maxBytes )) == 0 )
  {
    MSG_ERROR( "WriteCb: failed to write to obj", 0, 0, 0 );
    status = BT_CS_PF_WRITE_ERROR;
  }
  else
  {
    pMe->bytesRcvd += bytesWritten;
    OEMBTExtFTP_NotifyProgress( pMe );
  }
  (void) LEAVE_APP_CONTEXT( pCurAC );

  // done with EvCb
  rex_enter_crit_sect( &FTPCritSect );
  pEvCb->bInUse = FALSE;
  rex_leave_crit_sect( &FTPCritSect );

  if ( pMe->bIsServer )
  {
    bt_cmd_pf_ftp_srv_write_done( pMe->appId, pMe->serverConnID,
                                  pEvCb->handle, status );
  }
  else
  {
    bt_cmd_pf_ftp_cli_write_done( pMe->appId, pMe->clientConnID,
                                  pEvCb->handle, status );
  }
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */

}

static void OEMBTExtFTP_CloseCb( OEMBTExtFTP_EvCb* pEvCb )
{
  OEMBTExtFTPobj_t*  pMe = OEMBTExtFTP_FindMe( pEvCb->appId );
  AEEBTNotification* notif;
  boolean            bSendNotif = TRUE;
  AEEFileInfoEx      fileInfoEx;
  char               szFileName[ 2*AEEBT_MAX_FILE_NAME + 1 ] = "\0";

  fileInfoEx.nStructSize = sizeof( AEEFileInfoEx );
  fileInfoEx.pszFile = szFileName;
  fileInfoEx.nMaxFile = 2*AEEBT_MAX_FILE_NAME + 1;
  fileInfoEx.pszDescription = NULL;
  fileInfoEx.nDescriptionSize = 0;
  fileInfoEx.pClasses = 0;
  fileInfoEx.nClassesSize = 0;

  if ( pMe == NULL )
  {
    // how did we get here?
    MSG_ERROR( "CloseCb: wrong app id=%x",pEvCb->appId, 0, 0 );
  }
  else if ( pEvCb->handle == NULL ) 
  {
    MSG_ERROR( "CloseCb: NULL file handle", 0, 0, 0 );
  }
  else
  {
    notif = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );

    if( notif == NULL )
    {
      bSendNotif = FALSE;
    }

    if ( ((pMe->bIsServer != FALSE) && 
          (pMe->serverConnID != pEvCb->connId)) || 
         ((pMe->bIsServer == FALSE) && 
          (pMe->clientConnID != pEvCb->connId)) )
    {
      MSG_ERROR( "CloseCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    }

    // REMOVE when done DEBUGging
    // MSG_ERROR( "CloseCb: h=%x", pEvCb->handle, 0, 0 );
    // REMOVE when done DEBUGging

    //pMe->bytesRcvd = 0;
    //pMe->bytesSent = 0;
    pMe->bBrowse = FALSE;

    if ( IFILE_GetInfoEx( (IFile*)pEvCb->handle, &fileInfoEx ) != SUCCESS )
    {
      MSG_ERROR( "CloseCb: file inaccessible, h=%x", pEvCb->handle, 0, 0 );
    }

    if ( IFILE_Release( (IFile*)pEvCb->handle ) > 0 )
    {
      MSG_ERROR( "CloseCb: file h=%x isn't really closed", pEvCb->handle, 0, 0 );
    }
    else
    {
      pMe->pFile = NULL;
    }

    if ( pEvCb->event == BT_EV_PF_FTP_CLI_GET_FOLDER_LIST_RSP )
    {
      // the final folder listing data is received from server
      pMe->pFile = NULL;
      if( notif != NULL )
      {
        notif->uID = AEEBT_FTP_EVT_FOLDER_LISTING;
      }
    }
    else if ( pMe->bIsServer != FALSE )
    {
      if ( pMe->currReq == AEEBT_FTP_REQ_PUT )
      {
        if( notif != NULL )
        {
          notif->uID = AEEBT_FTP_EVT_PUT_DNE;
        }
      }
      else if ( pMe->currReq == AEEBT_FTP_REQ_GET )
      {
        if( notif != NULL )
        {
          notif->uID = AEEBT_FTP_EVT_GET_DNE;
        }

        // If the client abort the put operation, server should delete the
        // incomplete file.
        if ( OEMBTExt_MapCmdStatus( pEvCb->status ) != AEEBT_ERR_NONE )
        {
          IFILEMGR_Remove( pMe->pFileMgr, szFileName );
        }
      }
      else
      {
        bSendNotif = FALSE;
        MSG_LOW( "CloseCb: ignoring req=%x", pMe->currReq, 0, 0 );
      }
      pMe->currReq = AEEBT_FTP_REQ_NONE;
    }
    else
    {
      // Delete incomplete file if Client Get failed
      if ( OEMBTExt_MapCmdStatus( pEvCb->status ) != AEEBT_ERR_NONE )
      {
        IFILEMGR_Remove( pMe->pFileMgr, szFileName );
      }

      bSendNotif = FALSE;
    }
    if ( bSendNotif != FALSE )
    {
      notif->data.uError = OEMBTExt_MapCmdStatus( pEvCb->status );
      notif->uID = (notif->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_FTP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, notif, pMe->pac );
    }
    else
    {
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, notif );
    }
  }
  rex_enter_crit_sect( &FTPCritSect );
  pEvCb->bInUse = FALSE;
  rex_leave_crit_sect( &FTPCritSect );
}

static void OEMBTExtFTP_DisconnectCb( OEMBTExtFTP_EvCb* pEvCb )
{
  OEMBTExtFTPobj_t* pMe = OEMBTExtFTP_FindMe( pEvCb->appId );
  AEEBTNotification* notif;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( (pMe->bIsServer != FALSE) && (pMe->serverConnID != pEvCb->connId) || 
       (pMe->bIsServer == FALSE) && (pMe->clientConnID != pEvCb->connId) )
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
      if ( pMe->bIsServer == FALSE ) 
      {
        pMe->clientConnID = BT_PF_FTP_NO_CONN_ID;
      }
      else
      {
        pMe->serverConnID = BT_PF_FTP_NO_CONN_ID;
        notif->data.bdAddr = pMe->BDAddr;
      }

      notif->uID = AEEBT_FTP_EVT_DISCONNECTED;
      notif->data.uError = AEEBT_FTP_ERR_NONE;

      notif->uID = (notif->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_FTP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, notif, pMe->pac );
    }
    else
    {
      MSG_ERROR( "DisconnectCb: no free notifications",0,0,0);
    }
  }
  rex_enter_crit_sect( &FTPCritSect );
  pEvCb->bInUse = FALSE;
  rex_leave_crit_sect( &FTPCritSect );

  // Do server dereg if it is pending (after force disconnect)
  if ( pMe->bDoDeregister )
  {
    pMe->bDoDeregister = FALSE;
    if ( bt_cmd_pf_ftp_srv_deregister( pMe->appId ) != BT_CS_GN_PENDING )
    {
      MSG_ERROR( "DisconnectCb: S de-reg failed!", 0, 0, 0 );
    }
  }
}

static void OEMBTExtFTP_SrvOpenCb(OEMBTExtFTP_EvCb* pEvCb )
{
  OEMBTExtFTPobj_t*  pMe = OEMBTExtFTP_FindMe( pEvCb->appId);
  AEEBTNotification* pN    = NULL;

  MSG_LOW( "FTPEvCb: S open req, id=%x m=%x", 
          pEvt->conn_id, pEvt->mode, 0 );

  if ( pMe == NULL )
  {
    return;
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );

  if ( pEvCb->mode == BT_PF_FTP_MODE_READ )
  {
    pN->uID = AEEBT_FTP_EVT_GET_REQ;
  }
  else
  {
    pN->uID = AEEBT_FTP_EVT_PUT_REQ;
    pMe->dwFileSize = pEvCb->maxBytes;
  }

  WSTRLCPY( (void*)pN->data.AccessReq.wName, (void*)pEvCb->wName,
            ARR_SIZE(pN->data.AccessReq.wName) );

  pN->data.AccessReq.wName[ pEvCb->nameLen ] = 0;
  pN->data.AccessReq.objSize= pEvCb->maxBytes;

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_FTP );
  IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );

  // done with EvCb
  rex_enter_crit_sect( &FTPCritSect );
  pEvCb->bInUse = FALSE;
  rex_leave_crit_sect( &FTPCritSect );

  return;
}


static OEMBTExtFTP_EvCb* OEMBTExtFTP_GetFreeEvCb( void )
{
  uint8 i;
  OEMBTExtFTP_EvCb* pEvCb = NULL;

  for ( i=0; i<(sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++)
  {
    if ( gMgr.FreeEvCb[ i ].bInUse == FALSE )
    {
      gMgr.FreeEvCb[ i ].bInUse = TRUE;
      pEvCb = &gMgr.FreeEvCb[ i ];
      break;
    }
  }
  if ( pEvCb == NULL )
  {
    MSG_ERROR( "GetFreeEvCb: none left", 0, 0, 0 );
  }
  return pEvCb;
}

// called from BT task context; so just queue the event
static void OEMBTExtFTP_EventCallback( bt_ev_msg_type* ev_msg_ptr )
{
  OEMBTExtFTPobj_t*  pMe   = OEMBTExtFTP_FindMe( ev_msg_ptr->ev_hdr.bt_app_id );
  OEMBTExtFTP_EvCb*  pEvCb = NULL;
  AEEBTNotification* pN    = NULL;
  bt_cmd_status_type status;

  if ( pMe == NULL )
  {
    return; // probably for an app that no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "FTPEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }

  pN->data.uError  = AEEBT_FTP_ERR_NONE;

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
    {
      if ( ! OEMBTExtFTP_HandleCmdDone( &ev_msg_ptr->ev_msg.ev_gn_cmd_done,
                                        pN, pMe ) )
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return; // no need to send notification to app
      }
      break;
    } // end of case BT_EV_GN_CMD_DONE

    // ----------------------------------------------------------------------
    // client events
    // ----------------------------------------------------------------------
    case BT_EV_PF_FTP_CLI_CON_CFM:
    case BT_EV_PF_FTP_CLI_CON_READ_ONLY_CFM:
    {
      MSG_LOW( "FTPEvCb: C con cfm, id=%x stat=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_cli_con_cfm.conn_id,
               ev_msg_ptr->ev_msg.ev_ftp_cli_con_cfm.status, 0 );
      if (ev_msg_ptr->ev_msg.ev_ftp_cli_con_cfm.status == BT_CS_GN_SUCCESS )
      {
        pMe->clientConnID = ev_msg_ptr->ev_msg.ev_ftp_cli_con_cfm.conn_id;
        pN->uID = AEEBT_FTP_EVT_CONNECTED;
        pN->data.bReadOnly = TRUE;
        if ( ev_msg_ptr->ev_hdr.ev_type == BT_EV_PF_FTP_CLI_CON_CFM )
        {
          pN->data.bReadOnly = FALSE;
        }

        pMe->wCurrentFolder[ 0 ] = NULL;  // clear current folder name
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_CONN_FAILED;
      }
      break;
    }
    case BT_EV_PF_FTP_CLI_DCN_CFM:
    {
      MSG_LOW( "FTPEvCb: C dcn cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_cli_dcn_cfm.status, 
               ev_msg_ptr->ev_msg.ev_ftp_cli_dcn_cfm.conn_id, 0 );

      pEvCb = OEMBTExtFTP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_ftp_cli_dcn_cfm.conn_id;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_DisconnectCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_FTP_CLI_AUTH_REQ:
    {
      MSG_LOW( "FTPEvCb: C auth req, id=%x ur=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_cli_auth_req.conn_id,
               ev_msg_ptr->ev_msg.ev_ftp_cli_auth_req.user_id_required, 0 );

      pN->uID = AEEBT_FTP_EVT_AUTH_REQ;
      pN->data.AuthReq.bdAddr          = pMe->BDAddr;
      pN->data.AuthReq.bUserIDRequired = 
        ev_msg_ptr->ev_msg.ev_ftp_cli_auth_req.user_id_required;
      break;
    }
    case BT_EV_PF_FTP_CLI_PUT_DNE:
    {
      bt_pf_ev_ftp_cli_put_dne_type* pEvt = 
        &ev_msg_ptr->ev_msg.ev_ftp_cli_put_dne;
      if ( pMe->clientConnID == pEvt->conn_id )
      {
        MSG_LOW( "FTPEvCb: C put dne, stat=%x id=%x", 
                 pEvt->status, pEvt->conn_id, 0 );
        pN->uID = AEEBT_FTP_EVT_PUT_DNE;
        pN->data.uError = OEMBTExt_MapCmdStatus( pEvt->status );
      }
      else
      {
        MSG_ERROR( "FTPEvCb: C put dne, wrong conn=%x", pEvt->conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_FTP_CLI_DELETE_DNE:
    {
      bt_pf_ev_ftp_cli_delete_dne_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_cli_delete_dne;
      MSG_LOW( "FTPEvCb: C del dne, id=%x st=%x", 
               pEvt->conn_id, pEvt->status, 0 );
      if ( pMe->clientConnID == pEvt->conn_id )
      {
        pN->uID = AEEBT_FTP_EVT_DELETE_DNE;
        pN->data.uError = OEMBTExt_MapCmdStatus( pEvt->status );
      }
      else
      {
        MSG_ERROR( "FTPEvCb: C del dne, wrong conn=%x", pEvt->conn_id, 0, 0 );
      }
      break;
    }
    case BT_EV_PF_FTP_CLI_GET_RSP:
    {
      if ( pMe->clientConnID != 
           ev_msg_ptr->ev_msg.ev_ftp_cli_get_rsp.conn_id )
      {
        MSG_ERROR( "FTPEvCb: C get rsp, wrong conn id=%x", 
                   ev_msg_ptr->ev_msg.ev_ftp_cli_get_rsp.conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "FTPEvCb: C get rsp, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_cli_get_rsp.status, 
               ev_msg_ptr->ev_msg.ev_ftp_cli_get_rsp.conn_id, 0 );
      pN->uID = AEEBT_FTP_EVT_GET_DNE;
      pN->data.uError = OEMBTExt_MapCmdStatus( ev_msg_ptr->ev_msg.
                                               ev_ftp_cli_get_rsp.status );
      break;
    }
    case BT_EV_PF_FTP_CLI_SET_PATH_DNE:
    {
      bt_pf_ev_ftp_cli_set_path_dne_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_cli_set_path_dne;
      MSG_LOW( "FTPEvCb: C set path dne, id=%x st=%x", 
               pEvt->conn_id, pEvt->status, 0 );
      if ( pMe->clientConnID == pEvt->conn_id )
      {
        pN->uID = AEEBT_FTP_EVT_PATH_SET;
        pN->data.uError = OEMBTExt_MapCmdStatus( pEvt->status );
      }
      else
      {
        MSG_ERROR( "FTPEvCb: C set path dne, wrong conn id=%x", 
                   pEvt->conn_id, 0, 0 );
      }
      break;
    }
    case BT_EV_PF_FTP_CLI_NEW_SUBFOLDER_DNE:
    {
      bt_pf_ev_ftp_cli_new_subfolder_dne_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_cli_new_subfolder_dne;
      MSG_LOW( "FTPEvCb: C new subfolder dne, id=%x st=%x", 
               pEvt->conn_id, pEvt->status, 0 );
      if ( pMe->clientConnID == pEvt->conn_id )
      {
        pN->uID = AEEBT_FTP_EVT_FOLDER_CREATED;
        pN->data.uError = OEMBTExt_MapCmdStatus( pEvt->status );
      }
      else
      {
        MSG_ERROR( "FTPEvCb: C new subfolder dne, wrong conn id=%x", 
                   pEvt->conn_id, 0, 0 );
      }
      break;
    }
    case BT_EV_PF_FTP_CLI_GET_FOLDER_LIST_RSP:
    {
      bt_pf_ev_ftp_cli_get_folder_list_rsp_type* pEvt = 
        &ev_msg_ptr->ev_msg.ev_ftp_cli_get_folder_list_rsp;

      MSG_LOW( "FTPEvCb: C get folder list rsp, id=%x stat=%x", 
                 pEvt->conn_id, pEvt->status, 0 );

      if ( (pMe->clientConnID == pEvt->conn_id) && (pMe->pFile != NULL) )
      {
        if ( (pEvt->status == BT_CS_GN_SUCCESS) ||
             (pEvt->status == BT_CS_PF_END_OF_FILE) )
        {
          // schedule callback to write to file or to close file
          pEvCb = OEMBTExtFTP_GetFreeEvCb();
          if ( pEvCb != NULL )
          {
            pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
            pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
            pEvCb->connId   = pEvt->conn_id;
            pEvCb->handle   = (bt_pf_ftp_handle_type) pMe->pFile;

            if ( pEvt->folder_list_data_len > 0 )
            {
              pEvCb->pSrc     = pEvt->folder_list_data_ptr;
              pEvCb->maxBytes = pEvt->folder_list_data_len;
              CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_WriteCb, pEvCb );
            }
            else
            { 
              pEvCb->status = BT_CS_GN_SUCCESS;
              CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_CloseCb, pEvCb );
            }
            OEMBTExt_ResumeCallback ( &pEvCb->cb );
          }
        }
        else
        {
          pN->uID = AEEBT_FTP_EVT_FOLDER_LISTING;
          pN->data.uError = OEMBTExt_MapCmdStatus( pEvt->status );
          break;
        }
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      else if ( pMe->clientConnID != pEvt->conn_id )
      {
        MSG_ERROR( "FTPEvCb: C get folder list rsp, wrong conn id=%x", 
                   pEvt->conn_id, 0, 0 );
        status = OI_OBEX_NOT_FOUND;
      }
      else
      {
        MSG_ERROR( "FTPEvCb: C get folder list rsp, no opened file", 0, 0, 0 );
        status = OI_OBEX_PRECONDITION_FAILED;
      }
      // failed to get folder listing from server
      bt_cmd_pf_ftp_cli_write_done( pMe->appId, pMe->clientConnID,
                                    (bt_pf_ftp_handle_type) pMe->pFile, 
                                    status );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_FTP_CLI_TREE_PUT_DNE:
    {
      bt_pf_ev_ftp_cli_tree_put_dne_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_cli_tree_put_dne;
      MSG_LOW( "FTPEvCb: C tree put dne, id=%x st=%x", 
               pEvt->conn_id, pEvt->status, 0 );
      if ( pMe->clientConnID == pEvt->conn_id )
      {
        pN->uID = AEEBT_FTP_EVT_PUT_DNE;
        pN->data.uError = OEMBTExt_MapCmdStatus( pEvt->status );
      }
      break;
    }
    case BT_EV_PF_FTP_CLI_TREE_GET_RSP:
    {
      bt_pf_ev_ftp_cli_tree_get_rsp_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_cli_tree_get_rsp;
      MSG_LOW( "FTPEvCb: C tree get rsp, id=%x st=%x", 
               pEvt->conn_id, pEvt->status, 0 );
      if ( pMe->clientConnID == pEvt->conn_id )
      {
        pN->uID = AEEBT_FTP_EVT_GET_DNE;
        pN->data.uError = OEMBTExt_MapCmdStatus( pEvt->status );
      }
      break;
    }
    case BT_EV_PF_FTP_CLI_BROWSE_REQ:
    {
      bt_pf_ev_ftp_cli_browse_req_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_cli_browse_req;
      MSG_LOW( "FTPEvCb: C browse req, id=%x l=%x", 
               pEvt->conn_id, pEvt->folder_name_len, 0 );
      if ( pMe->clientConnID == pEvt->conn_id )
      {
        pN->uID = AEEBT_FTP_EVT_LS_REQ;
        bt_pf_copy_unicode_str( 
          (uint16*)pN->data.AccessReq.wName,
          (uint16*)pEvt->folder_name_str,
          BT_PF_MAX_FILENAME_LEN,
          NULL );
      }
      else
      {
        bt_cmd_pf_ftp_cli_browse_done( pMe->appId, pMe->clientConnID, 
                                       (bt_pf_ftp_handle_type) NULL ,
                                       0, BT_CS_PF_OBEX_NOT_FOUND );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_FTP_CLI_OPEN_REQ:
    {
      uint8 i;
      bt_pf_ev_ftp_cli_open_req_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_cli_open_req;
      MSG_LOW( "FTPEvCb: C open req, id=%x m=%x", 
               pEvt->conn_id, pEvt->mode, 0 );
      pEvCb = OEMBTExtFTP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = pEvt->conn_id;
        pEvCb->mode     = pEvt->mode;
        for ( i=0; i < (pEvt->name_len) && i < (sizeof(pEvCb->wName)/sizeof(AECHAR)) - 1; i++)
        {
          pEvCb->wName[ i ] = pEvt->name_str[ i ];
        }
        pEvCb->wName[ i ] = 0;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_OpenCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_FTP_CLI_CLOSE_REQ:
    {
      // current operation is complete; close object
      MSG_LOW( "FTPEvCb: C close req, id=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_cli_close_req.conn_id, 0, 0 );
      pEvCb = OEMBTExtFTP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_ftp_cli_close_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_ftp_cli_close_req.handle;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_ftp_cli_close_req.status;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_CloseCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_FTP_CLI_READ_REQ:
    {
      MSG_LOW( "FTPEvCb: C read req, id=%x h=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_cli_read_req.conn_id, 
               ev_msg_ptr->ev_msg.ev_ftp_cli_read_req.handle, 0 );
      pEvCb = OEMBTExtFTP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_ftp_cli_read_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_ftp_cli_read_req.handle;
        pEvCb->maxBytes = ev_msg_ptr->ev_msg.ev_ftp_cli_read_req.max_read;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_ReadCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_FTP_CLI_WRITE_REQ:
    {
      MSG_HIGH( "FTPEvCb: C wr req, id=%x h=%x , size %x ", 
               ev_msg_ptr->ev_msg.ev_ftp_cli_write_req.conn_id, 
               ev_msg_ptr->ev_msg.ev_ftp_cli_write_req.handle, ev_msg_ptr->ev_msg.ev_ftp_cli_write_req.obj_size );
      pEvCb = OEMBTExtFTP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_ftp_cli_write_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_ftp_cli_write_req.handle;
        pEvCb->pSrc     = ev_msg_ptr->ev_msg.ev_ftp_cli_write_req.buffer_ptr;
        pEvCb->maxBytes = ev_msg_ptr->ev_msg.ev_ftp_cli_write_req.buffer_len;
        pMe->dwFileSize = (ev_msg_ptr->ev_msg.ev_ftp_cli_write_req.obj_size != 0) ?
                           ev_msg_ptr->ev_msg.ev_ftp_cli_write_req.obj_size :
                           pMe->dwFileSize; // length header
                                            // may be sent only once. 

        CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_WriteCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_FTP_CLI_SET_FOLDER_REQ:
    {
      AECHAR* pwTmp;
      bt_pf_ev_ftp_cli_set_folder_req_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_cli_set_folder_req;

      MSG_LOW( "FTPEvCb: C set folder req, id=%x n=%x l=%x", 
               pEvt->conn_id, pEvt->create, pEvt->level );

      status = BT_CS_GN_SUCCESS;
      if ( pMe->clientConnID != pEvt->conn_id )
      {
        MSG_ERROR( "FTPEvCb: S set folder req, wrong id %x", 
                   pEvt->conn_id, 0, 0 );
        status = BT_CS_PF_OBEX_NOT_FOUND;
      }
      else if ( (pEvt->folder_name_len > BT_PF_MAX_FILENAME_LEN) ||
                ((pEvt->create != FALSE) && 
                 ((pEvt->folder_name_len == 0) || 
                  (pEvt->folder_name_str == NULL)))
              )
      {
        status = OI_OBEX_BAD_REQUEST;
      }

      if ( status != BT_CS_GN_SUCCESS )
      {
        bt_cmd_pf_ftp_cli_set_folder_done( pMe->appId, pMe->clientConnID,
                                           status );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      pN->uID = AEEBT_FTP_EVT_CD_REQ;
      if ( pEvt->level == 0)
      {
        pN->data.SetPathReq.flag = AEEBT_FTP_SET_PATH_TO_ROOT;
      }
      else if ( (pEvt->folder_name_len == 0) || 
                (pEvt->folder_name_str == NULL) )
      {
        pN->data.SetPathReq.flag = AEEBT_FTP_SET_PATH_TO_PARENT;
      }
      else
      {
        pN->data.SetPathReq.flag = AEEBT_FTP_SET_PATH_TO_FOLDER;
        pN->data.SetPathReq.bCreate = pEvt->create;
        pwTmp = WSTRRCHR( (AECHAR*)pEvt->folder_name_str, DIRECTORY_CHAR );
        if ( pwTmp != NULL )
        {
          pwTmp++;
        }
        else
        {
          pwTmp = (AECHAR*)pEvt->folder_name_str;
        }
        bt_pf_copy_unicode_str( 
          (uint16*)pN->data.SetPathReq.wName,
          (uint16*)pwTmp,
          BT_PF_MAX_FILENAME_LEN,
          NULL );
      }
      break;
    }

    // ----------------------------------------------------------------------
    // server events
    // ----------------------------------------------------------------------
    case BT_EV_PF_FTP_SRV_CON_IND:
    {
      MSG_LOW( "FTPEvCb: S con ind, id=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_srv_con_ind.conn_id, 0, 0 );

      pMe->serverConnID = ev_msg_ptr->ev_msg.ev_ftp_srv_con_ind.conn_id;
      pMe->BDAddr = 
        *((AEEBTBDAddr*)&ev_msg_ptr->ev_msg.ev_ftp_srv_con_ind.bd_addr);

      pN->uID = AEEBT_FTP_EVT_CONN_REQ;
      pN->data.ConnReq.bdAddr        = pMe->BDAddr;
      pN->data.ConnReq.bUnauthorized = 
        ev_msg_ptr->ev_msg.ev_ftp_srv_con_ind.unauthorized;
      if ( ev_msg_ptr->ev_msg.ev_ftp_srv_con_ind.user_id_len > 0 )
      {
        AEEBT_TO_WSTR( (char*)ev_msg_ptr->ev_msg.ev_ftp_srv_con_ind.user_id,
                   pN->data.ConnReq.wUserID, sizeof(pN->data.ConnReq.wUserID) );
      }
      else
      {
        pN->data.ConnReq.wUserID[ 0 ] = 0;  // NULL string
      }
      break;
    }
    case BT_EV_PF_FTP_SRV_DCN_IND:
    {
      MSG_LOW( "FTPEvCb: S dcn req, id=%x h=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_srv_dcn_ind.conn_id, 0, 0 );

      pEvCb = OEMBTExtFTP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_ftp_srv_dcn_ind.conn_id;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_DisconnectCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_FTP_SRV_BROWSE_REQ:
    {
      bt_pf_ev_ftp_srv_browse_req_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_srv_browse_req;
      MSG_LOW( "FTPEvCb: S browse req, id=%x l=%x", 
               pEvt->conn_id, pEvt->folder_name_len, 0 );
      if ( pMe->serverConnID == pEvt->conn_id )
      {
        pN->uID = AEEBT_FTP_EVT_LS_REQ;
        bt_pf_copy_unicode_str( 
          (uint16*)pN->data.AccessReq.wName,
          (uint16*)pEvt->folder_name_str ,
          BT_PF_MAX_FILENAME_LEN,
          NULL );
      }
      else
      {
        bt_cmd_pf_ftp_srv_browse_done( pMe->appId, pMe->serverConnID, 
                                       (bt_pf_ftp_handle_type) NULL ,
                                       0, BT_CS_PF_OBEX_NOT_FOUND );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_FTP_SRV_OPEN_REQ:
    {
      uint8 i;
      bt_pf_ev_ftp_srv_open_req_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_srv_open_req;

      MSG_LOW( "FTPEvCb: S open req, id=%x m=%x", 
               pEvt->conn_id, pEvt->mode, 0 );

      if ( pMe->serverConnID != pEvt->conn_id )
      {
        MSG_ERROR( "FTPEvCb: S open req, wrong conn id=%x", 
                   pEvt->conn_id, 0, 0 );
        bt_cmd_pf_ftp_srv_open_done( 
          pMe->appId, pMe->serverConnID, (bt_pf_ftp_handle_type)NULL, 0,
          BT_CS_PF_OBEX_NOT_FOUND );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      pEvCb = OEMBTExtFTP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = pEvt->conn_id;
        pEvCb->mode     = pEvt->mode;
        pEvCb->maxBytes = pEvt->obj_size ;
        pEvCb->nameLen  = pEvt->name_len ;

        for ( i=0; i < (pEvt->name_len) && i < (sizeof(pEvCb->wName)/sizeof(AECHAR)); i++)
        {
          pEvCb->wName[ i ] = pEvt->name_str[ i ];
        }

        CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_SrvOpenCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }

    case BT_EV_PF_FTP_SRV_CLOSE_REQ:
    {
      // current operation is complete; close object
      MSG_LOW( "FTPEvCb: S close req, id=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_srv_close_req.conn_id, 0, 0 );
      pEvCb = OEMBTExtFTP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_ftp_srv_close_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_ftp_srv_close_req.handle;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_ftp_srv_close_req.status;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_CloseCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_FTP_SRV_READ_REQ:
    {
      // driver requesting the next chunk of data from the object client
      // is trying to get
      MSG_LOW( "FTPEvCb: S read req, id=%x h=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_srv_read_req.conn_id, 
               ev_msg_ptr->ev_msg.ev_ftp_srv_read_req.handle, 0 );
      pEvCb = OEMBTExtFTP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_ftp_srv_read_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_ftp_srv_read_req.handle;
        pEvCb->maxBytes = ev_msg_ptr->ev_msg.ev_ftp_srv_read_req.max_read;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_ReadCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_FTP_SRV_WRITE_REQ:
    {
      // driver giving the next chunk of data from the object client
      // is pushing
      MSG_LOW( "FTPEvCb: S wr req, id=%x h=%x", 
               ev_msg_ptr->ev_msg.ev_ftp_srv_write_req.conn_id, 
               ev_msg_ptr->ev_msg.ev_ftp_srv_write_req.handle, 0 );
      pEvCb = OEMBTExtFTP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_ftp_srv_write_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_ftp_srv_write_req.handle;
        pEvCb->pSrc     = ev_msg_ptr->ev_msg.ev_ftp_srv_write_req.buffer_ptr;
        pEvCb->maxBytes = ev_msg_ptr->ev_msg.ev_ftp_srv_write_req.buffer_len;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtFTP_WriteCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_FTP_SRV_DELETE_REQ:
    {
      bt_pf_ev_ftp_srv_delete_req_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_ftp_srv_delete_req;

      MSG_LOW( "FTPEvCb: S del req, id=%x", pEvt->conn_id, 0, 0 );

      if ( pMe->serverConnID != pEvt->conn_id )
      {
        MSG_ERROR( "FTPEvCb: S delete req, wrong conn id=%x", 
                   pEvt->conn_id, 0, 0 );
        bt_cmd_pf_ftp_srv_delete_done( pMe->appId, pMe->serverConnID,
                                       BT_CS_PF_OBEX_NOT_FOUND );
      }
      else
      {
        pN->uID = AEEBT_FTP_EVT_DELETE_REQ;
        bt_pf_copy_unicode_str( 
          (uint16*)pN->data.AccessReq.wName,
          (uint16*)pEvt->name_str,
          BT_PF_MAX_FILENAME_LEN,
          NULL );
      }
      break;
    }
    case BT_EV_PF_FTP_SRV_SET_FOLDER_REQ:
    {
      bt_pf_ev_ftp_srv_set_folder_req_type* pEv =
        &ev_msg_ptr->ev_msg.ev_ftp_srv_set_folder_req;

      MSG_LOW( "FTPEvCb: S set folder req, id=%x n=%x l=%x", 
               pEv->conn_id, pEv->create, pEv->level );

      status = BT_CS_GN_SUCCESS;
      if ( pMe->serverConnID != pEv->conn_id )
      {
        MSG_ERROR( "FTPEvCb: S set folder req, wrong id %x", 
                   pEv->conn_id, 0, 0 );
        status = BT_CS_PF_OBEX_NOT_FOUND;
      }
      else if ( (pEv->folder_name_len > BT_PF_MAX_FILENAME_LEN) ||
                ((pEv->create != FALSE) && 
                 ((pEv->folder_name_len == 0) || 
                  (pEv->folder_name_str == NULL)))
              )
      {
        status = OI_OBEX_BAD_REQUEST;
      }

      if ( status != BT_CS_GN_SUCCESS )
      {
        bt_cmd_pf_ftp_srv_set_folder_done( pMe->appId, pMe->serverConnID,
                                           status );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      pN->uID = AEEBT_FTP_EVT_CD_REQ;
      if ( pEv->level == 0)
      {
        pN->data.SetPathReq.flag = AEEBT_FTP_SET_PATH_TO_ROOT;
      }
      else if ( (pEv->folder_name_len == 0) || 
                (pEv->folder_name_str == NULL) )
      {
        pN->data.SetPathReq.flag = AEEBT_FTP_SET_PATH_TO_PARENT;
      }
      else
      {
        pN->data.SetPathReq.flag = AEEBT_FTP_SET_PATH_TO_FOLDER;
        pN->data.SetPathReq.bCreate = pEv->create;
        bt_pf_copy_unicode_str( 
          (uint16*)pN->data.SetPathReq.wName,
          (uint16*)pEv->folder_name_str,
          BT_PF_MAX_FILENAME_LEN,
          NULL );
      }
      break;
    }
    default:
    {
      MSG_ERROR( "FTPEvCb - unexpect event %x", 
                 ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_FTP );
  IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
}


// This API gets the value of the Authentication flag from the FTP menu
// if it is an FTP client
// This API as of now is not exposed to the APP writers.
int OEMBTExtFTP_AuthData( IBTExtFTP *pParent,
                          const AEEBTFTPRegistration* pReg )
{
    OEMBTExtFTPobj_t*   pMe;
    MSG_LOW( "OEMBTExtFTP_AuthData",
                0, 0, 0 );
    if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT,
                        (OEMINSTANCE*)&pMe ) != TRUE )
    {
      return EFAILED;
    }

    if ( pReg == NULL )
    {
      return EBADPARM;
    }
    pMe->auth = pReg->auth;
    return SUCCESS;
}

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_FTP)

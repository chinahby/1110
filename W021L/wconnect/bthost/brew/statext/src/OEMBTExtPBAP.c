/*===========================================================================

FILE:      OEMBTExtPBAP.c

SERVICES:  BlueTooth Phone Book Access Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Phone Book Access 
           Profile.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2004-2008 by QUALCOMM Incorporated. 
                All Rights Reserved.
            Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtPBAP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
$Author: deepikas $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2008-10-07   pn  Replaced deprecated functions.

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_PBAP)
#include "bt.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "oi_obexspec.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtPBAP.h"

//==========================================================================
//   Macro definitions
//==========================================================================
#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
#define PBAP_DATA_BUFFER_SIZE   AEEBT_OBEX_BUFFER_SIZE
#else
#define PBAP_DATA_BUFFER_SIZE   32757
#endif /* FEATURE_BT_REDUCE_MEMORY_USAGE */
#else
#define PBAP_DATA_BUFFER_SIZE   4266
#endif /* FEATURE_BT_SOC */

#define NAME_ONLY( f )  \
  ((WSTRLEN( f ) > 0)? (f): NULL) 

//==========================================================================
//   Type definitions
//==========================================================================

typedef struct OEMBTExtPBAPobj_struct
{
  ACONTEXT*                   pac;
  IBTExtNotifier*             pNotifier;

  bt_app_id_type              appId;
  AEEBTBDAddr                 BDAddr;         // remote device's

  IShell*                     pIShell;

  IFile*                      pFile;
  FileInfo                    fileInfo;
  AECHAR                      wObjName[ AEEBT_MAX_FILE_NAME + 1 ];

  uint16                      maxListCount;

  uint32                      bytesSent;
  uint32                      bytesRcvd;

  uint16                      clientConnID;
  uint16                      serverConnID;  

  char*                       pDataBuffer;
  AEEBTPBAPbyteSeqType        data;   

  boolean                     bIsServer;
  boolean                     bDoDeregister; 

  AEECallback                 cb;

  struct OEMBTExtPBAPobj_struct* pNextPBAP;  // linked into mgr
} OEMBTExtPBAPobj_t;

typedef struct
{
  boolean                     bInUse;
  boolean                     final;
  bt_event_type               event;
  bt_app_id_type              appId;
  uint16                      connId;

  uint8*                      pData;
  uint16                      dataLen;

  uint16                      phoneBookSize;
  uint8                       newMissedCalls;

  uint16                      state;
  bt_cmd_status_type          status;
} OEMBTExtPBAP_EvCb;

// one of these
typedef struct
{
  OEMBTExtPBAPobj_t* pNextPBAP;    // singly linked list of PBAPs
  OEMBTExtPBAP_EvCb  FreeEvCb[ 10 ];
} OEMBTExtPBAP_Mgr;

//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static int  OEMBTExtPBAP_Deregister( OEMBTExtPBAPobj_t* pParent );
static int  OEMBTExtPBAP_RequestFailed(OEMBTExtPBAPobj_t* pMe,
                                       bt_pf_cmd_type cmd, 
                                       AEEBTError status);
static uint16 OEMBTExtPBAP_MapCmdStatus(AEEBTError status);
static int  OEMBTExtPBAP_CheckCmdStatus( bt_cmd_status_type stat );
static void OEMBTExtPBAP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);
static OEMBTExtPBAP_EvCb* OEMBTExtPBAP_GetFreeEvCb( void );

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtPBAP_Mgr        gMgr;               // manager object

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtPBAP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtPBAP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtPBAP_Init(IBTExtPBAP* pParent)
{
  OEMBTExtPBAPobj_t* pMe;

  // allocate pMe
  pMe = MALLOCREC(OEMBTExtPBAPobj_t);
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
  pMe->appId = bt_cmd_ec_get_app_id_and_register(OEMBTExtPBAP_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    FREE( pMe );
    return EFAILED;
  }

  if ( ( pMe->pDataBuffer = MALLOC( PBAP_DATA_BUFFER_SIZE ) ) == NULL )
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

  if ( gMgr.pNextPBAP == NULL ) // first time?
  {
    uint8 i;
    for ( i = 0; i < (sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++ )
    {
      gMgr.FreeEvCb[ i ].bInUse = FALSE;
    }
  }

  pMe->pIShell = pParent->m_pIShell;

  // link into mgr
  pMe->pNextPBAP = gMgr.pNextPBAP;
  gMgr.pNextPBAP = pMe;

  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtPBAP_Release( IBTExtPBAP* pParent )
{
  OEMBTExtPBAPobj_t*  pMe;
  OEMBTExtPBAPobj_t** ppc;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) == TRUE )
  {
    // unlink from mgr
    for (ppc = &gMgr.pNextPBAP; *ppc != NULL; ppc = &(*ppc)->pNextPBAP)
    {
      if ((*ppc)->appId == pMe->appId)
      {
        *ppc = pMe->pNextPBAP;
        pMe->pNextPBAP = NULL;
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

// IBTEXTPBAP_RegisterServer (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_RegisterServer( 
  IBTExtPBAP*             pParent, 
  const char*             pSrvName,
  AEEBTPBAPRepositories   supRepositories,
  AEEBTPBAPAuthentication auth
)
{
  bt_cmd_status_type stat;
  OEMBTExtPBAPobj_t   *pMe;

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

  stat = bt_cmd_pf_pbap_srv_register( pMe->appId,  
                                      (char*) pSrvName,
                                      supRepositories,
                                      (bt_pf_goep_srv_auth_type) auth );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_DeregisterServer (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_DeregisterServer( IBTExtPBAP* pParent )
{
  bt_cmd_status_type          stat;
  OEMBTExtPBAPobj_t            *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pMe->serverConnID != BT_PF_PBAP_NO_CONN_ID ) {
    // We have a connection up.  Tear it down.
    stat = bt_cmd_pf_pbap_srv_force_disconnect( pMe->appId, 
                                                pMe->serverConnID );
    if ( stat == BT_CS_GN_PENDING )
    {
      pMe->bDoDeregister = TRUE;
      stat = BT_CS_GN_SUCCESS;
    }
  }
  else
  {
    return OEMBTExtPBAP_Deregister( pMe );
  }

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_Authenticate (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_Authenticate( 
  IBTExtPBAP*          pParent,
  const AECHAR*        pwUserID,
  const AECHAR*        pwPIN
)
{
  bt_cmd_status_type  stat;
  OEMBTExtPBAPobj_t    *pMe;

  char szUserID[ AEEBT_MAX_USERID_LEN + 1 ];
  char szPIN[ AEEBT_MAX_PASSWORD_LEN + 1 ];

  uint8 userIDLen = 0;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pwUserID != NULL )
  {
    AEEBT_FROM_WSTR( pwUserID, szUserID, sizeof(szUserID) );
    userIDLen = STRLEN( szUserID );
  }
  AEEBT_FROM_WSTR( pwPIN, szPIN, sizeof(szPIN) );

  if ( pMe->bIsServer == FALSE )
  {
    stat = bt_cmd_pf_pbap_cli_auth_response( pMe->appId, 
                                             pMe->clientConnID,
                                             (byte*)szUserID, 
                                             userIDLen, szPIN );
  }
  else
  {
    stat = bt_cmd_pf_pbap_srv_auth_response( pMe->appId, 
                                             pMe->serverConnID,
                                             szPIN );
  }

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_AcceptConnection (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_AcceptConnection( 
  IBTExtPBAP*          pParent,
  boolean              bAccept
)
{
  bt_cmd_status_type  stat;
  OEMBTExtPBAPobj_t    *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_pbap_srv_accept_connect( pMe->appId, 
                                            pMe->serverConnID,
                                            bAccept );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_Connect (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_Connect( 
  IBTExtPBAP*             pParent, 
  const AEEBTBDAddr*      pBDAddr, 
  uint8                   uChannelNumber,
  boolean                 bAuth
)
{
  bt_cmd_status_type  stat;
  OEMBTExtPBAPobj_t    *pMe;
  bt_pf_realm_info_type realmInfo;
  boolean userId = TRUE;
  boolean accessControl = TRUE;  

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

  MSG_LOW("Calling bt_obex_client_authenticate. bAuth = %d",
           bAuth, 0, 0 );
  if(bAuth)
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

  stat = bt_cmd_pf_pbap_cli_connect( pMe->appId, 
                                     (bt_bd_addr_type*) pBDAddr,
                                     uChannelNumber  );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_Disconnect (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_Disconnect( 
  IBTExtPBAP*             pParent
)
{
  bt_cmd_status_type  stat;
  OEMBTExtPBAPobj_t *pMe;

  uint16 connID = 0;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_pbap_cli_disconnect( pMe->appId, connID );
 
  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_RegisterClient (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_RegisterClient( 
  IBTExtPBAP*             pParent, 
  const char*             pCliName
)
{
  bt_cmd_status_type stat;
  OEMBTExtPBAPobj_t   *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pCliName == NULL )
  {
    return EBADPARM;
  }

  stat = bt_cmd_pf_pbap_cli_register( pMe->appId,  
                                      (char*) pCliName );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_DeregisterClient (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_DeregisterClient( IBTExtPBAP* pParent )
{
  bt_cmd_status_type          stat;
  OEMBTExtPBAPobj_t            *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_pbap_cli_deregister( pMe->appId );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_Abort (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_Abort( IBTExtPBAP *pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtPBAPobj_t *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_pbap_cli_abort( pMe->appId, pMe->clientConnID );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_PullPhoneBook (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullPhoneBook( 
  IBTExtPBAP*                pParent, 
  const AECHAR*              pwObjName,
  const AEEBTPBAPAttribMask* pAttribMask,
  const AEEBTPBAPFormat*     pFormat,
  const uint16*              pMaxListCount,
  const uint16*              pListStartOffset
)
{
  bt_cmd_status_type  stat;
  OEMBTExtPBAPobj_t *pMe;
  IFileMgr* pIFileMgr       = NULL;

  static char sFileName[ AEEBT_MAX_FILE_NAME + 1];
  static char sFolderName[ AEEBT_MAX_FILE_NAME + 1 ];

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pwObjName == NULL ) 
  {
    return EBADPARM;
  }

  WSTRLCPY( pMe->wObjName, pwObjName, ARR_SIZE(pMe->wObjName) ); 
  AEEBT_FROM_WSTR( pwObjName, sFileName, sizeof(sFileName) );

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  // Create telecom folder, if it does not exist
  if ( IFILEMGR_Test( pIFileMgr, AEEBT_PBAP_FOLDER_NAME_TELECOM ) != SUCCESS )
  {
    if ( IFILEMGR_MkDir( pIFileMgr, AEEBT_PBAP_FOLDER_NAME_TELECOM ) != SUCCESS )
    {
      return EFAILED;
    }
  }
  
  // Create SIM1/telecom folder, if it does not exist
  STRLCPY( sFolderName, AEEBT_PBAP_FOLDER_NAME_SIM1, sizeof(sFolderName) );
  STRLCAT( sFolderName, DIRECTORY_STR, sizeof(sFolderName) );
  STRLCAT( sFolderName, AEEBT_PBAP_FOLDER_NAME_TELECOM, sizeof(sFolderName) );
  if ( IFILEMGR_Test( pIFileMgr, AEEBT_PBAP_FOLDER_NAME_SIM1 ) != SUCCESS )
  {
    if ( IFILEMGR_MkDir( pIFileMgr, AEEBT_PBAP_FOLDER_NAME_SIM1 ) != SUCCESS )
    {
      return EFAILED;
    }
    else if ( IFILEMGR_MkDir( pIFileMgr, sFolderName ) != SUCCESS ) 
    {
      return EFAILED;
    }
  }

  // Create object file in the appropriate folder.  Remove it first, if present 
  IFILEMGR_Remove( pIFileMgr, sFileName );
  if ( (pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                        sFileName, 
                                        _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return EFAILED;
  }

  stat = bt_cmd_pf_pbap_cli_pull_phone_book( pMe->appId, 
                                             pMe->clientConnID,
                                             pMe->wObjName,
                                             (uint64*) pAttribMask,
                                             (uint8*)  pFormat,
                                             (uint16*) pMaxListCount,
                                             (uint16*) pListStartOffset );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_SetPhoneBook (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_SetPhoneBook( 
  IBTExtPBAP*             pParent, 
  const AECHAR*           pwFolder,
  AEEBTPBAPSetPathFlag    flag 
)
{
  bt_cmd_status_type           stat;
  OEMBTExtPBAPobj_t*           pMe;
  bt_pf_pbap_cli_set_path_type set_path_ctrl;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  if ( (pwFolder == NULL) && (flag == AEEBT_PBAP_SET_PATH_TO_FOLDER) )
  {
    // Folder name is required 
    return EBADPARM;
  }
  else if ( (pwFolder != NULL) && (flag != AEEBT_PBAP_SET_PATH_TO_FOLDER) )
  {
    // pwFolder should be NULL 
    return EBADPARM;
  }

  switch ( flag )
  {
    case AEEBT_PBAP_SET_PATH_TO_FOLDER:
      set_path_ctrl = BT_PF_PBAP_CLI_SET_PATH_TO_FOLDER;
      break;
    case AEEBT_PBAP_SET_PATH_TO_PARENT:
      set_path_ctrl = BT_PF_PBAP_CLI_SET_PATH_TO_PARENT;
      break;
    case AEEBT_PBAP_SET_PATH_TO_ROOT:
      set_path_ctrl = BT_PF_PBAP_CLI_SET_PATH_TO_ROOT;
      break;
    default:
      return EBADPARM;
  }
  stat = bt_cmd_pf_pbap_cli_set_phone_book( pMe->appId, 
                                            pMe->clientConnID,
                                            (AECHAR*)pwFolder, 
                                            set_path_ctrl );
  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_PullvCardListing (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullvCardListing( 
  IBTExtPBAP*                   pParent, 
  const AECHAR*                 pwFolder,
  const AEEBTPBAPOrder*         pSearchOrder,
  const AECHAR*                 pwSearchValue,
  const AEEBTPBAPSearchAttrib*  pSearchAttrib,
  const uint16*                 pMaxListCount,
  const uint16*                 pListStartOffset 
)
{
  bt_cmd_status_type  stat;
  OEMBTExtPBAPobj_t *pMe;
  IFileMgr* pIFileMgr       = NULL;

  char       sSearchValue[ AEEBT_MAX_MIME_TYPE_LEN ];

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pwFolder != NULL ) 
  {
    WSTRLCPY( pMe->wObjName, pwFolder, ARR_SIZE(pMe->wObjName) ); 
  }
  else
  {
    pMe->wObjName[0] = '\0';
  }

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }
  IFILEMGR_Remove( pIFileMgr, AEEBT_PBAP_VCARD_LISTING_XML_FILE );
  if ( (pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                        AEEBT_PBAP_VCARD_LISTING_XML_FILE, 
                                        _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return EFAILED;
  }

  AEEBT_FROM_WSTR( pwSearchValue, sSearchValue, AEEBT_MAX_MIME_TYPE_LEN );

  stat = bt_cmd_pf_pbap_cli_pull_vcard_listing( pMe->appId, 
                                                pMe->clientConnID,
                                                NAME_ONLY( pMe->wObjName ),
                                                (uint8*) pSearchOrder,
                                                sSearchValue,
                                                (uint8*) pSearchAttrib,
                                                (uint16*) pMaxListCount,
                                                (uint16*) pListStartOffset );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_PullvCardEntry (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullvCardEntry( 
  IBTExtPBAP*                  pParent, 
  const AECHAR*                pwName,
  const AEEBTPBAPAttribMask*   pAttribMask,
  const AEEBTPBAPFormat*       pFormat 
)
{
  bt_cmd_status_type  stat;
  OEMBTExtPBAPobj_t *pMe;
  IFileMgr* pIFileMgr       = NULL;

  static char sFileName[ AEEBT_MAX_FILE_NAME + 1 ];

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pwName == NULL ) 
  {
    return EBADPARM;
  }

  WSTRCPY(pMe->wObjName, pwName); 
  AEEBT_FROM_WSTR( pwName, sFileName, sizeof(sFileName) );

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  IFILEMGR_Remove( pIFileMgr, sFileName );
  if ( (pMe->pFile = IFILEMGR_OpenFile( pIFileMgr, 
                                        sFileName, 
                                        _OFM_CREATE )) == NULL )
  { 
    IFILEMGR_Release( pIFileMgr );
    return EFAILED;
  }

  stat = bt_cmd_pf_pbap_cli_pull_vcard_entry( pMe->appId, 
                                              pMe->clientConnID,
                                              pMe->wObjName,
                                              (uint64*) pAttribMask,
                                              (uint8*) pFormat );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_PullPhoneBookResponse (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullPhoneBookResponse( 
  IBTExtPBAP*   pParent,
  const AECHAR* pwFileName,
  const uint16* pPhoneBookSize,
  const uint8*  pNewMissedCalls,
  AEEBTError    error 
)
{
  bt_cmd_status_type stat;
  OEMBTExtPBAPobj_t*  pMe;
  uint16             bytesRead = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*          pIFileMgr = NULL;

  char sFileName[ AEEBT_MAX_FILE_NAME + 1 ];

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  // If the PSE was unable to honor the request, notify PCE of failure
  if ( error != AEEBT_ERR_NONE ) 
  {
    stat = (bt_cmd_status_type)OEMBTExtPBAP_RequestFailed( pMe,
                                  BT_PF_CMD_PBAP_SRV_PULL_PHONE_BOOK_RSP, 
                                  error );

    return stat;
  }

  if ( pwFileName == NULL && pMe->maxListCount > 0 ) 
  {
    return EBADPARM;
  }

  if ( pMe->maxListCount > 0 )
  {
    AEEBT_FROM_WSTR(pwFileName, sFileName, sizeof (sFileName));

    ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
    if ( pIFileMgr == NULL) 
    {
      return EFAILED;
    }

    if ( ( pMe->pFile = 
           IFILEMGR_OpenFile( pIFileMgr, sFileName, _OFM_READ ) ) != NULL )
    {
      // get filesize info
      IFILE_GetInfo( pMe->pFile, &pMe->fileInfo );

      bytesRead = PBAP_DATA_BUFFER_SIZE;
      bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
      if ( bytesRead >= pMe->fileInfo.dwSize )
      {
        final  = 1;
        status = BT_CS_GN_SUCCESS;

        IFILE_Release( pMe->pFile );
        pMe->pFile = NULL;
      }
      pMe->data.AEEBTPBAPdata    = (byte*) pMe->pDataBuffer;
    }
    else
    {
      MSG_ERROR( "PullPhoneBookResponse: file open failed", 0, 0, 0 );

      pMe->data.AEEBTPBAPdata = NULL;
      final  = 1;
      status = BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    // Case where only PhoneBookSize info is sent
    pMe->data.AEEBTPBAPdata = NULL;
    final  = 1;
    status = BT_CS_GN_SUCCESS;
  }

  pMe->data.AEEBTPBAPdataLen = bytesRead;
  pMe->bytesSent             = bytesRead;

  if ( pIFileMgr ) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  stat = bt_cmd_pf_pbap_srv_pull_phone_book_response(
                                       pMe->appId, 
                                       pMe->serverConnID,
                                       (bt_pf_byte_seq_type*) &pMe->data,
                                       (uint16*) pPhoneBookSize,
                                       (uint8*) pNewMissedCalls,
                                       final,
                                       status );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXTPBAP_SetPhoneBookResponse (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_SetPhoneBookResponse( 
  IBTExtPBAP* pParent, 
  AEEBTError  status
)
{
  OEMBTExtPBAPobj_t*  pMe;
  bt_cmd_status_type  stat;

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_pbap_srv_set_phone_book_response(
                    pMe->appId, 
                    pMe->serverConnID,
                    (bt_cmd_status_type)OEMBTExtPBAP_MapCmdStatus(status) );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}     

// IBTEXTPBAP_PullvCardListingResponse (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullvCardListingResponse( 
  IBTExtPBAP*   pParent, 
  const AECHAR* pwFileName,
  const uint16* pPhoneBookSize,
  const uint8*  pNewMissedCalls,
  AEEBTError    error
)
{
  bt_cmd_status_type stat;
  OEMBTExtPBAPobj_t* pMe;
  uint16             bytesRead = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*          pIFileMgr = NULL;

  char sFileName[ AEEBT_MAX_FILE_NAME + 1 ];

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  // If the PSE was unable to honor the request, notify PCE of failure
  if ( error != AEEBT_ERR_NONE ) 
  {
    stat = (bt_cmd_status_type)
      OEMBTExtPBAP_RequestFailed( pMe,
                                  BT_PF_CMD_PBAP_SRV_PULL_VCARD_LISTING_RSP, 
                                  error );

    return stat;
  }

  if ( pwFileName == NULL && pMe->maxListCount > 0 ) 
  {
    return EBADPARM;
  }

  if ( pMe->maxListCount > 0 ) 
  {
    AEEBT_FROM_WSTR(pwFileName, sFileName, sizeof (sFileName));

    ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
    if ( pIFileMgr == NULL) 
    {
      return EFAILED;
    }

    if ( ( pMe->pFile = 
           IFILEMGR_OpenFile( pIFileMgr, sFileName, _OFM_READ ) ) != NULL )
    {
      // get filesize info
      IFILE_GetInfo( pMe->pFile, &pMe->fileInfo );

      bytesRead = PBAP_DATA_BUFFER_SIZE;
      bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
      if ( bytesRead >= pMe->fileInfo.dwSize )
      {
        final  = 1;
        status = BT_CS_GN_SUCCESS;

        IFILE_Release( pMe->pFile );
        pMe->pFile = NULL;
      }
      pMe->data.AEEBTPBAPdata    = (byte*) pMe->pDataBuffer;
    }
    else
    {
      MSG_ERROR( "PullvCardListingResponse: file open failed", 0, 0, 0 );

      pMe->data.AEEBTPBAPdata = NULL;
      final  = 1;
      status = BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    // Case where only PhoneBookSize info is sent
    pMe->data.AEEBTPBAPdata = NULL;
    
    final  = 1;
    status = BT_CS_GN_SUCCESS;
  }

  pMe->data.AEEBTPBAPdataLen = bytesRead;
  pMe->bytesSent             = bytesRead;

  if ( pIFileMgr ) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  stat = bt_cmd_pf_pbap_srv_pull_vcard_listing_response(
                                       pMe->appId, 
                                       pMe->serverConnID,
                                       (bt_pf_byte_seq_type*) &pMe->data,
                                       (uint16*) pPhoneBookSize,
                                       (uint8*) pNewMissedCalls,
                                       final,
                                       status );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
} 

// IBTEXTPBAP_PullvCardEntryResponse (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullvCardEntryResponse( 
  IBTExtPBAP*   pParent, 
  const AECHAR* pwFileName,
  AEEBTError    error
)
{
  bt_cmd_status_type stat;
  OEMBTExtPBAPobj_t*  pMe;
  uint16             bytesRead = 0;
  boolean            final     = 0;
  bt_cmd_status_type status    = BT_CS_PF_OBEX_CONTINUE;
  IFileMgr*          pIFileMgr = NULL;

  char sFileName[ AEEBT_MAX_FILE_NAME + 1 ];

  if( AEEHandle_From( &gOEMBTExtHandleList, 
                      pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  // If the PSE was unable to honor the request, notify PCE of failure
  if ( error != AEEBT_ERR_NONE ) 
  {
    stat = (bt_cmd_status_type)
      OEMBTExtPBAP_RequestFailed( pMe,
                                  BT_PF_CMD_PBAP_SRV_PULL_VCARD_ENTRY_RSP, 
                                  error );

    return stat;
  }

  if ( pwFileName == NULL ) 
  {
    return EBADPARM;
  }

  AEEBT_FROM_WSTR(pwFileName, sFileName, sizeof (sFileName));

  ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
  if ( pIFileMgr == NULL) 
  {
    return EFAILED;
  }

  if ( ( pMe->pFile = 
         IFILEMGR_OpenFile( pIFileMgr, sFileName, _OFM_READ ) ) != NULL )
  {
    // get filesize info
    IFILE_GetInfo( pMe->pFile, &pMe->fileInfo );

    bytesRead = PBAP_DATA_BUFFER_SIZE;
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
    MSG_ERROR( "PullvCardEntryResponse: file open failed", 0, 0, 0 );

    pMe->pDataBuffer = NULL;
    final  = 1;
    status = BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR;
  }

  pMe->data.AEEBTPBAPdata    = (byte*) pMe->pDataBuffer;
  pMe->data.AEEBTPBAPdataLen = bytesRead;
  pMe->bytesSent             = bytesRead;

  if ( pIFileMgr ) 
  {
    IFILEMGR_Release( pIFileMgr );
  }

  stat = bt_cmd_pf_pbap_srv_pull_vcard_entry_response(
                                       pMe->appId, 
                                       pMe->serverConnID,
                                       (bt_pf_byte_seq_type*) &pMe->data,
                                       final,
                                       status );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
} 

//==========================================================================
//   static helper functions
//==========================================================================
// IBTEXTPBAP_Deregister
static int OEMBTExtPBAP_Deregister( OEMBTExtPBAPobj_t* pMe )
{
  bt_cmd_status_type          stat;

  if( !pMe )
  {
    return EFAILED;
  }

  stat = bt_cmd_pf_pbap_srv_deregister( pMe->appId );

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

// IBTEXtPBAP_MapCmdStatus
static uint16 OEMBTExtPBAP_MapCmdStatus( AEEBTError status )
{
  switch ( status ) 
  {
    case AEEBT_ERR_NONE:
      return BT_CS_GN_SUCCESS;

    case AEEBT_ERR_OBEX_BAD_REQUEST:
      return BT_CS_PF_OBEX_BAD_REQUEST;

    case AEEBT_ERR_OBEX_NOT_IMPLEMENTED:
      return BT_CS_PF_OBEX_NOT_IMPLEMENTED;

    case AEEBT_ERR_OBEX_UNAUTHORIZED:
      return BT_CS_PF_OBEX_UNAUTHORIZED;

    case AEEBT_ERR_OBEX_PRECONDITION_FAILED:
      return BT_CS_PF_OBEX_PRECONDITION_FAILED;

    case AEEBT_ERR_OBEX_NOT_FOUND:
      return BT_CS_PF_OBEX_NOT_FOUND;

    case AEEBT_ERR_OBEX_NOT_ACCEPTABLE:
      return BT_CS_PF_OBEX_VALUE_NOT_ACCEPTABLE;

    case AEEBT_ERR_OBEX_SERVICE_UNAVAILABLE:
      return BT_CS_PF_OBEX_SERVICE_UNAVAILABLE;

    case AEEBT_ERR_OBEX_FORBIDDEN:
      return BT_CS_PF_OBEX_ACCESS_DENIED;

    default:
      return BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR;
  }
}

static int OEMBTExtPBAP_RequestFailed( 
  OEMBTExtPBAPobj_t* pMe,
  bt_pf_cmd_type     cmd, 
  AEEBTError         status 
)
{
  boolean            final = 1;
  bt_cmd_status_type stat;

  pMe->data.AEEBTPBAPdata    = NULL;
  pMe->data.AEEBTPBAPdataLen = 0;

  switch ( cmd ) 
  {
    case BT_PF_CMD_PBAP_SRV_PULL_PHONE_BOOK_RSP:
    {
      stat = bt_cmd_pf_pbap_srv_pull_phone_book_response(
                   pMe->appId, 
                   pMe->serverConnID,
                   NULL,
                   NULL,
                   NULL,
                   final,
                   (bt_cmd_status_type)OEMBTExtPBAP_MapCmdStatus(status) );
      break;
    }
    case BT_PF_CMD_PBAP_SRV_PULL_VCARD_LISTING_RSP:
    {
      stat = bt_cmd_pf_pbap_srv_pull_vcard_listing_response(
                   pMe->appId, 
                   pMe->serverConnID,
                   NULL,
                   NULL,
                   NULL,
                   final,
                   (bt_cmd_status_type)OEMBTExtPBAP_MapCmdStatus(status) );
      break;
    }
    case BT_PF_CMD_PBAP_SRV_PULL_VCARD_ENTRY_RSP:
    {
      stat = bt_cmd_pf_pbap_srv_pull_vcard_entry_response(
                   pMe->appId, 
                   pMe->serverConnID,
                   NULL,
                   final,
                   (bt_cmd_status_type)OEMBTExtPBAP_MapCmdStatus(status) );
      break;
    }
    default:
      MSG_ERROR( "RequestFailed: unrecognized cmd=%x", cmd, 0, 0 );
      return EFAILED;
  }

  return OEMBTExtPBAP_CheckCmdStatus( stat );
}

static int OEMBTExtPBAP_CheckCmdStatus( bt_cmd_status_type stat )
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

static OEMBTExtPBAPobj_t* OEMBTExtPBAP_FindMe(bt_app_id_type appId)
{
  OEMBTExtPBAPobj_t* pMe;

  for (pMe = gMgr.pNextPBAP; pMe != NULL; pMe = pMe->pNextPBAP)
  {
    if (pMe->appId == appId)
    {
      return pMe;
    }
  }
  return NULL;
}

static void OEMBTExtPBAP_PullPhoneBookClientCb( OEMBTExtPBAP_EvCb* pEvCb )
{
  OEMBTExtPBAPobj_t* pMe  = OEMBTExtPBAP_FindMe( pEvCb->appId );
  uint32 bytesWritten    = 0;
  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "PullPhoneBookClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }

  if ( pMe->pFile == NULL ) 
  {
    MSG_ERROR( "PullPhoneBookClientCb: pMe->pFile is NULL!", 0, 0, 0 );
    return;
  }

  if ( pEvCb->dataLen > 0 ) 
  {
    if ( (bytesWritten = IFILE_Write( pMe->pFile, 
                                      pEvCb->pData, 
                                      pEvCb->dataLen )) != pEvCb->dataLen )
    {
      MSG_ERROR( "PullPhoneBookClientCb: failed to write to obj", 0, 0, 0 );

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;

      return;
    }
 
    pMe->bytesRcvd += bytesWritten;
  }
  
  // If this is the last chunk of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "PBAP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID              = AEEBT_PBAP_EVT_PULL_PHONE_BOOK_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );

      pN->data.PBAPObject.phoneBookSize = pEvCb->phoneBookSize;
      pN->data.PBAPObject.newMissedCalls = pEvCb->newMissedCalls;
      
      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_PBAP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_pbap_cli_pull_phone_book( 
                         pMe->appId,
                         pMe->clientConnID,
                         pMe->wObjName,
                         NULL,
                         NULL,
                         NULL,
                         NULL );
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtPBAP_PullvCardListingClientCb( OEMBTExtPBAP_EvCb* pEvCb )
{
  OEMBTExtPBAPobj_t* pMe  = OEMBTExtPBAP_FindMe( pEvCb->appId );
  uint32 bytesWritten    = 0;
  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "PullvCardListingClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }

  if ( pMe->pFile == NULL ) 
  {
    MSG_ERROR( "PullvCardListingClientCb: pMe->pFile is NULL!", 0, 0, 0 );
    return;
  }

  if ( pEvCb->dataLen > 0 ) 
  {
    if ( (bytesWritten = IFILE_Write( pMe->pFile, 
                                      pEvCb->pData, 
                                      pEvCb->dataLen )) != pEvCb->dataLen )
    {
      MSG_ERROR( "PullvCardListingClientCb: failed to write to obj", 0, 0, 0 );

      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;

      return;
    } 
 
    pMe->bytesRcvd += bytesWritten;
  }
  
  // If this is the last chunk of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "PBAP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID              = AEEBT_PBAP_EVT_PULL_VCARD_LISTING_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );

      pN->data.PBAPObject.phoneBookSize = pEvCb->phoneBookSize;
      pN->data.PBAPObject.newMissedCalls = pEvCb->newMissedCalls;
      
      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_PBAP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_pbap_cli_pull_vcard_listing( 
                         pMe->appId,
                         pMe->clientConnID,
                         NAME_ONLY( pMe->wObjName ),
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL );
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtPBAP_PullvCardEntryClientCb( OEMBTExtPBAP_EvCb* pEvCb )
{
  OEMBTExtPBAPobj_t* pMe  = OEMBTExtPBAP_FindMe( pEvCb->appId );
  uint32 bytesWritten    = 0;
  AEEBTNotification*  pN = NULL;

  if ( pMe == NULL )
  {
    // how did we get here?
    return;
  }

  if ( pMe->clientConnID != pEvCb->connId )
  {
    MSG_ERROR( "PullvCardEntryClientCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    return;
  }

  if ( pMe->pFile == NULL ) 
  {
    MSG_ERROR( "PullvCardEntryClientCb: pMe->pFile is NULL!", 0, 0, 0 );
    return;
  }

  if ( (bytesWritten = IFILE_Write( pMe->pFile, 
                                    pEvCb->pData, 
                                    pEvCb->dataLen )) != pEvCb->dataLen )
  {
    MSG_ERROR( "PullvCardEntryClientCb: failed to write to obj", 0, 0, 0 );

    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    return;
  }
 
  pMe->bytesRcvd += bytesWritten;
  
  // If this is the last chunk of data, notify App that we are done
  if ( pEvCb->final == 1 || ( pEvCb->status != BT_CS_GN_SUCCESS &&
                              pEvCb->status != BT_CS_PF_OBEX_CONTINUE ) )
  {
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;

    pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
    if( pN == NULL )
    {
      MSG_ERROR( "PBAP - Can't get free not.",
                 0, 0, 0 );
    }
    else
    {
      pN->uID              = AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_DONE;
      pN->data.uError      = OEMBTExt_MapCmdStatus( pEvCb->status );
      
      pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_PBAP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    }
  }
  else
  {
    (void) bt_cmd_pf_pbap_cli_pull_vcard_entry( 
                         pMe->appId,
                         pMe->clientConnID,
                         pMe->wObjName,
                         NULL,
                         NULL );
  }

  pEvCb->bInUse = FALSE;
}

static void OEMBTExtPBAP_PullPhoneBookServerCb( OEMBTExtPBAP_EvCb* pEvCb )
{
  OEMBTExtPBAPobj_t* pMe     = OEMBTExtPBAP_FindMe( pEvCb->appId );
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
    MSG_ERROR( "PullPhoneBookServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else if ( pMe->pFile == NULL ) 
  {
    MSG_ERROR( "PullPhoneBookServerCb: pMe->pFile is NULL!", 0, 0, 0 );
    status = BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR;
  }
  else 
  {
    bytesRead = PBAP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    pMe->bytesSent += bytesRead;
    if ( bytesRead == 0 )
    {
      MSG_ERROR( "PullPhoneBookServerCb: failed to read from obj", 0, 0, 0 );
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

  pMe->data.AEEBTPBAPdataLen = bytesRead;
  pMe->data.AEEBTPBAPdata    = (byte*) pMe->pDataBuffer;

  bt_cmd_pf_pbap_srv_pull_phone_book_response( 
                                pMe->appId, 
                                pMe->serverConnID,
                                (bt_pf_byte_seq_type*) &pMe->data,
                                NULL,
                                NULL,
                                final,
                                status );
}

static void OEMBTExtPBAP_PullvCardListingServerCb( OEMBTExtPBAP_EvCb* pEvCb )
{
  OEMBTExtPBAPobj_t* pMe     = OEMBTExtPBAP_FindMe( pEvCb->appId );
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
    MSG_ERROR( "PullvCardListingServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else if ( pMe->pFile == NULL ) 
  {
    MSG_ERROR( "PullvCardListingServerCb: pMe->pFile is NULL!", 0, 0, 0 );
    status = BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR;
  }
  else 
  {
    bytesRead = PBAP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    pMe->bytesSent += bytesRead;
    if ( bytesRead == 0 )
    {
      MSG_ERROR( "PullvCardListingServerCb: failed to read from obj", 0, 0, 0 );
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

  pMe->data.AEEBTPBAPdataLen = bytesRead;
  pMe->data.AEEBTPBAPdata    = (byte*) pMe->pDataBuffer;

  bt_cmd_pf_pbap_srv_pull_vcard_listing_response( 
                                pMe->appId, 
                                pMe->serverConnID,
                                (bt_pf_byte_seq_type*) &pMe->data,
                                NULL,
                                NULL,
                                final,
                                status );
}

static void OEMBTExtPBAP_PullvCardEntryServerCb( OEMBTExtPBAP_EvCb* pEvCb )
{
  OEMBTExtPBAPobj_t* pMe     = OEMBTExtPBAP_FindMe( pEvCb->appId );
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
    MSG_ERROR( "PullvCardEntryServerCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else if ( pMe->pFile == NULL ) 
  {
    MSG_ERROR( "PullvCardListingServerCb: pMe->pFile is NULL!", 0, 0, 0 );
    status = BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR;
  }
  else 
  {
    bytesRead = PBAP_DATA_BUFFER_SIZE;
    bytesRead = IFILE_Read( pMe->pFile, pMe->pDataBuffer, bytesRead );
    pMe->bytesSent += bytesRead;
    if ( bytesRead == 0 )
    {
      MSG_ERROR( "PullvCardEntryServerCb: failed to read from obj", 0, 0, 0 );
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

  pMe->data.AEEBTPBAPdataLen = bytesRead;
  pMe->data.AEEBTPBAPdata    = (byte*) pMe->pDataBuffer;

  bt_cmd_pf_pbap_srv_pull_vcard_entry_response( 
                                pMe->appId, 
                                pMe->serverConnID,
                                (bt_pf_byte_seq_type*) &pMe->data,
                                final,
                                status );
}

static void OEMBTExtPBAP_DisconnectCb( OEMBTExtPBAP_EvCb* pEvCb )
{
  OEMBTExtPBAPobj_t* pMe = OEMBTExtPBAP_FindMe( pEvCb->appId );
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
        pMe->clientConnID = BT_PF_PBAP_NO_CONN_ID;
      }
      else
      {
        pMe->serverConnID = BT_PF_PBAP_NO_CONN_ID;
      }

      notif->uID = AEEBT_PBAP_EVT_DISCONNECTED;
      notif->data.uError = AEEBT_ERR_NONE;
    
      notif->uID = (notif->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_PBAP );
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
    if ( bt_cmd_pf_pbap_srv_deregister( pMe->appId ) != BT_CS_GN_PENDING )
    {
      MSG_ERROR( "DisconnectCb: S de-reg failed!", 0, 0, 0 );
    }
  }
}

static boolean OEMBTExtPBAP_HandleCmdDone( 
  bt_ev_gn_cmd_done_type* pCmdDn, 
  AEEBTNotification*      pN,
  OEMBTExtPBAPobj_t*       pMe
)
{
  boolean doSendNotif = TRUE;

  MSG_LOW( "PBAP_HandleCmdDone - stat=%x cmd=%x", pCmdDn->cmd_status, pCmdDn->cmd_type, 0 );

  switch ( pCmdDn->cmd_type )
  {
    case BT_PF_CMD_PBAP_SRV_REGISTER:
    {
      pN->uID = AEEBT_PBAP_EVT_SRV_REG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      pMe->serverConnID = BT_PF_PBAP_NO_CONN_ID;
      if ( pN->data.uError == AEEBT_ERR_NONE )
      {
        pMe->bIsServer = TRUE;
      }
      break;
    }
    case BT_PF_CMD_PBAP_SRV_DEREGISTER:
    {
      pN->uID = AEEBT_PBAP_EVT_SRV_DEREG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError != AEEBT_ERR_OPERATION_IN_PROGRESS )
      {
        pMe->bIsServer = FALSE;
      }
      break;
    }
    case BT_PF_CMD_PBAP_CLI_REGISTER:
    {
      pN->uID = AEEBT_PBAP_EVT_CLI_REG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_PBAP_CLI_DEREGISTER:
    {
      pN->uID = AEEBT_PBAP_EVT_CLI_DEREG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      break;
    }
    case BT_PF_CMD_PBAP_CLI_PULL_PHONE_BOOK:
    {
      pN->uID = AEEBT_PBAP_EVT_PULL_PHONE_BOOK_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE )
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_PBAP_CLI_SET_PHONE_BOOK:
    {
      pN->uID = AEEBT_PBAP_EVT_SET_PHONE_BOOK_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE )
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_PBAP_CLI_PULL_VCARD_LISTING:
    {
      pN->uID = AEEBT_PBAP_EVT_PULL_VCARD_LISTING_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE )
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_PBAP_CLI_PULL_VCARD_ENTRY:
    {
      pN->uID = AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_ERR_NONE )
      {
        doSendNotif = FALSE;
      }
      break;
    }
    case BT_PF_CMD_PBAP_CLI_ABORT:
    {
      pN->uID = AEEBT_PBAP_EVT_OP_ABORTED;
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

static OEMBTExtPBAP_EvCb* OEMBTExtPBAP_GetFreeEvCb( void )
{
  uint8 i;
  OEMBTExtPBAP_EvCb* pEvCb = NULL;

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

// called from BT task context; so just queue the event
static void OEMBTExtPBAP_EventCallback( bt_ev_msg_type* ev_msg_ptr )
{
  OEMBTExtPBAPobj_t*  pMe   = OEMBTExtPBAP_FindMe( ev_msg_ptr->ev_hdr.bt_app_id );
  OEMBTExtPBAP_EvCb*  pEvCb = NULL;
  AEEBTNotification* pN     = NULL;
  bt_cmd_status_type status = BT_CS_GN_SUCCESS;

  if ( pMe == NULL )
  {
    return; // probably for an app that no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_ERROR( "PBAPEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }

  pN->data.uError  = AEEBT_ERR_NONE;

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
    {
      if ( ! OEMBTExtPBAP_HandleCmdDone( &ev_msg_ptr->ev_msg.ev_gn_cmd_done,
                                        pN, pMe ) )
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return; // no need to send notification to app
      }
      break;
    }
    case BT_EV_PF_PBAP_CLI_AUTH_REQ:
    {
       bt_pf_ev_pbap_cli_auth_req_type* pEv =
         &ev_msg_ptr->ev_msg.ev_pbap_cli_auth_req;
      MSG_LOW( "PBAPEvCb: C auth req, id=%x st=%x", 
               pEv->conn_id, pEv->user_id_required, 0 );

      pMe->clientConnID        = pEv->conn_id;
      pN->data.PBAPObject.bdAddr          = pMe->BDAddr;
      pN->data.PBAPObject.bUserIDRequired = pEv->user_id_required;
      pN->uID = AEEBT_PBAP_EVT_AUTH_REQ;
      break;
    }
    case BT_EV_PF_PBAP_CLI_CON_CFM:
    {
       bt_pf_ev_pbap_cli_con_cfm_type* pEv =
         &ev_msg_ptr->ev_msg.ev_pbap_cli_con_cfm;
      MSG_LOW( "PBAPEvCb: C con cfm, id=%x stat=%x", 
               pEv->conn_id, pEv->status, 0 );
      if (pEv->status == BT_CS_GN_SUCCESS )
      {
        pMe->clientConnID = pEv->conn_id;
        pN->uID = AEEBT_PBAP_EVT_CONNECTED;
      }
      else
      {
        pN->uID = AEEBT_PBAP_EVT_CONN_FAILED;
      }
      break;
    }
    case BT_EV_PF_PBAP_CLI_DCN_CFM:
    {
      MSG_LOW( "PBAPEvCb: C dcn cfm, stat=%x id=%x", 
               pEv->status, pEv->conn_id, 0 );

      pEvCb = OEMBTExtPBAP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_pbap_cli_dcn_cfm.conn_id;
        CALLBACK_Init( &pMe->cb, OEMBTExtPBAP_DisconnectCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_PBAP_CLI_PULL_PHONE_BOOK_CFM:
    {
       bt_pf_ev_pbap_cli_pull_phone_book_cfm_type* pEv =
        &ev_msg_ptr->ev_msg.ev_pbap_cli_pull_phone_book_cfm;

      if ( pMe->clientConnID != pEv->conn_id )
      {
        MSG_ERROR( "PBAPEvCb: C pull pb cfm, wrong conn id=%x", 
                   pEv->conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "PBAPEvCb: C pull pb cfm, stat=%x id=%x", 
               pEv->status, 
               pEv->conn_id, 0 );

      pEvCb = OEMBTExtPBAP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = pEv->conn_id;
        pEvCb->pData    = pEv->data_ptr;
        pEvCb->dataLen  = pEv->data_len;

        if ( pEv->phone_book_size_ptr ) 
        {
          pEvCb->phoneBookSize = *(pEv->phone_book_size_ptr);
        }

        if ( pEv->new_missed_calls_ptr ) 
        {
          pEvCb->newMissedCalls = *(pEv->new_missed_calls_ptr);
        }

        pEvCb->final    = pEv->final;
        pEvCb->status   = pEv->status;
        CALLBACK_Init( &pMe->cb, OEMBTExtPBAP_PullPhoneBookClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_PBAP_CLI_SET_PHONE_BOOK_CFM:
    {
      bt_pf_ev_pbap_cli_set_phone_book_cfm_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_pbap_cli_set_phone_book_cfm;
      MSG_LOW( "PBAPEvCb: C set pb cfm, id=%x st=%x", 
               pEvt->conn_id, pEvt->status, 0 );
      if ( pMe->clientConnID == pEvt->conn_id )
      {
        pN->uID = AEEBT_PBAP_EVT_SET_PHONE_BOOK_DONE;
        pN->data.uError = OEMBTExt_MapCmdStatus( pEvt->status );
      }
      else
      {
        MSG_ERROR( "PBAPEvCb: C set pb dne, wrong conn id=%x", 
                   pEvt->conn_id, 0, 0 );
      }
      break;
    }
    case BT_EV_PF_PBAP_CLI_PULL_VCARD_LISTING_CFM:
    {
      bt_pf_ev_pbap_cli_pull_vcard_listing_cfm_type* pEv =
        &ev_msg_ptr->ev_msg.ev_pbap_cli_pull_vcard_listing_cfm;

      if ( pMe->clientConnID != pEv->conn_id )
      {
        MSG_ERROR( "PBAPEvCb: C pull vl cfm, wrong conn id=%x", 
                   pEv->conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "PBAPEvCb: C pull vl cfm, stat=%x id=%x", 
               pEv->status, 
               pEv->conn_id, 0 );

      pEvCb = OEMBTExtPBAP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = pEv->conn_id;
        pEvCb->pData    = pEv->data_ptr;
        pEvCb->dataLen  = pEv->data_len;

        if ( pEv->phone_book_size_ptr ) 
        {
          pEvCb->phoneBookSize = *(pEv->phone_book_size_ptr);
        }

        if ( pEv->new_missed_calls_ptr ) 
        {
          pEvCb->newMissedCalls = *(pEv->new_missed_calls_ptr);
        }

        pEvCb->final    = pEv->final;
        pEvCb->status   = pEv->status;
        CALLBACK_Init( &pMe->cb, OEMBTExtPBAP_PullvCardListingClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }
    case BT_EV_PF_PBAP_CLI_PULL_VCARD_ENTRY_CFM:
    {
      bt_pf_ev_pbap_cli_pull_vcard_entry_cfm_type* pEv =
        &ev_msg_ptr->ev_msg.ev_pbap_cli_pull_vcard_entry_cfm;

      if ( pMe->clientConnID != pEv->conn_id )
      {
        MSG_ERROR( "PBAPEvCb: C pull ve cfm, wrong conn id=%x", 
                   pEv->conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      MSG_LOW( "PBAPEvCb: C pull ve cfm, stat=%x id=%x", 
               pEv->status, 
               pEv->conn_id, 0 );

      pEvCb = OEMBTExtPBAP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = pEv->conn_id;
        pEvCb->pData    = pEv->data_ptr;
        pEvCb->dataLen  = pEv->data_len;

        pEvCb->final    = pEv->final;
        pEvCb->status   = pEv->status;
        CALLBACK_Init( &pMe->cb, OEMBTExtPBAP_PullvCardEntryClientCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      break;
    }

    case BT_EV_PF_PBAP_SRV_AUTH_IND:
    {
      bt_pf_ev_pbap_srv_auth_ind_type* pEv =
        &ev_msg_ptr->ev_msg.ev_pbap_srv_auth_ind;
      MSG_LOW( "PBAPEvCb: S auth ind, id=%x", 
               pEv->conn_id, 0, 0 );

      pMe->serverConnID = pEv->conn_id;
      pN->uID = AEEBT_PBAP_EVT_AUTH_REQ;
      break;
    }
    case BT_EV_PF_PBAP_SRV_CON_IND:
    {
      bt_pf_ev_pbap_srv_con_ind_type* pEv =
        &ev_msg_ptr->ev_msg.ev_pbap_srv_con_ind;
      MSG_LOW( "PBAPEvCb: S con ind, id=%x", 
               pEv->conn_id, 0, 0 );

      pMe->serverConnID = pEv->conn_id;
      pN->uID = AEEBT_PBAP_EVT_CONN_REQ;
      if ( pEv->user_id_len > 0 )
      {
        AEEBT_TO_WSTR( (char*)pEv->user_id,
                       pN->data.PBAPObject.wUserID, 
                       sizeof(pN->data.PBAPObject.wUserID) );
      }
      else
      {
        pN->data.PBAPObject.wUserID[ 0 ] = 0;  // NULL string
      }
      pN->data.bdAddr = *((AEEBTBDAddr*)&ev_msg_ptr->ev_msg.ev_pbap_srv_con_ind.bd_addr);  
      break;
    }
    case BT_EV_PF_PBAP_SRV_DCN_IND:
    {
      MSG_LOW( "PBAPEvCb: S dcn ind, id=%x", 
               pEv->conn_id, 0, 0 );
      pEvCb = OEMBTExtPBAP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_pbap_srv_dcn_ind.conn_id;
        CALLBACK_Init( &pMe->cb, OEMBTExtPBAP_DisconnectCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_PBAP_SRV_PULL_PHONE_BOOK_IND:
    {
      bt_pf_ev_pbap_srv_pull_phone_book_ind_type* pEv =
        &ev_msg_ptr->ev_msg.ev_pbap_srv_pull_phone_book_ind;

      if ( pMe->serverConnID != pEv->conn_id )
      {
        MSG_ERROR( "PBAPEvCb: S pull pb ind, wrong conn id=%x", 
                   pEv->conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      if ( pEv->state == BT_PF_PBAP_SRV_REQ_INITIAL ) 
      {
        pN->data.PBAPObject.filter          = pEv->attribute_mask;
        pN->data.PBAPObject.format          = pEv->format;
        pN->data.PBAPObject.maxListCount    = pEv->max_list_count;
        pN->data.PBAPObject.listStartOffset = pEv->list_start_offset;

        // Save max-list-count information locally 
        pMe->maxListCount = pEv->max_list_count;

        bt_pf_copy_unicode_str( 
          (uint16*)pN->data.PBAPObject.pwObjectName,
          (uint16*)pEv->obj_name,
          BT_PF_MAX_FILENAME_LEN,
          NULL );

        pN->uID = AEEBT_PBAP_EVT_PULL_PHONE_BOOK_REQ;
        break;
      }
      
      if ( pEv->state == BT_PF_PBAP_SRV_REQ_CLEANUP ) 
      {
        pN->uID = AEEBT_PBAP_EVT_PULL_PHONE_BOOK_DONE;
        break;
      }

      pEvCb = OEMBTExtPBAP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = pEv->conn_id;
        pEvCb->state    = pEv->state;
        pEvCb->status   = pEv->status;

        CALLBACK_Init( &pMe->cb, OEMBTExtPBAP_PullPhoneBookServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_PBAP_SRV_SET_PHONE_BOOK_IND:
    {
      bt_pf_ev_pbap_srv_set_phone_book_ind_type* pEv =
        &ev_msg_ptr->ev_msg.ev_pbap_srv_set_phone_book_ind;

      MSG_LOW( "PBAPEvCb: S set folder req, id=%x n=%x l=%x", 
               pEv->conn_id, pEv->create, pEv->level );

      status = BT_CS_GN_SUCCESS;
      if ( pMe->serverConnID != pEv->conn_id )
      {
        MSG_ERROR( "PBAPEvCb: S set folder req, wrong id %x", 
                   pEv->conn_id, 0, 0 );
        status = BT_CS_PF_OBEX_NOT_FOUND;
      }

      if ( status != BT_CS_GN_SUCCESS )
      {
        bt_cmd_pf_pbap_srv_set_phone_book_response( 
                                               pMe->appId, 
                                               pMe->serverConnID,
                                               status );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }
      pN->uID = AEEBT_PBAP_EVT_SET_PHONE_BOOK_REQ;
      if ( pEv->level == 0)
      {
        pN->data.PBAPObject.flag = AEEBT_PBAP_SET_PATH_TO_ROOT;
      }
      else if ( (pEv->folder_len == 0) || (pEv->folder_str == NULL) )
      {
        pN->data.PBAPObject.flag = AEEBT_PBAP_SET_PATH_TO_PARENT;
      }
      else
      {
        pN->data.PBAPObject.flag = AEEBT_PBAP_SET_PATH_TO_FOLDER;
        bt_pf_copy_unicode_str( 
          (uint16*)pN->data.PBAPObject.wName,
          (uint16*)pEv->folder_str,
          BT_PF_MAX_FILENAME_LEN,
          NULL );
      }
      break;
    }
    case BT_EV_PF_PBAP_SRV_PULL_VCARD_LISTING_IND:
    {
      bt_pf_ev_pbap_srv_pull_vcard_listing_ind_type* pEv =
        &ev_msg_ptr->ev_msg.ev_pbap_srv_pull_vcard_listing_ind;

      if ( pMe->serverConnID != pEv->conn_id )
      {
        MSG_ERROR( "PBAPEvCb: S pull vl ind, wrong conn id=%x", 
                   pEv->conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      if ( pEv->state == BT_PF_PBAP_SRV_REQ_INITIAL ) 
      {
        pN->data.PBAPObject.searchOrder     = pEv->order;
        pN->data.PBAPObject.searchAttrib    = pEv->search_attrib;
        pN->data.PBAPObject.maxListCount    = pEv->max_list_count;
        pN->data.PBAPObject.listStartOffset = pEv->list_start_offset;

        // Save max-list-count information locally 
        pMe->maxListCount = pEv->max_list_count;

        bt_pf_copy_unicode_str( 
          (uint16*)pN->data.PBAPObject.pwObjectName,
          (uint16*)pEv->obj_name,
          BT_PF_MAX_FILENAME_LEN,
          NULL );

        AEEBT_TO_WSTR( (char*)pEv->value,
                       pN->data.PBAPObject.searchValue,
                       sizeof(pN->data.PBAPObject.searchValue) );

        pN->uID = AEEBT_PBAP_EVT_PULL_VCARD_LISTING_REQ;
        break;
      }
      
      if ( pEv->state == BT_PF_PBAP_SRV_REQ_CLEANUP ) 
      {
        pN->uID = AEEBT_PBAP_EVT_PULL_VCARD_LISTING_DONE;
        break;
      }

      pEvCb = OEMBTExtPBAP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = pEv->conn_id;
        pEvCb->state    = pEv->state;
        pEvCb->status   = pEv->status;

        CALLBACK_Init( &pMe->cb, OEMBTExtPBAP_PullvCardListingServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_PBAP_SRV_PULL_VCARD_ENTRY_IND:
    {
      bt_pf_ev_pbap_srv_pull_vcard_entry_ind_type* pEv =
        &ev_msg_ptr->ev_msg.ev_pbap_srv_pull_vcard_entry_ind;

      if ( pMe->serverConnID != pEv->conn_id )
      {
        MSG_ERROR( "PBAPEvCb: S pull ve ind, wrong conn id=%x", 
                   pEv->conn_id, 0, 0 );
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return;
      }

      if ( pEv->state == BT_PF_PBAP_SRV_REQ_INITIAL ) 
      {
        pN->data.PBAPObject.filter = pEv->attribute_mask;
        pN->data.PBAPObject.format = pEv->format;

        bt_pf_copy_unicode_str( 
          (uint16*)pN->data.PBAPObject.pwObjectName,
          (uint16*)pEv->obj_name,
          BT_PF_MAX_FILENAME_LEN,
          NULL );

        pN->uID = AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_REQ;
        break;
      }
      
      if ( pEv->state == BT_PF_PBAP_SRV_REQ_CLEANUP ) 
      {
        pN->uID = AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_DONE;
        break;
      }

      pEvCb = OEMBTExtPBAP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = pEv->conn_id;
        pEvCb->state    = pEv->state;
        pEvCb->status   = pEv->status;

        CALLBACK_Init( &pMe->cb, OEMBTExtPBAP_PullvCardEntryServerCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pMe->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }

    default:
    {
      MSG_ERROR( "PBAPEvCb - unexpect event %x", 
                 ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_PBAP );
  IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
}

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_PBAP)

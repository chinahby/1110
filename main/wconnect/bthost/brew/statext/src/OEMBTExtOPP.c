/*===========================================================================

FILE:      OEMBTExtOPP.c

SERVICES:  BlueTooth Object Push Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Object Push Profile.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2004-2009 QUALCOMM Incorporated.
      All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtOPP.c#5 $ 
  $DateTime: 2009/07/13 12:23:06 $
  $Author: ganeshs $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2009-07-10   gs  Fixed file handle double free issue
  2009-04-17   gs  Reset 'state' if connection fails.
  2009-04-17   gs  Added support for rejecting incoming push request
  2008-10-07   pn  Replaced deprecated functions.
  2008-09-19   rs  Klokworks error fixes and freeing up the bInUse before 
                   returning from the callback function.

===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_OPP)
#include "bt.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "oi_obexspec.h"
#include "msg.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtOPP.h"

//==========================================================================
//   Macro definitions
//==========================================================================

#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
/* MAX payload size in OBEX Body/End-of-Body header 
  ( 16378 = 16K - 6 byte header ) */
#define OPP_BUFFER_SIZE   AEEBT_OBEX_BUFFER_SIZE + 5 
                          /* extra 5 bytes since conn-id hdr is not used */
#else
#define OPP_BUFFER_SIZE   32757
#endif /* FEATURE_BT_REDUCE_MEMORY_USAGE */
#else
#define OPP_BUFFER_SIZE   4266
#endif /* FEATURE_BT_SOC */
#define NAME_ONLY( f )                       \
  ((WSTRRCHR( f, DIRECTORY_CHAR ) == NULL)?  \
   (f): (WSTRRCHR(f, DIRECTORY_CHAR) + 1)) 

//==========================================================================
//   Type definitions
//==========================================================================
typedef enum
{
  AEEBT_OPP_STATE_INIT,
  AEEBT_OPP_STATE_REGISTERED,   // server only
  AEEBT_OPP_STATE_CONNECTING,
  AEEBT_OPP_STATE_CONNECTED,
  AEEBT_OPP_STATE_PUSH_STARTED,
  AEEBT_OPP_STATE_PULL_STARTED,
  AEEBT_OPP_STATE_PUSH_REQ_PENDING,
  AEEBT_OPP_STATE_PULL_REQ_PENDING
} AEEBTOPPState;


typedef struct OEMBTExtOPPobj_struct
{
  ACONTEXT*                   pac;
  IBTExtNotifier*             pNotifier;

  bt_app_id_type              appId;
  AEEBTBDAddr                 BDAddr;         // remote device's

  IFileMgr*                   pFileMgr;
  IFile*                      pFile;
  uint32                      dwFileSize;
  AECHAR                      wName[ AEEBT_MAX_FILE_NAME + 1 ];
  char                        szFileName[ AEEBT_MAX_FILE_NAME + 1 ];
  char*                       pszType;

  uint32                      bytesSent;
  uint32                      bytesRcvd;

  bt_pf_opp_srv_conn_id_type  serverConnID;
  bt_pf_opp_cli_conn_id_type  clientConnID;

  uint8*                      pBuffer;
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
  bt_cmd_status_type          prevWriteStatus;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */

  AEEBTOPPState               state;
  boolean                     bIsServer;
  boolean                     bPushAllowed;     // for server only
  boolean                     bPullAllowed;     // for server only
  boolean                     bDoDeregister;    // for server only
  AEEBTOPPFormat              supportedFormat;  // for server only

  struct OEMBTExtOPPobj_struct* pNextOPP;  // linked into mgr
} OEMBTExtOPPobj_t;

typedef struct
{
  boolean                     bInUse;
  bt_event_type               event;
  bt_app_id_type              appId;
  uint16                      connId;
  bt_pf_opp_handle_type       handle;
  uint32                      maxBytes;
  uint8*                      pSrc;
  bt_cmd_status_type          status;

  AEECallback                 cb;
  AECHAR                      wName[ AEEBT_MAX_FILE_NAME + 1 ];
  AECHAR                      wType[ AEEBT_MAX_MIME_TYPE_LEN + 1 ];
  uint8                       nameLen;
  uint32                      objSize;
} OEMBTExtOPP_EvCb;

// one of these
typedef struct
{
  OEMBTExtOPPobj_t* pNextOPP;    // singly linked list of OPPs
  OEMBTExtOPP_EvCb  FreeEvCb[ 10 ];
} OEMBTExtOPP_Mgr;

#ifdef CUST_EDITION
typedef struct
{
  AEEBTObjectType	ObjectType;
  char* 			pMIMEString;
  char* 			pFileExt;
} AEEBTObjectTypeMap;

static const AEEBTObjectTypeMap ObjectTypeMap[] =
{
	{ AEEBT_OPP_ICAL,				OI_OBEX_ICALENDAR_TYPE, 			".ics" },
	{ AEEBT_OPP_VCAL,				OI_OBEX_VCALENDAR_TYPE, 			".vcs" },
	{ AEEBT_OPP_VCARD,				OI_OBEX_VCARD_TYPE, 				".vcf" },
	{ AEEBT_OPP_VNOTE,				OI_OBEX_VNOTE_TYPE, 				".vnt" },
	{ AEEBT_OPP_VMESSAGE,			OI_OBEX_VMESSAGE_TYPE, 				".vmg" },
	{ AEEBT_OPP_JPEG,				OI_OBEX_JPEG_TYPE, 					".jpg" },
	{ AEEBT_OPP_JPEG,				OI_OBEX_JPEG_TYPE, 					".jpe" },
	{ AEEBT_OPP_JPEG,				OI_OBEX_JPEG_TYPE, 					".jpeg" },
	{ AEEBT_OPP_JPEG,				OI_OBEX_JPEG_TYPE, 					".jfif" },
	{ AEEBT_OPP_JPEG,				OI_OBEX_JPEG_TYPE, 					".jff" },
	{ AEEBT_OPP_JPEG,				OI_OBEX_JPEG_TYPE, 					".jft" },
	{ AEEBT_OPP_IMAGE_BMP,			OI_OBEX_IMAGE_BMP_TYPE, 			".bmp" },
	{ AEEBT_OPP_IMAGE_PNG,			OI_OBEX_IMAGE_PNG_TYPE, 			".png" },
	{ AEEBT_OPP_IMAGE_GIF,			OI_OBEX_IMAGE_GIF_TYPE, 			".gif" },
	{ AEEBT_OPP_IMAGE_WBMP,			OI_OBEX_IMAGE_WBMP_TYPE,			".wbmp" },
	{ AEEBT_OPP_IMAGE_WBMP,			OI_OBEX_IMAGE_WBMP_TYPE,			".wbm" },
	{ AEEBT_OPP_AUDIO_AMR,			OI_OBEX_AUDIO_AMR_TYPE,				".amr" },
	{ AEEBT_OPP_AUDIO_AAC,			OI_OBEX_AUDIO_AAC_TYPE,				".aac" },
	{ AEEBT_OPP_AUDIO_MP3,			OI_OBEX_AUDIO_MP3_TYPE,				".mp3" },
	{ AEEBT_OPP_AUDIO_WAV,			OI_OBEX_AUDIO_WAV_TYPE,				".wav" },
	{ AEEBT_OPP_AUDIO_IMY,			OI_OBEX_AUDIO_IMELODY_TYPE,			".imy" },
	{ AEEBT_OPP_AUDIO_MID,			OI_OBEX_AUDIO_MID_TYPE, 			".mid" },
	{ AEEBT_OPP_AUDIO_MIDI,			OI_OBEX_AUDIO_MIDI_TYPE,			".midi" },
	{ AEEBT_OPP_AUDIO_MMF,			OI_OBEX_AUDIO_MMF_TYPE,				".mmf" },
	{ AEEBT_OPP_AUDIO_XMF,			OI_OBEX_AUDIO_XMF_TYPE,				".xmf" },
	{ AEEBT_OPP_AUDIO_RHZ,			OI_OBEX_AUDIO_RHZ_TYPE,				".rhz" },
	{ AEEBT_OPP_AUDIO_WMA,			OI_OBEX_AUDIO_WMA_TYPE,				".wma" },
	{ AEEBT_OPP_AUDIO_M4A,			OI_OBEX_AUDIO_MP4_TYPE,				".m4a" },
	{ AEEBT_OPP_AUDIO_3GP,			OI_OBEX_AUDIO_3GPP_TYPE,			".3ga" },
	{ AEEBT_OPP_VIDEO_3GP,			OI_OBEX_VIDEO_3GPP_TYPE,			".3gp" },
	{ AEEBT_OPP_VIDEO_MP4,			OI_OBEX_VIDEO_MP4_TYPE,				".mp4" },
	{ AEEBT_OPP_VIDEO_WMV,			OI_OBEX_VIDEO_WMV_TYPE,				".wmv" },
	{ AEEBT_OPP_VIDEO_ASF,			OI_OBEX_VIDEO_ASF_TYPE,				".asf" },
	{ AEEBT_OPP_TEXT,				OI_OBEX_TEXT_TYPE, 					".txt" },
	{ AEEBT_OPP_JAVA_JAD,			OI_OBEX_APPLICATION_JAD_TYPE,		".jad" },
	{ AEEBT_OPP_JAVA_JAR,			OI_OBEX_APPLICATION_JAR_TYPE,		".jar" },
	{ AEEBT_OPP_APPLICATION_DOC,	OI_OBEX_APPLICATION_DOC_TYPE,		".doc" },
	{ AEEBT_OPP_APPLICATION_PDF,	OI_OBEX_APPLICATION_PDF_TYPE,		".pdf" },
	{ AEEBT_OPP_APPLICATION_EXCEL,	OI_OBEX_APPLICATION_EXCEL_TYPE,		".xls" },
	{ AEEBT_OPP_UNKNOWN_TYPE,		NULL, 								NULL }
};

#endif /* CUST_EDITION */


//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static int  OEMBTExtOPP_CheckCmdStatus( bt_cmd_status_type stat );
static void OEMBTExtOPP_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);
static OEMBTExtOPP_EvCb* OEMBTExtOPP_GetFreeEvCb( void );

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtOPP_Mgr        gMgr;               // manager object

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtOPP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtOPP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtOPP_Init(IBTExtOPP* pParent)
{
  OEMBTExtOPPobj_t* pMe;

  // allocate pMe
  pMe = MALLOCREC(OEMBTExtOPPobj_t);
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
    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    FREE( pMe );
    return EFAILED;
  }

  // init BT layer
  pMe->appId = bt_cmd_ec_get_app_id_and_register(OEMBTExtOPP_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    IFILEMGR_Release( pMe->pFileMgr );
    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    FREE( pMe );
    return EFAILED;
  }

  pMe->pBuffer = MALLOC( OPP_BUFFER_SIZE );
  if ( pMe->pBuffer == NULL )
  {
    (void)bt_cmd_ec_free_application_id( pMe->appId );
    IFILEMGR_Release( pMe->pFileMgr );
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

  if ( gMgr.pNextOPP == NULL ) // first time?
  {
    uint8 i;
    for ( i = 0; i < (sizeof(gMgr.FreeEvCb)/sizeof(gMgr.FreeEvCb[0])); i++ )
    {
      gMgr.FreeEvCb[ i ].bInUse = FALSE;
    }
  }
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
  pMe->prevWriteStatus = BT_CS_GN_SUCCESS;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */

  // link into mgr
  pMe->pNextOPP = gMgr.pNextOPP;
  gMgr.pNextOPP = pMe;

  return SUCCESS;
}


// IBase (see AEE.h)
uint32 OEMBTExtOPP_Release( IBTExtOPP* pParent )
{
  OEMBTExtOPPobj_t*  pMe;
  OEMBTExtOPPobj_t** ppc;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) == TRUE )
  {
    // unlink from mgr
    for (ppc = &gMgr.pNextOPP; *ppc != NULL; ppc = &(*ppc)->pNextOPP)
    {
      if ((*ppc)->appId == pMe->appId)
      {
        *ppc = pMe->pNextOPP;
        pMe->pNextOPP = NULL;
        break;
      }
    }

    if ( pMe->pBuffer != NULL )
    {
      FREE( pMe->pBuffer );
      pMe->pBuffer = NULL;
    }

    (void)bt_cmd_ec_free_application_id( pMe->appId );
    pMe->appId = BT_APP_ID_NULL;

    IFILEMGR_Release( pMe->pFileMgr );
    pMe->pFileMgr = NULL;

    IBTEXTNOTIFIER_Release( pMe->pNotifier );
    pMe->pNotifier = NULL;
    pMe->pac = NULL;

    FREE(pMe);
    (void) AEEHandle_Clear( &gOEMBTExtHandleList, pParent->m_hBT );

    return SUCCESS;
  }

  return EFAILED;
}

// IBTEXTOPP_Register (see AEEBTExtOPP.h)
int OEMBTExtOPP_Register( IBTExtOPP*     pParent, 
                          AEEBTOPPFormat supportedFormat,
                          const char*    pszServiceName )
{
  bt_cmd_status_type stat;
  OEMBTExtOPPobj_t   *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pszServiceName == NULL )
  {
    return EBADPARM;
  }
  if ( pMe->state > AEEBT_OPP_STATE_INIT )
  {
    return EBADSTATE;
  }

  stat = bt_cmd_pf_opp_srv_register( pMe->appId, 
                                     (uint32)supportedFormat, 
                                     (char*)pszServiceName );
  if ( stat == BT_CS_GN_PENDING )
  {
    pMe->supportedFormat = supportedFormat;
  }
  return OEMBTExtOPP_CheckCmdStatus( stat );
}


// IBTEXTOPP_Deregister (see AEEBTExtOPP.h)
int OEMBTExtOPP_Deregister( IBTExtOPP* pParent )
{
  bt_cmd_status_type          stat;
  OEMBTExtOPPobj_t            *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  switch ( pMe->state )
  {
    case AEEBT_OPP_STATE_INIT:
    case AEEBT_OPP_STATE_PUSH_STARTED:     // fall through
    case AEEBT_OPP_STATE_PULL_STARTED:     // fall through
    case AEEBT_OPP_STATE_PUSH_REQ_PENDING: // fall through
    case AEEBT_OPP_STATE_PULL_REQ_PENDING: // fall through
      return EBADSTATE;

    case AEEBT_OPP_STATE_REGISTERED:
      stat = bt_cmd_pf_opp_srv_deregister( pMe->appId );
      return OEMBTExtOPP_CheckCmdStatus( stat );

    case AEEBT_OPP_STATE_CONNECTED:
      stat = bt_cmd_pf_opp_srv_force_disconnect( pMe->appId, 
                                                 pMe->serverConnID );
      if ( stat == BT_CS_GN_PENDING )
      {
        pMe->bDoDeregister = TRUE;
      }
      return SUCCESS;
    
    default:
      /* any other state is not a place to handle this command */
      return EBADSTATE;
  }

}


// IBTEXTOPP_AcceptConnection (see AEEBTExtOPP.h)
int OEMBTExtOPP_AcceptConnection( 
  IBTExtOPP*          pParent, 
  const AEEBTBDAddr*  pBDAddr, 
  boolean             bAllowPush,
  boolean             bAllowPull
)
{
  bt_cmd_status_type  stat;
  OEMBTExtOPPobj_t    *pMe;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }
  
  if ( pBDAddr == NULL )
  {
    return EBADPARM;
  }

  if ( pMe->state >= AEEBT_OPP_STATE_CONNECTED )
  {
    return EBADSTATE;
  }

  pMe->bPushAllowed = bAllowPush;
  pMe->bPullAllowed = bAllowPull;

  stat = bt_cmd_pf_opp_srv_accept_connect( pMe->appId, pMe->serverConnID,
                                           bAllowPush, bAllowPull );
  return OEMBTExtOPP_CheckCmdStatus( stat );
}


// IBTEXTOPP_Connect (see AEEBTExtOPP.h)
int OEMBTExtOPP_Connect( 
  IBTExtOPP*          pParent, 
  const AEEBTBDAddr*  pBDAddr, 
  uint8               uChannelNumber
)
{
  bt_cmd_status_type  stat;
  OEMBTExtOPPobj_t    *pMe;

  MSG_FATAL("***zzg OEMBTExtOPP_Connect Start***", 0, 0, 0);

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
  	MSG_FATAL("***zzg OEMBTExtOPP_Connect return 1***", 0, 0, 0);
    return EFAILED;
  }
  if ( pBDAddr == NULL )
  {
  	MSG_FATAL("***zzg OEMBTExtOPP_Connect return 2***", 0, 0, 0);
    return EBADPARM;
  }
  if ( pMe->state >= AEEBT_OPP_STATE_CONNECTED )
  {
  	MSG_FATAL("***zzg OEMBTExtOPP_Connect return 3***", 0, 0, 0);
    return EBADSTATE;
  }
  stat = bt_cmd_pf_opp_cli_connect( pMe->appId, (bt_bd_addr_type*)pBDAddr,
                                    uChannelNumber );

  MSG_FATAL("***zzg OEMBTExtOPP_Connect stat=%x***", stat, 0, 0);
  
  return OEMBTExtOPP_CheckCmdStatus( stat );
}


// IBTEXTOPP_Abort (see AEEBTExtOPP.h)
int OEMBTExtOPP_Abort( IBTExtOPP *pParent )
{
  bt_cmd_status_type stat;
  OEMBTExtOPPobj_t*  pMe;

  MSG_FATAL("***zzg OEMBTExtOPP_Abort Start***", 0, 0, 0);

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
  	MSG_FATAL("***zzg OEMBTExtOPP_Abort return 1***", 0, 0, 0);
    return EFAILED;
  }
  if ( pMe->state <= AEEBT_OPP_STATE_CONNECTED )
  {
  	MSG_FATAL("***zzg OEMBTExtOPP_Abort return 2***", 0, 0, 0);
    return EBADSTATE;
  }
  stat = bt_cmd_pf_opp_cli_abort( pMe->appId, pMe->clientConnID );

  MSG_FATAL("***zzg OEMBTExtOPP_Abort stat=%x***", stat, 0, 0);
  
  return OEMBTExtOPP_CheckCmdStatus( stat );
}


// IBTEXTOPP_Push (see AEEBTExtOPP.h)
int OEMBTExtOPP_Push( 
  IBTExtOPP*      pParent,
  const AECHAR*   pwFileName,
  AEEBTObjectType objType
)
{
  bt_cmd_status_type  statOut, statIn;
  OEMBTExtOPPobj_t*   pMe;
  char                szFileName[ AEEBT_MAX_FILE_NAME ];
  ACONTEXT*           pCurAC;
  AEEFileInfoEx       fileInfoEx;

  char tempstr[100];

  WSTRTOSTR(pwFileName, tempstr, sizeof(tempstr));

  DBGPRINTF("***zzg OEMBTExtOPP_Push pszName=%s, WSTRlen=%d***", tempstr, WSTRLEN( pwFileName ));
    

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
  
#ifndef CUST_EDITION
  if ( (pwFileName == NULL) && (pMe->bIsServer == FALSE) )
  {
  	DBGPRINTF("***zzg OEMBTExtOPP_Push EBADPARM 111***");
    return EBADPARM;
  }
  switch ( objType )
  {
    case AEEBT_OPP_ICAL:
      pMe->pszType = OI_OBEX_ICALENDAR_TYPE;
      break;
    case AEEBT_OPP_VCAL:
      pMe->pszType = OI_OBEX_VCALENDAR_TYPE;
      break;
    case AEEBT_OPP_VCARD:
      pMe->pszType = OI_OBEX_VCARD_TYPE;
      break;
    case AEEBT_OPP_VNOTE:
      pMe->pszType = OI_OBEX_VNOTE_TYPE;
      break;
    case AEEBT_OPP_VMESSAGE:
      pMe->pszType = OI_OBEX_VMESSAGE_TYPE;
      break;
    case AEEBT_OPP_JPEG:
      pMe->pszType = OI_OBEX_JPEG_TYPE;
      break;
    default:
      pMe->pszType = NULL;
      break;
  }
#else
  if ( ( pwFileName == NULL ) || ( WSTRLEN( pwFileName ) == 0 ) )
  {
  		DBGPRINTF("***zzg OEMBTExtOPP_Push EBADPARM 111***");
    	return EBADPARM;
  }

  pMe->pszType = OI_OBEX_VCARD_TYPE;
  
  if ( objType == AEEBT_OPP_UNKNOWN_TYPE )
  {
	   uint8 i = 0;

	   AECHAR *pExt = NULL;

	   char suffix[ 6 ] = { 0 };	   

	   pExt = WSTRRCHR( pwFileName, L'.' );


	   if ( pExt != NULL )
	   {
		    AEEBT_FROM_WSTR( pExt, suffix, sizeof( suffix ) );

			for ( i = 0; i < ( sizeof( ObjectTypeMap ) / sizeof( AEEBTObjectTypeMap ) ); i++ )
			{
				  if ( STRICMP( suffix, ObjectTypeMap[ i ].pFileExt ) == 0 )  //!=0??
			      {
			     	   pMe->pszType = ObjectTypeMap[ i ].pMIMEString;
					   break;
			      }
			}
	   }
  }
  else
  {
	   uint8 i = 0;


	   for ( i = 0; i < ( sizeof( ObjectTypeMap ) / sizeof( AEEBTObjectTypeMap ) ); i++ )
	   {
			 if ( objType == ObjectTypeMap[ i ].ObjectType )
		     {
		     	  pMe->pszType = ObjectTypeMap[ i ].pMIMEString;
				  break;
		     }
	   }
  }

#endif /* CUST_EDITION */

  if ( pwFileName != NULL )
  {
    statIn = BT_CS_GN_SUCCESS;
	
#ifdef CUST_EDITION
	memset((void*)pMe->wName, 0, sizeof(pMe->wName));
#endif /* CUST_EDITION */

    WSTRLCPY( pMe->wName, pwFileName, ARR_SIZE( pMe->wName ) );

    //AEEBT_FROM_WSTR( pwFileName, szFileName, sizeof(szFileName) );
    WSTRTOSTR(pwFileName, szFileName, sizeof(szFileName));	//Modify by zzg 2011_12_28

    pCurAC = ENTER_APP_CONTEXT( pMe->pac );

    IFILEMGR_GetInfoEx( pMe->pFileMgr, szFileName, &fileInfoEx );
    pMe->dwFileSize = fileInfoEx.dwSize;

	DBGPRINTF("***zzg OEMBTExtOPP_Push dwSize=%d***", fileInfoEx.dwSize);
	DBGPRINTF("***zzg OEMBTExtOPP_Push pszFile=%s***", fileInfoEx.pszFile);
	DBGPRINTF("***zzg OEMBTExtOPP_Push szFileName=%s***", szFileName);

    pMe->pFile = IFILEMGR_OpenFile( pMe->pFileMgr, szFileName, _OFM_READ );
    (void) LEAVE_APP_CONTEXT( pCurAC );

    if ( pMe->pFile == NULL )
    {
    	DBGPRINTF("***zzg OEMBTExtOPP_Push EBADPARM 333***");
      return EBADPARM;
    }
  }
  else
  {
    pMe->dwFileSize = 0;
    pMe->wName[ 0 ] = 0;
    statIn = OI_OBEX_NOT_FOUND;
  }

  TASKLOCK();
  if ( pMe->bIsServer == FALSE )
  {
    // client calls this function to push data to server
    if ( pMe->state != AEEBT_OPP_STATE_CONNECTED )
    {
      statOut = BT_CS_GN_BAD_CMD_STATE;
    }
    else
    {
      statOut = bt_cmd_pf_opp_cli_push( pMe->appId, pMe->clientConnID, 
                                        pMe->wName, pMe->pszType );
    }
  }
  else
  {
    // server calls this function to push its default business card
    // to the client who issued PULL request
    if ( pMe->state != AEEBT_OPP_STATE_PULL_REQ_PENDING )
    {
      statOut = BT_CS_GN_BAD_CMD_STATE;
    }
    else
    {
      statOut = bt_cmd_pf_opp_srv_open_read_done( 
      pMe->appId, pMe->serverConnID, (bt_pf_opp_handle_type)pMe->pFile,
      NAME_ONLY( pMe->wName ), pMe->pszType, pMe->dwFileSize, statIn );
    }
  }

  if ( statIn == OI_OBEX_NOT_FOUND )
  {
    pMe->state = AEEBT_OPP_STATE_CONNECTED;
  }
  else if ( (statOut == BT_CS_GN_PENDING) || (statOut == BT_CS_GN_SUCCESS) )
  {
    pMe->state     = AEEBT_OPP_STATE_PUSH_STARTED;
    pMe->bytesSent = 0;
    pMe->bytesRcvd = 0;
  }
  TASKFREE();

  return OEMBTExtOPP_CheckCmdStatus( statOut );
}


// IBTEXTOPP_Pull (see AEEBTExtOPP.h)
int OEMBTExtOPP_Pull( IBTExtOPP* pParent, const AECHAR* pwFileName )
{
  bt_cmd_status_type  stat;
  OEMBTExtOPPobj_t*   pMe;
  ACONTEXT*           pCurAC;
  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EFAILED;
  }

  if ( pwFileName == NULL && pMe->bIsServer == TRUE )
  {
    // reject push request 
    status = BT_CS_PF_OBEX_ACCESS_DENIED;
  }
  else if ( pwFileName == NULL )
  {
    return EBADPARM;
  }

  if ( pwFileName != NULL ) 
  {
    pCurAC = ENTER_APP_CONTEXT( pMe->pac );
    AEEBT_FROM_WSTR( pwFileName, pMe->szFileName, sizeof(pMe->szFileName) );
    pMe->pFile = IFILEMGR_OpenFile( pMe->pFileMgr, pMe->szFileName, _OFM_CREATE );
    (void) LEAVE_APP_CONTEXT( pCurAC );

    if ( pMe->pFile == NULL )
    {
      return EBADPARM;
    }
  }

  TASKLOCK();
  if ( pMe->bIsServer == FALSE )
  {
    // client calls this function to pull server's default business card
    if ( pMe->state != AEEBT_OPP_STATE_CONNECTED )
    {
      stat = BT_CS_GN_BAD_CMD_STATE;
    }
    else
    {
      stat = bt_cmd_pf_opp_cli_pull( pMe->appId, pMe->clientConnID );
    }
  }
  else
  {
    // server calls this function to pull the object that client tries to push
    if ( (pMe->state != AEEBT_OPP_STATE_PUSH_REQ_PENDING) )
    {
      stat = BT_CS_GN_BAD_CMD_STATE;
    }
    else
    {
      stat = bt_cmd_pf_opp_srv_open_write_done(
        pMe->appId, pMe->serverConnID, (bt_pf_opp_handle_type) pMe->pFile, 
        status );
    }
  }
  if ( (stat == BT_CS_GN_PENDING) || (stat == BT_CS_GN_SUCCESS) )
  {
    pMe->state     = AEEBT_OPP_STATE_PULL_STARTED;
    pMe->bytesRcvd = 0;
    pMe->bytesSent = 0;
  }
  TASKFREE();

  return OEMBTExtOPP_CheckCmdStatus( stat );
}


// IBTEXTOPP_Disconnect (see AEEBTExtOPP.h)
int OEMBTExtOPP_Disconnect( IBTExtOPP* pParent )
{
  bt_cmd_status_type  stat;
  OEMBTExtOPPobj_t *pMe;

  MSG_FATAL("***zzg OEMBTExtOPP_Disconnect start***", 0, 0, 0);

  if( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                      (OEMINSTANCE*)&pMe ) != TRUE )
  {
  	MSG_FATAL("***zzg OEMBTExtOPP_Disconnect return 1***", 0, 0, 0);
    return EFAILED;
  }
  if ( pMe->state < AEEBT_OPP_STATE_CONNECTING )
  {
  	MSG_FATAL("***zzg OEMBTExtOPP_Disconnect return 2***", 0, 0, 0);
    return EBADSTATE;
  }
  if ( pMe->bIsServer )
  {
    // force disconnect
    stat = bt_cmd_pf_opp_srv_force_disconnect( pMe->appId, pMe->serverConnID );
  }
  else
  {
    stat = bt_cmd_pf_opp_cli_disconnect( pMe->appId, pMe->clientConnID );
  }

  MSG_FATAL("***zzg OEMBTExtOPP_Disconnect stat=%x***", stat, 0, 0);
  
  return OEMBTExtOPP_CheckCmdStatus( stat );
}

// IBTEXTOPP_GetProgressInfo (see AEEBTExtOPP.h)

int OEMBTExtOPP_GetProgressInfo(IBTExtOPP* pParent, AEEBTProgressInfo* pProgressInfo)
{
  OEMBTExtOPPobj_t *pMe;
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
    pProgressInfo->objSize = pMe->dwFileSize;
    pProgressInfo->numBytes = ( pMe->bytesSent > 0 ) ?
                              pMe->bytesSent :
                              pMe->bytesRcvd;
  }
  return ret;
}
//==========================================================================
//   static helper functions
//==========================================================================
static int OEMBTExtOPP_CheckCmdStatus( bt_cmd_status_type stat )
{
  switch (stat)
  {
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return SUCCESS;
    case BT_CS_GN_CMD_Q_FULL:
      return ENOMEMORY;
    case BT_CS_GN_BAD_CMD_STATE:
      return EBADSTATE;
    case OI_STATUS_INVALID_PARAMETERS:
      return EBADPARM;
    default:
      return EFAILED;
  }
}

static OEMBTExtOPPobj_t* OEMBTExtOPP_FindMe(bt_app_id_type appId)
{
  OEMBTExtOPPobj_t* pMe;

  for (pMe = gMgr.pNextOPP; pMe != NULL; pMe = pMe->pNextOPP)
  {
    if (pMe->appId == appId)
    {
      return pMe;
    }
  }
  return NULL;
}

static boolean OEMBTExtOPP_HandleCmdDone( 
  bt_ev_gn_cmd_done_type* pCmdDn, 
  AEEBTNotification*      pN,
  OEMBTExtOPPobj_t*       pMe
)
{
  boolean doSendNotif = TRUE;

  MSG_FATAL( "OPP_HandleCmdDone - st=%x stat=%x cmd=%x", pMe->state,
           pCmdDn->cmd_status, pCmdDn->cmd_type );

  MSG_FATAL("***zzg OEMBTExtOPP_HandleCmdDone cmd_type=%x***", pCmdDn->cmd_type, 0, 0);

  switch ( pCmdDn->cmd_type )
  {
    case BT_PF_CMD_OPP_SRV_REGISTER:
    {
      pN->uID = AEEBT_OPP_EVT_REG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError == AEEBT_OPP_ERR_NONE )
      {
        pMe->state = AEEBT_OPP_STATE_REGISTERED;
        pMe->bIsServer = TRUE;
      }
      break;
    }
    case BT_PF_CMD_OPP_SRV_DEREGISTER:
    {
      pN->uID = AEEBT_OPP_EVT_DEREG_DONE;
      pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      if ( pN->data.uError != AEEBT_ERR_OPERATION_IN_PROGRESS )
      {
        pMe->state = AEEBT_OPP_STATE_INIT;
        pMe->bIsServer = FALSE;
      }
      break;
    }
    case BT_PF_CMD_OPP_SRV_ACCEPT:
    {
      if ( ( pCmdDn->cmd_status != BT_CS_GN_SUCCESS ) ||
           ( ( pMe->bPushAllowed == FALSE ) && 
             ( pMe->bPullAllowed == FALSE ) ) )
      {
        // rejecting incoming connection if neither push nor pull is allowed 
        doSendNotif = FALSE;
        pMe->serverConnID = BT_PF_OPP_NO_CONN_ID;
      }
      else
      {
        pN->uID = AEEBT_OPP_EVT_CONNECTED;
        pMe->state = AEEBT_OPP_STATE_CONNECTED;
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
        pMe->prevWriteStatus = BT_CS_GN_SUCCESS;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */
      }
      break;
    }
    case BT_PF_CMD_OPP_SRV_FORCE_DCN:
    {
      doSendNotif = FALSE;
      break;
    }
    case BT_PF_CMD_OPP_CLI_CONNECT:
    {
      if ( pCmdDn->cmd_status != BT_CS_GN_SUCCESS )
      {
      	MSG_FATAL("***zzg AEEBT_OPP_EVT_CONN_FAILED 1 cmd_status=%x***", pCmdDn->cmd_status, 0, 0);
        pN->uID = AEEBT_OPP_EVT_CONN_FAILED;
        pN->data.uError = OEMBTExt_MapCmdStatus( pCmdDn->cmd_status );
      }
      else
      {
        doSendNotif = FALSE;
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
        pMe->prevWriteStatus = BT_CS_GN_SUCCESS;
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */
      }
      break;
    }
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
    case BT_PF_CMD_OPP_CLI_PUSH:
    case BT_PF_CMD_OPP_CLI_PULL:
    {
      doSendNotif = FALSE;
      /* Reset the status since a write has not occurred yet (TBD) */
      pMe->prevWriteStatus = BT_CS_GN_SUCCESS;
      break;
    }
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */
    default:
    {
      doSendNotif = FALSE;
    }
  }

  return doSendNotif;
}
static void OEMBTExtOPP_NotifyProgress(OEMBTExtOPPobj_t* pMe)
{
  AEEBTNotification*  pN    = NULL;

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_FATAL( "OPP Notify progress - Can't get free not.",
               0, 0, 0 );
  }
  else
  {
    pN->data.uError  = AEEBT_OPP_ERR_NONE;
    pN->data.ProgressInfo.objSize = pMe->dwFileSize;
    pN->data.ProgressInfo.numBytes = ( pMe->bytesSent > 0 ) ?
                                       pMe->bytesSent :
                                       pMe->bytesRcvd;
    pN->uID = AEEBT_OPP_EVT_PROGRESS;

    MSG_HIGH( "OPP Notify progress - objSize %x, bytes trans %x",
               pMe->dwFileSize, pN->data.ProgressInfo.numBytes, 0 );

    pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_OPP );
    IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
  }
  return;
}

static void OEMBTExtOPP_ReadCb( OEMBTExtOPP_EvCb* pEvCb )
{
  OEMBTExtOPPobj_t* pMe = OEMBTExtOPP_FindMe( pEvCb->appId );
  uint32 bytesRead = 0;
  bt_cmd_status_type status = BT_CS_GN_SUCCESS;

  if ( pMe == NULL )
  {
    // how did we get here?
    TASKLOCK();
    pEvCb->bInUse = FALSE;
    TASKFREE();
    MSG_FATAL( "ReadCb: appID not found, aID=%x", pEvCb->appId, 0, 0 );
    return;
  }

  if ( ((pMe->bIsServer != FALSE) && (pMe->serverConnID != pEvCb->connId)) || 
       ((pMe->bIsServer == FALSE) && (pMe->clientConnID != pEvCb->connId)) )
  {
    MSG_FATAL( "ReadCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else if ( (void*)pMe->pFile != (void*)pEvCb->handle )
  {
    MSG_FATAL( "ReadCb: wrong handle=%x", pEvCb->handle, 0, 0 );
    status = BT_CS_PF_INVALID_HANDLE;
  }
  else if ( pMe->bytesSent >= pMe->dwFileSize )
  {
    MSG_FATAL( "ReadCb: all %d bytes read", pMe->dwFileSize, 0, 0 );
    status = BT_CS_PF_END_OF_FILE;
  }
  else // everything checks out
  {
    if ( pMe->bytesSent == 0 )
    {
      if ( IFILE_Seek( pMe->pFile, _SEEK_START, 0 ) != SUCCESS )
      {
        MSG_FATAL( "ReadCb: seek failed", 0, 0, 0 );
      }
    }
    bytesRead = pEvCb->maxBytes;
    if ( bytesRead > OPP_BUFFER_SIZE )
    {
      bytesRead = OPP_BUFFER_SIZE;
    }
    bytesRead = IFILE_Read( pMe->pFile, pMe->pBuffer, bytesRead );
    pMe->bytesSent += bytesRead;
    if ( bytesRead == 0 )
    {
      MSG_FATAL( "ReadCb: failed to read from obj", 0, 0, 0 );
      status = BT_CS_PF_READ_ERROR;
    }
    // notify the app about progress
    OEMBTExtOPP_NotifyProgress(pMe);
  }
  // done with EvCb
  TASKLOCK();
  pEvCb->bInUse = FALSE;
  TASKFREE();

  if ( pMe->bIsServer)
  {
    bt_cmd_pf_opp_srv_read_done( pMe->appId, pMe->serverConnID,
                                 (bt_pf_opp_handle_type) pMe->pFile,
                                 pMe->pBuffer, bytesRead, status );
  }
  else
  {
    bt_cmd_pf_opp_cli_read_done( pMe->appId, pMe->clientConnID,
                                 (bt_pf_opp_handle_type) pMe->pFile,
                                 pMe->pBuffer, bytesRead, status );
  }
}

static void OEMBTExtOPP_WriteCb( OEMBTExtOPP_EvCb* pEvCb )
{
  OEMBTExtOPPobj_t* pMe = OEMBTExtOPP_FindMe( pEvCb->appId );
  uint32 bytesWritten = 0;
  bt_cmd_status_type status = BT_CS_GN_SUCCESS;

#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
  /* Default to TRUE so Write Done notification is done unless explictly disabled. */
  boolean notifyWriteDoneNeeded = TRUE;
#endif

  if ( pMe == NULL )
  {
    TASKLOCK();
    pEvCb->bInUse = FALSE;
    TASKFREE();
    // how did we get here?
    return;
  }

  if ( ((pMe->bIsServer != FALSE) && (pMe->serverConnID != pEvCb->connId)) || 
       ((pMe->bIsServer == FALSE) && (pMe->clientConnID != pEvCb->connId)) )
  {
    MSG_FATAL( "WriteCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
  }
  else if ( (void*)pMe->pFile != (void*)pEvCb->handle )
  {
    MSG_FATAL( "WriteCb: wrong handle=%x", pEvCb->handle, 0, 0 );
    status = BT_CS_PF_INVALID_HANDLE;
  }
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
  else if (pMe->prevWriteStatus != BT_CS_GN_SUCCESS) 
  {
    /* Error on previous write operation, don't do this write, return status. */
    status = pMe->prevWriteStatus;

    MSG_FATAL( "WriteCb: Previous Write Error! Status = 0x%X", pMe->prevWriteStatus,0,0 );
  }
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */
  else if ( pEvCb->maxBytes == 0 )
  {
    MSG_FATAL( "WriteCb: nothing to write", 0, 0, 0 );
  }
  else if ( IFILEMGR_GetFreeSpace( pMe->pFileMgr, NULL ) < pEvCb->maxBytes )
  {
    MSG_FATAL( "WriteCb: EFS full, failed to write to obj", 0, 0, 0 );
    status = BT_CS_PF_OBEX_DATABASE_FULL;
  }
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
  /* Check for NULL buffer and if size is larger than buffer size. */
  else if (pMe->pBuffer == NULL || pEvCb->maxBytes > OPP_BUFFER_SIZE) 
  {
    /* Error, can't copy to save buffer, shouldn't happen. */
    MSG_FATAL( "WriteCb: Save Buffer Error, pBuffer:0x%X, maxBytes:%d", 
               pMe->pBuffer, pEvCb->maxBytes, 0 );
    
    status = BT_CS_PF_WRITE_ERROR;
  }
  else
  {
    /* Copy data to saved buffer. */
    MSG_FATAL("OEMBTExtOPP_WriteCb: Copy to saved buffer",0, 0, 0 );
    MEMCPY( (void*)pMe->pBuffer, (void*)pEvCb->pSrc, pEvCb->maxBytes);

    MSG_FATAL("OEMBTExtOPP_WriteCb: Sending Write Done Notify",0, 0, 0 );
    if ( pMe->bIsServer)
    {
      bt_cmd_pf_opp_srv_write_done( pMe->appId, pMe->serverConnID,
                                    (bt_pf_opp_handle_type) pMe->pFile, status );
    }
    else
    {
      bt_cmd_pf_opp_cli_write_done( pMe->appId, pMe->clientConnID,
                                    (bt_pf_opp_handle_type) pMe->pFile, status );
    }

    /* Write Done notification is done so not needed to be done after File write. */
    notifyWriteDoneNeeded = FALSE;

    MSG_FATAL("OEMBTExtOPP_WriteCb: Call IFILE_Write",0, 0, 0 );
    if ( (bytesWritten = IFILE_Write( (IFile*)pEvCb->handle, pMe->pBuffer, 
                                       pEvCb->maxBytes )) == 0 )
    {
      MSG_FATAL( "WriteCb: failed to write to obj", 0, 0, 0 );
      status = BT_CS_PF_WRITE_ERROR;

    }
    else
    {
      MSG_FATAL("WriteCb: FILE Write Done: maxBytes-0x%X bytesWritten-0x%X",pEvCb->maxBytes, bytesWritten,0 );

      pMe->bytesRcvd += bytesWritten;
      status = BT_CS_GN_SUCCESS;
      OEMBTExtOPP_NotifyProgress(pMe);
    }
  }

  // done with EvCb
  TASKLOCK();
  pEvCb->bInUse = FALSE;
  TASKFREE();

  /* Write Done notification is done so not needed to be done after File write. */
  if (notifyWriteDoneNeeded == TRUE)
  {
    MSG_FATAL("OEMBTExtOPP_WriteCb: Sending Write Done Notify",0, 0, 0 );
    if ( pMe->bIsServer)
    {
      bt_cmd_pf_opp_srv_write_done( pMe->appId, pMe->serverConnID,
                                    (bt_pf_opp_handle_type) pMe->pFile, status );
    }
    else
    {
      bt_cmd_pf_opp_cli_write_done( pMe->appId, pMe->clientConnID,
                                    (bt_pf_opp_handle_type) pMe->pFile, status );
    }
  }

  /* Save current status as previous for next call. */
  pMe->prevWriteStatus = status;
  MSG_FATAL("OEMBTExtOPP_WriteCb: Saved Write Status: 0x%X",pMe->prevWriteStatus, 0, 0 );

#else
  else if ( (bytesWritten = IFILE_Write( pMe->pFile, pEvCb->pSrc, 
                                         pEvCb->maxBytes )) == 0 )
  {
    MSG_FATAL( "WriteCb: failed to write to obj", 0, 0, 0 );
    status = BT_CS_PF_WRITE_ERROR;
  }
  else
  {
    pMe->bytesRcvd += bytesWritten;
    status = BT_CS_GN_SUCCESS;
    OEMBTExtOPP_NotifyProgress(pMe);
  }

  // done with EvCb
  TASKLOCK();
  pEvCb->bInUse = FALSE;
  TASKFREE();

  if ( pMe->bIsServer)
  {
    bt_cmd_pf_opp_srv_write_done( pMe->appId, pMe->serverConnID,
                                  (bt_pf_opp_handle_type) pMe->pFile, status );
  }
  else
  {
    bt_cmd_pf_opp_cli_write_done( pMe->appId, pMe->clientConnID,
                                  (bt_pf_opp_handle_type) pMe->pFile, status );
  }

#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */

}

static void OEMBTExtOPP_CloseCb( OEMBTExtOPP_EvCb* pEvCb )
{
  OEMBTExtOPPobj_t* pMe = OEMBTExtOPP_FindMe( pEvCb->appId );
  AEEBTNotification* notif;

  if ( pMe == NULL )
  {
    TASKLOCK();
    pEvCb->bInUse = FALSE;
    TASKFREE();
    // how did we get here?
    return;
  }

  if ( ((pMe->bIsServer != FALSE) && (pMe->serverConnID != pEvCb->connId)) || 
       ((pMe->bIsServer == FALSE) && (pMe->clientConnID != pEvCb->connId)) )
  {
    MSG_FATAL( "CloseCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
  }
  else if ( pMe->state <= AEEBT_OPP_STATE_CONNECTED )
  {
    MSG_FATAL( "CloseCb: close req unexp, st=%x", pMe->state, 0, 0 );
  }
  else if ( (void*)pMe->pFile != (void*)pEvCb->handle )
  {
    MSG_FATAL( "CloseCb: wrong handle=%x", pEvCb->handle, 0, 0 );
  }
  else
  {
    if ( pMe->pFile != NULL ) 
    {
      IFILE_Release( pMe->pFile );
      pMe->pFile = NULL;
    }

    if ( pMe->bIsServer != FALSE )
    {
      notif = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
      if ( notif != NULL )
      {
        if ( pMe->state == AEEBT_OPP_STATE_PUSH_STARTED )
        {
          notif->uID = AEEBT_OPP_EVT_OBJ_PUSHED;
        }
        else
        {
          notif->data.OppObject.pszName = pMe->szFileName;
          notif->uID = AEEBT_OPP_EVT_OBJ_PULLED;
        }
        notif->data.uError = OEMBTExt_MapCmdStatus( pEvCb->status );

        notif->uID = (notif->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_OPP );
        IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, notif, pMe->pac );
      }
      else
      {
        MSG_FATAL( "CloseCb: no free notifications",0,0,0);
      }
    }
    pMe->state = AEEBT_OPP_STATE_CONNECTED;
  }
  TASKLOCK();
  pEvCb->bInUse = FALSE;
  TASKFREE();
}

static void OEMBTExtOPP_PushDoneCb( OEMBTExtOPP_EvCb* pEvCb )
{
  OEMBTExtOPPobj_t* pMe = OEMBTExtOPP_FindMe( pEvCb->appId );
  AEEBTNotification* notif;

  if ( pMe == NULL )
  {
    TASKLOCK();
    pEvCb->bInUse = FALSE;
    TASKFREE();
    // how did we get here?
    return;
  }

  if ( pMe->bIsServer == FALSE && pMe->clientConnID != pEvCb->connId )
  {
    MSG_FATAL( "PushDoneCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
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
      pMe->state = AEEBT_OPP_STATE_CONNECTED;
      notif->uID = AEEBT_OPP_EVT_OBJ_PUSHED;
      notif->data.uError = OEMBTExt_MapCmdStatus( pEvCb->status );

      notif->uID = (notif->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_OPP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, notif, pMe->pac );
    }
    else
    {
      MSG_FATAL( "PushDoneCb: no free notifications",0,0,0);
    }
  }
  TASKLOCK();
  pEvCb->bInUse = FALSE;
  TASKFREE();
}

static void OEMBTExtOPP_PullDoneCb( OEMBTExtOPP_EvCb* pEvCb )
{
  OEMBTExtOPPobj_t* pMe = OEMBTExtOPP_FindMe( pEvCb->appId );
  AEEBTNotification* notif;

  if ( pMe == NULL )
  {
    TASKLOCK();
    pEvCb->bInUse = FALSE;
    TASKFREE();
    // how did we get here?
    return;
  }

  if ( pMe->bIsServer == FALSE && pMe->clientConnID != pEvCb->connId )
  {
    MSG_FATAL( "PushDoneCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
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
      pMe->state = AEEBT_OPP_STATE_CONNECTED;
      notif->uID = AEEBT_OPP_EVT_OBJ_PULLED;
      notif->data.OppObject.pszName = pMe->szFileName;
      notif->data.uError = OEMBTExt_MapCmdStatus( pEvCb->status );

      notif->uID = (notif->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_OPP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, notif, pMe->pac );
    }
    else
    {
      MSG_FATAL( "PushDoneCb: no free notifications",0,0,0);
    }
  }
  TASKLOCK();
  pEvCb->bInUse = FALSE;
  TASKFREE();
}

static void OEMBTExtOPP_DisconnectCb( OEMBTExtOPP_EvCb* pEvCb )
{
  OEMBTExtOPPobj_t* pMe = OEMBTExtOPP_FindMe( pEvCb->appId );
  AEEBTNotification* notif;

  MSG_FATAL("***zzg OEMBTExtOPP_DisconnectCb pMe=%x, pEvCb->connId=%x***", pMe, pEvCb->connId, 0);

  if ( pMe == NULL )
  {
    TASKLOCK();
    pEvCb->bInUse = FALSE;
    TASKFREE();
    // how did we get here?
    return;
  }

  MSG_FATAL("***zzg OEMBTExtOPP_DisconnectCb bIsServer=%x, serverConnID=%x, clientConnID=%x***", 
  				pMe->bIsServer, pMe->serverConnID, pMe->clientConnID);

  if ( (pMe->bIsServer != FALSE) && (pMe->serverConnID != pEvCb->connId) ||
       (pMe->bIsServer == FALSE) && (pMe->clientConnID != pEvCb->connId) )
  {
    MSG_FATAL( "DisconnectCb: wrong conn id=%x", pEvCb->connId, 0, 0 );
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
        pMe->state = AEEBT_OPP_STATE_INIT;
        pMe->clientConnID = BT_PF_OPP_NO_CONN_ID;
      }
      else
      {
        pMe->state = AEEBT_OPP_STATE_REGISTERED;
        pMe->serverConnID = BT_PF_OPP_NO_CONN_ID;
        notif->data.bdAddr = pMe->BDAddr;
      }

      notif->uID = AEEBT_OPP_EVT_DISCONNECTED;
      notif->data.uError = AEEBT_OPP_ERR_NONE;
    
      notif->uID = (notif->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_OPP );
      IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, notif, pMe->pac );
    }
    else
    {
      MSG_FATAL( "DisconnectCb: no free notifications",0,0,0);
    }
  }
  TASKLOCK();
  pEvCb->bInUse = FALSE;
  TASKFREE();

  // Do server dereg if it is pending (after force disconnect)
  if ( pMe->bDoDeregister )
  {
    pMe->bDoDeregister = FALSE;
    if ( bt_cmd_pf_opp_srv_deregister( pMe->appId ) != BT_CS_GN_PENDING )
    {
      MSG_FATAL( "DisconnectCb: S de-reg failed!", 0, 0, 0 );
    }
  }
}

static AEEBTObjectType OEMBTExtOPP_MapObjType(
  AECHAR* pwName, 
  uint8   nameLen,
  char*   pszType )
{
  char suffix[ 5 ];
  uint16 i;
  AEEBTObjectType objType;

  if (pszType[ 0 ] != NULL) /* first char in type */
  {
    if ( STRICMP( pszType, OI_OBEX_VCARD_TYPE ) == 0 )
    {
      objType = AEEBT_OPP_VCARD;
    }
    else if ( STRICMP( pszType, OI_OBEX_VCALENDAR_TYPE ) == 0 )
    {
      objType = AEEBT_OPP_VCAL;
    }
    else if ( STRICMP( pszType, OI_OBEX_VNOTE_TYPE ) == 0 )
    {
      objType = AEEBT_OPP_VNOTE;
    }
    else if ( STRICMP( pszType, OI_OBEX_VMESSAGE_TYPE ) == 0 )
    {
      objType = AEEBT_OPP_VMESSAGE;
    }
    else if ( STRICMP( pszType, OI_OBEX_ICALENDAR_TYPE ) == 0 )
    {
      objType = AEEBT_OPP_ICAL;
    }
    else
    {
      objType = AEEBT_OPP_UNKNOWN_TYPE;
    }
  }
  else
  {
    for (i = 0; i < 4; ++i) 
    {
      suffix[i] = (char)(*(pwName + nameLen - 4 + i) & 0xFF);
    }
    suffix[i] = 0;

    if ( STRICMP( suffix, OI_OBEX_VCARD_SUFFIX ) == 0 )
    {
      objType = AEEBT_OPP_VCARD;
    }
    else if ( STRICMP( suffix, OI_OBEX_VCALENDAR_SUFFIX ) == 0 )
    {
      objType = AEEBT_OPP_VCAL;
    }
    else if ( STRICMP( suffix, OI_OBEX_VNOTE_SUFFIX ) == 0 )
    {
      objType = AEEBT_OPP_VNOTE;
    }
    else if ( STRICMP( suffix, OI_OBEX_VMESSAGESUFFIX ) == 0 )
    {
      objType = AEEBT_OPP_VMESSAGE;
    }
    else if ( STRICMP( suffix, OI_OBEX_ICALENDAR_SUFFIX ) == 0 )
    {
      objType = AEEBT_OPP_ICAL;
    }
    else
    {
      objType = AEEBT_OPP_UNKNOWN_TYPE;
    }
  }

  return objType;
}

static OEMBTExtOPP_EvCb* OEMBTExtOPP_GetFreeEvCb( void )
{
  uint8 i;
  OEMBTExtOPP_EvCb* pEvCb = NULL;

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


static void OEMBTExtOPP_OpenCb( OEMBTExtOPP_EvCb* pEvCb )
{
  OEMBTExtOPPobj_t*  pMe = OEMBTExtOPP_FindMe( pEvCb->appId );
  bt_cmd_status_type status = BT_CS_GN_SUCCESS;
  ACONTEXT*          pCurAC;
   // Remote object name
  char  szVCardName[ AEEBT_MAX_FILE_NAME + 1 ];
  char* pszName = NULL;

  if ( (pMe == NULL) || ( pMe->clientConnID != pEvCb->connId ) )
  {
    MSG_FATAL( "OPPEvCb: C open wr req, wrong conn id=%x", 
               pEvCb->connId, 0, 0 );
    status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;

    TASKLOCK();
    pEvCb->bInUse = FALSE;
    TASKFREE();
    return;
  }
  else if ( pMe->state != AEEBT_OPP_STATE_PULL_STARTED )
  {
    MSG_FATAL( "OPPEvCb: C open wr req unexpected", 0, 0, 0 );
    status = BT_CS_PF_INVALID_STATE;
  }
  else if ( pMe->pFile == NULL )
  {
    MSG_FATAL( "OPPEvCb: C open wr req, no obj handle", 0, 0, 0 );
    status = BT_CS_PF_OBEX_PRECONDITION_FAILED;
  }

  pCurAC = ENTER_APP_CONTEXT( pMe->pac );

  // close previously opened file with default name
  if ( pMe->pFile ) 
  {
    IFILE_Release( pMe->pFile );
    pMe->pFile = NULL;
  }

  // remove default object
  if ( (IFILEMGR_Test( pMe->pFileMgr, pMe->szFileName ) == SUCCESS) &&
       (IFILEMGR_Remove( pMe->pFileMgr, pMe->szFileName ) != SUCCESS) )
  {
    MSG_FATAL( "OPPEvCb: C open wr req, unable to del def f", 0, 0, 0 );
  }
     
  if ( pEvCb->nameLen != 0 )
  {
    // use the actual name of the object to save it
    AEEBT_FROM_WSTR( (const AECHAR*) pEvCb->wName, 
                     szVCardName, 
                     sizeof(szVCardName) );

    // replace default filename with the actual object name
    pszName = STRRCHR( pMe->szFileName, DIRECTORY_CHAR );
    if ( pszName == NULL )
    {
      pszName = pMe->szFileName;
    }
    else
    {
      pszName++;
    }
    STRLCPY( pszName, szVCardName, 
             (sizeof( pMe->szFileName ) - (pszName - pMe->szFileName)) );
  }

  // remove object if it exists
  if ( (IFILEMGR_Test( pMe->pFileMgr, pMe->szFileName ) == SUCCESS) &&
       (IFILEMGR_Remove( pMe->pFileMgr,pMe->szFileName ) != SUCCESS) )
  {
    MSG_FATAL( "OPPEvCb: C open wr req, unable to delete file", 0, 0, 0 );
    status = BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR;
  }
  else
  {
    pMe->pFile = 
      IFILEMGR_OpenFile( pMe->pFileMgr, pMe->szFileName, _OFM_CREATE );

    if ( pMe->pFile == NULL )
    {
      MSG_FATAL( "OPPEvCb: C open wr req, file open failed", 0, 0, 0 );
      status = BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR;
    }
    else
    {
       pMe->dwFileSize = pEvCb->objSize;
    }
  }
  (void) LEAVE_APP_CONTEXT( pCurAC );

  bt_cmd_pf_opp_cli_open_write_done( pMe->appId, pMe->clientConnID, 
                                     (bt_pf_opp_handle_type)pMe->pFile, 
                                     status );
  TASKLOCK();
  pEvCb->bInUse = FALSE;
  TASKFREE();

  return;
}

static void OEMBTExtOPP_SrvOpenCb( OEMBTExtOPP_EvCb* pEvCb )
{
    bt_cmd_status_type status = BT_CS_GN_SUCCESS;
    AEEBTNotification*  pN    = NULL;
    OEMBTExtOPPobj_t* pMe     = OEMBTExtOPP_FindMe ( pEvCb->appId );
#ifdef CUST_EDITION
	uint32 dwFreeSpace = 0;
#endif
    if( pMe == NULL )
    {
      MSG_FATAL( "OEMBTExtOPP_SrvOpenCb - Can't get pMe.",
                 0, 0, 0 );
      TASKLOCK();
      pEvCb->bInUse = FALSE;
      TASKFREE();
      return;
    }
  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_FATAL( "OPPEvCb - Can't get free not.",
               0, 0, 0 );
    TASKLOCK();
    pEvCb->bInUse = FALSE;
    TASKFREE();
    return;
  }

  pN->data.uError  = AEEBT_OPP_ERR_NONE;

#ifdef CUST_EDITION
  IFILEMGR_GetFreeSpaceEx(pMe->pFileMgr, AEEFS_CARD0_DIR, NULL, &dwFreeSpace);	//Check the TCard Free Space

  MSG_FATAL("***zzg OEMBTExtOPP_SrvOpenCb 2 FreeSize=%d***", dwFreeSpace, 0, 0);
#endif
  if ( (pMe->serverConnID == pEvCb->connId) &&
       (pMe->state == AEEBT_OPP_STATE_CONNECTED) &&
       (pMe->bPushAllowed != FALSE) &&
#ifdef CUST_EDITION
       (dwFreeSpace>pEvCb->objSize)
#else
       (IFILEMGR_GetFreeSpace( pMe->pFileMgr, NULL ) > pEvCb->objSize)
#endif
       )
  {
    pMe->state = AEEBT_OPP_STATE_PUSH_REQ_PENDING;
    pMe->dwFileSize = pEvCb->objSize;
    pN->uID    = AEEBT_OPP_EVT_PUSH_REQ;
    bt_pf_copy_unicode_str(
      (uint16*)pMe->wName,
      (uint16*)pEvCb->wName,
      AEEBT_MAX_FILE_NAME,
      NULL );
    pN->data.OppObject.pwName = pMe->wName;
    pN->data.OppObject.objType = 
      OEMBTExtOPP_MapObjType( (AECHAR*)pEvCb->wName, pEvCb->nameLen,
                              (char*)pEvCb->wType );
    pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_OPP );
    IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
    // done with EvCb
    TASKLOCK();
    pEvCb->bInUse = FALSE;
    TASKFREE();

    return;
  }

    // error conditions
    if ( pMe->serverConnID != pEvCb->connId )
    {
      MSG_FATAL( "OPPEvCb: S open wr req, wrong conn id=%x", 
                 pEvCb->connId, 0, 0 );
      status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
    }
    else if ( pMe->state != AEEBT_OPP_STATE_CONNECTED )
    {
      MSG_FATAL( "OPPEvCb: S open wr req unexpected", 0, 0, 0 );
      status = BT_CS_PF_INVALID_STATE;
    }
    else
    {
      status = OI_OBEX_SERVICE_UNAVAILABLE;
    }

    bt_cmd_pf_opp_srv_open_write_done( pMe->appId, pEvCb->connId, 
                                       (bt_pf_opp_handle_type)NULL, status );

    IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );

    TASKLOCK();
    pEvCb->bInUse = FALSE;
    TASKFREE();

    return;
}

// called from BT task context; so just queue the event
static void OEMBTExtOPP_EventCallback( bt_ev_msg_type* ev_msg_ptr )
{
  OEMBTExtOPPobj_t*   pMe   = OEMBTExtOPP_FindMe( ev_msg_ptr->ev_hdr.bt_app_id );
  OEMBTExtOPP_EvCb*   pEvCb = NULL;
  AEEBTNotification*  pN    = NULL;
  bt_cmd_status_type  status;

  if ( pMe == NULL )
  {
    return; // probably for an app that no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->pNotifier );
  if( pN == NULL )
  {
    MSG_FATAL( "OPPEvCb - Can't get free not.",
               0, 0, 0 );
    return;
  }

  pN->data.uError  = AEEBT_OPP_ERR_NONE;

  MSG_FATAL("***zzg OEMBTExtOPP_EventCallback ev_type=%x***", ev_msg_ptr->ev_hdr.ev_type, 0, 0);

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
    {
      if ( ! OEMBTExtOPP_HandleCmdDone( &ev_msg_ptr->ev_msg.ev_gn_cmd_done,
                                        pN, pMe ) )
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return; // no need to send notification to app
      }
      break;
    } // end of case BT_EV_GN_CMD_DONE
    case BT_EV_PF_OPP_CLI_CON_CFM:
    {
      MSG_FATAL( "OPPEvCb: C con cfm, id=%x stat=%x", 
               ev_msg_ptr->ev_msg.ev_opp_cli_con_cfm.conn_id,
               ev_msg_ptr->ev_msg.ev_opp_cli_con_cfm.status, 0 );
      if (ev_msg_ptr->ev_msg.ev_opp_cli_con_cfm.status == BT_CS_GN_SUCCESS )
      {
        pMe->state = AEEBT_OPP_STATE_CONNECTED;
        pMe->clientConnID = ev_msg_ptr->ev_msg.ev_opp_cli_con_cfm.conn_id;
        pN->uID = AEEBT_OPP_EVT_CONNECTED;
      }
      else
      {
        pMe->state = AEEBT_OPP_STATE_INIT;
        pN->uID = AEEBT_OPP_EVT_CONN_FAILED;

		MSG_FATAL("***zzg AEEBT_OPP_EVT_CONN_FAILED 2 cmd_status=%x***", ev_msg_ptr->ev_msg.ev_opp_cli_con_cfm.status, 0, 0);
      }
      break;
    }
    case BT_EV_PF_OPP_CLI_CON_PROG_IND:
    {
      MSG_FATAL( "OPPEvCb: C con prog ind, id=%x stat=%x", 
               ev_msg_ptr->ev_msg.ev_opp_cli_con_prog_ind.conn_id,
               ev_msg_ptr->ev_msg.ev_opp_cli_con_prog_ind.status, 0 );
      if (ev_msg_ptr->ev_msg.ev_opp_cli_con_prog_ind.status == BT_CS_GN_SUCCESS )
      {
        pMe->clientConnID = ev_msg_ptr->ev_msg.ev_opp_cli_con_prog_ind.conn_id;
        pMe->state = AEEBT_OPP_STATE_CONNECTING;
        pN->uID = AEEBT_OPP_EVT_CONNECTING;
      }
      else
      {
        pN->uID = AEEBT_OPP_EVT_CONN_FAILED;

		MSG_FATAL("***zzg AEEBT_OPP_EVT_CONN_FAILED 3 cmd_status=%x***", ev_msg_ptr->ev_msg.ev_opp_cli_con_prog_ind.status, 0, 0);
      }
      break;
    }
    case BT_EV_PF_OPP_CLI_DCN_CFM:
    {
      MSG_FATAL( "OPPEvCb: C dcn cfm, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_opp_cli_dcn_cfm.status, 
               ev_msg_ptr->ev_msg.ev_opp_cli_dcn_cfm.conn_id, 0 );

      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_opp_cli_dcn_cfm.conn_id;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_DisconnectCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_CLI_PSH_DNE:
    {
      MSG_FATAL( "OPPEvCb: C psh dne, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_opp_cli_push_done.status, 
               ev_msg_ptr->ev_msg.ev_opp_cli_push_done.conn_id, 0 );

      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_opp_cli_push_done.conn_id;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_opp_cli_push_done.status;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_PushDoneCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_CLI_PUL_DNE:
    {
      MSG_FATAL( "OPPEvCb: C pull dne, stat=%x id=%x", 
               ev_msg_ptr->ev_msg.ev_opp_cli_pull_done.status, 
               ev_msg_ptr->ev_msg.ev_opp_cli_pull_done.conn_id, 0 );

      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_opp_cli_pull_done.conn_id;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_opp_cli_pull_done.status;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_PullDoneCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_CLI_OPEN_READ_REQ:
    {
      // driver requesting client app to open object for reading; this means
      // client app had issued PUSH command; at this point a connection
      // should already be present and the object already opened for reading
      bt_pf_ev_opp_cli_open_read_req_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_opp_cli_open_read_req;

      MSG_FATAL( "OPPEvCb: C open rd req, id=%x st=%x", 
               pEvt->conn_id, pMe->state, 0 );
      status = BT_CS_GN_SUCCESS;
      if ( pMe->clientConnID != pEvt->conn_id )
      {
        MSG_FATAL( "OPPEvCb: C open rd req, wrong conn id=%x", 
                   pEvt->conn_id, 0, 0 );
        status = BT_CS_PF_OBEX_CONNECTION_NOT_FOUND;
      }
      else if ( pMe->state != AEEBT_OPP_STATE_PUSH_STARTED )
      {
        MSG_FATAL( "OPPEvCb: C open rd req unexpected", 0, 0, 0 );
        status = BT_CS_PF_INVALID_STATE;
      }
      else if ( pMe->pFile == NULL )
      {
        MSG_FATAL( "OPPEvCb: C open rd req, no obj handle", 0, 0, 0 );
        status = BT_CS_PF_OBEX_PRECONDITION_FAILED;
      }
      else if ( WSTRCMP( pMe->wName, (AECHAR*)pEvt->name_str ) != 0 )
      {
        MSG_FATAL( "OPPEvCb: obj names differ", 0, 0, 0 );
        status = BT_CS_PF_FAIL;
      }
      bt_cmd_pf_opp_cli_open_read_done( 
        pMe->appId, pMe->clientConnID, (bt_pf_opp_handle_type)pMe->pFile, 
        NAME_ONLY( (AECHAR*) pEvt->name_str ), pMe->pszType, pMe->dwFileSize, 
        status );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_CLI_OPEN_WRITE_REQ:
    {    
      int i = 0;            
      bt_pf_ev_opp_cli_open_write_req_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_opp_cli_open_write_req;

      MSG_FATAL( "OPPEvCb: C open wr req, id=%x st=%x", 
               pEvt->conn_id, pMe->state, 0 );

      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = pEvt->conn_id;
        pEvCb->nameLen  = pEvt->name_len;
        pEvCb->objSize  = pEvt->obj_size;

        for ( i=0; i < (pEvt->name_len) && i < ( (sizeof(pEvCb->wName)/sizeof(AECHAR))-1); i++)
        {
          pEvCb->wName[ i ] = pEvt->name_str[ i ];
        }
        pEvCb->wName[ i ] = 0;

        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_OpenCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_CLI_CLOSE_REQ:
    {
      // current operation is complete; close object
      MSG_FATAL( "OPPEvCb: C close req, id=%x st=%x", 
               ev_msg_ptr->ev_msg.ev_opp_cli_close_req.conn_id, pMe->state, 0 );
      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_opp_cli_close_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_opp_cli_close_req.handle;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_CloseCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_CLI_READ_REQ:
    {
      // driver requesting the next chunk of data from the object to be pushed
      // to server
      MSG_FATAL( "OPPEvCb: C read req, id=%x h=%x", 
               ev_msg_ptr->ev_msg.ev_opp_cli_read_req.conn_id, 
               ev_msg_ptr->ev_msg.ev_opp_cli_read_req.handle, 0 );
      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_opp_cli_read_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_opp_cli_read_req.handle;
        pEvCb->maxBytes = ev_msg_ptr->ev_msg.ev_opp_cli_read_req.max_read;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_ReadCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_CLI_WRITE_REQ:
    {
      // driver giving the next block of data from the object pulled from
      // server
      MSG_FATAL( "OPPEvCb: C wr req, id=%x h=%x", 
               ev_msg_ptr->ev_msg.ev_opp_cli_write_req.conn_id, 
               ev_msg_ptr->ev_msg.ev_opp_cli_write_req.handle, 0 );
      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_opp_cli_write_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_opp_cli_write_req.handle;
        pEvCb->pSrc     = ev_msg_ptr->ev_msg.ev_opp_cli_write_req.buffer_ptr;
        pEvCb->maxBytes = ev_msg_ptr->ev_msg.ev_opp_cli_write_req.buffer_len;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_WriteCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_SRV_CON_IND:
    {
      MSG_FATAL( "OPPEvCb: S con ind, id=%x st=%x", 
               ev_msg_ptr->ev_msg.ev_opp_srv_con_ind.conn_id, pMe->state, 0 );
      if ( (pMe->state == AEEBT_OPP_STATE_INIT) ||
           (pMe->state >= AEEBT_OPP_STATE_CONNECTED) )
      {
        IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
        return; // already connected or not ready for connection
      }
      pMe->serverConnID = ev_msg_ptr->ev_msg.ev_opp_srv_con_ind.conn_id;
      pMe->BDAddr = 
        *((AEEBTBDAddr*)&ev_msg_ptr->ev_msg.ev_opp_srv_con_ind.bd_addr);

      pN->uID = AEEBT_OPP_EVT_CONN_REQ;
      pN->data.bdAddr = pMe->BDAddr;
      break;
    }
    case BT_EV_PF_OPP_SRV_DCN_IND:
    {
      MSG_FATAL( "OPPEvCb: S dcn req, id=%x h=%x", 
               ev_msg_ptr->ev_msg.ev_opp_srv_dcn_ind.conn_id, 0, 0 );

      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_opp_srv_dcn_ind.conn_id;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_DisconnectCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_SRV_OPEN_READ_REQ:
    {
      // client is trying to pull server's default business card
      bt_pf_ev_opp_srv_open_read_req_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_opp_srv_open_read_req;

      MSG_FATAL( "OPPEvCb: S open rd req, id=%x st=%x", 
               pEvt->conn_id, pMe->state, 0 );

      if ( (pMe->serverConnID == pEvt->conn_id) &&
           (pMe->state == AEEBT_OPP_STATE_CONNECTED) &&
           (pMe->bPullAllowed != FALSE) )
      {
        pMe->state = AEEBT_OPP_STATE_PULL_REQ_PENDING;
        pN->uID    = AEEBT_OPP_EVT_PULL_REQ;
        break;
      }

      // error conditions
      if ( pMe->serverConnID != pEvt->conn_id )
      {
        MSG_FATAL( "OPPEvCb: S open rd req, wrong conn id=%x", 
                   pEvt->conn_id, 0, 0 );
        status = OI_OBEX_NOT_FOUND;
      }
      else if ( pMe->state != AEEBT_OPP_STATE_CONNECTED )
      {
        MSG_FATAL( "OPPEvCb: S open rd req unexp, st=%x", pMe->state, 0, 0 );
        status = BT_CS_PF_INVALID_STATE;
      }
      else
      {
        status = OI_OBEX_SERVICE_UNAVAILABLE;
      }
      bt_cmd_pf_opp_srv_open_read_done( 
        pMe->appId, pEvt->conn_id, (bt_pf_opp_handle_type)NULL, 
        (AECHAR*)NULL, (char*) NULL, 0, status );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_SRV_OPEN_WRITE_REQ:
    {
      int i = 0;
      // client trying to push an object to server; notify the app
      // driver is responsible to make sure it is of a type supported by server
      bt_pf_ev_opp_srv_open_write_req_type* pEvt =
        &ev_msg_ptr->ev_msg.ev_opp_srv_open_write_req;

      MSG_FATAL( "OPPEvCb: S open wr req, id=%x st=%x", 
               pEvt->conn_id, pMe->state, 0 );
      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if((pEvCb != NULL))
      {
        pEvCb->event   = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId   = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId  = pEvt->conn_id;
        pEvCb->objSize = pEvt->obj_size;
       
        for (i=0; i<AEEBT_MAX_FILE_NAME ; i++)
        {
          pEvCb->wName[i] = pEvt->name_str[i];
        }
        pEvCb->wName[i] = 0;

        for (i=0; i<AEEBT_MAX_MIME_TYPE_LEN ; i++)
        {
          pEvCb->wType[i] = pEvt->type_str[i];
        }
        pEvCb->wType[i] = 0;
        pEvCb->nameLen = pEvt->name_len;

        CALLBACK_Init ( &pEvCb->cb, OEMBTExtOPP_SrvOpenCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      else
      {
        status = OI_OBEX_SERVICE_UNAVAILABLE;
        bt_cmd_pf_opp_srv_open_write_done( pMe->appId, pEvt->conn_id, 
                                         (bt_pf_opp_handle_type)NULL, status );
      }

      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_SRV_CLOSE_REQ:
    {
      // current operation is complete; close object
      MSG_FATAL( "OPPEvCb: S close req, id=%x st=%x", 
               ev_msg_ptr->ev_msg.ev_opp_cli_close_req.conn_id, pMe->state, 0 );
      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_opp_srv_close_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_opp_srv_close_req.handle;
        pEvCb->status   = ev_msg_ptr->ev_msg.ev_opp_srv_close_req.status;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_CloseCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_SRV_READ_REQ:
    {
      // driver requesting the next chunk of data from the object client
      // is pulling
      MSG_FATAL( "OPPEvCb: S read req, id=%x h=%x", 
               ev_msg_ptr->ev_msg.ev_opp_srv_read_req.conn_id, 
               ev_msg_ptr->ev_msg.ev_opp_srv_read_req.handle, 0 );
      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_opp_srv_read_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_opp_srv_read_req.handle;
        pEvCb->maxBytes = ev_msg_ptr->ev_msg.ev_opp_srv_read_req.max_read;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_ReadCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    case BT_EV_PF_OPP_SRV_WRITE_REQ:
    {
      // driver giving the next chunk of data from the object client
      // is pushing
      MSG_FATAL( "OPPEvCb: S wr req, id=%x h=%x", 
               ev_msg_ptr->ev_msg.ev_opp_srv_write_req.conn_id, 
               ev_msg_ptr->ev_msg.ev_opp_srv_write_req.handle, 0 );
      pEvCb = OEMBTExtOPP_GetFreeEvCb();
      if ( pEvCb != NULL )
      {
        pEvCb->event    = ev_msg_ptr->ev_hdr.ev_type;
        pEvCb->appId    = ev_msg_ptr->ev_hdr.bt_app_id;
        pEvCb->connId   = ev_msg_ptr->ev_msg.ev_opp_srv_write_req.conn_id;
        pEvCb->handle   = ev_msg_ptr->ev_msg.ev_opp_srv_write_req.handle;
        pEvCb->pSrc     = ev_msg_ptr->ev_msg.ev_opp_srv_write_req.buffer_ptr;
        pEvCb->maxBytes = ev_msg_ptr->ev_msg.ev_opp_srv_write_req.buffer_len;
        CALLBACK_Init( &pEvCb->cb, OEMBTExtOPP_WriteCb, pEvCb );
        OEMBTExt_ResumeCallback ( &pEvCb->cb );
      }
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
    default:
    {
      MSG_FATAL( "OPPEvCb - unexpect event %x", 
                 ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | GET_NOTIFIER_MASK( NMASK_BT_OPP );
  IBTEXTNOTIFIER_DoNotify( pMe->pNotifier, pN, pMe->pac );
}

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_OPP)

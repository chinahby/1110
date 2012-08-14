/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppOBEX.c                                */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains OBEX test feature.       */  
/*                                                                           */  
/*              Copyright (c) 2006-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*============================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppOBEX.c#2 $
$DateTime: 2009/06/03 12:43:40 $
$Author: phuongn $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
06/01/09   pn    Fixed critical Lint errors.
05/12/08   VM    Replacing deprecated functions with BREW replacements.
04/10/08   VM    Added BTApp_EnableOBEX()
06/26/07   pn    Removed compiler warnings.
03/30/07   dsn   Typecasting fixes for rct2.1 compiler.
03/15/07   MH    Server disconnect response functionality.
11/27/06   JJ    Modifications done to process passwd and passkey seperately 
                 in ServerRegistration and Clientconnect interfaces.
11/24/06   ns    Fixed compilation error introduced in previous checkin
11/23/06   JJ    Added automation plugins
11/15/06   smr   Added Authentication support for IOBEX.
11/07/06   smr   Initial Version
===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH
#ifdef FEATURE_IOBEX

#include "BTApp_priv.h"		//"BTApp.h"

#include "BTAppUtils.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

/* Client and Server Events */
#define EVT_OBEX_DCN_IND                     1

#define OBEX_PUTFILE_NAME                    "obex_put.txt"
#define OBEX_GETFILE_NAME                    "obex_get.txt"
#define OBEX_FOLDER_NAME                     "obex_folder"

#define MAX_OBEX_HDRS                        5
#define OBEX_UUID                            0x0008
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
/* ==========================================================================
FUNCTION BTApp_OBEXDisConnModelEventHandlerCb
DESCRIPTION
=============================================================================*/
void BTApp_OBEXDisConnModelEventHandlerCb( CBTApp* pMe, 
                                           ModelEvent *pEvent )
{
  AEEOBEXModelNotificationDataType*  pN = NULL;
  ModelEvent                         pModelEvent;

  pN = ( AEEOBEXModelNotificationDataType* )pEvent;
  pModelEvent = pN->base;

  MSG_LOW( "OBEX DISC EVT CB - Evt=%d", pModelEvent.evCode, 0, 0 );
  
  if ( pModelEvent.pModel == pMe->mOBEX.poClient->m_pIModel )
  {
    switch( pModelEvent.evCode )
    {
      case AEE_OBEX_EVT_CLI_DCN_IND:
      {
        BTAPP_POST_USER_EVENT( OBEX, EVT_OBEX_DCN_IND );
        break;
      }
      default:
      {
        MSG_LOW( "OBEX DISC EVT CB - Unexpected Model event %x", 
                 pModelEvent.evCode, 0, 0 );
        break;
      }
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_OBEXSetPathModelEventHandlerCb
DESCRIPTION
=============================================================================*/
static void BTApp_OBEXSetPathModelEventHandlerCb( CBTApp* pMe, 
                                                  ModelEvent *pEvent )
{
  AEEOBEXModelNotificationDataType*  pN = NULL;
  ModelEvent                         pModelEvent;

  pN = ( AEEOBEXModelNotificationDataType* )pEvent;
  pModelEvent = pN->base;

  MSG_LOW( "OBEX SET PATH EVT CB - Evt=%d", pModelEvent.evCode, 0, 0 );

  if ( pModelEvent.pModel == pMe->mOBEX.poClient->m_pIModel )
  {
    switch( pModelEvent.evCode )
    {
      case AEE_OBEX_EVT_CLI_SET_PATH_CFM:
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
        if ( pModelEvent.dwParam == AEE_OBEX_ERR_NONE )
        {
          MSG_LOW( "OBEX SET PATH EVT CB - SetPath SUCCESS", 0, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
          BTApp_ShowMessage( pMe, IDS_OBEX_MSG_SETPATH_SUCCESS, NULL, 3 );
        }
        else
        {
          MSG_LOW( "OBEX SET PATH EVT CB - SetPath Failed with Status %d", 
                   pModelEvent.dwParam, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_OBEX_MSG_SETPATH_FAILED, NULL, 3 );
        }
        break;
      }
      case AEE_OBEX_EVT_CLI_DCN_IND:
      {
        BTAPP_POST_USER_EVENT( OBEX, EVT_OBEX_DCN_IND );
        break;
      }
      default:
        MSG_LOW( "OBEX SET PATH EVT CB - Unexpected Model event %x", 
                 pModelEvent.evCode, 0, 0 );
        break;
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_OBEXPutModelEventHandlerCb
DESCRIPTION
=============================================================================*/
static void BTApp_OBEXPutModelEventHandlerCb( CBTApp* pMe, ModelEvent *pEvent )
{
  AEEOBEXModelNotificationDataType*  pN = NULL;
  ModelEvent                         pModelEvent;
  AECHAR                             wObjName[ AEEBT_MAX_FILE_NAME+1 ];

  pN = ( AEEOBEXModelNotificationDataType* ) pEvent;
  pModelEvent = pN->base;
  MSG_LOW( "OBEX PUT EVT CB - Evt=%d", pModelEvent.evCode, 0, 0 );

  STRTOWSTR(OBEX_PUTFILE_NAME, wObjName, sizeof(wObjName));
  
  if ( pModelEvent.pModel == pMe->mOBEX.poClient->m_pIModel )
  {
    switch( pModelEvent.evCode )
    {
      case AEE_OBEX_EVT_CLI_PUT_CFM:
      {
        switch( pModelEvent.dwParam )
        {
          case AEE_OBEX_ERR_NONE:
          {
            pMe->mOBEX.bPutInProgress = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
            BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT, wObjName, 3 );
            break;
          }
          case AEE_OBEX_ERR_NOT_ALLOWED:
          {
            pMe->mOBEX.bPutInProgress = FALSE;
            MSG_HIGH( "OBEX PUT EVT CB - Put not allowed", 0, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
            BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wObjName, 3 );
            break;
          }
          case AEE_OBEX_ERR_CONTINUE:
          {
            ISHELL_Resume( pMe->a.m_pIShell, &pMe->mOBEX.m_cb );
            break;
          }
          case AEE_OBEX_ERR_FAILED:
          {
            pMe->mOBEX.bPutInProgress = FALSE;
            MSG_HIGH( "OBEX PUT EVT CB - Put Failed", 0, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
            BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wObjName, 3 );
            break;
          }
          default:
          {
            pMe->mOBEX.bPutInProgress = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
            MSG_HIGH( "OBEX PUT EVT CB - Uexpected status %x", 
                      pModelEvent.dwParam, 0, 0 );
            BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wObjName, 3 );
            break;
          }
        }
        break;
      }
      case AEE_OBEX_EVT_CLI_DCN_IND:
      {
        BTAPP_POST_USER_EVENT( OBEX, EVT_OBEX_DCN_IND );
        break;
      }
      default:
      {
        MSG_HIGH( "OBEX PUT EVT CB -Unexpected Model event %x", 
                   pModelEvent.evCode, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
        BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wObjName, 3 );
        break;
      }
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_OBEXGetModelEventHandlerCb
DESCRIPTION
=============================================================================*/
static void BTApp_OBEXGetModelEventHandlerCb( CBTApp* pMe, ModelEvent *pEvent )
{
  AEEOBEXModelNotificationDataType*  pN = NULL;
  ModelEvent                         pModelEvent;
  AEEOBEXAccessRequestType           accReq;
  uint8                              u8Count;
  uint8                              u8HdrCount = 0;
  IFile*                             pFile = NULL;
  AEEOBEXHdrType                     emptyHeader;
  int                                nResult;
  boolean                            bLenHdrPresent = FALSE;
  AECHAR                             wObjName[ AEEBT_MAX_FILE_NAME+1 ];

  pN = ( AEEOBEXModelNotificationDataType* )pEvent;
  pModelEvent = pN->base;
  MEMSET( &emptyHeader, 0, sizeof(emptyHeader) );

  STRTOWSTR( OBEX_GETFILE_NAME, wObjName, sizeof( wObjName ) );
  MSG_LOW( "OBEX GET EVT CB - Evt=%d", pModelEvent.evCode, 0, 0 );
  if ( pModelEvent.pModel == pMe->mOBEX.poClient->m_pIModel )
  {
    switch( pModelEvent.evCode )
    {
      case AEE_OBEX_EVT_CLI_GET_RCV_DATA:
      {
        if ( ( pModelEvent.dwParam == AEE_OBEX_ERR_CONTINUE ) || 
             ( pModelEvent.dwParam == AEE_OBEX_ERR_NONE ) )
        {
          accReq = pN->u.accessReq;
          if ( pMe->mOBEX.bGetInProgress == FALSE )
          {
            for ( u8Count = 0; u8Count < accReq.headerCount; 
                  u8Count++ )
            {
              if ( accReq.headerPtr[ u8Count ].id == 
                  AEE_OBEX_HDR_LENGTH )
              {
                pMe->mOBEX.fileSize = 
                     accReq.headerPtr[ u8Count ].val.length;
                bLenHdrPresent = TRUE;
                break;
              }
            }
            if ( bLenHdrPresent == FALSE )
            {
              MSG_FATAL( "OBEX GET EVT CB - Length Hdr not recieved", 
                         0, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
              BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wObjName, 3 );
              return;
            }
            pFile = IFILEMGR_OpenFile( pMe->mOBEX.pIFileMgr, pMe->mOBEX.szName,
                                       _OFM_CREATE );
            if ( pFile == NULL )
            {
              MSG_FATAL( "OBEX GET EVT CB - File Creation Failed", 0, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
              BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wObjName, 3 );
            }
            else
            {
              pMe->mOBEX.bGetInProgress = TRUE;
            }
          }
          else
          {
            pFile = IFILEMGR_OpenFile( pMe->mOBEX.pIFileMgr, pMe->mOBEX.szName,
                                       _OFM_READWRITE );
          }
          if ( pFile != NULL )
          {
            if ( IFILE_Seek( pFile, _SEEK_START, pMe->mOBEX.bytesRcvd) == 
                 SUCCESS )
            {
              for ( u8Count = 0; u8Count < accReq.headerCount;
                    u8Count++ )
              {
                if ( accReq.headerPtr[ u8Count ].id == AEE_OBEX_HDR_BODY )
                {
                  IFILE_Write( pFile, 
                               accReq.headerPtr[ u8Count ].val.body.data,
                               accReq.headerPtr[ u8Count ].val.body.len);
                  pMe->mOBEX.bytesRcvd += accReq.headerPtr[u8Count].val.body.len;
                  IFILE_Release( pFile );
                  if ( pMe->mOBEX.bytesRcvd <= pMe->mOBEX.fileSize )
                  {
                    nResult = IOBEXClient_Get( pMe->mOBEX.poClient, 
                                               &emptyHeader, u8HdrCount, TRUE);
                    if ( nResult != SUCCESS )
                    {
                      MSG_HIGH( "OBEX GET EVT CB - Get failed, res=%x", 
                                nResult, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
                      BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED,
                                         wObjName, 3 );
                      return;
                    }
                  }
                  else
                  {
                    pMe->mOBEX.bGetInProgress = FALSE;
                    MSG_FATAL( "Recieved Data is more than the file size - %x",
                               pMe->mOBEX.bytesRcvd, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
                    BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, 
                                       wObjName, 3 );
                  }
                  break;
                }
                else if ( accReq.headerPtr[ u8Count ].id == 
                          AEE_OBEX_HDR_END_OF_BODY )
                {
                  IFILE_Write( pFile, 
                               accReq.headerPtr[ u8Count ].val.endOfBody.data,
                               accReq.headerPtr[ u8Count ].val.endOfBody.len);
                  pMe->mOBEX.bytesRcvd += 
                               accReq.headerPtr[ u8Count ].val.endOfBody.len;
                  IFILE_Release( pFile );
                  pMe->mOBEX.bGetInProgress = FALSE;
                  if ( pMe->mOBEX.bytesRcvd == pMe->mOBEX.fileSize )
                  {
                    MSG_LOW( "OBEX GET EVT CB - Get File success", 0, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
                    BTApp_ShowMessage( pMe, IDS_MSG_FILE_GOT, wObjName, 3 );
                  }
                  else
                  {
                    MSG_FATAL( "Recieved Data is more than the file size - %x",
                               pMe->mOBEX.bytesRcvd, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
                    BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, 
                                       wObjName, 3 );
                  }
                  break;
                }
              }
            }
            else
            {
              MSG_FATAL( "OBEX GET EVT CB Could not Seek the Specified File",
                         0, 0, 0 );
              IFILE_Release( pFile );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
              BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wObjName, 3 );
           }
          }
          else
          {
            MSG_FATAL( "OBEX GET EVT CB - File Open Error", 0, 0, 0 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
            BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wObjName, 3 );
          }
        }
        else if ( pModelEvent.dwParam == AEE_OBEX_ERR_FAILED )
        {
          pMe->mOBEX.bGetInProgress = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
          MSG_FATAL( "OBEX GET EVT CB - Get Pkt from Server Failed", 0, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wObjName, 3 );
        }
        break;
      }
      case AEE_OBEX_EVT_CLI_DCN_IND:
      {
        BTAPP_POST_USER_EVENT( OBEX, EVT_OBEX_DCN_IND );
        break;
      }
      default:
        MSG_HIGH( "OBEX GET EVT CB - Unexpected Model event %x",
                   pModelEvent.evCode, 0, 0 );
        break;
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_OBEXConnModelEventHandlerCb
DESCRIPTION
=============================================================================*/
static void BTApp_OBEXConnModelEventHandlerCb( CBTApp* pMe, ModelEvent *pEvent )
{
  AEEOBEXModelNotificationDataType*  pN = NULL;
  ModelEvent                         pModelEvent;

  pN = ( AEEOBEXModelNotificationDataType* )pEvent;
  pModelEvent = pN->base;

  MSG_LOW( "OBEX CONN EVT CB - Evt=%d", pModelEvent.evCode, 0, 0 );

  if ( pModelEvent.pModel == pMe->mOBEX.poClient->m_pIModel )
  {
    switch( pModelEvent.evCode )
    {
      case AEE_OBEX_EVT_CLI_AUTH_REQ:
      {
        MSG_LOW( "OBEX CONN EVT CB - CLI AUTH REQ REVD", 0, 0, 0 );
        pMe->mOBEX.bUidReqd = pN->u.authReq.bUserIDRequired;
        if ( pMe->mOBEX.bUidReqd == TRUE )
        {
          MSG_LOW( "OBEX CONN EVT CB - UID REQD", 0, 0, 0 );
          BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX_USERID );
        }
        else
        {
          MSG_LOW( "UID NOT REQD", 0, 0, 0 );
          BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX_PASSWORD );
        }
        break;
      }
      case AEE_OBEX_EVT_CLI_AUTH_RESP:
      {
        MSG_LOW( "OBEX CONN EVT CB - CLI AUTH REQ REVD", 0, 0, 0 );
        pMe->mOBEX.bUidReqd = pMe->mOBEX.bDoAuthWithUid;
        if ( pMe->mOBEX.bUidReqd == TRUE )
        {
          MSG_LOW( "OBEX CONN EVT CB - UID REQD", 0, 0, 0 );
          BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX_USERID );
        }
        else
        {
          MSG_LOW( "OBEX CONN EVT CB - UID NOT REQD", 0, 0, 0 );
          BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX_PASSWORD );
        }
        break;
      }
      case AEE_OBEX_EVT_CLI_CON_CFM:
      {
        pMe->mOBEX.bPutInProgress = FALSE;
        pMe->mOBEX.bGetInProgress = FALSE;
		
        STRLCPY(pMe->mOBEX.szCurrentFolder, BTAPP_ROOT_DIR, sizeof( pMe->mOBEX.szCurrentFolder));
		
        pMe->mOBEX.bConnected = TRUE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
        BTApp_BuildTopMenu( pMe );
        break;
      }
      case AEE_OBEX_EVT_CLI_CONN_FAILED:
      {
        pMe->mOBEX.bConnected = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
        BTAPP_POST_USER_EVENT( OBEX, EVT_OBEX_DCN_IND );
        break;
      }
      case AEE_OBEX_EVT_CLI_DCN_IND:
      {
        BTAPP_POST_USER_EVENT( OBEX, EVT_OBEX_DCN_IND );
        break;
      }
      default:
        break;
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_OBEXSvrModelEventHandlerCb
DESCRIPTION
=============================================================================*/
static void BTApp_OBEXSvrModelEventHandlerCb( CBTApp* pMe, ModelEvent *pEvent )
{
  AEEOBEXModelNotificationDataType*  pN = NULL;
  ModelEvent                         pModelEvent;
  IFile*                             pFile = NULL;
  FileInfo                           fileInfo;
  boolean                            bNameHdrPresent = FALSE;
  boolean                            bBodyPresent = FALSE;
  boolean                            bLenHdrPresent = FALSE;
  AEEOBEXAccessRequestType           accReq;
  AEEOBEXSetPathRequestType          setPathReq;
  AEEOBEXCmdStatusType               cmdStatus;
  AEEOBEXHdrType*                    pHeaders = NULL;
  AEEOBEXHdrType                     emptyHeader;
  byte*                              pDataBuffer = NULL;
  uint8                              u8Count;
  uint8                              u8HdrCount = 0;
  uint32                             dwBytestoSend = 0;
  int                                nResult = SUCCESS;
  char*                              pObjName = NULL;
  char                               szObjectName[ AEEBT_MAX_FILE_NAME + 1 ];

  pN = (AEEOBEXModelNotificationDataType*)pEvent;
  pModelEvent = pN->base;

  MSG_LOW( "OBEX SRV EVT CB - Evt=%d", pModelEvent.evCode, 0, 0 );
  
  MEMSET( &emptyHeader, 0, sizeof( emptyHeader ) );
  switch( pModelEvent.evCode )
  {
    case AEE_OBEX_EVT_CONNECTED:
    {
      if ( pModelEvent.dwParam == 0 )
      {
        pMe->mOBEX.bPutInProgress = FALSE;
        pMe->mOBEX.bGetInProgress = FALSE;
				
        STRLCPY( pMe->mOBEX.szCurrentFolder, BTAPP_ROOT_DIR, 
                 sizeof( pMe->mOBEX.szCurrentFolder ) );
        pMe->mOBEX.bConnected = TRUE;
        MSG_LOW( "OBEX SRV EVT CB - Connected", 0, 0, 0 );
      }
      else
      {
        MSG_FATAL( "OBEX SRV EVT CB - Connection Failed", 0, 0, 0 );
      }
      BTApp_BuildTopMenu( pMe );
      break;
    }
    case AEE_OBEX_EVT_SRV_DCN_IND:
    {
      nResult = IOBEXServer_DisconnectResponse(pMe->mOBEX.poServer, NULL, 
                                               0, AEE_OBEX_STAT_SUCCESS );

      if ( nResult != SUCCESS )
      {
        MSG_FATAL( "OBEX SRV EVT CB - Disconnect response Failed - %x", 
                                                        nResult, 0, 0 );
      }

      BTAPP_POST_USER_EVENT( OBEX, EVT_OBEX_DCN_IND );
      break;
    }
    case AEE_OBEX_EVT_SRV_GET_REQ:
    {
      accReq = pN->u.accessReq;
      MSG_LOW( "OBEX SRV EVT CB - GET request received", 0, 0, 0 );
      //Check if this is the first GET request for a file/folder
      if ( pMe->mOBEX.bGetInProgress != TRUE )
      {
        // Look for Required headers and setup the File/Folder details
        for ( u8Count = 0; u8Count < accReq.headerCount;
              u8Count++ )
        {
          if ( accReq.headerPtr[ u8Count ].id == 
               AEE_OBEX_HDR_NAME )
          {
            WSTRTOSTR( accReq.headerPtr[ u8Count ].val.name.str, 
                       szObjectName, 
                       accReq.headerPtr[ u8Count ].val.name.len );
            STRLCPY( pMe->mOBEX.szName, pMe->mOBEX.szCurrentFolder, 
                     sizeof( pMe->mOBEX.szName ) );
            STRLCAT( pMe->mOBEX.szName, DIRECTORY_STR, 
                     sizeof( pMe->mOBEX.szName ) );
            STRLCAT( pMe->mOBEX.szName, szObjectName, 
                     sizeof( pMe->mOBEX.szName) );
            bNameHdrPresent = TRUE;
            break;
          }
        }
        if ( bNameHdrPresent == TRUE )
        {
          // Test if the file exists
          if ( IFILEMGR_Test( pMe->mOBEX.pIFileMgr, 
               pMe->mOBEX.szName ) == SUCCESS )
          {
            MSG_LOW( "OBEX SRV EVT CB - File Exist", 0, 0, 0 );
            pMe->mOBEX.bytesSent = 0;
          }
          else
          {
            MSG_HIGH( "OBEX SRV EVT CB - File Does Not Exist", 0, 0, 0 );
            nResult = IOBEXServer_GetResponse( pMe->mOBEX.poServer, 
                                               &emptyHeader, u8HdrCount,
                                               AEE_OBEX_STAT_NOT_FOUND );
            if ( nResult != SUCCESS )
            {
              MSG_FATAL( "OBEX SRV EVT CB - Get response Failed - %x", 
                         nResult, 0, 0 );
            }
            return;
          }
        }
        else
        {
          //Name hdr is mandatory for GET
          MSG_LOW( "OBEX SRV EVT CB - Name Hdr not present", 0, 0, 0 );
          nResult = IOBEXServer_GetResponse( pMe->mOBEX.poServer, 
                                             &emptyHeader, u8HdrCount,
                                             AEE_OBEX_STAT_NAME_REQUIRED );
          if ( nResult != SUCCESS )
          {
            MSG_FATAL( "OBEX SRV EVT CB - Get rsp Failed:%x", nResult, 0, 0 );
          }
          return;
        }
      }
      nResult = IFILEMGR_GetInfo( pMe->mOBEX.pIFileMgr, pMe->mOBEX.szName, 
                                  &fileInfo );
      if ( nResult != SUCCESS )
      {
        MSG_FATAL( "OBEX SRV EVT CB - File Get info Failed Returning - %x",
                   nResult, 0, 0 );
        nResult = IOBEXServer_GetResponse( pMe->mOBEX.poServer, &emptyHeader,
                                           u8HdrCount,
                                           AEE_OBEX_STAT_GET_RESPONSE_ERROR );
        if ( nResult != SUCCESS )
        {
          MSG_FATAL( "OBEX SRV EVT CB - Get rsp Failed - %x", nResult, 0, 0 );
        }
        return;
      }
      if ( fileInfo.attrib == _FA_DIR )
      {
        MSG_FATAL( "OBEX SRV EVT CB - Object is not a File", 0, 0, 0 );
        nResult = IOBEXServer_GetResponse( pMe->mOBEX.poServer, &emptyHeader,
                                           u8HdrCount,
                                           AEE_OBEX_STAT_GET_RESPONSE_ERROR );
        if ( nResult != SUCCESS )
        {
          MSG_FATAL( "OBEX SRV EVT CB - File Get info Failed res=%x", nResult,
                     0, 0 );
        }
        return;
      }
      pFile = IFILEMGR_OpenFile( pMe->mOBEX.pIFileMgr, pMe->mOBEX.szName, 
                                 _OFM_READ );
      if ( (pFile != NULL) && 
           (IFILE_Seek( pFile, _SEEK_START, pMe->mOBEX.bytesSent ) == SUCCESS) )
      {
        uint32 uHeadersSize = sizeof( AEEOBEXHdrType ) * MAX_OBEX_HDRS;

        if ( (SUCCESS == ERR_MALLOC( uHeadersSize, &pHeaders )) &&
             (SUCCESS == ERR_MALLOC( OBEX_DATA_BUFFER_SIZE, &pDataBuffer)) )
        {
          //Initializing headers for GET Response
          if ( pMe->mOBEX.bGetInProgress != TRUE )
          {
            pHeaders[ u8HdrCount ].id = AEE_OBEX_HDR_LENGTH;
            pHeaders[ u8HdrCount ].val.length = fileInfo.dwSize;
            u8HdrCount++;
            pMe->mOBEX.bGetInProgress = TRUE;
          }
          if ( fileInfo.dwSize > 
               ( pMe->mOBEX.bytesSent + OBEX_DATA_BUFFER_SIZE ) )
          {
            dwBytestoSend = OBEX_DATA_BUFFER_SIZE;
            IFILE_Read( pFile, pDataBuffer, dwBytestoSend );
            pHeaders[ u8HdrCount ].id = AEE_OBEX_HDR_BODY;
            pHeaders[ u8HdrCount ].val.body.len = dwBytestoSend;
            pHeaders[ u8HdrCount ].val.body.data = pDataBuffer;
            cmdStatus = AEE_OBEX_STAT_CONTINUE;
          }
          else
          {
            dwBytestoSend = fileInfo.dwSize - pMe->mOBEX.bytesSent;
            IFILE_Read( pFile, pDataBuffer, dwBytestoSend );
            pHeaders[ u8HdrCount ].id = AEE_OBEX_HDR_END_OF_BODY;
            pHeaders[ u8HdrCount ].val.endOfBody.len = dwBytestoSend;
            pHeaders[ u8HdrCount ].val.endOfBody.data = pDataBuffer;
            cmdStatus = AEE_OBEX_STAT_SUCCESS;
            pMe->mOBEX.bGetInProgress = FALSE;
          }
          u8HdrCount++;
          nResult = IOBEXServer_GetResponse( pMe->mOBEX.poServer, 
                                             pHeaders,
                                             u8HdrCount, cmdStatus );
          if ( nResult != SUCCESS )
          {
            MSG_FATAL( "OBEX SRV EVT CB - Get Response Failed Returning - %x", 
                       nResult, 0, 0 );
          }
          else
          {
            pMe->mOBEX.bytesSent += dwBytestoSend;
          }
        }
        else
        {
          MSG_LOW( "OBEX SRV EVT CB - Could not allocate memory", 0, 0, 0 );
        }
        FREEIF( pHeaders );
        FREEIF( pDataBuffer );
        IFILE_Release( pFile );
      }
      else
      {
        // Passing an empty header in get response, as the API 
        // cannnot take NULL
        MSG_FATAL( "OBEX SRV EVT CB - file access error", 0, 0, 0 );
        pMe->mOBEX.bGetInProgress = FALSE;
        nResult = IOBEXServer_GetResponse( pMe->mOBEX.poServer, 
                                           &emptyHeader, u8HdrCount,
                                           AEE_OBEX_STAT_GET_RESPONSE_ERROR );
        if ( nResult != SUCCESS )
        {
          MSG_FATAL( "OBEX SRV EVT CB - Get Resp Failed Returning - %x", 
                     nResult, 0, 0 );
        }
      }
      break;
    }
    case AEE_OBEX_EVT_SRV_PUT_REQ:
    {
      accReq = pN->u.accessReq;
      MSG_LOW( "OBEX SRV EVT CB - Put Req Recieved", 0, 0, 0 );
      if ( pMe->mOBEX.bPutInProgress != TRUE )
      {
        for ( u8Count = 0; u8Count < accReq.headerCount;
              u8Count++ )
        {
          if ( accReq.headerPtr[ u8Count ].id ==
               AEE_OBEX_HDR_NAME )
          {
            WSTRTOSTR( accReq.headerPtr[ u8Count ].val.name.str, 
                       szObjectName, 
                       accReq.headerPtr[ u8Count ].val.name.len );
            STRLCPY( pMe->mOBEX.szName, pMe->mOBEX.szCurrentFolder, 
                     sizeof( pMe->mOBEX.szName ) );
            STRLCAT( pMe->mOBEX.szName, DIRECTORY_STR, 
                     sizeof( pMe->mOBEX.szName ) );
            STRLCAT( pMe->mOBEX.szName, szObjectName, 
                     sizeof( pMe->mOBEX.szName ) );
            bNameHdrPresent = TRUE;
          }
          if ( accReq.headerPtr[ u8Count ].id ==
               AEE_OBEX_HDR_LENGTH )
          {
            pMe->mOBEX.fileSize = accReq.headerPtr[ u8Count ].val.length;
            bLenHdrPresent = TRUE;
          }
          if ( accReq.headerPtr[ u8Count ].id == AEE_OBEX_HDR_BODY ||  
               accReq.headerPtr[ u8Count ].id == AEE_OBEX_HDR_END_OF_BODY )
          {
            bBodyPresent = TRUE;
          }
        }
        if ( bNameHdrPresent == TRUE )
        {
          MSG_LOW( "OBEX SRV EVT CB - File name is present", 
                   pMe->mOBEX.szName, 0, 0 );
          if ( bBodyPresent != TRUE )
          {
            //Implementation of PUT-DELETE - No Body is present
            if ( IFILEMGR_Test( pMe->mOBEX.pIFileMgr, 
                                pMe->mOBEX.szName ) == SUCCESS )
            {
              nResult = IFILEMGR_GetInfo( pMe->mOBEX.pIFileMgr, 
                                          pMe->mOBEX.szName, &fileInfo );
              if ( nResult != SUCCESS )
              {
                MSG_FATAL( "OBEX SRV EVT CB - File Get info Failed", 0, 0, 0 );
                nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                            pHeaders, u8HdrCount, 
                            AEE_OBEX_STAT_PUT_RESPONSE_ERROR );
                if ( nResult != SUCCESS )
                {
                  MSG_FATAL( "OBEX SRV EVT CB - Put Resp failed. %x", 
                             nResult, 0, 0 );
                }
                return;
              }
              else
              {
                if ( fileInfo.attrib == _FA_DIR )
                {
                  MSG_FATAL( "OBEX SRV EVT CB - Object Could not be removed", 
                             nResult, 0, 0 );
                }
                else
                {
                  nResult = IFILEMGR_Remove( pMe->mOBEX.pIFileMgr, 
                                             pMe->mOBEX.szName );
                }
                if ( nResult != SUCCESS )
                {
                  MSG_FATAL( "OBEX SRV EVT CB - File not removed", 0, 0, 0 );
                  nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                              pHeaders, u8HdrCount, 
                              AEE_OBEX_STAT_PUT_RESPONSE_ERROR );
                  if ( nResult != SUCCESS )
                  {
                    MSG_FATAL( "OBEX SRV EVT CB - Put Resp failed. %x", 
                                nResult, 0, 0 );
                  }
                  return;
                }
                else
                {
                  MSG_LOW( "OBEX SRV EVT CB - File removed successfully", 
                           0, 0, 0 );
                  nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                              pHeaders, u8HdrCount, 
                              AEE_OBEX_STAT_SUCCESS );
                  if ( nResult != SUCCESS )
                  {
                    MSG_FATAL( "OBEX SRV EVT CB - Put Resp failed. %x", 
                               nResult, 0, 0 );
                  }
                  return;
                }
              }
            }
            else
            {
              MSG_MED( "OBEX SRV EVT CB - File Deos not exist", 0, 0, 0 );
              nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                          pHeaders,u8HdrCount, 
                          AEE_OBEX_STAT_PUT_RESPONSE_ERROR );
              if ( nResult != SUCCESS )
              {
                MSG_FATAL( "OBEX SRV EVT CB - Put Resp failed. %x", 
                           nResult, 0, 0 );
              }
              return;
            }
          }
          else if ( bLenHdrPresent != TRUE )
          {
            MSG_FATAL( "OBEX SRV EVT CB - Length Hdr not present", 0, 0, 0 );
            nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                        pHeaders, u8HdrCount,
                        AEE_OBEX_STAT_LENGTH_REQUIRED );
            if ( nResult != SUCCESS )
            {
              MSG_FATAL( "OBEX SRV EVT CB - Put Resp failed. Returning %x", 
                         nResult, 0, 0 );
            }
            return;
          }
          else if ( IFILEMGR_Test( pMe->mOBEX.pIFileMgr, pMe->mOBEX.szName ) 
                    == SUCCESS )
          {
            nResult = IFILEMGR_Remove( pMe->mOBEX.pIFileMgr, 
                                       pMe->mOBEX.szName );
            if ( nResult != SUCCESS )
            {
              MSG_FATAL( "OBEX SRV EVT CB - Error in File Remove", 0, 0, 0 );
              nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                          pHeaders, u8HdrCount,
                          AEE_OBEX_STAT_PUT_RESPONSE_ERROR );
              if ( nResult != SUCCESS )
              {
                MSG_FATAL( "OBEX SRV EVT CB - Put Resp failed. Returning %x", 
                           nResult, 0, 0 );
              }
              return;
            }
          }
          pFile = IFILEMGR_OpenFile( pMe->mOBEX.pIFileMgr, pMe->mOBEX.szName, 
                                     _OFM_CREATE );
          if ( pFile != NULL)
          {
            pMe->mOBEX.bPutInProgress = TRUE;
            pMe->mOBEX.bytesRcvd = 0;
          }
          else
          {
            MSG_FATAL( "OBEX SRV EVT CB - Error in File creation", 0, 0, 0 );
            nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                        pHeaders, u8HdrCount,
                        AEE_OBEX_STAT_PUT_RESPONSE_ERROR );
            if ( nResult != SUCCESS )
            {
              MSG_FATAL( "OBEX SRV EVT CB - Put Resp failed. Returning %x", 
                         nResult, 0, 0 );
            }
            return;
          }
        }
        else
        {
          MSG_FATAL( "OBEX SRV EVT CB - Name hdr not present", 0, 0, 0 );
          nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                      pHeaders, u8HdrCount,
                      AEE_OBEX_STAT_NAME_REQUIRED );
          if ( nResult != SUCCESS )
          {
            MSG_FATAL( "OBEX SRV EVT CB - Put Resp failed %x", nResult, 0, 0 );
          }
          return;
        }
      }
      else
      {
        pFile = IFILEMGR_OpenFile( pMe->mOBEX.pIFileMgr, pMe->mOBEX.szName, 
                                   _OFM_APPEND );
      }
      if ( pFile != NULL )
      {
        for ( u8Count = 0; u8Count < accReq.headerCount;
              u8Count++ )
        {
          if ( accReq.headerPtr[ u8Count ].id == AEE_OBEX_HDR_BODY )
          {
            pMe->mOBEX.bytesRcvd += accReq.headerPtr[ u8Count ].val.body.len;
            if ( pMe->mOBEX.bytesRcvd <= pMe->mOBEX.fileSize )
            {
              IFILE_Write( pFile, accReq.headerPtr[ u8Count ].val.body.data,
                           accReq.headerPtr[ u8Count ].val.body.len);
              nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                                                 pHeaders, u8HdrCount,
                                                 AEE_OBEX_STAT_CONTINUE );
              if ( nResult != SUCCESS )
              {
                 MSG_FATAL( "OBEX SRV EVT CB - Put Rsp failed. Returning %x",
                            nResult, 0, 0 );
              }
              else
              {
                MSG_LOW( "OBEX SRV EVT CB - Put Rsp sent to client", 0, 0, 0 );
              }
            }
            else
            {
              MSG_MED( "OBEX SRV EVT CB - Data recieved is more", 0, 0, 0 );
              pMe->mOBEX.bPutInProgress = FALSE;
              pMe->mOBEX.bytesRcvd = 0;
              nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                                                 pHeaders, u8HdrCount,
                                                 AEE_OBEX_STAT_ERROR );
              if ( nResult != SUCCESS )
              {
                MSG_FATAL( "OBEX SRV EVT CB - Put Rsp failed. %x", 
                           nResult, 0, 0 );
              }
              else
              {
                MSG_LOW( "OBEX SRV EVT CB - Put Rsp sent to client", 0, 0, 0 );
              }
            }
            break;
          }
          else if ( accReq.headerPtr[ u8Count ].id == 
                    AEE_OBEX_HDR_END_OF_BODY )
          {
            pMe->mOBEX.bytesRcvd += 
              accReq.headerPtr[u8Count].val.endOfBody.len;
            if ( pMe->mOBEX.bytesRcvd == pMe->mOBEX.fileSize )
            {
              IFILE_Write( pFile, 
                           accReq.headerPtr[ u8Count ].val.endOfBody.data,
                           accReq.headerPtr[ u8Count ].val.endOfBody.len);
              nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                                                 pHeaders, u8HdrCount,
                                                 AEE_OBEX_STAT_SUCCESS );
              if ( nResult != SUCCESS )
              {
                MSG_FATAL( "OBEX SRV EVT CB - Put Resp failed. %x", 
                           nResult, 0, 0 );
              }
              else
              {
                MSG_LOW( "OBEX SRV EVT CB - Put Rsp sent to client", 0, 0, 0 );
              }
            }
            else
            {
              MSG_MED( "OBEX SRV EVT CB - Data recieved is more than Length",
                       0, 0, 0 );
              nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, 
                                                 pHeaders, u8HdrCount,
                                                 AEE_OBEX_STAT_ERROR );
              if ( nResult != SUCCESS )
              {
                MSG_FATAL( "OBEX SRV EVT CB -Put Rsp fail=%x", nResult, 0, 0 );
              }
              else
              {
                MSG_LOW( "OBEX SRV EVT CB - Put Rsp sent to client", 0, 0, 0 );
              }
            }
            pMe->mOBEX.bPutInProgress = FALSE;
            pMe->mOBEX.bytesRcvd = 0;
            break;
          }
        }
        IFILE_Release( pFile );
      }
      else
      {
        MSG_FATAL( "OBEX SRV EVT CB - Could not Open File", 0, 0, 0 );
        nResult = IOBEXServer_PutResponse( pMe->mOBEX.poServer, pHeaders,
                    u8HdrCount, AEE_OBEX_STAT_PUT_RESPONSE_ERROR );
        if ( nResult != SUCCESS )
        {
          MSG_FATAL( "OBEX SRV EVT CB - Put Resp failed %x", nResult, 0, 0 );
        }
      }
      break;
    }
    case AEE_OBEX_EVT_SRV_SET_PATH_REQ:
    {
      setPathReq = pN->u.setPathReq;
	  
      STRLCPY( szObjectName, BTAPP_ROOT_DIR, sizeof( szObjectName ) );
      if ( setPathReq.upLevel )
      {
        // Setting the path to a step above the current path
        pObjName = STRRCHR( szObjectName, DIRECTORY_CHAR );
        if ( pObjName != NULL )
        {
          *pObjName = '\0';
		  
          if (  STRCMP( szObjectName, BTAPP_ROOT_DIR ) < 0 )
          {
            MSG_FATAL( "OBXSRVEVTCB - Path cannot be set above the ROOT fldr",
                       0, 0, 0 );
            nResult = IOBEXServer_ConfirmSetPath( pMe->mOBEX.poServer,
                                                  pHeaders, u8HdrCount,
                                                  AEE_OBEX_ERR_FAILED );
            if ( nResult != SUCCESS )
            {
              MSG_FATAL( "OBEX SRV EVT CB - Cnf Setpath failed. Returning %x",
                         nResult, 0, 0 );
            }
            return;
          }
        }
      }
      else
      {
        if ( WSTRLEN( setPathReq.folderName ) + STRLEN( DIRECTORY_STR ) + 
             STRLEN( szObjectName ) > AEEBT_MAX_FILE_NAME )
        {
          MSG_FATAL( "OBEX SRV EVT CB - Objt name size exceeds MAX characters",
                     0, 0, 0 );
          nResult = IOBEXServer_ConfirmSetPath( pMe->mOBEX.poServer, pHeaders, 
                                                u8HdrCount, 
                                                AEE_OBEX_ERR_FAILED );
          if ( nResult != SUCCESS )
          {
            MSG_FATAL( "OBEX SRV EVT CB - Cnf Setpath failed. Returning %x", 
                       nResult, 0, 0 );
          }
          return;
        }
        if ( WSTRLEN( setPathReq.folderName ) > 0)
        {
          STRLCAT( szObjectName, DIRECTORY_STR, sizeof( szObjectName ) );
          WSTRTOSTR( setPathReq.folderName, 
                     ( szObjectName + STRLEN( szObjectName ) ), 
                     ( sizeof( szObjectName ) - STRLEN( szObjectName ) ) );
        }
        else
        {        
          //Setting the path to ROOT Folder of the server
          STRLCPY( szObjectName, BTAPP_ROOT_DIR, sizeof( szObjectName ) );
        }
      }
      if ( IFILEMGR_Test( pMe->mOBEX.pIFileMgr, szObjectName ) != SUCCESS )
      {
        if ( setPathReq.bCreate == TRUE )
        {
          MSG_LOW( "OBEX SRV EVT CB - Creating Dir for SET PATH", 0, 0, 0 );
          nResult = IFILEMGR_MkDir( pMe->mOBEX.pIFileMgr, szObjectName );
          if ( nResult != SUCCESS)
          {
            MSG_FATAL( "OBEX SRV EVT CB - Folder Creation Failed", 0, 0, 0 );
            nResult = IOBEXServer_ConfirmSetPath( pMe->mOBEX.poServer,
                                                  pHeaders, u8HdrCount, 
                                                  AEE_OBEX_ERR_FAILED );
            if ( nResult != SUCCESS )
            {
              MSG_FATAL( "OBEX SRV EVT CB - Cnf Setpath failed. Returning %x",
                         nResult, 0, 0 );
            }
            return;
          }
        }
        else
        {
          MSG_FATAL( "OBEX SRV EVT CB - Directory not Found", 0, 0, 0 );
          nResult = IOBEXServer_ConfirmSetPath( pMe->mOBEX.poServer, pHeaders, 
                                                u8HdrCount, 
                                                AEE_OBEX_NO_SUCH_FOLDER );
          if ( nResult != SUCCESS )
          {
            MSG_FATAL( "Cnf Setpath failed. Returning %x", nResult, 0, 0 );
          }
          return;
        }
      }
      STRLCPY( pMe->mOBEX.szCurrentFolder, szObjectName, 
               sizeof( pMe->mOBEX.szCurrentFolder ) );
      nResult = IOBEXServer_ConfirmSetPath( pMe->mOBEX.poServer, pHeaders, 
                                            u8HdrCount, 
                                            AEE_OBEX_STAT_SUCCESS );
      if ( nResult != SUCCESS )
      {
        MSG_FATAL( "Cnf Setpath failed %x", nResult, 0, 0 );
      }
      break;
    }
    default:
    {
      MSG_MED( "OBEX SRV EVT CB - Unexpected Model event %x ", 
               pModelEvent.evCode, 0, 0 );
      break;
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_ProcessOBEXNotifications
DESCRIPTION
=============================================================================*/
boolean BTApp_ProcessOBEXNotifications(
  CBTApp*   pMe, 
  AEEEvent  eCode, 
  uint16    wParam, 
  uint32    dwParam
)
{
  AEENotify*  pN = ( AEENotify* ) dwParam;
  uint8       u8HdrCount = 0;
  boolean     bStatus = TRUE;
  OBEXNotificationDataType* pObexData;
  AEEOBEXDeviceAddressType  BDAddr;
  AEEOBEXRealm              realmInfo;
  int                       nResult = SUCCESS;
  boolean                   bAccept = FALSE;

  pObexData = ( OBEXNotificationDataType* )( pN->pData );
  MSG_LOW( "OBEXNotifications -Notifier: %d; Event: %d", 
           GET_NOTIFIER_MASK( pN->dwMask ), 
           GET_NOTIFIER_VAL (pN->dwMask ), 0 );

  if ( GET_NOTIFIER_MASK( pN->dwMask ) == NMASK_OBEX_GOEP )
  {
    switch( GET_NOTIFIER_VAL( pN->dwMask ) )
    {
      case AEE_OBEX_EVT_SRV_CON_REQ:
      {
        MSG_LOW( "OBEXNotifications - SVR CON REQ Recieved", 0, 0, 0 );
        if ( pMe->mOBEX.poServer == NULL )
        {
          MSG_FATAL( "OBEXNotifications - Server Obj is NULL", 0, 0, 0 );
          return bStatus;
        }
        if ( IOBEXServer_Initialize( pMe->mOBEX.poServer, 
                                     pObexData->obexHandle ) != SUCCESS )
        {
          MSG_FATAL( "OBEXNotifications -OBEX initialize Failed", 0, 0, 0 );
          IOBEXServer_AcceptConnection( pMe->mOBEX.poServer, FALSE, 
                                        NULL, u8HdrCount, 
                                        AEE_OBEX_STAT_ERROR );
          return bStatus;
        }
        pMe->mOBEX.sModelListener.pfnListener = ( PFNLISTENER )
          BTApp_OBEXSvrModelEventHandlerCb;
        pMe->mOBEX.sModelListener.pListenerData = pMe;
        if ( IMODEL_AddListener( pMe->mOBEX.poServer->m_pIModel, 
                                 &pMe->mOBEX.sModelListener ) != SUCCESS )
        {
          MSG_FATAL( "OBEXNotifications -Add Listener Failed", 0, 0, 0 );
          IOBEXServer_AcceptConnection( pMe->mOBEX.poServer, FALSE, 
                                        NULL, u8HdrCount, 
                                        AEE_OBEX_STAT_ERROR );
          return bStatus;
        }
        if( pObexData->ConnReq.bUnauthorized == TRUE )
        {
          pMe->mOBEX.bUidReqd = pMe->mOBEX.bDoAuthWithUid;
          if( pMe->mOBEX.bUidReqd == TRUE )
          {
            WSTRTOSTR( pObexData->ConnReq.wUserID, 
                       pMe->mOBEX.szUIDForVerification, 
                       sizeof( pMe->mOBEX.szUIDForVerification ) );
            DBGPRINTF_LOW ( "OBEXNotifications - UID For verification is %s",
                            pMe->mOBEX.szUIDForVerification );
            pMe->mOBEX.bVerifyUID = TRUE;
            BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX_USERID );
          }
          else
          {
            BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX_PASSWORD );
          }
        }
        else
        {
          if( ( pMe->mOBEX.bDoAuthenticate == TRUE ) || 
              ( pMe->mOBEX.bDoAuthWithUid == TRUE ) )
          {
            if( pMe->mOBEX.bAuthRequested == TRUE )
            {
              bAccept = TRUE;
              nResult = IOBEXServer_AcceptConnection( pMe->mOBEX.poServer,
                                                      bAccept, NULL, 
                                                      u8HdrCount,
                                                      AEE_OBEX_STAT_SUCCESS );
              if( nResult != SUCCESS )
              {
                MSG_FATAL( "OBEXNotifications - Accept Conn Failed", 0, 0, 0 );
              }
              else
              {
                MSG_LOW( "OBEXNotifications - Connection Accepted", 0, 0, 0 );
                pMe->mOBEX.bAuthRequested = FALSE;
              }
            }
            else
            {
              BDAddr.transport = pObexData->transportType;
              BDAddr.transport_conn.bt_conn.uuid = OBEX_UUID;
              BDAddr.transport_conn.bt_conn.security = AEEOBEX_BT_SEC_NONE;
              BDAddr.transport_conn.bt_conn.channel = pObexData->ConnReq.srvChNum;
              BDAddr.transport_conn.bt_conn.bd_addr_ptr = 
                ( AEEOBEX_BT_BD_AddrType* ) &(pObexData->bdAddr);
              nResult = IOBEXServer_Authenticate( pMe->mOBEX.poServer,&BDAddr,
                                                  &realmInfo,
                                                  pMe->mOBEX.bDoAuthWithUid,
                                                  pMe->mOBEX.bFullAccess );
              if( nResult != SUCCESS )
              {
                MSG_FATAL( "OBEXNotifications - SVR Auth failed. Returning %x",
                           nResult, 0, 0 );
              }
              else
              {
                pMe->mOBEX.bAuthRequested = TRUE;
                MSG_LOW( "OBEXNotifications - Initiated Auth of Connection",
                         0, 0, 0 );
              }
            }
          }
          else
          {
            bAccept = TRUE;
            nResult = IOBEXServer_AcceptConnection( pMe->mOBEX.poServer, 
                                                    bAccept, NULL, u8HdrCount, 
                                                    AEE_OBEX_STAT_SUCCESS );
            if( nResult != SUCCESS )
            {
              MSG_FATAL( "OBEXNotifications - Accept Conn Failed", 0, 0, 0 );
            }
            else
            {
              MSG_LOW( "OBEXNotifications - Connection Accepted", 0, 0, 0 );
              pMe->mOBEX.bAuthRequested = FALSE;
            }
          }
        }
        return bStatus;
      }
      case AEE_OBEX_EVT_SRV_AUTH_REQ:
      {
        MSG_LOW( "OBEXNotifications - SVR AUTH REQ Recieved", 0, 0, 0 );
        if ( pMe->mOBEX.poServer == NULL )
        {
          MSG_FATAL( "OBEXNotifications - Server Obj is NULL", 0, 0, 0 );
          return bStatus;
        }
        if( ( pMe->mOBEX.bDoAuthenticate == TRUE ) ||
            ( pMe->mOBEX.bDoAuthWithUid == TRUE ) )
        {
          BDAddr.transport = AEE_OBEX_BLUETOOTH_TRANSPORT;
          BDAddr.transport_conn.bt_conn.uuid = OBEX_UUID;
          BDAddr.transport_conn.bt_conn.security = AEEOBEX_BT_SEC_NONE;
          BDAddr.transport_conn.bt_conn.channel = pMe->mOBEX.u8SCN;
          BDAddr.transport_conn.bt_conn.bd_addr_ptr = 
                 ( AEEOBEX_BT_BD_AddrType* ) &( pObexData->bdAddr );
          nResult = IOBEXServer_Authenticate( pMe->mOBEX.poServer,&BDAddr,
                                              &realmInfo, 
                                              pMe->mOBEX.bDoAuthWithUid,
                                              pMe->mOBEX.bFullAccess );
          if( nResult != SUCCESS )
          {
            MSG_FATAL( "OBEXNotifications - Server Authenticate failed - %x",
                       nResult, 0, 0 );
          }
          else
          {
            pMe->mOBEX.bAuthRequested = TRUE;
            MSG_LOW( "OBEXNotifications - Initiated Auth of Connection", 
                     0, 0, 0 );
          }
        }
        else
        {
          pMe->mOBEX.bUidReqd = pObexData->AuthReq.bUidReqd;
          if( pMe->mOBEX.bUidReqd == TRUE )
          {
            BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX_USERID );
          }
          else
          {
            BTApp_BuildMenu( pMe, BT_APP_MENU_OBEX_PASSWORD );
          }
        }
        break;
      }
      default:
      {
        MSG_LOW( "OBEXNotifications - Unexpected EVT Notifier %x in EVT hdlr",
                 GET_NOTIFIER_VAL( pN->dwMask ), 0, 0 );
        return bStatus;
      }
    }
  }
  return FALSE;
}

/* ==========================================================================
FUNCTION BTApp_OBEXPutPacketCb
DESCRIPTION
=============================================================================*/
static void BTApp_OBEXPutPacketCb( CBTApp* pMe )
{
  AECHAR               wObjName[ AEEBT_MAX_FILE_NAME+1 ];
  IFile*               pFile = NULL;
  uint32               dwBytestoSend;
  FileInfo             fileInfo;
  int                  nResult = SUCCESS;
  AEEOBEXCmdStatusType cmdStatus;
  uint8                u8HdrCount = 0;
  AEEOBEXHdrType*      pHeaders = NULL;
  byte*                pDataBuffer = NULL;
  uint32               u32BufferSize = OBEX_DATA_BUFFER_SIZE;
  uint32               uHeadersSize = sizeof( AEEOBEXHdrType ) * MAX_OBEX_HDRS;

  STRTOWSTR( OBEX_PUTFILE_NAME, wObjName, sizeof(wObjName) );
  pMe->mOBEX.sModelListener.pfnListener = 
             ( PFNLISTENER ) BTApp_OBEXPutModelEventHandlerCb;
  pMe->mOBEX.sModelListener.pListenerData = pMe;
  nResult = IMODEL_AddListener( pMe->mOBEX.poClient->m_pIModel, 
                                &pMe->mOBEX.sModelListener );
  if ( nResult != SUCCESS )
  {
    MSG_FATAL( "OBXPutPktCb - Error in AddListener", 0, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wObjName, 3 );
    return;
  }
  nResult = IFILEMGR_GetInfo( pMe->mOBEX.pIFileMgr, pMe->mOBEX.szName, 
                              &fileInfo );
  if ( nResult!= SUCCESS )
  {
    MSG_FATAL( "OBXPutPktCb - File Get Info Failed", 0, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wObjName, 3 );
    return;
  }
  if ( (SUCCESS == ERR_MALLOC( uHeadersSize, &pHeaders )) &&
       (SUCCESS == ERR_MALLOC( u32BufferSize, &pDataBuffer)) )
  {
    pFile = IFILEMGR_OpenFile( pMe->mOBEX.pIFileMgr, pMe->mOBEX.szName, 
                               _OFM_READ );
    if( pFile != NULL )
    {
      if( IFILE_Seek( pFile, _SEEK_START, 
                      pMe->mOBEX.bytesSent ) == SUCCESS )
      {
        if( pMe->mOBEX.bytesSent == 0 )
        {
          pHeaders[ u8HdrCount ].id = AEE_OBEX_HDR_NAME;
          pHeaders[ u8HdrCount ].val.name.len = WSTRLEN( wObjName ) + 1;
          pHeaders[ u8HdrCount ].val.name.str = wObjName;
          u8HdrCount++;
          pHeaders[ u8HdrCount ].id = AEE_OBEX_HDR_LENGTH;
          pHeaders[ u8HdrCount ].val.length = fileInfo.dwSize;
          u8HdrCount++;
          pMe->mOBEX.bPutInProgress = TRUE;
        }
        if ( fileInfo.dwSize > ( pMe->mOBEX.bytesSent+u32BufferSize ) )
        {
          dwBytestoSend = u32BufferSize;
          IFILE_Read( pFile, pDataBuffer, dwBytestoSend );
          pHeaders[ u8HdrCount ].id = AEE_OBEX_HDR_BODY;
          pHeaders[ u8HdrCount ].val.body.len = dwBytestoSend;
          pHeaders[ u8HdrCount ].val.body.data = pDataBuffer;
          cmdStatus = AEE_OBEX_STAT_CONTINUE;
        }
        else 
        {
          dwBytestoSend = fileInfo.dwSize - pMe->mOBEX.bytesSent;
          IFILE_Read( pFile, pDataBuffer, dwBytestoSend );
          pHeaders[ u8HdrCount ].id = AEE_OBEX_HDR_END_OF_BODY;
          pHeaders[ u8HdrCount ].val.endOfBody.len = dwBytestoSend;
          pHeaders[ u8HdrCount ].val.endOfBody.data = pDataBuffer;
          cmdStatus = AEE_OBEX_STAT_SUCCESS;
        }
        u8HdrCount++;
        nResult = IOBEXClient_Put( pMe->mOBEX.poClient, pHeaders, 
                                   u8HdrCount,  cmdStatus );
        IFILE_Release(pFile);
        if ( nResult != SUCCESS )
        {
          MSG_FATAL( "OBXPutPktCb - PUT API Failed; Returning %x",
                     nResult, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wObjName, 3 );
        }
        else
        {
          pMe->mOBEX.bytesSent += dwBytestoSend;
        }
      }
      else
      {
        MSG_FATAL( "OBXPutPktCb - Error in File seek", 0, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wObjName, 3 );
      }
    }
    else
    {
      MSG_FATAL( "OBXPutPktCb - File Open Failed", 0, 0, 0 );
      BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wObjName, 3 );
    }
  }
  else
  {
    MSG_FATAL( "OBXPutPktCb - Memory Allcoation Failed", 0, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wObjName, 3 );
  }
  FREEIF( pHeaders );
  FREEIF( pDataBuffer );
}

/* ==========================================================================
FUNCTION BTApp_OBEXPut
DESCRIPTION
=============================================================================*/
void BTApp_OBEXPut( CBTApp* pMe )
{
  AECHAR         wObjName[ AEEBT_MAX_FILE_NAME+1 ];
  char           szObjectNm[ AEEBT_MAX_FILE_NAME+1 ];
  FileInfo       fileInfo;
  int            nResult = SUCCESS;
  
  STRLCPY( szObjectNm, BTAPP_ROOT_DIR, sizeof( szObjectNm ) );
  STRLCAT( szObjectNm, DIRECTORY_STR, sizeof( szObjectNm ) );
  STRLCAT( szObjectNm, OBEX_PUTFILE_NAME, sizeof( szObjectNm ) );

  nResult = IFILEMGR_Test( pMe->mOBEX.pIFileMgr, szObjectNm );
  if ( nResult == SUCCESS )
  {
    nResult = IFILEMGR_GetInfo( pMe->mOBEX.pIFileMgr, szObjectNm, &fileInfo );
    if ( nResult == SUCCESS )
    {
      if ( fileInfo.attrib == _FA_DIR )
      {
        MSG_LOW( "OBEXPut - Object should be a File", 0, 0, 0 );
        return;
      }
      else
      {
        STRLCPY( pMe->mOBEX.szName, szObjectNm, sizeof( pMe->mOBEX.szName ) );
        pMe->mOBEX.bytesSent = 0;
        CALLBACK_Init( &pMe->mOBEX.m_cb, BTApp_OBEXPutPacketCb, pMe );
        ISHELL_Resume( pMe->a.m_pIShell, &pMe->mOBEX.m_cb );
        BTApp_ShowBusyIcon( pMe );
      }
    }
    else
    {
      MSG_LOW( "OBXPut - MSG_FATAL in File Get info", 0, 0, 0 );
    }
  }
  else
  {
    STRTOWSTR( OBEX_PUTFILE_NAME, wObjName, sizeof(wObjName) );
    BTApp_ShowMessage( pMe, IDS_MSG_OBJ_NOT_FOUND, wObjName, 3 );
    return;
  }
}

/* ==========================================================================
FUNCTION BTApp_OBEXGet
DESCRIPTION
=============================================================================*/
void BTApp_OBEXGet( CBTApp* pMe )
{
  char           szObjectNm[ AEEBT_MAX_FILE_NAME+1 ];
  AECHAR         wObjName[ AEEBT_MAX_FILE_NAME+1 ];
  FileInfo       fileInfo;
  int            nResult = SUCCESS;
  AEEOBEXHdrType header;
  
  STRLCPY( szObjectNm, BTAPP_ROOT_DIR, sizeof( szObjectNm ) );
  STRLCAT( szObjectNm, DIRECTORY_STR, sizeof( szObjectNm ) );
  STRLCAT( szObjectNm, OBEX_GETFILE_NAME, sizeof( szObjectNm ) );

  STRTOWSTR( OBEX_GETFILE_NAME, wObjName, sizeof(wObjName) );
  if ( IFILEMGR_Test( pMe->mOBEX.pIFileMgr, szObjectNm ) == SUCCESS )
  {
    nResult = IFILEMGR_GetInfo( pMe->mOBEX.pIFileMgr, szObjectNm, &fileInfo );
    if ( nResult != SUCCESS )
    {
      MSG_MED( "OBXGet -IFile GetInfo Failed Returning %x ", nResult, 0, 0 );
    }
    else
    {
      if ( fileInfo.attrib == _FA_DIR )
      {
        MSG_MED("OBXGet - The Object should be a file", 0, 0, 0 );
      }
      else
      {
        nResult = IFILEMGR_Remove( pMe->mOBEX.pIFileMgr, szObjectNm );
      }
      if ( nResult != SUCCESS )
      {
        MSG_MED( "OBEGet - Failed to remove the existing file - %x ", 
                 nResult, 0, 0 );
        return;
      }
      else
      {
        MSG_LOW( "OBEGet - Removed the existing file", 0, 0, 0 );
      }
    }
  }
  pMe->mOBEX.sModelListener.pfnListener = 
             ( PFNLISTENER ) BTApp_OBEXGetModelEventHandlerCb;
  pMe->mOBEX.sModelListener.pListenerData = pMe;
  nResult = IMODEL_AddListener( pMe->mOBEX.poClient->m_pIModel, 
                                &pMe->mOBEX.sModelListener );
  if ( nResult != SUCCESS )
  {
    MSG_LOW( "OBEGet - Could not Add Listener CB", 0, 0, 0 );
    return;
  }

  STRLCPY( pMe->mOBEX.szName, szObjectNm, sizeof( pMe->mOBEX.szName ) );
  pMe->mOBEX.bGetInProgress = FALSE;
  pMe->mOBEX.bytesRcvd = 0;

  header.id = AEE_OBEX_HDR_NAME;
  header.val.name.len = WSTRLEN( wObjName ) + 1;
  header.val.name.str = wObjName;
  nResult = IOBEXClient_Get( pMe->mOBEX.poClient, &header, 1, TRUE );
  if ( nResult != SUCCESS )
  {
    MSG_LOW( "OBEGet - Get failed. Returning %x", nResult, 0, 0 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe ); // wait for command done
  }
}

/* ==========================================================================
FUNCTION BTApp_ConnectObexServer
DESCRIPTION
=============================================================================*/
static int BTApp_ConnectObexServer( CBTApp* pMe )
{
  AEEOBEXDeviceAddressType BDAddr;
  AEEOBEXRealm             realmInfo;
  AEEOBEXHdrType           pHeaders[1];
  int                      nResult = SUCCESS;
  uint8                    u8HeaderCount = 0;

  BDAddr.transport = AEE_OBEX_BLUETOOTH_TRANSPORT;
  BDAddr.transport_conn.bt_conn.uuid = OBEX_UUID;
  //BDAddr.transport_conn.bt_conn.security = AEEOBEX_BT_SEC_AUTHENTICATE;
  BDAddr.transport_conn.bt_conn.security = AEEOBEX_BT_SEC_NONE;
  BDAddr.transport_conn.bt_conn.channel = pMe->mOBEX.u8SCN;

  BDAddr.transport_conn.bt_conn.bd_addr_ptr = ( AEEOBEX_BT_BD_AddrType* )
                                                &(pMe->mOBEX.mRemoteBDAddr);
  pMe->mOBEX.sModelListener.pfnListener = ( PFNLISTENER )
                                            BTApp_OBEXConnModelEventHandlerCb;
  pMe->mOBEX.sModelListener.pListenerData = pMe;
  nResult = IMODEL_AddListener( pMe->mOBEX.poClient->m_pIModel, 
                                &pMe->mOBEX.sModelListener );
  if ( nResult != SUCCESS )
  {
    MSG_LOW( "OBXHndlSelection - Couldn't Add model Listener ", 0, 0, 0 );
    return nResult;
  }
  if ( ( pMe->mOBEX.bDoAuthenticate == TRUE ) ||
       ( pMe->mOBEX.bDoAuthWithUid == TRUE ) )
  {
    nResult = IOBEXClient_Authenticate( pMe->mOBEX.poClient, &BDAddr, 
                                        &realmInfo, pMe->mOBEX.bDoAuthWithUid, 
                                        pMe->mOBEX.bFullAccess );
    if ( nResult != SUCCESS )
    {
      MSG_FATAL( "Client Authenticate failed - %x", nResult, 0, 0 );
      return nResult;
    }
  }
  nResult = IOBEXClient_Connect( pMe->mOBEX.poClient, &BDAddr, 
                                 pHeaders, u8HeaderCount );
  if ( nResult != SUCCESS )
  {
    MSG_FATAL( "Connection Request Failed - %x", nResult, 0, 0 );
  }
  return( nResult );
}

/* ==========================================================================
FUNCTION:    BTApp_OBEXConnect
DESCRIPTION: 
============================================================================*/
void BTApp_OBEXConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr )
{
  pMe->mOBEX.mRemoteBDAddr = (*pBDAddr);
  IBTEXTSD_BrowseService( pMe->mSD.po, pBDAddr,
                          pMe->mSD.browseRec, 
                          AEEBT_MAX_NUM_OF_SRV_REC );
  BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
}

/* ==========================================================================
FUNCTION BTApp_OBEXRegister
DESCRIPTION
=============================================================================*/
int BTApp_OBEXRegister( CBTApp* pMe )
{
  AEEOBEXByteSeqType          target;
  AEEOBEXRegistrationInfoType reg;

  target.len = 0;
  target.data = NULL;
  reg.targetPtr = &target;
  reg.auth = AEE_OBEX_AUTH_NONE;
  reg.bReadOnly = FALSE;
  reg.authRequired = TRUE;
  reg.transportConfig.transports = AEE_OBEX_BLUETOOTH_TRANSPORT;
  reg.transportConfig.bt_config.uuid = OBEX_UUID;
  reg.transportConfig.bt_config.security = AEEOBEX_BT_SEC_AUTHENTICATE;

  return IOBEXServer_Register( pMe->mOBEX.poServer, &reg, &pMe->mOBEX.u8SCN );
}

/* ==========================================================================
FUNCTION BTApp_OBEXBuildSettingsMenu
DESCRIPTION
=============================================================================*/
void BTApp_OBEXBuildSettingsMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );

  BTApp_InitAddItem( &ai );

  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SETTINGS, NULL );

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, 
    &ai, IDS_AUTHENTICATE, 
    pMe->mOBEX.bDoAuthenticate ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, 
    &ai, IDS_OBEX_AUTHENTICATE_WITH_USERID, 
    pMe->mOBEX.bDoAuthWithUid ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_OBEX_SETTINGS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_OBEXBuildClientMenu
DESCRIPTION
=============================================================================*/
void BTApp_OBEXBuildClientMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  if ( pMe->mOBEX.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->a.m_pIShell, 
                          AEE_APPSBTAPP_RES_FILE, 
                          IDS_OBEX_CLIENT, 
                          pMe->pText2, 
                          SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
    IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
  }
  else
  {
    IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_OBEX_CLIENT, NULL );
  }

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mOBEX.bConnected == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PUT, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_GET, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OBEX_SETPATH, 0 );
  }
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_OBEX_CLIENT );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
}

/* ==========================================================================
FUNCTION BTApp_OBEXBuildServerMenu
DESCRIPTION
=============================================================================*/
void BTApp_OBEXBuildServerMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  if ( pMe->mOBEX.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->a.m_pIShell, 
                          AEE_APPSBTAPP_RES_FILE, 
                          IDS_OBEX_SERVER, 
                          pMe->pText2, 
                          SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
    IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
  }
  else
  {
    IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_OBEX_SERVER, NULL );
  }

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mOBEX.bRegistered == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REGISTER, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEREGISTER, 0 );
    if ( pMe->mOBEX.bConnected != FALSE )
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OBEX_FORCE_DISCONNECT,
                         0 );
    }
  }

  // Activate menu
  PUSH_MENU( BT_APP_MENU_OBEX_SERVER );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
}

/* ==========================================================================
FUNCTION BTApp_OBEXBuildMainMenu
DESCRIPTION
=============================================================================*/
void BTApp_OBEXBuildMainMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  


  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_OBEX_TESTS, NULL );  
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->a.m_pIShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_OPP,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mOBEX.bRegistered == TRUE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OBEX_SERVER, 0 );
  }
  else if ( pMe->mOBEX.bConnected == TRUE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OBEX_CLIENT, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OBEX_SERVER, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OBEX_CLIENT, 0 );
  }
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SETTINGS, 0 );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_OBEX_TESTS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_OBEXCleanup
DESCRIPTION
=============================================================================*/
void BTApp_OBEXCleanup( CBTApp* pMe )
{
  if ( pMe->mOBEX.poServer != NULL )
  {
    IOBEXServer_Release( pMe->mOBEX.poServer );
    pMe->mOBEX.poServer = NULL;
  }
  if ( pMe->mOBEX.poClient != NULL )
  {
    IOBEXClient_Release( pMe->mOBEX.poClient );
    pMe->mOBEX.poClient = NULL;
  }
  if ( pMe->mOBEX.pIFileMgr != NULL )
  {
    IFILEMGR_Release( pMe->mOBEX.pIFileMgr );
    pMe->mOBEX.pIFileMgr = NULL;
  }
  // unregister for OBEX notification
  ISHELL_RegisterNotify( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_APP, 
                         AEECLSID_OBEX_NOTIFIER, 0 );
}

/* ==========================================================================
FUNCTION:    BTApp_OBEXInit
DESCRIPTION: Registers OBEX notifications and creates an object of file manager
============================================================================ */
boolean BTApp_OBEXInit( CBTApp* pMe )
{
  static boolean init_done = FALSE;
  uint32 uNMask = NMASK_OBEX_GOEP | ( ( (uint32)(NOTIFIER_VAL_ANY) ) << 16 );

  if ( init_done == FALSE )
  {
    if ( (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, 
            (void **)&pMe->mOBEX.pIFileMgr ) == SUCCESS) && 
            ( ISHELL_RegisterNotify( pMe->a.m_pIShell,  AEECLSID_BLUETOOTH_APP,
            AEECLSID_OBEX_NOTIFIER, uNMask) == SUCCESS ) )
    {	  
      STRLCPY( pMe->mOBEX.szCurrentFolder, BTAPP_ROOT_DIR, 
               sizeof( pMe->mOBEX.szCurrentFolder ) );
      pMe->mOBEX.bPutInProgress = FALSE;
      pMe->mOBEX.bGetInProgress = FALSE;
      pMe->mOBEX.bAuthRequested = FALSE;
      MEMSET( pMe->mOBEX.szName, 0, sizeof(pMe->mOBEX.szName) );
      init_done = TRUE;
	  
      STRLCPY( pMe->mOBEX.szCurrentFolder, BTAPP_ROOT_DIR, 
               sizeof( pMe->mOBEX.szCurrentFolder ) );
    }
  }
  return init_done;
}

/* ==========================================================================
FUNCTION BTApp_OBEXBuildMenu
DESCRIPTION
=============================================================================*/
boolean BTApp_OBEXBuildMenu( CBTApp* pMe, BTAppMenuType menu )
{
  boolean built = TRUE;

  
  switch ( menu)
  {
    case BT_APP_MENU_OBEX_TESTS:
    {
      if ( BTApp_OBEXInit( pMe ) != FALSE )
      {
        BTApp_OBEXBuildMainMenu( pMe );
      }
      else
      {
        MSG_FATAL( "OBEXBuildMenu - failed to create Obex objs", 0, 0, 0 );
        BTApp_OBEXCleanup( pMe );
        built = FALSE;
      }
      break;
    }
    case BT_APP_MENU_OBEX_SERVER:
      BTApp_OBEXBuildServerMenu( pMe ); 
      break;
    case BT_APP_MENU_OBEX_CLIENT:
      BTApp_OBEXBuildClientMenu( pMe );
      break;
    case BT_APP_MENU_OBEX_SETTINGS:
      BTApp_OBEXBuildSettingsMenu( pMe );
      break;
    default:
      built = FALSE;
      break;
  }
  return built;
}

/* ==========================================================================
FUNCTION BTApp_OBEXDoServerDeregister
DESCRIPTION
=============================================================================*/

void BTApp_OBEXDoServerDeregister( CBTApp* pMe )
{
  uint16 nResult;

  nResult = IOBEXServer_Deregister( pMe->mOBEX.poServer );
  if ( nResult != SUCCESS )
  {
    MSG_FATAL( "Deregister Failed returning - %x", nResult, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
  }
  else
  {
    pMe->mOBEX.bConnected = FALSE;
    pMe->mOBEX.bRegistered = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
    BTApp_CheckToClearDiscoverable( pMe );
    if ( pMe->mOBEX.poServer != NULL )
    {
       IOBEXServer_Release( pMe->mOBEX.poServer );
       pMe->mOBEX.poServer = NULL;
    }
    BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_DONE, NULL, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_OBEXHandleSelection
DESCRIPTION
=============================================================================*/
static boolean BTApp_OBEXHandleSelection( CBTApp* pMe, uint16 sel )
{
  boolean key_handled = TRUE;
  uint16  selection;
  AEEOBEXHdrType*  pHeaders = NULL;
  uint8            u8HdrCount = 0;
  int nResult = SUCCESS;

  switch ( sel )
  {
    case IDS_OBEX_SERVER:
    {
      BTApp_OBEXBuildMenu( pMe, BT_APP_MENU_OBEX_SERVER );
      break;
    }
    case IDS_OBEX_CLIENT:
    {
      BTApp_OBEXBuildMenu( pMe, BT_APP_MENU_OBEX_CLIENT );
      break;
    }
    case IDS_SETTINGS:
    {
      BTApp_OBEXBuildMenu( pMe, BT_APP_MENU_OBEX_SETTINGS );
      break;
    }
    case IDS_AUTHENTICATE:
    {
      pMe->bConfigChanged = TRUE;
      pMe->mOBEX.bDoAuthenticate = pMe->mOBEX.bDoAuthenticate ? FALSE : TRUE;
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );

      BTApp_UpdateMenuItemImage( 
        pMe->a.m_pIDisplay, 
        pMe->m_pIMenu, 
        selection,
        pMe->mOBEX.bDoAuthenticate ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
      break;
    }
    case IDS_OBEX_AUTHENTICATE_WITH_USERID:
    {
      pMe->bConfigChanged = TRUE;
      pMe->mOBEX.bDoAuthWithUid = pMe->mOBEX.bDoAuthWithUid ? FALSE : TRUE;
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );

      BTApp_UpdateMenuItemImage( 
        pMe->a.m_pIDisplay, 
        pMe->m_pIMenu, 
        selection,
        pMe->mOBEX.bDoAuthWithUid ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
      break;
    }
    case IDS_REGISTER:
    {
      //Code for registration
      if ( pMe->mOBEX.poServer == NULL )
      {
        nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_OBEXSERVER,
                                         (void**)&pMe->mOBEX.poServer );
        if ( nResult != SUCCESS )
        {
          MSG_FATAL( "OBEXHandleSelection - Cannot create Server obj",
                     0, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
          break;
        }
      }
      else
      {
        MSG_FATAL( "OBEXHandleSelection - Server Object already created",
                    0, 0, 0 );
      }
      BTApp_SetBondable( pMe );
      if ( BTApp_OBEXRegister( pMe ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe );
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
      }
      else
      {
        pMe->mOBEX.bRegistered = TRUE;
        if ( pMe->mSD.bDiscoverable == FALSE )
        {
          IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
          pMe->mSD.bDiscoverable = TRUE;
        }
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_DONE, NULL, 3 );
      }
      break;
    }
    case IDS_DEREGISTER:
    {
      if ( pMe->mOBEX.bConnected )
      {
        if ( IOBEXServer_ForceDisconnect( pMe->mOBEX.poServer ) != SUCCESS )
        {
          BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
        }
        else
        {
          pMe->mOBEX.bPendingDisc = TRUE;
          BTApp_ShowBusyIcon( pMe );
        }
      }
      else
      {
        BTApp_OBEXDoServerDeregister( pMe );
      }
      break;
    }
    case IDS_CONNECT:
    {
      key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_ALL );
      break;
    }
    case IDS_DISCONNECT:
    {
      pMe->mOBEX.sModelListener.pfnListener = 
                 ( PFNLISTENER ) BTApp_OBEXDisConnModelEventHandlerCb;
      pMe->mOBEX.sModelListener.pListenerData = pMe;
      nResult = IMODEL_AddListener( pMe->mOBEX.poClient->m_pIModel, 
                                    &pMe->mOBEX.sModelListener );
      if ( nResult != SUCCESS )
      {
        MSG_LOW( "OBXHndlSelection - Couldn't Add model Listener ", 0, 0, 0 );
      }
      nResult = IOBEXClient_Disconnect( pMe->mOBEX.poClient, pHeaders, 
                                        u8HdrCount );
      if ( nResult != SUCCESS )
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
        BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case IDS_GET:
    {
      BTApp_OBEXGet( pMe );
      break;
    }
    case IDS_PUT:
    {
      BTApp_OBEXPut( pMe );
      break;
    }
    case IDS_OBEX_SETPATH:
    {
      boolean  bDontCreate = FALSE;
      boolean  bUpDown = FALSE;
      uint8    n8HdrCount = 0;
      AECHAR   wObjName[ AEEBT_MAX_FILE_NAME+1 ];

      STRTOWSTR( OBEX_FOLDER_NAME, wObjName, sizeof(wObjName) );
      pMe->mOBEX.sModelListener.pfnListener = 
                 ( PFNLISTENER ) BTApp_OBEXSetPathModelEventHandlerCb;
      pMe->mOBEX.sModelListener.pListenerData = pMe;
      IMODEL_AddListener( pMe->mOBEX.poClient->m_pIModel, 
                          &pMe->mOBEX.sModelListener );
      nResult = IOBEXClient_SetPath( pMe->mOBEX.poClient, wObjName, bUpDown, 
                                     bDontCreate, NULL, n8HdrCount );
      if ( nResult != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_OBEX_MSG_SETPATH_FAILED, NULL, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case IDS_OBEX_FORCE_DISCONNECT:
    {
      if ( IOBEXServer_ForceDisconnect( pMe->mOBEX.poServer ) != SUCCESS )
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
        BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    default:
    {
      key_handled = FALSE;
      break;
    }
  }
  return key_handled;
}

/* ==========================================================================
FUNCTION BTApp_HandleOBEXMainMenu
DESCRIPTION
=============================================================================*/
static boolean BTApp_HandleOBEXMainMenu( CBTApp* pMe, uint16 key )
{
  uint16 selection;
  boolean ev_processed = TRUE;

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_OBEXHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key)
    {
      case AVK_INFO:		//Add By zzg 2010_11_16
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_OBEXHandleSelection( pMe, selection );
        break;
      }
      case AVK_CLR:
      {
        BTApp_HandleClearKey( pMe );
        break;
      }
      default:
      {
        ev_processed = FALSE;
        break;
      }
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleOBEXServerMenu
DESCRIPTION
=============================================================================*/
static boolean BTApp_HandleOBEXServerMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlOBEXServer - k=0x%x", key, 0, 0 );

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_OBEXHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_INFO:		//Add By zzg 2010_11_16
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_OBEXHandleSelection( pMe, selection );
        break;
      }
      case AVK_CLR:
      {
        BTApp_HandleClearKey( pMe );
        break;
      }
      default:
        ev_processed = FALSE;
        break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleOBEXClientMenu
DESCRIPTION
============================================================================*/
static boolean BTApp_HandleOBEXClientMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;
  int nResult = SUCCESS;

  MSG_MED( "HndlOBEXClient - k=0x%x", key, 0, 0 );

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_OBEXHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_INFO:		//Add By zzg 2010_11_16
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_OBEXHandleSelection( pMe, selection );
        break;
      }
      case AVK_CLR:
      {
        MSG_FATAL( "HndlOBEXClient - CLR Key hnldr invoked", 0, 0, 0 );
        if( ( pMe->mOBEX.bPutInProgress == TRUE ) || 
            ( pMe->mOBEX.bGetInProgress == TRUE ) )
        {
           nResult = IOBEXClient_AbortOperation( pMe->mOBEX.poClient );
           if( nResult != SUCCESS )
           {
             MSG_FATAL( "HndlOBEXClient - Abort Failed", 0, 0, 0 );
           }
        }
        else if ( pMe->mOBEX.bEndingTest == FALSE)
        {
          BTApp_HandleClearKey( pMe );
        }
        break;
      }
      default:
        ev_processed = FALSE;
        break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandleOBEXSettingsMenu
DESCRIPTION
=============================================================================*/
static boolean BTApp_HandleOBEXSettingsMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlOBEXSettings - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_INFO:		//Add By zzg 2010_11_16
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      ev_processed = BTApp_OBEXHandleSelection( pMe, selection );
      break;
    }
    case AVK_CLR:
    {
      BTApp_HandleClearKey( pMe );
      break;
    }
    default:
      ev_processed = FALSE;
      break;
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_OBEXHandleMenus
DESCRIPTION
=============================================================================*/
boolean BTApp_OBEXHandleMenus( CBTApp *pMe, uint16 key, BTAppMenuType menu )
{
  boolean handled = TRUE;
  switch ( menu )
  {
    case BT_APP_MENU_OBEX_TESTS:
      handled = BTApp_HandleOBEXMainMenu( pMe, key );
      break;
    case BT_APP_MENU_OBEX_SERVER:
      handled = BTApp_HandleOBEXServerMenu( pMe, key );
      break;
    case BT_APP_MENU_OBEX_CLIENT:
      handled = BTApp_HandleOBEXClientMenu( pMe, key );
      break;
    case BT_APP_MENU_OBEX_SETTINGS:
      handled = BTApp_HandleOBEXSettingsMenu( pMe, key );
      break;
    default:
      MSG_FATAL("OBXHndlMenus - unexpected menu type %x", menu, 0, 0 );
      handled = FALSE;
      break;
  }
  return handled;
}

/* ==========================================================================
FUNCTION BTApp_OBEXHandleUserEvents
DESCRIPTION
=============================================================================*/
void BTApp_OBEXHandleUserEvents( CBTApp* pMe, uint32 dwParam )
{
  uint8     uNoRecs = 0;
  boolean   bFound = FALSE;
  int       nResult = SUCCESS;
  uint8     u8HdrCount = 0;
  char      szPassWord[ AEEBT_MAX_PASSWORD_LEN*2 + 1 ];
  char      szUserID[ AEEBT_MAX_USERID_LEN*2 + 1 ];
  boolean   bAccept = FALSE;

  switch ( dwParam )
  {
    case EVT_DEV_BROWSE_RESP:
    {
      MSG_LOW( "OBEXHandleUserEvents - OBEX Browse Resp Received", 0, 0, 0 );
      for ( uNoRecs=0; uNoRecs < pMe->mSD.uNumBrowseRecs; uNoRecs++ )
      {
        if ( pMe->mSD.browseRec[uNoRecs].uSvcClsId == OBEX_UUID )
        {
          bFound = TRUE;
          pMe->mOBEX.u8SCN = pMe->mSD.browseRec[uNoRecs].uSrvChanNum;
          MSG_LOW( "OBEXHandleUserEventsSCN Found to be %x", 
                   pMe->mOBEX.u8SCN, 0, 0 );
          break;
        }
      }
      if ( bFound == TRUE )
      {
        if ( pMe->mOBEX.poClient == NULL )
        {
          nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, 
                                           AEECLSID_OBEXCLIENT,
                                           (void**)&pMe->mOBEX.poClient );

          if ( nResult != SUCCESS )
          {
            MSG_FATAL( "OBEXHandleUserEvents - Cannot create Client obj",
                       0, 0, 0 );
            return;
          }
        }
        else
        {
          MSG_FATAL( "OBEXHandleUserEvents - Client Object already created",
                      0, 0, 0 );
        }
        if ( BTApp_ConnectObexServer( pMe ) != SUCCESS )
        {
          BTApp_ShowDevMsg( pMe, IDS_MSG_CONN_FAILED, 
             &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 2 );
        }
      }
      else
      {
        BTApp_ShowDevMsg( pMe, IDS_OBEX_MSG_SERVICE_NOT_FOUND, 
           &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 2 );
      }
      break;
    }
    case EVT_OBEX_AUTH:
    {
      WSTRTOSTR( pMe->mOBEX.wPassword, szPassWord, 
                 sizeof( szPassWord ) );
      DBGPRINTF_LOW ( "OBEXHandleUserEvents - Password is %s", szPassWord );
      if ( pMe->mOBEX.bUidReqd != FALSE )
      {
        WSTRTOSTR( pMe->mOBEX.wUserID, szUserID, 
                   sizeof( szUserID ) );
        DBGPRINTF_LOW ( "OBEXHandleUserEvents - UID is %s", szUserID );
        if( pMe->mOBEX.bRegistered == TRUE )
        {
          if( pMe->mOBEX.bVerifyUID == TRUE )
          {
            if( !STRCMP( pMe->mOBEX.szUIDForVerification, szUserID ) )
            {
              nResult = IOBEXServer_AuthResponse( pMe->mOBEX.poServer, 
                                                  szUserID, szPassWord );
            }
            else
            {
              IOBEXServer_AcceptConnection( pMe->mOBEX.poServer, bAccept, 
                                            NULL, u8HdrCount, 
                                            AEE_OBEX_STAT_UNAUTHORIZED );
            }
            pMe->mOBEX.bVerifyUID = FALSE;
          }
          else
          {
            nResult = IOBEXServer_AuthResponse( pMe->mOBEX.poServer, szUserID,
                                                szPassWord );
          }
        }
        else
        {
          nResult = IOBEXClient_AuthResponse( pMe->mOBEX.poClient, szUserID,
                                              szPassWord );
        }
      }
      else
      {
         if( pMe->mOBEX.bRegistered == TRUE )
        {
          nResult = IOBEXServer_AuthResponse( pMe->mOBEX.poServer, NULL, 
                                              szPassWord );
        }
        else
        {
           nResult = IOBEXClient_AuthResponse( pMe->mOBEX.poClient, NULL, 
                                               szPassWord );
        }
      }
      if ( nResult != SUCCESS )
      {
        MSG_FATAL( "OBEXHandleUserEvents - OBEX_Authenticate Resp failed", 
                   0, 0, 0 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case EVT_OBEX_DCN_IND:
    {
      pMe->mOBEX.bConnected = FALSE;
      if ( pMe->mOBEX.poClient != NULL )
      {
        IOBEXClient_Release( pMe->mOBEX.poClient );
        pMe->mOBEX.poClient = NULL;
      }
	  else if ( pMe->mOBEX.bPendingDisc == TRUE )
	  {
        // do register
        BTApp_OBEXDoServerDeregister( pMe );
		pMe->mOBEX.bPendingDisc = FALSE;
	  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      BTApp_BuildTopMenu( pMe );
      break;
    }
  }
}
/* ==========================================================================
FUNCTION BTApp_EnableOBEX
DESCRIPTION
============================================================================= */
void BTApp_EnableOBEX( 
  CBTApp*  pMe, 
  boolean* pbSettingBondable, 
  boolean* pbSettingDiscoverable )
{
  if ( pMe->mOBEX.bServerEnable == TRUE )
  {
    if ( BTApp_OBEXInit( pMe ) == FALSE )
    {
      MSG_FATAL( "EnableOBEX - failed to create OBEX object", 0, 0, 0 );
      BTApp_OBEXCleanup( pMe );
      return;
    }
    if( pMe->mOBEX.poServer == NULL )
    {
      if ( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_OBEXSERVER,
                                       (void**)&pMe->mOBEX.poServer ) != SUCCESS )
      {
        MSG_FATAL( "EnableOBEX - Cannot create OBEX Server obj",
                   0, 0, 0 );
      }
      else
      {
        if ( *pbSettingBondable == FALSE )
        {
          BTApp_SetBondable( pMe );
          *pbSettingBondable = TRUE;
        }
        MSG_LOW( "EnableOBEX - enabling OBEX", 0, 0, 0 );
        if ( BTApp_OBEXRegister( pMe ) != SUCCESS )
        {
          BTApp_ClearBondable( pMe );
          MSG_FATAL( "EnableOBEX - Enabling OBEX failed", 0, 0, 0 );
        }
        else
        {
          pMe->mOBEX.bRegistered = TRUE;
          if ( *pbSettingDiscoverable == FALSE )
          {
            IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
            *pbSettingDiscoverable = TRUE;
          }
        }
      }
    }
    else
    {
      MSG_FATAL( "Enable OBEX - Server Obj already created",
                 0, 0, 0 );
    }
  }
}

#ifdef FEATURE_APP_TEST_AUTOMATION 
#error code not present
#endif  // Automation end
#endif
#endif /* FEATURE_APP_BLUETOOTH */


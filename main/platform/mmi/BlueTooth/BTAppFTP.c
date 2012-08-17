/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppFTP.c                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains FTP test feature.        */  
/*                                                                           */  
/*              Copyright (c) 2004-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppFTP.c#5 $
$DateTime: 2009/07/10 11:37:08 $
$Author: nksingh $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
06/11/09   ns   changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
01/14/09   GS   Added support for file transfer progress indication event.
01/14/09   GS   Updated to allow longer filenames in FTP Put/Get operations.
05/12/08   VM   Replacing deprecated functions with BREW replacements.
04/10/08   VM   Changed the profile enabling mechanism to correct the failure
                in enabling profiles.
03/06/08   VM   Removed redundant IBTEXTRM_SetSecBySvcCls call
01/25/08   mh   Get full path name function is updated to pass size of buf.
12/04/07   mh   Remote BD address update and
                Profile configuration updates to app manager
11/01/07   ag   Merged Lisbon(BT2.1) code to the mainline
06/26/07   pn   Removed compiler warnings.
05/22/07   ns   Fixed automation failure while deregistering server when it is
                already connected
04/20/07   pn   Screen is refreshed when registration result is received.
03/29/07   pn   Server Reg menu is now exited right after registration 
                attempt is done.
02/01/07   JJ   Added automation support for FTP settings
11/10/06   mh   Fixed issue with FTP authentication settings.
10/10/06   gs   Added support for handling object size info in FTP Put request
09/23/06   sn   Fixed issue with server's current folder parameter being set to 
                wrong path after GET operation.
09/12/06   gs   Fixing RVCT compile issues on 7K target.
08/14/06   bk   Modified to update the client initiated obex authentication.
08/03/06   sn   Changed List Folder menu option to not perform SETPATH to 
                Server's ROOT and instead just do List Folder of Server's 
                current folder.
06/21/06   sn   Fixed issue with second and above level folder PUT operation 
                when performed second time.
06/07/06   sn   FTP connect now will not do remote device ROOT folder browsing.
05/04/06   sn   Added support to choose EFS or SD MC as storage location for 
                FTP.
03/22/06   sn   - Updated FTP main menu options to reflect app support for 
                  Client or Server one at a time.
                - During GET folder operation, FTP app deletes the folder if it 
                  already exists.
12/28/05   sn   Handled PUT/GET success case for FTP client ABORT diagnostic.
12/09/05   sn   Updated to not display error message while browsing back to FTP 
                client menu from server's root.
12/06/05   sn   Added diagnostic support for FTP Client ABORT feature.
11/02/05   dc   BTApp_DiagSendRemoteBrowsingRsp and BTApp_DiagSendLocalBrowsingRsp
                now return only the filename instead of sending all the file/folder 
                contents.
10/27/05   sn   Changed the following to handle long filenames
                - IFILEMGR_GetInfo to IFILEMGR_GetInfoEx
                - IFILEMGR_EnumNext to IFILEMGR_EnumNextEx
10/25/05   ns   Fixed automation featurization problems
10/09/05   dc   Removed ShowMessage when pMe->mFTP.bRegistered is TRUE 
                pMe->bIsConcEventActive is TRUE.
                Changed the order of inputs to the BTApp_DiagFTPSvrRegReqType.
09/28/05   ns   Removed using glodal variable for storing passkey. Always 
                build menu's from main menu during automation to avoid menu 
                stack overflow.
09/13/05   pn   BD address from CONN_REQ event is used instead of one from 
                CONNECTED event.
08/10/05   sn   'bBusyIconUp' is made TRUE to avoid handling of CLEAR key 
                press twice while registering for FTP server.
08/02/05   sn   Corrected wrong name displayed when SET PATH fails. Also 
                fixed last character truncation during PUT of a file when FTP 
                is server.
07/27/05   ns   Added support to enable FTP server during startup
07/21/05   sn   - Added soft keys such as SELECT, GET, DELETE, CREATE_FOLDER, 
                  MOVE_TO_FOLDER, MOVE_TO_ROOT, FTP_Client in remote BROWSING 
                  menu.
                - Added support to handle MOVE_TO_FOLDER, MOVE_TO_ROOT, 
                  FTP_Client soft keys.
                - Updated automation part to use soft keys.
                - Fixed a bug; FTP app now handles DISCONNECTED event properly 
                  when TOP menu is BT_APP_MENU_FTP_BROWSING.
07/12/05   dc   Added concurrency test automation interface for FTP Get 
                operation
07/06/05   sn   Added support to delete non-empty folder.
07/06/05   sn   Added app and automation support to select allowbrowse, 
                readonly options while registering FTP server.
06/27/05   ns   Added concurrency test automation interface for FTP Put 
                operation
06/27/05   sn   Added support for FTP ABORT feature
06/20/05   ns   Fixed problems with FTP automation interfaces for put file, 
                get file, list folder, create folder, delete folder
06/16/05   sn   Fixed issue with menu item selection using number keys while 
                in FTP Browsing menu; Also fixed issue with PUT of a subfolder 
                onto the server.
06/13/05   sn   - Added app and automation support to input folder name for FTP 
                  create folder feature
                - Changed FTP local browsing feature to check the maximum 
                  length of file/folder name (and not path)
                - Included check to determine whether folder name exceeds max 
                  during multi level GET folder
06/09/05   ns   FTP Authentication value will be saved in phone and will be 
                used during the phone reset, no default value during reset.
05/28/05   ns   Fixed problem with the incorrect app dehaviour when passkey 
                menu pops up during FTP connect request. BTApp_BuildMenu is 
                called instead of BTApp_FTPLoadBrowsingMenu.
05/23/05   ns   Automated bonding procedure for FTP server.
04/12/05   dc   Added Optional field to BTApp_DiagFTPCliConExReqType to handle
                PIN code menu during connection.
04/11/05   sn   Added support for local folder browsing feature to PUT local 
                files and folders onto the server
03/25/05   pn   Made use of BTApp_SetBondable() and BTApp_ClearBondable().
                Added BTApp_FTPConnect().
03/08/05   gr   Optimized Menu stack usage for Browsing Menu
                Fixed an issue with browsing in CD failed condition
                Applied the same logic to AVK_CLR in Browsing Menu
03/07/05   sn   Fixed issues with BTApp_DiagSendRemoteBrowsingRsp(), 
                BTApp_HandleFTPRemoteBrowsingMenu()
03/01/05   dc   Fixed the UI issue with the FTP Browsing Menu. Removed the 
                ShowMessage at the end of of BTApp_FTPLoadBrowsingMenu.
02/23/05   sn   Added 
                - UI support to browse backward to parent folder
                - automation support to browse through server's folders 
                  backward to parent & root folder
                - max file / folder name check
02/16/05   sn   Added FTP diagnostics support
02/04/05   sn   Added List Folder and Create Folder UI features
11/08/04   pn   Changed BTApp_res.h to btapp_res.h.
08/24/04   pn   Authentication is now optional.  Requires server deregistration
                prior to connect as client.  Correctly registers for BT 
                notification (e.g. BT events.)  Fixed problem of multiple 
                discoverable mode setting.
08/12/04   pn   Added debug messages.  Server app is now discoverable.
08/10/04   pn   Added Put tree and Get tree test. Fixed many problems.
08/03/04   pn   Client app now presents user with a list of OBEX servers
                to connect to.  Added Put Folder test. Fixed problem processing
                AEEBT_FTP_EVT_CD_REQ.
07/29/04   pn   Updated to use IBT FTP.  Exiting test menu no longer means
                quitting test.
07/26/04   pn   Added include file BTApp_res.h
07/01/04   pn   Some resource string names modified.
05/25/04   pn   Server test gets cleaned up before exiting.
05/17/04   jh   Updated parameters for call to bt_cmd_pf_ftp_cli_auth_response
05/12/04   pn   Fixed server connection problem.
                Fixed screen freezing problem when server got disconnected.
04/20/04   pn   Initial Revision.

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp_priv.h"		//"BTApp.h"

#include "BTAppUtils.h"
#include "btapp_res.h"

#ifdef FEATURE_BT_EXTPF_FTP
#include "AEEBTExtFTP.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

#define EVT_FTP_REG                 1
#define EVT_FTP_REG_FAILED          2
#define EVT_FTP_DEREG               3
#define EVT_FTP_DEREG_FAILED        4
#define EVT_FTP_CONN_REQ            5
#define EVT_FTP_AUTH_REQ            6
#define EVT_FTP_CONNECTED           7
#define EVT_FTP_CONN_FAILED         8
#define EVT_FTP_DISCONNECTED        9
#define EVT_FTP_BROWSE_FLDR_REQ    10
#define EVT_FTP_SET_FLDR_REQ       11
#define EVT_FTP_MAKE_FLDR_REQ      12
#define EVT_FTP_PUT_REQ            13
#define EVT_FTP_GET_REQ            14
#define EVT_FTP_DEL_REQ            15
#define EVT_FTP_SETPATH_REQ_FAILED 16

#define EVT_FTP_PUT_DONE           20
#define EVT_FTP_PUT_FAILED         21
#define EVT_FTP_GET_DONE           22
#define EVT_FTP_GET_FAILED         23
#define EVT_FTP_DEL_DONE           24
#define EVT_FTP_DEL_FAILED         25
#define EVT_FTP_CD_DONE            26
#define EVT_FTP_CD_FAILED          27
#define EVT_FTP_LS_DONE            28
#define EVT_FTP_LS_FAILED          29
#define EVT_FTP_MD_DONE            30
#define EVT_FTP_MD_FAILED          31

#define EVT_FTP_PROG_INFO          32

#define NAME_ONLY( f ) (STRRCHR( f, DIRECTORY_CHAR ) + 1)
#define DIR_LIST_NAME  BTAPP_ROOT_DIR DIRECTORY_STR "browselist.txt"

#define NEW_FOLDER_NAME    "newfolder"
#define FOLDER_NAME_STR    "folder name"
#define FILE_NAME_STR      "file name"
#define SEARCH_CHR         '"'
#define ROOT_FOLDER_STR    "ROOT"
#define PARENT_FOLDER_STR  "PARENT"
#define CURRENT_FOLDER_STR "CURRENT"

/* Max. path length supported by file system */
#define FILESYS_MAX_PATH          128

#define STORAGE_DEV_EFS             0
#define STORAGE_DEV_MEM_CARD        1

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

static AEEBTProgressInfo progInfo;

extern char   szOperandex[AEEBT_MAX_FILE_NAME+1];	//Add By zzg 2011_06_18
static char   szOperand[AEEBT_MAX_FILE_NAME+1];
static char   szBuf[100];
extern uint32 uBTApp_NMask;

static boolean BTApp_HandleFTPMainMenu( CBTApp* pMe, uint16 key );
static boolean BTApp_HandleFTPServerMenu( CBTApp* pMe, uint16 key );
static boolean BTApp_HandleFTPClientMenu( CBTApp* pMe, uint16 key );
static boolean BTApp_HandleFTPServerRegisterMenu( CBTApp* pMe, uint16 key );
static boolean BTApp_HandleFTPRemoteBrowsingMenu( CBTApp* pMe, uint16 key );
static boolean BTApp_HandleFTPLocalBrowsingMenu( CBTApp* pMe, uint16 key );
static boolean BTApp_HandleFTPSettingsMenu( CBTApp* pMe, uint16 key );

//static void BTApp_GetNameOfLocalObjects( CBTApp* pMe, char *pszDir );
static boolean BTApp_FTPExtractFolderContents( CBTApp* pMe );
//static int BTApp_FTPListFolder( CBTApp* pMe );
static AEEBTFTPStatus BTApp_FTPFrameFolderContents( CBTApp* pMe );
//static int BTAppFTP_RemoveDir( CBTApp* pMe, char* pszDir );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

//Add By zzg 2011_02_23
void BTApp_FTPUpdateSendingProgress(CBTApp* pMe)
{
	MSG_FATAL("***zzg FTPUpdateSendingProgress numBytes=%d, objSize=%d***", progInfo.numBytes, progInfo.objSize, 0);

	if (progInfo.numBytes > 0)
	{		
		pMe->m_fileprogInfo.numBytes	= progInfo.numBytes;
		pMe->m_fileprogInfo.objSize		= progInfo.objSize;	
	}

	if (pMe->m_pActiveDlgID == IDD_BT_FILE_PROGRESS)
	{
		MSG_FATAL("***zzg EVT_UPDATE_PROGRESS m_pActiveDlgID == IDD_BT_FILE_PROGRESS***", 0, 0, 0);

		(void) ISHELL_PostEvent(pMe->m_pShell,
								AEECLSID_BLUETOOTH_APP,
								EVT_USER_REDRAW,
								0,
								0);
	}
	else
	{
		pMe->m_eDlgRet = DLGRET_BT_FILE_PROGRESS; 
		(void) ISHELL_EndDialog(pMe->m_pShell);				
	}	
}
//Add End


/* ==========================================================================
FUNCTION:    BTApp_ProcessFTPNotifications
DESCRIPTION: Processes FTP notifications and posts appropriate user events.
============================================================================ */
void BTApp_ProcessFTPNotifications(
  CBTApp*           pMe, 
  AEEEvent          evt, 
  NotificationData* pData
)
{
  uint32 userEvent;
  uint8  uLen;

  MSG_LOW( "FTP Notif - evt=%d %d", evt, pData->uError, 0 );

  MSG_FATAL("***zzg BTApp_ProcessFTPNotifications evt=%d, uError=%d***", evt, pData->uError, 0);

  switch ( evt )
  {
    case AEEBT_FTP_EVT_REG_DONE:    // registration process done
    {
      if ( pData->uError == AEEBT_FTP_ERR_NONE )
      {
        userEvent = EVT_FTP_REG;
      }
      else 
      {
        userEvent = EVT_FTP_REG_FAILED;
      }
      BTAPP_POST_USER_EVENT( FTP, userEvent );
      break;
    }
    case AEEBT_FTP_EVT_DEREG_DONE:  // deregistration process done
    {
      if ( pData->uError == AEEBT_FTP_ERR_NONE )
      {
        userEvent = EVT_FTP_DEREG;
      }
      else 
      {
        userEvent = EVT_FTP_DEREG_FAILED;
      }
      BTAPP_POST_USER_EVENT( FTP, userEvent );
      break;
    }
    case AEEBT_FTP_EVT_CONN_REQ:    // a client wants to connect
    {      
      pMe->mFTP.remoteBDAddr   = pData->ConnReq.bdAddr;
      pMe->mFTP.bAuthenticated = !pData->ConnReq.bUnauthorized;
      BTAPP_POST_USER_EVENT( FTP, EVT_FTP_CONN_REQ );
      break;
    }
    case AEEBT_FTP_EVT_CD_REQ:      // remote device requesting to set path
    {
      char szName[ AEEBT_MAX_FILE_NAME+1 ] = "";
      MSG_LOW( "FTP Notif - CD req flag=%x", pData->SetPathReq.flag, 0, 0 );

      WSTRTOSTR( pData->SetPathReq.wName, szName, sizeof( szName ) );

      userEvent = EVT_FTP_SET_FLDR_REQ;
      switch ( pData->SetPathReq.flag )
      {
        case AEEBT_FTP_SET_PATH_TO_FOLDER:
        {
          STRLCPY( pMe->mFTP.szName, szName, sizeof( pMe->mFTP.szName ) );

          if ( (STRLEN( szName ) + STRLEN( DIRECTORY_STR ) + 
                STRLEN( pMe->mFTP.szCurrentFolder )) > AEEBT_MAX_FILE_NAME )
          {
            MSG_ERROR( "FTP SET_PATH - Folder name size exceeds max", 
                       0, 0, 0 );
            userEvent = EVT_FTP_SETPATH_REQ_FAILED;
            break;
          }

          BTApp_GetFullPathName( pMe->mFTP.szName, sizeof(pMe->mFTP.szName), 
                                 pMe->mFTP.szCurrentFolder, sizeof(pMe->mFTP.szCurrentFolder) );
          if ( pData->SetPathReq.bCreate != FALSE )
          {
            userEvent = EVT_FTP_MAKE_FLDR_REQ;
          }
          break;
        }
        case AEEBT_FTP_SET_PATH_TO_PARENT:
        {
          char* pszStr;

          STRLCPY( pMe->mFTP.szName, pMe->mFTP.szCurrentFolder, 
                   sizeof( pMe->mFTP.szName ) );
          pszStr = STRRCHR( pMe->mFTP.szName, DIRECTORY_CHAR );
          if ( pszStr != NULL )
          {
            *pszStr = NULL;
          }
          break;
        }
        case AEEBT_FTP_SET_PATH_TO_ROOT:
        {
          if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_EFS )
          {
            STRLCPY( pMe->mFTP.szName, BTAPP_ROOT_DIR, 
                     sizeof( pMe->mFTP.szName ) );
          }
          else if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_MEM_CARD )
          {
            STRLCPY( pMe->mFTP.szName, AEEFS_CARD0_DIR, 
                     sizeof( pMe->mFTP.szName ) );

            /* Removing the terminal '/' character as this character is added 
               during FTP operations. Otherwise, the path will contain "//". */
            pMe->mFTP.szName[ STRLEN(AEEFS_CARD0_DIR) - 1 ] = '\0';
          }
          break;
        }
        default:
        {
          userEvent = EVT_FTP_SETPATH_REQ_FAILED;
          MSG_ERROR( "FTP SVR: unexpected setpath request type %x", 
                     pData->SetPathReq.flag, 0, 0 );
          break;
        }
      }
      BTAPP_POST_USER_EVENT( FTP, userEvent );
      break;
    }
    case AEEBT_FTP_EVT_LS_REQ:      // remote device requesting folder listing
    {
      STRLCPY( pMe->mFTP.szName, pMe->mFTP.szCurrentFolder, 
               sizeof( pMe->mFTP.szName ) );
      if ( WSTRLEN( pData->AccessReq.wName ) > 0 )
      {
        STRLCAT( pMe->mFTP.szName, DIRECTORY_STR, sizeof( pMe->mFTP.szName ) );
        uLen = STRLEN( pMe->mFTP.szName );
        WSTRTOSTR( pData->AccessReq.wName, &pMe->mFTP.szName[ uLen ], 
                   (sizeof( pMe->mFTP.szName ) - uLen) );
      }
      BTAPP_POST_USER_EVENT( FTP, EVT_FTP_BROWSE_FLDR_REQ );
      break;
    }
    case AEEBT_FTP_EVT_PUT_REQ:
    {
      WSTRTOSTR( pData->AccessReq.wName, pMe->mFTP.szName, 
                 sizeof( pMe->mFTP.szName ) );
      pMe->mFTP.objSize = pData->AccessReq.objSize;
      BTAPP_POST_USER_EVENT( FTP, EVT_FTP_PUT_REQ );
      break;
    }
    case AEEBT_FTP_EVT_GET_REQ:
    {
      WSTRTOSTR( pData->AccessReq.wName, pMe->mFTP.szName, 
                 sizeof( pMe->mFTP.szName ) );
      BTAPP_POST_USER_EVENT( FTP, EVT_FTP_GET_REQ );
      break;
    }
    case AEEBT_FTP_EVT_DELETE_REQ:
    {
      WSTRTOSTR( pData->AccessReq.wName, pMe->mFTP.szName, 
                 sizeof( pMe->mFTP.szName ) );
      BTAPP_POST_USER_EVENT( FTP, EVT_FTP_DEL_REQ );
      break;
    }
    case AEEBT_FTP_EVT_AUTH_REQ:       // server wants authentication
    {      
      pMe->mFTP.remoteBDAddr    = pData->AuthReq.bdAddr;
      pMe->mFTP.bUserIDRequired = pData->AuthReq.bUserIDRequired;
      BTAPP_POST_USER_EVENT( FTP, EVT_FTP_AUTH_REQ );
      break;
    }
    case AEEBT_FTP_EVT_CONNECTED:      // connection is up
    {
      BTAPP_POST_USER_EVENT( FTP, EVT_FTP_CONNECTED );
      BTAppMgr_UpdateProfileType( pMe, &pMe->mFTP.remoteBDAddr, BTAPP_MGR_FTP, BTAPP_MGR_CONNECTED );
      break;
    }
    case AEEBT_FTP_EVT_PROGRESS_IND:    // progress evt
    {
		MSG_HIGH("BTAppFTP: Progress Evt: num_bytes = %d, obj_size = %d",pData->ProgressInfo.numBytes, pData->ProgressInfo.objSize, 0);
		MSG_FATAL("***zzg BTAppFTP: Progress Evt: num_bytes = %d, obj_size = %d***",pData->ProgressInfo.numBytes, pData->ProgressInfo.objSize, 0);

		//Add By zzg 2011_02_23
		if (IBTEXTFTP_GetProgressInfo(pMe->mFTP.po, &progInfo) == SUCCESS)
		{
			MSG_HIGH("BTAppFTP: GetProgInfo num_bytes = %d, obj_size = %d",progInfo.numBytes, progInfo.objSize, 0);
			BTApp_FTPUpdateSendingProgress(pMe);	
		}
		else
		{
			MSG_ERROR("BTAppFTP: GetProgInfo failed!", 0, 0, 0);
		}
		//Add End

		BTAPP_POST_USER_EVENT( FTP, EVT_FTP_PROG_INFO );
		break;
    }
    case AEEBT_FTP_EVT_CONN_FAILED:    // connection could not be brought up
    {
      BTAPP_POST_USER_EVENT( FTP, EVT_FTP_CONN_FAILED );
      break;
    }
    case AEEBT_FTP_EVT_DISCONNECTED:   // connection is down
    {
      if ( TOP_MENU == BT_APP_MENU_FTP_BROWSING )
      {
        pMe->mFTP.bEndingTest = TRUE;
      }

      BTAPP_POST_USER_EVENT( FTP, EVT_FTP_DISCONNECTED );
      BTAppMgr_UpdateProfileType( pMe, &pMe->mFTP.remoteBDAddr, BTAPP_MGR_FTP, BTAPP_MGR_DISCONNECTED );
      break;
    }
    case AEEBT_FTP_EVT_PATH_SET:       // path is set on remote server
    {
      if ( pData->uError == AEEBT_FTP_ERR_NONE )
      {
        userEvent = EVT_FTP_CD_DONE;
      }
      else
      {
        userEvent = EVT_FTP_CD_FAILED;
      }
      BTAPP_POST_USER_EVENT( FTP, userEvent );
      break;
    }
    case AEEBT_FTP_EVT_FOLDER_CREATED: // a new folder created on remote server
    {
      if ( pData->uError == AEEBT_FTP_ERR_NONE )
      {
        userEvent = EVT_FTP_MD_DONE;
      }
      else
      {
        userEvent = EVT_FTP_MD_FAILED;
      }
      BTAPP_POST_USER_EVENT( FTP, userEvent );
      break;
    }
    case AEEBT_FTP_EVT_FOLDER_LISTING: 
    { // content of current folder on remote server
    
      MSG_FATAL("***zzg BTAppFtp ProcessFTPNotify AEEBT_FTP_EVT_FOLDER_LISTING uError=%d***", pData->uError, 0, 0);
	  
      if ( pData->uError == AEEBT_FTP_ERR_NONE )
      {
        userEvent = EVT_FTP_LS_DONE;
      }
      else
      {
        userEvent = EVT_FTP_LS_FAILED;
      }
      BTAPP_POST_USER_EVENT( FTP, userEvent );
      break;
    }
    case AEEBT_FTP_EVT_PUT_DNE:        // file/folder copied to remote device
    {
      if ( pData->uError == AEEBT_FTP_ERR_NONE )
      {
        userEvent = EVT_FTP_PUT_DONE;
      }
      else
      {
        userEvent = EVT_FTP_PUT_FAILED;
      }
      BTAPP_POST_USER_EVENT( FTP, userEvent );
      break;
    }
    case AEEBT_FTP_EVT_GET_DNE:        // file/folder copied from remote device
    {
      if ( pData->uError == AEEBT_FTP_ERR_NONE )
      {
        userEvent = EVT_FTP_GET_DONE;
      }
      else
      {
        userEvent = EVT_FTP_GET_FAILED;
      }
      BTAPP_POST_USER_EVENT( FTP, userEvent );
      break;
    }
    case AEEBT_FTP_EVT_DELETE_DNE:     
    { // file/folder deleted from remote device
      if ( pData->uError == AEEBT_FTP_ERR_NONE )
      {
        userEvent = EVT_FTP_DEL_DONE;
      }
      else
      {
        userEvent = EVT_FTP_DEL_FAILED;
      }
      BTAPP_POST_USER_EVENT( FTP, userEvent );
      break;
    }
    default:
    {
      MSG_LOW( "FTP SVR: unexpected event type %x", evt, 0, 0 );
      break;
    }
  }
}

/* ==========================================================================
FUNCTION:    BTApp_FTPBuildMainMenu
DESCRIPTION: Builds FTP main menu.
============================================================================ */
void BTApp_FTPBuildMainMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_TESTS, NULL );

  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_FTP_TESTS,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }
  //Add End

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if( pMe->mFTP.bRegistered == TRUE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTP_SERVER, 0 );
  }
  else if( pMe->mFTP.bConnected == TRUE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTP_CLIENT, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTP_SERVER, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTP_CLIENT, 0 );
  }
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SETTINGS, 0 );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_FTP_TESTS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION:    BTApp_FTPBuildServerMenu
DESCRIPTION: Builds FTP server menu.
============================================================================ */
void BTApp_FTPBuildServerMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  /*
  if ( pMe->mFTP.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_FTP_SERVER, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN - len) * sizeof( AECHAR ) );
    IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
  }
  else
  {
    IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_SERVER, NULL );
  }
  */

  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
  	if ( pMe->mFTP.bConnected != FALSE )
    {
      szStatus[ 3 ] = 'C';
      ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_FTP_SERVER, 
                            pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );
      len = WSTRLEN( pMe->pText2 );
  
      STRTOWSTR( szStatus, &pMe->pText2[ len ], 
                 (SHORT_TEXT_BUF_LEN - len) * sizeof( AECHAR ) );
      //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );     		
      if(pMe->m_pIAnn != NULL)
      {
          IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
      }
    }
    else
    {
      //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_SERVER, NULL );
      AECHAR WTitle[20] = {0};
      ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_FTP_SERVER,
                         WTitle,
                         sizeof(WTitle));
 
		if(pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
		}
    }    
	
  }
  //Add End

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mFTP.bRegistered == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REGISTER, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEREGISTER, 0 );
  }
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_FTP_SERVER );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
}

/* ==========================================================================
FUNCTION:    BTApp_FTPBuildClientMenu
DESCRIPTION: Builds FTP client menu.
============================================================================ */
void BTApp_FTPBuildClientMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  /*
  if ( pMe->mFTP.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_FTP_CLIENT, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
    IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
  }
  else
  {
    IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_CLIENT, NULL );
  }
  */
  
  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
  	if ( pMe->mFTP.bConnected != FALSE )
    {
      szStatus[ 3 ] = 'C';
      ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_FTP_CLIENT, 
                            pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );
      len = WSTRLEN( pMe->pText2 );
  
      STRTOWSTR( szStatus, &pMe->pText2[ len ], 
                 (SHORT_TEXT_BUF_LEN - len) * sizeof( AECHAR ) );
      //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );     		
      if(pMe->m_pIAnn != NULL)
      {
          IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
      }
    }
    else
    {
      //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_CLIENT, NULL );
      AECHAR WTitle[20] = {0};
      ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_FTP_CLIENT,
                         WTitle,
                         sizeof(WTitle));
 
		if(pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
		}
    }    
	
  }
  //Add End

  

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mFTP.bConnected == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PUT, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTP_LIST_FOLDER, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
  }

  // Activate menu
  PUSH_MENU( BT_APP_MENU_FTP_CLIENT );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
}

/* ==========================================================================
FUNCTION:    BTApp_FTPBuildSettingsMenu
DESCRIPTION: Builds FTP setting menu.
============================================================================ */
void BTApp_FTPBuildSettingsMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SETTINGS, NULL );
  //Add By zzg 2010_11_01
  {
	  AECHAR WTitle[20] = {0};
	  ISHELL_LoadResString(pMe->m_pShell,
		                   AEE_APPSBTAPP_RES_FILE,                                
		                   IDS_SETTINGS,
		                   WTitle,
		                   sizeof(WTitle));
	  
	  if (pMe->m_pIAnn != NULL)
	  {
	  	IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	  }
  }
  //Add End

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUTHENTICATE, 
                     pMe->mFTP.bDoAuthenticate ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_EFS, 
                     ( pMe->mFTP.u8StorageDevice == STORAGE_DEV_EFS ) ? 
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_MEMORY_CARD, 
                     ( pMe->mFTP.u8StorageDevice == STORAGE_DEV_MEM_CARD ) ? 
                     IDB_BT_RADIO_FILLED : IDB_BT_RADIO_UNFILLED );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_FTP_SETTINGS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION:    BTApp_FTPBuildServerRegisterMenu
DESCRIPTION: Builds FTP server registration menu.
============================================================================ */
void BTApp_FTPBuildServerRegisterMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_REGISTER, NULL );
  //Add By zzg 2010_11_01
  {
	  AECHAR WTitle[20] = {0};
	  ISHELL_LoadResString(pMe->m_pShell,
		                   AEE_APPSBTAPP_RES_FILE,                                
		                   IDS_REGISTER,
		                   WTitle,
		                   sizeof(WTitle));
	  
	  if (pMe->m_pIAnn != NULL)
	  {
	  	IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	  }
  }
  //Add End

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTP_SRV_ALLOW_BROWSE, 
                     pMe->mFTP.reg.bBrowseAllowed ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTP_SRV_READ_ONLY, 
                     pMe->mFTP.reg.bReadOnly ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_FTP_SRV_REGISTER );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );  
}

/* ==========================================================================
FUNCTION:    BTApp_FTPBuildEnableMenu
DESCRIPTION: Builds setting menu for FTP used during feature enable.
============================================================================ */
void BTApp_FTPBuildEnableMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SETTINGS_FTP, NULL );
  //Add By zzg 2010_11_01
  {
	  AECHAR WTitle[20] = {0};
	  ISHELL_LoadResString(pMe->m_pShell,
		                   AEE_APPSBTAPP_RES_FILE,                                
		                   IDS_SETTINGS_FTP,
		                   WTitle,
		                   sizeof(WTitle));
	  
	  if (pMe->m_pIAnn != NULL)
	  {
	  	IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	  }
  }
  //Add End

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTP_SRV_ALLOW_BROWSE, 
                     pMe->mFTP.reg.bBrowseAllowed ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_FTP_SRV_READ_ONLY, 
                     pMe->mFTP.reg.bReadOnly ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_FTP_ENABLING );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );  
}

/* ==========================================================================
FUNCTION:    BTApp_FTPLoadBrowsingMenu
DESCRIPTION: Loads list folder menu with the folder contents as menu items.
============================================================================ */
void BTApp_FTPLoadBrowsingMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  AEERect    rc;
  uint8      uNoOfItems = 0;
  AECHAR     wName[ AEEBT_MAX_FILE_NAME+1 ];
  
  /* Resets (delete) the contents on the screen; Also leaves the active mode 
     from the current screen */
  IMENUCTL_Reset( pMe->m_pIMenu );

  if ( pMe->mFTP.bRemoteBrowsing )
  {
    SETAEERECT( &rc, pMe->m_rect.x, pMe->m_rect.y, pMe->m_rect.dx, 
                pMe->m_rect.dy - 20); // leave room for SK menu
    // Set the area on the device screen where the menu is drawn
    IMENUCTL_SetRect( pMe->m_pIMenu, &rc );
	
    // Set the title for the menu
    //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_FTP_LIST_FOLDER, NULL );
    //Add By zzg 2010_11_01
	  {
		  AECHAR WTitle[20] = {0};
		  ISHELL_LoadResString(pMe->m_pShell,
			                   AEE_APPSBTAPP_RES_FILE,                                
			                   IDS_FTP_LIST_FOLDER,
			                   WTitle,
			                   sizeof(WTitle));
		  
		  if (pMe->m_pIAnn != NULL)
		  {
		  	IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
		  }
	  }
  //Add End
	

    uNoOfItems = pMe->mFTP.uNumOfObjInRemoteDev;
  }
  else
  {
    // Set the area on the device screen where the menu is drawn
    IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

    // Set the title for the menu
    STRTOWSTR( "Local Browsing", wName, sizeof( wName ) );
    //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, 0, wName );
    //Add By zzg 2010_11_01
	  {
		  if (pMe->m_pIAnn != NULL)
		  {
		  	IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, wName);
		  }
	  }
  //Add End

    uNoOfItems = pMe->mFTP.uNumOfObjInLocalDev;
  }

  while ( uNoOfItems > 0 )
  {	
    if ( pMe->mFTP.bRemoteBrowsing )
    {
      STRTOWSTR( pMe->mFTP.remoteObjects[ pMe->mFTP.uNumOfObjInRemoteDev-
                 uNoOfItems ].szFolderFileName, wName, sizeof( wName ) );
      // Add the folder / file names as menu items
      IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                        pMe->mFTP.uNumOfObjInRemoteDev - uNoOfItems, wName, 0 );
    }
    else
    {
      STRTOWSTR( pMe->mFTP.localObjects[ pMe->mFTP.uNumOfObjInLocalDev-
                 uNoOfItems ].szFolderFileName, wName, sizeof( wName ) );

      // Add the folder / file names as menu items
      IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                        pMe->mFTP.uNumOfObjInLocalDev - uNoOfItems, wName, 0 );
    }

    uNoOfItems--;
  }

  // Activate the menu
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

  if ( pMe->mFTP.bRemoteBrowsing )
  {
    // Add SK menu at the bottom of screen
    IMENUCTL_Reset( pMe->m_pISoftMenu );

    // Set rect for SK menu
    rc.y  = rc.dy;
    rc.dy = pMe->m_rect.dy - rc.y;
    IMENUCTL_SetRect( pMe->m_pISoftMenu, &rc );

    BTApp_InitAddItem( &ai );

    // Add individual entries to SK menu
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_SELECT, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_GET, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_DELETE, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_FTP_CREATE_FOLDER, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_FTP_MOVE_TO_FOLDER, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_FTP_MOVE_TO_ROOT, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_FTP_CLIENT, 0 );

    IMENUCTL_SetActive( pMe->m_pISoftMenu, TRUE );
    IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
    IMENUCTL_SetSel( pMe->m_pISoftMenu, IDS_SELECT );
  }

  if ( TOP_MENU != BT_APP_MENU_FTP_BROWSING ) /* Use the stack sparingly */
  {
    PUSH_MENU( BT_APP_MENU_FTP_BROWSING );
  }

  pMe->bBusyIconUp = FALSE;

  /* When ABORT fails, file/folder transfer is still on and hence show 
     BusyIcon */
  if ( pMe->mFTP.bObjectTransfer )
  {
    ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, FALSE );
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
}

/* ==========================================================================
FUNCTION:    BTApp_FTPInit
DESCRIPTION: Registers FTP notifications and creates an object of file manager.
============================================================================ */
boolean BTApp_FTPInit( CBTApp* pMe )
{
	static boolean init_done = FALSE;
	uint32 uNMask = NMASK_BT_FTP | (((uint32)(NOTIFIER_VAL_ANY)) << 16);

	if (init_done == FALSE)
	{
		if ((ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_BLUETOOTH_FTP, (void**)&pMe->mFTP.po) == SUCCESS) 
			 && (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_FILEMGR, (void **)&pMe->mFTP.pIFileMgr ) == SUCCESS) 
			 &&	(ISHELL_RegisterNotify( pMe->m_pShell,  AEECLSID_BLUETOOTH_APP,  AEECLSID_BLUETOOTH_NOTIFIER, uNMask) == SUCCESS))
		{
		  STRLCPY(pMe->mFTP.szCurrentFolder, BTAPP_ROOT_DIR, sizeof(pMe->mFTP.szCurrentFolder));
		  STRTOWSTR(NEW_FOLDER_NAME, pMe->mFTP.wFolderName, sizeof(pMe->mFTP.wFolderName));
		  pMe->mFTP.bObjectTransfer = FALSE;
		  pMe->mFTP.u8StorageDevice = STORAGE_DEV_MEM_CARD;//STORAGE_DEV_EFS;

		  uBTApp_NMask |= NMASK_BT_FTP;
		  init_done = TRUE;

		  /* Store 'ROOT' as interop device root folder name is not known */
		  STRLCPY(szOperand, ROOT_FOLDER_STR, sizeof(szOperand));
		}
	}
	return init_done;
}

/* ==========================================================================
FUNCTION:    BTApp_FTPCleanup
DESCRIPTION: Unregisters FTP notifications and releases file manager object.
============================================================================ */
void BTApp_FTPCleanup( CBTApp* pMe )
{
  if ( pMe->mFTP.po != NULL )
  {
    IBTEXTFTP_Release( pMe->mFTP.po );
    pMe->mFTP.po = NULL;
  }
  if ( pMe->mFTP.pIFileMgr != NULL )
  {
    IFILEMGR_Release( pMe->mFTP.pIFileMgr );
    pMe->mFTP.pIFileMgr = NULL;
  }
  // unregister FTP notification
  ISHELL_RegisterNotify( pMe->m_pShell,  AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 0 );
  uBTApp_NMask &= ~NMASK_BT_FTP;
  ISHELL_RegisterNotify( pMe->m_pShell,  AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, uBTApp_NMask );
}

/* ==========================================================================
FUNCTION:    BTApp_FTPFrameFolderContents
DESCRIPTION: Frames the directory contents in XML format
============================================================================*/
static AEEBTFTPStatus BTApp_FTPFrameFolderContents( CBTApp* pMe )
{
  IFile*          pIFile;
  AEEFileInfoEx   fileInfoEx;
  char            szName[ FILESYS_MAX_PATH ];
  AEEOpenFileMode mode = _OFM_CREATE;
  AEEBTFTPStatus  status = AEEBT_FTP_STAT_SUCCESS;

  if ( IFILEMGR_Test( pMe->mFTP.pIFileMgr, DIR_LIST_NAME ) == SUCCESS )
  {
    mode = _OFM_READWRITE;
  }

  if ( IFILEMGR_Test( pMe->mFTP.pIFileMgr, pMe->mFTP.szName ) != SUCCESS )
  {
    MSG_ERROR( "FTPHndlUserEv - browse folder req folder not found", 0, 0, 0 );
    status = AEEBT_FTP_STAT_NOT_FOUND;
  }
  else if ( (pIFile = IFILEMGR_OpenFile( pMe->mFTP.pIFileMgr, DIR_LIST_NAME, 
                                         mode )) == NULL )
  {
    MSG_ERROR( "BROWSE_FLDR_REQ-Open DIR_LIST_NAME failed m=%x", mode, 0, 0 );
    status = AEEBT_FTP_STAT_FILESYS_FULL;
  }
  else
  {
    if ( (mode == _OFM_READWRITE) && (IFILE_Truncate( pIFile, 0 ) != SUCCESS) )
    {
      MSG_ERROR( "BROWSE_FLDR_REQ - can't truncate dirList file", 0, 0, 0 );
    }

    fileInfoEx.nStructSize = sizeof( AEEFileInfoEx );
    fileInfoEx.attrib = _FA_NORMAL;
    fileInfoEx.dwCreationDate = 0;
    fileInfoEx.dwSize = 0;
    fileInfoEx.pszFile = szName;
    MEMSET( fileInfoEx.pszFile, 0, sizeof( szName ) );
    fileInfoEx.nMaxFile = sizeof( szName );
    fileInfoEx.pszDescription = NULL;
    fileInfoEx.nDescriptionSize = 0;
    fileInfoEx.pClasses = 0;
    fileInfoEx.nClassesSize = 0;

    STRLCPY( szBuf, "<?xml version=\"1.0\"?>\n", sizeof( szBuf ) );
    IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

    STRLCPY( szBuf, 
            "<!DOCTYPE folder-listing SYSTEM \"obex-folder-listing.dtd\">\n", 
             sizeof( szBuf ) );
    IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

    STRLCPY( szBuf, "<folder-listing version=\"1.0\">\n", sizeof( szBuf ) );
    IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

    // enumerate directories first
    IFILEMGR_EnumInit( pMe->mFTP.pIFileMgr, pMe->mFTP.szName, TRUE );
    while ( IFILEMGR_EnumNextEx( pMe->mFTP.pIFileMgr, &fileInfoEx ) != FALSE )
    {
      SNPRINTF( szBuf, sizeof( szBuf ), "<folder name=\"%s\" ", 
                ( STRRCHR( fileInfoEx.pszFile, DIRECTORY_CHAR )+1 ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "user-perm=\"RWD\" ", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "created=\"20040128T100808Z\" ", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "modified=\"20040128T100808Z\"/>\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );
    }

    // now, enumerate files
    IFILEMGR_EnumInit( pMe->mFTP.pIFileMgr, pMe->mFTP.szName, FALSE );
    while ( IFILEMGR_EnumNextEx( pMe->mFTP.pIFileMgr, &fileInfoEx ) != FALSE )
    {
      SNPRINTF( szBuf, sizeof( szBuf ), "<file name=\"%s\" ", 
                ( STRRCHR( fileInfoEx.pszFile, DIRECTORY_CHAR )+1 ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "user-perm=\"RWD\" ", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "created=\"20040128T100808Z\" ", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "modified=\"20040128T100808Z\" ", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      SNPRINTF( szBuf, sizeof( szBuf ), "size=\"%ld\"/>\n", fileInfoEx.dwSize );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );
    }

    STRLCPY( szBuf, "</folder-listing>\n", sizeof( szBuf ) );
    IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

    IFILE_Release( pIFile );
  }

  return status;
}

/* ==========================================================================
FUNCTION:    BTApp_GetNameOfLocalObjects
DESCRIPTION: Reads name of files and folders available in the local folder
             'pszDir'
============================================================================ */
/*static*/ void BTApp_GetNameOfLocalObjects( CBTApp* pMe, char *pszDir )
{
	AEEFileInfoEx fileInfoEx;
	char          szName[FILESYS_MAX_PATH];
	char*         pszName = NULL;
	uint8         szCount = 0;

	DBGPRINTF( "BTApp_GetNameOfLocalObjects: Path is %s", pszDir);
	
	pMe->mFTP.uNumOfObjInLocalDev = 0;

	for (szCount = 0; szCount < MAX_OBJECTS; szCount++)	
	{
		MEMSET(pMe->mFTP.localObjects[szCount].szFolderFileName, 0, AEEBT_MAX_FILE_NAME + 1);
	}

	fileInfoEx.nStructSize = sizeof(AEEFileInfoEx);
	fileInfoEx.attrib = _FA_NORMAL;
	fileInfoEx.dwCreationDate = 0;
	fileInfoEx.dwSize = 0;
	fileInfoEx.pszFile = szName;
	
	MEMSET(fileInfoEx.pszFile, 0, sizeof(szName));
	
	fileInfoEx.nMaxFile = sizeof(szName);
	fileInfoEx.pszDescription = NULL;
	fileInfoEx.nDescriptionSize = 0;
	fileInfoEx.pClasses = 0;
	fileInfoEx.nClassesSize = 0;

	szCount = 0;
	
	if (IFILEMGR_EnumInit(pMe->mFTP.pIFileMgr, pszDir, TRUE) == SUCCESS)
	{
		MSG_FATAL("***zzg bRemoteBrowsing=%d 111***", pMe->mFTP.bRemoteBrowsing, 0, 0);
		
		while (IFILEMGR_EnumNextEx(pMe->mFTP.pIFileMgr, &fileInfoEx))
		{
			pszName = STRRCHR(fileInfoEx.pszFile, DIRECTORY_CHAR) + 1;

			MSG_FATAL("***zzg bRemoteBrowsing=%d, szCount=%d EnumInit_TRUE***", pMe->mFTP.bRemoteBrowsing, szCount, 0);
			MSG_FATAL("***zzg pszName=%s EnumInit_TRUE***", pszName, 0, 0);

			if (STRLEN(pszName) <= AEEBT_MAX_FILE_NAME)
			{
				MEMCPY(pMe->mFTP.localObjects[szCount].szFolderFileName, pszName, STRLEN(pszName));
				pMe->mFTP.localObjects[szCount].szTypeOfObj = AEEBT_FTP_FOLDER;
				szCount++;
			}
		}

		MSG_FATAL("***zzg bRemoteBrowsing=%d 222***", pMe->mFTP.bRemoteBrowsing, 0, 0);
	}
	else
	{
		MSG_FATAL("***zzg bRemoteBrowsing=%d 333***", pMe->mFTP.bRemoteBrowsing, 0, 0);
		
		MSG_ERROR("Could not initialize FileMgr object", 0, 0, 0);
	}

	if (IFILEMGR_EnumInit(pMe->mFTP.pIFileMgr, pszDir, FALSE) == SUCCESS)
	{
		MSG_FATAL("***zzg bRemoteBrowsing=%d 444***", pMe->mFTP.bRemoteBrowsing, 0, 0);
		
		while (IFILEMGR_EnumNextEx(pMe->mFTP.pIFileMgr, &fileInfoEx))
		{
			pszName = STRRCHR(fileInfoEx.pszFile, DIRECTORY_CHAR) + 1;

			MSG_FATAL("***zzg bRemoteBrowsing=%d, szCount=%d EnumInit_FALSE***", pMe->mFTP.bRemoteBrowsing, szCount, 0);
			DBGPRINTF("***zzg pszName=%s EnumInit_FALSE***", pszName);

			if (STRLEN(pszName) <= AEEBT_MAX_FILE_NAME)
			{
				MEMCPY(pMe->mFTP.localObjects[szCount].szFolderFileName, pszName, STRLEN(pszName));
				pMe->mFTP.localObjects[szCount].szTypeOfObj = AEEBT_FTP_FILE;
				szCount++;
			}
		}

		MSG_FATAL("***zzg bRemoteBrowsing=%d 555***", pMe->mFTP.bRemoteBrowsing, 0, 0);
	}
	else
	{
		MSG_FATAL("***zzg bRemoteBrowsing=%d 666***", pMe->mFTP.bRemoteBrowsing, 0, 0);
		
		MSG_ERROR("Could not initialize FileMgr object", 0, 0, 0);
	}

	DBGPRINTF("***zzg BTApp_GetNameOfLocalObjects szCount=%d", szCount);

	MSG_FATAL("***zzg bRemoteBrowsing=%d 777***", pMe->mFTP.bRemoteBrowsing, 0, 0);

	pMe->mFTP.uNumOfObjInLocalDev = szCount;
	MSG_MED("No of Objects is %d", pMe->mFTP.uNumOfObjInLocalDev, 0, 0);

	return;
}


/* ==========================================================================
FUNCTION:    BTApp_FTPListFolder
DESCRIPTION: Sends a request to server to GET the server's current folder 
             contents.
============================================================================*/
/*static*/ int BTApp_FTPListFolder( CBTApp* pMe )
{
  IFile*          pIFile;
  AEEOpenFileMode mode = _OFM_CREATE;
  int             result = EFAILED;
  AECHAR          wBuf[ AEEBT_MAX_FILE_NAME+1 ];

  if ( IFILEMGR_Test( pMe->mFTP.pIFileMgr, DIR_LIST_NAME ) == SUCCESS )
  {
    mode = _OFM_READWRITE;
  }

  if ( mode == _OFM_READWRITE )
  {
    if ( (pIFile = IFILEMGR_OpenFile( pMe->mFTP.pIFileMgr, DIR_LIST_NAME,
                                      mode )) != NULL )
    {
      if ( IFILE_Truncate( pIFile, 0 ) != SUCCESS )
      {
        MSG_ERROR( "FTP_CON - can't truncate dirList file", 0, 0, 0 );
      }

      IFILE_Release( pIFile );
    }
    else
    {
      MSG_ERROR( "FTP_CON - can't open dirList file, m=%d", mode, 0, 0 );
    }
  }

  STRTOWSTR( DIR_LIST_NAME, wBuf, sizeof( wBuf ) );
  result = IBTEXTFTP_ListFolder( pMe->mFTP.po, NULL, wBuf );
 
  if ( result == SUCCESS )
  {
    BTApp_ShowBusyIcon( pMe ); // wait for result
  }
  else
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

    MSG_ERROR( "FTPHndlUserEv - ListFolder failed r=%x", result, 0, 0 );
  }

  MSG_FATAL("***zzg BTApp_FTPListFolder result=%d***", result, 0, 0);

  return result;
}

/* ==========================================================================
FUNCTION:    BTApp_FTPExtractFolderContents
DESCRIPTION: Extracts the server's folder contents represented in XML format 
             from the file DIR_LIST_NAME and stores it in an array.
============================================================================*/
static boolean BTApp_FTPExtractFolderContents( CBTApp* pMe )
{
  IFile*   pIFile = NULL;
  char*    pszFolderContents = NULL;
  char*    pszTagPosition = NULL;
  char*    pszStartPos = NULL;
  char*    pszEndPos = NULL;
  FileInfo fileInfo;
  boolean  result = FALSE;
  uint32   dwBytesRead = 0;
  char     szNoOfItems = 0;
  char     szCount = 0;

  // Check whether the file DIR_LIST_NAME exists already or not
  if ( IFILEMGR_Test( pMe->mFTP.pIFileMgr, DIR_LIST_NAME ) == SUCCESS )
  {
    // File DIR_LIST_NAME exists, open it for reading its contents
    pIFile = IFILEMGR_OpenFile( pMe->mFTP.pIFileMgr, DIR_LIST_NAME, 
                                _OFM_READWRITE );
    if ( pIFile != NULL )
    {
      // Get the file size for the file DIR_LIST_NAME  
      if ( IFILE_GetInfo( pIFile, &fileInfo ) == SUCCESS )
      {
        pszFolderContents = (char*) MALLOC( fileInfo.dwSize+1 );
        if ( pszFolderContents != NULL )
        {
          MEMSET( pszFolderContents, 0, fileInfo.dwSize+1 );

          // Read the contents of the file DIR_LIST_NAME 
          dwBytesRead = IFILE_Read( pIFile, pszFolderContents, 
                                    fileInfo.dwSize );

          if ( dwBytesRead > 0 )
          {
            // Initialize the required members
            pMe->mFTP.uNumOfObjInRemoteDev = 0;
            for( szCount=0; szCount<MAX_OBJECTS; szCount++ )
            {
              MEMSET( pMe->mFTP.remoteObjects[ szCount ].szFolderFileName, 0, 
                      AEEBT_MAX_FILE_NAME+1 );
            }

            /* Parse the contents read from the file DIR_LIST_NAME
               First, extract the folder names present, if any */
            pszTagPosition = pszFolderContents;
            while ( pszTagPosition != NULL )
            {
              // Look for the tag "folder name"
              pszTagPosition = STRSTR( pszTagPosition, FOLDER_NAME_STR );
              if ( pszTagPosition == NULL )
              { 
                break;
              }

              /* Look for the start of the double quote ("); Data is available 
                 within the double quotes */
              pszStartPos = STRCHR( pszTagPosition, SEARCH_CHR );
              if ( pszStartPos == NULL )
              {
                break;
              } 

              // Look for the end of the double quote (")
              pszEndPos = STRCHR( pszStartPos+1, SEARCH_CHR );
              if ( pszEndPos == NULL )
              {
                break;
              }

              if ( pszEndPos - ( pszStartPos+1 ) <= AEEBT_MAX_FILE_NAME )
              {  
                // Copy the name of the folder in an array
                MEMCPY( pMe->mFTP.remoteObjects[ szNoOfItems ].szFolderFileName, 
                        pszStartPos+1, pszEndPos - ( pszStartPos+1 ) );

				MSG_FATAL("***zzg szTypeOfObj 3***", 0, 0, 0);
                pMe->mFTP.remoteObjects[ szNoOfItems ].szTypeOfObj = AEEBT_FTP_FOLDER;
                szNoOfItems++;
                if ( szNoOfItems >= MAX_OBJECTS )
                {
                  break;
                }
              }

              pszTagPosition = pszEndPos + 1;
            }

            // Now, extract the file names present, if any
            pszStartPos = pszEndPos = NULL;
            pszTagPosition = pszFolderContents;
            while ( pszTagPosition != NULL )
            {  
              // Look for the tag "file name"
              pszTagPosition = STRSTR( pszTagPosition, FILE_NAME_STR );
              if ( pszTagPosition == NULL )
              {
                break;
              }

              /* Look for the start of the double quote ("); Data is available 
                 within the double quotes */
              pszStartPos = STRCHR( pszTagPosition, SEARCH_CHR );
              if ( pszStartPos == NULL )
              {
                break;
              }  

              // Look for the end of the double quote (")
              pszEndPos = STRCHR( pszStartPos+1, SEARCH_CHR );
              if ( pszEndPos == NULL )
              { 
                break;
              }

              /* Copy the name of the file in an array, if no. of items found 
                 is less than MAX_OBJECTS */
              if ( szNoOfItems < MAX_OBJECTS )
              { 
                if ( pszEndPos - ( pszStartPos+1 ) <= AEEBT_MAX_FILE_NAME )
                {
                  MEMCPY( pMe->mFTP.remoteObjects[szNoOfItems].szFolderFileName,
                          pszStartPos+1, pszEndPos - ( pszStartPos+1 ) );

				  MSG_FATAL("***zzg szTypeOfObj 4***", 0, 0, 0);
                  pMe->mFTP.remoteObjects[ szNoOfItems ].szTypeOfObj = AEEBT_FTP_FILE;
                  szNoOfItems++;
                }
              }
              else 
              { 
                break;
              }
              pszTagPosition = pszEndPos + 1;
            }  

            /* Store the number of items (folder/file) found on the server's 
               current folder */
            pMe->mFTP.uNumOfObjInRemoteDev = szNoOfItems;

            /* Server's current folder contents has been retrieved and stored 
               in an array */
            result = TRUE;
          }

          // Free the memory that holds the contents of the file DIR_LIST_NAME
          FREE( pszFolderContents );
        }
        else
        {
          MSG_ERROR( "Memory allocation failed", 0, 0, 0 );
        } 
      }

      // Close the file
      IFILE_Release( pIFile );
    }
    else
    {
      MSG_ERROR( "Could not open the file DIR_LIST_NAME successfully", 
                 0, 0, 0 );
    }
  }

  return result;
}

/* ==========================================================================
FUNCTION:    BTApp_FTPBuildMenu
DESCRIPTION: Builds FTP menus.
============================================================================ */
boolean BTApp_FTPBuildMenu( CBTApp* pMe, BTAppMenuType menu )
{
  boolean built = TRUE;
  switch ( menu )
  {
    case BT_APP_MENU_FTP_TESTS:
      if ( BTApp_FTPInit( pMe ) != FALSE )
      {
        BTApp_FTPBuildMainMenu( pMe );
      }
      else
      {
        MSG_ERROR( "FTPBuildMenu - failed to create FTP object", 0, 0, 0 );
        BTApp_FTPCleanup( pMe );
        built = FALSE;
      }
      break;
    case BT_APP_MENU_FTP_SERVER:
      BTApp_FTPBuildServerMenu( pMe );
      break;
    case BT_APP_MENU_FTP_CLIENT:
      BTApp_FTPBuildClientMenu( pMe );
      break;
    case BT_APP_MENU_FTP_SETTINGS:
      BTApp_FTPBuildSettingsMenu( pMe );
      break;
    case BT_APP_MENU_FTP_SRV_REGISTER:
      BTApp_FTPBuildServerRegisterMenu( pMe );
      break;
    case BT_APP_MENU_FTP_BROWSING:
      BTApp_FTPLoadBrowsingMenu( pMe );
      break;
    case BT_APP_MENU_FTP_CREATE_FLDR:
      PUSH_MENU( BT_APP_MENU_FTP_CREATE_FLDR );
      built = ( ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                     IDD_BT_TEXT_EDIT, NULL ) == SUCCESS );
      break;
    case BT_APP_MENU_FTP_MOVE_TO_FLDR:
      PUSH_MENU( BT_APP_MENU_FTP_MOVE_TO_FLDR );
      built = ( ISHELL_CreateDialog( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                                     IDD_BT_TEXT_EDIT, NULL ) == SUCCESS );
      break;
    default:
      built = FALSE;
      break;
  }
  return built;
}

/* ==========================================================================
FUNCTION:    BTAppFTP_RemoveDir
DESCRIPTION: Deletes an empty / non-empty folder from FS.
============================================================================ */
/*static*/ int BTAppFTP_RemoveDir( CBTApp* pMe, char* pszDir )
{
  char          szFldrName[ AEEBT_MAX_FILE_NAME*2+1 ];
  char          *pszName = NULL;
  AEEFileInfoEx fileInfoEx;
  char          szName[ FILESYS_MAX_PATH ];
  boolean       bSubDirFound = FALSE;
  int           result = SUCCESS;

  fileInfoEx.nStructSize = sizeof( AEEFileInfoEx );
  fileInfoEx.attrib = _FA_NORMAL;
  fileInfoEx.dwCreationDate = 0;
  fileInfoEx.dwSize = 0;
  fileInfoEx.pszFile = szName;
  MEMSET( fileInfoEx.pszFile, 0, sizeof( szName ) );
  fileInfoEx.nMaxFile = sizeof( szName );
  fileInfoEx.pszDescription = NULL;
  fileInfoEx.nDescriptionSize = 0;
  fileInfoEx.pClasses = 0;
  fileInfoEx.nClassesSize = 0;

  STRLCPY( szFldrName, pszDir, sizeof( szFldrName ) );

  while( STRCMP( szFldrName, pszDir ) >= 0 )
  {
    if( IFILEMGR_EnumInit( pMe->mFTP.pIFileMgr, szFldrName, FALSE ) == SUCCESS )
    {
      while( IFILEMGR_EnumNextEx( pMe->mFTP.pIFileMgr, &fileInfoEx ) )
      {
        result = IFILEMGR_Remove( pMe->mFTP.pIFileMgr, fileInfoEx.pszFile );
        if( result != SUCCESS )
        {
          DBGPRINTF( "File %s could not be removed", fileInfoEx.pszFile );
          return result;
        }
      }
    }
    else
    {
      MSG_MED( "Failed to initialize IFileMgr interface object", 0, 0, 0 );
      return EFAILED;
    }

    if( IFILEMGR_EnumInit( pMe->mFTP.pIFileMgr, szFldrName, TRUE ) == SUCCESS )
    {
      while( IFILEMGR_EnumNextEx( pMe->mFTP.pIFileMgr, &fileInfoEx ) )
      {
        bSubDirFound = TRUE;
        STRLCPY( szFldrName, fileInfoEx.pszFile, sizeof( szFldrName ) );
        break;
      }

      if( bSubDirFound == FALSE )
      {
        result = IFILEMGR_RmDir( pMe->mFTP.pIFileMgr, szFldrName );
        if( result != SUCCESS )
        {
          DBGPRINTF( "Dir %s could not be removed", szFldrName );
          return result;
        }
        else
        {
          pszName = STRRCHR( szFldrName, DIRECTORY_CHAR );
          if ( pszName != NULL )
          {
            *pszName = NULL;
          }
        }
      }
    }
    else
    {
      MSG_MED( "Failed to initialize IFileMgr interface object", 0, 0, 0 );
      return EFAILED;
    }

    bSubDirFound = FALSE;
  }

  return result;
}

/* ==========================================================================
FUNCTION:    BTApp_FTPHandleSelection
DESCRIPTION: Handles FTP menu selection.
============================================================================ */
boolean BTApp_FTPHandleSelection( CBTApp* pMe, uint16 sel )
{
  int     result;
  uint16  selection;
  boolean key_handled = TRUE;
  AECHAR  wBuf[ AEEBT_MAX_FILE_NAME+1 ];
  boolean bRegistered = FALSE;

  switch ( sel )
  {
    case IDS_FTP_SERVER:
    {
      BTApp_FTPBuildServerMenu( pMe );
      break;
    }
    case IDS_FTP_CLIENT:
    {
      BTApp_FTPBuildClientMenu( pMe );
      break;
    }
    case IDS_SETTINGS:
    {
      BTApp_FTPBuildSettingsMenu( pMe );
      break;
    }
    case IDS_AUTHENTICATE:
    {
      pMe->bConfigChanged = TRUE;
      pMe->mFTP.bDoAuthenticate = pMe->mFTP.bDoAuthenticate ? FALSE : TRUE;
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      BTApp_UpdateMenuItemImage( 
        pMe->m_pIDisplay, pMe->m_pIMenu, selection,
        pMe->mFTP.bDoAuthenticate ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
      BTApp_WriteConfigFile( pMe );
      break;
    }
    case IDS_EFS:
    {
      pMe->mFTP.u8StorageDevice = STORAGE_DEV_EFS;
      BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, pMe->m_pIMenu, IDS_EFS, 
                                 IDB_BT_RADIO_FILLED );
      BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, pMe->m_pIMenu, 
                                 IDS_MEMORY_CARD, IDB_BT_RADIO_UNFILLED );

      STRLCPY( pMe->mFTP.szCurrentFolder, BTAPP_ROOT_DIR, 
               sizeof( pMe->mFTP.szCurrentFolder ) );
      break;
    }
    case IDS_MEMORY_CARD:
    {
      pMe->mFTP.u8StorageDevice = STORAGE_DEV_MEM_CARD;
      BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, pMe->m_pIMenu, IDS_EFS, 
                                 IDB_BT_RADIO_UNFILLED );
      BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, pMe->m_pIMenu, 
                                 IDS_MEMORY_CARD, IDB_BT_RADIO_FILLED );

      STRLCPY( pMe->mFTP.szCurrentFolder, AEEFS_CARD0_DIR, 
               sizeof( pMe->mFTP.szCurrentFolder ) );

      /* Removing the terminal '/' character as this character is added during 
         FTP operations. Otherwise, the path will contain "//". */
      pMe->mFTP.szCurrentFolder[ STRLEN(AEEFS_CARD0_DIR) - 1 ] = '\0';
      break;
    }
    case IDS_REGISTER:
    {
      key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_FTP_SRV_REGISTER );
      break;
    }
    case IDS_DEREGISTER:
    {
      if ( (result = IBTEXTFTP_Deregister( pMe->mFTP.po )) != SUCCESS )
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

        MSG_ERROR( "FTP_Deregister() failed with %x", result, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      }
      else
      {
        bRegistered = pMe->mFTP.bRegistered; //backing up of bRegistered value
        pMe->mFTP.bRegistered = FALSE;
        BTApp_CheckToClearDiscoverable( pMe );
        pMe->mFTP.bRegistered = bRegistered;
        BTApp_ShowBusyIcon( pMe ); // wait for command done
      }
      break;
    }
    case IDS_CONNECT:
    {
      if ( pMe->mFTP.bRegistered == FALSE )
      {
        key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_OBEX_SERVERS );
      }
      else
      {
        //BTApp_ShowMessage( pMe, IDS_MSG_DEREG_SVR_FIRST, NULL, 0 );
      }
      break;
    }
    case IDS_PUT:
    {
      pMe->mFTP.bRemoteBrowsing = FALSE;

      if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_EFS )
      {
        STRLCPY( pMe->mFTP.szLocalBrowsePath, BTAPP_ROOT_DIR, 
                 sizeof( pMe->mFTP.szLocalBrowsePath ) );
        STRLCPY( pMe->mFTP.szCurrentFolder, BTAPP_ROOT_DIR, 
                 sizeof( pMe->mFTP.szCurrentFolder ) );

        BTApp_GetNameOfLocalObjects( pMe, pMe->mFTP.szLocalBrowsePath );
      }
      else if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_MEM_CARD )
      {
        STRLCPY( pMe->mFTP.szLocalBrowsePath, AEEFS_CARD0_DIR, 
                 sizeof( pMe->mFTP.szLocalBrowsePath ) );
        STRLCPY( pMe->mFTP.szCurrentFolder, AEEFS_CARD0_DIR, 
                 sizeof( pMe->mFTP.szCurrentFolder ) );

        /* Removing the terminal '/' character as this character is added 
           during FTP operations. Otherwise, the path will contain "//". */
        pMe->mFTP.szLocalBrowsePath[ STRLEN(AEEFS_CARD0_DIR) - 1 ] = '\0';
        pMe->mFTP.szCurrentFolder[ STRLEN(AEEFS_CARD0_DIR) - 1 ] = '\0';

        BTApp_GetNameOfLocalObjects( pMe, pMe->mFTP.szLocalBrowsePath );
      }

      key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_FTP_BROWSING );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      break;
    }
    case IDS_FTP_LIST_FOLDER:
    {
      pMe->mFTP.bRemoteBrowsing = TRUE;

      /* Store 'ROOT' as interop device root folder name is not known */
      STRLCPY( szOperand, ROOT_FOLDER_STR, sizeof( szOperand ) );

      if( BTApp_FTPListFolder( pMe ) != SUCCESS )
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

        STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
        BTApp_ShowMessage( pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wBuf, 3 );
      }
      else
      {
        BTApp_ShowBusyIcon( pMe ); // wait for LS done
      }
      break;
    }
    case IDS_DISCONNECT:
    {
      if ( (result = IBTEXTFTP_Disconnect( pMe->mFTP.po )) != SUCCESS )
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

        MSG_ERROR( "FTP_Disconnect() failed with %x", result, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
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
FUNCTION:    BTApp_HandleFTPMainMenu
DESCRIPTION: Handles FTP main menu selection.
============================================================================ */
static boolean BTApp_HandleFTPMainMenu( CBTApp* pMe, uint16 key )
{
  uint16  selection;
  boolean ev_processed = TRUE;

  MSG_MED( "HndlFTPMain - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      ev_processed = BTApp_FTPHandleSelection( pMe, selection );
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
  return ev_processed;
}

/* ==========================================================================
FUNCTION:    BTApp_HandleFTPServerMenu
DESCRIPTION: Handles server menu selection.
============================================================================ */
static boolean BTApp_HandleFTPServerMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlFTPServer - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      ev_processed = BTApp_FTPHandleSelection( pMe, selection );
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
FUNCTION:    BTApp_HandleFTPClientMenu
DESCRIPTION: Handles client menu selection.
============================================================================ */
static boolean BTApp_HandleFTPClientMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlFTPClient - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      ev_processed = BTApp_FTPHandleSelection( pMe, selection );
      break;
    }
    case AVK_CLR:
    {
      if ( pMe->mFTP.bEndingTest == FALSE )
      {
        BTApp_HandleClearKey( pMe );
      }
      break;
    }
    default:
      ev_processed = FALSE;
      break;
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION:    BTApp_HandleFTPSettingsMenu
DESCRIPTION: Handles FTP settings menu.
============================================================================ */
static boolean BTApp_HandleFTPSettingsMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlFTPSettings - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      ev_processed = BTApp_FTPHandleSelection( pMe, selection );
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
FUNCTION:    BTApp_HandleFTPServerRegisterMenu
DESCRIPTION: Handles FTP server registration menu.
============================================================================ */
static boolean BTApp_HandleFTPServerRegisterMenu( CBTApp* pMe, uint16 key )
{
  uint16  selection;
  int     result;
  boolean ev_processed = TRUE;

  MSG_MED( "HndlFTPSettings - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_RIGHT:
    case AVK_LEFT:
    {
      break;
    }
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      switch( selection )
      {
        case IDS_FTP_SRV_ALLOW_BROWSE:
        {
          pMe->mFTP.reg.bBrowseAllowed = 
            pMe->mFTP.reg.bBrowseAllowed ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mFTP.reg.bBrowseAllowed ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
        case IDS_FTP_SRV_READ_ONLY:
        {
          pMe->mFTP.reg.bReadOnly = pMe->mFTP.reg.bReadOnly ? FALSE : TRUE;
          BTApp_UpdateMenuItemImage( pMe->m_pIDisplay, 
            pMe->m_pIMenu, selection, 
            pMe->mFTP.reg.bReadOnly ? IDB_BT_CHECK_ON : IDB_BT_CHECK_OFF );
          break;
        }
      }

      break;
    }
    case AVK_CLR:
    {
      if ( pMe->mFTP.bDoAuthenticate == FALSE )
      {
        pMe->mFTP.reg.auth         = AEEBT_FTP_AUTH_NONE;
      }
      else
      {
        pMe->mFTP.reg.auth         = AEEBT_FTP_AUTH_PASSWORD;
      }

      STRLCPY( pMe->mFTP.reg.szServiceName, "QC FTP", 
               sizeof( pMe->mFTP.reg.szServiceName ) );

      MSG_LOW( "FTP_Register() - auth=%x %d %d", pMe->mFTP.reg.auth, 
               pMe->mFTP.reg.bReadOnly, pMe->mFTP.reg.bBrowseAllowed );
      BTApp_SetBondable( pMe );
      if ( (result = IBTEXTFTP_Register( pMe->mFTP.po, 
                                         &pMe->mFTP.reg )) != SUCCESS )
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

        MSG_ERROR( "FTP_Register() failed with %x", result, 0, 0 );
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
      }
      else
      {
        if ( pMe->mSD.bDiscoverable == FALSE )
        {
          IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
        }
        BTApp_ShowBusyIcon( pMe ); // wait for command done
      }
      (void) POP_MENU(); // done with this menu
      break;
    }
    default:
    {
      ev_processed = FALSE;
      break;
    }
  }

  return ev_processed;
}

/* ==========================================================================
FUNCTION:    BTApp_HandleFTPRemoteBrowsingMenu
DESCRIPTION: Handles the remote device Browsing Menu selection. 
             AVK_SELECT - Retrieves information about the selected folder's 
                          contents from server. In case of file, displays 
                          error message.
             AVK_RIGHT  - Copies the selected folder's contents from server.
             AVK_LEFT   - Deletes the selected folder / file from the server.
=============================================================================*/
static boolean BTApp_HandleFTPRemoteBrowsingMenu( CBTApp* pMe, uint16 key )
{
  /* The max value of uIndex is 256. When MAX_OBJECTS - the number of menu items
     that can be displayed on the menu exceeds 256, uIndex type has to be 
     changed to unsigned short */
  uint8   uIndex = 0x00; 
  boolean ev_processed = TRUE;
  AECHAR  wName[ AEEBT_MAX_FILE_NAME+1 ];
  AECHAR  wBuf[ AEEBT_MAX_FILE_NAME+1 ];
  int     result = 0;

  MSG_MED( "HndlFTPSettings - k=0x%x MenuSel %d SoftMenuSel %d", key, 
           IMENUCTL_GetSel( pMe->m_pIMenu ), 
           IMENUCTL_GetSel( pMe->m_pISoftMenu ) );

  switch ( key )
  {
    /* Server's folder is selected by the user to browse its contents. 
       In case when a file is selected, then it is GOT from server. */
    case AVK_SELECT:
    {
      switch( IMENUCTL_GetSel( pMe->m_pISoftMenu ) )
      {
        case IDS_SELECT:
        {
          if ( IMENUCTL_GetItemCount( pMe->m_pIMenu ) > 0 )
          {
            uIndex = IMENUCTL_GetSel( pMe->m_pIMenu );
            // Get the name of the menu item selected on the screen
            STRTOWSTR( pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName, wName, 
                       sizeof( wName ) ); 

            /* Check whether the selected item is a folder or file. If file, 
               then GET the file. If folder, then browse the folder */
            if ( pMe->mFTP.remoteObjects[ uIndex ].szTypeOfObj == AEEBT_FTP_FILE )
            {
              if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_EFS )
              {
                STRLCPY( szOperand, BTAPP_ROOT_DIR, sizeof( szOperand ) );
                STRLCAT( szOperand, DIRECTORY_STR, sizeof( szOperand ) );
              }
              else if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_MEM_CARD )
              {
                STRLCPY( szOperand, AEEFS_CARD0_DIR, sizeof( szOperand ) );
              }

              if ( (STRLEN( szOperand ) + 
                   STRLEN( pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName )) > 
                   AEEBT_MAX_FILE_NAME ) 
              {
                MSG_ERROR( "File / Folder name exceeds max", 0, 0, 0 );
                BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wName, 3 );
                break;            
              }

              STRLCAT( szOperand, 
                       pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName, 
                       sizeof( szOperand ) );
              // Remove if the file already exists
              if ( IFILEMGR_Test( pMe->mFTP.pIFileMgr, szOperand ) == SUCCESS )
              {
                if ( IFILEMGR_Remove( pMe->mFTP.pIFileMgr, szOperand ) != SUCCESS )
                {
                  DBGPRINTF( "Could not remove the file %s", szOperand );
                }
              }
          
              STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );

              if ( IBTEXTFTP_Get( pMe->mFTP.po, 
                                  pMe->mFTP.remoteObjects[ uIndex ].szTypeOfObj, 
                                  wBuf, wName ) != SUCCESS )
              {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

                MSG_ERROR( "FTP file Get failed", 0, 0, 0 );
                BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wName, 3 );
              }
              else
              {
                /* Copying the object name just to display once GET is over */
                STRLCPY( szOperand, 
                         pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName, 
                         sizeof( szOperand ) );
                pMe->mFTP.bObjectTransfer = TRUE;
                ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, 
                              FALSE );
              }
              break;
            }

            if ( IBTEXTFTP_SetPath( pMe->mFTP.po, wName, 
                                    AEEBT_FTP_SET_PATH_TO_FOLDER ) != SUCCESS )
            {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

              MSG_ERROR( "Set Path to folder failed", 0, 0, 0 );
              BTApp_ShowMessage( pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wName, 3 );
            }
            else
            {
              /* Storing folder name to display it later in case of failure */
              STRLCPY( szOperand, 
                       pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName, 
                       sizeof( szOperand ) );
              BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
            }
          }

          break;
        }
        case IDS_GET: /* Folder/file selected will be GOT from server */
        {
          if ( IMENUCTL_GetItemCount( pMe->m_pIMenu ) > 0 )
          {
            uIndex = IMENUCTL_GetSel( pMe->m_pIMenu );
            // Get the name of the menu item selected on the screen
            STRTOWSTR( pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName, wName, 
                       sizeof( wName ) );

            if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_EFS )
            {
              STRLCPY( szOperand, BTAPP_ROOT_DIR, sizeof( szOperand ) );
              STRLCAT( szOperand, DIRECTORY_STR, sizeof( szOperand ) );
            }
            else if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_MEM_CARD )
            {
              STRLCPY( szOperand, AEEFS_CARD0_DIR, sizeof( szOperand ) );
            }

            if ( (STRLEN( szOperand ) + 
                 STRLEN( pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName )) > 
                 AEEBT_MAX_FILE_NAME ) 
            {
              MSG_ERROR( "File / Folder name exceeds max", 0, 0, 0 );
              BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wName, 3 );
              break;            
            }

            STRLCAT( szOperand, 
                     pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName, 
                     sizeof( szOperand ) );
            STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
            DBGPRINTF( "GET file/folder name is %s", szOperand );

            if ( pMe->mFTP.remoteObjects[ uIndex ].szTypeOfObj == AEEBT_FTP_FILE )
            {
              // Remove if the file already exists
              if ( IFILEMGR_Test( pMe->mFTP.pIFileMgr, szOperand ) == SUCCESS )
              {
                if ( IFILEMGR_Remove( pMe->mFTP.pIFileMgr, szOperand ) != SUCCESS )
                {
                  MSG_MED( "Could not remove the file", 0, 0, 0 );
                }
              }
            }
            else
            {
              // Remove if the folder already exists
              if( IFILEMGR_Test( pMe->mFTP.pIFileMgr, szOperand ) == SUCCESS )
              {
                if( BTAppFTP_RemoveDir( pMe, szOperand ) != SUCCESS )
                {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

                  MSG_ERROR( "Folder exists but couldn't be removed", 0, 0, 0 );
                  MSG_MED( "FTP Folder Get failed", 0, 0, 0 );
                  BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wName, 3 );
                  break;
                }
              }
            }

            if ( IBTEXTFTP_Get( pMe->mFTP.po, 
                 pMe->mFTP.remoteObjects[ uIndex ].szTypeOfObj, wBuf, wName ) != SUCCESS )
            {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

              MSG_ERROR( "FTP File / Folder Get failed", 0, 0, 0 );
              BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wName, 3 );
            }
            else
            { 
              // Copying the object name just to display once GET is over 
              STRLCPY( szOperand, 
                       pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName, 
                       sizeof( szOperand ) );
              pMe->mFTP.bObjectTransfer = TRUE;
              ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, 
                            FALSE );
            }
          }

          break;
        }
        case IDS_DELETE: /* Folder/file selected will be deleted from server */
        {
          if ( IMENUCTL_GetItemCount( pMe->m_pIMenu ) > 0 )
          {
            uIndex = IMENUCTL_GetSel( pMe->m_pIMenu );
            // Get the name of the menu item selected on the screen
            STRTOWSTR( pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName, wName, 
                       sizeof( wName ) );

            // Just storing folder name to display it later 
            STRLCPY( szOperand, 
                     pMe->mFTP.remoteObjects[ uIndex ].szFolderFileName, 
                     sizeof( szOperand ) );

            if ( IBTEXTFTP_Delete( pMe->mFTP.po, wName ) != SUCCESS )
            {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

              MSG_ERROR( "FTP_Delete() failed", 0, 0, 0 );
              BTApp_ShowMessage( pMe, IDS_MSG_FILE_DELETE_FAILED, wName, 3 );
            }
            else
            {
              BTApp_ShowBusyIcon( pMe ); // wait for result
            }
          }

          break;
        }
        case IDS_FTP_CREATE_FOLDER:
        {
          BTApp_BuildMenu( pMe, BT_APP_MENU_FTP_CREATE_FLDR );
          break;
        }
        case IDS_FTP_MOVE_TO_FOLDER:
        {
          STRLCPY( szOperand, NEW_FOLDER_NAME, sizeof( szOperand ) );
          BTApp_BuildMenu( pMe, BT_APP_MENU_FTP_MOVE_TO_FLDR );
          break;
        }
        case IDS_FTP_MOVE_TO_ROOT:
        {
          if ( IBTEXTFTP_SetPath( pMe->mFTP.po, NULL, 
                                  AEEBT_FTP_SET_PATH_TO_ROOT ) != SUCCESS )
          {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

            MSG_ERROR( "Set Path to ROOT failed", 0, 0, 0 );
            /* Clear to client Menu if SetPath fails */
            ev_processed = BTApp_HandleClearKey( pMe );
          }
          else
          {
            /* Store 'ROOT' as interop device root folder name is not known */
            STRLCPY( szOperand, ROOT_FOLDER_STR, sizeof( szOperand ) );
            BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
          }

          break;
        }
        case IDS_FTP_CLIENT:
        {
          ev_processed = BTApp_HandleClearKey( pMe );
          break;
        }
        default:
        {
          ev_processed = FALSE;
          break;
        }
      }

      break;
    }
    case AVK_CLR:
    {
      if( pMe->mFTP.bObjectTransfer )
      {
        result = IBTEXTFTP_Abort( pMe->mFTP.po );
        if( result != SUCCESS )
        {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

          MSG_MED( "Abort failed %d", result, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_MSG_ABORT_FAILED, 0, 2 );
        }
        else
        {
          pMe->mFTP.bObjectTransfer = FALSE;
          MSG_MED( "Aborted", 0, 0, 0 );
        }
      }
      else
      {
        if ( TOP_MENU == BT_APP_MENU_FTP_BROWSING )
        {
          /* Tries to set server's path back to parent and list the folder 
             contents of parent. This is for use case scenario when the user 
             migrates from one browsing menu to another backwards. 
             There is no need to handle clear key as that will be handled as 
             soon as CD fails. */
          if ( IBTEXTFTP_SetPath( pMe->mFTP.po, NULL, 
                                  AEEBT_FTP_SET_PATH_TO_PARENT ) != SUCCESS )
          {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

            MSG_ERROR( "Set Path to parent failed", 0, 0, 0 );
            /* Clear to client Menu if SetPath fails */
            ev_processed = BTApp_HandleClearKey( pMe );
          }
          else
          {
            /* Store 'PARENT' as interop device parent folder name is not 
               known */
            STRLCPY( szOperand, PARENT_FOLDER_STR, sizeof( szOperand ) );
            BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
          }
        }
      }

      break;
    }
    default:
    {
      ev_processed = FALSE;
      break;
    }
  }

  return ev_processed;
}

/* ==========================================================================
FUNCTION:    BTApp_HandleFTPLocalBrowsingMenu
DESCRIPTION: Handles FTP local folder browsing menu selection to PUT local
             files and folders on to the server
============================================================================ */
static boolean BTApp_HandleFTPLocalBrowsingMenu( CBTApp* pMe, uint16 key )
{
  /* The max value of uIndex is 256. When MAX_OBJECTS - the number of menu items
     that can be displayed on the menu exceeds 256, uIndex type has to be 
     changed to unsigned short */
  uint8   uIndex = 0x00; 
  boolean ev_processed = TRUE;
  char    szName[ AEEBT_MAX_FILE_NAME+1 ];
  AECHAR  wName[ AEEBT_MAX_FILE_NAME+1 ]; 
  AECHAR  wBuf[ AEEBT_MAX_FILE_NAME+1 ];
  int     result=0;

  MSG_MED( "HndlFTPSettings - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_SELECT:
    {
      if ( IMENUCTL_GetItemCount( pMe->m_pIMenu ) > 0 )
      {
        uIndex = IMENUCTL_GetSel( pMe->m_pIMenu );

        STRTOWSTR( pMe->mFTP.localObjects[ uIndex ].szFolderFileName, wName, 
                   sizeof( wName ) );

        if ( pMe->mFTP.localObjects[ uIndex ].szTypeOfObj == AEEBT_FTP_FILE )
        {
          if ( STRLEN( pMe->mFTP.szLocalBrowsePath ) + STRLEN( DIRECTORY_STR ) +
               STRLEN( pMe->mFTP.localObjects[ uIndex ].szFolderFileName ) > 
               AEEBT_MAX_FILE_NAME )
          {
            MSG_ERROR( "File name exceeds max", 0, 0, 0 );
            BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wName, 3 );
            ev_processed = FALSE;
            break;
          }

          STRLCPY( szName, pMe->mFTP.szLocalBrowsePath, sizeof( szName ) );
          STRLCAT( szName, DIRECTORY_STR, sizeof( szName ) );
          STRLCAT( szName, pMe->mFTP.localObjects[ uIndex ].szFolderFileName, 
                   sizeof( szName ) );
          STRTOWSTR( szName, wBuf, sizeof( wBuf ) );
          STRLCPY( szOperand, pMe->mFTP.localObjects[uIndex].szFolderFileName, 
                   sizeof( szOperand ) );

          if ( (result = IBTEXTFTP_Put( pMe->mFTP.po, 
                pMe->mFTP.localObjects[ uIndex ].szTypeOfObj, wBuf, wName )) !=
                SUCCESS )
          {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

            MSG_MED( "Put file failed. Result=%d", result, 0, 0 );
            BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wName, 3 );
            ev_processed = FALSE;
          }
          else         
          {
            pMe->mFTP.bObjectTransfer = TRUE;
            ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, 
                          FALSE );
          }
          
          break;
        }

        /* Folder name having more than AEEBT_MAX_FILE_NAME characters will 
           not be stored at all and hence there is no need to check for it */
        if ( STRLEN( pMe->mFTP.szLocalBrowsePath ) + STRLEN( DIRECTORY_STR ) + 
             STRLEN( pMe->mFTP.localObjects[ uIndex ].szFolderFileName ) > 
             AEEBT_MAX_FILE_NAME * 2 )
        {
            MSG_ERROR( "Folder name path exceeds max*2", 0, 0, 0 );
            BTApp_ShowMessage( pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wName, 3 );
            ev_processed = FALSE;
            break;
        }

        STRLCAT( pMe->mFTP.szLocalBrowsePath, DIRECTORY_STR, 
                 sizeof( pMe->mFTP.szLocalBrowsePath ) );
        STRLCAT( pMe->mFTP.szLocalBrowsePath, 
                 pMe->mFTP.localObjects[ uIndex ].szFolderFileName, 
                 sizeof( pMe->mFTP.szLocalBrowsePath ) );

        /* Local browse path is copied on to szCurrentFolder so that PUT of a
           local device subfolder points to the right path to read and PUT the 
           contents on to the server */
        STRLCPY( pMe->mFTP.szCurrentFolder, pMe->mFTP.szLocalBrowsePath, 
                 sizeof( pMe->mFTP.szCurrentFolder ) );

        BTApp_GetNameOfLocalObjects( pMe, pMe->mFTP.szLocalBrowsePath );
        BTApp_BuildMenu( pMe, BT_APP_MENU_FTP_BROWSING );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      }
      break;
    }
    case AVK_RIGHT:
    {
      if ( IMENUCTL_GetItemCount( pMe->m_pIMenu ) > 0 )
      {
        uIndex = IMENUCTL_GetSel( pMe->m_pIMenu );

        STRTOWSTR( pMe->mFTP.localObjects[ uIndex ].szFolderFileName, wName, 
                   sizeof( wName ) );

        if ( STRLEN( pMe->mFTP.szLocalBrowsePath ) + STRLEN( DIRECTORY_STR ) + 
             STRLEN( pMe->mFTP.localObjects[ uIndex ].szFolderFileName ) > 
             AEEBT_MAX_FILE_NAME )
        {
          MSG_ERROR( "File / Folder name exceeds max", 0, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wName, 3 );
          ev_processed = FALSE;
          break;
        }

        STRLCPY( szName, pMe->mFTP.szLocalBrowsePath, sizeof( szName ) );
        STRLCAT( szName, DIRECTORY_STR, sizeof( szName ) );
        STRLCAT( szName, pMe->mFTP.localObjects[ uIndex ].szFolderFileName, 
                 sizeof( szName ) );
        DBGPRINTF( "PUT file/folder name is %s", szName );

        STRTOWSTR( szName, wBuf, sizeof( wBuf ) );
        STRLCPY( szOperand, pMe->mFTP.localObjects[ uIndex ].szFolderFileName, 
                 sizeof( szOperand ) );

        if ( (result = IBTEXTFTP_Put( pMe->mFTP.po, 
              pMe->mFTP.localObjects[ uIndex ].szTypeOfObj, wBuf, wName )) != 
              SUCCESS )
        {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

          MSG_MED( "Put file/folder failed. Result=0x%d", result, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wName, 3 );
          ev_processed = FALSE;
        }
        else
        {
          pMe->mFTP.bObjectTransfer = TRUE;
          ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, 
                        FALSE );
        }
      }
      break;
    }
    case AVK_CLR:
    {
      if( pMe->mFTP.bObjectTransfer )
      {
        result = IBTEXTFTP_Abort( pMe->mFTP.po );
        if( result != SUCCESS )
        {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

          MSG_MED( "Abort failed %d", result, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_MSG_ABORT_FAILED, 0, 2 );
        }
        else
        {
          pMe->mFTP.bObjectTransfer = FALSE;
          MSG_MED( "Aborted", 0, 0, 0 );
        } 
      }
      else
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }
    default:
    {
      ev_processed = FALSE;
      MSG_MED( "Received key press event = %x", key, 0, 0 );
      break;
    }
  }

  return ev_processed;
}

/* ==========================================================================
FUNCTION:    BTApp_FTPHandleMenus
DESCRIPTION: Handles FTP menus. 
============================================================================ */
boolean BTApp_FTPHandleMenus( CBTApp* pMe, uint16 key, BTAppMenuType menu )
{
  uint16  selection;
  boolean handled = TRUE;

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    if ( TOP_MENU != BT_APP_MENU_FTP_BROWSING )
    {
      handled = BTApp_FTPHandleSelection( pMe, selection );
    }
  }
  else
  {
    switch ( menu )
    {
      case BT_APP_MENU_FTP_TESTS:
        handled = BTApp_HandleFTPMainMenu( pMe, key );
        break;
      case BT_APP_MENU_FTP_SERVER:
        handled = BTApp_HandleFTPServerMenu( pMe, key );
        break;
      case BT_APP_MENU_FTP_CLIENT:
        handled = BTApp_HandleFTPClientMenu( pMe, key );
        break;
      case BT_APP_MENU_FTP_SETTINGS:
        handled = BTApp_HandleFTPSettingsMenu( pMe, key );
        break;
      case BT_APP_MENU_FTP_SRV_REGISTER:
        handled = BTApp_HandleFTPServerRegisterMenu( pMe, key );
        break;
      case BT_APP_MENU_FTP_BROWSING:
	  	
        if ( pMe->mFTP.bRemoteBrowsing )
        {
          handled = BTApp_HandleFTPRemoteBrowsingMenu( pMe, key );
        }
        else
        {
          handled = BTApp_HandleFTPLocalBrowsingMenu( pMe, key );
        }
        break;
      default:
        MSG_ERROR(" FTP: unexpected menu type %x", menu, 0, 0 );
        handled = FALSE;
        break;
    }
  }

  return handled;
}

/* ==========================================================================
FUNCTION:    BTApp_FTPHandleUserEvents
DESCRIPTION: Handles FTP user events and takes / displays appropriate 
             action / message.
============================================================================ */
void BTApp_FTPHandleUserEvents( CBTApp* pMe, uint32 dwParam )
{
  AECHAR          wBuf[ (2*AEEBT_MAX_FILE_NAME)+1 ];
  char*           pszTmp;
  char            szName[ AEEBT_MAX_FILE_NAME+1 ] = "";
  int             result;
  BTAppMenuType   menu;
  AEEBTFTPStatus  status = AEEBT_FTP_STAT_SUCCESS;

  MSG_FATAL("***zzg BTApp_FTPHandleUserEvents dwParam=%d***", dwParam, 0, 0);

  switch ( dwParam )
  {
    case EVT_FTP_REG:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      pMe->mFTP.bRegistered = TRUE;
      if(pMe->mEnablingType != BTAPP_ENABLING_NONE)
      {
        pMe->mEnablingType++;       
        BTApp_EnableBT(pMe);
      }
      else
      {
        BTApp_BuildMenu( pMe, TOP_MENU );
      } 
	  
      break;
    }
    case EVT_FTP_REG_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      if(pMe->mEnablingType != BTAPP_ENABLING_NONE)
      {
        pMe->mEnablingType++;       
        BTApp_EnableBT(pMe);
      } 
      else
      {
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
      }
	  	 
      break;
    }
    case EVT_FTP_DEREG:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION
		
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_DONE, NULL, 2 );
      pMe->mFTP.bRegistered = FALSE;
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
	  
      break;
    }
    case EVT_FTP_DEREG_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION
		  
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      break;
    }
    case EVT_FTP_AUTH_REQ:
    {
      if ( pMe->mFTP.bAuthenticated == FALSE )
      {
        //BTApp_BuildMenu( pMe, BT_APP_MENU_FTP_PASSWORD );		
		pMe->m_edit_id = IDS_FTP_CLIENT;
		pMe->m_bEditNeedStr = FALSE;		

		pMe->m_edit_state_id = BTAPPST_FTP_CLIENT;								

		pMe->m_eDlgRet = DLGRET_BT_EDIT; 
		(void) ISHELL_EndDialog(pMe->m_pShell);		
		
      }
      else
      {
        /* Already authenticated once. 
           Not sure about the third parameter USERID and hence NULL */
        IBTEXTFTP_Authenticate( pMe->mFTP.po, &pMe->mFTP.remoteBDAddr, NULL, 
                                pMe->mFTP.wPassWord );
      }
      break;
    }
    case EVT_FTP_CONN_REQ:
    {
      if ( pMe->mFTP.bAuthenticated == FALSE )
      {
        BTApp_BuildMenu( pMe, BT_APP_MENU_FTP_PASSWORD );
      }
      else
      {
        if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_EFS )
        {
          STRTOWSTR( BTAPP_ROOT_DIR, wBuf, sizeof( wBuf ) );
        }
        else if( pMe->mFTP.u8StorageDevice == STORAGE_DEV_MEM_CARD )
        {
          STRLCPY( szName, AEEFS_CARD0_DIR, sizeof( szName ) );

          /* Removing the terminal '/' character as this character is added 
             during FTP operations. Otherwise, the path will contain "//". */
          szName[ STRLEN(AEEFS_CARD0_DIR) - 1 ] = '\0';
          STRTOWSTR( szName, wBuf, sizeof( wBuf ) );
        }

        IBTEXTFTP_AcceptConnection( pMe->mFTP.po, &pMe->mFTP.remoteBDAddr,
                                    TRUE, wBuf );
      }
      break;
    }
    case EVT_FTP_CONNECTED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      pMe->mFTP.bConnected = TRUE;
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore

      if( pMe->mFTP.bRegistered == TRUE ) // server
      {
        /* Store 'ROOT' as interop device root folder name is not known */
        STRLCPY( szOperand, ROOT_FOLDER_STR, sizeof( szOperand ) );
      }

	  MSG_FATAL("***zzg BTApp_FTPHandleUserEvents EVT_FTP_CONNECTED***", 0, 0, 0);

	  //Add By zzg 2011_06_08
	  pMe->m_eDlgRet = DLGRET_FTP_CLIENT;
      (void)ISHELL_EndDialog(pMe->m_pShell);  
	  //Add End
	  
      //BTApp_BuildTopMenu( pMe );	//Del By zzg 2011_06_07
      break;
    }
    case EVT_FTP_CONN_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      break;
    }
    case EVT_FTP_DISCONNECTED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      pMe->mFTP.bConnected = FALSE;
      if ( pMe->mFTP.bEndingTest != FALSE )
      {
        pMe->mFTP.bEndingTest = FALSE;
        BTApp_HandleClearKey( pMe );
      }
      else
      {
        BTApp_BuildTopMenu( pMe ); // rebuild menu to remove 'C'
      }
      break;
    }
    case EVT_FTP_BROWSE_FLDR_REQ:
    {
      status = BTApp_FTPFrameFolderContents( pMe );

      STRTOWSTR( DIR_LIST_NAME, wBuf, sizeof( wBuf ) );
      result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_LS, status, 
                                     wBuf );
      if ( result != SUCCESS )
      {
        MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
      }
      break;
    }
    case EVT_FTP_SET_FLDR_REQ:
    {
      pszTmp = STRRCHR( pMe->mFTP.szName, DIRECTORY_CHAR );
      if( pszTmp++ == NULL )
      {
        pszTmp = pMe->mFTP.szName;
      }
      MSG_LOW( "FTPHndlUserEv - Set Folder %c%c%c", pszTmp[0],
               pszTmp[1], pszTmp[2] );

      if( IFILEMGR_Test( pMe->mFTP.pIFileMgr, pMe->mFTP.szName ) == SUCCESS )
      {
        STRLCPY( pMe->mFTP.szCurrentFolder, pMe->mFTP.szName, 
                 sizeof( pMe->mFTP.szCurrentFolder ) );
      }
      else
      {
        MSG_ERROR( "FTPHndlUserEv - folder not found %c%c%c", 
                   pszTmp[0], pszTmp[1], pszTmp[2] );
        status = AEEBT_FTP_STAT_NOT_FOUND;
      }

      STRTOWSTR( pMe->mFTP.szName, wBuf, sizeof( wBuf ) );
      result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_CD, status, 
                                     wBuf );
      if ( result != SUCCESS )
      {
        MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
      }
      break;
    }
    case EVT_FTP_MAKE_FLDR_REQ:
    {
      MSG_LOW( "FTPHndlUserEv - Make Folder %c%c%c", pMe->mFTP.szName[0],
               pMe->mFTP.szName[1], pMe->mFTP.szName[2] );

      STRTOWSTR( pMe->mFTP.szName, wBuf, sizeof( wBuf ) );
      if ( STRLEN( pMe->mFTP.szName ) <= AEEBT_MAX_FILE_NAME )
      {
        if ( IFILEMGR_Test( pMe->mFTP.pIFileMgr, pMe->mFTP.szName ) != SUCCESS )
        {
          if ( IFILEMGR_MkDir( pMe->mFTP.pIFileMgr, 
                               pMe->mFTP.szName ) == SUCCESS )
          {
            STRLCPY( pMe->mFTP.szCurrentFolder, pMe->mFTP.szName, 
                     sizeof( pMe->mFTP.szCurrentFolder ) );
          }
          else
          {
            MSG_ERROR( "FTPHndlUserEv - make folder req failed", 0, 0, 0 );
            status = AEEBT_FTP_STAT_FILESYS_FULL;
          }
        }
        else
        {
          MSG_ERROR( "FTPHndlUserEv - make folder req: folder exists", 0, 0, 0 );
          status = AEEBT_FTP_STAT_EXISTS;
        }
      }
      else
      {
        status = AEEBT_FTP_STAT_NOT_ALLOWED;
        MSG_ERROR( "FTPHndlUserEv - GET req: Folder name size exceeds max", 
                   0, 0, 0 );
      }

      result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_CD, status, 
                                     wBuf );
      if ( result != SUCCESS )
      {
        MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
      }
      break;
    }
    case EVT_FTP_SETPATH_REQ_FAILED:
    {
      STRTOWSTR( pMe->mFTP.szName, wBuf, sizeof( wBuf ) );
      result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_CD, 
                                     AEEBT_FTP_STAT_BAD_REQ, wBuf );

      if ( result != SUCCESS )
      {
        MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
      }
      break;
    }
    case EVT_FTP_PUT_REQ:
    {
      AECHAR* pwName = NULL;

      MSG_HIGH( "FTPHndlUserEv - put req: Obj size = %d bytes", 
                pMe->mFTP.objSize, 0, 0 );

      if ( (STRLEN( pMe->mFTP.szName ) + STRLEN( DIRECTORY_STR ) + 
            STRLEN( pMe->mFTP.szCurrentFolder )) > AEEBT_MAX_FILE_NAME*2 )
      {
        MSG_ERROR( "FTPHndlUserEv - put req: File/Folder name size exceeds max",
                   0, 0, 0 );
        STRTOWSTR( pMe->mFTP.szName, wBuf, sizeof( wBuf ) );
        result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_PUT, 
                                       AEEBT_FTP_STAT_NOT_ALLOWED, wBuf );
        if ( result != SUCCESS )
        {
          MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
        }

        break; 
      }

      BTApp_GetFullPathName( pMe->mFTP.szName, sizeof(pMe->mFTP.szName), 
                             pMe->mFTP.szCurrentFolder, sizeof(pMe->mFTP.szCurrentFolder) );
      STRTOWSTR( pMe->mFTP.szName, wBuf, sizeof( wBuf ) );
      pwName = WSTRRCHR( wBuf, DIRECTORY_CHAR ) + 1;
      if ( (IFILEMGR_Test( pMe->mFTP.pIFileMgr, 
                           pMe->mFTP.szName ) == SUCCESS) &&
           (IFILEMGR_Remove( pMe->mFTP.pIFileMgr, 
                             pMe->mFTP.szName ) != SUCCESS) )
      {
        MSG_ERROR( "FTPHndlUserEv - put req: failed to remove file", 0, 0, 0 );
      }
      if ( IFILEMGR_Test( pMe->mFTP.pIFileMgr, pMe->mFTP.szName ) == SUCCESS )
      {
        MSG_ERROR( "FTPHndlUserEv - put req: file exists", 0, 0, 0 );
        result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_PUT, 
                                       AEEBT_FTP_STAT_EXISTS, wBuf );
        if ( result != SUCCESS )
        {
          MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
        }
      }
      else if ( IBTEXTFTP_Get( pMe->mFTP.po, AEEBT_FTP_FILE,
                               wBuf, pwName ) != SUCCESS )
      {
        MSG_ERROR( "FTPHndlUserEv - put req: FTP_Get failed", 0, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, pwName, 3 );
        result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_PUT, 
                                       AEEBT_FTP_STAT_NOT_ALLOWED, wBuf );
        if ( result != SUCCESS )
        {
          MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
        }
      }
      else
      {
        // store the value to display file name when get is done
        WSTRTOSTR( pwName, szOperand, sizeof( szOperand ) );
        if ( pMe->mFTP.bRegistered != TRUE ) // client?
        {
          BTApp_ShowBusyIcon( pMe ); // wait for get done
        }
      }
      break;
    }
    case EVT_FTP_PROG_INFO:
    {
	  pMe->bUpdateProgress	  = TRUE; //Add By zzg 2011_02_23
		
      if (IBTEXTFTP_GetProgressInfo(pMe->mFTP.po, &progInfo) == SUCCESS)
      {
        MSG_HIGH("BTAppFTP: GetProgInfo num_bytes = %d, obj_size = %d", progInfo.numBytes, progInfo.objSize, 0);
      }
      else
      {
        MSG_ERROR("BTAppFTP: GetProgInfo failed!", 0, 0, 0);
      }
      break;
    }
    case EVT_FTP_GET_REQ:
    {
      AECHAR* pwName = NULL;

      if ( (STRLEN( pMe->mFTP.szName ) + STRLEN( DIRECTORY_STR ) + 
            STRLEN( pMe->mFTP.szCurrentFolder )) > AEEBT_MAX_FILE_NAME*2 )
      {
        MSG_ERROR( "FTPHndlUserEv - get req: File/Folder name size exceeds max",
                   0, 0, 0 );
        STRTOWSTR( pMe->mFTP.szName, wBuf, sizeof( wBuf ) );
        result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_GET, 
                                       AEEBT_FTP_STAT_NOT_ALLOWED, wBuf );
        if ( result != SUCCESS )
        {
          MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
        }

        break; 
      }

      BTApp_GetFullPathName( pMe->mFTP.szName, sizeof(pMe->mFTP.szName), 
                             pMe->mFTP.szCurrentFolder, sizeof(pMe->mFTP.szCurrentFolder) );

      STRTOWSTR( pMe->mFTP.szName, wBuf, sizeof( wBuf ) );
      pwName = WSTRRCHR( wBuf, DIRECTORY_CHAR ) + 1;
      if ( IFILEMGR_Test( pMe->mFTP.pIFileMgr, pMe->mFTP.szName ) != SUCCESS )
      {
        MSG_ERROR( "FTPHndlUserEv - get req: file not found", 0, 0, 0 );
        result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_GET, 
                                       AEEBT_FTP_STAT_NOT_FOUND, wBuf );
        if ( result != SUCCESS )
        {
          MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
        }
      }
      else if ( IBTEXTFTP_Put( pMe->mFTP.po, AEEBT_FTP_FILE, wBuf, 
                               pwName ) != SUCCESS )
      {
        MSG_ERROR( "FTPHndlUserEv - get req: FTP_Put failed", 0, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, pwName, 3 );
        result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_GET, 
                                       AEEBT_FTP_STAT_NOT_ALLOWED, wBuf );
        if ( result != SUCCESS )
        {
          MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
        }
      }
      else
      {
        // store the value to display file name when put is done
        WSTRTOSTR( pwName, szOperand, sizeof( szOperand ) );
        if ( pMe->mFTP.bRegistered != TRUE ) // client?
        {
          BTApp_ShowBusyIcon( pMe ); // wait for put done
        }
      }
      break;
    }
    case EVT_FTP_DEL_REQ:
    {
      AEEFileInfoEx fileInfoEx;

      STRTOWSTR( pMe->mFTP.szName, wBuf, sizeof( wBuf ) );

      if ( STRLEN( pMe->mFTP.szName ) > AEEBT_MAX_FILE_NAME*2 )
      {
        MSG_ERROR( "FTPHndlUserEv - del req: File/Folder name size exceeds max",
                   0, 0, 0 );
        result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_DEL, 
                                       AEEBT_FTP_STAT_NOT_ALLOWED, wBuf );
        if ( result != SUCCESS )
        {
          MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
        }

        break; 
      }

      BTApp_GetFullPathName( pMe->mFTP.szName, sizeof(pMe->mFTP.szName), 
                             pMe->mFTP.szCurrentFolder, sizeof(pMe->mFTP.szCurrentFolder) );

      if ( IFILEMGR_Test( pMe->mFTP.pIFileMgr, pMe->mFTP.szName ) != SUCCESS )
      {
        MSG_ERROR( "FTPHndlUserEv - del req: file/folder not found", 0, 0, 0 );
        status = AEEBT_FTP_STAT_NOT_FOUND;
      }
      else 
      {
        fileInfoEx.nStructSize = sizeof( AEEFileInfoEx );
        fileInfoEx.attrib = _FA_NORMAL;
        fileInfoEx.dwCreationDate = 0;
        fileInfoEx.dwSize = 0;
        fileInfoEx.pszFile = NULL;
        fileInfoEx.nMaxFile = 0;
        fileInfoEx.pszDescription = NULL;
        fileInfoEx.nDescriptionSize = 0;
        fileInfoEx.pClasses = 0;
        fileInfoEx.nClassesSize = 0;

        result = IFILEMGR_GetInfoEx( pMe->mFTP.pIFileMgr, pMe->mFTP.szName, 
                                   &fileInfoEx );
        if( result != SUCCESS )
        {
          status = AEEBT_FTP_STAT_NOT_ALLOWED;
        }
        else
        {
          if( fileInfoEx.attrib == _FA_DIR )
          {
            result = BTAppFTP_RemoveDir( pMe, pMe->mFTP.szName );
            if( result != SUCCESS )
            {
              status = AEEBT_FTP_STAT_NOT_ALLOWED;
            }
          }
          else
          {
            result = IFILEMGR_Remove( pMe->mFTP.pIFileMgr, pMe->mFTP.szName );
            if( result != SUCCESS )
            {
              status = AEEBT_FTP_STAT_NOT_ALLOWED;
            }
          }
        }
      }

      result = IBTEXTFTP_SendResult( pMe->mFTP.po, AEEBT_FTP_REQ_DEL, status, 
                                     wBuf );
      if ( result != SUCCESS )
      {
        MSG_ERROR( "FTPHndlUserEv - SendResult failed r=%x", result, 0, 0 );
      }
      break;
    }
    case EVT_FTP_PUT_DONE:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      pMe->mFTP.bObjectTransfer = FALSE;
      //STRTOWSTR(szOperand, wBuf, sizeof(wBuf));
      STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18

      /* Once PUT folder is done on FTP client, app gets set path to ROOT 
         request and 'szCurrentFolder' is set to ROOT. Copy current local 
         device path to 'szCurrentFolder' so that second and above level folder 
         PUT operation can be performed */

      if( pMe->mFTP.bRegistered == FALSE )
      {
        STRLCPY( pMe->mFTP.szCurrentFolder, pMe->mFTP.szLocalBrowsePath, sizeof( pMe->mFTP.szCurrentFolder ) );
      }

      if ( pMe->mFTP.bRegistered != TRUE )
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
        {
          BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT, wBuf, 2 );
        }
      }
      break;
    }
    case EVT_FTP_PUT_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      pMe->mFTP.bObjectTransfer = FALSE;
      //STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
      STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18
      
      if ( pMe->mFTP.bRegistered != TRUE )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_FILE_PUT_FAILED, wBuf, 2 );
      }
      break;
    }
    case EVT_FTP_GET_DONE:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      pMe->mFTP.bObjectTransfer = FALSE;
      //STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
	  STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18
      if ( pMe->mFTP.bRegistered != TRUE )
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION
        {
          BTApp_ShowMessage( pMe, IDS_MSG_FILE_GOT, wBuf, 2 );
        }
      }
      break;
    }
    case EVT_FTP_GET_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      pMe->mFTP.bObjectTransfer = FALSE;
      //STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
      STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18
      if ( pMe->mFTP.bRegistered != TRUE )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_FILE_GET_FAILED, wBuf, 2 );
      }
      break;
    }
    case EVT_FTP_DEL_DONE:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      //STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
      STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18
      
      /* Display file/folder name which got deleted */
      BTApp_ShowMessage( pMe, IDS_MSG_FILE_DELETED, wBuf, 2 );

      //STRLCPY( szOperand, CURRENT_FOLDER_STR, sizeof( szOperand ) );
      STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18
      
      if ( BTApp_FTPListFolder( pMe ) != SUCCESS )
      {
        /* Display current folder name */
        //STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
        STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18
        
        BTApp_ShowMessage( pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wBuf, 3 );
      }
      break;
    }
    case EVT_FTP_DEL_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      //STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
      STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18
      
      BTApp_ShowMessage( pMe, IDS_MSG_FILE_DELETE_FAILED, wBuf, 2 );
      break;
    }
    case EVT_FTP_CD_DONE:
    {
      MSG_MED( "FTPHndlUserEv - Set Path done", 0, 0, 0 );
	  MSG_FATAL("***zzg BTAppFTP EVT_FTP_CD_DONE m_pActiveDlgID=%x***", pMe->m_pActiveDlgID, 0, 0);

      if (BTApp_FTPListFolder(pMe) != SUCCESS)
      {
      		MSG_FATAL("***zzg BTAppFTP EVT_FTP_CD_DONE BTApp_FTPListFolder=FALSE***", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

        /* This check is to differentiate MOVE_TO_FOLDER and SELECT operations; 
           Both uses SETPATH. */
        if ( IMENUCTL_GetSel( pMe->m_pISoftMenu ) == IDS_FTP_MOVE_TO_FOLDER )
        {
          BTApp_ShowMessage( pMe, IDS_MSG_FOLDER_BROWSING_FAILED, 
                             pMe->mFTP.wFolderName, 2 );
          STRTOWSTR( NEW_FOLDER_NAME, pMe->mFTP.wFolderName, 
                     sizeof( pMe->mFTP.wFolderName ) );
        }
        else
        {
          //STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
          STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18
          
          BTApp_ShowMessage( pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wBuf, 3 );
        }
      }	  
      break;
    }
    case EVT_FTP_CD_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

		MSG_ERROR( "FTPHndlUserEv - Set Path failed", 0, 0, 0 );

		//Add By zzg 2011_02_22		
		MSG_FATAL("*** zzg FTPHndlUserEv EVT_FTP_CD_FAILED m_pActiveDlgID=%x***", pMe->m_pActiveDlgID, 0, 0);

		//BTApp_ShowMessage(pMe, IDS_MSG_FOLDER_BROWSING_FAILED, pMe->mFTP.wFolderName, 2);

		if (pMe->m_pActiveDlgID == IDD_BT_FTP_BROWSE)	
		{
			pMe->m_eDlgRet = DLGRET_FTP_CLIENT; 		
			(void) ISHELL_EndDialog(pMe->m_pShell);
		}
		else if (pMe->m_pActiveDlgID == IDD_BT_FTP_CLIENT)	
		{
			BTApp_ShowMessage(pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wBuf, 2);
		}		
		//Add End
		
				

		/*
		menu = TOP_MENU;

		//This check is to differentiate MOVE_TO_FOLDER and SELECT operations; 
		//Both uses SETPATH. 
		if ((menu == BT_APP_MENU_FTP_BROWSING) 
			&& (IMENUCTL_GetSel(pMe->m_pISoftMenu) == IDS_FTP_MOVE_TO_FOLDER) 
			&& (STRCMP(szOperand, PARENT_FOLDER_STR) != 0))
		{
			BTApp_ShowMessage(pMe, IDS_MSG_FOLDER_BROWSING_FAILED, pMe->mFTP.wFolderName, 2);
			STRTOWSTR(NEW_FOLDER_NAME, pMe->mFTP.wFolderName, sizeof(pMe->mFTP.wFolderName));
		}
		else if ((menu == BT_APP_MENU_FTP_BROWSING ) 
				&& (STRCMP(szOperand, PARENT_FOLDER_STR) != 0))
		{
			STRTOWSTR(szOperand, wBuf, sizeof(wBuf));
			BTApp_ShowMessage(pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wBuf, 2);
		}
		else if (menu == BT_APP_MENU_FTP_CLIENT)
		{
			STRTOWSTR(szOperand, wBuf, sizeof(wBuf));
			BTApp_ShowMessage(pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wBuf, 2);
		}
		else
		{
			MSG_MED("Moving to FTP Client menu", 0, 0, 0);
			BTApp_HandleClearKey(pMe);
		}
		*/

		break;
    }
    case EVT_FTP_MD_DONE:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      BTApp_ShowMessage( pMe, IDS_FTP_MSG_FOLDER_CREATED, NULL, 2 );

      //WSTRTOSTR( pMe->mFTP.wFolderName, szOperand, sizeof( szOperand ) );
      WSTRTOSTR( pMe->mFTP.wFolderName, szOperandex, sizeof( szOperand ) );//Add By zzg 2011_06_18
            
      if ( BTApp_FTPListFolder( pMe ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_FOLDER_BROWSING_FAILED, 
                           pMe->mFTP.wFolderName, 3 );
      }

      STRTOWSTR( NEW_FOLDER_NAME, pMe->mFTP.wFolderName, 
                 sizeof( pMe->mFTP.wFolderName ) );
      break;
    }
    case EVT_FTP_MD_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      BTApp_ShowMessage( pMe, IDS_FTP_MSG_FOLDER_CREATION_FAILED, NULL, 2 );
      STRTOWSTR( NEW_FOLDER_NAME, pMe->mFTP.wFolderName, 
                 sizeof( pMe->mFTP.wFolderName ) );
      break;
    }
    case EVT_FTP_LS_DONE:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

	  MSG_FATAL("***zzg FTPHandle EVT_FTP_LS_DONE***", 0, 0, 0);

      /* Client stores server's current folder contents in DIR_LIST_NAME
         Parse the contents of DIR_LIST_NAME and then extract the folder 
         and file names and store */
      if ( !BTApp_FTPExtractFolderContents( pMe ) )
      {
        MSG_ERROR( "Could not extract the folder contents successfully", 
                   0, 0, 0 );
        //STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
        STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18
        
        BTApp_ShowMessage( pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wBuf, 2 );
        break;
      }

	  MSG_FATAL("***zzg FTPHandle BTApp_BuildMenu BT_APP_MENU_FTP_BROWSING***", 0, 0, 0);


	  pMe->m_eDlgRet = DLGRET_FTP_BROWSE; 
	  (void) ISHELL_EndDialog(pMe->m_pShell);		

      //BTApp_BuildMenu(pMe, BT_APP_MENU_FTP_BROWSING);
      
#ifndef FEATURE_APP_TEST_AUTOMATION
      if ( IFILEMGR_Remove( pMe->mFTP.pIFileMgr, DIR_LIST_NAME ) != SUCCESS )
      {
        MSG_ERROR( "LS_DONE - can't remove dirList file", 0, 0, 0 );
      }
#endif
      break;
    }
    case EVT_FTP_LS_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      //STRTOWSTR( szOperand, wBuf, sizeof( wBuf ) );
      STRTOWSTR(szOperandex, wBuf, sizeof(wBuf));	//Add By zzg 2011_06_18
      
      BTApp_ShowMessage( pMe, IDS_MSG_FOLDER_BROWSING_FAILED, wBuf, 2 );
      if ( (IFILEMGR_Test( pMe->mFTP.pIFileMgr, DIR_LIST_NAME ) == SUCCESS) &&
           (IFILEMGR_Remove( pMe->mFTP.pIFileMgr, DIR_LIST_NAME ) != SUCCESS) )
      {
        MSG_ERROR( "LS_FAILED - can't remove dirList file", 0, 0, 0 );
      }
      break;
    }
    default:
      MSG_ERROR( "FTP: unexpected user event %x", dwParam, 0, 0 );
      break;
  }
}

/* ==========================================================================
FUNCTION:    BTApp_FTPConnect
DESCRIPTION: 
============================================================================ */
void BTApp_FTPConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr )
{
  int result;

  MSG_LOW( "FTP: BTApp_FTPConnect", 0, 0, 0 ); 

  MSG_FATAL("***zzg BTApp_FTPConnect Start***", 0, 0, 0);
  
  BTApp_SetBondable(pMe);  

  pMe->mFTP.remoteBDAddr = *pBDAddr;

  if (pMe->mFTP.bDoAuthenticate == FALSE )
  {
    pMe->mFTP.reg.auth         = AEEBT_FTP_AUTH_NONE;
  }
  else
  {
    pMe->mFTP.reg.auth         = AEEBT_FTP_AUTH_PASSWORD;
  }

  MSG_LOW("FTP: BTApp_FTPConnect pMe->mFTP.reg.auth = %d", pMe->mFTP.reg.auth, 0, 0);

  if ((result = IBTEXTFTP_AuthData( pMe->mFTP.po, &pMe->mFTP.reg))!= SUCCESS )
  {
      MSG_ERROR( "FTP: IBTEXTFTP_AuthData result = %d", result, 0, 0 );
      return;
  }  

  MSG_FATAL("***zzg BTApp_FTPConnect Before IBTEXTFTP_Connect***", 0, 0, 0);
  
  if (IBTEXTFTP_Connect( pMe->mFTP.po, pBDAddr, 0) != SUCCESS)
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

	MSG_FATAL("***zzg BTApp_FTPConnect IBTEXTFTP_Connect Failed!***", 0, 0, 0);
    BTApp_ClearBondable( pMe ); // no need to be bondable anymore
    BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
  }
  else
  {
  	MSG_FATAL("***zzg BTApp_FTPConnect IBTEXTFTP_Connect SUCCEED!***", 0, 0, 0);
    BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
  }
}
/* ==========================================================================
FUNCTION BTApp_EnableFTP
DESCRIPTION
============================================================================= */
boolean BTApp_EnableFTP( 
  CBTApp*  pMe, 
  boolean* pbSettingBondable, 
  boolean* pbSettingDiscoverable )
{
  boolean bFTPEnabled = FALSE;
  int result;

//  if ( pMe->mFTP.bServerEnable == TRUE )
  {
#ifdef FEATURE_BT_2_1
    if ( pMe->mFTP.bSecSelected !=FALSE ) 
    {
      IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                               AEEBT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER , 
                               pMe->mFTP.srvSecType,
                               pMe->mFTP.bAuthorize,
                               pMe->mFTP.bAuthorizeFirst );

    }
#endif //FEATURE_BT_2_1

    if ( BTApp_FTPInit( pMe ) == FALSE )
    {
      MSG_ERROR( "EnableFTP - failed to create FTP object", 0, 0, 0 );
      BTApp_FTPCleanup( pMe );
    }
    else if ( pMe->mFTP.bRegistered == FALSE )
    {
      if ( pMe->mFTP.bDoAuthenticate == FALSE )
      {
        pMe->mFTP.reg.auth         = AEEBT_FTP_AUTH_NONE;
      }
      else
      {
        pMe->mFTP.reg.auth         = AEEBT_FTP_AUTH_PASSWORD;
      }
      STRLCPY( pMe->mFTP.reg.szServiceName, "QC FTP", sizeof(pMe->mFTP.reg.szServiceName) );
      MSG_LOW( "FTP_Register() - auth=%x %d %d", pMe->mFTP.reg.auth, 
               pMe->mFTP.reg.bReadOnly, pMe->mFTP.reg.bBrowseAllowed );

      if ( *pbSettingBondable == FALSE )
      {
        BTApp_SetBondable( pMe );
        *pbSettingBondable = TRUE;
      }
      MSG_LOW( "EnableFTP - enabling FTP", 0, 0, 0 );
      if ( (result = IBTEXTFTP_Register( pMe->mFTP.po, 
                                         &pMe->mFTP.reg )) != SUCCESS )
      {
        MSG_ERROR( "FTP_Register() failed with %x", result, 0, 0 );
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      }
      else
      {
        if ( *pbSettingDiscoverable == FALSE )
        {
          IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
          *pbSettingDiscoverable = TRUE;
        }
        BTApp_ShowBusyIcon( pMe ); // wait for command done
        bFTPEnabled = TRUE;
      }
    }
  }
  return bFTPEnabled;
}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

#endif /* FEATURE_BT_EXTPF_FTP */

#endif /* FEATURE_APP_BLUETOOTH */

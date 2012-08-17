/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppOPP.c                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains OPP test feature.        */  
/*                                                                           */  
/*              (c) COPYRIGHT 2004-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppOPP.c#6 $
$DateTime: 2009/06/17 10:41:55 $
$Author: nksingh $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
06/11/09   ns   changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
02/24/09   GS   Fixed issue in clearing 'bConnecting' flag when connection fails
02/23/09   GS   Added support for cancelling OPP client connect using CLR key
01/14/09   GS   Added support for CONNECTING and file transfer progress info
                events
01/14/09   GS   Updated BTApp_OPPPull() to allow longer filenames
10/13/08   RS   Client Pull operation now passes correct file name to the 
                Interface.
05/12/08   VM   Replacing deprecated functions with BREW replacements.
04/10/08   VM   Changed the profile enabling mechanism to correct the failure
                in enabling profiles.
03/06/08   VM   Removed redundant IBTEXTRM_SetSecBySvcCls call
12/04/07   mh   Remote device address update for OPP and
                Profile configuration updates to app manager
11/01/07   ag   Merged Lisbon(BT2.1) code to the mainline
06/26/07   pn   Removed compiler warnings.
05/22/07   ns   Fixed automation failure while deregistering server when it is
                already connected
09/12/06   gs   Fixing RVCT compile issues on 7K target.
09/05/06   gs   Added check to validate filename ptr in 
                AEEBT_OPP_EVT_OBJ_PULLED event handling.
07/14/06   gs   Updated OPP Register function call.
03/22/05   sn   Updated OPP main menu options to reflect app support for Client 
                or Server one at a time.
12/28/05   sn   Added diagnostic support for OPP Client abort feature.
12/12/05   sn   Added app support to abort OPP push/pull operation.
10/25/05   ns   Fixed automation featurization problems
10/19/05   gs   Added support for storing vCard pulled from the server
                using the actual object name.
10/09/05   dc   Removed ShowMessage when pMe->mOPP.bRegistered is TRUE 
                pMe->bIsConcEventActive is TRUE. Added ability to push JPEG 
                images.
09/28/05   ns   Removed using glodal variable for storing passkey. Always 
                build menu's from main menu during automation to avoid menu 
                stack overflow.
07/27/05   ns   Added support to enable OPP server during startup
07/12/05   dc   Added concurrency test automation interface for OPP Pull
06/27/05   ns   Added concurrency test automation interface for OPP Push 
                operation
05/26/05   ns   OPP Server will keep the name of the received files as it is.
05/23/05   ns   Automated bonding procedure for OPP server.
04/05/05   dc   Added Optional field to BTApp_DiagOPPCliConExReqType to handle
                PIN code menu during connection.
03/25/05   pn   Made use of BTApp_SetBondable() and BTApp_ClearBondable().
                Added BTApp_OPPConnect().
01/27/05   pn   Exchange now continues even if Push fails.
01/18/04   dc   Added the OPP Object Selection menu to the UI
                Updated the BTApp_DiagOPPClientPush IDIAG function
                to select and perform operations on this object menu.
11/23/04   gr   Changed indent to 2 spaces
                Updated any comments/code to 80 cols
                Reviewed if's for braces
                Reviewed space between brackets and parameters
09/20/04   gr   Updated the Fetch and Send interface (memory efficiency)
09/14/04   gr   Updated BTApp_DiagSetSelection to BTApp_DiagSetMenuSelection
11/08/04   pn   Changed BTApp_res.h to btapp_res.h.
09/01/04   pn   Server responds properly to PULL request when no default 
                VCard is present.
08/24/04   pn   Requires server deregistration prior to connect as client.
                Correctly registers for BT notification (e.g. BT events.)
                Fixed problem of multiple discoverable mode setting.
08/12/04   pn   Uses /btapp as root directory. Server app is now discoverable.
                More debug messages.  Test VCard is created as needed.
08/13/04   gr   Added CONEX capability
07/27/04   gr   Added OPP Client connect, disconnect, push, pull
07/22/04   gr   Added OPP server Reg and Dereg Diagnostics
08/03/04   pn   Client app now presents user with a list of OBEX servers
                to connect to.
07/28/04   pn   Fixed Pull problem. Added support for Business Card Exchange
                function. Used modified Push and Pull APIs.
07/26/04   pn   Added include file BTApp_res.h
07/01/04   pn   Updated to use IBT OPP.  Exiting test menu no longer means
                quitting test.
05/25/04   pn   Server test gets cleaned up before exiting.
05/12/04   pn   Fixed screen freezing problem when server got disconnected.
04/20/04   pn   Initial Revision.

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp_priv.h"		//"BTApp.h"

#include "BTAppUtils.h"
#include "btapp_res.h"

#ifdef FEATURE_BT_EXTPF_OPP
#include "AEEBTExtOPP.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#define EVT_OPP_CONN_REQ         1
#define EVT_OPP_CONNECTED        2
#define EVT_OPP_CONN_FAILED      3
#define EVT_OPP_DISCONNECTED     4
#define EVT_OPP_REG              5
#define EVT_OPP_REG_FAILED       6
#define EVT_OPP_DEREG            7
#define EVT_OPP_DEREG_FAILED     8
#define EVT_OPP_PUSH_REQ         9
#define EVT_OPP_PULL_REQ        10
#define EVT_OPP_OBJ_PUSHED      11
#define EVT_OPP_OBJ_PUSH_FAILED 12
#define EVT_OPP_OBJ_PULLED      13
#define EVT_OPP_OBJ_PULL_FAILED 14
#define EVT_OPP_CONNECTING      15
#define EVT_OPP_PROG_INFO       16

#define DEFAULT_VCARD_NAME    BTAPP_ROOT_DIR DIRECTORY_STR "opp_vcard.vcf"
#define DEFAULT_VCAL_NAME     BTAPP_ROOT_DIR DIRECTORY_STR "opp_vcal.vcs"
#define DEFAULT_VMSG_NAME     BTAPP_ROOT_DIR DIRECTORY_STR "opp_vmsg.vmg"
#define DEFAULT_VNOTE_NAME    BTAPP_ROOT_DIR DIRECTORY_STR "opp_vnote.vnt"
#define DEFAULT_ICAL_NAME     BTAPP_ROOT_DIR DIRECTORY_STR "opp_ical.ics"
#define DEFAULT_JPEG_NAME     BTAPP_ROOT_DIR DIRECTORY_STR "opp_image.jpg"
#define REMOTE_VCARD_NAME     BTAPP_ROOT_DIR DIRECTORY_STR "their.vcf"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

char*  szServerNameOPP      = "QC OPP";

/* File Format Types */
static AECHAR VCARD[]       =   {'V','c','a','r','d', 0};
static AECHAR VCALENDAR[]   =   {'V','c','a','l','e','n','d','a','r', 0};
static AECHAR VMESSAGE[]    =   {'V','m','e','s','s','a','g','e', 0};
static AECHAR VNOTE[]       =   {'V','n','o','t','e', 0};
static AECHAR ICAL[]        =   {'I','c','a','l','e','n','d','a','r', 0};
static AECHAR JPEG[]        =   {'J','p','e','g',0};

static AEEBTProgressInfo progInfo;



//Add By zzg 2010_11_19

enum
{
	OBJ_IMAGE_JPEG_TYPE= 0,	
	OBJ_IMAGE_BMP_TYPE,
	OBJ_IMAGE_PNG_TYPE,
	OBJ_IMAGE_GIF_TYPE,
	OBJ_AUDIO_MP3_TYPE,
	OBJ_AUDIO_WAV_TYPE,
	OBJ_AUDIO_MID_TYPE,
	OBJ_AUDIO_MIDI_TYPE,
	OBJ_AUDIO_WMA_TYPE,
	OBJ_OTHER_TYPE	
};

typedef struct
{
  int 			type;
  char* 		pFileExt;
} BTObjectTypeMap;

static const BTObjectTypeMap TypeMap[] =
{	
	{OBJ_IMAGE_JPEG_TYPE,	".jpg" },
	{OBJ_IMAGE_JPEG_TYPE,	".jpe" },
	{OBJ_IMAGE_JPEG_TYPE,	".jpeg" },
	{OBJ_IMAGE_JPEG_TYPE, 	".jfif" },
	{OBJ_IMAGE_JPEG_TYPE,	".jff" },
	{OBJ_IMAGE_JPEG_TYPE,	".jft" },
	{OBJ_IMAGE_BMP_TYPE,	".bmp" },
	{OBJ_IMAGE_PNG_TYPE,	".png" },
	{OBJ_IMAGE_GIF_TYPE,	".gif" },	
	{OBJ_AUDIO_MP3_TYPE,	".mp3" },
	{OBJ_AUDIO_WAV_TYPE,	".wav" },	
	{OBJ_AUDIO_MID_TYPE,	".mid" },
	{OBJ_AUDIO_MIDI_TYPE,	".midi" },
	{OBJ_AUDIO_WMA_TYPE,	".wma" },	
	{OBJ_OTHER_TYPE,		NULL }
};

//Add End


// This will store the name of the file
static AECHAR wDefaultObjectName[ AEEBT_MAX_FILE_NAME + 1 ];

static AECHAR wRemoteVCardName[ AEEBT_MAX_FILE_NAME + 1 ];
                                
extern uint32 uBTApp_NMask;

/* Static Function Declarations */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
void BTApp_OPPConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr );
boolean BTApp_EnableOPP( CBTApp*  pMe, boolean* pbSettingBondable, boolean* pbSettingDiscoverable );


void BTApp_OPPUpdateSendingProgress( CBTApp* pMe )
{
	MSG_FATAL("***zzg OPPUpdateSendingProgress numBytes=%d, objSize=%d***", progInfo.numBytes, progInfo.objSize, 0);

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

/* ==========================================================================
FUNCTION BTApp_ProcessOPPNotifications
DESCRIPTION
============================================================================= */
void BTApp_ProcessOPPNotifications(
  CBTApp* pMe, 
  AEEEvent evt, 
  NotificationData* pData
)
{
  uint32 userEvent;

  MSG_FATAL("***zzg BTApp_ProcessOPPNotifications evt=%x***", evt, 0, 0);
  
  switch ( evt )
  {
    case AEEBT_OPP_EVT_REG_DONE:    // registration process done
    {
      if ( pData->uError == AEEBT_OPP_ERR_NONE )
      {
        userEvent = EVT_OPP_REG;
      }
      else 
      {
        userEvent = EVT_OPP_REG_FAILED;
      }
	  
      BTAPP_POST_USER_EVENT( OPP, userEvent );
      break;
    }
    case AEEBT_OPP_EVT_DEREG_DONE:  // deregistration process done
    {
      if ( pData->uError == AEEBT_OPP_ERR_NONE )
      {
        userEvent = EVT_OPP_DEREG;
      }
      else 
      {
        userEvent = EVT_OPP_DEREG_FAILED;
      }
      BTAPP_POST_USER_EVENT( OPP, userEvent );
      break;
    }
    case AEEBT_OPP_EVT_CONN_REQ:    // a client wants to connect
    {
      pMe->mOPP.remoteBDAddr = pData->bdAddr;
      BTAPP_POST_USER_EVENT( OPP, EVT_OPP_CONN_REQ );
      break;
    }
    case AEEBT_OPP_EVT_PROGRESS:    // progress evt
    {
      MSG_HIGH("BTAppOPP: Progress Evt: num_bytes = %d, obj_size = %d",
               pData->ProgressInfo.numBytes, pData->ProgressInfo.objSize, 0);

		if (IBTEXTOPP_GetProgressInfo( pMe->mOPP.po, &progInfo ) == SUCCESS)
		{
			MSG_HIGH("BTAppOPP: GetProgInfo num_bytes = %d, obj_size = %d",progInfo.numBytes, progInfo.objSize, 0);
			BTApp_OPPUpdateSendingProgress(pMe);		//Add By zzg 2010_11_19
		}
		else
		{
			MSG_FATAL("BTAppOPP: GetProgInfo failed!", 0, 0, 0);
		}
	  
      BTAPP_POST_USER_EVENT( OPP, EVT_OPP_PROG_INFO );
      break;
    }
    case AEEBT_OPP_EVT_PUSH_REQ:    // client pushing object to server
    {
      pMe->mOPP.objType = pData->OppObject.objType;
      WSTRLCPY( pMe->mOPP.wName, pData->OppObject.pwName, 
                ARR_SIZE( pMe->mOPP.wName ) );
      BTAPP_POST_USER_EVENT( OPP, EVT_OPP_PUSH_REQ );
      break;
    }
    case AEEBT_OPP_EVT_PULL_REQ:    // client pulling server's default object
    {
      BTAPP_POST_USER_EVENT( OPP, EVT_OPP_PULL_REQ );
      break;
    }
    case AEEBT_OPP_EVT_OBJ_PUSHED:  // current push operation completed
    {
	  MSG_FATAL("***zzg BTApp_ProcessOPPNotify AEEBT_OPP_EVT_OBJ_PUSHED uError=%x***", pData->uError, 0, 0);
	  
      if ( pData->uError == AEEBT_OPP_ERR_NONE )
      {
        userEvent = EVT_OPP_OBJ_PUSHED;
      }
      else 
      {
        userEvent = EVT_OPP_OBJ_PUSH_FAILED;
      }
      BTAPP_POST_USER_EVENT( OPP, userEvent );
      break;
    }
    case AEEBT_OPP_EVT_OBJ_PULLED:  // server's default business card pulled
    {
	  
	  MSG_FATAL("***zzg BTApp_ProcessOPPNotify AEEBT_OPP_EVT_OBJ_PULLED uError=%x***", pData->uError, 0, 0);
	  		
      if ( pData->OppObject.pszName ) 
      {
        STRTOWSTR( pData->OppObject.pszName, 
                   pMe->mOPP.wName, 
                   sizeof (pMe->mOPP.wName) );
      }
      if ( pData->uError == AEEBT_OPP_ERR_NONE )
      {
        userEvent = EVT_OPP_OBJ_PULLED;
      }
      else 
      {
        userEvent = EVT_OPP_OBJ_PULL_FAILED;
      }
      BTAPP_POST_USER_EVENT( OPP, userEvent );
      break;
    }
    case AEEBT_OPP_EVT_OP_ABORTED:  // current operation aborted
    {
      break;
    }
    case AEEBT_OPP_EVT_CONNECTING:   // connection is coming up
    {
      BTAPP_POST_USER_EVENT( OPP, EVT_OPP_CONNECTING );
      break;
    }
    case AEEBT_OPP_EVT_CONNECTED:   // connection is up
    {
      BTAPP_POST_USER_EVENT( OPP, EVT_OPP_CONNECTED );
      BTAppMgr_UpdateProfileType( pMe, &pMe->mOPP.remoteBDAddr, BTAPP_MGR_OPP, BTAPP_MGR_CONNECTED );
      break;
    }
    case AEEBT_OPP_EVT_CONN_FAILED: // connection could not be brought up
    {
      BTAPP_POST_USER_EVENT( OPP, EVT_OPP_CONN_FAILED );
      break;
    }
    case AEEBT_OPP_EVT_DISCONNECTED:  // connection is down
    {
      BTAPP_POST_USER_EVENT( OPP, EVT_OPP_DISCONNECTED );
      BTAppMgr_UpdateProfileType( pMe, &pMe->mOPP.remoteBDAddr, BTAPP_MGR_OPP, BTAPP_MGR_DISCONNECTED );
      break;
    }
    default:
      MSG_FATAL( "OPP - unexpected ev=%d", evt, 0, 0 );
      break;
  }
}

/* ==========================================================================
FUNCTION BTApp_OPPBuildMainMenu
DESCRIPTION
============================================================================= */
void BTApp_OPPBuildMainMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_OPP_TESTS, NULL );

  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_OPP_TESTS,
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
 
  if( pMe->mOPP.bRegistered == TRUE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OPP_SERVER, 0 );
  }
  else if( pMe->mOPP.bConnected == TRUE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OPP_CLIENT, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OPP_SERVER, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OPP_CLIENT, 0 );
  }
 

  IMENUCTL_SetBottomBarType(pMe->m_pIMenu, BTBAR_SELECT_BACK);  	//Add By zzg 2010_11_09

  // Activate menu
  PUSH_MENU( BT_APP_MENU_OPP_TESTS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

//Add By  zzg 2010_11_09
/* ==========================================================================
FUNCTION BTApp_OPPBuildSendFileMenu 
============================================================================= */
void BTApp_OPPBuildSendFileMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_OPP_TESTS, NULL );

  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_OPP,
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
  if( pMe->mOPP.bRegistered == TRUE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OPP_SERVER, 0 );
  }
  else if( pMe->mOPP.bConnected == TRUE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OPP_CLIENT, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OPP_SERVER, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_OPP_CLIENT, 0 );
  }
  
  IMENUCTL_SetBottomBarType(pMe->m_pIMenu, BTBAR_SELECT_BACK);  	//Add By zzg 2010_11_09
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_OPP_SENDFILE );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}


void BTApp_OPPBuildSettingMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->m_pShell,
                         AEE_APPSBTAPP_RES_FILE,                                
                         IDS_OPP,
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
  if( pMe->mOPP.bConnected == TRUE )
  {
    BTApp_AddMenuItem(pMe, pMe->m_pIMenu, &ai, IDS_OPP_CLIENT, 0);
  }
  else
  {
    BTApp_AddMenuItem(pMe, pMe->m_pIMenu, &ai, IDS_OPP_SERVER, 0);    
  }  
  
  IMENUCTL_SetBottomBarType(pMe->m_pIMenu, BTBAR_SELECT_BACK);  	//Add By zzg 2010_11_09
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_OPP_SETTING );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
}

//Add End

/* ==========================================================================
FUNCTION BTApp_OPPBuildServerMenu
DESCRIPTION
============================================================================= */
void BTApp_OPPBuildServerMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  if ( pMe->mOPP.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_OPP_SERVER, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof( AECHAR ) );
	
    //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );

	//Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
	{
	  	IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
	}  

	{	 
		if(pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
		}		
	}
	//Add End
  }
  else
  {
    //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_OPP_SERVER, NULL );
	//Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
	{
	  	IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
	}  

	{
		AECHAR WTitle[20] = {0};
		ISHELL_LoadResString(pMe->m_pShell,
		                     AEE_APPSBTAPP_RES_FILE,                                
		                     IDS_OPP_SERVER,
		                     WTitle,
		                     sizeof(WTitle));

		if(pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
		}
	}
	//Add End
  }

  BTApp_InitAddItem( &ai );

  MSG_FATAL("***zzg BTApp_OPPBuildServerMenu bRegistered=%d***", pMe->mOPP.bRegistered, 0, 0);

  // Add individual entries to the Menu
  if ( pMe->mOPP.bRegistered == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REGISTER, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEREGISTER, 0 );
  }

  IMENUCTL_SetBottomBarType(pMe->m_pIMenu, BTBAR_SELECT_BACK);  	//Add By zzg 2010_11_09

  // Activate menu
  PUSH_MENU( BT_APP_MENU_OPP_SERVER );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_OPPBuildClientMenu
DESCRIPTION
============================================================================= */
void BTApp_OPPBuildClientMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  if ( pMe->mOPP.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_OPP_CLIENT, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof( AECHAR ) );
    //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
    //Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
	{
	  	IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
	}  

	{	 
		if(pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
		}		
	}
	//Add End
  }
  else
  {
    //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_OPP_CLIENT, NULL );
    //Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
	{
	  	IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
	}  

	{
		AECHAR WTitle[20] = {0};
		ISHELL_LoadResString(pMe->m_pShell,
		                     AEE_APPSBTAPP_RES_FILE,                                
		                     IDS_OPP_CLIENT,
		                     WTitle,
		                     sizeof(WTitle));

		if(pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
		}
	}
	//Add End
  }

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mOPP.bConnected == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PUSH, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PULL, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BUSINESS_CARD_EXCHANGE, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
  }

  IMENUCTL_SetBottomBarType(pMe->m_pIMenu, BTBAR_SELECT_BACK);  	//Add By zzg 2010_11_09
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_OPP_CLIENT );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

  if( pMe->mOPP.bObjectTransfer )
  {
    ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, FALSE );
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

//Add By zzg 2010_11_09
/* ==========================================================================
FUNCTION BTApp_OPPBuildSendFileClientMenu
DESCRIPTION
============================================================================= */
void BTApp_OPPBuildSendFileClientMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  int 		 result;
  boolean 	 bRegistered = FALSE; 	  


  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  MSG_FATAL("***zzg BTApp_OPPBuildSendFileClientMenu bRegistered=%d***", pMe->mOPP.bRegistered, 0, 0);
  

  //Add By zzg 2010_11_22
  //If  server, change to client
  if (pMe->mOPP.bRegistered == TRUE)
  {
  	if ((result = IBTEXTOPP_Deregister(pMe->mOPP.po)) != SUCCESS)
	{
		MSG_FATAL("***zzg IBTEXTOPP_Deregister FAILED result = %x", result, 0, 0 );        
		//BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
	}
	else
	{
		bRegistered = pMe->mOPP.bRegistered; // backing up the value of mOPP.bRegistered
		pMe->mOPP.bRegistered = FALSE;
		BTApp_CheckToClearDiscoverable(pMe);
		pMe->mOPP.bRegistered =bRegistered; 

		MSG_FATAL("***zzg IBTEXTOPP_Deregister SUCCEED Deregister=%x", pMe->mOPP.bRegistered, 0, 0 );        
	}
  }  
  //Add End

  // set the title
  if ( pMe->mOPP.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_OPP_CLIENT, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof( AECHAR ) );
    //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
    //Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
	{
	  	IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
	}  

	{	 
		if(pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
		}		
	}
	//Add End
  }
  else
  {
    //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_OPP_CLIENT, NULL );
    //Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
	{
	  	IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
	}  

	{
		AECHAR WTitle[20] = {0};
		ISHELL_LoadResString(pMe->m_pShell,
		                     AEE_APPSBTAPP_RES_FILE,                                
		                     IDS_OPP_CLIENT,
		                     WTitle,
		                     sizeof(WTitle));

		if(pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
		}
	}
	//Add End
  }

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mOPP.bConnected == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PUSH, 0 );    
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
  }

  IMENUCTL_SetBottomBarType(pMe->m_pIMenu, BTBAR_SELECT_BACK);  	//Add By zzg 2010_11_09
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_OPP_SENDFILE );    
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

  if( pMe->mOPP.bObjectTransfer )
  {
    ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, FALSE );
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_OPPSettingClientMenu DESCRIPTION
============================================================================= */
void BTApp_OPPSettingClientMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

 if ( pMe->mOPP.bConnected == FALSE )
 {
 	MSG_FATAL("***zzg BTApp_OPPSettingClientMenu pMe->mOPP.bConnected == FALSE***", 0, 0, 0);
	return;
 }

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  if ( pMe->mOPP.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_OPP_CLIENT, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof( AECHAR ) );
    //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
    //Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
	{
	  	IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
	}  

	{	 
		if(pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
		}		
	}
	//Add End
  }
  else
  {
    //IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_OPP_CLIENT, NULL );
    //Add By zzg 2010_11_01
	if(pMe->m_pIAnn != NULL)
	{
	  	IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
	}  

	{
		AECHAR WTitle[20] = {0};
		ISHELL_LoadResString(pMe->m_pShell,
		                     AEE_APPSBTAPP_RES_FILE,                                
		                     IDS_OPP_CLIENT,
		                     WTitle,
		                     sizeof(WTitle));

		if(pMe->m_pIAnn != NULL)
		{
		    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
		}
	}
	//Add End
  }

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mOPP.bConnected == TRUE )  
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
  }

  IMENUCTL_SetBottomBarType(pMe->m_pIMenu, BTBAR_SELECT_BACK);  	//Add By zzg 2010_11_09
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_OPP_SETTING );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

  if( pMe->mOPP.bObjectTransfer )
  {
    ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, FALSE );
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}


//Add End

/* ==========================================================================
FUNCTION BTApp_OPPBuildFileTypeMenu
DESCRIPTION
============================================================================= */
void BTApp_OPPBuildFileTypeMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  
  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect ); 

  BTApp_InitAddItem( &ai );
/* ICAL Has no file associated a this time */
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_OPP_ICAL,
                    ICAL, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_OPP_VCAL,
                    VCALENDAR, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_OPP_VCARD,
                    VCARD, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_OPP_VNOTE,
                    VNOTE, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_OPP_VMESSAGE,
                    VMESSAGE, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_OPP_JPEG,
                    JPEG, 0 );

  IMENUCTL_SetBottomBarType(pMe->m_pIMenu, BTBAR_SELECT_BACK);  	//Add By zzg 2010_11_09
  
    /* Activate menu */
  PUSH_MENU( BT_APP_MENU_OPP_LIST_FILE_TYPES );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

  if( pMe->mOPP.bObjectTransfer )
  {
    ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, FALSE );
  }
}

/* ==========================================================================
FUNCTION BTApp_OPPCleanup
DESCRIPTION
============================================================================= */
void BTApp_OPPCleanup( CBTApp* pMe )
{  
  if (pMe->mOPP.po != NULL)
  {
    IBTEXTOPP_Release( pMe->mOPP.po );
    pMe->mOPP.po = NULL;
  }
  if (pMe->mOPP.pIFileMgr != NULL)
  {
    IFILEMGR_Release( pMe->mOPP.pIFileMgr );
    pMe->mOPP.pIFileMgr = NULL;
  }
  // unregister OPP notification
  ISHELL_RegisterNotify( pMe->m_pShell,  AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 0 );
  uBTApp_NMask &= ~NMASK_BT_OPP;
  ISHELL_RegisterNotify( pMe->m_pShell,  AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, uBTApp_NMask );
}

/* ==========================================================================
FUNCTION BTApp_OPPCreateVCard
DESCRIPTION
============================================================================= */
static void BTApp_OPPCreateVCard( CBTApp* pMe )
{
  IFile* pIFile;
  char   szBuf[ 100 ];

  if ( IFILEMGR_Test( pMe->mOPP.pIFileMgr, DEFAULT_VCARD_NAME ) != SUCCESS )
  {
    pIFile = IFILEMGR_OpenFile( pMe->mOPP.pIFileMgr, DEFAULT_VCARD_NAME, 
                                _OFM_CREATE );

    if ( pIFile == NULL )
    {
      MSG_FATAL( "CreateVCard - failed to create file", 0, 0, 0 );
    }
    else
    {
      STRLCPY( szBuf, "BEGIN:VCARD\nVERSION:2.1\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "N:Tooth;Blue\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "FN:Blue Tooth\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "ORG:Qualcomm, Inc.\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "TEL;WORK;VOICE:+1 (408) 423-0356\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "TEL;FAX;VOICE:+1 (408) 557-1001\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "ADR;WORK:;;675 Campbell Technology Pkwy;"
                     "Campbell;CA;95008;USA\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "LABEL;WORK;ENCODING=QUOTED-PRINTABLE:"
                     "675 Campbell Technology Pkwy=0D=0A"
                     "Campbell, CA 95008=0D=0AUSA\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "URL;WORK:http://www.qualcomm.com\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "EMAIL;PREF;INTERNET:btooth@qualcomm.com\n", 
               sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      STRLCPY( szBuf, "END:VCARD\n", sizeof( szBuf ) );
      IFILE_Write( pIFile, szBuf, STRLEN( szBuf ) );

      IFILE_Release( pIFile );
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_OPPPull
DESCRIPTION
============================================================================= */
void BTApp_OPPPull( CBTApp* pMe )
{
  int result;
  char szVCardName[ 2*AEEBT_MAX_FILE_NAME + 1 ];
  char szName[ AEEBT_MAX_FILE_NAME + 1 ];

  int  vCardNameLen = sizeof( szVCardName );

  MSG_FATAL("***zzg BTApp_OPPPull bRegistered=%d***", pMe->mOPP.bRegistered, 0, 0);

  if (pMe->mOPP.bRegistered == TRUE)
  {
  	MSG_FATAL("***zzg BTApp_OPPPull bRegistered == TRUE***", 0, 0, 0);
	
    //STRLCPY(szVCardName, BTAPP_ROOT_DIR, sizeof(szVCardName));  
    //STRLCAT(szVCardName, DIRECTORY_STR, sizeof(szVCardName));
	//WSTRTOSTR(pMe->mOPP.wName, szName, sizeof(szName));

    ///*
    //Add By zzg 2010_11_19
	if (IFILEMGR_Test(pMe->mOPP.pIFileMgr, AEEFS_CARD0_DIR)==SUCCESS)		//Have  T Card
	{   
		uint8 i = 0;
		AECHAR *pExt = NULL;
		char suffix[6] = {0};	
		boolean bPicture = FALSE;
		boolean bMusic = FALSE;

		pExt = WSTRRCHR(pMe->mOPP.wName, L'.' );

		if ( pExt != NULL )
		{		
			uint8 len = WSTRLEN(pExt);                         
			MEMSET((void*)(suffix), 0x0, sizeof(suffix));                       
			WSTRTOUTF8( (pExt), len, (uint8*)(suffix), sizeof(suffix));   

			for (i = 0; i < (sizeof(TypeMap) / sizeof(BTObjectTypeMap)); i++)
			{
				  if (STRICMP(suffix, TypeMap[i].pFileExt) == 0)
			      {
					   switch(TypeMap[i].type)
					   {
					   	 case OBJ_IMAGE_JPEG_TYPE:
						 case OBJ_IMAGE_BMP_TYPE:
						 case OBJ_IMAGE_PNG_TYPE:
						 case OBJ_IMAGE_GIF_TYPE:							
						 {
						 	bPicture = TRUE;
							break;
						 }
						 case OBJ_AUDIO_MP3_TYPE:
						 case OBJ_AUDIO_WAV_TYPE:
						 case OBJ_AUDIO_MID_TYPE:
						 case OBJ_AUDIO_MIDI_TYPE:
						 case OBJ_AUDIO_WMA_TYPE:
						 {
						 	bMusic = TRUE;
							break;
						 }	
						 default:
						 {
							break;
						 }				
					   }
							
					   break;
			      }
			}
				
		}

	   
		if (TRUE == bPicture)	//Pictures
		{			
			MSG_FATAL("***zzg BTApp_OPPPull TRUE == bPicture***", 0, 0, 0);
			
			STRLCPY(szVCardName, BTAPP_PICTURE_DIR, sizeof(szVCardName)); 
			STRLCAT(szVCardName, DIRECTORY_STR, sizeof(szVCardName));
			WSTRTOSTR(pMe->mOPP.wName, szName, sizeof(szName));
		}
		else if (TRUE == bMusic)	//Music
		{		
			MSG_FATAL("***zzg BTApp_OPPPull TRUE == bMusic***", 0, 0, 0);
			
			STRLCPY(szVCardName, BTAPP_MUSIC_DIR, sizeof(szVCardName));     	
		    STRLCAT(szVCardName, DIRECTORY_STR, sizeof(szVCardName));
			WSTRTOSTR(pMe->mOPP.wName, szName, sizeof(szName));
		}
		else	//other
		{
			STRLCPY(szVCardName, BTAPP_OTHER_DIR, sizeof(szVCardName));  	
		    WSTRTOSTR(pMe->mOPP.wName, szName, sizeof(szName));
		}
	}
	else
	{	 
		MSG_FATAL("***zzg Tcard not exist, OPPPull File Failed!***", 0, 0, 0);
	}
	//Add End
	//*/
	
    if ((STRLEN(szVCardName) + STRLEN(szName)) > AEEBT_MAX_FILE_NAME*2)
    {
      MSG_FATAL( "File / Folder name exceeds max length", 0, 0, 0 );
      BTApp_ShowMessage( pMe, IDS_MSG_OBJ_PULL_FAILED, pMe->mOPP.wName, 3 );
      return;
    }
	
    STRLCAT( szVCardName, szName, sizeof( szVCardName ) );
  }
  else
  {
    STRLCPY( szName, REMOTE_VCARD_NAME, sizeof( szName ) );
    IFILEMGR_ResolvePath( pMe->mOPP.pIFileMgr, 
                          szName, 
                          szVCardName, 
                          &vCardNameLen );
  }
  
  // remove object if exists
  if ( (IFILEMGR_Test( pMe->mOPP.pIFileMgr, szVCardName ) == SUCCESS) &&
       ((result = IFILEMGR_Remove( pMe->mOPP.pIFileMgr, 
                                   szVCardName )) != SUCCESS) )
  {
    MSG_FATAL( "OPP_Pull() failed to remove file r=%x", result, 0, 0 );
  }

  // convert string to wname
  STRTOWSTR( szVCardName, wRemoteVCardName, sizeof( wRemoteVCardName ) );

  if ( (result = IBTEXTOPP_Pull( pMe->mOPP.po, wRemoteVCardName )) != SUCCESS )
  {
    MSG_FATAL( "OPP_Pull() failed r=%x", result, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_MSG_OBJ_PULL_FAILED, wRemoteVCardName, 3 );
  }
  else
  {
    if ( pMe->mOPP.bRegistered != TRUE ) // client?
    {
      pMe->mOPP.bObjectTransfer = TRUE;
      ShowBusyIcon(pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, FALSE ); // wait for pull confirm
    }
	else	//server
	{
		MSG_FATAL("***pMe->mOPP.bRegistered == TRUE***", 0, 0, 0);
	}
  }
}

/* ==========================================================================
FUNCTION BTApp_OPPPush
DESCRIPTION
============================================================================= */
void BTApp_OPPPush( CBTApp* pMe, AEEBTObjectType objType )
{
  int result = SUCCESS;
  
  const char * pszName = NULL;  
  
  AECHAR* pwName = wDefaultObjectName;
  
  if( objType == AEEBT_OPP_VCARD )
  {
    pszName = DEFAULT_VCARD_NAME;
  }
  else if( objType == AEEBT_OPP_VCAL )
  {
    pszName = DEFAULT_VCAL_NAME;
  }
  else if( objType == AEEBT_OPP_VNOTE )
  {
    pszName = DEFAULT_VNOTE_NAME;
  }
  else if( objType == AEEBT_OPP_VMESSAGE )
  {
    pszName = DEFAULT_VMSG_NAME;
  }
  else if( objType == AEEBT_OPP_ICAL )
  {
    pszName = DEFAULT_ICAL_NAME;
  }
  else if( objType == AEEBT_OPP_JPEG )
  {
    pszName = DEFAULT_JPEG_NAME;
  }

  STRTOWSTR( pszName, wDefaultObjectName, 
            sizeof( wDefaultObjectName ) );

  if ( IFILEMGR_Test( pMe->mOPP.pIFileMgr, pszName ) != SUCCESS )
  {
    if ( pMe->mOPP.bRegistered != FALSE ) // server?
    {     
      pwName = NULL;
    }
    else
    {
      BTApp_ShowMessage( pMe, IDS_MSG_OBJ_NOT_FOUND, wDefaultObjectName, 0 );
      result = EFAILED;
    }
  }
  
  if ( result == SUCCESS )
  {  	
    result = IBTEXTOPP_Push( pMe->mOPP.po, pwName, objType );
	
    if ( pMe->mOPP.bRegistered == FALSE ) // client?
    {    	
      if ( result == SUCCESS )
      {       
        pMe->mOPP.bObjectTransfer = TRUE;
        ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, 
                      FALSE );
      }
      else
      {
		MSG_FATAL( "OPP_Push() failed with %x", result, 0, 0 );

		//Add By zzg 2011_12_29	
	   	{
			char tempstr[256];

			WSTRTOSTR(pwName, tempstr, 256);				
			
			UTF8TOWSTR((byte*)tempstr, 256, pwName, 	(256)*sizeof(AECHAR));
		}	
		//Add End	
				
        BTApp_ShowMessage( pMe, IDS_MSG_OBJ_PUSH_FAILED, pwName, 4 );
      }
    }
  }  
}


//Add By zzg 2010_11_10
/* ==========================================================================
FUNCTION BTApp_OPPPushEx
============================================================================= */
void BTApp_OPPPushEx( CBTApp* pMe, char* filepath, AEEBTObjectType objType )
{
  int result = SUCCESS;
  int i=0;

  int tempresult = SUCCESS; 
  
  const char * pszName = filepath;
  
  AECHAR* pwName = wDefaultObjectName;  

  //gb2312_to_ucs2((unsigned char *)pszName, STRLEN(pszName), wDefaultObjectName, sizeof(wDefaultObjectName));	//Add By zzg 2011_12_29
  STRTOWSTR(pszName, wDefaultObjectName, sizeof(wDefaultObjectName));

  MSG_FATAL("***zzg BTApp_OPPPushEx Start***", 0, 0, 0);
  
  DBGPRINTF("***zzg BTApp_OPPPushEx filepath=%s, pszName=%s***", filepath, pszName);

  tempresult = IFILEMGR_Test( pMe->mOPP.pIFileMgr, pszName );

  MSG_FATAL("***zzg BTApp_OPPPushEx IFILEMGR_Test result=%d, bRegistered=%d***", tempresult, pMe->mOPP.bRegistered, 0);

  if (tempresult != SUCCESS )
  {	
    if ( pMe->mOPP.bRegistered != FALSE ) // server?
    {
      pwName = NULL;
    }
    else
    {
      BTApp_ShowMessage( pMe, IDS_MSG_OBJ_NOT_FOUND, wDefaultObjectName, 0 );
      result = EFAILED;
    }
  }  
  
  MSG_FATAL("***zzg BTApp_OPPPushEx Start result=%x, bRegistered=%x***", result, pMe->mOPP.bRegistered, 0);
    
  if ( result == SUCCESS )
  {
    result = IBTEXTOPP_Push( pMe->mOPP.po, pwName, objType );
	
    //if ( pMe->mOPP.bRegistered == FALSE ) // client?
    {
      if ( result == SUCCESS )
      {
        pMe->mOPP.bObjectTransfer = TRUE;
        //ShowBusyIcon( pMe->m_pShell, pMe->m_pIDisplay, &pMe->m_rect, FALSE );
      }
      else
      {
		MSG_FATAL( "BTApp_OPPPExush() failed with %x", result, 0, 0 );

		//Add By zzg 2011_12_29	
	   	{
			char tempstr[256];

			WSTRTOSTR(pwName, tempstr, 256);				
			
			UTF8TOWSTR((byte*)tempstr, 256, pwName, 	(256)*sizeof(AECHAR));	
		}	
		//Add End	
		
        BTApp_ShowMessage( pMe, IDS_MSG_OBJ_PUSH_FAILED, pwName, 4 );   
      }
    }
  } 
}

//Add End

/* ==========================================================================
FUNCTION BTApp_OPPInit
DESCRIPTION
============================================================================= */
boolean BTApp_OPPInit( CBTApp* pMe )
{
  static boolean init_done = FALSE;
  uint32 uNMask = NMASK_BT_OPP | (((uint32)(NOTIFIER_VAL_ANY)) << 16);

  if ( init_done == FALSE )
  {
    if ( (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BLUETOOTH_OPP, 
                                 (void**)&pMe->mOPP.po ) == SUCCESS) &&
         (ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_FILEMGR, 
                                 (void **)&pMe->mOPP.pIFileMgr ) == SUCCESS) &&
         (ISHELL_RegisterNotify( pMe->m_pShell,  AEECLSID_BLUETOOTH_APP,
                                 AEECLSID_BLUETOOTH_NOTIFIER, 
                                 uNMask ) == SUCCESS) )
    {
      STRTOWSTR( DEFAULT_VCARD_NAME, wDefaultObjectName, 
                 sizeof( wDefaultObjectName ) );
      STRTOWSTR( REMOTE_VCARD_NAME, wRemoteVCardName, 
                 sizeof( wRemoteVCardName ) );
      uBTApp_NMask |= NMASK_BT_OPP;
      init_done = TRUE;

      pMe->mOPP.bObjectTransfer = FALSE;	  
      pMe->mOPP.bPushFileReq = FALSE;
    }
  }
  return init_done;
}
/* ==========================================================================
FUNCTION BTApp_OPPBuildMenu
DESCRIPTION
============================================================================= */
boolean BTApp_OPPBuildMenu( CBTApp* pMe, BTAppMenuType menu )
{
  boolean built = TRUE;
  switch ( menu)
  {
    //Add By zzg 2010_11_09
    case BT_APP_MENU_OPP_SENDFILE:
	{		
		if (BTApp_OPPInit(pMe) != FALSE)
		{
			MSG_FATAL("***zzg BTApp_OPPBuildMenu BT_APP_MENU_OPP_SENDFILE***", 0, 0, 0);
			BTApp_OPPBuildSendFileClientMenu(pMe);	//直接用CLIENT(如果是SERVER状态，则切换)
			
			//BTApp_OPPBuildSendFileMenu(pMe);
		}
		else
		{
			MSG_FATAL( "OPPBuildMenu - failed to create OPP object", 0, 0, 0 );
			BTApp_OPPCleanup( pMe );
			built = FALSE;
		}
		break;
	}
    //Add End
    
    //Add By zzg 2010_11_17
    case BT_APP_MENU_OPP_SETTING:
    {
		if (pMe->mOPP.po != NULL)		//Check if OPP object has been created
		{
	    	BTApp_OPPBuildSettingMenu(pMe);
		}
		else
		{
			if (BTApp_OPPInit(pMe) != FALSE)
			{
				BTApp_OPPBuildSettingMenu(pMe);
			}
			else
			{
				MSG_FATAL("OPPBuildMenu - failed to create OPP object", 0, 0, 0);
				BTApp_OPPCleanup(pMe);
				built = FALSE;
			}	
		}
		break;
	}
	//Add End
	
    case BT_APP_MENU_OPP_TESTS:
    {
      if ( BTApp_OPPInit( pMe ) != FALSE )
      {
        BTApp_OPPBuildMainMenu( pMe );
      }
      else
      {
        MSG_FATAL( "OPPBuildMenu - failed to create OPP object", 0, 0, 0 );
        BTApp_OPPCleanup( pMe );
        built = FALSE;
      }
      break;
    }
    case BT_APP_MENU_OPP_SERVER:
      BTApp_OPPBuildServerMenu( pMe );
      break;
    case BT_APP_MENU_OPP_CLIENT:
      BTApp_OPPBuildClientMenu( pMe );
      break;
    case BT_APP_MENU_OPP_LIST_FILE_TYPES:
      BTApp_OPPBuildFileTypeMenu( pMe );
      break;
    default:
      built = FALSE;
      break;
  }
  return built;
}

/* ==========================================================================
FUNCTION BTApp_OPPHandleSelection
DESCRIPTION
============================================================================= */
boolean BTApp_OPPHandleSelection( CBTApp* pMe, uint16 sel )
{
  int result;
  boolean key_handled = TRUE;
  boolean bRegistered = FALSE; 
  switch ( sel )
  {
    case IDS_OPP_SERVER:
    {
      BTApp_OPPBuildServerMenu( pMe );
      break;
    }
    case IDS_OPP_CLIENT:
    {
      BTApp_OPPBuildClientMenu( pMe );
      break;
    }
    case IDS_REGISTER:
    {
      BTApp_SetBondable( pMe );

      if ( (result = IBTEXTOPP_Register( pMe->mOPP.po, 
                                         AEEBT_OPP_FORMAT_ALL, 
                                         szServerNameOPP )) != SUCCESS )
      {
        MSG_FATAL( "OPP_Register() failed with %x", result, 0, 0 );
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
      break;
    }
    case IDS_DEREGISTER:
    {
      if ( (result = IBTEXTOPP_Deregister( pMe->mOPP.po )) != SUCCESS )
      {
        MSG_FATAL( "OPP_Deregister() failed with %x", result, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      }
      else
      {      
        bRegistered = pMe->mOPP.bRegistered; // backing up the value of mOPP.bRegistered
        pMe->mOPP.bRegistered = FALSE;
        BTApp_CheckToClearDiscoverable( pMe );
        pMe->mOPP.bRegistered =bRegistered;		
        BTApp_ShowBusyIcon( pMe ); // wait for command done
      }
      break;
    }
    case IDS_CONNECT:
    {
	  
      if ( pMe->mOPP.bRegistered == FALSE )
      {
        key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_OBEX_SERVERS );
      }
      else
      {
        //BTApp_ShowMessage( pMe, IDS_MSG_DEREG_SVR_FIRST, NULL, 0 );
      }
      break;
    }
    case IDS_PUSH:
    {
      BTApp_BuildMenu( pMe, BT_APP_MENU_OPP_LIST_FILE_TYPES );
      break;
    }
    case IDS_PULL:
    {
      BTApp_OPPPull( pMe );
      break;
    }
    case IDS_BUSINESS_CARD_EXCHANGE:
    {
      pMe->mOPP.bExchanging = TRUE;
      BTApp_OPPCreateVCard( pMe );
      BTApp_OPPPush( pMe, AEEBT_OPP_VCARD );
      break;
    }
    case IDS_DISCONNECT:
    {
      if ( IBTEXTOPP_Disconnect( pMe->mOPP.po ) != SUCCESS )
      {
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

//Add By zzg 2010_11_09
/* ==========================================================================
FUNCTION BTApp_OPPHandleSendFileSelection
============================================================================= */
boolean BTApp_OPPHandleSendFileSelection( CBTApp* pMe, uint16 sel )
{
  int result;
  boolean key_handled = TRUE;
  boolean bRegistered = FALSE; 

  
  switch ( sel )
  {
    case IDS_OPP_SERVER:
    {
      BTApp_OPPBuildServerMenu( pMe );		//Server keep the code;
      break;
    }
    case IDS_OPP_CLIENT:
    {
		MSG_FATAL("***zzg BTApp_OPPHandleSendFileSelection IDS_OPP_CLIENT***",0,0,0);
      	BTApp_OPPBuildSendFileClientMenu( pMe );		//client should change for sendfile from Tcard
      	break;
    }
    case IDS_REGISTER:
    {
      BTApp_SetBondable( pMe );

      if ( (result = IBTEXTOPP_Register( pMe->mOPP.po, 
                                         AEEBT_OPP_FORMAT_ALL, 
                                         szServerNameOPP )) != SUCCESS )
      {
        MSG_FATAL( "OPP_Register() failed with %x", result, 0, 0 );
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
      break;
    }
    case IDS_DEREGISTER:
    {
      if ( (result = IBTEXTOPP_Deregister( pMe->mOPP.po )) != SUCCESS )
      {
        MSG_FATAL( "OPP_Deregister() failed with %x", result, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      }
      else
      {
        bRegistered = pMe->mOPP.bRegistered; // backing up the value of mOPP.bRegistered
        pMe->mOPP.bRegistered = FALSE;
        BTApp_CheckToClearDiscoverable( pMe );
        pMe->mOPP.bRegistered =bRegistered;		
        BTApp_ShowBusyIcon( pMe ); // wait for command done
      }
      break;
    }
    case IDS_CONNECT:
    {
		
      if ( pMe->mOPP.bRegistered == FALSE )
      {
        key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_OBEX_SERVERS );
      }
      else
      {
        //BTApp_ShowMessage( pMe, IDS_MSG_DEREG_SVR_FIRST, NULL, 0 );
      }
      break;
    }
    case IDS_PUSH:
    {		
      //BTApp_BuildMenu( pMe, BT_APP_MENU_OPP_LIST_FILE_TYPES );		
	  BTApp_OPPPushEx(pMe, pMe->m_pfilepath, AEEBT_OPP_UNKNOWN_TYPE);	
	  break;
    }

	/*
    case IDS_PULL:
    {
      BTApp_OPPPull( pMe );
      break;
    }
    */
    
    case IDS_DISCONNECT:
    {
      if (IBTEXTOPP_Disconnect( pMe->mOPP.po ) != SUCCESS)
      {
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

//Add By zzg 2010_11_17
boolean BTApp_OPPHandleSettingSelection( CBTApp* pMe, uint16 sel )
{
  int result;
  boolean key_handled = TRUE;
  boolean bRegistered = FALSE; 

  
  switch ( sel )
  { 
    case IDS_DISCONNECT:
    {
      if (IBTEXTOPP_Disconnect( pMe->mOPP.po ) != SUCCESS)
      {
        BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
      }
	  else
	  {
		BTApp_ShowMessage( pMe, IDS_DISCONNECT_CLIENT, NULL, 3 );
	  }
      break;
    }
	  
    case IDS_OPP_SERVER:
    {
      BTApp_OPPBuildServerMenu( pMe );		//Server keep the code;
      break;
    }    
	
	case IDS_OPP_CLIENT:
    {
      	BTApp_OPPSettingClientMenu( pMe );		//client should change for sendfile from Tcard
      	break;
    }
    case IDS_REGISTER:
    {			
      BTApp_SetBondable( pMe );

      if ( (result = IBTEXTOPP_Register( pMe->mOPP.po, 
                                         AEEBT_OPP_FORMAT_ALL, 
                                         szServerNameOPP )) != SUCCESS )
      {
      
        MSG_FATAL( "OPP_Register() failed with %x", result, 0, 0 );
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
      break;
    }
    case IDS_DEREGISTER:
    {
      if ( (result = IBTEXTOPP_Deregister( pMe->mOPP.po )) != SUCCESS )
      {
        MSG_FATAL( "OPP_Deregister() failed with %x", result, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      }
      else
      {
        bRegistered = pMe->mOPP.bRegistered; // backing up the value of mOPP.bRegistered
        pMe->mOPP.bRegistered = FALSE;
        BTApp_CheckToClearDiscoverable( pMe );
        pMe->mOPP.bRegistered =bRegistered;		
        BTApp_ShowBusyIcon( pMe ); // wait for command done
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

//Add End


/* ==========================================================================
FUNCTION BTApp_HandleOPPSendFileMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleOPPSendFileMenu( CBTApp* pMe, uint16 key )
{
  uint16 selection;
  boolean ev_processed = TRUE;


  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    //ev_processed = BTApp_OPPHandleSelection( pMe, selection );
    ev_processed = BTApp_OPPHandleSendFileSelection( pMe, selection );
  }
  else
  {
    switch ( key)
    {
      case AVK_INFO:
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );

		
        //ev_processed = BTApp_OPPHandleSelection( pMe, selection );
        ev_processed = BTApp_OPPHandleSendFileSelection( pMe, selection );	//Modify by zzg 2010_11_10
        
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
FUNCTION BTApp_HandleOPPSendFileMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleOPPSettingMenu( CBTApp* pMe, uint16 key )
{
  uint16 selection;
  boolean ev_processed = TRUE;


  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_OPPHandleSettingSelection( pMe, selection );
  }
  else
  {
    switch ( key)
    {
      case AVK_INFO:
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );       
        ev_processed = BTApp_OPPHandleSettingSelection( pMe, selection );	
        
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



//Add End

/* ==========================================================================
FUNCTION BTApp_HandleOPPMainMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleOPPMainMenu( CBTApp* pMe, uint16 key )
{
  uint16 selection;
  boolean ev_processed = TRUE;


  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_OPPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key)
    {
      case AVK_INFO:		//Add By zzg 2010_11_16
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_OPPHandleSelection( pMe, selection );
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
FUNCTION BTApp_HandleOPPServerMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleOPPServerMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlOPPServer - k=0x%x", key, 0, 0 );


  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_OPPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_INFO:		//Add By zzg 2010_11_16
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_OPPHandleSelection( pMe, selection );
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
FUNCTION BTApp_HandleOPPClientMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleOPPClientMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;
  int     result = 0;

  MSG_MED( "HndlOPPClient - k=0x%x", key, 0, 0 );


  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_OPPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_INFO:		//Add By zzg 2010_11_16
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_OPPHandleSelection( pMe, selection );
        break;
      }
      case AVK_CLR:
      {
        if( pMe->mOPP.bObjectTransfer )
        {
          result = IBTEXTOPP_Abort( pMe->mOPP.po );
          if( result != SUCCESS )
          {
            MSG_MED( "Abort failed %d", result, 0, 0 );
            BTApp_ShowMessage( pMe, IDS_MSG_ABORT_FAILED, 0, 2 );
          }
          else
          {
            MSG_MED( "Aborting", 0, 0, 0 );
          }
        }
        else if ( pMe->mOPP.bConnecting ) 
        {
          if ( IBTEXTOPP_Disconnect( pMe->mOPP.po ) != SUCCESS )
          {
            BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
          }
          else
          {
            MSG_MED( "Cancelling Connect", 0, 0, 0 );
            pMe->mOPP.bConnecting = FALSE;
          }
        }
        else
        {
          if ( pMe->mOPP.bEndingTest == FALSE)
          {
            BTApp_HandleClearKey( pMe );
          }
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
FUNCTION BTApp_HandleOPPFileTypesMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleOPPFileTypesMenu( CBTApp* pMe, uint16 key )
{
  uint16 selection;
  boolean ev_processed = TRUE;
  int     result = 0;

 
  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key ) ) != 0 )
  {
    ev_processed = BTApp_OPPHandleSelection( pMe, selection );
  }
  else
  {
    switch( key )
    {
      case AVK_INFO:		//Add By zzg 2010_11_16
      case AVK_SELECT:
      {
        /* Create a Vcard/Vms/Vnote/Vcal depending on the selection and push the corresponding file type */

        pMe->mOPP.objType = IMENUCTL_GetSel( pMe->m_pIMenu );
        BTApp_OPPCreateVCard( pMe );
        BTApp_OPPPush( pMe, pMe->mOPP.objType );
        break;
      }
      case AVK_CLR:
      {
        if( pMe->mOPP.bObjectTransfer )
        {
          result = IBTEXTOPP_Abort( pMe->mOPP.po );
          if( result != SUCCESS )
          {
            MSG_MED( "Abort failed %d", result, 0, 0 );
            BTApp_ShowMessage( pMe, IDS_MSG_ABORT_FAILED, 0, 2 );
          }
          else
          {
            MSG_MED( "Aborting", 0, 0, 0 );
          }
        }
        else
        {
          BTApp_HandleClearKey( pMe );
        }
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
FUNCTION BTApp_OPPHandleMenus
DESCRIPTION
============================================================================= */
boolean BTApp_OPPHandleMenus( CBTApp* pMe, uint16 key, BTAppMenuType menu )
{
  boolean handled = TRUE;

  
  switch ( menu )
  {
    //Add By zzg 2010_11_09
  	case BT_APP_MENU_OPP_SENDFILE:
	  handled = BTApp_HandleOPPSendFileMenu( pMe, key );
	  break;
	//Add End	
	
	//Add By zzg 2010_11_17
	case BT_APP_MENU_OPP_SETTING:	
	  handled = BTApp_HandleOPPSettingMenu( pMe, key );
	  break;
	//Add End
		
    case BT_APP_MENU_OPP_TESTS:
      handled = BTApp_HandleOPPMainMenu( pMe, key );
      break;
    case BT_APP_MENU_OPP_SERVER:
      handled = BTApp_HandleOPPServerMenu( pMe, key );
      break;
    case BT_APP_MENU_OPP_CLIENT:
      handled = BTApp_HandleOPPClientMenu( pMe, key );
      break;
    case BT_APP_MENU_OPP_LIST_FILE_TYPES:
      handled = BTApp_HandleOPPFileTypesMenu( pMe, key );
      break;
    default:
      MSG_FATAL(" OPP: unexpected menu type %x", menu, 0, 0 );
      handled = FALSE;
      break;
  }
  return handled;
}

/* ==========================================================================
FUNCTION BTApp_OPPHandleUserEvents
DESCRIPTION
============================================================================= */
void BTApp_OPPHandleUserEvents( CBTApp* pMe, uint32 dwParam )
{
	uint16 msgID;

	MSG_FATAL("***zzg BTApp_OPPHandle dwParam=%d***", dwParam, 0, 0);

	switch (dwParam)
	{
		case EVT_OPP_CONN_REQ:
			IBTEXTOPP_AcceptConnection(pMe->mOPP.po, &pMe->mOPP.remoteBDAddr, TRUE, TRUE);
		  	break;

	    //Server send file to Client
		case EVT_OPP_PUSH_REQ:
			BTApp_OPPPull( pMe );
			break;

		case EVT_OPP_PULL_REQ:
		{	  
			BTApp_OPPPush(pMe, AEEBT_OPP_VCARD);	  
			
			break;
		}

		case EVT_OPP_PROG_INFO:
		{
			pMe->bUpdateProgress	= TRUE;	//Add By zzg 2010_11_27

			if (IBTEXTOPP_GetProgressInfo( pMe->mOPP.po, &progInfo ) == SUCCESS)
			{
				MSG_HIGH("BTAppOPP: GetProgInfo num_bytes = %d, obj_size = %d",progInfo.numBytes, progInfo.objSize, 0);				
			}
			else
			{
				MSG_FATAL("BTAppOPP: GetProgInfo failed!", 0, 0, 0);
			}			
			break;
		}

		case EVT_OPP_REG:	   
		{
			pMe->mOPP.bRegistered  = TRUE;	
			
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
            if(pMe->mOPP.bPushFileReq)
            {
                pMe->mOPP.bPushFileReq = FALSE;
                break;
            }
            
			if(pMe->mEnablingType != BTAPP_ENABLING_NONE)
			{
				pMe->mEnablingType++;
				BTApp_EnableBT(pMe);	//Del By zzg 2010_11_18
			}
			else
			{
				BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_DONE, NULL, 2 );
			}
			break;
		}

		case EVT_OPP_REG_FAILED:
		{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
			BTApp_ClearBondable( pMe ); // no need to be bondable anymore
			if(pMe->mOPP.bPushFileReq)
            {
                break;
            }
			if (pMe->mEnablingType != BTAPP_ENABLING_NONE)
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

		case EVT_OPP_DEREG:
		{
			//BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_DONE, NULL, 2 ); //Del by zzg 2010_11_20      
			pMe->mOPP.bRegistered = FALSE;		
			BTApp_ClearBondable( pMe ); // no need to be bondable anymore
			
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
            if(pMe->mOPP.bPushFileReq)
            {
                BTApp_OPPConnect(pMe,&pMe->mOPP.remoteBDAddr);
            }
			break;
		}

		case EVT_OPP_DEREG_FAILED:
		{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
			BTApp_ShowMessage(pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3);
			break;
		}
		case EVT_OPP_CONNECTED:
		{
			MSG_FATAL("***zzg BTApp_OppHandleUserEvent EVT_OPP_CONNECTED bRegistered=%d***", pMe->mOPP.bRegistered, 0, 0);

			pMe->mOPP.bConnected = TRUE;
			pMe->mOPP.bConnecting = FALSE;
			BTApp_ClearBondable( pMe ); // no need to be bondable anymore
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
			//BTApp_BuildTopMenu( pMe ); // rebuild menu to show 'C'

			//Add By zzg 2011_01_22
			if (pMe->m_pActiveDlgID == IDD_BT_OBEX_LIST_SERVERS)	//When Push Files, Connect first		
			{
				pMe->m_eDlgRet = DLGRET_BT_SEND_FILE; 		
				(void) ISHELL_EndDialog(pMe->m_pShell);
			}
			//Add End

			break;
		}

		case EVT_OPP_CONNECTING:
		{
			pMe->mOPP.bConnecting = TRUE;
		 	break;
		}

		case EVT_OPP_DISCONNECTED:	  
		{
			pMe->mOPP.bConnected = FALSE;
			pMe->mOPP.bConnecting = FALSE;
			pMe->mOPP.bObjectTransfer = FALSE;
			
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 

			if (pMe->mOPP.bEndingTest != FALSE)
			{
				pMe->mOPP.bEndingTest = FALSE;
				BTApp_HandleClearKey(pMe);
			}
			else
			{
				BTApp_BuildTopMenu(pMe); // rebuild menu to hide 'C'
			}
            
            if(pMe->mOPP.bPushFileReq)
            {
                boolean      bSettingBondable = FALSE;
                boolean      bSettingDiscoverable = FALSE;
                BTApp_EnableOPP(pMe, &bSettingBondable, &bSettingDiscoverable);
            }
			break;
		}

		case EVT_OPP_CONN_FAILED:
		{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 

			BTApp_ClearBondable(pMe); // no need to be bondable anymore
			BTApp_ShowMessage(pMe, IDS_MSG_CONN_FAILED, NULL, 3);
			pMe->mOPP.bConnecting = FALSE;
            if(pMe->mOPP.bPushFileReq)
            {
                boolean      bSettingBondable = FALSE;
                boolean      bSettingDiscoverable = FALSE;
                BTApp_EnableOPP(pMe, &bSettingBondable, &bSettingDiscoverable);
            }
			break;
		}
		case EVT_OPP_OBJ_PUSHED:
		{
			MSG_FATAL("***zzg BTApp_OPPHandle EVT_OPP_OBJ_PUSHED bRegistered=%d***", pMe->mOPP.bRegistered, 0, 0);

			if ( pMe->mOPP.bRegistered == FALSE )
			{
				msgID = IDS_MSG_OBJ_PUSHED;
				pMe->mOPP.bObjectTransfer = FALSE;
			}
			else
			{
				msgID = IDS_MSG_OBJ_SENT;
			}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 

			//if ( pMe->mOPP.bRegistered != TRUE )
			{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
				{
					pMe->bUpdateProgress	= FALSE;	//Add By zzg 2010_11_27	

					//Add By zzg 2011_12_29	
				   	{
						char tempstr[256];

						WSTRTOSTR(wDefaultObjectName, tempstr, 256);				
						
						UTF8TOWSTR((byte*)tempstr, 256, wDefaultObjectName, (256)*sizeof(AECHAR));	

						DBGPRINTF("***zzg IMenuCtl_AddItem tempstr=%s***", tempstr);
						DBGPRINTF("***zzg IMenuCtl_AddItem wDefaultObjectName=%s***", wDefaultObjectName);	
					}	
					//Add End
					
					BTApp_ShowMessage( pMe, msgID, wDefaultObjectName, 2 );		  
				}
			}

			if ( pMe->mOPP.bExchanging != FALSE )
			{
				pMe->mOPP.bExchanging = FALSE;		

				BTApp_OPPPull( pMe );
			}

			//ISHELL_CloseApplet(pMe->m_pShell, FALSE );	//Add By zzg 2010_11_27
            if(pMe->mOPP.bPushFileReq)
            {
                IBTEXTOPP_Disconnect( pMe->mOPP.po);
            }
			break;
		}
		case EVT_OPP_OBJ_PUSH_FAILED:
		{
			MSG_FATAL("***zzg BTApp_OPPHandle EVT_OPP_OBJ_PUSH_FAILED bRegistered=%d***", pMe->mOPP.bRegistered, 0, 0);

			if ( pMe->mOPP.bRegistered == FALSE )
			{
				msgID = IDS_MSG_OBJ_PUSH_FAILED;
				pMe->mOPP.bObjectTransfer = FALSE;
			}
			else
			{
				msgID = IDS_MSG_OBJ_NOT_SENT;
			}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
			if ( pMe->mOPP.bExchanging != FALSE )
			{
				pMe->mOPP.bExchanging = FALSE;		
				BTApp_OPPPull( pMe );
			}
			else
			{
				//if ( pMe->mOPP.bRegistered != TRUE  )
				{
					pMe->bUpdateProgress	= FALSE;	//Add By zzg 2010_11_27		

					//Add By zzg 2011_12_29	
				   	{
						char tempstr[256];

						WSTRTOSTR(wDefaultObjectName, tempstr, 256);				
						
						UTF8TOWSTR((byte*)tempstr, 256, wDefaultObjectName, (256)*sizeof(AECHAR));	

						DBGPRINTF("***zzg IMenuCtl_AddItem tempstr=%s***", tempstr);
						DBGPRINTF("***zzg IMenuCtl_AddItem wDefaultObjectName=%s***", wDefaultObjectName);	
					}	
					//Add End	
		
					BTApp_ShowMessage( pMe, msgID, wDefaultObjectName, 2 );
				}
			}

			//ISHELL_CloseApplet(pMe->m_pShell, FALSE );	//Add By zzg 2010_11_27
            if(pMe->mOPP.bPushFileReq)
            {
                IBTEXTOPP_Disconnect( pMe->mOPP.po);
            }
			break;
		}
		case EVT_OPP_OBJ_PULLED:
		{
			MSG_FATAL("***zzg BTApp_OPPHandle EVT_OPP_OBJ_PULLED bRegistered=%d***", pMe->mOPP.bRegistered, 0, 0);

			if ( pMe->mOPP.bRegistered == FALSE )
			{
				msgID = IDS_MSG_OBJ_PULLED;
				pMe->mOPP.bObjectTransfer = FALSE;
			}
			else
			{
				msgID = IDS_MSG_OBJ_RCVD;
			}
			
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 

			//if ( pMe->mOPP.bRegistered != TRUE )
			{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 
				{
					pMe->bUpdateProgress	= FALSE;	//Add By zzg 2010_11_27					
					BTApp_ShowMessage( pMe, msgID, pMe->mOPP.wName, 2 );
				}
			}

			//ISHELL_CloseApplet(pMe->m_pShell, FALSE );	//Add By zzg 2010_11_27

			break;
		}
		case EVT_OPP_OBJ_PULL_FAILED:
		{
			MSG_FATAL("***zzg BTApp_OPPHandle EVT_OPP_OBJ_PULL_FAILED bRegistered=%d***", pMe->mOPP.bRegistered, 0, 0);

			if ( pMe->mOPP.bRegistered == FALSE )
			{
				msgID = IDS_MSG_OBJ_PULL_FAILED;
				pMe->mOPP.bObjectTransfer = FALSE;
			}
			else
			{
				msgID = IDS_MSG_OBJ_NOT_RCVD;
			}
			
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif 

			//if ( pMe->mOPP.bRegistered != TRUE  )
			{
				pMe->bUpdateProgress	= FALSE;	//Add By zzg 2010_11_27				
				BTApp_ShowMessage( pMe, msgID, pMe->mOPP.wName, 2 );
			}	  

			//ISHELL_CloseApplet(pMe->m_pShell, FALSE );	//Add By zzg 2010_11_27
			break;
		}
		default:
		{
			MSG_FATAL(" OPP: unexpected user event %x", dwParam, 0, 0 );
		  	break;
		}
	}
}

/* ==========================================================================
FUNCTION BTApp_OPPHandleUserEvents
DESCRIPTION
============================================================================= */
void BTApp_OPPConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr )
{
  if(pMe->mOPP.bRegistered)
  {
      int result;
      pMe->mOPP.bPushFileReq = FALSE;
      if ( (result = IBTEXTOPP_Deregister( pMe->mOPP.po )) != SUCCESS )
      {
        MSG_FATAL( "OPP_Deregister() failed with %x", result, 0, 0 );
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
        return;
      }
      else
      {
        pMe->mOPP.remoteBDAddr = *pBDAddr;
        pMe->mOPP.bPushFileReq = TRUE;
        return;
      }
  }
  BTApp_SetBondable( pMe );
  pMe->mOPP.remoteBDAddr = *pBDAddr;
  pMe->mOPP.bConnecting = FALSE;
  
  MSG_FATAL("***zzg BTApp_OPPConnect***", 0, 0, 0);
  if ( IBTEXTOPP_Connect( pMe->mOPP.po, pBDAddr, 0 ) != SUCCESS )
  {
    BTApp_ClearBondable( pMe ); // no need to be bondable anymore
    BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
  }
  else
  {
    // BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
  }
}
/* ==========================================================================
FUNCTION BTApp_EnableOPP
DESCRIPTION
============================================================================= */
boolean BTApp_EnableOPP( 
  CBTApp*  pMe, 
  boolean* pbSettingBondable, 
  boolean* pbSettingDiscoverable )
{
  boolean bOPPEnabled = FALSE;
  int result;

  MSG_FATAL("***zzg BTApp_EnableOPP %d***", pMe->mOPP.bRegistered, 0, 0);

  //if ( pMe->mOPP.bServerEnable == TRUE )		//Del By zzg 2010_11_25
  {

#ifdef FEATURE_BT_2_1
    if ( pMe->mOPP.bSecSelected !=FALSE ) 
    {
        IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                                 AEEBT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH , 
                                 pMe->mOPP.srvSecType,
                                 pMe->mOPP.bAuthorize,
                                 pMe->mOPP.bAuthorizeFirst);
    }
#endif //FEATURE_BT_2_1

    if ( BTApp_OPPInit( pMe ) == FALSE )
    {
      MSG_FATAL( "EnableOPP - failed to create OPP object"
                  , 0, 0, 0 );
      BTApp_OPPCleanup( pMe );
    }
    else if ( pMe->mOPP.bRegistered == FALSE )
    {
      if ( *pbSettingBondable == FALSE )
      {
        BTApp_SetBondable( pMe );
        *pbSettingBondable = TRUE;
      }
      MSG_FATAL( "EnableOPP - enabling OPP", 0, 0, 0 );
      if ( (result = IBTEXTOPP_Register( pMe->mOPP.po, AEEBT_OPP_FORMAT_ALL, 
                                         szServerNameOPP )) != SUCCESS )
      {		
        MSG_FATAL( "OPP_Register() failed with %x", result, 0, 0 );
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
        bOPPEnabled = TRUE;
      }
    }
    else
    {
        MSG_FATAL( "EnableOPP - pMe->mOPP.bRegistered == TRUE", 0, 0, 0 );
    }
  }  

  return bOPPEnabled;
}
 

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#endif /* FEATURE_BT_EXTPF_OPP */

#endif /* FEATURE_APP_BLUETOOTH */


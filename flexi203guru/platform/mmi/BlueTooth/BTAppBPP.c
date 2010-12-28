/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppBPP.c                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains BPP test feature.        */  
/*                                                                           */  
/*              (c) COPYRIGHT 2004-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppBPP.c#3 $
$DateTime: 2009/06/17 10:41:55 $
$Author: nksingh $

when      who  what, where, why
--------  ---  ----------------------------------------------------------
06/11/09   ns    changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
06/01/09   pn    Fixed critical Lint errors.
05/12/08   VM    Replacing deprecated functions with BREW replacements.
04/10/08   VM    Changed the profile enabling mechanism to correct the failure
                 in enabling profiles.
03/06/08   VM    Removed redundant IBTEXTRM_SetSecBySvcCls call
12/04/07   mh    Remote BD address update for BPP and
                 Profile configuration updates to app manager
11/01/07   ag    Merged Lisbon(BT2.1) code to the mainline
09/21/07   ns    Replaced IShell_SendEvent with IShell_PostEvent
09/14/07   ns    Fixed problem of APP not receiving key press events during
                 automation on UIOne targets.
07/30/07   rs    Modify implemenation to send the continue soap responses in 
                 BPP to the application and get further requests from the 
                 applicatoin for "getevent".
06/26/07   pn    Removed compiler warnings.
05/22/07   ns    Fixed automation failure while deregistering server when it is
                 already connected
03/14/07   gs    Added NULL ptr check in IDS_BPP_TEST_PRINT
09/12/06   gs    Fixing RVCT compile issues on 7K target.
08/22/06   ns    Fixed problem with BPPDisconnect, BPPSecDisconnect 
                 and BPPSend concurrency APIs
06/12/06   ns    Added support for UserID during obex authentication
04/20/06   ns    Provided I/F to print file using BPP from another BREW App
12/26/05   ns    Device is set to discoverable mode when BPP server is registered
12/01/05   dc    Removed the pop_menu() from EVT_BPP_DISCONNECTED since 
                 it is done in DoUnbondOne() function in BTApp.c
10/17/05   dc    Moved the bEventIsProcessed and bGenericResult from the 
                 pMe struct to the BPP struct.
09/28/05   ns    Removed setting BusyIcon while sending responses to allow
                 concurrent operations. Removed using glodal variable for 
                 storing passkey. Always build menu's from main menu 
                 during automation to avoid menu stack overflow.
07/27/05   ns    Added support to enable BPP server during startup
07/12/05   dc    Added concurrency test automation interface for BPP Send
06/09/05   ns    BPP Main Menu item Deregister will not be displayed once BPP
                 is connected to remote
05/23/05   ns    Automated bonding procedure for BPP server.
05/12/05   gs    Added EVT_BPP_DISCONNECT_FAILED to handle STS disconnect
                 failed event due to an ongoing GetEvent SOAP
                 transaction.
05/01/05   gs    Allow only one SendFile operation to be pending at any 
                 point of time
04/18/05   dc    Fixed CR# 60335:BPP disconnect of STS channel leads to 
                 BPP wrong menu
04/05/05   dc    Added Optional field to BTApp_DiagBPPCliConExReqType to 
                 handle PIN code menu during connection.
03/25/05   pn    Made use of BTApp_SetBondable() and BTApp_ClearBondable().
                 Added BTApp_FTPConnect().
01/26/05   dc    Added automation support for STS connection.
                 Modified BTApp_DiagBPPProcessOBEXMatch to handle an
                 extra argument to process the event type.
01/12/04   gs    Disallow STS connection setup before DPS and DPS
                 disconnection before STS
01/05/05   dc    Added a Static Function to take care of selecting the BPP 
                 Channel. 
01/05/05   ns    Added automation support for BPP Settings and Target Menu 
                 options
12/28/04   dc    Added Automation Support to handle addition Menu to select 
                 DPS/STS
12/16/04   gs    Added support for 
                 - connecting to DPS (Job) and/or STS (Status) target
                 - sending SOAP request on Job or Status channel
                 - OBEX Authentication
12/15/04   gs    Increased soap response buffer size to 4k to accomodate
                 GetPrinterAttributes (All Attributes) response
11/23/04   gr    Changed indent to 2 spaces
                 Updated any comments/code to 80 cols
                 Reviewed if's for braces
                 Reviewed space between brackets and parameters
                 Removed unneeded global
10/28/04   gr    Added support for a distinct default file for each MIME type
10/13/04   gr    Added support for 
                 - Selecting OBEX servers for BPP
                 - Selecting MIME type for BPP
                 - Entering Job ID for BPP
                 - Support for automation of Register and DeRegister
                 - Support for Automation of Connect and Disconnect
                 - Support for Automation of Send
                 - Support for Automation of SendSOAP
08/24/04  pn     Correctly registers for BT notification (e.g. BT events.)

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp.h"
#include "BTAppUtils.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#ifdef FEATURE_BT_EXTPF_BPP
#include "AEEBTExtBPP.h"

#define EVT_BPP_CONN_REQ                         1
#define EVT_BPP_CONNECTED                        2
#define EVT_BPP_CONN_FAILED                      3
#define EVT_BPP_DISCONNECTED                     4

#define EVT_BPP_REG                              5
#define EVT_BPP_REG_FAILED                       6
#define EVT_BPP_DEREG                            7
#define EVT_BPP_DEREG_FAILED                     8

#define EVT_BPP_SEND_FILE_SUCCESS                9
#define EVT_BPP_SEND_FILE_FAILED                10
#define EVT_BPP_SOAP_REQ_SUCCESS                11
#define EVT_BPP_SOAP_REQ_FAILED                 12

#define EVT_BPP_SEC_DISCONNECTED                13
#define EVT_BPP_GET_REF_OBJ_REQ                 14

#define EVT_BPP_AUTH_REQ                        15
#define EVT_BPP_DISCONNECT_FAILED               16
#define EVT_BPP_SOAP_REQ_SUCCESS_CONTINUE       17


// Constants
#define BPP_SOAP_REQ_MAX_SIZE                 1000
#define BPP_SOAP_RSP_MAX_SIZE                 4000

char*  szServerNameBPP             = "QC BPP";

static char*  sPfileName[]         = {  
                                        "bpp_xhtml.txt",
                                        "bpp_basic.txt",
                                        "bpp_vcard2.vcf",
                                        "bpp_vcard3.vcf",
                                        "bpp_vcal1.vcs",
                                        "bpp_vcal2.vcs",
                                        "bpp_vmsg1.vmg",
                                        "bpp_pscrpt2.ps",
                                        "bpp_pscrpt3.ps",
                                        "bpp_pcl3c.pcl",
                                        "bpp_pcl5e.pcl",
                                        "bpp_pdf.pdf",
                                        "bpp_image.jpg",
                                        "bpp_image.gif",
                                      };
/* Filenames are now MIME sensitive and therefore the right file name is 
displayed via the wpFileName buffer */
static AECHAR wPfileName[32];

static char*  sSfileName           = "bpp_soap.txt";
static AECHAR wSfileName[]         = {'b','p','p','_','s','o','a','p','.','t','x','t',0};

static char*  sSRfileName          = "bpp_soap.txt.rsp";

/* Mime Format Types */
static AECHAR XHTML_PRINT[] =   {'X','h','t','m','l', 0};
static AECHAR BASIC_TEXT[]  =   {'P','l','a','i','n', 0};
static AECHAR VCARD_2[]     =   {'V','c','a','r','d', '2', 0};
static AECHAR VCARD_3[]     =   {'V','c','a','r','d', '3', 0};
static AECHAR VCAL_1[]      =   {'V','c','a','l','1', 0};
static AECHAR VCAL_2[]      =   {'V','c','a','l','2', 0};
static AECHAR VMSG_1[]      =   {'V','m','s','g','1', 0};
static AECHAR PSCRPT_2[]    =   {'P','s','c','r','p', 't', '2', 0};
static AECHAR PSCRPT_3[]    =   {'P','s','c','r','p', 't', '3', 0};
static AECHAR PCL3C[]       =   {'P','c','l','3','C', 0};
static AECHAR PCL5E[]       =   {'P','c','l','5','E', 0};
static AECHAR PDF[]         =   {'P','d','f', 0};
static AECHAR JPG[]         =   {'J','p','g', 0};
static AECHAR GIF89A[]      =   {'G','i','f', '8', '9', 'a', 0};

/* Target Type */
static AECHAR TARGET_DPS[]  =   {'D','P','S', 0};
static AECHAR TARGET_STS[]  =   {'S','T','S', 0};

extern uint32 uBTApp_NMask;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

/* ==========================================================================
FUNCTION BTApp_ProcessBPPNotifications
DESCRIPTION
============================================================================= */
void BTApp_ProcessBPPNotifications(
  CBTApp* pMe, 
  AEEEvent evt, 
  NotificationData* pData
)
{
  uint32 userEvent;

  switch ( evt )
  {
    case AEEBT_BPP_EVT_REG_DONE:  
    {
      if ( pData->uError == AEEBT_BPP_ERR_NONE )
      {
        userEvent = EVT_BPP_REG;
      }
      else 
      {
        userEvent = EVT_BPP_REG_FAILED;
      }
      BTAPP_POST_USER_EVENT( BPP, userEvent );
      break;
    }
    case AEEBT_BPP_EVT_DEREG_DONE: 
    {
      if ( pData->uError == AEEBT_BPP_ERR_NONE )
      {
        userEvent = EVT_BPP_DEREG;
      }
      else 
      {
        userEvent = EVT_BPP_DEREG_FAILED;
      }
      BTAPP_POST_USER_EVENT( BPP, userEvent );
      break;
    }
    case AEEBT_BPP_EVT_AUTH_REQ:
    {
      pMe->mBPP.bUserIDRequired = pData->BppObject.bUserIDRequired;
      pMe->mBPP.target          = pData->BppObject.target;
      MSG_LOW( "BPP - AuthReq UserIDReq=%d", pMe->mBPP.bUserIDRequired, 0, 0 );
      BTAPP_POST_USER_EVENT( BPP, EVT_BPP_AUTH_REQ );
      break;
    }
    case AEEBT_BPP_EVT_CONN_REQ:  
    {
      BTAPP_POST_USER_EVENT( BPP, EVT_BPP_CONN_REQ );
      pMe->mBPP.remoteBDAddr = pData->bdAddr;
      break;
    }
    case AEEBT_BPP_EVT_CONNECTED:  
    {
      if ( pData->BppObject.target == AEEBT_BPP_TARGET_DPS )
      {
        // Job channel
        pMe->mBPP.bConnected = TRUE;
        pMe->mBPP.bOKToSend  = TRUE;
      }
      else if ( pData->BppObject.target == AEEBT_BPP_TARGET_STS )
      {
        // Status channel
        pMe->mBPP.bSTSConnected = TRUE;
      }
      BTAppMgr_UpdateProfileType( pMe, &pMe->mBPP.remoteBDAddr, BTAPP_MGR_BPP, BTAPP_MGR_CONNECTED );
      BTAPP_POST_USER_EVENT( BPP, EVT_BPP_CONNECTED );
      break;
    }
    case AEEBT_BPP_EVT_CONN_FAILED: 
    {
      BTAPP_POST_USER_EVENT( BPP, EVT_BPP_CONN_FAILED );
      break;
    }
    case AEEBT_BPP_EVT_DISCONNECTED: 
    {
      if ( pData->uError != AEEBT_BPP_ERR_NONE )
      {
        BTAPP_POST_USER_EVENT( BPP, EVT_BPP_DISCONNECT_FAILED );
      }
      else 
      {
        if ( pData->BppObject.target == AEEBT_BPP_TARGET_DPS )
        {
          // Job channel
          pMe->mBPP.bConnected = FALSE;
          pMe->mBPP.bOKToSend  = TRUE;
          BTAPP_POST_USER_EVENT( BPP, EVT_BPP_DISCONNECTED );
        }
        else if ( pData->BppObject.target == AEEBT_BPP_TARGET_STS )
        {
          // Status channel
          pMe->mBPP.bSTSConnected = FALSE;
          BTAPP_POST_USER_EVENT( BPP, EVT_BPP_SEC_DISCONNECTED );
        }
      }
      BTAppMgr_UpdateProfileType( pMe, &pMe->mBPP.remoteBDAddr, BTAPP_MGR_BPP, BTAPP_MGR_DISCONNECTED );
      break;
    }
    case AEEBT_BPP_EVT_SEC_DISCONNECTED: 
    {
      BTAPP_POST_USER_EVENT( BPP, EVT_BPP_SEC_DISCONNECTED );
      break;
    }
    case AEEBT_BPP_EVT_SEND_FILE_DONE:
    {
      if ( pData->uError == AEEBT_BPP_ERR_NONE )
      {
        userEvent = EVT_BPP_SEND_FILE_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BPP_SEND_FILE_FAILED;
      }
      BTAPP_POST_USER_EVENT( BPP, userEvent );
      break;
    }
    case AEEBT_BPP_EVT_SOAP_REQUEST_DONE:
    {
      if ( pData->uError == AEEBT_BPP_ERR_NONE )
      {
        MSG_HIGH(" AEEBT_BPP_EVT_SOAP_REQUEST_DONE: err stat:-> AEEBT_BPP_ERR_NONE", 0, 0, 0); 
        userEvent = EVT_BPP_SOAP_REQ_SUCCESS;
      }
      else 
      {
        MSG_ERROR("AEEBT_BPP_EVT_SOAP_REQUEST_DONE: err stat:-> %x",pData->uError, 0, 0);
        userEvent = EVT_BPP_SOAP_REQ_FAILED;
      }
      BTAPP_POST_USER_EVENT( BPP, userEvent);
      break;
    }
    case AEEBT_BPP_EVT_SOAP_REQ_DONE_CONTINUE:
    {
      if ( pData->uError == AEEBT_BPP_ERR_NONE )
      {
        MSG_HIGH(" AEEBT_BPP_EVT_SOAP_REQUEST_DONE: err stat:-> AEEBT_BPP_ERR_NONE", 0, 0, 0); 
        userEvent = EVT_BPP_SOAP_REQ_SUCCESS_CONTINUE;
      }
      else 
      {
        MSG_ERROR("AEEBT_BPP_EVT_SOAP_REQUEST_DONE: err stat:-> %x",pData->uError, 0, 0);
        userEvent = EVT_BPP_SOAP_REQ_FAILED;
      }
      BTAPP_POST_USER_EVENT( BPP, userEvent);
      break;
    }
  case AEEBT_BPP_EVT_GET_REF_OBJ_REQ:  
    {
      pMe->mBPP.offset = pData->BppObject.offset;
      pMe->mBPP.count  = pData->BppObject.count;
      WSTRLCPY( pMe->mBPP.wRefObjName, pData->BppObject.wRefObjName,
                ARR_SIZE( pMe->mBPP.wRefObjName ) );

      BTAPP_POST_USER_EVENT( BPP, EVT_BPP_GET_REF_OBJ_REQ );
      break;
    }
    default:
      MSG_ERROR( "BPP - unexpected ev=%d", evt, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BPPBuildListMimeMenu
DESCRIPTION
============================================================================= */
static void BTApp_BPPBuildListMimeMenu( CBTApp* pMe )
{
  char       szStatus[] = " -      ";
  uint8      len = 0;
  
  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  if ( pMe->mBPP.bConnected )
  {
    szStatus[ 5 ] = 'C';
  }
  if ( pMe->mBPP.bSTSConnected )
  {
    szStatus[ 6 ] = 'C';
  }
  if ( pMe->mBPP.bSecConnected )
  {
    szStatus[ 7 ] = 'C';
  }
  if ( pMe->mBPP.bRegistered )
  {
    szStatus[ 4 ] = 'R';
  }

  ISHELL_LoadResString( pMe->a.m_pIShell, 
                        AEE_APPSBTAPP_RES_FILE, 
                        IDS_BPP_TESTS, 
                        pMe->pText2, 
                        SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
  len = WSTRLEN( pMe->pText2 );

  STRTOWSTR( szStatus, &pMe->pText2[ len ], 
             (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
  IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );

  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_XHTML_PRINT,
                    XHTML_PRINT, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_BASIC_TEXT,
                    BASIC_TEXT, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_VCARD,
                    VCARD_2, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_VCARD_3_0,
                    VCARD_3, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_VCALENDAR,
                    VCAL_1, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_VCALENDAR_2_0,
                    VCAL_2, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_VMESSAGE,
                    VMSG_1, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_POSTSCRIPT_2,
                    PSCRPT_2, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_POSTSCRIPT_3,
                    PSCRPT_3, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_PCL5E,
                    PCL5E, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_PCL3C,
                    PCL3C, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_PDF,
                    PDF, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_JPEG,
                    JPG, 0 );
  IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                    AEEBT_BPP_DOC_FORMAT_GIF89A,
                    GIF89A, 0 );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_BPP_LIST_MIME );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BPPBuildMainMenu
DESCRIPTION
============================================================================= */
void BTApp_BPPBuildMainMenu( CBTApp* pMe )
{
  char       szStatus[] = " -      ";
  uint8      len = 0;

  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  if ( pMe->mBPP.bConnected )
  {
    szStatus[ 5 ] = 'C';
  }
  if ( pMe->mBPP.bSTSConnected )
  {
    szStatus[ 6 ] = 'C';
  }
  if ( pMe->mBPP.bSecConnected )
  {
    szStatus[ 7 ] = 'C';
  }
  if ( pMe->mBPP.bRegistered )
  {
    szStatus[ 4 ] = 'R';
  }

  ISHELL_LoadResString( pMe->a.m_pIShell, 
                        AEE_APPSBTAPP_RES_FILE, 
                        IDS_BPP_TESTS, 
                        pMe->pText2, 
                        SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
  len = WSTRLEN( pMe->pText2 );

  STRTOWSTR( szStatus, &pMe->pText2[ len ], 
             (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
  IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );

  BTApp_InitAddItem( &ai );

  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SETTINGS, 0 );

  if ( pMe->mBPP.bRegistered == FALSE ) 
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REGISTER, 0 );
  }
  else
  {
    if ( pMe->mBPP.bConnected == FALSE && pMe->mBPP.bSTSConnected == FALSE )
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEREGISTER, 0 );
    }
  }

  if ( pMe->mBPP.bConnected == FALSE || pMe->mBPP.bSTSConnected == FALSE) 
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT, 0 );
  }
  
  if ( pMe->mBPP.bConnected == TRUE || pMe->mBPP.bSTSConnected == TRUE ) 
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
  }

  if ( pMe->mBPP.bRegistered == TRUE ) 
  {
    if ( pMe->mBPP.bConnected == TRUE && pMe->mBPP.bOKToSend == TRUE ) 
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BPP_SEND_FILE, 0 );
    }
    if ( pMe->mBPP.bConnected == TRUE || pMe->mBPP.bSTSConnected == TRUE ) 
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BPP_SOAP_REQUEST, 0 );
    }
  }
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_BPP_TESTS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_BPPBuildSettingsMenu
DESCRIPTION
============================================================================= */
void BTApp_BPPBuildSettingsMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );

  BTApp_InitAddItem( &ai );

  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, AEE_APPSBTAPP_RES_FILE, IDS_SETTINGS, NULL );

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_USE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BPP_TEST_PRINT, 0 );
  BTApp_AddMenuItem( pMe, 
                     pMe->m_pIMenu, 
                     &ai, 
                     IDS_AUTHENTICATE, 
                     pMe->mBPP.bDoAuthenticate ? IDB_CHECK_ON : IDB_CHECK_OFF );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_BPP_SETTINGS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BPPBuildTargetMenu
DESCRIPTION
============================================================================= */
void BTApp_BPPBuildTargetMenu( CBTApp* pMe )
{
  char       szStatus[] = " -      ";
  uint8      len = 0;
  
  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  if ( pMe->mBPP.bConnected )
  {
    szStatus[ 5 ] = 'C';
  }
  if ( pMe->mBPP.bSTSConnected )
  {
    szStatus[ 6 ] = 'C';
  }
  if ( pMe->mBPP.bSecConnected )
  {
    szStatus[ 7 ] = 'C';
  }
  if ( pMe->mBPP.bRegistered )
  {
    szStatus[ 4 ] = 'R';
  }

  ISHELL_LoadResString( pMe->a.m_pIShell, 
                        AEE_APPSBTAPP_RES_FILE, 
                        IDS_BPP_TESTS, 
                        pMe->pText2, 
                        SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
  len = WSTRLEN( pMe->pText2 );

  STRTOWSTR( szStatus, &pMe->pText2[ len ], 
             (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
  IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );

  if ( pMe->mBPP.sel == IDS_CONNECT ) 
  {
    if ( pMe->mBPP.bConnected == FALSE )
    {
      IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                        AEEBT_BPP_TARGET_DPS,
                        TARGET_DPS, 0 );
    }
    else if ( pMe->mBPP.bSTSConnected == FALSE )
    {
      IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                        AEEBT_BPP_TARGET_STS,
                        TARGET_STS, 0 );
    }
  }
  else if ( pMe->mBPP.sel == IDS_DISCONNECT )
  {
    // IDS_DISCONNECT 
    if ( pMe->mBPP.bSTSConnected == TRUE )
    {
      IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                        AEEBT_BPP_TARGET_STS,
                        TARGET_STS, 0 );
    }
    else if ( pMe->mBPP.bConnected == TRUE )
    {
      IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                        AEEBT_BPP_TARGET_DPS,
                        TARGET_DPS, 0 );
    }
  }
  else
  {
    // IDS_BPP_SOAP_REQUEST
    if ( pMe->mBPP.bConnected == TRUE )
    {
      IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                        AEEBT_BPP_TARGET_DPS,
                        TARGET_DPS, 0 );
    }
    if ( pMe->mBPP.bSTSConnected == TRUE )
    {
      IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                        AEEBT_BPP_TARGET_STS,
                        TARGET_STS, 0 );
    }
  }
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_BPP_LIST_TARGET );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BPPCleanup
DESCRIPTION
============================================================================= */
void BTApp_BPPCleanup( CBTApp* pMe )
{
  if ( pMe->mBPP.po != NULL )
  {
    IBTEXTBPP_Release( pMe->mBPP.po );
    pMe->mBPP.po = NULL;
  }
  if ( pMe->mBPP.pIFileMgr != NULL )
  {
    IFILEMGR_Release( pMe->mBPP.pIFileMgr );
    pMe->mBPP.pIFileMgr = NULL;
  }
  // unregister for BPP notification
  ISHELL_RegisterNotify( pMe->a.m_pIShell,  
                         AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 
                         0 );
  uBTApp_NMask &= ~NMASK_BT_BPP;
  ISHELL_RegisterNotify( pMe->a.m_pIShell,  
                         AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 
                         uBTApp_NMask );
}

/* ==========================================================================
FUNCTION BTApp_BPPRemotePrintReq
DESCRIPTION
============================================================================= */
boolean BTApp_BPPRemotePrintReq( CBTApp* pMe, uint32 dwParam )
{
  BTApp_BPPPrintDetails* printDetails;
  int                    nbufSize = 0;


  
  MSG_LOW( "BPPRemotePrintReq - Print req from any BREW APP", 0, 0, 0 );
  /* store the print details */
  printDetails = (BTApp_BPPPrintDetails*)dwParam;
  nbufSize = STRLEN(printDetails->pPrintFileName) + 1;
  pMe->mBPP.printDetails.pPrintFileName = MALLOC( nbufSize );

  
  if ( pMe->mBPP.printDetails.pPrintFileName == NULL )
  {
    MSG_ERROR( "BPPRemotePrintReq - Malloc failed", 0, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_BPP_MSG_SEND_FILE_FAILED, NULL, 2 );
    /* free the memory received from remote APP */
    FREEIF( printDetails->pPrintFileName );
    FREEIF( printDetails );
    return FALSE;
  }
  
  STRLCPY( pMe->mBPP.printDetails.pPrintFileName, printDetails->pPrintFileName, nbufSize );
  pMe->mBPP.printDetails.uMIMMediaType = printDetails->uMIMMediaType;
  
  /* free the memory received from remote APP */
  FREEIF( printDetails->pPrintFileName );
  FREEIF( printDetails );


  
  /* register BPP if not registered */
  if ( pMe->mBPP.bRegistered == FALSE )
  {
    BTApp_SetBondable( pMe );
    if ( IBTEXTBPP_Register( pMe->mBPP.po, szServerNameBPP ) != SUCCESS )
    {
	  
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      MSG_HIGH( "BPPRemotePrintReq - BPP registration failed", 0, 0, 0 );
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
      FREE( pMe->mBPP.printDetails.pPrintFileName );
      return FALSE;    
    }
    else
    {
      pMe->mBPP.bPrintFile = TRUE;
      if ( pMe->mSD.bDiscoverable == FALSE )
      {
        IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
      }
    }
  }
  else if ( pMe->mBPP.bConnected == FALSE )
  {

	
    if ( IBTEXTBPP_Connect( pMe->mBPP.po, 
                            &pMe->mBPP.printerBDAddr, 
                            0x0, 
                            AEEBT_BPP_TARGET_DPS ) != SUCCESS )
    {
	  
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      MSG_HIGH( "BPPRemotePrintReq - conn failed", 0, 0, 0 );
      BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      FREE( pMe->mBPP.printDetails.pPrintFileName );
      return FALSE;
    }
    pMe->mBPP.bPrintFile = TRUE;
  }
  else
  {
	
    /* already connected, just print the file */
    pMe->mBPP.bPrintFile = FALSE;
    STRTOWSTR( pMe->mBPP.printDetails.pPrintFileName, wPfileName, 
               sizeof( wPfileName ) );
    if ( IBTEXTBPP_SendFile( pMe->mBPP.po, 
                             pMe->mBPP.printDetails.uMIMMediaType,
                             pMe->mBPP.printDetails.pPrintFileName,
                             NULL,
                             0 ) != SUCCESS )
    {
	  
      MSG_HIGH( "BPPRemotePrintReq - print file failed", 0, 0, 0 );
      BTApp_ShowMessage( pMe, IDS_BPP_MSG_SEND_FILE_FAILED, wPfileName, 3 );
      FREE( pMe->mBPP.printDetails.pPrintFileName );
      return FALSE;
    }
  }
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_BPPSendFile
DESCRIPTION
============================================================================= */
boolean BTApp_BPPSendFile( CBTApp* pMe, AEEBTBPPdocType docType, uint32 jobID )
{
  if ( pMe->mBPP.bOKToSend == FALSE ) 
  {
    BTApp_ShowMessage( pMe, IDS_BPP_MSG_SEND_FILE_FAILED, wPfileName, 3 );
    return FALSE;
  }

  DBGPRINTF( "BTApp_BPPSendFile DocType=%d, JobID=%d, file=%s", docType, 
             jobID, sPfileName[ docType ] );
  STRTOWSTR( sPfileName[ docType ], wPfileName, sizeof( wPfileName ) );
  if ( IBTEXTBPP_SendFile( pMe->mBPP.po, 
                           docType,
                           sPfileName[ docType ],
                           NULL,
                           jobID ) != SUCCESS )
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    BTApp_ShowMessage( pMe, IDS_BPP_MSG_SEND_FILE_FAILED, wPfileName, 3 );
    return FALSE;
  }
  else
  {
    pMe->mBPP.bOKToSend = FALSE;

    BTApp_ShowBusyIcon( pMe );
    return TRUE;
  }
}

/* ==========================================================================
FUNCTION BTApp_BPPSOAPRequest
DESCRIPTION
============================================================================= */
void BTApp_BPPSOAPRequest( CBTApp* pMe )
{
  // SOAP request
  pMe->mBPP.sndrSOAPmsgLen    = 0;
  pMe->mBPP.sndrSOAPmsgMaxLen = BPP_SOAP_REQ_MAX_SIZE;

  // SOAP response
  pMe->mBPP.SOAPmsgLen        = 0;
  pMe->mBPP.SOAPmsgMaxLen     = BPP_SOAP_RSP_MAX_SIZE;

  if ( pMe->mBPP.pSndrSOAPmsg != NULL ) 
  {
    FREE ( pMe->mBPP.pSndrSOAPmsg );
    pMe->mBPP.pSndrSOAPmsg = NULL;
  }
  if ( (pMe->mBPP.pSndrSOAPmsg = 
        MALLOC( pMe->mBPP.sndrSOAPmsgMaxLen )) == NULL )
  {
    BTApp_ShowMessage( pMe, IDS_BPP_MSG_SOAP_REQ_FAILED, NULL, 3 );
  }

  if ( pMe->mBPP.pSOAPmsg != NULL ) 
  {
    FREE ( pMe->mBPP.pSOAPmsg );
    pMe->mBPP.pSOAPmsg = NULL;
  }

  if ( ( pMe->mBPP.pSOAPmsg = MALLOC( pMe->mBPP.SOAPmsgMaxLen ) ) == NULL )
  {
    BTApp_ShowMessage( pMe, IDS_BPP_MSG_SOAP_REQ_FAILED, NULL, 3 );
  }

  if ( (pMe->mBPP.pIFile = 
        IFILEMGR_OpenFile( pMe->mBPP.pIFileMgr, 
                           sSfileName, 
                           _OFM_READ )) == NULL )
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    BTApp_ShowMessage( pMe, IDS_BPP_MSG_SOAP_REQ_FAILED, NULL, 3 );
    return;
  }
  else
  {
    // Read Sender's SOAP message
    pMe->mBPP.sndrSOAPmsgLen = IFILE_Read( pMe->mBPP.pIFile, 
                                           pMe->mBPP.pSndrSOAPmsg, 
                                           pMe->mBPP.sndrSOAPmsgMaxLen - 1 );

    // Null terminate the string
    pMe->mBPP.pSndrSOAPmsg[ pMe->mBPP.sndrSOAPmsgLen ] = '\0';

    IFILE_Release( pMe->mBPP.pIFile );
  }

  if ( IBTEXTBPP_SOAPRequest( pMe->mBPP.po, 
                              pMe->mBPP.pSndrSOAPmsg,
                              pMe->mBPP.pSOAPmsg, 
                              pMe->mBPP.SOAPmsgMaxLen,
                              pMe->mBPP.target ) != SUCCESS )
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    BTApp_ShowMessage( pMe, IDS_BPP_MSG_SOAP_REQ_FAILED, NULL, 3 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe ); // wait for confirm
  }
}

void BTApp_BPPEmptyRequest( CBTApp* pMe )
{
  // SOAP request
  pMe->mBPP.sndrSOAPmsgLen    = 0;
  pMe->mBPP.sndrSOAPmsgMaxLen = BPP_SOAP_REQ_MAX_SIZE;
  if ( pMe->mBPP.pSndrSOAPmsg != NULL ) 
  {
    FREE ( pMe->mBPP.pSndrSOAPmsg );
    pMe->mBPP.pSndrSOAPmsg = NULL;
  }

  // SOAP response
  pMe->mBPP.SOAPmsgLen        = 0;
  pMe->mBPP.SOAPmsgMaxLen     = BPP_SOAP_RSP_MAX_SIZE;
  if ( pMe->mBPP.pSOAPmsg != NULL ) 
  {
    FREE ( pMe->mBPP.pSOAPmsg );
    pMe->mBPP.pSOAPmsg = NULL;
  }

  if ( ( pMe->mBPP.pSOAPmsg = MALLOC( pMe->mBPP.SOAPmsgMaxLen ) ) == NULL )
  {
    BTApp_ShowMessage( pMe, IDS_BPP_MSG_SOAP_REQ_FAILED, NULL, 3 );
  }

  if ( IBTEXTBPP_SOAPRequest( pMe->mBPP.po, 
                              pMe->mBPP.pSndrSOAPmsg,
                              pMe->mBPP.pSOAPmsg, 
                              pMe->mBPP.SOAPmsgMaxLen,
                              pMe->mBPP.target ) != SUCCESS )
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    BTApp_ShowMessage( pMe, IDS_BPP_MSG_SOAP_REQ_FAILED, NULL, 3 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe ); // wait for confirm
  }
}



/* ==========================================================================
FUNCTION BTApp_BPPGetRefObjResponse
DESCRIPTION
============================================================================= */
void BTApp_BPPGetRefObjResponse( CBTApp* pMe )
{
  char sRefObjName[ AEEBT_MAX_FILE_NAME + 1 ];
  
  (void) WSTRTOSTR( pMe->mBPP.wRefObjName, 
                    sRefObjName, 
                    sizeof (sRefObjName) );
  
  if ( IBTEXTBPP_GetRefObjResponse( pMe->mBPP.po, 
                                    sRefObjName,
                                    pMe->mBPP.offset,
                                    pMe->mBPP.count ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, 
                       IDS_BPP_MSG_SEND_FILE_FAILED, 
                       pMe->mBPP.wRefObjName, 
                       3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BPPSaveSOAPresponse
DESCRIPTION
============================================================================= */
void BTApp_BPPSaveSOAPresponse( CBTApp* pMe )
{
  // Remove target file
  IFILEMGR_Remove( pMe->mBPP.pIFileMgr, sSRfileName );

  if ( (pMe->mBPP.pIFile = 
        IFILEMGR_OpenFile( pMe->mBPP.pIFileMgr, 
                           sSRfileName, 
                           _OFM_CREATE )) == NULL )
  {
    BTApp_ShowMessage( pMe, IDS_BPP_MSG_SOAP_REQ_FAILED, NULL, 3 );
    return;
  }
  else
  {
    IFILE_Write( pMe->mBPP.pIFile, pMe->mBPP.pSOAPmsg, 
                 STRLEN( pMe->mBPP.pSOAPmsg ) );
    IFILE_Release( pMe->mBPP.pIFile );
  }

  // Free allocated memory
  if ( pMe->mBPP.pSOAPmsg )
  {
    FREE( pMe->mBPP.pSOAPmsg );
    pMe->mBPP.pSOAPmsg   = NULL;
    pMe->mBPP.SOAPmsgLen = 0;
  }
}

/* ==========================================================================
FUNCTION BTApp_BPPInit
DESCRIPTION
============================================================================= */
boolean BTApp_BPPInit( CBTApp* pMe )
{
  static boolean init_done = FALSE;
  uint32 uNMask = NMASK_BT_BPP | (((uint32)(NOTIFIER_VAL_ANY)) << 16);

  if ( init_done == FALSE )
  {
    if ( (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_BPP, 
                                 (void**)&pMe->mBPP.po ) == SUCCESS) &&
         (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, 
                                 (void **)&pMe->mBPP.pIFileMgr ) == SUCCESS) &&
         (ISHELL_RegisterNotify( pMe->a.m_pIShell,  AEECLSID_BLUETOOTH_APP,
                                 AEECLSID_BLUETOOTH_NOTIFIER, 
                                 uNMask ) == SUCCESS) )
    {
      uBTApp_NMask |= NMASK_BT_BPP;
      init_done = TRUE;
    }
  }
  return init_done;
}

/* ==========================================================================
FUNCTION BTApp_BPPBuildMenu
DESCRIPTION
============================================================================= */
boolean BTApp_BPPBuildMenu( CBTApp* pMe, BTAppMenuType menu )
{
  boolean built = TRUE;
  switch ( menu)
  {
    case BT_APP_MENU_BPP_TESTS:
    {
      if ( BTApp_BPPInit( pMe ) != FALSE )
      {
        BTApp_BPPBuildMainMenu( pMe );
      }
      else
      {
        MSG_ERROR( "BPPBuildMenu - failed to create BPP object", 0, 0, 0 );
        BTApp_BPPCleanup( pMe );
        built = FALSE;
      }
      break;
    }
    case BT_APP_MENU_BPP_LIST_MIME:
    {
      BTApp_BPPBuildListMimeMenu( pMe );
      break;
    }
    case BT_APP_MENU_BPP_LIST_TARGET:
    {
      BTApp_BPPBuildTargetMenu( pMe );
      break;
    }
    case BT_APP_MENU_BPP_SETTINGS:
    {
      BTApp_BPPBuildSettingsMenu( pMe );
      break;
    }  
    default:
      built = FALSE;
      break;
  }
  return built;
}

/* ==========================================================================
FUNCTION BTApp_BPPHandleSelection
DESCRIPTION
============================================================================= */
static boolean BTApp_BPPHandleSelection( CBTApp* pMe, uint16 sel )
{
  uint16  selection   = 0;
  boolean key_handled = TRUE;
  boolean bRegistered = FALSE;

  switch ( sel )
  {
    case IDS_REGISTER:
    {
      BTApp_SetBondable( pMe );
      if ( IBTEXTBPP_Register( pMe->mBPP.po,
                               szServerNameBPP ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
      }
      else
      {
        if ( pMe->mSD.bDiscoverable == FALSE )
        {
          IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
        }
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case IDS_DEREGISTER:
    {
      if ( IBTEXTBPP_Deregister( pMe->mBPP.po ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      }
      else
      {
        bRegistered = pMe->mBPP.bRegistered;
        pMe->mBPP.bRegistered = FALSE;
        BTApp_CheckToClearDiscoverable( pMe );
        pMe->mBPP.bRegistered = bRegistered;
        BTApp_ShowBusyIcon( pMe );
      }
      break;
    }
    case IDS_SETTINGS:
    {
      BTApp_BPPBuildSettingsMenu( pMe );
      break;
    }
    case IDS_USE:
    {
      key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_OBEX_SERVERS );
      break;
    }
    case IDS_BPP_TEST_PRINT:
    {
      BTApp_BPPPrintDetails* printDetails;
      int                    nLen = STRLEN(sPfileName[0]) + 1;
      if ( (SUCCESS == ERR_MALLOCREC(BTApp_BPPPrintDetails, &printDetails)) &&
           (SUCCESS == ERR_MALLOC(nLen, &printDetails->pPrintFileName)) )
      {
        STRLCPY( printDetails->pPrintFileName, sPfileName[0], nLen );
        printDetails->uMIMMediaType = AEEBT_BPP_DOC_FORMAT_XHTML_PRINT;
        ISHELL_PostEventEx( pMe->a.m_pIShell, EVTFLG_ASYNC, 
                            AEECLSID_BLUETOOTH_APP,
                            EVT_USER, EVT_BPP_SIMPLE_PUSH, (uint32)printDetails );
      }
      else
      {
        MSG_ERROR( "BPPHandleSelection : Mem Alloc Failure", 0, 0, 0 );
        FREEIF ( printDetails );
      }
      break;
    }    
    case IDS_AUTHENTICATE:
    {
      pMe->bConfigChanged = TRUE;
      pMe->mBPP.bDoAuthenticate = pMe->mBPP.bDoAuthenticate ? FALSE : TRUE;
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );

      BTApp_UpdateMenuItemImage( 
        pMe->a.m_pIDisplay, 
        pMe->m_pIMenu, 
        selection,
        pMe->mBPP.bDoAuthenticate ? IDB_CHECK_ON : IDB_CHECK_OFF );
      break;
    }
    case IDS_CONNECT:
    case IDS_DISCONNECT:
    case IDS_BPP_SOAP_REQUEST:
    {
      /* Remember selection */
      pMe->mBPP.sel = sel;

      /* Show list of targets */
      BTApp_BuildMenu( pMe, BT_APP_MENU_BPP_LIST_TARGET );
      break;
    }
    case IDS_BPP_SEND_FILE:
    {
      /* Provide a list of DocumentTypes to choose from - Menu Control */
      BTApp_BuildMenu( pMe, BT_APP_MENU_BPP_LIST_MIME );
      /* Provide a mechanism to provide a job ID - Text Control */
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
FUNCTION BTApp_HandleBPPMimeMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleBPPMimeMenu( CBTApp* pMe, uint16 key )
{
  /* Selection has been determined */
  switch ( key )
  {
    case AVK_SELECT:
    {
      pMe->mBPP.iDocType = IMENUCTL_GetSel( pMe->m_pIMenu );
      BTApp_BuildMenu( pMe, BT_APP_MENU_BPP_JOB_ID );
      break;
    }
    case AVK_CLR:
    {
      BTApp_HandleClearKey( pMe );
      break;
    }
    default:
    {
      return FALSE;
    }
  }
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_HandleBPPTargetMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleBPPTargetMenu( CBTApp* pMe, uint16 key )
{
  boolean key_handled = TRUE;

  /* Selection has been determined */
  switch ( key )
  {
    case AVK_SELECT:
    {
      pMe->mBPP.target = IMENUCTL_GetSel( pMe->m_pIMenu );

      switch ( pMe->mBPP.sel ) 
      {
        case IDS_CONNECT:
        {
          key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_OBEX_SERVERS );
          break;
        }
        case IDS_DISCONNECT:
        {
          if ( IBTEXTBPP_Disconnect( pMe->mBPP.po, 
                                     pMe->mBPP.target ) != SUCCESS )
          {
            BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
          }
          else
          {
            (void) POP_MENU();
            BTApp_ShowBusyIcon( pMe );
          }
          break;
        }
        case IDS_BPP_SOAP_REQUEST:
        {
          BTApp_BPPSOAPRequest( pMe );
          break;
        }
      }
      break;
    }
    case AVK_CLR:
    {
      BTApp_HandleClearKey( pMe );
      break;
    }
    default:
    {
      key_handled = FALSE;
    }
  }
  return key_handled;
}

/* ==========================================================================
FUNCTION BTApp_HandleBPPMainMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleBPPMainMenu( CBTApp* pMe, uint16 key )
{
  uint16 selection;
  boolean ev_processed = TRUE;

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_BPPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key)
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_BPPHandleSelection( pMe, selection );
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
FUNCTION BTApp_HandleBPPSettingsMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleBPPSettingsMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlBPPSettings - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      ev_processed = BTApp_BPPHandleSelection( pMe, selection );
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
FUNCTION BTApp_BPPHandleMenus
DESCRIPTION
============================================================================= */
boolean BTApp_BPPHandleMenus( CBTApp* pMe, uint16 key, BTAppMenuType menu )
{
  boolean handled = TRUE;
  switch ( menu )
  {
    case BT_APP_MENU_BPP_TESTS:
      handled = BTApp_HandleBPPMainMenu( pMe, key );
      break;
    case BT_APP_MENU_BPP_LIST_MIME:
      handled = BTApp_HandleBPPMimeMenu( pMe, key );
      break;
    case BT_APP_MENU_BPP_LIST_TARGET:
      handled = BTApp_HandleBPPTargetMenu( pMe, key );
      break;
    case BT_APP_MENU_BPP_SETTINGS:
      handled = BTApp_HandleBPPSettingsMenu( pMe, key );
      break;
    default:
      MSG_ERROR(" BPP: unexpected menu type %x", menu, 0, 0 );
      handled = FALSE;
      break;
  }
  return handled;
}

/* ==========================================================================
FUNCTION BTApp_BPPHandleUserEvents
DESCRIPTION
============================================================================= */
void BTApp_BPPHandleUserEvents( CBTApp* pMe, uint32 dwParam )
{
  uint16 msgID;

  switch ( dwParam )
  {
    case EVT_BPP_AUTH_REQ:
      if ( pMe->mBPP.bAuthenticated == FALSE )
      {
        pMe->mBPP.bAuthRequested = TRUE;
        if ( pMe->mBPP.bUserIDRequired != FALSE )
        {
          BTApp_BuildMenu( pMe, BT_APP_MENU_BPP_USERID );
        }
        else
        {
          BTApp_BuildMenu( pMe, BT_APP_MENU_BPP_PASSWORD );
        }
      }
      else
      {
        // Already authenticated once
        if ( pMe->mBPP.bUserIDRequired != FALSE )
        {
          if ( pMe->mBPP.wUserID[0] != NULL )
          {
            IBTEXTBPP_Authenticate( pMe->mBPP.po,
                                    pMe->mBPP.target,
                                    pMe->mBPP.wUserID,
                                    pMe->mBPP.wPassword );
          }
          else
          {
            pMe->mBPP.bAuthRequested = TRUE;
            BTApp_BuildMenu( pMe, BT_APP_MENU_BPP_USERID );
          }
        }
        else
        {
          IBTEXTBPP_Authenticate( pMe->mBPP.po,
                                  pMe->mBPP.target,
                                  NULL,
                                  pMe->mBPP.wPassword );
        }
      }
      break;
    case EVT_BPP_CONN_REQ:
      pMe->mBPP.bSecConnected = TRUE;
      IBTEXTBPP_AcceptConnection( pMe->mBPP.po );
      if ( pMe->mBPP.bAuthRequested != FALSE )
      {
        pMe->mBPP.bAuthenticated = TRUE;
        pMe->mBPP.bAuthRequested = FALSE;
        BTApp_BuildTopMenu( pMe );
      }
      break;
  case EVT_BPP_REG:
      if ( pMe->mEnablingType == BTAPP_ENABLING_NONE )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_DONE, NULL, 2 );
      }
      pMe->mBPP.bRegistered = TRUE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      /* incase registration was request by remote app */
      if ( pMe->mBPP.bPrintFile != FALSE )
      {
        if ( IBTEXTBPP_Connect( pMe->mBPP.po, 
                                &pMe->mBPP.printerBDAddr, 
                                0x0, 
                                AEEBT_BPP_TARGET_DPS ) != SUCCESS )
        {
          BTApp_ClearBondable( pMe ); // no need to be bondable anymore
          MSG_HIGH( "BPPRemotePrintReq - conn failed", 0, 0, 0 );
          if ( pMe->mEnablingType == BTAPP_ENABLING_NONE )
          {
            BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
          }
          FREEIF( pMe->mBPP.printDetails.pPrintFileName );
          pMe->mBPP.bPrintFile = FALSE;
        }
      }
      if(pMe->mEnablingType != BTAPP_ENABLING_NONE)
      {
        pMe->mEnablingType++;       
        BTApp_EnableBT(pMe);
      } 
      break;
    case EVT_BPP_REG_FAILED:
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      if ( pMe->mBPP.bPrintFile != FALSE )
      {
        FREEIF( pMe->mBPP.printDetails.pPrintFileName );
        pMe->mBPP.bPrintFile = FALSE;// incase of remote printing
      }
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
    case EVT_BPP_DEREG:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_DONE, NULL, 2 );
      pMe->mBPP.bRegistered = FALSE;
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BPP_DEREG_FAILED:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      break;
    case EVT_BPP_CONNECTED:
      pMe->mBPP.bConnected = TRUE;
      if ( pMe->mBPP.bAuthRequested != FALSE )
      {
        pMe->mBPP.bAuthenticated = TRUE;
        pMe->mBPP.bAuthRequested = FALSE;
      }      
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      if ( pMe->mBPP.bPrintFile != FALSE )
      {
        STRTOWSTR( pMe->mBPP.printDetails.pPrintFileName, wPfileName, 
                   sizeof( wPfileName ) );
        if ( IBTEXTBPP_SendFile( pMe->mBPP.po, 
                                 pMe->mBPP.printDetails.uMIMMediaType,
                                 pMe->mBPP.printDetails.pPrintFileName,
                                 NULL,
                                 0 ) != SUCCESS )
        {
          MSG_HIGH( "BPPRemotePrintReq - print file failed", 0, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_BPP_MSG_SEND_FILE_FAILED, wPfileName, 3 );
        }
        FREEIF( pMe->mBPP.printDetails.pPrintFileName );
        pMe->mBPP.bPrintFile = FALSE;
      }
      (void) POP_MENU();
      BTApp_BuildTopMenu( pMe ); // rebuild menu to show 'C'
      break;
    case EVT_BPP_DISCONNECTED:
      pMe->mBPP.bConnected = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      pMe->mBPP.bAuthenticated = FALSE;
      if ( pMe->mBPP.bEndingTest != FALSE )
      {
        pMe->mBPP.bEndingTest = FALSE;
        BTApp_HandleClearKey( pMe );
      }
      else
      {
        BTApp_BuildTopMenu( pMe ); // rebuild menu to hide 'C'
      }
      break;
    case EVT_BPP_SEC_DISCONNECTED:
      pMe->mBPP.bSecConnected = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION     
      BTApp_BuildTopMenu( pMe ); // rebuild menu to hide second 'C'
      break;
    case EVT_BPP_DISCONNECT_FAILED:
      BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
      break;
    case EVT_BPP_CONN_FAILED:
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      if ( pMe->mBPP.bPrintFile != FALSE )
      {
        FREEIF( pMe->mBPP.printDetails.pPrintFileName ); // in case of remote printing
        pMe->mBPP.bPrintFile = FALSE;
      }
      break;
    case EVT_BPP_SEND_FILE_SUCCESS:
      pMe->mBPP.bOKToSend = TRUE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BPP_MSG_SEND_FILE_SUCCESS;
      BTApp_ShowMessage( pMe, msgID, wPfileName, 2 );
      break;
    case EVT_BPP_SEND_FILE_FAILED:
      pMe->mBPP.bOKToSend = TRUE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BPP_MSG_SEND_FILE_FAILED;
      BTApp_ShowMessage( pMe, msgID, wPfileName, 2 );
      break;
    case EVT_BPP_SOAP_REQ_SUCCESS:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BPP_MSG_SOAP_REQ_SUCCESS;

      // Store the SOAP response message in a file
      BTApp_BPPSaveSOAPresponse( pMe );

      BTApp_ShowMessage( pMe, msgID, wSfileName, 2 );
      break;

  case EVT_BPP_SOAP_REQ_SUCCESS_CONTINUE:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

     // The memory that stores response soap is freed in BPPSaveSOAPresponse
     // and so call the BTApp_BPPEmptyRequest after parsing the data. 
     // Check if the response is for a "getevent" and 
     // the content in the soap response says anything else than "idle":
     // send the soap request once again: 
    if (NULL != STRSTR((const char *)pMe->mBPP.pSOAPmsg,(const char *)"GetEvent")) {
        // The soap response is for getevent
        if ( NULL == STRSTR((const char *)pMe->mBPP.pSOAPmsg, (const char *)"idle")) {

            // Store the SOAP response message in a file
            BTApp_BPPSaveSOAPresponse( pMe );

            msgID = IDS_BPP_MSG_SOAP_REQ_SUCCESS;
            BTApp_ShowMessage( pMe, msgID, wSfileName, 2 );

             // Keep sending the GetEvent message for the printing status
             // as the printing still continues
            BTApp_BPPEmptyRequest( pMe );
        }else{
            // This is the place when we should stop sending any further 
            // GetEvent as the printing is over.
            // Abort the connection now */
            // This API also needs a target as there could be multiple channels open 
            // AEEBTBPPTargetType     target
            IBTEXTBPP_Abort(pMe->mBPP.po);

            // This would allow the abort to go through beyond the obex
            // And finally once the abort is completed a response indicating a
            // fail message for this request shall be returned.
            BTApp_BPPEmptyRequest( pMe );

        }
    }
    break;


    case EVT_BPP_SOAP_REQ_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION      
      msgID = IDS_BPP_MSG_SOAP_REQ_FAILED;
      BTApp_ShowMessage( pMe, msgID, wSfileName, 2 );
      break;
    case EVT_BPP_GET_REF_OBJ_REQ:
      BTApp_BPPGetRefObjResponse( pMe );
      break;
    default:
      MSG_ERROR(" BPP: unexpected user event %x", dwParam, 0, 0 );
      break;
  }
}

/* ==========================================================================
FUNCTION BTApp_BPPConnect
DESCRIPTION
============================================================================= */
void BTApp_BPPConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr )
{
  BTApp_SetBondable( pMe );
  if ( IBTEXTBPP_Connect( pMe->mBPP.po, 
                          pBDAddr, 
                          0x0, 
                          pMe->mBPP.target ) != SUCCESS )
  {
    BTApp_ClearBondable( pMe ); // no need to be bondable anymore
    BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
  }
  else
  {
    pMe->mBPP.remoteBDAddr = *pBDAddr; 
    BTApp_ShowBusyIcon( pMe );
  }
}
/* ==========================================================================
FUNCTION BTApp_EnableBPP
DESCRIPTION
============================================================================= */
boolean BTApp_EnableBPP( 
  CBTApp*  pMe, 
  boolean* pbSettingBondable, 
  boolean* pbSettingDiscoverable )
{
  boolean bBPPEnabled = FALSE;
  
  if ( pMe->mBPP.bServerEnable == TRUE )
  {

#ifdef FEATURE_BT_2_1
    if( pMe->mBPP.bSecSelected !=FALSE ) 
    {
      IBTEXTRM_SetSecBySvcCls( 
             pMe->mRM.po, 
             AEEBT_SD_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE,
             pMe->mBPP.srvSecType,
             pMe->mBPP.bAuthorize,
             pMe->mBPP.bAuthorizeFirst );
    }
#endif //FEATURE_BT_2_1

    if ( BTApp_BPPInit( pMe ) == FALSE )
    {
      MSG_ERROR( "EnableBPP - failed to create BPP object", 0, 0, 0 );
      BTApp_BPPCleanup( pMe );
    }
    else if ( pMe->mBPP.bRegistered == FALSE )
    {
      if ( *pbSettingBondable == FALSE )
      {
        BTApp_SetBondable( pMe );
        *pbSettingBondable = TRUE;
      }
      MSG_LOW( "EnableBPP - enabling BPP", 0, 0, 0 );
      if ( IBTEXTBPP_Register( pMe->mBPP.po,
                               szServerNameBPP ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        MSG_ERROR( "EnableBPP - Failed to enable BPP", 0, 0, 0 );
      }
      else
      {
        if ( *pbSettingDiscoverable == FALSE )
        {
          IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
          *pbSettingDiscoverable = TRUE;
        }
        BTApp_ShowBusyIcon( pMe );
        bBPPEnabled = TRUE;
      }
    }
  }
  return bBPPEnabled;
}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

#endif /* FEATURE_BT_EXTPF_BPP */

#endif /* FEATURE_APP_BLUETOOTH */


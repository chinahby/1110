/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppBIP.c                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains BIP test feature.        */  
/*                                                                           */  
/*              Copyright (c) 2004-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppBIP.c#5 $
$DateTime: 2009/06/26 15:13:15 $
$Author: phuongn $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
06/26/09   pn   Fixed bug in extracting data from string incrementally.
06/11/09   ns   changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
06/01/09   pn   Fixed critical Lint errors.
05/22/09   pn   Improved handling of GetImageList request.
05/21/08   RS   Brought in the feature of Aborting the ongoing PUT or GET 
                and other pending operation.05/21/08   NS	Added Automatic archive functions
05/12/08   VM   Replacing deprecated functions with BREW replacements.
04/10/08   VM   Changed the profile enabling mechanism to correct the failure
                in enabling profiles.
03/06/08   VM   Removed redundant IBTEXTRM_SetSecBySvcCls call
12/04/07   mh   App manager to support low power configuration
11/27/07   rs   The UUID associated to the adv img printing supplied correctly.
11/01/07   ag   Merged Lisbon(BT2.1) code to the mainline
09/21/07   ns   Used a macro for simulating a key press event in automation.
                Replaced send event with post event
09/14/07   ns   Fixed problem of APP not receiving key press events during
                automation on UIOne targets.
06/26/07   pn   Removed compiler warnings.
06/21/07   gs   Fixed incorrect supportedFunctions attribute in Imaging responder
                service registration.  Added corresponding #defines.
05/22/07   ns   Fixed automation failure while deregistering server when it is
                already connected
03/14/07   gs   Added NULL ptr check in IDS_BIP_TEST_PRINT
11/03/06   gs   Modified BIP client and server menu options to take into 
                account primary vs secondary client/server roles.
09/12/06   gs   Fixing RVCT compile issues on 7K target.
06/07/06   ns   BIP server status updated to connected once connection is
                accepted, as no connected event received on server side.
04/17/06   sn   Reverted the changes made to BIP menu options as BIP needs both
                client and server menu options.
04/10/06   gs   Fixed RemoteDisplayResponse() to allow for empty 
                image-handle in the request.
04/05/06   ns   Fixed problem of busy icon being displayed on server during 
                connection when authentication is enabled
03/22/05   sn   Updated BIP main menu options to reflect app support for Client 
                or Server one at a time.
12/16/05   ns   Fixed problem with remote printing
11/14/05   ns   Added support to send a BIP print request from other apps
10/25/05   ns   Fixed automation featurization problems
10/09/05   dc   Removed ShowMessage for all the BTApp_BIP*Response functions.
                Removed ShowMessage when the bIsConcEventActive flag is TRUE
09/28/05   ns   Removed setting BusyIcon while sending responses to allow
                concurrent operations. Removed using glodal variable for 
                storing passkey. Always build menu's from main menu 
                during automation to avoid menu stack overflow.
08/19/05   ns   Fixed problem with Advanced Image Printing
08/17/05   dc   Added concurrency test automation support for BIP Client Image
                Printing 
07/27/05   ns   Added support to enable BIP server during startup
07/13/05   ns   Added concurrency test automation support for BIP push, pull,
                auto archive, remote camera, remote display, archived objects,
                referenced objects.
                Removed pushing menu in BTApp_BIPBuildServerSubMenu as its 
                already pushed during BTApp_BIPBuildServerMenu
                Automation events status not updated previously in case of BIP 
                function calls returning failure. Now updates done accordingly.
05/23/05   ns   Automated bonding procedure for BIP server.
05/11/05   gs   Make ourselves Discoverable when registered as Server.
04/05/05   dc   Added Optional field to BTApp_DiagBIPCliConExReqType to handle
                PIN code menu during connection.
                Modified BTApp_DiagBIPLoadClientMenu to initialize the BIP object.
03/25/05   pn   Made use of BTApp_SetBondable() and BTApp_ClearBondable().
02/10/05   gs   Fixed BREW heap corruption problem in GetImageList (Set 
                pXMLdata ptr to NULL after freeing)
01/14/05   gs   Updated partialOffset and partialLength values in 
                GetPartialImage.  Updated Archive and Printer responder menu.
01/11/05   gs   Fixed UI freeze problem during multiple connect-disconnects
01/10/05   dc   Added Support for BTApp_DiagBIPClientConnectEx to handle 
                OBEX Servers.
                Changed the function name BTApp_DiagBIPClientConnect to
                BTApp_DiagBIPClientConnectEx
                Added helper function BTApp_DiagBIPProcessOBEXMatch and 
                BTApp_DiagBIPSelectResponderType
01/05/05   gs   Various fixes/changes 
                - UI freezes while navigating up and down BIP client menu
                - Allow empty XML strings in Get and Put interfaces
                - Handle file-not-found conditions gracefully so that
                  UI is not stuck in hourglass screen
01/03/05   gs   Added support for selecting OBEX server to connect to
12/21/04   gs   Save image-handle that the Server returns in response to
                PutImage operation
12/21/04   gr   Fixed issue with clientconnect timeout
12/14/04   gr   Added automation support for PUSH OPS, PULL OPS, IMG PRN
                Automatic Archive, Remote Camera, Remote Display
                Arch/Ref Objects, Settings Menu
                Updated ClientConnect to handle PIN request
                Added BuildMenu support for BT_APP_MENU_BIP_SETTINGS
12/08/04   gr   Added support for automation of Client Connect, Disconnect
                Remote Display UI enhancements for Prev, Next, Current
11/30/04   gr   Added support to get Image count and recent Images
                Start archive added to Archived object sub menu
                Monitoring Images Save option added
                Added automation interfaces for Register, Deregister
08/24/04   pn   Correctly registers for BT notification (e.g. BT events.)

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp.h"
#include "BTAppUtils.h"

#ifdef FEATURE_BT_EXTPF_BIP
#include "AEEBTExtBIP.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#ifndef RELEASEIF
#define RELEASEIF(p) {if (NULL!=p) {(void)IBASE_Release((IBase*)(p));(p)=NULL;}}
#endif

/* Client Events */
#define EVT_BIP_CONNECTED                          1
#define EVT_BIP_CONN_FAILED                        2
#define EVT_BIP_DISCONNECTED                       3

#define EVT_BIP_GET_CAPABILITIES_SUCCESS          10
#define EVT_BIP_GET_CAPABILITIES_FAILED           11
#define EVT_BIP_PUT_IMAGE_SUCCESS                 12
#define EVT_BIP_PUT_IMAGE_FAILED                  13
#define EVT_BIP_PUT_ATTACHMENT_SUCCESS            14
#define EVT_BIP_PUT_ATTACHMENT_FAILED             15
#define EVT_BIP_PUT_THUMBNAIL_SUCCESS             16
#define EVT_BIP_PUT_THUMBNAIL_FAILED              17
#define EVT_BIP_GET_IMAGE_LIST_SUCCESS            18
#define EVT_BIP_GET_IMAGE_LIST_FAILED             19
#define EVT_BIP_GET_IMAGE_PROPERTIES_SUCCESS      20
#define EVT_BIP_GET_IMAGE_PROPERTIES_FAILED       21
#define EVT_BIP_GET_IMAGE_SUCCESS                 22
#define EVT_BIP_GET_IMAGE_FAILED                  23
#define EVT_BIP_GET_ATTACHMENT_SUCCESS            24
#define EVT_BIP_GET_ATTACHMENT_FAILED             25
#define EVT_BIP_GET_THUMBNAIL_SUCCESS             26
#define EVT_BIP_GET_THUMBNAIL_FAILED              27
#define EVT_BIP_DELETE_IMAGE_SUCCESS              28
#define EVT_BIP_DELETE_IMAGE_FAILED               29
#define EVT_BIP_GET_MONITORING_IMAGE_SUCCESS      30
#define EVT_BIP_GET_MONITORING_IMAGE_FAILED       31
#define EVT_BIP_REMOTE_DISPLAY_SUCCESS            32
#define EVT_BIP_REMOTE_DISPLAY_FAILED             33
#define EVT_BIP_START_ARCHIVE_SUCCESS             34
#define EVT_BIP_START_ARCHIVE_FAILED              35
#define EVT_BIP_START_PRINT_SUCCESS               36
#define EVT_BIP_START_PRINT_FAILED                37
#define EVT_BIP_GET_PARTIAL_IMAGE_SUCCESS         38
#define EVT_BIP_GET_PARTIAL_IMAGE_FAILED          39
#define EVT_BIP_GET_STATUS_SUCCESS                40
#define EVT_BIP_GET_STATUS_FAILED                 41

/* Server Events */
#define EVT_BIP_CONN_REQ                           4        
#define EVT_BIP_REG                                5
#define EVT_BIP_REG_FAILED                         6
#define EVT_BIP_DEREG                              7
#define EVT_BIP_DEREG_FAILED                       8

#define EVT_BIP_GET_CAPABILITIES_REQ_INITIAL      50
#define EVT_BIP_GET_CAPABILITIES_REQ_CLEANUP      51

#define EVT_BIP_PUT_IMAGE_REQ_INITIAL             52
#define EVT_BIP_PUT_IMAGE_REQ_CLEANUP             53

#define EVT_BIP_PUT_ATTACHMENT_REQ_INITIAL        54
#define EVT_BIP_PUT_ATTACHMENT_REQ_CLEANUP        55

#define EVT_BIP_PUT_THUMBNAIL_REQ_INITIAL         56
#define EVT_BIP_PUT_THUMBNAIL_REQ_CLEANUP         57

#define EVT_BIP_GET_IMAGE_LIST_REQ_INITIAL        58
#define EVT_BIP_GET_IMAGE_LIST_REQ_CLEANUP        59

#define EVT_BIP_GET_IMAGE_PROPERTIES_REQ_INITIAL  60
#define EVT_BIP_GET_IMAGE_PROPERTIES_REQ_CLEANUP  61

#define EVT_BIP_GET_IMAGE_REQ_INITIAL             62
#define EVT_BIP_GET_IMAGE_REQ_CLEANUP             63

#define EVT_BIP_GET_MONITORING_IMAGE_REQ_INITIAL  64
#define EVT_BIP_GET_MONITORING_IMAGE_REQ_CLEANUP  65

#define EVT_BIP_GET_PARTIAL_IMAGE_REQ_INITIAL     66
#define EVT_BIP_GET_PARTIAL_IMAGE_REQ_CLEANUP     67

#define EVT_BIP_GET_ATTACHMENT_REQ_INITIAL        68
#define EVT_BIP_GET_ATTACHMENT_REQ_CLEANUP        69

#define EVT_BIP_GET_THUMBNAIL_REQ_INITIAL         70
#define EVT_BIP_GET_THUMBNAIL_REQ_CLEANUP         71

#define EVT_BIP_DELETE_IMAGE_REQ_INITIAL          72
#define EVT_BIP_DELETE_IMAGE_REQ_CLEANUP          73

#define EVT_BIP_REMOTE_DISPLAY_REQ_INITIAL        74
#define EVT_BIP_REMOTE_DISPLAY_REQ_CLEANUP        75

#define EVT_BIP_START_ARCHIVE_REQ_INITIAL         76
#define EVT_BIP_START_ARCHIVE_REQ_CLEANUP         77

/* Client and Server Events */
#define EVT_BIP_AUTH_REQ                         100

/* Constants */
char*  szServerNameIR              = "QC BIP IR";
char*  szServerNameAO              = "QC BIP AO";
char*  szServerNameRO              = "QC BIP RO";

static char*  szICfileName         = "bip_xml1.txt";
static AECHAR wICfileName[]        = {'b','i','p','_','x','m','l','1','.','t','x','t',0};

static char*  szPIfileName         = "bip_c.jpg";
static AECHAR wPIfileName[]        = {'b','i','p','_','c','.','j','p','g',0};

static char*  szPIdescFileName     = "bip_xml2.txt";
static char*  szPAdescFileName     = "bip_xml4.txt";
static char*  szSPdpofFileName     = "bip_dpof.txt";

static char*  szPAfileName         = "bip_attachment_c.txt";
static AECHAR wPAfileName[]        = {'b','i','p','_','a','t','t','a','c','h','m','e','n','t','_','c','.','t','x','t',0};

static char*  szPTfileName         = "bip_c.tn.jpg";
static AECHAR wPTfileName[]        = {'b','i','p','_','c','.','t','n','.','j','p','g',0};

static char*  szIPfileName         = "bip_xml3.txt";
static AECHAR wIPfileName[]        = {'b','i','p','_','x','m','l','3','.','t','x','t',0};

static char*  szILfileName         = "bip_xml5.txt";
static AECHAR wILfileName[]        = {'b','i','p','_','x','m','l','5','.','t','x','t',0};

static char*  szGAfileName         = "bip_attachment_s.txt";
static AECHAR wGAfileName[]        = {'b','i','p','_','a','t','t','a','c','h','m','e','n','t','_','s','.','t','x','t',0};

static char*  szGIfileName         = "bip_s.jpg";
static AECHAR wGIfileName[]        = {'b','i','p','_','s','.','j','p','g',0};

static char*  szGTfileName         = "bip_s.tn.jpg";
static AECHAR wGTfileName[]        = {'b','i','p','_','s','.','t','n','.','j','p','g',0};

static char*  szMIfileName         = "bip_mi_s.jpg";
static AECHAR wMIfileName[]        = {'b','i','p','_','m','i','_','s','.','j','p','g',0};

/* Imaging responder SDP attributes */
#define BTAPP_BIP_IMG_RESPONDER_SUPPORTED_CAP       0x0F
#define BTAPP_BIP_IMG_RESPONDER_SUPPORTED_FEATURES  0x01FF
#define BTAPP_BIP_IMG_RESPONDER_SUPPORTED_FUNCTIONS 0x00016FFF
#define BTAPP_BIP_IMG_RESPONDER_CAPACITY            0x000000000000FFFF

/* Referenced Objects server SDP attributes */
#define BTAPP_BIP_REF_OBJECTS_SUPPORTED_FUNCTIONS   0x00001001

/* Archived Objects server SDP attributes */
#define BTAPP_BIP_ARCH_OBJECTS_SUPPORTED_FUNCTIONS  0x000007E1


extern uint32 uBTApp_NMask;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

/* ==========================================================================
FUNCTION BTApp_ProcessBIPNotifications
DESCRIPTION
============================================================================= */
void BTApp_ProcessBIPNotifications(
  CBTApp* pMe, 
  AEEEvent evt, 
  NotificationData* pData
)
{
  uint32 userEvent = 0;

  switch ( evt )
  {
    // Client and Server AEE Events
    case AEEBT_BIP_EVT_AUTH_REQ:  
    {
      BTAPP_POST_USER_EVENT( BIP, EVT_BIP_AUTH_REQ );
      break;
    }
    // Client AEE Events
    case AEEBT_BIP_EVT_CONNECTED:  
    {
      BTAPP_POST_USER_EVENT( BIP, EVT_BIP_CONNECTED );
      BTAppMgr_UpdateProfileType( pMe, &pMe->mBIP.remoteBDAddr, BTAPP_MGR_BIP, BTAPP_MGR_CONNECTED );
      break;
    }
    case AEEBT_BIP_EVT_CONN_FAILED: 
    {
      BTAPP_POST_USER_EVENT( BIP, EVT_BIP_CONN_FAILED );
      break;
    }
    case AEEBT_BIP_EVT_DISCONNECTED: 
    {
      if ( pData->BipObject.bServerEvt )
      {
        // Server event
        if ( pMe->mBIP.bRegistered == TRUE )
        {
          // Primary Server
          pMe->mBIP.bConnected = FALSE;
          pMe->mBIP.bAuthenticated = FALSE;
        }
        else
        {
          // Secondary server
          pMe->mBIP.bSecConnected = FALSE;
        }
      }
      else
      {
        // Client event
        if ( pMe->mBIP.bRegistered == TRUE ) 
        {
          // Secondary client
          pMe->mBIP.bSecConnected = FALSE;
        }
        else
        {
          // Primary client
          pMe->mBIP.bConnected = FALSE;
          pMe->mBIP.bAuthenticated = FALSE;
        }
      }
      BTAppMgr_UpdateProfileType( pMe, &pMe->mBIP.remoteBDAddr, BTAPP_MGR_BIP, BTAPP_MGR_DISCONNECTED );
      BTAPP_POST_USER_EVENT( BIP, EVT_BIP_DISCONNECTED );
      break;
    }
    case AEEBT_BIP_EVT_GET_CAPABILITIES_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_GET_CAPABILITIES_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_GET_CAPABILITIES_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_PUT_IMAGE_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_PUT_IMAGE_SUCCESS;

        // Save image handle info that the Server sent in response to 
        // PutImage operation
        WSTRLCPY( pMe->mBIP.wImageHandle, pData->BipObject.wImageHandle, 
                  ARR_SIZE( pMe->mBIP.wImageHandle ) );
      }
      else 
      {
        userEvent = EVT_BIP_PUT_IMAGE_FAILED;

        // Empty out image handle buffer
        STRTOWSTR( "", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_PUT_ATTACHMENT_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_PUT_ATTACHMENT_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_PUT_ATTACHMENT_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_PUT_THUMBNAIL_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_PUT_THUMBNAIL_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_PUT_THUMBNAIL_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_IMAGE_LIST_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_GET_IMAGE_LIST_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_GET_IMAGE_LIST_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_GET_IMAGE_PROPERTIES_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_GET_IMAGE_PROPERTIES_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_IMAGE_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_GET_IMAGE_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_GET_IMAGE_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_GET_PARTIAL_IMAGE_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_GET_PARTIAL_IMAGE_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_MONITORING_IMAGE_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_GET_MONITORING_IMAGE_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_GET_MONITORING_IMAGE_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_STATUS_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_GET_STATUS_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_GET_STATUS_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_ATTACHMENT_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_GET_ATTACHMENT_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_GET_ATTACHMENT_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_THUMBNAIL_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_GET_THUMBNAIL_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_GET_THUMBNAIL_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_DELETE_IMAGE_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_DELETE_IMAGE_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_DELETE_IMAGE_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_REMOTE_DISPLAY_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_REMOTE_DISPLAY_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_REMOTE_DISPLAY_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_START_ARCHIVE_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_START_ARCHIVE_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_START_ARCHIVE_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_START_PRINT_DONE:
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_START_PRINT_SUCCESS;
      }
      else 
      {
        userEvent = EVT_BIP_START_PRINT_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }

    // Server AEE Events
    case AEEBT_BIP_EVT_REG_DONE:  
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_REG;
        if ( pData->BipObject.bPriServer == TRUE ) 
        {
          pMe->mBIP.bRegistered = TRUE;
        }
        else
        {
          pMe->mBIP.bSecRegistered = TRUE;
        }
      }
      else 
      {
        userEvent = EVT_BIP_REG_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_DEREG_DONE: 
    {
      if ( pData->uError == AEEBT_BIP_ERR_NONE )
      {
        userEvent = EVT_BIP_DEREG;
        if (pData->BipObject.bPriServer == TRUE ) 
        {
          pMe->mBIP.bRegistered = FALSE;
        }
        else
        {
          pMe->mBIP.bSecRegistered = FALSE;
        }
      }
      else 
      {
        userEvent = EVT_BIP_DEREG_FAILED;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_CONN_REQ:  
    {
      BTAPP_POST_USER_EVENT( BIP, EVT_BIP_CONN_REQ );
      pMe->mBIP.remoteBDAddr = pData->bdAddr;
      break;
    }
    case AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_INITIAL:
    case AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_GET_CAPABILITIES_REQ_INITIAL;
      }
      else
      {
        userEvent = EVT_BIP_GET_CAPABILITIES_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_PUT_IMAGE_REQ_INITIAL:
    case AEEBT_BIP_EVT_PUT_IMAGE_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_PUT_IMAGE_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_PUT_IMAGE_REQ_INITIAL;

        // Copy client's XML descriptor string in a local buffer.
        STRLCPY( pMe->mBIP.clientXMLdata, pData->BipObject.ImageDescriptor, sizeof( pMe->mBIP.clientXMLdata ) );
      }
      else
      {
        userEvent = EVT_BIP_PUT_IMAGE_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_PUT_ATTACHMENT_REQ_INITIAL:
    case AEEBT_BIP_EVT_PUT_ATTACHMENT_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_PUT_ATTACHMENT_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_PUT_ATTACHMENT_REQ_INITIAL;
      }
      else
      {
        userEvent = EVT_BIP_PUT_ATTACHMENT_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_PUT_THUMBNAIL_REQ_INITIAL:
    case AEEBT_BIP_EVT_PUT_THUMBNAIL_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_PUT_THUMBNAIL_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_PUT_THUMBNAIL_REQ_INITIAL;
      }
      else
      {
        userEvent = EVT_BIP_PUT_THUMBNAIL_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_INITIAL:
    case AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_GET_IMAGE_LIST_REQ_INITIAL;

        // Copy XML descriptor string in a local buffer.  It will
        // be sent back in the response message
        STRLCPY( pMe->mBIP.clientXMLdata, pData->BipObject.ImageDescriptor, sizeof( pMe->mBIP.clientXMLdata ) );

        // Other parameters based on which the response message is
        // constructed
        pMe->mBIP.handleCount = pData->BipObject.handleCount;
        pMe->mBIP.startOffset = pData->BipObject.startOffset;
        pMe->mBIP.bLatest     = pData->BipObject.bLatest;
      }
      else
      {
        userEvent = EVT_BIP_GET_IMAGE_LIST_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_INITIAL:
    case AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_GET_IMAGE_PROPERTIES_REQ_INITIAL;
      }
      else
      {
        userEvent = EVT_BIP_GET_IMAGE_PROPERTIES_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_IMAGE_REQ_INITIAL:
    case AEEBT_BIP_EVT_GET_IMAGE_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_GET_IMAGE_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_GET_IMAGE_REQ_INITIAL;

        // Copy XML descriptor string in a local buffer.
        STRLCPY( pMe->mBIP.clientXMLdata, pData->BipObject.ImageDescriptor, 
                 sizeof( pMe->mBIP.clientXMLdata ) );

        // Store the image handle locally
        WSTRLCPY( pMe->mBIP.wImageHandle, pData->BipObject.wImageHandle, 
                  ARR_SIZE( pMe->mBIP.wImageHandle ) );
      }
      else
      {
        userEvent = EVT_BIP_GET_IMAGE_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_INITIAL:
    case AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_GET_PARTIAL_IMAGE_REQ_INITIAL;

        // Copy parameters to local variables
        pMe->mBIP.partialLength = pData->BipObject.partialLength;
        pMe->mBIP.partialOffset = pData->BipObject.partialOffset;
        WSTRLCPY( pMe->mBIP.wImageName, pData->BipObject.wImageName, 
                  ARR_SIZE( pMe->mBIP.wImageName ) );        
      }
      else
      {
        userEvent = EVT_BIP_GET_PARTIAL_IMAGE_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_INITIAL:
    case AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_GET_MONITORING_IMAGE_REQ_INITIAL;

        // Save store flag
        pMe->mBIP.bStoreFlag = pData->BipObject.bStoreFlag;
      }
      else
      {
        userEvent = EVT_BIP_GET_MONITORING_IMAGE_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_INITIAL:
    case AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_GET_ATTACHMENT_REQ_INITIAL;

        // Store the attachment name and image handle
        WSTRLCPY( pMe->mBIP.wAttachmentName, pData->BipObject.wAttachmentName, 
                  ARR_SIZE( pMe->mBIP.wAttachmentName ) );
        WSTRLCPY( pMe->mBIP.wImageHandle, pData->BipObject.wImageHandle, 
                  ARR_SIZE( pMe->mBIP.wImageHandle ) );
      }
      else
      {
        userEvent = EVT_BIP_GET_ATTACHMENT_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_INITIAL:
    case AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_GET_THUMBNAIL_REQ_INITIAL;

        // Store the image handle 
        WSTRLCPY( pMe->mBIP.wImageHandle, pData->BipObject.wImageHandle, 
                  ARR_SIZE( pMe->mBIP.wImageHandle ) );
      }
      else
      {
        userEvent = EVT_BIP_GET_THUMBNAIL_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_DELETE_IMAGE_REQ_INITIAL:
    case AEEBT_BIP_EVT_DELETE_IMAGE_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_DELETE_IMAGE_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_DELETE_IMAGE_REQ_INITIAL;
        // Store the image handle 
        WSTRLCPY( pMe->mBIP.wImageHandle, pData->BipObject.wImageHandle, 
                  ARR_SIZE( pMe->mBIP.wImageHandle ) );
      }
      else
      {
        userEvent = EVT_BIP_DELETE_IMAGE_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_INITIAL:
    case AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_REMOTE_DISPLAY_REQ_INITIAL;

        // Store the image handle and display command
        WSTRLCPY( pMe->mBIP.wImageHandle, pData->BipObject.wImageHandle, 
                  ARR_SIZE( pMe->mBIP.wImageHandle ) );
        pMe->mBIP.displayCommand = pData->BipObject.displayCommand;
      }
      else
      {
        userEvent = EVT_BIP_REMOTE_DISPLAY_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }
    case AEEBT_BIP_EVT_START_ARCHIVE_REQ_INITIAL:
    case AEEBT_BIP_EVT_START_ARCHIVE_REQ_CLEANUP:
    {
      if ( evt == AEEBT_BIP_EVT_START_ARCHIVE_REQ_INITIAL ) 
      {
        userEvent = EVT_BIP_START_ARCHIVE_REQ_INITIAL;

        // Store the serviceID to connect to 
        MEMCPY( (void*) &pMe->mBIP.serviceID, 
                (void*) &pData->BipObject.serviceID,
                sizeof ( pMe->mBIP.serviceID ) );
      }
      else
      {
        userEvent = EVT_BIP_START_ARCHIVE_REQ_CLEANUP;
      }
      BTAPP_POST_USER_EVENT( BIP, userEvent );
      break;
    }

    default:
      MSG_ERROR( "BIP - unexpected ev=%d", evt, 0, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPBuildMainMenu
DESCRIPTION
============================================================================= */
void BTApp_BIPBuildMainMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_BIP_TESTS, NULL );
  //Add By zzg 2010_11_01
  {
	  AECHAR WTitle[20] = {0};
	  ISHELL_LoadResString(pMe->a.m_pIShell,
		                   BTAPP_RES_FILE,                                
		                   IDS_BIP_TESTS,
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
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_SERVER, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_CLIENT, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SETTINGS, 0 );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_BIP_TESTS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BIPBuildServerMenu
DESCRIPTION
============================================================================= */
void BTApp_BIPBuildServerMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  /*
  if ( pMe->mBIP.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->a.m_pIShell, 
                          BTAPP_RES_FILE, 
                          IDS_BIP_SERVER, 
                          pMe->pText2, 
                          SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
    IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
  }
  else
  {
    IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_BIP_SERVER, NULL );
  }
  */

  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  if ( pMe->mBIP.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->a.m_pIShell, 
                          BTAPP_RES_FILE, 
                          IDS_BIP_SERVER, 
                          pMe->pText2, 
                          SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
    //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
	}
  }
  else
  {
    //IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_BIP_SERVER, NULL );
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->a.m_pIShell,
                         BTAPP_RES_FILE,                                
                         IDS_BIP_SERVER,
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
  if ( pMe->mBIP.bRegistered == FALSE && pMe->mBIP.bSecRegistered == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REGISTER, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEREGISTER, 0 );
  }
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_BIP_SERVER );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_BIPBuildClientSubMenu
DESCRIPTION
============================================================================= */
void BTApp_BIPBuildClientSubMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  //IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_BIP_CLIENT, NULL );
  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->a.m_pIShell,
                         BTAPP_RES_FILE,                                
                         IDS_BIP_CLIENT,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }
  //Add End

  BTApp_InitAddItem( &ai );

  if ( pMe->mBIP.bRegistered == FALSE )
  {
    // Primary client -- Connect menu
    if ( pMe->mBIP.bConnected == FALSE )
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_CONNECT_IMAGE_PUSH, 0 );
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_CONNECT_IMAGE_PULL, 0 );
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_CONNECT_AUTOMATIC_ARCHIVE, 0 );
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_CONNECT_IMAGE_PRINTING, 0 );
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_CONNECT_REMOTE_CAMERA, 0 );
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_CONNECT_REMOTE_DISPLAY, 0 );
    }
    else
    {
      // Connected menu
      if ( pMe->mBIP.responderType == AEEBT_BIP_TARGET_IMAGE_PUSH ) 
      {
        if ( pMe->mBIP.bSecConnected == FALSE ) 
        {
          // Cannot disconnect if sec connection is up
          BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
        }
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_CAPABILITIES, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_PUT_IMAGE, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_PUT_THUMBNAIL, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_PUT_ATTACHMENT, 0 );
      }
      else if ( pMe->mBIP.responderType == AEEBT_BIP_TARGET_IMAGE_PULL ) 
      {
        if ( pMe->mBIP.bSecConnected == FALSE ) 
        {
          // Cannot disconnect if sec connection is up
          BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
        }
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_CAPABILITIES, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_LIST, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_TOTAL, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_RECENT_IMAGE_LIST, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_PROPERTIES, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_THUMBNAIL, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_ATTACHMENT, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_DELETE_IMAGE, 0 );
      }
      else if ( pMe->mBIP.responderType == AEEBT_BIP_TARGET_IMAGE_PRINTING ) 
      {
        if ( pMe->mBIP.bSecConnected == FALSE ) 
        {
          // Cannot disconnect if sec connection is up
          BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
        }
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_CAPABILITIES, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_START_PRINT, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_STATUS, 0 );
      }
      else if ( pMe->mBIP.responderType == AEEBT_BIP_TARGET_AUTOMATIC_ARCHIVE ) 
      {
        if ( pMe->mBIP.bSecConnected == FALSE ) 
        {
          // Cannot disconnect if sec connection is up
          BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
        }
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_CAPABILITIES, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_LIST, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_TOTAL, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_RECENT_IMAGE_LIST, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_PROPERTIES, 0 ); 
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_THUMBNAIL, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_ATTACHMENT, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_DELETE_IMAGE, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_START_ARCHIVE, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_STATUS, 0 );
      }
      else if ( pMe->mBIP.responderType == AEEBT_BIP_TARGET_REMOTE_CAMERA ) 
      {
        if ( pMe->mBIP.bSecConnected == FALSE ) 
        {
          // Cannot disconnect if sec connection is up
          BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
        }
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_MONITORING_IMAGE, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_MONITORING_IMAGE_SAVE, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_PROPERTIES, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_THUMBNAIL, 0 );
      }
      else if ( pMe->mBIP.responderType == AEEBT_BIP_TARGET_REMOTE_DISPLAY ) 
      {
        if ( pMe->mBIP.bSecConnected == FALSE ) 
        {
          // Cannot disconnect if sec connection is up
          BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
        }
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_CAPABILITIES, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_LIST, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_TOTAL, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_RECENT_IMAGE_LIST, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_PUT_IMAGE, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_PUT_THUMBNAIL, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_REMOTE_DISPLAY, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_REMOTE_DISPLAY_CURRENT, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_REMOTE_DISPLAY_NEXT, 0 );
        BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_REMOTE_DISPLAY_PREV, 0 );
      }
    }
  }
  else if ( pMe->mBIP.bConnected == TRUE )
  {
    // Secondary client and primary connection exists
    if ( pMe->mBIP.bSecConnected == FALSE )
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_CONNECT_ARCHIVED_OBJECTS, 0 );
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_CONNECT_REFERENCED_OBJECTS, 0 );
    }
    else
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );
     // BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_PARTIAL_IMAGE, 0 );
     if (pMe->mBIP.responderType == AEEBT_BIP_TARGET_ARCHIVED_OBJECTS_SERVER)
      {
         BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_CAPABILITIES, 0 );
         BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_LIST, 0 );
         BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_TOTAL, 0 );
         BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_RECENT_IMAGE_LIST, 0 );
         BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE_PROPERTIES, 0 ); 
         BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_IMAGE, 0 );
         BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_THUMBNAIL, 0 );
         BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_ATTACHMENT, 0 );
         BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_DELETE_IMAGE, 0 );
      }
      else // must be AEEBT_BIP_TARGET_REFERENCED_OBJECTS_SERVER
      {
         BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_GET_PARTIAL_IMAGE, 0 );
      }
    }
  }
  else
  {
  // No Connect menu to display
  }
  
  // Activate menu
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_BIPBuildServerSubMenu
DESCRIPTION
============================================================================= */
void BTApp_BIPBuildServerSubMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  //IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_BIP_SERVER, NULL );
  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->a.m_pIShell,
                         BTAPP_RES_FILE,                                
                         IDS_BIP_SERVER,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }
  //Add End

  BTApp_InitAddItem( &ai );

  if ( pMe->mBIP.bRegistered == FALSE && pMe->mBIP.bConnected == FALSE ) 
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_REGISTER_IMAGING_RESPONDER, 0 );
  }

  if ( pMe->mBIP.bSecRegistered == FALSE ) 
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_REGISTER_ARCHIVED_OBJECTS_SERVER, 0 );
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_REGISTER_REFERENCED_OBJECTS_SERVER, 0);
  }
   
  // Activate menu
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_BIPBuildClientMenu
DESCRIPTION
============================================================================= */
void BTApp_BIPBuildClientMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  /*
  if ( pMe->mBIP.bConnected != FALSE && pMe->mBIP.bSecConnected == FALSE)
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->a.m_pIShell, 
                          BTAPP_RES_FILE, 
                          IDS_BIP_CLIENT, 
                          pMe->pText2, 
                          SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
    IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
  }
  else
  {
    IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_BIP_CLIENT, NULL );
  }
  */

  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  


  if ( pMe->mBIP.bConnected != FALSE && pMe->mBIP.bSecConnected == FALSE)
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->a.m_pIShell, 
                          BTAPP_RES_FILE, 
                          IDS_BIP_CLIENT, 
                          pMe->pText2, 
                          SHORT_TEXT_BUF_LEN * sizeof(AECHAR) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
    //IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, pMe->pText2);
	}
  }
  else
  {
    //IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_BIP_CLIENT, NULL );
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->a.m_pIShell,
                         BTAPP_RES_FILE,                                
                         IDS_BIP_CLIENT,
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
  if ( ( pMe->mBIP.bRegistered == FALSE && pMe->mBIP.bConnected == FALSE ) || 
       ( pMe->mBIP.bRegistered == TRUE && pMe->mBIP.bSecConnected == FALSE &&
         pMe->mBIP.bConnected == TRUE ) )
  {
    // We are primary client and a connection doesn't exist -or-
    // We are primary server and a primary connection already exists
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT, 0 );
  }
  else
  {
    BTApp_BIPBuildClientSubMenu( pMe );
  }
  // Reset the OP progress 
  if ( pMe->mBIP.bOpInProgress == TRUE )
  {
    pMe->mBIP.bOpInProgress = FALSE;
  }
  // Activate menu
  PUSH_MENU( BT_APP_MENU_BIP_CLIENT );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/* ==========================================================================
FUNCTION BTApp_BIPBuildSettingsMenu
DESCRIPTION
============================================================================= */
void BTApp_BIPBuildSettingsMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );

  BTApp_InitAddItem( &ai );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_SETTINGS, NULL );
  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->a.m_pIShell,
                         BTAPP_RES_FILE,                                
                         IDS_SETTINGS,
                         WTitle,
                         sizeof(WTitle));
 
    if(pMe->m_pIAnn != NULL)
	{
	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, WTitle);
	}
	
  }
  //Add End

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_USE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_BIP_TEST_PRINT, 0 );
  BTApp_AddMenuItem( pMe, 
                     pMe->m_pIMenu, 
                     &ai, 
                     IDS_AUTHENTICATE, 
                     pMe->mBIP.bDoAuthenticate ? IDB_CHECK_ON : IDB_CHECK_OFF );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_BIP_SETTINGS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION:    BTApp_BIPBuildEnableMenu
DESCRIPTION: Builds setting menu for BIP used during feature enable.
============================================================================ */
void BTApp_BIPBuildEnableMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );

  // set the title
  //IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_SETTINGS_BIP, NULL );
  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  
  {
    AECHAR WTitle[20] = {0};
	ISHELL_LoadResString(pMe->a.m_pIShell,
                         BTAPP_RES_FILE,                                
                         IDS_SETTINGS_BIP,
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
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, 
                     IDS_BIP_REGISTER_IMAGING_RESPONDER, 
                     (pMe->mBIP.serviceSelection == AEEBT_BIP_IMAGING_RESPONDER) ?
                     IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, 
                     IDS_BIP_REGISTER_ARCHIVED_OBJECTS_SERVER, 
                     (pMe->mBIP.serviceSelection == AEEBT_BIP_ARCHIVED_OBJECTS) ?
                     IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, 
                     IDS_BIP_REGISTER_REFERENCED_OBJECTS_SERVER, 
                     (pMe->mBIP.serviceSelection == AEEBT_BIP_REFERENCED_OBJECTS) ?
                     IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
                     
  // Activate menu
  PUSH_MENU( BT_APP_MENU_BIP_ENABLING );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );  
}

/* ==========================================================================
FUNCTION BTApp_BIPCleanup
DESCRIPTION
============================================================================= */
void BTApp_BIPCleanup( CBTApp* pMe )
{
  if ( pMe->mBIP.po != NULL )
  {
    IBTEXTBIP_Release( pMe->mBIP.po );
    pMe->mBIP.po = NULL;
  }
  if ( pMe->mBIP.pIFileMgr != NULL )
  {
    IFILEMGR_Release( pMe->mBIP.pIFileMgr );
    pMe->mBIP.pIFileMgr = NULL;
  }
  // unregister for BIP notification
  ISHELL_RegisterNotify( pMe->a.m_pIShell,  
                         AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 
                         0 );
  uBTApp_NMask &= ~NMASK_BT_BIP;
  ISHELL_RegisterNotify( pMe->a.m_pIShell,  
                         AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 
                         uBTApp_NMask );
}

/* ==========================================================================
FUNCTION BTApp_BIPGetCapabilities
DESCRIPTION
============================================================================= */
void BTApp_BIPGetCapabilities( CBTApp* pMe )
{
  // Allocate buffer space for storing the capabilities xml string that
  // the server sends back.
  pMe->mBIP.XMLdataMaxSize = 2000;
  if ( pMe->mBIP.pXMLdata != NULL ) 
  {
    FREE ( pMe->mBIP.pXMLdata );
    pMe->mBIP.pXMLdata = NULL;
  }

  if ( ( pMe->mBIP.pXMLdata = MALLOC( pMe->mBIP.XMLdataMaxSize) ) == NULL )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wICfileName, 3 );   
  }
  else if ( IBTEXTBIP_GetCapabilities( pMe->mBIP.po, 
                                       pMe->mBIP.pXMLdata, 
                                       pMe->mBIP.XMLdataMaxSize ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wICfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetImageList
DESCRIPTION
============================================================================= */
void BTApp_BIPGetImageList( CBTApp* pMe, uint16 sel )
{
  uint16 result = EBADPARM;

  pMe->mBIP.descLen = 0;

  if ( (pMe->mBIP.pIFile = 
        IFILEMGR_OpenFile( pMe->mBIP.pIFileMgr, 
                           szPIdescFileName, 
                           _OFM_READ )) != NULL )
  {
    // Read image descriptor info
    pMe->mBIP.descLen = 
      IFILE_Read( pMe->mBIP.pIFile, pMe->mBIP.imageDesc, 500 - 1 );

    IFILE_Release( pMe->mBIP.pIFile );
    pMe->mBIP.pIFile =  NULL;
  }
  else
  {
    // Send empty XML string
  }

  // Null terminate the string 
  pMe->mBIP.imageDesc[ pMe->mBIP.descLen ] = '\0';

  if ( pMe->mBIP.pXMLdata != NULL ) 
  {
    FREE ( pMe->mBIP.pXMLdata );
    pMe->mBIP.pXMLdata = NULL;
  }

  pMe->mBIP.handleCount = 0xFFFF;
  pMe->mBIP.startOffset = 0x0;
  pMe->mBIP.bLatest     = FALSE;

  // Allocate buffer space for storing the xml string that
  // the server sends back.
  pMe->mBIP.XMLdataMaxSize = 2000;
  if ( ( pMe->mBIP.pXMLdata = MALLOC( pMe->mBIP.XMLdataMaxSize ) ) == NULL )
  {
    // bad
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wILfileName, 3 );
  }
  else 
  {
    switch ( sel )
    {
      /* Get Image List */
      case IDS_BIP_GET_IMAGE_LIST:
      {
        result = IBTEXTBIP_GetImageList( pMe->mBIP.po, 
                                         pMe->mBIP.imageDesc,
                                         pMe->mBIP.handleCount,
                                         pMe->mBIP.startOffset,
                                         pMe->mBIP.bLatest,
                                         pMe->mBIP.pXMLdata, 
                                         pMe->mBIP.XMLdataMaxSize ); 
        break;
      }
      /* Get Total number of Images */
      case IDS_BIP_GET_IMAGE_TOTAL:
      {
        pMe->mBIP.handleCount = 0x0000;
        result = IBTEXTBIP_GetImageList( pMe->mBIP.po, 
                                         pMe->mBIP.imageDesc,
                                         pMe->mBIP.handleCount,
                                         pMe->mBIP.startOffset,
                                         pMe->mBIP.bLatest,
                                         pMe->mBIP.pXMLdata, 
                                         pMe->mBIP.XMLdataMaxSize ); 
        break;
      }
      /* Get Recent Image List */
      case IDS_BIP_GET_RECENT_IMAGE_LIST:
      {
        pMe->mBIP.bLatest = TRUE;
        result = IBTEXTBIP_GetImageList( pMe->mBIP.po, 
                                         pMe->mBIP.imageDesc,
                                         pMe->mBIP.handleCount,
                                         pMe->mBIP.startOffset,
                                         pMe->mBIP.bLatest,
                                         pMe->mBIP.pXMLdata, 
                                         pMe->mBIP.XMLdataMaxSize ); 
        break;
      }
      default:
      {
        result = EBADPARM;
        break;
      }
    }
    if ( result != SUCCESS )
    {
      BTApp_ShowMessage( pMe, IDS_MSG_OBJ_PULL_FAILED, NULL, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    }
    else
    {
      pMe->mBIP.bOpInProgress = TRUE;
      BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetImageProperties
DESCRIPTION
============================================================================= */
void BTApp_BIPGetImageProperties( CBTApp* pMe )
{
  STRTOWSTR( "1234567", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );

  if ( pMe->mBIP.pXMLdata != NULL ) 
  {
    FREE ( pMe->mBIP.pXMLdata );
    pMe->mBIP.pXMLdata = NULL;
  }

  // Allocate buffer space for storing the xml string that
  // the server sends back.
  pMe->mBIP.XMLdataMaxSize = 2000;
  if ( ( pMe->mBIP.pXMLdata = MALLOC( pMe->mBIP.XMLdataMaxSize ) ) == NULL )
  {
    // bad
    BTApp_ShowMessage( pMe, IDS_MSG_OBJ_PULL_FAILED, NULL, 3 );
  }
  else if ( IBTEXTBIP_GetImageProperties( pMe->mBIP.po, 
                                          pMe->mBIP.wImageHandle,
                                          pMe->mBIP.pXMLdata, 
                                          pMe->mBIP.XMLdataMaxSize ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wIPfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetImage
DESCRIPTION
============================================================================= */
void BTApp_BIPGetImage( CBTApp* pMe )
{
  STRTOWSTR( "1234567", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );
  pMe->mBIP.descLen = 0;

  if ( (pMe->mBIP.pIFile = 
        IFILEMGR_OpenFile( pMe->mBIP.pIFileMgr, 
                           szPIdescFileName, 
                           _OFM_READ )) != NULL )
  {
    // Read image descriptor info
    pMe->mBIP.descLen = 
      IFILE_Read( pMe->mBIP.pIFile, pMe->mBIP.imageDesc, 500 - 1 );

    IFILE_Release( pMe->mBIP.pIFile );
    pMe->mBIP.pIFile =  NULL;
  }
  else
  {
    // Send empty XML string
  }

  // Null terminate the string
  pMe->mBIP.imageDesc[ pMe->mBIP.descLen ] = '\0';

  if ( IBTEXTBIP_GetImage( pMe->mBIP.po, 
                           pMe->mBIP.wImageHandle,
                           pMe->mBIP.imageDesc,
                           szGIfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wGIfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetPartialImage
DESCRIPTION
============================================================================= */
void BTApp_BIPGetPartialImage( CBTApp* pMe )
{
  // Retrive entire image
  uint32 partialOffset = 0x00000000;
  uint32 partialLength = 0xFFFFFFFF;

  if ( IBTEXTBIP_GetPartialImage( pMe->mBIP.po, 
                                  wGIfileName,
                                  partialLength,
                                  partialOffset ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wGIfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetLinkedAttachment
DESCRIPTION
============================================================================= */
void BTApp_BIPGetLinkedAttachment( CBTApp* pMe )
{
  STRTOWSTR( "1234567", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );

  if ( IBTEXTBIP_GetLinkedAttachment( pMe->mBIP.po, 
                                      pMe->mBIP.wImageHandle,
                                      wGAfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wGAfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetLinkedThumbnail
DESCRIPTION
============================================================================= */
void BTApp_BIPGetLinkedThumbnail( CBTApp* pMe )
{
  STRTOWSTR( "1234567", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );

  if ( IBTEXTBIP_GetLinkedThumbnail( pMe->mBIP.po, 
                                     pMe->mBIP.wImageHandle,
                                     szGTfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wGTfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetMonitoringImage
DESCRIPTION
============================================================================= */
void BTApp_BIPGetMonitoringImage( CBTApp* pMe, uint16 sel )
{
  uint16 result = EBADPARM;
  boolean storeFlag = FALSE;

  switch ( sel )
  {
    /* No Save Option */
    case IDS_BIP_GET_MONITORING_IMAGE:
    {
      result = IBTEXTBIP_GetMonitoringImage( pMe->mBIP.po, 
                                             storeFlag,
                                             szMIfileName );
      break;
    }
    /* Save Option */
    case IDS_BIP_GET_MONITORING_IMAGE_SAVE:
    {
      storeFlag = TRUE;
      result = IBTEXTBIP_GetMonitoringImage( pMe->mBIP.po, 
                                             storeFlag,
                                             szMIfileName );
      break;
    }
    default:
    {
      result = EBADPARM;
      break;
    }
  }

  if ( result != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wMIfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    BTApp_ShowBusyIcon( pMe ); // wait for confirm
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetStatus
DESCRIPTION
============================================================================= */
void BTApp_BIPGetStatus( CBTApp* pMe )
{
  if ( IBTEXTBIP_GetStatus( pMe->mBIP.po ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_GET_STATUS_FAILED, NULL, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPDeleteImage
DESCRIPTION
============================================================================= */
void BTApp_BIPDeleteImage( CBTApp* pMe )
{
  STRTOWSTR( "1234567", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );
  pMe->mBIP.descLen = 0;

  if ( IBTEXTBIP_DeleteImage( pMe->mBIP.po, 
                              pMe->mBIP.wImageHandle ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_DELETE_FAILED, wGIfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    BTApp_ShowBusyIcon( pMe ); // wait for confirm
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPStartArchive
DESCRIPTION
============================================================================= */
void BTApp_BIPStartArchive( CBTApp* pMe )
{
  AEEBTBIPSDUUID128type serviceID = 
      { 0xC0, 0x26, 0x01, 0x94, 0xD5, 0x11, 0x08, 0x46, 
        0x02, 0x00, 0x1A, 0x84, 0x4E, 0x5B, 0x32, 0xA4 };

  if ( IBTEXTBIP_StartArchive( pMe->mBIP.po, 
                               &serviceID ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_START_ARCHIVE_FAILED, NULL, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPStartPrint
DESCRIPTION
============================================================================= */
void BTApp_BIPStartPrint( CBTApp* pMe )
{
  /* 
   * Basic Imaging Advanced Image Printing is 
   * 92353350-4608-11D5-841A-0002A5325B4E
   */
  AEEBTBIPSDUUID128type serviceID = 
      { 0x50, 0x33, 0x35, 0x92, 0x46, 0x08, 0x11, 0xD5,
        0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E };

  if ( IBTEXTBIP_StartPrint( pMe->mBIP.po, 
                             &serviceID,
                             szSPdpofFileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_START_PRINT_FAILED, NULL, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPRemoteDisplay
DESCRIPTION
============================================================================= */
void BTApp_BIPRemoteDisplay( CBTApp* pMe, uint16 sel )
{
  AEEBTBIPRDcmd displayCommand = AEEBT_BIP_REMOTE_DISPLAY_SELECT_IMAGE_CMD;
  AECHAR* imageHandle = NULL;

  /* Display command is Select */
  if ( sel == IDS_BIP_REMOTE_DISPLAY ) 
  {
    STRTOWSTR( "1234567", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );
    imageHandle = pMe->mBIP.wImageHandle;
  }
  /* Display command is current - Img Handle = NULL */
  else if ( sel == IDS_BIP_REMOTE_DISPLAY_CURRENT )
  {
    displayCommand = AEEBT_BIP_REMOTE_DISPLAY_CURRENT_IMAGE_CMD;
  }
  /* Display command is Prev  - Img Handle = NULL */
  else if ( sel == IDS_BIP_REMOTE_DISPLAY_PREV )
  {
    displayCommand = AEEBT_BIP_REMOTE_DISPLAY_PREVIOUS_IMAGE_CMD;
  }
  /* Display command is Next  - Img Handle = NULL */
  else if ( sel == IDS_BIP_REMOTE_DISPLAY_NEXT )
  {
    displayCommand = AEEBT_BIP_REMOTE_DISPLAY_NEXT_IMAGE_CMD;
  }

  if ( IBTEXTBIP_RemoteDisplay( pMe->mBIP.po, 
                                imageHandle,
                                displayCommand ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_REMOTE_DISPLAY_FAILED, NULL, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    BTApp_ShowBusyIcon( pMe ); // wait for confirm
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPPutImage
DESCRIPTION
============================================================================= */
void BTApp_BIPPutImage( CBTApp* pMe )
{
  pMe->mBIP.descLen = 0;

  if ( (pMe->mBIP.pIFile = 
        IFILEMGR_OpenFile( pMe->mBIP.pIFileMgr, 
                           szPIdescFileName, 
                           _OFM_READ )) != NULL )
  {
    // Read image descriptor info
    pMe->mBIP.descLen = 
      IFILE_Read( pMe->mBIP.pIFile, pMe->mBIP.imageDesc, 500 - 1 );

    IFILE_Release( pMe->mBIP.pIFile );
    pMe->mBIP.pIFile =  NULL;
  }
  else
  {
    // Send empty XML descriptor string
  }

  // Null terminate the string 
  pMe->mBIP.imageDesc[ pMe->mBIP.descLen ] = '\0';

  if ( IBTEXTBIP_PutImage( pMe->mBIP.po, 
                           pMe->mBIP.imageDesc,
                           szPIfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wPIfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPPrintImage
DESCRIPTION
============================================================================= */
void BTApp_BIPPrintImage( CBTApp* pMe )
{
  MSG_LOW( "BIPPrintImage - Printing Image", 0, 0, 0 );
  
   /* printing initiated from another APP */
  pMe->mBIP.descLen = 0;
  if ( (pMe->mBIP.pIFile = 
        IFILEMGR_OpenFile( pMe->mBIP.pIFileMgr, 
                          pMe->mBIP.printDetails.pFileDescName, 
                          _OFM_READ )) != NULL )
  {
    // Read image descriptor info
    pMe->mBIP.descLen = 
      IFILE_Read( pMe->mBIP.pIFile, pMe->mBIP.imageDesc, 500 - 1 );
    IFILE_Release( pMe->mBIP.pIFile );
    pMe->mBIP.pIFile =  NULL;
  }
  else
  {
    // Send empty XML descriptor string
    MSG_LOW( "BIPPrintImage - Image desc file cannot be opened", 0, 0, 0 );
  }
  // Null terminate the string 
  pMe->mBIP.imageDesc[ pMe->mBIP.descLen ] = NULL;
  if ( IBTEXTBIP_PutImage( pMe->mBIP.po, 
                           pMe->mBIP.imageDesc,
                           pMe->mBIP.printDetails.pPrintFileName 
                           ) != SUCCESS )
  {
    MSG_LOW( "BIPPrintImage - Failure in printing Image", 0, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, NULL, 3 );
  }
  return;
}

/* ==========================================================================
FUNCTION BTApp_BIPRemotePrintImage
DESCRIPTION
============================================================================= */
boolean BTApp_BIPRemotePrintImage( CBTApp* pMe, uint32 dwParam )
{
  BTApp_BIPPrintDetails* printDetails;
 
  MSG_LOW( "BIP - Printing initiated from another APP", 0, 0, 0 );
  /* store the print details */
  printDetails = (BTApp_BIPPrintDetails*)dwParam;
  pMe->mBIP.printDetails.pPrintFileName = 
    MALLOC( STRLEN(printDetails->pPrintFileName) + 1 );
  pMe->mBIP.printDetails.pFileDescName = 
    MALLOC( STRLEN(printDetails->pFileDescName) + 1 );
  if ( (pMe->mBIP.printDetails.pPrintFileName == NULL) ||
       (pMe->mBIP.printDetails.pFileDescName == NULL) )
  {
    MSG_ERROR( "HandleEvent - Memory allocation failure", 0, 0, 0 );
    return FALSE;
  }
  MEMCPY( pMe->mBIP.printDetails.pPrintFileName, 
          printDetails->pPrintFileName,
          STRLEN(printDetails->pPrintFileName) );
  MEMCPY( pMe->mBIP.printDetails.pFileDescName, 
          printDetails->pFileDescName, 
          STRLEN(printDetails->pFileDescName) );
  /* terminate with a null charracter */
  pMe->mBIP.printDetails.pPrintFileName[ 
    STRLEN(printDetails->pPrintFileName) ] = NULL;
  pMe->mBIP.printDetails.pFileDescName[ 
    STRLEN(printDetails->pFileDescName) ] = NULL;
  pMe->mBIP.bPrintFile = TRUE;
  /* free memory received */
  FREEIF( printDetails->pPrintFileName );
  FREEIF( printDetails->pFileDescName );
  FREEIF( printDetails );

  /* connect with printer if not connected and then send the file */
  if ( pMe->mBIP.bConnected == FALSE )
  {
    AEEBTBIPSDUUID128type pushTargetUUID = 
      { 0xE3, 0x3D, 0x95, 0x45, 0x83, 0x74, 0x4A, 0xD7, 
        0x9E, 0xC5, 0xC1, 0x6B, 0xE3, 0x1E, 0xDE, 0x8E };
    BTApp_SetBondable( pMe );

	
    if ( IBTEXTBIP_Connect( 
           pMe->mBIP.po, 
           &pMe->mBIP.printerBDAddr,
           0x0,
           &pushTargetUUID ) != SUCCESS )
    {
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      pMe->mBIP.bPrintFile = FALSE;
      FREEIF( pMe->mBIP.printDetails.pPrintFileName );
      FREEIF( pMe->mBIP.printDetails.pFileDescName );
      return FALSE;
    }
    else
    {
      pMe->mBIP.responderType = AEEBT_BIP_TARGET_IMAGE_PUSH;
    }
  }
  else
  {
    /* already connected, print file */
    BTApp_BIPPrintImage( pMe );
    pMe->mBIP.bPrintFile = FALSE;    
    FREEIF( pMe->mBIP.printDetails.pPrintFileName );
    FREEIF( pMe->mBIP.printDetails.pFileDescName );
  }
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_BIPPutLinkedAttachment
DESCRIPTION
============================================================================= */
void BTApp_BIPPutLinkedAttachment( CBTApp* pMe )
{
  pMe->mBIP.descLen = 0;

  if ( (pMe->mBIP.pIFile = 
        IFILEMGR_OpenFile( pMe->mBIP.pIFileMgr, 
                           szPAdescFileName, 
                           _OFM_READ )) != NULL )
  {
    // Read attachment descriptor info
    pMe->mBIP.descLen = 
      IFILE_Read( pMe->mBIP.pIFile, pMe->mBIP.attachmentDesc, 500 - 1 );

    IFILE_Release( pMe->mBIP.pIFile );
    pMe->mBIP.pIFile =  NULL;
  }
  else
  {
    // Send empty XML descriptor string
  }

  // Null terminate the string 
  pMe->mBIP.imageDesc[ pMe->mBIP.descLen ] = '\0';

  STRTOWSTR( "1234567", 
             pMe->mBIP.wImageHandle, 
             sizeof (pMe->mBIP.wImageHandle) );

  if ( IBTEXTBIP_PutLinkedAttachment( pMe->mBIP.po, 
                                      pMe->mBIP.wImageHandle,
                                      pMe->mBIP.attachmentDesc,
                                      szPAfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wPAfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPPutLinkedThumbnail
DESCRIPTION
============================================================================= */
void BTApp_BIPPutLinkedThumbnail( CBTApp* pMe )
{
  if ( WSTRLEN( pMe->mBIP.wImageHandle ) == 0 )
  {
    STRTOWSTR( "1234567", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );
  }

  if ( IBTEXTBIP_PutLinkedThumbnail( pMe->mBIP.po, 
                                     pMe->mBIP.wImageHandle,
                                     szPTfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wPTfileName, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
    pMe->mBIP.bOpInProgress = TRUE;
    BTApp_ShowMessage( pMe, IDS_MSG_BIP_OP_PROGRESS, NULL, 0 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPInit
DESCRIPTION
============================================================================= */
boolean BTApp_BIPInit( CBTApp* pMe )
{
  static boolean init_done = FALSE;
  uint32 uNMask = NMASK_BT_BIP | (((uint32)(NOTIFIER_VAL_ANY)) << 16);

  if ( init_done == FALSE )
  {
    if ( (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_BIP, 
                                 (void**)&pMe->mBIP.po ) == SUCCESS) &&
         (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, 
                                 (void **)&pMe->mBIP.pIFileMgr ) == SUCCESS) &&
         (ISHELL_RegisterNotify( pMe->a.m_pIShell,  AEECLSID_BLUETOOTH_APP,
                                 AEECLSID_BLUETOOTH_NOTIFIER, 
                                 uNMask ) == SUCCESS) )
    {
      uBTApp_NMask |= NMASK_BT_BIP;
      init_done = TRUE;
    }
  }
  return init_done;
}

/* ==========================================================================
FUNCTION BTApp_BIPGetCapabilitiesResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPGetCapabilitiesResponse( CBTApp* pMe )
{
  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    if ( pMe->mBIP.pXMLdata ) 
    {
      FREE ( pMe->mBIP.pXMLdata );
      pMe->mBIP.pXMLdata   = NULL;
    }

    if ( pMe->mBIP.pIFile ) 
    {
      IFILE_Release( pMe->mBIP.pIFile );
      pMe->mBIP.pIFile = NULL;
    }

    pMe->mBIP.bCleanup = FALSE;
   // BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_SUCCESS, wICfileName, 3 );
    return;
  }

  if ( ( pMe->mBIP.pIFile = 
         IFILEMGR_OpenFile( pMe->mBIP.pIFileMgr, 
                            szICfileName, 
                            _OFM_READ )) != NULL )
  {
    // Read file info (size etc)
    IFILE_GetInfo( pMe->mBIP.pIFile, &pMe->mBIP.fileInfo );
    pMe->mBIP.XMLdataLen = pMe->mBIP.fileInfo.dwSize;
  }
  else
  {
    // Send empty XML string since the source file is missing.
    pMe->mBIP.XMLdataLen = 0;
  }

  // Allocate buffer space (free if already allocated)
  if ( pMe->mBIP.pXMLdata ) 
  {
    FREE ( pMe->mBIP.pXMLdata );
    pMe->mBIP.pXMLdata = NULL;
  }

  if ( (pMe->mBIP.pXMLdata = MALLOC( pMe->mBIP.XMLdataLen + 1 ) ) == NULL )
  {
    // Don't expect this to happen or else client app could 
    // get stuck in hourglass screen.
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wICfileName, 3 );
    return;
  }

  if ( pMe->mBIP.XMLdataLen > 0 ) 
  {
    // Read imaging capabilities info
    if ( IFILE_Read( pMe->mBIP.pIFile, 
                     pMe->mBIP.pXMLdata,
                     pMe->mBIP.XMLdataLen ) != pMe->mBIP.XMLdataLen )
    {
      BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wICfileName, 3 );
      return;
    }
    IFILE_Release( pMe->mBIP.pIFile );
    pMe->mBIP.pIFile =  NULL;
  }

  // Null terminate the string since we are passing it in as char buffer
  // without giving the length information explicitly
  pMe->mBIP.pXMLdata[ pMe->mBIP.XMLdataLen ] = '\0';

  if ( IBTEXTBIP_GetCapabilitiesResponse( 
                           pMe->mBIP.po, 
                           pMe->mBIP.pXMLdata ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wICfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPPutImageResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPPutImageResponse( CBTApp* pMe )
{
  // Whether to get the thumbnail version of the image following
  // the image transfer
  boolean bGetThumbnail = FALSE;

  // Image handle
  STRTOWSTR( "1234567", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );

  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  if ( IBTEXTBIP_PutImageResponse( 
                           pMe->mBIP.po, 
                           pMe->mBIP.wImageHandle,
                           bGetThumbnail ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wPIfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPPutLinkedAttachmentResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPPutLinkedAttachmentResponse( CBTApp* pMe )
{
  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  if ( IBTEXTBIP_PutLinkedAttachmentResponse( 
                           pMe->mBIP.po,
                           szPAfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wPAfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPPutLinkedThumbnailResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPPutLinkedThumbnailResponse( CBTApp* pMe )
{
  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  if ( IBTEXTBIP_PutLinkedThumbnailResponse( 
                           pMe->mBIP.po,
                           szPTfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PULL_FAILED, wPTfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetImagePropertiesResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPGetImagePropertiesResponse( CBTApp* pMe )
{
  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    if ( pMe->mBIP.pXMLdata ) 
    {
      FREE ( pMe->mBIP.pXMLdata );
      pMe->mBIP.pXMLdata   = NULL;
      pMe->mBIP.XMLdataLen = 0;
    }

    if ( pMe->mBIP.pIFile ) 
    {
      IFILE_Release( pMe->mBIP.pIFile );
      pMe->mBIP.pIFile = NULL;
    }

    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  if ( (pMe->mBIP.pIFile = 
        IFILEMGR_OpenFile( pMe->mBIP.pIFileMgr, 
                           szIPfileName, 
                           _OFM_READ )) != NULL )
  {
    // Read file info (size etc)
    IFILE_GetInfo( pMe->mBIP.pIFile, &pMe->mBIP.fileInfo );
    pMe->mBIP.XMLdataLen = pMe->mBIP.fileInfo.dwSize;
  }
  else
  {
    // Send empty XML string since the source file is missing
    pMe->mBIP.XMLdataLen = 0;
  }

  // Allocate buffer space (free if already allocated)
  if ( pMe->mBIP.pXMLdata ) 
  {
    FREE ( pMe->mBIP.pXMLdata );
    pMe->mBIP.pXMLdata = NULL;
  }

  if ( (pMe->mBIP.pXMLdata = MALLOC( pMe->mBIP.XMLdataLen + 1) ) == NULL )
  {
    // Hopefully, this won't happen.
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wIPfileName, 3 );
    return;
  }

  if ( pMe->mBIP.XMLdataLen > 0 ) 
  {
    // Read imaging capabilities info
    if ( IFILE_Read( pMe->mBIP.pIFile, 
                     pMe->mBIP.pXMLdata,
                     pMe->mBIP.XMLdataLen ) != pMe->mBIP.XMLdataLen )
    {
      BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wIPfileName, 3 );
      return;
    }
    IFILE_Release( pMe->mBIP.pIFile );
    pMe->mBIP.pIFile =  NULL;
  }

  // Null terminate the string 
  pMe->mBIP.pXMLdata[ pMe->mBIP.XMLdataLen ] = '\0';

  if ( IBTEXTBIP_GetImagePropertiesResponse( 
                           pMe->mBIP.po, 
                           pMe->mBIP.pXMLdata ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wIPfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetImageListResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPGetImageListResponse( CBTApp* pMe )
{
  // Number of image handles 
  uint16 numHandles = 0;

  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    if ( pMe->mBIP.pXMLdata ) 
    {
      FREE ( pMe->mBIP.pXMLdata );
      pMe->mBIP.pXMLdata   = NULL;
      pMe->mBIP.XMLdataLen = 0;
    }

    if ( pMe->mBIP.pIFile ) 
    {
      IFILE_Release( pMe->mBIP.pIFile );
      pMe->mBIP.pIFile = NULL;
    }

    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  pMe->mBIP.XMLdataLen = 0;

  if ((SUCCESS != IFILEMGR_GetInfo( pMe->mBIP.pIFileMgr, szILfileName,
                                    &pMe->mBIP.fileInfo )) ||
      ((pMe->mBIP.pIFile = IFILEMGR_OpenFile( pMe->mBIP.pIFileMgr, 
                                              szILfileName, 
                                              _OFM_READ )) == NULL) ||
      (NULL == (pMe->mBIP.pXMLdata = MALLOC( pMe->mBIP.fileInfo.dwSize + 1 ))) ||
      (IFILE_Read( pMe->mBIP.pIFile, 
                   pMe->mBIP.pXMLdata,
                   pMe->mBIP.fileInfo.dwSize ) != pMe->mBIP.fileInfo.dwSize))
  {
    MSG_ERROR( "BIPGetImageListRsp - piFile=0x%x pData=0x%x", 
               pMe->mBIP.pIFile, pMe->mBIP.pXMLdata, 0 );
    // Not much we can do except send empty XML strings
  }
  else
  {
    char* pszTemp = pMe->mBIP.pXMLdata;
    const char szTag[] = "image handle";
    int len = pMe->mBIP.fileInfo.dwSize;

    // find out how many handles the list contains
    while ( pszTemp != NULL )
    {
      pszTemp = MEMSTR( pszTemp, szTag, len );
      if ( pszTemp != NULL )
      {
        numHandles++;
        pszTemp += sizeof(szTag);
        len = pMe->mBIP.fileInfo.dwSize - (pszTemp - pMe->mBIP.pXMLdata);
      }
    }
    MSG_LOW( "BIPGetImageListRsp - found %d handles", numHandles, 0, 0 );

    if ( pMe->mBIP.handleCount != 0 )
    {
      pMe->mBIP.XMLdataLen = pMe->mBIP.fileInfo.dwSize;
    }
    // else, client has requested only the number of image handles to be returned.
    // Send empty image list XML string
  }
  if ( pMe->mBIP.pXMLdata != NULL )
  {
    pMe->mBIP.pXMLdata[ pMe->mBIP.XMLdataLen ] = '\0';
  }
  RELEASEIF(pMe->mBIP.pIFile);

  // Send clients image descriptor filter as is in the response message
  // (need not be the case)
  if ( IBTEXTBIP_GetImageListResponse( 
                           pMe->mBIP.po, 
                           pMe->mBIP.pXMLdata,
                           pMe->mBIP.clientXMLdata,
                           numHandles ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wILfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetImageResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPGetImageResponse( CBTApp* pMe )
{
  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  if ( IBTEXTBIP_GetImageResponse( pMe->mBIP.po, 
                                   szGIfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wGIfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetMonitoringImageResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPGetMonitoringImageResponse( CBTApp* pMe )
{
  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  // Image handle
  STRTOWSTR( "1234567", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );

  if ( IBTEXTBIP_GetMonitoringImageResponse( pMe->mBIP.po,
                                             pMe->mBIP.wImageHandle,
                                             szMIfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wMIfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetPartialImageResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPGetPartialImageResponse( CBTApp* pMe )
{
  char sImageName[ AEEBT_MAX_FILE_NAME + 1 ];

  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }
  WSTRTOSTR( pMe->mBIP.wImageName, sImageName, sizeof( sImageName ) );
  if ( IBTEXTBIP_GetPartialImageResponse( 
                             pMe->mBIP.po,
                             sImageName,
                             pMe->mBIP.partialLength,
                             pMe->mBIP.partialOffset ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, pMe->mBIP.wImageName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetLinkedAttachmentResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPGetLinkedAttachmentResponse( CBTApp* pMe )
{
  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  if ( IBTEXTBIP_GetLinkedAttachmentResponse( 
                             pMe->mBIP.po,
                             szGAfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wGAfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPGetLinkedThumbnailResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPGetLinkedThumbnailResponse( CBTApp* pMe )
{
  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  if ( IBTEXTBIP_GetLinkedThumbnailResponse( 
                             pMe->mBIP.po,
                             szGTfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_PUSH_FAILED, wGTfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPDeleteImageResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPDeleteImageResponse( CBTApp* pMe )
{
  boolean bSuccess = TRUE;

  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  // Delete the image file (corrsponding to the image handle that the
  // client sent)
  if ( IFILEMGR_Remove( pMe->mBIP.pIFileMgr, szGIfileName ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_DELETE_FAILED, wGIfileName, 3 );
    bSuccess = FALSE;
  }

  if ( IBTEXTBIP_DeleteImageResponse( pMe->mBIP.po,
                                      bSuccess ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_DELETE_FAILED, wGIfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPRemoteDisplayResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPRemoteDisplayResponse( CBTApp* pMe )
{
  boolean bSuccess = TRUE;

  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  // Check if the file exists (corresponding to the image handle that the
  // client sent).  Additionally, the app should take appropriate action
  // based on the display command that the client sent.
  if ( (pMe->mBIP.pIFile = 
        IFILEMGR_OpenFile( pMe->mBIP.pIFileMgr, 
                           szGIfileName, 
                           _OFM_READ )) == NULL )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_REMOTE_DISPLAY_FAILED, wGIfileName, 3 );
    bSuccess = FALSE;
  }
  else
  {
    IFILE_Release( pMe->mBIP.pIFile );
    pMe->mBIP.pIFile = NULL;
  }

  if (pMe->mBIP.displayCommand != AEEBT_BIP_REMOTE_DISPLAY_SELECT_IMAGE_CMD) 
  {
    // Hacked handle
    STRTOWSTR( "1234567", pMe->mBIP.wImageHandle, sizeof (pMe->mBIP.wImageHandle) );
  }

  if ( IBTEXTBIP_RemoteDisplayResponse( pMe->mBIP.po,
                                        pMe->mBIP.wImageHandle,
                                        bSuccess ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_REMOTE_DISPLAY_FAILED, wGIfileName, 3 );
  }
}

/* ==========================================================================
FUNCTION BTApp_BIPStartArchiveResponse
DESCRIPTION
============================================================================= */
void BTApp_BIPStartArchiveResponse( CBTApp* pMe )
{
  boolean bSuccess = TRUE;

  if ( pMe->mBIP.bCleanup == TRUE) 
  {
    pMe->mBIP.bCleanup = FALSE;
    return;
  }

  if ( IBTEXTBIP_StartArchiveResponse( pMe->mBIP.po,
                                       bSuccess ) != SUCCESS )
  {
    BTApp_ShowMessage( pMe, IDS_BIP_MSG_REMOTE_DISPLAY_FAILED, NULL, 3 );
  }
  // At this point, establish a secondary OBEX connection to the 
  // ArchivedObject service using the serviceID provided by the client.  
  // Subsequently, call GetPartialImage to retrive the images.
}

/* ==========================================================================
FUNCTION BTApp_BIPSaveXMLstring
DESCRIPTION
============================================================================= */
static void BTApp_BIPSaveXMLstring( CBTApp* pMe, const char* szfileName )
{
  // Remove target file
  IFILEMGR_Remove( pMe->mBIP.pIFileMgr, szfileName );

  if ( (pMe->mBIP.pXMLdata != NULL) &&
       (pMe->mBIP.pIFile = IFILEMGR_OpenFile( pMe->mBIP.pIFileMgr, 
                                              szfileName, 
                                              _OFM_CREATE )) != NULL )
  {
    // Save XML string in a file
    IFILE_Write( pMe->mBIP.pIFile, pMe->mBIP.pXMLdata, 
                 STRLEN( pMe->mBIP.pXMLdata ) );
  }
  RELEASEIF( pMe->mBIP.pIFile );
  FREEIF( pMe->mBIP.pXMLdata );
}

/* ==========================================================================
FUNCTION BTApp_BIPBuildMenu
DESCRIPTION
============================================================================= */
boolean BTApp_BIPBuildMenu( CBTApp* pMe, BTAppMenuType menu )
{
  boolean built = TRUE;
  switch ( menu)
  {
    case BT_APP_MENU_BIP_TESTS:
    {
      if ( BTApp_BIPInit( pMe ) != FALSE )
      {
        BTApp_BIPBuildMainMenu( pMe );
      }
      else
      {
        MSG_ERROR( "BIPBuildMenu - failed to create BIP object", 0, 0, 0 );
        BTApp_BIPCleanup( pMe );
        built = FALSE;
      }
      break;
    }
    case BT_APP_MENU_BIP_SERVER:
      BTApp_BIPBuildServerMenu( pMe );
      break;
    case BT_APP_MENU_BIP_CLIENT:
      BTApp_BIPBuildClientMenu( pMe );
      break;
    case BT_APP_MENU_BIP_SETTINGS:
      BTApp_BIPBuildSettingsMenu( pMe );
      break;
    default:
      built = FALSE;
      break;
  }
  return built;
}

/* ==========================================================================
FUNCTION BTApp_BIPHandleSelection
DESCRIPTION
============================================================================= */
static boolean BTApp_BIPHandleSelection( CBTApp* pMe, uint16 sel )
{
  boolean key_handled = TRUE;
  uint16  selection;
  boolean bRegistered = FALSE;

  switch ( sel )
  {
    case IDS_BIP_SERVER:
    {
      BTApp_BIPBuildServerMenu( pMe );
      break;
    }
    case IDS_BIP_CLIENT:
    {
      BTApp_BIPBuildClientMenu( pMe );
      break;
    }
    case IDS_SETTINGS:
    {
      BTApp_BIPBuildSettingsMenu( pMe );
      break;
    }
    case IDS_USE:
    {
      key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_OBEX_SERVERS );
      break;
    }
    case IDS_BIP_TEST_PRINT:
    {
      BTApp_BIPPrintDetails* printDetails;
      int nNameLen = STRLEN( szPIfileName ) + 1;
      int nDescLen = STRLEN( szPIdescFileName ) + 1;
      if ( (SUCCESS == ERR_MALLOCREC( BTApp_BIPPrintDetails, &printDetails )) &&
           (SUCCESS == ERR_MALLOC( nNameLen,
                                   &printDetails->pPrintFileName )) &&
           (SUCCESS == ERR_MALLOC( nDescLen,
                                   &printDetails->pFileDescName )) )
      {
        STRLCPY( printDetails->pPrintFileName, szPIfileName, nNameLen );
        STRLCPY( printDetails->pFileDescName, szPIdescFileName, nDescLen );
        ISHELL_PostEventEx( pMe->a.m_pIShell, EVTFLG_ASYNC, 
                            AEECLSID_BLUETOOTH_APP,
                            EVT_USER, EVT_BIP_PUT_IMAGE, (uint32)printDetails );
      }
      else
      {
        MSG_ERROR( "BIPHandleSelection : Mem Alloc Failure", 0, 0, 0 );
        if (NULL != printDetails)
        {
          FREEIF( printDetails->pPrintFileName );
          FREEIF( printDetails->pFileDescName );
          FREE( printDetails );
        }
      }
      break;
    }
    case IDS_AUTHENTICATE:
    {
      pMe->bConfigChanged = TRUE;
      pMe->mBIP.bDoAuthenticate = pMe->mBIP.bDoAuthenticate ? FALSE : TRUE;
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );

      BTApp_UpdateMenuItemImage( 
        pMe->a.m_pIDisplay, 
        pMe->m_pIMenu, 
        selection,
        pMe->mBIP.bDoAuthenticate ? IDB_CHECK_ON : IDB_CHECK_OFF );
      break;
    }
    case IDS_REGISTER:
    {
      BTApp_BIPBuildServerSubMenu( pMe );
      break;
    }
    case IDS_BIP_REGISTER_IMAGING_RESPONDER:
    {
      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_RegisterImagingResponder( 
                               pMe->mBIP.po, 
                               szServerNameIR,
                               BTAPP_BIP_IMG_RESPONDER_SUPPORTED_CAP,
                               BTAPP_BIP_IMG_RESPONDER_SUPPORTED_FEATURES,
                               BTAPP_BIP_IMG_RESPONDER_SUPPORTED_FUNCTIONS,
                               BTAPP_BIP_IMG_RESPONDER_CAPACITY,
                               pMe->mBIP.bDoAuthenticate ) != SUCCESS )
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
        BTApp_ShowBusyIcon( pMe ); // wait for command done
      }
      break;
    }
    case IDS_BIP_REGISTER_ARCHIVED_OBJECTS_SERVER:
    {
      AEEBTBIPSDUUID128type serviceID = 
          { 0x5E, 0xF9, 0x61, 0x8E, 0xD4, 0x11, 0x79, 0x1A, 
            0x80, 0x00, 0xA4, 0x8E, 0x34, 0x98, 0x9B, 0x5E };

      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_RegisterArchivedObjects( 
                               pMe->mBIP.po, 
                               szServerNameAO,
                               &serviceID,
                               BTAPP_BIP_ARCH_OBJECTS_SUPPORTED_FUNCTIONS,
                               pMe->mBIP.bDoAuthenticate ) != SUCCESS )
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
        BTApp_ShowBusyIcon( pMe ); // wait for command done
      }
      break;
    }
    case IDS_BIP_REGISTER_REFERENCED_OBJECTS_SERVER:
    {
      AEEBTBIPSDUUID128type serviceID = 
          { 0x5D, 0xF9, 0x61, 0x8E, 0xD4, 0x11, 0x79, 0x1A, 
            0x80, 0x00, 0xA4, 0x8E, 0x34, 0x98, 0x9B, 0x5E };

      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_RegisterReferencedObjects( 
                               pMe->mBIP.po, 
                               szServerNameRO,
                               &serviceID,
                               BTAPP_BIP_REF_OBJECTS_SUPPORTED_FUNCTIONS,
                               pMe->mBIP.bDoAuthenticate ) != SUCCESS )
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
        BTApp_ShowBusyIcon( pMe ); // wait for command done
      }
      break;
    }
    case IDS_DEREGISTER:
    {
      if ( IBTEXTBIP_Deregister( pMe->mBIP.po ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      }
      else
      {
        bRegistered = pMe->mBIP.bRegistered;
        pMe->mBIP.bRegistered = FALSE;
        BTApp_CheckToClearDiscoverable( pMe );
        pMe->mBIP.bRegistered = bRegistered;
        BTApp_ShowBusyIcon( pMe ); // wait for command done
      }
      break;
    }
    case IDS_CONNECT:
    {
      key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_OBEX_SERVERS );
      break;
    }
    case IDS_BIP_CONNECT_IMAGE_PUSH:
    {
      AEEBTBIPSDUUID128type pushTargetUUID = 
          { 0xE3, 0x3D, 0x95, 0x45, 0x83, 0x74, 0x4A, 0xD7, 
            0x9E, 0xC5, 0xC1, 0x6B, 0xE3, 0x1E, 0xDE, 0x8E };
      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_Connect( pMe->mBIP.po, 
                              &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 
                              0x0,
                              &pushTargetUUID ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      }
      else
      {
        pMe->mBIP.responderType = AEEBT_BIP_TARGET_IMAGE_PUSH;
        pMe->mBIP.remoteBDAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
        BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
      }
      break;
    }
    case IDS_BIP_CONNECT_IMAGE_PULL:
    {
      AEEBTBIPSDUUID128type pullTargetUUID = 
          { 0x8E, 0xE9, 0xB3, 0xD0, 0x46, 0x08, 0x11, 0xD5, 
            0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E };
      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_Connect( pMe->mBIP.po, 
                              &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 
                              0x0,
                              &pullTargetUUID ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      }
      else
      {
        pMe->mBIP.responderType = AEEBT_BIP_TARGET_IMAGE_PULL;
        pMe->mBIP.remoteBDAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
        BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
      }
      break;
    }
    case IDS_BIP_CONNECT_IMAGE_PRINTING:
    {
      AEEBTBIPSDUUID128type printTargetUUID = 
          { 0x92, 0x35, 0x33, 0x50, 0x46, 0x08, 0x11, 0xD5, 
            0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E };
      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_Connect( pMe->mBIP.po, 
                              &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 
                              0x0,
                              &printTargetUUID ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      }
      else
      {
        pMe->mBIP.responderType = AEEBT_BIP_TARGET_IMAGE_PRINTING;
        pMe->mBIP.remoteBDAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
        BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
      }
      break;
    }
    case IDS_BIP_CONNECT_AUTOMATIC_ARCHIVE:
    {
      AEEBTBIPSDUUID128type archiveTargetUUID = 
          { 0x94, 0x01, 0x26, 0xC0, 0x46, 0x08, 0x11, 0xD5, 
            0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E };
      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_Connect( pMe->mBIP.po, 
                              &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 
                              0x0,
                              &archiveTargetUUID ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      }
      else
      {
        pMe->mBIP.responderType = AEEBT_BIP_TARGET_AUTOMATIC_ARCHIVE;
        pMe->mBIP.remoteBDAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
        BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
      }
      break;
    }
    case IDS_BIP_CONNECT_REMOTE_CAMERA:
    {
      AEEBTBIPSDUUID128type cameraTargetUUID = 
          { 0x94, 0x7E, 0x74, 0x20, 0x46, 0x08, 0x11, 0xD5, 
            0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E };
      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_Connect( pMe->mBIP.po, 
                              &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 
                              0x0,
                              &cameraTargetUUID ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      }
      else
      {
        pMe->mBIP.responderType = AEEBT_BIP_TARGET_REMOTE_CAMERA;
        pMe->mBIP.remoteBDAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
        BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
      }
      break;
    }
    case IDS_BIP_CONNECT_REMOTE_DISPLAY:
    {
      AEEBTBIPSDUUID128type displayTargetUUID = 
          { 0x94, 0xC7, 0xCD, 0x20, 0x46, 0x08, 0x11, 0xD5, 
            0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E };
      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_Connect( pMe->mBIP.po, 
                              &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 
                              0x0,
                              &displayTargetUUID ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      }
      else
      {
        pMe->mBIP.responderType = AEEBT_BIP_TARGET_REMOTE_DISPLAY;
        pMe->mBIP.remoteBDAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
        BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
      }
      break;
    }
    case IDS_BIP_CONNECT_ARCHIVED_OBJECTS:
    {
      AEEBTBIPSDUUID128type targetUUID = 
          { 0x8E, 0x61, 0xF9, 0x5E, 0x1A, 0x79, 0x11, 0xD4, 
            0x8E, 0xA4, 0x00, 0x80, 0x5F, 0x9B, 0x98, 0x34 };
      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_Connect( pMe->mBIP.po, 
                              &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 
                              0x0,
                              &targetUUID ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      }
      else
      {
        pMe->mBIP.responderType = AEEBT_BIP_TARGET_ARCHIVED_OBJECTS_SERVER;
        pMe->mBIP.remoteBDAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
        BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
      }
      break;
    }
    case IDS_BIP_CONNECT_REFERENCED_OBJECTS:
    {
      AEEBTBIPSDUUID128type targetUUID = 
          { 0x8E, 0x61, 0xF9, 0x5D, 0x1A, 0x79, 0x11, 0xD4, 
            0x8E, 0xA4, 0x00, 0x80, 0x5F, 0x9B, 0x98, 0x34 };
      BTApp_SetBondable( pMe );
      if ( IBTEXTBIP_Connect( pMe->mBIP.po, 
                              &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr, 
                              0x0,
                              &targetUUID ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      }
      else
      {
        pMe->mBIP.responderType = AEEBT_BIP_TARGET_REFERENCED_OBJECTS_SERVER;
        pMe->mBIP.remoteBDAddr = pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr;
        BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
      }
      break;
    }
    case IDS_DISCONNECT:
    {
      if ( IBTEXTBIP_Disconnect( pMe->mBIP.po ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
      }
      break;
    }
    case IDS_BIP_GET_CAPABILITIES:
    {
      BTApp_BIPGetCapabilities( pMe );
      break;
    }
    case IDS_BIP_GET_IMAGE_PROPERTIES:
    {
      BTApp_BIPGetImageProperties( pMe );
      break;
    }
    case IDS_BIP_GET_IMAGE_LIST:
    case IDS_BIP_GET_IMAGE_TOTAL:
    case IDS_BIP_GET_RECENT_IMAGE_LIST:
    {
      BTApp_BIPGetImageList( pMe, sel );
      break;
    }
    case IDS_BIP_GET_IMAGE:
    {
      BTApp_BIPGetImage( pMe );
      break;
    }
    case IDS_BIP_GET_PARTIAL_IMAGE:
    {
      BTApp_BIPGetPartialImage( pMe );
      break;
    }
    case IDS_BIP_GET_ATTACHMENT:
    {
      BTApp_BIPGetLinkedAttachment( pMe );
      break;
    }
    case IDS_BIP_GET_THUMBNAIL:
    {
      BTApp_BIPGetLinkedThumbnail( pMe );
      break;
    }
    case IDS_BIP_GET_MONITORING_IMAGE:
    case IDS_BIP_GET_MONITORING_IMAGE_SAVE:
    {
      BTApp_BIPGetMonitoringImage( pMe, sel );
      break;
    }
    case IDS_BIP_GET_STATUS:
    {
      BTApp_BIPGetStatus( pMe );
      break;
    }
    case IDS_BIP_DELETE_IMAGE:
    {
      BTApp_BIPDeleteImage( pMe );
      break;
    }
    case IDS_BIP_REMOTE_DISPLAY:
    case IDS_BIP_REMOTE_DISPLAY_NEXT:
    case IDS_BIP_REMOTE_DISPLAY_PREV:  
    case IDS_BIP_REMOTE_DISPLAY_CURRENT:
    {
      BTApp_BIPRemoteDisplay( pMe, sel );
      break;
    }
    case IDS_BIP_PUT_IMAGE:
    {
      BTApp_BIPPutImage( pMe );
      break;
    }
    case IDS_BIP_PUT_ATTACHMENT:
    {
      BTApp_BIPPutLinkedAttachment( pMe );
      break;
    }
    case IDS_BIP_PUT_THUMBNAIL:
    {
      BTApp_BIPPutLinkedThumbnail( pMe );
      break;
    }
    case IDS_BIP_START_ARCHIVE:
    {
      BTApp_BIPStartArchive( pMe );
      break;
    }
    case IDS_BIP_START_PRINT:
    {
      BTApp_BIPStartPrint( pMe );
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
FUNCTION BTApp_HandleBIPMainMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleBIPMainMenu( CBTApp* pMe, uint16 key )
{
  uint16 selection;
  boolean ev_processed = TRUE;

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_BIPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key)
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_BIPHandleSelection( pMe, selection );
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
FUNCTION BTApp_HandleBIPServerMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleBIPServerMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlBIPServer - k=0x%x", key, 0, 0 );

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_BIPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_BIPHandleSelection( pMe, selection );
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
FUNCTION BTApp_HandleBIPClientMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleBIPClientMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;
  int     result = 0;

  MSG_MED( "HndlBIPClient - k=0x%x", key, 0, 0 );
  if (( pMe->mBIP.bOpInProgress ) && ( key != AVK_CLR ))
  {
      //operation in progress. Dont respond to any key
      // other than CLR key. 
      return ev_processed;
  } 
  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_BIPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_BIPHandleSelection( pMe, selection );
        break;
      }
      case AVK_CLR:
      {
        if ( pMe->mBIP.bOpInProgress ) 
        {
            /* This is to be aborted Now */
            MSG_MED( "HndlBIPClient - Abort the on going operation now OP %x ", 
                       pMe->mBIP.bOpInProgress, 0 , 0 );
            result = IBTEXTBIP_Abort( pMe->mBIP.po );
            if( result != SUCCESS )
            {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

                MSG_MED( "BIP Abort failed %d", result, 0, 0 );
                BTApp_ShowMessage( pMe, IDS_MSG_ABORT_FAILED, 0, 2 );
            }
            else
            {
                MSG_MED( "Aborted", 0, 0, 0 );
            }
        }

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
FUNCTION BTApp_HandleBIPSettingsMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandleBIPSettingsMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlBIPSettings - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      ev_processed = BTApp_BIPHandleSelection( pMe, selection );
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
FUNCTION BTApp_BIPHandleMenus
DESCRIPTION
============================================================================= */
boolean BTApp_BIPHandleMenus( CBTApp *pMe, uint16 key, BTAppMenuType menu )
{
  boolean handled = TRUE;
  switch ( menu )
  {
    case BT_APP_MENU_BIP_TESTS:
      handled = BTApp_HandleBIPMainMenu( pMe, key );
      break;
    case BT_APP_MENU_BIP_SERVER:
      handled = BTApp_HandleBIPServerMenu( pMe, key );
      break;
    case BT_APP_MENU_BIP_CLIENT:
      handled = BTApp_HandleBIPClientMenu( pMe, key );
      break;
    case BT_APP_MENU_BIP_SETTINGS:
      handled = BTApp_HandleBIPSettingsMenu( pMe, key );
      break;
    default:
      MSG_ERROR(" BIP: unexpected menu type %x", menu, 0, 0 );
      handled = FALSE;
      break;
  }
  return handled;
}

/* ==========================================================================
FUNCTION BTApp_BIPHandleUserEvents
DESCRIPTION
============================================================================= */
void BTApp_BIPHandleUserEvents( CBTApp* pMe, uint32 dwParam )
{
  uint16 msgID;

  switch ( dwParam )
  {
    // Common to client and server
    case EVT_BIP_AUTH_REQ:
      if ( pMe->mBIP.bAuthenticated == FALSE )
      {
        BTApp_BuildMenu( pMe, BT_APP_MENU_BIP_PIN );
      }
      else
      {
        // Already authenticated once
        IBTEXTBIP_Authenticate( pMe->mBIP.po, pMe->mBIP.wPIN );
      }
      break;
    // Client User Events
    case EVT_BIP_CONNECTED:
    {
      if ( pMe->mBIP.bConnected == FALSE ) 
      {
        // No prior connection.  This must be primary.
        pMe->mBIP.bConnected = TRUE;
        pMe->mBIP.bAuthenticated = TRUE;
      }
      else
      {
        // Primary connection already up.  This must be sec.
        pMe->mBIP.bSecConnected = TRUE;
      }
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      if ( pMe->mBIP.bPrintFile != FALSE )
      {
        BTApp_BIPPrintImage( pMe );
        pMe->mBIP.bPrintFile = FALSE;
        FREEIF( pMe->mBIP.printDetails.pPrintFileName );
        FREEIF( pMe->mBIP.printDetails.pFileDescName );
      }
      BTApp_BuildTopMenu( pMe ); // rebuild menu to show 'C'
      break;
    }
    case EVT_BIP_DISCONNECTED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      BTApp_BuildTopMenu( pMe ); // rebuild menu to hide 'C'
      break;
    case EVT_BIP_CONN_FAILED:
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_GET_CAPABILITIES_SUCCESS:
      msgID = IDS_BIP_MSG_PULL_SUCCESS;
      // Store the xml data in a file
      BTApp_BIPSaveXMLstring( pMe, szICfileName );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wICfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_GET_CAPABILITIES_FAILED:
      msgID = IDS_BIP_MSG_PULL_FAILED;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      BTApp_ShowMessage( pMe, msgID, wICfileName, 2 );
      break;
    case EVT_BIP_GET_IMAGE_LIST_SUCCESS:
      // Store the xml data in a file
      BTApp_BIPSaveXMLstring( pMe, szILfileName );
      msgID = IDS_BIP_MSG_PULL_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wILfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_GET_IMAGE_LIST_FAILED:
      msgID = IDS_BIP_MSG_PULL_FAILED;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      BTApp_ShowMessage( pMe, msgID, wILfileName, 2 );
      break;
    case EVT_BIP_GET_IMAGE_PROPERTIES_SUCCESS:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_PULL_SUCCESS;

      // Store the xml data in a file
      BTApp_BIPSaveXMLstring( pMe, szIPfileName );

      BTApp_ShowMessage( pMe, msgID, wIPfileName, 2 );
      break;
    case EVT_BIP_GET_IMAGE_PROPERTIES_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_PULL_FAILED;
      BTApp_ShowMessage( pMe, msgID, wIPfileName, 2 );
      break;
    case EVT_BIP_GET_IMAGE_SUCCESS:
      msgID = IDS_BIP_MSG_PULL_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wGIfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION

      break;
    case EVT_BIP_GET_IMAGE_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_PULL_FAILED;
      BTApp_ShowMessage( pMe, msgID, wGIfileName, 2 );
      break;
    case EVT_BIP_GET_PARTIAL_IMAGE_SUCCESS:
      msgID = IDS_BIP_MSG_PULL_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wGIfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_GET_PARTIAL_IMAGE_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_PULL_FAILED;
      BTApp_ShowMessage( pMe, msgID, wGIfileName, 2 );
      break;
    case EVT_BIP_GET_ATTACHMENT_SUCCESS:
      msgID = IDS_BIP_MSG_PULL_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wGAfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_GET_ATTACHMENT_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_PULL_FAILED;
      BTApp_ShowMessage( pMe, msgID, wGAfileName, 2 );
      break;
    case EVT_BIP_GET_THUMBNAIL_SUCCESS:
      msgID = IDS_BIP_MSG_PULL_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wGTfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_GET_THUMBNAIL_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_PULL_FAILED;
      BTApp_ShowMessage( pMe, msgID, wGTfileName, 2 );
      break;
    case EVT_BIP_GET_MONITORING_IMAGE_SUCCESS:
      msgID = IDS_BIP_MSG_PULL_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wMIfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_GET_MONITORING_IMAGE_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_PULL_FAILED;
      BTApp_ShowMessage( pMe, msgID, wGIfileName, 2 );
      break;
    case EVT_BIP_GET_STATUS_SUCCESS:
      msgID = IDS_BIP_MSG_GET_STATUS_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, NULL, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_GET_STATUS_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_GET_STATUS_FAILED;
      BTApp_ShowMessage( pMe, msgID, NULL, 2 );
      break;
    case EVT_BIP_DELETE_IMAGE_SUCCESS:
      msgID = IDS_BIP_MSG_DELETE_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wGIfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_DELETE_IMAGE_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_DELETE_FAILED;
      BTApp_ShowMessage( pMe, msgID, wGIfileName, 2 );
      break;
    case EVT_BIP_REMOTE_DISPLAY_SUCCESS:
      msgID = IDS_BIP_MSG_REMOTE_DISPLAY_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, NULL, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_REMOTE_DISPLAY_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_REMOTE_DISPLAY_FAILED;
      BTApp_ShowMessage( pMe, msgID, NULL, 2 );
      break;
    case EVT_BIP_PUT_IMAGE_SUCCESS:
      msgID = IDS_BIP_MSG_PUSH_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wPIfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_PUT_IMAGE_FAILED:
      msgID = IDS_BIP_MSG_PUSH_FAILED;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      BTApp_ShowMessage( pMe, msgID, wPIfileName, 2 );
      break;
    case EVT_BIP_PUT_ATTACHMENT_SUCCESS:
      msgID = IDS_BIP_MSG_PUSH_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wPAfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_PUT_ATTACHMENT_FAILED:
      msgID = IDS_BIP_MSG_PUSH_FAILED;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      BTApp_ShowMessage( pMe, msgID, wPAfileName, 2 );
      break;
    case EVT_BIP_PUT_THUMBNAIL_SUCCESS:
      msgID = IDS_BIP_MSG_PUSH_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, wPTfileName, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_PUT_THUMBNAIL_FAILED:
      msgID = IDS_BIP_MSG_PUSH_FAILED;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      BTApp_ShowMessage( pMe, msgID, wPTfileName, 2 );
      break;
    case EVT_BIP_START_ARCHIVE_SUCCESS:
      msgID = IDS_BIP_MSG_START_ARCHIVE_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, NULL, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_START_ARCHIVE_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_START_ARCHIVE_FAILED;
      BTApp_ShowMessage( pMe, msgID, NULL, 2 );
      break;
    case EVT_BIP_START_PRINT_SUCCESS:
      msgID = IDS_BIP_MSG_START_PRINT_SUCCESS;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#else
      BTApp_ShowMessage( pMe, msgID, NULL, 2 );
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_START_PRINT_FAILED:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      msgID = IDS_BIP_MSG_START_PRINT_FAILED;
      BTApp_ShowMessage( pMe, msgID, NULL, 2 );
      break;

    // Server User Events
    case EVT_BIP_CONN_REQ:
      if ( IBTEXTBIP_AcceptConnection( pMe->mBIP.po ) == SUCCESS )
      {
        if ( pMe->mBIP.bRegistered == TRUE ) 
        {
          // Primary server
          pMe->mBIP.bAuthenticated = TRUE;
          pMe->mBIP.bConnected = TRUE; // no connected evt received by server
          BTAppMgr_UpdateProfileType( pMe, &pMe->mBIP.remoteBDAddr, BTAPP_MGR_BIP, BTAPP_MGR_CONNECTED );
        }
        else
        {
          pMe->mBIP.bSecConnected = TRUE; // secondary server
        }
      }
      if (pMe->mBIP.bDoAuthenticate) 
      {
        // Clear PIN screen
        BTApp_BuildTopMenu( pMe );
      }  
      break;
    case EVT_BIP_REG:
      pMe->mBIP.bAuthenticated = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      if(pMe->mEnablingType != BTAPP_ENABLING_NONE)
      {
        pMe->mEnablingType++;       
        BTApp_EnableBT(pMe);
      }
      else
      {
        BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_DONE, NULL, 2 );
      }
      break;
    case EVT_BIP_REG_FAILED:
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
    case EVT_BIP_DEREG:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_DONE, NULL, 2 );
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    case EVT_BIP_DEREG_FAILED:
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
      break;
    case EVT_BIP_GET_CAPABILITIES_REQ_INITIAL:
    case EVT_BIP_GET_CAPABILITIES_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_GET_CAPABILITIES_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPGetCapabilitiesResponse( pMe );
      break;
    case EVT_BIP_GET_IMAGE_LIST_REQ_INITIAL:
    case EVT_BIP_GET_IMAGE_LIST_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_GET_IMAGE_LIST_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPGetImageListResponse( pMe );
      break;
    case EVT_BIP_GET_IMAGE_PROPERTIES_REQ_INITIAL:
    case EVT_BIP_GET_IMAGE_PROPERTIES_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_GET_IMAGE_PROPERTIES_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPGetImagePropertiesResponse( pMe );
      break;
    case EVT_BIP_GET_IMAGE_REQ_INITIAL:
    case EVT_BIP_GET_IMAGE_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_GET_IMAGE_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPGetImageResponse( pMe );
      break;
    case EVT_BIP_GET_PARTIAL_IMAGE_REQ_INITIAL:
    case EVT_BIP_GET_PARTIAL_IMAGE_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_GET_PARTIAL_IMAGE_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPGetPartialImageResponse( pMe );
      break;
    case EVT_BIP_GET_ATTACHMENT_REQ_INITIAL:
    case EVT_BIP_GET_ATTACHMENT_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_GET_ATTACHMENT_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPGetLinkedAttachmentResponse( pMe );
      break;
    case EVT_BIP_GET_THUMBNAIL_REQ_INITIAL:
    case EVT_BIP_GET_THUMBNAIL_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_GET_THUMBNAIL_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPGetLinkedThumbnailResponse( pMe );
      break;
    case EVT_BIP_GET_MONITORING_IMAGE_REQ_INITIAL:
    case EVT_BIP_GET_MONITORING_IMAGE_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_GET_MONITORING_IMAGE_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPGetMonitoringImageResponse( pMe );
      break;
    case EVT_BIP_DELETE_IMAGE_REQ_INITIAL:
    case EVT_BIP_DELETE_IMAGE_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_DELETE_IMAGE_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPDeleteImageResponse( pMe );
      break;
    case EVT_BIP_REMOTE_DISPLAY_REQ_INITIAL:
    case EVT_BIP_REMOTE_DISPLAY_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_REMOTE_DISPLAY_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPRemoteDisplayResponse( pMe );
      break;
    case EVT_BIP_PUT_IMAGE_REQ_INITIAL:
    case EVT_BIP_PUT_IMAGE_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_PUT_IMAGE_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPPutImageResponse( pMe );
      break;
    case EVT_BIP_PUT_ATTACHMENT_REQ_INITIAL:
    case EVT_BIP_PUT_ATTACHMENT_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_PUT_ATTACHMENT_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPPutLinkedAttachmentResponse( pMe );
      break;
    case EVT_BIP_PUT_THUMBNAIL_REQ_INITIAL:
    case EVT_BIP_PUT_THUMBNAIL_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_PUT_THUMBNAIL_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPPutLinkedThumbnailResponse( pMe );
      break;
    case EVT_BIP_START_ARCHIVE_REQ_INITIAL:
    case EVT_BIP_START_ARCHIVE_REQ_CLEANUP:
      if ( dwParam == EVT_BIP_START_ARCHIVE_REQ_CLEANUP ) 
      {
        pMe->mBIP.bCleanup = TRUE;
      }
      else
      {
        pMe->mBIP.bCleanup = FALSE;
      }
      BTApp_BIPStartArchiveResponse( pMe );
      break;

    default:
      MSG_ERROR(" BIP: unexpected user event %x", dwParam, 0, 0 );
      break;
  }
}
/* ==========================================================================
FUNCTION BTApp_EnableBIP
DESCRIPTION
============================================================================= */
boolean BTApp_EnableBIP( 
  CBTApp*  pMe, 
  boolean* pbSettingBondable, 
  boolean* pbSettingDiscoverable )
{
  boolean bBIPEnabled = FALSE;
      
  if ( pMe->mBIP.bServerEnable == TRUE )
  {

#ifdef FEATURE_BT_2_1
    if ( pMe->mBIP.bSecSelected !=FALSE ) 
    {
      if( pMe->mBIP.serviceSelection == AEEBT_BIP_IMAGING_RESPONDER )
      {
        IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                                 AEEBT_SD_SERVICE_CLASS_IMAGING_RESPONDER,
                                 pMe->mBIP.srvSecType,
                                 pMe->mBIP.bAuthorize,
                                 pMe->mBIP.bAuthorizeFirst );  
      }
      else if ( pMe->mBIP.serviceSelection == AEEBT_BIP_ARCHIVED_OBJECTS )
      {
        IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                                 AEEBT_SD_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE,
                                 pMe->mBIP.srvSecType,
                                 pMe->mBIP.bAuthorize,
                                 pMe->mBIP.bAuthorizeFirst );   
      }
      else if ( pMe->mBIP.serviceSelection == AEEBT_BIP_REFERENCED_OBJECTS )
      {
        IBTEXTRM_SetSecBySvcCls( 
                                pMe->mRM.po, 
                                AEEBT_SD_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS,
                                pMe->mBIP.srvSecType,
                                pMe->mBIP.bAuthorize,
                                pMe->mBIP.bAuthorizeFirst ); 
      } 
    }
#endif //#ifdef FEATURE_BT_2_1

    if ( BTApp_BIPInit( pMe ) == FALSE )
    {
      MSG_ERROR( "EnableBIP - failed to create BIP object", 0, 0, 0 );
      BTApp_BIPCleanup( pMe );
    }
    else if ( pMe->mBIP.bRegistered == FALSE )
    {
      switch ( pMe->mBIP.serviceSelection )
      {
        case AEEBT_BIP_IMAGING_RESPONDER:
        {
          if ( *pbSettingBondable == FALSE )
          {
            BTApp_SetBondable( pMe );
            *pbSettingBondable = TRUE;
          }
          MSG_LOW( "EnableBIP - enabling BIP", 0, 0, 0 );
          if ( IBTEXTBIP_RegisterImagingResponder( 
                                   pMe->mBIP.po, 
                                   szServerNameIR,
                                   0x0F,
                                   0x01FF,
                                   0x0000FFFF,
                                   0x000000000000FFFF,
                                   pMe->mBIP.bDoAuthenticate ) != SUCCESS )
          {
            BTApp_ClearBondable( pMe ); // no need to be bondable anymore
            MSG_ERROR( "EnableBIP - Failed to enable BIP", 0, 0, 0 );
          }
          else
          {
            if ( *pbSettingDiscoverable == FALSE )
            {
              IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
              *pbSettingDiscoverable = TRUE;
            }
            BTApp_ShowBusyIcon( pMe ); // wait for command done
            bBIPEnabled = TRUE;
          }
          break;
        }
        case AEEBT_BIP_ARCHIVED_OBJECTS:
        {
          AEEBTBIPSDUUID128type serviceID = 
              { 0x5E, 0xF9, 0x61, 0x8E, 0xD4, 0x11, 0x79, 0x1A, 
                0x80, 0x00, 0xA4, 0x8E, 0x34, 0x98, 0x9B, 0x5E };

          if ( *pbSettingBondable == FALSE )
          {
            BTApp_SetBondable( pMe );
            *pbSettingBondable = TRUE;
          }

          if ( IBTEXTBIP_RegisterArchivedObjects( 
                                   pMe->mBIP.po, 
                                   szServerNameAO,
                                   &serviceID,
                                   0x000007E1,
                                   pMe->mBIP.bDoAuthenticate ) != SUCCESS )
          {
            BTApp_ClearBondable( pMe ); // no need to be bondable anymore
            MSG_ERROR( "EnableBIP - Failed to enable BIP", 0, 0, 0 );
          }
          else
          {
            if ( *pbSettingDiscoverable == FALSE )
            {
              IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
              *pbSettingDiscoverable = TRUE;
            }
            BTApp_ShowBusyIcon( pMe ); // wait for command done
            bBIPEnabled = TRUE;
          }
          break;
        }
        case AEEBT_BIP_REFERENCED_OBJECTS:
        {
          AEEBTBIPSDUUID128type serviceID = 
              { 0x5D, 0xF9, 0x61, 0x8E, 0xD4, 0x11, 0x79, 0x1A, 
                0x80, 0x00, 0xA4, 0x8E, 0x34, 0x98, 0x9B, 0x5E };
          
          if ( *pbSettingBondable == FALSE )
          {
            BTApp_SetBondable( pMe );
            *pbSettingBondable = TRUE;
          }

          if ( IBTEXTBIP_RegisterReferencedObjects( 
                                   pMe->mBIP.po, 
                                   szServerNameRO,
                                   &serviceID,
                                   0x00001001,
                                   pMe->mBIP.bDoAuthenticate ) != SUCCESS )
          {
            BTApp_ClearBondable( pMe ); // no need to be bondable anymore
            MSG_ERROR( "EnableBIP - Failed to enable BIP", 0, 0, 0 );
          }
          else
          {
            if ( *pbSettingDiscoverable == FALSE )
            {
              IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
              *pbSettingDiscoverable = TRUE;
            }
            BTApp_ShowBusyIcon( pMe ); // wait for command done
            bBIPEnabled = TRUE;
          }
          break;
        }
      }
    }
  }
  return bBIPEnabled;
}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#endif /* FEATURE_BT_EXTPF_BIP */

#endif /* FEATURE_APP_BLUETOOTH */

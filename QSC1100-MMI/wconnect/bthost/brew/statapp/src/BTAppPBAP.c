/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppPBAP.c                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains PBAP test feature.        */  
/*                                                                           */  
/*              Copyright (c) 2006-2008 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppPBAP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
$Author: deepikas $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
10/09/08   GS   Replaced deprecated function in PBAPClientDownloadReq().
05/12/08   VM   Replacing deprecated functions with BREW replacements.
04/10/08   VM   Changed the profile enabling mechanism to correct the failure
                in enabling profiles.
03/06/08   VM    Removed redundant IBTEXTRM_SetSecBySvcCls call
02/08/08   gb   Fixed PBAP client registration failure in Lisbon Enabled builds.
02/01/08   mh   Fixed crashing Icard calls when ICARD create instance fails.
01/16/08   nm   Added code to obtain number of missed calls from the
                missed call history
01/15/08   rs   Added filter check before framing V-Card response for Get operations
12/04/07   mh   App manager to support low power configuration
11/20/07   hs   The SIM folder can only be browsed on the targets which contain
                SIM or RUIM card. Similarly SIM phonebook/other folders can only 
				be downloaded from the targets containing SIM/RUIM card.
11/01/07   ag   Merged Lisbon(BT2.1) code to the mainline
10/29/07   hs   Formatted the name properly for vcard listing
07/12/07   rs   Fixed the 
                - list Offset in the phonebook pull feature and  
                - Correct representation of Both Vcard versions
06/28/07   pn   Added PBAP client auth support
06/28/07   pn   Fixed compile error introduced in previous change.
06/27/07   gs   Made following updates for UIOne targets.
                - name search is done based on lastname.
                - updated vcard-listing builder logic to make it more robust.
06/26/07   pn   Removed compiler warnings.
06/07/07   gs   Fixed NULL pointer access in PBAPBuildABListingObject().
                Added stricter folder validation check for SetPhoneBook
                handling in PSE role.
05/22/07   ns   Fixed automation failure while deregistering server when it is
                already connected
04/05/07   gs   Updated to return the right error code if client tries 
                to download call history phonebook in SIM.
03/02/07   gs   Fixed issue in authentication. Always prompt for PIN even if
                PIN exists from previous auth attempt.
                Added support for new missed calls in PullPhoneBook
11/24/06   ns   Fixed problem of password dialog not getting closed on server 
                when authentication fails
11/06/06   ns   Added name property to the callhistory listing. Fixed problem
                of password menu being displayed when authentication fails on 
                server
11/03/06   ns   - Fixed problem with server not returning the phone number when 
                  there is no corresponding entry available in the phonebook
                - Fixed problem of server returning two entries while pulling
                  call history items when the entry is available in EFS and SIM
                - Fixed problem of filter value not being processed properly 
                  during address book listing
10/23/06   gs   - Updated OBEX error codes to confirm to spec.
                - Updated BTApp_PBAPHandlePBObjectListing() to return error
                  respose if the listing request has invalid phonebook 
                  object name.
09/15/06   ns   Fixed problem with search feature returning 0th rec always
09/12/06   gs   Fixing RVCT compile issues on 7K target.
08/10/06   ns   Fixed problem with server returning wrong no of vcards when
                maxlist count value is less than no of items in PB.
                Fixed problem with server returning vcard from PB while 
                client is pulling vcard from SIM.
07/13/06   ns   Added concurrency support for pull vcard obj operation
07/07/06   ns   Added support to allow browsing from one level above the 
                leaf directory. Added concurrency support for download PB obj
06/14/06   ns   - Added support for generic phone number field in vcard contents, 
                  as contact app stores phone no in SIM in generic category
                - Client operations not allowed when server is registered
                - Dir & file names received from client are treated as case 
                  insensitive by the server
                - Fixed problem of returning incorrect no of AB records
06/13/06   ns   Deregister menu option displayed after client disconnects
                Fixed problem with missing '>' character in vcard lisiting
                Changed implementation such that browsing is allowed only 
                when at the leaf node of the directory hierarchy
06/07/06   ns   Added automation support for PBAP
02/01/06   ns   Initial Revision.

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp.h"
#include "BTAppUtils.h"
#include "btapp_res.h"

#ifdef FEATURE_BT_EXTPF_PBAP
#include "AEEBTExtPBAP.h"
#include "AEECard.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#define EVT_PBAP_SRV_REG                    1
#define EVT_PBAP_SRV_REG_FAILED             2
#define EVT_PBAP_SRV_DEREG                  3
#define EVT_PBAP_SRV_DEREG_FAILED           4
#define EVT_PBAP_CONN_REQ                   5
#define EVT_PBAP_CONNECTED                  6
#define EVT_PBAP_CONN_FAILED                7
#define EVT_PBAP_DISCONNECTED               8
#define EVT_PBAP_PULL_PB_REQ                9
#define EVT_PBAP_PULL_PB_DONE               10
#define EVT_PBAP_SET_PB_REQ                 11
#define EVT_PBAP_SET_PB_DONE                12
#define EVT_PBAP_PULL_VCARD_LISTING_REQ     13
#define EVT_PBAP_PULL_VCARD_LISTING_DONE    14
#define EVT_PBAP_PULL_VCARD_ENTRY_REQ       15
#define EVT_PBAP_PULL_VCARD_ENTRY_DONE      16
#define EVT_PBAP_PULL_PB_FAILED             17
#define EVT_PBAP_SET_PB_FAILED              18
#define EVT_PBAP_PULL_VCARD_LISTING_FAILED  19
#define EVT_PBAP_PULL_VCARD_ENTRY_FAILED    20
#define EVT_PBAP_CLI_REG                    21
#define EVT_PBAP_CLI_REG_FAILED             22
#define EVT_PBAP_CLI_DEREG                  23
#define EVT_PBAP_CLI_DEREG_FAILED           24
#define EVT_PBAP_AUTH_REQ                   25
#define EVT_PBAP_OP_ABORTED                 26

// repository directory
#define BT_APP_PBAP_PHONE_DIR               "telecom"
#define BT_APP_PBAP_SIM_DIR                 "SIM1"
// Size of the above string
#define BT_APP_PBAP_SIZEOF_DIR              12

// object type
#define BT_APP_PBAP_OBJECT_PB               1
#define BT_APP_PBAP_OBJECT_ICH              2
#define BT_APP_PBAP_OBJECT_OCH              3
#define BT_APP_PBAP_OBJECT_MCH              4
#define BT_APP_PBAP_OBJECT_CCH              5

// Max number of digits in a call history entry 
#ifdef FEATURE_36_DIGIT_DIAL_STRING
#define BT_APP_PBAP_CALLHIST_MAXDIGITS      37
#else
#define BT_APP_PBAP_CALLHIST_MAXDIGITS      33
#endif

// Max number of letters of an call history entry 
// (including the NULL byte)
#define BT_APP_PBAP_CALLHISTORY_MAXLETTERS  25

#define BT_APP_VCARD2_1      "BEGIN:VCARD\r\nVERSION:2.1\r\nN:OWNER\r\nTEL\r\nEND:VCARD\r\n"
#define BT_APP_VCARD3_0      "BEGIN:VCARD\r\nVERSION:3.0\r\nN:OWNER\r\nFN:OWNER\r\nTEL\r\nEND:VCARD\r\n"


// virtual folder path names
#define BT_APP_PBAP_TELECOM       AEEBT_PBAP_FOLDER_NAME_ROOT DIRECTORY_STR AEEBT_PBAP_FOLDER_NAME_TELECOM
#define BT_APP_PBAP_SIM1          AEEBT_PBAP_FOLDER_NAME_ROOT DIRECTORY_STR AEEBT_PBAP_FOLDER_NAME_SIM1
#define BT_APP_PBAP_SIM1_TELCOM   AEEBT_PBAP_FOLDER_NAME_ROOT DIRECTORY_STR AEEBT_PBAP_FOLDER_NAME_SIM1 \
                                    DIRECTORY_STR AEEBT_PBAP_FOLDER_NAME_TELECOM

// files for storing local objects
#define PB_VCF   BTAPP_ROOT_DIR DIRECTORY_STR "pb.vcf"
#define ICH_VCF  BTAPP_ROOT_DIR DIRECTORY_STR "ich.vcf"
#define OCH_VCF  BTAPP_ROOT_DIR DIRECTORY_STR "och.vcf"
#define MCH_VCF  BTAPP_ROOT_DIR DIRECTORY_STR "mch.vcf"
#define CCH_VCF  BTAPP_ROOT_DIR DIRECTORY_STR "cch.vcf"

// files for storing SIM objects
#define SIM_DIR      "SIM"
#define SIM_PB_VCF   BTAPP_ROOT_DIR DIRECTORY_STR \
                       SIM_DIR DIRECTORY_STR "pb.vcf"
#define SIM_ICH_VCF  BTAPP_ROOT_DIR DIRECTORY_STR \
                       SIM_DIR DIRECTORY_STR "ich.vcf"
#define SIM_OCH_VCF  BTAPP_ROOT_DIR DIRECTORY_STR \
                       SIM_DIR DIRECTORY_STR "och.vcf"
#define SIM_MCH_VCF  BTAPP_ROOT_DIR DIRECTORY_STR \
                       SIM_DIR DIRECTORY_STR "mch.vcf"
#define SIM_CCH_VCF  BTAPP_ROOT_DIR DIRECTORY_STR \
                       SIM_DIR DIRECTORY_STR "cch.vcf"

// file for vcard listing
#define VLIST_XML    AEEBT_PBAP_VCARD_LISTING_XML_FILE

// used for XML parsing
#define FOLDER_NAME_STR      "folder name"
#define FILE_NAME_STR        "file name"
#define CARD_HANDLE_STR      "card handle"
#define CARD_NAME_STR        "name"
#define SEARCH_CHR           '"'

static uint32 uNMaskCard = NMASK_CARD_STATUS | NMASK_BT_PIN1_STATUS;
static ICard*   pICard;

extern uint32 uBTApp_NMask;
char*  szServerNamePBAP = "QC PBAP Server";
char*  szClientNamePBAP = "QC PBAP Client";
char   sClientPath[ AEEBT_MAX_FILE_NAME + 1 ];
char   sServerPath[ AEEBT_MAX_FILE_NAME + 1 ];
char   sSetFolder[ AEEBT_MAX_FILE_NAME + 1 ];
char   szBuf[200];

void   BTApp_PBAPDoClientSettingsMenu( CBTApp* pMe, int item );
static boolean BTApp_PBAPHandleSelection( CBTApp* pMe, uint16 sel );
static boolean BTApp_PBAPBuildCCHObject( CBTApp * pMe );
static boolean BTApp_PBAPBuildCHObject( CBTApp * pMe, uint16 uCallType );
static boolean BTApp_PBAPBuildABObject( CBTApp * pMe, boolean bFromSIM );
static boolean BTApp_PBAPAddVCardProperties( CBTApp* pMe, IAddrRec* addrRec,
                                             IFile* pIFile);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION


/* ==========================================================================
FUNCTION BTApp_ProcessPBAPNotifications
DESCRIPTION
============================================================================= */
void BTApp_ProcessPBAPNotifications(
  CBTApp* pMe, 
  AEEEvent evt, 
  NotificationData* pData
)
{
  uint32   userEvent;

  MSG_MED( "ProcessPBAPNotifications - evt=%d", evt, 0, 0 );

  switch ( evt )
  {
    case AEEBT_PBAP_EVT_SRV_REG_DONE:    // registration process done
    {
      if ( pData->uError == AEEBT_ERR_NONE )
      {
        userEvent = EVT_PBAP_SRV_REG;
      }
      else 
      {
        userEvent = EVT_PBAP_SRV_REG_FAILED;
      }
      BTAPP_POST_USER_EVENT( PBAP, userEvent );
      break;
    }
    case AEEBT_PBAP_EVT_SRV_DEREG_DONE:  // deregistration process done
    {
      if ( pData->uError == AEEBT_ERR_NONE )
      {
        userEvent = EVT_PBAP_SRV_DEREG;
      }
      else 
      {
        userEvent = EVT_PBAP_SRV_DEREG_FAILED;
      }
      BTAPP_POST_USER_EVENT( PBAP, userEvent );
      break;
    }
    case AEEBT_PBAP_EVT_CLI_REG_DONE:  // for client
    {
      if ( pData->uError == AEEBT_ERR_NONE )
      {
        userEvent = EVT_PBAP_CLI_REG;
      }
      else 
      {
        userEvent = EVT_PBAP_CLI_REG_FAILED;
      }
      BTAPP_POST_USER_EVENT( PBAP, userEvent );
      break;
    }
    case AEEBT_PBAP_EVT_CLI_DEREG_DONE:  // for client
    {
      if ( pData->uError == AEEBT_ERR_NONE )
      {
        userEvent = EVT_PBAP_CLI_DEREG;
      }
      else 
      {
        userEvent = EVT_PBAP_CLI_DEREG_FAILED;
      }
      BTAPP_POST_USER_EVENT( PBAP, userEvent );
      break;
    }
    case AEEBT_PBAP_EVT_CONN_REQ: // for server
    {
      pMe->mPBAP.remoteBDAddr = pData->bdAddr;
      if ( pData->PBAPObject.wUserID[0] != NULL )
      {
        WSTRLCPY( pMe->mPBAP.wCliUserID, pData->PBAPObject.wUserID, 
                  ARR_SIZE( pMe->mPBAP.wCliUserID ) );
      }
      else
      {
        pMe->mPBAP.wCliUserID[0] = NULL;
      }
      BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_CONN_REQ );
      break;
    }
    case AEEBT_PBAP_EVT_CONNECTED: // for server and client
    {
      BTAppMgr_UpdateProfileType( pMe, &pMe->mPBAP.remoteBDAddr, BTAPP_MGR_PBAP, BTAPP_MGR_CONNECTED );
      BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_CONNECTED );
      break;
    }
    case AEEBT_PBAP_EVT_CONN_FAILED:
    {
      BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_CONN_FAILED );
      break;
    }
    case AEEBT_PBAP_EVT_DISCONNECTED: // for server and client
    {
      BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_DISCONNECTED );
      BTAppMgr_UpdateProfileType( pMe, &pMe->mPBAP.remoteBDAddr, BTAPP_MGR_PBAP, BTAPP_MGR_DISCONNECTED );
      break;
    }
    case AEEBT_PBAP_EVT_PULL_PHONE_BOOK_REQ: // for server
    {
      // store the request details
      pMe->mPBAP.PBAPObject.filter = pData->PBAPObject.filter;
      pMe->mPBAP.PBAPObject.format = pData->PBAPObject.format;
      pMe->mPBAP.PBAPObject.maxListCount = pData->PBAPObject.maxListCount;
      pMe->mPBAP.PBAPObject.listStartOffset=pData->PBAPObject.listStartOffset;
      WSTRLCPY( pMe->mPBAP.PBAPObject.pwObjectName, 
                pData->PBAPObject.pwObjectName, 
                ARR_SIZE( pMe->mPBAP.PBAPObject.pwObjectName ) );
      BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_PULL_PB_REQ );
      break;
    }
    case AEEBT_PBAP_EVT_PULL_PHONE_BOOK_DONE: // for client
    {
      if ( pMe->mPBAP.bSrvRegistered == FALSE )
      {
        if ( pData->uError == AEEBT_ERR_NONE )
        {
          if ( pMe->mPBAP.PBAPObject.maxListCount == 0 ) 
          {
            pMe->mPBAP.PBAPObject.phoneBookSize = 
              pData->PBAPObject.phoneBookSize;
            MSG_HIGH ("ProcessPBAPNotifications - pb size = %d",
                      pMe->mPBAP.PBAPObject.phoneBookSize, 0, 0);
          }
          else
          {
            /* If requested phonebook type is MCH, get newMissedCalls info */
            if ( !STRCMP(pMe->mPBAP.szObjectName, 
                          AEEBT_PBAP_OBJECT_NAME_LOCAL_MCH) ||
                 !STRCMP(pMe->mPBAP.szObjectName, 
                          AEEBT_PBAP_OBJECT_NAME_SIM_MCH) ) 
            {
              pMe->mPBAP.PBAPObject.newMissedCalls = 
                pData->PBAPObject.newMissedCalls;
              MSG_HIGH ("ProcessPBAPNotifications - missed calls = %d",
                        pMe->mPBAP.PBAPObject.newMissedCalls, 0, 0);
            }       
          }
          BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_PULL_PB_DONE );
        }
        else
        {
          BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_PULL_PB_FAILED );      
        }
      }
      break;
    }
    case AEEBT_PBAP_EVT_SET_PHONE_BOOK_REQ: // for server
    {
      pMe->mPBAP.PBAPObject.flag = pData->PBAPObject.flag;
      if ( pMe->mPBAP.PBAPObject.flag == AEEBT_PBAP_SET_PATH_TO_FOLDER )
      {
        WSTRLCPY( pMe->mPBAP.PBAPObject.wName, pData->PBAPObject.wName, 
                  ARR_SIZE( pMe->mPBAP.PBAPObject.wName ) );
      }
      BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_SET_PB_REQ );
      break;
    }
    case AEEBT_PBAP_EVT_SET_PHONE_BOOK_DONE: // for client
    {
      if ( pData->uError == AEEBT_ERR_NONE )
      {
        BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_SET_PB_DONE );
      }
      else
      {
        BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_SET_PB_FAILED );
      }
      break;
    }
    case AEEBT_PBAP_EVT_PULL_VCARD_LISTING_REQ: // for server
    {
      pMe->mPBAP.PBAPObject.searchOrder = pData->PBAPObject.searchOrder;
      pMe->mPBAP.PBAPObject.searchAttrib = pData->PBAPObject.searchAttrib;
      pMe->mPBAP.PBAPObject.maxListCount = pData->PBAPObject.maxListCount;
      pMe->mPBAP.PBAPObject.listStartOffset = 
                                pData->PBAPObject.listStartOffset;
      WSTRLCPY( pMe->mPBAP.PBAPObject.searchValue, 
                pData->PBAPObject.searchValue, 
                ARR_SIZE( pMe->mPBAP.PBAPObject.searchValue ) );
      WSTRLCPY( pMe->mPBAP.PBAPObject.pwObjectName, 
                pData->PBAPObject.pwObjectName, 
                ARR_SIZE( pMe->mPBAP.PBAPObject.pwObjectName ) );
      BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_PULL_VCARD_LISTING_REQ );
      break;
    }
    case AEEBT_PBAP_EVT_PULL_VCARD_LISTING_DONE: // for client
    {
      if ( pMe->mPBAP.bSrvRegistered == FALSE )
      {
        if ( pData->uError == AEEBT_ERR_NONE )
        {
          if ( pMe->mPBAP.PBAPObject.maxListCount == 0 ) 
          {
            pMe->mPBAP.PBAPObject.phoneBookSize = 
              pData->PBAPObject.phoneBookSize;
          }
          BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_PULL_VCARD_LISTING_DONE );
        }
        else
        {
          BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_PULL_VCARD_LISTING_FAILED );
        }
      }
      break;
    }
    case AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_REQ: // for server
    {
      pMe->mPBAP.PBAPObject.filter = pData->PBAPObject.filter;
      pMe->mPBAP.PBAPObject.format = pData->PBAPObject.format;
      WSTRLCPY( pMe->mPBAP.PBAPObject.pwObjectName, 
                pData->PBAPObject.pwObjectName, 
                ARR_SIZE( pMe->mPBAP.PBAPObject.pwObjectName ) );
      BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_PULL_VCARD_ENTRY_REQ );
      break;      
    }
    case AEEBT_PBAP_EVT_PULL_VCARD_ENTRY_DONE: // for client
    {
      if ( pMe->mPBAP.bSrvRegistered == FALSE )
      {
        if ( pData->uError == AEEBT_ERR_NONE )
        {
          BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_PULL_VCARD_ENTRY_DONE );
        }
        else
        {
          BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_PULL_VCARD_ENTRY_FAILED );
        }
      }
      break;
    }
    case AEEBT_PBAP_EVT_AUTH_REQ:
    {
      MSG_LOW( "PBAP: Auth UID:%d", pData->PBAPObject.bUserIDRequired, 0, 0 );
      pMe->mPBAP.bUserIDRequired = pData->PBAPObject.bUserIDRequired;
      BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_AUTH_REQ );
      break;
    }
    case AEEBT_PBAP_EVT_OP_ABORTED:
    {
      BTAPP_POST_USER_EVENT( PBAP, EVT_PBAP_OP_ABORTED );
      break;
    }
    default:
      MSG_LOW( "ProcessPBAPNotifications - invalid event", 0, 0, 0 );
      break;
  }
}

/* ==========================================================================
FUNCTION BTApp_PBAPClientPullVCardListingAboveLeaf
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPClientPullVCardListingAboveLeaf( CBTApp* pMe,
                                                          uint8 uIndex )
{
  int      uResult = EFAILED;
  uint16   maxListCount;
  uint16   listStartOffset;
  char     sMaxListCount[ MAX_LIST_COUNT_VAL_LEN + 1 ];
  char     sListStartOffset[ MAX_LIST_COUNT_VAL_LEN + 1 ];
  char*    pFolder = NULL;
  AECHAR   wName[ AEEBT_MAX_FILE_NAME+1 ]; 

  pMe->mPBAP.bListOneLevelAbove = TRUE;
  if (STRCMP(pMe->mPBAP.pbObjects[uIndex].szObjHdleName, "pb.vcf") == 0)
  {
    pFolder = AEEBT_PBAP_FOLDER_NAME_PB;
  }
  else if (STRCMP(pMe->mPBAP.pbObjects[uIndex].szObjHdleName, "ich.vcf") == 0)
  {
    pFolder = AEEBT_PBAP_FOLDER_NAME_ICH;
  }
  else if (STRCMP(pMe->mPBAP.pbObjects[uIndex].szObjHdleName, "och.vcf") == 0)
  {
    pFolder = AEEBT_PBAP_FOLDER_NAME_OCH;
  }
  else if (STRCMP(pMe->mPBAP.pbObjects[uIndex].szObjHdleName, "mch.vcf") == 0)
  {
    pFolder = AEEBT_PBAP_FOLDER_NAME_MCH;
  }
  else if (STRCMP(pMe->mPBAP.pbObjects[uIndex].szObjHdleName, "cch.vcf") == 0)
  {
    pFolder = AEEBT_PBAP_FOLDER_NAME_CCH;
  }
  STRTOWSTR( pFolder, wName, sizeof(wName) );

  /* convert listcount & listoffset form wchar to uint32 */
  WSTRTOSTR( pMe->mPBAP.wMaxListCount, sMaxListCount, sizeof(sMaxListCount) );
  maxListCount = ATOI( sMaxListCount );
  WSTRTOSTR( pMe->mPBAP.wListStartOffset, sListStartOffset, 
             sizeof(sListStartOffset) );
  listStartOffset = ATOI( sListStartOffset );

  // Save maxListCount for future use
  pMe->mPBAP.PBAPObject.maxListCount = maxListCount;

  uResult = IBTEXTPBAP_PullvCardListing( pMe->mPBAP.po, wName, 
              &pMe->mPBAP.sortOrder, pMe->mPBAP.wSearchValue,
              &pMe->mPBAP.searchAttrib, 
              &maxListCount, &listStartOffset );
  if ( uResult != SUCCESS )
  {
    MSG_HIGH( "PBAPClientPullVCardListing - IBT call failed", 0, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_PBAP_MSG_PULL_VCARD_LIST_FAIL, NULL, 2 );
    return FALSE;
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
  }
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_PBAPHexStr2Int
DESCRIPTION: returns a negative number if fails
============================================================================= */
static int BTApp_PBAPHexStr2Int( char* sHex )
{
  int n = 0;         // position in string
  int m = 0;         // position in digit[] to shift
  int count;         // loop index
  int intValue = 0;  // integer value of hex string
  int digit[9];      // hold values to convert

  while ( n < 8 )
  {
     if ( sHex[n]=='\0' )
     {
       break;
     }
     if ( sHex[n] >= '0'  && sHex[n] <= '9' ) //if 0 to 9
     {
       digit[n] = sHex[n] & 0x0f;            //convert to int
     }
     else if ( sHex[n] >='a' && sHex[n] <= 'f' ) //if a to f
     {
       digit[n] = (sHex[n] & 0x0f) + 9;      //convert to int
     }
     else if ( sHex[n] >='A' && sHex[n] <= 'F' ) //if A to F
     {
       digit[n] = (sHex[n] & 0x0f) + 9;      //convert to int
     }
     else
     {
       return -1;
     }
    n++;
  }
  count = n;
  m = n - 1;
  n = 0;
  while(n < count) {
     // digit[n] is value of hex digit at position n
     // (m << 2) is the number of positions to shift
     // OR the bits into return value
     intValue = intValue | (digit[n] << (m << 2));
     m--;   // adjust the position to set
     n++;   // next digit to process
  }
  return (intValue);
}

/* ==========================================================================
FUNCTION BTApp_PBAPAddTelecomListing
DESCRIPTION
============================================================================= */
static void BTApp_PBAPAddTelecomListing( IFile* pFile, boolean bFromSIM, 
                                         int uListCount )
{
  int i = 0;
  
  /* adding folders */
  SNPRINTF( szBuf, sizeof( szBuf ), "<folder name=\"pb\"/>\n" );
  IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
  i++;

  if ( i < uListCount )
  {
    SNPRINTF( szBuf, sizeof( szBuf ), "<folder name=\"ich\"/>\n" );
    IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
    i++;
  }
  if ( i < uListCount )
  {
    SNPRINTF( szBuf, sizeof( szBuf ), "<folder name=\"och\"/>\n" );
    IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
    i++;
  }
  if ( i < uListCount )
  {
    SNPRINTF( szBuf, sizeof( szBuf ), "<folder name=\"mch\"/>\n" );
    IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
    i++;
  }
  if ( i < uListCount )
  {
    SNPRINTF( szBuf, sizeof( szBuf ), "<folder name=\"cch\"/>\n" );
    IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
    i++;
  }

  /*adding files */
  if ( i < uListCount )
  {
    SNPRINTF( szBuf, sizeof( szBuf ), "<file name=\"pb.vcf\"/>\n" );
    IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
    i++;
  }

  if ( bFromSIM == FALSE )
  {
    if ( i < uListCount )
    {
      SNPRINTF( szBuf, sizeof( szBuf ), "<file name=\"ich.vcf\"/>\n" );
      IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
      i++;
    }
    if ( i < uListCount )
    {
      SNPRINTF( szBuf, sizeof( szBuf ), "<file name=\"och.vcf\"/>\n" );
      IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
      i++;
    }
    if ( i < uListCount )
    {
      SNPRINTF( szBuf, sizeof( szBuf ), "<file name=\"mch.vcf\"/>\n" );
      IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
      i++;
    }
    if ( i < uListCount )
    {
      SNPRINTF( szBuf, sizeof( szBuf ), "<file name=\"cch.vcf\"/>\n" );
      IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_PBAPAddVCardListingHeader
DESCRIPTION
============================================================================= */
static void BTApp_PBAPAddVCardListingHeader( IFile* pFile )
{
  STRLCPY( szBuf, "<?xml version=\"1.0\"?>\n", sizeof( szBuf ) );
  IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );

  STRLCPY( szBuf, 
           "<!DOCTYPE vcard-listing SYSTEM \"vcard-listing.dtd\">\n", 
           sizeof( szBuf ) );
  IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );

  STRLCPY( szBuf, "<vCard-listing version=\"1.0\">\n", sizeof( szBuf ) );
  IFILE_Write( pFile, szBuf, STRLEN( szBuf ) );
}

/* ==========================================================================
FUNCTION BTApp_PBAPIsSIMRec
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPIsSIMRec( AEEAddrCat  category )
{
  if ( (category==AEE_ADDR_CAT_SIM_ADN) || (category==AEE_ADDR_CAT_SIM_ADN2)||
       (category==AEE_ADDR_CAT_SIM_FDN) || (category==AEE_ADDR_CAT_SIM_FDN2) ||
       (category==AEE_ADDR_CAT_SIM_BDN) ||(category==AEE_ADDR_CAT_SIM_BDN2) ||
       (category==AEE_ADDR_CAT_SIM_SDN) || (category==AEE_ADDR_CAT_SIM_SDN2) )
  {
    return TRUE;
  }
  return FALSE;
}

/* ==========================================================================
FUNCTION BTApp_PBAPFormatTime
DESCRIPTION
============================================================================= */
static void BTApp_PBAPFormatTime( CBTApp* pMe, dword* dTimeStamp, char* pBuf,
                                  int sizeofpBuf )
{
  JulianType  jdate;
  uint16      num, rem;
  char        year[5] = { '0', '0', '0', '0', NULL };
  char        twoDigits[3] = { '0', '0', NULL };
  int8        index = 3;
  uint8       len = 0;
  
  GETJULIANDATE( *dTimeStamp, &jdate );
  /* convert year */
  num = jdate.wYear; index=3;
  while ( (num != 0) && (index >= 0) )
  {
    rem = num%10;
    num = num/10;
    year[index--] = (char)(rem + 48);
  }
  SNPRINTF( pBuf, sizeofpBuf,"%s", year );
  len = len + 4;

  /* convert month */
  num = jdate.wMonth;index=1;
  while ( (num != 0) && (index >= 0) )
  {
    rem = num%10;
    num = num/10;
    twoDigits[index--] = (char)(rem + 48);
  }
  SNPRINTF( pBuf+len, sizeofpBuf-len,"%s", twoDigits );
  len = len + 2;

  /* convert day */
  num = jdate.wDay;index=1;
  twoDigits[0]='0';  twoDigits[1]='0';
  while ( (num != 0) && (index >= 0) )
  {
    rem = num%10;
    num = num/10;
    twoDigits[index--] = (char)(rem + 48);
  }
  SNPRINTF( pBuf+len, sizeofpBuf-len, "%s", twoDigits );
  len = len + 2;
  
  /* adding time */
  /* convert hour */
  num = jdate.wHour;index=1;
  twoDigits[0]='0';  twoDigits[1]='0';  
  while ( (num != 0) && (index >= 0) )
  {
    rem = num%10;
    num = num/10;
    twoDigits[index--] = (char)(rem + 48);
  }
  SNPRINTF( pBuf+len, sizeofpBuf-len, "T%s", twoDigits );
  len = len + 3; // 2 + 1 for character "T"
  
  /* convert minutes */
  num = jdate.wMinute;index=1;
  twoDigits[0]='0';  twoDigits[1]='0';  
  while ( (num != 0) && (index >= 0) )
  {
    rem = num%10;
    num = num/10;
    twoDigits[index--] = (char)(rem + 48);
  }
  SNPRINTF( pBuf+len, sizeofpBuf-len, "%s", twoDigits );
  len = len + 2;
  
  /* convert secs */
  num = jdate.wSecond;index=1;
  twoDigits[0]='0';  twoDigits[1]='0';  
  while ( (num != 0) && (index >= 0) )
  {
    rem = num%10;
    num = num/10;
    twoDigits[index--] = (char)(rem + 48);
  }
  SNPRINTF( pBuf+len, sizeofpBuf-len, "%s", twoDigits );
  return;
}

/* ==========================================================================
FUNCTION BTApp_PBAPFindNumberOfABItems
DESCRIPTION
============================================================================= */
static uint16 BTApp_PBAPFindNumberOfABItems( CBTApp * pMe, boolean bFromSIM )
{
  int         Status = EFAILED;
  IAddrRec*   addrRec=NULL;
  AEEAddrCat  category = AEE_ADDR_CAT_NONE;
  boolean     bSIMRec = FALSE;
  uint16      uNoRecs = 0;

  MSG_LOW( "PBAPFindNumberOfABItems - no of items in AB", 0, 0, 0 );
  /* not implemented the search feature */

  /* initialize the address book */
  Status = IADDRBOOKEXT_EnumRecInitEx( pMe->mPBAP.pAddrbook, AEE_ADDR_CAT_NONE,
                                  AEE_ADDRFIELD_NONE, NULL, 0, 0 );
  if ( Status == EFAILED )
  {
    MSG_HIGH( "PBAPBuildABObject - EnumRecInit return Failed", 0, 0, 0 );
    return FALSE;
  }
  /* start reading the records */
  while ( (addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook )) != NULL )
  {
    category = IADDRREC_GetCategory(addrRec);
    bSIMRec = BTApp_PBAPIsSIMRec(category);
    if ( bSIMRec == bFromSIM )
    {
      uNoRecs++;
    }
    IADDRREC_Release( addrRec );
  }
  return uNoRecs;
}

/* ==========================================================================
FUNCTION BTApp_PBAPAddVCardProperties
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPAddVCardProperties( CBTApp* pMe, IAddrRec* addrRec,
                                             IFile* pIFile)
{
  AEEAddrField*          addrField;
  uint8       uFieldCount = 0;
  uint8       i = 0;
  char*       pName = NULL;
  char*       pFirstName = NULL;
  char*       pMiddleName = NULL;
  char*       pLastName = NULL;
  char*       pOtherName = NULL;
  char*       pNameSuffix = NULL;
  char*       pNamePrefix = NULL;
  char*       pPhoneWork = NULL;
  char*       pPhoneHome = NULL;
  char*       pPhoneVoice = NULL;
  char*       pPhoneCell = NULL;
  char*       pPhoneGeneric = NULL;  
  char*       pEmail = NULL;
  char*       pBuffer = NULL;
  uint8       len = 0;
  boolean     bNameAvail = FALSE;

  if ( addrRec == NULL || pIFile == NULL )
  {
    return FALSE;
  }
  /* add record fields to vcard object */
  uFieldCount = IADDRREC_GetFieldCount( addrRec );
  for ( i = 0; i < uFieldCount; i++ )
  {
    addrField = IADDRREC_GetField( addrRec, i );
    /* add name */
    if ( (addrField->fID == AEE_ADDRFIELD_NAME) && 
         (addrField->wDataLen != 0) )
    {
      pName = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pName == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR( addrField->pBuffer, pName, addrField->wDataLen + 1 );
    }
    /* save first name */
    if ( (addrField->fID == AEE_ADDRFIELD_FIRSTNAME) && 
         (addrField->wDataLen != 0) )
    {
      pFirstName = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pFirstName == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR( addrField->pBuffer, pFirstName, addrField->wDataLen + 1 );
    }
    /* save middle name */
    if ( (addrField->fID == AEE_ADDRFIELD_MIDDLENAME) && 
         (addrField->wDataLen != 0) )
    {
      pMiddleName = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pMiddleName == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR( addrField->pBuffer, pMiddleName, addrField->wDataLen + 1 );
    }
    /* save last name */
    if ( (addrField->fID == AEE_ADDRFIELD_LASTNAME) &&
         (addrField->wDataLen != 0) )
    {
      pLastName = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pLastName == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR( addrField->pBuffer, pLastName, addrField->wDataLen + 1 );
    }
    /* save other name */
    if ( (addrField->fID == AEE_ADDRFIELD_OTHERNAME) &&
         (addrField->wDataLen != 0) )
    {
      pOtherName = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pOtherName == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR( addrField->pBuffer, pOtherName, addrField->wDataLen + 1 );
    }
    /* save name prefix */
    if ( (addrField->fID == AEE_ADDRFIELD_NAMEPREFIX) &&
         (addrField->wDataLen != 0) )
    {
      pNamePrefix = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pNamePrefix == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR( addrField->pBuffer, pNamePrefix, addrField->wDataLen + 1 );
    }
    /* save name sufix */
    if ( (addrField->fID == AEE_ADDRFIELD_NAMESUFFIX) &&
         (addrField->wDataLen != 0) )
    {
      pNameSuffix = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pNameSuffix == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR( addrField->pBuffer, pNameSuffix, addrField->wDataLen + 1 );
    }
    /* save phone work */
    if ( (addrField->fID == AEE_ADDRFIELD_PHONE_WORK) &&
         (addrField->wDataLen != 0) )
    {
      pPhoneWork = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pPhoneWork == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR( addrField->pBuffer, pPhoneWork, addrField->wDataLen + 1 );
    }
    /* save phone home */
    if ( (addrField->fID == AEE_ADDRFIELD_PHONE_HOME) &&
         (addrField->wDataLen != 0) )
    {
      pPhoneHome = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pPhoneHome == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR( addrField->pBuffer, pPhoneHome, addrField->wDataLen + 1 );
    }
    /* save phone voice */
    if ( (addrField->fID == AEE_ADDRFIELD_PHONE_VOICE) &&
         (addrField->wDataLen != 0) )
    {
      pPhoneVoice = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pPhoneVoice == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR(  addrField->pBuffer, pPhoneVoice, addrField->wDataLen + 1 );
    }
    /* save phone cell */
    if ( (addrField->fID == AEE_ADDRFIELD_PHONE_CELL) &&
         (addrField->wDataLen != 0) )
    {
      pPhoneCell = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pPhoneCell == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR(  addrField->pBuffer, pPhoneCell, addrField->wDataLen + 1 );
    }
    /* save phone generic */
    if ( (addrField->fID == AEE_ADDRFIELD_PHONE_GENERIC) &&
         (addrField->wDataLen != 0) )
    {
      pPhoneGeneric = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pPhoneGeneric == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR(  addrField->pBuffer, pPhoneGeneric, addrField->wDataLen + 1 );
    }
    /* save email address */
    if ( (addrField->fID == AEE_ADDRFIELD_EMAIL) &&
         (addrField->wDataLen != 0) )
    {
      pEmail = (char*)MALLOC( addrField->wDataLen + 1 );
      if ( pEmail == NULL )
      {
        MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
        return FALSE;
      }
      WSTRTOSTR(  addrField->pBuffer, pEmail, addrField->wDataLen + 1 );
    }
  }
  /* buff to store formated strings */
  pBuffer = (char*)MALLOC( LONG_TEXT_BUF_LEN );
  if ( pBuffer == NULL )
  {
    MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
    return FALSE;
  }

  /* N: is a mandatory field for either case 2_1 and 3_0 so */
  /* there need not be any check for filling in that information */
  /* The only check required is for FN as below: */
  /* if ((FORMAT == 3_0) || */
  /*       (filter & AEEBT_PBAP_ATTRIBUTE_FN) || */
  /*          (filter == 0)) */

  {
    SNPRINTF( pBuffer, LONG_TEXT_BUF_LEN, "N:" );
    len = STRLEN( pBuffer );
    if ( pLastName != NULL )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, "%s", pLastName );
      bNameAvail = TRUE;
    }
    len = STRLEN( pBuffer );
    if ( pFirstName != NULL )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, ";%s", pFirstName );
      bNameAvail = TRUE;
    }
    len = STRLEN( pBuffer );
    if ( pMiddleName != NULL )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, ";%s", pMiddleName );
      bNameAvail = TRUE;
    }
    len = STRLEN( pBuffer );
    if ( (bNameAvail == FALSE) && (pName != NULL) )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, "%s", pName );
    }
    len = STRLEN( pBuffer );
    if ( pNamePrefix != NULL )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, ";%s", pNamePrefix );
    }
    len = STRLEN( pBuffer );
    if ( pNameSuffix != NULL )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, ";%s", pNameSuffix );
    }
    len = STRLEN( pBuffer );
    SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, "\r\n" );
    IFILE_Write( pMe->mPBAP.pIFile, pBuffer, STRLEN(pBuffer) );
  }
      
  bNameAvail = FALSE;
  /* if vcard ver=3.0, its mandatory to add FORMAT NAME propoerty */
  if ( (pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_3_0) ||
       (pMe->mPBAP.PBAPObject.filter & AEEBT_PBAP_ATTRIBUTE_FN) ||
       (pMe->mPBAP.PBAPObject.filter == 0) )
  {
    SNPRINTF( pBuffer, LONG_TEXT_BUF_LEN, "FN:" );
    len = STRLEN( pBuffer );
    if ( pNamePrefix != NULL )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, "%s", pNamePrefix );
    }
    len = STRLEN( pBuffer );
    if ( pFirstName != NULL )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, " %s", pFirstName );
      bNameAvail = TRUE;
    }
    len = STRLEN( pBuffer );
    if ( pMiddleName != NULL )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, " %s", pMiddleName );
      bNameAvail = TRUE;
    }
    len = STRLEN( pBuffer );
    if ( pLastName != NULL )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, " %s", pLastName );
      bNameAvail = TRUE;      
    }
    len = STRLEN( pBuffer ); // when no name available
    if ( (bNameAvail == FALSE) && (pName != NULL) )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, "%s", pName );
    }
    len = STRLEN( pBuffer );
    if ( pNameSuffix != NULL )
    {
      SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, " %s", pNameSuffix );
    }
    len = STRLEN( pBuffer );
    SNPRINTF( (pBuffer+len), LONG_TEXT_BUF_LEN - len, "\r\n" );
    IFILE_Write( pMe->mPBAP.pIFile, pBuffer, STRLEN(pBuffer) );
  }
  /* free memory allocated for storing name strings */
  FREEIF( pFirstName );      
  FREEIF( pMiddleName );
  FREEIF( pNamePrefix );         
  FREEIF( pNameSuffix );          
  

  /* TEL is a mandatory entry in both versions of Vcard */
  SNPRINTF( pBuffer, LONG_TEXT_BUF_LEN, "TEL;WORK;VOICE:");
  IFILE_Write( pMe->mPBAP.pIFile, pBuffer, STRLEN(pBuffer) );
  /* add telephone numbers to vcard - work, home, cell, voice */
  if ( pPhoneWork != NULL )
  {
    SNPRINTF( pBuffer, LONG_TEXT_BUF_LEN, "%s", pPhoneWork );
    IFILE_Write( pMe->mPBAP.pIFile, pBuffer, STRLEN(pBuffer) );
    FREE( (void*)pPhoneWork );
   }
  SNPRINTF( pBuffer, LONG_TEXT_BUF_LEN, "\r\n");
  IFILE_Write( pMe->mPBAP.pIFile, pBuffer, STRLEN(pBuffer) );


  if ( pPhoneHome != NULL )
  {
    SNPRINTF( pBuffer, LONG_TEXT_BUF_LEN, "TEL;HOME;VOICE:%s\r\n", pPhoneHome );
    IFILE_Write( pMe->mPBAP.pIFile, pBuffer, STRLEN(pBuffer) );
    FREE( (void*)pPhoneHome );
  }
  if ( pPhoneCell != NULL )
  {
    SNPRINTF( pBuffer, LONG_TEXT_BUF_LEN, "TEL;CELL;VOICE:%s\r\n", pPhoneCell );
    IFILE_Write( pMe->mPBAP.pIFile, pBuffer, STRLEN(pBuffer) );
    FREE( (void*)pPhoneCell );
  }
  if ( pPhoneGeneric != NULL )
  {
    SNPRINTF( pBuffer, LONG_TEXT_BUF_LEN, "TEL;VOICE:%s\r\n", pPhoneGeneric );
    IFILE_Write( pMe->mPBAP.pIFile, pBuffer, STRLEN(pBuffer) );
    FREE( (void*)pPhoneGeneric );
  }
  if ( pPhoneVoice != NULL )
  {
    SNPRINTF( pBuffer, LONG_TEXT_BUF_LEN, "TEL;VOICE:%s\r\n", pPhoneVoice );
    IFILE_Write( pMe->mPBAP.pIFile, pBuffer, STRLEN(pBuffer) );
    FREE( (void*)pPhoneVoice );
  }
  
  /* add email address */
  /* 
   * The email shall be sent only when requested ie either the filter 
   * explicitly says or a zero value for the filter is received 
   */
  if ( ( pEmail != NULL ) && 
       ( (pMe->mPBAP.PBAPObject.filter & AEEBT_PBAP_ATTRIBUTE_EMAIL) ||
         (pMe->mPBAP.PBAPObject.filter == 0) ) )
  {
    SNPRINTF( pBuffer, LONG_TEXT_BUF_LEN, "EMAIL;PREF;INTERNET:%s\r\n", pEmail );
    IFILE_Write( pMe->mPBAP.pIFile, pBuffer, STRLEN(pBuffer) );
  }
  /* free mem */
  FREEIF( pBuffer );
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildCCHVCardEntry
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPBuildCCHVCardEntry( CBTApp* pMe, char* ObjName )
{
  const AEECallHistoryEntry*  pCallHistoryEntry = NULL;
  AEECallHistoryField*        pItems = NULL;
  char*       pTemp = NULL;
  char        sRecID[8];
  int16       RecID = 0;
  int         uResult = SUCCESS;
  int         Status;
  int         i=0;
  IAddrRec*   addrRec = NULL;
  boolean     bRecordFound = FALSE;
  char        sTimeStamp[BT_APP_PBAP_CALLHIST_MAXDIGITS + 1];
  char        sBuf[150];
  uint8       callType;
  dword       dTimeStamp = 0;  
  AECHAR      wNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS + 1];
  AECHAR      wFileName[AEEBT_MAX_FILE_NAME + 1];
  
  /*extract recID from ObjName*/ 
  pTemp = STRRCHR( ObjName, '.');
  MEMCPY( sRecID, ObjName, (pTemp-ObjName) );
  sRecID[pTemp-ObjName] = NULL;
  RecID = BTApp_PBAPHexStr2Int( sRecID );
  if ( RecID < 0 )
  {
    MSG_HIGH( "PBAPBuildCHVCardEntry - invalid handle", 0, 0, 0 );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                       AEEBT_ERR_BAD_PARAM );    
    IFILE_Release( pMe->mPBAP.pIFile );                                       
    return FALSE;  
  }
  
  MSG_LOW( "PBAPBuildCCHVCardEntry - RecID=%d", RecID, 0, 0 );
  
  ICALLHISTORY_EnumInit( pMe->mPBAP.pCallHist );
  pCallHistoryEntry = ICALLHISTORY_EnumNext( pMe->mPBAP.pCallHist, &Status);
  i++;
  while ( (pCallHistoryEntry != NULL) && (Status == SUCCESS) && (i < RecID) )
  {
    pCallHistoryEntry = ICALLHISTORY_EnumNext( pMe->mPBAP.pCallHist, &Status);
    i++;
  }
  if ( (pCallHistoryEntry == NULL) || (Status != SUCCESS) )
  {
    MSG_LOW( "PBAPBuildCCHVCardEntry - no rec with this id", 0, 0, 0 );
    uResult = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                                 AEEBT_ERR_OBEX_NOT_FOUND );
    if ( uResult != SUCCESS )
    {
      MSG_ERROR( "PBAPBuildCCHVCardEntry - IBT returned failure", 0, 0, 0 );
    }
    return FALSE;
  }
  /* create vcard file */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, ObjName ) == SUCCESS )
  {
    if ( (pMe->mPBAP.pIFile = IFILEMGR_OpenFile(pMe->mPBAP.pIFileMgr, 
                                      ObjName,  _OFM_READWRITE)) != NULL )
    {
      if ( IFILE_Truncate( pMe->mPBAP.pIFile, 0 ) != SUCCESS )
      {
        MSG_ERROR( "PBAP - can't truncate dirList file", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );
        IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                           AEEBT_ERR_RESOURCE );   
        return FALSE;
      }
    }
  }
  else
  {
    /* create vcard file */
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           ObjName, _OFM_CREATE );
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_HIGH( "PBAPBuildCCHVCardEntry - File open failed", 0, 0, 0 );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                           AEEBT_ERR_RESOURCE );    
    return FALSE;
  }
  
  /* building vcard */
  pItems = pCallHistoryEntry->pFields;
  if ( (pCallHistoryEntry->wNumFields != 0) && 
       (pItems != NULL) )
  {
    callType = 0;
    MEMSET( wNumber, 0, sizeof(wNumber) );
    for ( i = 0; i < pCallHistoryEntry->wNumFields; i++ )
    {
      switch (pItems[i].wID)
      {
        case AEECALLHISTORY_FIELD_DATE_TIME:
        {
          MEMCPY( &dTimeStamp, pItems[i].pData, pItems[i].wDataLen );
          break;
        }
        case AEECALLHISTORY_FIELD_NUMBER_EX:
        {
          MEMCPY( wNumber, pItems[i].pData, 
                  MIN(ARR_SIZE(wNumber)*sizeof(AECHAR), 
                      pItems[i].wDataLen) );
          break;
        }
        case AEECALLHISTORY_FIELD_CALL_TYPE:
        {
          callType = *( (uint8*)pItems[i].pData );
          break;
        }
        default:
          break;
      }
    }
    /* find if there is an entry corresponding to this number in AB */
    bRecordFound = FALSE;
    if ( SUCCESS == IADDRBOOKEXT_EnumRecInitEx( pMe->mPBAP.pAddrbook,
                                                AEE_ADDR_CAT_NONE,
                                                AEE_ADDRFIELD_NONE,
                                                wNumber,
                                                (uint16) WSTRSIZE(wNumber),
                                                ADDRREC_SEARCHTYPE_EXACT |
                                                ADDRREC_FIELDID_PHONE ) ) 
    {
      addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook );
      /* there might be multiple records, add only the first record */
      if ( NULL != addrRec ) 
      {
        bRecordFound = TRUE;
        IFILE_Write( pMe->mPBAP.pIFile, "BEGIN:VCARD\r\n", 
                     STRLEN("BEGIN:VCARD\r\n") );
        if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
        { 
          IFILE_Write( pMe->mPBAP.pIFile, "VERSION:2.1\r\n", 
                       STRLEN("VERSION:2.1\r\n") );
        }
        else
        {
          IFILE_Write( pMe->mPBAP.pIFile, "VERSION:3.0\r\n", 
                       STRLEN("VERSION:3.0\r\n") );
        }
        /* add this record to the vcard */
        if ( BTApp_PBAPAddVCardProperties( pMe, addrRec, 
                                           pMe->mPBAP.pIFile ) == FALSE )
        {
          MSG_HIGH( "PBAPBuildCCHVCardEntry- err adding vcard prop", 0, 0, 0 );
        }

        if ( ( pMe->mPBAP.PBAPObject.filter & AEEBT_PBAP_ATTRIBUTE_TIMESTAMP ) ||
             ( pMe->mPBAP.PBAPObject.filter == 0 ) )
        {
          /* 
           * If the attribute mask shows that the client 
           * has requested for the time stamp 
           */
          /* add time stamp property to this vcard */
          BTApp_PBAPFormatTime( pMe, &dTimeStamp, sTimeStamp, 
                                sizeof(sTimeStamp) );
          if ( callType == AEECALLHISTORY_CALL_TYPE_FROM )
          {
            SNPRINTF( sBuf, sizeof(sBuf), 
                      "X-IRMC-CALL-DATETIME;RECEIVED:%s\r\n", sTimeStamp );
          }
          else if ( callType == AEECALLHISTORY_CALL_TYPE_TO )
          {
            SNPRINTF( sBuf, sizeof(sBuf), 
                      "X-IRMC-CALL-DATETIME;DIALLED:%s\r\n", sTimeStamp );
          }
          else if ( callType == AEECALLHISTORY_CALL_TYPE_MISSED )
          {
            SNPRINTF( sBuf, sizeof(sBuf), 
                      "X-IRMC-CALL-DATETIME;MISSED:%s\r\n", sTimeStamp );
          }
          IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
        }

        IFILE_Write( pMe->mPBAP.pIFile, "END:VCARD\r\n", 
                     STRLEN("END:VCARD\r\n") );
      }
    }
    if ( bRecordFound == FALSE )
    {
      /* no record found, add just the time stamp property*/
      IFILE_Write( pMe->mPBAP.pIFile, "BEGIN:VCARD\r\n", 
                   STRLEN("BEGIN:VCARD\r\n") );
      if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
      { 
        IFILE_Write( pMe->mPBAP.pIFile, "VERSION:2.1\r\n", 
                     STRLEN("VERSION:2.1\r\n") );
      }
      else
      {
        IFILE_Write( pMe->mPBAP.pIFile, "VERSION:3.0\r\n", 
                     STRLEN("VERSION:3.0\r\n") );
      }

      if ( ( pMe->mPBAP.PBAPObject.filter & AEEBT_PBAP_ATTRIBUTE_TIMESTAMP ) ||
           ( pMe->mPBAP.PBAPObject.filter == 0 ) )
      {
        /* 
         * If the attribute mask shows that the client 
         * has requested for the time stamp 
         */
        BTApp_PBAPFormatTime( pMe, &dTimeStamp, sTimeStamp, 
                              sizeof(sTimeStamp) );
        if ( callType == AEECALLHISTORY_CALL_TYPE_FROM )
        {
          SNPRINTF( sBuf, sizeof(sBuf), 
                    "X-IRMC-CALL-DATETIME;RECEIVED:%s\r\n", sTimeStamp );
        }
        else if ( callType == AEECALLHISTORY_CALL_TYPE_TO )
        {
          SNPRINTF( sBuf, sizeof(sBuf), 
                    "X-IRMC-CALL-DATETIME;DIALLED:%s\r\n", sTimeStamp );
        }
        else if ( callType == AEECALLHISTORY_CALL_TYPE_MISSED )
        {
          SNPRINTF( sBuf, sizeof(sBuf), 
                    "X-IRMC-CALL-DATETIME;MISSED:%s\r\n", sTimeStamp );
        }
        IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
      }
      IFILE_Write( pMe->mPBAP.pIFile, "END:VCARD\r\n", 
                   STRLEN("END:VCARD\r\n") );
    }
    IFILE_Release( pMe->mPBAP.pIFile );
    STRTOWSTR( ObjName, wFileName, sizeof(wFileName) );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, wFileName, 
                                       AEEBT_ERR_NONE );
    return TRUE;
  }
  else
  {
    IFILE_Release( pMe->mPBAP.pIFile );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
      AEEBT_ERR_OBEX_NOT_FOUND );
    return FALSE;
  }
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildCHVCardEntry
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPBuildCHVCardEntry( CBTApp* pMe, char* ObjName,
                                            uint8 uCallType )
{
  const AEECallHistoryEntry*  pCallHistoryEntry = NULL;
  AEECallHistoryField*        pItems = NULL;
  char*       pTemp = NULL;
  char        sRecID[8];
  int16       RecID = 0;
  int         uResult = SUCCESS;
  int         Status;
  int         i = 0;
  IAddrRec*   addrRec = NULL;
  boolean     bRecordFound = FALSE;
  char        sTimeStamp[BT_APP_PBAP_CALLHIST_MAXDIGITS + 1];
  char        sBuf[150];
  char*       pTimePropStr = NULL;
  dword       dTimeStamp = 0;  
  AECHAR      wNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS + 1];
  char        sNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS + 1];
  AECHAR      wFileName[AEEBT_MAX_FILE_NAME + 1];
  
  /*extract recID from ObjName*/ 
  pTemp = STRRCHR( ObjName, '.');
  MEMCPY( sRecID, ObjName, (pTemp-ObjName) );
  sRecID[pTemp-ObjName] = NULL;
  RecID = BTApp_PBAPHexStr2Int( sRecID );
  if ( RecID < 0 )
  {
    MSG_HIGH( "PBAPBuildCHVCardEntry - invalid handle", 0, 0, 0 );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                       AEEBT_ERR_BAD_PARAM );    
    IFILE_Release( pMe->mPBAP.pIFile );                                       
    return FALSE;  
  }

  MSG_LOW( "PBAPBuildCHVCardEntry - RecID=%d", RecID, 0, 0 );
  
  ICALLHISTORY_EnumInitByCallType( pMe->mPBAP.pCallHist, uCallType );
  pCallHistoryEntry = ICALLHISTORY_EnumNext( pMe->mPBAP.pCallHist, &Status);
  i++;
  while ( (pCallHistoryEntry != NULL) && (Status == SUCCESS) && (i < RecID) )
  {
    pCallHistoryEntry = ICALLHISTORY_EnumNext( pMe->mPBAP.pCallHist, &Status);
    i++;
  }
  if ( (pCallHistoryEntry == NULL) || (Status != SUCCESS) )
  {
    MSG_LOW( "PBAPBuildCHVCardEntry - no rec with this id", 0, 0, 0 );
    uResult = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                                 AEEBT_ERR_OBEX_NOT_FOUND );
    if ( uResult != SUCCESS )
    {
      MSG_ERROR( "PBAPBuildCHVCardEntry - IBT returned failure", 0, 0, 0 );
    }    
    return FALSE;    
  }
  /* create vcard file */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, ObjName ) == SUCCESS )
  {
    if ( (pMe->mPBAP.pIFile = IFILEMGR_OpenFile(pMe->mPBAP.pIFileMgr, 
                                      ObjName,  _OFM_READWRITE)) != NULL )
    {
      if ( IFILE_Truncate( pMe->mPBAP.pIFile, 0 ) != SUCCESS )
      {
        MSG_ERROR( "PBAP - can't truncate dirList file", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );
        IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                           AEEBT_ERR_RESOURCE );   
        return FALSE;
      }
    }
  }
  else
  {
    /* create vcard file */
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           ObjName, _OFM_CREATE );
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_HIGH( "PBAPBuildCHVCardEntry - File open failed", 0, 0, 0 );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                       AEEBT_ERR_RESOURCE );
    return FALSE;
  }
  /* building vcard */
  if ( uCallType == AEECALLHISTORY_CALL_TYPE_FROM )
  {
    pTimePropStr = "RECEIVED";
  }
  else if ( uCallType == AEECALLHISTORY_CALL_TYPE_TO )
  {
    pTimePropStr = "DIALLED";
  }
  else if ( uCallType == AEECALLHISTORY_CALL_TYPE_MISSED )
  {
    pTimePropStr = "MISSED";      
  }

  pItems = pCallHistoryEntry->pFields;
  if ( (pCallHistoryEntry->wNumFields != 0) && 
       (pItems != NULL) )
  {
    MEMSET( wNumber, 0, sizeof(wNumber) );
    for ( i = 0; i < pCallHistoryEntry->wNumFields; i++ )
    {
      switch (pItems[i].wID)
      {
        case AEECALLHISTORY_FIELD_DATE_TIME:
        {
          MEMCPY( &dTimeStamp, pItems[i].pData, pItems[i].wDataLen );
          break;
        }
        case AEECALLHISTORY_FIELD_NUMBER_EX:
        {
          MEMCPY( wNumber, pItems[i].pData, 
                  MIN(ARR_SIZE(wNumber)*sizeof(AECHAR), 
                      pItems[i].wDataLen) );
          break;
        }
        default:
          break;
      }
    }
    /* find if there is an entry corresponding to this number in AB */
    bRecordFound = FALSE;
    if ( SUCCESS == IADDRBOOKEXT_EnumRecInitEx( pMe->mPBAP.pAddrbook,
                                                AEE_ADDR_CAT_NONE,
                                                AEE_ADDRFIELD_NONE,
                                                wNumber,
                                                (uint16) WSTRSIZE(wNumber),
                                                ADDRREC_SEARCHTYPE_EXACT |
                                                ADDRREC_FIELDID_PHONE ) ) 
    {
      addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook );
      /* there might be multiple records, add only the first record */
      if ( NULL != addrRec ) 
      {
        bRecordFound = TRUE;
        IFILE_Write( pMe->mPBAP.pIFile, "BEGIN:VCARD\r\n", 
                     STRLEN("BEGIN:VCARD\r\n") );
        if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
        { 
          IFILE_Write( pMe->mPBAP.pIFile, "VERSION:2.1\r\n", 
                       STRLEN("VERSION:2.1\r\n") );
        }
        else
        {
          IFILE_Write( pMe->mPBAP.pIFile, "VERSION:3.0\r\n", 
                       STRLEN("VERSION:3.0\r\n") );
        }
        /* add this record to the vcard */
        if ( BTApp_PBAPAddVCardProperties( pMe, addrRec, 
                                           pMe->mPBAP.pIFile ) == FALSE )
        {
          MSG_HIGH( "PBAPBuildCHVCardEntry - err adding vcard prop", 0, 0, 0 );
        }
        if ( ( pMe->mPBAP.PBAPObject.filter & AEEBT_PBAP_ATTRIBUTE_TIMESTAMP ) ||
             (pMe->mPBAP.PBAPObject.filter == 0) )
        {
          /* 
           * If the attribute mask shows that the client 
           * has requested for the time stamp 
           */

          /* add time stamp property to this vcard */
          BTApp_PBAPFormatTime( pMe, &dTimeStamp, sTimeStamp,
                                sizeof(sTimeStamp) );
          SNPRINTF( sBuf, sizeof(sBuf), "X-IRMC-CALL-DATETIME;%s:%s\r\n", 
                    pTimePropStr, sTimeStamp );
          IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
        }
        IFILE_Write( pMe->mPBAP.pIFile, "END:VCARD\r\n", 
                     STRLEN("END:VCARD\r\n") );
      }
    }
    if ( bRecordFound == FALSE )
    {
      /* no record found, add just the time stamp property*/
      IFILE_Write( pMe->mPBAP.pIFile, "BEGIN:VCARD\r\n", 
                   STRLEN("BEGIN:VCARD\r\n") );
      if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
      { 
        IFILE_Write( pMe->mPBAP.pIFile, "VERSION:2.1\r\n", 
                     STRLEN("VERSION:2.1\r\n") );
      }
      else
      {
        IFILE_Write( pMe->mPBAP.pIFile, "VERSION:3.0\r\n", 
                     STRLEN("VERSION:3.0\r\n") );
      }
      /* add number property present in call history */
      WSTRTOSTR( wNumber, sNumber, sizeof(sNumber) );
      SNPRINTF( sBuf, sizeof(sBuf), "TEL;VOICE:%s\r\n", sNumber );

      if ( ( pMe->mPBAP.PBAPObject.filter & AEEBT_PBAP_ATTRIBUTE_TIMESTAMP )  ||
           (pMe->mPBAP.PBAPObject.filter == 0) )
      {
        /* 
         * If the attribute mask shows that the client 
         * has requested for the time stamp 
         */

        BTApp_PBAPFormatTime( pMe, &dTimeStamp, sTimeStamp,
                              sizeof(sTimeStamp) );
        SNPRINTF( sBuf, sizeof(sBuf), "X-IRMC-CALL-DATETIME;%s:%s\r\n", 
                  pTimePropStr, sTimeStamp );
        IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
      }

      IFILE_Write( pMe->mPBAP.pIFile, "END:VCARD\r\n", 
                   STRLEN("END:VCARD\r\n") );            
    }
    IFILE_Release( pMe->mPBAP.pIFile );
    STRTOWSTR( ObjName, wFileName, sizeof(wFileName) );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, wFileName, 
                                       AEEBT_ERR_NONE );
    return TRUE;
  }
  else
  {
    IFILE_Release( pMe->mPBAP.pIFile );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
      AEEBT_ERR_OBEX_NOT_FOUND );
    return FALSE;
  }
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildABVCardEntry
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPBuildABVCardEntry( CBTApp* pMe, char* ObjName,
                                            boolean bFromSIM )
{
  char*       pTemp = NULL;
  char         sRecID[8];
  int16       RecID = 0;
  IAddrRec*   addrRec = NULL;
  int         uResult = SUCCESS;
  AEEAddrCat  category = AEE_ADDR_CAT_NONE;
  boolean     bRecFromSIM;
  AECHAR      wFileName[AEEBT_MAX_FILE_NAME + 1];

  STRTOWSTR( ObjName, wFileName, sizeof(wFileName) );
  /* check if file with this obj name already exists and truncate */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, ObjName ) == SUCCESS )
  {
    if ( (pMe->mPBAP.pIFile = IFILEMGR_OpenFile(pMe->mPBAP.pIFileMgr, 
                                      ObjName,  _OFM_READWRITE)) != NULL )
    {
      if ( IFILE_Truncate( pMe->mPBAP.pIFile, 0 ) != SUCCESS )
      {
        MSG_ERROR( "PBAP - can't truncate dirList file", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );
        IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                       AEEBT_ERR_RESOURCE );   
        return FALSE;
      }
    }
  }
  else
  {
    /* create vcard file */
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           ObjName, _OFM_CREATE );
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_HIGH( "PBAPBuildABVCardEntry - File open failed", 0, 0, 0 );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                       AEEBT_ERR_RESOURCE );    
    return FALSE;
  }

  /*extract recID from ObjName*/ 
  pTemp = STRRCHR( ObjName, '.');
  MEMCPY( sRecID, ObjName, (pTemp-ObjName) );
  sRecID[pTemp-ObjName] = NULL;
  RecID = BTApp_PBAPHexStr2Int( sRecID );
  if ( RecID < 0 )
  {
    MSG_HIGH( "PBAPBuildABVCardEntry - invalid handle", 0, 0, 0 );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                       AEEBT_ERR_BAD_PARAM );    
    IFILE_Release( pMe->mPBAP.pIFile );                                       
    return FALSE;  
  }
  
  MSG_LOW( "PBAPBuildABVCardEntry - RecID=%d", RecID, 0, 0 );
  
  /* if its 0th record then pass on the phone's vcard */
  if ( RecID == 0 )
  {
    if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
    {
      IFILE_Write( pMe->mPBAP.pIFile, BT_APP_VCARD2_1, 
                   STRLEN(BT_APP_VCARD2_1) );
    }
    else
    { 
      IFILE_Write( pMe->mPBAP.pIFile, BT_APP_VCARD3_0, 
                   STRLEN(BT_APP_VCARD3_0) );
    }
    IFILE_Release( pMe->mPBAP.pIFile );
    uResult = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, wFileName, 
                                                 AEEBT_ERR_NONE );
    if ( uResult != SUCCESS )
    {
      MSG_ERROR( "PBAPBuildABVCardEntry - IBT returned failure", 0, 0, 0 );
      return FALSE;
    }    
    return TRUE;
  }
  
  /* read record from address book and create a vcard */
  addrRec = IADDRBOOK_GetRecByID( pMe->mPBAP.pAddrbook, RecID );
  if ( addrRec == NULL )
  {
    MSG_HIGH( "PBAPBuildABVCardEntry - cannot find rec in AB", 0, 0, 0 );
    uResult = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                                 AEEBT_ERR_OBEX_NOT_FOUND );
    if ( uResult != SUCCESS )
    {
      MSG_ERROR( "PBAPBuildABVCardEntry - IBT returned failure", 0, 0, 0 );
    }
    IFILE_Release( pMe->mPBAP.pIFile );
    return FALSE;
  }
  else
  {
    /* if the record requested is from SIM or Phone */
    category = IADDRREC_GetCategory(addrRec);
    bRecFromSIM = BTApp_PBAPIsSIMRec(category);
    if ( bRecFromSIM == bFromSIM )
    {
      /* building vcard */    
      IFILE_Write( pMe->mPBAP.pIFile, "BEGIN:VCARD\r\n", 
                   STRLEN("BEGIN:VCARD\r\n") );
      if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
      {
        IFILE_Write( pMe->mPBAP.pIFile, "VERSION:2.1\r\n", 
                     STRLEN("VERSION:2.1\r\n") );
      }
      else if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_3_0 )
      {
        IFILE_Write( pMe->mPBAP.pIFile, "VERSION:3.0\r\n", 
                     STRLEN("VERSION:3.0\r\n") );
      }
      if ( BTApp_PBAPAddVCardProperties( pMe, addrRec, 
                                        pMe->mPBAP.pIFile ) == FALSE )
      {
        MSG_HIGH( "PBAPBuildABVCardEntry - error adding vcard prop", 0, 0, 0 );
      }
      IFILE_Write( pMe->mPBAP.pIFile, "END:VCARD\r\n", STRLEN("END:VCARD\r\n") );
      IFILE_Release( pMe->mPBAP.pIFile );
      IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, wFileName, 
                                         AEEBT_ERR_NONE );
      return TRUE;
    }
    else
    {
      MSG_HIGH( "PBAPBuildABVCardEntry - cannot find rec in AB", 0, 0, 0 );
      uResult = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                                   AEEBT_ERR_OBEX_NOT_FOUND );
      if ( uResult != SUCCESS )
      {
        MSG_ERROR( "PBAPBuildABVCardEntry - IBT returned failure", 0, 0, 0 );
      }
      IFILE_Release( pMe->mPBAP.pIFile );
      return FALSE;
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_PBAPServerPullVCardEntryRsp
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPServerPullVCardEntryRsp( CBTApp* pMe )
{
  char*       pBuf = NULL;
  char         ObjName[ AEEBT_MAX_FILE_NAME + 1 ];
  AECHAR      wFileName[ AEEBT_MAX_FILE_NAME + 1 ];
  boolean     bFromSIM = FALSE;
  int         result = EFAILED;
  boolean     bStatus = FALSE;
  
  WSTRTOSTR( pMe->mPBAP.PBAPObject.pwObjectName, ObjName, sizeof(ObjName) );
  /* check if the current directory is below telecom */
  pBuf = STRSTR( sServerPath, AEEBT_PBAP_FOLDER_NAME_TELECOM );
  if ( pBuf == NULL )
  {
    MSG_LOW( "PBAPSrvPullVCardEntryRsp - Invalid directory", 0, 0, 0 );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, EFAILED );
    return FALSE;
  }
  /* find out if we are in SIM */
  pBuf = STRSTR( sServerPath, AEEBT_PBAP_FOLDER_NAME_SIM1 );
  if ( pBuf != NULL )
  {
    bFromSIM = TRUE;
  }
  /* extract the current dir */
  pBuf = STRRCHR( sServerPath, '/' );
  if ( pBuf == NULL )
  {
    MSG_LOW( "PBAPSrvPullVCardEntryRsp - Invalid directory path", 0, 0, 0 );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                       AEEBT_ERR_UNKNOWN );
    return FALSE;
  }
  pBuf++;
  /* if the current dir is telecom, then it can be complete pb object */
  if ( STRCMP(pBuf, AEEBT_PBAP_FOLDER_NAME_TELECOM) == 0 )
  {
    if ( STRICMP(ObjName, "pb.vcf") == 0 )
    {
      if ( bFromSIM )
      {
        if ( BTApp_PBAPBuildABObject( pMe, TRUE ) != FALSE )
        {
          STRTOWSTR( SIM_PB_VCF, wFileName, sizeof(wFileName) );
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                                                      wFileName, 
                                                      AEEBT_ERR_NONE );
        }
        else
        {
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                     AEEBT_ERR_OBEX_NOT_FOUND );
        }      
      }
      else
      {
        if ( BTApp_PBAPBuildABObject( pMe, FALSE ) != FALSE )
        {
          STRTOWSTR( PB_VCF, wFileName, sizeof(wFileName) );
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                                                      wFileName, 
                                                      AEEBT_ERR_NONE );
        }
        else
        {
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                     NULL, AEEBT_ERR_OBEX_NOT_FOUND );
        }
      }
    }
    else if ( STRICMP(ObjName, "ich.vcf") == 0 )
    {
      if ( bFromSIM == FALSE )
      {
        if ( BTApp_PBAPBuildCHObject( pMe, 
               AEECALLHISTORY_CALL_TYPE_FROM ) != FALSE )
        {
          STRTOWSTR( ICH_VCF, wFileName, sizeof(wFileName) );
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                     wFileName, AEEBT_ERR_NONE );
        }
        else
        {
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                     NULL, AEEBT_ERR_OBEX_NOT_FOUND );
        }
      }
      else
      {
        result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                   NULL, AEEBT_ERR_OBEX_NOT_FOUND );
      }
    }
    else if ( STRICMP(ObjName, "och.vcf") == 0 )
    {
      if ( bFromSIM == FALSE )
      {
        if ( BTApp_PBAPBuildCHObject( pMe, 
               AEECALLHISTORY_CALL_TYPE_TO ) != FALSE )
        {
          STRTOWSTR( OCH_VCF, wFileName, sizeof(wFileName) );
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                     wFileName, AEEBT_ERR_NONE );
        }
        else
        {
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                     NULL, AEEBT_ERR_OBEX_NOT_FOUND );
        }
      }
      else
      {
        result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                   NULL, AEEBT_ERR_OBEX_NOT_FOUND );
      }
    }
    else if ( STRICMP(ObjName, "mch.vcf") == 0 )
    {
      if ( bFromSIM == FALSE )
      {
        if ( BTApp_PBAPBuildCHObject( pMe, 
               AEECALLHISTORY_CALL_TYPE_MISSED ) != FALSE )
        {
          STRTOWSTR( MCH_VCF, wFileName, sizeof(wFileName) );
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                     wFileName, AEEBT_ERR_NONE );
        }
        else
        {
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                     NULL, AEEBT_ERR_OBEX_NOT_FOUND );
        }
      }    
      else
      {
        result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                   NULL, AEEBT_ERR_OBEX_NOT_FOUND );
      }
    }
    else if ( STRICMP(ObjName, "cch.vcf") == 0 )
    {
      if ( bFromSIM == FALSE )
      {
        if ( BTApp_PBAPBuildCCHObject( pMe ) != FALSE )
        {
          STRTOWSTR( CCH_VCF, wFileName, sizeof(wFileName) );
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                     wFileName, AEEBT_ERR_NONE );
        }
        else
        {
          result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                     NULL, AEEBT_ERR_OBEX_NOT_FOUND );
        }
      }    
      else
      {
        result = IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, 
                   NULL, AEEBT_ERR_OBEX_NOT_FOUND );
      }
    }
    else
    {
      MSG_LOW( "PBAPSrvPullVCardEntryRsp - invalid file name", 0, 0, 0 );
      IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, EFAILED );
      return FALSE;
    }
    bStatus = (result == SUCCESS);
  }
  else if ( STRCMP(pBuf, AEEBT_PBAP_FOLDER_NAME_PB) == 0 )
  {
    bStatus = BTApp_PBAPBuildABVCardEntry( pMe, ObjName, bFromSIM );
  }
  else if ( STRCMP(pBuf, AEEBT_PBAP_FOLDER_NAME_ICH) == 0 )
  {
    bStatus = BTApp_PBAPBuildCHVCardEntry( pMe, ObjName, 
                                           AEECALLHISTORY_CALL_TYPE_FROM );
  }
  else if ( STRCMP(pBuf, AEEBT_PBAP_FOLDER_NAME_OCH) == 0 )
  {
    bStatus = BTApp_PBAPBuildCHVCardEntry( pMe, ObjName, 
                                           AEECALLHISTORY_CALL_TYPE_TO );
  }
  else if ( STRCMP(pBuf, AEEBT_PBAP_FOLDER_NAME_MCH) == 0 )
  {
    bStatus = BTApp_PBAPBuildCHVCardEntry( pMe, ObjName, 
                                           AEECALLHISTORY_CALL_TYPE_MISSED);
  }
  else if ( STRCMP(pBuf, AEEBT_PBAP_FOLDER_NAME_CCH) == 0 )
  {
    bStatus = BTApp_PBAPBuildCCHVCardEntry( pMe, ObjName );
  }
  else
  {
    MSG_LOW( "PBAPSrvPullVCardEntryRsp - invalid path", 0, 0, 0 );
    IBTEXTPBAP_PullvCardEntryResponse( pMe->mPBAP.po, NULL, 
                                       AEEBT_ERR_OBEX_BAD_REQUEST );
    return FALSE;
  }
  return bStatus;
}

/* ==========================================================================
FUNCTION BTApp_PBAPClientBuildVCardListingMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPClientBuildVCardListingMenu( CBTApp* pMe )
{
  IFile*   pIFile = NULL;
  char*    pszFolderContents = NULL;
  char*    pszTagPosition = NULL;
  char*    pszStartPos = NULL;
  char*    pszEndPos = NULL;
  FileInfo fileInfo;
  uint32   dwBytesRead = 0;
  char     szNoOfItems = 0;
  char     szCount = 0;

  // Check whether the file DIR_LIST_NAME exists already or not
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, VLIST_XML) == SUCCESS )
  {
    // File DIR_LIST_NAME exists, open it for reading its contents
    pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                VLIST_XML, 
                                _OFM_READWRITE );
    if ( pIFile == NULL )
    {
      MSG_ERROR( "PBAPClientBuildVCardListingMenu - file open fail", 0, 0, 0 );
      return FALSE;
    }
    // Get the file size for the file DIR_LIST_NAME  
    if ( IFILE_GetInfo( pIFile, &fileInfo ) != SUCCESS )
    {
      MSG_ERROR( "PBAPClientBuildVCardListingMenu - file open fail", 0, 0, 0 );
      return FALSE;
    }
    pszFolderContents = (char*) MALLOC( fileInfo.dwSize+1 );
    if ( pszFolderContents != NULL )
    {
      MEMSET( pszFolderContents, 0, fileInfo.dwSize+1 );
      // Read the contents of the file DIR_LIST_NAME 
      dwBytesRead = IFILE_Read( pIFile, pszFolderContents, 
                                fileInfo.dwSize );
      if ( dwBytesRead == 0 )
      {
        MSG_HIGH( "PBAPClientBuildVCardListingMenu - empty file", 0, 0, 0 );
        return FALSE;
      }
      /* Initialize the required members */
      pMe->mPBAP.uNumOfObj = 0;
      for( szCount=0; szCount<MAX_PBAP_OBJECTS; szCount++ )
      {
        MEMSET( pMe->mPBAP.pbObjects[ szCount ].szObjHdleName, 0, 
                MAX_HANDLE_NAME_LEN+1 );
        MEMSET( pMe->mPBAP.pbObjects[ szCount ].szObjName, 0,
                MAX_HANDLE_NAME_LEN+1 );
      }
      /* extract folders */
      pszTagPosition = pszFolderContents;
      while ( pszTagPosition != NULL )
      {
        /* Look for the tag "folder name" */
        pszTagPosition = STRSTR( pszTagPosition, FOLDER_NAME_STR );
        if ( pszTagPosition == NULL )
        { 
          break;
        }
        /* Look for the start of the double quote */
        pszStartPos = STRCHR( pszTagPosition, SEARCH_CHR );
        if ( pszStartPos == NULL )
        {
          break;
        } 
        /* Look for the end of the double quote */
        pszEndPos = STRCHR( pszStartPos+1, SEARCH_CHR );
        if ( pszEndPos == NULL )
        {
          break;
        }
        if ( pszEndPos - ( pszStartPos+1 ) <= MAX_HANDLE_NAME_LEN )
        {  
          /* Copy the name of the folder in an array */
          MEMCPY( pMe->mPBAP.pbObjects[ szNoOfItems ].szObjHdleName, 
                  pszStartPos+1, pszEndPos - ( pszStartPos+1 ) );
          pMe->mPBAP.pbObjects[ szNoOfItems ].bFolder = TRUE;
          szNoOfItems++;
          if ( szNoOfItems >= MAX_PBAP_OBJECTS )
          {
            break;
          }
        }
        pszTagPosition = pszEndPos + 1;
      }// extract folders
        
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif //FEATURE_BT_TEST_API
        
      /* extract files */
      pszStartPos = pszEndPos = NULL;
      pszTagPosition = pszFolderContents;
      while ( pszTagPosition != NULL )
      {
        /* Look for the tag "file name" */
        pszTagPosition = STRSTR( pszTagPosition, FILE_NAME_STR );
        if ( pszTagPosition == NULL )
        { 
          break;
        }
        /* Look for the start of the double quote */
        pszStartPos = STRCHR( pszTagPosition, SEARCH_CHR );
        if ( pszStartPos == NULL )
        {
          break;
        } 
        /* Look for the end of the double quote */
        pszEndPos = STRCHR( pszStartPos+1, SEARCH_CHR );
        if ( pszEndPos == NULL )
        {
          break;
        }
        if ( pszEndPos - ( pszStartPos+1 ) <= MAX_HANDLE_NAME_LEN )
        {  
          /* Copy the name of the folder in an array */
          MEMCPY( pMe->mPBAP.pbObjects[ szNoOfItems ].szObjHdleName, 
                  pszStartPos+1, pszEndPos - ( pszStartPos+1 ) );
          pMe->mPBAP.pbObjects[ szNoOfItems ].bFolder = FALSE;
          szNoOfItems++;
          if ( szNoOfItems >= MAX_PBAP_OBJECTS )
          {
            break;
          }
        }
        pszTagPosition = pszEndPos + 1;
      }// extract files

      /* extract card objects */
      pszStartPos = pszEndPos = NULL;
      pszTagPosition = pszFolderContents;
      while ( pszTagPosition != NULL )
      {
        /* Look for the tag "card handle" */
        pszTagPosition = STRSTR( pszTagPosition, CARD_HANDLE_STR );
        if ( pszTagPosition == NULL )
        { 
          break;
        }
        /* Look for the start of the double quote */
        pszStartPos = STRCHR( pszTagPosition, SEARCH_CHR );
        if ( pszStartPos == NULL )
        {
          break;
        } 
        /* Look for the end of the double quote */
        pszEndPos = STRCHR( pszStartPos+1, SEARCH_CHR );
        if ( pszEndPos == NULL )
        {
          break;
        }
        if ( pszEndPos - ( pszStartPos+1 ) <= MAX_HANDLE_NAME_LEN )
        {  
          /* Copy the name of the folder in an array */
          MEMCPY( pMe->mPBAP.pbObjects[ szNoOfItems ].szObjHdleName, 
                  pszStartPos+1, pszEndPos - ( pszStartPos+1 ) );
          pMe->mPBAP.pbObjects[ szNoOfItems ].bFolder = FALSE;
          /* Look for card name */
          pszTagPosition = pszEndPos + 1;
          pszTagPosition = STRSTR( pszTagPosition, CARD_NAME_STR );
          if ( pszTagPosition != NULL )
          { 
            /* Look for the start of the double quote */
            pszStartPos = STRCHR( pszTagPosition, SEARCH_CHR );
            if ( pszStartPos == NULL )
            {
              break;
            } 
            /* Look for the end of the double quote */
            pszEndPos = STRCHR( pszStartPos+1, SEARCH_CHR );
            if ( pszEndPos == NULL )
            {
              break;
            }
            if ( pszEndPos - ( pszStartPos+1 ) <= AEEBT_MAX_FILE_NAME )
            {
              /* Copy the name of the folder in an array */
              MEMCPY( pMe->mPBAP.pbObjects[ szNoOfItems ].szObjName, 
                      pszStartPos+1, pszEndPos - ( pszStartPos+1 ) );
            }
          }
          szNoOfItems++;
          if ( szNoOfItems >= MAX_PBAP_OBJECTS )
          {
            break;
          }
        }
        pszTagPosition = pszEndPos + 1;
      }// extract object details
      pMe->mPBAP.uNumOfObj = szNoOfItems;
      MSG_LOW( "PBAPBuildVCardListMenu - no of objs=%d", szNoOfItems, 0, 0 );
    }
    FREE( pszFolderContents );
    IFILE_Release( pIFile );
    return TRUE;
  }
  else
  {
    MSG_HIGH( "PBAPBuildVCardListMenu - no vcard list file", 0, 0, 0 );
    return FALSE;
  }
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildEmptyListingObject
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildEmptyListingObject( CBTApp* pMe )
{
  uint16      uNoOfRec = 0;
  AECHAR      wFileName[ AEE_MAX_FILE_NAME + 1 ];
  
  if ( pMe->mPBAP.PBAPObject.maxListCount == 0 )
  {
    /* no need to generate the xml object, just return the no of items */
    if ( IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, 
           &uNoOfRec, NULL, AEEBT_ERR_NONE ) != SUCCESS )
    {
      MSG_ERROR( "PBAPBuildEmptyLstObj - vcard listing resp failed", 0, 0, 0 );
    }
    return;
  }

  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, VLIST_XML ) != SUCCESS )
  {
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           VLIST_XML, _OFM_CREATE );
  }
  else
  {
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           VLIST_XML, _OFM_READWRITE );
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_ERROR( "PBAPBuildEmptyLstObj - file open failed", 0, 0, 0 );
    return;
  }

  BTApp_PBAPAddVCardListingHeader( pMe->mPBAP.pIFile );
  IFILE_Write( pMe->mPBAP.pIFile, "</vCard-listing>\n", 
                STRLEN("</vCard-listing>\n") );
  IFILE_Release( pMe->mPBAP.pIFile );
  /* transmit vcard */
  STRTOWSTR( VLIST_XML, wFileName, sizeof(wFileName) );
  if ( IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, wFileName, 
                                NULL, NULL, AEEBT_ERR_NONE ) != SUCCESS )
  {
    MSG_ERROR( "PBAPBuildEmptyLstObj - vcard listing resp failed", 0, 0, 0 );
  }
  return;
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildCHListingObject
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildCCHListingObject( CBTApp* pMe )
{
  uint16      uNoOfRec = 0;
  int         Status;
  uint16      uNoOfRecRead = 0;
  uint16      uOffset = 0;
  boolean     bReadAll = FALSE;
  char        sBuf[50];
  AECHAR      wFileName[ AEE_MAX_FILE_NAME + 1 ];
  AECHAR      wNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS + 1];
  char        sNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS + 1];
  boolean     bNameAdded = FALSE;
  IAddrRec*   addrRec=NULL;
  uint8       uFieldCount = 0;
  uint8       i = 0;
  char*       pName = NULL;
  AEEAddrField*               addrField;
  const AEECallHistoryEntry*  pCallHistoryEntry = NULL;
  AEECallHistoryField*        pItems = NULL;  

  if ( pMe->mPBAP.PBAPObject.maxListCount == 0 )
  {
    /* no need to generate the xml object, just return the no of items */
    ICALLHISTORY_EnumInit( pMe->mPBAP.pCallHist );
    while ( (pCallHistoryEntry = ICALLHISTORY_EnumNext(pMe->mPBAP.pCallHist, 
                                   &Status)) != NULL )
    {
      uNoOfRec++;
    }
    MSG_LOW( "PBAPBuildCHLstObj - No Of Obj=%d", uNoOfRec, 0, 0 );
    if ( IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, 
           &uNoOfRec, NULL, AEEBT_ERR_NONE ) != SUCCESS )
    {
      MSG_ERROR( "PBAPBuildCCHLstObj - vcard listing resp failed", 0, 0, 0 );
    }
    return;
  }
  /* build the vcard listing object */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, VLIST_XML ) != SUCCESS )
  {
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           VLIST_XML, _OFM_CREATE );
  }
  else
  {
    if ( (pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                VLIST_XML, _OFM_READWRITE )) != NULL )
    {
      if ( IFILE_Truncate( pMe->mPBAP.pIFile, 0 ) != SUCCESS )
      {
        MSG_HIGH( "PBAPBuildCCHLstObj - File truncate failed", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );
        IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, NULL, 
                                             NULL, AEEBT_ERR_RESOURCE );
        return;
      }    
    }
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_ERROR( "PBAPBuildCCHLstObj - file open failed", 0, 0, 0 );
    return;
  }

  BTApp_PBAPAddVCardListingHeader( pMe->mPBAP.pIFile );

    /* if maxlist count is zero it would have been handled before */
  if ( pMe->mPBAP.PBAPObject.maxListCount == 65535 )
  {
    bReadAll = TRUE;
  }
  if ( pMe->mPBAP.PBAPObject.listStartOffset != 0 )
  {
    uOffset = pMe->mPBAP.PBAPObject.listStartOffset;
  }

  ICALLHISTORY_EnumInit( pMe->mPBAP.pCallHist );
  uNoOfRecRead = 1;
  while ( ((pCallHistoryEntry = ICALLHISTORY_EnumNext( 
              pMe->mPBAP.pCallHist, &Status)) != NULL ) && 
            (Status == SUCCESS) && ((bReadAll == TRUE) || 
            ((uNoOfRecRead - uOffset) <= pMe->mPBAP.PBAPObject.maxListCount)) )
  {
    if ( uNoOfRecRead > uOffset )
    {
      SNPRINTF( sBuf, sizeof(sBuf), "<%s = \"%X.vcf\" " , 
                CARD_HANDLE_STR, uNoOfRecRead );
      IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
      /* adding name property */
      /* extract the number and check if there is a record in addressbook */
      pItems = pCallHistoryEntry->pFields;
      if ( (pCallHistoryEntry->wNumFields != 0) && 
           (pItems != NULL) )
      {
        MEMSET( wNumber, 0, sizeof(wNumber) );
        for ( i = 0; i < pCallHistoryEntry->wNumFields; i++ )
        {
          if ( pItems[i].wID == AEECALLHISTORY_FIELD_NUMBER_EX )
          {
            MEMCPY( wNumber, pItems[i].pData, 
                    MIN(ARR_SIZE(wNumber)*sizeof(AECHAR), 
                        pItems[i].wDataLen) );
            break;
          }
        }
        bNameAdded = FALSE;
        if ( SUCCESS == IADDRBOOKEXT_EnumRecInitEx( pMe->mPBAP.pAddrbook,
                                                    AEE_ADDR_CAT_NONE,
                                                    AEE_ADDRFIELD_NONE,
                                                    wNumber,
                                                    (uint16) WSTRSIZE(wNumber),
                                                    ADDRREC_SEARCHTYPE_EXACT |
                                                    ADDRREC_FIELDID_PHONE ) ) 
        {
          addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook );
          /* there might be multiple records, add only the first record */
          if ( NULL != addrRec ) 
          {
            /* add this record's name */
            uFieldCount = IADDRREC_GetFieldCount( addrRec );
            for ( i = 0; i < uFieldCount; i++ )
            {
              addrField = IADDRREC_GetField( addrRec, i );
              if ( (addrField->fID == AEE_ADDRFIELD_NAME) && 
                   (addrField->wDataLen != 0) )
              {
                pName = (char*)MALLOC( addrField->wDataLen + 1 );
                if ( pName == NULL )
                {
                  MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
                  IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, NULL, 
                                                       NULL, AEEBT_ERR_RESOURCE );
                  return;
                }
                WSTRTOSTR( addrField->pBuffer, pName, addrField->wDataLen + 1 );
                SNPRINTF( sBuf, sizeof(sBuf), "%s = \"%s\"/>\n", 
                          CARD_NAME_STR, pName );
                IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
                bNameAdded = TRUE;
              }
            }
          }
        }
        if ( bNameAdded == FALSE )
        {
          /* no record found, add telephone number in name property */
          WSTRTOSTR( wNumber, sNumber, sizeof(sNumber) );
          SNPRINTF( sBuf, sizeof(sBuf), "%s = \"%s\"/>\n", 
                    CARD_NAME_STR, sNumber );
          IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
        }
      }        
    }
    uNoOfRecRead++;
  }
  /* add footer to the xml file */
  /* add vcard listing footer*/
  IFILE_Write( pMe->mPBAP.pIFile, "</vCard-listing>\n", 
                STRLEN("</vCard-listing>\n") );
  IFILE_Release( pMe->mPBAP.pIFile );
  /* transmit vcard */
  STRTOWSTR( VLIST_XML, wFileName, sizeof(wFileName) );
  if ( IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, wFileName, 
                                NULL, NULL, AEEBT_ERR_NONE ) != SUCCESS )
  {
    MSG_ERROR( "PBAPBuildCCHLstObj - vcard listing resp failed", 0, 0, 0 );
  }
  return;
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildCHListingObject
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildCHListingObject( CBTApp* pMe, uint8 uCallType )
{
  uint16      uNoOfRec = 0;
  int         Status;
  uint16      uNoOfRecRead = 0;
  uint16      uOffset = 0;
  boolean     bReadAll = FALSE;
  char        sBuf[50];
  AECHAR      wFileName[ AEE_MAX_FILE_NAME + 1 ];
  AECHAR      wNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS + 1];
  char        sNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS + 1];
  boolean     bNameAdded = FALSE;
  IAddrRec*   addrRec=NULL;
  uint8       uFieldCount = 0;
  uint8       i = 0;
  char*       pName = NULL;
  AEEAddrField*               addrField;
  const AEECallHistoryEntry*  pCallHistoryEntry = NULL;
  AEECallHistoryField*        pItems = NULL;  

  if ( pMe->mPBAP.PBAPObject.maxListCount == 0 )
  {
    /* no need to generate the xml object, just return the no of items */
    ICALLHISTORY_EnumInitByCallType( pMe->mPBAP.pCallHist, uCallType );
    while ( (pCallHistoryEntry = ICALLHISTORY_EnumNext(pMe->mPBAP.pCallHist, 
                                   &Status)) != NULL )
    {
      uNoOfRec++;
    }
    MSG_LOW( "PBAPBuildCHLstObj - No Of Obj=%d", uNoOfRec, 0, 0 );
    if ( IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, 
           &uNoOfRec, NULL, AEEBT_ERR_NONE ) != SUCCESS )
    {
      MSG_ERROR( "PBAPBuildCHLstObj - vcard listing resp failed", 0, 0, 0 );
    }
    return;
  }
  /* build the vcard listing object */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, VLIST_XML ) != SUCCESS )
  {
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           VLIST_XML, _OFM_CREATE );
  }
  else
  {
    if ( (pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                VLIST_XML, _OFM_READWRITE )) != NULL )
    {
      if ( IFILE_Truncate( pMe->mPBAP.pIFile, 0 ) != SUCCESS )
      {
        MSG_HIGH( "PBAPBuildCHLstObj - File truncate failed", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );
        IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, NULL, 
                                             NULL, AEEBT_ERR_RESOURCE );
        return;
      }    
    }
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_ERROR( "PBAPBuildCHLstObj - file open failed", 0, 0, 0 );
    return;
  }

  BTApp_PBAPAddVCardListingHeader( pMe->mPBAP.pIFile );

    /* if maxlist count is zero it would have been handled before */
  if ( pMe->mPBAP.PBAPObject.maxListCount == 65535 )
  {
    bReadAll = TRUE;
  }
  if ( pMe->mPBAP.PBAPObject.listStartOffset != 0 )
  {
    uOffset = pMe->mPBAP.PBAPObject.listStartOffset;
  }

  ICALLHISTORY_EnumInitByCallType( pMe->mPBAP.pCallHist, uCallType );
  uNoOfRecRead = 1;
  while ( ((pCallHistoryEntry = ICALLHISTORY_EnumNext( 
              pMe->mPBAP.pCallHist, &Status)) != NULL ) && 
            (Status == SUCCESS) && ((bReadAll == TRUE) || 
            ((uNoOfRecRead - uOffset) <= pMe->mPBAP.PBAPObject.maxListCount)) )
  {
    if ( uNoOfRecRead > uOffset )
    {
      SNPRINTF( sBuf, sizeof(sBuf), "<%s = \"%X.vcf\" " , CARD_HANDLE_STR, 
                uNoOfRecRead );
      IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
      /* adding name property */
      /* extract the number and check if there is a record in addressbook */
      pItems = pCallHistoryEntry->pFields;
      if ( (pCallHistoryEntry->wNumFields != 0) && 
           (pItems != NULL) )
      {
        MEMSET( wNumber, 0, sizeof(wNumber) );
        for ( i = 0; i < pCallHistoryEntry->wNumFields; i++ )
        {
          if ( pItems[i].wID == AEECALLHISTORY_FIELD_NUMBER_EX )
          {
            MEMCPY( wNumber, pItems[i].pData, 
                    MIN(ARR_SIZE(wNumber)*sizeof(AECHAR), 
                        pItems[i].wDataLen) );
            break;
          }
        }
        bNameAdded = FALSE;
        if ( SUCCESS == IADDRBOOKEXT_EnumRecInitEx( pMe->mPBAP.pAddrbook,
                                                    AEE_ADDR_CAT_NONE,
                                                    AEE_ADDRFIELD_NONE,
                                                    wNumber,
                                                    (uint16) WSTRSIZE(wNumber),
                                                    ADDRREC_SEARCHTYPE_EXACT |
                                                    ADDRREC_FIELDID_PHONE ) ) 
        {
          addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook );
          /* there might be multiple records, add only the first record */
          if ( NULL != addrRec ) 
          {
            /* add this record's name */
            uFieldCount = IADDRREC_GetFieldCount( addrRec );
            for ( i = 0; i < uFieldCount; i++ )
            {
              addrField = IADDRREC_GetField( addrRec, i );
              if ( (addrField->fID == AEE_ADDRFIELD_NAME) && 
                   (addrField->wDataLen != 0) )
              {
                pName = (char*)MALLOC( addrField->wDataLen + 1 );
                if ( pName == NULL )
                {
                  MSG_ERROR( "PBAPBuildLocalAddBook - Mem alloc failed", 0, 0, 0 );
                  IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, NULL, 
                                                       NULL, AEEBT_ERR_RESOURCE );
                  return;
                }
                WSTRTOSTR( addrField->pBuffer, pName, addrField->wDataLen + 1 );
                SNPRINTF( sBuf, sizeof(sBuf), "%s = \"%s\"/>\n", 
                          CARD_NAME_STR, pName );
                IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
                bNameAdded = TRUE;
              }
            }
          }
        }
        if ( bNameAdded == FALSE )
        {
          /* no record found, add telephone number in name property */
          WSTRTOSTR( wNumber, sNumber, sizeof(sNumber) );
          SNPRINTF( sBuf, sizeof( sBuf ), "%s = \"%s\"/>\n", 
                    CARD_NAME_STR, sNumber );
          IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
        }
      }        
    }
    uNoOfRecRead++;
  }
  /* add footer to the xml file */
  /* add vcard listing footer*/
  IFILE_Write( pMe->mPBAP.pIFile, "</vCard-listing>\n", 
                STRLEN("</vCard-listing>\n") );
  IFILE_Release( pMe->mPBAP.pIFile );
  /* transmit vcard */
  STRTOWSTR( VLIST_XML, wFileName, sizeof(wFileName) );
  if ( IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, wFileName, 
                                NULL, NULL, AEEBT_ERR_NONE ) != SUCCESS )
  {
    MSG_ERROR( "PBAPBuildCHLstObj - vcard listing resp failed", 0, 0, 0 );
  }
  return;
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildABListingObject
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildABListingObject( CBTApp* pMe, boolean bFromSIM )
{
  uint16      uNoOfRec = 0;
  int         Status = EFAILED;
  uint16      uNoOfRecRead = 0;
  uint16      uOffset = 0;
  IAddrRec*   addrRec=NULL;
  boolean     bReadAll = FALSE;
  AEEAddrCat  category = AEE_ADDR_CAT_NONE;
  boolean     bRecFromSIM = FALSE;
  char        sBuf[100];
  uint8       count = 0;
  uint8       uFieldCount = 0;
  uint8       i = 0;
  char*       pName = NULL;
  char*       pFirstName = NULL;
  char*       pLastName = NULL;
  AECHAR      wFileName[ AEE_MAX_FILE_NAME + 1 ];
  AEEAddrField*          addrField;
  uint16      uRecID[25];
  uint8       uIndex, j, k;
  uint8       uFirstRec = 0;
    
  if ( pMe->mPBAP.PBAPObject.maxListCount == 0 )
  {
    uNoOfRec = BTApp_PBAPFindNumberOfABItems( pMe, bFromSIM );
    uNoOfRec++; // to account for the 0.vcf record
    MSG_HIGH("PBAPBuildABListingObject - num records = %d", uNoOfRec, 0, 0);
    /* no need to generate the xml object, just return the no of items */
    if ( IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, 
                                              &uNoOfRec, NULL, 
                                              AEEBT_ERR_NONE ) != SUCCESS )
    {
      MSG_ERROR( "PBAPBuildABLstObj - vcard listing resp failed", 0, 0, 0 );
    }
    return;
  }
  /* build the vcard listing object */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, VLIST_XML ) != SUCCESS )
  {
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           VLIST_XML, _OFM_CREATE );
  }
  else
  {
    if ( (pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                VLIST_XML, _OFM_READWRITE )) != NULL )
    {
      if ( IFILE_Truncate( pMe->mPBAP.pIFile, 0 ) != SUCCESS )
      {
        MSG_HIGH( "PBAPBuildABLstObj - File truncate failed", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );
        IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, NULL, 
                                             NULL, AEEBT_ERR_RESOURCE );
        return;
      }    
    }
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_ERROR( "PBAPBuildABLstObj - file open failed", 0, 0, 0 );
    IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, NULL, 
                                         NULL, AEEBT_ERR_RESOURCE );
    return;
  }

  /* if maxlist count is zero it would have been handled before */
  if ( pMe->mPBAP.PBAPObject.maxListCount == 65535 )
  {
    bReadAll = TRUE;
  }
  if ( pMe->mPBAP.PBAPObject.listStartOffset != 0 )
  {
    uOffset = pMe->mPBAP.PBAPObject.listStartOffset;
  }
  /* check if search is requested */
  if ( pMe->mPBAP.PBAPObject.searchValue[0] != NULL )
  {
    MSG_LOW( "PBAPBuildABLstObj - searching AB...", 0, 0, 0 );
    if ( pMe->mPBAP.PBAPObject.searchAttrib == 
           AEEBT_PBAP_SEARCH_ATTRIBUTE_NAME )
    {
      /* initialize the address book */  
      Status = IADDRBOOKEXT_EnumRecInitEx( 
                 pMe->mPBAP.pAddrbook, 
                 AEE_ADDR_CAT_NONE,
#ifdef FEATURE_UIONE_HDK
                 /* On UIOne targets, contract app creates addr book entries 
                    with FN and LN fields whereas, on 3G-UI, "NAME" field is used.
                    Here we do search by lastname only.
                  */
                 AEE_ADDRFIELD_LASTNAME, 
#else
                 AEE_ADDRFIELD_NAME,
#endif
                 pMe->mPBAP.PBAPObject.searchValue,
                 WSTRSIZE(pMe->mPBAP.PBAPObject.searchValue), 
                 ADDRREC_SEARCHTYPE_CONTAINS );
    }
    else if ( pMe->mPBAP.PBAPObject.searchAttrib == 
                AEEBT_PBAP_SEARCH_ATTRIBUTE_NUMBER )
    {
      /* initialize the address book */  
      Status = IADDRBOOKEXT_EnumRecInitEx( pMe->mPBAP.pAddrbook, 
                 AEE_ADDR_CAT_NONE,
                 AEE_ADDRFIELD_NONE, 
                 pMe->mPBAP.PBAPObject.searchValue,
                 WSTRSIZE(pMe->mPBAP.PBAPObject.searchValue), 
                 ADDRREC_SEARCHTYPE_CONTAINS | ADDRREC_FIELDID_PHONE );
    }
    else
    {
      /* initialize the address book */  
      Status = IADDRBOOKEXT_EnumRecInitEx( pMe->mPBAP.pAddrbook, 
                                           AEE_ADDR_CAT_NONE,
                                           AEE_ADDRFIELD_NONE, NULL, 0, 0 );
    }
  }
  else
  {
    /* initialize the address book */  
    Status = IADDRBOOKEXT_EnumRecInitEx( pMe->mPBAP.pAddrbook, 
                                         AEE_ADDR_CAT_NONE,
                                         AEE_ADDRFIELD_NONE, NULL, 0, 0 );
  }

  if ( Status != SUCCESS )
  {
    MSG_ERROR( "PBAPBuildABLstObj - AddBk init failed", 0, 0, 0 );
    IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, NULL, 
                                         NULL, AEEBT_ERR_RESOURCE );
    IFILE_Release( pMe->mPBAP.pIFile );                                         
    return;  
  }
  else
  {
    /* start reading the records */
    addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook );
  }

  BTApp_PBAPAddVCardListingHeader( pMe->mPBAP.pIFile );
  /* add first rec 0.vcf which corresponding to that of phone if no 
     search criteria specified and if offset is equal to 0 */
  if ( pMe->mPBAP.PBAPObject.searchValue[0] == NULL )
  {
    if ( uOffset == 0 )
    {
      SNPRINTF( sBuf, sizeof(sBuf), "<%s = \"0.vcf\" ", CARD_HANDLE_STR );
      IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
      SNPRINTF( sBuf, sizeof(sBuf), "%s = \"\"/>\n", CARD_NAME_STR );
      IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );        

      uRecID[0] = 0;
      uFirstRec = 1;
    }
    /* increment uNoOfRecRead to consider the 0th rec. This should be 
    incremented even when uOffset > 0 as 0th rec is added and removed */
    uNoOfRecRead++;
  }
  uIndex = uFirstRec;
  while ( (addrRec != NULL) && ( (bReadAll == TRUE) || 
          (uIndex < pMe->mPBAP.PBAPObject.maxListCount) ) )
  {
    /* check if the address category is sim or not */
    category = IADDRREC_GetCategory(addrRec);
    bRecFromSIM = BTApp_PBAPIsSIMRec(category);
    if ( bRecFromSIM == bFromSIM )
    {
      uNoOfRecRead++;
      if ( uNoOfRecRead > uOffset )
      {
        uRecID[uIndex] = IADDRREC_GetRecID( addrRec );
        MSG_LOW( "PBAPBuildABLstObj - RecID=%d,i=%d", uRecID[uIndex], 
                  uIndex, 0 );
        uIndex++;
        if ( uIndex >= 25 )
        {
          MSG_HIGH( "PBAPBuildABLstObj - Support only 25 recs", 0, 0, 0 );
          break;
        }
      }
    }
    IADDRREC_Release( addrRec );
    addrRec = NULL;
    addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook );
  }
  /* sort records in the ascending order of handle */
  for ( j=uFirstRec; j<uIndex; j++ )
  {
    for ( k=j+1; k<uIndex; k++ )
    {
      if ( uRecID[j] > uRecID[k] )
      { /*swap numbers*/
        uRecID[j] = uRecID[j]+uRecID[k];
        uRecID[k] = uRecID[j]-uRecID[k];
        uRecID[j] = uRecID[j]-uRecID[k];
      }
    }
  }
  /* add records to file */
  for ( j=uFirstRec; j<uIndex; j++ )
  {
    /* get the record from RecID */
    addrRec = IADDRBOOK_GetRecByID( pMe->mPBAP.pAddrbook, uRecID[j] );
    if ( addrRec == NULL )
    {
      MSG_ERROR( "PBAPBuildABLstObj - record id without record", 0, 0, 0 );
      break;
    }
    /* add this record's handle */
    SNPRINTF( sBuf, sizeof(sBuf), "<%s = \"%X.vcf\" ", 
              CARD_HANDLE_STR, uRecID[j] );
    IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
    /* add this record's name */
    uFieldCount = IADDRREC_GetFieldCount( addrRec );
    count = 0;
    for ( i = 0; i < uFieldCount; i++ )
    {
      addrField = IADDRREC_GetField( addrRec, i );
      /* 
       * On 3G-UI target, contact app stores name in ADDRFIELD_NAME whereas
       * Contact app in UIOne stores name in FIRSTNAME and LASTNAME fields 
       */
      if ( addrField->fID == AEE_ADDRFIELD_NAME )
      {
        if ( addrField->wDataLen != 0 )
        {
          pName = (char*)MALLOC( addrField->wDataLen + 1 );
          WSTRTOSTR( addrField->pBuffer, pName, addrField->wDataLen + 1 );
          /*
           * expecting no other addressfield will be available if ADDRFIELD_NAME
           * is existing. Terminator is appended at a common place outside the
           * "for" loop.
           */
          SNPRINTF( sBuf, sizeof(sBuf), "%s = \"%s", CARD_NAME_STR, pName );
          count += STRLEN( sBuf );
        }
      }
      else if ( addrField->fID == AEE_ADDRFIELD_LASTNAME )
      {
        if ( addrField->wDataLen != 0 )
        {
          pLastName = (char*)MALLOC( addrField->wDataLen + 1 );
          WSTRTOSTR( addrField->pBuffer, pLastName, addrField->wDataLen + 1 );
          SNPRINTF( sBuf, sizeof(sBuf), "%s = \"%s", CARD_NAME_STR, pLastName );
          count += STRLEN( sBuf );
        }
      }
      else if ( addrField->fID == AEE_ADDRFIELD_FIRSTNAME ) 
      {
        if ( addrField->wDataLen != 0 )
        {
          pFirstName = (char*)MALLOC( addrField->wDataLen + 1 );
          WSTRTOSTR( addrField->pBuffer, pFirstName, addrField->wDataLen + 1 );
          if ( pLastName ) 
          {
            SNPRINTF( sBuf + count, sizeof( sBuf ) - count , ";%s", pFirstName );
          }
          else
          { 
            /* Last name not available for this record */
            SNPRINTF( sBuf, sizeof(sBuf), "%s = \"%s", CARD_NAME_STR, 
                      pFirstName );
          }
        }
      } 
    }
    

    if ( !pFirstName && !pLastName && !pName ) 
    {
      /* this record has no name*/
      SNPRINTF( sBuf, sizeof(sBuf), "%s = \"\"/>\n", CARD_NAME_STR );
    }
    else if(0 != uFieldCount) 
    {
      /* It appends the terminator for 3G-UI as well as UIOne targets */
      SNPRINTF( (sBuf + STRLEN( sBuf )), 
                sizeof ( sBuf ) - STRLEN( sBuf ), "\"/>\n");
    }

    IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) ); 
    FREEIF(pName);
    FREEIF(pLastName);
    FREEIF(pFirstName);
  }

  /* add vcard listing footer*/
  IFILE_Write( pMe->mPBAP.pIFile, "</vCard-listing>\n", 
                STRLEN("/vCard-listing>\n") );
  IFILE_Release( pMe->mPBAP.pIFile );
  /* transmit vcard */
  STRTOWSTR( VLIST_XML, wFileName, sizeof(wFileName) );
  if ( IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, wFileName, 
         NULL, NULL, AEEBT_ERR_NONE ) != SUCCESS )
  {
    MSG_ERROR( "PBAPBuildABLstObj - vcard listing resp failed", 0, 0, 0 );
  }
  return;  
}

/* ==========================================================================
FUNCTION BTApp_PBAPHandlePBObjectListing
DESCRIPTION
============================================================================= */
static void BTApp_PBAPHandlePBObjectListing( CBTApp* pMe, char* pObjectName )
{
  boolean     bFromSIM = FALSE;
  
  /* is the object requested from phone or SIM1 */
  if ( STRSTR( sServerPath, AEEBT_PBAP_FOLDER_NAME_SIM1 ) != NULL )
  {
    bFromSIM = TRUE;
  }
  
  if ( STRICMP( pObjectName, AEEBT_PBAP_FOLDER_NAME_PB ) == 0 )
  {
    BTApp_PBAPBuildABListingObject( pMe, bFromSIM );
  }
  else if ( STRICMP( pObjectName, AEEBT_PBAP_FOLDER_NAME_ICH ) == 0 )
  {
    if ( bFromSIM )
    {
      MSG_HIGH( "PBAPHdlePBObjList - no support of ICH in SIM", 0, 0, 0 );
      BTApp_PBAPBuildEmptyListingObject( pMe );
    }
    else
    {
      BTApp_PBAPBuildCHListingObject( pMe, AEECALLHISTORY_CALL_TYPE_FROM );
    }
  }
  else if ( STRICMP( pObjectName, AEEBT_PBAP_FOLDER_NAME_OCH ) == 0 )
  {
    if ( bFromSIM )
    {
      MSG_HIGH( "PBAPHdlePBObjList - no support of OCH in SIM", 0, 0, 0 );
      BTApp_PBAPBuildEmptyListingObject( pMe );
    }
    else
    {
      BTApp_PBAPBuildCHListingObject( pMe, AEECALLHISTORY_CALL_TYPE_TO );
    }  
  }
  else if ( STRICMP( pObjectName, AEEBT_PBAP_FOLDER_NAME_MCH ) == 0 )
  {
    if ( bFromSIM )
    {
      MSG_HIGH( "PBAPHdlePBObjList - no support of MCH in SIM", 0, 0, 0 );
      BTApp_PBAPBuildEmptyListingObject( pMe );
    }
    else
    {
      BTApp_PBAPBuildCHListingObject( pMe, AEECALLHISTORY_CALL_TYPE_MISSED );
    }
  }
  else if ( STRICMP( pObjectName, AEEBT_PBAP_FOLDER_NAME_CCH ) == 0 )
  {
    if ( bFromSIM )
    {
      MSG_HIGH( "PBAPHdlePBObjList - no support of CCH in SIM", 0, 0, 0 );
      BTApp_PBAPBuildEmptyListingObject( pMe );
    }
    else
    {
      BTApp_PBAPBuildCCHListingObject( pMe );
    }
  }
  else
  {
    // Not a valid request
    MSG_HIGH( "PBAPHdlePBObjList - Bad PullvCardListing request - %s", 
              pObjectName, 0, 0 );
    if ( IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, 
           NULL, NULL, AEEBT_ERR_OBEX_BAD_REQUEST ) != SUCCESS )
    {
      MSG_ERROR( "PBAPHdlePBObjList - vcard listing resp failed", 0, 0, 0 );
    }
  }
  return;
}

/* ==========================================================================
FUNCTION BTApp_PBAPServerPullVCardListingRsp
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPServerPullVCardListingRsp( CBTApp* pMe )
{
  char        sDirString[ AEEBT_MAX_FILE_NAME + 1 ];
  AECHAR      wFileName[ AEEBT_MAX_FILE_NAME + 1 ];
  char*       pDir;
  int         uResult = SUCCESS;
  uint16      uNoOfObjects;
  boolean     bFromSIM = FALSE;

  MSG_LOW( "PBAPServerPullVCardListingRsp - Srv vCard Listing", 0, 0, 0 );

  STRTOWSTR( VLIST_XML, wFileName, sizeof(wFileName) );
  /* truncate the file if it already exists */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, VLIST_XML ) == SUCCESS )
  {
    if ( (pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                      VLIST_XML,  _OFM_READWRITE)) != NULL )
    {
      if ( IFILE_Truncate( pMe->mPBAP.pIFile, 0 ) != SUCCESS )
      {
        MSG_ERROR( "PBAP - can't truncate dirList file", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );
        IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, NULL, 
                                             NULL, AEEBT_ERR_RESOURCE );      
        return FALSE;
      }
    }
  }
  else
  {
    /* create file for vcard listing */
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           VLIST_XML, _OFM_CREATE );
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_HIGH( "PBAPServerPullVCardListingRsp - File open failed", 0, 0, 0 );
    IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, NULL, 
                                         NULL, AEEBT_ERR_RESOURCE );    
    return FALSE;
  }

  WSTRTOSTR( pMe->mPBAP.PBAPObject.pwObjectName, sDirString, 
             sizeof(sDirString) );
  if ( sServerPath[0] == NULL )
  {
    MSG_ERROR( "PBAPVCardListingRsp - invalid dir name", 0, 0, 0 );
    IFILE_Release( pMe->mPBAP.pIFile );    
    uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL, NULL, 
                NULL, AEEBT_ERR_OBEX_BAD_REQUEST );
  }
  else if ( STRCMP( sServerPath, AEEBT_PBAP_FOLDER_NAME_ROOT ) == 0 )
  {
    if ( sDirString[0] == NULL )
    {
      /* send the current directory information */
      if ( pMe->mPBAP.PBAPObject.maxListCount != 0 )
      {
        BTApp_PBAPAddVCardListingHeader( pMe->mPBAP.pIFile );
        if ( pMe->mPBAP.PBAPObject.maxListCount == 1 )
        {
          SNPRINTF(szBuf, sizeof( szBuf ), "<folder name=\"telecom\"/>\n");
        }
        else
        {
        SNPRINTF(szBuf, sizeof( szBuf ), 
                 "<folder name=\"telecom\"/>\n<folder name=\"SIM1\"/>\n");
        }
        IFILE_Write( pMe->mPBAP.pIFile, szBuf, STRLEN( szBuf ) );
        SNPRINTF(szBuf, sizeof( szBuf ), "</vCard-listing>\n" );
        IFILE_Write( pMe->mPBAP.pIFile, szBuf, STRLEN( szBuf ) );
        IFILE_Release( pMe->mPBAP.pIFile );
        uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                    wFileName, NULL, NULL, AEEBT_ERR_NONE );        
      }
      else
      {
        uNoOfObjects = 2;
        IFILE_Release( pMe->mPBAP.pIFile );
        uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                    NULL, &uNoOfObjects, NULL, AEEBT_ERR_NONE );      
      }
    }
    else
    {
      /* folder has to be either telecom or SIM1 */
      if ( STRICMP( sDirString, AEEBT_PBAP_FOLDER_NAME_TELECOM ) == 0 )
      {
        if ( pMe->mPBAP.PBAPObject.maxListCount != 0 )
        {
          BTApp_PBAPAddVCardListingHeader( pMe->mPBAP.pIFile );
          BTApp_PBAPAddTelecomListing( pMe->mPBAP.pIFile, FALSE, 
                                       pMe->mPBAP.PBAPObject.maxListCount );
          SNPRINTF( szBuf, sizeof( szBuf ), "</vCard-listing>\n" );
          IFILE_Write( pMe->mPBAP.pIFile, szBuf, STRLEN( szBuf ) );
          IFILE_Release( pMe->mPBAP.pIFile );
          uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                      wFileName, NULL, NULL, AEEBT_ERR_NONE );        
        }
        else
        {
          uNoOfObjects = 10;
          IFILE_Release( pMe->mPBAP.pIFile );          
          uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                      NULL, &uNoOfObjects, NULL, AEEBT_ERR_NONE );      
        }
      }
      else if ( STRICMP( sDirString, AEEBT_PBAP_FOLDER_NAME_SIM1 ) == 0 )
      {
        if ( pMe->mPBAP.PBAPObject.maxListCount != 0 )
        {
          BTApp_PBAPAddVCardListingHeader( pMe->mPBAP.pIFile );
          SNPRINTF( szBuf, sizeof( szBuf ), "<folder name=\"telecom\"/>\n" );
          IFILE_Write( pMe->mPBAP.pIFile, szBuf, STRLEN( szBuf ) );
          SNPRINTF( szBuf, sizeof( szBuf ), "</vCard-listing>\n" );
          IFILE_Write( pMe->mPBAP.pIFile, szBuf, STRLEN( szBuf ) );
          IFILE_Release( pMe->mPBAP.pIFile );
          uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                      wFileName, NULL, NULL, AEEBT_ERR_NONE );        
        }
        else
        {
          uNoOfObjects = 1;
          IFILE_Release( pMe->mPBAP.pIFile );
          uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                      NULL, &uNoOfObjects, NULL, AEEBT_ERR_NONE );      
        }
      }
      else
      {
        MSG_ERROR( "PBAPVCardListingRsp - invalid dir name", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );
        uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL,
                    NULL, NULL, AEEBT_ERR_OBEX_BAD_REQUEST );
      }
    }
  }
  else
  {
    pDir = STRRCHR( sServerPath, '/' );
    if ( pDir != NULL )
    {
      pDir++; // point to the start of the directory
      if ( STRCMP( pDir, AEEBT_PBAP_FOLDER_NAME_TELECOM ) == 0 )
      {
        /* check if this dir is from SIM */
        if ( STRSTR( sServerPath, AEEBT_PBAP_FOLDER_NAME_SIM1 ) != NULL )
        {
          bFromSIM = TRUE;
        }
        if ( sDirString[0] == NULL )
        {
          if ( pMe->mPBAP.PBAPObject.maxListCount != 0 )
          {
            BTApp_PBAPAddVCardListingHeader( pMe->mPBAP.pIFile );
            BTApp_PBAPAddTelecomListing( pMe->mPBAP.pIFile, bFromSIM,
                                         pMe->mPBAP.PBAPObject.maxListCount );
            SNPRINTF( szBuf, sizeof( szBuf ), "</vCard-listing>\n" );
            IFILE_Write( pMe->mPBAP.pIFile, szBuf, STRLEN( szBuf ) );
            IFILE_Release( pMe->mPBAP.pIFile );
            uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                        wFileName, NULL, NULL, AEEBT_ERR_NONE );        
          }
          else
          {
            uNoOfObjects = 10;
            IFILE_Release( pMe->mPBAP.pIFile );            
            uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                        NULL, &uNoOfObjects, NULL, AEEBT_ERR_NONE );      
          }
        }
        else
        {
          IFILE_Release( pMe->mPBAP.pIFile );
          BTApp_PBAPHandlePBObjectListing( pMe, sDirString );
        }
      }
      else if ( STRCMP( pDir, AEEBT_PBAP_FOLDER_NAME_SIM1 ) == 0 )
      {
        if ( sDirString[0] == NULL )
        {
          if ( pMe->mPBAP.PBAPObject.maxListCount != 0 )
          {
            BTApp_PBAPAddVCardListingHeader( pMe->mPBAP.pIFile );
            SNPRINTF( szBuf, sizeof( szBuf ), "<folder name=\"telecom\"/>\n" );
            IFILE_Write( pMe->mPBAP.pIFile, szBuf, STRLEN( szBuf ) );
            SNPRINTF( szBuf, sizeof( szBuf ), "</vCard-listing>\n" );
            IFILE_Write( pMe->mPBAP.pIFile, szBuf, STRLEN( szBuf ) );
            IFILE_Release( pMe->mPBAP.pIFile );
            uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                        wFileName, NULL, NULL, AEEBT_ERR_NONE );
          }
          else
          {
            uNoOfObjects = 1;
            IFILE_Release( pMe->mPBAP.pIFile );
            uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                        NULL, &uNoOfObjects, NULL, AEEBT_ERR_NONE );      
          }          
        }
        else if ( STRICMP( sDirString, AEEBT_PBAP_FOLDER_NAME_TELECOM ) == 0 )
        {
          if ( pMe->mPBAP.PBAPObject.maxListCount != 0 )
          {
            BTApp_PBAPAddVCardListingHeader( pMe->mPBAP.pIFile );
            BTApp_PBAPAddTelecomListing( pMe->mPBAP.pIFile, TRUE,
                                         pMe->mPBAP.PBAPObject.maxListCount );
            SNPRINTF( szBuf, sizeof( szBuf ), "</vCard-listing>\n" );
            IFILE_Write( pMe->mPBAP.pIFile, szBuf, STRLEN( szBuf ) );
            IFILE_Release( pMe->mPBAP.pIFile );
            uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                        wFileName, NULL, NULL, AEEBT_ERR_NONE );
          }
          else
          {
            uNoOfObjects = 1;
            IFILE_Release( pMe->mPBAP.pIFile );
            uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, 
                        NULL, &uNoOfObjects, NULL, AEEBT_ERR_NONE );
          }
        }
        else
        {
          MSG_ERROR( "PBAPVCardListingRsp - invalid dir name", 0, 0, 0 );
          IFILE_Release( pMe->mPBAP.pIFile );
          uResult = IBTEXTPBAP_PullvCardListingResponse( pMe->mPBAP.po, NULL,  
                      NULL, NULL, AEEBT_ERR_OBEX_BAD_REQUEST );
        }
      }
      else 
      {
        IFILE_Release( pMe->mPBAP.pIFile );
        BTApp_PBAPHandlePBObjectListing( pMe, pDir );
      }
    }
  }
  if ( uResult != SUCCESS )
  {
    MSG_LOW( "PBAPVCardListingRsp - IBT returned failure", 0, 0, 0 );
    return FALSE;
  }
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_PBAPClientPullVCardListing
DESCRIPTION
============================================================================= */
static void BTApp_PBAPClientPullVCardListing( CBTApp* pMe )
{
  int      uResult = EFAILED;
  IFile*  pIFile = NULL;
  uint16   maxListCount;
  uint16   listStartOffset;
  char     sMaxListCount[ MAX_LIST_COUNT_VAL_LEN + 1 ];
  char     sListStartOffset[ MAX_LIST_COUNT_VAL_LEN + 1 ];

  MSG_LOW( "PBAPClientPullVCardListing", 0, 0, 0 );
  
  /* delete the listing file which is already present */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, VLIST_XML ) == SUCCESS )
  {
    if ( (pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, VLIST_XML,
                                      _OFM_READWRITE )) != NULL )
    {
      if ( IFILE_Truncate( pIFile, 0 ) != SUCCESS )
      {
        MSG_ERROR( "PBAP - can't truncate dirList file", 0, 0, 0 );
      }
      IFILE_Release( pIFile );
    }
    else
    {
      MSG_ERROR( "PBAP - can't open dirList file", 0, 0, 0 );
    }
  }
  
  /* get the vcard listing */
  /* convert listcount & listoffset form wchar to uint32 */
  WSTRTOSTR( pMe->mPBAP.wMaxListCount, sMaxListCount, sizeof(sMaxListCount) );
  maxListCount = ATOI( sMaxListCount );
  WSTRTOSTR( pMe->mPBAP.wListStartOffset, sListStartOffset, 
             sizeof(sListStartOffset) );
  listStartOffset = ATOI( sListStartOffset );

  // Save maxListCount for future use
  pMe->mPBAP.PBAPObject.maxListCount = maxListCount;

  uResult = IBTEXTPBAP_PullvCardListing( pMe->mPBAP.po, NULL, 
                               &pMe->mPBAP.sortOrder, pMe->mPBAP.wSearchValue,
                               &pMe->mPBAP.searchAttrib, 
                               &maxListCount, &listStartOffset );
  if ( uResult != SUCCESS )
  {
    MSG_HIGH( "PBAPClientPullVCardListing - IBT call failed", 0, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_PBAP_MSG_PULL_VCARD_LIST_FAIL, NULL, 2 );
    return;
  }                               
}

/* ==========================================================================
FUNCTION BTAPP_PBAPIsFolderNameValid
DESCRIPTION
============================================================================= */
static boolean BTAPP_PBAPIsFolderNameValid( CBTApp * pMe )
{
  char        sFolderName[ AEEBT_MAX_FILE_NAME + 1 ];
  boolean     retVal = FALSE;
  boolean     bValidICardPresent = FALSE;
  uint8       cardStatus = AEECARD_NO_CARD ; 

  WSTRTOSTR( pMe->mPBAP.PBAPObject.wName, sFolderName, sizeof(sFolderName) );

  if( pICard != NULL )
  {
    ICARD_GetCardStatus(pICard, &cardStatus);
  }
  if ((cardStatus != AEECARD_NO_CARD) &&
      (cardStatus != AEECARD_INVALID_CARD)) 
  {
    bValidICardPresent = TRUE;
  }

  if ( !STRICMP (sServerPath, AEEBT_PBAP_FOLDER_NAME_ROOT) )
  {
	  /* Sim folder can be browsed if valid SIM card is present in the phone */
	  if ( !STRICMP (sFolderName, AEEBT_PBAP_FOLDER_NAME_TELECOM) || 
		   ( !STRICMP (sFolderName, AEEBT_PBAP_FOLDER_NAME_SIM1) && 
		   (TRUE == bValidICardPresent) ) )
	  {
		  retVal = TRUE;
	  }
  }
  else if ( !STRICMP (sServerPath, BT_APP_PBAP_SIM1) )
  {
      if(TRUE == bValidICardPresent)
	  {
		  /* we are in root/SIM1.  Only "telecom" is allowed */
        if ( !STRICMP (sFolderName, AEEBT_PBAP_FOLDER_NAME_TELECOM) ) 
		{
		  retVal = TRUE;
		}
	  }
  }
  else if ( !STRICMP (sServerPath, BT_APP_PBAP_TELECOM) ||
            ( !STRICMP (sServerPath, BT_APP_PBAP_SIM1_TELCOM) && 
			(TRUE == bValidICardPresent) ) ) 
  {
    /* we are in either telecom or SIM1/telecom */
    if ( !STRICMP (sFolderName, AEEBT_PBAP_FOLDER_NAME_PB) || 
         !STRICMP (sFolderName, AEEBT_PBAP_FOLDER_NAME_ICH) || 
         !STRICMP (sFolderName, AEEBT_PBAP_FOLDER_NAME_OCH) ||
         !STRICMP (sFolderName, AEEBT_PBAP_FOLDER_NAME_MCH) ||
         !STRICMP (sFolderName, AEEBT_PBAP_FOLDER_NAME_CCH) )
    {
      retVal = TRUE;
    }
  }
  else
  {
    MSG_ERROR ("BTAPP_PBAPIsFolderNameValid - invalid sServerPath!", 0, 0, 0);
  }
  return retVal;
}

/* ==========================================================================
FUNCTION BTApp_PBAPServerSetPBRsp
DESCRIPTION
============================================================================= */
static void BTApp_PBAPServerSetPBRsp( CBTApp* pMe )
{
  char*       pTemp = NULL;
  char        sFolderName[ AEEBT_MAX_FILE_NAME + 1 ];
  boolean     bResult = TRUE;
  int         uStatus = EFAILED;
  
  MSG_LOW( "PBAPServerSetPBRsp", 0, 0, 0 );

  if ( pMe->mPBAP.PBAPObject.flag == AEEBT_PBAP_SET_PATH_TO_ROOT )
  {
    STRLCPY( sServerPath, AEEBT_PBAP_FOLDER_NAME_ROOT, sizeof( sServerPath ) );
  }
  else if ( pMe->mPBAP.PBAPObject.flag == AEEBT_PBAP_SET_PATH_TO_PARENT )
  {
    /* are we in the root */
    if ( STRCMP( sServerPath, AEEBT_PBAP_FOLDER_NAME_ROOT ) == 0 )
    {
      MSG_LOW( "PBAPServerSetPBRsp - already in root", 0, 0, 0 );
    }
    else 
    {
      pTemp = STRRCHR( sServerPath, '/' );
      if ( pTemp == NULL )
      {
        MSG_ERROR( "PBAPServerSetPBRsp - Srv Path Invalid", 0, 0, 0 );
        bResult = FALSE;
      }
      /* terminate the path with NULL so that only parent location is stored*/
      pTemp[0] = NULL;
    }
  }
  else if ( pMe->mPBAP.PBAPObject.flag == AEEBT_PBAP_SET_PATH_TO_FOLDER )
  {
    if ( BTAPP_PBAPIsFolderNameValid( pMe ) ) 
    {
      STRLCAT( sServerPath, "/", sizeof( sServerPath ) );
      WSTRTOSTR( pMe->mPBAP.PBAPObject.wName, sFolderName, 
                 sizeof(sFolderName) );
      STRLCAT( sServerPath, sFolderName, sizeof( sServerPath ) );
    }
    else
    {
      MSG_ERROR( "PBAPServerSetPBRsp - no folder name to set", 0, 0, 0 );
      bResult = FALSE;
    }
  }
  
  if ( bResult != FALSE )
  {
    uStatus = IBTEXTPBAP_SetPhoneBookResponse( pMe->mPBAP.po, AEEBT_ERR_NONE );
  }
  else
  {
    uStatus = IBTEXTPBAP_SetPhoneBookResponse( pMe->mPBAP.po, 
                                               AEEBT_ERR_OBEX_BAD_REQUEST );
  }
  if ( uStatus != SUCCESS )
  {
    MSG_ERROR( "PBAPServerSetPBRsp - IBT returned failure ", 0, 0, 0 );
  }
  return;
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildCCHObject
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPBuildCCHObject( CBTApp * pMe )
{
  const AEECallHistoryEntry*  pCallHistoryEntry = NULL;
  AEECallHistoryField*        pItems = NULL;
  dword       dTimeStamp = 0;
  AECHAR      wNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS];
  char        sNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS + 1];  
  uint8       callType;
  int         Status;
  int         i = 0;
  boolean     bReadAll = FALSE;
  uint16      uOffset = 0;
  uint16      uNoOfRecRead = 0;
  IAddrRec*   addrRec = NULL;
  boolean     bRecordFound = FALSE;
  char        sTimeStamp[BT_APP_PBAP_CALLHIST_MAXDIGITS];
  char        sBuf[100];

  /* create file for storing vcard details */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, CCH_VCF ) != SUCCESS )
  {
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           CCH_VCF, _OFM_CREATE );
  }
  else
  {
    if ( (pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                CCH_VCF, _OFM_READWRITE )) != NULL )
    {
      if ( IFILE_Truncate( pMe->mPBAP.pIFile, 0 ) != SUCCESS )
      {
        MSG_HIGH( "PBAPBuildCCHObject - File truncate failed", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );        
        return FALSE;
      }
    }
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_HIGH( "PBAPBuildCCHObject - File open failed", 0, 0, 0 );
    return FALSE;
  }

  /* if maxlist count is zero it would have been handled before */
  if ( pMe->mPBAP.PBAPObject.maxListCount == 65535 )
  {
    bReadAll = TRUE;
  }
  if ( pMe->mPBAP.PBAPObject.listStartOffset != 0 )
  {
    uOffset = pMe->mPBAP.PBAPObject.listStartOffset;
  }
  
  ICALLHISTORY_EnumInit( pMe->mPBAP.pCallHist );
  uNoOfRecRead = 1;
  while ( ((pCallHistoryEntry = ICALLHISTORY_EnumNext( 
              pMe->mPBAP.pCallHist, &Status)) != NULL ) && 
            (Status == SUCCESS) && ((bReadAll == TRUE) || 
            ((uNoOfRecRead - uOffset) <= pMe->mPBAP.PBAPObject.maxListCount)) )
  {
    if ( uNoOfRecRead > uOffset )
    {
      pItems = pCallHistoryEntry->pFields;
      if ( (pCallHistoryEntry->wNumFields != 0) && 
           (pItems != NULL) )
      {
        callType = 0;
        MEMSET( wNumber, 0, sizeof(wNumber) );
        for ( i = 0; i < pCallHistoryEntry->wNumFields; i++ )
        {
          switch (pItems[i].wID)
          {
            case AEECALLHISTORY_FIELD_DATE_TIME:
            {
              MEMCPY( &dTimeStamp, pItems[i].pData, pItems[i].wDataLen );
              break;
            }
            case AEECALLHISTORY_FIELD_NUMBER_EX:
            {
              MEMCPY( wNumber, pItems[i].pData, 
                      MIN(ARR_SIZE(wNumber)*sizeof(AECHAR), 
                          pItems[i].wDataLen) );
              break;
            }
            case AEECALLHISTORY_FIELD_CALL_TYPE:
            {
              callType = *( (uint8*)pItems[i].pData );
              break;
            }
          }
        }
        if ( (callType == AEECALLHISTORY_CALL_TYPE_FROM) ||
             (callType == AEECALLHISTORY_CALL_TYPE_TO) || 
             (callType == AEECALLHISTORY_CALL_TYPE_MISSED) )
        {
          /* add this entry to ICF object */
          /* find if there is an entry corresponding to this number in AB */
          bRecordFound = FALSE;
          if ( SUCCESS == IADDRBOOKEXT_EnumRecInitEx( 
                                           pMe->mPBAP.pAddrbook,
                                           AEE_ADDR_CAT_NONE,
                                           AEE_ADDRFIELD_NONE,
                                           wNumber,
                                           (uint16) WSTRSIZE(wNumber),
                                           ADDRREC_SEARCHTYPE_EXACT |
                                           ADDRREC_FIELDID_PHONE ) ) 
          {
            addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook );
            /* there might be multiple records, return only the first rec */
            if ( NULL != addrRec ) 
            {
              bRecordFound = TRUE;
              IFILE_Write( pMe->mPBAP.pIFile, "BEGIN:VCARD\r\n", 
                           STRLEN("BEGIN:VCARD\r\n") );
              if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
              { 
                IFILE_Write( pMe->mPBAP.pIFile, "VERSION:2.1\r\n", 
                             STRLEN("VERSION:2.1\r\n") );
              }
              else
              {
                IFILE_Write( pMe->mPBAP.pIFile, "VERSION:3.0\r\n", 
                             STRLEN("VERSION:3.0\r\n") );
              }
              /* add this record to the vcard */
              if ( BTApp_PBAPAddVCardProperties( pMe, addrRec, 
                                                 pMe->mPBAP.pIFile ) == FALSE )
              {
                MSG_HIGH( "PBAPBuildCCHObject - error adding vcard prop", 0, 0, 0 );
              }
              if ( ( pMe->mPBAP.PBAPObject.filter & AEEBT_PBAP_ATTRIBUTE_TIMESTAMP ) ||
                   ( pMe->mPBAP.PBAPObject.filter == 0 ) )
              {
                /* 
                 * If the attribute mask shows that the client 
                 * has requested for the time stamp 
                 */
                /* add time stamp property to this vcard */
                BTApp_PBAPFormatTime( pMe, &dTimeStamp, sTimeStamp,
                                      sizeof(sTimeStamp) );
                if ( callType == AEECALLHISTORY_CALL_TYPE_FROM )
                {
                  SNPRINTF( sBuf, sizeof( sBuf ), 
                            "X-IRMC-CALL-DATETIME;RECEIVED:%s\r\n", 
                            sTimeStamp );
                }
                else if ( callType == AEECALLHISTORY_CALL_TYPE_TO )
                {
                  SNPRINTF( sBuf, sizeof( sBuf ), 
                            "X-IRMC-CALL-DATETIME;DIALLED:%s\r\n", sTimeStamp );
                }
                else if ( callType == AEECALLHISTORY_CALL_TYPE_MISSED )
                {
                  SNPRINTF( sBuf, sizeof( sBuf ), 
                            "X-IRMC-CALL-DATETIME;MISSED:%s\r\n", sTimeStamp );
                }
                IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
              }
              IFILE_Write( pMe->mPBAP.pIFile, "END:VCARD\r\n", 
                           STRLEN("END:VCARD\r\n") );
            }
          }
          if ( bRecordFound == FALSE )
          {
            /* no record found, add just the time stamp property*/
            IFILE_Write( pMe->mPBAP.pIFile, "BEGIN:VCARD\r\n", 
                         STRLEN("BEGIN:VCARD\r\n") );
            if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
            { 
              IFILE_Write( pMe->mPBAP.pIFile, "VERSION:2.1\r\n", 
                           STRLEN("VERSION:2.1\r\n") );
            }
            else
            {
              IFILE_Write( pMe->mPBAP.pIFile, "VERSION:3.0\r\n", 
                           STRLEN("VERSION:3.0\r\n") );
            }
            /* add number property present in call history */
            WSTRTOSTR( wNumber, sNumber, sizeof(sNumber) );
            SNPRINTF( sBuf, sizeof( sBuf ), "TEL;VOICE:%s\r\n", sNumber );
            IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
            
            if ( ( pMe->mPBAP.PBAPObject.filter & AEEBT_PBAP_ATTRIBUTE_TIMESTAMP ) ||
                 ( pMe->mPBAP.PBAPObject.filter == 0) )
            {
              /* 
               * If the attribute mask shows that the client 
               * has requested for the time stamp 
               */

              BTApp_PBAPFormatTime( pMe, &dTimeStamp, sTimeStamp,
                                    sizeof(sTimeStamp) );
              if ( callType == AEECALLHISTORY_CALL_TYPE_FROM )
              {
                SNPRINTF( sBuf, sizeof( sBuf ), 
                          "X-IRMC-CALL-DATETIME;RECEIVED:%s\r\n", sTimeStamp );
              }
              else if ( callType == AEECALLHISTORY_CALL_TYPE_TO )
              {
                SNPRINTF( sBuf, sizeof( sBuf ), 
                          "X-IRMC-CALL-DATETIME;DIALLED:%s\r\n", sTimeStamp );
              }
              else if ( callType == AEECALLHISTORY_CALL_TYPE_MISSED )
              {
                SNPRINTF( sBuf, sizeof( sBuf ), 
                          "X-IRMC-CALL-DATETIME;MISSED:%s\r\n", sTimeStamp );
              }
              IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
            }
            IFILE_Write( pMe->mPBAP.pIFile, "END:VCARD\r\n", 
                         STRLEN("END:VCARD\r\n") );
          }
        }
      }
    }
    uNoOfRecRead++;
  }
  MSG_LOW( "PBAPBuildCCHObject - Rec added=%d", (uNoOfRecRead-1), 0, 0 );
  /* close the file */
  IFILE_Release( pMe->mPBAP.pIFile );
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildCHObject
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPBuildCHObject( CBTApp * pMe, uint16 uCallType )
{
  const AEECallHistoryEntry*  pCallHistoryEntry = NULL;
  AEECallHistoryField*        pItems = NULL;
  char*       pFileName = NULL;
  dword       dTimeStamp = 0;  
  AECHAR      wNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS + 1];
  char        sNumber[BT_APP_PBAP_CALLHISTORY_MAXLETTERS + 1];
  int         Status;
  int         i = 0;
  boolean     bReadAll = FALSE;
  uint16      uOffset = 0;
  uint16      uNoOfRecRead = 0;
  IAddrRec*   addrRec = NULL;
  boolean     bRecordFound = FALSE;
  char        sTimeStamp[BT_APP_PBAP_CALLHIST_MAXDIGITS + 1];
  char        sBuf[150];
  char*       pTimePropStr = NULL;

  /* create file for storing vcard details */
  if ( uCallType == AEECALLHISTORY_CALL_TYPE_FROM )
  {
    pFileName = ICH_VCF;
    pTimePropStr = "RECEIVED";
  }
  else if ( uCallType == AEECALLHISTORY_CALL_TYPE_TO )
  {
    pFileName = OCH_VCF;      
    pTimePropStr = "DIALLED";
  }
  else if ( uCallType == AEECALLHISTORY_CALL_TYPE_MISSED )
  {
    pFileName = MCH_VCF;      
    pTimePropStr = "MISSED";      
  }
  /* create file for storing objects */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, pFileName ) != SUCCESS )
  {
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           pFileName, _OFM_CREATE );
  }
  else
  {
    if ( (pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                pFileName, _OFM_READWRITE )) != NULL )
    {
      if ( IFILE_Truncate( pMe->mPBAP.pIFile, 0 ) != SUCCESS )
      {
        MSG_HIGH( "PBAPBuildCHObject - File truncate failed", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );
        return FALSE;
      }
    }
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_HIGH( "PBAPBuildCHObject - File open failed", 0, 0, 0 );
    return FALSE;
  }

  /* if maxlist count is zero it would have been handled before */
  if ( pMe->mPBAP.PBAPObject.maxListCount == 65535 )
  {
    bReadAll = TRUE;
  }
  if ( pMe->mPBAP.PBAPObject.listStartOffset != 0 )
  {
    uOffset = pMe->mPBAP.PBAPObject.listStartOffset;
  }
  
  ICALLHISTORY_EnumInitByCallType( pMe->mPBAP.pCallHist, uCallType );
  uNoOfRecRead = 1;
  while ( ((pCallHistoryEntry = ICALLHISTORY_EnumNext( 
              pMe->mPBAP.pCallHist, &Status)) != NULL ) && 
            (Status == SUCCESS) && ((bReadAll == TRUE) || 
            ((uNoOfRecRead - uOffset) <= pMe->mPBAP.PBAPObject.maxListCount)) )
  {
    if ( uNoOfRecRead > uOffset )
    {
      pItems = pCallHistoryEntry->pFields;
      if ( (pCallHistoryEntry->wNumFields != 0) && 
           (pItems != NULL) )
      {
        MEMSET( wNumber, 0, sizeof(wNumber) );
        for ( i = 0; i < pCallHistoryEntry->wNumFields; i++ )
        {
          switch (pItems[i].wID)
          {
            case AEECALLHISTORY_FIELD_DATE_TIME:
            {
              MEMCPY( &dTimeStamp, pItems[i].pData, pItems[i].wDataLen );
              break;
            }
            case AEECALLHISTORY_FIELD_NUMBER_EX:
            {
              MEMCPY( wNumber, pItems[i].pData, 
                      MIN(ARR_SIZE(wNumber)*sizeof(AECHAR), 
                          pItems[i].wDataLen) );
              break;
            }
            default:
              break;
          }
        }
        /* add this entry to ICF object */
        /* find if there is an entry corresponding to this number in AB */
        bRecordFound = FALSE;
        if ( SUCCESS == IADDRBOOKEXT_EnumRecInitEx( pMe->mPBAP.pAddrbook,
                                                    AEE_ADDR_CAT_NONE,
                                                    AEE_ADDRFIELD_NONE,
                                                    wNumber,
                                                    (uint16) WSTRSIZE(wNumber),
                                                    ADDRREC_SEARCHTYPE_EXACT |
                                                    ADDRREC_FIELDID_PHONE ) ) 
        {
          addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook );
          /* there might be multiple records, add only the first record */
          if ( NULL != addrRec ) 
          {
            bRecordFound = TRUE;
            IFILE_Write( pMe->mPBAP.pIFile, "BEGIN:VCARD\r\n", 
                         STRLEN("BEGIN:VCARD\r\n") );
            if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
            { 
              IFILE_Write( pMe->mPBAP.pIFile, "VERSION:2.1\r\n", 
                           STRLEN("VERSION:2.1\r\n") );
            }
            else
            {
              IFILE_Write( pMe->mPBAP.pIFile, "VERSION:3.0\r\n", 
                           STRLEN("VERSION:3.0\r\n") );
            }
            /* add this record to the vcard */
            if ( BTApp_PBAPAddVCardProperties( pMe, addrRec, 
                                               pMe->mPBAP.pIFile ) == FALSE )
            {
              MSG_HIGH("PBAPBuildCHObject- error adding vcard prop", 0, 0, 0);
            }

            if ( ( pMe->mPBAP.PBAPObject.filter & AEEBT_PBAP_ATTRIBUTE_TIMESTAMP ) ||
                 ( pMe->mPBAP.PBAPObject.filter == 0 ) )
            {
              /* 
               * If the attribute mask shows that the client 
               * has requested for the time stamp 
               */
              /* add time stamp property to this vcard */
              BTApp_PBAPFormatTime( pMe, &dTimeStamp, sTimeStamp,
                                    sizeof(sTimeStamp) );
              SNPRINTF( sBuf, sizeof( sBuf ), "X-IRMC-CALL-DATETIME;%s:%s\r\n", 
                        pTimePropStr, sTimeStamp );
              IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
            }
            IFILE_Write( pMe->mPBAP.pIFile, "END:VCARD\r\n", 
                         STRLEN("END:VCARD\r\n") );
          }
        }
        if ( bRecordFound == FALSE )
        {
          /* no record found, add just the time stamp property*/
          IFILE_Write( pMe->mPBAP.pIFile, "BEGIN:VCARD\r\n", 
                       STRLEN("BEGIN:VCARD\r\n") );
          if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
          { 
            IFILE_Write( pMe->mPBAP.pIFile, "VERSION:2.1\r\n", 
                         STRLEN("VERSION:2.1\r\n") );
          }
          else
          {
            IFILE_Write( pMe->mPBAP.pIFile, "VERSION:3.0\r\n", 
                         STRLEN("VERSION:3.0\r\n") );
          }
          /* add number property present in call history */
          WSTRTOSTR( wNumber, sNumber, sizeof(sNumber) );
          SNPRINTF( sBuf, sizeof( sBuf ), "TEL;VOICE:%s\r\n", sNumber );
          IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );

          if ( ( pMe->mPBAP.PBAPObject.filter & AEEBT_PBAP_ATTRIBUTE_TIMESTAMP ) ||
               ( pMe->mPBAP.PBAPObject.filter == 0 ) )
          {
            /* 
             * If the attribute mask shows that the client 
             * has requested for the time stamp 
             */
            BTApp_PBAPFormatTime( pMe, &dTimeStamp, sTimeStamp,
                                  sizeof(sTimeStamp) );
            SNPRINTF( sBuf, sizeof( sBuf ), "X-IRMC-CALL-DATETIME;%s:%s\r\n", 
                      pTimePropStr, sTimeStamp );
            IFILE_Write( pMe->mPBAP.pIFile, sBuf, STRLEN(sBuf) );
          }

          IFILE_Write( pMe->mPBAP.pIFile, "END:VCARD\r\n", 
                       STRLEN("END:VCARD\r\n") );            
        }
      }
    }
    uNoOfRecRead++;
  }
  MSG_LOW( "PBAPBuildCHObject - Rec added=%d", (uNoOfRecRead-1), 0, 0 );
  /* close the file */
  IFILE_Release( pMe->mPBAP.pIFile );
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildABObject
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPBuildABObject( CBTApp * pMe, boolean bFromSIM )
{
  int         Status = EFAILED;
  /* Keeps the count of records parsed in the phonebook */
  uint16      uNoOfRecRead = 0;
  /* Keeps the count of records being sent as a part of response */
  uint16      uNoOfRecInResp = 0;
  uint16      uOffset = 0;
  IAddrRec*   addrRec=NULL;
  boolean     bReadAll = FALSE;
  AEEAddrCat  category = AEE_ADDR_CAT_NONE;
  boolean     bSIMRec = FALSE;
  char*       pObjName = NULL;
  

  MSG_LOW( "PBAPBuildABObject - building Adress Book", 0, 0, 0 );
  /* not implemented the search feature */

  /* initialize the address book */
  Status = IADDRBOOKEXT_EnumRecInitEx( pMe->mPBAP.pAddrbook, AEE_ADDR_CAT_NONE,
                                  AEE_ADDRFIELD_NONE, NULL, 0, 0 );
  if ( Status == EFAILED )
  {
    MSG_HIGH( "PBAPBuildABObject - EnumRecInit return Failed", 0, 0, 0 );
    return FALSE;
  }
  
  if ( bFromSIM )
  {
    pObjName = SIM_PB_VCF;
  }
  else
  {
    pObjName = PB_VCF;
  }
  
  /* create file for storing vcard details */
  if ( IFILEMGR_Test( pMe->mPBAP.pIFileMgr, pObjName ) != SUCCESS )
  {
    pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                           pObjName, _OFM_CREATE );
  }
  else
  {
    if ( (pMe->mPBAP.pIFile = IFILEMGR_OpenFile( pMe->mPBAP.pIFileMgr, 
                                pObjName, _OFM_READWRITE )) != NULL )
    {
      if ( IFILE_Truncate( pMe->mPBAP.pIFile, 0 ) != SUCCESS )
      {
        MSG_HIGH( "PBAPBuildABObject - File truncate failed", 0, 0, 0 );
        IFILE_Release( pMe->mPBAP.pIFile );
        return FALSE;
      }
    }
  }
  if ( pMe->mPBAP.pIFile == NULL )
  {
    MSG_HIGH( "PBAPBuildABObject - File open failed", 0, 0, 0 );
    return FALSE;
  }

  /* if maxlist count is zero it would have been handled before */
  if ( pMe->mPBAP.PBAPObject.maxListCount == 65535 )
  {
    bReadAll = TRUE;
  }
  if ( pMe->mPBAP.PBAPObject.listStartOffset != 0 )
  {
    uOffset = pMe->mPBAP.PBAPObject.listStartOffset;
  }

  /* first vcard entry should contain the details of local device */
  if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
  {
    IFILE_Write( pMe->mPBAP.pIFile, BT_APP_VCARD2_1, STRLEN(BT_APP_VCARD2_1) );
  }
  else
  {
    IFILE_Write( pMe->mPBAP.pIFile, BT_APP_VCARD3_0, STRLEN(BT_APP_VCARD3_0) );
  }
  /* Here the 1st entry in the response is added */
  /* this corresponds to the local device vcard 0.vcf */
  uNoOfRecInResp++;
  /* start reading the records */
  addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook );
  /* The 1st entry in the addrbook is browsed here */
  uNoOfRecRead++;

  while ( (addrRec != NULL) && ( (bReadAll == TRUE) || 
          (uNoOfRecInResp <= pMe->mPBAP.PBAPObject.maxListCount)))
  {
    if ( uNoOfRecRead > uOffset )
    {
      /* check if the address category is not sim */
      category = IADDRREC_GetCategory(addrRec);
      bSIMRec = BTApp_PBAPIsSIMRec(category);
      if ( bSIMRec == bFromSIM )
      {
        IFILE_Write( pMe->mPBAP.pIFile, "BEGIN:VCARD\r\n", 
                     STRLEN("BEGIN:VCARD\r\n") );
        if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_2_1 )
        {
          IFILE_Write( pMe->mPBAP.pIFile, "VERSION:2.1\r\n", 
                       STRLEN("VERSION:2.1\r\n") );
        }
        else if ( pMe->mPBAP.PBAPObject.format == AEEBT_PBAP_FORMAT_VCARD_3_0 )
        {
          IFILE_Write( pMe->mPBAP.pIFile, "VERSION:3.0\r\n", 
                       STRLEN("VERSION:3.0\r\n") );
        }
        /* ad properties to vcard object */
        if ( BTApp_PBAPAddVCardProperties( pMe, addrRec, 
                                           pMe->mPBAP.pIFile ) == FALSE )
        {
          MSG_HIGH("PBAPBuildABObject- error adding vcard prop", 0, 0, 0);
        }
        IFILE_Write( pMe->mPBAP.pIFile, "END:VCARD\r\n", 
                     STRLEN("END:VCARD\r\n") );
      }
      IADDRREC_Release( addrRec );
      addrRec = NULL;
      /* Adding count to the number of response */
      uNoOfRecInResp++;
    }
    /* Next record being read now from the Addrbook*/
    addrRec = IADDRBOOK_EnumNextRec( pMe->mPBAP.pAddrbook );
    uNoOfRecRead++;
  }
  /* close the file */
  IFILE_Release( pMe->mPBAP.pIFile );
  return TRUE;
}

//#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/* ==========================================================================
FUNCTION BTApp_PBAPBuildSIMPBObject
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPBuildSIMPBObject( CBTApp * pMe, uint8 uPBType )
{
  boolean     bStatus = FALSE;
  uint16      uNoOfRec = 0;
  AECHAR*     pwObjectName;
  
  MSG_LOW( "PBAPBuildSIMPBObject - objtype=%d", uPBType, 0, 0 );

  switch ( uPBType )
  {
    case BT_APP_PBAP_OBJECT_PB:
    {
      /* maxlist count */
      if ( pMe->mPBAP.PBAPObject.maxListCount == 0 )
      {
        uNoOfRec = BTApp_PBAPFindNumberOfABItems( pMe, TRUE );
        uNoOfRec++; // to account for 0.vcf record
        MSG_HIGH("PBAPBuildSIMPBObject - num records = %d", uNoOfRec, 0, 0);
        /* no need to generate the pb.vcf object, just return the no of items */
        if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
               &uNoOfRec, NULL, AEEBT_ERR_NONE ) != SUCCESS )
        {
          MSG_ERROR( "PBAPBuildSIMPBObject - pull PB resp failed", 0, 0, 0 );
        }
      }
      else
      {
        bStatus = BTApp_PBAPBuildABObject( pMe, TRUE );
        if ( bStatus != FALSE )
        {
          pwObjectName=(AECHAR*)MALLOC((STRLEN(SIM_PB_VCF)+1)*sizeof(AECHAR));
          if ( pwObjectName == NULL )
          {
            MSG_ERROR( "PBAPBuildSIMPBObject - malloc failed", 0, 0, 0 );
            IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
                                              NULL, NULL, AEEBT_ERR_RESOURCE );
            return FALSE;
          }
          STRTOWSTR( SIM_PB_VCF, pwObjectName, 
                     (STRLEN(SIM_PB_VCF)+1)*sizeof(AECHAR) );
          if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, pwObjectName, 
                                      NULL, NULL, AEEBT_ERR_NONE ) != SUCCESS )
          {
            MSG_HIGH( "PBAPBuildSIMPBObject - pull PB resp failed", 0, 0, 0 );
            FREE( (void*)pwObjectName );
            return FALSE;
          }
        }
      }
      break;
    }
    case BT_APP_PBAP_OBJECT_ICH:
    case BT_APP_PBAP_OBJECT_OCH:
    case BT_APP_PBAP_OBJECT_MCH:
    case BT_APP_PBAP_OBJECT_CCH:
    {
      MSG_HIGH( "PBAPBuildSIMPBObject- Call Hist not stored in SIM", 0, 0, 0 );
      IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
                                        NULL, NULL, 
                                        AEEBT_ERR_OBEX_NOT_FOUND );
      break;
    }
    default:
      break; 
  }
  return TRUE;
}
//#endif

/* ==========================================================================
FUNCTION BTApp_PBAPBuildLocalPBObject
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPBuildLocalPBObject( CBTApp * pMe, uint8 uPBType )
{
  boolean     bStatus = FALSE;
  uint16      uNoOfRec = 0;
  uint16      uCallType = 0;
  uint8       uNewMissedCalls = 0;
  int         Status;
  char*       pFileName = NULL;
  AECHAR*     pwObjectName = NULL;

  MSG_LOW( "PBAPBuildLocalPBObject - objtype=%d", uPBType, 0, 0 );
  switch ( uPBType )
  {
    case BT_APP_PBAP_OBJECT_PB:
    {
      /* maxlist count */
      if ( pMe->mPBAP.PBAPObject.maxListCount == 0 )
      {
        uNoOfRec = BTApp_PBAPFindNumberOfABItems( pMe, FALSE );
        uNoOfRec++; // to account for o.vcf record
        MSG_HIGH("PBAPBuildLocalPBObject - num records = %d", uNoOfRec, 0, 0);
        /* no need to generate the pb.vcf object, just return the no of items*/
        if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
               &uNoOfRec, NULL, AEEBT_ERR_NONE ) != SUCCESS )
        {
          MSG_ERROR( "PBAPBuildLocalPBObject - pull PB resp failed", 0, 0, 0 );
        }
      }
      else
      {
        bStatus = BTApp_PBAPBuildABObject( pMe, FALSE );
        if ( bStatus != FALSE )
        {
          pwObjectName = (AECHAR*)MALLOC( (STRLEN(PB_VCF)+1)*sizeof(AECHAR) );
          if ( pwObjectName == NULL )
          {
            MSG_ERROR( "PBAPBuildLocalPBObject - malloc failed", 0, 0, 0 );
            IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
                                              NULL, NULL, AEEBT_ERR_RESOURCE );

            return FALSE;
          }
          STRTOWSTR( PB_VCF, pwObjectName, (STRLEN(PB_VCF)+1)*sizeof(AECHAR) );
          if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, pwObjectName, 
                                      NULL, NULL, AEEBT_ERR_NONE ) != SUCCESS )
          {
            MSG_HIGH( "PBAPBuildLocalPBObject - pull PB resp failed", 0, 0, 0 );
            FREE( (void*)pwObjectName );
            return FALSE;
          }
        }
      }
      break;
    }
    case BT_APP_PBAP_OBJECT_ICH:
    case BT_APP_PBAP_OBJECT_OCH:
    case BT_APP_PBAP_OBJECT_MCH:
    {
      if ( uPBType == BT_APP_PBAP_OBJECT_ICH )
      {
        uCallType = AEECALLHISTORY_CALL_TYPE_FROM;
        pFileName = ICH_VCF;      
      }
      else if ( uPBType == BT_APP_PBAP_OBJECT_OCH )
      {
        uCallType = AEECALLHISTORY_CALL_TYPE_TO;
        pFileName = OCH_VCF;       
      }
      else if ( uPBType == BT_APP_PBAP_OBJECT_MCH )
      {
        uCallType = AEECALLHISTORY_CALL_TYPE_MISSED;
        pFileName = MCH_VCF;
      }
      /* maxlist count */
      if ( pMe->mPBAP.PBAPObject.maxListCount == 0 )
      {
        ICALLHISTORY_EnumInitByCallType( pMe->mPBAP.pCallHist, uCallType );
        while ( (ICALLHISTORY_EnumNext( pMe->mPBAP.pCallHist, 
                                        &Status) != NULL ) && 
                (Status == SUCCESS) )
        {
          uNoOfRec++;
        }
        MSG_HIGH("PBAPBuildLocalPBObject - num records = %d", uNoOfRec, 0, 0);        
        /* no need to generate the pb.vcf object, just return the no of items */
        if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
               &uNoOfRec, NULL, AEEBT_ERR_NONE ) != SUCCESS )
        {
          MSG_ERROR( "PBAPBuildLocalPBObject - pull PB resp failed", 0, 0, 0 );
        }
      }
      else
      {
        bStatus = BTApp_PBAPBuildCHObject( pMe, uCallType );
        if ( bStatus != FALSE )
        {
          pwObjectName=(AECHAR*)MALLOC((STRLEN(pFileName)+1)*sizeof(AECHAR));
          if ( pwObjectName == NULL )
          {
            MSG_ERROR( "PBAPBuildLocalPBObject - malloc failed", 0, 0, 0 );
            IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
                                              NULL, NULL, AEEBT_ERR_RESOURCE );            
            return FALSE;
          }
          STRTOWSTR( pFileName, pwObjectName, 
                     (STRLEN(pFileName)+1)*sizeof(AECHAR) );

          if ( uPBType == BT_APP_PBAP_OBJECT_MCH ) 
          {
            /* obtain the number of missed calls from the missed call history */
            ICALLHISTORY_EnumInitByCallType( pMe->mPBAP.pCallHist, 
                                             AEECALLHISTORY_CALL_TYPE_MISSED );
            while ( (ICALLHISTORY_EnumNext( pMe->mPBAP.pCallHist, &Status) != NULL ) &&
                    (Status == SUCCESS) )
            {
              uNewMissedCalls++;
            }

            // Send new missed calls info if requested phonebook type is MCH
            if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, pwObjectName, 
                   NULL, &uNewMissedCalls, AEEBT_ERR_NONE ) != SUCCESS )
            {
              MSG_HIGH( "PBAPBuildLocalPBObject - pull PB resp failed", 0, 0, 0 );
              FREE( (void*)pwObjectName );
              return FALSE;
            }
      
            MSG_HIGH( "PBAPBuildLocalPBObject - newMissedCalls = %d", 
                      uNewMissedCalls, 0, 0 );
          }
          else
          {
            if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, pwObjectName, 
                   NULL, NULL, AEEBT_ERR_NONE ) != SUCCESS )
            {
              MSG_HIGH( "PBAPBuildLocalPBObject - pull PB resp failed", 0, 0, 0 );
              FREE( (void*)pwObjectName );
              return FALSE;
            }
          }
        }        
      }
      break;
    }
    case BT_APP_PBAP_OBJECT_CCH:
    {
      /* maxlist count */
      if ( pMe->mPBAP.PBAPObject.maxListCount == 0 )
      {
        ICALLHISTORY_EnumInit( pMe->mPBAP.pCallHist );
        while ( (ICALLHISTORY_EnumNext( pMe->mPBAP.pCallHist, 
                                        &Status) != NULL ) && 
                (Status == SUCCESS) )
        {
          uNoOfRec++;
        }
        MSG_HIGH("PBAPBuildLocalPBObject - num records = %d", uNoOfRec, 0, 0);        
        /* no need to generate the pb.vcf object, just return the no of items */
        if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
               &uNoOfRec, NULL, AEEBT_ERR_NONE ) != SUCCESS )
        {
          MSG_ERROR( "PBAPBuildLocalPBObject - pull PB resp failed", 0, 0, 0 );
        }
      }
      else
      {
        bStatus = BTApp_PBAPBuildCCHObject( pMe );
        if ( bStatus != FALSE )
        {
          pwObjectName = (AECHAR*)MALLOC( (STRLEN(CCH_VCF)+1)*sizeof(AECHAR) );
          if ( pwObjectName == NULL )
          {
            MSG_ERROR( "PBAPBuildLocalPBObject - malloc failed", 0, 0, 0 );
            IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
                                              NULL, NULL, AEEBT_ERR_RESOURCE );
            return FALSE;
          }
          STRTOWSTR(CCH_VCF, pwObjectName, (STRLEN(CCH_VCF)+1)*sizeof(AECHAR));
          if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, pwObjectName, 
                                      NULL, NULL, AEEBT_ERR_NONE ) != SUCCESS )
          {
            MSG_HIGH( "PBAPBuildLocalPBObject - pull PB resp failed", 0, 0, 0 );
            FREE( (void*)pwObjectName );
            return FALSE;
          }
        }        
      }
      break;
    }
    default:
      return FALSE;
  }

  if ( pwObjectName ) 
  {
    FREE( (void*)pwObjectName );
  }

  return bStatus;
}

/* ==========================================================================
FUNCTION BTApp_PBAPServerDownloadRsp
DESCRIPTION
============================================================================= */
static void BTApp_PBAPServerDownloadRsp(CBTApp * pMe)
{
  char         objectName[ AEEBT_MAX_FILE_NAME + 1 ];
  char*        pPBName = NULL;
  char         pRep[BT_APP_PBAP_SIZEOF_DIR];
  uint8        uRep = AEEBT_PBAP_REPOSITORY_INVALID;
  uint8        uPBType = 0;
  uint8        cardStatus = AEECARD_NO_CARD ; 

  WSTRTOSTR( pMe->mPBAP.PBAPObject.pwObjectName, 
             objectName, sizeof(objectName) );
  /* extract the repository name from the path */
  pPBName = STRCHR( objectName, '/' );
  if ( pPBName == NULL )
  {
    MSG_ERROR( "PBAPServerDownloadRsp - Invalid object name", 0, 0, 0 );

    if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
             NULL, NULL, AEEBT_ERR_OBEX_BAD_REQUEST ) != SUCCESS )
    {
      MSG_ERROR( "PBAPServerDownloadRsp - pull PB resp failed", 0, 0, 0 );
    }
    return;
  }
  MEMCPY( pRep, objectName, (pPBName - objectName) );
  pRep[pPBName - objectName] = 0; //null terminate
  if ( STRICMP(pRep, BT_APP_PBAP_PHONE_DIR) == 0 )
  {
    uRep = AEEBT_PBAP_REPOSITORY_LOCAL;
    MSG_LOW( "PBAPServerDownloadRsp - phone obj req", 0, 0, 0 );
  }
  else if ( STRICMP(pRep, BT_APP_PBAP_SIM_DIR) == 0 )
  {
    if( pICard != NULL)
    {
      ICARD_GetCardStatus(pICard, &cardStatus);
    }
    if ((cardStatus == AEECARD_NO_CARD) ||
        (cardStatus == AEECARD_INVALID_CARD)) 
    {
  	  if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
               NULL, NULL, AEEBT_ERR_OBEX_NOT_FOUND) != SUCCESS )
      {
        MSG_ERROR( "PBAPServerDownloadRsp - pull PB resp failed", 0, 0, 0 );
      }
      return;
    }
    uRep = AEEBT_PBAP_REPOSITORY_SIM_CARD;
    MSG_LOW( "PBAPServerDownloadRsp - SIM1 obj req", 0, 0, 0 );
  }
  else
  {
    MSG_ERROR( "PBAPServerDownloadRsp - Invalid object name", 0, 0, 0 );

    if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
             NULL, NULL, AEEBT_ERR_OBEX_BAD_REQUEST ) != SUCCESS )
    {
      MSG_ERROR( "PBAPServerDownloadRsp - pull PB resp failed", 0, 0, 0 );
    }
    return;
  }
  /* find the phone object being requested */
  pPBName = STRRCHR( objectName, '/' );
  if ( pPBName == NULL )
  {
    MSG_ERROR( "PBAPServerDownloadRsp - Invalid object name", 0, 0, 0 );

    if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
             NULL, NULL, AEEBT_ERR_OBEX_BAD_REQUEST ) != SUCCESS )
    {
      MSG_ERROR( "PBAPServerDownloadRsp - pull PB resp failed", 0, 0, 0 );
    }
    return;
  }
  pPBName++; // points to phonebook object
  
  if ( STRICMP( pPBName, "pb.vcf" ) == 0 )
  {
    uPBType = BT_APP_PBAP_OBJECT_PB;
  }
  else if ( STRICMP( pPBName, "ich.vcf" ) == 0 )
  {
    uPBType = BT_APP_PBAP_OBJECT_ICH;  
  }
  else if ( STRICMP( pPBName, "och.vcf" ) == 0 )
  {
    uPBType = BT_APP_PBAP_OBJECT_OCH;  
  }
  else if ( STRICMP( pPBName, "mch.vcf" ) == 0 )
  {
    uPBType = BT_APP_PBAP_OBJECT_MCH;  
  }
  else if ( STRICMP( pPBName, "cch.vcf" ) == 0 )
  {
    uPBType = BT_APP_PBAP_OBJECT_CCH;  
  }
  else
  {
    MSG_ERROR( "PBAPServerDownloadRsp - invalid object request", 0, 0, 0 );

    if ( IBTEXTPBAP_PullPhoneBookResponse( pMe->mPBAP.po, NULL, 
             NULL, NULL, AEEBT_ERR_OBEX_BAD_REQUEST ) != SUCCESS )
    {
      MSG_ERROR( "PBAPServerDownloadRsp - pull PB resp failed", 0, 0, 0 );
    }
    return;
  }

  if ( uRep == AEEBT_PBAP_REPOSITORY_LOCAL )
  {
    BTApp_PBAPBuildLocalPBObject( pMe, uPBType );
  }
  else if ( uRep == AEEBT_PBAP_REPOSITORY_SIM_CARD )
  {
    /* WCDMA or GSM mode */
//#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    BTApp_PBAPBuildSIMPBObject( pMe, uPBType );
//#else
    // MSG_LOW( "PBAPServerDownloadRsp - No SIM available", 0, 0, 0 );
//#endif
  }
}

/* ==========================================================================
FUNCTION BTApp_PBAPClientDownloadReq
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPClientDownloadReq(CBTApp * pMe, uint16 sel)
{
  const char*          objectName = NULL;
  AEEBTPBAPAttribMask  attribMask = 0;
  AECHAR               wObjectName[ AEEBT_MAX_FILE_NAME + 1 ];
  char                 sFilterValue[ MAX_FILTER_VAL_LEN +1 ];
  char                 sMaxListCount[ MAX_LIST_COUNT_VAL_LEN + 1 ];
  char                 sListStartOffset[ MAX_LIST_COUNT_VAL_LEN + 1 ];
  uint16               maxListCount = 0xFFFF;
  uint16               listStartOffset = 0;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif //FEATURE_BT_TEST_API

  if ( pMe->mPBAP.pbRepository == AEEBT_PBAP_REPOSITORY_LOCAL )
  {
    switch ( sel )
    {
      case IDS_PBAP_DW_PB:
        objectName = AEEBT_PBAP_OBJECT_NAME_LOCAL_PB;
        break;
      case IDS_PBAP_DW_ICH:
        objectName = AEEBT_PBAP_OBJECT_NAME_LOCAL_ICH;
        break;
      case IDS_PBAP_DW_OCH:
        objectName = AEEBT_PBAP_OBJECT_NAME_LOCAL_OCH;
        break;
      case IDS_PBAP_DW_MCH:
        objectName = AEEBT_PBAP_OBJECT_NAME_LOCAL_MCH;
        break;
      case IDS_PBAP_DW_CCH:
        objectName = AEEBT_PBAP_OBJECT_NAME_LOCAL_CCH;
        break;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif //FEATURE_BT_TEST_API
      default:
        MSG_ERROR( "PBAPClientDownloadReq - invalid selection", 0, 0, 0 );
        return FALSE;
    }
  }
  else if ( pMe->mPBAP.pbRepository == AEEBT_PBAP_REPOSITORY_SIM_CARD )
  {
    switch ( sel )
    {
      case IDS_PBAP_DW_PB:
        objectName = AEEBT_PBAP_OBJECT_NAME_SIM_PB;
        break;
      case IDS_PBAP_DW_ICH:
        objectName = AEEBT_PBAP_OBJECT_NAME_SIM_ICH;
        break;
      case IDS_PBAP_DW_OCH:
        objectName = AEEBT_PBAP_OBJECT_NAME_SIM_OCH;
        break;
      case IDS_PBAP_DW_MCH:
        objectName = AEEBT_PBAP_OBJECT_NAME_SIM_MCH;
        break;
      case IDS_PBAP_DW_CCH:
        objectName = AEEBT_PBAP_OBJECT_NAME_SIM_CCH;
        break;
      default:
        MSG_ERROR( "PBAPClientDownloadReq - invalid selection", 0, 0, 0 );
        return FALSE;
    }
  }
  else
  {
    MSG_ERROR( "PBAPClientDownloadReq - no repository selected", 0, 0, 0 );
    return FALSE;
  }
  // remember requested phonebook object requested
  STRLCPY( pMe->mPBAP.szObjectName, objectName, sizeof(pMe->mPBAP.szObjectName) );

  // convert object name from string to AECHAR
  STRTOWSTR( objectName, wObjectName, sizeof(wObjectName) );

  // convert attribute mask, listcount & listoffset form wchar to uint32
  WSTRTOSTR( pMe->mPBAP.wFilterValue, sFilterValue, sizeof(sFilterValue) );
  attribMask = ATOI( sFilterValue );
  
  WSTRTOSTR( pMe->mPBAP.wMaxListCount, sMaxListCount, sizeof(sMaxListCount) );
  maxListCount = ATOI( sMaxListCount );

  WSTRTOSTR( pMe->mPBAP.wListStartOffset, sListStartOffset, 
             sizeof(sListStartOffset) );
  listStartOffset = ATOI( sListStartOffset );

  // Save maxListCount for future use
  pMe->mPBAP.PBAPObject.maxListCount = maxListCount;

  if ( IBTEXTPBAP_PullPhoneBook( pMe->mPBAP.po, wObjectName, &attribMask, 
                                 &pMe->mPBAP.vCardFormat,
                                 &maxListCount,
                                 &listStartOffset ) != SUCCESS )
  {
    MSG_ERROR( "PullPhoneBook - returned failure", 0, 0, 0 );
    BTApp_ShowMessage( pMe, IDS_PBAP_MSG_CLIENT_DOWNLOAD_FAILED, NULL, 3 );
    return FALSE;
  }
  else
  {
    BTApp_ShowBusyIcon( pMe );
  }
  return TRUE;
}

/* ==========================================================================
FUNCTION BTApp_HandlePBAPAuthenticateMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPAuthenticateMenu(CBTApp * pMe, uint16 key)
{
  boolean               ev_processed = FALSE;
  uint8                 i;
  uint16                selection = 0;
  CtlAddItem            ai;
  uint16                sel = IDS_AUTH_NONE;
  AEEBTPBAPAuthentication    authOptions;

  MSG_MED( "HndlPBAPAuthenticateMenu - k=0x%x", key, 0, 0 );
  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_CLR:
    {
      for ( i=IMENUCTL_GetItemCount( pMe->m_pIMenu ); i>0; i-- )
      {
        if ( IMENUCTL_GetItem( pMe->m_pIMenu, 
                               IMENUCTL_GetItemID( pMe->m_pIMenu, i-1 ), 
                               &ai ) != FALSE )
        {
          if ( ai.wImage == IDB_RADIO_FILLED )
          {
            selection = ai.wItemID;
            break;
          }
        }
      }
      switch ( selection )
      {
        case IDS_AUTH_NONE:
          authOptions = AEEBT_PBAP_AUTH_NONE;
          break;
        case IDS_AUTH_PWD:
          authOptions = AEEBT_PBAP_AUTH_PIN;
          break;
        case IDS_AUTH_USERID_PWD:
        default:
          authOptions = AEEBT_PBAP_AUTH_USERID_AND_PIN;
          break;
      }
      if ( authOptions != pMe->mPBAP.authOptions )
      {
        pMe->bConfigChanged = TRUE;
        pMe->mPBAP.authOptions = authOptions;
      }
      if(authOptions)
      {
          pMe->mPBAP.bDoAuthenticate = TRUE ;
      } 
      else
      {
          pMe->mPBAP.bDoAuthenticate = FALSE ;
      }

      if ( key == AVK_CLR )
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      // update menu
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->a.m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );

        switch ( sel )
        {
          case IDS_AUTH_NONE:
            sel = IDS_AUTH_PWD;
            break;
          case IDS_AUTH_PWD:
            sel = IDS_AUTH_USERID_PWD;
            break;
          case IDS_AUTH_USERID_PWD:
            sel = 0;
            break;
        }
      }
      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandlePBAPClientBrowseMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPClientBrowseMenu(CBTApp * pMe, uint16 key)
{
  boolean ev_processed = TRUE;
  uint8   uIndex = 0;
  AECHAR  wName[ AEEBT_MAX_FILE_NAME+1 ]; 
  int     result = 0;
  AEEBTPBAPAttribMask  attribMask = 0;
  char                 sFilterValue[ MAX_FILTER_VAL_LEN +1 ];
  
  MSG_MED( "HandlePBAPClientBrowseMenu - k=0x%x", key, 0, 0 );

  switch ( key )
  {
    case AVK_SELECT:
    {
      if ( IMENUCTL_GetItemCount( pMe->m_pIMenu ) > 0 )
      {
        uIndex = IMENUCTL_GetSel( pMe->m_pIMenu );
        STRTOWSTR( pMe->mPBAP.pbObjects[uIndex].szObjHdleName, 
                   wName, sizeof(wName) );
        if ( pMe->mPBAP.pbObjects[uIndex].bFolder != FALSE )
        {
          /* verify if the folder name is *.vcf, which indicates a list folder 
          request from one level above the leaf node */
          if ( (STRCMP( pMe->mPBAP.pbObjects[uIndex].szObjHdleName, 
                        "pb.vcf") == 0) ||
               (STRCMP( pMe->mPBAP.pbObjects[uIndex].szObjHdleName, 
                        "ich.vcf") == 0) ||                       
               (STRCMP( pMe->mPBAP.pbObjects[uIndex].szObjHdleName, 
                        "och.vcf") == 0) ||                       
               (STRCMP( pMe->mPBAP.pbObjects[uIndex].szObjHdleName, 
                        "mch.vcf") == 0) ||                       
               (STRCMP( pMe->mPBAP.pbObjects[uIndex].szObjHdleName, 
                        "cch.vcf") == 0) )
          {
            BTApp_PBAPClientPullVCardListingAboveLeaf( pMe, uIndex );
          }
          else
          {
            /* This is a normal browse request */
            STRLCPY( pMe->mPBAP.sSetFolder, 
                     pMe->mPBAP.pbObjects[uIndex].szObjHdleName, 
                     sizeof( pMe->mPBAP.sSetFolder ) );
            pMe->mPBAP.setPathFlag = AEEBT_PBAP_SET_PATH_TO_FOLDER;
            /* this is a folder so browse this folder */
            result = IBTEXTPBAP_SetPhoneBook( pMe->mPBAP.po, wName, 
                                              AEEBT_PBAP_SET_PATH_TO_FOLDER );
            if ( result != SUCCESS )
            {
              MSG_HIGH( "HdlePBAPClientBrowseMenu - set path fail", 0, 0, 0 );
              pMe->mPBAP.sSetFolder[0] = NULL;
              BTApp_ShowMessage(pMe, IDS_PBAP_MSG_PULL_VCARD_LIST_FAIL, NULL, 2);
            }
            else
            {
              BTApp_ShowBusyIcon( pMe );
            }
          }
        }
        else
        {
          /* get the vcard */
          WSTRTOSTR( pMe->mPBAP.wFilterValue, sFilterValue, 
                     sizeof(sFilterValue) );
          attribMask = ATOI( sFilterValue );
          
          result = IBTEXTPBAP_PullvCardEntry( pMe->mPBAP.po, wName, 
                                              &attribMask, 
                                              &pMe->mPBAP.vCardFormat );
          if ( result != SUCCESS )
          {
            MSG_HIGH( "HdlePBAPBrowseMenu - pull vcard entry fail", 0, 0, 0 );
            pMe->mPBAP.sSetFolder[0] = NULL;
            BTApp_ShowMessage(pMe, IDS_PBAP_MSG_PULL_VCARD_LIST_FAIL, NULL, 2);
          }
          else
          {
            BTApp_ShowBusyIcon( pMe );
          }
        }
      }
      break;
    }
    case AVK_CLR:
    {
      /* check if operation needs to be aborted */
      if ( pMe->bBusyIconUp != FALSE )
      {
        MSG_HIGH( "HdlePBAPCliBrowseMenu - Aborting.. ", 0, 0, 0 );
        result = IBTEXTPBAP_Abort( pMe->mPBAP.po );
        if ( result != SUCCESS )
        {
          MSG_HIGH( "HdlePBAPCliBrowseMenu - Abort failed", 0, 0, 0 );
          BTApp_ShowMessage(pMe, IDS_PBAP_MSG_ABORT_REQ_FAIL, NULL, 2);
        }        
      }
      else
      {
        if ( STRCMP( sClientPath, AEEBT_PBAP_FOLDER_NAME_ROOT ) == 0 )
        {
          ev_processed = BTApp_HandleClearKey( pMe );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
        }
        else
        {
          pMe->mPBAP.setPathFlag = AEEBT_PBAP_SET_PATH_TO_PARENT; 
          pMe->mPBAP.sSetFolder[0] = NULL;       
          result = IBTEXTPBAP_SetPhoneBook( pMe->mPBAP.po, NULL, 
                                            AEEBT_PBAP_SET_PATH_TO_PARENT );
          if ( result != SUCCESS )
          {
            MSG_HIGH( "HdlePBAPClientBrowseMenu - set path fail", 0, 0, 0 );
            BTApp_ShowMessage(pMe, IDS_PBAP_MSG_PULL_VCARD_LIST_FAIL, NULL, 2);
          }
          else
          {
            BTApp_ShowBusyIcon( pMe );
          }
        }
      }
      break;
    }
    default:
      ev_processed = FALSE;
      MSG_MED( "Received key press event = %x", key, 0, 0 );
      break;
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandlePBAPClientDownloadSubMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPClientDownloadSubMenu(CBTApp * pMe, uint16 key)
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HandlePBAPClientDownloadSubMenu - k=0x%x", key, 0, 0 );

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
        break;
      }
      case AVK_CLR:
      {
        /* check if curr Operation needs to be aborted */
        if ( pMe->bBusyIconUp != FALSE )
        {
          MSG_HIGH( "HdlePBAPClientDownloadSubMenu - Aborting...", 0, 0, 0 );
          if ( IBTEXTPBAP_Abort( pMe->mPBAP.po ) != SUCCESS )
          {
            MSG_HIGH( "HdlePBAPCliDwnldSubMenu - PBAP Abort Failed", 0, 0, 0 );
            BTApp_ShowMessage(pMe, IDS_PBAP_MSG_ABORT_REQ_FAIL, NULL, 2);
          }                
        }
        else
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
FUNCTION BTApp_HandlePBAPClientDownloadMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPClientDownloadMenu(CBTApp * pMe, uint16 key)
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HandlePBAPClientDownloadMenu - k=0x%x", key, 0, 0 );

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
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
FUNCTION BTApp_HandlePBAPSearchAttributeMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPSearchAttributeMenu( CBTApp* pMe, uint16 key )
{
  boolean               ev_processed = FALSE;
  uint8                 i;
  uint16                selection = 0;
  AEEBTPBAPSearchAttrib searchAttrib;
  CtlAddItem            ai;
  uint16                sel = IDS_PBAP_SEARCH_NAME;


  MSG_MED( "HndlPBAPSearchAttributeMenu - k=0x%x", key, 0, 0 );
  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_CLR:
    {
      for ( i=IMENUCTL_GetItemCount( pMe->m_pIMenu ); i>0; i-- )
      {
        if ( IMENUCTL_GetItem( pMe->m_pIMenu, 
                               IMENUCTL_GetItemID( pMe->m_pIMenu, i-1 ), 
                               &ai ) != FALSE )
        {
          if ( ai.wImage == IDB_RADIO_FILLED )
          {
            selection = ai.wItemID;
            break;
          }
        }
      }
      switch ( selection )
      {
        case IDS_PBAP_SEARCH_NUMBER:
          searchAttrib = AEEBT_PBAP_SEARCH_ATTRIBUTE_NUMBER;
          break;
        case IDS_PBAP_SEARCH_SOUND:
          searchAttrib = AEEBT_PBAP_SEARCH_ATTRIBUTE_SOUND;
          break;
        case IDS_PBAP_SEARCH_NAME:
        default:
          searchAttrib = AEEBT_PBAP_SEARCH_ATTRIBUTE_NAME;
          break;
      }
      if ( searchAttrib != pMe->mPBAP.searchAttrib )
      {
        pMe->bConfigChanged = TRUE;
        pMe->mPBAP.searchAttrib = searchAttrib;
      }
      if ( key == AVK_LEFT )
      {
        BTApp_PBAPDoClientSettingsMenu( pMe, MOVE_LEFT );
      }
      else if ( key == AVK_RIGHT )
      {
        BTApp_PBAPDoClientSettingsMenu( pMe, MOVE_RIGHT );
      }
      else /* must be AVK_CLR */
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      // update menu
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->a.m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );

        switch ( sel )
        {
          case IDS_PBAP_SEARCH_NAME:
            sel = IDS_PBAP_SEARCH_NUMBER;
            break;
          case IDS_PBAP_SEARCH_NUMBER:
            sel = IDS_PBAP_SEARCH_SOUND;
            break;
          case IDS_PBAP_SEARCH_SOUND:
            sel = 0;
            break;
        }
      }
      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandlePBAPSortOrderMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPSortOrderMenu( CBTApp* pMe, uint16 key )
{
  boolean              ev_processed = FALSE;
  uint8                i;
  uint16               selection = 0;
  AEEBTPBAPOrder       sortOrder;
  CtlAddItem           ai;
  uint16               sel = IDS_PBAP_SORT_INDEX;


  MSG_MED( "HndlPBAPSortOrderMenu - k=0x%x", key, 0, 0 );
  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_CLR:
    {
      for ( i=IMENUCTL_GetItemCount( pMe->m_pIMenu ); i>0; i-- )
      {
        if ( IMENUCTL_GetItem( pMe->m_pIMenu, 
                               IMENUCTL_GetItemID( pMe->m_pIMenu, i-1 ), 
                               &ai ) != FALSE )
        {
          if ( ai.wImage == IDB_RADIO_FILLED )
          {
            selection = ai.wItemID;
            break;
          }
        }
      }
      switch ( selection )
      {
        case IDS_PBAP_SORT_ALPHA:
          sortOrder = AEEBT_PBAP_SORT_ORDER_ALPHANUMERIC;
          break;
        case IDS_PBAP_SORT_SOUND:
          sortOrder = AEEBT_PBAP_SORT_ORDER_PHONETIC;
          break;
        case IDS_PBAP_SORT_INDEX:
        default:
          sortOrder = AEEBT_PBAP_SORT_ORDER_INDEXED;
          break;
      }
      if ( sortOrder != pMe->mPBAP.sortOrder )
      {
        pMe->bConfigChanged = TRUE;
        pMe->mPBAP.sortOrder = sortOrder;
      }
      if ( key == AVK_LEFT )
      {
        BTApp_PBAPDoClientSettingsMenu( pMe, MOVE_LEFT );
      }
      else if ( key == AVK_RIGHT )
      {
        BTApp_PBAPDoClientSettingsMenu( pMe, MOVE_RIGHT );
      }
      else /* must be AVK_CLR */
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      // update menu
      while ( sel != 0 )
      {
        BTApp_UpdateMenuItemImage( 
          pMe->a.m_pIDisplay, pMe->m_pIMenu, sel, 
          (sel == selection) ? IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );

        switch ( sel )
        {
          case IDS_PBAP_SORT_INDEX:
            sel = IDS_PBAP_SORT_ALPHA;
            break;
          case IDS_PBAP_SORT_ALPHA:
            sel = IDS_PBAP_SORT_SOUND;
            break;
          case IDS_PBAP_SORT_SOUND:
            sel = 0;
            break;
        }
      }
      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_HandlePBAPvCardFormatMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPvCardFormatMenu( CBTApp* pMe, uint16 key )
{
  boolean              ev_processed = FALSE;
  uint8                i;
  uint16               selection = 0;
  AEEBTPBAPFormat      newFormat;
  CtlAddItem           ai;

  MSG_MED( "HndlvPBAPCardFormatMenu - k=0x%x", key, 0, 0 );
  switch ( key )
  {
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_CLR:
    {
      for ( i=IMENUCTL_GetItemCount( pMe->m_pIMenu ); i>0; i-- )
      {
        if ( IMENUCTL_GetItem( pMe->m_pIMenu, 
                               IMENUCTL_GetItemID( pMe->m_pIMenu, i-1 ), 
                               &ai ) != FALSE )
        {
          if ( ai.wImage == IDB_RADIO_FILLED )
          {
            selection = ai.wItemID;
            break;
          }
        }
      }
      switch ( selection )
      {
        case IDS_PBAP_VCARD_3_0:
          newFormat = AEEBT_PBAP_FORMAT_VCARD_3_0;
          break;
        case IDS_PBAP_VCARD_2_1:
        default:
          newFormat = AEEBT_PBAP_FORMAT_VCARD_2_1;
          break;
      }
      if ( newFormat != pMe->mPBAP.vCardFormat )
      {
        pMe->bConfigChanged = TRUE;
        pMe->mPBAP.vCardFormat = newFormat;
      }
      if ( key == AVK_LEFT )
      {
        BTApp_PBAPDoClientSettingsMenu( pMe, MOVE_LEFT );
      }
      else if ( key == AVK_RIGHT )
      {
        BTApp_PBAPDoClientSettingsMenu( pMe, MOVE_RIGHT );
      }
      else /* must be AVK_CLR */
      {
        ev_processed = BTApp_HandleClearKey( pMe );
      }
      break;
    }
    case AVK_SELECT:
    {
      selection = IMENUCTL_GetSel( pMe->m_pIMenu );
      // update menu
      BTApp_UpdateMenuItemImage( pMe->a.m_pIDisplay, pMe->m_pIMenu, 
        IDS_PBAP_VCARD_2_1, 
        (selection == IDS_PBAP_VCARD_2_1) ?
        IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
      BTApp_UpdateMenuItemImage( pMe->a.m_pIDisplay, pMe->m_pIMenu, 
        IDS_PBAP_VCARD_3_0, 
        (selection == IDS_PBAP_VCARD_3_0) ? 
        IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
      ev_processed = TRUE;
      break;
    }
  }
  return ev_processed;
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildAuthenticateMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildAuthenticateMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;
  uint16      sel = 0;

  MSG_LOW( "PBAPBuildAuthenticateMenu", 0, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );
  
  BTApp_InitAddItem( &ai );
  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_AUTHENTICATE, NULL );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUTH_NONE, 
    (pMe->mPBAP.authOptions == AEEBT_PBAP_AUTH_NONE) ?
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUTH_PWD, 
    (pMe->mPBAP.authOptions == AEEBT_PBAP_AUTH_PIN) ? 
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUTH_USERID_PWD, 
    (pMe->mPBAP.authOptions == AEEBT_PBAP_AUTH_USERID_AND_PIN) ? 
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );

  if ( pMe->mPBAP.authOptions == AEEBT_PBAP_AUTH_NONE )
  {
    sel = IDS_AUTH_NONE;
  }
  else if ( pMe->mPBAP.authOptions == AEEBT_PBAP_AUTH_PIN )
  {
    sel = IDS_AUTH_PWD;
  }
  else
  {
    sel = IDS_AUTH_USERID_PWD;
  }

  IMENUCTL_SetSel( pMe->m_pIMenu, sel );
  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_AUTHENTICATE );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );

}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildClientBrowseMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildClientBrowseMenu( CBTApp* pMe )
{
  AEERect     rc;
  uint8       uNoOfItems = 0;
  AECHAR      wName[ AEEBT_MAX_FILE_NAME+1 ];

  MSG_LOW( "PBAPBuildClientBrowseMenu", 0, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );
  STRTOWSTR( sClientPath, wName, sizeof(wName) );
  IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, wName );

  uNoOfItems = pMe->mPBAP.uNumOfObj;
  if ( uNoOfItems == 0 )
  {
    MSG_LOW( "PBAPBuildClientBrowseMenu - No items", 0, 0, 0 );
  }
  else
  {
    while ( uNoOfItems > 0 )
    {
     if( ( STRLEN( pMe->mPBAP.pbObjects[pMe->mPBAP.uNumOfObj -
                 uNoOfItems].szObjName) == 0 ) ||( pMe->mPBAP.pbObjects[pMe->mPBAP.uNumOfObj -
                 uNoOfItems].bFolder == TRUE ) )
     {
      STRTOWSTR( pMe->mPBAP.pbObjects[pMe->mPBAP.uNumOfObj -
                 uNoOfItems].szObjHdleName, wName, sizeof(wName) );
     }
     else
     {
       STRTOWSTR( pMe->mPBAP.pbObjects[pMe->mPBAP.uNumOfObj -
                 uNoOfItems].szObjName, wName, sizeof(wName) );
     }
      IMENUCTL_AddItem( pMe->m_pIMenu, NULL, 0, 
                        (pMe->mPBAP.uNumOfObj-uNoOfItems), wName, 0 );
      uNoOfItems--;
    }
  }
  if ( TOP_MENU != BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU )
  {
    PUSH_MENU( BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU );
  }
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildClientDownloadSubMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildClientDownloadSubMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;

  MSG_LOW( "PBAPBuildClientDownloadSubMenu", 0, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );

  BTApp_InitAddItem( &ai );
  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, 
                     IDS_PBAP_DOWNLOAD, NULL );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_DW_PB, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_DW_ICH, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_DW_OCH, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_DW_MCH, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_DW_CCH, 0 );
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif //FEATURE_BT_TEST_API

  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_CLIENT_DOWNLOAD_SUBMENU );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif  
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildClientDownloadMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildClientDownloadMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;

  MSG_LOW( "PBAPBuildClientDownloadMenu", 0, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );

  BTApp_InitAddItem( &ai );
  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, 
                     IDS_PBAP_SELECT_SOURCE, NULL );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_PHONE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SIM, 0 );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_CLIENT_DOWNLOAD );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL ); 
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildSearchAttributeMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildSearchAttributeMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;
  uint16      sel = 0;

  MSG_LOW( "PBAPBuildSearchAttributeMenu", 0, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );
  
  BTApp_InitAddItem( &ai );
  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, 
                     IDS_PBAP_SEARCH_ATTRIBUTE, NULL );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SEARCH_NAME, 
    (pMe->mPBAP.searchAttrib == AEEBT_PBAP_SEARCH_ATTRIBUTE_NAME) ?
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SEARCH_NUMBER, 
    (pMe->mPBAP.searchAttrib == AEEBT_PBAP_SEARCH_ATTRIBUTE_NUMBER) ?
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SEARCH_SOUND, 
    (pMe->mPBAP.searchAttrib == AEEBT_PBAP_SEARCH_ATTRIBUTE_SOUND) ?
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
    
  if ( pMe->mPBAP.searchAttrib == AEEBT_PBAP_SEARCH_ATTRIBUTE_NAME )
  {
    sel = IDS_PBAP_SEARCH_NAME;
  }
  else if ( pMe->mPBAP.searchAttrib == AEEBT_PBAP_SEARCH_ATTRIBUTE_NUMBER )
  {
    sel = IDS_PBAP_SEARCH_NUMBER;
  }
  else
  {
    sel = IDS_PBAP_SEARCH_SOUND;
  }
  IMENUCTL_SetSel( pMe->m_pIMenu, sel );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_SEARCH_ATTR );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildSortOrderMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildSortOrderMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;
  uint16      sel = 0;

  MSG_LOW( "PBAPBuildSortOrderMenu", 0, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );
  
  BTApp_InitAddItem( &ai );
  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_PBAP_SORTING, NULL );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SORT_INDEX, 
    (pMe->mPBAP.sortOrder == AEEBT_PBAP_SORT_ORDER_INDEXED) ?
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SORT_ALPHA, 
    (pMe->mPBAP.sortOrder == AEEBT_PBAP_SORT_ORDER_ALPHANUMERIC) ?
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SORT_SOUND, 
    (pMe->mPBAP.sortOrder == AEEBT_PBAP_SORT_ORDER_PHONETIC) ?
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
    
  if ( pMe->mPBAP.sortOrder == AEEBT_PBAP_SORT_ORDER_INDEXED )
  {
    sel = IDS_PBAP_SORT_INDEX;
  }
  else if ( pMe->mPBAP.sortOrder == AEEBT_PBAP_SORT_ORDER_ALPHANUMERIC )
  {
    sel = IDS_PBAP_SORT_ALPHA;
  }
  else
  {
    sel = IDS_PBAP_SORT_SOUND;
  }
  IMENUCTL_SetSel( pMe->m_pIMenu, sel );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_SORT_ORDER );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildvCardFormatMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildvCardFormatMenu( CBTApp* pMe )
{
  AEERect     rc;
  CtlAddItem  ai;
  uint16      sel = 0;

  MSG_LOW( "PBAPBuildvCardFormatMenu", 0, 0, 0 );

  IMENUCTL_Reset( pMe->m_pIMenu );
  SETAEERECT (&rc, 0, 0, pMe->m_rect.dx, pMe->m_rect.dy-20);
  IMENUCTL_SetRect( pMe->m_pIMenu, &rc );
  
  BTApp_InitAddItem( &ai );
  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_PBAP_VCARD_FORMAT, 
                     NULL );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_VCARD_2_1, 
    (pMe->mPBAP.vCardFormat == AEEBT_PBAP_FORMAT_VCARD_2_1) ?
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_VCARD_3_0, 
    (pMe->mPBAP.vCardFormat == AEEBT_PBAP_FORMAT_VCARD_3_0) ? 
    IDB_RADIO_FILLED : IDB_RADIO_UNFILLED );

  sel = (pMe->mPBAP.vCardFormat == AEEBT_PBAP_FORMAT_VCARD_2_1) ? 
          IDS_PBAP_VCARD_2_1 : IDS_PBAP_VCARD_3_0;

  IMENUCTL_SetSel( pMe->m_pIMenu, sel );
  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_VCARD_FORMAT );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_PBAPDoClientSettingsMenu
DESCRIPTION
============================================================================= */
void BTApp_PBAPDoClientSettingsMenu( CBTApp* pMe, int item )
{
  uint16 selection;
  int32 index;
  int count;
  
  selection = IMENUCTL_GetSel( pMe->m_pISoftMenu );
  IMENUCTL_GetItemData( pMe->m_pISoftMenu, selection, (uint32 *)&index );
  count = IMENUCTL_GetItemCount( pMe->m_pISoftMenu );
  index += item - (AVK_0 + 1);
  if (index < 0)
  {
    index = count - 1;
  }
  else if (index >= count)
  {
    index = 0;
  }
  MSG_MED( "PBAPDoSettingsMenu - item=%d curSel=%d", item, selection, 0 );
  
  selection = IMENUCTL_GetItemID( pMe->m_pISoftMenu, index );
  
  if ( item != 0)
  {
    (void)POP_MENU();
  }
  switch (selection)
  {
    case IDS_PBAP_VCARD_FORMAT:
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_VCARD_FORMAT );
      break;
    case IDS_PBAP_SORT_ORDER:
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_SORT_ORDER );
      break;
    case IDS_PBAP_SEARCH_ATTR:
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_SEARCH_ATTR );
      break;
    default:
      MSG_LOW( "PBAPDoSettingsMenu - invalid selection", 0, 0, 0 );
      break;
  }
   
  IMENUCTL_Redraw( pMe->m_pISoftMenu );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildClientSettingsOptionsMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildClientSettingsValuesMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  BTApp_InitAddItem( &ai );

  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE,
                     IDS_PBAP_SET_CONFIG_VALUES, NULL );

  // Add soft menu items
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_FILTER, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_MAX_LIST_COUNT, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_LIST_START_OFFSET, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SEARCH_VALUE, 0 );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_CLIENT_SETTINGS_VALUES );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );  
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildClientSettingsOptionsMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildClientSettingsOptionsMenu( CBTApp* pMe )
{
  CtlAddItem ai;
  AEERect    rc;

  IMENUCTL_Reset( pMe->m_pISoftMenu );
  SETAEERECT (&rc, 0, 160, pMe->m_rect.dx, pMe->m_rect.dy-160);
  IMENUCTL_SetRect( pMe->m_pISoftMenu, &rc );

  BTApp_InitAddItem( &ai );

  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE,
                     IDS_PBAP_SET_CONFIG_OPTIONS, NULL );

  // Add soft menu items
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_PBAP_VCARD_FORMAT, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_PBAP_SORT_ORDER, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pISoftMenu, &ai, IDS_PBAP_SEARCH_ATTR, 0 );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_CLIENT_SETTINGS_OPTIONS );
  IMENUCTL_SetActive( pMe->m_pISoftMenu, TRUE );
  BTApp_PBAPDoClientSettingsMenu( pMe, 0 );
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildClientSettingsMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildClientSettingsMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  BTApp_InitAddItem( &ai );

  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_SETTINGS, NULL );

  // Add soft menu items
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SET_CONFIG_OPTIONS, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SET_CONFIG_VALUES, 0 );
   // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUTHENTICATE, 0 );
  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_CLIENT_SETTINGS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildServerSettingsMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildServerSettingsMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  BTApp_InitAddItem( &ai );

  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_SETTINGS, NULL );

  // Add individual entries to the Menu
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_AUTHENTICATE, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_USERID, 0 );

  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_SERVER_SETTINGS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildClientMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildClientMenu(CBTApp * pMe)
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  if ( pMe->mPBAP.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->a.m_pIShell, BTAPP_RES_FILE, IDS_PBAP_CLIENT, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof( AECHAR ) );
    IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
  }
  else
  {
    IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_PBAP_CLIENT, NULL );
  }

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mPBAP.bCliRegistered == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REGISTER, 0 );
  }
  else
  {
    if ( pMe->mPBAP.bConnected == FALSE )
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEREGISTER, 0 );
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_CONNECT, 0 );
    }
    else
    {
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_DOWNLOAD, 0 );
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_BROWSE, 0 );
      BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DISCONNECT, 0 );    
    }
  }
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SETTINGS, 0 );  
  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_CLIENT );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION  
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildServerMenu
DESCRIPTION
============================================================================= */
static void BTApp_PBAPBuildServerMenu(CBTApp * pMe)
{
  CtlAddItem ai;
  char       szStatus[] = " -  ";
  uint8      len = 0;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect( pMe->m_pIMenu, &pMe->m_rect );

  // set the title
  if ( pMe->mPBAP.bConnected != FALSE )
  {
    szStatus[ 3 ] = 'C';
    ISHELL_LoadResString( pMe->a.m_pIShell, BTAPP_RES_FILE, IDS_PBAP_SERVER, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN * sizeof( AECHAR ) );
    len = WSTRLEN( pMe->pText2 );

    STRTOWSTR( szStatus, &pMe->pText2[ len ], 
               (SHORT_TEXT_BUF_LEN-len) * sizeof( AECHAR ) );
    IMENUCTL_SetTitle( pMe->m_pIMenu, NULL, NULL, pMe->pText2 );
  }
  else
  {
    IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_PBAP_SERVER, NULL );
  }

  BTApp_InitAddItem( &ai );

  // Add individual entries to the Menu
  if ( pMe->mPBAP.bSrvRegistered == FALSE )
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_REGISTER, 0 );
  }
  else
  {
    BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_DEREGISTER, 0 );
  }
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_SETTINGS, 0 );  

  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_SERVER );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
  IMENUCTL_SetSel( pMe->m_pIMenu, MENU_SEL );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION  
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildMainMenu
DESCRIPTION
============================================================================= */
void BTApp_PBAPBuildMainMenu( CBTApp* pMe )
{
  CtlAddItem ai;

  IMENUCTL_Reset( pMe->m_pIMenu );
  IMENUCTL_SetRect(pMe->m_pIMenu, &pMe->m_rect);  

  // set the title
  IMENUCTL_SetTitle( pMe->m_pIMenu, BTAPP_RES_FILE, IDS_PBAP_TESTS, NULL );

  BTApp_InitAddItem( &ai );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_SERVER, 0 );
  BTApp_AddMenuItem( pMe, pMe->m_pIMenu, &ai, IDS_PBAP_CLIENT, 0 );
 
  // Activate menu
  PUSH_MENU( BT_APP_MENU_PBAP_TESTS );
  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
  IDISPLAY_UpdateEx( pMe->a.m_pIDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_PBAPCleanup
DESCRIPTION
============================================================================= */
void BTApp_PBAPCleanup( CBTApp* pMe )
{
  if ( pMe->mPBAP.po != NULL )
  {
    IBTEXTPBAP_Release( pMe->mPBAP.po );
    pMe->mPBAP.po = NULL;
  }
  if ( pMe->mPBAP.pIFileMgr != NULL )
  {
    IFILEMGR_Release( pMe->mPBAP.pIFileMgr );
    pMe->mPBAP.pIFileMgr = NULL;
  }
  // unregister for PBAP notification
  ISHELL_RegisterNotify( pMe->a.m_pIShell,  
                         AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 
                         0 );
  uBTApp_NMask &= ~NMASK_BT_PBAP;
  ISHELL_RegisterNotify( pMe->a.m_pIShell,  
                         AEECLSID_BLUETOOTH_APP,
                         AEECLSID_BLUETOOTH_NOTIFIER, 
                         uBTApp_NMask );
}

/* ==========================================================================
FUNCTION BTApp_PBAPInit
DESCRIPTION
============================================================================= */
boolean BTApp_PBAPInit( CBTApp* pMe )
{
  static boolean init_done = FALSE;
  uint32 uNMask = NMASK_BT_PBAP | (((uint32)(NOTIFIER_VAL_ANY)) << 16);

  if ( init_done == FALSE )
  {
    if ( (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_PBAP, 
                                 (void**)&pMe->mPBAP.po ) == SUCCESS) &&
         (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, 
                                 (void**)&pMe->mPBAP.pIFileMgr ) == SUCCESS) &&
         (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_ADDRBOOK, 
                                 (void**)&pMe->mPBAP.pAddrbook ) == SUCCESS) &&
         (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_CALLHISTORY, 
                                 (void**)&pMe->mPBAP.pCallHist ) == SUCCESS) &&
         (ISHELL_RegisterNotify( pMe->a.m_pIShell,  AEECLSID_BLUETOOTH_APP,
                                 AEECLSID_BLUETOOTH_NOTIFIER, 
                                 uNMask ) == SUCCESS) )
    {
      uBTApp_NMask |= NMASK_BT_PBAP;
      init_done = TRUE;
    }

	if ( (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_CARD, 
									 (void**)&pICard ) != SUCCESS ) ||
			 (ISHELL_RegisterNotify( pMe->a.m_pIShell, AEECLSID_BLUETOOTH_APP,
									 AEECLSID_CARD_NOTIFIER, 
									 uNMaskCard ) != SUCCESS ) )
		{
		  MSG_LOW( "PBAPInit - ICard can't be initialised", 0, 0, 0 ); 
		}
  }
  return init_done;
}

/* ==========================================================================
FUNCTION BTApp_PBAPBuildMenu
DESCRIPTION
============================================================================= */
boolean BTApp_PBAPBuildMenu( CBTApp* pMe, BTAppMenuType menu )
{
  boolean built = TRUE;

  switch ( menu )
  {
    case BT_APP_MENU_PBAP_TESTS:
    {
      if ( BTApp_PBAPInit( pMe ) != FALSE )
      {
        BTApp_PBAPBuildMainMenu( pMe );
      }
      else
      {
        MSG_ERROR( "PBAPBuildMenu - failed to create PBAPobject", 0, 0, 0 );
        BTApp_PBAPCleanup( pMe );
        built = FALSE;
      }
      break;
    }
    case BT_APP_MENU_PBAP_SERVER:
    {
      BTApp_PBAPBuildServerMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_CLIENT:
    {
      BTApp_PBAPBuildClientMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_SERVER_SETTINGS:
    {
      BTApp_PBAPBuildServerSettingsMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS:
    {
      BTApp_PBAPBuildClientSettingsMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS_OPTIONS:
    {
      BTApp_PBAPBuildClientSettingsOptionsMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS_VALUES:
    {
      BTApp_PBAPBuildClientSettingsValuesMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_VCARD_FORMAT:
    {
      BTApp_PBAPBuildvCardFormatMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_FILTER:
    {
      PUSH_MENU( BT_APP_MENU_PBAP_FILTER );
      built = (ISHELL_CreateDialog( pMe->a.m_pIShell, BTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS );
      break;
    }
    case BT_APP_MENU_PBAP_MAX_LIST_COUNT:
    {
      PUSH_MENU( BT_APP_MENU_PBAP_MAX_LIST_COUNT );
      built = (ISHELL_CreateDialog( pMe->a.m_pIShell, BTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS );
      break;
    }
    case BT_APP_MENU_PBAP_LIST_START_OFFSET:
    {
      PUSH_MENU( BT_APP_MENU_PBAP_LIST_START_OFFSET );
      built = (ISHELL_CreateDialog( pMe->a.m_pIShell, BTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS );
      break;
    }
    case BT_APP_MENU_PBAP_SORT_ORDER:
    {
      BTApp_PBAPBuildSortOrderMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_SEARCH_ATTR:
    {
      BTApp_PBAPBuildSearchAttributeMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_SEARCH_VALUE:
    {
      PUSH_MENU( BT_APP_MENU_PBAP_SEARCH_VALUE );
      built = (ISHELL_CreateDialog( pMe->a.m_pIShell, BTAPP_RES_FILE, 
                                    IDD_BT_TEXT_EDIT, NULL ) == AEE_SUCCESS );
      break;
    }
    case BT_APP_MENU_PBAP_CLIENT_DOWNLOAD:
    {
      BTApp_PBAPBuildClientDownloadMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_CLIENT_DOWNLOAD_SUBMENU:
    {
      BTApp_PBAPBuildClientDownloadSubMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU:
    {
      BTApp_PBAPBuildClientBrowseMenu( pMe );
      break;
    }
    case BT_APP_MENU_PBAP_AUTHENTICATE:
    {
      BTApp_PBAPBuildAuthenticateMenu( pMe );
      break;
    }
    default:
      built = FALSE;
      break;
  }
  return built;
}

/* ==========================================================================
FUNCTION BTApp_PBAPHandleSelection
DESCRIPTION
============================================================================= */
static boolean BTApp_PBAPHandleSelection( CBTApp* pMe, uint16 sel )
{
  int     result = EFAILED;
  boolean key_handled = TRUE;
  boolean bRegistered = FALSE;

  switch ( sel )
  {
    case IDS_PBAP_SERVER:
    {
      BTApp_PBAPBuildServerMenu( pMe );
      break;
    }
    case IDS_PBAP_CLIENT:
    {
      BTApp_PBAPBuildClientMenu( pMe );
      break;
    }
    case IDS_SETTINGS:
    {
      if ( TOP_MENU == BT_APP_MENU_PBAP_SERVER )
      {
        BTApp_PBAPBuildServerSettingsMenu( pMe );
      }
      else if ( TOP_MENU == BT_APP_MENU_PBAP_CLIENT )
      {
        BTApp_PBAPBuildClientSettingsMenu( pMe );
      }
      break;
    }
    case IDS_REGISTER:
    {
      BTApp_SetBondable( pMe );
      if ( TOP_MENU == BT_APP_MENU_PBAP_SERVER )
      {
        if ( (result = IBTEXTPBAP_RegisterServer( pMe->mPBAP.po, szServerNamePBAP, 
                         AEEBT_PBAP_REPOSITORY_LOCAL_AND_SIM_CARD,
                         pMe->mPBAP.authOptions )) != SUCCESS )
        {
          MSG_ERROR( "PBAP_RegisterServer() failed with %x", result, 0, 0 );
          BTApp_ClearBondable( pMe ); // no need to be bondable anymore
          BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
        }
      }
      else if ( TOP_MENU == BT_APP_MENU_PBAP_CLIENT ) // client register
      {
        if ( pMe->mPBAP.bSrvRegistered != FALSE )
        {
          BTApp_ClearBondable( pMe ); // no need to be bondable anymore
          BTApp_ShowMessage( pMe, IDS_MSG_DEREG_SVR_FIRST, NULL, 3 );
          return key_handled; 
        }
        if ( (result = IBTEXTPBAP_RegisterClient( pMe->mPBAP.po, 
                              szClientNamePBAP )) != SUCCESS ) 
        {
          MSG_ERROR("PBAP_RegisterClient() failed with %x", result, 0, 0 );
          BTApp_ClearBondable( pMe ); // no need to be bondable anymore
          BTApp_ShowMessage( pMe, IDS_MSG_SVR_REG_FAILED, NULL, 3 );
        }
      }
      if ( result == SUCCESS )
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
      if ( TOP_MENU == BT_APP_MENU_PBAP_SERVER )
      {
        if ( (result = IBTEXTPBAP_DeregisterServer( pMe->mPBAP.po )) != SUCCESS )
        {
          MSG_ERROR( "PBAP_DeregisterServer() failed with %x", result, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
        }
      }
      else if ( TOP_MENU == BT_APP_MENU_PBAP_CLIENT ) // deregister client
      {
        if ( (result = IBTEXTPBAP_DeregisterClient( pMe->mPBAP.po )) != SUCCESS )
        {
          MSG_ERROR( "PBAP_DeregisterClient() failed with %x", result, 0, 0 );
          BTApp_ShowMessage( pMe, IDS_MSG_CLI_DEREG_FAILED, NULL, 3 );
        }
      }
      if ( result == SUCCESS )
      {
        bRegistered = pMe->mPBAP.bSrvRegistered;
        pMe->mPBAP.bSrvRegistered = FALSE;
        BTApp_CheckToClearDiscoverable( pMe );
        pMe->mPBAP.bSrvRegistered = bRegistered;
        BTApp_ShowBusyIcon( pMe ); // wait for command done
      }
      break;
    }
    case IDS_CONNECT:
    {
      key_handled = BTApp_BuildMenu( pMe, BT_APP_MENU_LIST_OBEX_SERVERS );
      break;
    }
    case IDS_DISCONNECT:
    {
      if ( IBTEXTPBAP_Disconnect( pMe->mPBAP.po ) != SUCCESS )
      {
        BTApp_ShowMessage( pMe, IDS_MSG_DISCONN_FAILED, NULL, 3 );
      }
      break;      
    }
    case IDS_AUTHENTICATE:
    {
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_AUTHENTICATE );
      break;
    }
    case IDS_USERID:
    {
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_USERID );
      break;    
    }
    case IDS_PBAP_SET_CONFIG_OPTIONS:
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_CLIENT_SETTINGS_OPTIONS );
      break;
    case IDS_PBAP_SET_CONFIG_VALUES:
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_CLIENT_SETTINGS_VALUES );
      break;
    case IDS_PBAP_VCARD_FORMAT:
    case IDS_PBAP_SORT_ORDER:
    case IDS_PBAP_SEARCH_ATTR:
      break;
    case IDS_PBAP_FILTER:
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_FILTER );
      break;
    case IDS_PBAP_MAX_LIST_COUNT:
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_MAX_LIST_COUNT );
      break;
    case IDS_PBAP_LIST_START_OFFSET:
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_LIST_START_OFFSET );
      break;
    case IDS_PBAP_SEARCH_VALUE:
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_SEARCH_VALUE );
      break;
    case IDS_PBAP_DOWNLOAD:
    {
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_CLIENT_DOWNLOAD );
      break;
    }
    case IDS_PBAP_PHONE:
    {
      pMe->mPBAP.pbRepository = AEEBT_PBAP_REPOSITORY_LOCAL;
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_CLIENT_DOWNLOAD_SUBMENU );
      break;
    }
    case IDS_PBAP_SIM:
    {
      pMe->mPBAP.pbRepository = AEEBT_PBAP_REPOSITORY_SIM_CARD;
      BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_CLIENT_DOWNLOAD_SUBMENU );
      break;
    }
    case IDS_PBAP_DW_PB:
    case IDS_PBAP_DW_ICH:
    case IDS_PBAP_DW_OCH:
    case IDS_PBAP_DW_MCH:
    case IDS_PBAP_DW_CCH:
    case IDS_PBAP_DW_INVALID:
      BTApp_PBAPClientDownloadReq( pMe, sel );
      break;
    case IDS_PBAP_BROWSE:
    {
      pMe->mPBAP.setPathFlag = AEEBT_PBAP_SET_PATH_TO_ROOT; 
      pMe->mPBAP.sSetFolder[0] = NULL;       
      result = IBTEXTPBAP_SetPhoneBook( pMe->mPBAP.po, NULL, 
                                        AEEBT_PBAP_SET_PATH_TO_ROOT );
      if ( result != SUCCESS )
      {
        MSG_LOW( "PBAPClientSetPBReq - SetPB failed", 0, 0, 0 );
        BTApp_ShowMessage( pMe, IDS_PBAP_MSG_SET_PB_FAILED, NULL, 2 );    
        return FALSE;
      }
      else
      {
        BTApp_ShowBusyIcon( pMe );
        return TRUE;
      }
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
FUNCTION BTApp_HandlePBAPSettingsMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPSettingsMenu(CBTApp * pMe, uint16 key)
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlPBAPSettings - k=0x%x", key, 0, 0 );

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
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
FUNCTION BTApp_HandlePBAPClientMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPClientMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlPBAPClient - k=0x%x", key, 0, 0 );

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
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
FUNCTION BTApp_HandlePBAPServerMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPServerMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlPBAPServer - k=0x%x", key, 0, 0 );

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
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
FUNCTION BTApp_HandlePBAPMainMenu
DESCRIPTION
============================================================================= */
static boolean BTApp_HandlePBAPMainMenu( CBTApp* pMe, uint16 key )
{
  boolean ev_processed = TRUE;
  uint16  selection;

  MSG_MED( "HndlPBAPMain - k=0x%x", key, 0, 0 );

  if ( (selection = BTApp_NumKey2Selection( pMe->m_pIMenu, key )) != 0 )
  {
    ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
  }
  else
  {
    switch ( key )
    {
      case AVK_SELECT:
      {
        selection = IMENUCTL_GetSel( pMe->m_pIMenu );
        ev_processed = BTApp_PBAPHandleSelection( pMe, selection );
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
FUNCTION BTApp_PBAPHandleMenus
DESCRIPTION
============================================================================= */
boolean BTApp_PBAPHandleMenus( CBTApp* pMe, uint16 key, BTAppMenuType menu )
{
  boolean handled = TRUE;
  
  switch ( menu )
  {
    case BT_APP_MENU_PBAP_TESTS:
      handled = BTApp_HandlePBAPMainMenu( pMe, key );
      break;
    case BT_APP_MENU_PBAP_SERVER:
      handled = BTApp_HandlePBAPServerMenu( pMe, key );
      break;
    case BT_APP_MENU_PBAP_CLIENT:
      handled = BTApp_HandlePBAPClientMenu( pMe, key );
      break;
    case BT_APP_MENU_PBAP_SERVER_SETTINGS:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS_OPTIONS:
    case BT_APP_MENU_PBAP_CLIENT_SETTINGS_VALUES:
      handled = BTApp_HandlePBAPSettingsMenu( pMe, key );
      break;
    case BT_APP_MENU_PBAP_VCARD_FORMAT:
      handled = BTApp_HandlePBAPvCardFormatMenu( pMe, key );
      break;
    case BT_APP_MENU_PBAP_SORT_ORDER:
      handled = BTApp_HandlePBAPSortOrderMenu( pMe, key );
      break;
    case BT_APP_MENU_PBAP_SEARCH_ATTR:
      handled = BTApp_HandlePBAPSearchAttributeMenu( pMe, key );
      break;
    case BT_APP_MENU_PBAP_CLIENT_DOWNLOAD:
      handled = BTApp_HandlePBAPClientDownloadMenu( pMe, key );
      break;
    case BT_APP_MENU_PBAP_CLIENT_DOWNLOAD_SUBMENU:
      handled = BTApp_HandlePBAPClientDownloadSubMenu( pMe, key );
      break;
    case BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU:
      handled = BTApp_HandlePBAPClientBrowseMenu( pMe, key );
      break;
    case BT_APP_MENU_PBAP_AUTHENTICATE:
      handled = BTApp_HandlePBAPAuthenticateMenu( pMe, key );
      break;
    default:
      MSG_ERROR(" PBAP: unexpected menu type %x", menu, 0, 0 );
      handled = FALSE;
      break;
  }
  return handled;
}

/* ==========================================================================
FUNCTION BTApp_PBAPHandleUserEvents
DESCRIPTION
============================================================================= */
void BTApp_PBAPHandleUserEvents( CBTApp* pMe, uint32 dwParam )
{
  char*       pTemp = NULL;
  int         szCount;
  char        sNoOfItems[5];
  AECHAR      wNoOfItems[5];  
  
  switch ( dwParam )
  {
    case EVT_PBAP_SRV_REG:
    {
      pMe->mPBAP.bSrvRegistered = TRUE;
      pMe->mPBAP.bIsServer = TRUE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif      
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
    }
    case EVT_PBAP_SRV_DEREG:
    {
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_DONE, NULL, 2 );
      pMe->mPBAP.bSrvRegistered = FALSE;
      pMe->mPBAP.bIsServer = FALSE;
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION      
      break;
    }
    case EVT_PBAP_CLI_REG:
    {
      BTApp_ShowMessage( pMe, IDS_MSG_CLI_REG_DONE, NULL, 2 );
      pMe->mPBAP.bCliRegistered = TRUE;
      pMe->mPBAP.bIsServer = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif            
      break;
    }
    case EVT_PBAP_CLI_DEREG:
    {
      BTApp_ShowMessage( pMe, IDS_MSG_CLI_DEREG_DONE, NULL, 2 );
      pMe->mPBAP.bCliRegistered = FALSE;
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      break;
    }
    case EVT_PBAP_SRV_REG_FAILED:
    {
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
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION            
      break;
    }
    case EVT_PBAP_SRV_DEREG_FAILED:
    {
      BTApp_ShowMessage( pMe, IDS_MSG_SVR_DEREG_FAILED, NULL, 3 );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      break;
    }
    case EVT_PBAP_CLI_REG_FAILED:
    {
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif            
      BTApp_ShowMessage( pMe, IDS_MSG_CLI_REG_FAILED, NULL, 3 );
      break;
    }
    case EVT_PBAP_CLI_DEREG_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif            
      BTApp_ShowMessage( pMe, IDS_MSG_CLI_DEREG_FAILED, NULL, 3 );
      break;
    }
    case EVT_PBAP_CONN_REQ:
    {
      if ( pMe->mPBAP.wCliUserID[0] != NULL )
      {
        if ( WSTRCMP(pMe->mPBAP.wCliUserID, pMe->mPBAP.wUserID) != 0 )
        {
          IBTEXTPBAP_AcceptConnection( pMe->mPBAP.po, FALSE );
          MSG_HIGH( "PBAP CON REQ EVT - UserID does not match", 0, 0, 0 );
          break;
        }
      }
      IBTEXTPBAP_AcceptConnection( pMe->mPBAP.po, TRUE );
      BTAppMgr_UpdateProfileType( pMe, &pMe->mPBAP.remoteBDAddr, BTAPP_MGR_PBAP, BTAPP_MGR_CONNECTED );
      if ( pMe->mPBAP.bAuthRequested != FALSE )
      {
        pMe->mPBAP.bAuthRequested = FALSE;
        BTApp_BuildTopMenu( pMe );
      }
      break;
    }
    case EVT_PBAP_CONNECTED:
    {
      pMe->mPBAP.bConnected = TRUE;
      STRLCPY( sClientPath, AEEBT_PBAP_FOLDER_NAME_ROOT, sizeof( sClientPath ) );
      STRLCPY( sServerPath, AEEBT_PBAP_FOLDER_NAME_ROOT, sizeof( sServerPath ) );
      if ( pMe->mPBAP.bAuthRequested != FALSE )
      {
        pMe->mPBAP.bAuthRequested = FALSE;
      }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION      
      BTApp_BuildTopMenu( pMe ); // rebuild menu to show 'C'
      break;
    }
    case EVT_PBAP_CONN_FAILED:
    {
      pMe->mPBAP.bConnected = FALSE;
      BTApp_ClearBondable( pMe ); // no need to be bondable anymore
      /* reset userid and passwd */
      pMe->mPBAP.wUserID[0] = 0;  pMe->mPBAP.wUserID[1] = 0;
      pMe->mPBAP.wPassword[0] = 0;  pMe->mPBAP.wPassword[1] = 0;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION      
      BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
      break;
    }
    case EVT_PBAP_DISCONNECTED:
    {
      pMe->mPBAP.bConnected = FALSE;
      STRLCPY( sClientPath, AEEBT_PBAP_FOLDER_NAME_ROOT, sizeof( sClientPath ) );
      STRLCPY( sServerPath, AEEBT_PBAP_FOLDER_NAME_ROOT, sizeof( sServerPath ) );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      if ( (TOP_MENU == BT_APP_MENU_PBAP_USERID) || 
           (TOP_MENU == BT_APP_MENU_PBAP_PASSWORD) )
      {
        /* this could be because of failure in authentication, end dialog */
        if ( ISHELL_EndDialog( pMe->a.m_pIShell ) == EFAILED )
        {
          MSG_ERROR( "TextEditSave - ISHELL_EndDialog() failed", 0, 0, 0 );
        }
        /* reset userid and passwd */
        pMe->mPBAP.wUserID[0] = 0;  pMe->mPBAP.wUserID[1] = 0;
        pMe->mPBAP.wPassword[0] = 0;  pMe->mPBAP.wPassword[1] = 0;
      }
      BTApp_BuildTopMenu( pMe ); // rebuild menu to hide 'C'
      break;
    }
    case EVT_PBAP_PULL_PB_REQ:
    {
      BTApp_PBAPServerDownloadRsp( pMe );
      break;
    }
    case EVT_PBAP_PULL_PB_DONE:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      // Was this a request to get phonebook size?
      if ( pMe->mPBAP.PBAPObject.maxListCount == 0 ) 
      {
        MSG_HIGH( "PBAPHandleUserEvents - phonebook size = %d", 
                  pMe->mPBAP.PBAPObject.phoneBookSize, 0, 0 );
        SNPRINTF( sNoOfItems, sizeof( sNoOfItems ), "%d", 
                  pMe->mPBAP.PBAPObject.phoneBookSize );
        STRTOWSTR( sNoOfItems, wNoOfItems, sizeof(wNoOfItems) );
        BTApp_ShowMessage( pMe, IDS_PBAP_MSG_NO_OF_PB_OBJS, wNoOfItems, 3 );
      }
      else
      {
        BTApp_ShowMessage( pMe, IDS_PBAP_MSG_PULL_PB_DONE, NULL, 3 );
      }
      break;
    }
    case EVT_PBAP_SET_PB_REQ:
    {
      BTApp_PBAPServerSetPBRsp( pMe );
      break;
    }
    case EVT_PBAP_SET_PB_DONE:
    {
      if ( pMe->mPBAP.setPathFlag == AEEBT_PBAP_SET_PATH_TO_FOLDER )
      {
        /* copy the folder name to path */
        STRLCAT( sClientPath, "/", sizeof( sClientPath ) );
        STRLCAT( sClientPath, pMe->mPBAP.sSetFolder, sizeof( sClientPath ) );
      }
      else if ( pMe->mPBAP.setPathFlag == AEEBT_PBAP_SET_PATH_TO_PARENT )
      {
        if ( STRCMP( sClientPath, AEEBT_PBAP_FOLDER_NAME_ROOT ) != 0 )
        {
          pTemp = STRRCHR( sClientPath, '/' );
          if ( pTemp != NULL )
          {
            pTemp[0] = NULL;
          }
        }
      }
      else if ( pMe->mPBAP.setPathFlag == AEEBT_PBAP_SET_PATH_TO_ROOT )
      {
        STRLCPY( sClientPath, AEEBT_PBAP_FOLDER_NAME_ROOT, 
                 sizeof( sClientPath ) );
      }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      /* pull the vcard listing of the current folder if we are in one
         of the leaf nodes in the virtual folder hierarchy */
      if ( !STRCMP(pMe->mPBAP.sSetFolder, AEEBT_PBAP_FOLDER_NAME_PB) || 
           !STRCMP(pMe->mPBAP.sSetFolder, AEEBT_PBAP_FOLDER_NAME_ICH) || 
           !STRCMP(pMe->mPBAP.sSetFolder, AEEBT_PBAP_FOLDER_NAME_OCH) ||
           !STRCMP(pMe->mPBAP.sSetFolder, AEEBT_PBAP_FOLDER_NAME_MCH) ||
           !STRCMP(pMe->mPBAP.sSetFolder, AEEBT_PBAP_FOLDER_NAME_CCH) ) 
      {
        MSG_HIGH( "PBAPHandleUserEvents - Sending PullvCardListing, pb = %s", 
                  pMe->mPBAP.sSetFolder, 0, 0 );
        BTApp_PBAPClientPullVCardListing( pMe );
      }
      else
      {
        if ( !STRCMP( sClientPath, "root" ) )
        {  
          /* root folder */
          pMe->mPBAP.uNumOfObj = 2;

          STRLCPY( pMe->mPBAP.pbObjects[ 0 ].szObjHdleName, 
                   AEEBT_PBAP_FOLDER_NAME_TELECOM, 
                   sizeof( pMe->mPBAP.pbObjects[ 0 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 0 ].bFolder = TRUE;

          STRLCPY( pMe->mPBAP.pbObjects[ 1 ].szObjHdleName, 
                   AEEBT_PBAP_FOLDER_NAME_SIM1, 
                   sizeof( pMe->mPBAP.pbObjects[ 1 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 1 ].bFolder = TRUE;
        }
        else if ( !STRCMP( sClientPath, "root/SIM1" ) )
        {  
          /* SIM1 */
          pMe->mPBAP.uNumOfObj = 1;

          STRLCPY( pMe->mPBAP.pbObjects[ 0 ].szObjHdleName, 
                   AEEBT_PBAP_FOLDER_NAME_TELECOM, 
                   sizeof( pMe->mPBAP.pbObjects[ 0 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 0 ].bFolder = TRUE;
        }
        else if ( !STRCMP( sClientPath, "root/telecom" ) ||
                  !STRCMP( sClientPath, "root/SIM1/telecom" ) ) 
        {
          /* telecom */
          pMe->mPBAP.uNumOfObj = 10;

          STRLCPY( pMe->mPBAP.pbObjects[ 0 ].szObjHdleName, 
                   AEEBT_PBAP_FOLDER_NAME_PB, 
                   sizeof( pMe->mPBAP.pbObjects[ 0 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 0 ].bFolder = TRUE;

          STRLCPY( pMe->mPBAP.pbObjects[ 1 ].szObjHdleName, 
                   AEEBT_PBAP_FOLDER_NAME_ICH, 
                   sizeof( pMe->mPBAP.pbObjects[ 1 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 1 ].bFolder = TRUE;

          STRLCPY( pMe->mPBAP.pbObjects[ 2 ].szObjHdleName, 
                   AEEBT_PBAP_FOLDER_NAME_OCH, 
                   sizeof( pMe->mPBAP.pbObjects[ 2 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 2 ].bFolder = TRUE;

          STRLCPY( pMe->mPBAP.pbObjects[ 3 ].szObjHdleName, 
                   AEEBT_PBAP_FOLDER_NAME_MCH, 
                   sizeof( pMe->mPBAP.pbObjects[ 3 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 3 ].bFolder = TRUE;

          STRLCPY( pMe->mPBAP.pbObjects[ 4 ].szObjHdleName, 
                   AEEBT_PBAP_FOLDER_NAME_CCH, 
                   sizeof( pMe->mPBAP.pbObjects[ 4 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 4 ].bFolder = TRUE;

          /* following entries are to test vcard listing one level above 
          the leaf node */
          STRLCPY( pMe->mPBAP.pbObjects[ 5 ].szObjHdleName, "pb.vcf", 
                   sizeof( pMe->mPBAP.pbObjects[ 5 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 5 ].bFolder = TRUE;
          STRLCPY( pMe->mPBAP.pbObjects[ 6 ].szObjHdleName, "ich.vcf",
                   sizeof( pMe->mPBAP.pbObjects[ 6 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 6 ].bFolder = TRUE;
          STRLCPY( pMe->mPBAP.pbObjects[ 7 ].szObjHdleName, "och.vcf", 
                   sizeof( pMe->mPBAP.pbObjects[ 7 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 7 ].bFolder = TRUE;
          STRLCPY( pMe->mPBAP.pbObjects[ 8 ].szObjHdleName, "mch.vcf", 
                   sizeof( pMe->mPBAP.pbObjects[ 8 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 8 ].bFolder = TRUE;
          STRLCPY( pMe->mPBAP.pbObjects[ 9 ].szObjHdleName, "cch.vcf", 
                   sizeof( pMe->mPBAP.pbObjects[ 9 ].szObjHdleName ) );
          pMe->mPBAP.pbObjects[ 9 ].bFolder = TRUE;
        }

        /* build client browse menu */
        BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU );

      }
      break;
    }
    case EVT_PBAP_PULL_VCARD_LISTING_REQ:
    {
      BTApp_PBAPServerPullVCardListingRsp( pMe );
      break;
    }
    case EVT_PBAP_PULL_VCARD_LISTING_DONE:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      if ( pMe->mPBAP.bListOneLevelAbove != FALSE )
      {
        /* In case of listing above leaf level */
        pMe->mPBAP.bListOneLevelAbove = FALSE;
        if ( pMe->mPBAP.PBAPObject.maxListCount == 0 ) 
        {
          SNPRINTF( sNoOfItems, sizeof( sNoOfItems ), "%d", 
                    pMe->mPBAP.PBAPObject.phoneBookSize );
          STRTOWSTR( sNoOfItems, wNoOfItems, sizeof(wNoOfItems) );
          BTApp_ShowMessage( pMe, IDS_PBAP_MSG_MAX_LIST_ZERO, wNoOfItems, 3 );
        }
        else
        {
          BTApp_ShowMessage( pMe, IDS_PBAP_MSG_PULL_VCARD_LIST_DONE, NULL, 3 );
        }
      }
      else if ( pMe->mPBAP.PBAPObject.maxListCount == 0 ) 
      {
        MSG_HIGH( "PBAPClientBuildVCardListingMenu - phonebook size = %d", 
                  pMe->mPBAP.PBAPObject.phoneBookSize, 0, 0 );
        /* Initialize the required members */
        pMe->mPBAP.uNumOfObj = 0;
        for( szCount=0; szCount<MAX_PBAP_OBJECTS; szCount++ )
        {
          MEMSET( pMe->mPBAP.pbObjects[ szCount ].szObjHdleName, 0, 
                  MAX_HANDLE_NAME_LEN+1 );
          MEMSET( pMe->mPBAP.pbObjects[ szCount ].szObjName, 0,
                  MAX_HANDLE_NAME_LEN+1 );
        }
        /* build empty menu items */
        BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU );
        SNPRINTF( sNoOfItems, sizeof( sNoOfItems ), "%d", 
                  pMe->mPBAP.PBAPObject.phoneBookSize );
        STRTOWSTR( sNoOfItems, wNoOfItems, sizeof(wNoOfItems) );
        BTApp_ShowMessage( pMe, IDS_PBAP_MSG_MAX_LIST_ZERO, wNoOfItems, 3 );
      }
      else
      {
        if ( BTApp_PBAPClientBuildVCardListingMenu( pMe ) != FALSE )
        {
          BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_CLIENT_BROWSE_MENU );
        }
      }
      break;
    }
    case EVT_PBAP_PULL_VCARD_ENTRY_REQ:
    {
      BTApp_PBAPServerPullVCardEntryRsp( pMe );
      break;
    }
    case EVT_PBAP_PULL_VCARD_ENTRY_DONE:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      BTApp_ShowMessage( pMe, IDS_PBAP_MSG_VOBJ_PULLED, NULL, 2 );
      break;
    }
    case EVT_PBAP_PULL_PB_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      BTApp_ShowMessage( pMe, IDS_PBAP_MSG_PULL_PB_FAILED, NULL, 2 );
      break;
    }
    case EVT_PBAP_SET_PB_FAILED:
    {
      BTApp_ShowMessage( pMe, IDS_PBAP_MSG_SET_PB_FAILED, NULL, 2 );
      break;
    }
    case EVT_PBAP_PULL_VCARD_LISTING_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      BTApp_ShowMessage( pMe, IDS_PBAP_MSG_PULL_VCARD_LIST_FAIL, NULL, 2 );
      break;
    }
    case EVT_PBAP_PULL_VCARD_ENTRY_FAILED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      BTApp_ShowMessage( pMe, IDS_PBAP_MSG_PULL_VOBJ_FAILED, NULL, 2 );
      break;
    }
    case EVT_PBAP_AUTH_REQ:
    {
      pMe->mPBAP.bAuthRequested = TRUE;
      if ( (pMe->mPBAP.bUserIDRequired != FALSE) && 
           (pMe->mPBAP.bCliRegistered != FALSE) )
      {
        BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_USERID );
      }
      else
      {
        BTApp_BuildMenu( pMe, BT_APP_MENU_PBAP_PASSWORD );
      }
      break;    
    }
    case EVT_PBAP_OP_ABORTED:
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      MSG_LOW( "PBAPHndlUserEvts - Operation Aborted", 0, 0, 0 );
      break;
    }
    default:
      MSG_ERROR(" PBAP: unexpected user event %x", dwParam, 0, 0 );
      break;
  }
}

/* ==========================================================================
FUNCTION BTApp_PBAPConnect
DESCRIPTION
============================================================================= */
void BTApp_PBAPConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr )
{

  BTApp_SetBondable( pMe );

  pMe->mPBAP.remoteBDAddr = *pBDAddr ; 
  if ( IBTEXTPBAP_Connect( pMe->mPBAP.po, pBDAddr, 0,
                           pMe->mPBAP.bDoAuthenticate) != SUCCESS )
  {
    BTApp_ClearBondable( pMe ); // no need to be bondable anymore
    BTApp_ShowMessage( pMe, IDS_MSG_CONN_FAILED, NULL, 3 );
  }
  else
  {
    BTApp_ShowBusyIcon( pMe ); // wait for connect confirm
  }
}
/* ==========================================================================
FUNCTION BTApp_EnablePBAP
DESCRIPTION
============================================================================= */
boolean BTApp_EnablePBAP( 
  CBTApp*  pMe, 
  boolean* pbSettingBondable, 
  boolean* pbSettingDiscoverable )
{
  boolean bPBAPEnabled = FALSE;
  
  if( pMe->mPBAP.bServerEnable == TRUE )
  {
    if( pMe->mPBAP.bSecSelected !=FALSE )
    {
      IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                               AEEBT_SD_SERVICE_CLASS_PBAP_PSE,
                               pMe->mPBAP.srvSecType,
                               pMe->mPBAP.bAuthorize,
                               pMe->mPBAP.bAuthorizeFirst );
      IBTEXTRM_SetSecBySvcCls( pMe->mRM.po, 
                               AEEBT_SD_SERVICE_CLASS_PBAP_PCE,
                               pMe->mPBAP.srvSecType,
                               pMe->mPBAP.bAuthorize,
                               pMe->mPBAP.bAuthorizeFirst );
    }
    if( BTApp_PBAPInit( pMe ) == FALSE )
    {
      MSG_ERROR( "EnablePBAP - failed to create PBAP object", 0, 0, 0 );
      BTApp_PBAPCleanup( pMe );
    }
    else if ( pMe->mPBAP.bSrvRegistered == FALSE )
    {
      if ( *pbSettingBondable == FALSE )
      {
        BTApp_SetBondable( pMe );
        *pbSettingBondable = TRUE;
      }
      MSG_LOW( "EnablePBAP - enabling PBAP", 0, 0, 0 );
      if ( IBTEXTPBAP_RegisterServer( pMe->mPBAP.po, szServerNamePBAP,
                                      AEEBT_PBAP_REPOSITORY_LOCAL_AND_SIM_CARD,
                                      pMe->mPBAP.authOptions ) != SUCCESS )
      {
        BTApp_ClearBondable( pMe ); // no need to be bondable anymore
        MSG_ERROR( "EnablePBAP - enabling PBAP failed", 0, 0, 0 );
      }
      else
      {
        if ( *pbSettingDiscoverable == FALSE )
        {
          IBTEXTSD_SetDiscoverable( pMe->mSD.po, TRUE );
          *pbSettingDiscoverable = TRUE;
        }
        BTApp_ShowBusyIcon( pMe );
        bPBAPEnabled = TRUE;
      }
    }
  }
  return bPBAPEnabled;
}
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#endif /* FEATURE_BT_EXTPF_PBAP */

#endif /* FEATURE_APP_BLUETOOTH */


/*=============================================================================

FILE: DialerUtils.c

SERVICES: Dialer Applet

GENERAL DESCRIPTION: Misc functions

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2007 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerUtils.c#108 $
$DateTime: 2008/04/16 08:03:39 $
$Author: cmedrano $
$Change: 643522 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what: where, why
--------   ---     ---------------------------------------------------------
04/16/08   clm     Fix double free error.
08/02/07   cvs     Featurize all secondary display support
06/19/07   alb     Display the alpha in the incoming call screen.
05/18/07   jks     Fixed access to dereferenced pointer that was causing 
                   dialog stack problems
01/10/07   jks     Added support for remote ring (ringback) for VoIP calls
12/15/06   alb     Changed prototype of ISUPPSVC_StartSS to indicate whether
                   FDN check is required or not.
07/12/06   cvs     Fix compile warnings
05/22/06   alb     Originate partial SS numbers as a voice call (87047).
02/17/06   jks     Added CEPT sounds
10/14/05   cvs     support for attaching carkit display mid call
10/12/05   cvs     Carkit display
08/26/05   AT      Added check for Emergency Number before SS check.
07/19/05   AT      Fix issue when one-touch dialed numbers exceed 20 digits.
07/11/05   aps     Added PI field lookup when flashed with info frm BS.
06/28/05   ak      Added support for FDN verification
06/07/05   sun     Added support for CCBS
05/11/05   SUN     Fixed Double Free and Memory Leak
05/11/05   AT      Adding flag when an origination is pending.
03/01/05   sun     Initialized CallNum to 1 in OriginateVoiceCall
01/21/05   cvs     Rearchitect uisnd_set_device()
12/10/04   SUN     Added AppBeinSuspended flag - Dialer misses events between 
                   the time it tries to close itself and when it actually closes
                   This flag helps to capture all those events 
11/18/04   SUN     Use OEMNV #defines for DTMF
09/28/04   JAS     Added case for missed calls on secondary display
09/20/04   SUN     Fixed Mute and Vocoder settings
09/16/04   SUN     Fixed SS Strings
09/03/04   SUN     Do not send DTMFs for USSD Dialogs
07/26/04   JAS     Added support for secondary display
07/18/04   SUN     Send DTMF for only one active call
07/08/04   SUN     Remove Notif Dlg For Incoming Call 
06/24/04   SUN     Start Default Dialog on OriginateVoice
                   Do not send extra DTMFs 
05/25/04   SUN     Only one Start/Stop DTMF should be sent at a time
05/20/04   SUN     Handle Progress Info Event
04/09/04   SUN     Fixed Lint Errors
04/05/04   BPW     Alert changes for failed calls
03/19/04   sun     Added changes for DTMF
02/27/04   BPW     Modified what string is displayed for a call
02/10/04   BPW     Return TRUE after a dialog is updated in the stack
01/30/04   BPW     Fixed speed dial 
01/30/04   BPW     Removed check for status dialog in create dlg
01/29/04   BPW     Fixed lint error
01/22/04   BPW     Optimized NV gets
01/21/04   BPW     Added ISuppSvc API changes & status dlg startup check
01/20/03   BPW     DTMF and other memory cleanup and lint errors
01/16/04   BPW     Fixed when the dlg ID to be created is saved
01/09/04   BPW     Fixed OTASP calls behaving like partial SS strings
01/08/04   BPW     Use own dialer hyphenation function
01/07/03   BPW     Changed how dialogs startup after being suspended
12/19/03   BPW     Modified Dialog Priorities
12/18/03   BPW     Added more SS feedback for requesting and incomplete
12/18/03   BPW     Added base station signalling support
12/10/03   BPW     Added string to display for EBADSTATE
12/09/03   BPW     Removed unused variables and error checks
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/26/03   SUN     Added Support for IALERT
11/25/03   BPW     Added code review changes and comments
11/25/03   BPW     Linted for the Saber 6250 build
11/20/03   BPW     Send DTMFs if USER_RESUMED option is enabled
11/20/03   BPW     Added displays for OTASP calls
11/19/03   BPW     Addded refresh of active dialog after text control clear
11/13/03   BPW     Added function to format display for phone number
11/13/03   BPW     Press and hold clear to clear contents of text control
11/13/03   BPW     Made auto answer timer based
10/21/03   SUN     Fixed Compile Errors
10/17/03   SUN     Reversed the order of string checks.
10/16/03   RAM     Change in OriginateVoiceCall for call-dependant SuppSvc.
08/29/03   BW      Fixed compile warnings
08/28/03   BW      Moved ErrorToIDS funtion to DlgManager
08/22/03   BW      Modified CDialerApp_PopDlg function
08/19/03   BW      Added messages, param checking, and more code clean up
08/19/03   SUN     Changed AEESYS to AEESUPPSYS
08/14/03   BW      Integration with both ICM and ISuppSvc
07/25/03   BW      Created DialerUtils Source File

=============================================================================*/
//lint -save -e611
//lint -save -e740

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"
#include "AEEAppGen.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEText.h"
#include "AEE.h"
#include "AEE_OEM.h"
#include "AppComFunc.h"
#include "AEECM.h"
#include "DialerUtils.h"
#include "DialerNotif.h"
#include "DialerApp.h"
#include "dialerapp_res.h"
#include "uiutils.h"
#include "AEEAlert.h"
#include "disp.h"
#include "OEMCFGI.h"
#include "AEEAddrBookExt.h"

#ifdef FEATURE_USB_CARKIT
#include "usbcc.h"
#endif

/*===========================================================================

LOCAL/STATIC DATA

===========================================================================*/
// priority struct for dialogs
typedef struct _CDlgPriority
{
  uint16 wDlgID;
  uint16 wPriority;
  boolean bDuplicates;
} CDlgPriority;

// priority structure for dialogs
// 1 is highest priority
static const CDlgPriority DlgPriority[] = {
// Dialog ID                 Priority        Duplicates Can Exist
  {0,                        65535,          FALSE},
  {IDD_NUMEDIT,              2,              FALSE},
  {IDD_CALLING,              7,              FALSE},
  {IDD_SERVICE_PROGRAMMING,  100,            FALSE},
  {IDD_CALL_FORWARDING,      100,            FALSE},
  {IDD_SUPS_OPERATION,       100,            FALSE},
  {IDD_CF_REGISTER,          100,            FALSE},
  {IDD_SERVICE_GROUP,        100,            FALSE},
  {IDD_USSD_PASSWORD,        6,              TRUE },
  {IDD_CFNR_TIMER,           100,            FALSE}, 
  {IDD_USSD_INFO,            6,              TRUE }, 
  {IDD_CALL_BARRING,         100,            FALSE},
  {IDD_CB_PASSWORD,          100,            FALSE},
  {IDD_CALLER_IDENTITY,      100,            FALSE},
  {IDD_CHANGE_PASSWORD,      100,            FALSE},
  {IDD_WILDCARD,             100,            FALSE},
  {IDD_NOTIF,                1,              FALSE},
  {IDD_INCOMING,             5,              FALSE},
  {IDD_STATUS,               65535,          FALSE},
  {IDD_INCOM_OPTIONS,        4,              FALSE},
  {IDD_CALL_OPTIONS,         100,            FALSE},
  {IDD_MISSEDCALL,           8,              FALSE},
  {IDD_DETAIL,               100,            FALSE},
  {IDD_USSD_PROMPT,          6,              TRUE },
  {IDD_TIMEPAUSE,            100,            FALSE},
  {IDD_FORWARDING,           3,              FALSE},
  {IDD_SSREQUEST,            7,              FALSE},
  {IDD_CCBS_SUPS_OPERATION,  100,            FALSE},
  {IDD_RECALL,               7,              FALSE},
  {IDD_SETUP_RECALL,         7,              FALSE},
  {IDD_CCBS_INDEX,           100,            FALSE},
  {IDD_CCBS_PENDING_LIST,    100,            FALSE}};

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/
//static void    CDialerApp_PrintDlgStack(void *pUser);
static boolean CDialerApp_DisplayStatusDlgRemove(CDialerApp *pMe, uint32 wPos);

/*=============================================================================
FUNCTION: CDialerApp_CheckAEEReturnStatus

DESCRIPTION: Checks the return status and tries to print a message if false

PARAMETERS:
  *pUser: CDialerApp object pointer
  nStatus: status value to check

RETURN VALUE:
  boolean: Returns TRUE if status value was SUCCESS and FALSE otherwise

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CheckAEEReturnStatus(void *pUser, int nStatus)
{
  CDialerApp *pMe = (CDialerApp*) pUser; 

  PRINT_FUNCTION_NAME();
  if(pMe == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  if(nStatus == SUCCESS)
  {
    return TRUE;
  }
  else
  {
    uint16 wIDS;
    AECHAR *pszError = NULL;
    AECHAR *pszMessage = NULL;
    AECHAR *pszBuf = NULL;
    AECHAR  szFormat[] = {'%','s',':',' ','%','s','\0'};

    // try and print out a message
    switch(nStatus)
    {
    case EBADPARM:
      DIALER_ERR("Recieved EBADPARAM", 0, 0, 0);
      wIDS = IDS_EBADPARM;
      break;

    case ENOMEMORY:
      DIALER_ERR("Recieved ENOMEMORY", 0, 0, 0);
      wIDS = IDS_ENOMEMORY;
      break;

    case EUNSUPPORTED:
      DIALER_ERR("Recieved EUNSUPPORTED", 0, 0, 0);
      wIDS = IDS_EUNSUPPORTED;
      break;

    case EBADSTATE:
      DIALER_ERR("Recieved EBADSTATE", 0, 0, 0);
      wIDS = IDS_EBADSTATE;
      break;

    case EFAILED:
      DIALER_ERR("Recieved EFAILED", 0, 0, 0);
      // fall through
    default: 
      wIDS = IDS_EFAILED;
      break;

    }

    pszError = (AECHAR*) MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
    pszMessage = (AECHAR*) MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
    pszBuf = (AECHAR*) MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
    if((pszBuf == NULL) || (pszMessage == NULL) || (pszError == NULL))
    {
      DIALER_ERR("No memory", 0,0,0);
      FREEIF(pszError);
      FREEIF(pszMessage);
      FREEIF(pszBuf);
      return FALSE;
    }

    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_ERROR_CAPS, pszError, 
      ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, wIDS, pszMessage, 
      ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));

    WSPRINTF(pszBuf, (sizeof(AECHAR)*DIALERAPP_MAX_NOTIF_BUFFER_SIZE), szFormat, pszError, pszMessage);

    CDialerApp_NotifDlg_DisplayMsgEx(pMe, pszBuf, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

    FREE(pszError);
    FREE(pszMessage);
    FREE(pszBuf);

    // this is an error so return false
    return FALSE;
  }
}

/*=============================================================================
FUNCTION: CDialerApp_ClearTextCtl

DESCRIPTION: Clears a text control's contents

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_ClearTextCtl(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog *pActiveDlg;
  ITextCtl *pITextCtl;

  PRINT_FUNCTION_NAME();
  if(pMe == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);

  // error checking
  if (pActiveDlg == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }
  
  // Get control of the text control
  pITextCtl = (ITextCtl*)IDIALOG_GetControl(pActiveDlg, (int16) pMe->m_wTextCtlIDToClear);
  if(pITextCtl == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }
  
  // clear text control's contents
  (void) ITEXTCTL_SetText(pITextCtl, NULL, 0);
  (void) ITEXTCTL_Redraw(pITextCtl);

  pMe->m_wTextCtlIDToClear = 0;

  (void) CDialerApp_RefreshDlg(pMe);
}


/*=============================================================================
FUNCTION: CDialerApp_TextCtlGetText

DESCRIPTION: Gets text from text control

PARAMETERS:
  *pUser: CDialerApp object pointer
  *pszStr: string pointer to store result in
  nStrMax: length of string to get
  nTextCtlID: textctl id

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_TextCtlGetText(void *pUser, AECHAR *pszStr,
                                  uint16 wStrMax, uint16 wTextCtlID)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog *pActiveDlg;
  ITextCtl *pITextCtl;

  PRINT_FUNCTION_NAME();
  if(pMe == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);

  // error checking
  if (pActiveDlg == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }
  
  // Get control of the text control
  pITextCtl = (ITextCtl*)IDIALOG_GetControl(pActiveDlg, (int16) wTextCtlID);
  if(pITextCtl == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }
  
  // Get the Text from Text box and update param pszStr
  return(ITEXTCTL_GetText(pITextCtl, pszStr, wStrMax));

}

/*=============================================================================
FUNCTION: CDialerApp_TextCtlGetTextEx

DESCRIPTION: Gets text from text control and creates memory for it

PARAMETERS:
  **pszDestBuf: string pointer to store result in
  *pIDialog; pointer to dialog
  nTextCtlID: textctl id

RETURN VALUE:
  uint16: length of string stored

COMMENTS:

SIDE EFFECTS: Memory will be dynamically allocated and the caller must free 
              it when done.

SEE ALSO:

=============================================================================*/
uint16 CDialerApp_TextCtlGetTextEx(AECHAR **pszDestBuf, IDialog *pIDialog, uint16 wTextCtlId)
{
  uint16 wLen = 0;
  ITextCtl *pITextCtl;
  AECHAR *pszTempBuf = NULL;

  PRINT_FUNCTION_NAME();
  if(pIDialog == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  *pszDestBuf = NULL;

  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, (int16) wTextCtlId);

  if(pITextCtl != NULL)
  {
    pszTempBuf = ITEXTCTL_GetTextPtr(pITextCtl);

    if(pszTempBuf != NULL)
    {
      wLen = (uint16) WSTRLEN(pszTempBuf);
      if(wLen != 0)
      {
        *pszDestBuf = (AECHAR*) MALLOC(sizeof(AECHAR)*(wLen+1));
        if(pszDestBuf == NULL)
        {
          DIALER_ERR("No memory", 0,0,0);
          return 0;
        }
        (void) WSTRNCOPY(*pszDestBuf, (wLen+1), pszTempBuf);
      }
    }
  }

  return wLen;
}

/*=============================================================================
FUNCTION: CDialerApp_HyphenateNumberString

DESCRIPTION: Hyphenates the number string passed in.  If non digit chars
             exist, the string will just be returned without modification.

PARAMETERS:
  *pszStr: null terminated number string to be hyphenated

RETURN VALUE:
  AECHAR*: null terminated hyphenated string

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AECHAR* CDialerApp_HyphenateNumberString(AECHAR const *pszStr)
{
  uint16 i;
  AECHAR* pszHyphenated;
  uint16 wHyphenatedIndex = 0;
  int nStrLen;

  // allocate memory for the temp string
  pszHyphenated = (AECHAR *) MALLOC(DIALERAPP_MAX_STR_SIZE*sizeof(AECHAR));
  if(pszHyphenated == NULL)
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    return NULL;
  }
  nStrLen = WSTRLEN(pszStr);

  // check for non digit chars
  for(i=0; i<nStrLen; i++)
  {
    if((pszStr[i] < (AECHAR) '0') || (pszStr[i] > (AECHAR) '9'))
    {
      (void) WSTRNCOPY(pszHyphenated, DIALERAPP_MAX_STR_SIZE, pszStr);
      return pszHyphenated;
    }
  }

  
  // build up the hyphenated string based on the hyphen state
  if(nStrLen <= 5) 
  { // xxxxx
    for(i=0; i<nStrLen; i++)
    {
      pszHyphenated[wHyphenatedIndex++] = pszStr[i];
    }
    pszHyphenated[wHyphenatedIndex] = 0;
  }
  else if(nStrLen >= 6 && nStrLen <= 7)
  { // xxx-xxx
    // Make the no hyphen string hyphenated
    for(i=0; i<nStrLen; i++)
    {
      if(i == 3)
      {
        pszHyphenated[wHyphenatedIndex++] = (AECHAR) '-';
      }
      pszHyphenated[wHyphenatedIndex++] = pszStr[i];
    }
    pszHyphenated[wHyphenatedIndex] = 0;
  }
  else if(nStrLen >= 8 && nStrLen <= 10)
  { // xxx-xxx-xx
    // Make the no hyphen string hyphenated
    for(i=0; i<nStrLen; i++)
    {
      if((i == 3) || (i == 6))
      {
        pszHyphenated[wHyphenatedIndex++] = (AECHAR) '-';
      }
      pszHyphenated[wHyphenatedIndex++] = pszStr[i];
    }
    pszHyphenated[wHyphenatedIndex] = 0;
  }
  else
  { // x-xxx-xxx-xxxx
    // Make the no hyphen string hyphenated
    for(i=0; i<nStrLen; i++)
    {
      if(wHyphenatedIndex > DIALERAPP_MAX_STR_SIZE-1)
      {
        pszHyphenated[wHyphenatedIndex] = 0;  
        return pszHyphenated;
      }

      if((i == 1) || (i == 4) || (i == 7))
      {
        pszHyphenated[wHyphenatedIndex++] = (AECHAR) '-';
      }
      pszHyphenated[wHyphenatedIndex++] = pszStr[i];
    }
    pszHyphenated[wHyphenatedIndex] = 0;
  }

  return pszHyphenated;
}

/*=============================================================================
FUNCTION: CDialerApp_FormatNumberDisplayString

DESCRIPTION: Formats and decides what string to display for the number from
             the call info

PARAMETERS:
  *pUser: CDialerApp object pointer
  *pCallInfo: Call info to extract string from
  *pszStrBuf: string buffer to store string
  *dwSizeBuf: size of string buffer in AECHARs
  bHyphen: include hyphens in number

RETURN VALUE:
  none

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_FormatNumberDisplayString(void *pUser, AEECMCallInfo *pCallInfo, 
                                          AECHAR *pszStrBuf, uint32 dwSizeBuf,
                                          boolean bHyphen)
{
  CDialerApp *pMe = (CDialerApp*) pUser;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }


  switch(pCallInfo->srv_opt)
  {   
  case AEECM_SO_LOOPBACK:
  case AEECM_SO_LOOPBACK_13K:
  case AEECM_SO_LOOPBACK_SO55:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_LOOPBACK, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
    break;

  case AEECM_SO_MARKOV_SO54:
  case AEECM_SO_MARKOV:
  case AEECM_SO_MARKOV_13K:
  case AEECM_SO_RS1_MARKOV:
  case AEECM_SO_RS2_MARKOV:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_MARKOV, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
    break;

  case AEECM_SO_SMS:
  case AEECM_SO_RS2_SMS:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_SMS, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
    break;

  case AEECM_SO_ASYNC_DATA_13K_PRE707:
  case AEECM_SO_ASYNC_DATA_PRE707:
  case AEECM_SO_ASYNC_DATA_IS707:
  case AEECM_SO_ASYNC_DATA_13K_PROPTRY:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_ASYNCDATA, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
    break;

  case AEECM_SO_G3_FAX_13K_PRE707:
  case AEECM_SO_ANALOG_FAX_RS1:
  case AEECM_SO_ANALOG_FAX_RS2:
  case AEECM_SO_G3_FAX_PRE707:
  case AEECM_SO_G3_FAX_IS707:
  case AEECM_SO_G3_FAX_13K_PROPTRY:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FAX, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
    break;

  case AEECM_SO_MDR_PKT_DATA_FRS1_RRS1:
  case AEECM_SO_MDR_PKT_DATA_FRS1_RRS2:
  case AEECM_SO_MDR_PKT_DATA_FRS2_RRS1:
  case AEECM_SO_MDR_PKT_DATA_FRS2_RRS2:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_MDRPKTDATA, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
    break;

  case AEECM_SO_PPP_PKT_DATA_PRE707:
  case AEECM_SO_PPP_PKT_DATA_3G:
  case AEECM_SO_PPP_PKT_DATA_13K_PROPTRY:
  case AEECM_SO_PPP_PKT_DATA_IS707:
  case AEECM_SO_PPP_PKT_DATA_13K_PRE707:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_PPPPKTDATA, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
    break;

  case AEECM_SO_RS1_PD:
  case AEECM_SO_RS2_PD:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_POSDET, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
    break;

  case AEECM_SO_DATA:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_DATA, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
    break;

  default:
    // service option is not available so check the type
    switch(pCallInfo->call_type)
    {
    case AEECM_CALL_TYPE_EMERGENCY:
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_EMERGENCY, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));  
      break;

    case AEECM_CALL_TYPE_STD_OTASP:
    case AEECM_CALL_TYPE_NON_STD_OTASP:
      if(pCallInfo->call_state == AEECM_CALL_STATE_ORIG)
      {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_ACTIVATION, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));  
      }
      else
      {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_ACTIVATIONPROGRESS, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));  
      }
      break;

    case AEECM_CALL_TYPE_CS_DATA:
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_CSDATA, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
      break;

    case AEECM_CALL_TYPE_PS_DATA:
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_PSDATA, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
      break;

    case AEECM_CALL_TYPE_SMS:
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_SMS, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
      break;

    case AEECM_CALL_TYPE_PD:
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_POSDET, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
      break;

    case AEECM_CALL_TYPE_TEST:
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_TESTCALL, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
      break;

    case AEECM_CALL_TYPE_VOICE:
    default:
      if(pMe->m_CallInfo.pi == AEECM_PI_RES)
      {
        /* Number was NULL because PI was restricted, so ICM did
           not propagate number */
        (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_RESTRICTED_CALL, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
      }
      else if(WSTRLEN(pCallInfo->alpha) != 0)
      {
        //If there is an alpha from a CNAP message, display it instead of the 
        //calling party no., or the phone book name
        (void) WSTRNCOPY(pszStrBuf, (int)(dwSizeBuf), pCallInfo->alpha);
      }
      else if(WSTRLEN(pCallInfo->other_party_no) != 0)
      { // look up number in contacts
        DoNumberLookup(pMe->m_pIADDRBOOK, pCallInfo->other_party_no, pszStrBuf, (dwSizeBuf * sizeof(AECHAR)));
        if(WSTRLEN(pszStrBuf) == 0)
        {   
          if(bHyphen == TRUE)
          {
            AECHAR *pszHyphenatedNum = CDialerApp_HyphenateNumberString(pCallInfo->other_party_no);
            (void) WSTRNCOPY(pszStrBuf, (int)(dwSizeBuf), pszHyphenatedNum);
            FREEIF(pszHyphenatedNum);
          }
          else
          {
            (void) WSTRNCOPY(pszStrBuf, (int)(dwSizeBuf), pCallInfo->other_party_no);
          }
        }
      }
      else
      {
        // empty other party number = UNKNOWN
        (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_UNKNOWN, pszStrBuf, (int) (dwSizeBuf * sizeof(AECHAR)));
      }
    
    }//switch(pCallInfo->call_type)
    break;

  }//switch(pCallInfo->srv_opt)

}

/*=============================================================================
FUNCTION: CDialerApp_IsCallsToDisplay

DESCRIPTION: checks to see if there is any active calls to display

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if active calls exist

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IsCallsToDisplay(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  uint32 dwCurrentTime;
  int i;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  // check the connected calls first
  if(AEECM_IS_ANYCALL_CONNECTED(pMe->m_pICM) == TRUE)
  {
    return TRUE;
  }

  // get current time
  dwCurrentTime = GETUPTIMEMS();

  // look for ended calls that still need to be displayed
  for(i=0; i<DIALERAPP_MAX_NUM_CALLS; i++)
  {
    if(pMe->m_EndedCalls[i] != NULL)
    {
      if(pMe->m_EndedCalls[i]->dwDisplayStart == 0)
      {
        return TRUE;
      }
      else if((pMe->m_EndedCalls[i]->dwDisplayStart + DIALERAPP_MAX_ENDED_DISPLAY_TIME) > dwCurrentTime) 
      {
        return TRUE;
      }
    }
  }

  // no calls to display
  return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_HandleEndKey

DESCRIPTION: Handles the actions of the end key depending on active dialog

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean : TRUE if all the dialogs were ended successfuly.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_HandleEndKey(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;

  PRINT_FUNCTION_NAME();
  if (pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // check for active calls
  if(ICM_GetActiveCallIDs(pMe->m_pICM, AEECM_CALL_TYPE_NONE, AEECM_CALL_STATE_NONE, NULL, 0) > 0)
  {
    if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_EndAllCalls(pMe->m_pICM)) == FALSE)
    {
      DIALER_ERR("ICM_EndAllCalls FAILED", 0, 0, 0);  
      return FALSE;
    }

    return CDialerApp_DisplayStatusDlg(pMe);
  }

  // close dialer app and return to core
  return CDialerApp_StopApp(pMe);  
}

/*=============================================================================
FUNCTION: CDialerApp_CenterTitle

DESCRIPTION: Pads the beginning of title string with spaces to center it

PARAMETERS:
  *pUser: CDialerApp object pointer
  *pszSource: Contains the title string to be centered
  **pszDest: Contains the pointer to the location to store centered string
             MUST point to NULL
  wWidth: max width in pixels of area to center over
  Font: font of the title to be used

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:
  The The memory for the destination buffer is allocated in this function and 
  MUST be freed at a later time.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CenterTitle(void *pUser, AECHAR *pszSource, AECHAR **pszDest,
                               uint32 wWidth, AEEFont Font)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  AECHAR szSpace[] = {' ','\0'};
  AECHAR *pszBuf = NULL;
  int nStrWidth;
  int nNumSpaces;
  int nCurrSize = 0;
  int i;

  PRINT_FUNCTION_NAME();
  if((pMe == NULL) || (pszSource == NULL) || (wWidth == 0))
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  if(*pszDest != NULL)
  { 
    // error checking 
    DIALER_ERR("Dest pointer must be NULL", 0, 0, 0);
    return FALSE;
  }

  pszBuf = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  if(pszBuf == NULL)
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    return FALSE; //error
  }

  nStrWidth = IDISPLAY_MeasureText(pMe->a.m_pIDisplay, Font, pszSource);

  if((uint16)nStrWidth <= wWidth)
  {
    // title will fit on one line

    nNumSpaces = (int)(((wWidth - (uint16)nStrWidth)/2)/pMe->m_dwSpaceLength);
    for(i=0; i<nNumSpaces; i++)
    {
      nCurrSize++;
      if(nCurrSize < DIALERAPP_MAX_STR_SIZE)
      {
        (void) WSTRLCAT(pszBuf, szSpace, DIALERAPP_MAX_STR_SIZE);  
      }
      else
      {
        DIALER_ERR("Size Exceeded", 0, 0, 0);
        return FALSE;
      } 
    }

    nCurrSize += WSTRLEN(pszSource);
    if(nCurrSize < DIALERAPP_MAX_STR_SIZE)
    {
      (void) WSTRLCAT(pszBuf, pszSource, DIALERAPP_MAX_STR_SIZE);
    }
    else
    {
      DIALER_ERR("Size Exceeded", 0, 0, 0);
      return FALSE;
    }
  }
  else
  {
    DIALER_ERR("Size Exceeds One Line", 0, 0, 0);
    return FALSE;
  }
  
  *pszDest = pszBuf;

  // do not free memory for pszBuf the app must
  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_AutoAnswer

DESCRIPTION: Checks the rings and settings to see if the call should be auto
             answered

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_AutoAnswer(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;

  PRINT_FUNCTION_NAME();
  if(pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  if(CDialerApp_NVCheck_IsAutoAnswer() == TRUE)
  {
    int nRetVal;

    DIALER_MSG("AUTO ANSWER Incoming Voice Call", 0, 0, 0);

    // answer the current incoming call
    nRetVal = ICM_AnswerCall(pMe->m_pICM, pMe->m_byCurrIncomCall);

    if(CheckAEEReturnStatus(nRetVal) == FALSE)
    {
      DIALER_ERR("ICM_AnswerCall FAILED for AutoAnswer", 0, 0, 0);  
      return FALSE;
    }
  }

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_CCBSAutoAccept

DESCRIPTION: Checks the Config to see if the recall should be auto
             answered

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CCBSAutoAccept(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  OEMCCBS ccbs;
  AEECMRecallRspType respType;

  PRINT_FUNCTION_NAME();
  if(pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  ccbs = (OEMCCBS) CDialerApp_Check_IsCCBSAutoAccept(pMe);

  if(ccbs != OEM_CCBS_NONE)
  {
    int nRetVal;

    DIALER_MSG("AUTO Accept Recall Indication", 0, 0, 0);

    // answer the current incoming call
    switch(ccbs)
    {
      case OEM_CCBS_ACCEPT:
        respType = AEECM_RECALL_RESP_CCBS_ACCEPT;
        break;
      case OEM_CCBS_REJECT:
        respType = AEECM_RECALL_RESP_CCBS_REJECT;
        break;
      case OEM_CCBS_HOLD_N_ACCEPT:
        respType = AEECM_RECALL_RESP_CCBS_HOLD_ACCEPT;
        break;
      case OEM_CCBS_RELEASE_N_ACCEPT:
        respType = AEECM_RECALL_RESP_CCBS_REL_ACCEPT;
        break;
      default:
         return FALSE;
    }

    if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM))
    {
      if(respType == AEECM_RECALL_RESP_CCBS_ACCEPT)
      {
        respType = AEECM_RECALL_RESP_CCBS_HOLD_ACCEPT;
      }
    }
    else
    {
      if(respType == AEECM_RECALL_RESP_CCBS_HOLD_ACCEPT ||
         respType == AEECM_RECALL_RESP_CCBS_REL_ACCEPT)
      {
         respType = AEECM_RECALL_RESP_CCBS_ACCEPT;
      }
    }
    
    nRetVal = ICM_RecallRsp(pMe->m_pICM, respType, pMe->m_byCurrRecallID );

    if(CheckAEEReturnStatus(nRetVal) == FALSE)
    {
      DIALER_ERR("ICM_RecallRsp FAILED for AutoAccept", 0, 0, 0);  
      return FALSE;
    }
  }

  return TRUE;
}


/*=============================================================================
FUNCTION: CDialerApp_Check_IsCCBSAutoAccept

DESCRIPTION: Checks the Config Setting to see if the recall should be auto
             answered

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 CDialerApp_Check_IsCCBSAutoAccept(void *pUser)
{
  IConfig *pIConfig;
  int32 nReturnStatus;
  OEMCCBS ccbs;
  OEMCCBS retVal = OEM_CCBS_NONE;

  CDialerApp *pMe = (CDialerApp*) pUser;
  

  PRINT_FUNCTION_NAME();
  if(pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

   /* Create the IConfig Serivce object. */
  nReturnStatus = ISHELL_CreateInstance(pMe->a.m_pIShell,
                               AEECLSID_CONFIG, (void **) &pIConfig);

  if(nReturnStatus == SUCCESS && pIConfig != NULL)
  {
     if(CheckAEEReturnStatus(ICONFIG_GetItem(pIConfig, CFGI_CCBS , 
        &ccbs, sizeof(OEMCCBS))) != TRUE)
     {
        retVal =  OEM_CCBS_NONE;
     }
     else
     {
       retVal = ccbs;
     }
     ICONFIG_Release(pIConfig);
  }
  else
  {
     retVal = OEM_CCBS_NONE;
  }

  return (uint32)retVal;

}

/*=============================================================================
FUNCTION: CDialerApp_OriginateVoiceCall

DESCRIPTION: Originate a voice call with all of the proper checks and create
             the needed dialog

PARAMETERS:
  *pUser: CDialerApp object pointer
  *pszNumber: number buffer to be dialed

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_OriginateVoiceCall(void *pUser, AECHAR* pszNumber)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  uint8 nCMCallID;
  AEECMCallID byCallNum = 1;
  int nRetVal; 
  AECHAR *pszHardPause;
  AECHAR *pszTimePause;
  IDialog *pIDialog;
  boolean bEmergencyNumber = FALSE;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)   
  uint32 dwStrLen = (uint32) WSTRLEN(pszNumber);
  int nOperation;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

  PRINT_FUNCTION_NAME();
  if((pMe == NULL) || (pszNumber == NULL))
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // dial string checking
  if((WSTRLEN(pszNumber) == 0) || (pszNumber[0] == 'T') || ((pszNumber[0] == 'H')))
  {
    DIALER_ERR("Bad Dial String", 0, 0, 0);
    return FALSE;  
  }

   // Check for wildcard chars
  if(WSTRCHR(pszNumber, (AECHAR) '?') != NULL)
  {
    uint16 wDlgID;
    pIDialog= CDialerApp_GetActiveDlg(pMe);

    if(pIDialog != NULL)
    {
      wDlgID = CDialerApp_GetActiveDlgID(pMe);
      
      // check to see if current dialog is wildcard
      if(wDlgID == IDD_WILDCARD)
      {
        // if so pop it so we can reset the dialog
        (void) CDialerApp_EndDlg(pMe);
      }
    }

    // copy over orgination string to dial string for dialog
    (void) WSTRNCOPY(pMe->m_pszDialString, DIALERAPP_NUMBER_BUFFER_SIZE, pszNumber);
     
    return CDialerApp_CreateDlg(pMe, IDD_WILDCARD, NULL, 0);
  }

  // check for time or hard pauses
  pszHardPause = WSTRCHR(pszNumber, 'H');
  pszTimePause = WSTRCHR(pszNumber, 'T');
  if((pszTimePause != NULL) || (pszHardPause != NULL))
  {
    AECHAR *pszFirstPause;
    
    // find location of first time pause
    if(pszHardPause == NULL)
    {
      pszFirstPause = pszTimePause;
    }
    else if(pszTimePause == NULL)
    {
      pszFirstPause = pszHardPause;
    }
    else
    {
      pszFirstPause = (pszHardPause < pszTimePause) ? pszHardPause:pszTimePause;
    }

    if(pszFirstPause != NULL)
    {
      // make sure there is numbers after pauses
      if(*(pszFirstPause+1) != 0)
      {
        // break up the dialed string
        (void) WSTRNCOPYN(pMe->m_pszTimePauseSent, (DIALERAPP_NUMBER_BUFFER_SIZE*2), pszNumber, (pszFirstPause-pszNumber)); 
        (void) WSTRNCOPYN(pMe->m_pszTimePauseRemaining, DIALERAPP_NUMBER_BUFFER_SIZE, pszFirstPause, WSTRLEN(pszFirstPause));
        (void) WSTRNCOPY(pszNumber, DIALERAPP_NUMBER_BUFFER_SIZE, pMe->m_pszTimePauseSent);
      }
      else
      {
        // get rid of this useless pause so it doesnt show in calling and status
        *pszFirstPause = '\0';
      }
    }
  }

  /* Check for an emergency number first and bypass all other checks if
   * it is one. */
  if (IADDRBOOKEXT_EnumRecInitEx(pMe->m_pIADDRBOOK, AEE_ADDR_CAT_ECC,
                                 AEE_ADDRFIELD_NONE, pszNumber,
                                 (WSTRLEN(pszNumber)+1) * sizeof(AECHAR),
                                 ADDRREC_SEARCHTYPE_EXACT) != AEE_SUCCESS)
  {
    DIALER_ERR("IADDRBOOKEXT_EnumRecInitExFAILED", 0, 0, 0);  
  }
  else
  {
    IAddrRec *addrRec;
    addrRec = IADDRBOOK_EnumNextRec(pMe->m_pIADDRBOOK);
    if (addrRec != NULL)
    {
      /* We found a record, thus we know its an emergency number. */
      bEmergencyNumber = TRUE;
      IADDRREC_Release(addrRec);
    }
  }
          

  if (!bEmergencyNumber)
  {
    nRetVal = (uint32)ICM_CallOpsStringCheck(pMe->m_pICM, pszNumber, &byCallNum);
    if((nRetVal == (int)AEECM_IS_CALLOPS) || (nRetVal == (int)AEECM_IS_CALLOPS_NEED_CALLID))
    {
      // get rid of the timepause digits because they are not needed
      (void) MEMSET(pMe->m_pszTimePauseSent, 0, (sizeof(AECHAR)*(DIALERAPP_NUMBER_BUFFER_SIZE*2)));
      (void) MEMSET(pMe->m_pszTimePauseRemaining, 0, (sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));

      // check for call dependent call operation strings
      if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_CallOpsRequest(pMe->m_pICM, pszNumber, byCallNum)) == FALSE)
      {
        DIALER_ERR("ICM_CallOpsRequest FAILED", 0, 0, 0);  
        return FALSE;
      } 
      else
      {
        pIDialog= CDialerApp_GetActiveDlg(pMe);

        if(pIDialog == NULL)
        {
          CDialerApp_CreateDlg(pMe, IDD_STATUS, NULL, 0);
        }

        ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
        return TRUE;
      }
    }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)   
    // check for SUPPSVC strings
    nRetVal = ISUPPSVC_StringCheck(pMe->m_pISUPPSVC, (int) dwStrLen, pszNumber, &nOperation); 
    if(nRetVal == AEESUPPSVC_IS_SS) 
    {
      uint8 requestId;
      uint8 returnValue;

      // get rid of the timepause digits because they are not needed
      (void) MEMSET(pMe->m_pszTimePauseSent, 0, (sizeof(AECHAR)*(DIALERAPP_NUMBER_BUFFER_SIZE*2)));
      (void) MEMSET(pMe->m_pszTimePauseRemaining, 0, (sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));

       returnValue   = ISUPPSVC_StartSS(pMe->m_pISUPPSVC, pszNumber,&requestId,TRUE);
       
       switch(returnValue)
       {
         case AEESUPPSVC_SS_FAILED_INCORRECT_MODE:
         case AEESUPPSVC_SS_FAILED_NOT_SUPPORTED:
         case AEESUPPSVC_SS_FAILED_UNKNOWN:
         case AEESUPPSVC_SS_FAILED_NOT_SS:
         case AEESUPPSVC_SS_FAILED_NO_RESOURCES:
         case AEESUPPSVC_SS_FAILED_PINS_DONT_MATCH:
         case AEESUPPSVC_SS_FAILED_FDN_CHECK:
             CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_REQUEST_NOT_COMPLETED, 
                                        DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
             MSG_ERROR("Start SS Failed with error = %d\n",returnValue,0,0);
           return TRUE;       
         default:
           break;
       }

      // display status 
      pIDialog= CDialerApp_GetActiveDlg(pMe);

      if(pIDialog == NULL)
      {
        CDialerApp_CreateDlg(pMe, IDD_STATUS, NULL, 0);
      }
      ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
      return TRUE;
    }
    //Any partial SS numbers or otherwise will be originated as a voicecall

#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)  

  }
  else
  {
    DIALER_MSG("Emergency Number found, originated this one now.", 0, 0, 0);
  }
  // place call using ICM
  nRetVal = ICM_OriginateVoiceCall(pMe->m_pICM, pszNumber, &nCMCallID);
  if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
  {
    DIALER_ERR("ICM_OriginateVoiceCall FAILED", 0, 0, 0);  
    return FALSE;
  }
  pMe->m_bWaitingforOrigVoiceCall = TRUE;
  
  pIDialog= CDialerApp_GetActiveDlg(pMe);

  if(pIDialog == NULL)
  {
    CDialerApp_CreateDlg(pMe, IDD_STATUS, NULL, 0);
  }
  ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_StartKeyPressDTMF

DESCRIPTION: Starts playing Key Press DTMF

PARAMETERS:
  *pUser: CDialerApp object pointer
  cDTMF: char of dtmf tone to be started

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StartKeyPressDTMF(void *pUser, AECHAR cDTMF)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
  uint16 wNumCallIDs, wActiveDlgID;
  boolean bSentDTMF;
  AECHAR szDTMF[2];
  uint8 i= 0;

  PRINT_FUNCTION_NAME();
  if (pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  wActiveDlgID = CDialerApp_GetActiveDlgID(pMe);


  if(wActiveDlgID == IDD_USSD_PASSWORD || wActiveDlgID == IDD_USSD_INFO)
  {
    DIALER_ERR("Not on Valid Dialog", 0, 0, 0);
    return FALSE;
  }

  // init return value
  bSentDTMF = FALSE;

  // setup burst DTMF
  szDTMF[0] = cDTMF;
  szDTMF[1] = '\0';

  // get all active call ids
  wNumCallIDs = ICM_GetActiveCallIDs(pMe->m_pICM, (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                                                  AEECM_CALL_TYPE_EMERGENCY|
                                                                  AEECM_CALL_TYPE_NON_STD_OTASP|
                                                                  AEECM_CALL_TYPE_STD_OTASP), 
                (AEECMCallState)AEECM_CALL_STATE_NONE, (AEECMCallID*) CallIDs, sizeof(CallIDs));

  if(wNumCallIDs>0)
  {
    while(i < wNumCallIDs && !bSentDTMF)
    {

      AEECMCallID byCurrentCallID = CallIDs[i];
    
    if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, byCurrentCallID, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == TRUE)
    {
      if(pMe->m_CallInfo.call_state == AEECM_CALL_STATE_CONV
         || 
         (pMe->m_CallInfo.call_state == AEECM_CALL_STATE_ORIG && 
           pMe->m_CallInfo.call_progress == AEECM_PROGIND_USER_RESUMED))
      {
        if(pMe->m_byNVDTMF == OEMNV_DTMFBURST) // BURST DTMFs
        {    
          if(CheckAEEReturnStatus(ICM_BurstDTMF(pMe->m_pICM, byCurrentCallID, AEECM_DTMF_ON_150, AEECM_DTMF_OFF_100, szDTMF)) 
            == FALSE)
          {
            DIALER_ERR("ICM_BurstDTMF FAILED", 0, 0, 0);  
          }
          else
          {
            bSentDTMF = TRUE;
            DIALER_MSG("Burst DTMF %c was sent", cDTMF, 0, 0);
          }
        } 
        else if(pMe->m_byNVDTMF == OEMNV_DTMFCONT)// CONTINUOUS DTMFs
        {
          if(pMe->m_CurrDTMF != 0)
          { 
            // DTMF is already playing so stop it before starting a new one
            if(CheckAEEReturnStatus(ICM_StopContDTMF(pMe->m_pICM, byCurrentCallID)) == FALSE)
            {
              DIALER_ERR("ICM_StopContDTMF FAILED", 0, 0, 0);  
            } 
          }

          // start transmitting continous DTMF
          if(CheckAEEReturnStatus(ICM_StartContDTMF(pMe->m_pICM, byCurrentCallID, cDTMF)) == FALSE)
          {
            DIALER_ERR("ICM_StartContDTMF FAILED", 0, 0, 0);  
          }
          else
          {
            DIALER_MSG("START DTMF %c", cDTMF, 0, 0);
            bSentDTMF = TRUE;
          }
        }
      }
    }
      i++;
    }
  }

  // update global sent variable
  if(bSentDTMF == TRUE)
  {
    if(pMe->m_byNVDTMF == OEMNV_DTMFCONT)
    {
      pMe->m_CurrDTMF = cDTMF; 
    }
    else
    {
      pMe->m_CurrDTMF = 0; 
    }
  }

  return bSentDTMF;
}

/*=============================================================================
FUNCTION: CDialerApp_StopKeyPressDTMF

DESCRIPTION: Stops the Key Press DTMF

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StopKeyPressDTMF(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
  uint16 wNumCallIDs;
  boolean bStopDTMF;
  uint8 i = 0;

  PRINT_FUNCTION_NAME();
  if (pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // init return value
  bStopDTMF = FALSE;

  wNumCallIDs = ICM_GetActiveCallIDs(pMe->m_pICM, 
    (AEECMCallType)(AEECM_CALL_TYPE_VOICE|AEECM_CALL_TYPE_EMERGENCY|
                    AEECM_CALL_TYPE_NON_STD_OTASP|AEECM_CALL_TYPE_STD_OTASP), 
    (AEECMCallState)AEECM_CALL_STATE_NONE, (AEECMCallID*)CallIDs, sizeof(CallIDs));     

  // check to see if there is a DTMF that needs to be stoped
  if(pMe->m_CurrDTMF != 0) 
  {
    if(wNumCallIDs>0)
    {  
      while(i < wNumCallIDs && !bStopDTMF)
      {

    AEECMCallID byCurrentCallID = CallIDs[i];

        if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, byCurrentCallID, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == TRUE)
        {
          if((pMe->m_CallInfo.call_state == AEECM_CALL_STATE_CONV) || 
         (pMe->m_CallInfo.call_state == AEECM_CALL_STATE_ORIG && 
              pMe->m_CallInfo.call_progress == AEECM_PROGIND_USER_RESUMED))
          {

            if(CheckAEEReturnStatus(ICM_StopContDTMF(pMe->m_pICM, byCurrentCallID)) == FALSE)
            {
              DIALER_ERR("ICM_StopContDTMF FAILED", 0, 0, 0);  
            }
            else
            {
              DIALER_MSG("STOP DTMF", 0, 0, 0);
              bStopDTMF = TRUE;
            }
          }
        }
        i++;
      }
    }
    
  }

  if(bStopDTMF == TRUE)
  {
    pMe->m_CurrDTMF = 0;
  }

  return bStopDTMF;
}

/*=============================================================================
FUNCTION: CDialerApp_StopApp

DESCRIPTION: Stops dialer and returns to core and performs any cleanup needed

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StopApp(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  
  PRINT_FUNCTION_NAME();
  if (pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // close Dialer App
  if(pMe->m_bSuspended == TRUE)
  {
    DIALER_MSG("Already suspended so cannot stop App", 0, 0, 0);
    // can't close app if we are suspended already
    // but perform some cleanup
    return CDialerApp_OnAppStop(pMe, 0, 0);
  }
  
  // set the suspended flag because the call is async and something may come
  // in before it is closed
  pMe->m_bAppBeingSuspended = TRUE;

  DIALER_MSG("Stopping App", 0, 0, 0);
  return CheckAEEReturnStatus(ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE));
}

/*=============================================================================
FUNCTION: CDialerApp_SpeedDial

DESCRIPTION: Attempts to perform a speed dial

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_SpeedDial(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  AECHAR *pszSpeedDial = NULL;
  AECHAR *pszNumber = NULL;
  AECHAR szNewChar[2];

  PRINT_FUNCTION_NAME();
  if(pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return;
  }

  pszSpeedDial = (AECHAR *) MALLOC((DIALERAPP_NUMBER_BUFFER_SIZE) * sizeof(AECHAR));
  pszNumber = (AECHAR *) MALLOC((DIALERAPP_NUMBER_BUFFER_SIZE) * sizeof(AECHAR));
  if((pszNumber == NULL) || (pszSpeedDial == NULL))
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    FREEIF(pszSpeedDial);
    FREEIF(pszNumber);
    return; //error
  }

  if(CDialerApp_GetActiveDlgID(pMe) == IDD_NUMEDIT)
  {
    // get contents of text control
    if(CDialerApp_TextCtlGetText(pMe, pszNumber, 
      DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_NUMEDIT) == FALSE)
    {
      FREE(pszSpeedDial);
      FREE(pszNumber);
      return;
    }
  }
  else
  {
    // add on the digit because it is not in the text control yet
    szNewChar[0] = pMe->m_LastKeyPressed;
    szNewChar[1] = '\0';
    (void) WSTRLCAT(pszNumber, szNewChar, DIALERAPP_NUMBER_BUFFER_SIZE);
  }

  // check the speed dial entries
  DoSpeedDialLookup(pMe->m_pIADDRBOOK, pszNumber, pszSpeedDial,
                    DIALERAPP_NUMBER_BUFFER_SIZE * sizeof(AECHAR));
  if (*pszSpeedDial != '\0')
  { 
    if(CDialerApp_OriginateVoiceCall(pMe, pszSpeedDial) == TRUE)
    {
      pMe->m_bSpeedDialing = TRUE;
    }
  }

  // free the memory
  FREE(pszSpeedDial);
  FREE(pszNumber);
}

/*=============================================================================
FUNCTION: CDialerApp_StartTimer_SpeedDial

DESCRIPTION: Starts the timer for speed dial

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StartTimer_SpeedDial(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;

  PRINT_FUNCTION_NAME();
  if(pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  if(pMe->m_bNVOneTouch == TRUE) 
  { 
    (void) ISHELL_SetTimer(pMe->a.m_pIShell, DIALERAPP_ONETOUCH_SPEEDDIAL_TIMEOUT, 
                  (PFNNOTIFY) CDialerApp_SpeedDial, (void*) pMe);
  }

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_StopTimer_SpeedDial

DESCRIPTION: Stops the timer for speed dial

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StopTimer_SpeedDial(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;

  PRINT_FUNCTION_NAME();
  if(pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // reset the core flag
  pMe->m_bOneTouchFromCore = FALSE;

  return CheckAEEReturnStatus(ISHELL_CancelTimer(pMe->a.m_pIShell, 
                               (PFNNOTIFY) CDialerApp_SpeedDial, (void*) pMe));
}

/*=============================================================================
FUNCTION: CDialerApp_PlayCallFailedSound

DESCRIPTION:  Plays the call failed sound

PARAMETERS:
  *pUser: CDialerApp object pointer
  Alert: Alert to be played by IAlert

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_PlayCallFailedSound(void *pUser, uint32 Alert)
{
  CDialerApp *pMe = (CDialerApp*) pUser;

  if(pMe == NULL)
  {
    DIALER_ERR("NULL Pointer", 0, 0, 0);   
    return;
  }

  // save the current alert
  pMe->m_CallFailedAlert = (AEEALERTType)Alert;

  if(Alert != (uint32)AEEALERT_ALERT_NONE)
  {
    (void) IALERT_StartAlerting(pMe->m_pIALERT, AEEALERT_CALLTYPE_VOICE, NULL, (AEEALERTType)Alert);
    (void) ISHELL_SetTimer(pMe->a.m_pIShell, (DIALERAPP_MAX_DISPLAY_CALL_FAIL*1000), (PFNNOTIFY)CDialerApp_StopCallFailedSound, (void*)pMe);
  }
}

/*=============================================================================
FUNCTION: CDialerApp_StopCallFailedSound

DESCRIPTION:  Stops the call failed sound if it is still playing

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_StopCallFailedSound(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  AEEALERTType AlertType;

  if(pMe == NULL)
  {
    DIALER_ERR("NULL Pointer", 0, 0, 0);   
    return;
  }

  (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)CDialerApp_StopCallFailedSound, (void*)pMe);

  (void) IALERT_GetInfo(pMe->m_pIALERT, NULL, NULL, 0, &AlertType);

  if(pMe->m_CallFailedAlert == AlertType)
  {
    pMe->m_CallFailedAlert = AEEALERT_ALERT_NONE;
    (void) IALERT_StopAlerting(pMe->m_pIALERT); 
  }
}

/*=============================================================================
FUNCTION: CDialerApp_NVCheck_IsAutoHyphen

DESCRIPTION: Checks NV auto hyphen setting

PARAMETERS:
  NONE

RETURN VALUE:
  boolean: returns TRUE if nv setting is set

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NVCheck_IsAutoHyphen(void)
{
  nv_item_type nvi;

  PRINT_FUNCTION_NAME();
  if(ui_get_nv(NV_AUTO_HYPHEN_I, &nvi) == NV_DONE_S) 
  {
    return(nvi.auto_hyphen);
  }
  else
  {
    // default
    return TRUE;   
  }
}

/*=============================================================================
FUNCTION: CDialerApp_NV_GetDTMF

DESCRIPTION: Checks NV cont DTMF setting

PARAMETERS:
  NONE

RETURN VALUE:
  uint8 : returns TRUE if nv setting is set
           1 = NORMAL Beep Length
           2 = LONG Beep Length
           3 = OFF

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint8 CDialerApp_NV_GetDTMF(void)
{
  nv_item_type nvi;

  PRINT_FUNCTION_NAME();
  if(ui_get_nv(NV_DTMF_I, &nvi) == NV_DONE_S) 
  {
    return(nvi.dtmf);
  }
  else
  {
    // default
    return TRUE;   
  }
}

/*=============================================================================
FUNCTION: CDialerApp_NVCheck_IsOneTouch

DESCRIPTION: Checks NV one touch setting

PARAMETERS:
  NONE

RETURN VALUE:
  boolean: returns TRUE if nv setting is set

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NVCheck_IsOneTouch(void)
{
  nv_item_type nvi;

  PRINT_FUNCTION_NAME();
  if(ui_get_nv(NV_ONETOUCH_DIAL_I, &nvi) == NV_DONE_S) 
  {
    return(nvi.onetouch_dial);
  }
  else
  {
    // default
    return FALSE;   
  }
}

/*=============================================================================
FUNCTION: CDialerApp_NVCheck_IsAutoAnswer

DESCRIPTION: Checks NV auto answer setting

PARAMETERS:
  NONE

RETURN VALUE:
  boolean: returns TRUE if nv setting is set

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NVCheck_IsAutoAnswer(void)
{
  nv_item_type nvi;

  PRINT_FUNCTION_NAME();
  if(ui_get_nv(NV_AUTO_ANSWER_I, &nvi) == NV_DONE_S) 
  {
    return(nvi.auto_answer.enable);
  }
  else
  {
    // default
    return FALSE;   
  }
}

//****************************************************************************
//                  Dialog Stack Handling Functions
//****************************************************************************

/*=============================================================================
FUNCTION: CDialerApp_DisplayStatusDlg

DESCRIPTION: Bring the program to the status screen display

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_DisplayStatusDlg(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  int i;
  CDlgItem *pDlgItem = NULL;
  CDlgItem *pDlgAddItem = NULL;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  // check to see if we are suspended
  if(pMe->m_bSuspended == TRUE)
  {
    // store startup dialog info
    pMe->m_pStartupDlgInfo = NULL;
    pMe->m_dwStartupDlgInfoSize = 0;
    pMe->m_wStartupDlgID = IDD_STATUS;

    return(CheckAEEReturnStatus(ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_DIALER)));
  }

  // cancel shutdown timer
  (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_StopApp, (void*) pMe);
  
  // check to see if notif dialog is showing
  if(CDialerApp_GetActiveDlgID(pMe) == IDD_NOTIF)
  {
    // we want to hurry up and get rid of notif
    (void) CDialerApp_NotifDlg_CloseDlg(pMe);

    // kill all of the dialogs below notif
    for(i=(int)(IVector_Size(pMe->m_pDialogStack)-2); i>=0; i--)
    {
      pDlgItem = IVector_RemoveElementAt(pMe->m_pDialogStack, (uint32)i);
      if(pDlgItem == NULL)
      {
        DIALER_ERR("Null Pointer", 0, 0, 0);
      }
      else
      {
        FREEIF(pDlgItem->pDlgInfo);
        FREE(pDlgItem);
      }
    }  
  }
  else
  {
    // close all of the dialogs and create the status dlg
    if(CDialerApp_EndAllDlgs(pMe) == FALSE)
    {
      DIALER_ERR("CDialerApp_EndAllDlgs FAILED", 0, 0, 0);
      return FALSE;
    }  
  }

  if(IVector_IsEmpty(pMe->m_pDialogStack) == TRUE)
  {
    // check to see if the status dialog should be displayed or app should be ended
    if(CDialerApp_IsCallsToDisplay(pMe) == FALSE)
    { 
      if(pMe->m_bIconStartedApp == FALSE)
      {
        // set timer to close the dialer app
        DIALER_MSG("Dialer will close in 250 ms unless a dialog is created", 0,0,0);
        (void) ISHELL_SetTimer(pMe->a.m_pIShell, 250, (PFNNOTIFY) CDialerApp_StopApp, (void*) pMe);
        return TRUE;
      }
    }
  }

  // create element to add to stack
  pDlgAddItem = (CDlgItem*) MALLOC(sizeof(CDlgItem));
  if(pDlgAddItem == NULL)
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    return FALSE;
  }
  pDlgAddItem->pDlgInfo = NULL; 
  pDlgAddItem->wDlgID = IDD_STATUS;  
  pDlgAddItem->dwInfoSize = 0;

  // add status dialog to the bottom position
  if(CheckAEEReturnStatus(IVector_InsertElementAt(pMe->m_pDialogStack, pDlgAddItem, 0)) == FALSE)
  {
    DIALER_ERR("IVector_InsertElementAt failed for dialog IDD_STATUS", 0, 0, 0);
    FREEIF(pDlgAddItem);
    return FALSE;
  }

  // check to see if the brew display needs to be updated
  if(ISHELL_GetActiveDialog(pMe->a.m_pIShell) == NULL)
  {
    // display dlg on the top of stack in brew
    pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (uint32)(IVector_Size(pMe->m_pDialogStack)-1));
    if(pDlgItem == NULL)
    {
      DIALER_ERR("DIALOG STACK CORUPTION !!!!!", 0, 0, 0);
      (void) CDialerApp_StopApp(pMe);
      return FALSE;  
    }

    if(CheckAEEReturnStatus(ISHELL_CreateDialog(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
      pDlgItem->wDlgID, NULL)) == FALSE)
    {
      DIALER_ERR("ISHELL_CreateDialog FAILED for IDD_STATUS", 0, 0, 0);
      FREEIF(pDlgAddItem);
      IVector_RemoveElementAt(pMe->m_pDialogStack, (uint32)(IVector_Size(pMe->m_pDialogStack)-1));
      (void) CDialerApp_StopApp(pMe);
      return FALSE;
    } 
  }

  //CDialerApp_PrintDlgStack(pMe);

  return TRUE;
} 

/*=============================================================================
FUNCTION: CDialerApp_EndCallRelatedDlgs

DESCRIPTION: End dialogs related to the passed call id.  This could possibly 
             cause the StatusDlg to be displayed.

PARAMETERS:
  *pUser: CDialerApp object pointer
   byCallID: call id from an event

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_EndCallRelatedDlgs(void *pUser, uint8 byCallID)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  CDlgItem *pDlgItem = NULL;
  CDlgItem *pDlgAddItem = NULL;
  int i;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  // check to see if we are suspended
  if(pMe->m_bSuspended == TRUE)
  {
    // store startup dialog info
    pMe->m_pStartupDlgInfo = NULL;
    pMe->m_dwStartupDlgInfoSize = 0;
    pMe->m_wStartupDlgID = IDD_STATUS;

    return(CheckAEEReturnStatus(ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_DIALER)));
  }

  // cancel shutdown timer
  (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_StopApp, (void*) pMe);

  // we want to delete every thing that is not important from the dialog stack
  for(i=(int)(IVector_Size(pMe->m_pDialogStack)-1); i>=0; i--)
  {
    pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (uint32)i);
    if(pDlgItem == NULL)
    {
      DIALER_ERR("Null pointer", 0,0,0);
      return FALSE;
    }

    switch(pDlgItem->wDlgID)
    {
    case IDD_NOTIF:
      // do nothing here
      break;

    case IDD_FORWARDING:
    case IDD_INCOM_OPTIONS:
    case IDD_INCOMING:
      if(byCallID == pMe->m_byCurrIncomCall)
      {
        // remove this dialog from the stack
        if(CDialerApp_DisplayStatusDlgRemove(pMe, (uint32)i) == FALSE)
        { 
          return FALSE;
        }
      }
      break;

    case IDD_CALLING:
      if(byCallID == pMe->m_byCurrOrigCall)
      {
        // remove this dialog from the stack
        if(CDialerApp_DisplayStatusDlgRemove(pMe, (uint32)i) == FALSE)
        { 
          return FALSE;
        }
      }
      break;

    case IDD_USSD_PROMPT:
    case IDD_USSD_INFO:
    case IDD_USSD_PASSWORD:
    case IDD_SSREQUEST:
    case IDD_MISSEDCALL:
      // do not close these dialogs
      break;

    default:
      // remove this dialog from the stack
      if(CDialerApp_DisplayStatusDlgRemove(pMe, (uint32)i) == FALSE)
      { 
        return FALSE;
      }
      break;
    }
  }

  if(IVector_IsEmpty(pMe->m_pDialogStack) == TRUE)
  {
    // check to see if the status dialog should be displayed or app should be ended
    if(CDialerApp_IsCallsToDisplay(pMe) == FALSE)
    { 
      if(pMe->m_bIconStartedApp == FALSE)
      {
        // set timer to close the dialer app
        DIALER_MSG("Dialer will close in 250 ms unless a dialog is created", 0,0,0);
        (void) ISHELL_SetTimer(pMe->a.m_pIShell, 250, (PFNNOTIFY) CDialerApp_StopApp, (void*) pMe);
        return TRUE;
      }
    }
  }

  // create element to add to stack
  pDlgAddItem = (CDlgItem*) MALLOC(sizeof(CDlgItem));
  if(pDlgAddItem == NULL)
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    return FALSE;
  }
  pDlgAddItem->pDlgInfo = NULL; 
  pDlgAddItem->wDlgID = IDD_STATUS;  
  pDlgAddItem->dwInfoSize = 0;

  // add status dialog to the bottom position
  if(CheckAEEReturnStatus(IVector_InsertElementAt(pMe->m_pDialogStack, pDlgAddItem, 0)) == FALSE)
  {
    DIALER_ERR("IVector_InsertElementAt failed for dialog IDD_STATUS", 0, 0, 0);
    FREEIF(pDlgAddItem);
    return FALSE;
  }
  
  // check to see if the brew display needs to be updated
  if(ISHELL_GetActiveDialog(pMe->a.m_pIShell) == NULL)
  {
    // display dlg on the top of stack in brew
    pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (uint32)(IVector_Size(pMe->m_pDialogStack)-1));
    if(pDlgItem == NULL)
    {
      DIALER_ERR("DIALOG STACK CORUPTION !!!!!", 0, 0, 0);
      (void) CDialerApp_StopApp(pMe);
      return FALSE;  
    }

    if(CheckAEEReturnStatus(ISHELL_CreateDialog(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
      pDlgItem->wDlgID, NULL)) == FALSE)
    {
      DIALER_ERR("ISHELL_CreateDialog FAILED for IDD_STATUS", 0, 0, 0);
      FREEIF(pDlgAddItem);
      (void) CDialerApp_StopApp(pMe);
      return FALSE;
    } 
  }

  //CDialerApp_PrintDlgStack(pMe);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_DisplayStatusDlgRemove

DESCRIPTION: 

PARAMETERS:


RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_DisplayStatusDlgRemove(CDialerApp *pMe, uint32 wPos)
{
  CDlgItem *pDlgItem = NULL;
  IDialog *pIDialog = NULL;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  // remove this dialog from the stack
  pDlgItem = IVector_RemoveElementAt(pMe->m_pDialogStack, wPos);
  if(pDlgItem == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  //close dialog if it is active in brew
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog != NULL)
  {
    if(IDIALOG_GetID(pIDialog) == pDlgItem->wDlgID)
    {
      // close this active brew dialog
      if(CheckAEEReturnStatus(ISHELL_EndDialog(pMe->a.m_pIShell)) == FALSE)
      {
        DIALER_ERR("ISHELL_EndDialog FAILED", 0,0,0);
        FREEIF(pDlgItem->pDlgInfo);
        FREE(pDlgItem);
        return FALSE;
      }
    }
  }

  // free allocated memory
  FREEIF(pDlgItem->pDlgInfo);
  FREE(pDlgItem);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_EndAllDlgs

DESCRIPTION: Ends all of the dialogs unconditionally

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean : TRUE if the successfull

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
boolean CDialerApp_EndAllDlgs(void *pUser)
{
  CDialerApp* pMe = (CDialerApp*) pUser;
  uint32 dwStackSize;
  CDlgItem *pDlgItem = NULL;

  PRINT_FUNCTION_NAME();
  if (pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // end all of the brew dialogs
  while(ISHELL_GetActiveDialog(pMe->a.m_pIShell) != NULL)
  {
    // this call is ok here
    (void) ISHELL_EndDialog(pMe->a.m_pIShell);
  } 

  dwStackSize = IVector_Size(pMe->m_pDialogStack);
  if(dwStackSize!= 0)
  {
    int i;

    for(i=0; i<(int)dwStackSize; i++)
    {
      pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (uint32)i);
      if(pDlgItem == NULL)
      {
        DIALER_ERR("Null Pointer", 0, 0, 0);
      }
      else
      {
        FREEIF(pDlgItem->pDlgInfo);
        FREE(pDlgItem);
      }
    }
  }

  IVector_RemoveAllElements(pMe->m_pDialogStack);

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
  if (CDialerApp_IsAnyUSBCarkitConnected())
  {
    CDialerApp_PrintToCarkit(NULL);
  }

  pMe->m_DispState = DIALER_DISP_STATE_NONE;
#endif

  //CDialerApp_PrintDlgStack(pMe);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_EndDlg

DESCRIPTION: Ends the active dialog unconditionally.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean : TRUE if the successfull

COMMENTS: 

SIDE EFFECTS: App will stop only one dialog is in the stack.

SEE ALSO:

=============================================================================*/
boolean CDialerApp_EndDlg(void *pUser)
{
  CDialerApp* pMe = (CDialerApp*) pUser;
  IDialog* pIDialog = NULL;
  uint16 wDlgID;
  uint32 dwStackSize;
  CDlgItem *pDlgItem = NULL;

  PRINT_FUNCTION_NAME();
  if (pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIDialog == NULL) 
  { 
    DIALER_ERR("NULL pointer", 0, 0, 0);
    return FALSE;
  }

  wDlgID = IDIALOG_GetID(pIDialog);
  if (wDlgID == 0) 
  { 
    DIALER_ERR("Bad Value", 0, 0, 0);
    return FALSE;
  }

  // remove dialog from stack
  dwStackSize = IVector_Size(pMe->m_pDialogStack);
  if(dwStackSize == 0)
  {
    DIALER_ERR("Can't remove from empty dlg stack", 0, 0, 0);
    return FALSE;
  }

  pDlgItem = IVector_RemoveElementAt(pMe->m_pDialogStack, (dwStackSize-1));
  if(pDlgItem == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  if(pDlgItem->wDlgID != wDlgID)
  {
    DIALER_ERR("DIALOG STACK CORUPTION !!!!!", 0, 0, 0);
    (void) CDialerApp_StopApp(pMe);
    return FALSE;
  }

  // free up allocated memory
  FREEIF(pDlgItem->pDlgInfo);
  FREE(pDlgItem);

  if(CheckAEEReturnStatus(ISHELL_EndDialog(pMe->a.m_pIShell)) == FALSE)
  {
    DIALER_ERR("ISHELL_EndDialog FAILED", 0, 0, 0);
    return FALSE;
  }

  //CDialerApp_PrintDlgStack(pMe);

  if(IVector_IsEmpty(pMe->m_pDialogStack) == TRUE)
  {
    // empty dialog stack so stop app
    DIALER_MSG("Dialer will close in 250 ms unless a dialog is created", 0,0,0);
    (void) ISHELL_SetTimer(pMe->a.m_pIShell, 250, (PFNNOTIFY) CDialerApp_StopApp, (void*) pMe);
    return TRUE;
  }

  // brew should not have an active dialog ... only one brew dialog is on the stack
  // at anytime and we closed that one above
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog != NULL) 
  { 
    DIALER_ERR("DIALOG STACK CORUPTION !!!!!", 0, 0, 0);
    (void) CDialerApp_StopApp(pMe);
    return FALSE;    
  }

  // brew stack is empty but ours is not so create next dialog
  pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (IVector_Size(pMe->m_pDialogStack) - 1));
  if(pDlgItem == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // check to see if the status dialog should be displayed or app should be ended
  if(pDlgItem->wDlgID == IDD_STATUS)
  {
    if(CDialerApp_IsCallsToDisplay(pMe) == FALSE)
    { 
      if(pMe->m_bIconStartedApp == FALSE)
      {
        // set timer to close the dialer app
        DIALER_MSG("Dialer will close in 250 ms unless a dialog is created", 0,0,0);
        (void) ISHELL_SetTimer(pMe->a.m_pIShell, 250, (PFNNOTIFY) CDialerApp_StopApp, (void*) pMe);
        return TRUE;
      }
    }
  }

  // create the correct dialog in brew
  if(CheckAEEReturnStatus(ISHELL_CreateDialog(pMe->a.m_pIShell, DIALERAPP_RES_FILE, pDlgItem->wDlgID, NULL)) == FALSE)
  {
    DIALER_ERR("ISHELL_CreateDialog FAILED", 0, 0, 0);
    return FALSE;
  }

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_EndDlgEx

DESCRIPTION: Ends or removes the first occurance of the specified dialog.

PARAMETERS:
  *pUser: CDialerApp object pointer
  wDlgID: ID of dialog to end

RETURN VALUE:
  boolean : TRUE if the successfull

COMMENTS: 

SIDE EFFECTS: App will stop only one dialog is in the stack.

SEE ALSO:

=============================================================================*/
boolean CDialerApp_EndDlgEx(void *pUser, uint16 wDlgID)
{
  CDialerApp* pMe = (CDialerApp*) pUser;
  uint32 dwStackSize;
  CDlgItem *pDlgItem = NULL;
  int i;

  PRINT_FUNCTION_NAME();
  if (pMe == NULL) 
  { 
    // error checking 
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  if(CDialerApp_GetActiveDlgID(pMe) == wDlgID)
  {
    // end the active dialog so call regular EndDlg
    return CDialerApp_EndDlg(pMe);
  }

  // remove dialog from stack
  dwStackSize = IVector_Size(pMe->m_pDialogStack);
  if(dwStackSize == 0)
  {
    DIALER_MSG("Can't remove from empty dlg stack", 0, 0, 0);
    return FALSE;
  }
  
  // search for the dialog in the stack
  for(i=(int)(dwStackSize-1); i >=0; i--)
  {
    pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (uint32)i);
    if(pDlgItem == NULL)
    {
      DIALER_ERR("Null Pointer", 0, 0, 0);
      return FALSE;
    }

    // check to see if the we are have the dialog to remove
    if(pDlgItem->wDlgID == wDlgID)
    {
      // remove the dialog
      pDlgItem = IVector_RemoveElementAt(pMe->m_pDialogStack, (uint32)i);
      if(pDlgItem == NULL)
      {
        DIALER_ERR("Null Pointer", 0, 0, 0);
        return FALSE;
      }

      // free up allocated memory
      FREEIF(pDlgItem->pDlgInfo);
      FREE(pDlgItem);

      // we are done since se we found the dialog
      //CDialerApp_PrintDlgStack(pMe);
      return TRUE;
    }
  }

  return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_CreateDlg

DESCRIPTION: Create the dialog passed in but make checks for priority of dialogs

PARAMETERS:
  *pUser: CDialerApp object pointer
  wCreateDlgID: dialog to create
  *pInfo: info to be added to the dialog 
  dwInfoSize: size of info object

RETURN VALUE:
  boolean: TRUE if SUCCESS in creating the Dialog

COMMENTS: pInfo may be NULL and dwInfoSize equal to zero if there is no
          additional info for the dialog.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CreateDlg(void *pUser, uint16 wCreateDlgID, void *pInfo, uint32 dwInfoSize)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog *pIDialog= NULL;
  uint16 wActiveDlgID;
  CDlgItem *pDlgItem = NULL;
  CDlgItem *pDlgAddItem= NULL;
  void* pDlgInfo = NULL;
  int nStackPosToInsert;
  int nStackSize;

  PRINT_FUNCTION_NAME();
  // param checking
  if(pMe == NULL)
  { 
    // error checking
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  if((wCreateDlgID == 0) || ((pInfo == NULL) && (dwInfoSize != 0)) || ((pInfo != NULL) && (dwInfoSize == 0)))
  {
    DIALER_ERR("Bad Parameters", 0, 0, 0);
    return FALSE;
  }

  // don't want to display an empty message queue
  if((wCreateDlgID == IDD_NOTIF) && (IVector_IsEmpty(pMe->m_pNotifMsgQueue) == TRUE))
  {
    DIALER_ERR("Must use DisplayMsg to start Notif Dialog", 0, 0, 0);
    return FALSE;
  }

  // check to see if we are suspended
  if(pMe->m_bSuspended == TRUE || pMe->m_bAppBeingSuspended == TRUE)
  {
    // store the startup dialog info
    if((pInfo != NULL) && (dwInfoSize != 0))
    {
      // this will be freed after use later
      pMe->m_pStartupDlgInfo = (void*) MALLOC(dwInfoSize);
      if(pMe->m_pStartupDlgInfo == NULL)
      {
        DIALER_ERR("No Memory", 0, 0, 0);
        return FALSE;
      }

      // copy over the info needed to start later
      (void) MEMCPY(pMe->m_pStartupDlgInfo, pInfo, dwInfoSize);
      pMe->m_dwStartupDlgInfoSize = dwInfoSize;
    }

    // save the dlg ID so it can be started later
    pMe->m_wStartupDlgID = wCreateDlgID;

    return(CheckAEEReturnStatus(ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_DIALER)));
  }

  // cancel shutdown timer
  (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_StopApp, (void*) pMe);

  pIDialog = CDialerApp_GetActiveDlg(pMe);
  if(pIDialog == NULL) 
  { 
    wActiveDlgID = 0;
  }
  else
  {
    wActiveDlgID = CDialerApp_GetActiveDlgID(pMe);
    if(wActiveDlgID == 0) 
    { 
      DIALER_ERR("Bad Value", 0, 0, 0);
      return FALSE;
    }
  }

  // construct element to add to dialog stack
  // allocate memory for objects to be placed in stack
  pDlgAddItem = (CDlgItem*) MALLOC(sizeof(CDlgItem));
  if(pDlgAddItem == NULL)
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    return FALSE;
  }

  // allocate memory for info and copy data
  if((pInfo != NULL) && (dwInfoSize != 0))
  {
    pDlgInfo = (void*) MALLOC(dwInfoSize);
    if(pDlgInfo == NULL)
    {
      DIALER_ERR("No Memory", 0, 0, 0);
      FREEIF(pDlgAddItem);
      return FALSE;
    }

    // copy over the info
    (void) MEMCPY(pDlgInfo, pInfo, dwInfoSize);
    pDlgAddItem->pDlgInfo = pDlgInfo;
  }
  else
  {
    // no info for this dialog
    pDlgAddItem->pDlgInfo = NULL; 
  }
  pDlgAddItem->wDlgID = wCreateDlgID;  
  pDlgAddItem->dwInfoSize = dwInfoSize;

  // get dlg stack size
  nStackSize = (int) IVector_Size(pMe->m_pDialogStack);

  // check to see if dialog stacks are empty
  if((nStackSize == 0) && (wActiveDlgID == 0))
  {
    // create new dialog since stack is empty and insert at the top of stack
    nStackPosToInsert = -1;
  }
  else if(((nStackSize == 0) && (wActiveDlgID != 0)) ||
          ((nStackSize != 0) && (wActiveDlgID == 0)))
  {
    // this is bad because brew and dialer stack are out of sync
    DIALER_ERR("DIALOG STACK CORUPTION !!!!!", 0, 0, 0);
    if(pDlgAddItem != NULL)
    {
      FREEIF(pDlgAddItem->pDlgInfo);
      FREE(pDlgAddItem);
    }
    (void) CDialerApp_StopApp(pMe);
    return FALSE;
  }
  else
  { 
    int i = nStackSize;
    const CDlgPriority *pDlgToCreate;
    const CDlgPriority *pDlgInStack;
    boolean bFoundPos = FALSE;
 
    // travese the dialog stack and decide what the location is
    while((bFoundPos == FALSE) && (i >= 0))
    {
      // go to the next index in stack
      i--;

      if(i == -1)
      {
        // we are at the bottom so dlg should go at the bottom of stack
        bFoundPos = TRUE;
        break;
      }

      // get the dialog at i postition in the stack
      pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (uint32)i);
      if(pDlgItem == NULL)
      {
        DIALER_ERR("Null Pointer", 0, 0, 0);
        if(pDlgAddItem != NULL)
        {
          FREEIF(pDlgAddItem->pDlgInfo);
          FREE(pDlgAddItem);
        }
        (void) CDialerApp_StopApp(pMe);
        return FALSE;
      }

      pDlgToCreate = &DlgPriority[(uint32)(wCreateDlgID-DIALERAPP_DIALOG_OFFSET)];
      pDlgInStack = &DlgPriority[(uint32)(pDlgItem->wDlgID-DIALERAPP_DIALOG_OFFSET)];

      // sanity check for priority array of dialogs
      if((pDlgToCreate->wDlgID != wCreateDlgID) || (pDlgInStack->wDlgID != pDlgItem->wDlgID))
      {
        // this is a fail safe check to make sure that the priority dlg IDs
        // match up with each dialog ID
        ERR_FATAL("Dialog Priority ID Mismatch", 0, 0, 0);  
      }

      if(pDlgToCreate->wPriority < pDlgInStack->wPriority)
      {
        // insert the dialog above the current dialog since is has a higher priority
        bFoundPos = TRUE;
      }
      else if((pDlgInStack->wDlgID == IDD_NUMEDIT) && (pDlgToCreate->wDlgID != IDD_STATUS))
      {
        // special case for IDD_NUMEDIT that allows any dialog except IDD_STATUS to be placed
        // above it
        bFoundPos = TRUE;
      }
      else if(pDlgToCreate->wPriority == pDlgInStack->wPriority)
      {
        // check to see if they are same dialog
        if(pDlgToCreate->wDlgID == pDlgInStack->wDlgID)
        {
          // check to see if duplicates are allowed for that dialog
          if(pDlgToCreate->bDuplicates == FALSE)
          {
            // update the info on the stack and refresh the screen
            FREEIF(pDlgItem->pDlgInfo);
            pDlgItem->pDlgInfo = pDlgAddItem->pDlgInfo;
            pDlgItem->dwInfoSize = pDlgAddItem->dwInfoSize;
            FREEIF(pDlgAddItem);
            if(wActiveDlgID == pDlgToCreate->wDlgID)
            {
              // refresh the dialog since it is showing
              return CDialerApp_RefreshDlg(pMe);
            }

            // dialog info was updated so return TRUE and do not free the pDlgInfo
            return TRUE;
          }
          // else duplicates can exist so we will place it under the current
          // continue in while loop and go to the next dialog down in the stack
        }
        else
        {
          // insert the dialog above the current dialog since priority is equal
          bFoundPos = TRUE;
        }
      }
      // pDlgToCreate->wPriority > pDlgInStack->wPriority
      // continue in while loop and go to the next dialog down in the stack

    }

    nStackPosToInsert = i+1;
    if(nStackPosToInsert == nStackSize)
    {
      // dlg is placed on the top so we have to have brew create it
      nStackPosToInsert = -1;
    }
  } // end of while

  if(nStackPosToInsert == -1)
  {
    // add dialog to the the top of stack
    if(CheckAEEReturnStatus(IVector_AddElement(pMe->m_pDialogStack, pDlgAddItem)) == FALSE)
    {
      DIALER_ERR("IVector_AddElement failed for dialog %d", wCreateDlgID, 0, 0);
      FREEIF(pDlgAddItem);
      FREEIF(pDlgInfo);
      return FALSE;
    }

    if(ISHELL_GetActiveDialog(pMe->a.m_pIShell) != NULL)
    {
      // close the brew dialog 
      // we only want one dialog open at a time in brew
      (void) ISHELL_EndDialog(pMe->a.m_pIShell);
    }

    if(CheckAEEReturnStatus(ISHELL_CreateDialog(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
      wCreateDlgID, NULL)) == FALSE)
    {
      DIALER_ERR("Could not create dialog %d", wCreateDlgID, 0, 0);
      //The pDlgAddItem may no longer be at the top of the stack.
      (void) IVector_RemoveElement(pMe->m_pDialogStack, pDlgAddItem);
      FREEIF(pDlgAddItem);
      FREEIF(pDlgInfo);
      return FALSE;
    } 
  }
  else
  {
    // add dialog to the given position
    if(CheckAEEReturnStatus(IVector_InsertElementAt(pMe->m_pDialogStack, pDlgAddItem, (uint32)nStackPosToInsert)) == FALSE)
    {
      DIALER_ERR("IVector_InsertElementAt failed for dialog %d", wCreateDlgID, 0, 0);
      FREEIF(pDlgAddItem);
      FREEIF(pDlgInfo);
      return FALSE;
    } 
    pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (uint32)nStackSize);
    if(pDlgItem->wDlgID == IDD_NOTIF)
    {
      CDialerApp_NotifDlg_CloseDlg(pMe);
    }
  

    // no need to have brew create it since it is not showing
  }

  //CDialerApp_PrintDlgStack(pMe);

  // do not free the pDlgItem here, it will be done on removal from stack
  return TRUE;
} 


/*=============================================================================
FUNCTION: CDialerApp_GetActiveDlg

DESCRIPTION: Returns pointer to the active dialog object.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  IDialog*: pointer to the active dialog object or NULL if error occurs

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
IDialog* CDialerApp_GetActiveDlg(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog *pActiveDlg = NULL;
  uint16 wActiveDlgID;
  CDlgItem *pDlgItem = NULL;

  PRINT_FUNCTION_NAME();
  if(pMe == NULL)
  { 
    // error checking
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return NULL;
  }

  pActiveDlg = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pActiveDlg == NULL)
  {
    return NULL;
  }

  wActiveDlgID = IDIALOG_GetID(pActiveDlg);

  pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (IVector_Size(pMe->m_pDialogStack)-1));
  if(pDlgItem == NULL)
  {
    return NULL;
  }

  if(pDlgItem->wDlgID != wActiveDlgID)
  {
    DIALER_ERR("DIALOG STACK CORUPTION !!!!!", 0, 0, 0);
    (void) CDialerApp_StopApp(pMe);
    return NULL;
  }

  return pActiveDlg;
}

/*=============================================================================
FUNCTION: CDialerApp_GetActiveDlgID 

DESCRIPTION: Prints the dlg stack.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  uint16: active dialog ID or 0 if an error occurs

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
uint16 CDialerApp_GetActiveDlgID(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog *pActiveDlg = NULL;
  uint16 wActiveDlgID;
  CDlgItem *pDlgItem = NULL;

  PRINT_FUNCTION_NAME();
  if(pMe == NULL)
  { 
    // error checking
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return 0;
  }

  pActiveDlg = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pActiveDlg == NULL)
  {
    return 0;
  }

  wActiveDlgID = IDIALOG_GetID(pActiveDlg);

  pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (IVector_Size(pMe->m_pDialogStack)-1));
  if(pDlgItem == NULL)
  {
    return 0;
  }

  if(pDlgItem->wDlgID != wActiveDlgID)
  {
    DIALER_ERR("DIALOG STACK CORUPTION !!!!!", 0, 0, 0);
    (void) CDialerApp_StopApp(pMe);
    return 0;
  }

  return pDlgItem->wDlgID;
}

/*=============================================================================
FUNCTION: CDialerApp_GetActiveDlgInfo

DESCRIPTION: Prints the dlg stack.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  void*: pointer to the active dialogs info structure and NULL if error occurs

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
void* CDialerApp_GetActiveDlgInfo(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog *pActiveDlg = NULL;
  uint16 wActiveDlgID;
  CDlgItem *pDlgItem = NULL;

  PRINT_FUNCTION_NAME();
  if(pMe == NULL)
  { 
    // error checking
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return NULL;
  }

  pActiveDlg = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pActiveDlg == NULL)
  {
    return NULL;
  }

  wActiveDlgID = IDIALOG_GetID(pActiveDlg);

  pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (IVector_Size(pMe->m_pDialogStack)-1));
  if(pDlgItem == NULL)
  {
    return NULL;
  }

  if(pDlgItem->wDlgID != wActiveDlgID)
  {
    DIALER_ERR("DIALOG STACK CORUPTION !!!!!", 0, 0, 0);
    (void) CDialerApp_StopApp(pMe);
    return NULL;
  }

  return pDlgItem->pDlgInfo;
}


/*=============================================================================
FUNCTION: CDialerApp_HandleProgressInfo

DESCRIPTION: Handles the Information sent by ICM in PROGESS_INFO

PARAMETERS:
  *pUser: CDialerApp object pointer
  callID: The current call ID

RETURN VALUE:
  void

COMMENTS: 

SIDE EFFECTS: Will play the RingBack tone in GSM/WCDMA and VoIP

SEE ALSO:

=============================================================================*/
void CDialerApp_HandleProgressInfo(void *pUser, uint8 callId)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  
  DIALER_MSG("Entry CDialerApp_HandleProgressInfo %d", callId, 0, 0);
  
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, callId, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE)
  {
    return;
  }

  switch( pMe->m_CallInfo.call_progress )
  {
   case AEECM_PROGIND_USER_RESUMED:
   /* check for calls in active/held state before
      updating the device status
   */
     DIALER_MSG("CDialerApp_HandleProgressInfo User Resumed", 0, 0, 0);

     if(AEECM_IS_CALLTYPE_VOICE(pMe->m_CallInfo.call_type) == TRUE)
     {
        CDialerApp_SetVocoder(pMe,TRUE);
     }
     break;

   case AEECM_PROGIND_REMOTE_ALERT:
   case AEECM_PROGIND_IP_REMOTE_ALERT:
     DIALER_MSG("CDialerApp_HandleProgressInfo REMOTE_ALERT", 0, 0, 0);
     /* no inband information available */
     /*  generate a local ringback tone */
#ifdef FEATURE_CEPT
     uisnd_snd(UI_CALL_EAR_SND, (uint16)SND_CEPT_RING);
#else
     uisnd_snd(UI_CALL_EAR_SND, (uint16)SND_RING_BACK);
#endif /* FEATURE_CEPT */
   break;

   case AEECM_PROGIND_BEARER_CHANGED:
     /* Lower layers reconfigure the vocoder */
     DIALER_MSG("Progress Info Ind - BEARER_CHANGED", 0, 0, 0);
   break;

   case AEECM_PROGIND_USER_SUSPENDED:
     DIALER_MSG("Progress Info Ind - USER_SUSPENDED", 0, 0, 0);
   break;

   case AEECM_PROGIND_IP_INFO_NONE:
     DIALER_MSG("Progress Info Ind - IP_INFO_NONE", 0, 0, 0);
   break;

   default:
      MSG_ERROR( "Invalid Call Progress Indicator",0,0,0);
   break;
  }
}

/*=============================================================================
FUNCTION: CDialerApp_HandleNSSRelRecord

DESCRIPTION: The NSS Release Record handling is done by this function.
             The requirement is to release the call when we see this record

PARAMETERS:
  *pUser: CDialerApp object pointer
   uint8: Call Id of the current call

RETURN VALUE:
   boolean: TRUE if the call was ended FALSE otherwise
COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
boolean CDialerApp_HandleNSSRelRecord(void *pUser, uint8 callId)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  int nRetVal  = EFAILED;

  nRetVal = ICM_EndCall(pMe->m_pICM, callId);
  DIALER_MSG("Ended call due to NSS Release Record", 0, 0, 0);
  if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
  {
    (void) CDialerApp_EndCallRelatedDlgs(pMe, callId);
    DIALER_ERR("ICM_EndCall FAILED in NSS Release Info Handling", 0, 0, 0);  
    return FALSE;
  }
    
  ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
  return TRUE;
}

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
/*=============================================================================
FUNCTION: CDialerApp_GetSecText

DESCRIPTION: Gets the static control for the secondary display.  Creates
  the control if it does not already exist.

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean:  TRUE if the text pointer is now available; FALSE otherwise

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
boolean CDialerApp_GetSecText(CDialerApp *pMe)
{
  int nCreateStatus;
  AEERect  sStaticRect;        // Area where the Static object will display
  AEEDeviceInfo sDevInfo;      // Includes size of secondary LCD

  if (NULL != pMe->m_pSecText)
  {
    // if the pointer already exists, just return TRUE
    return TRUE;
  }

  if (NULL == pMe->m_pIDisplay2)
  {
    // If we don't have a secondary display, return FALSE
    return FALSE;
  }

  // Create the static text object
  IDISPLAY_MakeDefault(pMe->m_pIDisplay2);
  IDISPLAY_SetColor(pMe->m_pIDisplay2, CLR_USER_BACKGROUND, RGB_WHITE);

  // Create Static control for secondary display text
  nCreateStatus = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC,
                                        (void **)&pMe->m_pSecText);
  IDISPLAY_MakeDefault(pMe->a.m_pIDisplay);
  if ((CheckAEEReturnStatus(nCreateStatus) == FALSE) ||
      (pMe->m_pSecText == NULL))
  {
     MSG_ERROR("Unable to create secondary text static control: %d",
               nCreateStatus, 0, 0);
     pMe->m_pSecText = NULL;
     return FALSE;
  }
  else
  {
    /* We want to center the text in the middle of the screen, so
     * we need to set its display rectangle correctly.
     */
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &sDevInfo);
    sStaticRect.x = 0;
    sStaticRect.y = 0;
    sStaticRect.dx = (int16)(sDevInfo.cxAltScreen);
    sStaticRect.dy = (int16)(sDevInfo.cyAltScreen);
    ISTATIC_SetRect(pMe->m_pSecText, &sStaticRect);
    ISTATIC_SetProperties(pMe->m_pSecText, ST_CENTERTEXT | ST_MIDDLETEXT);
  }

  return TRUE;

}

/*=============================================================================
FUNCTION: CDialerApp_UpdateSecondaryDisp

DESCRIPTION: Updates the secondary display based on the inputs

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  None

COMMENTS:
  This routine updates the display based on the m_DispState member of the
  Dialer app structure.  As a result, that member should be updated as
  necessary before invoking this function.

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
void CDialerApp_UpdateSecondaryDisp(CDialerApp *pMe)
{

  AECHAR      *pszSecText;               // Text to display on secondary
  AECHAR      *pszScratch;               // Scratchpad for constructing text
  int          nStrlen;                  // Scratch strlen value

  if (!pMe)
  {
    DIALER_ERR("NULL pMe", 0, 0, 0);
    return;
  }

  pszSecText = (AECHAR *)MALLOC((DIALERAPP_MAX_SMALL_STR_SIZE) *
                                sizeof(AECHAR));
  pszScratch = (AECHAR *)MALLOC((DIALERAPP_MAX_SMALL_STR_SIZE) *
                                  sizeof(AECHAR));
  if ((NULL == pszSecText) || (NULL == pszScratch))
  {
    FREEIF(pszSecText);
    FREEIF(pszScratch);
    DIALER_ERR("No memory for secondary display strings", 0, 0, 0);
    return;
  }

  switch (pMe->m_DispState)
  {
    case DIALER_DISP_STATE_INCOM:

      // Load the "Calling" string
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                                 IDS_CALLING, pszScratch,
                                 (DIALERAPP_MAX_SMALL_STR_SIZE *
                                  sizeof(AECHAR)));

      // Reserve room in the display string for a space and "calling" string
      nStrlen = DIALERAPP_MAX_SMALL_STR_SIZE - WSTRLEN(pszScratch) - 1;

      // Copy in the caller info
      CDialerApp_FormatNumberDisplayString(pMe, &(pMe->m_CallInfo),
                                           pszSecText, (uint32)nStrlen,
                                           pMe->m_bNVAutoHyphen);

      // Add a space and new null terminator
      nStrlen = WSTRLEN(pszSecText);
      pszSecText[nStrlen++] = (AECHAR)' ';
      pszSecText[nStrlen] = (AECHAR)'\0';

      // Now, concatenate "calling" string
      WSTRLCAT((void *)pszSecText, (void *)pszScratch,
               DIALERAPP_MAX_SMALL_STR_SIZE);

      // String has been constructed, now display it
      if (pMe->m_pIDisplay2 && CDialerApp_GetSecText(pMe))
      {
        ISTATIC_SetText(pMe->m_pSecText, NULL, pszSecText, AEE_FONT_LARGE,
                        AEE_FONT_LARGE);
        ISTATIC_Redraw(pMe->m_pSecText);
      }

      if (CDialerApp_IsAnyUSBCarkitConnected())
      {
        CDialerApp_PrintToCarkit(pszSecText);
      }
      break;

    case DIALER_DISP_STATE_ORIG:
      // Load the "Calling" string
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                                 IDS_CALLING, pszSecText,
                                 (DIALERAPP_MAX_SMALL_STR_SIZE *
                                  sizeof(AECHAR)));

      // Add a space and new null terminator
      nStrlen = WSTRLEN(pszSecText);
      pszSecText[nStrlen++] = (AECHAR)' ';
      pszSecText[nStrlen] = (AECHAR)'\0';

      // Calculate the amount of room left
      nStrlen = DIALERAPP_MAX_SMALL_STR_SIZE - WSTRLEN(pszSecText);

      // Copy in the caller info
      CDialerApp_FormatNumberDisplayString(pMe, &(pMe->m_CallInfo),
                                           pszScratch, (uint32)nStrlen,
                                           pMe->m_bNVAutoHyphen);

      // Now, concatenate formatted number
      WSTRLCAT((void *)pszSecText, (void *)pszScratch,
               DIALERAPP_MAX_SMALL_STR_SIZE);

      // String has been constructed, now display it
      if (pMe->m_pIDisplay2 && CDialerApp_GetSecText(pMe))
      {
        ISTATIC_SetText(pMe->m_pSecText, NULL, pszSecText, AEE_FONT_LARGE,
                        AEE_FONT_LARGE);
        ISTATIC_Redraw(pMe->m_pSecText);
      }

      if (CDialerApp_IsAnyUSBCarkitConnected())
      {
        CDialerApp_PrintToCarkit(pszSecText);
      }
      break;

    case DIALER_DISP_STATE_MISSEDCALL:
      // Missed call notifications are handled by the missed call dialog
      break;

    case DIALER_DISP_STATE_STATUS:
      // Status updates are handled via the callback
      break;

    case DIALER_DISP_STATE_NONE:
      break;

    default:
      break;

  }

  FREEIF(pszSecText);
  FREEIF(pszScratch);
  return;
  
}

/*=============================================================================
FUNCTION: CDialerApp_PrintToCarkit

DESCRIPTION: Updates the carkit display based on the string passed in

PARAMETERS:
  *pszText: pointer to the new text to put on the display.  NULL clears the 
            display.

RETURN VALUE:
  None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_PrintToCarkit(AECHAR *pszText)
{
#ifdef FEATURE_USB_CARKIT
  char *text = NULL;
  int len = 0;

  if (CDialerApp_IsAnyUSBCarkitConnected())
  {
    if (pszText) 
    {
      len = WSTRLEN(pszText);
      len++; //for NULL

      text = MALLOC(len);
      if (text == NULL)
      {
        DIALER_ERR("Could not alloc text", 0, 0, 0);
      }

      (void)WSTRTOSTR(pszText, text, len);
    }

    usb_otg_cc_set_carkit_display_panel(text, len);

    FREEIF(text);
  }
#endif
}
#endif //FEATURE_UI_SECONDARY_DISP_SUPPORT

/*=============================================================================
FUNCTION: CDialerApp_SetVocoder

DESCRIPTION: Sets the vocoder on/off.

PARAMETERS:
  *pUser: CDialerApp object pointer
   bVocoder : Set the vocoder on/off

RETURN VALUE:
  NONE

COMMENTS: 

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
void CDialerApp_SetVocoder(void *pUser, boolean bVocoder)
{
  CDialerApp* pMe = (CDialerApp*)pUser;
  
  if(pMe == NULL)
    return;

  if(bVocoder)
  {
    // If vocoder is already on, do not do anything
    if(!pMe->m_bVocoder)
    {
      uisnd_set_mute(UISND_MUTE_UNMUTED, UISND_MUTE_UNMUTED, NULL, NULL);
      pMe->m_bMuted = FALSE;
      pMe->m_bVocoder = TRUE;
    }
  }
  else
  {
    // If vocoder is on, turn it off. Also the mute flag should be turned off.
    if(pMe->m_bVocoder)
    {
      uisnd_set_mute(UISND_MUTE_MUTED, UISND_MUTE_MUTED, NULL, NULL);
      pMe->m_bMuted = TRUE;
      pMe->m_bVocoder = FALSE;
      }
  
  }


}

/*=============================================================================
FUNCTION: CDialerApp_SetMute

DESCRIPTION: Sets the mic on/off.

PARAMETERS:
  *pUser: CDialerApp object pointer
   bMute : Set the mic on/off

RETURN VALUE:
  NONE

COMMENTS:  pMe->m_bMuted is the user setting. It can be off/on when the call is up
           If there is no call, the vocoder should be off so the flag should always 
           be FALSE
    

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
void CDialerApp_SetMute(void *pUser, boolean bMute)
{
  
  CDialerApp* pMe = (CDialerApp*)pUser;
  
  if(pMe == NULL)
    return;

 
  if(bMute)
  {
    // Mute the phone if the vocoder is on and the call is not already muted
    if(!pMe->m_bMuted && pMe->m_bVocoder)
    {
      uisnd_set_mute(UISND_MUTE_UNMUTED, UISND_MUTE_MUTED, NULL, NULL);
      pMe->m_bMuted = TRUE;
    }
  }
  else
  {
    // Unmute the phone if the vocoder is on and call is still up.
    if(pMe->m_bMuted && pMe->m_bVocoder)
    {
      uisnd_set_mute(UISND_MUTE_UNMUTED, UISND_MUTE_UNMUTED, NULL, NULL);
      pMe->m_bMuted = FALSE;
    }
  }
}

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
/*=============================================================================
FUNCTION: CDialerApp_IsAnyUSBCarkitConnected

DESCRIPTION: Checks the Config to see if there is a USB carkit connected

PARAMETERS:
  
RETURN VALUE:
  boolean: TRUE if there is a carkit connected

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IsAnyUSBCarkitConnected(void)
{
  boolean hfk = FALSE;

  IConfig *pIConfig = NULL;
  IShell *shell_ptr = AEE_GetShell();

  if (shell_ptr == NULL) {
    MSG_ERROR("Shell ptr null", 0, 0, 0);
    return FALSE;
  }

  ISHELL_CreateInstance(shell_ptr, AEECLSID_CONFIG, (void **) &pIConfig);

  if (pIConfig) 
  {
    ICONFIG_GetItem(pIConfig, CFGI_USB_HFK_ATTACHED, &hfk, 1);

    //if no regular carkit, try stereo
    if (!hfk)
    {
      ICONFIG_GetItem(pIConfig, CFGI_STEREO_USB_HFK_ATTACHED, &hfk, 1);
    }

    ICONFIG_Release(pIConfig);
  } 
  else 
  {
    DIALER_MSG("Cant create IConfig", 0, 0, 0);  
  }

  return  hfk;
}
#endif  //FEATURE_UI_SECONDARY_DISP_SUPPORT

/*=============================================================================
FUNCTION: CDialerApp_PrintDlgStack

DESCRIPTION: Prints the dlg stack.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  NONE

COMMENTS: Used for debug purposes

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
//static void CDialerApp_PrintDlgStack(void *pUser)
//{
//  CDialerApp *pMe = (CDialerApp*) pUser;
//  CDlgItem *pDlgItem = NULL;
//  int i;
//  uint32 dwSize;
//
//  PRINT_FUNCTION_NAME();
//  if(pMe == NULL)
//  { 
//    // error checking
//    DIALER_ERR("Null Pointer", 0, 0, 0);
//    return;
//  }
//
//  dwSize = IVector_Size(pMe->m_pDialogStack);
//
//  MSG_HIGH("*********************************", 0, 0, 0);
//  if(dwSize == 0)
//  {
//    MSG_HIGH("STACK EMPTY", 0, 0, 0);
//  }
//  else
//  {
//    for(i=(int)(dwSize-1); i >= 0; i--)
//    {
//      pDlgItem = IVector_ElementAt(pMe->m_pDialogStack, (uint32)i);
//      if(pDlgItem == NULL)
//      {
//        DIALER_ERR("Null Pointer", 0, 0, 0);
//        return;
//      }
//
//      switch(pDlgItem->wDlgID)
//      {
//      case IDD_NUMEDIT: 
//        MSG_HIGH("%d: IDD_NUMEDIT - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_CALLING:
//        MSG_HIGH("%d: IDD_CALLING - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_SERVICE_PROGRAMMING:
//        MSG_HIGH("%d: IDD_SERVICE_PROGRAMMING - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_CALL_FORWARDING:   
//        MSG_HIGH("%d: IDD_CALL_FORWARDING - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_SUPS_OPERATION: 
//        MSG_HIGH("%d: IDD_SUPS_OPERATION - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_CF_REGISTER:
//        MSG_HIGH("%d: IDD_CF_REGISTER - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_SERVICE_GROUP: 
//        MSG_HIGH("%d: IDD_SERVICE_GROUP - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_USSD_PASSWORD: 
//        MSG_HIGH("%d: IDD_USSD_PASSWORD - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_CFNR_TIMER:  
//        MSG_HIGH("%d: IDD_CFNR_TIMER - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_USSD_INFO: 
//        MSG_HIGH("%d: IDD_USSD_INFO - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_CALL_BARRING: 
//        MSG_HIGH("%d: IDD_CALL_BARRING - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_CB_PASSWORD:  
//        MSG_HIGH("%d: IDD_CB_PASSWORD - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_CALLER_IDENTITY: 
//        MSG_HIGH("%d: IDD_CALLER_IDENTITY - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_CHANGE_PASSWORD:  
//        MSG_HIGH("%d: IDD_CHANGE_PASSWORD - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_WILDCARD:    
//        MSG_HIGH("%d: IDD_WILDCARD - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_NOTIF:
//        MSG_HIGH("%d: IDD_NOTIF - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_INCOMING: 
//        MSG_HIGH("%d: IDD_INCOMING - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_STATUS: 
//        MSG_HIGH("%d: IDD_STATUS - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_INCOM_OPTIONS: 
//        MSG_HIGH("%d: IDD_INCOM_OPTIONS - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_CALL_OPTIONS:  
//        MSG_HIGH("%d: IDD_CALL_OPTIONS - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_MISSEDCALL: 
//        MSG_HIGH("%d: IDD_MISSEDCALL - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_DETAIL:    
//        MSG_HIGH("%d: IDD_DETAIL - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_USSD_PROMPT: 
//        MSG_HIGH("%d: IDD_USSD_PROMPT - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_TIMEPAUSE:
//        MSG_HIGH("%d: IDD_TIMEPAUSE - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_FORWARDING:
//        MSG_HIGH("%d: IDD_FORWARDING - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      case IDD_SSREQUEST:
//        MSG_HIGH("%d: IDD_SSREQUEST - %d", i, pDlgItem->dwInfoSize, 0);
//        break;
//
//      default:
//        ERR("UNKNOWN DIALOG:%d", pDlgItem->wDlgID, 0, 0);
//        break;
//      }
//    }
//  } 
//  MSG_HIGH("*********************************", 0, 0, 0);
//}

//lint -restore 

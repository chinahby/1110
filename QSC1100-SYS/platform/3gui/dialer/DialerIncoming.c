/*=============================================================================

FILE: DialerIncoming.c

SERVICES: Dialer Applet Dialogs

GENERAL DESCRIPTION: Incoming Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerIncoming.c#42 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
08/02/07   cvs     Featurize all secondary display support
07/12/06   cvs     Fix compile warnings
10/14/05   cvs     support for attaching carkit display mid call
10/12/05   cvs     Carkit display
06/28/05   cvs     Change answer softkey to post the SEND key
09/03/04   SUN     Brew 3.1 related changes
04/05/04   BPW     Alert changes for failed calls
01/22/04   BPW     Optimized NV gets
01/08/04   BPW     Use own dialer hyphenation function
01/07/03   BPW     Give call number to ICM for a forward incoming call request
12/18/03   BPW     Added base station signalling support
12/10/03   BPW     Fixed forwarding dialog
12/04/03   BPW     Changed when transfer menu option is visible
12/04/03   BPW     Fixed compiler errors
12/03/03   BPW     Simplification of call option menu items
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/25/03   BPW     Linted for the Saber 6250 build
11/21/03   BPW     Fixed display for CDMA call waiting
11/13/03   BPW     Added function to format display for phone number
11/13/03   BPW     Press and hold clear to clear contents of text control
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
07/24/03   BW      Created DialerIncoming Source File 

=============================================================================*/
//lint -save -e611
//lint -save -e740

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "BREWVersion.h"
#include "OEMFeatures.h"
#include "AEEAppGen.h"
#include "AEEStdLib.h"
#include "AEEText.h"
#include "AEEModGen.h"
#include "AEE.h"
#include "AEEDisp.h"
#include "AEEStdLib.h"
#include "AEEShell.h"
#include "DialerApp.h"
#include "DialerIncoming.h"
#include "DialerNotif.h"
#include "dialerapp_res.h"
#include "DialerUtils.h"
#include "AppComFunc.h"
#include "ContApp.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static boolean  CDialerApp_ForwardingDlg_Forward(CDialerApp *pMe);

/*===========================================================================

STATIC and PUBLIC FUNCTIONS

===========================================================================*/
/*=============================================================================
FUNCTION: CDialerApp_IncomingDlg_Init

DESCRIPTION: Initializes the controls for Incoming dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomingDlg_Init(CDialerApp *pMe)
{
  IDialog   *pActiveDlg;
  IStatic *pIStatic;
  IMenuCtl *pISoftKeyMenu; 
  AEERect StaticRect;
  AEERect MenuRect;
  AECHAR *pszTitle = NULL;    

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    return FALSE;
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);

  // error checking
  if (pActiveDlg == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, 
    IDC_STATIC_INCOMING);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_SK_INCOMING);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // get incoming call info
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrIncomCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE)
  {
    DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);  
    return FALSE;
  }

  // init title buffer
  pszTitle = MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  if (pszTitle == NULL) 
  {
    DIALER_ERR("No memory", 0,0,0);
    return FALSE;
  }

  // load title string
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_INCOMINGCALL, pszTitle, 
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_ANSWER,
    IDL_SK_INCOMING_ANSWER, NULL, 0);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SILENCE,
      IDL_SK_INCOMING_SILENCE, NULL, 0);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_REJECT,
    IDL_SK_INCOMING_REJECT, NULL, 0);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM) 
  // this check is for the call waiting screen because no options for call waiting
  if(AEECM_IS_CALLSTATE_CONNECTED(pMe->m_CallInfo.call_state) == FALSE)
  {
    // this is an incomming call and not call waiting 
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_OPTIONS,
      IDL_SK_INCOMING_OPTIONS, NULL, 0);
  }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);


  // Set Static control size 
  ISTATIC_GetRect(pIStatic, &StaticRect);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);  
  SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, pMe->m_rc.dy-MenuRect.dy-20); 

  // Static Info Properties
  ISTATIC_SetRect(pIStatic, &StaticRect);   
  ISTATIC_SetProperties(pIStatic, ST_MIDDLETEXT | ST_UNDERLINE |ST_CENTERTITLE |ST_CENTERTEXT | ST_NOSCROLL); 
  (void) ISTATIC_SetText(pIStatic, pszTitle, NULL, AEE_FONT_BOLD, AEE_FONT_NORMAL);

  // refresh the screen
  (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_INCOMING);

  FREE(pszTitle);
  
#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
  // print incoming msg on secondary display
  pMe->m_DispState = DIALER_DISP_STATE_INCOM;
  pMe->m_bIsActiveCalls = TRUE;
  CDialerApp_UpdateSecondaryDisp(pMe);
#endif

  return CDialerApp_IncomingDlg_DisplayRefresh(pMe);
} 

/*=============================================================================
FUNCTION: CDialerApp_IncomingDlg_KeyHandler

DESCRIPTION: Handles Incoming Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomingDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
   PRINT_FUNCTION_NAME();
   // error checking
   if(pMe == NULL)
   {
     return FALSE;
   }

   if (evt == EVT_KEY)
   {
      switch(key)
      {
      case AVK_SEND:
        {
          int nRetVal = ICM_AnswerCall(pMe->m_pICM, pMe->m_byCurrIncomCall);

          if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
          {
            DIALER_ERR("ICM_AnswerCall FAILED", 0, 0, 0);  
            return FALSE;
          }

          ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
          // do not let main event handler get event
          return TRUE;
        }

      case AVK_END:
         {
          int nRetVal;

          // get incoming call info
          if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrIncomCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE)
          {
            DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);  
            return FALSE;
          }

          if(AEECM_IS_CALLSTATE_CONNECTED(pMe->m_CallInfo.call_state) == FALSE)
          {
            // only want to end incoming calls and not call waiting
            nRetVal = ICM_EndCall(pMe->m_pICM, pMe->m_byCurrIncomCall);
            if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
            {
              DIALER_ERR("ICM_EndCall FAILED", 0, 0, 0);  
              return FALSE;
            }
          }

          (void) IALERT_StopAlerting(pMe->m_pIALERT);
          ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
          // do not let main event handler handle get this event 
          return TRUE;
        }

      case AVK_CLR:
        // do not let main event handler handle event
        // we cannot leave incoming screen
        return TRUE;

      default:
        return FALSE;
      }
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_IncomingDlg_CmdHandler

DESCRIPTION: Handles Incoming Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomingDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
   PRINT_FUNCTION_NAME();
   // error checking
   if(pMe == NULL)
   {
     return FALSE;
   }

   switch (ctrl_id) 
   {
   case IDL_SK_INCOMING_SILENCE:
     (void) IALERT_StopAlerting(pMe->m_pIALERT);
     return TRUE;

   case IDL_SK_INCOMING_REJECT:
     {
       int nRetVal; 

       // get incoming call info
       if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrIncomCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE)
       {
         DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);  
         return FALSE;
       }

       if(AEECM_IS_CALLSTATE_CONNECTED(pMe->m_CallInfo.call_state) == FALSE)
       {
         // only want to end incoming calls and not call waiting
         nRetVal = ICM_EndCall(pMe->m_pICM, pMe->m_byCurrIncomCall);
         if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
         {
           DIALER_ERR("ICM_EndCall FAILED", 0, 0, 0);  
           return FALSE;
         }   
       }

       (void) IALERT_StopAlerting(pMe->m_pIALERT);
       ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
       return TRUE;
     }

   case IDL_SK_INCOMING_OPTIONS: 
     return CDialerApp_CreateDlg(pMe, IDD_INCOM_OPTIONS, NULL, 0);

   case IDL_SK_INCOMING_ANSWER:
     ISHELL_PostEventEx(pMe->a.m_pIShell, 0, 0, EVT_KEY, AVK_SEND, 0);
     return TRUE;

   default:
     return FALSE;
   }
}

/*=============================================================================
FUNCTION: CDialerApp_IncomingDlg_DisplayRefresh

DESCRIPTION: Refreshes the incoming dialog display

PARAMETERS:
  *puser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomingDlg_DisplayRefresh(void *pUser)
{
  CDialerApp* pMe = (CDialerApp*) pUser;
  IDialog   *pActiveDlg;
  IStatic *pIStatic;
  AECHAR *pszName = NULL;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
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

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_INCOMING)
  {
    DIALER_ERR("Incoming Dialog is not active", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, 
    IDC_STATIC_INCOMING);
  if(pIStatic == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  pszName = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  if((pszName == NULL))
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    return FALSE; //error
  }

  // get incoming call info
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrIncomCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE)
  {
    DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);  
    return FALSE;
  }

  // display the other party number and name if any in contacts
  if(pMe->m_CallInfo.call_state == AEECM_CALL_STATE_INCOM)
  {
    // this is an incoming call
    CDialerApp_FormatNumberDisplayString(pMe, &(pMe->m_CallInfo), pszName, DIALERAPP_MAX_STR_SIZE, pMe->m_bNVAutoHyphen);
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, FALSE); 
    if(WSTRLEN(pMe->m_CallInfo.other_party_no) != 0)
    {
      if(pMe->m_bNVAutoHyphen == TRUE)
      {
        AECHAR *pszHyphen = CDialerApp_HyphenateNumberString(pMe->m_CallInfo.other_party_no);
        if(WSTRCMP(pszHyphen, pszName) != 0)
        {
          // want the phone number if it is not null and already being displayed
          (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
          (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszHyphen, NULL, TRUE);
        }
        FREEIF(pszHyphen);
      }
      else
      {
        if(WSTRCMP(pMe->m_CallInfo.other_party_no, pszName) != 0)
        {
          // want the phone number if it is not null and already being displayed
          (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
          (void) ISTATIC_SetTextEx(pIStatic, (byte*) pMe->m_CallInfo.other_party_no, NULL, TRUE);
        }
      }
    }
  }
  else
  {
    // this is call waiting CDMA and we display things a little differently here
    if(WSTRLEN(pMe->m_pszCallWaitingNum) != 0)
    { 
      boolean bAppend = FALSE;

      // look up number in contacts
      DoNumberLookup(pMe->m_pIADDRBOOK, pMe->m_pszCallWaitingNum, pszName, (DIALERAPP_MAX_STR_SIZE * sizeof(AECHAR)));
      if(WSTRLEN(pszName) != 0)
      {   
        (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, bAppend); 
        (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
        bAppend = TRUE;     
      }

      // display number also
      if(pMe->m_bNVAutoHyphen == TRUE)
      {
        AECHAR *pszHyphenatedNum = CDialerApp_HyphenateNumberString(pMe->m_pszCallWaitingNum);
        (void) WSTRNCOPY(pszName, DIALERAPP_MAX_STR_SIZE, pszHyphenatedNum);
        FREEIF(pszHyphenatedNum);
      }
      else
      {
        (void) WSTRNCOPY(pszName, DIALERAPP_MAX_STR_SIZE, pMe->m_pszCallWaitingNum);
      }

      (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, bAppend); 
    }
    else 
    { 
      // number is empty
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_UNKNOWN, pszName, (DIALERAPP_MAX_STR_SIZE * sizeof(AECHAR)));
      (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, FALSE);  
    }
  }

  IDIALOG_Redraw(pActiveDlg);

  // free memory
  FREE(pszName);
  
  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_IncomOptionsDlg_Init

DESCRIPTION: Initializes the controls for Incoming Options dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomOptionsDlg_Init(CDialerApp *pMe)
{
  IDialog   *pActiveDlg;
  IMenuCtl *pIMenu; 

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    return FALSE;
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);

  // error checking
  if (pActiveDlg == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
                                         IDC_MENU_INCOM_OPTIONS);
  if(pIMenu == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pIMenu);
  (void) IMENUCTL_SetTitle(pIMenu, DIALERAPP_RES_FILE, IDS_INCOMOPTIONSTITLE, NULL); 

  // visible always
  // 0 SEND
  (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_BUSY,
      IDL_INCOM_OPTIONS_BUSY, NULL, 0);

  // visible if one or more active calls
  if(ICM_GetActiveCallIDs(pMe->m_pICM, 
                          (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                          AEECM_CALL_TYPE_EMERGENCY|
                                          AEECM_CALL_TYPE_NON_STD_OTASP|
                                          AEECM_CALL_TYPE_STD_OTASP), 
                          AEECM_CALL_STATE_CONV, 
                          NULL, 
                          0) > 0)
  {
    // 1 SEND
    (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_ENDACTIVE,
      IDL_INCOM_OPTIONS_ENDACTIVE, NULL, 0);
    
    // 2 SEND
    (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_SWAP,
      IDL_INCOM_OPTIONS_SWAP, NULL, 0); 
  } 

  // visible if any connected voice calls are connected
  if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM) == TRUE)
  {
    // 4 SEND
    (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_TRANSFER,
      IDL_INCOM_OPTIONS_TRANSFER, NULL, 0);
  }

  // visible always
  // 4 * Directory Number SEND
  (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_REDIRECT,
    IDL_INCOM_OPTIONS_REDIRECT, NULL, 0);

  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);
  IMENUCTL_SetActive(pIMenu, TRUE);
  (void) IMENUCTL_Redraw(pIMenu);
  
  return TRUE;
} 

/*=============================================================================
FUNCTION: CDialerApp_IncomOptionsDlg_KeyHandler

DESCRIPTION: Handles IncomOptions Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomOptionsDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
   PRINT_FUNCTION_NAME();
   // error checking
   if (pMe == NULL) 
   {
     return FALSE;
   }

   if (evt == EVT_KEY)
   {
      switch(key)
      { 
      case AVK_END:
        {
          int nRetVal = ICM_EndCall(pMe->m_pICM, pMe->m_byCurrIncomCall);

          if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
          {
            DIALER_ERR("ICM_EndCall FAILED", 0, 0, 0);  
            return FALSE;
          }
          (void) IALERT_StopAlerting(pMe->m_pIALERT);

          ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);

          // do not let main event handler handle get this event
          return TRUE;
        }

      case AVK_SEND:
        {
          int nRetVal = ICM_AnswerCall(pMe->m_pICM, pMe->m_byCurrIncomCall);

          if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
          {
            DIALER_ERR("ICM_AnswerCall FAILED", 0, 0, 0);  
            return FALSE;
          }

          ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
          // do not let main event handler get event
          return TRUE;
        }

      default:
        return FALSE;
      }
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_IncomOptionsDlg_CmdHandler

DESCRIPTION: Handles IncomOptions Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomOptionsDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
   PRINT_FUNCTION_NAME();
   // error checking
   if (pMe == NULL) 
   {
     return FALSE;
   }

   switch (ctrl_id) 
   {
   case IDL_INCOM_OPTIONS_BUSY:    
     {
       int nRetVal;
       AECHAR szStr[3];

       szStr[0] = '0';
       szStr[1] = '\0';

       nRetVal = ICM_CallOpsRequest(pMe->m_pICM, szStr, 0);
       if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
       {
         DIALER_ERR("ICM_CallOpsRequest FAILED", 0, 0, 0);  
         return FALSE;
       }

       DIALER_MSG("Sent: 0 SEND", 0, 0, 0);
       ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
       return TRUE;
     }

   case IDL_INCOM_OPTIONS_ENDACTIVE:
     {
       int nRetVal;
       AECHAR szStr[3];

       szStr[0] = '1';
       szStr[1] = '\0';

       nRetVal = ICM_CallOpsRequest(pMe->m_pICM, szStr, 0);
       if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
       {
         DIALER_ERR("ICM_CallOpsRequest FAILED", 0, 0, 0);  
         return FALSE;
       }

       DIALER_MSG("Sent: 1 SEND", 0, 0, 0);
       ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
       return TRUE;
     }

   case IDL_INCOM_OPTIONS_SWAP:    
      {
       int nRetVal;
       AECHAR szStr[3];

       szStr[0] = '2';
       szStr[1] = '\0';

       nRetVal = ICM_CallOpsRequest(pMe->m_pICM, szStr, 0);
       if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
       {
         DIALER_ERR("ICM_CallOpsRequest FAILED", 0, 0, 0);  
         return FALSE;
       }

       DIALER_MSG("Sent: 2 SEND", 0, 0, 0);
       ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
       return TRUE;
     }

   case IDL_INCOM_OPTIONS_TRANSFER:    
     {
       int nRetVal;
       AECHAR szStr[3];

       szStr[0] = '4';
       szStr[1] = '\0';

       nRetVal = ICM_CallOpsRequest(pMe->m_pICM, szStr, 0);
       if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
       {
         DIALER_ERR("ICM_CallOpsRequest FAILED", 0, 0, 0);  
         return FALSE;
       }

       DIALER_MSG("Sent: 4 SEND", 0, 0, 0);
       ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
       return TRUE;
     }

   case IDL_INCOM_OPTIONS_REDIRECT:
     (void) CDialerApp_CreateDlg(pMe, IDD_FORWARDING, NULL, 0);
     return TRUE;

   default:
     return FALSE;
   }    
}

/*=============================================================================
FUNCTION: CDialerApp_IncomOptionsDlg_DisplayRefresh

DESCRIPTION: Refreshes the incoming options dialog display

PARAMETERS:
  *puser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomOptionsDlg_DisplayRefresh(void *pUser)
{
  CDialerApp* pMe = (CDialerApp*) pUser;
  
  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_INCOM_OPTIONS)
  {
    DIALER_ERR("Incoming Options Dialog is not active", 0,0,0);
    return FALSE;
  }

  return CDialerApp_IncomOptionsDlg_Init(pMe);
}


/*=============================================================================
FUNCTION: CDialerApp_ForwardingDlg_KeyHandler

DESCRIPTION: Handles ForwardingDlg Dialog's key events

PARAMETERS:
  *pMe: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_ForwardingDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
   PRINT_FUNCTION_NAME();

   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   //Tab Control get to handle events first
   TabController_HandleEvent(&pMe->m_pTabController, evt, key);

   switch(evt)
   {
   case EVT_KEY_PRESS:
     if(key == AVK_CLR)
     {
       pMe->m_wTextCtlIDToClear = IDC_TEXT_FORWARDING;
       // press and hold clear timer
       (void) ISHELL_SetTimer(pMe->a.m_pIShell, DIALERAPP_CLEAR_TEXTCTL_TIMEOUT, 
         (PFNNOTIFY) CDialerApp_ClearTextCtl, (void*) pMe);
     }
     // give to main event handler
     return FALSE;

   case EVT_KEY_RELEASE:
     (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_ClearTextCtl, (void*) pMe);
     // give to main event handler
     return FALSE;

   case EVT_KEY:
     switch(key)
     {
     case AVK_SELECT:
       {
         // want to highlight the forward sk on select key press
         IDialog *pActiveDlg = CDialerApp_GetActiveDlg(pMe);
         ITextCtl *pITextCtl = NULL;;

         if(pActiveDlg == NULL) 
         {
           DIALER_ERR("Null pointer", 0,0,0);
           return FALSE;
         }

         pITextCtl = (ITextCtl*)IDIALOG_GetControl(pActiveDlg, 
           IDC_TEXT_FORWARDING);
         if(pITextCtl == NULL) 
         {
           DIALER_ERR("Null pointer", 0,0,0);
           return FALSE;
         }

         if(ITEXTCTL_IsActive(pITextCtl) == TRUE)
         {
           IMenuCtl *pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(
             pActiveDlg, IDC_SK_FORWARDING);

           // highlight the send sk 
           IMENUCTL_SetSel(pISoftKeyMenu, IDL_SK_FORWARDING_FORWARD); 
           (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_NUMEDIT);
           IDIALOG_Redraw(pActiveDlg);
           return TRUE;
         }

         return FALSE;
       }

     case AVK_SEND:
       (void) CDialerApp_ForwardingDlg_Forward(pMe);
       // do not let main event handler handle get this event
       return TRUE;

     case AVK_END:
       {
         int nRetVal = ICM_EndCall(pMe->m_pICM, pMe->m_byCurrIncomCall);

         if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
         {
           DIALER_ERR("ICM_EndCall FAILED", 0, 0, 0);  
           return FALSE;
         }
         (void) IALERT_StopAlerting(pMe->m_pIALERT);
         ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
         // do not let main event handler handle get this event
         return TRUE;
       }

     default:
       return FALSE;
     }
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_ForwardingDlg_CmdHandler

DESCRIPTION: Handles Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_ForwardingDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
   PRINT_FUNCTION_NAME();

   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   //Tab Control get to handle events first
   TabController_HandleEvent(&pMe->m_pTabController, EVT_COMMAND, ctrl_id);

   switch (ctrl_id) 
   {
   case IDL_SK_FORWARDING_FORWARD:
     return CDialerApp_ForwardingDlg_Forward(pMe); 

   case IDL_SK_FORWARDING_PHONEBOOK: 
     //TOFO: Add the changes when done in contact app
     (void) CDialerApp_CheckAEEReturnStatus(pMe, EUNSUPPORTED);
     //ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_CONTACT_APP); 
     return TRUE;

   default:
     return FALSE;
   }
}

/*=============================================================================
FUNCTION: CDialerApp_ForwardingDlg_EndHandler

DESCRIPTION: Handles ForwardingDlg Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_ForwardingDlg_EndHandler(CDialerApp *pMe)
{
   PRINT_FUNCTION_NAME();

   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   //Tab Control get to handle events first
   TabController_HandleEvent(&pMe->m_pTabController, EVT_DIALOG_END, 0);

   (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_ClearTextCtl, (void*) pMe);
   return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_ForwardingDlg_Forward

DESCRIPTION: Handles the Forward SK or SEND key press

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_ForwardingDlg_Forward(CDialerApp *pMe)
{
  AECHAR szFormat[] = {'4','*','%','s','\0'};
  AECHAR *pszBuf = NULL;
  int nRetVal;
  IDialog *pActiveDlg = CDialerApp_GetActiveDlg(pMe);
  AECHAR *pszForwardNum = NULL;

  // error checking
  if (pActiveDlg == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  if(CDialerApp_TextCtlGetTextEx(&pszForwardNum, pActiveDlg, IDC_TEXT_FORWARDING) == 0)
  {
    CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_PLEASEDIAL, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
    return FALSE;
  }

  if(pszForwardNum == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
    return FALSE;
  }

  pszBuf = MALLOC ((DIALERAPP_NUMBER_BUFFER_SIZE+2)*sizeof(AECHAR));
  if (pszBuf == NULL) 
  {
    DIALER_ERR("No memory", 0,0,0);
    (void) CDialerApp_CheckAEEReturnStatus(pMe, ENOMEMORY);
    return FALSE;
  }

  WSPRINTF(pszBuf, (DIALERAPP_NUMBER_BUFFER_SIZE+2)*sizeof(AECHAR), szFormat, pszForwardNum);  
  
  nRetVal = ICM_CallOpsRequest(pMe->m_pICM, pszBuf, pMe->m_byCurrIncomCall);

  FREE(pszForwardNum);
  FREE(pszBuf);

  if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
  {
    DIALER_ERR("ICM_CallOpsRequest FAILED", 0, 0, 0);  
    return FALSE;
  }

  ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);

  DIALER_MSG("Sent: 4*<number> SEND", 0, 0, 0);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_ForwardingDlg_Init

DESCRIPTION: Initializes the controls for Forwarding dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_ForwardingDlg_Init(CDialerApp *pMe)
{
  IDialog   *pActiveDlg;
  IStatic *pIStatic;
  ITextCtl *pIText;
  IMenuCtl *pISoftKeyMenu; 
  AEERect StaticRect;
  AEERect TextRect;
  AEERect MenuRect;
  AECHAR *pszStaticTitle = NULL;
  AECHAR *pszTextTitle = NULL;
  AECHAR *pszCenterTextTitle = NULL;  
  int nFontAscent;
  int nFontDescent;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    return FALSE;
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);

  // error checking
  if (pActiveDlg == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  //Tab Control get to handle events first

  // Initialize the tab control  
  TabController_Init(&pMe->m_pTabController, 2);
  // Add the controls  
  TabController_AddControl(pMe->m_pTabController, TEXT_CTL, IDIALOG_GetControl(pActiveDlg, IDC_TEXT_FORWARDING));
  TabController_AddControl(pMe->m_pTabController, SK_CTL, IDIALOG_GetControl(pActiveDlg, IDC_SK_FORWARDING));
  // Set the initial focused control
  TabController_SetActiveEx(pMe->m_pTabController, 0);

  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, 
    IDC_STATIC_FORWARDING);
  pIText = (ITextCtl*) IDIALOG_GetControl(pActiveDlg,
    IDC_TEXT_FORWARDING);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_SK_FORWARDING);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL) || (pIText == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // init static title buffer
  pszTextTitle = MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  pszStaticTitle = MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  if((pszStaticTitle == NULL) || (pszTextTitle == NULL) )
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszStaticTitle);
    FREEIF(pszTextTitle);
    return FALSE;
  }

  // load titles
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FORWARDINCOMINGCALL, pszStaticTitle, 
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_FORWARDCALLTO, pszTextTitle, 
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));

    // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_FORWARD,
    IDL_SK_FORWARDING_FORWARD, NULL, 0);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_PHONEBOOK,
    IDL_SK_FORWARDING_PHONEBOOK, NULL, 0);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);

  // Set control rect sizes
  ISTATIC_GetRect(pIStatic, &StaticRect);
  ITEXTCTL_GetRect(pIText, &TextRect);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);
  (void) IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, &nFontAscent, &nFontDescent); 
  SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, pMe->m_rc.dy-MenuRect.dy-(2*(nFontAscent+nFontDescent)+16)-30); 
  SETAEERECT(&TextRect, 10, StaticRect.dy+StaticRect.y+10, pMe->m_rc.dx-20, (2*(nFontAscent+nFontDescent)+16)); 

  // Static Info Properties
  ISTATIC_SetRect(pIStatic, &StaticRect);   
  ISTATIC_SetProperties(pIStatic, ST_MIDDLETEXT | ST_UNDERLINE |ST_CENTERTITLE |ST_CENTERTEXT | ST_NOSCROLL); 
  (void) ISTATIC_SetText(pIStatic, pszStaticTitle, NULL, AEE_FONT_BOLD, AEE_FONT_NORMAL);
  
  // Text Ctl Propeties
  ITEXTCTL_SetMaxSize(pIText, DIALERAPP_NUMBER_BUFFER_SIZE-1);
#if MIN_BREW_VERSION(3,1)
  ITEXTCTL_SetProperties(pIText, (uint32)(TP_AUTOHYPHEN | TP_MULTILINE | 
                                    TP_FRAME | TP_FONTMORPH));
#else
  ITEXTCTL_SetProperties(pIText, (uint32)(OEM_TP_AUTOHYPHEN | TP_MULTILINE | 
                                    TP_FRAME | OEM_TP_FONTMORPH));
#endif
  ITEXTCTL_SetRect(pIText, &TextRect);
  (void) CDialerApp_CenterTitle(pMe, pszTextTitle, &pszCenterTextTitle,
                               (unsigned)(int)TextRect.dx, AEE_FONT_BOLD);
  (void) ITEXTCTL_SetTitle(pIText, NULL, 0, pszCenterTextTitle);
  ITEXTCTL_SetCursorPos(pIText, TC_CURSORSTART);
#if MIN_BREW_VERSION(3,1)
  (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_DIALED_DIGITS);
#else
  (void) ITEXTCTL_SetInputMode(pIText, OEM_TM_DIALED_DIGITS);
#endif
  (void) ITEXTCTL_SetText(pIText, NULL, 0);

  (void) IDIALOG_SetFocus(pActiveDlg, IDC_TEXT_FORWARDING);

  // free buffer memory
  FREE(pszStaticTitle);
  FREE(pszTextTitle);
  FREEIF(pszCenterTextTitle);

  return CDialerApp_ForwardingDlg_DisplayRefresh(pMe);
}

/*=============================================================================
FUNCTION: CDialerApp_ForwardingDlg_DisplayRefresh

DESCRIPTION: Refreshes the forwarding dialog display

PARAMETERS:
  *puser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_ForwardingDlg_DisplayRefresh(void *pUser)
{
  CDialerApp* pMe = (CDialerApp*) pUser; 
  IDialog   *pActiveDlg;
  IStatic *pIStatic;
  AECHAR *pszName = NULL;
  AECHAR *pszHyphenatedNum = NULL;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
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

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_FORWARDING)
  {
    DIALER_ERR("Forwarding Dialog is not active", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, 
    IDC_STATIC_FORWARDING);
  if(pIStatic == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  pszName = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  if((pszName == NULL))
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    return FALSE; //error
  }

  // get info for incoming call
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrIncomCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE)
  {
    DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);  
    return FALSE;
  }

  // Static Info Properties
  // display the other party number and name if any in contacts
  if(pMe->m_CallInfo.call_type == AEECM_CALL_TYPE_EMERGENCY)
  { // emrgency call
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_EMERGENCY, pszName, (DIALERAPP_MAX_STR_SIZE * sizeof(AECHAR)));
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, FALSE);    
  }
  else if(WSTRLEN(pMe->m_CallInfo.other_party_no) == 0)
  {
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_UNKNOWN, 
                        pszName, ((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR)));
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, FALSE); 
  }
  else
  {
    if(pMe->m_bNVAutoHyphen == TRUE)
    {
      pszHyphenatedNum = CDialerApp_HyphenateNumberString(pMe->m_CallInfo.other_party_no);
      (void) ISTATIC_SetTextEx(pIStatic, (byte *) pszHyphenatedNum, NULL, FALSE);
    }
    else
    {
      (void) ISTATIC_SetTextEx(pIStatic, (byte*) pMe->m_CallInfo.other_party_no, NULL, FALSE); 
    }

    DoNumberLookup(pMe->m_pIADDRBOOK, pMe->m_CallInfo.other_party_no, pszName, DIALERAPP_MAX_STR_SIZE);
    if(*pszName != '\0')
    {    
      (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
      (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, TRUE);
    }
  }

  IDIALOG_Redraw(pActiveDlg);

  // free memory
  FREE(pszName);
  FREEIF(pszHyphenatedNum);

  return TRUE;
}
//lint -restore 

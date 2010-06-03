/*=============================================================================

FILE: DialerUSSD.c

SERVICES: Dialer Applet Dialogs

GENERAL DESCRIPTION: USSD Dialogs

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerUSSD.c#31 $
$DateTime: 2008/04/11 10:42:22 $
$Author: sgavini $
$Change: 641127 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
06/19/07   AYK     Set the TP_FOCUS_NOSEL property of the textctl in the
                   USSD info dialog.
04/09/04   SUN     Fixed Lint Errors
04/05/04   SUN     Fixed the response fields for USSD
01/20/03   BPW     DTMF and other memory cleanup and lint errors
01/14/04   BPW     Removed empty string check for USSD Info
01/09/04   BPW     Added static ctl for USSD info dialog
12/19/03   BPW     USSD & MissedCall now uses own dialg info structure
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/25/03   BPW     Added code review changes and comments
11/25/03   BPW     Linted for the Saber 6250 build
11/13/03   BPW     Press and hold clear to clear contents of text control
11/7/03    SUN     Send response to USSD automatically
10/21/03   SUN     Fixed Compile Errors
10/14/03   RAM     Modified functionality for USSD NW Initiated messages.
10/06/03   SUN     Fixed USSD Display
09/04/03   SUN     Changes related to AEESUPPSVC
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
08/13/03   BW      Created DialerUSSD Source File 

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
#include "DialerUSSD.h"
#include "DialerUtils.h"
#include "dialerapp_res.h"
#include "AppComFunc.h"

/*===========================================================================


                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_USSDPasswordDlg_CmdHandler

DESCRIPTION: Handles USSDPasswordDlg Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDPasswordDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   switch (ctrl_id) 
   {
   case IDL_SK_USSD_PASSWORD_OK:
     {
       CUSSDDlgInfo *pDlgInfo;

       pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);
       if(pDlgInfo == FALSE)
       {
         DIALER_ERR("Null Pointer", 0, 0, 0);
         (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
         return FALSE;
       }

       pDlgInfo->SSResponse.user_input = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
       if(pDlgInfo->SSResponse.user_input == NULL)
       {
           DIALER_ERR("No Memory", 0, 0, 0);
           return FALSE; //error
       }

       if(CDialerApp_TextCtlGetText(pMe, pDlgInfo->SSResponse.user_input, 
                              DIALERAPP_MAX_STR_SIZE, IDC_TEXT_USSD_PASSWORD) == FALSE)
       {
         DIALER_ERR("CDialerApp_TextCtlGetText FAILED", 0, 0, 0);
         (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
         FREE(pDlgInfo->SSResponse.user_input);
         return FALSE;
       }

       if(WSTRLEN(pDlgInfo->SSResponse.user_input)  == 0)
       {
         // cannot send empty string back
         DIALER_MSG("USSD Response cannot be NULL", 0, 0, 0);
         FREE(pDlgInfo->SSResponse.user_input);
         return TRUE;
       }

       pDlgInfo->SSResponse.length = (uint32) WSTRLEN(pDlgInfo->SSResponse.user_input);
       pDlgInfo->SSResponse.resp_type = AEESUPPSVC_GET_PASSWORD_RES;   
       (void) ISUPPSVC_ContinueSS(pMe->m_pISUPPSVC, &(pDlgInfo->SSResponse));
       FREE(pDlgInfo->SSResponse.user_input);   
       (void) CDialerApp_EndDlg(pMe);      
       return TRUE;

     }

   default:
     break;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM) 

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_USSDPasswordDlg_KeyHandler

DESCRIPTION: Handles USSDPasswordDlg Dialog's key events

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
boolean CDialerApp_USSDPasswordDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   switch(evt)
   {
   case EVT_KEY_PRESS:
     if(key == (uint32)AVK_CLR)
     {
       pMe->m_wTextCtlIDToClear = IDC_TEXT_USSD_PASSWORD;
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
     case AVK_END:
     case AVK_CLR:
       {
         CUSSDDlgInfo *pDlgInfo;

         pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);
         if(pDlgInfo == FALSE)
         {
           DIALER_ERR("Null Pointer", 0, 0, 0);
           return FALSE;
         }

         (void) ISUPPSVC_CancelSS(pMe->m_pISUPPSVC, (uint8) pDlgInfo->SSResponse.invoke_id, (AEESuppSvcReleaseSSOp) pDlgInfo->ssOper);
         (void) CDialerApp_EndDlg(pMe);
         return TRUE; 
       }

     case AVK_SELECT:
       {
         // select the softkey ok when select is pressed
         IDialog *pActiveDlg = CDialerApp_GetActiveDlg(pMe);
         ITextCtl *pIText;

         // error checking
         if (pActiveDlg == NULL) 
         {
           DIALER_ERR("Null pointer", 0,0,0);
           return FALSE;
         }

         pIText = (ITextCtl*)IDIALOG_GetControl(pActiveDlg, 
           IDC_TEXT_USSD_PASSWORD);

         if(ITEXTCTL_IsActive(pIText))
         {
           IMenuCtl *pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(
             pActiveDlg, IDC_SK_USSD_PASSWORD);

           IMENUCTL_SetSel(pISoftKeyMenu, IDL_SK_USSD_PASSWORD_OK); 
           (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_USSD_PASSWORD);
           IDIALOG_Redraw(pActiveDlg);
           return TRUE;
         }

         // event not handled
         return FALSE;
       }

     default:
       return FALSE;
     }
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM) 

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_USSDPasswordDlg_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDPasswordDlg_EndHandler(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_ClearTextCtl, (void*) pMe);
   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM) 
}

/*=============================================================================
FUNCTION: CDialerApp_USSDPasswordDlg_Init

DESCRIPTION: Initializes the controls for USSD Password dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDPasswordDlg_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pActiveDlg;
   ITextCtl *pIText;
   IMenuCtl *pISoftKeyMenu; 
   CUSSDDlgInfo *pDlgInfo;

   PRINT_FUNCTION_NAME();
   // error checking
   if (pMe == NULL) 
   {
     return FALSE;
   }

   pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);
   if(pDlgInfo == FALSE)
   {
     DIALER_ERR("Null Pointer", 0, 0, 0);
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
   pIText = (ITextCtl*)IDIALOG_GetControl(pActiveDlg, 
                                             IDC_TEXT_USSD_PASSWORD);
   pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
                                                 IDC_SK_USSD_PASSWORD);
   if((pIText == NULL) || (pISoftKeyMenu == NULL)) 
   {
     DIALER_ERR("Null pointer", 0,0,0);
     return FALSE; // error 
   }

   // Update the softkey menu
   (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
   (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_OK,
                                    IDL_SK_USSD_PASSWORD_OK, NULL, 0);
   ITEXTCTL_SetSoftKeyMenu(pIText, NULL);
   SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
   IMENUCTL_SetSel(pISoftKeyMenu, IDL_SK_NUMEDIT_SEND); 

   // Set textctl properties 
   ITEXTCTL_SetMaxSize(pIText, DIALERAPP_MAX_STR_SIZE);
   ITEXTCTL_SetRect(pIText, &pMe->m_DialedDigitsRect);
   ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
   (void) ITEXTCTL_SetTitle(pIText, DIALERAPP_RES_FILE, 0, pDlgInfo->szNWInitiatedData);
   ITEXTCTL_SetProperties(pIText, TP_FRAME | TP_PASSWORD);
   (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
   (void) ITEXTCTL_Redraw(pIText);
   (void) IDIALOG_SetFocus(pActiveDlg, IDC_TEXT_USSD_PASSWORD);
   IDIALOG_Redraw(pActiveDlg);

   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM) 
} 

/*=============================================================================
FUNCTION: CDialerApp_USSDInfoDlg_Init

DESCRIPTION: Initializes the controls for USSDInfoDlg dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDInfoDlg_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   IDialog *pActiveDlg;
   ITextCtl *pIText;
   IMenuCtl *pISoftKeyMenu; 
   IStatic *pIStatic;
   CUSSDDlgInfo *pDlgInfo;
   AEERect StaticRect;
   AEERect MenuRect;
   AEERect TextRect;
   uint32 dwTextProps;

   PRINT_FUNCTION_NAME();
   // error checking
   if (pMe == NULL) 
   {
     return FALSE;
   }

   pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);
   if(pDlgInfo == FALSE)
   {
     DIALER_ERR("Null Pointer", 0, 0, 0);
     return FALSE;
   }

   pActiveDlg = CDialerApp_GetActiveDlg(pMe);
   if (pActiveDlg == NULL) 
   {
     DIALER_ERR("Null pointer", 0,0,0);
     return FALSE;
   }

   // Initialize the tab control  
   TabController_Init(&pMe->m_pTabController, 2);
   // Add the controls  
   TabController_AddControl(pMe->m_pTabController, TEXT_CTL, IDIALOG_GetControl(pActiveDlg, IDC_TEXT_USSD_INFO));
   TabController_AddControl(pMe->m_pTabController, SK_CTL, IDIALOG_GetControl(pActiveDlg, IDC_SK_USSD_INFO));
   // Set the initial focused control
   TabController_SetActiveEx(pMe->m_pTabController, 0);

   // Get controls in dialog
   pIText = (ITextCtl*)IDIALOG_GetControl(pActiveDlg, 
                                             IDC_TEXT_USSD_INFO);
   pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
                                                 IDC_SK_USSD_INFO);
   pIStatic = (IStatic*)IDIALOG_GetControl(pActiveDlg, 
                                                 IDC_STATIC_USSD_INFO);
   if((pIText == NULL) || (pISoftKeyMenu == NULL) || (pIStatic == NULL)) 
   {
     DIALER_ERR("Null pointer", 0,0,0);
     return FALSE; // error 
   }

   // Update the softkey menu
   (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
   (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_OK,
                                    IDL_SK_USSD_INFO_OK, NULL, 0);
   ITEXTCTL_SetSoftKeyMenu(pIText, NULL);
   SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
   IMENUCTL_SetSel(pISoftKeyMenu, IDL_SK_NUMEDIT_SEND); 

   // set rects
   IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);
   SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, ((pMe->m_rc.dy-MenuRect.dy-30)*.65));     
   SETAEERECT(&TextRect, 10, StaticRect.dy+20, pMe->m_rc.dx-20, ((pMe->m_rc.dy-MenuRect.dy-30)*.35));    

   // Set static properties
   ISTATIC_SetRect(pIStatic, &StaticRect);		 
   ISTATIC_SetActive(pIStatic, FALSE);
   ISTATIC_SetProperties(pIStatic, ST_MIDDLETEXT | ST_CENTERTEXT  | ST_NOSCROLL); 
   (void) ISTATIC_SetText(pIStatic, NULL, pDlgInfo->szNWInitiatedData, AEE_FONT_BOLD, 
                   AEE_FONT_BOLD);

   // Set textctl properties 
   ITEXTCTL_SetMaxSize(pIText, DIALERAPP_MAX_STR_SIZE);
   ITEXTCTL_SetRect(pIText, &TextRect);  
   ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);

   dwTextProps = TP_FRAME | TP_MULTILINE;
 #if MIN_BREW_VERSIONEx(3,1,3)
   dwTextProps |= TP_FOCUS_NOSEL;
 #endif

   ITEXTCTL_SetProperties(pIText, dwTextProps);
 #if MIN_BREW_VERSION(3,1)
   (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_DIALED_DIGITS);
 #else
   (void) ITEXTCTL_SetInputMode(pIText, OEM_TM_DIALED_DIGITS);
 #endif
   (void) ITEXTCTL_Redraw(pIText);

   (void) IDIALOG_SetFocus(pActiveDlg, IDC_TEXT_USSD_INFO);
   IDIALOG_Redraw(pActiveDlg);

   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

/*=============================================================================
FUNCTION: CDialerApp_USSDInfoDlg_CmdHandler

DESCRIPTION: Handles USSDInfoDlg Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDInfoDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
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
   case IDL_SK_USSD_INFO_OK:
     {
       CUSSDDlgInfo *pDlgInfo;

       pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);
       if(pDlgInfo == FALSE)
       {
         DIALER_ERR("Null Pointer", 0, 0, 0);
         (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
         return FALSE;
       }

       pDlgInfo->SSResponse.user_input = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
       if(pDlgInfo->SSResponse.user_input == NULL)
       {
           DIALER_ERR("No Memory", 0, 0, 0);
           return FALSE; //error
       }

       (void) CDialerApp_TextCtlGetText(pMe, pDlgInfo->SSResponse.user_input, 
                              DIALERAPP_MAX_STR_SIZE, IDC_TEXT_USSD_INFO);

       pDlgInfo->SSResponse.notify = AEESUPPSVC_USS_RES_OK;
       pDlgInfo->SSResponse.length = (uint32) WSTRLEN(pDlgInfo->SSResponse.user_input);

       (void) ISUPPSVC_ContinueSS(pMe->m_pISUPPSVC, &(pDlgInfo->SSResponse));

       FREEIF(pDlgInfo->SSResponse.user_input);
       (void) CDialerApp_EndDlg(pMe);

       return TRUE;      
     }

   default:
     break;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_USSDInfoDlg_KeyHandler

DESCRIPTION: Handles USSDInfoDlg Dialog's key events

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
boolean CDialerApp_USSDInfoDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
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
     if(key == (uint32)AVK_CLR)
     {
       pMe->m_wTextCtlIDToClear = IDC_TEXT_USSD_INFO;
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
     case AVK_CLR:
     case AVK_END:
       {
         CUSSDDlgInfo *pDlgInfo;

         pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);
         if(pDlgInfo == FALSE)
         {
           DIALER_ERR("Null Pointer", 0, 0, 0);
           return FALSE;
         }

         pDlgInfo->SSResponse.notify = AEESUPPSVC_USS_RES_BUSY_DISP;
         (void) ISUPPSVC_ContinueSS(pMe->m_pISUPPSVC, &(pDlgInfo->SSResponse));
         (void) CDialerApp_EndDlg(pMe);
         return TRUE; 
       }

     case AVK_SELECT:
       {
         // select the softkey ok when select is pressed
         IDialog *pActiveDlg = CDialerApp_GetActiveDlg(pMe);
         ITextCtl *pIText;

         // error checking
         if (pActiveDlg == NULL) 
         {
           DIALER_ERR("Null pointer", 0,0,0);
           return FALSE;
         }

         pIText = (ITextCtl*)IDIALOG_GetControl(pActiveDlg, 
           IDC_TEXT_USSD_INFO);

         if(ITEXTCTL_IsActive(pIText))
         {
           IMenuCtl *pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(
             pActiveDlg, IDC_SK_USSD_INFO);

           IMENUCTL_SetSel(pISoftKeyMenu, IDL_SK_USSD_INFO_OK); 
           (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_USSD_INFO);
           IDIALOG_Redraw(pActiveDlg);
           return TRUE;
         }

         // event not handled
         return FALSE;
       }

     default:
       return FALSE;
     }
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_USSDInfoDlg_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDInfoDlg_EndHandler(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
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
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
/*=============================================================================
FUNCTION: CDialerApp_USSDSendResponse

DESCRIPTION: Sends the USSD Response

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_USSDSendResponse(CDialerApp *pMe)
{
  CUSSDDlgInfo *pDlgInfo;

  PRINT_FUNCTION_NAME();

  // error checking 
  if(pMe == NULL) 
  { 
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);
  if(pDlgInfo == FALSE)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  pDlgInfo->SSResponse.notify = AEESUPPSVC_USS_RES_OK;
  (void) ISUPPSVC_ContinueSS(pMe->m_pISUPPSVC, &(pDlgInfo->SSResponse));

  (void) CDialerApp_EndDlg(pMe);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_USSDPromptDlg_ResetTimer

DESCRIPTION: Resets the timer for CDialerApp_USSDSendResponse

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CDialerApp_USDDPromptDlg_ResetTimer(CDialerApp *pMe)
{
   int nReturnStatus;

   (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_USSDSendResponse, pMe);
   nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                   5000,
                   (PFNNOTIFY) CDialerApp_USSDSendResponse,
                   (uint32*) pMe);
   (void)CheckAEEReturnStatus(nReturnStatus);
}
#endif  /*F_WCDMA || F_GSM*/

/*=============================================================================
FUNCTION: CDialerApp_USSDPromptDlg_CmdHandler

DESCRIPTION: Handles USSDDPromptlg Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDPromptDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   CDialerApp_USDDPromptDlg_ResetTimer(pMe);

   switch (ctrl_id) 
   {
   case IDL_SK_USSD_PROMPT_ACCEPT:
     (void) CDialerApp_USSDSendResponse(pMe);
     return TRUE;

   case IDL_SK_USSD_PROMPT_REJECT:
     {
       CUSSDDlgInfo *pDlgInfo;

       pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);
       if(pDlgInfo == FALSE)
       {
         DIALER_ERR("Null Pointer", 0, 0, 0);
         return FALSE;
       }

       pDlgInfo->SSResponse.notify = AEESUPPSVC_USS_RES_BUSY_DISP;
       (void) ISUPPSVC_ContinueSS(pMe->m_pISUPPSVC, (&pDlgInfo->SSResponse));

       (void) CDialerApp_EndDlg(pMe);
       return TRUE;
     }

   default:
     break;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_USSPromptDDlg_KeyHandler

DESCRIPTION: Handles USSDPromptDlg Dialog's key events

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
boolean CDialerApp_USSDPromptDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   switch(evt)
   {
   case EVT_KEY_PRESS:
      CDialerApp_USDDPromptDlg_ResetTimer(pMe);
      return FALSE;

   case EVT_KEY:
     switch(key) 
     { 
     case AVK_CLR:
     case AVK_END:
       {
         CUSSDDlgInfo *pDlgInfo;

         pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);
         if(pDlgInfo == FALSE)
         {
           DIALER_ERR("Null Pointer", 0, 0, 0);
           return FALSE;
         }

         pDlgInfo->SSResponse.notify = AEESUPPSVC_USS_RES_BUSY_DISP;
         (void) ISUPPSVC_ContinueSS(pMe->m_pISUPPSVC, &(pDlgInfo->SSResponse));   
         (void) CDialerApp_EndDlg(pMe);
         return TRUE;    
       }

     default:
       return FALSE;
     }
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_USSDPromptDlg_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDPromptDlg_EndHandler(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_USSDSendResponse, pMe);
   return TRUE;
#else
   return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}


/*=============================================================================
FUNCTION: CDialerApp_USSDPromptDlg_Init

DESCRIPTION: Initializes the controls for USSD Prompt dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDPromptDlg_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  IDialog *pActiveDlg;
  IStatic *pIStatic;
  IMenuCtl *pISoftKeyMenu; 
  AEERect StaticRect;
  AEERect MenuRect;
  CUSSDDlgInfo *pDlgInfo;       
  
  PRINT_FUNCTION_NAME();

  // error checking
  if (pMe == NULL) 
  {
    return FALSE;
  }

  CDialerApp_USDDPromptDlg_ResetTimer(pMe);

  pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);
  if(pDlgInfo == FALSE)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);
  if (pActiveDlg == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, 
    IDC_STATIC_USSD_PROMPT);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_SK_USSD_PROMPT);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_ACCEPT,
    IDL_SK_USSD_PROMPT_ACCEPT, NULL, 0);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_REJECT,
    IDL_SK_USSD_PROMPT_REJECT, NULL, 0);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
  IMENUCTL_SetActive(pISoftKeyMenu, TRUE);
  (void) IMENUCTL_Redraw(pISoftKeyMenu);


  // Set Static control size 
  ISTATIC_GetRect(pIStatic, &StaticRect);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);  
  SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, pMe->m_rc.dy-MenuRect.dy-20); 

  // Static Info Properties
  ISTATIC_SetRect(pIStatic, &StaticRect); 
  ISTATIC_SetProperties(pIStatic, ST_MIDDLETEXT | ST_CENTERTEXT | ST_NOSCROLL);
  (void) ISTATIC_SetText(pIStatic, NULL, pDlgInfo->szNWInitiatedData, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
  ISTATIC_SetActive(pIStatic, FALSE);
  (void) ISTATIC_Redraw(pIStatic);
  
  (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_USSD_PROMPT);
  IDIALOG_Redraw(pActiveDlg);
  
  return TRUE;
#else
  return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
} 
//lint -restore 

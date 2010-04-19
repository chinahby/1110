/*=============================================================================

FILE: DialerCalling.c

SERVICES: Dialer Applet Dialogs

GENERAL DESCRIPTION: Calling Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerCalling.c#40 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
08/22/07   cvs     Add framework for dialogs
08/02/07   cvs     Featurize all secondary display support
10/14/05   cvs     support for attaching carkit display mid call
10/12/05   cvs     Carkit display
07/28/05   AT      Fix compiler warnings and indentation.
06/07/05   sun     Added support for CCBS
05/19/05   SUN     Fixed SS Operation and SS Code Type 
04/06/05   RI      Support for TTY
04/09/04   SUN     Fixed Lint Errors
04/05/04   BPW     Alert changes for failed calls
01/22/04   BPW     Optimized NV gets
01/21/04   BPW     Added ISuppSvc API changes
01/16/03   BPW     Display alpha string if present
01/08/04   BPW     Use own dialer hyphenation function
12/19/03   BPW     Fixed Cancel SS request
12/18/03   BPW     Added more SS feedback for requesting and incomplete
12/03/03   BPW     Added CUG display
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/25/03   BPW     Linted for the Saber 6250 build
11/13/03   BPW     Added "Looking for Emergency Service" string when needed
11/13/03   BPW     Added function to format display for phone number
08/29/03   BW      Dialog ends even if ICM fails
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
07/24/03   BW      Created DialerCalling Source File 

=============================================================================*/
//lint -save -e611
//lint -save -e740

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
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
#include "DialerCalling.h"
#include "dialerapp_res.h"
#include "DialerUtils.h"
#include "AppComFunc.h"
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
static boolean  CDialerApp_ConvertOperToRelOp (AEESuppSvcSSOperationType ssOper, AEESuppSvcReleaseSSOp *relOp);

#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

/*===========================================================================

STATIC and PUBLIC FUNCTIONS

===========================================================================*/
/*=============================================================================
FUNCTION: CDialerApp_CallingDlg_KeyHandler

DESCRIPTION: Handles Calling Dialog's key events

PARAMETERS:
  *pUser: CDialerApp object pointer
  evt: event code
  key: keycode to handle

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallingDlg_KeyHandler(CDialerApp *pMe,
                                         AEEEvent evt,
                                         uint16 key)
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
     case AVK_CLR:
       // do not let main event handler get event
       return TRUE;

     case AVK_END:
       {
         int nRetVal = ICM_EndCall(pMe->m_pICM, pMe->m_byCurrOrigCall);

         if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
         {
           (void) CDialerApp_EndCallRelatedDlgs(pMe, pMe->m_byCurrOrigCall);
           DIALER_ERR("ICM_EndCall FAILED", 0, 0, 0);  
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
FUNCTION: CDialerApp_CallingDlg_CmdHandler

DESCRIPTION: Handles Calling Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallingDlg_CmdHandler(CDialerApp *pMe,
                                         uint16 ctrl_id)
{
  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL) 
  { 
    return FALSE;
  }

  switch (ctrl_id) 
  {
  case IDL_SK_CALLING_END:
    {
      int nRetVal = ICM_EndCall(pMe->m_pICM, pMe->m_byCurrOrigCall);

      if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
      {
        (void) CDialerApp_EndCallRelatedDlgs(pMe, pMe->m_byCurrOrigCall);
        DIALER_ERR("ICM_EndCall FAILED", 0, 0, 0);  
        return FALSE;
      }

      ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);

      return TRUE;
    }

  default:
    return FALSE;
  }     

}

/*=============================================================================
FUNCTION: CDialerApp_CallingDlg_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallingDlg_EndHandler(CDialerApp *pMe)
{
  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL) 
  { 
    return FALSE;
  }

  (void) ISHELL_CancelTimer(pMe->a.m_pIShell, 
    (PFNNOTIFY) CDialerApp_CallingDlg_DisplayRefresh, (void*) pMe);
  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_CallingDlg_Init

DESCRIPTION: Initializes the controls for Calling dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallingDlg_Init(CDialerApp *pMe)
{
  IDialog *pActiveDlg;
  IStatic *pIStatic;
  IMenuCtl *pISoftKeyMenu; 
  AEERect StaticRect;
  AEERect MenuRect;
  AECHAR *pszTitle = NULL;
  
  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
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
    IDC_STATIC_CALLING);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_SK_CALLING);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // init buffer
  pszTitle = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)); 
  if(pszTitle == NULL)
  {
    DIALER_ERR("No memory", 0,0,0);
    return FALSE;
  }

  // load the title
#ifdef FEATURE_TTY
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrOrigCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE)
  {
    DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);
    return FALSE;
  }
  if( pMe->m_CallInfo.bTTYcall ){
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_CALLING_TTY, pszTitle, 
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
  }
  else {
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_CALLING, pszTitle, 
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
  }
#else
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_CALLING, pszTitle, 
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
#endif
  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_END,
    IDL_SK_CALLING_END, NULL, 0);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);


  // Set Static control size 
  ISTATIC_GetRect(pIStatic, &StaticRect);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);  
  SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, pMe->m_rc.dy-MenuRect.dy-20); 

  // Static Info Properties
  ISTATIC_SetRect(pIStatic, &StaticRect);
  ISTATIC_SetActive(pIStatic, FALSE);
  ISTATIC_SetProperties(pIStatic, ST_MIDDLETEXT | ST_UNDERLINE | ST_CENTERTITLE | ST_CENTERTEXT | ST_NOSCROLL); 
  (void) ISTATIC_SetText(pIStatic, pszTitle, NULL, AEE_FONT_BOLD, 
                  AEE_FONT_NORMAL);
  
  (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_CALLING);

  // set timer for emergency calls "searching"
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrOrigCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == TRUE)
  {
    if(pMe->m_CallInfo.call_type == AEECM_CALL_TYPE_EMERGENCY)
    {
      (void) ISHELL_SetTimer(pMe->a.m_pIShell, 1000, 
      (PFNNOTIFY) CDialerApp_CallingDlg_DisplayRefresh, (void*) pMe);
    }
  }

  FREE(pszTitle);

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
  // print origination msg on secondary display
  pMe->m_DispState = DIALER_DISP_STATE_ORIG;
  pMe->m_bIsActiveCalls = TRUE;
  CDialerApp_UpdateSecondaryDisp(pMe);
#endif

  return CDialerApp_CallingDlg_DisplayRefresh(pMe);
} 

/*=============================================================================
FUNCTION: CDialerApp_CallingDlg_DisplayRefresh

DESCRIPTION: Refreshes the calling dialog display

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallingDlg_DisplayRefresh(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog *pActiveDlg;
  IStatic *pIStatic;
  AECHAR szCUGFormat[] = {'%','s',':',' ','%','u','\0'};
  AECHAR *pszCUGBuf = NULL;
  AECHAR *pszName = NULL; 
  
  PRINT_FUNCTION_NAME();
  // error checking
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

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_CALLING)
  {
    DIALER_ERR("Calling Dialog is not active", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, 
    IDC_STATIC_CALLING);
  if(pIStatic == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // init buffer
  pszName = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  pszCUGBuf = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  if((pszName== NULL) || (pszCUGBuf == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszName);
    FREEIF(pszCUGBuf);
    return FALSE;
  }
  
  // display all of the needed info to the screen
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrOrigCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE)
  {
    DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);
    FREEIF(pszName);  
    FREEIF(pszCUGBuf);
    return TRUE;
  }

  // display the other party number and name if any in contacts
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

  // check for CUG info
  if(pMe->m_CallInfo.forward_cug_info.cm_cug_index.present == TRUE)
  {
    // display CUG info
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_USERGROUP, 
      pszName, ((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR)));  
    WSPRINTF(pszCUGBuf, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR), szCUGFormat, pszName, 
      ((uint16)((pMe->m_CallInfo.forward_cug_info.cm_cug_index.msb << 8)|(pMe->m_CallInfo.forward_cug_info.cm_cug_index.lsb))));
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszCUGBuf, NULL, TRUE);
  }

  // check for alpha
  if(WSTRLEN(pMe->m_CallInfo.alpha) > 0)
  {
    // display alpha
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) pMe->m_CallInfo.alpha, NULL, TRUE);
  }

  if(pMe->m_CallInfo.call_type == AEECM_CALL_TYPE_EMERGENCY)
  {
    AEECMSSInfo SSInfo;

    // reset the timer
    (void) ISHELL_SetTimer(pMe->a.m_pIShell, 1000, 
      (PFNNOTIFY) CDialerApp_CallingDlg_DisplayRefresh, (void*) pMe);

    if(CheckAEEReturnStatus(ICM_GetSSInfo(pMe->m_pICM, &SSInfo, sizeof(AEECMSSInfo))) == TRUE)
    {
      if(SSInfo.srv_status == AEECM_SRV_STATUS_NO_SRV)
      {
        // we want to display looking for service
        (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_LOOKINGFOREMERGENCY, pszName, (DIALERAPP_MAX_STR_SIZE * sizeof(AECHAR)));  
        (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
        (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
        (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, TRUE); 
      }
    }
  }
  
  IDIALOG_Redraw(pActiveDlg);

  // free memory
  FREE(pszName);
  FREE(pszCUGBuf);
  
  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_SSRequestDlg_KeyHandler

DESCRIPTION: Handles SSRequestDlg Dialog's key events

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
boolean CDialerApp_SSRequestDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  AEESuppSvcReleaseSSOp relOp;
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
     case AVK_CLR:
       // do not let main event handler get event
       return TRUE;

     case AVK_END:
       {
         CSSRequestDlgInfo *pDlgInfo = NULL;

         // cancel the ss request

         pDlgInfo = (CSSRequestDlgInfo*) CDialerApp_GetActiveDlgInfo(pMe);
         if(pDlgInfo == NULL)
         {
           DIALER_ERR("NULL Pointer", 0,0,0);
           (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
           // still want to swallow this event
           return TRUE;
         }

         if(!CDialerApp_ConvertOperToRelOp(pDlgInfo->ssOper, &relOp) )
         {
           (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
           return TRUE;
         }

         if(CDialerApp_CheckAEEReturnStatus(pMe, 
             ISUPPSVC_CancelSS(pMe->m_pISUPPSVC, pDlgInfo->byReqID, (AEESuppSvcReleaseSSOp)relOp)) == FALSE)
         {
           DIALER_ERR("ISUPPSVC_CancelSS FAILED", 0,0,0);
           // still want to swallow this event
           return TRUE;
         }

         (void) CDialerApp_EndDlg(pMe);
         // do not let main event handler get event
         return TRUE;
       }

     default:
       return FALSE;
     }  
  }
#endif

  return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SSRequestDlg_CmdHandler

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
boolean CDialerApp_SSRequestDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  AEESuppSvcReleaseSSOp relOp;
  PRINT_FUNCTION_NAME();

  // error checking 
  if(pMe == NULL) 
  { 
    return FALSE;
  }

  switch (ctrl_id) 
  {
  case IDL_SK_SSREQUEST_CANCEL:
    {
      CSSRequestDlgInfo *pDlgInfo = NULL;

      // cancel the ss request

      pDlgInfo = (CSSRequestDlgInfo*) CDialerApp_GetActiveDlgInfo(pMe);
      if(pDlgInfo == NULL)
      {
        DIALER_ERR("NULL Pointer", 0,0,0);
        (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
        // still want to swallow this event
        return TRUE;
      }

      if(!CDialerApp_ConvertOperToRelOp(pDlgInfo->ssOper, &relOp) )
      {
       (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
        return TRUE;
      }

      if(CDialerApp_CheckAEEReturnStatus(pMe, ISUPPSVC_CancelSS(pMe->m_pISUPPSVC, pDlgInfo->byReqID, 
                                         (AEESuppSvcReleaseSSOp)relOp)) == FALSE)
      {
        DIALER_ERR("ISUPPSVC_CancelSS FAILED", 0,0,0);
        return TRUE;
      }

      (void) CDialerApp_EndDlg(pMe);
      return TRUE;
    }

  default:
    break;
  }     
#endif // defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

  return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_RecallDlg_Init

DESCRIPTION: Initializes the controls for Calling dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_RecallDlg_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  IDialog *pActiveDlg;
  IStatic *pIStatic;
  IMenuCtl *pISoftKeyMenu;
  AEERect StaticRect;
  AEERect MenuRect;
  AECHAR *pszRecall = NULL;
  AECHAR *pszName = NULL;
  CRecallDlgInfo *pDlgInfo = NULL;
  
  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
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

  pDlgInfo = (CRecallDlgInfo*) CDialerApp_GetActiveDlgInfo(pMe);

  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, IDC_STATIC_RECALL);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_SK_RECALL);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL))
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error
  }

  // init buffer
  pszRecall = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  if(pszRecall == NULL)
  {
    DIALER_ERR("No memory", 0,0,0);
    return FALSE;
  }

  pszName = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  if(pszName == NULL)
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszRecall);
    return FALSE;
  }

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);

  if(AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM))
  {
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE,
                            IDS_HOLD_N_ACCEPT, IDL_RECALL_SK_HOLD_N_ACCEPT,
                            NULL, 0);
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_REL_N_ACCEPT,
                            IDL_RECALL_SK_REL_N_ACCEPT, NULL, 0);
  }
  else
  {
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_CALL,
                            IDL_RECALL_SK_CALL, NULL, 0);
  }
 
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_CANCEL,
                          IDL_RECALL_SK_CANCEL, NULL, 0);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
 
  // Set Static control size
  ISTATIC_GetRect(pIStatic, &StaticRect);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);
  SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, pMe->m_rc.dy-MenuRect.dy-20);

  // Static Info Properties
  ISTATIC_SetRect(pIStatic, &StaticRect);
  ISTATIC_SetActive(pIStatic, FALSE);
  ISTATIC_SetProperties(pIStatic, ST_MIDDLETEXT | ST_CENTERTEXT | ST_NOSCROLL);
 
  // Display name or number
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM,pDlgInfo->callID,
                          &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == TRUE)
  {
    CDialerApp_FormatNumberDisplayString(pMe, &(pMe->m_CallInfo), pszName,
                                         DIALERAPP_MAX_STR_SIZE,
                                         pMe->m_bNVAutoHyphen);
    (void) ISTATIC_SetText(pIStatic, pszName, NULL, AEE_FONT_BOLD,
                           AEE_FONT_NORMAL);

    if(WSTRLEN(pMe->m_CallInfo.other_party_no) != 0)
    {
      if(pMe->m_bNVAutoHyphen == TRUE)
      {
        AECHAR *pszHyphen = CDialerApp_HyphenateNumberString(
                                               pMe->m_CallInfo.other_party_no);
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
          (void) ISTATIC_SetTextEx(pIStatic,
                                   (byte*) pMe->m_CallInfo.other_party_no,
                                   NULL, TRUE);
        }
      }
    }
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                              IDS_AVAILABLE, pszRecall,
                              ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszRecall, NULL, TRUE);

  (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_RECALL);

  FREEIF(pszRecall);
  FREEIF(pszName);

  return TRUE;
#else
  return FALSE;
#endif // defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
}


/*=============================================================================
FUNCTION: CDialerApp_SetupRecallDlg_Init

DESCRIPTION: Initializes the controls for Calling dialog

PARAMETERS:
*pMe: CDialerApp object pointer

RETURN VALUE:
boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SetupRecallDlg_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  IDialog *pActiveDlg;
  IStatic *pIStatic;
  IMenuCtl *pISoftKeyMenu;
  AEERect StaticRect;
  AEERect MenuRect;
  AECHAR *pszRecall = NULL;

  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
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
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, IDC_STATIC_SETUP_RECALL);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg,
                                                IDC_SK_SETUP_RECALL);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL))
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error
  }

  // init buffer
  pszRecall = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  if(pszRecall == NULL)
  {
    DIALER_ERR("No memory", 0,0,0);
    return FALSE;
  }

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_OK,
                          IDL_SK_SETUP_RECALL, NULL, 0);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_CANCEL,
                          IDL_SK_SETUP_RECALL_CANCEL, NULL, 0);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);

  // Set Static control size
  ISTATIC_GetRect(pIStatic, &StaticRect);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);
  SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, pMe->m_rc.dy-MenuRect.dy-20);

  // Static Info Properties
  ISTATIC_SetRect(pIStatic, &StaticRect);
  ISTATIC_SetActive(pIStatic, FALSE);
  ISTATIC_SetProperties(pIStatic, ST_MIDDLETEXT | ST_CENTERTEXT | ST_NOSCROLL);

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                              IDS_SETUP_RECALL, pszRecall,
                              ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
  (void) ISTATIC_SetText(pIStatic, pszRecall, NULL, AEE_FONT_BOLD,
                         AEE_FONT_NORMAL);

  (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_SETUP_RECALL);

  FREEIF(pszRecall);

  return TRUE;
#else
  return FALSE;
#endif // defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
}


/*=============================================================================
FUNCTION: CDialerApp_RecallDlg_KeyHandler

DESCRIPTION: Handles RecallDlg Dialog's key events

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
boolean CDialerApp_RecallDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
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
        case AVK_CLR:
          // do not let main event handler get event
          return TRUE;
  
        case AVK_END:
        {
          (void) CDialerApp_EndDlg(pMe);
          // do not let main event handler get event
          return TRUE;
        }
  
        default:
          return FALSE;
      }  
   }
#endif // defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_RecallDlg_CmdHandler

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
boolean CDialerApp_RecallDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
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
     case IDL_RECALL_SK_CALL:
     {
       CRecallDlgInfo *pDlgInfo = NULL;
       pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);

       if(pDlgInfo == NULL)
       {
         DIALER_ERR("Null pointer", 0,0,0);
         return FALSE;
       }
       if( ICM_RecallRsp(pMe->m_pICM, AEECM_RECALL_RESP_CCBS_ACCEPT, pDlgInfo->callID) != SUCCESS)
       {
         DIALER_ERR("ICM_RecallRsp failed", 0,0,0);
         return FALSE;
       }

       (void) CDialerApp_EndDlg(pMe);
       return TRUE;

     }

    case IDL_RECALL_SK_HOLD_N_ACCEPT:
    {
      CRecallDlgInfo *pDlgInfo = NULL;
      pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);

      if(pDlgInfo == NULL)
      {
        DIALER_ERR("Null pointer", 0,0,0);
        return FALSE;
      }
      if( ICM_RecallRsp(pMe->m_pICM, AEECM_RECALL_RESP_CCBS_HOLD_ACCEPT, pDlgInfo->callID) != SUCCESS)
      {
        DIALER_ERR("ICM_RecallRsp failed", 0,0,0);
        return FALSE;
      }

      (void) CDialerApp_EndDlg(pMe);
      return TRUE;
    }

    case IDL_RECALL_SK_REL_N_ACCEPT:
    {
      CRecallDlgInfo *pDlgInfo = NULL;
      pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);

      if(pDlgInfo == NULL)
      {
        DIALER_ERR("Null pointer", 0,0,0);
        return FALSE;
      }
      if( ICM_RecallRsp(pMe->m_pICM, AEECM_RECALL_RESP_CCBS_REL_ACCEPT, pDlgInfo->callID) != SUCCESS)
      {
        DIALER_ERR("ICM_RecallRsp failed", 0,0,0);
        return FALSE;
      }
      (void) CDialerApp_EndDlg(pMe);
      return TRUE;
    }

    case IDL_RECALL_SK_CANCEL:
     {
       CRecallDlgInfo *pDlgInfo = NULL;
       pDlgInfo = CDialerApp_GetActiveDlgInfo(pMe);

       if(pDlgInfo == NULL)
       {
         DIALER_ERR("Null pointer", 0,0,0);
         return FALSE;
       }
       if( ICM_RecallRsp(pMe->m_pICM, AEECM_RECALL_RESP_CCBS_REJECT, pDlgInfo->callID) != SUCCESS)
       {
         DIALER_ERR("ICM_RecallRsp failed", 0,0,0);
         return FALSE;
       }

     }
     (void) CDialerApp_EndDlg(pMe);
     return TRUE;

   default:
     break;
   }
#endif // defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

   return FALSE;
}


/*=============================================================================
FUNCTION: CDialerApp_SetupRecallDlg_KeyHandler

DESCRIPTION: Handles SetupRecallDlg Dialog's key events

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
boolean CDialerApp_SetupRecallDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
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
        case AVK_CLR:
          // do not let main event handler get event
          return TRUE;
  
        case AVK_END:
        {
           int nRetVal = ICM_EndCall(pMe->m_pICM, pMe->m_byCurrOrigCall);
    
           if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
           {
             (void) CDialerApp_EndCallRelatedDlgs(pMe, pMe->m_byCurrOrigCall);
             DIALER_ERR("ICM_EndCall FAILED", 0, 0, 0);  
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
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SetupRecallDlg_CmdHandler

DESCRIPTION: Handles SetupRecall Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SetupRecallDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
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
     case IDL_SK_SETUP_RECALL:
     {
       AECHAR szStr[2];
       int nRetVal;
       CRecallDlgInfo *pDlgInfo = NULL;

       pDlgInfo = (CRecallDlgInfo*) CDialerApp_GetActiveDlgInfo(pMe);

       szStr[0] = '5';
       szStr[1] = '\0';

       nRetVal = ICM_CallOpsRequest(pMe->m_pICM, szStr, pDlgInfo->callID);
       if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
       {
         DIALER_ERR("ICM_CallOpsRequest FAILED", 0, 0, 0);  
         return FALSE;
       }
       (void) CDialerApp_EndDlg(pMe);
       return TRUE;
     }

    case IDL_SK_SETUP_RECALL_CANCEL:
      {
        int nRetVal = ICM_EndCall(pMe->m_pICM, pMe->m_byCurrOrigCall);

        if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
        {
          (void) CDialerApp_EndCallRelatedDlgs(pMe, pMe->m_byCurrOrigCall);
          DIALER_ERR("ICM_EndCall FAILED", 0, 0, 0);  
          return FALSE;
        }

        (void) CDialerApp_EndDlg(pMe);
        // do not let main event handler get event
        return TRUE;           
      }

   default:
     break;
   }     
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_SSRequestDlg_Init

DESCRIPTION: Initializes the controls for SS Request dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SSRequestDlg_Init(CDialerApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  IDialog *pActiveDlg;
  IStatic *pIStatic;
  IMenuCtl *pISoftKeyMenu; 
  AEERect StaticRect;
  AEERect MenuRect;
  AECHAR *pszStrBeg = NULL;
  AECHAR *pszStrEnd = NULL;
  AECHAR *pszBuf = NULL;
  AECHAR szFormat[] = {'%','s',' ','%','s','\0'};
  CSSRequestDlgInfo *pDlgInfo = NULL;
  uint16 wIDSOp;

  
  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
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

  pDlgInfo = (CSSRequestDlgInfo*) CDialerApp_GetActiveDlgInfo(pMe);
  if(pDlgInfo == NULL)
  {
    DIALER_ERR("NULL Pointer", 0,0,0);
    return FALSE;
  }


  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, 
    IDC_STATIC_SSREQUEST);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_SK_SSREQUEST);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // init buffer
  pszBuf = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE+DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszStrBeg = (AECHAR*) MALLOC ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR));
  pszStrEnd = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  if((pszBuf == NULL) || (pszStrBeg == NULL) || (pszStrEnd == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszBuf);
    FREEIF(pszStrBeg);
    FREEIF(pszStrEnd);
    return FALSE;
  }

  switch(pDlgInfo->ssCode)
  {
  case AEESUPPSVC_CLIP:  
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_CLIP;
    break;

  case AEESUPPSVC_CLIR:  
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_CLIR;
    break;
                                                        
  case AEESUPPSVC_COLP:  
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_COLP;
    break;
                                                        
  case AEESUPPSVC_COLR:   
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_COLR;
    break;
                                                          
  case AEESUPPSVC_ALL_FORWARDING_SS:    
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_ALL_FORWARDING_SS;
    break;
       
  case AEESUPPSVC_CFU:  
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_CFU;
    break;
                        
  case AEESUPPSVC_ALL_CONDFWD_SS:      
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_ALL_CONDFWD_SS;
    break;
        
  case AEESUPPSVC_CFB:   
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_CFB;
    break;
                                                             
  case AEESUPPSVC_CFNRY:     
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_CFNRY;
    break;
                   
  case AEESUPPSVC_CFNRC:  
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_CFNRC;
    break;
                     
  case AEESUPPSVC_CW:   
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_CW;
    break;
                       
  case AEESUPPSVC_ALL_CALL_RSTRCT_SS:      
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_ALL_CALL_RSTRCT_SS;
    break;
     
  case AEESUPPSVC_BARRING_OUTGOING_CALLS:    
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_BARRING_OUTGOING_CALLS;
    break;
   
  case AEESUPPSVC_BAOC:    
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_BAOC;
    break;
                     
  case AEESUPPSVC_BOIC:     
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_BOIC;
    break;
                                                            
  case AEESUPPSVC_BOIC_EXHC:    
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_BOIC_EXHC;
    break;
                                                                                                         
  case AEESUPPSVC_BARRING_OF_INCOMING_CALLS:   
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_BARRING_OF_INCOMING_CALLS;
    break;
 
  case AEESUPPSVC_BAIC:    
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_BAIC;
    break;
                     
  case AEESUPPSVC_BICROAM:     
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_BICROAM;
    break;
                                                                                             
  case AEESUPPSVC_CCBS:
    wIDSOp = IDS_SUPPSYS_SS_CODE_E_TYPE_CCBS;
    break;                                                                                             
    
  default: 
    wIDSOp = IDS_SS;
    break;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, wIDSOp, pszStrBeg, 
                      ((DIALERAPP_MAX_NOTIF_BUFFER_SIZE)*sizeof(AECHAR)));
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_REQUESTING, pszStrEnd, 
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
  WSPRINTF(pszBuf, (sizeof(AECHAR)*(DIALERAPP_MAX_NOTIF_BUFFER_SIZE+DIALERAPP_MAX_STR_SIZE)), szFormat, pszStrBeg, pszStrEnd);

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_CANCELSSREQUEST,
    IDL_SK_SSREQUEST_CANCEL, NULL, 0);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);


  // Set Static control size 
  ISTATIC_GetRect(pIStatic, &StaticRect);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);  
  SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, pMe->m_rc.dy-MenuRect.dy-20); 

  // Static Info Properties
  ISTATIC_SetRect(pIStatic, &StaticRect);
  ISTATIC_SetActive(pIStatic, FALSE);
  ISTATIC_SetProperties(pIStatic, ST_MIDDLETEXT | ST_CENTERTEXT | ST_NOSCROLL); 
  
  // check if alpha should be displayed
  if(WSTRLEN(pDlgInfo->szAlpha) > 0)
  {
    (void) ISTATIC_SetText(pIStatic, NULL, pDlgInfo->szAlpha, AEE_FONT_BOLD, 
      AEE_FONT_BOLD);
  }
  else
  {
    (void) ISTATIC_SetText(pIStatic, NULL, pszBuf, AEE_FONT_BOLD, 
      AEE_FONT_BOLD);
  }
  
  (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_SSREQUEST);
  
  FREE(pszBuf);
  FREE(pszStrBeg);
  FREE(pszStrEnd);

  return TRUE;
#else
  return FALSE;
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
} 

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
/*=============================================================================
FUNCTION: CDialerApp_ConvertOperToRelOp

DESCRIPTION: Convert SS operation into RelOperation Type

PARAMETERS:
  AEESuppSvcSSOperationType ssOper : Input SS Operation Type
  AEESuppSvcReleaseSSOp *relOp   : Output Release Operation Type


RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_ConvertOperToRelOp
 (AEESuppSvcSSOperationType ssOper, 
 AEESuppSvcReleaseSSOp *relOp)
{
  switch(ssOper)
  {
    case AEESUPPSVC_REGISTER_SS:
      *relOp = AEESUPPSVC_REL_REGISTER_SS;
      break;
    case AEESUPPSVC_ERASE_SS:
      *relOp = AEESUPPSVC_REL_ERASE_SS;
      break;
    case AEESUPPSVC_ACTIVATE_SS:
      *relOp = AEESUPPSVC_REL_ACTIVATE_SS;
      break;
    case AEESUPPSVC_DEACTIVATE_SS:
      *relOp = AEESUPPSVC_REL_DEACTIVATE_SS;
      break;
    case AEESUPPSVC_INTERROGATE_SS:
      *relOp = AEESUPPSVC_REL_INTERROGATE_SS;
      break;
    case AEESUPPSVC_REGISTER_PASSWD:
      *relOp = AEESUPPSVC_REL_REG_PASSWD_SS;
      break;
    case AEESUPPSVC_PROCESS_USSD_DATA:
      *relOp = AEESUPPSVC_REL_PROCESS_USS;
      break;
    case AEESUPPSVC_FWD_CHECK_SS_IND:
      *relOp = AEESUPPSVC_REL_FWD_CHECK_SS_IND;
      break;
    case AEESUPPSVC_PROCESS_USSD_REQ:
      *relOp = AEESUPPSVC_REL_PROCESS_USS;
      break;
    case AEESUPPSVC_USS_REQ:
      *relOp = AEESUPPSVC_REL_USS;
      break;
    default:
      *relOp = AEESUPPSVC_REL_NO_OP;
       DIALER_ERR("Invalid Operation, Cannot be cancelled", 0,0,0);
       // still want to swallow this event
      return FALSE;
      
  }

  return TRUE;
}
#endif // defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
//lint -restore 


/*=============================================================================

FILE: DialerDetail.c

SERVICES: Dialer Applet Dialogs

GENERAL DESCRIPTION: Detail Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerDetail.c#42 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/26/08   cvs     Remove deprecated functions
03/19/08   sg      Modify all dialogs to use the new framework
09/18/06   cvs     Fix compiler warnings
07/28/05   AT      Fix compiler warnings and indentation.
06/07/05   aps     Changed 1x<send> call ICM_EndCall() to end calls.
04/06/05   RI      Support for TTY.
03/04/04   JAS     Added code to store detail dialog data in app ptr for diags
01/22/04   BPW     Optimized NV gets
01/08/04   BPW     Use own dialer hyphenation function
12/03/03   BPW     Simplification of call option menu items
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/25/03   BPW     Linted for the Saber 6250 build
11/21/03   BPW     Fixed compile error
11/20/03   BPW     Featurized privacy on/off SKs
11/20/03   BPW     Do not display options SK if no GSM/WCDMA mode
11/19/03   BPW     Added privacy SK and displays
11/13/03   BPW     Added function to format display for phone number
08/29/03   BW      Fixed compile warnings
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
07/24/03   BW      Created DialerDetail Source File 

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
#include "DialerDetail.h"
#include "dialerapp_res.h"
#include "DialerUtils.h"
#include "AppComFunc.h"
#include "ContApp.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static void     CDialerApp_DetailDlg_TimeRefresh(CDialerApp *pMe);
/*===========================================================================

STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_DetailDlg_CmdHandler

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
boolean CDialerApp_DetailDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
   PRINT_FUNCTION_NAME();

   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   switch (ctrl_id) 
   {
     case IDL_SK_DETAIL_SAVE:
     {
       if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrDetailCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE) 
       {
         DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0); 
         return FALSE;
       }

       // make sure we have a other party number
       if(WSTRLEN(pMe->m_CallInfo.other_party_no) > 0)
       {
         IContactApp *pICONTACTAPP = NULL;  

         if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONTACT_APP,(void **) &pICONTACTAPP) != SUCCESS)
         {
           DIALER_ERR("CONTACTAPP Create Failed", 0, 0, 0);
           return FALSE;
         }
         if(pICONTACTAPP == NULL) 
         {
           DIALER_ERR("Null pointer", 0, 0, 0);
           return FALSE; //error
         }

        (void) IContactApp_SaveNumber(pICONTACTAPP, pMe->m_CallInfo.other_party_no, 
           STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT, SAVE_NEW);

        (void) IContactApp_Release(pICONTACTAPP);
       }
       else
       {
         DIALER_ERR("Cannot add empty string to contact app", 0, 0, 0);   
         return FALSE;
       }
     }
     return TRUE;

   case IDL_SK_DETAIL_ADDTO:
     {
       if(IADDRBOOK_GetNumRecs(pMe->m_pIADDRBOOK) == 0)
       {
         DIALER_ERR("Addrbook is empty", 0, 0, 0);   
         return FALSE;
       }

       if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrDetailCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE) 
       {
         DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0); 
         return FALSE;
       }

       // make sure we have a other party number
       if(WSTRLEN(pMe->m_CallInfo.other_party_no) > 0)
       {
         IContactApp *pICONTACTAPP = NULL;  

         if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONTACT_APP,(void **) &pICONTACTAPP) != SUCCESS)
         {
           DIALER_ERR("CONTACTAPP Create Failed", 0, 0, 0);
           return FALSE;
         }
         if(pICONTACTAPP == NULL) 
         {
           DIALER_ERR("Null pointer", 0, 0, 0);
           return FALSE; //error
         }

         (void) IContactApp_SaveNumber(pICONTACTAPP, pMe->m_CallInfo.other_party_no, 
           STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT, SAVE_TO_EXISTING);

         (void) IContactApp_Release(pICONTACTAPP);
       }
       else
       {
         DIALER_ERR("Cannot add empty string to contact app", 0, 0, 0);   
         return FALSE;
       }
     }
     return TRUE;

   case IDL_SK_DETAIL_END:
     {
       int nRetVal;

       if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrDetailCall, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE) 
       {
         DIALER_ERR("Could not get call info", 0,0,0);
         return FALSE;
       }

       nRetVal = ICM_EndCall(pMe->m_pICM, pMe->m_byCurrDetailCall);
       if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
       {
         DIALER_ERR("ICM_EndCall FAILED", 0, 0, 0);  
         return FALSE;
       }

       ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);

       return TRUE;
     }

   case IDL_SK_DETAIL_PRIVACYON:
     if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_SetPrivacyPref(pMe->m_pICM, AEECM_PRIVACY_PREF_ENHANCED)) == FALSE)
     {
       DIALER_ERR("OEMCM_SetPrivacyPref FAILED", 0,0,0);
       return FALSE; 
     }
     return TRUE;


   case IDL_SK_DETAIL_PRIVACYOFF:
     if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_SetPrivacyPref(pMe->m_pICM, AEECM_PRIVACY_PREF_STANDARD)) == FALSE)
     {
       DIALER_ERR("OEMCM_SetPrivacyPref FAILED", 0,0,0);
       return FALSE; 
     }
     return TRUE;


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)   
   case IDL_SK_DETAIL_OPTIONS: 
     pMe->m_byCurrCallOptions = pMe->m_byCurrDetailCall;
     return CDialerApp_CreateDlg(pMe, IDD_CALL_OPTIONS, NULL, 0);
#endif  // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   default:
     return FALSE;
   }
}

/*=============================================================================
FUNCTION: CDialerApp_DetailDlg_EndHandler

DESCRIPTION: Handles DetailDlg Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_DetailDlg_EndHandler(CDialerApp *pMe)
{
   PRINT_FUNCTION_NAME();

   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   (void) ISHELL_CancelTimer(pMe->a.m_pIShell, 
               (PFNNOTIFY) CDialerApp_DetailDlg_TimeRefresh, (void*) pMe);
#ifdef FEATURE_APP_TEST_AUTOMATION
   FREEIF(pMe->m_pDlgData);
   pMe->m_pDlgData = NULL;
#endif
   return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_DetailDlg_Init

DESCRIPTION: Initializes the controls for Detail dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_DetailDlg_Init(CDialerApp *pMe)
{
  IDialog *pActiveDlg;
  IStatic *pIStatic;
  IMenuCtl *pISoftKeyMenu; 
  AEERect StaticRect;
  AEERect MenuRect;

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
    IDC_STATIC_DETAIL);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_SK_DETAIL);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // Set Static control size 
  ISTATIC_GetRect(pIStatic, &StaticRect);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);  
  SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, pMe->m_rc.dy-MenuRect.dy-20); 

  // Static Info Properties
  ISTATIC_SetRect(pIStatic, &StaticRect);
  ISTATIC_SetProperties(pIStatic, ST_MIDDLETEXT | ST_UNDERLINE |ST_CENTERTITLE |ST_CENTERTEXT | ST_NOSCROLL);
  
  (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_DETAIL);
  
  /* Allocate mem for diagnostic data.  Don't fail the display if
   * we can't get memory, since this is only for diagnostic data.
   * Discard any existing data
   */
#ifdef FEATURE_APP_TEST_AUTOMATION
  FREEIF(pMe->m_pDlgData);
  pMe->m_pDlgData = MALLOC(sizeof(CCallDetail));
  if (pMe->m_pDlgData == NULL)
    DIALER_ERR("Malloc failure for detail dlg diag data", 0, 0, 0);  
#endif

  if(CDialerApp_DetailDlg_DisplayRefresh(pMe) != TRUE)
  {
    return FALSE;
  }

  // set timer for display refresh
  (void) ISHELL_SetTimer(pMe->a.m_pIShell, 1000, 
    (PFNNOTIFY) CDialerApp_DetailDlg_TimeRefresh, (void*) pMe);
  return TRUE;
} 

/*=============================================================================
FUNCTION: CDialerApp_DetailDlg_DisplayRefresh

DESCRIPTION: Refreshes the controls for Detail dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_DetailDlg_DisplayRefresh(void *pUser)
{
  CDialerApp* pMe = (CDialerApp*) pUser;
  IDialog *pActiveDlg;
  IStatic *pIStatic;
  IMenuCtl *pISoftKeyMenu; 
  AECHAR *pszTitle = NULL;
  uint16 wSelSK;
  uint32 dwCurrentTime ;
  uint32 dwMinutes;
  uint32 dwSeconds;
  AECHAR szTimeFormat[] = {'%','s',':',' ','%','u',':','%','0','2','u','\0'};
  AECHAR szNumberFormat[] = {'%','s',' ','%','u','\0'};
  AECHAR szCUGFormat[] = {'%','s',':',' ','%','u','\0'};
  AECHAR *pszTimeBuf = NULL;
  AECHAR *pszNumberBuf = NULL;
  AECHAR *pszCUGBuf = NULL;
  AECHAR *pszName = NULL;
#ifdef FEATURE_APP_TEST_AUTOMATION
  CCallDetail *pDetailData;
#endif
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#ifdef FEATURE_TTY
  AECHAR *wBuf = NULL;
#endif /* FEATURE_TTY */
#endif

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
  pDetailData = (CCallDetail *)(pMe->m_pDlgData);
#endif

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);

  // error checking
  if (pActiveDlg == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_DETAIL)
  {
    DIALER_ERR("Detail Dialog is not active", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, IDC_STATIC_DETAIL);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_SK_DETAIL);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // init title buffer
  pszTitle = (AECHAR *) MALLOC ((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  pszCUGBuf = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  pszTimeBuf = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  pszNumberBuf = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  pszName = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  if((pszName == NULL) || (pszTimeBuf == NULL) || (pszNumberBuf == NULL) 
    || (pszTitle == NULL) || (pszCUGBuf == NULL))
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    FREEIF(pszTimeBuf);
    FREEIF(pszNumberBuf);
    FREEIF(pszName);
    FREEIF(pszCUGBuf);
    FREEIF(pszTitle);
    return FALSE; //error
  }

  if (CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrDetailCall,
                                           &(pMe->m_CallInfo),
                                           sizeof(AEECMCallInfo))) == FALSE)
  {
    DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);  
    return FALSE;
  }

  // update sk menu
  wSelSK = IMENUCTL_GetSel(pISoftKeyMenu);
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_END,
                          IDL_SK_DETAIL_END, NULL, 0);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)   
  if((AEECM_IS_CALLSTATE_CONNECTED(pMe->m_CallInfo.call_state) == TRUE) && 
     (AEECM_IS_CALLTYPE_VOICE(pMe->m_CallInfo.call_type) == TRUE))
  {
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_OPTIONS,
                            IDL_SK_DETAIL_OPTIONS, NULL, 0);
  }
#endif  // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

#ifdef FEATURE_AUTH
  // do not want to display these SK unless we have privacy built in
  if(pMe->m_CallInfo.is_privacy == TRUE)
  {
    IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_PRIVACYOFF,
                     IDL_SK_DETAIL_PRIVACYOFF, NULL, 0);
  }
  else
  {
    IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_PRIVACYON,
                     IDL_SK_DETAIL_PRIVACYON, NULL, 0);
  }
#endif //FEATURE_AUTH

  if(WSTRLEN(pMe->m_CallInfo.other_party_no) > 0)
  {
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SAVE,
                            IDL_SK_DETAIL_SAVE, NULL, 0);

    if(IADDRBOOK_GetNumRecs(pMe->m_pIADDRBOOK) > 0)
    {
      (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_ADDTO,
                              IDL_SK_DETAIL_ADDTO, NULL, 0);
    }
  }
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
  IMENUCTL_SetSel(pISoftKeyMenu, wSelSK);

  // select the correct title for the detail dlg
  switch(pMe->m_CallInfo.call_state)
  {     
  case AEECM_CALL_STATE_CONV:  
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                                IDS_CALLCONNECTED, pszTitle,
                                ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
    break;

  case AEECM_CALL_STATE_ONHOLD:  
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                                IDS_CALLONHOLD, pszTitle,
                                ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR))); 
    break;

  case AEECM_CALL_STATE_DORMANT:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                                IDS_CALLDORMANT, pszTitle,
                                ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR))); 
    break;

  default:
    DIALER_MSG("Invalid Call State for Detail Dlg", 0,0,0);
    FREEIF(pszTimeBuf);
    FREEIF(pszNumberBuf);
    FREEIF(pszName);
    FREEIF(pszCUGBuf);
    FREEIF(pszTitle);
    return CDialerApp_EndDlg(pMe); 
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
  // Save the status info
  if (pDetailData != NULL)
    WSTRTOSTR(pszTitle, pDetailData->CallStatus, DIALERAPP_MAX_STR_SIZE);
#endif

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#ifdef FEATURE_TTY
  if ( pMe->m_CallInfo.bTTYcall ){
    wBuf = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
    if (wBuf == NULL){
      DIALER_ERR("No Memory", 0, 0, 0);
      return FALSE;
    }
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                                IDS_TTY_MODE, wBuf, 
                                ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
    (void) WSTRLCAT(pszTitle, wBuf, DIALERAPP_MAX_STR_SIZE);
    FREEIF(wBuf);
  }
#endif /* FEATURE_TTY */
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM)*/
  // Static Info Properties
  (void) ISTATIC_SetText(pIStatic, pszTitle, NULL, AEE_FONT_BOLD,
                         AEE_FONT_NORMAL);

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_CALL,
                              pszName,
                              ((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR)));
  WSPRINTF(pszNumberBuf, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR),
           szNumberFormat, pszName, pMe->m_byCurrDetailCall);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszNumberBuf, NULL, FALSE);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);

  // Store call id
#ifdef FEATURE_APP_TEST_AUTOMATION
  if (pDetailData != NULL)
    pDetailData->CallID = pMe->m_byCurrDetailCall;
#endif

  // display the other party number and name if any in contacts
  CDialerApp_FormatNumberDisplayString(pMe, &(pMe->m_CallInfo), pszName,
                                       DIALERAPP_MAX_STR_SIZE,
                                       pMe->m_bNVAutoHyphen);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, TRUE);

#ifdef FEATURE_APP_TEST_AUTOMATION
  if (pDetailData != NULL)
  {
    WSTRTOSTR(pszName, pDetailData->OtherParty1, DIALERAPP_MAX_STR_SIZE);

    /* Default second "other party" field to empty; we will fill it in
     * if needed
     */
    pDetailData->OtherParty2[0] = '\0';
  }
#endif

  if(WSTRLEN(pMe->m_CallInfo.other_party_no) != 0)
  {
    if(pMe->m_bNVAutoHyphen == TRUE)
    {
      AECHAR *pszHyphen =
               CDialerApp_HyphenateNumberString(pMe->m_CallInfo.other_party_no);
      if(WSTRCMP(pszHyphen, pszName) != 0)
      {
        // want the phone number if it is not null and already being displayed
        (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
        (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszHyphen, NULL, TRUE);
#ifdef FEATURE_APP_TEST_AUTOMATION
        if (pDetailData != NULL)
          WSTRTOSTR(pszHyphen, pDetailData->OtherParty2,
                    DIALERAPP_MAX_STR_SIZE);
#endif
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
#ifdef FEATURE_APP_TEST_AUTOMATION
        if (pDetailData != NULL)
          WSTRTOSTR(pMe->m_CallInfo.other_party_no, pDetailData->OtherParty2,
                    DIALERAPP_MAX_STR_SIZE);
#endif
      }
    }
  }

  // check for CUG info
  if(pMe->m_CallInfo.forward_cug_info.cm_cug_index.present == TRUE)
  {
    // display CUG info
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                                IDS_USERGROUP, pszName,
                                ((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR)));
    WSPRINTF(pszCUGBuf, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR),
             szCUGFormat, pszName,
             ((uint16)((pMe->m_CallInfo.forward_cug_info.cm_cug_index.msb << 8)
                       |(pMe->m_CallInfo.forward_cug_info.cm_cug_index.lsb))));
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszCUGBuf, NULL, TRUE);
#ifdef FEATURE_APP_TEST_AUTOMATION
    if (pDetailData != NULL)
      WSTRTOSTR(pszCUGBuf, pDetailData->CUGString, DIALERAPP_MAX_STR_SIZE);
#endif
  }

  dwCurrentTime = GETUPTIMEMS();
  dwMinutes = (uint32) (((dwCurrentTime - pMe->m_CallInfo.start_time)/1000)/60);
  dwSeconds = ((dwCurrentTime - pMe->m_CallInfo.start_time)/1000) -
              (dwMinutes*60);

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                              IDS_TIMEINCALL, pszName,
                              ((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR)));
  WSPRINTF(pszTimeBuf, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR), szTimeFormat,
           pszName, dwMinutes, dwSeconds);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszTimeBuf, NULL, TRUE);
#ifdef FEATURE_APP_TEST_AUTOMATION
  if (pDetailData != NULL)
    WSTRTOSTR(pszTimeBuf, pDetailData->MinSecTime, DIALERAPP_MAX_STR_SIZE);
#endif

  // check for privacy mode
  if(pMe->m_CallInfo.is_privacy == TRUE)
  {
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE,
                                IDS_ENHANCEDPRIVACY, pszName,
                                ((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR)));
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, TRUE);
#ifdef FEATURE_APP_TEST_AUTOMATION
    if (pDetailData != NULL)
      WSTRTOSTR(pszName, pDetailData->Privacy, DIALERAPP_MAX_STR_SIZE);
#endif
  }

  IDIALOG_Redraw(pActiveDlg);

  // free memory
  FREE(pszTimeBuf);
  FREE(pszNumberBuf);
  FREE(pszName);
  FREE(pszCUGBuf);
  FREE(pszTitle);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_DetailDlg_TimeRefresh

DESCRIPTION: Resets the timer and refreshes the dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CDialerApp_DetailDlg_TimeRefresh(CDialerApp *pMe)
{
  uint16 wActiveDlgID;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL)
  {
    return;
  }

  // reset the timer first
  (void) ISHELL_SetTimer(pMe->a.m_pIShell, 1000,
    (PFNNOTIFY) CDialerApp_DetailDlg_TimeRefresh, (void*) pMe);

  wActiveDlgID = CDialerApp_GetActiveDlgID(pMe);
  if(wActiveDlgID == IDD_DETAIL)
  {
    // Dialog is active so refresh display
    if(CDialerApp_DetailDlg_DisplayRefresh(pMe) == FALSE)
    {
      DIALER_ERR("Detail Refresh Failed", 0,0,0);
      (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                (PFNNOTIFY) CDialerApp_DetailDlg_TimeRefresh, (void*) pMe);
    }
  }
}

/*=============================================================================
FUNCTION: CDialerApp_CallOptionsDlg_Init

DESCRIPTION: Initializes the controls for Call Options dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallOptionsDlg_Init(CDialerApp *pMe)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  IDialog *pActiveDlg;
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
                                         IDC_MENU_CALL_OPTIONS);
  if(pIMenu == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error
  }

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pIMenu);
  (void) IMENUCTL_SetTitle(pIMenu, DIALERAPP_RES_FILE, IDS_CALLOPTIONSTITLE, NULL);
  if(AEECM_IS_ANYCALL_CONNECTED(pMe->m_pICM) == TRUE)
  {
    // get all of the info for the selected call
    if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, pMe->m_byCurrCallOptions, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE) 
    {
      DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);  
      return FALSE;
    }

    // visible if held calls exist
    if(ICM_GetActiveCallIDs(pMe->m_pICM, 
                            (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                            AEECM_CALL_TYPE_EMERGENCY|
                                            AEECM_CALL_TYPE_NON_STD_OTASP|
                                            AEECM_CALL_TYPE_STD_OTASP),
                            (AEECMCallState)AEECM_CALL_STATE_ONHOLD, NULL, 0) > 0)
    {
      // 0 SEND
      (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_ENDHELD,
        IDL_CALL_OPTIONS_ENDHELD, NULL, 0);
    }

    // visible active calls exists
    if(ICM_GetActiveCallIDs(pMe->m_pICM, 
                            (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                            AEECM_CALL_TYPE_EMERGENCY|
                                            AEECM_CALL_TYPE_NON_STD_OTASP|
                                            AEECM_CALL_TYPE_STD_OTASP),
                            (AEECMCallState)AEECM_CALL_STATE_CONV, NULL, 0) > 0)
    {
      // 1 SEND
      (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_ENDACTIVE,
        IDL_CALL_OPTIONS_ENDACTIVE, NULL, 0);
    }

    // check if call is on hold or active
    if(pMe->m_CallInfo.call_state == AEECM_CALL_STATE_ONHOLD)
    {
      // 2 SEND
      (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_UNHOLD,
        IDL_CALL_OPTIONS_UNHOLD, NULL, 0);
    }
    else
    {
      // 2 SEND
      (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_HOLD,
        IDL_CALL_OPTIONS_HOLD, NULL, 0);
    }
      
    // visible if call x is active and 2 or more total active calls
    if((pMe->m_CallInfo.call_state == AEECM_CALL_STATE_CONV) && 
       (ICM_GetActiveCallIDs(pMe->m_pICM, 
                             (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                             AEECM_CALL_TYPE_EMERGENCY|
                                             AEECM_CALL_TYPE_NON_STD_OTASP|
                                             AEECM_CALL_TYPE_STD_OTASP),
                             (AEECMCallState)AEECM_CALL_STATE_CONV, NULL, 0) >= 2))
    {
      // 2 x SEND
      (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_PRIVATE,
        IDL_CALL_OPTIONS_PRIVATECALL, NULL, 0);
    }

    // visible if any held calls and 2 or more total calls
    if((ICM_GetActiveCallIDs(pMe->m_pICM, 
                             (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                             AEECM_CALL_TYPE_EMERGENCY|
                                             AEECM_CALL_TYPE_NON_STD_OTASP|
                                             AEECM_CALL_TYPE_STD_OTASP),
                             (AEECMCallState)AEECM_CALL_STATE_NONE, NULL, 0) >= 2) && 
       (ICM_GetActiveCallIDs(pMe->m_pICM, 
                             (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                             AEECM_CALL_TYPE_EMERGENCY|
                                             AEECM_CALL_TYPE_NON_STD_OTASP|
                                             AEECM_CALL_TYPE_STD_OTASP), 
                             (AEECMCallState)AEECM_CALL_STATE_ONHOLD, NULL, 0) > 0))
    {
      // 3 SEND
      (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_CONFERENCE,
        IDL_CALL_OPTIONS_CONFERENCE, NULL, 0);
    }

    // visible if 2 or more total calls and at least one active
    if((ICM_GetActiveCallIDs(pMe->m_pICM, 
                             (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                             AEECM_CALL_TYPE_EMERGENCY|
                                             AEECM_CALL_TYPE_NON_STD_OTASP|
                                             AEECM_CALL_TYPE_STD_OTASP), 
                             (AEECMCallState)AEECM_CALL_STATE_NONE, NULL, 0) >= 2) && 
       (ICM_GetActiveCallIDs(pMe->m_pICM, 
                             (AEECMCallType)(AEECM_CALL_TYPE_VOICE|
                                             AEECM_CALL_TYPE_EMERGENCY|
                                             AEECM_CALL_TYPE_NON_STD_OTASP|
                                             AEECM_CALL_TYPE_STD_OTASP),
                             (AEECMCallState)AEECM_CALL_STATE_CONV, NULL, 0) > 0))
    {
      // 4 SEND
      (void) IMENUCTL_AddItem(pIMenu, DIALERAPP_RES_FILE, IDS_TRANSFER,
        IDL_CALL_OPTIONS_TRANSFER, NULL, 0);
    }
  }
  else
  {
    // no need to show the options if there is no connected call
    return CDialerApp_EndDlg(pMe);
  }

  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

  (void) IDIALOG_SetFocus(pActiveDlg, IDC_MENU_CALL_OPTIONS);
  IDIALOG_Redraw(pActiveDlg);
  return TRUE;
#else
  return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)   
} 

/*=============================================================================
FUNCTION: CDialerApp_CallOptionsDlg_CmdHandler

DESCRIPTION: Handles CallOptions Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallOptionsDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
   PRINT_FUNCTION_NAME();
   // error checking
   if(pMe == NULL)
   {
     return FALSE;
   }

   switch (ctrl_id)
   {
   case IDL_CALL_OPTIONS_ENDHELD:
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

   case IDL_CALL_OPTIONS_ENDACTIVE:
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

   case IDL_CALL_OPTIONS_UNHOLD:
   case IDL_CALL_OPTIONS_HOLD:
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

   case IDL_CALL_OPTIONS_PRIVATECALL:
     {
       int nRetVal;
       AECHAR szStr[3];

       szStr[0] = '2';
       szStr[1] = pMe->m_byCurrCallOptions;
       szStr[2] = '\0';

       nRetVal = ICM_CallOpsRequest(pMe->m_pICM, szStr, pMe->m_byCurrCallOptions);
       if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
       {
         DIALER_ERR("ICM_CallOpsRequest FAILED", 0, 0, 0);
         return FALSE;
       }

       DIALER_MSG("Sent: 2 %d SEND", pMe->m_byCurrCallOptions, 0, 0);
       ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
       return TRUE;
     }

   case IDL_CALL_OPTIONS_CONFERENCE:
     {
       int nRetVal;
       AECHAR szStr[3];

       szStr[0] = '3';
       szStr[1] = '\0';

       nRetVal = ICM_CallOpsRequest(pMe->m_pICM, szStr, 0);
       if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
       {
         DIALER_ERR("ICM_CallOpsRequest FAILED", 0, 0, 0);
         return FALSE;
       }

       DIALER_MSG("Sent: 3 SEND", 0, 0, 0);
       ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);
       return TRUE;
     }

   case IDL_CALL_OPTIONS_TRANSFER:
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

   default:
     break;
   }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)   

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_CallOptionsDlg_DisplayRefresh

DESCRIPTION: Refreshes the call options dialog display

PARAMETERS:
  *puser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallOptionsDlg_DisplayRefresh(void *pUser)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)   
  CDialerApp* pMe = (CDialerApp*) pUser;
  
  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_CALL_OPTIONS)
  {
    DIALER_ERR("Call Options Dialog is not active", 0,0,0);
    return FALSE;
  }

  return CDialerApp_CallOptionsDlg_Init(pMe);
#else
  return FALSE;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)   
}
//lint -restore 


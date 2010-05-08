/*=============================================================================

FILE: DialerStatus.c

SERVICES: Dialer Applet Dialogs

GENERAL DESCRIPTION: Status Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerStatus.c#71 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/26/08   cvs     Remove deprecated functions
03/19/08   sg      Modify all dialogs to use the new framework
08/02/07   cvs     Featurize all secondary display support
10/24/06   jas     Fix an off-by-one array access
09/18/06   ypmw    Fix the timer status when the call is displayed on VGA 
07/25/06   jas     Fix more (new) compile warnings
07/12/06   cvs     Fix compile warnings
07/10/06   jas     Cleaning up a build warning
05/31/06   RI      Added support for IMS VideoShare.
04/18/06   jas     Ignore SMS calls when displaying active calls
10/14/05   cvs     support for attaching carkit display mid call
10/12/05   cvs     Carkit display
06/06/05   aps     Changed 1x<send> call ICM_EndCall() to end calls.
05/10/05   AT      Only refresh the status screen if there is something to
                   display.  Also, allow us to enter the status screen if we
                   are waiting for the ORIG event from CM.
04/06/05   RI      Support for TTY.
03/11/05   cvs     Optimize Status screen
09/28/04   JAS     Added missed call notification to secondary display
09/20/04   SUN     Fixed Mute and Vocoder settings
03/04/04   JAS     Added code to store status dialog data in app ptr for diags
01/29/04   BPW     Removed GETUPTIMEMS() from for loops
01/22/04   BPW     Optimized NV gets
01/20/03   BPW     DTMF and other memory cleanup and lint errors
01/08/04   BPW     Use own dialer hyphenation function
12/19/03   BPW     USSD & MissedCall now uses own dialg info structure
12/18/03   BPW     End missed call dialog rather than go to status dlg
12/04/03   BPW     Fixed compiler errors
12/03/03   BPW     Simplification of call option menu items
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/25/03   BPW     Added code review changes and comments
11/25/03   BPW     Linted for the Saber 6250 build
11/20/03   BPW     Added displays for OTASP calls
11/20/03   BPW     Do not display options SK if no GSM/WCDMA mode
11/20/03   BPW     Fixed mute SK
11/19/03   BPW     Added privacy mode displays
11/13/03   BPW     Added function to format display for phone number
08/29/03   BW      Fixed compile warnings
08/22/03   BW      Modified the CDialerApp_StatusDlg_GetSelectedCall function
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
07/27/03   BW      Created DialerStatus Source File 

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
#include "AEECM.h"
#include "DialerApp.h"
#include "DialerStatus.h"
#include "dialerapp_res.h"
#include "DialerUtils.h"
#include "AppComFunc.h"
#include "DATASTATSAPP.BID"
#include "ContApp.h"

#ifdef FEATURE_IMS
#ifdef FEATURE_IMS_VIDEOSHARE
#include "DialerNotif.h"
#include "IMS_VIDEOSHARE.BID"
#endif // FEATURE_IMS_VIDEOSHARE
#endif // FEATURE_IMS
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static uint32   CDialerApp_StatusDlg_GetIcon(CDialerApp *pMe, AEECMCallState CallState, AEECMCallType CallType);
static int      CDialerApp_StatusDlg_GetSelectedCall(CDialerApp *pMe);
static boolean  CDialerApp_StatusDlg_SoftKeyRefresh(CDialerApp *pMe);
static boolean  CDialerApp_StatusDlg_AddMenuItem(CDialerApp *pMe, IMenuCtl *pIMenu, AEECMCallID byCurrentCallID, AEECMCallInfo *pCallInfo, int16 wMenuWidth);
static boolean  CDialerApp_StatusDlg_UpdateMenuItem(CDialerApp *pMe, IMenuCtl *pIMenu, AEECMCallID byCurrentCallID, AEECMCallInfo *pCallInfo, int16 wMenuWidth);

/*===========================================================================

STATIC and PUBLIC FUNCTIONS

===========================================================================*/
/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_KeyHandler

DESCRIPTION: Handles Status Dialog's key events

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
boolean CDialerApp_StatusDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
   PRINT_FUNCTION_NAME();
   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   switch(evt)
   {
   case EVT_KEY_PRESS:
     // this is used for bolding and unbolding the 
     // selected call in the menu
     if((key == AVK_LEFT) || (key == AVK_RIGHT))
     {
       CtlAddItem pai;
       uint16 wSelId;
       IDialog	*pActiveDlg = CDialerApp_GetActiveDlg(pMe);
       IMenuCtl* pIMenu;

       // error checking
       if (pActiveDlg == NULL) 
       {
         DIALER_ERR("Null pointer", 0,0,0);
         return FALSE;
       }

       pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
         IDC_MENU_STATUS);
       if(pIMenu == NULL)
       {
         DIALER_ERR("Null pointer", 0,0,0);
         return FALSE; // error 
       }

       if(IMENUCTL_IsActive(pIMenu) == TRUE)
       {
         wSelId = IMENUCTL_GetSel(pIMenu);

         (void) IMENUCTL_GetItem(pIMenu, wSelId, &pai);
         pai.wFont = AEE_FONT_BOLD;
         (void) IMENUCTL_SetItem(pIMenu, wSelId, MSIF_FONT, &pai);
         return TRUE;
       }    
     }

     if((key == AVK_DOWN) || (key == AVK_UP))
     {
       CtlAddItem pai;
       uint16 wSelId;
       IDialog	*pActiveDlg = CDialerApp_GetActiveDlg(pMe);
       IMenuCtl* pIMenu;

       // error checking
       if(pActiveDlg == NULL) 
       {
         DIALER_ERR("Null pointer", 0,0,0);
         return FALSE;
       }

       pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
         IDC_MENU_STATUS);
       if(pIMenu == NULL)
       {
         DIALER_ERR("Null pointer", 0,0,0);
         return FALSE; // error 
       }

       if(IMENUCTL_IsActive(pIMenu) == FALSE)
       {
         wSelId = IMENUCTL_GetSel(pIMenu);

         (void) IMENUCTL_GetItem(pIMenu, wSelId, &pai);
         pai.wFont = AEE_FONT_NORMAL;
         (void) IMENUCTL_SetItem(pIMenu, wSelId, MSIF_FONT, &pai);
         return TRUE;
       }   
       else
       {
         // used to update the softkey menu after up and down key presses
         (void) CDialerApp_StatusDlg_SoftKeyRefresh(pMe);
         IDIALOG_Redraw(pActiveDlg);
       }
     }
     return FALSE;

   case EVT_KEY:
     switch(key)
     {
     case AVK_CLR:
       return CDialerApp_StopApp(pMe);

     default:
       return FALSE;
     }
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_CmdHandler

DESCRIPTION: Handles Status Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StatusDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
   PRINT_FUNCTION_NAME();
   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   switch (ctrl_id) 
   {
   case IDL_STATUS_CALLEND_1:
   case IDL_STATUS_CALLEND_2:
   case IDL_STATUS_CALLEND_3:
   case IDL_STATUS_CALLEND_4:
   case IDL_STATUS_CALLEND_5:
   case IDL_STATUS_CALLEND_6:
   case IDL_STATUS_CALLEND_7:
   case IDL_STATUS_CALLEND_8:
   case IDL_STATUS_CALLEND_9:
   case IDL_STATUS_CALLEND_10:
     // call has ended so don't go to detail dialog
     return TRUE;

   case IDL_SK_STATUS_INFO:
   case IDL_STATUS_CALL_1:
   case IDL_STATUS_CALL_2:
   case IDL_STATUS_CALL_3:
   case IDL_STATUS_CALL_4:
   case IDL_STATUS_CALL_5:
   case IDL_STATUS_CALL_6:
   case IDL_STATUS_CALL_7:
   case IDL_STATUS_CALL_8:
   case IDL_STATUS_CALL_9:
   case IDL_STATUS_CALL_10:
     {
       int nRetVal;
       int nSel = CDialerApp_StatusDlg_GetSelectedCall(pMe);

       if((nSel == -1))
       {
         DIALER_ERR("Bad Call Selection", 0,0,0);
         return FALSE;
       } 

       nRetVal = ICM_GetCallInfo(pMe->m_pICM, (uint8)nSel, &(pMe->m_CallInfo), sizeof(AEECMCallInfo));
       if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
       {
         DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);  
         return FALSE;
       }

       if(AEECM_IS_CALLSTATE_CONNECTED(pMe->m_CallInfo.call_state))
       {
         // For data calls, start DataStats App.
         if(pMe->m_CallInfo.call_type == AEECM_CALL_TYPE_CS_DATA ||
           pMe->m_CallInfo.call_type  == AEECM_CALL_TYPE_PS_DATA)
         {
           char pszArg[2];
           (void) SNPRINTF(pszArg, sizeof(pszArg), "%d", nSel);
           (void) ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_DATASTATS, pszArg);
         }
         else
         {
           pMe->m_byCurrDetailCall = (uint8)nSel;
           (void) CDialerApp_CreateDlg(pMe, IDD_DETAIL, NULL, 0);
         }
       }
       return TRUE;
     }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
   case IDL_SK_STATUS_SSMENU:
     return CDialerApp_CreateDlg(pMe, IDD_SERVICE_PROGRAMMING, NULL, 0);
#else
   case IDL_SK_STATUS_OK:
     return CDialerApp_StopApp(pMe);
#endif //defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)   
   case IDL_SK_STATUS_OPTIONS:
     { 
     int nSel = CDialerApp_StatusDlg_GetSelectedCall(pMe);

     if((nSel == -1))
     {
       DIALER_ERR("Bad Call Selection", 0,0,0);
       return FALSE;
     } 

     pMe->m_byCurrCallOptions = (uint8)nSel;

     return CDialerApp_CreateDlg(pMe, IDD_CALL_OPTIONS, NULL, 0);
     }
#endif //defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   case IDL_SK_STATUS_MUTE:
      CDialerApp_SetMute(pMe, TRUE);
     (void) CDialerApp_StatusDlg_SoftKeyRefresh(pMe);
     return TRUE;

   case IDL_SK_STATUS_UNMUTE:
      CDialerApp_SetMute(pMe, FALSE);
     (void) CDialerApp_StatusDlg_SoftKeyRefresh(pMe);
     return TRUE;

   case IDL_SK_STATUS_END:
     {
       int nRetVal;
       int nSel = CDialerApp_StatusDlg_GetSelectedCall(pMe);

       if((nSel == -1))
       {
         DIALER_ERR("Bad Call Selection", 0,0,0);
         return FALSE;
       } 

       if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_GetCallInfo(pMe->m_pICM, (uint8)nSel, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE) 
       {
         DIALER_ERR("Could not get call info", 0,0,0);
         return FALSE;
       }

       nRetVal = ICM_EndCall(pMe->m_pICM, (uint8)nSel);
       if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE)
       {
          DIALER_ERR("ICM_EndCall FAILED", 0, 0, 0);  
          return FALSE;
       }

       ShowBusyIcon(pMe->a.m_pIShell, pMe->a.m_pIDisplay, &pMe->m_rc, FALSE);

       return TRUE;
     }

#ifdef FEATURE_IMS
#ifdef FEATURE_IMS_VIDEOSHARE
  case IDL_SK_STATUS_VIDEOSHARE:
   { 
     int nRetVal;
     boolean VSAvailable = FALSE;

     int nSel = CDialerApp_StatusDlg_GetSelectedCall(pMe);

     if((nSel == -1)){
       DIALER_ERR("Bad Call Selection", 0,0,0);
       return FALSE;
     } 
     /* get the call details */
     nRetVal = ICM_GetCallInfo(pMe->m_pICM, (uint8)nSel, &(pMe->m_CallInfo), sizeof(AEECMCallInfo));

     if(CDialerApp_CheckAEEReturnStatus(pMe, nRetVal) == FALSE){
         DIALER_ERR("ICM_GetCallInfo FAILED", 0, 0, 0);  
         return FALSE;
     }

     /* The following are to be checked before starting Videoshare
        If it is a voice call AND the SIP registration is still valid AND the call is not
        part of a MPTY call. 
        Note: The other checks on CLI and call state are currently done in
              VideoShare Application.
              There is no combined HOLD for both VideoShare and Voice call.
     */

     VSAvailable = FALSE;

     if ( (pMe->m_bSIPRegStatus) &&
          (pMe->m_CallInfo.call_type == AEECM_CALL_TYPE_VOICE) && 
          ( (pMe->m_CallInfo.call_state == AEECM_CALL_STATE_CONV) || ( pMe->m_CallInfo.call_state == AEECM_CALL_STATE_ONHOLD) ))
     {

       if ( (pMe->m_CallInfo.call_state == AEECM_CALL_STATE_CONV) && 
            ( ICM_GetActiveCallIDs( pMe->m_pICM, AEECM_CALL_TYPE_VOICE,AEECM_CALL_STATE_CONV, NULL, 0) >  1))  
         break;

       VSAvailable = TRUE;     
     }     
     if ( VSAvailable ) {
       (void) ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_IMS_VIDEOSHARE);
     } else { 
       CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_VS_NOT_AVAIL, 
                                      DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, 
                                      DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
     }
     return TRUE;
   }  
#endif // FEATURE_IMS_VIDEOSHARE
#endif // FEATURE_IMS

   default:
     break;
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_NoftifDlg_EndHandler

DESCRIPTION: Handles Noftif Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StatusDlg_EndHandler(CDialerApp *pMe)
{
   int i;

   PRINT_FUNCTION_NAME();
   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   (void) ISHELL_CancelTimer(pMe->a.m_pIShell, 
         (PFNNOTIFY) CDialerApp_StatusDlg_TimeRefresh, (void*) pMe);

   //clear out call names
   for(i=0; i<DIALERAPP_MAX_NUM_CALLS; i++)
   {
     FREEIF(pMe->m_szCallNames[i]);
   }

#ifdef FEATURE_APP_TEST_AUTOMATION
   FREEIF(pMe->m_pDlgData);
   pMe->m_pDlgData = NULL;
   // No need to free the call status pointer, since we never malloc it
   pMe->m_pCallStatus = NULL;
#endif
   return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_Init

DESCRIPTION: Initializes the controls for Status dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StatusDlg_Init(CDialerApp *pMe)
{
  IDialog	*pActiveDlg;
  IMenuCtl *pIMenu;
  IMenuCtl *pISoftKeyMenu; 
  AEERect MenuRect;
  AEERect SKMenuRect;
  
  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    return FALSE;
  }

  pMe->m_bOKToRefreshStatusDlg = FALSE;

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);

  // error checking
  if (pActiveDlg == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_MENU_STATUS);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_SK_STATUS);
  if((pISoftKeyMenu == NULL) || (pIMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }
  
  // Update the softkey look
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);

  // Set Menu control size 
  IMENUCTL_GetRect(pISoftKeyMenu, &SKMenuRect);  
  SETAEERECT(&MenuRect, 10, 10, (pMe->m_rc.dx-20), (pMe->m_rc.dy-SKMenuRect.dy-20)); 


  //Menu Properties
  IMENUCTL_SetRect(pIMenu, &MenuRect);	 
  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);

  (void) IDIALOG_SetFocus(pActiveDlg, IDC_MENU_STATUS);
  
#ifdef FEATURE_APP_TEST_AUTOMATION
  // Discard any old diag data
  FREEIF(pMe->m_pDlgData);

  /* Allocate mem for diagnostic data.  Don't fail the display if
   * we can't get memory, since this is only for diagnostic data
   */
  pMe->m_pDlgData = MALLOC(sizeof(CAllCallStatus));
  if (pMe->m_pDlgData == NULL)
    DIALER_ERR("Malloc failure for status dlg diag data", 0, 0, 0);
  pMe->m_pCallStatus = NULL;

#endif
  
  // update dialog with current call info
  if (CDialerApp_StatusDlg_DisplayInit(pMe) == FALSE)
  {
    return FALSE;
  }
  else
  {
    // set timer for time refresh
    (void) ISHELL_SetTimer(pMe->a.m_pIShell, 1000, 
      (PFNNOTIFY) CDialerApp_StatusDlg_TimeRefresh, (void*) pMe);
    return TRUE;
  }
} 

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_SoftKeyRefresh

DESCRIPTION: Refreshes the soft key menu depending on what call is selected

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_StatusDlg_SoftKeyRefresh(CDialerApp *pMe)
{
  IDialog	*pActiveDlg;
  IMenuCtl *pISoftKeyMenu;
  uint16 wSelectedSK;
#if (!defined (FEATURE_WCDMA)) && (!defined (FEATURE_GSM))
  boolean bNoOKSK = FALSE;
#endif // !defined (FEATURE_WCDMA) || !defined (FEATURE_GSM)

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

  // Get controls in dialog
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_SK_STATUS);
  if(pISoftKeyMenu == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // get the currently selected sk
  wSelectedSK = IMENUCTL_GetSel(pISoftKeyMenu);

  // update the mute selection if it changes
  if((wSelectedSK == IDL_SK_STATUS_MUTE) && (pMe->m_bMuted == TRUE))
  {
    wSelectedSK = IDL_SK_STATUS_UNMUTE;
  }
  else if((wSelectedSK == IDL_SK_STATUS_UNMUTE) && (pMe->m_bMuted == FALSE))
  {
    wSelectedSK = IDL_SK_STATUS_MUTE;
  }

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  if(CDialerApp_IsCallsToDisplay(pMe) == TRUE)
  {
    int nSel = CDialerApp_StatusDlg_GetSelectedCall(pMe);

    if(nSel == -1)
    {
      DIALER_ERR("Selection is incorrect", 0, 0, 0);  
      return FALSE;
    }

    if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, (uint8)nSel, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == TRUE)
    {
      if(AEECM_IS_CALLSTATE_CONNECTED(pMe->m_CallInfo.call_state))
      {  
#if (!defined (FEATURE_WCDMA)) && (!defined (FEATURE_GSM))
        bNoOKSK = TRUE;
#endif // !defined (FEATURE_WCDMA) && !defined (FEATURE_GSM)
        // active call
        (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_INFO,
          IDL_SK_STATUS_INFO, NULL, 0);

        (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_END,
            IDL_SK_STATUS_END, NULL, 0);

        if(AEECM_IS_CALLTYPE_VOICE(pMe->m_CallInfo.call_type) == TRUE)
        {
          // decide which mute to display
          if(pMe->m_bMuted == FALSE)
          {
            (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_MUTE,
              IDL_SK_STATUS_MUTE, NULL, 0);
          }
          else
          {
            (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_UNMUTE,
              IDL_SK_STATUS_UNMUTE, NULL, 0);
          }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)          
          (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_OPTIONS,
            IDL_SK_STATUS_OPTIONS, NULL, 0); 
#endif //defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
        }

#ifdef FEATURE_IMS
#ifdef FEATURE_IMS_VIDEOSHARE     
        /* 
           1. SIP registration status must be available.
           2. There must be at least one call that is active 
              ( rest of the checks are made when the user selects a call for VS)
        */
        if ( pMe->m_bSIPRegStatus ){
          if ( ICM_GetActiveCallIDs(pMe->m_pICM, AEECM_CALL_TYPE_VOICE,
                                    (AEECMCallState)(AEECM_CALL_STATE_CONV | AEECM_CALL_STATE_ONHOLD),
                                    NULL, 0) >=  1) 
          {
            (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_VIDEOSHARE,
                                   IDL_SK_STATUS_VIDEOSHARE, NULL, 0); 
          }
        }             
#endif // FEATURE_IMS_VIDEOSHARE         
#endif // FEATURE_IMS
      }
    }
  }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SSMENU,
    IDL_SK_STATUS_SSMENU, NULL, 0);
#else
  if(bNoOKSK == FALSE)
  {
    IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_OK,
      IDL_SK_STATUS_OK, NULL, 0);
  }
#endif //defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

  // set back the selection
  IMENUCTL_SetSel(pISoftKeyMenu, wSelectedSK);

  // redraw the menu
  (void) IMENUCTL_Redraw(pISoftKeyMenu);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_GetSelectedCall

DESCRIPTION: returns the call number of the selected call

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  int: id of the selected call and -1 if an error has occured

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CDialerApp_StatusDlg_GetSelectedCall(CDialerApp *pMe)
{
  IDialog	*pActiveDlg;
  IMenuCtl *pIMenu;
  int nSel;
  uint32 dwData;
  
  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return (-1);
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);
  if(pActiveDlg == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return (-1);
  }

  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_MENU_STATUS);
  if(pIMenu == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return (-1);
  }

  nSel = IMENUCTL_GetSel(pIMenu);

  if(nSel == 0)
  {
    DIALER_ERR("Bad Selection", 0,0,0);
    return (-1);
  }

  (void) IMENUCTL_GetItemData(pIMenu, (uint16)nSel, &dwData);

  if(dwData > DIALERAPP_MAX_NUM_CALLS)
  {
    DIALER_ERR("Call ID out of Bounds", 0,0,0);
    return (-1);
  }

  return((int)dwData); 
}

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_DisplayInit

DESCRIPTION: redraws the entire status dialog display and gets updated info

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StatusDlg_DisplayInit(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog	*pActiveDlg = NULL;
  IMenuCtl* pIMenu = NULL;
  IMenuCtl* pISoftKeyMenu = NULL;
  AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
  AEERect MenuRect;
  uint16 wActiveDlgID;
  uint16 wNumCallIDs;
  uint32 dwSelected;
  uint32 dwCurrentTime;
  boolean ActiveCalls[DIALERAPP_MAX_NUM_CALLS] = {FALSE};
  int i;
  AECHAR *pszTitle = NULL;
  AECHAR *pszTitleCentered = NULL;
  AEECMCallType    CallTypes;
  AEECMCallState   CallStates;
#ifdef FEATURE_APP_TEST_AUTOMATION
  uint16 NextDiagRecIdx;             // Index to next position to store data
  CAllCallStatus *pDiagInfo;         // Pointer to diag data storage
#endif
  
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
  
  wActiveDlgID = CDialerApp_GetActiveDlgID(pMe);
  if(wActiveDlgID != IDD_STATUS)
  {
    // Dialog is not active so do not change display
    DIALER_ERR("Refresh Non Status Dlg", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_MENU_STATUS);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_SK_STATUS);
  if((pISoftKeyMenu == NULL) || (pIMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // allocate memory for string buffers
  pszTitle = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  if(pszTitle == NULL)
  {
      DIALER_ERR("No Memory", 0, 0, 0);
      return FALSE; //error
  }
  
  // get the menu rect
  IMENUCTL_GetRect(pIMenu, &MenuRect);

  //clear out call names
  for(i=0; i<DIALERAPP_MAX_NUM_CALLS; i++)
  {
    FREEIF(pMe->m_szCallNames[i]);
  }

  // get array of call ids - NO OTAPA calls or SMS calls
  CallTypes = (AEECMCallType)(AEECM_CALL_TYPE_VOICE| AEECM_CALL_TYPE_CS_DATA|
                              AEECM_CALL_TYPE_PS_DATA| AEECM_CALL_TYPE_PD|
                              AEECM_CALL_TYPE_TEST| AEECM_CALL_TYPE_STD_OTASP|
                              AEECM_CALL_TYPE_NON_STD_OTASP|
                              AEECM_CALL_TYPE_EMERGENCY);
  CallStates = (AEECMCallState)(AEECM_CALL_STATE_CONV| AEECM_CALL_STATE_ONHOLD|
                                AEECM_CALL_STATE_DORMANT);
  wNumCallIDs = ICM_GetActiveCallIDs(pMe->m_pICM, CallTypes, CallStates,
                                     (AEECMCallID*)CallIDs, 
                                     sizeof(CallIDs));

#ifdef FEATURE_APP_TEST_AUTOMATION
  pDiagInfo = (CAllCallStatus *)(pMe->m_pDlgData);
#endif

  // check for calls to display
  pMe->m_bLastCallsToDisplay = CDialerApp_IsCallsToDisplay(pMe);
  if(pMe->m_bLastCallsToDisplay == FALSE)
  { // no calls are present to display
  
    if ((pMe->m_bIconStartedApp == TRUE) || (pMe->m_bWaitingforOrigVoiceCall == TRUE))
    {

#ifdef FEATURE_APP_TEST_AUTOMATION
      // empty the diag data (set number of calls to zero)
      pDiagInfo->yNumDisplayedCalls = 0;
#endif

      // delete all of the calls from menu
      (void) IMENUCTL_DeleteAll(pIMenu);

      // load the title string
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_NOACTIVE, 
        pszTitle, ((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR)));

      // set the SK to active
      (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_STATUS);
    }
    else
    {
      // free buffer memory
      FREE(pszTitle);

      // return to core since nothing left to display
      return CDialerApp_StopApp(pMe);
    }
  }
  else
  { // calls are active so display them

    // record the old selection
    dwSelected = IMENUCTL_GetSel(pIMenu);

    // delete all of the calls 
    (void) IMENUCTL_DeleteAll(pIMenu);

    // load the menu title
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_STATUSTITLE, 
      pszTitle, ((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR)));

    // update array with active calls 
    for(i=0; i<wNumCallIDs; i++)
    {
      ActiveCalls[CallIDs[i]] = TRUE;
    }
    //Save state for refresh
    for(i=0; i<ARR_SIZE(ActiveCalls); i++)
    {
      pMe->m_bLastActiveCalls[i] = ActiveCalls[i];
    }

    dwCurrentTime = GETUPTIMEMS();

#ifdef FEATURE_APP_TEST_AUTOMATION
    NextDiagRecIdx = 0;
#endif

    // traverse the active call array
    for(i=0; i<DIALERAPP_MAX_NUM_CALLS; i++)
    {
      // Check to see if there is a active call
      if(ActiveCalls[i] == TRUE)
      {
        // get all of the info for the current call id
        if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, (uint8)i, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE) 
        {
          DIALER_ERR("ICM_GetCallInfo FAILED for ID %c", i, 0, 0);  
        } 
        else 
        {

#ifdef FEATURE_APP_TEST_AUTOMATION
          if (pDiagInfo != NULL)
            pMe->m_pCallStatus = &(pDiagInfo->CallStatus[NextDiagRecIdx++]);
#endif

          // add the menu item
          (void) CDialerApp_StatusDlg_AddMenuItem(pMe, pIMenu, (uint8)i,
                                             &(pMe->m_CallInfo), MenuRect.dx);
        }
      }

      // check to see if there is an ended call
      if(pMe->m_EndedCalls[i] != NULL)
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
        if (pDiagInfo != NULL)
            pMe->m_pCallStatus = &(pDiagInfo->CallStatus[NextDiagRecIdx++]);
#endif

        // perform some ended processing
        if(pMe->m_EndedCalls[i]->dwDisplayStart == 0)
        {
          // first time the ended call is displayed so time stamp it
          pMe->m_EndedCalls[i]->dwDisplayStart = dwCurrentTime;

          // add the menu item
          (void) CDialerApp_StatusDlg_AddMenuItem(pMe, pIMenu, (uint8)i,
                                            &(pMe->m_EndedCalls[i]->CallInfo),
                                            MenuRect.dx);

        }
        else if((pMe->m_EndedCalls[i]->dwDisplayStart+DIALERAPP_MAX_ENDED_DISPLAY_TIME) <= dwCurrentTime)
        {
          // remove the struct since it has expired
          FREEIF(pMe->m_EndedCalls[i]);
          pMe->m_EndedCalls[i] = NULL;

          //Don't display it
        }
        else //normal ended call.  Keep displaying it
        {
          // add the menu item
          (void) CDialerApp_StatusDlg_AddMenuItem(pMe, pIMenu, (uint8)i,
                                            &(pMe->m_EndedCalls[i]->CallInfo),
                                            MenuRect.dx);
        }
      }
    }

#ifdef FEATURE_APP_TEST_AUTOMATION
    if (pMe->m_pDlgData != NULL)
    {
      ((CAllCallStatus *)(pMe->m_pDlgData))->yNumDisplayedCalls =
                                          (uint8)IMENUCTL_GetItemCount(pIMenu);
    }
#endif

    // set the old selection back
    IMENUCTL_SetSel(pIMenu, (uint16)dwSelected);

    // if menu item is selected and SK is active make it bold  
    if(IMENUCTL_IsActive(pIMenu) == FALSE)
    {
      CtlAddItem pai;

      (void) IMENUCTL_GetItem(pIMenu, (uint16)dwSelected, &pai);
      pai.wFont = AEE_FONT_BOLD;
      (void) IMENUCTL_SetItem(pIMenu, (uint16)dwSelected, MSIF_FONT, &pai); 
    }
  }

  // Center and set the title
  (void) CDialerApp_CenterTitle(pMe, pszTitle, &pszTitleCentered, (unsigned)(int)MenuRect.dx, AEE_FONT_BOLD);
  (void) IMENUCTL_SetTitle(pIMenu, DIALERAPP_RES_FILE, 0, pszTitleCentered);
  (void) IMENUCTL_Redraw(pIMenu);

  // Update the softkey menu
  (void) CDialerApp_StatusDlg_SoftKeyRefresh(pMe);

  // free buffer memory
  FREEIF(pszTitleCentered);
  FREEIF(pszTitle);

#ifdef FEATURE_APP_TEST_AUTOMATION
  pMe->m_pCallStatus = NULL;
#endif

  PRINT_FUNCTION_NAMEN(2);

  pMe->m_bOKToRefreshStatusDlg = TRUE;
  (void) ISHELL_SetTimer(pMe->a.m_pIShell, 1000, 
                         (PFNNOTIFY) CDialerApp_StatusDlg_TimeRefresh,
                         (void*) pMe);

  return TRUE;
} 

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_DisplayRefresh

DESCRIPTION: refreshes call info on status screen.  Calls 
  CDialerApp_StatusDlg_DisplayInit if it's more complicated

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StatusDlg_DisplayRefresh(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog	*pActiveDlg = NULL;
  IMenuCtl* pIMenu = NULL;
  IMenuCtl* pISoftKeyMenu = NULL;
  AEECMCallID CallIDs[DIALERAPP_MAX_NUM_CALLS];
  AEERect MenuRect;
  uint16 wActiveDlgID;
  uint16 wNumCallIDs;
  boolean ActiveCalls[DIALERAPP_MAX_NUM_CALLS] = {FALSE};
  int i;
  AEECMCallType  CallTypes;
  AEECMCallState CallStates;
  
  PRINT_FUNCTION_NAMEN(1);
  MSG_HIGH("CDialerApp_StatusDlg_DisplayRefresh: 1", 0, 0, 0);

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
  
  wActiveDlgID = CDialerApp_GetActiveDlgID(pMe);
  if(wActiveDlgID != IDD_STATUS)
  {
    // Dialog is not active so do not change display
    DIALER_ERR("Refresh Non Status Dlg", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_MENU_STATUS);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_SK_STATUS);
  if((pISoftKeyMenu == NULL) || (pIMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // get the menu rect
  IMENUCTL_GetRect(pIMenu, &MenuRect);

  // get array of call ids - NO OTAPA calls
  CallTypes = (AEECMCallType)(AEECM_CALL_TYPE_VOICE| AEECM_CALL_TYPE_CS_DATA|
                              AEECM_CALL_TYPE_PS_DATA| AEECM_CALL_TYPE_PD|
                              AEECM_CALL_TYPE_TEST| AEECM_CALL_TYPE_STD_OTASP|
                              AEECM_CALL_TYPE_NON_STD_OTASP|
                              AEECM_CALL_TYPE_EMERGENCY);
  CallStates = (AEECMCallState)(AEECM_CALL_STATE_CONV| AEECM_CALL_STATE_ONHOLD|
                                AEECM_CALL_STATE_DORMANT);
  wNumCallIDs = ICM_GetActiveCallIDs(pMe->m_pICM, CallTypes, CallStates,
                                     (AEECMCallID*)CallIDs, sizeof(CallIDs));

  //If we haven't run Init yet, run it now
  if (!pMe->m_bOKToRefreshStatusDlg) {
    return CDialerApp_StatusDlg_DisplayInit(pMe);
  }

  // check for calls to display
  if(CDialerApp_IsCallsToDisplay(pMe) == FALSE)
  { // no calls are present to display
    //Handle this where the whole dialog is refreshed.
    return CDialerApp_StatusDlg_DisplayInit(pMe);
  }

  // calls are active
  if(pMe->m_bLastCallsToDisplay == FALSE)
  {
    //No calls last time.  Need more complicated drawing
    //Handle this where the whole dialog is refreshed.
    return CDialerApp_StatusDlg_DisplayInit(pMe);
  }

  // calls are active, and displayed.  Time for updating.

  // update array with active calls 
  for(i=0; i<wNumCallIDs; i++)
  {
    ActiveCalls[CallIDs[i]] = TRUE;
  }

  for(i=0; i<ARR_SIZE(ActiveCalls); i++)
  {
    if (pMe->m_bLastActiveCalls[i] != ActiveCalls[i]) {
      //The calls have changed.
      //Handle this where the whole dialog is refreshed.
      return CDialerApp_StatusDlg_DisplayInit(pMe);
    }
    if(pMe->m_EndedCalls[i] != NULL) {
      //There are ended calls.
      //Handle this where the whole dialog is refreshed.
      return CDialerApp_StatusDlg_DisplayInit(pMe);
    }

    // Check to see if there is a active call
    if(ActiveCalls[i] == TRUE)
    {
      // get all of the info for the current call id
      if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, (uint8)i, &(pMe->m_CallInfo), sizeof(AEECMCallInfo))) == FALSE) 
      {
        DIALER_ERR("ICM_GetCallInfo FAILED for ID %c", i, 0, 0);  
      } 
      else 
      {
        // update the menu item
        CDialerApp_StatusDlg_UpdateMenuItem(pMe, pIMenu, (uint8)i, 
                                           &(pMe->m_CallInfo), MenuRect.dx);
      }
    }
  }

  PRINT_FUNCTION_NAMEN(2);

  (void) IMENUCTL_Redraw(pIMenu);

  PRINT_FUNCTION_NAMEN(3);

  return TRUE;
} 

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_AddMenuItem

DESCRIPTION: Adds item to the status menu

PARAMETERS:
  *pMe: CDialerApp object pointer
  *pIMenu: pointer to status menu object 
  byCurrentCallID: current call id for item to add
  *pCallInfo: current call info for item to add
  wMenuWidth: width of the menu

RETURN VALUE:
  boolean: returns TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_StatusDlg_AddMenuItem(CDialerApp *pMe,
                                                IMenuCtl *pIMenu, 
                                                AEECMCallID byCurrentCallID,
                                                AEECMCallInfo *pCallInfo,
                                                   int16 wMenuWidth)
{
  CtlAddItem menuItem;
  uint32 dwCurrentTime;
  uint16 wNumSpaces;
  uint16 wStringLength; 
  int i;
  AECHAR cPrivacy;
  AECHAR *pszTime = NULL;
  AECHAR *pszString = NULL;
  AECHAR *pszDisplay= NULL; 
  AECHAR szStringFormat[] = {'%','u','.',' ','%','s','\0'};
  AECHAR szTimeFormat[] = {'%','u',':','%','0','2','u','\0'};
  AECHAR szDisplayFormat[] = {'%','s','%','s','%','c','\0'};
  AECHAR szSpace[] = {' ','\0'}; 
  boolean bRetVal;
  const uint16 dwMaxLength = 11;
#ifdef FEATURE_APP_TEST_AUTOMATION
  CCallStatus *pDiagRec;
#endif
  
  PRINT_FUNCTION_NAME();
  // error checking
  if((pMe == NULL) || (pCallInfo == NULL))
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
  pDiagRec = pMe->m_pCallStatus;
#endif

  // allocate memory for string buffers
  pszString = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  pszDisplay = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  pszTime = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE ) * sizeof(AECHAR));
  if (pMe->m_szCallNames[byCurrentCallID] == NULL) {
    pMe->m_szCallNames[byCurrentCallID] = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE)* sizeof(AECHAR));
  }
  
  if((pMe->m_szCallNames[byCurrentCallID] == NULL) || (pszTime == NULL) || (pszDisplay == NULL) || (pszString == NULL))
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    FREEIF(pszTime);
    FREEIF(pszString);
    FREEIF(pszDisplay);
    FREEIF(pMe->m_szCallNames[byCurrentCallID]);
    return FALSE;
  }

  // fill out menu item parameters
  
  menuItem.pImage = NULL;
  menuItem.wText = 0;
  menuItem.pszResText = NULL;
  menuItem.pszResImage = DIALERAPP_RES_FILE;
  menuItem.wFont = AEE_FONT_NORMAL;
  if(pCallInfo->call_state == AEECM_CALL_STATE_ENDED)
  {
    menuItem.wItemID = DIALERAPP_STATUS_CONTROL_OFFSET + byCurrentCallID + DIALERAPP_MAX_NUM_CALLS;
    menuItem.dwData = 0; // no data since we cannot see details of an ended call
  }
  else
  {
    menuItem.wItemID = DIALERAPP_STATUS_CONTROL_OFFSET + byCurrentCallID;
    menuItem.dwData = byCurrentCallID;
  }
  menuItem.wImage = (uint16)CDialerApp_StatusDlg_GetIcon(pMe, pCallInfo->call_state, pCallInfo->call_type);
  
#ifdef FEATURE_APP_TEST_AUTOMATION
  if (pDiagRec != NULL)
  {
    pDiagRec->CallID = byCurrentCallID;
    pDiagRec->CallState = pCallInfo->call_state;
    pDiagRec->CallType = pCallInfo->call_type;
  }
#endif
  
  if((pCallInfo->call_type == AEECM_CALL_TYPE_STD_OTASP) ||
     (pCallInfo->call_type == AEECM_CALL_TYPE_NON_STD_OTASP))
  {
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_ACTIVATION, pMe->m_szCallNames[byCurrentCallID], (dwMaxLength * sizeof(AECHAR)));    
  }
  else
  {
    boolean bAutoHyphen = pMe->m_bNVAutoHyphen;

    CDialerApp_FormatNumberDisplayString(pMe, pCallInfo, pMe->m_szCallNames[byCurrentCallID], DIALERAPP_MAX_STR_SIZE, bAutoHyphen);
    if(WSTRLEN(pMe->m_szCallNames[byCurrentCallID]) > dwMaxLength)
    {
      if(bAutoHyphen == TRUE)
      {
        // try it again without the hyphens
        CDialerApp_FormatNumberDisplayString(pMe, pCallInfo, pMe->m_szCallNames[byCurrentCallID], DIALERAPP_MAX_STR_SIZE, FALSE);
      }

      if(WSTRLEN(pMe->m_szCallNames[byCurrentCallID]) > dwMaxLength)
      {
        // need to truncate because 11 is the max chars that will fit
        pMe->m_szCallNames[byCurrentCallID][10] = '.';
        pMe->m_szCallNames[byCurrentCallID][11] = '.';
        pMe->m_szCallNames[byCurrentCallID][12] = '.';
        pMe->m_szCallNames[byCurrentCallID][13] = 0;
      }
    }
  }


  WSPRINTF(pszDisplay, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR), szStringFormat, byCurrentCallID, pMe->m_szCallNames[byCurrentCallID]);
#ifdef FEATURE_APP_TEST_AUTOMATION
  if (pDiagRec != NULL)
    WSTRTOSTR(pMe->m_szCallNames[byCurrentCallID], pDiagRec->OtherParty, DIALERAPP_MAX_STR_SIZE);
#endif

  // get current time
  dwCurrentTime = GETUPTIMEMS();

  // print the time
  if(AEECM_IS_CALLSTATE_CONNECTED(pCallInfo->call_state))
  {
    uint32 dwMinutes = (uint32) (((dwCurrentTime - pCallInfo->start_time)/1000)/60);
    uint32 dwSeconds = ((dwCurrentTime - pCallInfo->start_time)/1000)-(dwMinutes*60);

    WSPRINTF(pszTime, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR), szTimeFormat, dwMinutes, dwSeconds);
#ifdef FEATURE_APP_TEST_AUTOMATION
    if (pDiagRec != NULL)
      WSTRTOSTR(pszTime, pDiagRec->MinSecTime, DIALERAPP_MAX_STR_SIZE);
#endif
  }
  else if(pCallInfo->call_state == AEECM_CALL_STATE_ENDED)
  {

      // display the end time
      uint32 dwMinutes = (uint32) ((pCallInfo->duration/1000)/60);
      uint32 dwSeconds = ((pCallInfo->duration)/1000)-(dwMinutes*60);

    /* Calculate and store time for diags.  If we need to blink the time,
     * we will empty the displayed string after storing it.
     */
    WSPRINTF(pszTime, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR), szTimeFormat,
             dwMinutes, dwSeconds);
#ifdef FEATURE_APP_TEST_AUTOMATION
    if (pDiagRec != NULL)
      WSTRTOSTR(pszTime, pDiagRec->MinSecTime, DIALERAPP_MAX_STR_SIZE);
#endif

    // handled displaying ended calls
    if(((dwCurrentTime/1000)%2) == 0)
    {
      // set string to empty for a blinking time
      *pszTime = 0;
    }
  }

  // add spaces so the times line up
  wStringLength = (uint16) IDISPLAY_MeasureText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, pszDisplay); 
  if((30 + wStringLength + pMe->m_dwTimeLength) >= (uint16) wMenuWidth)
  {
    wNumSpaces = 0;  
  }
  else
  {
    wNumSpaces = (uint16)(((unsigned)(int)(wMenuWidth - 30 - wStringLength) - pMe->m_dwTimeLength)/pMe->m_dwSpaceLength);
  }
  for(i=0; i<wNumSpaces; i++)
  {
    (void) WSTRLCAT(pszDisplay, szSpace, DIALERAPP_MAX_STR_SIZE);
  }

  // check for privacy mode
  if ((pCallInfo->is_privacy) &&
      (pCallInfo->call_state != AEECM_CALL_STATE_ENDED))
    cPrivacy = 'P';
  else 
    cPrivacy = 0;

#ifdef FEATURE_APP_TEST_AUTOMATION
  if (pDiagRec != NULL)
    pDiagRec->Privacy = cPrivacy;
#endif

  // build display string
  WSPRINTF(pszString, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR), szDisplayFormat, pszDisplay, pszTime, cPrivacy);
  menuItem.pText = pszString;

  // add the item to the menu passed in
  bRetVal = IMENUCTL_AddItemEx(pIMenu, &menuItem);

  // free strings
  FREEIF(pszTime);
  FREEIF(pszString);
  FREEIF(pszDisplay);

  return bRetVal;
}

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_UpdateMenuItem

DESCRIPTION: Updates item within the status menu

PARAMETERS:
  *pMe: CDialerApp object pointer
  *pIMenu: pointer to status menu object 
  byCurrentCallID: current call id for item to add
  *pCallInfo: current call info for item to add
  wMenuWidth: width of the menu

RETURN VALUE:
  boolean: returns TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_StatusDlg_UpdateMenuItem(CDialerApp *pMe,
                                                IMenuCtl *pIMenu,
                                                AEECMCallID byCurrentCallID,
                                                AEECMCallInfo *pCallInfo,
                                                   int16 wMenuWidth)
{
  uint32 dwCurrentTime;
  uint32 dwNumDisplayChar;
  uint32 dwNumSpaces;
  uint32 dwStringLength;
  int i;
  AECHAR cPrivacy;
  AECHAR *pszTime = NULL;
  AECHAR *pszCallID = NULL;
  AECHAR *pszDisplay = NULL;
  AECHAR szCallIDFormat[] = {'%','u','.',' ','%','s','\0'};
  AECHAR szTimeFormat[] = {'%','u',':','%','0','2','u','\0'};
  AECHAR szSpace = ' '; 
  
  PRINT_FUNCTION_NAMEN(1);
  MSG_HIGH("CDialerApp_StatusDlg_UpdateMenuItem:", 0, 0, 0);

  // error checking
  if ((pMe == NULL) || (pCallInfo == NULL))
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }
  do
  {
    // allocate memory for time string buffer
    pszTime = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE ) * sizeof(AECHAR));
    pszCallID = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));

    if ((pszTime == NULL) || (pszCallID == NULL))
    {
      DIALER_ERR("No Memory at %x, %x", pszTime, pszCallID, 0);
      break;
    }

    // get call ID and call number
    WSPRINTF(pszCallID, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR), szCallIDFormat,
             byCurrentCallID, pMe->m_szCallNames[byCurrentCallID]);
    dwNumDisplayChar = WSTRLEN(pszCallID); 

    // get current time
    dwCurrentTime = GETUPTIMEMS();
    // print the time
    if (AEECM_IS_CALLSTATE_CONNECTED(pCallInfo->call_state))
    {
      uint32 dwMinutes = (uint32) (((dwCurrentTime -
                                     pCallInfo->start_time)/1000)/60);
      uint32 dwSeconds = ((dwCurrentTime - pCallInfo->start_time)/1000)-
                         (dwMinutes*60);

      WSPRINTF(pszTime, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR), szTimeFormat,
               dwMinutes, dwSeconds);
    }
    dwNumDisplayChar += WSTRLEN(pszTime); 

    // check for privacy mode
    if ((pCallInfo->is_privacy) &&
        (pCallInfo->call_state != AEECM_CALL_STATE_ENDED))
    {
      cPrivacy = 'P';
    }
    else 
    {
      cPrivacy = 0;   
    }
    dwNumDisplayChar++; //add one for the Privacy char

    // add spaces so the times line up
    dwStringLength = (uint16) IDISPLAY_MeasureText(pMe->a.m_pIDisplay,
                                                   AEE_FONT_NORMAL, pszCallID); 
    if ((30 + dwStringLength + pMe->m_dwTimeLength) >= (uint16) wMenuWidth)
    {
      dwNumSpaces = 0;  
    }
    else
    {
      /* Calculate the number of spaces:
       * 1) Start with the whole menu width in pixels
       * 2) Subtract a 30-pixel buffer for the margins
       * 3) Subtract the size of the call ID and called number string in pixels
       * 4) Subtrace the size of the time string in pixels
       * 5) Divide by the size of a single space character
       */
      dwNumSpaces = (uint16)(((unsigned)(int)(wMenuWidth - 30 - dwStringLength) -
                              pMe->m_dwTimeLength)/pMe->m_dwSpaceLength);
    }
    dwNumDisplayChar = dwNumDisplayChar + dwNumSpaces + 1; //add 1 for '\0'

    // Allocate buffer for final display 
    pszDisplay = (AECHAR *) MALLOC((dwNumDisplayChar) * sizeof(AECHAR));
    if (pszDisplay == NULL)
    {
      DIALER_ERR("No Memory at %x", pszDisplay, 0, 0);
      break;
    }
    (void) WSTRLCPY(pszDisplay, pszCallID, dwNumDisplayChar);
    for (i=0; i<dwNumSpaces; i++)
    {
      pszDisplay[i+WSTRLEN(pszCallID)] = szSpace;
    }
    pszDisplay[i+WSTRLEN(pszCallID)] = (AECHAR)'\0';

    //Build the final string to be displayed in the menu
    WSTRLCAT(pszDisplay, pszTime, dwNumDisplayChar);
    
    /* If we don't have enough room for the privacy
     * and NULL characters, log an error.  Otherwise,
     * add the privacy character.  (Just being paranoid
     * here.)
     */
    if (WSTRLEN(pszDisplay) > (dwNumDisplayChar - 2))
    {
       DIALER_ERR("Dialer string too long.", 0, 0, 0);
    }
    else
    {
      pszDisplay[dwNumDisplayChar-2] = cPrivacy;
    }

    /* Need a NULL terminator, regardless of string length */
    pszDisplay[dwNumDisplayChar-1] = (AECHAR)'\0';

    // update the item
    IMENUCTL_SetItemText(pIMenu,
                         DIALERAPP_STATUS_CONTROL_OFFSET + byCurrentCallID,
                         NULL, 0, pszDisplay);
  } while(0);

  // free strings
  FREEIF(pszTime);
  FREEIF(pszCallID);
  FREEIF(pszDisplay);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_TimeRefresh

DESCRIPTION: Calls the display refresh function and resets the timer again

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_StatusDlg_TimeRefresh(void *pUser)
{
  CDialerApp* pMe = (CDialerApp*) pUser;
  uint16 wActiveDlgID;
  uint32 dwCurrentTime;
  int i;
  
  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  } 

  // check to see if suspended
  if(pMe->m_bSuspended == TRUE)
  {
    DIALER_MSG("Do not refresh because of suspend", 0,0,0);
    return;
  }

  if (CDialerApp_IsCallsToDisplay(pMe))
  {
    (void) ISHELL_SetTimer(pMe->a.m_pIShell, 1000, 
                           (PFNNOTIFY) CDialerApp_StatusDlg_TimeRefresh,
                           (void*) pMe);
  }
  else
  {
    DIALER_MSG("Do not refresh again - nothing to display", 0, 0, 0);
    (void) ISHELL_CancelTimer(pMe->a.m_pIShell, 
              (PFNNOTIFY) CDialerApp_StatusDlg_TimeRefresh, (void*) pMe);
  }

  wActiveDlgID = CDialerApp_GetActiveDlgID(pMe);
  if(wActiveDlgID == IDD_STATUS)
  {
    // Dialog is active so refresh display
    if(CDialerApp_StatusDlg_DisplayRefresh(pMe) == FALSE)
    {
      DIALER_ERR("Status Refresh Failed", 0,0,0);
      (void) ISHELL_CancelTimer(pMe->a.m_pIShell, 
                (PFNNOTIFY) CDialerApp_StatusDlg_TimeRefresh, (void*) pMe);
    }
  } 
  else
  {
    dwCurrentTime = GETUPTIMEMS();

    // check to see if we need to get rid of a ended calls info
    for(i=0; i<DIALERAPP_MAX_NUM_CALLS; i++)
    {
      if(pMe->m_EndedCalls[i] != NULL)
      {
        if(pMe->m_EndedCalls[i]->dwDisplayStart != 0)
        {
          if((pMe->m_EndedCalls[i]->dwDisplayStart + DIALERAPP_MAX_ENDED_DISPLAY_TIME) <= dwCurrentTime) 
          {
            // remove the struct since it has expired
            FREEIF(pMe->m_EndedCalls[i]);
            pMe->m_EndedCalls[i] = NULL;
          }
        }
      }
    }
  }
} 

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_GetIcon

DESCRIPTION: gets the correct picture to display in menu

PARAMETERS:
  *pMe: CDialerApp object pointer
   CallState: state of call
   CallType: type of call

RETURN VALUE:
  uint32: returns the picture ID

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 CDialerApp_StatusDlg_GetIcon(CDialerApp *pMe, AEECMCallState CallState, AEECMCallType CallType)
{  
  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return IDB_EMPTY;
  }

  switch(CallType)
  {
  case AEECM_CALL_TYPE_EMERGENCY:
    switch(CallState)
    {
    case AEECM_CALL_STATE_CONV:
#ifdef FEATURE_TTY
    if( pMe->m_CallInfo.bTTYcall )
      return IDB_TTY;
#endif /* FEATURE_TTY */

      return IDB_EMERGENCY;

    case AEECM_CALL_STATE_ONHOLD: 

#ifdef FEATURE_TTY
    if( pMe->m_CallInfo.bTTYcall )
      return IDB_TTYONHOLD;
#endif /* FEATURE_TTY */

      return IDB_EMERGENCYHOLD;  

    case AEECM_CALL_STATE_ENDED: 

#ifdef FEATURE_TTY
    if( pMe->m_CallInfo.bTTYcall )
      return IDB_TTYENDED;
#endif /* FEATURE_TTY */

      return IDB_EMERGENCYEND;

    case AEECM_CALL_STATE_DORMANT:
      return IDB_DORMANT;

    default:
      break; //IDB_EMPTY
    }
    break;

  case AEECM_CALL_TYPE_VOICE:
  case AEECM_CALL_TYPE_STD_OTASP:
  case AEECM_CALL_TYPE_NON_STD_OTASP:
    switch(CallState)
    {
    case AEECM_CALL_STATE_CONV: 

#ifdef FEATURE_TTY
    if ( ( CallType == AEECM_CALL_TYPE_VOICE ) && ( pMe->m_CallInfo.bTTYcall ))
      return IDB_TTY;
#endif /* FEATURE_TTY */

      return IDB_VOICE;

    case AEECM_CALL_STATE_ONHOLD: 

#ifdef FEATURE_TTY
    if ( ( CallType == AEECM_CALL_TYPE_VOICE ) && ( pMe->m_CallInfo.bTTYcall ))
      return IDB_TTYONHOLD;
#endif /* FEATURE_TTY */

      return IDB_VOICEHOLD;  

    case AEECM_CALL_STATE_ENDED: 

#ifdef FEATURE_TTY
    if ( ( CallType == AEECM_CALL_TYPE_VOICE ) && ( pMe->m_CallInfo.bTTYcall ))
      return IDB_TTYENDED;
#endif /* FEATURE_TTY */

      return IDB_VOICEEND;

    case AEECM_CALL_STATE_DORMANT:
      return IDB_DORMANT;

    default:
      break; //IDB_EMPTY
    }
    break;

  case AEECM_CALL_TYPE_TEST:
  case AEECM_CALL_TYPE_SMS:
  case AEECM_CALL_TYPE_PD:
  case AEECM_CALL_TYPE_PS_DATA: 
    switch(CallState)
    {
    case AEECM_CALL_STATE_CONV:  
      return IDB_DATA; 		

    case AEECM_CALL_STATE_ONHOLD: 
      return IDB_DATAHOLD;  

    case AEECM_CALL_STATE_ENDED: 
      return IDB_DATAEND;

    case AEECM_CALL_STATE_DORMANT:
      return IDB_DORMANT;

    default:
      break; //IDB_EMPTY
    }
    break;

  case AEECM_CALL_TYPE_CS_DATA: 
    switch(CallState)
    {
    case AEECM_CALL_STATE_CONV:  
      return IDB_FAX;	

    case AEECM_CALL_STATE_ONHOLD: 
      return IDB_FAXHOLD;  

    case AEECM_CALL_STATE_ENDED: 
      return IDB_FAXEND;

    case AEECM_CALL_STATE_DORMANT:
      return IDB_DORMANT;

    default:
      break; //IDB_EMPTY
    }
    break;

  default:
    break;
  }
  	
  return IDB_EMPTY;
}

/*=============================================================================
FUNCTION: CDialerApp_MissedCallDlg_KeyHandler

DESCRIPTION: Handles MissedCallDlg Dialog's key events

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
boolean CDialerApp_MissedCallDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
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
      case AVK_END:
      case AVK_0:
      case AVK_1:
      case AVK_2:
      case AVK_3:
      case AVK_4:
      case AVK_5:
      case AVK_6:
      case AVK_7:
      case AVK_8:
      case AVK_9:
      case AVK_STAR:
      case AVK_POUND:
      case AVK_UP:
      case AVK_DOWN:
      case AVK_LEFT:
      case AVK_RIGHT:
      case AVK_SELECT:
      case AVK_SEND:
      case AVK_CLR:
      case AVK_MESSAGE:  
      case AVK_MENU:
      case AVK_MUTE:
        // these key events will make the dialog goes away
        (void) CDialerApp_EndDlg(pMe);
        pMe->m_wNumMissedCalls = 0;
        return FALSE;

      default:
        return FALSE; 
      }
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_MissedCallDlg_CmdHandler

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
boolean CDialerApp_MissedCallDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
   PRINT_FUNCTION_NAME();

   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

   switch (ctrl_id) 
   {
   case IDL_SK_MISSEDCALL_OK:
     (void) CDialerApp_EndDlg(pMe);
     pMe->m_wNumMissedCalls = 0;
     return TRUE;

   case IDL_SK_MISSEDCALL_SAVE:	
     {
       CMissedCallDlgInfo *pDlgInfo = NULL;

       pDlgInfo = (CMissedCallDlgInfo*) CDialerApp_GetActiveDlgInfo(pMe);
       if(pDlgInfo == NULL)
       {
         DIALER_ERR("NULL Pointer", 0,0,0);
         (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
         // still want to swallow this event
         return TRUE;
       }

       // make sure we have a other party number
       if(WSTRLEN(pDlgInfo->MissedCallInfo.other_party_no) > 0)
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

        (void) IContactApp_SaveNumber(pICONTACTAPP, pDlgInfo->MissedCallInfo.other_party_no, 
           STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT, SAVE_NEW);

        (void) CDialerApp_EndDlg(pMe);

        (void) IContactApp_Release(pICONTACTAPP);
       }
       else
       {
         DIALER_ERR("Cannot add empty string to contact app", 0, 0, 0);   
         return FALSE;
       }
     }
     return TRUE;

   case IDL_SK_MISSEDCALL_ADDTO:		
     {
       CMissedCallDlgInfo *pDlgInfo = NULL;

       if(IADDRBOOK_GetNumRecs(pMe->m_pIADDRBOOK) == 0)
       {
         DIALER_ERR("Addrbook is empty", 0, 0, 0);   
         return FALSE;
       }

       pDlgInfo = (CMissedCallDlgInfo*) CDialerApp_GetActiveDlgInfo(pMe);
       if(pDlgInfo == NULL)
       {
         DIALER_ERR("NULL Pointer", 0,0,0);
         (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
         // still want to swallow this event
         return TRUE;
       }

       // make sure we have a other party number
       if(WSTRLEN(pDlgInfo->MissedCallInfo.other_party_no) > 0)
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

         (void) IContactApp_SaveNumber(pICONTACTAPP, pDlgInfo->MissedCallInfo.other_party_no, 
           STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT, SAVE_TO_EXISTING);

         (void) CDialerApp_EndDlg(pMe);

         (void) IContactApp_Release(pICONTACTAPP);
       }
       else
       {
         DIALER_ERR("Cannot add empty string to contact app", 0, 0, 0);   
         return FALSE;
       }
     }
     return TRUE;


   default:
     return FALSE;
   }
}

/*=============================================================================
FUNCTION: CDialerApp_MissedCallDlg_EndHandler

DESCRIPTION: Handles MissedCallDlg Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_MissedCallDlg_EndHandler(CDialerApp *pMe)
{
   PRINT_FUNCTION_NAME();

   // error checking 
   if(pMe == NULL) 
   { 
     return FALSE;
   }

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
   /* If there's a valid start time, then we need to return to Status
    * mode and let the callback update the display.  Otherwise, there's
    * nothing to display, so clean up.
    */
   if (pMe->m_dwStartTS)
   {
     pMe->m_DispState = DIALER_DISP_STATE_STATUS;
   }
   else
   {
     pMe->m_DispState = DIALER_DISP_STATE_NONE;

     if (pMe->m_pIDisplay2 && CDialerApp_GetSecText(pMe))
     {
       ISTATIC_Release(pMe->m_pSecText);
       pMe->m_pSecText = NULL;

       // Update the display to remove the notification
       IDISPLAY_Update(pMe->m_pIDisplay2);
     }

     if (CDialerApp_IsAnyUSBCarkitConnected())
     {
       CDialerApp_PrintToCarkit(NULL);
     }
   }
#endif //FEATURE_UI_SECONDARY_DISP_SUPPORT

   return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_MissedCallDlg_Init

DESCRIPTION: Initializes the controls for MissedCall dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_MissedCallDlg_Init(CDialerApp *pMe)
{
  IDialog	*pActiveDlg;
  IStatic *pIStatic;
  IMenuCtl *pISoftKeyMenu; 
  AEERect StaticRect;
  AEERect MenuRect;
  AECHAR *pszTitle = NULL;
  AECHAR *pszTitleBuf = NULL;
  AECHAR *pszTime = NULL;
  AECHAR *pszString = NULL;
  AECHAR *pszTimePostfix = NULL;
  AECHAR *pszName = NULL;
  AECHAR *pszHyphenatedNum = NULL;
  AECHAR szTitleFormat[] = {'%','u',' ','%','s','\0'};
  AECHAR szTimeFormat[] = {'%','u',':','%','0','2','u',' ','%','s','\0'};
  AECHAR szAM[] = {'A','M','\0'};
  AECHAR szPM[] = {'P','M','\0'};
  JulianType MissedCallTime;   
  CMissedCallDlgInfo *pDlgInfo = NULL;
  
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

  pDlgInfo = (CMissedCallDlgInfo*) CDialerApp_GetActiveDlgInfo(pMe);
  if(pDlgInfo == NULL)
  {
    DIALER_ERR("NULL Pointer", 0,0,0);
    (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
    // still want to swallow this event
    return TRUE;
  }
  
  // get time formated
  GETJULIANDATE(pDlgInfo->MissedCallInfo.start_time, &MissedCallTime); 

  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, 
    IDC_STATIC_MISSEDCALL);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_SK_MISSEDCALL);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // init title buffer
  pszString = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  pszTime = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  pszTitle = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  pszTitleBuf = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  pszName = (AECHAR*) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  if((pszTitleBuf == NULL) || (pszTitle == NULL) || (pszTime == NULL) || (pszString == NULL) ||
     (pszName == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszTitle);
    FREEIF(pszTitleBuf);
    FREEIF(pszTime);
    FREEIF(pszString);
    FREEIF(pszName);
    return FALSE;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_MISSEDCALLS, pszTitle, 
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_LASTMISSED, pszString, 
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
  WSPRINTF(pszTitleBuf, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR), szTitleFormat, pMe->m_wNumMissedCalls, pszTitle);  

  // format the time for 12 hour display
  if(MissedCallTime.wHour > 12) 
  {
    if(MissedCallTime.wHour == 24)
    {
      pszTimePostfix = szAM;
    }
    else
    {
      pszTimePostfix = szPM;
    }

    MissedCallTime.wHour = MissedCallTime.wHour - 12; 
  }
  else
  {
    if(MissedCallTime.wHour == 12)
    {
      pszTimePostfix = szPM;
    }
    else
    {
      pszTimePostfix = szAM;
    }
  }

  WSPRINTF(pszTime, (DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR), szTimeFormat, MissedCallTime.wHour, MissedCallTime.wMinute, pszTimePostfix);  

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_OK,
    IDL_SK_MISSEDCALL_OK, NULL, 0);
  if(WSTRLEN(pDlgInfo->MissedCallInfo.other_party_no) > 0)
  {
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SAVE,
      IDL_SK_MISSEDCALL_SAVE, NULL, 0);

    if(IADDRBOOK_GetNumRecs(pMe->m_pIADDRBOOK) > 0)
    {
      (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_ADDTO,
        IDL_SK_MISSEDCALL_ADDTO, NULL, 0);
    }
  }
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);

  // Set Static control size 
  ISTATIC_GetRect(pIStatic, &StaticRect);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);  
  SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, pMe->m_rc.dy-MenuRect.dy-20); 

  // Static Info Properties
  ISTATIC_SetRect(pIStatic, &StaticRect);	
  ISTATIC_SetProperties(pIStatic, ST_MIDDLETEXT | ST_UNDERLINE |ST_CENTERTITLE |ST_CENTERTEXT | ST_NOSCROLL); 
  (void) ISTATIC_SetText(pIStatic, pszTitleBuf, NULL, AEE_FONT_BOLD, AEE_FONT_NORMAL);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszString, NULL, FALSE);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszTime, NULL, TRUE);
  (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);

  // display the other party number and name if any in contacts
  if(pDlgInfo->MissedCallInfo.call_type == AEECM_CALL_TYPE_EMERGENCY)
  { // emrgency call
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_EMERGENCY, pszName, (DIALERAPP_MAX_STR_SIZE * sizeof(AECHAR)));
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, TRUE);    
  }
  else if(WSTRLEN(pDlgInfo->MissedCallInfo.other_party_no) == 0)
  {
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_UNKNOWN, 
                        pszName, ((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR)));
    (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, TRUE); 
  }
  else
  {
    if(pMe->m_bNVAutoHyphen == TRUE)
    {
      pszHyphenatedNum = CDialerApp_HyphenateNumberString(pDlgInfo->MissedCallInfo.other_party_no);
      (void) ISTATIC_SetTextEx(pIStatic, (byte *) pszHyphenatedNum, NULL, TRUE);
    }
    else
    {
      (void) ISTATIC_SetTextEx(pIStatic, (byte*) pDlgInfo->MissedCallInfo.other_party_no, NULL, TRUE); 
    }

    DoNumberLookup(pMe->m_pIADDRBOOK, pDlgInfo->MissedCallInfo.other_party_no, pszName, DIALERAPP_MAX_STR_SIZE);
    if(*pszName != '\0')
    {    
      (void) ISTATIC_SetTextEx(pIStatic, (byte*) "\n", NULL, TRUE);
      (void) ISTATIC_SetTextEx(pIStatic, (byte*) pszName, NULL, TRUE);
    }
  }
  
  (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_MISSEDCALL);
  IDIALOG_Redraw(pActiveDlg);

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
  // Put the notification on the secondary display
  pMe->m_DispState = DIALER_DISP_STATE_MISSEDCALL;
  if (pMe->m_pIDisplay2 && (CDialerApp_GetSecText(pMe)))
  {
    ISTATIC_SetText(pMe->m_pSecText, NULL, pszTitleBuf, AEE_FONT_LARGE,
                    AEE_FONT_LARGE);
    ISTATIC_Redraw(pMe->m_pSecText);
  }

  if (CDialerApp_IsAnyUSBCarkitConnected())
  {
    CDialerApp_PrintToCarkit(pszTitleBuf);
  }
#endif

  // free memory
  FREE(pszTitle);
  FREE(pszTitleBuf);
  FREE(pszTime);
  FREE(pszString);
  FREE(pszName);
  FREEIF(pszHyphenatedNum);
  // don't need to free postfix since it points to a local var

  return TRUE;
} 
//lint -restore 

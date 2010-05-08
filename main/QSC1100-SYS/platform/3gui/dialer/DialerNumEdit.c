/*=============================================================================

FILE: DialerNumEdit.c

SERVICES: Dialer Applet Dialogs

GENERAL DESCRIPTION: NumEdit and Wildcard Dialogs

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerNumEdit.c#78 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
08/22/07   cvs     Add framework for dialogs
12/15/06   alb     Changed prototype of ISUPPSVC_StartSS to indicate whether
                   FDN check is required or not.
09/18/06   cvs     Integrate brew 4.0 changes from BTG.
08/22/06   jas     Dialed digits dialog box should not select text on focus.
06/01/06   cvs     fix race condition when dialer is suspended while 
                   entering a number
03/27/06   ng      Featurized VT app.
08/10/05   AT      Added a new text property to selectively validate input
                   in dialed digits strings.  We want this property set.
06/06/05   aps     Code fix to handle errors for changepin Supp Svc
05/23/05   JAS     Fixing display of substring matches.
05/18/05   AT      When matching a number with wild cards, fill in the
                   wild card characters with those already enterred when
                   doing the comparison.
04/26/05   RK      Fixed CR issue caused because of dialing long numbers
03/09/05   sun     Fixed Compile Errors
12/14/04   AT      Fix a problem when two pauses are back to back.
12/07/04   JAS     Correcting mask for dialed number completion.
10/07/04   AT      Make sure a STOP sound is sent with multi-sounds.
07/02/04   SUN     Get the current value of AutoHyphen
04/05/04   BPW     Alert changes for failed calls
02/10/04   BPW     Added WMS App support
01/22/04   BPW     Optimized NV gets
01/21/04   BPW     Added ISuppSvc API changes
01/20/03   BPW     DTMF and other memory cleanup and lint errors
01/08/04   BPW     Modified return values after send key press
01/07/04   BPW     Removed the Find SK
12/18/03   BPW     End timepause dialog rather than go to status dlg
12/10/03   BPW     Adding functionality to Video Call SK
12/09/03   BPW     Empty string error messages and removed BlockID
12/08/03   BPW     Changed Select key press on auto complete menu
12/03/03   BPW     Fixed compiler warning
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/25/03   BPW     Linted for the Saber 6250 build
11/21/03   BPW     Fixed compile error
11/20/03   BPW     Temporarily removed SS Wizard SK
11/20/03   BPW     Made timepause refresh function global
11/19/03   BPW     Added number matching display and functionality
11/17/03   BPW     Increased Max dialed digits to 40 and made rect dynamic
11/13/03   BPW     Press and hold clear to clear contents of text control
10/21/03   SUN     Fixed Compile Errors
10/09/03   SUN     Check for Partial SS also after SS StringCheck
09/04/03   SUN     Changes related to AEESUPPSVC
08/29/03   BW      Modified BurstDTMF params
08/22/03   BW      Added TimePauseDlg and functionality
08/19/03   BW      Added messages, param checking, and more code clean up
08/19/03   SUN     Changed AEESYS to AEESUPPSYS
08/14/03   BW      Integration with both ICM and ISuppSvc
07/25/03   BW      Removed bid include
06/25/03   BW      Created DialerNumEdit Source File
=============================================================================*/
//lint -save -e611
//lint -save -e740
//lint -save -esym(528, CDialerApp_NumEditDlg_ShowWizardSK)

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
#include "DialerUtils.h"
#include "DialerNumEdit.h"
#include "DialerNotif.h"
#include "DialerSups.h"
#include "dialerapp_res.h"
#include "AppComFunc.h"
#include "ContApp.h"
#include "AEEAddrBookExt.h"
#include "OEMText.h"

#ifdef FEATURE_QVPHONE_APP
#include "VideoPhone.h"
#endif /* FEATURE_QVPHONE_APP */

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static boolean       CDialerApp_NumEditDlg_Send(CDialerApp *pMe);
static void          CDialerApp_NumEditDlg_ADNLookup(CDialerApp *pMe);
static void          CDialerApp_NumEditDlg_RedrawNumberMatchMenu(CDialerApp *pMe);
static boolean       CDialerApp_NumEditDlg_GetNumberMatch(CDialerApp *pMe, uint32 dwData);
static boolean       CDialerApp_WildCardDlg_Send(CDialerApp *pMe);
static boolean       CDialerApp_TimePauseDlg_Send(CDialerApp *pMe);
static snd_tone_type CDialerApp_TimePauseDlg_DigitToSound(AECHAR cDigit);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
static void          CDialerApp_NumEditDlg_CheckForSS(CDialerApp *pMe);
#if 0
static boolean       CDialerApp_NumEditDlg_ShowWizardSK(CDialerApp *pMe, boolean bDisplay);
#endif // 0
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

/*===========================================================================

STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_KeyHandler

DESCRIPTION: Handles NumEdit Dialog's key events

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
boolean CDialerApp_NumEditDlg_KeyHandler(CDialerApp *pMe,
                                         AEEEvent evt,
                                         uint16 key)
{
  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
  {
    return FALSE;
  }

  switch (evt)
  {
    case EVT_KEY_PRESS:
      switch (key)
      {
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
          {
            // we do not want to select the number matching menu if it is empty
            IDialog *pActiveDlg = CDialerApp_GetActiveDlg(pMe);
            ITextCtl *pITextCtl = NULL;
            IMenuCtl *pIMenu = NULL;
            IMenuCtl *pSKMenu = NULL;

            if (pActiveDlg == NULL)
            {
              DIALER_ERR("Null pointer", 0,0,0);
              return FALSE;
            }

            pITextCtl = (ITextCtl*)IDIALOG_GetControl(pActiveDlg, IDC_TEXT_NUMEDIT);
            pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_MENU_NUMEDIT);
            pSKMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_SK_NUMEDIT);
            if ((pITextCtl == NULL) || (pIMenu == NULL) || (pSKMenu == NULL))
            {
              DIALER_ERR("Null pointer", 0,0,0);
              return FALSE;
            }

            if (ITEXTCTL_IsActive(pITextCtl) == TRUE)
            {
              if (key == AVK_UP)
              {
                IMENUCTL_SetActive(pIMenu, FALSE);
                IMENUCTL_SetActive(pSKMenu, FALSE);
                ITEXTCTL_SetActive(pITextCtl, FALSE);
                pMe->m_wNextNumEditDlg = IDC_SK_NUMEDIT;
              }
              else if (key == AVK_DOWN)
              {
                IMENUCTL_SetActive(pIMenu, FALSE);
                IMENUCTL_SetActive(pSKMenu, FALSE);
                ITEXTCTL_SetActive(pITextCtl, FALSE);
                if (IMENUCTL_GetItemCount(pIMenu) == 0)
                {
                  pMe->m_wNextNumEditDlg = IDC_SK_NUMEDIT;
                }
                else
                {
                  pMe->m_wNextNumEditDlg = IDC_MENU_NUMEDIT;
                }
              }
            }
            else if (IMENUCTL_IsActive(pIMenu) == TRUE)
            {
              if (key == AVK_LEFT)
              {
                IMENUCTL_SetActive(pIMenu, FALSE);
                IMENUCTL_SetActive(pSKMenu, FALSE);
                ITEXTCTL_SetActive(pITextCtl, FALSE);
                pMe->m_wNextNumEditDlg = IDC_TEXT_NUMEDIT;
              }
              else if (key == AVK_RIGHT)
              {
                IMENUCTL_SetActive(pIMenu, FALSE);
                IMENUCTL_SetActive(pSKMenu, FALSE);
                ITEXTCTL_SetActive(pITextCtl, FALSE);
                pMe->m_wNextNumEditDlg = IDC_SK_NUMEDIT;
              }
              else if (key == AVK_UP)
              {
                if (IMENUCTL_GetItemID(pIMenu, 0) == IMENUCTL_GetSel(pIMenu))
                {
                  IMENUCTL_SetActive(pIMenu, FALSE);
                  IMENUCTL_SetActive(pSKMenu, FALSE);
                  ITEXTCTL_SetActive(pITextCtl, FALSE);
                  pMe->m_wNextNumEditDlg = IDC_TEXT_NUMEDIT;
                }
              }
              else if (key == AVK_DOWN)
              {
                int nCount = IMENUCTL_GetItemCount(pIMenu);

                if (IMENUCTL_GetItemID(pIMenu, (nCount-1)) == IMENUCTL_GetSel(pIMenu))
                {
                  IMENUCTL_SetActive(pIMenu, FALSE);
                  IMENUCTL_SetActive(pSKMenu, FALSE);
                  ITEXTCTL_SetActive(pITextCtl, FALSE);
                  pMe->m_wNextNumEditDlg = IDC_SK_NUMEDIT;
                }
              }
            }
            else if (IMENUCTL_IsActive(pSKMenu) == TRUE)
            {
              if (key == AVK_UP)
              {
                IMENUCTL_SetActive(pIMenu, FALSE);
                IMENUCTL_SetActive(pSKMenu, FALSE);
                ITEXTCTL_SetActive(pITextCtl, FALSE);
                if (IMENUCTL_GetItemCount(pIMenu) == 0)
                {
                  pMe->m_wNextNumEditDlg = IDC_TEXT_NUMEDIT;
                }
                else
                {
                  pMe->m_wNextNumEditDlg = IDC_MENU_NUMEDIT;
                }
              }
              else if (key == AVK_DOWN)
              {
                IMENUCTL_SetActive(pIMenu, FALSE);
                IMENUCTL_SetActive(pSKMenu, FALSE);
                ITEXTCTL_SetActive(pITextCtl, FALSE);
                pMe->m_wNextNumEditDlg = IDC_TEXT_NUMEDIT;
              }
            }

            break;
          }

        case AVK_POUND:
          // ADN Lookup
          (void) ISHELL_SetTimer(pMe->a.m_pIShell, 200,
                                 (PFNNOTIFY) CDialerApp_NumEditDlg_ADNLookup, (void*) pMe);
          (void) ISHELL_SetTimer(pMe->a.m_pIShell, 250,
                                 (PFNNOTIFY) CDialerApp_NumEditDlg_RedrawNumberMatchMenu, (void*) pMe);
          break;

        case AVK_CLR:
          pMe->m_wTextCtlIDToClear = IDC_TEXT_NUMEDIT;
          // press and hold clear timer
          (void) ISHELL_SetTimer(pMe->a.m_pIShell, DIALERAPP_CLEAR_TEXTCTL_TIMEOUT,
                                 (PFNNOTIFY) CDialerApp_ClearTextCtl, (void*) pMe);
          (void) ISHELL_SetTimer(pMe->a.m_pIShell, 250,
                                 (PFNNOTIFY) CDialerApp_NumEditDlg_RedrawNumberMatchMenu, (void*) pMe);
          break;

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
          (void) ISHELL_SetTimer(pMe->a.m_pIShell, 250,
                                 (PFNNOTIFY) CDialerApp_NumEditDlg_RedrawNumberMatchMenu, (void*) pMe);
          break;

        default:
          break;
      }
      // let main event handler get event also
      return FALSE;


    case EVT_KEY_RELEASE:
      {
        /*get the number from the text control */
        (void)CDialerApp_TextCtlGetText(pMe, pMe->m_pszDialString,
                                        DIALERAPP_NUMBER_BUFFER_SIZE, 
                                        IDC_TEXT_NUMEDIT);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
        // ss wizard check
        if (pMe->m_LastKeyPressed == '#')
        {
          (void) ISHELL_SetTimer(pMe->a.m_pIShell, 50,
                                 (PFNNOTIFY) CDialerApp_NumEditDlg_CheckForSS, (void*) pMe);
        }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

        (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_ClearTextCtl, (void*) pMe);
        // let main event handler get event also

        if (pMe->m_wNextNumEditDlg != 0)
        {
          // we do not want to select the number matching menu if it is empty
          IDialog *pActiveDlg = CDialerApp_GetActiveDlg(pMe);
          ITextCtl *pITextCtl = NULL;
          IMenuCtl *pIMenu = NULL;
          IMenuCtl *pSKMenu = NULL;

          if (pActiveDlg == NULL)
          {
            DIALER_ERR("Null pointer", 0,0,0);
            return FALSE;
          }

          pITextCtl = (ITextCtl*)IDIALOG_GetControl(pActiveDlg, IDC_TEXT_NUMEDIT);
          pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_MENU_NUMEDIT);
          pSKMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_SK_NUMEDIT);
          if ((pITextCtl == NULL) || (pIMenu == NULL) || (pSKMenu == NULL))
          {
            DIALER_ERR("Null pointer", 0,0,0);
            return FALSE;
          }

          switch (pMe->m_wNextNumEditDlg)
          {
            case IDC_TEXT_NUMEDIT:
              ITEXTCTL_SetActive(pITextCtl, TRUE);
              (void) ITEXTCTL_Redraw(pITextCtl);
              (void) IMENUCTL_Redraw(pIMenu);
              (void) IMENUCTL_Redraw(pSKMenu);
              break;

            case IDC_MENU_NUMEDIT:
              IMENUCTL_SetActive(pIMenu, TRUE);
              (void) ITEXTCTL_Redraw(pITextCtl);
              (void) IMENUCTL_Redraw(pIMenu);
              (void) IMENUCTL_Redraw(pSKMenu);
              break;

            case IDC_SK_NUMEDIT:
              IMENUCTL_SetActive(pSKMenu, TRUE);
              (void) ITEXTCTL_Redraw(pITextCtl);
              (void) IMENUCTL_Redraw(pIMenu);
              (void) IMENUCTL_Redraw(pSKMenu);
              break;

            default:
              break;
          }

          // reset flag
          pMe->m_wNextNumEditDlg = 0;
        }

        return FALSE;
      }

    case EVT_KEY:
      switch (key)
      {
        case AVK_SELECT:
          {
            // want to highlight the send sk on select key press
            IDialog *pActiveDlg = CDialerApp_GetActiveDlg(pMe);
            ITextCtl *pITextCtl = NULL;;

            if (pActiveDlg == NULL)
            {
              DIALER_ERR("Null pointer", 0,0,0);
              return FALSE;
            }

            pITextCtl = (ITextCtl*)IDIALOG_GetControl(pActiveDlg,
                                                      IDC_TEXT_NUMEDIT);
            if (pITextCtl == NULL)
            {
              DIALER_ERR("Null pointer", 0,0,0);
              return FALSE;
            }

            if (ITEXTCTL_IsActive(pITextCtl) == TRUE)
            {
              IMenuCtl *pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(
                                                                     pActiveDlg, IDC_SK_NUMEDIT);

              // highlight the send sk
              IMENUCTL_SetSel(pISoftKeyMenu, IDL_SK_NUMEDIT_SEND);
              (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_NUMEDIT);
              IDIALOG_Redraw(pActiveDlg);
              return TRUE;
            }

            return FALSE;
          }

        case AVK_SEND:
          {
            IDialog *pActiveDlg = CDialerApp_GetActiveDlg(pMe);
            IMenuCtl *pIMenu = NULL;;

            if (pActiveDlg == NULL)
            {
              DIALER_ERR("Null pointer", 0,0,0);
              return FALSE;
            }

            pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg,
                                                   IDC_MENU_NUMEDIT);
            if (pIMenu == NULL)
            {
              DIALER_ERR("Null pointer", 0,0,0);
              return FALSE;
            }

            if (IMENUCTL_IsActive(pIMenu) == TRUE)
            {
              uint32 dwID;

              (void) IMENUCTL_GetItemData(pIMenu, IMENUCTL_GetSel(pIMenu), &dwID);

              if (CDialerApp_NumEditDlg_GetNumberMatch(pMe, dwID) == FALSE)
              {
                (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
                DIALER_ERR("Could not get number from contacts", 0, 0, 0);
                return FALSE;
              }

              (void) CDialerApp_OriginateVoiceCall(pMe, pMe->m_pszDialString);

            }
            else
            {
              (void) CDialerApp_NumEditDlg_Send(pMe);
            }

            return TRUE;
          }

        default:
          return FALSE;
      }
  }
  return FALSE;
}


/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_CmdHandler

DESCRIPTION: Handles NumEdit Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NumEditDlg_CmdHandler(CDialerApp *pMe,
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

#ifdef FEATURE_QVPHONE_APP
  case IDL_SK_NUMEDIT_VIDEOCALL:
    if(CDialerApp_TextCtlGetText(pMe, pMe->m_pszDialString,
      DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_NUMEDIT) == FALSE)
    {
      (void)CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
      DIALER_ERR("CDialerApp_TextCtlGetText Failed", 0,0,0);
      return FALSE;
    }
    (void) CDialerApp_StopApp(pMe);
    (void) CVideoPhone_OriginateVideoCall(pMe->a.m_pIShell, pMe->m_pszDialString);
    return TRUE;
#endif /* FEATURE_QVPHONE_APP */

#ifdef FEATURE_WMS_APP
  case IDL_SK_NUMEDIT_SENDSMS:
    {
      char *pszAddr = NULL;

      pszAddr = (char*) MALLOC(sizeof(char)*DIALERAPP_NUMBER_BUFFER_SIZE);
      if(pszAddr == NULL)
      {
        DIALER_ERR("No Memory", 0,0,0);
        return FALSE;
      }

      if(CDialerApp_TextCtlGetText(pMe, pMe->m_pszDialString,
        DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_NUMEDIT) == FALSE)
      {
        (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
        DIALER_ERR("CDialerApp_TextCtlGetText Failed", 0,0,0);
        FREE(pszAddr);
        return FALSE;
      }

      (void) WSTRTOSTR(pMe->m_pszDialString, pszAddr, DIALERAPP_NUMBER_BUFFER_SIZE);

      if(CDialerApp_CheckAEEReturnStatus(pMe, OriginateMessage(FALSE, pszAddr)) == FALSE)
      {
        FREE(pszAddr);
        return FALSE;
      }

      // stop dialer since we going to send an sms message
      (void) CDialerApp_StopApp(pMe);
      FREE(pszAddr);
      return TRUE;
    }
#endif //FEATURE_WMS_APP

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  case IDL_SK_NUMEDIT_SSWIZARD:
  {
    IDialog *pActiveDlg;
    AECHAR *pszStr = NULL;
    int nlen;

    pActiveDlg = CDialerApp_GetActiveDlg(pMe);

    if(pActiveDlg == NULL)
    {
      DIALER_ERR("Null pointer", 0,0,0);
      return FALSE;
    }

    nlen = CDialerApp_TextCtlGetTextEx(&pszStr, pActiveDlg, IDC_TEXT_NUMEDIT);

    if((nlen == 0) || (pszStr == NULL))
    {
      DIALER_ERR("CDialerApp_TextCtlGetTextEx FAILED", 0, 0, 0);
      return FALSE;
    }

    CDialerApp_SupsInitializeStartSSObject(pMe);

    // TODO
    // free string memory
    FREE(pszStr);

    return FALSE;
  }
#endif //defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

  case IDL_SK_NUMEDIT_SEND:
    (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
        (PFNNOTIFY) CDialerApp_NumEditDlg_ADNLookup, (void*) pMe);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
        (PFNNOTIFY) CDialerApp_NumEditDlg_CheckForSS, (void*) pMe);
#endif //defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    return CDialerApp_NumEditDlg_Send(pMe);

  case IDL_SK_NUMEDIT_SAVE:
    {
      IContactApp *pICONTACTAPP = NULL;

      // get number from text control
      if(CDialerApp_TextCtlGetText(pMe, pMe->m_pszDialString,
        DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_NUMEDIT) == FALSE)
      {
        (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
        DIALER_ERR("CDialerApp_TextCtlGetText Failed", 0,0,0);
        return FALSE;
      }

      if(WSTRLEN(pMe->m_pszDialString) == 0)
      {
        CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_PLEASEDIAL, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
        return FALSE;
      }

      if(CDialerApp_CheckAEEReturnStatus(pMe, ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONTACT_APP,(void **) &pICONTACTAPP)) == FALSE)
      {
        DIALER_ERR("CONTACTAPP Create Failed", 0, 0, 0);
        return FALSE;
      }
      if(pICONTACTAPP == NULL)
      {
        (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
        DIALER_ERR("Null pointer", 0, 0, 0);
        return FALSE; //error
      }

      if(CDialerApp_CheckAEEReturnStatus(pMe, IContactApp_SaveNumber(pICONTACTAPP, pMe->m_pszDialString,
        STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT, SAVE_NEW)) == FALSE)
      {
        DIALER_ERR("IContactApp_FindNumber Failed", 0, 0, 0);
        return FALSE;
      }

      (void) IContactApp_Release(pICONTACTAPP);

      return TRUE;
    }

#ifdef FEATURE_MMGSDI
  case IDL_SK_NUMEDIT_SAVESIM:
    {
      IContactApp *pICONTACTAPP = NULL;

      // get number from text control
      if(CDialerApp_TextCtlGetText(pMe, pMe->m_pszDialString,
        DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_NUMEDIT) == FALSE)
      {
        (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
        DIALER_ERR("CDialerApp_TextCtlGetText Failed", 0,0,0);
        return FALSE;
      }

      if(WSTRLEN(pMe->m_pszDialString) == 0)
      {
        CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_PLEASEDIAL, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
        return FALSE;
      }

      if(CDialerApp_CheckAEEReturnStatus(pMe, ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONTACT_APP,(void **) &pICONTACTAPP)) == FALSE)
      {
        DIALER_ERR("CONTACTAPP Create Failed", 0, 0, 0);
        return FALSE;
      }
      if(pICONTACTAPP == NULL)
      {
        (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
        DIALER_ERR("Null pointer", 0, 0, 0);
        return FALSE; //error
      }

      if(CDialerApp_CheckAEEReturnStatus(pMe, IContactApp_SaveNumber(pICONTACTAPP, pMe->m_pszDialString,
        STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT, SAVE_TO_RUIM)) == FALSE)
      {
        DIALER_ERR("IContactApp_FindNumber Failed", 0, 0, 0);
        return FALSE;
      }

      (void) IContactApp_Release(pICONTACTAPP);

      return TRUE;
    }
#endif //FEATURE_MMGSDI


  case IDL_SK_NUMEDIT_ADDTO:
    {
      IContactApp *pICONTACTAPP = NULL;

      if(IADDRBOOK_GetNumRecs(pMe->m_pIADDRBOOK) == 0)
      {
        DIALER_ERR("Addrbook is empty", 0, 0, 0);
        return FALSE;
      }

      // get number from text control
      if(CDialerApp_TextCtlGetText(pMe, pMe->m_pszDialString,
        DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_NUMEDIT) == FALSE)
      {
        (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
        DIALER_ERR("CDialerApp_TextCtlGetText Failed", 0,0,0);
        return FALSE;
      }

      if(WSTRLEN(pMe->m_pszDialString) == 0)
      {
        CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_PLEASEDIAL, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
        return FALSE;
      }

      if(CDialerApp_CheckAEEReturnStatus(pMe, ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONTACT_APP,(void **) &pICONTACTAPP)) == FALSE)
      {
        DIALER_ERR("CONTACTAPP Create Failed", 0, 0, 0);
        return FALSE;
      }
      if(pICONTACTAPP == NULL)
      {
        (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
        DIALER_ERR("Null pointer", 0, 0, 0);
        return FALSE; //error
      }

      if(CDialerApp_CheckAEEReturnStatus(pMe, IContactApp_SaveNumber(pICONTACTAPP, pMe->m_pszDialString,
        STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT, SAVE_TO_EXISTING)) == FALSE)
      {
        DIALER_ERR("IContactApp_FindNumber Failed", 0, 0, 0);
        return FALSE;
      }

      (void) IContactApp_Release(pICONTACTAPP);

      return TRUE;
    }

  case IDL_NUMEDIT_SPEEDDIAL:
  case IDL_NUMEDIT_NUM1:
  case IDL_NUMEDIT_NUM2:
  case IDL_NUMEDIT_NUM3:
  case IDL_NUMEDIT_NUM4:
  case IDL_NUMEDIT_NUM5:
  case IDL_NUMEDIT_NUM6:
  case IDL_NUMEDIT_NUM7:
  case IDL_NUMEDIT_NUM8:
  case IDL_NUMEDIT_NUM9:
  case IDL_NUMEDIT_NUM10:
  case IDL_NUMEDIT_NUM11:
  case IDL_NUMEDIT_NUM12:
  case IDL_NUMEDIT_NUM13:
  case IDL_NUMEDIT_NUM14:
  case IDL_NUMEDIT_NUM15:
    {
      IDialog *pActiveDlg = NULL;
      IMenuCtl *pIMenu = NULL;
      ITextCtl *pIText = NULL;
      uint32 dwID = 0;

      pActiveDlg = CDialerApp_GetActiveDlg(pMe);
      if(pActiveDlg == NULL)
      {
        DIALER_ERR("Null pointer", 0,0,0);
        return FALSE;
      }

      // Get menu in dialog
      pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_MENU_NUMEDIT);
      pIText = (ITextCtl*)IDIALOG_GetControl(pActiveDlg, IDC_TEXT_NUMEDIT);
      if((pIMenu == NULL) || (pIText == NULL))
      {
        DIALER_ERR("Null pointer", 0,0,0);
        return FALSE; // error
      }

      (void) IMENUCTL_GetItemData(pIMenu, ctrl_id, &dwID);

      if(CDialerApp_NumEditDlg_GetNumberMatch(pMe, dwID) == FALSE)
      {
        (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
        DIALER_ERR("Could not dial number from contacts", 0, 0, 0);
        return FALSE;
      }

      // turn off menu control
      IMENUCTL_SetActive(pIMenu, FALSE);

      // set the text control to active and refresh contents
      (void) ITEXTCTL_SetText(pIText, pMe->m_pszDialString, WSTRLEN(pMe->m_pszDialString));
      ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
      ITEXTCTL_SetActive(pIText, TRUE);
      (void) ITEXTCTL_Redraw(pIText);

      // refresh number match menu
      CDialerApp_NumEditDlg_RedrawNumberMatchMenu(pMe);

      return TRUE;
    }

  default:
    break;
  }

  return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_EndHandler

DESCRIPTION: Handles NumEdit Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NumEditDlg_EndHandler(CDialerApp *pMe)
{
  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
  {
    return FALSE;
  }

  (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
        (PFNNOTIFY) CDialerApp_NumEditDlg_ADNLookup, (void*) pMe);
  (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
        (PFNNOTIFY) CDialerApp_ClearTextCtl, (void*) pMe);
  (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
        (PFNNOTIFY) CDialerApp_NumEditDlg_RedrawNumberMatchMenu, (void*) pMe);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
        (PFNNOTIFY) CDialerApp_NumEditDlg_CheckForSS, (void*) pMe);
#endif //defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  return TRUE;

}



/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_ADNLookup

DESCRIPTION: Checks the ADN phonebook for a match to the location.
             Example 12#.

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CDialerApp_NumEditDlg_ADNLookup(CDialerApp *pMe)
{
  int nStrLen;
  char *pszNumber = NULL;
  AECHAR *pszPhoneNumber = NULL;
  IAddrRec *pIAddrRec = NULL;
  uint16 wNumber;
  AEEAddrField *AddrField = NULL;
  ITextCtl *pITextCtl = NULL;
  IDialog *pActiveDlg = NULL;

  PRINT_FUNCTION_NAME();

  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  // check to see if app is running
  if(pMe->m_bSuspended == TRUE)
  {
    return;
  }

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_NUMEDIT)
  {
    return;
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);
  if(pActiveDlg == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  if(CDialerApp_TextCtlGetText(pMe, pMe->m_pszDialString,
    DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_NUMEDIT) == FALSE)
  {
    return;
  }

  nStrLen = WSTRLEN(pMe->m_pszDialString);

  // make sure the last char is a #
  if(pMe->m_pszDialString[nStrLen-1] != (AECHAR) '#')
  {
    return;
  }

  // Get controls in dialog
  pITextCtl = (ITextCtl*)IDIALOG_GetControl(pActiveDlg,
                                            IDC_TEXT_NUMEDIT);
  if(pITextCtl == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return; // error
  }

  // allocate memory needed
  pszPhoneNumber = (AECHAR*) MALLOC ((DIALERAPP_NUMBER_BUFFER_SIZE)*sizeof(AECHAR));
  pszNumber = (char*) MALLOC ((DIALERAPP_NUMBER_BUFFER_SIZE)*sizeof(char));
  if((pszNumber == NULL) || (pszPhoneNumber == NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszNumber);
    FREEIF(pszPhoneNumber);
    return;
  }

  // get number from string and or it to the category
  (void) WSTRTOSTR(pMe->m_pszDialString, pszNumber, ((DIALERAPP_NUMBER_BUFFER_SIZE)*sizeof(char)));
  pszNumber[nStrLen-1] = 0;
  wNumber = (uint16)ATOI(pszNumber);

  // check to see if it is valid
  if(wNumber > 0)
  {
    wNumber = wNumber | 0x1000; // cheat to get ADN category

    // get the record from the addrbook
    pIAddrRec = IADDRBOOK_GetRecByID(pMe->m_pIADDRBOOK, wNumber);

    // check for a match
    if(pIAddrRec != NULL)
    {
      int i=0;
      int nNumFields  = IADDRREC_GetFieldCount(pIAddrRec);

      // go through the fields till we get a phone number
      while(i<nNumFields)
      {
        AddrField = IADDRREC_GetField(pIAddrRec, i);

        switch(AddrField->fID)
        {
        case AEE_ADDRFIELD_PHONE_WORK:
        case AEE_ADDRFIELD_PHONE_HOME:
        case AEE_ADDRFIELD_PHONE_FAX:
        case AEE_ADDRFIELD_PHONE_VOICE:
        case AEE_ADDRFIELD_PHONE_PREF:
        case AEE_ADDRFIELD_PHONE_CELL:
        case AEE_ADDRFIELD_PHONE_PAGER:
        case AEE_ADDRFIELD_PHONE_GENERIC:
        case AEE_ADDRFIELD_PHONE_OTHER:
          // copy phone number
          if(WSTRLEN((AECHAR*) AddrField->pBuffer) != 0)
          {
            i = nNumFields;
            (void) WSTRNCOPYN(pszPhoneNumber, DIALERAPP_NUMBER_BUFFER_SIZE, AddrField->pBuffer, (AddrField->wDataLen/sizeof(AECHAR)));
            // update text control
            (void) ITEXTCTL_SetText(pITextCtl, pszPhoneNumber, WSTRLEN(pszPhoneNumber));
            ITEXTCTL_SetCursorPos(pITextCtl, TC_CURSOREND);
            (void) ITEXTCTL_Redraw(pITextCtl);
            (void) ISHELL_SetTimer(pMe->a.m_pIShell, 250,
              (PFNNOTIFY) CDialerApp_NumEditDlg_RedrawNumberMatchMenu, (void*) pMe);
          }
          break;

        default:

          break;
        }

        i++;
      }
    }
  }

  // free memory
  FREEIF(pszNumber);
  FREEIF(pszPhoneNumber);
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_CheckForSS

DESCRIPTION: Checks the text control of the numedit for an SS string and preforms
             any actions needed.

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CDialerApp_NumEditDlg_CheckForSS(CDialerApp *pMe)
{
  //lint -save -esym(715, pMe)
  int nStrLen;
  int nSSRetVal;
  int nStartSSRetVal;
  int nOperation;

  AECHAR pszwErrorMsg[DIALERAPP_MAX_STR_SIZE];

  PRINT_FUNCTION_NAME();

  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  // check to see if app is running
  if(pMe->m_bSuspended == TRUE)
  {
    return;
  }

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_NUMEDIT)
  {
    return;
  }

  if(CDialerApp_TextCtlGetText(pMe, pMe->m_pszDialString,
    DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_NUMEDIT) == FALSE)
  {
    return;
  }

  nStrLen = WSTRLEN(pMe->m_pszDialString);
  if(nStrLen == 0)
  {
    return;
  }

  nSSRetVal = ISUPPSVC_StringCheck(pMe->m_pISUPPSVC,
           nStrLen,
           pMe->m_pszDialString,
           &nOperation);

  switch (nSSRetVal)
  {
   case AEESUPPSVC_NOT_SS:
     break;

  case AEESUPPSVC_IS_SS:
    if((nOperation == AEESUPPSVC_CHANGE_PIN)  ||
       (nOperation == AEESUPPSVC_UNBLOCK_PIN) ||
       (nOperation == AEESUPPSVC_DISPLAY_IMEI))
      {
  uint8 requestId;

  nStartSSRetVal = ISUPPSVC_StartSS(pMe->m_pISUPPSVC,
            pMe->m_pszDialString,
            &requestId, TRUE);

  switch (nStartSSRetVal)
  {
  case AEESUPPSVC_SS_CMD_STARTED:
    DIALER_MSG("Supplementary Service Command Successfully Started",0,0,0);
    break;

  case AEESUPPSVC_SS_FAILED_PINS_DONT_MATCH:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
              DIALERAPP_RES_FILE,
              IDS_PINS_DONT_MATCH,
              pszwErrorMsg,
              DIALERAPP_MAX_STR_SIZE);

    CDialerApp_NotifDlg_DisplayMsgEx(pMe,
             pszwErrorMsg,
             DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY,
             DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
    DIALER_MSG("PINs failed to match",0,0,0);
    break;

  default:
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
              DIALERAPP_RES_FILE,
              IDS_SS_START_FAILED,
              pszwErrorMsg,
              DIALERAPP_MAX_STR_SIZE);

    CDialerApp_NotifDlg_DisplayMsgEx(pMe,
             pszwErrorMsg,
             DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY,
             DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

    DIALER_MSG("Supplementary Service Command failed to start %d",nStartSSRetVal,0,0);
    break;
  }

  (void) CDialerApp_EndDlgEx(pMe, IDD_NUMEDIT);
      }
    break;

  case AEESUPPSVC_ILLEGAL_ENTRY:
    DIALER_MSG("Illegal entry received for SS String",0,0,0);

    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
        DIALERAPP_RES_FILE,
        IDS_INVALID_SS_FMT,
        pszwErrorMsg,
        DIALERAPP_MAX_STR_SIZE);

    CDialerApp_NotifDlg_DisplayMsgEx(pMe,
             pszwErrorMsg,
             DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY,
             DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
    (void) CDialerApp_EndDlgEx(pMe, IDD_NUMEDIT);
    break;

  default:
    DIALER_MSG("Unknown error %d for SS String",nSSRetVal,0,0);
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
        DIALERAPP_RES_FILE,
        IDS_SS_START_FAILED,
        pszwErrorMsg,
        DIALERAPP_MAX_STR_SIZE);

    CDialerApp_NotifDlg_DisplayMsgEx(pMe,
             pszwErrorMsg,
             DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY,
             DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);

    break;
  }

#if 0 // temporarily removed
  if((nStrLen != 0) && (nSSRetVal == AEESUPPSVC_IS_SS  || nSSRetVal == AEESUPPSVC_IS_PARTIAL_SS))
  {
    CDialerApp_NumEditDlg_ShowWizardSK(pMe, TRUE);
  }
  else
  {
    CDialerApp_NumEditDlg_ShowWizardSK(pMe, FALSE);
  }
#endif // 0
}
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#if 0
/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_ShowWizardSK

DESCRIPTION: Displays the ss wizard sk

PARAMETERS:
  *pMe: CDialerApp object pointer
  bDisplay: boolean to determine if wizard sk should be displayed

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_NumEditDlg_ShowWizardSK(CDialerApp *pMe, boolean bDisplay)
{
  IDialog *pActiveDlg;
  IMenuCtl* pISoftKeyMenu;

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

  if(pMe->m_bWizardSKDisplayed != bDisplay)
  {
    pMe->m_bWizardSKDisplayed = bDisplay;

    // Get controls in dialog
    pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg,
                                                  IDC_SK_NUMEDIT);
    if(pISoftKeyMenu == NULL)
    {
      DIALER_ERR("Null pointer", 0,0,0);
      return FALSE; // error
    }

    // Update the softkey menu
    (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SEND,
                                    IDL_SK_NUMEDIT_SEND, NULL, 0);
    if(bDisplay == TRUE)
    {
      (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SSWIZARD,
                                    IDL_SK_NUMEDIT_SSWIZARD, NULL, 0);
    }


    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SAVE,
                                    IDL_SK_NUMEDIT_SAVE, NULL, 0);
#ifdef FEATURE_MMGSDI
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SAVESIM,
                                   IDL_SK_NUMEDIT_SAVESIM, NULL, 0);
#endif //FEATURE_MMGSDI
    if(IADDRBOOK_GetNumRecs(pMe->m_pIADDRBOOK) > 0)
    {
      (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_ADDTO,
                                    IDL_SK_NUMEDIT_ADDTO, NULL, 0);

      (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_FIND,
                                    IDL_SK_NUMEDIT_FIND, NULL, 0);
    }

    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_VIDEOCALL,
                                    IDL_SK_NUMEDIT_VIDEOCALL, NULL, 0);
#ifdef FEATURE_WMS_APP
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SENDSMS,
                                    IDL_SK_NUMEDIT_SENDSMS, NULL, 0);
#endif
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_BLOCKID,
                                    IDL_SK_NUMEDIT_BLOCKID, NULL, 0);

    if(bDisplay == TRUE)
    {
      IMENUCTL_SetSel(pISoftKeyMenu, IDL_SK_NUMEDIT_SSWIZARD);
    }
    else
    {
      IMENUCTL_SetSel(pISoftKeyMenu, IDL_SK_NUMEDIT_SEND);
    }

    (void) IMENUCTL_Redraw(pISoftKeyMenu);
  }
  return TRUE;
}
#endif // 0
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_Send

DESCRIPTION: Handles the AVK_SEND or IDL_SK_NUMEDIT_SEND events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_NumEditDlg_Send(CDialerApp *pMe)
{
  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  if(CDialerApp_TextCtlGetText(pMe, pMe->m_pszDialString,
    DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_NUMEDIT) == FALSE)
  {
    (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
    DIALER_ERR("CDialerApp_TextCtlGetText Failed", 0,0,0);
    return FALSE;
  }

  if(WSTRLEN(pMe->m_pszDialString) == 0)
  {
    CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_PLEASEDIAL, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
    return FALSE;
  }

  return (CDialerApp_OriginateVoiceCall(pMe, pMe->m_pszDialString));
}

/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_Init

DESCRIPTION: Initializes the controls for NumEdit dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NumEditDlg_Init(CDialerApp *pMe)
{
  IDialog *pActiveDlg;
  ITextCtl* pITextCtl;
  IMenuCtl* pISoftKeyMenu;
  IMenuCtl* pIMenu;
  AEERect MenuRect;
  AEERect SKRect;
  uint32 dwTextProps;

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
  pITextCtl = (ITextCtl*)IDIALOG_GetControl(pActiveDlg,
                                            IDC_TEXT_NUMEDIT);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg,
                                                IDC_SK_NUMEDIT);
  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg,
                                                IDC_MENU_NUMEDIT);

  if((pITextCtl == NULL) || (pISoftKeyMenu == NULL) || (pIMenu == NULL))
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error
  }

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SEND,
                                   IDL_SK_NUMEDIT_SEND, NULL, 0);

  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SAVE,
                                   IDL_SK_NUMEDIT_SAVE, NULL, 0);
#ifdef FEATURE_MMGSDI
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SAVESIM,
                                   IDL_SK_NUMEDIT_SAVESIM, NULL, 0);
#endif //FEATURE_MMGSDI

  if(IADDRBOOK_GetNumRecs(pMe->m_pIADDRBOOK) > 0)
  {
    (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_ADDTO,
                                    IDL_SK_NUMEDIT_ADDTO, NULL, 0);
  }

#ifdef FEATURE_QVPHONE_APP
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_VIDEOCALL,
    IDL_SK_NUMEDIT_VIDEOCALL, NULL, 0);
#endif /* FEATURE_QVPHONE_APP */

#ifdef FEATURE_WMS_APP
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SENDSMS,
    IDL_SK_NUMEDIT_SENDSMS, NULL, 0);
#endif //FEATURE_WMS_APP

  //ITEXTCTL_SetSoftKeyMenu(pITextCtl, NULL);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
  IMENUCTL_SetSel(pISoftKeyMenu, IDL_SK_NUMEDIT_SEND);


  // Set MENU control size
  IMENUCTL_GetRect(pISoftKeyMenu, &SKRect);
  (void) IMENUCTL_DeleteAll(pIMenu);
  SETAEERECT(&MenuRect, 10, (pMe->m_DialedDigitsRect.y+pMe->m_DialedDigitsRect.dy), pMe->m_rc.dx-20, pMe->m_rc.dy-(pMe->m_DialedDigitsRect.y+pMe->m_DialedDigitsRect.dy)-SKRect.dy-20);
  IMENUCTL_SetRect(pIMenu, &MenuRect);
  SetDefaultMenuLook(pMe->a.m_pIShell, pIMenu);
  (void) IMENUCTL_Redraw(pIMenu);

  /* Set textctl properties
   * In BREW 3.1.4 and later, if we use the "new" text control,
   * we need to set the TP_FOCUS_NOSEL so that all of the text is not
   * selected when the text control takes focus.  AEECLSID_TEXTCTL is
   * the "new" text control for BREW 3.1.4 and later revisions.
   */
  ITEXTCTL_SetMaxSize(pITextCtl, DIALERAPP_NUMBER_BUFFER_SIZE-1);
#if MIN_BREW_VERSION(3,1)
  dwTextProps = TP_MULTILINE | TP_FRAME | TP_FONTMORPH;
#ifdef TP_VALIDATE_INPUT
  dwTextProps |= TP_VALIDATE_INPUT;
#endif
#if MIN_BREW_VERSIONEx(3,1,3)
  dwTextProps |= TP_FOCUS_NOSEL;
#endif
#else
  dwTextProps = TP_MULTILINE | TP_FRAME | OEM_TP_FONTMORPH;
#endif

  pMe->m_bNVAutoHyphen = CDialerApp_NVCheck_IsAutoHyphen();

  if(pMe->m_bNVAutoHyphen == TRUE)
  {
#if MIN_BREW_VERSION(3,1)
    dwTextProps |= TP_AUTOHYPHEN;
#else
    dwTextProps |= OEM_TP_AUTOHYPHEN;
#endif
  }
  ITEXTCTL_SetProperties(pITextCtl, dwTextProps);

  ITEXTCTL_SetRect(pITextCtl, &pMe->m_DialedDigitsRect);
  (void) ITEXTCTL_SetText(pITextCtl, pMe->m_pszDialString,
                   WSTRLEN(pMe->m_pszDialString));
  ITEXTCTL_SetCursorPos(pITextCtl, TC_CURSOREND);
#if MIN_BREW_VERSION(3,1)
  (void) ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_DIALED_DIGITS);
#else
  (void) ITEXTCTL_SetInputMode(pITextCtl, OEM_TM_DIALED_DIGITS);
#endif

  (void) IDIALOG_SetFocus(pActiveDlg, IDC_TEXT_NUMEDIT);
  IDIALOG_Redraw(pActiveDlg);

  CDialerApp_NumEditDlg_RedrawNumberMatchMenu(pMe);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  // ss wizard check
  //CDialerApp_NumEditDlg_CheckForSS(pMe);
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_DisplayRefresh

DESCRIPTION: Refreshes the NumEdit dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NumEditDlg_DisplayRefresh(void *pUser)
{
  CDialerApp* pMe = (CDialerApp*) pUser;
  IDialog *pActiveDlg;

  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
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

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_NUMEDIT)
  {
    DIALER_ERR("NumEdit Dialog is not active", 0,0,0);
    return FALSE;
  }

  IDIALOG_Redraw(pActiveDlg);

  CDialerApp_NumEditDlg_RedrawNumberMatchMenu(pMe);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  // ss wizard check
  //CDialerApp_NumEditDlg_CheckForSS(pMe);
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_GetNumberMatchMenu

DESCRIPTION: Get the phone number associated with the menu item

PARAMETERS:
  *pMe: CDialerApp object pointer
  dwData: record id and field of entry to dial

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:
  bits 16-31 of dwData = record id to retrieve
  bits  0-15 of dwData = field index of record for number to dial

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_NumEditDlg_GetNumberMatch(CDialerApp *pMe, uint32 dwData)
{
  IAddrRec *pIAddrRec = NULL;
  AEEAddrField *AddrField = NULL;
  uint16 wRecID;
  uint16 wFieldIndex;

  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  // check param
  if(dwData == 0)
  {
    DIALER_ERR("Bad RecID Param", 0,0,0);
    return FALSE;
  }

  // bits 16-31 of dwData = record id to retrieve
  // bits  0-15 of dwData = field index of record for number to dial
  wRecID = (uint16) (dwData >> 16);
  wFieldIndex = (uint16) (dwData & 0x0000FFFF);

  // get the record from the addrbook
  pIAddrRec = IADDRBOOK_GetRecByID(pMe->m_pIADDRBOOK, wRecID);

  // check for a match
  if(pIAddrRec != NULL)
  {
    AddrField = IADDRREC_GetField(pIAddrRec, wFieldIndex);
    if(AddrField != NULL)
    {
      if(AddrField->fType == AEEDB_FT_PHONE)
      {
        int field_len = 0;
        // call phone number
        if((field_len = WSTRLEN((AECHAR*) AddrField->pBuffer)) != 0)
        {
          int i;
          int dial_string_length = WSTRLEN(pMe->m_pszDialString);
          for (i = 0; (i < dial_string_length) && (i < field_len); i++)
             if (((AECHAR *) (AddrField->pBuffer))[i] == (AECHAR) '?')
               ((AECHAR *) (AddrField->pBuffer))[i] = pMe->m_pszDialString[i];

          (void) WSTRNCOPYN(pMe->m_pszDialString, DIALERAPP_NUMBER_BUFFER_SIZE, AddrField->pBuffer, (AddrField->wDataLen/sizeof(AECHAR)));
          return TRUE;
        }
      }
    }
  }

  // did not originate call
  DIALER_ERR("Could not get number for Rec:%d Field:%d", wRecID, wFieldIndex, 0);
  return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_RefreshNumberMatchMenu

DESCRIPTION: Redraw's the number matching menu

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CDialerApp_NumEditDlg_RedrawNumberMatchMenu(CDialerApp *pMe)
{
  IDialog *pActiveDlg;
  IMenuCtl* pIMenu;
  IAddrRec *addrRec;
  int wTotalFields;
  int i, j;
  uint16 wIndex;
  boolean bDone;
  AECHAR* pszName = NULL;
  AECHAR* pszNumBuf = NULL;
  CtlAddItem menuItem;
  AEEAddrCat category;
  uint16 wRecID = 0;
  int actualValue,lengthStr;
  int char_index;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_NUMEDIT)
  {
    return;
  }

  // check to see if app is running
  if(pMe->m_bSuspended == TRUE)
  {
    return;
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);
  if (pActiveDlg == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  // Get menu in dialog
  pIMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, IDC_MENU_NUMEDIT);
  if(pIMenu == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return; // error
  }

  // delete all items
  (void) IMENUCTL_DeleteAll(pIMenu);

  // get contents of text control
  if(CDialerApp_TextCtlGetText(pMe, pMe->m_pszDialString,
    DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_NUMEDIT) == FALSE)
  {
    (void) IMENUCTL_Redraw(pIMenu);
    return;
  }

  if(WSTRLEN(pMe->m_pszDialString) == 0)
  {
    (void) IMENUCTL_Redraw(pIMenu);
    return;
  }

  // allocate memory for strings
  pszNumBuf = (AECHAR *) MALLOC((DIALERAPP_NUMBER_BUFFER_SIZE)* sizeof(AECHAR));
  pszName = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE)* sizeof(AECHAR));
  if((pszName == NULL) || (pszNumBuf == NULL))
  {
    DIALER_ERR("No Memory", 0,0,0);
    FREEIF(pszName);
    FREEIF(pszNumBuf);
    (void) IMENUCTL_Redraw(pIMenu);
    return;
  }

  // look for the number in speed dial first
  actualValue=0; lengthStr=0;
  lengthStr = WSTRLEN(pMe->m_pszDialString);
  for(j=0;j<lengthStr;j++)
  {
    actualValue = (actualValue * 10) +
                  (pMe->m_pszDialString[j] - (AECHAR) '0') ;
  }
  wIndex = (uint16)actualValue;

  //Start searching the address book with the search string
  if (SUCCESS == IADDRBOOK_EnumRecInit(pMe->m_pIADDRBOOK, AEE_ADDR_CAT_SPEEDDIAL, AEE_ADDRFIELD_NONE, NULL, 0))
  {
    boolean bFound = FALSE;

    // reset the name
    (void) MEMSET(pszName, 0, (sizeof(AECHAR)*DIALERAPP_MAX_STR_SIZE));

    menuItem.pImage = NULL;
    menuItem.wText = 0;
    menuItem.pszResText = NULL;
    menuItem.pszResImage = DIALERAPP_RES_FILE;
    menuItem.wFont = AEE_FONT_NORMAL;
    menuItem.wImage = IDB_SPEEDDIAL;
    menuItem.wItemID = IDL_NUMEDIT_SPEEDDIAL;

    while(bFound == FALSE)
    {
      addrRec = IADDRBOOK_EnumNextRec(pMe->m_pIADDRBOOK);

      if (NULL == addrRec)
      {
        // No more matching records found, exit loop
        bFound = TRUE;
        continue;
      }

      //Get the name of the Contact from the record
      wTotalFields = IADDRREC_GetFieldCount(addrRec);

      for (j = 0; j < wTotalFields; j++)
      {
        AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);

        //lint -esym(40, AEE_ADDRFIELD_INDEX)
        if(addrField && (addrField->fID == AEE_ADDRFIELD_INDEX))
        {
          if(wIndex == *((uint16 *)addrField->pBuffer))
          {
            bFound = TRUE;
          }
        }
      }

      // index was matched so add item to menu
      if(bFound==TRUE)
      {
        uint16 wField = 65535;

        for (j = 0; j < wTotalFields; j++)
        {
          AEEAddrField *addrField = IADDRREC_GetField(addrRec, j);

          if((addrField != NULL) &&
            (addrField->pBuffer != NULL) &&
            (addrField->wDataLen > 0))
          {

            if(addrField->fID == AEE_ADDRFIELD_NAME)
            {
              wRecID = IADDRREC_GetRecID(addrRec);

              (void) WSTRNCOPY(pszName, DIALERAPP_MAX_STR_SIZE , addrField->pBuffer);
              // set the name
              menuItem.pText = pszName;
            }

            if(addrField->fType == AEEDB_FT_PHONE)
            {
              // get the field index for the number
              wField = (uint16)j;
            }
          }
        }

        // check to see if we found a good match
        if((wField != 65535) && (WSTRLEN(menuItem.pText) != 0))
        {
          // bits 16-31 of dwData = record id to retrieve
          // bits  0-15 of dwData = field index of record for number to dial
          menuItem.dwData = wRecID;
          menuItem.dwData = menuItem.dwData  << 16;
          menuItem.dwData = menuItem.dwData | wField;

          // add the item to the menu
          (void) IMENUCTL_AddItemEx(pIMenu, &menuItem);
        }
      }

      // release record
      (void) IADDRREC_Release(addrRec);
    }
  }

  //lint -save -esym(40, ADDRREC_SEARCHTYPE_STARTSWITH)
  //lint -save -esym(40, ADDRREC_FIELDID_PHONE)
  //lint -save -esym(718, IADDRBOOK_EnumRecInitEx)
  //lint -save -esym(746, IADDRBOOK_EnumRecInitEx)
  // look for number matches in phonebook
  if (SUCCESS == IADDRBOOKEXT_EnumRecInitEx(pMe->m_pIADDRBOOK, AEE_ADDR_CAT_NONE,
                                            AEE_ADDRFIELD_NONE, pMe->m_pszDialString,
                                            (uint16)WSTRSIZE(pMe->m_pszDialString),
                                            ADDRREC_ENUMERATE_AVAILABLE |
                                            ADDRREC_SEARCHTYPE_STARTSWITH | ADDRREC_FIELDID_PHONE))
  {
    i = 0;
    bDone = FALSE;

    while(bDone != TRUE)
    {
      if(i > (IDL_NUMEDIT_NUMMAX - IDL_NUMEDIT_NUM1))
      {
        int k;
        // there is too many entries so delete them all and exit
        for(k = 0; k <=(IDL_NUMEDIT_NUMMAX - IDL_NUMEDIT_NUM1); k++)
        {
          (void) IMENUCTL_DeleteItem(pIMenu, (uint16)(IDL_NUMEDIT_NUM1+k));
        }
        FREEIF(pszName);
        FREEIF(pszNumBuf);
        return;
      }

      addrRec = IADDRBOOK_EnumNextRec(pMe->m_pIADDRBOOK);

      if(NULL == addrRec)
      {
        // No more matching records found, exit loop
        bDone = TRUE;
        continue;
      }

      category = IADDRREC_GetCategory(addrRec);

      if(category != AEE_ADDR_CAT_SPEEDDIAL)
      {
        // reset the name
        (void) MEMSET(pszName, 0, (sizeof(AECHAR)*DIALERAPP_MAX_STR_SIZE));

        // get the record ID
        wRecID = IADDRREC_GetRecID(addrRec);

        menuItem.pImage = NULL;
        menuItem.wText = 0;
        menuItem.pszResText = NULL;
        menuItem.pszResImage = DIALERAPP_RES_FILE;
        menuItem.wFont = AEE_FONT_NORMAL;
        menuItem.wImage = IDB_CONTACT;

        //Get the name of the Contact from the record
        wTotalFields = IADDRREC_GetFieldCount(addrRec);

        // first look for a name in the record id
        for (j = 0; j < wTotalFields; j++)
        {
          AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);

          if((addrField != NULL) &&
            (addrField->pBuffer != NULL) &&
            (addrField->wDataLen > 0))
          {
            if(addrField->fID == AEE_ADDRFIELD_NAME)
            {
              // add this item to the menu
              (void) WSTRNCOPY(pszName, DIALERAPP_MAX_STR_SIZE ,addrField->pBuffer);
              j = wTotalFields;
            }
          }
        }

        if(WSTRLEN(pszName) == 0)
        {
          // no name for this record .. go to next
          continue;
        }

        // set the name for menu item
        menuItem.pText = pszName;

        // now look for phone numbers - there can be multiple numbers per name
        for (j = 0; j < wTotalFields; j++)
        {
          AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);

          if((addrField != NULL) &&
            (addrField->pBuffer != NULL) &&
            (addrField->wDataLen > 0))
          {
            if(addrField->fType == AEEDB_FT_PHONE)
            {
              int nLen = WSTRLEN(pMe->m_pszDialString);

              // if dial string is larger than phone number, it is not a match so do not add
              if(nLen <= (int)(addrField->wDataLen/sizeof(AECHAR)))
              {
                (void) WSTRNCOPYN(pszNumBuf, DIALERAPP_NUMBER_BUFFER_SIZE, (AECHAR*)addrField->pBuffer, nLen);
                for (char_index = 0; char_index < nLen; char_index++)
                {
                  if (pszNumBuf[char_index] == (AECHAR) '?')
                    pszNumBuf[char_index] = pMe->m_pszDialString[char_index];
                }

                // make sure we have a match in numbers since the fields are not narrowed down
                if(WSTRCMP(pMe->m_pszDialString, pszNumBuf) == 0)
                {
                  // pick the icon
                  switch (addrField->fID)
                  {
                  case AEE_ADDRFIELD_PHONE_WORK:
                    menuItem.wImage = IDB_WORKNUMBER;
                    break;

                  case AEE_ADDRFIELD_PHONE_CELL:
                    menuItem.wImage = IDB_MOBILENUMBER;
                    break;

                  case AEE_ADDRFIELD_PHONE_PAGER:
                    menuItem.wImage = IDB_PAGERNUMBER;
                    break;

                    //If it is on RUIM
                  case AEE_ADDRFIELD_PHONE_GENERIC:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    switch(category)
                    {
                    case AEE_ADDR_CAT_SIM_ADN:
                    case AEE_ADDR_CAT_SIM_FDN:
                      menuItem.wImage = IDB_RUIM;
                      break;

                    case AEE_ADDR_CAT_SIM_ADN2:
                    case AEE_ADDR_CAT_SIM_FDN2:
                      menuItem.wImage = IDB_RUIM2;
                      break;

                    default:
                      break;

                    }
                    break;
#else
                    menuItem.wImage = IDB_RUIM;
                    break;
#endif

                  case AEE_ADDRFIELD_PHONE_HOME:
                    menuItem.wImage = IDB_HOMENUMBER;
                    break;

                  case AEE_ADDRFIELD_PHONE_OTHER:
                  default:
                    menuItem.wImage = IDB_CONTACT;
                    break;
                  }

                  // set the id of the menu item
                  menuItem.wItemID = (uint16)(IDL_NUMEDIT_NUM1 + i);

                  // bits 16-31 of dwData = record id to retrieve
                  // bits  0-15 of dwData = field index of record for number to dial
                  menuItem.dwData = wRecID;
                  menuItem.dwData = menuItem.dwData  << 16;
                  menuItem.dwData = menuItem.dwData | (uint32)j;

                  // add the item to the menu
                  (void) IMENUCTL_AddItemEx(pIMenu, &menuItem);
                  i++;
                }
              }
            }
          }
        }
      }
      // release record
      (void) IADDRREC_Release(addrRec);
    }
  }

  // redraw menu control
  (void) IMENUCTL_Redraw(pIMenu);

  // free memory
  FREEIF(pszName);
  FREEIF(pszNumBuf);
}

/*=============================================================================
FUNCTION: CDialerApp_WildCardDlg_Init

DESCRIPTION: Initializes the controls for WildCard dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_WildCardDlg_Init(CDialerApp *pMe)
{
  IDialog *pActiveDlg;
  ITextCtl *pITextCtl;
  IMenuCtl *pISoftKeyMenu;
  AECHAR *pszBuf = NULL;
  AECHAR *pszCenterTitle = NULL;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL)
  {
    return FALSE;
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);

  // Error checking
  if (pActiveDlg == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  // Get controls in the wildcard dialog
  pITextCtl = (ITextCtl*)IDIALOG_GetControl(pActiveDlg,
                                            IDC_TEXT_WILDCARD);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg,
                                                IDC_SK_WILDCARD);
  if((pITextCtl == NULL) || (pISoftKeyMenu == NULL))
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;  //error checking
  }

  pszBuf = (AECHAR *) MALLOC((DIALERAPP_MAX_STR_SIZE) * sizeof(AECHAR));
  if(pszBuf == NULL)
  {
    DIALER_ERR("No Memory", 0, 0, 0);
    return FALSE; //error
  }

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SEND,
                                   IDL_SK_WILDCARD_SEND, NULL, 0);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_CANCEL,
                                   IDL_SK_WILDCARD_CANCEL, NULL, 0);
  ITEXTCTL_SetSoftKeyMenu(pITextCtl, NULL);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);


  // Set textctl properties
  ITEXTCTL_SetMaxSize(pITextCtl, DIALERAPP_NUMBER_BUFFER_SIZE-1);
#if MIN_BREW_VERSION(3,1)
  ITEXTCTL_SetProperties(pITextCtl, TP_OVERWRITE | TP_MULTILINE |
                                    TP_FRAME | TP_FONTMORPH);
#else
  ITEXTCTL_SetProperties(pITextCtl, OEM_TP_OVERWRITE | TP_MULTILINE |
                                    TP_FRAME | OEM_TP_FONTMORPH);
#endif
  ITEXTCTL_SetRect(pITextCtl, &pMe->m_DialedDigitsRect);
  (void) ITEXTCTL_SetText(pITextCtl, pMe->m_pszDialString, WSTRLEN(pMe->m_pszDialString));
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_WILDCARD_TITLE, pszBuf, DIALERAPP_MAX_STR_SIZE);
  (void) CDialerApp_CenterTitle(pMe, pszBuf, &pszCenterTitle, (unsigned)(int)pMe->m_DialedDigitsRect.dx, AEE_FONT_BOLD);
  (void) ITEXTCTL_SetTitle (pITextCtl, DIALERAPP_RES_FILE, 0, pszCenterTitle);
  ITEXTCTL_SetCursorPos (pITextCtl, TC_CURSOREND);
#if MIN_BREW_VERSION(3,1)
  (void) ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_DIALED_DIGITS);
#else
  (void) ITEXTCTL_SetInputMode(pITextCtl, OEM_TM_DIALED_DIGITS);
#endif

  (void) IDIALOG_SetFocus(pActiveDlg, IDC_TEXT_WILDCARD);
  IDIALOG_Redraw(pActiveDlg);

  FREE(pszBuf);
  FREEIF(pszCenterTitle);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_WildCardDlg_KeyHandler

DESCRIPTION: Handles WildCard Dialog's key events

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
boolean CDialerApp_WildCardDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
   PRINT_FUNCTION_NAME();
   // error checking
   if (pMe == NULL)
   {
     return FALSE;
   }

   switch(evt)
   {
   case EVT_KEY_PRESS:
     if(key == AVK_CLR)
     {
       pMe->m_wTextCtlIDToClear = IDC_TEXT_WILDCARD;
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
         // highlight send sk on select key press
         IDialog *pActiveDlg = CDialerApp_GetActiveDlg(pMe);
         ITextCtl *pITextCtl;

         // error checking
         if (pActiveDlg == NULL)
         {
           DIALER_ERR("Null pointer", 0,0,0);
           return FALSE;
         }

         pITextCtl = (ITextCtl*)IDIALOG_GetControl(pActiveDlg,
           IDC_TEXT_WILDCARD);

         if(ITEXTCTL_IsActive(pITextCtl))
         {
           IMenuCtl *pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(
             pActiveDlg, IDC_SK_WILDCARD);

           IMENUCTL_SetSel(pISoftKeyMenu, IDL_SK_WILDCARD_SEND);
           (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_WILDCARD);
           IDIALOG_Redraw(pActiveDlg);
           return TRUE;
         }
         return FALSE;
       }

     case AVK_SEND:
       (void) CDialerApp_WildCardDlg_Send(pMe);
       return TRUE;

     default:
       return FALSE;
     }
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_WildCardDlg_CmdHandler

DESCRIPTION: Handles WildCard Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_WildCardDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
   PRINT_FUNCTION_NAME();
   // error checking
   if (pMe == NULL)
   {
     return FALSE;
   }

   switch(ctrl_id)
   {
   case IDL_SK_WILDCARD_CANCEL:
     // go back to numedit dialog
     return CDialerApp_EndDlg(pMe);

   case IDL_SK_WILDCARD_SEND:
     return CDialerApp_WildCardDlg_Send(pMe);

   default:
     return FALSE;
   }
}

/*=============================================================================
FUNCTION: CDialerApp_WildCardDlg_EndHandler

DESCRIPTION: Handles WildCard Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_WildCardDlg_EndHandler(CDialerApp *pMe)
{
   PRINT_FUNCTION_NAME();
   // error checking
   if (pMe == NULL)
   {
     return FALSE;
   }

   (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_ClearTextCtl, (void*) pMe);
   return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_WildCardDlg_Send

DESCRIPTION: Handles the AVK_SEND or IDL_SK_WILCARD_SEND events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_WildCardDlg_Send(CDialerApp *pMe)
{
  AECHAR *pszBuf;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL)
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  pszBuf = (AECHAR*) MALLOC(sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE);
  if(pszBuf == NULL)
  {
    DIALER_ERR("No Memory", 0,0,0);
    (void) CDialerApp_CheckAEEReturnStatus(pMe, ENOMEMORY);
    return FALSE;
  }

  if(CDialerApp_TextCtlGetText(pMe, pszBuf,
    DIALERAPP_NUMBER_BUFFER_SIZE, IDC_TEXT_WILDCARD) == FALSE)
  {
    (void) CDialerApp_CheckAEEReturnStatus(pMe, EFAILED);
    DIALER_ERR("CDialerApp_TextCtlGetText Failed", 0,0,0);
    return FALSE;
  }

  if(WSTRLEN(pszBuf) == 0)
  {
    CDialerApp_NotifDlg_DisplayMsg(pMe, IDS_PLEASEDIAL, DIALERAPP_MIN_DEFAULT_NOTIF_DISPLAY, DIALERAPP_MAX_DEFAULT_NOTIF_DISPLAY);
    return FALSE;
  }

  if(CDialerApp_OriginateVoiceCall(pMe, pszBuf) == FALSE)
  {
    return FALSE;
  }

  FREE(pszBuf);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_KeyHandler

DESCRIPTION: Handles TimePauseDlg Dialog's key events

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
boolean CDialerApp_TimePauseDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
{
   PRINT_FUNCTION_NAME();

   if(pMe == NULL) // error checking
   {
     return FALSE;
   }

   if (evt == EVT_KEY && key == AVK_SEND)
   {
      (void) CDialerApp_TimePauseDlg_Send(pMe);
      return TRUE;
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_CmdHandler

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
boolean CDialerApp_TimePauseDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
   PRINT_FUNCTION_NAME();

   if(pMe == NULL) // error checking
   {
     return FALSE;
   }

   switch (ctrl_id)
   {
   case IDL_SK_TIMEPAUSE_CANCEL:
     // go back to status dialog
     (void) CDialerApp_EndDlg(pMe);
     return TRUE;

   case IDL_SK_TIMEPAUSE_SEND:
     return CDialerApp_TimePauseDlg_Send(pMe);

   default:
     return FALSE;
   }
}

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_EndHandler

DESCRIPTION: Handles TimePauseDlg Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_TimePauseDlg_EndHandler(CDialerApp *pMe)
{
   PRINT_FUNCTION_NAME();

   if(pMe == NULL) // error checking
   {
     return FALSE;
   }

   (void) MEMSET(pMe->m_pszTimePauseSent, 0, (sizeof(AECHAR)*(DIALERAPP_NUMBER_BUFFER_SIZE*2)));
   (void) MEMSET(pMe->m_pszTimePauseRemaining, 0, (sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE));
   (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
         (PFNNOTIFY) CDialerApp_TimePauseDlg_Send, (void*) pMe);
   return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_Init

DESCRIPTION: Initializes the controls for TimePause dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_TimePauseDlg_Init(CDialerApp *pMe)
{
  IDialog *pActiveDlg;
  IStatic *pIStaticSent;
  IStatic *pIStaticRemaining;
  IMenuCtl* pISoftKeyMenu;
  AEERect StaticRectSent;
  AEERect StaticRectRemaining;
  AEERect MenuRect;
  AECHAR *pszSentTitle = NULL;
  AECHAR *pszRemainingTitle = NULL;

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
  pIStaticSent = (IStatic*) IDIALOG_GetControl(pActiveDlg,
    IDC_STATIC_TIMEPAUSE_SENT);
  pIStaticRemaining = (IStatic*) IDIALOG_GetControl(pActiveDlg,
    IDC_STATIC_TIMEPAUSE_REMAINING);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg,
    IDC_SK_TIMEPAUSE);
  if((pIStaticSent == NULL) || (pIStaticRemaining == NULL) || (pISoftKeyMenu == NULL))
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error
  }

  // init buffer
  pszSentTitle = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  pszRemainingTitle = (AECHAR*) MALLOC ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR));
  if((pszRemainingTitle == NULL) || (pszSentTitle== NULL))
  {
    DIALER_ERR("No memory", 0,0,0);
    FREEIF(pszSentTitle);
    FREEIF(pszRemainingTitle);
    return FALSE;
  }
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_DTMFSENT, pszSentTitle,
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));
  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, IDS_DTMFREMAINING, pszRemainingTitle,
                      ((DIALERAPP_MAX_STR_SIZE)*sizeof(AECHAR)));


  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_SEND,
    IDL_SK_TIMEPAUSE_SEND, NULL, 0);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_CANCEL,
    IDL_SK_TIMEPAUSE_CANCEL, NULL, 0);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
  IMENUCTL_SetActive(pISoftKeyMenu, TRUE);

  // Set Static control size
  ISTATIC_GetRect(pIStaticSent, &StaticRectSent);
  ISTATIC_GetRect(pIStaticRemaining, &StaticRectRemaining);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);
  SETAEERECT(&StaticRectSent, 10, 10, pMe->m_rc.dx-20, (pMe->m_rc.dy-MenuRect.dy-30)/2);
  SETAEERECT(&StaticRectRemaining, 10, ((pMe->m_rc.dy-MenuRect.dy-30)/2)+20, pMe->m_rc.dx-20, (pMe->m_rc.dy-MenuRect.dy-30)/2);


  // Static Sent Properties
  ISTATIC_SetRect(pIStaticSent, &StaticRectSent);
  ISTATIC_SetActive(pIStaticSent, FALSE);
  ISTATIC_SetProperties(pIStaticSent, ST_MIDDLETEXT | ST_UNDERLINE | ST_CENTERTITLE | ST_CENTERTEXT  | ST_NOSCROLL);
  (void) ISTATIC_SetText(pIStaticSent, pszSentTitle, NULL, AEE_FONT_BOLD,
                  AEE_FONT_NORMAL);

  // Static Remaining Properties
  ISTATIC_SetRect(pIStaticRemaining, &StaticRectRemaining);
  ISTATIC_SetActive(pIStaticRemaining, FALSE);
  ISTATIC_SetProperties(pIStaticRemaining, ST_MIDDLETEXT | ST_UNDERLINE | ST_CENTERTITLE | ST_CENTERTEXT | ST_NOSCROLL);
  (void) ISTATIC_SetText(pIStaticRemaining, pszRemainingTitle, NULL, AEE_FONT_BOLD,
                  AEE_FONT_NORMAL);

  FREE(pszSentTitle);
  FREE(pszRemainingTitle);

  if(CDialerApp_TimePauseDlg_DisplayRefresh(pMe) == FALSE)
  {
    DIALER_ERR("CDialerApp_TimePauseDlg_Refresh FAILED", 0,0,0);
    return FALSE;
  }

  if(pMe->m_pszTimePauseRemaining[0] == (AECHAR) 'T')
  {
    (void) ISHELL_SetTimer(pMe->a.m_pIShell, 3000,
      (PFNNOTIFY) CDialerApp_TimePauseDlg_Send, (void*) pMe);
  }

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_Refresh

DESCRIPTION: Refreshes the controls for TimePause dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_TimePauseDlg_DisplayRefresh(void *pUser)
{
  CDialerApp * pMe = (CDialerApp*) pUser;
  IDialog *pActiveDlg;
  IStatic *pIStaticSent;
  IStatic *pIStaticRemaining;

  PRINT_FUNCTION_NAME();
  // error checking
  if(pMe == NULL)
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

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_TIMEPAUSE)
  {
    DIALER_ERR("Cannot refresh not TimePause Dlg", 0,0,0);
    return FALSE;
  }

  // Get controls in dialog
  pIStaticSent = (IStatic*) IDIALOG_GetControl(pActiveDlg,
    IDC_STATIC_TIMEPAUSE_SENT);
  pIStaticRemaining = (IStatic*) IDIALOG_GetControl(pActiveDlg,
    IDC_STATIC_TIMEPAUSE_REMAINING);
  if((pIStaticSent == NULL) || (pIStaticRemaining == NULL))
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error
  }

  (void) ISTATIC_SetTextEx(pIStaticSent, (byte*)pMe->m_pszTimePauseSent, NULL, FALSE);
  (void) ISTATIC_Redraw(pIStaticSent);
  (void) ISTATIC_SetTextEx(pIStaticRemaining, (byte*)pMe->m_pszTimePauseRemaining, NULL, FALSE);
  (void) ISTATIC_Redraw(pIStaticRemaining);

  return TRUE;
}

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_Send

DESCRIPTION: Sends the next batch of DTMFs

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_TimePauseDlg_Send(CDialerApp *pMe)
{
  AECHAR *pszDTMF = NULL;
  int i;
  int j = 0;
  uint16 wDTMFLen;
  snd_compact_tone_type *pSoundArray;

  PRINT_FUNCTION_NAME();

  // error checking
  if(pMe == NULL)
  {
    DIALER_ERR("Null Pointer", 0, 0, 0);
    return FALSE;
  }

  // cancel any timers
  (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
          (PFNNOTIFY) CDialerApp_TimePauseDlg_Send, (void*) pMe);

  // init buffer
  pszDTMF = (AECHAR*) MALLOC(sizeof(AECHAR) * DIALERAPP_NUMBER_BUFFER_SIZE);
  if (pszDTMF == NULL)
  {
    DIALER_ERR("No memory", 0,0,0);
    (void) CDialerApp_EndDlgEx(pMe, IDD_TIMEPAUSE);
    return FALSE;
  }

  // check to see if more timepauses are in the string or not
  if((WSTRCHR((pMe->m_pszTimePauseRemaining+1), 'T') != NULL) || (WSTRCHR((pMe->m_pszTimePauseRemaining+1), 'H') != NULL))
  {
    // more time pauses are in the string
    AECHAR *pszFirstPause = NULL;
    AECHAR *pszHardPause = WSTRCHR((pMe->m_pszTimePauseRemaining+1), 'H');
    AECHAR *pszTimePause = WSTRCHR((pMe->m_pszTimePauseRemaining+1), 'T');
    AECHAR *pszBufBegin = NULL;
    AECHAR *pszBufEnd = NULL;

    // init buffers
    pszBufBegin = (AECHAR*) MALLOC(sizeof(AECHAR) * DIALERAPP_NUMBER_BUFFER_SIZE);
    pszBufEnd = (AECHAR*) MALLOC(sizeof(AECHAR) * DIALERAPP_NUMBER_BUFFER_SIZE);
    if((pszBufEnd == NULL) || (pszBufBegin == NULL))
    {
      DIALER_ERR("No memory", 0,0,0);
      FREEIF(pszBufBegin);
      FREEIF(pszBufEnd);
      FREEIF(pszDTMF);
      return FALSE;
    }

    // find the next time pause
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

    // break up the strings and move data around
    (void) WSTRNCOPYN(pszBufBegin, DIALERAPP_NUMBER_BUFFER_SIZE, pMe->m_pszTimePauseRemaining, (pszFirstPause-pMe->m_pszTimePauseRemaining));
    (void) WSTRNCOPYN(pszBufEnd, DIALERAPP_NUMBER_BUFFER_SIZE, pszFirstPause, WSTRLEN(pszFirstPause));
    (void) WSTRNCOPY(pszDTMF, DIALERAPP_NUMBER_BUFFER_SIZE, (pszBufBegin+1));
    (void) WSTRLCAT(pMe->m_pszTimePauseSent, pszBufBegin, (DIALERAPP_NUMBER_BUFFER_SIZE*2));
    (void) WSTRNCOPY(pMe->m_pszTimePauseRemaining, DIALERAPP_NUMBER_BUFFER_SIZE, pszBufEnd);

    FREE(pszBufBegin);
    FREE(pszBufEnd);
  }
  else
  {
    // no more pauses in the string
    (void) WSTRNCOPY(pszDTMF, DIALERAPP_NUMBER_BUFFER_SIZE, (pMe->m_pszTimePauseRemaining+1));
    (void) MEMSET(pMe->m_pszTimePauseSent, 0, sizeof(AECHAR)*(DIALERAPP_NUMBER_BUFFER_SIZE*2));
    (void) MEMSET(pMe->m_pszTimePauseRemaining, 0, sizeof(AECHAR)*DIALERAPP_NUMBER_BUFFER_SIZE);

    if(WSTRLEN(pszDTMF) == 0)
    {
      // just a lone pause at end so do nothing
      FREE(pszDTMF);
      (void) CDialerApp_EndDlgEx(pMe, IDD_TIMEPAUSE);
      return TRUE;
    }
  }

  /* Get the length of the digits.  It may be multiple pauses together
   * so the length may be zero.
   */
  wDTMFLen = (uint16)WSTRLEN(pszDTMF);
  if (wDTMFLen > 0)
  {
    //send DTMF burst
    if(CDialerApp_CheckAEEReturnStatus(pMe, ICM_BurstDTMF(pMe->m_pICM, pMe->m_byCurrTimePauseCall, AEECM_DTMF_ON_150, AEECM_DTMF_OFF_100, pszDTMF)) == FALSE)
    {
      DIALER_ERR("ICM_BurstDTMF FAILED", 0, 0, 0);
      FREEIF(pszDTMF);
      (void) CDialerApp_EndDlgEx(pMe, IDD_TIMEPAUSE);
      return FALSE;
    }

    // play the DTMFs locally
    wDTMFLen = (uint16)WSTRLEN(pszDTMF);
    pSoundArray = (snd_compact_tone_type*) MALLOC(sizeof(snd_compact_tone_type) * wDTMFLen * 2);
    if (pSoundArray == NULL)
    {
      DIALER_ERR("No memory", 0,0,0);
      FREEIF(pszDTMF);
      (void) CDialerApp_EndDlgEx(pMe, IDD_TIMEPAUSE);
      return FALSE;
    }

    MEMSET(pSoundArray, 0, sizeof(snd_compact_tone_type) * wDTMFLen * 2);

    for(i=0; i<wDTMFLen; i++)
    {
      pSoundArray[j].tone = CDialerApp_TimePauseDlg_DigitToSound(pszDTMF[i]);
      pSoundArray[j++].param.duration_ms = 150;
      pSoundArray[j].tone = SND_SILENCE;  // silent
      pSoundArray[j++].param.duration_ms = 100;
    }

    pSoundArray[--j].tone = SND_STOP;  // stop the sound
    pSoundArray[j].param.duration_ms = 0;

    uisnd_multi(FALSE, pSoundArray, FALSE);
    FREEIF(pSoundArray);
  }

  FREEIF(pszDTMF);

  if(WSTRLEN(pMe->m_pszTimePauseRemaining) == 0)
  {
    // no more DTMFs to send
    (void) CDialerApp_EndDlgEx(pMe, IDD_TIMEPAUSE);
    return TRUE;
  }


  // set timer if needed
  if(pMe->m_pszTimePauseRemaining[0] == (AECHAR) 'T')
  {
    // set a timer for time pause
    (void) ISHELL_SetTimer(pMe->a.m_pIShell, 3000,
      (PFNNOTIFY) CDialerApp_TimePauseDlg_Send, (void*) pMe);
  }

  return CDialerApp_TimePauseDlg_DisplayRefresh(pMe);
}

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_DigitToSound

DESCRIPTION: Translate AECHAR to sound enum for burst DTMF playback

PARAMETERS:
  cDigit: AECHAR to translate

RETURN VALUE:
  snd_tone_type: translated tone

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static snd_tone_type CDialerApp_TimePauseDlg_DigitToSound(AECHAR cDigit)
{
  PRINT_FUNCTION_NAME();
  if(cDigit == '*')
  {
    return((snd_tone_type)SND_STAR);
  }
  else if(cDigit == '#')
  {
    return((snd_tone_type)SND_POUND);
  }
  else
  {
    if((cDigit < '0') || (cDigit > '9'))
    {
      DIALER_ERR("Digit out of bounds", 0, 0, 0);
      return SND_ERR;
    }
    return((snd_tone_type)(cDigit - '0'+ SND_0));
  }
}
//lint -restore

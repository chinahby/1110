/*===========================================================================
  FILE: SMSDialogHandler.c

  SERVICES: SMS Dialog Handlers
 
  GENERAL DESCRIPTION:
      This file implements all SMS dialog handlers which are used
      by the SMSApp to bring up various SMS screens to the user

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright © 2002 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES FOR MODULE
===========================================================================*/

#include "gsm1xsms_res.h"          // Applet Resource Header
#include "GSM1xSMSApp.h"           // Main Applet Header
#include "GSM1xSMSUtil.h"          // Applet Utilities Header
#include "GSM1xSMSMessageStore.h"  // Message Storage Header
#include "AEEConfig.h"


/*===========================================================================
                           DEFINITIONS AND CONSTANTS
===========================================================================*/

// special flag to mark the text control to track properties for.
// KY Added
#define TP_DISPLAY_COUNT 0x10000000
// Time (in ms) before a MsgBox will close automatically
#define MSGBOX_TIMEOUT 5000

#define SEND_FAILURE_MSG_TIMEOUT 5000

// Max number of characters allowed in the MsgBox text
#define MSGBOX_MAXTEXTLEN  256

// Time (in ms) before a YesNo Box will close automatically
#define YESNO_TIMEOUT 6000


/*===========================================================================
                               MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONs
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

static void DialogTimeoutCallback(void *pUser);

static void SetMenuStyle(IShell *shell, IMenuCtl *menu);

static boolean HandleMainDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                     uint16 wParam, uint32 dwParam
                                    );
static boolean HandleYesNoDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                      uint16 wParam, uint32 dwParam
                                     );
static boolean HandleMsgBoxDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                       uint16 wParam, uint32 dwParam
                                      );
static boolean HandleMailBoxListDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                            uint16 wParam, uint32 dwParam
                                           );
static boolean HandleInMsgDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                      uint16 wParam, uint32 dwParam
                                     );
static boolean HandleReplyInfoDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                          uint16 wParam, uint32 dwParam
                                         );
static boolean HandleOutMsgDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                       uint16 wParam, uint32 dwParam
                                      );
static boolean HandleMessageArrivedDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                               uint16 wParam, uint32 dwParam
                                              );
static boolean HandleEraseMsgsDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                          uint16 wParam, uint32 dwParam
                                         );
static boolean HandleMsgSettingsDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                            uint16 wParam, uint32 dwParam
                                           );
static boolean HandleMsgAlertDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                         uint16 wParam, uint32 dwParam
                                        );
static boolean HandleVMNumSettingsDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                              uint16 wParam, uint32 dwParam
                                             );
static boolean HandleVMNumEditDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                          uint16 wParam, uint32 dwParam
                                         );
static boolean HandleVoiceMailDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                          uint16 wParam, uint32 dwParam
                                         );


static boolean HandleSIMMsgDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                        uint16 wParam, uint32 dwParam
                                       );
static boolean HandleMsgMoveDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                        uint16 wParam, uint32 dwParam
                                       );


static boolean HandleOKDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                   uint16 wParam, uint32 dwParam
                                  );
static boolean HandleNumMsgReceivedDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                               uint16 wParam, uint32 dwParam
                                              );
static boolean HandleSentReceiveStatusDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                                  uint16 wParam, uint32 dwParam
                                                 );
static boolean HandleCreateMsgNumberDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                                uint16 wParam, uint32 dwParam
                                               );
static boolean HandleCreateMsgTextDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                              uint16 wParam, uint32 dwParam
                                             );
static boolean HandleSendOptionsDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                            uint16 wParam, uint32 dwParam
                                           );
static boolean HandleEraseOldMsgDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                            uint16 wParam, uint32 dwParam
                                           );
static boolean HandleSendPreformatDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                              uint16 wParam, uint32 dwParam
                                             );
static boolean HandleSendFailureDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                            uint16 wParam, uint32 dwParam
                                           );
static boolean HandleEditSCAddrDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                          uint16 wParam, uint32 dwParam
                                         );
static boolean HandleTestDialogEvent(SMSApp *pMe, AEEEvent eCode,
                                          uint16 wParam, uint32 dwParam
                                         );

static void SetMyDefaultMenuLook( IShell *ps, IMenuCtl *pm);

/*===========================================================================
                              GLOBAL DATA
===========================================================================*/

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/


/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

//------------------------------------------------------------------------------
//                        Public Function Definitions 
//------------------------------------------------------------------------------

/*=============================================================================
FUNCTION: GSM1xSMSDialogHandler_UpdateActiveDialogInfo

DESCRIPTION: Updates the key dialog parameters in the SMSApp object.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   wParam, dwParam [in]: Active dialog information

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.
   
COMMENTS:
   The updated SMS applet dialog attributes are later used to route the 
   dialog events to thier respective handlers.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSDialogHandler_UpdateActiveDialogInfo
(
   SMSApp       *pMe,
   uint16       wParam,
   uint32       dwParam
)
{
   DIALOG_EVENTHANDLER fcnPtr;

   switch (wParam)
   {
      case DLG_MAIN:
         fcnPtr = HandleMainDialogEvent;
         break;

      case DLG_YESNO:
         fcnPtr = HandleYesNoDialogEvent;
         break;

      case DLG_MSGBOX:
         fcnPtr = HandleMsgBoxDialogEvent;
         break;

      case DLG_MAIL_BOX_LIST:
         fcnPtr = HandleMailBoxListDialogEvent;
         break;

      case DLG_IN_MSG:
         fcnPtr = HandleInMsgDialogEvent;
         break;

      case DLG_REPLY_INFO:
         fcnPtr = HandleReplyInfoDialogEvent;
         break;

      case DLG_OUT_MSG:
         fcnPtr = HandleOutMsgDialogEvent;
         break;

      case DLG_MSG_ARRIVED:
         fcnPtr = HandleMessageArrivedDialogEvent;
         break;

      case DLG_ERASE_MSGS:
         fcnPtr = HandleEraseMsgsDialogEvent;
         break;

      case DLG_MS:
         fcnPtr = HandleMsgSettingsDialogEvent;
         break;

      case DLG_MS_ALERT:
         fcnPtr = HandleMsgAlertDialogEvent;
         break;

      case DLG_MS_VMNUM:
         fcnPtr = HandleVMNumSettingsDialogEvent;
         break;

      case DLG_MS_VMNUM_EDIT:
         fcnPtr = HandleVMNumEditDialogEvent;
         break;

      case DLG_VOICEMAIL:
         fcnPtr = HandleVoiceMailDialogEvent;
         break;

      case DLG_SIM_MSG:
         fcnPtr = HandleSIMMsgDialogEvent;
         break;

      case DLG_MSG_MOVE:
         fcnPtr = HandleMsgMoveDialogEvent;
         break;

      case DLG_OK:
         fcnPtr = HandleOKDialogEvent;
         break;

      case DLG_NUM_MSG_RECEIVED:
         fcnPtr = HandleNumMsgReceivedDialogEvent;
         break;

      case DLG_SENT_RECEIVE_STATUS:
         fcnPtr = HandleSentReceiveStatusDialogEvent;
         break;

      case DLG_EDIT_SC_ADDR:
         fcnPtr = HandleEditSCAddrDialogEvent;
         break;

      case DLG_CREATE_MSG:
         fcnPtr = HandleCreateMsgNumberDialogEvent;
         break;

      case DLG_CREATE_MSG_TEXT:
         fcnPtr = HandleCreateMsgTextDialogEvent;
         break;

      case DLG_SEND_OPTIONS:
         fcnPtr = HandleSendOptionsDialogEvent;
         break;

      case DLG_ERASE_OLD_MSGS:
         fcnPtr = HandleEraseOldMsgDialogEvent;
         break;

      case DLG_SEND_PREFORMAT:
         fcnPtr = HandleSendPreformatDialogEvent;
         break;

      case DLG_SEND_FAILURE:
         fcnPtr = HandleSendFailureDialogEvent;
         break;

      case DLG_TEST:
         fcnPtr = HandleTestDialogEvent;
         break;

      default:
         return;
   }
   if (pMe)
   {
      pMe->m_activeDlg = (IDialog*)dwParam;
      pMe->m_activeDlgHandleEvent = fcnPtr;
   }
}

/*=============================================================================
FUNCTION: GSM1xSMSDialogHandler_ShowDialog

DESCRIPTION:
   This function is used by various state handlers to bring up one of the many:)
   SMS dialogs. The calling function passes the dialog resource id as a 
   parameter to this function and this function creates the dialog.

PARAMETERS:
   *pMe: Pointer to the SMS Applet structure.
   dlgResId: Dialog Id.

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSDialogHandler_ShowDialog
(
   SMSApp           *pMe,
   uint16           dlgResId
)
{
   int iRet;

   // At most one dialog open at once
   if(ISHELL_GetActiveDialog(pMe->a.m_pIShell) != NULL)
   {
      // Looks like there is one dialog already opened. Flag an error an return
      // without doing anything.
      DBGPRINTF("Trying to create a dialog without closing the previous one");
      return;
   }

   iRet = ISHELL_CreateDialog(pMe->a.m_pIShell,GSM1XSMS_RES_FILE,dlgResId,NULL);

   if(iRet != SUCCESS)
   {
      DBGPRINTF("Failed to create the dialog in the SMS applet");
   }
}

/*=============================================================================
FUNCTION: GSM1xSMSDialogHandler_ShowMsgBox

DESCRIPTION:
   This function is used by various state handlers to bring up a dialog with 
   just a static control in it. The text (resource id) to be put in the static
   control is also passed as a parameter.

PARAMETERS:
   *pMe: Pointer to the SMS Applet structure
   msgTextResId:  Message text resource id to be shown in the static dialog.

RETURN VALUE:
    None

COMMENTS:                   

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSDialogHandler_ShowMsgBox
(
   SMSApp          *pMe,
   uint16           msgTextResId
)
{
   pMe->m_msgBox_textId = msgTextResId;

   // Create a message box dialog.
   GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_MSGBOX);
}

/*=============================================================================
FUNCTION: GSM1xSMSDialogHandler_ShowOKBox

DESCRIPTION:
   This function is used by various state handlers to bring up a dialog with 
   a static control and a OK softkey in it. The text (resource id) to be put in 
   the static control is also passed as a parameter.

PARAMETERS:
   *pMe: Pointer to the SMS Applet structure
   msgTextResId:  Message text resource id to be shown in the static dialog.

RETURN VALUE:
    None

COMMENTS:                   

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSDialogHandler_ShowOKBox
(
   SMSApp          *pMe,
   uint16           msgTextResId
)
{
   pMe->m_msgBox_textId = msgTextResId;

   GSM1xSMSDialogHandler_ShowDialog(pMe, DLG_OK);
}

/*=============================================================================
FUNCTION: GSM1xSMSDialogHandler_RouteDialogEvent

DESCRIPTION: Route a BREW event to the active dialog handler

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean GSM1xSMSDialogHandler_RouteDialogEvent
(
   SMSApp      *pMe,
   AEEEvent    eCode,
   uint16      wParam,
   uint32      dwParam
)
{
   if( (NULL == pMe->m_activeDlg) ||
       (NULL == pMe->m_activeDlgHandleEvent) )
   {
      return FALSE;
   }
   else
   {
      return pMe->m_activeDlgHandleEvent(pMe,eCode,wParam,dwParam);
   }
} 

//------------------------------------------------------------------------------
//   Local Function Definitions (used only in the SMSDialogHandler.c file)
//------------------------------------------------------------------------------

/*=============================================================================
FUNCTION: DialogTimeoutCallback

DESCRIPTION:
   Callback function passed to the BREW layer so that whenever the timer that
   we have set in the dialog handler expires, we can post a timeout event to the
   dialog.
   
PARAMETERS:
   *pUser: pUser is assumed to be a SMSApp pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void DialogTimeoutCallback(void *pUser)
{
   SMSApp *pMe = (SMSApp*)pUser;

   // Post a message to the dialog telling it to close
   (void) ISHELL_PostEvent(pMe->a.m_pIShell, 
                           AEECLSID_APP_GSM1XSMS,
                           EVT_DIALOG_TIMEOUT, 
                           0, 
                           0);
}


/*===========================================================================

FUNCTION SetMyDefaultMenuLook

DESCRIPTION

PROTOTYPE:


PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
static void SetMyDefaultMenuLook( IShell *ps, IMenuCtl *pm)
{
  AEEItemStyle normal, selected;
  AEEBitmapInfo info;
#if 0
  IBitmap *pIBitmap;
  int rv;
#endif

  ISHELL_GetItemStyle( ps, AEE_IT_MENU, &normal, &selected );

#if 0
  rv = IDISPLAY_GetDeviceBitmap(pMe->a.m_pIDisplay, IBitmap &pIBitmap);
  if (rv == SUCCESS)
  {
     rv = IBITMAP_GetInfo(pIBitmap, &info, sizeof(AEEBitmapInfo));
     if (rv != SUCCESS)
     {
        info.nDepth = 1
     }
  }

#endif
  // TODO figure out how to get pMe and enable color displays
  info.nDepth = 1;
  if (info.nDepth > 1)
  {
      normal.ft = AEE_FT_NONE;
      normal.xOffset = normal.yOffset = 0;
      normal.roImage = AEE_RO_MASK;

      selected = normal;
   }
   else
   {
      normal.ft = AEE_FT_NONE;
      normal.xOffset = normal.yOffset = 0;
      normal.roImage = AEE_RO_COPY;  

      selected = normal;
      selected.roImage = AEE_RO_XOR;
   }

  IMENUCTL_SetStyle( pm, &normal, &selected );
  IMENUCTL_SetProperties( pm, MP_UNDERLINE_TITLE );
}

/*=============================================================================
FUNCTION: SetMenuStyle

DESCRIPTION:
   Set menu style.

PARAMETERS:
   *menu [in] : Pointer to the menu control for which the style needs to be set.

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void SetMenuStyle(IShell *shell, IMenuCtl *menu)
{
   SetMyDefaultMenuLook(shell, menu);
}

/*=============================================================================
FUNCTION: HandleMainDialogEvent

DESCRIPTION:
   This dialog event brings up the main SMS screen (the very first screen seen
   by the user from the SMS option selection). 

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleMainDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)
   
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg, MAIN_MENU);
   if(menu == NULL)
   {
      return FALSE;
   }
   
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AECHAR szFormat[AECHAR_64_BUF_SIZE];
         AECHAR szBuf[AECHAR_64_BUF_SIZE];
         CtlAddItem cai;

         GSM1xSMSApp_UpdateMboxCounters(pMe);

         // Load the Voice Mail button string
         (void)ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                    IDS_VOICE_MAIL_FMT,
                                    szFormat, sizeof(szFormat));
         // Insert number of voice messages to the menu item string.
         WSPRINTF(szBuf, sizeof(szBuf), szFormat, pMe->m_cfg.m_wNumVMMsgs);

         // Update the menu item text
         IMENUCTL_SetItemText(menu, MAIN_MENUITEM_VM, NULL, 0, szBuf);

         // We need to change the voice mail menu item font property based on
         // whether there are any pending voice mails or not.
         if(pMe->m_cfg.m_wNumVMMsgs)
         {
            // change the item text to bold.
            MEMSET(&cai, 0, sizeof(cai));
            cai.wFont = (uint16) AEE_FONT_BOLD;
            (void) IMENUCTL_SetItem(menu, MAIN_MENUITEM_VM, MSIF_FONT, &cai);
         }

         // Load the Inbox button string
         (void)ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                    IDS_INBOX_FMT,
                                    szFormat, sizeof(szFormat));

         // Insert number of inbox messages to the menu item string.
         WSPRINTF(szBuf, sizeof(szBuf), szFormat, 
                  pMe->m_wNumInMsgsNew, pMe->m_wNumInMsgs);     

         IMENUCTL_SetItemText(menu, MAIN_MENUITEM_INBOX, NULL, 0, szBuf);

         // We need to change the inbox menu item font property based on
         // whether there are any new inbox messages.
         if(pMe->m_wNumInMsgsNew)
         {
            // change the item text to bold.
            MEMSET(&cai, 0, sizeof(cai));
            cai.wFont = (uint16) AEE_FONT_BOLD;
            (void) IMENUCTL_SetItem(menu,MAIN_MENUITEM_INBOX,MSIF_FONT,&cai);
         }

         if (pMe->m_bSSTDisabled)
         {
            // Load the SIM button string
            (void)ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                    IDS_SIM_NO_ACCESS,
                                    szBuf, sizeof(szBuf));
         }
         else if (pMe->m_IGSMSMSInit)
         {
            // Load the SIM button string
            (void)ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                    IDS_SIM_FMT,
                                    szFormat, sizeof(szFormat));

            // Insert number of SIM messages to the menu item string.
            WSPRINTF(szBuf, sizeof(szBuf), szFormat, 
                     pMe->m_wNumSIMMsgsNew, pMe->m_wNumSIMMsgs);     
         }
         else
         {
            // Load the SIM button string
            (void)ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                    IDS_SIM_NOT_INIT,
                                    szBuf, sizeof(szBuf));
         }

         IMENUCTL_SetItemText(menu, MAIN_MENUITEM_SIM, NULL, 0, szBuf);

         // We need to change the SIM menu item font property based on
         // whether there are any new SIM messages.
         if(pMe->m_wNumSIMMsgsNew)
         {
            // change the item text to bold.
            MEMSET(&cai, 0, sizeof(cai));
            cai.wFont = (uint16) AEE_FONT_BOLD;
            (void) IMENUCTL_SetItem(menu,MAIN_MENUITEM_SIM,MSIF_FONT,&cai);
         }

         // Load the Outbox button string
         (void)ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                    IDS_OUTBOX_FMT,
                                    szFormat, sizeof(szFormat));

         // Insert number of Outbox messages to the menu item string.
         WSPRINTF(szBuf, sizeof(szBuf), szFormat,
                  pMe->m_wNumOutMsgs);     // Show num msgs

#if !defined(GSM1XSMSAPP_UNIT_TEST)
//#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
         {
            IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg, MAIN_MENU);
            if(menu != NULL)
            {
               (void) IMENUCTL_DeleteItem(menu, MAIN_MENUITEM_TEST);
            }
         }
#endif
//#endif

         IMENUCTL_SetItemText(menu, MAIN_MENUITEM_OUTBOX, NULL, 0, szBuf);

         IMENUCTL_SetSel(menu, pMe->m_mainMenuSel);
         SetMenuStyle(pMe->a.m_pIShell, menu);
         return TRUE;
      }

      case EVT_DIALOG_END:
         return TRUE;

      case EVT_KEY:
         // Handle only a case where the user hit the "Send" key while the 
         // cursor is on the "Voice Mail" menu item.

         if((wParam == (uint16) AVK_SEND) &&
            (IMENUCTL_GetSel(menu) == MAIN_MENUITEM_VM))
         {
            CLOSE_DIALOG(DLGRET_MAIN_SEND);
            return TRUE;
         }
         
         // Otherwise, let the BREW handle this event for us.
         return FALSE;

      case EVT_COMMAND:
         // Check what menu item the user selected and set the dlgResult 
         // accordingly.
         pMe->m_mainMenuSel = IMENUCTL_GetSel(menu);
         switch(wParam)
         {
            case MAIN_MENUITEM_VM:
               CLOSE_DIALOG(DLGRET_MAIN_VM)
               return TRUE;

            case MAIN_MENUITEM_INBOX:
               CLOSE_DIALOG(DLGRET_MAIN_INBOX)
               return TRUE;

            case MAIN_MENUITEM_CREATE:
               CLOSE_DIALOG(DLGRET_MAIN_CMSG)
               return TRUE;

            case MAIN_MENUITEM_SENDPREFORMAT:
               CLOSE_DIALOG(DLGRET_MAIN_SENDPREFORMAT)
               return TRUE;

            case MAIN_MENUITEM_OUTBOX:
               CLOSE_DIALOG(DLGRET_MAIN_OUTBOX)
               return TRUE;

            case MAIN_MENUITEM_SIM:
               CLOSE_DIALOG(DLGRET_MAIN_SIM)
               return TRUE;

            case MAIN_MENUITEM_ERASE:
               CLOSE_DIALOG(DLGRET_MAIN_ERASE)
               return TRUE;

            case MAIN_MENUITEM_SETTINGS:
               CLOSE_DIALOG(DLGRET_MAIN_SETTINGS)
               return TRUE;

#ifdef GSM1XSMSAPP_UNIT_TEST
            case MAIN_MENUITEM_TEST:
               CLOSE_DIALOG(DLGRET_MAIN_TEST)
               return TRUE;
#endif
            case MAIN_MENUITEM_ABOUT:
               CLOSE_DIALOG(DLGRET_MAIN_ABOUT)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleMainDialogEvent()

/*=============================================================================
FUNCTION: HandleYesNoDialogEvent

DESCRIPTION:
   This common dialog handler is used to disply a dialog with a static control
   and two softkeys (yes and no). This is created from several SMS states.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleYesNoDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;
         AECHAR  szText[MSGBOX_MAXTEXTLEN];
         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                        SK_YESNO);
         if(NULL == menu)
         {
            return FALSE;
         }

         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         // Fill the static control of the yesno dialog with the yesno
         // dialog title (set by the calling function that created this
         // dialog)
         (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                     GSM1XSMS_RES_FILE,
                                     pMe->m_yesnoDialogTitle,
                                     szText,
                                     sizeof(szText));

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                               (ST_CENTERTEXT | ST_MIDDLETEXT));

         (void) ISTATIC_SetText(pMe->m_msgBox_IStatic,
                                NULL,
                                szText,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);

         // If yesno timeout flag is set then we start the timer. Otherwise,
         // no action is taken w.r.t the timer.
         if(pMe->m_yesnoTimeout)
         {
            IMENUCTL_SetSel(menu, SK_YESNO_NO);
            (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                                   YESNO_TIMEOUT,
                                   DialogTimeoutCallback,
                                   pMe);
         }

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                   DialogTimeoutCallback,
                                   pMe);

         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;
     
      case EVT_KEY_PRESS:
         // We are using the EVT_KEY_PRESS event here instead of
         // the normal EVT_KEY event.  This is because in BREW 2.0
         // when an IMenuCtl is the active control in a dialog it will
         // always handle the UP/DOWN EVT_KEY events, so those events
         // will never be sent to the applet.
         if ((AVK_UP   == (AVKType) wParam) ||
             (AVK_DOWN == (AVKType) wParam) ) {
            return ISTATIC_HandleEvent(pMe->m_msgBox_IStatic, EVT_KEY, 
                                       wParam, dwParam);
         }
         break;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;
            
            default:
               break;
         }
         break;

      case EVT_DIALOG_TIMEOUT:
         CLOSE_DIALOG(DLGRET_CANCELED)
         return TRUE;

      case EVT_COMMAND:

         // See what softkey the user selected and set the dialog result value.
         switch(wParam)
         {
            case SK_YESNO_NO:
               CLOSE_DIALOG(DLGRET_NO)
               return TRUE;

            case SK_YESNO_YES:
               CLOSE_DIALOG(DLGRET_YES)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleYesNoDialogEvent

/*=============================================================================
FUNCTION: HandleMsgBoxDialogEvent

DESCRIPTION:
   This dialog handler is to create a dialog with only a static control (no
   softkeys) in it. This is shared by many SMS states to give status updates
   and warning/informative messages to the user. 
   
   After the dialog is created, it starts a timer and when the timer expires
   it closes the dialog (in case if the dialog is not yet closed by the user)

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleMsgBoxDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(wParam)

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      {
         AEERect rc;
         AECHAR  szText[MSGBOX_MAXTEXTLEN];

         SETAEERECT(&rc,0,0,pMe->m_deviceInfo.cxScreen,
                    pMe->m_deviceInfo.cyScreen);

         // Set the static control text based on the user settings of the
         // m_msgBox_textId parameter.

         (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                     GSM1XSMS_RES_FILE,
                                     pMe->m_msgBox_textId,
                                     szText,
                                     sizeof(szText));

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                               (ST_CENTERTEXT | ST_MIDDLETEXT));

         (void) ISTATIC_SetText(pMe->m_msgBox_IStatic,
                                NULL,
                                szText,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);

         // Always start a timer for the message box. 
         (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                                MSGBOX_TIMEOUT,
                                DialogTimeoutCallback,
                                pMe);
         return TRUE;
      }

      case EVT_DIALOG_START:
         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         (void) ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);

         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY:
      // If the key is up/down then we pass it to the static control.
      if ((AVK_UP   == (AVKType) wParam) || (AVK_DOWN == (AVKType) wParam)) 
      {
         (void) ISTATIC_HandleEvent(pMe->m_msgBox_IStatic, 
                                    EVT_KEY, 
                                    wParam, 
                                    dwParam);
      }
      else 
      {
         // For any other key press (other than up/down), we close this dialog.
         CLOSE_DIALOG(DLGRET_MSGBOX_OK)
      }
      return TRUE;

      case EVT_DIALOG_TIMEOUT:
         // If the timer expires then close the dialog.
         CLOSE_DIALOG(DLGRET_MSGBOX_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // HandleMsgBoxDialogEvent()

/*=============================================================================
FUNCTION: HandleMailBoxListDialogEvent

DESCRIPTION:
   This dialog is shared by Inbox/Outbox and the SIM box states to show a list
   of Inbox/Outbox and the SIM messages.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleMailBoxListDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{

   uint32 menuItemData;

   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  MENU_MAILBOX);

   if(NULL == menu)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         // The m_eMBoxType attribute of the SMSApp (pMe) object is already
         // set by the calling function. All we have to do here is add items
         // to the menu using the helper function.

         GSM1xSMSApp_UpdateMenuItemsForMailBox(pMe,menu);
         IMENUCTL_SetSel(menu, pMe->m_prevMenuItemSel);
         return TRUE;

      case EVT_DIALOG_END:
         if(pMe->m_suspending)
         {
            pMe->m_wCurrMsgID = IMENUCTL_GetSel(menu);
         }
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            case AVK_SEND:
               // We process the Send key only if the user selected a message
               // from the inbox.
               if(pMe->m_eMBoxType == SMS_MB_INBOX)
               {
                  pMe->m_prevMenuItemSel = IMENUCTL_GetSel(menu);
                  // Get the box type and ID from the parameter
                  (void)IMENUCTL_GetItemData(menu,
                                             pMe->m_prevMenuItemSel,
                                             &menuItemData);
                  GSM1xSMSUtil_DecodeMBoxAndID(menuItemData, &pMe->m_eMBoxType, 
                                          &pMe->m_wCurrMsgID);
                  CLOSE_DIALOG(DLGRET_INBOX_SEND);
                  return TRUE;
               }
               break;

            default:
               break;

         }
         break;

      case EVT_COMMAND:
         pMe->m_prevMenuItemSel = IMENUCTL_GetSel(menu);
         // Get the box type and ID from the parameter
         (void)IMENUCTL_GetItemData(menu, wParam, &menuItemData);
         GSM1xSMSUtil_DecodeMBoxAndID(menuItemData, &pMe->m_eMBoxType, 
                                 &pMe->m_wCurrMsgID);
         CLOSE_DIALOG(DLGRET_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // HandleMailBoxListDialogEvent

/*=============================================================================
FUNCTION: HandleInMsgDialogEvent

DESCRIPTION:
   Dialog handler that creates a dialog to show the message selected by the
   user from the Inbox.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleInMsgDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg, SK_IN_MSG);
   if(NULL == menu)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;
         uint16  nHeaderResID = IDS_IN_MSG_LABEL;
         AECHAR  NewLineStr[2] = {(AECHAR) '\n', (AECHAR) '\0'};
         AECHAR  *szBuf64  = NULL;
         AECHAR  *szBuf512 = NULL;

         // Before we do anything, allocate memory for our local buffers.
         // If we failed to allocate memory then return right away :(
         szBuf64 = (AECHAR *) MALLOC(AECHAR_64_BUF_SIZE * sizeof(AECHAR));
         if(szBuf64 == NULL)
         {
            DBGPRINTF("Fatal memory allocation failure");
            return FALSE;
         }

         szBuf512 = (AECHAR *) MALLOC(AECHAR_512_BUF_SIZE * sizeof(AECHAR));
         if(szBuf512 == NULL)
         {
            FREE(szBuf64);
            DBGPRINTF("Fatal memory allocation failure");
            return FALSE;
         }

         //IMENUCTL_DeleteItem(menu, SK_IN_MSG_MOVESIM);

         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         // Set the title to be in the center.
         ISTATIC_SetProperties(pMe->m_msgBox_IStatic, ST_CENTERTITLE);

         // Load the message label from the resource file.
         nHeaderResID = IDS_IN_MSG_LABEL;

         (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                    GSM1XSMS_RES_FILE,
                                    nHeaderResID,
                                    szBuf512,
                                    AECHAR_512_BUF_SIZE * sizeof(AECHAR));

         // Append a new line
         WSTRCAT(szBuf512, NewLineStr);

         // Copy the from field to the display text
         if((WSTRLEN(pMe->m_msIn.m_szTitle) > 0) &&
            (WSTRNCMP(pMe->m_msIn.m_szTitle, pMe->m_msIn.m_szMessage,
                      sizeof(pMe->m_msIn.m_szTitle)/sizeof(AECHAR)))
           )
         {
            // There's a title and the title is not derived from the message,
            // which means the title is either a contact name or a number
            (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                       GSM1XSMS_RES_FILE,
                                       IDS_IN_FROM,
                                       szBuf64, 
                                       AECHAR_64_BUF_SIZE * sizeof(AECHAR));

            // Append the FROM information
            WSPRINTF(szBuf512,
                     AECHAR_512_BUF_SIZE * sizeof(AECHAR),
                     szBuf64,
                     pMe->m_msIn.m_szTitle);

            // Append a new line
            WSTRCAT(szBuf512, NewLineStr);
         }

         // Append the message text to the body.
         if((WSTRLEN(szBuf512) + WSTRLEN(pMe->m_msIn.m_szMessage)) < 
            AECHAR_512_BUF_SIZE)
         {
            WSTRCAT(szBuf512, pMe->m_msIn.m_szMessage);
         }

         // Append date to end of message body.
         GSM1xSMSUtil_SecsToDateString(pMe, 
                                  pMe->m_msIn.m_dwTimeStamp,
                                  szBuf64, 
                                  AECHAR_64_BUF_SIZE * sizeof(AECHAR));

         if((WSTRLEN(szBuf512) + WSTRLEN(szBuf64)) < 
            AECHAR_512_BUF_SIZE_MINUS_ONE)
         {
            WSTRCAT(szBuf512, NewLineStr);
            WSTRCAT(szBuf512, szBuf64);
         }

         // Show the message body.
         // Want the PAGE/MESSAGE/... header to scroll too.
         (void)ISTATIC_SetText(pMe->m_msgBox_IStatic, 
                               NULL,
                               szBuf512,
                               AEE_FONT_NORMAL, 
                               AEE_FONT_NORMAL);
         // We don't need the buffers anymore. Release the memory right away.
         FREE(szBuf64);
         FREE(szBuf512);

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, 
                                 AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 
                                 0, 
                                 0);
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;                   

      case EVT_KEY_PRESS:
         // We are using the EVT_KEY_PRESS event here instead of
         // the normal EVT_KEY event.  This is because in BREW 2.0
         // when an IMenuCtl is the active control in a dialog it will
         // always handle the UP/DOWN EVT_KEY events, so those events
         // will never be sent to the applet.
         if ((AVK_UP   == (AVKType) wParam) ||
             (AVK_DOWN == (AVKType) wParam) ) {
            return ISTATIC_HandleEvent(pMe->m_msgBox_IStatic, EVT_KEY, 
                                       wParam, dwParam);
         }
         break;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // See what softkey the user selected and set the dlgResult. Close the
         // dialog to return to the state machine.
         switch(wParam)
         {
            case SK_IN_MSG_NEXT:
               CLOSE_DIALOG(DLGRET_IN_NEXT)
               return TRUE;

            case SK_IN_MSG_ERASE:
               CLOSE_DIALOG(DLGRET_IN_ERASE)
               return TRUE;

            case SK_IN_MSG_FORWARD:
               CLOSE_DIALOG(DLGRET_IN_FORWARD)
               return TRUE;

            case SK_IN_MSG_REPLY:
               CLOSE_DIALOG(DLGRET_IN_REPLY)
               return TRUE;

            case SK_IN_MSG_SENDER:
               CLOSE_DIALOG(DLGRET_IN_SENDER)
               return TRUE;

            case SK_IN_MSG_MOVESIM:
               CLOSE_DIALOG(DLGRET_IN_MOVESIM)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleInMsgBoxDialogEvent

/*=============================================================================
FUNCTION: HandleReplyInfoDialogEvent

DESCRIPTION:
   Dialog handler to show a dialog with all the reply information. Also support
   softkeys to send the message to the sender, call the sender and save the 
   sender information in the contact list.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleReplyInfoDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  MENU_REPLY_INFO);

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         GSM1xSMSApp_AddAddresseeToMenu(pMe,menu);

         return TRUE;

      case EVT_DIALOG_END:
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // Only one address is supported
         if(wParam == ADD_CMD_BASE)
         {
            // Determine the address from the menu command.
            GSM1xSMSMessageStore_GetAddresseeAtIndex( pMe,
                                                 pMe->m_eMBoxType,
                                                 pMe->m_wCurrMsgID,
                                                 pMe->m_strReplyInfoNum,
                                                 MAX_REPLYINFO_CHARS);
         }

         // Map the softkey to set the dialog result. 
         switch(wParam)
         {
            case SK_REPLY_INFO_CALL:
               CLOSE_DIALOG(DLGRET_CALL)
               return TRUE;

            case SK_REPLY_INFO_REPLY:
               CLOSE_DIALOG(DLGRET_REPLY)
               return TRUE;

            default:
               // Do not assert here. Selecting the address from the list
               // results in returning the id of the selected address.
               // Currently, we display only one number and we already have
               // this information (in the m_szBuf128 temp buffer.
               // So, no need to do anything here.
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleReplyInfoDialogEvent

/*=============================================================================
FUNCTION: HandleOutMsgDialogEvent

DESCRIPTION:
   Dialog handler that creates a dialog to show the message selected by the
   user from the Outbox.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleOutMsgDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg, SK_OUT_MSG);
   if(NULL == menu)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;
         AECHAR  *szBuf512 = NULL;
         AECHAR   NewLineStr[2] = {(AECHAR) '\n', (AECHAR) '\0'};

         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         szBuf512 = (AECHAR *) MALLOC(AECHAR_512_BUF_SIZE * sizeof(AECHAR));
         if(szBuf512 == NULL)
         {
            DBGPRINTF("Fatal memory allocation failure");
            return FALSE;
         }

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic, ST_CENTERTITLE);

         szBuf512[0] = (AECHAR)'\0';

         // Copy the To field to the display text
         if((WSTRLEN(pMe->m_msOut.m_szTo) > 0))
         {
             AECHAR *szBuf64 = (AECHAR *) MALLOC(AECHAR_64_BUF_SIZE * sizeof(AECHAR));

             if(szBuf64 != NULL)
             {
                (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                           GSM1XSMS_RES_FILE,
                                           IDS_OUT_TO,
                                           szBuf64, 
                                           AECHAR_64_BUF_SIZE * sizeof(AECHAR));

                // Append the To information
                WSPRINTF(szBuf512,
                         AECHAR_512_BUF_SIZE * sizeof(AECHAR),
                         szBuf64,
                         pMe->m_msOut.m_szTo);

                // Append a new line
                WSTRCAT(szBuf512, NewLineStr);

                FREE(szBuf64);
             }
         }
         WSTRCAT(szBuf512, pMe->m_msOut.m_szMessage);

         // Append a new line
         WSTRCAT(szBuf512, NewLineStr);

         // Set the static text to be the message body.
         (void)ISTATIC_SetText(pMe->m_msgBox_IStatic, NULL,
                               szBuf512,
                               AEE_FONT_BOLD, AEE_FONT_NORMAL);

         FREE(szBuf512);

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY_PRESS:
         // We are using the EVT_KEY_PRESS event here instead of
         // the normal EVT_KEY event.  This is because in BREW 2.0
         // when an IMenuCtl is the active control in a dialog it will
         // always handle the UP/DOWN EVT_KEY events, so those events
         // will never be sent to the applet.
         if ((AVK_UP   == (AVKType) wParam) ||
             (AVK_DOWN == (AVKType) wParam) ) {
            return ISTATIC_HandleEvent(pMe->m_msgBox_IStatic, EVT_KEY, 
                                       wParam, dwParam);
         }
         break;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;
            
            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // Map the softkeys on the dialog control to the dialog result.
         switch(wParam)
         {
            case SK_OUT_MSG_DONE:
               CLOSE_DIALOG(DLGRET_OUT_DONE)
               return TRUE;

            case SK_OUT_MSG_ERASE:
               CLOSE_DIALOG(DLGRET_OUT_ERASE)
               return TRUE;

            case SK_OUT_MSG_SEND:
               CLOSE_DIALOG(DLGRET_OUT_SEND)
               return TRUE;

            case SK_OUT_MSG_FORWARD:
               CLOSE_DIALOG(DLGRET_OUT_FORWARD)
               return TRUE;

			case SK_OUT_MSG_MOVESIM:
			   CLOSE_DIALOG(DLGRET_OUT_MOVESIM)
			   return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleOutMsgBoxDialogEvent

/*=============================================================================
FUNCTION: HandleMessageArrivedDialogEvent

DESCRIPTION:
   Dialog handler to show a dialog to the user with all the information about
   the newly received message.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleMessageArrivedDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg, 
                                                  SK_MSG_ARRIVED);
   if(NULL == menu)
   {
      return FALSE;
   }


   //if message is a class 1 or class 2, or if there is
   //no more room in NV, delete the save menu item.
   if(pMe->m_msIn.m_eClass == GSMSMS_MSG_CLASS_1 || 
 	   pMe->m_msIn.m_eClass == GSMSMS_MSG_CLASS_2) 
   {
	   IMENUCTL_DeleteItem(menu, SK_MSG_ARRIVED_SAVE);
   }
    //if message is a class 0 or class none, delete the view menu item.
   else
   {
	   if((pMe->m_wNumInMsgs + pMe->m_wNumOutMsgs) >= pMe->m_wMaxNVMsgs)
	   {
			IMENUCTL_DeleteItem(menu, SK_MSG_ARRIVED_SAVE);
         // Indicate that the message has never been saved
         pMe->m_wCurrMsgID = 0xffff;
	   }
	   else
	   {
			IMENUCTL_DeleteItem(menu, SK_MSG_ARRIVED_VIEW);
	   }
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;
         AECHAR  *szBuf64 = NULL;
         AECHAR  *szBuf256 = NULL;
         uint16  nHeaderResID = 0;
         int16   nTitleLen;
         int16   nMsgLen;
         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic, 
                               ST_CENTERTITLE|ST_CENTERTEXT);

         nHeaderResID = IDS_MSG_ARRIVAL_HEADER_MESSAGE;

         szBuf256 = (AECHAR *) MALLOC(AECHAR_256_BUF_SIZE * sizeof(AECHAR));
         if(szBuf256 == NULL)
         {
            // We can not continue here without allocating any memory for the
            // main buffer.
            DBGPRINTF("Fatal Memory Allocation Failure");
            return FALSE;
         }

         szBuf64 = (AECHAR *) MALLOC(AECHAR_64_BUF_SIZE * sizeof(AECHAR));
         if(szBuf64)
         {
            // Load the message header that indicates the message type
            (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                       GSM1XSMS_RES_FILE,
                                       nHeaderResID,
                                       szBuf64,
                                       AECHAR_64_BUF_SIZE * sizeof(AECHAR));
         }
         else
         {
            // Failed to allocate memory!!! It is okay to continue here as no
            // harm is done. We will not be able to show the title. But what the
            // heck ...
            DBGPRINTF("Memory Allocation Failure");
         }

         MEMSET(szBuf256,0, AECHAR_256_BUF_SIZE * sizeof(AECHAR));
         GSM1xSMSUtil_SecsToDateString(pMe, 
                                  pMe->m_msIn.m_dwTimeStamp,
                                  szBuf256, 
                                  AECHAR_256_BUF_SIZE * sizeof(AECHAR));

         // Append the message title.
         nTitleLen = (int16)WSTRLEN(pMe->m_msIn.m_szTitle);
         nMsgLen = (int16)WSTRLEN(szBuf256);

         if((nTitleLen != 0) && 
            (nTitleLen + nMsgLen) < AECHAR_256_BUF_SIZE_MINUS_ONE)
         {
            // An AECHAR string is two bytes per character.  The ASCII string
            // literal "\n" is really just this: {'\n', \0'},  but AECHAR 
            // equivalent to this string is {\n', '\0', '\0', '\0'}. The
            // first NULL byte completes the newline character and the 
            // second two NULL bytes are for the NULL word at the end of the 
            // AECHAR (Unicode) string.

            // The reason there are only two NULL bytes in the ASCIIZ string
            // literal "\n\0\0" is because there is an implicit NULL byte at 
            // the end of the string which is added by the compiler.

            WSTRCAT(szBuf256, (AECHAR*)"\n\0\0");
            WSTRCAT(szBuf256, pMe->m_msIn.m_szTitle);
         }
         else if((nMsgLen + MAX_TITLE_LEN) < AECHAR_256_BUF_SIZE_MINUS_ONE)
         {
            WSTRCAT(szBuf256, (AECHAR*)"\n\0\0");
            WSTRNCOPYN(&(szBuf256[nMsgLen + 1]),
                       (AECHAR_256_BUF_SIZE_MINUS_ONE - MAX_TITLE_LEN),
                       pMe->m_msIn.m_szMessage,
                       MAX_TITLE_LEN);
         }

         // Show the message information.
         (void)ISTATIC_SetText(pMe->m_msgBox_IStatic, 
                               szBuf64,
                               szBuf256,
                               AEE_FONT_BOLD, 
                               AEE_FONT_NORMAL);

         // We don't need the temporary buffers anymore. Release the memory
         // right away.
         if(szBuf64)
         {
            FREE(szBuf64);
         }

         if(szBuf256)
         {
            FREE(szBuf256);
         }

         // Start the alert, if necessary
         if(pMe->m_cfg.m_eMsgAlertPref != ALERT_NONE)
         {
            BeepType smsAlert = BEEP_MSG;
            // Map the alert from the SMS configuration structure to the
            // IShell Beep type
            switch(pMe->m_cfg.m_eMsgAlertPref)
            {
               case ALERT_VIB_ONCE:
                  smsAlert = BEEP_VIBRATE_ALERT;
                  break;

               case ALERT_VIB_REMIND:             
                  smsAlert = BEEP_VIBRATE_REMIND;
                  break;

               case ALERT_BEEP_ONCE:
                  smsAlert = BEEP_MSG;
                  break;

               case ALERT_BEEP_REMIND:
                  smsAlert = BEEP_REMINDER;
                  break;

               default:
                  DBGPRINTF("Invalid alert type is set in the SMS config database");
                  break;
            }
               
            ISHELL_Beep(pMe->a.m_pIShell, smsAlert, FALSE); // KY
         }

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);

         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }

         // Stop the alert
         ISHELL_Beep(pMe->a.m_pIShell, BEEP_OFF, FALSE); // KY

         return TRUE;

      case EVT_KEY_PRESS:
         // We are using the EVT_KEY_PRESS event here instead of
         // the normal EVT_KEY event.  This is because in BREW 2.0
         // when an IMenuCtl is the active control in a dialog it will
         // always handle the UP/DOWN EVT_KEY events, so those events
         // will never be sent to the applet.
         if ((AVK_UP   == (AVKType) wParam) ||
             (AVK_DOWN == (AVKType) wParam) ) {
            return ISTATIC_HandleEvent(pMe->m_msgBox_IStatic, EVT_KEY, 
                                       wParam, dwParam);
         }
         break;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            case AVK_SEND:
               CLOSE_DIALOG(DLGRET_MA_SEND);
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // Map the softkey (selected by the user) to the dialog result 
         // (dlgResult) and then close the dialog.
         switch(wParam)
         {
            case SK_MSG_ARRIVED_VIEW:
               CLOSE_DIALOG(DLGRET_MA_VIEW)
               return TRUE;

            case SK_MSG_ARRIVED_IGNORE:
               CLOSE_DIALOG(DLGRET_MA_IGNORE)
               return TRUE;

			case SK_MSG_ARRIVED_SAVE:
				CLOSE_DIALOG(DLGRET_MA_SAVE)
				return TRUE;
            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleMessageArrivedDialogEvent

/*=============================================================================
FUNCTION: HandleEraseMsgsDialogEvent

DESCRIPTION:
   Dialog handler to create a dialog which helps the user to erase all
   messages from the Inbox, Outbox, SIM box or all the old messages (only
   from the Inbox).                  

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleEraseMsgsDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)

   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  MENU_EMSGS);
   if(menu == NULL)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:

         IMENUCTL_SetSel(menu, pMe->m_prevMenuItemSel);
         SetMenuStyle(pMe->a.m_pIShell, menu);
         return TRUE;

      case EVT_DIALOG_END:
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // Hmmm! let's see which message box user wants to delete and set 
         // the dialog result (dlgResult) accordingly.
         pMe->m_prevMenuItemSel = IMENUCTL_GetSel(menu);
         switch(wParam)
         {
            case MENU_EMSGS_NO:
               CLOSE_DIALOG(DLGRET_EM_NO)
               return TRUE;

            case MENU_EMSGS_INBOX:
               CLOSE_DIALOG(DLGRET_EM_INBOX)
               return TRUE;

            case MENU_EMSGS_OUTBOX:
               CLOSE_DIALOG(DLGRET_EM_OUTBOX)
               return TRUE;

            case MENU_EMSGS_SIM:
               CLOSE_DIALOG(DLGRET_EM_SIM)
               return TRUE;

            case MENU_EMSGS_OLDINBOX:
               CLOSE_DIALOG(DLGRET_ERASE_OLD_MSGS)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleEraseMsgsDialogEvent()

/*=============================================================================
FUNCTION: HandleMsgSettingsDialogEvent

DESCRIPTION:
   Dialog handler to show a dialog which helps the user to change the 
   message settings (like voice mail number and the incoming page/message/
   voice mail alert type).

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleMsgSettingsDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)

   IMenuCtl *menu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_activeDlg, MENU_MS);
   if(menu == NULL)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         IMENUCTL_SetSel(menu, pMe->m_prevMenuItemSel);
         return TRUE;

      case EVT_DIALOG_END:
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // See what options user wants to change (voice mail number or the
         // alert type ?).
         pMe->m_prevMenuItemSel = IMENUCTL_GetSel(menu);
         switch(wParam)
         {
            case MENU_MS_VMNUM:
               CLOSE_DIALOG(DLGRET_MS_VMNUM)
               return TRUE;

            case MENU_MS_ALERT:
               CLOSE_DIALOG(DLGRET_MS_ALERT)
               return TRUE;

			case MENU_MS_EDIT_SC_ADDR:
               CLOSE_DIALOG(DLGRET_MS_EDIT_SC_ADDR)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleMsgSettingsDialogEvent

/*=============================================================================
FUNCTION: HandleMsgAlertDialogEvent

DESCRIPTION:
   Dialog handler to show a dialog which helps the user to change the incoming
   message/page/voice mail alert type.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleMsgAlertDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg, 
                                                        MENU_ALERT);
         uint16 menuItemId = MENUITEM_ALERT_NONE;

         if(menu == NULL)
         {
            return FALSE;
         }
         // Change the menu icons depending upon the users previous settings 
         // (stored in the configuration database and are already extracted
         // into the SMSApp object)

         switch(pMe->m_cfg.m_eMsgAlertPref)
         {
            case ALERT_NONE:
               menuItemId = MENUITEM_ALERT_NONE;
               break;
            
            case ALERT_VIB_ONCE:
               menuItemId = MENUITEM_ALERT_VO;
               break;
            
            case ALERT_VIB_REMIND:
               menuItemId = MENUITEM_ALERT_VR;
               break;
            
            case ALERT_BEEP_ONCE:
               menuItemId = MENUITEM_ALERT_BO;
               break;
            
            case ALERT_BEEP_REMIND:
               menuItemId = MENUITEM_ALERT_BR;
               break;
            
            default:
               ASSERT_NOT_REACHABLE;
               break;
         }

         GSM1xSMSUtil_InitMenuIcons(menu);

         // Based on the users previous settings, set the menu icon.
         GSM1xSMSUtil_SetMenuIcon(menu, menuItemId, TRUE);

         return TRUE;
      }

      case EVT_DIALOG_END:
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // Set the preference.
         switch(wParam)
         {
            case MENUITEM_ALERT_NONE:
               pMe->m_cfg.m_eMsgAlertPref = ALERT_NONE;
               break;
            
            case MENUITEM_ALERT_VO:
               pMe->m_cfg.m_eMsgAlertPref = ALERT_VIB_ONCE;
               break;
            
            case MENUITEM_ALERT_VR:
               pMe->m_cfg.m_eMsgAlertPref = ALERT_VIB_REMIND;
               break;
            
            case MENUITEM_ALERT_BO:
               pMe->m_cfg.m_eMsgAlertPref = ALERT_BEEP_ONCE;
               break;
            
            case MENUITEM_ALERT_BR:
               pMe->m_cfg.m_eMsgAlertPref = ALERT_BEEP_REMIND;
               break;
            
            default:
               ASSERT_NOT_REACHABLE;
               break;
         }

         CLOSE_DIALOG(DLGRET_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // HandleMsgAlertDialogEvent()

/*=============================================================================
FUNCTION: HandleVMNumSettingsDialogEvent

DESCRIPTION:
   Dialog handler to show a dialog which helps the user to change the voicemail
   number.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleVMNumSettingsDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;
         AECHAR  voiceMailNum[MAX_PH_DIGITS];

         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                        SOFTKEY_VM_NUM);
         if(NULL == menu)
         {
            return FALSE;
         }

         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         MEMCPY(&voiceMailNum[0], &pMe->m_cfg.m_szVoiceMailNum[0], 
             sizeof(voiceMailNum));

         // If we haven't retrieved the VoiceMail number yet, then we 
         // display a warning (infomative) screen to the user to edit/add 
         // a voice mail number.
         if(voiceMailNum[0] == (AECHAR)'\0')
         {
            AECHAR  szText[MSGBOX_MAXTEXTLEN];
            (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        GSM1XSMS_RES_FILE,
                                        IDS_MS_VM_WARNING, 
                                        szText,
                                        sizeof(szText));

            (void) ISTATIC_SetText(pMe->m_msgBox_IStatic, 
                                   NULL, 
                                   szText,
                                   AEE_FONT_NORMAL, 
                                   AEE_FONT_NORMAL);
         }
         else
         {
            AECHAR  szTitle[AECHAR_64_BUF_SIZE];
            (void) ISHELL_LoadResString(pMe->a.m_pIShell, 
                                        GSM1XSMS_RES_FILE,
                                        IDS_MS_VM_NUMBER, 
                                        szTitle,
                                        sizeof(szTitle));

            (void) ISTATIC_SetText(pMe->m_msgBox_IStatic, 
                                   szTitle, 
                                   voiceMailNum,
                                   AEE_FONT_BOLD, 
                                   AEE_FONT_NORMAL);

            ISTATIC_SetProperties(pMe->m_msgBox_IStatic, ST_CENTERTEXT);
         }

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }

         return TRUE;

      case EVT_KEY_PRESS:
         // We are using the EVT_KEY_PRESS event here instead of
         // the normal EVT_KEY event.  This is because in BREW 2.0
         // when an IMenuCtl is the active control in a dialog it will
         // always handle the UP/DOWN EVT_KEY events, so those events
         // will never be sent to the applet.
         if ((AVK_UP   == (AVKType) wParam) ||
             (AVK_DOWN == (AVKType) wParam) ) 
         {
            (void) ISTATIC_HandleEvent(pMe->m_msgBox_IStatic, 
                                       EVT_KEY, 
                                       wParam, 
                                       dwParam);
         }
         break;
         
      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // See what user wants to do? Is he just invoked this dialog to see
         // the previously set voice mail number or he wants to change the
         // number (by selecting the edit button from the softkey control 
         // menu)
         switch(wParam)
         {
            case SK_VMNUM_OK:
               CLOSE_DIALOG(DLGRET_MS_VMNUM_OK)
               return TRUE;
            
            case SK_VMNUM_EDIT:
               CLOSE_DIALOG(DLGRET_MS_VMNUM_EDIT)
               return TRUE;
            
            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleVMNumSettingsDialogEvent()

/*=============================================================================
FUNCTION: HandleVMNumEditDialogEvent

DESCRIPTION:
   Dialog handler to show a dialog which helps the user to change the voicemail
   number. (Uses the text control to let the user enter a new number for the
   voice mail box)

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleVMNumEditDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)

   ITextCtl *textCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_activeDlg, 
                                                     VM_NUM_EDIT);
   AECHAR  voiceMailNum[MAX_PH_DIGITS];

   if(NULL == textCtl)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                        SOFTKEY_VM_EDIT);
         if(NULL == menu)
         {
            return FALSE;
         }
         
         MEMCPY(&voiceMailNum[0], &pMe->m_cfg.m_szVoiceMailNum[0], 
             sizeof(voiceMailNum));

         // Limit the user to enter only MAX_TO_CHARS chars.
         ITEXTCTL_SetMaxSize(textCtl, MAX_TO_CHARS);
         
         // Show the previously configured voice mail number to the user
         // (in the text edit control box)
         (void) ITEXTCTL_SetText(textCtl, voiceMailNum, -1);
         ITEXTCTL_SetProperties(textCtl, TP_FRAME);
         (void) ITEXTCTL_SetInputMode(textCtl, AEE_TM_NUMBERS);

         // Remove the multitap softkey as soon as possible
         (void) IMENUCTL_DeleteAll(menu);
         (void) IMENUCTL_AddItem(menu, GSM1XSMS_RES_FILE, IDS_MS_VMNUM_EDIT_OK,
                                 SK_VMNUM_EDIT_OK, (AECHAR*)NULL, 
                                 (uint32)NULL);
         return TRUE;
      }

      case EVT_DIALOG_END:
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // Set the preference.
         switch(wParam)
         {
            case SK_VMNUM_EDIT_OK:
               // Extract the number from the text control and update the 
               // object configuration structure parameters.
               (void) ITEXTCTL_GetText(textCtl, 
                                       voiceMailNum, 
                                       MAX_PH_DIGITS);

               MEMCPY(&pMe->m_cfg.m_szVoiceMailNum[0], &voiceMailNum[0],
                  sizeof(pMe->m_cfg.m_szVoiceMailNum));

               (void)ISHELL_SetPrefs(pMe->a.m_pIShell, 
                  AEECLSID_APP_GSM1XSMS, 
                  SMS_CFG_VERSION,
                  &pMe->m_cfg, 
                  sizeof(GSM1xSMSAppConfig));

               CLOSE_DIALOG(DLGRET_MS_VMNUM_EDIT_OK)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleVMNumEditDialogEvent()

/*=============================================================================
FUNCTION: HandleVoiceMailDialogEvent

DESCRIPTION:
   Dialog handler to create a dialog that shows a voice mail message 
   information to the user. This dialog also creates softkey controls to let
   the user reset the number of voice mails to zero and to call the voice mail
   number.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleVoiceMailDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg, 
                                                  SK_VOICEMAIL);
   if(NULL == menu)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;
         AECHAR  *szBuf64 = NULL;
         AECHAR  *szBuf256 = NULL;
         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                               ST_CENTERTITLE|ST_NOSCROLL);

         szBuf256 = (AECHAR *) MALLOC(AECHAR_256_BUF_SIZE * sizeof(AECHAR));
         if(szBuf256 == NULL)
         {
            // We can not continue here without allocating any memory for the
            // main buffer.
            DBGPRINTF("Fatal Memory Allocation Failure");
            return FALSE;
         }
         
         szBuf64 = (AECHAR *) MALLOC(AECHAR_64_BUF_SIZE * sizeof(AECHAR));
         if(szBuf64)
         {
            // Load the message header that indicates the message type
            (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                       GSM1XSMS_RES_FILE,
                                       IDS_IN_VOICEMAIL_LABEL,
                                       szBuf64,
                                       AECHAR_64_BUF_SIZE * sizeof(AECHAR));
         }
         else
         {
            // Failed to allocate memory!!! It is okay to continue here as no
            // harm is done. We will not be able to show the title. But what the
            // heck ...
            DBGPRINTF("Memory Allocation Failure");
         }

         // Append date to end of message body.
         MEMSET(szBuf256,0, AECHAR_256_BUF_SIZE * sizeof(AECHAR));
         GSM1xSMSUtil_SecsToDateString(pMe, 
                                  pMe->m_msIn.m_dwTimeStamp,
                                  szBuf256, 
                                  AECHAR_256_BUF_SIZE * sizeof(AECHAR));

         // Show the message information.
         (void)ISTATIC_SetText(pMe->m_msgBox_IStatic, 
                               szBuf64,
                               szBuf256,
                               AEE_FONT_BOLD, 
                               AEE_FONT_NORMAL);

         // We don't need the temporary buffers anymore. Release the memory
         // right away.
         if(szBuf64)
         {
            FREE(szBuf64);
         }

         if(szBuf256)
         {
            FREE(szBuf256);
         }

         // Start the alert, if necessary
         if(pMe->m_cfg.m_eMsgAlertPref != ALERT_NONE)
         {
            BeepType smsAlert = BEEP_MSG;
            // Map the alert from the SMS configuration structure to the
            // IShell Beep type
            switch(pMe->m_cfg.m_eMsgAlertPref)
            {
               case ALERT_VIB_ONCE:
                  smsAlert = BEEP_VIBRATE_ALERT;
                  break;

               case ALERT_VIB_REMIND:             
                  smsAlert = BEEP_VIBRATE_REMIND;
                  break;

               case ALERT_BEEP_ONCE:
                  smsAlert = BEEP_MSG;
                  break;

               case ALERT_BEEP_REMIND:
                  smsAlert = BEEP_REMINDER;
                  break;

               default:
                  DBGPRINTF("Invalid alert type is set in the SMS config database");
                  break;
            }
               
            ISHELL_Beep(pMe->a.m_pIShell, smsAlert, FALSE); // KY
         }

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY_PRESS:
         // We are using the EVT_KEY_PRESS event here instead of
         // the normal EVT_KEY event.  This is because in BREW 2.0
         // when an IMenuCtl is the active control in a dialog it will
         // always handle the UP/DOWN EVT_KEY events, so those events
         // will never be sent to the applet.
         if ((AVK_UP   == (AVKType) wParam) ||
             (AVK_DOWN == (AVKType) wParam) ) {
            return ISTATIC_HandleEvent(pMe->m_msgBox_IStatic, EVT_KEY, 
                                       wParam, dwParam);
         }
         break;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;
               
            case AVK_SEND:
               CLOSE_DIALOG(DLGRET_VOICEMAIL_SEND)
               return TRUE;
            
            default:
               break;
         }
         break;

      case EVT_COMMAND:
         switch(wParam)
         {
            case SK_VOICEMAIL_CALL:
               CLOSE_DIALOG(DLGRET_VOICEMAIL_CALL)
               return TRUE;

            case SK_VOICEMAIL_RESET:
               CLOSE_DIALOG(DLGRET_VOICEMAIL_RESET)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleVoiceMailDialogEvent

/*=============================================================================
FUNCTION: HandleSIMMsgDialogEvent

DESCRIPTION:
   Dialog handler to show a dialog to the user with information about selected
   message from the SIM message box menu.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleSIMMsgDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect  rc;
         AECHAR   *szBuf64 = NULL;
         AECHAR   *szBuf512 = NULL;
         AECHAR   NewLineStr[2] = {(AECHAR) '\n', (AECHAR) '\0'};
         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                        SK_SIM_MSG);
         if(NULL == menu)
         {
            return FALSE;
         }

         // Before we do anything, allocate memory for our local buffers.
         // If we failed to allocate memory then return right away :(
         szBuf64 = (AECHAR *) MALLOC(AECHAR_64_BUF_SIZE * sizeof(AECHAR));
         if(szBuf64 == NULL)
         {
            DBGPRINTF("Fatal memory allocation failure");
            return FALSE;
         }

         szBuf512 = (AECHAR *) MALLOC(AECHAR_512_BUF_SIZE * sizeof(AECHAR));
         if(szBuf512 == NULL)
         {
            FREE(szBuf64);
            DBGPRINTF("Fatal memory allocation failure");
            return FALSE;
         }

         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic, ST_CENTERTITLE);

         // Copy the SIM message from the SMSApp SIM message object.
         // Load the message label from the resource file.
         (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                    GSM1XSMS_RES_FILE,
                                    IDS_SIM_MSG_LABEL,
                                    szBuf512,
                                    AECHAR_512_BUF_SIZE * sizeof(AECHAR));

         // Append a new line
         WSTRCAT(szBuf512, NewLineStr);

         // Copy the from field to the display text
         if((WSTRLEN(pMe->m_msIn.m_szTitle) > 0) &&
            (WSTRNCMP(pMe->m_msIn.m_szTitle, pMe->m_msIn.m_szMessage,
                      sizeof(pMe->m_msIn.m_szTitle)/sizeof(AECHAR))))
         {
            // There's a title and the title is not derived from the message,
            //  which means the title is either a contact name or a number
            (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                       GSM1XSMS_RES_FILE,
                                       IDS_IN_FROM, 
                                       szBuf64, 
                                       AECHAR_64_BUF_SIZE * sizeof(AECHAR));

            // Append the FROM information
            WSPRINTF(szBuf512, 
                     AECHAR_512_BUF_SIZE * sizeof(AECHAR), 
                     szBuf64, 
                     pMe->m_msIn.m_szTitle);
            
            // Append a new line
            WSTRCAT(szBuf512, NewLineStr);
         }

         // Append the message text to the body.
         if((WSTRLEN(szBuf512) + WSTRLEN(pMe->m_msIn.m_szMessage)) < 
            AECHAR_512_BUF_SIZE)
         {
            WSTRCAT(szBuf512, pMe->m_msIn.m_szMessage);
         }

         // Append date to end of message body.
         GSM1xSMSUtil_SecsToDateString(pMe, 
                                  pMe->m_msIn.m_dwTimeStamp,
                                  szBuf64, 
                                  AECHAR_64_BUF_SIZE * sizeof(AECHAR));
         if((WSTRLEN(szBuf512) + WSTRLEN(szBuf64)) < 
            AECHAR_512_BUF_SIZE_MINUS_ONE)
         {
            WSTRCAT(szBuf512, NewLineStr);
            WSTRCAT(szBuf512, szBuf64);
         }

         // Show the message body.
         // Want the PAGE/MESSAGE/... header to scroll too.
         (void)ISTATIC_SetText(pMe->m_msgBox_IStatic, 
                               NULL,
                               szBuf512,
                               AEE_FONT_NORMAL, 
                               AEE_FONT_NORMAL);
         // We don't need the buffers anymore. Release the memory right away.
         FREE(szBuf64);
         FREE(szBuf512);
         
         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;                   

      case EVT_KEY_PRESS:
         // We are using the EVT_KEY_PRESS event here instead of
         // the normal EVT_KEY event.  This is because in BREW 2.0
         // when an IMenuCtl is the active control in a dialog it will
         // always handle the UP/DOWN EVT_KEY events, so those events
         // will never be sent to the applet.
         if ((AVK_UP   == (AVKType) wParam) ||
             (AVK_DOWN == (AVKType) wParam) ) {
            return ISTATIC_HandleEvent(pMe->m_msgBox_IStatic, EVT_KEY, 
                                       wParam, dwParam);
         }
         break;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // See what user wants to do from here. Based on the users action set
         // the dlgResult attribute of the SMSApp structure.
         switch(wParam)
         {
            case SK_SIM_MSG_NEXT:
               CLOSE_DIALOG(DLGRET_SIM_NEXT)
               return TRUE;

            case SK_SIM_MSG_ERASE:
               CLOSE_DIALOG(DLGRET_SIM_ERASE)
               return TRUE;

            case SK_SIM_MSG_SEND:
               CLOSE_DIALOG(DLGRET_SIM_SENDER)
               return TRUE;

            case SK_SIM_MSG_MOVE:
               CLOSE_DIALOG(DLGRET_SIM_MOVEIN)
               return TRUE;

            case SK_SIM_MSG_FORWARD:
               CLOSE_DIALOG(DLGRET_SIM_FORWARD)
               return TRUE;

			case SK_SIM_MSG_REPLY:
			   CLOSE_DIALOG(DLGRET_SIM_REPLY)
			   return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleSIMMsgBoxDialogEvent

/*=============================================================================
FUNCTION: HandleMsgMoveDialogEvent

DESCRIPTION:
   Dialog handler to help the user move a message from the Inbox -> SIM and 
   SIM box -> Inbox.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

Generally we don't do any state specific or the SMS specific tasks in the 
dialog handlers. As per the common definition of the dialog handlers (within the
SMS applet), they are supposed to be used to show various screens to the user 
and set the app object data based on the user settings. The state handlers are
the ones looks at the object data and makes the decisions.

But to ease the design, it is desided that we will show two screens in this 
dialog. One screen is to show a status update information to the user saying 
that the message moveoperation is in progress. And the other screen is to give
the user a final update (based on whether the operation is succeed or not).

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleMsgMoveDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   AECHAR  szText[MSGBOX_MAXTEXTLEN];

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
            return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;

         SETAEERECT(&rc,0,0,pMe->m_deviceInfo.cxScreen,
                    pMe->m_deviceInfo.cyScreen);


         // Set the static box text based on whether the user wants to 
         // move a message from the Inbox -> SIM or from the SIM -> Inbox.
         if(pMe->m_bMoveToSIM)
         {
            // Move operation is from the Inbox -> SIM
            pMe->m_msgBox_textId = IDS_MOVING_TO_SIM;
         }
         else
         {
            // Move operation is from the SIM box -> Inbox
            pMe->m_msgBox_textId = IDS_MOVING_TO_INBOX;
         }

         (void) ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                     pMe->m_msgBox_textId, szText,
                                     sizeof(szText));

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                               ST_CENTERTEXT | ST_MIDDLETEXT);

         (void) ISTATIC_SetText(pMe->m_msgBox_IStatic, NULL, szText,
                                AEE_FONT_NORMAL, AEE_FONT_NORMAL);

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
      
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);

         IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);

         // We need to start the move operation. Once the operation is complete,
         // we display the status screen to the user.

         if(GSM1xSMSApp_MoveSIMMsg(pMe, pMe->m_bMoveToSIM))
         {
            // The move operation is success. 
            pMe->m_msgBox_textId = IDS_SIM_MOVED;
         }
         else
         {
            // For some reason, the move operation has failed. Let the user 
            // know about it.
            pMe->m_msgBox_textId = IDS_SIM_MOVE_FAILED;
         }
         (void) ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                     pMe->m_msgBox_textId, szText,
                                     sizeof(szText));
         (void) ISTATIC_SetText(pMe->m_msgBox_IStatic, NULL, szText,
                                AEE_FONT_NORMAL, AEE_FONT_NORMAL);
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);

         (void) ISHELL_SetTimer(pMe->a.m_pIShell, MSGBOX_TIMEOUT,
                                DialogTimeoutCallback, pMe);

         return TRUE;

      case EVT_DIALOG_END:
         (void) ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);

         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_OK)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_DIALOG_TIMEOUT:
         CLOSE_DIALOG(DLGRET_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // HandleMsgMoveDialogEvent()

/*=============================================================================
FUNCTION: HandleOKDialogEvent

DESCRIPTION:
   Dialog handler shared by many SMS states to show a dialog with static control
   and only one softkey (OK). Mainly used to show warning messages to the user.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleOKDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;
         AECHAR  szText[MSGBOX_MAXTEXTLEN];
         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                        SK_OK);
         if(NULL == menu)
         {
            return FALSE;
         }

         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                     GSM1XSMS_RES_FILE,
                                     pMe->m_msgBox_textId,
                                     szText,
                                     sizeof(szText));

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                               ST_CENTERTEXT | ST_MIDDLETEXT);

         (void) ISTATIC_SetText(pMe->m_msgBox_IStatic,
                                NULL,
                                szText,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY_PRESS:
         // We are using the EVT_KEY_PRESS event here instead of
         // the normal EVT_KEY event.  This is because in BREW 2.0
         // when an IMenuCtl is the active control in a dialog it will
         // always handle the UP/DOWN EVT_KEY events, so those events
         // will never be sent to the applet.
         if ((AVK_UP   == (AVKType) wParam) ||
             (AVK_DOWN == (AVKType) wParam) ) {
            return ISTATIC_HandleEvent(pMe->m_msgBox_IStatic, EVT_KEY, 
                                       wParam, dwParam);
         }
         break;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;
            
            default:
               break;
         }
         break;

      case EVT_COMMAND:
         switch(wParam)
         {
            case SK_MENUITEM_OK:
               CLOSE_DIALOG(DLGRET_OK)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleOKDialogEvent

/*=============================================================================
FUNCTION: HandleNumMsgReceivedDialogEvent

DESCRIPTION:
   Dialog handler to create a dialog that helps user to see the number of 
   messages received.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleNumMsgReceivedDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;
         AECHAR szFormat[AECHAR_64_BUF_SIZE];
         AECHAR szText[AECHAR_64_BUF_SIZE];
         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                        SK_NUM_MSG_RCVD);
         if(NULL == menu)
         {
            return FALSE;
         }

         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                     GSM1XSMS_RES_FILE,
                                     IDS_RECEIVING_MESSAGES_NUMRECEIVED,
                                     szFormat,
                                     sizeof(szText));


         WSPRINTF(szText, sizeof(szText), szFormat, pMe->m_wNumReceived);

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                               ST_CENTERTEXT | ST_MIDDLETEXT);

         (void) ISTATIC_SetText(pMe->m_msgBox_IStatic,
                                NULL,
                                szText,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         switch(wParam)
         {
            case SK_MENUITEM_NUM_MSG_RECEIVED_OK:
               CLOSE_DIALOG(DLGRET_OK)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleNumMsgReceivedDialogEvent

/*=============================================================================
FUNCTION: HandleSentReceiveStatusDialogEvent

DESCRIPTION:
   Dialog handler to create a dialog that helps the user to see the status 
   of the send/receive operation. If one is in progress then it displays a
   screen giving the same information and also an option to end an incoming/
   outgoing call.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleSentReceiveStatusDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;
         AECHAR  szText[MSGBOX_MAXTEXTLEN];
         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                    SK_SENT_RECEIVE_STATUS);
         if(NULL == menu)
         {
            return FALSE;
         }

         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         if(pMe->m_bSendingMessage)
         {
            (void) ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                        IDS_SENDING,
                                        szText, sizeof(szText));
         }
         else
         {
            (void) ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                                        IDS_RECEIVING_MESSAGES,
                                        szText, sizeof(szText));
         }

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                               ST_CENTERTEXT | ST_MIDDLETEXT);

         (void) ISTATIC_SetText(pMe->m_msgBox_IStatic,
                                NULL,
                                szText,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         switch(wParam)
         {
            case SK_MENUITEM_SENT_RECEIVE_STATUS_END:
               CLOSE_DIALOG(DLGRET_END)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleSentReceiveStatusDialogEvent

/*=============================================================================
FUNCTION: HandleCreateMsgNumberDialogEvent

DESCRIPTION:
   Dialog handler to create a dialog that helps the user to enter a new number
   for the outgoing message.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleCreateMsgNumberDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)

   ITextCtl *textCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_activeDlg, 
                                                     CREATE_MSG_NUMBER);

   if(NULL == textCtl)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         CtlAddItem ai;
         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                        SK_CREATE_MSG);
         if (menu == NULL) 
         {
            return FALSE;
         }

         // Build the softkey menu manually.
         (void) IMENUCTL_DeleteAll(menu);
         (void) IMENUCTL_Redraw(menu);

         MEMSET(&ai, 0, sizeof(ai));
         ai.pszResText = GSM1XSMS_RES_FILE;
         ai.wText = IDS_ST_ADD_CONT;
         ai.wItemID = SK_CREATE_MSG_CONTINUE;
         (void) IMENUCTL_AddItemEx(menu, &ai);
         
         ai.wText = IDS_ST_CONTACTS;
         ai.wItemID = SK_CREATE_MSG_CONTACTS;
         (void) IMENUCTL_AddItemEx(menu, &ai);

         ITEXTCTL_SetSoftKeyMenu(textCtl, menu);

         // Set the title of the screen.
         (void) ITEXTCTL_SetTitle(textCtl,GSM1XSMS_RES_FILE,
                                  pMe->m_nSendToTitleResId, NULL);

         ITEXTCTL_SetProperties(textCtl, TP_FRAME);

         // Limit the user to enter only MAX_TO_CHARS chars.
         ITEXTCTL_SetMaxSize(textCtl, MAX_TO_CHARS);

         (void) ITEXTCTL_SetInputMode(textCtl, AEE_TM_NUMBERS);

         // Set the contents to the text control.
         (void)ITEXTCTL_SetText(textCtl, pMe->m_msOut.m_szTo, -1);

         return TRUE;
      }

      case EVT_DIALOG_END:
         // Theoratically speaking we don't have to extract the text from the 
         // text control as it is already done in EVT_COMMAND handler. But
         // there are some scenarios where this dialog is closed to bring up
         // an incoming message screen. And in those circumstances, we don't 
         // want to loose all the data that was already entered by the user.
         // And more over there is no harm in extracting the text and 
         // overwriting the m_msOut.m_szTo field :)
         (void)ITEXTCTL_GetText(textCtl, pMe->m_msOut.m_szTo, MAX_TO_CHARS);
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // Set the preference.
         switch(wParam)
         {
            case SK_CREATE_MSG_CONTINUE:
               // Extract the number from the text control and update the 
               // object out message structure parameters.

               // Get the text from the text control.
               (void)ITEXTCTL_GetText(textCtl, pMe->m_msOut.m_szTo,
                                      MAX_TO_CHARS);
               CLOSE_DIALOG(DLGRET_CM_CONTINUE)
               return TRUE;

            case SK_CREATE_MSG_CONTACTS:
               CLOSE_DIALOG(DLGRET_CM_CONTACTS)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleCreateMsgNumberDialogEvent

/*=============================================================================
FUNCTION: HandleCreateMsgTextDialogEvent

DESCRIPTION:
   Dialog handler to create a dialog that helps the user to enter message text
   for the outgoing message.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleCreateMsgTextDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)

   ITextCtl *textCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_activeDlg, 
                                                     CREATE_MSG_TEXT);

   if(NULL == textCtl)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEEDeviceInfo di;
         AEERect       rc;
         uint8         maxSize = 0;
         CtlAddItem ai;

         // fools brew into allowing three lines in english
         AECHAR temptitle[2]={' ', '\0'};
         
         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                        SK_CM_TEXT);
         if (menu == NULL) 
         {
            return FALSE;
         }

         maxSize = 140; // Max SMS_Submit UD is 140

         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &di);

         // Build the softkey menu manually.
         (void) IMENUCTL_DeleteAll(menu);
         (void) IMENUCTL_Redraw(menu);

         MEMSET(&ai, 0, sizeof(ai));
         ai.pszResText = GSM1XSMS_RES_FILE;
         ai.wItemID = SK_MENUITEM_CM_CONTINUE;
         ai.wText = IDS_ST_ADD_CONT;
         (void) IMENUCTL_AddItemEx(menu, &ai);

         // Update the softkey menu to make the "Multitap" sofkey to appear
         // at the end of the menu.
         ITEXTCTL_SetSoftKeyMenu(textCtl, menu);

         // The title is over written by OEM_DrawText.
         // So this just fools brew into allow three lines in english.
         (void) ITEXTCTL_SetTitle(textCtl, NULL, 0, temptitle);

         // Set the text control to be framed and multi-line.
         ITEXTCTL_SetProperties(textCtl,TP_FRAME|TP_MULTILINE|TP_DISPLAY_COUNT);

         {
            int input_mode;

            input_mode = AEE_TM_LETTERS;
            ITEXTCTL_SetMaxSize(textCtl, maxSize);
            (void) ITEXTCTL_SetInputMode(textCtl, input_mode);
         }

         // Make the text control object to occupy the entire screen (leaving
         // space for the menu)
         ITEXTCTL_SetRect(textCtl, &rc);
         
         // Set the default text for the text edit control.
         (void)ITEXTCTL_SetText(textCtl,pMe->m_msOut.m_szMessage,-1);

         return TRUE;
      }

      case EVT_DIALOG_END:
         // Theoratically speaking we don't have to extract the text from the 
         // text control as it is already done in EVT_COMMAND handler. But
         // there are some scenarios where this dialog is closed to bring up
         // an incoming message screen. And in those circumstances, we don't 
         // want to loose all the data that was already entered by the user.
         // And more over there is no harm in extracting the text and 
         // overwriting the m_msOut.m_szMessage field :)
         (void)ITEXTCTL_GetText(textCtl, pMe->m_msOut.m_szMessage, MAX_MSG_LEN);
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         switch(wParam)
         {
            case SK_MENUITEM_CM_CONTINUE:
               // Extract the number from the text control and update the 
               // object out message structure parameters.

               // Get the text from the message body.
               (void)ITEXTCTL_GetText(textCtl, pMe->m_msOut.m_szMessage,
                                      MAX_MSG_LEN);
               CLOSE_DIALOG(DLGRET_CM_CONTINUE)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;

} // HandleCreateMsgTextDialogEvent

/*=============================================================================
FUNCTION: HandleSendOptionsDialogEvent

DESCRIPTION:
   Dialog handler to create a dialog that prompts the user to either save or
   send the newly created message.

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleSendOptionsDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      case EVT_DIALOG_START:
      case EVT_DIALOG_END:
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         // See what user wants to do with this new outgoing message?
         // Set the dlgResult field of the SMSApp structure accordingly.
         switch(wParam)
         {
            case MENUITEM_SEND_OPTS_SEND:
               CLOSE_DIALOG(DLGRET_CM_SEND)
               return TRUE;

            case MENUITEM_SEND_OPTS_SAVE:
               CLOSE_DIALOG(DLGRET_CM_SAVE)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleSendOptionsDialogEvent

/*=============================================================================
FUNCTION: HandleEraseOldMsgDialogEvent

DESCRIPTION:
   Dialog handler to show a warning message screen to the user with an option
   to erase old messages. 

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleEraseOldMsgDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
      {
         AEERect rc;
         AECHAR  szText[MSGBOX_MAXTEXTLEN];
         IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                        SK_ERASE_OLD_MSGS);
         if(NULL == menu)
         {
            return FALSE;
         }

         IMENUCTL_GetRect(menu, &rc);

         rc.dy = rc.y;
         rc.dx = (int16)pMe->m_rc.dx;
         rc.y = pMe->m_rc.y;
         rc.x = pMe->m_rc.x;

         (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                     GSM1XSMS_RES_FILE,
                                     pMe->m_msgBox_textId,
                                     szText,
                                     sizeof(szText));

         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                               (ST_CENTERTEXT | ST_MIDDLETEXT));

         (void) ISTATIC_SetText(pMe->m_msgBox_IStatic,
                                NULL,
                                szText,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);

         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY_PRESS:
         // We are using the EVT_KEY_PRESS event here instead of
         // the normal EVT_KEY event.  This is because in BREW 2.0
         // when an IMenuCtl is the active control in a dialog it will
         // always handle the UP/DOWN EVT_KEY events, so those events
         // will never be sent to the applet.
         if ((AVK_UP   == (AVKType) wParam) ||
             (AVK_DOWN == (AVKType) wParam) ) {
            return ISTATIC_HandleEvent(pMe->m_msgBox_IStatic, EVT_KEY, 
                                       wParam, dwParam);
         }
         break;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         switch(wParam)
         {
            case SK_ERASE_OLD_MSGS_OK:
               CLOSE_DIALOG(DLGRET_OK)
               return TRUE;
            
            case SK_ERASE_OLD_MSGS_ERASE:
               CLOSE_DIALOG(DLGRET_ERASE_OLD_MSGS)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleEraseOldMsgDialogEvent


/*=============================================================================
FUNCTION: HandleSendPreformatDialogEvent

DESCRIPTION:
   Dialog handler to show a list of preformed messages to the uer. 

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleSendPreformatDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   IMenuCtl *menu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_activeDlg,
                                                  MENU_SEND_PREFORMAT);
   if (menu == NULL) 
   {
      return FALSE;
   }
   
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         IMENUCTL_SetSel(menu, pMe->m_prevMenuItemSel);
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;
            
            default:
               break;
         }
         break;

      case EVT_COMMAND:
      {
         CtlAddItem cai;
         pMe->m_prevMenuItemSel = IMENUCTL_GetSel(menu);
         (void) IMENUCTL_GetItem(menu, wParam, &cai);
         
         // Reset the m_msOut fields.
         pMe->m_msOut.m_szTo[0] = (AECHAR)'\0';
         pMe->m_msOut.m_szMessage[0] = (AECHAR)'\0';

         // No need to set any default number. Just update the message field 
         // with the user selected preformed message.
         WSTRNCOPYN(pMe->m_msOut.m_szMessage,
                    sizeof(pMe->m_msOut.m_szMessage)/sizeof(AECHAR),
                    cai.pText, 
                    -1);

         // We are done here. Return to the state handler by closing this dialog
         CLOSE_DIALOG(DLGRET_OK)
         return TRUE;
      }

      default:
         break;
   }

   return FALSE;
} // HandleSendPreformatDialogEvent

/*=============================================================================
FUNCTION: HandleSendFailureDialogEvent

DESCRIPTION:
   This dialog handler is to create a dialog with only a static control (no
   softkeys) in it. This is shared by many SMS states to give status updates
   and warning/informative messages to the user. 
   
   After the dialog is created, it starts a timer and when the timer expires
   it closes the dialog (in case if the dialog is not yet closed by the user)

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleSendFailureDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(wParam)

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      {
         AEERect rc;
         AECHAR* szFinal = NULL;
         uint16 errorResId = IDS_MSG_SEND_FAILED;

         szFinal = (AECHAR *) MALLOC(AECHAR_128_BUF_SIZE * sizeof(AECHAR));
         if(szFinal == NULL)
         {
            // We can not continue here without allocating any memory for the
            // main buffer.
            DBGPRINTF("Fatal Memory Allocation Failure");
            return FALSE;
         }

         SETAEERECT(&rc,0,0,pMe->m_deviceInfo.cxScreen, 
                    pMe->m_deviceInfo.cyScreen);

         // Choose the error string (to be displayed) depending upon the error 
         // type.
         switch(pMe->m_wSendFailureStatus)
         {
            case SMS_ADDRESS_TRANSLATION_FAILURE_S:
               // Unknown destination address.
               errorResId = IDS_MSG_SEND_FAIL1;
               break;

            case SMS_SMS_ORIGINATION_DENIED_S:
               // The mobile station originated SMS feature is not activated 
               // in the network.
               errorResId = IDS_MSG_SEND_FAIL2;
               break;

            case SMS_SMS_NOT_SUPPORTED_S:
               // The SMS feature is not supported by the base station.
               errorResId = IDS_MSG_SEND_FAIL3;
               break;

            case SMS_NOT_GSM1X_MODE_FAILURE:
               // GSM1x mode is not active
               errorResId = IDS_MSG_GSM1X_MODE_NOT_ACTIVE;
               break;

            case SMS_SUBMIT_REPORT_FAILURE:
               // The SMS Submit report was received so show TP-FCS info
               errorResId = pMe->m_wSubmitReportFailure;
               break;

            default:
               errorResId = IDS_MSG_SEND_FAIL4;
               break;
         }

         // Load the resource string based on the errorResId.
         (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                    GSM1XSMS_RES_FILE,
                                    errorResId,
                                    szFinal, 
                                    AECHAR_128_BUF_SIZE * sizeof(AECHAR));
         
         // Create a static control in the dialog.
         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_STATIC,
                                      (void**)&pMe->m_msgBox_IStatic);

         ISTATIC_SetRect(pMe->m_msgBox_IStatic, &rc);

         ISTATIC_SetProperties(pMe->m_msgBox_IStatic,
                               (ST_CENTERTEXT | ST_MIDDLETEXT));

         (void) ISTATIC_SetText(pMe->m_msgBox_IStatic,
                                NULL,
                                szFinal,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);

         // Free up the memory...
         if(szFinal)
         {
            FREE(szFinal);
         }

         // Always start a timer for the message box. 
         (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                                SEND_FAILURE_MSG_TIMEOUT,
                                DialogTimeoutCallback,
                                pMe);
         return TRUE;
      }

      case EVT_DIALOG_START:
         // KLUDGE!  Need to wait for the Dialog to draw itself before
         //          the ISTATIC_Redraw() command can be issued or the
         //          Dialog will overwrite the IStatic control
         (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_APP_GSM1XSMS,
                                 EVT_WAIT_FOR_REDRAW, 0, 0);
         return TRUE;

      case EVT_WAIT_FOR_REDRAW:
         (void) ISTATIC_Redraw(pMe->m_msgBox_IStatic);
         return TRUE;

      case EVT_DIALOG_END:
         (void) ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);

         if(NULL != pMe->m_msgBox_IStatic)
         {
            ISTATIC_Reset(pMe->m_msgBox_IStatic);
            (void) ISTATIC_Release(pMe->m_msgBox_IStatic);
            pMe->m_msgBox_IStatic = NULL;
         }
         return TRUE;

      case EVT_KEY:
      case EVT_DIALOG_TIMEOUT:
         // If the timer expires then close the dialog.
         CLOSE_DIALOG(DLGRET_MSGBOX_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
} // HandleSendFailureDialogEvent()

/*=============================================================================
FUNCTION: HandleEditSCAddrDialogEvent

DESCRIPTION:
   Dialog handler to show a dialog which allows the user to edit the default 
   SC address 

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleEditSCAddrDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)
   boolean retVal = FALSE;

   ITextCtl *textCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_activeDlg, 
                                                     CREATE_SC_ADDR_NUMBER);

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         retVal = TRUE;
         break;

      case EVT_DIALOG_START:
      {
         (void)ITEXTCTL_SetText(textCtl, &pMe->m_szSCAddr[0], MAX_TO_CHARS);

         ITEXTCTL_SetProperties(textCtl, TP_FRAME);

         // Limit the user to enter only MAX_TO_CHARS chars.
         ITEXTCTL_SetMaxSize(textCtl, MAX_TO_CHARS);

         (void) ITEXTCTL_SetInputMode(textCtl, AEE_TM_NUMBERS);

         return TRUE;
      }

      case EVT_DIALOG_END:
         // Theoratically speaking we don't have to extract the text from the 
         // text control as it is already done in EVT_COMMAND handler. But
         // there are some scenarios where this dialog is closed to bring up
         // an incoming message screen. And in those circumstances, we don't 
         // want to loose all the data that was already entered by the user.
         // And more over there is no harm in extracting the text and 
         // overwriting the m_msOut.m_szTo field :)
         (void)ITEXTCTL_GetText(textCtl, &pMe->m_szSCAddr[0], MAX_TO_CHARS);
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               retVal = TRUE;
               break;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         switch(wParam)
         {
            case SK_EDIT_SC_ADDR_CONTINUE:
               // Get the text from the message body.
               (void)ITEXTCTL_GetText(textCtl, &pMe->m_szSCAddr[0], MAX_TO_CHARS);

               CLOSE_DIALOG(DLGRET_EDIT_SC_ADDR_CONTINUE)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
         }
         break;

      default:
         break;
   }

   return retVal;
} // HandleEditSCAddrDialogEvent()

/*=============================================================================
FUNCTION: HandleTestDialogEvent

DESCRIPTION:
   Dialog handler to unit test the SMS App

PARAMETERS:
   pMe [in]: Pointer to the SMS Applet structure. This structure contains
            information specific to this applet.
            
   ecode [in]: Specifies the Event sent to this applet
   
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE:    If the dialog handler has processed the event.
   FALSE:   If the dialog handler did not process the event.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean HandleTestDialogEvent
(
   SMSApp   *pMe,
   AEEEvent eCode,
   uint16   wParam,
   uint32   dwParam
)
{
   PARAM_NOT_REF(dwParam)

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      case EVT_DIALOG_START:
      case EVT_DIALOG_END:
         return TRUE;

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               CLOSE_DIALOG(DLGRET_CANCELED)
               return TRUE;

            default:
               break;
         }
         break;
		 
	   case EVT_COMMAND:
         // Check what menu item the user selected and set the dlgResult 
         // accordingly.
         switch(wParam)
         {
            case TEST_MENUITEM_CLASS0:
               CLOSE_DIALOG(DLGRET_TEST_CLASS0)
               return TRUE;

            case TEST_MENUITEM_CLASS1:
               CLOSE_DIALOG(DLGRET_TEST_CLASS1)
               return TRUE;

            case TEST_MENUITEM_CLASS2:
               CLOSE_DIALOG(DLGRET_TEST_CLASS2)
               return TRUE;

            case TEST_MENUITEM_CLASS_NONE:
               CLOSE_DIALOG(DLGRET_TEST_CLASS_NONE)
               return TRUE;

            case TEST_MENUITEM_VM_PID:
               CLOSE_DIALOG(DLGRET_TEST_VM_PID)
               return TRUE;

            case TEST_MENUITEM_VM_DCS_STORE:
               CLOSE_DIALOG(DLGRET_TEST_VM_DCS_STORE)
               return TRUE;

            case TEST_MENUITEM_VM_DCS_DISCARD:
               CLOSE_DIALOG(DLGRET_TEST_VM_DCS_DISCARD)
               return TRUE;

            case TEST_MENUITEM_VM_UDH:
               CLOSE_DIALOG(DLGRET_TEST_VM_UDH)
               return TRUE;

            default:
               ASSERT_NOT_REACHABLE;
               break;
         }
         break;

      default:
         break;
   }

   return FALSE;
} // HandleTestDialogEvent()

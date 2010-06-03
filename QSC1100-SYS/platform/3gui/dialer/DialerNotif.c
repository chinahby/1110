/*=============================================================================

FILE: DialerNotif.c

SERVICES: Dialer Applet Dialogs

GENERAL DESCRIPTION: Notif Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerNotif.c#31 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/26/08   cvs     Remove deprecated functions
03/19/08   sg      Modify all dialogs to use the new framework
05/20/04   SUN     Fixed Incorrect Error Check
04/05/04   BPW     Alert changes for failed calls
01/29/04   BPW     Changed IShell calls to GETUPTIMEMS()
01/07/04   BPW     Cancel max display timer after up or down key press
12/04/03   BPW     Added code review commnents and changes
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/25/03   BPW     Linted for the Saber 6250 build
08/29/03   BW      Fixed compile warnings
08/29/03   BW      Added own buffer size constant
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
07/29/03   BW      Removed lint comments and bid include
07/13/03   BW      Created DialerNotif Source File 

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
#include "DialerNotif.h"
#include "DialerUtils.h"
#include "dialerapp_res.h"
#include "AppComFunc.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static void     CDialerApp_NotifDlg_AddMsg(CDialerApp *pMe, AECHAR *pszMsg, uint32 dwMinTime, uint32 dwMaxTime);
static void     CDialerApp_NotifDlg_RemoveMsg(CDialerApp *pMe);
static boolean  CDialerApp_NotifDlg_FormatMsg(AECHAR *pszMsg, AECHAR *pszFormatMsg, int nFormatMsgSize);

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_KeyHandler

DESCRIPTION: Handles Notif Dialog's key events

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
boolean CDialerApp_NotifDlg_KeyHandler(CDialerApp *pMe, AEEEvent evt, uint16 key)
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
     switch(key)
     {
     case AVK_UP:
     case AVK_DOWN:
       // cancel timer so they have a chance to read notifs
       (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) CDialerApp_NotifDlg_RemoveMsg, (void*) pMe);
       break;

     default:
       break;
     }
     // let main event handler get this event too
     return FALSE;

   case EVT_KEY:
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
     case AVK_SELECT:
     case AVK_SEND:
     case AVK_CLR:
     case AVK_MESSAGE:  
     case AVK_MENU:
     case AVK_MUTE:
       // these key events will make the dialog goes away and also cancel the timer
       (void) CDialerApp_EndDlg(pMe);
       //let main event handler handle event
       return FALSE;

     default:
       return FALSE; 
     }
   }

   return FALSE;
}

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_CmdHandler

DESCRIPTION: Handles Notif Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NotifDlg_CmdHandler(CDialerApp *pMe, uint16 ctrl_id)
{
   PRINT_FUNCTION_NAME();
   // error checking
   if (pMe == NULL) 
   { 
     return FALSE;
   }

   switch(ctrl_id)
   {
   case IDL_SK_NOTIF_CANCEL:
     // cancel was pressed so end dialog
     return CDialerApp_EndDlg(pMe);

   default:
     return FALSE;
   }
}

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_Init

DESCRIPTION: 
  Initializes all of the dialogs controls.

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NotifDlg_Init(CDialerApp *pMe)
{
  IDialog	*pActiveDlg;
  IStatic *pIStatic;
  IMenuCtl *pISoftKeyMenu; 
  AEERect StaticRect;
  AEERect MenuRect;
  boolean bReturnStatus;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    return FALSE;
  }

  if(IVector_IsEmpty(pMe->m_pNotifMsgQueue) == TRUE)
  {
    DIALER_ERR("Empty Queue", 0,0,0);
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
    IDC_STATIC_NOTIF);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pActiveDlg, 
    IDC_SK_NOTIF);
  if((pIStatic == NULL) || (pISoftKeyMenu == NULL)) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // Update the softkey menu
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, DIALERAPP_RES_FILE, IDS_CANCEL,
    IDL_SK_NOTIF_CANCEL, NULL, 0);
  SetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
  IMENUCTL_SetActive(pISoftKeyMenu, TRUE);
  bReturnStatus = IMENUCTL_Redraw(pISoftKeyMenu);
  if (bReturnStatus == FALSE) 
  {
    return FALSE;
  }

  // Set Static control size 
  ISTATIC_GetRect(pIStatic, &StaticRect);
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);
  SETAEERECT(&StaticRect, 10, 10, pMe->m_rc.dx-20, pMe->m_rc.dy-MenuRect.dy-20);          

  // Static Properties
  ISTATIC_SetRect(pIStatic, &StaticRect);
  ISTATIC_SetProperties(pIStatic, ST_ENABLE_HLGHT|ST_ENABLETAB );  
  (void) ISTATIC_SetText(pIStatic, NULL, NULL, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
  ISTATIC_SetActive(pIStatic, FALSE);

  (void) IDIALOG_SetFocus(pActiveDlg, IDC_SK_NOTIF);

  pMe->m_dwNotifStartTime = GETUPTIMEMS();
  (void) ISHELL_SetTimer(pMe->a.m_pIShell, 1000, 
    (PFNNOTIFY) CDialerApp_NotifDlg_RemoveMsg, (void*) pMe);
  (void) CDialerApp_NotifDlg_DisplayRefresh(pMe);
  return TRUE;
} 

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_DisplayRefresh

DESCRIPTION:  
  Redraws the text that is in the static control which is stored in the queue.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE: 
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NotifDlg_DisplayRefresh(void *pUser)
{
  CDialerApp *pMe = (CDialerApp*) pUser;
  IDialog	*pActiveDlg = NULL;
  IStatic *pIStatic = NULL;
  CMsgQueueItem *pItem = NULL;
  boolean bAppend;
  uint32 dwCurrentTime;
  int i;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  pActiveDlg = CDialerApp_GetActiveDlg(pMe);
  if(pActiveDlg == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_NOTIF)
  {
    DIALER_ERR("Refresh non Notif dlg", 0,0,0);
    return FALSE;
  }

  if(IVector_IsEmpty(pMe->m_pNotifMsgQueue) == TRUE)
  {
     DIALER_ERR("Empty Queue", 0,0,0);
     return FALSE;
  } 

  // Get controls in dialog
  pIStatic = (IStatic*) IDIALOG_GetControl(pActiveDlg, IDC_STATIC_NOTIF);
  if(pIStatic == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE; // error 
  }

  // build string
  bAppend = FALSE;
  dwCurrentTime = GETUPTIMEMS();
  for(i=0; i<(int)IVector_Size(pMe->m_pNotifMsgQueue); i++)
  {
    pItem = IVector_ElementAt(pMe->m_pNotifMsgQueue, (uint32)i);

    (void) ISTATIC_SetTextEx(pIStatic, (byte *) pItem->pszStr, NULL, bAppend);
    bAppend = TRUE;

    if(pItem->dwExpireTime == 0)
    {
      // set expire time since it is being diplayed for the first time
      pItem->dwExpireTime = (pItem->dwMaxDisplaySecs*1000) + dwCurrentTime;
    }
  }

  (void) ISTATIC_Redraw(pIStatic);

  return TRUE;
}


/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_EndHandler

DESCRIPTION: Does any cleanup needed during for the end of the notif dialog.

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE: 
  Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NotifDlg_EndHandler(CDialerApp *pMe)
{
  int i;
  CMsgQueueItem *pItem;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    return FALSE;
  }
  
  // remove all elements in the queue
  for(i=(int)(IVector_Size(pMe->m_pNotifMsgQueue)-1); i>=0; i--)
  {
    // free the memory
    pItem = IVector_ElementAt(pMe->m_pNotifMsgQueue, (uint32)i);
    FREEIF(pItem->pszStr);
    FREEIF(pItem);
  }
  IVector_RemoveAllElements(pMe->m_pNotifMsgQueue);

  (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) 
    CDialerApp_NotifDlg_RemoveMsg,(void*) pMe); 
  (void) ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) 
    CDialerApp_NotifDlg_CloseDlg,(void*) pMe); 
  pMe->m_dwNotifMinTime = 0;
  pMe->m_dwNotifStartTime = 0;

  // stop playing the call failed alert since the notif is going away
  CDialerApp_StopCallFailedSound(pMe);

  return TRUE;
} 

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_CloseDlg

DESCRIPTION:  
  Close notif dialog after the min time to live has expired.

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE: 
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:
  A timer will be set to check the min time to live.  If that time has
  expired the dialog will be closed.  Otherwise the timer will reset.

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NotifDlg_CloseDlg(void *pUser)
{
  CDialerApp* pMe = (CDialerApp*) pUser;
  uint32 dwCurrentTime;
  uint32 dwExpireTime;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;
  }

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_NOTIF)
  {
    DIALER_ERR("Active Dlg must be NOTIF to use func", 0,0,0);
    return FALSE;   
  }

  // calculate times
  dwCurrentTime = GETUPTIMEMS();
  dwExpireTime = pMe->m_dwNotifStartTime + pMe->m_dwNotifMinTime;

  if(dwExpireTime > dwCurrentTime)
  {
    // try again to close it later
    (void) ISHELL_SetTimer(pMe->a.m_pIShell, (int32)(dwExpireTime - dwCurrentTime), 
      (PFNNOTIFY) CDialerApp_NotifDlg_CloseDlg, (void*) pMe); 
  }
  else
  {
    return CDialerApp_EndDlg(pMe); 
  }

  return TRUE; 
}

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_DisplayMsg

DESCRIPTION:  
  This function is called by other dialogs or apps to display a notification.

PARAMETERS:
  *pMe: CDialerApp object pointer
  nStringID: strings ID in resource file
  dwMinDisplaySecs: min time to display in second
  dwMaxDisplaySecs: max time to display in second

RETURN VALUE: 
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_NotifDlg_DisplayMsg(void *pUser, 
                                    uint16 wStringID, 
                                    uint32 dwMinDisplaySecs,
                                    uint32 dwMaxDisplaySecs)
{
  CDialerApp* pMe = (CDialerApp*) pUser;
  AECHAR *pszBuf = NULL;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  if(wStringID == 0) 
  {
    DIALER_ERR("Bad Param", 0, 0, 0);
    return;
  }

  pszBuf = MALLOC (DIALERAPP_MAX_NOTIF_BUFFER_SIZE*sizeof(AECHAR));
  if (pszBuf == NULL) 
  {
    DIALER_ERR("No memory", 0,0,0);
    return;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DIALERAPP_RES_FILE, wStringID, pszBuf, 
                      (DIALERAPP_MAX_NOTIF_BUFFER_SIZE*sizeof(AECHAR)));

  CDialerApp_NotifDlg_AddMsg(pMe, pszBuf, dwMinDisplaySecs, dwMaxDisplaySecs);

  FREE(pszBuf);
} 

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_DisplayMsgEx

DESCRIPTION:  
  This function is called by other dialogs or apps to display a notification.

PARAMETERS:
  *pMe: CDialerApp object pointer
  *pszText: pointer to the string to display
  dwMinDisplaySecs: min time to display in second
  dwMaxDisplaySecs: max time to display in second

RETURN VALUE: 
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_NotifDlg_DisplayMsgEx(void *pUser, 
                                      AECHAR *pszText, 
                                      uint32 dwMinDisplaySecs,
                                      uint32 dwMaxDisplaySecs)
{
  CDialerApp* pMe = (CDialerApp*) pUser;

  PRINT_FUNCTION_NAME();
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  if(pszText == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  CDialerApp_NotifDlg_AddMsg(pMe, pszText, dwMinDisplaySecs, dwMaxDisplaySecs);
} 


/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_AddMsg
 
DESCRIPTION:  
  Adds a message to the queue.  It also starts the dialog if it is not already
  running.

PARAMETERS:
  *pMe: CDialerApp object pointer
  *pszMsg: String of the message
  dwMinTime: minimum time to display message
  dwMaxTime: maximum time to display message

RETURN VALUE: 
  NONE

COMMENTS:

SIDE EFFECTS:
  Creates a timer that will remove the message to the queue when its max time
  to live has expired.

SEE ALSO:

=============================================================================*/
static void CDialerApp_NotifDlg_AddMsg(CDialerApp *pMe, 
                                       AECHAR *pszMsg, 
                                       uint32 dwMinTime,
                                       uint32 dwMaxTime)
{
  CMsgQueueItem *pItem = NULL;
  AECHAR *pBuf = NULL;

  PRINT_FUNCTION_NAME();
  // error checking
  if((pMe == NULL) || (pszMsg == NULL))
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  pBuf = (AECHAR *) MALLOC(((uint32)WSTRLEN(pszMsg)+3)*sizeof(AECHAR));
  pItem = (CMsgQueueItem *) MALLOC(sizeof(CMsgQueueItem));
  if((pBuf == NULL) || (pItem == NULL))
  {
    DIALER_ERR("No Memory", 0,0,0);
    FREEIF(pBuf);
    FREEIF(pItem);
    return;
  }

  // format message
  if(CDialerApp_NotifDlg_FormatMsg(pszMsg, pBuf, WSTRLEN(pszMsg)+3) == FALSE)
  {
    DIALER_ERR("Could not format message", 0,0,0);
    return;
  }

  // init the message element
  pItem->dwExpireTime = 0;
  pItem->pszStr = pBuf;
  pItem->dwMaxDisplaySecs = dwMaxTime;

  if(IVector_Size(pMe->m_pNotifMsgQueue) >= DIALERAPP_MAX_NOTIF_MSGS)
  {
    CMsgQueueItem *pItemRemove = IVector_ElementAt(pMe->m_pNotifMsgQueue, 0);

    // remove the oldest element to make room for newest
    if(IVector_RemoveElementAt(pMe->m_pNotifMsgQueue, 0) == NULL)
    {
      DIALER_ERR("Notif queue is full", 0,0,0);
      FREEIF(pBuf);
      FREEIF(pItem);
      return; 
    }

    FREEIF(pItemRemove->pszStr);
    FREEIF(pItemRemove);
  }

  // add message to the queue
  if(CheckAEEReturnStatus(IVector_AddElement(pMe->m_pNotifMsgQueue, pItem)) == FALSE)
  {
    DIALER_ERR("Could not add messege to the Notif queue", 0,0,0);
    FREEIF(pBuf);
    FREEIF(pItem);
    return; 
  }

  // add to the Min Display Time
  if(pMe->m_dwNotifStartTime == 0)
  {
    // notif is not yet being displayed so take the larger of the display times
    if(dwMinTime > pMe->m_dwNotifMinTime)
    {
      pMe->m_dwNotifMinTime = dwMinTime;
    }
  }
  else
  {
    // notif dialog is being displayed so adjust time accordingly comparing previous value
    uint32 dwTempNotifMinTime = (GETUPTIMEMS() - pMe->m_dwNotifStartTime) 
                               + (dwMinTime * 1000);
    if(dwTempNotifMinTime > pMe->m_dwNotifMinTime)
    {
       pMe->m_dwNotifMinTime = dwTempNotifMinTime;
    }
  }

  // create the dialog if it is not already showing and init
  if(CDialerApp_GetActiveDlgID(pMe) != IDD_NOTIF)
  {
    (void) CDialerApp_CreateDlg(pMe, IDD_NOTIF, NULL, 0);
  }

  // redraw the screen
  if(CDialerApp_GetActiveDlgID(pMe) == IDD_NOTIF)
  { 
    (void) CDialerApp_NotifDlg_DisplayRefresh(pMe);
  }

  // do not free pBuf or pItem
}

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_RemoveMsg

DESCRIPTION:  
  If the message at the beginning of the queue has expired, then it will be 
  removed.  Also if the queue is empty after deletion it will close the dialog,
  otherwise it restarts the timer.

PARAMETERS: 
  *pMe: CDialerApp object pointer

RETURN VALUE: 

COMMENTS:

SIDE EFFECTS:
  Creates a timer if queue is not empty that will remove the message to the 
  queue when its max time to live has expired.

SEE ALSO:

=============================================================================*/
static void CDialerApp_NotifDlg_RemoveMsg(CDialerApp *pMe)
{ 
  CMsgQueueItem *pItem = NULL;
  uint32 dwCurrentTime;
  boolean bRedraw;
  int i;

  PRINT_FUNCTION_NAME();
  // error checking
  if (pMe == NULL) 
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return;
  }

  if(CDialerApp_GetActiveDlgID(pMe) != IDD_NOTIF)
  {
    // this timer should only run while notif is being shown
    DIALER_ERR("Non Notif Dialog", 0,0,0);
    return;
  }

  if(IVector_IsEmpty(pMe->m_pNotifMsgQueue) == TRUE)
  {
    DIALER_ERR("Empty Queue", 0,0,0);
    return;
  }

  bRedraw = FALSE;
  dwCurrentTime = GETUPTIMEMS();

  // check to see if we must remove any messages
  for(i=(int)(IVector_Size(pMe->m_pNotifMsgQueue)-1); i>=0; i--)
  {
    pItem = IVector_ElementAt(pMe->m_pNotifMsgQueue, (uint32)i);

    if((pItem->dwExpireTime != 0) && (pItem->dwExpireTime <= dwCurrentTime))
    {
      // remove the message since it has expired
      FREEIF(pItem->pszStr);
      FREEIF(pItem);
      (void) IVector_RemoveElementAt(pMe->m_pNotifMsgQueue, (uint32)i);
      bRedraw = TRUE;
    } 
  }

  if(IVector_IsEmpty(pMe->m_pNotifMsgQueue) == TRUE)
  {
    // nothing left to display so close Dlg and don't reset timer
    (void) CDialerApp_EndDlg(pMe);
    return;  
  }

  if(bRedraw == TRUE)
  {
    (void) CDialerApp_NotifDlg_DisplayRefresh(pMe);
  }

  // reset the timer for remove check
  (void) ISHELL_SetTimer(pMe->a.m_pIShell, 1000, 
      (PFNNOTIFY) CDialerApp_NotifDlg_RemoveMsg, (void*) pMe);
}


/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_FormatMsg

DESCRIPTION:  

PARAMETERS: 
  *pszMsg: string to format
 *pszFormatMsg: pointer to formatted string

RETURN VALUE: 
  boolean: returns true if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CDialerApp_NotifDlg_FormatMsg(AECHAR *pszMsg, AECHAR *pszFormatMsg, int nFormatMsgSize)
{
  AECHAR szBullet[] = {'-', '\0'};
  AECHAR szNewLine[] = {'\n', '\0'};

  PRINT_FUNCTION_NAME();
  if((pszMsg == NULL) || (pszFormatMsg == NULL))
  {
    DIALER_ERR("Null pointer", 0,0,0);
    return FALSE;  
  }

  // format message
  (void) WSTRLCAT(pszFormatMsg, szBullet, nFormatMsgSize);
  (void) WSTRLCAT(pszFormatMsg, pszMsg, nFormatMsgSize);
  (void) WSTRLCAT(pszFormatMsg, szNewLine, nFormatMsgSize);

  return TRUE;
}
//lint -restore 

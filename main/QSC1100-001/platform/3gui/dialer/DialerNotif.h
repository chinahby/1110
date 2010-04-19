#ifndef DIALERNOTIF_H
#define DIALERNOTIF_H

/*=============================================================================

FILE: DialerNotif.h

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

$Header: //depot/asic/msmshared/apps/Dialer/DialerNotif.h#11 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
04/05/04   BPW     Alert changes for failed calls
08/29/03   BW      Fixed compile warnings
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
07/13/03   BW      Created DialerNotif Header File

=============================================================================*/

/*=============================================================================

                       CONSTANTS

=============================================================================*/

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/
/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_Init

DESCRIPTION: Initializes the controls for Notif dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NotifDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_KeyHandler

DESCRIPTION: Handles Notif Dialog's key events

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
boolean CDialerApp_NotifDlg_KeyHandler(CDialerApp *pUser,
                                       AEEEvent evt,
                                       uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_CmdHandler

DESCRIPTION: Handles Notif Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NotifDlg_CmdHandler(CDialerApp *pUser,
                                       uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_NotifDlg_EndHandler

DESCRIPTION: Handles Notif Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NotifDlg_EndHandler(CDialerApp *pUser);

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
boolean CDialerApp_NotifDlg_CloseDlg(void *pUser);

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
boolean CDialerApp_NotifDlg_DisplayRefresh(void *pUser);

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
                                    uint32 dwMaxDisplaySecs);


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
                                      uint32 dwMaxDisplaySecs);

#endif // DIALERNOTIF_H

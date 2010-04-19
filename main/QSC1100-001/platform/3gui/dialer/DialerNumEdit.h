#ifndef DIALERNUMEDIT_H
#define DIALERNUMEDIT_H

/*=============================================================================

FILE: DialerNumEdit.h

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

$Header: //depot/asic/msmshared/apps/Dialer/DialerNumEdit.h#9 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
08/22/07   cvs     Add framework for dialogs
11/21/03   BPW     Fixed compile error
11/20/03   BPW     Made timepause refresh function global
11/19/03   BPW     Added DisplayRefresh for NumeEdit
08/22/03   BW      Added TimePauseDlg and functionality
08/14/03   BW      Integration with both ICM and ISuppSvc
06/25/03   BW      Created DialerNumEdit Header File

=============================================================================*/

/*===========================================================================

CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DECLARATIONS

===========================================================================*/

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
boolean CDialerApp_NumEditDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_KeyHandler

DESCRIPTION: Handles NumEdit Dialog's key events

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
boolean CDialerApp_NumEditDlg_KeyHandler(CDialerApp *pUser,
                                         AEEEvent evt,
                                         uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_CmdHandler

DESCRIPTION: Handles NumEdit Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NumEditDlg_CmdHandler(CDialerApp *pUser,
                                         uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_NumEditDlg_EndHandler

DESCRIPTION: Handles NumEdit Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NumEditDlg_EndHandler(CDialerApp *pUser);


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
boolean CDialerApp_NumEditDlg_DisplayRefresh(void *pUser);

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
boolean CDialerApp_WildCardDlg_Init(CDialerApp *pMe);

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
boolean CDialerApp_WildCardDlg_KeyHandler(CDialerApp *pUser,
                                          AEEEvent evt,
                                          uint16 key);

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
boolean CDialerApp_WildCardDlg_CmdHandler(CDialerApp *pUser,
                                          uint16 ctrl_id);

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
boolean CDialerApp_WildCardDlg_EndHandler(CDialerApp *pUser);

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_Init

DESCRIPTION: Initializes TimePauseDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_TimePauseDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_KeyHandler

DESCRIPTION: Handles TimePauseDlg Dialog's key events

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
boolean CDialerApp_TimePauseDlg_KeyHandler(CDialerApp *pUser,
                                           AEEEvent evt,
                                           uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_CmdHandler

DESCRIPTION: Handles Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_TimePauseDlg_CmdHandler(CDialerApp *pUser,
                                           uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_TimePauseDlg_EndHandler

DESCRIPTION: Handles TimePauseDlg Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_TimePauseDlg_EndHandler(CDialerApp *pUser);

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
boolean CDialerApp_TimePauseDlg_DisplayRefresh(void *pUser);



#endif // DIALERNUMEDIT_H

#ifndef DIALERINCOMING_H
#define DIALERINCOMING_H

/*=============================================================================

FILE: DialerIncoming.h

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

$Header: //depot/asic/msmshared/apps/Dialer/DialerIncoming.h#6 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
08/14/03   BW      Integration with both ICM and ISuppSvc
07/24/03   BW      Created DialerIncoming Header File 

=============================================================================*/

/*===========================================================================

FUNCTION DECLARATIONS

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
boolean CDialerApp_IncomingDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_IncomingDlg_KeyHandler

DESCRIPTION: Handles Incoming Dialog's key events

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
boolean CDialerApp_IncomingDlg_KeyHandler(CDialerApp *pUser,
                                          AEEEvent evt,
                                          uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_IncomingDlg_CmdHandler

DESCRIPTION: Handles Incoming Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomingDlg_CmdHandler(CDialerApp *pUser,
                                          uint16 ctrl_id);

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
boolean CDialerApp_IncomingDlg_DisplayRefresh(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_IncomOptionsDlg_Init

DESCRIPTION: Initializes the controls for IncomOptions dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomOptionsDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_IncomOptionsDlg_KeyHandler

DESCRIPTION: Handles IncomOptions Dialog's key events

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
boolean CDialerApp_IncomOptionsDlg_KeyHandler(CDialerApp *pUser,
                                              AEEEvent evt,
                                              uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_IncomOptionsDlg_CmdHandler

DESCRIPTION: Handles IncomOptions Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IncomOptionsDlg_CmdHandler(CDialerApp *pUser,
                                              uint16 ctrl_id);

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
boolean CDialerApp_IncomOptionsDlg_DisplayRefresh(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_ForwardingDlg_Init

DESCRIPTION: Initializes ForwardingDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_ForwardingDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_ForwardingDlg_KeyHandler

DESCRIPTION: Handles ForwardingDlg Dialog's key events

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
boolean CDialerApp_ForwardingDlg_KeyHandler(CDialerApp *pUser,
                                            AEEEvent evt,
                                            uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_ForwardingDlg_CmdHandler

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
boolean CDialerApp_ForwardingDlg_CmdHandler(CDialerApp *pUser,
                                            uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_ForwardingDlg_EndHandler

DESCRIPTION: Handles ForwardingDlg Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_ForwardingDlg_EndHandler(CDialerApp *pUser);

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
boolean CDialerApp_ForwardingDlg_DisplayRefresh(void *pUser);

#endif // DIALERINCOMING_H

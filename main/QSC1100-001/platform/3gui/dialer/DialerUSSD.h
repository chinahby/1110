#ifndef DIALERUSSD_H
#define DIALERUSSD_H

/*=============================================================================

FILE: DialerUSSD.h

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

$Header: //depot/asic/msmshared/apps/Dialer/DialerUSSD.h#2 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
08/14/03   BW      Integration with both ICM and ISuppSvc
08/13/03   BW      Created DialerUSSD Header File

=============================================================================*/

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_USSDPasswordDlg_Init

DESCRIPTION: Initializes the controls for USSDPasswordDlg dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDPasswordDlg_Init(CDialerApp *pMe);

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
boolean CDialerApp_USSDPasswordDlg_CmdHandler(CDialerApp *pMe,
                                                uint16 ctrl_id);

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
boolean CDialerApp_USSDPasswordDlg_KeyHandler(CDialerApp *pMe,
                                                AEEEvent evt,
                                                uint16 key);

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
boolean CDialerApp_USSDPasswordDlg_EndHandler(CDialerApp *pMe);

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
boolean CDialerApp_USSDInfoDlg_Init(CDialerApp *pMe);

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
boolean CDialerApp_USSDInfoDlg_CmdHandler(CDialerApp *pMe,
                                                uint16 ctrl_id);

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
boolean CDialerApp_USSDInfoDlg_KeyHandler(CDialerApp *pMe,
                                                AEEEvent evt,
                                                uint16 key);

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
boolean CDialerApp_USSDInfoDlg_EndHandler(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_USSDPromptDlg_Init

DESCRIPTION: Initializes the controls for USSDPromptDlg dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_USSDPromptDlg_Init(CDialerApp *pMe);

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
boolean CDialerApp_USSDPromptDlg_CmdHandler(CDialerApp *pMe,
                                            uint16 ctrl_id);

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
boolean CDialerApp_USSDPromptDlg_KeyHandler(CDialerApp *pMe,
                                            AEEEvent evt,
                                            uint16 key);

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
boolean CDialerApp_USSDPromptDlg_EndHandler(CDialerApp *pMe);

#endif // DIALERUSSD_H

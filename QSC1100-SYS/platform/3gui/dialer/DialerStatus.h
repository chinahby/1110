#ifndef DIALERSTATUS_H
#define DIALERSTATUS_H

/*=============================================================================

FILE: DialerStatus.h

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

$Header: //depot/asic/msmshared/apps/Dialer/DialerStatus.h#10 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
03/11/05   cvs     Optimize Status screen
08/29/03   BW      Fixed compile warnings
08/14/03   BW      Integration with both ICM and ISuppSvc
07/27/03   BW      Created DialerStatus Header File 

=============================================================================*/

/*===========================================================================

FUNCTION DECLARATIONS

===========================================================================*/

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
boolean CDialerApp_StatusDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_KeyHandler

DESCRIPTION: Handles Status Dialog's key events

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
boolean CDialerApp_StatusDlg_KeyHandler(CDialerApp *pUser,
                                        AEEEvent evt,
                                        uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_CmdHandler

DESCRIPTION: Handles Status Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StatusDlg_CmdHandler(CDialerApp *pUser,
                                        uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_NoftifDlg_EndHandler

DESCRIPTION: Handles Noftif Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StatusDlg_EndHandler(CDialerApp *pUser);

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
boolean CDialerApp_StatusDlg_DisplayInit(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_StatusDlg_DisplayRefresh

DESCRIPTION: Gets updated info for status screen if possible, else uses DisplayInit()

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StatusDlg_DisplayRefresh(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_MissedCallDlg_Init

DESCRIPTION: Initializes MissedCallDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_MissedCallDlg_Init(CDialerApp *pMe);

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
boolean CDialerApp_MissedCallDlg_KeyHandler(CDialerApp *pUser,
                                            AEEEvent evt,
                                            uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_MissedCallDlg_CmdHandler

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
boolean CDialerApp_MissedCallDlg_CmdHandler(CDialerApp *pUser,
                                            uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_MissedCallDlg_EndHandler

DESCRIPTION: Handles MissedCallDlg Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_MissedCallDlg_EndHandler(CDialerApp *pUser);

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
void CDialerApp_StatusDlg_TimeRefresh(void *pUser);

#endif // DIALERSTATUS_H

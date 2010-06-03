#ifndef DIALERCALLING_H
#define DIALERCALLING_H

/*=============================================================================

FILE: DialerCalling.h

SERVICES: Dialer Applet Dialogs

GENERAL DESCRIPTION: Calling Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerCalling.h#7 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
08/22/07   cvs     Add framework for dialogs
06/07/05   sun     Added support for CCBS
08/14/03   BW      Integration with both ICM and ISuppSvc
07/24/03   BW      Created DialerCalling Header File 

=============================================================================*/

/*===========================================================================

FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_CallingDlg_KeyHandler

DESCRIPTION: Handles Calling Dialog's key events

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
boolean CDialerApp_CallingDlg_KeyHandler(CDialerApp *pMe,
                                         AEEEvent evt,
                                         uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_CallingDlg_CmdHandler

DESCRIPTION: Handles Calling Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallingDlg_CmdHandler(CDialerApp *pUser,
                                         uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_CallingDlg_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallingDlg_EndHandler(CDialerApp *pUser);

/*=============================================================================
FUNCTION: CDialerApp_CallingDlg_Init

DESCRIPTION: Initializes the controls for Calling dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallingDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_CallingDlg_DisplayRefresh

DESCRIPTION: Refreshes the calling dialog display

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallingDlg_DisplayRefresh(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_SSRequestDlg_Init

DESCRIPTION: Initializes SSRequestDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SSRequestDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SSRequestDlg_KeyHandler

DESCRIPTION: Handles SSRequestDlg Dialog's key events

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
boolean CDialerApp_SSRequestDlg_KeyHandler(CDialerApp *pUser,
                                           AEEEvent evt,
                                           uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_SSRequestDlg_CmdHandler

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
boolean CDialerApp_SSRequestDlg_CmdHandler(CDialerApp *pUser,
                                           uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_RecallDlg_Init

DESCRIPTION: Initializes RecallDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_RecallDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_RecallDlg_KeyHandler

DESCRIPTION: Handles RecallDlg Dialog's key events

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
boolean CDialerApp_RecallDlg_KeyHandler(CDialerApp *pUser,
                                        AEEEvent evt,
                                        uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_RecallDlg_CmdHandler

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
boolean CDialerApp_RecallDlg_CmdHandler(CDialerApp *pUser,
                                        uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SetupRecallDlg_Init

DESCRIPTION: Initializes SetupRecallDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SetupRecallDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SetupRecallDlg_KeyHandler

DESCRIPTION: Handles SetupRecallDlg Dialog's key events

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
boolean CDialerApp_SetupRecallDlg_KeyHandler(CDialerApp *pUser,
                                             AEEEvent evt,
                                             uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_SetupRecallDlg_CmdHandler

DESCRIPTION: Handles SetupRecall Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SetupRecallDlg_CmdHandler(CDialerApp *pUser,
                                             uint16 ctrl_id);

#endif // DIALERCALLING_H

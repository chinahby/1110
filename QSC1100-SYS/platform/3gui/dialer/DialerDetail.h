#ifndef DIALERDETAIL_H
#define DIALERDETAIL_H

/*=============================================================================

FILE: DialerDetail.h

SERVICES: Dialer Applet Dialogs

GENERAL DESCRIPTION: Detail Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerDetail.h#5 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
08/14/03   BW      Integration with both ICM and ISuppSvc
08/07/03   BW      Created DialerDetail Header File 

=============================================================================*/

/*===========================================================================

FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: CDialerApp_DetailDlg_Init

DESCRIPTION: Initializes DetailDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_DetailDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_DetailDlg_CmdHandler

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
boolean CDialerApp_DetailDlg_CmdHandler(CDialerApp *pUser,
                                        uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_DetailDlg_EndHandler

DESCRIPTION: Handles DetailDlg Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_DetailDlg_EndHandler(CDialerApp *pUser);

/*=============================================================================
FUNCTION: CDialerApp_DetailDlg_DisplayRefresh

DESCRIPTION: Refreshes the controls for Detail dialog

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_DetailDlg_DisplayRefresh(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_CallOptionsDlg_Init

DESCRIPTION: Initializes the controls for CallOptions dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallOptionsDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_CallOptionsDlg_CmdHandler

DESCRIPTION: Handles CallOptions Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallOptionsDlg_CmdHandler(CDialerApp *pUser,
                                             uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_CallOptionsDlg_EventHandler

DESCRIPTION: Handles Call Options Dialog's events

PARAMETERS:
  *pUser: CDialerApp object pointer
  evt: event code
  w: word event param
  dw: dword event param

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallOptionsDlg_EventHandler(void *pUser, 
                                                AEEEvent evt, 
                                                uint16 w, 
                                                uint32 dw);

/*=============================================================================
FUNCTION: CDialerApp_CallOptionsDlg_DisplayRefresh

DESCRIPTION: Refreshes the call options dialog display

PARAMETERS:
  *puser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CallOptionsDlg_DisplayRefresh(void *pUser);

#endif // DIALERDETAIL_H

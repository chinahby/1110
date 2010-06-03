#ifndef DIALERSUPS_H 
#define DIALERSUPS_H

/*=============================================================================

FILE: DialerSupps.h

SERVICES: Supplementary Services for Dialer Applet

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
						EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerSups.h#10 $
$DateTime: 2008/04/10 11:06:43 $
$Author: sgavini $
$Change: 640290 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/19/08   sg      Modify all dialogs to use the new framework
06/07/05   sun     Added support for CCBS
11/25/03   BPW     Linted for the Saber 6250 build
10/21/03   SUN     Fixed Compile Error
09/04/03   SUN     Changes related to AEESUPPSVC
08/19/03   BW      Added messages and more code clean up
06/26/03   ram     Created Initial version

=============================================================================*/


/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef FEATURE_APP_DIALER
#include "AEEStdLib.h" // required for "boolean" data type
#include "dialerapp_res.h"


/*===========================================================================

 MACRO DEFINITIONS

===========================================================================*/

#define IS_CF_SS(ss_code) (AEESUPPSVC_CFB == ss_code ||AEESUPPSVC_CFNRY == ss_code \
    || AEESUPPSVC_CFNRC == ss_code ||AEESUPPSVC_CFU == ss_code \
    || AEESUPPSVC_ALL_FORWARDING_SS == ss_code || AEESUPPSVC_ALL_CONDFWD_SS == ss_code)

#define IS_CB_SS(ss_code) (AEESUPPSVC_BAOC == ss_code ||AEESUPPSVC_BAIC == ss_code \
    || AEESUPPSVC_BICROAM == ss_code ||AEESUPPSVC_BOIC == ss_code \
    || AEESUPPSVC_BOIC_EXHC == ss_code || AEESUPPSVC_ALL_CALL_RSTRCT_SS == ss_code\
    || AEESUPPSVC_BARRING_OF_INCOMING_CALLS == ss_code || AEESUPPSVC_BARRING_OUTGOING_CALLS == ss_code)

#define IS_CW_SS(ss_code) ( AEESUPPSVC_CW == ss_code)

#define IS_CI_SS(ss_code) (AEESUPPSVC_CLIP == ss_code ||AEESUPPSVC_CLIR == ss_code \
    || AEESUPPSVC_COLP == ss_code ||AEESUPPSVC_COLR == ss_code)

/*=============================================================================
FUNCTION: CDialerApp_SupsServiceProgramming_Init

DESCRIPTION: Initializes the controls for SupsServiceProgramming dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsServiceProgramming_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsServiceProgramming_CmdHandler

DESCRIPTION: Handles SupsServiceProgramming Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsServiceProgramming_CmdHandler(CDialerApp *pUser,
                                         uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SupsCallForwarding_Init

DESCRIPTION: Initializes the controls for SupsCallForwarding dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallForwarding_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsCallForwarding_CmdHandler

DESCRIPTION: Handles SupsCallForwarding Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallForwarding_CmdHandler(CDialerApp *pUser,
                                         uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SupsCallForwarding_KeyHandler

DESCRIPTION: Handles SupsCallForwarding Dialog's key events

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
boolean CDialerApp_SupsCallForwarding_KeyHandler(CDialerApp *pUser,
                                         AEEEvent evt,
                                         uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_SupsSupsOperation_Init

DESCRIPTION: Initializes the controls for SupsSupsOperation dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsSupsOperation_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsSupsOperation_CmdHandler

DESCRIPTION: Handles SupsSupsOperation Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsSupsOperation_CmdHandler(CDialerApp *pUser,
                                                uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SupsSupsOperation_KeyHandler

DESCRIPTION: Handles SupsSupsOperation Dialog's key events

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
boolean CDialerApp_SupsSupsOperation_KeyHandler(CDialerApp *pUser,
                                                AEEEvent evt,
                                                uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_SupsCFRegister_Init

DESCRIPTION: Initializes the controls for SupsCFRegister dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCFRegister_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsCFRegister_CmdHandler

DESCRIPTION: Handles SupsCFRegister Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCFRegister_CmdHandler(CDialerApp *pUser,
                                                uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SupsCFRegister_KeyHandler

DESCRIPTION: Handles SupsCFRegister Dialog's key events

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
boolean CDialerApp_SupsCFRegister_KeyHandler(CDialerApp *pUser,
                                                AEEEvent evt,
                                                uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_SupsCFRegister_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCFRegister_EndHandler(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsServiceGroup_Init

DESCRIPTION: Initializes the controls for SupsServiceGroup dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsServiceGroup_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsServiceGroup_CmdHandler

DESCRIPTION: Handles SupsServiceGroup Dialog's EVT_COMMAND events

PARAMETERS:
  *pUser: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsServiceGroup_CmdHandler(CDialerApp *pUser,
                                                uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SupsServiceGroup_KeyHandler

DESCRIPTION: Handles SupsServiceGroup Dialog's key events

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
boolean CDialerApp_SupsServiceGroup_KeyHandler(CDialerApp *pUser,
                                                AEEEvent evt,
                                                uint16 key);

/*===========================================================================
FUNCTION CDialerApp_SupsCFNRTimerDlgHandler

DESCRIPTION
  Call Forwarding on No Reply Timer Dialog Event Handler

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDialerApp_SupsCFNRTimerDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*=============================================================================
FUNCTION: CDialerApp_SupsCallBarring_Init

DESCRIPTION: Initializes the controls for SupsCallBarring dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallBarring_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsCallBarring_CmdHandler

DESCRIPTION: Handles SupsCallBarring Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallBarring_CmdHandler(CDialerApp *pMe,
                                              uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SupsCallBarring_KeyHandler

DESCRIPTION: Handles SupsCallBarring Dialog's key events

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
boolean CDialerApp_SupsCallBarring_KeyHandler(CDialerApp *pMe,
                                              AEEEvent evt,
                                              uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_SupsCBPassword_Init

DESCRIPTION: Initializes the controls for SupsCBPassword dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCBPassword_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsCBPassword_CmdHandler

DESCRIPTION: Handles SupsCBPassword Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCBPassword_CmdHandler(CDialerApp *pMe,
                                             uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SupsCBPassword_KeyHandler

DESCRIPTION: Handles SupsCBPassword Dialog's key events

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
boolean CDialerApp_SupsCBPassword_KeyHandler(CDialerApp *pMe,
                                             AEEEvent evt,
                                             uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_SupsCBPassword_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCBPassword_EndHandler(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsCallerIdentity_Init

DESCRIPTION: Initializes the controls for SupsCallerIdentity dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallerIdentity_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsCallerIdentity_CmdHandler

DESCRIPTION: Handles SupsCallerIdentity Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCallerIdentity_CmdHandler(CDialerApp *pMe,
                                                 uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SupsCallerIdentity_KeyHandler

DESCRIPTION: Handles SupsCallerIdentity Dialog's key events

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
boolean CDialerApp_SupsCallerIdentity_KeyHandler(CDialerApp *pMe,
                                                 AEEEvent evt,
                                                 uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_SupsChangePassword_Init

DESCRIPTION: Initializes the controls for SupsChangePassword dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsChangePassword_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsChangePassword_CmdHandler

DESCRIPTION: Handles SupsChangePassword Dialog's EVT_COMMAND events

PARAMETERS:
  *pMe: CDialerApp object pointer
  ctrl_id: control ID generating EVT_COMMAND

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsChangePassword_CmdHandler(CDialerApp *pMe,
                                                 uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SupsChangePassword_KeyHandler

DESCRIPTION: Handles SupsChangePassword Dialog's key events

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
boolean CDialerApp_SupsChangePassword_KeyHandler(CDialerApp *pMe,
                                                 AEEEvent evt,
                                                 uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_SupsChangePassword_EndHandler

DESCRIPTION: Handles Calling Dialog's EVT_DIALOG_END events

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsChangePassword_EndHandler(CDialerApp *pMe);

/*===========================================================================
FUNCTION CDialerApp_SupsChangePasswordDlgHandler

DESCRIPTION
  Change Password Dialog Event Handler

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDialerApp_SupsChangePasswordDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDialerApp_SupsInitializeStartSSObject

DESCRIPTION
    Allocate and initialize the fields of the Startss_event object
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CDialerApp_SupsInitializeStartSSObject(CDialerApp *pMe);

/*=============================================================================
FUNCTION:  CDialerApp_SupsPopupWizard

DESCRIPTION: Function to pop-up the SS Menu Dialog from SS Wizard.
Based on the values set (and values not set), in the ss_start structure
decide the dialog to create.
PARAMETERS: 

RETURN VALUE:

COMMENTS: 

    ASSUMES THAT pMe->m,_pSSstart STRUCTURE HAS ALREADY BEEN ALLOCATED AND
    FILLED BY CALLEE.(BY CALLING THE ISUPPS FUNCTION??)
   
SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsPopupWizard(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsCCBSDlg_Init

DESCRIPTION: Initializes SupsCCBSDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCCBSDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsCCBSDlg_KeyHandler

DESCRIPTION: Handles SupsCCBSDlg Dialog's key events

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
boolean CDialerApp_SupsCCBSDlg_KeyHandler(CDialerApp *pUser,
                                          AEEEvent evt,
                                          uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_SupsCCBSDlg_CmdHandler

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
boolean CDialerApp_SupsCCBSDlg_CmdHandler(CDialerApp *pUser,
                                          uint16 ctrl_id);

/*=============================================================================
FUNCTION: CDialerApp_SupsCCBSListDlg_Init

DESCRIPTION: Initializes CCBSIndexDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_SupsCCBSListDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_SupsCCBSListDlg_KeyHandler

DESCRIPTION: Handles SupsCCBSListDlg Dialog's key events

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
boolean CDialerApp_SupsCCBSListDlg_KeyHandler(CDialerApp *pUser,
                                              AEEEvent evt,
                                              uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_CCBSIndexDlg_Init

DESCRIPTION: Initializes CCBSIndexDlg the controls for  dialog

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CCBSIndexDlg_Init(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_CCBSIndexDlg_KeyHandler

DESCRIPTION: Handles CCBSIndexDlg Dialog's key events

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
boolean CDialerApp_CCBSIndexDlg_KeyHandler(CDialerApp *pUser,
                                           AEEEvent evt,
                                           uint16 key);

/*=============================================================================
FUNCTION: CDialerApp_CCBSIndexDlg_CmdHandler

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
boolean CDialerApp_CCBSIndexDlg_CmdHandler(CDialerApp *pUser,
                                           uint16 ctrl_id);

#endif //FEATURE_APP_DIALER
#endif // DIALERSUPS_H

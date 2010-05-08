#ifndef DIALERUTILS_H
#define DIALERUTILS_H

/*=============================================================================

FILE: DialerUtils.h

SERVICES: Dialer Applet

GENERAL DESCRIPTION: Misc functions

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 - 2007 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerUtils.h#39 $
$DateTime: 2007/08/02 13:17:34 $
$Author: carls $
$Change: 516623 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/02/07   cvs     Featurize all secondary display support
11/22/06   ak      Added a function prototype for warning removal
10/12/05   cvs     Carkit display
06/07/05   sun     Added support for CCBS
09/20/04   SUN     Fixed Mute and Vocoder settings
07/27/04   JAS     Added support for dual-display targets
05/20/04   SUN     Handle Progress Info Event
04/05/04   BPW     Alert changes for failed calls
03/19/04   sun     Added changes for DTMF
01/08/04   BPW     Use own dialer hyphenation function
12/18/03   BPW     Added base station signalling support
12/02/03   BPW     Code review changes, BREW 3.0 support, & minor changes
11/25/03   BPW     Linted for the Saber 6250 build
11/13/03   BPW     Added function to format display for phone number
11/13/03   BPW     Press and hold clear to clear contents of text control
08/22/03   BW      Modified CDialerApp_PopDlg function
08/19/03   BW      Added messages, param checking, and more code clean up
08/14/03   BW      Integration with both ICM and ISuppSvc
06/15/03   BW      Created DialerUtils Header File

=============================================================================*/
/*=============================================================================

                       INCLUDES

=============================================================================*/
#include "err.h"
#include "msg.h"
#include "AEEShell.h"
#include "DialerApp.h"

/*=============================================================================

                      MACRO DECLARATIONS

=============================================================================== */
#ifdef _lint
#define DIALER_ERR(format, code1, code2, code3) MSG_ERROR("", code1, code2, code3)
#define DIALER_MSG(format, code1, code2, code3) MSG_HIGH("", code1, code2, code3)
#else
/*******
 * The following bit of ugliness is needed to allow string pasting of __LINE__ in the
 * following macros.  We have to onvert __LINE__ from an integer value to a string.  
 * We use # to stringify, but it has to be a parameter to use that operator.  But, if
 * we only use a single function then it stringifies the keyword (__FILE__) instead of
 * the value of __FILE__!
 *******/
#define __MAKESTRING2__(x) #x
#define __MAKESTRING1__(x) __MAKESTRING2__(x) 
#ifdef T_WINNT
#define DIALER_ERR(format, code1, code2, code3) MSG_ERROR("DIALER: " format " " __FILE__ "(" __MAKESTRING1__(__LINE__) ")", code1, code2, code3)
#define DIALER_MSG(format, code1, code2, code3) MSG_HIGH("DIALER: " format " " __FILE__ "(" __MAKESTRING1__(__LINE__) ")", code1, code2, code3)
#else
#define DIALER_ERR(format, code1, code2, code3) MSG_ERROR("DIALER: " format " " __MODULE__ "(" __MAKESTRING1__(__LINE__) ")", code1, code2, code3)
#define DIALER_MSG(format, code1, code2, code3) MSG_HIGH("DIALER: " format " " __MODULE__ "(" __MAKESTRING1__(__LINE__) ")", code1, code2, code3)
#endif  /* T_WINNT */
#endif
/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/
/*=============================================================================
FUNCTION: CDialerApp_CheckAEEReturnStatus

DESCRIPTION: Checks the return status and tries to print a message if false

PARAMETERS:
  *pUser: CDialerApp object pointer
  nStatus: status value to check

RETURN VALUE:
  boolean: Returns TRUE if status value was SUCCESS and FALSE otherwise

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CheckAEEReturnStatus(void *pUser, int nStatus);

/*=============================================================================
FUNCTION: CDialerApp_ClearTextCtl

DESCRIPTION: Clears a text control's contents

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_ClearTextCtl(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_TextCtlGetText

DESCRIPTION: Gets text from text control

PARAMETERS:
  *pUser: CDialerApp object pointer
  *pszStr: string pointer to store result in
  nStrMax: length of string to get
  nTextCtlID: textctl id

RETURN VALUE:
  boolean: Returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_TextCtlGetText(void *pUser, AECHAR *pszStr,
                                  uint16 wStrMax, uint16 wTextCtlID);

/*=============================================================================
FUNCTION: CDialerApp_TextCtlGetTextEx

DESCRIPTION: Gets text from text control and creates memory for it

PARAMETERS:
  **pszDestBuf: string pointer to store result in
  *pIDialog; pointer to dialog
  nTextCtlID: textctl id

RETURN VALUE:
  uint16: length of string stored

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16 CDialerApp_TextCtlGetTextEx(AECHAR **pszDestBuf, IDialog *pIDialog, uint16 wTextCtlId);

/*=============================================================================
FUNCTION: CDialerApp_HyphenateNumberString

DESCRIPTION: Hyphenates the number string passed in.  If non digit chars
             exist, the string will just be returned without modification.

PARAMETERS:
  *pszStr: null terminated number string to be hyphenated

RETURN VALUE:
  AECHAR*: null terminated hyphenated string

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AECHAR* CDialerApp_HyphenateNumberString(AECHAR const *pszStr);

/*=============================================================================
FUNCTION: CDialerApp_FormatNumberDisplayString

DESCRIPTION: Formats and decides what string to display for the number from
             the call info

PARAMETERS:
  *pUser: CDialerApp object pointer
  *pCallInfo: Call info to extract string from
  *pszStrBuf: string buffer to store string
  *dwSizeBuf: size of string buffer in AECHARs
  bHyphen: include hyphens in number

RETURN VALUE:
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_FormatNumberDisplayString(void *pUser, AEECMCallInfo *pCallInfo, 
                                          AECHAR *pszStrBuf, uint32 dwSizeBuf,
                                          boolean bHyphen);

/*=============================================================================
FUNCTION: CDialerApp_IsCallsToDisplay

DESCRIPTION: checks to see if there is any active calls to display

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if active calls exist

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IsCallsToDisplay(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_HandleEndKey

DESCRIPTION: Handles the actions of the end key depending on active dialog

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean : TRUE if all the dialogs were ended successfuly.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_HandleEndKey(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_CenterTitle

DESCRIPTION: Pads the beginning of title string with spaces to center it

PARAMETERS:
  *pUser: CDialerApp object pointer
  *pszSource: Contains the title string to be centered
  **pszDest: Contains the pointer to the location to store centered string
             MUST point to NULL
  wWidth: max width in pixels of area to center over
  Font: font of the title to be used

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:
  The memory for the destination buffer is allocated in this function and 
  MUST be freed at a later time.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CenterTitle(void *pUser, AECHAR *pszSource, AECHAR **pszDest,
                               uint32 wWidth, AEEFont Font);

/*=============================================================================
FUNCTION: CDialerApp_AutoAnswer

DESCRIPTION: Checks the rings and settings to see if the call should be auto
             abnswered

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_AutoAnswer(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_OriginateVoiceCall

DESCRIPTION: Originate a voice call with all of the proper checks and create
             the needed dialog

PARAMETERS:
  *pUser: CDialerApp object pointer
  *pszNumber: number buffer to be dialed

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_OriginateVoiceCall(void *pUser, AECHAR* pszNumber);

/*=============================================================================
FUNCTION: CDialerApp_StartKeyPressDTMF

DESCRIPTION: Starts playing Key Press DTMF

PARAMETERS:
  *pUser: CDialerApp object pointer
  cDTMF: char of dtmf tone to be started

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StartKeyPressDTMF(void *pUser, AECHAR cDTMF);

/*=============================================================================
FUNCTION: CDialerApp_StopKeyPressDTMF

DESCRIPTION: Stops the Key Press DTMF

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StopKeyPressDTMF(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_StopApp

DESCRIPTION: Stops dialer and returns to core and performs any cleanup needed

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: TRUE if successfull

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StopApp(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_SpeedDial

DESCRIPTION: Attempts to perform a speed dial

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_SpeedDial(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_StartTimer_SpeedDial

DESCRIPTION: Starts the timer for speed dial

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StartTimer_SpeedDial(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_StopTimer_SpeedDial

DESCRIPTION: Stops the timer for speed dial

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_StopTimer_SpeedDial(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_PlayCallFailedSound

DESCRIPTION:  Plays the call failed sound

PARAMETERS:
  *pUser: CDialerApp object pointer
  Alert: Alert to be played by IAlert

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_PlayCallFailedSound(void *pUser, uint32 Alert);

/*=============================================================================
FUNCTION: CDialerApp_StopCallFailedSound

DESCRIPTION:  Stops the call failed sound if it is still playing

PARAMETERS:
  *puser: CDialerApp object pointer

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_StopCallFailedSound(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_NVCheck_IsAutoHyphen

DESCRIPTION: Checks NV auto hyphen setting

PARAMETERS:
  NONE

RETURN VALUE:
  boolean: returns TRUE if nv setting is set

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NVCheck_IsAutoHyphen(void);

/*=============================================================================
FUNCTION: CDialerApp_NV_GetDTMF

DESCRIPTION: Gets NV DTMF setting

PARAMETERS:
  NONE

RETURN VALUE:
  uint8: returns nv setting

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint8 CDialerApp_NV_GetDTMF(void);

/*=============================================================================
FUNCTION: CDialerApp_NVCheck_IsOneTouch

DESCRIPTION: Checks NV one touch setting

PARAMETERS:
  NONE

RETURN VALUE:
  boolean: returns TRUE if nv setting is set

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NVCheck_IsOneTouch(void);

/*=============================================================================
FUNCTION: CDialerApp_NVCheck_IsAutoAnswer

DESCRIPTION: Checks NV auto answer setting

PARAMETERS:
  NONE

RETURN VALUE:
  boolean: returns TRUE if nv setting is set

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_NVCheck_IsAutoAnswer(void);


/*=============================================================================
FUNCTION: CDialerApp_GetActiveDlg

DESCRIPTION: Returns pointer to the active dialog object.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  IDialog*: pointer to the active dialog object or NULL if error occurs

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
IDialog* CDialerApp_GetActiveDlg(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_GetActiveDlgID 

DESCRIPTION: Prints the dlg stack.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  uint16: active dialog ID or 0 if an error occurs

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
uint16 CDialerApp_GetActiveDlgID(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_GetActiveDlgInfo

DESCRIPTION: Prints the dlg stack.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  void*: pointer to the active dialogs info structure and NULL if error occurs

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
void* CDialerApp_GetActiveDlgInfo(void *pUser);

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
/*=============================================================================
FUNCTION: CDialerApp_GetSecText

DESCRIPTION: Gets the static control for the secondary display.  Creates
  the control if it does not already exist.

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
    boolean:  TRUE if the text pointer is now available; FALSE otherwise

COMMENTS:
  None 

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_GetSecText(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_UpdateSecondaryDisp

DESCRIPTION: Updates the secondary display based on the inputs

PARAMETERS:
  *pMe: CDialerApp object pointer

RETURN VALUE:
  None

COMMENTS:
  This routine updates the display based on the m_DispState member of the
  Dialer app structure.  As a result, that member should be updated as
  necessary before invoking this function.

  Since this may be executed on single-display targets, the absence of any
  secondary display is not considered an error condition.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_UpdateSecondaryDisp(CDialerApp *pMe);

/*=============================================================================
FUNCTION: CDialerApp_PrintToCarkit

DESCRIPTION: Updates the carkit display based on the string passed in

PARAMETERS:
  *pszText: pointer to the new text to put on the display.  NULL clears the 
            display.

RETURN VALUE:
  None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void CDialerApp_PrintToCarkit(AECHAR *pszText);

#endif //FEATURE_UI_SECONDARY_DISP_SUPPORT

/*=============================================================================
FUNCTION: CDialerApp_DisplayStatusDlg

DESCRIPTION: Bring the program to the status screen display

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_DisplayStatusDlg(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_EndCallRelatedDlgs

DESCRIPTION: End dialogs related to the passed call id.  This could possibly 
             cause the StatusDlg to be displayed.

PARAMETERS:
  *pUser: CDialerApp object pointer
   byCallID: call id from an event

RETURN VALUE:
  boolean: returns TRUE if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_EndCallRelatedDlgs(void *pUser, uint8 byCallID);

/*=============================================================================
FUNCTION: CDialerApp_EndAllDlgs

DESCRIPTION: Ends all of the dialogs unconditionally

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean : TRUE if the successfull

COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
boolean CDialerApp_EndAllDlgs(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_EndDlg

DESCRIPTION: Ends the active dialog unconditionally.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean : TRUE if the successfull

COMMENTS: 

SIDE EFFECTS: App will stop only one dialog is in the stack.

SEE ALSO:

=============================================================================*/
boolean CDialerApp_EndDlg(void *pUser);

/*=============================================================================
FUNCTION: CDialerApp_EndDlgEx

DESCRIPTION: Ends the first occurance of the specified dialog.

PARAMETERS:
  *pUser: CDialerApp object pointer
  wDlgID: ID of dialog to end

RETURN VALUE:
  boolean : TRUE if the successfull

COMMENTS: 

SIDE EFFECTS: App will stop only one dialog is in the stack.

SEE ALSO:

=============================================================================*/
boolean CDialerApp_EndDlgEx(void *pUser, uint16 wDlgID);

/*=============================================================================
FUNCTION: CDialerApp_CreateDlg

DESCRIPTION: Create the dialog passed in but make checks for Notif dialog

PARAMETERS:
  *pUser: CDialerApp object pointer
  wCreateDlgID: dialog to create
  *pInfo: info to be added to the dialog 
  dwInfoSize: size of info object

RETURN VALUE:
  boolean: TRUE if SUCCESS in creating the Dialog

COMMENTS: pInfo may be NULL and dwInfoSize equal to zero if there is no
          additional info for the dialog.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CreateDlg(void *pUser, uint16 wCreateDlgID, void *pInfo, uint32 dwInfoSize);

/*=============================================================================
FUNCTION: CDialerApp_HandleProgressInfo

DESCRIPTION: Handles the Information sent by ICM in PROGESS_INFO

PARAMETERS:
  *pUser: CDialerApp object pointer
  callID: The current call ID

RETURN VALUE:
  void

COMMENTS: 

SIDE EFFECTS: Will play the RingBack tone in GSM/WCDMA

SEE ALSO:

=============================================================================*/

void CDialerApp_HandleProgressInfo(void *pUser, uint8 callId);

/*=============================================================================
FUNCTION: CDialerApp_SetVocoder

DESCRIPTION: Sets the vocoder on/off.

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  NONE

COMMENTS: 

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
void CDialerApp_SetVocoder(void *pUser, boolean bVocoder);

/*=============================================================================
FUNCTION: CDialerApp_SetMute

DESCRIPTION: Mutes/Unmutes the phone

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  NONE

COMMENTS: Used for debug purposes

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
void CDialerApp_SetMute(void *pUser, boolean mute);

/*=============================================================================
FUNCTION: CDialerApp_Check_IsCCBSAutoAccept

DESCRIPTION: Checks the Config Setting to see if the recall should be auto
             answered

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  uint32: Config Value

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 CDialerApp_Check_IsCCBSAutoAccept(void *pUser);
/*=============================================================================
FUNCTION: CDialerApp_CCBSAutoAccept

DESCRIPTION: Checks the Config to see if the recall should be auto
             answered

PARAMETERS:
  *pUser: CDialerApp object pointer

RETURN VALUE:
  boolean: TRUE if successfull
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_CCBSAutoAccept(void *pUser);

#ifdef FEATURE_UI_SECONDARY_DISP_SUPPORT
/*=============================================================================
FUNCTION: CDialerApp_IsAnyUSBCarkitConnected

DESCRIPTION: Checks the Config to see if there is a USB carkit connected

PARAMETERS:
  
RETURN VALUE:
  boolean: TRUE if there is a carkit connected

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_IsAnyUSBCarkitConnected(void);

#endif //FEATURE_UI_SECONDARY_DISP_SUPPORT

/*=============================================================================
FUNCTION: CDialerApp_HandleNSSRelRecord

DESCRIPTION: The NSS Release Record handling is done by this function.
             The requirement is to release the call when we see this record

PARAMETERS:
  *pUser: CDialerApp object pointer
   uint8: Call Id of the current call

RETURN VALUE:
   boolean: TRUE if the call was ended FALSE otherwise
COMMENTS:

SIDE EFFECTS: 

SEE ALSO:

=============================================================================*/
boolean CDialerApp_HandleNSSRelRecord(void *pUser, uint8 callId);

#endif // DIALERUTILS_H

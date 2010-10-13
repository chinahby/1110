#ifndef APPCOMFUNC_H
#define APPCOMFUNC_H

/*=============================================================================

FILE: AppComFunc.h

SERVICES: Provides common functions used in several of the BREW applets.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

void ShowBusyIcon(IShell *ps, IDisplay *pd, AEERect *rc);
void SetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm);
void SetDefaultMenuLook(IShell *ps, IMenuCtl *pm);
void SetDefaultIconLook(IShell *ps, IMenuCtl *pm);
void InitMenuIcons(IMenuCtl *pm);
void SetMenuIcon(IMenuCtl *pm, uint16 listId, boolean bSel);
void DoNumberLookup(IAddrBook *pa, AECHAR *number, AECHAR *name, uint32 size);
void DoSpeedDialLookup(IAddrBook *pa, AECHAR *speedialindex, AECHAR *number, uint32 size);
AECHAR *FormatPhoneNumberForDisplay(AECHAR const *number);
void GetTimeString(IConfig *cfg, ITimeCtl *tctl, IShell *ish,
                          uint32 timeVal, AECHAR *timeStr);
void TabController_Init(DlgTabCtl **pDlgTabCtl, int numControls);//, IShell *pIShell, AEECLSID clsId);
void TabController_AddControl(DlgTabCtl *pDlgTabCtl, int CtlType, IControl *pIControl);
void TabController_SetActiveEx(DlgTabCtl *pDlgTabCtl, int index);
void TabController_HandleEvent(DlgTabCtl** pDlgTabCtl, AEEEvent evt, uint16 wParam);

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 - 2003 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/Core/AppComFunc.h#44 $
  $DateTime: 2009/04/22 00:18:30 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
04/17/09   Gopi    Added back IsCallActive, ClearBlockUITask, BlockUITask, 
                      UnblockUITask 
03/11/09   Gopi    Moved IsCallActive, ClearBlockUITask, BlockUITask, 
                      UnblockUITask to uiutils.c for Mango UI Integration
02/23/07   jas     Refactoring for HDK builds
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/07/06   cvs     Fix lint warnings
09/19/06   jas     Moving a local AppComFunc structure definition to the
                   .c file and fixing a compile error.
09/19/06   jas     Adding band value to AEECMWLANPref structure.
09/08/06   jas     Removing menus that were related to the NetSettings app
08/02/06   joe     NetSettings support for CoreApp.
05/24/06   JD      Added selectfont function to increase font on VGA displays.
04/04/06   jas     Added routines to get primary display size and select fonts
07/29/05   AT      Added functions which allow OEM layer calls to block
                   the UI task.
06/10/05   cvs     Support for Smart USB Carkit
05/11/05   ak      Added a different Decoding Scheme
05/12/05   cvs     RVCT compiler fix
05/09/05   ak      Using the packed string Decoding
03/05/05   cvs     Added debugging macros
03/10/05   ak      Added CPHS EONS Support
01/25/05   sun     Allow other Apps to Change Backlight setting via CoreApp
11/23/04   sun     Added support for USB HFK
10/08/04   SUN     Added function to Display Data from other Apps
07/18/04   sun     Added broadcast to idle menu while the app is suspended
01/25/04   PMD     Added function OriginateMessage()
12/09/03   ram     Removed lint errors.
11/25/03   ram     Add broadcast text to idle menu.
11/25/03   BPW     Modified AnswerVoiceCall to reject the incoming call as well
11/06/03   BPW     Added AnswerVoiceCall function
10/15/03   BPW     Changed DoNumberLookup to be exact
09/24/03   kar     Added new function to return if in call
08/29/03   ram     Added Macros CheckAEEReturnStatus and PRINT_FUNCTION_NAME()
08/12/03   ram     Added Tab controller functionality for multi-control dialogs
08/08/03   sun     Added functions to initialize and set check boxes
05/23/03   kar     Updated comments
05/12/03   kar     Added OriginateVoiceCall
05/08/03   PA      Added DoVoiceTagLookup, GetVoiceTagByNumber and 
                   GetVoiceTagList
04/24/03   kar     Added FormatPhoneNumberForDisplay and GetTimeString
02/10/03   kar     Added AVKType2ASCII
02/06/03   kar     Included assert.h and msg.h
01/31/03   kar     Added new macros
01/27/03   kar     Added functions to initialize and set radio buttons
01/08/03   kar     Removed unused functions
01/07/03   kar     Removed unused functions
01/07/03   kar     Initial revision

=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "assert.h"
#include "msg.h"
#include "AEEMenu.h"
#include "AEEAddrBook.h"
#include "AEEConfig.h"
#include "AEETime.h"

#include "AEEShell.h"

#ifdef FEATURE_PHONE_VR
#include "AEEVR.h"
#endif

#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


// ISOUND volume levels are in the range of 0..100.  However
// the Applet and OEM layer currently use the range 0 to OEMSOUND_MAX_VOL
// (currently 7).
//
// This macro performs the inverse operation of what ISOUND performs on the
// volume level before it calls the OEM set volume function.  Thus when a UI
// volume level is converted with this macro and passed to ISOUND, ISOUND
// will 'undo' the conversion and will pass the original value to the OEM
// layer.
//
// This kludge currently works because ISOUND does not range check volume
// levels to ensure they are within the range 0..100.
//


#define GET_ISOUND_VOL_LEVEL(vol) \
      ( ((vol) * AEE_MAX_VOLUME + AEE_MAX_VOLUME/2) / (int) OEMSOUND_MAX_VOL )


// The CheckAEEReturnStatus macro
#define CheckAEEReturnStatus(nReturnStatus)  ((nReturnStatus) == SUCCESS? TRUE: FALSE)

#ifdef _WIN32
#define __func__ __FUNCTION__
#endif

#define PRINT_FUNCTION_NAME() MSG_MED(__func__, 0, 0, 0)
//RVCT doesn't like the syntax in the next one
//#define PRINT_FUNCTION_NAMEN(n) MSG_MED(__func__ ": %d", n, 0, 0)
#define PRINT_FUNCTION_NAMEN(n) MSG_MED(__func__, 0, 0, 0)

#define PARAM_NOT_REF(x)

#define ASSERT_NOT_REACHABLE ASSERT(FALSE);

#define TEXT_CTL    1
#define MENU_CTL    2
#define SK_CTL      3
#define LIST_CTL    4

#define APPCOM_QVGA_DIM1 240
#define APPCOM_QVGA_DIM2 320
#define APPCOM_VGA_DIM1  480
#define APPCOM_VGA_DIM2  640
#define APPCOM_VGA_RES         0x00000001
#define APPCOM_QVGA_RES        0x00000002
#define APPCOM_SUB_QVGA_RES    0x00000003

/* Status of network scanning. */
#define APPCOMFUNC_NW_SEL_INACTIVE      0 /* No network scan in progress. */
#define APPCOMFUNC_NW_SEL_WLAN          1 /* WLAN scan in progress. */
#define APPCOMFUNC_NW_SEL_MAN_PLMN      2 /* Manual PLMN scan in progress. */
#define APPCOMFUNC_NW_SEL_MAN_PLMN_WRAT 3 /* Manual PLMN scan with RAT. */

#define EVT_USB_GENERIC_BUTTON_PRESS     1217
#define EVT_USB_VOICEDIAL_BUTTON_PRESS   1218

#define MAX_BROADCAST_MSG_SIZE 128

/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/
/*Tab Controller for dialogs */
// Supported controls.

typedef struct 
{
    
    // Number of controls used in the controller.
    uint16 numCtls; 

    // The index to current active control.
    int currentActiveCtl; 
    
    // The array of pointers to the controls.
    IControl** ctls; 

    // The array of control-types.
    int* ctlType; 
    
    //Decide whether to move up/down in the controls.
    int tabDirection; 

}DlgTabCtl;


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION:  ShowBusyIcon

DESCRIPTION:

PARAMETERS:
   *ps:
   *pd:
   *rc:
   bClear:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ShowBusyIcon(IShell *ps, IDisplay *pd, AEERect *rc, boolean bClear);

/*=============================================================================
FUNCTION:  SetDefaultSoftkeyLook

DESCRIPTION:

PARAMETERS:
   *ps:
   *pm:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm);

/*=============================================================================
FUNCTION:  SetDefaultMenuLook

DESCRIPTION:

PARAMETERS:
   *ps:
   *pm:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SetDefaultMenuLook(IShell *ps, IMenuCtl *pm);

/*=============================================================================
FUNCTION:  SetDefaultIconLook

DESCRIPTION:

PARAMETERS:
   *ps:
   *pm:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SetDefaultIconLook(IShell *ps, IMenuCtl *pm);


/*=============================================================================
FUNCTION:  InitMenuIcons

DESCRIPTION: Adds non-selected icons to every menu item in the
             specified menu.

PARAMETERS:
   *pm:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void InitMenuIcons(IMenuCtl *pm);


/*=============================================================================
FUNCTION:  SetMenuIcon

DESCRIPTION:

PARAMETERS:
   *pm:
   listId:
   bSel:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void SetMenuIcon(IMenuCtl   *pm,
                        uint16      listId,
                        boolean     bSel);
/*=============================================================================
FUNCTION:  InitCheckBox

DESCRIPTION: Adds non-selected icons to every menu item in the
             specified menu.

PARAMETERS:
   *pm:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void InitCheckBox(IMenuCtl *pm);

/*=============================================================================
FUNCTION:  SetCheckBoxItem

DESCRIPTION: Sets Check Box Items on/off

PARAMETERS:
   *pm:
   listId:
   bSel:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SetCheckBoxItem(IMenuCtl   *pm,
                 uint16      listId,
                 boolean     bSel);

/*=============================================================================
FUNCTION:  GetCheckBoxVal

DESCRIPTION:

PARAMETERS:
   *pm:
   listId:

RETURN VALUE:
   boolean

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean GetCheckBoxVal(IMenuCtl   *pm,
                       uint16      listId);


/*=============================================================================
FUNCTION: AVKType2ASCII

DESCRIPTION:
   Converts a {AVK_0..AVK_9,AVK_STAR,AVK_POUND} to the
   ASCII equivalent

PARAMETERS:
   key: BREW keycode

RETURN VALUE:
   char: ASCII value

COMMENTS:
   Returns the NULL character if the input value is out of range

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern char AVKType2ASCII(AVKType key);

/*=============================================================================
FUNCTION: DoNumberLookup

DESCRIPTION:  Searches the Contacts database for a name match for the
              specified phone number.

PARAMETERS:
   *pa     [in]:  IAddrBook interface pointer
   *number [in]:  Phone number to lookup
   *name: [out]:  Destination buffer for the name
   size:   [in]:  Size of name in bytes

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void DoNumberLookup(IAddrBook *pa,
                           AECHAR    *number,
                           AECHAR    *name,
                           uint32     size);

/*=============================================================================
FUNCTION: DoSpeedDialLookup

DESCRIPTION:  Searches the Contacts database in the speed dial list category
              for a contact number match for the the speed dial index.

PARAMETERS:
   *pa             [in]:  IAddrBook interface pointer
   *speeddialindex [in]:  index number in the speed dial list range 1 - 99
   *number         [out]:  Phone number to lookup
   size:           [in]:  Size of name in bytes

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void DoSpeedDialLookup(IAddrBook *pa,
                              AECHAR *speeddialindex,
                              AECHAR *number,
                              uint32 size);

/*=============================================================================
FUNCTION: FormatPhoneNumberForDisplay

DESCRIPTION:  Formats a phone number for display.

PARAMETERS:
  *number  [in]: A purely numeric phone number  (ie, "8585551234")

RETURN VALUE:
   AECHAR const *: A formatted phone number for display (ie. "858-555-1234")

COMMENTS:
   The returned string is contained in a dynamically allocated buffer, that the
   caller must release using FREE().

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AECHAR *FormatPhoneNumberForDisplay(AECHAR const *number);

/*=============================================================================
FUNCTION:  GetTimeString

DESCRIPTION:

PARAMETERS:
   *ps:
   *pm:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern void GetTimeString(IConfig *cfg, ITimeCtl *tctl, IShell *ish,
                          uint32 timeVal, AECHAR *timeStr);


#ifdef FEATURE_PHONE_VR
/*=============================================================================
FUNCTION: DoVoiceTagLookup

DESCRIPTION:  Searches the Contacts database in the speed dial list category 
              for the given VoiceTag.

PARAMETERS:
   *pa             [in]:  IAddrBook interface pointer
   wUtt            [in]:  Voice Tag to be searched for
   *szName         [out]: Name of contact
   namesize:       [in]:  Size of szName in bytes
   *szNum          [out]: Phone number of contact
   numsize:        [in]:  Size of szNum in bytes

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern boolean DoVoiceTagLookup(IAddrBook *pa, 
                      AEEVRUtterance wUtt, 
                      AECHAR *szName, 
                      uint32 nameSize,
                      AECHAR *szNum,
                      uint32 numSize);

/*=============================================================================
FUNCTION: GetVoiceTagByNumber

DESCRIPTION:  Searches the Contacts database in the speed dial list category 
              for the given Number

PARAMETERS:
   *pa             [in]:  IAddrBook interface pointer
   *szNum          [in]:  Phone number to be searched
   pUtt            [out]: Voice Tag 
   *szName         [out]: Name of contact
   namesize:       [in]:  Size of szName in bytes

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern boolean GetVoiceTagByNumber(IAddrBook *pa, 
                            AECHAR *szNum,
                            AEEVRUtterance *pUtt, 
                            AECHAR *szName, 
                            uint32 nameSize);

/*=============================================================================
FUNCTION: GetVoiceTagList

DESCRIPTION:  Gets the list of all voice tags

PARAMETERS:
   *pa             [in]:  IAddrBook interface pointer
   list            [out]: Voice tags
   listsize        [in]:  Size of Voice Tag array in bytes

RETURN VALUE:
   Number of voice tags found

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
extern uint16 GetVoiceTagList(IAddrBook *pa, 
                       AEEVRUtterance *list, 
                       uint32 listsize);

#endif /* FEATURE_PHONE_VR */

/*=============================================================================
FUNCTION: OriginateVoiceCall

DESCRIPTION:  Originate voice call

PARAMETERS:
   number - number to be dialed

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
  This function will be called from another apps context.
  The number buffer must be NULL terminated.

SIDE EFFECTS:
  None

=============================================================================*/
int OriginateVoiceCall (const char* number);

/*=============================================================================
FUNCTION: AnswerVoiceCall

DESCRIPTION:  Answers voice call if there is one to answer and does nothing if
              not

PARAMETERS:
  boolean: TRUE will answer call and FALSE will reject

RETURN VALUE:
   SUCCESS - if successfully sent to Dialer.  Does not mean it was ansewered.

COMMENTS:
  This function will be called from another apps context.

SIDE EFFECTS:
  None

=============================================================================*/
int AnswerVoiceCall(boolean bAnswer);
#ifdef FEATURE_WMS_APP
/*=============================================================================
FUNCTION: OriginateMessage

DESCRIPTION:  Originate a Message (SMS) to a phone number or EMail Address


PARAMETERS:
   isEmail - boolean If Email Address
   Address - Phone Number or Email Address

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
  This function will be called from another apps context.
  The Address buffer must be NULL terminated.

SIDE EFFECTS:
  None

=============================================================================*/
int OriginateMessage (const boolean isEmail,
                      const char* Address);
#endif /* FEATURE_WMS_APP */


/* Tab Controller functions */
//Tab Controller functions - used by other Dialogs.

/*===========================================================================
FUNCTION TabController_Init

DESCRIPTION
 Init function, initialized the controller, must pass the correct value of the
 number of controls to be controlled.


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void TabController_Init(DlgTabCtl **pDlgTabCtl, uint16 numControls);

/*===========================================================================
FUNCTION TabController_AddControl

DESCRIPTION
 Add control to the controller. Must have called TabController_Init function with
 correct number of controls before addition.
  

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void TabController_AddControl(DlgTabCtl *pDlgTabCtl, int CtlType, IControl *pIControl);

/*===========================================================================
FUNCTION TabController_SetActiveEx

DESCRIPTION
 Set a particular control active and all other controls inactive(Read: Set Active Exclusive")
 To be called after the controller is initialized, the controls added and you want
 a particular control to be in focus at the start.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void TabController_SetActiveEx(DlgTabCtl *pDlgTabCtl, int index);

/*===========================================================================
FUNCTION TabController_HandleEvent

DESCRIPTION
   The event handler for the TabController.
 
The valid set of events.

    1. List Control :   Up/Down is tab and Left/Right moves Up/Down in  the menu.
    2. SK Control   :   Up/Down is tab.
    3. Text Control :   Up/Down is tab and Select moves to next control.
    4. Menu Control :   Up/Down moves inside the menu control and Left/Right is tab.
    
    All other events are ignored.

    The "tab-direction" is stored in Key_Press event and executed in Key_Release event.
    This storage is required for correct operation of the text controls.
MSMULATE_VERBOSE_STUB 
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void TabController_HandleEvent(DlgTabCtl** pDlgTabCtl, AEEEvent evt, uint16 wParam);

/*=============================================================================
FUNCTION: IsCallActive

DESCRIPTION:  Returns if UI is in a call

PARAMETERS:
   call type - type of call being queried
   Passing in AEECM_CALL_TYPE_NONE 
   for call type queries for all call types

RETURN VALUE:
   TRUE - if in a call
   FALSE - otherwise

COMMENTS:
   None

SIDE EFFECTS:
  None

=============================================================================*/
boolean IsCallActive (AEETCallType calltype);

/*=============================================================================
FUNCTION: SetIdleMenuText

DESCRIPTION:  Sets text in the static control of the idle menu.

PARAMETERS:
   AECHAR * to the text to be displayed

RETURN VALUE:
   None.

COMMENTS:
   None

SIDE EFFECTS:
  Suspends any other app and brings up Idle Menu if non-null text.

=============================================================================*/
boolean SetIdleMenuText (AECHAR *pszText);
/*=============================================================================
FUNCTION: SetIdleMenuTextInBkgrnd

DESCRIPTION:  Sets text in the static control of the idle menu.

PARAMETERS:
   AECHAR * to the text to be displayed

RETURN VALUE:
   TRUE if event was sent to CoreApp. (Not if the text was set)
   FALSE if otherwise.

COMMENTS:
   None

SIDE EFFECTS:
  Updates the Idle menu dlg only if the text is not null

=============================================================================*/
boolean SetIdleMenuTextInBkgrnd (AECHAR *pszText);


/*=========================================================================
FUNCTION
  AppCom_ConvertGSMDefaultToASCII

DESCRIPTION
  Convert 7-bit GSM Default alphabet string to ASCII

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void AppCom_ConvertGSMDefaultToASCII
(
  const uint8   *gsmdef,
  uint32        len,
  uint8         *ascii
);

/*=========================================================================
FUNCTION
  AppCom_ConvertGSMDefaultToASCII

DESCRIPTION
  Convert 7-bit packed array into an ASCII string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
byte AppCom_ConvertPacked7BitToASCII
(
  const uint8 *ud,             /* IN */
  uint32      ud_len,          /* IN */
  uint32      buf_len,         /* IN */
  uint8       *buf             /* OUT */
);

/*=========================================================================
FUNCTION
  AppCom_ConvertPacked7BitToGSMDefault

DESCRIPTION
  Unpack raw data to GSM 7-bit characters.

DEPENDENCIES
  None

RETURN VALUE
  Number of 7-bit charactes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint32 AppCom_ConvertPacked7BitToGSMDefault
(
  const uint8       *in,           /* Source buffer IN */
  uint32            in_len,        /* Number of 7-bit characters in source buf IN */
  uint32            out_len,       /* output buffer length IN */
  uint16            shift,         /* offset from which the data will start IN */
  uint8             *out           /* OUT */
);
/*=============================================================================
FUNCTION: DisplayString

DESCRIPTION:  displays the string on Dialer

PARAMETERS:
   pszBuf - string to be displayed

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
  This function will be called from another apps context.
  The number buffer must be NULL terminated.

SIDE EFFECTS:
  None

=============================================================================*/
int DisplayString (const AECHAR* pszBuf);

#define ui_is_in_call()         IsCallActive(AEET_CALL_TYPE_NONE)
#define ui_curr_call_is_sms()   IsCallActive(AEET_CALL_TYPE_SMS)
#define ui_curr_call_is_data()  IsCallActive(AEET_CALL_TYPE_CS_DATA)

/*=============================================================================
FUNCTION: SetBacklight

DESCRIPTION:  Sets the backlight on or off

PARAMETERS:
   onOff - backlight setting

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
  This function will be called from another apps context.
  After the backlight turns off, the backlight duration value willn go back to the original NV value.

SIDE EFFECTS:
  None

=============================================================================*/
int SetBacklight(boolean onOff);

/*=============================================================================
FUNCTION: ClearBlockUITask

DESCRIPTION:  Clears the signal we are going to block on.

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   None.

SIDE EFFECTS:
  None

=============================================================================*/
void ClearBlockUITask(void);

/*=============================================================================
FUNCTION: BlockUITask

DESCRIPTION:  Blocks the UI Task until a signal is received.

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
  This function blocks the entire UI task waiting for UnBlockUITask
  to be called.

SIDE EFFECTS:
  None

=============================================================================*/
void BlockUITask(void);

/*=============================================================================
FUNCTION: UnblockUITask

DESCRIPTION:  Sets the signal to free the UI task.

PARAMETERS:
   onOff - backlight setting

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
  This function will be called from another task's context.
  After the UI task is blocked using BlockUITask, we need this 
  signal to unblock it.

SIDE EFFECTS:
  None

=============================================================================*/
void UnblockUITask(void);

/*=============================================================================
FUNCTION: GetPrimaryScreenSize

DESCRIPTION:  Determines the size of the primary display

PARAMETERS:
   None

RETURN VALUE:
   One of the following values.
     COREAPP_VGA_RES
     COREAPP_QVGA_RES
     COREAPP_SUB_QVGA_RES
   If the screen size cannot be determined, the default value will be
   COREAPP_QVGA_RES.

COMMENTS:
  None

SIDE EFFECTS:
  None

=============================================================================*/
uint32 GetPrimaryScreenSize(void);

/*===========================================================================  
FUNCTION AppComFunc_SelectFonts

DESCRIPTION
  Sets up the application font data and sets the correct fonts for the
  primary display

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void AppComFunc_SelectFonts(IDisplay* pIDisp);

/*===========================================================================  
FUNCTION AppComFunc_DisplaySelectNetworkDialog

DESCRIPTION
  Signals coreapp to launch the select network dialog.

DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
void AppComFunc_DisplaySelectNetworkDialog(IShell* pIShell, uint32 dwParam);

#endif /* APPCOMFUNC_H */

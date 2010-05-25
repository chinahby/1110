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

  $Header: //depot/asic/msm6300/apps/Core/AppComFunc.h#1 $
$DateTime: 2003/09/18 13:54:35 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
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
#include "AEEMenu.h"
#include "AEEAddrBook.h"
#include "AEEConfig.h"
#include "AEETime.h"

#include "AEEShell.h"

#ifdef FEATURE_PHONE_VR
#include "AEEVR.h"
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
#define CheckAEEReturnStatus(nReturnStatus)  (nReturnStatus == SUCCESS? TRUE: FALSE)

#ifdef _WIN32
#define __func__ __FUNCTION__
#endif

#define PRINT_FUNCTION_NAME() MSG_LOW(__func__, 0, 0, 0)

#define PARAM_NOT_REF(x)

#ifndef WIN32
#define ASSERT_NOT_REACHABLE ASSERT(FALSE);
#else
#define ASSERT(FALSE)
#define ASSERT_NOT_REACHABLE ASSERT(FALSE);
#endif
#define TEXT_CTL    1
#define MENU_CTL    2
#define SK_CTL      3
#define LIST_CTL    4

#define CLOCKCTL_PROP_NOEDIT_AMPM 1
/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/
/*Tab Controller for dialogs */
// Supported controls.
typedef enum {
    CLOCKCTL_FOCUS_HOUR,
    CLOCKCTL_FOCUS_MIN,
    CLOCKCTL_FOCUS_AMPM,
    CLOCKCTL_FOCUS_NONE
}ClockCtlFocus;


typedef struct _ClockCtl {
   // XXX: This fields should not be accessed directly by anybody but
   //      the ClockCtl functions.
    uint32         m_dwDispTime;
    ClockCtlFocus  m_focus;
    boolean        m_largeFnt;
    uint32         m_props;
    uint16         m_minInc;
    IConfig       *m_pcfg;
    ITimeCtl      *m_pt;
    IDisplay      *m_pd;
    IShell        *m_ps;
}ClockCtl;

typedef struct 
{
    
    // Number of controls used in the controller.
    int numCtls; 

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
void TabController_Init(DlgTabCtl **pDlgTabCtl, int numControls);

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
FUNCTION: ClockCtl_Init

DESCRIPTION: Initializes a ClockCtl object

PARAMETERS:
   *ctl: [in] ClockCtl object
pt: [in] ITimeCtl to emulate
pd: [in] IDisplay interface
ps: [in] IShell interface
bLargeFnt: [in] Use a large font?


RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ClockCtl_Init(ClockCtl *ctl, 
                   ITimeCtl *pt, 
                   IDisplay *pd, 
                   IShell *ps, 
                   boolean bLargeFnt);

/*=============================================================================
FUNCTION: ClockCtl_Free

DESCRIPTION:  Releases any memory associated with the ClockCtl object

PARAMETERS:
   *ctl: [in] ClockCtl object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ClockCtl_Free(ClockCtl *ctl);

/*=============================================================================
FUNCTION: ClockCtl_SetFocus

DESCRIPTION: Sets the focus of the ClockCtl object

PARAMETERS:
   *ctl: [in] ClockCtl object
   focus: [in] The new focus

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ClockCtl_SetFocus(ClockCtl *ctl, ClockCtlFocus focus);


/*=============================================================================
FUNCTION: ClockCtl_SetProperties

DESCRIPTION: Set properties for the ClockCtl object

PARAMETERS:
   *ctl: [in] ClockCtl object
   props: [in] new properties

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ClockCtl_SetProperties(ClockCtl *ctl, uint32 props);

/*=============================================================================
FUNCTION: ClockCtl_SetIncrement

DESCRIPTION: Sets the number of minutes by which the minutes field of 
             the text control is incremented or decremented when the user 
             presses the UP or DOWN arrow keys while editing the field 
             (the default value is one minute). 

PARAMETERS:
   *ctl: [in] ClockCtl object
   wMins: [in] new increment value 

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ClockCtl_SetIncrement(ClockCtl *ctl, uint16 wMins);

/*=============================================================================
FUNCTION: ClockCtl_GetTime

DESCRIPTION: Returns the current time of the ClockCtl object

PARAMETERS:
   *ctl: [in] ClockCtl object

RETURN VALUE:
   uint32: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 ClockCtl_GetTime(ClockCtl *ctl);


/*=============================================================================
FUNCTION: ClockCtl_SetTime

DESCRIPTION: Sets the time of the ClockCtl object

PARAMETERS:
   *ctl: [in] ClockCtl object
   time:  [in] New time

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ClockCtl_SetTime(ClockCtl *ctl, uint32 time);


/*=============================================================================
FUNCTION: ClockCtl_Redraw

DESCRIPTION: This function uses the Time Control to get the time string, edits
             it to take care of the Chinese language, and prints it on the 
             screen depending on the font size 

PARAMETERS:
   *ctl: [in] ClockCtl object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ClockCtl_Redraw(ClockCtl *ctl);

/*=============================================================================
FUNCTION: ClockCtl_HandleKeyEvent

DESCRIPTION: Handles a key event for the ClockCtl object

PARAMETERS:
   *ctl: [in] ClockCtl object
   key: [in] The key to be handled

RETURN VALUE:
   boolean:  TRUE if the key was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean ClockCtl_HandleKeyEvent(ClockCtl *ctl, AVKType key);


/* End of Tab Controller functions */

#endif /* APPCOMFUNC_H */
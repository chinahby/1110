#ifndef COREMENU_H
#define COREMENU_H
/*===========================================================================

FILE: CoreMenu.h

DESCRIPTION
   This file is the header file for CoreMenu.c.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   Functions in this file is being called by CoreApp.c.

       Copyright © 2002 - 2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/CoreMenu.h#47 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------.
03/07/07   jas     Reimplement background images with overlays.
                   Support VGA background images.
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/11/06   RT      Removed prototypes of SIO config related event handlers
11/10/06   jmb     Added support for IMMOverlay management of BREW bitmap
10/09/06   ypmw    Add multi-port for data services
09/19/06   ypmw	   Modified plmn network scan dialog
09/08/06   jas     Removing menus that were related to the NetSettings app
08/23/06   jks     Added support for WLAN Adhoc networks
07/20/06   sj      Mainlining 6500 (5.3) Coreapp changes
05/10/06   jas     Modifying the handling of the background image to work on 7x00
02/13/06   jas     Added support for GCF language selection
12/05/05   jks     Added support for WLAN Network Scan Mode Select, added dynamic menus support 
11/21/05   jas     Support for disabling background images
08/31/05   sun     Added support for BT-SAP
04/20/05   jas     Adding support for WPA WLAN security
04/15/05   jas     Reworked WLAN security menus
02/22/05   jas     Additional changes for WLAN
12/06/04   BW      Added LAN menu and related submenu
11/12/04   ank,cvs Dualslot changes from 6300
09/22/04   cvs     Add Contrast menu, lint fixes
09/02/04   tml     Added CPHS support and added revision history comment
06/07/04   tml     Fixed compilation warning
06/07/04   sun     Added support for Manual PLMN Selection and  LCD Display
05/13/04   sun     Added LPM Changes
11/25/03   ram     Add broadcast text to idle menu and CUG Menu support
11/03/03   ram     Added PLMN selection menu.
09/21/03   tml     ICM support
08/08/03   tml     Dual Slot support
08/08/03   sun     Added new CheckBoxDlgEventHandler
07/10/03   bpw     Removed numedit dialog.
05/08/03   kar     System menu changes
05/06/03   tml     Added CoreMenuTranslateResourceIDToSetting
02/10/03   kar     Added Numedit dialog
01/31/03   ic      Added Extras -> Call Info and Settings -> Network dialogs.
10/08/02   tml     code review changes
10/02/02   kar     Code cleanup
09/19/02   tml     Removed Corefeatures.h
09/16/02   AAA     Added Multimedia Ringer support
08/21/02  js/tml   Support for ICARD integration
08/13/02   kar     Core menu header
===========================================================================*/

/*===========================================================================

                               INCLUDES

===========================================================================*/
#include "CoreApp.h"
#include "CoreUtils.h"
#ifdef FEATURE_OVERLAY
#include "IOverlay.h"
#endif

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
/* Language selection values. */
#define COREAPP_LANG_SEL_ENG 0
#define COREAPP_LANG_SEL_GER 1

#if defined(FEATURE_GSTK)
/* Size of the string to be sent to ISTK for language selection. */
#define COREAPP_ISTK_LENG_SEL_LEN 2
#endif /* defined(FEATURE_GSTK) */

#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

/*-------------------------------------------------------------------

                  Exported function prototypes

-------------------------------------------------------------------*/

/*===========================================================================
FUNCTION WriteSetting

DESCRIPTION
  Save the setting of given item through Set methods.
  Return TRUE if the setting was changed and FALSE otherwise.

DEPENDENCIES
  Various SetXXXSetting functions

SIDE EFFECTS
  None
===========================================================================*/
boolean WriteSetting(CCoreApp *pMe, uint16 wDlgID, uint16 wItemID);

/*===========================================================================
FUNCTION CreateIdleMenuDlg

DESCRIPTION
  create the idle menu dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CreateIdleMenuDlg(CCoreApp *pMe);

/*===========================================================================
FUNCTION CreateLPMDlg

DESCRIPTION
  create the idle menu dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CreateLPMDlg(CCoreApp *pMe);

/*===========================================================================
FUNCTION CreateKeyGuardOnDlg

DESCRIPTION
  Start the sequences of keyguard dialogs

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CreateKeyGuardOnDlg(CCoreApp * pMe);



/*===========================================================================
FUNCTION Security_Validate

DESCRIPTION
  Security_validate will initiate the security process.

  The Security_validate will begin by requesting the current hardware status
  by calling ICARD_GetStatus.  If ICARD_GetStatus returns, AEECARD_NO_CARD
  or AEECARD_INVALID_CARD an error will result.

  If ICARD_GetStatus returns AEECARD_VALID, Security_validate will process
  the remaining members of the status structure to determine the lock state.
  ICARD_GetStatus will return 1 of 6 lock states: AEECARD_PIN_UNKNOWN,
  AEECARD_PIN_DISABLED, AEECARD_PIN_ENABLED, AEECARD_PIN_BLOCKED,
  AEECARD_PIN_PERM_DISABLED, AEECARD_PIN_NOT_VALID.

  If ICARD_GetStatus returns AEECARD_PIN_UNKNOWN, AEECARD_PIN_PERM_DISABLED,
  or AEECARD_PIN_NOT_VALID, an error will be generated.

  If ICARD_GetStatus returns AEECARD_PIN_DISABLED, Security_validate shall
  allow access to the Security Menu.

  If ICARD_GetStatus returns Security_display will display the appropriate
  screen then register the corresponding event handler for that screen.
  AEECARD_PIN_ENABLED or AEECARD_PIN_BLOCKED, Security_validate will present
  the appropriate IDialog based on the functional area being accessed.  For
  example, attempting to access the Restrict menu item will prompt for PIN2
  validation.  The ICARD_VerifyPin function will validate both PIN1 and PIN2.


DEPENDENCIES
  CoreApp.c has populated initial values of pMe.


RETURN VALUE
  SUCCESS/EFAILED

SIDE EFFECTS
  None.

===========================================================================*/
void Security_Validate(CCoreApp *pMe);



/*===========================================================================
FUNCTION CheckPIN2Required

DESCRIPTION
  CheckPIN2Required will initiate the PIN2 security process.

  The CheckPIN2Required will begin by requesting the current hardware status
  by calling ICARD_GetStatus.  If ICARD_GetStatus returns, AEECARD_NO_CARD
  or AEECARD_INVALID_CARD an error will result.

  If ICARD_GetStatus returns AEECARD_VALID, Security_validate will process
  the remaining members of the status structure to determine the lock state.
  ICARD_GetStatus will return 1 of 6 lock states: AEECARD_PIN_UNKNOWN,
  AEECARD_PIN_DISABLED, AEECARD_PIN_ENABLED, AEECARD_PIN_BLOCKED,
  AEECARD_PIN_PERM_DISABLED, AEECARD_PIN_NOT_VALID.

  If ICARD_GetStatus returns AEECARD_PIN_UNKNOWN, AEECARD_PIN_PERM_DISABLED,
  or AEECARD_PIN_NOT_VALID, an error will be generated.

  If ICARD_GetStatus returns AEECARD_PIN_DISABLED, Security_validate shall
  allow access to the Security Menu.

  If ICARD_GetStatus returns Security_display will display the appropriate
  screen then register the corresponding event handler for that screen.
  AEECARD_PIN_ENABLED or AEECARD_PIN_BLOCKED, Security_validate will present
  the appropriate IDialog based on the functional area being accessed.  For
  example, attempting to access the Restrict menu item will prompt for PIN2
  validation.  The ICARD_VerifyPin function will validate both PIN1 and PIN2.


DEPENDENCIES
  CoreApp.c has populated initial values of pMe.


RETURN VALUE
  SUCCESS/EFAILED

SIDE EFFECTS
  None.

===========================================================================*/
void CheckPIN2Required(CCoreApp *pMe);

/*===========================================================================
FUNCTION IdleMenuDlgEventHandler

DESCRIPTION
  handle idle menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean IdleMenuDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);


/*===========================================================================
FUNCTION MenuDlgEventHandler

DESCRIPTION
  handle generic menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean MenuDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION RadioButtonDlgEventHandler

DESCRIPTION
  Handle radio button event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean RadioButtonDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION CheckBoxDlgEventHandler

DESCRIPTION
  Handle Check Box Dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CheckBoxDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION RestrictCheckBoxDlgEventHandler

DESCRIPTION
  Handle Restrict menu check box event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean RestrictCheckBoxDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION SoundRadioButtonDlgEventHandler

DESCRIPTION
  handle sound radio button dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SoundRadioButtonDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);



/*===========================================================================
FUNCTION SoundRadioButtonRingTypeDlgEventHandler

DESCRIPTION
  handle sound ringer button dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SoundRadioButtonRingTypeDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION StaticDlgEventHandler

DESCRIPTION
  handle static dlg events

DEPENDENCIES
  InitStaticDlg, CreateTextEditDlg,

SIDE EFFECTS
  None
===========================================================================*/
boolean StaticDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION BannerDlgEventHandler

DESCRIPTION
  handle banner editing dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean BannerDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);


/*===========================================================================
FUNCTION ContrastDlgEventHandler

DESCRIPTION
  handle contrast dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ContrastDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);



/*===========================================================================
FUNCTION TimeDateDlgEventHandler

DESCRIPTION
  time date edit dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean TimeDateDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);



/*===========================================================================
FUNCTION KeyGuardDlgEventHandler

DESCRIPTION
  handle keyguard radio button dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean KeyGuardDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);



/*===========================================================================
FUNCTION KeyGuardOnDlgEventHandler

DESCRIPTION
  handle keyguard ON dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean KeyGuardOnDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);



/*===========================================================================
FUNCTION SecurityDlgEventHandler

DESCRIPTION
  handle security menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SecurityDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);



/*===========================================================================
FUNCTION RestoreSettingsNowDlgEventHandler

DESCRIPTION
  handle restore setting events

DEPENDENCIES
  StartRestoring

SIDE EFFECTS
  None
===========================================================================*/
boolean RestoreSettingsNowDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);



/*===========================================================================
FUNCTION PINChangeDlgEventHandler

DESCRIPTION
  handle PIN Change dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean PINChangeDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);


/*===========================================================================
FUNCTION EnablePinDlgEventHandler

DESCRIPTION
  handle Entering PIN from radio button screen dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean EnablePinDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);


/*===========================================================================
FUNCTION TCTestModeDlgEventHandler

DESCRIPTION
  handle TC Test Mode dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean TCTestModeDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION PLMNListDlgEventHandler

DESCRIPTION
  Handle PLMN List Dialog Events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean PLMNListDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION PLMNSearchingEventHandler

DESCRIPTION
  Handle PLMN Searching Dialog Events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean PLMNSearchingDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);
/*===========================================================================
FUNCTION CUGGroupIDDlgEventHandler

DESCRIPTION
  Handle CUG Group ID Dialog Events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CUGGroupIDDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
/*===========================================================================
FUNCTION HideSplash

DESCRIPTION
  This function remove the splash image display

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean HideSplash(CCoreApp *pMe);

/*===========================================================================
FUNCTION SelectCardDlgEventHandler

DESCRIPTION
  Handle radio button event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SelectCardDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION SelectCardDlgEventHandler

DESCRIPTION
  Handle radio button event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SelectTechDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION DetermineNextCardState

DESCRIPTION
  Determine the card state based on card info
  This function delegates the determination to single and dual slot specific
  functions


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int DetermineNextCardState(CCoreApp *pMe,boolean bAllowByPass);
/*===========================================================================
FUNCTION LPMDlgEventHandler

DESCRIPTION
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean LPMDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*===========================================================================
FUNCTION DisplayPLMNListAgain

DESCRIPTION
  

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void DisplayPLMNListAgain(CCoreApp *pMe);

#endif
/*===========================================================================
FUNCTION ScanNetworkDlgEventHandler

DESCRIPTION
  

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ScanNetworkDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*===========================================================================
FUNCTION SearchManualPLMNList

DESCRIPTION
  

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void SearchManualPLMNList(CCoreApp *pMe);
#endif /*#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)*/

/*===========================================================================
FUNCTION ALSLineSelectDlgEventHandler

DESCRIPTION
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ALSLineSelectDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

#endif /* COREMENU_H */

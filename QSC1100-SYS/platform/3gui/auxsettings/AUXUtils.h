#ifndef AUXUTILS_H
#define AUXUTILS_H
/*===========================================================================

FILE: AUXUtils.h

DESCRIPTION
   Header file for AUXUtils.c

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:


       Copyright © 2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/07   sg      Support for setting BREW Secure Clock 
09/24/07   snb     Provide option to exit from USB MTP mode.
02/23/07   jas     Refactoring for HDK builds
02/09/07   jas     Add method to determine if the phone is online
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
11/1/06    RT      Initial Revision

===========================================================================*/


/*=============================================================================

                   INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "AUXSettings.h"
#include "AEE.h"
#ifdef FEATURE_ANNUNCIATOR
#include "AEEAnnunciator.h"
#endif /* FEATURE_ANNUNCIATOR */

#include "err.h"
#include "msg.h"

#define AUX_SELECTIONVERIFICATIONTIME        2000

#ifdef FEATURE_WLAN
#define AUXAPP_WLAN                          0x04

/* We don't have IAnnunciator, so we'll need to use local defines
 * for the numbers of bars.
 */
#ifdef FEATURE_ANNUNCIATOR
#define AUX_WLAN_NO_BARS       ANNUN_STATE_RSSI_0
#define AUX_WLAN_ONE_BAR       ANNUN_STATE_RSSI_1
#define AUX_WLAN_TWO_BARS      ANNUN_STATE_RSSI_2
#define AUX_WLAN_THREE_BARS    ANNUN_STATE_RSSI_3
#define AUX_WLAN_FOUR_BARS     ANNUN_STATE_RSSI_4
#else /* FEATURE_ANNUNCIATOR */
#define AUX_WLAN_NO_BARS      1
#define AUX_WLAN_ONE_BAR      2
#define AUX_WLAN_TWO_BARS     3
#define AUX_WLAN_THREE_BARS   4
#define AUX_WLAN_FOUR_BARS    5
#endif /* FEATURE_ANNUNCIATOR */

#endif /* FEATURE_WLAN */



/*=============================================================================

                   MACRO DECLARATION

=============================================================================== */

#define AUX_ERR(format, code1, code2, code3) MSG_ERROR(format, code1, code2, code3)
#define AUX_MSG(format, code1, code2, code3) MSG_MED(format, code1, code2, code3)

/*=============================================================================

                  FUNCTION DECLARATION

=============================================================================== */

/*===========================================================================
FUNCTION AUXDisplayMessageDlg

DESCRIPTION
  Create a message dialog based in resource id of string
  if nDisplaySeconds is zero, dialog will only clear on key input

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXDisplayMessageDlg(AUXApp *pMe, uint16 nStringID, int nDisplaySeconds);

/*===========================================================================
FUNCTION AUXEndMessageDlg

DESCRIPTION
  End a message dialog

DEPENDENCIES
  None

SIDE EFFECTS
  Send an EVT_MESSAGE_END event
===========================================================================*/
void AUXEndMessageDlg(AUXApp *pMe);

/*===========================================================================
FUNCTION AUXEndCurrentDlg

DESCRIPTION
  End the current dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXEndCurrentDlg(AUXApp *pMe);

/*===========================================================================
FUNCTION AUXSetGeneralStaticTextStyle

DESCRIPTION
  Set the standard textctl dialog style

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXSetGeneralStaticTextStyle(IStatic *pIStaticText);

/*===========================================================================
FUNCTION AUXSetStaticTextCtlSize

DESCRIPTION
  Set the rect for static control when there are one other control presented
  in the dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXSetStaticTextCtlSize(IStatic *pICurrentStatic, IControl *pIOtherCtrl);

#ifdef FEATURE_WLAN
/*===========================================================================
FUNCTION AUXIsHexWString

DESCRIPTION
  This function determines whether or not a given string contains only
  characters that represent hexadecimal numbers.  This routine assumes
  that the input is a correctly-formed, NULL-terminated string.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXIsHexWString(AECHAR *pszwHexString);

/*===========================================================================
FUNCTION AUXWATOI

DESCRIPTION
  Takes a AECHAR string and turns the result into an integer.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int AUXWATOI(const AECHAR *str);

/*===========================================================================
FUNCTION AUXConvertWStrToBinBuf

DESCRIPTION
  Converts an ASCII wide string of hexadecimal characters to a buffer of
  binary data.  Note that, since each character translates to 4 bits, the
  input character buffer must be of even length for the translation to work.
  e.g. Converts the wide string "1A2B" into two octets with values 0x1A 0x2B.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  Number of bytes populated in pBuf if successful.  (If the buffer provided
    is larger than necessary, only the returned number of bytes will be valid.)
  -1 otherwise.

COMMENTS
  This routine expects hexadecimal characters only, but it does perform
  all necessary error checking.  Upper- and lower-case hex values are
  both accepted.
===========================================================================*/
int AUXConvertWStrToBinBuf(const AECHAR *pszwIn, uint8 *pBuf, uint32 nBufLen);

/*===========================================================================
FUNCTION AUXDBToLevel

DESCRIPTION
  Converts RSSI absolute DB value to a number of bars

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int AUXDBToLevel (int nDBVal);

/*===========================================================================
FUNCTION AUX_PhoneIsOnline

DESCRIPTION
  This function returns a boolean, indicating whether the phone is in
  the online state

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUX_PhoneIsOnline(IShell *pIShell);

#endif /*#ifdef FEATURE_WLAN*/

/*===========================================================================
FUNCTION AUXPopDlg

DESCRIPTION
  Pop dialog stack until the specified dialog. 
  If not found, go back till the core dialog.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXPopDlg(AUXApp* pMe, uint16 wPopDlgID);

/*===========================================================================
FUNCTION AUXDisplayIdleMenuDlg

DESCRIPTION
  Bring the program to the Idle menu screen display

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXDisplayIdleMenuDlg(AUXApp *pMe);


#if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP)
/*===========================================================================
FUNCTION AUXGetUSBDevice

DESCRIPTION
  Get USB Device from USBDC_API  Errors return USBDC_SHARED_DEVICE_COUNT

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 AUXGetUSBDevice(AUXApp *pMe);

/*===========================================================================
FUNCTION AUXSetUSBDevice

DESCRIPTION
  Set Sound Device in uixsnd.  SND_DEVICE_MAX  indicates audoselect.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXSetUSBDevice(AUXApp *pMe, uint64 nNewVal);

#endif /* FEATURE_RUNTIME_DEVMAP && FEATURE_USB_DEFAULT_MTP */

/*===========================================================================
FUNCTION AUXSetTimeofDay

DESCRIPTION
  Sets the secure time

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if successful
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXSetSecureTimeofDay (AUXApp *pMe, JulianType *julian);

/*===========================================================================
FUNCTION AUXGetTimeofDay

DESCRIPTION
  Returns the current secure time

DEPENDENCIES
  none

RETURN VALUE
  TRUE - if time is valid
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXGetSecureTimeofDay (AUXApp *pMe, JulianType *julian_date);

/*===========================================================================
 FUNCTION AUXFormatTimeDate

 DESCRIPTION
  Formats the time and the date

 DEPENDENCIES
  AUXFormatTime, AUXFormatDate

SIDE EFFECTS
  None
===========================================================================*/
int AUXFormatTimeDate(JulianType j_time, AECHAR *pszDest,  int nLen);
#endif /* AUXUTIL_H */

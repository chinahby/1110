/*===========================================================================

FILE: AUXUtils.c

DESCRIPTION
   This file contains utility functions for AUX app.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:


       Copyright © 2007 QUALCOMM Incorporated.
                 All Rights Reserved.
               QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/07   sg      Support for setting the BREW secure clock
09/24/07   snb     Provide option to exit from USB MTP mode.
02/23/07   jas     Refactoring for HDK builds
02/09/07   jas     Add method to determine if the phone is online
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
11/1/06    RT      Initial revision

===========================================================================*/


/*=============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */

#include "OEMFeatures.h"
#include "AEEStdLib.h"
#include "AUXMenu.h"
#include "AUXUtils.h"
#include "AUXSettings.bid"

#if defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP)
/* USB Device Switching */
#include "usbdc_api.h"
#endif /* FEATURE_RUNTIME_DEVMAP && FEATURE_USB_DEFAULT_MTP */

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION AUXIsMessageDlg

DESCRIPTION
  Is the dialog ID one of those in the LookupDisplayMessageCtlID?

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXIsMessageDlg(uint16 wDlgID)
{
  if (wDlgID == IDD_GENERAL_MESSAGE) {
    return TRUE;
  }
  return FALSE;
} /* End AUXIsMessageDlg */


/*===========================================================================
FUNCTION AUXEndMessageDlg

DESCRIPTION
  End a message dialog

DEPENDENCIES
  None

SIDE EFFECTS
  Send an EVT_MESSAGE_END event
===========================================================================*/
void AUXEndMessageDlg(AUXApp *pMe)
{
  int nReturnStatus;
  IDialog * pIActiveDialog;
  uint16 wDlgID;

  if(NULL == pMe)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return;
  }

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog == NULL ) 
  {
    AUX_ERR ("Null pointer ", 0,0,0);
    return;
  }

  wDlgID = IDIALOG_GetID(pIActiveDialog);
  if (wDlgID == 0) /* error */
  {
    return;
  }
  
  if (AUXIsMessageDlg(wDlgID)) 
  {
    nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to end message dialog: %d", nReturnStatus, 0, 0);
    }
    (void)ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_AUXSETTINGS, 
                       EVT_USER, EVT_MESSAGE_END, wDlgID);
  }
  else 
  {
    AUX_ERR("EndMessageDlg(): End non Message Dlg", 0, 0, 0);
  }
} /*End AUXEndMessageDlg*/


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
void AUXDisplayMessageDlg(AUXApp *pMe, uint16 nStringID, int nDisplaySeconds)
{
  int nReturnStatus;

  if(NULL == pMe)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return;
  }

  pMe->m_MessageID = nStringID;
  nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                  IDD_GENERAL_MESSAGE, NULL);

  if(AUXCheckAEEReturnStatus(nReturnStatus)) {

    if (nDisplaySeconds > 0) {
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      (int) nDisplaySeconds*1000,
                      (PFNNOTIFY) AUXEndMessageDlg,
                      (uint32*) pMe);
      if (nReturnStatus != SUCCESS)
      {
        AUX_ERR("Fail to set timer: %d", nReturnStatus, 0, 0);
      }
    }
  }


} /*End AUXDisplayMessageDlg*/

/*===========================================================================
FUNCTION AUXEndCurrentDlg

DESCRIPTION
  End the current dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXEndCurrentDlg(AUXApp *pMe)
{
  int nReturnStatus;
  IDialog * pIActiveDialog;
  uint16 wDlgID;

  if(pMe == NULL)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return;
  }

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog != NULL) {
    wDlgID = IDIALOG_GetID(pIActiveDialog);
    if (wDlgID == 0) { /* error */
      return;
    }
    
    if (wDlgID == pMe->m_RootDialogID)  // If we're at the base dialog, end the app
    {
      (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
      return;
    }

    nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to end dialog: %d", nReturnStatus, 0, 0);
    }
  }
} /*End AUXEndCurrentDlg */

/*===========================================================================
FUNCTION AUXSetGeneralStaticTextStyle

DESCRIPTION
  Set the standard textctl dialog style

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXSetGeneralStaticTextStyle(IStatic *pIStaticText)
{
  ISTATIC_SetProperties(pIStaticText, ST_CENTERTEXT | ST_CENTERTITLE );
  ISTATIC_SetActive(pIStaticText, TRUE);
  return TRUE;
} /*End AUXSetGeneralStaticTextStyle */

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
boolean AUXSetStaticTextCtlSize(IStatic *pICurrentStatic, IControl *pIOtherCtrl)
{
  AEERect CtrlRect;
  AEERect StaticRect;

  if ( (pICurrentStatic == NULL) || (pIOtherCtrl == NULL)) { /* error checking */
    return FALSE;
  }

  /* Set static control size */
  ICONTROL_GetRect(pIOtherCtrl, &CtrlRect);
  ISTATIC_GetRect(pICurrentStatic, &StaticRect);

  StaticRect.dy = CtrlRect.y - 1;
  ISTATIC_SetRect(pICurrentStatic, &StaticRect);
  return TRUE;
} /*End AUXSetStaticTextCtlSize*/

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
boolean AUXIsHexWString(AECHAR *pszwHexString)
{
  if (! pszwHexString) {
    AUX_ERR("Null string", 0, 0, 0);
    return FALSE;
  }

  while (*pszwHexString)
  {
    if (((*pszwHexString >= '0') && (*pszwHexString <= '9')) ||
        ((*pszwHexString >= 'A') && (*pszwHexString <= 'F')) ||
        ((*pszwHexString >= 'a') && (*pszwHexString <= 'f')))
    {
      pszwHexString++;
      continue;
    }
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION AUXWATOI

DESCRIPTION
  Takes a AECHAR string and turns the result into an integer.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int AUXWATOI(const AECHAR *str)
{
  int ret_val = 0;
  int sign = 1;

  if (!str) return 0;

  /* Explicitly turn an empty string into zero */
  if (WSTRLEN(str) == 0) {
    return 0;
  }

  /* Get past any leading spaces. */
  while (*str == (AECHAR) ' ') { str++; }

  /* Check if its a negative number. */
  if (*str == (AECHAR) '-') {
    sign=-1;    /* Set the sign to negative. */
    str++;

//lint -save -e722 "Suspicious use of ;"
    while (*str == (AECHAR) ' ') { str++; }   /* Allow "- 5", for instance */
 //lint +e722 "Suspicious use of ;"
  }

  /* Loop until we find a non-digit (including NULL termination) */
  while ((*str >= (AECHAR) '0') && (*str <= (AECHAR) '9'))
  {
    ret_val = (ret_val * 10) + (*str - (AECHAR) '0');
    str++;
  }

  return (sign * ret_val);
} /*End WATOI */

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
int AUXConvertWStrToBinBuf(const AECHAR *pszwIn, uint8 *pBuf, uint32 nBufLen)
{
  int       nBytes = 0;
  int       nStrLen;
  int       i;
  AECHAR    cToConvert;
  uint8     nValue;

  if ((! pszwIn) || (! pBuf) || (nBufLen == 0))
  {
    AUX_MSG("Invalid input, %x %x %d", pszwIn, pBuf, nBufLen);
    return -1;
  }

  nStrLen = (int)WSTRLEN(pszwIn);
  if ((nStrLen & 0x00000001) != 0)
  {
    AUX_MSG("Input string has odd length, %d", nStrLen, 0, 0);
    return -1;
  }
  else if ((uint32)nStrLen >> 1 > nBufLen)
  {
    /* Buffer must be at least half the string length */
    AUX_MSG("Buffer too short for string %d %d", nBufLen, nStrLen, 0);
    return -1;
  }

  while (nStrLen != 0)
  {
    /* We already know that the output buffer is large enough, so we
     * just run until the input string is processed.  Consume two input
     * characters at at time, converting them into a single binary byte.
     */

    nValue = 0;
    for (i = 0; i < 2; i++)
    {
      /* Convert the character to binary value */
      cToConvert = *pszwIn++;
      if ((cToConvert >= (AECHAR)'0') && (cToConvert <= (AECHAR)'9'))
      {
        nValue += (uint8)(cToConvert - (AECHAR)'0');
      }
      else if ((cToConvert >= (AECHAR)'A') && (cToConvert <= (AECHAR)'F'))
      {
        nValue += (uint8)(cToConvert - (AECHAR)'A' + 0xA);
      }
      else if ((cToConvert >= (AECHAR)'a') && (cToConvert <= (AECHAR)'f'))
      {
        nValue += (uint8)(cToConvert - (AECHAR)'a' + 0xA);
      }
      else
      {
        AUX_MSG("Input string has invalid character, %d", (uint16)cToConvert,
                 0, 0);
        return -1;
      }

      /* For the first of the two bytes, shift it over to the upper nibble,
       * to make room for the other nibble.
       */
      if (i == 0)
      {
        nValue = (uint8)(nValue << 4);
      }
    }

    /* Copy the converted value into the appropriate byte */
    *pBuf = nValue;
    pBuf++;
    nBytes++;
    nStrLen = (int)WSTRLEN(pszwIn);
  }
  return nBytes;
}

/*===========================================================================
FUNCTION AUXDBToLevel

DESCRIPTION
  Converts RSSI absolute DB value to a number of bars.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int AUXDBToLevel (int nDBVal)
{
  if (nDBVal < 60)
    return AUX_WLAN_FOUR_BARS;
  else if (nDBVal < 75)
    return AUX_WLAN_THREE_BARS;
  else if (nDBVal < 90)
    return AUX_WLAN_TWO_BARS;
  else if (nDBVal < 105)
    return AUX_WLAN_ONE_BAR;
  else
    return AUX_WLAN_NO_BARS;
}

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
boolean AUX_PhoneIsOnline(IShell *pIShell)
{
  ICM         *pICM;
  AEECMPhInfo  sPhInfo;
  int          nRetVal;

  if (NULL == pIShell)
  {
    return FALSE;
  }

  nRetVal = ISHELL_CreateInstance(pIShell, AEECLSID_CM, (void **)&pICM);
  if ((SUCCESS != nRetVal) || (NULL == pICM))
  {
    return FALSE;
  }

  nRetVal = ICM_GetPhoneInfo(pICM, &sPhInfo, sizeof(sPhInfo));
  (void)ICM_Release(pICM);
  if (AEE_SUCCESS == nRetVal)
  {
    return (AEECM_OPRT_MODE_ONLINE == sPhInfo.oprt_mode);
  }
  return FALSE;
}

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
void AUXPopDlg(AUXApp* pMe, uint16 wPopDlgID)
{
  int nReturnStatus;
  IDialog* pIDialog;
  uint16 wDlgID;

  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return;
  }

  if (wPopDlgID == 0) {
    return;
  }

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIDialog == NULL) { /* error */
    return;
  }
  wDlgID = IDIALOG_GetID(pIDialog);
  if (wDlgID == 0) { /* error */
    return;
  }
  for (; (wDlgID != pMe->m_RootDialogID) && (wDlgID != wPopDlgID);) {
    nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to end dialog: %d", nReturnStatus, 0, 0);
    }

    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);  /* Get the next active dlg */
    if(pIDialog == NULL) { /* error */
      return;
    }
    wDlgID = IDIALOG_GetID(pIDialog);
    if(wDlgID == 0) {
      return;
    }
  }
} /*End AUXPopDlg */

/*===========================================================================
FUNCTION AUXDisplayIdleMenuDlg

DESCRIPTION
  Bring the program to the Idle menu screen display

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXDisplayIdleMenuDlg(AUXApp *pMe)
{ 
  IDialog *pIActiveDialog;
  uint16 wDlgID;

  if(pMe == NULL)
  {
    AUX_ERR("pMe NULL", 0, 0, 0);
    return FALSE;
  }

  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIActiveDialog == NULL) { /* error */
    return FALSE;
  }
  wDlgID = IDIALOG_GetID(pIActiveDialog);

  if(wDlgID == 0) {
    return FALSE;
  }

  /* cancel this timer if it is still running */
  (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);

  if (wDlgID == pMe->m_RootDialogID)
  {
    AUXEndCurrentDlg(pMe);
  }
  else
  {  
    AUXPopDlg(pMe, pMe->m_RootDialogID);
  }
  
  return TRUE;
} /*End AUXDisplayIdleMenuDlg*/

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
uint64 AUXGetUSBDevice(AUXApp *pMe)
{
  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return (uint64)USBDC_SHARED_DEVICE_COUNT;
  }

  return (uint64)usbdc_api_get_current_device();
} /* End AUXGetUSBDevice */

/*===========================================================================
FUNCTION AUXSetUSBDevice

DESCRIPTION
  Set Sound Device in uixsnd.  SND_DEVICE_MAX  indicates audoselect.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXSetUSBDevice(AUXApp *pMe, uint64 nNewVal)
{
  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if (nNewVal >= USBDC_SHARED_DEVICE_COUNT)
  {
    return FALSE;
  }

  return (USBDC_API_SUCCESS == usbdc_api_switch_device(
    (usbdc_supported_device_type)nNewVal));
} /* End AUXsetUSBDevice */

#endif /* FEATURE_RUNTIME_DEVMAP && FEATURE_USB_DEFAULT_MTP */

/*===========================================================================
FUNCTION AUXFormatTime

DESCRIPTION
  Formats the Julian date into the given string

DEPENDENCIES
  pszString must be valid

SIDE EFFECTS
  None
===========================================================================*/
static void AUXFormatTime (JulianType julian, AECHAR *pszString, int nLen)
{
  char c = 'a';
  char *pszTemp = (char *) MALLOC(sizeof(char)*(uint32) (nLen+1));

  if(!pszTemp)
    return;

  if (!pszString)
  {
     FREE(pszTemp);
     return;
  }

  if (julian.wHour >= 12)
  {
    c = 'p';
    if (julian.wHour > 12)
    {
      julian.wHour -= 12;
    }
  }
  if (julian.wHour == 0 )
  {
    julian.wHour = 12;
  }

  (void) SNPRINTF(pszTemp, sizeof(char)*(nLen+1),
                  "%2u:%02u%cm",julian.wHour,julian.wMinute, c);
  (void) STRTOWSTR (pszTemp, pszString, nLen * (int)sizeof(AECHAR));
  FREE(pszTemp);
}

/*===========================================================================
FUNCTION AUXFormatDate

DESCRIPTION
  Formats the Julian date into the given string

DEPENDENCIES
  pszString must be valid

SIDE EFFECTS
  None
===========================================================================*/
static void AUXFormatDate (JulianType julian, AECHAR *pszString, int nLen)
{
  static const char days[][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  static const char months[][4] = {"   ", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  int yr;
  char* pszTemp = (char *) MALLOC(sizeof(char)*(uint32)(nLen+1));

  if(!pszTemp)
    return;

  if ((!pszString) || (julian.wWeekDay > 6) || (julian.wMonth == 0) || (julian.wMonth > 12))
  {
     FREE(pszTemp);
     return;
  }

  yr = julian.wYear %100;
  (void) SNPRINTF(pszTemp, sizeof(char)*(nLen+1),
                  " %3s, %3s %2u \'%02u",
                  days[julian.wWeekDay], months[julian.wMonth],
                  julian.wDay, yr);
  (void) STRTOWSTR (pszTemp, pszString, nLen * sizeof(AECHAR));
  FREE(pszTemp);
}

/*===========================================================================
 FUNCTION AUXFormatTimeDate

 DESCRIPTION
  Formats the time and the date

 DEPENDENCIES
  AUXFormatTime, AUXFormatDate

SIDE EFFECTS
  None
===========================================================================*/
int AUXFormatTimeDate(JulianType j_time, AECHAR *pszDest,  int nLen)
{
  int nCurLen = 0;

  AUXFormatTime(j_time, pszDest, nLen);//format the time
  nCurLen = WSTRLEN (pszDest);

  AUXFormatDate(j_time, &pszDest[nCurLen], (nLen-nCurLen));//format the date
  nCurLen = WSTRLEN (pszDest);
  return nCurLen;

}

/*===========================================================================
FUNCTION AUXSetSecureTimeofDay

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
boolean AUXSetSecureTimeofDay (AUXApp *pMe, JulianType *julian)
/*lint -esym(715,julian) -esym(715,pMe) */
{
  uint32     localTime = 0;
  int        iResult;

  if((pMe == NULL) || (pMe->m_pISecSysClock == NULL))
  {
    AUX_ERR("Null pointers", 0, 0, 0);
    return FALSE;
  }

  localTime = JULIANTOSECONDS(julian);

#if MIN_BREW_VERSION(4,0)
  iResult = ISysClock_SetTimeS(pMe->m_pISecSysClock, localTime);
#else
  iResult = ISYSCLOCK_SetTimeS(pMe->m_pISecSysClock, localTime);
#endif

  if ( iResult != SUCCESS)
  {
    AUX_ERR("FAILED to set time for ISecSysClock, reason %d",iResult,0,0);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION AUXIsTimeSet

DESCRIPTION
  Returns the current time

DEPENDENCIES
  none

RETURN VALUE
  TRUE - if time is valid
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
static boolean AUXIsTimeSet(ISysClock *pClock)
{
  int  result;
  uint32 dwSeconds;

  #define SECS_PER_DAY (60l * 60l * 24l)

#if MIN_BREW_VERSION(4,0)
  result = ISysClock_GetTimeS(pClock, &dwSeconds);
#else
  result = ISYSCLOCK_GetTimeS(pClock, &dwSeconds);
#endif

  if (result != SUCCESS || dwSeconds < ((uint32)365 * SECS_PER_DAY))
  {
     return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION AUXGetSecureTimeofDay

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
boolean AUXGetSecureTimeofDay (AUXApp *pMe, JulianType *julian_date)
{
  uint32     TimeInSeconds = 0;
  int        iResult;

  if(( pMe == NULL ) || (pMe->m_pISecSysClock == NULL))
  {
    AUX_ERR("Null pointers", 0, 0, 0);
    return FALSE;
  }

  if(AUXIsTimeSet(pMe->m_pISecSysClock))
  { // user time has been set.

    // get the time
#if MIN_BREW_VERSION(4,0)
    iResult = ISysClock_GetTimeS(pMe->m_pISecSysClock, &TimeInSeconds);
#else
    iResult = ISYSCLOCK_GetTimeS(pMe->m_pISecSysClock, &TimeInSeconds);
#endif

    if (iResult == EBADSTATE)
    {
      AUX_MSG("Unreliable time for ISecSysClock",0,0,0);
    }
    else
    {
      if (iResult != SUCCESS)
      {
        AUX_ERR("FAILED to get time for ISecSysClock",0,0,0);
        return FALSE;
      }
    }

    GETJULIANDATE(TimeInSeconds, julian_date);
    return TRUE;
  } 
  else
  {
    GETJULIANDATE(0, julian_date);
    /* Process time only if valid */
    if (julian_date->wYear != 1980)
    {
      return TRUE;
    }
  }
  return FALSE;
}


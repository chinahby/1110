/*=============================================================================

FILE: QCamDlgMultiShot.c

SERVICES: QCamera Dialog

GENERAL DESCRIPTION: Multishot Setting Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2006 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/src/QcamDlgMultiShot.c#1 $
$DateTime: 2008/05/12 10:09:09 $
$Author: karenp $
$Change: 657887 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/14/07  KD     Removed deprecated function calls
09/07/06   NC      Added automation support for Multishot feature.
                   Set size per entry variable range.
07/28/06   NC      Redesigned menu calling logic for multishot
07/27/06   NC      Enable Bracketing mode option menu if it supported.
04/17/06   NC      Created QCamDlgMultoShot File
=============================================================================*/
//Deal with lint problems in BREW headers
//lint -emacro(740,SPRINTF)
//lint -emacro(740,STRTOWSTR)
//lint -emacro(740,STRLEN)
//lint -emacro(740,WSTRLEN)
//lint -emacro(740,WSTRTOSTR)
//lint -emacro(740,ATOI)
//lint -emacro(740,IDIALOG_GetControl)

//lint -emacro(611,SPRINTF)
//lint -emacro(611,STRTOWSTR)
//lint -emacro(611,STRLEN)
//lint -emacro(611,WSTRLEN)
//lint -emacro(611,WSTRTOSTR)
//lint -emacro(611,ATOI)

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "comdef.h"
#endif

#ifdef FEATURE_APP_QCAMERA
#include "QcamDlgMultiShot.h"
#include "qcamera.h"
#include "qcamera.brh"
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static void QCamDlgMultiShot_SetText(ITextCtl *ctrl, uint32 nVal, uint16 size);
static uint32 QCamDlgMultiShot_GetText(ITextCtl *ctrl, uint16 size);

/*===========================================================================

STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*=============================================================================
FUNCTION: QCamDlgMultiShot_SetText

DESCRIPTION: This function initializes the input text control.

PARAMETERS:
  ctrl:     ITextCtl interface pointer
  nVal:     number value to set to the text field
  size:     entry field size.

RETURN VALUE:
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void QCamDlgMultiShot_SetText(ITextCtl *ctrl, uint32 nVal, uint16 size)
{
  AECHAR valStr[QCAM_5_CHAR_LEN];
  char valChar[QCAM_5_CHAR_LEN];
  
  // Turn off Multiline, and turn on Password 
  // (so we don't display digits)
  ITEXTCTL_SetProperties(ctrl, 
                    (ITEXTCTL_GetProperties(ctrl) & ~TP_MULTILINE) | TP_FRAME);
                    

  ITEXTCTL_SetMaxSize(ctrl, (uint16)(size - 1));
  (void)SNPRINTF(valChar, sizeof(valChar), "%d", nVal);

  //print the old value in the entry box
  (void)STRTOWSTR(valChar, valStr, sizeof(AECHAR) * size);
  (void)ITEXTCTL_SetText(ctrl, valStr, WSTRLEN(valStr));

  /* Allow only numbers for PINs */
  (void)ITEXTCTL_SetInputMode(ctrl, AEE_TM_NUMBERS);
}

/*=============================================================================
FUNCTION: QCamDlgMultiShot_GetText

DESCRIPTION: Get the text from the text entry.

PARAMETERS:
  ctrl:     ITextCtl interface pointer
  size:     entry field size.

RETURN VALUE:
  boolean: Returns TRUE if input is larger than 0.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 QCamDlgMultiShot_GetText(ITextCtl *ctrl, uint16 size)
{
  AECHAR valStr[QCAM_5_CHAR_LEN];
  char valChar[QCAM_5_CHAR_LEN];
  uint32 nVal = 0;

  //Get user entry and set the member variable.
  (void)ITEXTCTL_GetText(ctrl, valStr, size);
  //Force user to enter a number before leave the dialog.
  if (WSTRLEN(valStr) > 0)
  {
    (void)WSTRTOSTR(valStr, valChar, sizeof(valChar));
    nVal = (uint32)ATOI(valChar);
  }

  return nVal;
}

/*=============================================================================
FUNCTION: QCamDlgMultiShot_EventHandler

DESCRIPTION: Handles Set Size Dialog's events

PARAMETERS:
  *pUser: QCam object pointer
  evt: event code
  w: word event param
  dw: dword event param

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean QCamDlgMultiShot_EventHandler(void * pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  QCam* pMe = (QCam*)pUser;
  IDialog *pCurrentDialog = NULL;
  ITextCtl *ctrl1 = NULL;
  ITextCtl *ctrl2 = NULL;
  uint32  nVal = 0;

  if (!pMe)
    return FALSE;

  pCurrentDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pCurrentDialog)
  {
    /* Get the pointers to the controls */
    ctrl1 = (ITextCtl *)IDIALOG_GetControl(pCurrentDialog, (int16)IDI_MULTISHOT_NUM);
    ctrl2 = (ITextCtl *)IDIALOG_GetControl(pCurrentDialog, (int16)IDI_MULTISHOT_DELAY);
  }

  switch (evt)                                                        
  {
  case EVT_DIALOG_START:
    if (!ctrl1 || !ctrl2)
      return FALSE;

    QCamDlgMultiShot_SetText(ctrl1, pMe->m_nMulPhotos, QCAM_3_CHAR_LEN);
    QCamDlgMultiShot_SetText(ctrl2, pMe->m_nPhotoInterval, QCAM_5_CHAR_LEN);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

    ITEXTCTL_SetActive(ctrl1, TRUE);
    if (pCurrentDialog)
    {
      (void)IDIALOG_SetFocus(pCurrentDialog, IDI_MULTISHOT_NUM);
      IDIALOG_Redraw(pCurrentDialog);
    }

    break;
  case EVT_KEY:  
    switch (w)
    {
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
      break;
    case AVK_SELECT:
      if (!ctrl1 || !ctrl2)
        return FALSE;

      nVal = QCamDlgMultiShot_GetText(ctrl1, QCAM_3_CHAR_LEN);
      //Force user to enter a multishot number larger than 0.
      //(1 could mean no multishot)
      if ((nVal < 1) || (nVal > QCAM_MAX_MULTISHOTS))
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
        return TRUE;
      }
      pMe->m_nMulPhotos = nVal;

      nVal = QCamDlgMultiShot_GetText(ctrl2, QCAM_5_CHAR_LEN);
      //Force user to enter a interval value smaller than 1000.
      if (nVal > QCAM_MAX_DELAY)
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
        return TRUE;
      }
      pMe->m_nPhotoInterval = nVal;
    
      //End this dialog
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      break;
    case AVK_CLR:
      //End this dialog
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      break;
    default:
      break;
    }
    break;
  case EVT_DIALOG_END:
    //Return to Multishot menu after this dialog ends.
    IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
    (void)IMENUCTL_Redraw(pMe->m_pIMenu);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    break;
  default: // Other events that have not been handled by the handler
    break;
  }
  //lint -save -e715 symbol dw not referenced here
  return TRUE;
  //lint -restore
}
#endif //FEATURE_APP_QCAMERA

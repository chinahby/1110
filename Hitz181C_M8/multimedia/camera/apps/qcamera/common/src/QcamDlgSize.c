/*=============================================================================

FILE: QCamDlgSize.c

SERVICES: QCamera Dialog

GENERAL DESCRIPTION: Size Setting Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/src/QcamDlgSize.c#1 $
$DateTime: 2008/05/12 10:09:09 $
$Author: karenp $
$Change: 657887 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/14/07   KD      Removed deprecated function calls
10/02/06   NC      Changed max quality input to 100.
09/07/06   NC      Used generic length definitions.
08/24/06   NC      Fix a crash from stack override in QCamDlgSize_SetText.
10/24/05   NC      More lint errors clean up.
10/20/05   NC      Fixed new lint errors.
09/15/05   NC      Lint errors clean up.
09/06/05   NC      Added NULL pointer checking to input parameters.
08/02/05   NC      Close dialog if automation test error.
08/01/05   SM      Added automation support for both file size and quality set.
07/08/05   NC      Added one more text control to handle the second input.
04/19/05   NC      Enabled encoding with both file size and quality set.
04/15/05   NC      Added error report for automation test.
02/24/05   JR      Fixed some lint errors.
01/31/05   NC      Fixed a bug in ITEXTCTL_GetText call. 
01/26/05   NC      Code clean up.
12/07/04   NC      Added DIAG features for Auto Test.
11/09/04   NC      Added the old value to entry box and change max. enter number.
10/15/05   NC      Updated logic to return to quality menu when the dialog is closed
10/14/04   NC      Fixed zero file size problem
09/21/04   NC      Add more features.
07/07/04   NC      Created QCamDlgSize Implement File
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

//lint -e818, Pointer parameter 'pMe' (line 110) could be declared as pointing to const
/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "comdef.h"
#endif

#ifdef FEATURE_APP_QCAMERA
#include "QcamDlgSize.h"
#include "qcamera.h"
#include "qcamera.brh"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static void QCamDlgSize_SetText(QCam *pMe, ITextCtl *ctrl);
static boolean QCamDlgSize_GetText(QCam *pMe, ITextCtl *ctrl, boolean bQuality);
/*===========================================================================

STATIC and PUBLIC FUNCTIONS

===========================================================================*/
/*=============================================================================
FUNCTION: QCamDlgSize_SetText

DESCRIPTION: This function initializes the input text control.

PARAMETERS:
  pMe:      QCam object pointer
  ctrl:     ITextCtl interface pointer

RETURN VALUE:
  NONE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void QCamDlgSize_SetText(QCam *pMe, ITextCtl *ctrl)
{
  AECHAR quality[QCAM_5_CHAR_LEN];
  char qualchar[QCAM_5_CHAR_LEN];
  uint16 nMaxNum;

  if (!pMe || !ctrl)
    return;

  nMaxNum = (pMe->m_qualityType == QCAM_QUALITY_PERCENT) ? 
             (QCAM_4_CHAR_LEN - 1) : (QCAM_5_CHAR_LEN - 1);
  // Turn off Multiline, and turn on Password 
  // (so we don't display digits)
  ITEXTCTL_SetProperties(ctrl, 
                    (ITEXTCTL_GetProperties(ctrl) & ~TP_MULTILINE) | 
                    TP_FRAME);

  // Limit the number of digits to be entered 1-100 for quality percentage
  // or 1-9999 for max. file size
  ITEXTCTL_SetMaxSize(ctrl, nMaxNum);
  if (pMe->m_qualityType == QCAM_QUALITY_PERCENT)
  {
    (void)SNPRINTF(qualchar, sizeof(qualchar), "%d", pMe->m_nQuality);
    (void)ITEXTCTL_SetTitle(ctrl, pMe->m_pResFile, IDS_QUALITY_PERCENT, NULL);
  }
  else
  {
    (void)SNPRINTF(qualchar, sizeof(qualchar), "%d", pMe->m_nMaxFileSize);
    (void)ITEXTCTL_SetTitle(ctrl, pMe->m_pResFile, IDS_QUALITY_FILESIZE, NULL);
  }

  //print the old value in the entry box
  (void)STRTOWSTR(qualchar, quality, sizeof(AECHAR) * (nMaxNum + 1));
  (void)ITEXTCTL_SetText(ctrl, quality, WSTRLEN(quality));

  /* Allow only numbers for PINs */
  (void)ITEXTCTL_SetInputMode(ctrl, AEE_TM_NUMBERS);
}

/*=============================================================================
FUNCTION: QCamDlgSize_GetText

DESCRIPTION: Get the text from the text entry.

PARAMETERS:
  pMe:      QCam object pointer
  ctrl:     ITextCtl interface pointer
  bQuality: Input type

RETURN VALUE:
  boolean: Returns TRUE if input is larger than 0.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean QCamDlgSize_GetText(QCam *pMe, ITextCtl *ctrl, boolean bQuality)
{
  AECHAR quality[QCAM_5_CHAR_LEN];
  char qualchar[QCAM_5_CHAR_LEN];
  uint16 nMaxNum;
  int qual = 0;

  if (!pMe || !ctrl)
    return FALSE;

  nMaxNum = (pMe->m_qualityType == QCAM_QUALITY_PERCENT) ? 
             (QCAM_4_CHAR_LEN - 1) : (QCAM_5_CHAR_LEN - 1);
  //Get user entry and set the member variable.
  (void)ITEXTCTL_GetText(ctrl, quality, nMaxNum + 1);
  //Force user to enter a number before leave the dialog.
  if (WSTRLEN(quality) > 0)
  {
    (void)WSTRTOSTR(quality, qualchar, sizeof(qualchar));
    qual = ATOI(qualchar);
    //Force user to enter a valid number (> 0).
    if (qual == 0)
      return FALSE;

    if(bQuality)
    {
      if (qual > 100)
        return FALSE;

      pMe->m_nQuality = (uint16)qual;
    }
    else
    {
      pMe->m_nMaxFileSize = (uint32)qual;
    }
    return TRUE;
  }

  return FALSE;
}

/*=============================================================================
FUNCTION: QCamDlgSize_EventHandler

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
boolean QCamDlgSize_EventHandler(void * pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  QCam* pMe = (QCam*)pUser;
  IDialog * pCurrentDialog;
  boolean result = FALSE;
  ITextCtl *ctrl1 = NULL;
  ITextCtl *ctrl2 = NULL;

  if (!pMe)
    return FALSE;

  pCurrentDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pCurrentDialog)
  {
    /* Get the pointers to the text controls */
    ctrl1 = (ITextCtl*)IDIALOG_GetControl(pCurrentDialog, (int16)IDI_QUALITY_INPUT);
    ctrl2 = (ITextCtl*)IDIALOG_GetControl(pCurrentDialog, (int16)IDI_SIZE_INPUT);
  }

  switch (evt)
  {
  case EVT_DIALOG_START:
    {
      /* Exit if we don't have a valid pointer */
      if (!ctrl1)
        return result;

      QCamDlgSize_SetText(pMe, ctrl1);
      if (pMe->m_nJPEGSpec == QCAM_JPEG_BOTH)
      {
        //Enable the second text input for file size
        pMe->m_qualityType = QCAM_QUALITY_FILESIZE;
        QCamDlgSize_SetText(pMe, ctrl2);
      }

      ITEXTCTL_SetActive(ctrl1, TRUE);
      if (pCurrentDialog)
        (void)IDIALOG_SetFocus(pCurrentDialog, IDI_QUALITY_INPUT);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

      if (pCurrentDialog)
        IDIALOG_Redraw(pCurrentDialog);

      result = TRUE;
      break;
    }
  case EVT_KEY:  
    switch (w)
    {
    case AVK_UP:
    case AVK_LEFT:
    case AVK_DOWN:
    case AVK_RIGHT:
      result = TRUE;
      break;
    case AVK_SELECT:
      {

        /* Exit if we don't have valid a pointer */
        if (!ctrl1)
          return result;

        //Get user entry and set the member variable.
        if (pMe->m_nJPEGSpec != QCAM_JPEG_BOTH)
        {
          boolean bQuality = (pMe->m_qualityType == QCAM_QUALITY_PERCENT);
          result = QCamDlgSize_GetText(pMe, ctrl1, bQuality);
          if (result)
          {
            //Clean up the old value
            if (bQuality)
              pMe->m_nMaxFileSize = 0;
            else
              pMe->m_nQuality = 0;
          }
        }
        else
        {
          result = QCamDlgSize_GetText(pMe, ctrl1, TRUE);
          if (result)
          {
            result = QCamDlgSize_GetText(pMe, ctrl2, FALSE);
          }
          pMe->m_qualityType = QCAM_QUALITY_BOTH;
        }

        if (!result)
        {
          //force the user to enter a number before exit with 
          //select key. It ensure a file size or quality percetage larger than zero.
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
          return TRUE;
        }
                
        //End this dialog
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      }
      break;
    case AVK_CLR:
      //End this dialog
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      result = TRUE;
      break;
    default:
      break;
    }
   
    break;
  case EVT_DIALOG_END:
    //Return to quality menu after the set size dialog ends.
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
    (void)IMENUCTL_Redraw(pMe->m_pIMenu);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    result = TRUE;
    break;
  default: // Other events that have not been handled by the handler
    break;
  }
  //lint -save -e715 symbol key not referenced here
  return result;
  //lint -restore
}
#endif //FEATURE_APP_QCAMERA

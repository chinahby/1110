/*=============================================================================

FILE: QCamDlgSave.c

SERVICES: QCamera Dialog

GENERAL DESCRIPTION: Save Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004 - 2005 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/src/QcamDlgSave.c#1 $
$DateTime: 2008/05/12 10:09:09 $
$Author: karenp $
$Change: 657887 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/02/06   NC      Used changable resource file name.
10/24/05   NC      More lint errors clean up.
09/15/05   NC      Lint errors clean up.
09/06/05   NC      Added NULL pointer checking to input parameters.
03/10/05   NC      Update per function prototype change.
10/19/04   NC      Limit file name entry to 10 characters
10/15/04   NC      Fixed bugs
09/21/04   NC      Add more features.
07/07/04   NC      Created QcamDlgSave Implement File
=============================================================================*/
//Deal with lint problems in BREW headers
//lint -emacro(740,IDIALOG_GetControl)
/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "comdef.h"
#endif

#ifdef FEATURE_APP_QCAMERA
#include "QcamDlgSave.h"
#include "qcamera.h"
#include "QcamCommon.h"
#include "QcamDisp.h"
#include "QcamMenus.h"
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*=============================================================================
FUNCTION: QCamDlgSave_EventHandler

DESCRIPTION: Handles Save Dialog's events

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
boolean QCamDlgSave_EventHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dw)
{
  QCam* pMe = (QCam*)pUser;
  IDialog * pCurrentDialog;
  IMenuCtl* pICurrentMenu = NULL;
  uint32 dirLen;
  uint32 dateDirLen;

  if (!pMe)
    return FALSE;

  Qcamera_GetDirLen(pMe, &dirLen, &dateDirLen);

  pCurrentDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pCurrentDialog)
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pCurrentDialog, IDM_SAVE_PIC_MENU);

  switch (evt)
  {
  case EVT_DIALOG_START:
    if (pCurrentDialog && pICurrentMenu)
    {
      uint16 id;
      AEERect rcMenu;
      ITextCtl *textCtl = (ITextCtl *)IDIALOG_GetControl(pCurrentDialog, IDI_SAVE_PIC_FILE_NAME);
      IStatic  *dirCtrl = (IStatic  *)IDIALOG_GetControl(pCurrentDialog, IDI_DIRECTORY);
      
      QCam_SetMenuStyle(pICurrentMenu, 0, 0);
      SETAEERECT(&rcMenu, 0, pMe->di.cyScreen - QCAM_BUTTON_MENU_HEIGHT, pMe->di.cxScreen, QCAM_BUTTON_MENU_HEIGHT);
      IMENUCTL_SetRect(pICurrentMenu, &rcMenu);

      // Delete "multitap" for BREW3.1.2 or before.
      id = IMENUCTL_GetItemID(pICurrentMenu, 0);
      if (id != IDC_SAVE)
      {
        (void)IMENUCTL_DeleteItem(pICurrentMenu, id);
        (void)IMENUCTL_MoveItem(pICurrentMenu, id, 1);
      }
      IMENUCTL_SetSel(pICurrentMenu, IDC_SAVE);
      pMe->m_activeMenu = QCAM_MENU_SAVE;
      (void)IMENUCTL_Redraw(pICurrentMenu);

      if (!Qcamera_GetFileName(pMe))
        return FALSE;

      if (dirCtrl)
      {
        QCamDisp_UpdateStaticField(dirCtrl, pMe->m_sCurrentFileName, dirLen, dateDirLen);
      }

      if (textCtl)
      {
        //limit the file name entry to 10 characters
        ITEXTCTL_SetMaxSize(textCtl, 11);
        QCamDisp_UpdateTextField(textCtl, pMe->m_sCurrentFileName, (int)dateDirLen);
      }

      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);

      return TRUE;
    }
    return FALSE;
  case EVT_KEY:  
    if (wParam == (uint16)AVK_CLR)
    {
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      pMe->m_wCurrentImage--; // Image not saved, decrement the image number
      Qcamera_TurnOnCameraPreview(pMe);
    }
    return TRUE;

  case EVT_KEY_PRESS:
    if (wParam == (uint16)AVK_DOWN || wParam == (uint16)AVK_UP)
    {
      //will switch the focus between the menu and text controls
      (void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_QCAMERA, EVT_KEY_PRESS, 0, 0);
      break;
    }
    break;

  case EVT_KEY_RELEASE:
    break;

  case EVT_COMMAND:
    if (!pICurrentMenu || !pCurrentDialog)
      return FALSE;

    if (IMENUCTL_IsActive(pICurrentMenu))
    {
      switch (wParam)
      {
        case IDC_SEND:
        case IDC_SAVE:
          {
            ITextCtl *textCtl = (ITextCtl *)IDIALOG_GetControl(pCurrentDialog, IDI_SAVE_PIC_FILE_NAME);
            if (!textCtl)
              return FALSE;

            if (QCam_GetValidFileName(textCtl, pMe->m_sCurrentFileName, (int)dateDirLen))
            {
              if (wParam == IDC_SEND)
                pMe->m_bNeedToSend = TRUE;

              (void)ISHELL_EndDialog(pMe->a.m_pIShell);
              Qcamera_SavePhoto(pMe);
            }
            else
            {
              (void)ISHELL_MessageBox(pMe->a.m_pIShell, pMe->m_pResFile, IDS_INVALID_NAME, IDS_MSG_INVALIDNAME);
            }
          }
          return TRUE;
        case IDC_CANCEL:
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
          pMe->m_wCurrentImage--; // Image not saved, decrement the image number
          Qcamera_TurnOnCameraPreview(pMe);
          return TRUE;
        
        default:
           break;
      }
      return FALSE;
    }
    break;

  case EVT_DIALOG_END:
    break;

  default: // Other events that have not been handled by the handler
    return FALSE;
  }

  //lint -save -e715, symbol dw not referenced here
  return TRUE;
  //lint -restore
}

#endif //FEATURE_APP_QCAMERA

/*===========================================================================

FILE: qalbumDlg.c

GENERAL DESCRIPTION
    This file contains the dialog handler of Qalbum.

      Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qalbum/main/latest/src/qalbumDlg.c#1 $
 $DateTime: 2008/05/12 10:09:09 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 11/12/07  NC     Created/Released m_pMediaUtil per saving to enable cancel encoding
 10/05/09  KV     Fixed the Compiler warning.
 09/18/09  NC     Fixed some string length issue on new function calls.
 09/17/07  NC     Simplify Qalbum_DlgSlideShowEventHandler and correct alignments
                  Added additional pointer checking.
 09/14/07  KD     Removed deprecated function calls
 05/22/07  SK     JPEG Decoder Resizing feature
 04/20/07  NC     Set a test string to discription screen for automation test.
 04/16/07  NC     Fixed error return bugs.
 02/21/07  TV     Added ability to print with PBPrint
 02/05/07  ZL     Removed early transitions from SAVE state to VIEW state.
 12/20/06  ZL     Fixed crashes when continuously pressing [SELECT] key
                  on IDD_SAVE_PIC.
 11/29/06  ZL     Fixed crashes when selected [Cancel] on IDD_SAVE_PIC.
 11/09/06  ZL     Changes caused by adding VIEW state before EDIT state.
 11/08/06  ZL     Replaced FEATURE_APP_QCAMERA with FEATURE_APP_QALBUM.
 10/02/06  NC     Fixed potential bug.
 09/25/06  RK     Add the Error Handler for IIPL Function.
 06/26/06  NC     Fixed page fault in adding describe to an image not from qcamera.
 03/20/06  NC     Fixed a typo.
 10/24/05  JN     2nd round lint clean up.
 09/20/05  JN     Lint error clean up.
 08/17/05  JN     Redesigned Qalbum to decouple it from AMSS.
 07/18/05  JN     Replaced all OEMJPEG with IIMAGE APIs.
 05/18/05  NC     Updated Qalbum_GetDirLen() to return the right len for MMC.
 05/10/05  JN     Added DlgDescribeEventHandler.
============================================================================*/

//lint -e740 suppress errors on pointer cast
//lint -e611
//lint -e545 suppress errors on use of &
//lint -e801 suppress errors on use of goto

/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#ifndef AEE_SIMULATOR
#include "msg.h"
#endif

#if defined(FEATURE_APP_QALBUM)
#include "AEEStdLib.h"
#include "QcamDisp.h"
#include "qalbum.h"
#include "qalbumDlg_i.h"
#include "QcamDef.h"

/*===========================================================================
FUNCTION Qalbum_DlgSaveEventHandler

  DESCRIPTION
    Key handler for Qalbum in (list) menu state.
    Only Set size menu in this state currently

  PARAMETERS:
    pUser   - Pointer to the applet instance, map to QCam * pMe
    evt     - AEEEvent id
    wParam  - 2 byte param
    dwParam - 4 byte param

  RETURN VALUE:
   None
===========================================================================*/
boolean Qalbum_DlgSaveEventHandler(void * pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
  Qalbum *pMe;
  IDialog * pCurrentDialog;
  IMenuCtl* pICurrentMenu = NULL;
  int dirLen;
  int dateDirLen;

  if (!pUser)
    return FALSE;

  pMe = (Qalbum *)pUser;

  Qalbum_GetDirLen(pMe, &dirLen, &dateDirLen);

  pCurrentDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pCurrentDialog)
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl(pCurrentDialog, IDM_SAVE_PIC_MENU);
  else
    return FALSE;

  switch (evt)
  {
  case EVT_DIALOG_START:
    if (pICurrentMenu)
    {
      uint16 id;
      AEERect rcMenu;
      ITextCtl  *textCtl  = (ITextCtl  *)IDIALOG_GetControl(pCurrentDialog, IDI_SAVE_PIC_FILE_NAME);
      IStatic   *dirCtrl  = (IStatic   *)IDIALOG_GetControl(pCurrentDialog, IDI_DIRECTORY);

      QCam_SetMenuStyle(pICurrentMenu, 0, 0);
      SETAEERECT(&rcMenu, 0, pMe->di.cyScreen - QCAM_BUTTON_MENU_HEIGHT, pMe->di.cxScreen, QCAM_BUTTON_MENU_HEIGHT);
      IMENUCTL_SetRect(pICurrentMenu, &rcMenu);

      // Delete "multitap"
      id = IMENUCTL_GetItemID(pICurrentMenu, 0);
      if (id !=IDC_SAVE)
      {
        (void)IMENUCTL_DeleteItem(pICurrentMenu, id);
        (void)IMENUCTL_MoveItem(pICurrentMenu, id, 1);
      }
      IMENUCTL_SetSel(pICurrentMenu, IDC_SAVE);
      (void)IMENUCTL_Redraw(pICurrentMenu);

      if (dirCtrl)
      {
        QCamDisp_UpdateStaticField(dirCtrl, pMe->m_sCurrentFileName, (uint32)dirLen, (uint32)dateDirLen);
      }

      if (textCtl)
      {
        //limit the file name entry to 10 characters
        ITEXTCTL_SetMaxSize(textCtl, 11);
        //Generate new file name
        Qalbum_GenerateEditedFileName(pMe->m_pIFileMgr, pMe->m_sCurrentFileName);
        QCamDisp_UpdateTextField(textCtl, pMe->m_sCurrentFileName, dateDirLen);
      }

      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
      return TRUE;
    }
    return FALSE;
  case EVT_KEY:
    if (wParam == (uint16)AVK_CLR)
    {
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      // Redraw the edit dialog
      pMe->eState = QCAM_STATE_VIEW;
      Qalbum_ViewPhoto(pMe, IDS_SAVE, FALSE);
    }
    return TRUE;

  case EVT_KEY_PRESS:
    break;

  case EVT_KEY_RELEASE:
    break;

  case EVT_COMMAND:
    if (!pICurrentMenu)
      return FALSE;

    if (IMENUCTL_IsActive(pICurrentMenu))
    {
      switch (wParam)
      {
        case IDC_SAVE:
          {
            ITextCtl *textCtl = (ITextCtl *)IDIALOG_GetControl(pCurrentDialog, IDI_SAVE_PIC_FILE_NAME);

            if (QCam_GetValidFileName(textCtl, pMe->m_sCurrentFileName, (int)dateDirLen))
            {
              (void)ISHELL_EndDialog(pMe->a.m_pIShell);
              Qalbum_SavePhoto(pMe);
            }
            else
            {
              (void)ISHELL_MessageBox(pMe->a.m_pIShell, QALBUM_RES_FILE, IDS_INVALID_NAME, IDS_MSG_INVALIDNAME);
            }
          }
          return TRUE;
        case IDC_CANCEL:
          (void)ISHELL_EndDialog(pMe->a.m_pIShell);
          // Redraw the edit dialog
          pMe->eState = QCAM_STATE_VIEW;
          Qalbum_ViewPhoto(pMe, IDS_SAVE, FALSE);
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

  return TRUE;
  //lint -save -e715 symbol 'dwParam' not used here
}
//lint -restore

/*===========================================================================
FUNCTION Qalbum_DlgDescribeEventHandler

  DESCRIPTION
    Key handler for Qalbum in add photo description menu.

  PARAMETERS:
    pUser   - Pointer to the applet instance, map to Qalbum
    evt     - AEEEvent id
    wParam  - 2 byte param
    dwParam - 4 byte param

  RETURN VALUE:
   None
===========================================================================*/
boolean Qalbum_DlgDescribeEventHandler(void * pUser, AEEEvent evt,
                                              uint16 wParam, uint32 dwParam)
{
  Qalbum*   pMe = (Qalbum*)pUser;
  IDialog*  pDialog;
  ITextCtl* pCtrl = NULL;
  OEMImageParmExt parm;
  int       retVal;
  uint32    len;
  AECHAR    wsBuf[QALBUM_EXIF_MAX_LEN + 1] = {0};
  char      szBuf[QALBUM_EXIF_MAX_LEN + 1] = {0};
  boolean   result = TRUE;

  if (!pMe || !pMe->m_pDisplayImage)
    return FALSE;

  pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (!pDialog)
    return FALSE;

  /* Get handle to the text control */
  pCtrl = (ITextCtl*)IDIALOG_GetControl(pDialog, (int16)IDI_DESCRIBE);
  if (!pCtrl)
    return FALSE;

  switch (evt)
  {
  case EVT_DIALOG_START:
    ITEXTCTL_SetProperties(pCtrl, ITEXTCTL_GetProperties(pCtrl)
                           | TP_MULTILINE | TP_FRAME );

    // Limit the number characters
    ITEXTCTL_SetMaxSize(pCtrl, QALBUM_EXIF_MAX_LEN);
    (void)ITEXTCTL_SetTitle(pCtrl, QALBUM_RES_FILE, IDS_DESCRIBE, NULL);

    // Show the old value in the entry box
    parm.uID = (uint32)IMAGE_DESCRIPTION;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && (parm.uLen > 0))
    {
      (void)STRTOWSTR(parm.pData, wsBuf, (int)(sizeof(wsBuf)));
      (void)ITEXTCTL_SetText(pCtrl, wsBuf, WSTRLEN(wsBuf));
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    /* Allow multitap */
    (void)ITEXTCTL_SetInputMode(pCtrl, AEE_TM_NONE);
    ITEXTCTL_SetActive(pCtrl, TRUE);

    //IDIALOG_SetFocus(pDialog, IDI_DESCRIBE);
    IDIALOG_Redraw(pDialog);
    result = TRUE;
    break;

  case EVT_KEY:
    switch (wParam)
    {
    case AVK_UP:
    case AVK_LEFT:
    case AVK_DOWN:
    case AVK_RIGHT:
      result = TRUE;
      break;

    case AVK_SELECT:
      // Get user entry and set the exif value
      (void)ITEXTCTL_GetText(pCtrl, wsBuf, QALBUM_EXIF_MAX_LEN + 1);
      len = (uint32)WSTRLEN(wsBuf);
      if (len > 0)
      {
        (void)WSTRTOSTR(wsBuf, szBuf, (int)(len + 1));
        ZEROAT(&parm);
        parm.uID   = (uint32)IMAGE_DESCRIPTION;
        parm.pData = szBuf;
        parm.uLen  = len + 1;
        retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_SET_ASCII_VALUE,
                                &parm, sizeof(OEMImageParmExt));
      }

      // End dialog
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      // Redraw the edit screen
      Qalbum_ViewPhoto(pMe, IDS_DESCRIBE, FALSE);
      result = TRUE;
      break;

    case AVK_CLR:
      // End dialog
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      // Redraw the edit screen
      Qalbum_ViewPhoto(pMe, IDS_DESCRIBE, FALSE);
      result = TRUE;
      break;

    default:
      break;
    }
    break;

  case EVT_DIALOG_END:
    // Redraw the edit screen
    Qalbum_ViewPhoto(pMe, IDS_DESCRIBE, FALSE);
    result = TRUE;
    break;

  default:
    break;
  }
  //lint -save -e715 symbol 'dwParam' not used here
  return result;
}
//lint -restore


/*===========================================================================
FUNCTION Qalbum_GetDirLen

  DESCRIPTION
    This function gets lenght of root directory and data directory.

  PARAMETERS:
    pMe    - Pointer to  Qalbum struct
    rootDirLen - Pointer to length of root directory
    dataDirLen - Pointer to length of data directory

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_GetDirLen(const Qalbum *pMe, int *rootDirLen, int *dataDirLen)
{
  char pDir[FILE_NAME_SIZE] = {0};

  if (!pMe || !rootDirLen || !dataDirLen)
    return;

  if (QCam_GetFilePath(pMe->m_sCurrentFileName, pDir, FILE_NAME_SIZE))
  {
    //Add 1 to path length to include the last '/'.
    *dataDirLen = (int)STRLEN(pDir) + 1;
    *rootDirLen = (STRRCHR(pDir, DIRECTORY_CHAR) - pDir) + 1;
  }
  else
  {
    //should nerver go here
    *dataDirLen = 0;
    *rootDirLen = 0;
  }
}

/*===========================================================================
FUNCTION Qalbum_GenerateFileName

  DESCRIPTION
    This function creates a new file name based on the provided file name.
    It appends a digit to the original file name.
    i.e. If the provided file name is "f:/~/qcam/img/2004-11-09/img001.jpg",
    and image0011.jpg doesn't exist, then the new file name is:
    "f:/~/qcam/img/2004-11-09/img0011.jpg"

  PARAMETERS:
    pMe - pointer to Qalbum structure.

  RETURN VALUE:
    NA
===========================================================================*/
static void Qalbum_GenerateEditedFileName(IFileMgr *pIFileMgr, char *szCurName)
{
  char szExt[FILE_NAME_SIZE] = {0};
  char *pExt;
  uint16 count = 0;

  if (!pIFileMgr || !szCurName)
    return;

  // Find insersion point
  pExt = szCurName + STRLEN(szCurName) - QCAM_EXTENSION_LENGTH;
  // Save the extension
  (void)STRLCPY(szExt, pExt, sizeof(szExt));
  // Loop until find a unique name
  do
  {
    // The file name becomes img001x.xxx
    (void)SNPRINTF(pExt, (size_t)(FILE_NAME_SIZE - STRLEN(szCurName)), "%u%s", 
                   ++count, szExt);
  } while (IFILEMGR_Test(pIFileMgr, szCurName) == SUCCESS);
}

/*===========================================================================
FUNCTION Qalbum_SavePhoto

  DESCRIPTION
    This function saves the current image.

  PARAMETERS:
    pMe            - Pointer to  Qalbum struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_SavePhoto(Qalbum *pMe)
{
  AEEMediaData   md;

  MSG_MED("ENTERING Save Photo",0,0,0);

  if (!pMe)
    return;

  if (!pMe->m_pMediaUtil)
  {
    if(ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_MEDIAUTIL,
                             (void **)&pMe->m_pMediaUtil) != SUCCESS)
    {
      MSG_HIGH("MediaUtil Creation failed", 0, 0, 0);
      return;
    }
  }

  //Fill media data
  md.clsData = MMD_FILE_NAME;
  md.pData = (void *)pMe->m_sCurrentFileName;
  md.dwSize = 0;

  if (CAM_ISTYPEFILE(QCAM_JPEG_EXTENSION, pMe->m_sCurrentFileName) ||
      CAM_ISTYPEFILE(QCAM_JPEG_EXTENSION_CAP, pMe->m_sCurrentFileName))
    Qalbum_SaveJpegImage(pMe, &md);
  else if (CAM_ISTYPEFILE(QCAM_PNG_EXTENSION, pMe->m_sCurrentFileName) ||
           CAM_ISTYPEFILE(QCAM_PNG_EXTENSION_CAP, pMe->m_sCurrentFileName))
    Qalbum_SavePngImage(pMe, &md);
  else
  {
    MSG_MED("Encoding type not supported",0,0,0);
    //Report error
    (void)Qalbum_DisplayError(pMe, EUNSUPPORTED);
  }
}

/*===========================================================================
FUNCTION Qalbum_GetEncodeCB

  DESCRIPTION
    This function is the callback from encoding engine.

  PARAMETERS:
    pUser    - Pointer to  Qalbum struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_GetEncodeCB(void *pUser)
{
  Qalbum *pMe;

  if (!pUser)
    return;

  pMe = (Qalbum *)pUser;

  if (pMe->m_EncodeResult.nResult == SUCCESS)
  {
    MSG_MED("Got encode complete callback",0,0,0);
    Qalbum_DisplayPhotoSavedMsg(pMe);
  }
  else
  {
    //report error
    (void)Qalbum_DisplayError(pMe, pMe->m_EncodeResult.nResult);
  }

  CAM_RELEASEIF(pMe->m_pEditThumbnail);
  ZEROAT(&pMe->m_EncodeResult);
}

/*===========================================================================
FUNCTION Qalbum_SaveJpegImage

  DESCRIPTION
    This function saves the current image in JPEG format.

  PARAMETERS:
    pMe - Pointer to  Qalbum struct
    pmd - Pointer to the AEEMediaData struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_SaveJpegImage(Qalbum *pMe, AEEMediaData *pmd)
{
  AEESize          * pSize;
  AEESize            sizeThumb;
  IDIB             * pDIB   = NULL;
  IYCbCr           * pYCbCr = NULL;
  AEEMediaEncodeInfo mei;
  AEEMediaData       pmdList[2];
  ImageEncodeSpec    spec;
  int                iImgSize;
  boolean            bMakeThumbnail = FALSE;
  OEMImageParmExt    parm;
  int                retVal = FALSE;

  if (!pMe || !pMe->m_pMediaUtil)
    return;

  CALLBACK_Init(&pMe->m_EncodeCB, Qalbum_GetEncodeCB, pMe);

  // Use QVGA size for thumbnail
  pSize = QCam_GetImageDimension((int)QCAM_SIZE_QVGA);
  sizeThumb.cx = pSize->cx;
  sizeThumb.cy = pSize->cy;

  if (IBITMAP_QueryInterface(pMe->m_pEditIn, AEECLSID_DIB, (void **)&pDIB) == SUCCESS)
  {
    // Create thumbnail if image size >= VGA
    iImgSize = Qalbum_GetImageSize(pDIB->cx, pDIB->cy);
    if (iImgSize <= (int)QCAM_SIZE_VGA)   // size table is in reverse order
    {
      bMakeThumbnail = TRUE;
      if (pDIB->cx > pDIB->cy)
      {
        // swap cx, cy
        sizeThumb.cx = pSize->cy;
        sizeThumb.cy = pSize->cx;
      }

      if (IBITMAP_CreateCompatibleBitmap(pMe->m_pEditIn, &pMe->m_pEditThumbnail,
                                         (uint16)sizeThumb.cx, (uint16)sizeThumb.cy) != SUCCESS)
      {
        bMakeThumbnail = FALSE;
      }
      else
      {
        ((IDIB *)pMe->m_pEditThumbnail)->nColorScheme = ((IDIB *)pMe->m_pEditIn)->nColorScheme;
        // Down size main image to thumbnail
        retVal = IIPL_ChangeSize(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditThumbnail);
        if (Qalbum_DisplayError(pMe, retVal) != SUCCESS)
        {
          goto Done;
        }
      }
    }

    // Do encoding if everything is ready
    ZEROAT(&spec);
    spec.bQuality = TRUE;
    spec.size.nQuality = 90;
    spec.wMainWidth = pDIB->cx;
    spec.wMainHigh = pDIB->cy;
    spec.wMainInColor = pDIB->nColorScheme;
    if (bMakeThumbnail)
    {
      spec.wThumbWidth = (uint16)sizeThumb.cx;
      spec.wThumbHigh  = (uint16)sizeThumb.cy;
      spec.wThumbInColor = pDIB->nColorScheme;
    }
    if (pMe->m_pDisplayImage)
    {
      retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_INFO,
                              &parm, sizeof(OEMImageParmExt));
    }
    if (retVal == SUCCESS)
      spec.pHeaderBuf = parm.pData;
    else
      spec.pHeaderBuf = NULL;

    ZEROAT(&mei);
    mei.pmdList = pmdList;

    mei.pmdDest = pmd;
    mei.pmdList[0].clsData = MMD_BUFFER;
    mei.pmdList[0].pData = ((IDIB *)pMe->m_pEditIn)->pBmp;
    if (bMakeThumbnail)
    {
      mei.pmdList[1].clsData = MMD_BUFFER;
      mei.pmdList[1].pData = ((IDIB *)pMe->m_pEditThumbnail)->pBmp;
      mei.nCount = 2;
    }
    else
    {
      mei.nCount = 1;
    }
  }
  else if (IBITMAP_QueryInterface(pMe->m_pEditIn, AEEIID_YCBCR, (void **)&pYCbCr) == SUCCESS)
  {
    // Create thumbnail if image size >= VGA
    iImgSize = Qalbum_GetImageSize((int)pYCbCr->cx, (int)pYCbCr->cy);
    if (iImgSize <= (int)QCAM_SIZE_VGA)   // size table is in reverse order
    {
      bMakeThumbnail = TRUE;
      if (pYCbCr->cx > pYCbCr->cy)
      {
        // swap cx, cy
        sizeThumb.cx = pSize->cy;
        sizeThumb.cy = pSize->cx;
      }

      if (IBITMAP_CreateCompatibleBitmap(pMe->m_pEditIn, &pMe->m_pEditThumbnail,
                                         (uint16)sizeThumb.cx, (uint16)sizeThumb.cy) != SUCCESS)
      {
        bMakeThumbnail = FALSE;
      }
      else
      {
        // Down size main image to thumbnail
        retVal = IIPL_ChangeSize(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditThumbnail);
        if (Qalbum_DisplayError(pMe, retVal) != SUCCESS)
        {
          goto Done;
        }
      }
    }

    // Do encoding if everything is ready
    ZEROAT(&spec);
    spec.bQuality = TRUE;
    spec.size.nQuality = 90;
    spec.wMainWidth = (uint16)pYCbCr->cx;
    spec.wMainHigh = (uint16)pYCbCr->cy;
    spec.wMainInColor = (uint16)pYCbCr->uScheme;
    if (bMakeThumbnail)
    {
      spec.wThumbWidth = (uint16)sizeThumb.cx;
      spec.wThumbHigh  = (uint16)sizeThumb.cy;
      spec.wThumbInColor = (uint16)pYCbCr->uScheme;
    }
    if (pMe->m_pDisplayImage)
    {
      retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_INFO,
                              &parm, sizeof(OEMImageParmExt));
    }
    if (retVal == SUCCESS)
      spec.pHeaderBuf = parm.pData;
    else
      spec.pHeaderBuf = NULL;

    ZEROAT(&mei);
    mei.pmdList = pmdList;

    mei.pmdDest = pmd;
    mei.pmdList[0].clsData = MMD_BUFFER;
    mei.pmdList[0].pData = ((IYCbCr *)pMe->m_pEditIn)->pLuma;
    if (bMakeThumbnail)
    {
      mei.pmdList[1].clsData = MMD_BUFFER;
      mei.pmdList[1].pData = ((IYCbCr *)pMe->m_pEditThumbnail)->pLuma;
      mei.nCount = 2;
    }
    else
    {
      mei.nCount = 1;
    }
  }
  else
  {
    //Report error
    (void)Qalbum_DisplayError(pMe, EUNSUPPORTED);
    goto Done;
  }

  mei.pEncodeSpec = (void *)&spec;
  mei.dwSpecSize = sizeof(ImageEncodeSpec);

  //Function will not callback if failed in some cases. handle it here to avoid UI freeze.
  if (IMEDIAUTIL_EncodeJPEG(pMe->m_pMediaUtil, &pMe->m_EncodeResult, &mei, &pMe->m_EncodeCB) != SUCCESS)
  {
    MSG_HIGH("Writing of JPEG image failed",0,0,0);
    pMe->m_EncodeResult.nResult = EFAILED;
    Qalbum_GetEncodeCB((void *)pMe);
  }

Done:
  CAM_RELEASEIF(pDIB);
  CAM_RELEASEIF(pYCbCr);
}

/*===========================================================================
FUNCTION Qalbum_SavePngImage

  DESCRIPTION
    This function saves the current image in PNG format.

  PARAMETERS:
    pMe - Pointer to  Qalbum struct
    pmd - Pointer to the AEEMediaData struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_SavePngImage(Qalbum *pMe, AEEMediaData *pmd)
{
  AEEMediaEncodeInfo mei;
  AEEMediaData       pmdList;
  ImageEncodeSpec    spec;

  if (!pMe || !pMe->m_pMediaUtil)
    return;

  CALLBACK_Init(&pMe->m_EncodeCB, Qalbum_GetEncodeCB, pMe);

  ZEROAT(&spec);
  spec.bQuality = TRUE;
  spec.size.nQuality = 90;
  spec.wMainWidth = ((IDIB *)pMe->m_pEditIn)->cx;
  spec.wMainHigh = ((IDIB *)pMe->m_pEditIn)->cy;
  spec.wMainInColor = ((IDIB *)pMe->m_pEditIn)->nColorScheme;

  // no thumbnails in PNG

  ZEROAT(&mei);
  mei.pmdList = &pmdList;
  mei.pmdDest = pmd;
  mei.pmdList->clsData = MMD_BUFFER;
  mei.pmdList->pData = ((IDIB *)pMe->m_pEditIn)->pBmp;
  mei.nCount = 1;
  mei.pEncodeSpec = (void *)&spec;
  mei.dwSpecSize = sizeof(ImageEncodeSpec);

  //Function will not callback if failed in some cases. handle it here to avoid UI freeze.
  if (IMEDIAUTIL_EncodeMedia(pMe->m_pMediaUtil, &pMe->m_EncodeResult, AEECLSID_PNG,
                             &mei, &pMe->m_EncodeCB) != SUCCESS)
  {
    MSG_HIGH("Writing of PNG image failed",0,0,0);
    pMe->m_EncodeResult.nResult = EFAILED;
    Qalbum_GetEncodeCB((void *)pMe);
  }
}

#ifdef FEATURE_JPEGD_RESIZE
/*=============================================================================
FUNCTION: Qalbum_DlgSlideShowSetText

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
static void Qalbum_DlgSlideShowSetText(ITextCtl *ctrl, uint16 nVal, uint16 size)
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
FUNCTION: Qalbum_DlgSlideShowGetText

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
static uint16 Qalbum_DlgSlideShowGetText(ITextCtl *ctrl, uint16 size)
{
  AECHAR valStr[QCAM_5_CHAR_LEN];
  char valChar[QCAM_5_CHAR_LEN];
  uint16 nVal = 0;

  //Get user entry and set the member variable.
  (void)ITEXTCTL_GetText(ctrl, valStr, size);
  //Force user to enter a number before leave the dialog.
  if (WSTRLEN(valStr) > 0)
  {
    (void)WSTRTOSTR(valStr, valChar, sizeof(valChar));
    nVal = (uint16)ATOI(valChar);
  }

  return nVal;
}


/*===========================================================================
FUNCTION Qalbum_DlgSlideShowEventHandler

  DESCRIPTION
    Key handler for Handling slide show dialog when resizing feature
    is enabled 

  PARAMETERS:
    pUser   - Pointer to the applet instance, map to QCam * pMe
    evt     - AEEEvent id
    wParam  - 2 byte param
    dwParam - 4 byte param

  RETURN VALUE:
   None
===========================================================================*/
boolean Qalbum_DlgSlideShowEventHandler(void * pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
  Qalbum *pMe;
  IDialog * pCurrentDialog;
  IMenuCtl* pICurrentMenu1 = NULL;
  IMenuCtl* pICurrentMenu2 = NULL;
  ITextCtl  *textCtl1;
  ITextCtl  *textCtl2;
  int dirLen;
  int dateDirLen;
  uint16 ResizeWidth;
  uint16 ResizeHeight;

  if (!pUser)
    return FALSE;

  pMe = (Qalbum *)pUser;

  Qalbum_GetDirLen(pMe, &dirLen, &dateDirLen);

  pCurrentDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pCurrentDialog)
  { 
    textCtl1  = (ITextCtl  *)IDIALOG_GetControl(pCurrentDialog, IDI_SLIDESHOW_WIDTH);
    textCtl2  = (ITextCtl  *)IDIALOG_GetControl(pCurrentDialog, IDI_SLIDESHOW_HEIGHT);
    pICurrentMenu1 = (IMenuCtl *)IDIALOG_GetControl(pCurrentDialog, IDM_SLIDESHOW_RESIZE);
    pICurrentMenu2 = (IMenuCtl *)IDIALOG_GetControl(pCurrentDialog, IDM_ASPECT_RATIO);
    if (!textCtl1 || !textCtl2 || !pICurrentMenu1 || !pICurrentMenu2)
      return FALSE;
  }
  else
    return FALSE;

  switch (evt)
  {
  case EVT_DIALOG_START:
    {
      AEERect rcMenu;
      QCam_SetMenuStyle(pICurrentMenu1, 0, 0);
      SETAEERECT(&rcMenu, 0, pMe->di.cyScreen - QCAM_BUTTON_MENU_HEIGHT, 
                 pMe->di.cxScreen, QCAM_BUTTON_MENU_HEIGHT);
      IMENUCTL_SetRect(pICurrentMenu1, &rcMenu);
      IMENUCTL_SetSel(pICurrentMenu1, IDC_RESIZE_SAVE);
      (void)IMENUCTL_Redraw(pICurrentMenu1);

      // Set resize width and height with initial values"
      Qalbum_DlgSlideShowSetText(textCtl1, pMe->m_dispSizes.wPhotoWidth, QCAM_5_CHAR_LEN);
      Qalbum_DlgSlideShowSetText(textCtl2, pMe->m_dispSizes.wPhotoHeight, QCAM_5_CHAR_LEN);

      //Update aspect ratio check mark
      QCam_SetMenuStyle(pICurrentMenu2, 0, 0);
      IMENUCTL_SetSel(pICurrentMenu2, IDC_ASPECT_RATIO);
      (void)IMENUCTL_Redraw(pICurrentMenu2);

      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE); 
      return TRUE;
    }
  case EVT_KEY:
    if (wParam == (uint16)AVK_CLR)
    {
      (void)ISHELL_EndDialog(pMe->a.m_pIShell);
      pMe->eState = QCAM_STATE_SLIDESHOW;
      pMe->m_bAutoPlayInit = TRUE;
      Qalbum_AutoPlay(pMe);
    }
    return TRUE;

  case EVT_KEY_PRESS:
    break;

  case EVT_KEY_RELEASE:
    break;

  case EVT_COMMAND:
    if (IMENUCTL_IsActive(pICurrentMenu1))
    {
      switch (wParam)
      {
      case IDC_RESIZE_SAVE:
        ResizeWidth = Qalbum_DlgSlideShowGetText(textCtl1, QCAM_5_CHAR_LEN);
        ResizeHeight = Qalbum_DlgSlideShowGetText(textCtl2, QCAM_5_CHAR_LEN); 
        if ((ResizeWidth > pMe->di.cxScreen) || (ResizeHeight > pMe->di.cyScreen))
        {
          MSG_HIGH("Entered width and height are larger than display size", 0, 0, 0);  
        }
        else
        {
          if (ResizeWidth < 80)
          {
            ResizeWidth = 80;  
          }
          if (ResizeHeight < 80)
          {
            ResizeHeight = 80;  
          }
          pMe->m_dispSizes.wPhotoWidth = ResizeWidth;
          pMe->m_dispSizes.wPhotoHeight = ResizeHeight;
        }
      //lint -fallthrough for key assignment.
      case IDC_RESIZE_CANCEL:
        (void)ISHELL_EndDialog(pMe->a.m_pIShell);
        pMe->eState = QCAM_STATE_SLIDESHOW;
        pMe->m_bAutoPlayInit = TRUE;
        MSG_HIGH("Start playing slideshow", 0, 0, 0);
        Qalbum_AutoPlay(pMe);
        return TRUE;

      default:
         break;
      }
      return FALSE;
    }

    if (IMENUCTL_IsActive(pICurrentMenu2))
    {
      switch (wParam)
      {
      case IDC_ASPECT_RATIO:
        pMe->m_bAspectRatio = !pMe->m_bAspectRatio;
        QCam_SetItemImg(pICurrentMenu2, pMe->m_pResFile, IDC_ASPECT_RATIO, 
                        (uint16)(pMe->m_bAspectRatio ? IDB_CHECKED : IDB_UNCHECKED));
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

  return TRUE;
  //lint -save -e715 symbol 'dwParam' not used here
}
#endif // FEATURE_JPEGD_RESIZE
#endif //#if defined(FEATURE_APP_QALBUM)

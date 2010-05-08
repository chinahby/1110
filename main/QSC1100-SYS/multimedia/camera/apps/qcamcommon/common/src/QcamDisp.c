/*===========================================================================

FILE: QCamDisp.c

GENERAL DESCRIPTION
    This file contains share functions for BREW application qcamera and qalbum.

    Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamcommon/main/latest/src/QcamDisp.c#1 $ $DateTime: 2008/05/12 10:09:09 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 06/18/07  NC     Added QCamDisp_GetResizeRatio and updated QCamDisp_GetResizedRect.
 05/11/07  NC     Fixed bug in handling different color format.
 04/25/07  NC     Fixed lint errors
 04/19/07  SK     UI Support for WVGA Display
 04/17/07  NC     Updated QCamDisp_IsSizeScreen to get more accurate checking. 
 04/13/07  TV     When adjusting Display Height and Width make sure that cx * cy is a mult of 64
 04/02/07  NC     Suppressed lint error from BREW macro ISHELL_CreateInstance.
 04/02/07  SK     Return the display size directly if image orientation is the same as display 
 03/30/07  TV     Fixed type in AdjustDisplaySize where we were comparing cy to cx in stead of cx to cx
 03/28/07  TV     Added methods to fit image sizes to the display keeping the aspect ratio
 03/23/07  KV     Added dynamic font size parameters in QCamDisp_GetRect() and QCamDisp_GetDisplayRect()
 02/23/07  NC     Updated QCamDisp_DisplayOvOnTV for more IMMOverlay setting.
 02/12/07  NC     Fixed full screen transparent issue.
 02/02/07  NC     Temp solution for WVGA LCD before service layer support.
 12/19/06  DK     Cleared pBuf in QCam_LoadPNGFrame() to avoid unwanted free()
 12/12/06  GM     Changes for FrameCallBack Mode
 11/21/06  NC     Replace IOverlay with IMMOverlay for BREW 4.0.
 11/17/06  TV     Changed DrawMenuTitle to display the text just after the image, instead of hardcodeing img width
 11/09/06  NC     Fixed compiler errors when FEATURE_CAMERA_MULTIPLE_DISPLAY is defined.
 11/06/06  NC     Fixed IOverlay with MDP2 full screen display issues.
 11/02/06  NC     Created share function QCamDisp_DisplayOvOnTV for multiple displays.
 10/02/06  NC     Added new IOVerlay share functions for MDP2 overlay interface.
 08/22/06  RK     Updated QCamDisp_DrawYCbCrImage(),QCamDisp_DrawYCbCrImage() for error checking.
 04/05/06  HV     Used IYCbCrFactory interface to replace YCbCr in QCamDisp_DrawYCbCrImage().
 03/23/06  NC     Used IIPL interface to replace ipl API in QCamDisp_DrawYCbCrImage().
                  Enimilated double data type in calculating radio.
 11/30/05  NC     Clean up lint errors from Lint v8.0.
 11/08/05  NC     Update per the new header file.
 10/27/05  JN     Fixed memory leak in QCam_LoadPNGFrame.
 10/20/05  NC     Fixed new lint errors.
 09/27/05  JN     Fixed couple of lint clean up changes.
 09/15/05  NC     Lint errors clean up and NULL pointer checking.
 08/17/05  JN     Added more arguments to QCamDisp_DrawImage().
 06/17/05  JN     Updated QcamDisp_ShowMsg to use rc coordinates to draw text.
 05/23/05  JN     Centralize font definitions.
 04/11/05  JN     Added more arguments to QCamDisp_DrawYCbCrImage().
 03/24/05  NC     Updated QCamDisp_ShowMsg()
 03/23/05  JN     Removed unused resize functions.
 03/22/05  JN     Use pitch field to determine image width.
 03/22/05  NC     Released DIB pointer after register MDP to avoid memory leak.
 03/10/05  NC     Update function parameters to eliminate lint errors.
 03/02/08  JN     Added support to draw YCbCr image.
 02/16/05  NC     Added #ifdef FEATURE_APP_QCAMERA condition.
 01/21/05  JN     Bug fix in QCamDisp_DrawIcon().
 01/14/05  NC     Update MDP feature functions to use IOverlay interface.
 11/22/04  NC     Added QCam_LoadPNGFrame() to load a PNG file with IImageDecoder.
 11/17/04  NC     Added QCamDisp_MdpDeregister() for clean up.
 11/08/04  NC     Added QCamDisp_InitMdp() for start up MDP.
 11/05/04  JN     Changed QCamDisp_DrawSelect() to pass in old rect value.
 11/02/04  JN     Changed QCamDisp_ShowMemoryFull() -> QCamDisp_ShowMsg();
 10/27/04  NC     Add QCam_GetPNGImageData function
 10/14/04  NC     Add MDP register functions
 09/21/04  NC     Add more features and redesign function names.
 06/16/04  NC     Initial design of the share file qcamdisp.c.
 ============================================================================*/
//Deal with lint problems in BREW headers
//lint -emacro(570,IDISPLAY_ClearScreen)
//lint -emacro(570,IDISPLAY_EraseRect)
//lint -emacro(570,IDISPLAY_FillRect)
//lint -emacro(666,ISHELL_CreateInstance)

//lint -e740, Unusual pointer cast for BREW macros
//lint -e611, Suspicious cast for BREW macros
//lint -e801, Use of goto is deprecated
/*===============================================================================
INCLUDES AND DEFINITIONS
=============================================================================== */
#include "QcamDisp.h"

#ifdef FEATURE_APP_QCAMERA
#include "QcamDef.h"
#include "AEEStdLib.h"
#include "IForceFeed.h"
#include "IImageDecoder.h"
#include "AEEFile.h"
#include "PNGDecoder.bid"

#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
#include "AEELogicalDisplayCfg.h"
#include "AEECLSID_MMOVERLAYMGR.BID"
#include "AEECLSID_LOGICALDISPLAYCFG.BID"
#include "AEECLSID_PHYSICALDISPLAY.BID"
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY

#ifndef AEE_SIMULATOR
#include "msg.h"
#endif
static uint16 QCamDisp_GetX(uint16 dx, int16 idx, int16 start_idx, ImageType eType);
static uint16 QCamDisp_GetY(uint16 dy, int16 idx, int16 start_idx, ImageType eType, int16 TitleHeight);
/*===============================================================================
FUNCTION DECLARATIONS
=============================================================================== */
/*===========================================================================
FUNCTION QCamDisp_InitScreen

  DESCRIPTION
    This function initializes the display screen.

  PARAMETERS:
    pIDisp  - pointer to IDisplay interface
    dx - The screen width
    dy - The screen height

  RETURN VALUE:
    NONE
===========================================================================*/
void QCamDisp_InitScreen(IDisplay *pIDisp, uint16 dx, uint16 dy)
{
  AEERect rc = {0};

  if (!pIDisp)
    return;

  // Clear the screen
  rc.dx = (int16)dx;
  rc.dy = (int16)dy;
  IDISPLAY_EraseRect(pIDisp, &rc);
}

/*===========================================================================
FUNCTION QCamDisp_GetX

  DESCRIPTION
    This function calculates the x position of the image.

  PARAMETERS:
    dx - The image width
    idx - The image index
    start_idx - The page index for the image
    eType - image type

  RETURN VALUE:
    x position of the image
===========================================================================*/
static uint16 QCamDisp_GetX(uint16 dx, int16 idx, int16 start_idx, ImageType eType)
{
  uint16 wStart = 0;
  uint16 wOffset = 0;
  int16  wNumX = 1;
  uint16 wPosX = 0;

  switch (eType)
  {
  case DIRIMAGE:
  case DIRTEXT:
  case SELDIR:
    wStart = QCAM_DIR_X_START;
    wOffset = dx + QCAM_DIR_X_OFFSET;
    wNumX = QCAM_DIRS_X;
    break;
  case THUMBNAIL:
  case SELTHUMBNAIL:
    wStart = QCAM_THUMBNAIL_X_START;
    wOffset = dx + QCAM_THUMBNAIL_X_OFFSET;
    wNumX = QCAM_THUMBNAILS_X;
    break;
  case PHOTO:
    wStart = QCAM_PICTURE_X_POS;
    wOffset = 0;
    wNumX = 1;
    break;
  default:
    break;
  }

  //It is the current thumbnail image start x position
  //If it is for the selection rect or dir image, need more offset on it.
  wPosX = (uint16)(wStart + ((idx - start_idx) % wNumX) * wOffset);
  if ((eType == SELDIR) || (eType == SELTHUMBNAIL))
    wPosX -= QCAM_SEL_OFFSET;
  else if (eType == DIRIMAGE)
    wPosX += (dx - QCAM_DIR_IMAGE_WIDTH) / 2;

  return wPosX;
}

/*===========================================================================
FUNCTION QCamDisp_GetY

  DESCRIPTION
    This function calculate the y position of the image.

  PARAMETERS:
    dy - The image height
    idx - The image index
    start_idx - The page index for the image
    eType - image type

  RETURN VALUE:
    y position of the image
===========================================================================*/
static uint16 QCamDisp_GetY(uint16 dy, int16 idx, int16 start_idx, ImageType eType, 
                            int16 TitleHeight)
{
  uint16 wStart = 0;
  uint16 wOffset = 0;
  int16  wNumX = 1;
  uint16 wPosY = 0;

  switch (eType)
  {
  case DIRIMAGE:
  case DIRTEXT:
  case SELDIR:
    wStart = (uint16)TitleHeight + 1;
    wOffset = dy + QCAM_DIR_Y_OFFSET;
    wNumX = QCAM_DIRS_X;
    break;
  case THUMBNAIL:
  case SELTHUMBNAIL:
    wStart = (uint16)TitleHeight + 1;
    wOffset = dy + QCAM_THUMBNAIL_Y_OFFSET;
    wNumX = QCAM_THUMBNAILS_X;
    break;
  case PHOTO:
    wStart = (uint16)TitleHeight;
    wOffset = 0;
    wNumX = 1;
    break;
  default:
    break;
  }

  //It is the current thumbnail image start y position
  //If it is for the selection rect or dir text, need more offset on it.
  wPosY = (uint16)(wStart + ((idx - start_idx) / wNumX) * wOffset);
  if ((eType == SELDIR) || (eType == SELTHUMBNAIL))
    wPosY -= QCAM_SEL_OFFSET;
  else if (eType == DIRTEXT)
    wPosY += (dy - QCAM_DIR_TEXT_HEIGHT);

  return wPosY;
}

/* ==========================================================================
FUNCTION QCamDisp_GetResizeRatio
  DESCRIPTION
    This function calculates the downsize ratio based on the image size and
    display rectangle. The radio is scaled up 100 to avoid using double type.

  PARAMETERS:
    rc [in]     - display rectangle
    dx [in]     - image width
    dy [in]     - image height

  RETURN VALUE:
    resize ratio scaled up 100

============================================================================= */
int16 QCamDisp_GetResizeRatio(AEERect rc, int16 dx, int16 dy)
{
  int16 xRatio = (int16)((rc.dx * 100) / dx); //scale up 100 to avoid 0 result
  int16 yRatio = (int16)((rc.dy * 100) / dy); //scale up 100 to avoid 0 result

  return MIN(xRatio, yRatio);
}

/*=====================================================================
FUNCTION QCamDisp_GetResizedRect

  DESCRIPTION: This is a helper function to calculate the reduced size for displaying
               a large image in a smaller screen. To avoid using double data type, we
               multiple the raio with 100 to get a non zero int first and devide by 100
               later to get the correct size.

  PARAMETERS:
    rcDisp - Display size for the image
    dxImg  - Image width
    dyImg  - Image height

  RETURN VALUE:
    AEERect - Reduced image size scaled to fit the display size.
======================================================================*/
AEERect QCamDisp_GetResizedRect(AEERect rcDisp, int16 dxImg, int16 dyImg)
{
  AEERect rc;
  int16 ratio  = QCamDisp_GetResizeRatio(rcDisp, dxImg, dyImg) / 2;  // make dx, dy even #s

  // scale down 100 to get the real value.
  rc.dx = (int16)(((dxImg * ratio) / 100) * 2); // change may yeild odd #
  rc.dy = (int16)(((dyImg * ratio) / 100) * 2);
  // Center the image in display rect
  rc.x = rcDisp.x + ((rcDisp.dx - rc.dx) / 2);
  rc.y = rcDisp.y + ((rcDisp.dy - rc.dy) / 2);

  return rc;
}


/*=====================================================================
FUNCTION Qalbum_IsSizeScreen

  DESCRIPTION: This function check if dx and dy is the selected sreen size

  PARAMETERS:
    dx    - screen width
    dy    - screen height
    size  - QCamSizeType size

  RETURN VALUE:
    TRUE if the size match.
======================================================================*/
boolean QCamDisp_IsSizeScreen(uint16 dx, uint16 dy, int size)
{
  AEESize *pSize = NULL;

  if (size >= (int)QCAM_SIZE_COUNT)
    return FALSE;

  pSize = QCam_GetImageDimension(size);
  if (((dx == pSize->cx) && (dy >= pSize->cy)) ||
      ((dx == pSize->cy) && (dy >= pSize->cx)))
    return TRUE;
  else
    return FALSE;
}
/*===========================================================================
FUNCTION QCamDisp_GetDisplayRect

  DESCRIPTION
    This function gets the full image display area.

  PARAMETERS:
    screenDx - [In]  The screen width return from get device info
    screenDy - [In]  The screen height return from get device info
    pRect    - [Out] Display rect size

  RETURN VALUE:
    None
===========================================================================*/
void QCamDisp_GetDisplayRect(uint16 screenDx, uint16 screenDy,
                             uint16 dyOffset, AEERect *pRect, int16 TitleHeight)
{
  //Add the offset for title and menu to the screen size to get the display area.
  //If title and menu area registered for overlay, it will be no offset.
  AEERect rc;

  if (!pRect)
    return;
//lint -save -e715 symbol dyOffset not referenced here
/* Set the Preview Screen size with the right offsets,if in Frame Call back
   Mode
 */ 
#ifdef FEATURE_OVERLAY
  //If use MDP update or in second lcd, no offset for the display rect.
  SETAEERECT(&rc, 0, 0, screenDx, screenDy);
#else
  SETAEERECT(&rc, 0, TitleHeight, screenDx,
             screenDy - (TitleHeight + dyOffset));
#endif //FEATURE_OVERLAY

(void)MEMCPY(pRect, &rc, sizeof(AEERect));
//lint -restore
}

/*===========================================================================
FUNCTION QCamDisp_GetCenterRect

  DESCRIPTION
    This function moves the display rect to the center if the image
    dx and/or dy is smaller than the display rect.

  PARAMETERS:
    rc - The image display rect. It is updated if dx/dy smaller than its dx/dy
    dx - The image real width
    dy - The image real height

  RETURN VALUE:
    AEERect in the center of display screen
===========================================================================*/
AEERect QCamDisp_GetCenterRect(AEERect rc, int16 dx, int16 dy)
{
  AEERect rcCenter;
  int xoffset;
  int yoffset;

  xoffset = (rc.dx - dx) / 2;
  yoffset = (rc.dy - dy) / 2;
  if (xoffset < 0)
  {
     xoffset = 0;
  }
  if (yoffset < 0)
  {
     yoffset = 0;
  }

  SETAEERECT(&rcCenter, rc.x + xoffset, rc.y + yoffset,
              rc.dx - xoffset * 2, rc.dy - yoffset * 2);
  return rcCenter;
}

/*===========================================================================
FUNCTION QCamDisp_AdjustDisplaySizeFitX

  DESCRIPTION
    This function adjusts the display size so that it keeps as close to the
    proper ratio as possible but still a mult of 4.  The X size for the 
    image is too big. cx * cy must also be a mult of 64.  So make sure that 
    our mult of 4 is also an even mult of 4  for both cx and cy (devide by 4
    and if the result is odd, subtract 4 from the cx or cy)

  PARAMETERS:    
    pImgRatio - The image ratio to keep
    dispSize - the current display size

  RETURN VALUE:
    AEESize that is the new screen size
===========================================================================*/
AEESize QCamDisp_AdjustDisplaySizeFitX(AEESize* pImgRatio, AEESize dispSize)
{
  AEESize newSize;
  
  // Make sure that we are a mult of 4 (and an even mult of 4)
  newSize.cx = (dispSize.cx / 4) * 4;
  if (0 != ((newSize.cx / 4) & 0x1))
  {
    newSize.cx -= 4;
  }

  newSize.cy = (int32)(newSize.cx * ((double)pImgRatio->cx / (double)pImgRatio->cy));
  // Make sure that we are a mult of 4 (and an even mult of 4)  
  newSize.cy = (newSize.cy / 4) * 4; 
  if (0 != ((newSize.cy / 4) & 0x1))
  {
    newSize.cy -= 4;
  }  
    
  return newSize;
}

/*===========================================================================
FUNCTION QCamDisp_AdjustDisplaySizeFitY

  DESCRIPTION
    This function adjusts the display size so that it keeps as close to the
    proper ratio as possible but still a mult of 4.  The Y size for the 
    image is too big  cx * cy must also be a mult of 64.  So make sure that 
    our mult of 4 is also an even mult of 4 for both cx and cy (devide by 4 and
    if the result is odd, subtract 4 from the cx or cy)

  PARAMETERS:    
    pImgRatio - The image ratio to keep
    dispSize - the current display size

  RETURN VALUE:
    AEESize that is the new screen size
===========================================================================*/
AEESize QCamDisp_AdjustDisplaySizeFitY(AEESize* pImgRatio, AEESize dispSize)
{
  AEESize newSize;

  // Make sure that we are a mult of 4 (and an even mult of 4)
  newSize.cy = (dispSize.cy / 4) * 4;
  if (0 != ((newSize.cy / 4) & 0x1))
  {
    newSize.cy -= 4;
  }

  newSize.cx = (int32)(newSize.cy * ((double)pImgRatio->cy / (double)pImgRatio->cx));
  // Make sure that we are a mult of 4 (and an even mult of 4)
  newSize.cx = (newSize.cx / 4) * 4;
  if (0 != ((newSize.cx / 4) & 0x1))
  {
    newSize.cx -= 4;
  }
    
  return newSize;
}

/*===========================================================================
FUNCTION QCamDisp_AdjustDisplaySize

  DESCRIPTION
    This function adjusts the display size so that it keeps as close to the
    proper ratio as possible but still a mult of 4.  It trys fitting the
    large dimention then the small dimention.This function is called when 
    both dx and dy of image size are larger than display size.    

  PARAMETERS:    
    pImgRatio - The image ratio to keep
    dispSize - the current display size

  RETURN VALUE:
    AEESize that is the new screen size
===========================================================================*/
AEESize QCamDisp_AdjustDisplaySize(AEESize* pImgRatio, AEESize dispSize)
{
  AEESize newSize;
  AEESize tmpDispSize; 

  tmpDispSize.cx = dispSize.cx;
  tmpDispSize.cy = dispSize.cy;

  if (dispSize.cx > dispSize.cy)
  {
    //We return the display size directly if image orientation is the same as display
    //Otherwise, we calculate the size to fit the dimention.
    if ((int32)pImgRatio->cx < (int32)pImgRatio->cy)
    {
      MSG_MED("Ignoring DisplaySize Adjust.", 0, 0, 0);
      return dispSize; 
    }
    else
    {
      do
      {
        // our X direction is bigger than our Y, so try fitting that dimention
        newSize = QCamDisp_AdjustDisplaySizeFitX(pImgRatio, tmpDispSize);

        // If the new cy is still to big try fitting the Y
        if (newSize.cy > dispSize.cy)
        {
          newSize = QCamDisp_AdjustDisplaySizeFitY(pImgRatio, tmpDispSize);       
        
          if (newSize.cx > dispSize.cx)
          {
            // We still don't fit, lets try reducing our fit to sizes by 4
            // This way we should keep the mult of 4 more easily
            tmpDispSize.cx -=4;
            tmpDispSize.cy -=4;
          }
        }
      } while ((newSize.cy > dispSize.cy) || (newSize.cx > dispSize.cx));
    }
  }
  else
  {
    //We return the display size directly if image orientation is the same as display
    //Otherwise, we calculate the size to fit the dimention.
    if ((int32)pImgRatio->cx > (int32)pImgRatio->cy)
    {
      MSG_MED("Ignoring DisplaySize Adjust.", 0, 0, 0);
      return dispSize; 
    }
    else
    {
      do
      {
        // our Y direction is bigger than our X, so try fitting that dimention
        newSize = QCamDisp_AdjustDisplaySizeFitY(pImgRatio, tmpDispSize);

        // If the new cx is still to big try fitting the X
        if (newSize.cx > dispSize.cx)
        {
          newSize = QCamDisp_AdjustDisplaySizeFitX(pImgRatio, tmpDispSize);
        
          if (newSize.cy > dispSize.cy)
          {
            // We still don't fit, lets try reducing our fit to sizes by 4
            // This way we should keep the mult of 4 more easily
            tmpDispSize.cx -=4;
            tmpDispSize.cy -=4;
          }
        }
      } while ((newSize.cy > dispSize.cy) || (newSize.cx > dispSize.cx));
    }
  }  
    
  return newSize;
}

/*===========================================================================
FUNCTION QCamDisp_GetRect

  DESCRIPTION
    This function gets the display rect based on the give parameters.

  PARAMETERS:
    dx - width of the display rect
    dy - height of the display rect
    idx - index of the display image
    start - start index of the screen
    eType - display image type

  RETURN VALUE:
    Display rect
===========================================================================*/
AEERect QCamDisp_GetRect(uint16 dx, uint16 dy, int16 idx, int16 start, ImageType eType, 
                         int16 TitleHeight)
{
  AEERect rc = {0};
  uint16 x, y;

  //calculate the x and y.
  x = QCamDisp_GetX(dx, idx, start, eType);
  y = QCamDisp_GetY(dy, idx, start, eType, TitleHeight);

  //set rect
  SETAEERECT (&rc, x, y, dx, dy);
  return rc;
}

/*===========================================================================
FUNCTION QCamDisp_UpdateStaticField

  DESCRIPTION
    This function gets the date folder and display it in the static field.

  PARAMETERS:
    dirCtrl - pointer to IStatic interface
    fileName - file name to update the static field("root/date folder/name")
    dirLen - root directory length
    dateDirLen - date folder length


  RETURN VALUE:
    NONE
===========================================================================*/
void QCamDisp_UpdateStaticField(IStatic *dirCtrl, const char *fileName, uint32 dirLen, uint32 dateDirLen)
{
  AECHAR szText[FILE_NAME_SIZE];

  if (!dirCtrl || ! fileName)
    return;

  (void)STRTOWSTR(&fileName[dirLen], szText, sizeof(szText));
  szText[dateDirLen - dirLen] = 0;

  (void)ISTATIC_SetText(dirCtrl, NULL, szText, QCAM_FONT_TITLE, QCAM_FONT_TITLE);
  (void)ISTATIC_Redraw(dirCtrl);
}

/*===========================================================================
FUNCTION QCamDisp_UpdateTextField

  DESCRIPTION
    This function gets the file name from folder name and displays it on the text control.

  PARAMETERS:
    textCtl - pointer to ITextCtl interface
    fileName - file name to update the text field("date folder/name")
    dateDirLen - date folder length

  RETURN VALUE:
    NONE
===========================================================================*/
void QCamDisp_UpdateTextField(ITextCtl *textCtl, const char *fileName, int dateDirLen)
{
  AECHAR szText[FILE_NAME_SIZE];

  if (!textCtl || !fileName)
    return;

  (void)STRTOWSTR(&fileName[dateDirLen], szText, sizeof(szText));

  // Remove the extension
  szText[WSTRLEN(szText) - 4] = 0;

  (void)ITEXTCTL_SetText(textCtl, szText, -1);
  ITEXTCTL_SetCursorPos(textCtl, TC_CURSOREND);
  (void)ITEXTCTL_Redraw(textCtl);
}

/*===========================================================================
FUNCTION QCamDisp_ShowMsg

  DESCRIPTION
    This function Displays a message on the screen to the user indicating
    that the disk is full and no more pictures can be saved.

  PARAMETERS:
    pIDisp - Pointer to the IDisplay interface
    szBuf -  Pointer to the message buffer
    rc - display area

  RETURN VALUE:
    None
===========================================================================*/
void QCamDisp_ShowMsg(IDisplay *pIDisp, const AECHAR *szBuf, AEERect rc)
{
  if (!pIDisp || !szBuf)
    return;

  // Clear the screen
  IDISPLAY_EraseRect(pIDisp, &rc);

  // Show message
  (void)IDISPLAY_DrawText(pIDisp, QCAM_FONT_TEXT, szBuf,
                          -1, rc.x, rc.y, NULL,
                          IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_UNDERLINE);

  // update screen
  IDISPLAY_UpdateEx(pIDisp, FALSE);
}

/*===========================================================================
FUNCTION QCamDisp_ShowMessage

  DESCRIPTION
    This function Displays a general message on the screen using ISHELL_MessageBoxText.
    The screen is closed by pressing the CLR key.

  PARAMETERS:
    pIShell - Pointer to the IShell interface
    title - Message box title. May be NULL
    text - Message content to display

  RETURN VALUE:
    None
===========================================================================*/
void QCamDisp_ShowMessage(IShell *pIShell, const char *title, const char *text)
{
  AECHAR szBufTitle[TEXT_STRING_SIZE] = {0};
  AECHAR szBufText[PROMPT_SIZE]       = {0};

  if (!pIShell || !text)
    return;

  if (title)
  {
    (void)STRTOWSTR(title, szBufTitle, sizeof(szBufTitle));
  }
  (void)STRTOWSTR(text, szBufText, sizeof(szBufText));
  (void)ISHELL_MessageBoxText(pIShell, szBufTitle, szBufText);
}

/*===========================================================================
FUNCTION QCamDisp_DrawDirImage

  DESCRIPTION
    This function draws the dir image in the position specified by idx.

  PARAMETERS:
    pIDisp - pointer to IDisplay interface.
    pDirImage - dir image pointer
    imgRC - rect for Dir image

  RETURN VALUE:
    NONE
===========================================================================*/
void QCamDisp_DrawDirImage(IDisplay *pIDisp, IImage *pDirImage, AEERect imgRC)
{
  if (!pIDisp || !pDirImage)
    return;

  IDISPLAY_EraseRect (pIDisp, &imgRC);
  IIMAGE_Draw(pDirImage, imgRC.x, imgRC.y);
}

/*===========================================================================
FUNCTION QCamDisp_DrawDirText

  DESCRIPTION
    This function draws the dir text in the position specified by idx.

  PARAMETERS:
    pIDisp - pointer to IDisplay interface.
    pDirText - dir text pointer
    textRC - rect for the text

  RETURN VALUE:
    NONE
===========================================================================*/
void QCamDisp_DrawDirText(IDisplay *pIDisp, const char *pDirText, AEERect textRC)
{
  AECHAR   szBuf[TEXT_STRING_SIZE];

  if (!pIDisp || ! pDirText)
    return;

  //Get display rect for dir text
  textRC.dy = QCAM_DIR_TEXT_HEIGHT;

  (void)STRTOWSTR(pDirText, szBuf, sizeof(szBuf));
  IDISPLAY_EraseRect(pIDisp, &textRC);
  (void)IDISPLAY_DrawText(pIDisp, QCAM_FONT_TITLE, szBuf,
                    -1, 0, 0, &textRC, IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
}

/*===========================================================================
FUNCTION QCamDisp_DrawDirSelect

  DESCRIPTION
    This function draws the dir selection rectange.

  PARAMETERS:
    pIDisp - pointer to IDisplay interface.
    oldRC - the current rect (will be erased)
    newRC - the new rect will be draw

  RETURN VALUE:
    NONE
===========================================================================*/
void QCamDisp_DrawSelect(IDisplay *pIDisp, AEERect *oldRC, AEERect newRC)
{
  if (!pIDisp || !oldRC)
    return;

  //get the display rect for selection rectange
  newRC.dx += QCAM_SEL_OFFSET*2;
  newRC.dy += QCAM_SEL_OFFSET*2;

  if (oldRC->dx != 0)
  {
     // erase the previous line
     IDISPLAY_DrawRect(pIDisp, oldRC, RGB_WHITE, 0, IDF_RECT_FRAME);
  }
  // Draw the selection rectange
  IDISPLAY_DrawRect(pIDisp, &newRC, RGB_BLACK, 0, IDF_RECT_FRAME);
  (void)MEMCPY(oldRC, &newRC, sizeof(AEERect));

  IDISPLAY_Update(pIDisp);
}

/*===========================================================================
FUNCTION QCamDisp_DrawYCbCrImage

  DESCRIPTION
    This function draws the selected image in YCbCr format.
    It will resize the image to fit the display area if image too big.

  PARAMETERS:
    pIDisp  - pointer to IDisplay interface
    pIYCbCrFactory - IYCBCRFactory interface pointer
    pIIpl   - IIPL interface pointer
    pYC     - IYCbCr pointer to the display image
    rc      - AEERect pointer to the display rect
    xSrc    - Source image x location
    ySrc    - Source image y location
    bResize - Flag for resize

  RETURN VALUE:
    TRUE if image is draw, FALSE otherwise.
===========================================================================*/
int QCamDisp_DrawYCbCrImage(IDisplay *pIDisp, IYCBCRFactory *pIYCBCRFactory,
                                IIpl *pIIpl, IYCbCr *pYC, const AEERect *rc,\
                                int xSrc, int ySrc, boolean bResize)
{
  IYCbCr  *sizedYC   = NULL;
  IYCbCr  *dispYC    = pYC;
  AEERect imageRC;
  int retVal = AEE_SUCCESS;

  if (!pIDisp || !pIYCBCRFactory || !pIIpl || !pYC || !rc)
    return EBADPARM;

  // If either the width or height of the image is greater than the
  // destination rectange, resize the image to fit
  if (((int16)pYC->cx > rc->dx || (int16)pYC->cy > rc->dy) && bResize)
  {
    //Get downsized dx and dy
    imageRC = QCamDisp_GetResizedRect(*rc, (int16)pYC->cx, (int16)pYC->cy);

    //lint -save -e571 Suspicious cast here
    retVal = IYCBCRFactory_Create(pIYCBCRFactory, (uint32)pYC->uScheme,
                                  (uint32)imageRC.dx, (uint32)imageRC.dy, &sizedYC);
    if (retVal != AEE_SUCCESS)
    {
      CAM_RELEASEIF(sizedYC); //reset sizeYC to NULL.
      return retVal;
    }
    //lint -restore
    if (sizedYC)
    {
      retVal = IIPL_ChangeSize(pIIpl, (IBitmap *)pYC, (IBitmap *)sizedYC) ;
      if (retVal != AEE_SUCCESS)
      {
        //MSG_HIGH("IIPL_ChangeSize faied", 0, 0, 0);
        CAM_RELEASEIF(sizedYC); //reset sizeYC to NULL.
        return retVal;
      }
    }
    dispYC = sizedYC;
  }
  else
  {
    //move rect to display image in the center
    imageRC = QCamDisp_GetCenterRect(*rc, (int16)pYC->cx, (int16)pYC->cy);
  }

  if (dispYC)
  {
    //clear the display area
    IDISPLAY_EraseRect(pIDisp, rc);

    IDISPLAY_BitBlt(pIDisp, imageRC.x, imageRC.y, (int)dispYC->cx, (int)dispYC->cy,
                    (const void *)dispYC, xSrc, ySrc, AEE_RO_COPY);

    IDISPLAY_UpdateEx(pIDisp, FALSE);
  }

  CAM_RELEASEIF(sizedYC);
  return retVal;
} /* END QCam_DrawYCbCrImage */

/*===========================================================================
FUNCTION QCamDisp_DrawImage

  DESCRIPTION
    This function draws the selected image.
    It will resize the dib to fit the display area if image too big.

  PARAMETERS:
    pIDisp  - pointer to IDisplay interface
    pIIpl   - IIPL interface pointer
    dib     - IDIB pointer to the display image
    rc      - AEERect pointer to the display rect
    xSrc    - Source image x location
    ySrc    - Source image y location
    bResize - Flag for resize


  RETURN VALUE:
    TRUE if image is draw, FALSE otherwise.
===========================================================================*/
int QCamDisp_DrawImage(IDisplay *pIDisp, IIpl *pIIpl, IDIB *dib, const AEERect *rc,
                           int xSrc, int ySrc, boolean bResize)
{
  IDIB    *sizedDIB  = NULL;
  IDIB    *dispDIB   = dib;
  AEERect imageRC;
  int retVal = AEE_SUCCESS;

  if (!pIDisp || !pIIpl || !dib || !rc)
    return EBADPARM;

  // If either the width or height of the image is greater than the
  // destination rectange, resize the image to fit
  if ((dib->cx > rc->dx || dib->cy > rc->dy) && bResize)
  {
    //find the closest image size that maintains the aspect ratio
    imageRC = QCamDisp_GetResizedRect(*rc, (int16)dib->cx, (int16)dib->cy);
    retVal = IDISPLAY_CreateDIBitmap(pIDisp, &sizedDIB, dib->nDepth,
                                     (uint16)imageRC.dx, (uint16)imageRC.dy);
    if (retVal != AEE_SUCCESS)
    {
      CAM_RELEASEIF(sizedDIB); //reset sizeDIB to NULL.
      return retVal;
    }

    if (sizedDIB)
    {
      //ColorScheme is not set by createDIBitmap, if not set Blt will not draw
      sizedDIB->nColorScheme = dib->nColorScheme;
      retVal = IIPL_ChangeSize(pIIpl, (IBitmap *)dib, (IBitmap *)sizedDIB);
      if (retVal != AEE_SUCCESS)
      {
        // MSG_HIGH("IIPL_ChangeSize faied", 0, 0, 0);
        CAM_RELEASEIF(sizedDIB); //reset sizeDIB to NULL.
        return retVal;
      }
    }
    dispDIB = sizedDIB;
  }
  else //move rect to display image in the center
    imageRC = QCamDisp_GetCenterRect(*rc, (int16)dib->cx, (int16)dib->cy);


  if (dispDIB)
  {
    //clear the display area
    IDISPLAY_EraseRect(pIDisp, rc);

    IDISPLAY_BitBlt(pIDisp, imageRC.x, imageRC.y, dispDIB->cx, dispDIB->cy,
                    (const void *)dispDIB, xSrc, ySrc, AEE_RO_COPY);

    IDISPLAY_UpdateEx(pIDisp, FALSE);
  }

  CAM_RELEASEIF(sizedDIB);
  return retVal;
} /* END QCam_DrawImage */

/*===========================================================================
FUNCTION QCamDisp_DrawMenuTitle

  DESCRIPTION
    This function draws the menu title.

  PARAMETERS:
    pIDisp - pointer to IDisplay interface
    rc - AEERect pointer to the display rect
    pImage - image pointer to the title icon
    txtStr - text buffer for title text string

  RETURN VALUE:
    TRUE if image is draw, FALSE otherwise.
===========================================================================*/
void QCamDisp_DrawMenuTitle(IDisplay *pIDisp, AEERect rc, IImage *pImage, const AECHAR *txtStr)
{
  AEEImageInfo info = {0};

  if (!pIDisp)
    return;

  if (pImage)
  {
    IIMAGE_GetInfo(pImage, &info);

    IIMAGE_Draw(pImage, rc.x, 0);
    (void)IIMAGE_Release(pImage);
    pImage = NULL;

    // move text location if image present
    rc.x += (int16)info.cx + 2;
  }

  if (txtStr)
  {
    (void)IDISPLAY_DrawText(pIDisp, QCAM_FONT_TITLE, txtStr, -1, 0, 1, &rc, IDF_ALIGN_LEFT);
  }
}
/* ==========================================================================
FUNCTION QCAM_DRAWICON
DESCRIPTION
============================================================================= */
void QCamDisp_DrawIcon(IDisplay *pIDisp, IImage *pIcon, AEERect rc)
{
  AEEImageInfo info = {0};

  if (!pIDisp)
    return;

  if (pIcon)
  {
    IDISPLAY_EraseRect(pIDisp, &rc);

    IIMAGE_GetInfo(pIcon, &info);

    IIMAGE_Draw(pIcon, rc.x + (rc.dx - info.cx)/2, rc.y + (rc.dy - info.cy)/2);
    (void)IIMAGE_Release(pIcon);
    pIcon = NULL;
    IDISPLAY_UpdateEx(pIDisp, FALSE);
  }
}

#ifdef FEATURE_OVERLAY
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
/*=====================================================================
FUNCTION QCamDisp_ChangeDisplay

  DESCRIPTION: This function change the DISPLAY3 linkage to provided state

  PARAMETERS:
    pIShell - Pointer to IShell interface
    state   - linkage state: 0 disconect, 1 connect

  RETURN VALUE:
    error code defined in AEEError.h
======================================================================*/
int QCamDisp_ChangeDisplay(IShell *pIShell, AEECLSID ClsId, int state)
{
  ILogicalDisplayCfg *pLogicalDispCfg;
  physicalDisplayStateType dispState;
  int status;

  if (!pIShell)
    return EFAILED;

  dispState.physicalDisplay = AEECLSID_PHYSICALDISPLAY3;
  dispState.state = state;

  status = ISHELL_CreateInstance(pIShell, AEECLSID_LOGICALDISPLAYCFG,
                                 (void **) &pLogicalDispCfg);
  if (status == SUCCESS)
  {
    status = ILOGICALDISPLAYCFG_ChangeDisplayLinkage(pLogicalDispCfg,
                                                     ClsId,
                                                     1,
                                                     &dispState);
  }

  CAM_RELEASEIF(pLogicalDispCfg);
  return status;
}

/*=====================================================================
FUNCTION QCamDisp_GetOldDispClsId

  DESCRIPTION: This function returns the current display class id for
               PHYSICALDISPLAY3.

  PARAMETERS:
    pIShell - Pointer to IShell interface

  RETURN VALUE:
    AEECLSID
======================================================================*/
AEECLSID QCamDisp_GetOldDispClsId(IShell *pIShell)
{
  ILogicalDisplayCfg *pLogicalDispCfg;

  if (!pIShell)
    return 0;

  if (ISHELL_CreateInstance(pIShell, AEECLSID_LOGICALDISPLAYCFG,
                            (void **) &pLogicalDispCfg) == SUCCESS)
  {
    return ILOGICALDISPLAYCFG_GetMappedLogicalDisplay(pLogicalDispCfg,
                                                      AEECLSID_PHYSICALDISPLAY3);
  }
  return 0;
}


/*=====================================================================
FUNCTION QCamDisp_DisplayOvOnTV

  DESCRIPTION: This function create or set the IOverlay object from a bitmap and
               display it on the TV (AEECLSID_DISPLAY3)

  PARAMETERS:
    pIShell   - Pointer to IShell interface
    ppMMOverlay  - Pointer to IOverlay object
    pBitmap   - Pointer to the bitmap

  RETURN VALUE:
    void
======================================================================*/
void QCamDisp_DisplayOvOnTV(IShell *pIShell, IMMOverlay **ppMMOverlay, IBitmap *pBitmap)
{
  AEERect rc = {0, 0, 0, 0};

  if (!pBitmap || !ppMMOverlay)
    return;

  if (!*ppMMOverlay)
  {
    //Create an overlay
    IMMOverlayMgr *pIMMOverlayMgr;
    AEEPoint destPoint;

    if (!pIShell)
    {
      MSG_HIGH( "Failed QCamDisp_DisplayOvOnTV, pIShell is NULL", 0, 0, 0);
      return;
    }


    if (ISHELL_CreateInstance(pIShell,
                              AEECLSID_MMOVERLAYMGR,
                              (void **)&pIMMOverlayMgr) != SUCCESS)
    {
      MSG_HIGH("Failed to create pIOverlayMgr in QCamDisp_DisplayOvOnTV", 0, 0, 0);
      return;
    }
    else
    {
      destPoint.x = 0;
      destPoint.y = 0;
      //Create overlay object in upper left corner of screen
      if(SUCCESS != IMMOVERLAYMGR_CreateOverlay(pIMMOverlayMgr,
                                           pBitmap,
                                           AEECLSID_DISPLAY3,
                                           destPoint,
                                           ppMMOverlay))
      {
        MSG_HIGH( "Failed QCamDisp_DisplayOvOnTV, pOverlay is NULL", 0, 0, 0);
        return;
      }
      MSG_MED("IMMOVERLAY_SetParam to set Rotate.", 0, 0, 0);
      IMMOVERLAY_SetParam(*ppMMOverlay, OV_PARM_ROTATE, 270, 0);

      MSG_MED("IMMOVERLAY_SetParam to set destination region.", 0, 0, 0);
      IMMOVERLAY_SetParam(*ppMMOverlay, OV_PARM_DEST_REGION, (uint32)&rc,
                          OV_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER |
                          OV_FLAG_DEST_REGION_ALIGN_VERT_CENTER);
    }
  }
  else
  {
    //Update the bitmap parameter
    MSG_MED("IMMOVERLAY_SetParam to set pOverlay.", 0, 0, 0);
    IMMOVERLAY_SetParam(*ppMMOverlay, OV_PARM_BITMAP, (uint32)pBitmap, 0);
  }

  //Display the IOverlay object on TV
  IMMOVERLAY_Update(*ppMMOverlay, &rc);
}
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef FEATURE_MMOVERLAY
/* ==========================================================================
IOverlay for 7K targets
============================================================================= */

/*===========================================================================
FUNCTION: QCamDisp_RegisterOv

  DESCRIPTION:
    This function register the specified foreground area for MDP overlay.
  
  PARAMETERS:
    pIMMOverlayMgr - Pointer to IMMOverlayMgr interface
    pIMMOverlay    - Pointer to the IMMOverlay pointer that will be created
    ov             - Overlay data structure 

  RETURN VALUE:
    TRUE if success. Otherwise FALSE.
===========================================================================*/
boolean QCamDisp_RegisterOv(IMMOverlayMgr *pIMMOverlayMgr, 
                            IMMOverlay **ppIMMOverlay, OvData ov)
{
  AEEPoint pt;

  if (!pIMMOverlayMgr)
    return FALSE;

  if (!ppIMMOverlay)
    return FALSE;

  pt.x = ov.rc.x;
  pt.y = ov.rc.y;

  //Create overlay object in AEECLSID_DISPLAY1 with provided ov data
  if(SUCCESS == IMMOVERLAYMGR_CreateOverlay(pIMMOverlayMgr, ov.pBmp, AEECLSID_DISPLAY1, pt, ppIMMOverlay))
  {
    (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_LAYER, (uint32)ov.layer, 0);
    (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_DEST_REGION, (uint32)&ov.rc, 0);
    (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_SRC_REGION, (uint32)&ov.rc, 0);
    if (ov.transparent == RGB_NONE) //turn off transparent
      (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_TRANSPARENCY, 0, 0);
    else
      (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_TRANSPARENCY, 1, ov.transparent);
    (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_ALPHA_BLEND_PCT, ov.alpha, 0);
    (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_SYSTEM_OVERLAY, 1, 0);
  }
  else
  {
    MSG_HIGH( "Failed QCamDisp_SetOverlay", 0, 0, 0);
    return FALSE;
  }

  return TRUE;
}

#else //FEATURE_MMOVERLAY
/* ==========================================================================
IOverlay for 6K targets
============================================================================= */
/*===========================================================================
FUNCTION: QCamDisp_InitMdp

  DESCRIPTION:
    This function register the device bitmap for mdp display.

  PARAMETERS:
    pIDisp - Pointer to the IDisplay interface.

  RETURN VALUE:
    NONE
===========================================================================*/
void QCamDisp_InitMdp(IDisplay *pIDisp)
{
  IDIB *pDib;
  IBitmap *bmp;

  if (!pIDisp)
    return;

  if (IDISPLAY_GetDeviceBitmap (pIDisp, &bmp) ==  SUCCESS)
  {
    (void)IBITMAP_QueryInterface (bmp, AEECLSID_DIB, (void **) &pDib);
    CAM_RELEASEIF(bmp);
    if (pDib)
    {
      //lint -save -e826 Suspicious pointer-to-pointer conversion (area too small)
      mdp_set_sys_bmp((uint32 *)(pDib->pBmp));
      //lint -restore
      CAM_RELEASEIF(pDib);
    }
  }
}

/*===========================================================================
FUNCTION: QCamDisp_MdpDeregister

  DESCRIPTION:
    This function deregister the MDP foregroung display.

  PARAMETERS:
    pIOverlay Interface pointer to IOverlay

  RETURN VALUE:
    None
===========================================================================*/
void QCamDisp_MdpDeregister(IOverlay *pIOverlay)
{
  if (!pIOverlay)
    return;

  //OVERLAY_LAYER_2 is for UI overlay, clean up first.
  IOVERLAY_RemoveAll(pIOverlay, OVERLAY_LAYER_2);
  mdp_set_sys_bmp(NULL);
}

/*===========================================================================
FUNCTION: QCamDisp_MdpRegister

  DESCRIPTION:
    This function register the specified foreground area for MDP overlay.

  PARAMETERS:
    pIDisp - Pointer to the IDisplay interface.
    pIOverlay  - Interface pointer to IOverlay
    rc - area to register

  RETURN VALUE:
    pointer to the registed mdp
===========================================================================*/
OVERLAY_ID QCamDisp_MdpRegister(IDisplay *pIDisp, IOverlay *pIOverlay, AEERect rc)
{
  IDIB *pDib;
  IBitmap *bmp;
  OverlayOpt  ovOpts[3];
  OVERLAY_ID id;

  if (!pIOverlay || !pIDisp)
    return 0;

  ovOpts[0].nId = OVERLAYOPT_ALPHA;
  ovOpts[0].val = 50;  // 50% alpha value

  ovOpts[1].nId = OVERLAYOPT_TRANSPARENCY;
  ovOpts[1].val = 0xFFFF;  //transparent value

  ovOpts[2].nId = OVERLAYOPT_END; //id to end the array
  ovOpts[2].val = 0;

  if (IDISPLAY_GetDeviceBitmap (pIDisp, &bmp) != SUCCESS)
    return 0;

  (void)IBITMAP_QueryInterface (bmp, AEECLSID_DIB, (void **) &pDib);
  CAM_RELEASEIF(bmp);

  if (!pDib)
    return 0;

  id = IOVERLAY_Add(pIOverlay, (IBitmap *)pDib, &rc, &rc, OVERLAY_LAYER_2, ovOpts);
  CAM_RELEASEIF(pDib);

  return id;
}

/*===========================================================================
FUNCTION: QCamDisp_MdpRegisterFullScreen

  DESCRIPTION:
    This function register the full screen area for mdp overlay
    on display image or setting menus.

  PARAMETERS:
    pIDisp - Pointer to the IDisplay interface.
    pIOverlay  - Interface pointer to IOverlay
    dx - screen width
    dy - screen heigh

  RETURN VALUE:
      None
===========================================================================*/
void QCamDisp_MdpRegisterFullScreen(IDisplay *pIDisp, IOverlay *pIOverlay, uint16 dx, uint16 dy)
{
  AEERect rc;

  if (!pIOverlay || !pIDisp)
    return;

  //Clear the former register first.
  IOVERLAY_RemoveAll(pIOverlay, OVERLAY_LAYER_2);

  //Register full screen for mdp display
  SETAEERECT (&rc, 0, 0, dx, dy);
  (void) QCamDisp_MdpRegister(pIDisp, pIOverlay, rc);

}
#endif // #ifdef FEATURE_MMOVERLAY
#endif // FEATURE_OVERLAY

/***************************************************************************
Following functions are temporary soulutions. It should be replaced or redesigned
once we get BREW interface support.

****************************************************************************/
/*===========================================================================
FUNCTION QCam_GetPNGImageData
===========================================================================*/
IDIB *QCam_GetPNGImageData(IDisplay *pIDisp, IImage *pImage)
{
  AEEImageInfo imgInfo = {0};
  IDIB *srcDIB = NULL;
  IBitmap *pngBmp1 = NULL;
  IBitmap *pngBmp2 = NULL;

  if (!pIDisp || !pImage)
    return NULL;

  IIMAGE_GetInfo(pImage,&imgInfo);
  if (IDISPLAY_GetDeviceBitmap(pIDisp, &pngBmp1) != SUCCESS)
    return NULL;

  if (pngBmp1)
  {
    (void)IBITMAP_CreateCompatibleBitmap(pngBmp1,&pngBmp2,imgInfo.cx, imgInfo.cy);
    CAM_RELEASEIF(pngBmp1);
  }
  if (pngBmp2)
  {
    if (IDISPLAY_SetDestination(pIDisp,pngBmp2) != SUCCESS)
    {
      CAM_RELEASEIF(pngBmp2);
      return NULL;
    }
    IDISPLAY_ClearScreen(pIDisp);
    IIMAGE_Draw(pImage,0,0);
    IDISPLAY_UpdateEx(pIDisp, FALSE);
    (void)IBITMAP_QueryInterface(pngBmp2, AEECLSID_DIB, (void **)&srcDIB);
    CAM_RELEASEIF(pngBmp2);
  }
  (void)IDISPLAY_SetDestination(pIDisp,NULL);
  IDISPLAY_UpdateEx(pIDisp, FALSE);
  return srcDIB;
}

/*===========================================================================
FUNCTION QCam_LoadPNGFrame
===========================================================================*/
boolean QCam_LoadPNGFrame(IShell *pIShell, IFileMgr *pIFileMgr, const char *pFile, IDIB **pDib)
{
  IImageDecoder *pPNGDecoder = NULL;
  IForceFeed *pForceFeed = NULL;
  IFile *pSrc = NULL;
  IBitmap *pIBitmap = NULL;
  FileInfo pInfo;
  byte *pBuf = NULL;
  uint32 dwCount;
  boolean ret = FALSE;

  if (!pIShell || !pIFileMgr || !pFile || !pDib)
    return FALSE;

  if (ISHELL_CreateInstance(pIShell, AEECLSID_PNGDECODER,
                           (void **)&pPNGDecoder) != SUCCESS)
    return FALSE;

  if (IIMAGEDECODER_QueryInterface(pPNGDecoder, AEEIID_FORCEFEED,
                                   (void**)&pForceFeed) != SUCCESS)
    goto CleanExit;

  if (IFILEMGR_GetInfo(pIFileMgr, pFile, &pInfo) != SUCCESS)
    goto CleanExit;

  pSrc = IFILEMGR_OpenFile(pIFileMgr, pFile, _OFM_READ);
  if (pSrc && pInfo.dwSize > 0)
  {
    dwCount = pInfo.dwSize;
    pBuf = (byte *)MALLOC(dwCount);
    if (pBuf)
    {
      if (IFILE_Read(pSrc, pBuf, dwCount) != (int32)dwCount)
        goto CleanExit;

      IFORCEFEED_Reset(pForceFeed);
      if (IFORCEFEED_Write(pForceFeed, pBuf, (int)dwCount) == SUCCESS)
      {
        (void)IIMAGEDECODER_GetBitmap(pPNGDecoder, &pIBitmap);
        if (pIBitmap)
        {
          if (IBITMAP_QueryInterface(pIBitmap, AEECLSID_DIB, (void**)pDib) == SUCCESS)
          {
            if (IIMAGEDECODER_GetRop(pPNGDecoder) != AEE_RO_TRANSPARENT)
              (*pDib)->ncTransparent = DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB888;
            ret = TRUE;
          }
        }
      }
    }
  }

CleanExit:
  CAM_FREEIF(pBuf);
  CAM_RELEASEIF(pIBitmap);
  CAM_RELEASEIF(pForceFeed);
  CAM_RELEASEIF(pPNGDecoder);
  CAM_RELEASEIF(pSrc);

  return ret;
}

/*===========================================================================
FUNCTION QCam_Get565TransparentVal
===========================================================================*/
void QCam_Get565TransparentVal(uint32 oldVal, uint16 *newVal)
{
  if (!newVal)
    return;

  if (oldVal == DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB888)
    *newVal = DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB565;
  else
  {
    // Explicity convert the QCam structure's transparent value
    // from RGB888 to RGB565.  Read each byte to ensure correct conversion
    *newVal = (uint16)((oldVal & 0x1F) |
              ((oldVal & 0x3F00) >> 3) |
              ((oldVal & 0x1F0000) >> 5));
  }
}

/* ==========================================================================
FUNCTION QCAM_PNG242DIB24
DESCRIPTION
============================================================================= */
void QCam_PNG242DIB24(byte *dibBuf, const byte *pngBuf, uint32 width, uint32 height)
{
  uint32 x;
  uint32 y;
  uint32 pos = 0;

  if (!dibBuf || !pngBuf)
   return;

  for (y = 0; y < height; y++)
  {
    for (x = 0; x < width; x++)
    {
       dibBuf[pos + 0] = pngBuf[pos + 2];
       dibBuf[pos + 1] = pngBuf[pos + 1];
       dibBuf[pos + 2] = pngBuf[pos + 0];
       pos += 3;
    }
  }
} /* END QCam_PNG242DIB24 */

/* ==========================================================================
FUNCTION QCAM_PNG2562DIB24
DESCRIPTION
============================================================================= */
void QCam_PNG2562DIB24(byte *dibBuf, const byte *pngBuf, const byte *palette, uint32 width, uint32 height)
{
  uint32 x;
  uint32 y;
  uint32 dstPos = 0;
  uint32 srcPos = 0;
  int16  index;

  if (!dibBuf || !pngBuf || !palette)
    return;

  for (y = 0; y < height; y++)
  {
    for (x = 0; x < width; x++)
    {
      index = (int16)(pngBuf[srcPos] * 3);
      dibBuf[dstPos + 0] = palette[index + 2];
      dibBuf[dstPos + 1] = palette[index + 1];
      dibBuf[dstPos + 2] = palette[index + 0];
      dstPos += 3;
      srcPos++;
    }
  }
} /* END QCam_PNG2562DIB24 */

#endif //#ifdef FEATURE_APP_QCAMERA


/*===========================================================================

FILE: qalbumAdv.c

GENERAL DESCRIPTION
    This file contains the advanced editing features of Qalbum.

      Copyright (c) 2004-2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qalbum/main/latest/src/qalbumAdv.c#1 $ $DateTime: 2008/05/12 10:09:09 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 06/29/07  NC     Removed up arrow in some edit features when reaching to max step.
 06/18/07  RK     Fixed the  BCDB 0 while doing Advanced Album Edit-Neon effect repeatedly.
 06/08/07  NC     Called Qalbum_ReturnView to avoid redraw image when exit to View.
 04/18/07  NC     Enable IIPL_DrawRgbHistogram test in simulator.
 04/12/07  NC     Fixed error exit bugs.
 01/24/07  ZL     Fixed automation failures caused by the addition of VIEW state.
 12/21/06  RK     Remove the Filter for RGB Histogram feature in 6K targets.
 11/28/06  NC     Filter out RGB Histogram feature from 6K targets.
 11/17/06  TV     Added check for small screen to figure out which DisplayTitle method to call
 11/09/06  ZL     Changes caused by adding VIEW state before EDIT state.
 11/08/06  ZL     Changes caused by moving editing feature to qalbumEdit.c.
 10/30/06  ZL     Adjusted some positions of menu titles.
 10/27/06  NC     Fixed compiler warning.
 10/09/06  NC     Added OnRGBHistogram and simplify compose edit features.
 09/21/06  NC     Optimized automation status report logic
 09/18/06  NC     Fixed automation issue in selecting 2nd image and simplified code.
 08/18/06  RK     Updated the Error handling through a Advanced helper function.
 05/17/06  NC     Enabled to cancel message timer on key press.
 04/20/06  HV     Updated Qalbum_AdvEditKeyHandler() to handle dynamic menu
 11/23/05  JN     Support for luma histogram.
 11/08/05  JN     Support for whiteboard effect.
 11/03/05  NC     Updated some Advanced edit features and added more error reports
                  for automation test.
 10/24/05  JN     2nd round of lint clean up.
 09/20/05  JN     Lint clean up.
 08/17/05  JN     Redesigned Qalbum to decouple it from AMSS.
 06/20/05  NC     Added advanced edit automation feature.
 05/26/05  NC     Make sure we get valid index before calling mapping function.
 05/13/05  JN     Update EditIn image buffer at the beginning of a edit option.
 02/14/05  JN     Cleaned up some lint errors.
 01/28/05  NC     Removed Qalbum_OnAdvanced()
 01/17/05  JN     Horizontal, vertical and center comp starts from min boundary,
                  hence only UP key is allowed.
 12/21/04  JN     Add advanced edit option into qalbum.
============================================================================*/

//lint -e740 suppress errors on pointer cast
//lint -e611

/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#ifndef AEE_SIMULATOR
#include "msg.h"
#endif

#if defined(FEATURE_APP_QALBUM)
#include "qalbumAdv_i.h"

#define WHITE_RGB565      0xFFFF
//#define WHITE_YCBCR       0x80EB

/*===========================================================================
FUNCTION Qalbum_DisplayAdvEditMenuName

  DESCRIPTION
    This function displays the selected menu item name of the advanced edit menu.

  PARAMETERS:
    pMe  - Pointer to  Qalbum struct
    item - move direction on current item (-1, 0, 1).

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_DisplayAdvEditMenuName(Qalbum* pMe, int item)
{
  uint16 selection;
  int index;
  int count;

  uint16 image1ID    = 0;
  uint16 image1StrID = 0;
  uint16 image2ID    = 0;
  uint16 image2StrID = 0;

  if (!pMe)
    return;

  //get old item index
  if (!IMENUCTL_GetItemData(pMe->m_pISoftMenu, pMe->m_wEditMenuSel, (uint32 *)&index))
    return;

  //get total items
  count = IMENUCTL_GetItemCount(pMe->m_pISoftMenu);

  if (count == 0)
    return;

  //get new item index and wrapped if needed
  index += item;
  index = (count + index) % count;

  //get new item id
  selection = IMENUCTL_GetItemID(pMe->m_pISoftMenu, index);
  //remind the new item id
  pMe->m_wEditMenuSel = selection;

  //display the title of the new item
  switch (selection)
  {
  case IDS_MAGNIFY:
  case IDS_OVERLAP:
    if (pMe->m_bInEdit)
    {
      image1ID    = IDB_MOVE;
      image1StrID = IDS_MOVE;
      image2ID    = IDB_DONE;
      image2StrID = IDS_DONE;
      if (pMe->m_bSmallScreen)
      {
        // We don't have room for everything to display, but we need to have two
        // text strings (so DisplayTitle_SM will not work)
        // So just drop the display of the "selection" text
        Qalbum_DisplayTitle(pMe, image1ID, image1StrID, image2ID, image2StrID, 0, 0);
      }
      else
      {
        Qalbum_DisplayTitle(pMe, 0, selection, image1ID, image1StrID, image2ID, image2StrID);
      }
    }
    else
    {
      if (pMe->m_bEditProcessed && pMe->m_wEditProcessedBy == selection)
      {
        image2ID    = IDB_U;
        image2StrID = IDS_UNDO;
      }
      if (pMe->m_bSmallScreen)
      {
        // We don't have room for everything to display, but we need to have two
        // text strings (so DisplayTitle_SM will not work)
        Qalbum_DisplayListNavArrows(pMe);
        Qalbum_DisplayTitle(pMe, IDB_SELECT, selection, image2ID, image2StrID, 0, 0);
      }
      else
      {
        Qalbum_DisplayListNavArrows(pMe);
        Qalbum_DisplayTitle(pMe, 0, 0, IDB_SELECT, selection, image2ID, image2StrID);
      }
    }
    break;

  case IDS_NEON:
  case IDS_HISTOGRAM:
  case IDS_RGB_HISTOGRAM:
    //If image has been edited, add undo option.
    if (pMe->m_bEditProcessed && pMe->m_wEditProcessedBy == selection)
    {
      image1ID = 0;
      image2ID = IDB_DOWN;
      image2StrID = IDS_UNDO;
    }
    else
    {
      image1ID = IDB_UP;
    }
    if (pMe->m_bSmallScreen)
    {
      Qalbum_DisplayTitle_SM(pMe, image1ID, image2ID, selection);
    }
    else
    {
      Qalbum_DisplayTitle(pMe, 0, 0, image1ID, selection, image2ID, image2StrID);
    }
    break;

  case IDS_PERSPECTIVE:
    image1ID = IDB_UP;
    image2ID = IDB_DOWN;
    image1StrID = IDS_ORIENTATION;
    image2StrID = IDS_UNDO;

    if (pMe->m_wEditProcessedBy != selection)
    {
      pMe->m_nEditProcessedVal = Qalbum_GetStartValue(selection);
    }
    if (pMe->m_bSmallScreen)
    {
      Qalbum_DisplayTitle_SM(pMe, image1ID, image2ID, selection);
    }
    else
    {
      Qalbum_DisplayTitle(pMe, 0, selection, image1ID, image1StrID, image2ID, image2StrID);
    }
    break;

  case IDS_FOG:
  case IDS_GAUSSIAN_BLUR:
  case IDS_UNIFORM_BLUR:
  case IDS_MOTION_BLUR:
  case IDS_ALPHA_BLEND:
  case IDS_HORIZONTAL_COMP:
  case IDS_VERTICAL_COMP:
  case IDS_CENTER_COMP:
  case IDS_WHITEBOARD:
    image1ID = IDB_UP;
    image2ID = IDB_DOWN;
    image1StrID = IDS_INCREASE;
    image2StrID = IDS_DECREASE;

    if (pMe->m_wEditProcessedBy != selection)
    {
      pMe->m_nEditProcessedVal = Qalbum_GetStartValue(selection);
    }

    //If it is max value, cannot increase more, hide increase option
    if (Qalbum_IsMaxValue(pMe->m_nEditProcessedVal, selection))
    {
      image1StrID = 0;
      image1ID    = 0;
    }
    //If it is min value, cannot decrease more, hide decrease option
    else if (Qalbum_IsMinValue(pMe->m_nEditProcessedVal, selection))
    {
      image2StrID = 0;
      image2ID    = 0;
    }

    if (pMe->m_bSmallScreen)
    {
      Qalbum_DisplayTitle_SM(pMe, image1ID, image2ID, selection);
    }
    else
    {
      Qalbum_DisplayTitle(pMe, 0, selection, image1ID, image1StrID, image2ID, image2StrID);
    }
    break;

  case IDS_ARB_ROTATION:
    image1ID = IDB_UP;
    image2ID = IDB_DOWN;
    image1StrID = IDS_CLOCKWISE;
    image2StrID = IDS_COUNTER_CLOCKWISE;

    if (pMe->m_wEditProcessedBy != selection)
    {
      pMe->m_nEditProcessedVal = Qalbum_GetStartValue(selection);
    }

    //If it is max value, cannot increase more, hide increase option
    if (Qalbum_IsMaxValue(pMe->m_nEditProcessedVal, selection))
    {
      image1StrID = 0;
      image1ID    = 0;
    }
    //If it is min value, cannot decrease more, hide decrease option
    else if (Qalbum_IsMinValue(pMe->m_nEditProcessedVal, selection))
    {
      image2StrID = 0;
      image2ID    = 0;
    }

    if (pMe->m_bSmallScreen)
    {
      Qalbum_DisplayTitle_SM(pMe, image1ID, image2ID, selection);
    }
    else
    {
      Qalbum_DisplayTitle(pMe, 0, selection, image1ID, image1StrID, image2ID, image2StrID);
    }
    break;

  default:
    break;
  }
}

/*===========================================================================
FUNCTION Qalbum_AdvEditKeyHandler

  DESCRIPTION
    Key handler for Qalbum in edit state

  PARAMETERS:
    pMe - Pointer to the Qalbum struct
    eCode - AEEEvent id
    key - key id

  RETURN VALUE:
   None
===========================================================================*/
boolean Qalbum_AdvEditKeyHandler(Qalbum *pMe, AEEEvent eCode, uint16 key)
{
  uint16 sel;
  uint32 index = 0;
  boolean update_display = FALSE;

  if (!pMe)
    return FALSE;

  //Message for error, return to edit screen
  if (pMe->m_bMsgTimerOn)
  {
    Qalbum_EditReturn(pMe);
    return TRUE;
  }

  sel = IMENUCTL_GetSel(pMe->m_pISoftMenu);
  //If sel is invalid for some reason, it should fail to get index.
  //ignor the key event in this case.
  if (!IMENUCTL_GetItemData(pMe->m_pISoftMenu, sel, &index))
  {
    MSG_HIGH("Invaild menu id %d", sel, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    return TRUE;
  }

  switch (key)
  {
  case AVK_RIGHT:
  case AVK_LEFT:
    if (!pMe->m_bInEdit)
    {
      //Navigate through menu icons
      Qalbum_DisplayAdvEditMenuName(pMe, (key == (uint16)AVK_LEFT ? -1 : 1) );
      (void)IMENUCTL_HandleEvent(pMe->m_pISoftMenu, eCode, key, 0);
    }
    else
    {
      if (((sel == IDS_MAGNIFY) || (sel == IDS_OVERLAP)) &&
          (QalbumAdvEditHandlers[index].wMenuID == sel))
      {
        update_display = QalbumAdvEditHandlers[index].MenuOnKeyFunc(pMe, key);
      }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    break;

  case AVK_UP:
  case AVK_DOWN:
  case AVK_SELECT:
  case AVK_8:
  case AVK_POUND:
    //Execute menu on focus
    if (QalbumAdvEditHandlers[index].wMenuID == sel)
    {
      update_display = QalbumAdvEditHandlers[index].MenuOnKeyFunc(pMe, key);
    }
    else
    {
      //incase the item is filtered out dyna
      for (index = 0; index < ARR_SIZE(QalbumAdvEditHandlers); index++)
      {
        if (QalbumAdvEditHandlers[index].wMenuID == sel)
        {
          update_display = QalbumAdvEditHandlers[index].MenuOnKeyFunc(pMe, key);
          break;
        }
      }
    }
    break;

  case AVK_CLR:
    if (pMe->m_bInEdit)
    {
      pMe->m_bInEdit = FALSE; // Clear the change
      update_display = TRUE;
      Qalbum_EditUndo(pMe);
      Qalbum_DisplayAdvEditMenuName(pMe, 0);
    }
    else
    {
      CAM_RELEASEIF(pMe->m_pEditIn2);
      Qalbum_ReturnView(pMe, IDS_ADVANCED);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
    }
    break;

  default:
    break;
  }

  if (update_display)
  {
    Qalbum_UpdateEditedImage(pMe);
  }
  return TRUE;
}


/*=====================================================================
FUNCTION Qalbum_DoMagnify
  DESCRIPTION:
    This is the helper function for Magnify feature.

  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
    TRUE if IIPL function success.
    FALSE otherwise.
======================================================================*/
static boolean Qalbum_DoMagnify(Qalbum *pMe)
{
  int retVal = AEE_SUCCESS;
  if (!pMe)
    return FALSE;

  retVal = IIPL_Magnify(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, &pMe->m_iCircle) ;
  // Checking the Error After editing the IIPL
  if (Qalbum_DisplayError(pMe, retVal) != AEE_SUCCESS)
    return FALSE;

  return TRUE;
}

/*=====================================================================
FUNCTION Qalbum_Doperspective
  DESCRIPTION:
    This is the helper function for Perspective feature.

  PARAMETERS:
    pMe   - Pointer to Qalbum struct
    pLine - Starting point coodinate
    type  - Perspective type: Up, Down, Left, Right

  RETURN VALUE:
    TRUE if IIPL function success.
    FALSE otherwise.
======================================================================*/
static boolean Qalbum_Doperspective(Qalbum *pMe, IIPLLine *pLine, IIPLPerspectiveType type)
{
  int retVal = AEE_SUCCESS;
  if (!pMe || !pLine)
    return FALSE;
  retVal = IIPL_ViewPerspective(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut,
                                pLine, WHITE_RGB565, type) ;
  if (Qalbum_DisplayError(pMe, retVal) != AEE_SUCCESS)
    return FALSE;

  return TRUE;
}

/*=====================================================================
FUNCTION Qalbum_DoOverlap
  DESCRIPTION:
    This is the helper function for Overlap feature.

  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
    boolean
======================================================================*/
static boolean Qalbum_DoOverlap(Qalbum *pMe)
{
  int retVal = AEE_SUCCESS;
  if (!pMe)
    return FALSE;

  retVal = IIPL_Overlap(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditIn2, pMe->m_pEditOut,
                        &pMe->m_iRect);
  if (Qalbum_DisplayError(pMe, retVal) != AEE_SUCCESS)
    return FALSE;

  return TRUE;
}


/*=====================================================================
FUNCTION Qalbum_DoCompose

  DESCRIPTION: This is a helper function to compose 2 images in selected type.

  PARAMETERS:
    pMe  - Pointer to Qalbum struct
    id   - edit option id
    key  - key id
    type - compose type: IIPL_CENTER, IIPL_HORIZONTAL, IIPL_VERTICAL

  RETURN VALUE:
    boolean
======================================================================*/
static boolean Qalbum_DoCompose(Qalbum *pMe, uint16 id, uint16 key, IIPLComposeType type)
{
  AEEBitmapInfo info;
  int retVal = AEE_SUCCESS;
  int32 step = 0;

  if (!pMe)
    return FALSE;

  // if 2nd photo is not loaded, display thumbnail to allow selection
  if (!pMe->m_pEditIn2)
  {
    if (key == (uint16)AVK_UP) // can not go down
    {
      //reset the edit buffer
      Qalbum_UpdateInIPLImage(pMe, id);
      Qalbum_Get2ndFile(pMe, key);
    }
    else
    {
      (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, FALSE);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    }
  }
  else
  {
    if (IBITMAP_GetInfo(pMe->m_pEditIn2, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
      return FALSE;

    step = (type == IIPL_VERTICAL) ? (int32)info.cy : (int32)info.cx;

    if (Qalbum_EditStep(pMe, id, key, (uint16)(step / 4), step - 1, 0))
    {
      retVal = IIPL_Compose(pMe->m_pIIpl, pMe->m_pEditIn2,
                            pMe->m_pEditIn, pMe->m_pEditOut,
                            type, pMe->m_nEditProcessedVal);
      //Checking the Error After editing the IIPL
      if (Qalbum_DisplayError(pMe, retVal) == AEE_SUCCESS)
        return TRUE;
    }
  }
  return FALSE;
}

/*=====================================================================
FUNCTION Qalbum_Get2ndFile

  DESCRIPTION: This is a helper function to initialize some parameters for
               thumbnail display for 2nd file selection.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
    void
======================================================================*/
static void Qalbum_Get2ndFile(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return;

  pMe->eState = QCAM_STATE_THUMBNAIL;
  pMe->m_bIs2ndPhoto = TRUE;
  pMe->m_nStartPhoto = 1;
  pMe->m_n2ndPhoto = 1;
  pMe->m_wKeyEdit = key;
  ZEROAT(&pMe->m_SelectRC);
  Qalbum_DisplayPhoto(pMe, TRUE);
}

/* ==========================================================================
FUNCTION Qalbum_OnMagnify
  DESCRIPTION
    This function applies the IPL magnify function to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnMagnify(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  boolean bInrange = TRUE;
  AEEBitmapInfo info;

  if (!pMe)
    return FALSE;

  if (IBITMAP_GetInfo(pMe->m_pEditIn, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
    return FALSE;

  switch (key)
  {
  case AVK_SELECT:
    //reset the edit buffer
    Qalbum_UpdateInIPLImage(pMe, IDS_MAGNIFY);
    if (pMe->m_pEditIn && pMe->m_pEditOut)
    {
      // circle at center, radius = 1/4 of width
      pMe->m_iCircle.cX = (int16)info.cx / 2;
      pMe->m_iCircle.cY = (int16)info.cy / 2;
      pMe->m_iCircle.radius = (uint16)((info.cx < info.cy)? info.cx / 4 : info.cy / 4);
      if (Qalbum_DoMagnify(pMe))
      {
        pMe->m_bEditProcessed = TRUE;
        pMe->m_bInEdit = TRUE;
        bNeedUpdate = TRUE;
      }
    }
    break;

  case AVK_DOWN:
    if (pMe->m_bInEdit)
    {
      if ((pMe->m_iCircle.cY + (int16)info.cy / 8) < (int16)info.cy)
      {
        pMe->m_iCircle.cY += (int16)info.cy / 8;
        bNeedUpdate = Qalbum_DoMagnify(pMe);
      }
      else
      {
        bInrange = FALSE;
      }
    }
    break;

  case AVK_UP:
    if (pMe->m_bInEdit)
    {
      if (pMe->m_iCircle.cY > (int16)info.cy / 8)
      {
        pMe->m_iCircle.cY -= (int16)info.cy / 8;
        bNeedUpdate = Qalbum_DoMagnify(pMe);
      }
      else
      {
        bInrange = FALSE;
      }
    }
    break;

  case AVK_LEFT:
    if (pMe->m_bInEdit)
    {
      if (pMe->m_iCircle.cX > (int16)info.cx / 8)
      {
        pMe->m_iCircle.cX -= (int16)info.cx / 8;
        bNeedUpdate = Qalbum_DoMagnify(pMe);
      }
      else
      {
        bInrange = FALSE;
      }
    }
    break;

  case AVK_RIGHT:
    if (pMe->m_bInEdit)
    {
      if (pMe->m_iCircle.cX + (int16)info.cx / 8 < (int16)info.cx)
      {
        pMe->m_iCircle.cX += (int16)info.cx / 8;
        bNeedUpdate = Qalbum_DoMagnify(pMe);
      }
      else
      {
        bInrange = FALSE;
      }
    }
    break;

  case AVK_8:
    if (!pMe->m_bInEdit)
    {
      Qalbum_EditUndo(pMe);
      bNeedUpdate = TRUE;
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    break;

  case AVK_POUND:
    pMe->m_bInEdit = FALSE;
    Qalbum_DisplayAdvEditMenuName(pMe, 0);
    bNeedUpdate = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
    break;

  default:
    break;
  }

  if (!bInrange)
  {
    (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, FALSE);
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnNeon
  DESCRIPTION
    This function applies the IPL neon effect to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnNeon(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  int retVal = AEE_SUCCESS;
  if (!pMe)
    return FALSE;

  switch (key)
  {
  case AVK_UP:
    //reset the edit buffer
    Qalbum_UpdateInIPLImage(pMe, IDS_NEON);
    if (!pMe->m_bEditProcessed && pMe->m_pEditIn && pMe->m_pEditOut)
    {
      retVal = IIPL_Effect(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, IIPL_NEON, 0);
      // Checking the Error After editing the IIPL
      if (Qalbum_DisplayError(pMe, retVal) == AEE_SUCCESS)
      {
        pMe->m_bEditProcessed = TRUE;
        bNeedUpdate = TRUE;
      }
    }
    break;
  case AVK_DOWN:
    Qalbum_EditUndo(pMe);
    bNeedUpdate = TRUE;
    break;

  default:
    break;
  }

  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnPerspective
  DESCRIPTION
    This function applies the IPL perspective function to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnPerspective(Qalbum *pMe, uint16 key)
{
  AEEBitmapInfo info;
  IIPLLine      line;
  boolean       bNeedUpdate = FALSE;

  if (!pMe)
    return FALSE;

  //reset the edit buffer
  Qalbum_UpdateInIPLImage(pMe, IDS_PERSPECTIVE);

  if (IBITMAP_GetInfo(pMe->m_pEditIn, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
    return FALSE;

  if (key == (uint16)AVK_UP)
  {
    //update m_nEditProcessedVal
    pMe->m_nEditProcessedVal = (pMe->m_nEditProcessedVal + 1) % 4; // 4 orientations
    pMe->m_bEditProcessed = TRUE;
    switch (pMe->m_nEditProcessedVal)
    {
    case 0:
      line.sX = (int16)info.cx / 4;
      line.sY = 0;
      line.eX = (int16)(info.cx * 3 / 4);
      line.eY = 0;
      bNeedUpdate = Qalbum_Doperspective(pMe, &line, IIPL_PERSPECTIVE_UP);
      break;
    case 1:
      line.sX = (int16)info.cx;
      line.sY = (int16)info.cy / 4;
      line.eX = (int16)info.cx;
      line.eY = (int16)(info.cy * 3 / 4);
      bNeedUpdate = Qalbum_Doperspective(pMe, &line, IIPL_PERSPECTIVE_RIGHT);
      break;
    case 2:
      line.sX = (int16)info.cx / 4;
      line.sY = (int16)info.cy;
      line.eX = (int16)(info.cx * 3 / 4);
      line.eY = (int16)info.cy;
      bNeedUpdate = Qalbum_Doperspective(pMe, &line, IIPL_PERSPECTIVE_DOWN);
      break;
    case 3:
      line.sX = 0;
      line.sY = (int16)info.cy / 4;
      line.eX = 0;
      line.eY = (int16)(info.cy * 3 / 4);
      bNeedUpdate = Qalbum_Doperspective(pMe, &line, IIPL_PERSPECTIVE_LEFT);
      break;
    default:
      break;
    }
  }
  else if (key == (uint16)AVK_DOWN)
  {
    Qalbum_EditUndo(pMe);
    bNeedUpdate = TRUE;
  }

  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnFog
  DESCRIPTION
    This function applies the IPL fog effect to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnFog(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  int retVal = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //update m_nEditProcessedVal
    bNeedUpdate = Qalbum_EditStep(pMe, IDS_FOG, key,
                                  QCAM_FOG_FACTOR, QCAM_FOG_MAX, QCAM_FOG_MIN);
    if (bNeedUpdate)
    {
      // to de-fog, we cannot apply ipl function
      if (pMe->m_nEditProcessedVal == QCAM_FOG_MIN)
      {
        Qalbum_EditUndo(pMe);
        pMe->m_nEditProcessedVal = QCAM_FOG_MIN; // got reset to 0 by last call
      }
      else
      {
        retVal = IIPL_Effect(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, IIPL_FOG,
                             pMe->m_nEditProcessedVal) ;
        // Checking the Error After editing the IIPL
        if (Qalbum_DisplayError(pMe, retVal) != AEE_SUCCESS)
          bNeedUpdate = FALSE;
      }
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnGaussianBlur
  DESCRIPTION
    This function applies the IPL Gaussian blur to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnGaussianBlur(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  int retVal = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //update m_nEditProcessedVal
    if (Qalbum_EditStep(pMe, IDS_GAUSSIAN_BLUR, key, QCAM_BLUR_FACTOR,
                        QCAM_BLUR_MAX, QCAM_BLUR_MIN))
    {
      Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);
      retVal = IIPL_Filter(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, IIPL_BLUR_GAUSSIAN,
                           (uint32)pMe->m_nEditProcessedVal);
      // Checking the Error After editing the IIPL
      if (Qalbum_DisplayError(pMe, retVal) == AEE_SUCCESS)
        bNeedUpdate = TRUE;

    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnUniformBlur
  DESCRIPTION
    This function applies the IPL uniform blur to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnUniformBlur(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  int retVal = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //update m_nEditProcessedVal
    if (Qalbum_EditStep(pMe, IDS_UNIFORM_BLUR, key, QCAM_BLUR_FACTOR,
                        QCAM_BLUR_MAX, QCAM_BLUR_MIN))
    {
      Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);
      retVal = IIPL_Filter(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, IIPL_BLUR_UNIFORM,
                           (uint32)pMe->m_nEditProcessedVal);
      // Checking the Error After editing the IIPL
      if (Qalbum_DisplayError(pMe, retVal) == AEE_SUCCESS)
        bNeedUpdate = TRUE;
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnMotionBlur
  DESCRIPTION
    This function applies the IPL motion blur to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnMotionBlur(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  int retVal = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //update m_nEditProcessedVal
    if (Qalbum_EditStep(pMe, IDS_MOTION_BLUR, key, QCAM_BLUR_FACTOR,
                        QCAM_BLUR_MAX, QCAM_BLUR_MIN))
    {
      Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);
      retVal = IIPL_MotionBlur(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut,
                               (uint32)pMe->m_nEditProcessedVal, 0);
      // Checking the Error After editing the IIPL
      if (Qalbum_DisplayError(pMe, retVal) == AEE_SUCCESS)
        bNeedUpdate = TRUE;
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnArbRotation
  DESCRIPTION
    This function applies the IPL arbitrary rotation to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnArbRotation(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  int retVal = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //update m_nEditProcessedVal
    bNeedUpdate = Qalbum_EditStep(pMe, IDS_ARB_ROTATION, key, QCAM_ARB_ROTATE_FACTOR,
                                  QCAM_ARB_ROTATE_MAX, QCAM_ARB_ROTATE_MIN);
    if (bNeedUpdate)
    {
       retVal = IIPL_ArbitRotate(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut,
                                 (int16)pMe->m_nEditProcessedVal, WHITE_RGB565);
       // Checking the Error After editing the IIPL
       if (Qalbum_DisplayError(pMe, retVal) != AEE_SUCCESS)
         bNeedUpdate = FALSE;
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnAlphaBlend
  DESCRIPTION
    This function applies the IPL Alpha blending to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnAlphaBlend(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  int retVal = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    // if 2nd photo is not loaded, display thumbnail to allow selection
    if (!pMe->m_pEditIn2)
    {
      if (key == (uint16)AVK_UP) // can not go down
      {
        //reset the edit buffer
        Qalbum_UpdateInIPLImage(pMe, IDS_ALPHA_BLEND);
        Qalbum_Get2ndFile(pMe, key);
      }
      else
      {
        (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, FALSE);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      }
    }
    else
    {
      bNeedUpdate = Qalbum_EditStep(pMe, IDS_ALPHA_BLEND, key, QCAM_BLEND_FACTOR,
                                    QCAM_BLEND_MAX, QCAM_BLEND_MIN);
      if (bNeedUpdate)
      {
         retVal = IIPL_Compose(pMe->m_pIIpl, pMe->m_pEditIn2, pMe->m_pEditIn, pMe->m_pEditOut,
                               IIPL_BLEND, pMe->m_nEditProcessedVal);
         // Checking the Error After editing the IIPL
         if (Qalbum_DisplayError(pMe, retVal) != AEE_SUCCESS)
           bNeedUpdate = FALSE;
      }
    }
  }

  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnOverlap
  DESCRIPTION
    This function applies the IPL overlap function to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnOverlap(Qalbum *pMe, uint16 key)
{
  boolean       bNeedUpdate = FALSE;
  boolean       bInrange = TRUE;
  AEEBitmapInfo info;

  if (!pMe)
    return FALSE;

  if (IBITMAP_GetInfo(pMe->m_pEditIn, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
    return FALSE;

  switch (key)
  {
  case AVK_SELECT:
    //reset the edit buffer
    Qalbum_UpdateInIPLImage(pMe, IDS_OVERLAP);

    // if 2nd photo is not loaded, display thumbnail to allow selection
    if (!pMe->m_pEditIn2)
    {
      Qalbum_Get2ndFile(pMe, key);
    }
    else
    {
      if (pMe->m_pEditIn && pMe->m_pEditOut)
      {
        // overlap at center, region = 1/2 of width and height, 2 pixels margin
        pMe->m_iRect.dx = (int16)info.cx / 2 - 2;
        pMe->m_iRect.dy = (int16)info.cy / 2 - 2;
        pMe->m_iRect.x  = (int16)info.cx / 4;
        pMe->m_iRect.y  = (int16)info.cy / 4;
        if (Qalbum_DoOverlap(pMe))
        {
          pMe->m_bEditProcessed = TRUE;
          pMe->m_bInEdit = TRUE;
          bNeedUpdate = TRUE;
        }
      }
    }
    break;

  case AVK_DOWN:
    if (pMe->m_bInEdit)
    {
      if (pMe->m_iRect.y + (int16)info.cy / 8 < (int16)info.cy - pMe->m_iRect.y)
      {
        pMe->m_iRect.y += (int16)info.cy / 8;
        bNeedUpdate = Qalbum_DoOverlap(pMe);
      }
      else
      {
        bInrange = FALSE;
      }
    }
    break;

  case AVK_UP:
    if (pMe->m_bInEdit)
    {
      if (pMe->m_iRect.y >= (int16)info.cy / 8)
      {
        pMe->m_iRect.y -= (int16)info.cy / 8;
        bNeedUpdate = Qalbum_DoOverlap(pMe);
      }
      else
      {
        bInrange = FALSE;
      }
    }
    break;

  case AVK_LEFT:
    if (pMe->m_bInEdit)
    {
      if (pMe->m_iRect.x >= (int16)info.cx / 8)
      {
        pMe->m_iRect.x -= (int16)info.cx / 8;
        bNeedUpdate = Qalbum_DoOverlap(pMe);
      }
      else
      {
        bInrange = FALSE;
      }
    }
    break;

  case AVK_RIGHT:
    if (pMe->m_bInEdit)
    {
      if (pMe->m_iRect.x + (int16)info.cx / 8 < (int16)info.cx - pMe->m_iRect.x)
      {
        pMe->m_iRect.x += (int16)info.cx / 8;
        bNeedUpdate = Qalbum_DoOverlap(pMe);
      }
      else
      {
        bInrange = FALSE;
      }
    }
    break;

  case AVK_8:
    if (!pMe->m_bInEdit)
    {
      Qalbum_EditUndo(pMe);
      bNeedUpdate = TRUE;
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    break;

  case AVK_POUND:
    pMe->m_bInEdit = FALSE;
    Qalbum_DisplayAdvEditMenuName(pMe, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
    break;

  default:
    break;
  }

  if (!bInrange)
  {
    (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, FALSE);
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnHorizontalComp
  DESCRIPTION
    This function applies the IPL horizontal composition function to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnHorizontalComp(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return FALSE;

  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //Update needed if edit success
    return Qalbum_DoCompose(pMe, IDS_HORIZONTAL_COMP, key, IIPL_HORIZONTAL);
  }

  return FALSE;
}

/* ==========================================================================
FUNCTION Qalbum_OnVerticalComp
  DESCRIPTION
    This function applies the IPL vertical composition function to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnVerticalComp(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return FALSE;

  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //Update needed if edit success
    return Qalbum_DoCompose(pMe, IDS_VERTICAL_COMP, key, IIPL_VERTICAL);
  }

  return FALSE;
}

/* ==========================================================================
FUNCTION Qalbum_OnCenterComp
  DESCRIPTION
    This function applies the IPL Center composition function to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnCenterComp(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return FALSE;

  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //Update needed if edit success
    return Qalbum_DoCompose(pMe, IDS_CENTER_COMP, key, IIPL_CENTER);
  }

  return FALSE;
}

/* ==========================================================================
FUNCTION Qalbum_OnWhiteboard
  DESCRIPTION
    This function applies the IPL whiteboard effect to the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnWhiteboard(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //update m_nEditProcessedVal
    if (Qalbum_EditStep(pMe, IDS_WHITEBOARD, key, QCAM_THRESHOLD_FACTOR,
                        QCAM_THRESHOLD_MAX, QCAM_THRESHOLD_MIN))
    {
      Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);
      if (pMe->m_nEditProcessedVal == 0)
      {
        Qalbum_EditUndo(pMe);
        bNeedUpdate = TRUE;
      }
      else
      {
        bNeedUpdate = Qalbum_EditEffect(pMe, IIPL_WHITEBOARD, FALSE);
      }
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnHistogram
  DESCRIPTION
    This function draws a 2D luma histogram chart on the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnHistogram(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  int retVal = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  switch (key)
  {
  case AVK_UP:
    //reset the edit buffer
    Qalbum_UpdateInIPLImage(pMe, IDS_HISTOGRAM);
    if (pMe->m_pEditIn && pMe->m_pEditOut)
    {
      retVal = IIPL_DrawHistogram(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, NULL) ;
      // Checking the Error After editing the IIPL
      if (Qalbum_DisplayError(pMe, retVal) == AEE_SUCCESS)
      {
        pMe->m_bEditProcessed = TRUE;
        bNeedUpdate = TRUE;
      }

    }
    break;

  case AVK_DOWN:
    Qalbum_EditUndo(pMe);
    bNeedUpdate = TRUE;
    break;

  default:
    break;
  }

  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnRgbHistogram
  DESCRIPTION
    This function draws a 2D RGB histogram chart on the current photo.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id

  RETURN VALUE:
   TRUE if need photo redrawn, FALSE otherwise

============================================================================= */
static boolean Qalbum_OnRgbHistogram(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  int retVal = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  switch (key)
  {
  case AVK_UP:
    //reset the edit buffer
    Qalbum_UpdateInIPLImage(pMe, IDS_RGB_HISTOGRAM);
    if (pMe->m_pEditIn && pMe->m_pEditOut)
    {
      retVal = IIPL_DrawRgbHistogram(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, NULL);
      // Checking the Error After editing the IIPL
      if (Qalbum_DisplayError(pMe, retVal) == AEE_SUCCESS)
      {
        pMe->m_bEditProcessed = TRUE;
        bNeedUpdate = TRUE;
      }

    }
    break;

  case AVK_DOWN:
    Qalbum_EditUndo(pMe);
    bNeedUpdate = TRUE;
    break;

  default:
    break;
  }

  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_GetStartValue
  DESCRIPTION
    This function returns the starting processing factor for various IPL functions.

  PARAMETERS:
    selection - edit option id

  RETURN VALUE:
   IPL processing factor

============================================================================= */
int Qalbum_GetStartValue(uint16 selection)
{
  int value = 0;

  switch (selection)
  {
  case IDS_PERSPECTIVE:
    value = QCAM_PERSPECTIVE_MIN;
    break;
  case IDS_FOG:
    value = QCAM_FOG_MIN;
    break;
  case IDS_GAUSSIAN_BLUR:
  case IDS_UNIFORM_BLUR:
  case IDS_MOTION_BLUR:
    value = QCAM_BLUR_MIN;
    break;
  case IDS_WHITEBOARD:
    value = QCAM_THRESHOLD_MIN;
    break;
  default:
    break;
  }
  return value;
}

#endif //#if defined(FEATURE_APP_QALBUM)

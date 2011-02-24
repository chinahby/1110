/*===========================================================================

FILE: qalbumEdit.c

GENERAL DESCRIPTION
    This file contains the editing features of Qalbum.

EXTERNALIZED FUNCTIONS
    Qalbum_DisplayEditMenuName
    Qalbum_EditKeyHandler
    Qalbum_MenuKeyHandler
    Qalbum_AddImageFrame

      Copyright (c) 2006 by QUALCOMM, Inc.  All Rights Reserved.


===========================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qalbum/main/latest/src/qalbumEdit.c#2 $
 $DateTime: 2008/10/14 02:33:51 $ $Author: c_caluru $

 when      who    what, where, why

 --------  -----  ----------------------------------------------------------
 10/14/08  chai   Fixed Rvct warnings on ULC
 12/15/07  KV     Added Qalbum_ExecuteColorFormatMenu for executing the Jpeg Color
                  Format.
 09/14/07  KD     Removed deprecated function calls
 06/29/07  NC     Removed up arrow in some edit features when reaching to max step.
 06/08/07  NC     Fixed some automation report issues.
 05/11/07  NC     Fixed bug in handling different color format.
 04/19/07  SK     UI Support for WVGA Display
 04/18/07  NC     Fixed bug in Qalbum_MenuKeyHandler
 04/16/07  NC     Fixed error return bugs.
 03/23/07  NC     Fixed compiler warning.
 11/09/06  ZL     Changes caused by adding VIEW state before EDIT state.
 11/08/06  ZL     Moved editing feature from qalbum.c.

============================================================================*/

//lint -e740 suppress errors on pointer cast
//lint -e611
//lint -e801 suppress errors on use of goto
//lint -e826 suppress errors on pointer conversion area small

//lint -emacro(570, IDISPLAY_ClearScreen) supress errors on Brew macros
//lint -emacro(522, IDISPLAY_ClearScreen)
//lint -emacro(570, IDISPLAY_EraseRect)

//lint -e765, Qalbum_CreateInstance, Qalbum_GetModInfo and Qalbum_Load could be made static
//lint -e714, Qalbum_GetModInfo not referenced

/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#ifndef AEE_SIMULATOR
#include "msg.h"
#endif

#if defined(FEATURE_APP_QALBUM)
#include "QcamDef.h"
#include "QcamDisp.h"
#include "qalbumEdit_i.h"

#ifdef HEAP_CHECK
#include "AEEHeap.h"
#include "malloc_mgr.h"
#define MSG_HEAP MSG_ERROR
#endif

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION Qalbum_DisplayEditMenuName

  DESCRIPTION
    This function displays the selected menu item name of the edit menu.

  PARAMETERS:
    pMe  - pointer to Qalbum data struct
    item - move direction on current item (-1, 0, 1).

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_DisplayEditMenuName(Qalbum *pMe, int item)
{
  uint16 selection;
  int    index;
  int    count;

  uint16 image1ID    = 0;
  uint16 image1StrID = 0;
  uint16 image2ID    = 0;
  uint16 image2StrID = 0;

  if (!pMe)
    return;

  //get old item index
  if (!IMENUCTL_GetItemData(pMe->m_pISoftMenu, pMe->m_wEditMenuSel, (uint32*)&index))
    return;

  //get total items
  count = IMENUCTL_GetItemCount(pMe->m_pISoftMenu);
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
  case IDS_IMAGE_RESIZE:
    //Redraw the navigate arrows in case back from resize menu
    Qalbum_DisplayListNavArrows(pMe);
    //lint -fallthrough for key assignment.
  case IDS_FRAME:
  case IDS_IMAGE_INVERT_COLORS:
  case IDS_FLASH:
  case IDS_IMAGE_GRAY_SCALE:
  case IDS_IMAGE_SEPIA:
    //If image has been edited, add undo option.
    if (pMe->m_bEditProcessed && pMe->m_wEditProcessedBy == selection)
    {
      image1ID = 0;
      image2ID = IDB_DOWN;
      image2StrID  = IDS_UNDO;
    }
    else
    {
      image1ID = IDB_UP;
    }
    if (pMe->m_bSmallScreen)
       Qalbum_DisplayTitle_SM(pMe, image1ID, image2ID, selection);
    else
      Qalbum_DisplayTitle(pMe, 0, 0, image1ID, selection, image2ID, image2StrID);
    break;

  case IDS_BRIGHTNESS:
  case IDS_IMAGE_HUE:
  case IDS_IMAGE_SATURATION:
  case IDS_IMAGE_SHARPNESS:
    {
      int32 val;

      image1ID = IDB_UP;
      image2ID = IDB_DOWN;
      image1StrID = IDS_INCREASE;
      image2StrID = IDS_DECREASE;

      if (pMe->m_wEditProcessedBy == selection)
      {
        val = pMe->m_nEditProcessedVal;
      }
      else
      {
        val = pMe->m_nEditProcessedVal = 0;
      }

      //If it is max value, cannot increase more, hide increase option
      if (Qalbum_IsMaxValue(val, selection))
      {
        image1StrID = 0;
        image1ID    = 0;
      }
      //If it is min value, cannot decrease more, hide decrease option
      else if (Qalbum_IsMinValue(val, selection))
      {
        image2StrID = 0;
        image2ID    = 0;
      }

      if (pMe->m_bSmallScreen)
        Qalbum_DisplayTitle_SM(pMe, image1ID, image2ID, selection);
      else
        Qalbum_DisplayTitle(pMe, 0, selection, image1ID, image1StrID, image2ID, image2StrID);
      break;
    }

  case IDS_IMAGE_SOLARIZE:
  case IDS_IMAGE_POSTERIZE:
    {
      int32 val;

      image1ID = IDB_UP;
      image2ID = IDB_DOWN;
      image1StrID = IDS_INCREASE;
      image2StrID = IDS_DECREASE;

      // Remember m_nEditProcessedVal according key selection.
      if (item == 1)
      {
        pMe->m_nPreviousVal = pMe->m_nEditProcessedVal;
      }
      if (item == -1)
      {
        pMe->m_nPreviousVal2 = pMe->m_nEditProcessedVal;
      }

      // Set the Value before toggeling the key
      if (pMe->m_wEditProcessedBy!= selection)
      {
        pMe->m_nEditProcessedVal = (selection == IDS_IMAGE_SOLARIZE) ?
                                   QCAM_SOLARIZE_MAX : QCAM_POSTERIZE_MAX;
      }
      else
      {
        //Selection is processed before. Pick up the old value.
        if (item == 1) //rigth key to option
        {
          pMe->m_nEditProcessedVal = pMe->m_nPreviousVal2;
        }
        else if (item == -1) //left key to option
        {
          pMe->m_nEditProcessedVal = pMe->m_nPreviousVal;
        }

        //If user navigates menu option in one direction for more than one time,
        //old value is no longer the one set by this selection. Reset the value.
        if (selection == IDS_IMAGE_SOLARIZE)
        {
          //More checking for Solarize. pMe->m_nEditProcessedVal set by both sides
          //are smaller than QCAM_SOLARIZE_MIN when selection is IDS_IMAGE_SOLARIZE.
          if (pMe->m_nEditProcessedVal < QCAM_SOLARIZE_MIN)
          {
            pMe->m_nEditProcessedVal = QCAM_SOLARIZE_MAX;
          }
        }
        else //(selection == IDS_IMAGE_POSTERIZE)
        {
          //More checking for Posterize. pMe->m_nEditProcessedVal set by both sides
          //could be out of range when selection is IDS_IMAGE_POSTERIZE.
          if ((pMe->m_nEditProcessedVal > QCAM_POSTERIZE_MAX) ||
              (pMe->m_nEditProcessedVal < QCAM_POSTERIZE_MIN))
          {
            pMe->m_nEditProcessedVal = QCAM_POSTERIZE_MAX;
          }
        }
      }
      val = pMe->m_nEditProcessedVal;

      //Solarize and Posterize has reverse order that the effect
      //is increased while number is decreased.
      if (Qalbum_IsMinValue(val, selection))
      {
        image1StrID = 0;
        image1ID    = 0;
      }
      else if (Qalbum_IsMaxValue(val, selection))
      {
        image2StrID = 0;
        image2ID    = 0;
      }

      if (pMe->m_bSmallScreen)
        Qalbum_DisplayTitle_SM(pMe, image1ID, image2ID, selection);
      else
        Qalbum_DisplayTitle(pMe, 0, selection, image1ID, image1StrID, image2ID, image2StrID);
      break;
    }

  case IDS_IMAGE_ROTATE:
    if (pMe->m_bSmallScreen)
      Qalbum_DisplayTitle_SM(pMe, IDB_UP, IDB_DOWN, selection);
    else
      Qalbum_DisplayTitle(pMe, 0, selection, IDB_UP, IDS_RIGHT, IDB_DOWN, IDS_LEFT);
    break;

  case IDS_IMAGE_FLIP:
    if (pMe->m_bSmallScreen)
        Qalbum_DisplayTitle_SM(pMe, IDB_UP, IDB_DOWN, selection);
    else
      Qalbum_DisplayTitle(pMe, 0, selection, IDB_UP, IDS_VERTICAL, IDB_DOWN, IDS_HORIZONTAL);
    break;

  default:
    break;
  }
}

/*===========================================================================
FUNCTION Qalbum_EditKeyHandler

  DESCRIPTION
    Key handler for Qalbum in edit state

  PARAMETERS:
    pMe   - pointer to Qalbum data struct
    eCode - AEEEvent id
    key   - key id

  RETURN VALUE:
    TRUE if event process, FALSE otherwise
===========================================================================*/
boolean Qalbum_EditKeyHandler(Qalbum *pMe, AEEEvent eCode, uint16 key)
{
  uint16 sel;
  uint32 index = 0;
  boolean update_display = FALSE;
  boolean bHandled = TRUE;

  if (!pMe)
    return FALSE;

  //Message for error, Could not determine where to go.
  //Just let the clr key activate.
  if (pMe->m_bMsgTimerOn)
  {
    if (key == (uint16)AVK_CLR)
    {
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
      pMe->m_bMsgTimerOn = FALSE;
    }
    else
      return TRUE;
  }

  sel  = IMENUCTL_GetSel(pMe->m_pISoftMenu);
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
    if (pMe->m_bPhotoErased)
    {
      pMe->m_bPhotoErased = FALSE;
      CAM_RELEASEIF(pMe->m_pIStatic);
      Qalbum_UpdateEditedImage(pMe);
    }
    //Navigate through menu icons
    Qalbum_DisplayEditMenuName(pMe, (key == (uint16)AVK_LEFT ? -1 : 1) );
    bHandled = IMENUCTL_HandleEvent(pMe->m_pISoftMenu, eCode, key, 0);
    break;

  case AVK_DOWN:
  case AVK_UP:
  case AVK_SELECT:
  case AVK_STAR:
    if (pMe->m_pEditIn) // Edit only if image is successfully loaded
    {
      //Execute menu on focus
      MSG_MED("Select Edit feature with ID %d", sel, 0, 0);
      if (QalbumEditHandlers[index].wMenuID == sel)
      {
        update_display = QalbumEditHandlers[index].MenuOnKeyFunc(pMe, key);
      }
      else
      {
        //incase the item is filtered out dyna
        for (index = 0; index < ARR_SIZE(QalbumEditHandlers); index++)
        {
          if (QalbumEditHandlers[index].wMenuID == sel)
          {
            update_display = QalbumEditHandlers[index].MenuOnKeyFunc(pMe, key);
            break;
          }
        }
      }
    }
    break;

  case AVK_CLR:
    Qalbum_ReturnView(pMe, IDS_EDIT);
    break;

  default:
    break;
  }

  if (update_display)
  {
    MSG_MED("Qalbum_UpdateEditedImage after edit", 0, 0, 0);
    Qalbum_UpdateEditedImage(pMe);
  }

#ifdef HEAP_CHECK
  MSG_HEAP("EditHandler:", 0, 0, 0);
  Qalbum_ShowHeapUsage(pMe);
#endif
  return bHandled;
}

/*===========================================================================
FUNCTION Qalbum_MenuKeyHandler

  DESCRIPTION
    Key handler for Qalbum in (list) menu state.
    Only Set size menu in this state currently

  PARAMETERS:
    pMe   - pointer to Qalbum data struct
    eCode - AEEEvent id
    key   - key id

  RETURN VALUE:
   None
===========================================================================*/
boolean Qalbum_MenuKeyHandler(Qalbum *pMe, AEEEvent eCode, uint16 key)
{
  boolean bHandled = TRUE;

  if (!pMe)
    return FALSE;

  if (eCode != EVT_KEY)
    return FALSE;

  MSG_MED("Qalbum_MenuKeyHandler - key %d", key, 0, 0);
  switch (key)
  {
  case AVK_LEFT:
  case AVK_RIGHT:
    return TRUE;

  case AVK_UP:
  case AVK_DOWN:
    bHandled = IMENUCTL_HandleEvent(pMe->m_pIMenu, EVT_KEY, key, 0);
    if (bHandled)
    {
      switch (pMe->m_eActiveMenu)
      {
      case RESIZE_MENU:
        Qalbum_UpdateResizeMenu(pMe);
        break;
	  case JPEG_COLOR_MENU:
	  	Qalbum_UpdateColorFormatMenu(pMe);
		break;
      case PRINT_MENU:
#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP)  || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
        Qalbum_UpdatePrintMenu(pMe);
#endif //#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
        break;

      }
    }
    break;

  case AVK_SELECT:
    bHandled = IMENUCTL_HandleEvent(pMe->m_pIMenu, EVT_KEY, key, 0);
    break;

  case AVK_CLR:
    if (pMe->m_eActiveMenu == RESIZE_MENU)
    {
      //Return to edit state
      pMe->eState = QCAM_STATE_EDIT;
    }
	else if(pMe->m_eActiveMenu == JPEG_COLOR_MENU)
	{
	  pMe->eState = QCAM_STATE_DIR;	
      IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);
      //Clean up screen
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);	  
	  Qalbum_DisplayPhotoDirs(pMe, TRUE);
	  return bHandled;
	}
    else
    {
      //Return to view state
      pMe->eState = QCAM_STATE_VIEW;
    }
    IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);
    //Clean up screen
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    // Redraw the image
    MSG_MED("On clr - Qalbum_UpdateEditedImage.", 0, 0, 0);
    Qalbum_UpdateEditedImage(pMe);
    break;
  default:
    break;
  }
  return bHandled;
}

/*===========================================================================
FUNCTION Qalbum_AddImageFrame

  DESCRIPTION


  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
   None
===========================================================================*/
void Qalbum_AddImageFrame(Qalbum *pMe)
{
  IDIB         *inDIB;
  IDIB         *tmpDIB;
  uint16        transparent;
  AEEBitmapInfo info;
  int           retVal = AEE_SUCCESS;
  uint16        msgId = 0;


  if (!pMe)
    return;

  // If fail to load frame, not need to update the image.
  pMe->m_bEditProcessed = FALSE;

  if (pMe->m_sFrameFileName[0] == '\0')
  {
    MSG_HIGH( "pMe->m_sFrameFileName is empty", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    return;
  }

  //Load the image frame to a IDIB pointer. It will contain a RGB888 bitmap.
  MSG_MED( "Start QCam_LoadPNGFrame", 0, 0, 0);
  if (!QCam_LoadPNGFrame(pMe->a.m_pIShell, pMe->m_pIFileMgr, pMe->m_sFrameFileName, &inDIB))
  {
    MSG_HIGH( "QCam_LoadPNGFrame failed", 0, 0, 0);
    msgId = IDS_MEM_FULL;
    goto Done;
  }

  //Convert the transparent color to 565 format for this LCD device
  MSG_MED( "Start color conversion", 0, 0, 0);
  QCam_Get565TransparentVal(inDIB->ncTransparent, &transparent);

  if (IDISPLAY_CreateDIBitmap(pMe->a.m_pIDisplay, &tmpDIB, 16,
                              inDIB->cx, inDIB->cy) != SUCCESS)
  {
    MSG_HIGH( "IDISPLAY_CreateDIBitmap failed", 0, 0, 0);
    msgId = IDS_MEM_FULL;
    goto Done;
  }
  tmpDIB->nColorScheme  = IDIB_COLORSCHEME_565;
  tmpDIB->ncTransparent = transparent;

  //Convert the image format from 888 to 565
  switch (inDIB->nDepth)
  {
    case 8:
      (void)ipl_RGBA8882RGB565plt(inDIB->pBmp, (uint8 *)inDIB->pRGB, (uint16 *)tmpDIB->pBmp,
                            (int16)inDIB->cx, (int16)inDIB->cy);
      break;
    case 16:
      (void)MEMCPY(tmpDIB->pBmp, inDIB->pBmp, inDIB->cx * inDIB->cy * 2);
      break;
    case 24:
      (void)ipl_RGB8882RGB565be(inDIB->pBmp, (uint16 *)tmpDIB->pBmp, (int16)inDIB->cx,
                              (int16)inDIB->cy);
      break;
    default:
      break;
  }
  //Compose the frame and the image
  if (IBITMAP_GetInfo(pMe->m_pEditIn, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
  {
    msgId = IDS_MEM_FULL;
    goto Done;
  }
  if(info.cx == tmpDIB->cx && info.cy == tmpDIB->cy)
  {
    MSG_MED( "Start IIPL_RotateCropAddFrame", 0, 0, 0);
    retVal = IIPL_RotateCropAddFrame(pMe->m_pIIpl, pMe->m_pEditIn, IDIB_TO_IBITMAP(tmpDIB),
                                     pMe->m_pEditOut, NULL, 0, NULL, IIPL_NO_ROTATE) ;
    if (Qalbum_DisplayError(pMe, retVal) != SUCCESS)
    {
      MSG_HIGH( "IIPL_RotateCropAddFrame failed", 0, 0, 0);
      goto Done;
    }
  }
  else
  {
    MSG_HIGH( "Bad frame size", 0, 0, 0);
    msgId = IDS_BAD_FRM_SIZE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    goto Done;
  }

  // update the edit image and redraw the edit display
  pMe->m_bEditProcessed = TRUE;
  Qalbum_DisplayListNavArrows(pMe);
  Qalbum_DisplayEditMenuName(pMe, 0);
  Qalbum_UpdateEditedImage(pMe);

Done:
  CAM_RELEASEIF(inDIB);
  CAM_RELEASEIF(tmpDIB);
  if (msgId > 0)
  {
    Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, msgId);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
  }
}

/* ==========================================================================
FUNCTION Qalbum_OnResize
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnResize(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    bNeedUpdate = Qalbum_OnUpKey(pMe, IDS_IMAGE_RESIZE, key);
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnFlash
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnFlash(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //implement on key
    bNeedUpdate = Qalbum_OnUpKey(pMe, IDS_FLASH, key);
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnFrame
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnFrame(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //implement on key
    bNeedUpdate = Qalbum_OnUpKey(pMe, IDS_FRAME, key);
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnGrayScale
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnGrayScale(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //implement on key
    bNeedUpdate = Qalbum_OnUpKey(pMe, IDS_IMAGE_GRAY_SCALE, key);
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnInvertColors
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnInvertColors(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //implement on key
    bNeedUpdate = Qalbum_OnUpKey(pMe, IDS_IMAGE_INVERT_COLORS, key);
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnBrightness
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnBrightness(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
#if QALBUM_MEASURE_TIME
  uint32 time1 = 0;
  uint32 time2 = 0;
#endif

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //update m_nEditProcessedVal
    bNeedUpdate = Qalbum_EditStep(pMe, IDS_BRIGHTNESS, key,
                                  QCAM_BRIGHTNESS_FACTOR,
                                  QCAM_BRIGHTNESS_MAX,
                                  QCAM_BRIGHTNESS_MIN);
    if (bNeedUpdate)
    {
      Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

#if QALBUM_MEASURE_TIME
      time1 = GETTIMEMS();
#endif
      bNeedUpdate = Qalbum_EditEffect(pMe, IIPL_INTENSITY, FALSE);

#if QALBUM_MEASURE_TIME
      time2 = GETTIMEMS();
      MSG_HIGH("IIPL_Effect - intensity took %d", time2 - time1, 0, 0);
#endif
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnHue
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnHue(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
#if QALBUM_MEASURE_TIME
  uint32 time1 = 0;
  uint32 time2 = 0;
#endif

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //update m_nEditProcessedVal
    bNeedUpdate = Qalbum_EditStep(pMe, IDS_IMAGE_HUE, key,
                                QCAM_HUE_FACTOR, QCAM_HUE_MAX, QCAM_HUE_MIN);
    if (bNeedUpdate)
    {
      Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

#if QALBUM_MEASURE_TIME
      time1 = GETTIMEMS();
#endif
      bNeedUpdate = Qalbum_EditEffect(pMe, IIPL_HUE, FALSE);

#if QALBUM_MEASURE_TIME
      time2 = GETTIMEMS();
      MSG_HIGH("IIPL_Effect - hue took %d", time2 - time1, 0, 0);
#endif
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnPosterize
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnPosterize(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //revert the key for implement in different direction
    uint16 revKey = (key == (uint16)AVK_UP) ? (uint16)AVK_DOWN : (uint16)AVK_UP;

    //update m_nEditProcessedVal
    bNeedUpdate = Qalbum_EditStep(pMe, IDS_IMAGE_POSTERIZE, revKey,
                                  QCAM_POSTERIZE_FACTOR,
                                  QCAM_POSTERIZE_MAX,
                                  QCAM_POSTERIZE_MIN);

    if (bNeedUpdate)
    {
      bNeedUpdate = Qalbum_EditEffect(pMe, IIPL_POSTERIZE, FALSE);
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnSolarize
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnSolarize(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //revert the key for implement in different direction
    uint16 revKey = (key == (uint16)AVK_UP) ? (uint16)AVK_DOWN : (uint16)AVK_UP;

    //update m_nEditProcessedVal
    bNeedUpdate = Qalbum_EditStep(pMe, IDS_IMAGE_SOLARIZE, revKey,
                                QCAM_SOLARIZE_FACTOR,
                                QCAM_SOLARIZE_MAX,
                                QCAM_SOLARIZE_MIN);

    if (bNeedUpdate)
    {
      bNeedUpdate = Qalbum_EditEffect(pMe, IIPL_SOLARIZE, FALSE);
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnSaturation
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnSaturation(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
#if QALBUM_MEASURE_TIME
  uint32 time1 = 0;
  uint32 time2 = 0;
#endif

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //update m_nEditProcessedVal
    bNeedUpdate = Qalbum_EditStep(pMe, IDS_IMAGE_SATURATION, key,
                                  QCAM_SATURATION_FACTOR,
                                  QCAM_SATURATION_MAX,
                                  QCAM_SATURATION_MIN);

    if (bNeedUpdate)
    {
      Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

#if QALBUM_MEASURE_TIME
      time1 = GETTIMEMS();
#endif
      bNeedUpdate = Qalbum_EditEffect(pMe, IIPL_SATURATE, FALSE);

#if QALBUM_MEASURE_TIME
      time2 = GETTIMEMS();
      MSG_HIGH("IIPL_Effect - saturate took %d", time2 - time1, 0, 0);
#endif
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnSharpness
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnSharpness(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  int          retVal = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    //update m_nEditProcessedVal
    bNeedUpdate = Qalbum_EditStep(pMe, IDS_IMAGE_SHARPNESS, key, 1, 1, -1);

    if (bNeedUpdate)
    {
      //call implement function
      if (pMe->m_nEditProcessedVal > 0)
      {
        retVal = IIPL_Filter(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut,
                             IIPL_SHARP, 0);
        // Checking for the IIPL Edit Error
        if (Qalbum_DisplayError(pMe, retVal) != SUCCESS)
        {
          bNeedUpdate = FALSE;
          MSG_HIGH("IIPL_Filter failed - IIPL_SHARP.", 0, 0, 0);
        }
      }
      else if (pMe->m_nEditProcessedVal < 0)
      {
        retVal = IIPL_Filter(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut,
                             IIPL_BLUR, 0);
        // Checking for the IIPL Edit Error
        if (Qalbum_DisplayError(pMe, retVal) != SUCCESS)
        {
          bNeedUpdate = FALSE;
          MSG_HIGH("IIPL_Filter failed IIPL_BLUR.", 0, 0, 0);
        }
      }
      else
        Qalbum_EditUndo(pMe);
    }
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnRotate
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnRotate(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    bNeedUpdate = Qalbum_DoRotate(pMe, key);
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnSepia
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnSepia(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    bNeedUpdate = Qalbum_OnUpKey(pMe, IDS_IMAGE_SEPIA, key);
  }
  return bNeedUpdate;
}

/* ==========================================================================
FUNCTION Qalbum_OnFlip
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnFlip(Qalbum *pMe, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  //Only responds to UP/DOWN keys
  if (key == (uint16)AVK_UP || key == (uint16)AVK_DOWN)
  {
    bNeedUpdate = Qalbum_DoFlip(pMe, key);
  }
  return bNeedUpdate;
}

/*===========================================================================
FUNCTION Qalbum_OnUpKey

  DESCRIPTION
    This function provides edit feature On up key and calls undo for down key

  PARAMETERS:
    pMe - pointer to Qalbum data struct
    sel - selected edit item ID
    key - Up/Down key ID

  RETURN VALUE:
   TRUE if rotate action handled
===========================================================================*/
static boolean Qalbum_OnUpKey(Qalbum *pMe, uint16 sel, uint16 key)
{
  boolean bNeedUpdate = FALSE;
  AEEBitmapInfo   info;

  if (!pMe)
    return FALSE;

  //reset the edit buffer
  Qalbum_UpdateInIPLImage(pMe, sel);

  if (key == (uint16)AVK_UP)
  {
    if (!pMe->m_bEditProcessed)
    {
      if (pMe->m_pEditIn && pMe->m_pEditOut)
      {
        bNeedUpdate = TRUE;
        pMe->m_bEditProcessed = TRUE;
        switch (sel)
        {
        case IDS_FRAME:
          if (Qalbum_GetMatchFrameDir(pMe))
          {
            //Display frames in thumbnail mode
            //Once a frame is selected, it will switch back to edit mode
            Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);
            pMe->eState = QCAM_STATE_THUMBNAIL;
            pMe->m_sFrameFileName[0] = '\0';
            MSG_MED("Start Qalbum_DisplayFrames in %s", pMe->m_sFrameDir, 0, 0);
            Qalbum_DisplayFrames(pMe, TRUE, TRUE);
          }
          else
          {
            Qalbum_DisplayTimedImg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDB_NO_FRAME);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
          }
          bNeedUpdate = FALSE;
          pMe->m_bEditProcessed = FALSE;
          break;

        case IDS_IMAGE_RESIZE:
          pMe->eState = QCAM_STATE_MENU;
          if (IBITMAP_GetInfo(pMe->m_pEditIn, &info, sizeof(AEEBitmapInfo)) == SUCCESS)
          {
            pMe->m_nImgSize = Qalbum_GetImageSize((int)info.cx, (int)info.cy);
            Qalbum_BuildResizeMenu(pMe);
          }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

          bNeedUpdate = FALSE;
          pMe->m_bEditProcessed = FALSE;
          break;

        case IDS_FLASH:
          bNeedUpdate = Qalbum_EditEffect(pMe, IIPL_FLASH, TRUE);
          break;

        case IDS_IMAGE_GRAY_SCALE:
          bNeedUpdate = Qalbum_EditEffect(pMe, IIPL_GRAYSCALE, TRUE);
          break;

        case IDS_IMAGE_INVERT_COLORS:
          bNeedUpdate = Qalbum_EditEffect(pMe, IIPL_NEGATIVE, TRUE);
          break;

        case IDS_IMAGE_SEPIA:
          bNeedUpdate = Qalbum_EditEffect(pMe, IIPL_SEPIA, TRUE);
          break;

        default:
          break;
        }
      }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
  else
  {
     Qalbum_EditUndo(pMe);
     bNeedUpdate = TRUE;
  }
  return bNeedUpdate;
}

/*===========================================================================
FUNCTION Qalbum_DoRotate

  DESCRIPTION
    This function rotates the image in 90 degree step
    On Up key   - rotate right (clockwise)
    On Down key - rotate left (counterclockwise)
  PARAMETERS:
    pMe - pointer to Qalbum data struct
    key - Up/Down key ID

  RETURN VALUE:
   TRUE if rotate action handled
===========================================================================*/
static boolean Qalbum_DoRotate(Qalbum *pMe, uint16 key)
{
  IDIB   *pDIB     = NULL;
  IYCbCr *pYCbCr   = NULL;
  boolean bHandled = FALSE;
  int     retVal   = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  //reset the edit buffer
  Qalbum_UpdateInIPLImage(pMe, IDS_IMAGE_ROTATE);
  if (pMe->m_pEditIn && pMe->m_pEditOut)
  {
    bHandled = TRUE;
    pMe->m_bEditProcessed = TRUE;
    // init nEditProcessedVal if out of range
    if (pMe->m_nEditProcessedVal >= IIPL_MAX_ROTATE_VAL)
    {
      pMe->m_nEditProcessedVal = 0;
    }
    if (key == (uint16)AVK_UP)
    {
      pMe->m_nEditProcessedVal++;
      if (pMe->m_nEditProcessedVal > IIPL_ROTATE_270)
      {
         pMe->m_nEditProcessedVal = IIPL_NO_ROTATE;
      }
    }
    else
    {
      pMe->m_nEditProcessedVal--;
      if (pMe->m_nEditProcessedVal < IIPL_NO_ROTATE)
      {
         pMe->m_nEditProcessedVal = IIPL_ROTATE_270;
      }
    }
    if (pMe->m_nEditProcessedVal == 0)
    {
      Qalbum_EditUndo(pMe);
    }
    else
    {
      if (IBITMAP_QueryInterface(pMe->m_pEditIn, AEECLSID_DIB, (void **)&pDIB) == SUCCESS)
      {
        if (pMe->m_nEditProcessedVal != IIPL_ROTATE_180)
        {
          ((IDIB *)pMe->m_pEditOut)->nPitch = (int16)(((IDIB *)pMe->m_pEditIn)->cy * 
                                              (((IDIB *)pMe->m_pEditIn)->nDepth / 8));
          ((IDIB *)pMe->m_pEditOut)->cx = ((IDIB *)pMe->m_pEditIn)->cy;
          ((IDIB *)pMe->m_pEditOut)->cy = ((IDIB *)pMe->m_pEditIn)->cx;
        }
        else
        {
          ((IDIB *)pMe->m_pEditOut)->nPitch = ((IDIB *)pMe->m_pEditIn)->nPitch;
          ((IDIB *)pMe->m_pEditOut)->cx = ((IDIB *)pMe->m_pEditIn)->cx;
          ((IDIB *)pMe->m_pEditOut)->cy = ((IDIB *)pMe->m_pEditIn)->cy;
        }
      }
      else if (IBITMAP_QueryInterface(pMe->m_pEditIn, AEEIID_YCBCR, (void **)&pYCbCr) == SUCCESS)
      {
        if (pMe->m_nEditProcessedVal != IIPL_ROTATE_180)
        {
          ((IYCbCr *)pMe->m_pEditOut)->nYPitch = ((IYCbCr *)pMe->m_pEditOut)->nCPitch
                                               = (int32)(((IYCbCr *)pMe->m_pEditIn)->cy * 2);
          ((IYCbCr *)pMe->m_pEditOut)->cx = ((IYCbCr *)pMe->m_pEditIn)->cy;
          ((IYCbCr *)pMe->m_pEditOut)->cy = ((IYCbCr *)pMe->m_pEditIn)->cx;
        }
        else
        {
          ((IYCbCr *)pMe->m_pEditOut)->nYPitch = ((IYCbCr *)pMe->m_pEditOut)->nCPitch
                                               = (int32)(((IYCbCr *)pMe->m_pEditIn)->cx * 2);
          ((IYCbCr *)pMe->m_pEditOut)->cx = ((IYCbCr *)pMe->m_pEditIn)->cx;
          ((IYCbCr *)pMe->m_pEditOut)->cy = ((IYCbCr *)pMe->m_pEditIn)->cy;
        }
      }
      retVal = IIPL_Transform(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, IIPL_ROTATE,
                              (uint32)pMe->m_nEditProcessedVal);
      // Checking for the IIPL Edit Error
      if (Qalbum_DisplayError(pMe, retVal) != SUCCESS)
      {
        MSG_HIGH("Qalbum_EditPicDoRotate failed.", 0, 0, 0);
        bHandled = FALSE;
      }
      CAM_RELEASEIF(pDIB);
      CAM_RELEASEIF(pYCbCr);
    }
  }

  return bHandled;
}

/*===========================================================================
FUNCTION Qalbum_DoFlip

  DESCRIPTION
    This function flips the image vertically or horizontally
    On Up key   - flip vertically
    On Down key - flip horizontally
  PARAMETERS:
    pMe - pointer to Qalbum data struct
    key - Up/Down key ID

  RETURN VALUE:
   TRUE if flip action handled
===========================================================================*/
static boolean Qalbum_DoFlip(Qalbum *pMe, uint16 key)
{
  boolean bHandled = FALSE;
  int     retVal   = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  //reset the edit buffer
  Qalbum_UpdateInIPLImage(pMe, IDS_IMAGE_FLIP);
  if (pMe->m_pEditIn && pMe->m_pEditOut)
  {
    bHandled = TRUE;
    pMe->m_bEditProcessed = TRUE;
    if (key == (uint16)AVK_UP)
    {
      if (pMe->m_nEditProcessedVal & QCAM_IMAGE_REFLECT_X_BIT)
      {
        pMe->m_nEditProcessedVal &= ~QCAM_IMAGE_REFLECT_X_BIT;
      }
      else
      {
        pMe->m_nEditProcessedVal |= QCAM_IMAGE_REFLECT_X_BIT;
      }
    }
    else
    {
      if (pMe->m_nEditProcessedVal & QCAM_IMAGE_REFLECT_Y_BIT)
      {
        pMe->m_nEditProcessedVal &= ~QCAM_IMAGE_REFLECT_Y_BIT;
      }
      else
      {
        pMe->m_nEditProcessedVal |= QCAM_IMAGE_REFLECT_Y_BIT;
      }
    }
    if (pMe->m_nEditProcessedVal == 0)
    {
      Qalbum_EditUndo(pMe);
    }
    if (pMe->m_bEditProcessed)
    {
      if ((pMe->m_nEditProcessedVal & QCAM_IMAGE_REFLECT_X_BIT) &&
          (pMe->m_nEditProcessedVal & QCAM_IMAGE_REFLECT_Y_BIT))
      {
        retVal = IIPL_Transform(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, IIPL_ROTATE,
                                IIPL_ROTATE_180);
      }
      else if (pMe->m_nEditProcessedVal & QCAM_IMAGE_REFLECT_X_BIT)
      {
        retVal = IIPL_Transform(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, IIPL_REFLECT,
                                IIPL_REFLECT_X);
      }
      else
      {
        retVal = IIPL_Transform(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, IIPL_REFLECT,
                                IIPL_REFLECT_Y);
      }
      // Checking for the IIPL Edit Error
      if (Qalbum_DisplayError(pMe, retVal) != SUCCESS)
      {
        bHandled = FALSE;
        MSG_HIGH("Qalbum_EditPicDoFlip failed.", 0, 0, 0);
      }
    }
  }

  return bHandled;
}

/*===========================================================================
FUNCTION Qalbum_DoResize

  DESCRIPTION
    This function resizes the display image.

  PARAMETERS:
    pMe - pointer to Qalbum data struct

  RETURN VALUE:
   None
===========================================================================*/
void Qalbum_DoResize(Qalbum *pMe)
{
  if (!pMe)
    return;

  if (pMe->m_pEditIn && pMe->m_pEditOut)
  {
    AEEBitmapInfo  info     = {0};
    IDIB          *pDIB     = NULL;
    int            in_size;
    int            new_size = pMe->m_nImgSize;
    AEESize       *pSize    = QCam_GetImageDimension(new_size);
    int32          new_width;
    int32          new_height;
    int            retVal   = AEE_SUCCESS;

    if (IBITMAP_GetInfo(pMe->m_pEditIn, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      return;
    }
    in_size = Qalbum_GetImageSize((int)info.cx, (int)info.cy);

    if (new_size >= 0 && new_size != in_size)
    {
      if (info.cx > info.cy)
      {
        new_width  = pSize->cy;
        new_height = pSize->cx;
      }
      else
      {
        new_width  = pSize->cx;
        new_height = pSize->cy;
      }

      CAM_RELEASEIF(pMe->m_pEditOut);
      if (IBITMAP_CreateCompatibleBitmap(pMe->m_pEditIn, &pMe->m_pEditOut,
                                         (uint16)new_width, (uint16)new_height) != SUCCESS)
      {
        Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_MEM_FULL);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
        goto Done;
      }
      if (IBITMAP_QueryInterface(pMe->m_pEditIn, AEECLSID_DIB, (void** )&pDIB) == SUCCESS)
      {
        ((IDIB *)pMe->m_pEditOut)->nColorScheme = pDIB->nColorScheme;
      }

      retVal = IIPL_ChangeSize(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut);
      if (Qalbum_DisplayError(pMe, retVal) == AEE_SUCCESS)
      {
        pMe->m_bEditProcessed = TRUE;
      }
      else
      {
        MSG_HIGH("IIPL_ChangeSize failed.", 0, 0, 0);
      }

Done:
      CAM_RELEASEIF(pDIB);
    }
  }
}

/*===========================================================================
FUNCTION Qalbum_GetMatchFrameDir

  DESCRIPTION
    This function fill the m_sFrameDir with right frame directory based on
    selected image size. If the image size is larger than QVGA, we don't
    support it now.

  PARAMETERS:
    pMe - pointer to Qalbum data struct

  RETURN VALUE:
    TRUE - Get the frame directory with match size frames.
===========================================================================*/
static boolean Qalbum_GetMatchFrameDir(Qalbum *pMe)
{
  AEEBitmapInfo info;
  int           size;

  if (!pMe)
    return FALSE;

  if (IBITMAP_GetInfo(pMe->m_pEditIn, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
  {
    return FALSE;
  }

  size = Qalbum_GetImageSize((int)info.cx, (int)info.cy);

  (void)STRLCPY(pMe->m_sFrameDir, QCAM_FRAMES_DIR, FILE_NAME_SIZE);
  switch(size)
  {
  case QCAM_SIZE_QQVGA:
    (void)STRLCAT(pMe->m_sFrameDir, QCAM_QQVGA_SDIR, FILE_NAME_SIZE);
    break;
  case QCAM_SIZE_QCIF:
    (void)STRLCAT(pMe->m_sFrameDir, QCAM_QCIF_SDIR, FILE_NAME_SIZE);
    break;
  case QCAM_SIZE_QVGA:
    (void)STRLCAT(pMe->m_sFrameDir, QCAM_QVGA_SDIR, FILE_NAME_SIZE);
    break;
  case QCAM_SIZE_VGA:
    (void)STRLCAT(pMe->m_sFrameDir, QCAM_VGA_SDIR, FILE_NAME_SIZE);
    break;
  case QCAM_SIZE_SXGA:
    (void)STRLCAT(pMe->m_sFrameDir, QCAM_SXGA_SDIR, FILE_NAME_SIZE);
    break;
  case QCAM_SIZE_UXGA:
    (void)STRLCAT(pMe->m_sFrameDir, QCAM_UXGA_SDIR, FILE_NAME_SIZE);
    break;
  default:
    return FALSE;
  }
  return TRUE;
}

/* ==========================================================================
FUNCTION Qalbum_BuildResizeMenu
DESCRIPTION
   Builds the Resize menu.
============================================================================= */
static void Qalbum_BuildResizeMenu(Qalbum *pMe)
{
  CtlAddItem ai = {0};
  AEERect    rc = {0};
  int        i, j;

  if (!pMe)
    return;

  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen);
  // Add Items in the Menu
  QCam_InitMenu(pMe->m_pIMenu, pMe->a.m_pIDisplay, pMe->m_pResFile, &ai, IDS_IMAGE_RESIZE, rc);

  pMe->m_eActiveMenu = RESIZE_MENU;

  for (i = 0; i < ImgSizeIDList[0]; i++)
  {
    if(pMe->m_nImgSize == (int)ImgSizeDataList[i])
    {
      // only can downsize
      for (j = i; j < ImgSizeIDList[0]; j++)
      {
        QCam_AddMenuItemEx(pMe->m_pIMenu, &ai,
                           ImgSizeIDList[j + 2],
                           ImgSizeIDList[j + 2],
                           IDB_RADIO_OFF,
                           ImgSizeDataList[j]);
      }
      break;
    }
  }
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE

  QCam_SetFocusFromSel(pMe->m_pIMenu, ImgSizeIDList, (uint32)pMe->m_nImgSize, pMe->m_pResFile, IDB_RADIO_ON);

  IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
} /* END QCam_BuildSizeMenu */

/*===========================================================================
  FUNCTION: Qalbum_UpdateResizeMenu

  DESCRIPTION:
    This function updates the the resize menu radio focus and the
    m_nImgSize variable.

  PARAMETERS:
    pMe - pointer to Qalbum data struct

  DEPENDENCIES:
    NA

  RETURN VALUE
    NA

  SIDE EFFECTS:
  none
===========================================================================*/
static void Qalbum_UpdateResizeMenu(Qalbum *pMe)
{
  uint32 sel;

  if (!pMe)
    return;

  sel = (uint32)pMe->m_nImgSize;
  QCam_UpdateRadioFocus(pMe->m_pIMenu,
                        ImgSizeIDList,
                        &sel,
                        pMe->m_pResFile);
  pMe->m_nImgSize = (int16)sel;
}


#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)  
/*===========================================================================
  FUNCTION: Qalbum_UpdatePrintMenu

  DESCRIPTION:
    This function updates the the print menu radio focus and the
    m_ePrintMenuActive variable.

  PARAMETERS:
    pMe - pointer to Qalbum data struct

  DEPENDENCIES:
    NA

  RETURN VALUE
    NA

  SIDE EFFECTS:
  none
===========================================================================*/
static void Qalbum_UpdatePrintMenu(Qalbum *pMe)
{
  uint32 sel;
  uint32 data;
  uint16 newFocus;
  CtlAddItem pai;
  int numItems = 0;
  int i;

  if (!pMe || !pMe->m_pIMenu)
    return;

  MSG_HIGH("In Qalbum_UpdatePrintMenu", 0, 0, 0);

  sel = (uint32)pMe->m_ePrintMenuActive;
  numItems = IMENUCTL_GetItemCount(pMe->m_pIMenu);

  //I cannot use UpdateRadioFocus like the ResizeMenu because our list of 
  // radio buttons is variable and the value is sel might not be
  // the index on the menu

  //Get new selected item ID
  newFocus = IMENUCTL_GetSel(pMe->m_pIMenu);

  //Update selection data
  if (IMENUCTL_GetItem(pMe->m_pIMenu, newFocus, &pai))
  {
    for (i = 0; i < numItems; ++i)
    {      
      (void)IMENUCTL_GetItemData(pMe->m_pIMenu, (uint16)i, &data);      
      if (sel == data)
      {
        QCam_SetRadioImg(pMe->m_pIMenu, (uint16)i, newFocus, pMe->m_pResFile);        
      }
    }
    sel = pai.dwData;
  }
  else
  {
    MSG_HIGH("We were unable to get the newFocus item", 0, 0, 0);
  }

  //Redraw the menu
  (void)IMENUCTL_Redraw(pMe->m_pIMenu);

  pMe->m_ePrintMenuActive = (BtPrintChoice)sel;
}

#endif // defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)

/*===========================================================================
  FUNCTION: Qalbum_ExecuteColorMenu

  DESCRIPTION:
    This function updates the the pMe->m_JpegColorFormat Variable.

  PARAMETERS:
    pMe - pointer to Qalbum data struct

  DEPENDENCIES:
    NA

  RETURN VALUE
    NA

  SIDE EFFECTS:
  none
===========================================================================*/

static void Qalbum_UpdateColorFormatMenu(Qalbum * pMe)
{
  uint32 sel;

  if (!pMe)
    return;

  sel = (uint32)pMe->m_JpegColorFormat;
  QCam_UpdateRadioFocus(pMe->m_pIMenu,
                        JpegColorFormatIDList,
                        &sel,
                        pMe->m_pResFile);
  pMe->m_JpegColorFormat = (QalbumColorFormat)sel;

}

#endif //#if defined(FEATURE_APP_QALBUM)

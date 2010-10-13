/*===========================================================================

FILE: QcamCommon.c

GENERAL DESCRIPTION
    This file contains common shared functions and definitions used for
    camera UI applications.

    Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamcommon/main/latest/src/QcamCommon.c#6 $ $DateTime: 2009/04/15 00:20:08 $ $Author: saigopal $

 when      who    what, where, why
 --------  -----  -------------------------------------------------------
04/15/09   sai    Added support for USB MSD as a save location
 03/05/09  sai    fix to Divide by zero crash in Qcam_copyToDest function 
09/07/08   KC     Sleeping the UI task for 10ms for every 100 pics of deletion as
                  deletion is a very time required process and hence was throttling the system.
 08/07/08  SJ     Fixed an issue w.r.t UI dog counter to be almost zero,
                  prior to resuming of monitoring the same
 06/03/08  SJ     Fixed an issue with the reporting of the dog on 
                  QCam_GetDirPhotos & Qcam_DeleteDirs.
 03/13/08  KD     Fixed casing of MMSApp.bid for Linux builds
 10/04/07  SK     Placed some of the wide resolutions at proer place 
 09/25/07  NC     Fixed bug that failed delete all.
 09/18/07  NC     Fixed some string length issue on new function calls.
 09/14/07  KD     Removed deprecated function calls
 09/05/07  NC     Added share function QCam_SetItemImg
 05/22/07  SK     JPEG Decoder Resizing feature
 04/19/07  SK     UI Support for WVGA Display
 04/09/07  NC     Added F3 message for debug help.
 03/28/07  TV     Added Image Aspect ratios and methods to get them
 02/20/07  NC     Fixed bug in QCam_GetArgString
 01/26/07  NC     Fixed lint error with strtok.
 12/06/06  ZL     Fixed LOC_RAW/LOC_YCBCR not listed in [DELETE ALL] menu.
 11/14/06  NC     Workaround to a issue that MMC card will disable FLASH display.
 11/09/06  RT     Removed empty directory from Dir list.
 11/09/06  NC     Added QCam_HasDirPhoto to optimize search.
 09/13/06  NC     Fixed lint errors.
 08/22/06  NC     Updated QCam_GetArgString & QCam_ParseArgString for memory issue.
 06/14/06  JR     Added support for FTL memory.
 06/08/06  NC     Added new function QCam_SetRadioImg() for sharing.
 03/31/06  NC     Added helper functions QCam_GetArgString() and QCam_ParseArgString().
 01/12/06  NC     Added QCam_HasPhotos to improve the performance.
 11/30/05  NC     Clean up lint errors from Lint v8.0.
 11/08/05  NC     Update per the new header file.
 10/27/05  NC     Enabled upper case char in image file name.
 10/20/05  NC     Fixed new lint errors.
 10/17/05  NC     Added QCAM_SIZE_MAX size to QCam_ImageDimensions
 10/13/05  NC     Added 3Mp, 4Mp and 5Mp size support.
 09/15/05  NC     Lint errors clean up.
 08/25/05  NC     Added checking for code security.
 06/16/05  JN     Added new param to QCam_CopyToDest to allow rename file.
 06/09/05  NC     Added search flag to QCam_GetTotalPhotos() and Qcam_BuildDirList()
 05/23/05  JN     Centralize font definitions.
 05/20/05  NC     Added new helper function QCam_GetDirPhotos and change menu font and offset.
 05/18/05  NC     Removed obsolete function and simplify QCam_ValidateFileName().
 03/28/05  NC     Updated QCam_BuildFrameList() to build the frame list with provided dir.
                  Added bDel parm to QCam_GetTotalPhotos().
 03/09/05  NC     Added bDel parm to Qcam_BuildDirList() so we can delete the Raw dir.
 02/22/05  NC     Fixed some lint errors and updated param name in Qcam_DeleteDirs().
 02/16/05  NC     Added #ifdef FEATURE_APP_QCAMERA condition.
 02/02/05  JN     Corrected 4VGA size definition.
 01/26/05  NC     Updated image size in portrait mode.
 11/16/04  NC     Modified Qcam_DeleteDirs for more delete options.
 11/12/04  NC     Rewrite QCam_GetFilePath to avoid returning local pointer.
 11/10/04  NC     Fixed Qcam_DeleteDirs issue with MMC and bug in QCam_GetCurrentFrameIndex.
 11/04/04  JN     Fixed nagivation issue with MMC.
 11/04/04  NC     Added new function QCam_BuildSoundList and QCam_InsertItemToList
 10/29/04  NC     Updated QCam_GenerateFileName to make directory if it isn't existing.
 10/27/04  NC     Fixed a bug in QCam_BuildFrameList
 10/22/04  JN     Forgot to include DIRECTORY_STR in QCam_CopyToDest().
 10/19/04  NC     Added QCam_DeleteItemFromList for deleting a item from the file list
 10/14/04  JN     Changed QCam_BuildDirList() to pass in QCam_FileList **, rather than
                  QCam_FileList *.
 10/12/04  NC     Add a new parameter to several generic menu functions.
 09/21/04  NC     Add general menu functions.
 06/16/04  NC     Initial design of the QcamCommon file.
============================================================================*/
//Deal with lint problems in BREW headers
//lint -e740, Unusual pointer cast for BREW macros
//lint -e611, Suspicious cast for BREW macros
//lint -emacro(570,IDISPLAY_EraseRect)
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "QcamCommon.h"
#include "dog.h"
#include "rex.h"

#ifdef FEATURE_APP_QCAMERA
#include "AEEStdLib.h"
#include "AEEFile.h"
#include "QcamDef.h"
#include <string.h>

#ifdef FEATURE_APP_MMS
#error code not present
#endif

#ifdef AEE_SIMULATOR
#include "QcamSim.h"
#else
#include "msg.h"
#endif
//lint -save -e786 String concatenation within initializer
static char *albumDirs[] =
{
  QCAM_MMC_DIR,
  QCAM_PHOTOS_DIR,
  QCAM_FTL_DIR,
  QCAM_RAW_DIR,
  QCAM_YCBCR_DIR,
  QCAM_MMC_RAW_DIR,
  QCAM_FTL_RAW_DIR,
  QCAM_MMC_YCBCR_DIR
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS  
  ,QCAM_USB_DIR,
  QCAM_USB_RAW_DIR,
  QCAM_USB_YCBCR_DIR
#endif
};
//lint -restore
static AEESize QCam_ImageDimensions[QCAM_SIZE_COUNT] =
{
  { 1944, 2592},  // dummy size for QCAM_SIZE_MAX.
  { 1944, 2592},  // QCAM_SIZE_QSXGA
  { 1752, 2336},  // QCAM_SIZE_QXGAP
  { 1536, 2048},  // QCAM_SIZE_QXGA
  { 1200, 1600},  // QCAM_SIZE_UXGA
  { 1024, 1280},  // QCAM_SIZE_SXGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 960,  1280},  // QCAM_SIZE_4VGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 768,  1024},  // QCAM_SIZE_XGA
  { 600,  800 },  // QCAM_SIZE_SVGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 480,  640 },  // QCAM_SIZE_VGA
  { 288,  352 },  // QCAM_SIZE_CIF
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 240,  320 },  // QCAM_SIZE_QVGA
  { 144,  176 },  // QCAM_SIZE_QCIF
  { 120,  160 },  // QCAM_SIZE_QQVGA
  { 48,   80  }   // QCAM_SIZE_STAMP
};

static AEESize QCam_LandscapeImageDimensions[QCAM_SIZE_COUNT] =
{
  { 2592, 1944},  // dummy size for QCAM_SIZE_MAX.
  { 2592, 1944},  // QCAM_SIZE_QSXGA
  { 2336, 1752},  // QCAM_SIZE_QXGAP
  { 2048, 1536},  // QCAM_SIZE_QXGA
  { 1600, 1200},  // QCAM_SIZE_UXGA
  { 1280, 1024},  // QCAM_SIZE_SXGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 1280,  960},  // QCAM_SIZE_4VGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 1024,  768},  // QCAM_SIZE_XGA
  {  800,  600},  // QCAM_SIZE_SVGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif // FEATURE_CAMERA_WIDESIZE
  {  640,  480},  // QCAM_SIZE_VGA
  {  352,  288},  // QCAM_SIZE_CIF
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  {  320,  240},  // QCAM_SIZE_QVGA
  {  176,  144},  // QCAM_SIZE_QCIF
  {  160,  120},  // QCAM_SIZE_QQVGA
  {   80,   48}   // QCAM_SIZE_STAMP
};

static AEESize QCam_ImageRatios[QCAM_SIZE_COUNT] =
{
  {  4, 3},  // dummy size for QCAM_SIZE_MAX.
  {  4, 3},  // QCAM_SIZE_QSXGA
  {  4, 3},  // QCAM_SIZE_QXGAP
  {  4, 3},  // QCAM_SIZE_QXGA
  {  4, 3},  // QCAM_SIZE_UXGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  {  5, 4},  // QCAM_SIZE_SXGA
  {  4, 3},  // QCAM_SIZE_4VGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  {  4, 3},  // QCAM_SIZE_XGA
  {  4, 3},  // QCAM_SIZE_SVGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  {  4, 3},  // QCAM_SIZE_VGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 11, 9},  // QCAM_SIZE_CIF
  {  4, 3},  // QCAM_SIZE_QVGA
  { 11, 9},  // QCAM_SIZE_QCIF
  {  4, 3},  // QCAM_SIZE_QQVGA
  {  5, 3}   // QCAM_SIZE_STAMP
};

static AEESize QCam_LandscapeImageRatios[QCAM_SIZE_COUNT] =
{
  { 3,  4},  // dummy size for QCAM_SIZE_MAX.
  { 3,  4},  // QCAM_SIZE_QSXGA
  { 3,  4},  // QCAM_SIZE_QXGAP
  { 3,  4},  // QCAM_SIZE_QXGA
  { 3,  4},  // QCAM_SIZE_UXGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 4,  5},  // QCAM_SIZE_SXGA
  { 3,  4},  // QCAM_SIZE_4VGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 3,  4},  // QCAM_SIZE_XGA
  { 3,  4},  // QCAM_SIZE_SVGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 3,  4},  // QCAM_SIZE_VGA
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  { 9, 11},  // QCAM_SIZE_CIF
  { 3,  4},  // QCAM_SIZE_QVGA
  { 9, 11},  // QCAM_SIZE_QCIF
  { 3,  4},  // QCAM_SIZE_QQVGA
  { 3,  5}   // QCAM_SIZE_STAMP
};

/*===========================================================================
FUNCTION QCam_ReleaseIF

  DESCRIPTION
    This function releases the pointer and set it to NULL if it is not NULL.

  PARAMETERS:
    ppif - Pointer to (IBase *), replaced by any BREW interface pointer

  RETURN VALUE:
   None
===========================================================================*/
void QCam_ReleaseIF(IBase ** ppif)
{
  if (ppif && *ppif)
  {
    (void)IBASE_Release(*ppif);
  }
  if (ppif)
    *ppif = NULL;
}

/*===========================================================================
FUNCTION QCam_FreeIF

  DESCRIPTION
    This function free the pointer and set it to NULL if it is not NULL.

  PARAMETERS:
    pif - Pointer to void

  RETURN VALUE:
   None
===========================================================================*/
void QCam_FreeIF(void** ppif)
{
  if (ppif)
  {
    FREEIF(*ppif);
    *ppif = NULL;
  }
}

/*===========================================================================
FUNCTION QCam_GetImageDimension

  DESCRIPTION
    This is a wrap function to look up an image dimension.

  PARAMETERS:
    size - image size index

  RETURN VALUE:
   Image dimension
===========================================================================*/
AEESize * QCam_GetImageDimension(int size)
{
  return &QCam_ImageDimensions[size];
}

/*===========================================================================
FUNCTION QCam_GetLandscapeImageDimension

  DESCRIPTION
    This is a wrap function to look up a portrait image dimension.

  PARAMETERS:
    size - image size index

  RETURN VALUE:
   Image dimension
===========================================================================*/
AEESize * QCam_GetLandscapeImageDimension(int size)
{
  return &QCam_LandscapeImageDimensions[size];
}


/*===========================================================================
FUNCTION QCam_GetImageRatio

  DESCRIPTION
    This is a wrap function to look up an image ratio.

  PARAMETERS:
    size - image size index

  RETURN VALUE:
   Image ratio
===========================================================================*/
AEESize * QCam_GetImageRatio(int size)
{
  return &QCam_ImageRatios[size];
}

/*===========================================================================
FUNCTION QCam_GetLandscapeImageRatio

  DESCRIPTION
    This is a wrap function to look up a portrait image ratio.

  PARAMETERS:
    size - image size index

  RETURN VALUE:
   Image ratio
===========================================================================*/
AEESize * QCam_GetLandscapeImageRatio(int size)
{
  return &QCam_LandscapeImageRatios[size];
}

/*===========================================================================
  FUNCTION: QCam_CheckProperties

  DESCRIPTION:
    This function check if the menu item has the selected property.

  PARAMETERS:
    propertyFlags:  All Properties
    propertiesMask: Properties in mask for checking

  DEPENDENCIES:
    NA

  RETURN VALUE
    Returns TRUE if all of the properties specified in propertiesMask are set.
    Otherwise, returns FALSE

  SIDE EFFECTS:
  none
===========================================================================*/
boolean QCam_CheckProperties(uint32 propertyFlags, uint32 propertiesMask)
{
  return(boolean)((propertyFlags & propertiesMask) == propertiesMask);
}

/* ==========================================================================
FUNCTION QCam_InvertProperties

  DESCRIPTION
    Inverts the bits of all of the properties specified in propertiesMask.

  RETURN VALUE
    Returns TRUE if, after the inversion, all of the properties specified in
    propertiesMask are set.
    Otherwise, returns FALSE.
============================================================================= */
boolean QCam_InvertProperties(uint32 *propertyFlags, uint32 propertiesMask)
{
  if (!propertyFlags)
    return FALSE;

  *propertyFlags ^= propertiesMask;
  return(boolean)((*propertyFlags & propertiesMask) == propertiesMask);
} /* END QCam_InvertProperties */

/*===========================================================================
FUNCTION QCam_DeleteFileList

  DESCRIPTION
    This function deletes the file list.

  PARAMETERS:
    first - head pointer to the QCam_FileList object

  RETURN VALUE:
    NONE
===========================================================================*/
void QCam_DeleteFileList(QCam_FileList **first)
{
  QCam_FileList *cur;
  QCam_FileList *next;

  if (!first)
   return;

  cur = *first;
  while (cur)
  {
    next = cur->next;
    FREEIF(cur);
    cur = next;
  }
  // set the list to empty
  if (*first)
    *first = NULL;
} /* END QCam_DeleteFileList */

/*===========================================================================
FUNCTION QCam_DeleteItemFromList

  DESCRIPTION
    This function deletes an item from the file list.
    If the first item or only one item in the list is deleted, the pointer
    to the head of the file list is updated.

  PARAMETERS:
    first - head pointer to the QCam_FileList object
    item  - item index (item is one base)

  RETURN VALUE:
    NONE
===========================================================================*/
void QCam_DeleteItemFromList(QCam_FileList **first, int item)
{
  QCam_FileList *cur = NULL;
  QCam_FileList *next = NULL;
  QCam_FileList *prev = NULL;

  int index = 1;

  if (!first)
    return;

  cur = *first;
  while (cur)
  {
    //Get the next item and previous item of the current item
    next = cur->next;
    prev = cur->prev;
    if (index == item)
    {
      //Rebuild the link to skip the current item
      if (prev)
      {
        //If prev item is not NULL, points its next to the cur->next
        prev->next = next;
      }
      if (next)
      {
        //If next is not NULL, points the prev item of next to cur->prev
        next->prev = prev;
      }
      //No link to cur item, free it.
      FREEIF(cur);
      break;
    }
    //Not this item, go to next one
    cur = cur->next;
    index++;
  }

  // Update *first if the first or only item is deleted
  if (index == 1)
    *first = next;
} /* END QCam_DeleteItemFromList */

/*===========================================================================
FUNCTION QCam_InsertItemToList

  DESCRIPTION
    This function insert an item to the file list after provided item number.

  PARAMETERS:
    fileName  - file name for the new added file node
    first - head pointer to the QCam_FileList object
    item  - item index (item is one base)

  RETURN VALUE:
    TRUE - create and insert a new item successfully
    FALSE - failed to create a new item.

  SIDE EFFECT
    A new item is malloc here and the memory needs to be free outside later.
===========================================================================*/
boolean QCam_InsertItemToList(const char *fileName, QCam_FileList **first, int item)
{
  int count = 0;
  QCam_FileList **cur     = first;
  QCam_FileList  *prev    = NULL;
  QCam_FileList  *newItem = NULL;

  if (!cur || !fileName)
    return FALSE;

  while (*cur != NULL && ++count <= item)
  {
    prev = *cur;
    cur = &(*cur)->next;
  }

  newItem = (QCam_FileList *)MALLOC(sizeof(QCam_FileList));
  if (newItem)
  {
    (void)STRLCPY(newItem->fileName, fileName, sizeof(newItem->fileName));
    newItem->next = *cur;
    newItem->prev = (*cur ? (*cur)->prev : prev);
    if (*cur)
    {
      (*cur)->prev = newItem;
    }
    *cur = newItem;
  }
  return(boolean)(newItem != NULL);
}

/*===========================================================================
FUNCTION QCam_InsertSortFileList

  DESCRIPTION
    This function inserts a new file node to the file list in specified sort order.

  PARAMETERS:
    fileName  - file name for the new added file node
    first     - head pointer to the QCam_FileList object
    ascending - sort order in asscending

  RETURN VALUE:
    TRUE if the new item is added

  SIDE EFFECT
    A new item is malloc here and the memory needs to be free outside later.
===========================================================================*/
boolean QCam_InsertSortFileList(const char *fileName, QCam_FileList **first, boolean ascending)
{
  QCam_FileList **cur     = first;
  QCam_FileList  *prev    = NULL;
  QCam_FileList  *newItem = NULL;

  if (!cur || !fileName)
    return FALSE;

  if (ascending)
  {
    while (*cur != NULL && STRCMP(fileName, (*cur)->fileName) > 0)
    {
      prev = *cur;
      cur = &(*cur)->next;
    }
  }
  else
  {
    while (*cur != NULL && STRCMP(fileName, (*cur)->fileName) < 0)
    {
      prev = *cur;
      cur = &(*cur)->next;
    }
  }
  newItem = (QCam_FileList *)MALLOC(sizeof(QCam_FileList));
  if (newItem)
  {
    (void)STRLCPY(newItem->fileName, fileName, sizeof(newItem->fileName));
    newItem->next = *cur;
    newItem->prev = (*cur ? (*cur)->prev : prev);
    if (*cur)
    {
      (*cur)->prev = newItem;
    }
    *cur = newItem;
  }
  return(boolean)(newItem != NULL);
} /* END QCam_InsertSortFileList */

/*===========================================================================
FUNCTION QCam_GetCurrentItem

  DESCRIPTION
    This function gets the current item from the list by index.

  PARAMETERS:
    first - head pointer to the QCam_FileList list
    cur   - pointer to the current returned item
    idx   - index of the current item (idx is one base)

  RETURN VALUE:
    TRUE if current item is not NULL
===========================================================================*/
boolean QCam_GetCurrentItem(QCam_FileList *first, QCam_FileList **cur, int16 idx)
{
  int dir = 0;
  QCam_FileList *pCur = first;

  if (!cur)
    return FALSE;

  while (pCur != NULL && ++dir < idx)
  {
    pCur = pCur->next;
  }
  *cur = pCur;
  if (!pCur)
  {
    MSG_MED("QCam_GetCurrentItem - no image item.", 0, 0, 0);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION QCam_BuildDirList

  DESCRIPTION
    This function builds the dir list.

  PARAMETERS:
    pIFileMgr - pointer to IFileMgr
    pDirList  - head pointer to the dir list
    ascending - sort order in asscending
    bDel      - Build the DirList for delete all
    flag      - location flag to build the directories.

  RETURN VALUE:
    total dir numbers in the dir list
===========================================================================*/
int16 QCam_BuildDirList(IFileMgr *pIFileMgr, QCam_FileList **ppDirList,
                         boolean ascending, boolean bDel, SearchFlag flag)
{
  FileInfo fi;
  int16 wCount = 0;
  boolean bMMC = FALSE;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  boolean bUSB = FALSE;
#endif
  boolean bFTL = FALSE;
  int i;
  int nCount = 1;
  QCam_FileList *dir = NULL;

  if (!pIFileMgr || !ppDirList)
    return 0;

  //clean up the old list if it is not null
  QCam_DeleteFileList(ppDirList);

  for (i = 0; i < (int)ARR_SIZE(albumDirs); i++)
  {
    //Skip the RAW and YCBCR directory because album can not display those images.
    if (!bDel || (flag == LOC_ALL_SHOW))
    {
      if (STRSTR(albumDirs[i], QCAM_RAW_SDIR) ||
          STRSTR(albumDirs[i], QCAM_YCBCR_SDIR))
      {
        continue;
      }
    }
    //Skip the unrelated directories based on the search flag(keep flag).
    bMMC = (STRNCMP(albumDirs[i], QCAM_MMC_CONSTANT, QCAM_MMC_CONST_LEN) == 0);
    if (bMMC)
    {
      if ((flag == LOC_FLASH) || (flag == LOC_FTL) 
        #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS 
        || (flag == LOC_USB) 
        #endif
     )
        continue;
    }
    else
    {
      if (flag == LOC_MMC)
        continue;
    }

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS	
    bUSB = (STRNCMP(albumDirs[i], QCAM_USB_CONSTANT, QCAM_USB_CONST_LEN) == 0);
    if (bUSB)
    {
      if ((flag == LOC_FLASH) || (flag == LOC_FTL) || (flag == LOC_MMC))
        continue;
    }
    else
    {
      if (flag == LOC_USB)
        continue;
    }
#endif
    bFTL = (STRNCMP(albumDirs[i], QCAM_FTL_CONSTANT, QCAM_FTL_CONST_LEN) == 0);
    if (bFTL)
    {
      if ((flag == LOC_FLASH) || (flag == LOC_MMC) 
        #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS 
          || (flag == LOC_USB) 
        #endif 
      )
        continue;
    }
    else
    {
      if (flag == LOC_FTL)
        continue;
    }

    if ((flag == LOC_RAW) && !STRSTR(albumDirs[i], QCAM_RAW_SDIR))
    {
      continue;
    }

    if ((flag == LOC_YCBCR) && !STRSTR(albumDirs[i], QCAM_YCBCR_SDIR))
    {
      continue;
    }

    //Build dir list on selected directory
    if (IFILEMGR_EnumInit(pIFileMgr, albumDirs[i], TRUE) != SUCCESS)
    {
      //If it fails to Enum this directory, give error message and continue.
      MSG_HIGH("IFILEMGR_EnumInit failed in directory index %d", i, 0, 0);
      continue;
    }
    while (IFILEMGR_EnumNext(pIFileMgr, &fi))
    {
      if (STRLEN(fi.szName) < FILE_NAME_SIZE)
      {
        if(!QCam_InsertSortFileList(fi.szName, ppDirList, ascending))
          break;
        wCount++;
      }
    }
  }

  dir = *ppDirList;
  //remove empty directories from the Dir list.
  while (dir)
  {
    if(!QCam_HasDirPhoto(pIFileMgr, dir->fileName))
    {
      dir = dir->prev;
      QCam_DeleteItemFromList(ppDirList ,nCount);
      wCount--;
      nCount--;
    }
    if (!dir)
      dir = *ppDirList;
    else
      dir = dir->next;
    nCount++;
  }

  return wCount;
}

/*===========================================================================
FUNCTION QCam_BuildPhotoList

  DESCRIPTION
    This function builds the photo list for all directories.

  PARAMETERS:
    pIFileMgr - pointer to IFileMgr
    curDir    - start dir pointer
    photoList - head pointer to the photo list
    ascending - sort order in asscending

  RETURN VALUE:
    total file count in the photo list
===========================================================================*/
int16 QCam_BuildPhotoList(IFileMgr *pIFileMgr, QCam_FileList *curDir, QCam_FileList **photoList, boolean ascending)
{
  FileInfo fi;
  int16 wCount = 0;
  boolean result = FALSE;

  if (!pIFileMgr || !curDir || !photoList)
    return 0;

  //clean up the old list if it is not null
  QCam_DeleteFileList(photoList);

  if (IFILEMGR_EnumInit(pIFileMgr, curDir->fileName, FALSE) != SUCCESS)
  {
    //If it fails to Enum this directory, give error message and return.
    MSG_MED("IFILEMGR_EnumInit failed in direcotry %s", curDir->fileName, 0, 0);
    return 0;
  }

  do
  {
    result = IFILEMGR_EnumNext(pIFileMgr, &fi);
    if (result && STRLEN(fi.szName) < FILE_NAME_SIZE)
    {
      if(!QCam_InsertSortFileList(fi.szName, photoList, ascending))
        break;

      wCount++;
    }
  } while (result);

  return wCount;
}

/*=====================================================================
FUNCTION QCam_HasDirPhoto

  DESCRIPTION: This function check if the given directory has file.

  PARAMETERS:
    pIFileMgr - Pointer to IFileMgr
    pDirName - Pointer to the root directory name.

  RETURN VALUE:
    boolean
======================================================================*/
boolean QCam_HasDirPhoto(IFileMgr *pIFileMgr, const char *pDirName)
{
  FileInfo fi;

  if ((!pIFileMgr) || (!pDirName))
    return 0;

  if (IFILEMGR_EnumInit(pIFileMgr, pDirName, FALSE) != SUCCESS)
  {
    //If it fails to Enum this directory, give error message and continue.
    MSG_HIGH("IFILEMGR_EnumInit failed in direcotry %s", pDirName, 0, 0);
    return FALSE;
  }
  else if (!IFILEMGR_EnumNext(pIFileMgr, &fi))
  {
    MSG_MED("No photo in directory %s", pDirName, 0, 0);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION QCam_GetDirPhotos

  DESCRIPTION
    This function gets the total photos in the specified directory.

  PARAMETERS:
    pIFileMgr - Pointer to IFileMgr
    pDirName - Pointer to the root directory name.

  RETURN VALUE:
    total photo numbers in specified directory
===========================================================================*/
uint32 QCam_GetDirPhotos(IFileMgr *pIFileMgr, const char *pDirName)
{
  FileInfo fi;
  uint32 totalPhotos = 0;
  unsigned short int count_sleep =0;
  /*We are perfoming a lengthier loop very below*/
  /*for the reason that it might be taking a while to execute the loop*/
  /*when the no: of images are very high, say more than 1000*/
  /*So  stop monitoring dog now from UI prespective now*/
  dog_monitor_pause(DOG_UI_RPT);
  if ((!pIFileMgr) || (!pDirName))
    return 0;

  if (IFILEMGR_EnumInit(pIFileMgr, pDirName, FALSE) != SUCCESS)
  {
    //If it fails to Enum this directory, give error message and continue.
    MSG_HIGH("IFILEMGR_EnumInit failed in direcotry %s", pDirName, 0, 0);
  }
  else
  {
    while(IFILEMGR_EnumNext(pIFileMgr, &fi))
    {
      totalPhotos++;
        /*Have a seperate counter to check whether no: of images has reached 400*/
        /*if so, give a breathing time  (i.e. rex_sleep) */
        /*for the low priority task to kick the dog*/
        /*& then reset the counter and repeat this action for the next bunch of 400 images*/
        /*Pls note that 400 images & 50 ms are very tentative & this has been fixed in a random experiment*/
        /*proabably more experiments might increase no: of images & sleeping time to be lower*/
        /*& if we can do that, it will better the system performance*/
        count_sleep++;
        if (count_sleep == 100)
        {
            rex_sleep(10);
            count_sleep = 0;
        }
    }
  }
  /*Kick the dog just before it resumes*/
  /*This will cover the cases where in the dog counter w.r.t. UI is almost zero*/
  /*prior to resuming*/
  dog_report(DOG_UI_RPT);
  /*resume the dog monitoring process back*/
  dog_monitor_resume(DOG_UI_RPT);
  return totalPhotos;
}

/*===========================================================================
FUNCTION QCam_GetTotalPhotos

  DESCRIPTION
    This function gets the total photos.

  PARAMETERS:
    pIFileMgr - pointer to IFileMgr
    bDel - flag to tell if get the photo number for delete.
    flag - location flag to search the files.

  RETURN VALUE:
    total photo numbers in all photo directories
===========================================================================*/
uint32 QCam_GetTotalPhotos(IFileMgr *pIFileMgr, boolean bDel, SearchFlag flag)
{
  QCam_FileList *dir;
  QCam_FileList *DirList = NULL;
  uint32 totalPhotos = 0;

  if (!pIFileMgr)
    return 0;

  //build dir list first if it is not existing
  if (QCam_BuildDirList(pIFileMgr, &DirList, TRUE, bDel, flag) == 0)
    return 0;

  if (!DirList)
    return 0;

  dir = DirList;
  //loop through all directories to count photo files
  while (dir)
  {
    totalPhotos += QCam_GetDirPhotos(pIFileMgr, dir->fileName);
    dir = dir->next;
  }

  QCam_DeleteFileList(&DirList);

  return totalPhotos;
}


/*=====================================================================
FUNCTION QCam_HasPhotos

  DESCRIPTION:
    This function checks if the searched directories have photos.

  PARAMETERS:
    pIFileMgr - pointer to IFileMgr
    flag - location flag to search the files.

  RETURN VALUE:
    TRUE if at least a photo is found
======================================================================*/
boolean QCam_HasPhotos(IFileMgr *pIFileMgr, SearchFlag flag)
{
  boolean bDel = FALSE;
  QCam_FileList *DirList = NULL;
  int16 nDir = 0;

  if (!pIFileMgr)
    return FALSE;

  if ((flag == LOC_ALL) || (flag == LOC_RAW) || (flag == LOC_YCBCR))
  {
    bDel = TRUE;
  }

  nDir = QCam_BuildDirList(pIFileMgr, &DirList, TRUE, bDel, flag);
  QCam_DeleteFileList(&DirList);

  if (nDir == 0)
  {
    MSG_MED("No photo found", 0, 0, 0);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION QCam_BuildFrameList

  DESCRIPTION
    This function builds the frame list.

  PARAMETERS:
    pIFileMgr - [in] pointer to IFileMgr
    pDir      - [in] pointer to Frame directory
    pFrameList - [in, out]head pointer to the frame list

  RETURN VALUE:
    total file count in the frame list
===========================================================================*/
int16 QCam_BuildFrameList(IFileMgr *pIFileMgr, const char *pDir, QCam_FileList **pFrameList)
{
  FileInfo fi;
  int16 wCount = 0;
  boolean result = FALSE;

  if (!pIFileMgr || !pDir || !pFrameList)
    return 0;

  //clean up the old list if it is not null
  QCam_DeleteFileList(pFrameList);

  if (IFILEMGR_EnumInit(pIFileMgr, pDir, FALSE) != SUCCESS)
  {
    //If it fails to Enum this directory, give error message and return.
    MSG_HIGH("IFILEMGR_EnumInit failed in directory %s", pDir, 0, 0);
    return 0;
  }

  do
  {
    result = IFILEMGR_EnumNext(pIFileMgr, &fi);
    if (result)
    {
      if(!QCam_InsertSortFileList(fi.szName, pFrameList, TRUE))
        break;

      wCount++;
    }
  } while (result);

  return wCount;
}

/*===========================================================================
FUNCTION QCam_GetCurrentFrameIndex

  DESCRIPTION
    This function gets the current frame number.

  PARAMETERS:
    pFrameList - start address of the frame list
    sFrameName - current frame name

  RETURN VALUE:
    current frame index
===========================================================================*/
int16 QCam_GetCurrentFrameIndex(QCam_FileList *pFrameList, const char *sFrameName)
{
  QCam_FileList *curr = pFrameList;
  int16 nCount = 1; // start with one to include "No Frame"

  if (!sFrameName)
    return 1;

  while(curr)
  {
    nCount++;
    if (STRCMP(curr->fileName, sFrameName) == 0)
      break;
    curr = curr->next;
  }

  //reset curr if no match found.
  if (!curr)
    nCount = 1;

  return nCount;
}

/*===========================================================================
FUNCTION QCam_BuildSoundList

  DESCRIPTION
    This function builds the shutter sound list.

  PARAMETERS:
    pIFileMgr - pointer to IFileMgr
    pSoundList - head pointer to the sound list

  RETURN VALUE:
    total file count in the frame list
===========================================================================*/
uint16 QCam_BuildSoundList(IFileMgr *pIFileMgr, QCam_FileList **pSoundList)
{
  FileInfo fi;
  uint16 wCount = 0;
  boolean result;

  if (!pIFileMgr || !pSoundList)
    return 0;

  //clean up the old list if it is not null
  QCam_DeleteFileList(pSoundList);

  if (IFILEMGR_EnumInit(pIFileMgr, QCAM_SOUNDS_DIR, FALSE) != SUCCESS)
  {
    //If it fails to Enum this directory, give error message and continue.
    MSG_HIGH("IFILEMGR_EnumInit failed in direcotry %s", QCAM_SOUNDS_DIR, 0, 0);
  }
  else
  {
    //Add the sound file name to the list
    do
    {
      result = IFILEMGR_EnumNext(pIFileMgr, &fi);
      if (result)
      {
        if(!QCam_InsertSortFileList(fi.szName, pSoundList, TRUE))
          break;

        wCount++;
      }
    } while (result);
  }

  //Append item "No Sound" to the list.
  if (QCam_InsertItemToList(QCAM_SHUTTER_FILE, pSoundList, wCount))
    wCount++;

  return wCount;
}

/*===========================================================================
FUNCTION QCam_UpdateCurrentTime

  DESCRIPTION
    This function update the provided day, month and year with the current
    julian time.

  PARAMETERS:
    day - address of the day to be updated
    month - address of the month to be updated
    year - address of the year to be updated

  RETURN VALUE:
    None
===========================================================================*/
void QCam_UpdateCurrentTime(uint16 *day, uint16 *month, uint16 *year)
{
  JulianType t;

  GETJULIANDATE(0, &t);
  if (day)
    *day = t.wDay;
  if (month)
    *month = t.wMonth;
  if (year)
    *year = t.wYear;
}

/*===========================================================================
FUNCTION QCam_GetFileNameExtension

  DESCRIPTION
    This function gets the extension from given file name.

  PARAMETERS:
    pszFileName - file name with extension

  RETURN VALUE:
    extension pointer to the given file name
===========================================================================*/
char * QCam_GetFileNameExtension(const char *pszFileName)
{
  char *pszExtName;

  if (!pszFileName)
    return NULL;

  pszExtName = STRRCHR(pszFileName, (int)DOT_CHAR);
  if (!pszExtName)
    return NULL;

  return pszExtName;
}

/*===========================================================================
FUNCTION QCam_GetFilePath

  DESCRIPTION
    This function gets the path from given file name.

  PARAMETERS:
    pszFileName - full file name with path
    pDir        - buffer to hold the directory
    nLen        - Max. buffer size

  RETURN VALUE:
    TREU if the path is copy to the pDir buffer.
===========================================================================*/
boolean QCam_GetFilePath(const char * pszFileName, char *pDir, int nLen)
{
  char dirName[FILE_NAME_SIZE] = {0};
  char *slash = NULL;

  if (!pszFileName || !pDir)
    return FALSE;

  slash = STRRCHR(pszFileName, DIRECTORY_CHAR);
  if (!slash)
    return FALSE;

  //The file name with last "/" is excluded
  if (STRLCPY(dirName, pszFileName, (size_t)(slash + 1 - pszFileName)) >= FILE_NAME_SIZE)
    return FALSE;

  if (STRLCPY(pDir, dirName, nLen) >= (size_t)nLen)
    return FALSE;

  return TRUE;
}

/*===========================================================================
FUNCTION QCam_GetFileShortName

  DESCRIPTION
    This function gets the file name from given full name.

  PARAMETERS:
    pszFileName - full file name with path

  RETURN VALUE:
    pointer to the file name of given full name.
===========================================================================*/
const char * QCam_GetFileShortName(const char * pszFileName)
{
  char *pszName;

  if (!pszFileName)
    return NULL;

  pszName = STRRCHR(pszFileName, DIRECTORY_CHAR);
  if (!pszName)
    pszName = (char *)pszFileName;
  else
    pszName++;

  return (const char *)pszName;
}
/*===========================================================================
FUNCTION QCam_GetValidFileName

  DESCRIPTION
    This function gets the file name from the text control entry and creates
    a full valid file name.

  PARAMETERS:
    textCtl - ITextCtl interface pointer
    pFile - file name pointer to the old file name and return the new file name
    dirLen - directory length

  RETURN VALUE:
    TRUE if new file created
===========================================================================*/
boolean QCam_GetValidFileName(ITextCtl *textCtl, char *pFile, int dirLen)
{
  char fileName[FILE_NAME_SIZE] = {0};
  AECHAR szText[FILE_NAME_SIZE];
  boolean bFileNameValid = FALSE;
  char *ext;

  if (!textCtl || !pFile)
    return FALSE;

  ext = QCam_GetFileNameExtension(pFile);
  if (STRLCPY(fileName, pFile, sizeof(fileName)) >= FILE_NAME_SIZE)
    return FALSE;

  (void)ITEXTCTL_GetText(textCtl, szText, sizeof(szText));

  //Check if input file name is too long. Compare in length of char.
  //Subtract path length and 4 bytes for ext, 1 byte for null terminator.
  if ((WSTRLEN(szText) / 2) > ((FILE_NAME_SIZE - dirLen) - 5))
    return FALSE;

  bFileNameValid = QCam_ValidateFileName(szText);
  if (bFileNameValid)
  {
    (void)WSTRTOSTR(szText, &fileName[dirLen], (FILE_NAME_SIZE - dirLen));

    // Add in the extension
    if (STRLCAT(fileName, ext, sizeof(fileName)) >= FILE_NAME_SIZE)
      return FALSE;

    if (STRLCPY(pFile, fileName, FILE_NAME_SIZE) >= FILE_NAME_SIZE)
      return FALSE;
  }

  return bFileNameValid;
}

/*===========================================================================
FUNCTION QCam_ValidateFileName

  DESCRIPTION
    This function checks if the file name inside lengh and
    if it includes invalid character.

  PARAMETERS:
    fileName - file name to check

  RETURN VALUE:
    TRUE if file name is valid. FALSE otherwise
===========================================================================*/
boolean QCam_ValidateFileName(const AECHAR *fileName)
{
  int index = 0;

  if (!fileName)
    return FALSE;

  while (fileName[index] != 0)
  {
    //return false if any invalid character found
    if ((fileName[index] == (AECHAR)'\\') ||
        (fileName[index] == (AECHAR)'/')  ||
        (fileName[index] == (AECHAR)':')  ||
        (fileName[index] == (AECHAR)'?')  ||
        (fileName[index] == (AECHAR)'"')  ||
        (fileName[index] == (AECHAR)'<')  ||
        (fileName[index] == (AECHAR)'>')  ||
        (fileName[index] == (AECHAR)'|'))
    {
       return FALSE;
    }
    index++;
  }

  //return true if file name is valid
  return TRUE;
} /* END QCam_ValidateFileName */

/*===========================================================================
FUNCTION QCam_GenerateFileName

  DESCRIPTION
    This function creates a new file name based on the provided directory and
    extension. If the directory is not existing, creat it first.
    It appends a date folder to the root directory and increase the
    current file number to append to the default name.
    The new file name is like: "f:/~/qcam/img/2004-07-27/img001.jpg"

  PARAMETERS:
    pMe - pointer to QCam data struct
    pDir - file directory
    pExt - file extension
    cur - pointer to current file number. The number will be updated.
    strName - pointer to the file name buffer.

  RETURN VALUE:
    NA
===========================================================================*/
void QCam_GenerateFileName(IFileMgr *pIFileMgr, const char *pDir, const char *pExt, uint16 *cur, char* strName)
{
  char dirName[FILE_NAME_SIZE] = {0};
  char fileName[FILE_NAME_SIZE] = {0};
  JulianType t;
  uint16 count = 0;

  if (!pIFileMgr || !pDir || !cur || !pExt || !strName)
    return;

  count = *cur;
  GETJULIANDATE(0, &t);

  //Append date folder to directory
  (void)SNPRINTF(dirName, sizeof(dirName), "%s" DIRECTORY_STR "%04u-%02u-%02u", 
                 pDir, t.wYear, t.wMonth, t.wDay);

  //if it is a new date folder, reset the count number
  if (SUCCESS != IFILEMGR_Test(pIFileMgr, dirName))
  {
    count = 0;
    if (IFILEMGR_MkDir(pIFileMgr, dirName) != SUCCESS)
    {
      ZEROAT(strName);
      return;
    }
  }
  //Create the file name and make sure it is not already existing.
  do
  {
    (void)SNPRINTF(fileName, sizeof(fileName), "%s" DIRECTORY_STR "img%03u%s",
                   dirName, ++count, pExt);
  }while (SUCCESS==IFILEMGR_Test(pIFileMgr, fileName));

  *cur = count;
  (void)STRLCPY(strName, fileName, FILE_NAME_SIZE);
}

/*===========================================================================
FUNCTION QCam_DeleteFile

  DESCRIPTION
    This function deletes the selected file.

  PARAMETERS:
    pIFileMgr - Pointer to IFileMgr interface
    pFile     - file to delete
    bRmDir    - flag for removing the directory after deleting the file

  RETURN VALUE:
    TRUE if file deleted
===========================================================================*/
boolean QCam_DeleteFile(IFileMgr *pIFileMgr, const char *pFile, boolean bRmDir)
{
  if (!pIFileMgr || !pFile)
    return FALSE;

  if (IFILEMGR_Remove(pIFileMgr, pFile) != SUCCESS)
    return FALSE;

  if (bRmDir)
  {
    char pDir[FILE_NAME_SIZE] = {0};
    if (QCam_GetFilePath(pFile, pDir, FILE_NAME_SIZE))
    {
      (void)IFILEMGR_RmDir(pIFileMgr, pDir);
    }
  }

  return TRUE;
}

/*===========================================================================
FUNCTION Qcam_DeleteDirs

  DESCRIPTION
    This function deletes all photos and their directories.

  PARAMETERS:
    pIFileMgr - Pointer to IFileMgr interface
    flag - location flag to build the directories for deleting.

  RETURN VALUE:
    TRUE if all files are deleted.
==========================================================================*/
boolean Qcam_DeleteDirs(IFileMgr * pIFileMgr, SearchFlag flag)
{
  QCam_FileList *pDirList = NULL;
  QCam_FileList *pDir = NULL;
  FileInfo fi;
  int16 count;
  boolean result = TRUE;
  char* fileName = NULL;
  uint16   i;
  unsigned short int count_sleep =0;

  if (!pIFileMgr)
    return FALSE;
  /*We are perfoming a lengthier loop very below*/
  /*for the reason that it might be taking a while to execute the loop*/
  /*when the no: of images are very high, say more than 1000*/
  /*So  stop monitoring dog now from UI angle now*/
  dog_monitor_pause(DOG_UI_RPT);

  count = QCam_BuildDirList(pIFileMgr, &pDirList, TRUE, TRUE, flag);
  if (count > 0)
  {
    fileName = (char*) MALLOC(FILE_NAME_SIZE);
    for (i = 1; i <= count; i++)
    {
      if (QCam_GetCurrentItem(pDirList, &pDir, (int16)i))
      {
        if (IFILEMGR_EnumInit(pIFileMgr, pDir->fileName, FALSE) != SUCCESS)
        {
          //If it fails to Enum this directory, give error message and continue.
          MSG_HIGH("IFILEMGR_EnumInit failed in direcotry %s", pDir->fileName, 0, 0);
        }
        else
        {
          
          while(IFILEMGR_EnumNext(pIFileMgr, &fi))
          {
            /*Have a seperate counter to check whether no: of images has reached 400*/
            /*if so, give a breathing time  (i.e. rex_sleep) */
            /*for the low priority task to kick the dog*/
            /*& then reset the counter and repeat this action for the next bunch of 400 images*/
            /*Pls note that 400 images & 50 ms are very tentative & this has been fixed in a random experiment*/
            /*proabably more experiments might increase no: of images & sleeping time to be lower*/
            /*& if we can do that, it will better the system performance*/
            count_sleep++;
            if (count_sleep == 100)
            {
                rex_sleep(10);
                count_sleep = 0;
            }
            ZEROAT(fileName);
            (void)STRLCPY(fileName, fi.szName, FILE_NAME_SIZE);
            //Some file may be not deleted, just go ahead.
            if (IFILEMGR_Remove(pIFileMgr, fileName) != SUCCESS)
            {
              result = FALSE;
            }
          }
          
        }

        //Remove the directory if all files inside are deleted.
        if (result)
        {
          (void)IFILEMGR_RmDir(pIFileMgr, pDir->fileName);
        }
      }
    }
  }
  /*Kick the dog just before it resumes*/
  /*This will cover the cases where in the dog counter w.r.t. UI is almost zero*/
  /*prior to resuming*/
  dog_report(DOG_UI_RPT);
  /*Now we are done with bigger loop*/
  /*Kindly resume monitoring of dog from UI prespective*/
  dog_monitor_resume(DOG_UI_RPT);

  //Clean up
  QCam_FreeIF((void **)&fileName);
  QCam_DeleteFileList(&pDirList);
  return result;
}

/*===========================================================================
FUNCTION QCam_CopyToDest

  DESCRIPTION
    This function copies the selected picture to the seleted
    directory.

  PARAMETERS:
    pIFileMgr - Pointer to IFileMgr interface
    pFile     - Source file to copy
    pDest     - Destination directory

  RETURN VALUE:
    TRUE if file is copied
===========================================================================*/
boolean QCam_CopyToDest(IFileMgr *pIFileMgr, const char *pFile,
                        const char *pDestPath, const char *pDestFile)
{
  // Get the filename
  char szDest[FILE_NAME_SIZE];
  IFile * pDest, * pSrc;
  uint32 read, remaining, written;
  uint8 * pBuf;
  FileInfo info;

  if (!pIFileMgr || !pFile || !pDestPath)
    return FALSE;

  // Copy the common directory name into the destination buffer
  if (STRLCPY(szDest, pDestPath, sizeof(szDest)) >= FILE_NAME_SIZE)
    return FALSE;
  if (STRLCAT(szDest, DIRECTORY_STR, sizeof(szDest)) >= FILE_NAME_SIZE)
    return FALSE;
  // Copy the filename if not specified
  if (pDestFile == NULL)
  {
    if (STRLCAT(szDest, QCam_GetFileShortName(pFile), sizeof(szDest)) >= sizeof(szDest))
      return FALSE;
  }
  else
  {
    if (STRLCAT(szDest, pDestFile, sizeof(szDest)) >= sizeof(szDest))
      return FALSE;
  }

  // Check if the destination exists, if so, delete it so that we can
  // overwrite it in case this is a new version of the picture
  if (IFILEMGR_Test(pIFileMgr, szDest) == SUCCESS)
  {
    if (IFILEMGR_Remove(pIFileMgr, szDest) == EFAILED)
    {
       return FALSE;
    }
  }

  // Open the source file
  if ((pSrc = IFILEMGR_OpenFile(pIFileMgr, pFile, _OFM_READ))==NULL)
  {
    return FALSE;
  }

  // Open the destination file
  if ((pDest = IFILEMGR_OpenFile(pIFileMgr, szDest, _OFM_CREATE))==NULL)
  {
    (void)IFILE_Release(pSrc);
    return FALSE;
  }

  // Allocate the intermediate buffer
  pBuf = (uint8 *)MALLOC(FILE_COPY_SIZE);
  if (pBuf == NULL)
  {
    (void)IFILE_Release(pSrc);
    (void)IFILE_Release(pDest);
    return FALSE;
  }

  // Get the file size and check if enough space available
  (void)IFILE_GetInfo(pSrc, &info);
  remaining = info.dwSize;

  if((remaining==0) || (IFILEMGR_GetFreeSpace(pIFileMgr, NULL) < remaining))
  {
    MSG_MED("NOT ENOUGH DISK SPACE",0,0,0);
    QCam_FreeIF((void **)&pBuf);
	(void)IFILE_Release(pSrc);
    (void)IFILE_Release(pDest);
    //QCam_ShowMemoryFull(pMe);
    return FALSE;
  }

  // Loop while there is data remaining to be copied
  while (remaining)
  {
    // Figure out how much to read, either the rest of the file or
    // a chunk
    if (remaining < FILE_COPY_SIZE)
    {
      read = remaining;
    }
    else
    {
      read = FILE_COPY_SIZE;
    }
    // Read data
    read = (uint32)IFILE_Read(pSrc, pBuf, read);

    // Write out the data
    written = 0;
    while (written < read)
    {
      written += IFILE_Write(pDest, pBuf+written, read-written);
    }

    // Decrease the amount left to write
    remaining -= written;
  }

  // Release the files
  (void)IFILE_Release(pSrc);
  (void)IFILE_Release(pDest);
  QCam_FreeIF((void **)&pBuf);

  return TRUE;
}

#ifdef FEATURE_APP_MMS
#error code not present
#endif /* FEATURE_APP_MMS */

/* ==========================================================================
MENU HELPER FUNCTIONS
============================================================================= */

/*===========================================================================
FUNCTION QCam_SetMenuStyle

  DESCRIPTION
    This function sets the menu styles.

  PARAMETERS:
    pIMenu - pointer to IMenuCtl interface
    so_x   - X padding inside item
    so_y   - Y padding inside item

  RETURN VALUE:
    TRUE if image is draw, FALSE otherwise.
===========================================================================*/
void QCam_SetMenuStyle(IMenuCtl* pIMenu, int so_x, int so_y)
{
   AEEItemStyle sstyle;
   AEEItemStyle ustyle;
   AEEMenuColors col;
   uint32 dwProperties = 0;

   if (!pIMenu)
     return;

   // selection style
   sstyle.ft = AEE_FT_RAISED;
   sstyle.xOffset = 0;
   sstyle.yOffset = 0;
   sstyle.roImage = AEE_RO_TRANSPARENT;
   ustyle.ft = AEE_FT_NONE;
   ustyle.xOffset = (uint16)so_x;
   ustyle.yOffset = (uint16)so_y;
   ustyle.roImage = AEE_RO_TRANSPARENT;
   IMENUCTL_SetStyle(pIMenu, &ustyle, &sstyle);

   // colors
   col.cSelText= RGB_BLACK;
   col.wMask = MC_BACK | MC_SEL_BACK| MC_SEL_TEXT;
   col.cBack = QCAM_SOFT_MENU_COLOR;
   col.cSelBack = QCAM_SOFT_MENU_SEL_COLOR;

   //properties
   dwProperties |= IMENUCTL_GetProperties(pIMenu);
   dwProperties |= MP_UNDERLINE_TITLE | MP_WRAPSCROLL;

   IMENUCTL_SetColors(pIMenu, &col);
   IMENUCTL_SetProperties(pIMenu, dwProperties);
}

/*===========================================================================
FUNCTION QCam_InitMenuData

  DESCRIPTION
    This function initializes the menu data structure.

  PARAMETERS:
    ai     - pointer to menu item data
    pszRes - resource file name

  RETURN VALUE:
    NONE
===========================================================================*/
void QCam_InitMenuData(CtlAddItem *ai, const char * pszRes)
{
  if (!ai)
    return;

  ai->pText = NULL;
  ai->pImage = NULL;
  ai->pszResImage = ai->pszResText = pszRes;
  ai->wFont = QCAM_FONT_TEXT;
  ai->dwData = 0;
  ai->wText = 0;
}

/*===========================================================================
FUNCTION QCam_InitMenu

  DESCRIPTION
    This function initializes the menu screen and data structure.

  PARAMETERS:
    pIMenu - pointer to IMenuCtl interface
    pDisp  - pointer to IDisplay interface
    pszRes - resource file name
    ai     - pointer to menu item data
    nTitle - menu title id
    rc     - menu display rect

  RETURN VALUE:
    NONE
===========================================================================*/
void QCam_InitMenu(IMenuCtl *pMenu, IDisplay *pDisp, const char *pszRes, \
                          CtlAddItem *ai, uint16 nTitle, AEERect rc)
{
  if (!pMenu || !pDisp || !pszRes)
    return;

  //clean up old menu
  IMENUCTL_Reset(pMenu);
  (void)IMENUCTL_DeleteAll(pMenu);

  //Set the Rectangle for the Menu
  IMENUCTL_SetRect(pMenu, &rc);
  IDISPLAY_EraseRect(pDisp, &rc);

  // Set Title and init data structure
  (void)IMENUCTL_SetTitle(pMenu, pszRes, nTitle, NULL);
  QCam_InitMenuData(ai, pszRes);
}

/*===========================================================================
FUNCTION QCam_AddMenuItem

 DESCRIPTION
    This function adds a item to the soft menu.

  PARAMETERS:
    pIMenu  - pointer to IMenuCtl interface
    ai      - pointer to menu item data
    wItemID - menu item id
    wImage  - menu icon image id

  RETURN VALUE:
    NONE
===========================================================================*/
void QCam_AddMenuItem(IMenuCtl *pMenu, CtlAddItem *ai, uint16 wItemID, uint16 wImage)
{
  if (!pMenu || !ai)
    return;

  ai->wItemID = wItemID;
  ai->wImage  = wImage;
  (void)IMENUCTL_AddItemEx(pMenu, ai);
  ai->dwData++;
}

/*===========================================================================
FUNCTION QCam_AddMenuItemEx

  DESCRIPTION
    This function adds a item to the list menu

  PARAMETERS:
    pIMenu  - pointer to IMenuCtl interface
    ai      - pointer to menu item data
    wText   - menu item text id
    wItemID - menu item id
    wImage  - menu item image id
    dwData  - menu data id

  RETURN VALUE:
    NONE
===========================================================================*/
void QCam_AddMenuItemEx(IMenuCtl *pMenu, CtlAddItem *ai, uint16 wText, uint16 wItemID, uint16 wImage, uint32 dwData)
{
  if (!pMenu || !ai)
    return;

  ai->wText = wText;
  ai->wItemID = wItemID;
  ai->wImage  = wImage;
  ai->dwData = dwData;

  (void)IMENUCTL_AddItemEx(pMenu, ai);
}

/*===========================================================================
  FUNCTION: QCam_GetFocusFromSel

  DESCRIPTION:
    This function finds the menu item id that has the selected data.

  PARAMETERS:
    pMenu:  Menu control of the menu
    IdList: Menu item id list
    sel:    Selected data from this menu

  DEPENDENCIES:
    NA

  RETURN VALUE
    Item id if its data is selected. Otherwise the first item id

  SIDE EFFECTS:
  none
===========================================================================*/
uint16 QCam_GetFocusFromSel(IMenuCtl *pMenu, const uint16* IdList, uint32 sel)
{
  uint32 itemData;
  uint16 focus;
  int    i;

  if (!pMenu || !IdList)
    return 0;

  //IdList[0] holds the total item count.
  //IdList[1] is the title id
  //The item id starts from IdList[2].
  focus = IdList[2]; //start from the first item id
  for (i = 2; i < IdList[0]+2; i++)
  {
    (void)IMENUCTL_GetItemData(pMenu, IdList[i], &itemData);
    if (itemData == sel)
    {
      focus = IdList[i];
      break;
    }
  }
  return focus;
}

/*===========================================================================
  FUNCTION: QCam_SetFocusFromSel

  DESCRIPTION:
    This function finds the menu item id that has the selected data.
    Then change the radio button to On button.

  PARAMETERS:
    pMenu:  Menu control of the menu
    IdList: Menu item id list
    sel:    Selected data from this menu
    idOn:   On button id.

  DEPENDENCIES:
    NA

  RETURN VALUE
    NA

  SIDE EFFECTS:
  none
===========================================================================*/
void QCam_SetFocusFromSel(IMenuCtl *pMenu, const uint16 *IdList, uint32 sel, const char *pszRes, uint16 idOn)
{
  uint32 itemData;
  uint16 focus;
  int    i;

  if (!pMenu || !IdList)
    return;

  //IdList[0] holds the total item count.
  //IdList[1] is the title id
  //The item id starts from IdList[2].
  focus = IdList[2]; //start from the first item id
  for (i = 2; i < IdList[0]+2; i++)
  {
    (void)IMENUCTL_GetItemData(pMenu, IdList[i], &itemData);
    if (itemData == sel)
    {
      focus = IdList[i];
      QCam_SetItemImg(pMenu, pszRes, focus, idOn);
      break;
    }
  }
  //Redraw the menu
  IMENUCTL_SetSel(pMenu, focus);
}


/*=====================================================================
FUNCTION QCam_SetRadioImg

  DESCRIPTION:
    This function update the menu MSIF_IMAGE in the old and new focus items.

  PARAMETERS:
    pMenu:  Menu control pointer
    oldSel: Old focus index
    newSel: New focus index
    pszRes: Resource file pointer

  RETURN VALUE:
    void
======================================================================*/
void QCam_SetRadioImg(IMenuCtl *pMenu, const uint16 oldSel, const uint16 newSel,
                      const char *pszRes)
{
  CtlAddItem pai, paiOld;
  uint16 offImgId;

  if (!pMenu || !pszRes)
    return;

  //Set old selection item icon to RADIO_OFF and
  //new selection item icon to RADIO_ON by switching
  //the image id.
  if(IMENUCTL_GetItem(pMenu, oldSel, &paiOld))
  {
    if(IMENUCTL_GetItem(pMenu, newSel, &pai))
    {
      //save the image id of new selected item first
      offImgId = pai.wImage;
      //Set the radio on (old focus image id) to new focus
      pai.wImage = paiOld.wImage; //IDB_RADIO_ON
      pai.pszResImage = pai.pszResText = pszRes;
      pai.pImage = NULL;
      (void)IMENUCTL_SetItem(pMenu, newSel, MSIF_IMAGE, &pai);
      //Set the radio off to old focus item
      paiOld.wImage = offImgId; //IDB_RADIO_OFF
      paiOld.pszResImage = paiOld.pszResText = pszRes;
      paiOld.pImage = NULL;
      (void)IMENUCTL_SetItem(pMenu, oldSel, MSIF_IMAGE, &paiOld);
    }
  }

  //Redraw the menu
  (void)IMENUCTL_Redraw(pMenu);
}


/*=====================================================================
FUNCTION QCam_SetItemImg

  DESCRIPTION: A helper function to set the menu item image.

  PARAMETERS:
    pMenu:  Menu control pointer
    pszRes: Resource file pointer
    selID:  Item id
    imgID:  image id

  RETURN VALUE:
    void 
======================================================================*/
void QCam_SetItemImg(IMenuCtl *pMenu, const char *pszRes, uint16 selID, uint16 imgID)
{
  CtlAddItem pai;

  if(IMENUCTL_GetItem(pMenu, selID, &pai))
  {
    pai.pszResImage = pai.pszResText = pszRes;
    pai.pImage = NULL;
    pai.wImage = imgID;
    (void)IMENUCTL_SetItem(pMenu, selID, MSIF_IMAGE, &pai);
  }
  (void)IMENUCTL_Redraw(pMenu);
}

/*===========================================================================
  FUNCTION: QCam_UpdateRadioFocus

   DESCRIPTION:
    This function set the IDB_RADIO_ON to the new focus item.

  PARAMETERS:
    pMenu:  Menu control pointer
    IdList: Menu item id list
    sel:    Menu item data, will be updated with the new selection.
    pszRes: resource file pointer

  DEPENDENCIES:
    NA

  RETURN VALUE
    NA

  SIDE EFFECTS:
  none
===========================================================================*/
void QCam_UpdateRadioFocus(IMenuCtl *pMenu, const uint16 *IdList, uint32 *sel, const char * pszRes)
{
  uint16 oldFocus, newFocus;

  if (!pMenu || ! IdList || !sel)
    return;

  //Get old focus item ID
  oldFocus = QCam_GetFocusFromSel(pMenu, IdList, *sel);

  //Get new selected item ID
  newFocus = IMENUCTL_GetSel(pMenu);

  //Update selection data
  (void)IMENUCTL_GetItemData(pMenu, newFocus, sel);

  //Update the radio button with new On/Off
  QCam_SetRadioImg(pMenu, oldFocus, newFocus, pszRes);
}

/*===========================================================================
  FUNCTION: QCam_UpdateCheckMark

   DESCRIPTION:
    This function toggle the check mark on the selected property item.

  PARAMETERS:
    pMenu:      Menu control pointer
    properties: properties
    idOn:       image id for checked mark
    idOff:      image id for uncheck mark
    pszRes:     resource file pointer

  DEPENDENCIES:
    NA

  RETURN VALUE
    NA

  SIDE EFFECTS:
  none
===========================================================================*/
boolean QCam_UpdateCheckMark(IMenuCtl *pMenu, uint32 *properties,
                             uint16 idOn, uint16 idOff, const char * pszRes)
{
  boolean flag = FALSE;
  uint32 property = 0;
  uint16 selection = 0;

  if (!pMenu || !properties || !pszRes)
    return FALSE;

  //Get the current item property
  selection = IMENUCTL_GetSel(pMenu);
  if (!IMENUCTL_GetItemData(pMenu, selection, &property))
    return FALSE;

  //Use current property as mask and invert the property flag
  flag = QCam_InvertProperties(properties, property);

  //Set/reset the check mark in the selected item
  QCam_SetItemImg(pMenu, pszRes, selection, (uint16)(flag ? idOn : idOff));

  return flag;
}

/*===========================================================================
  FUNCTION: QCam_BuildRadioListMenu

  DESCRIPTION:
    This function build a list menu with radio icon for selection.

  PARAMETERS:
    pMenu:    Menu control pointer
    pai:      CtlAddItem pointer
    IdList:   Menu item id list
    DataList: Data list associate with this menu
    sel:      Selected data from this menu
    idOn:     Image id for radio on
    idOff:    Image id for radio off

  DEPENDENCIES:
    NA

  RETURN VALUE
    NA

  SIDE EFFECTS:
  none
===========================================================================*/
void QCam_BuildRadioListMenu(IMenuCtl *pMenu, CtlAddItem *pai,
                             const uint16 *IdList, const uint32 *DataList,
                             uint32 sel, uint16 idOn, uint16 idOff)
{
  uint32 dwData;
  uint16 index = 0;

  if (!pMenu || !pai || !IdList)
    return;

  //Add individual entries to the Menu
  while (index < IdList[0])
  {
    //fill the dwData
    dwData = DataList ? DataList[index] : index;

    //add menu items, set all radio button to Off button first
    QCam_AddMenuItemEx(pMenu, pai, IdList[index + 2], IdList[index + 2], idOff, dwData);
    index++;
  }

  //Update the radio button to On button if the item is focus item
  QCam_SetFocusFromSel(pMenu, IdList, sel, pai->pszResImage, idOn);
}

/*===========================================================================
  FUNCTION: QCam_BuildCheckListMenu

  DESCRIPTION:
    This function build a list menu with check icons.

  PARAMETERS:
    pMenu:    Menu control pointer
    pai:      CtlAddItem pointer
    IdList:   Menu item id list
    DataList: Data list associate with this menu
    sel:      properties of selection
    idOn:     image id for checked mark
    idOff:    image id for uncheck mark

  DEPENDENCIES:
    NA

  RETURN VALUE
    NA

  SIDE EFFECTS:
  none
===========================================================================*/
void QCam_BuildCheckListMenu(IMenuCtl *pMenu, CtlAddItem *pai,
                             const uint16* IdList, const uint32 *DataList,
                             uint32 sel, uint16 idOn, uint16 idOff)
{
  uint16 index = 0;
  uint16 wImage = idOff;
  uint32 dwData;

  if (!pMenu || !pai || !IdList)
    return;

  //Add individual entries to the Menu
  while (index < IdList[0])
  {
    dwData = DataList ? DataList[index] : index;
    wImage = (uint16)(QCam_CheckProperties(sel, dwData) ? idOn : idOff);

    //add menu items
    QCam_AddMenuItemEx(pMenu, pai, IdList[index + 2], IdList[index + 2], wImage, dwData);
    index++;
  }

  IMENUCTL_SetSel(pMenu, 0);
}

/*=====================================================================
FUNCTION QCam_GetArgString

  DESCRIPTION: This function converts the launch data structure into a char string.
               ISHELL_StartAppletArgs() only takes string argument

  PARAMETERS:
    pLaunchDS - Data struct for launching Qalbum
    pStr      - Argument string for ISHELL_StartAppletArgs
    size      - Size of the pStr buffer

  RETURN VALUE:
    None
======================================================================*/
void QCam_GetArgString(const Qalbum_LaunchDS *pLaunchDS, char *pStr, int size)
{
  if (!pLaunchDS || !pStr)
    return;

  if (size < ARG_STRING_SIZE)
    return;

  //Convert the data struct to a char string so it will be handled by
  //BREW system when Qcamera exits before Qalbum starts. We will stop
  //Qcamera when launching Qalbum to work around the heap limitation
  //issues when edit large image.
  if (pLaunchDS->mode == VIEWER_MODE_FRAME)
  {
    (void)SNPRINTF(pStr, size, "%d,%d,%d,%d,%d,%d,%d,%s,%s",
                  pLaunchDS->mode, pLaunchDS->nSaveLoc, pLaunchDS->bDirSortAscend,
                  pLaunchDS->bPhotoSortAscend, pLaunchDS->nSlideshowSpeed,
                  pLaunchDS->bMultiDisplayMode,pLaunchDS->bSlideshowResize,
                  pLaunchDS->sFrameSizeDir, pLaunchDS->sFrameFileName);
  }
  else
  {
    //No need to include frame directory and file name if it is not for frame mode
    (void)SNPRINTF(pStr, size, "%d,%d,%d,%d,%d,%d,%d",
                  pLaunchDS->mode, pLaunchDS->nSaveLoc, pLaunchDS->bDirSortAscend,
                  pLaunchDS->bPhotoSortAscend, pLaunchDS->nSlideshowSpeed,
                  pLaunchDS->bMultiDisplayMode, pLaunchDS->bSlideshowResize);
  }
}

/*=====================================================================
FUNCTION QCam_ParseArgString

  DESCRIPTION: This function converts the argument sting into Qalbum_LaunchDS data struct

  PARAMETERS:
    argStr - input data string.

  RETURN VALUE:
    Qalbum_LaunchDS type data

  DEPENDENCE:
    This function depends on how the QCam_GetArgString() create the string.
    If QCam_GetArgString() is updated, this function has to be updated as well.
======================================================================*/
Qalbum_LaunchDS *QCam_ParseArgString(const char *argStr)
{
  Qalbum_LaunchDS *pData = NULL;
  char tempStr[ARG_STRING_SIZE];
  char *token, *pChr;
  int count = 0;
  uint32 len;

  if (!argStr)
    return NULL;

  len = STRLEN(argStr);
  if ((len == 0) || (len >= ARG_STRING_SIZE))
    return NULL;

  //Need to allocate memory for the data struct.
  pData = (Qalbum_LaunchDS *)MALLOC(sizeof(Qalbum_LaunchDS));
  if (!pData)
    return NULL;

  (void)STRLCPY(tempStr, argStr, sizeof(tempStr));
  pChr = STRCHR(tempStr, ',');
  if (pChr)
  {
    *pChr = '\0';
    pChr++;
  }
  token = tempStr;
  while (token)
  {
    switch(count)
    {
    case 0:
      pData->mode = (QalbumLaunchMode)ATOI(token);
      break;
    case 1:
      pData->nSaveLoc = (QCamSaveType)ATOI(token);
      break;
    case 2:
      pData->bDirSortAscend = (ATOI(token) == 1) ? TRUE : FALSE;
      break;
    case 3:
      pData->bPhotoSortAscend = (ATOI(token) == 1) ? TRUE : FALSE;
      break;
    case 4:
      pData->nSlideshowSpeed = (uint16)ATOI(token);
      break;
    case 5:
      pData->bMultiDisplayMode = (ATOI(token) == 1) ? TRUE : FALSE;
      break;
    case 6:
      pData->bSlideshowResize = (ATOI(token) == 1) ? TRUE : FALSE;
      break;
    case 7:
      (void)STRLCPY(pData->sFrameSizeDir, token, sizeof(pData->sFrameSizeDir));
      break;
    case 8:
      (void)STRLCPY(pData->sFrameFileName, token, sizeof(pData->sFrameFileName));
      break;
    default:
      break;
    }
    if (pChr)
    {
      token = pChr;
      pChr = STRCHR(token, ',');
      if (pChr)
      {
        *pChr = '\0';
        pChr++;
      }
      count++;
    }
    else
      token = NULL;
  }

  return pData;
}

#endif //#ifdef FEATURE_APP_QCAMERA

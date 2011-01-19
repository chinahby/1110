/*===========================================================================

FILE: QCamCommon.h

GENERAL DESCRIPTION
    This is the common header file contains definitions for share functions

    Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamcommon/main/latest/inc/QcamCommon.h#2 $ $DateTime: 2009/04/15 00:20:08 $ $Author: saigopal $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/15/09  sai    Added save location paths for USB MSD feature
 01/08/08  KG     Removed AEEFTL.h from the include files.
 10/04/07  SK     Placed some of the wide resolutions at proer place
 09/18/09  NC     Fixed some string length issue on new function calls.
 09/05/07  NC     Added share function QCam_SetItemImg
 05/22/07  SK     JPEG Decoder Resizing feature
 04/19/07  SK     UI Support for WVGA Display
 03/28/07  TV     Added Methods to get Image Ratios
 02/08/07  NC     Added AEEFS_FTL_DIR definition back incase feature is not defined.
 02/07/07  NC     Change FTL directory include file to AEEFTL.h per BREW change.
 11/09/06  NC     Added QCam_HasDirPhoto to optimize search.
 10/27/06  NC     Added YCBCR format and other new definitions.
 08/15/06  NC     Added SXGA and UXGA frame overlay support.
 06/14/06  JR     Added support for FTL memory.
 06/08/06  NC     Added new function QCam_SetRadioImg().
 03/31/06  NC     Added ARG_STRING_SIZE for converting/parsing argument string.
                  Added helper functions QCam_GetArgString() and QCam_ParseArgString().
 03/20/06  NC     Added VGA frame directory
 10/17/05  NC     Added QCAM_SIZE_MAX support
 10/13/05  NC     Change image size definition to enum.
 09/15/05  NC     Lint errors clean up.
 08/25/05  NC     Used QCam_FreeIF for CAM_FREEIF macro.
 06/16/05  JN     Added new param to QCam_CopyToDest to allow rename file.
 06/09/05  NC     Update definition and parm for QCam_BuildDirList and QCam_GetTotalPhotos
 05/23/05  JN     Centralize font definitions.
 05/20/05  NC     Added new helper function QCam_GetDirPhotos and del flag DEL_RAW.
 05/18/05  NC     Added new definitions for MMC card.
 03/28/05  NC     Defined new directory strings for frame size directory.
                  Added bDel parm to QCam_GetTotalPhotos().
 03/09/05  NC     Added bDel parm to Qcam_BuildDirList() so we can delete the Raw dir.
 02/22/05  NC     Updated param name in Qcam_DeleteDirs()
 01/24/05  DRL    Changed FEATURE_COMMON_MM_DIR to be consistant with other
                  multi-media apps (media player and qcamcorder).
 01/17/05  NC     Added copyright and update header.
 ============================================================================*/
#ifndef QCAMCOMMON_H
#define QCAMCOMMON_H

#ifndef AEE_SIMULATOR
#include "comdef.h"
#endif

#ifdef FEATURE_APP_QCAMERA

#include "AEE.h"
#include "AEEText.h"
#include "AEECamera.h"
#include "OEMFeatures.h" //defined FEATURE_OVERLAY

//Multiple display must have IOverlay support.
//undef it if FEATURE_OVERLAY is off to simplify the UI
#ifndef FEATURE_OVERLAY
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
#error Must define FEATURE_OVERLAY if FEATURE_CAMERA_MULTIPLE_DIPSPLAY is to be defined.
#endif // FEATURE_CAMERA_MULTIPLE_DIPSPLAY
#endif // FEATURE_OVERLAY


#ifndef AEEFS_FTL_DIR
#define AEEFS_FTL_DIR "fs:/shared/ftl/"
#endif

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
#define AEEFS_MOD_USB_DIR "fs:/mod/26108/"
#endif

// Size of buffer used to hold text strings
#define FILE_NAME_SIZE        (80)
// Amount to read in one block when copying data between
// two files
#define FILE_COPY_SIZE        (2048)
// Size of buffer used to hold text strings
#define TEXT_STRING_SIZE      (40)
// Size of buffer used to hold the argument string to launch Qalbum from Qcamera
// For frame mode, the string is an address of the Qalbum_LaunchDS.
// For album and slideshow mode, the string has contents of Qalbum_LaunchDS
// converted from function QCam_GetArgString(). The result is parsed back to 
// Qalbum_LaunchDS by QCam_ParseArgString().
#define ARG_STRING_SIZE       (256) 

#define PROMPT_SIZE           (256)
#define QCAM_EXTENSION_LENGTH (4) //Not used for ycbcr extention.

#define QCAM_3_CHAR_LEN 3
#define QCAM_4_CHAR_LEN 4	
#define QCAM_5_CHAR_LEN 5			


#define QCAM_SHUTTER_FILE "No Sound"

#define DOT_CHAR '.'
#define QCAM_JPEG_EXTENSION ".jpg"
#define QCAM_PNG_EXTENSION  ".png"
#define QCAM_RAW_EXTENSION  ".raw"
#define QCAM_WAV_EXTENSION  ".wav"
#define QCAM_YCBCR_EXTENSION ".ycbcr"
#define QCAM_JPEG_EXTENSION_CAP ".JPG"
#define QCAM_PNG_EXTENSION_CAP  ".PNG"

#define QCAM_PHOTOS_SDIR "img"
#define QCAM_RAW_SDIR    "raw"
#define QCAM_YCBCR_SDIR  "ycbcr"
#define QCAM_SOUNDS_SDIR "snd"
#define QCAM_FRAMES_SDIR "frm"

#define QCAM_QVGA_SDIR  "/qvga"
#define QCAM_QCIF_SDIR  "/qcif"
#define QCAM_QQVGA_SDIR "/qqvga"
#define QCAM_VGA_SDIR   "/vga"
#define QCAM_SXGA_SDIR  "/sxga"
#define QCAM_UXGA_SDIR  "/uxga"
#define FRAME_SDIR_SIZE  7 //string size for frame sub directory and NULL termination

#ifdef FEATURE_COMMON_MM_DIR 
#define QCAM_ROOT_DIR    "fs:/shared/media"
#else
//Root directory includes QCAMERA class id to enable
//other application like QALBUM to access it. (from BREW 3.0)
#define QCAM_ROOT_DIR    "fs:/~0x01011b9a/qcam"
#endif

#define QCAM_CONFIG_FILE  QCAM_ROOT_DIR DIRECTORY_STR "qcam.cfg"

#define QCAM_MMC_DIR "fs:/card0/img"
#define QCAM_MMC_RAW_DIR "fs:/card0/raw"
#define QCAM_MMC_YCBCR_DIR "fs:/card0/ycbcr"
#define QCAM_MMC_CONSTANT "fs:/card0/"

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
//newly added for USB Mass storage device
#define QCAM_USB_DIR "fs:/mod/26108/img"
#define QCAM_USB_RAW_DIR "fs:/mod/26108/raw"
#define QCAM_USB_YCBCR_DIR "fs:/mod/26108/ycbcr"
#define QCAM_USB_CONSTANT "fs:/mod/26108/"
#endif

#define QCAM_FTL_DIR      AEEFS_FTL_DIR "img"
#define QCAM_FTL_RAW_DIR  AEEFS_FTL_DIR "raw"
#define QCAM_FTL_CONSTANT AEEFS_FTL_DIR

#define QCAM_PHOTOS_DIR QCAM_ROOT_DIR DIRECTORY_STR QCAM_PHOTOS_SDIR
#define QCAM_RAW_DIR    QCAM_ROOT_DIR DIRECTORY_STR QCAM_RAW_SDIR
#define QCAM_YCBCR_DIR  QCAM_ROOT_DIR DIRECTORY_STR QCAM_YCBCR_SDIR
#define QCAM_SOUNDS_DIR QCAM_ROOT_DIR DIRECTORY_STR QCAM_SOUNDS_SDIR
#define QCAM_FRAMES_DIR QCAM_ROOT_DIR DIRECTORY_STR QCAM_FRAMES_SDIR

#define QCAM_ROOT_DIR_LEN    (STRLEN(QCAM_ROOT_DIR    DIRECTORY_STR))
#define QCAM_PHOTOS_DIR_LEN  (STRLEN(QCAM_PHOTOS_DIR  DIRECTORY_STR))
#define QCAM_RAW_DIR_LEN     (STRLEN(QCAM_RAW_DIR     DIRECTORY_STR))
#define QCAM_YCBCR_DIR_LEN   (STRLEN(QCAM_YCBCR_DIR   DIRECTORY_STR))
#define QCAM_SOUNDS_DIR_LEN  (STRLEN(QCAM_SOUNDS_DIR  DIRECTORY_STR))
#define QCAM_FRAMES_DIR_LEN  (STRLEN(QCAM_FRAMES_DIR  DIRECTORY_STR))

#define QCAM_MMC_DIR_LEN       (STRLEN(QCAM_MMC_DIR     DIRECTORY_STR))
#define QCAM_MMC_CONST_LEN     (STRLEN(QCAM_MMC_CONSTANT))
#define QCAM_MMC_RAW_DIR_LEN   (STRLEN(QCAM_MMC_RAW_DIR DIRECTORY_STR))
#define QCAM_MMC_YCBCR_DIR_LEN (STRLEN(QCAM_MMC_YCBCR_DIR DIRECTORY_STR))

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
#define QCAM_USB_DIR_LEN       (STRLEN(QCAM_USB_DIR     DIRECTORY_STR))
#define QCAM_USB_CONST_LEN     (STRLEN(QCAM_USB_CONSTANT))
#define QCAM_USB_RAW_DIR_LEN   (STRLEN(QCAM_USB_RAW_DIR DIRECTORY_STR))
#define QCAM_USB_YCBCR_DIR_LEN (STRLEN(QCAM_USB_YCBCR_DIR DIRECTORY_STR))
#endif

#define QCAM_FTL_DIR_LEN     (STRLEN(QCAM_FTL_DIR     DIRECTORY_STR))
#define QCAM_FTL_CONST_LEN   (STRLEN(QCAM_FTL_CONSTANT))
#define QCAM_FTL_RAW_DIR_LEN (STRLEN(QCAM_FTL_RAW_DIR DIRECTORY_STR))

// Length of the the date sub directory + '/' (this is a fixed length)
#define QCAM_PHOTOS_DATE_SDIR_LEN (11)
// Length of the photos directory + the length of the date sub directory
#define QCAM_PHOTOS_DATE_DIR_LEN  (QCAM_PHOTOS_DIR_LEN  + QCAM_PHOTOS_DATE_SDIR_LEN)
#define QCAM_YCBCR_DATE_DIR_LEN   (QCAM_YCBCR_DIR_LEN   + QCAM_PHOTOS_DATE_SDIR_LEN)
#define QCAM_RAW_DATE_DIR_LEN     (QCAM_RAW_DIR_LEN     + QCAM_PHOTOS_DATE_SDIR_LEN)
#define QCAM_MMC_DATE_DIR_LEN     (QCAM_MMC_DIR_LEN     + QCAM_PHOTOS_DATE_SDIR_LEN)
#define QCAM_MMC_RAW_DATE_DIR_LEN (QCAM_MMC_RAW_DIR_LEN + QCAM_PHOTOS_DATE_SDIR_LEN)
#define QCAM_MMC_YCBCR_DATE_DIR_LEN (QCAM_MMC_YCBCR_DIR_LEN + QCAM_PHOTOS_DATE_SDIR_LEN)
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
#define QCAM_USB_DATE_DIR_LEN     (QCAM_USB_DIR_LEN     + QCAM_PHOTOS_DATE_SDIR_LEN)
#define QCAM_USB_RAW_DATE_DIR_LEN (QCAM_USB_RAW_DIR_LEN + QCAM_PHOTOS_DATE_SDIR_LEN)
#define QCAM_USB_YCBCR_DATE_DIR_LEN (QCAM_USB_YCBCR_DIR_LEN + QCAM_PHOTOS_DATE_SDIR_LEN)
#endif
#define QCAM_FTL_DATE_DIR_LEN     (QCAM_FTL_DIR_LEN     + QCAM_PHOTOS_DATE_SDIR_LEN)
#define QCAM_FTL_RAW_DATE_DIR_LEN (QCAM_FTL_RAW_DIR_LEN + QCAM_PHOTOS_DATE_SDIR_LEN)

// Common setting for QCam applications
#define QCAM_SOFT_MENU_COLOR MAKE_RGB(255, 255, 204) // menu background color
#define QCAM_SOFT_MENU_SEL_COLOR MAKE_RGB(153, 204, 204); // selected menu background color
#define QCAM_IMAGE_ICON_HEIGHT  80
#define QCAM_BRIGHTNESS_MAX     255
#define QCAM_BRIGHTNESS_MIN     -255
#define QCAM_BRIGHTNESS_FACTOR  8

//image size index
typedef enum
{
  QCAM_SIZE_MAX = 0, //actual sensor size
  QCAM_SIZE_QSXGA,
  QCAM_SIZE_QXGAP,
  QCAM_SIZE_QXGA,
  QCAM_SIZE_UXGA,
  QCAM_SIZE_SXGA, // 5
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  QCAM_SIZE_4VGA,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  QCAM_SIZE_XGA,
  QCAM_SIZE_SVGA,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  QCAM_SIZE_VGA,
  QCAM_SIZE_CIF, //10
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  QCAM_SIZE_QVGA,
  QCAM_SIZE_QCIF,
  QCAM_SIZE_QQVGA,
  QCAM_SIZE_STAMP,
  QCAM_SIZE_COUNT //15 
}QCamSizeType;

typedef enum
{
  QCAM_SAVE_FLASH,
  QCAM_SAVE_MMC,
  QCAM_SAVE_FTL
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  ,QCAM_SAVE_USB
#endif
}QCamSaveType;

typedef enum{
  VIEWER_MODE_ALBUM = 0,
  VIEWER_MODE_SLIDESHOW,
  VIEWER_MODE_FRAME
}QalbumLaunchMode;

typedef struct 
{
  uint16 wThumbnailWidth;
  uint16 wThumbnailHeight;
  uint16 wPhotoWidth;
  uint16 wPhotoHeight;
  uint16 wDirWidth;
  uint16 wDirHeight;
}QcamDispSizes;

typedef struct _QCam_FileList QCam_FileList;
struct _QCam_FileList
{
   char fileName[FILE_NAME_SIZE];
   QCam_FileList *next;
   QCam_FileList *prev;
};

typedef struct
{
  QalbumLaunchMode mode;
  QCamSaveType    nSaveLoc;
  uint16          nSlideshowSpeed;
  boolean         bDirSortAscend;
  boolean         bPhotoSortAscend;
  boolean         bMultiDisplayMode;
  boolean         bSlideshowResize;
  char            sFrameSizeDir[FRAME_SDIR_SIZE];
  char            sFrameFileName[FILE_NAME_SIZE];
}Qalbum_LaunchDS;

//struct for icon menu item id and icon id list
typedef struct
{
  uint16 menuID;
  uint16 iconID;
} CAMICONMENUID;

typedef enum{
  LOC_ALL = 0,
  LOC_FLASH,
  LOC_MMC,
  LOC_FTL,
  LOC_RAW,
  LOC_YCBCR,
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  LOC_USB,
#endif
  LOC_ALL_SHOW
}SearchFlag;
/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
void QCam_ReleaseIF(IBase **ppif);
void QCam_FreeIF(void **ppif);
AEESize * QCam_GetImageDimension(int size);
AEESize * QCam_GetLandscapeImageDimension(int size);
AEESize * QCam_GetImageRatio(int size);
AEESize * QCam_GetLandscapeImageRatio(int size);
void QCam_GetArgString(const Qalbum_LaunchDS *pLaunchDS, char *pStr, int size);
Qalbum_LaunchDS *QCam_ParseArgString(const char *argStr);

//image link list handlers
void QCam_DeleteFileList(QCam_FileList **first);
void QCam_DeleteItemFromList(QCam_FileList **first, int item);
boolean QCam_InsertItemToList(const char *fileName, QCam_FileList **first, int item);
boolean QCam_InsertSortFileList(const char *fileName, QCam_FileList **first, boolean ascending);
int16   QCam_BuildDirList(IFileMgr *pIFileMgr, QCam_FileList **DirList, 
                          boolean ascending, boolean bDel, SearchFlag flag);
int16  QCam_BuildPhotoList(IFileMgr *pIFileMgr, QCam_FileList *curDir, QCam_FileList **photoList, boolean ascending);
int16  QCam_BuildFrameList(IFileMgr *pIFileMgr, const char *pDir, QCam_FileList **pFrameList);
uint16 QCam_BuildSoundList(IFileMgr *pIFileMgr, QCam_FileList **pSoundList);

boolean QCam_GetCurrentItem(QCam_FileList *first, QCam_FileList **cur, int16 idx);
uint32  QCam_GetDirPhotos(IFileMgr *pIFileMgr, const char *pDirName);
uint32  QCam_GetTotalPhotos(IFileMgr *pIFileMgr, boolean bDel, SearchFlag flag);
int16   QCam_GetCurrentFrameIndex(QCam_FileList *pFrameList, const char *sFrameName);
boolean QCam_HasPhotos(IFileMgr *pIFileMgr, SearchFlag flag);
boolean QCam_HasDirPhoto(IFileMgr *pIFileMgr, const char *pDirName);

//property handlers
boolean QCam_CheckProperties(uint32 propertyFlags, uint32 propertiesMask);
boolean QCam_InvertProperties(uint32 *propertyFlags, uint32 propertiesMask);

//file name handlers 
char *  QCam_GetFileNameExtension(const char *pszFileName);
boolean QCam_GetFilePath(const char *pszFileName, char *pDir, int nLen);
const char * QCam_GetFileShortName(const char *pszFileName);
boolean QCam_ValidateFileName(const AECHAR *fileName);
boolean QCam_GetValidFileName(ITextCtl *textCtl, char *pFile, int dirLen);
void QCam_GenerateFileName(IFileMgr *pIFileMgr, const char *pDir, const char *pExt, uint16 *cur, char *strName);
void QCam_UpdateCurrentTime(uint16 *day, uint16 *month, uint16 *year);

//file handlers
boolean QCam_DeleteFile(IFileMgr *pIFileMgr, const char *pFile, boolean bRmDir);
boolean Qcam_DeleteDirs(IFileMgr *pIFileMgr, SearchFlag flag);
boolean QCam_CopyToDest(IFileMgr *pIFileMgr, const char *pFile,
                        const char *pDestPath, const char *pDestFile);
#ifdef FEATURE_APP_MMS
#error code not present
#endif //FEATURE_APP_MMS
/**************** Menu helper functions *******************/
void QCam_SetMenuStyle(IMenuCtl *pIMenu, int so_x, int so_y);
void QCam_InitMenuData(CtlAddItem *ai, const char *pszRes);
void QCam_InitMenu(IMenuCtl *pMenu, IDisplay *pDisp, const char *pszRes, 
                          CtlAddItem *ai, uint16 nTitle, AEERect rc);
void QCam_AddMenuItem(IMenuCtl *pMenu, CtlAddItem *ai, uint16 wItemID, uint16 wImage);
void QCam_AddMenuItemEx(IMenuCtl *pMenu, CtlAddItem *ai, uint16 wText, \
                               uint16 wItemID, uint16 wImage, uint32 dwData);
uint16 QCam_GetFocusFromSel(IMenuCtl *pMenu, const uint16 *IdList, uint32 sel);
void QCam_SetFocusFromSel(IMenuCtl *pMenu, const uint16 *IdList, uint32 sel, const char *pszRes, uint16 idOn);
void QCam_UpdateRadioFocus(IMenuCtl *pMenu, const uint16 *IdList, uint32 *sel, const char *pszRes);
void QCam_SetRadioImg(IMenuCtl *pMenu, const uint16 oldSel, const uint16 newSel, const char *pszRes);
boolean QCam_UpdateCheckMark(IMenuCtl *pMenu, uint32 *properties, uint16 idOn, uint16 idOff, const char *pszRes);
boolean QCam_IsSameAspectRation(int size1, int size2);
void QCam_SetItemImg(IMenuCtl *pMenu, const char *pszRes, uint16 selID, uint16 imgID);
void QCam_BuildRadioListMenu(IMenuCtl *pMenu, CtlAddItem *ai, const uint16 *IdList, const uint32 *DataList, \
                             uint32 sel, uint16 idOn, uint16 idOff);
void QCam_BuildCheckListMenu(IMenuCtl *pMenu, CtlAddItem *ai, const uint16 *IdList, const uint32 *DataList, \
                             uint32 sel, uint16 idOn, uint16 idOff);

/*===============================================================================
                     MACROS
=============================================================================== */
#define CAM_ISTYPEFILE(x, y)  (STRENDS(x, y))
#define CAM_RELEASEIF(p)      QCam_ReleaseIF((IBase **)&(p))
#define CAM_FREEIF(p)         QCam_FreeIF((void **)&(p))

#endif //FEATURE_APP_QCAMERA

#endif //QCAMCOMMON_H

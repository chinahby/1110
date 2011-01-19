/*===========================================================================

FILE: qalbumEdit_i.h

GENERAL DESCRIPTION
    This is a private header file only used by qalbumEdit.c.

      Copyright (c) 2006 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qalbum/main/latest/src/qalbumEdit_i.h#1 $ $DateTime: 2008/05/12 10:09:09 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 12/15/07  KV     Prototype for Qalbum_ExecuteColorFormatMenu routinue.
 10/01/07  SK     Fixed issue in building resize menu.
 04/19/07  SK     UI Support for WVGA Display
 04/17/07  NC     Enabled larger image resize.
 02/23/07  TV     Added PictBridge Printing ability
 11/08/06  ZL     Moved editing feature from qalbum_i.h.

============================================================================*/

/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#if defined(FEATURE_APP_QALBUM)
#include "qalbum.h"

#define QALBUM_MEASURE_TIME  0

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static void    Qalbum_BuildResizeMenu(Qalbum *pMe);
static void    Qalbum_UpdateResizeMenu(Qalbum *pMe);
#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
static void    Qalbum_UpdatePrintMenu(Qalbum *pMe);
#endif //#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
static boolean Qalbum_GetMatchFrameDir(Qalbum *pMe);

static boolean Qalbum_OnResize(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnFlash(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnFrame(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnGrayScale(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnInvertColors(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnBrightness(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnHue(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnPosterize(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnSolarize(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnSaturation(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnSharpness(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnRotate(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnSepia(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnFlip(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnUpKey(Qalbum *pMe, uint16 sel, uint16 key);

static boolean Qalbum_DoFlip(Qalbum *pMe, uint16 key);
static boolean Qalbum_DoRotate(Qalbum *pMe, uint16 key);

static void    Qalbum_UpdateColorFormatMenu(Qalbum * pMe);

// edit menu On select key handler function table
static const QalbumMenuKeyHandler QalbumEditHandlers[] =
{
  {IDS_IMAGE_SEPIA,         Qalbum_OnSepia},
  {IDS_IMAGE_RESIZE,        Qalbum_OnResize},
  {IDS_IMAGE_ROTATE,        Qalbum_OnRotate},
  {IDS_IMAGE_FLIP,          Qalbum_OnFlip},
  {IDS_IMAGE_GRAY_SCALE,    Qalbum_OnGrayScale},
  {IDS_IMAGE_INVERT_COLORS, Qalbum_OnInvertColors},
  {IDS_IMAGE_POSTERIZE,     Qalbum_OnPosterize},
  {IDS_IMAGE_SOLARIZE,      Qalbum_OnSolarize},
  {IDS_BRIGHTNESS,          Qalbum_OnBrightness},
  {IDS_IMAGE_HUE,           Qalbum_OnHue},
  {IDS_IMAGE_SATURATION,    Qalbum_OnSaturation},
  {IDS_IMAGE_SHARPNESS,     Qalbum_OnSharpness},
  {IDS_FLASH,               Qalbum_OnFlash},
  {IDS_FRAME,               Qalbum_OnFrame}
};

//Size selection items and data list
static const uint16 ImgSizeIDList[] =
{
#ifndef FEATURE_CAMERA_WIDESIZE
  14,
#else 
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  IDS_IMAGE_RESIZE,
  IDS_SIZE_QSXGA,
  IDS_SIZE_QXGAP,
  IDS_SIZE_QXGA,
  IDS_SIZE_UXGA,
  IDS_SIZE_SXGA,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  IDS_SIZE_4VGA,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  IDS_SIZE_XGA,
  IDS_SIZE_SVGA,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  IDS_SIZE_VGA,
  IDS_SIZE_CIF,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  IDS_SIZE_QVGA,
  IDS_SIZE_QCIF,
  IDS_SIZE_QQVGA,
  IDS_SIZE_STAMP
};

static const uint32 ImgSizeDataList[] =
{
  QCAM_SIZE_QSXGA,
  QCAM_SIZE_QXGAP,
  QCAM_SIZE_QXGA,
  QCAM_SIZE_UXGA,
  QCAM_SIZE_SXGA,
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
  QCAM_SIZE_CIF,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  QCAM_SIZE_QVGA,
  QCAM_SIZE_QCIF,
  QCAM_SIZE_QQVGA,
  QCAM_SIZE_STAMP
};

#endif //FEATURE_APP_QALBUM

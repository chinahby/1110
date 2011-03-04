/*===========================================================================

FILE: QCamCommon.h

GENERAL DESCRIPTION
    This is the common header file contains definitions for display specific settings.

    Copyright (c) 2004 - 2006 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamcommon/main/latest/inc/QcamDef.h#2 $ $DateTime: 2009/03/10 02:26:05 $ $Author: c_rtiwar $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 03/10/09  RK     Fixed the UI messed up issue in 7625 FFA.
 11/09/07  NC     Removed Multisho size definition to qcamera_i.h.
 08/15/07  KD     Added constants for multishot menu dimensions
 07/30/07  KV     Updated the QALBUM_THIRD_POS and SECOND_POS position. 
 05/24/07  NC     Defined new variables for 7K
 03/23/07  KV     Defined Picture Postion for VGA Display
 03/07/07  NC     Switch the QCAM_LCD_OFFSET per FEATURE_UIONE_HDK condition.
 01/24/07  TV     Added define for screen size 
 10/30/06  ZL     Added QCAM_NAV_ARROW_WIDTH.
 06/16/06  NC     Added target specific QCAM_LCD_OFFSET.
 11/01/05  NC     Initial design for specific definitions.
 ============================================================================*/
#ifndef QCAMDEF_H
#define QCAMDEF_H

#ifndef AEE_SIMULATOR
#include "comdef.h"
#endif

#ifdef FEATURE_APP_QCAMERA
/*============================================================================
DEFINITION FOR DISPLAY POSITION SETTING

===========================================================================*/
//Setting for full image display
#define QCAM_PICTURE_X_POS  (0)
#if (defined (T_MSM7625) && defined (T_FFA))
  #define QCAM_PICTURE_Y_POS  (30)
  #define QCAM_PICTURE_Y_POS_VGA (40)
#else
  #define QCAM_PICTURE_Y_POS  (16)
  #define QCAM_PICTURE_Y_POS_VGA (26)
#endif

//Settings for thumbnail display
#define QCAM_THUMBNAILS_X       2 // Number of thumbnails to draw in the x direction
#define QCAM_THUMBNAILS_Y       2 // Number of thumbnails to draw in the y direction
#define QCAM_THUMBNAILS         (QCAM_THUMBNAILS_X * QCAM_THUMBNAILS_Y)
#define QCAM_THUMBNAIL_X_START  5 //Left/right margin of thumbnail to screen
#define QCAM_THUMBNAIL_X_OFFSET 6 //Horizon space beteen thumbnails
#define QCAM_THUMBNAIL_Y_START  17 //top margin of thumbnail to screen
#define QCAM_THUMBNAIL_Y_OFFSET 6  //Vertical space beteen thumbnails

//Settings for directory display
#define QCAM_DIRS_X       2 // Number of directories to draw in the x direction
#define QCAM_DIRS_Y       3 // Number of directories to draw in the y direction
#define QCAM_DIRS         (QCAM_DIRS_X * QCAM_DIRS_Y)
#define QCAM_DIR_X_START  5
#define QCAM_DIR_X_OFFSET 6
#define QCAM_DIR_Y_START  17
#define QCAM_DIR_Y_OFFSET 6

// Setting for text display
#define QCAM_TXT_X_START  2
//Offset for sel image border
#define QCAM_SEL_OFFSET   3

// Title navigation arrow width
#define QCAM_NAV_ARROW_WIDTH  3

// Title navigation arrow width for VGA display
#define QCAM_NAV_ARROW_WIDTH_VGA  7

// Title image offsets for Qcamera
#define QCAM_FIRST_POS    2
#define QCAM_SECOND_POS   60
#define QCAM_THIRD_POS    150

// Title image offsets for Qlbum
#define QALBUM_FIRST_POS   (QCAM_NAV_ARROW_WIDTH + 2)
#define QALBUM_SECOND_POS  85
#define QALBUM_SECOND_POS_SM (QALBUM_FIRST_POS + 18)
#define QALBUM_THIRD_POS   180

// Font definition
#define QCAM_FONT_TITLE   AEE_FONT_BOLD
#define QCAM_FONT_TEXT    AEE_FONT_LARGE

#define QCAM_SOFT_MENU_HEIGHT 29 // Height in pixels of the soft key menu
#define QCAM_SOFT_MENU_HEIGHT_VGA 54 // Height in pixels of the soft key menu on a VGA screen
#define QCAM_BUTTON_MENU_HEIGHT 20 //Height in pixels of the button menu

//Small screen width
//SC1X values
//#define SMALL_SCREEN_WIDTH 128
//#define SMALL_SCREEN_HEIGHT 128
//SC2X values
#define SMALL_SCREEN_WIDTH 176
#define SMALL_SCREEN_HEIGHT 220

#define QCAM_DEFAULT_DISPLAY_SIZE 11 //QVGA
#ifdef FEATURE_CAMERA7500
#define QCAM_WVGA_WIDTH         800
#define QCAM_VGA_WIDTH          640
#endif //FEATURE_CAMERA7500
//LCD offset to make small preview image display in the same coordinate as the UI.
//When FEATURE_UIONE_HDK is defined, the offset for annunciator is changed to 30.
#ifdef FEATURE_UIONE_HDK
#define QCAM_LCD_OFFSET 30
#else
#define QCAM_LCD_OFFSET 40
#endif

#endif //FEATURE_APP_QCAMERA

#endif //QCAMDEF_H

/*===========================================================================

FILE: QcamMenus.c

GENERAL DESCRIPTION
    This file provides functions to build and execute qcamera menus.

    Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/src/QcamMenus.c#7 $ $DateTime: 2009/04/15 00:19:29 $ $Author: saigopal $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/15/09  sai    Added USB option in save location menu
 04/02/09  KC     Added Party mode in Bestshot menu, corrected YCbCr featurisations
 02/04/09  KC     Added missing featurisations for Raw preview capture feature.
 12/17/08  KC     Added Raw preview capture feature.
 10/23/08  KC     Got the featurisations correct when YCbCr encoding is supported in6K
 09/10/08  KC     Removed 7K featurisation so that YCbCr option is displayed in delete all menu
 10/26/07  NC     Initial effect variable in case GetParm failure.
 10/17/07  KG     Calling ICAMERA_SETPARM with the current panoramic state.
 10/11/07  KV     Updated the pMe->m_bUpdatePreviewFrame to TRUE by enabling the
                  Panorama.
 10/04/07  SK     Placed some of the wide resolutions at proer place
 10/01/07  hap    Added changes needed to support YCbCr as a type of encoding.
 09/28/07  NC     Hide AFModeMetering menu if sensor does not support focus.
 09/24/07  KV     Disabling the frame overlay if enabled in panoramic mode.
 09/14/07  KD     Removed deprecated function calls
 09/06/07  KV     Make size to VGA if it is grether than VGA when Panorama is enabled.
 09/05/07  NC     Rewrote QcamMenus_BuildPropertiesMenu and fixed bugs for Panoramic mode
 09/04/07  KD     Workaround for 6k: panaromic resource items are not defined
 08/07/07  SK     UI Changes for Panoramic Stitch feature
 08/02/07  NC     Fixed potential compiling issue.
 05/22/07  SK     JPEG Decoder Resizing feature
 05/24/07  NC     Removed extra variables.
 05/10/07  KV     Added 1-Button Snapshot option to Setting->Properties menu.
 04/23/07  NC     Fixed GetParm on Effect buffer override issue.
 04/19/07  SK     UI Support for WVGA Display
 04/16/07  KV     Added ICamera_GetParm for Effects to update radio button selection.
 04/13/07  sk     Removed compilation warning
 04/06/07  SK     Added PosterSize and Aqua in Effects Menu 
 04/03/07  KV     Added QCAM_SELF_TIMER_TOOLONG for 25Seconds SnapShot Timer in UI.
 03/23/07  KV     Changed from default font size to dynamic font size to fit the pitch of the display.
 03/14/07  NC     Fixed potential override in building Delete All menu.
                  Enhanced WB automation error report.
 02/28/07  NC     Enabled direct mode support if FEATURE_CAMERA_LCD_DIRECT_MODE is defined.
 02/23/07  NC     Updated QcamMenus_ExecutePropertiesMenu for TV display setting.
 01/03/07  SK     Redesiging the Multiple Focus Rectangles
 11/30/06  ZL     Fixed the white bar on the bottom of the settings menu.
                  Reset soft menu style on the preview screen.
 11/22/06  ZL     Removed softkey menu bar icons at Qcam settings screen.
 11/17/06  NC     Fixed wrong size issue with Max size option.
 11/09/06  NC     Fixed compiler errors when FEATURE_CAMERA_MULTIPLE_DISPLAY is defined.
 11/02/06  NC     Blocked LCD size display option for 7K.
 10/31/06  NC     Fixed main menu title display issues
 10/03/06  NC     Used changable resource file name.
                  Added new items to serveral menus.
 09/12/06  NC     Not reset frame on orientation change.
 09/07/06  NC     Added automation support for Multishot feature.
 09/05/06  NC     Set condition FEATURE_CAMERA_SUPPORT_ICON_ARRAY for icon overlay.
 08/31/06  NC     Added Yes (all) to delete all menu for more than one delete option.
 08/29/06  NC     Fixed a bug in QcamMenus_ExecuteSizeMenu from #94.
 08/25/06  NC     Added UI support to simulate 2nd display.
 08/25/06  NC     Added UI support to enable LCD size display.
 08/15/06  NC     Remove Text option from Bestshot menu per servise change.
 08/15/06  NC     Changed ISO_HJR option condition to FEATURE_CAMERA_HJR.
 08/07/06  NC     Added FPS High end setting support.
 07/27/06  NC     Changed mutltishot menu call sequency and change menu function names.
                  Only enable bracketing option when mode is supported.
 07/18/06  NC     Fixed a bug in get EV from #87 and set checking for new features.
 07/14/06  NC     Added Night option for BSM.
 07/13/06  NC     Added new functions and variables to support multishot and bracketing mode.
 06/14/06  JR     Added support for FTL memory.
 06/08/06  NC     Builded and excuted Auto Frame Rate Low End menu based on fps list.
 05/19/06  NC     Enabled Auto Exposure Mode support for all targets.
 05/01/06  NC     Filter out unused features to save memory space.
 04/25/06  NC     Updated dynamic AUTO option for Antibanding menu.
 04/21/06  JR     Added sharpness icon and control code for SC1x.
 04/21/06  JR     Added Antibanding AUTO option for SC1x.
 04/07/06  NC     Added Luma Adaptation and Hand Jitter Reduction to property menu.
                  Changed "Auto Frame Rate" menu title to "Auto Frame Rate Low End".
 04/06/06  NC     Display hour glass on setting menus that have long start processing.
                  Removed ICAMERA_SetDisplaySize from ExecuteEncodingMenu
                  SetDisplaySize workaround for SDK.
 04/03/06  NC     Removed ICAMERA_SetDisplaySize from ExecuteSizeMenu per service change.
 03/06/06  NC     Enabled AF Mode menu support.
 01/18/06  JR     Added support for small screen sizes as on SC1x target.
 01/12/06  NC     Replace QCam_GetTotalPhotos with QCam_HasPhotos to reduce time.
 01/03/06  NC     Added menus and supports for ISO and AF Mode options
 12/29/05  NC     Built dynamic setting menu based on sensor/camera support info.
 12/15/05  NC     Fixed a shutter sound menu display update bug.
 12/14/05  NC     Set image size to VGA when swithing to PNG encoding type with larger size.
 11/30/05  NC     Clean up lint errors from Lint v8.0 and update MCT icon on build menu.
 10/24/05  NC     More lint errors clean up.
 10/20/05  NC     Fixed new lint errors and a bug from adding QCAM_SIZE_MAX.
 10/14/05  NC     Added QCAM_SIZE_MAX for actual sensor size option.
 10/13/05  NC     Added 3Mp, 4Mp and 5Mp size support.
 10/12/05  SM     automation support for prompt to save/delete
 10/04/05  NC     Set dynamic FPS menu based on sensor support info.
 09/16/05  NC     Limit PNG image size option to VGA or smaller.
 09/15/05  NC     Lint errors clean up.
 09/06/05  NC     Added NULL pointer checking to input parameters.
                  Added more checking to function QcamMenus_ValueInDataList.
                  Added error report to automation if failed to set parm.
 08/01/05  NC     Fixed display bug in quality menu.
 07/29/05  NC     Reset m_sShutterSoundFile if it is not existing.
 07/25/05  NC     Build dynamic size menu based on sensor support size.
 07/22/05  NC     Used Qcamera_GetPreviewRect to return m_rcPreView.
 07/08/05  NC     Added a new setting menu for JPEG Spec. option
 06/08/05  NC     Eliminated 30 FPS option now.
 06/07/05  NC     Added a new setting menu for Preview FPS option.
 05/24/05  NC     Update ICAMERA_SetParm() parm to avoid lint errors.
 05/20/05  NC     Added Raw Only option to Delete All menu and update the taken photo
                  number on select save location and delete all in case MMC card is
                  inserted/removed while camera is running.
 05/12/05  NC     Highlight old numbers in the entry field of Quality/file size input.
 05/12/05  NC     Added a new helper function QcamMenus_ValueInDataList().
 04/15/05  NC     Report error status of Delete All automation test.
 04/05/05  drl    Added Call to ICAMERA_SetDisplaySize prior to call to
                  ICAMERA_SetSize to indicate display image dimensions
                  should be the same as the size of the encoded image.
 04/04/05  NC     Added status report and direct simulation for Delete All automation.
 03/28/05  NC     Fixed bugs in delete menu logic.
 03/15/05  NC     Fixed a bug in execute red eye reduction function.
                  Set update frame flag on change size and orientation.
                  Added solarize option back to effect menu.
 03/14/05  NC     Featured out Brightness menu option.
 03/11/05  JN     Added nightshot mode in setting menu.
 03/09/05  NC     Removed FEATURE_CAMERA_CONTRAST and updated data types of some variable.
 02/25/05  JR     Fixed Lint errors
 01/17/05  NC     Code clean up. Updated function names.
 01/06/05  NC     Build dynamic DeleteAll Menu.
 11/19/04  NC     Added new function Qcamera_UpdateDeleteAllMenu()
 11/16/04  NC     Added new options for delete all.
 11/12/04  NC     Fixed a bug in QcamMenus_BuildSaveMenu.
 11/04/04  NC     Fixed bugs in displaying and executing delete all menu
 11/04/04  NC     Modified build and execute sound menu functions
 11/02/04  NC     Enabled and updated Focus and Exposure mode menu
                  Fixed a bug in building Delete All menu.
 10/18/04  NC     Added Auto Exposure menu build/execute functions
 10/14/04  NC     Added MEM mode and fixed some type cast bugs
 10/12/04  NC     Modified all executing menu functions with a new interface.
 09/21/04  NC     Update design and limit it for qcamera application.
 06/16/04  NC     Initial design of the share file QcamMenus.c.
 ============================================================================*/

//Deal with lint problems in BREW headers
//lint -e740, Unusual pointer cast for BREW macros
//lint -e611, Suspicious cast for BREW macros
//lint -emacro(570,IDISPLAY_ClearScreen)

//lint -e818, Pointer parameter 'pMe' could be declared as pointing to const
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "QcamMenus.h"

#if defined(FEATURE_APP_QCAMERA)
#include "QcamDisp.h"
/*===============================================================================
MENU ITEM STRUCTURES

// Array of uint16s used to create menus with multiple string options.
// First field is how many settings (Number of items)
// Second field is title
// rest are the settings (item IDs)
=============================================================================== */
//Size selection items and data list
//It is in portrait mode for QVGA screen, and landscape mode for small screen.
static uint16 PrevSizeIDList[] =
{
#ifndef FEATURE_CAMERA_WIDESIZE
  15,
#else 
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  IDS_SIZE,
  IDS_SIZE_MAX,
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

static uint32 PrevSizeDataList[] =
{
  (uint32)QCAM_SIZE_MAX,
  (uint32)QCAM_SIZE_QSXGA,
  (uint32)QCAM_SIZE_QXGAP,
  (uint32)QCAM_SIZE_QXGA,
  (uint32)QCAM_SIZE_UXGA,
  (uint32)QCAM_SIZE_SXGA,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  (uint32)QCAM_SIZE_4VGA,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  (uint32)QCAM_SIZE_XGA,
  (uint32)QCAM_SIZE_SVGA,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  (uint32)QCAM_SIZE_VGA,
  (uint32)QCAM_SIZE_CIF,
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  (uint32)QCAM_SIZE_QVGA,
  (uint32)QCAM_SIZE_QCIF,
  (uint32)QCAM_SIZE_QQVGA,
  (uint32)QCAM_SIZE_STAMP
};

//Self timer selection items and data lists
static uint16 SelfTimerIDList[] =
{
  5,
  IDS_SELFTIMER,
  IDS_SELFTIMER_OFF,
  IDS_SELFTIMER_SHORT,
  IDS_SELFTIMER_MED,
  IDS_SELFTIMER_LONG,
  IDS_SELFTIMER_TOOLONG
};

static uint32 SelfTimerDataList[] =
{
  0,
  QCAM_SELF_TIMER_SHORT,
  QCAM_SELF_TIMER_MED,
  QCAM_SELF_TIMER_LONG,
  QCAM_SELF_TIMER_TOOLONG
};

//Slidshow Speed selection items and data lists
static uint16 SlideshowSpeedIDList[] =
{
  3,
  IDS_SLIDESHOW_SPEED,
  IDS_SLIDESHOW_SPEED_FAST,
  IDS_SLIDESHOW_SPEED_MED,
  IDS_SLIDESHOW_SPEED_SLOW
};

static uint32 SlideshowSpeedDataList[] =
{
  QCAM_SLIDESHOW_FAST,
  QCAM_SLIDESHOW_MED,
  QCAM_SLIDESHOW_SLOW
};

// Encoding selection items and data lists
static uint16 EncodingIDList[] =
{
#if defined (FEATURE_CAMERA_ENCODE_YCBCR) && defined (FEATURE_CAMERA_RAW_PREVIEW_CAPTURE)
#error code not present
#elif !defined (FEATURE_CAMERA_ENCODE_YCBCR) && !defined (FEATURE_CAMERA_RAW_PREVIEW_CAPTURE)
  4,
#else
  5,
#endif 
  IDS_ENCODING,
  IDS_ENCODING_JPEG,
  IDS_ENCODING_PNG,
  IDS_ENCODING_RAW,
  IDS_ENCODING_MEM,
#if defined (FEATURE_CAMERA_ENCODE_YCBCR)
  IDS_ENCODING_YCBCR,
#endif //FEATURE_CAMERA_ENCODE_YCBCR
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif
};

static uint32 EncodingDataList[] =
{
  (uint32)QCAM_ENCODING_JPEG,
  (uint32)QCAM_ENCODING_PNG,
  (uint32)QCAM_ENCODING_RAW,
  (uint32)QCAM_ENCODING_MEM,
#if defined (FEATURE_CAMERA_ENCODE_YCBCR)
  (uint32)QCAM_ENCODING_YCBCR,
#endif //FEATURE_CAMERA_ENCODE_YCBCR
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif

};

// JPEG Spec. selection items and data lists
static uint16 JPEGSpecIDList[] =
{
  3,
  IDS_JPEG_SPEC,
  IDS_JPEG_QUALITY,
  IDS_JPEG_FILESIZE,
  IDS_JPEG_BOTH
};

static uint32 JPEGSpecDataList[] =
{
  (uint32)QCAM_JPEG_QUALITY,
  (uint32)QCAM_JPEG_FILESIZE,
  (uint32)QCAM_JPEG_BOTH
};

// Quality selection items and data lists
static uint16 CamQualityIDList[] =
{
  4,
  IDS_QUALITY,
  IDS_QUALITY_LOW,
  IDS_QUALITY_MEDIUM,
  IDS_QUALITY_HIGH,
  IDS_QUALITY_PERCENT
};

static uint32 QualityDataList[] =
{
  (uint32)QCAM_QUALITY_LOW,
  (uint32)QCAM_QUALITY_MED,
  (uint32)QCAM_QUALITY_HIGH,
  (uint32)QCAM_QUALITY_PERCENT
};

//White balance selection items and data list
static uint16 CamWhiteBalanceIDList[] =
{
  5,
  IDS_WHITEBALANCE,
  IDS_WB_AUTO,
  IDS_WB_FLOURESCENT,
  IDS_WB_INCANDESCENT,
  IDS_WB_DAYLIGHT,
  IDS_WB_CLOUDY
};

static uint32 WhiteBalanceDataList[] =
{
  CAM_WB_AUTO,
  CAM_WB_FLUORESCENT,
  CAM_WB_INCANDESCENT,
  CAM_WB_DAYLIGHT,
  CAM_WB_CLOUDY_DAYLIGHT
};

// Effects selection items and data list
static uint16 CamEffectsIDList[] =
{
#ifdef FEATURE_CAMERA7500
   7,
#else //FEATURE_CAMERA7500
   5,
#endif //FEATURE_CAMERA7500
   IDS_EFFECTS,
   IDS_EFFECT_OFF,
   IDS_EFFECT_MONO,
   IDS_EFFECT_SEPIA,
   IDS_EFFECT_NEGATIVE,
   IDS_EFFECT_SOLARIZE,
#ifdef FEATURE_CAMERA7500
   IDS_EFFECT_POSTERSIZE,
   IDS_EFFECT_AQUA
#endif //FEATURE_CAMERA7500
};

static uint32 EffectsDataList[] =
{
  CAM_EFFECT_OFF,
  CAM_EFFECT_MONO,
  CAM_EFFECT_SEPIA,
  CAM_EFFECT_NEGATIVE,
  CAM_EFFECT_SOLARIZE,
#ifdef FEATURE_CAMERA7500
  CAM_EFFECT_POSTERIZE,
  CAM_EFFECT_AQUA
#endif //FEATURE_CAMERA7500
};

// Properties selection items and bits list
static uint16 PropertiesIDList[] =
{
  4,
  IDS_PROPERTIES,
  IDS_SAVE_PROMPT,
  IDS_ORIENTATION_PORTRAIT,
  IDS_DIR_SORT_ASCENDING,
  IDS_PHOTO_SORT_ASCENDING,
};

static uint32 PropertiesList[] =
{
  QCAM_PROMPT_SAVE,
  QCAM_PORTRAIT,
  QCAM_DIR_ASCENDING,
  QCAM_PHOTO_ASCENDING,
};

// Max. # of pictures selection items and data list
static uint16 MaxPicsIDList[] =
{
  4,
  IDS_MAX_PICS,
  IDS_MAX_PICS_10,
  IDS_MAX_PICS_20,
  IDS_MAX_PICS_50,
  IDS_MAX_PICS_NO_LIMIT
};

static uint32 MaxPicsDataList[] =
{
  QCAM_MAX_PICS_10,
  QCAM_MAX_PICS_20,
  QCAM_MAX_PICS_50,
  QCAM_MAX_PICS_NO_LIMIT
};

// Reflect selection items list
static uint16 ReflectIDList[] =
{
  3,
  IDS_REFLECT,
  IDS_OFF,
  IDS_QCAMERA_MIRROR_REFLECT,
  IDS_QCAMERA_WATER_REFLECT
};

// Flash selection items and data list
static uint16 FlashIDList[] =
{
  5,
  IDS_LED_FLASH,
  IDS_QCAMERA_OFF,
  IDS_QCAMERA_AUTO,
  IDS_QCAMERA_LOW,
  IDS_QCAMERA_MED,
  IDS_QCAMERA_HIGH
};

#ifdef FEATURE_CAMERA7500
// Strobe Flash selection items and data list
static uint16 StrobeFlashIDList[] =
{
  5,
  IDS_STROBE_FLASH,
  IDS_STROBE_QCAMERA_OFF,
  IDS_STROBE_QCAMERA_AUTO,
  IDS_STROBE_QCAMERA_LOW,
  IDS_STROBE_QCAMERA_MED,
  IDS_STROBE_QCAMERA_HIGH
};
#endif //FEATURE_CAMERA7500

static uint32 FlashDataList[] =
{
  CAM_FLASH_OFF,
  CAM_FLASH_AUTO,
  CAM_FLASH_LOW,
  CAM_FLASH_MEDIUM,
  CAM_FLASH_HIGH
};

//Save Location selection items and data list
static uint16 SaveIDList[] =
{
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  4,
#else
  3,
#endif
  IDS_SAVE_LOC,
  IDS_FLASH_LOC,
  IDS_MMC_LOC,
  IDS_FTL_LOC
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  ,IDS_USB_LOC
#endif
};

// Delete all selection items list
static uint16 DelAllIDList[] =
{
#if defined (FEATURE_EFS_HOTPLUG_USB_HOST_MS)
  8,
#else
  7,
#endif //FEATURE_EFS_HOTPLUG_USB_HOST_MS
  IDS_DELETEALL,
  IDS_YES,
  IDS_YES_FLASH,
  IDS_YES_MMC,
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  IDS_YES_USB,
#endif
  IDS_YES_RAW,
  IDS_YES_FTL,
#if defined (FEATURE_CAMERA_ENCODE_YCBCR)
  IDS_YES_YCBCR,
#endif //FEATURE_CAMERA_ENCODE_YCBCR
  IDS_NO
};

//GPS selection items and data list
static uint16 GPSIDList[] =
{
  3,
  IDS_PROPERTIES,
  IDS_GPS_DISABLE,
  IDS_GPS_ENABLE_NEW,
  IDS_GPS_ENABLE_PREV
};

static uint32 GPSDataList[] =
{
  (uint32)QCAM_GPS_DISABLED,
  (uint32)QCAM_GPS_ENABLED,
  (uint32)QCAM_GPS_DATA_VALID
};

//Antibanding selection items and data list
static uint16 AntiBandingIDList[] =
{
  4,
  IDS_ANTIBANDING,
  IDS_ANTIBANDING_OFF,
  IDS_ANTIBANDING_60HZ,
  IDS_ANTIBANDING_50HZ,
  IDS_ANTIBANDING_AUTO
};

static uint32 AntiBandingDataList[] =
{
  CAM_ANTIBANDING_OFF,
  CAM_ANTIBANDING_60HZ,
  CAM_ANTIBANDING_50HZ,
  CAM_ANTIBANDING_AUTO
};

//Redeye Reduction selection items and data list
static uint16 RedeyeReduceIDList[] =
{
  2,
  IDS_REDEYE_REDUCE,
  IDS_REDEYEREDUCE_OFF,
  IDS_REDEYEREDUCE_ON
};

//Nightshot mode selection items and data list
static uint16 NightshotModeIDList[] =
{
  2,
  IDS_NIGHTSHOT_MODE,
  IDS_NIGHTSHOT_DISABLE,
  IDS_NIGHTSHOT_ENABLE
};

//Auto Exposure Algorithm selection items and data list
static uint16 AutoExposureIDList[] =
{
  3,
  IDS_AUTO_EXPOSURE,
  IDS_AE_FRAME_AVERAGE,
  IDS_AE_CENTER_WEIGHTED,
  IDS_AE_SPOT_METERING
};

static uint32 AutoExposureDataList[] =
{
  (uint32)QCAM_AEC_FRAME_AVERAGE,
  (uint32)QCAM_AEC_CENTER_WEIGHTED,
  (uint32)QCAM_AEC_SPOT_METERING
};

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
// Frame Overlay selection items and data list
static uint16 FrameOverlayIDList[] =
{
  3,
  IDS_FRAME_OVERLAY,
  IDS_ICONS_NO,
  IDS_ICONS_PREVIEW,
  IDS_ICONS_ENCODE
};

// Frame Overlay selection items and data list
static uint32 FrameOverlayList[] =
{
  (uint32)QCAM_ICONS_NO,
  (uint32)QCAM_ICONS_PREVIEW_ONLY,
  (uint32)QCAM_ICONS_PREVIEW_ENCODE
};
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY

#ifndef FEATURE_CAMERA_V2
//Preview PFS selection items and data list
static uint16 PreviewFPSIDList[] =
{
  4,
  IDS_PREVIEWFPS,
  IDS_PREVIEWFPS_30000,
  IDS_PREVIEWFPS_15000,
  IDS_PREVIEWFPS_7500,
  IDS_PREVIEWFPS_3750
};

static uint16 PreviewHighFPSIDList[] =
{
  4,
  IDS_PREVIEWFPS_HIGH,
  IDS_HIGHFPS_30000,
  IDS_HIGHFPS_15000,
  IDS_HIGHFPS_7500,
  IDS_HIGHFPS_3750
};

static uint32 PreviewFPSDataList[] =
{
  QCAM_FPS_30000,
  QCAM_FPS_15000,
  QCAM_FPS_7500,
  QCAM_FPS_3750
};

//ISO items and data list
static uint16 ISOIDList[] =
{
  6,
  IDS_ISO,
  IDS_ISO_AUTO,
  IDS_ISO_HJR,
  IDS_ISO_100,
  IDS_ISO_200,
  IDS_ISO_400,
  IDS_ISO_800
};

static uint32 ISODataList[] =
{
  QCAM_ISO_0,
  QCAM_ISO_1,
  QCAM_ISO_100,
  QCAM_ISO_200,
  QCAM_ISO_400,
  QCAM_ISO_800
};

//AF mode items and data list
static uint16 AFModeIDList[] =
{
  2,
  IDS_AF_MODE,
  IDS_AF_NORMAL,
  IDS_AF_MACRO
};

static uint32 AFModeDataList[] =
{
  CAM_AF_MODE_NORMAL,
  CAM_AF_MODE_MACRO
};

#ifdef FEATURE_CAMERA7500 
//AF mode p2 items and data list
static uint16 AFModeMeteringIDList[] =
{
  4,
  IDS_AF_MODE_METERING,
  IDS_AF_METERING_AUTO,
  IDS_AF_METERING_CENTER_SPOT,
  IDS_AF_METERING_CENTER_WEIGHTED,
  IDS_AF_METERING_AVERAGE,
};

static uint32 AFModeMeteringDataList[] =
{
  CAM_AF_METER_AUTO,
  CAM_AF_METER_CENTER_SPOT,
  CAM_AF_METER_CENTER_WEIGHTED,
  CAM_AF_METER_AVERAGE 
};

#endif //FEATURE_CAMERA7500 

// Bracketing mode selection items
static uint16 MultishotIDList[] =
{
  3,
  IDS_MULTISHOT,
  IDS_MULTISHOT_OFF,
  IDS_MULTISHOT_ON,
  IDS_MULTISHOT_AE
};

static uint32 MultishotDataList[] =
{
  QCAM_MULTISHOT_OFF,
  QCAM_MULTISHOT_ON,
  QCAM_MULTISHOT_AE
};

// Bestshot selection items
static uint16 BestshotIDList[] =
{
  13,
  IDS_BEST_SHOT,
  IDS_BESTSHOT_NORMAL,
  IDS_BESTSHOT_LANDSCAPE,
  IDS_BESTSHOT_SNOW,
  IDS_BESTSHOT_BEACH,
  IDS_BESTSHOT_SUNSET,
  IDS_BESTSHOT_NIGHT,
  IDS_BESTSHOT_PORTRAIT,
  IDS_BESTSHOT_BACKLIGHT,
  IDS_BESTSHOT_SPORTS,
  IDS_BESTSHOT_ANTISHAKE,
  IDS_BESTSHOT_FLOWERS,
  IDS_BESTSHOT_CANDLE_LIGHT,
  IDS_BESTSHOT_PARTY
};

static uint32 BestshotDataList[] =
{
  CAM_BESTSHOT_OFF,
  CAM_BESTSHOT_LANDSCAPE,
  CAM_BESTSHOT_SNOW,
  CAM_BESTSHOT_BEACH,
  CAM_BESTSHOT_SUNSET,
  CAM_BESTSHOT_NIGHT,
  CAM_BESTSHOT_PORTRAIT,
  CAM_BESTSHOT_BACKLIGHT,
  CAM_BESTSHOT_SPORTS,
  CAM_BESTSHOT_ANTISHAKE,
  CAM_BESTSHOT_FLOWERS,
  CAM_BESTSHOT_CANDLE_LIGHT,
  CAM_BESTSHOT_PARTY
};
#endif //FEATURE_CAMERA_V2

#ifdef FEATURE_CAMERA_HISTOGRAM
static uint16 HistogramIDList[] =
{
  4,
  IDS_HISTOGRAM,
  IDS_HISTOGRAM_OFF,
  IDS_HISTOGRAM_Y,
  IDS_HISTOGRAM_RGB,
  IDS_HISTOGRAM_YRGB
};

static uint32 HistogramDataList[] =
{
  CAM_HISTOGRAM_OFF,
  CAM_HISTOGRAM_LUMA,
  CAM_HISTOGRAM_RGB,
  CAM_HISTOGRAM_BOTH
};
#endif //FEATURE_CAMERA_HISTOGRAM

//Take picture icon menu items list
static CAMICONMENUID TakePicIDList[] =
{
  {IDS_TAKE_PIC,    IDB_IMAGE_ICON},
  {IDS_ZOOM,        IDB_ZOOM},
#ifdef FEATURE_CAMERA_BRIGHTNESS
  {IDS_BRIGHTNESS,  IDB_BRIGHTNESS},
#else
  {IDS_EV,          IDB_EV},
#endif //FEATURE_CAMERA_BRIGHTNESS
  {IDS_FOCUS,       IDB_FOCUS},
  {IDS_CONTRAST,    IDB_CONTRAST},
#if defined (FEATURE_CAMERA_V2) || defined (FEATURE_CAMERA7500)
  {IDS_SHARPNESS,   IDB_IMAGE_SHARPNESS},
#endif //FEATURE_CAMERA_V2 || FEATURE_CAMERA7500
#ifdef FEATURE_CAMERA7500
  {IDS_HUE,         IDB_IMAGE_HUE},
  {IDS_SATURATION,  IDB_IMAGE_SATURATION},
  {IDS_EFFECTS,     IDB_EFFECTS},
#endif //FEATURE_CAMERA7500
#ifdef FEATURE_CAMERA_HISTOGRAM
  {IDS_HISTOGRAM,   IDB_HISTOGRAM},
#endif
  {IDS_ALBUM,       IDB_ALBUM},
  {IDS_SLIDESHOW,   IDB_SLIDESHOW},
  {IDS_FRAME,       IDB_FRAME},
#ifndef FEATURE_CAMERA_V2
  {IDS_GPS_SETUP,   IDB_GPS_INACTIVE},
#endif
  {IDS_SETTINGS,    IDB_PROPERTIES},
#ifdef FEATURE_APP_QCAMCORDER
  {IDS_QCAMCORDER,  IDB_QCAMCORDER_ICON},
#endif
#ifdef FEATURE_CAMERA_MOBICAT
  {IDS_MCT,         IDB_MCT_ON},
#endif
#ifndef FEATURE_CAMERA_V2
  {IDS_SIMULATE_2NDLCD,   IDB_2ND_DISP}
#endif
};

/**************** Help functions *******************/
static void QcamMenus_BuildDialog(QCam *pMe, uint16 DlgId, uint16 selId);
static uint16 QcamMenus_BuildSubSizeIdList(QCam *pMe, uint16 *pIdList, uint16 size);
static void QcamMenus_BuildSubSizeMenu(QCam *pMe);
/**************** Build menu functions for Camera application *******************/
static void QcamMenus_BuildSubMenu(QCam *pMe, ListMenuType type, const uint16 *IdList, const uint32 *DataList, uint32 sel);
static void QcamMenus_BuildMaxPicsMenu(QCam *pMe);
static void QcamMenus_BuildSaveMenu(QCam *pMe);

static void QcamMenus_BuildSizeMenu(QCam *pMe);
static void QcamMenus_BuildSelfTimerSettingsMenu(QCam *pMe);
static void QcamMenus_BuildSlideShowSpeedMenu(QCam *pMe);
static void QcamMenus_BuildEncodingMenu(QCam *pMe);
static void QcamMenus_BuildPropertiesMenu(QCam *pMe);

static void QcamMenus_BuildWhitebalanceMenu(QCam *pMe);
static void QcamMenus_BuildEffectsMenu(QCam *pMe);
static void QcamMenus_BuildDeleteAllMenu(QCam *pMe);
static void QcamMenus_BuildReflectMenu(QCam *pMe);
static void QcamMenus_BuildFlashMenu(QCam *pMe);
static void QcamMenus_BuildQualityMenu(QCam *pMe);
static void QcamMenus_BuildAntibandingMenu(QCam *pMe);
static void QcamMenus_BuildRedeyeReduceMenu(QCam *pMe);
static void QcamMenus_BuildNightshotModeMenu(QCam *pMe);
static void QcamMenus_BuildJPEGSpecMenu(QCam *pMe);
static void QcamMenus_BuildAutoExposureMenu(QCam *pMe);
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
static void QcamMenus_BuildFrameOverlayMenu(QCam *pMe);
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
#ifndef FEATURE_CAMERA_V2
static void QcamMenus_BuildPreviewFPSMenu(QCam *pMe, uint16 titleID, uint32 nCurVal);
static void QcamMenus_BuildHighFPSMenu(QCam *pMe);
static void QcamMenus_BuildLowFPSMenu(QCam *pMe);
static void QcamMenus_BuildISOMenu(QCam *pMe);
static void QcamMenus_BuildAFModeMenu(QCam *pMe);
static void QcamMenus_BuildMultishotMenu(QCam *pMe);
static void QcamMenus_BuildBestshotMenu(QCam *pMe);
#ifdef FEATURE_CAMERA7500
static void QcamMenus_BuildAFModeMeteringMenu(QCam *pMe);
static void QcamMenus_BuildStrobeFlashMenu(QCam *pMe);
#endif //#FEATURE_CAMERA7500
#endif //FEATURE_CAMERA_V2
/**************** Execut menu functions for Camera application *******************/
static void QcamMenus_ExecuteSelfTimerMenu(QCam* pMe);
static void QcamMenus_ExecuteSlideshowSpeedMenu(QCam* pMe);
static void QcamMenus_ExecuteEncodingMenu(QCam* pMe);
static void QcamMenus_ExecuteWhitebalanceMenu(QCam* pMe);
static void QcamMenus_ExecuteEffectsMenu(QCam* pMe);
static void QcamMenus_ExecuteMaxPicsMenu(QCam* pMe);
static void QcamMenus_ExecuteSoundMenu(QCam* pMe);
static void QcamMenus_ExecuteSizeMenu(QCam* pMe);
static void QcamMenus_ExecuteDeleteAllMenu(QCam* pMe);
static void QcamMenus_ExecuteSaveLocMenu(QCam* pMe);
static void QcamMenus_ExecutePropertiesMenu(QCam* pMe);
static void QcamMenus_ExecuteFlashMenu(QCam* pMe);
static void QcamMenus_ExecuteReflectMenu(QCam* pMe);
static void QcamMenus_ExecuteQualityMenu(QCam* pMe);
static void QcamMenus_ExecuteAntibandingMenu(QCam* pMe);
static void QcamMenus_ExecuteRedeyeReduceMenu(QCam* pMe);
static void QcamMenus_ExecuteNightshotModeMenu(QCam *pMe);
static void QcamMenus_ExecuteJPEGSpecMenu(QCam *pMe);
static void QcamMenus_ExecuteAutoExposureMenu(QCam* pMe);
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
static void QcamMenus_ExecuteFrameOverlayMenu(QCam* pMe);
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
#ifndef FEATURE_CAMERA_V2
static void QcamMenus_ExecutePreviewFPSMenu(QCam* pMe);
static void QcamMenus_ExecuteLowFPSMenu(QCam *pMe);
static void QcamMenus_ExecuteHighFPSMenu(QCam *pMe);
static void QcamMenus_ExecuteISOMenu(QCam *pMe);
static void QcamMenus_ExecuteAFModeMenu(QCam *pMe);
static void QcamMenus_ExecuteMultishotMenu(QCam *pMe);
static void QcamMenus_ExecuteBestshotMenu(QCam *pMe);
#ifdef FEATURE_CAMERA7500
static void QcamMenus_ExecuteAFModeMeteringMenu(QCam *pMe);
static void QcamMenus_ExecuteStrobeFlashMenu(QCam* pMe);
#endif //FEATURE_CAMERA7500
#endif //#ifndef FEATURE_CAMERA_V2

// Setting menu building function table
static const CSettingMenuHandler CSettingMenuHandlers[] =
{
  {IDS_SIZE,            QcamMenus_BuildSizeMenu},
  {IDS_SELFTIMER,       QcamMenus_BuildSelfTimerSettingsMenu},
  {IDS_SLIDESHOW_SPEED, QcamMenus_BuildSlideShowSpeedMenu},
  {IDS_ENCODING,        QcamMenus_BuildEncodingMenu},
  {IDS_AUTO_EXPOSURE,   QcamMenus_BuildAutoExposureMenu},
  {IDS_WHITEBALANCE,    QcamMenus_BuildWhitebalanceMenu},
  {IDS_EFFECTS,         QcamMenus_BuildEffectsMenu},
  {IDS_SHUTTER_SOUND,   QcamMenus_BuildSoundMenu},
  {IDS_PROPERTIES,      QcamMenus_BuildPropertiesMenu},
  {IDS_MAX_PICS,        QcamMenus_BuildMaxPicsMenu},
  {IDS_REFLECT,         QcamMenus_BuildReflectMenu},
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  {IDS_FRAME_OVERLAY,   QcamMenus_BuildFrameOverlayMenu},
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  {IDS_MEM_USAGE,       Qcamera_MemUsage},
  {IDS_LED_FLASH,       QcamMenus_BuildFlashMenu},
#ifdef FEATURE_CAMERA7500
  {IDS_STROBE_FLASH,    QcamMenus_BuildStrobeFlashMenu},
#endif //FEATURE_CAMERA7500
  {IDS_REDEYE_REDUCE,   QcamMenus_BuildRedeyeReduceMenu},
  {IDS_SAVE_LOC,        QcamMenus_BuildSaveMenu},
  {IDS_DELETEALL,       QcamMenus_BuildDeleteAllMenu},
  {IDS_ANTIBANDING,     QcamMenus_BuildAntibandingMenu},
  {IDS_NIGHTSHOT_MODE,  QcamMenus_BuildNightshotModeMenu},
#ifndef FEATURE_CAMERA_V2
  {IDS_PREVIEWFPS,      QcamMenus_BuildLowFPSMenu},
  {IDS_PREVIEWFPS_HIGH, QcamMenus_BuildHighFPSMenu},
  {IDS_ISO,             QcamMenus_BuildISOMenu},
  {IDS_AF_MODE,         QcamMenus_BuildAFModeMenu},
#ifdef FEATURE_CAMERA7500
  {IDS_AF_MODE_METERING,QcamMenus_BuildAFModeMeteringMenu},
#endif //FEATURE_CAMERA7500
  {IDS_MULTISHOT,       QcamMenus_BuildMultishotMenu},
  {IDS_BEST_SHOT,       QcamMenus_BuildBestshotMenu}
#endif //#ifndef FEATURE_CAMERA_V2
};

// Setting menu executing function table
static const CExecMenuHandler CExecMenuHandlers[] =
{
  {QCAM_MENU_GPS,             NULL},
  {QCAM_MENU_SIZE,            QcamMenus_ExecuteSizeMenu},
  {QCAM_MENU_SELFTIMER,       QcamMenus_ExecuteSelfTimerMenu},
  {QCAM_MENU_SLIDESHOW_SPEED, QcamMenus_ExecuteSlideshowSpeedMenu},
  {QCAM_MENU_ENCODING,        QcamMenus_ExecuteEncodingMenu},
  {QCAM_MENU_WHITEBALANCE,    QcamMenus_ExecuteWhitebalanceMenu},
  {QCAM_MENU_EFFECTS,         QcamMenus_ExecuteEffectsMenu},
  {QCAM_MENU_SOUND,           QcamMenus_ExecuteSoundMenu},
  {QCAM_MENU_PROPERTIES,      QcamMenus_ExecutePropertiesMenu},
  {QCAM_MENU_MAX_PICS,        QcamMenus_ExecuteMaxPicsMenu},
  {QCAM_MENU_REFLECT,         QcamMenus_ExecuteReflectMenu},
  {QCAM_MENU_FLASH,           QcamMenus_ExecuteFlashMenu},
#ifdef FEATURE_CAMERA7500
  {QCAM_MENU_STROBEFLASH,     QcamMenus_ExecuteStrobeFlashMenu},
#endif //FEATURE_CAMERA7500
  {QCAM_MENU_REDEYE,          QcamMenus_ExecuteRedeyeReduceMenu},
  {QCAM_MENU_SAVE,            QcamMenus_ExecuteSaveLocMenu},
  {QCAM_MENU_DELETEALL,       QcamMenus_ExecuteDeleteAllMenu},
  {QCAM_MENU_QUALITY,         QcamMenus_ExecuteQualityMenu},
  {QCAM_MENU_ANTIBANDING,     QcamMenus_ExecuteAntibandingMenu},
  {QCAM_MENU_NIGHTSHOT_MODE,  QcamMenus_ExecuteNightshotModeMenu},
  {QCAM_MENU_JPEGSPEC,        QcamMenus_ExecuteJPEGSpecMenu},
  {QCAM_MENU_AUTOEXPOSURE,    QcamMenus_ExecuteAutoExposureMenu},
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  {QCAM_MENU_FRAME_OVERLAY,   QcamMenus_ExecuteFrameOverlayMenu},
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
#ifndef FEATURE_CAMERA_V2
  {QCAM_MENU_PREVIEWFPS_LOW,  QcamMenus_ExecuteLowFPSMenu},
  {QCAM_MENU_PREVIEWFPS_HIGH, QcamMenus_ExecuteHighFPSMenu},
  {QCAM_MENU_ISO,             QcamMenus_ExecuteISOMenu},
  {QCAM_MENU_AFMODE,          QcamMenus_ExecuteAFModeMenu},
#ifdef FEATURE_CAMERA7500
  {QCAM_MENU_AFMODEMETERING,  QcamMenus_ExecuteAFModeMeteringMenu},
#endif //FEATURE_CAMERA7500
  {QCAM_MENU_MULTISHOT,       QcamMenus_ExecuteMultishotMenu},
  {QCAM_MENU_BESTSHOT,        QcamMenus_ExecuteBestshotMenu}
#endif //#ifndef FEATURE_CAMERA_V2
};

/* ==========================================================================
PUBLIC FUNCTIONS
============================================================================= */

/*===========================================================================
FUNCTION QcamMenus_ValueInDataList

  DESCRIPTION
    This function checks if the value is in the value list

  PARAMETERS:
    val - value to be checked
    valId - Menu id to find the data list.

  RETURN VALUE:
    TRUE if the value is found in the list
===========================================================================*/
boolean QcamMenus_ValueInDataList(uint32 val, uint16 valId)
{
  const uint32 *valList;
  int size;
  int i;

  //current only support two items. can enhance to more.
  switch (valId)
  {
  case QCAM_MENU_SELFTIMER:
    valList = SelfTimerDataList;
    size = SelfTimerIDList[0];
    break;
  case QCAM_MENU_SLIDESHOW_SPEED:
    valList = SlideshowSpeedDataList;
    size = SlideshowSpeedIDList[0];
    break;
  case QCAM_MENU_WHITEBALANCE:
    valList = WhiteBalanceDataList;
    size = CamWhiteBalanceIDList[0];
    break;
  case QCAM_MENU_MAX_PICS:
    valList = MaxPicsDataList;
    size = MaxPicsIDList[0];
    break;
  case QCAM_MENU_EFFECTS:
    valList = EffectsDataList;
    size = CamEffectsIDList[0];
    break;
  case QCAM_MENU_ENCODING:
    valList = EncodingDataList;
    size = EncodingIDList[0];
    break;
  default:
    return FALSE;
  }

  for (i = 0; i < size; i++)
  {
    if (val == valList[i])
      return TRUE;
  }
  return FALSE;
}

#ifdef FEATURE_CAMERA7500
/*=====================================================================
FUNCTION QcamMenus_GetItemDataPerStep

  DESCRIPTION: This function return a new IDS and data per step (+1 or -1)
               for the item.

  PARAMETERS:
    itemId    - IDS of selected item
    step      - step direction: +1 down, -1 up, 0 current
    pSelId    - pointer to the new option IDS
    pSelData  - pointer to the old/new option data

  RETURN VALUE:
    int  - error code defined in AEEError.h
======================================================================*/
uint16 QcamMenus_GetItemDataPerStep(uint16 itemId, int step, uint32 *pSelData)
{
  uint16 sel = 0;
  const uint32 *valList;
  const uint16 *idList;
  int size;
  int i;

  if (!pSelData)
    return 0;

  switch (itemId)
  {
  case IDS_EFFECTS:
    idList = CamEffectsIDList;
    valList = EffectsDataList;
    size = CamEffectsIDList[0];
    break;
#ifdef FEATURE_CAMERA_HISTOGRAM
  case IDS_HISTOGRAM:
    idList = HistogramIDList;
    valList = HistogramDataList;
    size = HistogramIDList[0];
    break;
#endif //FEATURE_CAMERA_HISTOGRAM
  default:
    return 0;
  }

  //Search IDS from the selected data.
  for (i = 0; i < size; i++)
  {
    if (*pSelData == valList[i])
    {
      i += step;
      if (i < 0)
        i = size - 1;
      else if (i == size)
        i = 0;

      *pSelData = valList[i];
      sel = idList[i + 2];
      return sel;
    }
  }
  return 0;
}
#endif //FEATURE_CAMERA7500

/*===========================================================================
  FUNCTION: QcamMenus_DoSettingsMenu

  DESCRIPTION:
    This function build the selected setting menu.

  PARAMETERS:
    pMe:    QCam data pointer
    item:   direction of setting menu icon moves (-1, 0, 1)

  DEPENDENCIES:
    NA

  RETURN VALUE
    False if failed to build the menu
    True if successful

  SIDE EFFECTS:
  none
===========================================================================*/
boolean QcamMenus_DoSettingsMenu(QCam *pMe, int item)
{
  uint16 selection;
  int32 index;
  uint16 count, i;

  if (!pMe)
    return FALSE;

  //Get current focused item id
  selection = IMENUCTL_GetSel(pMe->m_pISoftMenu);
  //Get index of the item from dwData
  if (!IMENUCTL_GetItemData(pMe->m_pISoftMenu, selection, (uint32 *)&index))
  {
    //if for some how has no focus, return.
    return FALSE;
  }
  //Get total number of menu items
  count = (uint16)IMENUCTL_GetItemCount(pMe->m_pISoftMenu);
  //move index to current selection item
  index += item;
  //circulate menu movement
  if (index < 0)
  {
    index = count - 1;
  }
  else if (index >= count)
  {
    index = 0;
  }

  //Get the selected item id
  selection = IMENUCTL_GetItemID(pMe->m_pISoftMenu, index);
  //Call the right build menu function from the map table
  if (CSettingMenuHandlers[index].MenuID == selection)
  {
    CSettingMenuHandlers[index].MenuBuildFunc(pMe);
  }
  else
  {
    for (i = 0; i < ARR_SIZE(CSettingMenuHandlers); i++)
    {
      if (CSettingMenuHandlers[i].MenuID == selection)
      {
        CSettingMenuHandlers[i].MenuBuildFunc(pMe);
        break;
      }
    }
  }

  //Update screen
  (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
  return TRUE;
}

/*===========================================================================
  FUNCTION: QcamMenus_ExecuteActiveMenu

  DESCRIPTION:
    This function call the menu executive function for the active menu from
    the map table.

  PARAMETERS:
    pMe:          QCam data pointer

  DEPENDENCIES:
    NA

  RETURN VALUE
    NA

  SIDE EFFECTS:
  none
===========================================================================*/
void QcamMenus_ExecuteActiveMenu(QCam* pMe)
{
  uint16 i;
  if (!pMe)
    return;

  for (i = 0; i < ARR_SIZE(CExecMenuHandlers); i++)
  {
    if (CExecMenuHandlers[i].dwActiveMenu == pMe->m_activeMenu)
    {
      if(CExecMenuHandlers[i].MenuExecFunc)
        CExecMenuHandlers[i].MenuExecFunc(pMe);
      break;
    }
  }
}

/*===========================================================================
  FUNCTION: QcamMenus_UpdateDeleteAllMenu

  DESCRIPTION:
    This is a wrap function to update the radio icon of the delete menu.

  PARAMETERS:
    pMe:  QCam data pointer
    item  step increase or decrease number

  DEPENDENCIES:
    NA

  RETURN VALUE
    NA

  SIDE EFFECTS:
  none
===========================================================================*/
void QcamMenus_UpdateDeleteAllMenu(QCam *pMe, int item)
{
  uint32 sel;
  uint16 focus;

  if (!pMe)
    return;

  focus = IMENUCTL_GetSel(pMe->m_pIMenu); //current focus
  if (IMENUCTL_GetItemData(pMe->m_pIMenu, focus, &sel)) //current selected item
  {
    sel += (uint32)item; //off set to the old item
    //Update radio icon on new focus
    QCam_UpdateRadioFocus(pMe->m_pIMenu, DelAllIDList, &sel, pMe->m_pResFile);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
}

/*===========================================================================
FUNCTION QcamMenus_BuildDialog

  DESCRIPTION
    This function create input dialog and set highlight to input field.

  PARAMETERS:
    pMe   - pointer to QCam data struct
    DlgId - Dialog id
    selId - Id of the text control to be focused

  RETURN VALUE:
    NONE
===========================================================================*/
static void QcamMenus_BuildDialog(QCam *pMe, uint16 DlgId, uint16 selId)
{
  int result;

  if (!pMe)
    return;

  result = ISHELL_CreateDialog(pMe->a.m_pIShell, pMe->m_pResFile, DlgId, NULL);
  if (result == AEE_SUCCESS)
  {
    //Work around to BREW issue. Could not set selection inside the dialog handler.
    //ITEXTCTL_SetSelection() is from BREW 3.1.3 and it required a new class id
    //to trigger the function. Otherwise, IDialog_Load function will unselect the
    //area that selected in the handler. We can set selection after dialog is created.
    IDialog * dlg = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if (dlg)
    {
      ITextCtl * ctl = (ITextCtl*)IDIALOG_GetControl(dlg, (int16)selId);
      if (ctl)
      {
        uint32 sel = TC_MAKE_SEL(TC_SEL_START, TC_SEL_END);
        ITEXTCTL_SetSelection(ctl, sel);
      }
    }
  }
}

/*=====================================================================
FUNCTION QcamMenus_BuildSubSizeIdList

  DESCRIPTION:
    This function builds the sub size id list from the size id list.
    It is used to build the dynamic size menu based on sensor support.

  PARAMETERS:
    pMe      - pointer to QCam data struct
    pIdList  - pointer to buffer for the sub size id list.
    size     - sub size buffer max size.

  RETURN VALUE:
    int       - Offset from the original size list.
======================================================================*/
static uint16 QcamMenus_BuildSubSizeIdList(QCam *pMe, uint16 *pIdList, uint16 size)
{
  uint16 i;
  int j = 2;
  uint16 count = PrevSizeIDList[0]; //Original size list count

  if (!pMe)
    return 0;

  if (!pIdList || (size < count))
    return count;

  pIdList[1] = PrevSizeIDList[1];    //Size list id name
  if (pMe->m_encodingType != QCAM_ENCODING_PNG)
  {
    j = 3; //offset for QCAM_SIZE_MAX if not for PNG encoding.
    pIdList[2] = PrevSizeIDList[2];    //Always keep Max size in list
  }
  else
  {
    count--; //offset for QCAM_SIZE_MAX if PNG encoding.
  }
  for (i = 1; i < PrevSizeIDList[0]; i++)
  {
    //If the max size id is larger than IDS_SIZE_UXGA,
    //skip the item from the list
    if (PrevSizeDataList[i] < pMe->m_nMaxSize)
    {
      count--;
      continue;
    }

    //copy the remain list to the sub size list buffer.
    pIdList[j] = PrevSizeIDList[i + 2];
    j++;
  }
  pIdList[0] = count;  //sub size list count
  //return the offset number.
  return (PrevSizeIDList[0] - count);
}

/*=====================================================================
FUNCTION QcamMenus_BuildSubSizeMenu

  DESCRIPTION:
    This function builds the dynamic size menu based on sensor info.
  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NA
======================================================================*/
static void QcamMenus_BuildSubSizeMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nSize;
    uint16 SubList[QCAM_SIZE_COUNT] = {0};
    uint32 SubDataList[QCAM_SIZE_COUNT] = {0};
    uint16 offset = QcamMenus_BuildSubSizeIdList(pMe, SubList, (uint16)QCAM_SIZE_COUNT);

    if (pMe->m_encodingType != QCAM_ENCODING_PNG)
    {
      offset += 1; //add max size offset;
      (void)MEMCPY(&SubDataList[1],
                   &PrevSizeDataList[offset],
                   (size_t)(sizeof(uint32) * ((uint16)QCAM_SIZE_COUNT - offset)));
    }
    else
    {
      (void)MEMCPY(SubDataList,
                   &PrevSizeDataList[offset],
                   (size_t)(sizeof(uint32) * ((uint16)QCAM_SIZE_COUNT - offset)));
    }
    QcamMenus_BuildSubMenu(pMe,
                  MENU_Radio,
                  SubList,
                  SubDataList,
                  sel);
  }
}

/*===========================================================================
FUNCTION QcamMenus_BuildTakePhotoMenu

  DESCRIPTION
    This function builds the soft menu for picture preview.

  PARAMETERS:
    pMe   - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
void QcamMenus_BuildTakePhotoMenu(QCam *pMe)
{
  AEERect qrc;
  CtlAddItem ai;
  IPosDet *pIPosDet;
  int result = EUNSUPPORTED;
  uint16 imageID = 0;
  uint16 i;

  if (!pMe)
    return;

  IMENUCTL_Reset(pMe->m_pISoftMenu);
  // Set the Rectangle for the Menu
  SETAEERECT (&qrc,
              0, pMe->di.cyScreen - pMe->m_softMenuHeight,  //x, y
              pMe->di.cxScreen, pMe->m_softMenuHeight);     //dx, dy

  IMENUCTL_SetRect(pMe->m_pISoftMenu, &qrc);

  // Always turn on the bShowMenu flag when building the Take Photo menu
  pMe->m_bShowMenu = TRUE;

  // Reset menu style
  QCam_SetMenuStyle(pMe->m_pISoftMenu, 0, 0);

  // Add Items in the Menu
  QCam_InitMenuData(&ai, pMe->m_pResFile);

  // Check if GPS feature is available on this phone
  (void)ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_POSDET, (void **)&pIPosDet);
  if (pIPosDet)
  {
    AEEGPSConfig config = {0};
    result = IPOSDET_GetGPSConfig(pIPosDet, &config);
    (void)IPOSDET_Release(pIPosDet);
  }

  //Add individual entries to the Menu

  for (i = 0; i < ARR_SIZE(TakePicIDList); i++)
  {
    //Get right icon id
    switch (TakePicIDList[i].menuID)
    {
    case IDS_TAKE_PIC:
      imageID = (uint16)(QCam_CheckProperties(pMe->m_nProperties, QCAM_PORTRAIT) ?
                IDB_IMAGE_ICON_PORTRAIT :
                IDB_IMAGE_ICON);
      break;
    case IDS_GPS_SETUP:
      if (result == SUCCESS)
      {
        //Add GPS
        imageID = (uint16)(pMe->m_eGPSState == QCAM_GPS_DATA_VALID?
                  IDB_GPS_ACTIVE :
                  IDB_GPS_INACTIVE);
      }
      else //skip the GPS item
        continue;
      break;
    case IDS_FOCUS:
      //If the sensor does not support focus feature, skip the menu item
      if (pMe->m_nFocusMax == 0)
        continue;
      imageID = TakePicIDList[i].iconID;
      break;
    case IDS_CONTRAST:
      //If the camera does not support contrast feature, skip the menu item
      if (pMe->m_nContrastMax == 0)
        continue;
      imageID = TakePicIDList[i].iconID;
      break;
#if defined (FEATURE_CAMERA_V2) || defined (FEATURE_CAMERA7500)
    case IDS_SHARPNESS:
      //If the camera does not support sharpness feature, skip the menu item
      if (pMe->m_nSharpnessMax == 0)
        continue;
      imageID = TakePicIDList[i].iconID;
      break;
#endif //#ifdef FEATURE_CAMERA_V2 || FEATURE_CAMERA7500
#ifdef FEATURE_CAMERA_MOBICAT
    case IDS_MCT:
      imageID = (uint16)(pMe->m_bMobiCaTon ? IDB_MCT_OFF : IDB_MCT_ON);
      break;
#endif
    default:
      imageID = TakePicIDList[i].iconID;
      break;
    }

    //Add menu items
    QCam_AddMenuItem(pMe->m_pISoftMenu, &ai, TakePicIDList[i].menuID, imageID);
  }

  // Activate menu
  pMe->m_activeSoftMenu = QCAM_SOFT_MENU_TAKE_PHOTO;
  IMENUCTL_SetSel(pMe->m_pISoftMenu, pMe->m_wTakePicMenuSel);
  IMENUCTL_SetActive(pMe->m_pISoftMenu, TRUE);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
  Qcamera_DisplayInfo(pMe);
  Qcamera_DisplayTakePicMenuName(pMe, 0);
}

/*===========================================================================
FUNCTION QcamMenus_BuildSettingsMenu

  DESCRIPTION
    This function builds the soft menu for picture setting.

  PARAMETERS:
    pMe   - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
void QcamMenus_BuildSettingsMenu(QCam *pMe)
{
  AEERect       qrc = {0};
  AEEItemStyle  sstyle = {0};
  AEEItemStyle  ustyle = {0};
  AEEMenuColors colors = {0};
  CtlAddItem    ai;
  uint16        i;

  if (!pMe)
    return;

  IMENUCTL_Reset(pMe->m_pISoftMenu);

  //Clear the Rectangle at bottom for the Menu
  SETAEERECT (&qrc, 0, pMe->di.cyScreen - pMe->m_softMenuHeight,
              pMe->di.cxScreen, pMe->m_softMenuHeight);
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);

  //Set the Rectangle at top for the Menu
  SETAEERECT (&qrc, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight);
  IMENUCTL_SetRect(pMe->m_pISoftMenu, &qrc);

  // Reset Menu item style
  IMENUCTL_GetStyle(pMe->m_pISoftMenu, &ustyle, &sstyle);
  ustyle.ft = AEE_FT_NONE;
  sstyle.ft = AEE_FT_INDENT;
  IMENUCTL_SetStyle(pMe->m_pISoftMenu, &ustyle, &sstyle);

  // Reset Menu colors
  IMENUCTL_GetColors(pMe->m_pISoftMenu, &colors);
  colors.wMask |= MC_SEL_BACK;
  colors.cSelBack = RGB_NONE;
  IMENUCTL_SetColors(pMe->m_pISoftMenu, &colors);

  //Add Items in the Menu
  QCam_InitMenuData(&ai, pMe->m_pResFile);

  //Make sure image resource is not loaded for the menu entries
  ai.pszResImage = NULL;

  //Add individual entries to the Menu
  ai.wFont = QCAM_FONT_TITLE;
  for (i = 0; i < ARR_SIZE(CSettingMenuHandlers); i++)
  {
    ai.wText = CSettingMenuHandlers[i].MenuID;

    //Add menu items dynamically
    switch (CSettingMenuHandlers[i].MenuID)
    {
    case IDS_NIGHTSHOT_MODE:
      if (!pMe->m_bNightShotSupported)
        continue;
      break;
    case IDS_REDEYE_REDUCE:
      if (!pMe->m_bRedEyeRemovalSupported)
        continue;
      break;
#ifndef FEATURE_CAMERA_V2
    case IDS_PREVIEWFPS:
    case IDS_PREVIEWFPS_HIGH:
      if (pMe->m_nMaxFPS == 0)
        continue;
      break;
    case IDS_ISO:
      if (pMe->m_nMaxISO == 0)
        continue;
      break;
    case IDS_AF_MODE:
#ifdef FEATURE_CAMERA7500
    case IDS_AF_MODE_METERING:
#endif //FEATURE_CAMERA7500
      if (pMe->m_nFocusMax == 0)
        continue;
      break;
    case IDS_BEST_SHOT:
      if (!pMe->m_bBSMSupported)
        continue;
      break;
    case IDS_MULTISHOT:
      if (!pMe->m_bMultiShotSupported)
        continue;
      break;
#endif //FEATURE_CAMERA_V2
    default:
      break;
    }

    QCam_AddMenuItem(pMe->m_pISoftMenu, &ai,
                     CSettingMenuHandlers[i].MenuID, 0);
    MSG_MED("QCam_AddMenuItem: %d", CSettingMenuHandlers[i].MenuID, 0, 0);
  }

 // Activate menu
 pMe->m_activeSoftMenu = QCAM_SOFT_MENU_SETTINGS;
 IMENUCTL_SetActive(pMe->m_pISoftMenu, TRUE);
 (void)QcamMenus_DoSettingsMenu(pMe, 0);
} /* END QcamMenus_BuildSettingsMenu */

/*===========================================================================
  FUNCTION: QcamMenus_BuildSubMenu

  DESCRIPTION:
    This function build the selected setting menu.

  PARAMETERS:
    pMe:      QCam pointer
    type:     Active menu type
    IdList:   Menu item id list
    DataList: Data list associate with this menu
    sel:      item(s) on focus

  DEPENDENCIES:
    NA

  RETURN VALUE
    False if failed to build the menu
    True if successful

  SIDE EFFECTS:
  none
===========================================================================*/
static void QcamMenus_BuildSubMenu(QCam *pMe, ListMenuType type, const uint16 *IdList, const uint32 *DataList, uint32 sel)
{
  CtlAddItem ai;
  AEERect rc;

  // Make sure the pointers we'll be using are valid
  if (!pMe || !IdList)
    return;

  if (pMe->m_bSmallScreen)
    SETAEERECT(&rc, 0, pMe->m_nTitleHeight, (int16)pMe->di.cxScreen, 
               (int16)(pMe->di.cyScreen - pMe->m_nTitleHeight));
  else
    SETAEERECT(&rc, 0, pMe->m_nTitleHeight, (int16)pMe->di.cxScreen, 
               (int16)(pMe->di.cyScreen - pMe->m_nTitleHeight));

  // Init menu screen and data
  QCam_InitMenu(pMe->m_pIMenu, pMe->a.m_pIDisplay, pMe->m_pResFile, &ai, 0, rc);

  switch(type)
  {
  case MENU_Radio:
    QCam_BuildRadioListMenu(pMe->m_pIMenu, &ai, IdList, DataList, sel, IDB_RADIO_ON, IDB_RADIO_OFF);
    break;
  case MENU_Check:
    QCam_BuildCheckListMenu(pMe->m_pIMenu, &ai, IdList, DataList, sel, IDB_CHECKED, IDB_UNCHECKED);
    break;
  default:
    break;
  }

  if (pMe->m_bSmallScreen)
    IMENUCTL_SetProperties(pMe->m_pIMenu, MP_WRAPSCROLL);

  IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
}

/* ==========================================================================
FUNCTION QCAM_BUILDGPSMENU
DESCRIPTION
============================================================================= */
void QcamMenus_BuildGPSMenu(QCam *pMe)
{
  CtlAddItem ai;
  uint16 index = 0;
  uint16 wImage = IDB_GPS_INACTIVE;
  AEERect rc;

  if (!pMe)
    return;

  SETAEERECT(&rc, 0, 0, (int16)pMe->di.cxScreen, (int16)pMe->di.cyScreen);

  // Add Items in the Menu
  QCam_InitMenu(pMe->m_pIMenu, pMe->a.m_pIDisplay, pMe->m_pResFile, &ai, GPSIDList[1], rc);

  //Add individual entries to the Menu
  while (index < GPSIDList[0])
  {
    QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, GPSIDList[index+2], GPSIDList[index+2], wImage, GPSDataList[index]);
    wImage = IDB_GPS_ACTIVE;
    index++;
  }

  pMe->m_activeMenu = QCAM_MENU_GPS;
  IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
} /* END QcamMenus_BuildGPSMenu */


/* ==========================================================================
FUNCTION QCAM_BUILDSOUNDMENU
DESCRIPTION
============================================================================= */
void QcamMenus_BuildSoundMenu(QCam *pMe)
{
  AEERect rc;
  CtlAddItem ai;
  uint16 count = 1;
  AECHAR szBuf[FILE_NAME_SIZE];
  QCam_FileList *pList;

  if (!pMe)
    return;

  pMe->m_nSelSound = 0;
  if (pMe->m_bSmallScreen)
    SETAEERECT(&rc, 0, pMe->m_nTitleHeight, (int16)pMe->di.cxScreen, 
               (int16)(pMe->di.cyScreen - pMe->m_nTitleHeight));
  else
    SETAEERECT(&rc, 0, pMe->m_nTitleHeight, (int16)pMe->di.cxScreen, 
               (int16)(pMe->di.cyScreen - pMe->m_nTitleHeight));

  //Build the sound list first
  if (!pMe->m_pSoundList)
    pMe->m_nTotalSound = QCam_BuildSoundList(pMe->m_pIFileMgr, &pMe->m_pSoundList);

  // Add Items in the Menu
  QCam_InitMenu(pMe->m_pIMenu, pMe->a.m_pIDisplay, pMe->m_pResFile, &ai, 0, rc);

  ai.dwData = 1;
  pList = pMe->m_pSoundList;
  while(pList)
  {
    //Add individual entries to the Menu
    if (count < pMe->m_nTotalSound) //sound file name
    {
      //lint -save -e740, Unusual pointer cast (incompatible indirect types)
      //lint -save -e611, Suspicious cast
      char *pStr = &pList->fileName[QCAM_SOUNDS_DIR_LEN];
      //lint -restore
      ai.wItemID = (uint16)count++;
      (void)STRTOWSTR(pStr, szBuf, sizeof(szBuf));
    }
    else //"No Sound" string
    {
      ai.wItemID = 0;
      (void) STRTOWSTR(pList->fileName, szBuf, sizeof(szBuf));
      if ((pMe->m_nSelSound == 0) &&
           STRCMP(QCAM_SHUTTER_FILE, pMe->m_sShutterSoundFile))
      {
        //Required shutter sound is not found, need reset it.
        (void)STRLCPY(pMe->m_sShutterSoundFile, QCAM_SHUTTER_FILE, FILE_NAME_SIZE);
      }
    }
    ai.pText    = szBuf;
    if (STRCMP(pList->fileName, pMe->m_sShutterSoundFile) == 0)
    {
      pMe->m_nSelSound = ai.wItemID;
      ai.wImage = IDB_RADIO_ON;
    }
    else
    {
       ai.wImage = IDB_RADIO_OFF;
    }
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    ai.dwData++;
    pList = pList->next;
  }

  IMENUCTL_SetSel(pMe->m_pIMenu, pMe->m_nSelSound);
  if (pMe->m_bSmallScreen)
    IMENUCTL_SetProperties(pMe->m_pIMenu, MP_WRAPSCROLL);

  pMe->m_activeMenu = QCAM_MENU_SOUND;
  IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);

} /* END QcamMenus_BuildSoundMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDMAXPICSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildMaxPicsMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nMaxNumPics;
    pMe->m_activeMenu = QCAM_MENU_MAX_PICS;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      MaxPicsIDList,
                      MaxPicsDataList,
                      sel);
  }
} /* END QcamMenus_BuildMaxPicsMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDSAVEMENU
DESCRIPTION
   Builds the Save Location menu.
============================================================================= */
static void QcamMenus_BuildSaveMenu(QCam *pMe)
{
  int nReturnStatus = EFAILED;
  CtlAddItem ai;
  boolean bAddMMC=FALSE;
  boolean bAddFTL=FALSE;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  boolean bAddUSB=FALSE;
#endif
  uint16 imageID;
  uint16 focus;
  AEERect rc;

  if (!pMe)
    return;

  //It is a long processing to check if MMC is available, draw a hour glass.
  Qcamera_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

  if (pMe->m_saveLoc == QCAM_SAVE_MMC)
    focus = IDS_MMC_LOC;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  else if (pMe->m_saveLoc == QCAM_SAVE_USB)
    focus = IDS_USB_LOC;
#endif
  else if (pMe->m_saveLoc == QCAM_SAVE_FTL)
    focus = IDS_FTL_LOC;
  else
    focus = IDS_FLASH_LOC;

  SETAEERECT(&rc, 0, pMe->m_nTitleHeight, (int16)pMe->di.cxScreen, 
             (int16)(pMe->di.cyScreen - pMe->m_nTitleHeight));

  if (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_MMC_DIR) == SUCCESS)
  {
    bAddMMC=TRUE;
  }
  if (!bAddMMC)
  {
    // If we did not find the image directory, try creating it.
    // This may be a new MMC card and the "img" directory may
    // not exist yet.
    nReturnStatus = IFILEMGR_MkDir(pMe->m_pIFileMgr, QCAM_MMC_DIR);
    if ((nReturnStatus == SUCCESS) &&
        (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_MMC_DIR) == SUCCESS))
    {
      bAddMMC = TRUE;
    }
    else
    {
      bAddMMC = FALSE;
      if (pMe->m_saveLoc == QCAM_SAVE_MMC)
      {
        pMe->m_saveLoc = QCAM_SAVE_FLASH;
        focus = IDS_FLASH_LOC;
      }
    }
  }

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  if (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_USB_DIR) == SUCCESS)
  {
    bAddUSB=TRUE;
  }
  if (!bAddUSB)
  {
    // If we did not find the image directory, try creating it.
    // This may be a new USB Storage device and the "img" directory may
    // not exist yet.
    nReturnStatus = IFILEMGR_MkDir(pMe->m_pIFileMgr, QCAM_USB_DIR);
    if ((nReturnStatus == SUCCESS) &&
      (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_USB_DIR) == SUCCESS))
    {
      bAddUSB = TRUE;
    }
    else
    {
      bAddUSB = FALSE;
      if (pMe->m_saveLoc == QCAM_SAVE_USB)
      {
        pMe->m_saveLoc = QCAM_SAVE_FLASH;
        focus = IDS_FLASH_LOC;
      }
    }
  }
#endif
  if (pMe->m_bFTL)
  {
    if (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_FTL_DIR) == SUCCESS)
    {
      bAddFTL=TRUE;
    }
    if (!bAddFTL)
    {
      // If we did not find the image directory, try creating it.
      // This may the first time FTL memory is used and the "img"
      // directory may not exist yet.
      nReturnStatus = IFILEMGR_MkDir(pMe->m_pIFileMgr, QCAM_FTL_DIR);
      if ((nReturnStatus == SUCCESS) &&
          (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_FTL_DIR) == SUCCESS))
      {
        bAddFTL = TRUE;
      }
      else
      {
        bAddFTL = FALSE;
        if (pMe->m_saveLoc == QCAM_SAVE_FTL)
        {
          pMe->m_saveLoc = QCAM_SAVE_FLASH;
          focus = IDS_FLASH_LOC;
        }
      }
    }
  }

  // Add Items in the Menu
  QCam_InitMenu(pMe->m_pIMenu, pMe->a.m_pIDisplay, pMe->m_pResFile, &ai, 0, rc);

  imageID = (focus == SaveIDList[2]) ? IDB_RADIO_ON : IDB_RADIO_OFF;
  QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, SaveIDList[2], SaveIDList[2], imageID, (uint32)QCAM_SAVE_FLASH);

  if(bAddMMC)
  {
    imageID = (focus == SaveIDList[3]) ? IDB_RADIO_ON : IDB_RADIO_OFF;
    QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, SaveIDList[3], SaveIDList[3], imageID, (uint32)QCAM_SAVE_MMC);
  }

  if(bAddFTL)
  {
    imageID = (focus == SaveIDList[4]) ? IDB_RADIO_ON : IDB_RADIO_OFF;
    QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, SaveIDList[4], SaveIDList[4], imageID, (uint32)QCAM_SAVE_FTL);
  }

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS  
  if(bAddUSB)
  {
    imageID = (focus == SaveIDList[5]) ? IDB_RADIO_ON : IDB_RADIO_OFF;
    QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, SaveIDList[5], SaveIDList[5], imageID, (uint32)QCAM_SAVE_USB);
  }
#endif

  IMENUCTL_SetSel(pMe->m_pIMenu, focus);
  pMe->m_activeMenu = QCAM_MENU_SAVE;
  IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
} /* END QcamMenus_BuildSaveLocMenu */

/* ==========================================================================
FUNCTION QCAM_BUILDSIZEMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildSizeMenu(QCam *pMe)
{
  if (pMe)
  {
    pMe->m_activeMenu = QCAM_MENU_SIZE;
    if (pMe->m_nMaxSize > PrevSizeDataList[1])
    {
      QcamMenus_BuildSubSizeMenu(pMe);
    }
    else
    {
      uint32 sel = (uint32)pMe->m_nSize;

      QcamMenus_BuildSubMenu(pMe,
                        MENU_Radio,
                        PrevSizeIDList,
                        PrevSizeDataList,
                        sel);
    }
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  }
}
/* ==========================================================================
FUNCTION QCAM_BUILDSELFTIMERSETTINGSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildSelfTimerSettingsMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nSelfTimerDelay;
    pMe->m_activeMenu = QCAM_MENU_SELFTIMER;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      SelfTimerIDList,
                      SelfTimerDataList,
                      sel);
  }
}

/* ==========================================================================
FUNCTION QCAM_BUILDSLIDESHOWSPEEDMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildSlideShowSpeedMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nSlideshowSpeed;
    pMe->m_activeMenu = QCAM_MENU_SLIDESHOW_SPEED;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      SlideshowSpeedIDList,
                      SlideshowSpeedDataList,
                      sel);
  }
}

/* ==========================================================================
FUNCTION QCAM_BUILDENCODINGMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildEncodingMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_encodingType;

    //Need to change the text of soft menu which is supposed
    //to be the title of the submenu.
    IMENUCTL_SetItemText(pMe->m_pISoftMenu, IDS_ENCODING,
                         pMe->m_pResFile, IDS_ENCODING, NULL);

    pMe->m_activeMenu = QCAM_MENU_ENCODING;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      EncodingIDList,
                      EncodingDataList,
                      sel);
  }
}

/* ==========================================================================
FUNCTION QCAM_BUILDJPEGSPECMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildJPEGSpecMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nJPEGSpec;

    //Need to change the text of soft menu which is supposed
    //to be the title of the submenu.
    IMENUCTL_SetItemText(pMe->m_pISoftMenu, IDS_ENCODING,
                         pMe->m_pResFile, JPEGSpecIDList[1], NULL);

    pMe->m_activeMenu = QCAM_MENU_JPEGSPEC;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      JPEGSpecIDList,
                      JPEGSpecDataList,
                      sel);
  }
}

/* ==========================================================================
FUNCTION QCAM_BUILDQUALITYMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildQualityMenu(QCam *pMe)
{
  uint32 sel;

  if (!pMe)
    return;

  //Need to change the text of soft menu which is supposed
  //to be the title of the submenu.
  IMENUCTL_SetItemText(pMe->m_pISoftMenu, IDS_ENCODING,
                       pMe->m_pResFile, CamQualityIDList[1], NULL);

  if (pMe->m_qualityType > QCAM_QUALITY_PERCENT)
  {
    pMe->m_qualityType = QCAM_QUALITY_MED;
  }
  sel = (uint32)pMe->m_qualityType;
  pMe->m_activeMenu = QCAM_MENU_QUALITY;
  QcamMenus_BuildSubMenu(pMe,
                    MENU_Radio,
                    CamQualityIDList,
                    QualityDataList,
                    sel);

}
/* ==========================================================================
FUNCTION QCAM_BUILDPROPERTIESMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildPropertiesMenu(QCam *pMe)
{
#ifndef FEATURE_CAMERA_V2
  CtlAddItem ai;
#endif //FEATURE_CAMERA_V2
  if (pMe)
  {
    pMe->m_activeMenu = QCAM_MENU_PROPERTIES;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Check,
                      PropertiesIDList,
                      PropertiesList,
                      pMe->m_nProperties);


#ifndef FEATURE_CAMERA_V2
    //Only display the option when it is supported.
    QCam_InitMenuData(&ai, pMe->m_pResFile);
    //Luma adaptation
    if (pMe->m_bLASupported)
    {
      ai.wImage = QCam_CheckProperties(pMe->m_nProperties, QCAM_LUMA_ADAPTATION) ?
                  IDB_CHECKED : IDB_UNCHECKED;
      QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, IDS_LUMA_ADAPTATION, 
                         IDS_LUMA_ADAPTATION, ai.wImage, QCAM_LUMA_ADAPTATION);
    }
#ifndef FEATURE_CAMERA7500
    //LCD size display
    ai.wImage = QCam_CheckProperties(pMe->m_nProperties, QCAM_LCD_DISP) ?
                IDB_CHECKED : IDB_UNCHECKED;
    QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, IDS_LCDSIZE_DISP, 
                       IDS_LCDSIZE_DISP, ai.wImage, QCAM_LCD_DISP);
#endif //FEATURE_CAMERA7500

#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
    //Multiple displays (TV out)
    ai.wImage = QCam_CheckProperties(pMe->m_nProperties, QCAM_MULTI_DISP) ?
                IDB_CHECKED : IDB_UNCHECKED;
    QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, IDS_MULTI_DISP, 
                       IDS_MULTI_DISP, ai.wImage, QCAM_MULTI_DISP);
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
    //one button snapshot (auto focus + snapshot)
    if(pMe->m_nFocusMax > 0)
    {
      ai.wImage = QCam_CheckProperties(pMe->m_nProperties, QCAM_1_BUTTON_SNAPSHOT) ?
                  IDB_CHECKED : IDB_UNCHECKED;
      QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, IDS_1_BUTTON_SNAPSHOT, 
                         IDS_1_BUTTON_SNAPSHOT, ai.wImage, QCAM_1_BUTTON_SNAPSHOT);
    }

#ifdef FEATURE_JPEGD_RESIZE
    //User control slideshow display size
    ai.wImage = QCam_CheckProperties(pMe->m_nProperties, QCAM_SLIDESHOW_RESIZE) ?
                IDB_CHECKED : IDB_UNCHECKED;
    QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, IDS_SLIDESHOW_RESIZE, 
                       IDS_SLIDESHOW_RESIZE, ai.wImage, QCAM_SLIDESHOW_RESIZE);
#endif //FEATURE_JPEGD_RESIZE
#ifdef FEATURE_CAMERA_PANORAMIC
    //Panoramic feature
    ai.wImage = (pMe->m_nPanoramic > 0) ? IDB_CHECKED : IDB_UNCHECKED;
    QCam_AddMenuItemEx(pMe->m_pIMenu, &ai, IDS_PANORAMIC_EFFECT, 
                         IDS_PANORAMIC_EFFECT, ai.wImage, QCAM_PANORAMIC_EFFECT);
#endif //FEATURE_CAMERA_PANORAMIC
    (void)IMENUCTL_Redraw(pMe->m_pIMenu);
#endif //FEATURE_CAMERA_V2
  }
}

/* ==========================================================================
FUNCTION QCAM_BUILDAUTOEXPOSUREMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildAutoExposureMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_autoExposure;
    pMe->m_activeMenu = QCAM_MENU_AUTOEXPOSURE;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      AutoExposureIDList,
                      AutoExposureDataList,
                      sel);
  }
}

/* ==========================================================================
FUNCTION QCAM_BUILDWHITEBALANCEMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildWhitebalanceMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nWhitebalance;
    pMe->m_activeMenu = QCAM_MENU_WHITEBALANCE;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      CamWhiteBalanceIDList,
                      WhiteBalanceDataList,
                      sel);
  }
}

/* ==========================================================================
FUNCTION QCAM_BUILDEFFECTSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildEffectsMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nEffect;
    (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_EFFECT, (int32 *)(&sel), NULL);  
    pMe->m_nEffect = (uint8)sel;
    pMe->m_activeMenu = QCAM_MENU_EFFECTS;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      CamEffectsIDList,
                      EffectsDataList,
                      sel);
  }
}

/* ==========================================================================
FUNCTION QCAM_BUILDDELETEALLMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildDeleteAllMenu(QCam *pMe)
{
  boolean bHaveMMC = FALSE;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  boolean bHaveUSB = FALSE;
#endif
  uint16 *menuIDList = NULL;
  uint16 i = 0;

  if (!pMe)
    return;

  //Allocate buffer for sublist. Need two extra spaces for number and title.
  menuIDList = (uint16 *)MALLOC(sizeof(uint16) * (DelAllIDList[0] + 2));
  if (!menuIDList)
  {
    MSG_HIGH("QcamMenus_BuildDeleteAllMenu error - No memory.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    return;
  }

  //It is a long processing to check if MMC is available, draw a hour glass.
  Qcamera_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

  //If no photo saved, no need to build the menu, display the message.
  pMe->m_activeMenu = QCAM_MENU_DELETEALL;
  if (!QCam_HasPhotos(pMe->m_pIFileMgr, LOC_ALL))
  {
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    Qcamera_DisplayTimedImg(pMe, NULL, (uint16)IDB_NO_PHOTO);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    return;
  }

  bHaveMMC = (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_MMC_DIR) == SUCCESS);
  MSG_MED("bHaveMMC = %d", bHaveMMC, 0, 0);

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  bHaveUSB = (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_USB_DIR) == SUCCESS);
  MSG_MED("bHaveUSB = %d", bHaveUSB, 0, 0);
#endif

  // Construct the menu based on available memory / raw photos.
  ZEROAT(menuIDList);
  menuIDList[1] = IDS_DELETEALL;  //For menu title
  menuIDList[2] = IDS_YES;        //For Yes (All) option
  i = 3;
  //If more than one device, add "Flash only"
  if (bHaveMMC || pMe->m_bFTL)
    menuIDList[i++] = IDS_YES_FLASH;
  if (bHaveMMC)
    menuIDList[i++] = IDS_YES_MMC;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  if (bHaveUSB)
    menuIDList[i++] = IDS_YES_USB;
#endif  
  if (QCam_HasPhotos(pMe->m_pIFileMgr, LOC_RAW))
    menuIDList[i++] = IDS_YES_RAW;
  if (pMe->m_bFTL)
    menuIDList[i++] = IDS_YES_FTL;
#if defined (FEATURE_CAMERA_ENCODE_YCBCR)
  if (QCam_HasPhotos(pMe->m_pIFileMgr, LOC_YCBCR))
    menuIDList[i++] = IDS_YES_YCBCR;
#endif //FEATURE_CAMERA_ENCODE_YCBCR

  menuIDList[i] = IDS_NO;
  menuIDList[0] = i - 1;  // i-1 menu items added to this list

  QcamMenus_BuildSubMenu(pMe, MENU_Radio, menuIDList, NULL, (uint32)(i - 2)); //default to No option item
  CAM_FREEIF(menuIDList);
}

/* ==========================================================================
FUNCTION QCAM_BUILDREFLECTMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildReflectMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nReflectPreview;
    pMe->m_activeMenu = QCAM_MENU_REFLECT;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      ReflectIDList,
                      NULL,
                      sel);
  }
}

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
/* ==========================================================================
FUNCTION QcamMenus_BuildFrameOverlayMenu
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildFrameOverlayMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_eIconState;
    pMe->m_activeMenu = QCAM_MENU_FRAME_OVERLAY;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      FrameOverlayIDList,
                      FrameOverlayList,
                      sel);
  }
}
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY

/* ==========================================================================
FUNCTION QCAM_BUILDANTIBANDINGMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildAntibandingMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nAntibanding;
    pMe->m_activeMenu = QCAM_MENU_ANTIBANDING;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      AntiBandingIDList,
                      AntiBandingDataList,
                      sel);
    if (!pMe->m_bHasAutoAntibanding)
      (void)IMENUCTL_DeleteItem(pMe->m_pIMenu, IDS_ANTIBANDING_AUTO);
  }
}

/* ==========================================================================
FUNCTION QCAM_BUILDFLASHMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildFlashMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nFlash;
    pMe->m_activeMenu = QCAM_MENU_FLASH;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      FlashIDList,
                      FlashDataList,
                      sel);
  }
}

#ifdef FEATURE_CAMERA7500
/* ==========================================================================
FUNCTION QCAM_BUILDSTROBEFLASHMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildStrobeFlashMenu(QCam *pMe)
{
  uint32 sel = (uint32)pMe->m_nStrobeFlash;
  pMe->m_activeMenu = QCAM_MENU_STROBEFLASH;
  QcamMenus_BuildSubMenu(pMe,
                    MENU_Radio,
                    StrobeFlashIDList,
                    FlashDataList,
                    sel);
}
#endif //FEATURE_CAMERA7500

/* ==========================================================================
FUNCTION QCAM_BUILDREDEYEREDUCEMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildRedeyeReduceMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_bRedeyeReduce;
    pMe->m_activeMenu = QCAM_MENU_REDEYE;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      RedeyeReduceIDList,
                      NULL,
                      sel);
  }
}

/* ==========================================================================
FUNCTION QCAM_BUILDNIGHTSHOTMODEHMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildNightshotModeMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_bNightshotMode;
    pMe->m_activeMenu = QCAM_MENU_NIGHTSHOT_MODE;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      NightshotModeIDList,
                      NULL,
                      sel);
  }
}

#ifndef FEATURE_CAMERA_V2
/* ==========================================================================
FUNCTION QCAMMENUS_BUILDLOWFPSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildLowFPSMenu(QCam *pMe)
{
  if (!pMe)
    return;

  pMe->m_activeMenu = QCAM_MENU_PREVIEWFPS_LOW;
  if (!pMe->m_pListFPS)
  {
    //Build menu by default settings
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      PreviewFPSIDList,
                      PreviewFPSDataList,
                      pMe->m_nLowFPS);

    //delete the item if it is larger than max fps
    if (pMe->m_nMaxFPS < QCAM_FPS_30000)
    {
      (void)IMENUCTL_DeleteItem(pMe->m_pIMenu, IDS_PREVIEWFPS_30000);
    }
  }
  else
  {
    //Build menu by FPS list
    QcamMenus_BuildPreviewFPSMenu(pMe, 0, pMe->m_nLowFPS);
  }
}

/* ==========================================================================
FUNCTION QCAMMENUS_BUILDHIGHFPSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildHighFPSMenu(QCam *pMe)
{
  if (!pMe)
    return;

  pMe->m_activeMenu = QCAM_MENU_PREVIEWFPS_HIGH;
  if (!pMe->m_pListFPS)
  {
    //Build menu by default settings
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      PreviewHighFPSIDList,
                      PreviewFPSDataList,
                      pMe->m_nHighFPS);

    //delete the item if it is larger than max fps
    if (pMe->m_nMaxFPS < QCAM_FPS_30000)
    {
      (void)IMENUCTL_DeleteItem(pMe->m_pIMenu, IDS_HIGHFPS_30000);
    }
  }
  else
  {
    //Build menu by FPS list
    QcamMenus_BuildPreviewFPSMenu(pMe, 0, pMe->m_nHighFPS);
  }
}

/* ==========================================================================
FUNCTION QCAMMENUS_BUILDPREVIEWFPSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildPreviewFPSMenu(QCam *pMe, uint16 titleID, uint32 nCurVal)
{
  CtlAddItem ai;
  AECHAR szBuf[TEXT_STRING_SIZE] = {0};
  char pStr[TEXT_STRING_SIZE] = {0};
  AEERect rc = {0};
  uint32 *pList = NULL;
  uint32 nVal = 0;
  uint16 nSel = 0;
  uint16 nCount = 0;
  uint16 nDen = 1;

  if (!pMe)
    return;

  // Add Items in the Menu
  SETAEERECT(&rc, 0, pMe->m_nTitleHeight, (int16)pMe->di.cxScreen, 
             (int16)(pMe->di.cyScreen - pMe->m_nTitleHeight));
  QCam_InitMenu(pMe->m_pIMenu, pMe->a.m_pIDisplay, pMe->m_pResFile, &ai, titleID, rc);

  //In the pList the FPS value format is as below (AEECamera.h):
  //dwFPS format: Lower 16 bits is Numerator.
  //Upper 16 bits is Denominator. Zero denominator is treated as 1.
  pList = pMe->m_pListFPS;
  while(*pList)
  {
    nVal = *pList;
    nDen = (uint16)(nVal >> 16);

    //Add individual entries to the Menu
    if (nDen > 1)
    {
      (void)SNPRINTF(pStr, sizeof(pStr), "%d/%d", (nVal & 0xFFFF), (nVal >> 16));
      ai.dwData = (uint32)((nVal & 0xFFFF) * 1000 / nDen);
    }
    else
    {
      (void)SNPRINTF(pStr, sizeof(pStr), "%d", (nVal & 0xFFFF));
      ai.dwData = (uint32)((nVal & 0xFFFF) * 1000);
    }
    (void)STRTOWSTR(pStr, szBuf, sizeof(szBuf));
    ai.wItemID = nCount++;
    ai.pText   = szBuf;

    //Check current selected value.
    if (ai.dwData == nCurVal)
    {
      nSel = ai.wItemID;
      ai.wImage = IDB_RADIO_ON;
    }
    else
    {
       ai.wImage = IDB_RADIO_OFF;
    }
    (void)IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai);
    pList++;
  }

  if (pMe->m_activeMenu == QCAM_MENU_PREVIEWFPS_LOW)
    pMe->m_nSelLowFPS = nSel;
  else
    pMe->m_nSelHighFPS = nSel;
  IMENUCTL_SetSel(pMe->m_pIMenu, nSel);
  IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
}

/* ==========================================================================
FUNCTION QCAMMENUS_BUILDISOMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildISOMenu(QCam *pMe)
{
  if (!pMe)
    return;

  pMe->m_activeMenu = QCAM_MENU_ISO;
  QcamMenus_BuildSubMenu(pMe,
                    MENU_Radio,
                    ISOIDList,
                    ISODataList,
                    (uint32)pMe->m_nISO);

  //delete the items if they are larger than the max value.
  if (pMe->m_nMaxISO < (int32)QCAM_ISO_800)
    (void)IMENUCTL_DeleteItem(pMe->m_pIMenu, IDS_ISO_800);
  if (pMe->m_nMaxISO < (int32)QCAM_ISO_400)
    (void)IMENUCTL_DeleteItem(pMe->m_pIMenu, IDS_ISO_400);
  if (pMe->m_nMaxISO < (int32)QCAM_ISO_200)
    (void)IMENUCTL_DeleteItem(pMe->m_pIMenu, IDS_ISO_200);
#ifndef FEATURE_CAMERA_HJR
  (void)IMENUCTL_DeleteItem(pMe->m_pIMenu, IDS_ISO_HJR);
#endif
}

/* ==========================================================================
FUNCTION QCAMMENUS_BUILDAFMODEMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildAFModeMenu(QCam *pMe)
{
  if (pMe)
  {
    pMe->m_activeMenu = QCAM_MENU_AFMODE;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      AFModeIDList,
                      AFModeDataList,
                      pMe->m_nAFMode);
  }
}

#ifdef FEATURE_CAMERA7500
/* ==========================================================================
FUNCTION QCAMMENUS_BUILDAFMODEMETERINGMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildAFModeMeteringMenu(QCam *pMe)
{
  if (pMe)
  {
    pMe->m_activeMenu = QCAM_MENU_AFMODEMETERING;
    QcamMenus_BuildSubMenu(pMe, 
                      MENU_Radio,
                      AFModeMeteringIDList, 
                      AFModeMeteringDataList, 
                      pMe->m_nAFModeMetering);
  }
}
#endif //FEATURE_CAMERA7500

/* ==========================================================================
FUNCTION QCAMMENUS_BUILDMULTISHOTMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildMultishotMenu(QCam *pMe)
{
  if (pMe)
  {
    if (pMe->m_nMulPhotos == 1)
      pMe->m_nSelMultishot = QCAM_MULTISHOT_OFF;

    pMe->m_activeMenu = QCAM_MENU_MULTISHOT;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      MultishotIDList,
                      MultishotDataList,
                      pMe->m_nSelMultishot);
    //Remove Bracketing option if it is not supported.
    if (!pMe->m_bBracketingSupported)
      (void)IMENUCTL_DeleteItem(pMe->m_pIMenu, IDS_MULTISHOT_AE);
  }
}

/* ==========================================================================
FUNCTION QCAMMENUS_BUILDBRACKETMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_BuildBestshotMenu(QCam *pMe)
{
  if (pMe)
  {
    pMe->m_activeMenu = QCAM_MENU_BESTSHOT;
    QcamMenus_BuildSubMenu(pMe,
                      MENU_Radio,
                      BestshotIDList,
                      BestshotDataList,
                      pMe->m_nBestshot);
  }
}
#endif //FEATURE_CAMERA_V2

/* ==========================================================================
FUNCTION QCAM_EXECUTESELFTIMERMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteSelfTimerMenu(QCam *pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nSelfTimerDelay;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          SelfTimerIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_nSelfTimerDelay = (uint16)sel;

    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  }
} /* END QcamMenus_ExecuteSelfTimerMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTESLIDESHOWSPEEDMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteSlideshowSpeedMenu(QCam* pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nSlideshowSpeed;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          SlideshowSpeedIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_nSlideshowSpeed = (uint16)sel;

    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  }
} /* END QcamMenus_ExecuteSlideshowSpeedMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEFLASHMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteFlashMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint32 sel = (uint32)pMe->m_nFlash;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          FlashIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_nFlash = (uint16)sel;
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    nRet = ICAMERA_SetFlashCtl(pMe->m_pICam, CAM_FLASHTYPE_LED,(int32)sel, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_FLASH return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
} /* END QcamMenus_ExecuteFlashMenu */

#ifdef FEATURE_CAMERA7500
/* ==========================================================================
FUNCTION QCAM_EXECUTESTROBEFLASHMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteStrobeFlashMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint32 sel = (uint32)pMe->m_nStrobeFlash;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          StrobeFlashIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_nStrobeFlash = (uint16)sel;
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    nRet = ICAMERA_SetFlashCtl(pMe->m_pICam, CAM_FLASHTYPE_STROBE,(int32)sel, 0);

    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_FLASHTYPE_STROBE return %d", nRet, 0, 0);
  #ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
  #endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
} /* END QcamMenus_ExecuteFlashMenu */
#endif //FEATURE_CAMERA7500
/* ==========================================================================
FUNCTION QCAM_EXECUTEENCODINGMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteEncodingMenu(QCam* pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_encodingType;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          EncodingIDList,
                          &sel,
                          pMe->m_pResFile);

    pMe->m_encodingType = (QCamEncodingType)sel;
    if (pMe->m_encodingType == QCAM_ENCODING_PNG)
    {
      pMe->m_nMaxSize = (uint16)QCAM_SIZE_VGA;
      if (pMe->m_nSize < pMe->m_nMaxSize)
      {
        int nRet = 0;
        AEESize *pSize;

        //If encoding type is PNG but old size is larger than QVGA,
        //limit the size to VGA and change the viewfinder setting.
        pMe->m_nSize = pMe->m_nMaxSize;
        pSize = Qcamera_GetSize(pMe);

#ifdef AEE_SIMULATOR
          (void)ICAMERA_SetDisplaySize(pMe->m_pICam, pSize);
#endif

        nRet = ICAMERA_SetSize(pMe->m_pICam, pSize);
        if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
        {
          MSG_HIGH("ICAMERA_SetSize return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
        }
      }
    }
    else
    {
      pMe->m_nMaxSize = Qcamera_GetMaxSizeId(&pMe->m_sensorInfo);
    }
    if(pMe->m_encodingType == QCAM_ENCODING_JPEG)
    {
      QcamMenus_BuildJPEGSpecMenu(pMe);
    }
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif	
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  }
} /* END QcamMenus_ExecuteEncodingMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEJPEGSPECMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteJPEGSpecMenu(QCam* pMe)
{
  uint32 sel;

  if (!pMe)
    return;

  sel = (uint32)pMe->m_nJPEGSpec;
  QCam_UpdateRadioFocus(pMe->m_pIMenu,
                        JPEGSpecIDList,
                        &sel,
                        pMe->m_pResFile);

  pMe->m_nJPEGSpec = (QCamJPEGSpec)sel;
  switch (pMe->m_nJPEGSpec)
  {
  case QCAM_JPEG_FILESIZE:
    pMe->m_qualityType = QCAM_QUALITY_FILESIZE;
    QcamMenus_BuildDialog(pMe, IDD_SET_SIZE, IDI_QUALITY_INPUT);
    break;
  case QCAM_JPEG_BOTH:
    pMe->m_qualityType = QCAM_QUALITY_PERCENT;
    QcamMenus_BuildDialog(pMe, IDD_SET_SIZE, IDI_QUALITY_INPUT);
    break;
  default:
    if (pMe->m_nQuality == 0)
    {
      //Switch from file size only option
      pMe->m_nQuality = JPEG_QUALITY_MED;
      pMe->m_qualityType = QCAM_QUALITY_MED;
    }
    else if (pMe->m_qualityType == QCAM_QUALITY_FILESIZE)
    {
      pMe->m_qualityType = QCAM_QUALITY_PERCENT;
    }
    QcamMenus_BuildQualityMenu(pMe);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    break;
  }

} /* END QcamMenus_ExecuteJPEGSpecMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEWHITEBALANCEMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteWhitebalanceMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint32 sel = (uint32)pMe->m_nWhitebalance;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          CamWhiteBalanceIDList,
                          &sel,
                          pMe->m_pResFile);

    pMe->m_nWhitebalance = (uint8)sel;
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_WHITE_BALANCE, (int32)sel, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_WHITE_BALANCE return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
} /* END QcamMenus_ExecuteWhitebalanceMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEMAXPICSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteMaxPicsMenu(QCam* pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_nMaxNumPics;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          MaxPicsIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_nMaxNumPics = (uint16)sel;
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  }
} /* END QcamMenus_ExecuteMaxPicsMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEEFFECTSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteEffectsMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint32 sel = (uint32)pMe->m_nEffect;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          CamEffectsIDList,
                          &sel,
                          pMe->m_pResFile);

    pMe->m_nEffect = (uint8)sel;
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_EFFECT, (int32)sel, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_EFFECT return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
} /* END QcamMenus_ExecuteEffectsMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEREFLECTMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteReflectMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint32 sel = (uint32)pMe->m_nReflectPreview;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          ReflectIDList,
                          &sel,
                          pMe->m_pResFile);

    pMe->m_nReflectPreview = (uint16)sel;
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_REFLECT, (int32)sel, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_REFLECT return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
} /* END QcamMenus_ExecuteReflectMenu */

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
/* ==========================================================================
FUNCTION QCAM_EXECUTEFRAMEOVERLAYMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteFrameOverlayMenu(QCam* pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_eIconState;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          FrameOverlayIDList,
                          &sel,
                          pMe->m_pResFile);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    pMe->m_eIconState = (QCamIconState)sel;
  }
} /* END QcamMenus_ExecuteFrameOverlayMenu */
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY

/* ==========================================================================
FUNCTION QCAM_EXECUTESOUNDMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteSoundMenu(QCam* pMe)
{
  uint32 index = 0;
  uint16 oldFocus;
  QCam_FileList *pList = NULL;

  if (!pMe)
    return;

  oldFocus = pMe->m_nSelSound;
  //Find the new focus data
  pMe->m_nSelSound = IMENUCTL_GetSel(pMe->m_pIMenu);

  if (pMe->m_nSelSound == oldFocus)
  {
    //play the sound if available. no display update needed.
    if (pMe->m_nSelSound != 0)
      Qcamera_PlayShutterSound(pMe);
    return;
  }

  //Find the index of the item
  (void)IMENUCTL_GetItemData(pMe->m_pIMenu, pMe->m_nSelSound, &index);

  //Update the sound file name
  if (QCam_GetCurrentItem(pMe->m_pSoundList, &pList, (int16)index))
  {
    (void)STRLCPY(pMe->m_sShutterSoundFile, pList->fileName, FILE_NAME_SIZE);
  }
  else
  {
    pMe->m_nSelSound = 0;
    (void)STRLCPY(pMe->m_sShutterSoundFile, QCAM_SHUTTER_FILE, FILE_NAME_SIZE);
  }

  //Update radio icon on new focus
  QCam_SetRadioImg(pMe->m_pIMenu, oldFocus, pMe->m_nSelSound, pMe->m_pResFile);
  (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);

  //Play the new shutter sound
  if (pMe->m_nSelSound != 0)
    Qcamera_PlayShutterSound(pMe);

} /* END QcamMenus_ExecuteSoundMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTESAVELOCMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteSaveLocMenu(QCam* pMe)
{
  if (!pMe)
    return;

  //If MMC or FTL dir exists, update the radio icon on new focus.
  //otherwise these is only one item, always selected, no update needed.
  if((IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_MMC_DIR)==SUCCESS) || 
    #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS 
      (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_USB_DIR)==SUCCESS)|| 
    #endif 
    pMe->m_bFTL)
  {
    uint32 sel = (uint32)pMe->m_saveLoc;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          SaveIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_saveLoc = (QCamSaveType)sel;
  }

  (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
} /* END QcamMenus_ExecuteSaveLocMenu */


/* ==========================================================================
FUNCTION QCAM_EXECUTEPROPERTIESMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecutePropertiesMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint16 selection = IMENUCTL_GetSel(pMe->m_pIMenu);
    boolean flag = QCam_UpdateCheckMark(pMe->m_pIMenu,
                                        &pMe->m_nProperties,
                                        IDB_CHECKED,
                                        IDB_UNCHECKED,
                                        pMe->m_pResFile);

    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    switch (selection)
    {
    case IDS_ORIENTATION_PORTRAIT:
      nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ROTATE_ENCODE, flag ? 90 : 0, 0);
#ifdef FEATURE_CAMERA_PANORAMIC
      if (flag)
      {
        pMe->m_nPanoramic = 0;
        pMe->m_nProperties &= ~QCAM_PANORAMIC_EFFECT;
        (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PANORAMIC,  pMe->m_nPanoramic, 0);
        QCam_SetItemImg(pMe->m_pIMenu, pMe->m_pResFile, 
                        IDS_PANORAMIC_EFFECT, IDB_UNCHECKED);
      }
#endif //FEATURE_CAMERA_PANORAMIC
      break;
#ifndef FEATURE_CAMERA_V2
    case IDS_LUMA_ADAPTATION:
      nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_LUMA_ADAPTATION, flag ? 1 : 0, 0);
      break;
    case IDS_LCDSIZE_DISP:
#ifndef AEE_SIMULATOR
      {
        AEESize *pSize = Qcamera_GetSize(pMe);
        pMe->m_bDisplayLCDSize = flag;
        pMe->m_rcPreView = Qcamera_GetPreviewRect(pMe);
        nRet = Qcamera_SetDisplaySize(pMe, pSize);
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
        //Reset direct mode display size
        nRet += Qcamera_SetDirectMode(pMe, TRUE);
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE
      }
#endif //AEE_SIMULATOR
      break;
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
    case IDS_MULTI_DISP:
      CAM_RELEASEIF(pMe->m_pOvFrame);
      if(flag)
      {
        //Enable display link to AEECLSID_DISPLAY3
        nRet = QCamDisp_ChangeDisplay(pMe->a.m_pIShell, AEECLSID_DISPLAY3, 1);
      }
      else
      {
        if (pMe->m_coreUIClsId == 0)
        {
          //Disable display link to AEECLSID_DISPLAY3
          (void)QCamDisp_ChangeDisplay(pMe->a.m_pIShell, AEECLSID_DISPLAY3, 0);
        }
        else
        {
          //Relink to old display
          (void)QCamDisp_ChangeDisplay(pMe->a.m_pIShell, pMe->m_coreUIClsId, 1);
        }
      }
      break;
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef FEATURE_CAMERA_WIDESIZE  
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE 
#ifdef FEATURE_CAMERA_PANORAMIC
    case IDS_PANORAMIC_EFFECT:
      pMe->m_nPanoramic = flag ? 1 : 0;
      nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PANORAMIC, pMe->m_nPanoramic , 0);
      if (flag)
      {
        pMe->m_sFrameFileName[0] = '\0';
		pMe->m_bUpdatePreviewFrame = TRUE;
        if(QCam_CheckProperties(pMe->m_nProperties, QCAM_PORTRAIT))
        {
          pMe->m_nProperties &= ~QCAM_PORTRAIT;
          nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ROTATE_ENCODE, 0, 0);
          QCam_SetItemImg(pMe->m_pIMenu, pMe->m_pResFile, 
                          IDS_ORIENTATION_PORTRAIT, IDB_UNCHECKED);
        }
        if((pMe->m_nSize < (uint16)QCAM_SIZE_VGA))
        {
          AEESize *pSize;
          pMe->m_nSize = (uint16)QCAM_SIZE_VGA;
          pSize = Qcamera_GetSize(pMe);
#ifdef AEE_SIMULATOR
          (void)ICAMERA_SetDisplaySize(pMe->m_pICam, pSize);
#else 
          nRet = Qcamera_SetDisplaySize(pMe, pSize);
#endif //AEE_SIMULATOR
          nRet += ICAMERA_SetSize(pMe->m_pICam, pSize);
        }
        if (pMe->m_bMultiShotSupported)
        {
          pMe->m_nMulPhotos = 1;
        }
      }
      break;
#endif //FEATURE_CAMERA_PANORAMIC
#endif //FEATURE_CAMERA_V2
    default:
      break;
    }
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("ICAMERA_SetParm returns %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
} /* END QcamMenus_ExecutePropertiesMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTESIZEMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteSizeMenu(QCam* pMe)
{
  AEESize *pSize;
  uint32 sel;
  int nRet;

  if (!pMe)
    return;

  sel = (uint32)pMe->m_nSize;
  // Important to null out frame information before changing qcam sizes
  // This is because we can get wierd transparent pixel artifacts if we
  // try to suddenly apply leftover values from the previously selected frame.
  // NULL out frame file names
  pMe->m_sFrameFileName[0] = '\0';
  pMe->m_bUpdatePreviewFrame = TRUE;

  if (pMe->m_nMaxSize > PrevSizeDataList[1])
  {
    uint16 SubList[QCAM_SIZE_COUNT] = {0};
    (void)QcamMenus_BuildSubSizeIdList(pMe, SubList, (uint16)QCAM_SIZE_COUNT);
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                        SubList,
                        &sel,
                        pMe->m_pResFile);
  }
  else
  {
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          PrevSizeIDList,
                          &sel,
                          pMe->m_pResFile);
  }
  pMe->m_nSize = (uint16)sel;
#ifdef FEATURE_CAMERA_PANORAMIC
  if ((pMe->m_nSize < (uint16)QCAM_SIZE_VGA) & pMe->m_nPanoramic)
  {
    pMe->m_nPanoramic = 0;
    pMe->m_nProperties &= ~QCAM_PANORAMIC_EFFECT;
    (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PANORAMIC,  pMe->m_nPanoramic, 0);
  }
#endif //FEATURE_CAMERA_PANORAMIC
#ifdef FEATURE_CAMERA_WIDESIZE 
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE 
  (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  pMe->m_rcPreView = Qcamera_GetPreviewRect(pMe);
  pSize = Qcamera_GetSize(pMe);

#ifdef AEE_SIMULATOR
  (void)ICAMERA_SetDisplaySize(pMe->m_pICam, pSize);
#else
#ifndef FEATURE_CAMERA_V2
  nRet = Qcamera_SetDisplaySize(pMe, pSize);
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  //Reset direct mode display size
  nRet += Qcamera_SetDirectMode(pMe, TRUE);
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE
  if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
  {
    MSG_HIGH("Qcamera Set Display Size returns %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
  }
#endif //FEATURE_CAMERA_V2
#endif //AEE_SIMULATOR

  nRet = ICAMERA_SetSize(pMe->m_pICam, pSize);
  if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
  {
    MSG_HIGH("ICAMERA_SetSize returns %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
  }
} /* END QcamMenus_ExecuteSizeMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEDELETEALLMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteDeleteAllMenu(QCam* pMe)
{
  if (!pMe)
    return;

  //if no photo, do nothing
  if (QCam_HasPhotos(pMe->m_pIFileMgr, LOC_ALL))
  {
    //Update soft key icons or prumpt for delete all
    pMe->m_nDelOpt = IMENUCTL_GetSel(pMe->m_pIMenu);
    if (pMe->m_nDelOpt != IDS_NO)
    {
      if (Qcamera_PromptForDelAll(pMe))
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      }
    }
    else
    {
      (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
} /* END QcamMenus_ExecuteDeleteAllMenu */

/* ==========================================================================
FUNCTION QCAM_EXECUTEQUALITYMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteQualityMenu(QCam* pMe)
{
  if (pMe)
  {
    uint32 sel = (uint32)pMe->m_qualityType;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          CamQualityIDList,
                          &sel,
                          pMe->m_pResFile);

    pMe->m_qualityType = (QCamQualityType)sel;
    switch(pMe->m_qualityType)
    {
    case QCAM_QUALITY_LOW:
      pMe->m_nQuality = JPEG_QUALITY_LOW;
      break;
    case QCAM_QUALITY_MED:
      pMe->m_nQuality = JPEG_QUALITY_MED;
      break;
    case QCAM_QUALITY_HIGH:
      pMe->m_nQuality = JPEG_QUALITY_HIGH;
      break;
    case QCAM_QUALITY_PERCENT:
      IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);
      QcamMenus_BuildDialog(pMe, IDD_SET_SIZE, IDI_QUALITY_INPUT);
      break;
    default:
      break;
    }
    if (pMe->m_qualityType != QCAM_QUALITY_PERCENT)
      (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  }
}

/* ==========================================================================
FUNCTION QCAM_EXECUTEAUTOEXPOSUREMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteAutoExposureMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint32 sel = (uint32)pMe->m_autoExposure;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          AutoExposureIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_autoExposure = (QCamAEmode)sel;
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_EXPOSURE_METERING, (int32)sel, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_EXPOSURE_METERING return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
}

/* ==========================================================================
FUNCTION QCAM_EXECUTEAUTOEXPOSUREMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteAntibandingMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint32 sel = (uint32)pMe->m_nAntibanding;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          AntiBandingIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_nAntibanding = (uint16)sel;
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ANTIBANDING, (int32)sel, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_ANTIBANDING return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
}

/* ==========================================================================
FUNCTION QCAM_EXECUTEAUTOEXPOSUREMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteRedeyeReduceMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint32 sel = pMe->m_bRedeyeReduce ? 1 : 0;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          RedeyeReduceIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_bRedeyeReduce = (sel == 1);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_RED_EYE_REDUCTION, (int32)sel, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_RED_EYE_REDUCTION return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
}

/* ==========================================================================
FUNCTION QCAM_EXECUTENIGHTSHOTMODEMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteNightshotModeMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint32 sel = pMe->m_bNightshotMode ? 1 : 0;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          NightshotModeIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_bNightshotMode = (sel == 1);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_NIGHTSHOT_MODE, (int32)sel, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_NIGHTSHOT_MODE return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
}

#ifndef FEATURE_CAMERA_V2
/* ==========================================================================
FUNCTION QCAM_EXECUTEPREVIEWFPSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecutePreviewFPSMenu(QCam *pMe)
{
  AEEFixedPointMinMax p1;
  int nRet = 0;

  if (!pMe)
    return;

  //Check if value valid.
  if (pMe->m_nLowFPS > pMe->m_nHighFPS)
  {
    MSG_HIGH("Selected low end FPS higher than high end. Set to same", 0, 0, 0);
    pMe->m_nLowFPS = pMe->m_nHighFPS;
  }

  //Update the camera
  ZEROAT(&p1);
  p1.nMax = (int32)pMe->m_nHighFPS;
  p1.nMin = (int32)pMe->m_nLowFPS;
  nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PREVIEW_FPS, (int32)&p1, 0);
  if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
  {
    MSG_HIGH("Set CAM_PARM_PREVIEW_FPS return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
  }
}
/* ==========================================================================
FUNCTION QCAM_EXECUTELOWFPSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteLowFPSMenu(QCam *pMe)
{
  uint16 oldFocus;

  if (!pMe)
    return;

  if (!pMe->m_pListFPS)
  {
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          PreviewFPSIDList,
                          &pMe->m_nLowFPS,
                          pMe->m_pResFile);
  }
  else
  {
    oldFocus = pMe->m_nSelLowFPS;
    //Find the new focus data
    pMe->m_nSelLowFPS = IMENUCTL_GetSel(pMe->m_pIMenu);
    if (pMe->m_nSelLowFPS == oldFocus)
      return;

    //Update radio icon on new focus
    QCam_SetRadioImg(pMe->m_pIMenu, oldFocus, pMe->m_nSelLowFPS, pMe->m_pResFile);
    //Find the data of the item
    (void)IMENUCTL_GetItemData(pMe->m_pIMenu, pMe->m_nSelLowFPS, &pMe->m_nLowFPS);
  }
  (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  //Update the service
  QcamMenus_ExecutePreviewFPSMenu(pMe);
}

/* ==========================================================================
FUNCTION QCAM_EXECUTEHIGHFPSMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteHighFPSMenu(QCam *pMe)
{
  uint16 oldFocus;

  if (!pMe)
    return;

  if (!pMe->m_pListFPS)
  {
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          PreviewHighFPSIDList,
                          &pMe->m_nHighFPS,
                          pMe->m_pResFile);
  }
  else
  {
    oldFocus = pMe->m_nSelHighFPS;
    //Find the new focus data
    pMe->m_nSelHighFPS = IMENUCTL_GetSel(pMe->m_pIMenu);
    if (pMe->m_nSelHighFPS == oldFocus)
      return;

    //Update radio icon on new focus
    QCam_SetRadioImg(pMe->m_pIMenu, oldFocus, pMe->m_nSelHighFPS, pMe->m_pResFile);
    //Find the data of the item
    (void)IMENUCTL_GetItemData(pMe->m_pIMenu, pMe->m_nSelHighFPS, &pMe->m_nHighFPS);
  }
  (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  //Update the service
  QcamMenus_ExecutePreviewFPSMenu(pMe);
}

/* ==========================================================================
FUNCTION QCAM_EXECUTEISOMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteISOMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    uint32 sel = (uint32)pMe->m_nISO;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          ISOIDList,
                          &sel,
                          pMe->m_pResFile);
    pMe->m_nISO = (int32)sel;
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);

    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ISO, pMe->m_nISO, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_ISO return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
}

/* ==========================================================================
FUNCTION QCAM_EXECUTEAFMODEMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteAFModeMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          AFModeIDList,
                          &pMe->m_nAFMode,
                          pMe->m_pResFile);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);

    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_AF_INFO, (int32)pMe->m_nAFMode, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_AF_INFO return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
}

#ifdef FEATURE_CAMERA7500 
/* ==========================================================================
FUNCTION QCAM_EXECUTEAFMODEMETERINGMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteAFModeMeteringMenu(QCam* pMe)
{
  if (pMe)
  {
    int nRet = 0;
    QCam_UpdateRadioFocus(pMe->m_pIMenu, 
                          AFModeMeteringIDList, 
                          &pMe->m_nAFModeMetering,
                          pMe->m_pResFile);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);

    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_AF_INFO, 0, (int32)pMe->m_nAFModeMetering);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_AF_INFO return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
}

#endif //FEATURE_CAMERA7500 

/* ==========================================================================
FUNCTION QCAM_EXECUTEMULTISHOTMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteMultishotMenu(QCam *pMe)
{
  if (pMe)
  {
    int nRet = 0;
    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          MultishotIDList,
                          &pMe->m_nSelMultishot,
                          pMe->m_pResFile);

    if (pMe->m_bBracketingSupported)
    {
      int32 nVal = (pMe->m_nSelMultishot == QCAM_MULTISHOT_AE) ?
                   CAM_BRACKETING_AE : CAM_BRACKETING_OFF;
      nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_BRACKETING, nVal, 0);
      if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
      {
        MSG_HIGH("Set CAM_PARM_BRACKETING return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      }
    }

    //Enable shot number option
    if (pMe->m_nSelMultishot == QCAM_MULTISHOT_OFF)
    {
      pMe->m_nMulPhotos = 1;
      (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    }
    else
    {
#ifdef FEATURE_CAMERA_PANORAMIC
      pMe->m_nPanoramic = 0;
      pMe->m_nProperties &= ~QCAM_PANORAMIC_EFFECT;
      (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PANORAMIC,  pMe->m_nPanoramic, 0);
#endif //FEATURE_CAMERA_PANORAMIC
      IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);
      QcamMenus_BuildDialog(pMe, IDD_MULTI_SHOT, IDI_MULTISHOT_NUM);
    }
  }
}

/* ==========================================================================
FUNCTION QCAM_EXECUTEBESTSHOTMENU
DESCRIPTION
============================================================================= */
static void QcamMenus_ExecuteBestshotMenu(QCam *pMe)
{
  if (pMe)
  {
    int nRet = 0;
    AEEFixedPointParm p1;

    QCam_UpdateRadioFocus(pMe->m_pIMenu,
                          BestshotIDList,
                          &pMe->m_nBestshot,
                          pMe->m_pResFile);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);

    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_BESTSHOT, (int32)pMe->m_nBestshot, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_BESTSHOT return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    }
    //camera has to change some other settings on BSM. Get it back to reflect the changes.
    if (SUCCESS ==
        ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_EXPOSURE_COMP, (int32 *)&p1, NULL))
    {
      pMe->m_nExposure = p1.nCurrent / 1000;
    }
    else
    {
      pMe->m_nExposure = 0;
    }
    (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_CONTRAST, &pMe->m_nContrast, NULL);
  }
}
#endif // FEATURE_CAMERA_V2

#endif //(FEATURE_APP_QCAMERA)

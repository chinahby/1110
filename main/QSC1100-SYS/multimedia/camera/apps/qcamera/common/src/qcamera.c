/*===========================================================================

FILE: qcamera.c

GENERAL DESCRIPTION
    This file contains a BREW application qcamera. It provides functions to
    view and take pictures.

EXTERNALIZED FUNCTIONS
    QCam_Load

    Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/src/qcamera.c#22 $ $DateTime: 2009/04/15 00:19:29 $ $Author: saigopal $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/15/09   sai   Added support to have USB MSD as the save location
 03/10/09  sm    Moving files from deport to mainline for 8k BMP
 01/15/09  RK    Made changes for ACC, Play shutter sound in callback when Stop preview get done notification. 
 01/02/09  KC    Fixed lint error in Qcamera_GetFileName
12/18/08   KC     Added support for Raw preview capture.
  12/10/08  KC     Setting the audio device to speaker before playing the shutter sound
 11/25/08  RK     Added a New Feature ACC(Audio Camera Concurrency) support if 
                  FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND is defined.
 11/25/08  KC     Calling IDISPLAY_UpdateEx in Frame callback mode
 10/30/08  sch    Add Qcamera KPI (Key Performance Indicator) messages.
 10/23/08  KC     Got the featurisations correct when YCbCr encoding is supported in6K
 10/20/08  KC     Setting the qostimer to 89 sec for the standalone gpsfixes.
 10/10/08  RK     Fixed the CR While launching QVP app, call gets droped. 
 09/30/08  SV     Added the check to make sure that camera is not launched, when VT is in progress.
 09/02/08  RK     Panaromic does not support raw format while taking snapshot. 
 08/11/08  KC     Made changes to make a decision on what Multishot mode supported during runtime
                  This decision is made while creating camera instance.
 07/07/08  KC     Changed the active menu state to QCAM_SOFT_MENU_NONE in Qcamera_MultishotCleanup.
                  This is to ensure that the active Menu state is not Multishot after the cleanup is done.
 07/07/08  KC     Changed the active menu state to QCAM_SOFT_MENU_NONE after encoding one picture
                  when FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE is defined.
 04/24/08  KC    Fixed more KLock errors 
 04/10/08  KC    Fixed KLock errors 
 03/26/08  KC     Modified the ecode to handle Multishot during automation.
 03/06/08  KC     Corrected the parameter names in Qcamera_DiagsetProperties() to fix WVGA automation issue
 12/20/07  NC     Added support to FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE. 
 11/13/07  KV     Fixed the bug in the Memoory usage block by sending the correct
                  memory info on request.
 10/26/07  NC     Fixed AFR variables initial bug.
 10/26/07  SK     Fixed corruption of qcamera symbols in TRACE32
 10/17/07  KC     Calling ICAMERA_SETPARM with the current panoramic state.
 10/11/07  KV     Fixed the Disabling the Panaroma if frame overlay enabled.
 09/25/07  KV     Removed the AF ICON from the title bar if not in focus menu option.
 09/24/07  KV     Disabling the panaroma if frame overlay is selected.
 09/24/07  KV     IDIAG support for Panoramic feature.
 09/19/07  KV     Fixed the Alignment of the overlay on the preview in Panorama mode.
 09/14/07  KD     Removed deprecated function calls
 09/06/07  KV     Disabling Panorama while changing the orientation during runtime.
 09/05/07  NC     Fixed some compiling issues with feature condition change.
 08/30/07  KD     Added feature conditions for multishot and panaromic features
 08/21/07  KV     IDIAG Support for Multishot feature.
 08/20/07  SK     UI Changes for Panoramic Stitch feature
 08/15/07  KD     Changed UI for Multishot Enhancement feature
 08/02/07  NC     Added automation support to some settings,
                  Fixed potential compiling issue on some FEATURE on/off combinations.
                  Fixed potential crash while failed getting init value from camera.
 07/31/07  KV     Change from IDisplay_MeasureText to IDISPLAY_MeasureText.
 07/31/07  KV     Added condition FEATURE_CAMERA7500 for QCAM_VGA_WIDTH.
 07/30/07  KV     Fixed Icon Text Overlap Issue for VGA and WVGA displays.
 07/26/07  NC     Fixed automation issues on GPS setting and when direct mode is on.
 07/26/07  RK     Fixed the UI issue when Self timer status bar is running and press any key.
 07/25/07  KV     Provision to Restarting the AF, if "0" key is pressed only after
                  "CAM_STATUS_DONE " message
 06/28/07  RK     Fixed the crash while capturing PNG images in multishot mode during In-Call.
 06/07/07  NC     Cleared screen before display message on take picture.
 06/06/07  NC     Fixed crash in QCamera_CheckListFps when FPS list is not supported.
 05/31/07  NC     Added WVGA automation support.
 05/25/07  KV     Add New Function QCamera_CheckListFps for setting the intial values
                  and checks High and Low Fps values in Fps_List.
 05/28/07  NC     Removed AFR workaround in automation test.
 05/22/07  SK     JPEG Decoder Resizing feature
 05/24/07  NC     Fixed wrong resourse settings for WVGA screen
 05/21/07  NC     Added state checking to Qcamera_DiagGetSensorInfo and Qcamera_DiagIsSupport
 05/17/07  NC     Log error codes in qcamera init to help debug. 
 05/10/07  KV     Fixed Failed to take pictures after removing MMC.
 05/10/07  KV     Added 1-Button Snapshot option to Setting->Properties menu.
 05/09/07  JG     Fixed previous checkin.
 05/07/07  JG     Added code for when control is returned from QMobiCaT
                  to Qcamera_TakePicKeyHandler(). Old way of calling 
                  Qcamera_ToggleMenuDisplay() doesn't work on targets
                  without MDP support.
 04/26/07  NC     Fixed compiling warning
 04/26/07  NC     Fixed lint errors.
 04/25/07  KV     Added FEATURE_CAMERA_BURST_MODE for MultiShot and removed 
                  "Enabled with Auto Exposure" from 7K multishots option.
 04/24/07  SK     Fixed compilation issue on 7k
 04/23/07  NC     Fixed some lint/compiler issues.
 04/19/07  SK     UI Support for WVGA Display
 04/19/07  NC     Compiling error if FEATURE_OVERLAY is defined.
 04/17/07  NC     Workaround LCD offset issue due to low layer changes.
 04/17/07  TV     Don't Adjust the display/preview image size if FEATURE_OVERLAY is defined
 04/10/07  KV     Added ICamera_GetParm for HUE, Satuartion and Effects to update the Display title.
 04/05/07  NC     Added feature condition for QCamera_GetOvObj to avoid compiler warning
 04/04/07  NC     Set Antibanding default to Auto under feature condition.
 04/03/07  KV     Introduced 25s UI option for snapshot timer 
 04/02/07  NC     Returned valid rc in Qcamera_GetPreviewRect on LCD size display.
 04/02/07  SK     Small change in the logic while comparing display and image sizes to avoid negative results 
 04/02/07  SK     Return the display size directly if image orientation is the same as display 
                  in Qcamera_GetPreviewRect 
 03/28/07  TV     If we are not using the full screen set the display size to fit on
                  the screen, while keeping the aspect ratio.
 03/23/07  KV     Changed from default font size to dynamic font size 
                  to fit the pitch of the display.
 03/21/07  NC     Cleared AF rects after time out.
 03/19/07  NC     Fixed 9-grid display issue for 7K.
 03/15/07  NC     Fixed a bug that focus rects update on other key events.
 03/14/07  NC     Changed LCD offset condition. It is only required with mdp display.
 03/07/07  ZL     Added FEATURE_CAMERA_MULTI_SENSOR support.
 02/28/07  NC     Enabled direct mode support if FEATURE_CAMERA_LCD_DIRECT_MODE is defined.
 02/26/07  NC     Added case QCAM_MULTI_DISP to Qcamera_DiagIsSupport.
                  Put case CAM_STATUS_UPDATE under FEATURE_CAMERA_HISTOGRAM condition.
 02/23/07  NC     Only init the TV display on user selecting TV out feature.
 02/15/07  NC     Call set parm to sync camera with UI AFR high/low end setting.
 02/15/07  NC     Fixed compiler issue if Histogram feature is not defined.
 02/14/07  NC     Fixed snapshot on prompt to save dialog display issue.
 02/14/07  NC     Redesigned preview histogram feature.
 02/13/07  NC     Fixed full screen transparent issue.
 02/06/07  NC     Optimized draw focus rect feature.
 02/02/07  NC     Temp solution for WVGA LCD before service layer support.
                  Removed compiler warning #68.
 01/29/07  NC     Remove some preview start delay and screen flash.
 01/24/07  TV     Added ability to have variable default screen size and don't add offset
                  to bitblt for smallscreen
 01/11/07  SK     Allocating more space if Image Size is STAMP and Encoding Type[Mem-mode]
 01/11/07  ZL     Added support for one button AF & snapshot.
 01/09/07  NC     Added automation support for AF mode metering.
 01/03/07  SK     Redesiging the multiple Focus Rectangles.
 12/27/06  ZL     Added FEATURE_CAMERA_MOBICAT_AUTOTUNE for QMobicaT auto tuning.
 12/12/06  GM     Changes to get preview working if in FrameCallBack
                  Mode.
 11/29/06  NC     Fixed IMMOverlay full screen register issue.
 11/27/06  TV     Added FEATURE_CAMERA_V2 check around use of IDS_PREVIEWFPS_HIGH
 11/22/06  ZL     Switched to use ISTATIC at memory usage screen.
 11/21/06  NC     Replace IOverlay with IMMOverlay for BREW 4.0.
 11/20/06  NC     Added shutter sound concurrence support.
 11/17/06  NC     Fixed wrong size issue with Max size option.
 11/15/06  NC     Optimized QMobicaT automation feature and solved target compatible issue.
 11/15/06  SG     IDIAG Support for QMobicaT
 11/12/06  NC     Fix a set size bug when user select max size option.
                  Enable m_bCanTakePic flag when first frame back.
 11/06/06  RK     Fixed the bug of the high and Low FPS.
 11/02/06  NC     Added multiple displays feature for preview.
 10/31/06  NC     Fixed main menu title display issues and focus failure callback issue.
 10/12/06  NC     Merged with 7K features.
 09/19/06  NC     Added AFR High End support to DiagGetProperty.
                  Recerved 10K bytes in MEM mode encoding buffer for JPEG header.
 09/13/06  NC     Fixed lint errors.
 09/12/06  NC     Released frame right after add overlay.
 09/07/06  NC     Added automation support for Multishot feature and IsSupport.
 09/05/06  NC     Set condition FEATURE_CAMERA_SUPPORT_ICON_ARRAY for icon overlay.
 09/01/06  NC     Filter out file/function for target compatible.
 08/29/06  NC     Added IDS_LCDSIZE_DISP to automation support.
 08/25/06  NC     Added UI support to simulate 2nd display.
 08/25/06  NC     Added UI support to enable LCD size display.
 08/22/06  NC     Redesigned launch qalbume on add frame mode to solve memory issue.
 08/15/06  NC     Added SXGA and UXGA frame overlay support.
 08/15/06  NC     Remove Text option from Bestshot menu per servise change.
 08/14/06  NC     Block Burst mode on Multishot take picture.
 08/07/06  NC     Added FPS High end setting support.
 07/28/06  NC     Updated UI logic to make HJR and ISO mutually exclusive.
 07/25/06  NC     Added automation support for Bestshot mode.
 07/24/06  NC     Changed logic in SUSPEND qcamera to set only one non-exit condition.
 07/19/06  NC     Fixed a bug in Qcamera_InitVals  and set checking for new features.
 07/13/06  NC     Added multishot and bestshot UI supports.
 07/12/06  NC     Added automation support for FPS list.
 07/06/06  NC     Fixed camera out sync issue caused by continue pressing Sel key quickly.
 06/20/06  NC     Added more F3 message to help debug.
 06/15/06  NC     Defined 40 pixels offset to recove camera shift for small image display.
 06/14/06  JR     Added support for FTL memory.
 06/08/06  NC     Added a new function Qcamera_GetListFPS to get fps list.
                  Added a new function Qcamera_InitVals to simpligy init code.
                  Added temp workaround for automation test of new fps approach.
                  Fixed a bug in Qcamera_DiagDetProperties.
 06/02/06  NC     Added F3 message for Mem Mode encoding debug.
 05/26/06  NC     Enabled Auto Exposure mode automation for all targets.
                  Returned max. zoom value for automation test if dwParam is 1.
                  Set HJR and LA default values to disabled on camera start up.
 05/19/06  NC     Blocked focus key events when focus is in processing.
 05/05/06  NC     Update Qcamera_TakePicKeyHandler logic to skip some unexpected key events.
                  Reserve one file size to add accurate in calculating memeory space.
                  Fixed a bug in title display after cancel selftimer.
 05/01/06  NC     Filter out unused features to save memory space.
                  Set antibanding default to auto if it supports auto option.
 04/25/06  NC     Added dynamic auto antibanding support to automation.
                  Kept overlay frame intact on orientation change.
 04/21/06  JR     Added sharpness icon and control code for SC1x.
 04/06/06  NC     Display hour glass on setting menus that have long start processing.
                  SetDisplaySize workaround for SDK.
 04/03/06  NC     Converted m_pLaunchDS into a char string for album mode and slideshow mode.
                  Exited qcamera on launch qalbum for album or slideshow modes.
                  Fixed potential memory leak on launch qalbum failure.
 04/03/06  NC     Set only one fixed display size per service change.
 03/20/06  NC     Added VGA frame overlay support.
 03/06/06  NC     Added optical/digital zoom identifier.
 02/28/06  NC     Added checking for burst mode automation.
 02/28/06  NC     Added automation support for burst mode take picture and smooth zoom.
 02/14/06  NC     Disable smooth zoom for SC1x target and automation.
 02/08/06  NC     Added dynamic menu support to automation test.
                  Reported automation error on abnormal termination of test.
                  Added ISO and AF mode automation support.
 01/30/06  JN     Simple file rename reverted for MobiCaT camera_tune effort.
 01/19/06  JN     Added smooth zoom support.
 01/18/06  JR     Added support for small screen sizes as on SC1x target.
 01/12/06  NC     Rewrite some logic to improve the performance.
 12/12/05  NC     Fixed a bug that Sel key will block the 2nd memory full message.
 12/08/05  NC     Added two new items to sensor info for automation test.
 12/07/05  NC     Checking eState instead of GetMode in ReleaseCamera to avoid timing issue.
 12/05/05  NC     Fixed a bug in burst mode with shutter sound.
 11/30/05  NC     Clean up lint errors from Lint v8.0.
 11/14/05  NC     Added icon to toggle MobiCaT feature On/Off
                  when feature FEATURE_CAMERA_MOBICAT is defined.
 11/09/05  JJ     OEMs want FEATURE_CAMERA_DEBUG called FEATURE_CAMERA_MOBICAT
 11/08/05  NC     Update per the new header file.
 11/01/05  NC     Fixed burst mode timing issue.
 10/27/05  NC     Fixed bug in setting max FPS.
 10/26/05  NC     Enabled 9 grids on frame overlay.
 10/24/05  NC     More lint errors clean up.
 10/21/05  NC     Added IDS_SIZE_MAX to Qcamera_DiagSetProperties
 10/20/05  NC     Updated some automation feature and fixed new lint errors.
 10/14/05  NC     Enable actaul sensor size option support.
 10/13/05  NC     Added 3Mp, 4Mp and 5Mp size support.
 10/12/05  NC     Block icon overlay for preview only when image size larger than QVGA.
 10/12/05  SM     Added automation support for prompt to save/delete
 10/06/05  NC     Updated shutter sound fail logic and added log message to
                  catch shutter sound play abort callback.
 10/04/05  NC     Set dynamic FPS menu based on sensor support info.
 09/29/05  NC     Cancel timer before return to preview when
                  the user terminates timed message.
 09/28/05  NC     Added automation support for get MemUsage info.
 09/27/05  SM     Added automation support for get sensor info.
 09/16/05  NC     Added more log messages for camera debug.
 09/16/05  NC     Limit PNG image size option to VGA or smaller.
 09/15/05  NC     Lint errors clean up.
 09/06/05  JJ     Fewer lines for mobile camera tuning:  9 total now.
 09/06/05  NC     Added NULL pointer checking to input parameters.
 08/25/05  NC     Fixed potential memory leak and crashing.
                  Eliminated control icon overlay on Stamp size image.
 08/10/05  NC     Added more error handlers for automation test.
 08/02/05  JJ     One line change for on-screen test and tune included code.
 08/02/05  NC     Added parameter checking to Qcamera_DiagJpegQuality
 08/01/05  SM     Added automation support for both file size and quality set.
 07/29/05  NC     Added automation support for shutter sound get/set.
 07/28/05  NC     Check dynamic size in Qcamera_DiagSetProperties.
 07/27/05  NC     Clear grids before take picture and not toggle grid with frame overlay.
                  Added more error checking in ICamera call back handler.
                  Fixed a bug in Qcamera_GetMaxSizeId.
 07/25/05  NC     Update the max size id and raw image size from sensor info.
 07/22/05  NC     Toggle 9 grids on preview screen by "*" key
 07/14/05  NC     Enable to composite lanscape frame with icons for preview overlay.
 07/08/05  NC     Fixed compilation error when FEATURE_MDP is not defined.
 06/30/05  JJ     Renamed field_debug to camera_tune_mobile, per request.
 06/23/05  JN     Deregister MDP system bitmap before launching Qalbum so that
                  Qalbum can further launch other apps without display problems.
 06/22/05  NC     Updated Qcamera_DiagGetProperties for EV return value.
 06/10/05  NC     Work around incorrect sensor size and mdp toggle menu issue.
 06/09/05  NC     Fixed bugs from MMC removing dynamically and remain picture num.
 06/08/05  NC     Enable shutter sound support on take picture.
 05/31/05  NC     Fixed compilation error when some features are not defined.
 05/26/05  NC     Make sure we get valid index before calling mapping function.
 05/24/05  NC     Updated EV handler per BREW 3.1.4. Added del Raw only option to automation test.
 05/20/05  NC     Added support to delete RAW only and fixed a few bugs.
 05/18/05  JJ     Further shrunk impact of Field Debug (FEATURE_CAMERA_DEBUG) on qcamera.c.
 05/18/05  NC     Added support to save RAW picture based on save location
                  and fixed some bugs in getting path length and picture number.
 05/16/05  NC     Bring up the menu button on dialog start up after BREW 3.1.3.
                  Added Set Slideshow Context for automation test.
 05/12/05  NC     Added flash/redeye state indicator to the second title bar.
                  Fixed wrong number of remain shots in MesUsage screen.
                  Fixed a bug in delete all response for No delete option.
                  Removed BREW version check and Qcamera_ValueInDataList().
 05/09/05  NC     Stopped AF indicator timer on toggle/navigate menu and take picture.
 05/06/05  NC     Added Qcamera_DisplayInfo to display resolution and
                  remain picture number on second title bar.
                  Simulate IDC_SAVE on prompt to save dialog for automation test.
 04/29/05  NC     Simplified Qcamera_DiagGetContext() function.
                  Updated GPS feature handler and added hourglass for get GPS info process.
                  Return Ok status after update frame for tests that return to preview after done.
 04/19/05  NC     Enabled encoding with both file size and quality set.
 04/15/05  NC     Added error status reports for all failure cases of automation test.
                  Simplified automation support code.
                  Moved Qcamera_UpdateAFIndicator() in Qcamera_DisplayTitle() to eliminate display delay.
 04/08/05  NC     Added MMC memory check for space checking.
 04/07/05  NC     Added shared functions to simplify Qcamera_MemUsage().
                  Added MMC memory usage block if MMC available and BREW 3.1.3 is used.
 04/05/05  drl    Changed call to ICAMERA_SetDisplaySize use same size as
                  ICAMERA_SetSize.  This indicates display image dimensions
                  are the same as the size of the selected encoded image dimensions.
 04/04/05  NC     Added more checking and status reports for automation test support.
 03/30/05  NC     Update automation support code to fix some unstable issues.
 03/30/05  NC     Handled icon/stamp overlay on orientation change.
 03/29/05  NC     Solve difference between debug and release mode by feature define.
 03/24/05  NC     Added new functions to display timed messages for reused codes.
                  Added new function to get the matched size frame directory.
 03/23/05  JJ     Shrunk the # of lines used for field_debug, now held in
                  services/camera/field_debug.{c,h} .
 03/22/05  NC     Corrected timer for Qcamera_DisplayPhotoSavedImage().
 03/21/05  NC     Added automation test support for new features and clean up code.
 03/17/05  NC     Corrected the remain picture number in Memory usage page.
 03/16/05  NC     Cleared overlay on orientation change.
 03/15/05  NC     Cleared the frame DIB whenever the frame needs to be updated.
                  Added orientation check to a selected frame.
                  Update logic for adding icons array and clear icons array.
 03/14/05  NC     Added EV feature support to UI and feature out Brightness.
 03/11/05  SM     Fixed automation test bugs.
 03/11/05  JN     Added nightshot mode under setting menu.
 03/10/05  NC     Changed logic to launch qcamcorder to get rid of two tones color in disply
 03/10/05  NC     Eliminated some more lint errors.
 03/09/05  NC     Removed FEATURE_CAMERA_CONTRAST and updated data types of some variable.
                  Removed 100 ms delay between encoding done and returning to preview.
 03/03/05  NC     Modified logic for adding frame overlay to preview.
 03/03/05  SM     Added support for new menu items for automation: antibanding and redeye reduction.
 02/28/05  NC     Start qcamcorder after camera release.
 02/25/05  NC     Fixed a potential page fault bug.
 02/18/05  NC     Let BREW stop camera if incoming call suspend it.
                  Changed focus indicator time out for done to 2s and fail to 500ms.
                  Used name definition for all timeout.
 02/16/05  NC     Reset pMe->m_bCanTakePic flag when Sel key is pressed while timer counting
                  so it can restart the counter again to take picture.
 02/15/05  NC     Release file pointer (close the file) after mem-mode file writing
 02/15/05  JR     Fixed some lint errors.
 02/14/05  NC     Added Antibanding and Red eye reduction variable.
                  Move CAM_PARM_PREVIEW_TYPE up before other param settings.
 01/27/05  NC     Added error handler for error return from record or encode snapshot
 01/26/05  JN     Do not abort Qcamera_Init if failed to create IPOSDET instance.
 01/26/05  NC     Initialize picture orientation to portrait mode.
 01/24/05  NC     Code clean up. Removed unused code.
 01/21/05  NC     Double the file size when checking free space to make sure enough room to save.
 01/19/05  NC     Update current focus step before doing manual focus.
 01/17/05  NC     Code clean up. Updated function names.
 01/14/05  NC     Use IOverlay for MDP feature.
 01/04/05  NC     Code clean up. replace QCam.h with qcamera_i.h.
 12/30/04  NC     Fixed data Abort on setting the Frame Overlay option for preview and Encode.
 12/10/04  NC     Update auto test handler and fix bugs.
 12/07/04  NC     Added DIAG features for Auto Test. Fixed bug in QCam_GPSHandler and focus
 11/29/04  NC     Added 300ms delay to call QCam_TakePicture in burst mode to work around the timing issue.
 11/22/04  NC     Removed QCam_Get565TransparentVal() to QCamDisp.c for sharing
 11/19/04  NC     Updated Delete All menu radio icon on Up/Down key.
 11/18/04  NC     Added a new function QCam_GetRemainPicNum() and fixed a few bugs.
 11/17/04  NC     Fixed display not updated on starting QCamera from more apps menu
 11/16/04  NC     Added new options for delete all.
 11/12/04  NC     Updated Save file logic to avoid potential failure due to timing issue.
 11/10/04  NC     Fixed bug in Delete All file feature.
 11/05/04  NC     Added the saved file name to the title bar and clear image on save message.
 11/05/04  NC     Simplified Event handlers and returned to preview after memory full message.
 11/04/04  NC     Updated logic for prompting and response to delete all option
 11/04/04  NC     Display image on save and fixed some display bugs
 11/02/04  NC     Enabled Focus and Auto Exposure mode features
 10/21/04  NC     Updated Focus feature based on BREW 3.1.2.
 10/19/04  NC     Added QCam_ResetAFIndicator and update AF indicator on callback
 10/18/04  NC     Added Focus feature APIs and featurized Contrast
 10/15/05  NC     Implemented MDP featuer on save a photo
 10/14/04  NC     Add MEM mode encoding type. Remove shared MDP functions
 10/13/04  JN     Fixed QCamViewer launch interface.
 10/12/04  NC     Update for key event handler.
 10/11/04  NC     Merge MDP feature.
 09/21/04  NC     Add more features.
 07/7/04   NC     Initial design for qcamera re-architecture
============================================================================*/

//Deal with lint problems in BREW headers
//lint -e740, Unusual pointer cast for BREW macros
//lint -e611, Suspicious cast for BREW macros

//lint -emacro(570,IDISPLAY_ClearScreen)
//lint -emacro(570,IDISPLAY_EraseRect)
//lint -emacro(570,IDISPLAY_FillRect)
//lint -emacro(732,ISHELL_LoadResBitmap)

//lint -e818, Pointer parameter 'pMe' could be declared as pointing to const
//lint -e801, Use of goto is deprecated
//lint -e826, Suspicious pointer-to-pointer conversion (area too small)

//lint -e766, Header file stdlib.h not used
//lint -e765, Qcam_CreateInstance, Qcam_Load, Qcamera_GetModInfo could be
//            made static
//lint -e714, Qcamera_GetModInfo not referenced

/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "qcamera_i.h"

#ifdef FEATURE_APP_QCAMERA
#include "QcamDef.h"
#include "qalbum.bid"
#ifndef AEE_SIMULATOR
#include "msg.h"
#endif

#ifdef FEATURE_OVERLAY
#ifdef FEATURE_MMOVERLAY
#include "AEEMMOverlay.h"
#include "AEECLSID_MMOVERLAYMGR.BID"
#else
#include "Overlay.bid"
#endif // #ifdef FEATURE_MMOVERLAY
#endif //FEATURE_OVERLAY

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#ifdef FEATURE_APP_QCAMCORDER
#include "Qcamcorder.bid"
#endif

#ifdef FEATURE_CAMERA_MULTI_SENSOR
#include "AEECamera.bid"
#endif //FEATURE_CAMERA_MULTI_SENSOR

#ifdef FEATURE_CAMERA_MOBICAT
#include "camera_tune.h"
extern QCam * camera_tune_the_host_brew_app;
extern boolean camera_tune_was_enabled_at_startup;
extern boolean camera_tune_return_control_to_app;
#endif //FEATURE_CAMERA_MOBICAT

#ifdef HEAP_CHECK
  #include "AEEHeap.h"
  #include "malloc_mgr.h"
  #define MSG_HEAP MSG_ERROR
  static uint32  dwOtherUsage = 0;
  void Qcamera_ShowHeapUsage(QCam *pMe);
#endif

#ifndef QCAM_LCD_OFFSET
#define QCAM_LCD_OFFSET 0
#endif

/*=========================================
static application
=====================================*/
#if defined(BREW_STATIC_APP)
int QCam_CreateInstance(AEECLSID ClsId,IShell *pIShell, IModule *po, void **ppObj);
int QCam_Load(IShell *ps, void *pHelpers, IModule **pMod);

//Static Data...
static AEEAppInfo gaiQCam = {AEECLSID_QCAMERA, QCAMERA_RES_FILE,
                             IDS_TITLE, 0, 0, 0, 0, AFLAG_TOOL};


PFNMODENTRY Qcamera_GetModInfo(IShell *ps, AEECLSID **ppClasses, AEEAppInfo **pApps, uint16 *pnApps, uint16 *pwMinPriv)
{
  *pApps = (AEEAppInfo *)&gaiQCam;
  *pnApps = 1;
  //lint -save -e715 symbol ps, pwMinPriv and ppClasses not referenced here
  return((PFNMODENTRY)QCam_Load);
  //lint -restore
}

int QCam_Load(IShell *ps, void *pHelpers, IModule **pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)), ps, pHelpers, pMod, QCam_CreateInstance, NULL));
}

#endif  //BREW_STATIC_APP

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */
/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
   This function is invoked while the app is being loaded. All Modules must provide this
   function. Ensure to retain the same name and parameters for this function.
   In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
   that has been provided in AEEAppGen.c.

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData().
   This is done as follows: InitAppData() is called to initialize AppletData
   instance. It is app developers responsibility to fill-in app data initialization
   code of InitAppData(). App developer is also responsible to release memory
   allocated for data contained in AppletData -- this can be done in
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
   clsID: [in]: Specifies the ClassID of the applet which is being loaded

   pIShell: [in]: Contains pointer to the IShell object.

   pIModule: pin]: Contains pointer to the IModule object to the current module to which
   this app belongs

   ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
   of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(BREW_STATIC_APP)
int QCam_CreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
#endif
{

#ifdef FEATURE_CAMERA_SNAPSHOT_PROFILING
#error code not present
#endif	/* FEATURE_CAMERA_SNAPSHOT_PROFILING */
  if (!ppObj)
    return EFAILED;

  *ppObj = NULL;
#ifdef FEATURE_CAMERA_SNAPSHOT_PROFILING
#error code not present
#endif /*FEATURE_CAMERA_SNAPSHOT_PROFILING*/
  if (ClsId == AEECLSID_QCAMERA)
  {
    if (AEEApplet_New(sizeof(QCam), ClsId, pIShell,po,(IApplet**)ppObj,
                      (AEEHANDLER)Qcamera_HandleEvent,
                      (PFNFREEAPPDATA)Qcamera_FreeAppData))
    {
      if (!(*ppObj))
        return EFAILED;

      if (Qcamera_InitData((QCam *) *ppObj) == TRUE)
        return(AEE_SUCCESS);
    }
  }
  return EFAILED;
}

/*===========================================================================

  FUNCTION: Qcamera_InitData

   DESCRIPTION:
    This function initializes app specific data, allocates memory for app
    data. App developer needs to fill-in the initialization code.

  PROTOTYPE:
    boolean Qcamera_InitData(QCam *pMe);

  PARAMETERS:
      pMe [in]: Pointer to the QCam structure. This structure contains
      information specific to this applet. It was initialized during the
      AEEClsCreateInstance() function.

  DEPENDENCIES:
     Assumes pMe is not NULL

  RETURN VALUE
   TRUE: If the app has app data is allocated and initialized successfully
   FALSE: Either app data could not be allocated or initialzied

  SIDE EFFECTS:
  none
===========================================================================*/
static boolean Qcamera_InitData(QCam *pMe)
{
  if (!pMe)
    return FALSE;

#ifdef HEAP_CHECK
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_HEAP, (void **)&pMe->m_pIHeap)
           == AEE_SUCCESS)
   {
      AEEDeviceInfo deviceInfo;
      deviceInfo.wStructSize = sizeof(AEEDeviceInfo);
      ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &deviceInfo);
      dwOtherUsage = IHEAP_GetMemStats(pMe->m_pIHeap);
      MSG_HEAP("Qcamera_InitData: total heap %d, other usage %d",
               deviceInfo.dwRAM, dwOtherUsage, 0);
      Qcamera_ShowHeapUsage(pMe);
   }
#endif

  ISHELL_GetDeviceInfo (pMe->a.m_pIShell, &pMe->di);

  // Check for SC1X screen.
  if ((pMe->di.cxScreen <= SMALL_SCREEN_WIDTH) && 
      (pMe->di.cyScreen <= SMALL_SCREEN_HEIGHT))
  {
    pMe->m_bSmallScreen = TRUE;
  }
  else
  {
    pMe->m_bSmallScreen = FALSE;
  }

#ifdef FEATURE_CAMERA7500
  //Check if it uses larger screen (VGA or WVGA)
  if (QCamDisp_IsSizeScreen(pMe->di.cxScreen, 
                            (uint16)(pMe->di.cyScreen + QCAM_LCD_OFFSET), 
                            (int)QCAM_SIZE_VGA) || 
                            (pMe->di.cxScreen == QCAM_WVGA_WIDTH))
  {
    int16 font_size_vga;
#ifndef AEE_SIMULATOR
    gaiQCam.pszMIF = QCAMERA_VGA_RES_FILE;
#endif
    pMe->m_pResFile = QCAMERA_VGA_RES_FILE;
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
   // pMe->m_softMenuHeight = QCAM_SOFT_MENU_HEIGHT_VGA;

   // Check for font size and assign the larger one to pMe->m_nTitleHeight 		
    font_size_vga =	(int16)IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay, 
                                                   QCAM_FONT_TITLE, 
                                                   NULL, NULL);
    if(font_size_vga > QCAM_PICTURE_Y_POS_VGA)
    {
      pMe->m_nTitleHeight = font_size_vga + 2;
      pMe->m_softMenuHeight = (uint16)(font_size_vga * 2) + 2;
    }
    else
    {
      pMe->m_nTitleHeight = QCAM_PICTURE_Y_POS_VGA;
      pMe->m_softMenuHeight = QCAM_SOFT_MENU_HEIGHT_VGA;	   
    }	
  }
  else
#endif //FEATURE_CAMERA7500
  {
    pMe->m_pResFile = QCAMERA_RES_FILE;    
    pMe->m_softMenuHeight = QCAM_SOFT_MENU_HEIGHT;    
    pMe->m_nTitleHeight = QCAM_PICTURE_Y_POS;
  }

  QCamDisp_GetDisplayRect(pMe->di.cxScreen, pMe->di.cyScreen, pMe->m_softMenuHeight,
                          &pMe->m_rcDisp, pMe->m_nTitleHeight);

  //Init interface pointers
  pMe->m_pICam = NULL;
  pMe->m_pIMenu = NULL;
  pMe->m_pISoftMenu = NULL;
  pMe->m_pIFileMgr = NULL;
  pMe->m_pMedia = NULL;
  pMe->m_pFrameImg = NULL;
  pMe->m_pIPosDet = NULL;
  pMe->m_pFrame = NULL;
  pMe->m_pFrameDIB = NULL;
  pMe->m_pIStatic = NULL;
  pMe->m_pData = NULL;
  pMe->m_pSoundList = NULL;

  pMe->m_eState = QCAM_STATE_NULL;
  pMe->m_pLaunchDS = NULL;
  pMe->m_activeMenu     = QCAM_MENU_NONE;
  pMe->m_activeSoftMenu = QCAM_SOFT_MENU_NONE;

  pMe->m_sCurrentFileName[0] = '\0';

  pMe->m_nSelSound = 0;
  pMe->m_nTotalSound = 0;
  pMe->m_nFileSize = 0;
  pMe->m_nLeftSec = 0;
  pMe->m_nContrast = 0;
  pMe->m_nSharpness = 0;
  pMe->m_nBright = 0;
  pMe->m_nExposure = 0;
  pMe->m_nZoom = 0;

  pMe->m_nEffect = CAM_EFFECT_OFF;
  pMe->m_nAntibanding = CAM_ANTIBANDING_OFF;
  pMe->m_wTakePicMenuSel = IDS_TPMENU_START;
  pMe->m_bRedeyeReduce = FALSE;
  pMe->m_bNightshotMode = FALSE;
  pMe->m_nDelOpt = IDS_NO;
  pMe->m_bSaveAppPic = FALSE;
  pMe->m_bUpdatePreviewFrame = TRUE;
  pMe->m_bKeyDown = FALSE;
  pMe->m_bNeedToSend = FALSE;
  pMe->m_bGPSmenu = FALSE;
  pMe->m_bFocusStarted = FALSE;
  pMe->m_bOneButtonSnapshot = FALSE;
  pMe->m_bMsgTimerOn = FALSE;
  pMe->m_nMsgId = 0;
  pMe->m_bGridOn = FALSE;
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND  
  pMe->m_bSnapCaptureDone = FALSE;
  pMe->m_bSnapShutterSndDone = FALSE;
  pMe->m_bNeedToEncodePicture = FALSE;
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND 

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  pMe->m_bIconLoaded = FALSE;
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  ZEROAT(&pMe->m_GPSInfo);
  ZEROAT(&pMe->GPSInfoCB);
  pMe->m_eGPSState = QCAM_GPS_DISABLED;
  pMe->m_nGPSAccuracy = AEEGPS_ACCURACY_HIGHEST;
  pMe->m_bHasAutoAntibanding = FALSE;
  pMe->m_bLASupported = FALSE;
  pMe->m_nAFMode = 0;
#ifdef FEATURE_CAMERA7500
  pMe->m_nAFModeMetering = 0;
  pMe->m_nRectcount = 0;
  pMe->m_pRect = NULL;
#endif //FEATURE_CAMERA7500
  pMe->m_pListFPS = NULL;
  pMe->m_bDisplayLCDSize = FALSE;
  pMe->m_b2ndDisplay = FALSE;
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#ifdef FEATURE_OVERLAY
#ifdef FEATURE_MMOVERLAY
  pMe->m_pIMMOverlayMgr = NULL;
  pMe->m_pIMMOverlayFull = NULL;
  pMe->m_pIMMOverlayInfo = NULL;
  pMe->m_pIMMOverlayTitle = NULL;
  pMe->m_pIMMOverlayMenu = NULL;
#ifdef FEATURE_CAMERA7500
  {
    int i = 0;
    while (i < QCAM_MAX_FOCUSRECTS)
    {
      pMe->m_pIMMOverlayRect[i++] = NULL;
    }
  }
#endif //FEATURE_CAMERA7500
#else
  pMe->m_pIOverlay = NULL;
#endif // FEATURE_MMOVERLAY
#endif // FEATURE_OVERLAY
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
  pMe->m_coreUIClsId = 0;
  pMe->m_pOvFrame = NULL;
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef FEATURE_CAMERA_MOBICAT
  pMe->m_bMobiCaTon = FALSE;
#endif
#ifdef FEATURE_CAMERA_HISTOGRAM
  pMe->m_pBmpHist = NULL;
  pMe->m_pOvHist = NULL;
  pMe->m_nPrvwHistogram = CAM_HISTOGRAM_OFF;
#endif // FEATURE_CAMERA_HISTOGRAM
#ifdef FEATURE_CAMERA_PANORAMIC
  pMe->m_pOvPanoramic = NULL;
#endif //FEATURE_CAMERA_PANORAMIC
  pMe->m_nPanoramic = 0;

  return TRUE;
}

/*===========================================================================
FUNCTION: Qcamera_Init

  DESCRIPTION:
  This function allocates the classes and interfaces for all the
  services required by QCam. It also reads the config file to update
  some variables.

  PROTOTYPE:
    void Qcamera_Init(QCam * pMe);

  PARAMETERS:
   QCam * pMe - Pointer to the applet instance

  RETURN VALUE:
      None
===========================================================================*/
static boolean Qcamera_Init(QCam *pMe)
{
  int nErr = SUCCESS;
  boolean result = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  if (!pMe)
    return FALSE;

  //Create instance for pMe->m_pIFileMgr
  nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,
                               (void **) &pMe->m_pIFileMgr);
  if (nErr != SUCCESS)
  {
    MSG_HIGH("Failed creating m_pIFileMgr with err code %d.", nErr, 0, 0);
    return FALSE;
  }
  //Create instance for pMe->m_pIPosDet and register for GetGPSInfo callback
  //This interface is for get GPS information only, if it is failed to get the
  //interface, we still start the camera app. But need to check the pointer
  //whenever use it.
  nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_POSDET,
                               (void **)&pMe->m_pIPosDet);
  if (nErr != SUCCESS)
  {
    MSG_MED( "Failed to create IPOSDET with err code %d.", nErr, 0, 0);
  }
  else
  {
    CALLBACK_Init(&pMe->GPSInfoCB, Qcamera_GetGPSInfoCB, pMe);
  }

  //Create instance for pMe->m_pISoftMenu and pMe->m_pIMenu
  nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL,
                               (void **) &pMe->m_pISoftMenu);
  if (nErr == SUCCESS)
  {
    nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL,
                                 (void **) &pMe->m_pIMenu);
    if (nErr != SUCCESS)
    {
      MSG_HIGH("Failed creating m_pIMenu with err code %d.", nErr, 0, 0);
    }
  }
  else
  {
    MSG_HIGH("Failed creating m_pISoftMenu with err code %d.", nErr, 0, 0);
  }
  if (nErr != SUCCESS)
  {
    CAM_RELEASEIF(pMe->m_pISoftMenu);
    CAM_RELEASEIF(pMe->m_pIMenu);
    CAM_RELEASEIF(pMe->m_pIPosDet);
    CAM_RELEASEIF(pMe->m_pIFileMgr);
    return FALSE;
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  if (nErr == SUCCESS)
  {
    /* Need to have m_pIFileMgr assigned before checking for FTL memory. */
#ifdef FEATURE_EFS_FTL
    pMe->m_bFTL = (IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_FTL_DIR) == SUCCESS);
    MSG_MED("FTL memory test returns %d",pMe->m_bFTL,0,0);
#else
    pMe->m_bFTL = FALSE;
    MSG_MED("FTL memory unavailable",0,0,0);
#endif

    //Get some default values from last saved config file
    Qcamera_GetConfigRecord(pMe);

    //Initialize menu controls
    QCam_SetMenuStyle(pMe->m_pISoftMenu, 0, 0);
    QCam_SetMenuStyle(pMe->m_pIMenu, 2, 0);
    IMENUCTL_SetProperties(pMe->m_pISoftMenu, MP_MAXSOFTKEYITEMS);
    //Default menu to display
    pMe->m_bShowMenu = TRUE;

#ifdef FEATURE_CAMERA_MULTI_SENSOR
    //Initialize the primary sensor id
    pMe->m_nSensorId = 0;
#endif //FEATURE_CAMERA_MULTI_SENSOR

    //Clear the old screen before init MDP in case it not go away.
#ifdef FEATURE_OVERLAY
#ifdef FEATURE_MMOVERLAY
    //Create instance for pMe->m_pIOverlayMgr and pMe->m_pIMMOverlayMgr
    nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MMOVERLAYMGR,
                                 (void **)&pMe->m_pIMMOverlayMgr);
    if (nErr == SUCCESS)
    {
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
      pMe->m_coreUIClsId = QCamDisp_GetOldDispClsId(pMe->a.m_pIShell);
      if (QCam_CheckProperties(pMe->m_nProperties, QCAM_MULTI_DISP))
      {
        if (QCamDisp_ChangeDisplay(pMe->a.m_pIShell, AEECLSID_DISPLAY3, 1) != SUCCESS)
        {
          MSG_HIGH("Failed QCamDisp_ChangeDisplay.", 0, 0, 0);
        }
      }
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
    }
    else
    {
      MSG_HIGH("m_pIMMOverlayMgr Creation Failed with err code %d", nErr, 0, 0);
    }
#else
    //Create instance for pMe->m_pIOverlay
    nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_OVERLAY1,
                                   (void **) &pMe->m_pIOverlay);
    if (nErr == SUCCESS)
    {
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
      //One of the mdp init function is still not ready in IOverlay
      //so we still have to call this function now.
      QCamDisp_InitMdp(pMe->a.m_pIDisplay);
    }
    else
    {
      MSG_HIGH("m_pIOverlay Creation Failed with err code %d", nErr, 0, 0);
    }
#endif //#ifdef FEATURE_MMOVERLAY
#endif // FEATURE_OVERLAY

#ifdef FEATURE_QVPHONE
    nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_VIDEOPHONE, (void **)&pMe->m_pIVP);
    if (nErr != SUCCESS)
    {
      MSG_HIGH("Failed creating Video Phone instance with err code %d.", nErr, 0, 0);
    }
#endif /* FEATURE_QVPHONE */
  }

  //Create camera when all other initialization is ok.
  if (nErr == SUCCESS)
  {
    result = Qcamera_CreateCamera(pMe);
  }
  //Clean up before exit if failed to init
  if (!result)
  {
    CAM_RELEASEIF(pMe->m_pISoftMenu);
    CAM_RELEASEIF(pMe->m_pIMenu);
    CAM_RELEASEIF(pMe->m_pIPosDet);
    CAM_RELEASEIF(pMe->m_pIFileMgr);

#ifdef FEATURE_OVERLAY
#ifdef FEATURE_MMOVERLAY
    CAM_RELEASEIF(pMe->m_pIMMOverlayMgr);
#else
    CAM_RELEASEIF(pMe->m_pIOverlay);
#endif //#ifdef FEATURE_MMOVERLAY
#endif // FEATURE_OVERLAY
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#ifdef FEATURE_QVPHONE
  if (pMe->m_pIVP)
  {
    (void)IVIDEOPHONE_Release( pMe->m_pIVP );
    pMe->m_pIVP = NULL;
  }
#endif /* FEATURE_QVPHONE */
  }
  return result;
}


/*=====================================================================
FUNCTION Qcamera_InitVals

  DESCRIPTION: This functio calls ICAMERA_GetParm and fills the parameters
               with the returned values if it is sucessful in GetParm

  PARAMETERS:
    pMe     - pointer to QCam data struct
    nId     - Parameter ID for getting the value
    nStep   - Default step factor. Could set to 1 if no need to concern.
    pVal    - Poniter to Current value
    pMin    - Poniter to Minumum value
    pMax    - Poniter to Maxmum value
    pStep   - Poniter to Step for value increase/decrease

  RETURN VALUE:
    void
======================================================================*/
static void Qcamera_InitVals(QCam *pMe, int16 nId, int32 nStep, int32 *pVal,
                             int32 *pMin, int32 *pMax, int32 *pStep)
{
  AEEParmInfo pInfo = {0};

  if (!pMe || !pVal || !pMin || !pMax || !pStep)
    return;

  if (ICAMERA_GetParm(pMe->m_pICam, nId, NULL, (int32 *)&pInfo) != SUCCESS)
  {
    MSG_HIGH("Qcamera_InitVals error - Item ID %d", nId, 0, 0);
    *pStep = 1;
    return;
  }

  *pVal = pInfo.nCurrent;
  *pMin = pInfo.nMin;
  *pMax = pInfo.nMax;
  *pStep = pInfo.nStep;

  if ((nStep > 1) && (*pStep < ((*pMax - *pMin) / nStep)))
    *pStep = (*pMax - *pMin) / nStep;
  if (*pStep < 1)
    *pStep = 1;
}


/*=====================================================================
FUNCTION *Qcamera_GetListFPS

  DESCRIPTION: This function gets the FPS list and copys it to a buffer.
               The buffer is dynamically allocated and must be freed before
               the application exits.
  DEPENDENCY:  The ICAMERA_GetParm must return a NULL terminated list so it won't
               hang the while loop that for getting the size of the list.

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    uint32 - Address of the FPS list buffer.
======================================================================*/
uint32 *Qcamera_GetListFPS(QCam *pMe)
{
  uint32 *pList = NULL;
  uint32 nVal = CAM_MODE_PREVIEW;
  uint32 nSize = 0;

  if (!pMe)
    return NULL;

  if (ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_FPS_LIST, (int32 *)&nVal, NULL) == SUCCESS)
  {
    pList = (uint32 *)nVal;
    while (*pList)
    {
      nSize++;
      pList++;
      //in case infinite loop from no NULL temination of pList
      if (nSize > 30)
        break;
    }
    if ((nSize > 0) && (nSize < 30))
    {
      pList = (uint32 *)MALLOC(sizeof(uint32) * (nSize + 1));
      if (pList)
      {
        (void)MEMCPY(pList, (uint32 *)nVal, sizeof(uint32) * (nSize + 1));
      }
    }
    else
      pList = NULL;
  }
  return pList;
}

/*=====================================================================
FUNCTION *Qcamera_ChecktListFPS

  DESCRIPTION: This function Checks High_FPS and Low_FPS values in the FPS list,
              These values are repelaced with max and min values if there are not present in the list.
              
  DEPENDENCY: The ICAMERA_GetParm must return a NULL terminated list so it won't
               hang the while loop that for getting the size of the list.

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE: void
    
======================================================================*/
static void Qcamera_CheckListFps(QCam *pMe)
{
  AEEParmInfo pInfo;
  uint32 *pFpsList = NULL;
  boolean bHasFpsHigh = FALSE;
  boolean bHasFpsLow = FALSE;
  AEEFixedPointMinMax p1;
  uint32 x;
  ZEROAT(&pInfo);
  if (ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_PREVIEW_FPS, (int32 *)&pInfo, NULL) != SUCCESS)
  {
    //Reset m_nMaxFPS if CAM_PARM_PREVIEW_FPS is not supported so menu is not built.
    MSG_MED("CAM_PARM_PREVIEW_FPS is not supported in this build", 0, 0, 0);
    pMe->m_nMaxFPS = 0;
    return;
  }
  MSG_MED("Returned FPS High: %d, Low: %d", pInfo.nMax, pInfo.nMin, 0);
  
  pFpsList = pMe->m_pListFPS;
  if (pFpsList)
  {
    //Loop is for checking the HighFPS and LowFPS in FPS list
    while(*pFpsList)  
    {
      x = ((*pFpsList) & 0xFFFF) * 1000;
      if((*pFpsList) >> 16)  // checking for decimal value
      {
        // Converting to Round-off value by dividing with decimal integer
        x /= ((*pFpsList) >> 16);
      }
      if (!bHasFpsHigh)
      {
        if (x == pMe->m_nHighFPS)
        {
          bHasFpsHigh = TRUE;
        }
      }
      if (!bHasFpsLow)
      {
        if (x == pMe->m_nLowFPS)
        {
          bHasFpsLow = TRUE;
        }
      }
      if(bHasFpsHigh & bHasFpsLow)
      {
        break;
      }
      pFpsList++;
    }

    if(!bHasFpsHigh) 
    {
      /* Reseting the HIGH_FPS to Max when the value is not present in FPS LIST*/
      pMe->m_nHighFPS = (uint32)pInfo.nMax;
    }

    if(!bHasFpsLow) 
    {
      /* Reseting the Low_FPS to Min when the value is not present in FPS LIST*/
      pMe->m_nLowFPS = (uint32)pInfo.nMin;	 
    }
  }
  else
  {
    //If low layer does not support FPS list, we don't know what FPS sensor supports.
    //We just make sure the old setting value from the config file not out of max FPS.
    if (pMe->m_nHighFPS > pMe->m_nMaxFPS)
      pMe->m_nHighFPS = pMe->m_nMaxFPS;
    if (pMe->m_nLowFPS > pMe->m_nHighFPS)
      pMe->m_nLowFPS = pMe->m_nHighFPS;
  }
  p1.nMax = (int32)pMe->m_nHighFPS;
  p1.nMin = (int32)pMe->m_nLowFPS;      
  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PREVIEW_FPS, (int32)&p1, 0);
}

/*=====================================================================
FUNCTION *Qcamera_GetSize

  DESCRIPTION: This function returns the rigth size based on the m_nSize.

  PARAMETERS:
    pMe  - pointer to QCam data struct

  RETURN VALUE:
    AEESize point
======================================================================*/
AEESize *Qcamera_GetSize(QCam *pMe)
{
  AEESize *pSize = NULL;

  if (!pMe)
    return NULL;

  if (pMe->m_nSize == (uint16)QCAM_SIZE_MAX)
  {
    pSize = &pMe->m_sensorInfo.size;
  }
  else
  {
    if (pMe->m_sensorInfo.bLandscape)
      pSize = QCam_GetLandscapeImageDimension(pMe->m_nSize);
    else
      pSize = QCam_GetImageDimension(pMe->m_nSize);
  }

  return pSize;
}

#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
/*=====================================================================
FUNCTION Qcamera_SetDirectMode

  DESCRIPTION: This function sets the LCD display mode. If it is to turn on
               the mode, the display info is passed from the second parm.

  PARAMETERS:
    pMe         - pointer to QCam data struct
    bDirectMode - flag to turn mode on/off

  RETURN VALUE:
    int Error code defined in AEEError.h
======================================================================*/
int Qcamera_SetDirectMode(QCam *pMe, boolean bDirectMode)
{
  int nRet = SUCCESS;
  if (!pMe)
    return EBADPARM;

  pMe->m_nDirectMode = bDirectMode;

  MSG_MED("Qcamera_SetDirectMode to %d", bDirectMode, 0, 0);
  if (!bDirectMode)
  {
    //No need to provide display info to turn off the mode
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_LCD_DIRECT_ACCESS_EX, 0, 0);
  }
  else
  {
    //Pass display info from p2 to turn on the mode.
    CameraDirectMode camera_direct;

    camera_direct.clsDisplay = 0; //for primary LCD display
    camera_direct.nLayer = -1;    //display preview in background
    camera_direct.xDst =  pMe->m_rcPreView.x; //display x
    camera_direct.yDst =  pMe->m_rcPreView.y; //display y
    if (!pMe->m_bDisplayLCDSize && (pMe->m_nSize > (uint16)QCAM_DEFAULT_DISPLAY_SIZE))
    {
      //Service layer always shift up for mdp to get the full screen
      //display for full image with QVGA LCD. We need to offset it for
      //images smaller than QVGA (or what ever the default screen size is)
      //to make it display on the center (match the UI display coordinate).
      camera_direct.yDst += QCAM_LCD_OFFSET;
    }
    camera_direct.cxDst = pMe->m_rcPreView.dx; //display dx
    camera_direct.cyDst = pMe->m_rcPreView.dy; //display dy
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_LCD_DIRECT_ACCESS_EX , 
                           1, (int32)&camera_direct);
  }
  return nRet;
}
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE

/*===========================================================================
FUNCTION Qcamera_CreateCamera

  DESCRIPTION
    This function initializes the camera device and get camera setting information

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    TRUE if a camera instance is created, FALSE otherwise.
===========================================================================*/
static boolean Qcamera_CreateCamera(QCam *pMe)
{
  int nErr = SUCCESS;
  uint16 quality = 0;
  int32  nValue = 0;
  AEEParmInfo pInfo;

  if (!pMe)
    return FALSE;

  //Create an instance for the pMe->m_pICam
  MSG_MED("Create Camera instance", 0, 0, 0);

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  if (pMe->m_pICam)
  {
    MSG_HIGH("Can not create Camera instance for Multisensor", 0, 0, 0);
    return FALSE;
  }

  switch (pMe->m_nSensorId)
  {
    case 0: // the primary sensor
      nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CAMERA1, 
                                   (void **)&pMe->m_pICam);
      if (nErr != SUCCESS)
      {
        MSG_HIGH("Can not create AEECLSID_CAMERA1 instance. err = %d", nErr, 0, 0);
        return FALSE;
      }
      break;

    case 1:
      nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CAMERA2, 
                                   (void **)&pMe->m_pICam);
      if (nErr != SUCCESS)
      {
        MSG_HIGH("Can not create AEECLSID_CAMERA2 instance. err = %d", nErr, 0, 0);
        return FALSE;
      }
      break;

    default: //invalid sensor id
      MSG_HIGH("Invalid sensor id for Multisensor: %d", pMe->m_nSensorId, 0, 0);
      return FALSE;
  }
#else
  nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CAMERA, (void **)&pMe->m_pICam);
  if (nErr != SUCCESS)
  {
    MSG_HIGH("Can not create AEECLSID_CAMERA instance. err = %d", nErr, 0, 0);
    return FALSE;
  }
#endif //FEATURE_CAMERA_MULTI_SENSOR

  quality = pMe->m_nQuality;
  if (pMe->m_encodingType == QCAM_ENCODING_RAW || pMe->m_encodingType == QCAM_ENCODING_YCBCR)
  {
    quality = JPEG_QUALITY_HIGH;
    pMe->m_qualityType = QCAM_QUALITY_HIGH;
  }

  MSG_MED("Set Camera default settings", 0, 0, 0);
  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PREVIEW_TYPE , CAM_PREVIEW_SNAPSHOT, 0);

  //Get sensor support items from sensor info
  ZEROAT(&pMe->m_sensorInfo);
  (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_SENSOR_INFO, (int32 *)&pMe->m_sensorInfo, 0);
#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif

#ifdef FEATURE_CAMERA_V2
  {
    AEESize dispSize;

    dispSize.cx = SMALL_SCREEN_WIDTH;
    dispSize.cy = (SMALL_SCREEN_WIDTH * 3) / 4;
    (void)ICAMERA_SetDisplaySize(pMe->m_pICam, &dispSize);
    if (pMe->m_nSize == (uint16)QCAM_SIZE_MAX)
    {
      (void)ICAMERA_SetSize(pMe->m_pICam, &pMe->m_sensorInfo.size);
    }
    else
    {
      (void)ICAMERA_SetSize(pMe->m_pICam, QCam_GetLandscapeImageDimension(pMe->m_nSize));
    }
  }
#else
  {
    AEESize *pSize = Qcamera_GetSize(pMe);
    //Initial this flag first
    pMe->m_bDisplayLCDSize = QCam_CheckProperties(pMe->m_nProperties, QCAM_LCD_DISP);

#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE

#ifdef AEE_SIMULATOR
    (void)ICAMERA_SetDisplaySize(pMe->m_pICam, pSize);
#else
    (void)Qcamera_SetDisplaySize(pMe, pSize);
#endif //AEE_SIMULATOR
    (void)ICAMERA_SetSize(pMe->m_pICam, pSize);
  }
#endif //FEATURE_CAMERA_V2
  pMe->m_rcPreView = Qcamera_GetPreviewRect(pMe);

  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ROTATE_ENCODE,
                        QCam_CheckProperties(pMe->m_nProperties, QCAM_PORTRAIT) ? 90 : 0, 0);
  (void)ICAMERA_SetQuality(pMe->m_pICam, quality);
  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_WHITE_BALANCE, (int32)pMe->m_nWhitebalance, 0);
  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_EFFECT, (int32)pMe->m_nEffect, 0);

  (void)ICAMERA_RegisterNotify(pMe->m_pICam, (PFNCAMERANOTIFY)Qcamera_CamEventNotify, pMe);

  //Initial dynamic support item flags

  pMe->m_bNightShotSupported = FALSE;
  (void)ICAMERA_IsSupport(pMe->m_pICam, CAM_PARM_NIGHTSHOT_MODE,
                          &pMe->m_bNightShotSupported);
  if (pMe->m_bNightShotSupported)
    (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_NIGHTSHOT_MODE,
                          (int32)pMe->m_bNightshotMode, 0);

  pMe->m_bRedEyeRemovalSupported = FALSE;
  (void)ICAMERA_IsSupport(pMe->m_pICam, CAM_PARM_RED_EYE_REDUCTION,
                          &pMe->m_bRedEyeRemovalSupported);

  pMe->m_bLASupported = FALSE;
  (void)ICAMERA_IsSupport(pMe->m_pICam, CAM_PARM_LUMA_ADAPTATION,
                          &pMe->m_bLASupported);
  if (pMe->m_bLASupported)
  {
    //Default to disable on start up
    pMe->m_nProperties &= ~QCAM_LUMA_ADAPTATION;
    (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_LUMA_ADAPTATION, 0, 0);
  }

  //Check if Bestshot supported
  pMe->m_bBSMSupported = FALSE;
  (void)ICAMERA_IsSupport(pMe->m_pICam, CAM_PARM_BESTSHOT,
                          &pMe->m_bBSMSupported);
  //Check if Multishot supported
#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#else
  pMe->m_bMultiShotSupported = FALSE;
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
  pMe->m_bBracketingSupported = FALSE;

  //Check if Panoramic mode enabled
#ifdef FEATURE_CAMERA_PANORAMIC
  pMe->m_nPanoramic = QCam_CheckProperties(pMe->m_nProperties, 
                                           QCAM_PANORAMIC_EFFECT) ? 1: 0;
  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PANORAMIC,  pMe->m_nPanoramic, 0);

#endif //FEATURE_CAMERA_PANORAMIC

  // Determine if the sensor supports Auto antibanding and enable option accordingly.
  ZEROAT(&pInfo);
  (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_ANTIBANDING, NULL, (int32 *)&pInfo);
  if (pInfo.nMax >= CAM_ANTIBANDING_AUTO)
  {
    pMe->m_bHasAutoAntibanding = TRUE;
#ifdef FEATURE_CAMERA_V2
    pMe->m_nAntibanding = CAM_ANTIBANDING_AUTO;
#endif
    MSG_MED("Enabling auto option for antibanding, nMax = %d", pInfo.nMax, 0, 0);
  }
  else
  {
    pMe->m_bHasAutoAntibanding = FALSE;
    MSG_MED("Auto option for antibanding NOT available, nMax = %d", pInfo.nMax, 0, 0);
  }
  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ANTIBANDING, (int32)pMe->m_nAntibanding, 0);

  // Default JPEG max file size parm to zero (disables this feature when zero)
  pMe->m_nMaxFileSize = 0;

  pMe->m_nMaxFPS = pMe->m_sensorInfo.dwFPSPreview * 1000;
  MSG_MED("Max FPS value returned = %d", pMe->m_nMaxFPS, 0, 0);

  pMe->m_nMaxSize = Qcamera_GetMaxSizeId(&pMe->m_sensorInfo);
  //reset the m_nMaxSize if encoding type is PNG.
  if (pMe->m_encodingType == QCAM_ENCODING_PNG)
    pMe->m_nMaxSize = (uint16)QCAM_SIZE_VGA;
  //reset the m_nSize if needed, index is increased when size is smaller.
  if (pMe->m_nSize < pMe->m_nMaxSize)
    pMe->m_nSize = pMe->m_nMaxSize;

  //update brighness, contrast, sharpness and zoom variables from sensor setting
#ifdef FEATURE_CAMERA_BRIGHTNESS
  Qcamera_InitVals(pMe, CAM_PARM_BRIGHTNESS, QCAM_BRIGHTNESS_FACTOR,
                   &pMe->m_nBright, &pMe->m_nBrightnessMin,
                   &pMe->m_nBrightnessMax, &pMe->m_nBrightnessStep);
  MSG_MED("Brightness value returned = %d", pMe->m_nBright, 0, 0);
#else
  //EV passed from BREW interface is a scaled value(numerator only).
  //The denominator is saved in OEM layer.
  //BREW added 3 digit precise to the value. Offset it by 1000.
  Qcamera_InitVals(pMe, CAM_PARM_EXPOSURE_COMP, 1,
                   &pMe->m_nExposure, &pMe->m_nExposureMin,
                   &pMe->m_nExposureMax, &pMe->m_nExposureStep);
  pMe->m_nExposure /= 1000;
  pMe->m_nExposureMin /= 1000;
  pMe->m_nExposureMax /= 1000;
  pMe->m_nExposureStep /= 1000;
#endif //FEATURE_CAMERA_BRIGHTNESS

  //Init Focus variables
  Qcamera_InitVals(pMe, CAM_PARM_FOCUS, 1,
                   &pMe->m_nFocus, &pMe->m_nFocusMin,
                   &pMe->m_nFocusMax, &pMe->m_nFocusStep);

  if (pMe->m_nFocusMax > 0)
  {
    (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_AF_INFO, (int32 *)&pMe->m_nAFMode, NULL);
#ifdef FEATURE_CAMERA7500
    (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_AF_INFO, NULL, (int32 *)&pMe->m_nAFModeMetering);
#endif //FEATURE_CAMERA7500

  }
  //Init Exposure mode
  (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_EXPOSURE_METERING, &nValue, NULL);
  pMe->m_autoExposure = (QCamAEmode)nValue;

  //Init Contrast variables
  Qcamera_InitVals(pMe, CAM_PARM_CONTRAST, QCAM_CONTRAST_STEPS, &pMe->m_nContrast,
                   &pMe->m_nContrastMin, &pMe->m_nContrastMax, &pMe->m_nContrastStep);
  MSG_MED("Contrast value returned = %d", pMe->m_nContrast, 0, 0);

  //Init Sharpness variables
  Qcamera_InitVals(pMe, CAM_PARM_SHARPNESS, QCAM_SHARPNESS_STEPS, &pMe->m_nSharpness,
                   &pMe->m_nSharpnessMin, &pMe->m_nSharpnessMax, &pMe->m_nSharpnessStep);
  MSG_MED("Sharpness value returned = %d", pMe->m_nSharpness, 0, 0);

#ifdef FEATURE_CAMERA7500
  //Init Hue variables
  Qcamera_InitVals(pMe, CAM_PARM_HUE, QCAM_HUE_STEPS, &pMe->m_nHue,
                   &pMe->m_nHueMin, &pMe->m_nHueMax, &pMe->m_nHueStep);
  MSG_MED("Hue value returned = %d", pMe->m_nHue, 0, 0);

  //Init Saturation variables
  Qcamera_InitVals(pMe, CAM_PARM_SATURATION, QCAM_SATURATION_STEPS, &pMe->m_nSaturation,
                   &pMe->m_nSaturationMin, &pMe->m_nSaturationMax, &pMe->m_nSaturationStep);
  MSG_MED("Saturation value returned = %d", pMe->m_nSaturation, 0, 0);
#endif //FEATURE_CAMERA7500

  //Init Zoom variables
  Qcamera_InitVals(pMe, CAM_PARM_ZOOM, 1, &pMe->m_nZoom,
                   &pMe->m_nZoomMin, &pMe->m_nZoomMax, &pMe->m_nZoomStep);
  MSG_MED("Zoom values returned: Max = %d, Min = %d, Current = %d",
          pMe->m_nZoomMax, pMe->m_nZoomMin, pMe->m_nZoom);

  CAM_FREEIF(pMe->m_pListFPS);
  pMe->m_pListFPS = Qcamera_GetListFPS(pMe);
  MSG_MED("FPS list returned = 0x%X", pMe->m_pListFPS, 0, 0);
  Qcamera_CheckListFps(pMe);
  

  //Init ISO variables
  ZEROAT(&pInfo);
  (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_ISO, &pMe->m_nISO, (int32 *)&pInfo);
  pMe->m_nMaxISO = pInfo.nMax;
  MSG_MED("Max ISO value returned = %d", pMe->m_nMaxISO, 0, 0);

#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  // Enable the Direct Mode by default if feature is on.
  // If this feature is on camera will direct the preview frame to bit buffer 
  // and IDisplay could not update the screen when mode is off (frame callback).
  (void)Qcamera_SetDirectMode(pMe, TRUE);
#endif //FEATURE_CAMERA_LCD_DIRECT_MODE

  pMe->m_nFlash = CAM_FLASH_OFF;
  pMe->m_nStrobeFlash = CAM_FLASH_OFF;
  return TRUE;
}

/*===========================================================================
FUNCTION Qcamera_ReleaseCamera

  DESCRIPTION
    This function stops the camera and release the camera instance

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_ReleaseCamera(QCam *pMe)
{
  if (!pMe)
    return;

  if (pMe->m_pICam)
  {
    //Safety function to clear frame overlay
    (void)ICAMERA_ClearOverlay(pMe->m_pICam);

    if (pMe->m_eState == QCAM_STATE_PREVIEW)
    {
#ifdef FEATURE_CAMERA_HISTOGRAM
      //Stop histogram before stop preview
      if (pMe->m_nPrvwHistogram != CAM_HISTOGRAM_OFF)
      {
        //Only tell low layer stop it. UI keep the option and resume on start preview
        (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_HISTOGRAM, CAM_HISTOGRAM_OFF, 0);
      }
#endif //#ifdef FEATURE_CAMERA_HISTOGRAM
      (void)ICAMERA_Stop(pMe->m_pICam);
    }

    (void)ICAMERA_RegisterNotify(pMe->m_pICam, NULL, NULL);

    MSG_MED("Release Camera instance", 0, 0, 0);
    (void)ICAMERA_Release(pMe->m_pICam);
    pMe->m_pICam = NULL;
  }
}

/*===========================================================================
FUNCTION Qcamera_FreeAppData

  DESCRIPTION
  This function frees any data initialized in the Init function.

  PROTOTYPE:
    void Qcamera_FreeAppData(IApplet* pi);

  PARAMETERS:
    QCam * pMe - Pointer to the applet instance

  RETURN VALUE:
   None
===========================================================================*/
static void Qcamera_FreeAppData (QCam *pMe)
{
  if (!pMe)
    return;
  // Cancel any pending timers
  (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);

#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
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
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY

  CAM_RELEASEIF(pMe->m_pISoftMenu);
  CAM_RELEASEIF(pMe->m_pIMenu);
  CAM_RELEASEIF(pMe->m_pFrame);
  CAM_RELEASEIF(pMe->m_pFrameDIB);
  CAM_RELEASEIF(pMe->m_pIFileMgr);
  CAM_RELEASEIF(pMe->m_pIStatic);

  //stop the shutter sound if it is on and
  //release the media interface.
  Qcamera_StopSound(pMe);
  QCam_DeleteFileList(&pMe->m_pSoundList);

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  //free memory
  if (pMe->m_bIconLoaded)
    Qcamera_ClearIcon(pMe);
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY

  //Release the GPS interface and cancel callback
  if (pMe->m_pIPosDet)
  {
    pMe->m_eGPSState = QCAM_GPS_DISABLED;
    CALLBACK_Cancel(&pMe->GPSInfoCB);
    (void)IPOSDET_Release(pMe->m_pIPosDet);
    pMe->m_pIPosDet = NULL;
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

#ifdef FEATURE_OVERLAY
#ifdef FEATURE_MMOVERLAY
  //Release MMOverlay objects
  CAM_RELEASEIF(pMe->m_pIMMOverlayFull);
  CAM_RELEASEIF(pMe->m_pIMMOverlayMenu);
  CAM_RELEASEIF(pMe->m_pIMMOverlayInfo);
  CAM_RELEASEIF(pMe->m_pIMMOverlayTitle);
  CAM_RELEASEIF(pMe->m_pIMMOverlayMgr);
#ifdef FEATURE_CAMERA7500
  //Clear the focus rect if any
  if (pMe->m_nRectcount > 0)
  {
    Qcamera_ClearAFRects(pMe);
  }
#endif //FEATURE_CAMERA7500
#else
  //Clear the MDP register pointer before exit.
  //It ensures the other application can update the display
  if (pMe->m_pIOverlay)
  {
    QCamDisp_MdpDeregister(pMe->m_pIOverlay);
  }

  CAM_RELEASEIF(pMe->m_pIOverlay);
#endif // #ifdef FEATURE_MMOVERLAY
#endif // FEATURE_OVERLAY
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
  CAM_RELEASEIF(pMe->m_pOvFrame);
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY

#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE

  //release camera
  Qcamera_ReleaseCamera(pMe);
  //free memory if encoding is aborted by end/clear key
  //wait after camera release ready so no memory leak occur.
  CAM_FREEIF(pMe->m_pData);
  CAM_FREEIF(pMe->m_pLaunchDS);
  CAM_FREEIF(pMe->m_pListFPS);

#ifdef FEATURE_CAMERA_HISTOGRAM
  CAM_RELEASEIF(pMe->m_pOvHist);
  CAM_RELEASEIF(pMe->m_pBmpHist);
#endif //FEATURE_CAMERA_HISTOGRAM

#ifdef FEATURE_CAMERA_PANORAMIC
  CAM_RELEASEIF(pMe->m_pOvPanoramic);
#endif //FEATURE_CAMERA_PANORAMIC

#ifdef HEAP_CHECK
  CAM_RELEASEIF (pMe->m_pIHeap);
  mm_reset_all_max_heap();
#endif
#ifdef FEATURE_QVPHONE
  if (pMe->m_pIVP)
  {
    (void)IVIDEOPHONE_Release( pMe->m_pIVP );
    pMe->m_pIVP = NULL;
  }
#endif /* FEATURE_QVPHONE */
}

/*===========================================================================
FUNCTION Qcamera_GetConfigRecord

  DESCRIPTION
    This function read initial data from the configuration file.
    If the file is not existing, set default initial values.
    The function is called on Qcamera application start

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_GetConfigRecord(QCam *pMe)
{
  IFile *pIFile = NULL;
  uint32 nVal;

  if (!pMe)
    return;

  if(IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_CONFIG_FILE)==SUCCESS)
  {
    pIFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, QCAM_CONFIG_FILE, _OFM_READ);
    if(pIFile)
    {
      //cfg file:
      /****************************************
      Thumbnail   Autoplay start date\time   position
      ON/OFF      OFF/SLOW/   MENU/ ON/OFF      ON/OFF
               STD/FAST CAMERA/
                        ALBUM
      ******************************************/
      (void)IFILE_Read(pIFile, &pMe->m_nSlideshowSpeed, sizeof(uint16));
      nVal = (uint32)pMe->m_nSlideshowSpeed;
      if (!QcamMenus_ValueInDataList(nVal, (uint16)QCAM_MENU_SLIDESHOW_SPEED))
      {
        pMe->m_nSlideshowSpeed = QCAM_SLIDESHOW_MED;
      }
      (void)IFILE_Read(pIFile, &pMe->m_nSelfTimerDelay, sizeof(uint16));
      nVal = (uint32)pMe->m_nSelfTimerDelay;
      if (!QcamMenus_ValueInDataList(nVal, (uint16)QCAM_MENU_SELFTIMER))
      {
        pMe->m_nSelfTimerDelay = 0;
      }

      if (IFILE_Read(pIFile, &pMe->m_wCurrentImage, sizeof(uint16)) == 0)
        pMe->m_wCurrentImage = 0;
      (void)IFILE_Read(pIFile, &pMe->m_wLastImageDay,   sizeof(uint16));
      (void)IFILE_Read(pIFile, &pMe->m_wLastImageMonth, sizeof(uint16));
      (void)IFILE_Read(pIFile, &pMe->m_wLastImageYear,  sizeof(uint16));
      (void)IFILE_Read(pIFile, &pMe->m_cDateTime,       sizeof(byte));
      (void)IFILE_Read(pIFile, &pMe->m_cPosition,       sizeof(byte));

      (void)IFILE_Read(pIFile, &pMe->m_nQuality, sizeof(uint16));
      nVal = (uint32)pMe->m_nQuality;
      if (!Qcamera_IsValueInBound(nVal, 1, 100))
      {
        pMe->m_nQuality = JPEG_QUALITY_MED;
        pMe->m_qualityType = QCAM_QUALITY_MED;
      }

      (void)IFILE_Read(pIFile, &pMe->m_nSize, sizeof(uint16));
      if (pMe->m_nSize > (uint16)QCAM_SIZE_COUNT - 1)
      {
        Qcamera_GetDefImageSize(pMe);
      }

      pMe->m_sFrameFileName[0] = '\0';
      (void)IFILE_Read(pIFile, pMe->m_sFrameFileName, FILE_NAME_SIZE);
      if ((pMe->m_sFrameFileName[0] != '\0') &&
          (IFILEMGR_Test(pMe->m_pIFileMgr, pMe->m_sFrameFileName) != SUCCESS))
      {
        pMe->m_sFrameFileName[0] = '\0';
      }
      // Read shutter sound file. If no sound files, then set it to "No Shutter"
      pMe->m_sShutterSoundFile[0] = '\0';
      (void)IFILE_Read(pIFile, pMe->m_sShutterSoundFile,  FILE_NAME_SIZE);
      if ((pMe->m_sShutterSoundFile[0] == '\0') ||
          (IFILEMGR_Test(pMe->m_pIFileMgr, pMe->m_sShutterSoundFile) != SUCCESS))
      {
        (void)STRLCPY(pMe->m_sShutterSoundFile, QCAM_SHUTTER_FILE, FILE_NAME_SIZE);
      }

      // Set the default to portrait mode to meet sensor setting.
      if (IFILE_Read(pIFile, &pMe->m_nProperties, sizeof(uint32)) == 0)
      {
        // Default to landscape on small screens.
        if (pMe->m_bSmallScreen == TRUE)
          pMe->m_nProperties = QCAM_LANDSCAPE;
        else
          pMe->m_nProperties = QCAM_PORTRAIT;
      }

      (void)IFILE_Read(pIFile, &pMe->m_nWhitebalance, sizeof(uint8));
      nVal = (uint32)pMe->m_nWhitebalance;
      if (!QcamMenus_ValueInDataList(nVal, (uint16)QCAM_MENU_WHITEBALANCE))
      {
        pMe->m_nWhitebalance = CAM_WB_AUTO;
      }

      (void)IFILE_Read(pIFile, &pMe->m_nMaxNumPics, sizeof(uint16));
      nVal = (uint32)pMe->m_nMaxNumPics;
      if (!QcamMenus_ValueInDataList(nVal, (uint16)QCAM_MENU_MAX_PICS))
      {
        pMe->m_nMaxNumPics = QCAM_MAX_PICS_NO_LIMIT;
      }

      (void)IFILE_Read(pIFile, &pMe->m_nEffect, sizeof(uint8));
      nVal = (uint32)pMe->m_nEffect;
      if (!QcamMenus_ValueInDataList(nVal, (uint16)QCAM_MENU_EFFECTS))
      {
        pMe->m_nEffect = CAM_EFFECT_OFF;
      }

      (void)IFILE_Read(pIFile, &pMe->m_encodingType, sizeof(pMe->m_encodingType));
      nVal = (uint32)pMe->m_encodingType;
      if (!QcamMenus_ValueInDataList(nVal, (uint16)QCAM_MENU_ENCODING))
      {
        pMe->m_encodingType = QCAM_ENCODING_JPEG;
      }

      (void)IFILE_Read(pIFile, &pMe->m_qualityType, sizeof(pMe->m_qualityType));
      nVal = (uint32)pMe->m_qualityType;
      if (!Qcamera_IsValueInBound(nVal, (uint32)QCAM_QUALITY_LOW, (uint32)QCAM_QUALITY_BOTH))
      {
        pMe->m_qualityType = QCAM_QUALITY_MED;
      }

      (void)IFILE_Read(pIFile, &pMe->m_saveLoc, sizeof(pMe->m_saveLoc));
      if ((pMe->m_saveLoc == QCAM_SAVE_MMC) &&
          (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_MMC_DIR) != SUCCESS))
      {
        pMe->m_saveLoc = QCAM_SAVE_FLASH;
      }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
      if ((pMe->m_saveLoc == QCAM_SAVE_USB) &&
          (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_USB_DIR) != SUCCESS))
      {
        pMe->m_saveLoc = QCAM_SAVE_FLASH;
      }
#endif
	   
      if (!pMe->m_bFTL && (pMe->m_saveLoc == QCAM_SAVE_FTL))
      {
        pMe->m_saveLoc = QCAM_SAVE_FLASH;
      }

      (void)IFILE_Read(pIFile, &pMe->m_eIconState, sizeof(pMe->m_eIconState));
      if (pMe->m_eIconState > QCAM_ICONS_PREVIEW_ENCODE)
      {
        pMe->m_eIconState = QCAM_ICONS_NO;
      }

      (void)IFILE_Read(pIFile, &pMe->m_nMulPhotos, sizeof(uint32));
      if ((pMe->m_nMulPhotos > QCAM_MAX_MULTISHOTS) || (pMe->m_nMulPhotos < 1))
      {
        pMe->m_nMulPhotos = 1;
      }
      if (pMe->m_nMulPhotos == 1)
        pMe->m_nSelMultishot = QCAM_MULTISHOT_OFF;
      else
        pMe->m_nSelMultishot = QCAM_MULTISHOT_ON;

      (void)IFILE_Read(pIFile, &pMe->m_nPhotoInterval, sizeof(uint32));
      (void)IFILE_Read(pIFile, &pMe->m_nLowFPS,        sizeof(uint32));
      (void)IFILE_Read(pIFile, &pMe->m_nHighFPS,       sizeof(uint32));
      if (pMe->m_nLowFPS == 0 || pMe->m_nHighFPS == 0)
      {
        pMe->m_nLowFPS  = QCAM_FPS_15000;
        pMe->m_nHighFPS = QCAM_FPS_15000;
      }
#ifdef FEATURE_CAMERA_HISTOGRAM
      (void)IFILE_Read(pIFile, &pMe->m_nPrvwHistogram, sizeof(uint8));
#endif //FEATURE_CAMERA_HISTOGRAM
      (void)IFILE_Release(pIFile);
    }
  }
  else  //no per-records
  {
    pMe->m_nSlideshowSpeed = QCAM_SLIDESHOW_MED;
    pMe->m_nSelfTimerDelay = 0;

    pMe->m_cDateTime       = 0;
    pMe->m_cPosition       = 0;
    pMe->m_wCurrentImage   = 0;
    pMe->m_wLastImageDay   = 0;
    pMe->m_wLastImageMonth = 0;
    pMe->m_wLastImageYear  = 0;

    pMe->m_sFrameFileName[0] = '\0';
    (void)STRLCPY(pMe->m_sShutterSoundFile, QCAM_SHUTTER_FILE, FILE_NAME_SIZE);
    Qcamera_GetDefImageSize(pMe);
    if (pMe->m_bSmallScreen == TRUE)
      pMe->m_nProperties = QCAM_LANDSCAPE;
    else
      pMe->m_nProperties = QCAM_PORTRAIT;
    pMe->m_nWhitebalance = CAM_WB_AUTO;
    pMe->m_nMaxNumPics = QCAM_MAX_PICS_NO_LIMIT;
    pMe->m_saveLoc = QCAM_SAVE_FLASH;
    pMe->m_nQuality = JPEG_QUALITY_MED;
    pMe->m_qualityType = QCAM_QUALITY_MED;
    pMe->m_encodingType = QCAM_ENCODING_JPEG;
    pMe->m_eIconState = QCAM_ICONS_NO;
    pMe->m_nEffect = CAM_EFFECT_OFF;
    pMe->m_nMulPhotos = 1; //Default no multishot
    pMe->m_nPhotoInterval = 0; //Default no delay for burst mode
  }
}

/*===========================================================================
FUNCTION Qcamera_SetConfigRecord

  DESCRIPTION
    This function writes the current setting data to the configuration file.
    The function is called on Qcamera application stop

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_SetConfigRecord(QCam *pMe)
{
  IFile*    pIFile = NULL;

  if (!pMe)
    return;

  if (pMe->m_pIFileMgr)
  {
    (void)IFILEMGR_Remove(pMe->m_pIFileMgr, QCAM_CONFIG_FILE);
    if (IFILEMGR_Test(pMe->m_pIFileMgr, QCAM_CONFIG_FILE) != SUCCESS)
    {
      pIFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, QCAM_CONFIG_FILE, _OFM_CREATE);
    }
    else
    {
      pIFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, QCAM_CONFIG_FILE, _OFM_READWRITE);
    }
    if (pIFile)
    {
      (void)IFILE_Write(pIFile, &pMe->m_nSlideshowSpeed,   sizeof(uint16));
      (void)IFILE_Write(pIFile, &pMe->m_nSelfTimerDelay,   sizeof(uint16));
      (void)IFILE_Write(pIFile, &pMe->m_wCurrentImage,     sizeof(uint16));
      (void)IFILE_Write(pIFile, &pMe->m_wLastImageDay,     sizeof(uint16));
      (void)IFILE_Write(pIFile, &pMe->m_wLastImageMonth,   sizeof(uint16));
      (void)IFILE_Write(pIFile, &pMe->m_wLastImageYear,    sizeof(uint16));
      (void)IFILE_Write(pIFile, &pMe->m_cDateTime,         sizeof(byte));
      (void)IFILE_Write(pIFile, &pMe->m_cPosition,         sizeof(byte));
      (void)IFILE_Write(pIFile, &pMe->m_nQuality,          sizeof(uint16));
      (void)IFILE_Write(pIFile, &pMe->m_nSize,             sizeof(uint16));
      (void)IFILE_Write(pIFile,  pMe->m_sFrameFileName,    FILE_NAME_SIZE);
      (void)IFILE_Write(pIFile,  pMe->m_sShutterSoundFile, FILE_NAME_SIZE);
      (void)IFILE_Write(pIFile, &pMe->m_nProperties,       sizeof(uint32));
      (void)IFILE_Write(pIFile, &pMe->m_nWhitebalance,     sizeof(uint8));
      (void)IFILE_Write(pIFile, &pMe->m_nMaxNumPics,       sizeof(uint16));
      (void)IFILE_Write(pIFile, &pMe->m_nEffect,           sizeof(uint8));
      (void)IFILE_Write(pIFile, &pMe->m_encodingType,      sizeof(pMe->m_encodingType));
      (void)IFILE_Write(pIFile, &pMe->m_qualityType,       sizeof(pMe->m_qualityType));
      (void)IFILE_Write(pIFile, &pMe->m_saveLoc,           sizeof(pMe->m_saveLoc));
      (void)IFILE_Write(pIFile, &pMe->m_eIconState,        sizeof(pMe->m_eIconState));
      (void)IFILE_Write(pIFile, &pMe->m_nMulPhotos,        sizeof(uint32));
      (void)IFILE_Write(pIFile, &pMe->m_nPhotoInterval,    sizeof(uint32));
      (void)IFILE_Write(pIFile, &pMe->m_nLowFPS,           sizeof(uint32));
      (void)IFILE_Write(pIFile, &pMe->m_nHighFPS,          sizeof(uint32));
#ifdef FEATURE_CAMERA_HISTOGRAM
      (void)IFILE_Write(pIFile, &pMe->m_nPrvwHistogram,    sizeof(uint8));
#endif //FEATURE_CAMERA_HISTOGRAM
      (void)IFILE_Release(pIFile);
    }
  }
}

#ifdef FEATURE_OVERLAY
/*=====================================================================
FUNCTION Qcamera_AddGrid

  DESCRIPTION: Add 9 grids to the preview screen

  PARAMETERS:
    pMe - Pointer to the applet instance

  RETURN VALUE:
    None
======================================================================*/
static void Qcamera_AddGrid (QCam *pMe)
{
  if (!pMe)
    return;

#ifdef FEATURE_MMOVERLAY //7K
  //Pass TRUE to register full screen display
  Qcamera_RegisterOvScreen(pMe, TRUE, TRUE);
#else //6K
  QCamDisp_MdpRegisterFullScreen(pMe->a.m_pIDisplay, pMe->m_pIOverlay,
                                 pMe->di.cxScreen, pMe->di.cyScreen);
#endif //#ifdef FEATURE_MMOVERLAY

  IDISPLAY_DrawHLine(pMe->a.m_pIDisplay,
                     pMe->m_rcPreView.x,
                     (int16)(pMe->m_rcPreView.y + (pMe->m_rcPreView.dy / 3)),
                     pMe->m_rcPreView.dx);
  IDISPLAY_DrawHLine(pMe->a.m_pIDisplay,
                     pMe->m_rcPreView.x,
                     (int16)(pMe->m_rcPreView.y + ((pMe->m_rcPreView.dy * 2) / 3)),
                     pMe->m_rcPreView.dx);
  IDISPLAY_DrawVLine(pMe->a.m_pIDisplay,
                     (int16)(pMe->m_rcPreView.x + (pMe->m_rcPreView.dx / 3)),
                     pMe->m_rcPreView.y,
                     pMe->m_rcPreView.dy);
  IDISPLAY_DrawVLine(pMe->a.m_pIDisplay,
                     (int16)(pMe->m_rcPreView.x + ((pMe->m_rcPreView.dx * 2) / 3)),
                     pMe->m_rcPreView.y,
                     pMe->m_rcPreView.dy);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
  pMe->m_bGridOn = TRUE;
}

/*===========================================================================
FUNCTION: Qcamera_MdpRegisterPreview

  DESCRIPTION:
    This function register the title bar and menu area for mdp overlay
    on preview mode.

  PARAMETERS:
    QCam * pMe - Pointer to the applet instance

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_MdpRegisterPreview (QCam *pMe)
{
  if (!pMe)
    return;

#ifdef FEATURE_MMOVERLAY //7k
  //Pass FALSE to register for preview
  Qcamera_RegisterOvScreen(pMe, FALSE, TRUE);
#else //6k
  {
    AEERect rc;

    //Clear the former register first.
    IOVERLAY_RemoveAll(pMe->m_pIOverlay, OVERLAY_LAYER_2);

    //Register for the icon menu area
    SETAEERECT (&rc, 0, pMe->di.cyScreen - pMe->m_softMenuHeight,
                pMe->di.cxScreen, pMe->m_softMenuHeight);
    (void)QCamDisp_MdpRegister(pMe->a.m_pIDisplay, pMe->m_pIOverlay, rc);

    //Register for the menu title
    SETAEERECT (&rc, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight);
    (void)QCamDisp_MdpRegister(pMe->a.m_pIDisplay, pMe->m_pIOverlay, rc);

    //Register for the info title
    SETAEERECT (&rc, 0, pMe->m_nTitleHeight, pMe->di.cxScreen, pMe->m_nTitleHeight);
    (void)QCamDisp_MdpRegister(pMe->a.m_pIDisplay, pMe->m_pIOverlay, rc);
  }
#endif //#ifdef FEATURE_MMOVERLAY
}

/*===========================================================================
FUNCTION: Qcamera_ToggleMenuDisplay

  DESCRIPTION:
    This function toggles the take picture menu and title on/off.
    And also toggle the grid off/on in opposite to menu bar.

  PARAMETERS:
    QCam * pMe - Pointer to the applet instance

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_ToggleMenuDisplay(QCam *pMe)
{
  if (!pMe)
    return;

  if (pMe->m_bShowMenu)
  {
    //Hide the menu if it is on
    AEERect rc;

    pMe->m_bShowMenu = FALSE;
    SETAEERECT (&rc, 0, pMe->di.cyScreen - pMe->m_softMenuHeight,
                pMe->di.cxScreen, pMe->m_softMenuHeight);
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
    SETAEERECT (&rc, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight * 2);
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
    //Then turn on the 9 grids if it is in preview
    if (pMe->m_eState == QCAM_STATE_PREVIEW)
      Qcamera_AddGrid(pMe);
  }
  else
  {
    //Clear the 9 grid if it is on
    //Build the menu if it is off
    pMe->m_bShowMenu = TRUE;
    pMe->m_bGridOn = FALSE;
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rcPreView);
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
    Qcamera_MdpRegisterPreview(pMe);
    QcamMenus_BuildTakePhotoMenu(pMe);
  }
}

#endif // FEATURE_OVERLAY

#ifdef FEATURE_MMOVERLAY
/*=====================================================================
FUNCTION Qcamera_RegisterOvScreen

  DESCRIPTION: This function registers the IMMOverlay display area for UI layer.

  PARAMETERS:
    pMe - Pointer to  QCam struct
    bFullScreen - flag to tell if it is registered for full screen or priview

  RETURN VALUE:
    void
======================================================================*/
static void Qcamera_RegisterOvScreen(QCam *pMe, boolean bFullScreen, boolean bTransparent)
{
  IBitmap *pBmp;
  OvData ov;
  AEEBitmapInfo bi;
  AEERect rect;

  if (!pMe)
    return;

#ifdef FEATURE_CAMERA_PANORAMIC
  //Hide title and menu if taking picture in panoramic mode
  if (pMe->m_nPanoramic > 1)
    return;
#endif //FEATURE_CAMERA_PANORAMIC

  //Clear old objects
  CAM_RELEASEIF(pMe->m_pIMMOverlayMenu);
  CAM_RELEASEIF(pMe->m_pIMMOverlayInfo);
  CAM_RELEASEIF(pMe->m_pIMMOverlayTitle);
  CAM_RELEASEIF(pMe->m_pIMMOverlayFull);

  //Get device bitmap
  if (IDISPLAY_GetDeviceBitmap (pMe->a.m_pIDisplay, &pBmp) == SUCCESS)
  {
    ov.pBmp = pBmp;
    ov.layer = OV_LAYER_TOP_LAYER;
    if (bTransparent)
      ov.transparent = RGB_WHITE;
    else
      ov.transparent = RGB_NONE; //No transparent

    if (bFullScreen)
    {
      //register full screen
      ov.alpha = 256; //No alpha blend
#ifdef FEATURE_CAMERA8X50
#error code not present
#else
      SETAEERECT(&ov.rc, 0, 0, 0, 0);
#endif
      (void)QCamDisp_RegisterOv(pMe->m_pIMMOverlayMgr, &pMe->m_pIMMOverlayFull, ov);
    }
    else
    {
#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
      {
        //for preview icon menu
        ov.alpha = 128; //half alpha blend
        SETAEERECT (&ov.rc, 0, pMe->di.cyScreen - pMe->m_softMenuHeight,
                    pMe->di.cxScreen, pMe->m_softMenuHeight);
      }
      (void)QCamDisp_RegisterOv(pMe->m_pIMMOverlayMgr, &pMe->m_pIMMOverlayMenu, ov);

      //Register for the menu title
      ov.transparent = RGB_WHITE;
      SETAEERECT (&ov.rc, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight);
      (void)QCamDisp_RegisterOv(pMe->m_pIMMOverlayMgr, &pMe->m_pIMMOverlayTitle, ov);

      if (bTransparent)
      {
        //Register for the preview info title
        SETAEERECT (&ov.rc, 0, pMe->m_nTitleHeight, pMe->di.cxScreen, pMe->m_nTitleHeight);
        (void)QCamDisp_RegisterOv(pMe->m_pIMMOverlayMgr, &pMe->m_pIMMOverlayInfo, ov);
      }
    }

    //update back ground bitmap
    if(SUCCESS == IBITMAP_GetInfo(pBmp, &bi, sizeof(bi)))
    {
      SETAEERECT(&rect, 0, 0, bi.cx, bi.cy);
      (void) IBITMAP_Invalidate(pBmp, &rect);
      IDISPLAY_Update(pMe->a.m_pIDisplay);
    }
    CAM_RELEASEIF(pBmp);
  }
  else
  {
    MSG_HIGH("IDISPLAY_GetDeviceBitmap Failed", 0, 0, 0);
  }
}
#endif //FEATURE_MMOVERLAY

#if defined FEATURE_CAMERA_HISTOGRAM || defined FEATURE_CAMERA_PANORAMIC
/*=====================================================================
FUNCTION QCamera_GetOvObj

  DESCRIPTION: This function creates an IMMOverlay object from the provided
               IBitmap.

  PARAMETERS:
    pMe - Pointer to  QCam struct
    pBitmap - Pointer to IBitmap interface that has the bitmap buffer pointer.
    ppMMOverlay - Pointer to the IMMOverlay object.

  RETURN VALUE:
    boolean TRUE - if success. FALSE otherwise
======================================================================*/
static boolean QCamera_GetOvObj(QCam *pMe,
                                IBitmap *pBitmap,
                                AEERect rc,
                                IMMOverlay **ppIMMOverlay)
{
  if (!pMe || !pBitmap || !ppIMMOverlay)
    return FALSE;

  if (!*ppIMMOverlay)
  {
    AEEPoint pt;

    pt.x = rc.x;
    pt.y = rc.y;
    //Create overlay object in upper left corner of screen
    if(SUCCESS != IMMOVERLAYMGR_CreateOverlay(pMe->m_pIMMOverlayMgr,
                                              pBitmap,
                                              AEECLSID_DISPLAY1,
                                              pt,
                                              ppIMMOverlay))
    {
      MSG_HIGH( "Failed QCamera_GetOvObj, ppIMMOverlay is NULL", 0, 0, 0);
      return FALSE;
    }
    else
    {
      (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_LAYER, OV_LAYER_TOP_LAYER, 0);
      (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_DEST_REGION, (uint32)&rc, 0);
      //(void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_SRC_REGION, (uint32)&rc, 0);
      (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_TRANSPARENCY, 1, RGB_WHITE);
      (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_ALPHA_BLEND_PCT, 160, 0);
      //(void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_SYSTEM_OVERLAY, 1, 0);
    }
  }
  else
  {
    //Update the bitmap parameter
    MSG_MED("IMMOVERLAY_SetParam to set ppIMMOverlay.", 0, 0, 0);
    (void)IMMOVERLAY_SetParam(*ppIMMOverlay, OV_PARM_BITMAP, (uint32)pBitmap, 0);
  }
  return TRUE;
}
#endif //FEATURE_CAMERA_HISTOGRAM || FEATURE_CAMERA_PANORAMIC

/*===========================================================================
FUNCTION Qcamera_GetRemainPicNum

  DESCRIPTION
    This function gets the remain number of shots.

  PARAMETERS:
    pMe - Pointer to  QCam struct
    bMMC - flag to get picture number from MMC or not
    bFTL - flag to get picture number from FTL or not

    bMMC and bFTL and flash are mutually exclusive.

  RETURN VALUE:
    remain number of shots
===========================================================================*/
static int Qcamera_GetRemainPicNum(QCam *pMe, boolean bMMC, boolean bFTL 
  #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
    , boolean bUSB
  #endif
)
{
  int picNum = 0;
  uint32 dwFreeSpace = 0;
  int picSize = 0;
  int index, picDirNum;
  SearchFlag sFlag;
  const char *savePath;

  if (!pMe)
    return 0;

  //Get the index for geting picture size
  index = Qcamera_GetQualityIndex (pMe);

  if (bMMC)
  {
    sFlag = LOC_MMC;
    savePath = AEEFS_CARD0_DIR;
  }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  else if (bUSB)
  {
    sFlag = LOC_USB;
    savePath = AEEFS_MOD_USB_DIR;
  }
#endif
  else if (bFTL)
  {
    sFlag = LOC_FTL;
    savePath = AEEFS_FTL_DIR;
  }
  else
  {
    sFlag = LOC_FLASH;
    savePath = AEEFS_ROOT_DIR;
  }

  //Get the photo number in the save location (include raw images).
  picDirNum = (int)QCam_GetTotalPhotos(pMe->m_pIFileMgr, TRUE, sFlag);

  //Get the number based on current selected picture size
  //and available space
  if(SUCCESS != IFILEMGR_GetFreeSpaceEx(pMe->m_pIFileMgr,
                                        savePath,
                                        NULL,
                                        &dwFreeSpace))
  {
    dwFreeSpace = IFILEMGR_GetFreeSpace(pMe->m_pIFileMgr, NULL);
  }

  //Convert the value in Bytes to KB
  dwFreeSpace /= 1024;
  if (pMe->m_encodingType == QCAM_ENCODING_RAW)
    picSize = Qcamera_GetRawSize(pMe);
  else if (pMe->m_nSize == (uint16)QCAM_SIZE_MAX)
    picSize = QCam_PictureSize[pMe->m_nMaxSize][index];
  else
    picSize = QCam_PictureSize[pMe->m_nSize][index];

  if (picSize == 0)
  {
    MSG_MED("Zero picture size, map to 1", 0, 0, 0);
    picSize = 1;
  }
  picNum = (int) (dwFreeSpace / (uint32)picSize);

  //Reserve one file size for more memory since the file size is just average size
  //except RAW image.
  if (pMe->m_encodingType != QCAM_ENCODING_RAW)
  {
    picNum = (picNum > 0) ? picNum - 1 : 0;
  }

  //always check memory available first. then check max. number selection.
  if ((pMe->m_nMaxNumPics > 0) && (picNum > (pMe->m_nMaxNumPics - picDirNum)))
  {
    picNum = (pMe->m_nMaxNumPics >= picDirNum) ? (pMe->m_nMaxNumPics - picDirNum) : 0;
  }

  return picNum;
}

/*===========================================================================
FUNCTION Qcamera_GetDefImageSize

  DESCRIPTION
    This function gets the default image size (in index) if the selected value
    is corrupted.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    default image size in preview
===========================================================================*/
static void Qcamera_GetDefImageSize(QCam *pMe)
{
  AEESize *pSize;

  if (pMe->m_sensorInfo.bLandscape)
    pSize = QCam_GetLandscapeImageDimension((uint16)QCAM_SIZE_QCIF);
  else
    pSize = QCam_GetImageDimension((uint16)QCAM_SIZE_QCIF);

  if (!pMe)
    return;

  if (pMe->di.cxScreen > pSize->cx)
    pMe->m_nSize = (uint16)QCAM_SIZE_QVGA;
  else
    pMe->m_nSize = (uint16)QCAM_SIZE_QQVGA;
}

/*===========================================================================
FUNCTION Qcamera_IsValueInBound

  DESCRIPTION
    This function checks if the value is in the given boundary

  PARAMETERS:
    val - value to be checked
    min - minumum value in the boundary
    max - maximum value in the boundary

  RETURN VALUE:
    TRUE if it is in boundary
===========================================================================*/
static boolean Qcamera_IsValueInBound(uint32 val, uint32 min, uint32 max)
{
  if (val < min || val > max)
    return FALSE;
  else
    return TRUE;
}

/*===========================================================================
FUNCTION Qcamera_IsSizeMatch

  DESCRIPTION
    This function checks if the provided cx and cy are matching
    the cx and cy in the selected image size

  PARAMETERS:
    pMe - Pointer to  QCam struct
    cx - width of the size to check
    cy - heigth of the size to check

  RETURN VALUE:
    TRUE if it is matched
===========================================================================*/
static boolean Qcamera_IsSizeMatch(QCam *pMe, uint16 cx, uint16 cy)
{
  AEESize *pSize = NULL;
  boolean bMatch;

  if (!pMe)
    return FALSE;

  pSize = Qcamera_GetSize(pMe);

  //QCam_GetImageDimension always returns size in portrait mode.
  //If preview image is in landscape mode, exchange cx and cy.

  bMatch = (((cx == pSize->cx) && (cy == pSize->cy)) ||
            ((cy == pSize->cx) && (cx == pSize->cy)));
  return bMatch;
}


/*=====================================================================
FUNCTION Qcamera_GetMaxSize

  DESCRIPTION:
    This function returns the max size id based on sensor info.

  PARAMETERS:
    pSensorInfo - sensor info structure

  RETURN VALUE:
    uint16
======================================================================*/
uint16 Qcamera_GetMaxSizeId(const CameraSensorInfo *pSensorInfo)
{
  uint16 nSizeId = (uint16)QCAM_SIZE_UXGA;
  int nSize;

  if (!pSensorInfo)
    return (uint16)QCAM_SIZE_UXGA;

  nSize = (pSensorInfo->size.cx * pSensorInfo->size.cy) /1000;
  if (nSize > 5000)
    nSizeId = (uint16)QCAM_SIZE_QSXGA;
  else if (nSize > 4000)
    nSizeId = (uint16)QCAM_SIZE_QXGAP;
  else if (nSize > 3000)
    nSizeId = (uint16)QCAM_SIZE_QXGA;
  else if (nSize > 1500)
    nSizeId = (uint16)QCAM_SIZE_UXGA;
  else if (nSize > 1300)
    nSizeId = (uint16)QCAM_SIZE_SXGA;
  else if (nSize > 1000)
    nSizeId = (uint16)QCAM_SIZE_4VGA;
  else if (nSize > 300)
    nSizeId = (uint16)QCAM_SIZE_VGA;
  else
    //assume the max size if it is not identified.
    nSizeId = (uint16)QCAM_SIZE_UXGA;

  return nSizeId;
}

/*===========================================================================
FUNCTION Qcamera_GetRawSize

  DESCRIPTION
    This function gets the raw image file size

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    index
===========================================================================*/
static int Qcamera_GetRawSize(QCam *pMe)
{
  int nSize;

  if(!pMe)
    return 0;

  // If sensor info returned the max size, use it to calculate.
  nSize = (pMe->m_sensorInfo.size.cx * pMe->m_sensorInfo.size.cy) / 1000;
  // RAW - (10 bit RAW) = Max. H x W * 4/3,
  nSize = (nSize * 4) / 3;

  return nSize;
}

/*===========================================================================
FUNCTION Qcamera_GetQualityIndex

  DESCRIPTION
    This function gets the index that will be used to find the image file size

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    index
===========================================================================*/
static int Qcamera_GetQualityIndex(QCam *pMe)
{
  int index = 1;

  if (!pMe)
    return 0;

  if (pMe->m_encodingType == QCAM_ENCODING_PNG)
    index = 3;   //QCAM_ENCODING_PNG
  else if (pMe->m_nQuality <= JPEG_QUALITY_LOW)
    index = 0;  //QCAM_QUALITY_LOW
  else if (pMe->m_nQuality >= JPEG_QUALITY_HIGH)
    index = 2;  //QCAM_QUALITY_HIGH
  else
    index = 1;  //QCAM_QUALITY_MED

  return index;
}

/*===========================================================================
FUNCTION Qcamera_IsSpaceAvailable

  DESCRIPTION
    This function checks if enough space to save a new photo.
    Reserve double size of an image file to make sure enough
    implementation space.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    TRUE if space available, FALSE otherwise
===========================================================================*/
static boolean Qcamera_IsSpaceAvailable(QCam *pMe)
{
  boolean bMMC = FALSE;
  boolean bFTL = FALSE;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  boolean bUSB = FALSE;
#endif
  int picNum = 0;

  if (!pMe)
    return FALSE;

  bMMC = ((pMe->m_saveLoc == QCAM_SAVE_MMC) &&
          (SUCCESS == IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_CARD0_DIR)));

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS	 
  bUSB = ((pMe->m_saveLoc == QCAM_SAVE_USB) &&
			  (SUCCESS == IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_MOD_USB_DIR)));
#endif
  bFTL = (pMe->m_bFTL && (pMe->m_saveLoc == QCAM_SAVE_FTL));

  picNum = Qcamera_GetRemainPicNum(pMe, bMMC, bFTL 
     #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS 
       , bUSB 
     #endif
    );
  if ((picNum == 0) || (picNum < (int)pMe->m_nMulPhotos))
  {
    // Check if they are over the limit of the max # of pics
    if (pMe->m_nMaxNumPics > 0)
    {
      MSG_MED("OVER TAKE PICTURE LIMIT", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    else
    {
      MSG_MED("NOT ENOUGH DISK SPACE", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION Qcamera_DisplayTimedImg

  DESCRIPTION
    This function displays the image and set the timer to end it.

  PARAMETERS:
    pMe - Pointer to QCam struct
    ph - call back funciton after time out
    msgID - message ID for the message to display

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_DisplayTimedImg(QCam *pMe, PFNNOTIFY ph, uint16 imgID)
{
  AEERect rc;
  IImage *pImage = NULL;

  if (!pMe)
    return;

  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen - pMe->m_softMenuHeight);
  pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, imgID);

  QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, rc);
  if (ph)
  {
    pMe->m_bMsgTimerOn = TRUE;
    //Return to somewhere after time out if ph is not NULL.
    //Otherwise stay till user exit.
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, TIMEOUT_1200, ph, (void *)pMe);
  }
}

/*===========================================================================
FUNCTION Qcamera_DisplayTimedMsg

  DESCRIPTION
    This function displays the message and set the timer to end it.

  PARAMETERS:
    pMe - Pointer to QCam struct
    ph - call back funciton after time out
    msgID - message ID for the message to display

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_DisplayTimedMsg(QCam* pMe, PFNNOTIFY ph, uint16 msgID)
{
  AEERect rc;
  AECHAR  szBuf[TEXT_STRING_SIZE] = {0};

  if (!pMe)
    return;

  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen - pMe->m_softMenuHeight);
  if (ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, msgID, szBuf, sizeof(szBuf)))
  {
#ifdef FEATURE_MMOVERLAY
    if (pMe->m_pIMMOverlayFull)
      (void)IMMOVERLAY_SetParam(pMe->m_pIMMOverlayFull, OV_PARM_TRANSPARENCY, 0, 0);
#endif
    QCamDisp_ShowMsg(pMe->a.m_pIDisplay, szBuf, rc);
  }

  if (ph)
  {
    pMe->m_bMsgTimerOn = TRUE;
    //Return to somewhere after time out if ph is not NULL.
    //Otherwise stay till user exit.
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, TIMEOUT_1200, ph, (void *)pMe);
  }
}


/*=====================================================================
FUNCTION Qcamera_DisplayTakePicErr

  DESCRIPTION: This function display error message and return status for
               automation test on take picture failure.

  PARAMETERS:
    pMe - Pointer to QCam struct
    msgID - message ID for the message to display

  RETURN VALUE:
    void
======================================================================*/
static void Qcamera_DisplayTakePicErr(QCam *pMe, uint16 msgID)
{
  if (!pMe)
    return;

  Qcamera_DisplayTimedMsg(pMe,
                          (PFNNOTIFY)Qcamera_TurnOnCameraPreview,
                          msgID);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
}

/*===========================================================================
FUNCTION Qcamera_GetSizeStrID

  DESCRIPTION
    This function returns a ID of the size string for the selected size.

  PARAMETERS:
    pMe    - Pointer to  QCam struct

  RETURN VALUE:
    uint16 str ID or 0
===========================================================================*/
static uint16 Qcamera_GetSizeStrID(QCam *pMe)
{
  uint16 sizeId = 0;

  if (!pMe)
    return 0;

  switch(pMe->m_nSize)
  {
  case QCAM_SIZE_QSXGA:
    sizeId = IDS_SIZE_QSXGA;
    break;
  case QCAM_SIZE_QXGAP:
    sizeId = IDS_SIZE_QXGAP;
    break;
  case QCAM_SIZE_QXGA:
    sizeId = IDS_SIZE_QXGA;
    break;
  case QCAM_SIZE_UXGA:
    sizeId = IDS_SIZE_UXGA;
    break;
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  case QCAM_SIZE_SXGA:
    sizeId = IDS_SIZE_SXGA;
    break;
  case QCAM_SIZE_4VGA:
    sizeId = IDS_SIZE_4VGA;
    break;
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  case QCAM_SIZE_XGA:
    sizeId = IDS_SIZE_XGA;
    break;
  case QCAM_SIZE_SVGA:
    sizeId = IDS_SIZE_SVGA;
    break;
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  case QCAM_SIZE_VGA:
    sizeId = IDS_SIZE_VGA;
    break;
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
  case QCAM_SIZE_CIF:
    sizeId = IDS_SIZE_CIF;
    break;
  case QCAM_SIZE_QVGA:
    sizeId = IDS_SIZE_QVGA;
    break;
  case QCAM_SIZE_QCIF:
    sizeId = IDS_SIZE_QCIF;
    break;
  case QCAM_SIZE_QQVGA:
    sizeId = IDS_SIZE_QQVGA;
    break;
  case QCAM_SIZE_STAMP:
    sizeId = IDS_SIZE_STAMP;
    break;
  default:
    break;
  }

  return sizeId;
}

/*===========================================================================
FUNCTION Qcamera_GetMatchSizeFrameDir

  DESCRIPTION
    This function returns a pointer to the size dir string for the frame directory.
    If the image size is larger than QVGA, we don't support it now.

  PARAMETERS:
    pMe    - Pointer to  QCam struct

  RETURN VALUE:
    Const string point or NULL.
===========================================================================*/
static const char * Qcamera_GetMatchSizeFrameDir(QCam *pMe)
{
  const char *pDir;

  if (!pMe)
    return NULL;

  switch(pMe->m_nSize)
  {
  case QCAM_SIZE_QQVGA:
    pDir = QCAM_QQVGA_SDIR;
    break;
  case QCAM_SIZE_QCIF:
    pDir = QCAM_QCIF_SDIR;
    break;
  case QCAM_SIZE_QVGA:
    pDir = QCAM_QVGA_SDIR;
    break;
  case QCAM_SIZE_VGA:
    pDir = QCAM_VGA_SDIR;
    break;
  case QCAM_SIZE_SXGA:
    pDir = QCAM_SXGA_SDIR;
    break;
  case QCAM_SIZE_UXGA:
    pDir = QCAM_UXGA_SDIR;
    break;
  default:
    pDir = NULL;
  }

  return pDir;
}

/*===========================================================================
FUNCTION Qcamera_GetDir

  DESCRIPTION
    This function gets default directory for encoding and save location.

  PARAMETERS:
    enc     - encoding type
    saveLoc - save location

  RETURN VALUE:
    directory pointer points to the default directory name
===========================================================================*/
static const char * Qcamera_GetDir(QCamEncodingType enc, QCamSaveType saveLoc)
{
  const char *pDir = NULL;
  if (enc == QCAM_ENCODING_RAW 
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif
)
    if (saveLoc == QCAM_SAVE_MMC)
      pDir = QCAM_MMC_RAW_DIR;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS 
    else if (saveLoc == QCAM_SAVE_USB)
      pDir = QCAM_USB_RAW_DIR;
#endif
    else if (saveLoc == QCAM_SAVE_FTL)
      pDir = QCAM_FTL_RAW_DIR;
    else
      pDir = QCAM_RAW_DIR;
  else if (enc == QCAM_ENCODING_YCBCR)
    if (saveLoc == QCAM_SAVE_MMC)
      pDir = QCAM_MMC_YCBCR_DIR;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
    else if (saveLoc == QCAM_SAVE_USB)
      pDir = QCAM_USB_YCBCR_DIR;
#endif
    else
      pDir = QCAM_YCBCR_DIR;
  else
  {
     if (saveLoc == QCAM_SAVE_MMC)
      pDir = QCAM_MMC_DIR;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS 
     else if (saveLoc == QCAM_SAVE_USB)
        pDir = QCAM_USB_DIR;
#endif
    else if (saveLoc == QCAM_SAVE_FTL)
      pDir = QCAM_FTL_DIR;
    else
      pDir = QCAM_PHOTOS_DIR;
  }
  return pDir;
}

/*===========================================================================
FUNCTION Qcamera_GetExtension

  DESCRIPTION
    This function gets default file extension for encoding type.

  PARAMETERS:
    enc     - encoding type

  RETURN VALUE:
    extension pointer points to the default file extension
===========================================================================*/
static const char * Qcamera_GetExtension(QCamEncodingType enc)
{
  const char *pExt = NULL;
  switch (enc)
  {
  case QCAM_ENCODING_YCBCR:
    pExt = QCAM_YCBCR_EXTENSION;
    break;
  case QCAM_ENCODING_RAW:
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif
    pExt = QCAM_RAW_EXTENSION;
    break;
  case QCAM_ENCODING_PNG:
    pExt = QCAM_PNG_EXTENSION;
    break;
  default:
    pExt = QCAM_JPEG_EXTENSION;
    break;
  }

  return pExt;
}

/*===========================================================================
FUNCTION Qcamera_GetDirLen

  DESCRIPTION
    This function gets length of root directory and data directory.

  PARAMETERS:
    pMe - Pointer to  QCam struct
    rootDirLen - Pointer to length of root directory
    dataDirLen - Pointer to length of data directory

  RETURN VALUE:
    NONE
===========================================================================*/
void Qcamera_GetDirLen(QCam *pMe, uint32 *rootDirLen, uint32 *dataDirLen)
{
  if (!pMe || !rootDirLen || !dataDirLen)
    return;

  //Update the saveLoc in case MMC is removed after camera starts.
  if ((pMe->m_saveLoc == QCAM_SAVE_MMC) &&
      (SUCCESS != IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_CARD0_DIR)))
  {
    pMe->m_saveLoc = QCAM_SAVE_FLASH;
  }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  if ((pMe->m_saveLoc == QCAM_SAVE_USB) &&
      (SUCCESS != IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_MOD_USB_DIR)))
  {
    pMe->m_saveLoc = QCAM_SAVE_FLASH;
  }
#endif

  if (!pMe->m_bFTL && (pMe->m_saveLoc == QCAM_SAVE_FTL))
  {
    pMe->m_saveLoc = QCAM_SAVE_FLASH;
  }

  switch (pMe->m_encodingType)
  {
  case QCAM_ENCODING_RAW:
    if (pMe->m_saveLoc == QCAM_SAVE_MMC)
    {
      *rootDirLen = QCAM_MMC_RAW_DIR_LEN;
      *dataDirLen = QCAM_MMC_RAW_DATE_DIR_LEN;
    }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
    else if (pMe->m_saveLoc == QCAM_SAVE_USB)
    {
      *rootDirLen = QCAM_USB_RAW_DIR_LEN;
      *dataDirLen = QCAM_USB_RAW_DATE_DIR_LEN;
    }
#endif
    else if (pMe->m_saveLoc == QCAM_SAVE_FTL)
    {
      *rootDirLen = QCAM_FTL_RAW_DIR_LEN;
      *dataDirLen = QCAM_FTL_RAW_DATE_DIR_LEN;
    }
    else
    {
      *rootDirLen = QCAM_RAW_DIR_LEN;
      *dataDirLen = QCAM_RAW_DATE_DIR_LEN;
    }
    break;
  case QCAM_ENCODING_YCBCR:
    if (pMe->m_saveLoc == QCAM_SAVE_MMC)
    {
      *rootDirLen = QCAM_MMC_YCBCR_DIR_LEN;
      *dataDirLen = QCAM_MMC_YCBCR_DATE_DIR_LEN;
    }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
    else if(pMe->m_saveLoc == QCAM_SAVE_USB)
    {
      *rootDirLen = QCAM_USB_YCBCR_DIR_LEN;
      *dataDirLen = QCAM_USB_YCBCR_DATE_DIR_LEN;
    }
#endif
    else
    {
      *rootDirLen = QCAM_YCBCR_DIR_LEN;
      *dataDirLen = QCAM_YCBCR_DATE_DIR_LEN;
    }
    break;
  default:
    if (pMe->m_saveLoc == QCAM_SAVE_MMC)
    {
      *rootDirLen = QCAM_MMC_DIR_LEN;
      *dataDirLen = QCAM_MMC_DATE_DIR_LEN;
    }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
    else if (pMe->m_saveLoc == QCAM_SAVE_USB)
    {
      *rootDirLen = QCAM_USB_DIR_LEN;
      *dataDirLen = QCAM_USB_DATE_DIR_LEN;
    }
#endif
    else if (pMe->m_saveLoc == QCAM_SAVE_FTL)
    {
      *rootDirLen = QCAM_FTL_DIR_LEN;
      *dataDirLen = QCAM_FTL_DATE_DIR_LEN;
    }
    else
    {
      *rootDirLen = QCAM_PHOTOS_DIR_LEN;
      *dataDirLen = QCAM_PHOTOS_DATE_DIR_LEN;
    }
  }
}

/*===========================================================================
FUNCTION Qcamera_GetFileName

  DESCRIPTION
    This function gets a new file name and update date variables.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    TRUE if the file name is not null.
===========================================================================*/
boolean Qcamera_GetFileName(QCam *pMe)
{
  uint16 count;
  const char *pDir;
  const char *pExt;

  if (!pMe)
    return FALSE;

  count = pMe->m_wCurrentImage;
  if((pMe->m_saveLoc == QCAM_SAVE_MMC) && 
     (SUCCESS != IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_CARD0_DIR)))
  {
    pMe->m_saveLoc = QCAM_SAVE_FLASH;
  }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  if((pMe->m_saveLoc == QCAM_SAVE_USB) && 
     (SUCCESS != IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_MOD_USB_DIR)))
  {
    pMe->m_saveLoc = QCAM_SAVE_FLASH;
  }
#endif
  
  pDir = Qcamera_GetDir(pMe->m_encodingType, pMe->m_saveLoc);
  pExt = Qcamera_GetExtension(pMe->m_encodingType);
  QCam_GenerateFileName(pMe->m_pIFileMgr, pDir, pExt, &count, pMe->m_sCurrentFileName);

  if (pMe->m_sCurrentFileName[0] == '\0')
  {
    MSG_HIGH("Cannot create a file name.",0,0,0);
    return FALSE;
  }

  //Double check if the directory is existing if it is newly created.
  //To create a directory in the MMC card takes longer time.
  //It may be not available even the MkDir() returns SUCCESS.
  //Set a timer to delay and retry test again for several times.
  pMe->m_nRetry = 3;
  if (!Qcamera_ValidateDir(pMe))
  {
    MSG_HIGH("Cannot create a file path.",0,0,0);
    return FALSE;
  }

  //Start a new date folder. Update time variables.
  if (count != pMe->m_wCurrentImage + 1)
    QCam_UpdateCurrentTime(&pMe->m_wLastImageDay, &pMe->m_wLastImageMonth, &pMe->m_wLastImageYear);

  //Update the current image number.
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif
  {
    pMe->m_wCurrentImage = count;
  }
  return TRUE;
}


/*=====================================================================
FUNCTION Qcamera_GetPreviewRect

  DESCRIPTION: This function get the preview rectangle in the way the camera
               does it. So the overlay add from UI can match the preview
               image location. (LCD in QVGA size)

  PARAMETERS:
    pMe  - Pointer to  QCam struct

  RETURN VALUE:
    AEERect - rect matching the preview image location.

  DEPENDENCY: m_rcDisp is ready by calling QCamDisp_GetDisplayRect.
======================================================================*/
AEERect Qcamera_GetPreviewRect(QCam *pMe)
{
  AEERect rc = {0};
  AEESize *pSize;
  AEESize dispSize;
#ifndef FEATURE_OVERLAY
  AEESize* pImgRatio;
#endif

  if (!pMe)
    return rc;

  //Check if user enabled LCD size display.
  if (!pMe->m_bDisplayLCDSize)
  {
    pSize = Qcamera_GetSize(pMe);
    SETAEERECT (&rc, pMe->m_rcDisp.x, pMe->m_rcDisp.y, 
                pMe->m_rcDisp.dx, pMe->m_rcDisp.dy);
#ifdef FEATURE_OVERLAY
    //Get image display size
    dispSize.cx = pSize->cx;
    dispSize.cy = pSize->cy;
#else
    //Get LCD display size.
    dispSize.cx = pMe->m_rcDisp.dx;
    dispSize.cy = pMe->m_rcDisp.dy;

    // Get ImageRatio
    if (pMe->m_sensorInfo.bLandscape)
      pImgRatio = QCam_GetLandscapeImageRatio(pMe->m_nSize);
    else
      pImgRatio = QCam_GetImageRatio(pMe->m_nSize);

    // If pSize is smaller than display size, set display to pSize
    if ((pSize->cx <= dispSize.cx) && (pSize->cy <= dispSize.cy))
    {
      dispSize.cx = pSize->cx;
      dispSize.cy = pSize->cy;
    }
    else if (pSize->cx <= dispSize.cx)
    {
      // We know that our picture cy is larger than display cy
      dispSize = QCamDisp_AdjustDisplaySizeFitY(pImgRatio, dispSize);
    }
    else if (pSize->cy <= dispSize.cy)
    {
      // We know that our picture cx is larger than display cx
      dispSize = QCamDisp_AdjustDisplaySizeFitX(pImgRatio, dispSize);
    }
    else
    {
      // We know that our picture cx and cy is larger than the display
      dispSize = QCamDisp_AdjustDisplaySize(pImgRatio, dispSize);
    }
#endif //FEATURE_OVERLAY

#ifdef FEATURE_CAMERA7500
    //In 7K, set the full display to VGA for WVGA screen if wide display
    //is not enabled or FEATURE_CAMERA_WIDESIZE is not defined.
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE
    {
      if (rc.dx == QCAM_WVGA_WIDTH)   
      {
        rc.x = (QCAM_WVGA_WIDTH - QCAM_VGA_WIDTH) >> 1;
        rc.dx = QCAM_VGA_WIDTH; 
      }
    }
#endif //FEATURE_CAMERA7500

    // Now center our display rect
    rc = QCamDisp_GetCenterRect(rc, (int16)dispSize.cx, (int16)dispSize.cy);

    return rc;
  }
  
  //Get LCD display size.
  return pMe->m_rcDisp;  
}

/*===========================================================================
FUNCTION Qcamera_StopCamera

  DESCRIPTION
    This function stops the camera and resets the state before switching
    to other screen.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    TRUE sccessful stop, FALSE otherwise
===========================================================================*/
static boolean Qcamera_StopCamera(QCam *pMe)
{
  if (!pMe)
    return FALSE;

#ifdef FEATURE_CAMERA_HISTOGRAM
  //Stop histogram before stop preview
  if (pMe->m_nPrvwHistogram != CAM_HISTOGRAM_OFF)
  {
    //Only tell low layer stop it. UI keep the option and resume on start preview
    (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_HISTOGRAM, CAM_HISTOGRAM_OFF, 0);
  }
#endif //#ifdef FEATURE_CAMERA_HISTOGRAM

  MSG_MED("Stop Camera Preview", 0, 0, 0);
  if (ICAMERA_Stop(pMe->m_pICam) != SUCCESS)
  {
    MSG_HIGH("Qcamera_StopCamera: ICAMERA_Stop failure.",0,0,0);
    return FALSE;
  }

  if (pMe->m_bOneButtonSnapshot)
  {
    MSG_HIGH("One Button AF & Snapshot is still in progress", 0, 0, 0);
  }

  pMe->m_eState = QCAM_STATE_NULL;
  (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

#ifdef FEATURE_OVERLAY
#ifdef FEATURE_MMOVERLAY //7K
  //Pass TRUE to register full screen display
  Qcamera_RegisterOvScreen(pMe, TRUE, FALSE);
#else //6K
  //Register full screen for message display
  QCamDisp_MdpRegisterFullScreen(pMe->a.m_pIDisplay, pMe->m_pIOverlay,
                                 pMe->di.cxScreen, pMe->di.cyScreen);
#endif // #ifdef FEATURE_MMOVERLAY
#endif //FEATURE_OVERLAY

  return TRUE;
}


/*=====================================================================
FUNCTION Qcamera_StopPreview

  DESCRIPTION:
    This function stops the camera privew before taking picture.
    If it fails, send automation report and log F3 message.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    void
======================================================================*/
static void Qcamera_StopPreview(QCam *pMe)
{
  if (!pMe)
    return;

#ifdef FEATURE_CAMERA_HISTOGRAM
  //Stop histogram before stop preview
  if (pMe->m_nPrvwHistogram != CAM_HISTOGRAM_OFF)
  {
    //Only tell low layer stop it. UI keep the option and resume on start preview
    (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_HISTOGRAM, CAM_HISTOGRAM_OFF, 0);
  }
#endif //#ifdef FEATURE_CAMERA_HISTOGRAM

  if (ICAMERA_Stop(pMe->m_pICam) != SUCCESS)
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    //Could not stop preview.
    MSG_HIGH("ICAMERA_Stop failure.",0,0,0);
  }
}

/*===========================================================================
FUNCTION Qcamera_UpdateFrame

  DESCRIPTION
    This function updates the display with the frame from the viewfinder.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_UpdateFrame(QCam *pMe)
{
  IBitmap *pFrame = NULL;
  int y = 0;

  if (!pMe)
    return;
  // IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame) after you're done with pFrame.
  //
  if ((pMe->m_eState == QCAM_STATE_SAVE) && (pMe->m_nMulPhotos > 1))
    (void)ICAMERA_GetFrameEx(pMe->m_pICam, (int)pMe->m_nShutIdx, &pFrame);
  else
    (void)ICAMERA_GetFrame(pMe->m_pICam, &pFrame);

  if (!pFrame)
    return;

  y = pMe->m_rcPreView.y;
#ifdef FEATURE_OVERLAY
  if (!pMe->m_bDisplayLCDSize && (pMe->m_rcPreView.dy < pMe->m_rcDisp.dy))
  {
    //Service layer always shift up for mdp to get the full screen
    //display for full image with QVGA LCD. We need to offset it for
    //images smaller than QVGA (or what ever the default screen size is)
    //to make it display on the center (match the UI display coordinate).
    y += QCAM_LCD_OFFSET;
  }
#endif //FEATURE_OVERLAY
  // Display the frame at center location of the screen
  IDISPLAY_BitBlt(pMe->a.m_pIDisplay, pMe->m_rcPreView.x, y, pMe->m_rcPreView.dx, \
                  pMe->m_rcPreView.dy, pFrame, 0, 0, AEE_RO_COPY);

#ifdef FEATURE_CAMERA_MOBICAT
  if (pMe->m_bMobiCaTon)
  {
    camera_tune_lcd_ui_updater();
  }
#endif //FEATURE_CAMERA_MOBICAT

  //No need to call this if using MDP update
#ifndef FEATURE_OVERLAY
  IDISPLAY_Update(pMe->a.m_pIDisplay);
#elif defined FEATURE_CAMERA_LCD_DIRECT_MODE
  if (!pMe->m_nDirectMode)
  {
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,FALSE);
  }
#endif
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
  if (QCam_CheckProperties(pMe->m_nProperties, QCAM_MULTI_DISP))
  {
    QCamDisp_DisplayOvOnTV(pMe->a.m_pIShell, &pMe->m_pOvFrame, pFrame);
  }
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY

  CAM_RELEASEIF(pFrame);
}

/*=====================================================================
FUNCTION Qcamera_BuildSaveDlg

  DESCRIPTION:

  PARAMETERS:
    pMe

  RETURN VALUE:
    void 
======================================================================*/
static void Qcamera_BuildSaveDlg(QCam *pMe)
{
  pMe->m_bSaveAppPic = TRUE;
  if (ISHELL_CreateDialog(pMe->a.m_pIShell, pMe->m_pResFile, IDD_SAVE_PIC, NULL) == SUCCESS)
  {
    //Work around a BREW issue from 3.1.3 to bring up the menu button
    //on dialog start up.
    IDialog * dlg = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    IMenuCtl* pCtl = (IMenuCtl *)IDIALOG_GetControl(dlg, IDM_SAVE_PIC_MENU);
    if (pCtl)
    {
      IMENUCTL_SetSel(pCtl, IDC_SAVE);
      (void)IMENUCTL_Redraw(pCtl);
    }
#ifdef FEATURE_CAMERA_PANORAMIC
    //The dialog loaded on the top of the frame, need to update it again.
    if (pMe->m_nPanoramic > 0)
    {
      Qcamera_UpdateStitchFrame(pMe);
    }
    else
#endif
    {
      Qcamera_UpdateFrame(pMe);
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
}

#ifdef FEATURE_CAMERA_PANORAMIC
/*===========================================================================
FUNCTION Qcamera_UpdateStitchFrame

  DESCRIPTION
    This function updates the display with the frame from the viewfinder.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_UpdateStitchFrame(QCam *pMe)
{
  IBitmap *pFrame = NULL;
  AEEBitmapInfo info;
  AEERect rcImg, rcDisp;
  int16 temp;

  if (!pMe)
    return;

  (void)ICAMERA_GetFrame(pMe->m_pICam, &pFrame);
  if (pFrame)
  {
    (void)IBITMAP_GetInfo(pFrame, &info, sizeof(info));

    if (info.cy == 0)
    {
      MSG_HIGH("Failed Qcamera_UpdateStitchFrame", 0, 0, 0);
      return;
    }
    rcImg.x = 0;
    rcImg.y = pMe->m_nTitleHeight;
    rcImg.dx = pMe->di.cxScreen;
    rcImg.dy = pMe->di.cyScreen - pMe->m_softMenuHeight;
    info.cx = (info.cx > pMe->di.cxScreen) ? pMe->di.cxScreen : info.cx;

    if (pMe->di.cxScreen < pMe->di.cyScreen)
    {
      temp = info.cx;
      info.cx = info.cy;
      info.cy = temp;
    }

    rcDisp = QCamDisp_GetCenterRect(rcImg, info.cx, info.cy);
          
    IDISPLAY_BitBlt(pMe->a.m_pIDisplay, rcDisp.x, rcDisp.y, rcDisp.dx, \
                    rcDisp.dy, pFrame, 0, 0, AEE_RO_COPY);
  }
  CAM_RELEASEIF(pFrame);
}

/*===========================================================================
FUNCTION Qcamera_UpdateNumberIconbar

  DESCRIPTION
    This function updates the display with the Panoramic snapshot number

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_UpdateNumberIconbar (QCam *pMe)
{
  OvData ov;
  IBitmap *pBmp = NULL;
  IImage *pImage = NULL;
  AEERect num;
  uint16 nID = 0;

  //Get fix location for number bar
  if (pMe->di.cxScreen < pMe->di.cyScreen)
  {
    num.x = 10;
    num.y = pMe->di.cxScreen/2;
    num.dx = 50;
    num.dy = 150;
  }
  else
  {
    num.x = (pMe->di.cxScreen/2);
    num.y = 10;
    num.dx = 150;
    num.dy = 50;
  }
  //Register the area if it is NULL
  if (!pMe->m_pIMMOverlayInfo)
  {
    if (IDISPLAY_GetDeviceBitmap (pMe->a.m_pIDisplay, &pBmp) == SUCCESS)
    {
      ov.alpha = 128; //half alpha blend
      ov.transparent = RGB_WHITE;
      ov.pBmp = pBmp;
      ov.layer = OV_LAYER_TOP_LAYER;

      SETAEERECT (&ov.rc, num.x, num.y, num.dx, num.dy);
      (void)QCamDisp_RegisterOv(pMe->m_pIMMOverlayMgr, &pMe->m_pIMMOverlayInfo, ov);
      CAM_RELEASEIF(pBmp);
    }
    else
    {
      MSG_HIGH("Failed IDISPLAY_GetDeviceBitmap for Number bar", 0, 0, 0);
      return;
    }
  }
  //Load the Panoramic number bar icon
  switch (pMe->m_nPanoramic)
  {
  case 2:
    nID = (pMe->di.cxScreen < pMe->di.cyScreen) ?
          IDB_PANORAMIC_POTRAIT2 : IDB_PANORAMIC_LANDSCAPE2;
    break;
  case 3:
    nID = (pMe->di.cxScreen < pMe->di.cyScreen) ?
          IDB_PANORAMIC_POTRAIT3 : IDB_PANORAMIC_LANDSCAPE3;
    break;
  default:
    break;
  }

  pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, nID);
  if (pImage)
  {
    QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, num);
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
  }
}

/*=====================================================================
FUNCTION *Qcamera_GetPartialFrame

  DESCRIPTION:

  PARAMETERS:
    pMe
    pIIpl

  RETURN VALUE:
    IBitmap 
======================================================================*/
static IBitmap *Qcamera_GetPartialFrame(QCam *pMe, IIpl *pIIpl)
{
  IBitmap *pFrame    = NULL;
  IBitmap *pBmpYCbCr = NULL;
  IBitmap *pBmpComp  = NULL;
  IBitmap *pBmp      = NULL;
  IBitmap *pBmpTemp  = NULL;
  IBitmap *pBmpSize  = NULL;
  IBitmap *pBmpRota  = NULL;
  AEEBitmapInfo info;
  AEERect rcSrc, rcDec;
  int16 x = 0;
  
  if (!pMe || !pIIpl)
    return NULL;

  if (ICAMERA_GetFrame(pMe->m_pICam, &pFrame) != SUCCESS)
    return NULL;

  if ((IBITMAP_QueryInterface(pFrame, AEEIID_YCBCR, (void** )&pBmpYCbCr) == SUCCESS)
     && (IDISPLAY_GetDeviceBitmap(pMe->a.m_pIDisplay, &pBmpComp) == SUCCESS))
  {
    //IIPL_CopyPaste only support the same color format for input and output.
    //We have to convert the color format first.
    (void)IBITMAP_GetInfo(pBmpYCbCr, &info, sizeof(info));
    if ((IBITMAP_CreateCompatibleBitmap(pBmpComp, &pBmp, (int16)info.cx, (int16)info.cy)
        == SUCCESS) && (IIPL_ConvertColor(pIIpl, pBmpYCbCr, pBmp) == SUCCESS))
    {
      CAM_RELEASEIF(pBmpYCbCr);
      CAM_RELEASEIF(pFrame);

      SETAEERECT(&rcDec, 0, 0, (info.cx / 4), info.cy);
      x = (pMe->di.cxScreen < pMe->di.cyScreen) ? 
          0 : (int16)(info.cx - rcDec.dx);
      SETAEERECT(&rcSrc, x, 0, (info.cx / 4), info.cy);

      //Get 1/4 of the original snapshot first.
      if ((IBITMAP_CreateCompatibleBitmap(pBmpComp, &pBmpTemp,  rcDec.dx, rcDec.dy)
          == SUCCESS) &&
          (IIPL_CopyPaste(pIIpl, pBmp, pBmpTemp, &rcSrc, &rcDec) == SUCCESS))
      {
        //Do rotation if screen display in portrat mode
        if (pMe->di.cxScreen < pMe->di.cyScreen)
        {
          if ((IBITMAP_CreateCompatibleBitmap(pBmpComp, &pBmpRota, rcDec.dy, rcDec.dx)
              == SUCCESS) &&
              (IIPL_Rotate(pIIpl, pBmpTemp, pBmpRota, IIPL_ROTATE_90) == SUCCESS))
          {
            CAM_RELEASEIF(pBmpTemp);
            pBmpTemp = pBmpRota;
          }
          else
          {
            MSG_HIGH("Failed to rotate snapshot", 0, 0, 0);
            CAM_RELEASEIF(pBmpTemp);
          }
        }
        //if image size is VGA, need to upsize the snapshot (only in QVGA size)
        //to match preview
        if (pBmpTemp)
        {
          Qcamera_GetOverlayRc(pMe, &rcDec);
          (void)IBITMAP_GetInfo(pBmpTemp, &info, sizeof(info));
          if ((rcDec.dx != (int16)info.cx) || (rcDec.dy != (int16)info.cy))
          {
            if ((IBITMAP_CreateCompatibleBitmap(pBmpComp, &pBmpSize, rcDec.dx, rcDec.dy)
                == SUCCESS) &&
                (IIPL_ChangeSize(pIIpl, pBmpTemp, pBmpSize) == SUCCESS))
            {
              CAM_RELEASEIF(pBmpTemp);
              pBmpTemp = pBmpSize;
            }
            else
            {
              MSG_HIGH("Failed to resize snapshot", 0, 0, 0);
              CAM_RELEASEIF(pBmpTemp);
            }
          }
        }
      }
      else
      {
        MSG_HIGH("Failed to copy snapshot", 0, 0, 0);
        CAM_RELEASEIF(pBmpTemp);
      }
    }
    else
    {
      MSG_HIGH("Failed to convert snapshot color", 0, 0, 0);
    }
  }
  else
  {
    MSG_HIGH("Failed to get snapshot", 0, 0, 0);
  }
  
  CAM_RELEASEIF(pBmp);
  CAM_RELEASEIF(pBmpComp);
  CAM_RELEASEIF(pBmpYCbCr);
  CAM_RELEASEIF(pFrame);
  
  return pBmpTemp;
}

/*=====================================================================
FUNCTION Qcamera_GetOverlayRc

  DESCRIPTION:

  PARAMETERS:
    pMe
    pRc

  RETURN VALUE:
    void 
======================================================================*/
static void Qcamera_GetOverlayRc(QCam *pMe, AEERect *pRc)
{
  AEERect rc;
  int16 y;

  if (!pMe || !pRc)
    return;

  y = pMe->m_rcPreView.y;
  if(pMe->m_rcPreView.dy < pMe->m_rcDisp.dy)
  {
    y += QCAM_LCD_OFFSET;
  }

  if (pMe->di.cxScreen > pMe->di.cyScreen)
  {
    SETAEERECT(&rc, pMe->m_rcPreView.x, y, 
               pMe->m_rcPreView.dx / 4, pMe->m_rcPreView.dy);
  }
  else
  {
    SETAEERECT(&rc, pMe->m_rcPreView.x, 
               y + (pMe->m_rcPreView.dy * 3/4), 
               pMe->m_rcPreView.dx, pMe->m_rcPreView.dy / 4);
  }

  //upshift y to origin for full screen display in case of annunciator offset
  if (pMe->m_rcPreView.dy > pMe->di.cyScreen)
  {
    rc.y = (int16)pMe->di.cyScreen - pMe->m_rcPreView.dy;
  }

  *pRc = rc; 
}
/*===========================================================================
FUNCTION Qcamera_UpdatePanoramicFrame

  DESCRIPTION
    This function updates the display with the rightmost 1/4th of 
    the previous snapshot at left most part of the display using overlay

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static boolean Qcamera_UpdatePanoramicFrame (QCam *pMe)
{
  IIpl    *pIIpl = NULL;
  IBitmap *pBmp = NULL;
  boolean  bOk = FALSE;
  AEERect  rcDisp;

  if (!pMe)
    return FALSE;

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IPL, (void ** )&pIIpl) 
      != SUCCESS)
  {
    MSG_HIGH("Failed Create IPL instance", 0, 0, 0);
    return FALSE;
  }

  //Get 1/4 of last snapshot.
  pBmp = Qcamera_GetPartialFrame(pMe, pIIpl);
  CAM_RELEASEIF(pIIpl);

  //Create Overlay object and display
  if (pBmp)
  {
    Qcamera_GetOverlayRc(pMe, &rcDisp);
    if (QCamera_GetOvObj(pMe, pBmp, rcDisp, &pMe->m_pOvPanoramic))
    {
      bOk = TRUE;
      (void)IMMOVERLAY_Update(pMe->m_pOvPanoramic, NULL);
    }
  }
  else
  {
    MSG_HIGH("Failed Qcamera_UpdatePanoramicFrame", 0, 0, 0);
  }

  CAM_RELEASEIF(pBmp);
  return bOk ? TRUE : FALSE;
}
#endif //FEATURE_CAMERA_PANORAMIC

/*===========================================================================
FUNCTION Qcamera_UpdatePerStep

  DESCRIPTION
    This function updates the nCur value per step change.

  PARAMETERS:
    pMe - QCam data pointer
    nCur - current value
    step - value in one step
    max - max value in range
    min - min value in range
    key - Up/Down key

  RETURN VALUE:
    new value updated
===========================================================================*/
static int32 Qcamera_UpdatePerStep(QCam *pMe, int32 nCur, int32 step,
                                   int32 max, int32 min, uint16 key)
{
  boolean bInRange = TRUE;
  int32 nVal = (key == (uint16)AVK_UP) ? (nCur + step) : (nCur - step);

  if (!pMe)
    return nCur;

  if (nVal > max)
  {
    MSG_MED("Max. value %d.", max, 0, 0);
    bInRange = FALSE;
    nVal = max;
  }
  else if (nVal < min)
  {
    MSG_MED("Min. value %d.", min, 0, 0);
    bInRange = FALSE;
    nVal = min;
  }

  if (!bInRange)
  {
    (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, TRUE);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
  }

  return nVal;
}

/*===========================================================================
FUNCTION Qcamera_PreLaunchViewer

  DESCRIPTION
    This function prepares for launch the Qalbum application.

  PARAMETERS:
    pMe - Pointer to  QCam struct
    mode - launch mode

  RETURN VALUE:
    TRUE - success, FALSE - failed
===========================================================================*/
static boolean Qcamera_PreLaunchViewer(QCam *pMe, QalbumLaunchMode mode)
{
  if (!pMe)
    return FALSE;

  pMe->m_pLaunchDS = (Qalbum_LaunchDS *)MALLOC(sizeof(Qalbum_LaunchDS));
  if (!pMe->m_pLaunchDS)
  {
    MSG_HIGH("Cannot create the Viewer launch data struct.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    return FALSE;
  }
  ZEROAT(pMe->m_pLaunchDS);
  pMe->m_pLaunchDS->mode = mode;
  pMe->m_pLaunchDS->nSaveLoc = pMe ->m_saveLoc;
  pMe->m_pLaunchDS->nSlideshowSpeed = pMe->m_nSlideshowSpeed;
  pMe->m_pLaunchDS->bDirSortAscend = QCam_CheckProperties(pMe->m_nProperties,
                                                          QCAM_DIR_ASCENDING);
  pMe->m_pLaunchDS->bPhotoSortAscend = QCam_CheckProperties(pMe->m_nProperties,
                                                            QCAM_PHOTO_ASCENDING);
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
  pMe->m_pLaunchDS->bMultiDisplayMode = QCam_CheckProperties(pMe->m_nProperties,
                                                             QCAM_MULTI_DISP);
#else
  pMe->m_pLaunchDS->bMultiDisplayMode = FALSE;
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef FEATURE_JPEGD_RESIZE
  pMe->m_pLaunchDS->bSlideshowResize = QCam_CheckProperties(pMe->m_nProperties, QCAM_SLIDESHOW_RESIZE); 
#else //FEATURE_JPEGD_RESIZE
  pMe->m_pLaunchDS->bSlideshowResize = FALSE;
#endif //FEATURE_JPEGD_RESIZE
  if (mode == VIEWER_MODE_FRAME)
  {
    //Initialize with the current frame name. If the user selects to add no frame,
    //it will clear the old name. If the user selects cancel, no change to it.
    if (pMe->m_sFrameFileName[0] != '\0')
    {
      (void)STRLCPY(pMe->m_pLaunchDS->sFrameFileName,
                   (const char *)pMe->m_sFrameFileName,
                   FILE_NAME_SIZE);
    }

    (void)STRLCPY(pMe->m_pLaunchDS->sFrameSizeDir, 
                 (char *)Qcamera_GetMatchSizeFrameDir(pMe),
                 FRAME_SDIR_SIZE);
  }

  //Call this function to stop the camera.
  //In the callback handler it will launch the viewer application.
  if (!Qcamera_StopCamera(pMe))
  {
    CAM_FREEIF(pMe->m_pLaunchDS);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION Qcamera_GetGPSid

  DESCRIPTION
    This function gets the GPS id based on the GPS state

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    GPS id
===========================================================================*/
static uint16 Qcamera_GetGPSid(QCam *pMe)
{
  uint16 id = IDS_GPS_DISABLED;

  if (!pMe)
    return id;

  switch (pMe->m_eGPSState)
  {
  case QCAM_GPS_DISABLED:
    id = IDS_GPS_DISABLED;
    break;
  case QCAM_GPS_ENABLED:
    id = IDS_GPS_ENABLED;
    break;
  case QCAM_GPS_DATA_VALID:
    id = IDS_GPS_DATA_VALID;
    break;
  case QCAM_GPS_ACQUIRING:
    id = IDS_GPS_ACQUIRING;
    break;
  case QCAM_GPS_FAILURE:
    id = IDS_GPS_FAILURE;
    break;
  case QCAM_GPS_TIMEOUT:
    id = IDS_GPS_TIMEOUT;
    break;
  case QCAM_GPS_INFO_UNAVAILABLE:
    id = IDS_GPS_INFO_UNAVAILABLE;
    break;
  default:
    break;
  }

  return id;

}

/*===========================================================================
FUNCTION Qcamera_GetGPSInfo

  DESCRIPTION
    This function writes the current setting data to the configuration file.
    The function is called on Qcamera application stop

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_GetGPSInfo(QCam *pMe)
{


  if (!pMe)
    return;

  if (pMe->m_eGPSState == QCAM_GPS_ENABLED)
  {
    //It is a long processing to acquire GPS info, draw a hour glass.
    Qcamera_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

    // Get GPS info
    if (pMe->m_pIPosDet)
    {
      int result = 0;
      AEEGPSConfig config = {0};

      pMe->m_eGPSState = QCAM_GPS_ACQUIRING;

      /*Setting the qos timer to89 sec for standalone gpsfix, else it was
        defaulted to 20sec in AEE layer*/
      config.qos = 89; 
      result = IPOSDET_SetGPSConfig(pMe->m_pIPosDet, &config);
      MSG_HIGH("Set Done..result is %d",result,0,0);
      result = IPOSDET_GetGPSInfo(pMe->m_pIPosDet,
                                 AEEGPS_GETINFO_LOCATION | AEEGPS_GETINFO_ALTITUDE,
                                 pMe->m_nGPSAccuracy,  &pMe->m_GPSInfo, &pMe->GPSInfoCB);

      if (result != SUCCESS)
      {
        pMe->m_eGPSState = QCAM_GPS_DISABLED;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      }
    }
  }
}

/*===========================================================================
FUNCTION Qcamera_ValidateDir

  DESCRIPTION
    This function validate the provided directory.
    If it is not existing, Add some delay by a for loop than try again.

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static boolean Qcamera_ValidateDir(QCam *pMe)
{
  char pDir[FILE_NAME_SIZE] = {0};

  if (!pMe)
    return FALSE;

  if (!QCam_GetFilePath(pMe->m_sCurrentFileName, pDir, FILE_NAME_SIZE))
    return FALSE;

  //This function needs to be a sync. function.
  //Otherwise, the whole logic has to be redesigned.
  while (SUCCESS != IFILEMGR_Test(pMe->m_pIFileMgr, pDir))
  {
    if (pMe->m_nRetry > 0)
    {
      int i;
      MSG_MED("Retry count %d", (3 - pMe->m_nRetry), 0, 0);
      pMe->m_nRetry--;
      for (i = 0; i < TIMEOUT_300; i++)
      {
        //Do nothing
      };
    }
    else
      return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION Qcamera_Qcamera_StartFocus

  DESCRIPTION
    This function starts Auto Focus if it is supported. Picture is taken
    after callback return from ICAMERA in Qcamera_CamEventNotify regardless
    Auto Focus success or failure.

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_StartFocus(QCam *pMe)
{
  int nRet = 0;

  if (!pMe)
    return;

  MSG_MED("Qcamera_StartFocus before take picture", 0, 0, 0);
  if (pMe->m_nFocusMax > 0 && (pMe->m_nPanoramic == 0) &&  \
      QCam_CheckProperties(pMe->m_nProperties, QCAM_1_BUTTON_SNAPSHOT))
  {
    //Set Auto Focus flag.
    pMe->m_bFocusStarted = TRUE;
    pMe->m_bOneButtonSnapshot = TRUE;
    //Update the status indicator
    Qcamera_UpdateAFIndicator(pMe, IDB_AF_START);

    //Set Auto Focus mode
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_FOCUS, CAM_FOCUS_AUTO, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_FOCUS_AUTO return %d", nRet, 0, 0);
      //Reset the status to No Focus
      Qcamera_ResetAFIndicator(pMe);
      //Go ahead to take picture
      Qcamera_PreTakePicture(pMe);
    }
  }
  else
  {
    //Auto Focus is not supported, go ahead to take picture
    Qcamera_PreTakePicture(pMe);
  }
}

/*===========================================================================
FUNCTION Qcamera_PreTakePicture

  DESCRIPTION
    This function prepares for taking picture and start the snapshot state:
    If a shutter shound is selected, play it.
    Otherwise stop the current camera. It is an asyn call.
    The new camera starts after call back return from ICAMERA
    in Qcamera_CamEventNotify

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_PreTakePicture(QCam *pMe)
{
  if (!pMe)
    return;

  MSG_MED("Start Qcamera_PreTakePicture", 0, 0, 0);
  pMe->m_eState = QCAM_STATE_SNAPSHOT;
  if (pMe->m_bGridOn)
  {
    //Turn off the grids before taking picture.
    pMe->m_bGridOn = FALSE;
    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rcPreView);
  }
  if (QCAM_HAS_SHUTTER_SOUND(pMe->m_sShutterSoundFile) && (pMe->m_nPanoramic < 2) && 
      (IFILEMGR_Test(pMe->m_pIFileMgr, pMe->m_sShutterSoundFile) == SUCCESS))
  {
#ifndef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND  
    MSG_MED("PreTakePicture - Play Shutter Sound", 0, 0, 0);
    Qcamera_PlayShutterSound(pMe);
#else /* if FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND is defined */
    MSG_MED("Stop Preview while shutter sound is playing", 0, 0, 0);
    Qcamera_StopPreview(pMe);
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
  }
  else
  {
    //No shutter sound. Stop preview.
    MSG_MED("PreTakePicture - Stop Preview", 0, 0, 0);
    Qcamera_StopPreview(pMe);
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
    MSG_MED("Stop Preview while shutter sound is not available", 0, 0, 0);
    (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_SHUTTER_SOUND, 0, 0);
    // If No Shutter sound is selcted. 
    pMe->m_bSnapShutterSndDone = TRUE; 
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
  }
}

/*===========================================================================
FUNCTION Qcamera_TakePicture

  DESCRIPTION
    This function start taking picture process:
    If m_nSelfTimerDelay is not zero, show delay time bar.
    Otherwise call Qcamera_PreTakePicture for next step

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_TakePicture(QCam *pMe)
{
  if (!pMe)
    return;

  //Check if space available first
  //No need to go through all if no space.
  if (!Qcamera_IsSpaceAvailable(pMe))
  {
    //Stop the preview to show the memory full message.
    if (Qcamera_StopCamera(pMe))
      pMe->m_nMsgId = (uint16)IDS_MEM_FULL;
    return;
  }

  if ((pMe->m_nSelfTimerDelay > 0) && (pMe->m_nPanoramic < 2))
  {
    pMe->m_nLeftSec = pMe->m_nSelfTimerDelay;
    Qcamera_DisplaySelfTimerBar(pMe);
  }
  else
  {
    Qcamera_StartFocus(pMe);
  }
} /* END Qcamera_TakePicture */

/*===========================================================================
FUNCTION Qcamera_DoSnapshot

  DESCRIPTION
    This function prepares the camera for snap shot and switches to record state.

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_DoSnapshot(QCam *pMe)
{
  int nRet = 0;

  if (!pMe)
    return;

#ifdef FEATURE_OVERLAY
  //Turn off the menu if it is MDP display
  if (pMe->m_bShowMenu)
    Qcamera_ToggleMenuDisplay(pMe);
#endif //FEATURE_OVERLAY

  //To support encoding with both file size and quality limit,
  //Pass the Max. file size if it is larger than zero.
  //And also pass the quality if it is larger than zero.
  if (pMe->m_nMaxFileSize > 0)
  {
    nRet = ICAMERA_SetMaxFileSize(pMe->m_pICam, pMe->m_nMaxFileSize);
    if (nRet != SUCCESS)
    {
      MSG_HIGH("ICAMERA_SetMaxFileSize error %d.", nRet, 0, 0);
      Qcamera_DisplayTakePicErr(pMe, IDS_FAIL_TAKEPICTURE);
      return;
    }
  }

  if (pMe->m_nQuality > 0)
  {
    nRet = ICAMERA_SetQuality(pMe->m_pICam, pMe->m_nQuality);
    if (nRet != SUCCESS)
    {
      MSG_HIGH("ICAMERA_SetQuality error %d.", nRet, 0, 0);
      Qcamera_DisplayTakePicErr(pMe, IDS_FAIL_TAKEPICTURE);
      return;
    }
  }

  switch(pMe->m_encodingType)
  {
  case QCAM_ENCODING_YCBCR:
    (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_YCBCR, 0);
    break;
  case QCAM_ENCODING_RAW:
    (void)ICAMERA_SetVideoEncode(pMe->m_pICam, CAM_ENCODE_RAW, 0);
    break;
  case QCAM_ENCODING_PNG:
    (void)ICAMERA_SetVideoEncode(pMe->m_pICam, AEECLSID_PNG, 0);
    break;
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif

  default:
    (void)ICAMERA_SetVideoEncode(pMe->m_pICam, AEECLSID_JPEG, 0);
    break;
  }

  if (pMe->m_eGPSState == QCAM_GPS_DATA_VALID)
  {
    (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_GPSINFO, (int32)&pMe->m_GPSInfo, 0);
  }
  else
  {
    (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_GPSINFO, 0, 0);
  }
  pMe->m_eState = QCAM_STATE_SAVE;
  (void)ICAMERA_DeferEncode(pMe->m_pICam, TRUE);
  MSG_MED("Call ICAMERA_RecordSnapshot",0,0,0);
  //Call for multishot if m_nMulPhotos larger than 1
  if (pMe->m_nMulPhotos > 1)
  {
    CameraSnapshotMulti psm;

    psm.nCount = (int)pMe->m_nMulPhotos;
    psm.dwInterval = pMe->m_nPhotoInterval;
    nRet = ICAMERA_RecordSnapshotEx(pMe->m_pICam, &psm);
  }
  else
  {
    nRet = ICAMERA_RecordSnapshot(pMe->m_pICam);
  }

  //When an error code returns, it may be no callback.
  //We have to return to preview here. Otherwise the UI stuck.
  if (nRet != SUCCESS)
  {
    MSG_HIGH("ICAMERA_RecordSnapshot error.",0,0,0);
    //Turn on pMe->m_bCanTakePic before return to preview mode.
    //It will enable take picture again in case of failure.
    Qcamera_DisplayTakePicErr(pMe, IDS_FAIL_TAKEPICTURE);
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
    // Snapshot Capture has been failed. 
    pMe->m_bSnapCaptureDone = FALSE;
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND 
  }
  else
  {
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
    // Snapshot Capture has been done sucessfully.
    pMe->m_bSnapCaptureDone = TRUE;
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND 
  }
}

/*===========================================================================
FUNCTION Qcamera_SavePhoto

  DESCRIPTION
    This function starts the encoding processing to save a new photo to the file.

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
void Qcamera_SavePhoto(QCam *pMe)
{
  AEEMediaData   md;
  int nRet = 0;

  if (!pMe)
    return;

  MSG_MED("ENTERING Save Photo",0,0,0);

  if(pMe->m_encodingType != QCAM_ENCODING_MEM)
  {
    //Fill media data
    md.clsData = MMD_FILE_NAME;
    md.pData = (void *)pMe->m_sCurrentFileName;
    md.dwSize = 0;
  }
  else
  {
    uint32 numbytes;
    AEESize *pSize = Qcamera_GetSize(pMe);
    //for mem mode, add 10K to reserve space for JPEG header in case of
    //image too small (STEMP).
    if (pMe->m_nSize == (uint16)QCAM_SIZE_STAMP)
      numbytes = (uint32)(pSize->cx * pSize->cy * 2 + 40000);
    else
      numbytes = (uint32)(pSize->cx * pSize->cy * 2 + 10000);

    //free buffer before using
    CAM_FREEIF(pMe->m_pData);
    md.clsData =  MMD_BUFFER;
    md.pData = (byte *)MALLOC(numbytes);
    pMe->m_pData = md.pData;
    md.dwSize = numbytes;
    pMe->m_nFileSize = md.dwSize;
  }

  //Start encoding processing
  (void)ICAMERA_SetMediaData(pMe->m_pICam, &md, 0);

  MSG_MED("Sending to encode picture",0,0,0);
  //When an error code returns, it may be no callback.
  //We have to return to preview here. Otherwise the UI stuck.
  if (pMe->m_nMulPhotos > 1)
    nRet = ICAMERA_EncodeSnapshotEx(pMe->m_pICam, (int)pMe->m_nShutIdx);
  else
    nRet = ICAMERA_EncodeSnapshot(pMe->m_pICam);

  if (nRet != SUCCESS)
  {
    MSG_HIGH("ICAMERA_EncodeSnapshot error.",0,0,0);
    //Turn on pMe->m_bCanTakePic before return to preview mode.
    //It will enable take picture again in case of failure.
    Qcamera_DisplayTakePicErr(pMe, IDS_FAIL_TAKEPICTURE);
  }

#ifdef HEAP_CHECK
  MSG_HEAP("Save Photo:", 0, 0, 0);
  Qcamera_ShowHeapUsage(pMe);
#endif //HEAP_CHECK
} /* END Qcamera_SavePhoto */

/*===========================================================================
FUNCTION Qcamera_DoAutoSave

  DESCRIPTION
    This function creates a new file name  and calls the save photo function
    to save the picture just taken.

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_DoAutoSave(QCam *pMe)
{
  if (!pMe)
    return;

  if (Qcamera_GetFileName(pMe))
  {
    //Display the saved image name on the title bar.
    Qcamera_DisplaySavedImageName(pMe);

    Qcamera_SavePhoto(pMe);
  }
  else
  {
    //Failed to create a file name for the new image, return to preview mode
    Qcamera_DisplayTakePicErr(pMe, IDS_FAIL_TAKEPICTURE);
  }
}

/*===========================================================================
FUNCTION Qcamera_StopSound

  DESCRIPTION
    This function stops the media and release it.
    It is a safety function. Only works if m_pMedia is not NULL

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_StopSound(QCam *pMe)
{
  if (!pMe)
    return;

  if (pMe->m_pMedia)
  {
    (void)IMEDIA_Stop(pMe->m_pMedia);
  }
  CAM_RELEASEIF(pMe->m_pMedia);
}

/*===========================================================================
FUNCTION Qcamera_PlayShutterSound

  DESCRIPTION
    This function plays a selected shutter sound.

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
void Qcamera_PlayShutterSound(QCam *pMe)
{
  AEEMediaData      md;
  AEEMediaCmdNotify cmd;

  if (!pMe)
    return;

  md.clsData = MMD_FILE_NAME;
  md.pData = (void *)pMe->m_sShutterSoundFile;
  md.dwSize = 0;

  //Stop the old sound if it is playing and create a new media
  Qcamera_StopSound(pMe);
  (void)AEEMediaUtil_CreateMedia(pMe->a.m_pIShell, &md, &pMe->m_pMedia);

  if (pMe->m_pMedia)
  {
    if (IMEDIA_RegisterNotify(pMe->m_pMedia, Qcamera_MediaNotify, pMe) != SUCCESS)
    {
      MSG_HIGH("IMEDIA_RegisterNotify failed.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      cmd.nCmd    = MM_CMD_PLAY;
      cmd.nStatus = MM_STATUS_DONE;
      Qcamera_MediaNotify((void *)pMe, &cmd);
      return;
    }

    if (CAM_ISTYPEFILE(QCAM_WAV_EXTENSION, pMe->m_sShutterSoundFile))
      (void) IMEDIA_SetMediaParm(pMe->m_pMedia, MM_PARM_CHANNEL_SHARE, FALSE, 0);

    (void) IMEDIA_SetMediaParm(pMe->m_pMedia, MM_PARM_AUDIO_DEVICE,AEE_SOUND_DEVICE_SPEAKER, 0);

    if (IMEDIA_Play(pMe->m_pMedia) != SUCCESS)
    {
      MSG_HIGH("IMEDIA_Play failed.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      cmd.nCmd    = MM_CMD_PLAY;
      cmd.nStatus = MM_STATUS_DONE;
      Qcamera_MediaNotify((void *)pMe, &cmd);
      return;
    }
  }
  else
  {
    //error handler for failed to create media
    MSG_HIGH("AEEMediaUtil_CreateMedia failed.",0,0,0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    cmd.nCmd    = MM_CMD_PLAY;
    cmd.nStatus = MM_STATUS_DONE;
    Qcamera_MediaNotify((void *)pMe, &cmd);
  }
}

/*===========================================================================
FU NCTION Qcamera_TurnOnCameraPreview

  DESCRIPTION
    This function starts the preview mode.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    NONE
===========================================================================*/
void Qcamera_TurnOnCameraPreview(QCam *pMe)
{
  if (!pMe)
    return;

  MSG_MED("SETTING CAN TAKE PIC TO TRUE",0,0,0);
  pMe->m_bMsgTimerOn = FALSE;
  pMe->m_activeSoftMenu = QCAM_SOFT_MENU_TAKE_PHOTO;
  (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_TAKE_PIC, 0);
}


/*=====================================================================
FUNCTION Qcamera_SetPreviewReady

  DESCRIPTION: This function prepare couple things before set the state to
               QCAM_STATE_PREVIEW.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    void 
======================================================================*/
static void Qcamera_SetPreviewReady(QCam *pMe)
{
  if (!pMe)
    return;

#ifdef FEATURE_CAMERA_PANORAMIC
  if (pMe->m_nPanoramic > 1)
  {
    // Display Number bar for panoramic mode
    Qcamera_UpdateNumberIconbar(pMe);
  }
  else
#endif //FEATURE_CAMERA_PANORAMIC
  {
    // Build the softmenu.
    QcamMenus_BuildTakePhotoMenu(pMe);
  }
#ifdef FEATURE_CAMERA_HISTOGRAM
  //set histogram when preview started if option not off
  if (pMe->m_nPrvwHistogram != CAM_HISTOGRAM_OFF)
  {
    int nRet = QCamera_SetHistogram(pMe);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("QCamera_SetHistogram return %d", nRet, 0, 0);
      CAM_RELEASEIF(pMe->m_pBmpHist);
    }
  }
#endif //#ifdef FEATURE_CAMERA_HISTOGRAM
  pMe->m_bCanTakePic = TRUE;
  pMe->m_eState = QCAM_STATE_PREVIEW;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/*===========================================================================
FUNCTION Qcamera_AddOverlay

  DESCRIPTION
    This function create the bitmap buffer from a data buffer and
    add the overlay.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_AddOverlay(QCam *pMe, const IDIB *pDIB)
{
  IBitmap *pIBitmap;

  if (pMe && pDIB)
  {
    //ICAMERA_AddOverlay() requires a IDIB pointer input
    //in IBitmap * type, not a real IBitmap pointer.
    pIBitmap = IDIB_TO_IBITMAP(pDIB);
    (void)ICAMERA_AddOverlay(pMe->m_pICam, pIBitmap);

#ifndef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
    //Clear the frame DIB before go ahead.
    CAM_RELEASEIF(pMe->m_pFrame);
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  }
}

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
/*===========================================================================
FUNCTION Qcamera_GetIconID

  DESCRIPTION
    This function gets next icon id from the index id.

  PARAMETERS:
    id - current icon index

  RETURN VALUE:
    next icon id
===========================================================================*/
static uint16 Qcamera_GetIconID(uint32 id)
{
   switch (id)
   {
      case 0:
         return IDB_ICON_1;
      case 1:
         return IDB_ICON_2;
      case 2:
         return IDB_ICON_3;
      case 3:
         return IDB_ICON_4;
      case 4:
         return IDB_ICON_5;
      case 5:
         return IDB_ICON_6;
      default:
         return IDB_ICON_1;
   }
}

/*===========================================================================
FUNCTION Qcamera_LoadIcons

  DESCRIPTION
    This function loads the icon list from the bitmap icons in the resource.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_LoadIcons(QCam *pMe)
{
  IBitmap *pIBitmap, *pIBitmap2, *pIBitmap3;
  IDIB *pDIB, *pDIB2;
  int numicons = 0;

  //If icon array is already loaded, return
  if (!pMe || pMe->m_bIconLoaded)
    return;

  do
  {
    pIBitmap = ISHELL_LoadResBitmap(pMe->a.m_pIShell, pMe->m_pResFile, Qcamera_GetIconID(numicons));
    if (pIBitmap == NULL)
    {
      MSG_HIGH("Could not get Bitmap",0,0,0);
      goto CleanBitmap;
    }

    //DIB for original image
    (void)IBITMAP_QueryInterface(pIBitmap, AEECLSID_DIB, (void**)&pDIB);
    if (!pDIB)
    {
      MSG_HIGH("Could not get DIB",0,0,0);
      goto CleanBitmap;
    }
    //create second bitmap
    (void)IDISPLAY_GetDeviceBitmap(pMe->a.m_pIDisplay, &pIBitmap2);
    if (!pIBitmap2)
    {
      MSG_HIGH("Could not get Bitmap",0,0,0);
      goto CleanBitmap;
    }
    //craete third bitmap
    (void)IBITMAP_CreateCompatibleBitmap(pIBitmap2, &pIBitmap3,  pDIB->cx, pDIB->cy);
    if (!pIBitmap3)
    {
      MSG_HIGH("Could not get Bitmap",0,0,0);
      goto CleanBitmap;
    }
    (void)IBITMAP_BltIn(pIBitmap3, 0,0, pDIB->cx, pDIB->cy, pIBitmap,0,0,AEE_RO_COPY );
    (void)IBITMAP_QueryInterface(pIBitmap3, AEECLSID_DIB, (void**)&pDIB2);
    if (!pDIB2)
    {
      MSG_HIGH("Could not get DIB",0,0,0);
      goto CleanBitmap;
    }

    //Call this function to load the data to the icon list.
    if (!Qcamera_LoadIconArray(pMe, pDIB2, numicons))
    {
      MSG_HIGH("Could not load icon array",0,0,0);
      goto CleanBitmap;
    }

    numicons++;

    CAM_RELEASEIF(pDIB);
    CAM_RELEASEIF(pDIB2);
    CAM_RELEASEIF(pIBitmap);
    CAM_RELEASEIF(pIBitmap2);
    CAM_RELEASEIF(pIBitmap3);
  } while (numicons < MAX_ICONS);

  pMe->m_bIconLoaded = TRUE;
  return;

CleanBitmap:
  CAM_RELEASEIF(pDIB);
  CAM_RELEASEIF(pDIB2);
  CAM_RELEASEIF(pIBitmap);
  CAM_RELEASEIF(pIBitmap2);
  CAM_RELEASEIF(pIBitmap3);
}

/*===========================================================================
FUNCTION Qcamera_ClearStamp

  DESCRIPTION
    This function clears the icon list from memory and add the frame overlay
    if it is available.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_ClearStamp(QCam *pMe)
{
  if (!pMe)
    return;

  if (pMe->m_bIconLoaded)
  {
    pMe->m_bIconLoaded = FALSE;
    Qcamera_ClearIcon(pMe);
  }

  //Safe function call.
  Qcamera_AddOverlay(pMe, pMe->m_pFrame);
}

/*===========================================================================
FUNCTION Qcamera_AddStampPreview

  DESCRIPTION
    This function sets the icon stamp to prewiew and adds the frame overlay.
    The stamp will not be encoded when taking a picture.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_AddStampPreview(QCam *pMe)
{
  if (!pMe)
    return;

  if (!pMe->m_bIconLoaded)
    Qcamera_LoadIcons(pMe);

  //Set the icon array overlay to preview
  if (!Qcamera_SetIconArray(pMe))
  {
    MSG_HIGH("Could not set icon array",0,0,0);
  }

  //Safe function call.
  Qcamera_AddOverlay(pMe, pMe->m_pFrame);
}

/*===========================================================================
FUNCTION Qcamera_AddStampEncode

  DESCRIPTION
    This function composes the icon stamps with the frame then sets the overlay.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_AddStampEncode(QCam *pMe)
{
  IDIB *pInDIB, *pOutDIB;
  AEESize *pSize;

  if (!pMe)
    return;

  //Load the icon list if it is not available
  if (!pMe->m_bIconLoaded)
    Qcamera_LoadIcons(pMe);

  //Compose the stamp images and the frame image together
  if (pMe->m_pFrame)
  {
    pInDIB = pMe->m_pFrame;
  }
  else
  {
    pInDIB = NULL;
  }

  pSize = Qcamera_GetSize(pMe);
  pOutDIB = Qcamera_ComposeStamp(pMe, pInDIB, *pSize);
  if (!pOutDIB)
  {
    MSG_HIGH("Qcamera_ComposeStamp failed", 0, 0, 0);
    //Safe function call.
    Qcamera_AddOverlay(pMe, pMe->m_pFrame);
    return;
  }

  //Add the overlay to preview.
  Qcamera_AddOverlay(pMe, pOutDIB);

  CAM_FREEIF(pOutDIB->pBmp);
  CAM_RELEASEIF(pOutDIB);
}

/*===========================================================================
FUNCTION Qcamera_AddRemoveIconsStamps

  DESCRIPTION
    This function adds the frame and icon stamps overlay based on the
    m_eIconState setting.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_AddRemoveIconsStamps(QCam *pMe)
{
  if (!pMe)
    return;

  switch(pMe->m_eIconState)
  {
  case QCAM_ICONS_NO:
    Qcamera_ClearStamp(pMe);
    break;
  case QCAM_ICONS_PREVIEW_ONLY:
    if ((pMe->m_nSize < (uint16)QCAM_SIZE_STAMP) && (pMe->m_nSize > (uint16)QCAM_SIZE_VGA))
      Qcamera_AddStampPreview(pMe);
    else
    {
      //Stamp size image is too small or too large to add the icons overlay
      pMe->m_eIconState = QCAM_ICONS_NO;
      Qcamera_ClearStamp(pMe);
    }
    break;
  case QCAM_ICONS_PREVIEW_ENCODE:
    if ((pMe->m_nSize < (uint16)QCAM_SIZE_STAMP) && (pMe->m_nSize > (uint16)QCAM_SIZE_VGA))
      Qcamera_AddStampEncode(pMe);
    else
    {
      //Stamp size image is too small or too large to add the icons overlay
      pMe->m_eIconState = QCAM_ICONS_NO;
      Qcamera_ClearStamp(pMe);
    }
    break;
  default:
    break;
  }
}
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY

/*===========================================================================
FUNCTION Qcamera_CameraPreview

  DESCRIPTION
    This function starts the viewfinder and update the preview screen.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_CameraPreview(QCam *pMe)
{
  int nRet = SUCCESS;

  if (!pMe)
    return;

  MSG_MED("Entering preview mode", 0, 0, 0);
  pMe->m_eState = QCAM_STATE_START_PREVIEW;
  pMe->m_activeMenu = QCAM_MENU_NONE;
  IMENUCTL_Reset(pMe->m_pIMenu);

  //In case of changing frame or application start up, need to reload the frame image.
  if (pMe->m_bUpdatePreviewFrame)
  {
    pMe->m_bUpdatePreviewFrame = FALSE;
    //first clear overlay if we need to add another one.
    (void)ICAMERA_ClearOverlay(pMe->m_pICam);

    if (pMe->m_sFrameFileName[0] != '\0')
    {
      //Load the frame DIB with frame file name.
#ifdef FEATURE_CAMERA_PANORAMIC      
      pMe->m_nPanoramic = 0;    
      pMe->m_nProperties &= ~QCAM_PANORAMIC_EFFECT;
      (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PANORAMIC,  pMe->m_nPanoramic, 0);
#endif //FEATURE_CAMERA_PANORAMIC	  
      if (!Qcamera_LoadFrameOverlay(pMe))
      {
        MSG_HIGH("Qcamera_LoadFrameOverlay error.", 0, 0, 0);
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
        Qcamera_AddRemoveIconsStamps(pMe);
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
        nRet = ICAMERA_Preview(pMe->m_pICam);
      }
    }
    else
    {
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
      Qcamera_AddRemoveIconsStamps(pMe);
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
      nRet = ICAMERA_Preview(pMe->m_pICam);
    }
  }
  else
  {
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
    Qcamera_AddRemoveIconsStamps(pMe);
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
    nRet = ICAMERA_Preview(pMe->m_pICam);
  }

  if (nRet != SUCCESS)
  {
    MSG_HIGH("ICAMERA_Preview failed.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    return;
  }
  // Update the zoom parameters which are calculated in the
  // preview command by the camera driver.
  Qcamera_InitVals(pMe, CAM_PARM_ZOOM, 1, &pMe->m_nZoom,
                   &pMe->m_nZoomMin, &pMe->m_nZoomMax, &pMe->m_nZoomStep);

  //Make sure clear screen happen before mdp
  //deregister the full screen. Otherwise,
  //the old foreground cannot be clean up.
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);

#ifdef FEATURE_OVERLAY
  Qcamera_MdpRegisterPreview(pMe);
#endif // FEATURE_OVERLAY
}

/*===========================================================================
FUNCTION Qcamera_LoadFrameOverlay

  DESCRIPTION
    This function loads the frame image and call the Qcamera_FrameImageNotify
    to handle the overlay.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    TRUE if image is loaded.
===========================================================================*/
static boolean Qcamera_LoadFrameOverlay(QCam *pMe)
{
  if (!pMe)
    return FALSE;

  pMe->m_pFrameImg = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->m_sFrameFileName);
  if (pMe->m_pFrameImg)
  {
    IIMAGE_Notify(pMe->m_pFrameImg, Qcamera_FrameImageNotify, pMe);
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION Qcamera_DeleteAll

  DESCRIPTION
    This function deletes all photo directories and files.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    TRUE if image is loaded.
===========================================================================*/
static boolean Qcamera_DeleteAll(QCam *pMe, uint16 id)
{
  boolean result=TRUE;
  SearchFlag flag = LOC_ALL;

  if (!pMe)
    return FALSE;

  switch(id)
  {
  case IDS_YES_FLASH:
    flag = LOC_FLASH;
    break;
  case IDS_YES_MMC:
    flag = LOC_MMC;
    break;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  case IDS_YES_USB:
	flag = LOC_USB;
	break;  
#endif
  case IDS_YES_FTL:
    flag = LOC_FTL;
    break;
  case IDS_YES_RAW:
    flag = LOC_RAW;
    break;
#if defined (FEATURE_CAMERA_ENCODE_YCBCR)
  case IDS_YES_YCBCR:
    flag = LOC_YCBCR;
    break;
#endif // FEATURE_CAMERA_ENCODE_YCBCR
  default:
    break;
  }

  if (!Qcam_DeleteDirs(pMe->m_pIFileMgr, flag))
  {
    //At least one file is not deleted, return FALSE.
    result = FALSE;
  }

  //reset this number so create new file name start from 1 again.
  pMe->m_wCurrentImage   = 0;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

  return result;
}

/*===========================================================================
FUNCTION Qcamera_DeleteAllFilesResponse

  DESCRIPTION
    This function handles the delete all selection and update the screen.

  PARAMETERS:
    pMe - Pointer to  QCam struct
    selId - User selection id (IDS_NO or IDS_YES_DEL)

  RETURN VALUE:
    NONE.
===========================================================================*/
static void Qcamera_DeleteAllFilesResponse(QCam *pMe, uint16 selId)
{
  if (!pMe)
    return;

  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
  if (selId != IDS_NO)
  {
    if (Qcamera_DeleteAll(pMe, pMe->m_nDelOpt))
    {
      //Deleted all files, display the erased message
      Qcamera_DisplayTimedImg(pMe, NULL, (uint16)IDB_ERASED);
    }
    else
    {
      //At least one file is not deleted, display the failure message.
      Qcamera_DisplayTimedMsg(pMe, NULL, (uint16)IDS_MSG_FAIL_DELFILES);
    }
    //reset the seletion
    pMe->m_nDelOpt = IDS_NO;
  }
  else
  {
    (void) IMENUCTL_Redraw(pMe->m_pIMenu);
  }
  (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  IDISPLAY_Update(pMe->a.m_pIDisplay);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
}

/*===========================================================================
FUNCTION Qcamera_GetTitleText

  DESCRIPTION
    This function gets the title text string from item id.

  PARAMETERS:
    pMe - Pointer to  QCam struct
    itemID - item id
    txtBuf - text buffer to hold the title string

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_GetTitleText(QCam *pMe, uint16 itemID, AECHAR *txtBuf, int size)
{
  char temp[20] = {0};

  if (!pMe || !txtBuf)
    return;

  //Load the id string first
  (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, itemID, txtBuf, size);

  //Get the current setting over total steps
  switch (itemID)
  {
  case IDS_BRIGHTNESS:
    (void)SNPRINTF(temp, sizeof(temp), "(%d/%d)",
                  (pMe->m_nBright - pMe->m_nBrightnessMin)/pMe->m_nBrightnessStep,
                  (pMe->m_nBrightnessMax - pMe->m_nBrightnessMin)/pMe->m_nBrightnessStep);
    break;

  case IDS_CONTRAST:
    if (pMe->di.cxScreen >= 240) //QVGA LCD
      (void)SNPRINTF(temp, sizeof(temp), "(%d/%d)",
                    (pMe->m_nContrast - pMe->m_nContrastMin)/pMe->m_nContrastStep,
                    (pMe->m_nContrastMax - pMe->m_nContrastMin)/pMe->m_nContrastStep);
    else
      (void)SNPRINTF(temp, sizeof(temp), " %d/%d",
                    (pMe->m_nContrast - pMe->m_nContrastMin)/pMe->m_nContrastStep,
                    (pMe->m_nContrastMax - pMe->m_nContrastMin)/pMe->m_nContrastStep);
    break;

  case IDS_SHARPNESS:
    if (pMe->di.cxScreen >= 240) //QVGA LCD
      (void)SNPRINTF(temp, sizeof(temp), "(%d/%d)",
                    (pMe->m_nSharpness - pMe->m_nSharpnessMin)/pMe->m_nSharpnessStep,
                    (pMe->m_nSharpnessMax - pMe->m_nSharpnessMin)/pMe->m_nSharpnessStep);
    else
      (void)SNPRINTF(temp, sizeof(temp), " %d/%d",
                    (pMe->m_nSharpness - pMe->m_nSharpnessMin)/pMe->m_nSharpnessStep,
                    (pMe->m_nSharpnessMax - pMe->m_nSharpnessMin)/pMe->m_nSharpnessStep);
    break;

#ifdef FEATURE_CAMERA7500
  case IDS_HUE:
    (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_HUE, &pMe->m_nHue, NULL);  	
    (void)SNPRINTF(temp, sizeof(temp), "(%d/%d)",
                  (pMe->m_nHue - pMe->m_nHueMin)/pMe->m_nHueStep,
                  (pMe->m_nHueMax - pMe->m_nHueMin)/pMe->m_nHueStep);
    break;

  case IDS_SATURATION:
    (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_SATURATION, &pMe->m_nSaturation, NULL);    	
    (void)SNPRINTF(temp, sizeof(temp), "(%d/%d)",
                  (pMe->m_nSaturation - pMe->m_nSaturationMin)/pMe->m_nSaturationStep,
                  (pMe->m_nSaturationMax - pMe->m_nSaturationMin)/pMe->m_nSaturationStep);
    break;
#endif //FEATURE_CAMERA7500

  case IDS_ZOOM:
  case IDS_ZOOM_OPTICAL:
  case IDS_ZOOM_DIGITAL:
    (void)SNPRINTF(temp, sizeof(temp), "(%d/%d)", (pMe->m_nZoom - pMe->m_nZoomMin) + 1,
                  (pMe->m_nZoomMax - pMe->m_nZoomMin) + 1);
    break;

  case IDS_EV:
    (void)SNPRINTF(temp, sizeof(temp), " %d (%d, %d)", pMe->m_nExposure,
                  pMe->m_nExposureMin, pMe->m_nExposureMax);
    break;

  default:
    break;
  }

  //Combine two string together.
  (void)STRTOWSTR(temp, &txtBuf[WSTRLEN(txtBuf)], size - WSTRLEN(txtBuf));
}

/*===========================================================================
FUNCTION Qcamera_UpdateAFIndicator

  DESCRIPTION
    This function displays the Focus status indicator.

  PARAMETERS:
    pMe     - Pointer to  QCam struct
    iconID  - Focus status icon id

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_UpdateAFIndicator(QCam *pMe, uint16 iconID)
{
  AEERect rc;
  IImage *pImage = NULL;

  if (!pMe)
    return;

  SETAEERECT(&rc, pMe->di.cxScreen - 11, 2, 11, 11);
  pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, iconID);
  if (pImage)
  {
    QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, rc);
  }
}

/*===========================================================================
FUNCTION Qcamera_ResetAFIndicator

  DESCRIPTION
    This function reset the Focus status indicator to No Focus.

  PARAMETERS:
    pMe     - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_ResetAFIndicator(QCam *pMe)
{
  //Only update the indicator while the Focus menu item is selected.
  if (!pMe)
    return;

  //reset the flag
  pMe->m_bFocusStarted = FALSE;
  pMe->m_bOneButtonSnapshot = FALSE;

#if defined (FEATURE_MMOVERLAY) && defined (FEATURE_CAMERA7500)
  if (pMe->m_nRectcount > 0)
  {
    Qcamera_ClearAFRects(pMe);
  }
#endif //FEATURE_MMOVERLAY && FEATURE_CAMERA7500

  Qcamera_UpdateAFIndicator(pMe, IDB_AF_NONE);
}

/*===========================================================================
FUNCTION Qcamera_StopIfAFIndicator

  DESCRIPTION
    This function cancel the Focus status indicator update.

  PARAMETERS:
    pMe     - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_StopIfAFIndicator(QCam *pMe)
{
  if (!pMe)
    return;

  //Cancel timer and reset related variables
  (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)Qcamera_ResetAFIndicator, pMe);
  Qcamera_ResetAFIndicator(pMe);
}

/*===========================================================================
FUNCTION Qcamera_DisplayInfo

  DESCRIPTION
    This function display extra info (resolution, remain photo #) in the
    second title bar.
    The function does not call IDisplay_UpdateEx. Need to call update if
    not called with display menu name.

  PARAMETERS:
    pMe     - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_DisplayInfo(QCam *pMe)
{
  AEERect rc;
  char str[TEXT_STRING_SIZE] = {0};
  AECHAR szBuf[TEXT_STRING_SIZE] = {0};
  IImage *pImage = NULL;
  boolean bMMC;
  boolean bFTL;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  boolean bUSB;
#endif
  uint16 id;
  int ret;
  int nPixles = 0;
  AEEImageInfo info = {0};


  if (!pMe)
    return;

  if (pMe->m_bSmallScreen)
    return;

  bMMC = ((pMe->m_saveLoc == QCAM_SAVE_MMC) &&
          (SUCCESS == IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_CARD0_DIR)));
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  bUSB = ((pMe->m_saveLoc == QCAM_SAVE_USB) &&
          (SUCCESS == IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_MOD_USB_DIR)));
#endif
  bFTL = (pMe->m_bFTL && (pMe->m_saveLoc == QCAM_SAVE_FTL));
  id = Qcamera_GetSizeStrID(pMe);
  ret = Qcamera_GetRemainPicNum(pMe, bMMC, bFTL 
          #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
            , bUSB 
          #endif
          );
  SETAEERECT(&rc, 0, pMe->m_nTitleHeight, pMe->di.cxScreen, pMe->m_nTitleHeight);
   //Clear title area
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);

  //Display resolution string
  if (id > 0)
  {
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, id, szBuf, sizeof(szBuf));
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szBuf, -1, 2, pMe->m_nTitleHeight + 2, &rc, 0);
    /* Get  the width of a given text string szBuf */	
    nPixles = IDISPLAY_MeasureText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szBuf);	
  }

  //Display remain photo number
  (void)SNPRINTF(str, sizeof(str), "%d", ret);
  (void)STRTOWSTR(str, szBuf, sizeof(szBuf));
//  rc.x = 120;
  (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szBuf, -1, 0, pMe->m_nTitleHeight + 2, &rc, IDF_ALIGN_RIGHT);

#ifdef FEATURE_CAMERA_PANORAMIC
  if (pMe->m_nPanoramic == 1)
    id = IDB_PANORAMIC_LANDSCAPE1;
  else
#endif //FEATURE_CAMERA_PANORAMIC
  //Display flash/redeye reduction state indicator
  if (pMe->m_bRedeyeReduce)
    id = IDB_REDEYE_NO;
  else if (pMe->m_nFlash == CAM_FLASH_OFF)
    id = IDB_FLASH_OFF;
  else if (pMe->m_nFlash == CAM_FLASH_AUTO)
    id = IDB_FLASH_AUTO;
  else
    id = IDB_FLASH_ON;

  //This x setting is applied to QVGA screen
  pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, id);
  if (pImage)
    IIMAGE_GetInfo(pImage, &info);
  /*Set the rc according to the Width and Height of the Image */
  SETAEERECT(&rc, nPixles + 20, pMe->m_nTitleHeight * 2 - info.cy, info.cx, info.cy);
  if (pImage)
  {
    QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, rc);
  }

  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
}
/*====================================================
FUNCTION Qcamera_DisplayTitle

  DESCRIPTION
    This function displays the title with provided image and text ids.

  PARAMETERS:
    pMe     - Pointer to  QCam struct
    titleID - selection item text id
    img1ID  - image id for the first icon in the title
    txt1Str - text buffer for the first string in title
    img2ID  - image id for the second icon in the title
    txt2ID  - text id for the second string in the title

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_DisplayTitle(QCam *pMe, uint16 titleID,
                                 uint16 img1ID, const AECHAR *txt1Str,
                                 uint16 img2ID, uint16 txt2ID)
{
  IImage *pImage = NULL;
  AECHAR szBuf[TEXT_STRING_SIZE] = {0};
  AEERect rc;
  int16 titlePos  = QCAM_FIRST_POS;
  int16 image1Pos = QCAM_SECOND_POS;
  int16 image2Pos = QCAM_THIRD_POS;

  if (!pMe)
    return;

#ifdef FEATURE_CAMERA7500
    /* Check the Display is VGA and double the postions */
  if(pMe->di.cxScreen >= QCAM_VGA_WIDTH)
  {
     image1Pos = 2 * QCAM_SECOND_POS;
     image2Pos = 2 * QCAM_THIRD_POS;
  }
#endif  

  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight);
   //Clear title area
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);

  //Draw the title
  if (titleID > 0)
  {
    rc.x = titlePos;
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, titleID, szBuf, sizeof(szBuf));
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szBuf, -1, 0, 1, &rc, IDF_ALIGN_LEFT);
    ZEROAT(szBuf);
  }
  else
    image1Pos = titlePos;

  //Draw the first image/string
  rc.x = image1Pos;
  if (img1ID)
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, img1ID);

  QCamDisp_DrawMenuTitle(pMe->a.m_pIDisplay, rc, pImage, txt1Str);
  pImage = NULL;

  if (!pMe->m_bSmallScreen)
  {
    //Draw the second title if it is available
    rc.x = image2Pos;
    if (img2ID)
      pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, img2ID);
    if (txt2ID)
    {
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, (uint16)txt2ID, szBuf, sizeof(szBuf));
    }

    QCamDisp_DrawMenuTitle(pMe->a.m_pIDisplay, rc, pImage, szBuf);
  }

  //draw focus state icon
  if (titleID == IDS_FOCUS)
  {
    //Update screen through this function.
    Qcamera_UpdateAFIndicator(pMe, IDB_AF_NONE);
  }
  else
  {
    //update screen directly
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
  }
}

/*===========================================================================
FUNCTION Qcamera_DisplayTakePicMenuName

  DESCRIPTION
    This function updates the take picture menu title based on the selection item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    item - move direction, -1, 0, 1

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_DisplayTakePicMenuName(QCam *pMe, int item)
{
  AECHAR szBuf[TEXT_STRING_SIZE] = {0};
  int index;
  int count;
  uint16  selection = 0;

  if (!pMe)
    return;

  //Find the new selected item
  count = IMENUCTL_GetItemCount(pMe->m_pISoftMenu);
  if (!IMENUCTL_GetItemData(pMe->m_pISoftMenu, pMe->m_wTakePicMenuSel, (uint32 *)&index))
  {
    MSG_HIGH("Invaild menu id %d, reset index", pMe->m_wTakePicMenuSel, 0, 0);
  }
  index += item;
  if (index < 0)
  {
    index = count - 1;
  }
  else if (index >= count)
  {
    index = 0;
  }

  selection = IMENUCTL_GetItemID(pMe->m_pISoftMenu, index);
  pMe->m_wTakePicMenuSel = selection;

  //Update the item title
  switch (selection)
  {
  case IDS_ALBUM:
  case IDS_SLIDESHOW:
  case IDS_SETTINGS:
  case IDS_FRAME:
#ifdef FEATURE_APP_QCAMCORDER
  case IDS_QCAMCORDER:
#endif //FEATURE_APP_QCAMCORDER
#ifdef FEATURE_CAMERA_MOBICAT
  case IDS_MCT:
#endif //FEATURE_CAMERA_MOBICAT
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, selection, szBuf, sizeof(szBuf));
    Qcamera_DisplayTitle(pMe, 0, IDB_SELECT, szBuf, 0, 0);
    break;

  case IDS_GPS_SETUP:
    {
      uint16 id = Qcamera_GetGPSid(pMe);
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile,
                                 id, szBuf, sizeof(szBuf));
    }
    Qcamera_DisplayTitle(pMe, 0, IDB_SELECT, szBuf, 0, 0);
    break;

  case IDS_ZOOM:
    if (!pMe->m_bSmallScreen)
      selection = (pMe->m_nZoom < 0) ? IDS_ZOOM_OPTICAL : IDS_ZOOM_DIGITAL;
    //lint -fallthrough
  case IDS_BRIGHTNESS:
  case IDS_CONTRAST:
  case IDS_SHARPNESS:
  case IDS_EV:
#ifdef FEATURE_CAMERA7500
  case IDS_HUE:
  case IDS_SATURATION:
#endif //FEATURE_CAMERA7500
    Qcamera_GetTitleText(pMe, selection, szBuf, sizeof(szBuf));
    Qcamera_DisplayTitle(pMe, 0, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
    break;

#ifdef FEATURE_CAMERA7500
  case IDS_EFFECTS:
    {
      uint32 val;
      uint16 sel; 	  
      (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_EFFECT, (int32 *)(&pMe->m_nEffect), NULL);
      val = pMe->m_nEffect;
      sel = QcamMenus_GetItemDataPerStep(IDS_EFFECTS, 0, &val);
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile,
                                 sel, szBuf, sizeof(szBuf));

      Qcamera_DisplayTitle(pMe, IDS_EFFECTS, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
    }
    break;
#endif //FEATURE_CAMERA7500

#ifdef FEATURE_CAMERA_HISTOGRAM
  case IDS_HISTOGRAM:
    {
      uint32 val = pMe->m_nPrvwHistogram;
      uint16 sel = QcamMenus_GetItemDataPerStep(IDS_HISTOGRAM, 0, &val);
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile,
                                 sel, szBuf, sizeof(szBuf));
      Qcamera_DisplayTitle(pMe, IDS_HISTOGRAM, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_SELECT);
    }
    break;
#endif //FEATURE_CAMERA_HISTOGRAM

  case IDS_FOCUS:
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, IDS_AUTO, szBuf, sizeof(szBuf));
    Qcamera_DisplayTitle(pMe, IDS_FOCUS, IDB_ZERO, szBuf, IDB_UP_DOWN, IDS_MANUAL);
    break;
  case IDS_TAKE_PIC:
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, IDS_ORIENTATION, szBuf, sizeof(szBuf));
    Qcamera_DisplayTitle(pMe, 0, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
    break;
#ifndef FEATURE_CAMERA_V2
  case IDS_SIMULATE_2NDLCD:
    selection = pMe->m_b2ndDisplay ? IDS_DISP_1ST : IDS_DISP_2ND;
    Qcamera_GetTitleText(pMe, IDS_SIMULATE_2NDLCD, szBuf, sizeof(szBuf));
    Qcamera_DisplayTitle(pMe, 0, IDB_SELECT, szBuf, 0, selection);
    break;
#endif //FEATURE_CAMERA_V2

  default:
    break;
  }
}

/*===========================================================================
FUNCTION Qcamera_DisplaySelfTimerBar

  DESCRIPTION
    This function updates the self timer bar and with left seconds

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_DisplaySelfTimerBar(QCam *pMe)
{
  AEERect bar;
  AEERect rc;
  AECHAR szBuf[TEXT_STRING_SIZE] = {0};
  AECHAR acBuf[TEXT_STRING_SIZE] = {0};
  int secLenght;

  if (!pMe)
    return;

  //Get display rect and bar rect
  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight);
  SETAEERECT(&bar, 88, 4, (pMe->di.cxScreen - 93), 5);

  //clear the rect and do offset
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
  rc.x = 2;

  //Get and draw left second text
  if (ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, IDS_TIME_OUT, acBuf, sizeof(acBuf)))
  {
    WSPRINTF(szBuf, sizeof(szBuf), acBuf, pMe->m_nLeftSec);
    (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szBuf, -1, 0, 1, &rc, IDF_ALIGN_LEFT);
  }

  //Update the timer bar with left second
  (void)IDISPLAY_DrawFrame(pMe->a.m_pIDisplay, &bar, AEE_FT_INDENT, CLR_SYS_SCROLLBAR);

  secLenght = bar.dx / pMe->m_nSelfTimerDelay;
  bar.dx = (int16)(pMe->m_nLeftSec * secLenght);

  IDISPLAY_FillRect(pMe->a.m_pIDisplay, &bar, MAKE_RGB(0, 0, 0xFF));
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);

  //Call timer callback function to check left second.
  Qcamera_selfTimerCB(pMe);
}

/*===========================================================================
FUNCTION Qcamera_selfTimerCB

  DESCRIPTION
    This function checks if these are left seconds.
    It calls the Qcamera_DisplaySelfTimerBar again to update the timer bar if left second.
    Otherwise start take picture process

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_selfTimerCB(QCam *pMe)
{
  if (!pMe)
    return;

  if (pMe->m_nLeftSec)
  {
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, TIMEOUT_1000, (PFNNOTIFY)Qcamera_DisplaySelfTimerBar,(void *)pMe);
    pMe->m_nLeftSec--;
  }
  else
  {
    Qcamera_StartFocus(pMe);
  }
}

/*===========================================================================
FUNCTION Qcamera_PromptForDelAll

  DESCRIPTION
    This function fills the AEEPromptInfo struct and display the delete all
    prompt screen by calling ISHELL_Prompt.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    TRUE - If Prompt message is display
    FALSE - If failed to prompt the message.
===========================================================================*/
boolean Qcamera_PromptForDelAll(QCam *pMe)
{
  uint16 nResID;
  AECHAR acBuf[PROMPT_SIZE];
  AECHAR szBuf[PROMPT_SIZE] = {0};
  AEEPromptInfo pi = {0};
  uint16 buttonIDs[3] = {IDS_YES_DEL, IDS_NO, 0};

  if (!pMe)
    return FALSE;

  pi.pszRes   = pMe->m_pResFile;
  pi.wTitleID = IDS_DELETE_FILE_TITLE;
  pi.fntTitle = QCAM_FONT_TITLE;
  pi.fntText  = QCAM_FONT_TEXT;
  pi.wDefBtn  = IDS_YES;
  pi.pBtnIDs  = buttonIDs;

  //Get the message id
  switch (pMe->m_nDelOpt)
  {
  case IDS_YES_FLASH:
    nResID = IDS_ALL_FILES_FLASH;
    break;
  case IDS_YES_MMC:
    nResID = IDS_ALL_FILES_MMC;
    break;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  case IDS_YES_USB:
    nResID = IDS_ALL_FILES_USB;
    break;
#endif
  case IDS_YES_FTL:
    nResID = IDS_ALL_FILES_FTL;
    break;
  case IDS_YES_RAW:
    nResID = IDS_ALL_FILES_RAW;
    break;
#ifdef FEATURE_CAMERA7500
  case IDS_YES_YCBCR:
    nResID = IDS_ALL_FILES_YCBCR;
    break;
#endif //FEATURE_CAMERA7500
  default:
    nResID = IDS_ALL_FILES;
    break;
  }
  //Get the prompt message
  if(!ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile,
                       IDS_DELETE_FILE_PROMPT, szBuf, sizeof(szBuf)))
  {
    MSG_HIGH("Resource String couldn't be loaded",0,0,0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    return FALSE;
  }
  else
  {
    (void)STRTOWSTR("\n", acBuf, sizeof(acBuf));
    (void)WSTRLCAT(szBuf, acBuf, sizeof(szBuf));
  }

  if(!ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile,
                           nResID, acBuf, sizeof(acBuf)))
  {
    MSG_HIGH("Resource String couldn't be loaded",0,0,0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    return FALSE;
  }
  else
  {
    (void)WSTRLCAT(szBuf, acBuf, sizeof(szBuf));
    (void)STRTOWSTR("?", acBuf, sizeof(acBuf));
    (void)WSTRLCAT(szBuf, acBuf, sizeof(szBuf));
  }

  pi.pText = szBuf;

  //clear old screen and display prompt screen
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);

  if (!ISHELL_Prompt(pMe->a.m_pIShell, &pi))
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
     Qcamera_DeleteAllFilesResponse(pMe, IDS_NO);
     return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION Qcamera_DisplaySavedImageName

  DESCRIPTION
    This function displays the saved image name on the title bar.


  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_DisplaySavedImageName(QCam *pMe)
{
  AEERect textRC;
  AECHAR szText[FILE_NAME_SIZE];
  uint32 dirLen;
  uint32 dirLen2;

  if (!pMe)
    return;

  SETAEERECT(&textRC, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight);
  Qcamera_GetDirLen(pMe, &dirLen, &dirLen2);

  // Display the date folder and name of the image
  (void)STRTOWSTR(&pMe->m_sCurrentFileName[dirLen], szText, sizeof(szText));
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &textRC);
  (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText,
                   -1, 0, 0, &textRC, IDF_ALIGN_LEFT);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
}

/*===========================================================================
FUNCTION Qcamera_DisplayPhotoSavedImage

  DESCRIPTION
    This function displays the photo saved message on the screen after image is
    saved for prompt save option.


  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_DisplayPhotoSavedImage(QCam *pMe)
{
  if (!pMe)
    return;

  //Display photo saved icon image for prompt to save option
  if (pMe->m_bSaveAppPic)
  {
    pMe->m_bSaveAppPic = FALSE;
#ifdef FEATURE_MMOVERLAY
    if (pMe->m_pIMMOverlayFull)
      (void)IMMOVERLAY_SetParam(pMe->m_pIMMOverlayFull, OV_PARM_TRANSPARENCY, 0, 0);
#endif
    Qcamera_DisplayTimedImg(pMe,
                            (PFNNOTIFY)Qcamera_TurnOnCameraPreview,
                            (uint16)IDB_PHOTO_SAVED);
    //Display the saved image name on the title bar.
    Qcamera_DisplaySavedImageName(pMe);
  }
  else
  {
    Qcamera_TurnOnCameraPreview(pMe);
  }
}

/*===========================================================================
FUNCTION Qcamera_DrawBusyImage

  DESCRIPTION
    This function draws the busy image which is an hour glass.

  PARAMETERS:
    pMe - Pointer to  Qalbum struct
    x   - x coordinate
    y   - y coordinate

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_DrawBusyImage(QCam *pMe, int x, int y)
{
  IImage * pIImage;

  if (!pMe)
    return;

  // load hourglass
  pIImage = ISHELL_LoadBusyImage(pMe->a.m_pIShell);
  // draw it
  if (pIImage)
  {
    IIMAGE_Draw(pIImage, x, y);
    CAM_RELEASEIF(pIImage);
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
  }
}

/*===========================================================================
FUNCTION Qcamera_MemUsageBlock

  DESCRIPTION
    This function display the selected memory usage information. It includes:
    total photos can take in selected encoding and quality type,
    available space, used space, percentage of memory usage.

  PARAMETERS:
    pMe - Pointer to  QCam struct
    bMMC - Flag for memory type

  RETURN VALUE:
    None

===========================================================================*/
static void Qcamera_MemUsageBlock(QCam *pMe, boolean bMMC, boolean bFTL 
                #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
                  , boolean bUSB 
                #endif
  )
{
  char   szBuf[TEXT_STRING_SIZE] = {0};
  char   szText[512] = {0};
  uint32 dwFreeSpace = 0;
  uint32 dwTotalFileSystemSize = 1;
  uint32 picNum = 0;
  uint32 perct = 0;
  const char *savePath;

  if (!pMe)
    return;

  if (bMMC)
  {
    (void)STRLCPY(szText, "\n[MMC MEMORY USAGE]\n", sizeof(szText));
    savePath = AEEFS_CARD0_DIR;
  }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  else if (bUSB)
  {
    (void)STRLCPY(szText, "\n[USB MEMORY USAGE]\n", sizeof(szText));
    savePath = AEEFS_MOD_USB_DIR;
  }
#endif
  else if (bFTL)
  {
    (void)STRLCPY(szText, "\n[FTL MEMORY USAGE]\n", sizeof(szText));
    savePath = AEEFS_FTL_DIR;
  }
  else
  {
    (void)STRLCPY(szText, "\n[FLASH MEMORY USAGE]\n", sizeof(szText));
    savePath = AEEFS_ROOT_DIR;
  }
  picNum = (uint32)Qcamera_GetRemainPicNum(pMe, bMMC, bFTL 
             #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
               , bUSB 
             #endif
              );
  // Display header for memory usage block
  if(SUCCESS != IFILEMGR_GetFreeSpaceEx(pMe->m_pIFileMgr,
                                        savePath,
                                        &dwTotalFileSystemSize,
                                        &dwFreeSpace))
  {
    if (bMMC || bFTL 
      #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
        || bUSB 
      #endif
      )
      dwFreeSpace = 0;
    else
      dwFreeSpace = IFILEMGR_GetFreeSpace(pMe->m_pIFileMgr, &dwTotalFileSystemSize);
  }

  //Convert the return value from Byte to KB
  dwFreeSpace /= 1024;
  dwTotalFileSystemSize /= 1024;
  if (dwTotalFileSystemSize == 0)
    dwTotalFileSystemSize = 1;

  //Calculate size and percentage
  perct = (((dwTotalFileSystemSize - dwFreeSpace ) * 100) / dwTotalFileSystemSize);

  // Display number of photo can be taken by file format
  (void)STRLCAT(szText, "You can take approximately:\n", sizeof(szText));
  (void)SNPRINTF(szBuf, sizeof(szBuf), " %u more photos\n", picNum);
  (void)STRLCAT(szText, szBuf, sizeof(szText));

  // Display storage info
  (void)STRLCAT(szText, "File Space Avail/Total [KB]:\n", sizeof(szText));
  (void)SNPRINTF(szBuf, sizeof(szBuf), "  %u / %u\n", dwFreeSpace, dwTotalFileSystemSize);
  (void)STRLCAT(szText, szBuf, sizeof(szText));

  // Display memory usage percentage
  (void)SNPRINTF(szBuf, sizeof(szBuf), "Memory Usage:  %u %%\n", perct);
  (void)STRLCAT(szText, szBuf, sizeof(szText));
  (void)ISTATIC_SetTextEx(pMe->m_pIStatic, (byte *)szText, NULL, TRUE);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/*===========================================================================
FUNCTION Qcamera_MemUsage

  DESCRIPTION
    This function display the memory usage information. It includes:
    total photos have taken, total photos can take in selected encoding and quality type,
    available space, used space, percentage of memory usage.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_MemUsage(QCam *pMe)
{
  AEERect rc;
  boolean bMMCavail = FALSE;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  boolean bUSBavail = FALSE;
#endif
  uint32 picNum = 0;
  uint32 rawNum = 0;
#ifdef FEATURE_CAMERA7500
  uint32 ycbcrNum = 0;
#endif //FEATURE_CAMERA7500
  AECHAR wsTitle[TEXT_STRING_SIZE] = {0};
  char   szBuf[TEXT_STRING_SIZE] = {0};
  char   szText[512] = {0};

  if (!pMe)
    return;

  //It is a long processing to check if MMC is available, draw a hour glass.
  Qcamera_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

  MSG_MED("Start QCam_GetTotalPhotos.", 0, 0, 0);
  picNum = QCam_GetTotalPhotos(pMe->m_pIFileMgr, FALSE, LOC_ALL);
  MSG_MED("picNum %d", picNum, 0, 0);
  rawNum = QCam_GetTotalPhotos(pMe->m_pIFileMgr, TRUE, LOC_RAW);
  MSG_MED("End QCam_GetTotalPhotos. rawNum %d", rawNum, 0, 0);
#ifdef FEATURE_CAMERA7500
  ycbcrNum = QCam_GetTotalPhotos(pMe->m_pIFileMgr, TRUE, LOC_YCBCR);
  MSG_MED("End QCam_GetTotalPhotos. ycbcrNum %d", ycbcrNum, 0, 0);
#endif //FEATURE_CAMERA7500

  IMENUCTL_Reset(pMe->m_pIMenu);
  SETAEERECT(&rc, 0, pMe->m_nTitleHeight, pMe->di.cxScreen, pMe->di.cyScreen - pMe->m_nTitleHeight);
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);

  //Check if MMC card is available
  MSG_MED("Start MMC IFILEMGR_Test.", 0, 0, 0);
  if(SUCCESS == IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_CARD0_DIR))
    bMMCavail = TRUE;
  MSG_MED("Stop MMC IFILEMGR_Test.", 0, 0, 0);

#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
//check if USB Mass storage device is available
  MSG_MED("Start USB IFILEMGR_Test.", 0, 0, 0);
  if(SUCCESS == IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_MOD_USB_DIR))
    bUSBavail = TRUE;
  MSG_MED("Stop USB IFILEMGR_Test.", 0, 0, 0);
#endif

  // Create a IStatic interface
  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC,
                            (void**)&pMe->m_pIStatic) != SUCCESS)
  {
    MSG_HIGH("Couldn't create instance for AEECLSID_STATIC.", 0, 0, 0);
    return;
  }

  // Set control rect
  SETAEERECT(&rc, QCAM_TXT_X_START, pMe->m_nTitleHeight + 2,
             pMe->di.cxScreen - QCAM_TXT_X_START,
             pMe->di.cyScreen - (pMe->m_nTitleHeight + 2));

  ISTATIC_SetRect(pMe->m_pIStatic, &rc);

  // Specify control style
  ISTATIC_SetProperties(pMe->m_pIStatic,
                        (ST_CENTERTITLE | ST_UNDERLINE | ST_ASCII | ST_NOSCROLL));

  // Display current used memory
  if (bMMCavail && (pMe->m_saveLoc == QCAM_SAVE_MMC))
    (void)STRTOWSTR("Save Location: MMC", wsTitle, sizeof(wsTitle));
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  else if (bUSBavail && (pMe->m_saveLoc == QCAM_SAVE_USB))
    (void)STRTOWSTR("Save Location: USB", wsTitle, sizeof(wsTitle));
#endif
  else if (pMe->m_bFTL && (pMe->m_saveLoc == QCAM_SAVE_FTL))
    (void)STRTOWSTR("Save Location: FTL", wsTitle, sizeof(wsTitle));
  else
    (void)STRTOWSTR("Save Location: FLASH", wsTitle, sizeof(wsTitle));

  (void)ISTATIC_SetText(pMe->m_pIStatic,
                        wsTitle,
                        (AECHAR *)"",
                        QCAM_FONT_TITLE,
                        QCAM_FONT_TITLE);

  // Display number of photo already took
  (void)STRLCPY(szText, "You have taken:\n", sizeof(szText));
  if (rawNum > 0)
  {
     (void)SNPRINTF(szBuf, sizeof(szBuf), " %u photos and %u raw images\n", picNum, rawNum);
     (void)STRLCAT(szText, szBuf, sizeof(szText));
  }
#ifdef FEATURE_CAMERA7500
  else if(ycbcrNum > 0)
  {
    (void)SNPRINTF(szBuf, sizeof(szBuf), " %u photos and %u ycbcr images\n", picNum, ycbcrNum);
    (void)STRLCAT(szText, szBuf, sizeof(szText));
  }
#endif //FEATURE_CAMERA7500
  else
  {
    (void)SNPRINTF(szBuf, sizeof(szBuf), " %u photos\n", picNum);
    (void)STRLCAT(szText, szBuf, sizeof(szText));
  }
  (void)ISTATIC_SetTextEx(pMe->m_pIStatic, (byte *)szText, NULL, TRUE);

  // Display Flash memory usage info block
  Qcamera_MemUsageBlock(pMe, FALSE, FALSE 
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
      , FALSE 
#endif
    );

  if (bMMCavail)
  {
    // Display MMC memory usage info block
    Qcamera_MemUsageBlock(pMe, TRUE, FALSE 
      #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
        , FALSE 
      #endif
      );
  }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
  if (bUSBavail)
  {
    // Display USB memory usage info block
    Qcamera_MemUsageBlock(pMe, FALSE, FALSE, TRUE);
  }
#endif

  if (pMe->m_bFTL)
  {
    // Display FTL memory usage info block
    Qcamera_MemUsageBlock(pMe, FALSE, TRUE 
      #ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
        , FALSE 
      #endif
      );
  }

  (void)ISTATIC_Redraw(pMe->m_pIStatic);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);

  pMe->m_activeMenu = QCAM_MENU_MEM_USAGE;
}

#ifndef FEATURE_CAMERA_V2
/*=====================================================================
FUNCTION Qcamera_SetDisplaySize

  DESCRIPTION: This function sets display size based on user option.
               If LCD size display is enabled, QQVGA image could be displayed
               in QVGA size.

  PARAMETERS:
    pMe  - Pointer to  QCam struct

  RETURN VALUE:
    int error code defined in AEEError.h
======================================================================*/
int Qcamera_SetDisplaySize(QCam *pMe, AEESize *pSize)
{
  AEESize dispSize;
#ifndef FEATURE_OVERLAY
  AEESize* pImgRatio;
#endif //FEATURE_OVERLAY

  if (!pMe || !pSize)
    return EFAILED;

  //Get LCD display size.
  dispSize.cx = pMe->m_rcDisp.dx;
  dispSize.cy = pMe->m_rcDisp.dy;

#ifndef FEATURE_OVERLAY
  // Get ImageRatio
  if (pMe->m_sensorInfo.bLandscape)
    pImgRatio = QCam_GetLandscapeImageRatio(pMe->m_nSize);
  else
    pImgRatio = QCam_GetImageRatio(pMe->m_nSize);
#else
  //Add off set back for MDP display on QVGA or VGA screen.
  //We don't know if low layer put any offset or not, so just hard code it to 
  //full size of that screen.
  if (dispSize.cx == 176) //For SC2X QCIF+ screen. No size option for it.
  {
    dispSize.cy = 220;
  }
  else if (QCamDisp_IsSizeScreen(pMe->di.cxScreen, 
                                 (uint16)(pMe->di.cyScreen + QCAM_LCD_OFFSET),
                                 (int)QCAM_SIZE_QVGA))
  {
    dispSize.cy = (dispSize.cy > dispSize.cx) ? 320 : 240;
  }
  else if (QCamDisp_IsSizeScreen(pMe->di.cxScreen, 
                                 (uint16)(pMe->di.cyScreen + QCAM_LCD_OFFSET),
                                 (int)QCAM_SIZE_VGA))
  {
    dispSize.cy = (dispSize.cy > dispSize.cx) ? 640 : 480;
  }
#ifdef FEATURE_CAMERA7500
  else if (pMe->di.cxScreen == QCAM_WVGA_WIDTH)
  {
    dispSize.cy = 480;
  }
#endif //FEATURE_CAMERA7500
  pMe->m_rcDisp.dy = dispSize.cy;
#endif // FEATURE_OVERLAY

#ifdef FEATURE_CAMERA7500
  if (dispSize.cx == QCAM_WVGA_WIDTH)
  {
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#else
    dispSize.cx = QCAM_VGA_WIDTH;
#endif //FEATURE_CAMERA_WIDESIZE
  }
#endif //FEATURE_CAMERA7500

  //If user does not enable LCD size display, display the original size
  //for images with size smaller than the LCD size.
  if (!pMe->m_bDisplayLCDSize)
  {
    // If pSize is smaller than display size, set display to pSize
    if ((pSize->cx < dispSize.cx) && (pSize->cy < dispSize.cy))
    {
      dispSize.cx = pSize->cx;
      dispSize.cy = pSize->cy;
    }
#ifndef FEATURE_OVERLAY
    else if (pSize->cx <= dispSize.cx)
    {
      // We know that our picture cy is larger than display cy
      dispSize = QCamDisp_AdjustDisplaySizeFitY(pImgRatio, dispSize);
    }
    else if (pSize->cy <= dispSize.cy)
    {
      // We know that our picture cx is larger than display cx
      dispSize = QCamDisp_AdjustDisplaySizeFitX(pImgRatio, dispSize);
    }
    else
    {
      // We know that our picture cx and cy is larger than the display
      dispSize = QCamDisp_AdjustDisplaySize(pImgRatio, dispSize);
    }
#endif //FEATURE_OVERLAY
  }
  //Set the new display size
  MSG_MED("Setting displaySize to %d x %d.", dispSize.cx, dispSize.cy, 0);
  return ICAMERA_SetDisplaySize(pMe->m_pICam, &dispSize);
}
#endif //FEATURE_CAMERA_V2

/*********************************************************************************************/
/*****                                                                                   *****/
/*****                      Event Handlers for Camera Application                        *****/
/*****                                                                                   *****/
/*********************************************************************************************/
/*===========================================================================
FUNCTION Qcamera_MapEventHandlers

  DESCRIPTION
    Handler for mapping event to dialog

  PARAMETERS:
    pMe - Pointer to  QCam struct
    pIDialog - Dialog object pointer
    wDlgResID - Dialog id

  RETURN VALUE:
    TRUE if event handled
===========================================================================*/
static boolean Qcamera_MapEventHandlers(QCam *pMe, IDialog *pIDialog, uint16 wDlgResID)
{
  if (!pMe || !pIDialog)
    return FALSE;

  switch (wDlgResID)
  {
  case IDD_SAVE_PIC:
    IDIALOG_SetEventHandler(pIDialog, QCamDlgSave_EventHandler, pMe);
    break;
  case IDD_SET_SIZE:
    IDIALOG_SetEventHandler(pIDialog, QCamDlgSize_EventHandler, pMe);
    break;
#ifndef FEATURE_CAMERA_V2
  case IDD_MULTI_SHOT:
    IDIALOG_SetEventHandler(pIDialog, QCamDlgMultiShot_EventHandler, pMe);
    break;
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif
#endif //FEATURE_CAMERA_V2
  default:
    return FALSE;
  }
  return TRUE;
}


/*===========================================================================
FUNCTION Qcamera_TakePicKeyHandler

  DESCRIPTION
    Key event handler for take picture menu

  PARAMETERS:
    pMe   - Pointer to  QCam struct
    eCode - Event id
    key   - Key id

  RETURN VALUE:
    TRUE if event handled, FALSE if it is not handled
===========================================================================*/
static boolean Qcamera_TakePicKeyHandler(QCam *pMe, AEEEvent eCode, uint16 key)
{
  boolean result = TRUE;

  if (!pMe)
    return FALSE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

  //Hit any key will unlock the message screen and return to preview.
  if (pMe->m_bMsgTimerOn)
  {
    //There are three key events for each key press
    //We only handle EVT_KEY and skip the other two to prevent the side effect
    //from unexpected handling of the key events.
    MSG_MED("Message on, eCode %d, key %d", eCode, key, 0);
    if ((eCode == EVT_KEY) && (key != (uint16)AVK_SELECT))
    {
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
      Qcamera_TurnOnCameraPreview(pMe);
    }
    return TRUE;
  }
  else if (((pMe->m_eState != QCAM_STATE_PREVIEW) && (key != (uint16)AVK_CLR)) ||
	   ((pMe->m_eState != QCAM_STATE_PREVIEW) && (key != (uint16)AVK_END)) || 
           ((pMe->m_bOneButtonSnapshot) && (key != (uint16)AVK_CLR)))
  {
    //Skip any key events that could cause unexpected behavior.
    //Example case: Sel key press before show messge after stop caemra.
    MSG_MED("Current state %d, drop key %d", pMe->m_eState, key, 0);
    return TRUE;
  }

  //If it is still counting time, stop it to cancel the take picture process.
  //The following EVT_KEY handler will set focus on next selected menu item.
  if ((eCode == EVT_KEY_PRESS) && (pMe->m_nLeftSec > 0))
  {
    //Cancel processing and reset flags.
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell,
                             (PFNNOTIFY)Qcamera_DisplaySelfTimerBar,
                             (void *)pMe);
    pMe->m_nLeftSec = 0;
    pMe->m_bCanTakePic = TRUE;
    Qcamera_DisplayTakePicMenuName(pMe, 0);

    return TRUE;
  }

#ifdef FEATURE_CAMERA_MOBICAT
  if (pMe->m_bMobiCaTon)
  {
    camera_tune_was_enabled_at_startup = pMe->m_bMobiCaTon ;
    camera_tune_the_host_brew_app = pMe;
    if ( (eCode == EVT_KEY) &&
         camera_tune_lcd_ui_keypress_handler(key)
       )
    {
      if (camera_tune_return_control_to_app)
      {
        IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rcPreView);
        IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
#ifdef FEATURE_OVERLAY
        Qcamera_MdpRegisterPreview(pMe);
#endif //FEATURE_OVERLAY
        QcamMenus_BuildTakePhotoMenu(pMe);
        camera_tune_return_control_to_app = FALSE;
      }

      return TRUE;    //...meaning "The event was handled successfully.".
    };
  }
#endif //FEATURE_CAMERA_MOBICAT

  if ((eCode == EVT_KEY_PRESS) && (key != (uint16)AVK_0))
  {
    //Cancel the AF timer if it is counting
    Qcamera_StopIfAFIndicator(pMe);
  }

  //General key handler for soft key menu
  switch (key)
  {
#ifdef FEATURE_OVERLAY
  case AVK_STAR:

    if (eCode == EVT_KEY)
    {
      //Toggle menu feature only apply when the menu is overlay on image.
      if (pMe->m_nSize <= (uint16)QCAM_SIZE_QVGA)
      {
        Qcamera_ToggleMenuDisplay(pMe);
      }
      else
      {
        //Toggle grid feature will apply to all cases.
        if (pMe->m_bGridOn)
        {
          pMe->m_bGridOn = FALSE;
          IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rcPreView);
          IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
        }
        else
        {
          Qcamera_AddGrid(pMe);
        }
      }
    }
    break;
#endif //FEATURE_OVERLAY
  case AVK_0:
    if (pMe->m_nFocusMax > 0)
    {
      if (eCode == EVT_KEY)
        Qcamera_OnFocus(pMe, key);
    }
    break;

  case AVK_LEFT:
  case AVK_RIGHT:
    if ((eCode == EVT_KEY) && pMe->m_bShowMenu)
    {
      //Navigate through menu icon, set focus to next item
      result = IMENUCTL_HandleEvent(pMe->m_pISoftMenu, eCode, key, 0);
      if (result)
      {
        IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
        Qcamera_DisplayTakePicMenuName(pMe, (key == (uint16)AVK_LEFT ? -1 : 1));
      }
    }
    break;
  case AVK_SELECT:
    //If key press on the sel key and menu is hide,
    //take a picture, otherwise fall through to handle the menu
    if ((eCode == EVT_KEY_PRESS) && !pMe->m_bShowMenu)
    {
      Qcamera_OnTakePicture(pMe);
      return TRUE;
    }
    //lint -fallthrough
  case AVK_UP:
  case AVK_DOWN:
    if ((eCode == EVT_KEY) && pMe->m_bShowMenu)
    {
      uint32 index;
      uint16 sel = IMENUCTL_GetSel(pMe->m_pISoftMenu);
      //If sel is invalid for some reason, it should fail to get index.
      //ignor the key event in this case.
      if (!IMENUCTL_GetItemData(pMe->m_pISoftMenu, sel, &index))
      {
        MSG_HIGH("Invaild menu id %d", sel, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
        return FALSE;
      }

      //Execute menu on focus
      if (CamMenuKeyHandlers[index].wMenuID == sel)
      {
        CamMenuKeyHandlers[index].MenuOnKeyFunc(pMe, key);
      }
      else
      {
        //incase the item is filtered out dyna
        for (index = 0; index < ARR_SIZE(CamMenuKeyHandlers); index++)
        {
          if (CamMenuKeyHandlers[index].wMenuID == sel)
          {
            CamMenuKeyHandlers[index].MenuOnKeyFunc(pMe, key);
            break;
          }
        }
      }
    }
    break;
  case AVK_END:
  case AVK_CLR:
    if (eCode == EVT_KEY)
    {
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND 
        // Reset the flag in clear key 
        pMe->m_bSnapCaptureDone = FALSE;
        pMe->m_bSnapShutterSndDone = FALSE;
        pMe->m_bNeedToEncodePicture = FALSE;
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND 

      // Stop the camera preview before exiting
      (void)Qcamera_StopCamera(pMe);
      result = IMENUCTL_HandleEvent(pMe->m_pISoftMenu, eCode, key, 0);
    }
    break;

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  case AVK_POUND:
    if (eCode == EVT_KEY && pMe->m_eState == QCAM_STATE_PREVIEW)
    {
      //Switch the camera
      (void)Qcamera_StopCamera(pMe);
      Qcamera_ReleaseCamera(pMe);
      return ISHELL_HandleEvent(pMe->a.m_pIShell, EVT_COMMAND, IDS_TAKE_PIC, (uint32)AVK_POUND);
    }
    break;
#endif //FEATURE_CAMERA_MULTI_SENSOR

  default:
    break;
  }
  return result;
} /* END Qcamera_TakePicKeyHandler */

/*===========================================================================
FUNCTION Qcamera_GPSHandler

  DESCRIPTION
    Key event handler for GPS menu

  PARAMETERS:
    pMe   - Pointer to  QCam struct
    eCode - Event id
    key   - Key id

  RETURN VALUE:
    TRUE if event handled, 2 if it is not handled
===========================================================================*/
static boolean Qcamera_GPSHandler(QCam *pMe, AEEEvent eCode, uint16 key)
{
  boolean result = TRUE;

  if (!pMe)
    return FALSE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  if (eCode == EVT_KEY)
  {
    switch (key)
    {
    case AVK_LEFT:
    case AVK_RIGHT:
      break;
    case AVK_SELECT:
      {
        AEEGPSInfo gpsinfo;
        uint16 selection = IMENUCTL_GetSel(pMe->m_pIMenu);

        // Close the GPS menu first
        // IPOSDET_GetGPSInfo could create privacy dialog
        // depending on the system setting. Therefore,
        // any draw operation after calling the function
        // has undeterministic result. The best way is
        // to put up a dialog that could redraw itself
        // before calling the function.
        // Since the GPS operation takes time, it is
        // recommended to put up a tracking bar like
        // "GPS in progress" during the GPS operations
        // [which is not done here]

        pMe->m_eGPSState = QCAM_GPS_DISABLED;
        switch (selection)
        {
        case IDS_GPS_DISABLE:
          pMe->m_eGPSState = QCAM_GPS_DISABLED;
          break;
        case IDS_GPS_ENABLE_NEW:
          pMe->m_eGPSState = QCAM_GPS_ENABLED;
          ZEROAT(&pMe->m_GPSInfo);
          Qcamera_GetGPSInfo(pMe);
          break;
        case IDS_GPS_ENABLE_PREV:
          gpsinfo.dwTimeStamp = GETTIMESECONDS();
          if (pMe->m_GPSInfo.fValid)
          {
            pMe->m_eGPSState = QCAM_GPS_DATA_VALID;
          }
          else
          {
            //No vaild data, fill in some default values.
            gpsinfo.dwLat = DEFAULT_LATITUDE;
            gpsinfo.dwLon = DEFAULT_LONGITUDE;
            gpsinfo.wAltitude = DEFAULT_ALTITUDE;
          }
          (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_GPSINFO, (int32)&gpsinfo, 0);
          break;
        default:
          break;
        }

        if (selection != IDS_GPS_ENABLE_NEW)
        {
          //Need to clear menu screen before call Qcamera_CameraPreview
          //Otherwise it will fail to clean up with mdp display.
          (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_TAKE_PIC, 0);
        }
      }
      break;
    case AVK_UP:
    case AVK_DOWN:
      result = IMENUCTL_HandleEvent(pMe->m_pIMenu, eCode, key, 0);
      break;
    case AVK_CLR:
      //Need to clear menu screen before call Qcamera_CameraPreview
      //Otherwise it will fail to clean up with mdp display.
      (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_TAKE_PIC, 0);
      break;
    default:
      break;
    }
  }

  return result;
}

/*===========================================================================
FUNCTION Qcamera_OnTakePicture

  DESCRIPTION
    Handler for select key press on preview mode

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnTakePicture(QCam *pMe)
{
  if (!pMe)
    return;

  MSG_MED("USER Pressed Take pic key",0,0,0);

  if ((pMe->m_nMulPhotos > 1) && ((pMe->m_encodingType == QCAM_ENCODING_RAW) 
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif
))
  {
    //multishot does not support raw format.
    (void)Qcamera_StopCamera(pMe);
    pMe->m_nMsgId = (uint16)IDS_INVALID_RAW;
    return;
  }

#ifdef FEATURE_CAMERA_PANORAMIC
  if ((pMe->m_nPanoramic > 0) && ((pMe->m_encodingType == QCAM_ENCODING_RAW) 
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif
))
  {
    //Panaromic does not support raw format.
    (void)Qcamera_StopCamera(pMe);
    pMe->m_nMsgId = (uint16)IDS_INVALID_PANORAMIC_RAW;
    return;
  }
#endif //FEATURE_CAMERA_PANORAMIC

  if (pMe->m_bCanTakePic)
  {
    pMe->m_bCanTakePic = FALSE;
    MSG_MED("User is allowed to take picture ",0,0,0);
    //Burst mode is only work when not promp to save, no delay timer and
    //multishot is not selected.
    if (!QCam_CheckProperties(pMe->m_nProperties, QCAM_PROMPT_SAVE) &&
        (pMe->m_nSelfTimerDelay == 0) && (pMe->m_nMulPhotos == 1))
    {
      pMe->m_bKeyDown = TRUE;
      pMe->m_wLastKey = (uint16)AVK_SELECT;
    }
    Qcamera_TakePicture(pMe);
  }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

}

/*===========================================================================
FUNCTION Qcamera_OnOrientation

  DESCRIPTION
    Handler for key event on Orientation menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnOrientation(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    int nRet = 0;
    CtlAddItem pai = {0};
    byte portrait = QCam_InvertProperties(&(pMe->m_nProperties), QCAM_PORTRAIT);
    pai.pszResImage = pai.pszResText = pMe->m_pResFile;
    pai.pImage = NULL;
    pai.wImage = (uint16)(portrait ? IDB_IMAGE_ICON_PORTRAIT : IDB_IMAGE_ICON);
    (void)IMENUCTL_SetItem(pMe->m_pISoftMenu, IDS_TAKE_PIC, MSIF_IMAGE, &pai);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    Qcamera_DisplayTakePicMenuName(pMe, 0);

    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ROTATE_ENCODE, portrait ? 90 : 0, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_ROTATE_ENCODE return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
#ifdef FEATURE_CAMERA_PANORAMIC
  else if (portrait && (pMe->m_nPanoramic > 0))
  {
    pMe->m_nPanoramic = 0;    
    pMe->m_nProperties &= ~QCAM_PANORAMIC_EFFECT;
    (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PANORAMIC,  pMe->m_nPanoramic, 0);
    Qcamera_DisplayInfo(pMe);
  }  
#endif //#ifdef FEATURE_CAMERA_PANORAMIC
  }
  else if (key == (uint16)AVK_SELECT)
    Qcamera_OnTakePicture(pMe);
}

/*===========================================================================
FUNCTION Qcamera_OnZoom

  DESCRIPTION
    Handler for key event on Zoom menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnZoom(QCam *pMe, uint16 key)
{
  uint16 titleId = IDS_ZOOM;

  if (!pMe)
    return;

  if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    int nRet = 0;
    AECHAR szBuf[TEXT_STRING_SIZE] = {0};

    // Refresh zoom info
    (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_ZOOM, &pMe->m_nZoom, NULL);

    if (!pMe->m_bKeyDown && !pMe->m_bSmallScreen)
    {
      // 1st click, take 3 steps
      pMe->m_nZoom = Qcamera_UpdatePerStep(pMe, pMe->m_nZoom, pMe->m_nZoomStep * 3,
                                           pMe->m_nZoomMax, pMe->m_nZoomMin, key);
      // Remember key is down and which key it is
      pMe->m_bKeyDown = TRUE;
      pMe->m_wLastKey = key;
    }
    else
    {
      // key hold, smooth zoom with 1 step
      pMe->m_nZoom = Qcamera_UpdatePerStep(pMe, pMe->m_nZoom, pMe->m_nZoomStep,
                                           pMe->m_nZoomMax, pMe->m_nZoomMin, key);
    }
    nRet = ICAMERA_SetZoom(pMe->m_pICam, pMe->m_nZoom);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set Zoom return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }

    // The target with this flag does not support smooth zooming.
    // Should use a more meaningful flag later.
    if (!pMe->m_bSmallScreen)
    {
      titleId = (pMe->m_nZoom < 0) ? IDS_ZOOM_OPTICAL : IDS_ZOOM_DIGITAL;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      (void)ISHELL_SetTimer(pMe->a.m_pIShell, TIMEOUT_300,
                            (PFNNOTIFY)Qcamera_DoSmoothZoom,(void *)pMe);
    }
    Qcamera_GetTitleText(pMe, titleId, szBuf, TEXT_STRING_SIZE);
    Qcamera_DisplayTitle(pMe, 0, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
  }
  else if (key == (uint16)AVK_SELECT)
    Qcamera_OnTakePicture(pMe);
}

/*===========================================================================
FUNCTION Qcamera_DoSmoothZoom

  DESCRIPTION
    This function performs continuous zoom at 1 step per increment.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qcamera_DoSmoothZoom(QCam *pMe)
{
  int nRet = 0;
  AECHAR szBuf[TEXT_STRING_SIZE] = {0};
  uint16 titleId = IDS_ZOOM;

  if (!pMe)
    return;

  // Continue to smooth zoom if key is still down
  if (pMe->m_bKeyDown)
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    pMe->m_nZoom = Qcamera_UpdatePerStep(pMe, pMe->m_nZoom, pMe->m_nZoomStep,
                                         pMe->m_nZoomMax, pMe->m_nZoomMin, pMe->m_wLastKey);
    nRet = ICAMERA_SetZoom(pMe->m_pICam, pMe->m_nZoom);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set Zoom return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }

    (void)ISHELL_SetTimer(pMe->a.m_pIShell, TIMEOUT_67,
                          (PFNNOTIFY)Qcamera_DoSmoothZoom,(void *)pMe);

    titleId = (pMe->m_nZoom < 0) ? IDS_ZOOM_OPTICAL : IDS_ZOOM_DIGITAL;
    Qcamera_GetTitleText(pMe, titleId, szBuf, TEXT_STRING_SIZE);
    Qcamera_DisplayTitle(pMe, 0, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
  }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
}

#ifdef FEATURE_CAMERA_BRIGHTNESS
/*===========================================================================
FUNCTION Qcamera_OnBrightness

  DESCRIPTION
    Handler for key event on Brightness menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnBrightness(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    int nRet = 0;
    AECHAR szBuf[TEXT_STRING_SIZE] = {0};

    pMe->m_nBright = Qcamera_UpdatePerStep(pMe, pMe->m_nBright,
                                           pMe->m_nBrightnessStep,
                                           pMe->m_nBrightnessMax,
                                           pMe->m_nBrightnessMin, key);
    nRet = ICAMERA_SetBrightness(pMe->m_pICam, pMe->m_nBright);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set Brightness return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    Qcamera_GetTitleText(pMe, IDS_BRIGHTNESS, szBuf, TEXT_STRING_SIZE);
    Qcamera_DisplayTitle(pMe, 0, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
  }
  else if (key == (uint16)AVK_SELECT)
    Qcamera_OnTakePicture(pMe);
}
#else
/*===========================================================================
FUNCTION Qcamera_OnExposure

  DESCRIPTION
    Handler for key event on EV menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnExposure(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    int nRet = 0;
    AECHAR szBuf[TEXT_STRING_SIZE] = {0};
    AEEFixedPointParm p1;

    //EV value is scaled value with 3 digit precise introduced from BREW,
    //Need 1000 offset before pass pMe->m_nExposure back to BREW.
    pMe->m_nExposure = Qcamera_UpdatePerStep(pMe, pMe->m_nExposure,
                                           pMe->m_nExposureStep,
                                           pMe->m_nExposureMax,
                                           pMe->m_nExposureMin, key);
    ZEROAT(&p1);
    p1.nCurrent = (int32)(pMe->m_nExposure * 1000);
    nRet = ICAMERA_SetParm(pMe->m_pICam,
                          CAM_PARM_EXPOSURE_COMP,
                          (int32)&p1,
                          0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_EXPOSURE_COMP return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    Qcamera_GetTitleText(pMe, IDS_EV, szBuf, TEXT_STRING_SIZE);
    Qcamera_DisplayTitle(pMe, 0, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
  }
  else if (key == (uint16)AVK_SELECT)
    Qcamera_OnTakePicture(pMe);
}
#endif //FEATURE_CAMERA_BRIGHTNESS
/*===========================================================================
FUNCTION Qcamera_OnFocus

  DESCRIPTION
    Handler for key event on Focus menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnFocus(QCam *pMe, uint16 key)
{
  int nRet = 0;

  if (!pMe)
    return;

  if (pMe->m_bFocusStarted && (key != (uint16)AVK_SELECT))
  {
    MSG_HIGH("Drop key %d: Set focus is in processing", key, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    return;
  }
  else
  {
     (void)ISHELL_CancelTimer(pMe->a.m_pIShell,
                             (PFNNOTIFY)Qcamera_ResetAFIndicator,
                             (void *)pMe);
  }

  if (key == (uint16)AVK_0)
  {
    //Set the flag. Auto focus can be started without setting to the menu item
    pMe->m_bFocusStarted = TRUE;
    //Update the status indicator
    Qcamera_UpdateAFIndicator(pMe, IDB_AF_START);

    //Auto Focus mode
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_FOCUS, CAM_FOCUS_AUTO, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_FOCUS_AUTO return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      Qcamera_UpdateAFIndicator(pMe, IDB_AF_FAIL);
      //Reset the status to No Focus after 500 ms.
      (void)ISHELL_SetTimer(pMe->a.m_pIShell, TIMEOUT_500, (PFNNOTIFY)Qcamera_ResetAFIndicator,(void *)pMe);
    }
  }
  else if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    //Update the m_nFocus with current step got from camera before update the step.
    //It could be changed by auto focus or other factor.
    if (ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_FOCUS, &pMe->m_nFocus, NULL) != SUCCESS)
    {
      MSG_HIGH("Get CAM_PARM_FOCUS failed", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

      Qcamera_UpdateAFIndicator(pMe, IDB_AF_FAIL);
      //Reset the status to No Focus after 500 ms.
      (void)ISHELL_SetTimer(pMe->a.m_pIShell, TIMEOUT_500, (PFNNOTIFY)Qcamera_ResetAFIndicator,(void *)pMe);
      return;
    }

    //Set the flag.
    pMe->m_bFocusStarted = TRUE;

    //Manual focus mode, set step for focus
    pMe->m_nFocus = Qcamera_UpdatePerStep(pMe, pMe->m_nFocus, pMe->m_nFocusStep,
                                          pMe->m_nFocusMax, pMe->m_nFocusMin, key);
    //Update the status indicator
    Qcamera_UpdateAFIndicator(pMe, IDB_AF_START);

    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_FOCUS, CAM_FOCUS_MANUAL, pMe->m_nFocus);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_FOCUS_MANUAL return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

      Qcamera_UpdateAFIndicator(pMe, IDB_AF_FAIL);
      //Reset the status to No Focus after 500 ms.
      (void)ISHELL_SetTimer(pMe->a.m_pIShell, TIMEOUT_500, (PFNNOTIFY)Qcamera_ResetAFIndicator,(void *)pMe);
    }
  }
  else if (key == (uint16)AVK_SELECT)
  {
    //Not show menu but still implement
    Qcamera_OnTakePicture(pMe);
  }
}

/*===========================================================================
FUNCTION Qcamera_OnContrast

  DESCRIPTION
    Handler for key event on Contrast menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnContrast(QCam * pMe, uint16 key)
{
  if (!pMe)
    return;

  if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    int nRet = 0;
    AECHAR szBuf[TEXT_STRING_SIZE] = {0};

    pMe->m_nContrast = Qcamera_UpdatePerStep(pMe, pMe->m_nContrast,
                                             pMe->m_nContrastStep,
                                             pMe->m_nContrastMax,
                                             pMe->m_nContrastMin, key);
    nRet = ICAMERA_SetContrast(pMe->m_pICam, pMe->m_nContrast);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("ICAMERA_SetContrast return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }

    Qcamera_GetTitleText(pMe, IDS_CONTRAST, szBuf, TEXT_STRING_SIZE);
    Qcamera_DisplayTitle(pMe, 0, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
  }
  else if (key == (uint16)AVK_SELECT)
    Qcamera_OnTakePicture(pMe);
}

#if defined (FEATURE_CAMERA_V2) || defined (FEATURE_CAMERA7500)
/*===========================================================================
FUNCTION Qcamera_OnSharpness

  DESCRIPTION
    Handler for key event on Sharpness menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnSharpness(QCam * pMe, uint16 key)
{
  if (!pMe)
    return;

  if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    int nRet = 0;
    AECHAR szBuf[TEXT_STRING_SIZE] = {0};

    pMe->m_nSharpness = Qcamera_UpdatePerStep(pMe, pMe->m_nSharpness,
                                              pMe->m_nSharpnessStep,
                                              pMe->m_nSharpnessMax,
                                              pMe->m_nSharpnessMin, key);
    nRet = ICAMERA_SetSharpness(pMe->m_pICam, pMe->m_nSharpness);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("ICAMERA_SetSharpness return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }

    Qcamera_GetTitleText(pMe, IDS_SHARPNESS, szBuf, TEXT_STRING_SIZE);
    Qcamera_DisplayTitle(pMe, 0, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
  }
  else if (key == (uint16)AVK_SELECT)
    Qcamera_OnTakePicture(pMe);
}
#endif //FEATURE_CAMERA_V2 || FEATURE_CAMERA7500

#ifdef FEATURE_CAMERA_HISTOGRAM
/*=====================================================================
FUNCTION Qcamera_GetHistogramInfo

  DESCRIPTION: This function calculates the rectangle and creates a bitmap
               buffer for the preview histogram

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    int - error code defined in AEEError.h
======================================================================*/
static int Qcamera_GetHistogramInfo(QCam *pMe)
{
  IBitmap *pBmp = NULL;
  int nRet = EFAILED;
  uint16 x = 0;

  if (!pMe)
    return nRet;

  //workaround annunciator offset issue to make sure calculating below
  //does not create odd width.
  if (pMe->di.cxScreen < pMe->di.cyScreen)
  {
    if (pMe->di.cxScreen >= 480)
      x = 640;
    else
      x = 320;
  }
  else
    x = pMe->di.cxScreen;

  //Get a histogram rect relative to LCD size.
  pMe->m_rcHist.dx = (int16)(x >> 2); //1/4 of LCD longer side length
  pMe->m_rcHist.dy = (pMe->m_rcHist.dx >> 2) * 3;    //3/4 of rect width
  //bottom right above menu icon
  pMe->m_rcHist.x = (int16)(pMe->di.cxScreen - pMe->m_rcHist.dx) - 4;
  pMe->m_rcHist.y = (int16)(pMe->di.cyScreen - pMe->m_softMenuHeight) -
                    (pMe->m_rcHist.dy + 4);

  //Get the histogram IBitmap
  if (IDISPLAY_GetDeviceBitmap(pMe->a.m_pIDisplay, &pBmp) == SUCCESS)
  {
    nRet = IBITMAP_CreateCompatibleBitmap(pBmp, &pMe->m_pBmpHist,
                                          (uint16)pMe->m_rcHist.dx, 
                                          (uint16)pMe->m_rcHist.dy);
    CAM_RELEASEIF(pBmp);
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

  return nRet;
}

/*=====================================================================
FUNCTION QCamera_SetHistogram

  DESCRIPTION: This function set the preview histogram command based on
               user option.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    int - error code defined in AEEError.h
======================================================================*/
static int QCamera_SetHistogram(QCam *pMe)
{
  int32 nP2 = 0;

  if (!pMe)
    return EFAILED;

  if (pMe->m_nPrvwHistogram != CAM_HISTOGRAM_OFF)
  {
    if (!pMe->m_pBmpHist &&
        Qcamera_GetHistogramInfo(pMe) != SUCCESS)
    {
      return EFAILED;
    }
    nP2 = (int32)pMe->m_pBmpHist; //Pass buf down for histogram
  }
  else
  {
    nP2 = 0; //Pass NULL pointer down for OFF option
  }

  return ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_HISTOGRAM,
                         (int32)pMe->m_nPrvwHistogram, nP2);
}
#endif //#ifdef FEATURE_CAMERA_HISTOGRAM

#ifdef FEATURE_CAMERA7500
#ifdef FEATURE_MMOVERLAY
/*=====================================================================
FUNCTION Qcamera_ClearAFRects

  DESCRIPTION:
    This function release the focus rect objects to clear them.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    void 
======================================================================*/
static void Qcamera_ClearAFRects(QCam *pMe)
{
  int i = 0;

  if (!pMe)
    return;

  for (i = 0; i < pMe->m_nRectcount; i++)
  {
    if (pMe->m_pRect)
    {
      //clear the AF rects by drawing the white rects.
      IDISPLAY_DrawRect(pMe->a.m_pIDisplay, &pMe->m_pRect[i], RGB_WHITE, RGB_WHITE, IDF_RECT_FRAME);
      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
    }
    CAM_RELEASEIF(pMe->m_pIMMOverlayRect[i]);
  }
  pMe->m_nRectcount = 0;
  pMe->m_pRect = NULL;
}

/*=====================================================================
FUNCTION Qcamera_UpdateAFRects

  DESCRIPTION: This function draws the focus rectangles on the UI layer.
               The rectangles location are returned from camera.
               They are relative to the image not LCD so need the offset.
               The max. number of rectangles are 3.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    void
======================================================================*/
static void Qcamera_UpdateAFRects(QCam *pMe)
{
  int i;
  AEERect *pRectList = NULL;
  IBitmap *pBmp = NULL;
  OvData ov;

  if (!pMe)
    return;

  //Release the old objects first if any
  if (pMe->m_nRectcount > 0)
  {
    Qcamera_ClearAFRects(pMe);
  }
  //Get new focus rect list.
  (void)ICAMERA_GetParm(pMe->m_pICam, CAM_PARM_AUTO_FOCUS_REGION_LIST ,
                        (int32 *)&pMe->m_nRectcount, (int32 *)&pRectList);

  if (IDISPLAY_GetDeviceBitmap(pMe->a.m_pIDisplay, &pBmp) != SUCCESS)
  {
    MSG_HIGH("Failed GetDeviceBitmap in Qcamera_UpdateAFRects.", 0, 0, 0);
  }
  else if ((pMe->m_nRectcount > 0) && (pMe->m_nRectcount <= QCAM_MAX_FOCUSRECTS))
  {
    //Make sure rect number is in the boundary
    //Fill the OvData
    ov.pBmp = pBmp;
    ov.transparent = RGB_WHITE;
    ov.alpha = 128;
    ov.layer = OV_LAYER_TOP_LAYER;

    for (i = 0; i < pMe->m_nRectcount; i++)
    {
      //Register the rectangle area in UI layer
      pRectList[i].x += pMe->m_rcPreView.x;
      pRectList[i].y += pMe->m_rcPreView.y;
      SETAEERECT (&ov.rc, pRectList[i].x, pRectList[i].y, pRectList[i].dx, pRectList[i].dy);
      (void)QCamDisp_RegisterOv(pMe->m_pIMMOverlayMgr, &pMe->m_pIMMOverlayRect[i], ov);

      //Draw the green AF rect.
      IDISPLAY_DrawRect(pMe->a.m_pIDisplay, &pRectList[i], RGB_GREEN, RGB_WHITE, IDF_RECT_FRAME);
      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
    }

    //pRectList is static variables in low layer.
    pMe->m_pRect = pRectList;
  }
  CAM_RELEASEIF(pBmp);
}
#endif //FEATURE_MMOVERLAY

/*===========================================================================
FUNCTION Qcamera_OnEffect

  DESCRIPTION
    Handler for key event on Effect menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnEffect(QCam * pMe, uint16 key)
{
  if (!pMe)
    return;

  if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    AECHAR szBuf[TEXT_STRING_SIZE] = {0};
    uint32 val = (uint32)pMe->m_nEffect;
    uint16 sel = 0;
    int nRet = 0;
    int item = (key == (uint16)AVK_UP) ? -1 : 1;

    sel = QcamMenus_GetItemDataPerStep(IDS_EFFECTS, item, &val);
    if (sel > 0)
    {
      pMe->m_nEffect = (uint8)val;
      nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_EFFECT, (int32)val, 0);
      if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
      {
        MSG_HIGH("ICAMERA_SetParm return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      }
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile,
                                 sel, szBuf, sizeof(szBuf));
      Qcamera_DisplayTitle(pMe, IDS_EFFECTS, IDB_UP_DOWN,
                           szBuf, IDB_SELECT, IDS_TAKE_PIC);
    }
    else
    {
      //Out of range when key up/down to the last option.
      (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, TRUE);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
  else if (key == (uint16)AVK_SELECT)
    Qcamera_OnTakePicture(pMe);

}

/*===========================================================================
FUNCTION Qcamera_OnHistogram

  DESCRIPTION
    Handler for key event on PreviewHistogram menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnHistogram(QCam * pMe, uint16 key)
{
#ifdef FEATURE_CAMERA_HISTOGRAM
  if (!pMe)
    return;

  if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    AECHAR szBuf[TEXT_STRING_SIZE] = {0};
    uint32 val = (uint32)pMe->m_nPrvwHistogram;
    uint16 sel = 0;
    int item = (key == (uint16)AVK_UP) ? 1 : -1;

    sel = QcamMenus_GetItemDataPerStep(IDS_HISTOGRAM, item, &val);
    if (sel > 0)
    {
      pMe->m_nPrvwHistogram = (uint8)val;
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile,
                                 sel, szBuf, sizeof(szBuf));
      Qcamera_DisplayTitle(pMe, IDS_HISTOGRAM, IDB_UP_DOWN,
                           szBuf, IDB_SELECT, IDS_SELECT);
    }
    else
    {
      //Out of range when key up/down to the last option.
      (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, TRUE);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
  else if (key == (uint16)AVK_SELECT)
  {
    int nRet = QCamera_SetHistogram(pMe);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("QCamera_SetHistogram return %d", nRet, 0, 0);
      CAM_RELEASEIF(pMe->m_pBmpHist);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
#else
  MSG_HIGH("Preview histogram is not supported", 0, 0, 0);
#endif //#ifdef FEATURE_CAMERA_HISTOGRAM
}

/*===========================================================================
FUNCTION Qcamera_OnHue

  DESCRIPTION
    Handler for key event on Hue menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnHue(QCam * pMe, uint16 key)
{
  if (!pMe)
    return;

  if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    int nRet = 0;
    AECHAR szBuf[TEXT_STRING_SIZE] = {0};

    pMe->m_nHue = Qcamera_UpdatePerStep(pMe, pMe->m_nHue,
                                              pMe->m_nHueStep,
                                              pMe->m_nHueMax,
                                              pMe->m_nHueMin, key);
    nRet = ICAMERA_SetHue(pMe->m_pICam, pMe->m_nHue);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("ICAMERA_SetHue return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }

    Qcamera_GetTitleText(pMe, IDS_HUE, szBuf, TEXT_STRING_SIZE);
    Qcamera_DisplayTitle(pMe, 0, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
  }
  else if (key == (uint16)AVK_SELECT)
    Qcamera_OnTakePicture(pMe);
}

/*===========================================================================
FUNCTION Qcamera_OnSaturation

  DESCRIPTION
    Handler for key event on Saturation menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnSaturation(QCam * pMe, uint16 key)
{
  if (!pMe)
    return;

  if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    int nRet = 0;
    AECHAR szBuf[TEXT_STRING_SIZE] = {0};

    pMe->m_nSaturation = Qcamera_UpdatePerStep(pMe, pMe->m_nSaturation,
                                             pMe->m_nSaturationStep,
                                             pMe->m_nSaturationMax,
                                             pMe->m_nSaturationMin, key);
    nRet = ICAMERA_SetSaturation(pMe->m_pICam, pMe->m_nSaturation);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("ICAMERA_SetSaturation return %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }

    Qcamera_GetTitleText(pMe, IDS_SATURATION, szBuf, TEXT_STRING_SIZE);
    Qcamera_DisplayTitle(pMe, 0, IDB_UP_DOWN, szBuf, IDB_SELECT, IDS_TAKE_PIC);
  }
  else if (key == (uint16)AVK_SELECT)
    Qcamera_OnTakePicture(pMe);
}

#endif //FEATURE_CAMERA7500

/*===========================================================================
FUNCTION Qcamera_OnAlbum

  DESCRIPTION
    Handler for select key on Album menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnAlbum(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if (key == (uint16)AVK_SELECT)
  {
    if (!QCam_HasPhotos(pMe->m_pIFileMgr, LOC_ALL_SHOW))
    {
      (void)Qcamera_StopCamera(pMe);
      pMe->m_nMsgId = (uint16)IDB_NO_PHOTO;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    else
    {
      if (!Qcamera_PreLaunchViewer(pMe, VIEWER_MODE_ALBUM))
      {
        MSG_HIGH("Failed to launch album.", 0, 0, 0);
      }
    }
  }
}

/*===========================================================================
FUNCTION Qcamera_OnSlideshow

  DESCRIPTION
    Handler for select key on Slideshow menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnSlideshow(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if (key == (uint16)AVK_SELECT)
  {
    if (!QCam_HasPhotos(pMe->m_pIFileMgr, LOC_ALL_SHOW))
    {
      (void)Qcamera_StopCamera(pMe);
      pMe->m_nMsgId = (uint16)IDB_NO_PHOTO;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    else
    {
      if (!Qcamera_PreLaunchViewer(pMe, VIEWER_MODE_SLIDESHOW))
      {
        MSG_HIGH("Failed to launch slideshow.", 0, 0, 0);
      }
    }
  }
}

/*===========================================================================
FUNCTION Qcamera_OnFrame

  DESCRIPTION
    Handler for select key on Add Frame menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnFrame(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if (key == (uint16)AVK_SELECT)
  {
    if (!Qcamera_GetMatchSizeFrameDir(pMe))
    {
      (void)Qcamera_StopCamera(pMe);
      pMe->m_nMsgId = (uint16)IDB_NO_FRAME;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    else
    {
      if (!Qcamera_PreLaunchViewer(pMe, VIEWER_MODE_FRAME))
      {
        MSG_HIGH("Failed to launch frame.", 0, 0, 0);
      }
    }
  }
}

#ifndef FEATURE_CAMERA_V2
/*===========================================================================
FUNCTION Qcamera_OnGPS

  DESCRIPTION
    Handler for select key on GPS menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnGPS(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if (key == (uint16)AVK_SELECT)
  {
    pMe->m_bGPSmenu = TRUE;
    if (!Qcamera_StopCamera(pMe))
    {
      MSG_HIGH("Not launch GPS menu - no memory or failed to stop preview.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      return;
    }
    pMe->m_eState = QCAM_STATE_MENU;
  }
}
#endif //FEATURE_CAMERA_V2

/*===========================================================================
FUNCTION Qcamera_OnSettings

  DESCRIPTION
    Handler for key select on Setting menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnSettings(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if (key == (uint16)AVK_SELECT)
  {
    if (!Qcamera_StopCamera(pMe))
    {
      MSG_HIGH("Not launch Setting menu - no memory or failed to stop preview.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      return;
    }
    pMe->m_eState = QCAM_STATE_MENU;
  }
}

#ifdef FEATURE_APP_QCAMCORDER
/*===========================================================================
FUNCTION Qcamera_OnQCamcorder

  DESCRIPTION
    Handler for select key on Qcamcorder menu item

  PARAMETERS:
    pMe - Pointer to  QCam struct
    key - key id

  RETURN VALUE:
    None
===========================================================================*/
void Qcamera_OnQCamcorder(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if (key == (uint16)AVK_SELECT)
  {
    //Qcamcorder will be started after camera is stoped.
    if (!Qcamera_StopCamera(pMe))
    {
      MSG_HIGH("Not launch Camcorder - no memory or failed to stop preview.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      return;
    }
    pMe->m_eState = QCAM_STATE_QCAMCORDER;
  }
}
#endif //FEATURE_APP_QCAMCORDER

#ifdef FEATURE_CAMERA_MOBICAT
/*=====================================================================
FUNCTION Qcamera_OnMCT

  DESCRIPTION:

  PARAMETERS:
    pMe
    key

  RETURN VALUE:
    void
======================================================================*/
void Qcamera_OnMCT(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if (key == (uint16)AVK_SELECT)
  {
    CtlAddItem pai = {0};

    pMe->m_bMobiCaTon = !pMe->m_bMobiCaTon;
    pai.pszResImage = pai.pszResText = pMe->m_pResFile;
    pai.pImage = NULL;
    pai.wImage = (uint16)(pMe->m_bMobiCaTon ? IDB_MCT_OFF : IDB_MCT_ON);
    (void)IMENUCTL_SetItem(pMe->m_pISoftMenu, IDS_MCT, MSIF_IMAGE, &pai);
    (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
    Qcamera_DisplayTakePicMenuName(pMe, 0);
  }
}
#endif //FEATURE_CAMERA_MOBICAT

#ifndef FEATURE_CAMERA_V2
/*=====================================================================
FUNCTION Qcamera_OnSimulateDisplay

  DESCRIPTION: This function is to simulate the 2nd LCD display for test.
               It assumes the LCD is in 180 degree rotated.
               After simulation, the m_nReflectPreview is reset.

  PARAMETERS:
    pMe   - pointer to QCam data struct

  RETURN VALUE:
    void
======================================================================*/
void Qcamera_OnSimulateDisplay(QCam *pMe, uint16 key)
{
  if (!pMe)
    return;

  if (key == (uint16)AVK_SELECT)
  {
    int32 nRotateEnc;
    int32 nRotatePrew;
    int32 nReflect;
    int nRet;

    pMe->m_b2ndDisplay = !pMe->m_b2ndDisplay;
    Qcamera_DisplayTakePicMenuName(pMe, 0);

    if (!pMe->m_b2ndDisplay)
    {
      MSG_MED("Return to first display.", 0, 0, 0);
      nRotatePrew = 0;
      nRotateEnc = QCam_CheckProperties(pMe->m_nProperties, QCAM_PORTRAIT) ? 90 : 0;
      nReflect = 0;
    }
    else
    {
      MSG_MED("Simulate second display.", 0, 0, 0);
      nRotatePrew = 180;
      nRotateEnc = QCam_CheckProperties(pMe->m_nProperties, QCAM_PORTRAIT) ? 270 : 180;
      nReflect = 2;
    }
    pMe->m_nReflectPreview = (uint16)nReflect;
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ROTATE_PREVIEW, nRotatePrew, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_ROTATE_PREVIEW returns %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_REFLECT, nReflect, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_REFLECT returns %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    nRet = ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_ROTATE_ENCODE, nRotateEnc, 0);
    if ((nRet != SUCCESS) && (nRet != CAM_PENDING))
    {
      MSG_HIGH("Set CAM_PARM_ROTATE_ENCODE returns %d", nRet, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
}
#endif //FEATURE_CAMERA_V2

/*===========================================================================
FUNCTION Qcamera_SettingsKeyHandler

  DESCRIPTION
    Key event handler for Setting menu

  PARAMETERS:
    pMe   - Pointer to  QCam struct
    eCode - Event id
    key   - Key id

  RETURN VALUE:
    TRUE if event handled, FALSE if it is not handled
===========================================================================*/
static boolean Qcamera_SettingsKeyHandler (QCam *pMe, AEEEvent eCode, uint16 key)
{
  boolean result = TRUE;
  uint16  focus = 0;

  if (!pMe)
    return FALSE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  if (eCode == EVT_KEY)
  {
    // If the shutter sound menu is active, make sure it is not still playing a sound.
    if (pMe->m_activeMenu == QCAM_MENU_SOUND)
    {
      Qcamera_StopSound(pMe);
    }

    switch (key)
    {
    case AVK_SELECT:
    case AVK_UP:
    case AVK_DOWN:
      if (pMe->m_activeMenu == QCAM_MENU_MEM_USAGE)
      {
        return ISTATIC_HandleEvent(pMe->m_pIStatic, EVT_KEY, key, 0);
      }
      else
      {
        focus = IMENUCTL_GetFocus(pMe->m_pIMenu);
        result = IMENUCTL_HandleEvent(pMe->m_pIMenu, eCode, key, 0);
        if (result && (key != (uint16)AVK_SELECT) && (pMe->m_activeMenu == QCAM_MENU_DELETEALL))
        {
          //If it is Up/Down key on the delete all menu, update the radio icon
          //because select key will overwrite the menu screen right away.
          int item = (key == (uint16)AVK_UP) ? 1 : -1;
          //If old focus is in first one or last one, set item to 0
          if ((focus == IDS_YES && item == 1) ||
              (focus == IDS_NO && item == -1))
          {
            item = 0;
          }

          QcamMenus_UpdateDeleteAllMenu(pMe, item);
        }
      }
      break;
    case AVK_LEFT:
    case AVK_RIGHT:
      if (pMe->m_activeMenu == QCAM_MENU_MEM_USAGE)
        CAM_RELEASEIF(pMe->m_pIStatic);
      if (QcamMenus_DoSettingsMenu(pMe, (key == (uint16)AVK_LEFT ? -1 : 1) ))
        result = IMENUCTL_HandleEvent(pMe->m_pISoftMenu, eCode, key, 0);
      else
        result = FALSE;
      break;
    case AVK_CLR:
      result = TRUE;
      CAM_RELEASEIF(pMe->m_pIStatic);
      //Reset the selected menu item so the user can take picture
      //right after returning to preview mode.
      IMENUCTL_SetActive(pMe->m_pIMenu,FALSE);
      pMe->m_wTakePicMenuSel = IDS_TPMENU_START;
      pMe->m_activeSoftMenu = QCAM_SOFT_MENU_TAKE_PHOTO;
     (void) ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_TAKE_PIC, 0);
      break;
    default:
      break;
    }
  }

  return result;
} /* END Qcamera_SettingsKeyHandler */

/*===========================================================================

FUNCTION Qcamera_HandleEvent

DESCRIPTION
   This is the EventHandler for this app. All events to this app are handled in this
   function. All APPs must supply an Event Handler.

PROTOTYPE:
   boolean Qcamera_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
   pi: Pointer to the AEEApplet structure. This structure contains information specific
   to this applet. It was initialized during the AEEClsCreateInstance() function.

   ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean Qcamera_HandleEvent(QCam *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
#ifdef FEATURE_QVPHONE
  AEERect rc;
  IImage *pImage = NULL;
#endif /*FEATURE_QVPHONE*/

  if (!pMe)
    return FALSE;

  switch (eCode)
  {
  case EVT_APP_START:
    MSG_MED("Received EVT_APP_START event", 0, 0, 0);
    if (!Qcamera_Init(pMe))
      return FALSE;
#ifdef FEATURE_QVPHONE
    /* Disallow Qcamera start when in a VT call
    ** Draw icon notifying user of the non-operation */
    SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen - pMe->m_softMenuHeight);
    if (AEEVP_IS_VIDEOCALL_PRESENT( pMe->m_pIVP ))
    {
	  pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, IDB_NO_CAMERA_VT);
	  QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, rc);
	  return TRUE;
    }
    else
#endif /*FEATURE_QVPHONE*/
    {
    pMe->m_activeSoftMenu = QCAM_SOFT_MENU_TAKE_PHOTO;
    return ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_TAKE_PIC, 0);
    }
  case EVT_APP_RESUME:
    MSG_MED("Received EVT_APP_RESUME event", 0, 0, 0);
    pMe->m_activeSoftMenu = QCAM_SOFT_MENU_TAKE_PHOTO;
    return ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_TAKE_PIC, 0);

  case EVT_APP_STOP:
    MSG_MED("Received EVT_APP_STOP event", 0, 0, 0);
    Qcamera_SetConfigRecord(pMe);
    return TRUE;

  case EVT_APP_SUSPEND:
    MSG_MED("EVT_APP_SUSPEND, state %d", pMe->m_eState, 0, 0);
    //Simply return FALSE so BREW will stop Qcamera.
    //BREW will restart qcamera again when the other application stops.
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    return FALSE;

  case EVT_DIALOG_INIT:
    if (Qcamera_MapEventHandlers(pMe, (IDialog *) dwParam, wParam))
    {
      IDIALOG_SetProperties((IDialog *) dwParam, DLG_HANDLE_ALL_EVENTS);
      return TRUE;
    }
    else
      return FALSE;

  case EVT_DIALOG_START:
    // Add dialog startup code here
    return TRUE;

  case EVT_DIALOG_END:
    return TRUE;

  case EVT_KEY_RELEASE:
    pMe->m_bKeyDown = FALSE;
    //lint -fallthrough
  case EVT_KEY_PRESS:
  case EVT_KEY:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    if (pMe->m_activeSoftMenu == QCAM_SOFT_MENU_TAKE_PHOTO)
    {
      if (pMe->m_activeMenu == QCAM_MENU_GPS)
      {
        //handle GPS menu separally because it is list menu
        return Qcamera_GPSHandler(pMe, eCode, wParam);
      }
      else if ((pMe->m_eState != QCAM_STATE_SAVE) || (wParam == (uint16)AVK_CLR))
      {
        return Qcamera_TakePicKeyHandler(pMe, eCode, wParam);
      }
      else
        return TRUE;
    }
    else if (pMe->m_activeSoftMenu == QCAM_SOFT_MENU_SETTINGS)
    {
      return Qcamera_SettingsKeyHandler(pMe, eCode, wParam);
    }
#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE  
    else if (eCode == EVT_KEY )
    {
#ifdef FEATURE_QVPHONE
      if(!AEEVP_IS_VIDEOCALL_PRESENT( pMe->m_pIVP ))
#endif /*FEATURE_QVPHONE*/
      {
        pMe->m_activeSoftMenu = QCAM_SOFT_MENU_TAKE_PHOTO;
        return ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_TAKE_PIC, 0);
      }
    }
    break;

  case EVT_COMMAND:
    if (pMe->m_activeSoftMenu == QCAM_SOFT_MENU_TAKE_PHOTO)
    {
      switch (wParam)
      {
      case IDS_TAKE_PIC: // Builds Take Picture menu and starts preview mode
#ifdef FEATURE_CAMERA_MULTI_SENSOR
        if (dwParam == (uint32)AVK_POUND)
        {
          //Switch the camera
          pMe->m_nSensorId = !(pMe->m_nSensorId);

          //Initialize the camera
          (void)Qcamera_CreateCamera(pMe);
        }
#endif //FEATURE_CAMERA_MULTI_SENSOR
        MSG_MED("CHANGING STATE TO SOFT_MENU_TAKE_PHOTO HERE",0,0,0);
        Qcamera_CameraPreview(pMe);
        return TRUE;
      default:
        return FALSE;
      }
    }
    else if(pMe->m_activeMenu != QCAM_MENU_NONE)
    {
      if((pMe->m_activeMenu == QCAM_MENU_DELETEALL) &&
         (wParam == IDS_NO || wParam == IDS_YES_DEL))
        Qcamera_DeleteAllFilesResponse(pMe, wParam);
      else
        QcamMenus_ExecuteActiveMenu(pMe);
      return TRUE;
    }
    break;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

    default:
      break;
  }
  return FALSE;
}

/*********************************************************************************************/
/*****                                                                                   *****/
/*****                  Notifiers/Callbacks for Camera Application                       *****/
/*****                                                                                   *****/
/*********************************************************************************************/
/*===========================================================================
FUNCTION Qcamera_CamEventNotify

  DESCRIPTION
    OEMCamera call back handler. This function is registered with ICAMERA and
    after an asyn. function call to ICAMERA, it will return the status here.
    Refer to AEECamrea.h for the camera processing sequence and callback.

  PARAMETERS:
    pMe - Pointer to  QCam struct
    pcn - Pointer to AEECameraNotify struct which holds all callback information

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_CamEventNotify(QCam *pMe, AEECameraNotify *pcn)
{
  if (!pMe || !pcn)
    return;

  if ((pcn->nStatus == CAM_STATUS_ABORT) ||
      (pcn->nStatus == CAM_STATUS_SPACE_ERROR) ||
      (pcn->nStatus == CAM_STATUS_IO_ERROR))
  {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    CAM_FREEIF(pMe->m_pData);
    CAM_FREEIF(pMe->m_pLaunchDS);
    goto ResumePreview;
  }

  switch (pcn->nCmd)
  {
  case CAM_CMD_START:
    switch (pcn->nStatus)
    {
    case CAM_STATUS_START:
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
      //Init preview mode on receive this status because direct mode
      //will not send CAM_STATUS_FRAME callback.
      //Stress test with Sel key could cause camera out sync issue if
      //QCAM_STATE_PREVIEW is set before camera is really switched in
      //priview state.
      if (pMe->m_eState == QCAM_STATE_START_PREVIEW)
      {
        MSG_MED("Qcamera_SetPreviewReady on start callback", 0, 0, 0);
        Qcamera_SetPreviewReady(pMe);
      }
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */

      // Restart preview
      if (pMe->m_eState == QCAM_STATE_SNAPSHOT)
      {
        MSG_MED("Start preview mode from QCAM_STATE_SNAPSHOT", 0, 0, 0);
        Qcamera_CameraPreview(pMe);
      }
      break;

    case CAM_STATUS_FRAME:
#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif  //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE 
      //It is the image from the viewfinder.
      Qcamera_UpdateFrame(pMe);

      //Init preview mode on receive first frame. Keep this here for
      //backward compatible issue.(Old service layer code returns 
      //CAM_STATUS_START right after receive start Preview command)
      //Stress test with Sel key could cause camera out sync issue if
      //QCAM_STATE_PREVIEW is set before camera is really switched in
      //priview state.
      if (pMe->m_eState == QCAM_STATE_START_PREVIEW)
      {
        MSG_MED("Qcamera_SetPreviewReady on first FRAME callback", 0, 0, 0);
        Qcamera_SetPreviewReady(pMe);
      }
      //Continue burst mode take picture after the first frame back
      //entering preview mode. Not support when direct mode on because it 
      //does not return callback to tell when the first frame is ready.
      if (pMe->m_bKeyDown && (pMe->m_wLastKey == (uint16)AVK_SELECT) &&
          (pMe->m_eState == QCAM_STATE_PREVIEW) && !pMe->m_bOneButtonSnapshot)
      {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
        Qcamera_TakePicture(pMe);
      }

#ifdef HEAP_CHECK
      if (pMe->m_pIHeap)
      {
        static uint32 dwLastUsed = 0;
        uint32 dwSizeUsed = 0;

        dwSizeUsed = IHEAP_GetMemStats(pMe->m_pIHeap);
        // print out only if different
        if (dwSizeUsed != dwLastUsed)
        {
          MSG_HEAP("Frame Update: current heap usage %d ", dwSizeUsed, 0, 0);
          Qcamera_ShowHeapUsage(pMe);
          dwLastUsed = dwSizeUsed;
        }
      }
#endif //HEAP_CHECK

      break;

    case CAM_STATUS_DONE:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

      //Before start the Qalbum, we fill in m_pLaunchDS and stop the camera.
      //When we get this call back, start the Qalbum if the m_pLaunchDS is not null.
      if (pMe->m_pLaunchDS)
      {
        char pszArg[ARG_STRING_SIZE] = {0};

        QCam_GetArgString(pMe->m_pLaunchDS, pszArg, ARG_STRING_SIZE);

        if (ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_QALBUM, pszArg) != SUCCESS)
        {
          MSG_HIGH("ISHELL_StartAppletArgs failed", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
        }
        else
        {
          MSG_MED("Start Qalbum", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
        }
        break;
      }

#ifdef FEATURE_APP_QCAMCORDER
      //Stop camera is for launching qcamcorder
      if (pMe->m_eState == QCAM_STATE_QCAMCORDER)
      {
        if (ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_QCAMCORDER) != SUCCESS)
        {
          MSG_HIGH("ISHELL_StartAppletArgs failed", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
        }
        else
        {
          MSG_MED("Start Qcamcorder", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
        }
        break;
      }
#endif //#ifdef FEATURE_APP_QCAMCORDER
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND 
      if (pMe->m_eState == QCAM_STATE_SNAPSHOT)
      {
        if (QCAM_HAS_SHUTTER_SOUND(pMe->m_sShutterSoundFile) && (pMe->m_nPanoramic < 2) && 
           (IFILEMGR_Test(pMe->m_pIFileMgr, pMe->m_sShutterSoundFile) == SUCCESS))
        {
          MSG_MED("PreTakePicture - Play Shutter Sound", 0, 0, 0);
          Qcamera_PlayShutterSound(pMe);
        }
      }
#endif //#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND

      //Handle it based on the state otherwise
      switch (pMe->m_eState)
      {
      case QCAM_STATE_NULL:
        if (pMe->m_nMsgId > 0)
        {
          //Display message after preview stop than reset the message id.
          if (IS_IMGID(pMe->m_nMsgId))
          {
            Qcamera_DisplayTimedImg(pMe,
                                    (PFNNOTIFY)Qcamera_TurnOnCameraPreview ,
                                    pMe->m_nMsgId);
          }
          else
          {
            Qcamera_DisplayTimedMsg(pMe,
                                    (PFNNOTIFY)Qcamera_TurnOnCameraPreview ,
                                    pMe->m_nMsgId);
          }
          pMe->m_nMsgId = 0;
        }
        break;

      case QCAM_STATE_MENU:
        //reset the eState to NULL so it works as before
        pMe->m_eState = QCAM_STATE_NULL;
        if (pMe->m_bGPSmenu)
        {
          pMe->m_bGPSmenu = FALSE;
          QcamMenus_BuildGPSMenu(pMe);
        }
        else
        {
          QcamMenus_BuildSettingsMenu(pMe);
        }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
        break;

      case QCAM_STATE_SNAPSHOT:
#ifdef FEATURE_OVERLAY
#ifdef FEATURE_MMOVERLAY //7K
        //Pass TRUE to register full screen display
        Qcamera_RegisterOvScreen(pMe, TRUE, TRUE);
#else //6K
        //Register full screen for message display
        QCamDisp_MdpRegisterFullScreen(pMe->a.m_pIDisplay, pMe->m_pIOverlay,
                                       pMe->di.cxScreen, pMe->di.cyScreen);
#endif // #ifdef FEATURE_MMOVERLAY
#endif //FEATURE_OVERLAY

        //Display the last frame on the screen
        MSG_MED("Start record snap shot",0,0,0);
        Qcamera_DoSnapshot(pMe);
        break;

      case QCAM_STATE_SAVE:
        MSG_MED("End record snap shot", 0, 0, 0);
#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
#ifdef FEATURE_CAMERA_PANORAMIC
        if (pMe->m_nPanoramic > 0)
        {
          //If smaller than 3 shots, return to preview for next shot
          //Otherwise, Call camera stitch to combine the 3 shots before encoding
          pMe->m_nPanoramic++;
          if (pMe->m_nPanoramic <= 3)
          {
            if (Qcamera_UpdatePanoramicFrame(pMe))
            {
              Qcamera_TurnOnCameraPreview(pMe);
            }
            else
            {
              //Disable the panoramic mode for any error and 
              //notify service layer to clean up buffers.
              pMe->m_nPanoramic = 0;
              pMe->m_nProperties &= ~QCAM_PANORAMIC_EFFECT;
              (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PANORAMIC,  pMe->m_nPanoramic, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION  
              (void)ICAMERA_DoStitch(pMe->m_pICam, 0, 0);
              Qcamera_DisplayTimedMsg(pMe,
                                      (PFNNOTIFY)Qcamera_TurnOnCameraPreview,
                                      IDS_FAIL_TAKEPICTURE);
            }
          }
          else
          {
            CAM_RELEASEIF(pMe->m_pOvPanoramic);
            pMe->m_nPanoramic--;
            if (ICAMERA_DoStitch(pMe->m_pICam, pMe->m_nPanoramic, 4) != CAM_PENDING)
            {
              MSG_HIGH("ICAMERA_DoStitch error.", 0, 0, 0);
              pMe->m_nPanoramic = 0;
              pMe->m_nProperties &= ~QCAM_PANORAMIC_EFFECT;
              (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PANORAMIC,  pMe->m_nPanoramic, 0);
              goto ResumePreview;
            }
            pMe->m_nPanoramic = 1;
          }
          break;
        }
#endif //FEATURE_CAMERA_PANORAMIC
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif
        if (!QCam_CheckProperties(pMe->m_nProperties, QCAM_PROMPT_SAVE))
        {
          pMe->m_bSaveAppPic = FALSE;
          Qcamera_UpdateFrame(pMe);
#ifndef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
          Qcamera_DoAutoSave(pMe);
#else

          if (pMe->m_bSnapShutterSndDone == TRUE && pMe->m_bSnapCaptureDone == TRUE)
          {
             Qcamera_DoAutoSave(pMe);
             pMe->m_bSnapShutterSndDone = FALSE;
             pMe->m_bSnapCaptureDone = FALSE;
          }
          else
          {
             // Encode the snapshot, if shutter shound done.
             pMe->m_bNeedToEncodePicture = TRUE;
          }
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND 
        }
        else
        {
          //Create a save dialog
          Qcamera_BuildSaveDlg(pMe);
        }
        break;
      default:
        break;
      }
      break;
    case CAM_STATUS_FAIL:  
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      CAM_FREEIF(pMe->m_pLaunchDS);
      goto ResumePreview;
    default:
      break;
    }
    break;
  case CAM_CMD_SETPARM:
    switch (pcn->nStatus)
    {
#ifdef FEATURE_CAMERA_HISTOGRAM
    case CAM_STATUS_UPDATE:
      //set histogram when preview started if option not off
      if (pMe->m_nPrvwHistogram != CAM_HISTOGRAM_OFF)
      {
        if (QCamera_GetOvObj(pMe, pMe->m_pBmpHist, pMe->m_rcHist, &pMe->m_pOvHist))
          (void)IMMOVERLAY_Update(pMe->m_pOvHist, NULL);
      }
      break;
#endif //#ifdef FEATURE_CAMERA_HISTOGRAM
    case CAM_STATUS_FAIL:
    case CAM_STATUS_DONE:
#ifdef FEATURE_CAMERA_PANORAMIC
      if ((pMe->m_eState == QCAM_STATE_SAVE) && (pMe->m_nPanoramic > 0))
      {
        //Panoramic stitch return here
        if (pcn->nStatus == CAM_STATUS_DONE)
        {
          //Create a save dialog to display the stitched image
          Qcamera_BuildSaveDlg(pMe);
        }
        else
        {
          MSG_HIGH("Panoramic stitch error.", 0, 0, 0);
          pMe->m_nPanoramic = 0;
          pMe->m_nProperties &= ~QCAM_PANORAMIC_EFFECT;
	  (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_PANORAMIC,  pMe->m_nPanoramic, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
          goto ResumePreview;
        }
        break;
      }
#endif //FEATURE_CAMERA_PANORAMIC
      //Only update the indicator while the Focus menu item is selected.
      if (pMe->m_bFocusStarted)
      {
        pMe->m_bFocusStarted = FALSE;
        if (pMe->m_bOneButtonSnapshot)
        {
          pMe->m_bOneButtonSnapshot = FALSE;
          //Go ahead to take picture no matter focus or not.
          Qcamera_PreTakePicture(pMe);
        }
        else
        {
          if (pcn->nStatus == CAM_STATUS_FAIL)
          {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
            Qcamera_UpdateAFIndicator(pMe, IDB_AF_FAIL);
          }
          else
          {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
            //Update the indicator
            Qcamera_UpdateAFIndicator(pMe, IDB_AF_DONE);

#if defined (FEATURE_MMOVERLAY) && defined (FEATURE_CAMERA7500)
            //Update the focus rectangles
            Qcamera_UpdateAFRects(pMe);
#endif //FEATURE_MMOVERLAY && FEATURE_CAMERA7500
          }
          //Reset the status to No Focus after 2 seconds.
          (void) ISHELL_SetTimer(pMe->a.m_pIShell, TIMEOUT_2000, (PFNNOTIFY)Qcamera_ResetAFIndicator,(void *)pMe);
        }
      }
#ifdef FEATURE_CAMERA_HISTOGRAM
      else //callback is not for focus
      {
        //clear buffer if preview histogram is off
        if (pMe->m_nPrvwHistogram == CAM_HISTOGRAM_OFF)
        {
          CAM_RELEASEIF(pMe->m_pOvHist);
          CAM_RELEASEIF(pMe->m_pBmpHist);
        }
      }
#endif //#ifdef FEATURE_CAMERA_HISTOGRAM
      break;
    default:
      break;
    } //switch (pcn->nStatus)
    break;
  case CAM_CMD_ENCODESNAPSHOT:
    MSG_MED("Got Encode callback ", 0, 0, 0);
    if (pcn->nStatus == CAM_STATUS_DONE)
    {
      MSG_MED("Got encode complete callback", 0, 0, 0);

      if(pMe->m_encodingType == QCAM_ENCODING_MEM)
      {
        IFile * pIFile = NULL;
        uint32 size = (uint32) pcn->pData;
        if (size > 0)
        {
          pIFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, pMe->m_sCurrentFileName, _OFM_CREATE);
          if(pIFile && pMe->m_pData && (size < pMe->m_nFileSize))
            (void)IFILE_Write(pIFile, pMe->m_pData, size);
          else
            MSG_HIGH("Encode callback returns size %d larger than %d.", size, pMe->m_nFileSize, 0);
        }
        else
        {
          MSG_HIGH("Encode callback returns 0 size.", 0, 0, 0);
        }
        CAM_FREEIF(pMe->m_pData);
        CAM_RELEASEIF(pIFile);
      }
      if (pMe->m_nMulPhotos == 1)
      {
        if (pMe->m_bNeedToSend)
        {
#ifdef FEATURE_APP_MMS
#error code not present
#else
          MSG_HIGH("FEATURE_APP_MMS not defined. Failed to send picture", 0, 0, 0);
#endif //FEATURE_APP_MMS
          pMe->m_bNeedToSend = FALSE;
        }
        Qcamera_DisplayPhotoSavedImage(pMe);
      }
#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
    }
    else
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      CAM_FREEIF(pMe->m_pData);
      goto ResumePreview;
    }
    break;
  default:
    if (pcn->nStatus == CAM_STATUS_FAIL)
      goto ResumePreview;
    break;
  }
  return;

ResumePreview:
  MSG_HIGH("ICAMERA call back error.",0,0,0);
  //Turn on pMe->m_bCanTakePic before return to preview mode.
  //It will enable take picture again in case of failure.
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  {
    //If UI failed to check the correct space but low layer return no memory,
    //display the message before return to preview.
#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
    if (pcn->nStatus == CAM_STATUS_SPACE_ERROR)
    {
      Qcamera_DisplayTimedMsg(pMe,
                              (PFNNOTIFY)Qcamera_TurnOnCameraPreview,
                              IDS_MEM_FULL);
    }
    else if (!pMe->m_bCanTakePic)
    {
#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif //FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE
      //If it is in take picture processing, show failure message.
      Qcamera_DisplayTimedMsg(pMe,
                              (PFNNOTIFY)Qcamera_TurnOnCameraPreview,
                              IDS_FAIL_TAKEPICTURE);
    }
    else
      Qcamera_TurnOnCameraPreview(pMe);
  }
  return;
}

/*===========================================================================
FUNCTION Qcamera_MediaNotify

  DESCRIPTION
    IMEDIA call back handler. When a sound is played or abort, get the callback.

  PARAMETERS:
    pUser - Pointer cast to  QCam struct
    pCmdNotify - Pointer to AEEMediaCmdNotify struct which holds all callback information

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
{
  QCam *pMe = (QCam *)pUser;

  if (!pMe || !pCmdNotify)
    return;

  if (pCmdNotify->nCmd == MM_CMD_PLAY)  // IMEDIA_Play events
  {
    switch (pCmdNotify->nStatus)
    {
    case MM_STATUS_START:
#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
      // Send the information to lower layer Shutter sound begun. 
      (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_SHUTTER_SOUND, (int32)pCmdNotify->nStatus, 0);
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
      break;
    case MM_STATUS_ABORT:
      //Log message to make sure we get failure call back
      MSG_HIGH("Failed to play shutter sound %s.", pMe->m_sShutterSoundFile, 0, 0);
      if (STRCMP(pMe->m_sShutterSoundFile, QCAM_SHUTTER_FILE) != 0)
      {
        // Bad sound file, reset to default "No Sound"
        (void)STRLCPY(pMe->m_sShutterSoundFile, QCAM_SHUTTER_FILE, FILE_NAME_SIZE);
      }
      if (pMe->m_activeSoftMenu != QCAM_SOFT_MENU_SETTINGS)
      {
#ifndef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
        // Unable to play sound file, take picture anyway.
        MSG_MED("Stop preview after shutter sound abort.", 0, 0, 0);
        Qcamera_StopPreview(pMe);
#else
        // Send the information to lower layer Shutter sound Abort. 
        (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_SHUTTER_SOUND, (int32)pCmdNotify->nStatus, 0);
        if ( pMe->m_bNeedToEncodePicture == TRUE)
        {
          Qcamera_DoAutoSave(pMe); 
          pMe->m_bSnapCaptureDone = FALSE;
          pMe->m_bSnapShutterSndDone = FALSE; 
          pMe->m_bNeedToEncodePicture = FALSE;
        }
#endif // FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
      }
      else
      {
        // reset the sound menu to reflect the bad sound file
        QcamMenus_BuildSoundMenu(pMe);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      }
      break;

    case MM_STATUS_DONE:    // playback done
      if (pMe->m_activeSoftMenu != QCAM_SOFT_MENU_SETTINGS)
      {
        MSG_MED("Shutter sound ends.", 0, 0, 0);
#ifndef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
        MSG_MED("Stop preview after shutter sound complete.", 0, 0, 0);
        Qcamera_StopPreview(pMe);
#else
        // Send the information to lower layer Shutter sound done. 
        (void)ICAMERA_SetParm(pMe->m_pICam, CAM_PARM_SHUTTER_SOUND, (int32)pCmdNotify->nStatus, 0);
        pMe->m_bSnapShutterSndDone = TRUE;
        if ( pMe->m_bNeedToEncodePicture == TRUE)
    {
          Qcamera_DoAutoSave(pMe); 
          pMe->m_bSnapCaptureDone = FALSE;
          pMe->m_bSnapShutterSndDone = FALSE; 
          pMe->m_bNeedToEncodePicture = FALSE;
    }
#endif //FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
      }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

      break;
    default:
      break;
    }
  } // MM_CMD_PLAY
}

/*===========================================================================
FUNCTION Qcamera_FrameImageNotify

  DESCRIPTION
    IMAGE call back handler. When a frame is loaded, it is called for further process.

  PARAMETERS:
    pUser - Pointer cast to  QCam struct
    pImage - Image pointer to the image buffer
    pi - Pointer to AEEImageInfo struct which holds image information

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_FrameImageNotify(void *pUser,
                                     IImage *pImage,
                                     AEEImageInfo *pi,
                                     int nErr)
{
  QCam *pMe = (QCam *)pUser;

  if (!pMe)
    return;

  if (!pImage || nErr || !pi)
  {
    MSG_HIGH("Failed to load frame image.", 0, 0, 0);
    goto FnExit;
  }

  if (Qcamera_IsSizeMatch(pMe, pi->cx, pi->cy))
  {
    //get a RGB565 DIB from the decoded image
    pMe->m_pFrame = QCam_GetPNGImageData(pMe->a.m_pIDisplay, pImage);
    //ncTransparent is not correct in m_pFrame, just use default color now.
    if (pMe->m_pFrame)
    {
      pMe->m_pFrame->ncTransparent = DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB565;
    }
    else
    {
      MSG_HIGH("QCam_GetPNGImageData returns NULL.", 0, 0, 0);
    }
  }
  else
  {
    //Just log the message now.
    MSG_HIGH("Frame orientation does not match.", 0, 0, 0);
  }

FnExit:
  CAM_RELEASEIF(pMe->m_pFrameImg);
#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
  Qcamera_AddRemoveIconsStamps(pMe);
#else
  Qcamera_AddOverlay(pMe, pMe->m_pFrame);
#endif //#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY

  if (ICAMERA_Preview(pMe->m_pICam) != SUCCESS)
  {
    MSG_HIGH("ICAMERA_Preview failed.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  }
}

/*===========================================================================
FUNCTION Qcamera_GetGPSInfoCB

  DESCRIPTION
    GPS call back handler. Information from callback already saved in pMe->GPSInfoCB

  PARAMETERS:
    pUser - Pointer cast to  QCam struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qcamera_GetGPSInfoCB(void *pUser)
{
  QCam *pMe = (QCam *)pUser;
  boolean retry = FALSE;

  if (!pMe)
    return;

  if (pMe->m_eGPSState == QCAM_GPS_ACQUIRING)
  {
    switch (pMe->m_GPSInfo.status)
    {
    case AEEGPS_ERR_NO_ERR:
      pMe->m_eGPSState = QCAM_GPS_DATA_VALID;
      break;
    case AEEGPS_ERR_GENERAL_FAILURE:
      pMe->m_eGPSState = QCAM_GPS_FAILURE;
      break;
    case AEEGPS_ERR_TIMEOUT:
      pMe->m_eGPSState = QCAM_GPS_TIMEOUT;
      break;
    case AEEGPS_ERR_ACCURACY_UNAVAIL:
      if (pMe->m_nGPSAccuracy > AEEGPS_ACCURACY_LOWEST)
      {
        pMe->m_eGPSState = QCAM_GPS_ENABLED;
        pMe->m_nGPSAccuracy--;
        retry = TRUE;
      }
      else
      {
        pMe->m_eGPSState = QCAM_GPS_FAILURE;
      }
      break;
    case AEEGPS_ERR_INFO_UNAVAIL:
      pMe->m_eGPSState = QCAM_GPS_INFO_UNAVAILABLE;
      break;

    default:
      pMe->m_eGPSState = QCAM_GPS_FAILURE;
      break;
    }

    // Re-draw the screen to clear messages due to GPS operations
    if (!retry)
    {
      (void)ISHELL_HandleEvent (pMe->a.m_pIShell, EVT_COMMAND, IDS_TAKE_PIC, 0);
    }
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

  if (retry)
  {
    Qcamera_GetGPSInfo(pMe);
  }
}

#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE

#if defined (FEATURE_CAMERA_BURST_MODE) || defined (FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE)
#error code not present
#endif // FEATURE_CAMERA_BURST_MODE || FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE

#ifdef FEATURE_CAMERA_SUPPORT_ICON_ARRAY
/*************************************************************************

FUNTIONS call ipl and camera functions.
Will be replaced with BREW API calls

************************************************************************/
extern int camera_clr_icon_array(void);  //  returns camera_ret_code_type (an enum)
extern int camera_set_icon_array(uint32 p1, uint32 p2, uint32 p3, ipl_icon_type** p4);


boolean Qcamera_LoadIconArray(QCam *pMe, const IDIB *pDIB, int index)
{
  int xOffset = 20;
  int yOffset = 20;
  ipl_icon_type *pCurrentIcon;

  if (!pMe || !pDIB)
    return FALSE;

  //construct a ipl_icon_type data from pDIB
  pCurrentIcon = MALLOC(sizeof(ipl_icon_type));
  if (!pCurrentIcon)
    return FALSE;

  pCurrentIcon->x = 15 *(uint32) index + (uint32)xOffset;
  pCurrentIcon->y = 20 * (uint32)index + (uint32)yOffset;
//NC_NOTE: current display is fixed to 16 bits output. It will be updated to 18 bits
//or switched between 16 and 18. Need to update this one.
  pCurrentIcon->cFormat = IPL_RGB565;
  pCurrentIcon->dx = pDIB->cx;
  pCurrentIcon->dy = pDIB->cy;
  pCurrentIcon->pitch = pDIB->nPitch;
  pCurrentIcon->imgPtr = MALLOC((uint32)(pDIB->nPitch * pDIB->cy));
  if (!pCurrentIcon->imgPtr)
  {
    CAM_FREEIF(pCurrentIcon);
    MSG_HIGH("MALLOC FAILED",0,0,0);
    return FALSE;
  }
  ZEROAT(pCurrentIcon->imgPtr);
  (void)MEMCPY(pCurrentIcon->imgPtr, pDIB->pBmp, (uint32)(pDIB->nPitch * pDIB->cy));

  //assign the data to the iconList
  pMe->iconList[index] = pCurrentIcon;
  return TRUE;
}

boolean Qcamera_SetIconArray(QCam *pMe)
{
  AEESize *pSize;

  if (!pMe)
    return FALSE;


  pSize = Qcamera_GetSize(pMe);

  (void)camera_set_icon_array((uint32)pSize->cx,
                              (uint32)pSize->cy,
                              DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB565,
                              pMe->iconList);
  return TRUE;
}

void Qcamera_ClearIcon(QCam *pMe)
{
  int i = 0;

  if (!pMe)
    return;

  while (pMe->iconList[i])
  {
    CAM_FREEIF(pMe->iconList[i]->imgPtr);
    CAM_FREEIF(pMe->iconList[i]);
    i++;
  }

  (void)camera_clr_icon_array();
}

//This function assumes the LCD is 16 bits color format.
//The function will be re implemented once the IIPL interface is ready.
IDIB *Qcamera_ComposeStamp(QCam *pMe, IDIB *pInDIB, AEESize size)
{
  IDIB *pOutDIB = NULL;
  ipl_image_type *pFrame = NULL;
  ipl_image_type outputFrame;
  uint32 total = (uint32)(size.cx * size.cy);
  uint32 counter = 0;

  if (!pMe)
    return NULL;

  pFrame = MALLOC(sizeof(ipl_image_type));
  if (!pFrame)
    return NULL;

  if (pInDIB)
  {
    pFrame->dx = (uint32)pInDIB->cx;
    pFrame->dy = (uint32)pInDIB->cy;
    pFrame->cFormat = IPL_RGB565;
    pFrame->imgPtr = MALLOC((uint32)(pInDIB->cy * pInDIB->nPitch));
    if (!pFrame->imgPtr)
      goto ErrExit;
    (void)MEMCPY(pFrame->imgPtr, pInDIB->pBmp, (uint32)(pInDIB->cy * pInDIB->nPitch));
  }
  else
  {
    pFrame->dx = (uint32)size.cx;
    pFrame->dy = (uint32)size.cy;
    pFrame->cFormat = IPL_RGB565;
    pFrame->imgPtr = MALLOC((uint32)total * 2);
    if (!pFrame->imgPtr)
      goto ErrExit;

    //Fill in all default transtarent color
    do
    {
      *(uint16*)(pFrame->imgPtr) = DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB565;
      pFrame->imgPtr += 2;
      counter++;
    }while (counter < total);
    pFrame->imgPtr -= 2 * total;
  }

  ZEROAT(&outputFrame);
  outputFrame.imgPtr = MALLOC(total * 2);
  if (!outputFrame.imgPtr)
    goto ErrExit;

  ZEROAT(outputFrame.imgPtr);
  outputFrame.dx = pFrame->dx;
  outputFrame.dy = pFrame->dy;
  outputFrame.cFormat = IPL_RGB565;

  if (pMe->iconList[0])
  {
    //clear the old icons, the icon list will combine with the frame overlay
    (void)camera_clr_icon_array();
    // Stamps always use the default transparent pixel value
    //lint -save -e641 Converting enum to int
    if (ipl_frame_compose(pFrame, pMe->iconList, DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB565, &outputFrame) != SUCCESS)
    //lint -restore
      goto ErrExit;
  }

  //Create output DIB and return
  if (IDISPLAY_CreateDIBitmap(pMe->a.m_pIDisplay, &pOutDIB, 16, (uint16)outputFrame.dx, (uint16)outputFrame.dy) == SUCCESS)
  {
    pOutDIB->pBmp = outputFrame.imgPtr;
    pOutDIB->ncTransparent = DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB565;
    CAM_FREEIF(pFrame->imgPtr);
    CAM_FREEIF(pFrame);

    return pOutDIB;
  }

ErrExit:
  MSG_HIGH("Qcamera_ComposeStamp failed",0,0,0);
  CAM_FREEIF(pFrame->imgPtr);
  CAM_FREEIF(pFrame);
  CAM_FREEIF(outputFrame.imgPtr);
  return NULL;
}
#endif //FEATURE_CAMERA_SUPPORT_ICON_ARRAY
///////////////////////////////////////////////


#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#ifdef HEAP_CHECK
void Qcamera_ShowHeapUsage(QCam *pMe)
{
  uint32 dwCurUsage = 0;
  uint32 dwMaxUsage = 0;
  uint32 dwAppUsage = 0;

  if (!pMe)
    return;

  if (pMe->m_pIHeap)
  {
    if (IHEAP_GetModuleMemStats(pMe->m_pIHeap, AEECLSID_QCAMERA, &dwMaxUsage, &dwCurUsage)
        == SUCCESS)
    {
      MSG_HEAP("  app         %9d, app max         %9d", dwCurUsage, dwMaxUsage, 0);
      dwAppUsage = dwMaxUsage;
      mm_get_mem_stat(CAMERA_SVCS_GROUP, &dwMaxUsage, &dwCurUsage);
      MSG_HEAP("  camera svcs %9d, camera svcs max %9d", dwCurUsage, dwMaxUsage, 0);
      mm_get_mem_stat(JPEG_GROUP, &dwMaxUsage, &dwCurUsage);
      MSG_HEAP("  jpeg        %9d, jpeg max        %9d", dwCurUsage, dwMaxUsage, 0);
      mm_get_mem_stat(IPL_GROUP, &dwMaxUsage, &dwCurUsage);
      MSG_HEAP("  ipl         %9d, ipl max         %9d", dwCurUsage, dwMaxUsage, 0);
      mm_get_mem_stat(CAMERA_COMBINED, &dwMaxUsage, &dwCurUsage);
      MSG_HEAP("  svcs total  %9d, svcs total max  %9d", dwCurUsage, dwMaxUsage, 0);
      MSG_HEAP("  qcamera total %9d, overall total %9d", dwAppUsage + dwMaxUsage,
               dwAppUsage + dwMaxUsage + dwOtherUsage, 0);
    }
  }
}
#endif //HEAP_CHECK

#endif //#if defined(FEATURE_APP_QCAMERA)

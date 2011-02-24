/*===========================================================================

FILE: Qalbum.c

GENERAL DESCRIPTION
    This file contains a BREW application Qalbum. It provides functions such
    as viewing and editing photos.

EXTERNALIZED FUNCTIONS
    Qalbum_Load

      Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.


===========================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qalbum/main/latest/src/qalbum.c#10 $
 $DateTime: 2009/04/15 00:18:51 $ $Author: saigopal $

 when      who    what, where, why

 --------  -----  ----------------------------------------------------------
 04/15/09   sai   Added support to have USB MSD as the save location
 03/30/09  KC     Handling END key while loading thumbnails
 02/26/09  KC     verify m_bDecodeInProgress to see if decode is in progress before requesting for
                  a new jpeg instance
 10/28/08  KC     Handling End key press differently when images are loading in thumbnail mode.
 10/20/08  KC     Setting the qostimer to 89 sec for the standalone gpsfixes.
 08/14/08  KC     Changed default option when CLR key is pressed on Delete confirmation 
                  screen to IDS_NO instead of IDS_YES.
 07/11/08  RK     Fixed the Kernel Panic Page Fault in album overlap and alpha blend.
 04/24/08  KC     Fixed a compiler warning in Qalbum_ProceedWithGeoTagging function 
 04/10/08   KC    Fixed KLock errors 
 04/10/08  KC     Adding automation support for RGB Histogram
 03/25/08  KC     Adding Geo_tagging feature.
 03/06/08  KC     Added an other check in Qalbum_OnDescribe() to fix the klock error.
 02/13/08  SK     Undo solarize effect when it reaches max value when user press down key 
 01/25/08  KC     Fixed PNG automation failure issue.assigning the pMe->m_wKeyEdit in qalbum_DiagAlbumEdit 
 01/17/08  MSV    Changes to keep the display dimensions always Even.
 12/17/07  KV     Routinue for building Jpeg Color Format Menu.
 11/12/07  NC     Created/Released m_pMediaUtil per saving to enable cancel encoding
 10/01/07  SK     Fixed issue in building resize menu.
 09/25/07  NC     Fixed the extract thumbnail issue and save issue.
 09/17/07  NC     Freed m_pDisplayImage to save memory on view/edit full image
                  Updated logic for Save, Property, Description and Extra thumbnail.
 09/14/07  KD     Removed deprecated function calls
 07/31/07  TV     Suspend/Resume now frees up m_pDisplayImage, if we think it is safe.
 07/31/07  KV     Added condition FEATURE_CAMERA7500 for QCAM_VGA_WIDTH.
 07/30/07  KV     Fixed Icon Text Overlap Issue for VGA and WVGA displays.
 06/29/07  NC     Returned to Adv Edit screen on cancel select 2nd image.
 06/29/07  NC     Removed up arrow in some edit features when reaching to max step.
 06/19/07  NC     Removed unnecessary resize call in init zoom.
 06/18/07  NC     Fixed bugs in zoom feature.
 06/08/07  NC     Fixed some automation report issues.
 06/04/07  RR     Fix : Unable to view more than 4 frames in the add-frame list
 05/22/07  SK     JPEG Decoder Resizing feature
 05/24/07  NC     Fixed wrong resourse settings for WVGA screen
 05/21/07  RK     Fixed Posterize effect will not be decreased after increasing.
 05/11/07  NC     Fixed bug in handling different color format.
 04/30/07  NC     Fixed compiler warnings.
 04/25/07  NC     Report "Failed to print image" message on screen if failed printing.
 04/23/07  NC     Fixed some lint errors.
 04/20/07  SK     UI Support for WVGA Display
 04/20/07  NC     Report OK to automation test for editing in view mode.
 04/12/07  NC     Fixed error handling bugs.
 04/09/07  NC     Added F3 message for debug help.
 03/23/07  KV     Changed from default font size to dynamic font size to fit the pitch of the display.
 03/23/07  NC     Fixed compiler error on some feature conditions.
 03/22/07  NC     Voted no sleep for slide show to workaround unexpected delay.
 02/23/07  TV     Added ability to print with PBPrint
 02/05/07  ZL     Dropped key events when loading images and in SAVE state.
 01/24/07  ZL     Made Qalbum_DiagReportErr an external function.
 01/05/07  ZL     Enabled JPEG decoder resizing only in THUMBNAIL state.
 12/22/06  ZL     Fixed memory leak when overlaying JPEG images.
 12/22/06  ZL     Changed some F3 messages from MED to HIGH for performance checking.
 12/20/06  RK     Fixed the crash when we add description upto 5lines and go to properties.
 12/20/06  ZL     Turned on flag m_bMsgTimerOn in Qalbum_DisplayPhotoSavedMsg().
 12/06/06  RK     Fixed the crash when we pressed up/down key in properties.
 12/05/06  ZL     Added support for FEATURE_JPEGD_RESIZE.
 12/04/06  NC     Fixed compiler error with IMMOverlay & FEATURE_CAMERA_MULTIPLE_DISPLAY.
 11/21/06  NC     Replace IOverlay with IMMOverlay for BREW 4.0.
 11/15/06  RK     Fixed a Crash when extract a thumbnail then got to Properties.
 11/17/06  TV     Updated DisplayTitle_SM to handle Nav arrows.
 11/14/06  ZL     Added automation test workaround with the addition of
                  the new VIEW state.
 11/12/06  NC     Fixed a bug in display MMC directory name.
 11/09/06  NC     Fixed compiler errors when FEATURE_CAMERA_MULTIPLE_DISPLAY is defined.
 11/09/06  ZL     Added a new VIEW state before EDIT state.
 11/08/06  ZL     Moved editing feature to qalbumEdit.c.
 11/02/06  NC     Replace Qalbum_SetOvObject with QCamDisp_DisplayOvOnTV.
 11/01/06  NC     Fixed bug in Qalbum_DiagGetMatchedFile
 10/30/06  ZL     Added Qalbum_BuildEditList() and Qalbum_DisplayListNavArrows().
 10/27/06  NC     Removed IOverlay register feature and optimized codes.
 10/09/06  NC     Added Multiple Display feature.
                  Used generic resource file name and menu height for target compatible.
                  Optimize automation support features.
 09/25/06  NC     Added F3 messages for performance checking.
 09/21/06  NC     Optimized automation status report logic and fixed bug in handling error.
 09/18/06  NC     Fixed lose focus issue on returning to thumbnail after some advanced editing.
 09/14/06  RK     Fixed the following issues.
                  1.Images not be displayed in album after alpha blend or overlap operation
                  2.Bad image generated after keep selecting extract thumbnail.
 09/13/06  NC     Fixed lint errors.
 08/31/06  NC     Fixed a typo and workaround target dependency of BPP print feature
 08/25/06  NC     Fixed a bug in Qalbum_ImageNotify for error check.
 08/24/06  NC     Added SXGA/UXGA frame overlay support and memory issue handlers.
 08/18/06  RK     Added a New Helper function Qalbum_DisplayError() for Error checking.
 07/17/06  RK     Fixed the Key toggeling issue in Solarize screen .
 07/14/06  NC     Fixed lint errors and added more F3 messages in OnPrint function.
 06/19/06  NC     Added FEATURE_BT_EXTPF_BPP condition for BPP print feature
 06/14/06  JR     Added support for FTL memory.
 05/22/06  NC     Fixed compiler warning when FEATURE_APP_MMS is not defined.
 05/19/06  NC     Revert message timer to 2 seconds. It is too short for long message.
 05/17/06  NC     Enabled timer for memory full message. Blocked all keys except CLR
                  when error message on edit state. Shorten message time to 1 second.
 05/01/06  NC     Filter out unused features to save memory space.
 04/20/06  HV     Updated Qalbum_OnSend() to skip calling QCam_SendPicture()
                  if FEATURE_APP_MMS is not defined
 04/05/06  HV     Integrating IYCbCrFactory on Qalbum
 03/31/06  NC     Called QCam_ParseArgString() to parse the argument string.
                  Free m_pLaunchDS except Frame mode.
 03/27/06  NC     Fixed compiler warning when DIAG feature is turned off.
 03/23/06  NC     Added parameter IIpl to QCamDisp_DrawYCbCrImage()
 03/20/06  NC     Added VGA frame overlay support.
 02/23/06  NC     Added helper functions for Zoom and Pan logic update.
 02/13/06  NC     Added automation support for Whiteboard and Histogram.
 02/08/06  NC     Reported automation error on abnormal termination of test.
 01/05/06  NC     Fixed a potential memory leak issue.
 01/05/06  JB     Added small screen support.
 12/29/05  NC     Add return values for Qalbum_DiagAdvAlbumEdit Property.
 12/07/05  JN     Eliminate file copy from BT print option.
 12/07/05  NC     Fixed a bug on getting 2nd file name for automation test.
 12/01/05  NC     Return TRUE after active report sent for automation test.
 11/15/05  JN     Do not reset bPNGFile flag for frames.
 11/08/05  NC     Update per the new header file.
 11/08/05  JN     Edit handler proceeds only if image is successfully loaded.
 11/04/05  NC     Added helper function to simplify codes and added more error returns
                  to automation test.
 10/24/05  JN     2nd round of lint clean up.
 10/18/05  JN     Release directory, photo and frame list when exiting.
 10/18/05  NC     Fixed a bug in Qalbum_DiagAlbumEdit.
 10/13/05  NC     Updated Qalbum_DiagAdvAlbumEdit to get input parameters from payload.
                  Fixed lint error from last change.
 10/12/05  SM     automation support for prompt to save/delete
 10/06/05  NC     stop and restart slide show on app suspend and resume.
 09/28/05  JN     Delay extract thumbnail until used.
 09/16/05  JN     Lint error clean up.
 09/13/05  NC     Added checking to draw image returns and report errors to automation.
 09/13/05  JN     Removed m_pEditTemp from Qalbum struct. Lint clean up.
 09/12/05  NC     Fixed a compilation error when automation feature is not defined.
 09/07/05  NC     Added AddFrame automation support.
 08/17/05  JN     Redesigned Qalbum to decouple it from AMSS.
 07/26/05  JN     Don't use m_pDisplayImage for loading frames because it
                  causes m_pDisplayImage freed after loading a frame.
 07/20/05  NC     Added Advanced Edit to Edit view automation support
 07/18/05  JN     Replaced all OEMJPEG with IIMAGE APIs.
 06/29/05  JN     Replaced OEMJPEG_GetImageData with IIMAGE_SetParm.
 06/21/05  NC     Added zoom and pan automation feature.
 06/20/05  NC     Added advanced edit automation feature.
 06/17/05  JN     Show memory full message if edit buffer cannot be allocated.
                  Redraw edit image upon app resume.
 06/16/05  JN     New feature to print photo through BT app.
 06/09/05  NC     Update QCam_BuildDirList() call per parm change.
 06/09/05  JN     Combined OEMJPEG_GetJPEG and OEMJPEG_extractThumbnail.
 06/07/05  JN     PNG image still uses ISHELL_LoadImage interface unlike JPEG.
                  Do not allow adding description to PNG images.
 05/27/05  JN     Support loading Jpeg images in different modes: RGB or YCbCr,
                  main or thumbnail.
 05/26/05  NC     Make sure we get valid index before calling mapping function.
 05/26/05  NC     Free pLaunchDS only when it is created by Qalbum self.
 05/23/05  JN     Centralize font definitions.
 05/20/05  JN     Use IStatic interface to show properties.
 05/18/05  JN     Show GPS info in properties menu.
 05/18/05  NC     Included date path in file name display for MMC image.
 05/16/05  JN     Fixed a bug when No Frames message doesn't go away.
 05/16/05  NC     Bring up the menu button on dialog start up after BREW 3.1.3.
 05/12/05  JN     Remove freeing image buffer in save dialog.
                  In rotate, initialize nEditProcessedVal if out of range.
                  Show busy image for slow IPL functions.
 05/10/05  JN     New feature to add image description. Moved save to qalbumDlg.c.
 05/02/05  JN     Display Exif info under properties menu.
 04/28/05  NC     Added Qalbum_DiagGetContext() to support GetContext test.
 04/28/05  JN     Allow Qalbum to be launched from MoreApps with default settings.
 04/25/05  JN     Panning step size is set to 1/4 of the display size.
                  IImage handle is not released after loading, rather keep it
                  until loading next IImage, this allows access to exif header.
                  Exif header is encoded into the edited images.
 04/15/05  NC     Change automation simulate key event definition.
 04/15/05  JN     In frame option bEditProcessed flag is not set properly.
                  Show error msg when frame size don't match photo size.
 04/14/05  NC     Added error status report to automation test for all failure cases.
                  Added MMC memory check for space checking.
                  Fixed bugs in automation test and some features.
 04/11/05  JN     Added zoom and pan option.
 03/30/05  NC     Draw timed message to exit app. if last photo is deleted.
 03/28/05  NC     Added a new function Qalbum_GetMatchFrameDir() and updated
                  Qalbum_DisplayFrames to rebuild the frame directory
                  based on image size.
                  Added a new message "No Frames".
 03/25/05  JN     Support save to PNG.
 03/22/05  JN     Use pitch field to determine image width.
 03/21/05  JN     Use ipl_convert_image master api. Add condition check before
                  free buffer in Qalbum_UpdateEditedImage.
 03/14/05  NC     Fixed a display bug in second access to the frame mode.
 03/15/05  JN     Convert odd width image to even width for RGB2YCbCr.
 03/14/05  JN     Disallow editing of 2 images with different dimensions.
                  Add new params for 2nd image file.
 03/09/05  NC     Updated function call Qcam_BuildDirList() with new parm.
 03/02/05  JN     Support for Jpeg output YCbCr format.
 02/25/05  JN     Definition of image size has changed to portrait mode.
 02/17/05  JN     Better error handling when ISHELL_LoadImage return NULL pointer.
                  Release DIB for PNG, but not for JPEG because JPEG_GetImageData
                  doesn't malloc.
 02/14/05  JN     Cleaned up some lint errors.
 02/09/05  JN     Added a state check in Qalbum_EditPhotoSaved().
 01/28/05  NC     Code clean up. Created a shared function Qalbum_BuildEditMenu().
 01/21/05  JN     Display bad photo icon when failed to load thumbnail image.
 01/17/05  JN     Horizontal, vertical and center comp min boundary is 0.
 01/14/05  NC     Use IOverlay for MDP feature.
 01/05/05  JN     Updated advanced edit options.
 01/05/05  NC     Code clean up.
 12/30/04  NC     Fixed potential UI freeze problem on save to JPEG file.
 12/20/04  NC     Removed color format conversion function. Let OEM handle it.
 12/16/04  JN     Don't reset EditProcessedVal in UpdateInIPLImage.
                  Set EditProcessedVal in DisplayEditPicMenuName.
 12/16/04  JN     Thumbnail size should be QVGA.
 12/10/04  NC     Update auto test handler and fix bugs.
 12/07/04  NC     Added DIAG features for Auto Test.
 12/06/04  JN     DisplayDIB need to be updated when doing resize.
 11/29/04  JN     DisplayDIB need to be updated when doing rotation.
 11/24/04  JN     Do not reset soft menu at property screen.
 11/23/04  JN     Changed encoding format from YCbCr420LP to YCrCb420LP.
 11/22/04  NC     Redesign Qalbum_AddImageFrame() to use QCam_LoadPNGFrame().
 11/19/04  JN     Added extract thumbnail option.
 11/15/04  JN     Create thumbnail along with main image when saving.
 11/10/04  JN     Update file name when saving a file.
 11/09/04  NC     Fixed bugs in frame navigation and frame image notify
 11/05/04  JN     Changed QCamDisp_DrawSelect() to pass in old rect value.
 11/02/04  NC     Showed image on the save dialog and enable save image feature
 11/02/04  NC     Fixed a bug in display edit menu title
 11/02/04  JN     Load full image instead of thumbnail for editing.
 10/29/04  NC     Implemented Qalbum_AddImageFrame and fixed couple bugs.
 10/29/04  JN     Fix image resize handler.
 10/28/04  JN     Added busy indicator support for loading full image.
 10/27/04  NC     Display PNG files and fixed couple bugs.
 10/22/04  JN     Implement EditPicUndo().
 10/21/04  NC     Updated Focus feature based on BREW 3.1.2.
 10/21/04  JN     Reset edit menu select whenever edit menu is rebuilt.
 10/21/04  JN     Delete photolist whenever directory changes.
 10/20/04  NC     Fixed bug that causes wrong file name is displayed.
                  Modified DisplayPhoto logic to build the photo list only when it is NULL.
                  Updated DeleteFileResponse to update the file list after deleting an item.
 10/19/04  JN     QCamViewer -> QCamAlbum -> Qalbum.
 10/15/04  JN     Removed CAM_ReleaseIf() of image and dib within QCamV_ImageNotify().
 10/14/04  JN     Register full screen to MDP at init.
 10/14/04  JN     Changed QCam_BuildDirList() to pass in QCam_FileList **, rather than
                  QCam_FileList *.
 09/21/04  NC     Add more features and redesign function names.
 06/16/04  NC     Initial design of the photo viewer and editor.

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
#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP)
#include "BTApp_priv.h"		//"BTApp.h"

#endif //defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP)
#endif

#if defined(FEATURE_APP_QALBUM)
#include "qalbum_i.h"
#include "QcamDef.h"

#include "JPEG.BID"
#include "IPL.BID"
#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP)
#include "BTApp.bid"
#include "AEEBTExt.h"
#endif //defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP)

#ifdef HEAP_CHECK
#include "AEEHeap.h"
#include "malloc_mgr.h"
#define MSG_HEAP MSG_ERROR
static uint32 dwOtherUsage = 0;
#endif

#ifdef FEATURE_JPEGD_RESIZE
/* Macro to Convert Unsigned Odd numbers to Even numbers by subtracting one from the ODD number */
#define ODD_2_EVEN(x) if(x & 0x1) { x = x & ~0x1; } 
#endif
/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*=========================================
static application
=====================================*/

#if defined(BREW_STATIC_APP)
int Qalbum_CreateInstance(AEECLSID ClsId, IShell *pIShell, IModule * o, void **ppObj);
int Qalbum_Load(IShell *ps, void *pHelpers, IModule **pMod);

//Static Data...
static AEEAppInfo gaiQalbum = {AEECLSID_QALBUM, QALBUM_RES_FILE,
                               IDS_TITLE, 0, 0, 0, 0, AFLAG_TOOL};


PFNMODENTRY Qalbum_GetModInfo(IShell *ps, AEECLSID **ppClasses, AEEAppInfo **pApps,
                              uint16 *pnApps, uint16 *pwMinPriv)
{
  *pApps = (AEEAppInfo *)&gaiQalbum;
  *pnApps = 1;
  return((PFNMODENTRY)Qalbum_Load);
  //lint -save -e715 -e818 ps, pwMinPriv and ppClasses not referenced, function prototype is fixed
}
//lint -restore

int Qalbum_Load(IShell *ps, void *pHelpers, IModule **pMod)
{
  return(AEEStaticMod_New((int16)(sizeof(AEEMod)), ps, pHelpers, pMod,
                          Qalbum_CreateInstance, NULL));
}

#endif  //BREW_STATIC_APP

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
int Qalbum_CreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
#endif

{
  if (!ppObj)
  {
    return EFAILED;
  }

  *ppObj = NULL;

  if (ClsId == AEECLSID_QALBUM)
  {
    if (AEEApplet_New(sizeof(Qalbum), ClsId, pIShell, po, (IApplet**)ppObj,
                      (AEEHANDLER)Qalbum_HandleEvent, (PFNFREEAPPDATA)Qalbum_FreeAppData))
    {
      if (Qalbum_InitData((Qalbum *) *ppObj) == TRUE)
        return(AEE_SUCCESS);
    }
  }
  return(EFAILED);
}

/*===========================================================================

  FUNCTION: Qalbum_InitData

   DESCRIPTION:
    This function initializes app specific data.

  PROTOTYPE:
    boolean QCam_InitAppData(Qalbum* pMe);

  PARAMETERS:
      pi [in]: Pointer to the Qalbum structure. This structure contains
      information specific to this applet. It was initialized during the
      AEEClsCreateInstance() function.

  DEPENDENCIES:
     Assumes pMe is not NULL

  RETURN VALUE
   TRUE: The app has app data is initialized successfully

  SIDE EFFECTS:
  none
===========================================================================*/
static boolean Qalbum_InitData (Qalbum * pMe)
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
      MSG_HEAP("Qalbum_InitData: total heap %d, other usage %d",
               deviceInfo.dwRAM, dwOtherUsage, 0);
      Qalbum_ShowHeapUsage(pMe);
   }
#endif

  Qalbum_GetDisplaySizes(pMe);
  pMe->m_pISoftMenu = NULL;
  pMe->m_pIMenu = NULL;
  pMe->m_pIFileMgr = NULL;
  pMe->m_pMediaUtil = NULL;
  pMe->m_pDisplayImage = NULL;
  pMe->m_pFrameImage = NULL;
  pMe->m_pFrameDIB = NULL;
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
  pMe->m_pAlbum = NULL;
  pMe->m_coreUIClsId = 0;
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  pMe->m_pLaunchDS = NULL;

  pMe->m_pDirList   = NULL;
  pMe->m_pPhotoList = NULL;
  pMe->m_pFrameList = NULL;
  pMe->m_pCurrentDir = NULL;
  pMe->m_pCurrentPhoto = NULL;
  pMe->m_p2ndPhoto     = NULL;
  pMe->m_pCurrentFrame = NULL;
#ifdef  FEATURE_GEOTAGGING
  pMe->m_pIPosDet = NULL;
  pMe->m_nGPSAccuracy = AEEGPS_ACCURACY_HIGHEST;
  ZEROAT(&pMe->GPSInfoCB);
#endif /*FEATURE_GEOTAGGING*/

  pMe->m_nCurrentPhoto = 1; //1st one
  pMe->m_n2ndPhoto     = 1;
  pMe->m_nCurrentDir = 1; //1st one
  pMe->m_nCurrentFrame = 1;
  pMe->m_nStartPhoto = pMe->m_nCurrentPhoto;
  pMe->m_nStartDir = pMe->m_nCurrentDir;
  pMe->m_nStartFrame = pMe->m_nCurrentFrame;
  pMe->m_nTotalPhotos = 0;
  pMe->m_nTotalDirs = 0;
  pMe->m_nTotalFrames = 0;
  pMe->m_sFrameFileName[0] = '\0';
  pMe->m_sFrameDir[0] = '\0';

  pMe->m_sCurrentFileName[0] = '\0';
  pMe->eState = QCAM_STATE_NULL;
  pMe->m_pEditIn  = NULL;
  pMe->m_pEditIn2 = NULL;
  pMe->m_pEditOut = NULL;
  pMe->m_pZoomImg = NULL;
  pMe->m_pEditThumbnail = NULL;
  pMe->m_bIs2ndPhoto = FALSE;
  pMe->m_bEditProcessed = FALSE;
  pMe->m_wEditProcessedBy = 0;
  pMe->m_bDisplayFrames = FALSE;
  pMe->m_bAutoPlayInit = FALSE;
  pMe->m_bIsSlideShowStarted = FALSE;
  pMe->m_bPNGFile = FALSE;
  pMe->m_bMsgTimerOn = FALSE;
  pMe->m_bThumbnailFlag = FALSE;
  pMe->m_bStopLoadingThumbnails = FALSE;
  pMe->m_bImagesLoading = FALSE;
  pMe->m_bDecodeInProgress = FALSE;
  pMe->m_bAspectRatio = TRUE;
#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
  pMe->m_ePrintMenuActive = MAX_PRINT_NUM;
  if (pMe->m_ePrintMenuActive > 2)
  {
    // There are at least two print options, we cannot default to one of them
    pMe->m_ePrintMenuActive = NO_BT_PRINT_ASSIGNED;
  }
  else
  {
    // We only have one print option
#if defined(FEATURE_BT_EXTPF_BPP) && !defined(FEATURE_SKIP_BPP)
    pMe->m_ePrintMenuActive = BPP_BT_PRINT;
#endif
#ifdef FEATURE_BT_EXTPF_BIP
    pMe->m_ePrintMenuActive = BIP_BT_PRINT;
#endif
#if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))
    pMe->m_ePrintMenuActive = PICTBRIDGE_PRINT;
#endif
  }
#endif //#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)

  ZEROAT(&pMe->m_EncodeResult);
  ZEROAT(&pMe->m_EncodeCB);

  ZEROAT(&pMe->m_SelectRC);

  pMe->m_JpegColorFormat = QALBUM_COLOR_FORMAT_RGB565; // default to RGB565
  pMe->m_bZoom    = FALSE;
  pMe->m_bMaxZoom = FALSE;

  return TRUE;
}

/*===========================================================================

  FUNCTION: Qalbum_Init

   DESCRIPTION:
    This function initializes app specific data, allocates memory for app
    data. App developer needs to fill-in the initialization code.

  PROTOTYPE:
    boolean QCam_Init(Qalbum* pi);

  PARAMETERS:
      pMe - Pointer to Qalbum struct

  DEPENDENCIES:
     Assumes pMe is not NULL

  RETURN VALUE
   TRUE: If the app has app data is allocated and initialized successfully
   FALSE: Either app data could not be allocated or initialzied

  SIDE EFFECTS:
  none
===========================================================================*/
static boolean Qalbum_Init (Qalbum * pMe)
{
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  if (!pMe)
    return FALSE;

  if (ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_FILEMGR,
                            (void **)&pMe->m_pIFileMgr) != SUCCESS)
  {
    MSG_HIGH("FileMgr Creation failed", 0, 0, 0);
    return FALSE;
  }

   //Create instance for pMe->m_pIPosDet and register for GetGPSInfo callback
  //This interface is for get GPS information only, if it is failed to get the
  //interface, we still start the camera app. But need to check the pointer
  //whenever use it.
#ifdef FEATURE_GEOTAGGING
  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_POSDET,
                               (void **)&pMe->m_pIPosDet) != SUCCESS)
  {
    MSG_MED( "Failed to create IPOSDET ", 0, 0, 0);
	goto CleanExit;
  }
  else
  {
    CALLBACK_Init(&pMe->GPSInfoCB, Qalbum_GetGPSInfoCB, pMe);
  }
#endif /* FEATURE_GEOTAGGING*/

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL,
                            (void **) &pMe->m_pISoftMenu) != SUCCESS)
  {
    MSG_HIGH("m_pISoftMenu Creation failed", 0, 0, 0);
    goto CleanExit;
  }

  if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL,
                            (void **) &pMe->m_pIMenu) != SUCCESS)
  {
    MSG_HIGH("m_pIMenu Creation failed", 0, 0, 0);
    goto CleanExit;
  }


#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
  if (pMe->m_pLaunchDS->bMultiDisplayMode)
  {
    pMe->m_coreUIClsId = QCamDisp_GetOldDispClsId(pMe->a.m_pIShell);
    //Disable DISPLAY3 display link on start up
    if (QCamDisp_ChangeDisplay(pMe->a.m_pIShell, AEECLSID_DISPLAY3, 0) != SUCCESS)
    {
      MSG_HIGH("Failed QCamDisp_ChangeDisplay.", 0, 0, 0);
    }
  }
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_YCBCR,
                           (void **)&pMe->m_pIYCbCrFactory) != SUCCESS)
  {
    MSG_HIGH("IYCBCR Creation Failed", 0, 0, 0);
    goto CleanExit;
  }

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IPL,
                           (void **)&pMe->m_pIIpl) != SUCCESS)
  {
    MSG_HIGH("IIPL Creation Failed", 0, 0, 0);
    goto CleanExit;
  }

  QCam_SetMenuStyle(pMe->m_pISoftMenu, 0, 0);
  QCam_SetMenuStyle(pMe->m_pIMenu, 2, 0);
  IMENUCTL_SetProperties(pMe->m_pISoftMenu, MP_MAXSOFTKEYITEMS);
  return TRUE;

CleanExit:
  CAM_RELEASEIF(pMe->m_pIFileMgr);
  CAM_RELEASEIF(pMe->m_pISoftMenu);
  CAM_RELEASEIF(pMe->m_pIMenu);
#ifdef FEATURE_GEOTAGGING
  CAM_RELEASEIF(pMe->m_pIPosDet);
#endif /*FEATURE_GEOTAGGING*/
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  CAM_RELEASEIF(pMe->m_pIYCbCrFactory);
  CAM_RELEASEIF(pMe->m_pIIpl);

  return FALSE;
}

/*===========================================================================
FUNCTION Qalbum_HandleStartApp

  DESCRIPTION
    This function starts the application based on the launch mode.
    If launch mode is:
    VIEWER_MODE_FRAME - eState is QCAM_STATE_NULL. Display frame thumbnails.
    VIEWER_MODE_SLIDESHOW - eState is QCAM_STATE_SLIDESHOW, Start auto play photos.
    VIEWER_MODE_ALBUM - eState is QCAM_STATE_DIR. Display date folder thumbnails.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    dwParam - address to AEEAppStart struct.

  RETURN VALUE:
   None
===========================================================================*/
static boolean Qalbum_HandleStartApp(Qalbum * pMe, uint32 dwParam)
{
  AEEAppStart *args = (AEEAppStart *)dwParam;

  if (!pMe)
    return FALSE;

  if (args && args->pszArgs)
  {
    //Parse the argument string into the m_pLaunchDS.
    pMe->m_pLaunchDS = QCam_ParseArgString(args->pszArgs);
  }

  //If m_pLaunchDS is NULL, we need to set some default values.
  if (!pMe->m_pLaunchDS)
  {
    // Assume default setting if launched from MoreApps
    pMe->m_pLaunchDS = (Qalbum_LaunchDS *)MALLOC(sizeof(Qalbum_LaunchDS));
    if (!pMe->m_pLaunchDS)
      return FALSE;

    pMe->m_pLaunchDS->mode             = VIEWER_MODE_ALBUM;
    pMe->m_pLaunchDS->nSaveLoc         = QCAM_SAVE_FLASH;
    pMe->m_pLaunchDS->bDirSortAscend   = FALSE;
    pMe->m_pLaunchDS->bPhotoSortAscend = FALSE;
    pMe->m_pLaunchDS->bMultiDisplayMode = FALSE;
  }

  MSG_HIGH("Qalbum_HandleStartApp, mode %d", pMe->m_pLaunchDS->mode, 0, 0);
  if (!Qalbum_Init(pMe))
    return FALSE;

  if (pMe->m_pLaunchDS->mode == VIEWER_MODE_FRAME)
  {
    Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);
    pMe->eState = QCAM_STATE_NULL;

    (void)STRLCPY(pMe->m_sFrameDir, QCAM_FRAMES_DIR, FILE_NAME_SIZE);
    (void)STRLCAT(pMe->m_sFrameDir, pMe->m_pLaunchDS->sFrameSizeDir, FILE_NAME_SIZE);
    MSG_HIGH("Start Qalbum_DisplayFrames in %s", pMe->m_sFrameDir, 0, 0);
    Qalbum_DisplayFrames(pMe, TRUE, TRUE);
  }
  else
  {
    //build the image file list
    MSG_HIGH("Start QCam_BuildDirList", 0, 0, 0);
    pMe->m_nTotalDirs = QCam_BuildDirList(pMe->m_pIFileMgr,
                                          &pMe->m_pDirList,
                                          pMe->m_pLaunchDS->bDirSortAscend,
                                          FALSE,
                                          LOC_ALL);
    MSG_HIGH("QCam_BuildDirList returns %d directories", pMe->m_nTotalDirs, 0, 0);
    if (pMe->m_nTotalDirs)
    {
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
      IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
      pMe->m_nCurrentPhoto = 1;
      pMe->m_nStartPhoto = 1;
      pMe->m_nCurrentDir = 1;
      pMe->m_nStartDir = 1;
      if (pMe->m_pLaunchDS->mode == VIEWER_MODE_SLIDESHOW)
      {
#ifdef  FEATURE_JPEGD_RESIZE
        if (pMe->m_pLaunchDS->bSlideshowResize)
        {
          pMe->eState = QCAM_STATE_SLIDESHOW;
          pMe->m_bAutoPlayInit = TRUE;
          if (!Qalbum_BuildDialog(pMe, IDD_SLIDE_SHOW, IDM_SLIDESHOW_RESIZE,
                                  IDC_RESIZE_SAVE))
          {
            MSG_HIGH("Failed to create slideshow dialog.", 0, 0, 0);
            Qalbum_AutoPlay(pMe);
          }
        }
        else
#endif  //FEATURE_JPEGD_RESIZE
        {
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
          if (pMe->m_pLaunchDS->bMultiDisplayMode)
          {
            //enable display link to AEECLSID_DISPLAY3 for slide show
            if (QCamDisp_ChangeDisplay(pMe->a.m_pIShell, AEECLSID_DISPLAY3, 1) != SUCCESS)
            {
              MSG_HIGH("Failed QCamDisp_ChangeDisplay.", 0, 0, 0);
            }
          }
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
          pMe->eState = QCAM_STATE_SLIDESHOW;
          pMe->m_bAutoPlayInit = TRUE;
          MSG_HIGH("Start playing slideshow", 0, 0, 0);
          Qalbum_AutoPlay(pMe);
        }
      }
      else  //single play mode
      {
        pMe->eState = QCAM_STATE_DIR;
        MSG_HIGH("Start Qalbum_DisplayPhotoDirs", 0, 0, 0);
        Qalbum_DisplayPhotoDirs(pMe, TRUE);
      }
    }
    else  //no photo
    {
      pMe->eState = QCAM_STATE_NULL;
      Qalbum_DisplayTimedImg(pMe, (PFNNOTIFY)Qalbum_Exit, IDB_NO_PHOTO);
    }
  }
  return TRUE;
}

/*===========================================================================
FUNCTION Qalbum_Exit

  DESCRIPTION
    This function exits the current state.

  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
   None
===========================================================================*/
static void Qalbum_Exit(Qalbum * pMe)
{
  if (!pMe)
    return;

  (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
  pMe->m_bMsgTimerOn = FALSE;
  //If no frame on thumbnail mode, return to edit state
  //else it is in start up, exit the application
  if (pMe->eState == QCAM_STATE_THUMBNAIL)
    (void)Qalbum_ThumbnailKeyHandler(pMe, EVT_KEY, (uint16)AVK_CLR);
  else if (pMe->eState == QCAM_STATE_VIEW)
    (void)Qalbum_ViewKeyHandler(pMe, EVT_KEY, (uint16)AVK_CLR);
  else if (pMe->eState == QCAM_STATE_EDIT)
    (void)Qalbum_EditKeyHandler(pMe, EVT_KEY, (uint16)AVK_CLR);
#ifndef FEATURE_IPL_LITE
  else if (pMe->eState == QCAM_STATE_ADV_EDIT)
    (void)Qalbum_AdvEditKeyHandler(pMe, EVT_KEY, (uint16)AVK_CLR);
#endif //FEATURE_IPL_LITE
  else
    (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
}

/*===========================================================================
FUNCTION Qalbum_MapDialogHandlers

  DESCRIPTION
    Handler for mapping event to dialog

  PARAMETERS:
    pMe     - Pointer to  Qalbum struct
    dwParam - Dialog object pointer
    wParam  - Dialog id

  RETURN VALUE:
    TRUE if event handled
===========================================================================*/
static boolean Qalbum_MapDialogHandler(Qalbum *pMe, uint32 dwParam, uint16 wParam)
{
  switch (wParam)
  {
  case IDD_SAVE_PIC:
    IDIALOG_SetEventHandler((IDialog*)dwParam, Qalbum_DlgSaveEventHandler, pMe);
    break;
  case IDD_DESCRIBE:
    IDIALOG_SetEventHandler((IDialog*)dwParam, Qalbum_DlgDescribeEventHandler, pMe);
    break;
#ifdef  FEATURE_JPEGD_RESIZE
  case IDD_SLIDE_SHOW:
    IDIALOG_SetEventHandler((IDialog*)dwParam, Qalbum_DlgSlideShowEventHandler, pMe);
    break;
#endif //FEATURE_JPEGD_RESIZE
  default:
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION Qalbum_HandleEvent

DESCRIPTION
   This is the EventHandler for this app. All events to this app are handled in this
   function. All APPs must supply an Event Handler.

PROTOTYPE:
   boolean QCamera_HandleEvent(Qalbum * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
   pMe: Pointer to the AEEApplet structure. This structure contains information specific
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
static boolean Qalbum_HandleEvent(Qalbum * pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  if (!pMe)
    return FALSE;

  switch (eCode)
  {
  case EVT_APP_START:
    MSG_HIGH("Received EVT_APP_START event", 0, 0, 0);
    return Qalbum_HandleStartApp(pMe, dwParam);

  case EVT_APP_RESUME:
    MSG_HIGH("Received EVT_APP_RESUME event", 0, 0, 0);

    if (pMe->eState == QCAM_STATE_THUMBNAIL)
    {
      MSG_HIGH("Resume thumbnail screen", 0, 0, 0);
      Qalbum_DisplayPhoto(pMe, TRUE);
    }
    else if (pMe->eState == QCAM_STATE_VIEW ||
             pMe->eState == QCAM_STATE_EDIT ||
             pMe->eState == QCAM_STATE_ADV_EDIT)
    {
      MSG_HIGH("Resume editing image", 0, 0, 0);
      Qalbum_UpdateEditedImage(pMe);
    }
    else if(pMe->eState == QCAM_STATE_SLIDESHOW)
    {
      //resume slide show
      MSG_HIGH("Resume slideshow", 0, 0, 0);
      Qalbum_AutoPlay(pMe);
    }
    return TRUE;

  case EVT_APP_STOP:
     MSG_HIGH("Received EVT_APP_STOP event", 0, 0, 0);

    if (pMe->m_bImagesLoading)
    {
      MSG_HIGH("Image loading...so push the app to background", 0, 0, 0);
      //put the key in queue so it will be executed after loading done
      pMe->m_bStopLoadingThumbnails = TRUE;
      pMe->m_wKeyPressed = (uint16)AVK_END;
      *(boolean *)dwParam = FALSE;
    }       
    return TRUE;

  case EVT_APP_SUSPEND://one case: if there is incoming call
    MSG_HIGH("EVT_APP_SUSPEND in state %d", pMe->eState, 0, 0);
    if(pMe->eState == QCAM_STATE_SLIDESHOW)
    {
      MSG_HIGH("Suspend slideshow", 0, 0, 0);
    }
    if (pMe->m_pDisplayImage)
    {
      //stop slide show.
      IIMAGE_Notify(pMe->m_pDisplayImage, NULL, pMe);
    }
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    return TRUE;

  case EVT_APP_NO_SLEEP:
    if (pMe->eState == QCAM_STATE_SLIDESHOW)
    {
      MSG_HIGH("Vote no sleep for slideshow", 0, 0, 0);
      return TRUE;
    }
    return FALSE;

  case EVT_DIALOG_INIT:
    MSG_HIGH("EVT_DIALOG_INIT - Map dialog handler", 0, 0, 0);
    if(Qalbum_MapDialogHandler(pMe, dwParam, wParam))
    {
      IDIALOG_SetProperties((IDialog *)dwParam, DLG_HANDLE_ALL_EVENTS);
      return TRUE;
    }
    else
      return FALSE;

  case EVT_DIALOG_START:
    return TRUE;

  case EVT_DIALOG_END:
    MSG_HIGH("Dialog end", 0, 0, 0);
    return TRUE;

  case EVT_KEY:
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
    /* EVT_QALBUM_DIAG_USER_KEY is defined to be in the User event range. The purpose
    is to overcome a BREW limitation of not being able to send EVT_KEY using
    SendEvent, PostEvent or PostEventEx. These can only be used for EVT_COMMAND
    and above */
    MSG_HIGH("Key event %d, wParam = %d, state = %d", eCode, wParam, pMe->eState);
    if (pMe->eState != QCAM_STATE_SAVE)
    {
      return CKeyHandlers[pMe->eState].HandlerFunc(pMe, EVT_KEY, wParam);
    }
    else if (wParam == (uint16)AVK_CLR || wParam == (uint16)AVK_END)
    {
      //Cancel save on CLR key
      if (pMe->m_pMediaUtil)
      {
        //Release the object that is created in Qalbum_SaveJpegImage
        CAM_RELEASEIF(pMe->m_pEditThumbnail);
        CAM_RELEASEIF(pMe->m_pMediaUtil);
      }
      //Back to view state
      pMe->eState = QCAM_STATE_VIEW;
      Qalbum_ViewPhoto(pMe, IDS_SAVE, FALSE);
      return TRUE;
    }
    else
    {
      //Drop the key event, SAVE state doesn't have its own key handler.
      MSG_HIGH("Drop key event at SAVE state", 0, 0, 0);
      return TRUE;
    }

  case EVT_COMMAND:
    MSG_HIGH("EVT_COMMAND, wParam = %d", wParam, 0, 0);
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
    if (IMENUCTL_IsActive(pMe->m_pIMenu))
    {
      IMENUCTL_Reset(pMe->m_pIMenu);
      //Figure out which Menu is active and handle it
      switch (pMe->m_eActiveMenu)
      {
      case RESIZE_MENU:
        Qalbum_DoResize(pMe);
        // go back to edit state
        pMe->eState = QCAM_STATE_EDIT;
        // Redraw the image
        Qalbum_UpdateEditedImage(pMe);
        break;
	  case JPEG_COLOR_MENU:
     	pMe->eState = QCAM_STATE_DIR;
	    Qalbum_DisplayPhotoDirs(pMe, TRUE);
		break;
      case PRINT_MENU:
#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
        MSG_HIGH("We are trying to Print", 0, 0, 0);
        // go back to the view state
        pMe->eState = QCAM_STATE_VIEW;

        if (!Qalbum_DoPrint(pMe))
        {
          // Report error and redraw the image
          Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_PRINT_FAIL);
        }
#endif //#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
        break;
      }
      return TRUE;
    }
    if (wParam == IDS_YES || wParam == IDS_NO)
    {
      Qalbum_DeleteFileResponse(pMe, wParam);
      return TRUE;
    }
    break;
  case EVT_USER_CLOSEAPP:
    MSG_HIGH("EVT_USER_CLOSEAPP", 0, 0, 0);
    (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
    break;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  default:
    break;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION Qalbum_FreeAppData

  DESCRIPTION
  This function frees any data initialized in the Init function.

  PROTOTYPE:
    void Qalbum_FreeAppData(Qalbum* pi);

  PARAMETERS:
    QCam * pMe - Pointer to the applet instance

  RETURN VALUE:
   None
===========================================================================*/
static void Qalbum_FreeAppData (Qalbum * pMe)
{
  if (!pMe)
    return;

  // Cancel any pending timers
  MSG_HIGH("Qalbum_FreeAppData", 0, 0, 0);
  (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe );
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
  if (pMe->m_pLaunchDS->bMultiDisplayMode)
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
  CAM_RELEASEIF(pMe->m_pAlbum);
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef FEATURE_GEOTAGGING
  if (pMe->m_pIPosDet)
  {
   // pMe->m_eGPSState = QCAM_GPS_DISABLED;
    CALLBACK_Cancel(&pMe->GPSInfoCB);
    (void)IPOSDET_Release(pMe->m_pIPosDet);
    pMe->m_pIPosDet = NULL;
  }
#endif /*FEATURE_GEOTAGGING*/  
  //release interface pointers
  CAM_RELEASEIF(pMe->m_pISoftMenu);
  CAM_RELEASEIF(pMe->m_pIMenu);
  CAM_RELEASEIF(pMe->m_pIFileMgr);
  CAM_RELEASEIF(pMe->m_pDisplayImage);
  CAM_RELEASEIF(pMe->m_pFrameImage);
  CAM_RELEASEIF(pMe->m_pFrameDIB);
  CAM_RELEASEIF(pMe->m_pIStatic);
  CAM_RELEASEIF(pMe->m_pIYCbCrFactory);
  CAM_RELEASEIF(pMe->m_pIIpl);
  CAM_RELEASEIF(pMe->m_pEditThumbnail);
  CAM_RELEASEIF(pMe->m_pMediaUtil);

  // Release image data buffer if not NULL
  CAM_RELEASEIF(pMe->m_pEditIn);
  CAM_RELEASEIF(pMe->m_pEditIn2);
  CAM_RELEASEIF(pMe->m_pEditOut);
  CAM_RELEASEIF(pMe->m_pZoomImg);
  CAM_FREEIF(pMe->m_pThumbnail);
  CAM_FREEIF(pMe->m_pLaunchDS);
  // free directory, photo and frame list
  QCam_DeleteFileList(&pMe->m_pDirList);
  QCam_DeleteFileList(&pMe->m_pPhotoList);
  QCam_DeleteFileList(&pMe->m_pFrameList);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#ifdef HEAP_CHECK
  CAM_RELEASEIF (pMe->m_pIHeap);
  mm_reset_all_max_heap();
#endif
}

/*===========================================================================
FUNCTION Qalbum_CommonKeyHandler

  DESCRIPTION
    Key handler for Qalbum in NULL state. This state is for add frame mode.
    On Sel key - update the frame file name in the launch data struct then close the app.
    On Clr key - close the application.

  PARAMETERS:
    pUser - Pointer to the applet instance, map to QCam * pMe
    eCode - AEEEvent id
    key - key id

  RETURN VALUE:
   None
===========================================================================*/
static boolean Qalbum_CommonKeyHandler(Qalbum *pMe, AEEEvent eCode, uint16 key)
{
  if (!pMe)
    return FALSE;

  if (eCode != EVT_KEY)
    return FALSE;

  //Message for error or no photo. exit the application
  if (pMe->m_bMsgTimerOn)
  {
    Qalbum_Exit(pMe);
    return TRUE;
  }

  switch (key)
  {
  case AVK_UP:
  case AVK_LEFT:
  case AVK_DOWN:
  case AVK_RIGHT:
    if (pMe->m_bDisplayFrames)
    {
      //Navigate through frame thumbnails
      Qalbum_ThumbnailSelectNext(pMe, key);
    }
    break;
  case AVK_SELECT:
    {
      IFile* pIFile = NULL;
      boolean bFrame = FALSE;

      //clear the old file name, either replace with a new selected name or no frame.
      pMe->m_pLaunchDS->sFrameFileName[0] = '\0';
      if ((pMe->m_nCurrentFrame > 1) &&
          (QCam_GetCurrentItem(pMe->m_pFrameList, &pMe->m_pCurrentFrame,
                                (int16)(pMe->m_nCurrentFrame - 1))))
      {
        bFrame = TRUE;
      }

      pIFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, QCAM_CONFIG_FILE, _OFM_READWRITE);
      if (!pIFile)
      {
        MSG_HIGH("Failed to open the qcam.cfg file.", 0, 0, 0);
        Qalbum_DisplayFrameErr(pMe, IDS_OVERLAY_FAIL);
      }
      else
      {
        //No need to modify the whole file, seek to frame file name location.
        if (IFILE_Seek(pIFile, _SEEK_START, (int32)(sizeof(uint16) * 9)) == SUCCESS)
        {
          if (bFrame)
          {
            MSG_MED("Selected frame %s", pMe->m_pCurrentFrame->fileName, 0, 0);
            (void)IFILE_Write(pIFile, pMe->m_pCurrentFrame->fileName, FILE_NAME_SIZE);
          }
          else
          {
            MSG_MED("Selected No frame.", 0, 0, 0);
            (void)IFILE_Write(pIFile, pMe->m_pLaunchDS->sFrameFileName, FILE_NAME_SIZE);
          }
        }
        else
        {
          MSG_HIGH("Failed to seek.", 0, 0, 0);
          Qalbum_DisplayFrameErr(pMe, IDS_OVERLAY_FAIL);
        }
        (void)IFILE_Release(pIFile);
      }
    }
    //lint -fallthrough AVK_CLR to close the app
  case AVK_CLR:
    //Close viewer application and return to Qcamera
    MSG_HIGH("ISHELL_CloseApplet - Cancel get frame.", 0, 0, 0);
    (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
    break;
  default:
    break;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION Qalbum_AlbumKeyHandler

  DESCRIPTION
    Key handler for Qalbum in dir state
    On Sel key - enter thumbnail state to display images in selected dir.
    On Clr key - close the application

  PARAMETERS:
    pUser - Pointer to the applet instance, map to QCam * pMe
    eCode - AEEEvent id
    key - key id

  RETURN VALUE:
   None
===========================================================================*/
static boolean Qalbum_AlbumKeyHandler(Qalbum * pMe, AEEEvent eCode, uint16 key)
{
  if (!pMe)
    return FALSE;

  if (eCode != EVT_KEY)
    return FALSE;

  switch (key)
  {
  case AVK_UP:
  case AVK_LEFT:
  case AVK_DOWN:
  case AVK_RIGHT:
    //Navigate through dir folders
    Qalbum_ThumbnailSelectNext(pMe, key);
    break;
  case AVK_SELECT:
    //Display thumbnail images and enter into Thumbnail state
    pMe->eState = QCAM_STATE_THUMBNAIL;
    pMe->m_nStartPhoto = 1;
    pMe->m_nCurrentPhoto = 1;
    pMe->m_n2ndPhoto     = 1;
    ZEROAT(&pMe->m_SelectRC);
    MSG_HIGH("Enter thumbnail mode - Qalbum_DisplayPhoto.", 0, 0, 0);
    Qalbum_DisplayPhoto(pMe, TRUE);
    break;
  case AVK_CLR:
    //Close viewer application and return to Qcamera
    MSG_HIGH("ISHELL_PostEvent - EVT_USER_CLOSEAPP.", 0, 0, 0);
    (void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_QALBUM, EVT_USER_CLOSEAPP, 0, 0 );
    break;
  case AVK_STAR:
  	pMe->eState = QCAM_STATE_MENU;
    (void)Qalbum_BuildColorFormatMenu(pMe);
  default:
    break;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION Qalbum_SlideShowKeyHandler

  DESCRIPTION
    Key handler for Qalbum in Slideshow state, This state is for the slideshow mode
    On any key - return the thumbnail state.

  PARAMETERS:
    pUser - Pointer to the applet instance, map to QCam * pMe
    eCode - AEEEvent id
    key - key id

  RETURN VALUE:
   None
===========================================================================*/
static boolean Qalbum_SlideShowKeyHandler(Qalbum * pMe, AEEEvent eCode, uint16 key)
{
  if (!pMe)
    return FALSE;

  //No matter what key, always stop slide show and return to thumbnail state
  if ((eCode == EVT_KEY) && pMe->m_bIsSlideShowStarted)
  {
    MSG_HIGH("User stops Slideshow", 0, 0, 0);
    pMe->m_bIsSlideShowStarted = FALSE;
    pMe->eState = QCAM_STATE_THUMBNAIL;
    (void)ISHELL_CancelTimer( pMe->a.m_pIShell, NULL, pMe );
    //Find the screen with current image and display thumbnails of this screen
    pMe->m_nStartPhoto = (int16)((((pMe->m_nCurrentPhoto - 1) / QCAM_THUMBNAILS) * QCAM_THUMBNAILS) + 1);
    IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
    if (pMe->m_pLaunchDS->bMultiDisplayMode)
    {
      //Disable display link to AEECLSID_DISPLAY3 when exit slide show
      (void)QCamDisp_ChangeDisplay(pMe->a.m_pIShell, AEECLSID_DISPLAY3, 0);
    }
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef FEATURE_JPEGD_RESIZE
    if (pMe->m_pLaunchDS->bSlideshowResize)
    {
      pMe->m_dispSizes.wPhotoWidth = pMe->di.cxScreen;
      pMe->m_dispSizes.wPhotoHeight = (pMe->di.cyScreen - pMe->m_nTitleHeight) - pMe->m_softMenuHeight;
    }
#endif //FEATURE_JPEGD_RESIZE
    if(!pMe->m_bDecodeInProgress)
      Qalbum_DisplayPhoto(pMe, TRUE);
    MSG_HIGH("Slideshow stopped", 0, 0, 0);
  }
  //lint -save -e715 symbol key not referenced here
  return TRUE;
  //lint -restore
}

/*===========================================================================
FUNCTION Qalbum_ThumbnailKeyHandler

  DESCRIPTION
    Key handler for Qalbum in thumbnail state
    On select key - display full image and enter view state
    On Clear key - enter edit state if it is frame image
                   enter dir state if it is photo image

  PARAMETERS:
    pMe   - pointer to Qalbum struct
    eCode - AEEEvent id
    key   - key id

  RETURN VALUE:
   None
===========================================================================*/
static boolean Qalbum_ThumbnailKeyHandler(Qalbum *pMe, AEEEvent eCode, uint16 key)
{
  if (!pMe)
    return FALSE;

  if (eCode != EVT_KEY)
    return FALSE;

  //Message for no frame. call Qalbum_Exit to switch back previous state
  if (pMe->m_bMsgTimerOn)
  {
    Qalbum_Exit(pMe);
    return TRUE;
  }

  switch (key)
  {
  case AVK_UP:
  case AVK_LEFT:
  case AVK_DOWN:
  case AVK_RIGHT:
    // drop key events if thumbnail is being loaded
    if (!pMe->m_bImagesLoading)
    {
      Qalbum_ThumbnailSelectNext(pMe, key);
    }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
    break;
  case AVK_SELECT:
    if (pMe->m_bImagesLoading)
    {
      // Do not enter view state yet
      MSG_HIGH("Image loading...", 0, 0, 0);
      //put the key in queue so it will be executed after loading done
      pMe->m_bStopLoadingThumbnails = TRUE;
      pMe->m_wKeyPressed = (uint16)AVK_SELECT;
    }
    else if (pMe->m_bDisplayFrames)
    {
      //Enter to edit state
      pMe->eState = QCAM_STATE_EDIT;
      //reset variables first
      pMe->m_bDisplayFrames = FALSE;

      //if a frame is selected, Add the frame to the current image.
      if (pMe->m_nCurrentFrame > 1)
      {
        if (QCam_GetCurrentItem(pMe->m_pFrameList, &pMe->m_pCurrentFrame,
                                (int16)(pMe->m_nCurrentFrame - 1)))
        {
          (void)STRLCPY(pMe->m_sFrameFileName, pMe->m_pCurrentFrame->fileName, FILE_NAME_SIZE);
          MSG_HIGH("Add frame %s to image", pMe->m_sFrameFileName, 0, 0);
          Qalbum_AddImageFrame(pMe);
        }
      }
      else
      {
        MSG_HIGH("Update image without frame", 0, 0, 0);
        Qalbum_DisplayListNavArrows(pMe);
        Qalbum_DisplayEditMenuName(pMe, 0);
        Qalbum_UpdateEditedImage(pMe);
      }
    }
#ifndef FEATURE_IPL_LITE
    else if (pMe->m_bIs2ndPhoto)
    {
      pMe->eState = QCAM_STATE_ADV_EDIT;
      MSG_HIGH("Load 2nd image for edit", 0, 0, 0);
      Qalbum_LoadImage(pMe);
    }
#endif //FEATURE_IPL_LITE
    else
    {
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
      if (pMe->m_pLaunchDS->bMultiDisplayMode)
      {
        //enable display link to AEECLSID_DISPLAY3
        if (QCamDisp_ChangeDisplay(pMe->a.m_pIShell, AEECLSID_DISPLAY3, 1) != SUCCESS)
        {
          MSG_HIGH("Failed QCamDisp_ChangeDisplay.", 0, 0, 0);
        }
      }
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
      //Enter to view state
      pMe->eState = QCAM_STATE_VIEW;
      MSG_HIGH("Enter View mode", 0, 0, 0);
      Qalbum_ViewPhoto(pMe, 0, TRUE);
    }
    break;
	
  case AVK_END:
    if (pMe->m_bImagesLoading)
    {
      MSG_HIGH("Image loading...", 0, 0, 0);
      //put the key in queue so it will be executed after loading done
      pMe->m_bStopLoadingThumbnails = TRUE;
      pMe->m_wKeyPressed = (uint16)AVK_END;
    }
    else
    {
      (void)ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
    }
    break;
	
  case AVK_CLR:
    //came in thumbnail from edit mode, just return
    if (pMe->m_bDisplayFrames)
    {
      MSG_HIGH("Cancel add frame.", 0, 0, 0);
      //reset variables first
      pMe->m_bDisplayFrames = FALSE;
      pMe->m_bEditProcessed = FALSE;
      // Change state back to Edit
      pMe->eState = QCAM_STATE_EDIT;
      // Redraw the edit image
      Qalbum_DisplayListNavArrows(pMe);
      Qalbum_DisplayEditMenuName(pMe, 0);
      Qalbum_UpdateEditedImage(pMe);
    }
    else if (pMe->m_bImagesLoading)
    {
      MSG_HIGH("Image loading...", 0, 0, 0);
      //put the key in queue so it will be executed after loading done
      pMe->m_bStopLoadingThumbnails = TRUE;
      pMe->m_wKeyPressed = (uint16)AVK_CLR;
    }
#ifndef FEATURE_IPL_LITE
    else if (pMe->m_bIs2ndPhoto)
    {
      pMe->eState = QCAM_STATE_ADV_EDIT;
      pMe->m_bIs2ndPhoto = FALSE;
      // Redraw the edit image
      Qalbum_DisplayListNavArrows(pMe);
      Qalbum_DisplayAdvEditMenuName(pMe, 0);
      Qalbum_UpdateEditedImage(pMe);
    }
#endif //FEATURE_IPL_LITE
    else
    {
      //return to dir state
      MSG_HIGH("Return to Directory mode", 0, 0, 0);
      pMe->eState = QCAM_STATE_DIR;
      pMe->m_bIs2ndPhoto = FALSE;
      Qalbum_DisplayPhotoDirs(pMe, TRUE);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
    }
    break;
 case AVK_STAR:
#ifdef FEATURE_GEOTAGGING
      if(!pMe->m_bIs2ndPhoto && !pMe->m_bDisplayFrames)
      {
        Qalbum_AddGeoTags(pMe);
      }
#endif /* FEATURE_GEOTAGGING*/
      break;

  default:
    break;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION Qalbum_ViewKeyHandler

  DESCRIPTION
    Key handler for Qalbum in view state

  PARAMETERS:
    pMe   - pointer to Qalbum data struct
    eCode - AEEEvent id
    key   - key id

  RETURN VALUE:
    TRUE if event process, FALSE otherwise
===========================================================================*/
static boolean Qalbum_ViewKeyHandler(Qalbum *pMe, AEEEvent eCode, uint16 key)
{
  uint16  sel;
  uint32  index = 0;
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
    if ((sel == IDS_ZOOM ) && (pMe->m_bZoom == TRUE))
    {
      update_display = Qalbum_OnZoom(pMe, key);
    }
    else
    {
      //Navigate through menu icons
      Qalbum_DisplayViewMenuName(pMe, (key == (uint16)AVK_LEFT ? -1 : 1) );
      bHandled = IMENUCTL_HandleEvent(pMe->m_pISoftMenu, eCode, key, 0);
    }
    break;

  case AVK_SELECT:
    if (pMe->m_bImagesLoading)
    {
      //Drop SELECT key events when loading images
      MSG_HIGH("VIEW state: image loading...", 0, 0, 0);
      return TRUE;
    }
    //lint -fallthrough AVK_SELECT to call QalbumViewHandlers[].
  case AVK_DOWN:
  case AVK_UP:
  case AVK_STAR:
    if (pMe->m_pEditIn) // Edit only if image is successfully loaded
    {
      //Execute menu on focus
      MSG_MED("Select View feature with ID %d", sel, 0, 0);
      if (QalbumViewHandlers[index].wMenuID == sel)
      {
        update_display = QalbumViewHandlers[index].MenuOnKeyFunc(pMe, key);
      }
      else
      {
        //incase the item is filtered out dyna
        for (index = 0; index < ARR_SIZE(QalbumViewHandlers); index++)
        {
          if (QalbumViewHandlers[index].wMenuID == sel)
          {
            update_display = QalbumViewHandlers[index].MenuOnKeyFunc(pMe, key);
            break;
          }
        }
      }
    }
    break;

  case AVK_CLR:
    if (pMe->m_bPhotoErased)
    {
      pMe->m_bPhotoErased = FALSE;
      CAM_RELEASEIF(pMe->m_pIStatic);
      Qalbum_UpdateEditedImage(pMe);
      break;
    }
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
    if (pMe->m_pLaunchDS->bMultiDisplayMode)
    {
      //Disable display link to AEECLSID_DISPLAY3
      (void)QCamDisp_ChangeDisplay(pMe->a.m_pIShell, AEECLSID_DISPLAY3, 0);
    }
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
    // Release image data buffer
    CAM_RELEASEIF(pMe->m_pEditIn);
    CAM_RELEASEIF(pMe->m_pZoomImg);
    CAM_FREEIF(pMe->m_pThumbnail);
    pMe->m_bZoom    = FALSE;
    pMe->m_bMaxZoom = FALSE;
#ifndef FEATURE_IPL_LITE
    pMe->m_bIs2ndPhoto = FALSE;
#endif //FEATURE_IPL_LITE

    //Update before return to thumbnail state
    if (pMe->m_nTotalDirs)
    {
      MSG_HIGH("Return to Thumbnail mode", 0, 0, 0);
      pMe->eState = QCAM_STATE_THUMBNAIL;
      //Reset the Current selected photo and start count to 1.
      pMe->m_nStartPhoto = 1;
      pMe->m_nCurrentPhoto = 1;
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
      if(pMe->m_bDecodeInProgress)
        CAM_RELEASEIF(pMe->m_pDisplayImage);
      else
	  Qalbum_DisplayPhoto(pMe, TRUE);
    }
    else
    {
      //last file is deleted. no more file to view.
      //Display no photo message and exit the application.
      MSG_HIGH("Delete file completed - No photo.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      pMe->eState = QCAM_STATE_NULL;
      Qalbum_DisplayTimedImg(pMe, (PFNNOTIFY)Qalbum_Exit, IDB_NO_PHOTO);
    }
    break;

  default:
    break;
  }

  if (update_display)
  {
    MSG_HIGH("Qalbum_UpdateEditedImage after edit", 0, 0, 0);
    Qalbum_UpdateEditedImage(pMe);
  }

#ifdef HEAP_CHECK
  MSG_HEAP("ViewHandler:", 0, 0, 0);
  Qalbum_ShowHeapUsage(pMe);
#endif
  return bHandled;
}

/* ==========================================================================
FUNCTION Qalbum_OnSave
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnSave(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return FALSE;

  //Only responds to select key
  if (key == (uint16)AVK_SELECT)
  {
    MSG_MED("Start Qalbum_OnSave", 0, 0, 0);
    //Update current file name
    if (QCam_GetCurrentItem(pMe->m_pPhotoList, &pMe->m_pCurrentPhoto, pMe->m_nCurrentPhoto))
    {
      (void)STRLCPY(pMe->m_sCurrentFileName, pMe->m_pCurrentPhoto->fileName, FILE_NAME_SIZE);
    }
    else
    {
      MSG_HIGH("Failed to get current item.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      return FALSE;
    }
    //Check if space available
    if (!Qalbum_IsSpaceAvailable(pMe))
      return FALSE;

    //Init data buffers
    Qalbum_UpdateInIPLImage(pMe, IDS_SAVE);

    //Create save dialog for changing file name and selecting Save or Cancel
    pMe->eState = QCAM_STATE_SAVE;
    if (pMe->m_pDisplayImage || pMe->m_bPNGFile || !Qalbum_LoadJPEG(pMe, TRUE))
      Qalbum_ImageNotify(pMe, pMe->m_pDisplayImage, NULL, SUCCESS);
    else
      IIMAGE_Notify(pMe->m_pDisplayImage, Qalbum_ImageNotify, pMe);
  }
  //no need to update image
  return FALSE;
}

/* ==========================================================================
FUNCTION Qalbum_OnThumbnail
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnThumbnail(Qalbum *pMe, uint16 key)
{
  //Only responds to select key
  if (key == (uint16)AVK_SELECT)
  {
    //Check if space available
    if (!Qalbum_IsSpaceAvailable(pMe))
      return FALSE;

    Qalbum_UpdateInIPLImage(pMe, IDS_EXTRACT_THUMBNAIL);

    if (pMe->m_pDisplayImage || pMe->m_bPNGFile || !Qalbum_LoadJPEG(pMe, TRUE))
      Qalbum_ExtractThumbnail(pMe);
    else
      IIMAGE_Notify(pMe->m_pDisplayImage, Qalbum_ImageNotify, pMe);
  }
  //no need to update image
  return FALSE;
}

/* ==========================================================================
FUNCTION Qalbum_OnRevert
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnRevert(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return FALSE;

  //Only responds to select key
  if (key == (uint16)AVK_SELECT)
  {
    //reset the edit buffer
    Qalbum_UpdateInIPLImage(pMe, IDS_REVERT);
    CAM_RELEASEIF(pMe->m_pEditIn);
    //load the image again
    Qalbum_LoadImage(pMe);
  }
  return FALSE;
}

/* ==========================================================================
FUNCTION Qalbum_OnDel
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnDel(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return FALSE;

  //Only responds to select key
  if (key == (uint16)AVK_SELECT)
  {
    char cBuf[FILE_NAME_SIZE];
    AECHAR acBuf[FILE_NAME_SIZE];
    AECHAR szBuf[PROMPT_SIZE] = {0};
    AEEPromptInfo pi = {0};
    uint16 buttonIDs[3] = {IDS_YES, IDS_NO, 0};
    pi.pszRes   = pMe->m_pResFile;
    pi.wTitleID = IDS_DEL;
    pi.fntTitle = QCAM_FONT_TITLE;
    pi.fntText  = QCAM_FONT_TEXT;
    pi.wDefBtn  = IDS_NO;
    pi.pBtnIDs  = buttonIDs;

    (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile,
                               IDS_DEL, szBuf, sizeof(szBuf));

    if(STRNCMP(pMe->m_pCurrentDir->fileName, QCAM_MMC_CONSTANT, 5) == 0)
    {
      if (pMe->m_bSmallScreen)
      {
        /* On a small screen just display the file name */
        (void)SNPRINTF(cBuf, sizeof(cBuf), "\n%s?", &pMe->m_sCurrentFileName[QCAM_MMC_DATE_DIR_LEN]);
      }
      else
        (void)SNPRINTF(cBuf, sizeof(cBuf), "\n%s?", &pMe->m_sCurrentFileName[QCAM_MMC_DIR_LEN]);
    }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
    else if(STRNCMP(pMe->m_pCurrentDir->fileName, QCAM_USB_CONSTANT, QCAM_USB_CONST_LEN) == 0)
    {
      if (pMe->m_bSmallScreen)
      {
        /* On a small screen just display the file name */
        (void)SNPRINTF(cBuf, sizeof(cBuf), "\n%s?", &pMe->m_sCurrentFileName[QCAM_USB_DATE_DIR_LEN]);
      }
      else
        (void)SNPRINTF(cBuf, sizeof(cBuf), "\n%s?", &pMe->m_sCurrentFileName[QCAM_USB_DIR_LEN]);
    }
#endif
    else if(STRNCMP(pMe->m_pCurrentDir->fileName, QCAM_FTL_CONSTANT, QCAM_FTL_CONST_LEN) == 0)
    {
      if (pMe->m_bSmallScreen)
      {
        /* On a small screen just display the file name */
        (void)SNPRINTF(cBuf, sizeof(cBuf), "\n%s?", &pMe->m_sCurrentFileName[QCAM_FTL_DATE_DIR_LEN]);
      }
      else
        (void)SNPRINTF(cBuf, sizeof(cBuf), "\n%s?", &pMe->m_sCurrentFileName[QCAM_FTL_DIR_LEN]);
    }
    else
    {
      if (pMe->m_bSmallScreen)
      {
        /* On a small screen just display the file name */
        (void)SNPRINTF(cBuf, sizeof(cBuf), "\n%s?", &pMe->m_sCurrentFileName[QCAM_PHOTOS_DATE_DIR_LEN]);
      }
      else
        (void)SNPRINTF(cBuf, sizeof(cBuf), "\n%s?", &pMe->m_sCurrentFileName[QCAM_PHOTOS_DIR_LEN]);
    }

    (void)STRTOWSTR(cBuf, acBuf, sizeof(acBuf));
    (void)WSTRLCAT(szBuf, acBuf, sizeof(szBuf));

    pi.pText = szBuf;

    if (!ISHELL_Prompt(pMe->a.m_pIShell, &pi))
    {
      Qalbum_DeleteFileResponse(pMe, IDS_NO);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
  }
  //no need to update image
  return FALSE;
}

/* ==========================================================================
FUNCTION Qalbum_OnCopy
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnCopy(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return FALSE;

  //Only responds to select key
  if (key == (uint16)AVK_SELECT)
  {
    // Copy the image to the shared directory
    if (!QCam_CopyToDest(pMe->m_pIFileMgr, pMe->m_sCurrentFileName, AEE_SHARED_DIR, NULL))
    {
      MSG_HIGH("Failed to copy a file to the shared directory.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
    }
    // Go back to album mode
    (void)Qalbum_ViewKeyHandler(pMe, EVT_KEY, (uint16)AVK_CLR);
  }
  //no need to update image
  return FALSE;
}

#ifdef FEATURE_APP_MMS
#error code not present
#endif //FEATURE_APP_MMS

/*===========================================================================
FUNCTION Qalbum_SetProperties

  DESCRIPTION
    This function compiles a string that contains the properties of a photo.

  PARAMETERS:
    pMe    [in]  - point to Qalbum struct
    szText [out] - String contains properties

  RETURN VALUE:
    None
===========================================================================*/
static void Qalbum_SetProperties(const Qalbum * pMe, char * szText, int size)
{
  char            szBuf[QALBUM_EXIF_MAX_LEN + 1] = {0};
  FileInfo        fi = {0};
  OEMImageParmExt parm;
  int             retVal;

  if (!pMe)
    return;

  // File name
  if (pMe->m_bSmallScreen)
    (void)SNPRINTF(szText, size, "Name: %s", STRRCHR(pMe->m_sCurrentFileName, '/') + 1);
  else
    (void)SNPRINTF(szText, size, "File name: %s",
                   STRRCHR(pMe->m_sCurrentFileName, '/') + 1);

  // File size
  if (IFILEMGR_GetInfo(pMe->m_pIFileMgr, pMe->m_sCurrentFileName, &fi) != SUCCESS)
  {
    MSG_HIGH("Couldn't get File Info for Display",0,0,0);
  }
  else
  {
    if (pMe->m_bSmallScreen)
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nSize: %u bytes", fi.dwSize);
    else
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nFile size: %u bytes", fi.dwSize);
    (void)STRLCAT(szText, szBuf, size);
  }

  // Show exif info for Jpeg photos
  if (!pMe->m_bPNGFile && pMe->m_pDisplayImage)
  {
    // Make
    parm.uID = (uint32)EXIF_MAKE;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nMake: %s", parm.pData);
      (void)STRLCAT(szText, szBuf, size);
    }

    // Model
    parm.uID = (uint32)EXIF_MODEL;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nModel: %s", parm.pData);
      (void)STRLCAT(szText, szBuf, size);
    }

    // Software
    parm.uID = (uint32)SOFTWARE;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nSoftware: %s", parm.pData);
      (void)STRLCAT(szText, szBuf, size);
    }

    // Exif version
    parm.uID = (uint32)EXIF_VERSION;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nEXIF version: %s", parm.pData);
      (void)STRLCAT(szText, szBuf, size);
    }

    // Date info
    parm.uID = (uint32)EXIF_DATE_TIME_ORIGINAL;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nPhoto taken on: %s", parm.pData);
      (void)STRLCAT(szText, szBuf, size);
    }

    // Latitude reference
    parm.uID = (uint32)GPS_LATITUDE_REF;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nLatitude reference: %s", parm.pData);
      (void)STRLCAT(szText, szBuf, size);
    }
    // Latitude
    parm.uID = (uint32)GPS_LATITUDE;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nLatitude: %d/%d, %d/%d, %d/%d", *(int*)parm.pData,
                    *((int*)parm.pData+1), *((int*)parm.pData+2), *((int*)parm.pData+3),
                    *((int*)parm.pData+4), *((int*)parm.pData+5));
      (void)STRLCAT(szText, szBuf, size);
    }

    // Longitude reference
    parm.uID = (uint32)GPS_LONGITUDE_REF;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nLongitude reference: %s", parm.pData);
      (void)STRLCAT(szText, szBuf, size);
    }
    // Longitude
    parm.uID = (uint32)GPS_LONGITUDE;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nLatitude: %d/%d, %d/%d, %d/%d",
                    *(int*)parm.pData, *((int*)parm.pData+1), *((int*)parm.pData+2),
                    *((int*)parm.pData+3), *((int*)parm.pData+4), *((int*)parm.pData+5));
      (void)STRLCAT(szText, szBuf, size);
    }

    // Altitude reference
    parm.uID = (uint32)GPS_ALTITUDE_REF;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      if (*(uint8*)parm.pData == 0)
      {
        (void)SNPRINTF(szBuf, sizeof(szBuf), "\nAltitude reference: sea level");
      }
      else if (*(uint8*)parm.pData == 1)
      {
        (void)SNPRINTF(szBuf, sizeof(szBuf),
                       "\nAltitude reference: sea level reference (negative)");
      }
      else
      {
        (void)SNPRINTF(szBuf, sizeof(szBuf), "\nAltitude reference: reserved");
      }
      (void)STRLCAT(szText, szBuf, size);
    }
    // Altitude
    parm.uID = (uint32)GPS_ALTITUDE;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      (void)SNPRINTF(szBuf, sizeof(szBuf), "\nAltitude: %d/%d", *(int*)parm.pData, *((int*)parm.pData+1));
      (void)STRLCAT(szText, szBuf, size);
    }

    // White balance
    parm.uID = (uint32)WHITE_BALANCE;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      if (*(uint16*)parm.pData == 0)
      {
        (void)SNPRINTF(szBuf, sizeof(szBuf), "\nWhite balance: auto");
      }
      else if (*(uint16*)parm.pData == 1)
      {
        (void)SNPRINTF(szBuf, sizeof(szBuf), "\nWhite balance: manual");
      }
      else
      {
        (void)SNPRINTF(szBuf, sizeof(szBuf),"\nWhite balance: reserved");
      }
      (void)STRLCAT(szText, szBuf, size);
    }

    // Image description
    parm.uID = (uint32)IMAGE_DESCRIPTION;
    retVal = IIMAGE_ExtFunc(pMe->m_pDisplayImage, AEECLSID_EXIF_GET_VALUE,
                            &parm, sizeof(OEMImageParmExt));
    if ((retVal == SUCCESS) && (parm.uLen <= QALBUM_EXIF_MAX_LEN) && parm.pData)
    {
      if (parm.uLen > 0)
      {
        (void)SNPRINTF(szBuf, sizeof(szBuf), "\nDescription: %s", parm.pData);
        (void)STRLCAT(szText, szBuf, size);
      }
    }
  }
}


/*=====================================================================
FUNCTION Qalbum_DisplayProperties

  DESCRIPTION: Helper function to create property page and display it

  PARAMETERS:
    pMe:  point to Qalbum struct

  RETURN VALUE:
    void
======================================================================*/
static void Qalbum_DisplayProperties(Qalbum *pMe)
{
  AECHAR  wsTitle[20];
  AECHAR  wsText[QALBUM_PROPERTY_MAX_LEN] = {0};
  char    szText[QALBUM_PROPERTY_MAX_LEN] = {0};

  if (!pMe || !pMe->m_pIStatic)
    return;

  Qalbum_SetProperties(pMe, szText, QALBUM_PROPERTY_MAX_LEN);
  (void)STRTOWSTR("Properties", wsTitle, sizeof(wsTitle));
  (void)STRTOWSTR(szText, wsText, ((int)STRLEN(szText)+1)*2);
  (void)ISTATIC_SetText(pMe->m_pIStatic,
                        wsTitle,
                        wsText,
                        QCAM_FONT_TITLE,
                        QCAM_FONT_TEXT);

  (void)ISTATIC_Redraw(pMe->m_pIStatic);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
}

/* ==========================================================================
FUNCTION Qalbum_OnProperty
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnProperty(Qalbum *pMe, uint16 key)
{
  AEERect rc;

  if (!pMe)
    return FALSE;

  if (key == (uint16)AVK_SELECT)
  {
    // Clear the image display area
    SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen - pMe->m_softMenuHeight);

    IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
    pMe->m_bPhotoErased = TRUE;

    // Create a IStatic interface
    if (!pMe->m_pIStatic &&
        ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC,
                              (void**)&pMe->m_pIStatic) != SUCCESS)
    {
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      return FALSE;
    }
    // Set control rect
    SETAEERECT(&rc, QCAM_TXT_X_START, 0, pMe->di.cxScreen - QCAM_TXT_X_START,
               pMe->di.cyScreen - pMe->m_softMenuHeight);

    ISTATIC_SetRect(pMe->m_pIStatic, &rc);

    // Specify control's style
    ISTATIC_SetProperties(pMe->m_pIStatic, (ST_CENTERTITLE | ST_NOSCROLL));

    // Set title and text string
    if (pMe->m_pDisplayImage || !Qalbum_LoadJPEG(pMe, TRUE))
    {
      Qalbum_DisplayProperties(pMe);
    }
    else
    {
      IIMAGE_Notify(pMe->m_pDisplayImage, Qalbum_ImageNotify, pMe);
    }
  }
  else if ((key == (uint16)AVK_UP) || (key == (uint16)AVK_DOWN))
  {
    if (pMe->m_pIStatic)
    {
      (void)ISTATIC_HandleEvent(pMe->m_pIStatic, EVT_KEY, key, 0);
    }
  }

  return FALSE;
}

/* ==========================================================================
FUNCTION Qalbum_OnDescribe
DESCRIPTION
============================================================================= */
static boolean Qalbum_OnDescribe(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return FALSE;

  //Only responds to select key
  if (key != (uint16)AVK_SELECT)
    return FALSE;

  Qalbum_UpdateInIPLImage(pMe, IDS_DESCRIBE);

  if( pMe->m_bPNGFile || (!pMe->m_pDisplayImage && !Qalbum_LoadJPEG(pMe, TRUE)))
  {
    Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_DESCRIBE_FAIL);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  }
  else if (pMe->m_pDisplayImage || !Qalbum_LoadJPEG(pMe, TRUE))
  {
    Qalbum_ImageNotify(pMe, pMe->m_pDisplayImage, NULL, SUCCESS);
  }
  else
  {
    IIMAGE_Notify(pMe->m_pDisplayImage, Qalbum_ImageNotify, pMe);
  }

  return FALSE;
}

/* ==========================================================================
FUNCTION Qalbum_ValidateCoordinates
  DESCRIPTION
    This function update and checks if xSrc, ySrc stay within the dispRC,
    if not adjust them accordingly.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    cx  - image width
    cy  - image height

  RETURN VALUE:
    none.

============================================================================= */
static void Qalbum_ValidateCoordinates(Qalbum *pMe, int cx, int cy)
{
  int nXY = 0;

  if (!pMe)
    return;

  ////Update pMe->m_xSrc and pMe->m_ySrc
  pMe->m_xSrc = pMe->m_xOffset + ((cx - (int)pMe->m_dispRC.dx) / 2);
  pMe->m_ySrc = pMe->m_yOffset + ((cy - (int)pMe->m_dispRC.dy) / 2);

  // Check and update m_xSrc to make sure it not less than 0.
  if( pMe->m_xSrc < 0)
  {
    pMe->m_xSrc = 0;
  }
  nXY = cx - (int)pMe->m_dispRC.dx;
  if ((nXY > 0) && (pMe->m_xSrc > nXY))
  {
    pMe->m_xSrc = nXY;
  }
  // Check and update m_ySrc to make sure it not less than 0.
  if( pMe->m_ySrc < 0)
  {
    pMe->m_ySrc = 0;
  }
  nXY = cy - (int)pMe->m_dispRC.dy;
  if ((nXY > 0) && (pMe->m_ySrc > nXY))
  {
    pMe->m_ySrc = nXY;
  }
}

/*=====================================================================
FUNCTION Qalbum_DoZooming

  DESCRIPTION:
    This helper function calls IIPL API to does the zooming.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    cx  - image width
    cy  - image height
    bDIB - Flag to tell if the image in DIB format.

  RETURN VALUE:
    TRUE if success, Otherwise FALSE
======================================================================*/
static boolean Qalbum_DoZooming(Qalbum *pMe, uint16 cx, uint16 cy, boolean bDIB)
{
  int retVal = AEE_SUCCESS;
  if (!pMe)
    return FALSE;

  CAM_RELEASEIF(pMe->m_pZoomImg);
  // Need to create the original image in case reaching max zoom at 1st zoom
  if (IBITMAP_CreateCompatibleBitmap(pMe->m_pEditIn, &pMe->m_pZoomImg, cx, cy)
                                     != SUCCESS)
  {
    MSG_HIGH("Failed in IBITMAP_CreateCompatibleBitmap", 0, 0, 0);
    Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_MEM_FULL);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
    return FALSE;
  }

  if (bDIB)
  {
    //Correct the color scheme if it is IDIB type image
    ((IDIB *)pMe->m_pZoomImg)->nColorScheme = ((IDIB *)pMe->m_pEditIn)->nColorScheme;
  }
  //Do the resize
  retVal = IIPL_ChangeSize(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pZoomImg) ;
  // Checking for the IIPL Edit Error
  if (Qalbum_DisplayError(pMe, retVal) != SUCCESS)
  {
    MSG_HIGH("Failed in IIPL_ChangeSize", 0, 0, 0);
    return FALSE;
  }

  return TRUE;
}

/*=====================================================================
FUNCTION Qalbum_InitZoomIn

  DESCRIPTION:
    This function initializes the zooming in.

  PARAMETERS:
    pMe  - Pointer to Qalbum struct
    bDIB - Flag to tell if the image in DIB format.
    info - Image info

  RETURN VALUE:
    TRUE if success, Otherwise FALSE
======================================================================*/
static boolean Qalbum_InitZoomIn(Qalbum *pMe, AEEBitmapInfo info)
{
  if (!pMe)
    return FALSE;

  // Initialize m_dispRC, m_xSrc, m_ySrc
  SETAEERECT(&pMe->m_dispRC, QCAM_PICTURE_X_POS, pMe->m_nTitleHeight,
             pMe->m_dispSizes.wPhotoWidth, pMe->m_dispSizes.wPhotoHeight);
  pMe->m_xOffset = 0;
  pMe->m_yOffset = 0;
  pMe->m_xSrc = 0;
  pMe->m_ySrc = 0;

  // Can we zoom in?
  if (((int16)info.cx > pMe->m_dispRC.dx) || ((int16)info.cy > pMe->m_dispRC.dy))
  {
    pMe->m_ratio = QCamDisp_GetResizeRatio(pMe->m_dispRC,
                                           (int16)info.cx, (int16)info.cy);
    //reset flags
    pMe->m_bZoom    = TRUE;
    pMe->m_bMaxZoom = FALSE;

    return TRUE;
  }

  return FALSE;
}

/*=====================================================================
FUNCTION Qalbum_ZoomIn

  DESCRIPTION:
    This function does zooming in

  PARAMETERS:
    pMe  - Pointer to Qalbum struct
    bDIB - Flag to tell if the image in DIB format.
    info - Image info

  RETURN VALUE:
    TRUE if success, Otherwise FALSE
======================================================================*/
static boolean Qalbum_ZoomIn(Qalbum *pMe, boolean bDIB, AEEBitmapInfo info)
{
  int     cx = 0;
  int     cy = 0;
  AEERect rc;

  if (pMe->m_ratio * 2 < 100)  // zoom in 2x if possible
  {
    pMe->m_ratio       = pMe->m_ratio * 2;
    pMe->m_xOffset     = pMe->m_xOffset * 2;
    pMe->m_yOffset     = pMe->m_yOffset * 2;

    if (pMe->m_pZoomImg)
    {
      if (bDIB)
      {
        cx = (int)(((IDIB *)pMe->m_pZoomImg)->cx * 2);
        cy = (int)(((IDIB *)pMe->m_pZoomImg)->cy * 2);
      }
      else
      {
        cx = (int)(((IYCbCr *)pMe->m_pZoomImg)->cx * 2);
        cy = (int)(((IYCbCr *)pMe->m_pZoomImg)->cy * 2);
      }
    }
    else
    {
      //If it is first zoom in, pMe->m_pZoomImg is not created.
      rc = QCamDisp_GetResizedRect(pMe->m_dispRC, (int16)info.cx, (int16)info.cy);
      cx = (int)rc.dx * 2;
      cy = (int)rc.dy * 2;
    }
    //Update pMe->m_xSrc and pMe->m_ySrc
    Qalbum_ValidateCoordinates(pMe, cx, cy);

    if (!Qalbum_DoZooming(pMe, (uint16)cx, (uint16)cy, bDIB))
    {
      MSG_HIGH("Qalbum_ZoomIn failed.", 0, 0, 0);
      return FALSE;
    }
  }
  else  // cannot zoom in 2x, then zoom in to max size
  {
    pMe->m_bMaxZoom = TRUE;  // at the max size
    pMe->m_xOffset  = (int)((pMe->m_xOffset * 100) / pMe->m_ratio);
    pMe->m_yOffset  = (int)((pMe->m_yOffset * 100) / pMe->m_ratio);
    //Update pMe->m_xSrc and pMe->m_ySrc
    Qalbum_ValidateCoordinates(pMe, (int)info.cx, (int)info.cy);
  }
  return TRUE;
}

/*=====================================================================
FUNCTION Qalbum_ZoomOut

  DESCRIPTION:
    This function does zooming out

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    bDIB - Flag to tell if the image in DIB format.

  RETURN VALUE:
    TRUE if success, Otherwise FALSE
======================================================================*/
static boolean Qalbum_ZoomOut(Qalbum *pMe, boolean bDIB)
{
  AEEBitmapInfo  info;
  int     cx = 0;
  int     cy = 0;
  IBitmap *pBmp;
  AEERect rc;

  if (!pMe)
    return FALSE;

  //Get the zoomed image info.
  pBmp = pMe->m_pZoomImg ? pMe->m_pZoomImg : pMe->m_pEditIn;
  if (IBITMAP_GetInfo(pBmp, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
  {
    MSG_HIGH("Qalbum_ZoomOut failed - IBITMAP_GetInfo.", 0, 0, 0);
    return FALSE;
  }
  if (!pMe->m_pZoomImg)
  {
    //Get original display image if image is not 2 times larger than display size.
    rc = QCamDisp_GetResizedRect(pMe->m_dispRC, (int16)info.cx, (int16)info.cy);
    if (!Qalbum_DoZooming(pMe, (uint16)rc.dx, (uint16)rc.dy, bDIB))
    {
      MSG_HIGH("Qalbum_ZoomOut failed.", 0, 0, 0);
      return FALSE;
    }
    info.cx = (uint32)rc.dx;
    info.cy = (uint32)rc.dy;
  }

  if (!pMe->m_bMaxZoom) // zoom out 2x when not in max zoom
  {
    pMe->m_ratio   = pMe->m_ratio / 2;
    pMe->m_xOffset = pMe->m_xOffset / 2;
    pMe->m_yOffset = pMe->m_yOffset / 2;
    cx             = ((int)info.cx / 4) * 2;  // dx must be even #
    cy             = (int)info.cy / 2;

    // back to original size?
    if ((cx <= pMe->m_dispRC.dx) && (cy <= pMe->m_dispRC.dy))
    {
      pMe->m_xSrc    = 0;
      pMe->m_ySrc    = 0;
      pMe->m_xOffset = 0;
      pMe->m_yOffset = 0;
      pMe->m_bZoom = FALSE;
    }
    else // still in zoom, but 2x smaller
    {
      //Update pMe->m_xSrc and pMe->m_ySrc
      Qalbum_ValidateCoordinates(pMe, cx, cy);
    }

    if (!Qalbum_DoZooming(pMe, (uint16)cx, (uint16)cy, bDIB))
    {
      MSG_HIGH("Qalbum_ZoomOut failed.", 0, 0, 0);
      return FALSE;
    }
  }
  else // zoom out from max zoom
  {
    pMe->m_bMaxZoom = FALSE; // clear flag
    pMe->m_xOffset  = (int)((pMe->m_xOffset * pMe->m_ratio) / 100);
    pMe->m_yOffset  = (int)((pMe->m_yOffset * pMe->m_ratio) / 100);

    if (((int16)info.cx <= pMe->m_dispRC.dx) && ((int16)info.cy <= pMe->m_dispRC.dy))
    {
      // back to original size
      pMe->m_xSrc    = 0;
      pMe->m_ySrc    = 0;
      pMe->m_xOffset = 0;
      pMe->m_yOffset = 0;
      pMe->m_bZoom   = FALSE;
    }
    else
    {
      //Update pMe->m_xSrc and pMe->m_ySrc
      Qalbum_ValidateCoordinates(pMe, (int)info.cx, (int)info.cy);
    }
  }

  return TRUE;
}

/*=====================================================================
FUNCTION Qalbum_PanUp

  DESCRIPTION:
    This function does up pan.

  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
    void
======================================================================*/
static void Qalbum_PanUp(Qalbum *pMe)
{
  if (pMe)
  {
    int16 yDelta = pMe->m_dispRC.dy / 4;

    // Don't pan out of the dispRC
    if (pMe->m_ySrc > yDelta)
    {
      pMe->m_ySrc    = pMe->m_ySrc - yDelta;
      pMe->m_yOffset = pMe->m_yOffset - yDelta;
    }
    else
    {
      pMe->m_ySrc    = 0;
      pMe->m_yOffset = pMe->m_yOffset - pMe->m_ySrc;
    }
  }
}

/*=====================================================================
FUNCTION Qalbum_PanDown

  DESCRIPTION:
    This function does down pan.

  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
    void
======================================================================*/
static void Qalbum_PanDown(Qalbum *pMe, int16 cy)
{
  if (pMe)
  {
    int16 yDelta = pMe->m_dispRC.dy / 4;

    if ((pMe->m_ySrc + yDelta) < (cy - pMe->m_dispRC.dy))
    {
      pMe->m_ySrc    = pMe->m_ySrc + yDelta;
      pMe->m_yOffset = pMe->m_yOffset + yDelta;
    }
    else
    {
      pMe->m_ySrc    = cy - pMe->m_dispRC.dy;
      pMe->m_yOffset = pMe->m_yOffset + ((cy - pMe->m_dispRC.dy) - pMe->m_ySrc);
    }
  }
}

/*=====================================================================
FUNCTION Qalbum_PanLeft

  DESCRIPTION:
    This function does left pan.

  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
    void
======================================================================*/
static void Qalbum_PanLeft(Qalbum *pMe)
{
  if (pMe)
  {
    int16 xDelta = pMe->m_dispRC.dx / 4;

    // Don't pan out of the dispRC
    if (pMe->m_xSrc > xDelta)
    {
      pMe->m_xSrc    = pMe->m_xSrc - xDelta;
      pMe->m_xOffset = pMe->m_xOffset - xDelta;
    }
    else
    {
      pMe->m_xSrc    = 0;
      pMe->m_xOffset = pMe->m_xOffset - pMe->m_xSrc;
    }
  }
}


/*=====================================================================
FUNCTION Qalbum_PanRight

  DESCRIPTION:
    This function does right pan.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    cx  - image width

  RETURN VALUE:
    void
======================================================================*/
static void Qalbum_PanRight(Qalbum *pMe, int16 cx)
{
  if (pMe)
  {
    int16 xDelta = pMe->m_dispRC.dx / 4;

    if ((pMe->m_xSrc + xDelta) < (cx - pMe->m_dispRC.dx))
    {
      pMe->m_xSrc    = pMe->m_xSrc + xDelta;
      pMe->m_xOffset = pMe->m_xOffset + xDelta;
    }
    else
    {
      pMe->m_xSrc    = cx - pMe->m_dispRC.dx;
      pMe->m_xOffset = pMe->m_xOffset + ((cx - pMe->m_dispRC.dx) - pMe->m_xSrc);
    }
  }
}

/* ==========================================================================
FUNCTION Qalbum_OnZoom
  DESCRIPTION
    This is the handler for zoom option. Zoom up 2x at a time until full size.
    When in zoom, image is in m_pZoomImg.
    When in max zoom, image is in m_pEditIn.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key event

  RETURN VALUE:
   Always return FALSE because we don't want the photo to be redrawn elsewhere.

============================================================================= */
static boolean Qalbum_OnZoom(Qalbum *pMe, uint16 key)
{
  AEEBitmapInfo  info;
  IDIB   *pDIB   = NULL;
  IYCbCr *pYCbCr = NULL;
  boolean status = TRUE;
  boolean bDIB = TRUE;

  if (!pMe)
    return FALSE;

  if (IBITMAP_QueryInterface(pMe->m_pEditIn, AEECLSID_DIB, (void** )&pDIB) == SUCCESS)
    pYCbCr = NULL;
  else if (IBITMAP_QueryInterface(pMe->m_pEditIn, AEEIID_YCBCR, (void** )&pYCbCr) == SUCCESS)
  {
    bDIB = FALSE;
    pDIB = NULL;
  }
  else
  {
    status = FALSE;
    goto Done;
  }

  switch (key)
  {
  case AVK_SELECT:
    if (IBITMAP_GetInfo(pMe->m_pEditIn, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
    {
      MSG_HIGH("Qalbum_OnZoom failed - IBITMAP_GetInfo.", 0, 0, 0);
      status = FALSE;
      goto Done;
    }

    if (!pMe->m_bZoom)
    {
      Qalbum_UpdateInIPLImage(pMe, IDS_ZOOM);
      if (!Qalbum_InitZoomIn(pMe, info))
      {
        status = FALSE;
        goto Done;
      }
    }

    // How much can we zoom in?
    if (!pMe->m_bMaxZoom)
    {
      if (!Qalbum_ZoomIn(pMe, bDIB, info))
      {
        status = FALSE;
        goto Done;
      }
    }
    else // cannot zoom in at max zoom
    {
      status = FALSE;
    }
    break;

  case AVK_STAR:
    // Zoom out only if had zoomed in
    if (pMe->m_bZoom)
    {
      if (!Qalbum_ZoomOut(pMe, bDIB))
      {
        status = FALSE;
        goto Done;
      }
    }
    else
    {
      status = FALSE;
    }
    break;

  // Panning is only possible if in zoom
  case AVK_UP:
    if (pMe->m_bZoom)
    {
      if (pMe->m_ySrc == 0) // already at the top
      {
        status = FALSE;
      }
      else
      {
        Qalbum_PanUp(pMe);
      }
    }
    else // not in zoom, no panning
    {
      status = FALSE;
    }
    break;

  case AVK_DOWN:
    if (pMe->m_bZoom)
    {
      IBitmap *pBmp = pMe->m_bMaxZoom ? pMe->m_pEditIn : pMe->m_pZoomImg;

      if (IBITMAP_GetInfo(pBmp, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
      {
        status = FALSE;
        goto Done;
      }

      if (pMe->m_ySrc >= (int16)info.cy - pMe->m_dispRC.dy) // at the bottom
      {
        status = FALSE;
      }
      else
      {
        Qalbum_PanDown(pMe, (int16)info.cy);
      }
    }
    else // not zoom
    {
      status = FALSE;
    }
    break;

  case AVK_LEFT:
    if (pMe->m_bZoom)
    {
      if (pMe->m_xSrc == 0) // at far left
      {
        status = FALSE;
      }
      else
      {
        Qalbum_PanLeft(pMe);
      }
    }
    else
    {
      status = FALSE;
    }
    break;

  case AVK_RIGHT:
    if (pMe->m_bZoom)
    {
      IBitmap *pBmp = pMe->m_bMaxZoom ? pMe->m_pEditIn : pMe->m_pZoomImg;

      if (IBITMAP_GetInfo(pBmp, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
      {
        status = FALSE;
        goto Done;
      }

      if (pMe->m_xSrc >= (int16)info.cx - pMe->m_dispRC.dx) // at far right
      {
        status = FALSE;
      }
      else
      {
        Qalbum_PanRight(pMe, (int16)info.cx);
      }
    }
    else // not zoom
    {
      status = FALSE;
    }
    break;

  default:
    break;
  }

Done:
  if (status == TRUE)
  {
    Qalbum_DisplayZoomImage(pMe);
    MSG_HIGH("Zoom or pan operation successful", 0, 0, 0);
  }
  else
  {
    (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, FALSE);
    MSG_HIGH("Cannot zoom or pan further", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  }
  CAM_RELEASEIF(pDIB);
  CAM_RELEASEIF(pYCbCr);
  if (!pMe->m_bZoom)
  {
    //release m_pZoomImg when no zooming
    CAM_RELEASEIF(pMe->m_pZoomImg);
  }
  return FALSE;
}

#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
/* ==========================================================================
FUNCTION Qalbum_OnPrint
  DESCRIPTION
    This is the handler for print options. It sends the photo to a printer
    via Bluetooth connection, assuming the connection is already established.

    If neither BIP nor BPP is supported, then just return.
    If either BIP or BPP is supported, then use the supported protocol.
    If both BIP and BPP are supported, then use the user-selectable choice.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - user selected key id

  RETURN VALUE:
   TRUE if photo image needs redrawn FALSE otherwise.
   We always return FAlSE here because this handler doesn't alter the image.

============================================================================= */
static boolean Qalbum_OnPrint(Qalbum *pMe, uint16 key)
{
  if (!pMe)
  {
    MSG_HIGH("Qalbum_OnPrint got NULL pMe!", 0, 0, 0);
    return FALSE;
  }

  //Only implement feature on Sel key
  if (key != (uint16)AVK_SELECT)
    return FALSE;

  if (NO_BT_PRINT_ASSIGNED == pMe->m_ePrintMenuActive)
  {
    MSG_HIGH("No Print Choice selected, poping up menu", 0, 0, 0);
    pMe->eState = QCAM_STATE_MENU;
    Qalbum_BuildPrintOptionMenu(pMe);
  }
  else
  {
    MSG_HIGH("Print Choice Already selected %d, try to print", pMe->m_ePrintMenuActive, 0, 0);
    if (!Qalbum_DoPrint(pMe))
    {
      // Report error and redraw the image
      Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_PRINT_FAIL);
    }
  }

  return FALSE;
  //lint -save -e818 pMe cannot be declared as const because prototype is fixed
}
//lint -restore
/*===========================================================================
FUNCTION Qalbum_DoPrint

  DESCRIPTION
    This function calls the proper printing apps to print the selected image.

  PARAMETERS:
    pMe - pointer to Qalbum data struct

  RETURN VALUE:
    TRUE if success, FALSE is failed
===========================================================================*/
static boolean Qalbum_DoPrint(Qalbum *pMe)
{
  if (!pMe)
  {
    MSG_HIGH("Qalbum_DoPrint got NULL pMe!", 0, 0, 0);
    return FALSE;
  }

  switch (pMe->m_ePrintMenuActive)
  {
  case NO_BT_PRINT_ASSIGNED:
    // If we somehow got here, call OnPrint
    (void)Qalbum_OnPrint(pMe, (uint16)AVK_SELECT);
    break;
#if defined(FEATURE_BT_EXTPF_BPP) && !defined(FEATURE_SKIP_BPP)
  case BPP_BT_PRINT:
    {
      IFile  *pFile;
      uint32  filenameLen = 0;
      BTApp_BPPPrintDetails *printDetails;

      MSG_HIGH("Trying to Print with BPP", 0, 0, 0);

      if (pMe->m_sCurrentFileName[0] == 0)
      {
        MSG_HIGH("BPP print got empty filename!", 0, 0, 0);
        return FALSE;
      }

      /* Make sure the requested file exists. */
      pFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, pMe->m_sCurrentFileName, _OFM_READ);
      if (!pFile)
      {
        MSG_HIGH("BPP print can't open file %s!", pMe->m_sCurrentFileName, 0, 0);
        return FALSE;
      }
      (void)IFILE_Release(pFile);

      /* Make sure this is a JPEG file. */
      if (!CAM_ISTYPEFILE(QCAM_JPEG_EXTENSION, pMe->m_sCurrentFileName))
      {
        MSG_HIGH("BPP print only works on JPEG images.", 0, 0, 0);
        return FALSE;
      }

      /* Fill in the BPP structure with the printing information. */
      printDetails = (BTApp_BPPPrintDetails*)MALLOC(sizeof(BTApp_BPPPrintDetails));
      if (!printDetails)
      {
        MSG_HIGH("BPP print can't allocate print details!", 0, 0, 0);
        return FALSE;
      }
      filenameLen = STRLEN(pMe->m_sCurrentFileName) + 1;
      printDetails->pPrintFileName = MALLOC(filenameLen);
      if (!printDetails->pPrintFileName)
      {
        MSG_HIGH("BPP print can't allocate print filename!", 0, 0, 0);
        CAM_FREEIF(printDetails);
        return FALSE;
      }
      printDetails->uMIMMediaType = AEEBT_BPP_DOC_FORMAT_JPEG;

      (void)STRLCPY(printDetails->pPrintFileName, pMe->m_sCurrentFileName, filenameLen);
      MSG_MED("BPP printing %s", pMe->m_sCurrentFileName, 0, 0);

      //Launch the BlueTooth application to print the photo.
      //If failed launch or failed post event, free the buffer.
      //Otherwise it is the resposibility of BTApp to free the printDetail buffers.
      if (ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_BLUETOOTH_APP) == SUCCESS)
      {
        MSG_MED("Posting bluetooth BPP print event.", 0, 0, 0);
        if (ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,
                               AEECLSID_BLUETOOTH_APP,
                               EVT_USER, EVT_BPP_SIMPLE_PUSH,
                               (uint32)printDetails) == FALSE)
        {
          MSG_HIGH("Failed to put image.", 0, 0, 0);
          CAM_FREEIF(printDetails->pPrintFileName);
          CAM_FREEIF(printDetails);
          return FALSE;
        }
      }
      else
      {
        MSG_HIGH("Failed to launch BTApp.", 0, 0, 0);
        CAM_FREEIF(printDetails->pPrintFileName);
        CAM_FREEIF(printDetails);
        return FALSE;
      }
    }
    break;
#endif  //defined(FEATURE_BT_EXTPF_BPP) && !defined(FEATURE_SKIP_BPP)

#ifdef FEATURE_BT_EXTPF_BIP
  case BIP_BT_PRINT:
    {
      AEEBitmapInfo bmpInfo;
      IFile   *pFile;
      FileInfo info;
      uint32   len1, len2;
      char     imgDesc[AEEBT_BIP_MAX_IMAGE_DESC_LEN] =
                      "<image-descriptor version=\"1.0\">\
                       <image encoding=\"JPEG\"";
      BTApp_BIPPrintDetails *printDetails;

      MSG_HIGH("Trying to Print with BIP", 0, 0, 0);

      // Create bip_xml2.txt file
      // Fill in pixel param
      if (IBITMAP_GetInfo(pMe->m_pEditIn, &bmpInfo, sizeof(AEEBitmapInfo)) != SUCCESS)
      {
        MSG_HIGH("BIP print can't get image info!", 0, 0, 0);
        return FALSE;
      }
      len1 = STRLEN(imgDesc);
      (void)SNPRINTF(imgDesc + len1, sizeof(imgDesc) - len1, " pixel=\"%d*%d\"", bmpInfo.cx, bmpInfo.cy);

      // Fill in size param
      pFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, pMe->m_sCurrentFileName, _OFM_READ);
      if (!pFile)
      {
        MSG_HIGH("BIP print can't open file %s!", pMe->m_sCurrentFileName, 0, 0);
        return FALSE;
      }
      if (IFILE_GetInfo(pFile, &info) == SUCCESS)
      {
        len1 = STRLEN(imgDesc);
        (void)SNPRINTF(imgDesc + len1, sizeof(imgDesc) - len1, " size=\"%d\"/>\n</image-descriptor>", info.dwSize);
      }
      (void)IFILE_Release(pFile);

      // Remove old xml file if exists
      if (IFILEMGR_Test(pMe->m_pIFileMgr, BTAPP_BIP_XML_FILENAME_W_PATH) == SUCCESS)
      {
        if (IFILEMGR_Remove(pMe->m_pIFileMgr, BTAPP_BIP_XML_FILENAME_W_PATH) == EFAILED)
        {
          MSG_HIGH("BIP print can't remove %s!", BTAPP_BIP_XML_FILENAME_W_PATH, 0, 0);
          return FALSE;
        }
      }
      // Write the new xml file
      pFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr,
                                BTAPP_BIP_XML_FILENAME_W_PATH, _OFM_CREATE);
      if (!pFile)
      {
        MSG_HIGH("BIP print can't make %s!", BTAPP_BIP_XML_FILENAME_W_PATH, 0, 0);
        return FALSE;
      }
      len1 = STRLEN(imgDesc);
      len2 = IFILE_Write(pFile, imgDesc, len1);
      (void)IFILE_Release(pFile);

      // Launch bt app to print it
      if (len1 == len2)
      {
        printDetails = (BTApp_BIPPrintDetails*)MALLOC(sizeof(BTApp_BIPPrintDetails));
        if (!printDetails)
        {
          MSG_HIGH("BIP print can't allocate buffer for printDetails!", 0, 0, 0);
          return FALSE;
        }

        printDetails->pPrintFileName = MALLOC(STRLEN(pMe->m_sCurrentFileName) + 1);
        printDetails->pFileDescName = MALLOC(STRLEN(BTAPP_BIP_XML_FILENAME_W_PATH) + 1);

        if (!printDetails->pPrintFileName || !printDetails->pFileDescName)
        {
          MSG_HIGH("BIP print can't allocate buffer for file name!", 0, 0, 0);
          CAM_FREEIF(printDetails->pPrintFileName);
          CAM_FREEIF(printDetails->pFileDescName);
          CAM_FREEIF(printDetails);
          return FALSE;
        }

        (void)MEMCPY(printDetails->pPrintFileName, pMe->m_sCurrentFileName,
                     STRLEN(pMe->m_sCurrentFileName));
        (void)MEMCPY(printDetails->pFileDescName, BTAPP_BIP_XML_FILENAME_W_PATH,
                     STRLEN(BTAPP_BIP_XML_FILENAME_W_PATH));
        printDetails->pPrintFileName[STRLEN(pMe->m_sCurrentFileName)] = '\0';
        printDetails->pFileDescName[STRLEN(BTAPP_BIP_XML_FILENAME_W_PATH)] = '\0';

        //Launch the BlueTooth application to print the photo.
        //If failed to launch or failed to post event, free the buffer.
        //Otherwise it is the resposibility of BTApp to free the printDetail buffers.
        if (ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_BLUETOOTH_APP) == SUCCESS)
        {
          if (ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,
                                 AEECLSID_BLUETOOTH_APP,
                                 EVT_USER, EVT_BIP_PUT_IMAGE,
                                 (uint32)printDetails) == FALSE)
          {
            MSG_HIGH("Failed to put image.", 0, 0, 0);
            CAM_FREEIF(printDetails->pPrintFileName);
            CAM_FREEIF(printDetails->pFileDescName);
            CAM_FREEIF(printDetails);
            return FALSE;
          }
        }
        else
        {
          MSG_HIGH("Failed to launch BTApp.", 0, 0, 0);
          CAM_FREEIF(printDetails->pPrintFileName);
          CAM_FREEIF(printDetails->pFileDescName);
          CAM_FREEIF(printDetails);
          return FALSE;
        }
      }
    } //case of BIP
    break;
#endif // FEATURE_BT_EXTPF_BIP

#if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))
  case PICTBRIDGE_PRINT:
    MSG_HIGH("Trying to Print with PictBridge", 0, 0, 0);

    if (ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_PBPRINT, pMe->m_sCurrentFileName) != SUCCESS)
    {
      MSG_HIGH("Failed to launch PBPrint.", 0, 0, 0);
      return FALSE;
    }
    break;
#endif // #if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))
  default:
    MSG_HIGH("Bad choice %d. No print support", pMe->m_ePrintMenuActive, 0, 0);
    return FALSE;
  }

  return TRUE;
  //
  //BTApp uses the printDetails and is responsible for freeing it.
  //It could cause memory leak if BTApp does not free the memory for any reason.
  //

//lint -save -e818 pMe cannot be declared as const because prototype is fixed
}
  //lint -restore
#endif  // FEATURE_BT_EXTPF_BIP || FEATURE_BT_EXTPF_BPP || FEATURE_PICTBRIDGE

/*===========================================================================
FUNCTION Qalbum_OnEdit

  DESCRIPTION
    This is the handler for Edit options. It builds a new edit menu and
    switch to the QCAM_STATE_EDIT state.

  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
   TRUE if photo image needs redrawn FALSE otherwise.
   We always return FAlSE here because this handler doesn't alter the image.
===========================================================================*/
static boolean Qalbum_OnEdit(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return FALSE;

  MSG_MED("Start Qalbum_OnEdit", 0, 0, 0);

  //Only responds to SELECT key
  if (key == (uint16)AVK_SELECT)
  {
    //Build the edit menu
    Qalbum_BuildEditList(pMe, EditIDList, ARR_SIZE(EditIDList));

    //Go to new state
    pMe->m_nEditProcessedVal = 0;
    pMe->eState = QCAM_STATE_EDIT;

    //Copy pEditIn into pEditOut
    if (!Qalbum_IBitmapCopy(pMe->m_pEditIn, &pMe->m_pEditOut))
    {
      Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_Exit, IDS_MEM_FULL);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      return FALSE;
    }

    Qalbum_DisplayListNavArrows(pMe);
    Qalbum_DisplayEditMenuName(pMe, 0);
  }
  return FALSE; // don't update the photo
}

#ifndef FEATURE_IPL_LITE
/* ==========================================================================
FUNCTION Qalbum_OnAdvanced
  DESCRIPTION
    This is the handler for Advanced Edit options.
    It builds a new advanced edit menu and switch to the
    QCAM_STATE_ADV_EDIT state.

  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
   TRUE if photo image needs redrawn FALSE otherwise.
   We always return FAlSE here because this handler doesn't alter the image.

============================================================================= */
static boolean Qalbum_OnAdvanced(Qalbum *pMe, uint16 key)
{
  if (!pMe)
    return FALSE;

  //Only responds to SELECT key
  if (key == (uint16)AVK_SELECT)
  {
    //Build the edit menu
    Qalbum_BuildEditList(pMe, AdvEditIDList, ARR_SIZE(AdvEditIDList));

    //Go to new state
    pMe->m_nEditProcessedVal = 0;
    pMe->eState = QCAM_STATE_ADV_EDIT;

    //Copy pEditIn into pEditOut
    if (!Qalbum_IBitmapCopy(pMe->m_pEditIn, &pMe->m_pEditOut))
    {
      Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_Exit, IDS_MEM_FULL);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      return FALSE;
    }

    Qalbum_DisplayListNavArrows(pMe);
    Qalbum_DisplayAdvEditMenuName(pMe, 0);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  }
  return FALSE; // don't update the photo
}
#endif //FEATURE_IPL_LITE

/*===========================================================================
FUNCTION Qalbum_GetDisplaySizes

  DESCRIPTION
    This function calculate the display sizes based on device information.
    It updates the pMe->m_dispSizes struct.

  PARAMETERS:
    pMe - Pointer to QCam struct

  RETURN VALUE:
   None
===========================================================================*/
static void Qalbum_GetDisplaySizes(Qalbum *pMe)
{
  uint16 wOffset;



  if (!pMe)
    return;

  ISHELL_GetDeviceInfo (pMe->a.m_pIShell, &pMe->di);

  // Check for SC1X screen.
  if ((pMe->di.cxScreen <= SMALL_SCREEN_WIDTH) && (pMe->di.cyScreen <= SMALL_SCREEN_HEIGHT))
  {
    pMe->m_bSmallScreen = TRUE;
  }
  else
  {
    pMe->m_bSmallScreen = FALSE;
  }

#ifdef FEATURE_CAMERA7500
  //Check if it uses larger screen
  if (QCamDisp_IsSizeScreen(pMe->di.cxScreen, pMe->di.cyScreen + QCAM_LCD_OFFSET,
                            (int)QCAM_SIZE_VGA) ||
                            (pMe->di.cxScreen == QCAM_WVGA_WIDTH))
  {
    int16 font_size_vga;
    gaiQalbum.pszMIF = QALBUM_VGA_RES_FILE;
    pMe->m_pResFile = QALBUM_VGA_RES_FILE;
    pMe->m_softMenuHeight = QCAM_SOFT_MENU_HEIGHT_VGA;
    pMe->m_nNavArrowWidth = QCAM_NAV_ARROW_WIDTH_VGA;

#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif //FEATURE_CAMERA_WIDESIZE

    // Check for font size and assign the larger one to pMe->m_nTitleHeight
    font_size_vga =	(int16)IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay,
                                                   QCAM_FONT_TITLE,
                                                   NULL, NULL);
    if(font_size_vga > QCAM_PICTURE_Y_POS_VGA)
    {
      pMe->m_nTitleHeight = font_size_vga + 2;
    }
    else
    {
      pMe->m_nTitleHeight = QCAM_PICTURE_Y_POS_VGA;
    }
  }
  else
#endif //FEATURE_CAMERA7500
  {
    pMe->m_pResFile = QALBUM_RES_FILE;
    pMe->m_softMenuHeight = QCAM_SOFT_MENU_HEIGHT;
    pMe->m_nTitleHeight = QCAM_PICTURE_Y_POS;
    pMe->m_nNavArrowWidth = QCAM_NAV_ARROW_WIDTH;
  }

  //Get Dir width;
  wOffset = QCAM_DIR_X_START * 2 + (QCAM_DIRS_X - 1) * QCAM_DIR_X_OFFSET;
  pMe->m_dispSizes.wDirWidth = (pMe->di.cxScreen - wOffset) / QCAM_DIRS_X;
  //Get Dir height
  wOffset = (uint16)(pMe->m_nTitleHeight * 2 + (QCAM_DIRS_Y - 1) * QCAM_DIR_Y_OFFSET);
  pMe->m_dispSizes.wDirHeight = (pMe->di.cyScreen - wOffset) / QCAM_DIRS_Y;

  //Get thumbnail image width
  wOffset = (uint16)(QCAM_THUMBNAIL_X_START * 2 +
                     (QCAM_THUMBNAILS_X - 1) * QCAM_THUMBNAIL_X_OFFSET);
  pMe->m_dispSizes.wThumbnailWidth = (pMe->di.cxScreen - wOffset) / QCAM_THUMBNAILS_X;
  //Get thumbnail image height
  wOffset = (uint16)((pMe->m_nTitleHeight + 1) + pMe->m_softMenuHeight +
                     (QCAM_THUMBNAILS_Y - 1) * QCAM_THUMBNAIL_Y_OFFSET);

  pMe->m_dispSizes.wThumbnailHeight = (pMe->di.cyScreen - wOffset) / QCAM_THUMBNAILS_Y;

#ifdef FEATURE_JPEGD_RESIZE
  /* Convert Odd dimensons to Even */ 
  ODD_2_EVEN(pMe->m_dispSizes.wThumbnailWidth);
  ODD_2_EVEN(pMe->m_dispSizes.wThumbnailHeight);
#endif
  //Get photo image width
  pMe->m_dispSizes.wPhotoWidth  = pMe->di.cxScreen;

  //Get photo image height
  pMe->m_dispSizes.wPhotoHeight = (pMe->di.cyScreen - pMe->m_nTitleHeight) - pMe->m_softMenuHeight;
}

/*===========================================================================
FUNCTION Qalbum_IBitmapCopy

  DESCRIPTION
    This function creates a duplicate copy of a IBitmap

  PARAMETERS:
    pSrcBmp  - pointer to source bitmap
    ppDstBmp - pointer to destination bitmap interface

  RETURN VALUE:
   TRUE if successful, FALSE otherwise
===========================================================================*/
boolean Qalbum_IBitmapCopy(IBitmap * pSrcBmp, IBitmap ** ppDstBmp)
{
  IDIB   * pDIB   = NULL;
  IDIB   * pDIB2  = NULL;
  IYCbCr * pYCbCr = NULL;
  boolean  status = TRUE;
  int      i;
  int32 result = SUCCESS;

  if (!pSrcBmp || !ppDstBmp)
    return FALSE;

  // free the dst bitmap
  CAM_RELEASEIF(*ppDstBmp);

  // check src bitmap format
  if (IBITMAP_QueryInterface(pSrcBmp, AEECLSID_DIB, (void** )&pDIB) == SUCCESS)
  {
    // create bitmap
    result = IBITMAP_CreateCompatibleBitmap(pSrcBmp, ppDstBmp, pDIB->cx, pDIB->cy);
    if (result != SUCCESS)
    {
      // out of memory
      MSG_HIGH("Failed to create Compatible DIB Bitmap with Error %d", result, 0, 0);
      status = FALSE;
      goto Done;
    }
    else
    {
      // copy the image content
      if (IBITMAP_QueryInterface(*ppDstBmp, AEECLSID_DIB, (void **)&pDIB2) != SUCCESS)
      {
        MSG_HIGH("Failed to create pDIB2", 0, 0, 0);
        status = FALSE;
        goto Done;
      }
      if (pDIB->cx & 1)
      {
        // odd width image was rounded up to even # by IBITMAP_CreateCompatibleBitmap
        pDIB2->cx++;
        for (i = 0; i < pDIB->cy; i++ )
        {
          (void)MEMCPY(pDIB2->pBmp + pDIB2->nPitch*i,
                       pDIB->pBmp + pDIB->nPitch*i, (uint16)pDIB->nPitch);
        }
      }
      else
      {
        (void)MEMCPY(pDIB2->pBmp, pDIB->pBmp, pDIB->nPitch * pDIB->cy);
      }
      pDIB2->nColorScheme = pDIB->nColorScheme;
    }
  }
  else if (IBITMAP_QueryInterface(pSrcBmp, AEEIID_YCBCR, (void** )&pYCbCr) == SUCCESS)
  {
    if (IBITMAP_CreateCompatibleBitmap(pSrcBmp, ppDstBmp, (uint16)pYCbCr->cx,
                                       (uint16)pYCbCr->cy) != SUCCESS)
    {
      MSG_HIGH("Failed to Create compatible YCBCR bitmap", 0, 0, 0);
      status = FALSE;
      goto Done;
    }

    if (pYCbCr->uScheme == IYCBCR_COLORSCHEME_422LP)
    {
      (void)MEMCPY(((IYCbCr *)*ppDstBmp)->pLuma, pYCbCr->pLuma, pYCbCr->cx * pYCbCr->cy * 2);
    }
    else if (pYCbCr->uScheme == IYCBCR_COLORSCHEME_420LP)
    {
      (void)MEMCPY(((IYCbCr *)*ppDstBmp)->pLuma, pYCbCr->pLuma, pYCbCr->cx * pYCbCr->cy * 3 / 2);
    }
    else
    {
      // unsupported YCbCr color scheme
      MSG_HIGH("unsupported YCbCr color scheme", 0, 0, 0);
      status = FALSE;
      goto Done;
    }
  }
  else
  {
    // unsupported bitmap
    MSG_HIGH("unsupported bitmap", 0, 0, 0);
    status = FALSE;
  }

Done:
  CAM_RELEASEIF(pDIB);
  CAM_RELEASEIF(pDIB2);
  CAM_RELEASEIF(pYCbCr);

  return status;
}

/*===========================================================================
FUNCTION Qalbum_UpdateInIPLImage

  DESCRIPTION
    This function reset the m_pEditIn and m_pEditOut buffer for editing

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    selection - edit item ID

  RETURN VALUE:
   TRUE if rotate action handled
===========================================================================*/
void Qalbum_UpdateInIPLImage(Qalbum* pMe, uint16 selection)
{
  if (!pMe)
    return;

  if (pMe->m_bEditProcessed && pMe->m_wEditProcessedBy != selection)
  {
    (void)Qalbum_IBitmapCopy(pMe->m_pEditOut, &pMe->m_pEditIn);
    pMe->m_bEditProcessed    = FALSE;
  }
  pMe->m_wEditProcessedBy = selection;
  return;
} /* END Qalbum_UpdateInIPLImage */

/*=====================================================================
FUNCTION Qalbum_EditEffect

  DESCRIPTION:
    This is a helper function to execute IIPL_Effect feature.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    type - Effect type

  RETURN VALUE:
    boolean
======================================================================*/
boolean Qalbum_EditEffect(Qalbum *pMe, IIPLEffectType type, boolean bUpdate)
{
  int32 nVal = 0;
  int retVal = AEE_SUCCESS;

  if (!pMe)
    return FALSE;

  if (!bUpdate)
    nVal = pMe->m_nEditProcessedVal; //For edit effect in step.
  if (pMe->m_nEditProcessedVal == QCAM_POSTERIZE_MAX)
    nVal = 0;
  if((type == IIPL_SOLARIZE) && (nVal == QCAM_SOLARIZE_MAX))
  {
    Qalbum_EditUndo(pMe);
    return TRUE;
  }
  retVal = IIPL_Effect(pMe->m_pIIpl, pMe->m_pEditIn, pMe->m_pEditOut, type, nVal);
  // Checking for the IIPL Edit Error
  if (Qalbum_DisplayError(pMe, retVal) != SUCCESS)
  {
    if (bUpdate)
      pMe->m_bEditProcessed = FALSE; //For edit effect in one time

    MSG_HIGH("IIPL_Effect failed.", 0, 0, 0);
    return FALSE;
  }
  return TRUE;
}

#ifdef FEATURE_GEOTAGGING
/*===========================================================================
FUNCTION Qalbum_AddGeoTags

  DESCRIPTION
    This function adds the current Gps tags to the existent images in the Qalbum
  PARAMETERS:
    pMe - Pointer to Qalbum struct
    
  RETURN VALUE:
   TRUE if edit value updated
===========================================================================*/
void Qalbum_AddGeoTags(Qalbum *pMe)
{
  if( pMe->m_bPNGFile)
  {
    // string to be created, so commenting for time being
   Qalbum_DisplayTimedMsg(pMe, NULL, IDS_GEOTAGGING_FAILED);
  }
  else
  {
    Qalbum_GetGPSInfo(pMe);
  } 
}

/*===========================================================================
FUNCTION Qalbum_GetGPSInfo

  DESCRIPTION
  This function gets the current gps information

  PARAMETERS:
    pMe - Pointer to  Qalbum struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qalbum_GetGPSInfo(Qalbum *pMe)
{
  if (!pMe)
    return;

  Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

    // Get GPS info
    if (pMe->m_pIPosDet)
    {
      int result = 0;
      AEEGPSConfig config = {0};
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
          MSG_HIGH("EVT_QCAMERA_DIAG_GPS_SET error - GetGPSInfo.", 0, 0, 0);
      }
   }
 }
/*===========================================================================
FUNCTION Qalbum_GetGPSInfoCB

  DESCRIPTION
    GPS call back handler. Information from callback already saved in pMe->GPSInfoCB

  PARAMETERS:
    pUser - Pointer cast to  QCam struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_GetGPSInfoCB(void *pUser)
{
  Qalbum *pMe = (Qalbum*)pUser;
  boolean retry = FALSE;

  if (!pMe)
    return;
    switch(pMe->m_GPSInfo.status)
    {
    case AEEGPS_ERR_NO_ERR:
      Qalbum_ProceedWithGeoTagging(pMe);
      break;
    case AEEGPS_ERR_ACCURACY_UNAVAIL:
      if (pMe->m_nGPSAccuracy > AEEGPS_ACCURACY_LOWEST)
      {
       // pMe->m_eGPSState = QCAM_GPS_ENABLED;
        pMe->m_nGPSAccuracy--;
        retry = TRUE;
        break;
      }
      else
      {
      }
    case AEEGPS_ERR_GENERAL_FAILURE:
    case AEEGPS_ERR_TIMEOUT:
    case AEEGPS_ERR_INFO_UNAVAIL:
    default:
     Qalbum_DisplayTimedMsg(pMe, NULL, IDS_GEOTAGGING_FAILED);
     MSG_HIGH("Capturing GPS information FAILED", 0, 0, 0);
      break;
    }

  if (retry)
  {
    Qalbum_GetGPSInfo(pMe);
  }
  return;
}

/*===========================================================================
FUNCTION Qalbum_ProceedWithGeoTagging

  DESCRIPTION
  The acquired gps info is valid and this function would proceed with 
  adding the tags to the EXIF header of the image 

  PARAMETERS:

  RETURN VALUE:
    NONE
===========================================================================*/

void Qalbum_ProceedWithGeoTagging(Qalbum *pMe )
{
  OEMImageParmExt parm = {0};
  AEECLSID cls;

  GeoTagInfo exif_geo_tag_info;
  switch (pMe->m_JpegColorFormat)
  {
    case QALBUM_COLOR_FORMAT_YCBCR420LP:
      cls=AEECLSID_JPEG_YCBCR420LP_MAIN;
      break;
    case QALBUM_COLOR_FORMAT_YCBCR422LP:
      cls=AEECLSID_JPEG_YCBCR422LP_MAIN;
      break;
    case QALBUM_COLOR_FORMAT_RGB565:
    default:
      cls= AEECLSID_JPEG_RGB_MAIN;
      break;
  }
  if(ISHELL_CreateInstance(pMe->a.m_pIShell, cls,
                           (void **)&pMe->m_pDisplayImage) != SUCCESS)
  {
    MSG_HIGH("cannot create JPEG instance", 0, 0, 0);
    return;
  }
  if (QCam_GetCurrentItem(pMe->m_pPhotoList, &pMe->m_pCurrentPhoto, pMe->m_nCurrentPhoto))
  {
    (void)STRLCPY(exif_geo_tag_info.m_sCurrentFileName, pMe->m_pCurrentPhoto->fileName, FILE_NAME_SIZE);
  }

  exif_geo_tag_info.gps_info = MALLOC(sizeof(AEEGPSInfo));

  exif_geo_tag_info.gps_info->dwTimeStamp = pMe->m_GPSInfo.dwTimeStamp;
  exif_geo_tag_info.gps_info->wAltitude=  pMe->m_GPSInfo.wAltitude;
  exif_geo_tag_info.gps_info->dwLat=  pMe->m_GPSInfo.dwLat;
  exif_geo_tag_info.gps_info->dwLon=  pMe->m_GPSInfo.dwLon;

  //exif_geo_tag_info.m_nCallback =(void *)Qalbum_TaggingStatusNotify;
   
      /*exif_geo_tag_info.gps_info->dwTimeStamp = 315367890;
      exif_geo_tag_info.gps_info->wAltitude= 645;
      exif_geo_tag_info.gps_info->dwLat= 6897300;
      exif_geo_tag_info.gps_info->dwLon= -21810381;*/
	
  parm.pData = &exif_geo_tag_info;
  parm.uLen  = sizeof(GeoTagInfo);


  IIMAGE_ExtFunc(pMe->m_pDisplayImage,ADD_TAGS,
                            &parm, sizeof(OEMImageParmExt));    
  IIMAGE_Notify(pMe->m_pDisplayImage, Qalbum_TaggingStatusNotify, pMe);
  
  FREEIF(exif_geo_tag_info.gps_info);
  MSG_HIGH("Capturing GPS information SUCCESSFUL", 0, 0, 0);
}

/*===========================================================================
FUNCTION Qalbum_TaggingStatusNotify

  DESCRIPTION
  callback from OEM to UI informing the status of the tagging

  PARAMETERS:

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_TaggingStatusNotify(void * pUser, IImage * pImage, AEEImageInfo * pi, int nErr)
{
  Qalbum *pMe = (Qalbum *)pUser;

  if (!pMe)
    return;
  
  if (nErr != SUCCESS)
    Qalbum_DisplayTimedMsg(pMe, NULL, IDS_GEOTAGGING_FAILED);
  else
    Qalbum_DisplayTimedMsg(pMe, NULL, IDS_GEOTAGGING_DONE);
}

#endif /* FEATURE_GEOTAGGING */

/*===========================================================================
FUNCTION Qalbum_EditStep

  DESCRIPTION
    This function changes the setting value in a step
    On Up key - selected edit value moves up one step toward the max value
    On Down key - selected edit value moves down one step toward the min value
  PARAMETERS:
    pMe - Pointer to Qalbum struct
    sel - Selected edit ID
    key - Up/Down key ID
    step - specified value for each step
    max - maxmum value for selected edit item
    min - minmum value for selected edit item

  RETURN VALUE:
   TRUE if edit value updated
===========================================================================*/
boolean Qalbum_EditStep(Qalbum* pMe, uint16 sel, uint16 key, uint16 step, int32 max, int32 min)
{
  boolean bInRange = FALSE;

  if (!pMe)
    return FALSE;

  //reset the edit buffer
  Qalbum_UpdateInIPLImage(pMe, sel);
  if (pMe->m_pEditIn && pMe->m_pEditOut)
  {
    bInRange = TRUE;
    pMe->m_bEditProcessed = TRUE;

    if (key == (uint16)AVK_UP)
    {
      if (pMe->m_nEditProcessedVal == max)
      {
        (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, FALSE);
        bInRange = FALSE;
      }
      else
      {
        pMe->m_nEditProcessedVal += step;
        if (pMe->m_nEditProcessedVal > max)
        {
           pMe->m_nEditProcessedVal = max;
        }
      }
    }
    else
    {
      if (pMe->m_nEditProcessedVal == min)
      {
        (void)ISHELL_Beep(pMe->a.m_pIShell, BEEP_ERROR, FALSE);
        bInRange = FALSE;
      }
      else
      {
        pMe->m_nEditProcessedVal -= step;
        if (pMe->m_nEditProcessedVal < min)
        {
           pMe->m_nEditProcessedVal = min;
        }
      }
    }
  }
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  return bInRange;
}

/*===========================================================================
FUNCTION Qalbum_EditReturn

  DESCRIPTION
    Wrapper for the timer callback

  PARAMETERS:
    pMe    - Pointer to  Qalbum struct

  RETURN VALUE:
    NONE
===========================================================================*/
void Qalbum_EditReturn(Qalbum *pMe)
{
  if (!pMe)
    return;

  //In case it is directly called by the handler
  (void)ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
  pMe->m_bMsgTimerOn = FALSE;

  if ((pMe->eState == QCAM_STATE_VIEW) ||
      (pMe->eState == QCAM_STATE_EDIT) ||
      (pMe->eState == QCAM_STATE_ADV_EDIT))
  {
    MSG_MED("Qalbum_UpdateEditedImage on edit/adv edit return", 0, 0, 0);
    Qalbum_UpdateEditedImage(pMe);
  }
}

/*===========================================================================
FUNCTION Qalbum_EditUndo

  DESCRIPTION


  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
   None
===========================================================================*/
void Qalbum_EditUndo(Qalbum *pMe)
{
  if (!pMe)
    return;

  (void)Qalbum_IBitmapCopy(pMe->m_pEditIn, &pMe->m_pEditOut);
  pMe->m_bEditProcessed    = FALSE;
#ifndef FEATURE_IPL_LITE
  pMe->m_nEditProcessedVal = Qalbum_GetStartValue(pMe->m_wEditProcessedBy);
#else
  pMe->m_nEditProcessedVal = 0;
#endif //FEATURE_IPL_LITE
}

/*===========================================================================
FUNCTION Qalbum_DisplayZoomImage

  DESCRIPTION
    This function displays the zoomed image
  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
   None
===========================================================================*/
static void Qalbum_DisplayZoomImage(Qalbum *pMe)
{
  IDIB          * pDIB   = NULL;
  IYCbCr        * pYCbCr = NULL;
  IBitmap       * pZoomImg;
  int             retVal = AEE_SUCCESS;

  if (!pMe)
    return;

  if (pMe->m_bMaxZoom == TRUE)
  {
    pZoomImg = pMe->m_pEditIn;
  }
  else if (pMe->m_pZoomImg)
  {
    pZoomImg = pMe->m_pZoomImg;
  }
  else
    return;

  // Draw hourglass before drawing full image
  Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

  // Display image as is
  if (IBITMAP_QueryInterface(pZoomImg, AEECLSID_DIB, (void** )&pDIB) == SUCCESS)
  {
    retVal = QCamDisp_DrawImage(pMe->a.m_pIDisplay, pMe->m_pIIpl, pDIB, &pMe->m_dispRC,
                                pMe->m_xSrc, pMe->m_ySrc, FALSE);
  }
  else if (IBITMAP_QueryInterface(pZoomImg, AEEIID_YCBCR, (void** )&pYCbCr) == SUCCESS)
  {
    retVal = QCamDisp_DrawYCbCrImage(pMe->a.m_pIDisplay,  pMe->m_pIYCbCrFactory,
                                     pMe->m_pIIpl, pYCbCr, &pMe->m_dispRC,
                                     pMe->m_xSrc, pMe->m_ySrc, FALSE);
  }

  CAM_RELEASEIF(pDIB);
  CAM_RELEASEIF(pYCbCr);

  if (Qalbum_DisplayError(pMe, retVal) == AEE_SUCCESS)
  {
    // Update the title bar
    Qalbum_DisplayViewMenuName(pMe, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  }
}

/*===========================================================================
FUNCTION Qalbum_UpdateEditedImage

  DESCRIPTION
    This function updates the display with edited image. Edited images are in
    YCbCr422 format.

  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
   None
===========================================================================*/
void Qalbum_UpdateEditedImage(Qalbum *pMe)
{
  AEERect         rc;
  IBitmap       * pNewBmp;
  int             retVal = AEE_SUCCESS;

  if (!pMe || !pMe->m_pEditIn)
    return;

  SETAEERECT(&rc, QCAM_PICTURE_X_POS, pMe->m_nTitleHeight,
             pMe->m_dispSizes.wPhotoWidth, pMe->m_dispSizes.wPhotoHeight);

  if (pMe->m_bEditProcessed && pMe->m_pEditOut)
  {
    pNewBmp = pMe->m_pEditOut;
  }
  else
  {
    pNewBmp = pMe->m_pEditIn; // Save calls this func w/o edit
  }

  // Draw hourglass before drawing full image
  Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

  //Draw image to rc
  retVal = Qalbum_DrawImage(pMe, pNewBmp, rc);

  if (Qalbum_DisplayError(pMe, retVal) == AEE_SUCCESS)
  {
    //Update the title bar. If the image is edited, add undo option.
    if (pMe->eState == QCAM_STATE_ADV_EDIT)
    {
#ifndef FEATURE_IPL_LITE
      Qalbum_DisplayAdvEditMenuName(pMe, 0);
#endif
    }
    else if (pMe->eState == QCAM_STATE_EDIT)
    {
      Qalbum_DisplayEditMenuName(pMe, 0);
    }
    else if (pMe->eState == QCAM_STATE_VIEW)
    {
      Qalbum_DisplayViewMenuName(pMe, 0);
    }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  }
}

/*===========================================================================
FUNCTION Qalbum_DisplayPhoto

  DESCRIPTION
    This function loads the current photo image and call Qalbum_ImageNotify to display.
    It set the m_bImagesLoading flag to disable key actions while loading the image.
    The flag will be reset after load image done or any error happens.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    redraw - redraw flag. If it is TRUE, rebuild the PhotoList.

  RETURN VALUE:
   None
===========================================================================*/
void Qalbum_DisplayPhoto(Qalbum* pMe, boolean redraw)
{
  boolean bLoadStat;

  if (!pMe)
    return;

  MSG_LOW("Qalbum_DisplayPhoto",0,0,0);

  //Force rebuild the PhotoList.
  if (pMe->m_pPhotoList == NULL)
  {
    // First get the current directory
    if (!QCam_GetCurrentItem(pMe->m_pDirList, &pMe->m_pCurrentDir, pMe->m_nCurrentDir))
      return;

    // Update the number of photos in this directory
    pMe->m_nTotalPhotos = QCam_BuildPhotoList(pMe->m_pIFileMgr, pMe->m_pCurrentDir,
                                              &pMe->m_pPhotoList, pMe->m_pLaunchDS->bPhotoSortAscend);
  }

  if (redraw)
  {
    //Clean up the screen
    QCamDisp_InitScreen(pMe->a.m_pIDisplay, pMe->di.cxScreen, pMe->di.cyScreen);

    //Move to start photo
    pMe->m_nFileMgrEnumPhoto = pMe->m_nStartPhoto;
    if (!QCam_GetCurrentItem(pMe->m_pPhotoList, &pMe->m_pCurrentPhoto, pMe->m_nStartPhoto))
      return;
  }
  else if (pMe->m_pCurrentPhoto) //get next image
  {
    //get next photo
    pMe->m_nFileMgrEnumPhoto++;
    pMe->m_pCurrentPhoto = pMe->m_pCurrentPhoto->next;
  }

  if (pMe->m_pCurrentPhoto)
  {
    //start loading current image from the file
    pMe->m_bImagesLoading = TRUE;
    pMe->m_bPNGFile = CAM_ISTYPEFILE(QCAM_PNG_EXTENSION, pMe->m_pCurrentPhoto->fileName) ||
                      CAM_ISTYPEFILE(QCAM_PNG_EXTENSION_CAP, pMe->m_pCurrentPhoto->fileName);

    // Free last IImage instance if not NULL
    CAM_RELEASEIF(pMe->m_pDisplayImage);
    if (pMe->m_bPNGFile)   // PNG image uses ISHELL_LoadImage
    {
      pMe->m_pDisplayImage = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->m_pCurrentPhoto->fileName);
      if (pMe->m_pDisplayImage)
        bLoadStat = TRUE;
      else
        bLoadStat = FALSE;
    }
    // JPEG image doesn't use ISHELL_LoadImage
    else if (CAM_ISTYPEFILE(QCAM_JPEG_EXTENSION, pMe->m_pCurrentPhoto->fileName) ||
             CAM_ISTYPEFILE(QCAM_JPEG_EXTENSION_CAP, pMe->m_pCurrentPhoto->fileName))
    {
      //load thumbnail for viewing
      bLoadStat = Qalbum_LoadJPEG(pMe, TRUE);
    }
    else
    {
      bLoadStat = FALSE; // we don't support other formats
    }

    if (bLoadStat == TRUE)
    {
      IIMAGE_Notify(pMe->m_pDisplayImage, Qalbum_ImageNotify, pMe);
    }
    else
    {
      // Direct error handling in same function, and allow rest of thumbnails to show
      Qalbum_ImageNotify(pMe, pMe->m_pDisplayImage, NULL, EFAILED);
    }
  } /* pCurrentPhoto */
  else
  {
    //No more photo to load, reset the loading flag
    pMe->m_bImagesLoading = FALSE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  }
}

/*===========================================================================
FUNCTION Qalbum_DisplayPhotoDirs

  DESCRIPTION
    This function load and display the directory images in thumbnail mode.

  PARAMETERS:
    pMe - Pointer to QCam struct

  RETURN VALUE:
   None
===========================================================================*/
void Qalbum_DisplayPhotoDirs(Qalbum* pMe, boolean redraw)
{
  AEERect rc;
  IImage *pImage = NULL;
  int16   dir = 0;
  QCam_FileList *pDir;

  if (!pMe)
    return;

  pDir = pMe->m_pDirList;

  //Clean up screen if redraw
  if (pDir == NULL || redraw)
  {
    QCamDisp_InitScreen(pMe->a.m_pIDisplay, pMe->di.cxScreen, pMe->di.cyScreen);
  }

  //Get start directory
  while (pDir != NULL && ++dir < pMe->m_nStartDir)
  {
    pDir = pDir->next;
  }

  //Draw the Dir images and texts in the creen screen
  while (pDir && dir < pMe->m_nStartDir + QCAM_DIRS)
  {
    //Load image
    if(STRNCMP(pDir->fileName,QCAM_MMC_CONSTANT,5) == 0)
    {
      pImage = ISHELL_LoadResImage(pMe->a.m_pIShell,
                                   pMe->m_pResFile, IDB_MMC_DIRECTORY);
    }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
    else if(STRNCMP(pDir->fileName,QCAM_USB_CONSTANT,QCAM_USB_CONST_LEN) == 0)
    {
      pImage = ISHELL_LoadResImage(pMe->a.m_pIShell,
                                   pMe->m_pResFile, IDB_USB_DIRECTORY);
    }
#endif
    else if(STRNCMP(pDir->fileName,QCAM_FTL_CONSTANT,QCAM_FTL_CONST_LEN) == 0)
    {
      pImage = ISHELL_LoadResImage(pMe->a.m_pIShell,
                                   pMe->m_pResFile, IDB_FTL_DIRECTORY);
    }
    else
    {
      pImage = ISHELL_LoadResImage(pMe->a.m_pIShell,
                                   pMe->m_pResFile, IDB_DIRECTORY);
    }

    //Draw the dir image in its position
    rc = QCamDisp_GetRect(pMe->m_dispSizes.wDirWidth,
                          pMe->m_dispSizes.wDirHeight,
                          dir, pMe->m_nStartDir, DIRIMAGE, pMe->m_nTitleHeight);
    if (pImage)
    {
      QCamDisp_DrawDirImage(pMe->a.m_pIDisplay, pImage, rc);
      (void)IIMAGE_Release(pImage);
    }

    //Draw the dir text in its position
    rc = QCamDisp_GetRect(pMe->m_dispSizes.wDirWidth,
                          pMe->m_dispSizes.wDirHeight,
                          dir, pMe->m_nStartDir, DIRTEXT, pMe->m_nTitleHeight);

    if(STRNCMP(pDir->fileName,QCAM_MMC_CONSTANT,5)==0)
    {
      QCamDisp_DrawDirText(pMe->a.m_pIDisplay, &pDir->fileName[QCAM_MMC_DIR_LEN], rc);
    }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS 
    else if(STRNCMP(pDir->fileName, QCAM_USB_CONSTANT, QCAM_USB_CONST_LEN) == 0)
    {
      QCamDisp_DrawDirText(pMe->a.m_pIDisplay, &pDir->fileName[QCAM_USB_DIR_LEN], rc);
    }
#endif
    else if(STRNCMP(pDir->fileName, QCAM_FTL_CONSTANT, QCAM_FTL_CONST_LEN) == 0)
    {
      QCamDisp_DrawDirText(pMe->a.m_pIDisplay, &pDir->fileName[QCAM_FTL_DIR_LEN], rc);
    }
    else
    {
      QCamDisp_DrawDirText(pMe->a.m_pIDisplay, &pDir->fileName[QCAM_PHOTOS_DIR_LEN], rc);
    }

    //Update screen and get next dir
    IDISPLAY_Update(pMe->a.m_pIDisplay);
    pDir = pDir->next;
    dir++;
  }

  //Draw the selected item border
  rc = QCamDisp_GetRect(pMe->m_dispSizes.wDirWidth,
                 pMe->m_dispSizes.wDirHeight,
                 pMe->m_nCurrentDir, pMe->m_nStartDir, SELDIR, pMe->m_nTitleHeight);

  QCamDisp_DrawSelect(pMe->a.m_pIDisplay, &pMe->m_SelectRC, rc);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
}

/*===========================================================================
FUNCTION Qalbum_DisplaySelection

  DESCRIPTION
    This function updates the file name and draw the border for selected thumbnail.

  PARAMETERS:
    pMe - Pointer to QCam struct
    bThumbNail - It is thumbnail image

  RETURN VALUE:
   None
===========================================================================*/
static void Qalbum_DisplaySelection(Qalbum *pMe, boolean bThumbNail)
{
  if (!pMe)
    return;

  if (bThumbNail)
  {
    AEERect rc;
    int16 idx, start;
    if (pMe->m_bDisplayFrames)  //frame image
    {
      idx = pMe->m_nCurrentFrame;
      start = pMe->m_nStartFrame;
      //Update the m_pCurrentFrame pointer based on the selected index
      if (pMe->m_nCurrentFrame > 1)
      {
        if (!QCam_GetCurrentItem(pMe->m_pFrameList, &pMe->m_pCurrentFrame,
                                 (int16)(pMe->m_nCurrentFrame - 1)))
          return;
      }
    }
    else //photo image
    {
      start = pMe->m_nStartPhoto;

      //Update the m_pCurrentPhoto pointer based on the selected index
      if(pMe->m_bIs2ndPhoto)
      {
        idx = pMe->m_n2ndPhoto;
        if (!QCam_GetCurrentItem(pMe->m_pPhotoList, &pMe->m_p2ndPhoto, pMe->m_n2ndPhoto))
          return;
      }
      else
      {
        idx = pMe->m_nCurrentPhoto;
        if (!QCam_GetCurrentItem(pMe->m_pPhotoList,
                                 &pMe->m_pCurrentPhoto,
                                 pMe->m_nCurrentPhoto))
          return;
      }
    }

    //get display rect for selection rectange
    rc = QCamDisp_GetRect(pMe->m_dispSizes.wThumbnailWidth,
                     pMe->m_dispSizes.wThumbnailHeight,
                     idx, start, SELTHUMBNAIL, pMe->m_nTitleHeight);
    QCamDisp_DrawSelect(pMe->a.m_pIDisplay, &pMe->m_SelectRC, rc);
  }
  Qalbum_DisplayImageFileName(pMe);
  IDISPLAY_Update(pMe->a.m_pIDisplay);
}

/*===========================================================================
FUNCTION Qalbum_DisplayFrames

  DESCRIPTION
    This function load and display the frame images in thumbnail mode.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    redraw - flag to update the m_nCurrentFrame and m_nStartFrame

  RETURN VALUE:
   None
===========================================================================*/
void Qalbum_DisplayFrames(Qalbum* pMe, boolean rebuild, boolean redraw)
{
  if (!pMe)
    return;

  pMe->m_bDisplayFrames = TRUE;

  if (rebuild)
  {
    //Selected image size will change the frame directory. rebuild it when flag is set.
    //build the frame list and update the total number of frames
    pMe->m_nTotalFrames = QCam_BuildFrameList(pMe->m_pIFileMgr, pMe->m_sFrameDir, &pMe->m_pFrameList);
    MSG_MED("QCam_BuildFrameList returns %d frames", pMe->m_nTotalFrames, 0, 0);
    if (pMe->m_nTotalFrames == 0)
    {
      Qalbum_DisplayTimedImg(pMe, (PFNNOTIFY)Qalbum_Exit, IDB_NO_FRAME);
      return;
    }

    if (pMe->m_pLaunchDS->sFrameFileName[0] != '\0')
    {
      // Find the current frame number
      pMe->m_nCurrentFrame = QCam_GetCurrentFrameIndex(pMe->m_pFrameList, pMe->m_pLaunchDS->sFrameFileName);
      if (pMe->m_nCurrentFrame == -1)
      {
        //Could not find the file. reset to 1 for No Frame
        pMe->m_nCurrentFrame = 1;
      }
      pMe->m_nStartFrame = (int16)((((pMe->m_nCurrentFrame - 1) / QCAM_THUMBNAILS) * QCAM_THUMBNAILS) + 1);
    }
    else
    {
      //Index for No frame
      pMe->m_nCurrentFrame = 1;
      pMe->m_nStartFrame = 1;
    }
  }

  //When start up or switch to next page, need to redraw.
  if (redraw)
  {
    //Clean up the screen
    QCamDisp_InitScreen(pMe->a.m_pIDisplay, pMe->di.cxScreen, pMe->di.cyScreen);
    //get current frame in next page
    pMe->m_nFileMgrEnumPhoto = pMe->m_nStartFrame;
    //pMe->m_nStartFrame == 2 has the first frame in the list. Need offset to the index.
    if (pMe->m_nStartFrame > 1)
    {
      if (!QCam_GetCurrentItem(pMe->m_pFrameList,
                              &pMe->m_pCurrentFrame,
                              (int16)(pMe->m_nStartFrame - 1)))
      {
        return;
      }
    }
    else
    {
      //start number 1 is for no frame. switch to the fist frame
      pMe->m_nFileMgrEnumPhoto++;
      pMe->m_pCurrentFrame = pMe->m_pFrameList;

      if (pMe->m_nCurrentFrame == 1)
      {
        //The current selection is no frame, need to draw it
        //outside the QCam_ImageNotify.
        Qalbum_DisplaySelection(pMe, TRUE);
      }
    }
  }
  else if (pMe->m_pCurrentFrame) //get next frame
  {
    //get next frame
    pMe->m_nFileMgrEnumPhoto++;
    pMe->m_pCurrentFrame = pMe->m_pCurrentFrame->next;
  }

  if (pMe->m_pCurrentFrame)
  {
    //start loading current image from the file
    if (pMe->m_pFrameImage)
    {
      CAM_RELEASEIF(pMe->m_pFrameImage);
    }
    pMe->m_pFrameImage = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->m_pCurrentFrame->fileName);
    if (pMe->m_pFrameImage)
    {
      IIMAGE_Notify(pMe->m_pFrameImage, Qalbum_ImageNotify, pMe);
    }
    else
    {
      Qalbum_DisplayFrameErr(pMe, IDS_MEM_FULL);
    }
  } /* pCurrentPhoto */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
}

/*===========================================================================
FUNCTION Qalbum_DisplayImageFileName

  DESCRIPTION
    This function draws the file name for the selected image.

  PARAMETERS:
    pMe - pointer to Qalbum data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_DisplayImageFileName(const Qalbum *pMe)
{
  char *fileName = NULL;
  AECHAR  szBuf[TEXT_STRING_SIZE] = {0};
  AEERect textRC;

  if (!pMe)
    return;

  SETAEERECT(&textRC, 0, 0, pMe->di.cxScreen, (pMe->m_nTitleHeight + 1) - QCAM_SEL_OFFSET);

  // Find the selected file name
  if (pMe->m_bDisplayFrames)
  {
    if (pMe->m_nCurrentFrame > 1)
    {
      fileName = pMe->m_pCurrentFrame->fileName;
      (void)STRTOWSTR(&fileName[QCAM_FRAMES_DIR_LEN], szBuf, sizeof(szBuf));
    }
    else
      (void)STRTOWSTR("No Frame", szBuf, sizeof(szBuf));
  }
  else
  {
    if(pMe->m_bIs2ndPhoto)
    {
      fileName = pMe->m_p2ndPhoto->fileName;
    }
    else
    {
      fileName = pMe->m_pCurrentPhoto->fileName;
    }

    if(STRNCMP(pMe->m_pCurrentDir->fileName, QCAM_MMC_CONSTANT, 5) == 0)
    {
      (void)STRTOWSTR(&fileName[QCAM_MMC_DIR_LEN], szBuf, sizeof(szBuf));
    }
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
    else if(STRNCMP(pMe->m_pCurrentDir->fileName, QCAM_USB_CONSTANT, QCAM_USB_CONST_LEN) == 0)
    {
      (void)STRTOWSTR(&fileName[QCAM_USB_DIR_LEN], szBuf, sizeof(szBuf));
    }
#endif
    else if(STRNCMP(pMe->m_pCurrentDir->fileName, QCAM_FTL_CONSTANT, QCAM_FTL_CONST_LEN) == 0)
    {
      (void)STRTOWSTR(&fileName[QCAM_FTL_DIR_LEN], szBuf, sizeof(szBuf));
    }
    else
    {
      (void)STRTOWSTR(&fileName[QCAM_PHOTOS_DIR_LEN], szBuf, sizeof(szBuf));
    }
  }

  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &textRC);
  (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, QCAM_FONT_TITLE, szBuf,
                          -1, 0, 0, &textRC, IDF_ALIGN_CENTER|IDF_TEXT_UNDERLINE);
}

/*===========================================================================
FUNCTION Qalbum_AutoPlay

  DESCRIPTION
    This function increases the m_nCurrentPhoto to display the next photo.
    It also checks if the index out of the range and reset it.

  PARAMETERS:
    pMe - Pointer to QCam struct

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_AutoPlay(Qalbum* pMe)
{
  if (!pMe)
    return;

  if (!pMe->m_bAutoPlayInit)
  {
    MSG_HIGH("Qalbum_AutoPlay - Next picture", 0, 0, 0);
    pMe->m_nCurrentPhoto++;
    if (pMe->m_nCurrentPhoto > pMe->m_nTotalPhotos)
    {
       //pMe->nCurrentPhoto = 1;
       pMe->m_nCurrentDir++;
       // free the current photo list
       QCam_DeleteFileList(&pMe->m_pPhotoList);
       if (pMe->m_nCurrentDir > pMe->m_nTotalDirs)
       {
          pMe->m_nCurrentDir = 1;
       }
       pMe->m_nCurrentPhoto = 1;
       pMe->m_bAutoPlayInit = TRUE;
    }
  }
  Qalbum_DisplayPhoto(pMe, pMe->m_bAutoPlayInit);
  pMe->m_bAutoPlayInit = FALSE;
}

/*===========================================================================
FUNCTION Qalbum_ViewPhoto

  DESCRIPTION
    This function wraps the function calls to update the full screen display
    for a newly load image
    It is called on select, on save or on delete.

  PARAMETERS:
    pMe     - pointer to Qalbum struct
    nItemID - View menu item id
    bReload - flag to tell if redraw or reload the image.

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_ViewPhoto(Qalbum *pMe, uint16 nItemID, boolean bReload)
{
  if (!pMe)
    return;

  MSG_MED("Start Qalbum_ViewPhoto", 0, 0, 0);
  //Clean up screen
  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

  //Reset data
  pMe->m_nEditProcessedVal = 0;
  Qalbum_BuildIconMenu(pMe, ViewIDList, ARR_SIZE(ViewIDList));

  if (nItemID != 0)
  {
    IMENUCTL_SetSel(pMe->m_pISoftMenu, nItemID);
    pMe->m_wEditMenuSel = nItemID;
  }

  if (bReload)
  {
    //Update the title bar for the selected menu icon.
    Qalbum_DisplayViewMenuName(pMe, 0);

    //Reload the image
    Qalbum_LoadImage(pMe);
  }
  else
  {
    //Redraw the image
    Qalbum_UpdateEditedImage(pMe);
  }
}

/*===========================================================================
FUNCTION Qalbum_LoadImage

  DESCRIPTION
    This function loads current image for full screen display

  PARAMETERS:
    pMe - Pointer to QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void Qalbum_LoadImage(Qalbum *pMe)
{
  boolean status;
  boolean bLoadStat;

  if (!pMe)
    return;

  MSG_LOW("Qalbum_LoadImage", 0, 0, 0);

  if (pMe->m_bImagesLoading)
  {
    MSG_HIGH("Cannot load image when in loading", 0, 0, 0);
    return;
  }
  //Get the current image from the photo list
  if (pMe->m_bIs2ndPhoto)
  {
    status = QCam_GetCurrentItem(pMe->m_pPhotoList, &pMe->m_p2ndPhoto, pMe->m_n2ndPhoto);
  }
  else
  {
    status = QCam_GetCurrentItem(pMe->m_pPhotoList, &pMe->m_pCurrentPhoto, pMe->m_nCurrentPhoto);
  }

  if (status)
  {
    if (pMe->m_bIs2ndPhoto)
    {
      (void)STRLCPY(pMe->m_sCurrentFileName, pMe->m_p2ndPhoto->fileName, FILE_NAME_SIZE);
    }
    else
    {
      (void)STRLCPY(pMe->m_sCurrentFileName, pMe->m_pCurrentPhoto->fileName, FILE_NAME_SIZE);
    }

    // draw hourglass before loading full image
    Qalbum_DrawBusyImage(pMe, pMe->di.cxScreen / 2, pMe->di.cyScreen / 2);

    // Set loading flag
    pMe->m_bImagesLoading = TRUE;
    pMe->m_bPNGFile = CAM_ISTYPEFILE(QCAM_PNG_EXTENSION, pMe->m_sCurrentFileName) ||
                      CAM_ISTYPEFILE(QCAM_PNG_EXTENSION_CAP, pMe->m_sCurrentFileName);

    MSG_MED( "Start to load full size image.", 0, 0, 0);
    // Free last IImage instance if not NULL
    CAM_RELEASEIF(pMe->m_pDisplayImage);
    if (pMe->m_bPNGFile)   // PNG image uses ISHELL_LoadImage
    {
      pMe->m_pDisplayImage = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->m_sCurrentFileName);
      if (pMe->m_pDisplayImage)
        bLoadStat = TRUE;
      else
        bLoadStat = FALSE;
    }
    // JPEG image doesn't use ISHELL_LoadImage
    else if (CAM_ISTYPEFILE(QCAM_JPEG_EXTENSION, pMe->m_sCurrentFileName) ||
             CAM_ISTYPEFILE(QCAM_JPEG_EXTENSION_CAP, pMe->m_sCurrentFileName))
    {
      //load full image for editing
      bLoadStat = Qalbum_LoadJPEG(pMe, FALSE);
    }
    else
      bLoadStat = FALSE; // We don't support other formats

    if (bLoadStat == TRUE)
    {
      IIMAGE_Notify(pMe->m_pDisplayImage, Qalbum_LoadImageNotify, pMe);
    }
    else
    {
      // Direct error handling in same function
      MSG_HIGH("Failed to load image.", 0, 0, 0);
      Qalbum_LoadImageNotify(pMe, pMe->m_pDisplayImage, NULL, EFAILED);
    }
  }
  else
  {
    // Direct error handling in same function
    MSG_HIGH("Failed to get image item.", 0, 0, 0);
    Qalbum_LoadImageNotify(pMe, pMe->m_pDisplayImage, NULL, EFAILED);
  }
}

/*=====================================================================
FUNCTION Qalbum_LoadJPEG

  DESCRIPTION: Helper function to load jpeg image based on bThumb flag

  PARAMETERS:
    pMe:    pointer to Qalbum struct
    bThumb: flage to tell loading thumbnail or main image

  RETURN VALUE:
    boolean TRUE if loading success. FALSE otherwise
======================================================================*/
static boolean Qalbum_LoadJPEG(Qalbum *pMe, boolean bThumb)
{
  boolean bLoadStat = FALSE;

  if (!pMe)
    return FALSE;

  MSG_LOW("Qalbum_LoadJPEG", 0, 0, 0);

  //load thumbnail for viewing
  switch (pMe->m_JpegColorFormat)
  {
  case QALBUM_COLOR_FORMAT_YCBCR420LP:
    bLoadStat = bThumb ?
                Qalbum_LoadJpegImage(pMe, pMe->m_pCurrentPhoto->fileName,
                                     AEECLSID_JPEG_YCBCR420LP_THUMB) :
                Qalbum_LoadJpegImage(pMe, pMe->m_sCurrentFileName,
                                     AEECLSID_JPEG_YCBCR420LP_MAIN);
    break;
  case QALBUM_COLOR_FORMAT_YCBCR422LP:
    bLoadStat = bThumb ?
                Qalbum_LoadJpegImage(pMe, pMe->m_pCurrentPhoto->fileName,
                                     AEECLSID_JPEG_YCBCR422LP_THUMB) :
                Qalbum_LoadJpegImage(pMe, pMe->m_sCurrentFileName,
                                     AEECLSID_JPEG_YCBCR422LP_MAIN);

    break;
#if (defined(AEECLSID_JPEG_YCRCB420LP_THUMB) && defined(AEECLSID_JPEG_YCRCB422LP_MAIN))
  case QALBUM_COLOR_FORMAT_YCRCB420LP:
    bLoadStat = bThumb ?
                Qalbum_LoadJpegImage(pMe, pMe->m_pCurrentPhoto->fileName,
                                     AEECLSID_JPEG_YCRCB420LP_THUMB) :
                Qalbum_LoadJpegImage(pMe, pMe->m_sCurrentFileName,
                                     AEECLSID_JPEG_YCRCB420LP_MAIN);
    break;
  case QALBUM_COLOR_FORMAT_YCRCB422LP:
    bLoadStat = bThumb ?
                Qalbum_LoadJpegImage(pMe, pMe->m_pCurrentPhoto->fileName,
                                     AEECLSID_JPEG_YCRCB422LP_THUMB) :
                Qalbum_LoadJpegImage(pMe, pMe->m_sCurrentFileName,
                                     AEECLSID_JPEG_YCRCB422LP_MAIN);

    break;
#endif //#if (defined(AEECLSID_JPEG_YCRCB420LP_THUMB) && defined(AEECLSID_JPEG_YCRCB422LP_MAIN))
  case QALBUM_COLOR_FORMAT_RGB565:
  default:
    bLoadStat = bThumb ?
                Qalbum_LoadJpegImage(pMe, pMe->m_pCurrentPhoto->fileName,
                                     AEECLSID_JPEG_RGB_THUMB) :
                Qalbum_LoadJpegImage(pMe, pMe->m_sCurrentFileName,
                                     AEECLSID_JPEG_RGB_MAIN);
    break;
  }

  return bLoadStat;
}

/*===========================================================================
FUNCTION Qalbum_LoadJpegImage

  DESCRIPTION
    This function implements ISHELL_LoadImage with more options.

  PARAMETERS:
    pMe      - pointer to Qalbum struct
    pszFile  - string containing filename

  RETURN VALUE:
    pointer to IImage
===========================================================================*/
static boolean Qalbum_LoadJpegImage(Qalbum *pMe, const char *pszFile, AEECLSID cls)
{
  IFile * pIFile;
#ifdef FEATURE_JPEGD_RESIZE
  jpegdResizeInfo resizeInfo = {0};
  OEMImageParmExt parm = {0};
#endif //FEATURE_JPEGD_RESIZE

  if (!pMe)
    return FALSE;

  if(!pMe->m_bDecodeInProgress)
  {
    if(ISHELL_CreateInstance(pMe->a.m_pIShell, cls,
                           (void **)&pMe->m_pDisplayImage) != SUCCESS)
    {
      MSG_HIGH("cannot create JPEG instance", 0, 0, 0);
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }

  pIFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, pszFile, _OFM_READ);
  if(pIFile)
  {
#ifdef FEATURE_JPEGD_RESIZE
    if ((pMe->eState == QCAM_STATE_THUMBNAIL) || (pMe->eState == QCAM_STATE_SLIDESHOW))
    {
      if (pMe->eState == QCAM_STATE_THUMBNAIL)
      {
        resizeInfo.resizeImageWidth  = pMe->m_dispSizes.wThumbnailWidth;
        resizeInfo.resizeImageHeight = pMe->m_dispSizes.wThumbnailHeight;
      }
      else
      {
        resizeInfo.resizeImageWidth  = pMe->m_dispSizes.wPhotoWidth;
        resizeInfo.resizeImageHeight = pMe->m_dispSizes.wPhotoHeight;
      }
      resizeInfo.aspectRatioFlag   = pMe->m_bAspectRatio;
      resizeInfo.jpegdResizeFlag   = TRUE;

      parm.pData = &resizeInfo;
      parm.uLen  = sizeof(jpegdResizeInfo);
      if (IIMAGE_ExtFunc(pMe->m_pDisplayImage, JPEGD_RESIZE,
                         &parm, sizeof(OEMImageParmExt)) != SUCCESS)
      {
        MSG_HIGH("JPEGD_RESIZE failed", 0, 0, 0);
        return FALSE;
      }
    }
#endif //FEATURE_JPEGD_RESIZE

    MSG_HIGH("m_pDisplayImage = %u", pMe->m_pDisplayImage, 0, 0);

    IIMAGE_SetStream(pMe->m_pDisplayImage, (IAStream *)pIFile);
    (void)IFILE_Release(pIFile);
    pMe->m_bDecodeInProgress = TRUE;
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION Qalbum_DeleteFileResponse

  DESCRIPTION
    This function implement delete selection.

  PARAMETERS:
    pMe - Pointer to QCam struct
    id  - Selection id (Yes or No to delete the file)

  RETURN VALUE:
    None
===========================================================================*/
static void Qalbum_DeleteFileResponse(Qalbum *pMe, uint16 id)
{
  if (!pMe)
    return;

  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
  if (id == IDS_YES)
  {
    if (pMe->m_nTotalPhotos)
    {
      pMe->m_nTotalPhotos--;
      //delete the file and remove the directory if it is empty
      (void)QCam_DeleteFile(pMe->m_pIFileMgr, pMe->m_sCurrentFileName,
                            (boolean)(pMe->m_nTotalPhotos == 0));
      //Update the file list. If there is no more file in the directory, pMe->m_pPhotoList
      //is updated to NULL so it ready for rebuilding the photo list from another directory.
      QCam_DeleteItemFromList(&pMe->m_pPhotoList, pMe->m_nCurrentPhoto);

      //If it is last photo in the directory, need to rebuild the dirlist
      if (pMe->m_nTotalPhotos == 0)
      {
        pMe->m_nTotalDirs = QCam_BuildDirList(pMe->m_pIFileMgr,
                                              &pMe->m_pDirList,
                                              pMe->m_pLaunchDS->bDirSortAscend,
                                              FALSE,
                                              LOC_ALL);
        if (pMe->m_nTotalDirs)
        {
          //reinitialize the start number
          pMe->m_nCurrentDir = 1;
          pMe->m_nStartDir = 1;
          pMe->m_nCurrentPhoto = 1;
          pMe->m_nStartPhoto = 1;
        }
      }
      else
      {
        //If it is not the first item, move to previous item
        if (pMe->m_nCurrentPhoto > 1)
          pMe->m_nCurrentPhoto--;
        //if no photo in this page, move to previous page.
        if (pMe->m_nCurrentPhoto < pMe->m_nStartPhoto)
          pMe->m_nStartPhoto -= QCAM_THUMBNAILS;
      }

      //return to thumbnail mode after delete the current file.
      (void)Qalbum_ViewKeyHandler(pMe, EVT_KEY, (uint16)AVK_CLR);
    }
  }
  else
  {
    // Redraw the edit dialog
    Qalbum_ViewPhoto(pMe, IDS_DEL, FALSE);
  }
}

/*********************************************************************************************/
/*****                                                                                   *****/
/*****                  Notifiers/Callbacks for Qalbum Application                   *****/
/*****                                                                                   *****/
/*********************************************************************************************/
/*===========================================================================
FUNCTION Qalbum_ImageNotify

  DESCRIPTION
    This function is called back from the frame, thumbnail or slideshow
    image loading function.
    It will process the image after it is loaded or handle the error.

  PARAMETERS:
    pUser - Pointer to user data, map to Qalbum struct
    pImage - Pointer to the loaded image data
    pi     - Pointer to the loaded image info
    nErr   - error code for loading status

  RETURN VALUE:
    None
===========================================================================*/
static void Qalbum_ImageNotify(void * pUser, IImage * pImage, AEEImageInfo * pi, int nErr)
{
  AEERect    imageRC;
  Qalbum    *pMe      = (Qalbum *)pUser;
  uint16     dx, dy;
  IBitmap   *pBitmap = NULL;
  int        retVal = AEE_SUCCESS;
  int16      start;
  int        current;
  ImageType  eType;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  if (!pMe)
    return;

  pMe->m_bDecodeInProgress = FALSE;

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  MSG_HIGH("Qalbum_ImageNotify with err code %d", nErr, 0, 0);
  //Do this no matter success or not.
  if (pMe->eState == QCAM_STATE_SAVE)
  {
    if (Qalbum_BuildDialog(pMe, IDD_SAVE_PIC, IDM_SAVE_PIC_MENU, IDC_SAVE))
    {
      MSG_MED("Qalbum_UpdateEditedImage on save dialog", 0, 0, 0);
      Qalbum_UpdateEditedImage(pMe);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    else
    {
      MSG_HIGH("Failed to create save dialog.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    return;
  }
  else if (pMe->eState == QCAM_STATE_VIEW)
  {
    if (pMe->m_pIStatic)
    {
      Qalbum_DisplayProperties(pMe);
      return;
    }
    switch (pMe->m_wEditProcessedBy)
    {
    case IDS_EXTRACT_THUMBNAIL:
      Qalbum_ExtractThumbnail(pMe);
      break;
    case IDS_DESCRIBE:
      if (Qalbum_BuildDialog(pMe, IDD_DESCRIBE, 0, 0))
      {
        (void)ISHELL_GetActiveDialog(pMe->a.m_pIShell);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
      }
      break;
    default:
      break;
    }
    return;
  }

  // If load failed, show a default bad image
  if (nErr != SUCCESS)
  {
    if (pMe->m_bDisplayFrames)
    {
      //If it failed to load frame, no more memory to process the overlay.
      //Stop here.
      Qalbum_DisplayFrameErr(pMe, IDS_MEM_FULL);
      return;
    }
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, IDB_BAD_PHOTO);
  }
  else
  {
    //Get pBitmap
    if (pImage)
    {
      if(pMe->m_bPNGFile || pMe->m_bDisplayFrames)
      {
        pBitmap = (IBitmap *)QCam_GetPNGImageData(pMe->a.m_pIDisplay, pImage);
      }
      else // image types are checked before loading, so it must be jpeg if not png
      {
        IIMAGE_SetParm(pImage, IPARM_GETBITMAP, (int)&pBitmap, (int)&retVal);
      }
      // Just in case OEMJPEG fails to create image buffer
      if (!pBitmap)
      {
        MSG_HIGH("pBitmap is NULL from pImage", 0, 0, 0);
        if (pMe->m_bDisplayFrames)
        {
          //If it failed to get frame bitmap, no more memory to process the overlay.
          //Stop here.
          Qalbum_DisplayFrameErr(pMe, IDS_MEM_FULL);
          return;
        }
        pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, IDB_BAD_PHOTO);
      }
    }
  }

  //Get Rect
  if (pMe->m_bDisplayFrames) //frame image
  {
    start   = pMe->m_nStartFrame;
    current = pMe->m_nCurrentFrame;
  }
  else //photo image
  {
    start   = pMe->m_nStartPhoto;
    if (pMe->m_bIs2ndPhoto)
    {
      current = pMe->m_n2ndPhoto;
    }
    else
    {
      current = pMe->m_nCurrentPhoto;
    }
  }

  if(pMe->eState == QCAM_STATE_SLIDESHOW) //full screen image
  {
    dx = pMe->di.cxScreen;
    dy = (pMe->di.cyScreen - pMe->m_nTitleHeight) - pMe->m_softMenuHeight;
    eType = PHOTO;
  }
  else  //thumbnail image (frame and photo)
  {
    dx = pMe->m_dispSizes.wThumbnailWidth;
    dy = pMe->m_dispSizes.wThumbnailHeight;
    eType = THUMBNAIL;
  }

  //Draw image in rect and update selection
  if(pImage)
  {
    imageRC = QCamDisp_GetRect(dx, dy, pMe->m_nFileMgrEnumPhoto, start, eType,
		                       pMe->m_nTitleHeight);

    if (pBitmap)
    {
      retVal = Qalbum_DrawImage(pMe, pBitmap, imageRC);

      if (retVal != AEE_SUCCESS)
      {
        //We display error message and exit if it is frame mode because
        //it is memory issue in most cases and blocks the overlay processing.
        //We display a broken image for the general thumbnail failure because
        //it is bad image in most cases and it won't block the other decoding.
        if (pMe->m_bDisplayFrames)
        {
          //If it failed to display image, no more memory to process the overlay.
          //Release interfaces and stop here.
          MSG_HIGH("Draw image failed", 0, 0, 0);
          Qalbum_DisplayFrameErr(pMe, IDS_MEM_FULL);
          CAM_RELEASEIF(pBitmap);
          CAM_RELEASEIF(pMe->m_pFrameImage);
          return;
        }
        else
        {
          // Draw the bad image for thumbnail
          pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, IDB_BAD_PHOTO);
          if (pImage)
            QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, imageRC);
        }
      }
    }
    else
    {
      // Draw the bad image
      QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, imageRC);
    }
  }

  if ((pMe->eState == QCAM_STATE_SLIDESHOW || pMe->m_nFileMgrEnumPhoto == current) && pImage)
  {
    Qalbum_DisplaySelection(pMe, (uint8)(pMe->eState != QCAM_STATE_SLIDESHOW));
  }

  if (pMe->m_bPNGFile || pMe->m_bDisplayFrames) // Frames are always PNG for now
  {
    // Need to release DIB for PNG, but not for JPEG
    CAM_RELEASEIF(pBitmap);
  }
  if (pMe->m_bDisplayFrames)
  {
    CAM_RELEASEIF(pMe->m_pFrameImage);
  }
  else
  {
    CAM_RELEASEIF(pMe->m_pDisplayImage);
  }

  if(pMe->m_wKeyPressed == AVK_END && pMe->m_bStopLoadingThumbnails && pMe->eState != QCAM_STATE_SLIDESHOW)
  {
    pMe->m_bImagesLoading = FALSE;
    pMe->m_bStopLoadingThumbnails = FALSE;
    pMe->m_wKeyPressed = 0;
    (void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_QALBUM, EVT_USER_CLOSEAPP, 0, 0 );
    return;
  }
  //Continue the loop
  if (pMe->eState != QCAM_STATE_SLIDESHOW)
  {
    if ((pMe->m_nFileMgrEnumPhoto - start) < (QCAM_THUMBNAILS - 1))
    {
      //continue to load the next thumbnail in the same screen
      if (pMe->m_bStopLoadingThumbnails)
      {
        //Stop loading the thumbnail and execute the key event in queue
        pMe->m_bImagesLoading = FALSE;
        pMe->m_bStopLoadingThumbnails = FALSE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
           (void)ISHELL_PostEvent(pMe->a.m_pIShell, 0, EVT_KEY, pMe->m_wKeyPressed, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      }
      else
      {
        if (pMe->m_bDisplayFrames)
        {
          Qalbum_DisplayFrames(pMe, FALSE, FALSE); //next frame image
        }
        else
        {
          Qalbum_DisplayPhoto(pMe, FALSE); //next photo image
        }
      }
    }
    else //all thumbnail images in the screen are loaded
    {
      //reset the flag to enable keys
      pMe->m_bImagesLoading = FALSE;
      /* Thumbnail loading is completed at this point. We need to inform success
       to the caller for view_type == QCAMERA_DIAG_THUMBNAIL_VIEW */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
    }
  }
  else //set timer to post next photo for slide show
  {
    MSG_HIGH("Slideshow timer speed: %d", pMe->m_pLaunchDS->nSlideshowSpeed, 0, 0);
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, pMe->m_pLaunchDS->nSlideshowSpeed,
                          (PFNNOTIFY)Qalbum_AutoPlay,(void *)pMe);
    pMe->m_bIsSlideShowStarted = TRUE;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  }
  //lint -save -e715 -e818 symbol pi not referenced, function prototype is fixed
}
//lint -restore

/*===========================================================================
FUNCTION Qalbum_LoadImageNotify

  DESCRIPTION
    This function is called back from the image loading function when
    the image is selected for view mode(view in full screen).
    It will set the m_pEditIn image, if it is not already set.
    Then draw the image in full screen will.

  PARAMETERS:
    pUser - Pointer to user data, map to Qalbum struct
    pImage - Pointer to the loaded image data
    pi     - Pointer to the loaded image info
    nErr   - error code for loading status

  RETURN VALUE:
    None
===========================================================================*/
static void Qalbum_LoadImageNotify(void *pUser, IImage *pImage, AEEImageInfo *pi,
                                   int nErr)
{
  Qalbum  *pMe = (Qalbum *)pUser;
  IBitmap *pBitmap = NULL;
  int      retVal;

  if (!pMe)
    return;

  // Clear loading flag
  pMe->m_bImagesLoading = FALSE;
  pMe->m_bDecodeInProgress = FALSE;

  // display error if image loading failed
  MSG_HIGH("Qalbum_LoadImageNotify with err code %d", nErr, 0, 0);
  if (nErr != SUCCESS)
  {
    Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_Exit, IDS_LOAD_FAIL);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

    return;
  }

#ifdef HEAP_CHECK
  MSG_HEAP("After Jpeg decoding:", 0, 0, 0);
  Qalbum_ShowHeapUsage(pMe);
#endif

  //Get IBitmap pointer from the image
  if (pImage)
  {
    //If it is png file
    if(pMe->m_bPNGFile)
    {
      pBitmap = (IBitmap *)QCam_GetPNGImageData(pMe->a.m_pIDisplay, pImage);
    }
    else // image types are checked before loading, so it must be jpeg if not png
    {
      IIMAGE_SetParm(pImage, IPARM_GETBITMAP, (int)&pBitmap, (int)&retVal);
    }
  }

  //If IBitmap, update the display with the image.
  if (pBitmap)
  {
    (void)Qalbum_EditImage(pMe, pBitmap);
  }
  else
  {
    // DIB failed, release the IImage object.
    MSG_HIGH("No memory to get image data.", 0, 0, 0);
    Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_Exit, IDS_LOAD_FAIL);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  }
  //release the image after get required info
  if(pMe->m_bStopLoadingThumbnails == TRUE && pMe->m_wKeyPressed == (uint16)AVK_END)
    (void)ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_QALBUM, EVT_USER_CLOSEAPP, 0, 0 );
  else
    CAM_RELEASEIF(pMe->m_pDisplayImage);
  //lint -save -e715 -e818 symbol pi not referenced, function prototype is fixed
}
//lint -restore

/*=====================================================================
FUNCTION Qalbum_DrawImage

  DESCRIPTION: This function draw image by IDisplay to DISPLAY1

  PARAMETERS:
    pMe      - Pointer to user data, map to Qalbum struct
    pBitmap  - Pointer to the image data in RGB565 or YCbCr format
    rc       - Dsiplay rect

  RETURN VALUE:
    error code defined in AEEError.h
======================================================================*/
//lint -save -e818 Pointer parameter 'pMe' could be declared as pointing to const
static int Qalbum_DrawImage(Qalbum *pMe, IBitmap *pBitmap, AEERect rc)
{
  IDIB    *pDIB   = NULL;
  IYCbCr  *pYCbCr = NULL;
  int     retVal = EUNSUPPORTED;

  if (!pMe || !pBitmap)
    return EBADPARM;

  // Draw the image in rc by IDISPLAY1
  if (IBITMAP_QueryInterface(pBitmap, AEECLSID_DIB, (void** )&pDIB) == SUCCESS)
  {
    retVal = QCamDisp_DrawImage(pMe->a.m_pIDisplay, pMe->m_pIIpl,
                                pDIB, &rc, 0, 0, TRUE);
  }
  else if (IBITMAP_QueryInterface(pBitmap, AEEIID_YCBCR, (void** )&pYCbCr) == SUCCESS)
  {
    retVal = QCamDisp_DrawYCbCrImage(pMe->a.m_pIDisplay,  pMe->m_pIYCbCrFactory,
                                     pMe->m_pIIpl, pYCbCr, &rc, 0, 0, TRUE);
  }

  CAM_RELEASEIF(pDIB);
  CAM_RELEASEIF(pYCbCr);

#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
  if (pMe->m_pLaunchDS->bMultiDisplayMode &&
      !pMe->m_bDisplayFrames &&
      (pMe->eState != QCAM_STATE_THUMBNAIL))
  {
    //Display full image in DISPLAY3 if it is in multiple displays mode
    QCamDisp_DisplayOvOnTV(pMe->a.m_pIShell, &pMe->m_pAlbum, pBitmap);
  }
#endif

  return retVal;
}
//lint -restore

/*===========================================================================
FUNCTION Qalbum_EditImage

  DESCRIPTION
    This function is called by LoadImageNotify to process an RGB565 or YCbCr image.

  PARAMETERS:
    pMe     - Pointer to user data, map to Qalbum struct
    pBitmap - Pointer to the image data in RGB565 or YCbCr format

  RETURN VALUE:
    None
===========================================================================*/
static boolean Qalbum_EditImage(Qalbum *pMe, IBitmap *pBitmap)
{
  boolean status = TRUE;
  int     retVal = AEE_SUCCESS;
  AEERect rc;

  if (!pMe || !pBitmap)
  {
    return FALSE;
  }

  if (!pMe->m_bIs2ndPhoto)
  {
    pMe->m_bEditProcessed = FALSE;
    // Set up display area
    SETAEERECT(&rc, QCAM_PICTURE_X_POS, pMe->m_nTitleHeight,
               pMe->m_dispSizes.wPhotoWidth, pMe->m_dispSizes.wPhotoHeight);
    // Copy pBitmap into pEditIn, If we don't alreay have m_pEditIn
    if (!pMe->m_pEditIn)
    {
      if (Qalbum_IBitmapCopy(pBitmap, &pMe->m_pEditIn) == FALSE)
      {
        Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_Exit, IDS_MEM_FULL);
        retVal = ENOMEMORY;
        status = FALSE;
        goto Done;
      }
    }

#ifdef HEAP_CHECK
    MSG_HEAP("Edit Buffers Allocated:", 0, 0, 0);
    Qalbum_ShowHeapUsage(pMe);
#endif

    //Draw image to rc
    retVal = Qalbum_DrawImage(pMe, pMe->m_pEditIn, rc);

    if (retVal != AEE_SUCCESS)
    {
      //Need to exit edit mode in any drawing failure after display message.
      if (retVal == ENOMEMORY)
        Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_Exit, IDS_MEM_FULL);
      else
        Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_Exit, IDS_UNSUPPORTED_FORMAT);

      status = FALSE;
      goto Done;
    }
  }
#ifndef FEATURE_IPL_LITE
  else
  {
    AEEBitmapInfo   info1, info2;

    pMe->m_bIs2ndPhoto = FALSE;
    pMe->m_nStartPhoto = pMe->m_nCurrentPhoto;

    // IPL doesn't support editing on 2 images that have different dimensions
    if ((IBITMAP_GetInfo(pMe->m_pEditIn, &info1, sizeof(AEEBitmapInfo)) == SUCCESS) &&
        (IBITMAP_GetInfo(pBitmap, &info2, sizeof(AEEBitmapInfo)) == SUCCESS))
    {
      if ((info1.cx != info2.cx) || (info1.cy != info2.cy))
      {
        //Failed in this Adv edit but still able to execute other.
        //It could still stay in Adv Edit mode with Qalbum_EditReturn.
        pMe->eState = QCAM_STATE_ADV_EDIT;
        retVal = EBADPARM;
        Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_BAD_PHOTO_SIZE);
        status = FALSE;
        goto Done;
      }
    }
    else
    {
      retVal = EFAILED;
      status = FALSE;
      goto Done;
    }

    if (Qalbum_IBitmapCopy(pBitmap, &pMe->m_pEditIn2) == FALSE )
    {
      Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_MEM_FULL);
      retVal = ENOMEMORY;
      status = FALSE;
      goto Done;
    }

#ifdef HEAP_CHECK
    MSG_HEAP("2nd Edit Buffer Allocated:", 0, 0, 0);
    Qalbum_ShowHeapUsage(pMe);
#endif

    // Back to advanced edit
    pMe->eState = QCAM_STATE_ADV_EDIT;
    status = Qalbum_AdvEditKeyHandler(pMe, EVT_KEY, pMe->m_wKeyEdit);
    if (!status)
      retVal = EFAILED;
  }
#endif

Done:
  // Do not release pBitmap for JPEG because OEMJPEG_GetImageData doesn't malloc
  if (pMe->m_bPNGFile)
  {
    CAM_RELEASEIF(pBitmap);
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  return status;
}

/*===========================================================================
FUNCTION Qalbum_DisplayTimedImg

  DESCRIPTION
    This function displays the image and set the timer to end it.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    ph - call back funciton after time out
    msgID - message ID for the message to display

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_DisplayTimedImg(Qalbum* pMe, PFNNOTIFY ph, uint16 imgID)
{
  AEERect rc;
  IImage *pImage = NULL;

  if (!pMe)
    return;

  //Display message
  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen);
  pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, imgID);
  QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, rc);
  //set timer if we get return function
  if (ph)
  {
    pMe->m_bMsgTimerOn = TRUE;
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, QALBUM_TIMEOUT_2000, ph, (void *)pMe);
  }
}

/*===========================================================================
FUNCTION Qalbum_DisplayTimedMsg

  DESCRIPTION
    This function displays the message and set the timer to end it.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    ph - call back funciton after time out
    msgID - message ID for the message to display

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_DisplayTimedMsg(Qalbum* pMe, PFNNOTIFY ph, uint16 msgID)
{
  AEERect rc;
  AECHAR  szBuf[TEXT_STRING_SIZE] = {0};

  if (!pMe)
    return;

  // Clear screen
  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen);
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);

  // Display message
  SETAEERECT(&rc, QCAM_TXT_X_START, 0, pMe->di.cxScreen - QCAM_TXT_X_START, pMe->di.cyScreen);
  (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, msgID, szBuf, sizeof(szBuf));
  QCamDisp_ShowMsg(pMe->a.m_pIDisplay, szBuf, rc);

  //set timer if we get return function
  if (ph)
  {
    pMe->m_bMsgTimerOn = TRUE;
    (void)ISHELL_SetTimer(pMe->a.m_pIShell, QALBUM_TIMEOUT_2000, ph, (void *)pMe);
  }
}

/*===========================================================================
FUNCTION Qalbum_ThumbnailSelectNext

  DESCRIPTION
    This function handles navigate key actions when images displayed in thumbnail mode.

  PARAMETERS:
    pMe - Pointer to Qalbum struct
    key - key id (Up/Down/Left/Right)

  RETURN VALUE:
    None
===========================================================================*/
static void Qalbum_ThumbnailSelectNext(Qalbum *pMe, uint16 key)
{
  boolean updateThumbnails = FALSE;
  int16 *current;
  int16 total;
  int16 *start;
  int16 nScreenImages; //thumbnails in one screen
  int16 nLineImages;   //thumbnails in one line

  if (!pMe)
    return;

  //Set rigth variables for different images (frame, dir, photo)
  if (pMe->m_bDisplayFrames) //frame images
  {
    current = &pMe->m_nCurrentFrame;
    total   =  pMe->m_nTotalFrames + 1; //include "No Frame"
    start   = &pMe->m_nStartFrame;
    nScreenImages = QCAM_THUMBNAILS;
    nLineImages = QCAM_THUMBNAILS_X;
  }
  else if (pMe->eState == QCAM_STATE_DIR) //dir images
  {
    current = &pMe->m_nCurrentDir;
    total   =  pMe->m_nTotalDirs;
    start   = &pMe->m_nStartDir;
    nScreenImages = QCAM_DIRS;
    nLineImages = QCAM_DIRS_X;
    // free the current photo list
    QCam_DeleteFileList(&pMe->m_pPhotoList);
  }
  else //photo images
  {
    if (pMe->m_bIs2ndPhoto)
    {
      current = &pMe->m_n2ndPhoto;
    }
    else
    {
      current = &pMe->m_nCurrentPhoto;
    }
    total   =  pMe->m_nTotalPhotos;
    start   = &pMe->m_nStartPhoto;
    nScreenImages = QCAM_THUMBNAILS;
    nLineImages = QCAM_THUMBNAILS_X;
  }

  //Navigate through the images
  switch (key)
  {
  case AVK_UP:
    if ((*current) == 1)
    {
      //current is first one, go to last on the same col
      (*current) = (total % 2) ? total : total - 1;
      (*start) = (int16)((((total - 1) / nScreenImages) * nScreenImages) + 1);

      //Redraw if it switches to other screen
      if (total > nScreenImages)
        updateThumbnails = TRUE;
    }
    else if ((*current) <= nLineImages)
    {
      //current in first line, go to first one
      (*current) = 1;
    }
    else //current else where, go up one line
    {
      (*current) -= nLineImages;
    }
    break;
  case AVK_LEFT:
    if ((*current) <= 1)
    {
      //current is first one, go to last one
      (*current) = total;
      (*start) = (int16)((((total - 1) / nScreenImages) * nScreenImages) + 1);

      //Redraw if it switches to other screen
      if (total > nScreenImages)
        updateThumbnails = TRUE;
    }
    else //current else where, go back one item
    {
      (*current)--;
    }
    break;
  case AVK_DOWN:
    if ((*current) == total)
    {
      //current is last one, go to first one in the same col
      (*current) = (total % nLineImages) ? (int16)(total % nLineImages) : nLineImages;
      (*start) = 1;

      //Redraw if it switches to other screen
      if (total > nScreenImages)
        updateThumbnails = TRUE;
    }
    else if ((*current) > total - nLineImages)
    {
      //current in last line, go to last one
      (*current) = total;
    }
    else //current else where, go down one line
    {
      (*current) += nLineImages;
    }
    break;
  case AVK_RIGHT:
    if ((*current) + 1 > total)
    {
      MSG_MED("Last file in directory.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
      //current is last one, go to first one
      (*current) = 1;
      (*start) = 1;

      //Redraw if it switches to other screen
      if (total > nScreenImages)
        updateThumbnails = TRUE;
    }
    else //current else where, go to next one
    {
      (*current)++;
    }
    break;
  default:
    break;
  }

  //Check if need to update the screen start number
  if ((*current) < (*start))
  {
    //current switched to last screen
    (*start) -= nScreenImages;
    updateThumbnails = TRUE;
  }
  else if ((*current) >= (*start) + nScreenImages)
  {
    //current switched to next screen
    (*start) += nScreenImages;
    updateThumbnails = TRUE;
  }
  else
  {
    //current is in the same screen, only need to redraw the border.
    if (pMe->eState == QCAM_STATE_DIR)
    {
      AEERect rc = QCamDisp_GetRect(pMe->m_dispSizes.wDirWidth,
                                    pMe->m_dispSizes.wDirHeight,
                                    *current, *start, SELDIR, pMe->m_nTitleHeight);

      QCamDisp_DrawSelect(pMe->a.m_pIDisplay, &pMe->m_SelectRC, rc);
    }
    else
      Qalbum_DisplaySelection(pMe, TRUE);
  }

  //Move to a new screen, need to redraw all images.
  if (updateThumbnails)
  {
    if (pMe->m_bDisplayFrames)
      Qalbum_DisplayFrames(pMe, FALSE, TRUE);
    else if (pMe->eState == QCAM_STATE_DIR)
      Qalbum_DisplayPhotoDirs(pMe, TRUE);
    else
      Qalbum_DisplayPhoto(pMe, TRUE);
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
}

/*===========================================================================
FUNCTION Qalbum_IsMaxValue

  DESCRIPTION
    This function checks if the value a maxmum value.

  PARAMETERS:
    val     - value to check
    itemID  - item ID to determine the maxmum value

  RETURN VALUE:
    TRUE if it is the maxmum value. FALSE otherwise.
===========================================================================*/
boolean Qalbum_IsMaxValue(int32 val, uint16 itemID)
{
  boolean bIsMax = FALSE;
  switch (itemID)
  {
  case IDS_BRIGHTNESS:
    bIsMax = (val >= QCAM_BRIGHTNESS_MAX);
    break;
  case IDS_IMAGE_HUE:
    bIsMax = (val >= QCAM_HUE_MAX);
    break;
  case IDS_IMAGE_SATURATION:
    bIsMax = (val >= QCAM_SATURATION_MAX);
    break;
  case IDS_IMAGE_SHARPNESS:
    bIsMax = (val >= 1);
    break;
  case IDS_IMAGE_SOLARIZE:
    bIsMax = (val >= QCAM_SOLARIZE_MAX);
    break;
  case IDS_IMAGE_POSTERIZE:
    bIsMax = (val >= QCAM_POSTERIZE_MAX);
    break;
#ifndef FEATURE_IPL_LITE
  case IDS_FOG:
    bIsMax = (val >= QCAM_FOG_MAX);
    break;
  case IDS_GAUSSIAN_BLUR:
  case IDS_UNIFORM_BLUR:
  case IDS_MOTION_BLUR:
    bIsMax = (val >= QCAM_BLUR_MAX);
    break;
  case IDS_ARB_ROTATION:
    bIsMax = (val >= QCAM_ARB_ROTATE_MAX);
    break;
  case IDS_ALPHA_BLEND:
    bIsMax = (val >= QCAM_BLEND_MAX);
    break;
  case IDS_WHITEBOARD:
    bIsMax = (val >= QCAM_THRESHOLD_MAX);
    break;
#endif //FEATURE_IPL_LITE
  default:
    break;
  }
  return bIsMax;
}

/*===========================================================================
FUNCTION Qalbum_IsMinValue

  DESCRIPTION
    This function checks if the value a minimum value.

  PARAMETERS:
    val     - value to check
    itemID  - item ID to determine the minimum value

  RETURN VALUE:
    TRUE if it is the minimum value. FALSE otherwise.
===========================================================================*/
boolean Qalbum_IsMinValue(int32 val, uint16 itemID)
{
  boolean bIsMin = FALSE;
  switch (itemID)
  {
  case IDS_BRIGHTNESS:
    bIsMin = (val <= QCAM_BRIGHTNESS_MIN);
    break;
  case IDS_IMAGE_HUE:
    bIsMin = (val <= QCAM_HUE_MIN);
    break;
  case IDS_IMAGE_SATURATION:
    bIsMin = (val <= QCAM_SATURATION_MIN);
    break;
  case IDS_IMAGE_SHARPNESS:
    bIsMin = (val <= -1);
    break;
  case IDS_IMAGE_SOLARIZE:
    bIsMin = (val <= QCAM_SOLARIZE_MIN);
    break;
  case IDS_IMAGE_POSTERIZE:
    bIsMin = (val <= QCAM_POSTERIZE_MIN);
    break;
#ifndef FEATURE_IPL_LITE
  case IDS_FOG:
    bIsMin = (val <= QCAM_FOG_MIN);
    break;
  case IDS_GAUSSIAN_BLUR:
  case IDS_UNIFORM_BLUR:
  case IDS_MOTION_BLUR:
    bIsMin = (val <= QCAM_BLUR_MIN);
    break;
  case IDS_ARB_ROTATION:
    bIsMin = (val <= QCAM_ARB_ROTATE_MIN);
    break;
  case IDS_ALPHA_BLEND:
    bIsMin = (val <= QCAM_BLEND_MIN);
    break;
  case IDS_HORIZONTAL_COMP:
  case IDS_VERTICAL_COMP:
  case IDS_CENTER_COMP:
    bIsMin = (val <= 0);
    break;
  case IDS_WHITEBOARD:
    bIsMin = (val <= QCAM_THRESHOLD_MIN);
    break;
#endif //FEATURE_IPL_LITE
  default:
    break;
  }
  return bIsMin;
}

/*===========================================================================
FUNCTION Qalbum_DisplayListNavArrows

  DESCRIPTION
    This function displays the edit list navigation arrows.

  PARAMETERS:
    pMe - Pointer to Qalbum struct

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_DisplayListNavArrows(const Qalbum *pMe)
{
  AEERect rc = {0};
  IImage *pImage = NULL;

  if (!pMe)
    return;

  //Draw navigation arrows when not editing the image
  if (!pMe->m_bInEdit)
  {
    //Draw <- arrow
    SETAEERECT(&rc, 0, 0, QCAM_NAV_ARROW_WIDTH, pMe->m_nTitleHeight);
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, IDB_NAV_LEFT);
    QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, rc);

    //Draw -> arrow
    SETAEERECT(&rc, pMe->di.cxScreen - QCAM_NAV_ARROW_WIDTH, 0,
               QCAM_NAV_ARROW_WIDTH, pMe->m_nTitleHeight);
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, IDB_NAV_RIGHT);
    QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, rc);
  }
}

/*===========================================================================
FUNCTION Qalbum_DisplayTitle

  DESCRIPTION
    This function displays the title with provided image and text ids.

  PARAMETERS:
    pMe     - Pointer to  Qalbum struct
    img1ID  - image id for the first icon in the title
    txt1Str - text buffer for the first string in title
    img2ID  - image id for the second icon in the title
    txt2ID  - text id for the second string in the title

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_DisplayTitle(const Qalbum *pMe,
                         uint16 img1ID, uint16 txt1ID,
                         uint16 img2ID, uint16 txt2ID,
                         uint16 img3ID, uint16 txt3ID)
{
  IImage *pImage = NULL;
  AECHAR szBuf[TEXT_STRING_SIZE] = {0};
  AEERect rc;
  int16 image1Pos = QALBUM_FIRST_POS;
  int16 image2Pos = QALBUM_SECOND_POS;
  int16 image3Pos = QALBUM_THIRD_POS;

  if (!pMe)
    return;

#ifdef FEATURE_CAMERA7500
    /* Check the Display is VGA and double the postions */
  if(pMe->di.cxScreen >= QCAM_VGA_WIDTH)
  {
     image2Pos = 2 * QALBUM_SECOND_POS;
     image3Pos = 2 * QALBUM_THIRD_POS;
  }
#endif

  if (pMe->m_bInEdit)
  {
    //Include navigation arrow areas
    SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight);
  }
  else
  {
    //Exclude navigation arrow areas
    SETAEERECT(&rc, pMe->m_nNavArrowWidth, 0, pMe->di.cxScreen - 2 * QCAM_NAV_ARROW_WIDTH,
               pMe->m_nTitleHeight);
  }

  //Clear title area
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);

  //Draw the 1st image/string
  rc.x = image1Pos + pMe->m_nNavArrowWidth;
  if (img1ID)
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, img1ID);
  if (txt1ID)
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, txt1ID, szBuf, sizeof(szBuf));
  if (img1ID || txt1ID)
    QCamDisp_DrawMenuTitle(pMe->a.m_pIDisplay, rc, pImage, szBuf);

  //Draw the 2nd image/string
  rc.x = image2Pos;
  ZEROAT(szBuf);
  pImage = NULL; //released in draw funtion.
  if (img2ID)
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, img2ID);
  if (txt2ID > 0)
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, (uint16)txt2ID, szBuf, sizeof(szBuf));
  if (img2ID || txt2ID)
    QCamDisp_DrawMenuTitle(pMe->a.m_pIDisplay, rc, pImage, szBuf);

  //Draw the 3rd image/string
  rc.x = image3Pos;
  ZEROAT(szBuf);
  pImage = NULL;
  if (img3ID)
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, img3ID);
  if (txt3ID > 0)
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, (uint16)txt3ID, szBuf, sizeof(szBuf));
  if (img3ID || txt3ID)
    QCamDisp_DrawMenuTitle(pMe->a.m_pIDisplay, rc, pImage, szBuf);

  //update screen
  (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
}

/*===========================================================================
FUNCTION Qalbum_DisplayTitle_SM

  DESCRIPTION
    This function displays the title with images. This function is called
    when pMe->m_bSmallScreen value is true.

  PARAMETERS:
    pMe     - Pointer to  QCam struct
    img1ID  - image id for the first icon in the title
    img2ID  - image id for the second icon in the title
    txt2ID  - text id for the string in the title

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_DisplayTitle_SM(const Qalbum *pMe, uint16 img1ID, uint16 img2ID, uint16 txtID)
{
  IImage *pImage = NULL;
  AECHAR szBuf[TEXT_STRING_SIZE] = {0};
  AEERect rc;
  int16 image1Pos = QALBUM_FIRST_POS;
  int16 image2Pos = QALBUM_SECOND_POS_SM;

  if (!pMe)
    return;

  if (pMe->m_bInEdit)
  {
    //Include navigation arrow areas
    SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight);
  }
  else
  {
    //Exclude navigation arrow areas
    SETAEERECT(&rc, QCAM_NAV_ARROW_WIDTH, 0, pMe->di.cxScreen - 2 * QCAM_NAV_ARROW_WIDTH,
               pMe->m_nTitleHeight);
  }

  //Clear title area
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);

  //Draw the 1st image
  rc.x = image1Pos;
  if (img1ID)
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, img1ID);
  else
    pImage = NULL;
  QCamDisp_DrawMenuTitle(pMe->a.m_pIDisplay, rc, pImage, NULL);

  //Draw the 2nd image/string
  rc.x = image2Pos;
  if (img2ID)
    pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, img2ID);
  else
    pImage = NULL;
  if (txtID > 0)
    (void)ISHELL_LoadResString(pMe->a.m_pIShell, pMe->m_pResFile, (uint16)txtID, szBuf, sizeof(szBuf));
  QCamDisp_DrawMenuTitle(pMe->a.m_pIDisplay, rc, pImage, szBuf);

  //update screen
  (void)IMENUCTL_Redraw(pMe->m_pISoftMenu);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
}

/*===========================================================================
FUNCTION Qalbum_DisplayViewMenuName

  DESCRIPTION
    This function displays the selected menu item name of the view menu.

  PARAMETERS:
    pMe  - Pointer to Qalbum struct
    item - move direction on current item (-1, 0, 1).

  RETURN VALUE:
    None
===========================================================================*/
static void Qalbum_DisplayViewMenuName(Qalbum *pMe, int item)
{
  uint16 selection;
  int    index;
  int    count;

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
  case IDS_SAVE:
  case IDS_REVERT:
  case IDS_DEL:
  case IDS_IMAGE_COPY:
#ifdef FEATURE_APP_MMS
#error code not present
#endif //#ifdef FEATURE_APP_MMS
  case IDS_PROPERTIES:
  case IDS_DESCRIBE:
  case IDS_EXTRACT_THUMBNAIL:
  case IDS_PRINT:
  case IDS_EDIT:
#ifndef FEATURE_IPL_LITE
  case IDS_ADVANCED:
#endif //FEATURE_IPL_LITE
    Qalbum_DisplayTitle(pMe, IDB_SELECT, selection, 0, 0, 0, 0);
    break;

  case IDS_ZOOM:
    if (pMe->m_bZoom == TRUE)
    {
      Qalbum_DisplayTitle(pMe, IDB_SELECT, IDS_IN, IDB_STAR, IDS_OUT, IDB_MOVE, IDS_PAN);
    }
    else
    {
      Qalbum_DisplayTitle(pMe, IDB_SELECT, selection, 0, 0, 0, 0);
    }
    break;

  default:
    break;
  }
}

/*===========================================================================
FUNCTION Qalbum_DisplayPhotoSavedMsg

  DESCRIPTION
    This function displays the message after save to file.

  PARAMETERS:
    pMe  - Pointer to  Qalbum struct

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_DisplayPhotoSavedMsg(Qalbum *pMe)
{
  AEERect rc;
  AECHAR szText[FILE_NAME_SIZE];
  IImage *pImage = NULL;
  const char *strFileName;

  if (!pMe)
    return;

  strFileName = QCam_GetFileShortName(pMe->m_sCurrentFileName);
  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight);
  // Display the name of the image
  (void)STRTOWSTR(strFileName, szText, sizeof(szText));
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
  (void)IDISPLAY_DrawText(pMe->a.m_pIDisplay, QCAM_FONT_TITLE, szText,
                          -1, 0, 0, &rc, IDF_ALIGN_CENTER);

  SETAEERECT(&rc, 0, pMe->m_nTitleHeight, pMe->di.cxScreen,
             pMe->di.cyScreen - pMe->m_nTitleHeight);
  pImage = ISHELL_LoadResImage(pMe->a.m_pIShell, pMe->m_pResFile, IDB_PHOTO_SAVED);

  QCamDisp_DrawIcon(pMe->a.m_pIDisplay, pImage, rc);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  // Update the number of photos in this directory
  pMe->m_nTotalPhotos = QCam_BuildPhotoList(pMe->m_pIFileMgr,
                                            pMe->m_pCurrentDir,
                                            &pMe->m_pPhotoList,
                                            pMe->m_pLaunchDS->bPhotoSortAscend);

  //Set timer to return to preview mode
  pMe->m_bMsgTimerOn = TRUE;
  (void)ISHELL_SetTimer(pMe->a.m_pIShell, QALBUM_TIMEOUT_2000,
                        (PFNNOTIFY)Qalbum_EditPhotoSaved, (void *)pMe);
}

/* ==========================================================================
FUNCTION Qalbum_GETIMAGESIZE
DESCRIPTION
   Returns the size of the image:
     QCAM_SIZE_UXGA   1200 x 1600
     QCAM_SIZE_SXGA   1024 x 1280
     QCAM_SIZE_4VGA   960  x 1280
     QCAM_SIZE_XGA    768  x 1024
     QCAM_SIZE_SVGA   600  x 800
     QCAM_SIZE_VGA    480  x 640
     QCAM_SIZE_QVGA   240  x 320
     QCAM_SIZE_QCIF   144  x 176
     QCAM_SIZE_QQVGA  120  x 160
     QCAM_SIZE_STAMP  80   x 48
============================================================================= */
int Qalbum_GetImageSize(int width, int height)
{
   int w = width;
   int i;
   AEESize *pSize;

   // look up by shorter dimension
   if (width > height)
   {
     w = height;
   }

   for (i = 0; i < (int)QCAM_SIZE_COUNT; i++)
   {
     pSize = QCam_GetImageDimension(i);
     if (w == pSize->cx)
     {

     // width of some wide resolutions same as other resolutions.Inthat case need to
     // compare height also.
#ifdef FEATURE_CAMERA_WIDESIZE
#error code not present
#endif // FEATURE_CAMERA_WIDESIZE
       return i;
     }
   }

   // 2nd try - UXGA size is sometimes smaller than 1200x1600 due to VFE limitation
   pSize = QCam_GetImageDimension((int)QCAM_SIZE_UXGA);
   if ((width < pSize->cx) && (width >= pSize->cx - 32))
     return (int)QCAM_SIZE_UXGA;

   // 3rd try - find the next smaller size
   for (i = 1; i < (int)QCAM_SIZE_COUNT; i++)
   {
     pSize = QCam_GetImageDimension(i);
     if (w > pSize->cx)
     {
       return i;
     }
   }

   return (int)QCAM_SIZE_COUNT;
} /* END QCam_GetImageSize */

/*===========================================================================
FUNCTION Qalbum_EditPhotoSaved

  DESCRIPTION
    Wrap function for seting timer to return

  PARAMETERS:
    pMe    - Pointer to  Qalbum struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_EditPhotoSaved(Qalbum *pMe)
{
  if (!pMe)
    return;

  pMe->m_bMsgTimerOn = FALSE;
  // Add a state check here to be safe because this function is called by the timer,
  // a key press could potentially cause a state change during this timeout period
  if (pMe->eState == QCAM_STATE_SAVE)
  {
    MSG_MED("Back to VIEW state from SAVE state", 0, 0, 0);
    pMe->eState = QCAM_STATE_VIEW;
    Qalbum_ViewPhoto(pMe, IDS_SAVE, TRUE);
  }
  else
  {
    //return to view mode after saving thumbnail image.
    Qalbum_ViewPhoto(pMe, IDS_EXTRACT_THUMBNAIL, TRUE);
  }
}

/*===========================================================================
FUNCTION Qalbum_ExtractThumbnail

  DESCRIPTION
    This function extracts the thumbnail from the main image.

  PARAMETERS:
    pMe - Pointer to  Qalbum struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_ExtractThumbnail(Qalbum *pMe)
{
  OEMImageParmExt parm;
  IFile          *fp;

  if (!pMe)
    return;

  if (!pMe->m_bPNGFile && pMe->m_pDisplayImage)
  {
    CAM_FREEIF(pMe->m_pThumbnail); // Release thumbnail data buffer if not NULL
    parm.pData = &pMe->m_pThumbnail;
    if (IIMAGE_ExtFunc( pMe->m_pDisplayImage, AEECLSID_EXTRACT_THUMBNAIL,
                        &parm, sizeof(OEMImageParmExt)) != SUCCESS)
    {
      pMe->m_uThumbSize = 0;
    }
    else
    {
      pMe->m_uThumbSize = parm.uLen;
    }
  }

  if( pMe->m_bPNGFile || !pMe->m_pThumbnail || !pMe->m_uThumbSize )
  {
    Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_EXTRACT_FAIL);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    return;
  }

  // Generate new file name
  Qalbum_GenerateThumbnailFileName(pMe->m_sCurrentFileName);

  // Save thumbnail into new file
  fp = IFILEMGR_OpenFile(pMe->m_pIFileMgr, pMe->m_sCurrentFileName, _OFM_READWRITE);
  if (!fp)
  {
    fp = IFILEMGR_OpenFile(pMe->m_pIFileMgr, pMe->m_sCurrentFileName, _OFM_CREATE);
  }
  if (fp)
  {
    if (IFILE_Write(fp, pMe->m_pThumbnail, pMe->m_uThumbSize) != pMe->m_uThumbSize)
    {
      MSG_HIGH("Writing of thumbnail failed", 0, 0, 0);
      Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_SAVE_FAIL);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    }
    else
    {
      // to update the screen
      Qalbum_DisplayPhotoSavedMsg(pMe);
    }

    // Release the IFile pointer
    CAM_RELEASEIF(fp);
  }
  else
  {
    MSG_HIGH("Failed to create file to extract thumbnail", 0, 0, 0);
    Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_SAVE_FAIL);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
  }

  CAM_FREEIF(pMe->m_pThumbnail);
}

/*===========================================================================
FUNCTION Qalbum_IsSpaceAvailable

  DESCRIPTION
    This function checks if enough space to save a new file.
    The edited file size is the same or smaller than the origional file.
    So compare available space with the file size.

  PARAMETERS:
    pMe - Pointer to  Qalbum struct

  RETURN VALUE:
    TRUE if space available, FALSE otherwise

===========================================================================*/
static boolean Qalbum_IsSpaceAvailable(Qalbum *pMe)
{
  FileInfo fi;
  uint32 dwFreeSpace = 0;
  boolean  result = TRUE;
  const char *savePath;

  if (!pMe)
    return FALSE;

  //Get size of the current file
  if (IFILEMGR_GetInfo(pMe->m_pIFileMgr, pMe->m_sCurrentFileName, &fi) != SUCCESS)
  {
    MSG_HIGH("Failed in IFILEMGR_GetInfo", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, (uint16)IDS_SAVE_FAIL);
    result = FALSE;
  }
  else if (STRLEN(fi.szName) >= (AEE_MAX_FILE_NAME - 1))
  {
    //Brew Support only 64 Charecters for File name.
    // According to AEEFileInfo Structure.
    MSG_HIGH("NOT ENOUGH SPACE TO WRITE A FILE NAME", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
    Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_INVALID_NAME);
    result = FALSE;
  }
  else
  {
    if (pMe->m_pLaunchDS->nSaveLoc == QCAM_SAVE_MMC)
      savePath = AEEFS_CARD0_DIR;
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
    else if (pMe->m_pLaunchDS->nSaveLoc == QCAM_SAVE_USB)
      savePath = AEEFS_MOD_USB_DIR;
#endif
    else if (pMe->m_pLaunchDS->nSaveLoc == QCAM_SAVE_FTL)
      savePath = AEEFS_FTL_DIR;
    else
      savePath = AEEFS_ROOT_DIR;

    //Compare free space with the file size
    //Display memory full message if not enough space to save a new file.
    if(SUCCESS != IFILEMGR_GetFreeSpaceEx(pMe->m_pIFileMgr,
                                          savePath,
                                          NULL,
                                          &dwFreeSpace))
    {
      dwFreeSpace = IFILEMGR_GetFreeSpace(pMe->m_pIFileMgr, NULL);
    }

    if (dwFreeSpace < fi.dwSize)
    {
      MSG_HIGH("NOT ENOUGH DISK SPACE", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
      Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, (uint16)IDS_MEM_FULL);
      result = FALSE;
    }
  }

  return result;
}

/*===========================================================================
FUNCTION Qalbum_BuildEditList

  DESCRIPTION
    This helper function builds the soft menu for Qalbum application.

  PARAMETERS:
    pMe    - pointer to Qalbum data struct
    idList - pointer to the menu ID list
    size   - size of the ID list array

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_BuildEditList(Qalbum *pMe, const uint16 *idList, int size)
{
  CtlAddItem ai;
  AEERect qrc;
  int16 i;

  // Make sure the pointers we'll be using are valid
  if (!pMe || !pMe->a.m_pIShell || !idList)
    return;

  IMENUCTL_Reset(pMe->m_pISoftMenu);

  // Clear the Rectangle for the Menu
  SETAEERECT (&qrc, 0, pMe->di.cyScreen - pMe->m_softMenuHeight,
              pMe->di.cxScreen, pMe->m_softMenuHeight);
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);

  SETAEERECT (&qrc, 0, pMe->di.cyScreen,
              pMe->di.cxScreen, pMe->m_softMenuHeight);
  IMENUCTL_SetRect(pMe->m_pISoftMenu, &qrc);

  // Add Items in the Menu
  QCam_InitMenuData(&ai, pMe->m_pResFile);

  // Make sure image resource is not loaded for the menu entries
  ai.pText = (AECHAR *)"";
  ai.pszResImage = NULL;

  // Add edit entries to the Menu
  for (i = 0; i < size; ++i)
  {
    // Add menu items
    QCam_AddMenuItem(pMe->m_pISoftMenu, &ai, idList[i], 0);
  }

  pMe->m_wEditMenuSel = IMENUCTL_GetSel(pMe->m_pISoftMenu);
  IMENUCTL_SetActive(pMe->m_pISoftMenu, TRUE);
}

/*===========================================================================
FUNCTION Qalbum_BuildIconMenu

  DESCRIPTION
    This helper function builds the soft menu for Qalbum application.

  PARAMETERS:
    pMe    - pointer to Qalbum data struct
    idList - pointer to the icon menu ID list
    size   - size of the ID list array

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_BuildIconMenu(Qalbum *pMe, const CAMICONMENUID *idList, int size)
{
  CtlAddItem ai;
  int16 i;
  AEERect qrc;

  // Make sure the pointers we'll be using are valid
  if (!pMe || !pMe->a.m_pIShell || !idList)
    return;

  IMENUCTL_Reset(pMe->m_pISoftMenu);
  //Set the Rectangle for the Menu
  SETAEERECT (&qrc, 0, pMe->di.cyScreen - pMe->m_softMenuHeight,
              pMe->di.cxScreen, pMe->m_softMenuHeight);
  IMENUCTL_SetRect(pMe->m_pISoftMenu, &qrc);
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &qrc);

  // Add Items in the Menu
  QCam_InitMenuData(&ai, pMe->m_pResFile);

  //Add edit entries to the Menu
  for (i = 0; i < size; ++i)
  {
    //Add menu items
    QCam_AddMenuItem(pMe->m_pISoftMenu, &ai, idList[i].menuID, idList[i].iconID);
  }

  pMe->m_wEditMenuSel = IMENUCTL_GetSel(pMe->m_pISoftMenu);
  IMENUCTL_SetActive(pMe->m_pISoftMenu, TRUE);
}

/*=====================================================================
FUNCTION Qalbum_BuildDialog

  DESCRIPTION: The helper function builds a entry dialog and sets focus to a control

  PARAMETERS:
    pMe     - pointer to Qalbum data struct
    dlgID   - ID number of the dialog
    menuID  - ID number of the control menu
    cltID   - ID number of the control for focus

  RETURN VALUE:
    boolean TRUE if dialog is built
======================================================================*/
static boolean Qalbum_BuildDialog(Qalbum *pMe, uint16 dlgID, uint16 menuID, uint16 cltID)
{
  int nRet = SUCCESS;

  if (!pMe)
    return FALSE;

  MSG_MED("Qalbum_BuildDialog %d, menuID %d, cltID %d", dlgID, menuID, cltID);
  nRet = ISHELL_CreateDialog(pMe->a.m_pIShell, pMe->m_pResFile, dlgID, NULL);
  if ((nRet == SUCCESS) && (menuID > 0))
  {
    //Work around a BREW issue from 3.1.3 to bring up the menu button
    //on dialog start up.
    IDialog *dlg = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    IMenuCtl* pCtl = (IMenuCtl*)IDIALOG_GetControl(dlg, menuID);
    IMENUCTL_SetSel(pCtl, cltID);
    (void)IMENUCTL_Redraw(pCtl);
  }
  return (nRet == SUCCESS);
}

#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
/*===========================================================================
FUNCTION Qalbum_BuildPrintOptionMenu

  DESCRIPTION
    This helper function builds the menu for selecting how the user wants to print.

  PARAMETERS:
    pMe    - pointer to Qalbum data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static void Qalbum_BuildPrintOptionMenu(Qalbum *pMe)
{
  CtlAddItem ai = {0};
  AEERect    rc = {0};
  uint16 wMenuItemId = 0;



  if (!pMe)
    return;

  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen);
  // Add Items in the Menu

  QCam_InitMenu(pMe->m_pIMenu, pMe->a.m_pIDisplay, pMe->m_pResFile, &ai, IDS_SELECT_PRINT, rc);
  pMe->m_eActiveMenu = PRINT_MENU;


#if defined(FEATURE_BT_EXTPF_BPP) && !defined(FEATURE_SKIP_BPP)
  QCam_AddMenuItemEx(pMe->m_pIMenu, &ai,
                     IDS_BT_PRINT_BPP,
                     wMenuItemId++,
                     IDB_RADIO_ON,
                     (uint32)BPP_BT_PRINT);
  pMe->m_ePrintMenuActive = BPP_BT_PRINT;
#endif //defined(FEATURE_BT_EXTPF_BPP) && !defined(FEATURE_SKIP_BPP)

#ifdef FEATURE_BT_EXTPF_BIP
  {
    // If BPP is not enabled, our radio should be on, otherwise off
    uint16 bipMenuItem = IDB_RADIO_OFF;

    if (0 == wMenuItemId)
    {
      pMe->m_ePrintMenuActive = BIP_BT_PRINT;
      bipMenuItem = IDB_RADIO_ON;
    }
    QCam_AddMenuItemEx(pMe->m_pIMenu, &ai,
                       IDS_BT_PRINT_BIP,
                       wMenuItemId++,
                       bipMenuItem,
                       (uint32)BIP_BT_PRINT);
  }
#endif // FEATURE_BT_EXTPF_BIP)



#if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))
  // Pictbridge should never be the first, so always
  // use the OFF radio
  QCam_AddMenuItemEx(pMe->m_pIMenu, &ai,
                     IDS_PICTBRIDGE_PRINT,
                     wMenuItemId++,
                     IDB_RADIO_OFF,
                     (uint32)PICTBRIDGE_PRINT);
#endif // #if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))


  //Redraw the menu
  IMENUCTL_SetSel(pMe->m_pIMenu, 0);
  (void)IMENUCTL_Redraw(pMe->m_pIMenu);

  IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
} /* Qalbum_BuildPrintOptionMenu */
#endif //#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE)

/*===========================================================================
FUNCTION Qalbum_DrawBusyImage

  DESCRIPTION
    This function draws the busy image which is an hour glass.

  PARAMETERS:
    pMe - Pointer to  Qalbum struct
    x   - x coordinate
    y   - y coordinate

  RETURN VALUE:
    None
===========================================================================*/
void Qalbum_DrawBusyImage(const Qalbum *pMe, int x, int y)
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
FUNCTION Qalbum_GenerateThumbnailFileName

  DESCRIPTION
    This function creates a thumbnail file name based on the provided file name.
    It appends "t" to the original file name.
    i.e. If the provided file name is "f:/~/qcam/img/2004-11-09/img001.jpg",
    then the new file name is: "f:/~/qcam/img/2004-11-09/img001t.jpg"

  PARAMETERS:
    pMe - pointer to Qalbum structure.

  RETURN VALUE:
    NA
===========================================================================*/
static void Qalbum_GenerateThumbnailFileName(char *szCurName)
{
  char szExt[FILE_NAME_SIZE] = {0};
  char *pExt;

  if (!szCurName)
    return;

  // Find insersion point
  pExt = szCurName + STRLEN(szCurName) - QCAM_EXTENSION_LENGTH;
  // Save the extension
  (void)STRLCPY(szExt, pExt, sizeof(szExt));
  // The file name becomes imgxxx_t.xxx
  (void)SNPRINTF(pExt, (size_t)(FILE_NAME_SIZE - STRLEN(szCurName)), "t%s", szExt);
}

/*---------------------------------------------------------------------
FUNCTION: Qalbum_DisplayError

DESCRIPTION: The function facilitates

PARAMETERS:
    pMe         pointer to the Applet member structure
    err         Integer Variable taking the Error id

RETURN:
     AEE_SUCCESS if successful
     EBADPARM if unsuccessful

SIDE EFFECTS:
     NO
-----------------------------------------------------------------------*/
int Qalbum_DisplayError(Qalbum *pMe, int err)
{
  if (!pMe)
  {
    return EBADPARM;
  }

  MSG_HIGH("Qalbum_DisplayError get err code 0x%X", err, 0, 0);
  switch (err)
  {
    case AEE_SUCCESS:
      break;
    case ENOMEMORY:
      Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_MEM_FULL);
      break;
    case EUNSUPPORTED :
      Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_UNSUPPORTED_FORMAT);
      break;
    case EFAILED:
    default:
      if (pMe->eState != QCAM_STATE_SAVE)
        Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_IPL_FAIL);
      else
      {
        pMe->eState = QCAM_STATE_VIEW;
        Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_EditReturn, IDS_SAVE_FAIL);
      }
      break;
  }

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  return err;
}

/*=====================================================================
FUNCTION Qalbum_DisplayFrameErr

  DESCRIPTION: Log and display error message when failed to load frame for overlay.

  PARAMETERS:
    pMe   - pointer to Qalbum structure.
    MsgID - message id

  RETURN VALUE:
    void
======================================================================*/
static void Qalbum_DisplayFrameErr(Qalbum *pMe, uint16  MsgID)
{
  if (!pMe)
    return;

  switch (MsgID)
  {
  case IDS_MEM_FULL:
    MSG_HIGH("No more memory to implement frame overlay", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
    break;
  default:
    MSG_HIGH("Failed to add frame overlay.", 0, 0, 0);
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
  }

  Qalbum_DisplayTimedMsg(pMe, (PFNNOTIFY)Qalbum_Exit, MsgID);
}

/*=====================================================================
FUNCTION Qalbum_ReturnView

  DESCRIPTION: This function returns to view state from edid or adv edit state.
               It only updates variables and rebuild menu. No need to redraw image.

  PARAMETERS:
    pMe     - pointer to Qalbum structure.
    nItemID - IDS of icon option in view menu

  RETURN VALUE:
    void
======================================================================*/
void Qalbum_ReturnView(Qalbum *pMe, uint16 nItemID)
{
  AEERect rc;

  if (!pMe)
    return;

  //Copy pEditOut into pEditIn
  if (pMe->m_pEditOut && pMe->m_bEditProcessed)
    (void)Qalbum_IBitmapCopy(pMe->m_pEditOut, &pMe->m_pEditIn);

  CAM_RELEASEIF(pMe->m_pEditOut);
  pMe->m_bEditProcessed = FALSE;
  pMe->m_nEditProcessedVal = 0;

  //Change state back to View
  pMe->eState = QCAM_STATE_VIEW;
  //Redraw the view menu
  Qalbum_BuildIconMenu(pMe, ViewIDList, ARR_SIZE(ViewIDList));
  if (nItemID != 0)
  {
    IMENUCTL_SetSel(pMe->m_pISoftMenu, nItemID);
    pMe->m_wEditMenuSel = nItemID;
  }
  //Clear old title
  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->m_nTitleHeight);
  IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rc);
  Qalbum_DisplayViewMenuName(pMe, 0);
}

/*===========================================================================
FUNCTION Qalbum_BuildColorFormatMenu

  DESCRIPTION
    This helper function builds the menu for selecting Jpeg Color format.

  PARAMETERS:
    pMe    - pointer to Qalbum data struct

  RETURN VALUE:
    NONE
===========================================================================*/

static void Qalbum_BuildColorFormatMenu(Qalbum * pMe)
{
  CtlAddItem ai = {0};
  AEERect    rc = {0};
  uint32 sel = (uint32)pMe->m_JpegColorFormat;

  if (!pMe)
    return;

  SETAEERECT(&rc, 0, 0, pMe->di.cxScreen, pMe->di.cyScreen);
  // Add Items in the Menu
  QCam_InitMenu(pMe->m_pIMenu, pMe->a.m_pIDisplay, pMe->m_pResFile, &ai, IDS_JPEG_COLOR_FORMAT, rc);

  pMe->m_eActiveMenu = JPEG_COLOR_MENU;

  QCam_BuildRadioListMenu(pMe->m_pIMenu, &ai, JpegColorFormatIDList, JpegColorFormatDataList, sel, IDB_RADIO_ON, IDB_RADIO_OFF);

  IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);

}

//Test automation functions
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

#ifdef HEAP_CHECK
void Qalbum_ShowHeapUsage(Qalbum *pMe)
{
  uint32 dwCurUsage = 0;
  uint32 dwMaxUsage = 0;
  uint32 dwAppUsage = 0;

  if (!pMe)
    return;

  if (pMe->m_pIHeap)
  {
    if (IHEAP_GetModuleMemStats(pMe->m_pIHeap, AEECLSID_QALBUM, &dwMaxUsage, &dwCurUsage)
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
      MSG_HEAP("  qalbum total %9d, overall total  %9d", dwAppUsage + dwMaxUsage,
               dwAppUsage + dwMaxUsage + dwOtherUsage, 0);
    }
  }
}
#endif

#endif //#if defined(FEATURE_APP_QALBUM)

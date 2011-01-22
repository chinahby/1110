/*===========================================================================

FILE: qalbum.h

GENERAL DESCRIPTION
    This is the common header file contains definitions for both qalbum
    and qalbumAdv.

      Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qalbum/main/latest/inc/qalbum.h#3 $ $DateTime: 2009/02/26 04:01:26 $ $Author: kgelli $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 02/26/09  KC     Added a flag to keep track if decode is in progress
 02/19/09  KC     Fixed compiler warnings
 03/25/08  KC     Adding Geo_tagging feature.
 12/15/07  KV     Added new Active menu JPEG_COLOR_MENU and added 
                  QALBUM_COLOR_FORMAT_YCRCB420LP and QALBUM_COLOR_FORMAT_YCRCB422LP.
 07/29/07  KV     Added member variable m_nNavArrowWidth for width of the Nav arrow.
 06/18/07  NC     Fixed bugs in zoom feature.
 06/08/07  NC     Fixed potential compiling issue.
 05/22/07  SK     JPEG Decoder Resizing feature
 04/19/07  SK     UI Support for WVGA Display
 04/09/07  NC     Updated document to correct logic sequences.
 03/23/07  KV     Added m_nTitleHeight in _Qalbum struct for dynamic font size.
 02/21/07  TV     Added ability to print with PBPrint
 01/24/07  ZL     Moved Qalbum_DiagReportErr() from qalbum_i.h.
 12/21/06  ZL     Added definition of QALBUM_PROPERTY_MAX_LEN.
 11/21/06  NC     Replace IOverlay with IMMOverlay for BREW 4.0.
 11/09/06  ZL     Added a new VIEW state before EDIT state.
 11/08/06  ZL     Moved editing feature to qalbumEdit_i.h.
 10/30/06  ZL     Added Qalbum_DisplayListNavArrows().
 10/09/06  NC     Defined new helper function and items.
 08/22/06  NC     Removed m_bFreeLaunchDS.
 08/18/06  RK     Added a new helper function for error checking.
 07/17/06  RK     Fixed the Key toggeling issue in Solarize screen .
 07/14/06  NC     Fixed lint errors and added more F3 messages in OnPrint function.
 05/22/06  NC     Updated QCAM_THRESHOLD_xxx per ipl API change.
 04/05/06  HV     Added m_pIYCbCrFactory as member to Qalbum struct.
 02/23/06  NC     Added helper functions for Zoom and Pan logic update.
 11/08/05  JN     Support for whiteboard effect.
 10/13/05  NC     Revert last change to fix lint error.
 10/12/05  SM     automation support for prompt to save/delete
 09/13/05  JN     Removed m_pEditTemp from Qalbum struct.
 08/17/05  JN     Redesigned Qalbum to decouple it from AMSS.
 07/18/05  JN     Replaced all OEMJPEG with IIMAGE APIs.
 06/20/05  NC     Added advanced edit automation feature.
 05/10/05  JN     Added public function for qalbumDlg.c.
 04/11/05  JN     Added zoom and pan option.
 03/02/05  JN     Support for Jpeg output YCbCr format.
 01/28/05  NC     Documented logic go through for qalbum
 12/21/04  JN     Add advanced edit option into qalbum.
 12/07/04  NC     Added DIAG features for Auto Test.
 11/05/04  JN     Added new member variable m_SelectRC.
 10/19/04  JN     QCamViewer -> QCamAlbum -> Qalbum.
 06/16/04  NC     Initial design of the photo viewer and editor.
 ============================================================================*/
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#ifndef AEE_SIMULATOR
#include "comdef.h"
#endif

#if defined(FEATURE_APP_QALBUM)

#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"           // File interface definitions
#include "AEEMenu.h"          // Menu Services
#include "AEEImage.h"
#include "AEEImageCtl.h"
#include "AEEMediaUtil.h"
#include "AEEEncodeSpec.h"
#include "AEEImageExtFunc.h"
#include "OEMImageParmExt.h"
#include "IYCbCrFactory.h"

#include "IIPL.h"

#include "qalbum.brh"
#include "QcamCommon.h"
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
#include "AEEMMOverlay.h"
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifndef AEE_SIMULATOR
#include "ipl.h"
#else
#include "QCamSim.h"
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

/*============================================================================
DEFINITION FOR IMAGE EDITING
===========================================================================*/
#define QALBUM_APP_VERSION "1.0"

#define QCAM_HUE_FACTOR        8
#define QCAM_SATURATION_FACTOR 8
#define QCAM_POSTERIZE_FACTOR  14
#define QCAM_SOLARIZE_FACTOR   16
#define QCAM_FOG_FACTOR        1
#define QCAM_BLUR_FACTOR       4
#define QCAM_ARB_ROTATE_FACTOR 25
#define QCAM_BLEND_FACTOR      25
#define QCAM_THRESHOLD_FACTOR  1

#define QCAM_HUE_MAX          360
#define QCAM_HUE_MIN          -360
#define QCAM_SATURATION_MAX   255
#define QCAM_SATURATION_MIN   -255
#define QCAM_POSTERIZE_MIN    4
#define QCAM_POSTERIZE_MAX    32
#define QCAM_SOLARIZE_MIN     80
#define QCAM_SOLARIZE_MAX     240
#define QCAM_PERSPECTIVE_MIN  -1
#define QCAM_FOG_MIN          -1
#define QCAM_FOG_MAX          1
#define QCAM_BLUR_MIN         1
#define QCAM_BLUR_MAX         17
#define QCAM_ARB_ROTATE_MAX   350
#define QCAM_ARB_ROTATE_MIN   -350
#define QCAM_BLEND_MIN        0
#define QCAM_BLEND_MAX        100
//Whiteboad option value for granulatiry when removing non-white board items
//0: high, 1: regular, 2: coarse, 3: do not remove non-white board items.
#define QCAM_THRESHOLD_MIN    0
#define QCAM_THRESHOLD_MAX    3

#define QCAM_IMAGE_REFLECT_X_BIT (1)
#define QCAM_IMAGE_REFLECT_Y_BIT (2)

#define QALBUM_EXIF_MAX_LEN     255
#define QALBUM_PROPERTY_MAX_LEN 512

#ifdef FEATURE_BT_EXTPF_BIP
#define BTAPP_BIP_XML_FILENAME_W_PATH "fs:/mod/btapp/bip_xml2.txt"
#endif
#ifndef QALBUM_VGA_RES_FILE
#define QALBUM_VGA_RES_FILE "qalbum_vga.bar"
#endif
/*============================================================================
DEFINITION FOR QALBUM STRUCTURE

===========================================================================*/
typedef enum
{
  QCAM_STATE_NULL,
  QCAM_STATE_DIR,
  QCAM_STATE_THUMBNAIL,
  QCAM_STATE_SLIDESHOW,
  QCAM_STATE_VIEW,
  QCAM_STATE_EDIT,
  QCAM_STATE_MENU,
  QCAM_STATE_ADV_EDIT,
  QCAM_STATE_SAVE      // SAVE state doesn't have its own no handler function
}QalbumState;

typedef enum
{
  QALBUM_COLOR_FORMAT_RGB565,
  QALBUM_COLOR_FORMAT_YCBCR420LP,
  QALBUM_COLOR_FORMAT_YCBCR422LP,
  QALBUM_COLOR_FORMAT_YCRCB420LP,
  QALBUM_COLOR_FORMAT_YCRCB422LP
} QalbumColorFormat;

typedef enum
{
  GPS_LATITUDE_REF              = 0x0001,
  GPS_LATITUDE                  = 0x0002,
  GPS_LONGITUDE_REF             = 0x0003,
  GPS_LONGITUDE                 = 0x0004,
  GPS_ALTITUDE_REF              = 0x0005,
  GPS_ALTITUDE                  = 0x0006,
  GPS_PROCESS_METHOD            = 0x001B,
  IMAGE_DESCRIPTION             = 0x010e,
  EXIF_MAKE                     = 0x010f,
  EXIF_MODEL                    = 0x0110,
  SOFTWARE                      = 0x0131,
  EXIF_VERSION                  = 0x9000,
  EXIF_DATE_TIME_ORIGINAL       = 0x9003,
  EXIF_DATE_TIME_DIGITIZED      = 0x9004,
  FLASH                         = 0x9209,
  EXIF_USER_COMMENT             = 0x9286,
  WHITE_BALANCE                 = 0xA403
} Qalbum_ExifTagId;


#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
typedef enum
{
NO_BT_PRINT_ASSIGNED = 0,
#if defined(FEATURE_BT_EXTPF_BPP) && !defined(FEATURE_SKIP_BPP)
BPP_BT_PRINT,
#endif
#ifdef FEATURE_BT_EXTPF_BIP
BIP_BT_PRINT,
#endif
#if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))
PICTBRIDGE_PRINT,
#endif
MAX_PRINT_NUM
} BtPrintChoice;
#endif //#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)

typedef enum
{
  QCAM_JPEG_COLOR_FORMAT_RGB565 = 0,
  QCAM_JPEG_COLOR_FORMAT_YCBCR420LP,
  QCAM_JPEG_COLOR_FORMAT_YCBCR422LP,
  QCAM_JPEG_COLOR_FORMAT_YCRCB420LP,
  QCAM_JPEG_COLOR_FORMAT_YCRCB422LP
#ifdef AEE_SIMULATOR
  ,
  QCAM_JPEG_COLOR_FORMAT_RGB888
#endif
}JPEG_COLOR_FORMAT;


//Jpeg color format items and data list

static const uint16 JpegColorFormatIDList[] =
{
  6,
  IDS_JPEG_COLOR_FORMAT,
  IDS_JPEG_COLOR_FORMAT_RGB565,
  IDS_JPEG_COLOR_FORMAT_YCBCR420LP,
  IDS_JPEG_COLOR_FORMAT_YCBCR422LP,
  IDS_JPEG_COLOR_FORMAT_YCRCB420LP,
  IDS_JPEG_COLOR_FORMAT_YCRCB422LP,
#ifdef AEE_SIMULATOR 
  IDS_JPEG_COLOR_FORMAT_RGB888
#endif
};

static const uint32 JpegColorFormatDataList[] =
{
  QCAM_JPEG_COLOR_FORMAT_RGB565,
  QCAM_JPEG_COLOR_FORMAT_YCBCR420LP,
  QCAM_JPEG_COLOR_FORMAT_YCBCR422LP,
  QCAM_JPEG_COLOR_FORMAT_YCRCB420LP,
  QCAM_JPEG_COLOR_FORMAT_YCRCB422LP,  
#ifdef AEE_SIMULATOR 
  QCAM_JPEG_COLOR_FORMAT_RGB888
#endif
};


typedef enum
{
  RESIZE_MENU = 0,
  JPEG_COLOR_MENU,	
  PRINT_MENU
} ActiveMenu;

typedef struct _Qalbum Qalbum;  //all variable needed in this application
// Qalbum applet structure
struct _Qalbum
{
  AEEApplet      a;
  AEEDeviceInfo  di;

  IMenuCtl        *m_pISoftMenu;
  IMenuCtl        *m_pIMenu;
  ActiveMenu       m_eActiveMenu;
  IFileMgr        *m_pIFileMgr;
  IMediaUtil      *m_pMediaUtil;
  IImage          *m_pDisplayImage;
  IImage          *m_pFrameImage;
  IDIB            *m_pFrameDIB;
  IStatic         *m_pIStatic;
  IIpl            *m_pIIpl;
  IYCBCRFactory   *m_pIYCbCrFactory;
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
  IMMOverlay      *m_pAlbum;
  AEECLSID         m_coreUIClsId;
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#ifdef HEAP_CHECK
  IHeap           *m_pIHeap;
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

  uint8           *m_pThumbnail;  // pointer to thumbnail data
  uint32           m_uThumbSize;  // thumbnail data size
  //
  QalbumState      eState;
  Qalbum_LaunchDS *m_pLaunchDS;
  QcamDispSizes    m_dispSizes;
  const char      *m_pResFile;
  uint16           m_softMenuHeight;
  int16            m_nFileMgrEnumPhoto;
  //
  QCam_FileList   *m_pPhotoList;
  QCam_FileList   *m_pCurrentPhoto;
  QCam_FileList   *m_p2ndPhoto;
  char             m_sCurrentFileName[FILE_NAME_SIZE];
  int16            m_nTotalPhotos;
  int16            m_nCurrentPhoto;
  int16            m_n2ndPhoto;
  int16            m_nStartPhoto;
  //
  QCam_FileList   *m_pDirList;
  QCam_FileList   *m_pCurrentDir;
  int16            m_nTotalDirs;
  int16            m_nCurrentDir;
  int16            m_nStartDir;
  //
  #ifdef FEATURE_GEOTAGGING
  IPosDet         *m_pIPosDet;
  AEEGPSInfo       m_GPSInfo;
  uint8            m_nGPSAccuracy;
  AEECallback      GPSInfoCB;
  #endif /*FEATURE_GEOTAGGING*/
  //
  QCam_FileList   *m_pFrameList;
  QCam_FileList   *m_pCurrentFrame;
  char             m_sFrameDir[FILE_NAME_SIZE];
  char             m_sFrameFileName[FILE_NAME_SIZE];
  int16            m_nTotalFrames;
  int16            m_nCurrentFrame;
  int16            m_nStartFrame;
  //
  boolean          m_bThumbnailFlag;
  boolean          m_bImagesLoading;
  boolean          m_bDecodeInProgress;
  boolean          m_bStopLoadingThumbnails;
  boolean          m_bDisplayFrames;
  boolean          m_bAutoPlayInit;
  boolean          m_bIsSlideShowStarted;
  boolean          m_bPNGFile;
  boolean          m_bMsgTimerOn;
  boolean          m_bSmallScreen;      // TRUE on SC1x and other smaller displays
  boolean          m_bWVGADisplaySupported;
  boolean          m_bAspectRatio;

  uint16           m_wKeyPressed;
  uint16           m_wKeyEdit;
  uint16           m_wEditMenuSel;
  int              m_nImgSize;

  IBitmap         *m_pEditIn;
  IBitmap         *m_pEditIn2;
  IBitmap         *m_pEditOut;
  IBitmap         *m_pEditThumbnail;

  boolean          m_bEditProcessed;
  uint16           m_wEditProcessedBy;
  int32            m_nEditProcessedVal;
  int32            m_nPreviousVal;  // Store the previous value when toggle the key.
  int32            m_nPreviousVal2;
  int16            m_nTitleHeight;
  int16            m_nNavArrowWidth;

  // variables for new jpeg encoder
  AEEMediaEncodeResult m_EncodeResult;
  AEECallback      m_EncodeCB;

  AEERect          m_SelectRC;          // current selection rectangle
  boolean          m_bPhotoErased;      // photo image area was erased by another screen

  boolean          m_bInEdit;          // restrict nav key use in adv editing
  IIPLCircle       m_iCircle;          // used by magnify
  AEERect          m_iRect;            // used by overlap
  boolean          m_bIs2ndPhoto;      // flag to load 2nd photo

  QalbumColorFormat m_JpegColorFormat;// support jpeg decoder output RGB565, YCbCr42xlp

  boolean          m_bZoom;            // in zoom?
  boolean          m_bMaxZoom;         // reached max zoom capacity?
  int16            m_ratio;            // downsize ratio up scale 100
  IBitmap         *m_pZoomImg;
  AEERect          m_dispRC;           // dst display RC
  int              m_xSrc;             // display upper left corner x, y
  int              m_ySrc;
  int              m_xOffset;          // x, y offset from image center during panning
  int              m_yOffset;
#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
  BtPrintChoice    m_ePrintMenuActive;    // currently selelected menu Item while selecting printing
#endif // defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
};

//Key handler type definition
typedef boolean (*PFNKEYHANDLER) (Qalbum *pMe, AEEEvent eCode, uint16 key);

//Key handler map definition
typedef struct _CKeyHandler
{
  QalbumState   state;
  PFNKEYHANDLER HandlerFunc;    //On select menu icon to build menu function
} CKeyHandler;

// execute menu on sel function definition
typedef boolean (*PFNQALBMENUSEL) (Qalbum *pMe, uint16 key);

// event handler sanity check definition
typedef struct _QalbumMenuKeyHandler
{
  uint16         wMenuID;
  PFNQALBMENUSEL MenuOnKeyFunc;
} QalbumMenuKeyHandler;

// Public functions amoung qalbum, qalbumAdv, qalbumDlg
#ifdef HEAP_CHECK
void Qalbum_ShowHeapUsage(Qalbum *pMe);
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#ifdef FEATURE_GEOTAGGING
void Qalbum_AddGeoTags(Qalbum *pMe);
void Qalbum_ProceedWithGeoTagging(Qalbum *pMe );
#endif/* FEATURE_GEOTAGGING */
boolean Qalbum_MenuKeyHandler(Qalbum *pMe, AEEEvent eCode, uint16 key);
boolean Qalbum_EditKeyHandler(Qalbum * pMe, AEEEvent eCode, uint16 key);
#ifndef FEATURE_IPL_LITE
boolean Qalbum_AdvEditKeyHandler(Qalbum *pMe, AEEEvent eCode, uint16 key);
void    Qalbum_DisplayAdvEditMenuName(Qalbum *pMe, int item);
#endif //FEATURE_IPL_LITE
void    Qalbum_DisplayEditMenuName(Qalbum *pMe, int item);
void    Qalbum_ViewPhoto(Qalbum *pMe, uint16 nItemID, boolean bReload);
void    Qalbum_DisplayListNavArrows(const Qalbum *pMe);
void    Qalbum_DisplayTitle(const Qalbum *pMe,
                            uint16 img1ID, uint16 txt1ID,
                            uint16 img2ID, uint16 txt2ID,
                            uint16 img3ID, uint16 txt3ID);
void    Qalbum_DisplayTitle_SM(const Qalbum *pMe,
                               uint16 img1ID, uint16 img2ID, uint16 txtID);
void    Qalbum_ReturnView(Qalbum *pMe, uint16 nItemID);
void    Qalbum_UpdateInIPLImage(Qalbum *pMe, uint16 selection);
void    Qalbum_UpdateEditedImage(Qalbum *pMe);
void    Qalbum_EditUndo(Qalbum *pMe);
int     Qalbum_GetStartValue(uint16 selection);
boolean Qalbum_IsMaxValue(int32 val, uint16 itemID);
boolean Qalbum_IsMinValue(int32 val, uint16 itemID);
boolean Qalbum_EditStep(Qalbum *pMe, uint16 sel, uint16 key, uint16 step, int32 max, int32 min);
void    Qalbum_AutoPlay(Qalbum* pMe);
void    Qalbum_DisplayPhoto(Qalbum *pMe, boolean redraw);
int     Qalbum_GetImageSize(int width, int height);
void    Qalbum_DisplayFrames(Qalbum *pMe, boolean rebuild, boolean redraw);
int     Qalbum_DisplayError(Qalbum *pMe, int err);
void    Qalbum_DisplayPhotoSavedMsg(Qalbum *pMe);
void    Qalbum_DisplayTimedImg(Qalbum *pMe, PFNNOTIFY ph, uint16 imgID);
void    Qalbum_DisplayTimedMsg(Qalbum *pMe, PFNNOTIFY ph, uint16 msgID);
boolean Qalbum_IBitmapCopy(IBitmap *pSrcBmp, IBitmap **ppDstBmp);
void    Qalbum_DoResize(Qalbum *pMe);
void    Qalbum_EditReturn(Qalbum *pMe);
void    Qalbum_DrawBusyImage(const Qalbum *pMe, int x, int y);
void    Qalbum_AddImageFrame(Qalbum *pMe);
boolean Qalbum_EditEffect(Qalbum *pMe, IIPLEffectType type, boolean bUpdate);
boolean Qalbum_DlgSaveEventHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
boolean Qalbum_DlgDescribeEventHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
void    Qalbum_DisplayPhotoDirs(Qalbum *pMe, boolean redraw);
#ifdef  FEATURE_JPEGD_RESIZE
boolean Qalbum_DlgSlideShowEventHandler(void * pUser, AEEEvent evt, uint16 w, uint32 dw);
#endif // FEATURE_JPEGD_RESIZE

#endif //FEATURE_APP_QALBUM

/*===========================================================================
                      MAIN QALBUM LOGIC GO THROUGH

  1. Qalbum start up and initialize
  2. Select and view a photo image
  3. Edit a picture
  4. Resize an image
  5. Add frame overlay to a photo image
  6. Advanced editing
  7. Save a picture
  8. Qalbum in SLIDESHOW mode
  9. Qalbum in FRAME mode

  **************************************************************************
1. Qalbum start up and initialize
  - On EVT_APP_START of Qalbum_HandleEvent()
    Qalbum_HandleStartApp()
      Qalbum_Init()
      //Check launch mode and enter different state
      //If (VIEWER_MODE_FRAME) - Logic 9
      - pMe->eState = QCAM_STATE_NULL;
      - Qalbum_DisplayFrames() -> load the frame images for user option
      //If (VIEWER_MODE_SLIDESHOW) - Logic 8
      - QCam_BuildDirList()
      - pMe->eState = QCAM_STATE_SLIDESHOW;
      - Qalbum_AutoPlay() -> display all photos one by one in selected speed
      //else (VIEWER_MODE_ALBUM) - Logic 2
      - QCam_BuildDirList()
      - pMe->eState = QCAM_STATE_DIR;
      - Qalbum_DisplayPhotoDirs() -> load photo directories for user option

  **************************************************************************
2. Select and view a photo image
  //There are three main states in this mode:
  //QCAM_STATE_DIR -> display all photo directories for selection
  //QCAM_STATE_THUMBNAIL -> display all images of a directory in thumbnail
  //QCAM_STATE_VIEW -> display a selected image in full view
  //The state transitions are:
  //QCAM_STATE_DIR <-> QCAM_STATE_THUMBNAIL <-> QCAM_STATE_VIEW

  - In Qalbum_DisplayPhotoDirs() //first state
      QCamDisp_InitScreen() //When need to redraw
      Get start directory
      //whit a while loop to draw icons and names for all directories
      - ISHELL_LoadResImage()
      - QCamDisp_GetRect()      //get the image rect
      - QCamDisp_DrawDirImage() //draw the directory image
      - QCamDisp_GetRect()      //get the text rect
      - QCamDisp_DrawDirText()  //draw the directory name under icon
      QCamDisp_GetRect()        //get the selected directory border
      QCamDisp_DrawSelect()     //draw the border on selected dir icon

  //When user hit a key in QCAM_STATE_DIR state, the event is mapped to
  //Qalbum_AlbumKeyHandler()
  - On EVT_KEY in Qalbum_HandleEvent()
    CKeyHandlers[pMe->eState].HandlerFunc() ->
      Qalbum_AlbumKeyHandler()

  - in Qalbum_AlbumKeyHandler()
    //On navigate keys
    Qalbum_ThumbnailSelectNext()
      Draw border on next selected directory

    //On AVK_SELECT
    pMe->eState = QCAM_STATE_THUMBNAIL;
    Qalbum_DisplayPhoto() -> Enter second state
      QCam_GetCurrentItem() //Get user selected directory
      QCam_BuildPhotoList() //Only do it the first time for the selected dir
      QCamDisp_InitScreen() //When need to redraw
      QCam_GetCurrentItem() //Get current photo
      //Set pMe->m_bImagesLoading to true if image available
      ISHELL_LoadImage()    //For loading PNG image
      Qalbum_LoadJpegImage() //For loading JPEG thumbnail image
        ISHELL_CreateInstance() //Create JPEG instance with cls
        IFILEMGR_OpenFile(_OFM_READ)
        //If FEATURE_JPEGD_RESIZE is defined, pass info to decoder
        IIMAGE_ExtFunc(JPEGD_RESIZE) //Only do it for thumbnail mode
        IIMAGE_SetStream() //Load the image by QCT JPEG decoder
        IFILE_Release() //Close file
      IIMAGE_Notify(Qalbum_ImageNotify) -> Decoder callback after loading
      //Call Qalbum_ImageNotify directly to display broken image if loading failed.
      //Reset pMe->m_bImagesLoading to false if no more image.

    //This is the call back function to handle the image drawing and continue
    //thumbnail loading until all images are loaded or processing is interrupted.
  - in Qalbum_ImageNotify()
    ISHELL_LoadResImage(IDB_BAD_PHOTO) //Do this if nErr is not success.
    IIMAGE_SetParm(IPARM_GETBITMAP) or QCam_GetPNGImageData() //base on file type
    ISHELL_LoadResImage(IDB_BAD_PHOTO) //Do this if we could not get Bitmap.
    QCamDisp_GetRect()  //get image rect
    //if image is loaded
    - Qalbum_DrawImage()
        IBITMAP_QueryInterface()  //Check if IDIB or IYCbCr bitmap
        QCamDisp_DrawImage() //If it is IDIB
        QCamDisp_DrawYCbCrImage() //If it is IYCbCr
        //if failed to draw the image, load a broken icon for it.
        ISHELL_LoadResImage(IDB_BAD_PHOTO)
        QCamDisp_DrawIcon()
    //else if it is a bad image, use a broken icon to replace it.
    - QCamDisp_DrawIcon()
    Qalbum_DisplaySelection() //draw the selection border and file name
    //Call the loop the draw the next thumbnail if it is avilable
    Qalbum_DisplayPhoto() 
    //else reset the pMe->m_bImagesLoading to false before go ahead

    //On AVK_CLR
    ISHELL_PostEvent(EVT_USER_CLOSEAPP) -> close Qalbum application

  //When user hit a key in QCAM_STATE_THUMBNAIL state, the event is mapped to
  //Qalbum_ThumbnailKeyHandler()
  - On EVT_KEY in Qalbum_HandleEvent()
    CKeyHandlers[pMe->eState].HandlerFunc() ->
      Qalbum_ThumbnailKeyHandler()

  - In Qalbum_ThumbnailKeyHandler()
    //On navigate keys
    Qalbum_ThumbnailSelectNext() //if thumbnail is not loading, otherwise drop the event

    //On AVK_SELECT
    pMe->eState = QCAM_STATE_VIEW;
    Qalbum_ViewPhoto()
      IDISPLAY_ClearScreen() //Clear the thumbnail screen
      Qalbum_BuildIconMenu() //Load the view menu icon
      Qalbum_DisplayViewMenuName() //Display the menu option
        Qalbum_DisplayTitle()
          QCamDisp_DrawMenuTitle()
      Qalbum_LoadImage() //Reload the image
        QCam_GetCurrentItem()
        Qalbum_DrawBusyImage()
        //Set pMe->m_bImagesLoading to true
        ISHELL_LoadImage() //For loading PNG image
        Qalbum_LoadJpegImage() //For loading JPEG image
        IIMAGE_Notify(Qalbum_LoadImageNotify) //Success to load the image
        //Call Qalbum_LoadImageNotify() directly if fail.

    //This is the call back function to handle the full image drawing
  - In Qalbum_LoadImageNotify()
    Reset the pMe->m_bImagesLoading to false
    If loading error, give error message then return to thumbnail mode.
    IIMAGE_SetParm(IPARM_GETBITMAP) or QCam_GetPNGImageData() //base on file type
    Qalbum_EditImage()
      Qalbum_IBitmapCopy() //Copy bitmap to pMe->m_pEditIn
      Qalbum_DrawImage()

    //On AVK_CLR
    pMe->eState = QCAM_STATE_DIR; -> Return to first state
    Qalbum_DisplayPhotoDirs()

  //When user hit a key in QCAM_STATE_VIEW state, the event is mapped to
  //Qalbum_ViewKeyHandler()
  - On EVT_KEY in Qalbum_HandleEvent()
    CKeyHandlers[pMe->eState].HandlerFunc() ->
      Qalbum_ViewKeyHandler()

  - In Qalbum_ViewKeyHandler()
    //On Left/Right key
    Qalbum_UpdateEditedImage() //if return from property or other page
    //If it is inside the zooming mode
    Qalbum_OnZoom()
    //else navigate to next icon
    Qalbum_DisplayViewMenuName()
    IMENUCTL_HandleEvent() //Update the icon highlight

    //On AVK_SELECT
    Drop key event if pMe->m_bImagesLoading
    else
    QalbumViewHandlers[index].MenuOnKeyFunc -> Map to an On key function
    - Qalbum_OnSave()
        Save the image - Logic 7
    - Qalbum_OnThumbnail()
        Qalbum_IsSpaceAvailable() //Check if space available
        Qalbum_ExtractThumbnail() //Save the thumbnail to a new file name
    - Qalbum_OnRevert()
        Qalbum_UpdateInIPLImage(IDS_REVERT) //Reset the editing buffer
        Qalbum_EditPhotoLoadImage()         //Reload the image
    - Qalbum_OnDel()
        ISHELL_Prompt() //Prompt for delete a file,
                        //User response will send a event to Qalbum

      - On EVT_COMMAND in Qalbum_HandleEvent()
        Qalbum_DeleteFileResponse() ->Handle delete and return to thumbnail
    - Qalbum_OnCopy()
        QCam_CopyToDest()
        Qalbum_EditKeyHandler(AVK_CLR) -> Go back to thumbnail mode
    - Qalbum_OnSend()
        QCam_SendPicture()
    - Qalbum_OnProperty()
        Qalbum_ShowProperties()
    - Qalbum_OnZoom()
        IBITMAP_QueryInterface to get IYCbCr or IDIB
        handler switches on key case
          //On AVK_SELECT
          Qalbum_UpdateInIPLImage(IDS_ZOOM)
          Qalbum_InitZoomIn()
          Qalbum_ZoomIn() //if not in max zoom
          //On AVK_STAR
          Qalbum_ZoomOut() //if zoomed in
          //On AVK_UP, 
          Qalbum_PanUp() //if display not in top
          //On AVK_DOWN
          Qalbum_PanDown() //if display not in bottom
          //On AVK_LEFT
          Qalbum_PanLeft() //if display not in far left
          //On AVK_RIGHT
          Qalbum_PanRight() //if display not in far right
        Qalbum_DisplayZoomImage() //after zoom or pan
    - Qalbum_OnPrint()  //Only support BT and PictBridge print
        // If not select a print type yet:
        pMe->eState = QCAM_STATE_MENU;
        Qalbum_BuildPrintOptionMenu(pMe)
        //else
        Qalbum_DoPrint() //Launch BTApp or PBPrint application to print.

    - Qalbum_OnEdit()
        Qalbum_BuildEditList() //Build a new edit menu
        pMe->eState = QCAM_STATE_EDIT -> switch to edit state
        Qalbum_IBitmapCopy() //copy EditIn to EditOut
        Qalbum_DisplayListNavArrows()
        Qalbum_DisplayEditMenuName() //No icon display
        //Logic 3 for edit a picture

    - Qalbum_OnAdvanced()
        Qalbum_BuildEditList()  //Build a new edit menu for advanced edit
        pMe->eState = QCAM_STATE_ADV_EDIT -> switch to advanced edit state
        Qalbum_IBitmapCopy() //copy EditIn to EditOut
        Qalbum_DisplayListNavArrows()
        Qalbum_DisplayAdvEditMenuName() //No icon display
        //Logic 6 for advanced editing

    //On AVK_CLR:
    pMe->eState = QCAM_STATE_THUMBNAIL;
    IDISPLAY_ClearScreen()
    Qalbum_DisplayPhoto() -> return to thumbnail mode

  **************************************************************************
3. Edit a picture
  //When user hit a key in QCAM_STATE_EDIT state, the event is mapped to
  //Qalbum_EditKeyHandler()
  - On EVT_KEY in Qalbum_HandleEvent()
    CKeyHandlers[pMe->eState].HandlerFunc() ->
      Qalbum_EditKeyHandler()

  - In Qalbum_EditKeyHandler()
    //On AVK_RIGHT or AVK_LEFT:
    Qalbum_UpdateEditedImage() //if back from a menu or property page.
    Qalbum_DisplayEditMenuName()
    IMENUCTL_HandleEvent()  //Highlight next selected menu icon

    //On AVK_SELECT, AVK_DOWN or AVK_UP:
  - QalbumMenuKeyHandlers[index].MenuOnKeyFunc() -> Map to an On key edit function    
    - Qalbum_OnResize()
    - Qalbum_OnFlash()
    - Qalbum_OnFrame()
    - Qalbum_OnGrayScale()
    - Qalbum_OnInvertColors()
    - Qalbum_OnSepia()
        Qalbum_OnUpKey()
          Qalbum_UpdateInIPLImage()
          //On AVK_UP
          - Edit continues ->
            //Add Frame
            Display frames in thumbnail for user option - Logic 5
            //Resize image
            Build a edit size menu for user option - Logic 4
            //Other editing
            Qalbum_EditEffect()
              IIPL_Effect() //call ipl function to edit the effect.
          //On AVK_DOWN
          - Qalbum_EditUndo

    - Qalbum_OnBrightness()
    - Qalbum_OnHue()
    - Qalbum_OnPosterize()
    - Qalbum_OnSolarize()
    - Qalbum_OnSaturation()
        Qalbum_EditStep() //Get the new step to set effect
        Qalbum_EditEffect() //Call IIPL_Effect()
    - Qalbum_OnSharpness()
        Qalbum_EditStep() //Get the new step to set effect
        IIPL_Filter()
    - Qalbum_OnRotate()
        Qalbum_EditPicDoRotate()
    - Qalbum_OnFlip()
        Qalbum_EditPicDoFlip()

    //On AVK_CLR:
    Qalbum_ReturnView()
      Qalbum_IBitmapCopy() //copy EditOut to EditIn
      pMe->eState = QCAM_STATE_VIEW;
      Qalbum_BuildIconMenu() //build menu for view mode
      Qalbum_DisplayViewMenuName()

  **************************************************************************
4. Resize an image
  - In Qalbum_EditOnUpKey()
    pMe->eState = QCAM_STATE_MENU
    Qalbum_GetImageSize()   //get current image size
    Qalbum_BuildEditSizeMenu() //build a list menu for size option

  //When user hit a key in QCAM_STATE_MENU state, the event is mapped to
  //Qalbum_MenuKeyHandler()
  - On EVT_KEY in Qalbum_HandleEvent()
    CKeyHandlers[pMe->eState].HandlerFunc() ->
      Qalbum_MenuKeyHandler()

  - In Qalbum_MenuKeyHandler()
    //On AVK_UP or AVK_DOWN
    IMENUCTL_HandleEvent() //Update menu selection highlight
    Qalbum_UpdateResizeMenu() //Update radio icon
    //On AVK_SELECT
    IMENUCTL_HandleEvent() -> Send an event back to Qalbum_HandleEvent()

  - On EVT_COMMAND in Qalbum_HandleEvent()
    Qalbum_EditDoResize() -> implement resize processing
    Qalbum_UpdateEditedImage
    pMe->eState = QCAM_STATE_EDIT;

    //On AVK_CLR
    pMe->eState = QCAM_STATE_EDIT;
    Qalbum_UpdateEditedImage() -> return to edit state and update the image

  **************************************************************************
5. Add frame overlay to a photo image
  - In Qalbum_EditOnUpKey()
    pMe->eState = QCAM_STATE_THUMBNAIL;
    Qalbum_DisplayFrames() -> display frames in thumbnail mode
      QCam_BuildFrameList()
      //If no frame
      - Qalbum_DisplayTimedMsg() ->Display message and return to edit mode
      //else
      - Get current frame index
      - QCam_GetCurrentItem() //get the frame name from list
      - ISHELL_LoadImage()    //load the image
      - IIMAGE_Notify(Qalbum_ImageNotify) //decoder callback

  - In Qalbum_ImageNotify()
    QCam_GetPNGImageData()  //Frame is png image
    Draw frame image in thumbnail size
    Qalbum_DisplayFrames() -> Go back to load the next frame until all done

  //When user hit a key in QCAM_STATE_THUMBNAIL state, the event is mapped to
  //Qalbum_ThumbnailKeyHandler()
  - On EVT_KEY in Qalbum_HandleEvent()
    CKeyHandlers[pMe->eState].HandlerFunc() ->
      Qalbum_ThumbnailKeyHandler()

  - In Qalbum_ThumbnailKeyHandler()
    //On navigate keys
    Qalbum_ThumbnailSelectNext() //if thumbnail is not loading, otherwise drop the event

    //On AVK_SELECT
    pMe->eState = QCAM_STATE_EDIT;
    QCam_GetCurrentItem() //If frame is selected
    Qalbum_AddImageFrame()
      QCam_LoadPNGFrame()
      QCam_Get565TransparentVal()
      Call ipl function to convert the color format of the image data
      Call ipl function to compose the frame and photo image data
      Qalbum_EditPhoto() //return to edit mode and update the display

    //On AVK_CLR
    pMe->eState = QCAM_STATE_EDIT;
    Qalbum_EditPhoto() //return to edit mode and update the display

  **************************************************************************
6. Advanced editing
  //When user hit a key in QCAM_STATE_ADV_EDIT state, the event is mapped to
  //Qalbum_AdvEditKeyHandler() <qalbumAdv.c>
  - On EVT_KEY in Qalbum_HandleEvent()
    CKeyHandlers[pMe->eState].HandlerFunc() ->
      Qalbum_AdvEditKeyHandler()

  - In Qalbum_AdvEditKeyHandler()
    //On AVK_RIGHT or AVK_LEFT
    //If not in edit mode
    - Qalbum_DisplayAdvEditMenuName()
    - IMENUCTL_HandleEvent() -> navigate menu item and highlight the icon
    //If in Magnify or Overlay edit
    - QalbumAdvEditHandlers[index].MenuOnKeyFunc() -> Map to On key function

    //On other keys:
    QalbumAdvEditHandlers[index].MenuOnKeyFunc() -> Map to On key function

    //On AVK_CLR
    Qalbum_ReturnView()
      Qalbum_IBitmapCopy() //copy EditOut to EditIn
      pMe->eState = QCAM_STATE_VIEW;
      Qalbum_BuildIconMenu() //build menu for view mode
      Qalbum_DisplayViewMenuName()

    //Before exit function
    Qalbum_UpdateEditedImage() //Update image if edited.

  - QalbumAdvEditHandlers[index].MenuOnKeyFunc() ->
    - Qalbum_OnMagnify()
    - Qalbum_OnNeon()
    - Qalbum_OnPerspective()
        Qalbum_UpdateInIPLImage()
        Update edit parameters on key
        Call iipl function to edit the image

    - Qalbum_OnFog()
    - Qalbum_OnGaussianBlur()
    - Qalbum_OnUniformBlur()
    - Qalbum_OnMotionBlur()
    - Qalbum_OnArbRotation()
    - Qalbum_OnWhiteboard()
        Qalbum_EditStep()
        Call iipl function to edit the image

    - Qalbum_OnAlphaBlend()
    - Qalbum_OnOverlap()
    - Qalbum_OnHorizontalComp()
    - Qalbum_OnVerticalComp()
    - Qalbum_OnCenterComp()
        //Prompt user to select the second image first from thumbnail
        Qalbum_UpdateInIPLImage()
        Qalbum_Get2ndFile()
          pMe->eState = QCAM_STATE_THUMBNAIL;
          Qalbum_DisplayPhoto()
        //Whe the second input is selected
        Qalbum_EditStep() or Update edit parameters on key
        Call iipl function to edit.

    - Qalbum_OnHistogram()
    - Qalbum_OnRgbHistogram()
        Qalbum_UpdateInIPLImage()
        Call iipl function to draw the histogram

  **************************************************************************
7. Save a picture
  - In Qalbum_OnSave()
    Qalbum_IsSpaceAvailable() //Check if space available
    Qalbum_UpdateInIPLImage() //init data buffer
    pMe->eState = QCAM_STATE_SAVE;
    ISHELL_CreateDialog(IDD_SAVE_PIC)     //Create the save dialog

  - On EVT_DIALOG_INIT in Qcamera_HandleEvent()
    Qcamera_MapEventHandlers(IDD_SAVE_PIC)
      IDIALOG_SetEventHandler(Qalbum_DlgSaveEventHandler)
      IDIALOG_SetProperties()

  - On EVT_DIALOG_START in Qalbum_DlgSaveEventHandler()
    Create static field for date directory of long file name display.
    Create text field for short file name display and edit.
    Create menu buttons for user option (Save, Cancel)

    //Update the photo image after creating the dialog
    //Otherwise, the image is hide under the dialog.
    Qalbum_UpdateEditedImage()

    //If Save button is pressed, BREW IMenuCtl will handle the key event
    //and send a EVT_COMMEND back. Logic turns to here
  - On EVT_COMMAND in Qalbum_DlgSaveEventHandler()
    QCam_GetValidFileName() //check if user updated file is valid
    ISHELL_EndDialog()      //end the save dialog
    Qalbum_SavePhoto()      //Start save photo processing

  - In Qalbum_SavePhoto()
    //If file type is JPEG
    - Qalbum_SaveJpegImage()
        CALLBACK_Init(Qalbum_GetEncodeCB) //encoder callback
        Qalbum_GetImageSize()
        Create a thumbnail from the full image if size larger than QVGA
        Fill encoding structure
        IMEDIAUTIL_EncodeJPEG() //In normal case, callback after encoding

    //If file type is PNG
    - Qalbum_SavePngImage
        CALLBACK_Init(Qalbum_GetEncodeCB)
        Fill encoding structure
        IMEDIAUTIL_EncodeMedia(AEECLSID_PNG)

  - In Qalbum_GetEncodeCB()
      Qalbum_DisplayPhotoSavedMsg() -> display message and set timer to go back
        Draw the saved message on screen
        QCam_BuildPhotoList() //update the photo number
        ISHELL_SetTimer(timeout, Qalbum_EditPhotoSaved)

  - In Qalbum_EditPhotoSaved()
      pMe->eState = QCAM_STATE_VIEW
      Qalbum_ViewPhoto() -> return to view state

  **************************************************************************
8. Qalbum in SLIDESHOW mode
  - In Qalbum_AutoPlay()
      Increase file index to go to next image
      If all files in the directory is showed, go to next directory
      Qalbum_DisplayPhoto() -> Load the image, decoder callback

  - In Qalbum_ImageNotify()
      Display loaded image in full screen view
      ISHELL_SetTimer(Qalbum_AutoPlay) -> Set timer to start the next one

  //When user hit a key in QCAM_STATE_SLIDESHOW state, the event is mapped to
  //Qalbum_SlideShowKeyHandler()
  - On EVT_KEY in Qalbum_HandleEvent()
    CKeyHandlers[pMe->eState].HandlerFunc() ->
      Qalbum_SlideShowKeyHandler()

  - In Qalbum_SlideShowKeyHandler()
      //Any key will stop the slideshow and return to thumbnail mode
      pMe->eState = QCAM_STATE_THUMBNAIL;
      ISHELL_CancelTimer()
      IDISPLAY_ClearScreen()
      Qalbum_DisplayPhoto() -> Load images in thumbnail mode

  **************************************************************************
9. Qalbum in FRAME mode
  //See Logic 5 for Qalbum_DisplayFrames()

  //When user hit a key in QCAM_STATE_NULL state, the event is mapped to
  //Qalbum_CommonKeyHandler()
  - On EVT_KEY in Qalbum_HandleEvent()
    CKeyHandlers[pMe->eState].HandlerFunc() ->
      Qalbum_CommonKeyHandler()

  - In Qalbum_CommonKeyHandler()
    //On navigate keys
    Qalbum_ThumbnailSelectNext()

    //On AVK_SELECT
    QCam_GetCurrentItem() //Get the current selected frame file name
    Copy the selected frame file name to Launch data struct.
    ISHELL_CloseApplet() -> Return to Qcamera with a new frame file name

    //On AVK_CLR
    ISHELL_CloseApplet() -> Return to Qcamera without any change

  ===========================================================================*/

/*===========================================================================

FILE: qalbum_i.h

GENERAL DESCRIPTION
    This is a private header file only used by qalbum.c.

      Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qalbum/main/latest/src/qalbum_i.h#1 $ $DateTime: 2008/05/12 10:09:09 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 03/25/08  KC     Adding Geo_tagging feature.
 12/15/07  KV     Added prototype for Qalbum_BuildColorFormatMenu.
 09/06/07  NC     Added helper functions for memory redution design.
 06/18/07  NC     Fixed bugs in zoom feature.
 05/22/07  SK     JPEG Decoder Resizing feature
 04/18/07  NC     Fixed compiler warning on some condition setting.
 02/21/07  TV     Added ability to print with PBPrint
 01/24/07  ZL     Moved Qalbum_DiagReportErr() to qalbum.h.
 12/21/06  NC     Remove the Filter for RGB Histogram feature in 6K targets.
 11/28/06  NC     Filter out RGB Histogram feature from 6K targets.
 11/09/06  ZL     Added a new VIEW state before EDIT state.
 11/08/06  ZL     Moved editing feature to qalbumEdit_i.h and removed menu
                  bar icons from EditIDList[].
 11/03/06  NC     Removed function Qalbum_SetOvObject.
 10/30/06  ZL     Added Qalbum_BuildEditList() and removed menu bar icons
                  from AdvEditIDList[].
 10/09/06  NC     Defined new helper function and items.
 09/21/06  NC     Added new helper function to optimize automation test report.
 09/13/06  NC     Fixed lint errors.
 08/22/06  NC     Added Qalbum_DisplayFrameErr.
 06/19/06  NC     Added FEATURE_BT_EXTPF_BPP condition for BPP print feature.
 05/22/06  NC     Fixed compiler warning when FEATURE_APP_MMS is not defined.
 05/17/06  NC     Defined a new message timer with 1 second.
 04/20/06  HV     Build a dynamic qalbum edit menu
 02/23/06  NC     Added helper functions for Zoom and Pan logic update.
 02/09/06  NC     Enable Whiteboard and Luma Histogram support
 10/13/05  NC     Updated Qalbum_DiagAdvAlbumEdit input parameters.
 09/13/05  JN     Lint clean up.
 09/07/05  NC     Added Qalbum_DiagAddFrame for automation support.
 08/17/05  JN     Redesigned Qalbum to decouple it from AMSS.
 06/20/05  NC     Added advanced edit automation feature.
 06/16/05  JN     New feature to print photo through BT app.
 05/10/05  JN     New feature to add image description.
 04/28/05  NC     Added Qalbum_DiagGetContext() to support GetContext test.
 04/13/05  NC     Added a new helper function for automation test.
 04/11/05  JN     Added zoom and pan option.
 01/28/05  NC     Moved Qalbum_OnAdvanced() back from qalbumAdv
 12/21/04  JN     Add advanced edit option into qalbum.
 12/07/04  NC     Added DIAG features for Auto Test.
 10/21/04  NC     Added functions for save image feature
 10/19/04  JN     QCamViewer -> QCamAlbum -> Qalbum.
 09/21/04  NC     Add more features and redesign function names.
 06/16/04  NC     Initial design of the photo viewer and editor.
 ============================================================================*/
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#if defined(FEATURE_APP_QALBUM)

#include "AEEModGen.h"          // Module interface definitions
#include "AEEStdLib.h"   // AEE stdlib services

#include "AEEMedia.h"         // AEE Multimedia Services

#include "qalbum.h"
#include "qalbum.bid"
#include "QcamDisp.h"


#if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))
#include "pbprint.bid"
#endif //#if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))

#define EVT_USER_CLOSEAPP    (EVT_USER + 1)

#define QALBUM_TIMEOUT_1000  1000   // 1 seconds
#define QALBUM_TIMEOUT_2000  2000   // 2 seconds


/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static boolean Qalbum_InitData (Qalbum * pi);
static boolean Qalbum_Init (Qalbum * pMe);
static void    Qalbum_FreeAppData (Qalbum * pMe);
static void    Qalbum_Exit(Qalbum * pMe);
static boolean Qalbum_HandleEvent(Qalbum * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static boolean Qalbum_HandleStartApp(Qalbum * pMe, uint32 dwParam);
static boolean Qalbum_AlbumKeyHandler(Qalbum * pMe, AEEEvent eCode, uint16 key);
static boolean Qalbum_SlideShowKeyHandler(Qalbum * pMe, AEEEvent eCode, uint16 key);
static boolean Qalbum_ThumbnailKeyHandler(Qalbum * pMe, AEEEvent eCode, uint16 key);
static boolean Qalbum_ViewKeyHandler(Qalbum * pMe, AEEEvent eCode, uint16 key);
static boolean Qalbum_CommonKeyHandler(Qalbum * pMe, AEEEvent eCode, uint16 key);

static boolean Qalbum_OnSave(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnRevert(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnDel(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnCopy(Qalbum *pMe, uint16 key);

#ifdef FEATURE_GEOTAGGING
static void Qalbum_GetGPSInfo(Qalbum *pMe);
static void Qalbum_GetGPSInfoCB(void *pUser);
static void Qalbum_TaggingStatusNotify(void * pUser, IImage * pImage, AEEImageInfo * pi, int nErr);
#endif /* FEATURE_GEOTAGGING */

#ifdef FEATURE_APP_MMS
#error code not present
#endif
static boolean Qalbum_OnProperty(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnDescribe(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnThumbnail(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnZoom(Qalbum *pMe, uint16 key);
#if (defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))
static boolean Qalbum_OnPrint(Qalbum *pMe, uint16 key);
static boolean Qalbum_DoPrint(Qalbum *pMe);
static void    Qalbum_BuildPrintOptionMenu(Qalbum *pMe);
#endif // defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
static boolean Qalbum_OnEdit(Qalbum *pMe, uint16 key);
#ifndef FEATURE_IPL_LITE
static boolean Qalbum_OnAdvanced(Qalbum *pMe, uint16 key);
#endif //FEATURE_IPL_LITE

static boolean Qalbum_IsSpaceAvailable(Qalbum *pMe);
static void Qalbum_GetDisplaySizes (Qalbum *pMe);

static void Qalbum_DisplaySelection(Qalbum *pMe, boolean bThumbNail);
static void Qalbum_DisplayImageFileName(const Qalbum *pMe);
static void Qalbum_DisplayViewMenuName(Qalbum *pMe, int item);
static void Qalbum_SetProperties(const Qalbum *pMe, char * szText, int size);
static void Qalbum_DisplayProperties(Qalbum *pMe);
static void Qalbum_DisplayZoomImage(Qalbum *pMe);
static void Qalbum_DisplayFrameErr(Qalbum *pMe, uint16 MsgID);

static boolean Qalbum_LoadJpegImage(Qalbum *pMe, const char * pszFile, AEECLSID cls);
static boolean Qalbum_LoadJPEG(Qalbum *pMe, boolean bThumb);
static void Qalbum_LoadImage(Qalbum *pMe);
static void Qalbum_EditPhotoSaved(Qalbum *pMe);

//static void Qalbum_AutoPlay(Qalbum *pMe);
static void Qalbum_ExtractThumbnail(Qalbum *pMe);
static boolean Qalbum_DoZooming(Qalbum *pMe, uint16 cx, uint16 cy, boolean bDIB);

static void Qalbum_DeleteFileResponse(Qalbum *pMe, uint16 id);
static void Qalbum_ImageNotify(void *pUser, IImage *pImage, AEEImageInfo *pi, int nErr);
static void Qalbum_LoadImageNotify(void *pUser, IImage *pImage, AEEImageInfo *pi, int nErr);
static boolean Qalbum_EditImage(Qalbum *pMe, IBitmap *pBitmap);
static int  Qalbum_DrawImage(Qalbum *pMe, IBitmap *pBitmap, AEERect rc);

static void Qalbum_ThumbnailSelectNext(Qalbum *pMe, uint16 key);
static void Qalbum_BuildEditList(Qalbum *pMe, const uint16 *idList, int size);
static void Qalbum_BuildIconMenu(Qalbum *pMe, const CAMICONMENUID *idList, int size);
static boolean Qalbum_BuildDialog(Qalbum *pMe, uint16 dlgID, uint16 menuID, uint16 cltID);
static void    Qalbum_BuildColorFormatMenu(Qalbum * pMe);

static void Qalbum_GenerateThumbnailFileName(char *szCurName);

//Zoom and Pan functions
static void Qalbum_ValidateCoordinates(Qalbum *pMe, int cx, int cy);
static boolean Qalbum_InitZoomIn(Qalbum *pMe, AEEBitmapInfo info);
static boolean Qalbum_ZoomIn(Qalbum *pMe, boolean bDIB, AEEBitmapInfo info);
static boolean Qalbum_ZoomOut(Qalbum *pMe, boolean bDIB);
static void    Qalbum_PanUp(Qalbum *pMe);
static void    Qalbum_PanDown(Qalbum *pMe, int16 cy);
static void    Qalbum_PanLeft(Qalbum *pMe);
static void    Qalbum_PanRight(Qalbum *pMe, int16 cx);

//Test automation functions
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION

/*-------------------------------------------------------------------
EXTERN FUNCTIONS
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
MAIN EVENT HANDLERS MAPPING
-------------------------------------------------------------------*/
// Key handler mapping table
static const CKeyHandler CKeyHandlers[] =
{
  {QCAM_STATE_NULL,       Qalbum_CommonKeyHandler},
  {QCAM_STATE_DIR,        Qalbum_AlbumKeyHandler},
  {QCAM_STATE_THUMBNAIL,  Qalbum_ThumbnailKeyHandler},
  {QCAM_STATE_SLIDESHOW,  Qalbum_SlideShowKeyHandler},
  {QCAM_STATE_VIEW,       Qalbum_ViewKeyHandler},
  {QCAM_STATE_EDIT,       Qalbum_EditKeyHandler},
  {QCAM_STATE_MENU,       Qalbum_MenuKeyHandler},
#ifndef FEATURE_IPL_LITE
  {QCAM_STATE_ADV_EDIT,   Qalbum_AdvEditKeyHandler}
#endif //FEATURE_IPL_LITE
};

/*-------------------------------------------------------------------
  View Options:
-------------------------------------------------------------------*/
static const CAMICONMENUID ViewIDList[] =
{
  {IDS_SAVE,                IDB_SAVE},
  {IDS_REVERT,              IDB_UNDO},
  {IDS_IMAGE_COPY,          IDB_IMAGE_COPY},
#ifdef FEATURE_APP_MMS
#error code not present
#endif //FEATURE_APP_MMS
  {IDS_DEL,                 IDB_DEL},
  {IDS_PROPERTIES,          IDB_PROPERTIES},
  {IDS_DESCRIBE,            IDB_DESCRIBE},
  {IDS_EXTRACT_THUMBNAIL,   IDB_THUMBNAIL},
  {IDS_ZOOM,                IDB_ZOOM},
#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
  {IDS_PRINT,               IDB_PRINT},
#endif //(FEATURE_BT_EXTPF_BIP) || (FEATURE_BT_EXTPF_BPP)  || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
  {IDS_EDIT,                IDB_IMAGE_EDIT},
#ifndef FEATURE_IPL_LITE
  {IDS_ADVANCED,            IDB_ADVANCED}
#endif //FEATURE_IPL_LITE
};

/*-------------------------------------------------------------------
  Edit Options:
-------------------------------------------------------------------*/
const uint16 EditIDList[] =
{
  IDS_IMAGE_SEPIA,
  IDS_IMAGE_RESIZE,
  IDS_IMAGE_ROTATE,
  IDS_IMAGE_FLIP,
  IDS_IMAGE_GRAY_SCALE,
  IDS_IMAGE_INVERT_COLORS,
  IDS_IMAGE_POSTERIZE,
  IDS_IMAGE_SOLARIZE,
  IDS_BRIGHTNESS,
  IDS_IMAGE_HUE,
  IDS_IMAGE_SATURATION,
  IDS_IMAGE_SHARPNESS,
  IDS_FLASH,
  IDS_FRAME
};

#ifndef FEATURE_IPL_LITE
/*-------------------------------------------------------------------
  Advanced Edit Options:
-------------------------------------------------------------------*/
const uint16 AdvEditIDList[] =
{
  IDS_MAGNIFY,
  IDS_NEON,
  IDS_PERSPECTIVE,
  IDS_FOG,
  IDS_GAUSSIAN_BLUR,
  IDS_UNIFORM_BLUR,
  IDS_MOTION_BLUR,
  IDS_ARB_ROTATION,
  IDS_ALPHA_BLEND,
  IDS_OVERLAP,
  IDS_HORIZONTAL_COMP,
  IDS_VERTICAL_COMP,
  IDS_CENTER_COMP,
  IDS_WHITEBOARD,
  IDS_HISTOGRAM,
  IDS_RGB_HISTOGRAM
};
#endif //FEATURE_IPL_LITE

/*-------------------------------------------------------------------
  View Options: on select key handler function table
-------------------------------------------------------------------*/
static const QalbumMenuKeyHandler QalbumViewHandlers[] =
{
  {IDS_SAVE,                Qalbum_OnSave},
  {IDS_REVERT,              Qalbum_OnRevert},
  {IDS_IMAGE_COPY,          Qalbum_OnCopy},
#ifdef FEATURE_APP_MMS
#error code not present
#endif //FEATURE_APP_MMS
  {IDS_DEL,                 Qalbum_OnDel},
  {IDS_PROPERTIES,          Qalbum_OnProperty},
  {IDS_DESCRIBE,            Qalbum_OnDescribe},
  {IDS_EXTRACT_THUMBNAIL,   Qalbum_OnThumbnail},
  {IDS_ZOOM,                Qalbum_OnZoom},
#if defined(FEATURE_BT_EXTPF_BIP) || defined(FEATURE_BT_EXTPF_BPP) || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
  {IDS_PRINT,               Qalbum_OnPrint},
#endif //FEATURE_BT_EXTPF_BIP || FEATURE_EXTPF_BPP || defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS)
  {IDS_EDIT,                Qalbum_OnEdit},
#ifndef FEATURE_IPL_LITE
  {IDS_ADVANCED,            Qalbum_OnAdvanced}
#endif //FEATURE_IPL_LITE
};

#endif //FEATURE_APP_QALBUM

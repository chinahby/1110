#ifndef QCAMDISP_H
#define QCAMDISP_H
/*===========================================================================

FILE: QCamDisp.h

GENERAL DESCRIPTION
    This is the common header file contains definitions for share functions

    Copyright (c) 2004 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamcommon/main/latest/inc/QcamDisp.h#2 $ $DateTime: 2009/03/04 00:40:22 $ $Author: c_rtiwar $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 03/04/09  RK     Fixed the CR 161312 Directory and Image names are overlapping in Album mode.
 12/13/07  NC     Fixed compiling issue if FEATURE_MMOVERLAY is not defined.
 06/18/07  NC     Added QCamDisp_GetResizeRatio and updated QCamDisp_GetResizedRect.
 03/28/07  TV     Added methods to fit image sizes to the display keeping the aspect ratio
 03/23/07  KV     Added dynamic font size parameters in QCamDisp_GetRect() and QCamDisp_GetDisplayRect()
 02/13/07  NC     Fixed full screen transparent issue.
 01/08/07  NC     Replace FEATURE_CAMERA7500 with FEATURE_MMOVERLAY for MMOverlay.
 11/21/06  NC     Replace IOverlay with IMMOverlay for BREW 4.0.
 11/03/06  NC     Added multiple displays share functions.
 10/13/06  NC     Added new IOverlay features.
 08/22/06  RK     Updated the function QCamDisp_DrawYCbCrImage(),QCamDisp_DrawYCbCrImage().
 04/05/06  HV     Added parameter IYCBCRFactory to QCamDisp_DrawYCbCrImage().
 03/23/06  NC     Added parameter IIpl to QCamDisp_DrawYCbCrImage().
 11/08/05  NC     Remove some definitions to a new header file.
 08/17/05  JN     Added more arguments to QCamDisp_DrawImage().
 04/11/05  JN     Added more arguments to QCamDisp_DrawYCbCrImage().
 03/02/05  JN     Added support to draw YCbCr image.
 01/14/05  NC     Updated MDP related functions with a new param IOverlay *
 06/16/04  NC     Initial design of the share file qcamdisp.h.
 ============================================================================*/
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#ifdef AEE_SIMULATOR
#include "QcamSim.h"
#else
#include "comdef.h"
#endif

#if defined(FEATURE_APP_QCAMERA)
#include "QcamCommon.h"
#include "AEEDisp.h"
#include "IYCbCrFactory.h"
#include "IIPL.h"
#ifdef FEATURE_OVERLAY
#include "IOverlay.h"
#ifdef FEATURE_MMOVERLAY
#include "AEEMMOverlay.h"
#else
#include "mdp_drv_api.h"
#endif //ifndef FEATURE_MMOVERLAY
#endif //FEATURE_OVERLAY
#ifndef FEATURE_MMOVERLAY
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
#undef FEATURE_CAMERA_MULTIPLE_DISPLAY
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY
#endif //FEATURE_MMOVERLAY
/*============================================================================
DEFINITION FOR DISPLAY POSITION SETTING

===========================================================================*/
//Settings for thumbnail display
#define QCAM_DIR_IMAGE_WIDTH  32
#define QCAM_DIR_IMAGE_HEIGHT 26
#define QCAM_DIR_TEXT_HEIGHT  25

//Menu title image width
#define TITLE_IMAGE_WIDTH 16

// RGB: 255,0,255 -> a magenta-like color
#define DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB888 0x00FF00FF
// same color in "565" format
#define DEFAULT_TRANSPARENT_PIXEL_VALUE_RGB565 0xF81F

typedef enum{
  DIRIMAGE = 0,
  DIRTEXT,
  THUMBNAIL,
  SELDIR,
  SELTHUMBNAIL,
  PHOTO
} ImageType;

#ifdef FEATURE_MMOVERLAY
typedef struct
{
  AEERect rc;
  IBitmap *pBmp;
  RGBVAL  transparent;
  uint32  alpha;
  uint8   layer;
} OvData;
#endif //FEATURE_MMOVERLAY
/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
int16 QCamDisp_GetResizeRatio(AEERect rc, int16 dx, int16 dy);
AEERect QCamDisp_GetCenterRect(AEERect rc, int16 dx, int16 dy);
AEERect QCamDisp_GetResizedRect(AEERect rcDisp, int16 dxImg, int16 dyImg);

void QCamDisp_InitScreen(IDisplay *pIDisp, uint16 dx, uint16 dy);
AEERect QCamDisp_GetRect(uint16 dx, uint16 dy, int16 idx, int16 start, 
                         ImageType eType, int16 TitleHeight);
boolean QCamDisp_IsSizeScreen(uint16 dx, uint16 dy, int size);
void QCamDisp_GetDisplayRect(uint16 screenDx, uint16 screenDy, 
                             uint16 dyOffset, AEERect *pRect, int16 TitleHeight);
AEESize QCamDisp_AdjustDisplaySizeFitX(AEESize* pImgRatio, AEESize dispSize);
AEESize QCamDisp_AdjustDisplaySizeFitY(AEESize* pImgRatio, AEESize dispSize);
AEESize QCamDisp_AdjustDisplaySize(AEESize* pImgRatio, AEESize dispSize);

void QCamDisp_UpdateStaticField(IStatic *dirCtrl, const char * fileName, 
                                uint32 dirLen, uint32 dateDirLen);
void QCamDisp_UpdateTextField(ITextCtl  *textCtl, const char * fileName, int dateDirLen);

void QCamDisp_DrawDirImage(IDisplay *pIDisp, IImage *pDirImage, AEERect imgRC);
void QCamDisp_DrawDirText(IDisplay *pIDisp, const char *pDirText, AEERect textRC);
void QCamDisp_DrawSelect(IDisplay *pIDisp, AEERect *oldRC, AEERect newRC);
int QCamDisp_DrawImage(IDisplay *pIDisp, IIpl *pIIpl, IDIB *dib, const AEERect *rc,
                       int xSrc, int ySrc, boolean bResize);
int QCamDisp_DrawYCbCrImage(IDisplay *pIDisp, IYCBCRFactory *pIYCBCRFactory, 
                            IIpl *pIIpl, IYCbCr *pYC, const AEERect *rc,
                            int xSrc, int ySrc, boolean bResize);

void QCamDisp_DrawMenuTitle(IDisplay *pIDisp, AEERect rc, IImage *pImage, 
                            const AECHAR *txtStr);
void QCamDisp_DrawIcon(IDisplay *pIDisp, IImage *pIcon, AEERect rc);
void QCamDisp_ShowMsg(IDisplay *pIDisp, const AECHAR *szBuf, AEERect rc);
void QCamDisp_ShowMessage(IShell * pIShell, const char *title, const char *text);

/**************** MDP related functions *******************/
#ifdef FEATURE_OVERLAY
#ifdef FEATURE_CAMERA_MULTIPLE_DISPLAY
int QCamDisp_ChangeDisplay(IShell *pIShell, AEECLSID ClsId, int state);
AEECLSID QCamDisp_GetOldDispClsId(IShell *pIShell);
void QCamDisp_DisplayOvOnTV(IShell *pIShell, IMMOverlay **ppMMOverlay, IBitmap *pBitmap);
#endif //FEATURE_CAMERA_MULTIPLE_DISPLAY

#ifdef FEATURE_MMOVERLAY
boolean QCamDisp_RegisterOv (IMMOverlayMgr *pIMMOverlayMgr, 
                             IMMOverlay **ppIMMOverlay, OvData ov);
#else
void QCamDisp_InitMdp (IDisplay *pIDisp);
void QCamDisp_MdpDeregister (IOverlay *pIOverlay);
OVERLAY_ID QCamDisp_MdpRegister (IDisplay *pIDisp, IOverlay *pIOverlay, AEERect rc);
void QCamDisp_MdpRegisterFullScreen (IDisplay *pIDisp, IOverlay *pIOverlay, 
                                     uint16 dx, uint16 dy);
#endif // #ifdef FEATURE_MMOVERLAY
#endif // FEATURE_OVERLAY

/**************** PNG decode related functions *******************/
IDIB *QCam_GetPNGImageData(IDisplay *pIDisp, IImage *pImage);
boolean QCam_LoadPNGFrame(IShell *pIShell, IFileMgr *pIFileMgr, 
                          const char *pFile, IDIB **pDib);
void QCam_Get565TransparentVal(uint32 oldVal, uint16 *newVal);

void QCam_PNG2562DIB24(byte *dibBuf, const byte *pngBuf, const byte *palette, 
                       uint32 width, uint32 height);
void QCam_PNG242DIB24(byte *dibBuf, const byte *pngBuf, uint32 width, uint32 height);
void QCam_RGB242YCbCr(unsigned char* data_in,unsigned char* data_out, 
                      short width, short height);
void QCam_RGB242RGB565(uint8* data_in, uint16* data_out, int16 width, int16 height);
void QCam_RGB82RGB565(uint8* data_in, uint8* palette, uint16* data_out, 
                      int16 width, int16 height);


#endif //FEATURE_APP_QCAMERA
#endif //QCAMDISP_H

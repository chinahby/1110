/**
 * @file a800_camera.h
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#include "A8_common.h"
#include "AIT700_EBIbus.h"
#include "A8_graphics.h"
#include "A8_win.h"
#include "A8_panel.h"
#include "A8_dscapi.h"
#include "A8_sensor.h"

#ifndef _A800CAMERA_H_
#define _A800CAMERA_H_

//==============================

#define	DIR_LEFT	0
#define	DIR_RIGHT	1
#define	DIR_UP		2
#define	DIR_DOWN	3

#define	A8_SENSOR_SEN_PIN 0
#define	A8_SENSOR_SCK_PIN 1
#define	A8_SENSOR_SDA_PIN 2
#define	A8_SENSOR_RST_PIN 3

/* Definition for WinState in PreviewDT and PlaybackDT */
/* WinState is used to determine which window should be turned on */
#define A8_MAIN_WIN_ON          (1)
#define A8_PIP_WIN_ON           (1<<1)
#define A8_OVERLAY_WIN_ON       (1<<2)
#define A8_ICON_WIN_ON          (1<<3)

/* Definition for WinPriority in PreviewDT and PlaybackDT */
/* WinPriority is used to determine the display order of windows */
#define A8_WIN_PRIORITY_NORM          		(0)      /* 0:  Overlay, Main, PIP */
#define A8_WIN_PRIORITY_NORM_ALL   		(1)      /* 1:  Icon, Overlay, Main, PIP */
#define A8_WIN_PRIORITY_REVE          			(2)      /* 2:  Overlay, PIP, Main */
#define A8_WIN_PRIORITY_NORM_OVBACK	(3)      /* 3:  Main, PIP, Overlay */
#define A8_WIN_PRIORITY_REVE_OVBACK   (4)      /* 4:  PIP, Main, Overlay */

/* Definition for BusType in LCDInfoDT */

/* Definition for BurstMode in LCDInfoDT */

#define	A8_INT_FOR_HOST	0x01
#define	A8_INT_FOR_CPU	0x02

typedef struct Coordinate
{
	u_short	 X;
	u_short	 Y;	
	
}A8S_Coordinate;

typedef struct SystemSetting
{
	u_int	 MemoryAddress;
	u_int	 Command_Pin;
}A8S_SystemSetting;

typedef struct CameraSetting
{
	u_short	 CameraDirection;
	u_short	 Environment;
	u_short	 DeBandMode;
}A8S_CameraSetting;

typedef struct IconInfo
{
   u_short IconWidth, IconHeight;
   u_short IconBufAddr;
   struct IconInfo *NextIcon;
} IconInfoDT;

typedef struct OSDInfo
{
	u_short OSDBufWidth, OSDBufHeight;
	u_short OSDDispLcdX, OSDDispLcdY;
	u_short OSDBufFormat;    /* RGB565 or Yuv422 */
	u_int OSDBufAddr;
	u_short OSDEnable;
} OSDInfoDT;

typedef struct
{
   u_short BufWidth, BufHeight;
   u_short DispWidth, DispHeight;
   u_short BufOriginX, BufOriginY;
   u_short DispLcdX, DispLcdY;
   u_short BufFormat;    /* RGB565 or Yuv422 */
   u_short Rotate;
   u_int BufAddr;
   u_short UpScale, DownScale;
   u_short ScaleRatio;
   u_short ZoomStep;
   u_short	LcdRefreshMode;
	OSDInfoDT MainOSD;
	OSDInfoDT OverlayOSD;
	OSDInfoDT IconOSD;

   u_short IconCount;
   IconInfoDT *IconPtr;
   u_short WinState;      /* MAIN_WIN_ON, PIP_WIN_ON, OVERLAY_WIN_ON, ICON_WIN_ON */
   u_short WinPriority;   /* Window display order */
							/* 0:  Overlay, Main, PIP */
							/* 1:  Overlay, PIP, Main */
							/* 2:  Main, PIP, Overlay */
							/* 3:  PIP, Main, Overlay */
	u_short TransColor;						
} WinInfoDT;
#ifdef __TWO_SENSOR__
typedef enum
{
	BACK_SENSOR = 0,
	FRONT_SENSOR = 1
}USE_WHICH_SENSOR;	
#endif


typedef struct SensorInfo
{
   u_short PrevWidth, PrevHeight;
   u_short MaxWidth, MaxHeight;
   u_short IspUsed;
} SensorInfoDT;

typedef struct
{
	s_short LCDCount;
	s_short ActiveLCDNo;      /* 0 = main LCD, 1 = sub LCD */
	LCDInfoDT MainLCDInfo;
#ifndef A8_DISABLE_SUB_LCD_FUNCTION	
	LCDInfoDT SubLCDInfo;
#endif
	SensorInfoDT SensorDes;

	WinInfoDT AITPreviewOSDModeSetting;


	WinInfoDT* Select;
	IconInfoDT IconDes[2];
} A800InfoDT;

typedef struct A800IMAGE
{
	u_int   ImageAddr;
	u_short Width;
	u_short Height;
	u_short  ColorDepth;
}A8S_IMAGE;

typedef struct A800Rect
{
	u_short  X1;
	u_short  Y1;
	u_short  X2;
	u_short  Y2;
}A8S_Rect;

typedef struct A800CaptureRation
{
	u_short  Width;
	u_short  Height;
	u_short  OffsetX;
	u_short  OffsetY;
	u_short  DownScale;
	u_short  UPScale;
}A8S_CaptureRation;

typedef struct A800_YUV_INFO
{
	u_char *yBuf;
	u_char *cbBuf;
	u_char *crBuf;
	u_short width;
	u_short height;
}A8S_YUV_INFO;

typedef struct A800_IMAGE_INFO
{
	u_short *dataBuf;
	u_short width;
	u_short height;
	u_char  colorDepth;
}A8S_IMAGE_INFO;

typedef struct A800_JPEG_INFO
{
	u_short *dataBuf;
	u_char  colorDepth;
	u_int     fileSize;
	u_short width;
	u_short height;
	u_short sx;
	u_short sy;
}A8S_JPEG_INFO;

//==========================================================================
// Function 	: A800_ChangeCameraSetting(u_char mode)
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern WinInfoDT *A800_ChangeCameraSetting(PREVIEW_MODE mode);
//==========================================================================
// Function 	: A800_DownloadFirmware()
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_char	A800_DownloadFirmware(u_char *ptr, u_int fwSize);
//==========================================================================
// Function 	: A800_Camera_Active_Mode() ; A800_Camera_Bypass_Mode()
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_char 	A800_Camera_Bypass_Mode(void);
extern u_char 	A800_Camera_Active_Mode(void);
//==========================================================================
// Function 	: A800_PowerSavingOn()
// Purpose      : enable ASIC internal IP
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_char 	A800_PowerSavingOn(void);
//==========================================================================
// Function 	: A800_PowerSavingOff()
// Purpose      : disable ASIC internal IP
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_char 	A800_PowerSavingOff(void);
//==========================================================================
// Function 	: A800_InitDefine(void)
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_char A800_InitDefine(void);
//==========================================================================
// Function 	: A800_LCDInit(value)
// Purpose      : init LCD panel and AIT800 LCD control
// Comments     :
// Parameters	: value : 0:power init by host, 1:power init by AIT800
// Return       : 
//==========================================================================
extern u_char 	A800_LCDInit(void);
//==========================================================================
// Function 	: A800_GraphicInit()
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_char 	A800_GraphicInit(void);
extern u_char 	IconInit(void);
//==========================================================================
// Function 	: A800_JPEGQuality(size)
// Purpose      : change bitrate contrrol target size
// Comments     :
// Parameters	: size : fine, middle, normal, economy
// Return       : 
//==========================================================================
extern u_char 	A800_JPEGQuality(u_char size);

//==========================================================================
// Function 	: A800_SensorFixedFrameRate( u_char set )
// Purpose      : 
// Comments     :
// Parameters	:
// Return       : 
//==========================================================================
extern u_char A800_SensorFixedFrameRate( u_char set );
//==========================================================================
// Function 	: A800_SensorRotation(value)
// Purpose      : 
// Comments     :
// Parameters	: value : 0:normal, 1:mirror, 2:vertical, 3:rotate
// Return       : 
//==========================================================================
extern u_char 	A800_SensorRotation(u_char value);
//==========================================================================
// Function 	: A800_SensorBrightness(value)
// Purpose      : tune the image exposure targe
// Comments     : + 0.25EV step
// Parameters	: value : 1~12
// Return       : 
//==========================================================================
extern u_char 	A800_SensorBrightness(u_char Value);
//==========================================================================
// Function 	: A800_SensorDebandMode(value)
// Purpose      : 50hz/60hz del-banding 
// Comments     :
// Parameters	: value : 0:xxxx, 1:60hz(Default), 2:50hz
// Return       : 
//==========================================================================
extern u_char 	A800_SensorDebandMode(u_char value);
//==========================================================================
// Function 	: A800_SensorWBMode(value)
// Purpose      : auto white balance
// Comments     :
// Parameters	: value : 0:auto, 1:8000k, 2:6500k, 3:4000k, 4:2500k
// Return       : 
//==========================================================================
extern u_char 	A800_SensorWBMode(u_char value);
//==========================================================================
// Function 	: A800_SensorMatrixMode(void)
// Purpose      : Matrix Mode
// Comments     :
// Parameters	
// Return       : 
//==========================================================================
extern u_char 	A800_SensorMatrixMode(void);
//==========================================================================
// Function 	: A800_SensorCenterWeightedMode(void)
// Purpose      : CenterWeighted Mode
// Comments     :
// Parameters	
// Return       : 
//==========================================================================
extern u_char 	A800_SensorCenterWeightedMode(void);
//==========================================================================
// Function 	: A800_SensorSpotMode(void)
// Purpose      : Spot Mode
// Comments     :
// Parameters	
// Return       : 
//==========================================================================
extern u_char 	A800_SensorSpotMode(void);
//==========================================================================
// Function 	: A800_GammaMode(value)
// Purpose      : 
// Comments     :
// Parameters	: 0:disable Gamma 1:enable gamma and load gamma table
// Return       : 
//==========================================================================
//u_char 	A800_GammaMode(u_char value);
//==========================================================================
// Function 	: A800_SetContrast(value)
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_char 	A800_SetContrast(u_char value);
//==========================================================================
// Function 	: A800_SensorNightMode(value)
// Purpose      : 
// Comments     :
// Parameters	: 0 : Disable , 1 : Enable
// Return       : 
//==========================================================================
extern u_char 	A800_SensorNightMode(u_char value);
//==========================================================================
// Function 	: A800_SensorEffect(value)
// Purpose      : 
// Comments     :
// Parameters	: value : 0:normal, 1~12 (specia, BW, oil,~~~)
// Return       : 
//==========================================================================
extern u_short	A800_SensorEffect(u_char value);
//=========================================================================
// A800_CheckSensorID
// Arguments :
//		None
//=========================================================================
extern u_short	A800_CheckSensorID(u_short Reg);
//==========================================================================
// Function 	: A800_OpenPreviewWindow()
// Purpose      : enable preview windows on
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
u_char 	A800_OpenPreviewWindow(void);
//==========================================================================
// Function 	: A800_ClosePreviewWindow()
// Purpose      : disable preview window off
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
u_char 	A800_ClosePreviewWindow(void);
//==========================================================================
// Function 	: A800_OpenMainWindow()
// Purpose      : enable preview windows on
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
u_char 	A800_OpenMainWindow(void);
//==========================================================================
// Function 	: A800_CloseMainWindow()
// Purpose      : disable preview window off
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
u_char 	A800_CloseMainWindow(void);
//==========================================================================
// Function 	: A800_MainLCDPreviewActive()
// Purpose      : 
// Comments     : 
// Parameters    : 
// Return       : 0 : Success ; 1 : TimeOut
//==========================================================================
extern u_short 	A800_MainLCDPreviewActive(void);
//==========================================================================
// Function 	: A800_SubLCDPreviewActive()
// Purpose      : 
// Comments     : 
// Parameters    : 
// Return       : 0 : Success ; 1 : TimeOut
//==========================================================================
extern u_short 	A800_SubLCDPreviewActive(void);
/**
 @fn		u_short A800_SetSECCameraMode()
 @brief	Select Preview mode to SEC Video Mode.
 @return	u_short.
 @note	.
 @bug	N/A.
*/
extern u_short A800_SetSECCameraMode(u_short	VideoMode);
//==========================================================================
// Function 	: A800_SetSECViewResolution(u_short ViewResol)
// Purpose      : 
// Comments     : 
// Parameters	: 
// Return       : 
//==========================================================================
extern u_short A800_SetSECViewResolution(u_short	ViewResol);
//==========================================================================
// Function 	: A800_SetPreviewMode(u_short mode)
// Purpose      : 
// Comments     : 
// Parameters	: 
// Return       : 
//==========================================================================
extern u_short	A800_SetPreviewMode(u_short mode);
//==========================================================================
// Function 	: A800_ResumeToPreviewMode()
// Purpose      : 
// Comments     : 
// Parameters	: 
// Return       : 
//==========================================================================
extern u_short	A800_ResumeToPreviewMode(u_short mode);
//==========================================================================
// Function 	: A800_SetPlaybackMode()
// Purpose      : 
// Comments     : 
// Parameters	: 
// Return       : 
//==========================================================================
extern u_short		A800_SetPlaybackMode(u_short mode);
//==========================================================================
// Function 	: A800_SetPreviewParam(u_short mode, u_short Width, u_short Height, u_short OffsetX, u_short OffsetY, u_short ZoomRate, u_short SensorPrevRes )
// Purpose      : 
// Comments     : 
// Parameters	:
// Return       : 
//==========================================================================
extern u_char	A800_SetPreviewParam(u_short mode, u_short Width, u_short Height, u_short OffsetX, u_short OffsetY, u_short ZoomRate, u_short SensorPrevRes );
//==========================================================================
// Function 	: A800_SetPreviewZoom(UINT16 mode, UINT16 ZoomRate)
// Purpose	: Adjust the preview zoom rate ( the preview view angle will be changed )
// Comments	: Notice that the function can only be called in preview mode.
// Parameters	:  
//		mode : the preview mode, must be one of FULL_SCREEN_PREVIEW_MODE, NORMAL_SCREEN_PREVIEW_MODE or HALF_SCREEN_PREVIEW_MODE
//    		ZoomRate : Digital Zoom Rate. 
//			Normal Mode : 0 ... 1X, 1 ... 1.25X, 2 ... 1.5X, 3 ... 1.75X, 4 ... 2X
//			Half Mode : 0 ... 1X, 1 ... 1.75X, 2 ... 2.5X, 3 ... 3.25X, 4 ... 4X
// Return      : 0
//==========================================================================
extern u_short	A800_SetPreviewZoom(u_short mode, u_short ZoomRate);
//==========================================================================
// Function 	: A800_Preview_MainWindow_SemiTransparency()
// Purpose      : 
// Comments     : 
// Parameters	:
// Return       : 
//==========================================================================
extern u_char	A800_Preview_MainWindow_SemiTransparency (u_int chromakey, u_short transparencyRate);
//==========================================================================
// Function 	: A800_Preview_MainWindow_Transparency()
// Purpose      : 
// Comments     : 
// Parameters	:
// Return       : 
//==========================================================================
extern u_char	A800_Preview_MainWindow_Transparency( u_short enable, u_int keycolor );
//==========================================================================
// Function 	: A800_JPEGCapture(u_char resolution, u_int maxFileSize, u_short *pdata, u_int *filesize)
// Purpose      : capture picture for JPEG format
// Comments     : 1.3M, VGA, QVGA,QQVGA, CIF, QCIF, QQCIF,
// Parameters	: resolution : 0:1.3M, 1:VGA, 2:QVGA, 3:QQVGA, 4:CIF, 5:QCIF, 6:QQCIF
//		: size : FINE, MIDDLE, NORMAL, ECONMY
//		: type : 
// Return       : 
//==========================================================================
extern A8_ERROR_MSG 	A800_JPEGCapture(u_char resolution, u_int maxFileSize, u_short *pdata, u_int *filesize);
//==========================================================================
// Function 	: A800_TakeStickerImage()
// Purpose     : 
// Comments    :
// Parameters	:  
//		resolution : 0x05 ... 128x160, 0x06 ... 64x72
//    pdata : the address of the image buffer ( in host )
//		filesize : the image size
// Return      : 0 ... Success, 1, 2, 3, 4 ... Failure
//==========================================================================
extern s_short	A800_TakeStickerImage(u_short resolution, u_short *pdata, u_int *filesize);
//==========================================================================
// Function 	: A800_DrawFrameBGR()
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_char A800_DrawFrameBGR(A8S_IMAGE * SourceImage,u_short X,u_short Y);
//==========================================================================
// Function 	: A800_UpdateMainWindowMemory()
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern s_short A800_UpdateMainWindowMemory( u_short *MemPtr, u_short x, u_short y, u_short w, u_short h );
//==========================================================================
// Function 	: A800_ClearMainWindowMemory()
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_char A800_ClearMainWindowMemory(u_short chromaKey);
//==========================================================================
// Function 	: A800_ClearWindowMemory()
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_char A800_ClearWindowMemory(void);
//==========================================================================
// Function 	: A800_DrawFrame(*pdata)
// Purpose      : 
// Comments     :
// Parameters	: *pdata : file point  
// Return       : 
//==========================================================================
extern u_char	A800_DrawFrame(A8S_IMAGE * SourceImage,u_short X,u_short Y);
//==========================================================================
// Function 	: A800_MemoryTestMode()
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_int A800_MemoryTestMode(void);
//==========================================================================
// Function 	: A800_GetFirmwareMode()
// Purpose      : 
// Comments     :
// Parameters	:  
// Return       : 
//==========================================================================
extern u_short A800_GetFirmwareMode(void);
//***********************************************************************************************
//***********************************************************************************************
extern u_short A800_DecodeJpeg( u_short *jpgData, u_int jpgLength, u_short *outW, u_short *outH, u_short *rgbBuf, u_char autoScale );
extern u_short A800_DecodeJpegToFixedBufferSize( u_short *jpgData, u_int jpgLength, u_short *outW, u_short *outH, u_short offsetX, u_short *rgbBuf);
extern unsigned short A800_GrabDecodeJpeg(unsigned short *srcJpgData,
									unsigned long srcJpgLength,
									unsigned short srcGrabX,
									unsigned short srcGrabY,
									unsigned short srcGrabW,
									unsigned short srcGrabH,
									unsigned short *rgbBuf,
									unsigned short outW,
									unsigned short outH);

//New AIT701 SW
extern u_char A800_DecodeJpegViaFIFO_SWSample(A8S_JPEG_INFO SrcJPEG, u_short* pRgbBuf, u_short OutW, u_short OutH, u_char OutFormat) ;

//***********************************************************************************************
//***********************************************************************************************
extern u_short YCbCr2RGB(u_short *RGB,u_short *YUV);
extern u_short A800_YCbCr2RGB(u_short *yuv_ptr, u_short *rgb_ptr, u_short width, u_short height);

extern u_short A800_EnableTGToHostInterrupt(u_char cSignalType, u_short cPolarity );
extern u_short A800_DisableTGToHostInterrupt(u_char cSignalType);
extern u_short A800_ClearTGToHostInterruptStatus(u_char cSignalType);
extern u_short A800_CheckTGToHostInterruptStatus(u_char cSignalType);


extern s_short A800_EnableCPUToHostInterrupt( s_short Polarity );
extern s_short A800_DisableCPUToHostInterrupt(void);
extern s_short A800_ClearCPUToHostInterruptStatus(void);

extern s_short	A800_CopyPreviewWindowBufferToHost( u_short *FrameBuf );
extern s_short	A800_CopyHostBufferToWindowBuffer( u_short *HostFrameBuf );

#if AIT_VIDEO_PHONE_SUPPORT
extern u_char A800_VideoPhoneStart(void);
extern u_char A800_VideoPhoneStop(void);
extern u_char A800_VideoPhoneGetRGB565Data(u_short rgbWidth, u_short rgbHeight, u_char *rgb565_ptr);
extern u_char A800_VideoPhoneGetYUV420Data(u_short yuvWidth, u_short yuvHeight, u_char *yuv420_ptr);
#endif

//***********************************************************************************************
//***********************************************************************************************
extern u_short A800_GetGPIO(u_short gpio, u_short PullUpDown);
extern u_short A800_SetGPIO(u_short gpio);
extern u_short A800_ClearGPIO(u_short gpio);

extern u_short A800_IsResetStatus(void);
extern u_char 	A800_StartPreview(void);
extern u_char 	A800_StopPreview(void);

extern u_char A800_SetPreviewLinearZoom(u_short Level);

extern u_short A800_SetCaptureResolution(u_short Reso);

//=============================
// Image Edit
extern s_short A800_Adjust_Brightness(A8S_IMAGE *Image, s_short bright);
extern s_short A800_Adjust_Contrast(A8S_IMAGE *Image, s_short contrast);
extern s_short A800_Set_Effect_Black_White(A8S_IMAGE *Image );
extern s_short A800_Set_Effect_Moonlight(A8S_IMAGE *Image );
extern s_short A800_Set_Effect_Antique(A8S_IMAGE *Image );
extern s_short A800_Set_Effect_Sepia(A8S_IMAGE *Image );
extern s_short A800_Set_Effect_Negative(A8S_IMAGE *Image );
extern s_short A800_Set_Effect_Fog(A8S_IMAGE *Image );
extern s_short A800_Set_Effect_Sketch(A8S_IMAGE *Image, u_char *OpBuffer );
extern s_short A800_Adjust_CR_MG_YB(A8S_IMAGE *Image, s_short cr, s_short mg, s_short yb) ;
extern s_short A800_Set_Effect_Blur(A8S_IMAGE *Image, u_char *OpBuffer);
extern s_short A800_Set_Effect_Sharpness(A8S_IMAGE *Image, u_char *OpBuffer);
extern s_short A800_Set_Effect_Solarize(A8S_IMAGE *Image) ;
extern s_short A800_Set_Effect_Posterize(A8S_IMAGE *Image) ;
extern void A800_PIPBypass(u_char bEnable);
extern void A800_SetQTable(u_char Index);
extern u_short A800_TakeJpegWithSticker(u_short *pdata, u_int *filesize);
extern void A800_FillJpegScaleUp(u_short JpegWidth, u_short JpegHeight, u_short InputWidth, u_short InputHeight, u_short	ScaleM, u_short ScaleN);
extern void A800_FillJpegScaleDown(u_short JpegWidth, u_short JpegHeight, u_short InputWidth, u_short InputHeight, u_short ScaleM, u_short ScaleN);
extern void A800_TakeJpegSetting(u_int JpegWidth, u_int JpegHeight);
extern void A800_TakeJpegPicture( s_short PictSize, s_short ZoomRate, u_char Re_Flag );
extern A8_ERROR_MSG 	A800_TakeJPEG(u_char resolution, u_int maxFileSize, u_short *pdata, u_int *filesize);
extern u_char A800_UpdateLCD(void);
extern u_char A800_SetPllFreq(u_char ait_mode, u_char on);
extern void A800_LCDScaler(u_short InWidth, u_short InHeight, u_short OutWidth, u_short OutHeight);
extern s_short A800_UpdateOSDBuffer( u_short mode, u_short *MemPtr, u_short x, u_short y, u_short w, u_short h );
extern u_char A800_ClearOSDBuffer(u_short chromaKey, u_short mode);

extern u_short	A800_SetMJPEGRecordStart(void);
extern u_short	A800_SetMJPEGRecordStop(void);
extern u_char A800_TransferMJPEGFrameToHost(u_char *frameBuf, u_int *frameSize);


extern u_char A800_PreInit_Sensor(void);


extern u_char A800_TestRegisterAccess(void);
extern u_char 	A800_TestMemoryAccess(void);
__align(2)static const u_char	AIT_QTableSuperFine[0x80]={
 0x03, 0x02, 0x02, 0x03, 0x02, 0x02, 0x03, 0x03,
 0x03, 0x03, 0x04, 0x03, 0x03, 0x04, 0x05, 0x08,
 0x05, 0x05, 0x04, 0x04, 0x05, 0x0a, 0x07, 0x07,
 0x06, 0x08, 0x0c, 0x0a, 0x0c, 0x0c, 0x0b, 0x0a,
 0x0b, 0x0b, 0x0d, 0x0e, 0x12, 0x10, 0x0d, 0x0e,
 0x11, 0x0e, 0x0b, 0x0b, 0x10, 0x16, 0x10, 0x11,
 0x13, 0x14, 0x15, 0x15, 0x15, 0x0c, 0x0f, 0x17,
 0x18, 0x16, 0x14, 0x18, 0x12, 0x14, 0x15, 0x16,
                 
 0x03, 0x04, 0x04, 0x05, 0x04, 0x05, 0x09, 0x05,
 0x05, 0x09, 0x14, 0x0d, 0x0b, 0x0d, 0x14, 0x14,
 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x15
};
__align(2)static	const	u_char	AIT_QTableFine[]={
 0x05, 0x05, 0x05, 0x06, 0x08, 0x08, 0x08, 0x0a,
 0x06, 0x08, 0x0b, 0x0b, 0x0f, 0x0f, 0x0f, 0x0f,
 0x08, 0x0a, 0x0a, 0x0d, 0x0f, 0x0f, 0x12, 0x12,
 0x0d, 0x0f, 0x14, 0x17, 0x17, 0x19, 0x1a, 0x1a,
 0x15, 0x18, 0x1a, 0x1a, 0x1e, 0x1e, 0x1e, 0x1e,
 0x15, 0x18, 0x1e, 0x1e, 0x1e ,0x1e, 0x1e, 0x1e,
 0x18, 0x1c, 0x1e, 0x1e, 0x1e, 0x20, 0x20, 0x20,
 0x19, 0x1e, 0x1e, 0x20, 0x20, 0x20, 0x1e, 0x1e,
                 
 0x06, 0x06, 0x06, 0x08, 0x0d, 0x10, 0x12, 0x12,
 0x0a, 0x0d, 0x0e, 0x0e, 0x10, 0x10, 0x13, 0x14,
 0x0d, 0x0f, 0x0f, 0x0f, 0x13, 0x17, 0x19, 0x19,
 0x0f, 0x13, 0x15, 0x18, 0x1a, 0x1c, 0x1d, 0x1f,
 0x15, 0x19, 0x1c, 0x1e, 0x21, 0x21, 0x21, 0x21,
 0x19, 0x1c, 0x1e, 0x21, 0x21, 0x21, 0x21, 0x21,
 0x19, 0x1e, 0x21, 0x21, 0x23, 0x23, 0x21, 0x21,
 0x1e, 0x21, 0x21, 0x23, 0x23, 0x21, 0x1e, 0x1e
};
__align(2)static	const	u_char	AIT_QTableNormal[]={
 13, 11, 13, 15, 18, 18, 18, 22,
 13, 13, 22, 22, 27, 27, 27, 32,
 18, 22, 22, 27, 32, 32, 36, 36,
 30, 34, 42, 47, 47, 52, 54, 54,
 45, 50, 54, 54, 60, 60, 65, 65,
 45, 50, 63, 63, 63, 63, 63, 63,
 50, 52, 63, 63, 63, 68, 63, 63,
 59, 63, 68, 68, 68, 63, 63, 63,
                 
 13, 16, 16, 18, 27, 36, 40, 40,
 22, 27, 32, 32, 32, 36, 40, 40,
 27, 32, 34, 36, 40, 50, 54, 44,
 36, 40, 45, 50, 50, 54, 63, 63,
 45, 54, 59, 63, 68, 68, 68, 68,
 54, 59, 63, 68, 68, 68, 68, 68,
 54, 63, 68, 68, 72, 72, 72, 72,
 63, 68, 68, 72, 72, 72, 68, 68
};
__align(2)static	const	u_char	AIT_QTableEconomy[]={
 14, 14, 16, 18, 20, 20, 20, 24,
 16, 18, 24, 24, 28, 28, 28, 32,
 20, 24, 24, 28, 32, 32, 36, 36,
 30, 34, 44, 50, 50, 54, 56, 56,
 48, 52, 56, 60, 66, 66, 70, 70,
 52, 56, 80, 84, 84, 84, 84, 90,
 60, 72, 80, 92, 96, 96, 92, 90,
 64, 76, 84, 96, 92, 90, 80, 88,
                 
 16, 20, 20, 24, 36, 44, 56, 72,
 32, 56, 58, 60, 68, 76, 76, 76,
 40, 60, 76, 78, 80, 88, 96, 96,
 48, 96, 96, 96, 96, 96, 96, 96,
 60, 96, 96, 104, 104, 104, 116, 116,
 72, 96, 96, 104, 104, 116, 116, 116,
 76, 96, 96, 104, 104, 120, 116, 100,
 76, 96, 104, 104, 116, 120, 116, 100
};

#define QTABLE_WORST_INC (80)
__align(2)static	const	u_char	AIT_QTableWorst[]={
 14 + QTABLE_WORST_INC, 14 + QTABLE_WORST_INC, 16 + QTABLE_WORST_INC, 18 + QTABLE_WORST_INC, 20 + QTABLE_WORST_INC, 20 + QTABLE_WORST_INC, 20 + QTABLE_WORST_INC, 24 + QTABLE_WORST_INC,
 16 + QTABLE_WORST_INC, 18 + QTABLE_WORST_INC, 24 + QTABLE_WORST_INC, 24 + QTABLE_WORST_INC, 28 + QTABLE_WORST_INC, 28 + QTABLE_WORST_INC, 28 + QTABLE_WORST_INC, 32 + QTABLE_WORST_INC,
 20 + QTABLE_WORST_INC, 24 + QTABLE_WORST_INC, 24 + QTABLE_WORST_INC, 28 + QTABLE_WORST_INC, 32 + QTABLE_WORST_INC, 32 + QTABLE_WORST_INC, 36 + QTABLE_WORST_INC, 36 + QTABLE_WORST_INC,
 30 + QTABLE_WORST_INC, 34 + QTABLE_WORST_INC, 44 + QTABLE_WORST_INC, 50 + QTABLE_WORST_INC, 50 + QTABLE_WORST_INC, 54 + QTABLE_WORST_INC, 56 + QTABLE_WORST_INC, 56 + QTABLE_WORST_INC,
 48 + QTABLE_WORST_INC, 52 + QTABLE_WORST_INC, 56 + QTABLE_WORST_INC, 60 + QTABLE_WORST_INC, 66 + QTABLE_WORST_INC, 66 + QTABLE_WORST_INC, 70 + QTABLE_WORST_INC, 70 + QTABLE_WORST_INC,
 52 + QTABLE_WORST_INC, 56 + QTABLE_WORST_INC, 80 + QTABLE_WORST_INC, 84 + QTABLE_WORST_INC, 84 + QTABLE_WORST_INC, 84 + QTABLE_WORST_INC, 84 + QTABLE_WORST_INC, 90 + QTABLE_WORST_INC,
 60 + QTABLE_WORST_INC, 72 + QTABLE_WORST_INC, 80 + QTABLE_WORST_INC, 92 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 92 + QTABLE_WORST_INC, 90 + QTABLE_WORST_INC,
 64 + QTABLE_WORST_INC, 76 + QTABLE_WORST_INC, 84 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 92 + QTABLE_WORST_INC, 90 + QTABLE_WORST_INC, 80 + QTABLE_WORST_INC, 88 + QTABLE_WORST_INC,

 16 + QTABLE_WORST_INC, 20 + QTABLE_WORST_INC, 20 + QTABLE_WORST_INC, 24 + QTABLE_WORST_INC, 36 + QTABLE_WORST_INC, 44 + QTABLE_WORST_INC, 56 + QTABLE_WORST_INC, 72 + QTABLE_WORST_INC,
 32 + QTABLE_WORST_INC, 56 + QTABLE_WORST_INC, 58 + QTABLE_WORST_INC, 60 + QTABLE_WORST_INC, 68 + QTABLE_WORST_INC, 76 + QTABLE_WORST_INC, 76 + QTABLE_WORST_INC, 76 + QTABLE_WORST_INC,
 40 + QTABLE_WORST_INC, 60 + QTABLE_WORST_INC, 76 + QTABLE_WORST_INC, 78 + QTABLE_WORST_INC, 80 + QTABLE_WORST_INC, 88 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC,
 48 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC,
 60 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 104 + QTABLE_WORST_INC, 104 + QTABLE_WORST_INC, 104 + QTABLE_WORST_INC, 116 + QTABLE_WORST_INC, 116 + QTABLE_WORST_INC,
 72 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 104 + QTABLE_WORST_INC, 104 + QTABLE_WORST_INC, 116 + QTABLE_WORST_INC, 116 + QTABLE_WORST_INC, 116 + QTABLE_WORST_INC,
 76 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 104 + QTABLE_WORST_INC, 104 + QTABLE_WORST_INC, 120 + QTABLE_WORST_INC, 116 + QTABLE_WORST_INC, 100 + QTABLE_WORST_INC,
 76 + QTABLE_WORST_INC, 96 + QTABLE_WORST_INC, 104 + QTABLE_WORST_INC, 104 + QTABLE_WORST_INC, 116 + QTABLE_WORST_INC, 120 + QTABLE_WORST_INC, 116 + QTABLE_WORST_INC, 100 + QTABLE_WORST_INC
};
#endif

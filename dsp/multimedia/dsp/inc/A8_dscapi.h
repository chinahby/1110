/**
 * @file A8_dscapi.h
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#include "a8_common.h"
#include "ait_interface.h"
#if !defined (AIT_DSCAPI_H)
#define AIT_DSCAPI_H


typedef struct
{
	u_short frontx0;
	u_short frontx1;
	u_short fronty0;
	u_short fronty1;
	u_short backx0;
	u_short backx1;
	u_short backy0;
	u_short backy1;
	u_short control;
	u_short m;
	u_short n;
	
} A8ScaleDT;

#if defined (AIT701_DEMO_BOARD)
//#define EXIFSUPPORT
#endif

#define	A8_JPEG_DECODE_MAX_WIDTH			(1280) //	(2048)
#define JpegDecodeWidthToDownSample			(640)  // (800)

#define	FORMAT_YUV_420	0
#define	FORMAT_YUV_422	1

#define	FILE_FORMAT_OTHERS		0
#define	FILE_FORMAT_3GP			1
#define	FILE_FORMAT_AVI			2

#define A8_JPEG_FORMAT_YUV411			(0x01)
#define A8_JPEG_FORMAT_YUV420			(0x02)
#define A8_JPEG_FORMAT_YUV422			(0x03)
#define A8_JPEG_FORMAT_YUV444			(0x04)
#define A8_JPEG_DECODE_SUCCESS		(A8_JPEG_FORMAT_YUV444+1)		/* Checking decode success by verifying the return value below this one */

#define A8_JPEG_FIFO_DECODE_ROW_BUF_ADDR			(0x7E00) //double buffer (2048x16x2)x2 +512
#define A8_JPEG_FIFO_DECODE_COMP_BUF_START_ADDR	(0x0000)
#define A8_JPEG_FIFO_DECODE_COMP_BUF_END_ADDR		(0x7DF0)
#define A8_JPEG_FIFO_DECODE_FIFO_BUF_SIZE			((A8_JPEG_FIFO_DECODE_COMP_BUF_END_ADDR - A8_JPEG_FIFO_DECODE_COMP_BUF_START_ADDR)>>3)

#if 1//defined(LCD_SIZE_176x220) || defined(LCD_SIZE_240x320)
#define A8_JPEG_ENCODE_COMP_BUF_START_ADDR	(0x0000)
#define A8_JPEG_ENCODE_COMP_BUF_END_ADDR	(0xEFFF)
#define A8_JPEG_ENCODE_ROW_BUF_ADDR			(0xf000)

#define A8_JPEG_DECODE_COMP_BUF_START_ADDR	(0x141c0)//(0x12e80)//(0x0000C600)
#define A8_JPEG_DECODE_COMP_BUF_END_ADDR	(0x161ff)//(0x000115F0)
#define A8_JPEG_DECODE_ROW_BUF_ADDR			(0x17800-0x1600)//(0x00011600)
#define A8_JPEG_DECODE_FIFO_BUF_SIZE			((A8_JPEG_DECODE_COMP_BUF_END_ADDR - A8_JPEG_DECODE_COMP_BUF_START_ADDR)>>3)

#define A8_JPEG_DECODE_COMP_BUF_START_ADDR_1	(0x00014200)//(0x00013E00)
#define A8_JPEG_DECODE_COMP_BUF_END_ADDR_1		(0x000161FF)
#define A8_JPEG_DECODE_ROW_BUF_ADDR_1			(A8_JPEG_DECODE_COMP_BUF_END_ADDR_1+1)
#define A8_JPEG_DECODE_FIFO_BUF_SIZE_1			((A8_JPEG_DECODE_COMP_BUF_END_ADDR_1 - A8_JPEG_DECODE_COMP_BUF_START_ADDR_1)>>3)

#define A8_JPEG_DECODE_COMP_BUF_START_ADDR_2	(0x15180)//(0x00013E00)
#define A8_JPEG_DECODE_COMP_BUF_END_ADDR_2		(0x155ff)
#define A8_JPEG_DECODE_ROW_BUF_ADDR_2			(A8_JPEG_DECODE_COMP_BUF_END_ADDR_2+1)
#define A8_JPEG_DECODE_FIFO_BUF_SIZE_2			((A8_JPEG_DECODE_COMP_BUF_END_ADDR_2 - A8_JPEG_DECODE_COMP_BUF_START_ADDR_2)>>3)

#define A8_AVI_ENCODE_COMP_BUF_START_ADDR		(0x141c0)//(0x00013E00)
#define A8_AVI_ENCODE_COMP_BUF_END_ADDR		(0x000161FF)
#define A8_AVI_ENCODE_ROW_BUF_ADDR				(A8_AVI_ENCODE_COMP_BUF_END_ADDR+1)
#elif defined(LCD_SIZE_128x128) || defined(LCD_SIZE_128x160)
#define A8_JPEG_ENCODE_COMP_BUF_START_ADDR		(0x0000) //(0x12E80)//	
#define A8_JPEG_ENCODE_COMP_BUF_END_ADDR			(0xEFFF) //for TechnoB (0x1A980)//
#define A8_JPEG_ENCODE_ROW_BUF_ADDR						(0xf000) ///for TechnoB (0x1B600) //( size = 16*1600*2 + 512 )

#define A8_JPEG_DECODE_COMP_BUF_START_ADDR	(0x0000)//(0x12E80) //176x220x2	(0x0A000)
#define A8_JPEG_DECODE_COMP_BUF_END_ADDR		(0x7DF0) //(0x0EDF0)
#define A8_JPEG_DECODE_ROW_BUF_ADDR					(0x7E00)// buffer size 850x16x2            (0x0EE00) for VGA ( size = 16*640*2 + 512 ) 
#define A8_JPEG_DECODE_FIFO_BUF_SIZE					((A8_JPEG_DECODE_COMP_BUF_END_ADDR - A8_JPEG_DECODE_COMP_BUF_START_ADDR)>>3)

#define A8_JPEG_DECODE_COMP_BUF_START_ADDR_1	(0xA000) //PIP buffer 176x220x2	
#define A8_JPEG_DECODE_COMP_BUF_END_ADDR_1		(0xEDF0)//(0x159F0) //(0x0EDF0)
#define A8_JPEG_DECODE_ROW_BUF_ADDR_1					(0xEE00)//double  buffer size (850x16x2)x2 +512            (0x0EE00) for VGA ( size = 16*640*2 + 512 ) 
#define A8_JPEG_DECODE_FIFO_BUF_SIZE_1					((A8_JPEG_DECODE_COMP_BUF_END_ADDR_1 - A8_JPEG_DECODE_COMP_BUF_START_ADDR_1)>>3)

#define A8_AVI_ENCODE_COMP_BUF_START_ADDR	(0x00013E00)
#define A8_AVI_ENCODE_COMP_BUF_END_ADDR		(0x000161FF)
#define A8_AVI_ENCODE_ROW_BUF_ADDR					(A8_AVI_ENCODE_COMP_BUF_END_ADDR+1)

#endif

#define A8_JPEG_DECODE_ERR_OVER_WIDTH				(0x0011)		// jpeg width over the limit
#define A8_JPEG_DECODE_ERR_WIN_INCORRECT		(0x0012)		// incorrect window setting
#define A8_JPEG_DECODE_ERR_PICT_TOO_SMALL		(0x0013)		// jpeg picture size too small to fit the window
#define A8_JPEG_DECODE_ERR_PICT_TOO_LARGE		(0x0014)		// jpeg picture size too large to fit the window
#define A8_JPEG_DECODE_ERR_UNKNOWN_FORMAT	(0x0015)		// unknown output image format
#define A8_JPEG_DECODE_ERR_TIMEOUT						(0x0020)		// unknown output image format

#define A8_AVI_DECODE_COMP_BUF_START_ADDR	(0x00013C00)
#define A8_AVI_DECODE_COMP_BUF_END_ADDR		(0x00015FFF)
#define A8_AVI_DECODE_ROW_BUF_ADDR					(A8_AVI_DECODE_COMP_BUF_END_ADDR+1)
#define A8_AVI_DECODE_FIFO_BUF_SIZE					((A8_AVI_DECODE_COMP_BUF_END_ADDR - A8_AVI_DECODE_COMP_BUF_START_ADDR)>>3)

#define A8_GET_JPEG_TIMEOUT					(0x100000)
#define A8_DECODE_JPEG_TIMEOUT				(0x100000)

#define A8_JPEG_FORMAT_NOT_3_COLOR		(0xF1)
#define A8_JPEG_FORMAT_NOT_BASELINE		(0xF2)
#define A8_JPEG_FORMAT_ERROR			(0xF3)

s_short A8L_SetCamMode( u_short CamMode );
s_short RestorePreviewSetting(void);
s_short A8L_TakeContinueJpegPicture( s_short PictSize, s_short ZoomRate );
s_short A8L_TakeJpegPicture( s_short PictSize, s_short ZoomRate , u_char Re_Flag);
s_short A8L_EncodeRGBToJpegByFW( u_short res );
s_short A8L_TakeStickJpegPicture( u_short PictSize, u_short ZoomRate );
s_int A8L_GetJpegPictureSize(void);
s_short A8L_IsJpegEncodeDone(void);
s_int GetJpegPicture( u_short *JpegBufferAddr );
s_short A8L_GetJpegPictureWithLimit( u_short *JpegBufferAddr, s_int Limit, u_int *pJpegSize );
s_short A8L_GetJpegPictureWithLimit_RAW( u_short *JpegBufferAddr, s_int Limit, s_short resolution, u_int *pJpegSize );
s_short A8L_GetJpegPictureBufWithLimit( u_short *JpegBufferAddr, u_int A8JpegBufAddr, s_int Limit, u_int *pJpegSize );
u_short A8L_MainLCDPreviewActive(void);
u_short A8L_SubLCDPreviewActive(void);
s_short A8L_StartLCDPreview(void);
s_short A8L_StopLCDPreview(void);
s_short A8L_SetCdspPreviewResolution( s_short TgRawW, s_short TgRawH, s_short PrevW, s_short PrevH, s_short ScaleN, s_short ScaleM, u_short ZoomRate );
s_short A8L_ConfigPreviewZoomParam( u_short ScaleRatio, u_short ScaleStep );
s_short A8L_SetPreviewZoom( u_short TgtZoomRate );
s_short A8L_GetJpegInfo( u_short *ImgAddr, s_int Length, s_short *Width, s_short *Height, s_short *Format, u_short *Quality );
s_short A8L_SetPreviewWindowAttribute( s_int Buffer0BaseAddr, s_int Buffer1BaseAddr, u_short width, u_short height );
s_short A8L_SetPreviewWindowToDisplay( u_short startx, u_short starty, u_short displayWidth, u_short displayHeight, 
											u_short displayOffsetx, u_short displayOffsety, u_short rotateType);
s_short A8L_SetPreviewPath( s_short PrevPath );
s_short A8L_SetCdspPreviewResolutionL( s_short TgRawW, s_short TgRawH, s_short ScaleN, s_short ScaleM, u_short ZoomRate );

s_short A8L_SetJpegQuality( s_short JpegQuality );
s_short A8L_DownloadA8Firmware( u_char *FWBuffer, s_short Length );
s_short A8L_InitialLCD(void);
s_short A8L_SetPllFreq( s_short X );
s_short A8L_CheckLcdBusy(void);
s_short A8L_UpdateLCD(void);
s_short A8L_CheckReadyForA8ByPass(void);
s_short A8L_CheckReadyForA8Resume(void);
s_short A8L_CheckReadyForA8Command(void);
s_short A8L_CheckReadyForPreview(u_char cStart);
u_short A8L_GetA8FirmwareVersion(void);
u_short A8L_GetA8ChipVersion(void);
s_short A8L_SetZoomRate( u_short ZoomRate );

s_short A8L_DecodeJpegBase( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, A8S_CameraParam A8S_Cameraparam);
s_short A8L_DecodeJpegBaseHWScale( u_short *JpegBufferPtr, u_short *pRgbBuf, s_int Length, s_int DestAddr ,u_char mode, u_short SrcX, u_short SrcY, u_short SrcWidth, u_short SrcHeight, u_short DstWidth, u_short DstHeight, u_char MyFave_mode);
s_short A8L_DecodeJpegPicture( u_short *JpegBufferPtr, s_int Length, s_int DestAddr,A8S_CameraParam A8S_Cameraparam);
s_short A8L_DecodeJpegToYuv( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, A8S_CameraParam A8S_Cameraparam);
s_short A8L_PlaybackJpegPicture( u_short *ImgAddr, s_int Length, s_short WinID );
s_short A8L_DecodeJpegDirectShow( u_short *JpegBufferPtr, s_int Length );

s_short A8L_DecodeJpegToYuvViaFIFO( u_short *JpegBufferPtr, s_int JpegLength, u_short *DestAddr );
s_short A8L_DecodeJpegToRGBViaFIFO( u_short *JpegBufferPtr, s_int JpegLength, u_short *DestAddr );


s_short A8L_ConvertImageFIFOToBuf(u_short *IMGPtr, u_short ScalN, u_short ScalM, 
									u_short SrcW, u_short SrcH, u_char SrcFormat, 
									u_short TarW, u_short TarH, u_char TarFormat, u_int BufADDR);
//u_int A8L_TranslateImageToJpegBase( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr, u_char *makerName, u_char *modelName, u_char *DateTimeInfo); //0124
u_int A8L_TranslateImageToJpegBase( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr); 
u_int A8L_TranslateRGBToJpegPicture( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr );
u_int A8L_TranslateYuvToJpegPicture( u_short *YuvPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr );
u_int A8L_EncodeRGBToJpegPicture( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferPtr );
u_int A8L_EncodeYuvToJpegPicture( u_short *YuvPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferPtr );

//u_int A8L_TranslateImageBufToJpegBase( u_int YuvAddr, A8S_CameraParam A8S_Cameraparam,u_short * JpegBufferAddr, u_char *makerName, u_char *modelName, u_char *DateTimeInfo);//0124
//u_int A8L_TranslateRGBBufToJpegPicture( u_int RgbAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr, u_char *makerName, u_char *modelName, u_char *DateTimeInfo );
u_int A8L_TranslateImageBufToJpegBase( u_int YuvAddr, A8S_CameraParam A8S_Cameraparam,u_short * JpegBufferAddr);
u_int A8L_TranslateRGBBufToJpegPicture( u_int RgbAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr);
u_int A8L_TranslateYuvBufToJpegPicture( u_int YuvAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr );
u_int A8L_EncodeRGBBufToJpegPicture( u_int RgbAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferPtr );
u_int A8L_EncodeYuvBufToJpegPicture( u_int YuvAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferPtr );

s_short A8L_DecodeRotateJpegBufToFIFO( u_int JpegBufferAddr, s_int JpegLength, u_short *DestAddr, s_short YuvWidth, s_short YuvHeight );
s_short A8L_PauseLCDRefresh(void);
s_short A8L_ResumeLCDRefresh(void);
s_short A8L_PauseLCDPreview(void);
s_short A8L_ResumeLCDPreview(void);
s_short A8L_CheckLCDPreview(void);
/* YChuls 080423 */
s_short A8L_DecodeJpegViaFIFOBase( u_short *JpegBufferPtr, s_int JpegLength, u_short *DestAddr, u_short OutFormat );
/* YChuls 080423 */

/* DSC or TV source */
#if AIT_VIDEO_PHONE_SUPPORT
extern s_short A8L_SetTVPreviewMode(unsigned short bEnable, u_int addr_pip0, u_int addr_pip1);
#else
extern s_short A8L_SetTVPreviewMode(unsigned short bEnable);
#endif

#ifdef EXIFSUPPORT
s_int GetJpegPictureWithEXIF( u_short *JpegBufferAddr, u_char *makerName, u_char *modelName, u_char *DateTimeInfo);
void SetEXIFMakeModelInfo(u_char *makerName, u_char *modelName);
void SetEXIFZoomInfo(void);
void SetEXIFImageResolutionInfo(void);
void SetEXIFDateTimeInfo(u_char *DateTimeInfo);
void SetEXIFExposureInfo(void);
void SetEXIFExposureBiasInfo(void);
void Remove_EXIFHeader_BTprint(u_short *JpegBufferAddr, u_int *lv_CaptureSize );
#endif

extern u_short A8L_GetJpegPictureForVideo( u_short *JpegBufferAddr, u_int Limit, u_int *pJpegSize );
#endif

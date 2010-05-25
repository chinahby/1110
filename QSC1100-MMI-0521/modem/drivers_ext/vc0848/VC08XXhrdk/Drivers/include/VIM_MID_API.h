/*************************************************************************
*                                                                       
*                Copyright (C) 2005 Vimicro CO.,LTD     		 
*
* FILE NAME                                  VERSION                
*                                                                       
*  VIM_MID_API.h		   	1.1                 
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*     5X 's function aip head file                          
*
*	Version		Author			Date		Description		
*  ---------------------------------------------------------------						 
*   0.1			angela		2005-11-15	The first version. 
*   0.2			angela		2006-6-20	updata for528
*   1.1			angela		2006-12-31	update for 548 1.1 version
*  ---------------------------------------------------------------
*                                                                       
*************************************************************************/
/****************************************************************************
This source code has been made available to you by VIMICRO on an
AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
copyrights to use it in any way he or she deems fit, including copying it,
modifying it, compiling it, and redistributing it either with or without
modifications. Any person who transfers this source code or any derivative
work must include the VIMICRO copyright notice and this paragraph in
the transferred software.
****************************************************************************/
#include "VIM_HIGH_API.h"

#ifndef _VIM_5XRDK_MAPI_H_
#define _VIM_5XRDK_MAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************
	Desc: set sensor mirror and flip callback 
	Para: BIT0 -- mirror
		  BIT1 -- flip
*******************************************************************/
typedef enum _VIM_MAPI_SENSOR_MIMODE
{
	VIM_SENSOR_NORMAL=0x03,
	VIM_SENSOR_MIRROR=1,
	VIM_SENSOR_FLIP=2,
	VIM_SENSOR_ROTATION=0,
	VIM_SENSOR__MIMODE_UNKOWN=0x7FFFFFFF
}VIM_MAPI_SENSOR_MIMODE;
typedef struct _VIM_MAPI_ZOOM_PARAMETER
{
	UINT8 CurrentZoomStep; 
	TSize InitAMemSize; 
	TSize InitSourceSize;
	UINT16 ZoomW;
	UINT16 ZoomH;
}VIM_MAPI_ZOOM_PARAMETER,*PVIM_MAPI_PARAMETER;
typedef struct tag_VIM_PREVIEW
{
	TSize Size;
	TPoint Point;
	VIM_HAPI_RESOLUTION bOutputIndex;
	VIM_HAPI_PREVIEW_MODE Mode;
	VIM_MAPI_SENSOR_MIMODE  SensorMirror; 
	VIM_HAPI_SENSORMODE  SensorMode; 
	VIM_HAPI_SPECIAL_EFFECT   SpecialEffect;
	VIM_MAPI_ZOOM_PARAMETER ZoomPara;
	UINT8 brightness;
	UINT8 contrast;
}VIM_PREVIEW,*PVIM_PREVIEW;
 typedef enum _VIM_VIDEO_MODE
{
	VIM_VIDEO_STARTCAPTURE=0,
	VIM_VIDEO_STARTDISPLAYE=1,	
	VIM_VIDEO_PAUSE=2,
	VIM_VIDEO_STOP=3,
	VIM_VIDEO_STARTMJPEG=4,
	VIM_VIDEO_WANTSTOP=5,
	VIM_VIDEO_DISPLAYEING=6,
	VIM_VIDEO_UNKOWN_MODE=0x7FFFFFFF
}VIM_VIDEO_MODE;



 typedef enum _VIM_CAPTURE_MODE
{
	VIM_CAPTUREDONE=0,
	VIM_CAPTURESTILL,
	VIM_Encode,
	VIM_CAPTURETHUMB,	
	VIM_MULTISHOT,		
	VIM_CAPTUREVIDEO,	
	VIM_CAPTUREAVI,
	VIM_CAPTUREUNKNOWN,
	VIM_CAPTURECAPTURE,
	VIM_CAPMULTIFRAME,
	CAPTURETHUMB_GETYUV_MODE,
	VIM_CAPTUREMPE4,
	VIM_CAPTURE_MODE_UNKOWN=0x7FFFFFFF
}VIM_CAPTURE_MODE;
 
typedef void (*VIM_CallBack)(UINT8 Status, UINT32 Byte_Length);
typedef UINT8* (*VIM_AVI_AudioCalLBack)(UINT32 *Byte_Length);
typedef struct tag_VIM_CAPTURE
{
	VIM_CAPTURE_MODE Mode;
	VIM_RESULT CaptureError;
	TSize Size;
	TSize ThumbSize;
	UINT32 CapFileLength;
	HUGE UINT8 *BufPoint;
	HUGE UINT8 *MallocPr;
	HUGE UINT8 *IndexPoint;
	HUGE UINT8 *ThumbBuf;
	UINT32 BufLength;
	UINT32 MarbJbufLength;
	UINT32 SensorOutputChange;
	UINT32 DownSensorClock;
	UINT32 SensorClock;
	UINT32 SizeChange;
	UINT8 QualityMode;
	UINT8 RecaptureTimes;
	UINT8 ErrorImageRecaptureTimes;
	VIM_CallBack CapCallback;
	VIM_AVI_AudioCalLBack pAudioCall;
	VIM_HAPI_SAVE_MODE SaveMode;
	HUGE void* pFile_Nameptr;
	UINT8 ReCapSize;
}VIM_CAPTURE,*PVIM_CAPTURE;

typedef struct tag_VIM_MPEG4
{
	VIM_MPEG4_RCParam RC;
	UINT32 m4Qp_g;
	UINT32 m4Coded_g;
#if (BOARD_TYPE==VC0820_FPGA)
	UINT32 m_Enc_OutBitStreamOrder;
	UINT32 m_Enc_SrcBitStreamOrder;
	UINT32 m4Rc_MultiFrames;	  
	UINT32 m_Enc_Stuff;
	UINT32 m_Enc_ResyncMarker_Disable;
	UINT32 m_Enc_Short_HeaderSurpport;
	UINT32 m_Enc_QuantType;
	UINT32 m_Enc_4mv_Disable;
	UINT32 m_Enc_Half_Pixsel_Estimation_Enable;
	UINT32 m_Enc_Rounding_Enable;
       UINT32 m_Enc_intra_dc_vlc;
	UINT32 m_Enc_HeaderExt;
	UINT32 m_Enc_HeaderExtMaxPacketWordCnt;

	  
	//UINT32 m_Enc_Out_BufferPtr[VIM_USER_MPEG4_ENC_OUTBUF_NUM];
	UINT32 m_Enc_Out_Bitrate_Ctrl_En;	 
       UINT32 m_Enc_Out_CurFrameLen;	
	UINT32 m_Enc_I_Frame_Estimat_Size;	
#else
	UINT32 m4Ticks;
	UINT32 BitRate;
	VIM_MPEG4_VOPTYPE m4Type;
#endif
	HUGE UINT8 * FramePoint;
	VIM_MPEG4INFO DisInfo;
	VIM_VIDEO_OUTINFO cur_DisInfo;
	UINT8 ShouldDec;
}VIM_MPEG4,*PVIM_MPEG4;

typedef enum _VIM_DECODE_STATUS
{
	VIM_DECODE_DONE=0,
	VIM_DECODE_BUFERROR,
	VIM_DECODE_SHOULDREWRITE,
	VIM_DECODE_READFILE,//only for vim_rom_save mode.  add by jiangkeqiang
	VIM_DECODE_ING,
	VIM_DECODE_STATUS_UNKOWN=0x7FFFFFFF
}VIM_DECODE_STATUS;


typedef enum _VIM_OUTPUTTYPE_
{
	VIM_OUTPUTLCD=0x00,
	VIM_OUTPUTTV = 0x01,
	VIM_OUTP_UNKOWN=0x7FFFFFFF
}VIM_OUTPUTTYPE;

typedef struct tag_VIM_DISPLAY
{
	HUGE UINT8 *DecPoint;		//decode target buffer point
	UINT32 DecLength;	//jpeg data buffer length
	UINT16 DecodeHeight;		//
	HUGE UINT8 *BufPoint;		//jpeg data buffer point
	HUGE UINT8 *MallocPr;
	UINT32 BufLength;	//jpeg data buffer length
	UINT32 BufOffset;	//jpeg data buffer length
	void* pFileHandle;	//jpeg file handle, use in ROM
	UINT32 FileLength;	//jpeg file length, use in ROM
	TPoint LcdWantPt;
	TSize LcdWantSize;
	VIM_DECODE_STATUS ReadOk;
	VIM_HAPI_SAVE_MODE SaveMode;
#if (BOARD_TYPE==VC0820_FPGA)
	UINT32 LCDCA0done;
	UINT32 RMFrameTimes;
	VIM_HAPI_JPEGCallBack JpegCallBack;
#endif
	VIM_OUTPUTTYPE outputtype;
}VIM_DISPLAY,*PVIM_DISPLAY;

#if (BOARD_TYPE==VC0820_FPGA)
typedef struct tag_VIM_ENCODE
{
	HUGE UINT8 *YUVBufPoint;
	HUGE UINT8 *BufPoint;
	UINT32 BufLength;
	UINT8 QualityMode;
	VIM_JPEG_YUVMODE YUVmode;
	TSize JpegSize;
	VIM_CallBack CapCallback;
}VIM_ENCODE,PVIM_ENCODE;
typedef struct tag_VIM_DECODE
{
	HUGE UINT8 *DecodePoint;
	UINT32 DecodeBufLength;
	UINT32 DecodeLine;
	UINT32 DecFileLength;
	UINT32 WriteBlock;
	VIM_JPEG_YUVMODE YUVmode;
	VIM_DECODE_MODE DecodeMode;
	TSize TargetSize;
	TSize JpegSize;
	VIM_HAPI_DECOCallBack DecCallback;
}VIM_DECODE,PVIM_DECODE;
#endif
typedef struct tag_VIM_LCD
{
	TSize Size;
	VIM_HAPI_LCDWORKMODE WorkMode;
	VIM_HAPI_COLORDEP_MODE ColorDep;
	VIM_DISP_ROTATEMODE ARotationMode;
	VIM_DISP_ROTATEMODE BRotationMode;
	VIM_HAPI_LCDPANEL NowPanel;
}VIM_LCD,*PVIM_LCD;
typedef struct tag_VIM_MARB
{
	VIM_MARB_Map MapList;
	VIM_DISP_BUFFERMODER ALayerMode;
	VIM_MARB_WORKMODE WorkMode;
#if(BOARD_TYPE==VC0820_FPGA)	
	VIM_MARB_Fifo VFifo[2];
	UINT32 vdec_bitstream_buf;
	UINT32 vdec_bitstream_len;
#endif
}VIM_MARB,*PVIM_MARB;


typedef struct tag_VIM_VIDEO
{
	VIM_VIDEO_MODE CapMode;
	VIM_VIDEO_MODE DisMode;
#if(BOARD_TYPE==VC0820_FPGA)	
      VIM_HAPI_MP4_ENC_CallBack Mp4EncCallBack; 	
#endif      
	VIM_HAPI_MPEGDecCallBack DecCallback;
	VIM_HAPI_MPEGCallBack Callback;
	UINT32	CapVideoFrameRate;		//VIM_WORK_MODE
	UINT32	DisVideoFrameRate;		//VIM_WORK_MODE
	UINT32	MaxFrame;		//VIM_WORK_MODE
	UINT32	NowFrame;		//VIM_WORK_MODE	
	UINT32	CapVideoLength;		//VIM_WORK_MODE
	UINT32	DisVideoLength;		//VIM_WORK_MODE
}VIM_VIDEO,*PVIM_VIDEO;

typedef struct tag_VIM_AEAF
{
	UINT8 Baeafen;//用于是否在preview时，同时打开AE,AF. 0 no,1 yes
	UINT8 Baeafsel;//用于在preview时，AE,AF互相切换。
	UINT8 Bafover;//用于单独capture时，调节AF时需要判断AF是否结束
	UINT8 rightstep;//用于记录调整完AF之后重新设置AF step
	UINT8 count;	
}VIM_AEAF;

typedef struct tag_VIM_STATUS{
	PTSnrInfo	pSensorInfo;
	PTPanelInfo   pDriverIcInfo;
	PVIM_USER_INITINFO	pUserInfo;
	VIM_PANEINIT MainPanel;
	VIM_PANEINIT SubPanel;

	UINT8	ChipWorkMode;		//VIM_WORK_MODE

	VIM_PREVIEW	PreviewStatus;
	VIM_CAPTURE CaptureStatus;
	VIM_MPEG4	Mpeg4Status;
	VIM_DISPLAY DisplayStatus;
#if (BOARD_TYPE==VC0820_FPGA)
	VIM_ENCODE EncodeStatus;
	VIM_DECODE DecodeStatus;
#endif
	VIM_LCD LcdStatus;
	VIM_MARB MarbStatus;
	VIM_VIDEO	VideoStatus;
	VIM_ISP_TAEADJUST 	VIM_SIF_AeAdjust;
	VIM_AEAF aeaf;

}VIM_STATUS, *PVIM_STATUS;

extern VIM_STATUS gVc0578b_Info;
extern VIM_JPEG_Index gVc0578b_JpegInfo;
extern VIM_HIF_TISR  gVc0578b_Isr;
VIM_RESULT VIM_MAPI_TestRegister (void);
VIM_RESULT VIM_MAPI_InitHif(void);
VIM_RESULT VIM_MAPI_AutoFindSensor(void);
void VIM_MAPI_InitCapInterrupt(void);
void VIM_MAPI_ISR_OneFrameDone(void);
void VIM_MAPI_ISR_LBufRead(void);
void VIM_MAPI_ISR_Recapture(void);
void VIM_MAPI_StartDecode(void);



VIM_RESULT VIM_MAPI_SetCaptureRatio(UINT8 ratio);
void VIM_MAPI_InitIfo(void);
void VIM_MAPI_SwitchPanel(VIM_HAPI_LCDPANEL bPanel);
void VIM_MAPI_SetChipMode(VIM_MARB_WORKMODE bMode,VIM_IPP_HAVEFRAM bHaveFrame);
void VIM_MAPI_Delay_Frame(UINT8 FrameCount);

void VIM_MAPI_WriteOneFrameData(UINT32 Truelen,HUGE UINT8 *start);
void VIM_MAPI_JpegAutoDecode(UINT32 Datasize);
VIM_RESULT VIM_MAPI_DecodeOneFrame(UINT32 Truelen,HUGE UINT8 *start);
VIM_RESULT VIM_MAPI_CheckDisplaySize(UINT16 LcdOffsetX, UINT16 LcdOffsetY, UINT16 DisplayWidth, UINT16 DisplayHeight,TSize *DisplaySize);
VIM_RESULT VIM_MAPI_CheckDisplayPanSize(UINT16 JpegOffsetX, UINT16 JpegOffsetY, TSize DisplaySize,TSize *DestDisplaySize,TPoint* pt );
VIM_RESULT VIM_MAPI_CheckDecodeTargetSize(VIM_JPEG_YUVMODE bYUVmode,TSize SourceSize, TSize DisplaySize);
VIM_RESULT VIM_MAPI_SetALayerDisplaySize(TPoint mPt,TSize MemorySize, TSize DisplaySize, UINT16 LcdOffsetX, UINT16 LcdOffsetY);
VIM_RESULT VIM_MAPI_Preview_CaptureZoom(UINT8 step);
VIM_RESULT VIM_MAPI_Preview_DisplayZoom(UINT8 step);
VIM_RESULT VIM_MAPI_GetJpegInformation(HUGE UINT8 *jpegBuf,UINT32 length);
void VIM_MAPI_SetDisplayJpegMode(void);
#if (BOARD_TYPE==VC0820_FPGA)
VIM_RESULT VIM_MAPI_SetDecodeJpegMode(VIM_DECODE_MODE decodemode);
#else
VIM_RESULT VIM_MAPI_SetDecodeJpegMode(void);
#endif
VIM_RESULT VIM_MAPI_SetDisplayJpegParameter(void);

UINT32 VIM_MAPI_WriteOneBlockData(UINT32 Truelen,HUGE UINT8 *start);


VIM_RESULT VIM_MAPI_AdjustPoint(VIM_DISP_ROTATEMODE byARotateMode,VIM_DISP_ROTATEMODE byBRotateMode);
VIM_RESULT VIM_MAPI_FastPreview(void);
VIM_RESULT VIM_MAPI_GetNewPoint(VIM_DISP_ROTATEMODE byRotateMode,PTPoint OldPoint,TSize OldSize,PTPoint NewPoint);
void VIM_MAPI_EnableSyncGen(UINT8 bEnable);
void VIM_MAPI_InitDisMpeg4Interrupt(void);


UINT8 VIM_MAPI_FindUpdateRange(TSize size, TPoint Point);
void VIM_MAPI_DoAeAF(void);
void VIM_MAPI_AutoFocusISR(void);

void VIM_MAPI_ISR_JbufError(void);
void VIM_MAPI_ISR_AddressError(void);

BOOL VIM_MAPI_BMP_TESTOK(UINT8 *bmp);
TSize VIM_MAPI_CutoffIPP(TSize Size_Jiang,TSize DisplaySize,TPoint *pt1);

#if(BOARD_TYPE==VC0820_FPGA)
void VIM_MAPI_StartDecodeMJpeg(void);
void VIM_MAPI_InitDispInterrupt(void);
void VIM_MAPI_ClearDispInterrupt(void);
void VIM_MAPI_ISR_JpegDecDone(void);
void VIM_MAPI_ISR_ThreshLow(void);
void VIM_MAPI_ISR_FIFOEmpty(void);
void VIM_MAPI_DecodelbufEn(VIM_MARB_WORKMODE bMode);
void VIM_MAPI_InitDecoInterrupt(void);
UINT32 VIM_MAPI_WriteJpegDataToFIFO(HUGE UINT8 *addr,UINT32 rawdatalen,VIM_HIF_SRAMMODE Mode);
void VIM_MAPI_SendYUVData(UINT8 *psrcbuf,TSize Size,VIM_JPEG_YUVMODE YUVmode);
void VIM_MAPI_StartDecodeYUVBlock(void);
void VIM_MAPI_SetDisplayYUVMode(void);
void VIM_MAPI_ISR_MpegDecDoneDebug(void);
void VIM_MAPI_ISR_LCDCA0DoneDebug(void);
void VIM_MAPI_ISR_SIFVSYNC(void);
void VIM_MAPI_ISR_SIFFRAMEEND(void);
void VIM_MAPI_FreeLCDCBuffer(void);
void VIM_MAPI_FreeLCDCA0Buffer(void);
void VIM_MAPI_ClearCapInterrupt(void);
void VIM_MAPI_FreeCaptureVideoBuffer(void);
void VIM_MAPI_FreeDisplayVideoBuffer(void);
void VIM_MAPI_FreeCaptureJpegBuffer(void);
void VIM_MAPI_FreeDisplayJpegBuffer(void);
void VIM_MAPI_FreeLCDCA0Buffer(void);
void VIM_MAPI_FreeDecodeJpegBuffer(void);
void VIM_MAPI_FreeEncodeJpegBuffer(void);
UINT32 VIM_MAPI_Encode_JbufDone(void);
#endif


#if VIM_SENSOR_HAVE_BAYER
void VIM_MAPI_AEIsrInit(void);
#endif

VIM_RESULT VIM_MAPI_ReadyToDecode(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *jpegBuf, UINT32 length,UINT16 DisplayWidth, UINT16 DisplayHeight);
VIM_RESULT VIM_MAPI_DecodeOneFrame(UINT32 Truelen,HUGE UINT8 *start);
VIM_RESULT VIM_MAPI_GetAmodeInMpeg4(TSize SourceSize,TSize TargetSize,PTSize Size,VIM_DISP_BUFFERMODER *Amode);

#ifdef __cplusplus
}
#endif
#if (BOARD_TYPE==VC0820_FPGA)
void VIM_MAPI_Osd_Lcdc_Init_ForDemo(void);
#endif

#endif /* _RDK_HIF_H_ */

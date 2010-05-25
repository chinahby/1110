/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2005                                                     
 * ------------------------------------------------------------------------------
 * [File Name]            : VIM_HIGH_API.h
 * [Description]          : 适用于578b.和High API 相关的函数 
 * [Author]                 : WendyGuo	
 * [Date Of Creation]  : 2007-04-02 
 * [Platform]              : 平台无关
 * [Note]                    : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        		Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2006-04-02  	WendyGuo         	Created 
 *******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/

#ifndef _VIM_5XRDK_HAPI_H_
#define _VIM_5XRDK_HAPI_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifndef __INFINEON_PLATFORM__
#define __INFINEON_PLATFORM__		0
//------Infineon------//
#if __INFINEON_PLATFORM__

#define 	HUGE		huge
#define	SHUGE		shuge
#define 	_FAR		_far	

#else

#define 	HUGE
#define	SHUGE
#define	_FAR

#endif
#endif





#ifndef SINT8
#define SINT8 char
#endif

//unsigned 8-bit integer
#ifndef UINT8
#define UINT8 unsigned char
#endif

//signed 16-bit integer
#ifndef SINT16
#define SINT16 short
#endif

//unsigned 16-bit integer
#ifndef UINT16
#define UINT16 unsigned short
#endif

//signed 32-bit integer
#ifndef SINT32
#define SINT32 int
#endif

//unsigned 32-bit integer
#ifndef UINT32
#define UINT32 unsigned int
#endif

#ifndef INT32
#define INT32 int
#endif

#ifndef INT16
#define INT16 short
#endif

#ifndef BOOL
#define BOOL unsigned char
#endif

typedef enum _VIM_HAPI_WORKMODE
{
	VIM_HAPI_MODE_CAMERAON=1,
	VIM_HAPI_MODE_BYPASS,
	VIM_HAPI_MODE_PHOTOPROCESS,
	VIM_HAPI_MODE_POWERDOWN,
	VIM_HAPI_WORKMODE_RESERVED=0x7fffffff
}VIM_HAPI_WORKMODE;
 typedef enum _VIM_HAPI_LCDWORKMODE
{
	VIM_HAPI_LCDMODE_AFIRST=0,
	VIM_HAPI_LCDMODE_BLONLY,
	VIM_HAPI_LCDMODE_OVERLAY,
	VIM_HAPI_LCDMODE_BLEND,
	VIM_HAPI_LCDMODE_BACKGROUND,
	VIM_HAPI_LCDMODE_BFIRST,
	VIM_HAPI_LCDWORKMODE_ERROR = 0x7fffffff
}VIM_HAPI_LCDWORKMODE;
 
 typedef enum _VIM_HAPI_LCDPANEL
{
	VIM_HAPI_LCDPANEL_MAIN=0,
	VIM_HAPI_LCDPANEL_SUB,
	VIM_HAPI_LCDPANEL_ERROR = 0x7fffffff
}VIM_HAPI_LCDPANEL;
 
// set B layer is B0/B1/ALL
typedef enum _VIM_HAPI_LAYER_enum
{
	VIM_HAPI_B0_LAYER = 2,
	VIM_HAPI_B1_LAYER=4,
	VIM_HAPI_ALLB_LAYER=6,
	VIM_HAPI_A_LAYER=1,
	VIM_HAPI_ALL_LAYER, 
	VIM_HAPI_LAYER_ERROR = 0x7fffffff
}VIM_HAPI_LAYER;

typedef enum _VIM_HAPI_ROTATEMODE{
	VIM_HAPI_ROTATE_0,
	VIM_HAPI_ROTATE_90,
	VIM_HAPI_ROTATE_180,
	VIM_HAPI_ROTATE_270,
	VIM_HAPI_MIRROR_0,
	VIM_HAPI_MIRROR_90,
	VIM_HAPI_MIRROR_180,
	VIM_HAPI_MIRROR_270,
	VIM_HAPI_ROTATEMODE_ERROR = 0x7fffffff
}VIM_HAPI_ROTATEMODE;

typedef enum tag_VIM_HAPI_RESOLUTION
{
	VIM_HAPI_MAXPIXEL = 0,
	VIM_HAPI_HALFPIXEL,
	VIM_HAPI_HALFHALFPIXEL,
	VIM_HAPI_RESOLUTION_ERROR = 0x7fffffff
}VIM_HAPI_RESOLUTION;


typedef enum tag_VIM_HAPI_SENSORMIRROR
{
	VIM_HAPI_SENSOR_NORMAL = 0x3,
	VIM_HAPI_SENSOR_FLIP=0x2,
	VIM_HAPI_SENSOR_MIRROR,
	VIM_HAPI_SENSORMIRROR_ERROR = 0x7fffffff
}VIM_HAPI_SENSORMIRROR;

typedef enum tag_VIM_HAPI_SENSORMODE
{
	
	VIM_HAPI_SENSOR_50HZ=0x1,
	VIM_HAPI_SENSOR_60HZ=2,
	VIM_HAPI_SENSOR_OUT = 0x3,
	VIM_HAPI_SENSOR_NIGHT,
	VIM_HAPI_SENSORMODE_ERROR = 0x7fffffff
}VIM_HAPI_SENSORMODE;

 typedef enum  _VIM_HAPI_PREVIEW_MODE
{
	VIM_HAPI_PREVIEW_OFF=0,
	VIM_HAPI_PREVIEW_ON,	
	VIM_HAPI_PREVIEW_FRAMEON,
	VIM_HAPI_MP4_PREVIEW_ON, 	// added by lizg 
	VIM_HAPI_PREVIEW_MODE_ERROR = 0x7fffffff
} VIM_HAPI_PREVIEW_MODE;


 typedef enum _VIM_HAPI_COLORDEP_MODE
{
	VIM_HAPI_COLORDEP_12BIT=4,
	VIM_HAPI_COLORDEP_A16BIT,
	VIM_HAPI_COLORDEP_16BIT,
	VIM_HAPI_COLORDEP_18BIT,
	VIM_HAPI_COLORDEP_24BIT,
	VIM_HAPI_COLORDEP_A24BIT,
	VIM_HAPI_COLORDEP_MODE_ERROR = 0x7fffffff
 }VIM_HAPI_COLORDEP_MODE;

 typedef enum _VIM_HAPI_CPTURE_QUALITY
{
	VIM_HAPI_CPTURE_RATIO_MODE=0,
	VIM_HAPI_CPTURE_QUALITY_LOW=20,
	VIM_HAPI_CPTURE_QUALITY_MID=14,
	VIM_HAPI_CPTURE_QUALITY_HIGH=10,
	VIM_HAPI_CPTURE_QUALITY_PERFECT,
	VIM_HAPI_CPTURE_QUALITY_ERROR = 0x7fffffff
}VIM_HAPI_CPTURE_QUALITY;
 
  typedef enum  _VIM_HAPI_SAVE_MODE
{
	VIM_HAPI_RAM_SAVE,
	VIM_HAPI_ROM_SAVE,
	VIM_HAPI_SAVE_MODE_ERROR = 0x7fffffff
}VIM_HAPI_SAVE_MODE;
  
   typedef enum  _VIM_HAPI_DISPLAY_ZOOM
{
	VIM_HAPI_DISPLAY_NORMAL=20,//VIM_USER_DISPLAY_ZOOMDIVISOR,
	VIM_HAPI_DISPLAY_25PER=17,//(VIM_USER_DISPLAY_ZOOMDIVISOR/2)+(VIM_USER_DISPLAY_ZOOMDIVISOR*3/8),
	VIM_HAPI_DISPLAY_50PER=15,//(VIM_USER_DISPLAY_ZOOMDIVISOR/2)+(VIM_USER_DISPLAY_ZOOMDIVISOR/4),
	VIM_HAPI_DISPLAY_100PER=10,//(VIM_USER_DISPLAY_ZOOMDIVISOR/2),
	VIM_HAPI_DISPLAY_200PER ,//(VIM_USER_DISPLAY_ZOOMDIVISOR/4)
	VIM_HAPI_DISPLAY_ZOOM_ERROR= 0x7fffffff
}VIM_HAPI_DISPLAY_ZOOM;
   
 typedef enum  _VIM_HAPI_CHANGE_MODE
{
	VIM_HAPI_CHANGE_NORMAL=0,
	VIM_HAPI_CHANGE_ADD,
	VIM_HAPI_CHANGE_DEC,
	VIM_HAPI_SET_STEP,
	VIM_HAPI_CHANGE_MODE_ERROR = 0x7fffffff
}VIM_HAPI_CHANGE_MODE;
 
 typedef enum _VIM_HAPI_CONTRAST_STEP
{
	VIM_HAPI_CONTRAST_STEP_ONE=1,
	VIM_HAPI_CONTRAST_STEP_TWO,
	VIM_HAPI_CONTRAST_STEP_THREE,
	VIM_HAPI_CONTRAST_STEP_MAX,
	VIM_HAPI_CONTRAST_STEP_ERROR = 0x7fffffff
}VIM_HAPI_CONTRAST_STEP;

	/*	VIM_IPP_EFFECT_NORMAL = 0x8,
	VIM_IPP_EFFECT_SEPHIA = 0x0,
	VIM_IPP_EFFECT_SPECIALCOLOR = 0x1,
	VIM_IPP_EFFECT_NEGATIVE = 0x2,
	VIM_IPP_EFFECT_SKETCH = 0x3,
	VIM_IPP_EFFECT_MONO=0X4,
	VIM_IPP_EFFECT_FOUR_COLOR=0X5,
	VIM_IPP_EFFECT_TV = 0x6,
	VIM_IPP_EFFECT_GRID=0X7*/
typedef enum  _VIM_HAPI_SPECIAL_EFFECT
{    
	VIM_HAPI_SPECIAL_NORMAL=8,
	VIM_HAPI_SPECIAL_MONOCHROME=4,
	VIM_HAPI_SPECIAL_SEPIA=0,
	VIM_HAPI_SPECIAL_NEGATIVE=2, 
	VIM_HAPI_SPECIAL_REDONLY,
	VIM_HAPI_SPECIAL_GREENONLY=7,
	VIM_HAPI_SPECIAL_EFFECT_ERROR = 0x7fffffff
} VIM_HAPI_SPECIAL_EFFECT; 
	
#if(BOARD_TYPE==VC0820_FPGA)
typedef struct tag_VIM_VIDEO_OUTINFO{
	unsigned int		v_srcwidth;			/* video display source window width form source image (IPP) */
	unsigned int		v_srcheight;			/* video display source window height form source image(IPP)*/
	unsigned int		v_srcxpos;			/* video display source window x pos form source image(IPP)*/
	unsigned int		v_srcypos;			/* video display source window y pos form source image (IPP)*/
	unsigned int		v_outwidth;			/* video display output width on LCD (IPP,LCDC)*/
	unsigned int		v_outheight;			/* video display output height on LCD (IPP,LCDC)*/
	unsigned int		v_outxpos;			/* video display output x pos on LCD (LCDC)*/
	unsigned int		v_outypos;			/* video display output y pos on LCD (LCDC)*/
	unsigned int		v_outrotation;		/* video display output rotation mode contorl by LCDC (LCDC)*/
	unsigned int		v_outeffect;			/* video display output effect control by IPP module (IPP)*/
}VIM_VIDEO_OUTINFO,*PVIM_VIDEO_OUTINFO;
 typedef enum _VIM_HAPI_CALLBACK_STATUS
{
	VIM_HAPI_CAPTURE_END=0,
	VIM_HAPI_FIFO_FULL,
	VIM_HAPI_FIFO_EMPTY,
	VIM_HAPI_ONEFRAME_END,
	VIM_HAPI_BUF_ERROR,
	VIM_HAPI_DECODE_END,
	VIM_HAPI_FIFO_THRLOW,
	VIM_HAPI_FIFO_UP,
	VIM_HAPI_LBUF_END,
	VIM_HAPI_UNKNOWN_ERROR = 0x7fffffff
}VIM_HAPI_CALLBACK_STATUS;
#else
 typedef enum _VIM_HAPI_CALLBACK_STATUS
{
	VIM_HAPI_CAPTURE_END=0,
	VIM_HAPI_FIFO_FULL,
	VIM_HAPI_ONEFRAME_END,
	VIM_HAPI_BUF_ERROR,
	VIM_HAPI_UNKNOWN_ERROR = 0x7fffffff
}VIM_HAPI_CALLBACK_STATUS;

#endif
 typedef enum _VIM_HAPI_DECODE_MODE
{
    VIM_DECODE_WITHFRAME_TO_YUV=0,
    VIM_DECODE_RESIZE_TO_YUV=1,
    VIM_DECODE_WITHFRAME_TO_RGB=2,
    VIM_DECODE_RESIZE_TO_RGB,
    VIM_HAPI_DECODE_MODE_ERROR = 0x7fffffff
 }VIM_HAPI_DECODE_MODE;

 typedef enum _VIM_HAPI_YUVTORGB
{
	VIM_CON_UYVY_565=0,
	VIM_CON_YUYV_565,
	VIM_CON_YYUV_565,
	VIM_HAPI_YUVTORGB_ERROR = 0x7fffffff
}VIM_HAPI_YUVTORGB;

 typedef enum _VIM_HAPI_MPEG4_STATUS
{
	VIM_HAPI_MPE4_END=1,
	VIM_HAPI_MPE4_ONEFRAME_END,
	VIM_HAPI_MPE4_GETDATA,
	VIM_HAPI_MPE4_ERROR,
	VIM_HAPI_MPE4_UNKNOWN_ERROR = 0x7fffffff
}VIM_HAPI_MPEG4_STATUS;
 
#if(BOARD_TYPE==VC0820_FPGA)
	typedef void (*VIM_HAPI_MP4DecCallBack)(UINT32 mp4_flag);
#endif
typedef struct tag_VIM_MPEG4INFO{
	UINT16 DecPoint_x;
	UINT16 DecPoint_y;
	UINT16 DecWidth;
	UINT16 DecHeigth;	
	UINT16 SorWidth;
	UINT16 SorHeigth;
	UINT32 FrameRate;
#if(BOARD_TYPE==VC0820_FPGA)
	VIM_HAPI_MP4DecCallBack Mp4DecCallback;
#endif
}VIM_MPEG4INFO, *PVIM_MPEG4INFO;
#if (BOARD_TYPE==VC0820_FPGA)
typedef enum _VIM_DECODE_MODE
{
	VIM_DECODE_ONLY=0,
	VIM_DECODE_IPP,
	VIM_DECODE_FRAME,
	VIM_DECODE_BYPASS_IPP,//only for vim_rom_save mode.  add by jiangkeqiang
	VIM_DECODE_BYPASS_FRAME,
	VIM_DECODE_NONE,
	VIM_DECODE_UNKOWN=0x7FFFFFFF
}VIM_DECODE_MODE;
#endif

#if(BOARD_TYPE==VC0820_FPGA)
#define		VIM_DECODE_WRITEBLOCK			0
#define		VIM_DECODE_BYBASSLBUF			0

typedef UINT32 (*VIM_HAPI_JPEGCallBack)(UINT32 Status, UINT32 Byte_Length);
typedef void (*VIM_HAPI_DECOCallBack)(UINT32 Status,UINT32 Byte_Length);
typedef enum _VIM_HAPI_DISPLAY_MODE
{
   VIM_HAPI_DISPLAY_JPEG=1,
   VIM_HAPI_DISPLAY_MOTIONJPEG,
   VIM_HAPI_DISPLAY_YUV,
   VIM_HAPI_DISPLAY_MODE_ERROR = 0x7fffffff
}VIM_HAPI_DISPLAY_MODE;

typedef struct tag_VIM_INJPEGDISPLAYINFO{
   HUGE UINT8 *filebuffer;
   UINT32 fileLength;
   TPoint LcdWantPt;
   TSize LcdWantSize; 
   UINT32 FrameRate;
   VIM_HAPI_DISPLAY_MODE DisplayMode;
   VIM_HAPI_JPEGCallBack JpegCallBack;
}VIM_INJPEGDISPLAYINFO, *PVIM_INJPEGDISPLAYINFO;

typedef struct tag_VIM_FILEINFO{
   VIM_HAPI_DISPLAY_MODE DisplayMode;
   HUGE UINT8 *RawDataPosition;
   UINT32 RawDataLength;
   UINT32 RawDataCounter;
   VIM_JPEG_Index ImageInfo;
}VIM_FILEINFO, *PVIM_FILEINFO;

typedef struct tag_VIM_DECODEINFO{
	VIM_HAPI_DISPLAY_MODE DisplayMode;
	VIM_DECODE_MODE   DecodeMode; 
	VIM_HAPI_SPECIAL_EFFECT EffectMode;
	VIM_HAPI_ROTATEMODE RotationMode;
	TPoint LcdWantPt;
	TSize LcdWantSize; 
	TPoint JpegOffsetPt;
	TSize JpegWinSize;
	VIM_JPEG_Index ImageInfo;
	VIM_HAPI_JPEGCallBack JpegCallBack;
	UINT32 JpegLbufAddr;
	UINT32 JpegDecodeBufAddr;
	UINT32 JpegDecodeBufLen;
}VIM_JPEG_DECODE_INFO, *PVIM_JPEG_DECODE_INFO;

typedef struct tag_VIM_BUFFERINFO{
	HUGE UINT8 *bufferaddr;
	UINT32 bufferlength;
	UINT32 readcursor;
	UINT32 writecursor;	
}VIM_BUFFERINFO, *PVIM_BUFFERINFO;
/*typedef struct tag_VIM_THUMBNAILINFO{
   TSize ThumbnailSize;
   HUGE UINT8 *ThumbnailAddr;
   UINT32 length;
   UINT8 IsThumbnail;
}VIM_THUMBNAILINFO, *PVIM_THUMBNAILINFO;*/
#endif 		

 


/*! tvdc osd format */
typedef enum _ENUM_HAPI_VIM_TVDC_OSDFORMAT
{
	VIM_HAPI_TVDC_OSDFMT_8BPP	= 0x03,
	VIM_HAPI_TVDC_OSDFMT_4BPP	= 0x07,
	VIM_HAPI_TVDC_OSDFMT_2BPP,
	VIM_HAPI_TVDC_OSDFORMAT_ERROR = 0x7fffffff
}VIM_HAPI_TVDC_OSDFORMAT;


typedef enum _ENUM_VIM_HAPI_TVDC_TVFORMAT
{
	VIM_HAPI_TVDC_TVFMT_PAL	= 0x00,
	VIM_HAPI_TVDC_TVFMT_NTSC,
	VIM_HAPI_TVDC_TVFORMAT_ERROR = 0x7fffffff
}VIM_HAPI_TVDC_TVFORMAT;

/*
typedef struct _VIM_HAPI_TVDC_OSD_PARA
{
	UINT32 datalen;
	UINT32 palettelen;
	
	//TSize windowsize;
	UINT16 display_x;
	UINT16 display_y;
	
	//TPoint startpoint;	
	UINT16 startpt_x;
	UINT16 startpt_y;

	VIM_HAPI_TVDC_OSDFORMAT osdformat;
	UINT8 alpha;
	UINT8 *data;
	UINT8 *palette;
	UINT8 enable;
}VIM_HAPI_TVDC_OSD_PARA;


typedef struct _VIM_HAPI_TVDC_PARAMETER
{
	//TSize adpcmsize;
	UINT16 display_x;
	UINT16 display_y;
	
	//TPoint startpoint;
	UINT16 startpt_x;
	UINT16 startpt_y;
	VIM_HAPI_TVDC_OSD_PARA *posd;

	VIM_HAPI_TVDC_TVFORMAT format;	
}VIM_HAPI_TVDC_PARAMETER;
*/
typedef enum _VIM_HAPI_FREQ_
{
	VIM_HAPI_FREQ_50HZ =		0x0,
	VIM_HAPI_FREQ_60HZ,
	VIM_HAPI_FREQ_ERROR = 0x7fffffff
}VIM_HAPI_FREQ;

typedef enum _VIM_HAPI_TVPREVIEW_MODE_
{
	VIM_HAPI_TVPREVIEW_ON = 0x00,
	VIM_HAPI_TVPREVIEW_OFF,
	VIM_HAPI_TVPREVIEW_MODE_ERROR = 0x7fffffff
}VIM_HAPI_TVPREVIEW_MODE;

typedef struct _VIM_HAPI_PreviewInfo
{
	VIM_HAPI_PREVIEW_MODE		mode;	
	VIM_HAPI_RESOLUTION			resolution;	
	VIM_HAPI_SPECIAL_EFFECT		effect;
	VIM_HAPI_CPTURE_QUALITY		quality;
	UINT32				cap_w;
	UINT32				cap_h;
	UINT32				pre_start_x;
	UINT32				pre_start_y;
	UINT32				pre_w;
	UINT32				pre_h;
} VIM_HAPI_PreviewInfo;

 
typedef void (*VIM_HAPI_CallBack)(	UINT8 Status, UINT32 Byte_Length);
  typedef void (*VIM_HAPI_MPEGCallBack)(VIM_HAPI_MPEG4_STATUS Status,UINT8* StartPoint, UINT32 Byte_Length);
  typedef void (*VIM_HAPI_MPEGDecCallBack)(VIM_HAPI_MPEG4_STATUS Status,UINT8** StartPoint, UINT32* Byte_Length);

#if (BOARD_TYPE==VC0820_FPGA)
typedef void (*VIM_HAPI_MP4_ENC_CallBack)(UINT32 flag, UINT32 Byte_Length);
typedef struct _VIM_HAPI_MP4_ENC_INIT_IN_INFO
{
      UINT32 preview_x;
      UINT32 preview_y;
      UINT32 preview_width;
      UINT32 preview_height;
      UINT32 osd_en;// 1: enable 0:disable  
      //UINT32 capture_x;
     // UINT32 capture_y;
      UINT32 capture_width;
      UINT32 capture_height;
      //UINT32 video_MaxTotalFrames;//0:no limit others: fix num recording num>1;	  
      VIM_HAPI_MP4_ENC_CallBack  pCallBack; 
      UINT32 time_increment;//suggestion 1001 but while 
      UINT32 time_resolution;//suggestion 30000 but while
	  
      UINT32 Enc_Stuff;// bit8: 0->8bit 1:->32bit and the lowest 8bits reprensent the stuff  data
      UINT32 Enc_Quality_Level; // 1-31 the higher the value is, the better the quality.
      UINT32 Enc_Half_Pixsel_Estimation_Enable;//
      UINT32 Enc_Short_HeaderSurpport;//0:not 1: yes def:0
//following info can not be used for shorthear      
      UINT32 Enc_Mpeg4_ResyncMarker_Disable;//0:not 1: yes
      UINT32 Enc_Mpeg4_HeaderExt;// 1: enable 0:disable  
      UINT32 Enc_Mpeg4_HeaderExtMaxPacketWordCnt;//it is usless when Enc_Mpeg4_ResyncMarker_Disable is 1
      UINT32 Enc_Mpeg4_QuantType;//0:H.263 1:mpeg-2
      UINT32 Enc_Mpeg4_4mv_Disable;//
      UINT32 Enc_Mpeg4_Rounding_Enable;//
      UINT32 Enc_Mpeg4_intra_dc_vlc;//0-7 
   
//-------------------------------------------------------------------
      UINT32 Enc_Out_Bitrate_Ctrl_En;
//following info can only be used when Enc_Out_Bitrate_Ctrl_En=1
      UINT32 RC_no_frame_skipping;
      UINT32 RC_fixed_Qp;
      UINT32 RC_out_bitrate;//128K bit/s set 128 64Kbit/s set 64 Unit Kbit/s
//----------------------------------------------------------------
}VIM_HAPI_MP4_ENC_INIT_IN_INFO,*PVIM_HAPI_MP4_ENC_INIT_IN_INFO;
typedef struct _VIM_HAPI_MP4_ENC_INIT_OUT_INFO
{
    //  UINT32 m_Enc_Out_BufferPtr[VIM_USER_MPEG4_ENC_OUTBUF_NUM];
      UINT32 OneBufferSize;
      UINT32 I_Frame_Estimat_Size;	  
}VIM_HAPI_MP4_ENC_INIT_OUT_INFO,*PVIM_HAPI_MP4_ENC_INIT_OUT_INFO;
typedef struct _VIM_HAPI_MP4_ENC_TIME
{
	UINT32 	bMod_Time_Base;		
	UINT32 	nTime_Inc;
} VIM_HAPI_MP4_ENC_TIME,*PVIM_HAPI_MP4_ENC_TIME;
#endif




typedef enum _TIMER_START_enum
{
   NO_TIMER_START =0,
   TIMER_STARTED
} TIMER_STATE;

typedef enum _LCD_UPDATE_enum
{
	UPDATED =0,
	NEED_UPDATE,
	FILLING
} UPDATE_STATE;

 typedef struct  _str_blayer_params_fill
{
	unsigned short   layer;
	unsigned short   startpointX;
	unsigned short   startpointY;
	unsigned short   endpointX;
	unsigned short   endpointY;
	unsigned short   screenW;
	unsigned short   screenH;
	unsigned short   lcd_update_part;
	unsigned short   lastLineNum;
	unsigned short   updateFlag;
	unsigned char	*pBuff;
}BLAYER_PARAMS_FILL;

 typedef struct  _str_blayer_params_update
{
	unsigned short   layer;
	unsigned short   startpointX;
	unsigned short   startpointY;
	unsigned short   screenW;
	unsigned short   screenH;
}BLAYER_PARAMS_UPDATE;


//UINT16 VIM_HAPI_InitVc05x(void);
///////////////////////chip mode ///////////////////////////////////
UINT16 VIM_HAPI_SetWorkMode(VIM_HAPI_WORKMODE WorkMode);
UINT16 VIM_HAPI_GetWorkMode(void);


///////////////////////////about preview/////////////////////////////////////
UINT16 VIM_HAPI_SetCaptureParameter(UINT16 CapWidth, UINT16 CapHeight);
UINT16 VIM_HAPI_SetPreviewParameter(UINT16 Lcd_OffsetW,UINT16 Lcd_OffsetH,UINT16 Width, UINT16 Height);
UINT16 VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_MODE MODE,VIM_HAPI_RESOLUTION bIndex);
UINT16 VIM_HAPI_SetThumbnailSize(UINT16 Width, UINT16 Height);
UINT16 VIM_HAPI_SetPreviewZoom(VIM_HAPI_CHANGE_MODE Mode,UINT8 step);
UINT16 VIM_HAPI_SetPreviewZoom_1(VIM_HAPI_CHANGE_MODE Mode,UINT8 step,INT32 framecx);
UINT16 VIM_HAPI_SetPreviewMirror(VIM_HAPI_SENSORMIRROR mode);

UINT16 VIM_HAPI_SetEffect(VIM_HAPI_SPECIAL_EFFECT Mode);
UINT16 VIM_HAPI_SetBrightness(VIM_HAPI_CHANGE_MODE Mode,UINT8 Step);
UINT16 VIM_HAPI_SetContrast(VIM_HAPI_CHANGE_MODE Mode,UINT8 Step);
void VIM_HAPI_Set_Freq(VIM_HAPI_FREQ freq);
///////////////////////////////lcd mode//////////////////////////////////////
UINT16 VIM_HAPI_UpdateLCD(VIM_HAPI_LAYER BLayer,UINT16 startx, UINT16 starty,UINT16 Width,UINT16 Height);
UINT16 VIM_HAPI_SelectLCDPanel(VIM_HAPI_LCDPANEL byPanel);
UINT16 VIM_HAPI_SetLCDMirror(VIM_HAPI_ROTATEMODE MirrorFlipMode, VIM_HAPI_LAYER Layer);
UINT16 VIM_HAPI_SetLCDSize(VIM_HAPI_LAYER BLayer,UINT16 wStartx,UINT16 wStarty,UINT16 Width,UINT16 Height);
UINT16 VIM_HAPI_SetLCDColordep(VIM_HAPI_COLORDEP_MODE byColorDepth);
UINT16 VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDWORKMODE byABLayerMode, UINT16 wValue);
UINT16 VIM_HAPI_ChangeLcdPosition(VIM_HAPI_LAYER BLayer,UINT16 wStartx,UINT16 wStarty);
void VIM_HAPI_LCDRotate(UINT8 degree);
void VIM_SetRT(UINT32 rtFlag);
UINT32 VIM_GetBlayerBuff(VIM_HAPI_LAYER BLayer);

/////////////////////////////b layer////////////////////////////
UINT16 VIM_HAPI_DrawLCDRctngl(VIM_HAPI_LAYER BLayer,UINT16 startx, UINT16 starty,UINT16 Width,UINT16 Height, HUGE  UINT8 *pwImage);
UINT16 VIM_HAPI_DrawLCDPureColor(VIM_HAPI_LAYER BLayer,UINT16 startx, UINT16 starty,UINT16 Width,UINT16 Height,UINT16 wRGBColor);
//UINT16 VIM_HAPI_UpdateLCD(VIM_HAPI_LAYER BLayer,UINT16 startx, UINT16 starty,UINT16 Width,UINT16 Height);
UINT16 VIM_HAPI_UpdateBG(UINT32 color,UINT16 x, UINT16 y,UINT16 width,UINT16 height);
UINT16 VIM_HAPI_GetLCDSize(VIM_HAPI_LAYER layer,UINT16 *width,UINT16 *height);
UINT16 VIM_HAPI_MallocBBuf(VIM_HAPI_LAYER BLayer,UINT16 wStartx,UINT16 wStarty,UINT16 Width,UINT16 Height);



/////////////capture//////////////////////////////////////////////
void VIM_HAPI_Timer(void);
UINT16 VIM_HAPI_SetCaptureQuality(VIM_HAPI_CPTURE_QUALITY Quality);
UINT16 VIM_HAPI_SetCaptureRatio(UINT8 ratio);
UINT16 VIM_HAPI_CaptureStill(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *StillBuf,UINT32 BUF_Length,VIM_HAPI_CallBack pUCallBack);
#if (BOARD_TYPE==VC0820_FPGA)
void VIM_HAPI_Cap_Recapture(void);
UINT32 VIM_HAPI_Cap_JbufDone(void);
UINT32 VIM_HAPI_GetJPEGCaptureStatus(void);
void VIM_HAPI_SetJPEGCaptureStatus(UINT32 marbflag);
UINT16 VIM_HAPI_CaptureStillThumbnail(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *StillBuf,UINT32 BUF_Length,VIM_HAPI_CallBack pUCallBack);
#endif
UINT32 VIM_HAPI_GetCaptureLength(void);
UINT16 VIM_HAPI_SetCaptureVideoInfo(VIM_HAPI_SAVE_MODE SaveMode,UINT8 bFramRate,UINT32 dwMaxFrameCount ,UINT32 BitRate);
#if (BOARD_TYPE==VC0820_FPGA)
UINT32 VIM_HAPI_Init_CaptureMp4Parameter(PVIM_HAPI_MP4_ENC_INIT_IN_INFO pMp4InInfo);
UINT32 VIM_HAPI_CaptureMp4_Init(PVIM_HAPI_MP4_ENC_INIT_IN_INFO pMp4InInfo,PVIM_HAPI_MP4_ENC_INIT_OUT_INFO pMp4OutInfo);
void VIM_HAPI_CaptureMp4_Release(void);
UINT32 VIM_HAPI_GetCaptureMp4_Mode(void);
void VIM_HAPI_SetCaptureMp4_Mode(UINT32 mode);
VIM_MARB_WORKMODE VIM_HAPI_Get_Marb_Mode(void);
void VIM_HAPI_CaptureMp4_StartNextVop(UINT32 nextpos,UINT32 voptype,UINT32 lastframelen,PVIM_HAPI_MP4_ENC_TIME enc_time);
UINT32 VIM_HAPI_StartCaptureMp4(UINT32 startpos);
UINT32 VIM_HAPI_StopCaptureMp4(void);
UINT32 VIM_HAPI_GetMp4EncStatus(void);
UINT32 VIM_HAPI_GetMp4EncVopLength(void);
UINT32 VIM_HAPI_GetMPEGEncIntStatus(void);
void VIM_HAPI_SetMPEGEncIntStatus(UINT32 marbflag);

#else
UINT16 VIM_HAPI_StartCaptureVideo(HUGE void *StillBuf,UINT32 BUF_Length,VIM_HAPI_MPEGCallBack pUCallBack);
#endif
UINT16 VIM_HAPI_StopCapture(void);
HUGE UINT8* VIM_HAPI_GetOneFrame(HUGE UINT8 *framebuf, UINT32 buflength, UINT8 framenumber, UINT32 *pframelength);
UINT16 VIM_HAPI_GetPictureSize(UINT16 *picWidth, UINT16 *picHeight);


/////////////display//////////////////////////////////////////////

UINT16 VIM_HAPI_ReadyToDisplay(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *jpegBuf, UINT32 length,UINT16 LcdOffsetX, UINT16 LcdOffsetY, UINT16 DisplayWidth, UINT16 DisplayHeight);
UINT16 VIM_HAPI_DisplayJpeg(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *jpegBuf, UINT32 length,UINT16 LcdOffsetX, UINT16 LcdOffsetY, UINT16 DisplayWidth, UINT16 DisplayHeight);


/////////////AF of sensor/////////////////////////////////////////
UINT16 VIM_HAPI_DoAutoFocus(void);
void VIM_HAPI_SetAFPreviewEnable(	UINT8 Able);

/////////////Video display////////////////////////////////////////
#if (BOARD_TYPE==VC0820_FPGA)
UINT32 VIM_HAPI_ChangeDisplaySize(UINT32 width,UINT32 height);
UINT32 VIM_HAPI_VideoDecInit(PVIM_MPEG4INFO Info);
UINT32 VIM_HAPI_GetDecInfo(PVIM_VIDEO_OUTINFO pDecInfo);
UINT32 VIM_HAPI_VideoResize(PVIM_MPEG4INFO Info);
UINT32 VIM_HAPI_StartNewBlockVop(UINT32 addr, UINT32 len, UINT32 mode);
UINT32 VIM_HAPI_GetMp4DecStatus(void);
UINT32 VIM_HAPI_DisableStuckError(void);
UINT32 VIM_HAPI_EnableStuckError(void);
UINT32 VIM_HAPI_VideoStopDec(void);
UINT32 VIM_HAPI_WriteVideoRawData(UINT32 addr,HUGE UINT8 *buf,UINT32 len);
//#else
UINT16 VIM_HAPI_SetDisplayVideoInfo(PVIM_MPEG4INFO Info,VIM_HAPI_SAVE_MODE SaveMode);
UINT16 VIM_HAPI_StartDisplayVideo(void *StillBuf,UINT32 BUF_Length,VIM_HAPI_MPEGDecCallBack pUCallBack);
UINT16 VIM_HAPI_ChangeVideoSize(void);
UINT32 VIM_HAPI_GetMPEGDecIntStatus(void);
void VIM_HAPI_SetMPEGDecIntStatus(UINT32 marbflag);

#endif

#if (BOARD_TYPE==VC0820_FPGA)
UINT32 VIM_HAPI_WriteJpegRawData(HUGE UINT8 *addr,UINT32 rawdatalen);
UINT32 VIM_HAPI_ReadJpegRawData(HUGE UINT8 *addr,UINT32 rawdatalen);
UINT32 VIM_HAPI_ParserJpegFile(PVIM_INJPEGDISPLAYINFO Info,PVIM_FILEINFO OutInfo);
UINT32 VIM_HAPI_InitDisplayInfo(PVIM_INJPEGDISPLAYINFO Info,PVIM_FILEINFO FileInfo,PVIM_JPEG_DECODE_INFO DecodeInfo);
UINT32 VIM_HAPI_DisplayYUVML(PVIM_JPEG_DECODE_INFO DecodeInfo);
void VIM_HAPI_InitDisplayRM(void);
UINT32 VIM_HAPI_DisplayYUVMLForRM(PVIM_JPEG_DECODE_INFO DecodeInfo);
UINT32 VIM_HAPI_GetRMVBDecInfo(PVIM_VIDEO_OUTINFO pDecInfo);
UINT32 VIM_HAPI_DisplayYUVForRM(PVIM_JPEG_DECODE_INFO DecodeInfo);
UINT32 VIM_HAPI_ReadyToDisplayJpeg(PVIM_JPEG_DECODE_INFO DecodeInfo);
UINT32 VIM_HAPI_StartToDisplayJpeg(PVIM_JPEG_DECODE_INFO DecodeInfo);
UINT32 VIM_HAPI_DecodeJpeg(HUGE UINT8 *addr,UINT32 rawdatalen);
//UINT32 VIM_HAPI_ReadyToDecodeJpeg(PVIM_JPEG_DECODE_INFO DecodeInfo,VIM_DECODE_MODE DecodeMode);
//UINT32 VIM_HAPI_StartDecodeJpeg(UINT8 * StillBuf,UINT32 BUF_Length,VIM_HAPI_DECOCallBack pUCallBack);
UINT32 VIM_HAPI_ReadyToDecodeJpeg(PVIM_JPEG_DECODE_INFO DecodeInfo);
UINT32 VIM_HAPI_StartDecodeJpeg(void);
UINT16 VIM_HAPI_SetCaptureThumbnailPreviewMode(VIM_HAPI_PREVIEW_MODE MODE,VIM_HAPI_RESOLUTION bIndex,TSize ThumbSize);
UINT16 VIM_HAPI_ReadyToEncodeJpeg(HUGE void *yuvdataBuf,TSize ImageSize,VIM_JPEG_YUVMODE YUVmode,VIM_HAPI_CPTURE_QUALITY Quality);
UINT16 VIM_HAPI_StartEncodeJpeg(HUGE void *StillBuf,UINT32 BUF_Length,VIM_HAPI_CallBack pUCallBack);
UINT32 VIM_HAPI_DirectDisplay(TSize ImageSize,TSize LCDsize,VIM_JPEG_YUVMODE YUVmode,VIM_HAPI_JPEGCallBack pUCallBack);
void VIM_HAPI_CaptureJpeg_Release(void);
void VIM_HAPI_DisplayJpeg_Release(void);
void VIM_HAPI_DecodeJpeg_Release(void);
UINT16 VIM_HAPI_SetSensorReg(UINT32 regaddr,UINT16 regvalue);
UINT16 VIM_HAPI_GetSensorReg(UINT32 regaddr,UINT16 *regvalue);
void VIM_HAPI_LCDCBuffer_Release(void);
void VIM_HAPI_LCDCBuffer_Cast(void);
void VIM_HAPI_Cap_Recapture(void);
void VIM_HAPI_Cap_FifoCont(void);
UINT32 VIM_HAPI_Cap_JbufDone(void);
void VIM_HAPI_Cap_JbufError(void);
UINT32 VIM_HAPI_ReadVideoRawData(UINT32 addr,HUGE UINT8 *buf,UINT32 len);
UINT32 VIM_HAPI_GetJpegDecStatus(void);
UINT16 VIM_HAPI_SetCaptureParameterChangeFlag(void);
UINT32 VIM_HAPI_ReadThumbnailData(HUGE UINT8 *addr,UINT32 buflen);
UINT16 VIM_HAPI_JPEG_MakeExif(UINT8* pbuf,UINT32 DataLen,UINT32 BufLen,UINT8* pthumb,UINT32 ThumbLen,UINT32 *jpglen);
void  VIM_HAPI_EncodeJpeg_Release(void);
UINT16 VIM_HAPI_EncodeJpeg(HUGE void *StillBuf,UINT32 BUF_Length,UINT32 *pjpglen);
UINT32 VIM_HAPI_GetDisplaySize(UINT32 * width,UINT32 * height);
UINT32 VIM_HAPI_LbufRead(void);
UINT32 VIM_HAPI_GetLbufReadStatus(void);
void VIM_HAPI_SetLbufReadStatus(UINT32 marbflag);
#endif


/*
/////////////About TV////////////////////////////////////////
void VIM_HAPI_TVDC_MoveOSD(UINT16 ptx,UINT16 pty, UINT8 show);
void  VIM_HAPI_DisplayJpeg_Show_BackColor_Only(void);
//UINT16 VIM_HAPI_DisplayJpeg_OnTV(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *jpegBuf, UINT32 length, UINT16 DisplayWidth, UINT16 DisplayHeight);
UINT16 VIM_HAPI_DisplayJpeg_OnTV(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *jpegBuf, UINT32 length, UINT16 DisplayWidth, UINT16 DisplayHeight,VIM_HAPI_TVDC_TVFORMAT format);
UINT16 VIM_HAPI_ReadyToDisplay_OnTV(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *jpegBuf, UINT32 length,UINT16 DisplayWidth, UINT16 DisplayHeight,VIM_HAPI_TVDC_TVFORMAT format);

UINT16 VIM_HAPI_StartDisplayVideo_OnTV(void *StillBuf,UINT32 BUF_Length,VIM_HAPI_MPEGCallBack pUCallBack,VIM_HAPI_TVDC_TVFORMAT format);
*/


UINT16 VIM_HAPI_SetTVPreviewMode(VIM_HAPI_TVPREVIEW_MODE MODE);
void VIM_HAPI_EnableTV(VIM_HAPI_TVDC_TVFORMAT format,UINT8 enable,UINT32 backcolor);
UINT16 VIM_HAPI_SetTVOSD(UINT16 startx,UINT16 starty,UINT8 alph,UINT8 enable,UINT8 *bmp);
UINT16 VIM_HAPI_TVDisplayJpeg(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *jpegBuf, UINT32 length, UINT16 DisplayWidth, UINT16 DisplayHeight);
UINT16 VIM_HAPI_TVStartDisplayVideo(void *StillBuf,UINT32 BUF_Length,VIM_HAPI_MPEGCallBack pUCallBack);

//////////////video conference/////////////////////////////////////////////////
UINT16 VIM_HAPI_SetVideoConferencePreviewMode(VIM_HAPI_PREVIEW_MODE MODE);

/*lhl add for MMI display yuv data on lcd a layer 2008.2.29*/
int VIM_DisplayYUVData2ALayer(UINT16 imW, UINT16 imH, UINT16 shX, UINT16 shY, UINT16 shW, UINT16 shH, UINT8* buff);

extern TSize gb0lcdsize;

#ifdef __cplusplus
}
#endif

#endif /* _RDK_HIF_H_ */

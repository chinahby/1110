/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_IPP_Driver.h
 * [Description]        : 适用于578b.和ipp相关的头文件
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-19 16:01
 * [Platform]               : 平台相关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-24 		angela      		 Created
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


#ifndef _VIM_5XRDK_IPP_H_
#define _VIM_5XRDK_IPP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define VIM_IPP_DOWNTOUP_DELAY	200

typedef enum _VIM_IPP_MODE
{
	VIM_IPP_MODE_PREVIEW=0,
	VIM_IPP_MODE_CAPTURE,
	VIM_IPP_MODE_DISPLAY,
	VIM_IPP_MODE_DECODE,
	VIM_IPP_MODE_CAPTURE_WITHTHUMB,
	VIM_IPP_MODE_ENCODE_MP4=5,
	VIM_IPP_MODE_DECODE_MP4=6,
	VIM_IPP_MODE_VIDEO_MP4=7,
	VIM_IPP_MODE_ERROR=0x7fffffff
}VIM_IPP_MODE;

typedef enum _VIM_IPP_SIZER
{
	VIM_IPP_BPSIZER_DISP_H 	= (1<<16),
	VIM_IPP_BPSIZER_DISP_W		= (1<<17),
	VIM_IPP_BPSIZER_CAP_DOWN_H	= (1<<18),
	VIM_IPP_BPSIZER_CAP_DOWN_W	= (1<<19),
	VIM_IPP_BPSIZER_CAP_UP_H		= (1<<20),
	VIM_IPP_BPSIZER_CAP_UP_W		= (1<<21),
	VIM_IPP_BPSIZER_THUMB_H		= (1<<22),
	VIM_IPP_BPSIZER_THUMB_W		= (1<<23),
	VIM_IPP_SIZER_ERROR=0x7fffffff
}VIM_IPP_SIZER;

typedef enum _VIM_IPP_CAPSIZE_MODE
{
	VIM_IPP_CAPMODE_DOWN= 0x2,
	VIM_IPP_CAPMODE_UP= 0x4,
#if (BOARD_TYPE==VC0820_FPGA)	
	VIM_IPP_DISP_CAPMODE_DOWN= 0x8,
#endif	
	VIM_IPP_CAPSIZE_MODE_ERROR=0x7fffffff
}VIM_IPP_CAPSIZE_MODE;

typedef enum _VIM_IPP_BUFFERMODER{
	VIM_IPP_MPG4_LINEBUF=1,
	VIM_IPP_MPG4_ONEFRAME=0,
	VIM_IPP_BUFFERMODER_ERROR=0x7fffffff
}VIM_IPP_BUFFERMODER;

typedef enum _VIM_IPP_THUMB_SELECT
{
	VIM_IPP_THUMB_FROM_CAPTURE= 0x2,
	VIM_IPP_THUMB_FROM_DISPLAY= 0x1,
	VIM_IPP_THUMB_FROM_SPECIAL=0,
	VIM_IPP_THUMB_FROM_LINEBUF=3,
	VIM_IPP_THUMB_SELECT_ERROR=0x7fffffff
}VIM_IPP_THUMB_SELECT;

typedef enum _VIM_IPP_EFFECT
{
	VIM_IPP_EFFECT_NORMAL = 0x8,
	VIM_IPP_EFFECT_SEPHIA = 0x0,
	VIM_IPP_EFFECT_SPECIALCOLOR = 0x1,
	VIM_IPP_EFFECT_NEGATIVE = 0x2,
	VIM_IPP_EFFECT_SKETCH = 0x3,
	VIM_IPP_EFFECT_MONO=0X4,
	VIM_IPP_EFFECT_FOUR_COLOR=0X5,
	VIM_IPP_EFFECT_TV = 0x6,
	VIM_IPP_EFFECT_GRID=0X7,
	VIM_IPP_EFFECT_ERROR=0x7fffffff
}VIM_IPP_EFFECT;

typedef enum _VIM_IPP_HAVEFRAM
{
	VIM_IPP_HAVEFRAME = 0x1,
	VIM_IPP_HAVE_NOFRAME=0X0,
	VIM_IPP_HAVEFRAM_ERROR=0x7fffffff
}VIM_IPP_HAVEFRAM;

typedef struct tag_VIM_IPP_SzDnFct
{
	UINT8	w1;
	UINT8	w2;
	UINT16	s1;
	UINT16	s2;
	UINT8	c;
	UINT16	maxdiv;
	UINT16	ifmaxdiv;
} VIM_IPP_SzDnFct, *PVIM_IPP_SzDnFct;

typedef struct tag_VIM_IPP_SzUpFct
{
	UINT8	w1;
	UINT8	w2;
	UINT16	s;
} VIM_IPP_SzUpFct, *PVIM_IPP_SzUpFct;





/*****************************************************************************************************/
#define	VIM_IPP_DISPLAY_MAX_W		0xa20		/*!< 2592 */
#define	VIM_IPP_DISPLAY_MAX_H		0x798		/*!< 1944 */

#define VIM_IPP_CSIZER_VIN_MAX		0x798		/*!< 1944 */
#define VIM_IPP_CSIZER_HIN_MAX		0xa20		/*!< 2592 */
#define VIM_IPP_CSIZER_VOUT_MAX		0x798		/*!< 1944 */
#define VIM_IPP_CSIZER_HOUT_MAX		0xa20		/*!< 2592 */

#define VIM_IPP_TSIZER_HOUT_MIN		0x28		/*!<  */
#define VIM_IPP_TSIZER_VOUT_MIN		0x1e		/*!<  */
#define VIM_IPP_TSIZER_HOUT_MAX		0x100		/*!<  */
#define VIM_IPP_TSIZER_VOUT_MAX		0x100		/*!<  */
/*****************************************************************************************************/



void VIM_IPP_Reset(void);
void VIM_IPP_SetMode(VIM_IPP_MODE bMode,VIM_IPP_HAVEFRAM bHaveFrame);
void VIM_IPP_GetThumbSize(PTSize Size);
void VIM_IPP_GetCaptureWindowSize(PTSize Size);
void VIM_IPP_SetImageSize(UINT16 wWidth, UINT16 wHeight);
void VIM_IPP_GetImageSize(UINT16 *wWidth, UINT16 *wHeight);
void VIM_IPP_GetWindowSize(UINT16 *wWidth, UINT16 *wHeight);
void VIM_IPP_GetDispalyWindowSize(UINT16 *wWidth, UINT16 *wHeight);
void VIM_IPP_SetSizeToLcdEnable(UINT8 enable);
VIM_IPP_CAPSIZE_MODE VIM_IPP_GetCaputreSizeSelect(void);


VIM_RESULT VIM_IPP_SetDispalySize(TPoint pt, TSize winSize, TSize dispSize);
VIM_RESULT VIM_IPP_SetCaptureSize(TSnrInfo *pSensorInfo,TPoint pt, TSize winSize, TSize CaptureSize);
VIM_RESULT VIM_IPP_SetThumbSize(UINT8 bWidth, UINT8 bheight,VIM_IPP_THUMB_SELECT ThumbSelect);
void VIM_IPP_SetDisplayDropFrame(UINT16 wDropFrame);
void VIM_IPP_GetDispSize(PTSize Size);
void VIM_IPP_SetEffectUVoffset(UINT8 bUoffset,UINT8 bVoffset);
void VIM_IPP_SetEffectUVThreshold(UINT8 bU_up,UINT8 bU_down,UINT8 bV_up,UINT8 bV_down);
void VIM_IPP_SetThumbbufMem(UINT32 StartAdd,UINT32 Size);
void VIM_IPP_SetEffect(VIM_IPP_EFFECT bEffect);
void VIM_IPP_SetMpeg4BufMode(VIM_IPP_BUFFERMODER bMode);
void VIM_IPP_SetCapSyncGen(BOOL Enable);
#if(BOARD_TYPE==VC0820_FPGA)
void VIM_IPP_SetThumbnailEn(BOOL Enable);
#endif
// tools 
VIM_RESULT VIM_IPP_ToolCaculateLessSrcWindow(TSize src, TSize dst,TSize *win);
VIM_RESULT VIM_IPP_ToolCaculateBigDstWindow(TSize src, TSize dst,TSize *win);
VIM_RESULT VIM_IPP_ToolCaculateLessDisplayWindow(TSize src, TSize dst,TSize *dis);
VIM_RESULT VIM_IPP_SetDisplaySizerSize(TPoint winpt, TSize winSize, TSize DisplaySize, TSize* DestSize);

#if (BOARD_TYPE==VC0820_FPGA)
//ipp close
UINT32 VIM_IPP_Close(void);
void VIM_IPP_Open(UINT32 value);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _RDK_HIF_H_ */

 

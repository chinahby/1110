/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2005                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : VIM_DISP_Driver.h
 * [Description]          : 5X 's LCDC moudle sub driver:578b 
 * [Author]                 : WendyGuo	
 * [Date Of Creation]  : 2007-03-23 
 * [Platform]              : not limited by platform
 * [Note]                    : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        		Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2005-11-02  	MaNing			The first version
 * 2006-06-08  	Angela			Update for 528
 * 2006-03-23  	WendyGuo         	Created 578b
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

#ifndef _VIM_5XRDK_DISP_H_
#define _VIM_5XRDK_DISP_H_

#ifdef __cplusplus
extern "C" {
#endif

// set B layer is B0/B1/ALL
#if 0
typedef enum _VIM_DISP_LAYER_enum
{
	VIM_DISP_ALAYER=0x01,			//wendy guo changed
//	VM_LCDC_A1LAYER=0x02,		/*!< not support in vc0578b */
	VIM_DISP_B0LAYER=0x04,
	VIM_DISP_B1LAYER=0x08,
	VIM_DISP_ALLBLAYER=0x0c,
	VM_LCDC_LAYER_BG=0x10,
	VIM_DISP_ALLLAYER=0x1d		/*!< if if support layer, a1, this value is 0x1f */			

	
}VIM_DISP_LAYER;
#endif

typedef enum _VIM_DISP_LAYER_enum
{
	VIM_DISP_ALAYER = 1,					//528 parameters
	VIM_DISP_B0LAYER= 2,
	VIM_DISP_B1LAYER=4,
	VIM_DISP_ALLBLAYER=6,
	VIM_DISP_A1LAYER=8,
	VIM_DISP_ALLLAYER=0xf,
	VIM_DISP_LAYER_ERROR = 0x7fffffff
}VIM_DISP_LAYER;


// work mode
typedef enum VIM_DISP_WORKMODE
{
	VIM_DISP_AFIRST,
	VIM_DISP_BFIRST,
	VIM_DISP_B0_OVERLAY,
	VIM_DISP_B0_BLEND,
	VIM_DISP_ALL_OVERLAY,
	VIM_DISP_ALL_BLEND,
	VIM_DISP_DECODE,
	VIM_DISP_WORKMODE_ERROR = 0x7fffffff
}VIM_DISP_WORKMODE;

// B layer Graphic buffer data format
typedef enum VIM_DISP_BFORMAT{
	VIM_DISP_BLAYER_RGB1=0,
	VIM_DISP_BLAYER_RGB2,
	VIM_DISP_BLAYER_RGB4,
	VIM_DISP_BLAYER_RGB8,
	VIM_DISP_BLAYER_RGB444,
	VIM_DISP_BLAYER_RGB555,
	VIM_DISP_BLAYER_RGB565,
	VIM_DISP_BLAYER_RGB666,
	VIM_DISP_BLAYER_RGB24,
	VIM_DISP_BLAYER_RGB32,
	VIM_DISP_BFORMAT_ERROR = 0x7fffffff
}VIM_DISP_BFORMAT;

typedef enum _VIM_DISP_ROTATEMODE{
	VIM_DISP_ROTATE_0=0,//revised by youhai 2007.9.17
	VIM_DISP_ROTATE_90,
	VIM_DISP_ROTATE_180,
	VIM_DISP_ROTATE_270,
	VIM_DISP_MIRROR_0,
	VIM_DISP_MIRROR_90,
	VIM_DISP_MIRROR_180,
	VIM_DISP_MIRROR_270,
	VIM_DISP_NOTCHANGE,
	VIM_DISP_ROTATEMODE_ERROR = 0x7fffffff
}VIM_DISP_ROTATEMODE;

typedef enum _VIM_DISP_BUFFERMODER{
	VIM_DISP_LINEBUF=0,
	VIM_DISP_NODISP=1,
	VIM_DISP_TWOFRAME= 2,
	VIM_DISP_ONEFRAME=3,
	VIM_DISP_TVOUT_ONEFRAME=4,
	VIM_DISP_TVOUT_TWOFRAME=5,
	VIM_DISP_BUFFERMODER_ERROR = 0x7fffffff
}VIM_DISP_BUFFERMODER;



typedef enum _VIM_DISP_DITHERSEL{
	VIM_DISP_RGB444=0,//revised by youhai 2007.9.17
	VIM_DISP_RGB565,
	VIM_DISP_RGB666,
	VIM_DISP_RGB332,
	VIM_DISP_DITHERSEL_ERROR = 0x7fffffff
}VIM_DISP_DITHERSEL;

/*! lcd panel interface mode */
typedef enum _VIM_DISP_PANELIFMODE			//wendy guo modified
{
	VM_LCDC_PANELIF_DBI		= 0x01,
	VM_LCDC_PANELIF_DPI		= 0x02,
	VIM_DISP_PANELIFMODE_ERROR = 0x7fffffff
}VIM_DISP_PANELIFMODE;

/*! lcdc layer b overlay mode */
typedef enum _VIM_DISP_OVERLAYMODE	//wendy guo modified
{
	VM_LCDC_OVERLAY_TRANSPARENT	= 0x00,
	VM_LCDC_OVERLAY_AND			= 0x01,
	VM_LCDC_OVERLAY_OR			= 0x02,
	VM_LCDC_OVERLAY_INVERT		= 0x03,
	VIM_DISP_OVERLAYMODE_ERROR = 0x7fffffff
}VIM_DISP_OVERLAYMODE;

/*!< lcdc background data mode */
typedef enum _VIM_DISP_BGCOLORFORMAT
{
	VM_LCDC_BGCOLOR_RGB24	= 0x00,		/*!< rgb 8:8:8, no alpha bit */
	VM_LCDC_BGCOLOR_RGB32	= 0x01,		/*!< rgb 8:8:8, with alpha bit */
	VIM_DISP_BGCOLORFORMAT_ERROR = 0x7fffffff
}VIM_DISP_BGCOLORFORMAT;

typedef enum _VIM_DISP_INT_TYPE
{
	VIM_INT_DISP_FIFO			= 0,
	VIM_INT_DISP_B0_UPDATE		= 1,
	VIM_INT_DISP_B1_UPDATE		= 2, //ignore
	VIM_INT_DISP_BG_UPDATE			= 3,
	VIM_INT_DISP_A0_DONE		= 4,
	VIM_INT_DISP_A1_DONE	= 5, //ignore
	VIM_INT_DISP_A0_START		= 6,
	VIM_INT_DISP_A0_BEF			= 7,
	VIM_INT_DISP_ALL			= VIM_DISP_INT_NUM,
	VIM_DISP_INT_TYPE_ERROR = 0x7fffffff
}VIM_DISP_INT_TYPE;
#define AM_MAXPIXEL	 1024

/* add for LCD Mode Driver,Zhang Xuecheng,2007-12-4 10:36:50 */
typedef  struct
{
 	UINT8* addr;
 	UINT32 rotate_mode;
 	UINT16 fmt;
 	UINT16 width;
 	UINT16 height;
	UINT16 dpt_x;
	UINT16 dpt_y;
	UINT16 panel_width;
}gbuf_info;

void VIM_IPP_GetWindowPos(UINT16 *wStart_X, UINT16 *wStart_Y);

VIM_RESULT VIM_DISP_SetWorkMode(VIM_DISP_WORKMODE bWorkMode);
VIM_RESULT VIM_DISP_SetDitherSelet(VIM_DISP_DITHERSEL byDitherSelect);
VIM_RESULT VIM_DISP_SetBufferMode(VIM_DISP_BUFFERMODER byBufferMode);
VIM_DISP_BUFFERMODER VIM_DISP_GetBufferMode(void);
VIM_RESULT VIM_DISP_SetDitherEnable(BOOL Enable);
VIM_RESULT VIM_DISP_SetLayerEnable(VIM_DISP_LAYER bLayer,BOOL Enable);

VIM_RESULT	VIM_DISP_SetPanelifMode(VIM_DISP_PANELIFMODE mode);		//new function
UINT8		VIM_DISP_GetPanelifMode(void);								//new function

//VIM_RESULT VIM_DISP_SaveAramEnable(BOOL Enable);		//no function in 578b
void VIM_DISP_ResetState(void);
VIM_RESULT VIM_DISP_Update(VIM_DISP_LAYER BLayer);
VIM_RESULT VIM_DISP_SetRotateMode(VIM_DISP_ROTATEMODE RotateMode);		//wendy guo//3/29/2007
VIM_RESULT VIM_DISP_SetOverlayMode(VIM_DISP_LAYER layer, VIM_DISP_OVERLAYMODE mode);	//new function
VIM_RESULT VIM_DISP_SetOverlayKeyColor(VIM_DISP_LAYER BLayer,UINT32 dwKeyColor);

VIM_RESULT VIM_DISP_SetBlendRatio(VIM_DISP_LAYER BLayer,UINT16 byBlendRatio);
VIM_RESULT VIM_DISP_SetGbufFormat(VIM_DISP_BFORMAT byGbufFmt);
void VIM_DISP_GetGbufFormat(VIM_DISP_BFORMAT * byGbufFmt);

#if 0
VIM_RESULT VIM_DISP_GetWinSize(VIM_DISP_LAYER layer, PTSize psize);			//new function
VIM_RESULT VIM_DISP_SetWinSize(VIM_DISP_LAYER layer, TSize size);				//new
VIM_RESULT VIM_DISP_SetDisplayPanel(VIM_DISP_LAYER layer, TPoint startpt, TSize winsize);	//new
VIM_RESULT VIM_DISP_SetRefreshBlock(VIM_DISP_LAYER layer, TPoint startpt, TSize winsize);	//new
#endif

VIM_RESULT VIM_DISP_UpdateBackgroud(VIM_DISP_BGCOLORFORMAT format, UINT32 color, UINT32 x, UINT32 y, UINT32 width, UINT32 height);	//new
/////////////////////////////////////////////////////////////////////
void VIM_DISP_GetAWinsize(PTSize winSize);
VIM_RESULT VIM_DISP_GetBSize(VIM_DISP_LAYER BLayer,PTSize pTSize);
VIM_RESULT VIM_DISP_SetA_Memory(TPoint memPoint,TSize memSize);
VIM_RESULT VIM_DISP_SetA_DisplayPanel(TPoint startPoint,TSize winSize);
VIM_RESULT VIM_DISP_SetA_DisplaySize(TSize winSize);
VIM_RESULT VIM_DISP_SetB0_DisplayPanel(TPoint startPoint,TSize winSize);
VIM_RESULT VIM_DISP_SetB1_DisplayPanel(TPoint startPoint,TSize winSize);
VIM_RESULT VIM_DISP_SetB0_RefreshBlock(TPoint startPoint,TSize winSize);
VIM_RESULT VIM_DISP_SetB1_RefreshBlock(TPoint startPoint,TSize winSize);
VIM_RESULT VIM_DISP_GetBStartPoint(VIM_DISP_LAYER BLayer,PTPoint pStartPoint);
void VIM_DISP_SetIntEnable(VIM_DISP_INT_TYPE bySecondLevelInt,BOOL Eanble);
void VIM_DISP_RegisterISR(VIM_DISP_INT_TYPE bySecondLevelInt, PVIM_Isr  Isr);
void _ISR_DispIntHandle(UINT8 wIntFlag);
void VIM_DISP_B0_Done_isr(void);
void VIM_DISP_B1_Done_isr(void);
//wendy guo void VIM_DISP_EnableVrgbMode(UINT8 bEnable);
//wendy guo void VIM_DISP_EnableDecodeSpecial(UINT8 bEnable);
VIM_RESULT VIM_DISP_SetA1_DisplayPanel(TPoint startPoint,TSize winSize);
void VIM_DISP_GetA1Winsize(PTSize winSize);
VIM_RESULT VIM_DISP_SetRGBgammaValue(void);
VIM_RESULT VIM_DISP_SetRGBgammaEnable(BOOL Enable);
#ifdef __cplusplus
}
#endif

#endif



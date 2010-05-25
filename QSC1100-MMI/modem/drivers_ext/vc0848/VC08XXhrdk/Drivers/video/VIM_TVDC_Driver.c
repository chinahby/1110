/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_TVDC_Driver.c
 * [Description]        : TV out 相关
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-05-18 10:00
 * [Platform]               : 平台无关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-05-18  		jiangkeqiang      	 Created
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

#include "VIM_COMMON.h"
/*
该文件目前提供4个对外接口。
1.VIM_RESULT VIM_TVDC_Init_Parameter(VIM_TVDC_PARAMETER * tvpara,ENUM_VIM_TVDC_PARASET_MODE mode)
用于设置些关于TV的信息

2. VIM_RESULT VIM_TVDC_OpenTV(VIM_TVDC_MODE mode) 
用于打开ColorBar,打开preview,colorbar,或者still image


3.void VIM_TVDC_CloseTV(void)
用于关闭TV


4.VIM_RESULT VIM_TVDC_MoveOSD(TPoint pt,VIM_TVDC_OSD_MODE show)
用于打开OSD,关闭OSD,移动OSD
*/


/******************************************************************************************/


#if VIM_USER_SUPPORT_TV
/*! tv format config */
const UINT8	g_btvformatcfg[2][55] = {
/*! pal format configuration */
{	
0x06,0xc0,0x02,0x71,0x05,0x05,0x05,0x2d,
0x02,0xe1,0x3f,0x7f,0x97,0xd4,0x01,0x1b,
0x29,0x44,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0x21,0x00,0x02,
0x00,0x22,0x00,0x38,0x00,0x43,0x00,0x03,
0x00,0x31,0x00,0x4e,0x00,0x5f,0x2a,0x09,
0x8a,0xcb,0xf8,0x00,0x57,0xbe,0xb0
},
/*! ntsc format configuration */
{
0x06,0xb4,0x02,0x0d,0x06,0x06,0x06,0x22,
0x02,0xdc,0x3e,0x7f,0x8f,0xd7,0x01,0x07,
0x2e,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0x10,0x00,0x02,
0x00,0x20,0x00,0x34,0x00,0x40,0x00,0x03,
0x00,0x2e,0x00,0x4a,0x00,0x5a,0x21,0xf0,
0x7b,0xd7,0xec,0x2c,0x76,0xba,0xba
}
};

/******************************************************************************************/


/***********************************************************************************
Description:
		 get the state of tv.
		
Parameters:
		void.
		
Returns:
		1	: ready.
		0	: not ready.
		
Remarks:
		
************************************************************************************/
UINT8 VIM_TVDC_GetTvState(void)
{
	return (VIM_HIF_GetReg32(V5_REG_TVDC_CTRL)&0x01);	
}

/***********************************************************************************
Description:
		 reset tv display controller.
		
Parameters:
		void.
		
Returns:
		void.
		
Remarks:
		
************************************************************************************/
void VIM_TVDC_Reset(void)
{
	UINT32 regval = 0, delay = 0x200;//0xff;

	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);
	regval |= ((UINT32)1<<1);
	VIM_HIF_SetReg32(V5_REG_TVDC_CTRL, regval);
	while(delay){
		delay--;
	}
	regval &= (~((UINT32)1<<1));
	VIM_HIF_SetReg32( V5_REG_TVDC_CTRL, regval);
}

/***********************************************************************************
Description:
		 enable/disable tv osd display.
		
Parameters:
		enable	: 1 for enable, 0 for disable.
		
Returns:
		void.
		
Remarks:
		
************************************************************************************/
void VIM_TVDC_EnableOsd(UINT8 enable)
{
	UINT32	regval = 0;
	
	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);
	if(enable){		/*!< enable */
		regval |= ((UINT32)1<<2);
	}else{			/*!< disable */
		regval &= (~((UINT32)1<<2));
	}
	VIM_HIF_SetReg32(V5_REG_TVDC_CTRL,	regval);
}

/***********************************************************************************
Description:
		 select tv format.
		
Parameters:
		format	: One of the following <U>VM_TVDC_TVFORMAT</U>
 				enumeration values that specify tv format type.
 				</VM_TVDC_TVFMT_PAL>
 				</VM_TVDC_TVFMT_NTSC>
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.

Remarks:
		
************************************************************************************/
VIM_RESULT VIM_TVDC_SetTvFormat(VIM_TVDC_TVFORMAT format)
{
	UINT32	regval = 0;
	SINT32	i = 0;
	
	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);	
	if(format==VIM_TVDC_TVFMT_PAL){
		regval &= (~((UINT32)1<<3));
	}else if(VIM_TVDC_TVFMT_NTSC){
		regval |= ((UINT32)1<<3);
	}else{
		return VIM_ERROR_TVDC_TVFMT_UNKNOWN;
	}
	
	for(i=0; i<55; i++){
		VIM_HIF_SetReg32(V5_REG_TVENC_TVCOLMAX_H+(i<<2),g_btvformatcfg[format][i]);
	}
	VIM_HIF_SetReg32(V5_REG_TVDC_CTRL,	regval);
	
	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 enable/disable zoom bypass.
		
Parameters:
		type	: One of the following <U>VM_TVDC_ZOOMTYPE</U>
 				enumeration values that specify tv zoom type.
		enable	: 1 for enable, 0 for disable.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.

Remarks:
		
************************************************************************************/
VIM_RESULT VIM_TVDC_EnableZoomBypass(VIM_TVDC_ZOOMTYPE type, UINT8 enable)
{
	UINT32	regval = 0;
	
	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);
	switch(type)
	{
	case VIM_TVDC_ZOOM_HORIZONTAL:
		regval &= (~((UINT32)1<<8));
		regval |= ((UINT32)enable<<8);
		break;
	case VIM_TVDC_ZOOM_VERTICAL:
		regval &= (~((UINT32)1<<10));
		regval |= ((UINT32)enable<<10);
		break;
	case VIM_TVDC_ZOOM_ALL:
		regval &= (~((UINT32)1<<8));
		regval |= ((UINT32)enable<<8);
		regval &= (~((UINT32)1<<10));
		regval |= ((UINT32)enable<<10);
		break;
	default:
		return VIM_ERROR_TVDC_ZOOMTYPE_UNKNOWN;
	}
	VIM_HIF_SetReg32(V5_REG_TVDC_CTRL,	regval);
	
	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 set tv osd display panel.
		
Parameters:
		startpt	: start point of tv osd.
		size	: tv osd size.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.

Remarks:
		
************************************************************************************/
VIM_RESULT VIM_TVDC_SetOsdPanel(TPoint startpt, TSize size)
{
	UINT32	regval=0, posval=0, sizeval=0;
	UINT32	buflen=0, osdfmt=0;
	UINT32	maxwidth=720, maxheight=0;
	
	startpt.x >>= 1;	/*!< start position coordinate x should be even */
	startpt.x <<= 1;	
	startpt.y >>= 1;	/*!< start position coordinate y should be even */
	startpt.y <<= 1;
	
	size.cx	>>= 1;		/*!< osd size coordinate x should be even */
	size.cx	<<= 1;
	size.cy	>>= 1;		/*!< osd size coordinate y should be even */
	size.cy	<<= 1;

	

	
	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);
	regval >>= 3;
	regval &= 0x01;
	if(regval==0)
	{	/*!< tv format : PAL */
		maxheight = VIM_TVDC_PAL_MAX_HEIGHT;
		maxwidth = VIM_TVDC_PAL_MAX_WIDTH;
		
		startpt.x += VIM_TVDC_PAL_MINOFFSET_X;
		startpt.y += VIM_TVDC_PAL_MINOFFSET_Y;
	}
	else
	{			/*!< tv format : NTSC */
		maxheight = VIM_TVDC_NTSC_MAX_HEIGHT;
		maxwidth = VIM_TVDC_NTSC_MAX_WIDTH;
		
		startpt.x += VIM_TVDC_NTSC_MINOFFSET_X;
		startpt.y += VIM_TVDC_NTSC_MINOFFSET_Y;
	}
	
	if((startpt.x>(maxwidth-1)) || (startpt.y>(maxheight-1))){
		return VIM_ERROR_TVDC_OSDPARAM_OUTOFRANGE;
	}
	if((size.cx>maxwidth) || (size.cy>maxheight)){
		return VIM_ERROR_TVDC_OSDPARAM_OUTOFRANGE;
	}
	
	posval	|= ((UINT32)(startpt.y&0x3ff)<<16);
	posval	|= ((UINT32)(startpt.x&0x3ff));
	sizeval	|= ((UINT32)(size.cy&0x3ff)<<16);
	sizeval	|= ((UINT32)(size.cx&0x3ff));
	osdfmt = VIM_HIF_GetReg32(V5_REG_TVDC_OSD_ALPHA);
	osdfmt >>= 8;
	osdfmt &= 0x0f;

	size.cx = size.cx /(osdfmt+1)*(osdfmt+1);
	
	buflen = (size.cx*size.cy)/(osdfmt+1);
	VIM_HIF_SetReg32(V5_REG_TVDC_OSD_XY,	posval);
	VIM_HIF_SetReg32(V5_REG_TVDC_OSD_SIZE,	sizeval);
	VIM_HIF_SetReg32(V5_REG_TVDC_0SD_LENGTH, buflen);
	
	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 set tv osd base address.
		
Parameters:
		addr	: osd base address.
		
Returns:
		void.

Remarks:
		
************************************************************************************/
void VIM_TVDC_SetOsdAddress(UINT32 addr)
{
	VIM_HIF_SetReg32(V5_REG_TVDC_OSD_BA,	(( addr >> 2 ) & 0x3fffffff ));
}

/***********************************************************************************
Description:
		 set tv osd alpha blending coefficient.
		
Parameters:
		alpha	: alpha blending coefficient.
		
Returns:
		void.

Remarks:
		
************************************************************************************/
void VIM_TVDC_SetOsdAlphablending(UINT8 alpha)
{
	UINT32	regval = 0;
	
	regval = VIM_HIF_GetReg32(V5_REG_TVDC_OSD_ALPHA);
	regval &= (~((UINT32)0xff));
	regval |= ((UINT32)alpha&0xff);
	VIM_HIF_SetReg32(V5_REG_TVDC_OSD_ALPHA,	regval);
}

/***********************************************************************************
Description:
		 set tv osd bpp format.
		
Parameters:
		format	: One of the following <U>VM_TVDC_OSDFORMAT</U>
 				enumeration values that specify tv osd bpp format.
				</VM_TVDC_OSDFMT_8BPP>
				</VM_TVDC_OSDFMT_4BPP>
				</VM_TVDC_OSDFMT_2BPP>
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.

Remarks:
		should call this function before function <vtvdc_setosdpanel>;
************************************************************************************/
VIM_RESULT VIM_TVDC_SetOsdbppFormat(VIM_TVDC_OSDFORMAT format)
{
	UINT32	regval = 0;
	
	if((format!=VIM_TVDC_OSDFMT_8BPP) && (format!=VIM_TVDC_OSDFMT_4BPP) && (format!=VIM_TVDC_OSDFMT_2BPP)){
		return VIM_ERROR_TVDC_OSDFMT_UNKNOWN;
	}
	
	regval = VIM_HIF_GetReg32(V5_REG_TVDC_OSD_ALPHA);
	regval &= (~((UINT32)0x0f<<8));
	regval |= ((UINT32)format<<8);
	VIM_HIF_SetReg32(V5_REG_TVDC_OSD_ALPHA,	regval);
	
	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 set tv display controller frame buffer base address.
		
Parameters:
		addr0	: frame buffer 0 base address.
		addr1	: frame buffer 1 base address.
		
Returns:
		void.

Remarks:
		
************************************************************************************/
void VIM_TVDC_SetFrameAddress(UINT32 addr0, UINT32 addr1)
{
	VIM_HIF_SetReg32(V5_REG_TVDC_FRM0_BA,	(addr0>>2));
	VIM_HIF_SetReg32(V5_REG_TVDC_FRM1_BA,	(addr1>>2));
}

/***********************************************************************************
Description:
		 set tv display controller background color.
		
Parameters:
		y	: background color y.
		u	: background color u.
		v	: background color v.
		
Returns:
		void.

Remarks:
		
************************************************************************************/
void VIM_TVDC_SetBackgroundColor(UINT8 y, UINT8 u, UINT8 v)
{
	UINT32	regval = 0;
	
	regval |= ((UINT32)y<<16);
	regval |= ((UINT32)u<<8);
	regval |= ((UINT32)v);
	VIM_HIF_SetReg32(V5_REG_TVDC_BG_COLOR,	regval);
}

/***********************************************************************************
Description:
		 set tv display controller active frame start point.
		
Parameters:
		startpt	: start point of tvdc active frame.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.

Remarks:
		
************************************************************************************/
VIM_RESULT VIM_TVDC_SetActiveFrameStartPoint(TPoint startpt)
{
	UINT32	regval = 0;
	UINT32	maxwidth=720, maxheight=0;
	
	startpt.x >>= 1;	/*!< start position coordinate x should be even */
	startpt.x <<= 1;	
	startpt.y >>= 1;	/*!< start position coordinate y should be even */
	startpt.y <<= 1;
	
	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);
	regval >>= 3;
	regval &= 0x01;
	if(regval==0)
	{	/*!< tv format : PAL */
		startpt.x += VIM_TVDC_PAL_MINOFFSET_X;
		startpt.y += VIM_TVDC_PAL_MINOFFSET_Y;
		maxheight = VIM_TVDC_PAL_MAX_HEIGHT;
		maxwidth = VIM_TVDC_PAL_MAX_WIDTH;
	}
	else
	{			/*!< tv format : NTSC */
		startpt.x += VIM_TVDC_NTSC_MINOFFSET_X;
		startpt.y += VIM_TVDC_NTSC_MINOFFSET_Y;
		maxheight = VIM_TVDC_NTSC_MAX_HEIGHT;
		maxwidth = VIM_TVDC_NTSC_MAX_WIDTH;
	}
	
	if((startpt.x>(maxwidth-1)) || (startpt.y>(maxheight-1))){
		return VIM_ERROR_TVDC_PCMPARAM_OUTOFRANGE;
	}
	
	regval = ((UINT32)(startpt.y&0x3ff)<<16);
	regval |= ((UINT32)(startpt.x&0x3ff));
	VIM_HIF_SetReg32(V5_REG_TVDC_FRM_XY,	regval);
	
	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 set tv display controller palette.
		
Parameters:
		num		: number of palette.
		ppalette: address to store palette data.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.

Remarks:
		because the clock domains are crossed when these registers are written, user
	must delay 20 cycles after each write operation to these palette registers.
************************************************************************************/
VIM_RESULT VIM_TVDC_SetYUVPalette(UINT16 num, UINT8 *ppalette)
{
	UINT16 i=0, j=0;
	UINT32 paladdr=0, regval=0;

	if(ppalette == NULL){
		return VIM_ERROR_TVDC_DATA_INVALID;
	}

	for(i=0; i<num; i++){
		paladdr = V5_REG_TVDC_PALLETTE+(i<<2);
		regval = (ppalette[i*4]|ppalette[i*4+1]<<8|ppalette[i*4+2]<<16|ppalette[i*4+3]<<24);
		//regval  = *(ppalette+i);
		VIM_HIF_SetReg32(paladdr,	regval);
		for(j=0; j<200; j++)
			;
	}
	
	return VIM_SUCCEED;
}



/***********************************************************************************
Description:
		 set tv display controller palette.
		
Parameters:
		num		: number of palette.
		ppalette: address to store palette data.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.

Remarks:
		because the clock domains are crossed when these registers are written, user
	must delay 20 cycles after each write operation to these palette registers.
************************************************************************************/
VIM_RESULT VIM_TVDC_SetRGBPalette(UINT16 num, UINT8 *rgb)
{
	UINT16 i=0, j=0;
	UINT32 paladdr=0, regval=0;
	UINT8 temp1,temp2,temp3;
	SINT32 val;
	UINT8 palette[4];


	if(rgb == NULL){
		return VIM_ERROR_TVDC_DATA_INVALID;
	}	
	for(i=0; i<num; i++){
		temp3 = rgb[0+4*i];
		temp2 = rgb[1+4*i];
		temp1 = rgb[2+4*i];

		//Y=0.299R + 0.587G + 0.114B
		val = (77 * temp1 + 150 * temp2 + 29 * temp3) >> 8;	//y
		palette[2] = val > 255 ? 255 : ( val < 0 ? 0 : val );


		//U = -0.147R - 0.289G + 0.436B
		val = ((128 * temp3 - 43 * temp1 - 85 * temp2) >> 8) + (256 >> 1); //u
		palette[1]  = val > 255 ? 255 : ( val < 0 ? 0 : val );

		//V = 0.615R - 0.515G - 0.100B
		val = ((128 * temp1 - 107 * temp2 - 21 * temp3) >> 8) + (256 >> 1); //v
		palette[0] = val > 255 ? 255 : ( val < 0 ? 0 : val );

		//reserved
		palette[3] = 0x0;
		
		
		paladdr = V5_REG_TVDC_PALLETTE+(i<<2);
		regval = (palette[0]|palette[1]<<8|palette[2]<<16|palette[3]<<24);
		//regval  = *(ppalette+i);
		VIM_HIF_SetReg32(paladdr,	regval);
		for(j=0; j<200; j++)
			;
	}
	
	return VIM_SUCCEED;
}
/***********************************************************************************
Description:
		 enable/disable adpcm encoder of tv.
		
Parameters:
		enable	: 1 for enable, 0 for disable.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.

Remarks:
		
************************************************************************************/
VIM_RESULT VIM_TVDC_EnableAdpcm(UINT8 enable)
{
	UINT32	regval=0, delay=0x0000ffff;
	
	if(enable){
		VIM_HIF_SetReg32(V5_REG_ADPCM_CTRL,	1);
		return VIM_SUCCEED;
	}
	else
	{
		regval = VIM_HIF_GetReg32(V5_REG_ADPCM_CTRL);
		if((regval&1) == 0)
			return VIM_SUCCEED;
	
		VIM_HIF_SetReg32(V5_REG_ADPCM_CTRL,	0);

		while(delay){
			regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL );
			if(regval&(1<<4)){
				return VIM_SUCCEED;
			}
			delay--;
		}
		return VIM_ERROR_TVDC_ADPCM_STOP;

	}
}

/***********************************************************************************
Description:
		 set adpcm image size.
		
Parameters:
		size	: image size.
		
Returns:
		void.

Remarks:
		image width must be the multiple of 16; image height must be even.

************************************************************************************/
VIM_RESULT VIM_TVDC_SetAdpcmSize(TSize size)
{
	UINT32 regval;
	UINT32 maxwidth,maxheight;


	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);
	regval >>= 3;
	regval &= 0x01;
	if(regval==0)
	{	/*!< tv format : PAL */
		maxwidth = VIM_TVDC_PAL_MAX_WIDTH;
		maxheight = VIM_TVDC_PAL_MAX_HEIGHT;
	}
	else
	{			/*!< tv format : NTSC */
		maxwidth = VIM_TVDC_NTSC_MAX_WIDTH;
		maxheight = VIM_TVDC_NTSC_MAX_HEIGHT;
	}

	if(size.cx > maxwidth || size.cy >maxheight)
	{
		return VIM_ERROR_TVDC_PCMPARAM_OUTOFRANGE;
	}

	size.cx >>= 4;	/*!< image width must be the multiple of 16 */
	size.cx <<= 4;
	size.cy >>= 1;	/*!< image height must be even */
	size.cy <<= 1;
	
	VIM_HIF_SetReg32(V5_REG_ADPCM_IMG_WIDTH,	size.cx);
	VIM_HIF_SetReg32(V5_REG_ADPCM_IMG_HEIGHT,	size.cy);

	return VIM_SUCCEED;
}

/***********************************************************************************
Description:
		 get adpcm image size.
		
Parameters:
		psize	: address to  store image size.
		
Returns:
		void.

Remarks:
		image width must be the multiple of 16; image height must be even.
************************************************************************************/
void VIM_TVDC_GetAdpcmSize(PTSize psize)
{
	psize->cx	= (UINT16)VIM_HIF_GetReg32(V5_REG_ADPCM_IMG_WIDTH);
	psize->cy	= (UINT16)VIM_HIF_GetReg32(V5_REG_ADPCM_IMG_HEIGHT);
}

/***********************************************************************************
Description:
		 set adpcm quantizer parameter.
		
Parameters:
		stderr	: adpcm quantizer parameter.
		
Returns:
		void.

Remarks:
		other quantizer parameters are calculated from stderr. it only appear in 
	adpcm header, and it doesn't affect the quantizer.
************************************************************************************/
void VIM_TVDC_SetAdpcmConfig(UINT8 stderror)
{
	UINT32 regval=0, tmp=stderror;

	regval = VIM_HIF_GetReg32(V5_REG_ADPCM_CTRL);
	regval &= 0x00ff;
	regval |= (((UINT32)stderror<<8) & 0xff00);
	VIM_HIF_SetReg32(V5_REG_ADPCM_CTRL,	regval);

	regval = ((tmp*504/1000) & 0x01ff);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QSEGMENT14,	regval);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QSEGMENT24,	(((regval*7)>>2) & 0x03ff));
	regval = ((tmp*1181/1000) & 0x01ff);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QSEGMENT15,	regval);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QSEGMENT25,	(((regval*7)>>2) & 0x03ff));
	regval = ((tmp *  2285 / 1000 ) & 0x01ff);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QSEGMENT16,	regval);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QSEGMENT26,	(((regval*7)>>2) & 0x03ff));

	regval = ((tmp*222/1000) & 0x01ff);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QVALUE14,	regval);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QVALUE24,	(((regval*7)>>2) & 0x03ff));
	regval = ((tmp*785/1000) & 0x01ff);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QVALUE15,	regval);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QVALUE25,	(((regval*7)>>2) & 0x03ff));
	regval = ((tmp*1576/1000) & 0x01ff);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QVALUE16,	regval);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QVALUE26,	(((regval*7)>>2) & 0x03ff));
	regval = ((tmp*2994/1000) & 0x01ff);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QVALUE17,	regval);
	VIM_HIF_SetReg32(V5_REG_ADPCM_QVALUE27,	(((regval*7)>>2) & 0x03ff));

	VIM_HIF_SetReg32(V5_REG_ADPCM_QUPDATE,0x01);
}





void VIM_TVDC_EnableCaptureSize(UINT8 able)
{
	UINT32 temp;
	temp = VIM_HIF_GetReg32(V5_REG_IPP_CTRL);

	if(able == ENABLE)
	{
		temp |= (1<<10);
		temp &=( ~(1<<9));	
		temp &=( ~(1<<8));	
		temp &=( ~(1<<11));			
	}
	else
	{
		temp &= (~(1<<10));
		temp &=( ~(1<<9));	
		temp &=( ~(1<<8));	
		temp &=( ~(1<<11));		
	}

	VIM_HIF_SetReg32(V5_REG_IPP_CTRL,temp);

	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE,0x01);
}



/***********************************************************************************
Description:
		Open TV
Parameters:

		mode:
			VIM_TVDC_MODE_COLORBAR:					output colorbar
			
			VIM_TVDC_MODE_NORMAL:						output the image from the sensor

			VIM_TVDC_MODE_STILL:						output sitll image
Returns:
		success	:error message or VM_SUCCEED.

Remarks:
		before call the fun,You must call VIM_TVDC_Init_Parameter
		
************************************************************************************/
void VIM_TVDC_OpenTV(VIM_TVDC_MODE mode) 
{
	//supply tv encoder power
	VIM_TVDC_SupplyPower(TRUE);

	switch(mode)
	{
		case VIM_TVDC_MODE_COLORBAR:
			//do nothing,but let tv encoder output clorbar
			VIM_HIF_SetReg32(V5_REG_TVENC_TV_CTRL, 0x24);
			VIM_TVDC_EnableCaptureSize(ENABLE);
			
			break;
		case VIM_TVDC_MODE_NORMAL:
			VIM_TVDC_EnableCaptureSize(ENABLE);
			VIM_TVDC_EnableAdpcm(ENABLE);
			break;
		case VIM_TVDC_MODE_STILL:
			break;
	}

}




/***********************************************************************************
Description:
		Close TV
Parameters:

		NULL
Returns:
		NULL
Remarks:
		NULL
************************************************************************************/
void VIM_TVDC_CloseTV(void)
{
	VIM_TVDC_EnableAdpcm(DISABLE);
	VIM_TVDC_EnableCaptureSize(DISABLE);
	
	//shut down power
	//VIM_HIF_SetReg32(V5_REG_CPM_VDAC_SLEEP_MCU,1);
	VIM_TVDC_SupplyPower(FALSE);
}


/***********************************************************************************
Description:
		Reset Osd para
Parameters:

		pt:												the new startpoint of the osd
		
		show:
			VIM_TVDC_OSD_SHOW							the osd will be showed on the screen
			VIM_TVDC_OSD_HIDE							the osd will be hiden on the screen
Returns:
		success	:error message or VM_SUCCEED.

Remarks:
		before call the fun,You must call VIM_TVDC_Init_Parameter
		
************************************************************************************/
VIM_RESULT VIM_TVDC_MoveOSD(TPoint pt,VIM_TVDC_OSD_MODE show)
{
	UINT32 regval;
	TSize size;
	VIM_RESULT result;

	if(show == VIM_TVDC_OSD_SHOW)
	{
		regval = VIM_HIF_GetReg32(V5_REG_TVDC_OSD_SIZE);

		size.cx = regval&0x3ff;
		size.cy = (regval>>16)&0x3ff;

		pt.x = ((pt.x>>1)<<1);
		pt.y = ((pt.y>>1)<<1);		

		result =VIM_TVDC_SetOsdPanel(pt,size);
		if(result)
			return result;
	
		VIM_TVDC_EnableOsd(ENABLE);
		
	}

	else if(show == VIM_TVDC_OSD_HIDE)
	{
		VIM_TVDC_EnableOsd(DISABLE);
	}
	else
	{
		return VIM_ERROR_TVDC_OSDMODE_UNKNOWN;
	}
	return VIM_SUCCEED;

}



void VIM_TVDC_ShowBackColor(VIM_TVDC_TVFORMAT fmt)
{	
	TSize size;
	TPoint pt;

	size.cx = size.cy =64;
	VIM_TVDC_EnableCaptureSize(DISABLE);

	VIM_TVDC_SetAdpcmSize(size);

	pt.x=pt.y = 0;
	VIM_TVDC_SetActiveFrameStartPoint(pt);
	VIM_TVDC_EnableOsd(DISABLE);

	
	VIM_TVDC_SetTvFormat(fmt);

	VIM_TVDC_EnableCaptureSize(ENABLE);

	VIM_TVDC_EnableAdpcm(ENABLE);

	VIM_TVDC_SupplyPower(TRUE);

}



void VIM_TVDC_EnableStillMode(void)
{
	UINT32 regval;

	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);

	regval |= (1<<9);

	VIM_HIF_SetReg32(V5_REG_TVDC_CTRL,regval);
}


void VIM_TVDC_EnableVideoMode(void)
{
	UINT32 regval;
	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);

	regval &= ~(1<<9);

	VIM_HIF_SetReg32(V5_REG_TVDC_CTRL,regval);
}


/*
给定一张长度为width.宽度为height的图片
居中显示
*/
VIM_RESULT VIM_TVDC_SetJpegPositionSize(UINT16 width,UINT16 height)
{
	UINT16 screenwidth,screenheight;
	UINT32 regval;
	TSize size;
	TPoint pt;
	
	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);
	regval >>= 3;
	regval &= 0x01;
	if(regval==0)
	{	/*!< tv format : PAL */
		screenwidth = VIM_TVDC_PAL_MAX_WIDTH;
		screenheight = VIM_TVDC_PAL_MAX_HEIGHT;
	}
	else
	{			/*!< tv format : NTSC */
		screenwidth = VIM_TVDC_NTSC_MAX_WIDTH;
		screenheight = VIM_TVDC_NTSC_MAX_HEIGHT;
	}


	if(width > screenwidth || height>screenheight)
		return VIM_ERROR_TVDC_PCMPARAM_OUTOFRANGE;


	size.cx = width;
	size.cy = height;

	VIM_TVDC_SetAdpcmSize(size);


	if(width<=screenwidth/2 &&height<=screenheight/2)
	{
		VIM_TVDC_EnableZoomBypass(VIM_TVDC_ZOOM_ALL,DISABLE);
		width *=2;
		height *=2;
	}
	else
		VIM_TVDC_EnableZoomBypass(VIM_TVDC_ZOOM_ALL,ENABLE);
	

	
	pt.x = (screenwidth-width)/2;
	pt.y = (screenheight - height)/2;


	VIM_TVDC_SetActiveFrameStartPoint(pt);

	
	return VIM_SUCCEED;
}




VIM_RESULT VIM_TVDC_VerifyJpegSize(UINT16 DisplayWidth, UINT16 DisplayHeight)
{
	UINT32 regval;

	
	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);
	regval >>= 3;
	regval &= 0x01;
	if(regval==0)
	{	/*!< tv format : PAL */
		if(DisplayWidth > VIM_TVDC_PAL_MAX_WIDTH || DisplayHeight>VIM_TVDC_PAL_MAX_HEIGHT)
			return VIM_ERROR_TVDC_JPG_OUTOFRANGE;
		
	}
	else
	{			/*!< tv format : NTSC */
		if(DisplayWidth>VIM_TVDC_NTSC_MAX_WIDTH || DisplayHeight>VIM_TVDC_NTSC_MAX_HEIGHT)
			return VIM_ERROR_TVDC_JPG_OUTOFRANGE;
	}

	return VIM_SUCCEED;
}



//color U32 RGB数据，需要转化为YUV格式后设置寄存器
void VIM_TVDC_SetBackColor(UINT32 color)
{
		UINT8 temp1,temp2,temp3,Y,U,V;
		SINT32 val;
		UINT32 aim;

		temp3 = (color>>16)&0xff;
		temp2 = (color>>8)&0xff;
		temp1 = color&0xff;

		//Y=0.299R + 0.587G + 0.114B
		val = (77 * temp1 + 150 * temp2 + 29 * temp3) >> 8;	//y
		Y = val > 255 ? 255 : ( val < 0 ? 0 : val );


		//U = -0.147R - 0.289G + 0.436B
		val = ((128 * temp3 - 43 * temp1 - 85 * temp2) >> 8) + (256 >> 1); //u
		U  = val > 255 ? 255 : ( val < 0 ? 0 : val );

		//V = 0.615R - 0.515G - 0.100B
		val = ((128 * temp1 - 107 * temp2 - 21 * temp3) >> 8) + (256 >> 1); //v
		V = val > 255 ? 255 : ( val < 0 ? 0 : val );

		aim = V|(U<<8)|(Y<<16);

		VIM_HIF_SetReg32(V5_REG_TVDC_BG_COLOR,aim);
		return;
		
}


BOOL VIM_TVDC_IsAdpcmBufferReady()
{
	UINT32 regval;

	regval = VIM_HIF_GetReg32(V5_REG_TVDC_CTRL);

	return (regval&0x01);
}


void VIM_TVDC_SupplyPower(BOOL yes)
{
	if(yes)
	{
		VIM_HIF_SetReg32(V5_REG_CPM_VDAC_SLEEP_MCU,0);
	}
	else
	{
		VIM_HIF_SetReg32(V5_REG_CPM_VDAC_SLEEP_MCU,1);
	}
}


void VIM_TVDC_EnableVdecSync(BOOL yes)
{
	UINT32 regval;
	regval = VIM_HIF_GetReg32(V5_REG_MP4DEC_PINGPANG_MBL);
	
	if(yes)
	{
		regval |= (1<<27);
	}
	else
	{
		regval &= (~(1<<27));
	}
	VIM_HIF_SetReg32(V5_REG_MP4DEC_PINGPANG_MBL,regval);	
}


#endif

/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_TVDC_Driver.h
 * [Description]        : 适用于578b.
 *				    tv_driver.c需要的头文件
 *				    				
 * [Author]                 : jiangkeqiang
 * [Date Of Creation]  : 2007-05-18 10:00
 * [Platform]               : not be limited by platform
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

#ifndef _VIM_5XRDK_TV_H_
#define _VIM_5XRDK_TV_H_


#if VIM_USER_SUPPORT_TV

#ifdef __cplusplus
extern "C" {
#endif

#define	VIM_TVDC_PAL_MINOFFSET_X		124		/*!< min x offset of PAL format TV to display picture */
#define	VIM_TVDC_PAL_MINOFFSET_Y		44		/*!< min y offset of PAL format TV to display picture */
#define	VIM_TVDC_NTSC_MINOFFSET_X		114		/*!< min x offset of NTSC format TV to display picture */
#define	VIM_TVDC_NTSC_MINOFFSET_Y		32		/*!< min x offset of NTSC format TV to display picture */

#define	VIM_TVDC_PAL_MAX_WIDTH			720		/*!< max width of PAL format TV */
#define	VIM_TVDC_PAL_MAX_HEIGHT		576		/*!< max height of PAL format TV */

#define	VIM_TVDC_NTSC_MAX_WIDTH		720		/*!< max width of NTSC format TV */
#define	VIM_TVDC_NTSC_MAX_HEIGHT		480		/*!< max height of NTSC format TV */




typedef enum _ENUM_VIM_TVDC_TVFORMAT
{
	VIM_TVDC_TVFMT_PAL	= 0x00,
	VIM_TVDC_TVFMT_NTSC	= 0x01
}VIM_TVDC_TVFORMAT;


/*! tvdc osd format */
typedef enum _ENUM_VIM_TVDC_OSDFORMAT
{
	VIM_TVDC_OSDFMT_8BPP	= 0x03,
	VIM_TVDC_OSDFMT_4BPP	= 0x07,
	VIM_TVDC_OSDFMT_2BPP	= 0x0f	
}VIM_TVDC_OSDFORMAT;


/*! tvdc zoom type */
typedef enum _ENUM_VIM_TVDC_ZOMMTYPE
{
	VIM_TVDC_ZOOM_HORIZONTAL	= 0x00,
	VIM_TVDC_ZOOM_VERTICAL	= 0x01,
	VIM_TVDC_ZOOM_ALL		= 0x02	
}VIM_TVDC_ZOOMTYPE;



typedef enum _ENUM_VIM_TVDC_MODE
{
	VIM_TVDC_MODE_COLORBAR = 0x00,
	VIM_TVDC_MODE_NORMAL=0x01,
	VIM_TVDC_MODE_STILL = 0x02
}VIM_TVDC_MODE;

typedef enum _ENUM_VIM_TVDC_OSD_MODE
{
	VIM_TVDC_OSD_HIDE = 0x00,
	VIM_TVDC_OSD_SHOW=0x01
}VIM_TVDC_OSD_MODE;


typedef struct _VIM_TVDC_OSD_PARA
{
	UINT32 datalen;
	UINT32 palettelen;
	
	//TSize windowsize;
	UINT16 display_x;
	UINT16 display_y;
	
	//TPoint startpoint;	
	UINT16 startpt_x;
	UINT16 startpt_y;

	VIM_TVDC_OSDFORMAT osdformat;
	UINT8 alpha;
	UINT8 *data;
	UINT8 *palette;
	UINT8 enable;
}VIM_TVDC_OSD_PARA;


typedef struct _VIM_TVDC_PARAMETER
{
	//TSize adpcmsize;
	UINT16 display_x;
	UINT16 display_y;
	
	//TPoint startpoint;
	UINT16 startpt_x;
	UINT16 startpt_y;
	
	VIM_TVDC_OSD_PARA *posd;

	VIM_TVDC_TVFORMAT format;	
}VIM_TVDC_PARAMETER;





void VIM_TVDC_SetOsdAddress(UINT32 addr);
void VIM_TVDC_GetAdpcmSize(PTSize psize);
void VIM_TVDC_SetFrameAddress(UINT32 addr0, UINT32 addr1);

void VIM_TVDC_OpenTV(VIM_TVDC_MODE mode) ;
void VIM_TVDC_CloseTV(void);
void VIM_TVDC_Reset(void);
void VIM_TVDC_EnableCaptureSize(UINT8 able);


VIM_RESULT VIM_TVDC_EnableAdpcm(UINT8 enable);
void VIM_TVDC_EnableOsd(UINT8 enable);
VIM_RESULT VIM_TVDC_SetTvFormat(VIM_TVDC_TVFORMAT format);
VIM_RESULT VIM_TVDC_SetAdpcmSize(TSize size);
VIM_RESULT VIM_TVDC_SetActiveFrameStartPoint(TPoint startpt);
VIM_RESULT VIM_TVDC_SetOsdbppFormat(VIM_TVDC_OSDFORMAT format);
void VIM_TVDC_SetOsdAlphablending(UINT8 alpha);
VIM_RESULT VIM_TVDC_SetOsdPanel(TPoint startpt, TSize size);
VIM_RESULT VIM_TVDC_SetYUVPalette(UINT16 num, UINT8 *ppalette);
VIM_RESULT VIM_TVDC_SetRGBPalette(UINT16 num, UINT8 *rgb);
void VIM_TVDCShowBackColor(VIM_TVDC_TVFORMAT fmt);
VIM_RESULT VIM_TVDC_MoveOSD(TPoint pt,VIM_TVDC_OSD_MODE show);

void VIM_TVDC_EnableStillMode(void);
void VIM_TVDC_EnableVideoMode(void);
VIM_RESULT VIM_TVDC_SetJpegPositionSize(UINT16 width,UINT16 height);
void VIM_TVDC_SupplyPower(BOOL yes);
VIM_RESULT VIM_TVDC_VerifyJpegSize(UINT16 DisplayWidth, UINT16 DisplayHeight);

void VIM_TVDC_SetBackColor(UINT32 color);
BOOL VIM_TVDC_IsAdpcmBufferReady(void);
void VIM_TVDC_EnableVdecSync(BOOL yes);
//---------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif


#endif //VIM_USER_SUPPORT_TV


#endif /* _RDK_TV_H_ */


/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_COMMON_Debug.h
 * [Description]        : 公共文件，用于调试使用
 *                       	   分为错误打印和调试打印两种
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-10 16:01
 * [Platform]               : 平台无关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-10  		angela      		 Created
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
#ifndef _VIM_5XRDK_COMMON_DEBUG_H_
#define _VIM_5XRDK_COMMON_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define		VIM_5XRDK_DEBUG		1
#define		VIM_5XRDK_ERROR		1

#define		VIM_HIGH_API_DEBUG	1
#define		VIM_MID_API_DEBUG		1
#define		VIM_SIF_DEBUG				0
#define		VIM_HIF_DEBUG				0
#define		VIM_LCDIF_DEBUG			0
#define		VIM_IPP_DEBUG				0
#define		VIM_MARB_DEBUG			0
#define		VIM_JPEG_DEBUG			0
#define		VIM_DISP_DEBUG			0
#define		VIM_USER_DEBUG			0
#define		VIM_SENSOR_DEBUG			0
#define		VIM_MPEG4_DEBUG			0



typedef enum _VIM_RESULT
{
/////////////succeed/////////////////////////////////////////
	VIM_SUCCEED=0,
/////////////common error///////1-10//////////////////////////
	VIM_ERROR_TIME_OUT=1,
	VIM_ERROR_PARAMETER=2,
	VIM_ERROR_FILE_END=3,
/////////////high api error 100-200////////////////////////////
	VIM_ERROR_WORKMODE=100,
	VIM_ERROR_PREVIEWMODE=101,
	VIM_ERROR_BUFFERLENGTHS=102,
	VIM_ERROR_NOINITERRUPT=103,
	VIM_ERROR_BRIGHT_CONTRAST=104,
	VIM_ERROR_LCDWORKMODE=105,
	VIM_ERROR_NOPIC_INBUF=106,
	VIM_ERROR_BUFPOINT=107,
	VIM_ERROR_CANNOT_CAPVIDEO=108,
	VIM_ERROR_VIDEO_MODE=109,
	VIM_ERROR_MAX_FRAMECOUNT=110,
	VIM_ERROR_LCD_UPDATERANGE=111,
	VIM_ERROR_LCD_DRAWOVERRANGE=112,
	VIM_ERROR_CANNOTCAPT_FRAMTHUMB=113,
	VIM_ERROR_PREVIEW_PARA=114,
	VIM_ERROR_FASET_PREVIEW=115,
	VIM_ERROR_IPP_CANNOTSUPPORT=116,
	VIM_ERROR_DECODE=117,
	VIM_ERROR_CANNOT_QUICK_DISPLAY=118,
	VIM_ERROR_LCD_SIZE=119,					//wendy guo 4/12/2007
	VIM_ERROR_LCD_POSITION=120,
	VIM_ERROR_MIRROR_NOTSUPPORT=121,
	VIM_ERROR_OVERSIZE_VIDEOCON=122,
	VIM_ERROR_OVERSIZE_DISPLAYONEFRAME=123,	
	VIM_ERROR_FLAGERROR_DISPLAYONEFRAME=124,		
	VIM_ERROR_CANNOTSUPPORT_CHANGESIZE=125,
/////////////mid api error 200-300////////////////////////////
	VIM_ERROR_HIF_READWRITE_REG8=201,
	VIM_ERROR_DEFINE_ENDIAN=202,
	VIM_ERROR_HIF_READWRITE_REG16=203,
	VIM_ERROR_IPP_READWRITE_REG8=204,
	VIM_ERROR_READWRITE_SRAM=205,
	VIM_ERROR_JBUF_INTERRUPT_BLOCK=206,

	VIM_ERROR_PREVIEW_CAPTURE_ZOOM_PARA=210,
	VIM_ERROR_PREVIEW_DISPLAY_ZOOM_PARA=211,

	VIM_ERROR_CAPTURE_MARBADDRESS=212,
	VIM_ERROR_CAPTURE_OVERFLOW=213,	
	VIM_ERROR_CAPTURE_MARB=214,	
	VIM_ERROR_CAPTURE_BUFFULL=215,	
	VIM_ERROR_CAPTURE_ROMOVERFLOW=216,	

	VIM_ERROR_DECODE_DOWNSIZEOVERFLOW=217,	//this add by amanda,because 578 420 height can not down size more than 1/16
	                                                                                    //444,422,410,400 can not downsize more than 1/8
	VIM_ERROR_ADJUST_OFFSET=218,
	VIM_ERROR_PREVIEW_BASE_ZOOM_PARA=219,
	VIM_ERROR_HIF_READWRITE_REG32=220,
	VIM_ERROR_READWRITE_FIFO=221,
	VIM_ERROR_CAPTURE_THUMB_OVERFLOW=222,	

	VIM_ERROR_DISPLAY_NULL=223,	
	VIM_ERROR_DISPLAY_ZOOMOVER=224,	

	VIM_ERROR_CAPTURE_MP4STUFF=225,	
	VIM_ERROR_CAPTURE_LINEBUF=226,	
	VIM_ERROR_READWRITE_SDRAM=227,
	VIM_ERROR_MP4ENC_LBUFF_BLOCKED=228,
	VIM_ERROR_MP4ENC_SENSOR_TOO_FAST=229,
	VIM_ERROR_MP4ENC_LBUFF_VSYNC=230,
	
/////////////sensor(isp) interface error 300-400////////////////////////////
	VIM_ERROR_SENSOR_NOINFO=301,
	VIM_ERROR_SENSOR_AUTOFIND=302,
	VIM_ERROR_SENSOR_RESOLUTION=303,
	VIM_ERROR_SENSOR_INPUTSIZE=304,
	VIM_ERROR_SENSOR_STATE=305,
	VIM_ERROR_SENSOR_NOROSELUTION=306,
	VIM_ERROR_SENSOR_RGB=307,
/////////////lcd interface error 400-500////////////////////////////




/////////////hif(cpm) interface error 500-600////////////////////////////
	VIM_ERROR_PLL_INCLK=501,
	VIM_ERROR_PLL_N=502,
	VIM_ERROR_PLL_M=503,
	VIM_ERROR_PLL_OUT=504,
	VIM_ERROR_PLL_NO=505,
	VIM_ERROR_PLL_MARB=506,
	VIM_ERROR_INTERRUPT_INIT=507,
	VIM_ERROR_INTERRUPT_EN=508,
	
	VIM_ERROR_PLL_LCDPCLOCK=509,					//add by wendy guo
	VIM_ERROR_PLL_ROVER=510,						//add by wendy guo
	VIM_ERROR_PLL_TVOUT=511,						//add by wendy guo
	VIM_ERROR_INTERRUPT_BREAKOFF=512,			//add by wendy guo
/////////////ipp(line buf)  error 600-700////////////////////////////
	VIM_ERROR_DISPLAY_WINDOW=601,
	VIM_ERROR_CAPTURE_WINDOW=602,
	VIM_ERROR_SOURCE_WINDOW=603,
	VIM_ERROR_THUMB_WINDOW=604,
	VIM_ERROR_CACAULATELE_SOURCE_WINDOW=605,		//add by amanda deng
	VIM_ERROR_CAPTURE_PARAMETER=606,			//add by amanda deng
	VIM_ERROR_PREVIEW_PARAMETER=607,			//add by amanda deng
	VIM_ERROR_UP_SOURCE_WINDOW=608,
	VIM_ERROR_UP_CAPTURE_WINDOW=609,
	VIM_ERROR_UP_RESIZE_OVER=610,
	VIM_ERROR_THUMB_HEIGHT_MORE=611,
	VIM_ERROR_LBUF_PARAM_OUTOFRANGE=612,
/////////////marb error 700-800////////////////////////////
	VIM_ERROR_INDEX_MAP=701,
	VIM_ERROR_THUMB_MAP=702,
	VIM_ERROR_LINEBUF_MAP=703,
	VIM_ERROR_JBUF_MAP=704,
	VIM_ERROR_ALAYER_MAP=705,
	VIM_ERROR_B1_MAP=706,
	VIM_ERROR_B0_MAP=707,

	VIM_ERROR_MARB_BIU=708,						//add by wendy guo
	VIM_ERROR_MARB_LBUF=709,						//add by wendy guo
	VIM_ERROR_MARB_TVDC=710,						//add by wendy guo
	VIM_ERROR_MARB_VDEC=711,						//add by wendy guo
	VIM_ERROR_MARB_VENC=712,						//add by wendy guo
	VIM_ERROR_MARB_JPEG=713,						//add by wendy guo
	VIM_ERROR_MARB_IPP=714,						//add by wendy guo
	VIM_ERROR_MARB_ADDRUNKNOW=715,				//add by wendy guo
	VIM_ERROR_MARB_SETOVERLEN=716,				//add by wendy guo
	VIM_ERROR_MARB_DIVERROR=717,				//DIV =0
	VIM_ERROR_MARB_MAP_DISTRIBUTE=718,			//wendy guo 4/12/2007
	VIM_ERROR_MPEG4_CAP=719,
	VIM_ERROR_MPEG4_MAP=720,
	VIM_ERROR_MARB_RAMINFO_TYPE=721,
	VIM_ERROR_MARB_RAMINFO_SRAM_NOSPACE=722,
	VIM_ERROR_MARB_RAMINFO_SDRAM_NOSPACE=723,

/////////////jpeg (line buf)  error 800-900////////////////////////////
	VIM_ERROR_DECODE_ERROR=801,
	VIM_ERROR_ENCODE_ERROR=802,
	VIM_ERROR_DELAY_FRAME=803,
	VIM_ERROR_YUV_UNKNOW=804,
	VIM_ERROR_JPEG_OVERRANGE=805,
	
/////////////disp error 900-1000////////////////////////////
#if 0	
wendy guo 
	VIM_ERROR_DISP_PARAM = 901,
	VIM_ERROR_DISP_MODE = 902,
	VIM_ERROR_DISP_BLAYER = 903,
	VIM_ERROR_DISP_B0B1_SUPERPOS = 904,
#endif


	VM_ERR_LCDC_MODE_UNKNOWN		= 901,
	VM_ERR_LCDC_LAYER_UNKNOWN		= 902,
	VM_ERR_LCDC_ROTATE_UNKNOWN		= 903,
	VM_ERR_LCDC_OVERLAY_UNKNOWN		= 904,
	VM_ERR_LCDC_BFORMAT_UNKNOWN		= 905,
	VM_ERR_LCDC_PARAM_OUTOFRANGE	= 906,
	VM_ERR_LCDC_PANELIF_UNKNOWN		= 907,
	VM_ERR_LCDC_BGCOLORFMT_UNKNOWN	= 908,
	VIM_ERROR_DISP_PARAM = 909,						//wendy guo add
	VIM_ERROR_DISP_BLAYER = 910,						//wendy guo add

/////////////user error 1000-1100////////////////////////////
	VIM_ERROR_USER_FREEMEM=1000,
	VIM_ERROR_USER_READFILE=1001,
	VIM_ERROR_USER_WRITEFILE=1002,
	VIM_ERROR_USER_SEEKFILE=1003,

	VIM_ERROR_USER_STARTTIMER=1004,
	VIM_ERROR_USER_STOPTIMER=1005,




/////////////tv error 1100-1200////////////////////////////	
	VIM_ERROR_TVDC_OSDFMT_UNKNOWN=1100,
	VIM_ERROR_TVDC_TVFMT_UNKNOWN=1101,
	VIM_ERROR_TVDC_ZOOMTYPE_UNKNOWN=1102,
	VIM_ERROR_TVDC_OSDPARAM_OUTOFRANGE=1103,
	VIM_ERROR_TVDC_PCMPARAM_OUTOFRANGE=1104,	


	VIM_ERROR_TVDC_DATA_INVALID=1105,
	VIM_ERROR_TVDC_ADPCM_STOP=1106,
	VIM_ERROR_TVDC_OSDMODE_UNKNOWN=1107,

	VIM_ERROR_TVDC_JPG_OUTOFRANGE=1108,
	VIM_ERROR_TVDC_BMP_FORMAT=1109,

/////////////SDRAM error 1100-1200////////////////////////////	
	VIM_ERR_SDRC_COMMAND_UNKNOWN=1200,
	VIM_ERR_SDRC_INIT_OUTOFTIME=1201,
       VIM_RESULT_RESERVED=0x7fffffff
}VIM_RESULT;








#ifdef __cplusplus
}
#endif

#endif 










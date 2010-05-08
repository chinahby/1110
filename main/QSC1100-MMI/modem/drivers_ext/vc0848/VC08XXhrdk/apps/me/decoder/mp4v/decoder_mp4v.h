/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	decoder_mp4v.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/03/28
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#ifndef _ME_DECODER_MP4V_
#define _ME_DECODER_MP4V_

#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\me_comhead.h"

#if ME_KM_DECODE_MP4V_EN

#if HARDWARE_V0820_VERSION==V0820_MPW_SV2
#include "..\..\..\..\Drivers\MMD_Driver.h"
#endif

#if HARDWARE_V0820_VERSION==V0820_FPGA
#ifndef _WIN32
#include "VIM_Drivers.h"
#endif 

typedef struct 
{
	ME_U32		v_width;
	ME_U32		v_height;
	ME_U32		src_width;
	ME_U32		src_height;
	ME_U32		src_xpos;
	ME_U32		src_ypos;
	ME_U32		disp_width;
	ME_U32		disp_height;
	ME_U32		disp_xpos;
	ME_U32		disp_ypos;
	ME_U32		disp_rotation;
	ME_U32		disp_effect;
	
	ME_U32		dec_start_flag;		
	ME_U32		vop_start_flag;		
	ME_U32		block_start_flag;	
	ME_U32		block_last_flag;	
#ifndef _WIN32
	VIM_HAPI_MP4DecCallBack callback;
#endif
}mp4v_decinfor_t;
extern mp4v_decinfor_t  mp4v_decinfor;
#endif
#endif	//#if ME_KM_DECODE_MP4V_EN

ME_U32 decoder_mp4v_init(void *srcinfor, void *outinfor, void **decodeinfor);
ME_U32 decoder_mp4v_release(void *decodeinfor);
ME_U32 decoder_mp4v_seek(void *decodeinfor, ME_U32 pos);
ME_U32 decoder_mp4v_start(void *decodeinfor);
ME_U32 decoder_mp4v_stop(void *decodeinfor);
ME_U32 decoder_mp4v_resume(void *decodeinfor);
ME_U32 decoder_mp4v_pause(void *decodeinfor);
ME_U32 decoder_mp4v_dst_buffersize(void *decodeinfor, ME_U32 maxsrcsize, ME_U32 *maxdstsize);
ME_U32 decoder_mp4v_decode(void *decodeinfor, char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen, ME_U32*decodelen);
ME_U32 decoder_mp4v_decode_frames(void *decodeinfor, void *src, void *dst);
ME_U32 decoder_mp4v_setoutparam(void *decodeinfor, void *outinfor);

ME_U32 decoder_mp4v_register(void);

#ifdef __cplusplus
}
#endif

#endif 

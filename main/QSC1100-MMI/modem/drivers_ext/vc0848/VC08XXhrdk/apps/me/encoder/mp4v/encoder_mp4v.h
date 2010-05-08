/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	encoder_samr.h
* User for : 	820
* Creat By : 	An Xinfang                                                                  
* CrateDate: 	2007/07/17
* Last modify : An Xinfang 2007-xx-xx     
*
* release history:
* 		
=======================================================================*/

#ifndef _ME_ENCODER_MP4V_
#define _ME_ENCODER_MPV4_

#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\me_comhead.h"


#ifndef _WIN32
#include "VIM_Drivers.h"
#endif

typedef struct 
{
	ME_U32		rec_width;
	ME_U32		rec_height;
	ME_U32		preview_width;
	ME_U32		preview_height;
	ME_U32		preview_xpos;
	ME_U32		preview_ypos;
	ME_U32		last_frame_size;
	ME_U32		key_vop_flag;
	ME_U32		osd_mode_flag;
    	ME_U32		enc_start_flag;		
	ME_U32		vop_start_flag;		
	ME_U32		enc_ticks_base;
	ME_U32		enc_ticks_inc;
	ME_U32 		OneBufferSize;
   	ME_U32 		I_Frame_Estimat_Size;
}mp4v_encinfor_t;

extern mp4v_encinfor_t  mp4v_encinfor;


ME_U32 encoder_mp4v_init(void *recinfor, void* inputinfor, void **encodeinfor);
ME_U32 encoder_mp4v_release(void *encodeinfor);
ME_U32 encoder_mp4v_set_inputparam(void *encodeinfor, void *inputinfor);
ME_U32 encoder_mp4v_start(void *encodeinfor);
ME_U32 encoder_mp4v_stop(void *encodeinfor);
ME_U32 encoder_mp4v_resume(void *encodeinfor);
ME_U32 encoder_mp4v_pause(void *encodeinfor);
ME_U32 encoder_mp4v_src_buffersize(void *encodeinfor, ME_U32 *maxsrcsize);
ME_U32 encoder_mp4v_dst_buffersize(void *encodeinfor, ME_U32 *maxdstsize);
ME_U32 encoder_mp4v_encodeframes(void *encodeinfor, void *src, void *dst);
ME_U32 encoder_mp4v_register(void);

#ifdef __cplusplus
}
#endif

#endif 

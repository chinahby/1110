/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	encoder_samr.c
* User for : 	820
* Creat By : 	Amanda Deng                                                                
* CrateDate: 	2007/07/19
* Last modify : Amanda Deng  2007-xx-xx     
*
* release history:
* 		
=======================================================================*/
#include "encoder_s263.h"

#if ME_KM_ENCODE_S263_EN
#include "..\mp4v\encoder_mp4v.h"

#if ME_KM_OBJ_STACK
static me_encoder_t 	_g_encoder_s263;
#endif

#if HARDWARE_V0820_VERSION==V0820_FPGA
extern void video_encode_callback(ME_U32 flag, ME_U32 framesize);
extern mp4v_encinfor_t  mp4v_encinfor;
/*******************************************************************************************
Description:
	amr encoder initialize
Parameters:
       void *headinfor:		audio information structor point
       void **encodeinfor:	encoder information structor point's point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.12.03		create first version
********************************************************************************************/
ME_U32 encoder_s263_init(void *recinfor, void* inputinfor, void **encodeinfor)
{
	me_video_infor 	  *video_infor;
	me_video_inputinfor *video_inputinfor;
#ifndef _WIN32	
	VIM_HAPI_MP4_ENC_INIT_IN_INFO    encinfo;
	VIM_HAPI_MP4_ENC_INIT_OUT_INFO outinfo;
#endif
	ME_U32  result;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_s263_init()"));
	#endif
	
	(*encodeinfor) = NULL;
	ME_ASSERT_PARAM_RETURN((recinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((inputinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	
	video_infor =(me_video_infor *)recinfor;
	video_inputinfor = (me_video_inputinfor *)inputinfor;
#ifndef _WIN32	
	encinfo.capture_width=video_infor->v_imagewidth;
	encinfo.capture_height=video_infor->v_imageheight;
	encinfo.osd_en=0;
	
	encinfo.Enc_Stuff=0x1ff;					// bit8: 0->8bit 1:->32bit and the lowest 8bits reprensent the stuff  data
	encinfo.Enc_Quality_Level=5; 				//  1-31 the higher, the better image quality.
	encinfo.Enc_Half_Pixsel_Estimation_Enable=1;	//
	encinfo.Enc_Short_HeaderSurpport=1;			//0:not 1: yes def:0
	encinfo.Enc_Mpeg4_ResyncMarker_Disable=1;	//0:not 1: yes	

	encinfo.time_increment=2;					//7.5frame
	encinfo.time_resolution=30;					//29.9
    encinfo.Enc_Out_Bitrate_Ctrl_En=0;

	encinfo.preview_x		= video_inputinfor->v_previewxpos;
	encinfo.preview_y		= video_inputinfor->v_previewypos;
	encinfo.preview_width	= video_inputinfor->v_previewwidth;
	encinfo.preview_height	= video_inputinfor->v_previewheight;
	
	
	
/*	if(mp4v_encinfor.callback!=NULL)
		encinfo.pCallBack= mp4v_encinfor.callback;
	else
		encinfo.pCallBack= video_encode_callback;*/
	
	 //reset encode configuration
	mp4v_encinfor.enc_start_flag 	= TRUE;		//when need start decode mp4 set this flag
	mp4v_encinfor.vop_start_flag 	= TRUE;		//when one frame end, need set this flag
	mp4v_encinfor.enc_ticks_base	= 1000;
	mp4v_encinfor.key_vop_flag	= 1;
	result = VIM_HAPI_CaptureMp4_Init(&encinfo, &outinfo);
	if(result)
	{
		ME_ERROR(("VIM_HAPI_CaptureMp4_Init return = 0x%08X", result));
		return result;
	}
	
	mp4v_encinfor.I_Frame_Estimat_Size = outinfo.I_Frame_Estimat_Size;
	mp4v_encinfor.OneBufferSize = outinfo.OneBufferSize;
#endif
	(*encodeinfor)= (void *)&mp4v_encinfor;
	return ME_SUCCESS;
}
#endif
/*******************************************************************************************
Description:
	amr encoder object register
Parameters:
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.19		create first version
********************************************************************************************/
ME_U32 encoder_s263_register(void)
{
	me_encoder_t	* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_s263_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis	= &_g_encoder_s263;
#else
	pthis	= (me_encoder_t *)me_malloc( sizeof(me_encoder_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif

	encoder_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "s263");
	pthis->init				= encoder_s263_init;
	pthis->release			= encoder_mp4v_release;
	pthis->start				= encoder_mp4v_start;
	pthis->stop				= encoder_mp4v_stop;
	pthis->pause				= encoder_mp4v_pause;
	pthis->resume			= encoder_mp4v_resume;
	pthis->src_buffersize		= encoder_mp4v_src_buffersize;
	pthis->dst_buffersize		= encoder_mp4v_dst_buffersize;
	pthis->encode_frames		= encoder_mp4v_encodeframes;
	pthis->set_inputparam		= encoder_mp4v_set_inputparam;
	return me_encoder_register(pthis);	/*×¢²ás263 encoder*/
}
#endif	//#if ME_KM_ENCODE_S263_EN

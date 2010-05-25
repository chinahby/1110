/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	Muxer_3gp.c
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "muxer_3gp.h"
#include "mmp_rdk.h"
#if (ME_KM_MUXER_3GP_EN|ME_KM_MUXER_MP4_EN)	
#include "..\..\libsrc\mp4\mp4_interface.h"

#if ME_KM_OBJ_STACK
static me_muxer_t		_g_muxer_3gp;
#endif
volatile ME_U32 g_bufferindex_video=0;
volatile ME_U32 g_bufferindex_audio=0;
volatile ME_U32 endduration=0;
/*******************************************************************************************
*aim:
*	it is used to initialize parser itselef.
*
*param:
*	pthis:	plugin pointer.
*	file:	file pointer.
*
*return:
*	return ME_SUCCESS if success, ME_FAILE if failed.
*
*******************************************************************************************/
ME_U32	muxer_3gp_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor)
{
	ME_U32 result = 0;
	mp4_infor_t *file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_init()"));
	#endif	
	
	file = mp4_open(" ",1);
	if(file)
	{
		file->stream			= mediafile;
		file->read_func		= fs_filefunc->read_func;
		file->seek_func		= fs_filefunc->seek_func;
		file->write_func		= fs_filefunc->write_func;
		file->tell_func			= fs_filefunc->tell_func;
		file->read_block_func   = fs_filefunc->read_block_func;
		file->write_block_func   = fs_filefunc->write_block_func;

		result = mp4_write_info(file);
		if(result)
		{
			mp4_close(file);
			(*fileinfor)=NULL;
			return result;
		}
		else
		{
			(*fileinfor)=(void *)file;
			return ME_SUCCESS;
		}
	}
	else
	{
		(*fileinfor)=NULL;
		return ME_FAILE;
	}
}
/******************************************************************************
*aim:
*	it is used to release parser itself.
*
*param:
*	pthis:	plugin pointer.
*	file:	file pointer.
*
*return:
*	return ME_SUCCESS if success, ME_FAILE if failed.
*
******************************************************************************/
ME_U32 muxer_3gp_release(void *fileinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_release()"));
	#endif	

#if VIDEORECORDERIN848
	media_recorder_video_stablebufrelease_smtd();
#endif
	return  mp4_close((mp4_infor_t *)fileinfor);
}


/******************************************************************************
*aim:
*	it is used to return 3gp information.
*
*param:
*	pthis:	plugin pointer.
*	info:	store parser information of 3gp.
*
*return:
*	return ME_SUCCESS if success, ME_FAILE if failed.
*
******************************************************************************/
ME_U32	muxer_3gp_set_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_set_audio_infor()"));
	#endif
	result = mp4_set_audio(file, 
						  audio_infor->a_chn, 
						  audio_infor->a_bits, 
						  audio_infor->a_smplrate,
						  audio_infor->a_frmsize,  
						  audio_infor->a_tickps, 
						  audio_infor->a_duration, 
						  audio_infor->a_codec);
	if(me_match_char32(audio_infor->a_codec, "samr"))
		mp4_set_damr_decoder_config(file, audio_infor->a_others);
	else if(me_match_char32(audio_infor->a_codec, "mp4a"))
		mp4_set_mp4a_decoder_config(file, audio_infor->a_others);
	return result;
}

/******************************************************************************
*aim:
*	it is used to get audio data from 3gp file.
*
*param:
*	pthis:	plugin pointer.
*	file:	file pointer.
*	fifo:	fifo to store audio data.
*
*return:
*	return ME_SUCCESS if success, ME_FAILE if failed.
*
******************************************************************************/
ME_U32	muxer_3gp_write_audio(void *fileinfor,char *audio_buffer, ME_U32 bytes, ME_U32 duration)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_write_audio()"));
	#endif
	
	return mp4_write_audio_frame(file, audio_buffer, bytes, duration);
}

ME_U32	muxer_3gp_write_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_write_audio()"));
	#endif

	return mp4_write_audio_frames(file, (audio_buffer_t *)audio_buffer, g_bufferindex_audio);
	
	//return mp4_write_audio_frames(file, (audio_buffer_t *)audio_buffer);
}

/******************************************************************************
*aim:
*	it is used to return 3gp information.
*
*param:
*	pthis:	plugin pointer.
*	info:	store parser information of 3gp.
*
*return:
*	return ME_SUCCESS if success, ME_FAILE if failed.
*
******************************************************************************/
ME_U32	muxer_3gp_set_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	mp4_infor_t 		*file = (mp4_infor_t *)fileinfor;
#if VIDEORECORDERIN848
	VID_STABLE_BUF_INFO sbl_buf_info;
#endif
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_set_video_infor()"));
	#endif
	
	result = mp4_set_video(file, 
						  1, 
						  video_infor->v_imagewidth, 
						  video_infor->v_imageheight, 
	                                     1000,					
	                                     video_infor->v_duration, 
	                                     video_infor->v_codec);
#if VIDEORECORDERIN848
	sbl_buf_info.stszstableaddr[0]=(ME_U32)file->sram_stsz_table_video[0];
	sbl_buf_info.stszstableaddr[1]=(ME_U32)file->sram_stsz_table_video[1];
	sbl_buf_info.sttsstableaddr[0]=(ME_U32)file->sram_stts_table[0];
	sbl_buf_info.sttsstableaddr[1]=(ME_U32)file->sram_stts_table[1];
	media_recorder_video_stablebufinit_smtd(&sbl_buf_info);
#endif
	if(me_match_char32(video_infor->v_codec, "s263"))
		mp4_set_d263_decoder_config(file, video_infor->v_others);
	else if(me_match_char32(video_infor->v_codec, "mp4v"))
		mp4_set_mp4v_decoder_config(file, video_infor->v_others);
	return result;
}

/******************************************************************************
*aim:
*	it is used to get video data from 3gp file.
*
*param:
*	pthis:	plugin pointer.
*	file:	file pointer.
*	fifo:	fifo to store video data.
*
*return:
*	return ME_SUCCESS if success, ME_FAILE if failed.
*
******************************************************************************/
ME_U32	muxer_3gp_write_video(void *fileinfor, char *video_buffer, ME_U32 bytes, ME_U32 duration,ME_U32 is_key_frame)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_write_video()"));
	#endif
	
	return mp4_write_video_frame(file,video_buffer,bytes,is_key_frame,duration);
}

ME_U32	muxer_3gp_write_video_frames(void *fileinfor, video_stream_infor *video_buffer)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_write_video()"));
	#endif
#if VIDEORECORDERIN848
	return mp4_write_video_frames(file,(video_buffer_t *)video_buffer,g_bufferindex_video);
#else
	return mp4_write_video_frames(file,(video_buffer_t *)video_buffer);
#endif
	
}

/******************************************************************************
*aim:
*	it is used to load 3gp parser plugin.
*
*param:
*	pthis:	nodelist.
*
*return:
*	return ME_SUCCESS if success, ME_FAILE if failed.
*
******************************************************************************/
ME_U32	muxer_3gp_register(void)
{
	me_muxer_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis = &_g_muxer_3gp;
#else
	pthis	= (me_muxer_t *)me_malloc( sizeof(me_muxer_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif
	muxer_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "3gp");
	pthis->init					  	= muxer_3gp_init;
	pthis->release				  	= muxer_3gp_release;
	pthis->set_audio_info			  	= muxer_3gp_set_audio_infor;
	pthis->write_audio_oneframe     		= muxer_3gp_write_audio;
	pthis->set_video_info		         	= muxer_3gp_set_video_infor;
 	pthis->write_video_oneframe     		= muxer_3gp_write_video;
	pthis->write_audio_frames			= muxer_3gp_write_audio_frames;
	pthis->write_video_frames     		= muxer_3gp_write_video_frames;
	return me_muxer_register(pthis);	/*зЂВс3gp muxer*/
}
#endif
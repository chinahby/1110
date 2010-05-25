#include "parser_mp4.h"
#if ME_KM_PARSER_MP4_EN	
#include "..\3gp\parser_3gp.h"

#if ME_KM_OBJ_STACK
static me_parser_t		_g_parser_mp4;
#endif

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

ME_U32	parser_mp4_register(void)
{

	me_parser_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp4_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis	= &_g_parser_mp4;
#else
	pthis	= (me_parser_t*)me_malloc( sizeof(me_parser_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif

	parser_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "mp4");
	
	pthis->init					  	= parser_3gp_init;
	pthis->release				  	= parser_3gp_release;
	pthis->check_file_type			  	= parser_3gp_check_filetype;
	pthis->seek				        	= parser_3gp_seek;
	
	pthis->get_audio_info			  	= parser_3gp_get_audio_infor;
	pthis->get_audio_frame_buffersize	= parser_3gp_get_audio_streambuffersize;
	pthis->get_audio_frame_duration		= parser_3gp_get_audio_duration;
	pthis->read_audio_oneframe     		= parser_3gp_read_audio;
	pthis->read_audio_frames     		= parser_3gp_read_audio_frames;

	pthis->get_video_info		         	= parser_3gp_get_video_infor;
   	pthis->get_video_frame_buffersize	= parser_3gp_get_video_streambuffersize;
   	pthis->get_video_frame_duration		= parser_3gp_get_video_duration;
 	pthis->read_video_oneframe     		= parser_3gp_read_video;
	pthis->read_video_keyframe     		= parser_3gp_read_key_video;
	pthis->read_video_frames     		= parser_3gp_read_video_frames;
	pthis->read_video_keyframes     		= parser_3gp_read_key_videoframes;
	pthis->seek_audio_syntime     		= parser_3gp_seek_audio_syntime;
#if (ME_VERSION2_FOURHANDLE)	
	pthis->read_audio_frames_reg		= parser_3gp_read_audio_frames_reg;
	pthis->read_video_frames_reg		= parser_3gp_read_video_frames_reg;
	pthis->read_audio_frames_post		= parser_3gp_read_audio_frames_post;
	pthis->read_video_frames_post		= parser_3gp_read_video_frames_post;
#endif
	return me_parser_register(pthis);	/*×¢²ámp4 Parser*/
}
#endif

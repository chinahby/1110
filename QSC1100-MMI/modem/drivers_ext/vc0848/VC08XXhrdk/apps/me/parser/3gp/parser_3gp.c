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
#include "parser_3gp.h"
#if (ME_KM_PARSER_3GP_EN|ME_KM_PARSER_MP4_EN)		
#include "..\..\libsrc\mp4\mp4_interface.h"

#if ME_KM_OBJ_STACK
static me_parser_t		_g_parser_3gp;
#endif

/*******************************************************************************************
Description:
	parser 3gp initialize
Parameters:
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
       void **fileinfor:		file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor)
{
	ME_U32 result = 0;
	mp4_infor_t *file;
#if MP4LIB_VERSION2_FOREHANDLE 
//	ME_U32 (*pfilehandle)[MAX_FILE_HANDLE_NUM];
//	pfilehandle = mediafile;
#endif
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_init()"));
	#endif	

	ME_ASSERT_PARAM_RETURN((mediafile!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filefunc!=NULL));

	file = mp4_open(" ",0);
	if(file)
	{
//#if !(MP4LIB_VERSION2_FOREHANDLE)
		file->stream				= mediafile;
//#else
//		file->stream				= (void *)((*pfilehandle)[0]);
//#endif
//by dnn 2008/4/11
#if MP4LIB_VERSION2_FOREHANDLE 
		file->read_block_func  		= fs_filefunc->read_block_func;
		file->write_block_func		= fs_filefunc->write_block_func;
		file->get_offset_sector_func	= fs_filefunc->get_sector_offset_func;
		file->duplicate_handle_func	= fs_filefunc->duplicate_handle_func;
		file->close_handle_func		= fs_filefunc->close_func;
		file->stream_videoindx		= 0;//(void *)((*pfilehandle)[1]);
		file->stream_audiodata		= 0;//(void *)((*pfilehandle)[2]);
		file->stream_videodata		= 0;//(void *)((*pfilehandle)[3]);
		file->stream_audioindx		= 0;//(void *)((*pfilehandle)[4]);
		file->videodata_origpos		= 0;
		file->audiodata_origpos		= 0;
		file->videoindx_origpos		= 0;
		file->audioindx_origpos		= 0;

		file->vtrack_counter=0;
		file->atrack_counter=0;

#endif
//end		
		file->read_func			= fs_filefunc->read_func;
		file->seek_func			= fs_filefunc->seek_func;
		file->write_func			= fs_filefunc->write_func;
		file->tell_func				= fs_filefunc->tell_func;

		result = mp4_read_info(file);
		if(result)
		{
			ME_ERROR(("-> mp4_read_info() return error!"));
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
		ME_ERROR(("->mp4_open() return error!"));
		(*fileinfor)=NULL;
		return ME_FAILE;
	}
	
}
/*******************************************************************************************
Description:
	parser close and release
Parameters:
      void *fileinfor: file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32	parser_3gp_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_release()"));
	#endif	
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
  	return  mp4_close((mp4_infor_t *)fileinfor);
}
/*******************************************************************************************
Description:
	check file is good 3gp file or not
Parameters:
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
Return:	
      ME_TRUE:	 Yes
      ME_FALSE: No
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_check_filetype(void *mediafile, fs_callbacks *fs_filefunc)
{
	ME_U32 result = 0;
	mp4_infor_t *file;
#if MP4LIB_VERSION2_FOREHANDLE 
//	ME_U32 (*pfilehandle)[MAX_FILE_HANDLE_NUM];
//	pfilehandle = mediafile;
#endif
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_check_filetype()"));
	#endif	
	
	ME_ASSERT_PARAM_RETURN((mediafile!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filefunc!=NULL));
	
	file = mp4_open("", 0);
	if(file)
	{
//#if !(MP4LIB_VERSION2_FOREHANDLE)
		file->stream				= mediafile;
//#else
//		file->stream				= (void *)((*pfilehandle)[0]);
//#endif
//by dnn 2008/4/11
#if MP4LIB_VERSION2_FOREHANDLE 
		file->read_block_func  		= fs_filefunc->read_block_func;
		file->write_block_func		= fs_filefunc->write_block_func;
		file->get_offset_sector_func	= fs_filefunc->get_sector_offset_func;
		file->duplicate_handle_func	= fs_filefunc->duplicate_handle_func;
		file->close_handle_func		= fs_filefunc->close_func;
		file->stream_videoindx		= 0;//(void *)((*pfilehandle)[1]);
		file->stream_audiodata		= 0;//(void *)((*pfilehandle)[2]);
		file->stream_videodata		= 0;//(void *)((*pfilehandle)[3]);
		file->stream_audioindx		= 0;//(void *)((*pfilehandle)[4]);
		file->videodata_origpos		= 0;
		file->audiodata_origpos		= 0;
		file->videoindx_origpos		= 0;
		file->audioindx_origpos		= 0;
#endif
//end		
		file->read_func	= fs_filefunc->read_func;
		file->seek_func	= fs_filefunc->seek_func;
		file->write_func	= fs_filefunc->write_func;
		file->tell_func		= fs_filefunc->tell_func;
		result = mp4_check_sig(file);
		mp4_close(file);
		return  result;
	}
	ME_ERROR(("->mp4_open() return error!"));
	return ME_FALSE;
}

/*******************************************************************************************
Description:
	get 3gp file audio information
Parameters:
	void *fileinfor: file information structor create by parser
	me_audio_infor *audio_infor: audio information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_get_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	mp4_infor_t 		*file = (mp4_infor_t *)fileinfor;
	char *compressor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_get_audio_infor()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_infor!=NULL));
	
	audio_infor->a_isvalid 	= file->total_atracks;
	//audio_infor->a_isvalid = 0;
	if(!audio_infor->a_isvalid)
		return ME_SUCCESS;
	audio_infor->a_totalframe = file->total_asamples;
	audio_infor->a_totaltime	= file->total_atime;
	
 	mp4_audio_bits(file, &audio_infor->a_bits);
 	audio_infor->a_smpllen = audio_infor->a_bits / 8;
  	mp4_audio_channels(file, &audio_infor->a_chn);
	mp4_audio_sample_rate(file, &audio_infor->a_smplrate);
	mp4_audio_compressor(file, &compressor);
	me_memcpy((ME_PS08)audio_infor->a_codec, compressor, 4);
	
	mp4_audio_stsz_frame_size(file,&audio_infor->a_frmsize);
	mp4_audio_sample_duration(file, 0, &audio_infor->a_duration);
	mp4_audio_time_scale(file, &audio_infor->a_tickps);
	mp4_get_audio_decoder_config(file, &audio_infor->a_others, &(audio_infor->a_otherslen));
	
	return ME_SUCCESS;

}
/*******************************************************************************************
Description:
	get audio max frame size
Parameters:
	void *fileinfor: file information structor create by parser
	ME_U32 *framesize: max audio frame size
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_get_audio_streambuffersize(void *fileinfor, ME_U32 *framesize, void* audio_info)
{
	ME_U32 result=0;
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_get_audio_max_frame_size()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((framesize!=NULL));
	
	result = mp4_audio_max_frame_size(file,framesize);
	if(result)
		return result;
	*framesize +=1024;
	return result;
}
/*******************************************************************************************
Description:
	get audio frame duration(ms)
Parameters:
	void *fileinfor: file information structor create by parser
	ME_U32 frame: frame index(from 0)
	ME_U32 * duration: frame duration(ms)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_get_audio_duration(void *fileinfor, ME_U32 frame, ME_U32 *duration)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_get_audio_duration()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((duration!=NULL));
	
    	return mp4_audio_sample_duration(file, frame, duration);
}

/*******************************************************************************************
Description:
	read one frame audio data form 3gp file
Parameters:
	void *fileinfor: file information structor create by parser
	char *audio_buffer: dest buffer point
	ME_U32 *readbytes: really read data length(bytes)
	ME_U32 bufferlength:buffer max leangth(bytes)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_read_audio(void *fileinfor, char *audio_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	ME_ASSERT_PARAM_RETURN((readbytes!=NULL));
	
	result = mp4_read_audio_frame(file, audio_buffer, bufferlength, readbytes);
	switch(result)
	{
		ME_MESSAGE(("->mp4_read_audio_frames() return =0x%08X!", result));
		case MP4_DEMUX_END_OF_AUDIO:
			return ME_FILE_END_OF_AUDIO;
		case MP4_MUX_DEMUX_HAVE_NO_AUDIO:
			return ME_ERR_HAVE_NO_AUDIO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_AUDIO:
			return ME_FILE_LAST_OF_AUDIO;
		default:
			return result;	
	}
	
}

/*******************************************************************************************
Description:
	read one frame audio data form 3gp file
Parameters:
	void *fileinfor: file information structor create by parser
	char *audio_buffer: dest buffer point
	ME_U32 *readbytes: really read data length(bytes)
	ME_U32 bufferlength:buffer max leangth(bytes)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.17		create first version
********************************************************************************************/
#if !(ME_VERSION2_FOURHANDLE)
ME_U32	parser_3gp_read_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = mp4_read_audio_frames(file, (audio_buffer_t *)audio_buffer);
	switch(result)
	{
		ME_MESSAGE(("->mp4_read_audio_frames() return =0x%08X!", result));
		case MP4_DEMUX_END_OF_AUDIO:
			return ME_FILE_END_OF_AUDIO;
		case MP4_MUX_DEMUX_HAVE_NO_AUDIO:
			return ME_ERR_HAVE_NO_AUDIO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_AUDIO:
			return ME_FILE_LAST_OF_AUDIO;
		default:
			return result;	
	}
	
}
#else	
ME_U32	parser_3gp_read_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

//	result = mp4_read_audio_frame(file, (char *)(audio_buffer->buffer_address), audio_buffer->buffer_size, &(audio_buffer->data_length));
	result = mp4_read_audio_frame_ver2(file, (audio_buffer_t *)audio_buffer);
	switch(result)
	{
		ME_MESSAGE(("->mp4_read_audio_frames() return =0x%08X!", result));
		case MP4_DEMUX_END_OF_AUDIO:
			return ME_FILE_END_OF_AUDIO;
		case MP4_MUX_DEMUX_HAVE_NO_AUDIO:
			return ME_ERR_HAVE_NO_AUDIO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_AUDIO:
			return ME_FILE_LAST_OF_AUDIO;
		default:
			return result;	
	}
}

ME_U32	parser_3gp_read_audio_frames_reg(void *fileinfor, audio_stream_infor *audio_buffer, ME_U32 reg, ME_U32 bit)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = mp4_read_audio_frame_ver3(file, (audio_buffer_t *)audio_buffer, reg, bit);
	switch(result)
	{
		ME_MESSAGE(("->mp4_read_audio_frames() return =0x%08X!", result));
		case MP4_DEMUX_END_OF_AUDIO:
			return ME_FILE_END_OF_AUDIO;
		case MP4_MUX_DEMUX_HAVE_NO_AUDIO:
			return ME_ERR_HAVE_NO_AUDIO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_AUDIO:
			return ME_FILE_LAST_OF_AUDIO;
		default:
			return result;	
	}
}

ME_U32	parser_3gp_read_audio_frames_post(void *fileinfor, audio_stream_infor *audio_buffer)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result=0;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = mp4_read_audio_frame_ver3_post(file, (audio_buffer_t *)audio_buffer);
	switch(result)
	{
		ME_MESSAGE(("->mp4_read_audio_frames() return =0x%08X!", result));
		case MP4_DEMUX_END_OF_AUDIO:
			return ME_FILE_END_OF_AUDIO;
		case MP4_MUX_DEMUX_HAVE_NO_AUDIO:
			return ME_ERR_HAVE_NO_AUDIO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_AUDIO:
			return ME_FILE_LAST_OF_AUDIO;
		default:
			return result;	
	}
}
#endif
/*******************************************************************************************
Description:
	get 3gp file video information
Parameters:
	void *fileinfor: file information structor create by parser
	me_video_infor *video_infor: video information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_get_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	mp4_infor_t 	*file = (mp4_infor_t *)fileinfor;
	char 		*compressor;
	unsigned int otherslen=0;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_get_video_infor()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_infor!=NULL));
	
	video_infor->v_isvalid = file->total_vtracks;
	if(!video_infor->v_isvalid)
		return ME_SUCCESS;
	ME_DEBUG(("file->video_srcwidth = %d", file->video_srcwidth));
	ME_DEBUG(("file->video_srcheight = %d", file->video_srcheight));
	ME_DEBUG(("file->video_vopwidth = %d", file->video_vopwidth));
	ME_DEBUG(("file->video_vopheight = %d", file->video_vopheight));
	
	video_infor->v_imagewidth = file->video_vopwidth;
	video_infor->v_imageheight = 	file->video_vopheight;
	video_infor->v_totalframe = file->total_vsamples;
	video_infor->v_totaltime = file->total_vtime;
	
	mp4_video_frame_rate(file, &video_infor->v_fps);
	mp4_video_time_scale(file, &video_infor->v_tickps);
	mp4_video_sample_duration(file, 0, &video_infor->v_duration);
	mp4_video_compressor(file, &compressor);
	me_memcpy((ME_PS08)video_infor->v_codec ,compressor, 4);
	mp4_get_video_decoder_config(file, &video_infor->v_others, &otherslen);
	return ME_SUCCESS;
}
/*******************************************************************************************
Description:
	get video max frame size
Parameters:
	void *fileinfor: file information structor create by parser
	ME_U32 *framesize: max video frame size
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_get_video_streambuffersize(void *fileinfor, ME_U32 *framesize)
{
	ME_U32 result = 0;
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_get_video_max_frame_size()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((framesize!=NULL));

	result = mp4_video_max_frame_size(file, framesize);
	if(result)
		return result;
	*framesize +=1024;
	return result;
}
/*******************************************************************************************
Description:
	get video frame duration(ms)
Parameters:
	void *fileinfor: file information structor create by parser
	ME_U32 frame: frame index(from 0)
	ME_U32 * duration: frame duration(ms)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_get_video_duration(void *fileinfor, ME_U32 frame, ME_U32 *duration)
{
	ME_U32 result = 0;
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_get_video_duration()"));
	#endif	
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((duration!=NULL));

	result = mp4_video_sample_duration(file, frame, duration);
	return result;
}

/*******************************************************************************************
Description:
	read one frame video data form 3gp file
Parameters:
	void *fileinfor: file information structor create by parser
	char *video_buffer: dest buffer point
	ME_U32 *readbytes: really read data length(bytes)
	ME_U32 bufferlength:buffer max leangth(bytes)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_read_video(void *fileinfor, char *video_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_video()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));
	ME_ASSERT_PARAM_RETURN((readbytes!=NULL));
	
	result = mp4_read_video_frame(file, video_buffer, bufferlength, readbytes);
	switch(result)
	{
		ME_MESSAGE(("->mp4_read_video_frames() return =0x%08X!", result));
		case MP4_DEMUX_END_OF_VIDEO:
			return ME_FILE_END_OF_VIDEO;
		case MP4_MUX_DEMUX_HAVE_NO_VIDEO:
			return ME_ERR_HVAE_NO_VIDEO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_VIDEO:
			return ME_FILE_LAST_OF_VIDEO;
		default:
			return result;	
	}
}

#if !(ME_VERSION2_FOURHANDLE)	
ME_U32	parser_3gp_read_video_frames(void *fileinfor, video_stream_infor *video_buffer)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_video()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));

	result = mp4_read_video_frames(file, (video_buffer_t *)video_buffer);
	switch(result)
	{
		ME_MESSAGE(("->mp4_read_video_frames() return =0x%08X!", result));
		case MP4_DEMUX_END_OF_VIDEO:
			return ME_FILE_END_OF_VIDEO;
		case MP4_MUX_DEMUX_HAVE_NO_VIDEO:
			return ME_ERR_HVAE_NO_VIDEO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_VIDEO:
			return ME_FILE_LAST_OF_VIDEO;
		default:
			return result;	
	}
}
#else
ME_U32	parser_3gp_read_video_frames(void *fileinfor, video_stream_infor *video_buffer)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_video()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));

	result = mp4_read_video_frame_ver2(file, (video_buffer_t *)video_buffer);
	switch(result)
	{
		ME_MESSAGE(("->mp4_read_video_frames() return =0x%08X!", result));
		case MP4_DEMUX_END_OF_VIDEO:
			return ME_FILE_END_OF_VIDEO;
		case MP4_MUX_DEMUX_HAVE_NO_VIDEO:
			return ME_ERR_HVAE_NO_VIDEO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_VIDEO:
			return ME_FILE_LAST_OF_VIDEO;
		default:
			return result;	
	}
}

ME_U32	parser_3gp_read_video_frames_reg(void *fileinfor, video_stream_infor *video_buffer, ME_U32 reg, ME_U32 bit)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_video()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));

	result = mp4_read_video_frame_ver3(file, (video_buffer_t *)video_buffer,reg,bit);
	switch(result)
	{
		ME_MESSAGE(("->mp4_read_video_frames() return =0x%08X!", result));
		case MP4_DEMUX_END_OF_VIDEO:
			return ME_FILE_END_OF_VIDEO;
		case MP4_MUX_DEMUX_HAVE_NO_VIDEO:
			return ME_ERR_HVAE_NO_VIDEO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_VIDEO:
			return ME_FILE_LAST_OF_VIDEO;
		default:
			return result;	
	}
}
	
ME_U32	parser_3gp_read_video_frames_post(void *fileinfor, video_stream_infor *video_buffer)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_video()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));

	result = mp4_read_video_frame_ver3_post(file, (video_buffer_t *)video_buffer);
	switch(result)
	{
		ME_MESSAGE(("->mp4_read_video_frames_ver3_post() return =0x%08X!", result));
		case MP4_DEMUX_END_OF_VIDEO:
			return ME_FILE_END_OF_VIDEO;
		case MP4_MUX_DEMUX_HAVE_NO_VIDEO:
			return ME_ERR_HVAE_NO_VIDEO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_VIDEO:
			return ME_FILE_LAST_OF_VIDEO;
		default:
			return result;	
	}
}
#endif

/*******************************************************************************************
Description:
	read one I  frame video data form 3gp file
Parameters:
	void *fileinfor: file information structor create by parser
	char *video_buffer: dest buffer point
	ME_U32 *readbytes: really read data length(bytes)
	ME_U32 bufferlength:buffer max leangth(bytes)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_read_key_video(void *fileinfor, char *video_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_key_video()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));
	ME_ASSERT_PARAM_RETURN((readbytes!=NULL));
	
	result = mp4_read_key_video_frame(file, video_buffer, bufferlength, readbytes);

	switch(result)
	{
		case MP4_DEMUX_END_OF_VIDEO:
			return ME_FILE_END_OF_VIDEO;
		case MP4_MUX_DEMUX_HAVE_NO_VIDEO:
			return ME_ERR_HVAE_NO_VIDEO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_VIDEO:
			return ME_FILE_LAST_OF_VIDEO;
		default:
			return result;	
	}
}

ME_U32	parser_3gp_read_key_videoframes(void *fileinfor, video_stream_infor *video_buffer)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_key_video()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));
	
	result = mp4_read_key_video_frames(file, (video_buffer_t*)video_buffer);

	switch(result)
	{
		case MP4_DEMUX_END_OF_VIDEO:
			return ME_FILE_END_OF_VIDEO;
		case MP4_MUX_DEMUX_HAVE_NO_VIDEO:
			return ME_ERR_HVAE_NO_VIDEO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		case MP4_DEMUX_LAST_OF_VIDEO:
			return ME_FILE_LAST_OF_VIDEO;
		default:
			return result;	
	}}


/*******************************************************************************************
Description:
	 random access time of the mp4 file
Parameters:
        void *fileinfor: mp4 infor stuctor point (include file handle)
        ME_U32 pos: user want time(ms)
	 ME_U32* accesspos: really access time (ms)
Return:	
	ME_SUCCESS:	no error;
      Other Value: error information;
NOTE:
      seek pos ticks is S; ****** modify 2007-04-19
      other type file seek function input pos and output accesspos is S, so mp4 modify to S
      seek pos ticks is ms; ****** modify 2007-08-27
      with new version API release seek function input pos and output accesspos is ms 
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
********************************************************************************************/
ME_U32	parser_3gp_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32  audioframe;
	ME_U32  result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_seek()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((videoframe!=NULL));
	
	result = mp4_random_access_time(file, pos, accesspos, &audioframe,videoframe);
	switch(result)
	{
		case MP4_DEMUX_END_OF_AUDIO:
			return ME_FILE_END_OF_AUDIO;
		case MP4_DEMUX_END_OF_VIDEO:
			return ME_FILE_END_OF_VIDEO;
		case MP4_MUX_DEMUX_HAVE_NO_AUDIO:
			return ME_ERR_HAVE_NO_AUDIO;
		case MP4_MUX_DEMUX_HAVE_NO_VIDEO:
			return ME_ERR_HVAE_NO_VIDEO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		default:
			return result;	
	}
}

ME_U32	parser_3gp_seek_audio_syntime(void *fileinfor, ME_U32 pos)
{
	mp4_infor_t *file = (mp4_infor_t *)fileinfor;
	ME_U32 audioframe;
	ME_U32  result;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_seek()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	
	result = mp4_random_access_audiotime(file, pos, &audioframe);
	switch(result)
	{
		case MP4_DEMUX_END_OF_AUDIO:
			return ME_FILE_END_OF_AUDIO;
		case MP4_DEMUX_END_OF_VIDEO:
			return ME_FILE_END_OF_VIDEO;
		case MP4_MUX_DEMUX_HAVE_NO_AUDIO:
			return ME_ERR_HAVE_NO_AUDIO;
		case MP4_MUX_DEMUX_HAVE_NO_VIDEO:
			return ME_ERR_HVAE_NO_VIDEO;
		case MP4_DEMUX_READ_FILE_ERROR:
			return ME_ERR_READ_FILE_ERROR;
		default:
			return result;	
	}
}

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
Description:
	parser 3gp register
Parameters:
     	No
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
ME_U32	parser_3gp_register(void)
{
	me_parser_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis	= &_g_parser_3gp;
#else
	pthis	= (me_parser_t*)me_malloc( sizeof(me_parser_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif

	parser_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "3gp");
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
	return me_parser_register(pthis);	/*зЂВс3gp Parser*/
}
#endif //#if ME_KM_PARSER_3GP_EN

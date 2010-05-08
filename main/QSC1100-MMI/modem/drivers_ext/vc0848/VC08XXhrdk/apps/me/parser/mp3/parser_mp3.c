#include "parser_mp3.h"
#include	"mmd_mm.h"
#if ME_KM_PARSER_MP3_EN	
#include "mp3_parser.h"
//#include "..\3gp\parser_3gp.h"
#if ME_KM_OBJ_STACK
static me_parser_t	_g_parser_mp3;
#endif

//#define MP3_CHECK_FILE_TYPE_LENGTH	(8192 + 1024)
#define MP3_CHECK_FILE_TYPE_LENGTH	20*1024  //modify by yjz 2009/3/6
#define MP3_CHECK_FILE_SEEK_LENGTH	4096

ME_U32	parser_mp3_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor)
{
	unsigned int result = 0;
	mp3_infor_t *file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_init()"));
	#endif	

	file = (mp3_infor_t *)mp3_init();
	if(file)
	{
		file->stream		= mediafile;
		file->read_func	= fs_filefunc->read_func;
		file->seek_func	= fs_filefunc->seek_func;
		file->write_func	= fs_filefunc->write_func;
		file->tell_func		= fs_filefunc->tell_func;
		(file->read_block_func) = (fs_filefunc->read_block_func);
		(file->write_block_func) = (fs_filefunc->write_block_func);
		(file->get_offset_sector_func) = (fs_filefunc->get_sector_offset_func);

		result = mp3_read_info(file);
			
		if(result)
		{
			mp3_close(file);
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

ME_U32	parser_mp3_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor)
{
	mp3_infor_t *file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_release()"));
	#endif
	
	file = (mp3_infor_t *)fileinfor;	
	mp3_close(file);		
	
	return ME_SUCCESS;
}

ME_U32	parser_mp3_check_filetype(void *mediafile, fs_callbacks *fs_filefunc)
{
	ME_PU08 pBuf = NULL;
	ME_S32 iret = 0;
	ME_S32 buflen = 0;
	ME_U32 filelen = 0;

	buflen = MP3_CHECK_FILE_TYPE_LENGTH;
	
	pBuf = MMD_Malloc(MP3_CHECK_FILE_TYPE_LENGTH);
	if(pBuf == NULL)
		return ME_ERR_MALLOC_MEMORY_ERROR;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_check_filetype()"));
	#endif
	
	fs_filefunc->seek_func(mediafile,0L,SEEK_END);
	filelen = fs_filefunc->tell_func(mediafile);

	//: Add by antonliu, 2008/06/04
	filelen = 0;
	fs_filefunc->seek_func(mediafile,filelen/2,SEEK_SET);

	fs_filefunc->read_func(mediafile,(char*)pBuf,MP3_CHECK_FILE_TYPE_LENGTH);

	//: Modified by antonliu, 2008/06/03
	//iret = SearchSyncword(pBuf,&buflen);
	iret = search_syncword(pBuf,&buflen);

	MMD_Free(pBuf);

	if(iret)
	{
		me_printf("this file is not an mp3 file!!!\n");
		return ME_FALSE;
	}
	else 
	{
		me_printf("this file is an mp3 file!!!\n");
		return ME_TRUE;
	}
		
}

ME_U32	parser_mp3_get_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	mp3_infor_t *mp3_file;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_get_audio_infor()"));
	#endif
	
	mp3_file = fileinfor;
	audio_infor->a_isvalid	= 0x01; 
	audio_infor->a_bits 		= mp3_file->a_bits;
	audio_infor->a_chn  		= mp3_file->a_chn;
	audio_infor->a_frmsize 	= mp3_file->a_frmsize;
	audio_infor->a_smplrate  	= mp3_file->a_smplrate;
	audio_infor->a_smpllen 	= mp3_file->a_smplcount;
	audio_infor->a_totaltime   = mp3_file->a_totaltime*1000;
	audio_infor->a_bitrate	= mp3_file->a_bitrate;
	me_memcpy((ME_PS08)audio_infor->a_codec, (ME_PS08)mp3_file->a_codec, 4);	
	
	audio_infor->a_codec[0]='m';
	audio_infor->a_codec[1]='p';
	audio_infor->a_codec[2]='3';
	audio_infor->a_codec[3]='.';	
	audio_infor->a_others = mp3_file->mp3DecInfo;
	audio_infor->a_otherslen = sizeof(MP3DecInfo);

	return ME_SUCCESS;
}

ME_U32	parser_mp3_get_audio_max_frame_size(void *fileinfor, ME_U32 *framesize, void* audio_info)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_get_audio_max_frame_size()"));
	#endif
	
	*framesize = 1024*6;
	return ME_SUCCESS;
}

ME_U32	parser_mp3_get_audio_duration(void *fileinfor, ME_U32 frame, ME_U32 * duration)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_get_audio_duration()"));
	#endif
	*duration = 30;
	
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	 get the time the audio frame
Parameters:
        file: aac infor stuctor point (include file handle)
	 frame: audio frame id
        *time: audio frame start time
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.02		create first version
********************************************************************************************/
ME_U32	parser_mp3_audio_time_of_frame(void *fileinfor, ME_U32  frame, ME_U32  *time)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_audio_time_of_frame()"));
	#endif	
	*time = 30;
	
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	 get the frame id of the time(ms)
Parameters:
        file: aac infor stuctor point (include file handle)
	 time: audio frame start time
        *frame: audio frame id
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.24		create first version
********************************************************************************************/
ME_U32	parser_mp3_audio_frame_of_time(void *fileinfor, ME_U32  time, ME_U32  *frame)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_audio_frame_of_time()"));
	#endif	
	
	*frame = 1;

	return ME_SUCCESS;
}


ME_U32	parser_mp3_read_audio(void *fileinfor, char *audio_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	mp3_infor_t *file;
	file = fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_read_audio()"));
	#endif
	
	mp3_readaudio(file,audio_buffer,readbytes,bufferlength);
	if(*readbytes == 0)
		return ME_FILE_END_OF_AUDIO;
	
	if(*readbytes < bufferlength)
	{
		//memset(audio_buffer+(*readbytes),0,(bufferlength-*readbytes));
		*readbytes = bufferlength;
	}

	return ME_SUCCESS;
}

ME_U32	parser_mp3_read_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
#if 1
	mp3_infor_t *file = (mp3_infor_t *)fileinfor;
	ME_U32 result;
	//ME_U32 audiostatus;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = mp3_read_audio_frame_ver2(file, (audio_stream_infor *)audio_buffer);
	
	return result;	
#else
	mp3_infor_t *file = NULL;
	file = (mp3_infor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_read_frames()"));
	#endif
	audio_buffer->data_length = file->read_func (file->stream,(char*)audio_buffer->buffer_address,audio_buffer->buffer_size);
	//mp3_readaudio(file,(char *)(audio_buffer->buffer_address+audio_buffer->cursor_write),
	//	&(audio_buffer->data_length),
	//	audio_buffer->buffer_size-audio_buffer->cursor_write);

	if(audio_buffer->data_length < audio_buffer->buffer_size)
		return ME_FILE_END_OF_AUDIO;
	
	audio_buffer->isnot_empty = 1;	
	audio_buffer->is_over = 0;	
	audio_buffer->cursor_read = 0;
	audio_buffer->cursor_write =0;
	
	if(audio_buffer->data_length < audio_buffer->buffer_size)
	{
		audio_buffer->data_length = (audio_buffer->buffer_size);
		audio_buffer->isnot_empty = 1;
		audio_buffer->is_over = 1;	
	}
	
	//uart_printf("rd data\n");
	
	return ME_SUCCESS;
#endif
}


ME_U32	parser_mp3_read_audio_frames_reg(void *fileinfor, audio_stream_infor *audio_buffer, ME_U32 reg, ME_U32 bit)
{
	mp3_infor_t *file = (mp3_infor_t *)fileinfor;
	ME_U32 result;
	//ME_U32 audiostatus;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = mp3_read_audio_frame_ver3(file, (audio_stream_infor *)audio_buffer, reg, bit);

#if MP3_DECODE_MULTI_FRAME	
	if(result < ME_FILE_END_OF_AUDIO)
	{
		//printf("audio_buffer->data_length=%d\n, isover=%d\n", audio_buffer->data_length, audio_buffer->is_over);
		media_player_audio_fillsrcbuf_smtd(bit, audio_buffer->data_length, 0, audio_buffer->is_over);
	}
#endif
	return result;	
}

ME_U32	parser_mp3_read_audio_frames_post(void *fileinfor, audio_stream_infor *audio_buffer)
{
	mp3_infor_t *file = (mp3_infor_t *)fileinfor;
	ME_U32 result=0;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = mp3_read_audio_frame_ver3_post(file, (audio_stream_infor *)audio_buffer);
	return result;
}


ME_U32	parser_mp3_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe)
{
	ME_U32 seek_len = 0;
	mp3_infor_t *file = NULL;
	file = fileinfor;
	pos /= 1000;

	if(pos > (file->a_totaltime))
		pos = (file->a_totaltime);
	
	seek_len = (ME_U64)file->a_bitrate*128*pos;

	file->uloffset = (ME_U32)seek_len+(ME_U32)file->first_frame_offset;
	if(file->uloffset >= file->file_length)
		file->uloffset = file->file_length - 1024;
	file->seek_func(file->stream,file->uloffset,SEEK_SET);
	*accesspos = pos*1000;
	
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	get mp3 file video information
Parameters:
	void *fileinfor: file information structor create by parser
	me_video_infor *video_infor: video information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.24		create first version
********************************************************************************************/
ME_U32	parser_mp3_get_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_get_video_infor()"));
	#endif

	video_infor->v_isvalid = 0;
	return ME_SUCCESS;
}

ME_U32	parser_mp3_register(void)
{
	me_parser_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis	= &_g_parser_mp3;
#else
	pthis	= (me_parser_t*)MMD_Malloc( sizeof(me_parser_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif
	
	parser_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "mp3");
	
	pthis->init					  	= parser_mp3_init;
	pthis->release				  	= parser_mp3_release;
	pthis->check_file_type			  	= parser_mp3_check_filetype;
	pthis->seek				        	= parser_mp3_seek;
	
	pthis->get_audio_info			  	= parser_mp3_get_audio_infor;
	pthis->get_audio_frame_buffersize	= parser_mp3_get_audio_max_frame_size;
	pthis->get_audio_frame_duration		= parser_mp3_get_audio_duration;
	pthis->read_audio_oneframe    		= parser_mp3_read_audio;
	pthis->read_audio_frames			= parser_mp3_read_frames;

	pthis->get_video_info		         	= parser_mp3_get_video_infor;

#if 0
	pthis->read_audio_frames_reg		= parser_3gp_read_audio_frames_reg;
	pthis->read_audio_frames_post		= parser_3gp_read_audio_frames_post;
#else
	pthis->read_audio_frames_reg		= parser_mp3_read_audio_frames_reg;
	pthis->read_audio_frames_post		= parser_mp3_read_audio_frames_post;
#endif
	
	return me_parser_register(pthis);	/*×¢²ámp3 Parser*/
}
#endif

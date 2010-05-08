/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	Parser_jpg.c
* User for : 	820
* Creat By : 	Zhou Jian()                                                                   
* CrateDate: 	2007/2/25
* Last modify : Zhou jian 2007-07-19     
*
* release history:
* 		version 0.1 release by Zhou jian
=======================================================================*/
#include "parser_jpg.h"
#if ME_KM_PARSER_JPG_EN
#include "jpg_demux.h"

#if ME_KM_OBJ_STACK
static me_parser_t	_g_parser_jpg;
#endif

/*******************************************************************************************
Description:
	parser jpg initialize
Parameters:
       void *fs_filehandle:		file handle(point)
       fs_callbacks *fs_filefunc:	file operate interface structor point
       void **fileinfor:			file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1                  Zhou Jian                        2007.07.19
********************************************************************************************/
ME_U32	parser_jpg_init(void *fs_filehandle, fs_callbacks *fs_filefunc,void **fileinfor)
{
	unsigned int result = 0;
	jpg_infor_t *file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_init()"));
	#endif	

	file = (jpg_infor_t *)jpg_init();
	if(file)
	{
		file->stream		= fs_filehandle;
		file->read_func	= fs_filefunc->read_func;
		file->seek_func	= fs_filefunc->seek_func;
		file->write_func	= fs_filefunc->write_func;
		file->tell_func		= fs_filefunc->tell_func;
		
		file->preload_size=0;
		file->file_position=0;
		file->file_length=0;
		file->preload_buffer=NULL;

		file->file_position=0;
		file->is_jpg=1;
		file->YUVType=JPG_UNKNOWN;
			
		result = jpg_read_info(file);
			
		if(result)
		{
			jpg_close(file);
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
	   0.1			Amanda Deng			2007.2.25		create first version
********************************************************************************************/
ME_U32 parser_jpg_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_release()"));
	#endif
	jpg_close((jpg_infor_t *)fileinfor);
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	check file is good aac file or not
Parameters:
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
Return:	
      ME_TRUE:	 Yes
      ME_FALSE: No
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Yin Zhang			2007.2.25		create first version
********************************************************************************************/

ME_U32	parser_jpg_check_filetype(void *mediafile, fs_callbacks *fs_filefunc)
{
	int result = 0;
	jpg_infor_t *file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_check_filetype()"));
	#endif
	
	file = (jpg_infor_t *)jpg_init();
	if(file)
	{
		file->stream		= mediafile;
		file->read_func	= fs_filefunc->read_func;
		file->seek_func	= fs_filefunc->seek_func;
		file->write_func	= fs_filefunc->write_func;
		file->tell_func		= fs_filefunc->tell_func;
		file->file_position	= 0;
		result = jpg_check_sig(file);
		jpg_close(file);
		return  result;
	}
	return ME_FALSE;

}

/*******************************************************************************************
Description:
	get amr file audio information
Parameters:
	void *fileinfor: file information structor create by parser
	me_audio_infor *audio_infor: audio information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Yin Zhang			        2007.2.25		create first version
	   0.1                  Zhou Jian                               2007.07.19
********************************************************************************************/

ME_U32	parser_jpg_get_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	char	jpgCodec[4] = {'j','p','g',0};
	jpg_infor_t * jpg_file;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_get_video_infor()"));
	#endif
	jpg_file = (jpg_infor_t *)fileinfor;
	video_infor->v_imagewidth=jpg_file->ImageWidth;
	video_infor->v_imageheight=jpg_file->ImageHeight;
	//video_infor->v_imagewidth=jpg_file->JpgWidth;
	//video_infor->v_imageheight=jpg_file->JpgHeight;
	if(me_cmp((ME_PU08)jpg_file->v_codec, (ME_PU08)jpgCodec, 3)==0)
	{
		video_infor->v_isvalid=3;
	}
	else
	{
		video_infor->v_isvalid=4;
	}
	video_infor->v_others=(void *)jpg_file;
	me_memcpy((ME_PS08)video_infor->v_codec, jpg_file->v_codec, 4);	
	return ME_SUCCESS;
}

ME_U32	parser_jpg_get_video_srcbuffersize(void *fileinfor, ME_U32 *framesize)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_get_audio_max_frame_size()"));
	#endif
#if	(CHIP_ID==ID_VC0824)
	*framesize = 148*1024;
#else
	*framesize = 64*1024;
#endif

	return ME_SUCCESS;
}

ME_U32	parser_jpg_get_video_duration(void *fileinfor, ME_U32 frame, ME_U32 *duration)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_get_video_duration()"));
	#endif	
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((duration!=NULL));
	
	return 0;
}

ME_U32	parser_jpg_seek_audio_syntime(void *fileinfor, ME_U32 pos)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_seek_audio_syntime()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	return 0;
}

ME_U32	parser_jpg_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_seek()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((videoframe!=NULL));
	*accesspos = 0;
	*videoframe = 0;
	return jpg_seek(fileinfor, pos, accesspos);
}


/*******************************************************************************************
Description:
	read one frame audio data form aac file
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
	   0.1			Yin Zhang			2007.2.25		create first version
********************************************************************************************/

ME_U32	parser_jpg_read_image(void *fileinfor, char *video_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	jpg_infor_t * jpg_file;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_read_image()"));
	#endif
	jpg_file=(jpg_infor_t *)fileinfor;
	jpg_read_image(jpg_file,video_buffer,readbytes,bufferlength);
	if(*readbytes == 0)
		return ME_FILE_END_OF_VIDEO;
	return ME_SUCCESS;
}

ME_U32	parser_jpg_read_image_frames(void *fileinfor, video_stream_infor *video_buffer)
{
	ME_U32 result;
	jpg_infor_t * jpg_file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_read_image_frames()"));
	#endif
	jpg_file=(jpg_infor_t *)fileinfor;
	video_buffer->frame_count = 1;
	video_buffer->data_length = 0;
	result =jpg_read_image(jpg_file,
				     (char *)video_buffer->buffer_address,
				     &video_buffer->data_length,
				     video_buffer->buffer_size);
	return result;
}

/*******************************************************************************************
Description:
	get amr file video information
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
ME_U32	parser_jpg_get_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_get_audio_infor()"));
	#endif
	audio_infor->a_isvalid = 0;
	return ME_SUCCESS;
}
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
Description:
	parser aac register
Parameters:
     	No
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.2.25		create first version
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

ME_U32	parser_jpg_register()
{

	me_parser_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_jpg_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis	= &_g_parser_jpg;
#else
	pthis	= (me_parser_t*)me_malloc( sizeof(me_parser_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif
	
	parser_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "jpg");
	
	pthis->init					  	= parser_jpg_init;
	pthis->release				  	= parser_jpg_release;
	pthis->check_file_type 		  		= parser_jpg_check_filetype;
	pthis->seek				        	= parser_jpg_seek;
  	pthis->get_audio_info			  	= parser_jpg_get_audio_infor;
	pthis->get_video_info		         	= parser_jpg_get_video_infor;
   	pthis->get_video_frame_buffersize	= parser_jpg_get_video_srcbuffersize;
   	pthis->get_video_frame_duration		= parser_jpg_get_video_duration;
 	pthis->read_video_oneframe     		= parser_jpg_read_image;
	pthis->read_video_keyframe     		= parser_jpg_read_image;
	pthis->read_video_frames     		= parser_jpg_read_image_frames;
	pthis->read_video_keyframes     		= parser_jpg_read_image_frames;
	pthis->seek_audio_syntime     		= parser_jpg_seek_audio_syntime;
	return me_parser_register(pthis);	/*×¢²ájpg Parser*/
}
#endif


/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	Parser_amr.c
* User for : 	820
* Creat By : 	Yin Zhang()                                                                   
* CrateDate: 	2007/2/25
* Last modify : Zhou jian 2007-07-19     
*
* release history:
* 		version 0.1 release by Yin Zhang
=======================================================================*/
#include "parser_amr.h"
#include "..\..\..\..\inc\mmd_file_driver.h"
#if ME_KM_PARSER_AMR_EN
#include "amr_demux.h"

#if ME_KM_OBJ_STACK
static me_parser_t	_g_parser_amr;
#endif

static unsigned short  packed_size[16] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 5, 5, 5, 0, 0, 0, 0};
unsigned int amr_frame_count = 0;
/*******************************************************************************************
Description:
	parser amr initialize
Parameters:
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
       void **fileinfor:		file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Yin Zhang			        2007.02.25		        create first version
	   0.1                  Zhou Jian                               2007.07.19
********************************************************************************************/

ME_U32	parser_amr_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor)
{
	unsigned int result = 0;
	amr_infor_t *file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_init()"));
	#endif	
	amr_frame_count = 0;
	file = (amr_infor_t *)amr_init();
	if(file)
	{
		file->stream		= mediafile;
		file->read_func	= fs_filefunc->read_func;
		file->seek_func	= fs_filefunc->seek_func;
		file->write_func	= fs_filefunc->write_func;
		file->tell_func		= fs_filefunc->tell_func;
		
		file->preload_size=0;
		file->file_position=0;
		file->file_length=0;
		file->preload_buffer=NULL;
		file->framecnt=0;
	
		result = amr_read_info(file);
			
		if(result)
		{
			amr_close(file);
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
	   0.1			Yin Zhang			2007.2.25		create first version
********************************************************************************************/

ME_U32	parser_amr_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_release()"));
	#endif
	amr_close(fileinfor);
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

ME_U32	parser_amr_check_filetype(void *mediafile, fs_callbacks *fs_filefunc)
{
	int result = 0;
	amr_infor_t *file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_check_filetype()"));
	#endif
	
	file = (amr_infor_t *)amr_init();
	if(file)
	{
		file->stream	= mediafile;
		file->read_func	= fs_filefunc->read_func;
		file->seek_func	= fs_filefunc->seek_func;
		file->write_func	= fs_filefunc->write_func;
		file->tell_func	= fs_filefunc->tell_func;
		file->file_position=0;
		result = amr_check_sig(file);
		amr_close(file);
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

ME_U32	parser_amr_get_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	amr_infor_t * amr_file;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_get_audio_infor()"));
	#endif
	amr_file = (amr_infor_t *)fileinfor;
	audio_infor->a_isvalid	= 0x01; 
	audio_infor->a_bits = amr_file->a_bits;
	audio_infor->a_chn 	= amr_file->a_chn;
	audio_infor->a_frmsize 	= amr_file->a_frmsize;
	audio_infor->a_smplrate = amr_file->a_smplrate;
	audio_infor->a_smpllen 	= amr_file->a_smpllen;
	audio_infor->a_totalframe=amr_file->framecnt;
	audio_infor->a_totaltime=amr_file->framecnt*20;

	audio_infor->a_codec[0]='a';//'m';
	audio_infor->a_codec[1]='m';//'i';
	audio_infor->a_codec[2]='r';//'d';
	audio_infor->a_codec[3]='.';//'.';
	
	//me_memcpy((ME_PS08)audio_infor->a_codec, amr_file->a_codec, 4);	
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
	   0.1			Yin Zhang			2007.2.25		create first version
********************************************************************************************/

ME_U32	parser_amr_get_audio_max_frame_size(void *fileinfor, ME_U32 *framesize, void* audio_info)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_get_audio_max_frame_size()"));
	#endif
	amr_getmaxframesize(fileinfor,framesize);
	return ME_SUCCESS;
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
	   0.1			Zhou Jian			       2007.07.19		create first version
********************************************************************************************/

ME_U32	parser_amr_get_audio_duration(void *fileinfor, ME_U32 frame, ME_U32 * duration)
{
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_get_audio_duration()"));
	#endif
	amr_getaudioduration(fileinfor,frame,duration);
	return ME_SUCCESS;
}
/*******************************************************************************************
Description:
	 get the time the audio frame
Parameters:
        file: amr infor stuctor point (include file handle)
	 frame: audio frame id
        *time: audio frame start time
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.02		create first version
********************************************************************************************/
ME_U32	parser_amr_audio_time_of_frame(void *fileinfor, ME_U32  frame, ME_U32  *time)
{
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_audio_time_of_frame()"));
	#endif	
	*time =frame*20;
	return ME_SUCCESS;

}
/*******************************************************************************************
Description:
	 get the frame id of the time(ms)
Parameters:
        file: amr infor stuctor point (include file handle)
	 time: audio frame start time
        *frame: audio frame id
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.24		create first version
********************************************************************************************/
ME_U32	parser_amr_audio_frame_of_time(void *fileinfor, ME_U32  time, ME_U32  *frame)
{

	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_audio_frame_of_time()"));
	#endif	
	*frame = (time/20);
	return ME_SUCCESS;
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

ME_U32	parser_amr_read_audio(void *fileinfor, char *audio_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_read_audio()"));
	#endif
	amr_readaudio(fileinfor,audio_buffer,readbytes,bufferlength);
	if(*readbytes == 0)
		return ME_FILE_END_OF_AUDIO;
	return ME_SUCCESS;
}

ME_U32	parser_amr_read_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	ME_U32 readbytes, result;
	
	audio_buffer->frame_count = 0;
	audio_buffer->data_length = 0;
	result = amr_readaudio(fileinfor,
				     (char *)audio_buffer->buffer_address,
				     &readbytes,
				     audio_buffer->buffer_size);
	audio_buffer->data_length = readbytes;
	if(readbytes == 0)
		return ME_FILE_END_OF_AUDIO;
	return result;
}

/*******************************************************************************************
Description:
	 random access time of the amr file
Parameters:
        void *fileinfor: aac infor stuctor point (include file handle)
        ME_U32 pos: user want time(ms)
	 ME_U32* accesspos: really access time (ms)
Return:	
	ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Yin Zhang			2007.2.25		create first version
********************************************************************************************/

ME_U32	parser_amr_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe)
{
	ME_U32 result;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_seek()"));
	#endif
	result = amr_seek(fileinfor,pos,accesspos);
	if(result == 0)
		return ME_SUCCESS;
	return ME_FAILE;
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
ME_U32	parser_amr_get_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_get_video_infor()"));
	#endif

	video_infor->v_isvalid = 0;
	return ME_SUCCESS;
}


ME_U32 amr_read_audio_frame_ver3(amr_infor_t *file, audio_stream_infor* audio_buffer, ME_U32 reg, ME_U32 bit)
{
	ME_U32 buffer_address;
	ME_U32 start_offset=0;
	ME_U32 readsize;
	ME_U16 ft = 0;
	ME_U32 bufferseek = 0;
	ME_U32 framhead;
	ME_S08* tmp_buf;

	if(file->file_end > 3)
	{
		return ME_FILE_END_OF_AUDIO;
	}
	
	amr_frame_count++;
	tmp_buf = me_malloc(audio_buffer->buffer_size);
	if(tmp_buf == NULL)
	{
		return ME_FAILE;
	}


	readsize = amr_read_data(file, tmp_buf, audio_buffer->buffer_size);

	buffer_address = audio_buffer->buffer_address+audio_buffer->cursor_write;
	start_offset = (buffer_address%4==0)?0:(4-buffer_address%4);
	buffer_address += start_offset;
	audio_buffer->cursor_read = audio_buffer->cursor_write+start_offset;
	audio_buffer->cursor_write += start_offset + readsize;
	audio_buffer->frame_count = 1;	



	if(readsize<audio_buffer->buffer_size)
	{
		audio_buffer->data_length = readsize;
		if(readsize==0)
		{
			audio_buffer->is_over =  1;
			me_free((void*)tmp_buf);
			return ME_FILE_LAST_OF_AUDIO;
		}
		audio_buffer->is_over =  1;
		Vm_WrSram((unsigned int)buffer_address, (char*)tmp_buf, audio_buffer->data_length);
		//file->file_end++;
		me_free((void*)tmp_buf);
		return ME_FILE_LAST_OF_AUDIO;
		//return ME_SUCCESS;
	}
	else
	{
		while(bufferseek<audio_buffer->buffer_size)
		{
			framhead = tmp_buf[bufferseek];
			ft = (framhead>> 3) & 0x0f;
			ft = packed_size[ft];
			//buf+=(ft+1);
			bufferseek+=(ft+1);
		}
		//buf-=bufferseek;
		if(bufferseek>audio_buffer->buffer_size)
		{
			bufferseek-=(ft+1);
			readsize-=bufferseek;
			file->file_position-=readsize;
		}
		file->seek_func(file->stream,file->file_position,SEEK_SET);
		audio_buffer->data_length = bufferseek;

		Vm_WrSram((unsigned int)buffer_address, (char*)tmp_buf, audio_buffer->data_length);
#if 0
		if(frame_count < 10)
		{
			me_printf("frm=%d, buffer_address=0x%x + 0x%x, cursor_read=0x%x, len=0x%x\n", 
				frame_count, audio_buffer->buffer_address+audio_buffer->cursor_write, start_offset, audio_buffer->cursor_read, audio_buffer->data_length);
			for(i=0; i<10; i++)
			{
				me_printf("0x%x ", tmp_buf[i]);
			}
			me_printf("\n");
		}		
#endif
		me_free((void*)tmp_buf);
		return ME_SUCCESS;
	}

	me_free((void*)tmp_buf);
	return ME_SUCCESS;

}
//called after filesys has transfered data on SD to 848core.
ME_U32 amr_read_audio_frame_ver3_post(amr_infor_t *file, audio_stream_infor *audio_buffer)
{
	return ME_SUCCESS;
}


ME_U32	parser_amr_read_audio_frames_reg(void *fileinfor, audio_stream_infor *audio_buffer, ME_U32 reg, ME_U32 bit)
{
	amr_infor_t *file = (amr_infor_t *)fileinfor;
	ME_U32 result;
	//ME_U32 audiostatus;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = amr_read_audio_frame_ver3(file, (audio_stream_infor *)audio_buffer, reg, bit);
	if(result == ME_SUCCESS)
	{
		media_player_audio_fillsrcbuf_smtd(bit, audio_buffer->data_length, 0, audio_buffer->is_over);
	}
	switch(result)
	{
		ME_MESSAGE(("->amr_read_audio_frames() return =0x%08X!", result));
		case ME_FILE_END_OF_AUDIO:
			return ME_FILE_END_OF_AUDIO;
		case ME_ERR_READ_FILE_ERROR:
			return ME_FILE_END_OF_AUDIO;
		case ME_FAILE:
			return ME_FILE_END_OF_AUDIO;
		case ME_FILE_LAST_OF_AUDIO:
			return ME_FILE_END_OF_AUDIO;	
		default:
			return ME_SUCCESS;	
	}
}

ME_U32	parser_amr_read_audio_frames_post(void *fileinfor, audio_stream_infor *audio_buffer)
{
	amr_infor_t *file = (amr_infor_t *)fileinfor;
	ME_U32 result=0;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = amr_read_audio_frame_ver3_post(file, (audio_stream_infor *)audio_buffer);
	return result;
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
	   0.1			Yin Zhang			2007.2.25		create first version
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

ME_U32	parser_amr_register()
{

	me_parser_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis	= &_g_parser_amr;
#else
	pthis	= (me_parser_t*)me_malloc( sizeof(me_parser_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif
	
	parser_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "amr");
	pthis->init					  	= parser_amr_init;
	pthis->release				  	= parser_amr_release;
	pthis->check_file_type 		  		= parser_amr_check_filetype;
	pthis->seek				        	= parser_amr_seek;
	
  	pthis->get_audio_info			  	= parser_amr_get_audio_infor;
	pthis->get_audio_frame_buffersize	= parser_amr_get_audio_max_frame_size;
	pthis->get_audio_frame_duration		= parser_amr_get_audio_duration;
	pthis->read_audio_oneframe     		= parser_amr_read_audio;
	pthis->read_audio_frames     		= parser_amr_read_audio_frames;
	
	pthis->get_video_info		         	= parser_amr_get_video_infor;

	pthis->read_audio_frames_reg		= parser_amr_read_audio_frames_reg;
	pthis->read_audio_frames_post		= parser_amr_read_audio_frames_post;	
	return me_parser_register(pthis);	/*×¢²áamr Parser*/
}
#endif





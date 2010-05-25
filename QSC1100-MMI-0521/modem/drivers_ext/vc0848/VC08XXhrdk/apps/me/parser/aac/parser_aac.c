/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	Parser_aac.c
* User for : 	820
* Creat By : 	Yin Zhang()                                                                   
* CrateDate: 	2007/2/25
* Last modify : Yin Zhang 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Yin Zhang
*		version 0.2 release by Xulei
=======================================================================*/
#include "parser_aac.h"
#include "..\..\..\..\inc\mmd_file_driver.h"
#if ME_KM_PARSER_AAC_EN
#include "aac_file_parser.h"
#include "..\..\libsrc\aac\aacdec.h"
#if ME_KM_OBJ_STACK
static me_parser_t	_g_parser_aac;
#endif
//#define  ACCURATLEY_SEEK 0
#define  READ_MAX_FRAMES 2//24
unsigned int aac_frame_count = 0;
/*******************************************************************************************
Description:
	parser aac initialize
Parameters:
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
       void **fileinfor:		file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Xulei			2007.08.25		create first version
********************************************************************************************/
//#define test_seek_time 1
ME_U32	parser_aac_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor)
{
	unsigned int result = 0;
	aac_infor_t *file;
	
#ifdef test_seek_time
	int	s_time, e_time,l_time;	
#endif	

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_init()"));
	#endif	
	aac_frame_count = 0;

	file = (aac_infor_t *)aac_init();
	if(file==NULL)
	{
		(*fileinfor)=NULL;
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	file->stream		= mediafile;
	file->read_func	= fs_filefunc->read_func;
	file->seek_func	= fs_filefunc->seek_func;
	file->write_func	= fs_filefunc->write_func;
	file->tell_func		= fs_filefunc->tell_func;
//	file->readBuf		= (char *)me_malloc(AAC_READBUF_SIZE);
#ifdef test_seek_time
	s_time = get_system_time_ticks();
#endif	

	result = aac_read_info(file);
		
#ifdef test_seek_time
	e_time = get_system_time_ticks();
	l_time =e_time-s_time;
	me_printf("\nOpen_time=%dms,total_time=%dms\n",l_time/1000,file->a_totaltime);	
#endif	
		
	if(result)
	{
		aac_close(file);
		(*fileinfor)=NULL;
		return result;
	}
	else
	{
		(*fileinfor)=(void *)file;
		return ME_SUCCESS;
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
ME_U32	parser_aac_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor)
{
	aac_infor_t *file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_release()"));
	#endif

	file = (aac_infor_t *)fileinfor;
	aac_close(file);		//add by amanda deng 2007/4/20, becauser memory lost
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
	   0.1			Xulei			2007.7.25		create first version
********************************************************************************************/
ME_U32	parser_aac_check_filetype(void *mediafile, fs_callbacks *fs_filefunc)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("\n->parser_aac_check_filetype()"));
	#endif
	
	HAACParser hAACParser;
	//HAACDecoder hAACDecoder;
	int err;
	char *readBuf;
	unsigned char *readPtr;
	//short *outBuf;
	int bytesRead,bytesLeft;

	//get data from file
	fs_filefunc->seek_func(mediafile, 0, SEEK_SET);	
	readBuf = (char *)me_malloc(768*4);
	bytesRead=fs_filefunc->read_func(mediafile,readBuf,768*4);
	readPtr = (unsigned char *)readBuf;
	bytesLeft = bytesRead;
	fs_filefunc->seek_func(mediafile, 0, SEEK_SET);

	//check file type by parser file head
	hAACParser = AACParserLib_Create();
	if (!hAACParser) 
	{
		me_free(readBuf);	
		return ME_FALSE;
	}
	err = AACParserLib_Open(hAACParser,(unsigned char **) &readPtr, &bytesLeft);	
	if ((err)||(bytesLeft<768*2)) 
	{
		AACParserLib_Release(hAACParser);
		me_free(readBuf);
		return ME_FALSE;
	}
	AACParserLib_Release(hAACParser);
#if 0
	//check file type again by decoding one frame
	readPtr = (unsigned char *)readBuf ;
	bytesLeft = bytesRead;
	outBuf = (short *)me_malloc(4096*2);
	hAACDecoder = (HAACDecoder)me_malloc(AACDecoderGetSize(1));
	if (!hAACDecoder) 
	{
		me_free(readBuf);	
		me_free(outBuf);
		return ME_FALSE;
	}
	AACInitDecoder(hAACDecoder, 1);
	err = AACDecode(hAACDecoder, &readPtr, &bytesLeft, outBuf);
	me_free(hAACDecoder);
	me_free(readBuf);
	me_free(outBuf);
	if(err)
		return ME_FALSE;             
#endif
	return ME_TRUE;
}

/*******************************************************************************************
Description:
	get aac file audio information
Parameters:
	void *fileinfor: file information structor create by parser
	me_audio_infor *audio_infor: audio information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Xulei			       2007.7.25		create first version
********************************************************************************************/

ME_U32	parser_aac_get_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	
	aac_infor_t * aac_file;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_get_audio_infor()"));
	#endif
	
	aac_file = (aac_infor_t *)fileinfor;
	audio_infor->a_isvalid	= 0x01; 
	audio_infor->a_bits = 16;   //16 Î»¿í
	audio_infor->a_chn 	= aac_file->a_chn;
	audio_infor->a_smplrate = aac_file->a_smplrate;
	audio_infor->a_smpllen 	= audio_infor->a_bits*audio_infor->a_chn/8;
	audio_infor->a_frmsize 	= 0;  //±äÖ¡³¤
	audio_infor->a_totaltime = aac_file->a_totaltime;  //ms
	audio_infor->a_bitrate = aac_file->a_bitrate;
	
	{
		audio_infor->a_codec[0]='a';
		audio_infor->a_codec[1]='a';
		audio_infor->a_codec[2]='c';
		audio_infor->a_codec[3]='.';
	}	
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
ME_U32	parser_aac_get_audio_max_frame_size(void *fileinfor, ME_U32 *framesize, void* audio_info)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_get_audio_max_frame_size()"));
	#endif
	*framesize = (768*2)*READ_MAX_FRAMES;
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
	   0.1			Yin Zhang			2007.2.25		create first version
********************************************************************************************/

ME_U32	parser_aac_get_audio_duration(void *fileinfor, ME_U32 frame, ME_U32 * duration)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_get_audio_duration()"));
	#endif

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
//#define test_readaudio_time 1
ME_U32	parser_aac_read_audio(void *fileinfor, char *audio_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
/*
	aac_infor_t *file;
	
#ifdef test_readaudio_time
	int	s_time, e_time,l_time;	
#endif	

	file = fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_read_audio()"));
	#endif
	
#ifdef test_readaudio_time
	s_time = get_system_time_ticks();
#endif	

	
	aac_readaudio(file,audio_buffer,readbytes,bufferlength);
	
	
#ifdef test_readaudio_time
	e_time = get_system_time_ticks();
	l_time =e_time-s_time;
	me_printf("------------------------------------READ_time=%dus\n",l_time);	
#endif	

	if(*readbytes == 0)
		return ME_FILE_END_OF_AUDIO;
*/
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
	   0.1			xulei		2007.09.17		create first version
********************************************************************************************/
ME_U32	parser_aac_read_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	aac_infor_t *file = (aac_infor_t *)fileinfor;
	ME_U32 result;
	ME_U32 readbytes;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	result=aac_readaudio(file,(char *)audio_buffer->buffer_address,&readbytes,audio_buffer->buffer_size);
	audio_buffer->data_length	= readbytes;
	audio_buffer->isnot_empty	= readbytes;
	if(result)
	{
		audio_buffer->is_over = 1;
		return ME_FILE_LAST_OF_AUDIO;
	}
	else
		return ME_SUCCESS;
	
}


/*******************************************************************************************
Description:
	 accurately access time of the aac file
Parameters:
        void *fileinfor: aac infor stuctor point (include file handle)
        ME_U32 pos: user want time(ms)
	 ME_U32* accesspos: really access time (ms)
Return:	
	ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1		    	Xulei			2007.2.25		create first version
********************************************************************************************/
//#if ACCURATLEY_SEEK
/*
ME_U32	parser_aac_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe)
{
#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_seek()"));
#endif
	ME_U32 audioframe;
	aac_infor_t *file;
	HAACParser hAACParser;
	unsigned int seek_frame;
	double time_a_frame; //time(s)
	char *readPtr;
	int err;	
	long framecnt=0;
	unsigned int len,nRead;
	int bytesLeft;
	
#ifdef test_seek_time
	int	s_time, e_time,l_time;	
#endif

	file = fileinfor;
	if(file->aac_format == AAC_FF_ADIF)
	{
		file->seek_func(file->stream,0,SEEK_SET);
		*accesspos = 0;
		me_printf("\nADIF FORMAT\n");
		return ME_SUCCESS;
	}
	if(pos > file->a_totaltime ) 
	{
		file->seek_func(file->stream,0,SEEK_SET);
		*accesspos = 0;
		me_printf("\nSeeking time is beyond the length of audio\n");
		return ME_SUCCESS;
	}

	hAACParser = (HAACParser)AACParserLib_Create();
	
	
	file->seek_func(file->stream, 0, SEEK_SET);
	readPtr = (char *)file->readBuf;
	aac_readaudio(file,readPtr,&nRead,AAC_READBUF_SIZE);
	bytesLeft = nRead;
	err = AACParserLib_Open(hAACParser, (unsigned char **)&readPtr, &bytesLeft);
	
	time_a_frame=((float)hAACParser->outputSamps*1000) / ((float)hAACParser->sampRate * hAACParser->nChans);//ms
	seek_frame=(float)pos/time_a_frame;
	
	
#ifdef test_seek_time
	s_time = get_system_time_ticks();
#endif

	while(framecnt < seek_frame)
	{
		err = AACParserLib_ADTSParsing(hAACParser,(unsigned char **)&readPtr, (int *)&bytesLeft);
		framecnt=hAACParser->frameCount;
		if(err!=0)
		{
			len = AAC_READBUF_SIZE-bytesLeft;
			me_memcpy((char *)file->readBuf,(char *)readPtr,bytesLeft);
			readPtr = (char *)file->readBuf;
			aac_readaudio(file,readPtr+bytesLeft,(unsigned int *)&nRead,len);
			
			if(nRead == 0)
				break;

			bytesLeft = AAC_READBUF_SIZE;
		}	
	}
	
	file->seek_func(file->stream, hAACParser->nDataCount, SEEK_SET);
	
#ifdef test_seek_time
	e_time = get_system_time_ticks();
	l_time =e_time-s_time;
	me_printf("\nSeeking process need time: %dms\n",l_time/1000);
#endif

	AACParserLib_Release(hAACParser);

	audioframe=seek_frame;
	* accesspos=pos;
	
	me_printf("\nseek_time=%d\n",*accesspos);	

//-------------------------------------------------
	return ME_SUCCESS;
}
*/
/*******************************************************************************************
Description:
	 random access time of the aac file
Parameters:
        void *fileinfor: aac infor stuctor point (include file handle)
        ME_U32 pos: user want time(ms)
	 ME_U32* accesspos: really access time (ms)
Return:	
	ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1		    	Xulei			2007.2.25		create first version
********************************************************************************************/
//#else
ME_U32	parser_aac_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe)
{
	HAACParser hAACParser;
	aac_infor_t *file = fileinfor;
	char *readBuf;
	unsigned char *readPtr;
	int bytesRead,bytesLeft;	
	long cur_byte;
	long Totalbytes;
	int offset=0;
	int err;
		
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_seek()"));
	#endif

	//ADIF stream can't be seek
	if(file->aac_format == AAC_FF_ADIF)
	{
		file->seek_func(file->stream,0,SEEK_SET);
		*accesspos = 0;
		me_printf("\nADIF FORMAT\n");
		return ME_SUCCESS;
	}

	file->seek_func(file->stream, 0, SEEK_END);
	Totalbytes = file->tell_func(file->stream);
	file->seek_func(file->stream, 0, SEEK_SET);
	if(pos > file->a_totaltime ) 
	{//seek to the end of file
		pos = file->a_totaltime;
		cur_byte = Totalbytes-(768*2)*(READ_MAX_FRAMES+1);
	}
	else
	{ 
		cur_byte = (pos/1000)*file->a_bitrate/8;
		if(cur_byte>Totalbytes-(768*2)*(READ_MAX_FRAMES+1))
		{//To keep two buffer having data
			cur_byte = Totalbytes-(768*2)*(READ_MAX_FRAMES+1);
		}
	}
	//parser the head of ADTS
	readBuf = (char *)me_malloc(768*4);
	bytesRead=file->read_func(file->stream,readBuf,768*4);
	readPtr = (unsigned char *)readBuf;
	bytesLeft = bytesRead;
	hAACParser = AACParserLib_Create();
	err = AACParserLib_Open(hAACParser,(unsigned char **) &readPtr, &bytesLeft);
	
	file->seek_func(file->stream,cur_byte,SEEK_SET);
	bytesRead=file->read_func(file->stream,readBuf,768*4);
	readPtr = (unsigned char *)readBuf;
	bytesLeft = bytesRead;
	do
	{
		readPtr +=offset;
		err = AACParserLib_ADTSParsing(hAACParser,(unsigned char **)&readPtr, (int *)&bytesLeft);
		if(!err) break;
		offset++;
		bytesLeft--;
		if(offset>=768*4)
		{
			offset = 0;
			cur_byte+=768*4;
			file->seek_func(file->stream,cur_byte,SEEK_SET);
			bytesRead=file->read_func(file->stream,readBuf,768*4);
			readPtr = (unsigned char *)readBuf;
			bytesLeft = bytesRead;		
		}	
	}while(err);
	cur_byte += bytesRead-bytesLeft;
	AACParserLib_Release(hAACParser);
	me_free(readBuf);
	*accesspos = pos;
	file->seek_func(file->stream,cur_byte,SEEK_SET);
	return ME_SUCCESS;
}
//#endif
/*******************************************************************************************
Description:
	get aac file video information
Parameters:
	void *fileinfor: file information structor create by parser
	me_video_infor *video_infor: video information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Yin Zhang			2007.2.25		create first version
********************************************************************************************/
ME_U32	parser_aac_get_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_get_video_infor()"));
	#endif

	video_infor->v_isvalid = 0;
	return ME_SUCCESS;
}

ME_U32 aac_read_audio_frame_ver3(aac_infor_t *file, audio_stream_infor* audio_buffer, ME_U32 reg, ME_U32 bit)
{
	ME_U32  result = ME_SUCCESS;
	ME_U32 buffer_address;
	ME_U32  readsize;
//	ME_U32 seek_offset = 0;
//	ME_U32 i = 0;
	ME_U08* tmp_buf;
	
	aac_frame_count++;

	if(file->file_end > 2)
	{
		return ME_FILE_END_OF_AUDIO;
	}
	
	tmp_buf = me_malloc(audio_buffer->buffer_size);
	if(tmp_buf == NULL)
	{
		return ME_FAILE;
	}


	result=aac_readaudio(file, (char *)tmp_buf, &readsize, audio_buffer->buffer_size);

	audio_buffer->data_length	= readsize;
	audio_buffer->isnot_empty	= readsize;

	buffer_address = audio_buffer->buffer_address+audio_buffer->cursor_write;

	audio_buffer->cursor_read = audio_buffer->cursor_write;
	audio_buffer->cursor_write += readsize;
	audio_buffer->frame_count = 1;	

	#if 0
	if( ((F_FILE*)(file->stream))->abspos + ((F_FILE*)(file->stream))->relpos > file->file_length)
	{
		return ME_FILE_LAST_OF_AUDIO;
	}
	#endif
	
	if(result)
	{
		me_printf("###audio_buffer->data_length=0x%x\n", audio_buffer->data_length);
		//file->file_end++;
		audio_buffer->is_over = 1;
		me_free((void*)tmp_buf);
		return ME_FILE_LAST_OF_AUDIO;	
	}
	else
	{
		if(readsize < audio_buffer->buffer_size)	audio_buffer->is_over = 1;
		Vm_WrSram((unsigned int)buffer_address, (char*)tmp_buf, audio_buffer->data_length);
	}

	me_free((void*)tmp_buf);
	return ME_SUCCESS;

}

//called after filesys has transfered data on SD to 848core.
ME_U32 aac_read_audio_frame_ver3_post(aac_infor_t *file, audio_stream_infor *audio_buffer)
{
	return ME_SUCCESS;
}


ME_U32	parser_aac_read_audio_frames_reg(void *fileinfor, audio_stream_infor *audio_buffer, ME_U32 reg, ME_U32 bit)
{
	aac_infor_t *file = (aac_infor_t *)fileinfor;
	ME_U32 result;
	//ME_U32 audiostatus;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = aac_read_audio_frame_ver3(file, (audio_stream_infor *)audio_buffer, reg, bit);
	if(result < ME_FILE_END_OF_AUDIO)
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
			return ME_FILE_LAST_OF_AUDIO;
		default:
			return ME_SUCCESS;	
	}
}

ME_U32	parser_aac_read_audio_frames_post(void *fileinfor, audio_stream_infor *audio_buffer)
{
	aac_infor_t *file = (aac_infor_t *)fileinfor;
	ME_U32 result=0;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_amr_read_audio()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = aac_read_audio_frame_ver3_post(file, (audio_stream_infor *)audio_buffer);
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
	   0.1			Amanda Deng			2007.2.25		create first version
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
ME_U32	parser_aac_register()
{
	me_parser_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_aac_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis	= &_g_parser_aac;
#else
	pthis	= (me_parser_t*)me_malloc( sizeof(me_parser_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif
	
	parser_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "aac");
	
	pthis->init					  	= parser_aac_init;
	pthis->release				  	= parser_aac_release;
	pthis->check_file_type 		  	= parser_aac_check_filetype;
	pthis->seek				        = parser_aac_seek;
	  	
	pthis->get_audio_info			  	= parser_aac_get_audio_infor;
	pthis->get_audio_frame_buffersize	= parser_aac_get_audio_max_frame_size;
	pthis->get_audio_frame_duration		= parser_aac_get_audio_duration;
	pthis->read_audio_oneframe     		= parser_aac_read_audio;	
	pthis->get_video_info		         	= parser_aac_get_video_infor;
	
	pthis->read_audio_frames     		= parser_aac_read_audio_frames;

	pthis->read_audio_frames_reg		= parser_aac_read_audio_frames_reg;
	pthis->read_audio_frames_post		= parser_aac_read_audio_frames_post;	
	
	return me_parser_register(pthis);	/*×¢²áaac Parser*/
}
#endif

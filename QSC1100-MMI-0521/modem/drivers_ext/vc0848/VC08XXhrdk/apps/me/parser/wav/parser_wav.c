/*======================================================================
*                                                                       
*                Copyright (C) 2007 Vimicro CO.,LTD     		 
* File Name: 	Parser_ape.c
* User for : 	820
* Creat By : 	Xulei                                                                   
* CrateDate: 	2007/11/28
* Last modify : Xulei 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Xulei
=======================================================================*/
#include "parser_wav.h"
#if ME_KM_PARSER_WAV_EN
#include "parser_wav_parser.h"
#include "..\..\..\..\inc\mmd_file_driver.h"

#if ME_KM_OBJ_STACK
static me_parser_t	_g_parser_wav;
#endif
unsigned int wav_frame_count;
#define USE_BLOCK_READ	0
#define MAX_SAMPLERATE_TAB 11

unsigned int support_samplerate[MAX_SAMPLERATE_TAB] = {8000, 11025, 12000, 16000, 22050, 
					24000, 32000, 44100, 48000, 88200, 96000};

//#define  READ_MAX_WAV_FRAMES 2//6
/*******************************************************************************************
Description:
	parser wav initialize
Parameters:
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
       void **fileinfor:		file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Xulei			    2007.11.28		 create first version
********************************************************************************************/
#ifndef _PC_SIM
ME_U32	parser_wav_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor)
#else
ME_U32	parser_wav_init(void *mediafile, me_file_t *fs_filefunc,void **fileinfor)
#endif
{
	wav_infor_parser *file;
	int nRetVal = 0;
	WAV_format_chunk * format_chunk;  
	WAV_Data_chunk *data_chunk;
	WAV_fact_chunk *fact_chunk; 
	int i = 0;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_init()"));
	#endif	

	file = (wav_infor_parser*)me_malloc(sizeof(wav_infor_parser));
	if(file==NULL)		
		return 2; //2: malloc error	
	me_memset((ME_PU08)file,0,sizeof(wav_infor_parser));
	wav_frame_count = 0;
	file->stream		= mediafile;
#ifndef _PC_SIM
	file->me_read	= fs_filefunc->read_func;
	file->me_seek	= fs_filefunc->seek_func;
	file->me_write	= fs_filefunc->write_func;
	file->me_tell		= fs_filefunc->tell_func;
#else
	file->me_read	= fs_filefunc->me_read;
	file->me_seek	= fs_filefunc->me_seek;
	file->me_write	= fs_filefunc->me_write;
	file->me_tell		= fs_filefunc->me_tell;
#endif

	(file->read_block_func) = (fs_filefunc->read_block_func);
	(file->write_block_func) = (fs_filefunc->write_block_func);
	(file->get_offset_sector_func) = (fs_filefunc->get_sector_offset_func);	

//add xul
	format_chunk = (WAV_format_chunk *)me_malloc(sizeof(WAV_format_chunk));
	data_chunk = (WAV_Data_chunk *)me_malloc(sizeof(WAV_Data_chunk));//sizeof(WAV_Data_chunk) == 8
	//nRetVal = Parser_wave_head(file,format_chunk,data_chunk);
	fact_chunk = (WAV_fact_chunk *)me_malloc(sizeof(WAV_fact_chunk));
	nRetVal = Parser_wave_head(file,format_chunk,data_chunk,fact_chunk);

	if(nRetVal)
		return nRetVal;

	file->WAV_Channels = format_chunk->Channels;
	file->WAV_SampleRate = format_chunk->SampleRate;
	file->WAV_BitsPerSample = format_chunk->BitPerSample;
	file->WAV_FormatTag = format_chunk->FormatTag;
	file->WAV_BitRate = format_chunk->AvgBytesPerSec*8;
	file->WAV_DataLength = data_chunk->Size; //for adpcm , compress data size
	file->WAV_TotalTime = (ME_U64)data_chunk->Size*1000/format_chunk->AvgBytesPerSec; //ms
	file->WAV_DataStartPoint = file->me_tell(file->stream);
	
	if(format_chunk->FormatTag==WAVE_FORMAT_DVI_ADPCM)
	{
		file->WAV_TotalTime = (ME_U64)fact_chunk->Data*1000/format_chunk->SampleRate; //ms
		file->ima_adpcm.nSamplesPerBlock=format_chunk->nSamplesPerBlock; //sample number per block
		file->ima_adpcm.nBytesPerBlock=format_chunk->BlockAlign;//byte number per block
	}
	*fileinfor  = (void *)file;
	me_free(format_chunk);
	me_free(data_chunk);
	me_free(fact_chunk);
	me_printf("WAV_DataStartPoint=0x%x\n", file->WAV_DataStartPoint);
	if((file->WAV_FormatTag==WAVE_FORMAT_PCM) || (file->WAV_FormatTag==WAVE_FORMAT_DVI_ADPCM))
	{
		if((file->WAV_FormatTag==WAVE_FORMAT_DVI_ADPCM) && (file->WAV_BitsPerSample != 4))
		{
			me_printf("\nillegal WAV_BitsPerSample=%d\n",  file->WAV_BitsPerSample);
			return ME_FAILE;	//	Not 4:1 IMA ADPCM
		}
		for(i=0; i<MAX_SAMPLERATE_TAB; i++)
		{
			if(file->WAV_SampleRate == support_samplerate[i])
				return ME_SUCCESS;
		}
		me_printf("\nillegal WAV_SampleRate=%d\n", file->WAV_SampleRate);
		return ME_FAILE;
	}
	else
		return ME_FAILE;
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
	   0.1			xulei			           2007.11.28		create first version
********************************************************************************************/
#ifndef _PC_SIM
ME_U32	parser_wav_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor)
#else
ME_U32	parser_wav_release(void *fileinfor)
#endif
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_release()"));
	#endif
	
	me_free(fileinfor);		
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	check file is good ape file or not
Parameters:
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
Return:	
      ME_TRUE:	 Yes
      ME_FALSE: No
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Xulei				2007.11.28		create first version
********************************************************************************************/
#ifndef _PC_SIM
ME_U32	parser_wav_check_filetype(void *mediafile, fs_callbacks *fs_filefunc)
#else
ME_U32	parser_wav_check_filetype(void *mediafile, me_file_t *fs_filefunc)
#endif
{
	WAV_RIFF_WAVE_chunk *RIFF_WAVE_chunk;
	ME_PS08 readBuf;
	ME_S32 bytesLeft;
	char cmpBuf1[4]={0x52,0x49,0x46,0x46};	//RIFF
	char cmpBuf2[4]={0x57,0x41,0x56,0x45}; 	//WAVE
	int i;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_check_filetype()"));
	#endif
	
#ifndef _PC_SIM
	fs_filefunc->seek_func(mediafile,0,SEEK_SET);
#else
	fs_filefunc->me_seek(mediafile,0,SEEK_SET);
#endif


	readBuf = (ME_PS08)me_malloc(12);
	RIFF_WAVE_chunk = (WAV_RIFF_WAVE_chunk *)me_malloc(sizeof(WAV_RIFF_WAVE_chunk));
#ifndef _PC_SIM
	bytesLeft=fs_filefunc->read_func(mediafile,readBuf,12);
#else
	bytesLeft=fs_filefunc->me_read(mediafile,readBuf,12);
#endif
	if(bytesLeft != 12)
		return ME_FALSE;
	
	Parser_RIFF_WAVE_Chunk(readBuf,RIFF_WAVE_chunk);
	for(i=0;i<4;i++)
	{
		if((RIFF_WAVE_chunk->ID[i] != cmpBuf1[i])||(RIFF_WAVE_chunk->TYPE[i] != cmpBuf2[i]))
		{
			me_free(readBuf);
			me_free(RIFF_WAVE_chunk);
			return ME_FALSE;
		}
	}
	
	//The file is wav
	me_free(readBuf);
	me_free(RIFF_WAVE_chunk);
#ifndef _PC_SIM
	fs_filefunc->seek_func(mediafile,0,SEEK_SET);
#else
	fs_filefunc->me_seek(mediafile,0,SEEK_SET);	
#endif

	return ME_TRUE;
}

/*******************************************************************************************
Description:
	get ape file audio information
Parameters:
	void *fileinfor: file information structor create by parser
	me_audio_infor *audio_infor: audio information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Xulei			       2007.11.28		create first version
********************************************************************************************/

ME_U32	parser_wav_get_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	
	wav_infor_parser *file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_get_audio_infor()"));
	#endif
	
	file = (wav_infor_parser *)fileinfor;
	
	audio_infor->a_isvalid	= 0x01; 
	audio_infor->a_bits 		= file->WAV_BitsPerSample;
	audio_infor->a_chn 		= file->WAV_Channels;
	audio_infor->a_smplrate   = file->WAV_SampleRate;
	audio_infor->a_smpllen 	= audio_infor->a_bits*audio_infor->a_chn/8;
	audio_infor->a_totaltime   = file->WAV_TotalTime; 
	//fengyuhong common
	//audio_infor->a_bits	= file->WAV_BitRate;
	if(file->WAV_FormatTag == WAVE_FORMAT_PCM)//pcm
	{
#ifndef _PC_SIM
		audio_infor->a_codec[0]='p';
		audio_infor->a_codec[1]='c';
		audio_infor->a_codec[2]='m';
		audio_infor->a_codec[3]='.';
#else
		audio_infor->a_codec[0]='w';
		audio_infor->a_codec[1]='a';
		audio_infor->a_codec[2]='v';	
		audio_infor->a_codec[3]='.';
#endif
	}	
	else if(file->WAV_FormatTag == WAVE_FORMAT_DVI_ADPCM)
	{
		audio_infor->a_codec[0]='i';//'I';
		audio_infor->a_codec[1]='m';//'M';
		audio_infor->a_codec[2]='a';//'A';
		audio_infor->a_codec[3]='.';
		me_printf("\nWAVE_FORMAT_DVI_ADPCM, ima_adpcm.nSamplesPerBlock=0x%x, ima_adpcm.nBytesPerBlock=0x%x\n",( file->ima_adpcm).nSamplesPerBlock, (file->ima_adpcm).nBytesPerBlock);
		audio_infor->a_others=(void *)&(file->ima_adpcm);
		audio_infor->a_otherslen = sizeof(wav_ima_adpcm);
	}
	else
		return ME_FAILE;
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
	   0.1			Xulei    			    2007.11.28		create first version
********************************************************************************************/
ME_U32	parser_wav_get_audio_max_frame_size(void *fileinfor, ME_U32 *framesize, void* audio_info)
{
	wav_infor_parser *file= fileinfor;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_get_audio_max_frame_size()"));
	#endif

	if(file->WAV_FormatTag==WAVE_FORMAT_PCM)
	{
		if(file->WAV_SampleRate < 48000)
			*framesize = (file->WAV_SampleRate/16)*file->WAV_BitsPerSample*file->WAV_Channels/8;//1/8s data
		else
			*framesize = (file->WAV_SampleRate/32)*file->WAV_BitsPerSample*file->WAV_Channels/8;//1/8s data			
	}
	else if(file->WAV_FormatTag==WAVE_FORMAT_DVI_ADPCM)
	{
		*framesize=file->ima_adpcm.nBytesPerBlock;
		//*framesize = *framesize << 2;
	}
#if USE_BLOCK_READ
	*framesize = (*framesize) << 2;
#endif
	me_printf("Get wav max frame size=0x%x\n", *framesize);
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
	   0.1			XULEI			2007.11.28		create first version
********************************************************************************************/

ME_U32	parser_wav_get_audio_duration(void *fileinfor, ME_U32 frame, ME_U32 * duration)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_get_audio_duration()"));
	#endif

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
#ifdef _PC_SIM
ME_U32	parser_wav_audio_time_of_frame(void *fileinfor, ME_U32  frame, ME_U32  *time)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE( ("->parser_wav_audio_time_of_frame()") );
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
ME_U32	parser_wav_audio_frame_of_time(void *fileinfor, ME_U32  time, ME_U32  *frame)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE( ("->parser_wav_audio_frame_of_time()") );
	#endif	
	*frame = 1;
	return ME_SUCCESS;
}
#endif

/*******************************************************************************************
Description:
	read one frame audio data form wav file
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
//just for test currently
ME_U32	parser_wav_read_audio(void *fileinfor, char *audio_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	wav_infor_parser *file= fileinfor;
	ME_S32 readLength;
	ME_S64 readpos;

#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_read_audio()"));
#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	
	if(file->WAV_FormatTag==WAVE_FORMAT_PCM)
	{
		readLength =  ((file->WAV_SampleRate/4)/64)*2*64;//1/2s samples
	    readLength =  readLength*file->WAV_BitsPerSample*file->WAV_Channels/8;//1/2s data
	}
	else if(file->WAV_FormatTag==WAVE_FORMAT_DVI_ADPCM)
	{
		readLength=file->ima_adpcm.nBytesPerBlock;
	}
	
	*readbytes = file->me_read(file->stream,audio_buffer,readLength);
	readpos = file->me_tell(file->stream);
	//	me_printf("Test-------read_length = %d,read_pos = %d\n",readbytes,readpos);
	if(readpos>=file->WAV_DataLength+file->WAV_DataStartPoint)
	{
		return ME_FILE_LAST_OF_AUDIO;
	}
	else
		return ME_SUCCESS;	
}
/*******************************************************************************************
Description:
	read one frame audio data form ape file
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
#ifndef _PC_SIM	//it's new version, midi need to realize this interface
ME_U32	parser_wav_read_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	wav_infor_parser *file= (wav_infor_parser *)fileinfor;
	ME_S32 readbytes;
	ME_S32 readLength;
	ME_S64 readpos;
	ME_U32 buffer_address;
	char* tmp_buf;
	//readLength =  ((file->WAV_SampleRate/4)/64)*2*64;//1/2s samples
	//readLength =  readLength*file->WAV_BitsPerSample*file->WAV_Channels/8;//1/2s data

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_read_audio_frames()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));


	if(file->WAV_FormatTag==WAVE_FORMAT_PCM)
	{
		//readLength =  ((file->WAV_SampleRate/4)/64)*2*64;//1/2s samples
		
		if(file->WAV_SampleRate < 48000)
			readLength =  ((file->WAV_SampleRate/32)/64)*2*64;//1/8s samples
		else
			readLength =  ((file->WAV_SampleRate/64)/64)*2*64;//1/16s samples
			
		readLength =  readLength*file->WAV_BitsPerSample*file->WAV_Channels/8;//1/2s data
		
	}
	else if(file->WAV_FormatTag==WAVE_FORMAT_DVI_ADPCM)
	{
		readLength=file->ima_adpcm.nBytesPerBlock;
		//readLength = readLength << 2;
	}

	
	buffer_address = audio_buffer->buffer_address+audio_buffer->cursor_write;
	tmp_buf = (char*)(me_malloc((ME_U32)readLength));
	readbytes = file->me_read(file->stream, tmp_buf, readLength);

	//readbytes = file->me_read(file->stream,(char *)audio_buffer->buffer_address,readLength);
	audio_buffer->data_length		= readbytes;
	audio_buffer->isnot_empty	= readbytes;
	readpos = file->me_tell(file->stream);

	Vm_WrSram((unsigned int)buffer_address, tmp_buf, audio_buffer->data_length);
/*
	MMD_BIU_Get_Device_Status(reg, &audiostatus);
	audiostatus &= ~(1<<bit);
	MMD_BIU_Set_Device_Status(reg, audiostatus);
*/
	if(readpos>=file->WAV_DataLength+file->WAV_DataStartPoint)
	{
		audio_buffer->is_over = 1;
		me_free((void*)tmp_buf);
		return ME_FILE_LAST_OF_AUDIO;
	}
	else
	{
		me_free((void*)tmp_buf);
		return ME_SUCCESS;	
	}
}
#endif
/*******************************************************************************************
Description:
	 accurately access time of the ape file
Parameters:
        void *fileinfor: ape infor stuctor point (include file handle)
        ME_U32 pos: user want time(ms)
	 ME_U32* accesspos: really access time (ms)
Return:	
	ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1		    	Xulei			2007.11.28		create first version
********************************************************************************************/
#ifndef _PC_SIM
ME_U32	parser_wav_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe)
#else
ME_U32	parser_wav_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *audioframe, ME_U32 *videoframe)
#endif
{
	wav_infor_parser *file= fileinfor;
	ME_S64 cur_read;
	ME_U32 BytePerSample;
	ME_U32 length;
	ME_U32 timeperblock,blocks;
	
#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_seek()"));
#endif

	if(pos > file->WAV_TotalTime)
		pos = file->WAV_TotalTime;  //ms
    
	if(file->WAV_FormatTag==WAVE_FORMAT_PCM)
	{
  		cur_read = file->WAV_DataStartPoint+(ME_S64)pos*(file->WAV_BitRate/8)/1000;

		length = (file->WAV_SampleRate/4)*file->WAV_BitsPerSample*file->WAV_Channels/8;
		if(pos!=0)
		{
			if((cur_read>= file->WAV_DataStartPoint+file->WAV_DataLength-length*2)&&( file->WAV_DataStartPoint+file->WAV_DataLength>=length*2))
				cur_read =  file->WAV_DataStartPoint+file->WAV_DataLength-length*2;
		}
	
		BytePerSample = file->WAV_BitsPerSample/8;
		cur_read = cur_read/BytePerSample*BytePerSample;
		file->me_seek(file->stream,cur_read,SEEK_SET);
	}
	else if(file->WAV_FormatTag==WAVE_FORMAT_DVI_ADPCM)
	{
		timeperblock=file->ima_adpcm.nSamplesPerBlock*10000/file->WAV_SampleRate; //0.1ms
        blocks=pos*10/timeperblock;
		cur_read=blocks*file->ima_adpcm.nBytesPerBlock+file->WAV_DataStartPoint;
        if(cur_read >= (file->WAV_DataStartPoint+file->WAV_DataLength - file->ima_adpcm.nBytesPerBlock)) 
           cur_read=file->WAV_DataStartPoint+file->WAV_DataLength - file->ima_adpcm.nBytesPerBlock;

		file->me_seek(file->stream,cur_read,SEEK_SET);
	}

	*accesspos = pos;
	/*
	{
		int  i =32768000;
		while(i--){}
		me_printf("seek_end_parser\n");
	}
	*/
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	get ape file video information
Parameters:
	void *fileinfor: file information structor create by parser
	me_video_infor *video_infor: video information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			xulei			              2007.11.28		create first version
********************************************************************************************/
ME_U32	parser_wav_get_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_get_video_infor()"));
	#endif

	video_infor->v_isvalid = 0;
	return ME_SUCCESS;
}

ME_U32 wav_read_audio_frame_ver3_post(wav_infor_parser *file, audio_stream_infor *audio_buffer)
{
	return ME_SUCCESS;
}

ME_U32	parser_wav_read_audio_frames_post(void *fileinfor, audio_stream_infor *audio_buffer)
{
	wav_infor_parser *file = (wav_infor_parser *)fileinfor;
	ME_U32 result=0;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_read_audio_frames_post()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = wav_read_audio_frame_ver3_post(file, (audio_stream_infor *)audio_buffer);
	return result;	
}

ME_U32	parser_wav_read_audio_frames_reg(void *fileinfor, audio_stream_infor *audio_buffer, ME_U32 reg, ME_U32 bit)
{
	wav_infor_parser *file= (wav_infor_parser *)fileinfor;
	ME_S32 readbytes;
	ME_S32 readLength;
	ME_S64 readpos;
	char* tmp_buf;
	//ME_U32 audiostatus;
	ME_U32 buffer_address, start_offset, sector_size, real_toreadbytes;
	//readLength =  ((file->WAV_SampleRate/4)/64)*2*64;//1/2s samples
	//readLength =  readLength*file->WAV_BitsPerSample*file->WAV_Channels/8;//1/2s data

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_read_audio_frames()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	wav_frame_count++;
	start_offset = sector_size = real_toreadbytes = 0;
	

	if(file->WAV_FormatTag==WAVE_FORMAT_PCM)
	{
		//readLength =  ((file->WAV_SampleRate/4)/64)*2*64;//1/2s samples
		if(file->WAV_SampleRate < 48000)
			readLength =  ((file->WAV_SampleRate/32)/64)*64;//1/8s samples
		else
			readLength =  ((file->WAV_SampleRate/64)/64)*64;//1/16s samples
		
		readLength =  readLength*file->WAV_BitsPerSample*file->WAV_Channels/8;//1/2s data
		
	}
	else if(file->WAV_FormatTag==WAVE_FORMAT_DVI_ADPCM)
	{
		readLength=file->ima_adpcm.nBytesPerBlock;
		//readLength = readLength << 2;
	}

#if USE_BLOCK_READ
	//get buffer address and buffer size
	buffer_address = audio_buffer->buffer_address+audio_buffer->cursor_write;
	start_offset = (buffer_address%4==0)?0:(4-buffer_address%4);
	buffer_address +=start_offset;// 4x
	buffer_size = audio_buffer->buffer_size-audio_buffer->cursor_write-start_offset;

	frame_sector_offset = file->get_offset_sector_func(file->stream, &sector_size);//offset on sector

	real_toreadbytes = (readLength+frame_sector_offset+(sector_size-1))&(0xffffffff-sector_size+1);//512x or 4x


	
	if(real_toreadbytes> buffer_size)
	{
		me_printf("real_toreadbytes(0x%x)> buffer_size(0x%x)\n", real_toreadbytes, buffer_size);
		return ME_FILE_LAST_OF_AUDIO;
	}
	if( ((F_FILE*)(file->stream))->abspos + ((F_FILE*)(file->stream))->relpos + real_toreadbytes > file->WAV_DataLength)
	{
		audio_buffer->is_over = 1;
		return ME_FILE_LAST_OF_AUDIO;	
	}
	readbytes = file->read_block_func(file->stream, (char*)buffer_address, real_toreadbytes, reg, bit);
	//me_printf("read_block_func, readbytes=0x%x, buffer_address=0x%x, real_toreadbytes=0x%x\n", readbytes, buffer_address, real_toreadbytes);
	if(real_toreadbytes != readLength)
	{
		seek_offset = real_toreadbytes-readLength;
		seek_offset = (((F_FILE*)(file->stream))->abspos + ((F_FILE*)(file->stream))->relpos) - seek_offset;
		file->me_seek(file->stream, seek_offset, F_SEEK_SET);
	}

	if(readbytes < readLength)
	{	
		me_printf("readbytes < readLength\n");
		audio_buffer->is_over = 1;
		return ME_FILE_LAST_OF_AUDIO;
	}

	readoffset = frame_sector_offset;//audio do not need 4x startaddress

	audio_buffer->data_length = readLength;
	

	audio_buffer->cursor_read = audio_buffer->cursor_write+start_offset+readoffset;
	audio_buffer->cursor_write+=start_offset+readoffset+readbytes;	


	audio_buffer->frame_count = 1;
	return ME_SUCCESS;

#else
	buffer_address = audio_buffer->buffer_address+audio_buffer->cursor_write;
	tmp_buf = (char*)(me_malloc((ME_U32)readLength));
	readbytes = file->me_read(file->stream, tmp_buf, readLength);

	//readbytes = file->me_read(file->stream,(char *)audio_buffer->buffer_address,readLength);
	audio_buffer->data_length		= readbytes;
	audio_buffer->isnot_empty	= readbytes;
	readpos = file->me_tell(file->stream);
#if 0
	if(wav_frame_count < 5)
	{
		me_printf("wav_frame_count=0x%x, readbytes=0x%x\n", wav_frame_count, readbytes);
		for(i=0; i<10; i++)
		{
			me_printf("0x%x ", tmp_buf[i]);
		}
		me_printf("\n");
	}
#endif	
	Vm_WrSram((unsigned int)buffer_address, tmp_buf, audio_buffer->data_length);

	if(readpos>=file->WAV_DataLength+file->WAV_DataStartPoint)
	{
		audio_buffer->is_over = 1;
		media_player_audio_fillsrcbuf_smtd(bit, audio_buffer->data_length, 0, audio_buffer->is_over);
		
		me_free((void*)tmp_buf);
		return ME_FILE_LAST_OF_AUDIO;
	}
	else
	{
	
		media_player_audio_fillsrcbuf_smtd(bit, audio_buffer->data_length, 0, 0);
		me_free((void*)tmp_buf);
		return ME_SUCCESS;	
	}
#endif
/**********************************************/
/*

	VM_MIDI_PARSER_INFO *file= (VM_MIDI_PARSER_INFO*)fileinfor;
	ME_U32 readLength;
	ME_U32 readpos;
	ME_U32 total_len=file->mid_DataLength+file->mid_DataStartPointBackup;
	ME_U32 i = 0;
	ME_U08* tmp_addr;
	ME_U32 offset = 0;
	ME_U32 audiostatus;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_read_audio_frames()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	mid_frame_count++;

	if(file->file_end > 3)
	{
		return ME_FILE_END_OF_AUDIO;
	}

	readLength = 1024;

	tmp_addr = me_malloc(readLength);
	
	if (total_len - file->mid_DataStartPoint < readLength ) 
	{
		readLength = total_len - file->mid_DataStartPoint;		
	}
	//me_memcpy((char*)audio_buffer->buffer_address, (char*)(file->mid_Data+file->mid_DataStartPoint), readLength);

	me_memcpy((char*)tmp_addr, (char*)( file->mid_Data + file->mid_DataStartPoint), readLength);

#if 0
	me_printf("\nFrame data: Vm_WrSram addr=0x%x, readLength=0x%x\n", audio_buffer->buffer_address, readLength);
	me_printf("file->mid_DataStartPoint=0x%x\n", file->mid_DataStartPoint);
	for(i=0; i<10; i++)
	{
		me_printf("0x%x ",  tmp_addr[i]);
	}
	me_printf("\n");
#endif

	Vm_WrSram(audio_buffer->buffer_address,  (char*)tmp_addr, readLength);
	
	audio_buffer->data_length		= readLength;
	audio_buffer->isnot_empty	= readLength;

	audio_buffer->cursor_read = audio_buffer->cursor_write;
	audio_buffer->cursor_write += readLength;
	audio_buffer->frame_count = 1;
	
	file->mid_DataStartPoint += readLength;
	readpos = file->mid_DataStartPoint;
	//	me_printf("Test-------read_length = %d,read_pos = %d\n",readbytes,readpos);

	me_free((void*)tmp_addr);
	if(readpos >= total_len)
	{
		last_frame_flag = 1;
		audio_buffer->is_over = 1;
		//audio_buffer->data_length	 = 0;
		MMD_BIU_Get_Device_Status(reg,&audiostatus);
		audiostatus &= ~(1<<bit);
		MMD_BIU_Set_Device_Status(reg, audiostatus);	
	#if 0
		//file->file_end++;
		return ME_SUCCESS;	
	#else
		return ME_FILE_LAST_OF_AUDIO;
		MMD_BIU_Get_Device_Status(reg,&audiostatus);
		if(audiostatus & 0xf == 0xf)
		{
			return ME_FILE_END_OF_AUDIO;
		}
		else
		{
			return ME_SUCCESS;	
		}
	#endif

	}
	else
	{

		MMD_BIU_Get_Device_Status(reg,&audiostatus);
		audiostatus &= ~(1<<bit);
		MMD_BIU_Set_Device_Status(reg, audiostatus);

		return ME_SUCCESS;	
	}
*/
}

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
Description:
	parser wav register
Parameters:
     	No
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Xulei			      2007.11.28		create first version
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

ME_U32	parser_wav_register()
{
	me_parser_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_wav_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis	= &_g_parser_wav;
#else
	pthis	= (me_parser_t*)me_malloc( sizeof(me_parser_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif

#ifndef _PC_SIM	
	parser_register_initialize(pthis);
#endif

	ME_SPRINTF(pthis->id, "wav");
	
	pthis->init					  		= parser_wav_init;
	pthis->release				  		= parser_wav_release;
	pthis->check_file_type 		  		= parser_wav_check_filetype;
	pthis->seek				        	= parser_wav_seek;
  	
	pthis->get_audio_info			  	= parser_wav_get_audio_infor;
	pthis->get_video_info		        = parser_wav_get_video_infor;
	pthis->get_audio_frame_duration		= parser_wav_get_audio_duration;
	pthis->get_audio_frame_buffersize	= parser_wav_get_audio_max_frame_size;
	pthis->read_audio_oneframe     		= parser_wav_read_audio;
	pthis->read_audio_frames     		= parser_wav_read_audio_frames;
	
	pthis->read_audio_frames_reg		= parser_wav_read_audio_frames_reg;
	pthis->read_audio_frames_post		= parser_wav_read_audio_frames_post;
	
	return me_parser_register(pthis);	/*×¢²áwav Parser*/


	
}
#endif























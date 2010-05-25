/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	Muxer_mp3.c
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/11/27
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#include "muxer_mp3.h"
#include "mmd_file_driver.h"
#if ME_KM_MUXER_MP3_EN	


#if ME_KM_OBJ_STACK
static me_muxer_t		_g_muxer_mp3;
#endif

#define 	MUXER_MP3_MESSAGE			0
#define 	MUXER_MP3_DEBUG				0

#if MUXER_MP3_MESSAGE
#define 	muxer_mp3_msg(str)			{me_printf("\nmuxer_mp3_msg:");me_printf str;}
#else
#define 	muxer_mp3_msg(str)
#endif

#if MUXER_MP3_DEBUG
#define 	muxer_mp3_dbg(str)			{me_printf("\nmuxer_mp3_dbg:");me_printf str;}
#else
#define 	muxer_mp3_dbg(str)
#endif

typedef struct
{
	void 	*filehandle;			/* FILE *stream; 820 file maybe not in file system*/
	ME_S32 	total_atime;			/* audio total play time (ms)*/
	ME_S32 	total_asamples;		/* audio total frame*/
	ME_S32  	total_length;		/* file total length */
	ME_S32  	file_position;      		/* Current position of file descriptor */
	ME_S32	(*read_func)(void *fs_filehandle, char* buffer, ME_S32 readbytes);
	ME_S32	(*write_func)(void *fs_filehandle, char* buffer, ME_S32 writebytes);
	ME_S32	(*seek_func)(void *fs_filehandle, ME_S32 offset, ME_S32 mode);
	ME_S32 	(*tell_func)(void *fs_filehandle);
	ME_S32		(*read_block_func)(void *fs_filehandle, char* buffer, ME_S32 readbytes, ME_U32 reg, ME_U32 bit);
	ME_S32		(*write_block_func)(void *fs_filehandle, char* buffer, ME_S32 writebytes, ME_U32 reg, ME_U32 bit);
	ME_S32		(*get_sector_offset_func)(void *fs_filehandle, ME_U32 *sectorsize);
	ME_PVOID	(*duplicate_handle_func)(void *file);
} mp3_recinfor_t;


extern F_FILE *file_rec;
ME_U32	muxer_mp3_init(void *fs_filehandle, fs_callbacks *fs_filefunc,void **fileinfor)
{
	mp3_recinfor_t *new_file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp3_init()"));
	#endif	

	new_file = (mp3_recinfor_t *)me_malloc(sizeof(mp3_recinfor_t));
	if(new_file==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		(*fileinfor)=NULL;
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	
	me_memset((ME_PU08)new_file, 0, sizeof(mp3_recinfor_t));
	new_file->filehandle		= fs_filehandle;
	new_file->read_func		= fs_filefunc->read_func;
	new_file->seek_func		= fs_filefunc->seek_func;
	new_file->write_func		= fs_filefunc->write_func;
	new_file->tell_func			= fs_filefunc->tell_func;
	new_file->read_block_func  = fs_filefunc->read_block_func;
	new_file->write_block_func  = fs_filefunc->write_block_func;
	new_file->file_position		= new_file->seek_func(new_file->filehandle, 0, SEEK_SET);
#if 0  //lily modified
	new_file->file_position		= new_file->write_func(new_file->filehandle, "#!AMR\n", 6);
	if(new_file->file_position!=6)
	{
		me_free(new_file);
		(*fileinfor)=NULL;
		return ME_ERR_WRITE_FILE_ERROR;
	}
	else
#endif
	{
		(*fileinfor)=(void *)new_file;
		return ME_SUCCESS;
	}
}

ME_U32 muxer_mp3_release(void *fileinfor)
{
	mp3_recinfor_t *file = (mp3_recinfor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp3_release()"));
	#endif	
	
	muxer_mp3_msg(("file->total_length = %d", file->total_length));
	muxer_mp3_msg(("file->total_asamples = %d", file->total_asamples));
	muxer_mp3_msg(("file->total_atime = %d", file->total_atime));
	muxer_mp3_msg(("file->file_position = %d", file->file_position));
	
	me_free(file);
	return ME_SUCCESS;
}


ME_U32	muxer_mp3_set_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp3_set_audio_infor()"));
	#endif
	
	return ME_SUCCESS;
}

ME_U32	muxer_mp3_write_audio(void *fileinfor,char *audio_buffer, ME_U32 bytes, ME_U32 duration)
{
	mp3_recinfor_t *file = (mp3_recinfor_t *)fileinfor;
	ME_U32	writebytes;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp3_write_audio()"));
	#endif

	file->seek_func(file->filehandle, file->file_position, SEEK_SET);
	//writebytes = file->write_func(file->filehandle, audio_buffer, bytes);
	writebytes = file->write_block_func(file->filehandle, audio_buffer, bytes,IDX_VID_STATUS,29);
	file->file_position 	   += writebytes;
	file->total_atime 	   += duration;
	file->total_asamples +=1;
	if(writebytes==bytes)
	{
		return ME_SUCCESS;
	}
	else
	{
		return ME_ERR_WRITE_FILE_ERROR;
	}
}

ME_U32	muxer_mp3_write_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	mp3_recinfor_t *file = (mp3_recinfor_t *)fileinfor;
	ME_U32	writebytes;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp3_write_audio_frames()"));
	#endif

	//printf("MP3_MUXER:audio_buffer->data_length=%d\n",audio_buffer->data_length);
	//writebytes = file->write_func(file->filehandle, (char*)audio_buffer->buffer_address, audio_buffer->data_length);
	writebytes = file->write_block_func(file->filehandle, (char*)audio_buffer->buffer_address, audio_buffer->data_length,IDX_VID_STATUS,29);
	file->file_position 	   += writebytes;
	file->total_atime 	   += audio_buffer->frame_count*audio_buffer->frame_duration;
	file->total_asamples += audio_buffer->frame_count;
	if(writebytes==audio_buffer->data_length)
	{
		return ME_SUCCESS;
	}
	else
	{
		return ME_ERR_WRITE_FILE_ERROR;
	}
}

ME_U32	muxer_mp3_register(void)
{
	me_muxer_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp3_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis = &_g_muxer_mp3;
#else
	pthis	= (me_muxer_t *)me_malloc( sizeof(me_muxer_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif
	muxer_register_initialize(pthis);
	sprintf(pthis->id, "mp3");
	pthis->init					  	= muxer_mp3_init;
	pthis->release				  	= muxer_mp3_release;
	pthis->set_audio_info			  	= muxer_mp3_set_audio_infor;
	pthis->write_audio_oneframe     		= muxer_mp3_write_audio;
	pthis->write_audio_frames			= muxer_mp3_write_audio_frames;
	return me_muxer_register(pthis);	/*×¢²ámp3 muxer*/
}
#endif


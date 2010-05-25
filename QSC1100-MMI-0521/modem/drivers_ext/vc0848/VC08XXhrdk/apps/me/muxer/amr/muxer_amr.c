/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	Muxer_amr.c
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/11/15
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#include "muxer_amr.h"
#include "mmd_file_driver.h"
#if ME_KM_MUXER_AMR_EN	

#if ME_KM_OBJ_STACK
static me_muxer_t		_g_muxer_amr;
#endif

#define 	MUXER_AMR_MESSAGE			0
#define 	MUXER_AMR_DEBUG				0

#if MUXER_AMR_MESSAGE
#define 	muxer_amr_msg(str)			{me_printf("\nmuxer_amr_msg:");me_printf str;}
#else
#define 	muxer_amr_msg(str)
#endif

#if MUXER_AMR_DEBUG
#define 	muxer_amr_dbg(str)			{me_printf("\nmuxer_amr_dbg:");me_printf str;}
#else
#define 	muxer_amr_dbg(str)
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
} amr_recinfor_t;


extern F_FILE *file_rec;
ME_U32	muxer_amr_init(void *fs_filehandle, fs_callbacks *fs_filefunc,void **fileinfor)
{
	amr_recinfor_t *new_file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_amr_init()"));
	#endif	

	new_file = (amr_recinfor_t *)me_malloc(sizeof(amr_recinfor_t));
	if(new_file==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		(*fileinfor)=NULL;
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	
	me_memset((ME_PU08)new_file, 0, sizeof(amr_recinfor_t));
	new_file->filehandle		= fs_filehandle;
	new_file->read_func		= fs_filefunc->read_func;
	new_file->seek_func		= fs_filefunc->seek_func;
	new_file->write_func		= fs_filefunc->write_func;
	new_file->tell_func			= fs_filefunc->tell_func;
	new_file->read_block_func  = fs_filefunc->read_block_func;
	new_file->write_block_func  = fs_filefunc->write_block_func;
	new_file->file_position		= /*MMD_FILE_Seek(file_rec, 0, SEEK_SET);*/new_file->seek_func(new_file->filehandle, 0, SEEK_SET);
	
	/*MMD_FILE_Write(file_rec,(MMD_U08 *)"#!AMR\n", 6,(MMD_U32*)&new_file->file_position);
	if(new_file->file_position!=6)
	{
		me_free(new_file);
		(*fileinfor)=NULL;
		return ME_ERR_WRITE_FILE_ERROR;
	}
	else*/
	{
		(*fileinfor)=(void *)new_file;
		return ME_SUCCESS;
	}
}

ME_U32 muxer_amr_release(void *fileinfor)
{
	amr_recinfor_t *file = (amr_recinfor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_amr_release()"));
	#endif	
	
	muxer_amr_msg(("file->total_length = %d", file->total_length));
	muxer_amr_msg(("file->total_asamples = %d", file->total_asamples));
	muxer_amr_msg(("file->total_atime = %d", file->total_atime));
	muxer_amr_msg(("file->file_position = %d", file->file_position));
	
	me_free(file);
	return ME_SUCCESS;
}


ME_U32	muxer_amr_set_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	//amr_recinfor_t *file = (amr_recinfor_t *)fileinfor;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_amr_set_audio_infor()"));
	#endif
	
	return ME_SUCCESS;
}

ME_U32	muxer_amr_write_audio(void *fileinfor,char *audio_buffer, ME_U32 bytes, ME_U32 duration)
{
	amr_recinfor_t *file = (amr_recinfor_t *)fileinfor;
	ME_U32	writebytes;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_amr_write_audio()"));
	#endif

	file->seek_func(file->filehandle, file->file_position, SEEK_SET);
	writebytes = file->write_block_func(file->filehandle, audio_buffer, bytes,IDX_VID_STATUS,29);
	file->file_position 	   += writebytes;
	file->total_atime 	   += duration;
	file->total_asamples +=1;
	if(writebytes==bytes)
	{
		me_printf("Write Frame End!\n");
		return ME_SUCCESS;
	}
	else
	{
		return ME_ERR_WRITE_FILE_ERROR;
	}
}

ME_U32	muxer_amr_write_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	amr_recinfor_t *file = (amr_recinfor_t *)fileinfor;
	ME_U32	writebytes;
	//F_COREMEM sCoreMem;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_amr_write_audio_frames()"));
	#endif
	writebytes = file->write_block_func(file->filehandle, (char*)audio_buffer->buffer_address, audio_buffer->data_length,IDX_VID_STATUS,29);

	file->file_position 	   += writebytes;
	file->total_atime 	   += audio_buffer->frame_count*audio_buffer->frame_duration;
	file->total_asamples += audio_buffer->frame_count;
	if(writebytes==audio_buffer->data_length)
	{
		//me_printf("Write Frames End !!\n");
		return ME_SUCCESS;
	}
	else
	{
		return ME_ERR_WRITE_FILE_ERROR;
	}
}

ME_U32	muxer_amr_register(void)
{
	me_muxer_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_amr_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis = &_g_muxer_amr;
#else
	pthis	= (me_muxer_t *)me_malloc( sizeof(me_muxer_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif
	muxer_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "amr");
	pthis->init					  	= muxer_amr_init;
	pthis->release				  	= muxer_amr_release;
	pthis->set_audio_info			  	= muxer_amr_set_audio_infor;
	pthis->write_audio_oneframe     		= muxer_amr_write_audio;
	pthis->write_audio_frames			= muxer_amr_write_audio_frames;
	return me_muxer_register(pthis);	/*×¢²áamr muxer*/
}

#endif

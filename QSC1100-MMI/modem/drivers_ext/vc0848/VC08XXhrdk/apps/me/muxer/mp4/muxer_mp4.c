/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	Muxer_mp4.c
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#include "muxer_mp4.h"
#include "mmd_file_driver.h"
#if ME_KM_MUXER_MP4_EN	
#include "..\3gp\muxer_3gp.h"
#include "..\..\libsrc\mp4\mp4_interface.h"
#if ME_KM_OBJ_STACK
static me_muxer_t		_g_muxer_mp4;
#endif

#define 	MUXER_MP4V_STREAM_DATA_ONLY		0

#if MUXER_MP4V_STREAM_DATA_ONLY
#define 	MUXER_MP4V_MESSAGE					0
#define 	MUXER_MP4V_DEBUG						0

#if MUXER_MP4V_MESSAGE
#define 	muxer_mp4v_msg(str)			{me_printf("\nmuxer_mp4v_msg:");me_printf str;}
#else
#define 	muxer_mp4v_msg(str)
#endif

#if MUXER_MP4V_DEBUG
#define 	muxer_mp4v_dbg(str)			{me_printf("\nmuxer_mp4v_dbg:");me_printf str;}
#else
#define 	muxer_mp4v_dbg(str)
#endif

typedef struct
{
	void 	*filehandle;			/* FILE *stream; 820 file maybe not in file system*/
	ME_S32 	total_vtime;			/* jpg total record time (ms)*/
	ME_S32 	total_vsamples;		/* jpg total frame*/
	ME_S32  	total_length;		/* file total length */
	ME_S32  	file_position;      		/* Current position of file descriptor */
	ME_S32	(*read_func)(void *fs_filehandle, char* buffer, ME_S32 readbytes);
	ME_S32	(*write_func)(void *fs_filehandle, char* buffer, ME_S32 writebytes);
	ME_S32	(*seek_func)(void *fs_filehandle, ME_S32 offset, ME_S32 mode);
	ME_S32 	(*tell_func)(void *fs_filehandle);
	ME_S32	(*read_block_func)(void *fs_filehandle, char* buffer, ME_S32 readbytes, ME_U32 reg, ME_U32 bit);
	ME_S32	(*write_block_func)(void *fs_filehandle, char* buffer, ME_S32 writebytes, ME_U32 reg, ME_U32 bit);
	ME_S32	(*get_sector_offset_func)(void *fs_filehandle, ME_U32 *sectorsize);
	ME_PVOID	(*duplicate_handle_func)(void *file);
}mp4v_recinfor_t;


ME_U32	muxer_mp4v_init(void *fs_filehandle, fs_callbacks *fs_filefunc,void **fileinfor)
{
	mp4v_recinfor_t *new_file;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_jpg_init()"));
	#endif	

	new_file = (mp4v_recinfor_t *)me_malloc(sizeof(mp4v_recinfor_t));
	if(new_file==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		(*fileinfor)=NULL;
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	
	me_memset((ME_PU08)new_file, 0, sizeof(mp4v_recinfor_t));
	new_file->filehandle		= fs_filehandle;
	new_file->read_func		= fs_filefunc->read_func;
	new_file->seek_func		= fs_filefunc->seek_func;
	new_file->write_func		= fs_filefunc->write_func;
	new_file->tell_func			= fs_filefunc->tell_func;
	new_file->read_block_func  	= fs_filefunc->read_block_func;
	new_file->write_block_func  = fs_filefunc->write_block_func;
	new_file->file_position		= new_file->seek_func(new_file->filehandle, 0, SEEK_SET);
	(*fileinfor)=(void *)new_file;
	return ME_SUCCESS;
	
}

ME_U32 muxer_mp4v_release(void *fileinfor)
{
	mp4v_recinfor_t *file = (mp4v_recinfor_t *)fileinfor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp4v_release()"));
	#endif	
	
	muxer_mp4v_msg(("file->total_length = %d", file->total_length));
	muxer_mp4v_msg(("file->total_vsamples = %d", file->total_vsamples));
	muxer_mp4v_msg(("file->total_vtime = %d", file->total_vtime));
	muxer_mp4v_msg(("file->file_position = %d", file->file_position));
	
	me_free(file);
	return ME_SUCCESS;
}

ME_U32	muxer_mp4v_set_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp4v_set_video_infor()"));
	#endif
	return ME_SUCCESS;
}

ME_U32	muxer_mp4v_write_video(void *fileinfor, char *video_buffer, ME_U32 bytes, ME_U32 duration,ME_U32 is_key_frame)
{
	mp4v_recinfor_t *file = (mp4v_recinfor_t *)fileinfor;
	ME_U32	writebytes;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp4v_write_video()"));
	#endif
	
	file->seek_func(file->filehandle, file->file_position, SEEK_SET);
	//writebytes = file->write_func(file->filehandle, video_buffer, bytes);
	writebytes = file->write_block_func(file->filehandle, video_buffer, bytes,IDX_VID_STATUS,28);
	file->file_position 	   += writebytes;
	file->total_vtime 	   += duration;
	file->total_vsamples +=1;
	if(writebytes==bytes)
	{
		return ME_SUCCESS;
	}
	else
	{
		return ME_ERR_WRITE_FILE_ERROR;
	}
}

ME_U32	muxer_mp4v_write_video_data(void *fileinfor, video_stream_infor *video_buffer)
{
	mp4v_recinfor_t *file = (mp4v_recinfor_t *)fileinfor;
	ME_U32	writebytes;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp4v_write_video_data()"));
	#endif
	
	file->seek_func(file->filehandle, file->file_position, SEEK_SET);
	//writebytes = file->write_func(file->filehandle, (char*)video_buffer->buffer_address, video_buffer->data_length);
	writebytes = file->write_block_func(file->filehandle, (char*)video_buffer->buffer_address, video_buffer->data_length,IDX_VID_STATUS,28);
	file->file_position 	   += writebytes;
	
	file->total_vtime 	   += video_buffer->frame_count*video_buffer->frame_duration[0];
	file->total_vsamples += video_buffer->frame_count;
	
	if(writebytes==video_buffer->data_length)
	{
		return ME_SUCCESS;
	}
	else
	{
		me_printf("\nME_MESSAGE:	writebytes=%d", writebytes);
		return ME_ERR_WRITE_FILE_ERROR;
	}
}
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
ME_U32	muxer_mp4_register(void)
{
	me_muxer_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_mp4_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis = &_g_muxer_mp4;
#else
	pthis	= (me_muxer_t *)me_malloc( sizeof(me_muxer_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif
	muxer_register_initialize(pthis);
	sprintf(pthis->id, "mp4");
#if MUXER_MP4V_STREAM_DATA_ONLY
	pthis->init					  	= muxer_mp4v_init;
	pthis->release				  	= muxer_mp4v_release;
	pthis->set_video_info		         	= muxer_mp4v_set_video_infor;
 	pthis->write_video_oneframe     		= muxer_mp4v_write_video;
	pthis->write_video_frames     		= muxer_mp4v_write_video_data;
#else
	pthis->init					  	= muxer_3gp_init;
	pthis->release				  	= muxer_3gp_release;
	pthis->set_audio_info			  	= muxer_3gp_set_audio_infor;
	pthis->write_audio_oneframe     		= muxer_3gp_write_audio;
	pthis->write_audio_frames			= muxer_3gp_write_audio_frames;
	pthis->set_video_info		         	= muxer_3gp_set_video_infor;
 	pthis->write_video_oneframe     		= muxer_3gp_write_video;
	pthis->write_video_frames     		= muxer_3gp_write_video_frames;
#endif
	return me_muxer_register(pthis);	/*×¢²ámp4 muxer*/
}
#endif


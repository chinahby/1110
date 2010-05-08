/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	mdhd.c  
* User for : 	mp4 demux mux, mdhd atom is leaf atom of mdia atom, it is mandatory 
*			it is media header, include all iformation of the media 
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/
#include "mp4_parser.h"

/*******************************************************************************************
Description:
	initialize hdlr head information
Parameters:	
	mdhd: point of the media head structor
Return:
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_mdhd_init(mp4_mdhd_t *mdhd)
{
	mdhd->version = 0;
	mdhd->flags = 0;
	mdhd->creation_time = mp4_current_time();
	mdhd->modification_time = mp4_current_time();
	mdhd->time_scale = 0;
	mdhd->duration = 0;
	mdhd->language = 0;
	mdhd->quality = 0;
}

/*******************************************************************************************
Description:
	initialize video media head information
Parameters:	
	mdhd: point of the media head structor
Return:
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_mdhd_init_video(mp4_infor_t *file, 
							 mp4_mdhd_t *mdhd,
							 uint32  time_scale)
{
	mdhd->time_scale = time_scale;
	mdhd->duration = 0;      /* set this when closing */
}

/*******************************************************************************************
Description:
	initialize audio media head information
Parameters:	
	mdhd: point of the media head structor
Return:
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_mdhd_init_audio(mp4_infor_t *file, 
							 mp4_mdhd_t *mdhd, 
							 uint32  time_scale)
{
	mdhd->time_scale = time_scale;
	mdhd->duration = 0;      /* set this when closing */
}

void mp4_mdhd_delete(mp4_mdhd_t *mdhd)
{
	
}
/*******************************************************************************************
Description:
	read atom type and size information form file, nomal need read 8 bytes
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_mdhd_dump(mp4_mdhd_t *mdhd)
{
	MP4_DEMUX_MSG(("media header\n"));
	MP4_DEMUX_MSG(("version %d\n", mdhd->version));
	MP4_DEMUX_MSG(("flags %d\n", mdhd->flags));
	MP4_DEMUX_MSG(("creation_time %u\n", mdhd->creation_time));
	MP4_DEMUX_MSG(("modification_time %u\n", mdhd->modification_time));
	MP4_DEMUX_MSG(("time_scale %d\n", mdhd->time_scale));
	MP4_DEMUX_MSG(("duration %d\n", mdhd->duration));
	MP4_DEMUX_MSG(("language %d\n", mdhd->language));
	MP4_DEMUX_MSG(("quality %d\n", mdhd->quality));
}

/*******************************************************************************************
Description:
	read media head information form file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	mdhd: point of the media head structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_mdhd(mp4_infor_t *file, mp4_mdhd_t *mdhd)
{
	uint32 result;
	
	result = mp4_read_char(file, &mdhd->version);
	if(result) return result;
	result = mp4_read_uint24(file, &mdhd->flags);
	if(result) return result;
	result = mp4_read_uint32(file, &mdhd->creation_time);
	if(result) return result;
	result = mp4_read_uint32(file, &mdhd->modification_time);
	if(result) return result;
	result = mp4_read_uint32(file, &mdhd->time_scale);
	if(result) return result;
	result = mp4_read_uint32(file, &mdhd->duration);
	if(result) return result;
	result = mp4_read_uint16(file, &mdhd->language);
	return  mp4_read_uint16(file, &mdhd->quality);

}
/*******************************************************************************************
Description:
	write media head information to file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	mdhd: point of the media head structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_mdhd(mp4_infor_t *file, mp4_mdhd_t *mdhd)
{
	mp4_atom_t atom;
	uint32 result;
	
	result = mp4_atom_write_header(file, &atom, "mdhd");
	if(result) return result;
	result = mp4_write_char(file, mdhd->version);
	if(result) return result;
	result = mp4_write_uint24(file, mdhd->flags);
	if(result) return result;
	result = mp4_write_uint32(file, mdhd->creation_time);
	if(result) return result;
	result = mp4_write_uint32(file, mdhd->modification_time);
	if(result) return result;
	result = mp4_write_uint32(file, mdhd->time_scale);
	if(result) return result;
	result = mp4_write_uint32(file, mdhd->duration);
	if(result) return result;
	result = mp4_write_uint16(file, mdhd->language);
	if(result) return result;
	
	if((file->audio_type == MP4_TYPE_AUDIO_IS_MP4A)||(file->video_type == MP4_TYPE_VIDEO_IS_MP4V))
	{
		result = mp4_write_uint16(file, 0x0000);
		if(result) return result;
	} 
	else 
	{
		result = mp4_write_uint16(file, mdhd->quality);	
		if(result) return result;
	}
	return mp4_atom_write_footer(file, &atom);
}


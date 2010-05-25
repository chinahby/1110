/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	mvhd.c  
* User for : 	mp4 demux mux, mvhd atom is leaf atom of moov atom, it is mandatory 
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
void mp4_mvhd_init(mp4_mvhd_t *mvhd)
{
	uint32 i;
	mvhd->version = 0;
	mvhd->flags = 0;
	mvhd->creation_time = mp4_current_time();
	mvhd->modification_time = mp4_current_time();
	mvhd->time_scale = 90000;
	mvhd->duration = 0;
	mvhd->preferred_rate = 1.0;
	mvhd->preferred_volume = 0.996094;
	for(i = 0; i < 10; i++) mvhd->reserved[i] = 0;
	mp4_matrix_init(&(mvhd->matrix));
	mvhd->preview_time = 0;
	mvhd->preview_duration = 0;
	mvhd->poster_time = 0;
	mvhd->selection_time = 0;
	mvhd->selection_duration = 0;
	mvhd->current_time = 0;
	mvhd->next_track_id = 1;
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
void mp4_mvhd_delete(mp4_mvhd_t *mvhd)
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
void mp4_mvhd_dump(mp4_mvhd_t *mvhd)
{
	MP4_DEMUX_MSG(("movie header\n"));
	MP4_DEMUX_MSG(("version %d\n", mvhd->version));
	MP4_DEMUX_MSG(("flags %d\n", mvhd->flags));
	MP4_DEMUX_MSG(("creation_time %u\n", mvhd->creation_time));
	MP4_DEMUX_MSG(("modification_time %u\n", mvhd->modification_time));
	MP4_DEMUX_MSG(("time_scale %d\n", mvhd->time_scale));
	MP4_DEMUX_MSG(("duration %d\n", mvhd->duration));
	MP4_DEMUX_MSG(("preferred_rate %f\n", mvhd->preferred_rate));
	MP4_DEMUX_MSG(("preferred_volume %f\n", mvhd->preferred_volume));
	mp4_matrix_dump(&(mvhd->matrix));
	MP4_DEMUX_MSG(("preview_time %d\n", mvhd->preview_time));
	MP4_DEMUX_MSG(("preview_duration %d\n", mvhd->preview_duration));
	MP4_DEMUX_MSG(("poster_time %d\n", mvhd->poster_time));
	MP4_DEMUX_MSG(("selection_time %d\n", mvhd->selection_time));
	MP4_DEMUX_MSG(("selection_duration %d\n", mvhd->selection_duration));
	MP4_DEMUX_MSG(("current_time %d\n", mvhd->current_time));
	MP4_DEMUX_MSG(("next_track_id %d\n", mvhd->next_track_id));
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
uint32 mp4_read_mvhd(mp4_infor_t *file, mp4_mvhd_t *mvhd)
{
	uint32 result;
	
	result = mp4_read_char(file, &mvhd->version);
	if(result) return result;
	result = mp4_read_uint24(file, &mvhd->flags);
	if(result) return result;
	result = mp4_read_uint32(file, &mvhd->creation_time);
	if(result) return result;
	result = mp4_read_uint32(file, &mvhd->modification_time);
	if(result) return result;
	result = mp4_read_uint32(file, &mvhd->time_scale);
	if(result) return result;
	result = mp4_read_uint32(file, &mvhd->duration);
	if(result) return result;
	result = mp4_read_fixed32(file, &mvhd->preferred_rate);
	if(result) return result;
	result = mp4_read_fixed16(file, &mvhd->preferred_volume);
	if(result) return result;
	result = mp4_read_data(file, mvhd->reserved, 10);
	if(result!=10) return MP4_DEMUX_READ_FILE_ERROR;
	result = mp4_read_matrix(file, &(mvhd->matrix));
	if(result) return result;
	result = mp4_read_uint32(file, &mvhd->preview_time);
	if(result) return result;
	result = mp4_read_uint32(file, &mvhd->preview_duration);
	if(result) return result;
	result = mp4_read_uint32(file, &mvhd->poster_time);
	if(result) return result;
	result = mp4_read_uint32(file, &mvhd->selection_time);
	if(result) return result;
	result = mp4_read_uint32(file, &mvhd->selection_duration);
	if(result) return result;
	result = mp4_read_uint32(file, &mvhd->current_time);
	if(result) return result;
	return  mp4_read_uint32(file, &mvhd->next_track_id);
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
uint32 mp4_write_mvhd(mp4_infor_t *file, mp4_mvhd_t *mvhd)
{
	mp4_atom_t atom;
	uint32 i, result;
	
	result = mp4_atom_write_header(file, &atom, "mvhd");
	if(result) return result;
	
	result = mp4_write_char(file, mvhd->version);
	if(result) return result;
	result = mp4_write_uint24(file, mvhd->flags);
	if(result) return result;
	result = mp4_write_uint32(file, mvhd->creation_time);
	if(result) return result;
	result = mp4_write_uint32(file, mvhd->modification_time);
	if(result) return result;
	result = mp4_write_uint32(file, mvhd->time_scale);
	if(result) return result;
	result = mp4_write_uint32(file, mvhd->duration);
	if(result) return result;
	
	if((file->audio_type == MP4_TYPE_AUDIO_IS_MP4A)||(file->video_type == MP4_TYPE_VIDEO_IS_MP4V))
	{
		result = mp4_write_uint32(file, 0x00010000);
		if(result) return result;
		result = mp4_write_uint16(file, 0x0100);
		if(result) return result;
		result = mp4_write_uint16(file, 0x0000);
		if(result) return result;
		result = mp4_write_uint32(file, 0x00000000);
		if(result) return result;
		result = mp4_write_uint32(file, 0x00000000);
		if(result) return result;
		result = mp4_write_uint32(file, 0x00010000);
		
		for (i = 0; i < 3; i++) 
		{
			result = mp4_write_uint32(file, 0x00000000);
			if(result) return result;
		}
		result = mp4_write_uint32(file, 0x00010000);
		if(result) return result;
		
		for (i = 0; i < 3; i++) 
		{
			result = mp4_write_uint32(file, 0x00000000);
			if(result) return result;
		}
		
		result = mp4_write_uint32(file, 0x40000000);
		if(result) return result;
		
		for (i = 0; i < 6; i++) 
		{
			result = mp4_write_uint32(file, 0x00000000);
			if(result) return result;
		}
	} 
	else 
	{
		result = mp4_write_fixed32(file, mvhd->preferred_rate);
		if(result) return result;
		result = mp4_write_fixed16(file, mvhd->preferred_volume);
		if(result) return result;
		result = mp4_write_data(file, mvhd->reserved, 10);
		if(result!=10) return MP4_MUX_WRITE_FILE_ERROR;
		result = mp4_write_matrix(file, &(mvhd->matrix));
		if(result) return result;
		result = mp4_write_uint32(file, mvhd->preview_time);
		if(result) return result;
		result = mp4_write_uint32(file, mvhd->preview_duration);
		if(result) return result;
		result = mp4_write_uint32(file, mvhd->poster_time);
		if(result) return result;
		result = mp4_write_uint32(file, mvhd->selection_time);
		if(result) return result;
		result = mp4_write_uint32(file, mvhd->selection_duration);
		if(result) return result;
		result = mp4_write_uint32(file, mvhd->current_time);
	}

	result = mp4_write_uint32(file, mvhd->next_track_id);
	if(result) return result;
	
	return  mp4_atom_write_footer(file, &atom);
}

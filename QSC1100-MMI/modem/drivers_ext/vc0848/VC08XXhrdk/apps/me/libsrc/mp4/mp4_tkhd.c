/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	tkhd.c  
* User for : 	mp4 demux mux, tkhd atom is leaf atom of trak atom, it is mandatory
*			track header, include overall information of this track
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"


void mp4_tkhd_init(mp4_tkhd_t *tkhd)
{
	uint32 i;
	tkhd->version = 0;
	tkhd->flags = 15;
	tkhd->creation_time = mp4_current_time();
	tkhd->modification_time = mp4_current_time();
	tkhd->track_id = 0;
	tkhd->reserved1 = 0;
	tkhd->duration = 0;      		/* need to set this when closing */
	for(i = 0; i < 8; i++) tkhd->reserved2[i] = 0;
	tkhd->layer = 0;
	tkhd->alternate_group = 0;
	tkhd->volume = 0.996094;
	tkhd->reserved3 = 0;
	mp4_matrix_init(&(tkhd->matrix));
	tkhd->track_width = 0;
	tkhd->track_height = 0;
	tkhd->is_audio = FALSE;
	tkhd->is_video = FALSE;
}

void mp4_tkhd_init_audio(mp4_infor_t *file, mp4_tkhd_t *tkhd)
{
	tkhd->is_audio = TRUE;
}

void mp4_tkhd_init_video(mp4_infor_t *file, mp4_tkhd_t *tkhd, uint32 frame_w, uint32 frame_h)
{
	tkhd->is_video = TRUE;
	tkhd->track_width = frame_w;
	tkhd->track_height = frame_h;
	tkhd->volume = 0;
}

void mp4_tkhd_delete(mp4_tkhd_t *tkhd)
{

}

void mp4_tkhd_dump(mp4_tkhd_t *tkhd)
{
	MP4_DEMUX_MSG(("  track header\n"));
	MP4_DEMUX_MSG(("   version %d\n", tkhd->version));
	MP4_DEMUX_MSG(("   flags %d\n", tkhd->flags));
	MP4_DEMUX_MSG(("   creation_time %u\n", tkhd->creation_time));
	MP4_DEMUX_MSG(("   modification_time %u\n", tkhd->modification_time));
	MP4_DEMUX_MSG(("   track_id %d\n", tkhd->track_id));
	MP4_DEMUX_MSG(("   reserved1 %d\n", tkhd->reserved1));
	MP4_DEMUX_MSG(("   duration %d\n", tkhd->duration));
	MP4_DEMUX_MSG(("   layer %d\n", tkhd->layer));
	MP4_DEMUX_MSG(("   alternate_group %d\n", tkhd->alternate_group));
	MP4_DEMUX_MSG(("   volume %f\n", tkhd->volume));
	MP4_DEMUX_MSG(("   reserved3 %d\n", tkhd->reserved3));
	mp4_matrix_dump(&(tkhd->matrix));
	MP4_DEMUX_MSG(("   track_width %f\n", tkhd->track_width));
	MP4_DEMUX_MSG(("   track_height %f\n", tkhd->track_height));
}

uint32 mp4_read_tkhd(mp4_infor_t *file, mp4_tkhd_t *tkhd)
{
	uint32 result;

	result = mp4_read_char(file, &tkhd->version);
	if(result) return result;
	result = mp4_read_uint24(file, &tkhd->flags);
	if(result) return result;
	result = mp4_read_uint32(file, &tkhd->creation_time);
	if(result) return result;
	result = mp4_read_uint32(file, &tkhd->modification_time);
	if(result) return result;
	result = mp4_read_uint32(file, &tkhd->track_id);
	if(result) return result;
	result = mp4_read_uint32(file, &tkhd->reserved1);
	if(result) return result;
	result = mp4_read_uint32(file, &tkhd->duration);
	if(result) return result;
	result = mp4_read_data(file, tkhd->reserved2, 8);
	if(result!=8) return MP4_DEMUX_READ_FILE_ERROR;
	result = mp4_read_uint16(file, &tkhd->layer);
	if(result) return result;
	result = mp4_read_uint16(file, &tkhd->alternate_group);
	if(result) return result;
	result = mp4_read_fixed16(file, &tkhd->volume);
	if(result) return result;
	result = mp4_read_uint16(file, &tkhd->reserved3);
	if(result) return result;
	result = mp4_read_matrix(file, &(tkhd->matrix));
	if(result) return result;
	result = mp4_read_fixed32(file, &tkhd->track_width);
	if(result) return result;
	return mp4_read_fixed32(file, &tkhd->track_height);
}

uint32 mp4_write_tkhd(mp4_infor_t *file, mp4_tkhd_t *tkhd)
{
	mp4_atom_t atom;
	uint32 result;
	
	result = mp4_atom_write_header(file, &atom, "tkhd");
	if(result) return result;
	
	result = mp4_write_char(file, tkhd->version);
	if(result) return result;
	
	if (((file->video_type == MP4_TYPE_VIDEO_IS_MP4V) && tkhd->flags != 0) 
		||((file->audio_type == MP4_TYPE_AUDIO_IS_MP4A) && tkhd->flags != 0))
	{
		result = mp4_write_uint24(file, 1);
		if(result) return result;
	} 
	else 
	{
		result = mp4_write_uint24(file, tkhd->flags);
		if(result) return result;
	}
	result = mp4_write_uint32(file, tkhd->creation_time);
	if(result) return result;
	result = mp4_write_uint32(file, tkhd->modification_time);
	if(result) return result;
	result = mp4_write_uint32(file, tkhd->track_id);
	if(result) return result;
	result = mp4_write_uint32(file, tkhd->reserved1);
	if(result) return result;
	result = mp4_write_uint32(file, tkhd->duration);
	if(result) return result;

	if (((tkhd->is_audio)&&((file->audio_type == MP4_TYPE_AUDIO_IS_MP4A)))||((tkhd->is_video)&&((file->video_type == MP4_TYPE_VIDEO_IS_MP4V))))
	{
		uint32 i;
		// 8 bytes tkhd->reserved2
		// 2 bytes tkhd->layer
		// 2 bytes tkhd->alternate_group
		for (i = 0; i < 3; i++) 
		{
			if(result) return result;
			result = mp4_write_uint32(file, 0x00000000);
		}

		if (tkhd->is_audio) 
		{
			result = mp4_write_uint16(file, 0x0100);
			if(result) return result;
		} else 
		{
			result = mp4_write_uint16(file, 0x0000);
			if(result) return result;
		}
		
		result = mp4_write_uint16(file, 0x0000);
		if(result) return result;
		result = mp4_write_uint32(file, 0x00010000);
		if(result) return result;
		
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
		
		if (tkhd->is_video) 
		{
			result = mp4_write_uint32(file, 0x01400000);
			if(result) return result;
			result = mp4_write_uint32(file, 0x00F00000);
			if(result) return result;
		} 
		else 
		{
			result = mp4_write_uint32(file, 0x00000000);
			if(result) return result;
			result = mp4_write_uint32(file, 0x00000000);
			if(result) return result;
		}
	} 
	else 
	{
		result = mp4_write_data(file, tkhd->reserved2, 8);
		if(result!=8) return MP4_MUX_WRITE_FILE_ERROR;
		result = mp4_write_uint16(file, tkhd->layer);
		if(result) return result;
		result = mp4_write_uint16(file, tkhd->alternate_group);
		if(result) return result;
		if (tkhd->is_audio) 
		{
			result = mp4_write_uint16(file, 0x0100);
			if(result) return result;
		} else 
		{
			result = mp4_write_uint16(file, 0x0000);
			if(result) return result;
		}
		
		result = mp4_write_uint16(file, tkhd->reserved3);
		if(result) return result;
		result = mp4_write_matrix(file, &(tkhd->matrix));
		if(result) return result;
		result = mp4_write_fixed32(file, tkhd->track_width);
		if(result) return result;
		result = mp4_write_fixed32(file, tkhd->track_height);
		if(result) return result;
	}

	return mp4_atom_write_footer(file, &atom);
}



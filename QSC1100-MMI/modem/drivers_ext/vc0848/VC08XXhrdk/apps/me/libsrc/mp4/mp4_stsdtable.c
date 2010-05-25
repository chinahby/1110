/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	stsdtable.c  
* User for : 	mp4 demux mux, it will be the one of mp4a,mp4v,s263,samr atom
*			include video or audio information
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"


void mp4_stsd_table_init(mp4_stsd_table_t *table)
{
	uint32 i;
	table->format[0] = 'y';
	table->format[1] = 'u';
	table->format[2] = 'v';
	table->format[3] = '2';
	for(i = 0; i < 6; i++) table->reserved[i] = 0;
	table->data_reference = 1;

	table->version = 0;
	table->revision = 0;
 	table->vendor[0] = 'l';
 	table->vendor[1] = 'n';
 	table->vendor[2] = 'u';
 	table->vendor[3] = 'x';

	table->temporal_quality = 0;
	table->spatial_quality = 258;
	table->width = 0;
	table->height = 0;
	table->dpi_horizontal = 72;
	table->dpi_vertical = 72;
	table->data_size = 0;
	table->frames_per_sample = 1;
	for(i = 0; i < 32; i++) table->compressor_name[i] = 0;
	MP4_SPRINTF(table->compressor_name, "Vimicro for VC820");
	table->depth = 24;
	table->ctab_id = 65535;
	
	table->gamma = 0;
	table->fields = 0;
	table->field_dominance = 1;
		
	table->channels = 0;
	table->sample_size = 0;
	table->compression_id = 0;
	table->packet_size = 0;
	table->sample_rate = 0;

	mp4_esds_init(&(table->esds));
	mp4_damr_init(&(table->damr));
	mp4_d263_init(&(table->d263));
}

void mp4_stsd_table_delete(mp4_stsd_table_t *table)
{
	mp4_esds_delete(&(table->esds));
	mp4_damr_delete(&(table->damr));
	mp4_d263_delete(&(table->d263));
}

void mp4_stsd_audio_dump(mp4_stsd_table_t *table)
{
	MP4_DEMUX_MSG(("       version %d\n", table->version));
	MP4_DEMUX_MSG(("       revision %d\n", table->revision));
	MP4_DEMUX_MSG(("       vendor %c%c%c%c\n", table->vendor[0], table->vendor[1], table->vendor[2], table->vendor[3]));
	MP4_DEMUX_MSG(("       channels %d\n", table->channels));
	MP4_DEMUX_MSG(("       sample_size %d\n", table->sample_size));
	MP4_DEMUX_MSG(("       compression_id %d\n", table->compression_id));
	MP4_DEMUX_MSG(("       packet_size %d\n", table->packet_size));
	MP4_DEMUX_MSG(("       sample_rate %f\n", table->sample_rate));
	mp4_esds_dump(&(table->esds));
	mp4_damr_dump(&(table->damr));
	
}

void mp4_stsd_video_dump(mp4_stsd_table_t *table)
{
	MP4_DEMUX_MSG(("       version %d\n", table->version));
	MP4_DEMUX_MSG(("       revision %d\n", table->revision));
	MP4_DEMUX_MSG(("       vendor %c%c%c%c\n", table->vendor[0], table->vendor[1], table->vendor[2], table->vendor[3]));
	MP4_DEMUX_MSG(("       temporal_quality %d\n", table->temporal_quality));
	MP4_DEMUX_MSG(("       spatial_quality %d\n", table->spatial_quality));
	MP4_DEMUX_MSG(("       width %d\n", table->width));
	MP4_DEMUX_MSG(("       height %d\n", table->height));
	MP4_DEMUX_MSG(("       dpi_horizontal %f\n", table->dpi_horizontal));
	MP4_DEMUX_MSG(("       dpi_vertical %f\n", table->dpi_vertical));
	MP4_DEMUX_MSG(("       data_size %d\n", table->data_size));
	MP4_DEMUX_MSG(("       frames_per_sample %d\n", table->frames_per_sample));
	MP4_DEMUX_MSG(("       compressor_name %s\n", table->compressor_name));
	MP4_DEMUX_MSG(("       depth %d\n", table->depth));
	MP4_DEMUX_MSG(("       ctab_id %d\n", table->ctab_id));
	MP4_DEMUX_MSG(("       gamma %f\n", table->gamma));
	if(table->fields)
	{
		MP4_DEMUX_MSG(("       fields %d\n", table->fields));
		MP4_DEMUX_MSG(("       field dominance %d\n", table->field_dominance));
	}
	mp4_esds_dump(&(table->esds));
	mp4_d263_dump(&(table->d263));
}


void mp4_stsd_table_dump(void *minf_ptr, mp4_stsd_table_t *table)
{
	mp4_minf_t *minf = minf_ptr;
	MP4_DEMUX_MSG(("       format %c%c%c%c\n", table->format[0], table->format[1], table->format[2], table->format[3]));
	MP4_DEMUX_MSG(("       data_reference %d\n", table->data_reference));

	if(minf->is_audio) mp4_stsd_audio_dump(table);
	if(minf->is_video) mp4_stsd_video_dump(table);
}


uint32 mp4_read_stsd_table(mp4_infor_t *file, mp4_minf_t *minf, mp4_stsd_table_t *table)
{
	mp4_atom_t leaf_atom;
	uint32 result;

	result = mp4_atom_read_header(file, &leaf_atom);
	if(result) return result;
	
	if(mp4_atom_is(&leaf_atom, "mp4a")) 
	{
		file->audio_type = MP4_TYPE_AUDIO_IS_MP4A;
	} 
	else if(mp4_atom_is(&leaf_atom, "mp4v")) 
	{
		file->video_type =MP4_TYPE_VIDEO_IS_MP4V;
	}
	else if(mp4_atom_is(&leaf_atom, "s263")) 
	{
		file->video_type = MP4_TYPE_VIDEO_IS_S263;
	}
	else if(mp4_atom_is(&leaf_atom, "samr")) 
	{
		file->audio_type = MP4_TYPE_AUDIO_IS_SAMR;
	}
	table->format[0] = leaf_atom.type[0];
	table->format[1] = leaf_atom.type[1];
	table->format[2] = leaf_atom.type[2];
	table->format[3] = leaf_atom.type[3];
	result = mp4_read_data(file, table->reserved, 6);
	if(result !=6) return MP4_DEMUX_READ_FILE_ERROR;
	result = mp4_read_uint16(file, &table->data_reference);
	if(result) return result;

	if(minf->is_audio)
	{
		result = mp4_read_stsd_audio(file, table, &leaf_atom);
		if(result) return result;
	}
	if(minf->is_video) 
	{
		result = mp4_read_stsd_video(file, table, &leaf_atom);
		if(result) return result;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_write_stsd_table(mp4_infor_t *file, mp4_minf_t *minf, mp4_stsd_table_t *table)
{
	mp4_atom_t atom;
	uint32 result;
	
	result = mp4_atom_write_header(file, &atom, table->format);
	if(result) return result;
	result = mp4_write_data(file, table->reserved, 6);
	if(result !=6) return MP4_MUX_WRITE_FILE_ERROR;
	result = mp4_write_uint16(file, table->data_reference);
	if(result) return result;	
	
	if(minf->is_audio) 
	{
		result = mp4_write_stsd_audio(file, table);
		if(result) return result;
	}
	if(minf->is_video) 
	{
		result = mp4_write_stsd_video(file, table);
		if(result) return result;
	}
	return mp4_atom_write_footer(file, &atom);

}

uint32 mp4_read_stsd_audio(mp4_infor_t *file, mp4_stsd_table_t *table, mp4_atom_t *parent_atom)
{
	uint32 result;
	mp4_atom_t leaf_atom;
	
	result = mp4_read_uint16(file, &table->version);
	if(result) return result;
	result = mp4_read_uint16(file, &table->revision);
	if(result) return result;
	result = mp4_read_data(file, table->vendor, 4);
	if(result!=4)  return MP4_DEMUX_READ_FILE_ERROR;
	result = mp4_read_uint16(file, &table->channels);
	if(result) return result;
	result = mp4_read_uint16(file, &table->sample_size);
	if(result) return result;
	result = mp4_read_uint16(file, &table->compression_id);
	if(result) return result;
	result = mp4_read_uint16(file, &table->packet_size);
	if(result) return result;
	result = mp4_read_fixed32(file, &table->sample_rate);
	if(result) return result;
	
	while (mp4_get_file_position(file) < parent_atom->end) 
	{
		result = mp4_atom_read_header(file, &leaf_atom);
		if(result) return result;
		
		if(mp4_atom_is(&leaf_atom, "esds")) 
		{
			result = mp4_read_esds(file, &(table->esds));
			if(result) return result;
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		} 
		else if(mp4_atom_is(&leaf_atom, "damr")) 
		{
			result = mp4_read_damr_audio(file, &(table->damr));
			if(result) return result;
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		} 
		else 
		{
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_write_stsd_audio(mp4_infor_t *file, mp4_stsd_table_t *table)
{
	uint32 result;
	if (file->audio_type == MP4_TYPE_AUDIO_IS_MP4A) 
	{
		result = mp4_write_uint32(file, 0);
		if(result) return result;
		result = mp4_write_uint32(file, 0);
		if(result) return result;
		result = mp4_write_uint16(file, table->channels);
		if(result) return result;
		result = mp4_write_uint16(file, table->sample_size);
		if(result) return result;
		result = mp4_write_uint32(file, 0);
		if(result) return result;
		result = mp4_write_uint16(file, table->sample_rate);
		if(result) return result;
		result = mp4_write_uint16(file, 0);
		if(result) return result;
		result = mp4_write_esds_audio(file, &(table->esds),
			file->atracks[0].track->tkhd.track_id);
		if(result) return result;
	} 
	else if (file->audio_type == MP4_TYPE_AUDIO_IS_SAMR)
	{
		result = mp4_write_uint32(file, 0);
		if(result) return result;
		result = mp4_write_uint32(file, 0);
		if(result) return result;
		result = mp4_write_uint16(file, table->channels);
		if(result) return result;
		result = mp4_write_uint16(file, table->sample_size);
		if(result) return result;
		result = mp4_write_uint32(file, 0);
		if(result) return result;
		result = mp4_write_uint16(file, table->sample_rate);
		if(result) return result;
		result = mp4_write_uint16(file, 0);
		if(result) return result;
		result = mp4_write_damr_audio(file, &(table->damr));
		if(result) return result;
	}
	else 
	{
		result = mp4_write_uint16(file, table->version);
		if(result) return result;
		result = mp4_write_uint16(file, table->revision);
		if(result) return result;
		result = mp4_write_data(file, table->vendor, 4);
		if(result!=4) return MP4_MUX_WRITE_FILE_ERROR;
		result = mp4_write_uint16(file, table->channels);
		if(result) return result;
		result = mp4_write_uint16(file, table->sample_size);
		if(result) return result;
		result = mp4_write_uint16(file, table->compression_id);
		if(result) return result;
		result = mp4_write_uint16(file, table->packet_size);
		if(result) return result;
		result = mp4_write_fixed32(file, table->sample_rate);
		if(result) return result;
		result = mp4_write_esds_audio(file, &(table->esds),
			file->atracks[0].track->tkhd.track_id);
		if(result) return result;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_read_stsd_video(mp4_infor_t *file, mp4_stsd_table_t *table, mp4_atom_t *parent_atom)
{
	mp4_atom_t leaf_atom;
	uint32 len, result;
	
	
	result = mp4_read_uint16(file, &table->version);
	if(result) return result;
	result = mp4_read_uint16(file, &table->revision);
	if(result)  return result;
	result = mp4_read_data(file, table->vendor, 4);
	if(result != 4) return MP4_DEMUX_READ_FILE_ERROR;
	result = mp4_read_uint32(file, &table->temporal_quality);
	if(result) return result;
	result = mp4_read_uint32(file, &table->spatial_quality);
	if(result) return result;
	result = mp4_read_uint16(file, &table->width);
	if(result) return result;
	result = mp4_read_uint16(file, &table->height);
	if(result) return result;
	result = mp4_read_fixed32(file, &table->dpi_horizontal);
	if(result) return result;
	result = mp4_read_fixed32(file, &table->dpi_vertical);
	if(result) return result;
	result = mp4_read_uint32(file, &table->data_size);
	if(result) return result;
	result = mp4_read_uint16(file, &table->frames_per_sample);
	if(result) return result;
	result = mp4_read_char(file, &len);
	if(result) return result;
	result = mp4_read_data(file, table->compressor_name, 31);
	if(result != 31) return MP4_DEMUX_READ_FILE_ERROR;
	result = mp4_read_uint16(file, &table->depth);
	if(result) return result;
	result = mp4_read_uint16(file, &table->ctab_id);
	if(result) return result;
	
	while(mp4_get_file_position(file) < parent_atom->end)
	{
		result = mp4_atom_read_header(file, &leaf_atom);
		if(result) return result;
		
		if(mp4_atom_is(&leaf_atom, "esds"))
		{
			result = mp4_read_esds(file, &(table->esds));
			if(result) return result;
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "d263"))
		{
			result = mp4_read_d263_video(file, &(table->d263));
			if(result) return result;
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
		else
		{
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_write_stsd_video(mp4_infor_t *file, mp4_stsd_table_t *table)
{
	mp4_atom_t atom;
	uint32 i, result;
	
	if (file->video_type == MP4_TYPE_VIDEO_IS_MP4V) 
	{
		for (i = 0; i < 4; i++) 
		{
			result = mp4_write_uint32(file, 0);
			if(result) return result;
		}
		result = mp4_write_uint16(file, table->width);
		if(result) return result;
		result = mp4_write_uint16(file, table->height);
		if(result) return result;
		result = mp4_write_uint32(file, 0x00480000);
		if(result) return result;
		result = mp4_write_uint32(file, 0x00480000);
		if(result) return result;
		result = mp4_write_uint32(file, 0);
		if(result) return result;
		result = mp4_write_uint16(file, 1);
		if(result) return result;

		for (i = 0; i < 32; i++) 
		{

			result = mp4_write_char(file, 0);
			if(result) return result;
		}

		result = mp4_write_uint16(file, 24);
		if(result) return result;
		result = mp4_write_uint16(file, -1);
		if(result) return result;
		
		result = mp4_write_esds_video(file, &(table->esds),
			file->vtracks[0].track->tkhd.track_id);
		if(result) return result;
	}
	else if (file->video_type == MP4_TYPE_VIDEO_IS_S263)
	{
		result = mp4_write_uint16(file, table->version);
		if(result) return result;
		result = mp4_write_uint16(file, table->revision);
		if(result) return result;
		result = mp4_write_data(file, table->vendor, 4);
		if(result != 4) return MP4_MUX_WRITE_FILE_ERROR;
		result = mp4_write_uint32(file, table->temporal_quality);
		if(result) return result;
		result = mp4_write_uint32(file, table->spatial_quality);
		if(result) return result;
		result = mp4_write_uint16(file, table->width);
		if(result) return result;
		result = mp4_write_uint16(file, table->height);
		if(result) return result;
		result = mp4_write_fixed32(file, table->dpi_horizontal);
		if(result) return result;
		result = mp4_write_fixed32(file, table->dpi_vertical);
		if(result) return result;
		result = mp4_write_uint32(file, table->data_size);
		if(result) return result;
		result = mp4_write_uint16(file, table->frames_per_sample);
		if(result) return result;
		result = mp4_write_char(file, strlen(table->compressor_name));
		if(result) return result;
		result = mp4_write_data(file, table->compressor_name, 31);
		if(result != 31) return MP4_MUX_WRITE_FILE_ERROR;
		result = mp4_write_uint16(file, table->depth);
		if(result) return result;
		result = mp4_write_uint16(file, table->ctab_id);
		if(result) return result;
		result = mp4_write_d263_video(file, &(table->d263));
		if(result) return result;
	}
	else 
	{
		result = mp4_write_uint16(file, table->version);
		if(result) return result;
		result = mp4_write_uint16(file, table->revision);
		if(result) return result;
		result = mp4_write_data(file, table->vendor, 4);
		if(result != 4) return MP4_MUX_WRITE_FILE_ERROR;
		result = mp4_write_uint32(file, table->temporal_quality);
		if(result) return result;
		result = mp4_write_uint32(file, table->spatial_quality);
		if(result) return result;
		result = mp4_write_uint16(file, table->width);
		if(result) return result;
		result = mp4_write_uint16(file, table->height);
		if(result) return result;
		result = mp4_write_fixed32(file, table->dpi_horizontal);
		if(result) return result;
		result = mp4_write_fixed32(file, table->dpi_vertical);
		if(result) return result;
		result = mp4_write_uint32(file, table->data_size);
		if(result) return result;
		result = mp4_write_uint16(file, table->frames_per_sample);
		if(result) return result;
		result = mp4_write_char(file, strlen(table->compressor_name));
		if(result) return result;
		result = mp4_write_data(file, table->compressor_name, 31);
		if(result != 31) return MP4_MUX_WRITE_FILE_ERROR;
		result = mp4_write_uint16(file, table->depth);
		if(result) return result;
		result = mp4_write_uint16(file, table->ctab_id);
		if(result) return result;
		result = mp4_write_esds_video(file, &(table->esds),
			file->vtracks[0].track->tkhd.track_id);
		if(result) return result;
	}

	if(table->fields)
	{
		result = mp4_atom_write_header(file, &atom, "fiel");
		if(result) return result;
		result = mp4_write_char(file, table->fields);
		if(result) return result;
		result = mp4_write_char(file, table->field_dominance);
		if(result) return result;
		result = mp4_atom_write_footer(file, &atom);
		if(result) return result;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}



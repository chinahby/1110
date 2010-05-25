/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	stcd.c  
* User for : 	mp4 demux mux, stsd atom is leaf atom of minf atom, it is mandatory
*			sample decripitions atom, include video or audio type and other initialization information
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"


void mp4_stsd_init(mp4_stsd_t *stsd)
{
	stsd->version = 0;
	stsd->flags = 0;
	stsd->total_entries = 0;
	stsd->table = NULL;
}

void mp4_stsd_init_table(mp4_stsd_t *stsd)
{
	if(stsd->table==NULL)
	{
		stsd->total_entries = 1;
		stsd->table = (mp4_stsd_table_t*)mp4_malloc(sizeof(mp4_stsd_table_t) * stsd->total_entries);
		mp4_stsd_table_init(&(stsd->table[0]));
	}
}

void mp4_stsd_init_video(mp4_infor_t *file, 
								mp4_stsd_t *stsd, 
								uint32 frame_w,
								uint32 frame_h, 
								char *compression)
{
	mp4_stsd_table_t *table;
	mp4_stsd_init_table(stsd);
	table = &(stsd->table[0]);

	mp4_copy_char32(table->format, compression);
	table->width = frame_w;
	table->height = frame_h;
	table->frames_per_sample = 1;
	table->depth = 24;
	table->ctab_id = 65535;
}

void mp4_stsd_init_audio(mp4_infor_t *file, 
							mp4_stsd_t *stsd, 
							uint32 channels,
							uint32 bits, 
							uint32  sample_rate, 
							char *compressor)
{
	mp4_stsd_table_t *table;
	mp4_stsd_init_table(stsd);
	table = &(stsd->table[0]);

	mp4_copy_char32(table->format, compressor);
	table->channels = channels;
	table->sample_size = bits;
	table->sample_rate = sample_rate;
}

void mp4_stsd_delete(mp4_infor_t *file, mp4_stsd_t *stsd)
{
	uint32 i;
	if(stsd->total_entries)
	{
		for(i = 0; i < stsd->total_entries; i++)
			mp4_stsd_table_delete(&(stsd->table[i]));
		mp4_free(stsd->table);
	}

	stsd->total_entries = 0;
}

void mp4_stsd_dump(void *minf_ptr, mp4_stsd_t *stsd)
{
	uint32 i;
	MP4_DEMUX_MSG(("sample description\n"));
	MP4_DEMUX_MSG(("version %d\n", stsd->version));
	MP4_DEMUX_MSG(("flags %d\n", stsd->flags));
	MP4_DEMUX_MSG(("total_entries %d\n", stsd->total_entries));
	
	for(i = 0; i < stsd->total_entries; i++)
	{
		mp4_stsd_table_dump(minf_ptr, &(stsd->table[i]));
	}
}

uint32 mp4_read_stsd(mp4_infor_t *file, mp4_minf_t *minf, mp4_stsd_t *stsd)
{
	uint32 i, result;

	result = mp4_read_char(file, &stsd->version);
	if(result) return result;
	result = mp4_read_uint24(file, &stsd->flags);
	if(result) return result;
	result = mp4_read_uint32(file, &stsd->total_entries);
	if(result) return result;
	
	stsd->table = (mp4_stsd_table_t*)mp4_malloc(sizeof(mp4_stsd_table_t) * stsd->total_entries);
	for(i = 0; i < stsd->total_entries; i++)
	{
		mp4_stsd_table_init(&(stsd->table[i]));
		result = mp4_read_stsd_table(file, minf, &(stsd->table[i]));
		if(result) return result;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_write_stsd(mp4_infor_t *file, mp4_minf_t *minf, mp4_stsd_t *stsd)
{
	mp4_atom_t atom;
	uint32 i, result;

	result = mp4_atom_write_header(file, &atom, "stsd");
	if(result) return result;
	
	result = mp4_write_char(file, stsd->version);
	if(result) return result;
	result = mp4_write_uint24(file, stsd->flags);
	if(result) return result;
	result = mp4_write_uint32(file, stsd->total_entries);
	if(result) return result;
	
	for(i = 0; i < stsd->total_entries; i++)
	{
		
		result = mp4_write_stsd_table(file, minf, stsd->table);
		if(result) return result;
	}
	return mp4_atom_write_footer(file, &atom);
}




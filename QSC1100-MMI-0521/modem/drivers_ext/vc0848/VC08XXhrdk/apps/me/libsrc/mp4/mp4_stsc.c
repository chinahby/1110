/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	stsc.c  
* User for : 	mp4 demux mux, stsc atom is leaf atom of stbl atom, it is mandatory
*			sample to chunk table
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"

void mp4_stsc_init(mp4_stsc_t *stsc)
{
	/*these member is stsc table mandatory*/
	stsc->version = 0;
	stsc->flags = 0;
	stsc->total_entries = 0;
	/* used by the library for allocating a table */
	stsc->table = NULL;
	stsc->entries_allocated = 0;
	/*these member define for mux because memory limited*/
	stsc->start_position= 0;		//stss start position index in file 
	stsc->end_position = 0;		//stss end position index in file 
	stsc->start_entries = 0;
	stsc->start_samples = 0;
	stsc->end_samples = 0;
	stsc->start_chunk = 0;
	stsc->end_chunk = 0;
	stsc->last_chunk = 1;
	stsc->last_chunk_sample = 0;
	/*these member define for creat tmp table when mux file*/
	stsc->is_video = 0;  			//stsz table video flag, will to video tmp table file
	stsc->is_audio = 0;  			//stsz table audio flag, will to audio tmp table file 
}

void mp4_stsc_init_common(mp4_infor_t *file, mp4_stsc_t *stsc)
{
	mp4_stsc_table_t *table;
	
	if(stsc->table==NULL)
	{
		stsc->entries_allocated = 1;
		stsc->table = (mp4_stsc_table_t*)mp4_malloc(sizeof(mp4_stsc_table_t) * stsc->entries_allocated);
	}
	table = &(stsc->table[0]);
	table->chunk = 1;
	table->samples = 0;         /* set this after completion or after every audio chunk is written */
	table->id = 1;
	stsc->total_entries = 1;
}

void mp4_stsc_delete(mp4_infor_t *file, mp4_stsc_t *stsc)
{
	if(stsc->table!=NULL)
		mp4_free(stsc->table);
	stsc->entries_allocated = 0;
	stsc->total_entries = 0;
}

void mp4_stsc_dump(mp4_stsc_t *stsc)
{
	MP4_DEMUX_MSG(("sample to chunk table\n"));
	MP4_DEMUX_MSG(("version %d\n", stsc->version));
	MP4_DEMUX_MSG(("flags %d\n", stsc->flags));
	MP4_DEMUX_MSG(("total_entries %d\n", stsc->total_entries));
}

uint32 mp4_read_stsc(mp4_infor_t *file, mp4_stsc_t *stsc)
{
	uint32 result;

	result = mp4_read_char(file, &stsc->version);
	if(result) return result;
	result = mp4_read_uint24(file, &stsc->flags);
	if(result) return result;
	result = mp4_read_uint32(file, &stsc->total_entries);
	if(result) return result;

	if(stsc->total_entries)
	{
#if MP4_IN_820_MEMORY_LIMITED
		//only malloc this memory once time each file
		if ((stsc->total_entries-stsc->start_entries)<=STSC_MAX_ENTRIES)
			stsc->entries_allocated = stsc->total_entries-stsc->start_entries;
		else
       		stsc->entries_allocated = STSC_MAX_ENTRIES;
		stsc->table = (mp4_stsc_table_t*)mp4_malloc(sizeof(mp4_stsc_table_t) * stsc->entries_allocated);
#else
		stsc->entries_allocated = stsc->total_entries-stsc->start_entries;
		stsc->table = (mp4_stsc_table_t*)mp4_malloc(sizeof(mp4_stsc_table_t) * stsc->entries_allocated);
#endif
		//read stsc table first time
		stsc->start_position = stsc->start_position+8+8;
		return  mp4_read_stsc_table(file, stsc,1);
	}
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}

void mp4_uptate_stsc_table_infor(mp4_stsc_t *stsc)
{
	uint32 i;
	uint32  chunk, last_chunk, current_chunk;
	uint32  last_chunksamples, total = stsc->start_samples;

	last_chunk = stsc->last_chunk;
	last_chunksamples = stsc->last_chunk_sample;
	i = 0;
	do
	{
		current_chunk = stsc->table[i].chunk;
		chunk = current_chunk - last_chunk;
		total += chunk * last_chunksamples;

		last_chunksamples =  stsc->table[i].samples;
		last_chunk = current_chunk;
		i++;
	}while(i < stsc->entries_allocated);
	stsc->end_samples = total;
}

uint32 mp4_read_stsc_table(mp4_infor_t *file, mp4_stsc_t *stsc, uint32 restart)
{
	uint32 i, result;
	
	if(restart)
	{
		stsc->start_entries = 0;
		stsc->start_samples = stsc->end_samples =0;
		stsc->last_chunk = 1;
		stsc->last_chunk_sample = 0;
	}
	else
	{
		stsc->start_entries += stsc->entries_allocated;
		stsc->start_samples = stsc->end_samples ;
		stsc->last_chunk = stsc->table[stsc->entries_allocated-1].chunk;
		stsc->last_chunk_sample = stsc->table[stsc->entries_allocated-1].samples;
	}
	
#if MP4_IN_820_MEMORY_LIMITED
	if ((stsc->total_entries-stsc->start_entries)<=STSC_MAX_ENTRIES)
		stsc->entries_allocated = stsc->total_entries-stsc->start_entries;
	else
       	stsc->entries_allocated = STSC_MAX_ENTRIES;
#endif

	result = mp4_set_file_position(file,stsc->start_position+stsc->start_entries*12);
	if(result) return result;
	
	result = mp4_read_data(file, (char *)stsc->table, stsc->entries_allocated*12);
	if(result !=stsc->entries_allocated*12) return MP4_DEMUX_READ_FILE_ERROR;

	for(i = 0; i < stsc->entries_allocated; i++)
	{
		stsc->table[i].chunk = mp4_swap_uint32(stsc->table[i].chunk );
		stsc->table[i].samples = mp4_swap_uint32(stsc->table[i].samples);
		stsc->table[i].id = mp4_swap_uint32(stsc->table[i].id);
	}
	mp4_uptate_stsc_table_infor(stsc);
	return MP4_MUX_DEMUX_RET_NOERR;
	
}

#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_video_stsc_table(mp4_infor_t *file, mp4_stsc_t *stsc, uint32 restart)
{
	uint32 i, result;
	
	if(restart)
	{
		stsc->start_entries = 0;
		stsc->start_samples = stsc->end_samples =0;
		stsc->last_chunk = 1;
		stsc->last_chunk_sample = 0;
	}
	else
	{
		stsc->start_entries += stsc->entries_allocated;
		stsc->start_samples = stsc->end_samples ;
		stsc->last_chunk = stsc->table[stsc->entries_allocated-1].chunk;
		stsc->last_chunk_sample = stsc->table[stsc->entries_allocated-1].samples;
	}
	
#if MP4_IN_820_MEMORY_LIMITED
	if ((stsc->total_entries-stsc->start_entries)<=STSC_MAX_ENTRIES)
		stsc->entries_allocated = stsc->total_entries-stsc->start_entries;
	else
       	stsc->entries_allocated = STSC_MAX_ENTRIES;
#endif

//	result = mp4_set_file_position(file,stsc->start_position+stsc->start_entries*12);
//	if(result) return result;
	file->seek_func(file->stream_videoindx,stsc->start_position+stsc->start_entries*12,SEEK_SET);
	
//	result = mp4_read_data(file, (char *)stsc->table, stsc->entries_allocated*12);
	result = mp4_read_videoindx(file, (char *)stsc->table, stsc->entries_allocated*12);
	if(result !=stsc->entries_allocated*12) return MP4_DEMUX_READ_FILE_ERROR;

	for(i = 0; i < stsc->entries_allocated; i++)
	{
		stsc->table[i].chunk = mp4_swap_uint32(stsc->table[i].chunk );
		stsc->table[i].samples = mp4_swap_uint32(stsc->table[i].samples);
		stsc->table[i].id = mp4_swap_uint32(stsc->table[i].id);
	}
	mp4_uptate_stsc_table_infor(stsc);
	return MP4_MUX_DEMUX_RET_NOERR;
	
}

uint32 mp4_read_audio_stsc_table(mp4_infor_t *file, mp4_stsc_t *stsc, uint32 restart)
{
	uint32 i, result;
	
	if(restart)
	{
		stsc->start_entries = 0;
		stsc->start_samples = stsc->end_samples =0;
		stsc->last_chunk = 1;
		stsc->last_chunk_sample = 0;
	}
	else
	{
		stsc->start_entries += stsc->entries_allocated;
		stsc->start_samples = stsc->end_samples ;
		stsc->last_chunk = stsc->table[stsc->entries_allocated-1].chunk;
		stsc->last_chunk_sample = stsc->table[stsc->entries_allocated-1].samples;
	}
	
#if MP4_IN_820_MEMORY_LIMITED
	if ((stsc->total_entries-stsc->start_entries)<=STSC_MAX_ENTRIES)
		stsc->entries_allocated = stsc->total_entries-stsc->start_entries;
	else
       	stsc->entries_allocated = STSC_MAX_ENTRIES;
#endif

//	result = mp4_set_file_position(file,stsc->start_position+stsc->start_entries*12);
//	if(result) return result;
	file->seek_func(file->stream_audioindx,stsc->start_position+stsc->start_entries*12,SEEK_SET);
	
//	result = mp4_read_data(file, (char *)stsc->table, stsc->entries_allocated*12);
	result = mp4_read_audioindx(file, (char *)stsc->table, stsc->entries_allocated*12);
	if(result !=stsc->entries_allocated*12) return MP4_DEMUX_READ_FILE_ERROR;

	for(i = 0; i < stsc->entries_allocated; i++)
	{
		stsc->table[i].chunk = mp4_swap_uint32(stsc->table[i].chunk );
		stsc->table[i].samples = mp4_swap_uint32(stsc->table[i].samples);
		stsc->table[i].id = mp4_swap_uint32(stsc->table[i].id);
	}
	mp4_uptate_stsc_table_infor(stsc);
	return MP4_MUX_DEMUX_RET_NOERR;
	
}
#endif

#if MP4_IN_820_MEMORY_LIMITED
uint32 mp4_write_stsc(mp4_infor_t *file, mp4_stsc_t *stsc)
{
	int i, result;
	mp4_atom_t atom;
	
	result = mp4_atom_write_header(file, &atom, "stsc");
	if(result) return result;
	result = mp4_write_char(file, stsc->version);
	if(result) return result;
	result = mp4_write_uint24(file, stsc->flags);
	if(result) return result;
	result = mp4_write_uint32(file, stsc->total_entries);
	if(result) return result;

	for(i = 0; i < stsc->total_entries; i++)
	{
		result = mp4_write_uint32(file, stsc->table[i].chunk);
		if(result) return result;
		result = mp4_write_uint32(file, stsc->table[i].samples);
		if(result) return result;
		result = mp4_write_uint32(file, stsc->table[i].id);
		if(result) return result;
	}
	return mp4_atom_write_footer(file, &atom);
}

uint32 mp4_update_stsc(mp4_infor_t *file,mp4_stsc_t *stsc, uint32  chunk, uint32  samples)
{
	stsc->table[0].samples = 1;
	stsc->table[0].chunk = 1;
	stsc->table[0].id = 1;
	stsc->total_entries = 1;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_write_stsc(mp4_infor_t *file, mp4_stsc_t *stsc)
{
	int i, last_same, result;
	mp4_atom_t atom;
	
	result = mp4_atom_write_header(file, &atom, "stsc");
	if(result) return result;

	for(i = 1, last_same = 0; i < stsc->total_entries; i++)
	{
		if(stsc->table[i].samples != stsc->table[last_same].samples)
		{
			last_same++;
			if(last_same < i)
			{
				stsc->table[last_same] = stsc->table[i];
			}
		}
	}
	last_same++;
	stsc->total_entries = last_same;

	result = mp4_write_char(file, stsc->version);
	if(result) return result;
	result = mp4_write_uint24(file, stsc->flags);
	if(result) return result;
	result = mp4_write_uint32(file, stsc->total_entries);
	if(result) return result;
	
	for(i = 0; i < stsc->total_entries; i++)
	{
		result = mp4_write_uint32(file, stsc->table[i].chunk);
		if(result) return result;
		result = mp4_write_uint32(file, stsc->table[i].samples);
		if(result) return result;
		result = mp4_write_uint32(file, stsc->table[i].id);
		if(result) return result;
	}
	return mp4_atom_write_footer(file, &atom);
}

uint32 mp4_update_stsc(mp4_infor_t *file,mp4_stsc_t *stsc, uint32  chunk, uint32  samples)
{
	mp4_stsc_table_t *new_table;
	uint32 i;

	if(chunk > stsc->entries_allocated)
	{
		stsc->entries_allocated = chunk * 2;
		new_table = (mp4_stsc_table_t*)mp4_malloc(sizeof(mp4_stsc_table_t) * stsc->entries_allocated);
		for(i = 0; i < stsc->total_entries; i++) new_table[i] = stsc->table[i];
		mp4_free(stsc->table);
		stsc->table = new_table;
	}
	stsc->table[chunk - 1].samples = samples;
	stsc->table[chunk - 1].chunk = chunk;
	stsc->table[chunk - 1].id = 1;
	
	stsc->total_entries = chunk;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif


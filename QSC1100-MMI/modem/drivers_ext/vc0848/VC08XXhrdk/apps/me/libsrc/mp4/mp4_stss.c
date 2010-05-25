/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	stss.c  
* User for : 	mp4 demux mux, stss atom is leaf atom of stbl atom, it is mandatory
*			sync sample table(use foe random access points)
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"


void mp4_stss_init(mp4_stss_t *stss)
{
	/*these member is stss table mandatory*/
	stss->version = 0;
	stss->flags = 0;
	stss->total_entries = 0;
	/* used by the library for allocating a table */
	stss->table = NULL;
	stss->entries_allocated = 0;		
	/*these member define for demux because memory limited*/
	stss->start_position= 0;		//stss table start position in file
	stss->end_position = 0;		//stss table end position in file
	/*these member define for demux because memory limited*/
	stss->start_entries = 0;		//stss start entries index in memory 
	stss->start_samples = 0;		//stss start sample index in memory 
	stss->end_samples = 0;		//stss end sample index in memory 
}

void mp4_stss_init_common(mp4_infor_t *file, mp4_stss_t *stss)
{
	if (stss->table == NULL) 
	{
		stss->entries_allocated = STSS_MAX_ENTRIES;
		stss->table = (mp4_stss_table_t*)mp4_malloc(sizeof(mp4_stss_table_t) * stss->entries_allocated);
		//file->sram_stss_table=(mp4_stss_table_t*)mp4_malloc_h2d(sizeof(mp4_stss_table_t) * stss->entries_allocated);
	}
	stss->total_entries = 0;
}

void mp4_stss_delete(mp4_infor_t *file, mp4_stss_t *stss)
{
	if(stss->table!=NULL) 
		mp4_free(stss->table);
	/*if(file->sram_stss_table!=NULL) 
		mp4_free_h2d((void *)file->sram_stss_table);*/
	stss->entries_allocated = 0;
	stss->total_entries = 0;
}

void mp4_stss_dump(mp4_stss_t *stss)
{
	MP4_DEMUX_ERR(("      sync sample table\n"));
	MP4_DEMUX_ERR(("      version %d\n", stss->version));
	MP4_DEMUX_ERR(("      flags %d\n", stss->flags));
	MP4_DEMUX_ERR(("      total_entries %d\n", stss->total_entries));
}

uint32 mp4_read_stss(mp4_infor_t *file, mp4_stss_t *stss)
{
	uint32 result;

	result = mp4_read_char(file, &stss->version);
	if(result) return result;
	result = mp4_read_uint24(file, &stss->flags);
	if(result) return result;
	result = mp4_read_uint32(file, &stss->total_entries);

	if(stss->total_entries)
	{
#if MP4_IN_820_MEMORY_LIMITED
		//only malloc this memory once time each file
		if ((stss->total_entries-stss->start_entries)<=STSS_MAX_ENTRIES)
			stss->entries_allocated = stss->total_entries-stss->start_entries;
		else
       		stss->entries_allocated = STSS_MAX_ENTRIES;
		stss->table = (mp4_stss_table_t*)mp4_malloc(sizeof(mp4_stss_table_t) * stss->entries_allocated);
#else
		stss->entries_allocated = stss->total_entries-stss->start_entries;
		stss->table = (mp4_stss_table_t*)mp4_malloc(sizeof(mp4_stss_table_t) * stss->entries_allocated);
#endif
		//read stss table first time
		stss->start_position = stss->start_position+8+8;
		return mp4_read_stss_table(file, stss, 1);
	}
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_read_stss_table(mp4_infor_t *file, mp4_stss_t *stss, uint32 restart)
{
	uint32 i,result;

	if(!stss->total_entries)
		return MP4_MUX_DEMUX_RET_NOERR;
	if(restart)
		stss->start_entries = 0;
	else
		stss->start_entries += stss->entries_allocated;
	
#if MP4_IN_820_MEMORY_LIMITED
	if ((stss->total_entries-stss->start_entries)<=STSS_MAX_ENTRIES)
		stss->entries_allocated = stss->total_entries-stss->start_entries;
	else
       	stss->entries_allocated = STSS_MAX_ENTRIES;
#endif

     	result = mp4_set_file_position(file,stss->start_position+stss->start_entries*4);
	if(result) return result;
	
	result = mp4_read_data(file, (char *)stss->table, stss->entries_allocated*4);
	if(result != stss->entries_allocated*4)  return MP4_DEMUX_READ_FILE_ERROR;

	for(i = 0; i < stss->entries_allocated; i++)
	{
		stss->table[i].sample = mp4_swap_uint32(stss->table[i].sample);
	}
	
	stss->start_samples = stss->table[0].sample;
	stss->end_samples = stss->table[stss->entries_allocated-1].sample;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_video_stss_table(mp4_infor_t *file, mp4_stss_t *stss, uint32 restart)
{
	uint32 i,result;

	if(!stss->total_entries)
		return MP4_MUX_DEMUX_RET_NOERR;
	if(restart)
		stss->start_entries = 0;
	else
		stss->start_entries += stss->entries_allocated;
	
#if MP4_IN_820_MEMORY_LIMITED
	if ((stss->total_entries-stss->start_entries)<=STSS_MAX_ENTRIES)
		stss->entries_allocated = stss->total_entries-stss->start_entries;
	else
       	stss->entries_allocated = STSS_MAX_ENTRIES;
#endif

//     result = mp4_set_file_position(file,stss->start_position+stss->start_entries*4);
//	if(result) return result;
	file->seek_func(file->stream_videoindx,stss->start_position+stss->start_entries*4,SEEK_SET);

//	result = mp4_read_data(file, (char *)stss->table, stss->entries_allocated*4);
	result = mp4_read_videoindx(file, (char *)stss->table, stss->entries_allocated*4);
	if(result != stss->entries_allocated*4)  return MP4_DEMUX_READ_FILE_ERROR;

	for(i = 0; i < stss->entries_allocated; i++)
	{
		stss->table[i].sample = mp4_swap_uint32(stss->table[i].sample);
	}
	
	stss->start_samples = stss->table[0].sample;
	stss->end_samples = stss->table[stss->entries_allocated-1].sample;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_read_audio_stss_table(mp4_infor_t *file, mp4_stss_t *stss, uint32 restart)
{
	uint32 i,result;

	if(!stss->total_entries)
		return MP4_MUX_DEMUX_RET_NOERR;
	if(restart)
		stss->start_entries = 0;
	else
		stss->start_entries += stss->entries_allocated;
	
#if MP4_IN_820_MEMORY_LIMITED
	if ((stss->total_entries-stss->start_entries)<=STSS_MAX_ENTRIES)
		stss->entries_allocated = stss->total_entries-stss->start_entries;
	else
       	stss->entries_allocated = STSS_MAX_ENTRIES;
#endif

//     	result = mp4_set_file_position(file,stss->start_position+stss->start_entries*4);
//	if(result) return result;
	file->seek_func(file->stream_audioindx,stss->start_position+stss->start_entries*4,SEEK_SET);

//	result = mp4_read_data(file, (char *)stss->table, stss->entries_allocated*4);
	result = mp4_read_audioindx(file, (char *)stss->table, stss->entries_allocated*4);
	if(result != stss->entries_allocated*4)  return MP4_DEMUX_READ_FILE_ERROR;

	for(i = 0; i < stss->entries_allocated; i++)
	{
		stss->table[i].sample = mp4_swap_uint32(stss->table[i].sample);
	}
	
	stss->start_samples = stss->table[0].sample;
	stss->end_samples = stss->table[stss->entries_allocated-1].sample;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

#if MP4_IN_820_ENCODE_HARDWARE
uint32 mp4_write_stss(mp4_infor_t *file, mp4_stss_t *stss)
{
	uint32 i, result;
	mp4_atom_t atom;

	if(!stss->total_entries)
		return MP4_MUX_DEMUX_RET_NOERR;
	//always audio have no the table, only video have the table
	result = mp4_atom_write_header(file, &atom, "stss");
	if(result) return result;
		
	result = mp4_write_char(file, stss->version);
	if(result) return result;
	result = mp4_write_uint24(file, stss->flags);
	if(result) return result;
	result = mp4_write_uint32(file, stss->total_entries);
	if(result) return result;

	stss->start_entries = 0;
	while(stss->total_entries>=STSS_MAX_ENTRIES)
    {
		for(i = 0; i < STSS_MAX_ENTRIES; i++)
			stss->table[i].sample = mp4_swap_uint32(stss->start_entries*MP4_IN_820_SYN_FRAME_TICKS+i*MP4_IN_820_SYN_FRAME_TICKS+1); 
		result = mp4_write_data(file, (char *)stss->table, STSS_MAX_ENTRIES*4);
		if(result !=STSS_MAX_ENTRIES*4) return MP4_MUX_WRITE_FILE_ERROR;
		stss->total_entries -= STSS_MAX_ENTRIES;
		stss->start_entries += STSS_MAX_ENTRIES;
	}
	if(stss->total_entries)
	{
		for(i = 0; i < stss->total_entries; i++)
			stss->table[i].sample = mp4_swap_uint32(stss->start_entries*MP4_IN_820_SYN_FRAME_TICKS+i*MP4_IN_820_SYN_FRAME_TICKS+1);
		result = mp4_write_data(file, (char *)stss->table, stss->total_entries*4);
		if(result !=stss->total_entries*4) return MP4_MUX_WRITE_FILE_ERROR;
		stss->total_entries -= stss->total_entries;
	}
	return mp4_atom_write_footer(file, &atom);
}

uint32 mp4_update_stss(mp4_infor_t *file, mp4_stss_t *stss, uint32  sample)
{
	stss->end_samples = sample;
	stss->total_entries = sample / MP4_IN_820_SYN_FRAME_TICKS + 1;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_write_stss(mp4_infor_t *file, mp4_stss_t *stss)
{
	uint32 i, result;
	mp4_atom_t atom;

	if(stss->total_entries)
	{
		result = mp4_atom_write_header(file, &atom, "stss");
		if(result) return result;
		
		result = mp4_write_char(file, stss->version);
		if(result) return result;
		result = mp4_write_uint24(file, stss->flags);
		if(result) return result;
		result = mp4_write_uint32(file, stss->total_entries);
		if(result) return result;
		
		for(i = 0; i < stss->total_entries; i++)
		{
			result =mp4_write_uint32(file, stss->table[i].sample);
			if(result) return result;
 		}

		result =mp4_atom_write_footer(file, &atom);
		if(result) return result;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_update_stss(mp4_infor_t *file, mp4_stss_t *stss, uint32  sample)
{
	mp4_stss_table_t *new_table;
	uint32 i;
	
	if (stss->total_entries >= stss->entries_allocated) 
	{
		stss->entries_allocated *= 2;
		new_table = (mp4_stss_table_t*)mp4_malloc(sizeof(mp4_stss_table_t) * stss->entries_allocated);
		for(i = 0; i < stss->total_entries; i++) new_table[i] = stss->table[i];
		mp4_free(stss->table);
		stss->table = new_table;
	}
	stss->table[stss->total_entries++].sample = sample;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

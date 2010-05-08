/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	stco.c  
* User for : 	mp4 demux mux, stco atom is leaf atom of stbl atom, it is mandatory
*			chunk offset , partial data offset information
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"

void mp4_stco_init(mp4_stco_t *stco)
{
	/*these member is stco table mandatory*/
	stco->version = 0;
	stco->flags = 0;
	stco->total_entries = 0;
	/* used by the library for allocating a table */
	stco->table = NULL;
	stco->entries_allocated = 0;   
	/*these member define for demux because memory limited*/
	stco->start_position= 0;		//stss start position index in file 
	stco->end_position = 0;		//stss start position index in file 
	stco->start_entries = 0;		//stss start entries index in memory 
	stco->start_chunk = 1;			//stss start chunk index in memory 
	stco->end_chunk = 1;			//stss end chunk index in memory 
	stco->end_offset = 0;
	/*these member define for write tmp table when mux file*/
	stco->is_video = 0;  			//stsz table video flag, will to video tmp table file
	stco->is_audio = 0;  			//stsz table audio flag, will to audio tmp table file 
}

void mp4_stco_delete(mp4_infor_t *file, mp4_stco_t *stco)
{
	if(stco->table!=NULL)
		mp4_free(stco->table);
	if(file->sram_stco_table_video!=NULL)
	{
		mp4_free_h2d((void *)file->sram_stco_table_video);
		file->sram_stco_table_video=NULL;
	}
	if(file->sram_stco_table_audio!=NULL)
	{
		mp4_free_h2d((void *)file->sram_stco_table_audio);
		file->sram_stco_table_audio=NULL;
	}
	stco->total_entries = 0;
	stco->entries_allocated = 0;
}

void mp4_stco_init_common_video(mp4_infor_t *file, mp4_stco_t *stco)
{
	if(stco->table==NULL)
	{
		stco->entries_allocated = STCO_MAX_ENTRIES;
		stco->total_entries = 0;
		stco->table = (mp4_stco_table_t*)mp4_malloc(sizeof(mp4_stco_table_t) * stco->entries_allocated);
		file->sram_stco_table_video=(mp4_stco_table_t*)mp4_malloc_h2d(sizeof(mp4_stco_table_t) * stco->entries_allocated);
	}
}

void mp4_stco_init_common_audio(mp4_infor_t *file, mp4_stco_t *stco)
{
	if(stco->table==NULL)
	{
		stco->entries_allocated = STCO_MAX_ENTRIES;
		stco->total_entries = 0;
		stco->table = (mp4_stco_table_t*)mp4_malloc(sizeof(mp4_stco_table_t) * stco->entries_allocated);
		file->sram_stco_table_audio=(mp4_stco_table_t*)mp4_malloc_h2d(sizeof(mp4_stco_table_t) * stco->entries_allocated);
	}
}


void mp4_stco_dump(mp4_stco_t *stco)
{
	MP4_DEMUX_MSG(("chunk offset table\n"));
	MP4_DEMUX_MSG(("version %d\n", stco->version));
	MP4_DEMUX_MSG(("flags %d\n", stco->flags));
	MP4_DEMUX_MSG(("total_entries %d\n", stco->total_entries));
}

uint32 mp4_read_stco(mp4_infor_t *file, mp4_stco_t *stco)
{
	uint32 result;
	
	result = mp4_read_char(file, &stco->version);
	if(result) return result;
	result = mp4_read_uint24(file, &stco->flags);
	if(result) return result;
	result = mp4_read_uint32(file, &stco->total_entries);
	if(result) return result;
	
	if(stco->total_entries)
	{
#if MP4_IN_820_MEMORY_LIMITED
		//only malloc this memory once time each file
		if ((stco->total_entries-stco->start_entries)<=STCO_MAX_ENTRIES)
			stco->entries_allocated = stco->total_entries-stco->start_entries;
       	else
       		stco->entries_allocated = STCO_MAX_ENTRIES;
      		stco->table = (mp4_stco_table_t*)mp4_malloc(sizeof(mp4_stco_table_t) * stco->entries_allocated);
#else
		stco->entries_allocated = stco->total_entries-stco->start_entries;
		stco->table = (mp4_stco_table_t*)mp4_malloc(sizeof(mp4_stco_table_t) * stco->entries_allocated);
#endif
		//read stco table first time
		stco->start_position = stco->start_position+8+8;
		return mp4_read_stco_table(file, stco,1);
	}
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_read_stco_table(mp4_infor_t *file, mp4_stco_t *stco, uint32 restart)
{
	uint32 i,result;

	if(restart)
		stco->start_entries = 0;
	else
		stco->start_entries += stco->entries_allocated;
	
#if MP4_IN_820_MEMORY_LIMITED
	if ((stco->total_entries-stco->start_entries)<=STCO_MAX_ENTRIES)
		stco->entries_allocated = stco->total_entries-stco->start_entries;
      	else
      		stco->entries_allocated = STCO_MAX_ENTRIES;
 #endif      
 
	result = mp4_set_file_position(file,stco->start_position+stco->start_entries*4);
	if(result) return result;
	
	result = mp4_read_data(file, (char *)stco->table, stco->entries_allocated*4);
	if(result!=stco->entries_allocated*4) return MP4_DEMUX_READ_FILE_ERROR;
	
	for(i = 0; i < stco->entries_allocated; i++)
	{
		stco->table[i].offset = mp4_swap_uint32(stco->table[i].offset);
	}
	stco->start_chunk = stco->start_entries+1;
	stco->end_chunk = stco->start_entries + stco->entries_allocated;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#if MP4LIB_VERSION2_FOREHANDLE 
uint32 mp4_read_video_stco_table(mp4_infor_t *file, mp4_stco_t *stco, uint32 restart)
{
	uint32 i,result;

	if(restart)
		stco->start_entries = 0;
	else
		stco->start_entries += stco->entries_allocated;
	
#if MP4_IN_820_MEMORY_LIMITED
	if ((stco->total_entries-stco->start_entries)<=STCO_MAX_ENTRIES)
		stco->entries_allocated = stco->total_entries-stco->start_entries;
      	else
      		stco->entries_allocated = STCO_MAX_ENTRIES;
 #endif      
 
//	result = mp4_set_file_position(file,stco->start_position+stco->start_entries*4);
//	if(result) return result;
	file->seek_func(file->stream_videoindx,stco->start_position+stco->start_entries*4,SEEK_SET);
 
//	result = mp4_read_data(file, (char *)stco->table, stco->entries_allocated*4);
	result = mp4_read_videoindx(file, (char *)stco->table, stco->entries_allocated*4);
	if(result!=stco->entries_allocated*4) return MP4_DEMUX_READ_FILE_ERROR;
	
	for(i = 0; i < stco->entries_allocated; i++)
	{
		stco->table[i].offset = mp4_swap_uint32(stco->table[i].offset);
	}
	stco->start_chunk = stco->start_entries+1;
	stco->end_chunk = stco->start_entries + stco->entries_allocated;
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_read_audio_stco_table(mp4_infor_t *file, mp4_stco_t *stco, uint32 restart)
{
	uint32 i,result;

	if(restart)
		stco->start_entries = 0;
	else
		stco->start_entries += stco->entries_allocated;
	
#if MP4_IN_820_MEMORY_LIMITED
	if ((stco->total_entries-stco->start_entries)<=STCO_MAX_ENTRIES)
		stco->entries_allocated = stco->total_entries-stco->start_entries;
      	else
      		stco->entries_allocated = STCO_MAX_ENTRIES;
 #endif      
 
//	result = mp4_set_file_position(file,stco->start_position+stco->start_entries*4);
//	if(result) return result;
	file->seek_func(file->stream_audioindx,stco->start_position+stco->start_entries*4,SEEK_SET);
 
//	result = mp4_read_data(file, (char *)stco->table, stco->entries_allocated*4);
	result = mp4_read_audioindx(file, (char *)stco->table, stco->entries_allocated*4);
	if(result!=stco->entries_allocated*4) return MP4_DEMUX_READ_FILE_ERROR;
	
	for(i = 0; i < stco->entries_allocated; i++)
	{
		stco->table[i].offset = mp4_swap_uint32(stco->table[i].offset);
	}
	stco->start_chunk = stco->start_entries+1;
	stco->end_chunk = stco->start_entries + stco->entries_allocated;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

#if MP4_IN_820_MEMORY_LIMITED
uint32 mp4_write_stco(mp4_infor_t *file, mp4_stco_t *stco)
{
	uint32 result;
	mp4_atom_t atom;
	
	result = mp4_atom_write_header(file, &atom, "stco");
	if(result) return result;
	
	result = mp4_write_char(file, stco->version);
	if(result) return result;
	result = mp4_write_uint24(file, stco->flags);
	if(result) return result;
	result = mp4_write_uint32(file, stco->total_entries);
	if(result) return result;

	//write stco table form table file
	if(stco->is_video)
	{
		mp4_fileseek(file->table_video, stco->start_entries*8, SEEK_SET);
		mp4_filewrite(file->table_video, (char*)stco->table, (stco->total_entries-stco->start_entries)*4);
		mp4_fileseek(file->table_video, 0, SEEK_SET);
	}
	if(stco->is_audio)
	{
		mp4_fileseek(file->table_audio, stco->start_entries*8, SEEK_SET);
		mp4_filewrite(file->table_audio, (char*)stco->table, (stco->total_entries-stco->start_entries)*4);
		mp4_fileseek(file->table_audio, 0, SEEK_SET);
	}
	while(stco->total_entries>=STCO_MAX_ENTRIES)
       {
		if(stco->is_video)
		{
			mp4_fileread(file->table_video, (char *)stco->table, STCO_MAX_ENTRIES*4);
			mp4_fileseek(file->table_video, STCO_MAX_ENTRIES*4, SEEK_CUR);
		}
		if(stco->is_audio)
		{
			mp4_fileread(file->table_audio, (char *)stco->table, STCO_MAX_ENTRIES*4);
			mp4_fileseek(file->table_audio, STCO_MAX_ENTRIES*4, SEEK_CUR);
		}
		result = mp4_write_data(file, (char *)stco->table, STCO_MAX_ENTRIES*4);
		if(result !=STCO_MAX_ENTRIES*4) return MP4_MUX_WRITE_FILE_ERROR;
		stco->total_entries -= STCO_MAX_ENTRIES;
	}
	if(stco->total_entries)
	{
		if(stco->is_video)
			mp4_fileread(file->table_video, (char *)stco->table, stco->total_entries*4);
		if(stco->is_audio)
			mp4_fileread(file->table_audio, (char *)stco->table, stco->total_entries*4);
		result = mp4_write_data(file, (char *)stco->table, stco->total_entries*4);
		if(result !=stco->total_entries*4) return MP4_MUX_WRITE_FILE_ERROR;
		stco->total_entries -= stco->total_entries;
	}
	return mp4_atom_write_footer(file, &atom);
}

uint32 mp4_update_stco(mp4_infor_t *file,mp4_stco_t *stco, uint32  chunk, uint32  offset)
{
	char * bufinhost,*bufindevice;
	if(chunk > (stco->start_entries+STCO_MAX_ENTRIES))
	{
		if(stco->is_video)
		{
			bufinhost=(char*)stco->table;
			bufindevice=(char*)file->sram_stco_table_video;
			mp4_memcpy_h2d(bufindevice,bufinhost,STCO_MAX_ENTRIES*4);
			mp4_filewrite_core(file->table_video, bufindevice, STCO_MAX_ENTRIES*4, MP4_IDX_VID_STATUS,27);
		}
		if(stco->is_audio)
		{
			bufinhost=(char*)stco->table;
			bufindevice=(char*)file->sram_stco_table_audio;
			mp4_memcpy_h2d(bufindevice,bufinhost,STCO_MAX_ENTRIES*4);
			mp4_filewrite_core(file->table_audio, bufindevice, STCO_MAX_ENTRIES*4, MP4_IDX_AUD_STATUS,26);
		}
		stco->start_entries+=STCO_MAX_ENTRIES;
	}
	stco->table[chunk -1-stco->start_entries].offset =mp4_swap_uint32(offset);
	stco->total_entries = chunk;
	stco->end_offset = offset;
	return MP4_MUX_DEMUX_RET_NOERR;
}

#else
uint32 mp4_write_stco(mp4_infor_t *file, mp4_stco_t *stco)
{
	uint32 i, result;
	mp4_atom_t atom;
	
	result = mp4_atom_write_header(file, &atom, "stco");
	if(result) return result;
	
	result = mp4_write_char(file, stco->version);
	if(result) return result;
	result = mp4_write_uint24(file, stco->flags);
	if(result) return result;
	result = mp4_write_uint32(file, stco->total_entries);
	if(result) return result;
	
	for(i = 0; i < stco->total_entries; i++)
	{
		result = mp4_write_uint32(file, stco->table[i].offset);
		if(result) return result;
	}
	return mp4_atom_write_footer(file, &atom);
}

uint32 mp4_update_stco(mp4_infor_t *file,mp4_stco_t *stco, uint32  chunk, uint32  offset)
{
	mp4_stco_table_t *new_table;
	uint32 i;
	
	if(chunk > stco->entries_allocated)
	{
		stco->entries_allocated = chunk * 2;
		new_table = (mp4_stco_table_t*)mp4_malloc(sizeof(mp4_stco_table_t) * stco->entries_allocated);
		for(i = 0; i < stco->total_entries; i++) new_table[i] = stco->table[i];
		mp4_free(stco->table);
		stco->table = new_table;
	}
	stco->table[chunk - 1].offset = offset;
	stco->total_entries = chunk;
	stco->end_offset = offset;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif


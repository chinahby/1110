/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	stsz.c  
* User for : 	mp4 demux mux, stsz atom is leaf atom of stbl atom, it is mandatory
*			sample size table.
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"

void mp4_stsz_init(mp4_stsz_t *stsz)
{
	/*these member is stsz table mandatory*/
	stsz->version = 0;  
	stsz->flags = 0;  
	stsz->sample_size = 0;  
	stsz->total_entries = 0;  
	/* used by the library for allocating a table */
	stsz->table = NULL; 
	stsz->entries_allocated = 0;    
	/*these member define for demux because memory limited*/
	stsz->start_position= 0;		//stsz table start position in file
	stsz->end_position = 0;		//stsz table end position in file
	stsz->start_entries = 0;		//stsz start entries index in memory  
	stsz->start_samples = 0;		//stsz start samples index in memory  
	stsz->end_samples = 0;		//stsz end samples index in memory  
	stsz->end_smpsize = 0;
	/*these member define for creat tmp table when mux file*/
	stsz->is_video = 0; 			//stsz table video flag, will to video tmp table file
	stsz->is_audio = 0; 			//stsz table audio flag, will to audio tmp table file 
}

void mp4_stsz_init_common_video(mp4_infor_t *file, mp4_stsz_t *stsz, uint32  sample_size)
{
	stsz->sample_size = sample_size;		/* if == 0, then use table */
	if(!stsz->entries_allocated)
	{
		stsz->entries_allocated = STSZ_MAX_ENTRIES;
		stsz->table = (mp4_stsz_table_t*)mp4_malloc(sizeof(mp4_stsz_table_t) * stsz->entries_allocated);
#if VIDEORECORDERIN848
		file->sram_stsz_table_video[0]=(mp4_stsz_table_t*)mp4_malloc_h2d(sizeof(mp4_stsz_table_t) * stsz->entries_allocated);
		file->sram_stsz_table_video[1]=(mp4_stsz_table_t*)mp4_malloc_h2d(sizeof(mp4_stsz_table_t) * stsz->entries_allocated);
		file->current_stsz_write=0;
#else
 		file->sram_stsz_table_video=(mp4_stsz_table_t*)mp4_malloc_h2d(sizeof(mp4_stsz_table_t) * stsz->entries_allocated);
#endif
	}
	stsz->total_entries = 0;   				/* set this when closing */
}

void mp4_stsz_init_common_audio(mp4_infor_t *file, mp4_stsz_t *stsz, uint32  sample_size)
{
	stsz->sample_size = sample_size;		/* if == 0, then use table */
	if(!stsz->entries_allocated)
	{
		stsz->entries_allocated = STSZ_MAX_ENTRIES;
		stsz->table = (mp4_stsz_table_t*)mp4_malloc(sizeof(mp4_stsz_table_t) * stsz->entries_allocated);
 		file->sram_stsz_table_audio=(mp4_stsz_table_t*)mp4_malloc_h2d(sizeof(mp4_stsz_table_t) * stsz->entries_allocated);
	}
	stsz->total_entries = 0;   				/* set this when closing */
}

#if VIDEORECORDERIN848
void mp4_stsz_delete(mp4_infor_t *file, mp4_stsz_t *stsz)
{
	uint32 i;
	if(stsz->table!=NULL) 
		mp4_free(stsz->table);
	for(i=0;i<2;i++)
	{
		if(file->sram_stsz_table_video[i]!=NULL) 
		{
			mp4_free_h2d((void *)file->sram_stsz_table_video[i]);
			file->sram_stsz_table_video[i]=NULL;
		}
	}
	if(file->sram_stsz_table_audio!=NULL)
	{
		mp4_free_h2d((void *)file->sram_stsz_table_audio);
		file->sram_stsz_table_audio=NULL;
	}		
	file->current_stsz_write=0;
	stsz->entries_allocated = 0;
	stsz->total_entries = 0;
}

#else
void mp4_stsz_delete(mp4_infor_t *file, mp4_stsz_t *stsz)
{
	if(stsz->table!=NULL) 
		mp4_free(stsz->table);
	if(file->sram_stsz_table_video!=NULL)
	{
		mp4_free_h2d((void *)file->sram_stsz_table_video);
		file->sram_stsz_table_video=0;
	}		
	if(file->sram_stsz_table_audio!=NULL)
	{
		mp4_free_h2d((void *)file->sram_stsz_table_audio);
		file->sram_stsz_table_audio=0;
	}		
	stsz->entries_allocated = 0;
	stsz->total_entries = 0;
}
#endif
void mp4_stsz_dump(mp4_stsz_t *stsz)
{
	MP4_DEMUX_ERR(("      sample size table\n"));
	MP4_DEMUX_ERR(("      version %d\n", stsz->version));
	MP4_DEMUX_ERR(("      flags %d\n", stsz->flags));
	MP4_DEMUX_ERR(("      sample_size %d\n", stsz->sample_size));
	MP4_DEMUX_ERR(("      total_entries %d\n", stsz->total_entries));
}

uint32 mp4_read_stsz(mp4_infor_t *file, mp4_stsz_t *stsz)
{
	uint32 result;
	
	result = mp4_read_char(file, &stsz->version);
	if(result) return result;
	result = mp4_read_uint24(file, &stsz->flags);
	if(result) return result;
	result = mp4_read_uint32(file, &stsz->sample_size);
	if(result) return result;
	result = mp4_read_uint32(file, &stsz->total_entries);
	if(result) return result;
	//all frames size is same
	if(stsz->sample_size)
		return MP4_MUX_DEMUX_RET_NOERR;
	//frames size is different	
	if(stsz->total_entries)
	{
#if MP4_IN_820_MEMORY_LIMITED
		//only malloc this memory once time each file
		if ((stsz->total_entries-stsz->start_entries)<=STSZ_MAX_ENTRIES)
			stsz->entries_allocated = stsz->total_entries-stsz->start_entries;
		else
       		stsz->entries_allocated = STSZ_MAX_ENTRIES;
		stsz->table = (mp4_stsz_table_t*)mp4_malloc(sizeof(mp4_stsz_table_t) * stsz->entries_allocated);
#else
		stsz->entries_allocated = stsz->total_entries-stsz->start_entries;
		stsz->table = (mp4_stsz_table_t*)mp4_malloc(sizeof(mp4_stsz_table_t) * stsz->entries_allocated);
#endif
		//read stsz table first time 
		stsz->start_position = stsz->start_position+8+12;
		return mp4_read_stsz_table(file, stsz, 1);
	}
	else
		return MP4_MUX_DEMUX_RET_NOERR;
	
}

uint32 mp4_read_stsz_table(mp4_infor_t *file, mp4_stsz_t *stsz, uint32 restart)
{
	uint32 i, result;

	if(restart)
		stsz->start_entries = 0;
	else
		stsz->start_entries += stsz->entries_allocated;

#if MP4_IN_820_MEMORY_LIMITED
	if ((stsz->total_entries-stsz->start_entries)<=STSZ_MAX_ENTRIES)
		stsz->entries_allocated = stsz->total_entries-stsz->start_entries;
	else
       	stsz->entries_allocated = STSZ_MAX_ENTRIES;
#endif	

	result = mp4_set_file_position(file,stsz->start_position+stsz->start_entries*4);
	if(result) return result;
	
	result = mp4_read_data(file, (char *)stsz->table, stsz->entries_allocated*4);
	if(result != stsz->entries_allocated*4)  return MP4_DEMUX_READ_FILE_ERROR;
	
	for(i = 0; i < stsz->entries_allocated; i++)
	{
		stsz->table[i].size = mp4_swap_uint32(stsz->table[i].size);
	}
	
	stsz->start_samples = stsz->start_entries;
	stsz->end_samples = stsz->start_entries + stsz->entries_allocated ;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#if (MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_read_video_stsz_table(mp4_infor_t *file, mp4_stsz_t *stsz, uint32 restart)
{
	uint32 i, result;

	if(restart)
		stsz->start_entries = 0;
	else
		stsz->start_entries += stsz->entries_allocated;

#if MP4_IN_820_MEMORY_LIMITED
	if ((stsz->total_entries-stsz->start_entries)<=STSZ_MAX_ENTRIES)
		stsz->entries_allocated = stsz->total_entries-stsz->start_entries;
	else
       	stsz->entries_allocated = STSZ_MAX_ENTRIES;
#endif	

//	result = mp4_set_file_position(file,stsz->start_position+stsz->start_entries*4);	
//	if(result) return result;
	file->seek_func(file->stream_videoindx, stsz->start_position+stsz->start_entries*4, SEEK_SET);

//	result = mp4_read_data(file, (char *)stsz->table, stsz->entries_allocated*4);
	result = mp4_read_videoindx(file, (char *)stsz->table, stsz->entries_allocated*4);
	if(result != stsz->entries_allocated*4)  return MP4_DEMUX_READ_FILE_ERROR;
	
	for(i = 0; i < stsz->entries_allocated; i++)
	{
		stsz->table[i].size = mp4_swap_uint32(stsz->table[i].size);
	}
	
	stsz->start_samples = stsz->start_entries;
	stsz->end_samples = stsz->start_entries + stsz->entries_allocated ;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_read_audio_stsz_table(mp4_infor_t *file, mp4_stsz_t *stsz, uint32 restart)
{
	uint32 i, result;

	if(restart)
		stsz->start_entries = 0;
	else
		stsz->start_entries += stsz->entries_allocated;

#if MP4_IN_820_MEMORY_LIMITED
	if ((stsz->total_entries-stsz->start_entries)<=STSZ_MAX_ENTRIES)
		stsz->entries_allocated = stsz->total_entries-stsz->start_entries;
	else
       	stsz->entries_allocated = STSZ_MAX_ENTRIES;
#endif	

//	result = mp4_set_file_position(file,stsz->start_position+stsz->start_entries*4);	
//	if(result) return result;
	file->seek_func(file->stream_audioindx, stsz->start_position+stsz->start_entries*4, SEEK_SET);

//	result = mp4_read_data(file, (char *)stsz->table, stsz->entries_allocated*4);
	result = mp4_read_audioindx(file, (char *)stsz->table, stsz->entries_allocated*4);
	if(result != stsz->entries_allocated*4)  return MP4_DEMUX_READ_FILE_ERROR;
	
	for(i = 0; i < stsz->entries_allocated; i++)
	{
		stsz->table[i].size = mp4_swap_uint32(stsz->table[i].size);
	}
	
	stsz->start_samples = stsz->start_entries;
	stsz->end_samples = stsz->start_entries + stsz->entries_allocated ;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

#if MP4_IN_820_MEMORY_LIMITED
uint32 mp4_write_stsz(mp4_infor_t *file, mp4_stsz_t *stsz)
{
	mp4_atom_t atom;
	uint32 result;
	
#if VIDEORECORDERIN848
	uint32 current;
	char *bufinhost,*bufindevice;
#endif

	result = mp4_atom_write_header(file, &atom, "stsz");
	if(result) return result;
	
	result = mp4_write_char(file, stsz->version);
	if(result) return result;
	result = mp4_write_uint24(file, stsz->flags);
	if(result) return result;
	result = mp4_write_uint32(file, stsz->sample_size);
	if(result) return result;
	result = mp4_write_uint32(file, stsz->total_entries);
	
	if(!stsz->sample_size)
	{
		if(stsz->is_video)
		{
			mp4_fileseek(file->table_video, stsz->start_entries*8+(stsz->total_entries-stsz->start_entries)*4, SEEK_SET);
#if VIDEORECORDERIN848
			current=file->current_stsz_write;
			bufinhost=(char*)stsz->table;
			bufindevice=(char*)file->sram_stsz_table_video[current];
			mp4_memcpy_d2h(bufinhost,bufindevice,(stsz->total_entries-stsz->start_entries)*4);
#endif
			mp4_filewrite(file->table_video, (char*)stsz->table, (stsz->total_entries-stsz->start_entries)*4);
			mp4_fileseek(file->table_video, 0, SEEK_SET);
		}
		if(stsz->is_audio)
		{
			mp4_fileseek(file->table_audio, stsz->start_entries*8+(stsz->total_entries-stsz->start_entries)*4, SEEK_SET);
			mp4_filewrite(file->table_audio, (char*)stsz->table, (stsz->total_entries-stsz->start_entries)*4);
			mp4_fileseek(file->table_audio, 0, SEEK_SET);
		}
       	while(stsz->total_entries>=STSZ_MAX_ENTRIES)
        {
			if(stsz->is_video)
			{
				mp4_fileseek(file->table_video, STSZ_MAX_ENTRIES*4, SEEK_CUR);
				mp4_fileread(file->table_video, (char *)stsz->table, STSZ_MAX_ENTRIES*4);
			}
			if(stsz->is_audio)
			{
				mp4_fileseek(file->table_audio, STSZ_MAX_ENTRIES*4, SEEK_CUR);
				mp4_fileread(file->table_audio, (char *)stsz->table, STSZ_MAX_ENTRIES*4);
			}
			result = mp4_write_data(file, (char *)stsz->table, STSZ_MAX_ENTRIES*4);
			if(result !=STSZ_MAX_ENTRIES*4) return MP4_MUX_WRITE_FILE_ERROR;
			stsz->total_entries -= STSZ_MAX_ENTRIES;
        }
       	if(stsz->total_entries)
       	{
    		if(stsz->is_video)
    		{
    			mp4_fileseek(file->table_video, stsz->total_entries*4, SEEK_CUR);
           		mp4_fileread(file->table_video, (char *)stsz->table, stsz->total_entries*4);
    		}
    		if(stsz->is_audio)
    		{
    			mp4_fileseek(file->table_audio, stsz->total_entries*4, SEEK_CUR);
    			mp4_fileread(file->table_audio, (char *)stsz->table, stsz->total_entries*4);
    		}
           	result = mp4_write_data(file, (char *)stsz->table, stsz->total_entries*4);
           	if(result !=stsz->total_entries*4) return MP4_MUX_WRITE_FILE_ERROR;
    		stsz->total_entries -= stsz->total_entries;
       	}
	}
	return mp4_atom_write_footer(file, &atom);
}
#if VIDEORECORDERIN848
uint32 mp4_update_stsz(mp4_infor_t *file,mp4_stsz_t *stsz, uint32  sample, uint32  sample_size)
{
	uint32 current;
	char * bufinhost,*bufindevice;
	if(sample >= (stsz->start_entries+STSZ_MAX_ENTRIES))
	{
		if(stsz->is_video)
		{ 
			current=file->current_stsz_write;
			mp4_filewrite_core(file->table_video, (char*)file->sram_stsz_table_video[current], STSZ_MAX_ENTRIES*4, MP4_IDX_VID_STATUS,27);
			current++;
			if(current>=2)
				current = 0;
			file->current_stsz_write=current;
		}
		if(stsz->is_audio)
		{
			bufinhost=(char*)stsz->table;
			bufindevice=(char*)file->sram_stsz_table_audio;
			mp4_memcpy_h2d(bufindevice,bufinhost,STSZ_MAX_ENTRIES*4);
			mp4_filewrite_core(file->table_audio, bufindevice, STSZ_MAX_ENTRIES*4, MP4_IDX_AUD_STATUS,26);
		}
		stsz->start_entries+=STSZ_MAX_ENTRIES;
	}
	if(stsz->is_audio)
	{
		stsz->table[sample-stsz->start_entries].size = mp4_swap_uint32(sample_size);
	}
	stsz->total_entries += 1;
	stsz->end_smpsize = sample_size;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_update_stsz(mp4_infor_t *file,mp4_stsz_t *stsz, uint32  sample, uint32  sample_size)
{
	char * bufinhost,*bufindevice;
	if(sample >= (stsz->start_entries+STSZ_MAX_ENTRIES))
	{
		if(stsz->is_video)
		{ 
			bufinhost=(char*)stsz->table;
			bufindevice=(char*)file->sram_stsz_table_video;
			mp4_memcpy_h2d(bufindevice,bufinhost,STSZ_MAX_ENTRIES*4);
			mp4_filewrite_core(file->table_video, bufindevice, STSZ_MAX_ENTRIES*4, MP4_IDX_VID_STATUS,27);
		}
		//mp4_filewrite(file->table_video, (char*)stsz->table, STSZ_MAX_ENTRIES*4);
		if(stsz->is_audio)
		{
			bufinhost=(char*)stsz->table;
			bufindevice=(char*)file->sram_stsz_table;
			mp4_memcpy_h2d(bufindevice,bufinhost,STSZ_MAX_ENTRIES*4);
			mp4_filewrite_core(file->table_audio, bufindevice, STSZ_MAX_ENTRIES*4, MP4_IDX_VID_STATUS,26);
		}
		//mp4_filewrite(file->table_audio, (char*)stsz->table, STSZ_MAX_ENTRIES*4);
		stsz->start_entries+=STSZ_MAX_ENTRIES;
	}
	stsz->table[sample-stsz->start_entries].size = mp4_swap_uint32(sample_size);
	stsz->total_entries += 1;
	stsz->end_smpsize = sample_size;
	return MP4_MUX_DEMUX_RET_NOERR;
}

#endif
#else
uint32 mp4_write_stsz(mp4_infor_t *file, mp4_stsz_t *stsz)
{
	int i, result;
	mp4_atom_t atom;

	result = mp4_atom_write_header(file, &atom, "stsz");
	if(result) return result;
	
	result = mp4_write_char(file, stsz->version);
	if(result) return result;
	result = mp4_write_uint24(file, stsz->flags);
	if(result) return result;
	result = mp4_write_uint32(file, stsz->sample_size);
	if(result) return result;
	result = mp4_write_uint32(file, stsz->total_entries);
	if(result) return result;
	if(!stsz->sample_size)
	{
		for(i = 0; i < stsz->total_entries; i++)
		{
			result = mp4_write_uint32(file, stsz->table[i].size);
			if(result) return result;
		}
	}
	return mp4_atom_write_footer(file, &atom);
}

uint32 mp4_update_stsz(mp4_infor_t *file,mp4_stsz_t *stsz, uint32  sample, uint32  sample_size)
{
	mp4_stsz_table_t *new_table;
	uint32 i;
	
	if(!stsz->sample_size)
	{
		if(sample >= stsz->entries_allocated)
		{
			stsz->entries_allocated = sample * 2;
			new_table = (mp4_stsz_table_t*)mp4_malloc(sizeof(mp4_stsz_table_t) * stsz->entries_allocated);
			for(i = 0; i < stsz->total_entries; i++) new_table[i] = stsz->table[i];
			mp4_free(stsz->table);
			stsz->table = new_table;
		}
		stsz->table[sample].size = sample_size;
	}
	stsz->total_entries += 1;
	stsz->end_smpsize = sample_size;
	return MP4_MUX_DEMUX_RET_NOERR;
}

#endif

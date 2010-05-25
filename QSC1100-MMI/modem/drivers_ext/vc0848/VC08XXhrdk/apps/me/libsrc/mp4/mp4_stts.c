/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	stss.c  
* User for : 	mp4 demux mux, stts atom is leaf atom of stbl atom, it is mandatory
*			user foe decoding, include sample duration inforamtion 
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"

void mp4_stts_init(mp4_stts_t *stts)
{
	/*these member is stts table mandatory*/
	stts->version = 0;
	stts->flags = 0;
	stts->total_entries = 0;
	/* used by the library for allocating a table */
	stts->table = NULL;
	stts->entries_allocated = 0;		
	/*these member define for demux because memory limited*/
	stts->start_position= 0;	//stts table start position in file
	stts->end_position = 0;	//stts table end position in file
	stts->start_entries = 0;
	stts->start_duration = 0;
	stts->end_duration = 0;
	stts->start_samples = 0;
	stts->end_samples = 0;
	/*these member define for if duration table is error*/
	stts->default_duration = 0;
	stts->default_samples = 0;
	stts->is_audio=0;
	stts->is_video=0;
}

void mp4_stts_init_common_video(mp4_infor_t *file, mp4_stts_t *stts, uint32  sample_duration)
{
#if VIDEORECORDERIN848
#else
	mp4_stts_table_t *table;
#endif

	if(stts->table==NULL)
	{
		stts->entries_allocated = STTS_MAX_ENTRIES;
		stts->table = (mp4_stts_table_t*)mp4_malloc(sizeof(mp4_stts_table_t) * stts->entries_allocated);
#if VIDEORECORDERIN848
		file->sram_stts_table[0]=(mp4_stts_table_t*)mp4_malloc_h2d(sizeof(mp4_stts_table_t) * stts->entries_allocated);
		file->sram_stts_table[1]=(mp4_stts_table_t*)mp4_malloc_h2d(sizeof(mp4_stts_table_t) * stts->entries_allocated);
		file->current_stts_write=0;
#else
		file->sram_stts_table=(mp4_stts_table_t*)mp4_malloc_h2d(sizeof(mp4_stts_table_t) * stts->entries_allocated);
#endif
	}
#if VIDEORECORDERIN848
#else
	table = &(stts->table[0]);
	table->sample_count = 0;      /* need to set this when closing */
	table->sample_duration = sample_duration;
	stts->table[0].sample_duration = sample_duration;
#endif
	stts->total_entries = 1;
}

void mp4_stts_init_common_audio(mp4_infor_t *file, mp4_stts_t *stts, uint32  sample_duration)
{
	mp4_stts_table_t *table;

	if(stts->table==NULL)
	{
		stts->entries_allocated = STTS_MAX_ENTRIES;
		stts->table = (mp4_stts_table_t*)mp4_malloc(sizeof(mp4_stts_table_t) * stts->entries_allocated);
	}
	table = &(stts->table[0]);
	table->sample_count = 0;      /* need to set this when closing */
	table->sample_duration = sample_duration;
	stts->table[0].sample_duration = sample_duration;
	stts->total_entries = 1;
}

#if VIDEORECORDERIN848
void mp4_stts_delete(mp4_infor_t *file, mp4_stts_t *stts)
{
	uint32 i;
	if(stts->table!=NULL)
		mp4_free(stts->table);
	for(i=0;i<2;i++)
	{
		if(file->sram_stts_table[i]!=NULL)
			mp4_free_h2d((void *)file->sram_stts_table[i]);
		file->sram_stts_table[i]=NULL;
	}
	file->current_stts_write=0;
	stts->entries_allocated = 0;
	stts->total_entries = 0;
}
#else
void mp4_stts_delete(mp4_infor_t *file, mp4_stts_t *stts)
{
	if(stts->table!=NULL)
		mp4_free(stts->table);
	if(file->sram_stts_table!=NULL)
		mp4_free_h2d((void *)file->sram_stts_table);
	stts->entries_allocated = 0;
	stts->total_entries = 0;
}
#endif
void mp4_stts_dump(mp4_stts_t *stts)
{
	MP4_DEMUX_ERR(("      time to sample table\n"));
	MP4_DEMUX_ERR(("      version %d\n", stts->version));
	MP4_DEMUX_ERR(("      flags %d\n", stts->flags));
	MP4_DEMUX_ERR(("      total_entries %d\n", stts->total_entries));
}

uint32 mp4_read_stts(mp4_infor_t *file, mp4_stts_t *stts)
{
	uint32 result;
	
	result = mp4_read_char(file, &stts->version);
	if(result) return result;
	result = mp4_read_uint24(file, &stts->flags);
	if(result) return result;
	result = mp4_read_uint32(file, &stts->total_entries);
	if(result) return result;
	
	if(stts->total_entries)
	{
#if 1	//MP4_IN_820_MEMORY_LIMITED (because will get duration in timer interrupt)
		//only malloc this memory once time each file
		if ((stts->total_entries)<=STTS_MAX_ENTRIES)
			stts->entries_allocated = stts->total_entries;
		else
       		stts->entries_allocated = STTS_MAX_ENTRIES;
		stts->table = (mp4_stts_table_t*)mp4_malloc(sizeof(mp4_stts_table_t) * stts->entries_allocated);
#else
		stts->entries_allocated = stts->total_entries;
		stts->table = (mp4_stts_table_t*)mp4_malloc(sizeof(mp4_stts_table_t) * stts->entries_allocated);
#endif
		//read stts table first time
		stts->start_position = stts->start_position+8+8;
		return mp4_read_stts_table(file, stts,1);
	}
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_read_stts_table(mp4_infor_t *file, mp4_stts_t *stts, uint32 restart)
{
	uint32 i, result;
	uint32 sample_count, sample_duration;
	
	if(!stts->total_entries)
		return MP4_MUX_DEMUX_RET_NOERR;
	
	if(restart)
	{
		stts->start_entries = 0;
		stts->start_samples = stts->end_samples = 0;
		stts->start_duration = stts->end_duration = 0;
	}
	else
	{
		stts->start_entries += stts->entries_allocated;
		stts->start_samples = stts->end_samples ;
		stts->start_duration = stts->end_duration ;
	}
	
	if ((stts->total_entries-stts->start_entries)<=STTS_MAX_ENTRIES)
		stts->entries_allocated = stts->total_entries-stts->start_entries;
	else
        	stts->entries_allocated = STTS_MAX_ENTRIES;
	

	result = mp4_set_file_position(file,stts->start_position+stts->start_entries*8);
	if(result) return result;
	
	result = mp4_read_data(file, (char *)stts->table, stts->entries_allocated*8);
	if(result != stts->entries_allocated*8)  return MP4_DEMUX_READ_FILE_ERROR;
	
	for(i = 0; i < stts->entries_allocated; i++)
	{
		sample_count = mp4_swap_uint32(stts->table[i].sample_count);
		sample_duration = mp4_swap_uint32(stts->table[i].sample_duration);
		stts->table[i].sample_count = (sample_count>0) ? sample_count : stts->default_samples ;
		stts->table[i].sample_duration = (sample_duration>0) ? sample_duration : stts->default_duration ;
		stts->end_samples += stts->table[i].sample_count ;
		stts->end_duration += stts->table[i].sample_duration * stts->table[i].sample_count ;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_video_stts_table(mp4_infor_t *file, mp4_stts_t *stts, uint32 restart)
{
	uint32 i, result;
	uint32 sample_count, sample_duration;
	
	if(!stts->total_entries)
		return MP4_MUX_DEMUX_RET_NOERR;
	
	if(restart)
	{
		stts->start_entries = 0;
		stts->start_samples = stts->end_samples = 0;
		stts->start_duration = stts->end_duration = 0;
	}
	else
	{
		stts->start_entries += stts->entries_allocated;
		stts->start_samples = stts->end_samples ;
		stts->start_duration = stts->end_duration ;
	}
	
	if ((stts->total_entries-stts->start_entries)<=STTS_MAX_ENTRIES)
		stts->entries_allocated = stts->total_entries-stts->start_entries;
	else
        	stts->entries_allocated = STTS_MAX_ENTRIES;
	

//	result = mp4_set_file_position(file,stts->start_position+stts->start_entries*8);
//	if(result) return result;
	file->seek_func(file->stream_videoindx,stts->start_position+stts->start_entries*8,SEEK_SET);

//	result = mp4_read_data(file, (char *)stts->table, stts->entries_allocated*8);
	result = mp4_read_videoindx(file, (char *)stts->table, stts->entries_allocated*8);
	if(result != stts->entries_allocated*8)  return MP4_DEMUX_READ_FILE_ERROR;
	
	for(i = 0; i < stts->entries_allocated; i++)
	{
		sample_count = mp4_swap_uint32(stts->table[i].sample_count);
		sample_duration = mp4_swap_uint32(stts->table[i].sample_duration);
		stts->table[i].sample_count = (sample_count>0) ? sample_count : stts->default_samples ;
		stts->table[i].sample_duration = (sample_duration>0) ? sample_duration : stts->default_duration ;
		stts->end_samples += stts->table[i].sample_count ;
		stts->end_duration += stts->table[i].sample_duration * stts->table[i].sample_count ;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_read_audio_stts_table(mp4_infor_t *file, mp4_stts_t *stts, uint32 restart)
{
	uint32 i, result;
	uint32 sample_count, sample_duration;
	
	if(!stts->total_entries)
		return MP4_MUX_DEMUX_RET_NOERR;
	
	if(restart)
	{
		stts->start_entries = 0;
		stts->start_samples = stts->end_samples = 0;
		stts->start_duration = stts->end_duration = 0;
	}
	else
	{
		stts->start_entries += stts->entries_allocated;
		stts->start_samples = stts->end_samples ;
		stts->start_duration = stts->end_duration ;
	}
	
	if ((stts->total_entries-stts->start_entries)<=STTS_MAX_ENTRIES)
		stts->entries_allocated = stts->total_entries-stts->start_entries;
	else
        	stts->entries_allocated = STTS_MAX_ENTRIES;
	

//	result = mp4_set_file_position(file,stts->start_position+stts->start_entries*8);
//	if(result) return result;
	file->seek_func(file->stream_audioindx,stts->start_position+stts->start_entries*8,SEEK_SET);

//	result = mp4_read_data(file, (char *)stts->table, stts->entries_allocated*8);
	result = mp4_read_audioindx(file, (char *)stts->table, stts->entries_allocated*8);
	if(result != stts->entries_allocated*8)  return MP4_DEMUX_READ_FILE_ERROR;
	
	for(i = 0; i < stts->entries_allocated; i++)
	{
		sample_count = mp4_swap_uint32(stts->table[i].sample_count);
		sample_duration = mp4_swap_uint32(stts->table[i].sample_duration);
		stts->table[i].sample_count = (sample_count>0) ? sample_count : stts->default_samples ;
		stts->table[i].sample_duration = (sample_duration>0) ? sample_duration : stts->default_duration ;
		stts->end_samples += stts->table[i].sample_count ;
		stts->end_duration += stts->table[i].sample_duration * stts->table[i].sample_count ;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

#if MP4_IN_820_ENCODE_HARDWARE
uint32 mp4_write_stts(mp4_infor_t *file, mp4_stts_t *stts)
{
	uint32 i, result;
	mp4_atom_t atom;
	
#if VIDEORECORDERIN848
	uint32 current;
	char *bufinhost,*bufindevice;
#endif
	
	result=mp4_atom_write_header(file, &atom, "stts");
	if(result) return result;
	result = mp4_write_char(file, stts->version);
	if(result) return result;
	result = mp4_write_uint24(file, stts->flags);
	if(result) return result;
	result = mp4_write_uint32(file, stts->total_entries);

	if(stts->is_video)
	{
		/*MMD_BIU_Get_Device_Status(6, &totalentries);
           	totalentries=((totalentries&0xffff0000)>>16);
		stts->total_entries=totalentries;
		if(stts->total_entries >= (stts->start_entries+STTS_MAX_ENTRIES))
              {
                     current=file->current_stts_write;
			bufinhost=(char*)stts->table;
    			bufindevice=(char*)file->sram_stts_table[current];	 
			mp4_memcpy_d2h(bufinhost,bufindevice,STTS_MAX_ENTRIES*8);	
			mp4_filewrite(file->table_duration, (char*)stts->table, STTS_MAX_ENTRIES*8);
                     //mp4_filewrite_core(file->table_duration,(char*)file->sram_stts_table[current], STTS_MAX_ENTRIES*8, MP4_IDX_VID_STATUS,27);
                     current++;
                     if(current>=2)
                     	current = 0;
                     file->current_stts_write=current;
                     stts->start_entries+=STTS_MAX_ENTRIES;
              }*/
    	current=file->current_stts_write;
   	 	bufinhost=(char*)stts->table;
    	bufindevice=(char*)file->sram_stts_table[current];
    	mp4_memcpy_d2h(bufinhost,bufindevice,(stts->total_entries-stts->start_entries)*8);
	}
	//write stts table fto muxer file
	if(stts->total_entries>STTS_MAX_ENTRIES)
	{//always is video duration table, need use tmp table file store these parameter
		mp4_filewrite(file->table_duration, (char*)stts->table, (stts->total_entries-stts->start_entries)*8);
		mp4_fileseek(file->table_duration, 0, SEEK_SET);
		while(stts->total_entries>=STTS_MAX_ENTRIES)
       	{
			mp4_fileread(file->table_duration, (char *)stts->table, STTS_MAX_ENTRIES*8);
			for(i = 0; i < STTS_MAX_ENTRIES; i++)
			{
				stts->table[i].sample_count = mp4_swap_uint32(stts->table[i].sample_count); 
				stts->table[i].sample_duration = mp4_swap_uint32(stts->table[i].sample_duration);
			}
			result = mp4_write_data(file, (char *)stts->table, STTS_MAX_ENTRIES*8);
			if(result !=STTS_MAX_ENTRIES*8) return MP4_MUX_WRITE_FILE_ERROR;
			stts->total_entries -= STTS_MAX_ENTRIES;
		}
		if(stts->total_entries)
		{
			mp4_fileread(file->table_duration, (char *)stts->table, stts->total_entries*8);
			for(i = 0; i < stts->total_entries; i++)
			{
				stts->table[i].sample_count = mp4_swap_uint32(stts->table[i].sample_count); 
				stts->table[i].sample_duration = mp4_swap_uint32(stts->table[i].sample_duration);
			}
			result = mp4_write_data(file, (char *)stts->table, stts->total_entries*8);
			if(result !=stts->total_entries*8) return MP4_MUX_WRITE_FILE_ERROR;
			stts->total_entries -= stts->total_entries;
		}
	}
	else
	{//always is audio duration table, is not used tmp table file
		for(i = 0; i < stts->total_entries; i++)
		{
			stts->table[i].sample_count = mp4_swap_uint32(stts->table[i].sample_count); 
			stts->table[i].sample_duration = mp4_swap_uint32(stts->table[i].sample_duration);
		}
		result = mp4_write_data(file, (char *)stts->table, stts->total_entries*8);
		if(result !=stts->total_entries*8) return MP4_MUX_WRITE_FILE_ERROR;
		stts->total_entries -= stts->total_entries;
	}
	return mp4_atom_write_footer(file, &atom);
}
#if VIDEORECORDERIN848
uint32 mp4_update_stts(mp4_infor_t *file,mp4_stts_t *stts, uint32  sample_duration)
{
	uint32 current,totalentries=0;
	if(stts->is_video)
	{
           	MMD_BIU_Get_Device_Status(7, &totalentries);
           	//totalentries=((totalentries&0xffff0000)>>16);
		
    	if(totalentries>stts->total_entries)
    	{
            stts->total_entries=totalentries;
            if(stts->total_entries >= (stts->start_entries+STTS_MAX_ENTRIES))
            {
            	current=file->current_stts_write;
                     	mp4_filewrite_core(file->table_duration,(char*)file->sram_stts_table[current], STTS_MAX_ENTRIES*8, MP4_IDX_VID_STATUS,27);
            	current++;
            	if(current>=2)
            		current = 0;
            	file->current_stts_write=current;
                     	stts->start_entries+=STTS_MAX_ENTRIES;
            }
    	}
	}
	if(stts->is_audio)
	{
		if((stts->total_entries==1)&&(!stts->table[0].sample_count))
			stts->table[0].sample_duration = sample_duration;
		stts->table[stts->total_entries-1-stts->start_entries].sample_count++;
		stts->end_duration += sample_duration;
	}
	stts->end_samples +=1;
	//stts->end_duration += sample_duration;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_update_stts(mp4_infor_t *file,mp4_stts_t *stts, uint32  sample_duration)
{
	char * bufinhost,*bufindevice;
	if((stts->total_entries==1)&&(!stts->table[0].sample_count))
		stts->table[0].sample_duration = sample_duration;
	
	if (sample_duration == stts->table[stts->total_entries-1-stts->start_entries].sample_duration) 
	{
		stts->table[stts->total_entries-1-stts->start_entries].sample_count++;
	} 
	else 
	{
		if(stts->total_entries >= (stts->start_entries+STTS_MAX_ENTRIES))
		{
			bufinhost=(char*)stts->table;
			bufindevice=(char*)file->sram_stts_table;
			mp4_memcpy_h2d(bufindevice,bufinhost,STTS_MAX_ENTRIES*4);
			mp4_filewrite_core(file->table_duration, bufindevice, STTS_MAX_ENTRIES*4, MP4_IDX_VID_STATUS,27);
			//mp4_filewrite(file->table_duration, (char*)stts->table, STTS_MAX_ENTRIES*8);
			stts->start_entries+=STSZ_MAX_ENTRIES;
		}
		stts->table[stts->total_entries-stts->start_entries].sample_count = 1;
		stts->table[stts->total_entries-stts->start_entries].sample_duration = sample_duration;
		stts->total_entries++;
	}
	stts->end_samples +=1;
	stts->end_duration += sample_duration;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

#else
uint32 mp4_write_stts(mp4_infor_t *file, mp4_stts_t *stts)
{
	uint32 i, result;
	mp4_atom_t atom;
	
	result=mp4_atom_write_header(file, &atom, "stts");
	if(result) return result;
	result = mp4_write_char(file, stts->version);
	if(result) return result;
	result = mp4_write_uint24(file, stts->flags);
	if(result) return result;
	result = mp4_write_uint32(file, stts->total_entries);
	for(i = 0; i < stts->total_entries; i++)
	{
		result = mp4_write_uint32(file, stts->table[i].sample_count);
		if(result) return result;
		result = mp4_write_uint32(file, stts->table[i].sample_duration);
		if(result) return result;
	}
	return mp4_atom_write_footer(file, &atom);
}

uint32 mp4_update_stts(mp4_infor_t *file,mp4_stts_t *stts, uint32  sample_duration)
{
	mp4_stts_table_t *new_table;
	uint32 i;
	
	if((stts->total_entries==1)&&(!stts->table[0].sample_count))
		stts->table[0].sample_duration = sample_duration;
	
	if (sample_duration == stts->table[stts->total_entries-1].sample_duration) 
	{
		stts->table[stts->total_entries-1].sample_count++;
	} 
	else 
	{
		if (stts->total_entries >= stts->entries_allocated) 
		{
			stts->entries_allocated *= 2;
			new_table = (mp4_stts_table_t*)mp4_malloc(sizeof(mp4_stts_table_t) * stts->entries_allocated);
			for(i = 0; i < stts->total_entries; i++) new_table[i] = stts->table[i];
				mp4_free(stts->table);
			stts->table = new_table;
		}
		stts->table[stts->total_entries].sample_count = 1;
		stts->table[stts->total_entries].sample_duration = sample_duration;
		stts->total_entries++;
	}
	stts->end_samples +=1;
	stts->end_duration += sample_duration;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

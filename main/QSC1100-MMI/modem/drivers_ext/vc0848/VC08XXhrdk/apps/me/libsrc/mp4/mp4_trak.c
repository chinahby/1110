/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	trak.c  
* User for : 	mp4 demux mux, trak atom is leaf atom of moov atom, it is mandatory 
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"


void mp4_trak_init(mp4_trak_t *trak)
{
	mp4_tkhd_init(&(trak->tkhd));
	mp4_mdia_init(&(trak->mdia));
}

void mp4_trak_init_video(mp4_infor_t *file, 
							mp4_trak_t *trak, 
							uint32 frame_w, 
							uint32 frame_h, 
							uint32  sample_duration,
							uint32  time_scale,
							char *compressor)
{
	mp4_tkhd_init_video(file, &(trak->tkhd), frame_w, frame_h);
	mp4_mdia_init_video(file, &(trak->mdia), frame_w, frame_h, 
		sample_duration, time_scale, compressor);
}

void mp4_trak_init_audio(mp4_infor_t *file, 
							mp4_trak_t *trak, 
							uint32 channels, 
							uint32 bits, 
							uint32  sample_rate, 
							uint32  sample_size,
							uint32  time_scale,
							uint32  sample_duration,
							char *compressor)
{
	mp4_tkhd_init_audio(file, &(trak->tkhd));
	mp4_mdia_init_audio(file, &(trak->mdia), channels, bits, sample_rate,
		sample_size, time_scale, sample_duration, compressor);
}



void mp4_trak_delete(mp4_infor_t *file,mp4_trak_t *trak)
{
	mp4_tkhd_delete(&(trak->tkhd));
	mp4_mdia_delete(file,&(trak->mdia));
}


void mp4_trak_dump(mp4_trak_t *trak)
{
	MP4_DEMUX_MSG((" track\n"));
	mp4_tkhd_dump(&(trak->tkhd));
	mp4_mdia_dump(&(trak->mdia));
}


mp4_trak_t* mp4_add_trak(mp4_moov_t *moov)
{
	if(moov->total_tracks < MAXTRACKS)
	{
		moov->trak[moov->total_tracks] = (mp4_trak_t*)mp4_malloc(sizeof(mp4_trak_t));
		mp4_trak_init(moov->trak[moov->total_tracks]);
		moov->total_tracks++;
	}
	return moov->trak[moov->total_tracks - 1];
}

uint32 mp4_delete_trak(mp4_infor_t *file,mp4_moov_t *moov, mp4_trak_t *trak)
{
	uint32 i, j;

	for (i = 0; i < moov->total_tracks; i++) 
	{
		if (moov->trak[i] == trak) {
			mp4_trak_delete(file,trak);
			mp4_free(trak);
			moov->trak[i] = NULL;
			for (j = i + 1; j < moov->total_tracks; j++, i++) 
			{
				moov->trak[i] = moov->trak[j];
			}
			moov->trak[j] = NULL;
			moov->total_tracks--;
			return MP4_MUX_DEMUX_RET_NOERR;
		}
	}
	return -1;
}


uint32 mp4_read_trak(mp4_infor_t *file, mp4_trak_t *trak, mp4_atom_t *trak_atom)
{
	mp4_atom_t leaf_atom;
	uint32 result;

	do
	{
		result = mp4_atom_read_header(file, &leaf_atom);
		if(result) return result;
		/* mandatory */
		if(mp4_atom_is(&leaf_atom, "tkhd"))
		{ 
			//this atom is must
			result = mp4_read_tkhd(file, &(trak->tkhd));
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "mdia"))
		{ 
			//this atom is must
			result = mp4_read_mdia(file, &(trak->mdia), &leaf_atom); 
			if(result) return result;
		}
		else
		{
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
	}while(mp4_get_file_position(file) < trak_atom->end);

	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_write_trak(mp4_infor_t *file, mp4_trak_t *trak, uint32  moov_time_scale)
{
	uint32  duration, timescale, result;
	mp4_atom_t atom;
	
	result = mp4_atom_write_header(file, &atom, "trak");
	if(result) return result;
	
	mp4_trak_duration(trak, &duration, &timescale);

	/* get duration in movie's units */
	if (timescale) 
	{
		trak->tkhd.duration = (uint32 )((float)duration / timescale * moov_time_scale);
	} 
	else 
	{
		trak->tkhd.duration = 0;
	}
	trak->mdia.mdhd.duration = duration;
	trak->mdia.mdhd.time_scale = timescale;

	result =mp4_write_tkhd(file, &(trak->tkhd));
	if(result) return result;
	result =mp4_write_mdia(file, &(trak->mdia));
	if(result) return result;
	
	return mp4_atom_write_footer(file, &atom);
}

uint32  mp4_track_samples(mp4_infor_t *file, mp4_trak_t *trak)
{
	/* get the sample count when reading only */
	mp4_stts_t *stts = &(trak->mdia.minf.stbl.stts);
	
	if(file->rw_mode)
	{
		//when write, stts->total_entries now have not update,so is not same as read.
 		return stts->end_samples;
	}
	else
	{

		if(stts->entries_allocated>=stts->total_entries)
			return stts->end_samples;
		else
		{
		 	while((stts->entries_allocated+stts->start_entries)<stts->total_entries)
				mp4_read_stts_table(file, stts,0);
		 	return stts->end_samples;
		}
	}
}

uint32  mp4_track_times(mp4_infor_t *file, mp4_trak_t *trak)
{
	mp4_stts_t *stts = &(trak->mdia.minf.stbl.stts);
	
	if(file->rw_mode)
	{
		//when write, stts->total_entries now have not update,so is not same as read.
 		return stts->end_duration;
	}
	else
	{
		/* get the total times when reading only */
		if(stts->entries_allocated>=stts->total_entries)
			return stts->end_duration;
		else
		{
		 	while((stts->entries_allocated+stts->start_entries)<stts->total_entries)
				mp4_read_stts_table(file, stts, 0);
		 	return stts->end_duration;
		}
	}
}
#if MP4LIB_VERSION2_FOREHANDLE 
uint32  mp4_video_track_samples(mp4_infor_t *file, mp4_trak_t *trak)
{
	/* get the sample count when reading only */
	mp4_stts_t *stts = &(trak->mdia.minf.stbl.stts);
	
	if(file->rw_mode)
	{
		//when write, stts->total_entries now have not update,so is not same as read.
 		return stts->end_samples;
	}
	else
	{

		if(stts->entries_allocated>=stts->total_entries)
			return stts->end_samples;
		else
		{
		 	while((stts->entries_allocated+stts->start_entries)<stts->total_entries)
//				mp4_read_stts_table(file, stts,0);
				mp4_read_video_stts_table(file, stts,0);				
		 	return stts->end_samples;
		}
	}
}

uint32  mp4_video_track_times(mp4_infor_t *file, mp4_trak_t *trak)
{
	mp4_stts_t *stts = &(trak->mdia.minf.stbl.stts);
	
	if(file->rw_mode)
	{
		//when write, stts->total_entries now have not update,so is not same as read.
 		return stts->end_duration;
	}
	else
	{
		/* get the total times when reading only */
		if(stts->entries_allocated>=stts->total_entries)
			return stts->end_duration;
		else
		{
		 	while((stts->entries_allocated+stts->start_entries)<stts->total_entries)
//				mp4_read_stts_table(file, stts, 0);
				mp4_read_video_stts_table(file, stts, 0);
		 	return stts->end_duration;
		}
	}
}

uint32  mp4_audio_track_samples(mp4_infor_t *file, mp4_trak_t *trak)
{
	/* get the sample count when reading only */
	mp4_stts_t *stts = &(trak->mdia.minf.stbl.stts);
	
	if(file->rw_mode)
	{
		//when write, stts->total_entries now have not update,so is not same as read.
 		return stts->end_samples;
	}
	else
	{

		if(stts->entries_allocated>=stts->total_entries)
			return stts->end_samples;
		else
		{
		 	while((stts->entries_allocated+stts->start_entries)<stts->total_entries)
//				mp4_read_stts_table(file, stts,0);
				mp4_read_audio_stts_table(file, stts, 0);
		 	return stts->end_samples;
		}
	}
}

uint32  mp4_audio_track_times(mp4_infor_t *file, mp4_trak_t *trak)
{
	mp4_stts_t *stts = &(trak->mdia.minf.stbl.stts);
	
	if(file->rw_mode)
	{
		//when write, stts->total_entries now have not update,so is not same as read.
 		return stts->end_duration;
	}
	else
	{
		/* get the total times when reading only */
		if(stts->entries_allocated>=stts->total_entries)
			return stts->end_duration;
		else
		{
		 	while((stts->entries_allocated+stts->start_entries)<stts->total_entries)
//				mp4_read_stts_table(file, stts, 0);
				mp4_read_audio_stts_table(file, stts, 0);
		 	return stts->end_duration;
		}
	}
}
#endif

uint32 mp4_chunk_of_sample(mp4_infor_t *file,uint32  *chunk_sample, uint32  *chunk, mp4_trak_t *trak, uint32  sample)
{
	mp4_stsc_t *stsc = NULL;
	uint32 i,use_lastchunk=1;
	uint32  last_chunk, current_chunk, 
		last_chunk_samples, range_samples, total ;

	if (trak == NULL) 
	{
		return -1;
	}
	
 	stsc = &trak->mdia.minf.stbl.stsc;

 	if (sample<stsc->start_samples)
      		mp4_read_stsc_table(file, stsc,1);
 	while((sample>=stsc->end_samples)&&(stsc->start_entries+stsc->entries_allocated<stsc->total_entries))
          	mp4_read_stsc_table(file, stsc, 0);

	last_chunk = stsc->last_chunk;
	last_chunk_samples = stsc->last_chunk_sample;
	total = stsc->start_samples;
	i =  0;

	do
	{
		current_chunk = stsc->table[i].chunk;
		*chunk = current_chunk - last_chunk;
		range_samples = *chunk * last_chunk_samples;

		if(sample < total + range_samples) break;
		use_lastchunk =0;
		last_chunk_samples =stsc->table[i].samples;
		last_chunk = current_chunk;

		if(i < stsc->entries_allocated)
		{
			i++;
			total += range_samples;
		}
	}while(i < stsc->entries_allocated);

	if((!use_lastchunk)&&(last_chunk_samples))
		*chunk = (sample - total) / last_chunk_samples + last_chunk;
	else
		*chunk = last_chunk;

	*chunk_sample = total + (*chunk - last_chunk) * last_chunk_samples;
	return 0;
}

#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_video_chunk_of_sample(mp4_infor_t *file,uint32  *chunk_sample, uint32  *chunk, mp4_trak_t *trak, uint32  sample)
{
	mp4_stsc_t *stsc = NULL;
	uint32 i,use_lastchunk=1;
	uint32  last_chunk, current_chunk, 
		last_chunk_samples, range_samples, total ;

	if (trak == NULL) 
	{
		return -1;
	}
	
 	stsc = &trak->mdia.minf.stbl.stsc;

 	if (sample<stsc->start_samples)
//      	mp4_read_stsc_table(file, stsc,1);
		mp4_read_video_stsc_table(file, stsc,1);
 	while((sample>=stsc->end_samples)&&(stsc->start_entries+stsc->entries_allocated<stsc->total_entries))
 //         	mp4_read_stsc_table(file, stsc, 0);
          	mp4_read_video_stsc_table(file, stsc, 0);

	last_chunk = stsc->last_chunk;
	last_chunk_samples = stsc->last_chunk_sample;
	total = stsc->start_samples;
	i =  0;

	do
	{
		current_chunk = stsc->table[i].chunk;
		*chunk = current_chunk - last_chunk;
		range_samples = *chunk * last_chunk_samples;

		if(sample < total + range_samples) break;
		use_lastchunk =0;
		last_chunk_samples =stsc->table[i].samples;
		last_chunk = current_chunk;

		if(i < stsc->entries_allocated)
		{
			i++;
			total += range_samples;
		}
	}while(i < stsc->entries_allocated);

	if((!use_lastchunk)&&(last_chunk_samples))
		*chunk = (sample - total) / last_chunk_samples + last_chunk;
	else
		*chunk = last_chunk;

	*chunk_sample = total + (*chunk - last_chunk) * last_chunk_samples;
	return 0;
}

uint32 mp4_audio_chunk_of_sample(mp4_infor_t *file,uint32  *chunk_sample, uint32  *chunk, mp4_trak_t *trak, uint32  sample)
{
	mp4_stsc_t *stsc = NULL;
	uint32 i,use_lastchunk=1;
	uint32  last_chunk, current_chunk, 
		last_chunk_samples, range_samples, total ;

	if (trak == NULL) 
	{
		return -1;
	}
	
 	stsc = &trak->mdia.minf.stbl.stsc;

 	if (sample<stsc->start_samples)
 //     	mp4_read_stsc_table(file, stsc,1);
 		mp4_read_audio_stsc_table(file, stsc,1);
 	while((sample>=stsc->end_samples)&&(stsc->start_entries+stsc->entries_allocated<stsc->total_entries))
  //        	mp4_read_stsc_table(file, stsc, 0);
  		mp4_read_audio_stsc_table(file, stsc, 0);

	last_chunk = stsc->last_chunk;
	last_chunk_samples = stsc->last_chunk_sample;
	total = stsc->start_samples;
	i =  0;

	do
	{
		current_chunk = stsc->table[i].chunk;
		*chunk = current_chunk - last_chunk;
		range_samples = *chunk * last_chunk_samples;

		if(sample < total + range_samples) break;
		use_lastchunk =0;
		last_chunk_samples =stsc->table[i].samples;
		last_chunk = current_chunk;

		if(i < stsc->entries_allocated)
		{
			i++;
			total += range_samples;
		}
	}while(i < stsc->entries_allocated);

	if((!use_lastchunk)&&(last_chunk_samples))
		*chunk = (sample - total) / last_chunk_samples + last_chunk;
	else
		*chunk = last_chunk;

	*chunk_sample = total + (*chunk - last_chunk) * last_chunk_samples;
	return 0;
}
#endif

uint32  mp4_chunk_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk)
{
	mp4_stco_t *stco = NULL;

	if (trak == NULL) 
	{
		return -1;
	}
	stco = &trak->mdia.minf.stbl.stco;

	if (chunk<stco->start_chunk)
      		mp4_read_stco_table(file, stco, 1);
      	while((chunk>stco->end_chunk)&&((stco->start_entries+stco->entries_allocated)<stco->total_entries))
       	mp4_read_stco_table(file, stco, 0);
 	
      	if(stco->total_entries && chunk > stco->total_entries)
		return stco->table[stco->entries_allocated- 1].offset;
	else
	{
		if(stco->total_entries)
			return stco->table[chunk - stco->start_chunk].offset;
		else
			return HEADER_LENGTH;
	}
}
#if MP4LIB_VERSION2_FOREHANDLE
uint32  mp4_video_chunk_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk)
{
	mp4_stco_t *stco = NULL;

	if (trak == NULL) 
	{
		return -1;
	}
	stco = &trak->mdia.minf.stbl.stco;

	if (chunk<stco->start_chunk)
//      	mp4_read_stco_table(file, stco, 1);
		mp4_read_video_stco_table(file, stco, 1);
      	while((chunk>stco->end_chunk)&&((stco->start_entries+stco->entries_allocated)<stco->total_entries))
//       	mp4_read_stco_table(file, stco, 0);
		mp4_read_video_stco_table(file, stco, 0);
 	
      	if(stco->total_entries && chunk > stco->total_entries)
		return stco->table[stco->entries_allocated- 1].offset;
	else
	{
		if(stco->total_entries)
			return stco->table[chunk - stco->start_chunk].offset;
		else
			return HEADER_LENGTH;
	}
}

uint32  mp4_audio_chunk_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk)
{
	mp4_stco_t *stco = NULL;

	if (trak == NULL) 
	{
		return -1;
	}
	stco = &trak->mdia.minf.stbl.stco;

	if (chunk<stco->start_chunk)
//     	mp4_read_stco_table(file, stco, 1);
		mp4_read_audio_stco_table(file, stco, 1);
      	while((chunk>stco->end_chunk)&&((stco->start_entries+stco->entries_allocated)<stco->total_entries))
//       	mp4_read_stco_table(file, stco, 0);
        	mp4_read_audio_stco_table(file, stco, 0);
		
      	if(stco->total_entries && chunk > stco->total_entries)
		return stco->table[stco->entries_allocated- 1].offset;
	else
	{
		if(stco->total_entries)
			return stco->table[chunk - stco->start_chunk].offset;
		else
			return HEADER_LENGTH;
	}
}
#endif

uint32  mp4_sample_range_size(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk_sample, uint32  sample)
{
	mp4_stsz_t *stsz = &trak->mdia.minf.stbl.stsz;
	uint32  i, total;

	if(stsz->sample_size)
	{
		/* assume audio */
		total =(sample - chunk_sample)* stsz->sample_size;
	}
	else
	{
		/* probably video */
		if (chunk_sample<stsz->start_samples)
      			mp4_read_stsz_table(file, stsz,1);
       	while(chunk_sample>=stsz->end_samples)
       		mp4_read_stsz_table(file, stsz,0);
      	
		if (sample<=stsz->end_samples)
		{
			for(i = (chunk_sample-stsz->start_samples), total = 0; i <(sample-stsz->start_samples); i++)
			total += stsz->table[i].size;
		}
		else
		{
			//chunck have samples more than samples in table, so need read again 
			for(i = (chunk_sample-stsz->start_samples), total = 0; i < stsz->entries_allocated; i++)
				total += stsz->table[i].size;
			//need read more times,except finally times
			while(sample>(stsz->end_samples+STSZ_MAX_ENTRIES))
			{
				mp4_read_stsz_table(file, stsz,0);
       			for(i = 0; i <stsz->entries_allocated; i++)
				total += stsz->table[i].size;
       			
			}
			//finally times sample in table
			mp4_read_stsz_table(file, stsz,0);
       		for(i = 0; i <(sample-stsz->start_samples); i++)
				total += stsz->table[i].size;
		}
	}
	return total;
}
#if MP4LIB_VERSION2_FOREHANDLE
uint32  mp4_video_sample_range_size(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk_sample, uint32  sample)
{
	mp4_stsz_t *stsz = &trak->mdia.minf.stbl.stsz;
	uint32  i, total;

	if(stsz->sample_size)
	{
		/* assume audio */
		total =(sample - chunk_sample)* stsz->sample_size;
	}
	else
	{
		/* probably video */
		if (chunk_sample<stsz->start_samples)
 //     		mp4_read_stsz_table(file, stsz,1);
 			mp4_read_video_stsz_table(file, stsz,1);
       	while(chunk_sample>=stsz->end_samples)
   //    		mp4_read_stsz_table(file, stsz,0);
      	    		mp4_read_video_stsz_table(file, stsz,0);
		
		if (sample<=stsz->end_samples)
		{
			for(i = (chunk_sample-stsz->start_samples), total = 0; i <(sample-stsz->start_samples); i++)
			total += stsz->table[i].size;
		}
		else
		{
			//chunck have samples more than samples in table, so need read again 
			for(i = (chunk_sample-stsz->start_samples), total = 0; i < stsz->entries_allocated; i++)
				total += stsz->table[i].size;
			//need read more times,except finally times
			while(sample>(stsz->end_samples+STSZ_MAX_ENTRIES))
			{
//				mp4_read_stsz_table(file, stsz,0);
				mp4_read_video_stsz_table(file, stsz,0);
       			for(i = 0; i <stsz->entries_allocated; i++)
				total += stsz->table[i].size;
       			
			}
			//finally times sample in table
//			mp4_read_stsz_table(file, stsz,0);
			mp4_read_video_stsz_table(file, stsz,0);
       		for(i = 0; i <(sample-stsz->start_samples); i++)
				total += stsz->table[i].size;
		}
	}
	return total;
}

uint32  mp4_audio_sample_range_size(mp4_infor_t *file, mp4_trak_t *trak, uint32  chunk_sample, uint32  sample)
{
	mp4_stsz_t *stsz = &trak->mdia.minf.stbl.stsz;
	uint32  i, total;

	if(stsz->sample_size)
	{
		/* assume audio */
		total =(sample - chunk_sample)* stsz->sample_size;
	}
	else
	{
		/* probably video */
		if (chunk_sample<stsz->start_samples)
 //     		mp4_read_stsz_table(file, stsz,1);
 			mp4_read_audio_stsz_table(file, stsz,1);
       	while(chunk_sample>=stsz->end_samples)
 //      		mp4_read_stsz_table(file, stsz,0);
 			mp4_read_audio_stsz_table(file, stsz,0);
      	
		if (sample<=stsz->end_samples)
		{
			for(i = (chunk_sample-stsz->start_samples), total = 0; i <(sample-stsz->start_samples); i++)
			total += stsz->table[i].size;
		}
		else
		{
			//chunck have samples more than samples in table, so need read again 
			for(i = (chunk_sample-stsz->start_samples), total = 0; i < stsz->entries_allocated; i++)
				total += stsz->table[i].size;
			//need read more times,except finally times
			while(sample>(stsz->end_samples+STSZ_MAX_ENTRIES))
			{
	//			mp4_read_stsz_table(file, stsz,0);
				mp4_read_audio_stsz_table(file, stsz,0);
       			for(i = 0; i <stsz->entries_allocated; i++)
				total += stsz->table[i].size;
       			
			}
			//finally times sample in table
	//		mp4_read_stsz_table(file, stsz,0);
			mp4_read_audio_stsz_table(file, stsz,0);
       		for(i = 0; i <(sample-stsz->start_samples); i++)
				total += stsz->table[i].size;
		}
	}
	return total;
}
#endif

uint32  mp4_sample_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  sample)
{
	uint32  chunk, chunk_sample, chunk_offset1, chunk_offset2;

	if (trak == NULL) 
	{
		return -1;
	}
	mp4_chunk_of_sample(file, &chunk_sample, &chunk, trak, sample);
	chunk_offset1 = mp4_chunk_to_offset(file, trak, chunk);
	chunk_offset2 = chunk_offset1 + mp4_sample_range_size(file, trak, chunk_sample, sample);
	return chunk_offset2;
}
#if MP4LIB_VERSION2_FOREHANDLE
uint32  mp4_video_sample_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  sample)
{
	uint32  chunk, chunk_sample, chunk_offset1, chunk_offset2;

	if (trak == NULL) 
	{
		return -1;
	}
	mp4_video_chunk_of_sample(file, &chunk_sample, &chunk, trak, sample);
	chunk_offset1 = mp4_video_chunk_to_offset(file, trak, chunk);
	chunk_offset2 = chunk_offset1 + mp4_video_sample_range_size(file, trak, chunk_sample, sample);
	return chunk_offset2;
}

uint32  mp4_audio_sample_to_offset(mp4_infor_t *file, mp4_trak_t *trak, uint32  sample)
{
	uint32  chunk, chunk_sample, chunk_offset1, chunk_offset2;

	if (trak == NULL) 
	{
		return -1;
	}
	mp4_audio_chunk_of_sample(file, &chunk_sample, &chunk, trak, sample);
	chunk_offset1 = mp4_audio_chunk_to_offset(file, trak, chunk);
	chunk_offset2 = chunk_offset1 + mp4_audio_sample_range_size(file, trak, chunk_sample, sample);
	return chunk_offset2;
}
#endif

uint32 mp4_update_tables(mp4_infor_t *file, 
							mp4_trak_t *trak, 
							uint32  offset, 
							uint32  chunk, 
							uint32  sample, 
							uint32  sample_size,
							uint32  sample_duration,
							uint32  isSyncSample)
{
	if (offset + sample_size > file->mdat.size) 
	{
		file->mdat.size = offset + sample_size;
	}
	mp4_update_stco(file, &(trak->mdia.minf.stbl.stco), chunk, offset);
	mp4_update_stsc(file, &(trak->mdia.minf.stbl.stsc), chunk, 1);
	if (sample_size) 
	{
		mp4_update_stsz(file, &(trak->mdia.minf.stbl.stsz), sample, sample_size);
	}
	if (sample_duration) 
	{
		mp4_update_stts(file, &(trak->mdia.minf.stbl.stts), sample_duration);
	}
	if (isSyncSample) 
	{
		mp4_update_stss(file, &(trak->mdia.minf.stbl.stss), sample+1);
	}
	return 0;
}

//useed for mux
void mp4_trak_duration(mp4_trak_t *trak, uint32  *duration, uint32  *timescale)
{
	if(trak->mdia.minf.stbl.stts.is_video)
	{
		trak->mdia.minf.stbl.stts.end_duration=endduration;
	}
	*duration = trak->mdia.minf.stbl.stts.end_duration;
	*timescale = trak->mdia.mdhd.time_scale;
}

uint32  mp4_track_data_end_position(mp4_trak_t *trak)
{
	uint32  size = 0;
	mp4_stsz_t *stsz = &(trak->mdia.minf.stbl.stsz);
	mp4_stco_t *stco = &(trak->mdia.minf.stbl.stco);
	size = stco->end_offset;
	size += stsz->end_smpsize;
	return size;
}


/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	mp4_interface.c  
* User for : 	mp4 demux mux, 
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"

mp4_trak_t* mp4_add_track(mp4_moov_t *moov)
{
	mp4_trak_t *trak;
	trak = moov->trak[moov->total_tracks] = mp4_calloc(1, sizeof(mp4_trak_t));
	mp4_trak_init(trak);
	trak->tkhd.track_id = moov->mvhd.next_track_id;
	moov->mvhd.next_track_id++;
	moov->total_tracks++;
	return trak;
}

void mp4_init_video_map(mp4_video_map_t *vtrack, mp4_trak_t *trak)
{
	vtrack->track = trak;
	vtrack->current_position = 0;
	vtrack->current_chunk = 1;
	vtrack->last_sample_size = 0;
	vtrack->last_offset = 0;
	vtrack->frame_cache = 0;
	vtrack->frames_cached = 0;
}

void mp4_init_audio_map(mp4_audio_map_t *atrack, mp4_trak_t *trak)
{
	atrack->track = trak;
	atrack->channels = trak->mdia.minf.stbl.stsd.table[0].channels;
	atrack->current_position = 0;
	atrack->current_chunk = 1;
	atrack->last_sample_size = 0;
	atrack->last_offset = 0;
}

void mp4_delete_video_map(mp4_video_map_t *vtrack)
{
	uint32 i;
	if(vtrack->frames_cached)
	{
		for(i = 0; i < vtrack->frames_cached; i++)
		{
			mp4_free(vtrack->frame_cache[i]);
		}
		mp4_free(vtrack->frame_cache);
		vtrack->frames_cached = 0;
	}
}

void mp4_delete_audio_map(mp4_audio_map_t *atrack)
{

}

/*******************************************************************************************
Description:
	debug all parameter information
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	MP4_MUX_DEMUX_RET_NOERR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_dump(mp4_infor_t *file)
{
	MP4_DEMUX_MSG(("mp4_information_dump\n"));
	MP4_DEMUX_MSG(("movie data\n"));
	MP4_DEMUX_MSG(("size %d\n", file->mdat.size));
	MP4_DEMUX_MSG(("start %d\n", file->mdat.start));
	mp4_moov_dump(&(file->moov));
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	delete the mp4 file all information and free all memory our malloc
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	MP4_MUX_DEMUX_RET_NOERR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_delete(mp4_infor_t *file)
{
	uint32 i;
	if(file->total_atracks) 
	{
		for(i = 0; i < file->total_atracks; i++)
			mp4_delete_audio_map(&(file->atracks[i]));
		mp4_free(file->atracks);
	}
	if(file->total_vtracks)
	{
		for(i = 0; i < file->total_vtracks; i++)
			mp4_delete_video_map(&(file->vtracks[i]));
		mp4_free(file->vtracks);
	}
	file->total_atracks = 0;
	file->total_vtracks = 0;
	if(file->preload_size)
	{
		mp4_free(file->preload_buffer);
		file->preload_size = 0;
	}
	mp4_moov_delete(file,&(file->moov));
	mp4_mdat_delete(&(file->mdat));

#if MP4LIB_VERSION2_FOREHANDLE 	
	if(file->stream_videodata){
		file->close_handle_func(file->stream_videodata);
		file->stream_videodata = NULL;
	}
	if(file->stream_audiodata){
		file->close_handle_func(file->stream_audiodata);
		file->stream_audiodata = NULL;
	}
	if(file->stream_videoindx){
		file->close_handle_func(file->stream_videoindx);
		file->stream_videoindx = NULL;
	}
	if(file->stream_audioindx){
		file->close_handle_func(file->stream_audioindx);
		file->stream_audioindx = NULL;
	}
#endif
	
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	use preload buffer to decrement file I/O operate, 
	set preload buffer information and malloc memory
Parameters:
        file: mp4 infor stuctor point (include file preload buffer information)
        preload: preload buffer size
Return:	
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_MALLOC_ERROR: malloc preload buffer error
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_set_preload(mp4_infor_t *file, uint32  preload)
{
	if(!file->preload_size)
	{
		file->preload_size = preload;
		file->preload_buffer = mp4_calloc(1, preload);
		file->preload_start = 0;
		file->preload_end = 0;
		file->preload_ptr = 0;
		
		if(!file->preload_buffer)
			return MP4_MUX_DEMUX_MALLOC_ERROR;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	free preload memory
Parameters:
        file: mp4 infor stuctor point (include file preload buffer information)
Return:	
	MP4_MUX_DEMUX_RET_NOERR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_free_preload(mp4_infor_t *file)
{
	if(file->preload_size)
	{
		mp4_free(file->preload_buffer);
		file->preload_size = 0;
		file->preload_start = 0;
		file->preload_end = 0;
		file->preload_ptr = 0;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

//FILE* mp4_get_fd(mp4_infor_t *file)
//{
//	if(ftell(file->stream) != file->file_position) fseek(file->stream, file->file_position, SEEK_SET);
//	return file->stream;
//}

//seek to data end position in file
uint32 mp4_seek_data_end(mp4_infor_t *file)
{
	return mp4_set_file_position(file, file->mdat.size + file->mdat.start);
}
//seek to data strat position in file
uint32 mp4_seek_data_start(mp4_infor_t *file)
{
	uint32 i;
	
	if(file->total_atracks)
		for(i = 0; i < file->total_atracks; i++)
		{
			file->atracks[i].current_position = 0;
			file->atracks[i].current_chunk = 1;
		}
	if(file->total_vtracks)
		for(i = 0; i < file->total_vtracks; i++)
		{
			file->vtracks[i].current_position = 0;
			file->vtracks[i].current_chunk = 1;
		}
	return mp4_set_file_position(file, file->mdat.start + HEADER_LENGTH);
}

/*******************************************************************************************
Description:
	 set file position to the audio frame data start position
Parameters:
        file: mp4 infor stuctor point (include file handle)
        sample: audio frame ID
Return:	
	the frame data start position in file
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32  mp4_set_audio_position(mp4_infor_t *file, uint32  sample)
{
	uint32  offset, chunk_sample, chunk;
	mp4_trak_t *trak;

	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;

	trak = file->atracks[DEFAULT_TRACK].track;
	file->atracks[DEFAULT_TRACK].current_position = sample;
	mp4_chunk_of_sample(file, &chunk_sample, &chunk, trak, sample);
	if ((chunk_sample == sample)||(file->reset_audio_position))
	{
		file->atracks[DEFAULT_TRACK].current_chunk = chunk;
		offset = mp4_sample_to_offset(file, trak, sample);
		mp4_set_file_position(file, offset);
		file->atracks[DEFAULT_TRACK].last_offset = offset;
		file->reset_audio_position = 0;
	}
	else		
	{	//this process can use min time 
		offset = file->atracks[DEFAULT_TRACK].last_offset + file->atracks[DEFAULT_TRACK].last_sample_size;
		mp4_set_file_position(file, offset);
		file->atracks[DEFAULT_TRACK].last_offset = offset;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32  mp4_set_audio_position(mp4_infor_t *file, uint32  sample)
{
	uint32  offset, chunk_sample, chunk;
	mp4_trak_t *trak;

	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;

	trak = file->atracks[DEFAULT_TRACK].track;
	file->atracks[DEFAULT_TRACK].current_position = sample;
	mp4_audio_chunk_of_sample(file, &chunk_sample, &chunk, trak, sample);
	if ((chunk_sample == sample)||(file->reset_audio_position))
	{
		file->atracks[DEFAULT_TRACK].current_chunk = chunk;
		offset = mp4_audio_sample_to_offset(file, trak, sample);
		file->seek_func(file->stream_audiodata,offset, SEEK_SET);
//		mp4_set_file_position(file, offset);
		file->atracks[DEFAULT_TRACK].last_offset = offset;
		file->reset_audio_position = 0;
	}
	else		
	{	//this process can use min time 
		offset = file->atracks[DEFAULT_TRACK].last_offset + file->atracks[DEFAULT_TRACK].last_sample_size;
		file->seek_func(file->stream_audiodata,offset, SEEK_SET);
//		mp4_set_file_position(file, offset);
		file->atracks[DEFAULT_TRACK].last_offset = offset;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

/*******************************************************************************************
Description:
	 set file position to the video frame data start position
Parameters:
        file: mp4 infor stuctor point (include file handle)
        frame: video frame ID
Return:	
	the frame data start position in file
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32  mp4_set_video_position(mp4_infor_t *file, uint32  frame)
{
	uint32  offset, chunk_sample, chunk;
	mp4_trak_t *trak;

	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	trak = file->vtracks[DEFAULT_TRACK].track;
	file->vtracks[DEFAULT_TRACK].current_position = frame;
	mp4_chunk_of_sample(file, &chunk_sample, &chunk, trak, frame);
	if ((chunk_sample == frame)||(file->reset_video_position))
	{
		file->vtracks[DEFAULT_TRACK].current_chunk = chunk;
		offset = mp4_sample_to_offset(file, trak, frame);
		mp4_set_file_position(file, offset);
		file->vtracks[DEFAULT_TRACK].last_offset = offset;
		file->reset_video_position = 0;
	}
	else
	{	//this process can use min time 
		offset = file->vtracks[DEFAULT_TRACK].last_offset+file->vtracks[DEFAULT_TRACK].last_sample_size;
		mp4_set_file_position(file, offset);
		file->vtracks[DEFAULT_TRACK].last_offset = offset;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32  mp4_set_video_position(mp4_infor_t *file, uint32  frame)
{
	uint32  offset, chunk_sample, chunk;
	mp4_trak_t *trak;

	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	trak = file->vtracks[DEFAULT_TRACK].track;
	file->vtracks[DEFAULT_TRACK].current_position = frame;
	mp4_video_chunk_of_sample(file, &chunk_sample, &chunk, trak, frame);
	if ((chunk_sample == frame)||(file->reset_video_position))
	{
		file->vtracks[DEFAULT_TRACK].current_chunk = chunk;
		offset = mp4_video_sample_to_offset(file, trak, frame);
//		mp4_set_file_position(file, offset);
		file->seek_func(file->stream_videodata, offset, SEEK_SET);
		file->vtracks[DEFAULT_TRACK].last_offset = offset;
		file->reset_video_position = 0;
	}
	else
	{	//this process can use min time 
		offset = file->vtracks[DEFAULT_TRACK].last_offset+file->vtracks[DEFAULT_TRACK].last_sample_size;
//		mp4_set_file_position(file, offset);
		file->seek_func(file->stream_videodata, offset, SEEK_SET);
		file->vtracks[DEFAULT_TRACK].last_offset = offset;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

//audio current sample ID
//uint32  mp4_audio_position(mp4_infor_t *file)
//{
//	return file->atracks[DEFAULT_TRACK].current_position;
//}
//video current sample ID
//uint32  mp4_video_position(mp4_infor_t *file)
//{
//	return file->vtracks[DEFAULT_TRACK].current_position;
//}

/*******************************************************************************************
Description:
	write mp4 information to file 
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_RET_NOERR: file have no moov atom(parameter information box)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.18		create first version
********************************************************************************************/
uint32 mp4_write_info(mp4_infor_t *file)
{
	uint32 result,buffersize=128*1024;
	int8 *buffer=NULL;
	int8 audiotmpfile[]={'a',0,'u',0,'d',0,'i',0,'o',0,'.',0,'t',0,'m',0,'p',0,0,0};
	int8 videotmpfile[]={'v',0,'i',0,'d',0,'e',0,'o',0,'.',0,'t',0,'m',0,'p',0,0,0};
	int8 sttstmpfile[]={'s',0,'t',0,'t',0,'s',0,'.',0,'t',0,'m',0,'p',0,0,0};
#if MP4_IN_820_MEMORY_LIMITED
	if(file->rw_mode)
	{
#ifdef _WIN32
		file->table_audio = mp4_fileopen("audio.tbl.tmp", "wb+");
		file->table_video = mp4_fileopen("video.tbl.tmp", "wb+");
		file->table_duration = mp4_fileopen("stts.tbl.tmp", "wb+");
#else
		buffer=(int8 *)mp4_malloc(buffersize);
		file->table_audio = mp4_fileopen(audiotmpfile, "w+");
		mp4_filewrite(file->table_audio, buffer, buffersize);
		mp4_fileseek(file->table_audio, 0, SEEK_SET);
		//file->seek_func(file->table_audio,1024*1024,SEEK_SET);
		//file->seek_func(file->table_audio,0,SEEK_SET);
		file->table_video = mp4_fileopen(videotmpfile, "w+");
		mp4_filewrite(file->table_video, buffer, buffersize);
		mp4_fileseek(file->table_video, 0, SEEK_SET);
		//file->seek_func(file->table_video,1024*1024,SEEK_SET);
		//file->seek_func(file->table_video,0,SEEK_SET);
		file->table_duration = mp4_fileopen(sttstmpfile, "w+");
		mp4_filewrite(file->table_duration, buffer, buffersize);
		mp4_fileseek(file->table_duration, 0, SEEK_SET);
		//file->seek_func(file->table_duration,1024*1024,SEEK_SET);
		//file->seek_func(file->table_duration,0,SEEK_SET);
		mp4_free((void *)buffer);
#endif
	}
#endif
	
	result = mp4_write_uint32(file, 0x14);
	if(result) return result;
	result = mp4_write_char32(file, "ftyp");
	if(result) return result;
	result = mp4_write_char32(file, "3gp4");
	if(result) return result;
	result = mp4_write_uint32(file, 0x200);
	if(result) return result;
	result = mp4_write_char32(file, "3gp4");
	if(result) return result;
	file->mdat.start = 0x14; 
	file->file_position = 0x14;

	/* start the data atom */
	result = mp4_write_uint32(file, 0);
	if(result) return result;
	return mp4_write_char32(file, "mdat");

}
/*******************************************************************************************
Description:
	write mp4 information to file 
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_RET_NOERR: file have no moov atom(parameter information box)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.19		create first version
********************************************************************************************/
uint32 mp4_write_table(mp4_infor_t *file)
{
	uint32 result;
	result = mp4_write_mdat(file, &(file->mdat));
	if(result)
       	return result;
	return  mp4_write_moov(file, &(file->moov));
}


/*******************************************************************************************
Description:
	read mp4 information form file except data
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_RET_NOERR: file have no moov atom(parameter information box)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_read_info(mp4_infor_t *file)
{
	uint32 time_scale, found_moov, found_data;
	uint32 result, i, track;
	mp4_atom_t  leaf_atom;
	mp4_esds_t* esds;
	mp4_trak_t*  trak;
	uint32 start_position = mp4_get_file_position(file);
	
	result = file->seek_func(file->stream, 0, SEEK_END);
	MP4_DEMUX_FILE((" file seek return = %d",result));
	
	file->total_length = file->tell_func(file->stream);
	MP4_DEMUX_FILE(("file->total_length = %d \n",file->total_length));
	
	result = file->seek_func(file->stream, 0, SEEK_SET);
	MP4_DEMUX_FILE((" file seek return = %d",result));

	/*ready to scan file*/
	found_moov = 0;
	found_data = 0;
	result = mp4_set_file_position(file, 0);	/*seek to file start*/
	if(result) return result;
	
	while(mp4_get_file_position(file)+7 < file->total_length)
	{
		result = mp4_atom_read_header(file, &leaf_atom);
		if(result) return result;
		
		if(mp4_atom_is(&leaf_atom, "mdat")) 
		{
			found_data = 1;
			result = mp4_read_mdat(file, &(file->mdat), &leaf_atom);
			//when data be destroy but table ok, than return success
			if(found_moov)
				break;
			//read data error
			if(result) return result;
		} 
		else if(mp4_atom_is(&leaf_atom, "moov")) 
		{
			mp4_set_preload(file, PRELOAD_BUFFER_SIZE);
			result = mp4_read_moov(file, &(file->moov), &leaf_atom);
			//moov table be destroy, can not demux this file
			if(result) return result;
			//moov track is ok, the set flag
			found_moov = 1;
			//if already have check data, then need not demux continue...
			if(found_data)
				break;
		} 
		else 
		{
			result = mp4_atom_skip(file, &leaf_atom);
			//when data and table all ok, than return success
			if(found_moov&&found_data)
				break;
			//file have be destroy, have no datatrack or moovtrack
			if(result) return result;
		}
		
	}

	/* go back to the original position */
	result = mp4_set_file_position(file, start_position);
	if(result) return result;
#if MP4LIB_VERSION2_FOREHANDLE 	
	file->stream_videodata = file->duplicate_handle_func(file->stream);
	file->stream_audiodata = file->duplicate_handle_func(file->stream);
#endif

	if(found_moov) 
	{
#if MP4LIB_VERSION2_FOREHANDLE 
		file->seek_func(file->stream_audioindx,file->audioindx_origpos,SEEK_SET);
		file->seek_func(file->stream_videoindx,file->videoindx_origpos,SEEK_SET);
#endif
		/* get tables for all audio tracks */
		file->total_atracks = 0;
		for(i = 0; i < file->moov.total_tracks; i++)
		{
			if(file->moov.trak[i]->mdia.minf.is_audio)
			{
				//this protasis add by myself, is not standard define
				if (file->moov.trak[i]->mdia.minf.stbl.stsd.table[0].esds.object_type!=0x40)
					file->moov.trak[i]->mdia.minf.is_audio = 0;
				else
					file->total_atracks ++;
			}
		}
		/* initialize audio tracks map */
		if(file->total_atracks)
		{
			file->total_atracks = 1;	/*now support one track audio*/
			file->atracks = (mp4_audio_map_t*)mp4_calloc(1, sizeof(mp4_audio_map_t) * file->total_atracks);
			for(i = 0, track = 0; i < file->total_atracks; i++) 
			{
				while(!file->moov.trak[track]->mdia.minf.is_audio)
					track++;
				mp4_init_audio_map(&(file->atracks[i]), file->moov.trak[track]);
			}
			/* get audio tracks total time and total samples */
			trak = file->atracks[DEFAULT_TRACK].track;
			time_scale = (uint32)trak->mdia.mdhd.time_scale;
			file->total_asamples =(uint32) trak->mdia.minf.stbl.stsz.total_entries;
			file->total_atime = (uint32) trak->mdia.mdhd.duration;
			if(file->total_atime&&file->total_asamples)
			{
				trak->mdia.minf.stbl.stts.default_duration = file->total_atime/file->total_asamples;
				trak->mdia.minf.stbl.stts.default_samples = file->total_asamples;
#if !(MP4LIB_VERSION2_FOREHANDLE) 				
				mp4_read_stts_table(file, &trak->mdia.minf.stbl.stts, 1);
#else
				mp4_read_audio_stts_table(file, &trak->mdia.minf.stbl.stts, 1);
#endif
				file->total_atime=(uint32) (file->total_atime/time_scale*1000);
			}
			else
			{
#if !(MP4LIB_VERSION2_FOREHANDLE) 			
			 	file->total_atime = mp4_track_times(file, trak);
				file->total_asamples = mp4_track_samples(file, trak);
#else
				file->total_vtime = mp4_audio_track_times(file, trak);
				file->total_vsamples = mp4_audio_track_samples(file, trak);
#endif
				file->total_atime=(uint32) (file->total_atime/time_scale*1000);
			}
		}
		
		/* get tables for all video tracks */
		file->total_vtracks = 0;
		for(i = 0; i < file->moov.total_tracks; i++)
		{
			if(file->moov.trak[i]->mdia.minf.is_video) 
				file->total_vtracks++;
		}
		/* initialize video tracks map */
		if(file->total_vtracks)
		{
			file->total_vtracks = 1; 	/*now support one track video*/
			file->vtracks = (mp4_video_map_t*)mp4_calloc(1, sizeof(mp4_video_map_t) * file->total_vtracks);
			for(track = 0, i = 0; i < file->total_vtracks; i++)
			{
				while(!file->moov.trak[track]->mdia.minf.is_video)
					track++;
				mp4_init_video_map(&(file->vtracks[i]), file->moov.trak[track]);
			}
			trak = file->vtracks[DEFAULT_TRACK].track;
			/* get video tracks total time and total samples */
			time_scale = (uint32)trak->mdia.mdhd.time_scale;
			file->total_vsamples =(uint32) trak->mdia.minf.stbl.stsz.total_entries;
			file->total_vtime = (uint32) trak->mdia.mdhd.duration;
			if(file->total_vtime&&file->total_vsamples)
			{
				trak->mdia.minf.stbl.stts.default_duration = file->total_vtime/file->total_vsamples;
				trak->mdia.minf.stbl.stts.default_samples = file->total_vsamples;
#if !(MP4LIB_VERSION2_FOREHANDLE) 				
				mp4_read_stts_table(file, &trak->mdia.minf.stbl.stts, 1);
#else
				mp4_read_video_stts_table(file, &trak->mdia.minf.stbl.stts, 1);
#endif
				file->total_vtime=(uint32) (file->total_vtime/time_scale*1000);
			}
			else
			{	
#if !(MP4LIB_VERSION2_FOREHANDLE) 			
				file->total_vtime = mp4_track_times(file, trak);
				file->total_vsamples = mp4_track_samples(file, trak);
#else
				file->total_vtime = mp4_video_track_times(file, trak);
				file->total_vsamples = mp4_video_track_samples(file, trak);
#endif
				file->total_vsamples=(uint32) (file->total_vsamples/time_scale*1000);
			}
			/* get video width and height */
			if(trak->mdia.minf.stbl.stsd.table[0].width)
				file->video_srcwidth = trak->mdia.minf.stbl.stsd.table[0].width;
			else
				file->video_srcwidth = (uint32)trak->tkhd.track_width;
			if (trak->mdia.minf.stbl.stsd.table[0].height) 
				file->video_srcheight = (uint32)trak->mdia.minf.stbl.stsd.table[0].height;
			else
				file->video_srcheight =(uint32)trak->tkhd.track_height;
			/*mp4v need check vop width and height form decode config*/
			if(file->video_type == MP4_TYPE_VIDEO_IS_MP4V)
			{
				file->mp4v_vop_info.video_object_layer_width = file->video_srcwidth;
				file->mp4v_vop_info.video_object_layer_height = file->video_srcheight;
				esds = &trak->mdia.minf.stbl.stsd.table[0].esds;
				mp4_analysize_mp4v_decoderconfig((unsigned char *) esds->decoderConfig, (uint32*)&esds->decoderConfigLen, &file->mp4v_vop_info);
				file->video_vopwidth = file->mp4v_vop_info.video_object_layer_width;
				file->video_vopheight = file->mp4v_vop_info.video_object_layer_height;
				//848 hardware decoder modle does not sport 16bits time resolution. added by dnn 2008/7/28
				if(file->mp4v_vop_info.vop_time_increment_resolution==16) {
					return MP4_THIS_VERSION_CANNOT_DEMUX;
				}	
			}
			/*s263 need check vop width and height form stream data*/
			if(file->video_type == MP4_TYPE_VIDEO_IS_S263)
			{
				file->h263_vop_info.video_object_layer_width = file->video_srcwidth;
				file->h263_vop_info.video_object_layer_height = file->video_srcheight;
				//added by dnn 2008/7/28
				//mp4_analysize_h263_streamheader((unsigned char *) esds->decoderConfig, (uint32*)&esds->decoderConfigLen, &file->h263_vop_info);
				file->video_vopwidth = file->h263_vop_info.video_object_layer_width;
				file->video_vopheight = file->h263_vop_info.video_object_layer_height;
			}
		}
	}

	if(found_moov) 
		return MP4_MUX_DEMUX_RET_NOERR; 
	else 
		return MP4_MUX_DEMUX_RET_ERROR;
}

/*******************************************************************************************
Description:
	check the user file is a mp4 file ???
Parameters:
       filename: user mp4 path name point
Return:	
	TRUE: the file is ok mp4 file, we can demux
       FALSE: the file is error mp4 format file, can not demux
       
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_check_sig(mp4_infor_t *file)
{
	mp4_atom_t leaf_atom;
	uint32 result;
	uint32 found_moov;

	result = file->seek_func(file->stream, 0, SEEK_END);
	MP4_DEMUX_FILE((" file seek return = %d",result));
	file->total_length = file->tell_func(file->stream);
	MP4_DEMUX_FILE(("file->total_length = %d \n",file->total_length));
	result = file->seek_func(file->stream, 0, SEEK_SET);
	MP4_DEMUX_FILE((" file seek return = %d",result));

	/*ready to scan file*/
	mp4_set_file_position(file, 0);	/*seek to file start*/
	result = 0;
	found_moov=0;
	while((!result)&& (mp4_get_file_position(file)+7 < file->total_length))
	{
		result = mp4_atom_read_header(file, &leaf_atom);
		MP4_DEMUX_DBG(("leaf_atom.start = %d",leaf_atom.start));
		MP4_DEMUX_DBG(("leaf_atom.end = %d",leaf_atom.end));
		MP4_DEMUX_DBG(("leaf_atom.size = %d",leaf_atom.size));
		/* just want the "moov" atom */
		if(mp4_atom_is(&leaf_atom, "moov"))
		{	
			found_moov = 1;
			//check table is ok??
			if(leaf_atom.end>file->total_length)
			{
				found_moov = 0;
			}
			result = MP4_MUX_DEMUX_RET_ERROR;
		}
		else
		{	
			result = mp4_atom_skip(file, &leaf_atom);	
		}
	}
	return found_moov;
}


/*******************************************************************************************
Description:
	open a file get file all information and read to memory; if memory limited, then read somes
	or creat a new mp4 file
Parameters:
       filename: user mp4 path name point
       rw_mode: =1, creat a new file
              	 =0, open a mp4 file an get some information
Return:	
	mp4 information structor point.
	if =NULL, then means open file error or creat file error
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
mp4_infor_t* mp4_open(char *filename, uint32 rw_mode)
{
	mp4_infor_t *new_file;
	
	new_file = (mp4_infor_t *)mp4_malloc(sizeof(mp4_infor_t));

	mp4_memset((char*)new_file, 0, sizeof(mp4_infor_t));
	mp4_mdat_init(&(new_file->mdat));
	mp4_moov_init(&(new_file->moov));

	new_file->rw_mode = rw_mode;
	new_file->mdat.start = 0;
	new_file->max_vbuffersize	=MP4_VIDEO_MAX_BUFFER_SIZE;		/* video buffer max size(bytes)*/
	new_file->max_vframeonce	=MP4_VIDEO_MAX_FRAME_READ;		/* video max frame read or write once*/
	new_file->min_vframeonce	=MP4_VIDEO_MIN_FRAME_READ;		/* video min frame read or write once */
	new_file->max_abuffersize	=MP4_AUDIO_MAX_BUFFER_SIZE;		/* audio buffer max size(bytes)*/
	new_file->max_aframeonce	=MP4_AUDIO_MAX_FRAME_READ;		/* audio max frame read or write once*/
	new_file->min_aframeonce	=MP4_AUDIO_MIN_FRAME_READ;		/* audio min frame read or write once */
	new_file->reset_audio_position = 1;	/*the flag =1 means audio data position need use stsc,stco,stsz table to get, 
	                                                                =0, only need last position and stsz table to get */
	new_file->reset_video_position = 1;	/*the flag =1 means video data position need use stsc,stco,stsz table to get, 
	                                                                =0, only need last position and stsz table to get */
	new_file->video_firsttime = 1;		/*the flag =1 means need fill decode config data to data buffer first */
	
	return new_file;
}

/*******************************************************************************************
Description:
	close our open or creat mp4 file, free mp4 information structor memory our malloc
	if our creat file, the write all information to file before close
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	MP4_MUX_DEMUX_RET_NOERR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_close(mp4_infor_t *file)
{
	uint32 result = MP4_MUX_DEMUX_RET_NOERR;
	int8 audiotmpfile[]={'a',0,'u',0,'d',0,'i',0,'o',0,'.',0,'t',0,'m',0,'p',0,0,0};
	int8 videotmpfile[]={'v',0,'i',0,'d',0,'e',0,'o',0,'.',0,'t',0,'m',0,'p',0,0,0};
	int8 sttstmpfile[]={'s',0,'t',0,'t',0,'s',0,'.',0,'t',0,'m',0,'p',0,0,0};
	if(file->rw_mode)
	{//write record file information table
		result = mp4_write_table(file);
#if MP4_IN_820_MEMORY_LIMITED
		mp4_fileclose(file->table_audio);
		mp4_filedelete(audiotmpfile);
		mp4_fileclose(file->table_video);
		mp4_filedelete(videotmpfile);
		mp4_fileclose(file->table_duration);
		mp4_filedelete(sttstmpfile);
#endif
	}
	// free mp4 information structor all memory
	//MMDPrintf("this file position is %d and 0x%x\n",file->file_position,file->file_position);
	mp4_delete(file);
	//free mp4 structor memory
	mp4_free(file);
	return result;
}

/*******************************************************************************************
Description:
	set mux file audio parameter information
Parameters:
	file : mp4 infor stuctor point (include file preload buffer information)
	channels : 
	bits : 
	sample_rate :
	sample_size : 
	time_scale :
	sample_duration :
	compressor:
Return:	
	MP4_MUX_DEMUX_RET_NOERR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_set_audio(mp4_infor_t *file, 
						   uint32 channels, 
						   uint32 bits, 
						   uint32  sample_rate, 
						   uint32  sample_size, 
						   uint32  time_scale, 
						   uint32  sample_duration, 
						   char *compressor)
{
	uint32 i;
	mp4_trak_t *trak;

	/* delete any existing tracks */
	for(i = 0; i < file->total_atracks; i++) 
	{
		mp4_delete_audio_map(&(file->atracks[i]));
		mp4_delete_trak(file,&(file->moov), file->atracks[i].track);
	}
	mp4_free(file->atracks);
	file->atracks = NULL;	
	file->total_atracks = 0;

	if(channels) 
	{
		file->atracks = (mp4_audio_map_t*)mp4_calloc(1, sizeof(mp4_audio_map_t));
		trak = mp4_add_track(&(file->moov));
		mp4_trak_init_audio(file, trak, channels, bits,  sample_rate,
			sample_size, time_scale, sample_duration, compressor);
		mp4_init_audio_map(&(file->atracks[0]), trak);
		file->atracks[file->total_atracks].track = trak;
		file->atracks[file->total_atracks].channels = channels;
		file->atracks[file->total_atracks].current_position = 0;
		file->atracks[file->total_atracks].current_chunk = 1;
		file->atracks[file->total_atracks].last_sample_size = 0;
		file->total_atracks++;
	}
	return MP4_MUX_DEMUX_RET_NOERR;   
}
/*******************************************************************************************
Description:
	set mux file video parameter information
Parameters:
	file : mp4 infor stuctor point (include file preload buffer information)
	tracks: 
	frame_w: 
	frame_h:
	time_scale:
	sample_duration:
	compressor:
Return:	
	MP4_MUX_DEMUX_RET_NOERR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_set_video(mp4_infor_t *file, 
						   uint32 tracks, 
						   uint32 frame_w, 
						   uint32 frame_h,
						   uint32  time_scale,
						   uint32  sample_duration,
						   char *compressor)
{
	uint32 i;
	mp4_trak_t *trak;

	/* delete any existing tracks */
	for(i = 0; i < file->total_vtracks; i++)
	{
		mp4_delete_video_map(&(file->vtracks[i]));
		mp4_delete_trak(file,&(file->moov), file->vtracks[i].track);
	}
	mp4_free(file->vtracks);
	file->vtracks = NULL;	
	file->total_vtracks = 0;

	if (tracks > 0) 
	{
		file->total_vtracks = tracks;
		file->vtracks = (mp4_video_map_t*)mp4_calloc(1, sizeof(mp4_video_map_t) * file->total_vtracks);
		for(i = 0; i < tracks; i++)
		{
			trak = mp4_add_track(&(file->moov));
			mp4_trak_init_video(file, trak, frame_w, frame_h, sample_duration,
				time_scale, compressor);
			mp4_init_video_map(&(file->vtracks[i]), trak);
		}
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

int mp4_set_mp4v_decoder_config(mp4_infor_t *file, void* dec_config)
{
	mp4_esds_t* set_config;
	mp4_esds_t* input_config;
	char 	      v820_config[19]={0x00, 0x00,0x01,0x00,0x00,0x00,0x01,0x20,0x00,0xc8,0x88,0x80,0x0f,0x50,0x58,0x41,0x21,0x41,0x83};

	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_RET_NOERR;
	//set video type
	file->video_type = MP4_TYPE_VIDEO_IS_MP4V;
	//get last video decoder config
	set_config = &file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds;
	//free last config buffer
	if (set_config->decoderConfig) 
		mp4_free(set_config->decoderConfig);
	set_config->decoderConfigLen = 0;
	//set video decode config
	if (!dec_config) 
	{
		//malloc new config buffer
		set_config->decoderConfig = mp4_malloc(19);
		if (!set_config->decoderConfig) 
			return MP4_MUX_DEMUX_MALLOC_ERROR;
		//copy config data
		set_config->decoderConfigLen = 19;
		mp4_memcopy((char*)set_config->decoderConfig, (const char*)v820_config, 19);
	}
	else
	{
		//check if have config data
		input_config = (mp4_esds_t*)dec_config;
		if(!input_config->decoderConfigLen)
			return MP4_MUX_DEMUX_RET_NOERR;
		if(!input_config->decoderConfig)
			return MP4_MUX_DEMUX_RET_NOERR;
		//malloc new config buffer
		set_config->decoderConfig = mp4_malloc(input_config->decoderConfigLen);
		if (!set_config->decoderConfig) 
			return MP4_MUX_DEMUX_MALLOC_ERROR;
		//copy config data
		set_config->decoderConfigLen = input_config->decoderConfigLen;
		mp4_memcopy((char*)set_config->decoderConfig, (const char*)input_config->decoderConfig, input_config->decoderConfigLen);
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

int mp4_set_mp4a_decoder_config(mp4_infor_t *file, void* dec_config)
{
	mp4_esds_t* set_config;
	mp4_esds_t* input_config;

	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	//set video type
	file->audio_type = MP4_TYPE_AUDIO_IS_MP4A;
	//check input config parameter
	if (!dec_config) 
		return MP4_MUX_DEMUX_RET_NOERR;
	//check if have config data
	input_config = (mp4_esds_t*)dec_config;
	if(!input_config->decoderConfigLen)
		return MP4_MUX_DEMUX_RET_NOERR;
	if(!input_config->decoderConfig)
		return MP4_MUX_DEMUX_RET_NOERR;
	//set video decoder config
	set_config = &file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds;
	//free last config buffer
	if (set_config->decoderConfig) 
		mp4_free(set_config->decoderConfig);
	//malloc new config buffer
	set_config->decoderConfig = mp4_malloc(input_config->decoderConfigLen);
	if (!set_config->decoderConfig) 
		return MP4_MUX_DEMUX_MALLOC_ERROR;
	//copy config data
	set_config->decoderConfigLen = input_config->decoderConfigLen;
	mp4_memcopy((char*)set_config->decoderConfig, (const char*)input_config->decoderConfig, input_config->decoderConfigLen);
	return MP4_MUX_DEMUX_RET_NOERR;
}

int mp4_set_d263_decoder_config(mp4_infor_t *file, void* d263_config)
{
	mp4_d263_t* d263_set;
	mp4_d263_t* d263_input;
	
	if (!file->total_vtracks)
		return MP4_MUX_DEMUX_RET_NOERR;
	//set video type
	file->video_type = MP4_TYPE_VIDEO_IS_S263;
	//check input config parameter
	if (!d263_config) 
		return MP4_MUX_DEMUX_RET_NOERR;
	//set d263 information
	d263_input = (mp4_d263_t*)d263_config;
	d263_set = &file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].d263;
	d263_set->h263level = d263_input->h263level;
	d263_set->h263profile = d263_input->h263profile;
	return MP4_MUX_DEMUX_RET_NOERR;
}

int mp4_set_damr_decoder_config(mp4_infor_t *file, void* damr_config)
{
	mp4_damr_t* damr_set;
	mp4_damr_t* damr_input;
	
	if (!file->total_atracks)
		return MP4_MUX_DEMUX_RET_NOERR;
	//set audio type
	file->audio_type = MP4_TYPE_AUDIO_IS_SAMR;
	//check input config parameter
	if (!damr_config) 
		return MP4_MUX_DEMUX_RET_NOERR;
	//set damr information
	damr_input = (mp4_damr_t*)damr_config;
	damr_set = &file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].damr;
	damr_set->frame_per_sample = damr_input->frame_per_sample;
	return MP4_MUX_DEMUX_RET_NOERR;
}

int mp4_get_video_decoder_config(mp4_infor_t *file, void** decconfig, unsigned int * len)
{
	//initialize return parameter
	(*decconfig )= NULL;
	//check if have video
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	//get decoder config
	if(file->video_type == MP4_TYPE_VIDEO_IS_MP4V) {
//		(*decconfig ) = (void*)&file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds;
		(*decconfig ) = (void*)&file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds.decoderConfig;
		(*len) = file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds.decoderConfigLen;
	}
	if(file->video_type == MP4_TYPE_VIDEO_IS_S263) {
		(*decconfig ) = (void*)&file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].d263;
		(*len) = sizeof(mp4_d263_t);
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

int mp4_get_audio_decoder_config(mp4_infor_t *file, void** decconfig, unsigned int *len)
{
	//initialize return parameter
	(*decconfig )= NULL;
	//check if have video
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	//get esds table
	if(file->audio_type == MP4_TYPE_AUDIO_IS_MP4A){
//		(*decconfig ) = (void*)&file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds;
		(*decconfig ) = (void*)&file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds.decoderConfig;
		(*len) = file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds.decoderConfigLen;
	}
	if(file->audio_type == MP4_TYPE_AUDIO_IS_SAMR) {
		(*decconfig ) = (void*)&file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].damr;
		(*len) = sizeof(mp4_damr_t);
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_set_time_scale(mp4_infor_t *file, uint32  time_scale)
{
	file->moov.mvhd.time_scale = (uint32)time_scale;
	return MP4_MUX_DEMUX_RET_NOERR;
}

// use time scale to calculate frame rate
uint32  mp4_get_time_scale(mp4_infor_t *file, uint32  *timescale)
{
	 *timescale =(uint32) file->moov.mvhd.time_scale;
	 return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	get audio sample data bit width
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: file have no audio 
	 16 or other: audio bits width
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_audio_bits(mp4_infor_t *file, uint32  *bits)
{
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	
	*bits = (uint32)file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].sample_size;

	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	get audio channels(always is two channel)
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: file have no audio 
	 2 or other: audio channels
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_audio_channels(mp4_infor_t *file, uint32  *channels)
{
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;

	if (!file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].channels) 
		*channels = file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].channels;
	else
		*channels = file->atracks[DEFAULT_TRACK].channels;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	 get audio sample rate
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: file have no audio 
	 8000,16000,32000,48000,44100 or other: audio sample rate
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_audio_sample_rate(mp4_infor_t *file, uint32  *samplerate)
{
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;

	if (file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].sample_rate>0)
		*samplerate =(uint32)file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].sample_rate;
	else
		*samplerate = (uint32)file->atracks[DEFAULT_TRACK].track->mdia.mdhd.time_scale;

	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	 get audio track time scale(this is same as samplerate in normal)
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: file have no audio 
	 8000,16000,32000,48000,44100 or other: get audio track time scale
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_audio_time_scale(mp4_infor_t *file, uint32 *timescale)
{
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	
	*timescale = (uint32)file->atracks[DEFAULT_TRACK].track->mdia.mdhd.time_scale;

	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	 get audio sample duration (audio is not change always)
Parameters:
        file: mp4 infor stuctor point (include file handle)
        sample: audio sample id(always start on 0 )
Return:	
	 0: file have no audio 
	 other: audio sample duration
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_audio_sample_duration(mp4_infor_t *file, uint32  sample, uint32  *duration)
{
	mp4_stts_t *stts;
	mp4_stsz_t *stsz;
	uint32 i = 0;
	uint32 f = 0;

	*duration = 0;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	
	stsz = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);
	if(sample>=stsz->total_entries)
		return MP4_DEMUX_END_OF_AUDIO;
	
	stts = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

       if (sample<stts->start_samples)
      		mp4_read_stts_table(file, stts,1);
       while((sample>=stts->end_samples)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
       	mp4_read_stts_table(file, stts,0);


        i = 0;
	 f = stts->start_samples;
	while (i < stts->entries_allocated) 
	{
		if ((f + stts->table[i].sample_count) > sample) 
		{
			*duration = (uint32)stts->table[i].sample_duration;
			break;
		}
		else 
		{
			f += stts->table[i].sample_count ;
			i++;
		}
	}
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_audio_sample_duration(mp4_infor_t *file, uint32  sample, uint32  *duration)
{
	mp4_stts_t *stts;
	mp4_stsz_t *stsz;
	uint32 i = 0;
	uint32 f = 0;

	*duration = 0;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	
	stsz = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);
	if(sample>=stsz->total_entries)
		return MP4_DEMUX_END_OF_AUDIO;
	
	stts = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

       if (sample<stts->start_samples)
//      	mp4_read_stts_table(file, stts,1);
      		mp4_read_audio_stts_table(file, stts,1);
       while((sample>=stts->end_samples)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
//       	mp4_read_stts_table(file, stts,0);
		mp4_read_audio_stts_table(file, stts,0);	

        i = 0;
	 f = stts->start_samples;
	while (i < stts->entries_allocated) 
	{
		if ((f + stts->table[i].sample_count) > sample) 
		{
			*duration = (uint32)stts->table[i].sample_duration;
			break;
		}
		else 
		{
			f += stts->table[i].sample_count ;
			i++;
		}
	}
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif
/*******************************************************************************************
Description:
	 get audio compressor, then can know how to decode
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 NULL: file have no codec information
	 other: audio codec name point(length = 4 bytes)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_audio_compressor(mp4_infor_t *file, char **compressor)
{
  	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;

  	if (!file->atracks[DEFAULT_TRACK].track)
    		 (*compressor) = NULL;
  	else
	 	(*compressor) = file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].format;

	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	 get audio frame size, if is not 0, then means audio frames is not change in all file
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: file have no codec information
	 other: audio frame size
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_audio_stsz_frame_size(mp4_infor_t *file, uint32 *framesize)
{
	mp4_trak_t *trak;
	
	*framesize =0;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	
	trak = file->atracks[DEFAULT_TRACK].track;
	 *framesize =(uint32) trak->mdia.minf.stbl.stsz.sample_size;
	 return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	 get audio frame size, if is not 0, then means audio frames is not change in all file
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: file have no codec information
	 other: audio frame size
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.17		create first version
********************************************************************************************/
#if MP4_IN_820_QUICK_GET_FRAMESIZE
uint32 mp4_audio_max_frame_size(mp4_infor_t *file, uint32 *framesize)
{
	uint32 maxsize;
	mp4_stsz_t *stsz;
	
	*framesize =0;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;

	stsz = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);

	if(stsz->sample_size)
	{	maxsize = stsz->sample_size;	}
	else
	{	maxsize = 1024;	}
	
	MP4_DEMUX_MSG(("audio max frame size = %d",maxsize));
#if MP4_IN_820_MEMORY_LIMITED
	if(maxsize>MP4_AUDIO_MAX_BUFFER_SIZE)
	{
		MP4_DEMUX_WARNING(("audio max frame size = %d",maxsize));
		MP4_DEMUX_WARNING(("MP4_AUDIO_MAX_BUFFER_SIZE < %d",maxsize));
	}
#endif
	maxsize = maxsize*MP4_AUDIO_MIN_FRAME_READ;
#if MP4_IN_820_MEMORY_LIMITED
	if(maxsize>MP4_AUDIO_MAX_BUFFER_SIZE)
		maxsize = MP4_AUDIO_MAX_BUFFER_SIZE;
#endif
	MP4_DEMUX_MSG(("audio stream frame size = %d",maxsize));

	*framesize = (uint32)maxsize;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_audio_max_frame_size(mp4_infor_t *file, uint32 *framesize)
{
	uint32 maxsize, size, frame;
	mp4_stsz_t *stsz;
	
	*framesize =0;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;

	stsz = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);

	if(stsz->sample_size)
		maxsize = stsz->sample_size;
	else
	{
	       mp4_read_stsz_table(file, stsz,1);
	       frame = 0;
	       maxsize = stsz->table[0].size;
	       frame ++;
       	while (frame < stsz->entries_allocated) 
		{
			size = stsz->table[frame].size;
			if (size>maxsize)
				maxsize = size;
			frame++;
		}
		while((stsz->entries_allocated+stsz->start_entries)<stsz->total_entries)
		{
			mp4_read_stsz_table(file, stsz, 0);
			frame =0;
			while (frame < stsz->entries_allocated) 
			{
				size = stsz->table[frame].size;
				if (size>maxsize)
					maxsize = size;
				frame++;
			}
		}
	}
	MP4_DEMUX_MSG(("audio max frame size = %d",maxsize));
#if MP4_IN_820_MEMORY_LIMITED
	if(maxsize>MP4_AUDIO_MAX_BUFFER_SIZE)
	{
		MP4_DEMUX_WARNING(("audio max frame size = %d",maxsize));
		MP4_DEMUX_WARNING(("MP4_AUDIO_MAX_BUFFER_SIZE < %d",maxsize));
	}
#endif
	maxsize = maxsize*MP4_AUDIO_MIN_FRAME_READ;
#if MP4_IN_820_MEMORY_LIMITED
	if(maxsize>MP4_AUDIO_MAX_BUFFER_SIZE)
		maxsize = MP4_AUDIO_MAX_BUFFER_SIZE;
#endif
	MP4_DEMUX_MSG(("audio stream frame size = %d",maxsize));

	*framesize = (uint32)maxsize;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_audio_max_frame_size(mp4_infor_t *file, uint32 *framesize)
{
	uint32 maxsize, size, frame;
	mp4_stsz_t *stsz;
	
	*framesize =0;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;

	stsz = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);

	if(stsz->sample_size)
		maxsize = stsz->sample_size;
	else
	{
//	       mp4_read_stsz_table(file, stsz,1);
	       mp4_read_audio_stsz_table(file, stsz,1);
	       frame = 0;
	       maxsize = stsz->table[0].size;
	       frame ++;
       	while (frame < stsz->entries_allocated) 
		{
			size = stsz->table[frame].size;
			if (size>maxsize)
				maxsize = size;
			frame++;
		}
		while((stsz->entries_allocated+stsz->start_entries)<stsz->total_entries)
		{
//			mp4_read_stsz_table(file, stsz, 0);
			mp4_read_audio_stsz_table(file, stsz, 0);
			frame =0;
			while (frame < stsz->entries_allocated) 
			{
				size = stsz->table[frame].size;
				if (size>maxsize)
					maxsize = size;
				frame++;
			}
		}
	}
	MP4_DEMUX_MSG(("audio max frame size = %d",maxsize));
#if MP4_IN_820_MEMORY_LIMITED
	if(maxsize>MP4_AUDIO_MAX_BUFFER_SIZE)
	{
		MP4_DEMUX_WARNING(("audio max frame size = %d",maxsize));
		MP4_DEMUX_WARNING(("MP4_AUDIO_MAX_BUFFER_SIZE < %d",maxsize));
	}
#endif
	maxsize = maxsize*MP4_AUDIO_MIN_FRAME_READ;
#if MP4_IN_820_MEMORY_LIMITED
	if(maxsize>MP4_AUDIO_MAX_BUFFER_SIZE)
		maxsize = MP4_AUDIO_MAX_BUFFER_SIZE;
#endif
	MP4_DEMUX_MSG(("audio stream frame size = %d",maxsize));

	*framesize = (uint32)maxsize;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif
#endif
/*******************************************************************************************
Description:
	 get audio frame Id size
Parameters:
        file: mp4 infor stuctor point (include file handle)
        frame: frame id(always start on 0 )
Return:	
	 audio frame size
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_audio_frame_size(mp4_infor_t *file, uint32  frame, uint32 *framesize)
{
	uint32 bytes, samples;
	mp4_stsz_t *stsz;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	
	stsz = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);

	if (frame>=stsz->total_entries)
		samples = stsz->total_entries-1;
	else
		samples = frame;
	
	if(stsz->sample_size)
		bytes = stsz->sample_size;
	else
	{
		if (samples<stsz->start_samples)
      			mp4_read_stsz_table(file, stsz,1);
       	while(samples>=stsz->end_samples)
       		mp4_read_stsz_table(file, stsz,0);
      	      	bytes = stsz->table[samples-stsz->start_samples].size;
	}
	*framesize = (uint32)bytes;
	
	if(frame>=stsz->total_entries)
		return MP4_DEMUX_END_OF_AUDIO;
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_audio_frame_size(mp4_infor_t *file, uint32  frame, uint32 *framesize)
{
	uint32 bytes, samples;
	mp4_stsz_t *stsz;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	
	stsz = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);

	if (frame>=stsz->total_entries)
		samples = stsz->total_entries-1;
	else
		samples = frame;
	
	if(stsz->sample_size)
		bytes = stsz->sample_size;
	else
	{
		if (samples<stsz->start_samples)
 //     		mp4_read_stsz_table(file, stsz,1);
 			mp4_read_audio_stsz_table(file, stsz,1);
       	while(samples>=stsz->end_samples)
 //      		mp4_read_stsz_table(file, stsz,0);
 			mp4_read_audio_stsz_table(file, stsz,0);
      	      	bytes = stsz->table[samples-stsz->start_samples].size;
	}
	*framesize = (uint32)bytes;
	
	if(frame>=stsz->total_entries)
		return MP4_DEMUX_END_OF_AUDIO;
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

/*******************************************************************************************
Description:
	get video width
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: file have no video 
	 other: video width
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_video_width(mp4_infor_t *file, uint32  *width)
{
	*width = file->video_vopwidth;
	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	get video height
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: file have no video 
	 other: video height
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_video_height(mp4_infor_t *file, uint32 *height)
{
	*height = file->video_vopheight;
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	get video depth, now I do not know this function use????????
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: file have no video 
	 other: video depth
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_video_depth(mp4_infor_t *file, uint32 *depth)
{
	*depth = 0;
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	*depth =(uint32)file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].depth;
	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	 get video compressor, then can know how to decode
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 NULL: file have no codec information
	 other: video codec name point(length = 4 bytes)
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_video_compressor(mp4_infor_t *file, char** compressor)
{
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;

	if (file->vtracks[DEFAULT_TRACK].track == NULL)
 	 	 (*compressor) = NULL;
	else
  		 (*compressor) = file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].format;
	return MP4_MUX_DEMUX_RET_NOERR;
}
/***********************************************************************************************
  this function modify by Amanda Deng, use for  variable frame rate check, need modify and testing... ... 2006/10/04
************************************************************************************************/
/*******************************************************************************************
Description:
	 get video frame rate(if return 0 then means is variable frame rate)
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: video is variable frame rate
	 other: video frame rate
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
#if (MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_video_frame_rate(mp4_infor_t *file, uint32 *framerate)
{
	mp4_stts_t *stts;
	uint32 num, sameduration=1;
	uint32 duration;
	float ret = 0;

	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	stts = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

	if (stts->total_entries>1)
	{
      		
		 mp4_read_stts_table(file, stts,1);
      		num =0;
		duration = stts->table[num].sample_duration;
		num++;
		while (num < stts->entries_allocated) 
		{
			if (duration != stts->table[num].sample_duration) 
			{
			   if (duration>0)
			   {
				   sameduration = 0;
				   break;
			   }
			}
			duration = stts->table[num].sample_duration;
			num++;
		}
		while((stts->entries_allocated+stts->start_entries)<stts->total_entries)
		{
			mp4_read_stts_table(file, stts, 0);
			num =0;
			while (num < stts->entries_allocated) 
			{
				if (duration != stts->table[num].sample_duration) 
				{
			   			if (duration>0)
			  		 	{
				  		 	sameduration = 0;
				  		 	break;
			   			}
				}
					duration = stts->table[num].sample_duration;
					num++;
			}
			if(!sameduration)
				  break;
		}
	}
	//calculate frame rate
	num =0;
 	if(sameduration) 
  	{
	    	ret = (float)file->vtracks[DEFAULT_TRACK].track->mdia.mdhd.time_scale;
    		if (file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts.table[0].sample_duration == 0)
      			num = 1;
  		 ret /= file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts.table[num].sample_duration;
		*framerate =(uint32)ret;
   	}
 	else
 	{
 		*framerate = 0;
 	}
 	 return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_video_frame_rate(mp4_infor_t *file, uint32 *framerate)
{
	mp4_stts_t *stts;
	uint32 num, sameduration=1;
	uint32 duration;
	float ret = 0;

	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	stts = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

	if (stts->total_entries>1)
	{
      		
//		 mp4_read_stts_table(file, stts,1);
		 mp4_read_video_stts_table(file, stts,1);
      		num =0;
		duration = stts->table[num].sample_duration;
		num++;
		while (num < stts->entries_allocated) 
		{
			if (duration != stts->table[num].sample_duration) 
			{
			   if (duration>0)
			   {
				   sameduration = 0;
				   break;
			   }
			}
			duration = stts->table[num].sample_duration;
			num++;
		}
		while((stts->entries_allocated+stts->start_entries)<stts->total_entries)
		{
//			mp4_read_stts_table(file, stts, 0);
			mp4_read_video_stts_table(file, stts, 0);
			num =0;
			while (num < stts->entries_allocated) 
			{
				if (duration != stts->table[num].sample_duration) 
				{
			   			if (duration>0)
			  		 	{
				  		 	sameduration = 0;
				  		 	break;
			   			}
				}
					duration = stts->table[num].sample_duration;
					num++;
			}
			if(!sameduration)
				  break;
		}
	}
	//calculate frame rate
	num =0;
 	if(sameduration) 
  	{
	    	ret = (float)file->vtracks[DEFAULT_TRACK].track->mdia.mdhd.time_scale;
    		if (file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts.table[0].sample_duration == 0)
      			num = 1;
  		 ret /= file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts.table[num].sample_duration;
		*framerate =(uint32)ret;
   	}
 	else
 	{
 		*framerate = 0;
 	}
 	 return MP4_MUX_DEMUX_RET_NOERR;
}
#endif
/*******************************************************************************************
Description:
	 get video track time scale(use for calculate frame rate)
Parameters:
        file: mp4 infor stuctor point (include file handle)
Return:	
	 0: file have no audio 
	 other: video track time scale
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
uint32 mp4_video_time_scale(mp4_infor_t *file, uint32 *timescale)
{
	uint32 time_scale;
	
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	time_scale = (uint32)file->vtracks[DEFAULT_TRACK].track->mdia.mdhd.time_scale;
	if (time_scale>0)
		*timescale = time_scale;
	else
		*timescale =(uint32)file->moov.mvhd.time_scale;
	
	return MP4_MUX_DEMUX_RET_NOERR;
	
}
/*******************************************************************************************
Description:
	 get video frame Id size
Parameters:
        file: mp4 infor stuctor point (include file handle)
        frame: frame id(always start on 0 )
Return:	
	 video frame size
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.17		create first version for 820 me
********************************************************************************************/
#if MP4_IN_820_QUICK_GET_FRAMESIZE
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_video_max_frame_size(mp4_infor_t *file, uint32  *framesize)
{
	uint32 maxsize, size, frame;
	mp4_stsz_t *stsz;
	mp4_stss_t *stss;
	
	*framesize = 0;
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;

	stsz = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);
	if(stsz->sample_size)
	{
		maxsize = stsz->sample_size;
		MP4_DEMUX_MSG(("video max frame size = %d",maxsize));
#if MP4_IN_820_MEMORY_LIMITED
		if((maxsize+256)>MP4_VIDEO_MAX_BUFFER_SIZE)
		{
			MP4_DEMUX_WARNING(("video max frame size = %d",maxsize));
			MP4_DEMUX_WARNING(("MP4_VIDEO_MAX_BUFFER_SIZE -256 < %d",maxsize));
		}
#endif
		maxsize = maxsize*MP4_VIDEO_MIN_FRAME_READ+256;
#if MP4_IN_820_MEMORY_LIMITED
		if(maxsize>MP4_VIDEO_MAX_BUFFER_SIZE)
			maxsize = MP4_VIDEO_MAX_BUFFER_SIZE;
#endif
		MP4_DEMUX_MSG(("video stream buffer size = %d",maxsize));

		*framesize = (uint32)maxsize;
		return MP4_MUX_DEMUX_RET_NOERR;
	}
	
	maxsize = 0;
	stss = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stss);
	
	if(stss->total_entries<=20)
	{
		// get all frame max size
	       mp4_read_stsz_table(file, stsz,1);
	       frame = 0;
	       maxsize = stsz->table[0].size;
	       frame ++;
       	while (frame < stsz->entries_allocated) 
		{
			size = stsz->table[frame].size;
			if (size>maxsize)
				maxsize = size;
			frame++;
		}
		while((stsz->entries_allocated+stsz->start_entries)<stsz->total_entries)
		{
			mp4_read_stsz_table(file, stsz, 0);
			frame =0;
			while (frame < stsz->entries_allocated) 
			{
				size = stsz->table[frame].size;
				if (size>maxsize)
					maxsize = size;
				frame++;
			}
		}
	}
	else
	{
		//only get I frame max size
		if(stss->start_entries)
			{mp4_read_stss_table(file, stss,1);}
	      	frame = 0;
		while (frame < stss->entries_allocated) 
		{
			mp4_video_frame_size(file,stss->table[frame].sample-1,(uint32*)&size);
			if (size>maxsize)
				maxsize = size;
			frame++;
		}
		
		while((stss->start_entries+stss->entries_allocated)<stss->total_entries)
		{
	       	mp4_read_stss_table(file, stss,0);
	       	frame = 0;
			while (frame < stss->entries_allocated) 
			{
				mp4_video_frame_size(file,stss->table[frame].sample-1,(uint32*)&size);
				if (size>maxsize)
					maxsize = size;
				frame++;
			}
		}
	}
	
	MP4_DEMUX_MSG(("video max frame size = %d",maxsize));
#if MP4_IN_820_MEMORY_LIMITED
	if((maxsize+256)>MP4_VIDEO_MAX_BUFFER_SIZE)
	{
		MP4_DEMUX_WARNING(("video max frame size = %d",maxsize));
		MP4_DEMUX_WARNING(("MP4_VIDEO_MAX_BUFFER_SIZE -256 < %d",maxsize));
	}
#endif
	maxsize = maxsize + maxsize/5;		//(1.2*maxsize )
	maxsize = maxsize*MP4_VIDEO_MIN_FRAME_READ+256;
#if MP4_IN_820_MEMORY_LIMITED
	if(maxsize>MP4_VIDEO_MAX_BUFFER_SIZE)
		maxsize = MP4_VIDEO_MAX_BUFFER_SIZE;
#endif
	MP4_DEMUX_MSG(("video stream buffer size = %d",maxsize));

	*framesize = (uint32)maxsize;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_video_max_frame_size(mp4_infor_t *file, uint32  *framesize)
{
	uint32 maxsize, size, frame;
	mp4_stsz_t *stsz;
	mp4_stss_t *stss;
	
	*framesize = 0;
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;

	stsz = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);
	if(stsz->sample_size)
	{
		maxsize = stsz->sample_size;
		MP4_DEMUX_MSG(("video max frame size = %d",maxsize));
#if MP4_IN_820_MEMORY_LIMITED
		if((maxsize+256)>MP4_VIDEO_MAX_BUFFER_SIZE)
		{
			MP4_DEMUX_ERR(("video max frame size = %d",maxsize));
			MP4_DEMUX_ERR(("MP4_VIDEO_MAX_BUFFER_SIZE -256 < %d",maxsize));
		}
#endif
		maxsize = maxsize*MP4_VIDEO_MIN_FRAME_READ+256;
#if MP4_IN_820_MEMORY_LIMITED
		if(maxsize>MP4_VIDEO_MAX_BUFFER_SIZE)
			maxsize = MP4_VIDEO_MAX_BUFFER_SIZE;
#endif
		MP4_DEMUX_MSG(("video stream buffer size = %d",maxsize));

		*framesize = (uint32)maxsize;
		return MP4_MUX_DEMUX_RET_NOERR;
	}
	//only get I frame max size
	maxsize = 0;
	stss = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stss);
	
	if(stss->total_entries<=20)
	{
		// get all frame max size
	       mp4_read_video_stss_table(file, stsz,1);
	       frame = 0;
	       maxsize = stsz->table[0].size;
	       frame ++;
       	while (frame < stsz->entries_allocated) 
		{
			size = stsz->table[frame].size;
			if (size>maxsize)
				maxsize = size;
			frame++;
		}
		while((stsz->entries_allocated+stsz->start_entries)<stsz->total_entries)
		{
			mp4_read_video_stss_table(file, stsz, 0);
			frame =0;
			while (frame < stsz->entries_allocated) 
			{
				size = stsz->table[frame].size;
				if (size>maxsize)
					maxsize = size;
				frame++;
			}
		}
	}
	else
	{

	if(stss->start_entries)
		{mp4_read_video_stss_table(file, stss,1);}
      	frame = 0;
	while (frame < stss->entries_allocated) 
	{
		mp4_video_frame_size(file,stss->table[frame].sample-1,(uint32*)&size);
		if (size>maxsize)
			maxsize = size;
		frame++;
	}
	
	while((stss->start_entries+stss->entries_allocated)<stss->total_entries)
	{
       	mp4_read_video_stss_table(file, stss,0);
       	frame = 0;
		while (frame < stss->entries_allocated) 
		{
			mp4_video_frame_size(file,stss->table[frame].sample-1,(uint32*)&size);
			if (size>maxsize)
				maxsize = size;
			frame++;
		}
	}
	}
	MP4_DEMUX_MSG(("video max frame size = %d",maxsize));
#if MP4_IN_820_MEMORY_LIMITED
	if((maxsize+256)>MP4_VIDEO_MAX_BUFFER_SIZE)
	{
		MP4_DEMUX_ERR(("video max frame size = %d",maxsize));
		MP4_DEMUX_ERR(("MP4_VIDEO_MAX_BUFFER_SIZE -256 < %d",maxsize));
	}
#endif
	maxsize = maxsize + maxsize/5;		//(1.2*maxsize )
	maxsize = maxsize*MP4_VIDEO_MIN_FRAME_READ+256;
#if MP4_IN_820_MEMORY_LIMITED
	if(maxsize>MP4_VIDEO_MAX_BUFFER_SIZE)
		maxsize = MP4_VIDEO_MAX_BUFFER_SIZE;
#endif
	MP4_DEMUX_MSG(("video stream buffer size = %d",maxsize));

	*framesize = (uint32)maxsize;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif
#else
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_video_max_frame_size(mp4_infor_t *file, uint32  *framesize)
{
	uint32 maxsize, size, frame;
	mp4_stsz_t *stsz;
	
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;

	stsz = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);

	if(stsz->sample_size)
		maxsize = stsz->sample_size;
	else
	{
	       mp4_read_stsz_table(file, stsz,1);
	       frame = 0;
	       maxsize = stsz->table[0].size;
	       frame ++;
       	while (frame < stsz->entries_allocated) 
		{
			size = stsz->table[frame].size;
			if (size>maxsize)
				maxsize = size;
			frame++;
		}
		while((stsz->entries_allocated+stsz->start_entries)<stsz->total_entries)
		{
			mp4_read_stsz_table(file, stsz, 0);
			frame =0;
			while (frame < stsz->entries_allocated) 
			{
				size = stsz->table[frame].size;
				if (size>maxsize)
					maxsize = size;
				frame++;
			}
		}
	}
	MP4_DEMUX_MSG(("video max frame size = %d",maxsize));
#if MP4_IN_820_MEMORY_LIMITED
	if((maxsize+256)>MP4_VIDEO_MAX_BUFFER_SIZE)
	{
		MP4_DEMUX_WARNING(("video max frame size = %d",maxsize));
		MP4_DEMUX_WARNING(("MP4_VIDEO_MAX_BUFFER_SIZE -256 < %d",maxsize));
	}
#endif
	maxsize = maxsize*MP4_VIDEO_MIN_FRAME_READ+256;
#if MP4_IN_820_MEMORY_LIMITED
	if(maxsize>MP4_VIDEO_MAX_BUFFER_SIZE)
		maxsize = MP4_VIDEO_MAX_BUFFER_SIZE;
#endif
	MP4_DEMUX_MSG(("video stream buffer size = %d",maxsize));

	*framesize = (uint32)maxsize;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_video_max_frame_size(mp4_infor_t *file, uint32  *framesize)
{
	uint32 maxsize, size, frame;
	mp4_stsz_t *stsz;
	
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;

	stsz = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);

	if(stsz->sample_size)
		maxsize = stsz->sample_size;
	else
	{
	       mp4_read_video_stsz_table(file, stsz,1);
	       frame = 0;
	       maxsize = stsz->table[0].size;
	       frame ++;
       	while (frame < stsz->entries_allocated) 
		{
			size = stsz->table[frame].size;
			if (size>maxsize)
				maxsize = size;
			frame++;
		}
		while((stsz->entries_allocated+stsz->start_entries)<stsz->total_entries)
		{
			mp4_read_video_stsz_table(file, stsz, 0);
			frame =0;
			while (frame < stsz->entries_allocated) 
			{
				size = stsz->table[frame].size;
				if (size>maxsize)
					maxsize = size;
				frame++;
			}
		}
	}
//	MP4_DEMUX_MSG(("video max frame size = %d",maxsize));
	MP4_DEMUX_ERR(("video max frame size = %d",maxsize));
#if MP4_IN_820_MEMORY_LIMITED
	if((maxsize/*+256*/)>MP4_VIDEO_MAX_BUFFER_SIZE)
	{
		MP4_DEMUX_WARNING(("video max frame size = %d",maxsize));
		MP4_DEMUX_WARNING(("MP4_VIDEO_MAX_BUFFER_SIZE -256 < %d",maxsize));
	}
#endif
	maxsize = maxsize*MP4_VIDEO_MIN_FRAME_READ/*+256*/;
#if MP4_IN_820_MEMORY_LIMITED
	if(maxsize>MP4_VIDEO_MAX_BUFFER_SIZE)
		maxsize = MP4_VIDEO_MAX_BUFFER_SIZE;
#endif
	MP4_DEMUX_MSG(("video stream buffer size = %d",maxsize));

	*framesize = (uint32)maxsize;
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif
#endif
/*******************************************************************************************
Description:
	 get video frame Id size
Parameters:
        file: mp4 infor stuctor point (include file handle)
        frame: frame id(always start on 0 )
Return:	
	 video frame size
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_video_frame_size(mp4_infor_t *file, uint32  frame, uint32  *framesize)
{
	uint32 bytes, samples;
	mp4_stsz_t *stsz;
	
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	stsz = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);

	if (frame>=stsz->total_entries)
		samples = stsz->total_entries-1;
	else
		samples = frame;
	
	if(stsz->sample_size)
		bytes = stsz->sample_size;
	else
	{
	       if (samples<stsz->start_samples)
      			mp4_read_stsz_table(file, stsz,1);
       	while(samples>=stsz->end_samples)
       		mp4_read_stsz_table(file, stsz,0);
      	      	bytes = stsz->table[samples-stsz->start_samples].size;

	}
	*framesize = (uint32)bytes;
	
	if(frame>=stsz->total_entries)
		return MP4_DEMUX_END_OF_VIDEO;
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_video_frame_size(mp4_infor_t *file, uint32  frame, uint32  *framesize)
{
	uint32 bytes, samples;
	mp4_stsz_t *stsz;
	
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	stsz = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);

	if (frame>=stsz->total_entries)
		samples = stsz->total_entries-1;
	else
		samples = frame;
	
	if(stsz->sample_size)
		bytes = stsz->sample_size;
	else
	{
	       if (samples<stsz->start_samples)
   //   		mp4_read_stsz_table(file, stsz,1);
   			mp4_read_video_stsz_table(file, stsz,1);
       	while(samples>=stsz->end_samples)
 //      		mp4_read_stsz_table(file, stsz,0);
 			mp4_read_video_stsz_table(file, stsz,0);
      	      	bytes = stsz->table[samples-stsz->start_samples].size;

	}
	*framesize = (uint32)bytes;
	
	if(frame>=stsz->total_entries)
		return MP4_DEMUX_END_OF_VIDEO;
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

/*******************************************************************************************
Description:
	 get video sample duration (audio is not change always)
Parameters:
        file: mp4 infor stuctor point (include file handle)
        sample: video sample id(always start on 0 )
Return:	
	 0: file have no video 
	 other: video sample duration
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.01.17		modify for 820 me
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_video_sample_duration(mp4_infor_t *file, uint32  sample, uint32 *duration)
{
	mp4_stts_t *stts;
	mp4_stsz_t *stsz;
	uint32 time_scale, result;
	uint32 tsduration, msduration, tmpduration;
	uint32 i = 0;
	uint32 f = 0;
	
	*duration = 0;
	tsduration = 0;
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	result = mp4_video_time_scale(file,&time_scale);
	if (result) 
		return result;
	if(!sample)
		file->last_vduration = 0;

	stsz = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);
	if(sample>=stsz->total_entries)
		return MP4_DEMUX_END_OF_VIDEO;
	
	stts = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

       if (sample<stts->start_samples)
      		mp4_read_stts_table(file, stts,1);
       while((sample>=stts->end_samples)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
       	mp4_read_stts_table(file, stts,0);

	i = 0;
	f = stts->start_samples;
       while (i < stts->entries_allocated) 
	{
		if ((f + stts->table[i].sample_count) > sample) 
		{
			tsduration = (uint32)stts->table[i].sample_duration;
			break;
		}
		else 
		{
			f += stts->table[i].sample_count ;
			i++;
		}
	}
	// transfer duration form time_scale to ms(time_scale==1000)
       if(time_scale%1000)
       {
       	if(time_scale%100)
       	{
       		tmpduration = file->last_vduration + (tsduration*1000);
			msduration = tmpduration/time_scale;
			file->last_vduration = tmpduration%time_scale;
       	}
		else
		{
			tmpduration = file->last_vduration + tsduration*10;
	   		msduration =tmpduration/(time_scale/100);
			file->last_vduration = tmpduration%(time_scale/100);
		}
       }
	else
	{
		tmpduration = file->last_vduration + tsduration;
		msduration =tmpduration/(time_scale/1000);
		file->last_vduration = tmpduration%(time_scale/1000);
	}
	if(!msduration)
		msduration = 50;
	*duration =msduration;
       return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_video_sample_duration(mp4_infor_t *file, uint32  sample, uint32 *duration)
{
	mp4_stts_t *stts;
	mp4_stsz_t *stsz;
	uint32 time_scale, result;
	uint32 tsduration, msduration, tmpduration;
	uint32 i = 0;
	uint32 f = 0;
	
	*duration = 0;
	tsduration = 0;
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	result = mp4_video_time_scale(file,&time_scale);
	if (result) 
		return result;
	if(!sample)
		file->last_vduration = 0;

	stsz = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsz);
	if(sample>=stsz->total_entries)
		return MP4_DEMUX_END_OF_VIDEO;
	
	stts = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

       if (sample<stts->start_samples)
//      	mp4_read_stts_table(file, stts,1);
		mp4_read_video_stts_table(file, stts,1);
       while((sample>=stts->end_samples)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
//       	mp4_read_stts_table(file, stts,0);
       	mp4_read_video_stts_table(file, stts,0);
	   
	i = 0;
	f = stts->start_samples;
       while (i < stts->entries_allocated) 
	{
		if ((f + stts->table[i].sample_count) > sample) 
		{
			tsduration = (uint32)stts->table[i].sample_duration;
			break;
		}
		else 
		{
			f += stts->table[i].sample_count ;
			i++;
		}
	}
	// transfer duration form time_scale to ms(time_scale==1000)
       if(time_scale%1000)
       {
       	if(time_scale%100)
       	{
       		tmpduration = file->last_vduration + (tsduration*1000);
			msduration = tmpduration/time_scale;
			file->last_vduration = tmpduration%time_scale;
       	}
		else
		{
			tmpduration = file->last_vduration + tsduration*10;
	   		msduration =tmpduration/(time_scale/100);
			file->last_vduration = tmpduration%(time_scale/100);
		}
       }
	else
	{
		tmpduration = file->last_vduration + tsduration;
		msduration =tmpduration/(time_scale/1000);
		file->last_vduration = tmpduration%(time_scale/1000);
	}
	if(!msduration)
		msduration = 50;
	*duration =msduration;
       return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

/*******************************************************************************************
Description:
	 judge the video frame is syn frame or not ?
Parameters:
        file: mp4 infor stuctor point (include file handle)
        sample: video frame ID
Return:	
	1: yes, the frame is syn frame
	0, no, the frame is not syn frame
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_is_syn_video_sample(mp4_infor_t *file, uint32  sample)
{
	mp4_stss_t *stss;
	uint32 i=0;
	
	stss = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stss);
	
	if (sample<stss->start_samples)
      		mp4_read_stss_table(file, stss,1);
	while((sample>stss->end_samples)&&(stss->start_entries+stss->entries_allocated<stss->total_entries))
       	mp4_read_stss_table(file, stss,0);
	
	if(sample>stss->end_samples)
		return 3;				//frame index more than last syn video frame
	if(sample==stss->end_samples)
		return 2;				//frame index is last syn video frame
 	while (i < stss->entries_allocated) 
	{
		if (stss->table[i].sample == sample)
			return 1;			//frame index is syn video frame
		i++;
	}
	return 0;
}
#else
uint32 mp4_is_syn_video_sample(mp4_infor_t *file, uint32  sample)
{
	mp4_stss_t *stss;
	uint32 i=0;
	
	stss = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stss);
	
	if (sample<stss->start_samples)
      		mp4_read_video_stss_table(file, stss,1);
	while((sample>stss->end_samples)&&(stss->start_entries+stss->entries_allocated<stss->total_entries))
       	mp4_read_video_stss_table(file, stss,0);
	
	if(sample>stss->end_samples)
		return 3;				//frame index more than last syn video frame
	if(sample==stss->end_samples)
		return 2;				//frame index is last syn video frame
 	while (i < stss->entries_allocated) 
	{
		if (stss->table[i].sample == sample)
			return 1;			//frame index is syn video frame
		i++;
	}
	return 0;
}
#endif

/***********************************************************************************
   the follow add by Amanda Deng, use for seek, need modify and testing... ... 2006/10/04
************************************************************************************/
/*******************************************************************************************
Description:
	 get the video syn sample nearest the sample user want 
Parameters:
        file: mp4 infor stuctor point (include file handle)
        sample: video frame ID
        *syn_sample: nearest syn frame id
Return:	
	MP4_MUX_DEMUX_HAVE_NO_VIDEO
	MP4_MUX_DEMUX_RET_NOERR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_get_syn_video_sample(mp4_infor_t *file, uint32 sample, uint32  *frame)
{
	mp4_stss_t *stss;
	uint32  delt, mixdelt;
	uint32 i=0, imix_index=0;
	
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	stss = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stss);

  	if ((sample+1)<stss->start_samples)
      	mp4_read_stss_table(file, stss,1);
      while((sample>stss->end_samples)&&(stss->start_entries+stss->entries_allocated<stss->total_entries))
       	mp4_read_stss_table(file, stss,0);

  	if (stss->table[i].sample >= sample)
		mixdelt = stss->table[i].sample - sample;
	else
		mixdelt = sample - stss->table[i].sample;
	i++;
       while (i < stss->entries_allocated) 
	{
		if (stss->table[i].sample >= sample)
			delt = stss->table[i].sample - sample;
		else
			delt = sample - stss->table[i].sample;
		
		if (delt<= mixdelt) 
		{
			imix_index = i;	
			mixdelt = delt;
		}
		i++;
	}
       
	file->vtracks[DEFAULT_TRACK].current_position = stss->table[imix_index].sample-1;
	*frame =(uint32) stss->table[imix_index].sample-1;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_get_syn_video_sample(mp4_infor_t *file, uint32 sample, uint32  *frame)
{
	mp4_stss_t *stss;
	uint32  delt, mixdelt;
	uint32 i=0, imix_index=0;
	
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	
	stss = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stss);

  	if ((sample+1)<stss->start_samples)
    	  	mp4_read_video_stss_table(file, stss,1);
      while((sample>stss->end_samples)&&(stss->start_entries+stss->entries_allocated<stss->total_entries))
       	mp4_read_video_stss_table(file, stss,0);

  	if (stss->table[i].sample >= sample)
		mixdelt = stss->table[i].sample - sample;
	else
		mixdelt = sample - stss->table[i].sample;
	i++;
       while (i < stss->entries_allocated) 
	{
		if (stss->table[i].sample >= sample)
			delt = stss->table[i].sample - sample;
		else
			delt = sample - stss->table[i].sample;
		
		if (delt<= mixdelt) 
		{
			imix_index = i;	
			mixdelt = delt;
		}
		i++;
	}
       
	file->vtracks[DEFAULT_TRACK].current_position = stss->table[imix_index].sample-1;
	*frame =(uint32) stss->table[imix_index].sample-1;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

/*******************************************************************************************
Description:
	 get the audio form the video syn frame time
Parameters:
        file: mp4 infor stuctor point (include file handle)
        time: video syn frame start time
        *frame: nearest audio frame id
Return:	
	MP4_MUX_DEMUX_HAVE_NO_VIDEO
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_get_syn_audio_sample(mp4_infor_t *file, uint32  time, uint32  *frame )
{
	mp4_stts_t *stts;
	uint32 i, f, start, deltime, deltframe;
	uint32 time_scale, startduration, result;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	
	result = mp4_audio_time_scale(file,&time_scale);
	if (result) 
		return result;
	
	if(time_scale%1000)
		{startduration =(uint32 )((time/40)*(time_scale/25));}
	else
		{startduration =(uint32 )(time*(time_scale/1000));}
	stts = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);
	
	if (startduration<stts->start_duration)
      		mp4_read_stts_table(file, stts,1);
	while((startduration>=stts->end_duration)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
       	mp4_read_stts_table(file, stts,0);
	
	i = 0;
	f = stts->start_samples;
	start = stts->start_duration;
	
	while (i < stts->entries_allocated) 
	{
		if ((start + stts->table[i].sample_duration* stts->table[i].sample_count) <= startduration) 
		{
			start += stts->table[i].sample_duration* stts->table[i].sample_count;
			f += stts->table[i].sample_count;
			i++;
		}
		else 
		{
			deltime = startduration-start;
			deltframe = (uint32 )(deltime/stts->table[i].sample_duration);
			f += deltframe;
			break;
		}
	}
	*frame = (uint32) f;	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_get_syn_audio_sample(mp4_infor_t *file, uint32  time, uint32  *frame )
{
	mp4_stts_t *stts;
	uint32 i, f, start, deltime, deltframe;
	uint32 time_scale, startduration, result;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	
	result = mp4_audio_time_scale(file,&time_scale);
	if (result) 
		return result;
	
	if(time_scale%1000)
		{startduration =(uint32 )((time/40)*(time_scale/25));}
	else
		{startduration =(uint32 )(time*(time_scale/1000));}
	stts = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);
	
	if (startduration<stts->start_duration)
      		mp4_read_audio_stts_table(file, stts,1);
	while((startduration>=stts->end_duration)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
       	mp4_read_audio_stts_table(file, stts,0);
	
	i = 0;
	f = stts->start_samples;
	start = stts->start_duration;
	
	while (i < stts->entries_allocated) 
	{
		if ((start + stts->table[i].sample_duration* stts->table[i].sample_count) <= startduration) 
		{
			start += stts->table[i].sample_duration* stts->table[i].sample_count;
			f += stts->table[i].sample_count;
			i++;
		}
		else 
		{
			deltime = startduration-start;
			deltframe = (uint32 )(deltime/stts->table[i].sample_duration);
			f += deltframe;
			break;
		}
	}
	*frame = (uint32) f;	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

/*******************************************************************************************
Description:
	 get the time the video syn frame
Parameters:
        file: mp4 infor stuctor point (include file handle)
	 frame: video syn frame id
        *start: video syn frame start time(ms)
Return:	
	MP4_MUX_DEMUX_HAVE_NO_VIDEO
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_get_time_of_video_frame(mp4_infor_t *file, uint32  frame, uint32  *start)
{
	mp4_stts_t *stts;
	uint32 i, f ;
	uint32 time_scale, startduration, result;

	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;

	result = mp4_video_time_scale(file,&time_scale);
	if (result) 
		return result;
	
	stts = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

       if (frame<stts->start_samples)
      		mp4_read_stts_table(file, stts,1);
      while((frame>=stts->end_samples)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
       	mp4_read_stts_table(file, stts,0);

        i = 0;
	 f = stts->start_samples;
	startduration = stts->start_duration;

       while (i < stts->entries_allocated) 
	{
		if (f + stts->table[i].sample_count <= frame)
		{
			startduration += stts->table[i].sample_duration * stts->table[i].sample_count;
			f += stts->table[i].sample_count;
			i++;
		} 
		else 
		{
			startduration += stts->table[i].sample_duration * (frame - f);
			break;
		}
	}
       
       if(time_scale>=100)
       	{*start =(startduration*10/(time_scale/100));}
       else
       	{*start =(startduration*1000)/time_scale;}
       
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_get_time_of_video_frame(mp4_infor_t *file, uint32  frame, uint32  *start)
{
	mp4_stts_t *stts;
	uint32 i, f ;
	uint32 time_scale, startduration, result;

	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;

	result = mp4_video_time_scale(file,&time_scale);
	if (result) 
		return result;
	
	stts = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

       if (frame<stts->start_samples)
      		mp4_read_video_stts_table(file, stts,1);
      while((frame>=stts->end_samples)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
       	mp4_read_video_stts_table(file, stts,0);

        i = 0;
	 f = stts->start_samples;
	startduration = stts->start_duration;

       while (i < stts->entries_allocated) 
	{
		if (f + stts->table[i].sample_count <= frame)
		{
			startduration += stts->table[i].sample_duration * stts->table[i].sample_count;
			f += stts->table[i].sample_count;
			i++;
		} 
		else 
		{
			startduration += stts->table[i].sample_duration * (frame - f);
			break;
		}
	}
       
       if(time_scale>=100)
       	{*start =(startduration*10/(time_scale/100));}
       else
       	{*start =(startduration*1000)/time_scale;}
       
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif

/*******************************************************************************************
Description:
	 get the time the audio frame
Parameters:
        file: mp4 infor stuctor point (include file handle)
	 frame: audio frame id
        *start: audio frame start time
Return:	
	MP4_MUX_DEMUX_HAVE_NO_VIDEO
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.18		create first version
********************************************************************************************/
uint32 mp4_get_time_of_audio_frame(mp4_infor_t *file, uint32  frame, uint32  *start)
{
	mp4_stts_t *stts;
	uint32 i, f;
	uint32 time_scale, startduration, result;

	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	
	result = mp4_audio_time_scale(file,&time_scale);
	if (result) 
		return result;
	
	stts = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

       if (frame<stts->start_samples)
      		mp4_read_stts_table(file, stts,1);
       while((frame>=stts->end_samples)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
       	mp4_read_stts_table(file, stts,0);


        i = 0;
	 f = stts->start_samples;
	startduration = stts->start_duration;

       while (i < stts->entries_allocated) 
	{
		if (f + stts->table[i].sample_count <= frame)
		{
			startduration += stts->table[i].sample_duration * stts->table[i].sample_count;
			f += stts->table[i].sample_count;
			i++;
		} 
		else 
		{
			startduration+= stts->table[i].sample_duration * (frame - f);
			break;
		}
	}

       *start =(uint32 )(startduration/(time_scale/1000));
       	
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	 get the video frame ID according with the time user want
Parameters:
        file: mp4 infor stuctor point (include file handle)
        time: user want time
	 *frame: video  frame id start nearest start time
Return:	
	MP4_MUX_DEMUX_HAVE_NO_VIDEO
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
#if !(MP4LIB_VERSION2_FOREHANDLE)
uint32 mp4_get_video_frame_of_time(mp4_infor_t *file, uint32  time,uint32  *frame)
{
	mp4_stts_t *stts;
	uint32 i,  f, start, deltime, deltframe;
	uint32 time_scale, startduration, result;
	
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;

	result = mp4_video_time_scale(file,&time_scale);
	if (result) 
		return result;
	
	startduration =(uint32 )(time/1000*time_scale);
		
	stts = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

       if (startduration<stts->start_duration)
      		mp4_read_stts_table(file, stts,1);
       while((startduration>=stts->end_duration)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
       	mp4_read_stts_table(file, stts,0);

       if(stts->end_duration==0)
		return MP4_MUX_DEMUX_INVALID_INFOR;

	i = 0;
	f = stts->start_samples;
	start = stts->start_duration;
	
	while (i < stts->entries_allocated) 
	{
		if ((start + stts->table[i].sample_duration* stts->table[i].sample_count) <= startduration) 
		{
			start += stts->table[i].sample_duration* stts->table[i].sample_count;
			f += stts->table[i].sample_count;
			i++;
		}
		else 
		{
			deltime = startduration-start;
			deltframe = (uint32 )(deltime/stts->table[i].sample_duration);
			 f += deltframe;
			break;
		}
	}
	*frame = (uint32)  f;	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
uint32 mp4_get_video_frame_of_time(mp4_infor_t *file, uint32  time,uint32  *frame)
{
	mp4_stts_t *stts;
	uint32 i,  f, start, deltime, deltframe;
	uint32 time_scale, startduration, result;
	
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;

	result = mp4_video_time_scale(file,&time_scale);
	if (result) 
		return result;
	
	startduration =(uint32 )(time/1000*time_scale);
		
	stts = &(file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

       if (startduration<stts->start_duration)
      		mp4_read_video_stts_table(file, stts,1);
       while((startduration>=stts->end_duration)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
       	mp4_read_video_stts_table(file, stts,0);

       if(stts->end_duration==0)
		return MP4_MUX_DEMUX_INVALID_INFOR;

	i = 0;
	f = stts->start_samples;
	start = stts->start_duration;
	
	while (i < stts->entries_allocated) 
	{
		if ((start + stts->table[i].sample_duration* stts->table[i].sample_count) <= startduration) 
		{
			start += stts->table[i].sample_duration* stts->table[i].sample_count;
			f += stts->table[i].sample_count;
			i++;
		}
		else 
		{
			deltime = startduration-start;
			deltframe = (uint32 )(deltime/stts->table[i].sample_duration);
			 f += deltframe;
			break;
		}
	}
	*frame = (uint32)  f;	
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif
/*******************************************************************************************
Description:
	 get the audio frame ID according with the time user want
Parameters:
        file: mp4 infor stuctor point (include file handle)
        time: user want time(ms)
	 *frame: audio  frame id start nearest start time
Return:	
	MP4_MUX_DEMUX_HAVE_NO_VIDEO
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.18		create first version
********************************************************************************************/
uint32 mp4_get_audio_frame_of_time(mp4_infor_t *file, uint32  time,uint32  *frame)
{
	mp4_stts_t *stts;
	uint32 i, f, start, deltime, deltframe;
	uint32 time_scale, startduration, result;
	
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;

	result = mp4_audio_time_scale(file,&time_scale);
	if (result) 
		return result;

	startduration =(uint32 )(time/1000*time_scale);
	
	stts = &(file->atracks[DEFAULT_TRACK].track->mdia.minf.stbl.stts);

       if (startduration<stts->start_duration)
      		mp4_read_stts_table(file, stts,1);
	while((startduration>=stts->end_duration)&&(stts->start_entries+stts->entries_allocated<stts->total_entries))
       	mp4_read_stts_table(file, stts,0);

	i = 0;
	f = stts->start_samples;
	start = stts->start_duration;
	
	while (i < stts->entries_allocated) 
	{
		if ((start + stts->table[i].sample_duration* stts->table[i].sample_count) <= startduration) 
		{
			start += stts->table[i].sample_duration* stts->table[i].sample_count;
			f += stts->table[i].sample_count;
			i++;
		}
		else 
		{
			deltime = startduration-start;
			deltframe = (uint32 )(deltime/stts->table[i].sample_duration);
			 f += deltframe;
			break;
		}
	}
	*frame = (uint32) f;	
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	 random access time of the mp4 file
Parameters:
        file: mp4 infor stuctor point (include file handle)
        time: user want time(ms)
	 *seektime: really access time (ms)
Return:	
	MP4_MUX_DEMUX_HAVE_NO_VIDEO
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_random_access_time(mp4_infor_t *file, uint32  time, uint32  *seektime, uint32 *audioframe, uint32 *videoframe)
{
	uint32  frame;
	uint32  access_time;
	uint32  syn_frame_video, syn_frame_audio;
	uint32  result = 0;
	
	*audioframe = 0;
	*videoframe = 0;
	*seektime = 0;
	
	if((!file->total_atracks)&&(!file->total_vtracks)) 
		return MP4_MUX_DEMUX_INVALID_PARAMETER;

	if(time==0)
	{
		if (file->total_vtracks) 
		{
			file->vtracks[DEFAULT_TRACK].current_position = 0;
			file->reset_video_position = 1;
			file->video_firsttime = 1;
			file->last_vduration = 0;
		}
		if (file->total_atracks) 
		{
			file->atracks[DEFAULT_TRACK].current_position = 0;
			file->reset_audio_position = 1;
		}
		return MP4_MUX_DEMUX_RET_NOERR;
	}

	if (file->total_vtracks) 
	{
		//get video time scale
		result = mp4_get_video_frame_of_time(file, time, &frame);
		if(result)
       		return result;
		result = mp4_get_syn_video_sample(file, frame, &syn_frame_video);
		if(result)
       		return result;
		result = mp4_get_time_of_video_frame(file, syn_frame_video, &access_time);
		if(result)
       		return result;
		file->vtracks[DEFAULT_TRACK].current_position = syn_frame_video;
		file->reset_video_position = 1;
		file->video_firsttime = 1;
		file->last_vduration = 0;
		
		*seektime =  access_time;
		*videoframe =  syn_frame_video;
		
		if (file->total_atracks) 
		{
			result = mp4_get_syn_audio_sample(file, access_time, &syn_frame_audio);
			if(result)
       			return result;
			file->atracks[DEFAULT_TRACK].current_position = syn_frame_audio;
			file->reset_audio_position = 1;
			
			*audioframe = syn_frame_audio;
		}
	}
	else
	{
		if (file->total_atracks) 
		{
			result = mp4_get_syn_audio_sample(file, time, &syn_frame_audio);
			if(result)
       			return result;
			
			file->atracks[DEFAULT_TRACK].current_position = syn_frame_audio;
			file->reset_audio_position = 1;

			*audioframe = syn_frame_audio;
			*seektime = time;
		}
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	 random access time of the mp4 file
Parameters:
        file: mp4 infor stuctor point (include file handle)
        time: user want time(ms)
	 *seektime: really access time (ms)
Return:	
	MP4_MUX_DEMUX_HAVE_NO_VIDEO
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_random_access_syntime(mp4_infor_t *file, uint32  time, uint32 *audioframe, uint32 *videoframe)
{
	uint32  syn_frame_video, syn_frame_audio;
	uint32  result = 0;
	
	*audioframe = 0;
	*videoframe = 0;
	
	if((!file->total_atracks)&&(!file->total_vtracks)) 
		return MP4_MUX_DEMUX_INVALID_PARAMETER;

	if(time==0)
	{
		if (file->total_vtracks) 
		{
			file->vtracks[DEFAULT_TRACK].current_position = 0;
			file->reset_video_position = 1;
			file->video_firsttime = 1;
			file->last_vduration = 0;
		}
		if (file->total_atracks) 
		{
			file->atracks[DEFAULT_TRACK].current_position = 0;
			file->reset_audio_position = 1;
		}
		return MP4_MUX_DEMUX_RET_NOERR;
	}

	if (file->total_vtracks) 
	{
		result = mp4_get_video_frame_of_time(file, time, &syn_frame_video);
		if(result)
       		return result;
		file->vtracks[DEFAULT_TRACK].current_position = syn_frame_video;
		file->reset_video_position = 1;
		file->video_firsttime = 1;
		file->last_vduration = 0;
		*videoframe =  syn_frame_video;
	}
	
	if (file->total_atracks) 
	{
		result = mp4_get_syn_audio_sample(file, time, &syn_frame_audio);
		if(result)
       		return result;
		file->atracks[DEFAULT_TRACK].current_position = syn_frame_audio;
		file->reset_audio_position = 1;
		*audioframe = syn_frame_audio;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	 random access time of the mp4 file
Parameters:
        file: mp4 infor stuctor point (include file handle)
        time: user want time(ms)
	 *seektime: really access time (ms)
Return:	
	MP4_MUX_DEMUX_HAVE_NO_VIDEO
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_random_access_audiotime(mp4_infor_t *file, uint32  time, uint32 *audioframe)
{
	uint32  syn_frame_audio;
	uint32  result = 0;
	
	*audioframe = 0;
	
	if(!file->total_atracks) 
		return MP4_MUX_DEMUX_INVALID_PARAMETER;

	if(time==0)
	{
		file->atracks[DEFAULT_TRACK].current_position = 0;
		file->reset_audio_position = 1;
		return MP4_MUX_DEMUX_RET_NOERR;
	}
	result = mp4_get_syn_audio_sample(file, time, &syn_frame_audio);
	if(result)
       	return result;
	file->atracks[DEFAULT_TRACK].current_position = syn_frame_audio;
	file->reset_audio_position = 1;
	*audioframe = syn_frame_audio;
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	 read current audio frame data, it can set pisition and update current frame index
Parameters:
        file: mp4 infor stuctor point (include file handle)
        audio_buffer: audio buffer start point
        maxBytes: max buffer size
Return:	
	MP4_MUX_DEMUX_MEMORY_LESS_DATA
	MP4_MUX_DEMUX_HAVE_NO_AUDIO
	MP4_DEMUX_END_OF_AUDIO
	MP4_DEMUX_READ_FILE_ERROR
	MP4_DEMUX_LAST_OF_AUDIO
	MP4_MUX_DEMUX_RET_NOERR
	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_audio_frame(mp4_infor_t *file, char *audio_buffer, uint32 maxBytes,  uint32 *readbytes)
{
	uint32  framesize, readsize;
	uint32 result = 0;

	*readbytes = 0;
	//if have audio track?
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	//if read end?
	if(file->atracks[DEFAULT_TRACK].current_position>=file->total_asamples)
		return MP4_DEMUX_END_OF_AUDIO;
	//get audio current sample size
	result = mp4_audio_frame_size(file, file->atracks[DEFAULT_TRACK].current_position, &framesize);
       if(result)
       	return result;
	//if buffer size ok?
      	if (framesize > maxBytes) 
		{return MP4_MUX_DEMUX_MEMORY_LESS_DATA;}
	//set audio sample position
      	result = mp4_set_audio_position(file, file->atracks[DEFAULT_TRACK].current_position);
 	if(result)
       	return result;
 	//read audio sample data
 	MP4_DEMUX_DBG(("audio frame %d adress=%d, framesize=%d",file->atracks[DEFAULT_TRACK].current_position,file->file_position,framesize));
#if !(MP4LIB_VERSION2_FOREHANDLE)
 	readsize = mp4_read_data(file, audio_buffer, framesize);
#else
	readsize = mp4_read_audiodata(file, audio_buffer, framesize,0,0);
#endif
	if(readsize!= framesize)
	{
		MP4_DEMUX_ERR(("framesize=%d",framesize));
		MP4_DEMUX_ERR(("readsize=%d",readsize));
		MP4_DEMUX_ERR(("atracks[DEFAULT_TRACK].current_position=%d",file->atracks[DEFAULT_TRACK].current_position));
		return MP4_DEMUX_READ_FILE_ERROR;
	}
	//return read size
	*readbytes = (uint32)readsize;
	//update audio sample counter
	file->atracks[DEFAULT_TRACK].last_sample_size = (uint32)framesize;
	file->atracks[DEFAULT_TRACK].current_position++; 
	//check return value
	if(file->atracks[DEFAULT_TRACK].current_position>=file->total_asamples)
		return MP4_DEMUX_LAST_OF_AUDIO;
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}

#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_audio_frame_ver2(mp4_infor_t *file, audio_buffer_t *audio_buffer)
{
	uint32  buffer_address, buffer_size;
//	uint32  framecount, readbytes;
//	uint32  datalength=0;
	uint32  result = MP4_MUX_DEMUX_RET_NOERR;

	uint32  framesize;
	uint32 sector_size=0;
	uint32 frame_sector_offset=0/*, frame_sector_offset_4x=0*/;
	uint32 real_toreadbytes=0;
	uint32 start_offset=0;
	uint32 readoffset, readsize;
	
	//if have audio track?
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	//if read end?
	if(file->atracks[DEFAULT_TRACK].current_position>=file->total_asamples)
		return MP4_DEMUX_END_OF_AUDIO;
	//get buffer address and buffer size
	buffer_address = audio_buffer->buffer_address+audio_buffer->cursor_write;
	start_offset = (buffer_address%4==0)?0:(4-buffer_address%4);
	buffer_address +=start_offset;// 4x
	buffer_size = audio_buffer->buffer_size-audio_buffer->cursor_write-start_offset;

	//get video current sample size
	result = mp4_audio_frame_size(file, file->atracks[DEFAULT_TRACK].current_position, &framesize);
       if(result)
       	return result;
	//set video sample position
	 result = mp4_set_audio_position(file, file->atracks[DEFAULT_TRACK].current_position);
 	if(result)
       	return result;
	frame_sector_offset = file->get_offset_sector_func(file->stream_audiodata, &sector_size);//offset on sector
	//if buffer size ok?
	//real_toreadbytes = (framesize+frame_sector_offset+511)&0xfffffe00;//512x
	real_toreadbytes = (framesize+frame_sector_offset+(sector_size-1))&(0xffffffff-sector_size+1);//512x or 4x
	if(real_toreadbytes> buffer_size)
		return MP4_MUX_DEMUX_MEMORY_LESS_DATA;
	//read video sample data
 	//MP4_DEMUX_ERR(("video frame %d adress=%d, framesize=%d",file->vtracks[DEFAULT_TRACK].current_position,file->file_position,framesize));
	readsize = mp4_read_audiodata(file, (char*)buffer_address, framesize, 0, 0);
	if(readsize!= framesize)
	{	//read current video frame data error
		MP4_DEMUX_ERR(("framesize=%d",framesize));
		MP4_DEMUX_ERR(("readsize=%d",readsize));
		MP4_DEMUX_ERR(("vtracks[DEFAULT_TRACK].current_position=%d",file->vtracks[DEFAULT_TRACK].current_position));
		return MP4_DEMUX_READ_FILE_ERROR;
	}
//	readsize+=frame_sector_offset%4;
//	frame_sector_offset_4x = frame_sector_offset&0xfffffffc;// 4x
//	readoffset = frame_sector_offset_4x;
	readoffset = frame_sector_offset;//audio do not need 4x startaddress
	//return read size
	audio_buffer->data_length = readsize;
	audio_buffer->cursor_read = audio_buffer->cursor_write+start_offset+readoffset;
	audio_buffer->cursor_write+=start_offset+readoffset+readsize;	
	audio_buffer->frame_count = 1;

	//update audio sample counter
	file->atracks[DEFAULT_TRACK].last_sample_size = (uint32)framesize;
	file->atracks[DEFAULT_TRACK].current_position++; 
	//check return value
	if(file->atracks[DEFAULT_TRACK].current_position>=file->total_asamples)
		return MP4_DEMUX_LAST_OF_AUDIO;
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}
//read data interface for 848 filesystem
uint32 mp4_read_audio_frame_ver3(mp4_infor_t *file, audio_buffer_t *audio_buffer, uint32 reg, uint32 bit)
{
	uint32  buffer_address, buffer_size;
//	uint32  framecount, readbytes;
//	uint32  datalength=0;
	uint32  result = MP4_MUX_DEMUX_RET_NOERR;

	uint32  framesize;
	uint32 sector_size=0;
	uint32 frame_sector_offset=0/*, frame_sector_offset_4x=0*/;
	uint32 real_toreadbytes=0;
	uint32 start_offset=0;
	uint32 readoffset, readsize;
	
	//if have audio track?
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	//if read end?
	if(file->atracks[DEFAULT_TRACK].current_position>=file->total_asamples)
		return MP4_DEMUX_END_OF_AUDIO;
	//get buffer address and buffer size
	buffer_address = audio_buffer->buffer_address+audio_buffer->cursor_write;
	start_offset = (buffer_address%4==0)?0:(4-buffer_address%4);
	buffer_address +=start_offset;// 4x
	buffer_size = audio_buffer->buffer_size-audio_buffer->cursor_write-start_offset;

	//get video current sample size
	result = mp4_audio_frame_size(file, file->atracks[DEFAULT_TRACK].current_position, &framesize);
       if(result)
       	return result;
	//set video sample position
	 result = mp4_set_audio_position(file, file->atracks[DEFAULT_TRACK].current_position);
 	if(result)
       	return result;
	frame_sector_offset = file->get_offset_sector_func(file->stream_audiodata, &sector_size);//offset on sector
	//if buffer size ok?
	//real_toreadbytes = (framesize+frame_sector_offset+511)&0xfffffe00;//512x
	real_toreadbytes = (framesize+frame_sector_offset+(sector_size-1))&(0xffffffff-sector_size+1);//512x or 4x
	if(real_toreadbytes> buffer_size)
		return MP4_MUX_DEMUX_MEMORY_LESS_DATA;
	//read video sample data
 	//MP4_DEMUX_ERR(("video frame %d adress=%d, framesize=%d",file->vtracks[DEFAULT_TRACK].current_position,file->file_position,framesize));
	readsize = mp4_read_audiodata(file, (char*)buffer_address, framesize, reg, bit);
	if(readsize!= framesize)
	{	//read current video frame data error
		MP4_DEMUX_ERR(("framesize=%d",framesize));
		MP4_DEMUX_ERR(("readsize=%d",readsize));
		MP4_DEMUX_ERR(("vtracks[DEFAULT_TRACK].current_position=%d",file->vtracks[DEFAULT_TRACK].current_position));
		return MP4_DEMUX_READ_FILE_ERROR;
	}
//	readsize+=frame_sector_offset%4;
//	frame_sector_offset_4x = frame_sector_offset&0xfffffffc;// 4x
//	readoffset = frame_sector_offset_4x;
	readoffset = frame_sector_offset;//audio do not need 4x startaddress
	//return read size
	audio_buffer->data_length = readsize;
	audio_buffer->cursor_read = audio_buffer->cursor_write+start_offset+readoffset;
	audio_buffer->cursor_write+=start_offset+readoffset+readsize;	
	audio_buffer->frame_count = 1;

	//update audio sample counter
	file->atracks[DEFAULT_TRACK].last_sample_size = (uint32)framesize;
	file->atracks[DEFAULT_TRACK].current_position++; 
	//check return value
	if(file->atracks[DEFAULT_TRACK].current_position>=file->total_asamples)
		return MP4_DEMUX_LAST_OF_AUDIO;
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}
//called after filesys has transfered data on SD to 848core.
uint32 mp4_read_audio_frame_ver3_post(mp4_infor_t *file, audio_buffer_t *audio_buffer)
{
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif
/*******************************************************************************************
Description:
	 read current video frame data, it can set pisition and update current frame index
Parameters:
        file: mp4 infor stuctor point (include file handle)
        video_buffer: video buffer start point
        maxBytes: max buffer size
Return:	
	MP4_MUX_DEMUX_MEMORY_LESS_DATA
	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_video_frame(mp4_infor_t *file, char *video_buffer, uint32 maxBytes,  uint32 *readbytes)
{
	mp4_esds_t* esds;
	
	uint32  framesize, readsize;
	uint32  buffer_address, configlength=0;
	uint32  result;
	
	*readbytes = 0;
	//if have video track?
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	//if read end?
	if(file->vtracks[DEFAULT_TRACK].current_position>=file->total_vsamples)
		return MP4_DEMUX_END_OF_VIDEO;
	// initialize buffer address
	buffer_address = (uint32)video_buffer;
	//add this for vimicro chip by Amanda.Deng 2007.03.07
	if(file->video_firsttime)
	{//if is 820 hardware decode video,need write mp4v decode confige information at first
		file->video_firsttime = 0;
		if (file->video_type == MP4_TYPE_VIDEO_IS_MP4V)
		{
			esds = &file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds;
			configlength= esds->decoderConfigLen;
//by dnn 2008/4/11
#if !(MP4LIB_VERSION2_FOREHANDLE)
			mp4_memcopy((char*)buffer_address, (char*)esds->decoderConfig, configlength);
#else
			MP4_MEMCPY_H2D((char*)buffer_address, (char*)esds->decoderConfig, configlength);
#endif
			buffer_address += configlength;
		}
	}

	//get video current sample size
	result = mp4_video_frame_size(file, file->vtracks[DEFAULT_TRACK].current_position, &framesize);
	if(result)
		return result;
	//if buffer size ok?
	if (framesize >= maxBytes) 
		return MP4_MUX_DEMUX_MEMORY_LESS_DATA;
	//set video sample position
	result =mp4_set_video_position(file, file->vtracks[DEFAULT_TRACK].current_position);
	 if(result)
       	return result;
	//read video sample data
 	//MP4_DEMUX_ERR(("video frame %d adress=%d, framesize=%d",file->vtracks[DEFAULT_TRACK].current_position,file->file_position,framesize));
	readsize = mp4_read_data(file, (char*)buffer_address, framesize);
	if(readsize!= framesize)
	{	//read current video frame data error
		MP4_DEMUX_ERR(("framesize=%d",framesize));
		MP4_DEMUX_ERR(("readsize=%d",readsize));
		MP4_DEMUX_ERR(("vtracks[DEFAULT_TRACK].current_position=%d",file->vtracks[DEFAULT_TRACK].current_position));
		return MP4_DEMUX_READ_FILE_ERROR;
	}
#if MP4_IN_820_DECODE_HARDWARE	
	if (file->video_type == MP4_TYPE_VIDEO_IS_MP4V)
	{
		mp4_analysize_mp4v_streamheader((unsigned char *)buffer_address, &readsize, &file->mp4v_vop_info);
	}
	else if (file->video_type == MP4_TYPE_VIDEO_IS_S263)
	{
		mp4_analysize_h263_streamheader((unsigned char *)buffer_address, &readsize, &file->h263_vop_info);
	}
#endif
	//return read size
	*readbytes = (uint32)readsize+configlength;	
	//update audio sample counter
	file->vtracks[DEFAULT_TRACK].last_sample_size = framesize;
	file->vtracks[DEFAULT_TRACK].current_position++;
	//check return value
	if(file->vtracks[DEFAULT_TRACK].current_position>=file->total_vsamples)
	{
		file->video_firsttime = 1;
		return MP4_DEMUX_LAST_OF_VIDEO;
	}
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}

#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_video_frame_ver2(mp4_infor_t *file, video_buffer_t *video_buffer)
{
	uint32  buffer_address, buffer_size;
//	uint32  framecount, readbytes;
//	uint32  datalength=0;
	uint32  result = MP4_MUX_DEMUX_RET_NOERR;

	mp4_esds_t* esds;
	uint32 configlength=0;
	uint32 configlen_4x=0;
	uint32  framesize;
	uint32 flg_readconf=0;
	uint32 sector_size=0;
	uint32 frame_sector_offset=0, frame_sector_offset_4x=0;
	uint32 real_toreadbytes=0;
	uint32 start_offset=0;
	uint32 readoffset, readsize;
	uint32 synflag, frameduration;
	
	//if have audio track?
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	//if read end?
	if(file->vtracks[DEFAULT_TRACK].current_position>=file->total_vsamples)
		return MP4_DEMUX_END_OF_VIDEO;
	//get buffer address and buffer size
	buffer_address = video_buffer->buffer_address+video_buffer->cursor_write;
	start_offset = (buffer_address%4==0)?0:(4-buffer_address%4);
	buffer_address +=start_offset;// 4x
	buffer_size = video_buffer->buffer_size-video_buffer->cursor_write-start_offset;

	if(file->video_firsttime && file->video_type == MP4_TYPE_VIDEO_IS_MP4V) {
		esds = &file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds;
		configlength= esds->decoderConfigLen;
		configlen_4x = configlength;
		if(configlength%4!=0) {
			configlen_4x = (configlength/4+1)*4;// 4x
		}
		buffer_address += configlen_4x;
		flg_readconf = 1;
	}
	//get video current sample size
	result = mp4_video_frame_size(file, file->vtracks[DEFAULT_TRACK].current_position, &framesize);
	if(result)
		return result;
	//set video sample position
	result =mp4_set_video_position(file, file->vtracks[DEFAULT_TRACK].current_position);
	 if(result)
       	return result;
	frame_sector_offset = file->get_offset_sector_func(file->stream_videodata, &sector_size);//offset on sector
	//if buffer size ok?
	//real_toreadbytes = (framesize+frame_sector_offset+511)&0xfffffe00;//512x
	real_toreadbytes = (framesize+frame_sector_offset+(sector_size-1))&(0xffffffff-sector_size+1);//512x or 4x
	real_toreadbytes += configlen_4x;
	if(real_toreadbytes> buffer_size)
		return MP4_MUX_DEMUX_MEMORY_LESS_DATA;
	//read video sample data
 	//MP4_DEMUX_ERR(("video frame %d adress=%d, framesize=%d",file->vtracks[DEFAULT_TRACK].current_position,file->file_position,framesize));
	readsize = mp4_read_videodata(file, (char*)buffer_address, framesize, 0, 0);
	if(readsize!= framesize)
	{	//read current video frame data error
		MP4_DEMUX_ERR(("framesize=%d",framesize));
		MP4_DEMUX_ERR(("readsize=%d",readsize));
		MP4_DEMUX_ERR(("vtracks[DEFAULT_TRACK].current_position=%d",file->vtracks[DEFAULT_TRACK].current_position));
		return MP4_DEMUX_READ_FILE_ERROR;
	}
	//fill 0xff for 4x alimengent
	if(frame_sector_offset%4!=0) {
		MP4_MEMSET_H2D(buffer_address+frame_sector_offset&0xfffffffc,frame_sector_offset%4);
	}
	readsize+=frame_sector_offset%4;
	frame_sector_offset_4x = frame_sector_offset&0xfffffffc;// 4x
	readoffset = frame_sector_offset_4x;
	if(flg_readconf) {
		buffer_address = buffer_address-(configlen_4x-frame_sector_offset_4x);
		result = MP4_MEMCPY_H2D((char*)buffer_address, (char*)esds->decoderConfig, configlen_4x);
		if(result!=configlen_4x) {
			MP4_DEMUX_ERR(("len=%d",configlen_4x));
			MP4_DEMUX_ERR(("MP4_MEMCPY_H2D=%d",result));
			return MP4_DEMUX_READ_FILE_ERROR;
		}
		configlength = configlen_4x;
		readsize += configlen_4x;
		file->video_firsttime = 0;
	}
	result = mp4_video_sample_duration(file,file->vtracks[DEFAULT_TRACK].current_position, (uint32*)&frameduration);
	if(result) {
		MP4_DEMUX_ERR(("mp4_video_sample_duration = 0x%08x",result));
		return MP4_DEMUX_READ_FILE_ERROR;
	}
	synflag = mp4_is_syn_video_sample(file, file->vtracks[DEFAULT_TRACK].current_position+1);
	//return read size
	video_buffer->data_length = readsize;
	video_buffer->cursor_read = video_buffer->cursor_write+start_offset+readoffset;
	video_buffer->cursor_write+=start_offset+readoffset+readsize;	
	video_buffer->frame_size[0]= readsize;
	video_buffer->frame_duration[0]= frameduration;
	video_buffer->frame_synflag[0]= synflag;
	video_buffer->frame_count = 1;

	//update audio sample counter
	file->vtracks[DEFAULT_TRACK].last_sample_size = framesize;
	file->vtracks[DEFAULT_TRACK].current_position++;
	//check return value
	if(file->vtracks[DEFAULT_TRACK].current_position>=file->total_vsamples)
	{
		file->video_firsttime = 1;
		return MP4_DEMUX_LAST_OF_VIDEO;
	}
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}
//read data interface for 848 filesystem
uint32 mp4_read_video_frame_ver3(mp4_infor_t *file, video_buffer_t *video_buffer, uint32 reg, uint32 bit)
{
	uint32  buffer_address, buffer_size;
//	uint32  framecount, readbytes;
//	uint32  datalength=0;
	uint32  result = 0;

	mp4_esds_t* esds;
	uint32 configlength=0;
	uint32 configlen_4x=0;
	uint32  framesize;
	uint32 flg_readconf=0;
	uint32 sector_size=0;
	uint32 frame_sector_offset=0, frame_sector_offset_4x=0;
	uint32 real_toreadbytes=0;
	uint32 start_offset=0;
	uint32 readoffset=0,readsize=0;
	uint32 synflag, frameduration;
	
	//if have audio track?
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	//if read end?
	if(file->vtracks[DEFAULT_TRACK].current_position>=file->total_vsamples)
		return MP4_DEMUX_END_OF_VIDEO;
	//get buffer address and buffer size
	buffer_address = video_buffer->buffer_address+video_buffer->cursor_write;
	start_offset = (buffer_address%4==0)?0:(4-buffer_address%4);
	buffer_address +=start_offset;// 4x
	buffer_size = video_buffer->buffer_size-video_buffer->cursor_write-start_offset;

	if(file->video_firsttime && file->video_type == MP4_TYPE_VIDEO_IS_MP4V) {
		esds = &file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds;
		configlength= esds->decoderConfigLen;
		configlen_4x = configlength;
		if(configlength%4!=0) {
			configlen_4x = (configlength+3)&0xfffffffc;// 4x
		}
		buffer_address += configlen_4x;
		flg_readconf = 1;
	}
	//get video current sample size
	result = mp4_video_frame_size(file, file->vtracks[DEFAULT_TRACK].current_position, &framesize);
	if(result)
		return result;
	//set video sample position
	result =mp4_set_video_position(file, file->vtracks[DEFAULT_TRACK].current_position);
	 if(result)
       	return result;
	frame_sector_offset = file->get_offset_sector_func(file->stream_videodata, &sector_size);//offset on sector
	//if buffer size ok?
	//real_toreadbytes = (framesize+frame_sector_offset+511)&0xfffffe00;//512x
	real_toreadbytes = (framesize+frame_sector_offset+(sector_size-1))&(0xffffffff-sector_size+1);//512x or 4x
	real_toreadbytes += configlen_4x;
	if(real_toreadbytes> buffer_size)
		return MP4_MUX_DEMUX_MEMORY_LESS_DATA;
	//read video sample data
 	//MP4_DEMUX_ERR(("video frame %d adress=%d, framesize=%d",file->vtracks[DEFAULT_TRACK].current_position,file->file_position,framesize));
	readsize = mp4_read_videodata(file, (char*)buffer_address, framesize, reg, bit);
	if(readsize!= framesize)
	{	//read current video frame data error
		MP4_DEMUX_ERR(("framesize=%d",framesize));
		MP4_DEMUX_ERR(("readsize=%d",readsize));
		MP4_DEMUX_ERR(("vtracks[DEFAULT_TRACK].current_position=%d",file->vtracks[DEFAULT_TRACK].current_position));
		return MP4_DEMUX_READ_FILE_ERROR;
	}
	result = mp4_video_sample_duration(file,file->vtracks[DEFAULT_TRACK].current_position, (uint32*)&frameduration);
	if(result) {
		MP4_DEMUX_ERR(("mp4_video_sample_duration = 0x%08x",result));
		return MP4_DEMUX_READ_FILE_ERROR;
	}
//	frameduration *=2;
	synflag = mp4_is_syn_video_sample(file, file->vtracks[DEFAULT_TRACK].current_position+1);
#if 0	
	//fill 0xff for 4x alimengent
	if(frame_sector_offset%4!=0) {
		MP4_MEMSET_H2D(buffer_address+frame_sector_offset&0xfffffffc,frame_sector_offset%4);
	}
	readsize+=frame_sector_offset%4;
	frame_sector_offset_4x = frame_sector_offset&0xfffffffc;// 4x
	readoffset = frame_sector_offset_4x;
	if(flg_readconf) {
		buffer_address = buffer_address-(configlen_4x-frame_sector_offset_4x);
		result = MP4_MEMCPY_H2D((char*)buffer_address, (char*)esds->decoderConfig, configlen_4x);
		if(result!=configlen_4x) {
			MP4_DEMUX_ERR(("len=%d",configlen_4x));
			MP4_DEMUX_ERR(("MP4_MEMCPY_H2D=%d",result));
			return MP4_DEMUX_READ_FILE_ERROR;
		}
		configlength = configlen_4x;
		readsize += configlen_4x;
		file->video_firsttime = 0;
	}
	//return read size
	video_buffer->data_length = readsize;
	video_buffer->cursor_read = video_buffer->cursor_write+start_offset+readoffset;
	video_buffer->cursor_write+=start_offset+readoffset+readsize;	
	video_buffer->frame_size[0]= readsize;
#else
	//no need to fill 0xff to invalid data, because it has done by filesystem.
	readsize+=frame_sector_offset%4;
	frame_sector_offset_4x = frame_sector_offset&0xfffffffc;// 4x
	readoffset = frame_sector_offset_4x;
	video_buffer->data_length = readsize;
	video_buffer->cursor_read = video_buffer->cursor_write+start_offset+readoffset+configlen_4x;
	video_buffer->cursor_write+=start_offset+readoffset+readsize+configlen_4x;	
	video_buffer->frame_size[0]= readsize;
#endif
	video_buffer->frame_duration[0]= frameduration;
	video_buffer->frame_synflag[0]= synflag;
	video_buffer->frame_count = 1;
	
	//update audio sample counter
	file->vtracks[DEFAULT_TRACK].last_sample_size = framesize;
	file->vtracks[DEFAULT_TRACK].current_position++;
	//check return value
	if(file->vtracks[DEFAULT_TRACK].current_position>=file->total_vsamples)
	{
//		file->video_firsttime = 1;
		return MP4_DEMUX_LAST_OF_VIDEO;
	}
	else
		return MP4_MUX_DEMUX_RET_NOERR;
}
//called after filesys has transfered data on SD to 848core.
uint32 mp4_read_video_frame_ver3_post(mp4_infor_t *file, video_buffer_t *video_buffer)
{
	uint32  result = 0;
	uint32  buffer_address;
	mp4_esds_t* esds;
	uint32 configlength=0;
	uint32 configlen_4x=0;
//	uint32 eofaddr=0;
//	uint32 eofaddr_4x = 0;

	buffer_address = video_buffer->buffer_address+video_buffer->cursor_read;
//	eofaddr = buffer_address+video_buffer->data_length;
//	eofaddr_4x = eofaddr%4;
//	if(eofaddr_4x) {
		//fill 0xff at the end of valid data
//		MP4_MEMSET_H2D(eofaddr,4-eofaddr_4x);
//	}
	if(file->video_firsttime && file->video_type == MP4_TYPE_VIDEO_IS_MP4V) {
		esds = &file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds;
		configlength= esds->decoderConfigLen;
		configlen_4x = configlength;
		if(configlength%4!=0) {
			configlen_4x = (configlength+3)&0xfffffffc;// 4x
		}

		buffer_address -= configlen_4x;
		result = MP4_MEMCPY_H2D((char*)buffer_address, (char*)esds->decoderConfig, configlen_4x);
		if(result!=configlen_4x) {
			MP4_DEMUX_ERR(("len=%d",configlen_4x));
			MP4_DEMUX_ERR(("MP4_MEMCPY_H2D=%d",result));
			return MP4_DEMUX_READ_FILE_ERROR;
		}
		video_buffer->data_length += configlen_4x;
		video_buffer->cursor_read -= configlen_4x;
		video_buffer->frame_size[0] += configlen_4x;
		file->video_firsttime = 0;
	}
	if(file->vtracks[DEFAULT_TRACK].current_position>=file->total_vsamples)
	{
		file->video_firsttime = 1;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif
/*******************************************************************************************
Description:
	 write one frame audio data to mux file, it can set pisition and update all tables
Parameters:
        file: mp4 infor stuctor point (include file handle)
        audio_buffer: audio buffer start point
        bytes: the frame audio data length
        duration: the frame duration
Return:	
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_FILE_POINT_OVERFLOW
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_audio_frame(mp4_infor_t *file, char *audio_buffer, uint32  bytes, uint32  duration)
{
	uint32  offset = mp4_get_file_position(file);
	uint32 result;

	/* Defeat 32 bit file size limit. */
	if(mp4_test_position(file)) 
		return MP4_MUX_DEMUX_FILE_POINT_OVERFLOW;
	/* write data to file*/
	result = mp4_write_data(file, audio_buffer, bytes);
	if(result!=bytes)
		{return MP4_MUX_WRITE_FILE_ERROR;}
	/*update table*/
	mp4_update_tables(file,
					     file->atracks[DEFAULT_TRACK].track,
					     offset,
					     file->atracks[DEFAULT_TRACK].current_chunk,
					     file->atracks[DEFAULT_TRACK].current_position,
					     (uint32)bytes,
					     (uint32)duration,
					     0);

	file->atracks[DEFAULT_TRACK].current_position += 1;
	file->atracks[DEFAULT_TRACK].current_chunk++;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	 write one frame video data to mux file, it can set pisition and update all tables
Parameters:
        file: mp4 infor stuctor point (include file handle)
        video_buffer: video buffer start point
        bytes: the frame audio data length
        isKeyFrame: =1 the frame is syn frame, =0 the frame is not syn frame
        duration: the frame duration
Return:	
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_FILE_POINT_OVERFLOW
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_video_frame(mp4_infor_t *file, 
								char *video_buffer, 
								uint32  bytes, 
								uint32  isKeyFrame,
								uint32  duration)
{
	uint32  offset = mp4_get_file_position(file);
	uint32 result;

	/* Defeat 32 bit file size limit. */
	if(mp4_test_position(file)) 
		return MP4_MUX_DEMUX_FILE_POINT_OVERFLOW;
	/* write data to file*/
	result = mp4_write_data(file, video_buffer, bytes);
	if(result!=bytes)
	{return MP4_MUX_WRITE_FILE_ERROR;}
	/*update table*/
	mp4_update_tables(file,
					     file->vtracks[DEFAULT_TRACK].track,
					     offset,
					     file->vtracks[DEFAULT_TRACK].current_chunk,
					     file->vtracks[DEFAULT_TRACK].current_position,
					     (uint32)bytes,
					     (uint32)duration,
					     (uint32)isKeyFrame);
	file->vtracks[DEFAULT_TRACK].current_position += 1;
	file->vtracks[DEFAULT_TRACK].current_chunk++;
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	 read audio data(one or more frames), it can set pisition and update current frame index
Parameters:
        file: mp4 infor stuctor point (include file handle)
        audio_buffer_t *audio_buffer
Return:	
	MP4_MUX_DEMUX_MEMORY_LESS_DATA
	MP4_DEMUX_LAST_OF_AUDIO
	MP4_MUX_DEMUX_HAVE_NO_AUDIO
	MP4_MUX_DEMUX_RET_NOERR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.15		create first version
********************************************************************************************/
uint32 mp4_read_audio_frames(mp4_infor_t *file, audio_buffer_t *audio_buffer)
{
	uint32  buffer_address, buffer_size;
	uint32  framecount, readbytes;
	uint32  datalength=0;
	uint32  result = MP4_MUX_DEMUX_RET_NOERR;
	
	audio_buffer->frame_count = 0;
	audio_buffer->data_length = 0;
	//if have audio track?
	if (!file->total_atracks) 
		return MP4_MUX_DEMUX_HAVE_NO_AUDIO;
	//if read end?
	if(file->atracks[DEFAULT_TRACK].current_position>=file->total_asamples)
		return MP4_DEMUX_END_OF_AUDIO;
	//get buffer address and buffer size
	buffer_address = audio_buffer->buffer_address;
	buffer_size = audio_buffer->buffer_size;
	//read frames(frame>=1) audio data
	framecount = MP4_AUDIO_MAX_FRAME_READ;
	while((!result)&&(framecount))
	{
		result = mp4_read_audio_frame(file, (char*)buffer_address, buffer_size, &readbytes);
       	if((!result) |(result==MP4_DEMUX_LAST_OF_AUDIO))
       	{
			//update buffer address and size
			datalength 	  += readbytes;
			buffer_address +=  readbytes;
			buffer_size 	  -=  readbytes;
			//update frame count
			framecount --;
		}
     	};
	//update audio stream buffer information
	audio_buffer->frame_count = MP4_AUDIO_MAX_FRAME_READ-framecount;
	audio_buffer->data_length  = datalength;
	//check return value
	if((result==MP4_MUX_DEMUX_MEMORY_LESS_DATA)&&(datalength))
		return MP4_MUX_DEMUX_RET_NOERR;
	return result;
}

/*******************************************************************************************
Description:
	 read video data(one or more frames), it can set pisition and update current frame index
Parameters:
        file: mp4 infor stuctor point (include file handle)
        audio_buffer_t *audio_buffer
Return:	
	MP4_MUX_DEMUX_MEMORY_LESS_DATA
	MP4_DEMUX_LAST_OF_AUDIO
	MP4_MUX_DEMUX_HAVE_NO_AUDIO
	MP4_MUX_DEMUX_RET_NOERR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.15		create first version
********************************************************************************************/
uint32 mp4_read_video_frames(mp4_infor_t *file, video_buffer_t *video_buffer)
{
	uint32  buffer_address, buffer_size;
	uint32  framecount, frameduration, readbytes;
	uint32  synflag, datalength=0;
	uint32  result = MP4_MUX_DEMUX_RET_NOERR;
	
	video_buffer->frame_count = 0;
	video_buffer->data_length = 0;
	//if have audio track?
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	//if read end?
	if(file->vtracks[DEFAULT_TRACK].current_position>=file->total_vsamples)
		return MP4_DEMUX_END_OF_VIDEO;
	//get buffer address and buffer size
	buffer_address = video_buffer->buffer_address;
	buffer_size = video_buffer->buffer_size;
	//read frames(frame>=1) video data
	framecount = MP4_VIDEO_MAX_FRAME_READ;
	while((!result)&&(framecount))
	{
		result = mp4_video_sample_duration(file,file->vtracks[DEFAULT_TRACK].current_position, (uint32*)&frameduration);
		if(!result)
		{
			synflag = mp4_is_syn_video_sample(file, file->vtracks[DEFAULT_TRACK].current_position+1);
			result = mp4_read_video_frame(file, (char*)buffer_address, buffer_size, &readbytes);
       		if((!result) |(result==MP4_DEMUX_LAST_OF_VIDEO))
       		{
				//update buffer address and size
				datalength 	  += readbytes;
				buffer_address +=  readbytes;
				buffer_size 	  -=  readbytes;
				video_buffer->frame_size[MP4_VIDEO_MAX_FRAME_READ-framecount]= readbytes;
				video_buffer->frame_duration[MP4_VIDEO_MAX_FRAME_READ-framecount]= frameduration;
				video_buffer->frame_synflag[MP4_VIDEO_MAX_FRAME_READ-framecount]= synflag;
				//update frame count
				framecount --;
			}
		}
     	};
	//update video stream buffer information
	video_buffer->frame_count = MP4_VIDEO_MAX_FRAME_READ-framecount;
	video_buffer->data_length  = datalength;
	//check return value
	if((result==MP4_MUX_DEMUX_MEMORY_LESS_DATA)&&(datalength))
		return MP4_MUX_DEMUX_RET_NOERR;
	return result;
}

/*******************************************************************************************
Description:
	 write one frame audio data to mux file, it can set pisition and update all tables
Parameters:
        file: mp4 infor stuctor point (include file handle)
        audio_buffer: audio buffer start point
        bytes: the frame audio data length
        duration: the frame duration
Return:	
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_FILE_POINT_OVERFLOW
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_audio_frames(mp4_infor_t *file, audio_buffer_t *audio_buffer,uint32 bufferindex)
{
	uint32 offset = mp4_get_file_position(file);
	uint32 framecount;
	uint32 result;

	/* Defeat 32 bit file size limit. */
	if(mp4_test_position(file)) 
		return MP4_MUX_DEMUX_FILE_POINT_OVERFLOW;
	/* write data to file*/
	//offset+=6;	
	result = mp4_write_data_core(file,(char*)audio_buffer->buffer_address,audio_buffer->data_length,MP4_IDX_AUD_STATUS,bufferindex);
	if(result!=audio_buffer->data_length)
		{return MP4_MUX_WRITE_FILE_ERROR;}
#if 0
	result = mp4_write_data(file, (char*)audio_buffer->buffer_address, audio_buffer->data_length);
	if(result!=audio_buffer->data_length)
		{return MP4_MUX_WRITE_FILE_ERROR;}
#endif
	/*update table*/
	for(framecount=0; framecount<audio_buffer->frame_count; framecount++)
	{
		mp4_update_tables(file,
					     file->atracks[DEFAULT_TRACK].track,
					     offset,
					     file->atracks[DEFAULT_TRACK].current_chunk,
					     file->atracks[DEFAULT_TRACK].current_position,
					     (uint32)audio_buffer->frame_size,
					     (uint32)audio_buffer->frame_duration,
					     0);
		file->atracks[DEFAULT_TRACK].current_position += 1;
		file->atracks[DEFAULT_TRACK].current_chunk++;
		offset += (uint32)audio_buffer->frame_size;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
#if VIDEORECORDERIN848
/*******************************************************************************************
Description:
	 write one frame video data to mux file, it can set pisition and update all tables
Parameters:
        file: mp4 infor stuctor point (include file handle)
        video_buffer: video buffer start point
        bytes: the frame audio data length
        isKeyFrame: =1 the frame is syn frame, =0 the frame is not syn frame
        duration: the frame duration
Return:	
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_FILE_POINT_OVERFLOW
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_video_frames(mp4_infor_t *file, video_buffer_t *video_buffer,uint32 bufferindex)
{
	uint32  offset = mp4_get_file_position(file);
	uint32  framecount, framesize;
	uint32  buffer_address, data_length;
	uint32  result;
	//mp4_esds_t* esds;
	/* Defeat 32 bit file size limit. */
	if(mp4_test_position(file)) 
		return MP4_MUX_DEMUX_FILE_POINT_OVERFLOW;
	buffer_address = video_buffer->buffer_address;
	data_length = video_buffer->data_length;
	#if 1
	//add this for vimicro chip by Amanda.Deng 2007.03.07
	if(file->video_firsttime)
	{//if is 820 hardware decode video,need write mp4v decode confige information at first
		file->video_firsttime = 0;
		if (file->video_type == MP4_TYPE_VIDEO_IS_MP4V)
		{
			//esds = &file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds;
			//mp4_memcopy((char*)esds->decoderConfig, (char*)buffer_address, 19);
			//buffer_address+=19;
			//data_length -=19;
			MMD_SYS_FillMemData(buffer_address,19,0);
			offset+=19;
			framesize = video_buffer->frame_size[0];
			framesize-=19;
			video_buffer->frame_size[0]=framesize;
		}
	}
	#endif
	/* write data to file*/
	result = mp4_write_data_core(file, (char*)buffer_address, data_length,MP4_IDX_VID_STATUS,bufferindex);
	if(result!=data_length)
		{return MP4_MUX_WRITE_FILE_ERROR;}
	/*update table*/
	for(framecount=0; framecount<video_buffer->frame_count; framecount++)
	{
		mp4_update_tables(file,
					     file->vtracks[DEFAULT_TRACK].track,
					     offset,
					     file->vtracks[DEFAULT_TRACK].current_chunk,
					     file->vtracks[DEFAULT_TRACK].current_position,
					     (uint32)video_buffer->frame_size[framecount],
					     (uint32)1,
					     (uint32)1);
		file->vtracks[DEFAULT_TRACK].current_position += 1;
		file->vtracks[DEFAULT_TRACK].current_chunk++;
		offset += (uint32)video_buffer->frame_size[framecount];
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
#else
/*******************************************************************************************
Description:
	 write one frame video data to mux file, it can set pisition and update all tables
Parameters:
        file: mp4 infor stuctor point (include file handle)
        video_buffer: video buffer start point
        bytes: the frame audio data length
        isKeyFrame: =1 the frame is syn frame, =0 the frame is not syn frame
        duration: the frame duration
Return:	
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_FILE_POINT_OVERFLOW
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_video_frames(mp4_infor_t *file, video_buffer_t *video_buffer)
{
	uint32  offset = mp4_get_file_position(file);
	uint32  framecount, framesize;
	uint32  buffer_address, data_length;
	uint32  result;
	mp4_esds_t* esds;
	/* Defeat 32 bit file size limit. */
	if(mp4_test_position(file)) 
		return MP4_MUX_DEMUX_FILE_POINT_OVERFLOW;
	buffer_address = video_buffer->buffer_address;
	data_length = video_buffer->data_length;
	#if 1
	//add this for vimicro chip by Amanda.Deng 2007.03.07
	if(file->video_firsttime)
	{//if is 820 hardware decode video,need write mp4v decode confige information at first
		file->video_firsttime = 0;
		if (file->video_type == MP4_TYPE_VIDEO_IS_MP4V)
		{
			esds = &file->vtracks[DEFAULT_TRACK].track->mdia.minf.stbl.stsd.table[0].esds;
			mp4_memcopy((char*)esds->decoderConfig, (char*)buffer_address, 19);
			//buffer_address+=19;
			//data_length -=19;
			offset+=19;
			framesize = video_buffer->frame_size[0];
			framesize-=19;
			video_buffer->frame_size[0]=framesize;
		}
	}
	#endif
	/* write data to file*/
	result = mp4_write_data_core(file, (char*)buffer_address, data_length,MP4_IDX_VID_STATUS,28);
	if(result!=data_length)
		{return MP4_MUX_WRITE_FILE_ERROR;}
	/*update table*/
	for(framecount=0; framecount<video_buffer->frame_count; framecount++)
	{
		mp4_update_tables(file,
					     file->vtracks[DEFAULT_TRACK].track,
					     offset,
					     file->vtracks[DEFAULT_TRACK].current_chunk,
					     file->vtracks[DEFAULT_TRACK].current_position,
					     (uint32)video_buffer->frame_size[framecount],
					     (uint32)video_buffer->frame_duration[framecount],
					     (uint32)video_buffer->frame_synflag[framecount]);
		file->vtracks[DEFAULT_TRACK].current_position += 1;
		file->vtracks[DEFAULT_TRACK].current_chunk++;
		offset += (uint32)video_buffer->frame_size[framecount];
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
#endif
/*******************************************************************************************
Description:
	 read current video frame data, it can set pisition and update current frame index
Parameters:
        file: mp4 infor stuctor point (include file handle)
        video_buffer: video buffer start point
        maxBytes: max buffer size
Return:	
	MP4_MUX_DEMUX_MEMORY_LESS_DATA
	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
	   0.11			Amanda Deng			2007.03.07		create first version
********************************************************************************************/
uint32 mp4_read_key_video_frame(mp4_infor_t *file, char *video_buffer, uint32 maxBytes,  uint32 *readbytes,  uint32 *duration)
{
	uint32  framesize, frameindex, frameduration;
	uint32  synflag, totalduration = 0;
	uint32  result = MP4_MUX_DEMUX_RET_NOERR;

	*readbytes = 0;
	*duration = 0;
	//if have video track?
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	//if read end?
	if(file->vtracks[DEFAULT_TRACK].current_position>=file->total_vsamples)
		return MP4_DEMUX_END_OF_VIDEO;
	//check for I_VOP sample and read the I_VOP sample data
	frameindex = file->vtracks[DEFAULT_TRACK].current_position;
	while((frameindex<file->total_vsamples)&&(!result))
	{
		mp4_video_sample_duration(file, frameindex, (uint32*)&frameduration);
		totalduration += frameduration;
		synflag = mp4_is_syn_video_sample(file, frameindex+1);
		if (synflag)
		{
			result = mp4_read_video_frame(file, video_buffer, maxBytes, readbytes);
			break;
		}
		else 
		{
			mp4_video_frame_size(file, file->vtracks[DEFAULT_TRACK].current_position, &framesize);
			mp4_set_video_position(file, file->vtracks[DEFAULT_TRACK].current_position);
			file->vtracks[DEFAULT_TRACK].last_sample_size = framesize;
			file->vtracks[DEFAULT_TRACK].current_position++;
		}
		frameindex++;
	}
	//check for next I_VOP sample get current I_VOP sample duration
	frameindex = file->vtracks[DEFAULT_TRACK].current_position;
	while((frameindex<file->total_vsamples)&&(!result))
	{
		synflag = mp4_is_syn_video_sample(file, frameindex+1);
		if (synflag)
		{
			break;
		}
		else 
		{
			mp4_video_sample_duration(file, frameindex, (uint32*)&frameduration);
			totalduration += frameduration;
			mp4_video_frame_size(file, file->vtracks[DEFAULT_TRACK].current_position, &framesize);
			mp4_set_video_position(file, file->vtracks[DEFAULT_TRACK].current_position);
			file->vtracks[DEFAULT_TRACK].last_sample_size = framesize;
			file->vtracks[DEFAULT_TRACK].current_position++;
		}
		frameindex++;
	}
	*duration = totalduration;
	return result;
}


/*******************************************************************************************
Description:
	 read current video frame data, it can set pisition and update current frame index
Parameters:
        file: mp4 infor stuctor point (include file handle)
        video_buffer: video buffer start point
        maxBytes: max buffer size
Return:	
	MP4_MUX_DEMUX_MEMORY_LESS_DATA
	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.15		create first version
********************************************************************************************/
uint32 mp4_read_key_video_frames(mp4_infor_t *file, video_buffer_t *video_buffer)
{
	uint32  buffer_address, buffer_size;
	uint32  framecount, frameduration, readbytes;
	uint32  datalength=0;
	uint32  result = MP4_MUX_DEMUX_RET_NOERR;

	video_buffer->frame_count = 0;
	video_buffer->data_length = 0;
	//if have audio track?
	if (!file->total_vtracks) 
		return MP4_MUX_DEMUX_HAVE_NO_VIDEO;
	//if read end?
	if(file->vtracks[DEFAULT_TRACK].current_position>=file->total_vsamples)
		return MP4_DEMUX_END_OF_VIDEO;
	//get buffer address and buffer size
	buffer_address = video_buffer->buffer_address;
	buffer_size = video_buffer->buffer_size;
	//read frames(frame>=1) video data
	framecount = MP4_VIDEO_MAX_FRAME_READ;
	while((!result)&&(framecount))
	{
		result = mp4_read_key_video_frame(file, (char*)buffer_address, buffer_size, &readbytes, &frameduration);
		if((!result) |(result==MP4_DEMUX_LAST_OF_VIDEO))
       	{
			//update buffer address and size
			datalength 	  += readbytes;
			buffer_address +=  readbytes;
			buffer_size 	  -=  readbytes;
			video_buffer->frame_duration[MP4_VIDEO_MAX_FRAME_READ-framecount]= frameduration;
			video_buffer->frame_synflag[MP4_VIDEO_MAX_FRAME_READ-framecount]= 1;
			video_buffer->frame_size[MP4_VIDEO_MAX_FRAME_READ-framecount]= readbytes;
			//update frame count
			framecount --;
		}
     	};
	//update video stream buffer information
	video_buffer->frame_count = MP4_VIDEO_MAX_FRAME_READ-framecount;
	video_buffer->data_length  = datalength;
	//check return value
	if((result==MP4_MUX_DEMUX_MEMORY_LESS_DATA)&&(datalength))
		return MP4_MUX_DEMUX_RET_NOERR;
	return result;
}




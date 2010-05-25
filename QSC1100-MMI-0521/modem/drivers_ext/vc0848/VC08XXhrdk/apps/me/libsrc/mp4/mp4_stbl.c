/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	stbl.c  
* User for : 	mp4 demux mux, stbl atom is leaf atom of minf atom, it is mandatory
*			sample table box, include all sample time and space map  information of this track
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"

void mp4_stbl_init(mp4_stbl_t *stbl)
{
	stbl->version = 0;
	stbl->flags = 0;
	mp4_stsd_init(&(stbl->stsd));
	mp4_stts_init(&(stbl->stts));
	mp4_stss_init(&(stbl->stss));
	mp4_stsc_init(&(stbl->stsc));
	mp4_stsz_init(&(stbl->stsz));
	mp4_stco_init(&(stbl->stco));
}

void mp4_stbl_init_video(mp4_infor_t *file, 
							mp4_stbl_t *stbl, 
							uint32 frame_w,
							uint32 frame_h, 
							uint32  sample_duration,
							char *compressor)
{
	mp4_stsd_init_video(file, &(stbl->stsd), frame_w, frame_h, compressor);
	mp4_stsz_init_common_video(file, &(stbl->stsz), 0);
	mp4_stts_init_common_video(file, &(stbl->stts), sample_duration);
	mp4_stss_init_common(file, &(stbl->stss));
	mp4_stsc_init_common(file, &(stbl->stsc));
	mp4_stco_init_common_video(file, &(stbl->stco));
	stbl->stsz.is_video = 1;
	stbl->stco.is_video = 1;
	stbl->stsc.is_video = 1;
	stbl->stts.is_video = 1;
	
}


void mp4_stbl_init_audio(mp4_infor_t *file, 
							mp4_stbl_t *stbl, 
							uint32 channels, 
							uint32 bits, 
							uint32  sample_rate, 
							uint32  sample_size,
							uint32  sample_duration,
							char *compressor)
{
	mp4_stsd_init_audio(file, &(stbl->stsd), channels, bits, sample_rate, compressor);
	mp4_stsz_init_common_audio(file, &(stbl->stsz), sample_size);
	mp4_stts_init_common_audio(file, &(stbl->stts), sample_duration);
	mp4_stss_init_common(file, &(stbl->stss));
	mp4_stsc_init_common(file, &(stbl->stsc));
	mp4_stco_init_common_audio(file, &(stbl->stco));
	stbl->stsz.is_audio = 1;
	stbl->stco.is_audio = 1;
	stbl->stsc.is_audio = 1;
	stbl->stts.is_audio = 1;
}

void mp4_stbl_delete(mp4_infor_t *file, mp4_stbl_t *stbl)
{
	mp4_stsd_delete(file,&(stbl->stsd));
	mp4_stts_delete(file,&(stbl->stts));
	mp4_stss_delete(file,&(stbl->stss));
	mp4_stsc_delete(file,&(stbl->stsc));
	mp4_stsz_delete(file,&(stbl->stsz));
	mp4_stco_delete(file,&(stbl->stco));
}

void mp4_stbl_dump(void *minf_ptr, mp4_stbl_t *stbl)
{
	MP4_DEMUX_MSG(("sample table\n"));
	mp4_stsd_dump(minf_ptr, &(stbl->stsd));
	mp4_stts_dump(&(stbl->stts));
	mp4_stss_dump(&(stbl->stss));
	mp4_stsc_dump(&(stbl->stsc));
	mp4_stsz_dump(&(stbl->stsz));
	mp4_stco_dump(&(stbl->stco));
}

uint32 mp4_read_stbl(mp4_infor_t *file, mp4_minf_t *minf, mp4_stbl_t *stbl, mp4_atom_t *parent_atom)
{
	mp4_atom_t leaf_atom;
	uint32 result;

	do
	{
		result = mp4_atom_read_header(file, &leaf_atom);
		if(result) return result;
		/* mandatory */
		if(mp4_atom_is(&leaf_atom, "stsd"))
		{ 
			result = mp4_read_stsd(file, minf, &(stbl->stsd)); 
			/* Some codecs store extra information at the end of this */
			if(result) return result;
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "stts"))
		{ 
			stbl->stts.start_position = leaf_atom.start;
			stbl->stts.end_position= leaf_atom.end;
			result = mp4_read_stts(file, &(stbl->stts)); 
			if(result) return result;
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "stss"))
		{ 	
			stbl->stss.start_position = leaf_atom.start;
			stbl->stss.end_position= leaf_atom.end;
			result = mp4_read_stss(file, &(stbl->stss)); 
			if(result) return result;
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "stsc"))
		{ 
			stbl->stsc.start_position = leaf_atom.start;
			stbl->stsc.end_position= leaf_atom.end;
			result = mp4_read_stsc(file, &(stbl->stsc)); 
			if(result) return result;
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "stsz"))
		{ 	
			stbl->stsz.start_position = leaf_atom.start;
			stbl->stsz.end_position= leaf_atom.end;
			result = mp4_read_stsz(file, &(stbl->stsz)); 
			if(result) return result;
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "stco"))
		{ 
			stbl->stco.start_position = leaf_atom.start;
			stbl->stco.end_position= leaf_atom.end;
			result = mp4_read_stco(file, &(stbl->stco)); 
			if(result) return result;
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
		else
		{
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
	}while(mp4_get_file_position(file) < parent_atom->end);

	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_write_stbl(mp4_infor_t *file, mp4_minf_t *minf, mp4_stbl_t *stbl)
{
	mp4_atom_t atom;
	uint32 result;
	
	result = mp4_atom_write_header(file, &atom, "stbl");
	if(result) return result;
	
	result = mp4_write_stsd(file, minf, &(stbl->stsd));
	if(result) return result;
	result = mp4_write_stco(file, &(stbl->stco));
	if(result) return result;
	result = mp4_write_stsc(file, &(stbl->stsc));
	if(result) return result;
	result = mp4_write_stsz(file, &(stbl->stsz));
	if(result) return result;
	result = mp4_write_stts(file, &(stbl->stts));
	if(result) return result;
	result = mp4_write_stss(file, &(stbl->stss));
	if(result) return result;
	
	return   mp4_atom_write_footer(file, &atom);

}



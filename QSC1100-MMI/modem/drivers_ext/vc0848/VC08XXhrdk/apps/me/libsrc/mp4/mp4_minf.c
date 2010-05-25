/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	minf.c  
* User for : 	mp4 demux mux, minf atom is leaf atom of mdia atom, it is mandatory 
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"

/*******************************************************************************************
Description:
	read atom type and size information form file, nomal need read 8 bytes
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_minf_init(mp4_minf_t *minf)
{
	minf->is_video = minf->is_audio = 0;
	mp4_vmhd_init(&(minf->vmhd));
	mp4_smhd_init(&(minf->smhd));
	mp4_hdlr_init(&(minf->hdlr));
	mp4_dinf_init(&(minf->dinf));
	mp4_stbl_init(&(minf->stbl));
}

/*******************************************************************************************
Description:
	read atom type and size information form file, nomal need read 8 bytes
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_minf_init_video(mp4_infor_t *file, 
								mp4_minf_t *minf, 
								uint32 frame_w,
								uint32 frame_h, 
								uint32  time_scale, 
								uint32  sample_duration,
								char *compressor)
{
	minf->is_video = 1;
	mp4_vmhd_init_video(file, &(minf->vmhd), frame_w, frame_h, sample_duration);
	mp4_stbl_init_video(file, &(minf->stbl), frame_w, frame_h, sample_duration, compressor);
	mp4_hdlr_init_video(&(minf->hdlr));
	mp4_dinf_init_all(&(minf->dinf));
}

/*******************************************************************************************
Description:
	read atom type and size information form file, nomal need read 8 bytes
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_minf_init_audio(mp4_infor_t *file, 
							mp4_minf_t *minf, 
							uint32 channels, 
							uint32 bits,
							uint32  sample_rate, 
							uint32  sample_size,
							uint32  time_scale,
							uint32  sample_duration,
							char *compressor)
{
	minf->is_audio = 1;
	/* smhd doesn't store anything worth initializing */
	mp4_stbl_init_audio(file, &(minf->stbl), channels, bits, sample_rate, sample_size, sample_duration, compressor);
	mp4_hdlr_init_data(&(minf->hdlr));
	mp4_dinf_init_all(&(minf->dinf));
}


/*******************************************************************************************
Description:
	read atom type and size information form file, nomal need read 8 bytes
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_minf_delete(mp4_infor_t *file,mp4_minf_t *minf)
{
	mp4_vmhd_delete(&(minf->vmhd));
	mp4_smhd_delete(&(minf->smhd));
	mp4_dinf_delete(&(minf->dinf));
	mp4_stbl_delete(file,&(minf->stbl));
	mp4_hdlr_delete(&(minf->hdlr));
}

/*******************************************************************************************
Description:
	read atom type and size information form file, nomal need read 8 bytes
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_minf_dump(mp4_minf_t *minf)
{
	MP4_DEMUX_MSG(("media info\n"));
	MP4_DEMUX_MSG(("is_audio %d\n", minf->is_audio));
	MP4_DEMUX_MSG(("is_video %d\n", minf->is_video));
	if(minf->is_audio) mp4_smhd_dump(&(minf->smhd));
	if(minf->is_video) mp4_vmhd_dump(&(minf->vmhd));
	mp4_hdlr_dump(&(minf->hdlr));
	mp4_dinf_dump(&(minf->dinf));
	mp4_stbl_dump(minf, &(minf->stbl));
}

/*******************************************************************************************
Description:
	read atom type and size information form file, nomal need read 8 bytes
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_minf(mp4_infor_t *file, mp4_minf_t *minf, mp4_atom_t *parent_atom)
{
//by dnn 2008/4/11
#if MP4LIB_VERSION2_FOREHANDLE 
//	static uint32 vtrack_counter=0;
//	static uint32 atrack_counter=0;
	uint32 stbl_pos;
#endif
//end	
	mp4_atom_t leaf_atom;
	uint32  result, pos = mp4_get_file_position(file);
	do
	{
		result = mp4_atom_read_header(file, &leaf_atom);
		if(result) return result;
		/* mandatory */
		if(mp4_atom_is(&leaf_atom, "vmhd"))
		{ 
			minf->is_video = 1; 
//by dnn 2008/4/11
#if MP4LIB_VERSION2_FOREHANDLE 
			file->vtrack_counter++;
#endif
//end
			result = mp4_read_vmhd(file, &(minf->vmhd));
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "smhd"))
		{ 
			minf->is_audio = 1; 
//by dnn 2008/4/11
#if MP4LIB_VERSION2_FOREHANDLE 
			file->atrack_counter++;
#endif
//end			
			result = mp4_read_smhd(file, &(minf->smhd)); 
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "hdlr"))
		{ 
			result = mp4_read_hdlr(file, &(minf->hdlr)); 
			if(result) return result;
			/* Main Actor doesn't write component name */
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "dinf"))
		{ 
			result = mp4_read_dinf(file, &(minf->dinf), &leaf_atom); 
			if(result) return result;
		}
		else
		{
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
	}while(mp4_get_file_position(file) < parent_atom->end);

	result = mp4_set_file_position(file, pos);
	if(result) return result;
	
	do {
		result = mp4_atom_read_header(file, &leaf_atom);
		if(result) return result;
		
		if(mp4_atom_is(&leaf_atom, "stbl")) 
		{
//by dnn 2008/4/11
#if MP4LIB_VERSION2_FOREHANDLE 
			stbl_pos = mp4_get_file_position(file);
			if(minf->is_video && file->vtrack_counter==DEFAULT_TRACK+1) {
				file->stream_videoindx = file->duplicate_handle_func(file->stream);
				file->videoindx_origpos = stbl_pos;
			} else if(minf->is_audio && file->atrack_counter==DEFAULT_TRACK+1) {
				file->stream_audioindx = file->duplicate_handle_func(file->stream);
				file->audioindx_origpos = stbl_pos;
			}
#endif
//end
			result = mp4_read_stbl(file, minf, &(minf->stbl), &leaf_atom);
			if(result) return result;
		} 
		else
		{
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
	} while(mp4_get_file_position(file) < parent_atom->end);

	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	read atom type and size information form file, nomal need read 8 bytes
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_minf(mp4_infor_t *file, mp4_minf_t *minf)
{
	mp4_atom_t atom;
	uint32  result;
	
	result = mp4_atom_write_header(file, &atom, "minf");
	if(result) return result;
	
	if(minf->is_video) 
	{
		result = mp4_write_vmhd(file, &(minf->vmhd));
		if(result) return result;
	}
	if(minf->is_audio) 
	{
		result = mp4_write_smhd(file, &(minf->smhd));
		if(result) return result;
	}
	result = mp4_write_hdlr(file, &(minf->hdlr));
	if(result) return result;
	result = mp4_write_dinf(file, &(minf->dinf));
	if(result) return result;
	result = mp4_write_stbl(file, minf, &(minf->stbl));
	if(result) return result;

	return mp4_atom_write_footer(file, &atom);
}

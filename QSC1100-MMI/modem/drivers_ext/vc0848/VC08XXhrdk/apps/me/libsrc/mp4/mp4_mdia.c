/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	mdia.c  
* User for : 	mp4 demux mux, mdia atom is leaf atom of trak atom, it is mandatory 
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
void mp4_mdia_init(mp4_mdia_t *mdia)
{
	mp4_mdhd_init(&(mdia->mdhd));
	mp4_hdlr_init(&(mdia->hdlr));
	mp4_minf_init(&(mdia->minf));
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
void mp4_mdia_init_video(mp4_infor_t *file, 
							mp4_mdia_t *mdia,
							uint32 frame_w,
							uint32 frame_h, 
							uint32 sample_duration,
							uint32 time_scale,
							char *compressor)
{
	mp4_mdhd_init_video(file, &(mdia->mdhd), time_scale);
	mp4_hdlr_init_video(&(mdia->hdlr));
	mp4_minf_init_video(file, &(mdia->minf), frame_w, frame_h, time_scale, sample_duration, compressor);
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
void mp4_mdia_init_audio(mp4_infor_t *file, 
							mp4_mdia_t *mdia, 
							uint32 channels,
							uint32 bits, 
							uint32  sample_rate, 
							uint32  sample_size,
							uint32  time_scale,
							uint32  sample_duration,
							char *compressor)
{
	mp4_mdhd_init_audio(file, &(mdia->mdhd), time_scale);
	mp4_hdlr_init_audio(&(mdia->hdlr));
	mp4_minf_init_audio(file, &(mdia->minf), channels, bits, sample_rate, sample_size, time_scale, sample_duration, compressor);
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
void mp4_mdia_delete(mp4_infor_t *file,mp4_mdia_t *mdia)
{
	mp4_mdhd_delete(&(mdia->mdhd));
	mp4_hdlr_delete(&(mdia->hdlr));
	mp4_minf_delete(file,&(mdia->minf));
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
void mp4_mdia_dump(mp4_mdia_t *mdia)
{
	MP4_DEMUX_MSG((" media\n"));
	mp4_mdhd_dump(&(mdia->mdhd));
	mp4_hdlr_dump(&(mdia->hdlr));
	mp4_minf_dump(&(mdia->minf));
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
uint32 mp4_read_mdia(mp4_infor_t *file, mp4_mdia_t *mdia, mp4_atom_t *trak_atom)
{
	mp4_atom_t leaf_atom;
	uint32 result;
	do
	{
		result = mp4_atom_read_header(file, &leaf_atom);
		if(result) return result;
	
		if(mp4_atom_is(&leaf_atom, "mdhd"))
		{ 
			result = mp4_read_mdhd(file, &(mdia->mdhd)); 
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "hdlr"))
		{
			result = mp4_read_hdlr(file, &(mdia->hdlr)); 
			if(result) return result;
			/* Main Actor doesn't write component name */
			result = mp4_atom_skip(file, &leaf_atom);
			if(result) return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "minf"))
		{ 
			result = mp4_read_minf(file, &(mdia->minf), &leaf_atom); 
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
uint32 mp4_write_mdia(mp4_infor_t *file, mp4_mdia_t *mdia)
{
	mp4_atom_t atom;
	uint32 result;
	
	result =mp4_atom_write_header(file, &atom, "mdia");
	if(result) return result;
	result = mp4_write_mdhd(file, &(mdia->mdhd));
	if(result) return result;
	result = mp4_write_hdlr(file, &(mdia->hdlr));
	if(result) return result;
	result = mp4_write_minf(file, &(mdia->minf));
	return mp4_atom_write_footer(file, &atom);

}

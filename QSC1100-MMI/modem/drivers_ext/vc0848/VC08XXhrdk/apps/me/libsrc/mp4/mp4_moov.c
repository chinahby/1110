/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	moov.c  
* User for : 	mp4 demux mux, a mp4 file have mdat and moov two atom, moov include video and audio information 
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
void mp4_moov_init(mp4_moov_t *moov)
{
	uint32 i;

	moov->total_tracks = 0;
	for(i = 0 ; i < MAXTRACKS; i++) 
		moov->trak[i] = 0;
	mp4_mvhd_init(&(moov->mvhd));
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
void mp4_moov_delete(mp4_infor_t *file,mp4_moov_t *moov)
{

	while(moov->total_tracks) 
	{
		mp4_delete_trak(file,moov, moov->trak[moov->total_tracks - 1]);
	}
	mp4_mvhd_delete(&(moov->mvhd));
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
void mp4_moov_dump(mp4_moov_t *moov)
{
	uint32 i;
	MP4_DEMUX_MSG(("movie\n"));
	mp4_mvhd_dump(&(moov->mvhd));
	for(i = 0; i < moov->total_tracks; i++)
		mp4_trak_dump(moov->trak[i]);
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
uint32 mp4_read_moov(mp4_infor_t *file, mp4_moov_t *moov, mp4_atom_t *parent_atom)
{
	/* mandatory mvhd */
	mp4_atom_t leaf_atom;
	uint32 result;
	/*parent_atom is moov*/
	do
	{
		//get leaf atom size(64 bits/32bits) and type (4bytes)
		result = mp4_atom_read_header(file, &leaf_atom);
		if(result)
			return result;
		
		if(mp4_atom_is(&leaf_atom, "mvhd"))
		{
			//get mvhd atom information(time_scale must use)
			mp4_read_mvhd(file, &(moov->mvhd));
			if(result)
				return result;
		}
		else
		if(mp4_atom_is(&leaf_atom, "trak"))
		{
			//a moov have less than one trak (trak is must)
			mp4_trak_t *trak = mp4_add_trak(moov);
			result = mp4_read_trak(file, trak, &leaf_atom);
			if(result)
				return result;
			//if have 10 track have add, must break because memory limited define
			if(moov->total_tracks>=MAXTRACKS)
			{
				MP4_DEMUX_WARNING(("MP4_THIS_VERSION_CANNOT_DEMUX!"));
				break;
			}
		}
		else
		{
			//others atom skip over (no use because these atom)
			result = mp4_atom_skip(file, &leaf_atom);
			if(result)
				return result;
		}
	}while(mp4_get_file_position(file)+7 < parent_atom->end);
	
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
	   0.11			Amanda Deng			2007.01.19		modify for 820 media engine
********************************************************************************************/
uint32 mp4_write_moov(mp4_infor_t *file, mp4_moov_t *moov)
{
	mp4_atom_t atom;
	uint32  i, result;
	uint32  longest_duration = 0;
	uint32  duration, timescale;

	/* write moov head */
	result = mp4_atom_write_header(file, &atom, "moov");
	if(result)
		return result;

	/* get the duration from the longest track in the mvhd's timescale */
	for(i = 0; i < moov->total_tracks; i++)
	{
		/* get each track duration and scaletime, (total time)*/
		mp4_trak_duration(moov->trak[i], &duration, &timescale);

		duration = (uint32 )((float)duration / timescale * moov->mvhd.time_scale);
		if(duration > longest_duration)
			longest_duration = duration;
	}
	/* write mvhd head */
	moov->mvhd.duration = longest_duration;
	moov->mvhd.selection_duration = longest_duration;
	result = mp4_write_mvhd(file, &(moov->mvhd));
	if(result)
		return result;
	/* write each track information*/
	for(i = 0; i < moov->total_tracks; i++)
	{
		result = mp4_write_trak(file, moov->trak[i], moov->mvhd.time_scale);
		if(result)
			return result;
	}
	/* write moov  atom size to file*/
	return  mp4_atom_write_footer(file, &atom);

}



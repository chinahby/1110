/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	mdat.c  
* User for : 	mp4 demux mux, a mp4 file have mdat and moov two atom, mdat is all data box, include video and audio
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
	initialize mdat head information
Parameters:	
	mdat: point of the mdat structor
Return:
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_mdat_init(mp4_mdat_t *mdat)
{
	mdat->size = 8;
	mdat->start = 0;
}

void mp4_mdat_delete(mp4_mdat_t *mdat)
{

}

/*******************************************************************************************
Description:
	set mdat size information and file point to data end position
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	mdat: point of the mdat structor
	parent_atom: point of the parent atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_mdat(mp4_infor_t *file, mp4_mdat_t *mdat, mp4_atom_t *parent_atom)
{
	mdat->size = parent_atom->size;
	mdat->start = parent_atom->start;
	return mp4_atom_skip(file, parent_atom);
}

/*******************************************************************************************
Description:
	write mdat size information to file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	mdat: point of the mdat structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_mdat(mp4_infor_t *file, mp4_mdat_t *mdat)
{
	uint32  size = 0, new_size = 0;
	uint32  i, result;
	
	for(i = 0; i < file->total_atracks; i++)
	{
		new_size = mp4_track_data_end_position(file->atracks[i].track);
		if(new_size > size) 
			size = new_size;
	}
	for(i = 0; i < file->total_vtracks; i++)
	{
		new_size = mp4_track_data_end_position(file->vtracks[i].track);
		if(new_size > size) 
			size = new_size;
	}
	mdat->size = size - mdat->start;
	//seek file position read to write data size
	result = mp4_set_file_position(file, mdat->start);
	if(result)
       	return result;
	//write data size to file
	result = mp4_write_uint32(file, mdat->size);
	if(result)
       	return result;
	//seek file position to data end for next to write
	return  mp4_set_file_position(file, mdat->start + mdat->size);
}

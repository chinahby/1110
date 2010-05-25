/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	hdlr.c  
* User for : 	mp4 demux mux, hdlr atom is leaf atom of mdia,  it is mandatory
*			hdlr atom is handler, delcares the mdia handler type
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
	initialize hdlr head information
Parameters:	
	hdlr: point of the hdlr structor
Return:
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_hdlr_init(mp4_hdlr_t *hdlr)
{
	hdlr->version = 0;
	hdlr->flags = 0;
	hdlr->component_type[0] = 'm';
	hdlr->component_type[1] = 'h';
	hdlr->component_type[2] = 'l';
	hdlr->component_type[3] = 'r';
	hdlr->component_subtype[0] = 'v';
	hdlr->component_subtype[1] = 'i';
	hdlr->component_subtype[2] = 'd';
	hdlr->component_subtype[3] = 'e';
	hdlr->component_manufacturer = 0;
	hdlr->component_flags = 0;
	hdlr->component_flag_mask = 0;
	strcpy(hdlr->component_name, "VimicroMediaHandler");
}

/*******************************************************************************************
Description:
	initialize video handler information
Parameters:	
	hdlr: point of the hdlr structor
Return:
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_hdlr_init_video(mp4_hdlr_t *hdlr)
{
	hdlr->component_subtype[0] = 'v';
	hdlr->component_subtype[1] = 'i';
	hdlr->component_subtype[2] = 'd';
	hdlr->component_subtype[3] = 'e';
	strcpy(hdlr->component_name, "VimicroVideoMediaHandler");
}

/*******************************************************************************************
Description:
	initialize audio handler information
Parameters:	
	hdlr: point of the hdlr structor
Return:
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_hdlr_init_audio(mp4_hdlr_t *hdlr)
{
	hdlr->component_type[0] = ' ';
	hdlr->component_type[1] = ' ';
	hdlr->component_type[2] = ' ';
	hdlr->component_type[3] = ' ';
	
	hdlr->component_subtype[0] = 's';
	hdlr->component_subtype[1] = 'o';
	hdlr->component_subtype[2] = 'u';
	hdlr->component_subtype[3] = 'n';
	strcpy(hdlr->component_name, "VimicroSoundMediaHandler");
}

/*******************************************************************************************
Description:
	initialize hdlr head information
Parameters:	
	hdlr: point of the hdlr structor
Return:
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_hdlr_init_data(mp4_hdlr_t *hdlr)
{
	hdlr->component_type[0] = 'd';
	hdlr->component_type[1] = 'h';
	hdlr->component_type[2] = 'l';
	hdlr->component_type[3] = 'r';
	hdlr->component_subtype[0] = 'a';
	hdlr->component_subtype[1] = 'l';
	hdlr->component_subtype[2] = 'i';
	hdlr->component_subtype[3] = 's';
	strcpy(hdlr->component_name, "Vimicro Alias Data Handler");
}

void mp4_hdlr_delete(mp4_hdlr_t *hdlr)
{
}

void mp4_hdlr_dump(mp4_hdlr_t *hdlr)
{
	MP4_DEMUX_MSG((" handler reference (hdlr)\n"));
	MP4_DEMUX_MSG((" version %d\n", hdlr->version));
	MP4_DEMUX_MSG((" flags %d\n", hdlr->flags));
	MP4_DEMUX_MSG((" component_type %c%c%c%c\n", hdlr->component_type[0], hdlr->component_type[1], hdlr->component_type[2], hdlr->component_type[3]));
	MP4_DEMUX_MSG((" component_subtype %c%c%c%c\n", hdlr->component_subtype[0], hdlr->component_subtype[1], hdlr->component_subtype[2], hdlr->component_subtype[3]));
	MP4_DEMUX_MSG((" component_name %s\n", hdlr->component_name));
}

/*******************************************************************************************
Description:
	read handler information form file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	hdlr: point of the hdlr structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_hdlr(mp4_infor_t *file, mp4_hdlr_t *hdlr)
{
	uint32 result;
	
	result = mp4_read_char(file, &hdlr->version);
	if(result) return result;
	result = mp4_read_uint24(file, &hdlr->flags);
	if(result) return result;
	result = mp4_read_char32(file, hdlr->component_type);
	if(result) return result;
	result = mp4_read_char32(file, hdlr->component_subtype);
	if(result) return result;
	result = mp4_read_uint32(file, &hdlr->component_manufacturer);
	if(result) return result;
	result = mp4_read_uint32(file, &hdlr->component_flags);
	return mp4_read_uint32(file, &hdlr->component_flag_mask);
}

/*******************************************************************************************
Description:
	write handler information to file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	hdlr: point of the hdlr structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_hdlr(mp4_infor_t *file, mp4_hdlr_t *hdlr)
{
	mp4_atom_t atom;
	uint32 i, length, result;
	
	result = mp4_atom_write_header(file, &atom, "hdlr");
	if(result) return result;
	result = mp4_write_char(file, hdlr->version);
	if(result) return result;
	result = mp4_write_uint24(file, hdlr->flags);
	if(result) return result;
	
	if((file->audio_type == MP4_TYPE_AUDIO_IS_MP4A)||(file->video_type == MP4_TYPE_VIDEO_IS_MP4V))
	{
		result = mp4_write_uint32(file, 0x00000000);
		if(result) return result;
		result = mp4_write_char32(file, hdlr->component_subtype);
		if(result) return result;
		for (i = 0; i < 3; i++) 
		{
			result = mp4_write_uint32(file, 0x00000000);
			if(result) return result;
		}
		length = strlen(hdlr->component_name) + 1;
		result = mp4_write_data(file, hdlr->component_name, length);
		if(result != length) return MP4_MUX_WRITE_FILE_ERROR;
	} 
	else 
	{
		result = mp4_write_char32(file, hdlr->component_type);
		if(result) return result;
		result = mp4_write_char32(file, hdlr->component_subtype);
		if(result) return result;
		result = mp4_write_uint32(file, hdlr->component_manufacturer);
		if(result) return result;
		result = mp4_write_uint32(file, hdlr->component_flags);
		if(result) return result;
		result = mp4_write_uint32(file, hdlr->component_flag_mask);
		if(result) return result;
		result = mp4_write_pascal(file, hdlr->component_name);
		if(result) return result;
	}
	return mp4_atom_write_footer(file, &atom);
}

/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	esds.c  
* User for : 	mp4 demux mux, esds atom is leaf atom of mp4a or mp4v atom,  it is mandatory
*						     damr atom is leaf atom of samr,  it is mandatory
*						     d263 atom is leaf atom of s263,  it is mandatory
*			esds, damr, d263 atom is codec decode config information box, use this can know use which codec to decode
*
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
	initialize esds head information
Parameters:	
	esds: point of the esds structor
Return:
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_esds_init(mp4_esds_t *esds)
{
	esds->version = 0;
	esds->flags = 0;
	esds->object_type = 0x40;
	esds->stream_type = 0x15;
	esds->decoderConfigLen = 0;
	esds->decoderConfig = NULL;
}

void mp4_esds_delete(mp4_esds_t *esds)
{
	if(esds->decoderConfig!=NULL)
		mp4_free(esds->decoderConfig);
}

void mp4_esds_dump(mp4_esds_t *esds)
{
	uint32 i;

	MP4_DEMUX_MSG((" elementary stream descriptor\n"));
	MP4_DEMUX_MSG((" version %d\n", esds->version));
	MP4_DEMUX_MSG((" flags %d\n", esds->flags));
	MP4_DEMUX_MSG((" decoder config "));
	for (i = 0; i < esds->decoderConfigLen; i++) 
	{	
		MP4_DEMUX_MSG(("%02x ", esds->decoderConfig[i]));
	}
	MP4_DEMUX_MSG(("\n"));
}

/*******************************************************************************************
Description:
	read esds structor information form file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	esds: point of the esds structor 
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
	MP4_MUX_DEMUX_MALLOC_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_esds(mp4_infor_t *file, mp4_esds_t *esds)
{
	uint32 tag, length, result;
#if MP4LIB_VERSION2_FOREHANDLE
	uint32 tmplen=0;
#endif

	result = mp4_read_char(file, &esds->version);
	if(result) return result;
	result = mp4_read_uint24(file, &esds->flags);
	if(result) return result;
	/* get and verify ES_DescrTag */
	result = mp4_read_char(file, &tag);
	if(result) return result;

	if (tag == 0x03) 
	{
		/* read length */
		result = mp4_read_descr_length(file, &length);
		if(result) return result;
		
		if (length < (5 + 15)) 
		{
			return MP4_DEMUX_ESDS_DESCR_TAG_ERROR;
	    	}
		/* skip 3 bytes */
		result = mp4_set_file_position(file, mp4_get_file_position(file) + 3);
		if(result) return result;
	} 
	else 
	{
		/* skip 2 bytes */
		result = mp4_set_file_position(file, mp4_get_file_position(file) + 2);
		if(result) return result;
	}
	/* get and verify DecoderConfigDescrTab */
	result = mp4_read_char(file, &tag);
	if(result) return result;

	if (tag != 0x04) 
	{
		return MP4_DEMUX_ESDS_CONFI_TAG_ERROR;
	}
	/* read length */
	result = mp4_read_descr_length(file, &length);
	if(result) return result;

	if (length < 15) 
	{
		return MP4_DEMUX_ESDS_DESCR_LEN_ERROR;
	}
	/* objectTypeIndication */
	result = mp4_read_char(file, &esds->object_type);
	if(result) return result;
	//MP4_DEMUX_ERR((" esds->object_type %d\n", esds->object_type));
	/* streamType */
	result = mp4_read_char(file, &esds->stream_type);
	if(result) return result;
	//MP4_DEMUX_ERR((" esds->stream_type %d\n", esds->stream_type));
	/* skip 11 bytes */
	result = mp4_set_file_position(file, mp4_get_file_position(file) + 11);
	if(result) return result;
	/* get and verify DecSpecificInfoTag */
	result = mp4_read_char(file, &tag);
	if(result) return result;

	if (tag != 0x05) 
	{
		return MP4_DEMUX_ESDS_SPECI_TAG_ERROR;
	}
	/* read length */
	result = mp4_read_descr_length(file, &esds->decoderConfigLen); 
	if(result) return result;
	
	if(esds->decoderConfig!=NULL)
		mp4_free(esds->decoderConfig);
	
	if(esds->decoderConfigLen)
	{
//by dnn 2008/4/18	
#if !(MP4LIB_VERSION2_FOREHANDLE)	
		esds->decoderConfig = mp4_malloc(esds->decoderConfigLen);
		if (esds->decoderConfig) 
		{
			result = mp4_read_data(file, (char*)esds->decoderConfig, esds->decoderConfigLen);
			if(result !=esds->decoderConfigLen)
				return MP4_DEMUX_READ_FILE_ERROR;
		} 
#else
		tmplen = (esds->decoderConfigLen%4==0)?0:(4-esds->decoderConfigLen%4);
		esds->decoderConfig = mp4_malloc(esds->decoderConfigLen+tmplen);
		if (esds->decoderConfig) 
		{
			result = mp4_read_data(file, (char*)esds->decoderConfig, esds->decoderConfigLen);
			if(result !=esds->decoderConfigLen)
				return MP4_DEMUX_READ_FILE_ERROR;
			mp4_memset((char *)(esds->decoderConfig)+esds->decoderConfigLen, 0xff, tmplen);
		}
#endif		
		else 
		{
			esds->decoderConfigLen = 0;
			return MP4_MUX_DEMUX_MALLOC_ERROR;
		}
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	write esds structor information to file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	esds: point of the esds structor 
	esid: esds track's id
	objectType: 
	streamType: data stream type of codec
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_WRITE_FILE_ERROR
	MP4_MUX_DEMUX_MALLOC_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_esds_common(mp4_infor_t *file, mp4_esds_t *esds, uint32 esid, uint8 objectType, uint8 streamType)
{
	mp4_atom_t atom;
	uint32 length, result;
	
	if((file->audio_type != MP4_TYPE_AUDIO_IS_MP4A)&&(file->video_type != MP4_TYPE_VIDEO_IS_MP4V))
	{
		return MP4_MUX_DEMUX_RET_NOERR;
	}
	
	result = mp4_atom_write_header(file, &atom, "esds");
	if(result) return result;
	result = mp4_write_char(file, esds->version);
	if(result) return result;
	result = mp4_write_uint24(file, esds->flags);
	if(result) return result;
	
	result = mp4_write_char(file, 0x03);			/* ES_DescrTag */
	if(result) return result;
	length= 3 + (5 + (13 + (5 + esds->decoderConfigLen))) + 3;
	result = mp4_write_descr_length(file, length, 0);
	if(result) return result;
	
	result = mp4_write_uint16(file, esid);
	if(result) return result;
	result = mp4_write_char(file, 0x10);			/* streamPriorty = 16 (0-31) */
	if(result) return result;
	
	/* DecoderConfigDescriptor */
	result = mp4_write_char(file, 0x04);			/* DecoderConfigDescrTag */
	if(result) return result;
	length= 13 + (5 + esds->decoderConfigLen);
	result = mp4_write_descr_length(file, length, 0);
	if(result) return result;
	
	result = mp4_write_char(file, objectType); 		/* objectTypeIndication */
	if(result) return result;
	result = mp4_write_char(file, streamType); 	/* streamType */
	if(result) return result;
	result = mp4_write_uint24(file, 0);				/* buffer size */
	if(result) return result;
	result = mp4_write_uint32(file, 0);				/* max bitrate */
	if(result) return result;
	result = mp4_write_uint32(file, 0);				/* average bitrate */
	if(result) return result;
	
	result = mp4_write_char(file, 0x05);			/* DecSpecificInfoTag */
	if(result) return result;
	length= esds->decoderConfigLen;
	result = mp4_write_descr_length(file, length , 0);
	if(result) return result;
	result = mp4_write_data(file, (char*)esds->decoderConfig, length);
	if(result!=length) return MP4_MUX_WRITE_FILE_ERROR;
	
	/* SLConfigDescriptor */
	result = mp4_write_char(file, 0x06);			/* SLConfigDescrTag */
	if(result) return result;
	result = mp4_write_char(file, 0x01);			/* length */
	if(result) return result;
	result = mp4_write_char(file, 0x02);			/* constant in mp4 files */
	if(result) return result;
	
	/* no IPI_DescrPointer */
	/* no IP_IdentificationDataSet */
	/* no IPME_DescriptorPointer */
	/* no LanguageDescriptor */
	/* no QoS_Descriptor */
	/* no RegistrationDescriptor */
	/* no ExtensionDescriptor */

	return mp4_atom_write_footer(file, &atom);
}

uint32 mp4_write_esds_audio(mp4_infor_t *file, mp4_esds_t *esds, uint32 esid)
{
	return mp4_write_esds_common(file, esds, esid, (uint8)0x40, (uint8)0x15);
}

uint32 mp4_write_esds_video(mp4_infor_t *file, mp4_esds_t *esds, uint32 esid)
{
	return mp4_write_esds_common(file, esds, esid, (uint8)0x20, (uint8)0x11);
}

/*******************************************************************************************
Description:
	initialize h263 head information
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	d263: point of the d263 information structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_d263_init(mp4_d263_t *d263)
{
	mp4_copy_char32(d263->vendor, "    ");
	d263->version = 0;			/* 1 bytes in file */
	d263->h263level = 0;			/* 1 bytes in file */
	d263->h263profile = 0;		/* 1 bytes in file */
}

void mp4_d263_delete(mp4_d263_t *d263)
{

}

void mp4_d263_dump(mp4_d263_t *d263)
{
	MP4_DEMUX_MSG((" d263:\n"));
	MP4_DEMUX_MSG((" vendor %c%c%c%c\n", d263->vendor[0], d263->vendor[1], d263->vendor[2], d263->vendor[3]));
	MP4_DEMUX_MSG((" version %d\n", d263->version));
	MP4_DEMUX_MSG((" h263level %d\n", d263->h263level));
	MP4_DEMUX_MSG((" h263profile %d\n", d263->h263profile));
	MP4_DEMUX_MSG(("\n"));
}

/*******************************************************************************************
Description:
	read h263 video decoder config information form file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	d263: point of the d263 structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_d263_video(mp4_infor_t *file, mp4_d263_t *d263)
{
	uint32 result;
	
	result = mp4_read_data(file, d263->vendor, 4);
	if(result!=4) return MP4_DEMUX_READ_FILE_ERROR;
	result = mp4_read_char(file, &d263->version);
	if(result) return result;
	result = mp4_read_char(file, &d263->h263level);
	if(result) return result;
	return mp4_read_char(file, &d263->h263profile);
}

/*******************************************************************************************
Description:
	write d263 video decoder config information to file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	d263: point of the d263 structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_d263_video(mp4_infor_t *file, mp4_d263_t *d263)
{
	mp4_atom_t atom;
	uint32 result;
	
	result = mp4_atom_write_header(file, &atom, "d263");
	if(result) return result;
	result = mp4_write_data(file, d263->vendor, 4);
	if(result!=4) return MP4_MUX_WRITE_FILE_ERROR;
	result = mp4_write_char(file, d263->version);
	if(result) return result;
	result = mp4_write_char(file, d263->h263level);
	if(result) return result;
	result = mp4_write_char(file, d263->h263profile);
	return mp4_atom_write_footer(file, &atom);
}

/*******************************************************************************************
Description:
	initialize damr head information
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	damr: point of the damr structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_damr_init(mp4_damr_t *damr)
{
	mp4_copy_char32(damr->vendor, "    ");
	damr->version = 0;
	damr->mode_set = 0;
	damr->mode_change_period = 0;
	damr->frame_per_sample = 5;
}

void mp4_damr_delete(mp4_damr_t *damr)
{

}

void mp4_damr_dump(mp4_damr_t *damr)
{
	MP4_DEMUX_MSG((" damr:\n"));
	MP4_DEMUX_MSG((" vendor %c%c%c%c\n", damr->vendor[0], damr->vendor[1], damr->vendor[2], damr->vendor[3]));
	MP4_DEMUX_MSG((" version %d\n", damr->version));
	MP4_DEMUX_MSG((" mode_set %d\n", damr->mode_set));
	MP4_DEMUX_MSG((" mode_change_period %d\n", damr->mode_change_period));
	MP4_DEMUX_MSG((" frame_per_sample %d\n", damr->frame_per_sample));
	MP4_DEMUX_MSG(("\n"));
}

/*******************************************************************************************
Description:
	read damr audio decoder config information form file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	damr: point of the damr structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_damr_audio(mp4_infor_t *file, mp4_damr_t *damr)
{
	uint32 result;
	
	result = mp4_read_data(file, damr->vendor, 4);
	if(result!=4) return MP4_DEMUX_READ_FILE_ERROR;
	result = mp4_read_char(file, &damr->version);
	if(result) return result;
	result = mp4_read_uint16(file, &damr->mode_set);
	if(result) return result;
	result = mp4_read_char(file, &damr->mode_change_period);
	if(result) return result;
	return mp4_read_char(file, &damr->frame_per_sample);
}

/*******************************************************************************************
Description:
	write damr audio decoder config information to file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	damr: point of the damr structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_damr_audio(mp4_infor_t *file, mp4_damr_t *damr)
{
	mp4_atom_t atom;
	uint32 result;
	
	result = mp4_atom_write_header(file, &atom, "damr");
	if(result) return result;
	result = mp4_write_data(file, damr->vendor, 4);
	if(result!=4) return MP4_MUX_WRITE_FILE_ERROR;
	result = mp4_write_char(file, damr->version);
	if(result) return result;
	result = mp4_write_uint16(file, damr->mode_set);
	if(result) return result;
	result = mp4_write_char(file, damr->mode_change_period);
	if(result) return result;
	result = mp4_write_char(file, damr->frame_per_sample);
	return mp4_atom_write_footer(file, &atom);
}

/*******************************************************************************************
Description:
	analysize mp4v decoder config, for checking video vop width and height
Parameters:	
	unsigned char* decoderConfig: decoder config buffer point address 
	uint32 decoderConfigLen: decoder config data length
	mp4_mp4v_info* info: mp4v header information structor point
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2008.03.10		create first version
********************************************************************************************/
#define MP4_VIDOBJ_START_CODE				0x00000100	
#define MP4_VIDOBJLAY_START_CODE			0x00000120	
#define MP4_VISOBJSEQ_START_CODE			0x000001b0
#define MP4_VISOBJSEQ_STOP_CODE			0x000001b1	
#define MP4_USERDATA_START_CODE			0x000001b2
#define MP4_GRPOFVOP_START_CODE			0x000001b3
#define MP4_VISOBJ_START_CODE				0x000001b5
#define MP4_VOP_START_CODE				0x000001b6
#define MP4_VISOBJ_TYPE_VIDEO				1
#define MP4_VIDOBJLAY_SHAPE_RECTANGULAR	0
#define MP4_VIDOBJLAY_SHAPE_BINARY		1
#define MP4_VIDOBJLAY_SHAPE_BINARY_ONLY	2
#define MP4_VIDOBJLAY_SHAPE_GRAYSCALE	3
#define MP4_VIDOBJLAY_AR_EXTPAR			15
#define MP4_VIDOBJLAY_START_CODE_MASK	0x0000000f
#define MP4_VIDOBJ_START_CODE_MASK		0x0000001f
#define MP4_SHORT_VIDEO_START_MARKER	0xfffffc00 

#define MP4_SPRITE_NONE		0
#define MP4_SPRITE_STATIC		1
#define MP4_SPRITE_GMC			2

uint32 mp4_analysize_mp4v_decoderconfig(unsigned char * decoderConfig, uint32 *decoderConfigLen, mp4_mp4v_info* info)
{
	uint32 tmpvalue, bit_offset;
#if MP4_IN_820_DECODE_HARDWARE
	uint32 voptic_bitoffset = 0;
#endif
	if (!decoderConfigLen) 
		return MP4_MUX_DEMUX_RET_NOERR;
	
	info->visual_object_start_code = 0;				// 32b 
	info->visual_object_ver_id = 0;					// 4 bslbf in file
	info->visual_object_priority = 0;					// 3 bslbf in file
	info->video_object_layer_verid = 0;				// 4 bslbf in file
	info->video_object_layer_priority = 0;			// 3 bslbf in file
	info->video_object_layer_shape = 0;				// 2 bslbf in file
	info->vop_time_increment_resolution = 0;			// 16 bslbf in file
	info->fixed_vop_rate = 0;
	info->fixed_vop_time_increment = 0;
	bit_offset = 0;
	while(1) 
	{
		/* visual_object_start_code, 32 bit */
		bit_offset += 7;
		bit_offset &=0xfffffff8;
		info->visual_object_start_code = mp4_bitstream_get_bits(decoderConfig,32,bit_offset);
		bit_offset += 32;
		if (info->visual_object_start_code == MP4_VISOBJSEQ_START_CODE) 
		{// 1.0
			MP4_DEMUX_DBG(("<visual_object_sequence start>"));
			/* profile_and_level_indication, 8 bit */
			tmpvalue = mp4_bitstream_get_bits(decoderConfig,8,bit_offset);
			bit_offset += 8;
			MP4_DEMUX_DBG(("profile_and_level_indication = %d\n", tmpvalue));
		} 
		else if (info->visual_object_start_code == MP4_VISOBJSEQ_STOP_CODE) 
		{// 1.1
			MP4_DEMUX_DBG(("<visual_object_sequence stop>"));
			break;
		} 
		else if (info->visual_object_start_code == MP4_VISOBJ_START_CODE) 
		{// 1.2
			/*vol level process. Amanda Deng, 2008.03.10*/
			MP4_DEMUX_DBG(("<visual_object>"));
			/* is_visual_object_identified, 1 bit */
			tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
			bit_offset ++;
			if (tmpvalue) 
			{// 1.2.1	
				/* visual_object_ver_id, 4 bit */
				info->visual_object_ver_id = mp4_bitstream_get_bits(decoderConfig,4,bit_offset);
				bit_offset += 4;
				/* visual_object_priority, 3 bit */
				info->visual_object_priority = mp4_bitstream_get_bits(decoderConfig,3,bit_offset);
				bit_offset += 3;
			} 
			else 
			{
				info->visual_object_ver_id = 1;
				info->visual_object_priority = 0;
			}
			MP4_DEMUX_DBG(("visual_object_ver_id=%d, visual_object_priority = %d", info->visual_object_ver_id, info->visual_object_priority));
			
			/* visual_object_type, 4 bit */
			tmpvalue = mp4_bitstream_get_bits(decoderConfig,4,bit_offset);
			bit_offset += 4;
			MP4_DEMUX_DBG(("visual_object_type: = %d", tmpvalue));
			if (tmpvalue!= MP4_VISOBJ_TYPE_VIDEO)	
			{// 1.2.2
				MP4_DEMUX_DBG(("visual_object_type != video\n"));
				break;
			}
			
			/* video_signal_type, 1 bit */
			tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
			bit_offset += 1;
			if (tmpvalue) 
			{// 1.2.3
				/* video_format, 3 bit */
				bit_offset += 3;
				/* video_range, 1 bit */
				bit_offset += 1;
				/* color_description, 1 bit */
				tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
				bit_offset += 1;
				if(tmpvalue) 
				{// 1.2.3.1
					/* color_primaries, 8 bit */
					bit_offset += 8;
					/* transfer_characteristics, 8 bit */
					bit_offset += 8;
					/* matrix_coefficients, 8 bit */
					bit_offset += 8;
				}// 1.2.3.1 end (color_description==1)
			}// 1.2.3 end (video_signal_type==1)
		} // 1.2
		else if ((info->visual_object_start_code & ~MP4_VIDOBJ_START_CODE_MASK) == MP4_VIDOBJ_START_CODE) 
		{// 1.3
			MP4_DEMUX_DBG(("<video_object>"));		
		} 
		else if ((info->visual_object_start_code & ~MP4_VIDOBJLAY_START_CODE_MASK) == MP4_VIDOBJLAY_START_CODE) 
		{// 1.4
			MP4_DEMUX_DBG(("<video_object_layer>"));
			MP4_DEMUX_DBG(("video_object_layer_id: vol id = %d", info->visual_object_start_code & MP4_VIDOBJLAY_START_CODE_MASK));
			/* random_accessible_vol, 1 bit */
			bit_offset += 1;
			/* video_object_type_indication, 8 bit */
			bit_offset += 8;
			/* is_object_layer_identifier, 1 bit */
			tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
			bit_offset += 1;
			if (tmpvalue)	
			{// 1.4.1
				MP4_DEMUX_DBG(("<is_object_layer_identifier>"));
				/* video_object_layer_verid, 4 bit */
				info->video_object_layer_verid = mp4_bitstream_get_bits(decoderConfig,4,bit_offset);
				bit_offset += 4;
				/* video_object_layer_priority, 3 bit */
				info->video_object_layer_priority = mp4_bitstream_get_bits(decoderConfig,3,bit_offset);
				bit_offset += 3;
			} // 1.4.1 end (is_object_layer_identifier==1)
			else 
			{// 1.4.1
				info->video_object_layer_verid = info->visual_object_ver_id;
				info->video_object_layer_priority = 0;
			}// 1.4.1
			MP4_DEMUX_DBG(("video_object_layer_verid = %d\n", info->video_object_layer_verid));

			/* aspect_ratio_info, 4 bit */
			tmpvalue = mp4_bitstream_get_bits(decoderConfig,4,bit_offset);
			bit_offset += 4;
			if (tmpvalue == MP4_VIDOBJLAY_AR_EXTPAR)	
			{// 1.4.2
				MP4_DEMUX_DBG(("<VIDOBJLAY_AR_EXTPAR>"));
				/* par_width, 8 bit */
				bit_offset += 8;
				/* par_height, 8 bit */
				bit_offset += 8;
			}// 1.4.2 	
			
			/* vol_control_parameters, 1 bit */
			tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
			bit_offset += 1;
			MP4_DEMUX_DBG(("vol_control_parameters = %d", tmpvalue));
			if (tmpvalue)	
			{// 1.4.3
				MP4_DEMUX_DBG(("<vol_control_parameters>"));
				/* chroma_format, 2 bit */
				bit_offset += 2;
				/* low_delay, 1 bit */
				bit_offset += 1;
				/* vbv_parameters, 1 bit */
				tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
				bit_offset += 1;
				MP4_DEMUX_DBG(("vbv_parameters = %d", tmpvalue));
				if (tmpvalue)	
				{// 1.4.3.1
					MP4_DEMUX_DBG(("<vbv_parameters>"));
					/* bit_rate = (first_half_bit_rate <<15+latter_half_bit_rate)*400*/
					/* first_half_bit_rate, 15 bit */
					/* marker_bit, 1 bit */
					/* latter_half_bit_rate, 15 bit */
					/* marker_bit, 1 bit */
					bit_offset += 32;

					/* buffer_size = first_half_vbv_buffer_size <<3+info->latter_half_vbv_buffer_size*/
					/* first_half_vbv_buffer_size, 15 bit */
					/* marker_bit, 1 bit */
					/* latter_half_vbv_buffer_size, 3 bit */
					bit_offset += 19;

					/* vbv_occupancy = first_half_vbv_occupancy <<15+latter_half_vbv_occupancy*/
					/* first_half_vbv_occupancy, 11 bit */
					/* marker_bit, 1 bit */
					/* latter_half_vbv_occupancy, 15 bit */
					/* marker_bit, 1 bit */
					bit_offset += 28;
				}// 1.4.3.1	
			}// 1.4.3 
			
			/* video_object_layer_shape, 2 bit */
			info->video_object_layer_shape = mp4_bitstream_get_bits(decoderConfig,2,bit_offset);
			bit_offset += 2;
			MP4_DEMUX_DBG(("video_object_layer_shape = %d",info->video_object_layer_shape));

			if (info->video_object_layer_shape != MP4_VIDOBJLAY_SHAPE_RECTANGULAR)
			{// 1.4.4
				MP4_DEMUX_DBG(("non-rectangular shapes are not supported"));
			}
			if (info->video_object_layer_shape == MP4_VIDOBJLAY_SHAPE_GRAYSCALE && info->video_object_layer_verid != 1) 
			{// 1.4.5
				/* video_object_layer_shape_extension, 4 bit */
				tmpvalue = mp4_bitstream_get_bits(decoderConfig,4,bit_offset);
				bit_offset += 4;
			}
			/* marker_bit, 1 bit */
			bit_offset ++;

			/********************** for decode B-frame time ***********************/
			#if MP4_IN_820_DECODE_HARDWARE
			voptic_bitoffset = bit_offset;
			#endif
			/* vop_time_increment_resolution, 16 bit */
			tmpvalue = mp4_bitstream_get_bits(decoderConfig,16,bit_offset);
			bit_offset += 16;
			MP4_DEMUX_DBG(("vop_time_increment_resolution = %d\n", tmpvalue));
			if (tmpvalue > 0) 
			{// 1.4.6
				/* decode vop_time_increment_resolution */
				info->vop_time_increment_resolution = MAX(mp4_log2bin(tmpvalue-1), 1);
			}
			else 
			{
				/* for "old" xvid compatibility, set time_inc_bits = 1 */
				info->vop_time_increment_resolution = 1;
			}
			
			/* marker_bit, 1 bit */
			bit_offset ++;
			/* fixed_vop_rate, 1 bit  */
			info->fixed_vop_rate = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
			bit_offset += 1;
			if (info->fixed_vop_rate)	
			{// 1.4.7
				/* fixed_vop_time_increment, bit count = vop_time_increment_resolution */
				info->fixed_vop_time_increment = mp4_bitstream_get_bits(decoderConfig,info->vop_time_increment_resolution,bit_offset);
				bit_offset += info->vop_time_increment_resolution;
			}
			#if MP4_IN_820_DECODE_HARDWARE
			/*because VC0820 can not support 16 bit vop_time_increment_resolution, so need change to 8 bit if it's 16 bit */
			if(info->vop_time_increment_resolution==16)
			{
				mp4_bitstream_set_bits(decoderConfig, 0xff, 16, voptic_bitoffset);
				if (info->fixed_vop_rate)
				{
					//change fixed_vop_time_increment form 16 bit to 8 bit
					voptic_bitoffset += 18;
					mp4_bitstream_set_bits(decoderConfig, 0xff, 16, voptic_bitoffset);
					//delete 8 bit form stream
					mp4_bitstream_delete_bits(decoderConfig, decoderConfigLen, 8, voptic_bitoffset);
					bit_offset -= 8;
				}
			}
			#endif
			//get width and height
			if (info->video_object_layer_shape != MP4_VIDOBJLAY_SHAPE_BINARY_ONLY) 
			{// 1.4.8
				if (info->video_object_layer_shape == MP4_VIDOBJLAY_SHAPE_RECTANGULAR) 
				{// 1.4.8.1
					/* marker_bit, 1 bit */
					bit_offset ++;
					/* video_object_layer_width, 13 bit */
					info->video_object_layer_width = mp4_bitstream_get_bits(decoderConfig,13,bit_offset);
					/* because 820 need width = 16*n*/
					tmpvalue = (info->video_object_layer_width+15)/16;
					tmpvalue *= 16;
					if(tmpvalue!=info->video_object_layer_width)
					{
						info->video_object_layer_width = tmpvalue;
						if((*decoderConfigLen*8)>=(bit_offset+13))
							{mp4_bitstream_set_bits(decoderConfig, tmpvalue, 13, bit_offset);}
					}
					bit_offset += 13;
					/* marker_bit, 1 bit */
					bit_offset ++;
					/* video_object_layer_height, 13 bit */
					info->video_object_layer_height = mp4_bitstream_get_bits(decoderConfig,13,bit_offset);
					/* because 820 need height = 16*n*/
					tmpvalue = (info->video_object_layer_height+15)/16;
					tmpvalue *= 16;
					if(tmpvalue!=info->video_object_layer_height)
					{
						info->video_object_layer_height = tmpvalue;
						if((*decoderConfigLen*8)>=(bit_offset+13))
							{mp4_bitstream_set_bits(decoderConfig, tmpvalue, 13,bit_offset);}
					}
					bit_offset += 13;
					/* marker_bit, 1 bit */
					bit_offset ++;
					MP4_DEMUX_DBG(("video_object_layer_width = %i\n", info->video_object_layer_width));
					MP4_DEMUX_DBG(("video_object_layer_height = %i\n", info->video_object_layer_height));
				}
				break;
				//cut some case because now need not process, Amanda Deng 2008.03.11
			}
			else 
			{  // 1.4.8
				/* info->video_object_layer_shape == VIDOBJLAY_SHAPE_BINARY_ONLY */		
				if (info->video_object_layer_verid != 1) 
				{// 1.4.8.14
					/* scalability, 1 bit */
					tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
					bit_offset += 1;
					MP4_DEMUX_DBG(("scalability = %d",tmpvalue));		
					if (tmpvalue)
					{
						MP4_DEMUX_DBG(("scalability not supported"));
						/* ref_layer_id, 4 bit */
						/* hor_sampling_factor_n, 5 bit */
						/* hor_sampling_factor_m, 5 bit */
						/* vert_sampling_factor_n, 5 bit */
						/* vert_sampling_factor_m, 5 bit */
						bit_offset += 24;
						break;	
					}
				} 
				/* resync_marker_disable, 1 bit */
				bit_offset += 1;
			} // 1.4.8

			/* VOL */
			break;	
		} // 1.4
		else 
		{ // 1.5
			/* start_code == ? */
			if ((info->visual_object_start_code & 0xffffff00)>>8 == 0x000001) 
			{
				MP4_DEMUX_DBG(("<unknown: %x>", info->visual_object_start_code));
			}
		} // 1.5
	} //end of while(1)
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_analysize_mp4v_streamheader(unsigned char * streamdata,uint32* datalength, mp4_mp4v_info* info)
{
	uint32 tmpvalue, bit_offset;
	
	if (!streamdata) 
		return MP4_MUX_DEMUX_RET_NOERR;

	#if MP4_IN_820_DECODE_HARDWARE
	/*because VC0820 can not support 16 bit vop_time_increment_resolution, so need change to 8 bit if it's 16 bit */
	if(info->vop_time_increment_resolution<16)
	{
		return MP4_MUX_DEMUX_RET_NOERR;		
	}
	#endif
	
	bit_offset = 0;
	while(1) 
	{
		/* visual_object_start_code, 32 bit */
		bit_offset += 7;
		bit_offset &=0xfffffff8;
		tmpvalue = mp4_bitstream_get_bits(streamdata,32,bit_offset);
		bit_offset += 32;
		if (tmpvalue == MP4_GRPOFVOP_START_CODE) 
		{// 1.0
			/*gop level process. Amanda Deng, 2008.04.15*/
			MP4_DEMUX_DBG(("<group_of_vop_plan start>"));
			/* time_code, 18 bit */
			bit_offset += 18;
			/* closed_gov, 1 bit */
			bit_offset ++;
			/* broken_link, 1 bit */
			bit_offset ++;
		} 
//		else 		//by dnn 2008/7/23 
		if (tmpvalue == MP4_VOP_START_CODE) 
		{// 1.1
			/*vop level process. Amanda Deng, 2008.04.15*/
			MP4_DEMUX_DBG(("<visual_object_plane start>"));
			/* vop_coding_type, 2 bit */
			tmpvalue = mp4_bitstream_get_bits(streamdata,2,bit_offset);
			bit_offset +=2;

			/* temporal time base */
			do
			{
				/* modulo_time_base, 1 bit */
				tmpvalue = mp4_bitstream_get_bits(streamdata,1,bit_offset);
				//modulo_time_base++;
				bit_offset ++;
			}while(tmpvalue);
	
			/* marker_bit, 1 bit */
			bit_offset ++;
	
			/* vop_time_increment, 1~16 bit */
			#if MP4_IN_820_DECODE_HARDWARE
			/*because VC0820 can not support 16 bit vop_time_increment_resolution, so need change to 8 bit if it's 16 bit */
			if(info->vop_time_increment_resolution==16)
			{
				//delete 8 bit form stream
				mp4_bitstream_delete_bits(streamdata, datalength, 8, bit_offset);
				//change fixed_vop_time_increment form 16 bit to 8 bit
				mp4_bitstream_set_bits(streamdata, 0xff, 8, bit_offset);
				bit_offset -= 8;
			}
			#endif
			/*.......now need not process ..... Amanda Deng*/
			break;
		} 
		else 
		{ // 1.2
			/* start_code == ? */
			if ((tmpvalue & 0xffffff00)>>8 == 0x000001) 
			{
				MP4_DEMUX_DBG(("<unknown: %x>", info->visual_object_start_code));
			}
			break;
		} // 1.2
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}


//cut( form 774 line) some case because now need not process, Amanda Deng 2008.03.11
#if 0
/* interlacing, 1 bit */
bit_offset += 1;
/* obmc_disable, 1 bit */
tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
bit_offset += 1;
MP4_DEMUX_DBG(("obmc_disabled = %d",tmpvalue));
if (!tmpvalue)	
{// 1.4.8.2
	MP4_DEMUX_DBG(("obmc_disabled==false not supported"));
	/* fucking divx4.02 has this enabled */
}
/* sprite_enable, 1/2 bit */
tmpvalue = (info->video_object_layer_verid == 1 ? 1 : 2);
info->sprite_enable = mp4_bitstream_get_bits(decoderConfig,tmpvalue,bit_offset);
bit_offset += tmpvalue;
if (info->sprite_enable) 
{// 1.4.8.3
	MP4_DEMUX_DBG(("sprite_enable==true not supported"));
}
MP4_DEMUX_DBG(("sprite_enable = %d", info->sprite_enable));

if (info->sprite_enable == SPRITE_STATIC || info->sprite_enable == SPRITE_GMC)
{// 1.4.8.4
	if (info->sprite_enable != SPRITE_GMC)
	{// 1.4.8.4.1
		/* sprite_width, 13 bit */
		/* marker_bit, 1 bit */
		/* sprite_height, 13 bit */
		/* marker_bit, 1 bit */
		tmpvalue = mp4_bitstream_get_bits(decoderConfig,28,bit_offset);
		bit_offset += 28;
		
		/* sprite_left_coordinate, 13 bit */
		/* marker_bit, 1 bit */
		/* sprite_top_coordinate, 13 bit */
		/* marker_bit, 1 bit */
		tmpvalue = mp4_bitstream_get_bits(decoderConfig,28,bit_offset);
		bit_offset += 28;
	}
	/* no_of_sprite_warping_points, 6 bit */
	bit_offset += 6;
	/* sprite_warping_accuracy, 2 bit */
	bit_offset += 2;
	/* brightness_change, 1 bit */
	bit_offset += 1;

	if (info->sprite_enable != SPRITE_GMC)
	{// 1.4.8.4.2
		/* low_latency_sprite_enable */
		tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
		bit_offset += 1;
	}
}// 1.4.8.4

if (info->video_object_layer_verid != 1 && info->video_object_layer_shape != VIDOBJLAY_SHAPE_RECTANGULAR) 
{// 1.4.8.5
	/* sadct_disable, 1 bit */
	tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
	bit_offset += 1;
	MP4_DEMUX_DBG(("sadct_disable = %d",tmpvalue));
}

/* not_8_bit , 1 bit*/
tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
bit_offset += 1;
MP4_DEMUX_DBG(("not_8_bit = %d",tmpvalue));
if (tmpvalue)	
{// 1.4.8.6
	MP4_DEMUX_DBG(("not_8_bit==true (ignored)"));
	/* quant_precision, 4 bit */
	info->quant_bits = mp4_bitstream_get_bits(decoderConfig,4,bit_offset);
	bit_offset += 4;
	/* bits_per_pixel */
	bit_offset += 4;
} 
else 
{
	info->quant_bits = 5;
}	

if (info->video_object_layer_shape == VIDOBJLAY_SHAPE_GRAYSCALE) 
{// 1.4.8.7
	/* no_gray_quant_update, 1 bit */
	bit_offset ++;
	/* composition_method, 1 bit */
	bit_offset ++;
	/* linear_composition, 1 bit */
	bit_offset ++;
}

/* quant_type, 1 bit */
tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
bit_offset += 1;
MP4_DEMUX_DBG(("quant_type = %i\n", tmpvalue));
if (tmpvalue) 
{// 1.4.8.8
	/* load_intra_quant_mat, 1 bit */
	tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
	bit_offset += 1;
	if (tmpvalue)	
	{// 1.4.8.8.1
		uint8 matrix[64];
		result = bs_get_matrix(bs, matrix);
		if (result) {
			return result;
		}
	} 

	/* load_inter_quant_mat, 1 bit */
	result = BitstreamGetBits(bs,1,&data,&readbits);
	tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
	bit_offset += 1;
	if (data)	
	{// 1.4.8.8.2
		uint8 matrix[64];
		result = bs_get_matrix(bs, matrix);
		if (result) {
			return result;
		}
	}
	
	if (info->video_object_layer_shape == VIDOBJLAY_SHAPE_GRAYSCALE) 
	{// 1.4.8.8.3
		MP4_DEMUX_DBG(("greyscale matrix not supported"));
		return -1;
	}
}

if (info->video_object_layer_verid != 1)
{// 1.4.8.9
	/* quarter_sample, 1 bit */
	info->quarter_sample = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
	bit_offset += 1;
	MP4_DEMUX_DBG(("quarterpel = %d", info->quarter_sample));
}
else
{
	info->quarter_sample = 0;
}

/* complexity estimation disable, 1 bit */
tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
bit_offset += 1;
MP4_DEMUX_DBG(("complexity_estimation_disable = %d", tmpvalue));
if (!tmpvalue)
{// 1.4.8.10
	MP4_DEMUX_DBG(("define_vop_complexity_estimation_header()-->"));
	result = read_vol_complexity_estimation_header(bs, info);
	if(result) return result;
	MP4_DEMUX_DBG(("end of define_vop_complexity_estimation_header()"));
}

/* resync_marker_disable, 1 bit */
bit_offset += 1;

/* data_partitioned, 1 bit */
tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
bit_offset += 1;
MP4_DEMUX_DBG(("data_partitioned = %d",tmpvalue));
if (tmpvalue)	
{// 1.4.8.11
	MP4_DEMUX_DBG(("data_partitioned not supported"));
	/* reversible_vlc, 1 bit */
	bit_offset += 1;
}

if (info->video_object_layer_verid != 1) 
{// 1.4.8.12
	/* newpred_enable, 1 bit */
	tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
	bit_offset += 1;
	MP4_DEMUX_DBG(("newpred_enable = %d",tmpvalue));
	if (tmpvalue)	
	{
		/* requested_upstream_message_type, 2 bit */
		bit_offset += 2;
		/* newpred_segment_type, 1 bit */
		bit_offset += 1;
	}
	/* reduced_resolution_vop_enable, 1 bit */
	tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
	bit_offset += 1;
	MP4_DEMUX_DBG(("reduced_resolution_vop_enable = %d", tmpvalue));
} 
else 
{
	MP4_DEMUX_DBG(("newpred_enable = 0"));
	MP4_DEMUX_DBG(("reduced_resolution_vop_enable = 0"));
} 

/* scalability, 1 bit */
tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
bit_offset += 1;
MP4_DEMUX_DBG(("scalability = %d",tmpvalue));		
if (tmpvalue)
{// 1.4.8.13
	MP4_DEMUX_DBG(("scalability not supported"));
	/* hierarchy_type, 1 bit */
	tmpvalue = mp4_bitstream_get_bits(decoderConfig,1,bit_offset);
	bit_offset += 1;
	/* ref_layer_id , 4 bit */
	/* ref_layer_sampling_direc , 1 bit */
	/* hor_sampling_factor_n , 5 bit */
	/* hor_sampling_factor_m , 5 bit */
	/* vert_sampling_factor_n , 5 bit */
	/* vert_sampling_factor_m , 5 bit */
	/* enhancement_type , 1 bit */
	bit_offset += 26;
	
	if(info->shape == VIDOBJLAY_SHAPE_BINARY && tmpvalue==0 ) 
	{
		/* use_ref_shape , 1 bit */
		/* use_ref_texture , 1 bit */
		/* shape_hor_sampling_factor_n , 5 bit */
		/* shape_hor_sampling_factor_m , 5 bit */
		/* shape_vert_sampling_factor_n , 5 bit */
		/* shape_vert_sampling_factor_m , 5 bit */
		bit_offset += 22;
	}
} 
#endif


/*******************************************************************************************
Description:
	analysize h263 stream header, for checking video vop width and height
Parameters:	
	char* streamdata: video one frame stream data start point
	mp4_h263_info* info: h263 header inforamtion structor point
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2008.03.10		create first version
********************************************************************************************/
#define MP4_H263_SOURCE_FORMAT_BITS		0x1C
#define MP4_H263_CODING_TYPE_BITS			0x02

#define MP4_H263_FORMAT_SUBQCIF			1		//128x96
#define MP4_H263_FORMAT_QCIF				2		//176x144	
#define MP4_H263_FORMAT_CIF				3		//352x288
#define MP4_H263_FORMAT_4CIF				4		//704x576
#define MP4_H263_FORMAT_16CIF				5		//1408x1152

uint32 mp4_analysize_h263_streamheader(unsigned char * streamdata,uint32* datalength,mp4_h263_info* info)
{
	if (streamdata==NULL) 
		return MP4_MUX_DEMUX_RET_NOERR;
	
	info->short_video_start_marker = 0;			/*  22 bslbf in data stream start  
												000080xx~000083xx*/
 	info->temporal_reference = 0;				/*  8 uimsbf in data stream start  */
	info->marker_bit = 0;						/*  1 bslbf in data stream start  */
	info->one_reserved_zero_bits = 0;			/*  1 bslbf in data stream start, one word end*/
	info->split_screen_indicator = 0;				/*  1 bslbf in data stream start  */
	info->document_camera_indicator = 0;		/*  1 bslbf in data stream start  */
	info->full_picture_freeze_release = 0;		/*  1 bslbf in data stream start  */
	info->source_format = 0;					/*  3 bslbf in data stream start  
											    000, 110, 111 reserved 
											    001: sub-QCIF 128x96
											    010: QCIF 176x144 
											    011: CIF 352x288
											    100: 4CIF 704x576
											    101: 16CIF 1408x1152*/
	info->picture_coding_type = 0;				/*  1 bslbf in data stream start  
											    0: the vop is I
											    1: the vop is P*/
	streamdata++;
	streamdata++;
	streamdata++;
	streamdata++;
	info->source_format = (*streamdata) & MP4_H263_SOURCE_FORMAT_BITS;
	info->picture_coding_type = (*streamdata) & MP4_H263_CODING_TYPE_BITS;
	switch(info->source_format ) 
	{
		case MP4_H263_FORMAT_SUBQCIF:
			info->video_object_layer_width = 128;
			info->video_object_layer_height = 96;
			break;
		case MP4_H263_FORMAT_QCIF:
			info->video_object_layer_width = 176;
			info->video_object_layer_height = 144;
			break;
		case MP4_H263_FORMAT_CIF:
			info->video_object_layer_width = 352;
			info->video_object_layer_height = 288;
			break;
		case MP4_H263_FORMAT_4CIF:
			info->video_object_layer_width = 704;
			info->video_object_layer_height = 576;
			break;
		case MP4_H263_FORMAT_16CIF:
			info->video_object_layer_width = 1408;
			info->video_object_layer_height = 1152;
			break;
		default:
			break;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}




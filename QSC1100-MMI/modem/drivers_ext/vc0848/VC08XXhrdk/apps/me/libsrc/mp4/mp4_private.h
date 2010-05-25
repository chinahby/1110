/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	private.h 
* User for : 	mp4 demux mux, 
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/
#ifndef PRIVATE_H
#define PRIVATE_H

#define MP4LIB_VERSION2_FOREHANDLE	1

/* ================================= structures */

#define HEADER_LENGTH 		8
#define MAXTRACKS 			10
#define DEFAULT_TRACK 		0
#define VIDEORECORDERIN848 		1


#define MP4_TYPE_AUDIO_IS_SAMR	1				/* file audio type  is amr */
#define MP4_TYPE_AUDIO_IS_MP4A	2				/* file audio type is MP4 (acc) */
#define MP4_TYPE_VIDEO_IS_MP4V	3				/* file video type is MP4V */
#define MP4_TYPE_VIDEO_IS_S263	4				/* file video type is H263(s263) */

typedef struct
{
	float 	values[9];
} mp4_matrix_t;

/*-------------------- track header information-----------------*/
typedef struct
{
	uint32 	version;				/* 1 bytes in file */
	uint32  	flags;				/* 3 bytes in file */
	uint32  	creation_time;		/* 4/8 bytes in file, v=0, 4 bytes, v=1, 8 bytes */
	uint32  	modification_time;	/* 4/8 bytes in file, v=0, 4 bytes, v=1, 8 bytes */
	uint32 	track_id;			/* 4 bytes in file */
	uint32  	reserved1;			/* 4 bytes in file */
	uint32  	duration;			/* 4/8 bytes in file, v=0, 4 bytes, v=1, 8 bytes */
	char 	reserved2[8];		/* 8 bytes in file */
	uint32 	layer;				/* 2 bytes in file */
	uint32 	alternate_group;		/* 2 bytes in file */
	float 	volume;				/* 2 bytes in file, audio = 0x0100, video =0 */
	uint32  	reserved3;			/* 2 bytes in file */
	mp4_matrix_t matrix;			/* 4*9 bytes in file */
	float 	track_width;			/* 4 bytes in file */
	float 	track_height;		/* 4 bytes in file */
	uint32 	is_video;			/* define by myself, not from file */
	uint32 	is_audio;			/* define by myself, not from file */
} mp4_tkhd_t;

/*{  stbl structure:
		********************************+
		|  stsd description                  		|
		--------------------------------
		|         stsd table					|
		|			mp4_esds_t (&)	       |
		|			mp4_d263_t (&)	       |
		|			mp4_damr_t (&)	       |
		********************************+
		|  stts description					|                  		
		--------------------------------
		|         stts table					|
		********************************+
		|  stsc description					|                  		
		--------------------------------
		|         stsc table					|
		********************************+
		|  stco description 					|                 		
		--------------------------------
		|         stco table					|
		********************************+
		|  stsz description					|                  		
		--------------------------------
		|         stsz table					|
		********************************+
		|  stss description					|                  		
		--------------------------------
		|         stss table					|
		********************************   }*/

/*---------- mp4a & mp4v decode config information----------*/
typedef struct
{
	uint32  version;					/* 1 bytes in file */				
	uint32  flags;						/* 3 bytes in file */
	uint32  object_type;				/* 1 bytes in file */
	uint32  stream_type;				/* 1 bytes in file */
	uint32  decoderConfigLen;			/* 1 bytes in file */
	unsigned char * decoderConfig;		/* 0-255 bytes in file */
} mp4_esds_t;

/*--------------- d263 decode config information-----------*/
typedef struct
{
	char     vendor[4];				/* 4 bytes in file */
	uint32  version;					/* 1 bytes in file */
	uint32  h263level;				/* 1 bytes in file */
	uint32  h263profile;				/* 1 bytes in file */
} mp4_d263_t;

/*--------------- damr decode config information -----------*/
typedef struct
{
	char	    vendor[4];				/* 4 bytes in file */
	uint32  version;					/* 1 bytes in file */
	uint32  mode_set;				/* 2 bytes in file */
	uint32  mode_change_period;		/* 1 bytes in file */
	uint32  frame_per_sample;		/* 1 bytes in file */
} mp4_damr_t;

/*----------sample descriptions table (codec types, parameters etc.)----------*/
typedef struct
{
	char		format[4];			/* 4 bytes in file, samr/s263/mp4v/mp4a */	
	char		reserved[6];			/* 6 bytes in file */
	uint32	data_reference;		/* 2 bytes in file */
/* common to audio and video */
	uint32 	version;				/* 2 bytes in file */
	uint32 	revision;			/* 2 bytes in file */
	char 	vendor[4];			/* 4 bytes in file */
/* video description */
	uint32  	temporal_quality;
	uint32  	spatial_quality;
	uint32 	width;
	uint32 	height;
	float 	dpi_horizontal;
	float 	dpi_vertical;
	uint32  	data_size;
	uint32 	frames_per_sample;
	char 	compressor_name[32];
	uint32 	depth;				
	uint32 	ctab_id;
	float 	gamma;
	uint32 	fields;    			/* 0, 1, or 2 */
	uint32 	field_dominance;   	/* 0 - unknown     1 - top first     2 - bottom first */
/* audio description */
	uint32 	channels;			/* 2 bytes in file */
	uint32 	sample_size;		/* 2 bytes in file */
	uint32 	compression_id;		/* 2 bytes in file */
	uint32 	packet_size;			/* 2 bytes in file */
	float 	sample_rate;		/* 4 bytes in file */
/* decoder config information*/
	mp4_esds_t 	esds;			/* mp4v/mp4a decoder information*/
	mp4_damr_t	damr;			/* samr decoder information*/
	mp4_d263_t	d263;			/* h263 decoder information*/
} mp4_stsd_table_t;

/*----------sample descriptions(codec types, parameters etc.) ----------*/
typedef struct
{
	uint32  version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	uint32  total_entries;			/* 4 bytes in file */
	mp4_stsd_table_t *table;
} mp4_stsd_t;

/*---------- time to sample table----------*/
typedef struct
{
	uint32  sample_count;			/* 4 bytes in file */
	uint32  sample_duration;		/* 4 bytes in file */
} mp4_stts_table_t;
/*---------- time to sample description box----------*/
typedef struct
{
	uint32  version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	uint32  total_entries;			/* 4 bytes in file */
	/* used by the library for allocating a table */
	mp4_stts_table_t *table;
	uint32  entries_allocated;		/* define by myself, not from file */
	/*these member define for mux because memory limited*/
	uint32  start_position;			/* define by myself, not from file */
	uint32  end_position;			/* define by myself, not from file */
	uint32  start_entries;			/* define by myself, not from file */
	uint32  start_duration;		/* define by myself, not from file */
	uint32  end_duration;			/* define by myself, not from file */
	uint32  start_samples;		/* define by myself, not from file */
	uint32  end_samples;			/* define by myself, not from file */
	/*these member define for duration table error*/
	uint32  default_duration;		/* define by myself, not from file */
	uint32  default_samples;		/* define by myself, not from file */
	uint32  is_video;				/* define by myself, not from file */
	uint32  is_audio;				/* define by myself, not from file */
} mp4_stts_t;

/*---------- sync sample table----------*/
typedef struct
{
	uint32  sample;				/* 4 bytes in file */
} mp4_stss_table_t;
/*---------- sync sample description box----------*/
typedef struct
{
	uint32  version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	uint32  total_entries;			/* 4 bytes in file */
	/* used by the library for allocating a table */
	mp4_stss_table_t *table;
	uint32  entries_allocated;		/* define by myself, not from file */	
	/*these member define for mux because memory limited*/
	uint32  start_entries;			/* define by myself, not from file */
	uint32  start_samples;		/* define by myself, not from file */
	uint32  end_samples;			/* define by myself, not from file */
	uint32  start_position;			/* define by myself, not from file */
	uint32  end_position;			/* define by myself, not from file */
} mp4_stss_t;

/*---------- sample to chunk table----------*/
typedef struct
{
	uint32  chunk;				/* 4 bytes in file */
	uint32  samples;				/* 4 bytes in file */
	uint32  id;					/* 4 bytes in file */
} mp4_stsc_table_t;
/*---------- sample to chunk description box----------*/
typedef struct
{
	uint32  version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	uint32  total_entries;			/* 4 bytes in file */
	/* used by the library for allocating a table */
	mp4_stsc_table_t *table;
	uint32  entries_allocated;		/* define by myself, not from file */	
	/*these member define for mux because memory limited*/
	uint32  start_position;			/* define by myself, not from file */
	uint32  end_position;			/* define by myself, not from file */
	uint32  start_entries;			/* define by myself, not from file */
	uint32  start_samples;		/* define by myself, not from file */
	uint32  end_samples;			/* define by myself, not from file */
	uint32  start_chunk;			/* define by myself, not from file */
	uint32  end_chunk;			/* define by myself, not from file */
	uint32  last_chunk;			/* define by myself, not from file */
	uint32  last_chunk_sample;	/* define by myself, not from file */
	/*these member define for write tmp table when mux file*/
	uint32 is_video;				/* define by myself, not from file */
	uint32 is_audio;				/* define by myself, not from file */
} mp4_stsc_t;

/*---------- sample size table----------*/
typedef struct
{
	uint32  size;					/* 4 bytes in file */
} mp4_stsz_table_t;
/*---------- sample size description box----------*/
typedef struct
{	
	/*these member is stsz table mandatory*/
	uint32  version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	uint32  sample_size;			/* 4 bytes in file */
	uint32  total_entries;			/* 4 bytes in file */
	/* used by the library for allocating a table */
	mp4_stsz_table_t *table;
	uint32  entries_allocated;    	/* define by myself, not from file */
	/*these member define for demux because memory limited*/
	uint32  start_position;			/* define by myself, not from file */
	uint32  end_position;			/* define by myself, not from file */
	uint32  start_entries;			/* define by myself, not from file */
	uint32  start_samples;		/* define by myself, not from file */
	uint32  end_samples;			/* define by myself, not from file */
	uint32  end_smpsize;			/* define by myself, not from file */
	/*these member define for write tmp table when mux file*/
	uint32  is_video;				/* define by myself, not from file */
	uint32  is_audio;				/* define by myself, not from file */
} mp4_stsz_t;

/*---------- chunk offset table----------*/
typedef struct
{
	uint32  offset;				/* 4 bytes in file */
} mp4_stco_table_t;
/*---------- chunk offset description box----------*/
typedef struct
{
	uint32 version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	uint32  total_entries;			/* 4 bytes in file */
	/* used by the library for allocating a table */
	mp4_stco_table_t *table;
	uint32  entries_allocated;   	/* define by myself, not from file */ 
	/*these member define for mux because memory limited*/
	uint32  start_position;			/* define by myself, not from file */
	uint32  end_position;			/* define by myself, not from file */
	uint32  start_entries;			/* define by myself, not from file */
	uint32  start_chunk;			/* define by myself, not from file */
	uint32  end_chunk;			/* define by myself, not from file */
	uint32  end_offset;			/* define by myself, not from file */
	/*these member define for write tmp table when mux file*/
	uint32 is_video;				/* define by myself, not from file */
	uint32 is_audio;				/* define by myself, not from file */
} mp4_stco_t;

/*-=-=-=-=-=-=-=-=-=- sample table information-=-=-=-=-=-=-=-=-=-*/
/*{   media information box structure: 
		********************************+
		|         vmhd description(&)			|
		|	    smhd description(&)			|
		|-------------------------------|
		|         dinf box description			|
		|               dref box description		|
		|                      dref table			|
		|-------------------------------|
		|         stbl box description			|
		|               stsd box description		|
		|               stts box description		|
		|               stsc box description		|
		|               stco box description		|
		|               stsz box description		|
		|               stss box description		|
		*********************************   }*/
typedef struct
{
	uint32 version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	mp4_stts_t stts;
	mp4_stsz_t stsz;
	mp4_stss_t stss;
	mp4_stsc_t stsc;
	mp4_stco_t stco;
	mp4_stsd_t stsd;
} mp4_stbl_t;

/*---------- data reference ----------*/
typedef struct
{
	uint32  size;					/* 4 bytes in file */
	char     type[4];				/* 4 bytes in file */
	uint32  version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	char*  data_reference;		/* size-12 bytes in file */
} mp4_dref_table_t;

typedef struct
{
	uint32 version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	uint32  total_entries;			/* 4 bytes in file */
	mp4_dref_table_t *table;
} mp4_dref_t;
/* ----------data information ----------*/
typedef struct
{
	mp4_dref_t dref;
} mp4_dinf_t;
/*---------- video media header ----------*/
typedef struct
{
	uint32 	version;				/* 1 bytes in file */
	uint32  	flags;				/* 3 bytes in file */
	uint32 	graphics_mode;		/* 4 bytes in file, 0 means replace other track video, other need operate with other track video */
	uint32 	opcolor[3];			/* 4*3 bytes in file, operate key color */
} mp4_vmhd_t;
/*---------- sound media header ----------*/
typedef struct
{
	uint32 	version;				/* 1 bytes in file */		
	uint32  	flags;				/* 3 bytes in file */
	uint32 	balance;			/* 2 bytes in file, 16 bit float, 0, left channel= right channel, -1.0 right channel=0, 1.0 left channel=0 */
	uint32 	reserved;			/* 2 bytes in file */
} mp4_smhd_t;

/*---------- handler reference ----------*/
typedef struct
{
	uint32 	version;					/* 1 bytes in file */
	uint32  	flags;					/* 3 bytes in file */
	char		component_type[4];		/* 4 bytes in file */
	char		component_subtype[4];	/* 4 bytes in file */
	uint32	component_manufacturer;/* 4 bytes in file */
	uint32	component_flags;		/* 4 bytes in file */
	uint32	component_flag_mask;	/* 4 bytes in file */
	char 	component_name[256];	/* 4 bytes in file */
} mp4_hdlr_t;

/*---------- media information structure ----------*/
typedef struct
{
	mp4_vmhd_t 	vmhd;
	mp4_smhd_t 	smhd;
	mp4_stbl_t 	stbl;
	mp4_hdlr_t 	hdlr;
	mp4_dinf_t 	dinf;
	uint32 	is_video;
	uint32 	is_audio;
} mp4_minf_t;

/*{  media box structure:
		********************************+
		|  mdhd description                  		|
		********************************+
		|  hdlr description                  		|
		********************************+
		|  minf box description                  	|
		--------------------------------
		|         vmhd description(&)			|
		|	    smhd description(&)			|
		|-------------------------------|
		|         dinf box description			|
		|-------------------------------|
		|         stbl box description			|
		*********************************   }*/
/*---------- media header information ----------*/
typedef struct
{
	uint32  version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	uint32  creation_time;			/* 4/8 bytes in file, v=0, 4 bytes, v=1, 8 bytes */
	uint32  modification_time;		/* 4/8 bytes in file, v=0, 4 bytes, v=1, 8 bytes */
	uint32  time_scale;			/* 4 bytes in file */
	uint32  duration;				/* 4/8 bytes in file, v=0, 4 bytes, v=1, 8 bytes */
	uint32  language;				/* 2 bytes in file, pad&language, 1 bit=pad,  15 bit=language code*/
	uint32  quality;				/* 2 bytes in file, default = 0 */
} mp4_mdhd_t;
/*---------- media ----------*/
typedef struct
{
	mp4_mdhd_t	mdhd;
	mp4_minf_t	minf;
	mp4_hdlr_t	hdlr;
} mp4_mdia_t;

/*{  moov box structure:
		********************************+
		|  mvhd description                  		|
		********************************+
		|  trak box description                  	|
		--------------------------------
		|         tkhd description(&)			|
		|-------------------------------|
		|         mdia box description			|
		|-------------------------------|
		|               mdhd description			|
		|               hdlr description			|
		|-------------------------------|
		|               minf box description		|
		|               	  vmhd description		|
		|               	  smhd description		|
		|               	  dinf box description	|
		|               	  stbl box description	|
		*********************************   }*/
typedef struct
{
	mp4_tkhd_t tkhd;
	mp4_mdia_t mdia;
} mp4_trak_t;

typedef struct
{
	uint32  version;				/* 1 bytes in file */
	uint32  flags;					/* 3 bytes in file */
	uint32  creation_time;			/* 4 bytes in file */
	uint32  modification_time;		/* 4 bytes in file */
	uint32  time_scale;			/* 4 bytes in file */
	uint32  duration;				/* 4 bytes in file, max track length= duration of longest track  */
	float     preferred_rate;		/* 4 bytes in file */
	float     preferred_volume;	/* 2 bytes in file */
	char     reserved[10];			/* 10 bytes in file */
	mp4_matrix_t matrix;			/* 4x9 bytes in file */
	uint32  preview_time;			/* 4 bytes in file */
	uint32  preview_duration;		/* 4 bytes in file */
	uint32  poster_time;			/* 4 bytes in file */
	uint32  selection_time;		/* 4 bytes in file */
	uint32  selection_duration;	/* 4 bytes in file */
	uint32  current_time;			/* 4 bytes in file */
	uint32  next_track_id;			/* 4 bytes in file */
} mp4_mvhd_t;

typedef struct
{
	uint32 total_tracks;
	mp4_mvhd_t mvhd;
	mp4_trak_t *trak[MAXTRACKS];
} mp4_moov_t;

typedef struct
{
	uint32  start;
	uint32  size;
} mp4_mdat_t;

typedef struct
{
	uint32  start;      			/* byte start in file */
	uint32  end;        			/* byte endpoint in file */
	uint32  size;       			/* byte size for writing */
	char type[4];
} mp4_atom_t;

/*=============== table of pointers to every track ===============*/
typedef struct
{
	mp4_trak_t *track; 		/* real quicktime track corresponding to this table */
	uint32  channels;            	/* number of audio channels in the track */
	uint32  current_position;   /* current sample in output file */
	uint32  current_chunk;	/* current chunk in output file */
	uint32  last_sample_size; 	/* last sample frame sise */
	uint32  last_offset;
	void*   codec;
} mp4_audio_map_t;

typedef struct
{
	mp4_trak_t *track;
	uint32  current_position;
	uint32  current_chunk;
	uint32  last_sample_size;   	/* last sample frame sise */
	uint32  last_offset;
	unsigned char **frame_cache;
	uint32  frames_cached;
	void*   codec;
} mp4_video_map_t;

/*=============== file descriptor passed to all routines ===============*/
typedef struct{
	uint32		buffer_address;		/* buffer pointer */
   	uint32 		buffer_size;			/* buffer size*/
	uint32		data_length;			/* bytes in buffer */
      	uint32		cursor_read;		/* read cursor in buffer */
	uint32		cursor_write;		/* write cursor in buffer */
	uint32		frame_count;		/* frame count */
	uint32		frame_size;			/* frame size */
	uint32		frame_duration;		/* frame duration */
	uint32		isnot_empty;		/* flag to identify buffer status, 0: empty, others: not empty */
	uint32		is_over;				/* flag to identify buffer data finsh, 0: is not last block data, 1: is last data block */
}audio_buffer_t;

typedef struct {
	uint32		buffer_address;		/* buffer pointer */
   	uint32 		buffer_size;			/* buffer size*/
	uint32		data_length;			/* bytes in buffer */
      	uint32		cursor_read;		/* read cursor in buffer */
	uint32		cursor_write;		/* write cursor in buffer */
	uint32		frame_count;		/* frame count */
	uint32		frame_size[30];		/* frame size */
	uint32		frame_synflag[30];	/* I frame flag */
	uint32		frame_duration[30];	/* frame duration */
	uint32		isnot_empty;		/* flag to identify buffer status, 0: empty, others: not empty */
	uint32		is_over;				/* flag to identify buffer data finsh, 0: is not last block data, 1: is last data block */
}video_buffer_t;

typedef struct
{
	uint32 	short_video_start_marker;		/*  22 bslbf in data stream start  
												000080xx~000083xx*/
 	uint32 	temporal_reference;				/*  8 uimsbf in data stream start  */
	uint32 	marker_bit;						/*  1 bslbf in data stream start  */
	uint32 	one_reserved_zero_bits;			/*  1 bslbf in data stream start, one word end*/
	uint32 	split_screen_indicator;			/*  1 bslbf in data stream start  */
	uint32 	document_camera_indicator;		/*  1 bslbf in data stream start  */
	uint32 	full_picture_freeze_release;		/*  1 bslbf in data stream start  */
	uint32 	source_format;					/*  3 bslbf in data stream start  
											    000, 110, 111 reserved 
											    001: sub-QCIF 128x96
											    010: QCIF 176x144 
											    011: CIF 352x288
											    100: 4CIF 704x576
											    101: 16CIF 1408x1152*/
	uint32 	picture_coding_type;				/*  1 bslbf in data stream start  
											    0: the vop is I
											    1: the vop is P*/
	uint32 	video_object_layer_width;		/* define by myself, not from file */
	uint32 	video_object_layer_height;		/* define by myself, not from file */
} mp4_h263_info;

typedef struct
{
	uint32 	visual_object_start_code;				/* 32b */
	uint32 	visual_object_ver_id;					/* 4 bslbf in file*/
	uint32 	visual_object_priority;					/* 3 bslbf in file*/
	uint32 	video_object_layer_verid;				/* 4 bslbf in file*/
	uint32 	video_object_layer_priority;				/* 3 bslbf in file*/
	uint32 	video_object_layer_shape;				/* 2 bslbf in file*/
	uint32 	vop_time_increment_resolution;			/* 16 bslbf in file*/
	uint32 	fixed_vop_rate;							/* 1 bslbf in file*/
	uint32	fixed_vop_time_increment;				/* vop_time_increment_resolution bslbf in file*/
	uint32 	video_object_layer_width;				/* 13 bslbf in file*/
	uint32 	video_object_layer_height;				/* 13 bslbf in file*/
} mp4_mp4v_info;

/*==========mp4_header_define_for_futuer========*/
typedef struct _mp4_mp4v_header {

	// vol
	uint32 ident;
	uint32 random_accessible_vol;
	uint32 type_indication;
	uint32 is_object_layer_identifier;
	uint32 visual_object_layer_verid;
	uint32 visual_object_layer_priority;
	uint32 aspect_ratio_info;
	uint32 vol_control_parameters;
	uint32 chroma_format;
	uint32 low_delay;
	uint32 vbv_parameters;
	uint32 first_half_bit_rate;
	uint32 latter_half_bit_rate;
	uint32 first_half_vbv_buffer_size;
	uint32 latter_half_vbv_buffer_size;
	uint32 first_half_vbv_occupancy;
	uint32 latter_half_vbv_occupancy;
	uint32 video_object_layer_shape;
	uint32 time_increment_resolution;
	uint32 fixed_vop_rate;
	uint32 fixed_vop_time_increment;
	uint32 video_object_layer_width;
	uint32 video_object_layer_height;
	uint32 interlaced;
	uint32 obmc_disable;
	uint32 sprite_usage;
	uint32 not_8_bit;
	uint32 quant_precision;
	uint32 bits_per_pixel;
	uint32 quant_type;
	uint32 load_intra_quant_matrix;
	uint32 load_nonintra_quant_matrix;
	uint32 quarter_pixel;
	uint32 complexity_estimation_disable;
	uint32 error_res_disable;
	uint32 data_partitioning;
	uint32 intra_acdc_pred_disable;
	uint32 scalability;

	// gop
	uint32 time_code;
	uint32 closed_gov;
	uint32 broken_link;

	// vop
	uint32 prediction_type;
	uint32 time_base;
	uint32 time_inc;
	uint32 vop_coded;
	uint32 rounding_type;
	uint32 hor_spat_ref;
	uint32 ver_spat_ref;
	uint32 change_CR_disable;
	uint32 constant_alpha;
	uint32 constant_alpha_value;
	uint32 intra_dc_vlc_thr;
	uint32 quantizer;
	uint32 fcode_for;
	uint32 shape_coding_type;

	// macroblock
	uint32 not_coded;
	uint32 mcbpc;
	uint32 derived_mb_type;
	uint32 cbpc;
	uint32 ac_pred_flag;
	uint32 cbpy;
	uint32 dquant;
	uint32 cbp;

	// extra/derived
	uint32 mba_size;
	uint32 mb_xsize;
	uint32 mb_ysize;
	uint32 picnum;
	uint32 mba;
	uint32 mb_xpos;
	uint32 mb_ypos;
	uint32 dc_scaler;

	uint32 intrablock_rescaled;

} mp4_mp4v_header;

typedef struct
{
	void *stream;				/* FILE *stream; 820 file maybe not in file system*/
	uint32  rw_mode;				/* 0, means read(demux) file, others means write(mux) file*/
	uint32  total_length;			/* file total length */

	uint32  audio_type;			/* samr, mp4a*/			
	uint32  video_type;			/* s263, mp4v*/		
	
	uint32  total_atime;			/* audio total play time (ms)*/
	uint32  total_asamples;		/* audio total frame*/
	uint32  total_atracks;			/* audio track counter */
	
	uint32  total_vtime;			/* video total play time (ms)*/
	uint32  total_vsamples;		/* video total frame*/
	uint32  total_vtracks;			/* video track counter */
	
	uint32  video_srcwidth;		/* video width form header table */
	uint32  video_srcheight;		/* video height form header table */
	uint32  video_vopwidth;		/* video width form stream/config */
	uint32  video_vopheight;		/* video height form stream/config */
	uint32  video_firsttime;		/* first time video data read/write */
	uint32  last_vduration;		/* video total play time (ms)*/
	
	mp4_audio_map_t *atracks;	/* audio track infotmation structor */
	mp4_video_map_t *vtracks;	/* video track infotmation structor */
	mp4_mdat_t  mdat;			/* data start address and size */
	mp4_moov_t moov;			/* moov atom information */
	mp4_h263_info  h263_vop_info;
	mp4_mp4v_info mp4v_vop_info;

	/* file I/O parameter and flag*/
	uint32  file_position;      		/* Current position of file descriptor */
	uint32  reset_audio_position;	/* 1: need get audio data address form all table, 0: can use offset mark by myself*/
	uint32  reset_video_position;   /* 1: need get video data address form all table, 0: can use offset mark by myself*/
	
	/* Read ahead buffer */
	uint32  preload_size;      		/* Enables preload when nonzero. */
	uint32  preload_start;     		/* Start of preload_buffer in file */
	uint32  preload_end;      		/* End of preload buffer in file */
	uint32  preload_ptr;       		/* Offset of preload_start in preload_buffer */
	char   *preload_buffer;

	/* file interface , add for 820, 2007/07/16*/
	void  *table_video;			/* video stsc,stco,stsz,stss table file when record, add by amanda deng 2007.7.16*/
	void  *table_audio;			/* audio stsc,stco,stsz table file when record, add by amanda deng 2007.7.16*/
	void  *table_duration;		/* video stts table file when record, add by amanda deng 2007.7.16*/

	/* file opration interface , add for 820, 2007/01/17*/
	int32  (*read_func)(void *file, char* buffer, int32 readbytes);
	int32  (*write_func)(void *file, char* buffer, int32 writebytes);
	int32  (*seek_func)(void *file, int32 ioffset, int32 mode);
	int32  (*tell_func)(void *file);
	/* file interface, add for record creat tmp file, 2008/04/14*/
	void  *tmpfile_vtable;			/* video stsc,stco,stsz,stss table file when record, add by amanda deng 2007.7.16*/
	void  *tmpfile_atable;			/* audio stsc,stco,stsz table file when record, add by amanda deng 2007.7.16*/
	void  *tmpfile_vstts;			/* video stts table file when record, add by amanda deng 2007.7.16*/

	/*=================from here need setting by user after open mp4 file=======================*/
	/* memory and frame counter control, add for 578/824/878 RELEASE LIB not src code, 2008/04/15*/
	uint32  max_vbuffersize;		/* video buffer max size(bytes)*/
	uint32  max_vframeonce;		/* video max frame read or write once*/
	uint32  min_vframeonce;		/* video min frame read or write once */
	uint32  max_abuffersize;		/* audio buffer max size(bytes)*/
	uint32  max_aframeonce;		/* audio max frame read or write once*/
	uint32  min_aframeonce;		/* audio min frame read or write once */
	
	/* file opration interface , add for record creat tmp file, 2008/04/14*/
	int32 (*fread)(void *buffer, int32 size, int32 count, void *fstream );
	int32 (*fwrite)(const void *buffer, int32 size, int32 count, void *fstream);
	int32 (*ftell)(void* fstream);
	int 	 (*fseek)(void *fstream, int32 offset, int origin);
	int 	 (*feof)(void* fstream);
	int 	 (*fclose)(void* fstream);
	void*(*fopen)(void *filename,void *mode);
	uint32 (*fdelete)(void *filename);

//by dnn 2008/4/11
#if MP4LIB_VERSION2_FOREHANDLE
	int32  (*read_block_func)(void *file, char* buffer, int32 readbytes, uint32 reg, uint32 bit);
	int32  (*write_block_func)(void *file, char* buffer, int32 writebytes, uint32 reg, uint32 bit);
	int32  (*get_offset_sector_func)(void *file, uint32 *sectorsize);
	void* (*duplicate_handle_func)(void *file);
	int32  (*close_handle_func)(void *file);
	void *stream_audiodata;				/* FILE *stream; 820 file maybe not in file system*/
	void *stream_videodata;				/* FILE *stream; 820 file maybe not in file system*/
	void *stream_videoindx;				/* FILE *stream; 820 file maybe not in file system*/
	void *stream_audioindx; 				/* FILE *stream; 820 file maybe not in file system*/
	uint32 audiodata_origpos;
	uint32 videodata_origpos;
	uint32 videoindx_origpos;
	uint32 audioindx_origpos;

	uint32 vtrack_counter;
	uint32 atrack_counter;
#endif	

#if VIDEORECORDERIN848
	mp4_stsz_table_t *sram_stsz_table_video[2];
	mp4_stts_table_t *sram_stts_table[2];
	uint32 current_stsz_write;
	uint32 current_stts_write;
#else
	mp4_stsz_table_t *sram_stsz_table_video;
	mp4_stts_table_t *sram_stts_table;
#endif
	//mp4_stss_table_t *sram_stss_table;
	mp4_stco_table_t *sram_stco_table_video;
	mp4_stsz_table_t *sram_stsz_table_audio;
	mp4_stco_table_t *sram_stco_table_audio;
} mp4_infor_t;
#endif

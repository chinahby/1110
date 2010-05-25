/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_object.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_OBJECT_
#define _ME_OBJECT_

#ifdef __cplusplus
extern "C" {
#endif

#include "me_typedef.h"

#define ME_VERSION2_FOURHANDLE	1
#if ME_VERSION2_FOURHANDLE
#define MAX_FILE_HANDLE_NUM	5
#endif
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for audio infor (2006/12/31 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _me_audio_infor_{
	/*all file(have audio) type must give these information, peremptory information*/
	ME_U32		a_chn;				/* 0: have no audio, 1: mono, 2: stereo */
	ME_U32		a_smplrate;			/* sample rate */
	ME_U32		a_bits;				/* bit length */
	ME_U32		a_bitrate;			/* audio data bit rate */
	ME_U32		a_frmsize;			/* frame size(if changeless frame size, else = 0) */
	ME_U32		a_totaltime;			/* audio total play time (ms)*/
	ME_U32		a_totalframe;		/* audio total frame*/
	ME_U32		a_format;			/* audio code format or audio data type */
	char			a_codec[4];			/* audio codec type id */
	/*these information form here can according file type give, optional information*/
	ME_U32		a_isvalid;			/* 0: have no audio,	1: have stream audio, such as aac
														2: have rawdata audio, such as mp4a(aac)*/
	ME_U32		a_smpllen;			/* byte number of one audio sample */
	ME_U32		a_duration;			/* frame duration(if changeless frame rate or first frame duration) */
	ME_U32		a_tickps;			/* tick number per second (timescale)*/
	ME_U32		a_fpsmp;			/* audio frame ctn of one block/sample*/
	//ME_U32		a_maxfrmsize;			/* audio frame ctn of one block/sample*/
	void*		a_others;			/* audio others information, add by Amanda Deng in 2007.3.26  */
#if (ME_VERSION2_FOURHANDLE)
	ME_U32		a_otherslen;
#endif
}me_audio_infor;

typedef struct _me_audio_baseinfor_{
	ME_U32		a_chn;				/* 0: have no audio, 1: mono, 2: stereo */
	ME_U32		a_smplrate;			/* sample rate */
	ME_U32		a_bits;				/* bit length */
	ME_U32		a_bitrate;			/* audio data bit rate */
	ME_U32		a_frmsize;			/* frame size(if changeless frame size, else = 0) */
	ME_U32		a_totaltime;			/* audio total play time (ms)*/
	ME_U32		a_totalframe;		/* audio total frame*/
	ME_U32		a_format;			/* audio code format or audio data type */
	char			a_codec[4];			/* audio codec type id */
}me_audio_baseinfor;

typedef struct _me_audio_outinfor_{
	ME_U32		a_outbits;			/* audio out bit length */
	ME_U32		a_outchn;			/* audio out channels 1: mono, 2: stereo */
	ME_U32		a_outsmplrate;		/* audio out sample rate */
	ME_U32		a_outmode;		/* audio out mode 0:PCM 1:I2S */
}me_audio_outinfor;

typedef struct _me_audio_inputinfor_{
	ME_U32		a_inputbits;			/* audio input bit length */
	ME_U32		a_inputchn;			/* audio input channels 1: mono, 2: stereo */
	ME_U32		a_inputsmplrate;	/* audio input sample rate */
}me_audio_inputinfor;

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for video infor (2006/12/31 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _me_video_infor_{
	/*all file(have video) type must give these information, peremptory information*/
	ME_U32		v_imagewidth;		/* video image width, 0: have no video */
	ME_U32		v_imageheight;		/* video image height, 0: have no video */
	ME_U32		v_jpgmode;			/* jpg mode */
	ME_U32		v_jpgdecodeaddr; 	/* jpg decode YUV buffer addr*/
	ME_U32		v_jpgdecodelen; 	/* jpg decode YUV buffer len*/
	ME_U32		v_fps;				/* frame number per second(if changeless frame size, else =0) */
	ME_U32		v_bitrate;			/* video data bit rate */
	ME_U32		v_totaltime;			/* video total play time (ms)*/
	ME_U32		v_totalframe;		/* video total frame*/
	ME_U32		v_format;			/* video data format */
	char			v_codec[4];			/* video codec type id */
	/*these information form here can according file type give, optional information*/
	ME_U32		v_isvalid;			/* 0: have no video,	1: have video decode by hardware output by decoder self, 
													   	2: have video decode by software output by decoder self, decode in video task
													  	3: have video decode by software output by decoder self, decode in file task */
	ME_U32		v_tickps;			/* tick number per second (timescale)*/
	ME_U32		v_duration;			/* frame duration(if changeless frame rate or first frame duration) */
   	void*		v_others;			/* video others information, add by Amanda Deng in 2007.3.26 */
}me_video_infor;

typedef struct _me_video_baseinfor_{
	ME_U32		v_imagewidth;		/* video image width, 0: have no video */
	ME_U32		v_imageheight;		/* video image height, 0: have no video */
	ME_U32		v_fps;				/* frame number per second(if changeless frame size, else =0) */
	ME_U32		v_bitrate;			/* video data bit rate */
	ME_U32		v_totaltime;			/* video total play time (ms)*/
	ME_U32		v_totalframe;		/* video total frame*/
	ME_U32		v_format;			/* video data format */
	char			v_codec[4];			/* video codec type id */
}me_video_baseinfor;

typedef struct _me_video_outinfor_{
	ME_U32		v_srcwidth;			/* video display source window width form source image (IPP) */
	ME_U32		v_srcheight;			/* video display source window height form source image(IPP)*/
	ME_U32		v_srcxpos;			/* video display source window x pos form source image(IPP)*/
	ME_U32		v_srcypos;			/* video display source window y pos form source image (IPP)*/
	ME_U32		v_outwidth;			/* video display output width on LCD (IPP,LCDC)*/
	ME_U32		v_outheight;			/* video display output height on LCD (IPP,LCDC)*/
	ME_U32		v_outxpos;			/* video display output x pos on LCD (LCDC)*/
	ME_U32		v_outypos;			/* video display output y pos on LCD (LCDC)*/
	ME_U32		v_outrotation;		/* video display output rotation mode contorl by LCDC (LCDC)*/
	ME_U32		v_outeffect;			/* video display output effect control by IPP module (IPP)*/
}me_video_outinfor;

typedef struct _me_video_inputinfor_{
	ME_U32		v_previewwidth;		/* video preview out width */
	ME_U32		v_previewheight;	/* video preview out height */
	ME_U32		v_previewxpos;		/* video preview out xpos */
	ME_U32		v_previewypos;		/* video preview out ypos */
	ME_U32		v_inputwidth;		/* video input width(capture/video width) */
	ME_U32		v_inputheight;		/* video input height(capture/video height)  */
	ME_U32		v_inputrotation;		/* video input rotation mode */
	ME_U32		v_inputeffect;		/* video input effect(include overlay mode) */
	ME_U32		v_inputfps;			/* video input fps */
}me_video_inputinfor;

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for file functions
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _fs_filefunc_s_  fs_callbacks;
struct _fs_filefunc_s_{
	ME_S32 		(*delete_func)(void *filename);
	ME_PVOID	(*open_func)(void *filename, void *mode);
	ME_S32 		(*close_func)(void *fs_filehandle);
	ME_S32		(*read_func)(void *fs_filehandle, char* buffer, ME_S32 readbytes);
	ME_S32		(*write_func)(void *fs_filehandle, char* buffer, ME_S32 writebytes);
	ME_S32		(*seek_func)(void *fs_filehandle, ME_S32 offset, ME_S32 mode);
	ME_S32 		(*tell_func)(void *fs_filehandle);
	ME_S32 		(*eof_func)(void *fs_filehandle);
//by dnn 2008/4/11
#if ME_VERSION2_FOURHANDLE
//block read/write func for video&audio data
	ME_S32		(*read_block_func)(void *fs_filehandle, char* buffer, ME_S32 readbytes, ME_U32 reg, ME_U32 bit);
	ME_S32		(*write_block_func)(void *fs_filehandle, char* buffer, ME_S32 writebytes, ME_U32 reg, ME_U32 bit);
	ME_S32		(*get_sector_offset_func)(void *fs_filehandle, ME_U32 *sectorsize);
	ME_PVOID	(*duplicate_handle_func)(void *file);
#endif	
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for record information (2007.8.16 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _me_player_infor_{
	//file information parameter form user
	char			fs_filetype [4];		/* user need playing file type*/	
//#if !(ME_VERSION2_FOURHANDLE)	
	void* 		fs_filehandle;		/* user opened file handle*/
//#else
//	void*		fs_filehandle[MAX_FILE_HANDLE_NUM];
//#endif
	fs_callbacks*	fs_filefunc;			/* file read,write,seek,tell ect callback functions*/
	//video output information parameter form user
	ME_U32		v_srcwidth;			/* video display source window width form source image (IPP) */
	ME_U32		v_srcheight;			/* video display source window height form source image(IPP)*/
	ME_U32		v_srcxpos;			/* video display source window x pos form source image(IPP)*/
	ME_U32		v_srcypos;			/* video display source window y pos form source image (IPP)*/
	ME_U32		v_outwidth;			/* video display output width on LCD (IPP,LCDC)*/
	ME_U32		v_outheight;			/* video display output height on LCD (IPP,LCDC)*/
	ME_U32		v_outxpos;			/* video display output x pos on LCD (LCDC)*/
	ME_U32		v_outypos;			/* video display output y pos on LCD (LCDC)*/
	ME_U32		v_outrotation;		/* video display output rotation mode contorl by LCDC (LCDC)*/
	ME_U32		v_outeffect;			/* video display output effect control by IPP module (IPP)*/
	ME_U32		v_jpgmode;			/* jpg mode*/
	ME_U32		v_jpgdecodeaddr; 	/*jpg decode YUV buffer addr*/
	ME_U32		v_jpgdecodelen; 	/*jpg decode YUV buffer len*/
	//audio output information parameter form user
	ME_U32		a_outbits;			/* audio out bit length */
	ME_U32		a_outchn;			/* audio out channels 1: mono, 2: stereo */
	ME_U32		a_outsmplrate;		/* audio out sample rate */
	ME_U32		a_outmode;		/* audio out mode 0:PCM 1:I2S */
}me_player_infor;

typedef struct _me_record_infor_{
	//file information parameter form user
	char			fs_filetype [4];		/* user need recording file type*/
	void* 		fs_filehandle;		/* user creat recording file handle*/
	fs_callbacks*	fs_filefunc;			/* file read,write,seek,tell ect callback functions*/
	ME_U32		a_recbits;			/* bit length */
	ME_U32		a_recchn;			/* 1: mono, 2: stereo */
	ME_U32		a_recsmplrate;		/* sample rate */
	ME_U32		a_recbitrate;		/* bit rate */
	char			a_codec[4];			/* audio codec type id */
	ME_U32		v_recwidth;			/* video width */
	ME_U32		v_recheight;			/* video height */
   	ME_U32		v_recfps;			/* frame number per second(if changeless frame size, else =0) */
	ME_U32		v_recbitrate;		/* bit rate */
	char			v_codec[4];			/* video codec type id */
	//video input information parameter form user
	ME_U32		v_previewwidth;		/* video preview out width */
	ME_U32		v_previewheight;	/* video preview out height */
	ME_U32		v_previewxpos;		/* video preview out xpos */
	ME_U32		v_previewypos;		/* video preview out ypos */
	ME_U32		v_inputwidth;		/* video input width(capture/video width) */
	ME_U32		v_inputheight;		/* video input height(capture/video height)  */
	ME_U32		v_inputrotation;		/* video input rotation mode */
	ME_U32		v_inputeffect;		/* video input effect(include overlay mode) */
	ME_U32		v_inputfps;			/* video input fps */
	//audio input information parameter form user
	ME_U32		a_inputbits;			/* audio input bit length */
	ME_U32		a_inputchn;			/* audio input channels 1: mono, 2: stereo */
	ME_U32		a_inputsmplrate;	/* audio input sample rate */
}me_record_infor;

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for buffer information 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define ME_FRAMECNT_IN_BUFFER		30

typedef struct _audio_pcm_infor_{
	ME_U32		buffer_address;		/* buffer pointer */
   	ME_U32 		buffer_size;			/* buffer size, get form decoder*/
	ME_U32		data_length;			/* output pcm bytes in buffer, when playing modify by dataprocess, clear by DMA interrupt */
	ME_U32		orig_length;			/* src pcm bytes in buffer, when playing modify by decoder, use in audio time */
	ME_U32		cursor_read;		/* read cursor in buffer, when playing modify by DMA interrupt, clear by decoder */
	ME_U32		cursor_write;		/* write cursor in buffer, when playing modify by decoder, clear by DMA interrupt */
	ME_U32		isnot_empty;		/* flag to identify buffer status, 0: buffer have no data, others: means data int the buffer can use*/
	ME_U32		is_ready;			/* flag to identify buffer data status, 0: buffer data can not output, others: means data int the buffer can output*/
	ME_U32		is_valid;			/* flag to identify buffer data status, 0: is noused data, others: is used data, when playing set in audio task  */
	ME_U32		is_over;				/* flag to identify buffer data finsh, 0: is not last block data, 1: is last data block, when playing set by decoder */
}audio_pcm_infor;

typedef struct _video_rawdata_infor_{
	ME_U32		buffer_address;		/* buffer pointer */
   	ME_U32 		buffer_size;			/* buffer size*/
	ME_U32		data_length;			/* bytes in buffer */
	ME_U32		isnot_empty;		/* flag to identify buffer status, 0: empty, others: not empty */
	ME_U32		is_valid;			/* flag to identify buffer data status, 0: is noused data, others: is used data  */
	ME_U32		is_over;				/* flag to identify buffer data finsh, 0: is not last block data, 1: is last data block */
}video_rawdata_infor;

typedef struct _audio_stream_infor_{
	ME_U32		buffer_address;		/* buffer pointer */
//	ME_U32		data_address;		/* real data pointer */
   	ME_U32 		buffer_size;			/* buffer size*/
	ME_U32		data_length;			/* bytes in buffer */
      	ME_U32		cursor_read;		/* read cursor in buffer */
	ME_U32		cursor_write;		/* write cursor in buffer */
	ME_U32		frame_count;		/* frame count */
	ME_U32		frame_size;			/* frame size */
	ME_U32		frame_duration;		/* frame duration */
	ME_U32		isnot_empty;		/* flag to identify buffer status, 0: empty, others: not empty */
	ME_U32		is_over;				/* flag to identify buffer data finsh, 0: is not last block data, 1: is last data block */
}audio_stream_infor;

typedef struct _video_stream_infor_{
	ME_U32		buffer_address;		/* buffer pointer */
   	ME_U32 		buffer_size;			/* buffer size*/
	ME_U32		data_length;			/* bytes in buffer */
      	ME_U32		cursor_read;		/* read cursor in buffer */
	ME_U32		cursor_write;		/* write cursor in buffer */
	ME_U32		frame_count;		/* frame count */
	ME_U32		frame_size[ME_FRAMECNT_IN_BUFFER];		/* frame size */
	ME_U32		frame_synflag[ME_FRAMECNT_IN_BUFFER];	/* I frame flag */
	ME_U32		frame_duration[ME_FRAMECNT_IN_BUFFER];	/* frame duration */
	ME_U32		isnot_empty;		/* flag to identify buffer status, 0: empty, others: not empty */
	ME_U32		is_over;				/* flag to identify buffer data finsh, 0: is not last block data, 1: is last data block */
}video_stream_infor;

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for parser 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define ME_PEREMPTORY_PARSER_INTERFACE			4
//#if !(ME_VERSION2_FOURHANDLE)	
#define ME_PARSER_AUDIO_INTERFACE				6
#define ME_PARSER_VIDEO_INTERFACE					7
//#else
//#define ME_PARSER_AUDIO_INTERFACE				6
//#define ME_PARSER_VIDEO_INTERFACE					8
//#endif
typedef struct _me_parser_s_  me_parser_t;
struct _me_parser_s_{
	char		id[4]; 
	ME_U32	(*check_file_type)(void *fs_filehandle, fs_callbacks *fs_filefunc); 
      	ME_U32	(*init)(void *fs_filehandle, fs_callbacks *fs_filefunc,void **fileinfor); 
	ME_U32	(*release)(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor);
	ME_U32	(*seek)(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe);

	ME_U32	(*get_audio_info)(void *fileinfor, me_audio_infor *audio_infor);
	ME_U32	(*get_audio_info_formidi)(void *fileinfor, me_audio_infor *audio_infor);
   	ME_U32	(*get_audio_frame_buffersize)(void *fileinfor, ME_U32 *framesize, void* audio_info);
	ME_U32	(*get_audio_frame_duration)(void *fileinfor, ME_U32 frame, ME_U32 * duration);
	ME_U32	(*read_audio_oneframe)(void *fileinfor, char *audio_buffer, ME_U32 *readbytes,ME_U32 bufferlength);
	ME_U32	(*read_audio_frames)(void *fileinfor, audio_stream_infor *audio_buffer);

	ME_U32	(*get_video_info)(void *fileinfor, me_video_infor *audio_infor);
	ME_U32	(*get_video_frame_duration)(void *fileinfor, ME_U32  frame,ME_U32 * duration);
  	ME_U32	(*get_video_frame_buffersize)(void *fileinfor, ME_U32 *framesize);
	ME_U32	(*read_video_oneframe)(void *fileinfor, char *video_buffer, ME_U32 *readbytes,ME_U32 bufferlength);
	ME_U32	(*read_video_keyframe)(void *fileinfor, char *video_buffer, ME_U32 *readbytes,ME_U32 bufferlength);
	ME_U32	(*read_video_keyframes)(void *fileinfor, video_stream_infor *video_buffer);
	ME_U32	(*read_video_frames)(void *fileinfor, video_stream_infor *video_buffer);
#if (ME_VERSION2_FOURHANDLE)
	ME_U32	(*read_audio_frames_reg)(void *fileinfor, audio_stream_infor *audio_buffer, ME_U32 reg, ME_U32 bit);
	ME_U32	(*read_audio_frames_post)(void *fileinfor, audio_stream_infor *audio_buffer);
	ME_U32	(*read_video_frames_reg)(void *fileinfor, video_stream_infor *video_buffer, ME_U32 reg, ME_U32 bit);
	ME_U32	(*read_video_frames_post)(void *fileinfor, video_stream_infor *video_buffer);
#endif		
	ME_U32	(*seek_audio_syntime)(void *fileinfor, ME_U32 pos);
};
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for muxer 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define ME_PEREMPTORY_MUXER_INTERFACE			2
#define ME_MUXER_AUDIO_INTERFACE					3
#define ME_MUXER_VIDEO_INTERFACE					3
typedef struct _me_muxer_s_  	me_muxer_t;
struct _me_muxer_s_{
	char		id[4]; 
      	ME_U32	(*init)(void *fs_filehandle, fs_callbacks *fs_filefunc,void **fileinfor); 
	ME_U32	(*release)(void *fileinfor);
	
	ME_U32	(*set_audio_info)(void *fileinfor, me_audio_infor *audio_infor);
	ME_U32	(*write_audio_oneframe)(void *fileinfor, char *audio_buffer, ME_U32 bytes, ME_U32 duration);
	ME_U32	(*write_audio_frames)(void *fileinfor, audio_stream_infor *audio_buffer);

	ME_U32	(*set_video_info)(void *fileinfor, me_video_infor *video_infor);
	ME_U32	(*write_video_oneframe)(void *fileinfor, char *video_buffer, ME_U32 bytes, ME_U32 duration, ME_U32 is_key_frame);
	ME_U32	(*write_video_frames)(void *fileinfor, video_stream_infor *video_buffer);
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for decoder 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define ME_PEREMPTORY_DECODER_INTERFACE			11
typedef struct _me_decoder_s_	  me_decoder_t;
struct _me_decoder_s_{
	char		id[4]; 
	ME_U32	(*init)(void *srcinfor, void *outinfor, void **decodeinfor);
	ME_U32	(*release)(void *decodeinfor);
	ME_U32	(*seek)(void *decodeinfor, ME_U32 pos);
	ME_U32	(*start)(void *decodeinfor);
	ME_U32	(*stop)(void *decodeinfor);
	ME_U32	(*resume)(void *decodeinfor);
	ME_U32	(*pause)(void *decodeinfor);
	ME_U32	(*dst_buffersize)(void *decodeinfor, ME_U32 maxsrcsize, ME_U32 *maxdstsize);
	ME_U32	(*set_outputparam)(void *decodeinfor, void* outinfor);
	ME_U32	(*get_outputparam)(void *decodeinfor, void* outinfor);
	ME_U32	(*get_frameduration)(void *decodeinfor, ME_U32 * duration);
	ME_U32	(*decode_oneframe)(void *decodeinfor, char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen, ME_U32*decodelen);
	ME_U32	(*decode_frames)(void *decodeinfor, void *src, void *dst);
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for encoder 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define ME_PEREMPTORY_ENCODER_INTERFACE			9
typedef struct _me_encoder_s_  me_encoder_t;
struct _me_encoder_s_{
	char		id[4]; 
	ME_U32	(*init)(void *recinfor, void* inputinfor, void **encodeinfor);
	ME_U32	(*release)(void *encodeinfor);
	ME_U32	(*set_inputparam)(void *decodeinfor, void* inputinfor);
	ME_U32	(*start)(void *encodeinfor);
	ME_U32	(*stop)(void *encodeinfor);
	ME_U32	(*resume)(void *encodeinfor);
	ME_U32	(*pause)(void *encodeinfor);
	ME_U32	(*src_buffersize)(void *encodeinfor, ME_U32 *maxdstsize);
	ME_U32	(*dst_buffersize)(void *encodeinfor, ME_U32 *maxdstsize);
	ME_U32	(*encode_frames)(void *encodeinfor, void *src, void *dst);
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for id3 property 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define 	ME_PROPERTY_MAX_SIZE		1024

typedef struct _me_property_{
	char		type[8];		/*need get  property type such as: title, track, artist ect. */
	ME_U32 	size;		/*property really size(property_size<=pProperty_size) */
  	ME_U32	paddress;	/*property point, differ property with differ point type */
	ME_U32 	psize;		/*property point memory size*/
 }me_property;

#define ME_PEREMPTORY_ID3_INTERFACE			3
typedef struct _me_id3_s_  me_id3_t;
struct _me_id3_s_{
	char		id[4]; 
      	ME_U32	(*init)(void *fs_filehandle, fs_callbacks *fs_filefunc,void **id3infor); 
	ME_U32	(*release)(void *id3infor);
	ME_U32	(*get_property) (void *id3infor, me_property*property);
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for data process 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _me_eq_parameter
{
       ME_U32          param_type;  //if 0,use index to choose EqPreset ; if 1,user must input the values of EqGain[11]
	ME_S32          index;            //EqPreset index, (0 ~ 5)
	ME_S32          EqGain[11];    //buffer of values which should be input by user

}me_eq_parameter;

typedef struct _me_src_parameter{
      ME_S32 channels;           	//input PCM channel
      ME_S32 InSampleRate;   	//input PCM samplerate
      ME_S32 OutSampleRate;  	//output PCM samplerate
      ME_S32 iValueIndex ;	  	//output value control, in index of dB, step of 0.5 dB , e.g. -5 is -2.5 dB, 8 is 4 dB  (-24 ~ 24)
 }me_src_parameter;

/*define audio slowplay postprocess parameter for setting*/
typedef struct  _me_slow_parameter
{
      ME_U32 channels;           	//input PCM channel
      ME_U32 samplerate;		//input PCM samplerate		
      ME_U32 level;  	 	 //slowly playing level 0~9
}me_slow_parameter;

/**********added by Anson 20080108************/
typedef struct _me_onc_parameter{
      ME_S32 channels;   //input PCM channel
      ME_S32 onc_gain;  //10-15db(15 is better)   			
 }me_onc_parameter;

typedef struct _me_agc_parameter{
      ME_S32 channels;   //input PCM channel
 }me_agc_parameter;
/*******************************************/

typedef struct _me_mir_parameter{
	ME_S32 samplerate;
	ME_S32 channel;
	ME_S32 algmode;			// 2: frequency domain Band Bar image
	ME_S32 bandnum;		// bandnum: band number for algmode 2, only 8\16\24\32
	ME_S32 outfreq;
	ME_S32 (*get_MIR_infor)(ME_PS32 band_data, ME_S32 band_wide);
}me_mir_parameter;

#define ME_PEREMPTORY_POSTPROC_INTERFACE 	5
typedef struct _me_effectproc_s_  me_effectproc_t;
struct _me_effectproc_s_{
	char		id[4]; 
	void*	infor;
	ME_U32	enable;	
	ME_U32	(*init) (me_effectproc_t *pthis, void *param1);
	ME_U32	(*release) (me_effectproc_t *pthis);
	ME_U32	(*reset) (me_effectproc_t *pthis);
	ME_U32	(*set_enable) (me_effectproc_t *pthis);
	ME_U32	(*set_disable) (me_effectproc_t *pthis); 
	ME_U32	(*set_param) (me_effectproc_t *pthis, void *param1);
	ME_U32	(*data_process) (me_effectproc_t *pthis, void *param1, void *param2, void *param3);
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and structor define for data buffer
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
//new buffer information define by Amanda Deng 2007.8.27
typedef struct _audio_stream_s_  audio_stream_t;
struct _audio_stream_s_{
	audio_stream_infor	buffer[4];		/* in our system user pingpang buffer of all */
	ME_U32		    	buffer_mode;	/* 1->use as fifo, 0->use as buffer */
	ME_U32 		    	buffer_write;	/* current buffer index 0: buffer 0, 1: buffer 1 */
	ME_U32 		    	buffer_read;		/* current buffer index 0: buffer 0, 1: buffer 1 */
	ME_U32 		    	buffer_count;	/* used buffer counter, max = 4 */
};

typedef struct _audio_pcm_s_  audio_pcm_t;
struct _audio_pcm_s_{
	audio_pcm_infor	buffer[4];		/* in our system user pingpang buffer of all */
	ME_U32 		    	buffer_count;	/* used buffer counter, max = 4 */
	ME_U32		    	buffer_mode;	/* 1->use as fifo, 0->use as buffer */
	ME_U32 		    	buffer_write;	/* current buffer index 0: buffer 0, 1: buffer 1 */
	ME_U32 		    	buffer_read;		/* current buffer index 0: buffer 0, 1: buffer 1 */
	ME_U32 		    	buffer_codec;	/* 1->need send decode or encode message, 0 need not */
};

typedef struct _video_stream_s_  video_stream_t;
struct _video_stream_s_{
	video_stream_infor	buffer[8];		/* in our system user pingpang buffer of all */
	ME_U32		    	buffer_mode;	/* 1->use as fifo, 0->use as buffer */
	ME_U32 		    	buffer_write;	/* current buffer index 0: buffer 0, 1: buffer 1 */
	ME_U32 		    	buffer_read;		/* current buffer index 0: buffer 0, 1: buffer 1 */
	ME_U32 		    	buffer_count;	/* used buffer counter, max = 4 */
	ME_U32 		    	duration_write;	/* current buffer index 0: buffer 0, 1: buffer 1 */
	ME_U32 		    	duration_read;	/* current buffer index 0: buffer 0, 1: buffer 1 */
	ME_U32			duration[ME_FRAMECNT_IN_BUFFER*4];	/* frame duration */
};

typedef struct _video_rawdata_s_  video_rawdata_t;
struct _video_rawdata_s_{
	video_rawdata_infor buffer[4];		/* in our system user pingpang buffer of all */
	ME_U32		    	buffer_mode;	/* 1->use as fifo, 0->use as buffer */
	ME_U32 		    	buffer_write;	/* current buffer index 0: buffer 0, 1: buffer 1 */
	ME_U32 		    	buffer_read;		/* current buffer index 0: buffer 0, 1: buffer 1 */
	ME_U32 		    	buffer_count;	/* used buffer counter, max = 4 */
};

typedef struct _mp_audio_buffer_{
	audio_stream_t	src;				/* src data buffer  */				
	audio_pcm_t	 	dst;				/* dst data buffer  */
}mp_audio_buffer;

typedef struct _mp_video_buffer_{
	video_stream_t	src;				/* src data buffer  */				
	video_rawdata_t	dst;				/* dst data buffer  */
}mp_video_buffer;

typedef struct _mr_audio_buffer_{
	audio_pcm_t		src;				/* src data buffer  */				
	audio_stream_t	dst;				/* dst data buffer  */
}mr_audio_buffer;

typedef struct _mr_video_buffer_{
	video_rawdata_t	src;				/* src data buffer  */				
	video_stream_t	dst;				/* dst data buffer  */
}mr_video_buffer;

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and structor define for list and note
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _me_node_s_ me_node_t;
struct _me_node_s_{
	me_node_t	*next;
	me_node_t	*prev;
	ME_VOID		*content;
	ME_S32		priority;
};

typedef struct _me_node_list_{
	ME_U32		counter;
 	me_node_t	*first;
	me_node_t	*last;
	me_node_t	*cur;
}me_node_list;

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and structor define for media engine
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _vm_media_engine_ vm_media_engine;
struct _vm_media_engine_
{
	me_node_list	*parserlist;
	me_node_list	*muxerlist;
	me_node_list	*decoderlist;
	me_node_list	*encoderlist;
	me_node_list	*id3list;
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and structor define for media engine player file 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _MP_FILE_  MP_FILE;
struct _MP_FILE_{
	// parameter input backup 
  	char					fs_filetype [4];		// file type
//#if !(ME_VERSION2_FOURHANDLE)	
	void*				fs_filehandle;		// file handle, maybe is memory file structor's point
//#else
//	void*				fs_filehandle[MAX_FILE_HANDLE_NUM];
//#endif
	fs_callbacks*			fs_filefunc;			// file callbacks
	// parser and id3 information 
	void*				parserinfor;	 		// parser information, creat by file parser
	me_parser_t*			parser;				// id3 information, creat by file id3
	void*				id3infor;	 		// player file's parser, select according with file type
	me_id3_t	*			id3;					// player file's id3, select according with file type
	// audio information
	void*				audio_decinfor;  		// audio encoder inaforamtion, creat by audio encoder
	me_decoder_t	*		audio_decoder;  		// audio encoder, select according with audio type
	me_node_list*			audio_postproc;		// audio pcm data post process list
	me_audio_infor		audio_infor;			// audio inforamtion
	me_audio_outinfor		audio_outinfor;		// audio output information
	mp_audio_buffer		audio_data;			// audio source and dest data buffer inforamtion
	// video information
	void*				video_decinfor;  		// video encoder information, creat by video encoder
	me_decoder_t	*		video_decoder;  		// video encoder, select according with video type
	me_node_list*			video_postproc;		// video yuv data post process list
	me_video_infor		video_infor;			// video inforamtion 
	me_video_outinfor		video_outinfor;		// video output information
	mp_video_buffer		video_data;			// video source and dest data buffer inforamtion
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and structor define for media engine recorder file 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _MR_FILE_  MR_FILE;
struct _MR_FILE_{
	// parameter input backup 
  	char					fs_filetype [4];		// file type
	void*				fs_filehandle;		// file handle, maybe is memory file structor's point
	fs_callbacks*			fs_filefunc;			// file callbacks
	// muxer information 
	void*				muxerinfor;	 		// muxer information, creat by file muxer
	me_muxer_t*			muxer;				// recorder file's muxer, select according with file type
	// audio information
	void*				audio_encinfor;  		// audio encoder inaforamtion, creat by audio encoder
	me_encoder_t	*		audio_encoder;  		// audio encoder, select according with audio type
	me_node_list*			audio_preproc;		// audio pcm data post process list
	me_audio_infor		audio_infor;			// audio inforamtion
	me_audio_inputinfor 	audio_inputinfor;	// audio input information
	mr_audio_buffer		audio_data;			// audio source and dest data buffer inforamtion
	// video information
	void*				video_encinfor;  		// video encoder information, creat by video encoder
	me_encoder_t	*		video_encoder;  		// video encoder, select according with video type
	me_node_list*			video_preproc;		// video yuv data post process list
	me_video_infor		video_infor;			// video inforamtion 
	me_video_inputinfor 	video_inputinfor;	// video input information
	mr_video_buffer		video_data;			// video source and dest data buffer inforamtion
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						function define for object check
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
ME_U32 me_parser_check_id(me_parser_t *parser, char *type);
ME_U32 me_muxer_check_id(me_muxer_t *muxer, char *type);
ME_U32 me_decoder_check_id(me_decoder_t *decoder, char *type);
ME_U32 me_encoder_check_id(me_encoder_t *encoder, char *type);
ME_U32 me_id3_check_id(me_id3_t *id3, char *type);
ME_U32 me_effectproc_check_id(me_effectproc_t *postproc, char *type);
ME_U32 me_match_char32(char *input, char *output);
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						no use function define for object. Amanda Deng
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
ME_U32 decoder_register_initialize(me_decoder_t* decoder);
ME_U32 encoder_register_initialize(me_encoder_t* encoder);
ME_U32 parser_register_initialize(me_parser_t* parser);
ME_U32 muxer_register_initialize(me_muxer_t* muxer);
ME_U32 effectprocess_register_initialize(me_effectproc_t *effect);
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						function define for PCM convertor
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
ME_U32 nomo8bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 nomo16bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 nomo24bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 nomo32bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 stereo8bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 stereo16bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 stereo24bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 stereo32bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 nomo8bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 nomo16bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 nomo24bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 nomo32bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 stereo8bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 stereo16bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 stereo24bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);
ME_U32 stereo32bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen);

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/

#ifdef __cplusplus
}
#endif

#endif //_ME_OBJECT_


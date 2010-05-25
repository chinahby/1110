/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	media_player.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/04/02
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	have 26 interface API for user .
*	
*	MP_U32 media_player_file_open(VIM_MEDIA_PLYFILE_INPUTINFOR *srcfile, VIM_MEDIA_PLAYER_FILE **pfile);
*	MP_U32 media_player_file_close(VIM_MEDIA_PLAYER_FILE *pfile);
*	MP_U32 media_player_file_play(VIM_MEDIA_PLAYER_FILE *pfile);
*	MP_U32 media_player_file_fastplay(VIM_MEDIA_PLAYER_FILE *pfile, MP_U32  mode);
*	MP_U32 media_player_file_pause(VIM_MEDIA_PLAYER_FILE *pfile);
*	MP_U32 media_player_file_stop(VIM_MEDIA_PLAYER_FILE *pfile);
*	MP_U32 media_player_file_resume(VIM_MEDIA_PLAYER_FILE *pfile);
*	MP_U32 media_player_file_seek(VIM_MEDIA_PLAYER_FILE *pfile, MP_U32 pos, MP_U32 *accesspos);
*	MP_U32 media_player_file_gettotaltime(VIM_MEDIA_PLAYER_FILE *pfile, MP_U32 *totaltime);
*	MP_U32 media_player_file_getplaytime(VIM_MEDIA_PLAYER_FILE *pfile, MP_U32 *playtime);
*	MP_U32 media_player_file_getproperty(VIM_MEDIA_PLAYER_FILE* pfile,  VIM_MEDIA_FILE_PROPERTY*pProperty);
*	MP_U32 media_player_file_setproperty(VIM_MEDIA_PLAYER_FILE* pfile,  VIM_MEDIA_FILE_PROPERTY*pProperty);
*	
*	MR_U32 media_recorder_file_open(MP_U32 inputfile,, VIM_MEDIA_RECFILE *rfilehandle);
*	MR_U32 media_recorder_file_close(VIM_MEDIA_RECORDER_FILE* rfile);
*	MR_U32 media_recorder_file_start(VIM_MEDIA_RECORDER_FILE* rfile);
*	MR_U32 media_recorder_file_stop(VIM_MEDIA_RECORDER_FILE* rfile);
*	
*		
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#ifndef _VIM_MEDIA_SYSTEM_H_
#define _VIM_MEDIA_SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "media_comhead.h"			

#define VOL_ADJUST_DISABLE	0x0000
#define VOL_ADJUST_START	0x8000
#define VOL_ADJUST_OVER	0x8001

typedef void (*Func_Callback)(MP_U32 status);
typedef void (*Func_RecCallback)(MR_U32 command, MR_U32 param1, MR_U32 param2);
typedef enum {
	MMD_PLY_FILE_END=(MAJ_D2H_PLY|0xE0),
	MMD_PLY_FILE_ERR=(MAJ_D2H_PLY|0xF0)	
}MMD_PLY_CALLBACK_STATUS;

typedef enum {
	MMD_REC_FILE_END=(MAJ_D2H_PLY|0xE3),
	MMD_REC_FILE_OK=(MAJ_D2H_PLY|0xE4),
	MMD_CAP_AUD_PLY=(MAJ_D2H_PLY|0xE5),
	MMD_CAP_RAM_FILELEN=(MAJ_D2H_PLY|0xE6),
	MMD_REC_FILE_ERR=(MAJ_D2H_PLY|0xF1)
}MMD_REC_CALLBACK_STATUS;
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and structor define for media file(2007.5.16 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef enum{
	MEDIA_AUDIO_STOP=0,
	MEDIA_AUDIO_PLAYING,
	MEDIA_AUDIO_RECORDING,
	MEDIA_AUDIO_PAUSE,
	MEDIA_AUDIO_ERROR=0x7FFFFFFF
}VIM_MEDIA_AUDIO_STATUS;

typedef enum{
	MEDIA_VIDEO_STOP=0,
	MEDIA_VIDEO_PLAYING,
	MEDIA_VIDEO_RECORDING,
	MEDIA_VIDEO_PAUSE,
	MEDIA_VIDEO_ERROR=0x7FFFFFFF
}VIM_MEDIA_VIDEO_STATUS;

typedef enum{
	MEDIA_FILE_STOP=0,
	MEDIA_FILE_PLAYING,
	MEDIA_FILE_FAST_PLAYING,
	MEDIA_FILE_RECORDING,
	MEDIA_FILE_PAUSE,
	MEDIA_FILE_ERROR=0x7FFFFFFF
}VIM_MEDIA_FILE_STATUS;

typedef enum{
	MEDIA_SYSTEM_PLYIDLE = 0,
	MEDIA_SYSTEM_PLYREADY,
	MEDIA_SYSTEM_PLAYING,
	MEDIA_SYSTEM_FASTPLAY,
	MEDIA_SYSTEM_PLYPAUSE,
	MEDIA_SYSTEM_PLYSTOP,
	MEDIA_SYSTEM_PLYERROR = 0x7fffffff
}VIM_MEDIA_PLAYER_STATUS;

typedef enum{
	MEDIA_SYSTEM_RECIDLE,
	MEDIA_SYSTEM_RECREADY,
	MEDIA_SYSTEM_RECORDING,
	MEDIA_SYSTEM_RECSTOP,
	MEDIA_SYSTEM_RECERROR = 0x7fffffff
}VIM_MEDIA_RECORD_STATUS;

typedef enum{
	MEDIA_DMA_STOP = 0,
	MEDIA_DMA_TRANSFER,
	MEDIA_DMA_ERROR = 0x7fffffff
}VIM_MEDIA_AUDDMA_STATUS;

typedef enum {
	FILE_IN_CORE_RAM=0,					
	FILE_IN_CORE_FILE=1,					
	FILE_IN_BASEBAND_RAM=2,				
	FILE_IN_BASEBAND_FILE=3,				
	FILE_IN_TCL_RAM=4,
	FILE_IN_ERROR = 0x7fffffff
}VIM_MEDIA_FILE_STORE_TYPE;

typedef enum{
	MP_AUDIO_STREAM_READY	=0x0001,
	MP_AUDIO_PCM_READY		=0x0002,
	MP_AUDIO_STREAM_END		=0x0004,
	MP_AUDIO_DECODE_END		=0x0008,
	MP_AUDIO_PCM_OUT_END	=0x0010,
	MP_AUDIO_PLAY_END		=0x001F,
	MP_VIDEO_STREAM_READY	=0x00010000,
	MP_VIDEO_YUV_READY		=0x00020000,
	MP_VIDEO_STREAM_END		=0x00040000,
	MP_VIDEO_DECODE_END		=0x00080000,
	MP_VIDEO_YUV_OUT_END		=0x00100000,
	MP_VIDEO_PLAY_END		=0x001F0000,
	MP_FILE_PLAY_END			=0x001F001F,
	MP_AUDIO_READ_ERROR		=0x0000101F,
	MP_AUDIO_DECODE_ERROR	=0x0000201F,
	MP_VIDEO_READ_ERROR		=0x101F0000,
	MP_VIDEO_DECODE_ERROR	=0x201F0000
}VIM_MEDIA_PLYFILE_CONTROL;


typedef enum{
	MR_AUDIO_STREAM_READY	=0x0001,
	MR_AUDIO_PCM_READY		=0x0002,
	MR_AUDIO_STREAM_END		=0x0004,
	MR_AUDIO_ENCODE_END		=0x0008,
	MR_AUDIO_PCM_INPUT_END	=0x0010,
	MR_AUDIO_RECORD_END		=0x001F,
	MR_VIDEO_STREAM_READY	=0x00010000,
	MR_VIDEO_YUV_READY		=0x00020000,
	MR_VIDEO_STREAM_END		=0x00040000,
	MR_VIDEO_DECODE_END		=0x00080000,
	MR_VIDEO_YUV_INPUT_END	=0x00100000,
	MR_VIDEO_RECORD_END		=0x001F0000,
	MR_FILE_RECORD_END		=0x001F001F,
	MR_AUDIO_WRITE_ERROR		=0x0FFFFFFF,
	MR_AUDIO_ENCODE_ERROR	=0x1FFFFFFF,
	MR_VIDEO_WRITE_ERROR		=0x2FFFFFFF,
	MR_VIDEO_ENCODE_ERROR	=0x4FFFFFFF
}VIM_MEDIA_RECFILE_CONTROL;


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							macro and structor define for  property[same as 820 core] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define 	PROPERTY_MAX_SIZE					1024

#define 	VIM_AUDIO_BASEINFOR				me_audio_baseinfor
#define 	VIM_AUDIO_OUTINFOR				me_audio_outinfor
#define 	VIM_AUDIO_INPUTINFOR				me_audio_inputinfor

#define 	VIM_VIDEO_BASEINFOR				me_video_baseinfor
#define 	VIM_VIDEO_OUTINFOR				me_video_outinfor
#define 	VIM_VIDEO_INPUTINFOR				me_video_inputinfor

/*define property union of all kinds of property*/
union VIM_MEDIA_PROPERTY
{
	MP_CHAR		title[PROPERTY_MAX_SIZE];		/*title is string, length<PROPERTY_MAX_SIZE*/
	MP_CHAR		artist[PROPERTY_MAX_SIZE];		/*artist is string, length<PROPERTY_MAX_SIZE*/
	MP_CHAR		album[PROPERTY_MAX_SIZE];		/*album is string, length<PROPERTY_MAX_SIZE*/
	MP_CHAR		year[PROPERTY_MAX_SIZE];		/*year is string, length<PROPERTY_MAX_SIZE*/
	MP_CHAR		comment[PROPERTY_MAX_SIZE];	/*comment is string, length<PROPERTY_MAX_SIZE*/
	MP_CHAR		genre[PROPERTY_MAX_SIZE];		/*genre is string, length<PROPERTY_MAX_SIZE*/
	MP_CHAR		track;							/*track is char,  0<=track<=255*/
	MP_CHAR		filetype[PROPERTY_MAX_SIZE];		/*filetype is string, length<PROPERTY_MAX_SIZE*/
	VIM_AUDIO_BASEINFOR	audioinfor;				/*audioinfor is structor, the structor must be same as 820 core */
	VIM_AUDIO_OUTINFOR		audioout;				/*audioout is structor, the structor must be same as 820 core  */
	VIM_VIDEO_BASEINFOR	videoinfor;				/*videoinfor is structor, the structor must be same as 820 core   */
	VIM_VIDEO_OUTINFOR 		videoout;				/*videoout is structor, the structor must be same as 820 core   */
};

/* property, if get form media engine, if set form host */
typedef struct _VIM_MEDIA_FILE_PROPERTY_  VIM_MEDIA_FILE_PROPERTY;
struct _VIM_MEDIA_FILE_PROPERTY_				
{
	MP_CHAR	stype[16];
	MP_U32		size;
	union VIM_MEDIA_PROPERTY   property;
};
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							macro and structor define for  post process[same as 820 core] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define 	PARAMETER_MAX_SIZE			1024
#define 	VIM_EQ_PARAMETER				me_eq_parameter
#define 	VIM_SRC_PARAMETER			me_src_parameter
#define   VIM_SLOW_PARAMETER			me_slow_parameter
#define 	VIM_RESIZE_PARAMETER			me_video_outinfor
#define 	VIM_MIR_PARAMETER			me_mir_parameter
#define 	VIM_ONC_PARAMETER			me_onc_parameter
#define 	VIM_AGC_PARAMETER			me_agc_parameter

/*define parameter union of all kinds of postprocess/preprocess */
union VIM_EFFECT_PARAMETER
{
	VIM_EQ_PARAMETER		eq;		
	VIM_SRC_PARAMETER   	src;	
	VIM_RESIZE_PARAMETER	resize;
	VIM_MIR_PARAMETER		mir;
	VIM_SLOW_PARAMETER		slow;
	VIM_ONC_PARAMETER		onc;
	VIM_AGC_PARAMETER		agc;
	MP_CHAR				other[PARAMETER_MAX_SIZE];	
};

/* postprocess/preprocess parameter, set form host */
typedef struct _VIM_MEDIA_EFFECTPROC_PARAM_  VIM_MEDIA_EFFECTPROC_PARAM;
struct _VIM_MEDIA_EFFECTPROC_PARAM_				
{
	MP_U32		type;
	MP_U32		size;
	union VIM_EFFECT_PARAMETER   parameter;
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						 structor and global variable define for media system(2007.5.16 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define 	PLAYER_DMA_BUFFER_SIZE		64*1024

/*define media file parameter for openning file, when open file need this parameter from host */
typedef struct _VIM_MEDIA_PLYFILE_INPUTINFOR_  VIM_MEDIA_PLYFILE_INPUTINFOR;
struct _VIM_MEDIA_PLYFILE_INPUTINFOR_				
{
	MP_CHAR	fs_filename[1024];	/*source media file's name include path*/
	MP_U32		fs_filetype;			/*source media file's type*/
	MP_U32		fs_filestore; 		/*source media file's store status*/
	MP_U32		mem_fileaddr;		/*source memory file's start address in 820 memory*/		
	MP_U32		mem_filelength;		/*source memory file's total length*/
	MP_U32		v_srcwidth;			/* video display source window width form source image (IPP) */
	MP_U32		v_srcheight;			/* video display source window height form source image(IPP)*/
	MP_U32		v_srcxpos;			/* video display source window x pos form source image(IPP)*/
	MP_U32		v_srcypos;			/* video display source window y pos form source image (IPP)*/
	MP_U32		v_outwidth; 			/*video width output on LCD/TV after rotation*/
	MP_U32		v_outheight; 		/*video height output on LCD/TV after rotation*/
	MP_U32		v_outxpos; 			/*video X position output on LCD/TV after rotation*/
	MP_U32		v_outypos; 			/*video X position output on LCD/TV after rotation*/
	MP_U32		v_outeffect; 		/*video special effect output on LCD/TV */
	MP_U32		v_outrotation; 		/*video rotation mode output on LCD/TV */
	MP_U32		v_jpgmode; 			/*jpg mode*/
	MP_U32		v_jpgdecodeaddr; 	/*jpg decode YUV buffer addr*/
	MP_U32		v_jpgdecodelen; 	/*jpg decode YUV buffer len*/
	MP_U32		a_outchn; 			/*audio channel output to D/A*/
	MP_U32		a_outbits; 			/*audio bits output to D/A*/
	MP_U32		a_outsmplrate; 		/*audio samplerate output to D/A*/
	MMD_U32	ring_info;			/*Ring information*/	
	MP_U32		ply_fileindex;		/*opened player file index*/
};

/*define media file parameter for openning file, after open file need this parameter to host */
typedef struct _VIM_MEDIA_PLYFILE_OUTINFOR_  VIM_MEDIA_PLYFILE_OUTINFOR;
struct _VIM_MEDIA_PLYFILE_OUTINFOR_				
{
	MP_U32		ply_fileindex;		/*opened player file index*/
	MP_U32		ply_filehandle;		/*source media file's handle*/
	MP_U32		ply_filetype;			/*source media file's type*/
	MP_U32		ply_filekind; 		/*source media file's kind, 0: audio file, 1:video file*/
	MP_U32		v_imagewidth;		/* video image width, 0: have no video */
	MP_U32		v_imageheight;		/* video image height, 0: have no video */
	MP_U32		v_fps;				/* frame number per second(if changeless frame size, else =0) */
	MP_U32		v_bitrate;			/* video data bit rate */
	MP_U32		v_totaltime;			/* video total play time (ms)*/
	MP_U32		v_totalframe;		/* video total frame*/
	MP_U32		a_chn;				/* 0: have no audio, 1: mono, 2: stereo */
	MP_U32		a_smplrate;			/* sample rate */
	MP_U32		a_bits;				/* audio samplerate bit length */
	MP_U32		a_bitrate;			/* audio data bit rate */
	MP_U32		a_totaltime;			/* audio total play time (ms)*/
	MP_U32		a_totalframe;		/* audio total frame*/
};

/*define media file parameter for creatting file, when record file need this parameter form host*/

typedef struct _VIM_MEDIA_RECFILE_INPUTINFOR_  VIM_MEDIA_RECFILE_INPUTINFOR;
struct _VIM_MEDIA_RECFILE_INPUTINFOR_				
{
	MR_CHAR	fs_filename[1024];	//record media file's name include path
	MR_U32		fs_filetype;			//record media file's type
	MR_U32 		fs_filestore;			//record media file's store status
	MR_U32		mem_fileaddr;		/*source memory file's start address */		
	MR_U32		mem_filelength;		/*source memory file's total length*/
	MR_U32		f_limitedtime;		//can set limited record total time by tjis parameter
	MR_U32		f_limitedsize;		//can set limited record file size by tjis parameter
	MR_U32		a_recchn; 			//audio channel record to file
	MR_U32		a_recbits; 			//audio bits record to file
	MR_U32		a_recsmplrate;		//audio samplerate record to file
	MR_U32		a_recbitrate;		//audio bitrate record to file
	MP_U32		a_codec;			//audio encoder type(id)
	MR_U32		v_recwidth; 			//audio channel record to file
	MR_U32		v_recheight; 		//audio channel record to file
	MR_U32		v_recfps;			//audio channel record to file
	MR_U32		v_codec;			//audio encoder type(id)
};

	
/* media player need backup this parameter */
typedef struct _VIM_MEDIA_PLAYER_FILE_  VIM_MEDIA_PLAYER_FILE;
struct _VIM_MEDIA_PLAYER_FILE_
{
	MP_FILE* 	mp_filehandle;	/*media engine create player file handle*/
	MP_U32		mp_filetype;		/*media player file type*/
	MP_U32		mp_filekind;		/*media player file kind*/
	MP_U32		fs_filehandle;	/*file system create player file handle*/
	MP_U32		fs_filestore; 	/*source media file's store status*/
	MP_U32		v_status;
	MP_U32 		v_frames;		// current video frame number
	MP_U32 		v_time;			// current video play time
	MP_U32 		v_duration;		// current video frame duration
	MP_U32 		v_samples;		// current video frame index
	MP_U32 		v_lastdone;		// last frame is decode finishe flag
	MP_U32 		v_framesdone;	// =0, current frame is no use frame 
	MP_U32 		v_databak;		
	MP_U32 		v_countbak;		 
	MP_U32		a_status;
	MP_U32 		a_samples;		// current audio samples number
	MP_U32 		a_time;			// current audio play time
	MP_U32 		a_duration;		// current audio DMA out duation
	MP_U32 		a_dmasamples;	// =0, last dma audio out is no use samples
	MP_U32 		a_lastleftsmpls;	// =0, last dma audio out is no use samples
	MP_V32		f_playstatus;
	MP_V32		f_playcontrol;	
	MP_V32 		f_playmode;	
	MP_V32 		f_playbufinit;
	MP_V32 		f_playtime;		// file current playing/recording time
	MP_V32 		f_basetime;		// file current base time
	MP_V32 		f_jpgmode;	
	MP_U32 		ring_info;
};

/* media recorder need backup this parameter */
///* by dnn 2008/4/10
typedef struct _VIM_MEDIA_RECORDER_FILE_  VIM_MEDIA_RECORDER_FILE;
struct _VIM_MEDIA_RECORDER_FILE_
{
	MR_FILE* 	mr_filehandle;		//media engine create recorder file handle
	MR_U32		fs_filehandle;		//file system create recorder file handle
	MR_U32 		fs_filestore; 		//record media file's store status
	MR_U32		v_status;
	MR_U32 		v_frames;			// current video frame number
	MR_U32 		v_time;				// current video play time
	MR_U32 		v_duration;			// current video frame duration
	MR_U32 		v_lastdone;			// last frame is decode finishe flag
	MR_U32 		v_framesdone;		// =0, current frame is no use frame 
	MR_U32		a_status;
	MR_U32 		a_samples;			// current audio samples number
	MR_U32 		a_time;				// current audio play time
	MR_U32 		a_duration;			// current audio DMA out duation
	MR_U32 		a_dmasamples;		// =0, last dma audio out is no use samples
	MR_U32 		a_lastleftsmpls;	// =0, last dma audio out is no use samples
	MR_V32		f_recordstatus;
	MR_V32		f_recordcontrol;	
	MR_V32 		f_recordmode;		// file current base time	
	MR_V32 		f_recordbufinit;
	MR_V32 		f_recordsize;		// file current recording size
	MR_U32 		f_recordtime;		// file current playing/recording time
	MR_V32 		f_basetime;			// file current base time
	MR_U32		f_limitedtime;		//use when confige record time
	MR_U32		f_limitedsize;		//use when confige record file size
	MR_V32		f_capturedone;		//use when confige record file size
};
//*/

typedef MP_U32  VIM_MEDIA_PLYFILE;
typedef MR_U32  VIM_MEDIA_RECFILE;


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and function define for file play (2007.5.16 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define 	PLAY_FILENAME_INPUT_UART				0

#define 	VIDEO_PLAY_SRC_BUFFER				2// 4	//source data buffer is pinagpang buffer
#define 	VIDEO_PLAY_DST_BUFFER				2	//dest data buffer is pinagpang buffer
#define 	AUDIO_PLAY_SRC_BUFFER				4	//source data buffer is pinagpang buffer
#define 	AUDIO_PLAY_DST_BUFFER				2	//dest data buffer is pinagpang buffer
#define 	AUDIO_PLAY_SRC_FIFO					0
#define 	AUDIO_PLAY_DST_FIFO					0

#define 	VIDEO_FRAMES_READ_TIME_OUTPUT		0
#define 	AUDIO_FRAMES_READ_TIME_OUTPUT		0
#define 	AUDIO_FRAMES_DECODE_TIME_OUTPUT	0
#define 	VIDEO_FRAMES_DECODE_TIME_OUTPUT	0

#define 	READ_VIDEO_DATA_TO_VC0578B			0

MP_U32 media_player_video_start(VIM_MEDIA_PLAYER_FILE* pfile);
MP_U32 media_player_video_stop(VIM_MEDIA_PLAYER_FILE* pfile);
MP_U32 media_player_audio_start(VIM_MEDIA_PLAYER_FILE* pfile);
MP_U32 media_player_audio_stop(VIM_MEDIA_PLAYER_FILE* pfile);
MP_U32 media_player_start_play(VIM_MEDIA_PLAYER_FILE* pfile);
MP_U32 media_player_start_fastplay(VIM_MEDIA_PLAYER_FILE* pfile);
MP_U32 media_player_video_read_oneframe(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 buffer_index);
MP_U32 media_player_audio_read_oneframe(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 buffer_index);
MP_U32 media_player_audio_decode_oneframe(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 src_index, MP_U32 dst_index);
MP_U32 media_player_video_read_frames(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 buffer_index);
MP_U32 media_player_audio_read_frames(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 buffer_index);
MP_U32 media_player_audio_decode_frames(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 src_index, MP_U32 dst_index);
MP_U32 media_player_video_decode_frames(VIM_MEDIA_PLAYER_FILE* pfile, MP_U32 src_index, MP_U32 dst_index);
MP_U32  media_player_change_displaysize(MP_U32 width,MP_U32 height);
MP_U32  media_player_get_displaysize(MP_PU32 pwidth,MP_PU32 pheight);


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						media player API function define(2007.5.16 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
//MP_U32 media_player_file_open(VIM_MEDIA_PLYFILE_INPUTINFOR *srcfile, VIM_MEDIA_PLYFILE*pfilehandle);
MP_U32 media_player_file_open(MP_U32 inputfile, VIM_MEDIA_PLYFILE*pfilehandle);
MP_U32 media_player_file_close(VIM_MEDIA_PLYFILE pfilehandle);

MP_U32 media_player_file_play(VIM_MEDIA_PLYFILE pfilehandle, MP_U32 Callback);
MP_U32 media_player_file_fastplay(VIM_MEDIA_PLYFILE pfilehandle, MP_U32  mode);
MP_U32 media_player_file_stop(VIM_MEDIA_PLYFILE pfilehandle);
MP_U32 media_player_file_pause(VIM_MEDIA_PLYFILE pfilehandle);
MP_U32 media_player_file_resume(VIM_MEDIA_PLYFILE pfilehandle);
MP_U32 media_player_file_seek(VIM_MEDIA_PLYFILE pfilehandle, MP_U32 pos, MP_U32 *accesspos);

MP_U32 media_player_file_gettotaltime(VIM_MEDIA_PLYFILE pfilehandle, MP_U32 *totaltime);
MP_U32 media_player_file_getplaytime(VIM_MEDIA_PLYFILE pfilehandle, MP_U32 *playtime);
MP_U32 media_player_file_getproperty(VIM_MEDIA_PLYFILE pfilehandle, VIM_MEDIA_FILE_PROPERTY*pProperty);
MP_U32 media_player_file_setproperty(VIM_MEDIA_PLYFILE pfilehandle, VIM_MEDIA_FILE_PROPERTY*pProperty);
//add by yjz 2009/2/12
MP_U32 media_player_file_get_mp3_id3property(VIM_MEDIA_PLYFILE pfilehandle,  VIM_MEDIA_FILE_PROPERTY *property);

MP_U32 media_player_file_set_audio_postprocparam(VIM_MEDIA_PLYFILE pfilehandle, VIM_MEDIA_EFFECTPROC_PARAM* param);
MP_U32 media_player_file_enable_audio_postproc(VIM_MEDIA_PLYFILE pfilehandle, char* post_id);
MP_U32 media_player_file_disable_audio_postproc(VIM_MEDIA_PLYFILE pfilehandle, char* post_id);
MP_U32 media_player_file_set_video_postprocparam(VIM_MEDIA_PLYFILE pfilehandle, VIM_MEDIA_EFFECTPROC_PARAM* param);
MP_U32 media_player_file_enable_video_postproc(VIM_MEDIA_PLYFILE pfilehandle, char* post_id);
MP_U32 media_player_file_disable_video_postproc(VIM_MEDIA_PLYFILE pfilehandle, char* post_id);
MR_U32 media_Write_TmpToFile(MR_CHAR * fileName,MR_U32 writeIndex);
#ifdef RV_ENABLE
MP_U32 media_rv_file_open(MP_U32 inputfile, VIM_MEDIA_PLYFILE*pfilehandle);
MP_U32 media_rv_file_play(VIM_MEDIA_PLYFILE pfilehandle, MP_U32 Callback);
MP_U32 media_rv_file_pause(VIM_MEDIA_PLYFILE pfilehandle);
MP_U32 media_rv_file_resume(VIM_MEDIA_PLYFILE pfilehandle);
MP_U32 media_rv_file_stop(VIM_MEDIA_PLYFILE pfilehandle);
MP_U32 media_rv_file_close(VIM_MEDIA_PLYFILE pfilehandle);
MP_U32 media_rv_file_seek(VIM_MEDIA_PLYFILE pfilehandle, MP_U32 pos, MP_U32*accesspos);
MP_U32 media_rv_file_gettotaltime(VIM_MEDIA_PLYFILE pfilehandle, MP_U32*totaltime);
MP_U32 media_rv_file_getplaytime(VIM_MEDIA_PLYFILE pfilehandle, MP_U32*playtime);
MP_U32 media_rv_file_getproperty(VIM_MEDIA_PLYFILE pfilehandle,  VIM_MEDIA_FILE_PROPERTY *property);

#endif
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							macro and function define for file record (2007.7.27 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/

#if(CHIP_ID==ID_VC0824)
#define 	VIDEO_RECORD_DST_BUFFER_MAXSIZE	1024*512
#else
#define 	VIDEO_RECORD_DST_BUFFER_MAXSIZE	1024*512
#endif

#define 	AUDIO_RECORD_SRC_BUFFER				2
#define 	AUDIO_RECORD_DST_BUFFER				2
#define 	VIDEO_RECORD_SRC_BUFFER				2
#define 	VIDEO_RECORD_DST_BUFFER				2

#define 	VIDEO_FRAMES_WRITE_TIME_OUTPUT		0
#define 	AUDIO_FRAMES_WRITE_TIME_OUTPUT		0
#define 	AUDIO_FRAMES_ENCODE_TIME_OUTPUT	1

#define	MAX_PIC_COUNT	9


MP_U32 media_recorder_video_start(VIM_MEDIA_RECORDER_FILE* rfile);
MP_U32 media_recorder_video_stop(VIM_MEDIA_RECORDER_FILE* rfile);
MP_U32 media_recorder_audio_write_frames(VIM_MEDIA_RECORDER_FILE* rfile, MR_U32 buffer_index);
//MP_U32 media_recorder_audio_start(VIM_MEDIA_RECORDER_FILE* rfile);
//MP_U32 media_recorder_audio_stop(VIM_MEDIA_RECORDER_FILE* rfile);
MP_U32 media_recorder_start_record(VIM_MEDIA_RECORDER_FILE* rfile);
//MR_U32 media_recorder_video_write_oneframe(VIM_MEDIA_RECORDER_FILE* rfile, MR_U32 buffer_index);
//MR_U32 media_recorder_audio_write_oneframe(VIM_MEDIA_RECORDER_FILE* rfile, MR_U32 buffer_index);
//MR_U32 media_recorder_audio_encode_oneframe(VIM_MEDIA_RECORDER_FILE* rfile, MR_U32 src_index, MR_U32 dst_index);
MR_U32 media_recorder_video_write_frames(VIM_MEDIA_RECORDER_FILE* rfile, MR_U32 buffer_index);
//MR_U32 media_recorder_audio_write_frames(VIM_MEDIA_RECORDER_FILE* rfile, MR_U32 buffer_index);
//MR_U32 media_recorder_audio_encode_frames(VIM_MEDIA_RECORDER_FILE* rfile, MR_U32 src_index, MR_U32 dst_index);
//MR_U32 media_recorder_file_open(MP_U32 inputfile,VIM_MEDIA_RECFILE *rfilehandle);
MR_U32 media_recorder_file_open(MP_U32 inputfile, VIM_MEDIA_RECFILE *rfilehandle,MR_U32 Callback);
MR_U32 media_recorder_file_close(VIM_MEDIA_RECFILE rfilehandle);
MR_U32 media_recorder_file_start(VIM_MEDIA_RECFILE rfilehandle, MP_U32 Callback);
MR_U32 media_recorder_file_stop(VIM_MEDIA_RECFILE rfilehandle);


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							media recorder API function define(2007.7.27 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
/* by dnn 2008/4/10


MR_U32 media_recorder_file_setproperty(VIM_MEDIA_RECFILE rfilehandle,  VIM_MEDIA_FILE_PROPERTY*property);
MR_U32 media_recorder_file_settotaltime(VIM_MEDIA_RECFILE rfilehandle, MR_U32 totaltime);

MR_U32 media_recorder_file_set_audio_preprocparam(VIM_MEDIA_RECFILE rfilehandle, VIM_MEDIA_EFFECTPROC_PARAM* param);
MR_U32 media_recorder_file_enable_audio_preproc(VIM_MEDIA_RECFILE rfilehandle, char* post_id);
MR_U32 media_recorder_file_disable_audio_preproc(VIM_MEDIA_RECFILE rfilehandle, char* post_id);
MR_U32 media_recorder_file_set_video_preprocparam(VIM_MEDIA_RECFILE rfilehandle, VIM_MEDIA_EFFECTPROC_PARAM* param);
MR_U32 media_recorder_file_enable_video_preproc(VIM_MEDIA_RECFILE rfilehandle, char* post_id);
MR_U32 media_recorder_file_disable_video_preproc(VIM_MEDIA_RECFILE rfilehandle, char* post_id);
*/

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
				structor and macro define for video confige [only in host system] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/

#define VIM_MEDIA_CAPTURE_QUALITY_NORMAL		(0x01)
#define VIM_MEDIA_CAPTURE_QUALITY_GOOD			(0x01<<1)
#define VIM_MEDIA_CAPTURE_QUALITY_BEST			(0x01<<2)
#define VIM_MEDIA_VIDEO_QUALITY_NORMAL			(0x01)
#define VIM_MEDIA_VIDEO_QUALITY_GOOD				(0x01<<1)
#define VIM_MEDIA_VIDEO_QUALITY_BEST				(0x01<<2)
#define VIM_MEDIA_DEFAULT_CAPTURE_WIDTH			640
#define VIM_MEDIA_DEFAULT_CAPTURE_HEIGHT		480
#define VIM_MEDIA_DEFAULT_RECVIDEO_WIDTH		320
#define VIM_MEDIA_DEFAULT_RECVIDEO_HEIGHT		240

#define 	VIM_MEDIA_CODEC_OUTINFOR		me_audio_outinfor
#define 	VIM_MEDIA_CODEC_INPUTINFOR		me_audio_inputinfor

//define video output inforamtion property for getting /setting property
typedef struct _VIM_MEDIA_PREVIEW_INFOR_   VIM_MEDIA_PREVIEW_INFOR;
struct _VIM_MEDIA_PREVIEW_INFOR_				
{
	MR_U32		preview_dispwidth;		
	MR_U32		preview_dispheight;			
	MR_U32		preview_dispxpos;			
	MR_U32		preview_dispypos;
	MR_U32		preview_effect;
	MR_U32		capture_width;
	MR_U32		capture_height;	
	MR_U32		thumbnail_width;
	MR_U32		thumbnail_height;	
	MR_U32		capture_quality;	
	MR_U32		recvideo_width;
	MR_U32		recvideo_height;	
	MR_U32		recvideo_quality;
	MR_U32		preview_frameon;
};

typedef struct _VIM_MEDIA_PREVIEW_OUTINFOR_   VIM_MEDIA_PREVIEW_OUTINFOR;
struct _VIM_MEDIA_PREVIEW_OUTINFOR_				
{
	MR_U32		display_width;		
	MR_U32		display_height;			
	MR_U32		display_xpos;			
	MR_U32		display_ypos;			
	MR_U32		display_effect;
};

MP_U32  media_set_effect(MP_U32 effect);
//MR_U32  media_set_preview_parameter(VIM_MEDIA_PREVIEW_OUTINFOR* preview_out);
MR_U32  media_set_preview_parameter(MR_U32 preview_outinfo);
MR_U32  media_set_capture_resolution(MR_U32 width, MR_U32 height);
MR_U32  media_set_thumbnail_resolution(MR_U32 width, MR_U32 height);
MR_U32  media_set_video_resolution(MR_U32 width, MR_U32 height);
MR_U32  media_set_capture_quality(MR_U32 quality_class);
MR_U32  media_set_video_quality(MR_U32 quality_class);
MR_U32  media_preview_on(MR_U32 previewinfor);
MR_U32  media_preview_off(void);
MR_U32  media_preview_zoomin(void);
MR_U32  media_preview_zoomout(void);
MR_U32  media_change_capture_resolution(MR_U32 width, MR_U32 height);


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
					macro and API function define define for media system(2008.01.15 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef enum _VIM_MEDIA_ROTATEMODE{
	VIM_MEDIA_ROTATE_0,
	VIM_MEDIA_ROTATE_90,
	VIM_MEDIA_ROTATE_180,
	VIM_MEDIA_ROTATE_270,
	VIM_MEDIA_MIRROR_0,
	VIM_MEDIA_MIRROR_90,
	VIM_MEDIA_MIRROR_180,
	VIM_MEDIA_MIRROR_270,
	VIM_MEDIA_ROTATEMODE_ERROR = 0x7fffffff
}VIM_MEDIA_ROTATEMODE;

/*define media mode of 820 core*/
typedef enum _VIM_MEDIA_WORK_MODE_
{
	VIM_MEDIA_MODE_IDLE=0x00000000,
	VIM_MEDIA_MODE_PLAY_AUDIO_ONLY=0x00000001,
	VIM_MEDIA_MODE_PLAY_VIDEO_ONLY=0x00000002,
	VIM_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO=0x00000004,
	VIM_MEDIA_MODE_RECORD_AUDIO_ONLY=0x00000008,
	VIM_MEDIA_MODE_RECORD_VIDEO_ONLY=0x00000010,
	VIM_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO=0x00000020,
	VIM_MEDIA_MODE_CAPTURE=0x00000040,
	VIM_MEDIA_MODE_VIDEOCONFERENCE=0x00000080,
	VIM_MEDIA_MODE_ERROR=0x7FFF0000,    	
	VIM_MEDIA_MODE_AAC_DISABLE=0x00000100  //add by yjz 2009/2/10
}VIM_MEDIA_WORK_MODE;

typedef struct _VIM_MEDIA_SYSTEM_INFOR_  VIM_MEDIA_SYSTEM_INFOR;
struct _VIM_MEDIA_SYSTEM_INFOR_{ 
	VIM_MEDIA_PLAYER_FILE 			*plyfilelst[VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER];	
	VIM_MEDIA_RECORDER_FILE 		*rcfilelst[1];				// our system only support record one file at one time	
	VIM_MEDIA_PREVIEW_INFOR		preview_infor;
	VIM_MEDIA_CODEC_OUTINFOR		codec_outinfor;
	VIM_MEDIA_CODEC_INPUTINFOR		codec_inputinfor;
	MP_U32 		work_mode;
	MP_U32 		audio_mode;
	MP_U32    	ply_status;				// player status 
	MP_U32 	 	ply_dmastatus;			// player dma status
	MP_U32 		ply_filecounter;			// this counter must <= VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER			
	MP_U32 		ply_audiocounter;		// this counter must <= VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER	
	MP_U32 		ply_videocounter;		// this counter must <=1, because our sytem only one vdec module
	MP_U32		ply_dmaaddr0;			// if ply_audiocounter>1 use this variable
	MP_U32		ply_dmaaddr1;			// if ply_audiocounter>1 use this variable
	MP_U32		ply_dmacurrent;			// if ply_audiocounter>1 use this variable
	MP_U32		ply_basetime;			// core system 
	MR_U32  		rec_status;				// recorder status
	MR_U32 	 	rec_dmastatus;			// recorder dma status
	MR_U32 		rec_filecounter;			// this counter must <= 1		
	MR_U32		rec_basetime;			// core system 
	MP_U32		dmadata[2048];			// mute data out when have no pcm data 
};

MP_U32	media_engine_initialize(void);
MP_U32 	media_engine_release(void);
MP_U32	media_work_mode_initialize(MP_U32 work_mode);
MP_U32	media_work_mode_exit(MP_U32 work_mode);

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							global variable define for media system(2007.5.16 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
extern 	VIM_MEDIA_SYSTEM_INFOR*		media_sysstatus;
extern 	VIM_MEDIA_PREVIEW_INFOR		g_previewinfor;
extern 	VIM_MEDIA_PLYFILE_INPUTINFOR    	g_srcmediafile;
extern 	VIM_MEDIA_PLYFILE_OUTINFOR    	g_plymediafile;
extern 	VIM_MEDIA_RECFILE_INPUTINFOR    	g_recmediafile;
extern 	VIM_MEDIA_FILE_PROPERTY			g_mpfileproperty;
extern 	VIM_MEDIA_EFFECTPROC_PARAM	g_postprcparam;
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							end alldefine of media system(2007.5.16 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/


#ifdef __cplusplus
}
#endif

#endif /*_MEDIA_PLAYER_H_*/

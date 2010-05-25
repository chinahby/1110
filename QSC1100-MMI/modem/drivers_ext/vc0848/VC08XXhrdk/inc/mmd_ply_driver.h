 
#ifndef _MMD_PLY_H_
#define _MMD_PLY_H_

/*
 * INCLUDE FILE                                              
 */
#include "mmp_rdk.h"
#include "Me_object.h" //add by yjz 2009/2/12
typedef enum _MMD_MEDIA_ROTATEMODE{
	MMD_MEDIA_ROTATE_0,
	MMD_MEDIA_ROTATE_90,
	MMD_MEDIA_ROTATE_180,
	MMD_MEDIA_ROTATE_270,
	MMD_MEDIA_MIRROR_0,
	MMD_MEDIA_MIRROR_90,
	MMD_MEDIA_MIRROR_180,
	MMD_MEDIA_MIRROR_270,
	MMD_MEDIA_ROTATEMODE_ERROR = 0x7fffffff
}MMD_MEDIA_ROTATEMODE;

typedef enum  _MMD_MEDIA_SPECIAL_EFFECT
{
	MMD_MEDIA_SPECIAL_NORMAL=8,
	MMD_MEDIA_SPECIAL_MONOCHROME=4,
	MMD_MEDIA_SPECIAL_SEPIA=0,
	MMD_MEDIA_SPECIAL_NEGATIVE=2, 
	MMD_MEDIA_SPECIAL_REDONLY,
	MMD_MEDIA_SPECIAL_GREENONLY=7,  //add by xinggang.xu 20090112
	MMD_MEDIA_SPECIAL_EFFECT_ERROR = 0x7fffffff
} MMD_MEDIA_SPECIAL_EFFECT; 

typedef enum  _MMD_MEDIA_CAPTURE_FRAMEMODE
{
	MMD_MEDIA_CAPTURE_NOFRAME,
	MMD_MEDIA_CAPTURE_WITHFRAME,	
	MMD_MEDIA_CAPTURE_FRAMEMODE_ERROR = 0x7fffffff
} MMD_MEDIA_CAPTURE_FRAMEMODE; 
typedef enum  _MMD_MEDIA_JPEG_MODE
{
	MMD_MEDIA_JPEG_MODE_DISPLAY,
	MMD_MEDIA_JPEG_MODE_DECODE,	
	MMD_MEDIA_JPEG_MODE_ERROR = 0x7fffffff
} MMD_MEDIA_JPEG_MODE; 

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							function define for media system(2007.5.16 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
MMD_U32  MMD_Media_System_Init(void);
MMD_U32  MMD_Media_System_Quit(void);
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
					macro and API function define define for media system(2008.01.15 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
/*define media mode of 820 core*/
typedef enum _MMD_MEDIA_WORK_MODE_
{
	MMD_MEDIA_MODE_IDLE=0x00000000,
	MMD_MEDIA_MODE_PLAY_AUDIO_ONLY=0x00000001,
	MMD_MEDIA_MODE_PLAY_VIDEO_ONLY=0x00000002,
	MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO=0x00000004,
	MMD_MEDIA_MODE_RECORD_AUDIO_ONLY=0x00000008,
	MMD_MEDIA_MODE_RECORD_VIDEO_ONLY=0x00000010,
	MMD_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO=0x00000020,
	MMD_MEDIA_MODE_CAPTURE=0x00000040,
	MMD_MEDIA_MODE_VIDEOCONFERENCE=0x00000080,
    	MMD_MEDIA_MODE_ERROR=0x7FFF0000
}MMD_MEDIA_WORK_MODE;

MMD_U32  MMD_Media_Mode_Initialzie(MMD_U32 work_mode);
MMD_U32  MMD_Media_Mode_Exit(MMD_U32 work_mode);

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
				macro and API function define for audio confige [only in host system] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define MMD_MEDIA_LEFTCHANNEL_ONLY		(0x01)
#define MMD_MEDIA_RIGHTCHANNEL_ONLY		(0x01<<1)
#define MMD_MEDIA_STEROECHANNEL			(MMD_MEDIA_LEFTCHANNEL_ONLY | MMD_MEDIA_RIGHTCHANNEL_ONLY)
#define MMD_MEDIA_HEADPHONEOUT			(0x01<<1)
#define MMD_MEDIA_SPEAKEROUT				(0x01)
#define MMD_MEDIA_SPEAKER_HP_OUT			(MMD_MEDIA_SPEAKEROUT | MMD_MEDIA_HEADPHONEOUT)
#define MMD_MEDIA_LINEIN					(0x01)
#define MMD_MEDIA_MICPHONEIN				(0x02)
#define MMD_MEDIA_MICIN_SINGLE			(0x03)		//单端MIC输入
#define MMD_MEDIA_MICIN_DIFFERENCE		(0x04)		//差分MIC输入
///////
MMD_U32  MMD_Media_Codec_Initialize(void);
MMD_U32  MMD_Media_Codec_Exit(void);
MMD_U32  MMD_Media_Codec_Set_Play_Mode(MMD_U32 nCahnnel, MMD_U32 nOutType);
MMD_U32  MMD_Media_Codec_Stop_Play_Mode(void);
MMD_U32 MMD_Media_Codec_Set_Play_Volume(MMD_U32 vLeft, MMD_U32 vRight);
MMD_U32 MMD_Media_Codec_Get_Play_Volume(MMD_U32 *pLeftVol,MMD_U32 *pRightVol);
MMD_U32  MMD_Media_Codec_Set_Record_Mode(MMD_U32 nCahnnel, MMD_U32 nInputType);
MMD_U32  MMD_Media_Codec_Stop_Record_Mode(void);
MMD_U32  MMD_Media_Codec_Set_Record_Volume(MMD_U32 vLeft, MMD_U32 vRight);
MMD_U32 MMD_Media_Codec_Get_Rec_Volume(MMD_U32 *pLeftVol,MMD_U32 *pRightVol);
MMD_U32 MMD_Media_Codec_Set_MuxMode(MMD_U32 mode);
MMD_U32 MMD_Media_Codec_Set_Mute(MMD_U32 mode,MMD_U32 muteFlag);
void MMD_Media_Codec_Recording_MixEnable(void);
void MMD_Media_Codec_Recording_PlayEnable(void);
void MMD_Media_Codec_Recording_PlayDisable(void);
//add by yjz 2009/2/10
void MMD_Media_AAC_Play_Mode(MMD_U32 mode);//MMD_U32 mode 1:enable,0:disable
///:~
MMD_U32 MMD_Media_AudioCodec_FM_Mode(unsigned int sampleRate, unsigned int MixFlag);
//add by yjz 2009/3/9
MMD_U32 MMD_Media_Switch_Play_Mode(MMD_U32 pfilehandle,MMD_U32 samplrate,MMD_U32 mode);
///:~
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
				structor and macro define for video confige [only in host system] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define MMD_MEDIA_CAPTURE_QUALITY_NORMAL		(0x01)
#define MMD_MEDIA_CAPTURE_QUALITY_GOOD			(0x01<<1)
#define MMD_MEDIA_CAPTURE_QUALITY_BEST			(0x01<<2)
#define MMD_MEDIA_VIDEO_QUALITY_NORMAL			(0x01)
#define MMD_MEDIA_VIDEO_QUALITY_GOOD			(0x01<<1)
#define MMD_MEDIA_VIDEO_QUALITY_BEST				(0x01<<2)
#define MMD_MEDIA_DEFAULT_CAPTURE_WIDTH		800
#define MMD_MEDIA_DEFAULT_CAPTURE_HEIGHT		600
#define MMD_MEDIA_DEFAULT_RECVIDEO_WIDTH		176
#define MMD_MEDIA_DEFAULT_RECVIDEO_HEIGHT		220


typedef struct _MMD_PREVIEW_OUTINFOR_   MMD_PREVIEW_OUTINFOR;
struct _MMD_PREVIEW_OUTINFOR_				
{
	MMD_U32		display_width;		
	MMD_U32		display_height;			
	MMD_U32		display_xpos;			
	MMD_U32		display_ypos;
	MMD_U32		display_effect;
};


MMD_U32  MMD_Media_Set_Preview_Parameter(MMD_PREVIEW_OUTINFOR* preview_param);
MMD_U32  MMD_Media_Set_Capture_FrameMode(MMD_U32 mode);
MMD_U32  MMD_Media_Get_Capture_FrameMode(void); //zhangxiang add 2009.03.09

MMD_U32  MMD_Media_Set_Capture_Resolution(MMD_U32 width, MMD_U32 height);
MMD_U32  MMD_Media_Set_Thumbnail_Resolution(MMD_U32 width, MMD_U32 height);
MMD_U32  MMD_Media_Set_Video_Resolution(MMD_U32 width, MMD_U32 height);
MMD_U32  MMD_Media_Set_Capture_Quality(MMD_U32 quality_class);
MMD_U32  MMD_Media_Set_Video_Quality(MMD_U32 quality_class);
MMD_U32  MMD_Media_Change_Capture_Resolution(MMD_U32 width, MMD_U32 height);
MMD_U32  MMD_Media_Preview_On(void);
MMD_U32  MMD_Media_Preview_Off(void);
MMD_U32  MMD_Media_Set_Effect(MMD_U32 effect);
MMD_U32  MMD_Media_Set_Brightness(UINT8 brightness_val);
MMD_U32  MMD_Media_Set_Environment(UINT8 environment_val);
MMD_U32  MMD_Media_Set_Banding(UINT8 banding_val);

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
				structor and macro define for media system [only in host system] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define MMD_D2H_MEDIA_COMMAND			(MAJ_D2H_PLY|0xFF)
#define MMD_ACK_MEDIA_COMMAND			(CMD_MSK_ACKTYPE|CMD_MSK_PARA|MAJ_PLY|0xFF)

#define MMD_D2H_PLY_FILE_END				(MAJ_D2H_PLY|0xE0)
#define MMD_D2H_PLY_TIME_TOTAL			(MAJ_D2H_PLY|0xE1)
#define MMD_D2H_PLY_TIME_PLAY				(MAJ_D2H_PLY|0xE2)
#define MMD_D2H_PLY_FILE_ERROR			(MAJ_D2H_PLY|0xF0)
#define MMD_D2H_REC_FILE_END				(MAJ_D2H_PLY|0xE3)
#define MMD_D2H_REC_FILE_OK				(MAJ_D2H_PLY|0xE4)
#define MMD_D2H_CAP_AUD_PLY				(MAJ_D2H_PLY|0xE5)
#define MMD_D2H_CAP_RAM_FILELEN			(MAJ_D2H_PLY|0xE6)
#define MMD_D2H_REC_FILE_ERROR			(MAJ_D2H_PLY|0xF1)
	
#define MMD_PLY_FILE_OPEN_OK                		(CMD_MSK_ACKTYPE|CMD_MSK_PARA|MAJ_PLY|0x10)
#define MMD_PLY_FILE_OPEN_ERROR                	(CMD_MSK_ACKTYPE|MAJ_PLY|0x10)
#define MMD_PLY_FILE_SEEK_OK               		(CMD_MSK_ACKTYPE|CMD_MSK_PARA|MAJ_PLY|0x16)
#define MMD_PLY_FILE_SEEK_ERROR               	(CMD_MSK_ACKTYPE|MAJ_PLY|0x16)
#define MMD_PLY_FILE_CLOSE               		(CMD_MSK_ACKTYPE|MAJ_PLY|0x11)

#define MMD_REC_FILE_OPEN_OK                		(CMD_MSK_ACKTYPE|CMD_MSK_PARA|MAJ_PLY|0x40)
#define MMD_REC_FILE_OPEN_ERROR               	(CMD_MSK_ACKTYPE|MAJ_PLY|0x40)
#define MMD_REC_FILE_CLOSE               		(CMD_MSK_ACKTYPE|MAJ_PLY|0x41)
#define MMD_REC_MODE_INITIALIZE             	(CMD_MSK_ACKTYPE|MAJ_PLY|0x02)
#define MMD_REC_MODE_EXIT               			(CMD_MSK_ACKTYPE|MAJ_PLY|0x03)

typedef void (*PLYCallback)(MMD_U32 status);//by dnn 2008/5/22
typedef void (*CAPCallback)(void);
typedef void (*RECCallback)(MMD_U32 command, MMD_U32 param1, MMD_U32 param2);
typedef void (*D2HCallback)(MMD_U32 command, MMD_U32 param1, MMD_U32 param2);

typedef MMD_U32  MMD_PLYFILE;
typedef MMD_U32  MMD_RECFILE;

MMD_U32  MMD_Media_Set_D2HCallback(D2HCallback Callback);


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
				structor and macro define for media system [same as 820 core] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/

/* Macro to make a FOURCC out of four characters */
#ifndef MMD_MEDIA_FOURCC
#define MMD_MEDIA_FOURCC( ch0, ch1, ch2, ch3 )				\
		( (MMD_U32)(MMD_CHAR)(ch0) | ( (MMD_U32)(MMD_CHAR)(ch1) << 8 ) |	\
		( (MMD_U32)(MMD_CHAR)(ch2) << 16 ) | ( (MMD_U32)(MMD_CHAR)(ch3) << 24 ) )
#endif

 //Only support to open a fixed number of media
#define MEDIA_MAX_SUPPORT_FILE_NUMBER    1

/*define file type macros for media open used as in parameter*/
#define MMD_MEDIA_FILE_TYPE_MP3 					MMD_MEDIA_FOURCC('m', 'p', '3', 0) 
#define MMD_MEDIA_FILE_TYPE_AMR 					MMD_MEDIA_FOURCC('a', 'm', 'r', 0)
//#define MMD_MEDIA_FILE_TYPE_AMR 					MMD_MEDIA_FOURCC('s','a', 'm', 'r')
#define MMD_MEDIA_FILE_TYPE_AAC 					MMD_MEDIA_FOURCC('a', 'a', 'c', 0) 
#define MMD_MEDIA_FILE_TYPE_WAV 					MMD_MEDIA_FOURCC('w', 'a', 'v', 0)
#define MMD_MEDIA_FILE_TYPE_WMA 					MMD_MEDIA_FOURCC('w', 'm', 'a', 0)
#define MMD_MEDIA_FILE_TYPE_MIDI 					MMD_MEDIA_FOURCC('m', 'i', 'd', 0)
#define MMD_MEDIA_FILE_TYPE_APE 					MMD_MEDIA_FOURCC('a', 'p', 'e', 0)  
#define MMD_MEDIA_FILE_TYPE_3GPP 					MMD_MEDIA_FOURCC('3', 'g', 'p', 0)
#define MMD_MEDIA_FILE_TYPE_MPG4 					MMD_MEDIA_FOURCC('m', 'p', '4', 0)
#define MMD_MEDIA_FILE_TYPE_JPEG 					MMD_MEDIA_FOURCC('j', 'p', 'g', 0)
#define MMD_MEDIA_FILE_TYPE_BMP 					MMD_MEDIA_FOURCC('b', 'm', 'p', 0)
#define MMD_MEDIA_FILE_TYPE_PNG 					MMD_MEDIA_FOURCC('p', 'n', 'g', 0)
#define MMD_MEDIA_FILE_TYPE_GIF 					MMD_MEDIA_FOURCC('g', 'i', 'f', 0)
#define MMD_MEDIA_FILE_TYPE_FLAC 					MMD_MEDIA_FOURCC('f', 'l', 'a', 'c')
#define MMD_MEDIA_TYPE_UNKNOWN 					MMD_MEDIA_FOURCC('u', 'n', 'k', 'w')

/*define audio type macros for audio property structer used as in or out parameter*/
#define MEDIA_AUDIO_TYPE_MP3 						MMD_MEDIA_FOURCC('m', 'p', '3', '.') //MMD_MEDIA_FOURCC('m', 'p', '3', 0) ,lily modified
#define MEDIA_AUDIO_TYPE_AMR 						MMD_MEDIA_FOURCC('a', 'm', 'r', 0)
#define MEDIA_AUDIO_TYPE_AAC 						MMD_MEDIA_FOURCC('a', 'a', 'c', 0)  
#define MEDIA_AUDIO_TYPE_WAV 						MMD_MEDIA_FOURCC('w', 'a', 'v', 0)
#define MEDIA_AUDIO_TYPE_WMA 						MMD_MEDIA_FOURCC('w', 'm', 'a', 0)
#define MEDIA_AUDIO_TYPE_MIDI 						MMD_MEDIA_FOURCC('m', 'i', 'd', 0)
#define MEDIA_AUDIO_TYPE_APE 						MMD_MEDIA_FOURCC('a', 'p', 'e', 0)   
#define MEDIA_AUDIO_TYPE_FLAC 					MMD_MEDIA_FOURCC('f', 'l', 'a', 'c')
#define MEDIA_AUDIO_TYPE_SAMR 					MMD_MEDIA_FOURCC('s', 'a', 'm', 'r')
#define MEDIA_AUDIO_TYPE_MP4A 					MMD_MEDIA_FOURCC('m', 'p', '4', 'a')

/*define audio type macros for audio property structer used as in or out parameter*/
#define MEDIA_VIDEO_TYPE_S263						MMD_MEDIA_FOURCC('s', '2', '6', '3')
#define MEDIA_VIDEO_TYPE_MP4V 					MMD_MEDIA_FOURCC('m', 'p', '4', 'v')
#define MEDIA_VIDEO_TYPE_JPG 						MMD_MEDIA_FOURCC('j', 'p', 'g', 0)

/*define post process type macros for set post process parameter*/
#define MEDIA_POSTPROCESS_EQ 						MMD_MEDIA_FOURCC('e', 'q', 0, 0) 
#define MEDIA_POSTPROCESS_3D 						MMD_MEDIA_FOURCC('3', 'd', 0, 0)
#define MEDIA_POSTPROCESS_MIR						MMD_MEDIA_FOURCC('m', 'i', 'r', 0)

/*define file property type macros for media get property and set property used as in parameter*/
#define MEDIA_FILE_PROPERTY_TYPE					"filetype"		// filetype property is a string, 0<=length(sYear)<=4
#define MEDIA_FILE_PROPERTY_AUDIO_INFOR 			"audioinf"		// audio information property is a structer
#define MEDIA_FILE_PROPERTY_VIDEO_INFOR 			"videoinf"		// video information property is a structer
#define MEDIA_FILE_PROPERTY_AUDIO_OUTINFOR 		"audioout"	// audio output property is a structer
#define MEDIA_FILE_PROPERTY_VIDEO_OUTINFOR 		"videoout"	// video output property is a structer

#define MEDIA_FILE_PROPERTY_TITLE 					"title"		// title property    is a string, 0<=length(sTitle)<=256
#define MEDIA_FILE_PROPERTY_ARSTIST 				"artist"		// artist property  is a string, 0<=length(sArtist)<=256
#define MEDIA_FILE_PROPERTY_ALBUM 				"album" 		// album property is a string, 0<=length(sAlbum)<=256  
#define MEDIA_FILE_PROPERTY_YEAR 					"year"		// year property   is a string, 0<=length(sYear)<=256
#define MEDIA_FILE_PROPERTY_COMMENT 				"comment"	// comment property is a string, 0<=length(sComment)<=256
#define MEDIA_FILE_PROPERTY_GENRE 				"genre"		// genre property  is a string, 0<=length(sGenre)<=256
#define MEDIA_FILE_PROPERTY_TRACK 				"track"  		// track property  is a unsigned char, 0<=track<=255 

#define MEDIA_FILE_PROPERTY_AUDIO_INPUTINFOR 	"audioin"		// audio input property is a structer
#define MEDIA_FILE_PROPERTY_VIDEO_INPUTINFOR 	"videoin"		// video input property is a structer

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							macro and structor define for  property[same as 820 core] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define 	PROPERTY_MAX_SIZE					1024

/*define audio inforamtion property for getting property*/
typedef enum  _MMD_MEDIA_AUDIO_MODE
{
	MMD_MEDIA_AUDIO_PCM=0,
	MMD_MEDIA_AUDIO_I2S,
	MMD_MEDIA_AUDIO_MODE_ERROR = 0x7fffffff
}MMD_MEDIA_AUDIO_MODE; 
typedef struct _AUDIO_SRCINFOR_   AUDIO_SRCINFOR;
struct _AUDIO_SRCINFOR_				
{
	MMD_U32		audio_channel;		/*audio channel get form media file*/
	MMD_U32		audio_smplrate;		/*audio samplerate get form media file*/
	MMD_U32		audio_bits;			/*audio bits get form media file*/
	MMD_U32		audio_bitrate;		/* audio data bit rate */
	MMD_U32		audio_frmsize;		/* frame size(if changeless frame size, else = 0) */
	MMD_U32		audio_totaltime; 	/*audio total play time get form media file*/
	MMD_U32		audio_totalframe; 	/*audio total frame counter get form media file*/
	MMD_U32		audio_format;		/*audio code format or audio data type */
	MMD_U32		audio_codec;		/*audio encoder type(id) get form media file*/
};

/*define video inforamtion property for getting property*/
typedef struct _VIDEO_SRCINFOR_   VIDEO_SRCINFOR;
struct _VIDEO_SRCINFOR_				
{
 	MMD_U32		video_width; 		/*video frame width get form media file*/
	MMD_U32		video_height; 		/*video frame height get form media file*/
	MMD_U32		video_fps;			/*video frames per second get form media file*/
	MMD_U32		video_bitrate;		/* video data bit rate */
	MMD_U32		video_totaltime; 	/*video total play time get form media file*/
	MMD_U32		video_totalframe; 	/*video total frame counter get form media file*/
	MMD_U32		video_format;		/* video data format */
	MMD_U32		video_codec;		/*video encoder type(id) get form media file*/
};

/*define audio output inforamtion property for getting /setting property*/
typedef struct _AUDIO_OUTINFOR_   AUDIO_OUTINFOR;
struct _AUDIO_OUTINFOR_				
{
	MMD_U32		out_bits; 			/*audio bits output to D/A*/
	MMD_U32		out_channel; 		/*audio channel output to D/A*/
	MMD_U32		out_smplrate;		/*audio samplerate output to D/A*/
};
/*define video output inforamtion property for getting /setting property*/
typedef struct _VIDEO_OUTINFOR_   VIDEO_OUTINFOR;
struct _VIDEO_OUTINFOR_				
{
	MMD_U32		src_width;			/* video display source window width form source image (IPP) */
	MMD_U32		src_height;			/* video display source window height form source image(IPP)*/
	MMD_U32		src_xpos;			/* video display source window x pos form source image(IPP)*/
	MMD_U32		src_ypos;			/* video display source window y pos form source image (IPP)*/
 	MMD_U32		out_width; 			/*video width output on LCD/TV after rotation*/
	MMD_U32		out_height; 			/*video height output on LCD/TV after rotation*/
	MMD_U32		out_xpos; 			/*video X position output on LCD/TV after rotation*/
	MMD_U32		out_ypos; 			/*video X position output on LCD/TV after rotation*/
	MMD_U32		out_rotation;		/* video display output rotation mode contorl by LCDC (LCDC)*/
	MMD_U32		out_effect;			/* video display output effect control by IPP module (IPP)*/
};

union PROPERTY
{
	MMD_CHAR		title[PROPERTY_MAX_SIZE];		/*title is string, length<PROPERTY_MAX_SIZE*/
	MMD_CHAR		artist[PROPERTY_MAX_SIZE];		/*artist is string, length<PROPERTY_MAX_SIZE*/
	MMD_CHAR		album[PROPERTY_MAX_SIZE];		/*album is string, length<PROPERTY_MAX_SIZE*/
	MMD_CHAR		year[PROPERTY_MAX_SIZE];		/*year is string, length<PROPERTY_MAX_SIZE*/
	MMD_CHAR		comment[PROPERTY_MAX_SIZE];	/*comment is string, length<PROPERTY_MAX_SIZE*/
	MMD_CHAR		genre[PROPERTY_MAX_SIZE];		/*genre is string, length<PROPERTY_MAX_SIZE*/
	MMD_CHAR		track;							/*track is char,  0<=track<=255*/
	MMD_CHAR		filetype[PROPERTY_MAX_SIZE];		/*filetype is string, length<PROPERTY_MAX_SIZE*/
	AUDIO_SRCINFOR	audioinfor;						/*audioinfor is structor, the structor must be same as 820 core */
	AUDIO_OUTINFOR	audioout;						/*audioout is structor, the structor must be same as 820 core  */
	VIDEO_SRCINFOR	videoinfor;						/*videoinfor is structor, the structor must be same as 820 core   */
	VIDEO_OUTINFOR 	videoout;						/*videoout is structor, the structor must be same as 820 core   */
};

/* property, if get form media engine, if set form host */
typedef struct _FILE_PROPERTY_  FILE_PROPERTY;
struct _FILE_PROPERTY_				
{
	MMD_CHAR		stype[16];
	MMD_U32		size;
	union PROPERTY   property;
};
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							macro and structor define for  post process[same as 820 core] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define 	PARAMETER_MAX_SIZE				1024

/*define audio eq postprocess parameter for setting*/
typedef struct _EQ_PARAMETER_  EQ_PARAMETER;
struct _EQ_PARAMETER_
{
       MMD_U32          type;  					/*EQ parameter type, 0: use EQ class index, 1: use EQ param by user define*/
	MMD_S32          index;        			/*EQ class index(0~5)*/
	MMD_S32          EqGain[11];    			/*EQ quanter parameter by user define*/
};
typedef MMD_S32 (*get_MIR_infor)(MMD_S32* band_data, MMD_S32 band_wide);

typedef struct _MIR_PARAMETER_  MIR_PARAMETER;
struct _MIR_PARAMETER_
{
       MMD_S32 samplerate;
	MMD_S32 channel;
	MMD_S32 algmode;		//2: frequency domain Band Bar image
	MMD_S32 bandnum;		//bandnum: band number for algmode 2, only 8\16\24\32
	MMD_S32 outfreq;
	get_MIR_infor MIR_info;
};

union PARAMETER
{
	EQ_PARAMETER		eq;	
	MIR_PARAMETER		mir;	
	MMD_CHAR			other[PARAMETER_MAX_SIZE];
};

/* postproc parameter, set form host */
typedef struct _PROCESSPARAM_  PROCESS_PARAM;
struct _PROCESSPARAM_				
{
	MMD_U32		  type;
	MMD_U32		  size;
	union PARAMETER   parameter;
};

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and structor define for media file[same as 820 core] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
/*define file store status for player file*/
//typedef enum {
//	FILE_IN_CORE_RAM=0,					
//	FILE_IN_CORE_FILE=1,					
//	FILE_IN_BASEBAND_RAM=2,				
//	FILE_IN_BASEBAND_FILE=3,				
//	FILE_IN_ERROR = 0x7fffffff
//} FILE_STORE_TYPE;

/*define media file parameter for openning file, when open file need this parameter */
typedef struct _PMEDIA_FILE_  PMEDIA_FILE;
struct _PMEDIA_FILE_				
{
	MMD_CHAR		fs_filename[1024];	/*source media file's name include path*/
	MMD_U32		fs_filetype;			/*source media file's type*/
	MMD_U32		fs_filestore; 		/*source media file's store status*/
	MMD_U32		mem_fileaddr;		/*source memory file's start address in 820 memory*/		
	MMD_U32		mem_filelength;		/*source memory file's total length*/
	MMD_U32		v_srcwidth;			/* video display source window width form source image (IPP) */
	MMD_U32		v_srcheight;			/* video display source window height form source image(IPP)*/
	MMD_U32		v_srcxpos;			/* video display source window x pos form source image(IPP)*/
	MMD_U32		v_srcypos;			/* video display source window y pos form source image (IPP)*/
	MMD_U32		v_outwidth; 			/*video width output on LCD/TV after rotation*/
	MMD_U32		v_outheight; 		/*video height output on LCD/TV after rotation*/
	MMD_U32		v_outxpos; 			/*video X position output on LCD/TV after rotation*/
	MMD_U32		v_outypos; 			/*video X position output on LCD/TV after rotation*/
	MMD_U32		v_outeffect; 		/*video special effect output on LCD/TV */
	MMD_U32		v_outrotation; 		/*video rotation mode output on LCD/TV */
	MMD_U32		v_jpgmode; 			/*jpg mode*/
	MMD_U32		v_jpgdecodeaddr; 	/*jpg decode YUV buffer addr*/
	MMD_U32		v_jpgdecodelen; 	/*jpg decode YUV buffer len*/
	MMD_U32		a_outchn; 			/*audio channel output to D/A*/
	MMD_U32		a_outbits; 			/*audio bits output to D/A*/
	MMD_U32		a_outsmplrate; 		/*audio samplerate output to D/A*/
	MMD_U32		ring_info;			/*Ring information*/
	MMD_U32		ply_fileindex;		/*opened player file index*/
};

/*define media file parameter for creatting file, when record file need this parameter */
typedef struct _RMEDIA_FILE_  RMEDIA_FILE;
struct _RMEDIA_FILE_				
{
	MMD_CHAR		fs_filename[1024];	/*record media file's name include path*/
	MMD_U32		fs_filetype;			/*record media file's type*/
	MMD_U32 		fs_filestore;			/*record media file's store status*/
	MMD_U32		mem_fileaddr;		/*source memory file's start address */		
	MMD_U32		mem_filelength;		/*source memory file's total length*/
	MMD_U32		f_limitedtime;		/*can set limited record total time by tjis parameter*/
	MMD_U32		f_limitedsize;		/*can set limited record file size by tjis parameter*/
	MMD_U32		a_recchn; 			/*audio channel record to file*/
	MMD_U32		a_recbits; 			/*audio bits record to file*/
	MMD_U32		a_recsmplrate;		/*audio samplerate record to file*/
	MMD_U32		a_recbitrate;		/*audio bitrate record to file*/
	MMD_U32		a_codec;			/*audio encoder type(id)*/
	MMD_U32		v_recwidth; 			/*audio channel record to file*/
	MMD_U32		v_recheight; 		/*audio channel record to file*/
	MMD_U32		v_recfps;			/*audio channel record to file*/
	MMD_U32		v_codec;			/*audio encoder type(id)*/
};

typedef struct _AUD_BUF_INFO
{
	MMD_U32	AUDIO_CODEC;
	MMD_U32	SRC_SIZE;
	MMD_U32	SRC_NUM;
	MMD_U32	PCM_SIZE;
	MMD_U32	PCM_NUM;	
}AUD_BUF_INFO, *pAUD_BUF_INFO;
typedef struct _AUD_BUF_ADDR
{
	MMD_U32	SRC_ADDR[8];
	MMD_U32	PCM_ADDR[8];	
}AUD_BUF_ADDR, *pAUD_BUF_ADDR;

typedef struct _VID_BUF_INFO
{
	MMD_U32	SRC_NUM;
	MMD_U32	SRC_SIZE;
	MMD_U32	SRC_ADDR[8];
}VID_BUF_INFO, *pVID_BUF_INFO;
#define ENCODEFRAMECOUNT 8
typedef struct _VID_FRAME_INFO
{
	MMD_U32 framecount;
	MMD_U32 framesize[ENCODEFRAMECOUNT];
}VID_FRAME_INFO, *pVID_FRAME_INFO;

typedef struct _VID_BUF_DATA
{
	MMD_U32 	idx;
	MMD_U32 	cursor_read;
	MMD_U32 	datalength;
	MMD_U32	lastblock;
}VID_BUF_DATA, *pVID_BUF_DATA;

#if VIDEORECORDERIN848
typedef struct _VID_STABLE_BUF_INFO
{
	MMD_U32		stszstableaddr[2];	/* stsz stable buffer addr*/
	MMD_U32		sttsstableaddr[2];	/* stts stable buffer addr  */	
}VID_STABLE_BUF_INFO, *pVID_STABLE_BUF_INFO;
#endif


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						media player API function define(2007.5.16 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
MMD_U32  MMD_MediaEngine_Init(void);
MMD_U32  MMD_MediaEngine_Release(void);
MMD_U32  MMD_Player_File_Open(PMEDIA_FILE *plyfile, MMD_PLYFILE*pfile);
MMD_U32  MMD_Player_File_Close(MMD_PLYFILE pfile, MMD_U32*, MMD_U32*);
MMD_U32  MMD_Player_File_Play(MMD_PLYFILE pfile, PLYCallback Callback);
#if 0
MMD_U32  MMD_Player_File_FastPlay(MMD_PLYFILE pfile, MMD_U32 mode);
#endif
MMD_U32  MMD_Player_File_Stop(MMD_PLYFILE pfile);
MMD_U32  MMD_Player_File_Pause(MMD_PLYFILE pfile);
MMD_U32  MMD_Player_File_Resume(MMD_PLYFILE pfile);
MMD_U32  MMD_Player_File_Seek(MMD_PLYFILE pfile, MMD_U32 pos, MMD_U32 *accesspos);

MMD_U32  MMD_Player_File_GetTotalTime(MMD_PLYFILE pfile, MMD_U32*uTotalTime);
MMD_U32  MMD_Player_File_GetPlayTime(MMD_PLYFILE pfile, MMD_U32*uPlayTime);
MMD_U32  MMD_Player_File_GetProperty(MMD_PLYFILE pfile, FILE_PROPERTY*Property);

//add by yjz 2009/2/12
MMD_U32 MMD_Player_File_Get_Mp3_ID3property(MMD_PLYFILE pfile,FILE_PROPERTY*property);
MMD_U32  MMD_Player_File_SetProperty(MMD_PLYFILE pfile, FILE_PROPERTY*Property);

///:~
#if 0
MMD_U32 MMD_Player_File_GetPlayTime_SendbyDevice(MMD_PLYFILE pfile, MMD_U32*uPlayTime);

MMD_U32 MMD_Player_File_SetAudioPostProcessParam(MMD_PLYFILE pfile, PROCESS_PARAM*parameter);
MMD_U32 MMD_Player_File_EnableAudioPostProcess(MMD_PLYFILE pfile, MMD_U32 type);
MMD_U32 MMD_Player_File_DisableAudioPostProcess(MMD_PLYFILE pfile, MMD_U32 type);
MMD_U32 MMD_Player_File_SetVideoPostProcessParam(MMD_PLYFILE pfile, PROCESS_PARAM*parameter);
MMD_U32 MMD_Player_File_EnableVideoPostProcess(MMD_PLYFILE pfile, MMD_U32 type);
MMD_U32 MMD_Player_File_DisableVideoPostProcess(MMD_PLYFILE pfile, MMD_U32 type);
#endif

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							media recorder API function define(2007.8.27 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/

MMD_U32  MMD_Recorder_File_Creat(RECCallback Callback, RMEDIA_FILE *recfile, MMD_RECFILE *rfile);
MMD_U32  MMD_Recorder_File_Close(RECCallback Callback, MMD_RECFILE rfile);
MMD_U32  MMD_Recorder_File_Start(MMD_RECFILE rfile, PLYCallback Callback);
MMD_U32  MMD_Recorder_File_Stop(MMD_RECFILE rfile);

MMD_U32  MMD_Recorder_File_Setproperty(MMD_RECFILE rfile,  FILE_PROPERTY*property);
MMD_U32  MMD_Recorder_File_SetTotalTime(MMD_RECFILE pfile, MMD_U32 uTotalTime);

MMD_U32  MMD_Recorder_File_SetAudioPreprocessParam(MMD_RECFILE rfile, PROCESS_PARAM*parameter);
MMD_U32  MMD_Recorder_File_EnableAudioPreprocess(MMD_RECFILE rfile, MMD_U32 type);
MMD_U32  MMD_Recorder_File_DisableAudioPreprocess(MMD_RECFILE rfile, MMD_U32 type);
MMD_U32  MMD_Recorder_File_SetVideoPreprocessParam(MMD_RECFILE rfile, PROCESS_PARAM*parameter);
MMD_U32  MMD_Recorder_File_EnableVideoPreprocess(MMD_RECFILE rfile, MMD_U32 type);
MMD_U32  MMD_Recorder_File_DisableVideoPreprocess(MMD_RECFILE rfile, MMD_U32 type);
//MMD_U32 MMD_Ply_VideoRecorder_StableBufRelease(void);
MMD_U32 media_recorder_video_stablebufrelease_smtd(void);
//MMD_U32 MMD_Ply_VideoRecorder_StableBufInit(VID_STABLE_BUF_INFO* pInfo);
MMD_U32 media_recorder_video_stablebufinit_smtd(VID_STABLE_BUF_INFO* pInfo);
//MMD_U32 MMD_Rec_Video_FrameInfoInit(MMD_U32 * InfoAddr);
MMD_U32 media_recorder_video_frameinfoinit_smtd(MMD_U32 * InfoAddr);
//MMD_U32 MMD_Rec_Video_FrameInfoRelease(void);
MMD_U32 media_recorder_video_frameinforelease_smtd(void);
//MMD_U32 MMD_Rec_Audio_BufRelease(void);
MMD_U32 media_recorder_audio_bufrelease_smtd(void);
//MMD_U32 MMD_Ply_VideoRecorder_StartTimer(void);
MMD_U32 media_player_videorecorder_starttimer_smtd(void);
//MMD_U32 MMD_Set_Samplerate(MMD_U32 samplerate);
MMD_U32 media_recorder_set_samplerate_smtd(MMD_U32 samplerate);
//MMD_U32 MMD_REC_StrInit(void *pThis);
MMD_U32 media_recorder_strinit_smtd(void *pThis);
//MMD_U32 MMD_REC_StrUpdata(void *pThis);
MMD_U32 media_recorder_strupdata_smtd(void *pThis);
//MMD_U32 MMD_Ply_Audio_REC_BufInit(AUD_BUF_INFO* pInfo, AUD_BUF_ADDR* pAddr);
MMD_U32 media_recorder_audio_bufinit_smtd(AUD_BUF_INFO* pInfo, AUD_BUF_ADDR* pAddr);
//MMD_U32 MMD_REC_Audio_Start(void);
MMD_U32 media_recorder_audio_start_smtd(void);
//MMD_U32 MMD_Ply_Audio_EncInit(void* pAudioInfo,	void* pAudioOutInfo, MMD_U32 a_others_size);
MMD_U32 media_recorder_audio_encinit_smtd(void* pAudioInfo,	void* pAudioOutInfo, MMD_U32 a_others_size);
//MMD_U32 MMD_Rec_Video_BufInit(VID_BUF_INFO* pInfo);
MMD_U32 media_recorder_video_bufinit_smtd(VID_BUF_INFO* pInfo);
//MMD_U32 MMD_Rec_Video_BufRelease(void);
MMD_U32 media_recorder_video_bufrelease_smtd(void);

extern volatile MMD_U32 g_filehandle, g_playtime;

MMD_U32  MMD_Media_Switch_AudioMode(MMD_U32 mode);
MMD_U32  MMD_Media_Set_AudioMode(MMD_U32 mode);
void         MMD_Media_SetRecodeChannel(MMD_U32 channel);
void MMD_Media_PlayMixEnable(void);
void MMD_Media_PlayMixDisable(void);
// Added for  848 ;
MMD_U32 MMD_Mode_Ply_Lisr(MMD_U32 cmd);
MMD_U32 MMD_Mode_Rec_Lisr(MMD_U32 cmd);
//MMD_U32 MMD_Ply_Audio_DecInit(void* pAudioInfo,	void* pAudioOutInfo, MMD_U32 a_others_size);
MMD_U32 media_player_audio_decinit_smtd(void* pAudioInfo,	void* pAudioOutInfo, MMD_U32 a_others_size);
MMD_U32 media_player_audio_codecinit_smtd(AUD_BUF_INFO* pInfo);
//MMD_U32 MMD_Ply_Audio_ParamInit(MMD_U32 samplerate, MMD_U32 audiomode);
MMD_U32 media_player_audio_paraminit_smtd(MMD_U32 samplerate, MMD_U32 audiomode);
MMD_U32 media_player_audio_paramset_smtd(void* pAudioInfo,	void* pAudioOutInfo, MMD_U32 a_others_size);//add by yjz 2009/3/10
//MMD_U32 MMD_Ply_Audio_DecStart(void);
MMD_U32 media_player_audio_decstart_smtd(void);
//MMD_U32 MMD_Ply_Audio_DecStop(void);
MMD_U32 media_player_audio_decstop_smtd(void);
//MMD_U32 MMD_Ply_Audio_DecPause(void);
MMD_U32 media_player_audio_decpause_smtd(void);
//MMD_U32 MMD_Ply_Audio_DecResume(void);
MMD_U32 media_player_audio_decresume_smtd(void);
//MMD_U32 MMD_Ply_Audio_DecSeek(MMD_U32 pos);
MMD_U32 media_player_audio_decseek_smtd(MMD_U32 addr,MMD_U32 len);//modify by yjz 2009/2/11
//MMD_U32 MMD_Ply_Audio_DecRelease(void);
MMD_U32 media_player_audio_decrelease_smtd(void);
//MMD_U32 MMD_Ply_Audio_BufInit(AUD_BUF_INFO* pInfo, AUD_BUF_ADDR* pAddr);
MMD_U32 media_player_audio_bufinit_smtd(AUD_BUF_INFO* pInfo, AUD_BUF_ADDR* pAddr);
//MMD_U32 MMD_Ply_Audio_BufRelease(void);
MMD_U32 media_player_audio_bufrelease_smtd(void);
//MMD_U32 MMD_Ply_Audio_DmaStart(void);
MMD_U32 media_player_audio_dmastart_smtd(void);
//MMD_U32 MMD_Ply_Audio_FillSrcBuf(MMD_U32 idx, MMD_U32 data_size, MMD_U32 cursor_read, MMD_U32 is_over);
MMD_U32 media_player_audio_fillsrcbuf_smtd(MMD_U32 idx, MMD_U32 data_size, MMD_U32 cursor_read, MMD_U32 is_over);
//MMD_U32 MMD_Ply_Audio_FillPcmBuf(void);
MMD_U32 media_player_audio_fillpcmbuf_smtd(void);
//MMD_U32 MMD_Ply_Video_BufInit(VID_BUF_INFO* pInfo);
MMD_U32 media_player_video_bufinit_smtd(VID_BUF_INFO* pInfo);
//MMD_U32 MMD_Ply_Video_BufRelease(void);
MMD_U32 media_player_video_bufrelease_smtd(void);
//MMD_U32 MMD_Ply_Video_FillSrcBuf(VID_BUF_DATA *pdata);
MMD_U32 media_player_video_fillsrcbuf_smtd(VID_BUF_DATA *pdata);
//MMD_U32 MMD_Ply_Video_DecStart(void);
MMD_U32 media_player_video_decstart_smtd(void);
//MMD_U32 MMD_Ply_AudioRecorder_Input(MMD_U32 data_address,MMD_U32 buffer_size);
MMD_U32 media_recorder_audio_input_smtd(MMD_U32 data_address,MMD_U32 buffer_size);
//MMD_U32 MMD_Ply_AudioRecorder_Stop(void);
MMD_U32 media_recorder_audiorecorder_stop_smtd(void);
//MMD_U32 MMD_Ply_AudioRecorder_SetParam(MMD_U32 smplrate,MMD_U32 inputchn);
MMD_U32 media_recorder_audio_setparam_smtd(MMD_U32 smplrate,MMD_U32 inputchn);
//MMD_U32 MMD_Ply_MP4_MP3(MMD_U32 mp4flag);
MMD_U32 media_player_mp4_mp3_smtd(MMD_U32 mp4flag);
void MMD_Set_SerPic_Count(MMD_U32 uCount);
MMD_U32 MMD_Get_SerPic_Count(void);
void MMD_Release_Tmp_Pic(void);
MMD_U32 MMD_Write_TmpToFile(char * chFilename,int iTmpIndex);
MMD_PLYFILE  Player_Get_PlyHandle(void);

void MMD_Set_HpSpkType(void);

void MMD_Set_HeadphoneType(void);
void MMD_Set_SpeakerType(void);

#endif  /* _MMD_PLY_H_ */


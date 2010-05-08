/*======================================================================
*                                                                       
*                Copyright (C) 2007 Vimicro CO.,LTD     		 
* File Name: 	mmd_ply_driver.c
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/05/14
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	have 51 interface API for user .
*	MMD_U32  MMD_Media_System_Init(void);
*	MMD_U32  MMD_Media_System_Quit(void);
*	
*	MMD_U32  MMD_Media_Set_D2HCallback(D2HCallback Callback);
*	
*	MMD_U32  MMD_Player_File_Open(PLYCallback Callback, PMEDIA_FILE *plyfile, MMD_PLYFILE*pfile);
*	MMD_U32  MMD_Player_File_Close(PLYCallback Callback, MMD_PLYFILE pfile);
*	MMD_U32  MMD_Player_File_Play(MMD_PLYFILE pfile);
*	MMD_U32  MMD_Player_File_FastPlay(MMD_PLYFILE pfile);
*	MMD_U32  MMD_Player_File_Stop(MMD_PLYFILE pfile);
*	MMD_U32  MMD_Player_File_Pause(MMD_PLYFILE pfile);
*	MMD_U32  MMD_Player_File_Resume(MMD_PLYFILE pfile);
*	MMD_U32  MMD_Player_File_Seek(PLYCallback Callback, MMD_PLYFILE pfile, MMD_U32 pos, MMD_U32 *accesspos);
*	
*	MMD_U32  MMD_Player_File_GetProperty(MMD_PLYFILE pfile, FILE_PROPERTY*Property);
*	MMD_U32  MMD_Player_File_GetTotalTime(MMD_PLYFILE pfile, MMD_U32*uTotalTime);
*	MMD_U32  MMD_Player_File_GetPlayTime(MMD_PLYFILE pfile, MMD_U32*uPlayTime);
*	
*	MMD_U32 MMD_Player_File_SetAudioPostProcessParam(MMD_PLYFILE pfile, PROCESS_PARAM*parameter);
*	MMD_U32 MMD_Player_File_EnableAudioPostProcess(MMD_PLYFILE pfile, char*pType);
*	MMD_U32 MMD_Player_File_DisableAudioPostProcess(MMD_PLYFILE pfile, char*pType);
*	MMD_U32 MMD_Player_File_SetVideoPostProcessParam(MMD_PLYFILE pfile, PROCESS_PARAM*parameter);
*	MMD_U32 MMD_Player_File_EnableVideoPostProcess(MMD_PLYFILE pfile, char*pType);
*	MMD_U32 MMD_Player_File_DisableVideoPostProcess(MMD_PLYFILE pfile, char*pType);
*	
*	MMD_U32  MMD_Player_SetAudioOutChannel(MMD_U32 uChannel);
*	MMD_U32  MMD_Player_SetAudioOutVolume(MMD_U32 uLeftVolume, MMD_U32 uRightVolume);
*	MMD_U32  MMD_Player_SetSpeakerVolume(MMD_U32 uVolume);
*	MMD_U32  MMD_Player_SetHeadphoneVolume(MMD_U32 uVolume);
*	
*	MMD_U32  MMD_Player_SetVideoDisplayPos(MMD_U32 xStartPos, MMD_U32 yStartPos);
*	MMD_U32  MMD_Player_SetVideoDisplaySize(MMD_U32 uWidth, MMD_U32 uHeight);
*	MMD_U32  MMD_Player_SetBLayerDisplayPos(MMD_U32 xStartPos, MMD_U32 yStartPos);
*	MMD_U32  MMD_Player_SetBLayerDisplaySize(MMD_U32 uWidth, MMD_U32 uHeight);
*	
*	MMD_U32  MMD_Recorder_File_Open(RECCallback Callback, RMEDIA_FILE *recfile, MMD_RECFILE *rfile);
*	MMD_U32  MMD_Recorder_File_Close(RECCallback Callback, MMD_RECFILE rfile);
*	MMD_U32  MMD_Recorder_File_Start(MMD_RECFILE rfile);
*	MMD_U32  MMD_Recorder_File_Stop(MMD_RECFILE rfile);
*	MMD_U32  MMD_Recorder_File_Pause(MMD_RECFILE rfile);
*	MMD_U32  MMD_Recorder_File_Resume(MMD_RECFILE rfile);
*	
*	MMD_U32  MMD_Recorder_File_Setproperty(MMD_RECFILE rfile,  FILE_PROPERTY*property);
*	MMD_U32  MMD_Recorder_File_SetTotalTime(MMD_RECFILE pfile, MMD_U32 uTotalTime);
*	MMD_U32  MMD_Recorder_File_GetRecordTime(MMD_RECFILE pfile, MMD_U32*uRecordTime);
*	
*	MMD_U32  MMD_Recorder_File_SetAudioPostProcessParam(MMD_RECFILE pfile, PROCESS_PARAM*parameter);
*	MMD_U32  MMD_Recorder_File_EnableAudioPostProcess(MMD_RECFILE pfile, char*pType);
*	MMD_U32  MMD_Recorder_File_DisableAudioPostProcess(MMD_RECFILE pfile, char*pType);
*	MMD_U32  MMD_Recorder_File_SetVideoPostProcessParam(MMD_RECFILE pfile, PROCESS_PARAM*parameter);
*	MMD_U32  MMD_Recorder_File_EnableVideoPostProcess(MMD_RECFILE pfile, char*pType);
*	MMD_U32  MMD_Recorder_File_DisableVideoPostProcess(MMD_RECFILE pfile, char*pType);
*	
*	MMD_U32  MMD_Recorder_SetAudioInputChannel(MMD_U32 uChannel);
*	MMD_U32  MMD_Recorder_SetAudioInputVolume(MMD_U32 uLeftVolume, MMD_U32 uRightVolume);
*	MMD_U32  MMD_Recorder_SetLineinVolume(MMD_U32 uVolume);
*	MMD_U32  MMD_Recorder_SetMicrophoneVolume(MMD_U32 uVolume);
*	
*	MMD_U32  MMD_Recorder_SetVideoPreviewPos(MMD_U32 xStartPos, MMD_U32 yStartPos);
*	MMD_U32  MMD_Recorder_SetVideoPreviewSize(MMD_U32 uWidth, MMD_U32 uHeight);
*	MMD_U32  MMD_Recorder_SetBLayerDisplayPos(MMD_U32 xStartPos, MMD_U32 yStartPos);
*	MMD_U32  MMD_Recorder_SetBLayerDisplaySize(MMD_U32 uWidth, MMD_U32 uHeight);
*	
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#include "mmp_rdk.h"
#include	"../../apps/media/media_player.h"
#include "SENSOR_Info.h"  //add by xinggang.xu 20090106
#include "AEESTDLIB.h"

#define MMD_PLY_DEBUG_INFO

#ifdef MMD_PLY_DEBUG_INFO
	#define PRINTF(str)        {MMDPrintf("\nMEDIA_MESSAGE:	");MMDPrintf str;} 
	#define PRINTFE(str)      {MMDPrintf("\nMEDIA_ERROR:	");MMDPrintf str;} 
#else
	#define PRINTF(str)
	#define PRINTFE(str)
#endif

typedef volatile MMD_U32		MED_V32;
typedef volatile MMD_CHAR 	MED_V08;

/*define media file command status*/
typedef enum _MEDIA_CMD_STATUS
{
	COMMAND_DATAACK_RECEIVED=0,
    	COMMAND_DATAACK_WAIT=1,
      	COMMAND_DATAACK_TIMEOUT=2,
    	COMMAND_DATAACK_ERROR=0x7FFFFFFF
}MEDIA_CMD_STATUS;

/* media have data ack command control stuctor */
typedef struct _MEDIA_CMD_CONTROL_  MEDIA_CMD_CONTROL;
struct _MEDIA_CMD_CONTROL_
{
    	MED_V32		cmdstatus; 
	MMD_U32 	ackcmd;
	MMD_U32 	ackop1;
    	MMD_U32 	ackop2;	
	MMD_U32	callback;
};

/*define media file parameter for openning file, after open file need this parameter to host */
typedef struct _MEDIA_PLYFILE_INFOR_  MEDIA_PLYFILE_INFOR;
struct _MEDIA_PLYFILE_INFOR_				
{
	MMD_U32		ply_fileindex;		/*opened player file index*/
	MMD_U32		ply_filehandle;		/*source media file's handle*/
	MMD_U32		ply_filetype;			/*source media file's type*/
	MMD_U32		ply_filekind; 		/*source media file's kind, 0: audio file, 1:video file*/
	MMD_U32		v_imagewidth;		/* video image width, 0: have no video */
	MMD_U32		v_imageheight;		/* video image height, 0: have no video */
	MMD_U32		v_fps;				/* frame number per second(if changeless frame size, else =0) */
	MMD_U32		v_bitrate;			/* video data bit rate */
	MMD_U32		v_totaltime;			/* video total play time (ms)*/
	MMD_U32		v_totalframe;		/* video total frame*/
	MMD_U32		a_chn;				/* 0: have no audio, 1: mono, 2: stereo */
	MMD_U32		a_smplrate;			/* sample rate */
	MMD_U32		a_bits;				/* audio samplerate bit length */
	MMD_U32		a_bitrate;			/* audio data bit rate */
	MMD_U32		a_totaltime;			/* audio total play time (ms)*/
	MMD_U32		a_totalframe;		/* audio total frame*/
};

/*define video output inforamtion property for getting /setting property*/
typedef struct _MMD_MEDIA_PREVIEW_INFOR_   MMD_MEDIA_PREVIEW_INFOR;
struct _MMD_MEDIA_PREVIEW_INFOR_				
{
	MMD_U32		preview_dispwidth;		
	MMD_U32		preview_dispheight;			
	MMD_U32		preview_dispxpos;			
	MMD_U32		preview_dispypos;
	MMD_U32		preview_effect;
	MMD_U32		capture_width;
	MMD_U32		capture_height;	
	MMD_U32		thumbnail_width;
	MMD_U32		thumbnail_height;	
	MMD_U32		capture_quality;	
	MMD_U32		recvideo_width;
	MMD_U32		recvideo_height;	
	MMD_U32		recvideo_quality;
	MMD_U32		preview_frameon;
};

typedef struct _MMD_MEDIA_SYSTEM_INFOR_  MMD_MEDIA_SYSTEM_INFOR;
struct _MMD_MEDIA_SYSTEM_INFOR_{ 
	MMD_U32 		work_mode;
	MMD_U32		preview_dispwidth;		
	MMD_U32		preview_dispheight;			
	MMD_U32		preview_dispxpos;			
	MMD_U32		preview_dispypos;
	MMD_U32		preview_effect;
	MMD_U32		capture_width;
	MMD_U32		capture_height;	
	MMD_U32		thumbnail_width;
	MMD_U32		thumbnail_height;	
	MMD_U32		capture_quality;	
	MMD_U32		recvideo_width;
	MMD_U32		recvideo_height;	
	MMD_U32		recvideo_quality;
	MMD_U32		preview_frameon;
};

static MMD_MEDIA_SYSTEM_INFOR meSystemStatus;
//static D2HCallback meD2HCallback=NULL;
volatile MMD_U32 g_filehandle, g_playtime;

/*--------------------------------------------
	Macro Define of postproc CMD
-------------------------------------------*/
unsigned int g_vol_ctl_enable = 1;	//	For volume Adjust Test
unsigned int g_vol_ctl_gain = 0;
static MMD_PLYFILE g_PlyHandle;

extern int audio_codec_latch(void);
extern MMD_U32 g_codec_play ;
extern MMD_U32 g_codec_rec ;
#define ENABLE_POSTPROC	0x8000
#define POSTPROC_VOLCTL	0x0001

#define ADJUST_VOL_B2C	0x9000
#define ADJUST_VOL_C2B	0x9001	
/*-------------------------------------------*/


/*******************************************************************************************
Description:
	media player mode initalize(include object register)
Parameters:
Return:	
	MMD_RET_NOERR: initialize ok
	Others:			error code	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32  MMD_Media_System_Init(void)
{
	
	PRINTF(("[PLY] MMD_PLY_Init\r\n"));
    
    //ERR("MMD_Media_System_Init", 0, 0, 0 );
	// set system mode 
	g_system_mode|=MODE_PLY;
	
	// set media system default parameter
	meSystemStatus.work_mode = MMD_MEDIA_MODE_IDLE;
	meSystemStatus.capture_width = MMD_MEDIA_DEFAULT_CAPTURE_WIDTH;
	meSystemStatus.capture_height = MMD_MEDIA_DEFAULT_CAPTURE_HEIGHT;
	meSystemStatus.capture_quality = MMD_MEDIA_CAPTURE_QUALITY_GOOD;
	meSystemStatus.recvideo_width = MMD_MEDIA_DEFAULT_RECVIDEO_WIDTH;
	meSystemStatus.recvideo_height = MMD_MEDIA_DEFAULT_RECVIDEO_HEIGHT;
	meSystemStatus.recvideo_quality = MMD_MEDIA_VIDEO_QUALITY_GOOD;

	meSystemStatus.preview_effect = MMD_MEDIA_SPECIAL_NORMAL;
	//callback process function register
#if 0	
   	MMD_AutoCmdCB_Register((MAJ_PLY>>8),MMD_Media_Callback);  
#endif
    	//MMD_D2H_CmdCB_Register((MAJ_D2H_PLY>>8),MMD_Media_D2H_Callback);	
	

	return MMD_RET_NOERR;
}

/*******************************************************************************************
Description:
	media player mode exit(include object release)
Parameters:
Return:	
	MMD_RET_NOERR: relese ok
	Others:			error code	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32  MMD_Media_System_Quit(void)
{
	
    	PRINTF(("[PLY] MMD_PLY_Quit\r\n"));
   	// check system mode 
    	if(!(g_system_mode&MODE_PLY))
    	{	
    		//system have already out media play mode, need not release again!
             	return MMD_RET_NOERR;		
   	}
    	// set media system default parameter
	meSystemStatus.work_mode = MMD_MEDIA_MODE_IDLE;
	meSystemStatus.capture_width = MMD_MEDIA_DEFAULT_CAPTURE_WIDTH;
	meSystemStatus.capture_height = MMD_MEDIA_DEFAULT_CAPTURE_HEIGHT;
	meSystemStatus.capture_quality = MMD_MEDIA_CAPTURE_QUALITY_GOOD;
	meSystemStatus.recvideo_width = MMD_MEDIA_DEFAULT_RECVIDEO_WIDTH;
	meSystemStatus.recvideo_height = MMD_MEDIA_DEFAULT_RECVIDEO_HEIGHT;
	meSystemStatus.recvideo_quality = MMD_MEDIA_VIDEO_QUALITY_GOOD;

	g_system_mode&=~MODE_PLY;
    	//MMD_AutoCmdCB_UnRegister(MAJ_PLY>>8);
    	//MMD_D2H_CmdCB_UnRegister(MAJ_D2H_PLY>>8);

	return MMD_RET_NOERR;
}

/*******************************************************************************************
Description:
	media work mode initalize(include hardware power on)
Parameters:
Return:	
	MMD_RET_NOERR: initialize ok
	Others:			error code	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2008.08.15		create first version
********************************************************************************************/
MMD_U32  MMD_Media_Mode_Initialzie(MMD_U32 work_mode)
{
	//CMD_ACK_FORMAT	ack;
	//CMD_FORMAT		cmd;
	//MMD_U32   ret;
    //ERR("MMD_Media_Mode_Initialzie ",0, 0, 0 );
	// check system mode 
	if(!(g_system_mode&MODE_PLY))
    	{
		PRINTFE(("Media player mode error\r\n"));
		return MMD_RET_MODEERR;
    	}
	switch(work_mode)
	{
		case MMD_MEDIA_MODE_PLAY_AUDIO_ONLY:  	
			PRINTF(("MMD_MEDIA_MODE_PLAY_AUDIO_ONLY " ));
			meSystemStatus.work_mode|=MMD_MEDIA_MODE_PLAY_AUDIO_ONLY;
			break;
		case MMD_MEDIA_MODE_PLAY_VIDEO_ONLY:  	
			PRINTF(("MMD_MEDIA_MODE_PLAY_VIDEO_ONLY " ));
			meSystemStatus.work_mode|=MMD_MEDIA_MODE_PLAY_VIDEO_ONLY;
			break;
		case MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO:  	
			PRINTF(("MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO " ));
			meSystemStatus.work_mode|=MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO;
			break;
		case MMD_MEDIA_MODE_RECORD_AUDIO_ONLY:  	
			PRINTF(("MMD_MEDIA_MODE_RECORD_AUDIO_ONLY " ));
			meSystemStatus.work_mode|=MMD_MEDIA_MODE_RECORD_AUDIO_ONLY;
			break;
		case MMD_MEDIA_MODE_RECORD_VIDEO_ONLY:  	
			PRINTF(("MMD_MEDIA_MODE_RECORD_VIDEO_ONLY " ));
			meSystemStatus.work_mode|=MMD_MEDIA_MODE_RECORD_VIDEO_ONLY;
			break;
		case MMD_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO:  	
			PRINTF(("MMD_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO " ));
			meSystemStatus.work_mode|=MMD_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO;
			break;
		case MMD_MEDIA_MODE_CAPTURE:  	
			PRINTF(("MMD_MEDIA_MODE_CAPTURE " ));
			meSystemStatus.work_mode|=MMD_MEDIA_MODE_CAPTURE;
			break;
		case MMD_MEDIA_MODE_VIDEOCONFERENCE:  	
			PRINTF(("MMD_MEDIA_MODE_VIDEOCONFERENCE " ));
			meSystemStatus.work_mode|=MMD_MEDIA_MODE_VIDEOCONFERENCE;
			break;
    		default:
    			PRINTF(("MMD_MEDIA_MODE_ERROR " ));
    			break;
	}
	media_work_mode_initialize(work_mode);
	return 0;
   	//send command and received command ack
    	/*cmd.cmd = MAJ_PLY|MIN_MEDIA_WORKMODE_INITIALIZE;
	cmd.op1 = work_mode;
	cmd.op2 = 0;
	ret =MMD_Media_NoDataAckCommand(&cmd, &ack);
	if(ret)
   		return ret;
   	//op1 is return value, if error then return
	return ack.op1;*/
}

/*******************************************************************************************
Description:
	media work mode exit(include hardware power off)
Parameters:
Return:	
	MMD_RET_NOERR: exit ok
	Others:			error code	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2008.08.15		create first version
********************************************************************************************/
MMD_U32  MMD_Media_Mode_Exit(MMD_U32 work_mode)
{
	//CMD_ACK_FORMAT ack;
	//CMD_FORMAT		 cmd;
	//MMD_U32 		 ret;
    	
      	//check if in media system mode, if not then can not do any media operation
    	if(!(g_system_mode&MODE_PLY))
    	{
        	PRINTFE(("Media player mode error\r\n"));
        	return MMD_RET_MODEERR;
    	}
		
	switch(work_mode)
	{
		case MMD_MEDIA_MODE_PLAY_AUDIO_ONLY:  	
			PRINTF(("MMD_MEDIA_MODE_PLAY_AUDIO_ONLY " ));
			meSystemStatus.work_mode&=~MMD_MEDIA_MODE_PLAY_AUDIO_ONLY;
			break;
		case MMD_MEDIA_MODE_PLAY_VIDEO_ONLY:  	
			PRINTF(("MMD_MEDIA_MODE_PLAY_VIDEO_ONLY " ));
			meSystemStatus.work_mode&=~MMD_MEDIA_MODE_PLAY_VIDEO_ONLY;
			break;
		case MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO:  	
			PRINTF(("MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO " ));
			meSystemStatus.work_mode&=~MMD_MEDIA_MODE_PLAY_AUDIO_AND_VIDEO;
			break;
		case MMD_MEDIA_MODE_RECORD_AUDIO_ONLY:  	
			PRINTF(("MMD_MEDIA_MODE_RECORD_AUDIO_ONLY " ));
			meSystemStatus.work_mode&=~MMD_MEDIA_MODE_RECORD_AUDIO_ONLY;
			break;
		case MMD_MEDIA_MODE_RECORD_VIDEO_ONLY:  	
			PRINTF(("MMD_MEDIA_MODE_RECORD_VIDEO_ONLY " ));
			meSystemStatus.work_mode&=~MMD_MEDIA_MODE_RECORD_VIDEO_ONLY;
			break;
		case MMD_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO:  	
			PRINTF(("MMD_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO " ));
			meSystemStatus.work_mode&=~MMD_MEDIA_MODE_RECORD_AUDIO_AND_VIDEO;
			break;
		case MMD_MEDIA_MODE_CAPTURE:  	
			PRINTF(("MMD_MEDIA_MODE_CAPTURE " ));
			meSystemStatus.work_mode&=~MMD_MEDIA_MODE_CAPTURE;
			break;
		case MMD_MEDIA_MODE_VIDEOCONFERENCE:  	
			PRINTF(("MMD_MEDIA_MODE_VIDEOCONFERENCE " ));
			meSystemStatus.work_mode&=~MMD_MEDIA_MODE_VIDEOCONFERENCE;
			break;
    		default:
    			PRINTF(("MMD_MEDIA_MODE_ERROR " ));
    			break;
	}
	media_work_mode_exit(work_mode);
	return 0;

}


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
				structor and macro define for video confige [only in host system] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
MMD_U32  MMD_Media_Set_Effect(MMD_U32 effect)
{
    FARF(MSG,("MMD_Media_Set_Effect=%d",effect));
	media_set_effect(effect);
	return MMD_RET_NOERR;
}

//add by xinggang.xu 20090106 start
MMD_U32  MMD_Media_Set_Brightness(UINT8 brightness_val)
{
   //MSG_FATAL("MMD_Media_Set_Brightness=%d", brightness_val, 0, 0 );
   FARF(MSG,("MMD_Media_Set_Brightness=%d",brightness_val));
   SensorSetBrightness_OV2650_YUV(brightness_val);   
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Set_Environment(UINT8 environment_val)
{
   //MSG_FATAL("MMD_Media_Set_Environment=%d", environment_val, 0, 0 );
   FARF(MSG,("MMD_Media_Set_Environment=%d",environment_val));
   SensorSetEnvironment_OV2650_YUV(environment_val);
   return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Set_Banding(UINT8 banding_val)
{
   //MSG_FATAL("MMD_Media_Set_Banding=%d", banding_val, 0, 0 );
   FARF(MSG,("MMD_Media_Set_Banding=%d",banding_val));
   SensorSetBanding_OV2650_YUV(banding_val);
   return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Set_Preview_Parameter(MMD_PREVIEW_OUTINFOR* preview_param)
{
	meSystemStatus.preview_dispwidth = preview_param->display_width;
	meSystemStatus.preview_dispheight = preview_param->display_height;
	meSystemStatus.preview_dispxpos = preview_param->display_xpos;
	meSystemStatus.preview_dispypos = preview_param->display_ypos;
	meSystemStatus.preview_effect = preview_param->display_effect;
   media_set_preview_parameter((MMD_U32)preview_param);
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Set_Capture_FrameMode(MMD_U32 mode)
{
    FARF(MSG,("MMD_Media_Set_Capture_FrameMode->preview_frameon:%d",mode));
	meSystemStatus.preview_frameon = mode;
	return MMD_RET_NOERR;
}
MMD_U32  MMD_Media_Get_Capture_FrameMode(void) //zhangxiang add 2009.03.09
{
    FARF(MSG,("MMD_Media_Get_Capture_FrameMode:%d"));
	return meSystemStatus.preview_frameon;
}

MMD_U32 capture_width=0;  //zhangxiang add for 320x240 capture fail
MMD_U32  MMD_Media_Set_Capture_Resolution(MMD_U32 width, MMD_U32 height)
{
	meSystemStatus.capture_width = width;
	meSystemStatus.capture_height = height;
	media_set_capture_resolution(width,height);
    capture_width=width;  //zhangxiang add for 320x240 capture fail
	return MMD_RET_NOERR;
}
MMD_U32  MMD_Media_Set_Thumbnail_Resolution(MMD_U32 width, MMD_U32 height)
{
	meSystemStatus.thumbnail_width= width;
	meSystemStatus.thumbnail_height= height;
	media_set_thumbnail_resolution(width,height);
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Change_Capture_Resolution(MMD_U32 width, MMD_U32 height)
{
	if((meSystemStatus.capture_width != width)||(meSystemStatus.capture_height!=height ))
	{
		meSystemStatus.capture_width = width;
		meSystemStatus.capture_height = height; 
		media_change_capture_resolution(width,height);		
	}
	
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Set_Video_Resolution(MMD_U32 width, MMD_U32 height)
{
	meSystemStatus.recvideo_width = width;
	meSystemStatus.recvideo_height = height;
	meSystemStatus.capture_width =0;
	meSystemStatus.capture_height = 0;
	meSystemStatus.capture_quality = 0;
	meSystemStatus.recvideo_quality = 0xa;
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Set_Capture_Quality(MMD_U32 quality_class)
{
	meSystemStatus.capture_quality = quality_class;
	meSystemStatus.recvideo_width = 0;
	meSystemStatus.recvideo_height = 0;
	meSystemStatus.recvideo_quality = 0;
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Set_Video_Quality(MMD_U32 quality_class)
{
	meSystemStatus.recvideo_quality = quality_class;
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Preview_On(void)
{
	MMD_MEDIA_PREVIEW_INFOR previewinfor;
	//CMD_ACK_FORMAT ack;
	//CMD_FORMAT		 cmd;
	MMD_U32 ret=0;
	MMD_U08 * buf_ptr;
	MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
  	//check if in media system mode, if not then can not do any media operation
    //MSG_FATAL("MMD_Media_Preview_On", 0, 0, 0);
    FARF(MSG,("MMD_Media_Preview_On"));
	 if(!(g_system_mode&MODE_PLY))
    	{
		PRINTFE(("Media player mode error\r\n"));
		return MMD_RET_MODEERR;
    	}

	MMD_SYS_SnrPowerOn();//zhangxiang del
	//set preview information
	previewinfor.preview_dispwidth	=meSystemStatus.preview_dispwidth;		
	previewinfor.preview_dispheight	=meSystemStatus.preview_dispheight;			
	previewinfor.preview_dispxpos	=meSystemStatus.preview_dispxpos;			
	previewinfor.preview_dispypos	=meSystemStatus.preview_dispypos;
	previewinfor.preview_effect=meSystemStatus.preview_effect;
	previewinfor.capture_width	=meSystemStatus.capture_width;
	previewinfor.capture_height	=meSystemStatus.capture_height;	
	previewinfor.thumbnail_width	=meSystemStatus.thumbnail_width;
	previewinfor.thumbnail_height	=meSystemStatus.thumbnail_height;
	previewinfor.capture_quality	=meSystemStatus.capture_quality;	
	previewinfor.recvideo_width	=meSystemStatus.recvideo_width;
	previewinfor.recvideo_height	=meSystemStatus.recvideo_height;	
	previewinfor.recvideo_quality=meSystemStatus.recvideo_quality;
	previewinfor.preview_frameon=meSystemStatus.preview_frameon;
	//write paramter
#if 0	
  	ret = MMD_LIF_WriteParam((MMD_S08 *)&previewinfor,sizeof(MMD_MEDIA_PREVIEW_INFOR),&paramaddr);
    	if(ret != MMD_RET_NOERR)
    	{
		PRINTF(("WriteParam error\r\n"));
		return ret;
   	 }
	//send command
	cmd.cmd = MAJ_PLY|CMD_MSK_PARA|MIN_MEDIA_PREVIEW_ON;
	cmd.op1 = paramaddr;
	cmd.op2 = sizeof(MMD_MEDIA_PREVIEW_INFOR);
    	ret =MMD_Media_NoDataAckCommand(&cmd, &ack);
	if(ret)
   		return ret;
   	//received command ack ok, check return value
	return ack.op1;
#endif


	ret=media_preview_on((MMD_U32)&previewinfor);
	return ret;

}

MMD_U32  MMD_Media_Preview_Off(void)
{
	//CMD_ACK_FORMAT ack;
	//CMD_FORMAT		 cmd;
	//MMD_U32 		 ret;
    //ERR("MMD_Media_Preview_Off", 0, 0, 0 );
    //MSG_FATAL("MMD_Media_Preview_Off", 0, 0, 0);
    FARF(MSG,("MMD_Media_Preview_Off"));

      	//check if in media system mode, if not then can not do any media operation
#if 0
    	if(!(g_system_mode&MODE_PLY))
    	{
        	PRINTFE(("Media player mode error\r\n"));
        	return MMD_RET_MODEERR;
    	}
    	//send command
    	cmd.cmd = MAJ_PLY|MIN_MEDIA_PREVIEW_OFF;
	cmd.op1 = 0;
	cmd.op2 = 0;
    	ret =MMD_Media_NoDataAckCommand(&cmd, &ack);
	if(ret)
   		return ret;
   	//op1 is return value, if error then return
	return ack.op1;
#endif
	media_preview_off();
	MMD_SYS_SetModeClk(CLK_CPU12M_BUS12M);
	return 0;
}

MMD_U32  MMD_Media_Preview_ZoomIn(void)
{
	MMD_U32 ret;
	ret=media_preview_zoomin();
	return ret;
}

MMD_U32  MMD_Media_Preview_ZoomOut(void)
{
	MMD_U32 ret;
	ret=media_preview_zoomout();
	return ret;
}
MMD_U32  MMD_MediaEngine_Init()
{
	return media_engine_initialize();
}

MMD_U32  MMD_MediaEngine_Release()
{
	return media_engine_release();
}
	
extern PLYCallback g_plyCallback;
MMD_U32  MMD_Player_File_SetCallback(PLYCallback Callback)
{
	g_plyCallback = Callback;
	return 0;
}
/*******************************************************************************************
Description:
	media player file open
Parameters:
	PLYCallback Callback:	user callback for the media file
	PMEDIA_FILE *plyfile:	media file information (include file handle, file length, file type, file store type)
	MMD_PLYFILE*pfile:	media player file structer point
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32  MMD_Player_File_Open(PMEDIA_FILE *plyfile, MMD_PLYFILE*pfile)
{
	MMD_U32  ret;//paramaddr, paramlen, ret;	
	 //MSG_FATAL("MMD_Player_File_Open", 0, 0, 0 );
	 FARF(MSG,("MMD_Player_File_Open"));
	*pfile = 0;	
	ret = media_player_file_open((MMD_U32)plyfile, pfile);
	g_PlyHandle = *pfile;
	PRINTF(("media_player_file_open=0x%x\n, filehandle=0x%x\n",ret,(unsigned int)pfile));
	return ret;
	
}

/*******************************************************************************************
Description:
	media player file close
Parameters:
	MMD_PLYFILE*pfile:	media player file structer point
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32  MMD_Player_File_Close(MMD_PLYFILE pfile, MMD_U32* volctl_state, MMD_U32* adjust_gain)
{
	MMD_U32 ret;
    //MSG_FATAL("MMD_Player_File_Close", 0, 0, 0 );
    FARF(MSG,("MMD_Player_File_Close"));
	ret =media_player_file_close((MMD_U32)pfile);
	if((g_vol_ctl_enable == 2) && (*volctl_state == VOL_ADJUST_START))
	{
		*volctl_state = VOL_ADJUST_OVER;
		*adjust_gain = g_vol_ctl_gain;
		me_printf("MMD_Player_File_Close, Set volctl_state=0x%x, adjust_gain=0x%x\n", *volctl_state, *adjust_gain);		
	}
	g_PlyHandle = 0;
	MMD_SYS_SetModeClk(CLK_CPU12M_BUS12M);
	return ret;
}

/*******************************************************************************************
Description:
	media player file start play
Parameters:
	MMD_PLYFILE*pfile:	media player file structer point
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32  MMD_Player_File_Play(MMD_PLYFILE pfile, PLYCallback Callback)
{
	MMD_U32 ret;
    //MSG_FATAL("MMD_Player_File_Play", 0, 0, 0 );
    FARF(MSG,("MMD_Player_File_Play"));

	ret =media_player_file_play((MMD_U32)pfile, (MMD_U32)Callback);
	return ret;
}

/*******************************************************************************************
Description:
	media player file stop play
Parameters:
	MMD_PLYFILE*pfile:	media player file structer point
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32  MMD_Player_File_Stop(MMD_PLYFILE pfile)
{
	MMD_U32 		 ret;

	ret =media_player_file_stop((MMD_U32)pfile);
	return ret;
}

/*******************************************************************************************
Description:
	media player file pause 
Parameters:
	MMD_PLYFILE*pfile:	media player file structer point
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32  MMD_Player_File_Pause(MMD_PLYFILE pfile)
{
	MMD_U32 		 ret;
    //MSG_FATAL("MMD_Player_File_Pause", 0, 0, 0 );
    FARF(MSG,("MMD_Player_File_Pause"));
	ret =media_player_file_pause((MMD_U32)pfile);
	return ret;
}

/*******************************************************************************************
Description:
	media player file resume playing
Parameters:
	MMD_PLYFILE*pfile:	media player file structer point
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32  MMD_Player_File_Resume(MMD_PLYFILE pfile)
{
	MMD_U32 		 ret;
    //MSG_FATAL("MMD_Player_File_Resume", 0, 0, 0 );
    FARF(MSG,("MMD_Player_File_Resume"));
	ret =media_player_file_resume((MMD_U32)pfile);
	return ret;
}

/*******************************************************************************************
Description:
	media player file seek
Parameters:
	MMD_PLYFILE*pfile:	media player file structer point
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32 MMD_Player_File_Seek(MMD_PLYFILE pfile, MMD_U32 pos, MMD_U32 *accesspos)
{
	MMD_U32 ret;

	ret =media_player_file_seek((MMD_U32)pfile, pos, accesspos);
	return ret;
}

/*******************************************************************************************
Description:
	get media player file property(include audio infor, video ifnor and other ID3 infor)
Parameters:
	MMD_PLYFILE*pfile:	media player file structer point
	FILE_PROPERTY*property:
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32 MMD_Player_File_GetProperty(MMD_PLYFILE pfile, FILE_PROPERTY*property)
{
	MMD_U32 		 ret;

	//intialize property size
	property->size = 0;
	ret = media_player_file_getproperty((VIM_MEDIA_PLYFILE)pfile, (VIM_MEDIA_FILE_PROPERTY *)property);
	return ret;
}


/*******************************************************************************************
Description:
	get media player file property(include audio infor, video ifnor and other ID3 infor)
Parameters:
	MMD_PLYFILE*pfile:	media player file structer point
	FILE_PROPERTY*property:
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32 MMD_Player_File_SetProperty(MMD_PLYFILE pfile, FILE_PROPERTY*property)
{
	MMD_U32 		 ret;

	//write paramter(include )
	ret = media_player_file_setproperty((VIM_MEDIA_PLYFILE)pfile, (VIM_MEDIA_FILE_PROPERTY *)property);
  
	return ret;
   	
}


/*******************************************************************************************
Description:
	media player file get total play time
Parameters:
	MMD_PLYFILE*pfile:	media player file structer point
	MMD_U32* uTotalTime: 	media file playing total time(MS)
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32 MMD_Player_File_GetTotalTime(MMD_PLYFILE pfile, MMD_U32*uTotalTime)
{
	MMD_U32 		 ret;

	ret = media_player_file_gettotaltime((MMD_U32)pfile, uTotalTime);
	return ret;
}

/*******************************************************************************************
Description:
	media player file get current playing time
Parameters:
	MMD_PLYFILE*pfile:	media player file structer point
	MMD_U32* uPlayTime: 		 media file playing current time(MS)
Return:	
	MMD_RET_NOERR: ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.05.14		create first version
********************************************************************************************/
MMD_U32 MMD_Player_File_GetPlayTime(MMD_PLYFILE pfile, MMD_U32*uPlayTime)
{
	MMD_U32 ret;

	ret = media_player_file_getplaytime((MMD_U32)pfile, uPlayTime);
	return ret;
}

/*******************************************************************************************
Description:
	media recorder file open
Parameters:
	RECCallback Callback:	user callback for the media file
	PMEDIA_FILE *plyfile:	recorde file infor structor point
	MMD_PLYFILE*pfile:	recorder file handle point's point
Return:	
	MMD_RET_NOERR: open player file ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.02		create first version
********************************************************************************************/
MMD_U32  MMD_Recorder_File_Creat(RECCallback Callback, RMEDIA_FILE *recfile, MMD_RECFILE *rfile)
{
	MMD_U32 ret;

	*rfile = 0;
    FARF(MSG,("MMD_Recorder_File_Creat"));
    
	// have audio record need set codec samplerate
	ret = media_recorder_file_open((MMD_U32)recfile, rfile,(MMD_U32)Callback);
	PRINTF(("media_recorder_file_open=0x%x\n, filehandle=0x%x",ret,(unsigned int)rfile));
	return ret;

}

/*******************************************************************************************
Description:
	media recorder file close 
Parameters:
	RECORDER_FILE* pthis: recorder file handle
Return:	
	MMD_RET_NOERR: close ok
	Others:			error code
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.02		create first version
********************************************************************************************/
MMD_U32  MMD_Recorder_File_Close(RECCallback Callback,MMD_RECFILE rfile)
{
	MMD_U32 ret;
    //MSG_FATAL("MMD_Recorder_File_Close", 0, 0, 0 );
    VIM_MEDIA_RECORDER_FILE *pfilehandle;
     FARF(MSG,("MMD_Recorder_File_Close"));
    pfilehandle=(VIM_MEDIA_RECORDER_FILE *)rfile;
    if(((pfilehandle->mr_filehandle->fs_filetype[0]=='a')&&(pfilehandle->mr_filehandle->fs_filetype[1]=='m')&&
       (pfilehandle->mr_filehandle->fs_filetype[2]=='r')) ||((pfilehandle->mr_filehandle->fs_filetype[0]=='m')&&(pfilehandle->mr_filehandle->fs_filetype[1]=='p')&&
       (pfilehandle->mr_filehandle->fs_filetype[2]=='3')))
       MMD_SYS_SetModeClk(CLK_CPU12M_BUS12M);
    ret =media_recorder_file_close((MMD_U32)rfile);
	return ret;
}

/*******************************************************************************************
Description:
	media recorder file start record
Parameters:
	MMD_RECFILE* pthis: recorder file handle
Return:	
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.09.02		create first version
********************************************************************************************/
MMD_U32  MMD_Recorder_File_Start(MMD_RECFILE rfile, PLYCallback Callback)
{
	//CMD_ACK_FORMAT ack;
	MMD_U32 		 ret;
    //MSG_FATAL("MMD_Recorder_File_Start", 0, 0, 0 );
    FARF(MSG,("MMD_Recorder_File_Start"));
	ret=media_recorder_file_start(rfile,(MMD_U32)Callback);

   	return ret;
   	//op1 is return value, if error then return
	//return ack.op1;
}
MMD_U32  MMD_Recorder_File_Stop(MMD_RECFILE rfile)
{
   	MMD_U32 ret=0;
    FARF(MSG,("MMD_Recorder_File_Stop"));
	ret = media_recorder_file_stop((MMD_U32) rfile);
   	return ret;
}

// Output headphone and speaker at the same time
void MMD_Set_HpSpkType(void)
{
 FARF(MSG,("MMD_Set_HpSpkType"));
 audio_codec.codec_set_playmode(CODEC_STEROECHANNEL,CODEC_SPEAKER_HP_OUT);
}
void MMD_Set_HeadphoneType(void)
{
 FARF(MSG,("MMD_Set_HeadphoneType"));
 audio_codec.codec_set_playmode(CODEC_STEROECHANNEL,CODEC_HEADPHONEOUT);
 
}
void MMD_Set_SpeakerType(void)
{
 FARF(MSG,("MMD_Set_SpeakerType"));
 audio_codec.codec_set_playmode(CODEC_STEROECHANNEL,CODEC_SPEAKEROUT);
}


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
				macro and API function define for audio confige [only in host system] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
MMD_U32  MMD_Media_Switch_AudioMode(MMD_U32 mode)
{
	VIM_Audio_PlayModeIf_Cfg(mode);
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Set_AudioMode(MMD_U32 mode)
{	
	media_sysstatus->audio_mode= mode;
	return MMD_RET_NOERR;
}

void MMD_Media_SetRecodeChannel(MMD_U32 channel)
{	gAudio_RecChSel = channel;
}
//------- temporarily --------//
void MMD_Media_PlayMixEnable(void)
{	
	unsigned int regValue;
	HAL_READ_UINT32(V5_REG_AUD_APPL,regValue);
	regValue |= 0x3000;	//I2S接口时，打开硬件MIX功能(MUX3_L=1, MUX3_R=1).		
	HAL_WRITE_UINT32(V5_REG_AUD_APPL,regValue);
}
void MMD_Media_PlayMixDisable(void)
{	
	unsigned int regValue;
	HAL_READ_UINT32(V5_REG_AUD_APPL,regValue);
	regValue &= ~0x3000;	//I2S接口时，关闭硬件MIX功能(MUX3_L=1, MUX3_R=1).		
	HAL_WRITE_UINT32(V5_REG_AUD_APPL,regValue);
}
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
				API function define for codec confige [only in host system] 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
MMD_U32  MMD_Media_Codec_Initialize(void)
{	
	audio_codec.codec_initialize();
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Codec_Exit(void)
{	
	audio_codec.codec_exit();
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Codec_Set_Play_Mode(MMD_U32 nCahnnel, MMD_U32 nOutType)
{
	/*
	media_sysstatus->codec_outinfor.a_outbits = 16;
	if(nCahnnel==VIM_MEDIA_STEROECHANNEL)
		media_sysstatus->codec_outinfor.a_outchn = 2;
	else
		media_sysstatus->codec_outinfor.a_outchn = 1;
	*/	
	audio_codec.codec_set_playmode(nCahnnel, nOutType);
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Codec_Stop_Play_Mode(void)
{
	audio_codec.codec_stop_playmode();
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Codec_Set_Play_Volume(MMD_U32 vLeft, MMD_U32 vRight)
{
    //MSG_FATAL("MMD_Media_Codec_Set_Play_Volume=%d,%d", vLeft, vRight, 0 );
    FARF(MSG,("MMD_Media_Codec_Set_Play_Volume=%d,%d",vLeft,vRight));
	audio_codec.codec_DAC_volume(vLeft, vRight);
	return MMD_RET_NOERR;
}
MMD_U32 MMD_Media_Codec_Get_Play_Volume(MMD_U32 *pLeftVol,MMD_U32 *pRightVol)
{//by hbp
	*pLeftVol = audio_codec.Codec_PlyLChVol;
	*pRightVol = audio_codec.Codec_PlyRChVol;
	return MMD_RET_NOERR;
}
MMD_U32  MMD_Media_Codec_Set_Record_Mode(MMD_U32 nCahnnel, MMD_U32 nInputType)
{

	media_sysstatus->codec_inputinfor.a_inputbits = 16;
	if(nCahnnel==MMD_MEDIA_STEROECHANNEL)
		media_sysstatus->codec_inputinfor.a_inputchn = 2;
	else
		media_sysstatus->codec_inputinfor.a_inputchn = 1;
	
	audio_codec.codec_set_recordmode(nCahnnel, nInputType);
	return MMD_RET_NOERR;
}

MMD_U32  MMD_Media_Codec_Stop_Record_Mode(void)
{
	audio_codec.codec_stop_recordmode();
	return MMD_RET_NOERR;
}
MMD_U32  MMD_Media_Codec_Set_Record_Volume(MMD_U32 vLeft, MMD_U32 vRight)
{
	audio_codec.codec_ADC_volume(vLeft, vRight);
	return MMD_RET_NOERR;
}

MMD_U32 MMD_Media_Codec_Get_Rec_Volume(MMD_U32 *pLeftVol,MMD_U32 *pRightVol)
{//by hbp
	*pLeftVol = audio_codec.Codec_RecLChVol;
	*pRightVol = audio_codec.Codec_RecRChVol;
	return MMD_RET_NOERR;
}
//mode:0,record 1,play ; muteFlag:o,unmute, 1,mute
MMD_U32 MMD_Media_Codec_Set_Mute(MMD_U32 mode,MMD_U32 muteFlag)
{
	if(muteFlag)
	{	audio_codec.codec_enable_mute(mode);
	}
	else
	{	audio_codec.codec_disable_mute(mode);
	}
	return MMD_RET_NOERR;		
}

//===============================================//
MMD_U32 MMD_Media_AudioCodec_FM_Mode(MMD_U32 sampleRate, MMD_U32 MixFlag)
{//by hbp
	VIM_Audio_FM_Cfg(sampleRate,MixFlag);
	return MMD_RET_NOERR;
}
MMD_U32 MMD_Media_Codec_Set_MuxMode(MMD_U32 mode)
{//FM时使用,内部Codec Bypass.
	audio_codec.codec_set_lineinoutmode();
	return MMD_RET_NOERR;
}
void MMD_Media_Codec_Recording_MixEnable(void)
{//录音时使能数字混音器(RX0(LeftChannel)和RX1(RightChannel)混合),
	UINT32 regValue;
	HAL_READ_UINT32(V5_REG_AUD_APPL,regValue);
	regValue &= 0xFFFFF0F0;	//Clear bits: MUX2_R,MUX2_L,SRC_RX1R,SRC_RX1L,SRC_RX0R,SRC_RX0L
	regValue |= 0x06|(5<<8);
	HAL_WRITE_UINT32(V5_REG_AUD_APPL,regValue);
}
void MMD_Media_Codec_Recording_PlayEnable(void)
{	//UINT32 regValue;

	SET_V8_REG(V5_REG_AUD_APPL,AU_BIT(14));
	HAL_WRITE_UINT32(V5_REG_AUD_WS_SRC,0x2);
	PLAY_EN();
}
void MMD_Media_Codec_Recording_PlayDisable(void)
{	//UINT32 regValue;

	CLR_V8_REG(V5_REG_AUD_APPL,AU_BIT(14));
	HAL_WRITE_UINT32(V5_REG_AUD_WS_SRC,0x4);
	PLAY_DIS();
}
//add by yjz 2009/2/10
/*
	acc enable/disable interface
	mode: 1: enable,0:disable
*/
void MMD_Media_AAC_Play_Mode(MMD_U32 mode)
{
	BIU_INC();
	PRINTF(("MMD_Media_AAC_Play_Mode,mode = %d\n",mode));
	//printf("yjz-----MMD_Media_AAC_Play_Mode,mode = %d\n",mode);
	if(mode == 0)
	{
		media_sysstatus->work_mode |= VIM_MEDIA_MODE_AAC_DISABLE;
	}
	else
	{
		media_sysstatus->work_mode &= ~VIM_MEDIA_MODE_AAC_DISABLE;
	}
	BIU_DEC();
	return;
}
///:~
//add by yjz 2009/2/12
/*
	get id3 info
*/
#if 1
MMD_U32 MMD_Player_File_Get_Mp3_ID3property(MMD_PLYFILE pfile, FILE_PROPERTY*property)
{
	MMD_U32 		 ret;
	BIU_INC();
	//intialize property size
	property->size = 0;
	ret = media_player_file_get_mp3_id3property((VIM_MEDIA_PLYFILE)pfile, (VIM_MEDIA_FILE_PROPERTY *)property);
	BIU_DEC();
	return ret;	
}
#endif
//add by yjz 2009/3/9
/*
	switch play mode between I2S and PCM
*/
MMD_U32 MMD_Media_Switch_Play_Mode(MMD_U32 pfilehandle,MMD_U32 samplrate,MMD_U32 mode)
{
	MP_U32	seekpos, asscessframe, result;
	MP_U32 dma_count=0;
	VIM_MEDIA_PLAYER_FILE* pfile;
	MP_U32 uPlayTime;
	MP_U32*accesspos;
	MP_FILE *pthis = NULL;
	BIU_INC();
	pfile = (VIM_MEDIA_PLAYER_FILE*)pfilehandle;
	result = media_player_check_filehandle(pfile);
	if(result)	
	{
		BIU_DEC();
		return result;
	}
	pthis = pfile->mp_filehandle;
	media_player_file_getplaytime(pfilehandle, &uPlayTime);	
	//before swith must stop decode and read data
	if(pfile->f_playstatus==MEDIA_FILE_PLAYING) 
	{
		media_player_timer_stop();	
	}
	if(pfile->a_status==MEDIA_AUDIO_PLAYING)
	{//stop audio output and decode
		media_player_audio_stop(pfile);
		pfile->a_status = MEDIA_AUDIO_STOP;
	}
	result  = media_player_audio_decrelease_smtd();
	if(result) {
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			ME_ERROR(("media_player_audio_decrelease_smtd() return = 0x%08X", result));
			BIU_DEC();
			return result;
	}
	//*********************************************
	//seek audio time position by parser
	result = pthis->parser->seek(pthis->parserinfor, 
							  uPlayTime, &seekpos, &asscessframe);
	if(result)
	{
		BIU_DEC();
		return result;
	}
	pfile->v_frames = asscessframe;
	*accesspos = pfile->f_playtime = seekpos;
	//seek audio decoder
	if(pthis->audio_infor.a_isvalid)
	{
		pfile->a_time = pfile->f_playtime;	
		pfile->a_duration	= 0;			
		dma_count = samplrate*4*(pfile->a_time/1000);		
		if(pfile->a_time%1000)
		{
			dma_count+=samplrate*4*(pfile->a_time%1000)/1000;	
		}
		MMD_BIU_Set_Device_Status(IDX_AUD_DMA_COUNT, dma_count);		
	}
	pthis->audio_outinfor.a_outsmplrate  = samplrate;
	//printf("\nyjz ----in samplrate  =%d\n",pthis->audio_outinfor.a_outsmplrate);
	result = media_player_audio_paramset_smtd(&(pthis->audio_infor), &(pthis->audio_outinfor), (pthis->audio_infor).a_otherslen);
	if(result) {
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			ME_ERROR(("media_player_audio_decinit_smtd() return = 0x%08X", result));
			BIU_DEC();
			return result;
	}
	media_player_audio_paraminit_smtd(samplrate,mode);
	MMD_Media_Set_AudioMode(mode);
	pfile->f_playcontrol|= MP_AUDIO_PLAY_END;
	if(pthis->audio_infor.a_isvalid)
	{
		pfile->f_playcontrol &= ~ MP_AUDIO_PLAY_END;
		pfile->a_dmasamples = 0;
		pfile->a_lastleftsmpls = 0;
		media_player_audio_buffer_filled(pfile);
	}
	if(pfile->f_playstatus==MEDIA_FILE_PAUSE)
	{//before swith, file status is pause 		
		if(pthis->audio_infor.a_isvalid)
		{ 	
			pfile->a_status=MEDIA_AUDIO_PAUSE;
		}
	}
	else if(pfile->f_playstatus==MEDIA_FILE_PLAYING)
	{//before swith, file status is playing		
		if(pthis->audio_infor.a_isvalid)
		{
			pfile->a_status=MEDIA_AUDIO_PLAYING;
			media_player_audio_start(pfile);
			if(!(pthis->video_infor.v_isvalid)) 
			{
				media_player_timer_start();
			}
		}
	}
	else if(pfile->f_playstatus==MEDIA_FILE_FAST_PLAYING)
	{//before swith, file status is fastplaying
		pfile->f_playcontrol|= MP_AUDIO_PLAY_END;
		if(pthis->video_infor.v_isvalid)//?
		{
			pfile->v_status=MEDIA_VIDEO_PLAYING;
			media_player_video_start(pfile);
			media_player_timer_start();
		}
	}
	BIU_DEC();
	return ME_SUCCESS;	
}
//===============================================//
//------------------------Added for 848-----------------------------
extern volatile MMD_U32 g_Ack_MinCmd;
extern volatile MMD_U32 g_Ack_OP1;
extern volatile MMD_U32 g_Ack_OP2;

/*************************************************
  Function:       
  	Mode_Sys_Lisr
  Description:    
 	sys mode command process call back function, called in LISR
  Input:
  	pMsg:	Host to device cmd msg
  Output:        
	NULL 	
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		06/12/31     	create this function  
*************************************************/
MMD_U32 MMD_Mode_Ply_Lisr(MMD_U32 cmd)
{
	MMD_MSG Msg;
	MMD_U08 min;

	// get command and parameter 
	MMD_BIU_Get_D2H_Ack(&Msg);
	MMD_ASSERT(Msg.CMD==cmd);
	
	min = Msg.CMD & CMD_MASK_MIN;
	
	g_Ack_MinCmd=min;
	g_Ack_OP1=Msg.OP1;
	g_Ack_OP2=Msg.OP2;

	switch(min)
	{
	default:
		break;
	}

	return MMD_RET_NOERR;
}

MMD_U32 MMD_Mode_Rec_Lisr(MMD_U32 cmd)
{
	MMD_MSG Msg;
	MMD_U08 min;

	// get command and parameter 
	MMD_BIU_Get_D2H_Ack(&Msg);
	MMD_ASSERT(Msg.CMD==cmd);
	
	min = Msg.CMD & CMD_MASK_MIN;
	
	g_Ack_MinCmd=min;
	g_Ack_OP1=Msg.OP1;
	g_Ack_OP2=Msg.OP2;

	switch(min)
	{
	default:
		break;
	}

	return MMD_RET_NOERR;
}

/*************************************************
  Function:       
  	MMD_Ply_Audio_BufInit
  Description:    
 	Audio buffer initial function, VC0848 alloc audio stream buffer
 	and PCM buffer
  Input:
	AUD_BUF_INFO* pInfo: 		buffer information
  		SRC_SIZE:			audio stream buffer size, which should be get from parser
		SRC_NUM:			audio stream buffer number, max number is 8
		PCM_SIZE:			audio PCM buffer size, 
								0: 		use the size get from decoder in VC0848
								other: 	specified by baseband code
		PCM_NUM:			audio PCM buffer number, max number is 8
  Output:        
	AUD_BUF_ADDR* pAddr:	Allocated buffer address information
		SRC_ADDR[8]:		Src buffer address
		PCM_ADDR[8]:		PCM buffer address
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_BufInit(AUD_BUF_INFO* pInfo, AUD_BUF_ADDR* pAddr)
MMD_U32 media_player_audio_bufinit_smtd(AUD_BUF_INFO* pInfo, AUD_BUF_ADDR* pAddr)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000,AudBufInfoAddr;
	
	AudBufInfoAddr=MMD_SYS_Malloc(sizeof(AUD_BUF_INFO));
	if(AudBufInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(AudBufInfoAddr, (MMD_S08*)pInfo, sizeof(AUD_BUF_INFO));

	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_BUF_INIT;
	Msg.OP1=AudBufInfoAddr;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
		{
			break;
		}
		timeout--;
	}

	if(timeout)
	{
		Vm_RdSram(g_Ack_OP1, (MMD_S08*)pAddr, sizeof(AUD_BUF_ADDR));
		MMD_SYS_Free(g_Ack_OP1);
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

/*************************************************
  Function:       
  	MMD_Ply_Audio_BufInit
  Description:    
 	Audio buffer initial function, VC0848 alloc audio stream buffer
 	and PCM buffer
  Input:
	AUD_BUF_INFO* pInfo: 		buffer information
  		SRC_SIZE:			audio stream buffer size, which should be get from parser
		SRC_NUM:			audio stream buffer number, max number is 8
		PCM_SIZE:			audio PCM buffer size, 
								0: 		use the size get from decoder in VC0848
								other: 	specified by baseband code
		PCM_NUM:			audio PCM buffer number, max number is 8
  Output:        
	AUD_BUF_ADDR* pAddr:	Allocated buffer address information
		SRC_ADDR[8]:		Src buffer address
		PCM_ADDR[8]:		PCM buffer address
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_BufInit(AUD_BUF_INFO* pInfo, AUD_BUF_ADDR* pAddr)
MMD_U32 media_player_audio_codecinit_smtd(AUD_BUF_INFO* pInfo)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000,AudBufInfoAddr;
	
	AudBufInfoAddr=MMD_SYS_Malloc(sizeof(AUD_BUF_INFO));
	if(AudBufInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(AudBufInfoAddr, (MMD_S08*)pInfo, sizeof(AUD_BUF_INFO));

	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_CODEC_INIT;
	Msg.OP1=AudBufInfoAddr;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
		{
			break;
		}
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

/*************************************************
  Function:       
  	MMD_Ply_Audio_BufRelease
  Description:    
 	Audio buffer release function, VC0848 release audio stream buffer
 	and PCM buffer
  Input:
  	NULL
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_BufRelease(void)
MMD_U32 media_player_audio_bufrelease_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_BUF_RELEASE;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

/*************************************************
  Function:       
	media_player_audio_decinit_smtd
  Description:    
 	Audio decoder initia function, it will call decoder->init
  Input:
  	void* pAudioInfo:	 		me_audio_infor pointer
  	void* pAudioOutInfo:		me_audio_outinfor	pointer
  	MMD_U32 a_others_size:	me_audio_infor->a_others information size 
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_DecInit(void* pAudioInfo,	void* pAudioOutInfo, MMD_U32 a_others_size)
MMD_U32 media_player_audio_decinit_smtd(void* pAudioInfo,	void* pAudioOutInfo, MMD_U32 a_others_size)
{
	MMD_MSG Msg;
	MMD_U32 timeout=2000, AudioInfoAddr, AudioOutInfoAddr;
	me_audio_infor* pInfo;

	pInfo = (me_audio_infor*)pAudioInfo;
	// allocate VC0848 meory, released in VC0848
	AudioInfoAddr=MMD_SYS_Malloc(sizeof(me_audio_infor)+a_others_size);
	if(AudioInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(AudioInfoAddr, (MMD_S08*)pInfo, sizeof(me_audio_infor));
	if(pInfo->a_others!=NULL)
	{ 
		Vm_WrSram(AudioInfoAddr+sizeof(me_audio_infor), (MMD_S08*)pInfo->a_others, a_others_size);
	}
	AudioOutInfoAddr=MMD_SYS_Malloc(sizeof(me_audio_outinfor));
	if(AudioOutInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(AudioOutInfoAddr, pAudioOutInfo, sizeof(me_audio_outinfor));
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_DECODER_INIT;
	Msg.OP1=AudioInfoAddr;
	Msg.OP2=AudioOutInfoAddr;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

/*************************************************
  Function:       
	media_player_audio_paraminit_smtd
  Description:    
 	Audio decoder initia function, it will call decoder->init
  Input:
  	void* pAudioInfo:	 		me_audio_infor pointer
  	void* pAudioOutInfo:		me_audio_outinfor	pointer
  	MMD_U32 a_others_size:	me_audio_infor->a_others information size 
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_ParamInit(MMD_U32 samplerate, MMD_U32 audiomode)
MMD_U32 media_player_audio_paraminit_smtd(MMD_U32 samplerate, MMD_U32 audiomode)
{
	audio_output_param_t	output_param;
	//-----------//
    	output_param.msMode     	= MASTER;
    	output_param.wdLen      	= B24;		//Embeded audio codec only support 24-bit word length
   	output_param.msbFst     	= MSBFST;
	output_param.fifoDepth		= 0x1F;		//TxFIFO Actual Depth =31+1
    	output_param.mono       	= A_STEREO;	//设置AudioMemory的数据为双声道数据.
    	output_param.pcmDataFmt 	= PCM16;	//设置TxFIFO的数据为16bit
    	output_param.wsWidth    	= FS64;		//for internal codec,zhang xuecheng,2007-12-21 16:49:57
    	output_param.edgemode	= EDGE0;
	output_param.fstWs		= SECSCLK;
	output_param.I2SMode		= audiomode;
	output_param.samRate		= samplerate;
	VIM_Audio_PlayMode_Cfg(output_param);
	return MMD_RET_NOERR;
/*
	MMD_MSG Msg;	
	MMD_U32 timeout=1000;	

	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_PARAM_INIT;
	Msg.OP1= samplerate;
	Msg.OP2= audiomode;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		if(audiomode==0)
        	{
        		VIM_HIF_SetReg32(V5_REG_AUD_CODEC_DIG_IF, 0x10a);//0x82
                     audio_codec_latch();
			//VIM_HIF_SetReg32(V5_REG_CKD_SCLK_CON, 0x1d001d);//SCLK 2M ,zhoujian modify
        	}
        	else
        	{
        		VIM_HIF_SetReg32(V5_REG_AUD_CODEC_DIG_IF, 0x82);//0x82
                      audio_codec_latch();
        	}
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}
*/
}
/*************************************************
  Function:       
	media_player_audio_paramset_smtd
  Description:    
 	Audio decoder param set function
  Input:
  	void* pAudioInfo:	 		me_audio_infor pointer
  	void* pAudioOutInfo:		me_audio_outinfor	pointer
  	MMD_U32 a_others_size:	me_audio_infor->a_others information size 
  	void* pAudioOutInfo:		me_audio_outinfor	pointer
  Return:         
  	Error code 
  History:        
      <author>  	<time>     		<desc>
      yjz    		09/03/10     		create this function       
*************************************************/
MMD_U32 media_player_audio_paramset_smtd(void* pAudioInfo,	void* pAudioOutInfo, MMD_U32 a_others_size)
{
	MMD_MSG Msg;
	MMD_U32 timeout=2000,  AudioInfoAddr, AudioOutInfoAddr;
	me_audio_infor* pInfo;
	
	pInfo = (me_audio_infor*)pAudioInfo;
	// allocate VC0848 meory, released in VC0848
	AudioInfoAddr=MMD_SYS_Malloc(sizeof(me_audio_infor)+a_others_size);
	if(AudioInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(AudioInfoAddr, (MMD_S08*)pInfo, sizeof(me_audio_infor));
	if(pInfo->a_others!=NULL)
	{ 
		Vm_WrSram(AudioInfoAddr+sizeof(me_audio_infor), (MMD_S08*)pInfo->a_others, a_others_size);
	}
	AudioOutInfoAddr=MMD_SYS_Malloc(sizeof(me_audio_outinfor));
	if(AudioOutInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(AudioOutInfoAddr, pAudioOutInfo, sizeof(me_audio_outinfor));
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_DECODER_SET_OUT_PARA;
	Msg.OP1=AudioInfoAddr;
	Msg.OP2=AudioOutInfoAddr;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_Ply_MP4_MP3(MMD_U32 mp4flag)
MMD_U32 media_player_mp4_mp3_smtd(MMD_U32 mp4flag)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;


	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_MP4;
	Msg.OP1=mp4flag;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}
}

/*************************************************
  Function:       
	media_player_audio_decstart_smtd 
  Description:    
 	Audio decoder start function, it will call decoder->start
  Input:
	Null
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_DecStart(void)
MMD_U32 media_player_audio_decstart_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_DECODER_START;
	Msg.OP1=0;
	Msg.OP2=0;

//:	Add for Volume Adjustment
	if(g_vol_ctl_enable == 1)
	{
		me_printf("enable_state=1, Enable VolCtl postproc\n");
		Msg.OP1 |= ((MMD_U32)(ENABLE_POSTPROC) << 16) & 0xffff0000;	//	Enable postproc MSG
		Msg.OP1 |= POSTPROC_VOLCTL & 0x0000ffff;	//	VolCtl postproc
	}
	else if(g_vol_ctl_enable == 2)	//	Recv adjust msg from last playstop
	{
		if (g_vol_ctl_gain > 0)	//	Volume need adjust
		{
			me_printf("enable_state=2, g_vol_ctl_gain=%d\n", g_vol_ctl_gain);
			Msg.OP1 |= ((MMD_U32)(ADJUST_VOL_B2C) << 16) & 0xffff0000;	//	Send vol Adjust MSG
			Msg.OP1 |= g_vol_ctl_gain & 0x0000ffff;	//	vol adjust value	
		}

		//: Reset test flags
//		g_vol_ctl_gain = 0;
//		g_vol_ctl_enable = 1;
//		me_printf("Reset g_vol_ctl_gain & g_vol_ctl_enable\n");
	}

///:~	
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
		{
			break;
		}
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}


/*************************************************
  Function:       
  	media_player_audio_decrelease_smtd
  Description:    
 	Audio decoder release function, it will call decoder->release
  Input:
	Null
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_DecRelease(void)
MMD_U32 media_player_audio_decrelease_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_DECODER_RELEASE;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}


/*************************************************
  Function:       
  	media_player_audio_decstop_smtd
  Description:    
 	Audio decoder stop function, it will call decoder->stop
  Input:
	Null
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_DecStop(void)
MMD_U32 media_player_audio_decstop_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=4000;
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_DECODER_STOP;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		//if(g_Ack_MinCmd != MIN_PLY_AUDIO_DECODER_STOP)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
		{
			break;
		}
		timeout--;
	}
//:  Add for volume Adjustment	
	if(g_vol_ctl_enable == 1)
	{
		if(((g_Ack_OP2 >> 16) & 0x0000ffff) == ADJUST_VOL_C2B)	//	RECV adjust vol value
		{
			g_vol_ctl_enable = 2;	//	Set volctl flag
			g_vol_ctl_gain = g_Ack_OP2 & 0x0000ffff;	//	Store recv vol ctl gain value
			me_printf("RECV g_vol_ctl_gain=%d, g_vol_ctl_enable set to %d\n", g_vol_ctl_gain, g_vol_ctl_enable);
		}
	}
///:~
	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		me_printf("media_player_audio_decstop_smtd ACK timeout!\n");
		return MMD_RET_TIMEOUT;
	}	
}

/*************************************************
  Function:       
  	media_player_audio_decseek_smtd
  Description:    
 	Audio decoder seek function, it will call decoder->seek
  Input:
	Null
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_DecSeek(MMD_U32 pos)
#if 0
MMD_U32 media_player_audio_decseek_smtd(MMD_U32 pos)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_DECODER_SEEK;
	Msg.OP1= pos;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}
#else
MMD_U32 media_player_audio_decseek_smtd(MMD_U32 addr,MMD_U32 len)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000,SeekInfoDataAddr;
	#if 1
	if(addr !=0 && len!=0)
	{
		SeekInfoDataAddr=MMD_SYS_Malloc(len);
		if(SeekInfoDataAddr==NULL)
		{
			return MMD_RET_DEVICENOSPACE;
		}
		Vm_WrSram(SeekInfoDataAddr, (MMD_S08*)addr, len);	
		//printf("yjz----media_player_audio_decseek_smtd,addr  =%d,len = %d,SeekInfoDataAddr = %d\n",addr,len,SeekInfoDataAddr);
	}
	else
	{
		SeekInfoDataAddr = 0;
		//printf("yjz----media_player_audio_decseek_smtd,addr  =%d,len = %d\n",addr,len);
	}	
	#endif	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_DECODER_SEEK;
	Msg.OP1= SeekInfoDataAddr;	
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		MMD_SYS_Free(SeekInfoDataAddr);
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}
#endif
//MMD_U32 MMD_Ply_Audio_DecPause(void)
MMD_U32 media_player_audio_decpause_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_DECODER_PAUSE;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_Ply_Audio_DecResume(void)
MMD_U32 media_player_audio_decresume_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_DECODER_RESUME;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}



/*************************************************
  Function:       
  	media_player_audio_dmastart_smtd
  Description:    
 	Start audio DMA operation, send PCM data to audio codec
  Input:
	Null
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_DmaStart(void)
MMD_U32 media_player_audio_dmastart_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_START_DMA_OUT;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

/*************************************************
  Function:       
  	media_player_audio_fillsrcbuf
  Description:    
 	Start audio DMA operation, send PCM data to audio codec
  Input:
	Null
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_FillSrcBuf(MMD_U32 idx, MMD_U32 data_size, MMD_U32 cursor_read, MMD_U32 is_over)
MMD_U32 media_player_audio_fillsrcbuf_smtd(MMD_U32 idx, MMD_U32 data_size, MMD_U32 cursor_read, MMD_U32 is_over)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_FILL_SRC_BUF;
	Msg.OP1=idx|(cursor_read<<16);
	Msg.OP2=data_size | (is_over << 16);
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

/*************************************************
  Function:       
  	media_player_audio_fillpcmbuf_smtd
  Description:    
 	Start audio decoder, call decode frames to fill all PCM buffer
 	but not output to audio codec
  Input:
	Null
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_FillPcmBuf(void)
MMD_U32 media_player_audio_fillpcmbuf_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=2000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_AUDIO_FILL_PCM_BUF;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_Ply_Video_BufInit(VID_BUF_INFO* pInfo)
MMD_U32 media_player_video_bufinit_smtd(VID_BUF_INFO* pInfo)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000,VidBufInfoAddr;
	
	VidBufInfoAddr=MMD_SYS_Malloc(sizeof(VID_BUF_INFO));
	if(VidBufInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(VidBufInfoAddr, (MMD_S08*)pInfo, sizeof(VID_BUF_INFO));

	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_VIDEO_BUF_INIT;
	Msg.OP1=VidBufInfoAddr;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
		{
			break;
		}
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

MMD_U32 media_player_video_bufrelease_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_VIDEO_BUF_RELEASE;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_Ply_Video_FillSrcBuf(VID_BUF_DATA *pdata)
MMD_U32 media_player_video_fillsrcbuf_smtd(VID_BUF_DATA *pdata)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000,VidBufDataAddr;

	VidBufDataAddr=MMD_SYS_Malloc(sizeof(VID_BUF_DATA));
	if(VidBufDataAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(VidBufDataAddr, (MMD_S08*)pdata, sizeof(VID_BUF_DATA));
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_VIDEO_FILL_SRC_BUF;
	Msg.OP1=VidBufDataAddr;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}
//MMD_U32 MMD_Ply_Video_DecStart(void)
MMD_U32 media_player_video_decstart_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_PLY_VIDEO_DECODER_START;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

#if VIDEORECORDERIN848

//MMD_U32 MMD_Ply_VideoRecorder_StableBufInit(VID_STABLE_BUF_INFO* pInfo)
MMD_U32 media_recorder_video_stablebufinit_smtd(VID_STABLE_BUF_INFO* pInfo)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000,VidStableBufInfoAddr;
	
	VidStableBufInfoAddr=MMD_SYS_Malloc(sizeof(VID_STABLE_BUF_INFO));
	if(VidStableBufInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(VidStableBufInfoAddr, (MMD_S08*)pInfo, sizeof(VID_STABLE_BUF_INFO));

	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_PLY_VIDEO_RECORDE_STABLE_BUF_INIT;
	Msg.OP1=VidStableBufInfoAddr;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
		{
			break;
		}
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_Ply_VideoRecorder_StableBufRelease(void)
MMD_U32 media_recorder_video_stablebufrelease_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_PLY_VIDEO_RECORDE_STABLE_BUF_RELEASE;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}


//MMD_U32 MMD_Ply_AudioRecorder_Input(MMD_U32 data_address,MMD_U32 buffer_size)
MMD_U32 media_recorder_audio_input_smtd(MMD_U32 data_address,MMD_U32 buffer_size)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_MEDIA_REC_AUDIO_INPUT_DMA;
	Msg.OP1=data_address;
	Msg.OP2=buffer_size;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_Ply_AudioRecorder_SetParam(MMD_U32 smplrate,MMD_U32 inputchn)
MMD_U32 media_recorder_audio_setparam_smtd(MMD_U32 smplrate,MMD_U32 inputchn)
{	
	audio_input_param_t	input_param;
	//-----------//
	input_param.msMode  		= MASTER;
	input_param.align			= LEFT;
	input_param.wdLen       	= B24;	//Embeded audio codec only support 24-bit word length
	input_param.msbFst          	= MSBFST;
	input_param.fifoDepth		= 0x0F;	//RxFIFO Actual Depth =15+1
	input_param.pcmDataFmt 	= PCM16;
	input_param.wsWidth       	= FS64;
	input_param.fstWs               = SECSCLK;
	input_param.edgemode		= EDGE0;
	input_param.samRate		= smplrate;
	if(inputchn == 1)
	{	input_param.mono = A_MONO;		//设置AudioMemory的数据为单声道数据.
	}
	else
	{	input_param.mono = A_STEREO;		//设置AudioMemory的数据为双声道数据.
	}
	VIM_Audio_RecordMode_Cfg(input_param);
	return MMD_RET_NOERR;
	/*
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_MEDIA_REC_AUDIO_PREPROC_SETPARAM;
	Msg.OP1=smplrate;
	Msg.OP2=inputchn;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}
	*/
}
MMD_U32 media_recorder_audiorecorder_stop_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_MEDIA_REC_FILE_STOP;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_Ply_VideoRecorder_StartTimer(void)
MMD_U32 media_player_videorecorder_starttimer_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_PLY_VIDEO_RECORDE_STARTTIMER;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

#endif

//MMD_U32 MMD_Rec_Video_BufInit(VID_BUF_INFO* pInfo)
MMD_U32 media_recorder_video_bufinit_smtd(VID_BUF_INFO* pInfo)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000,VidBufInfoAddr;
	
	VidBufInfoAddr=MMD_SYS_Malloc(sizeof(VID_BUF_INFO));
	if(VidBufInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(VidBufInfoAddr, (MMD_S08*)pInfo, sizeof(VID_BUF_INFO));

	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_REC_VIDEO_BUF_INIT;
	Msg.OP1=VidBufInfoAddr;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
		{
			break;
		}
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_Rec_Video_BufRelease(void)
MMD_U32 media_recorder_video_bufrelease_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_REC_VIDEO_BUF_RELEASE;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_Rec_Video_FrameInfoInit(MMD_U32 * InfoAddr)
MMD_U32 media_recorder_video_frameinfoinit_smtd(MMD_U32 * InfoAddr)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_REC_VIDEO_FRAMEINFO_INIT;
	Msg.OP1=InfoAddr[0];
	Msg.OP2=InfoAddr[1];
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
		{
			break;
		}
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_Rec_Video_FrameInfoRelease(void)
MMD_U32 media_recorder_video_frameinforelease_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_REC_VIDEO_FRAMEINFO_RELEASE;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
		{
			break;
		}
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}


MMD_U32 audRecFile;
MMD_U32 fileHandle;

//MMD_U32 MMD_Set_Samplerate(MR_U32 samplerate)
MMD_U32 media_recorder_set_samplerate_smtd(MR_U32 samplerate)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;


	g_Ack_MinCmd = MIN_RESERVED;
	Msg.CMD=MAJ_REC|MIN_AUDIO_REC_SET_SPST;

	Msg.OP1=samplerate;
	Msg.OP2=0;
	me_printf("media_recorder_set_samplerate_smtd samplerate(%d)  \n",samplerate);
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}
	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

//MMD_U32 MMD_REC_StrInit(void *pThis)
MMD_U32 media_recorder_strinit_smtd(void *pThis)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	VIM_MEDIA_RECORDER_FILE * tempFile;
	MR_FILE* 	tmpmr_filehandle;	

	tempFile = (VIM_MEDIA_RECORDER_FILE *)pThis;
	tmpmr_filehandle = tempFile->mr_filehandle;
	audRecFile=MMD_SYS_Malloc(sizeof(VIM_MEDIA_RECORDER_FILE));
	if(audRecFile==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	fileHandle=MMD_SYS_Malloc(sizeof(MR_FILE));
	if(fileHandle==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(audRecFile, (MMD_S08*)pThis, sizeof(VIM_MEDIA_RECORDER_FILE));
	Vm_WrSram(fileHandle, (MMD_S08*)tmpmr_filehandle, sizeof(MR_FILE));
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_AUDIO_REC_STR_INIT;
	Msg.OP1=audRecFile;
	Msg.OP2=fileHandle;
	me_printf("media_recorder_strinit_smtd RECORDER_FILE sizeof(%d) audRecFile(0x%x) fileHandle(0x%x) a_smplrate(%d)\n",sizeof(VIM_MEDIA_RECORDER_FILE),audRecFile,fileHandle,tmpmr_filehandle->audio_infor.a_smplrate);
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}
	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}


//MMD_U32 MMD_REC_StrUpdata(void *pThis)
MMD_U32 media_recorder_strupdata_smtd(void *pThis)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	VIM_MEDIA_RECORDER_FILE * tempFile;

	
	//MMD_U32 audRecFileUp;
	//audRecFileUp=MMD_SYS_Malloc(sizeof(VIM_MEDIA_RECORDER_FILE));
	tempFile = (VIM_MEDIA_RECORDER_FILE *)pThis;
	Vm_WrSram(audRecFile, (MMD_S08*)pThis, sizeof(VIM_MEDIA_RECORDER_FILE));
	Vm_WrSram(fileHandle, (MMD_S08*)tempFile->mr_filehandle, sizeof(MR_FILE));
	
	me_printf("media_recorder_strupdata_smtd mr_filehandle(0x%p) audRecFile(0x%x) a_inputsmplrate(%d)\n",tempFile->mr_filehandle,audRecFile,tempFile->mr_filehandle->audio_inputinfor.a_inputsmplrate);
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_AUDIO_REC_STR_UPDATA;
	Msg.OP1=audRecFile;
	Msg.OP2=fileHandle;
	me_printf("MMD_REC_StrInit RECORDER_FILE (0x%p)",pThis);
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}
	if(timeout)
	{
		//MMD_SYS_Free(audRecFileUp);
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

/*************************************************
  Function:       
	media_recorder_audio_bufinit_smtd
  Description:    
 	Audio buffer initial function, VC0848 alloc audio stream buffer
 	and PCM buffer
  Input:
	AUD_BUF_INFO* pInfo: 		buffer information
  		SRC_SIZE:			audio stream buffer size, which should be get from parser
		SRC_NUM:			audio stream buffer number, max number is 8
		PCM_SIZE:			audio PCM buffer size, 
								0: 		use the size get from decoder in VC0848
								other: 	specified by baseband code
		PCM_NUM:			audio PCM buffer number, max number is 8
  Output:        
	AUD_BUF_ADDR* pAddr:	Allocated buffer address information
		SRC_ADDR[8]:		Src buffer address
		PCM_ADDR[8]:		PCM buffer address
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_REC_BufInit(AUD_BUF_INFO* pInfo, AUD_BUF_ADDR* pAddr)
MMD_U32 media_recorder_audio_bufinit_smtd(AUD_BUF_INFO* pInfo, AUD_BUF_ADDR* pAddr)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000,AudBufInfoAddr;
	
	AudBufInfoAddr=MMD_SYS_Malloc(sizeof(AUD_BUF_INFO));
	if(AudBufInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(AudBufInfoAddr, (MMD_S08*)pInfo, sizeof(AUD_BUF_INFO));

	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_PLY_AUDIO_REC_BUF_INIT;
	Msg.OP1=AudBufInfoAddr;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		Vm_RdSram(g_Ack_OP1, (MMD_S08*)pAddr, sizeof(AUD_BUF_ADDR));
		MMD_SYS_Free(g_Ack_OP1);
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}


/*************************************************
  Function:       
  	MMD_Ply_Audio_BufRelease
  Description:    
 	Audio buffer release function, VC0848 release audio stream buffer
 	and PCM buffer
  Input:
  	NULL
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Rec_Audio_BufRelease(void)
MMD_U32 media_recorder_audio_bufrelease_smtd(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_PLY_AUDIO_BUF_RELEASE;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}


//MMD_U32 MMD_REC_Audio_Start()
MMD_U32 media_recorder_audio_start_smtd()
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	g_Ack_MinCmd = MIN_RESERVED;

	
	Msg.CMD=MAJ_REC|MIN_REC_AUDIO_START;
	Msg.OP1=0;
	
	//Msg.OP2=AudioOutInfoAddr;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
	
}

/*MMD_REC_Audio_GetBuffSize(MR_U32 * BuffSize)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_PLY|MIN_REC_AUDIO_GET_BUFF_SIZE;
	Msg.OP1=(MR_U32 )BuffSize;
	//Msg.OP2=AudioOutInfoAddr;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}*/

/*************************************************
  Function:       
  	MMD_Ply_Audio_EncInit
  Description:    
 	Audio decoder initia function, it will call decoder->init
  Input:
  	void* pAudioInfo:	 		me_audio_infor pointer
  	void* pAudioOutInfo:		me_audio_outinfor	pointer
  	MMD_U32 a_others_size:	me_audio_infor->a_others information size 
  Return:         
  	Error code 
  History:        
      <author>  	<time>     	<desc>
      lizg    		08/05/07     	create this function  
*************************************************/
//MMD_U32 MMD_Ply_Audio_EncInit(void* pAudioInfo,	void* pAudioOutInfo, MMD_U32 a_others_size)
MMD_U32 media_recorder_audio_encinit_smtd(void* pAudioInfo,	void* pAudioOutInfo, MMD_U32 a_others_size)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000, AudioInfoAddr, AudioOutInfoAddr;
	me_audio_infor* pInfo;
	
	pInfo = (me_audio_infor*)pAudioInfo;
	// allocate VC0848 meory, released in VC0848
	AudioInfoAddr=MMD_SYS_Malloc(sizeof(me_audio_infor)+a_others_size);
	if(AudioInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(AudioInfoAddr, (MMD_S08*)pInfo, sizeof(me_audio_infor));
	if(pInfo->a_others!=NULL)
	{
		Vm_WrSram(AudioInfoAddr+sizeof(me_audio_infor), (MMD_S08*)pInfo->a_others, a_others_size);
	}
	AudioOutInfoAddr=MMD_SYS_Malloc(sizeof(me_audio_inputinfor));
	if(AudioOutInfoAddr==NULL)
	{
		return MMD_RET_DEVICENOSPACE;
	}
	Vm_WrSram(AudioOutInfoAddr, pAudioOutInfo, sizeof(me_audio_inputinfor));
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_PLY_AUDIO_ENCODER_INIT;
	Msg.OP1=AudioInfoAddr;
	Msg.OP2=AudioOutInfoAddr;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}



extern MMD_U32 uSerPicCount,uPhotoCount;

void MMD_Set_SerPic_Count(MMD_U32 uCount)
{
	if(uCount<2)
		uSerPicCount = 0;
	else
		uSerPicCount = uCount;
	uPhotoCount = 0;
	//printf("Take (%d) Photo!!!!!!!!!\n",uSerPicCount);
}

MMD_U32 MMD_Get_SerPic_Count()
{
	return uSerPicCount;
}

extern MEMORY_FILE		SerPicfilehandle[MAX_PIC_COUNT];
void MMD_Release_Tmp_Pic()
{
	int iCount=0;
	for(;iCount<MAX_PIC_COUNT;iCount++)
	{
		if(SerPicfilehandle[iCount].FileAddress !=NULL)
		{
			me_printf("Free NO. %d Mem.\n",iCount);
			mp_free(SerPicfilehandle[iCount].FileAddress);
			SerPicfilehandle[iCount].FileAddress = NULL;
			SerPicfilehandle[iCount].FileTotalLength = 0;
		}
	}
}

MMD_U32 MMD_Write_TmpToFile(char * chFilename,int iTmpIndex)
{
	return media_Write_TmpToFile(chFilename,iTmpIndex);
}

#if 0
MMD_U32 MMD_REC_Video_BufInit(VID_BUF_INFO* pInfo)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000,VidBufInfoAddr;
	
	VidBufInfoAddr=MMD_SYS_Malloc(sizeof(VID_BUF_INFO));
	Vm_WrSram(VidBufInfoAddr, (MMD_S08*)pInfo, sizeof(VID_BUF_INFO));

	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_REC_VIDEO_BUF_INIT;
	Msg.OP1=VidBufInfoAddr;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
		{
			MMD_Check_Irq_Staus_And_Wait(1);
		}
		else
		{
			break;
		}
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}

MMD_U32 MMD_REC_Video_BufRelease(void)
{
	MMD_MSG Msg;
	MMD_U32 timeout=1000;
	
	g_Ack_MinCmd = MIN_RESERVED;
	
	Msg.CMD=MAJ_REC|MIN_REC_VIDEO_BUF_RELEASE;
	Msg.OP1=0;
	Msg.OP2=0;
	MMD_BIU_Send_H2D_Cmd(&Msg);
	
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(timeout)
	{
		return MMD_RET_NOERR;
	}	
	else
	{
		return MMD_RET_TIMEOUT;
	}	
}
#endif
MMD_PLYFILE  Player_Get_PlyHandle(void)
{
	return g_PlyHandle;
}

